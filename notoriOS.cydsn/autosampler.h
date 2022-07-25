/**
 * @file autosampler.h
 * @brief Declares functions for ISCO autosampler
 * @author Brandon Wong, Matt Bartos, and Meagan Tobias
 * @version TODO
 * @date 2022-07-18
 */

#ifndef AUTOSAMPLER_H   // checks if Autosampler.h macros already compiled, if not then while compile rest
#define AUTOSAMPLER_H

#include <stdio.h>      // Defines three variable types, several macros, and various functions for performing input and 
#include "project.h"
#include "testing.h"

/* includes from old version, but might be over kill
#include <stdlib.h> 
#include <string.h>
*/

#define ON 1u
#define OFF 0u

/*=========================================================================
*   Autosampler flags and initialization based off of physical sampler
*=========================================================================*/
#define AUTOSAMPLER_STATE_OFF       0
#define AUTOSAMPLER_STATE_IDLE      1
#define AUTOSAMPLER_STATE_BUSY      2
#define MAX_BOTTLE_COUNT            24
#define PULSE_COUNT                 19  
/*
Autosampler is programmed for 20 pulses on physical device, 
Turning on and off of relay counts as 1 pulse. 
Having firmware pulse count at 20 would mess up the bottle count on the sampler over time -maybe, or just be 1 extra
*/


/*=========================================================================
*   Variable initialization
*=========================================================================*/
extern uint8    autosampler_state;
extern uint8    SampleCount;

//test_t is a struct type defined in "testing.h" used to run autsoampler tests, but not technically a function
test_t autosampler_test();

/*=========================================================================
*   Function initialization
*=========================================================================*/

/**
 * @brief Starts the autosampler UART
 */
void autosampler_start();

/**
 * @brief Stops the autosampler UART
 */
void autosampler_stop();

/**
 * @brief Powers on the autosampler
 */
void autosampler_power_on();

/**
 * @brief Powers off the autosampler
 */
void autosampler_power_off();

/**
 * @brief Takes a sample with the autosampler and writes current bottle count to @p count
 *
 * @param count The current bottle count to be updated
 *
 * @return 1u on success  ---------------------------or do "flags" for if fails
 */
uint8 autosampler_take_sample(uint8 *count);

/** ---- not done but maybe will come back to one day
 * @brief Inserts current values of @p autosampler_trigger and @p bottle_count into labels and readings arrays.
 *
 * @param labels Array to store labels corresponding to each trigger result
 * @param readings Array to store trigger results as floating point values
 * @param array_ix Array index to label and readings
 * @param autosampler_trigger Current value of autosampler trigger
 * @param bottle_count Current bottle count
 * @param max_size Maximum size of label and reading arrays (number of entries)
 *
 * @return (*array_ix) + number of entries filled
 */
uint8 zip_autosampler(char *labels[], float readings[], uint8 *array_ix, int *autosampler_trigger, uint8 *bottle_count, uint8 max_size);


/*=========================================================================
*   App interface initialization
*=========================================================================*/
// "main" function for app, will return 0 to reset timer flag
uint8 App_Autosampler();
// initialize function to put message (command) into the autosampler inbox 'data wheel'
void Autosampler_Update(char * message);

 //flag for autosampler app being on or not
uint8 autosampler_enabled;
//initialize inbox for autosampler to read and send messages
char autosampler_inbox[100];

#endif