#include "chippycore.h"

#define ROM_START_ADDRESS 0x200
#define FONTSET_START_ADDRESS 0x50 
#define BITMASK_X 0x0F00

//ERROR CODES
#define ERROR_ROM_SIZE 1
#define ERROR_USER_KEYPRESS 3

void ChippyCore::handleError(uint8_t errorCode, bool debug){
  if(debug){
    switch(errorCode){
        case ERROR_ROM_SIZE:
            Serial.println("ERROR: The rom size is too big");
        break;
        case ERROR_USER_KEYPRESS:
            Serial.println("ERROR: You can only choose a keypress value 0-15 in your loopback");
        break;
        default:
        break;
    }
  }
  delay(2000);
}

bool ChippyCore::isRunning(){
    return flag.get(START);
}

void ChippyCore::load_and_run(const uint8_t* data, size_t dataSize, drawPixelCallback dwpcb, screenCallback sccb, keyCallback kkcb, const bool* config, bool debug){
    if(dwpcb){
        dpcb = dwpcb;
    }
    if(sccb){
        scb = sccb;
    }
    if(kkcb){
        kcb = kkcb;
    }
    initialize();
    flag.set(QUIRK4, config[0]);
    flag.set(QUIRK5, config[1]);
    flag.set(QUIRK6, config[2]);
    flag.set(QUIRK11, config[3]);
    uint8_t error = load_rom(data, dataSize);
    if(error){
        handleError(error,debug);
        flag.set(START, false);
        return;
    }
    flag.set(START,true);      
}
void ChippyCore::loop(bool debug){
    if(flag.get(START)){
        uint32_t currentInterruptCycle = millis();
        if((currentInterruptCycle - last_interrupt_cycle) >= (1000/750)){
            //loop callback with keypad and sound arguments
            uint8_t key = 255;
            bool key_state = false;
                
            bool pause = flag.get(PAUSE);
            bool stop = false;
            kcb(key,key_state,pause,stop);

            //if pause state changed, update the flag and call screen callback with new pause state.
            if(flag.get(PAUSE) != pause){
                flag.set(PAUSE, pause);
            }
            //if stop state changed, update the flag and break from loop.
            if(stop){
                flag.set(START, false);
            }
            if (key != 255) {
              if ((key < 0 || key > 15) && key != 255) {
                  handleError(ERROR_USER_KEYPRESS, debug);
                  flag.set(START, false);
                  return;
              }
              keys.set(key, key_state);
          }

                
            last_interrupt_cycle = currentInterruptCycle;
        }
        if(!flag.get(PAUSE)){
            cycle();
        }  
    }
}
void ChippyCore::initialize(){
    PC = ROM_START_ADDRESS; 
    INDEX = 0;
    SP = 0; 
    DELAYTIMER = 0; 
    SOUNDTIMER = 0;
    //SpriteCount = 0;
    memset(V,0,sizeof(V));
    memset(STACK,0,sizeof(STACK));
    memset(RAM, 0, sizeof(RAM));
    last_cpu_cycle = 0;
    last_gpu_cycle = 0;
    last_interrupt_cycle = 0;
    load_fontset();
}
uint8_t ChippyCore::load_rom(const uint8_t* data, size_t dataSize){
    if (dataSize > (RAM_SIZE - ROM_START_ADDRESS)) {
        return ERROR_ROM_SIZE;
    }
    memcpy(RAM + ROM_START_ADDRESS, data, dataSize);
    return 0;
}
void ChippyCore::load_fontset(){
    const uint8_t FONTSET[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    memcpy(RAM + FONTSET_START_ADDRESS, FONTSET, sizeof(FONTSET));
}
void ChippyCore::cycle(){

    uint32_t currentCpuCycle = millis();
    if((currentCpuCycle - last_cpu_cycle) >= (1000/500)){
        executeOpcode();
        last_cpu_cycle = currentCpuCycle;
    }
    uint32_t currentGpuCycle = millis();
    if((currentGpuCycle - last_gpu_cycle) >= (1000/60)){
        if(DELAYTIMER > 0){
            DELAYTIMER--;
        }

        if(SOUNDTIMER >= 0){
          if(SOUNDTIMER == 0){
                if(flag.get(SOUND_STATE)){
                    flag.set(SOUND_STATE,false);
                }
          }
          if(!flag.get(SOUND_STATE)){
                flag.set(SOUND_STATE,true);
          }
          SOUNDTIMER--;
        }
        if(flag.get(CLEAR_DISPLAY)){
          flag.set(CLEAR_DISPLAY,false);
          if(scb){
              scb(1,0);
          }
        }

        last_gpu_cycle = currentGpuCycle;
    }

}
bool ChippyCore::is_key_pressed(uint8_t key) {
    return key < 16 && keys.get(key) == 1; // Return true if key is within range and pressed
}
int8_t ChippyCore::get_pressed_key() {
    for (uint8_t key = 0; key < 16; key++) {
        if (keys.get(key) == 1) {
            return key; // Return the key value if pressed
        }
    }
    return -1; // Return -1 if no key is pressed
}
void ChippyCore::set_key_state(uint8_t key, bool is_pressed){
    if (key < 16) { 
        keys.set(key, is_pressed);
    }
}
void ChippyCore::executeOpcode() {
    uint16_t OPCODE = (RAM[PC] << 8) | RAM[PC + 1];
    switch (OPCODE & 0xF000) {
        case 0x0000:
            switch (OPCODE & 0x00FF) {
                case 0xE0:
                    // 00E0: Clear the display
                    flag.set(CLEAR_DISPLAY, true);
                    PC += 2;
                break;
                case 0xEE:
                    // 00EE: Return from subroutine
                    PC = STACK[--SP];
                break;
                default:
                break;
            }
        break;
        case 0x1000:
            // 1NNN: Jump to address NNN
            PC = OPCODE & 0x0FFF;
        break;
        case 0x2000:
            // 2NNN: Call subroutine at NNN
            if (SP < 16) {
                STACK[SP++] = PC + 2; // Push the current PC onto the stack
                PC = OPCODE & 0x0FFF;         // Jump to the address specified by the opcode
            } 
            else {
                PC += 2; // If stack is full, increment PC to skip this instruction
            }
        break;
        case 0x3000:
            // 3XNN: Skip next instruction if Vx equals NN
            if (V[(OPCODE & 0x0F00) >> 8] == (OPCODE & 0x00FF)) {
                PC += 4; // Skip next instruction
            } 
            else {
                PC += 2;
            }
        break;
        case 0x4000:
            // 4XNN: Skip next instruction if Vx does not equal NN
            if (V[(OPCODE & 0x0F00) >> 8] != (OPCODE & 0x00FF)) {
                PC += 4; // Skip next instruction
            } 
            else {
                PC += 2;
            }
        break;
        case 0x5000:
            // 5XY0: Skip next instruction if Vx equals Vy
            if (V[(OPCODE & 0x0F00) >> 8] == V[(OPCODE & 0x00F0) >> 4]) {
                PC += 4;
            } 
            else {
                PC += 2;
            }
        break;
        case 0x6000:
            // 6XNN: Set Vx = NN
            V[(OPCODE & 0x0F00) >> 8] = (OPCODE & 0x00FF);
            PC += 2;
        break;
        case 0x7000:
            // 7XNN: Set Vx = Vx + NN
            V[(OPCODE & 0x0F00) >> 8] += (OPCODE & 0x00FF);
            PC += 2;
        break;
        case 0x8000: {
            // 8XYN: Various arithmetic and logical operations
            uint8_t Y = (OPCODE & 0x00F0) >> 4;
            switch (OPCODE & 0x000F) {
                case 0x0:
                    // 8XY0: Set Vx = Vy
                    V[(OPCODE & 0x0F00) >> 8] = V[Y];
                    PC += 2;
                break;
                case 0x1:
                    // 8XY1: Set Vx = Vx OR Vy
                    V[(OPCODE & 0x0F00) >> 8] |= V[Y];
                    if(flag.get(QUIRK4)){
                        V[0xF] = 0;
                    }
                    PC += 2;
                break;
                case 0x2:
                    // 8XY2: Set Vx = Vx AND Vy
                    V[(OPCODE & 0x0F00) >> 8] &= V[Y];
                    if(flag.get(QUIRK4)){
                        V[0xF] = 0;
                    }
                    PC += 2;
                break;
                case 0x3:
                
                    // 8XY3: Set Vx = Vx XOR Vy
                    V[(OPCODE & 0x0F00) >> 8] ^= V[Y];
                    if(flag.get(QUIRK4)){
                        V[0xF] = 0;
                    }                    
                    PC += 2;
                break;
                case 0x4: {
                    // 8XY4: Set Vx = Vx + Vy, set VF = carry
                    uint16_t sum = V[(OPCODE & 0x0F00) >> 8] + V[Y];
                    V[0xF] = (sum > 0xFF) ? 1 : 0;
                    V[(OPCODE & 0x0F00) >> 8] = sum & 0xFF;
                    PC += 2;
                } 
                break;
                case 0x5:
                    // 8XY5: Set Vx = Vx - Vy, set VF = NOT borrow
                    V[0xF] = (V[(OPCODE & 0x0F00) >> 8] > V[Y]) ? 1 : 0;
                    V[(OPCODE & 0x0F00) >> 8] -= V[Y];
                    PC += 2;
                break;
                case 0x6:
                    // 8XY6: Set Vx = Vx SHR 1, set VF = least significant bit before shift
                    if(flag.get(QUIRK5)){
                        V[0xF] = (V[(OPCODE & 0x0F00) >> 8] & 0x1);
                        V[(OPCODE & 0x0F00) >> 8] >>= 1;
                    }
                    else{
                          V[0xF] = V[Y] & 0x1;
                          V[(OPCODE & 0x0F00) >> 8] = V[Y] >> 1;
                    }
                    PC += 2;
                break;
                case 0x7:
                    // 8XY7: Set Vx = Vy - Vx, set VF = NOT borrow
                    V[0xF] = (V[Y] > V[(OPCODE & 0x0F00) >> 8]) ? 1 : 0;
                    V[(OPCODE & 0x0F00) >> 8] = V[Y] - V[(OPCODE & 0x0F00) >> 8];
                    PC += 2;
                break;
                case 0xE:
                    // 8XYE: Set Vx = Vx SHL 1, set VF = most significant bit before shift
                    if(flag.get(QUIRK5)){
                        V[0xF] = (V[(OPCODE & 0x0F00) >> 8] & 0x80) ? 1 : 0;
                        V[(OPCODE & 0x0F00) >> 8] <<= 1;
                    }
                    else{
                        V[0xF] = (V[Y] & 0x80) ? 1 : 0;
                        V[(OPCODE & 0x0F00) >> 8] = V[Y] << 1;
                    }
                    PC += 2;
                break;
                default:
                
                break;
            }
        } 
        break;
        case 0x9000:
            // 9XY0: Skip next instruction if Vx != Vy
            if (V[(OPCODE & BITMASK_X) >> MAX_8] != V[(OPCODE & 0x00F0) >> 4]) {
                PC += 4;
            } 
            else {
                PC += 2;
            }
        break;
        case 0xA000:
            // ANNN: Set I = NNN
            INDEX = (OPCODE & 0x0FFF);
            PC += 2;
        break;
        case 0xB000:
            // BNNN: Jump to address NNN + V0
            PC = ((OPCODE & 0x0FFF) + V[0]);
        break;
        case 0xC000:
                // CXNN: Set Vx = random byte AND NN
                V[(OPCODE & BITMASK_X) >> MAX_8] = ((esp_random() & 0xFF) & (OPCODE & 0x00FF));
                PC += 2;
        break;
        case 0xD000: {
            V[0xF] = 0; 
            //SpriteLocationData[SpriteCount] = packSpriteLocationData((V[(OPCODE >> MAX_8) & 0x0F] % 64), (OPCODE & 0x000F), (V[(OPCODE >> 4) & 0x0F] % 32));
            for(uint8_t row = 0;row < (OPCODE & 0x000F); row++){
                for (uint8_t col = 0; col < 8; col++) {
                    if (((RAM[INDEX + row]) & (0x80 >> col)) != 0) {
                        
                          
                        uint8_t x = V[(OPCODE & 0x0F00) >> 8] + col;
                        uint8_t y = V[(OPCODE & 0x00F0) >> 4] + row;
                        if (flag.get(QUIRK6)) {
                            x %= 64;
                            y %= 32;
                        }
                        else if (x >= 64 || y >= 32) {
                            continue;
                        }   
                        bool collision = false;
                        if (dpcb) {   
                            dpcb(x, y, collision);
                        }
                        if (collision) {
                            V[0xF] = 1;
                        }
                    }
                }
            }
            if (scb) {
                scb(false, true);
            }
            PC += 2;
        } 
        break;
        case 0xE000: {
            // EX9E and EXA1: Key operations
            switch (OPCODE & 0x00FF) {
                case 0x9E:
                    if (is_key_pressed(V[(OPCODE & 0x0F00) >> MAX_8])) {
                        PC += 4;
                    } 
                    else {
                        PC += 2;
                    }
                break;
                case 0xA1:
                    if (!is_key_pressed(V[(OPCODE & 0x0F00) >> MAX_8])) {
                        PC += 4;
                    } 
                    else {
                        PC += 2;
                    }
                break;
                default:
                    PC += 2;
                break;
            }
        } 
        break;
        case 0xF000: {
            uint8_t X = (OPCODE & 0x0F00) >> MAX_8;
            switch (OPCODE & 0x00FF) {
                case 0x07:
                    V[(OPCODE & 0x0F00) >> MAX_8] = DELAYTIMER;
                    PC += 2;
                break;
                case 0x15:
                    // FX15: Set delay timer = Vx
                    DELAYTIMER = V[(OPCODE & 0x0F00) >> MAX_8];
                    PC += 2;
                break;
                case 0x18:
                    // FX18: Set sound timer = Vx
                    SOUNDTIMER = V[(OPCODE & 0x0F00) >> MAX_8];
                    PC += 2;
                break;
                case 0x0A: {
                  int8_t pressedKey = get_pressed_key();
                    // FX0A: Wait for a key press, then store the value of the key in Vx
                    if (pressedKey != -1) {
                        V[(OPCODE & 0x0F00) >> MAX_8] = pressedKey;
                            PC += 2;
                    }  
                } 
                break;
                case 0x1E:
                    // FX1E: Set I = I + Vx, set VF = carry
                    INDEX += V[(OPCODE & 0x0F00) >> MAX_8];
                    V[0xF] = (INDEX > 0xFFF) ? 1 : 0;
                    INDEX &= 0xFFF;
                    PC += 2;
                break;
                case 0x29:
                    // FX29: Set I = location of sprite for digit Vx
                    INDEX = 0x50 + (V[(OPCODE & 0x0F00) >> MAX_8] * 5);
                    PC += 2;
                break;
                case 0x30:
                    // FX30: Set I = location of 10-byte font sprite for digit Vx (SCHIP)
                    INDEX = 0xA0 + (V[(OPCODE & 0x0F00) >> MAX_8] * 10);
                    PC += 2;
                    break;
                case 0x33:
                    // FX33: Store BCD representation of Vx in memory locations I, I+1, and I+2
                    RAM[INDEX] = V[(OPCODE & 0x0F00) >> MAX_8] / 100;
                    RAM[INDEX + 1] = (V[(OPCODE & 0x0F00) >> MAX_8] / 10) % 10;
                    RAM[INDEX + 2] = V[(OPCODE & 0x0F00) >> MAX_8] % 10;
                    PC += 2;
                break;
                case 0x55:
                    // FX55: Store registers V0 through Vx in memory starting at location I
                    for (uint8_t reg1 = 0; reg1 <= X; ++reg1) {
                        RAM[INDEX + reg1] = V[reg1];
                    }
                    if(flag.get(QUIRK11)){
                        INDEX += X + 1;
                    }
                    PC += 2;
                break;
                case 0x65:
                    // FX65: Read registers V0 through Vx from memory starting at location I
                    for (uint8_t reg1 = 0; reg1 <= X; ++reg1) {
                        V[reg1] = RAM[INDEX + reg1];
                    }
                    if(flag.get(QUIRK11)){
                        INDEX += X + 1;
                    }
                    PC += 2;
                break;
                default:
                break;
            }
        } 
        break;
        default:
        break;
    }
}
