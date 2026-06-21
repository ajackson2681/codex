#pragma once

#include "bsp/board_api.h"
#include "tusb.h"
#include <cstdint>

namespace Keyboard 
{
    void ProcessInput(uint8_t keycode, bool shift, bool ctrl, bool alt);
};