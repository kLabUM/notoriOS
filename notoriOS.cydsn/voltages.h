#ifndef VOLTAGES_H
#define VOLTAGES_H

#include "project.h"
#include "testing.h"
#include <stdio.h>
#include <string.h>


#define ADC_MUX_OFFSET 1
#define ADC_MUX_VBAT 1
#define ADC_MUX_VSOL 2
#define ADC_MUX_PRTRANS 3
#define ADC_MUX_Valve_POS_reading 4
#define ADC_MUX_Valve_POS_Power 5
#define ADC_MUX_Valve2_POS_reading 6
#define ADC_MUX_Valve2_POS_Power 7

#define N_SAMPLES 11

typedef struct { 
    float voltage_battery;
    float voltage_solar;
    float voltage_charge_current;
    float voltage_pressure;
    uint8 valid;
} voltage_t;

typedef struct {
    float pressure_voltage_on;
    float pressure_voltage_off;
    float pressure_current;
    float pressure_depth;
} pressure_t;

void sort32(int32 a[],int32 n);
void swap32(int32 *p,int32 *q);
int32 find_median32(int32 array[] , int32 n);
test_t voltages_test();
voltage_t voltage_take_readings();
test_t pressure_sensor_test();
pressure_t pressure_sensor_readings();
pressure_t pressure_calculations(voltage_t voltage);

#endif 