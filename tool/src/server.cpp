#include <ESP32WebServer.h>
#include "SPIFFS.h"
#include <map>
#include <string>

#include "server.hpp"
#include "wasm_env.hpp"
#include "task.hpp"
#include "utils.hpp"

#define TASK_BASE_FOLDER    "/tasks/"
#define PORT_NUMBER         80
#define MAX_NUM_FILES       30  // TODO: just for filesystem printing; delete after

ESP32WebServer server(PORT_NUMBER);

int numFiles = 0; // number of files in the system
File file; // TODO: if various requests are handled at the same time, can this cause a race condition?
FileUploadStatus fileUploadStatus = VALID;

void handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

void handleWasmFileUpload() {
    HTTPUpload& httpUpload = server.upload();
    switch(httpUpload.status) {
        case UPLOAD_FILE_START:
            fileUploadStatus = VALID;
            if(numFiles >= MAX_NUM_FILES) {
                fileUploadStatus = FAILED;
                server.send(
                    400, 
                    "application/json", 
                    createErrorMessage("max number of files reached")
                );
                return;
            }

            if (httpUpload.filename == "") {
                fileUploadStatus = FAILED;
                server.send(
                    400, 
                    "application/json", 
                    createErrorMessage("file not present in request")
                );
                return;
            }
            if (!httpUpload.filename.endsWith(".wasm")) {
                fileUploadStatus = FAILED;
                server.send(
                    400, 
                    "application/json", 
                    createErrorMessage("file is not a WASM file")
                );
                return;
            }

            Serial.println("Upload started...");
            file = SPIFFS.open(
                TASK_BASE_FOLDER + httpUpload.filename, 
                FILE_WRITE
            );
            if (!file) {
                fileUploadStatus = FAILED;
                server.send(
                    500, 
                    "application/json", 
                    createErrorMessage("error opening file")
                );
                return;
            }
            break;
        
        case UPLOAD_FILE_WRITE:
            if (fileUploadStatus == FAILED) return;

            Serial.println("Writing file...");
            if (!file || (file.write(httpUpload.buf, httpUpload.currentSize) <= 0)) {
                fileUploadStatus = FAILED;
                server.send(
                    500, 
                    "application/json", 
                    createErrorMessage("file write failed")
                );
                return;
            }
            break;

        case UPLOAD_FILE_END:
            if (fileUploadStatus == VALID) {
                Serial.println("Finished writing file!");
            }
            break;
        
        case UPLOAD_FILE_ABORTED:
            fileUploadStatus = FAILED;
            Serial.println("Upload aborted...");
            break;
        
        default:
            break;
    }
}
void handleUploadSuccess() {
    if (fileUploadStatus == VALID) {
        if (!file) {
            server.send(
                400, 
                "application/json", 
                createErrorMessage("file not present in request")
            );
            return;
        }

        numFiles++;

        // TODO: just for checking the file contents; remove after
        String filename = file.name();
        file.close();
        File xeg = SPIFFS.open(filename, "r");
        if (!xeg) {
            Serial.println("Failed to open WASM file for reading");
            return;
        }
        size_t app_wasm_len = xeg.size();
        uint8_t app_wasm[app_wasm_len];
        xeg.read(app_wasm, app_wasm_len);
        xeg.close();
        char buf[sizeof(app_wasm)*2+1]; /* one extra for \0 */
        if(to_hex(buf, sizeof(buf), app_wasm, sizeof(app_wasm)))
            Serial.println(buf);
        // ends here

        bool liveUpdate = false;
        unsigned int reservedStackSize = 0, reservedInitialMemory = 0, memoryLimit = 0;
        getFileUploadArguments(liveUpdate, reservedStackSize, reservedInitialMemory, memoryLimit);
        addTaskDetails(filename, reservedStackSize, reservedInitialMemory, memoryLimit);
        if (liveUpdate) {
            liveUpdateTask(filename);
        }
        server.send(
            200, 
            "application/json", 
            createSuccessMessage("file uploaded successfully")
        );
        return;
    }

    if (file) {
        String filename = file.name();
        file.close();
        SPIFFS.remove(filename);
    }
    fileUploadStatus = VALID;
}

