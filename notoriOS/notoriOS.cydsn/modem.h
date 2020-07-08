//modem header
#ifndef MODEM_H
#define MODEM_H   
    
#include <project.h>  
#include <stdio.h> //defines three variable types, several macros, and various functions for performing input and output.
#include <stdlib.h> // defines four variable types, several macros, and various functions for performing general functions.
#include <string.h> //defines one variable type, one macro, and various functions for manipulating arrays of characters.
#include <time.h> // defines four variable types, two macro and various functions for manipulating date and time.
#include "testing.h"
    
    
#define MODEM_STATE_OFF 0u
#define MODEM_STATE_STARTUP 1u
#define MODEM_STATE_WAITING_FOR_NETWORK 2u
#define MODEM_STATE_WAITING_FOR_IP 3u
#define MODEM_STATE_READY 4u
    
    
    
uint8 modem_state;
uint8 modem_process_tasks();

void modem_intilize();
uint8 modem_power_up();
void modem_power_down();
void modem_soft_power_cycle();
void pins_configure_inactive();
void pins_configure_active();
test_t modem_test();
void modem_configure_settings();
uint8 is_connected_to_cell_network();
void get_cell_network_stats();
void set_up_internet_connection();
uint8 is_connected_to_internet();
uint8 modem_get_state();
void modem_sleep();
void modem_wakeup();
uint8_t at_write_command(char* commands, char* expected_response,uint32_t timeout);
uint8 extract_string(char* from, const char* beginMarker, const char* endMarker,  char* target);
long modem_get_network_time();


typedef struct { 
    char sim_id[20];
    char model_id[20];
    char imei[20];
} modem_info_t;

typedef struct { 
    uint8 rssi;     //received signal strength indication
    uint8 sq;       //signal quality, 
    int   time_to_network_connect; 
    int   time_to_acquire_ip;
} modem_stats_t;

typedef struct{
    float latitude;
    float longitude;
    float altitude;
    uint16 time_to_lock;
    uint8 valid;
    
} gps_t;

gps_t modem_get_gps_coordinates();


int32 modem_start_time_stamp;
modem_info_t modem_info;
modem_stats_t modem_stats;

    
#endif
