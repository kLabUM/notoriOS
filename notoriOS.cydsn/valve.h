#ifndef VALVE_H
#define VALVE_H
#endif

#define ON 1u
#define OFF 0u

#include "project.h"
#include "testing.h"


// TODO:
// define valve_t struct
// this will hold valve_type in the future when linear actuator is integrated
typedef struct { 
    float valve_pos_reading;
    float valve_pos_power;
} valve_position_t;


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

float32 calculate_discharge(float32 current_position);
float32 discharge_coefficient(float32 current_position);
float32 valve_diameter;
char valve_type[10];

// app interface
uint8 App_Valve();

uint8 valve_enabled;
char valve_inbox[100];
void Valve_Update(char * message);

float32 butterfly_Cv_curve(float32 current_position);