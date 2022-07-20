#include <stdint.h>
#include <cstddef>

#define WASM_EXPORT                   extern "C" __attribute__((used)) __attribute__((visibility ("default")))
#define WASM_EXPORT_AS(NAME)          WASM_EXPORT __attribute__((export_name(NAME)))
#define WASM_IMPORT(MODULE,NAME)      __attribute__((import_module(MODULE))) __attribute__((import_name(NAME)))
#define WASM_CONSTRUCTOR              __attribute__((constructor))

extern "C" {
    WASM_IMPORT("arduino", "micros")          uint32_t micros     (void);
    WASM_IMPORT("arduino", "delay")           void delay          (uint32_t ms);
    WASM_IMPORT("arduino", "pinMode")         void pinMode        (uint32_t pin, uint32_t mode);
    WASM_IMPORT("arduino", "digitalWrite")    void digitalWrite   (uint32_t pin, uint32_t value);
    WASM_IMPORT("arduino", "getPinLED")       uint32_t getPinLED  (void);
    WASM_IMPORT("arduino", "print")           void print          (const char* s, size_t len);
    WASM_IMPORT("arduino", "printInt")        void printInt       (int num);

    WASM_IMPORT("arduino", "pauseTask")       void pauseTask      (const uint8_t* state, size_t len, const char* filename);
    WASM_IMPORT("arduino", "resumeTask")      uint32_t resumeTask (const uint8_t* state, size_t len, const char* filename);
    WASM_IMPORT("arduino", "getFreeHeapSize") uint32_t getFreeHeapSize (void);

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

const char* filename = "/tasks/dynamic2.wasm";
uint8_t state[4];
uint32_t iterationCounter;

int fibonacci(int x) {
    int s1 = 0, s2 = 1, s3 = 0;
    for (int i = 0; i < x; i++) {
        s3 = s1 + s2;
        s1 = s2;
        s2 = s3;
    }
    return s1;
}

int run_fib_routine() {
    int res = 0;
    for (int i = 0; i < 1500; i++) {
        res = fibonacci(i);
    }
    return res;
}

void intToByteArray(int num, uint8_t* bytes) {
    bytes[0] = (num >> 24) & 0xFF;
    bytes[1] = (num >> 16) & 0xFF;
    bytes[2] = (num >> 8) & 0xFF;
    bytes[3] = (num >> 0) & 0xFF;
}

uint32_t byteArrayToInt(uint8_t* bytes) {
    return bytes[3] + (bytes[2] << 8) + (bytes[1] << 16) + (bytes[0] << 24);
}

void _setup() {
    print("Free heap size after starting task 2: "); println(getFreeHeapSize());
    println("Dynamic task 2 started!");
    iterationCounter = 0;
    if (resumeTask(state, 4, filename)) {
        println("State was recovered!");
        iterationCounter = byteArrayToInt(state);
    }
}

void _loop() {
    int fib_res = run_fib_routine();
    iterationCounter++;
    print("Dynamic2 ran iteration "); println(iterationCounter);
    delay(5000);
    if (fib_res < 0) {
        println("Something");
    }
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
    println("Calling pause operation - Dynamic Test 2");
    intToByteArray(iterationCounter, state);
    pauseTask(state, 4, filename);
}