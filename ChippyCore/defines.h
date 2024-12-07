#ifndef DEFINES_H
#define DEFINES_H
    //CONSTANTS
    #define RAM_SIZE 4096
    #define MAX_16 16
    #define MAX_8 8
    #define ROM_START_ADDRESS 0x200
    #define FONTSET_START_ADDRESS 0x50 
    
    //EMULATOR STATES AND CONTROLS
    #define START 1
    #define PAUSE 2
    #define CLEAR_DISPLAY 3
    #define SOUND_STATE 4

    //ERROR CODES
    #define ERROR_ROM_SIZE 1
    #define STACK_UNDERFLOW_ERROR 2
    #define ERROR_USER_KEYPRESS 3
    #define STACK_OVERFLOW_ERROR 4
#endif