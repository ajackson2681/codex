#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <functional>

#include "f_util.h"
#include "ff.h"
#include "GapBuffer.hpp"

class FileSystem
{
public:
    /**
     * @brief Attempts to mount the SD card if one is inserted
     * 
     * @return true if SD card was successfully mounted
     * @return false if SD card was unsuccessfully mounted
     */
    static bool Init();

    /**
     * @brief Polling check that reads the card detect pin and attemtps to mount
     * the card if it's not already
     */
    static void Check();

    /**
     * @brief Checks if card is inserted
     * 
     * @return true if Card Detect pin is active
     * @return false if not
     */
    static bool CardDetected();

    /**
     * @brief Returns whether or not the SD card is mounted
     * 
     * @return true if SD card is mounted
     * @return false 
     */
    static bool Mounted();
    
    /**
     * @brief Checks if a file exists in the mounted card. 
     * 
     * @param fileName 
     * @return true if the file exists
     * @return false if the file doesnt exist or if there is no mounted card
     */
    static bool FileExists(const std::string& fileName);

    /**
     * @brief Loads specified file into the gap buffer
     * 
     * @param fileName 
     * @param dest 
     */
    static void LoadFile(const std::string& fileName, GapBuffer& dest);

    /**
     * @brief Saves the gap buffer to the specified file
     * 
     * @param fileName 
     * @param src 
     */
    static void SaveFile(const std::string& fileName, GapBuffer& src);

    static void ForEachVisibleFile(std::function<void(const std::string&, int)> callback);
    
    static std::string& GetSelectedFile();

    static int GetSelectedIndex();
    static void SelectionDown();
    static void SelectionUp();
    
    static bool SelectionChanged();

    static bool HasFiles();
    static void EnumerateFiles();

    static void TrySaveFile();
    static void TryLoadFile();
    
    static void Uninit();
    
private:
    static FATFS fs;
    static FIL fil;
    static FILINFO fno;
    static DIR dir;
    
    static bool mounted;
    static bool selectionChanged;
    
    // currently selected document in the visible list
    static int selectedIndex;
    // offset in the file list that points to the first element to be displayed
    static int fileListOffset;
    
    static std::vector<std::string> fileList;

};