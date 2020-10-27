#ifndef PRESSURES_H
#define PRESSURES_H

#include "project.h"
#include "voltages.h"
#include "testing.h"
    
#define ADC_MUX_PRTRANS 3

#define N_SAMPLES 11

typedef struct { 
    float pressure_voltage;
    uint8 valid;
} pressure_t;

test_t pressures_test();
pressure_t pressure_take_readings();

#endif 