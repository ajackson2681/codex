#include <stdio.h>
#include "pico/stdlib.h"
#include "LiquidCrystal.hpp"
#include "bsp/board_api.h"
#include "tusb.h"
#include <functional>

#include "f_util.h"
#include "ff.h"

std::function<void(uint8_t)> kbCb = nullptr;

void tuh_mount_cb(uint8_t dev_addr) {
}

void tuh_umount_cb(uint8_t dev_addr) {
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


    // 40x4 dual-chip LCD
    LiquidCrystal lcd(
        21,            // RS
        LCD_PIN_NC,    // RW  -- tied to GND
        22,            // EN1 (chip 1, rows 0-1)
        20,            // EN2 (chip 2, rows 2-3) -- LCD_PIN_NC for single-chip
        14,            // D4
        12,            // D5
        15,            // D6
        13             // D7
    );
    lcd.begin(40, 4);
    lcd.blink();


    gpio_put(PICO_DEFAULT_LED_PIN, true);

    while (true) {
        tuh_task();
        if (buffer.isStale()) {
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
    } 
}
