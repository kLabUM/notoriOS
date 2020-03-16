#ifndef VOLTAGES_H
#define VOLTAGES_H

#include "project.h"
#include "testing.h"


#define ADC_MUX_VBAT 0
#define ADC_MUX_VSOL 1
#define ADC_MUX_CHRG 2

#define N_SAMPLES 11

typedef struct { 
    float voltage_battery;
    float voltage_solar;
    float voltage_charge_current;
    uint8 valid;
} voltage_t;

void sort32(int32 a[],int32 n);
void swap32(int32 *p,int32 *q);
int32 find_median32(int32 array[] , int32 n);
test_t voltages_test();

#endif 