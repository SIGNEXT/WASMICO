#include <stdint.h>
#include <cstddef>

#define WASM_EXPORT                   extern "C" __attribute__((used)) __attribute__((visibility ("default")))
#define WASM_EXPORT_AS(NAME)          WASM_EXPORT __attribute__((export_name(NAME)))
#define WASM_IMPORT(MODULE,NAME)      __attribute__((import_module(MODULE))) __attribute__((import_name(NAME)))
#define WASM_CONSTRUCTOR              __attribute__((constructor))

extern "C" {
    WASM_IMPORT("arduino", "delay")           void delay          (uint32_t ms);
    WASM_IMPORT("arduino", "pinMode")         void pinMode        (uint32_t pin, uint32_t mode);
    WASM_IMPORT("arduino", "digitalWrite")    void digitalWrite   (uint32_t pin, uint32_t value);
    WASM_IMPORT("arduino", "getPinLED")       uint32_t getPinLED  (void);
    WASM_IMPORT("arduino", "print")           void print          (const char* s, size_t len);
    WASM_IMPORT("arduino", "printInt")        void printInt       (int num);

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

    static inline void println (int num) {
        printInt(num); print("\n");
    }
}

const char* filename = "/tasks/fib.wasm";
int cnt;

int fibonacci(int x) {
    if((x == 1) || (x == 0)) {
       return x;
    }
    return fibonacci(x-1) + fibonacci(x-2);
}

void _setup() {
    cnt = 0;
}

void _loop() {
    println(fibonacci(cnt));
    cnt++;
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
    println("Calling pause operation - Big");
}