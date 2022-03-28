#include "project.h"
#include "testing.h"

#define TEMP_N_READINGS 11

typedef struct { 
    float32 all_TEMP_readings[TEMP_N_READINGS];
    float32 TEMP_reading;
} TEMP_t;

test_t TEMP_test();

TEMP_t TEMP_read();

// establish a debugging and reading struct
void TEMP_Debug();

// Calibration must be done in boiling water
// Calibration needs to be set only once 
uint8 TEMP_cal();

// Temp comes alive
void TEMP_Talk();

// Temp shut down 
void TEMP_Stop();