# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(texteditor C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_subdirectory(lib/fatfs/FatFs_SPI)

add_executable(texteditor 
    src/texteditor.cpp
    src/GapBuffer.cpp
    src/LiquidCrystal.cpp
    src/hid.cpp
    src/Keyboard.cpp
    src/hardware.c)

pico_set_program_name(texteditor "texteditor")
pico_set_program_version(texteditor "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(texteditor 0)
pico_enable_stdio_usb(texteditor 0)

# Add the standard library to the build
target_link_libraries(texteditor 
    pico_stdlib 
    tinyusb_host 
    tinyusb_board
    FatFs_SPI
)

# Add the standard include files to the build
target_include_directories(texteditor PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

pico_add_extra_outputs(texteditor)

