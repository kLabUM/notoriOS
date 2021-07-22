#ifndef VALVE_H
#define VALVE_H
#endif

#define ON 1u
#define OFF 0u

#include <project.h>
#include "testing.h"

// for opening and clsing valve
//open
void Power_VDD1_Write(uint8);
//close
void Power_VDD2_Write(uint8);

// for closing the 12V circuit and powering the valve
// using the level sensor power pin for now

// tests actuation and position reading
test_t valve_test();

float32 read_Valve_pos();

uint8 move_valve(float32 position_desired);
