#pragma once

#include "Config.hpp"

enum State 
{
    STARTUP,
    DOCUMENT_SELECTION,
    INITIALIZATION,
    WRITING
};

namespace SystemState 
{
    /**
     * @brief Returns the current state of the application
     * 
     * @return State 
     */
    State get();

    /**
     * @brief Sets the new state for the application. 
     * 
     * @param newState the state to switch to
     */
    void set(State newState);
};