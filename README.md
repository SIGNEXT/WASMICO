# WASMICO: Micro-containers in microcontrollers for the Internet-of-Things

WASMICO is a micro-containerization plataform built on top of Arduino and ESP-IDF, that allows the executing of WebAssembly modules on-demand on a resource-constrained IoT device. Because WASMICO executes WebAssembly tasks and uses a Wasm3 interpreter, it supports any programs written in any programming language that can be later compiled to WASM, like C++, Go, Rust, among many others. It supports operations that enable the user to control and manage the complete lifecycle of their tasks and containers, all through the HTTP protocol, in an over-the-air (OTA) manner that does not require a reboot of the device or a physical connection to it.

WASMICO also acts as an abstraction between the high-level specifications and the device’s low-level capabilities related to its hardware and operating system, offering to the client a controlled use, through a middleware, of the device capabilities, like reading and writing to pins and printing to the serial port. Furthermore, it increases isolation and security, given that a crash of a 6 task does not affect other running tasks or the rest of the system.

It also offers better code portability between devices since it works as a middleware between the tasks and the OS/hardware of the device. Even though WASMICO was developed having the ESP32 microcontroller in mind, it can be easily ported to any other constrained device that is able to run FreeRTOS (which is supported by many devices with different architectures), requiring no changes to the client-defined tasks.

The repository hosts the following content:

* `tool`: Contains the main WASMICO code that is installed on the IoT device.
* `cli`: Contains the code for the WASMICO CLI.
