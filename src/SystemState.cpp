#include "SystemState.hpp"
#include "Globals.hpp"

namespace SystemState 
{
    static State currentState = State::STARTUP;
    
    State get() 
    {
        return currentState;
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
                // startup only ever transitions to document selection, so we 
                // can just disable the cursor and clear the screen
                lcd.disableCursor();
                lcd.clear();
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
            default:
                break;
        }

        currentState = newState;
    }
};