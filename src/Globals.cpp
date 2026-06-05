#include "Globals.hpp"
#include "Config.hpp"

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
