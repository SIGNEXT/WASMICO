#include <Arduino.h>
#include <wasm3.h>
#include <WiFi.h>
#include <esp_task_wdt.h>
#include "SPIFFS.h"

#include "setup.hpp"
#include "server.hpp"
#include "wasm_env.hpp"

void startSerial() {
	Serial.begin(115200);
	while (!Serial) {}
	Serial.println("");
}

void disableWDT() {
    Serial.println("Disabling WDT...");
    if (esp_task_wdt_init(60, false) != ESP_OK) {
        Serial.println("Error occurred while disabling WDT!");
    }
}

void printGreetingMessage() {
    Serial.println("\nWasm3 v" M3_VERSION " (" M3_ARCH "), build " __DATE__ " " __TIME__);
}

void startFilesystem() {
    if(SPIFFS.begin(true)) {
        Serial.println("SPIFFS filesystem has been successfully mounted");
    }
    else {
        Serial.println("An Error has occurred while mounting SPIFFS");
    }
}

void startWifi() {
	WiFi.mode(WIFI_STA);
	WiFi.begin(WIFI_SSID, WIFI_PASS);
	// Wait for connection
	while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(WIFI_SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void initWasmEnvironment() {
    Serial.println("Initializing WASM environment...");
    createEnvironment();
}

void startWebServer() {
    Serial.println("Starting web server...");
    configServerRoutes();
    Serial.println("Listening to requests...");
}
