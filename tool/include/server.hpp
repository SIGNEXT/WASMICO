#ifndef Server_H
#define Server_H

#pragma once

enum FileUploadStatus { 
    VALID,
    FAILED
};

void handleNotFound();

void handleUploadSuccess();

void handleWasmFileUpload();

void handleWasmFileEdit();

void getFileUploadArguments(bool& liveUpdate, unsigned int& reservedStackSize, unsigned int& reservedInitialMemory, unsigned int& memoryLimit);

void handleWasmTaskDelete();

void handleTaskStart();

void handleTaskStop();

void handleTaskPause();

void handleTaskUnpause();

void handleListActiveTasks();

void handleListTaskDetails();

void handleEraseFilesystem();

void handleFilesystem();

void configServerRoutes();

void handleServerRequests();

void handleGetHeapSize();

#endif