void handleWasmFileEdit() {
    if(!server.hasArg("filename")) {
        server.send(
            400, 
            "application/json", 
            createErrorMessage("need to specify filename")
        );
        return;
    }
    String filename = TASK_BASE_FOLDER + server.arg("filename");

    if(!SPIFFS.exists(filename) || !doesTaskDetailsExist(filename)) {
        server.send(
            404, 
            "application/json", 
            createErrorMessage("file not found")
        );
        return;
    }

    bool liveUpdate = false;
    unsigned int reservedStackSize = 0, reservedInitialMemory = 0, memoryLimit = 0;
    getFileUploadArguments(liveUpdate, reservedStackSize, reservedInitialMemory, memoryLimit);
    addTaskDetails(filename, reservedStackSize, reservedInitialMemory, memoryLimit);
    if (liveUpdate) {
        liveUpdateTask(filename);
    }
    server.send(
        200, 
        "application/json", 
        createSuccessMessage("file details updated successfully")
    );
}

void getFileUploadArguments(bool& liveUpdate, unsigned int& reservedStackSize, unsigned int& reservedInitialMemory, unsigned int& memoryLimit) {
    liveUpdate = (
        server.hasArg("liveUpdate") 
        && 
        server.arg("liveUpdate") == "true"
    );
    reservedStackSize = server.hasArg("reservedStackSize") ?
        server.arg("reservedStackSize").toInt() : 0;
    reservedInitialMemory = server.hasArg("reservedInitialMemory") ?
        server.arg("reservedInitialMemory").toInt() : 0;
    memoryLimit = server.hasArg("memoryLimit") ?
        server.arg("memoryLimit").toInt() : 0;
}

void handleWasmTaskDelete() {
    if(!server.hasArg("filename")) {
        server.send(
            400, 
            "application/json", 
            createErrorMessage("need to specify filename")
        );
        return;
    }
    String filename = TASK_BASE_FOLDER + server.arg("filename");
    if(!SPIFFS.exists(filename) || !doesTaskDetailsExist(filename)) {
        server.send(
            404, 
            "application/json", 
            createErrorMessage("file not found")
        );
        return;
    }
    if(isTaskActive(filename)) {
        server.send(
            400, 
            "application/json", 
            createErrorMessage("task is currently running")
        );
        return;
    }
    deleteWasmFile(filename);
    server.send(
        200,
        "application/json", 
        createSuccessMessage("WASM file deleted successfully")
    );
}

void handleTaskStart() {
    if(!server.hasArg("filename")) {
        server.send(
            400, 
            "application/json", 
            createErrorMessage("need to specify filename")
        );
        return;
    }
    String filename = TASK_BASE_FOLDER + server.arg("filename");
    if(isTaskActive(filename)) {
        server.send(
            400, 
            "application/json", 
            createErrorMessage("already executing task")
        );
        return;
    }
    if(!SPIFFS.exists(filename) || !doesTaskDetailsExist(filename)) {
        server.send(
            404, 
            "application/json", 
            createErrorMessage("file not found")
        );
        return;
    }
    if (!taskStartIsPossible(filename)) {
        server.send(
            400, 
            "application/json", 
            createErrorMessage("device resources are not enough to execute task")
        );
        return;
    }
    createWasmTask(filename);
    server.send(
        200, 
        "application/json", 
        createSuccessMessage("task started successfully")
    );
}

void handleTaskStop() {
    if(!server.hasArg("filename")) {
        server.send(
            400, 
            "application/json", 
            createErrorMessage("need to specify filename")
        );
        return;
    }
    String filename = TASK_BASE_FOLDER + server.arg("filename");
    if(!isTaskActive(filename)) {
        server.send(
            400, 
            "application/json", 
            createErrorMessage("task is not being executed")
        );
        return;
    }
    removeWasmTask(filename);
    server.send(
        200,
        "application/json", 
        createSuccessMessage("task stopped successfully")
    );
}

