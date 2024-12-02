#ifndef CHIPPYCORE_H
#define CHIPPYCORE_H

#include <Arduino.h>
#include "BitVault.h"

#define RAM_SIZE 4096
#define MAX_16 16
#define MAX_8 8


#define IS_RUNNING 0
#define START 1
#define PAUSE 2
#define CLEAR_DISPLAY 3

///***********************************************************************************************///
///                                       EMULATOR QUIRKS                                         ///
///                                                                                               /// 
/////////////////////////////////////////////////////////////////////////////////////////////////////
#define QUIRK4 4    // COSMAC-based variants reset VF register to 0 before certain operations. 
                    // Enabled: VF is reset to 0 before instructions that modify it. 
                    // Disabled: VF retains its previous value before such instructions. 

#define QUIRK5 5    // CHIP-48/SCHIP-1.x do not set vX to vY when shifting, so only shift vX.

#define QUIRK6 6    // Wrapping or clipping behavior for sprites. 
                    // Enabled: Sprites wrap around the screen borders. 
                    // Disabled: Sprites are clipped at the screen edges. 

#define QUIRK11 7   // Increment behavior for the I register. 
                    // Enabled: For CHIP-48/SCHIP-1.0, I is incremented by the value of X. For SCHIP-1.1, I is not incremented. 
                    // Disabled: I is not incremented in either case.

//The ChippyCore Class
class ChippyCore{
    public:
    
        //Define Callbacks
        typedef void (*screenCallback)(bool clearScreen, bool updateScreen);
        typedef void (*keyCallback)(uint8_t& key_set, bool& key_state);
        typedef void (*drawPixelCallback)(const uint16_t X, const uint16_t Y, bool& collisionDetection);

        //Method
        void play_game(const uint8_t* data, size_t dataSize, drawPixelCallback callback, screenCallback callback1, keyCallback callback2,const bool* config);
        void stop();
        void pause();
        
    private:
        //Ram
        uint8_t RAM[RAM_SIZE];

        //Registers
        uint16_t PC; 
        uint16_t INDEX;
        alignas(2) uint16_t STACK[MAX_16];
        uint8_t SP; 
        uint8_t DELAYTIMER; 
        uint8_t SOUNDTIMER;
        uint8_t V[MAX_16];
        
        //Define Callbacks
        drawPixelCallback dpcb;
        screenCallback scb;
        keyCallback kcb;

        //Old cycle time variables 
        uint32_t last_cpu_cycle;  ///< Timestamp of the last CPU cycle
        uint32_t last_gpu_cycle;  ///< Timestamp of the last GPU cycle
        uint32_t last_interrupt_cycle;  ///< Timestamp of the last INTERRUPT cycle

        //Bit containers for state flags and keypad keys
        BitVault<uint16_t,true> flag;
        BitVault<uint16_t,true> keys;

        //Methods
        void initialize();
        void load_rom(const uint8_t* data, size_t dataSize);
        void load_fontset();
        void executeOpcode();
        bool is_key_pressed(uint8_t key);
        int8_t get_pressed_key();
        void cycle();
        void set_key_state(uint8_t key, bool is_pressed);
};
#endif