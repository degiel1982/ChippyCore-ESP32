Hier is een bijgewerkte versie van de README met een aantrekkelijke introductie:

---

# ChippyCore Library

## Table of Contents
1. [Introduction](#introduction)
2. [Overview](#overview)
3. [Public Methods](#public-methods)
   - [play_game](#play_game)
   - [stop](#stop)
   - [pause](#pause)
4. [Quirk Configuration](#quirk-configuration)
5. [Examples](#examples)
   - [Starting the Emulator](#example-1-starting-the-emulator)
   - [Stopping the Emulator](#example-2-stopping-the-emulator)
   - [Pausing and Resuming the Emulator](#example-3-pausing-and-resuming-the-emulator)

---

## Introduction
Welcome to the **ChippyCore Library**, a modern, lightweight, and customizable CHIP-8 emulator designed for developers, hobbyists, and retro computing enthusiasts. Whether you are exploring vintage games, building interactive projects, or studying emulation, ChippyCore provides a robust foundation for running CHIP-8 programs with ease.

With its modular design and support for configurable quirks, ChippyCore faithfully replicates the behavior of various CHIP-8 implementations while allowing you to adapt the emulator to your specific needs. This library integrates seamlessly with hardware or software environments and provides user-friendly callbacks for rendering, input handling, and screen updates.

---

## Overview
The **ChippyCore Library** is a CHIP-8 virtual machine emulator. It supports running CHIP-8 programs with customizable quirks and integrates callbacks for rendering, keyboard input, and screen updates.

---

## Public Methods

### 1. `play_game`
**Description**: Starts the emulation of a CHIP-8 program. This method initializes the emulator, loads the ROM, and executes the game loop.

**Parameters**:
- `data` (const `uint8_t*`): Pointer to the CHIP-8 ROM data.
- `dataSize` (size_t): Size of the ROM data.
- `drawPixelCallback` (`drawPixelCallback`): Callback for rendering individual pixels.
- `screenCallback` (`screenCallback`): Callback for handling screen updates.
- `keyCallback` (`keyCallback`): Callback for managing key states (press/release simulation).
- `config` (const `bool*`): Array to enable or disable specific quirks.

---

### 2. `stop`
**Description**: Immediately stops the emulation. Useful for clean termination or resetting the emulator.

---

### 3. `pause`
**Description**: Toggles the paused state of the emulator. While paused, the emulator halts execution cycles.

---

## Quirk Configuration
ChippyCore allows enabling/disabling specific quirks to match the behavior of different CHIP-8 variants.

| Quirk   | Description                                                                                       |
|---------|---------------------------------------------------------------------------------------------------|
| `QUIRK4` | **Reset VF register behavior:** Enabled resets VF to `0` before operations; disabled retains VF. |
| `QUIRK5` | **Shift behavior:** Enabled only shifts `vX`; disabled shifts `vX` and sets `vX` to `vY`.        |
| `QUIRK6` | **Sprite wrapping/clipping:** Enabled wraps sprites at screen borders; disabled clips them.      |
| `QUIRK11`| **I register increment:** Enabled increments `I` during certain instructions; disabled does not. |

The `config` array passed to `play_game` determines whether these quirks are enabled (`true`) or disabled (`false`).

---

## Examples

### Example 1: Starting the Emulator
```cpp
#include "ChippyCore.h"

// Define callbacks
void drawPixel(const uint16_t X, const uint16_t Y, bool& collision) {
    // Render a pixel on your screen
}

void updateScreen(bool clear, bool update) {
    if (clear) {
        // Clear the screen
    }
    if (update) {
        // Update the screen display
    }
}

// Simulate key presses/releases in the emulator
void handleKey(uint8_t& key, bool& state) {
    if (/* condition to press key */) {
        key = 1;    // Key index to press
        state = true; // Press key
    } else if (/* condition to release key */) {
        key = 1;    // Key index to release
        state = false; // Release key
    } else {
        key = -1;   // No key change
    }
}

// Initialize core and ROM data
ChippyCore core;
const uint8_t romData[] = { /* Your CHIP-8 ROM data */ };
bool config[4] = { true, false, true, false }; // Quirk configuration

void setup() {
    core.play_game(romData, sizeof(romData), drawPixel, updateScreen, handleKey, config);
}

void loop() {
    // Loop logic is handled internally by play_game
}
```

---

### Example 2: Stopping the Emulator
```cpp
void stopGame() {
    core.stop();
    Serial.println("Emulation stopped.");
}
```

---

### Example 3: Pausing and Resuming the Emulator
```cpp
void togglePause() {
    core.pause();
    Serial.println("Emulator paused or resumed.");
}
```

---

### Notes on `handleKey`
- The `key` parameter specifies the index of the key being simulated (0–15).
- The `state` parameter specifies whether the key is pressed (`true`) or released (`false`).
- To indicate no key press/release, set `key = -1`.

---

This README now includes an engaging introduction that highlights the purpose and versatility of the ChippyCore Library. Let me know if there’s anything else you'd like to tweak!