#include <stdio.h>
#include "pico/stdlib.h"
#include "LiquidCrystal.hpp"
#include "bsp/board_api.h"
#include "tusb.h"
#include <functional>
#include <GapBuffer.hpp>
#include "f_util.h"
#include "ff.h"
#include "Globals.hpp"
#include "Config.hpp"

#define FAKE_KEYS false

GapBuffer buffer;

bool shouldSave = false;
bool pressedEnter = false;
State writerState = State::STARTUP;
LiquidCrystal lcd(
    RS_PIN,            // RS
    LCD_PIN_NC,    // RW  -- tied to GND
    E1_PIN,            // EN1 (chip 1, rows 0-1)
    E2_PIN,            // EN2 (chip 2, rows 2-3) -- LCD_PIN_NC for single-chip
    D4_PIN,            // D4
    D5_PIN,            // D5
    D6_PIN,            // D6
    D7_PIN             // D7
);

void tuh_mount_cb(uint8_t dev_addr) {}

void tuh_umount_cb(uint8_t dev_addr) {}

void loadFile(FIL& fil)
{
    std::string path = "0:/document.txt";
    FRESULT fr = f_open(&fil, path.c_str(), FA_READ);

    if (fr != FR_OK) {
        return;
    }

    char c;
    UINT br;
    if (fr == FR_OK) {
        while (f_read(&fil, &c, 1, &br) == FR_OK && br > 0) {
            buffer.insert(c);
        }
        f_close(&fil);
    }
}

void saveFile(FIL& fil)
{
    std::string path = "0:/document.txt";
    FRESULT fr = f_open(&fil, path.c_str(), FA_WRITE);
    
    if (fr != FR_OK) {
        return;
    }
    
    fr = f_truncate(&fil); // remove all existing text, we want to overwrite it
    if (fr != FR_OK) {
        return;
    }
    
    for (int i = 0; i < buffer.totalChars(); i++) {
        f_putc(buffer.getCharAt(i), &fil);
    }

    fr = f_close(&fil);
    if (fr != FR_OK) {
        return;
    }

    shouldSave = false;

    gpio_put(PICO_DEFAULT_LED_PIN, true);
}

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

    FATFS fs;
    FIL fil;
    FILINFO fno;

    lcd.begin(40, 4);
    lcd.blink();
    lcd.write("CODEX v1.0.0\r");

    bool validDoc = false;

    FRESULT fr = f_mount(&fs, "0:", 1);
    if (fr == FR_OK) {
        lcd.write("SD Card Detected!\r");

        fr = f_stat("0:/document.txt", &fno);
        if (fr != FR_OK) {
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

    bool  doOnce = false;

    while (true) {
        tuh_task();
        if (writerState == State::STARTUP) {     
            continue;
        }
    
        if (!doOnce) {
            lcd.clear();
            
            sleep_ms(250);

            if (validDoc) {
                loadFile(fil);
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
            saveFile(fil);
        }
    } 

    f_unmount("0:");
}
