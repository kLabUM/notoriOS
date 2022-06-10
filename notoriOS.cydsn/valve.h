#ifndef VALVE_H
#define VALVE_H
#endif

#define ON 1u
#define OFF 0u

#include <project.h>
#include "testing.h"
#include "downstream_level_sensor.h"

// TODO:
// define valve_t struct
// this will hold valve_type in the future when linear actuator is integrated


// for opening and clsing valve
//open
void Power_VDD1_Write(uint8);
//close
void Power_VDD2_Write(uint8);

// for closing the 12V circuit and powering the valve
// using the pressure sensor power pin

// tests actuation and position reading
test_t valve_test();

float32 read_Valve_pos();

uint8 move_valve(float32 position_desired);

// fallback control strategy for when cell modem goes dark
void valve_level_controller(int16 level_reading);

float32 calculate_discharge();

