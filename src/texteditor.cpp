#include <stdio.h>
#include <functional>

#include "pico/stdlib.h"
#include "bsp/board_api.h"
#include "tusb.h"
#include "hardware/pwm.h"

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
    FileSystem::TryLoadFile();
    SystemState::set(State::WRITING);
}

void selectDocument()
{
    // skip to initialization if the file system isn't mounted
    if (!FileSystem::Mounted()) {
        SystemState::set(State::INITIALIZATION);
        return;
    }

    // loads any available file names. Only does this once, subsequent calls
    // do nothing
    FileSystem::EnumerateFiles();
    
    if (!FileSystem::HasFiles()) {
        SystemState::set(State::SET_DOC_NAME);
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

void setDocumentName()
{
    if (scratchBuffer.isStale()) {
        lcd.write("Document Name:\n");
        auto b = scratchBuffer.getVisibleFrame();
        
        lcd.setCursorPos(1,0);
    
        for (int i = 0; i < ROW_COUNT - 1; i++) { // only will have 3 lines available for this
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
    scratchBuffer.getCursorPos(row,col);
    // the cursor is on the second row, so we need to add 1 to the row value
    lcd.setCursorPos(row+1,col);
}

void renderScreen()
{    
    if (writerBuffer.isStale()) {
        auto b = writerBuffer.getVisibleFrame();
        
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
    writerBuffer.getCursorPos(row,col);
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

    gpio_init(PWM_CON_PIN);
    gpio_set_dir(PWM_CON_PIN, GPIO_OUT);
    gpio_put(PWM_CON_PIN, true);

    gpio_set_function(PWM_CON_PIN, GPIO_FUNC_PWM);
    gpio_set_function(PWM_BRI_PIN, GPIO_FUNC_PWM);
    
    uint slice1 = pwm_gpio_to_slice_num(PWM_CON_PIN);
    pwm_set_wrap(slice1, 255);
    pwm_set_enabled(slice1, true);

    uint slice2 = pwm_gpio_to_slice_num(PWM_BRI_PIN);
    pwm_set_wrap(slice2, 255);
    pwm_set_enabled(slice2, true);

    // default to max contrast
    pwm_set_gpio_level(PWM_CON_PIN, 0);
    // default to max brightness (apparently 0% duty is 100% brightness)
    pwm_set_gpio_level(PWM_BRI_PIN, 0);
}

int main()
{

    setup();

    lcd.initialize();
    lcd.enableCursor();

    while (true) {
        tuh_task();
        FileSystem::Check();

        switch (SystemState::get()) {
            case State::STARTUP:
                if (FileSystem::Mounted()) {
                    SystemState::set(State::STARTUP_CARD_DETECTED);
                }
                else {
                    SystemState::set(State::STARTUP_NO_CARD_DETECTED);
                }
                break;
            case State::STARTUP_CARD_DETECTED:
                if (!FileSystem::Mounted()) {
                    SystemState::set(State::STARTUP_NO_CARD_DETECTED);
                }
                break;
            case State::STARTUP_NO_CARD_DETECTED:
                if (FileSystem::Mounted()) {
                    SystemState::set(State::STARTUP_CARD_DETECTED);
                }
                break;
            case State::DOCUMENT_SELECTION: 
                selectDocument();
                break;
            case State::SET_DOC_NAME:
                setDocumentName();
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
