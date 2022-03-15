#include "project.h"
#include "testing.h"

// define temp struct 
typedef struct {
    float32 TEMP_reading;
} TEMP_sensor_t;

// establish a debugging and reading struct
TEMP_sensor_t TEMP_Debug();
TEMP_sensor_t TEMP_Read();

// Calibration must be done in boiling water
// Calibration needs to be set only once 
uint8 TEMP_cal();

// Temp comes alive
void TEMP_Talk();

// Temp shut down 
void TEMP_Stop();