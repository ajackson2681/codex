#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <functional>

#include "f_util.h"
#include "ff.h"
#include "GapBuffer.hpp"

namespace FileSystem
{
    /**
     * @brief Attempts to mount the SD card if one is inserted
     * 
     * @return true if SD card was successfully mounted
     * @return false if SD card was unsuccessfully mounted
     */
    bool Init();

    /**
     * @brief Polling check that reads the card detect pin and attemtps to mount
     * the card if it's not already
     */
    void Check();

    /**
     * @brief Checks if card is inserted
     * 
     * @return true if Card Detect pin is active
     * @return false if not
     */
    bool CardDetected();

    /**
     * @brief Returns whether or not the SD card is mounted
     * 
     * @return true if SD card is mounted
     * @return false 
     */
    bool Mounted();
    
    /**
     * @brief Checks if a file exists in the mounted card. 
     * 
     * @param fileName 
     * @return true if the file exists
     * @return false if the file doesnt exist or if there is no mounted card
     */
    bool FileExists(const std::string& fileName);

    /**
     * @brief Loads specified file into the gap buffer
     * 
     * @param fileName 
     * @param dest 
     */
    void LoadFile(const std::string& fileName, GapBuffer& dest);

    /**
     * @brief Saves the gap buffer to the specified file
     * 
     * @param fileName 
     * @param src 
     */
    void SaveFile(const std::string& fileName, GapBuffer& src);

    void ForEachVisibleFile(std::function<void(const std::string&, int)> callback);
    
    std::string& GetSelectedFile();

    int GetSelectedIndex();
    void SelectionDown();
    void SelectionUp();
    
    bool SelectionChanged();

    bool HasFiles();
    void EnumerateFiles();

    void TrySaveFile();
    void TryLoadFile();
    
    void Uninit();
};