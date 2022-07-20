#include <stdint.h>
#include <cstddef>

#define WASM_EXPORT                   extern "C" __attribute__((used)) __attribute__((visibility ("default")))
#define WASM_EXPORT_AS(NAME)          WASM_EXPORT __attribute__((export_name(NAME)))
#define WASM_IMPORT(MODULE,NAME)      __attribute__((import_module(MODULE))) __attribute__((import_name(NAME)))
#define WASM_CONSTRUCTOR              __attribute__((constructor))

extern "C" {
    WASM_IMPORT("arduino", "millis")          uint32_t millis     (void);
    WASM_IMPORT("arduino", "micros")          uint32_t micros     (void);
    WASM_IMPORT("arduino", "delay")           void delay          (uint32_t ms);
    WASM_IMPORT("arduino", "pinMode")         void pinMode        (uint32_t pin, uint32_t mode);
    WASM_IMPORT("arduino", "digitalWrite")    void digitalWrite   (uint32_t pin, uint32_t value);
    WASM_IMPORT("arduino", "getPinLED")       uint32_t getPinLED  (void);
    WASM_IMPORT("arduino", "print")           void print          (const char* s, size_t len);
    WASM_IMPORT("arduino", "printInt")        void printInt       (int num);
    WASM_IMPORT("arduino", "printDouble")     void printDouble    (double num);

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

    static inline void println (double num) {
        printDouble(num); print("\n");
    }
}

const char* filename = "/tasks/fib_it.wasm";
int cnt;
int fib_res;
unsigned long t1;

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
    for (int i = 0; i < 45; i++) {
        res += fibonacci(i);
    }
    return res;
}

void _setup() {
    cnt = 0;
    t1 = micros();

	for ( ;; ) {
        fib_res = run_fib_routine();
        cnt++;
        if (cnt == 100) {
            unsigned long t2 = micros();

            print("Total time: ");
            printInt((int) (t2 - t1));
            println(" microseconds");

            print("Average time: ");
            int avr = ((t2 - t1) / 100);
            printInt((int) avr);
            println(" microseconds");

            println(fib_res);
            delay(100000);
        }
	}
}

void _loop() {
    delay(100000);
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
    println("Calling pause operation - fib_it");
}