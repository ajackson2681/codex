#include "FileSystem.hpp"
#include "Config.hpp"
#include "Globals.hpp"
#include <pico/stdlib.h>
#include <algorithm>

namespace FileSystem
{
    FATFS fs;
    FIL fil;
    FILINFO fno;
    DIR dir;
    
    bool mounted = false;
    bool selectionChanged = true;
    
    int selectedIndex = 0;
    int fileListOffset = 0;
    
    std::vector<std::string> fileList = {};

    
    bool Init()
    {
        FRESULT fr = f_mount(&fs, "0:", 1);
        
        return fr == FR_OK;
    }

    void Check()
    {
        if (!mounted && CardDetected()) {
            mounted = Init();
        }
        else if (mounted && !CardDetected()) {
            Uninit();
        }
    }

    bool CardDetected()
    {
        // card detect is active low, so it will read 1 if it's NOT inserted, but
        // we want this to make sense boolean-wise, so invert it
        return !gpio_get(CD_PIN);
    }

    bool Mounted()
    {
        return mounted;
    }

    void EnumerateFiles()
    {
        if (fileList.size() > 0) {
            return;
        }

        FRESULT fr = f_opendir(&dir, "0:/");

        if (fr != FR_OK) {
            return;
        }

        while (1) {
            fr = f_readdir(&dir, &fno);
            
            // Break if an error occurs or the end of the directory is reached
            if (fr != FR_OK || fno.fname[0] == 0) {
                break; 
            }

            std::string fileName(fno.fname);

            // Skip dot entries ("." and "..")
            if (fileName.starts_with(".")) {
                continue;
            }

            if (fno.fattrib & AM_DIR) {
                // do nothing
            } 
            else {
                if (fileName.ends_with(".txt")) {
                    fileList.push_back(fileName);
                }
            }
        }

        fileList.push_back(NEW_FILE_NAME);

        f_closedir(&dir);
    }

    bool FileExists(const std::string& fileName)
    {
        std::string path = "0:/"+fileName;
        FRESULT fr = f_stat(path.c_str(), &fno);
        
        return fr == FR_OK;
    }

    void ForEachVisibleFile(std::function<void(const std::string&, int)> callback)
    {
        size_t end = std::min((size_t)(fileListOffset + 4), fileList.size());
        for (size_t i = fileListOffset; i < end; i++) {
            callback(fileList[i], i - fileListOffset);
        }
    }

    std::string& GetSelectedFile()
    {
        return fileList.at(fileListOffset+selectedIndex);
    }

    int GetSelectedIndex()
    {
        return selectedIndex;
    }

    void SelectionUp() 
    {
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

    void SelectionDown() 
    {
        // don't shift if we're already at the end of the list
        // or if the list is smaller than the view windows
        if (selectedIndex + 1 >= fileList.size() || 
            fileListOffset + 3 >= fileList.size()) {
            return;
        }
        
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

    bool SelectionChanged()
    {
        bool before = selectionChanged;
        selectionChanged = false;
        return before;
    }

    void LoadFile(const std::string& fileName, GapBuffer& dest)
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

    void SaveFile(const std::string& fileName, GapBuffer& src)
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


    bool HasFiles()
    {
        return fileList.size() > 1; // account for "New File" entry
    }

    void TrySaveFile()
    {
        if (!mounted) {
            return;
        }

        std::string& selectedFile = GetSelectedFile();
        if (selectedFile == NEW_FILE_NAME) {
            return; // maybe go to a "save as" flow in the future?
        }

        SaveFile(selectedFile, buffer);
    }

    void TryLoadFile()
    {
        if (!mounted) {
            return;
        }

        std::string& selectedFile = GetSelectedFile();
        if (selectedFile == NEW_FILE_NAME) {
            return;
        }

        LoadFile(selectedFile, buffer);
    }

    void Uninit()
    {
        f_unmount("0:");
        mounted = false;
        fileList.clear();
        selectedIndex = 0;
        fileListOffset = 0;
    }

};

