#include <stdio.h>
#include <functional>

#include "pico/stdlib.h"
#include "bsp/board_api.h"
#include "tusb.h"

#include "GapBuffer.hpp"
#include "Globals.hpp"
#include "FileSystem.hpp"
#include "Config.hpp"
#include "Keyboard.hpp"
#include "Version.hpp"
#include "SystemState.hpp"

void tuh_mount_cb(uint8_t dev_addr) {}

void tuh_umount_cb(uint8_t dev_addr) {}

void initialize()
{
    lcd.clear();
    
    FileSystem::TryLoadFile();

    SystemState::set(State::WRITING);

    lcd.enableCursor();
}

void selectDocument()
{
    lcd.disableCursor();

    // skip to initialization if the file system isn't mounted
    if (!FileSystem::Mounted()) {
        SystemState::set(State::INITIALIZATION);
        return;
    }

    // loads any available file names. Only does this once, subsequent calls
    // do nothing
    FileSystem::EnumerateFiles();
    
    if (!FileSystem::HasFiles()) {
        SystemState::set(State::INITIALIZATION);
        return;
    }
    
    if (FileSystem::SelectionChanged()) {    
        lcd.clear();

        FileSystem::ForEachVisibleFile([](const std::string& file, int index) {
            if (index == FileSystem::GetSelectedIndex()) {
                lcd.write("> ");
            } 
            else {
                lcd.write("  ");
            }
            lcd.write(file + "\n");
        });
    }
}

void renderScreen()
{    
    if (buffer.isStale()) {
        auto b = buffer.getVisibleFrame();
        
        lcd.setCursorPos(0,0);
    
        for (int i = 0; i < ROW_COUNT; i++) {
            for (int j = 0; j < COL_COUNT; j++) {
                char c = b[i][j];
    
                switch (c) {
                    case '\n':
                    case '\r':
                    case '\0':
                        lcd.write(' ');
                        break;
                    default:
                        lcd.write(c);
                        break;
                }
            }
        }
    }

    int row,col;
    buffer.getCursorPos(row,col);
    lcd.setCursorPos(row,col);
}

void setup()
{
    stdio_init_all();

    // init host stack on configured roothub port
    tuh_init(BOARD_TUH_RHPORT);
    
    if (board_init_after_tusb) {
        board_init_after_tusb();
    }

    gpio_init(CD_PIN);
    gpio_set_dir(CD_PIN, GPIO_IN);
    gpio_pull_up(CD_PIN);
}

int main()
{
    setup();

    lcd.initialize();
    lcd.enableCursor();
    lcd.write("CODEX v" CODEX_VERSION_STRING "\n");

    if (FileSystem::Init()) {
        lcd.write("SD Card Detected!\n");
    }
    else {
        lcd.write("No SD Card Detected. Can't save files.\n");
    }
    
    lcd.write("Press enter to continue.");

    while (true) {
        tuh_task();
        FileSystem::Check();

        switch (SystemState::get()) {
            case State::STARTUP:
                // don't do anything during startup. Just read input and wait
                // for an enter key press
                break;
            case State::DOCUMENT_SELECTION: 
                selectDocument();
                break;
            case State::INITIALIZATION:
                initialize();
                break;
            case State::WRITING:
                renderScreen();
                break;
            default:
                break;
        }
    } 
}
