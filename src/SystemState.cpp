#include "SystemState.hpp"
#include "Globals.hpp"
#include "Version.hpp"

namespace SystemState 
{
    static State currentState = State::STARTUP;
    
    State get() 
    {
        return currentState;
    }

    void writeCardDetectedText() {
        lcd.clear();
        lcd.write("CODEX v" CODEX_VERSION_STRING "\n");
        lcd.write("SD Card Detected!\n");
        lcd.write("Press enter to continue.");
    }

    void writeNoCardDetectedText() {
        lcd.clear();
        lcd.write("CODEX v" CODEX_VERSION_STRING "\n");
        lcd.write("No SD Card Detected. Can't save files.\n");
        lcd.write("Press enter to continue.");
    }

    void set(State newState) 
    {
        // don't do anything if we somehow get called with the same state we're
        // we're already in. Just to prevent any weirdness.
        if (newState == currentState) {
            return;
        }

        switch (currentState) {
            case State::STARTUP:
                // startup transitions to either STARTUP_CARD_DETECTED or 
                // STARTUP_NO_CARD_DETECTED, d
                if (newState == State::STARTUP_CARD_DETECTED) {
                    writeCardDetectedText();
                }
                else if (newState == State::STARTUP_NO_CARD_DETECTED) {
                    writeNoCardDetectedText();
                }
                break;
            case State::DOCUMENT_SELECTION:
                // document selection can transition to either set doc name or
                // initialization, which in turn transitions to writing, but
                // in either case, we can just enable the cursor and clear the
                // screen.
                lcd.enableCursor();
                lcd.clear();
                break;
            case State::SET_DOC_NAME:
                // delete the contents of the scratch buffer when we leave this
                // state, since we might need to reuse the scratch buffer.
                scratchBuffer.clearBuffer();
                // cursor _should_ be enabled already, but explicitly do it
                // jus to be safe.
                lcd.enableCursor();
                lcd.clear();
                break;
            case State::INITIALIZATION:
                // initialization only ever transitions to writing, so we can just
                // enable the cursor and clear the screen
                lcd.enableCursor();
                lcd.clear();
                break;
            case State::STARTUP_CARD_DETECTED:
                // if going from card detected to no card detected, we need to
                // write the no card detected text, otherwise just clear the
                // screen and enable the cursor.
                if (newState == State::STARTUP_NO_CARD_DETECTED) {
                    writeNoCardDetectedText();
                }
                else {
                    lcd.clear();
                    lcd.enableCursor();
                }
                break;
            case State::STARTUP_NO_CARD_DETECTED:
                // if going from no card detected to card detected, we need to
                // write the card detected text, otherwise just clear the screen
                // and enable the cursor.
                if (newState == State::STARTUP_CARD_DETECTED) {
                    writeCardDetectedText();
                }
                else {
                    lcd.clear();
                    lcd.enableCursor();
                }
                break;
            default:
                break;
        }

        currentState = newState;
    }
};