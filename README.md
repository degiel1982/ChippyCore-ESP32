# ChippyCore README

## Table of Contents
1. [Introduction](#introduction)
2. [Getting Started](#getting-started)
    - [System Requirements](#system-requirements)
    - [Installation](#installation)
3. [ChippyCore Overview](#chippycore-overview)
4. [Using ChippyCore](#using-chippycore)
    - [Loading and Running a ROM](#loading-and-running-a-rom)
    - [Callback Functions](#callback-functions)
    - [Quirks Configuration](#quirks-configuration)
5. [Public Methods Reference](#public-methods-reference)
6. [Examples](#examples)

## Introduction
ChippyCore is an emulator core designed to run Chip-8 programs, a simple 8-bit virtual machine that was originally used on early 1970s computers. It provides a flexible and extensible way to simulate the behavior of the Chip-8 system, allowing developers to integrate it into various projects or create custom emulators.

## Getting Started

### System Requirements
- A C++ compatible development environment (e.g., Arduino IDE, Visual Studio Code with PlatformIO)
- A basic understanding of C++ programming

### Installation
1. Clone the ChippyCore repository from GitHub.
2. Include the `ChippyCore` folder in your project directory.
3. Make sure to include the necessary header files and source files in your project.

## ChippyCore Overview
ChippyCore is designed to be modular and flexible, providing a set of public methods that allow developers to control the emulator's behavior. It supports loading Chip-8 ROMs, handling input/output operations, and configuring quirks specific to certain ROMs.

## Using ChippyCore

### Loading and Running a ROM
To load and run a Chip-8 ROM, you can use the `load_and_run` method provided by ChippyCore. This method takes several parameters:
- The ROM data as a byte array.
- The size of the ROM.
- Callback functions for drawing pixels and updating the screen.
- A callback function for handling user input (optional).
- An array specifying quirks configurations specific to certain ROMs.

```cpp
void playGame(const uint8_t* rom, const size_t romSize, const bool* quirkconfig_game) {
    ChippyCore cc;
    cc.load_and_run(rom, romSize, &drawPixelCallback, &screenUpdateCallback, &loopCallback, quirkconfig_game);
    while (cc.isRunning()) {
        cc.loop();
    }
}
```

### Callback Functions
ChippyCore requires callback functions to handle specific operations like drawing pixels and updating the screen. These functions should be implemented by the user:

- **drawPixelCallback**: Called when a pixel needs to be drawn on the screen with collision detection.
- **screenUpdateCallback**: Called when the screen needs to be updated or cleared.
- **loopCallback**: Optional callback for handling user input and controlling emulator state.

```cpp
void drawPixelCallback(const uint16_t X, const uint16_t Y, bool& collision) {
    // Implement logic to draw a pixel on the screen with collision detection here
}

void screenUpdateCallback(bool clearScreen, bool updateScreen) {
    if (clearScreen) {
        // Implement logic to clear the screen display here
    }
    if (updateScreen) {
        // Implement logic to update the screen display here
    }
}

void loopCallback(uint8_t& key, bool& key_state, bool& pause, bool& stop) {
    // Implement logic for user input and emulator state control here
}
```

### Quirks Configuration
Some Chip-8 ROMs require specific quirks to function correctly. ChippyCore allows you to configure these quirks using an array of boolean values:

```cpp
bool default_quirkconfig[5] = {   
    false,  // COSMAC-based variants reset VF register to 0 before certain operations.
            // Enabled: VF is reset to 0 before instructions that modify it. 
            // Disabled: VF retains its previous value before such instructions. 

    false,  // QUIRK5: CHIP-48/SCHIP-1.x don't set vX to vY when shifting, so only shift vX.

    false,  // Wrapping or clipping behavior for sprites. 
            // Enabled: Sprites wrap around the screen borders. 
            // Disabled: Sprites are clipped at the screen edges. 

    false,  // Increment behavior for the I register. 
            // Enabled: For CHIP-48/SCHIP-1.0, I is incremented by the value of X. For SCHIP-1.1, I is not incremented. 
            // Disabled: I is not incremented in either case.
};
```

## Public Methods Reference

### `load_and_run`
- **Description**: Loads a Chip-8 ROM and starts the emulator.
- **Parameters**:
  - `rom`: Pointer to the ROM data as a byte array.
  - `romSize`: Size of the ROM.
  - `drawPixelCallback`: Callback function for drawing pixels.
  - `screenUpdateCallback`: Callback function for updating the screen.
  - `loopCallback` (optional): Callback function for handling user input and emulator state control.
  - `quirkconfig_game`: Array specifying quirks configurations specific to certain ROMs.

### `isRunning`
- **Description**: Checks if the emulator is currently running.
- **Returns**: Boolean value indicating whether the emulator is running.

### `loop`
- **Description**: Executes a single cycle of the emulator, processing one opcode and updating the program counter accordingly.
- **Parameters**: None

## Examples
Here is an example of how to use ChippyCore to load and run a simple Chip-8 ROM:

```cpp
#include "chippycore.h"

// Callback function to draw a pixel on the screen with collision detection
void drawPixelCallback(const uint16_t X, const uint16_t Y, bool& collision) {
    // Implement logic to draw a pixel on the screen with collision detection here
}

// Callback function to update the screen display
void screenUpdateCallback(bool clearScreen, bool updateScreen) {
    if (clearScreen) {
        // Implement logic to clear the screen display here
    }
    if (updateScreen) {
        // Implement logic to update the screen display here
    }
}

void loopCallback(uint8_t& key, bool& key_state, bool& pause, bool& stop) {
    // Implement logic for user input and emulator state control here
}

bool default_quirkconfig[5] = {   
    false,
    false,
    false,
    false
};

void playGame(const uint8_t* rom, const size_t romSize, const bool* quirkconfig_game) {
    ChippyCore cc;
    cc.load_and_run(rom, romSize, &drawPixelCallback, &screenUpdateCallback, &loopCallback, quirkconfig_game);
    while (cc.isRunning()) {
        cc.loop();
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
}

void loop() {
    // Example ROM data to be loaded
    const uint8_t ROM[] = {0x12, 0x25};
    // Start the game with the specified ROM
    playGame(ROM, sizeof(ROM), default_quirkconfig);
}
```

This example demonstrates how to set up and run a simple Chip-8 program using ChippyCore. The callback functions need to be implemented based on your specific requirements for drawing pixels and handling user input.