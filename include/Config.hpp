#pragma once

#include <string>

#define BOARD_REV 0

#if BOARD_REV <= 1

#define RS_PIN 21

#define D7_PIN 13
#define D6_PIN 15
#define D5_PIN 12
#define D4_PIN 14

#define E1_PIN 22
#define E2_PIN 20

#else if BOARD_REV == 2

#define RS_PIN 9

#define D7_PIN 13
#define D6_PIN 14
#define D5_PIN 12
#define D4_PIN 15

#define E1_PIN 10
#define E2_PIN 8

#endif

#define COL_COUNT 40
#define ROW_COUNT 4