


#include "chippycore.h"
#include "roms.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
ChippyCore cc;
const int scaleFactor = 2;

bool quirkconfig[5] =  {   //Opcode variants
                      false, //  QUIRK4:  COSMAC based variants will reset VF  
                      false, //  QUIRK5:  CHIP-48/SCHIP-1.x don't set vX to vY, so only shift vX
                      false, //  QUIRK6:  CHIP-48/SCHIP-1.x don't set vX to vY, so only shift vX
                      false, // QUIRK11:  CHIP-48/SCHIP1.0 increment I only by X, SCHIP1.1 not at all
                  };

void drawPixelCallback(const uint16_t X, const uint16_t Y, bool& collision){
    // Perform collision detection on the original coordinates
    bool pixel_on = display.getPixel(X * scaleFactor, Y * scaleFactor);

    if (pixel_on) {
        collision = true;
    }

    // XOR the pixel state
    bool new_pixel_state = !pixel_on;

    // Draw a filled rectangle to represent the scaled pixel
    for (uint8_t i = 0; i < scaleFactor; i++) {
        for (uint8_t j = 0; j < scaleFactor; j++) {
            display.drawPixel(X * scaleFactor + i, Y * scaleFactor + j, new_pixel_state ? WHITE : BLACK);
        }
    }
}

void screenUpdateCallback(bool clearScreen, bool updateScreen){
    if(clearScreen){
        display.clearDisplay();
    }
    if(updateScreen){

        /* Added memory stats to flex :) */
        if(scaleFactor == 1){
            display.fillRect(0, 54, 128, 10, BLACK);
            display.fillRect(0, 44, 128, 10, BLACK);
            display.setTextSize(1);
            display.setTextColor(WHITE);
            display.setCursor(0, 54); // Position at the bottom-right
            display.print("Free Stack: ");
            display.println(uxTaskGetStackHighWaterMark(NULL));
            display.setCursor(0, 44); // Position at the bottom-right
            display.print("Free Heap: ");
            display.println(xPortGetFreeHeapSize());
        }
        display.display();
    }
}
void keyCallback(uint8_t& key, bool& key_state){
}

void setup() {
  // put your setup code here, to run once:
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    }

    delay(500);
    display.clearDisplay();
    display.display();
    delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  playGame(space_invaders, sizeof(space_invaders));
}

void playGame(const uint8_t* rom, size_t romSize){

  cc.play_game(rom, romSize, drawPixelCallback, screenUpdateCallback, keyCallback, quirkconfig);
}




