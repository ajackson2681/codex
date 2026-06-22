#pragma once

#include <cstdint>
#include <string>

#define MS_TO_US(x) ((x) * 1000)

#define DEFAULT_DELAY 40

// datasheet reference is located under datasheets directory in this repo

// taken from page 25
// 1. Display clear
// 2. Function set:
//     DL = 1; 8-bit interface data
//     N = 0; 1-line display
//     F = 0; 5x8 dot character font
// 3. Display on/off control:
//     D = 0; Display off
//     C = 0; Cursor off
//     B = 0; Blinking off
// 4. Entry mode set:
//     I/D = 1; Increment by 1
//     S = 0; No shift 

class Command 
{
public:
    static constexpr uint8_t CLEAR_DISPLAY   = 0b0000'0001;
    
    // 4-bit mode, 2 line mode, 5x8 character resolution, which is always what
    // we're using, so this just makes it so we don't have to do any bitwise
    // or operations to set the parameters for those things in the command.
    static constexpr uint8_t TWO_LINE_MODE   = 0b0010'1000; 

    // entry mode set with increment and no shift, which is always what we're using
    // so again, this just makes it so we don't have to do any bitwise or 
    // operations to set the parameters for those things in the command.
    static constexpr uint8_t SET_ENTRY_MODE  = 0b0000'0110;

    // for these, we just always assume display on (bit 2 = high, bits 0 & 1 are
    // the cursor and blinking bits respectively, which we can just set to 0 or 1).
    // Additionally, we just assume the cursor is always blinking if it's on.
    // NOTE: I found out that the cursor bit (bit 1) and the blink bit (bit 0)
    // are two entirely separate things. If you enable both of them, you'll have
    // the underline cursor and the blinking block cursor both, which looks weird
    // so this is ACTUALLY only setting the blink bit.
    static constexpr uint8_t CURSOR_ON       = 0b0000'1101;
    static constexpr uint8_t CURSOR_OFF      = 0b0000'1100;
    
    // this is a helper function where we just pass in the row and column we
    // want it it returns the correct command with the address based on the row
    // and column.
    static uint8_t SET_DDRAM_ADDRESS(uint8_t row, uint8_t col) {
        return SET_DDRAM_ADDR | (ROW_OFFSETS[row] + col);
    }
private:
    // two controllers each use the same offsets, but this just makes it so we
    // can easily index into the correct one based on the row number
    static constexpr uint8_t ROW_OFFSETS[4] = {
        0x00,
        0x40,
        0x00,
        0x40
    };

    // this one WILL need bitwise ORing because the cursor position is variable.
    // This is not publically accessible, and is instead accessed through the
    // SET_DDRAM_ADDRESS helper function.
    static constexpr uint8_t SET_DDRAM_ADDR  = 0b1000'0000;
};

/**
 * @brief Driver for a 40x4 LCD using ST7066 controller
 */
class LCD
{
public:
    /**
     * @brief Don't bother with D0-D3, since we're just using 4 bit mode, and 
     * rw is just tied to ground
     * 
     * @param rs Register select pin
     * @param e1 Enable pin 1
     * @param e2 Enable pin 2
     * @param d4 Data pin 4
     * @param d5 Data pin 5
     * @param d6 Data pin 6
     * @param d7 Data pin 7
     */
    LCD(uint8_t rs_, uint8_t e1_, uint8_t e2_,
        uint8_t d4_, uint8_t d5_, uint8_t d6_, uint8_t d7_);

    /**
     * @brief Clears the screen and moves to 0,0
     */
    void clear();

    void enableCursor();
    void disableCursor();

    void setCursorPos(uint8_t row, uint8_t col);
    void getCursorPos(uint8_t &row, uint8_t &col);
    
    void setCursorRow(uint8_t row);
    uint8_t getCursorRow();

    void setCursorCol(uint8_t col);
    uint8_t getCursorCol();

    /**
     * @brief Writes a single character to the LCD at the current position
     * 
     * @param c character to write
     */
    void write(char c);

    /**
     * @brief Helper function to write a string to the LCD, it just repeatedly
     * calls the write function for each character in the string
     * 
     * @param str 
     */
    void write(const std::string& str);
    
    /**
     * @brief Follows initialization sequence from page 28 of the datasheet
     */
    void initialize();

private:

    // local reference to the row and column of the cursor. Used for determining
    // which enable pin we care about
    uint8_t row;
    uint8_t col;

    // relevant pins
    uint8_t rs;
    uint8_t e1;
    uint8_t e2;
    uint8_t d4;
    uint8_t d5;
    uint8_t d6;
    uint8_t d7;

    bool cursorEnabled = false;

    /**
     * @brief Sets the specified pin to be an output pin
     * 
     * @param pin 
     */
    void setOutput(uint8_t pin);

    /**
     * @brief Sends a byte to the LCD, in two nibbles since we're using 4 bit
     * mode. 
     * 
     * @param value the byte to send
     * @param isData whether this byte is data (true) or a command (false).  
     * @param en which enable pin to pulse, either e1 or e2
     * @param delayUsAfter how long to wait after sending the byte, in microseconds
     */
    void sendByte(uint8_t value, bool isData, uint8_t en, uint32_t delayUsAfter = DEFAULT_DELAY);

    /**
     * @brief helper function to write a single nibble to the LCD. This function
     * does NOT pulse the enable pin, since it doesn't know which one to pulse.
     * The caller is responsible for pulsing the correct enable pin after
     * calling this.
     * 
     * @param value nibble to write. Only the least significant 4 bits are used.
     * The top 4 bits are ignored.
     */
    void writeNibble(uint8_t value);

    /**
     * @brief Pulses the given enable pin, waiting 1us between setting it high
     * and then low.
     * 
     * @param pin which enable pin to pulse, either e1 or e2
     */
    void pulse(uint8_t pin);

    /**
     * @brief runs the initialization sequence for a specific chip, which is
     * selected by the enable pin passed in
     * 
     * @param en which pin to use, either e1 or e2
     */
    void initialize(uint8_t en);

    /**
     * @brief Gets which enable pin we currently care about, based on the current
     * row. Rows 0 and 1 correspond to e1, and rows 2 and 3 correspond to e2.
     * 
     * @return uint8_t 
     */
    uint8_t currentEnable();

    /**
     * @brief Moves the cursor by one to the right. If we hit the end of a line,
     * it moves down to the beginning of the next line. If we hit the end of the
     * display, it wraps around to the beginning of the first line.
     */
    void incrementCursor();

    /**
     * @brief Clamps the cursor position to valid range (row 0 to 3, col 0 to 39)
     * 
     * @param row_ 
     * @param col_ 
     */
    void clampCursorPos(uint8_t& row_, uint8_t& col_);

    /**
     * @brief Compares the current row and the desired row and determines if we
     * would be switching chips. This is useful to not perform wasteful operations
     * on stuff like the cursor if we don't need to
     * 
     * @param curRow_ current row that the cursor is on
     * @param newRow_ row we wish to set the cursor to
     * 
     * @return true if the chips would switch
     * @return false otherwise
     */
    bool chipsAreSwitching(uint8_t curRow_, uint8_t newRow_);
};