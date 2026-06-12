#pragma once

#include "GapBuffer.hpp"
#include "LCD.hpp"

#define NEW_FILE_NAME "New File"

enum State {
    STARTUP,
    DOCUMENT_SELECTION,
    INITIALIZATION,
    WRITING
};

extern State writerState;

extern GapBuffer buffer;

extern bool shouldSave;
extern LCD lcd;