#include "project.h"
#include "testing.h"

#define N_READINGS 11
#define N_params 2

typedef struct { 
    float32 all_do_readings[N_READINGS];
    float32 do_reading;
    float32 all_temp_readings[N_READINGS];
    float32 temp_reading;
} wq_sensors_t;

test_t wq_sensor_test();

wq_sensors_t wq_take_reading();

float32 float_find_median(float32 array[] , uint8 n);

// function to sort the elements in the level readings array
void fsort(float32 a[],uint8 n);

// function to swap elements in the level readings array in order to sort them
void fswap(float32 *p,float32 *q);

// calibrate sensor to atmospheric oxygen levels
// see "single piont calibration" in datasheet
// this should be done once, before deploying, as calibration is robust to power cycling
uint8 wq_cal();

// prepare lines of communication
void wq_start_talking();

// pull everything low to stop power leaks
void wq_stop_talking();