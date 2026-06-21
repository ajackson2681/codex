#pragma once

#include "GapBuffer.hpp"
#include "LCD.hpp"

enum State {
    STARTUP,
    DOCUMENT_SELECTION,
    INITIALIZATION,
    WRITING
};

extern State writerState;

extern GapBuffer buffer;

extern bool shouldSave;
extern bool pressedEnter;
extern LCD lcd;