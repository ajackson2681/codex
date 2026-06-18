#pragma once

#include <string>

#define RS_PIN 21

#define D7_PIN 13
#define D6_PIN 15
#define D5_PIN 12
#define D4_PIN 14

#define E1_PIN 22
#define E2_PIN 20

#define VER_MAJOR 0 
#define VER_MINOR 1
#define VER_PATCH 0

std::string versionString() {
    return std::to_string(VER_MAJOR)+"."+std::to_string(VER_MINOR)+"."+std::to_string(VER_PATCH);
}
