#include "GapBuffer.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <cmath>

GapBuffer::GapBuffer(int initialSize) 
    : buffer(initialSize, '\0')
    , gapStart(0)
    , gapEnd(initialSize) 
    , viewFrameStart(0)
    , cursorRow(0)
    , cursorCol(0)
    , stale(false)
{

} 

void GapBuffer::insert(char c)
{
    buffer[gapStart++] = c;
    
    if (gapStart == gapEnd) {
        resize();
    }

    moveCursorRight();

    refreshFrameBuffer();
}

void GapBuffer::insert(const std::string& str)
{
    for (char c : str) {
        insert(c);
    }
}

int GapBuffer::findPrevLineStart(int from)
{
    if (from == 0) {
        return 0;
    }
    
    int pos = from - 1;
    
    // step past the newline that ended this line if present
    if (getCharAt(pos) == '\n' && pos > 0 && getCharAt(from) != '\n') {
        pos--;
    }

    // scan back up to 40 chars to find the line start
    int count = 0;
    while (pos > 0 && count < COL_COUNT) {
        if (getCharAt(pos - 1) == '\n') {
            break;
        }

        pos--;
        count++;

        if (count == COL_COUNT) {
            pos++;
        }
    }
    
    return pos;
}

int GapBuffer::findNextLineStart(int from)
{
    int col = 0;
    int pos = from;

    while (pos < totalChars()) {
        char c = getCharAt(pos++);

        if (c == '\n') {
            return pos;
        }
        
        if (++col >= COL_COUNT) {
            return pos;
        }
    }
    
    return pos;
}

void GapBuffer::moveCursorLeft()
{
    cursorCol--;
    if (cursorCol < 0) {
        cursorCol = COL_COUNT - 1;
        cursorRow--;
        if (cursorRow < 0) {
            cursorRow = 0;
            viewFrameStart = findPrevLineStart(viewFrameStart);
            refreshFrameBuffer();
        }
        else {
            // if we moved up a line but are still within the frame, we need to
            // move the column to the end of new current line
            if (frameBuf[cursorRow][cursorCol] == '\0') {
                // scan left until we find a non-empty character or the start of the line
                while (cursorCol > 0 && frameBuf[cursorRow][cursorCol] == '\0') {
                    cursorCol--;
                }
            }
        }
    }

}

void GapBuffer::moveLeft(bool moveCursor)
{

    if (gapStart == 0) {
        return;
    }

    buffer[gapEnd - 1] = buffer[gapStart - 1];

    gapStart--;
    gapEnd--;
    if (moveCursor) {
        moveCursorLeft();
    }
}

void GapBuffer::moveLeft()
{
    moveLeft(true);
}

void GapBuffer::moveCursorRight()
{
    cursorCol++;
    if (cursorCol == COL_COUNT || buffer[gapStart - 1] == '\n') {
        cursorCol = 0;
        cursorRow++;
        if (cursorRow >= ROW_COUNT) {
            cursorRow = ROW_COUNT - 1;
            viewFrameStart = findNextLineStart(viewFrameStart);
            refreshFrameBuffer();
        }
    }

}


void GapBuffer::moveRight(bool moveCursor)
{

    if (gapEnd == buffer.size()) {
        return;
    }

    buffer[gapStart] = buffer[gapEnd];

    gapStart++;
    gapEnd++;
    if (moveCursor) {
        moveCursorRight();
    }
}

void GapBuffer::moveRight()
{
    moveRight(true);
}

void GapBuffer::refreshFrameBuffer()
{
    std::memset(frameBuf, '\0', sizeof(frameBuf));
    
    int row = 0, col = 0;
    int totalCh = totalChars();
    
    for (int i = viewFrameStart; i < totalCh && row < ROW_COUNT; i++) {
        char c = getCharAt(i);
        
        frameBuf[row][col] = c;

        if (c == '\n') {
            row++;
            col = 0;
        } 
        else {
            col++;
            if (col >= COL_COUNT) {
                col = 0;
                row++;
            }
        }
    }

    invalidate();
}

void GapBuffer::resize()
{
    int suffixLen = buffer.size() - gapEnd;
    int oldSize = buffer.size();
    
    buffer.resize(buffer.size() * 2, '\0');

    for (int i = gapEnd; i < oldSize; i++) {
        buffer[buffer.size() - suffixLen + i - gapEnd] = buffer[i];
    }

    gapEnd = buffer.size() - suffixLen;
}

void GapBuffer::moveWordLeft() 
{
    if (gapStart > 0 && (buffer[gapStart - 1] == ' ' || buffer[gapStart - 1] == '\n')) {
        moveLeft(true);
    }

    while (gapStart > 0 && buffer[gapStart - 1] != ' ' && buffer[gapStart - 1] != '\n') {
        moveLeft(true);
    }
}

void GapBuffer::moveWordRight() 
{
    if (gapEnd < buffer.size() && (buffer[gapEnd] == ' ' || buffer[gapEnd] == '\n')) {
        moveRight(true);
    }

    while (gapEnd < buffer.size() && buffer[gapEnd] != ' ' && buffer[gapEnd] != '\n') {
        moveRight(true);
    }
}

void GapBuffer::moveToStart() 
{
    while (gapStart > 0) {
        moveLeft();
    }

    viewFrameStart = 0;
    cursorRow = 0;
    cursorCol = 0;

    refreshFrameBuffer();
}

