#include "chippycore.h"

//#define USE_STACK

#ifndef USE_STACK
    ChippyCore cc;
#endif

// Callback function to draw a pixel on the screen with collision detection
void drawPixelCallback(const uint16_t X, const uint16_t Y, bool& collision){

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

void loopCallback(uint8_t& keySet, bool& keyState, bool& pause, bool& stop){
}

//There are some roms that needs those quirks.
//I do not know yet which ones. Every rom you load can have his own set of quirks.
bool default_quirkconfig[5] =  {   
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

void showFreeMemory(){
    UBaseType_t highWaterMark = uxTaskGetStackHighWaterMark(NULL); // Get the high-water mark for this task 
    Serial.print("Free stack space: "); 
    Serial.println(highWaterMark);
    Serial.print("Initial free heap: "); 
    Serial.println(xPortGetFreeHeapSize()); 
    Serial.print("Minimum ever free heap: "); 
    Serial.println(xPortGetMinimumEverFreeHeapSize());
}

void playGame(const uint8_t* rom, const size_t romSize, const bool* quirkconfig_game){
    #ifdef USE_STACK
        ChippyCore cc;
        cc.load_and_run(rom, romSize, &drawPixelCallback, &screenUpdateCallback, &loopCallback, quirkconfig_game);
        while(cc.isRunning()){
            cc.loop();
        }

    #else
        if(!cc.isRunning()){
            cc.load_and_run(rom, romSize, &drawPixelCallback, &screenUpdateCallback, &loopCallback, quirkconfig_game);
        }
        else{
            cc.loop();
        }
    #endif
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


