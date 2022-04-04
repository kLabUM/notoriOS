#include "project.h"
#include "testing.h"

#define TEMP_N_READINGS 11

typedef struct { 
    float32 all_TEMP_readings[TEMP_N_READINGS];
    float32 TEMP_reading;
} TEMP_sensor_t;

test_t TEMP_sensor_test();

TEMP_sensor_t TEMP_read();


float32 float_find_median(float32 array[] , uint8 n);

// function to sort the elements in the level readings array
void fsort(float32 a[],uint8 n);

// function to swap elements in the level readings array in order to sort them
void fswap(float32 *p,float32 *q);

// calibrate sensor to atmospheric oxygen levels
// see "single piont calibration" in datasheet
// this should be done once, before deploying, as calibration is robust to power cycling
uint8 TEMP_Cal();

// prepare lines of communication
void TEMP_Talk();


// pull everything low to stop power leaks
void TEMP_Stop();

/*
#define TEMP_N_READINGS 11

typedef struct { 
    float32 all_TEMP_readings[TEMP_N_READINGS];
    float32 TEMP_reading;
} T_sensor_t;

test_t TEMP_test();

// establish a debugging and reading struct
void TEMP_Debug();

// Calibration must be done in boiling water
// Calibration needs to be set only once 
uint8 TEMP_cal();

// Temp comes alive
void TEMP_Talk();

// Temp shut down 
void TEMP_Stop();
*/
