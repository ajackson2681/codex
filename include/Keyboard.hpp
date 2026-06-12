#pragma once

#include "bsp/board_api.h"
#include "tusb.h"
#include <cstdint>

class Keyboard {
public:
    static void ProcessInput(uint8_t keycode, bool shift, bool ctrl, bool alt);

private:
    static void StartupStateHandler(uint8_t keycode);
    static void DocSelectStateHandler(uint8_t keycode);
    static void WritingStateHandler(uint8_t keycode, bool shift, bool ctrl);
    static void HandleAscii(uint8_t keycode, bool shift, bool ctrl);
};