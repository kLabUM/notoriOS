#ifndef UART_H
#define UART_H

//#include "notoriOS.h"
#include "project.h"
#include "testing.h"
    
    
#define N_READINGS 11
#define READING_INVALID 9999   //this should to be set to 5000 if we're using a short-range sensor
    
typedef struct { 
    int16 all_level_readings[N_READINGS];
    int16 level_reading;
    uint8 num_valid_readings;
    uint8 num_invalid_readings;
} level_sensor_t;
    

CY_ISR_PROTO(Level_Sensor_ISR);
const char *uart_string(void);
uint8_t uart_len(void);
void uart_clear(void);
void wait_for(const char *regex, uint32_t timeout);
level_sensor_t level_sensor_take_reading();
test_t level_sensor_test();
void sort16(int16 a[],int16 n);
void swap16(int16 *p,int16 *q);
uint16 find_median16(int16 array[] , uint8 n);


#endif