void GapBuffer::moveToEnd() 
{
    while (gapEnd < buffer.size()) {
        moveRight();
    }

    refreshFrameBuffer();
}

void GapBuffer::invalidate()
{
    stale = true;
}

int GapBuffer::frameCellToBufferIndex(int targetRow, int targetCol)
{
    int row = 0, col = 0;
    int pos = viewFrameStart;

    while (pos < totalChars()) {
        if (row == targetRow && col == targetCol) {
            return pos;
        } 
        
        char c = getCharAt(pos++);
        if (c == '\n') {
            row++;
            col = 0;
        } 
        else {
            col++;
            if (col >= COL_COUNT) {
                col = 0;
                row++;
            }
        }
    }

    return pos;
}

void GapBuffer::clamp()
{
    // clamp to end of target line
    bool shifted = false;
    while (frameBuf[cursorRow][cursorCol] == '\0' && cursorCol > 0) 
    {
        cursorCol--;
        shifted = true;
    }

    // if we actually clamped, and went back past a newline, we should shift
    // back forward one 
    if (shifted && frameBuf[cursorRow][cursorCol] != '\n' && frameBuf[cursorRow][cursorCol] != '\0') {
        cursorCol++;
    }
}

void GapBuffer::moveDownOneLine() 
{
    // check if the next row exists in the frame buffer
    // a row exists if the current row ended with \n, or if it has content
    bool currentRowHasNewline = false;
    for (int c = 0; c < COL_COUNT; c++) {
        if (frameBuf[cursorRow][c] == '\n') {
            currentRowHasNewline = true;
            break;
        }
    }
    
    if (!currentRowHasNewline && frameBuf[cursorRow][COL_COUNT - 1] == '\0') {
        // move to end of the current line if we're already at the bottom
        while (cursorCol < COL_COUNT && frameBuf[cursorRow][cursorCol] != '\0') {
            cursorCol++;
        }
    }
    else if (cursorRow >= 3) {
        // scroll frame down
        int nextStart = findNextLineStart(viewFrameStart);
        
        if (nextStart >= totalChars()) {
            return; // can't scroll down if there are no more lines
        }

        viewFrameStart = nextStart;
        refreshFrameBuffer();
    } 
    else {
        cursorRow++;
    }
    
    // clamp to end of target line
    clamp();
    
    // sync gap buffer to new cursor position
    int newPos = frameCellToBufferIndex(cursorRow, cursorCol);

    while (gapStart < newPos) {
        moveRight(false);
    }
}

void GapBuffer::moveUpOneLine() 
{
    if (cursorRow == 0) {
        // scroll frame up
        if (viewFrameStart == 0) {
            return; // can't scroll up if we're already at the top
        }
        viewFrameStart = findPrevLineStart(viewFrameStart);
        refreshFrameBuffer();
        // cursorRow stays the same, now pointing at new content
    } 
    else {
        cursorRow--;
    }

    clamp();
    
    // sync gap buffer to new cursor position
    int newPos = frameCellToBufferIndex(cursorRow, cursorCol);

    while(gapStart > newPos) {
        moveLeft(false);
    }
}

void GapBuffer::moveToFrameStart() 
{
    while (gapStart > viewFrameStart) {
        moveLeft();
    }
    refreshFrameBuffer();
}

void GapBuffer::moveToFrameEnd()
{
    while (cursorCol < COL_COUNT && 
           cursorRow < ROW_COUNT && 
           frameBuf[cursorRow][cursorCol] != '\0') 
    {
        moveRight();
    }
    
    refreshFrameBuffer();
}

char GapBuffer::getCharAt(int idx)
{
    // just return null char if out of bounds
    if (idx < 0) {
        return '\0';
    }
    else if (idx >= totalChars()) {
        return '\0';
    }

    if (idx < gapStart) {
        return buffer[idx];
    } 
    else {
        return buffer[gapEnd + idx - gapStart];
    }
}

void GapBuffer::backSpace()
{
    if (gapStart == 0) {
        return;
    }

    buffer[gapStart-1] = '\0';
    gapStart--;

    moveCursorLeft();

    refreshFrameBuffer();
}

void GapBuffer::backSpaceWord()
{
    backSpace();

    while (gapStart > 0 && buffer[gapStart - 1] != ' ' && buffer[gapStart - 1] != '\n') {
        backSpace();
    }
}

void GapBuffer::deleteChar()
{
    if (gapEnd == buffer.size()) {
        return;
    }
    
    buffer[gapEnd] = '\0';
    gapEnd++;

    refreshFrameBuffer();
}

void GapBuffer::deleteWord()
{
    deleteChar();

    while (gapEnd < buffer.size() && buffer[gapEnd] != ' ' && buffer[gapEnd] != '\n') {
        deleteChar();
    }
}

bool GapBuffer::isStale()
{
    return stale;
}

const char (&GapBuffer::getVisibleFrame())[ROW_COUNT][COL_COUNT]
{
    stale = false;
    return frameBuf;
}

int GapBuffer::getFrameRow()
{
    return cursorRow;
}

int GapBuffer::getFrameCol()
{
    return cursorCol;
}

int GapBuffer::totalChars()
{
    return gapStart + (buffer.size() - gapEnd);
}

GapBuffer::~GapBuffer() 
{

}