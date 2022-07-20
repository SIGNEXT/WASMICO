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

#define TEST  0
#define TEST1  0
#define TEST2  0
#define TEST3  0
#define TEST4  0
#define TEST5  0
#define TEST6  0
#define TEST7  0
#define TEST8  0
#define TEST9  0
#define TEST0  0

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

const char* filename = "/tasks/test_big.wasm";

int led;
const char* greetings = "Greetigs from inside WASM!";
int fibInput;

void callGreetings() {
    println(greetings);
}

bool activatePin(int ledNum) {
    pinMode(ledNum, OUTPUT);
    return true;
}

void turnOnLed(int ledNum) {
    println("Turning led to high...");
    digitalWrite(ledNum, HIGH);
}

void turnOffLed(int ledNum) {
    println("Turning led to low...");
    digitalWrite(ledNum, LOW);
}

int fibonnaci(int x) {
   if((x == 1) || (x == 0)) {
      return x;
   } else {
      return fibonnaci(x-1) + fibonnaci(x-2);
   }
}

int fibonnaci1(int x) {
   if((x == 1) || (x == 0)) {
      return x;
   } else {
      return fibonnaci(x-1) + fibonnaci(x-2);
   }
}

int fibonnaci2(int x) {
   if((x == 1) || (x == 0)) {
      return x;
   } else {
      return fibonnaci(x-1) + fibonnaci(x-2);
   }
}

int fibonnaci3(int x) {
   if((x == 1) || (x == 0)) {
      return x;
   } else {
      return fibonnaci(x-1) + fibonnaci(x-2);
   }
}

int fibonnaci4(int x) {
   if((x == 1) || (x == 0)) {
      return x;
   } else {
      return fibonnaci(x-1) + fibonnaci(x-2);
   }
}

int fibonnaci5(int x) {
   if((x == 1) || (x == 0)) {
      return x;
   } else {
      return fibonnaci(x-1) + fibonnaci(x-2);
   }
}

int fibonnaci6(int x) {
   if((x == 1) || (x == 0)) {
      return x;
   } else {
      return fibonnaci(x-1) + fibonnaci(x-2);
   }
}

int fibonnaci7(int x) {
   if((x == 1) || (x == 0)) {
      return x;
   } else {
      return fibonnaci(x-1) + fibonnaci(x-2);
   }
}

int fibonnaci8(int x) {
   if((x == 1) || (x == 0)) {
      return x;
   } else {
      return fibonnaci(x-1) + fibonnaci(x-2);
   }
}

int fibonnaci9(int x) {
   if((x == 1) || (x == 0)) {
      return x;
   } else {
      return fibonnaci(x-1) + fibonnaci(x-2);
   }
}

void _setup() {
    callGreetings();
    led = getPinLED();
    bool res = activatePin(led);
    fibInput = 0;
}

void _loop() {
    turnOnLed(led);
    delay(1000);
    turnOffLed(led);
    delay(1000);
    int x = fibonnaci(fibInput);
    println("Another fibonnaci iteration");
    delay(1000);
    fibonnaci1(fibInput);
    fibonnaci2(fibInput);
    fibonnaci3(fibInput);
    fibonnaci4(fibInput);
    fibonnaci5(fibInput);
    fibonnaci6(fibInput);
    fibonnaci7(fibInput);
    fibonnaci8(fibInput);
    fibonnaci9(fibInput);
    fibInput++;
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