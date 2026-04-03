#include <stdio.h>
#include "pico/stdlib.h"
#include "LiquidCrystal.hpp"
#include "bsp/board_api.h"
#include "tusb.h"
#include <functional>

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
        22,            // RS
        LCD_PIN_NC,   // RW  -- tied to GND
        26,            // EN1 (chip 1, rows 0-1)
        21,            // EN2 (chip 2, rows 2-3) -- LCD_PIN_NC for single-chip
        6,            // D4
        27,            // D5
        2,            // D6
        28             // D7
    );
    
    lcd.begin(40, 4);
    
    // // ---- Row 0 ----
    lcd.setCursor(0, 0);
    std::string f = "CODEX v1.0.0";
    for (char c : f) {
        lcd.write(c);
    }
    lcd.blink();
    
    kbCb = [&](uint8_t asciiCode) {
        lcd.write(asciiCode);
    };
    // p_lcd = &lcd;
    // lcd.write('a');
    // lcd.write('b');
    // lcd.write('c');
    // lcd.print("Pico SDK LiquidCrystal port");
 
    // // ---- Row 1 ----
    // lcd.setCursor(0, 1);
    // lcd.print("40x4 dual-HD44780 demo");
 
    // // ---- Row 2 (second chip) ----
    // lcd(0, 2) << "Rows 2-3 via EN2";
 
    // // ---- Row 3 ----
    // lcd(0, 3) << "Counter: ";
 
    // // ---- Custom character: smiley ----
    // const uint8_t smiley[8] = {
    //     0b00000,
    //     0b01010,
    //     0b01010,
    //     0b00000,
    //     0b10001,
    //     0b01110,
    //     0b00000,
    //     0b00000,
    // };
    // lcd.createChar(0, smiley);
    // lcd.setCursor(39, 0);
    // lcd.write(0x00);
    // lcd.setCursor(0,3);
    // // lcd.print("test");
    // lcd.blink();

    gpio_put(PICO_DEFAULT_LED_PIN, true);
    // // ---- Counter loop ----
    // uint32_t count = 0;
    // tusb_init();
    while (true) {
        tuh_task();
        // lcd.setCursor(9, 3);
        // lcd.print(count++);
        // sleep_ms(500);
    }
}
