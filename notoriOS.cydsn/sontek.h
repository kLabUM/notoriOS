#ifndef SONTEK_H
#define SONTEK_H
 
#include "project.h"
#include "SDI12.h"
 
/* 
 * Define a struct to be used for readings
 */
typedef struct {
    uint8 valid_readings;
} sontek_t;    

/**
 * @brief Take and read measurments from sontek sensor
 */
sontek_t sontek_take_reading();
#endif
