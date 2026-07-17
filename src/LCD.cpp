#include "LCD.hpp"
#include "Config.hpp"

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
    if (row == 0 || row == 1) {
        return e1;
    }
    else if (row == 2 || row == 3) {
        return e2;
    }

    // if row is somehow not in the valid range, just default to first chip 
    return e1;
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

    sendByte(Command::TWO_LINE_MODE, false, en);

    // no idea why, but the spec says to send this again
    sendByte(Command::TWO_LINE_MODE, false, en);

    // start with cursor off, we might only want to display information, and
    // not have input
    sendByte(Command::CURSOR_OFF, false, en);

    // sequence says to send clear display here
    sendByte(Command::CLEAR_DISPLAY, false, en, MS_TO_US(2));

    // finally, set the entry mode, which we're just defaulting to always
    // being increment (left to right) and no shift (screen stays fixed)
    // adding a 10ms delay here just to be safe since I was seeing sometimes
    // garbled characters
    sendByte(Command::SET_ENTRY_MODE, false, en, MS_TO_US(10));
    
    registerCustomChars(en);
}

void LCD::registerCustomChars(uint8_t en)
{
    sendByte(Command::SET_CGRAM_ADDRESS(CustomChar::RIGHT_ARROW), false, en);
    for (int i = 0; i < 8; ++i) {
        sendByte(CustomCharMaps::RIGHT_ARROW[i], true, en);
    }

    sendByte(Command::SET_CGRAM_ADDRESS(CustomChar::LEFT_ARROW), false, en);
    for (int i = 0; i < 8; ++i) {
        sendByte(CustomCharMaps::LEFT_ARROW[i], true, en);
    }

    sendByte(Command::SET_CGRAM_ADDRESS(CustomChar::UP_ARROW), false, en);
    for (int i = 0; i < 8; ++i) {
        sendByte(CustomCharMaps::UP_ARROW[i], true, en);
    }

    sendByte(Command::SET_CGRAM_ADDRESS(CustomChar::DOWN_ARROW), false, en);
    for (int i = 0; i < 8; ++i) {
        sendByte(CustomCharMaps::DOWN_ARROW[i], true, en);
    }
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
    // spec says clear display takes 1.52ms, so we'll wait 2ms just to be safe
    sendByte(Command::CLEAR_DISPLAY, false, e1, MS_TO_US(2)); 
    sendByte(Command::CLEAR_DISPLAY, false, e2, MS_TO_US(2)); 

    // after clearing, move the cursor to 0,0
    setCursorPos(0, 0); 
}


void LCD::sendByte(uint8_t value, bool isData, uint8_t en, uint32_t delayUsAfter) {
    bool prevRs = gpio_get(rs);
    
    gpio_put(rs, isData); // isData = true if writing a character, or false if issuing a command

    int highNibble = (value >> 4) & 0xf;
    int lowNibble = value & 0xf;

    writeNibble(highNibble);
    pulse(en);

    writeNibble(lowNibble);
    pulse(en);

    gpio_put(rs, prevRs); // restore previous RS state, likely to be low

    if (isData) { // only increment cursor if we're writing data, not if we're sending a command
        incrementCursor();
    }
    
    sleep_us(delayUsAfter);
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

    clampCursorPos(row_, col_);

    // if we're switching from rows 0 & 1 to 2 & 3 OR vice versa, we're switching
    // chips and need to disable the cursor on the chip we're leaving
    bool switchedChips = chipsAreSwitching(row, row_);
    
    row = row_;
    col = col_;
    
    uint8_t curEnable = currentEnable();

    // set DDRAM address to move cursor to correct position
    sendByte(Command::SET_DDRAM_ADDRESS(row, col), false, curEnable);
    
    if (switchedChips) {
        uint8_t otherEnable = (curEnable == e1) ? e2 : e1;
        
        // reset the other display to its 0,0 if we switched chips, otherwise 
        // weird stuff happens
        sendByte(Command::SET_DDRAM_ADDRESS(0,0), false, otherEnable);

        // if we're not switching chips, we don't want to mess with the cursor settings
        // or else we get a weird flickering artifact.
        if (cursorEnabled) {
            enableCursor(); // enable the current chip's cursor (also disables the 
            // other chip's cursor)
        }
    }
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
    int lRow = row;
    int lCol = col;

    if (c != '\n') {
        sendByte(c, true, currentEnable());
    }
    else {
        if (++lRow >= ROW_COUNT) {
            lRow = 0;
        }
        lCol = 0;
        
        setCursorPos(lRow,lCol);
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
    sendByte(Command::CURSOR_ON, false, curEnable);
    sendByte(Command::CURSOR_OFF, false, otherEnable);
}

void LCD::disableCursor() {
    cursorEnabled = false;
    // both of these can just be set to off, since it doesn't matter which one
    // is the "current" enable.
    sendByte(Command::CURSOR_OFF, false, e1);
    sendByte(Command::CURSOR_OFF, false, e2);
}

void LCD::incrementCursor() {
    if (++col >= COL_COUNT) {
        col = 0;
        if (++row >= ROW_COUNT) {
            row = 0;
        }
    }
}

void LCD::clampCursorPos(uint8_t& row_, uint8_t& col_)
{
    // weird potential edge case where a negative number gets sent here. Since
    // uint8_t can't ever be negative, it will be interpreted as a positive 
    // number from 128 to 255. In this case, we clamp to 0 instead of the row
    // count
    if (row_ > INT8_MAX) {
        row_ = 0;
    }
    else if (row_ > ROW_COUNT - 1) {
        row_ = ROW_COUNT - 1;
    }

    if (col_ > INT8_MAX) {
        col_ = 0;
    }
    else if (col_ > COL_COUNT - 1) {
        col_ = COL_COUNT - 1;
    }
}

bool LCD::chipsAreSwitching(uint8_t curRow_, uint8_t newRow_) 
{
    bool e1Toe2 = (curRow_ == 0 || curRow_ == 1) &&
                  (newRow_ == 2 || newRow_ == 3);

    bool e2Toe1 = (curRow_ == 2 || curRow_ == 3) &&
                  (newRow_ == 0 || newRow_ == 1);

    return e1Toe2 ^ e2Toe1; // can't be both, but use XOR just to be safe
}