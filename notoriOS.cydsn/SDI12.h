/**
 * @file SDI12.h
 * @brief Implements functions for SDI12 sensors.
 * @author Brandon Wong
 * @version TODO
 * @date 2017-10-28
 */
#ifndef SDI12_H
#define SDI12_H

#include "project.h"
#include <stdio.h>  /* NULL */
#include <stdlib.h> /* strtod */


/*
  Define useful commands for SDI-12
  See Table 5 <www.sdi-12.org/archives/SDI-12 Specification 1.3 April 7 2000.pdf>
    
    - The first character of all commands and responses is always a device address (e.g., 0-9, a-z, A-Z)
    - The last character of a command is the "!" character
    - the last two bytes of a response are <CR><LF>
*/
//Sontek uses more than just "M" and "D0" commands for requesting and reading measurements so M and D commands are made in-function
    //0 is used for the default "M" and "D" commands still
#define TAKE_MEASUREMENT    0  // Request Single Measurement
#define READ_MEASUREMENT    0 // Read Completed Measurement
#define CONC_MEASUREMENT    "C"  // Request Concurrent Measurement
#define ADDR_QUERY          "?"  // Get Sensor Address
#define ACK_ACTIVE          ""   // Check if the sensor is active
#define CHANGE_ADDR         "A"  // Change the addreess of the sensor
#define INFO                "I"  // Get sensor info
    
#define SONTEK_ADDRESS      "0" // Address of Sontek sensor
#define SONLIST_ADDRESS     "1" // Address of Sonlist sensor
#define DECAGON_ADDRESS     "2" // Address of Decagon sensor
    
#define SONTEK_NVARS        54  //Number of variables sontek sensor can return 27 for Default mode, 54 for Beam profile
#define SONLIST_NVARS       2  
#define DECAGON_NVARS       3  
    
/* 
 * Define a struct to be used for each sensor
 */
struct {
    /* core variables used for taking measurements */
    char*  address;     // String for the sensor address, should be 1 char (e.g. 0-9, a-z, A-Z)
    char** labels;      // Array of Strings to label each sensor measurement
    float* values;      // Array of floats for the value returned for each measurement
    uint8  nvars;       // The number of measurements to be returned by the SDI-12 sensor
    /* metadata */
    char   v_SDI12[3];  // SDI-12 software version number
    char   vendor[9];   // Sensor vendor
    char   model[7];    // Sensor model
    char   version[4];  // Sensor model - version number
    char   serial[14];  // Extra sensor data
} typedef SDI12_sensor;

void  SDI12_start();
void  SDI12_stop();
void  SDI12_uart_clear_string();
char* SDI12_uart_get_string();

/**
 * @brief Send an SDI12-formatted command. A command always starts with
 *        the address of the sensor, followed by the command, and is terminated by 
 *        and exclamation point: <address><command>!
 * 
 * @param command String of the command to send
 *
 */
void SDI12_send_command(char command[]);

/**
 * @brief Check if an SDI12 sensor is on, given its address
 * 
 * @param sensor SDI12_sensor that we want to check the status of
 *
 * @return 1u if active, 0u if inactive
 */
uint8 SDI12_is_active(SDI12_sensor* sensor);

/** 
 * @brief Change the address of an SDI12 sensor
 *
 * @param sensor SDI12_sensor that we want to change the address of
 * @param new_address String (single char, either 0-9, a-z, A-Z) of the desired address
 *
 * @return 1u if successful, 0u if unsuccessful
 */
uint8 SDI12_change_address(SDI12_sensor* sensor, char new_address[]);

/**
 * @brief Take a measurement for an SDI12 sensor, given its address
 * 
 * @param sensor SDI12_sensor that we want to request measurements from
 * @param take_meas_number int8 is the take measurement command number
 * @param read_meas_number int8 is the read measurement command number
 *
 * @ return 1u if measurements were successfully taken, 0u if there was a communication error
 */
uint8 SDI12_take_measurement(SDI12_sensor* sensor, int8 take_meas_number, int8 read_meas_number);
    
/**
 * @brief TODO: Take a concurrent measurement for an SDI12 sensor, given its address
 * 
 * @param sensor SDI12_sensor that we want to request measurements from
 *
 * @ return if sensor is active
 */
uint8 SDI12_take_concurrent_measurement(SDI12_sensor* sensor);

/**
 * @brief Obtain detailed info about an SDI12 sensor, given its address
 * 
 * @param sensor SDI12_sensor that we want to request info from
 *
 * @ return 1u if successful, 0u if unsuccessful
 */
uint8 SDI12_info(SDI12_sensor* sensor);

/**
 * @brief Inserts current values of all SDI12 sensors into labels and
 * readings arrays.
 *
 * @param take_meas_number int8 is the take measurement command number
 * @param read_meas_number int8 is the read measurement command number
 * @param labels Array to store labels corresponding to each sensor reading
 * @param readings Array to store sensor readings as floating point values
 * @param array_ix Array index to label and readings
 * @param max_size Maximum size of label and reading arrays (number of
 * entries)
 *
 * @return (*array_ix) + number of entries filled
 */
uint8 zip_SDI12(int SDI12_flag, int8 take_meas_number, int8 read_meas_number);

/**
 * @brief Resets a string to null bytes.
 *
 * @param str String to reset
 *
 * @return 1u on success, 0u otherwise
 */
uint8_t clear_str(char* str);



CY_ISR_PROTO(isr_SDI12_data);

#endif
/* [] END OF FILE */


/* [] END OF FILE */
