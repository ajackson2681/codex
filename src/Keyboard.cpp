#include "Keyboard.hpp"
#include <functional>

extern std::function<void(uint8_t)> kbCb;

static uint8_t const keycode2ascii[128][2] = { HID_KEYCODE_TO_ASCII };

void Keyboard::ProcessInput(const hid_keyboard_report_t *report, uint8_t keyCode)
{

    // not existed in previous report means the current key is pressed
    bool const is_shift = report->modifier & (KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT);
    bool const is_ctrl = report->modifier & (KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_RIGHTCTRL);

    switch (keyCode) {
        case HID_KEY_ARROW_UP:
            break;
        case HID_KEY_ARROW_LEFT:
            break;
        case HID_KEY_ARROW_RIGHT:
            break;
        case HID_KEY_ARROW_DOWN:
            break;
    }

    // uint8_t ch = keycode2ascii[report->keycode[i]][is_shift ? 1 : 0];
    // if (kbCb) {
    //   kbCb(ch);
    // }

    // putchar(ch);
    // if ( ch == '\r' ) putchar('\n'); // added new line for enter key

    // #ifndef __ICCARM__ // TODO IAR doesn't support stream control ?
    // fflush(stdout); // flush right away, else nanolib will wait for newline
    // #endif
}