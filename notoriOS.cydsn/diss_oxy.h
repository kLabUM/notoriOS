#include "project.h"
#include "testing.h"


#define DO_N_READINGS 11

typedef struct { 
    float32 all_do_readings[DO_N_READINGS];
    float32 do_reading;
} DO_sensor_t;

test_t DO_sensor_test();

DO_sensor_t DO_read();


float32 float_find_median(float32 array[] , uint8 n);

// function to sort the elements in the level readings array
void fsort(float32 a[],uint8 n);

// function to swap elements in the level readings array in order to sort them
void fswap(float32 *p,float32 *q);