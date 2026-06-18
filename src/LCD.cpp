#include "LCD.hpp"

#include <pico/stdlib.h>

LCD::LCD(uint8_t rs_, uint8_t e1_, uint8_t e2_,
         uint8_t d4_, uint8_t d5_, uint8_t d6_, uint8_t d7_)
    : row(0)
    , col(0)
    , rs(rs_)
    , e1(e1_)
    , e2(e2_)
    , d4(d4_)
    , d5(d5_)
    , d6(d6_)
    , d7(d7_)
{
    setOutput(rs);
    setOutput(e1);
    setOutput(e2);
    setOutput(d4);
    setOutput(d5);
    setOutput(d6);
    setOutput(d7);
}

uint8_t LCD::currentEnable() {
    if (row < 2) {
        return e1;
    }
    else {
        return e2;
    }
}

void LCD::initialize(uint8_t en)
{
    // page 28 of the datasheet specifies the initialization sequence

    // spec says to wait for more than 40ms, so we'll just do 45
    sleep_ms(45);
    
    writeNibble(0x3); // init command, not listed in their command table, but
    // mentioned in the init sequence on page 28
    pulse(en);
    
    // spec says wait for more than 37us, so wait 40
    sleep_us(DEFAULT_DELAY); 

    sendCommand(Command::TWO_LINE_MODE, en, DEFAULT_DELAY);

    // no idea why, but the spec says to send this again
    sendCommand(Command::TWO_LINE_MODE, en, DEFAULT_DELAY);

    // start with cursor off, we might only want to display information, and
    // not have input
    sendCommand(Command::CURSOR_OFF, en, DEFAULT_DELAY);

    // sequence says to send clear display here
    sendCommand(Command::CLEAR_DISPLAY, en, MS_TO_US(2));

    // finally, set the entry mode, which we're just defaulting to always
    // being increment (left to right) and no shift (screen stays fixed)
    sendCommand(Command::SET_ENTRY_MODE, en, MS_TO_US(10));
}


void LCD::initialize()
{
    initialize(e1);
    initialize(e2);

    setCursorPos(0, 0); // start with cursor at 0,0
}

void LCD::setOutput(uint8_t pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, 0);
}

void LCD::clear()
{
    // send to both, since we want the whole display cleared
    sendCommand(Command::CLEAR_DISPLAY, e1, 0); // don't need to delay on the first one, since the
    // second command is on a different chip
    sendCommand(Command::CLEAR_DISPLAY, e2, MS_TO_US(2)); // spec says clear display takes 1.52ms, so we'll wait 2ms
    // setCursorPos(0, 0); TODO: is this needed?
}

void LCD::sendCommand(uint8_t cmd, uint8_t en, uint32_t delayUsAfter) {

    // rs needs to be low to send a command
    gpio_put(rs, false);
    
    uint8_t highNibble = (cmd >> 4) & 0xf;
    uint8_t lowNibble = cmd & 0xf;

    writeNibble(highNibble);
    pulse(en);

    writeNibble(lowNibble);
    pulse(en);

    if (delayUsAfter > 0) {
        sleep_us(delayUsAfter);
    }
}

void LCD::writeNibble(uint8_t value)
{
    gpio_put(d4, (value >> 0) & 0x1);
    gpio_put(d5, (value >> 1) & 0x1);
    gpio_put(d6, (value >> 2) & 0x1);
    gpio_put(d7, (value >> 3) & 0x1);
}

void LCD::pulse(uint8_t pin) {
    gpio_put(pin, true);
    sleep_us(1); // pulse needs to be >450ns, so 1us is safe
    gpio_put(pin, false);
}

void LCD::setCursorPos(uint8_t row_, uint8_t col_) {
    row = row_;
    col = col_;
    
    uint8_t curEnable = currentEnable();

    // set DDRAM address to move cursor to correct position
    sendCommand(Command::SET_DDRAM_ADDRESS(row, col), curEnable, DEFAULT_DELAY);
}

void LCD::setCursorCol(uint8_t col_) {
    setCursorPos(row, col_);
}

void LCD::setCursorRow(uint8_t row_) {
    setCursorPos(row_, col);
}

void LCD::getCursorPos(uint8_t& row_, uint8_t& col_) {
    row_ = row;
    col_ = col;
}

uint8_t LCD::getCursorCol() {
    return col;
}

uint8_t LCD::getCursorRow() {
    return row;
}

void LCD::write(char c)
{
    if (c != '\r') {
        bool prevRs = gpio_get(rs);
        
        gpio_put(rs, true); // set RS high to write data (low is for commands)
    
        uint8_t curEnable = currentEnable();
    
        int highNibble = (c >> 4) & 0xf;
        int lowNibble = c & 0xf;
    
        writeNibble(highNibble);
        pulse(curEnable);
    
        writeNibble(lowNibble);
        pulse(curEnable);

        gpio_put(rs, prevRs); // restore previous RS state, likely to be low

        incrementCursor();
        
        sleep_us(DEFAULT_DELAY);
    }
    else {
        if (++row >= 4) {
            row = 0;
        }
        col = 0;
        
        setCursorPos(row,col);
    }

}

void LCD::write(const std::string& str)
{
    for (char c : str) {
        write(c);
    }
}

void LCD::enableCursor() {
    cursorEnabled = true;
    uint8_t curEnable = currentEnable();
    uint8_t otherEnable = (curEnable == e1) ? e2 : e1;
    sendCommand(Command::CURSOR_ON, curEnable, DEFAULT_DELAY);
    sendCommand(Command::CURSOR_OFF, otherEnable, DEFAULT_DELAY);
}

void LCD::disableCursor() {
    cursorEnabled = false;
    sendCommand(Command::CURSOR_OFF, e1, DEFAULT_DELAY);
    sendCommand(Command::CURSOR_OFF, e2, DEFAULT_DELAY);
}

void LCD::incrementCursor() {
    if (++col >= 40) {
        col = 0;
        if (++row >= 4) {
            row = 0;
        }
    }
}