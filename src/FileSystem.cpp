#include "FileSystem.hpp"

FATFS FileSystem::fs;
FIL FileSystem::fil;
FILINFO FileSystem::fno;
DIR FileSystem::dir;

bool FileSystem::mounted = false;
bool FileSystem::selectionChanged = true;
int FileSystem::selectedIndex = 0;
int FileSystem::fileListOffset = 0;

std::vector<std::string> FileSystem::fileList = {};

bool FileSystem::isMounted()
{
    return mounted;
}

bool FileSystem::init()
{
    FRESULT fr = f_mount(&fs, "0:", 1);

    // 1. Open the specified directory
    fr = f_opendir(&dir, "0:/");

    if (fr != FR_OK) {
        printf("Failed to open directory (Error: %d)\n", fr);
    }

    // 2. Read each directory item sequentially
    while (1) {
        fr = f_readdir(&dir, &fno);
        
        // Break if an error occurs or the end of the directory is reached
        if (fr != FR_OK || fno.fname[0] == 0) {
            break; 
        }

        // 3. Optional: Skip dot entries ("." and "..") if relative path tracking is active
        if (fno.fname[0] == '.') {
            continue;
        }

        // 4. Distinguish between directories and standard files
        if (fno.fattrib & AM_DIR) {
            // do nothing
        } 
        else {
            // printf("[FILE] %s (%lu bytes)\n", fno.fname, (unsigned long)fno.fsize);
            fileList.push_back(fno.fname);
        }
    }

    fr = f_closedir(&dir);

    mounted = fr == FR_OK;

    return mounted;
}

bool FileSystem::fileExists(const std::string& fileName)
{
    std::string path = "0:/"+fileName;
    FRESULT fr = f_stat(path.c_str(), &fno);
    
    return fr == FR_OK;
}

int FileSystem::getSelectionIndex()
{
    return selectedIndex;
}

int FileSystem::getFileListOffset()
{
    return fileListOffset;
}

std::vector<std::string>& FileSystem::getFileList()
{   
    return fileList;
}

void FileSystem::selectionUp() 
{
    if (selectedIndex == 0 && fileListOffset == 0) {
        return;
    }

    selectedIndex--;
    if (selectedIndex < 0) {
        selectedIndex = 0;
        fileListOffset--;
        if (fileListOffset < 0) {
            fileListOffset = 0;
        }
    }

    selectionChanged = true;
}

void FileSystem::selectionDown() 
{
    selectedIndex++;
    if (selectedIndex == 4) {
        selectedIndex = 3;
        fileListOffset++;
        if (fileListOffset + 3 == fileList.size()) {
            fileListOffset--;
        }
    }
    selectionChanged = true;
}

bool FileSystem::wasSelectionChanged()
{
    bool before = selectionChanged;
    selectionChanged = false;
    return before;
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