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
        switch (currentState) {
            case State::STARTUP:
                lcd.disableCursor();
                break;
            case State::DOCUMENT_SELECTION:
                lcd.enableCursor();
                lcd.clear();
                break;
            default:
                break;
        }

        currentState = newState;
    }
};