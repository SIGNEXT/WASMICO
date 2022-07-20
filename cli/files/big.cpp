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

#define INPUT_1           0x0
#define INPUT_2           0x0
#define INPUT_3           0x0
#define INPUT_4           0x0
#define INPUT_5           0x0
#define INPUT_6           0x0
#define INPUT_7           0x0

#define OUTPUT_1           0x0
#define OUTPUT_2           0x0
#define OUTPUT_3           0x0
#define OUTPUT_4           0x0
#define OUTPUT_5           0x0
#define OUTPUT_6           0x0
#define OUTPUT_7           0x0

extern "C" {
    WASM_IMPORT("arduino", "delay")           void delay          (uint32_t ms);
    WASM_IMPORT("arduino", "pinMode")         void pinMode        (uint32_t pin, uint32_t mode);
    WASM_IMPORT("arduino", "digitalWrite")    void digitalWrite   (uint32_t pin, uint32_t value);
    WASM_IMPORT("arduino", "getPinLED")       uint32_t getPinLED  (void);
    WASM_IMPORT("arduino", "print")           void print          (const char* s, size_t len);
    
    WASM_IMPORT("arduino", "pauseTask")       void pauseTask      (const uint8_t* state, size_t len, const char* filename);
    WASM_IMPORT("arduino", "resumeTask")      int resumeTask      (const uint8_t* state, size_t len, const char* filename);

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

const char* filename = "/tasks/big.wasm";

const char* globalStr;
int flag;
int flag2;

int fibonacci1(int x) {
    int s1 = 0, s2 = 1, s3 = 0;
    for (int i = 0; i < x; i++) {
        s3 = s1 + s2;
        s1 = s2;
        s2 = s3;
        println(INPUT_1); println(OUTPUT_1);
    }
    return s1;
}

int fibonacci2(int x) {
    int s1 = 0, s2 = 1, s3 = 0;
    for (int i = 0; i < x; i++) {
        s3 = s1 + s2;
        s1 = s2;
        s2 = s3;
        println(INPUT_2); println(OUTPUT_2);
    }
    return s1;
}

int fibonacci3(int x) {
    int s1 = 0, s2 = 1, s3 = 0;
    for (int i = 0; i < x; i++) {
        s3 = s1 + s2;
        s1 = s2;
        s2 = s3;
        println(INPUT_3); println(OUTPUT_3);
    }
    return s1;
}

int fibonacci4(int x) {
    int s1 = 0, s2 = 1, s3 = 0;
    for (int i = 0; i < x; i++) {
        s3 = s1 + s2;
        s1 = s2;
        s2 = s3;
        println(INPUT_4); println(OUTPUT_4);
    }
    return s1;
}

int fibonacci5(int x) {
    int s1 = 0, s2 = 1, s3 = 0;
    for (int i = 0; i < x; i++) {
        s3 = s1 + s2;
        s1 = s2;
        s2 = s3;
        println(INPUT_5); println(OUTPUT_5);
    }
    return s1;
}

int fibonacci6(int x) {
    int s1 = 0, s2 = 1, s3 = 0;
    for (int i = 0; i < x; i++) {
        s3 = s1 + s2;
        s1 = s2;
        s2 = s3;
        println(INPUT_6); println(OUTPUT_6);
    }
    return s1;
}

int fibonacci7(int x) {
    int s1 = 0, s2 = 1, s3 = 0;
    for (int i = 0; i < x; i++) {
        s3 = s1 + s2;
        s1 = s2;
        s2 = s3;
        println(INPUT_7); println(OUTPUT_7);
    }
    return s1;
}

int run_fib_routine1() {
    int res = 0;
    for (int i = 0; i < 45; i++) {
        res += fibonacci1(i);
    }
    return res;
}

int run_fib_routine2() {
    int res = 0;
    for (int i = 0; i < 45; i++) {
        res += fibonacci2(i);
    }
    return res;
}

int run_fib_routine3() {
    int res = 0;
    for (int i = 0; i < 45; i++) {
        res += fibonacci3(i);
    }
    return res;
}

int run_fib_routine4() {
    int res = 0;
    for (int i = 0; i < 45; i++) {
        res += fibonacci4(i);
    }
    return res;
}

int run_fib_routine5() {
    int res = 0;
    for (int i = 0; i < 45; i++) {
        res += fibonacci5(i);
    }
    return res;
}

int run_fib_routine6() {
    int res = 0;
    for (int i = 0; i < 45; i++) {
        res += fibonacci6(i);
    }
    return res;
}

int run_fib_routine7() {
    int res = 0;
    for (int i = 0; i < 45; i++) {
        res += fibonacci7(i);
    }
    return res;
}

void _setup() {
    resumeTask(NULL, 0, filename);
    globalStr = "GlobalTest1";
    flag = 0;
    flag2 = 0;
}

void _loop() {
    int res;
    if (flag2) {
        res = run_fib_routine1();
        res = run_fib_routine2();
        res = run_fib_routine3();
        res = run_fib_routine4();
        res = run_fib_routine5();
        res = run_fib_routine6();
        res = run_fib_routine7();
    }

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