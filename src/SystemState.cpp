#include "SystemState.hpp"

namespace SystemState 
{
    static State state = State::STARTUP;
    
    State get() 
    {
        return state;
    }

    void set(State newState) 
    {
        // could do validation / conditional logic to handle transitions between 
        // states, but for now just set the state
        state = newState;
    }
};