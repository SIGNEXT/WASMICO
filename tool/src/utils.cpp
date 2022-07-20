#include <ArduinoJSON.h>


#include "utils.hpp"

String createErrorMessage(String message) {
    return createJsonMessage(message, 1);
}

String createSuccessMessage(String message) {
    return createJsonMessage(message, 0);
}

String createJsonMessage(String message, int statusCode) {
    return "{\"status_code\":" + String(statusCode) + ",\"message\":\"" + message + "\"}";
}

String createHeapDetailsJsonMessage() {
    StaticJsonDocument<JSON_OBJECT_SIZE(3)> jsonDocument;
    jsonDocument["free_heap_size"] = xPortGetFreeHeapSize();
    jsonDocument["largest_free_block_size"] = heap_caps_get_largest_free_block(MALLOC_CAP_EXEC);
    jsonDocument["status_code"] = 0;
    
    String res;
    serializeJson(jsonDocument, res);
    return res;
}

unsigned int getAvailableStackSizeForTask() {
    return heap_caps_get_largest_free_block(MALLOC_CAP_EXEC);
}