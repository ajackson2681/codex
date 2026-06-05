#include <cstddef>
#include <cstdint>
#include <string>

#include "f_util.h"
#include "ff.h"
#include "GapBuffer.hpp"

class FileSystem
{
public:
    static bool init();
    static void uninit();
    
    /**
     * @brief Checks if a file exists in the mounted card. 
     * 
     * @param fileName 
     * @return true if the file exists
     * @return false if the file doesnt exist or if there is no mounted card
     */
    static bool fileExists(const std::string& fileName);

    /**
     * @brief Loads specified file into the gap buffer
     * 
     * @param fileName 
     * @param dest 
     */
    static void loadFile(const std::string& fileName, GapBuffer& dest);

    /**
     * @brief Saves the gap buffer to the specified file
     * 
     * @param fileName 
     * @param src 
     */
    static void saveFile(const std::string& fileName, GapBuffer& src);

private:
    static FATFS fs;
    static FIL fil;
    static FILINFO fno;
};