#ifndef Setup_H
#define Setup_H

#pragma once

void startSerial();

void disableWDT();

void printGreetingMessage();

void startFilesystem();

void startWifi();

void initWasmEnvironment();

void startWebServer();

void handleRequests();

#endif