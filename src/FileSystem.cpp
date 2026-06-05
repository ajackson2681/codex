#include "FileSystem.hpp"

FATFS FileSystem::fs;
FIL FileSystem::fil;
FILINFO FileSystem::fno;

bool FileSystem::init()
{
    FRESULT fr = f_mount(&fs, "0:", 1);

    return fr == FR_OK;
}

bool FileSystem::fileExists(const std::string& fileName)
{
    std::string path = "0:/"+fileName;
    FRESULT fr = f_stat(path.c_str(), &fno);

    return fr == FR_OK;
}

void FileSystem::loadFile(const std::string& fileName, GapBuffer& dest)
{

    std::string path = "0:/"+fileName;
    FRESULT fr = f_open(&fil, path.c_str(), FA_READ);

    if (fr != FR_OK) {
        return;
    }

    char c;
    UINT br;
    if (fr == FR_OK) {
        while (f_read(&fil, &c, 1, &br) == FR_OK && br > 0) {
            dest.insert(c);
        }
        f_close(&fil);
    }
}

void FileSystem::saveFile(const std::string& fileName, GapBuffer& src)
{
    std::string path = "0:/"+fileName;
    FRESULT fr = f_open(&fil, path.c_str(), FA_OPEN_ALWAYS | FA_WRITE);
    
    if (fr != FR_OK) {
        return;
    }
    
    fr = f_truncate(&fil); // remove all existing text, we want to overwrite it
    if (fr != FR_OK) {
        return;
    }
    
    for (int i = 0; i < src.totalChars(); i++) {
        f_putc(src.getCharAt(i), &fil);
    }

    fr = f_close(&fil);
    if (fr != FR_OK) {
        return;
    }
}

void FileSystem::uninit()
{
    f_unmount("0:");
}