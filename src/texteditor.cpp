#include <stdio.h>
#include <functional>

#include "pico/stdlib.h"
#include "LiquidCrystal.hpp"
#include "bsp/board_api.h"
#include "tusb.h"

#include "GapBuffer.hpp"
#include "Globals.hpp"
#include "FileSystem.hpp"

void tuh_mount_cb(uint8_t dev_addr) {}

void tuh_umount_cb(uint8_t dev_addr) {}

int main()
{
    stdio_init_all();
    // init host stack on configured roothub port
    tuh_init(BOARD_TUH_RHPORT);
    if (board_init_after_tusb) {
        board_init_after_tusb();
    }
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    lcd.begin(40, 4);
    lcd.blink();
    lcd.write("CODEX v1.0.0\r");

    bool validDoc = false;

    if (FileSystem::init()) {
        lcd.write("SD Card Detected!\r");

        if (!FileSystem::fileExists("document.txt")) {
            lcd.write("No document detected!\r");
        }
        else {
            lcd.write("Document detected!\r");
            validDoc = true;
        }
    }
    else {
        lcd.write("No SD Card Detected!\r");
    }
    
    lcd.write("Press enter to continue.");


    gpio_put(PICO_DEFAULT_LED_PIN, true);

    bool doOnce = false;

    while (true) {
        tuh_task();
        if (writerState == State::STARTUP) {     
            continue;
        }
    
        if (!doOnce) {
            lcd.clear();
            
            sleep_ms(250);

            if (validDoc) {
                FileSystem::loadFile("document.txt", buffer);
            }
            else {
                lcd.setCursor(0,0);
            }

            doOnce = true;
        }

        if (buffer.isStale()) {
            gpio_put(PICO_DEFAULT_LED_PIN, false);
            
            auto b = buffer.getVisibleFrame();
            lcd.setCursor(0,0);
            for (int i = 0; i < ROW_COUNT; i++) {
                for (int j = 0; j < COL_COUNT; j++) {
                    char c = b[i][j];

                    switch (c) {
                        case '\n':
                        case '\r':
                        case '\0':
                            lcd.write(' ');
                            break;
                        default:
                            lcd.write(c);
                            break;
                    }
                }
            }
        }

        int row,col;
        buffer.getCursorPos(row,col);
        lcd.setCursor(col,row);

        if (shouldSave) {
            FileSystem::saveFile("document.txt", buffer);
            shouldSave = false;
            gpio_put(PICO_DEFAULT_LED_PIN, true);
        }
    } 

    FileSystem::uninit();
}
