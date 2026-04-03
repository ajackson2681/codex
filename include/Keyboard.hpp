#pragma once

#include "bsp/board_api.h"
#include "tusb.h"
#include <cstdint>

class Keyboard {
public:
    static void ProcessInput(const hid_keyboard_report_t *report, uint8_t keyCode);
};