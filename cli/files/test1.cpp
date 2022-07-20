#include <stdint.h>
#include <cstddef>

#define WASM_EXPORT                   extern "C" __attribute__((used)) __attribute__((visibility ("default")))
#define WASM_EXPORT_AS(NAME)          WASM_EXPORT __attribute__((export_name(NAME)))
#define WASM_IMPORT(MODULE,NAME)      __attribute__((import_module(MODULE))) __attribute__((import_name(NAME)))
#define WASM_CONSTRUCTOR              __attribute__((constructor))

#define LOW               0x0
#define HIGH              0x1

#define INPUT             0x0
#define OUTPUT            0x1
#define INPUT_PULLUP      0x2

extern "C" {
    WASM_IMPORT("arduino", "delay")           void delay          (uint32_t ms);
    WASM_IMPORT("arduino", "pinMode")         void pinMode        (uint32_t pin, uint32_t mode);
    WASM_IMPORT("arduino", "digitalWrite")    void digitalWrite   (uint32_t pin, uint32_t value);
    WASM_IMPORT("arduino", "getPinLED")       uint32_t getPinLED  (void);
    WASM_IMPORT("arduino", "print")           void print          (const char* s, size_t len);
    
    WASM_IMPORT("arduino", "pauseTask")       void pauseTask      (const uint8_t* state, size_t len, const char* filename);
    WASM_IMPORT("arduino", "resumeTask")      void resumeTask     (const uint8_t* state, size_t len, const char* filename);

    static inline size_t strlen(const char *str) {
        const char *s;
        for (s = str; *s; ++s);
        return (s - str);
    }

    static inline void print (const char* s) {
        print(s, strlen(s));
    }

    static inline void println (const char* s) {
        print(s); print("\n");
    }
}

const char* filename = "/tasks/test1.wasm";

const char* globalStr;
int flag;

void _setup() {
    resumeTask(NULL, 0, filename);
    globalStr = "GlobalTest1";
    flag = 0;
}

void _loop() {
    const char* str = "Test1";
    if (flag) {
        println(str);
    }
    else {
        println(globalStr);
    }
    flag = flag ? 0 : 1;
    delay(2000);
}

/*
 * Entry point
 */
WASM_EXPORT
void _start() {
    _setup();
    while (1) { _loop(); }
}

WASM_EXPORT
void _pause() {
    pauseTask(NULL, 0, filename);
    println("Calling pause operation - Test1");
}