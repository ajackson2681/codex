#pragma once

#include <string>

#define BOARD_REV 1

#if BOARD_REV == 0

#define RS_PIN 21

#define D7_PIN 13
#define D6_PIN 15
#define D5_PIN 12
#define D4_PIN 14

#define E1_PIN 22
#define E2_PIN 20

#define CD_PIN 26

#elif BOARD_REV == 1

#define RS_PIN 21

#define D7_PIN 13
#define D6_PIN 15
#define D5_PIN 12
#define D4_PIN 14

#define E1_PIN 22
#define E2_PIN 20

#define CD_PIN 11

#define PWM_CON_PIN 0
#define PWM_BRI_PIN 9

#define BRI_POT 26

#endif

// confusing because this is actually "lowest" contrast, but it's the highest
// PWM value we want to consider. Thankfully, this number nicely lines up with
// percentage
#define MAX_CONTRAST_PWM 100

#define COL_COUNT 40
#define ROW_COUNT 4
#define MAX_FILE_NAME_LENGTH 120