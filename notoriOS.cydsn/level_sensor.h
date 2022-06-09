#ifndef UART_H
#define UART_H

#include <assert.h>     // Implements a runtime assertion, which can be used to verify assumptions made by the program and print a diagnostic message if this assumption is false. 
#include <stdint.h>     // Defines a set of integral type aliases with specific width requirements, along with macros specifying their limits and macro functions to create values of these types.
#include <stdbool.h>    // Contains four macros for a Boolean data type.
#include <math.h>       // Designed for basic mathematical operations. Most of the functions involve the use of floating point numbers. 
#include <stdio.h>      // Defines three variable types, several macros, and various functions for performing input and output.
#include "project.h"
#include "project.h"
#include "testing.h"
    
#define N_READINGS 11
#define READING_INVALID 9999   // This should to be set to 5000 if we're using a short-range sensor
    
typedef struct { 
    int16 all_level_readings[N_READINGS];
    int16 level_reading;
    uint8 num_valid_readings;
    uint8 num_invalid_readings;
} level_sensor_t;
    
//ISR: Interrupt Service Routine. The macro CY_ISR_PROTO declares an ISR function prototype.
CY_ISR_PROTO(Level_Sensor_ISR);
const char *uart_string(void);
uint8_t uart_len(void);
void uart_clear(void);
void wait_for(const char *regex, uint32_t timeout);
level_sensor_t level_sensor_take_reading();
test_t level_sensor_test();
void sort(int16 a[],int16 n);
void swap(int16 *p,int16 *q);
uint16 find_median(int16 array[] , uint8 n);
uint8 level_sensor();

// application interface
char level_sensor_inbox[100];
void Level_Sensor_Update(char * message);


#endif