void handleTaskPause() {
    if(!server.hasArg("filename")) {
        server.send(
            400, 
            "application/json", 
            createErrorMessage("need to specify filename")
        );
        return;
    }
    String filename = TASK_BASE_FOLDER + server.arg("filename");
    if(!isTaskActive(filename)) {
        server.send(
            400, 
            "application/json", 
            createErrorMessage("task is not being executed")
        );
        return;
    }
    if(isTaskPaused(filename)) {
        server.send(
            400, 
            "application/json", 
            createErrorMessage("task is already paused")
        );
        return;
    }
    pauseTask(filename);
    server.send(
        200,
        "application/json", 
        createSuccessMessage("task paused successfully")
    );
}

void handleTaskUnpause() {
    if(!server.hasArg("filename")) {
        server.send(
            400, 
            "application/json", 
            createErrorMessage("need to specify filename")
        );
        return;
    }
    String filename = TASK_BASE_FOLDER + server.arg("filename");
    if(!isTaskActive(filename)) {
        server.send(
            400, 
            "application/json", 
            createErrorMessage("task is not being executed")
        );
        return;
    }
    if(!isTaskPaused(filename)) {
        server.send(
            400, 
            "application/json", 
            createErrorMessage("task is not paused")
        );
        return;
    }
    createWasmTask(filename);
    server.send(
        200,
        "application/json", 
        createSuccessMessage("task unpaused successfully")
    );
}

void handleListActiveTasks() {
    server.send(200, "application/json", listActiveTasksJson());
}

void handleListTaskDetails() {
    server.send(200, "application/json", listTaskDetailsJson());
}

void handleEraseFilesystem() {
    String response = "";
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while(file) {
        SPIFFS.remove(file.name());
        file = root.openNextFile();
    }
    numFiles = 0;
    server.send(
        200, 
        "application/json", 
        createSuccessMessage("filesystem was erased")
    );
}

void handleFilesystem() {
    const size_t capacity = JSON_ARRAY_SIZE(MAX_NUM_FILES) + JSON_OBJECT_SIZE(2);
    StaticJsonDocument<capacity> jsonDocument;

    jsonDocument.clear();
    JsonArray filesArr = jsonDocument.createNestedArray("files");
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while(file) {
        filesArr.add(file.name());
        file = root.openNextFile();
    }
    jsonDocument["status_code"] = 0;

    String res;
    serializeJson(jsonDocument, res);
    server.send(200, "application/json", res);
}

void handleGetHeapSize() {
    server.send(200, "application/json", createHeapDetailsJsonMessage());
}

void configServerRoutes() {
    server.on("/task/upload", HTTP_POST, handleUploadSuccess, handleWasmFileUpload);
    server.on("/task/delete", HTTP_POST, handleWasmTaskDelete);
    server.on("/task/edit", HTTP_POST, handleWasmFileEdit);
    server.on("/task/start", HTTP_POST, handleTaskStart);
    server.on("/task/stop", HTTP_POST, handleTaskStop);
    server.on("/task/pause", HTTP_POST, handleTaskPause);
    server.on("/task/unpause", HTTP_POST, handleTaskUnpause);
    server.on("/task/active", HTTP_GET, handleListActiveTasks);
    server.on("/task/details", HTTP_GET, handleListTaskDetails);
    server.on("/heap", HTTP_GET, handleGetHeapSize);

    // TODO: for debug/testing purposes; delete after
    server.on("/filesystem", HTTP_POST, handleFilesystem);
    server.on("/erase", HTTP_POST, handleEraseFilesystem);

    server.onNotFound(handleNotFound);
    server.begin();
}

void handleServerRequests() {
    server.handleClient();
}