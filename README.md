
# ChippyCore: A Modular CHIP-8 Emulator

## Table of Contents
1. [Introduction](#introduction)
2. [Why This Project?](#why-this-project)
3. [Features](#features)
4. [Code Structure](#code-structure)
    - [Public Methods](#public-methods)
5. [Quirks Explained](#quirks-explained)
6. [Usage Guide](#usage-guide)
7. [Examples](#examples)
    - [Example `ChippyCore.ino`](#example-chippycoreino)
8. [Callback Functions Explanation](#callback-functions-explanation)
9. [Contributing](#contributing)

## Introduction
The CHIP-8 is a simple, interpreted programming language that was originally used on the COSMAC VIP and Telmac 1600 microcomputers in the mid-1970s. It is now commonly used for educational purposes to teach basic assembly language concepts. This project aims to create a modular CHIP-8 emulator that can be easily integrated with different hardware components like OLED screens, buzzers, and keypads.

## Why This Project?
The primary goal of this project is to build a highly modular Chip8 emulator that can be extended or modified to work with various hardware setups. The design allows developers to integrate features such as sound, display outputs, and input handling seamlessly, making it ideal for hobbyists, educators, and enthusiasts who want to experiment with CHIP-8 programming.

## Features
- **Modular Design:** Easily integrate different hardware components.
- **Quirk Support:** Emulate various quirks found in different implementations of the Chip8 interpreter.
- **Callbacks:** Use callback functions for drawing pixels, screen updates, and input handling.
- **Custom Quirks Configuration:** Configure emulator behavior to match specific ROM requirements.

## Code Structure
The project is structured into three main files: `chippycore.h`, `chippycore.cpp`, and `ChippyCore.ino`.

### Public Methods

#### `void load_and_run(const uint8_t* data, size_t dataSize, drawPixelCallback dCallback, screenCallback sCallback, loopCallback lCallback, const bool* config);`
- **Purpose:** Loads a ROM into the emulator and starts execution.
- **Parameters:**
    - `data`: Pointer to the ROM data.
    - `dataSize`: Size of the ROM data in bytes.
    - `dCallback`: Callback function for drawing pixels.
    - `sCallback`: Callback function for screen updates.
    - `lCallback`: Callback function for handling input and control (pause, stop).
    - `config`: Pointer to an array of booleans representing quirks configuration.

#### `bool isRunning();`
- **Purpose:** Checks if the emulator is currently running.
- **Returns:** Boolean indicating whether the emulator is running.

#### `void loop();`
- **Purpose:** Executes one cycle of the emulator. This method should be called repeatedly in the main loop to run the emulator.

## Quirks Explained
The Chip8 language has several quirks that can affect how certain instructions behave. These quirks are configurable through a set of booleans passed during initialization.

### Quirk 4 (QUIRK4)
- **Enabled:** VF register is reset to 0 before instructions that modify it.
- **Disabled:** VF retains its previous value before such instructions.

### Quirk 5 (QUIRK5)
- **Enabled:** Only shift VX when using shifting operations, not set VX to VY.
- **Disabled:** Set VX to VY and then perform the shift operation.

### Quirk 6 (QUIRK6)
- **Enabled:** Sprites wrap around the screen borders.
- **Disabled:** Sprites are clipped at the screen edges.

### Quirk 11 (QUIRK11)
- **Enabled:** I is incremented by the value of X. 
- **Disabled:** I is not incremented.

## Usage Guide
To use ChippyCore, you need to include the `ChippyCore.h` file and set up your Arduino sketch to handle callbacks for drawing pixels, screen updates, and input handling.

### Example `ChippyCore.ino`

```cpp
#include "chippycore.h"

// Callback function to draw a pixel on the screen with collision detection
void drawPixelCallback(const uint16_t X, const uint16_t Y, bool& collision) {
    // Implement logic to draw the pixel at (X, Y)
}

// Callback function to update the screen display
void screenUpdateCallback(bool clearScreen, bool updateScreen) {
    if(clearScreen) {
        // Implement logic to clear the screen display here
    }
    if(updateScreen) {
        // Implement logic to update the screen display here
    }
}

void loopCallback(uint8_t& keySet, bool& keyState, bool& pause, bool& stop) {
    // Handle key presses and control signals
    // For example:
    // if (someCondition) {
    //     keySet = 0x5;  // Set to key '5'
    //     keyState = true;
    // }
    // else if (anotherCondition) {
    //     pause = true;
    // }
}

// Quirks configuration
bool default_quirkconfig[4] = {   
    false,  // QUIRK4: VF reset before modifying
    false,  // QUIRK5: Only shift VX
    false,  // QUIRK6: Wrap sprites
    false   // QUIRK11: Increment behavior for I register
};

// ChippyCore instance
ChippyCore cc;

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Example ROM data to be loaded
    const uint8_t ROM[] = {0x12, 0x25};
    cc.load_and_run(ROM, sizeof(ROM), &drawPixelCallback, &screenUpdateCallback, &loopCallback, default_quirkconfig);
}

void loop() {
    if(cc.isRunning()) {
        cc.loop();
    }
}
```

## Callback Functions Explanation
The emulator uses callback functions to handle screen drawing, screen updates, and input handling.

### `drawPixelCallback`
- **Purpose:** Draws a pixel on the screen at a specific location.
- **Parameters:**
    - `X`: X-coordinate of the pixel.
    - `Y`: Y-coordinate of the pixel.
    - `collision`: Boolean reference indicating if a collision occurred during drawing.

### `screenUpdateCallback`
- **Purpose:** Handles screen updates and clearing.
- **Parameters:**
    - `clearScreen`: Boolean indicating whether to clear the screen.
    - `updateScreen`: Boolean indicating whether to update the screen display.

### `loopCallback`
- **Purpose:** Handles input handling and control signals (pause, stop).
- **Parameters:**
    - `keySet`: Reference to the key currently pressed (0-F). Set this value based on hardware input.
    - `keyState`: Reference to the state of the key press. Set to true if a key is pressed, false otherwise.
    - `pause`: Reference to control pausing and resuming emulator execution. Set to true to pause, false to resume.
    - `stop`: Reference to stop the emulator. Set to true to stop the emulator.

## Contributing
Contributions to this project are welcome! Feel free to submit pull requests with improvements or new features. Make sure to follow the existing code style and document any changes appropriately.

---
