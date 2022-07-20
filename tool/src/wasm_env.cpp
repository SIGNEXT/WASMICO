#include <ArduinoJSON.h>
#include <m3_env.h>
#include "SPIFFS.h"

#include "task.hpp"
#include "wasm_env.hpp"

////////////////////////
// macros
////////////////////////
#define ESP32_ONBOARD_LED   2
#define WASM_STACK_SLOTS    1024
#define WASM_TASK_PRIORITY  1
#define FATAL(func, msg) { Serial.print("Fatal: " func " "); Serial.println(msg); }

IM3Environment env;

std::map<String, TaskHandle_t> taskHandles;
std::map<String, IM3Runtime> taskRuntimes;

std::map<String, PausedTaskInfo> pausedTasks;


void createEnvironment() {
    // create WASM3 environment for the interpreter
    env = m3_NewEnvironment();
    if (!env) {
        FATAL("NewEnvironment", "failed");
    }
}

////////////////////////
// binding functions
////////////////////////
m3ApiRawFunction(m3_arduino_millis) {
    m3ApiReturnType(uint32_t)

    m3ApiReturn(millis());
}

uint8_t mapPinMode(uint8_t mode) {
    switch(mode) {
        case 0: return INPUT;
        case 1: return OUTPUT;
        case 2: return INPUT_PULLUP;
    }
    return INPUT;
}

m3ApiRawFunction(m3_arduino_pinMode) {
    m3ApiGetArg(uint32_t, pin)
    m3ApiGetArg(uint32_t, mode)

#if !defined(PARTICLE)
    typedef uint8_t PinMode;
#endif
    pinMode(pin, (PinMode)mapPinMode(mode));

    m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_getPinLED) {
    m3ApiReturnType(uint32_t)

    m3ApiReturn(ESP32_ONBOARD_LED);
}

m3ApiRawFunction(m3_arduino_digitalWrite) {
    m3ApiGetArg(uint32_t, pin)
    m3ApiGetArg(uint32_t, value)

    digitalWrite(pin, value);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_delay) {
    m3ApiGetArg(uint32_t, ms)

    vTaskDelay(ms / portTICK_PERIOD_MS);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_print) {
    m3ApiGetArgMem(const uint8_t *, buf)
    m3ApiGetArg(uint32_t, len)

    Serial.write(buf, len);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_printInt) {
    m3ApiGetArg(int, num)

    Serial.print(num);

    m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_pauseTask) {
    m3ApiGetArgMem(const uint8_t *, state)
    m3ApiGetArg(uint32_t, len)
    m3ApiGetArgMem(const char*, filename)

    PausedTaskInfo pausedTaskInfo;
    pausedTaskInfo.state = (uint8_t*) state;
    pausedTaskInfo.len = len;

    pausedTasks[filename] = pausedTaskInfo;

    m3ApiSuccess();
}

m3ApiRawFunction(m3_arduino_resumeTask) {
    m3ApiReturnType(uint32_t)
    m3ApiGetArgMem(uint8_t *, state)
    m3ApiGetArg(uint32_t, len)
    m3ApiGetArgMem(const char*, filename)

    uint32_t hadState = 0;
    if (pausedTasks.count(filename)) {
        PausedTaskInfo pausedTaskInfo = pausedTasks[filename];
        memmove(state, pausedTaskInfo.state, min(len, pausedTaskInfo.len));
        pausedTasks.erase(filename);
        hadState = 1;
    }

    m3ApiReturn(hadState);
}

bool to_hex(char* dest, size_t dest_len, const uint8_t* values, size_t val_len) {
    if(dest_len < (val_len*2+1)) /* check that dest is large enough */
        return false;
    *dest = '\0'; /* in case val_len==0 */
    while(val_len--) {
        /* sprintf directly to where dest points */
        sprintf(dest, "%02X", *values);
        dest += 2;
        ++values;
    }
    return true;
}

////////////////////////
// WASM interpreter
////////////////////////
M3Result bindWasmFunctions(IM3Runtime runtime) {
    IM3Module module = runtime->modules;
    const char* module_name = "arduino";

    m3_LinkRawFunction(module, module_name, "millis", "i()", &m3_arduino_millis);
    m3_LinkRawFunction(module, module_name, "pinMode", "v(ii)", &m3_arduino_pinMode);
    m3_LinkRawFunction(module, module_name, "getPinLED", "i()", &m3_arduino_getPinLED);
    m3_LinkRawFunction(module, module_name, "digitalWrite", "v(ii)", &m3_arduino_digitalWrite);
    m3_LinkRawFunction(module, module_name, "delay", "v(i)", &m3_arduino_delay);
    m3_LinkRawFunction(module, module_name, "print", "v(*i)", &m3_arduino_print);
    m3_LinkRawFunction(module, module_name, "printInt", "v(i)", &m3_arduino_printInt);

    m3_LinkRawFunction(module, module_name, "pauseTask", "v(*i*)", &m3_arduino_pauseTask);
    m3_LinkRawFunction(module, module_name, "resumeTask", "i(*i*)", &m3_arduino_resumeTask);

    // Serial.println("Linked Arduino functions");
    return m3Err_none;
}

