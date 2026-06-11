#include "Keyboard.hpp"
#include <functional>
#include "Globals.hpp"
#include "FileSystem.hpp"

extern std::function<void(uint8_t)> kbCb;

static uint8_t const keycode2ascii[128][2] = { HID_KEYCODE_TO_ASCII };

void Keyboard::StartupStateHandler(uint8_t keycode)
{
    if (keycode == HID_KEY_ENTER) {
        writerState = State::DOCUMENT_SELECTION;
    }
}

void Keyboard::DocSelectStateHandler(uint8_t keycode)
{
    switch (keycode) {
        case HID_KEY_ARROW_UP:
            FileSystem::SelectionUp();
            break;
        case HID_KEY_ARROW_DOWN:
            FileSystem::SelectionDown();
            break;
        case HID_KEY_ENTER:
            writerState = State::INITIALIZATION;
            break;
    }
}

void Keyboard::WritingStateHandler(uint8_t keycode, bool shift, bool ctrl)
{

    switch (keycode) {
        case HID_KEY_ARROW_LEFT:  
            if (ctrl) {
                buffer.moveWordLeft();
            }
            else {
                buffer.moveLeft();        
            }
            break;
        
        case HID_KEY_ARROW_RIGHT:
            if (ctrl) {
                buffer.moveWordRight();
            }
            else {
                buffer.moveRight();       
            }
            break;
        
        case HID_KEY_ARROW_UP:  
            if (ctrl) {
                buffer.moveToFrameStart();
            }
            else {
                buffer.moveUpOneLine();   
            }
            break;
        
        case HID_KEY_ARROW_DOWN:  
            if (ctrl) {
                buffer.moveToFrameEnd();
            }
            else {
                buffer.moveDownOneLine(); 
            }
            break;
        
        case HID_KEY_BACKSPACE:   
            if (ctrl) {
                buffer.backSpaceWord();
            }
            else {
                buffer.backSpace();       
            }
            break;
        
        case HID_KEY_DELETE:      
            if (ctrl) {
                buffer.deleteWord();
            }
            else {
                buffer.deleteChar();       
            }
            break;
        
        case HID_KEY_HOME:
            if (ctrl) {
                buffer.moveToStart();
            }
            else {
                buffer.moveToLineStart();
            }
            break;
        
        case HID_KEY_END:         
            if (ctrl) {
                buffer.moveToEnd(); 
            }
            else {
                buffer.moveToLineEnd();
            }
            break;
        
        case HID_KEY_ENTER:       
            if (ctrl && shift) {
                // buffer.insertNewlineAbove();
            }
            else if (ctrl) {
                // buffer.insertNewlineBelow();
            }
            else {
                buffer.insert('\n');      
            }
            break;
        case HID_KEY_TAB:
            buffer.insert("  "); // insert two spaces instead of a tab
            break;
            
        default:
            HandleAscii(keycode, shift, ctrl);
    }
}

void Keyboard::HandleAscii(uint8_t keycode, bool shift, bool ctrl)
{
    uint8_t ascii = keycode2ascii[keycode][shift ? 1 : 0];

    if (ctrl) {
        switch(keycode) {
            case HID_KEY_S:
                // TODO save file
                shouldSave = true;
                break;
        }
    }
    else if (ascii) {
        buffer.insert(ascii);
    }
}

void Keyboard::ProcessInput(uint8_t keycode, bool shift, bool ctrl)
{
    switch (writerState) {
        case State::STARTUP:
            StartupStateHandler(keycode);
            break;
        case State::DOCUMENT_SELECTION:
            DocSelectStateHandler(keycode);
            break;
        case State::WRITING:
            WritingStateHandler(keycode, shift, ctrl);
            break;
    }
}