#ifndef SONTEK_H
#define SONTEK_H
 
#include "project.h"
#include "SDI12.h"

//"M" for sontek requests M parameter measurements (1-9 in sontek.values)
#define TAKE_MEASUREMENT_1    "M1"  // Request M1 Parameters Measurements (10-18 in sontek.values)
#define TAKE_MEASUREMENT_2    "M2"  // Request M2 Parameters Measurements (19-27 in sontek.value)

//"D0" for sontek reads completed measurements 1-4
#define READ_MEASUREMENT_1    "D1" // Read Completed Measurements parameters 5-8
#define READ_MEASUREMENT_2    "D2" // Read Completed Measurement parameter 9

#define SONTEK_READING_INIT 0       //initializing
#define SONTEK_READING_VALID 1      // Successfully took reading 
#define SONTEK_READING_NOT_RESPONDING 2
#define SONTEK_READING_OFF 3
 
/* 
 * Define a struct to be used for readings
 */
typedef struct {
    uint8 valid;        //assign flags
    float depth;
    float SNR;
} sontek_t;    

/**
 * @brief Take a measurement specifically for sontek sensor, given its address. Special case of SDI_take_measurement() in SDI12.c
 * 
 * @param sensor SDI12_sensor that we want to request measurements from
 *
 * @ return 1u if measurements were successfully taken, 0u if there was a communication error
 */
uint8 SDI12_sontek_take_measurement(SDI12_sensor* sensor);

/**
 * @brief Take and read measurments from sontek sensor
 */
sontek_t sontek_take_reading();
#endif
