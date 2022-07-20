#include <Arduino.h>
#include "setup.hpp"
#include "server.hpp"

void setup() {
    // put your setup code here, to run once:
	startSerial();
    // disableWDT();
    printGreetingMessage();
    startFilesystem();
    startWifi();
    initWasmEnvironment();
    startWebServer();
}

void loop() {
    // put your main code here, to run repeatedly:
    handleServerRequests();
}