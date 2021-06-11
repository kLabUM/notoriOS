#ifndef SONTEK_H
#define SONTEK_H
 
#include "project.h"
#include "SDI12.h"

#define SONTEK_WRITE_TIME 5000  //Time delay to leave sensor on to give it enough time to write measurements on sensor recorder

#define SONTEK_READING_INIT 0       //initializing
#define SONTEK_READING_VALID 1      // Successfully took reading 
#define SONTEK_READING_PARSE_ERROR 2//Sensor is on, but not parsing 
#define SONTEK_READING_NOT_RESPONDING 3 

/* Specific for beam velocity mode */
#define MD0 0 //starting index for sontek_t.values for sending commands M + D0
#define MD1 4 //starting index for sontek_t.values M + D1   
#define M1D0 9 //starting index for sontek_t.values M1 + D0
#define M2D0 15 //starting index for sontek_t.values M2 + D0
#define M3D0 21 //starting index for sontek_t.values M3 + D0 
#define M4D0 27 //starting index for sontek_t.values M4 + D0    
#define M5D0 33 //starting index for sontek_t.values M5 + D0    
#define M6D0 39 //starting index for sontek_t.values M6 + D0    
#define M7D0 45 //starting index for sontek_t.values M7 + D0
#define M8D0 49 //starting index for sontek_t.values M8 + D0

/* 
 * Define a struct to be used for readings
 */
typedef struct {
    
    uint8 nvars;
    uint8 flag;          //assign flags
    //maybe delete values
    float* values;      // Array of floats for the value returned for each measurement
    char** labels;      // Array of Strings to label each sensor measurement
    //Arrays of floats to store each reading chunk separately due to memory issues
    float data0[MD1-MD0], data1[M1D0-MD1], data2[M2D0-M1D0], data3[M3D0-M2D0], data4[M4D0-M3D0], data5[M5D0-M4D0], data6[M6D0-M5D0], data7[M7D0-M6D0], data8[M8D0-M7D0], data9[SONTEK_NVARS-M8D0];
    
    
} sontek_t;    

/////////**
//////// * @brief Sends command to take reading with specific M and D commands
//////// *
//////// * @param M int8 measurement command number
//////// * @param D int8 read data command number
////////
//////// */
////////void sontek_get_data(SDI12_sensor* sensor, sontek_t* sontek_sensor, int8 M, int8 D);

/**
 * @brief Take a measurement specifically for sontek sensor, given its address. Special case of SDI_take_measurement() in SDI12.c
 * 
 * @param sensor SDI12_sensor that we want to request measurements from
 * @param sontek_sensor sontek_t that we want to store measurements to
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
