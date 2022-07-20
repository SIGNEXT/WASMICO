#ifndef WasmEnv_H
#define WasmEnv_H

#pragma once

#include <ArduinoJSON.h>
#include <wasm3.h>
#include <map>

struct PausedTaskInfo {
    uint8_t* state;
    size_t len;
};

void createEnvironment();

IM3Runtime createWasmRuntime(String filename);

void runWasmFunction(IM3Runtime runtime, const char* functionName);

void runWasmThread(void* parameters);

void runWasmPause(void* parameters);

void createWasmTask(String filename);

void removeWasmTask(String filename);

M3Result bindWasmFunctions(IM3Runtime runtime);

void callWasmFunction(IM3Runtime runtime, const char* functionName);

bool isTaskActive(String filename);

void liveUpdateTask(String filename);

void pauseTask(String filename);

bool isTaskPaused(String filename);

String listActiveTasksJson();

// TODO: remove this function after
bool to_hex(char* dest, size_t dest_len, const uint8_t* values, size_t val_len);

#endif