#pragma once

#include "bsp/board_api.h"
#include "tusb.h"
#include <cstdint>

class Keyboard {
public:
    static void ProcessInput(uint8_t keycode, bool shift, bool ctrl);

private:
    static void CheckForEnter(uint8_t keycode);
    static void HandleAscii(uint8_t keycode, bool shift, bool ctrl);
};