#include "chippycore.h"

// Configuration for opcode quirks in the emulator
bool quirkconfig[5] =  {   
                      false, //  QUIRK4: COSMAC-based variants will reset the VF register to 0 before certain operations.
                      false, //  QUIRK5: CHIP-48/SCHIP-1.x don't set vX to vY when shifting, so only shift vX.
                      false, //  QUIRK6: Determine behavior for sprite wrapping (enabled) or clipping (disabled).
                      false, //  QUIRK11: CHIP-48/SCHIP1.0 increment the I register by X, SCHIP1.1 does not increment I.
                  };

// Callback function to draw a pixel on the screen with collision detection
void drawPixelCallback(const uint16_t X, const uint16_t Y, bool& collision){
    // Implement your pixel drawing and collision detection logic here
}


// Callback function to update the screen display
void screenUpdateCallback(bool clearScreen, bool updateScreen){
    if(clearScreen){
        // Implement logic to clear the screen display here
    }
    if(updateScreen){
        // Implement logic to update the screen display here
    }
}

// This callback is executed every 1ms to handle key input.
// Since the loop() is blocked when starting a game, add additional code here to prevent blocking.
// Example: key = 0x05 & key_state true : this presses the 5 key
//          key = 0x05 & key_state false : this releases the 5 key
void looCallback(uint8_t& key, bool& key_state){
    // Implement your key press and release logic here
}

void setup() {
    // Initialize any hardware or peripherals here
}

void loop() {
    // Example ROM data to be loaded
    const uint8_t ROM[] = {0x12, 0x25};

    // Start the game with the specified ROM
    playGame(ROM, sizeof(ROM));
}

// Function to start playing a game with the given ROM data
void playGame(const uint8_t* rom, size_t romSize){
    ChippyCore cc;
    cc.play_game(rom, romSize, drawPixelCallback, screenUpdateCallback, loopCallback, quirkconfig);
}