void callWasmFunction(IM3Runtime runtime, const char* functionName) {
    M3Result result = m3Err_none;
    // start WASM program
    IM3Function f;
    result = m3_FindFunction(&f, runtime, functionName);
    if (result) FATAL("FindFunction", result);

    result = m3_CallV(f);

    // Getting here means we've encountered an error.
    // Let's print out some debugging information
    if (result) {
        M3ErrorInfo info;
        m3_GetErrorInfo(runtime, &info);
        Serial.print("Error: ");
        Serial.print(result);
        Serial.print(" (");
        Serial.print(info.message);
        Serial.println(")");
        if (info.file && strlen(info.file) && info.line) {
            Serial.print("At ");
            Serial.print(info.file);
            Serial.print(":");
            Serial.println(info.line);
        }
    }
}

void runWasmThread(void* parameters) {
    const char* filename = static_cast<const char*>(parameters);
    M3Result result = m3Err_none;

    IM3Runtime runtime = m3_NewRuntime(env, WASM_STACK_SLOTS, NULL);
    if (!runtime) {
        FATAL("NewRuntime", "failed");
        removeWasmTask(filename);
        return;
    }
    runtime->memoryLimit = getTaskDetails(filename).memoryLimit;

    // read WASM module from the filesystem into a uint8_t buffer
    if (!SPIFFS.exists(filename)) {
        Serial.println("WASM file does not exist");
        Serial.print("File: "); Serial.println(filename);
        removeWasmTask(filename);
        return;
    }
    // Serial.println("Trying to open WASM file...");
    File file = SPIFFS.open(filename, "r");
    if (!file) {
        Serial.println("Failed to open WASM file for reading");
        removeWasmTask(filename);
        return;
    }
    // Serial.println("Opened WASM file");
    size_t app_wasm_len = file.size();
    uint8_t app_wasm[app_wasm_len];
    file.read(app_wasm, app_wasm_len);
    file.close();
    // Serial.println("Copied file contents into buffer");

    // parse the module using the environment
    IM3Module module;
    result = m3_ParseModule(env, &module, app_wasm, app_wasm_len);
    if (result) {
        FATAL("ParseModule", result);
        removeWasmTask(filename);
        return;
    }

    // Serial.println("Parsed WASM module");
    result = m3_LoadModule(runtime, module);
    if (result) {
        FATAL("LoadModule", result);
        removeWasmTask(filename);
        return;
    }
    // Serial.println("Loaded WASM module");

    // link created binding functions
    result = bindWasmFunctions(runtime);
    if (result) {
        FATAL("BindWasmFunctions", result);
        removeWasmTask(filename);
        return;
    }

    taskRuntimes[filename] = runtime;
    callWasmFunction(runtime, "_start");
    
    removeWasmTask(filename);
}

void runWasmPause(void* parameters) {
    const char* filename = static_cast<const char*>(parameters);
    IM3Runtime runtime = taskRuntimes[filename];

    callWasmFunction(runtime, "_pause");

    vTaskDelete(taskHandles[filename]);
    taskHandles.erase(filename);

    vTaskDelete(NULL);
}

////////////////////////
// WASM task operations
////////////////////////

void createWasmTask(String filename) {
    Serial.print("Starting new task ");
    Serial.print(filename);
    Serial.println("...");

    TaskHandle_t taskHandle = NULL;
    taskHandles[filename] = taskHandle;

    xTaskCreate(
        &runWasmThread, 
        filename.c_str(),
        getTaskDetails(filename).reservedStackSize, 
        (void*) taskHandles.find(filename)->first.c_str(),  // get map key, to have a permanent reference
        WASM_TASK_PRIORITY, 
        &taskHandles[filename]
    );
}

void removeWasmTask(String filename) {
    Serial.print("Stopping task ");
    Serial.print(filename);
    Serial.println("...");

    taskRuntimes.erase(filename);
    pausedTasks.erase(filename);
    
    if (taskHandles.count(filename)) {
        TaskHandle_t taskHandler = taskHandles[filename];
        taskHandles.erase(filename);
        vTaskDelete(taskHandler);
    }
}

bool isTaskActive(String filename) {
    return taskRuntimes.count(filename);
}

void liveUpdateTask(String filename) {
    if (isTaskActive(filename)) {
        Serial.print("Performing live update for task ");
        Serial.print(filename);
        Serial.println("...");

        removeWasmTask(filename);
        createWasmTask(filename);
    }
}

void pauseTask(String filename) {
    xTaskCreate(
        &runWasmPause, 
        (filename + "_pause").c_str(),
        getTaskDetails(filename).reservedStackSize, 
        (void*) taskRuntimes.find(filename)->first.c_str(),  // get map key, to have a permanent reference
        WASM_TASK_PRIORITY, 
        NULL
    );
}

bool isTaskPaused(String filename) {
    return pausedTasks.count(filename);
}

String listActiveTasksJson() {
    const size_t numActiveTasks = taskRuntimes.size();
    const size_t capacity = JSON_ARRAY_SIZE(numActiveTasks) + numActiveTasks * JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(3);
    DynamicJsonDocument jsonDocument(capacity);

    JsonArray tasks = jsonDocument.createNestedArray("tasks");
    std::map<String, IM3Runtime>::iterator it;
    for(it = taskRuntimes.begin(); it != taskRuntimes.end(); it++) {
        JsonObject activeTaskObj = tasks.createNestedObject();
        activeTaskObj["filename"] = it->first.c_str();
        bool paused = isTaskPaused(it->first);
        activeTaskObj["paused"] = paused;
        if (!paused) {
            activeTaskObj["stack_high_water_mark"] = uxTaskGetStackHighWaterMark(taskHandles[it->first]);  
        }
    }
    jsonDocument["espressif_num_threads"] = uxTaskGetNumberOfTasks();
    jsonDocument["status_code"] = 0;
    
    String res;
    serializeJson(jsonDocument, res);
    return res;
}