#include "Keyboard.hpp"
#include <functional>
#include "Globals.hpp"

extern std::function<void(uint8_t)> kbCb;

static uint8_t const keycode2ascii[128][2] = { HID_KEYCODE_TO_ASCII };

void Keyboard::ProcessInput(uint8_t keycode, bool shift, bool ctrl)
{

    switch (writerState) {
        case State::STARTUP:
            if (keycode == HID_KEY_ENTER) {
                writerState = State::WRITING;
            }
            return;
    }
    
    switch (keycode) {
        case HID_KEY_ARROW_LEFT:  
            if (ctrl && shift) {
                // buffer.selectWordLeft();
            }
            else if (shift) {
                // buffer.selectCharLeft();
            }
            else if (ctrl) {
                buffer.moveWordLeft();
            }
            else {
                buffer.moveLeft();        
            }
            break;
        
        case HID_KEY_ARROW_RIGHT: 
            if (ctrl && shift) {
                // buffer.selectWordRight();
            }
            else if (shift) {
                // buffer.selectCharRight();
            }
            else if (ctrl) {
                buffer.moveWordRight();
            }
            else {
                buffer.moveRight();       
            }
            break;
        
        case HID_KEY_ARROW_UP:  
            if (ctrl && shift) {
                // buffer.selectToStartOfFrame();
            }
            else if (shift) {
                // buffer.selectUpOneLine();
            }
            else if (ctrl) {
                // buffer.moveToStartOfFrame();
            }
            else {
                buffer.moveUpOneLine();   
            }
            break;
        
        case HID_KEY_ARROW_DOWN:  
            if (ctrl && shift) {
                // buffer.selectToEndOfFrame();
            }
            else if (shift) {
                // buffer.selectDownOneLine();
            }
            else if (ctrl) {
                // buffer.moveToEndOfFrame();
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
                // buffer.moveToLineStart();
            }
            break;
        
        case HID_KEY_END:         
            if (ctrl) {
                buffer.moveToEnd(); // move to end of buffer or frame?
            }
            else {
                // buffer.moveToLineEnd();
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
            uint8_t ascii = keycode2ascii[keycode][shift ? 1 : 0];

            if (ctrl) {
                switch(keycode) {
                    case HID_KEY_A:
                        // TODO select all (in frame? or whole buffer?)
                        // buffer.selectAll();
                        break;
                    case HID_KEY_S:
                        // TODO save file
                        shouldSave = true;
                        break;
                    case HID_KEY_C:
                        // TODO copy
                        // buffer.copySelection();
                        break;
                    case HID_KEY_V:
                        // TODO paste
                        // buffer.pasteClipboard();
                        break;
                    case HID_KEY_X:
                        // TODO cut
                        // buffer.cutSelection();
                        break;
                    case HID_KEY_D:
                        // TODO select current word (what VSCode does)
                        // buffer.selectCurrentWord();
                        break;
            }
            }
            else if (ascii) {
                buffer.insert(ascii);
            }
            break;
    }
}