#include "Keyboard.hpp"
#include "Globals.hpp"
#include "FileSystem.hpp"
#include "SystemState.hpp"

namespace Keyboard
{
    uint8_t const keycode2ascii[128][2] = { HID_KEYCODE_TO_ASCII };

    void StartupStateHandler(uint8_t keycode);
    void DocSelectStateHandler(uint8_t keycode);
    void DocNameStateHandler(uint8_t keycode, bool shift);
    void WritingStateHandler(uint8_t keycode, bool shift, bool ctrl);
    void HandleAscii(uint8_t keycode, bool shift, bool ctrl, GapBuffer& buffer);

    void StartupCardDetectedStateHandler(uint8_t keycode)
    {
        if (keycode == HID_KEY_ENTER) {
            SystemState::set(State::DOCUMENT_SELECTION);
        }
    }

    void StartupNoCardDetectedStateHandler(uint8_t keycode)
    {
        if (keycode == HID_KEY_ENTER) {
            // should this go straight to initialization? maybe. 
            // No point in setting a doc name since we can't save
            SystemState::set(State::INITIALIZATION); 
        }
    }

    void DocNameStateHandler(uint8_t keycode, bool shift)
    {
        switch (keycode) {
            case HID_KEY_ARROW_UP:
                scratchBuffer.moveUpOneLine();
                break;

            case HID_KEY_ARROW_DOWN:
                scratchBuffer.moveDownOneLine();
                break;

            case HID_KEY_ARROW_LEFT:  
                scratchBuffer.moveLeft();        
                break;
            
            case HID_KEY_ARROW_RIGHT:
                scratchBuffer.moveRight();     
                break;
            
            case HID_KEY_BACKSPACE:   
                scratchBuffer.backSpace();       
                break;
            
            case HID_KEY_DELETE:      
                scratchBuffer.deleteChar();       
                break;
            
            case HID_KEY_ENTER:  {
                std::string fileName;
                
                for (int i = 0; i < scratchBuffer.totalChars(); i++) {
                    fileName += scratchBuffer.getCharAt(i);
                }

                FileSystem::SetNewFileName(fileName);

                scratchBuffer.clearBuffer();
                
                SystemState::set(State::INITIALIZATION);
                break;
            }

            default:
                if (scratchBuffer.totalChars() >= MAX_FILE_NAME_LENGTH) {
                    return; // don't allow more input if we're at the max buffer size
                }

                HandleAscii(keycode, shift, false, scratchBuffer);
                break;
        }
    }

    void DocSelectStateHandler(uint8_t keycode)
    {
        switch (keycode) {
            case HID_KEY_ARROW_UP:
                FileSystem::SelectionUp();
                break;
            case HID_KEY_ARROW_DOWN:
                FileSystem::SelectionDown();
                break;
            case HID_KEY_ENTER:
                if (FileSystem::GetSelectedFile() == NEW_FILE_NAME) {
                    SystemState::set(State::SET_DOC_NAME);
                    scratchBuffer.invalidate();
                }
                else {
                    SystemState::set(State::INITIALIZATION);
                }
                break;
        }
    }

    void WritingStateHandler(uint8_t keycode, bool shift, bool ctrl)
    {

        switch (keycode) {
            case HID_KEY_ARROW_LEFT:  
                if (ctrl) {
                    writerBuffer.moveWordLeft();
                }
                else {
                    writerBuffer.moveLeft();        
                }
                break;
            
            case HID_KEY_ARROW_RIGHT:
                if (ctrl) {
                    writerBuffer.moveWordRight();
                }
                else {
                    writerBuffer.moveRight();       
                }
                break;
            
            case HID_KEY_ARROW_UP:  
                if (ctrl) {
                    writerBuffer.moveToFrameStart();
                }
                else {
                    writerBuffer.moveUpOneLine();   
                }
                break;
            
            case HID_KEY_ARROW_DOWN:  
                if (ctrl) {
                    writerBuffer.moveToFrameEnd();
                }
                else {
                    writerBuffer.moveDownOneLine(); 
                }
                break;
            
            case HID_KEY_BACKSPACE:   
                if (ctrl) {
                    writerBuffer.backSpaceWord();
                }
                else {
                    writerBuffer.backSpace();       
                }
                break;
            
            case HID_KEY_DELETE:      
                if (ctrl) {
                    writerBuffer.deleteWord();
                }
                else {
                    writerBuffer.deleteChar();       
                }
                break;
            
            case HID_KEY_HOME:
                if (ctrl) {
                    writerBuffer.moveToStart();
                }
                else {
                    writerBuffer.moveToLineStart();
                }
                break;
            
            case HID_KEY_END:         
                if (ctrl) {
                    writerBuffer.moveToEnd(); 
                }
                else {
                    writerBuffer.moveToLineEnd();
                }
                break;
            
            case HID_KEY_ENTER:       
                if (ctrl && shift) {
                    writerBuffer.insertNewlineAbove();
                }
                else if (ctrl) {
                    writerBuffer.insertNewlineBelow();
                }
                else {
                    writerBuffer.insert('\n');      
                }
                break;
            case HID_KEY_TAB:
                writerBuffer.insert("  "); // insert two spaces instead of a tab
                break;
                
            default:
                HandleAscii(keycode, shift, ctrl, writerBuffer);
        }
    }

    void HandleAscii(uint8_t keycode, bool shift, bool ctrl, GapBuffer& buffer)
    {
        uint8_t ascii = keycode2ascii[keycode][shift ? 1 : 0];

        if (ctrl) {
            switch(keycode) {
                case HID_KEY_S:
                    FileSystem::TrySaveFile();
                    break;
            }
        }
        else if (ascii) {
            buffer.insert(ascii);
        }
    }

    void ProcessInput(uint8_t keycode, bool shift, bool ctrl, bool alt)
    {
        switch (SystemState::get()) {
            case State::STARTUP_CARD_DETECTED:
                StartupCardDetectedStateHandler(keycode);
                break;
            case State::STARTUP_NO_CARD_DETECTED:
                StartupNoCardDetectedStateHandler(keycode);
                break;
            case State::DOCUMENT_SELECTION:
                DocSelectStateHandler(keycode);
                break;
            case State::SET_DOC_NAME:
                DocNameStateHandler(keycode, shift);
                break;
            case State::WRITING:
                WritingStateHandler(keycode, shift, ctrl);
                break;
        }
    }
};