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

const char* filename = "/tasks/7threads_test7.wasm";

int fibonacci(int x) {
    int s1 = 0, s2 = 1, s3 = 0;
    for (int i = 0; i < x; i++) {
        s3 = s1 + s2;
        s1 = s2;
        s2 = s3;
    }
    return s1;
}

void _setup() {
    delay(1000);
    print("Start 7!\n");
    int res = 0;
    for (int i = 0; i < 1501; i++) {
        res = fibonacci(i);
        if (i == 1499) {
            print("End 7!\n");
        }
        else if (i == 1500) {
            int finalFreeHeapSize = getFreeHeapSize();
            printInt(finalFreeHeapSize); print(" <- Heap size when ending 7\n");
        }
    }
    if (res < 0) {  // res value needs to be used, otherwise compiler will omit fibonacci calls
        println("something");
    }
}

void _loop() {
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
    println("Calling pause operation - 7 Threads Load Test - Task 7");
}