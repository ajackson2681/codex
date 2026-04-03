#pragma once

#include <vector>
#include <string>
#include <cstdint>

#define DEFAULT_BUF_SIZE 32

#define SHOW_GAP false

#define COL_COUNT 40
#define ROW_COUNT 4
#define FRAME_SIZE (COL_COUNT * ROW_COUNT)

class GapBuffer {
private:
    std::vector<char> buffer;
    size_t gapStart;
    size_t gapEnd;    
    size_t viewFrameStart;
    // this is true if the buffer has been modified since the last time it was printed
    bool stale = true;
    // LiquidCrystal& lcd;
    /**
     * @brief Resizese the buffer vector. It will double in size with each
     * call to this until the buffer reaches a size of 64k, and then it will
     * only increase in size by 8k increments until it hits 112k. At this point
     * the buffer will no longer grow.
     */
    void resize();
    static std::string TEST_STR;
public:
    // GapBuffer(LiquidCrystal& _lcd, int initialSize = DEFAULT_BUF_SIZE);
    GapBuffer(int initialSize = DEFAULT_BUF_SIZE);
    ~GapBuffer();

    /**
     * @brief insert a character at the current cursor position. This will 
     * resize the buffer if necessary.
     * 
     * @param c Character to insert
     */
    void insert(char c);
    /**
     * @brief insert a string at the current cursor postion. This will resize
     * the buffer if necessary.
     * 
     * @param str String to insert
     */
    void insert(const std::string& str);

    /**
     * @brief Moves the cursor left by one character
     */
    void moveLeft();
    /**
     * @brief Moves the cursor right by one character
     */
    void moveRight();

    /**
     * @brief Moves the cursor left by one word. A "word" in this case is
     * defined as a space-separated value
     */
    void moveWordLeft();
    /**
     * @brief Moves the cursor right by one word. A "word" in this case is
     * defined as a space-separated value
     */
    void moveWordRight();

    /**
     * @brief Moves the cursor to the very beginning of the buffer
     */
    void moveToStart();
    /**
     * @brief Moves the cursor to the very end of the buffer
     */
    void moveToEnd();

    /**
     * @brief Moves the cursor up by one line relative to the frame buffer.
     * 
     */
    void moveUpOneLine();
    /**
     * @brief Moves the cursor down by one line relative to the frame buffer.
     * 
     */
    void moveDownOneLine();

    void frameShift();
    
    /**
     * @brief Fills the provided frame buffer with the visible portion of the gap 
     * buffer and returns the cursor index within that frame buffer. The frame 
     * buffer should be of size FRAME_SIZE.
     * 
     * @param frameBuf buffer to copy the visible portion of the gap buffer into
     * @return int cursor index within the frame buffer
     */
    int fillVisibleFrame(char frameBuf[FRAME_SIZE]);

    inline void invalidate() { stale = true; }
    inline bool isStale() { return stale; }
};