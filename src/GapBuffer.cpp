#include "GapBuffer.hpp"
#include <iostream>
#include <string>

GapBuffer::GapBuffer(int initialSize) 
    : buffer(initialSize, '\0')
    , gapStart(0)
    , gapEnd(initialSize) 
    , viewFrameStart(0)
    , stale(false)
{

} 

void GapBuffer::insert(char c)
{
    buffer[gapStart++] = c;

    if (gapStart == gapEnd) {
        this->resize();
    }

    frameShift();

    invalidate();
}

void GapBuffer::insert(const std::string& str)
{
    for (char c : str) {
        this->insert(c);
    }
}

void GapBuffer::moveLeft()
{
    if (gapStart == 0) {
        return;
    }

    buffer[gapEnd - 1] = buffer[gapStart - 1];
    gapStart--;
    if (gapStart == viewFrameStart) {
        viewFrameStart -= COL_COUNT;
        if (viewFrameStart < 0) {
            viewFrameStart = 0;
        }
    }
    gapEnd--;

    frameShift();
}

void GapBuffer::moveRight()
{
    if (gapEnd == buffer.size()) {
        return;
    }

    buffer[gapStart] = buffer[gapEnd];
    gapStart++;
    gapEnd++;

    frameShift();
}

void GapBuffer::resize()
{
    size_t suffixLen = buffer.size() - gapEnd;
    size_t oldSize = buffer.size();
    
    buffer.resize(buffer.size() * 2, '\0');

    for (int i = gapEnd; i < oldSize; i++) {
        buffer[buffer.size() - suffixLen + i - gapEnd] = buffer[i];
    }

    gapEnd = buffer.size() - suffixLen;
}

int GapBuffer::fillVisibleFrame(char frameBuf[FRAME_SIZE])
{
    int filledChars = 0;
    
    for (size_t i = viewFrameStart; 
         i < viewFrameStart + FRAME_SIZE && 
         i < gapStart &&
         filledChars < FRAME_SIZE; 
         i++) 
    {
        frameBuf[filledChars++] = buffer[i];
    }
    
    for (size_t i = gapEnd; 
         (i - gapEnd) < viewFrameStart + FRAME_SIZE && 
         i < buffer.size() &&
         filledChars < FRAME_SIZE; 
         i++) 
    {
        frameBuf[filledChars++] = buffer[i];
    }

    stale = false;

    return gapStart - viewFrameStart;
}

void GapBuffer::moveWordLeft() 
{
    if (gapStart > 0 && buffer[gapStart - 1] == ' ') {
        this->moveLeft();
    }

    while (gapStart > 0 && buffer[gapStart - 1] != ' ') {
        this->moveLeft();
    }
}

void GapBuffer::moveWordRight() 
{
    if (gapEnd < buffer.size() && buffer[gapEnd] == ' ') {
        this->moveRight();
    }

    while (gapEnd < buffer.size() && buffer[gapEnd] != ' ') {
        this->moveRight();
    }
}

void GapBuffer::moveToStart() 
{
    while (gapStart > 0) {
        this->moveLeft();
    }

    viewFrameStart = 0;
}

void GapBuffer::moveToEnd() 
{
    while (gapEnd < buffer.size()) {
        this->moveRight();
    }
}

void GapBuffer::moveDownOneLine() 
{
    int gapStartNew = gapStart + COL_COUNT;

    if (gapStartNew > buffer.size()) {
        gapStartNew = buffer.size();
    }

    while (gapStart < gapStartNew) {
        this->moveRight();
    }
}

void GapBuffer::moveUpOneLine() 
{
    int gapStartNew = gapStart - COL_COUNT;
    
    if (gapStartNew < 0) {
        gapStartNew = 0;
    }

    while (gapStart > gapStartNew) {
        this->moveLeft();
    }
}

void GapBuffer::frameShift()
{
    if (gapStart == viewFrameStart + FRAME_SIZE) {
        if (viewFrameStart + FRAME_SIZE > buffer.size()) {
            viewFrameStart = buffer.size() - FRAME_SIZE;
        }
        else {
            viewFrameStart += COL_COUNT;
        }
    }
    else if (gapStart == viewFrameStart) {
        if (viewFrameStart - COL_COUNT < 0) {
            viewFrameStart = 0;
        }
        else {
            viewFrameStart -= COL_COUNT;
        }
    }

}

GapBuffer::~GapBuffer() 
{

}