#ifndef Task_H
#define Task_H

#pragma once

#include <ArduinoJSON.h>

struct WasmTaskDetails {
    unsigned int reservedStackSize;
    unsigned int reservedInitialMemory;
    unsigned int memoryLimit;
};

void addTaskDetails(String filename, int reservedStackSize, int reservedInitialMemory, int memoryLimit);

WasmTaskDetails& getTaskDetails(String filename);

bool doesTaskDetailsExist(String filename);

bool taskStartIsPossible(String filename);

void deleteWasmFile(String filename);

String listTaskDetailsJson();

#endif