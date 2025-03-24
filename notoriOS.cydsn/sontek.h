#ifndef SONTEK_H__
#define SONTEK_H__

#include <stdio.h>      // Defines three variable types, several macros, and various functions for performing input and 
#include "project.h"
#include "testing.h"

// Add CyGetRand declaration
uint32 CyGetRand(void);

typedef struct {
    uint16 reading;
    uint8 status;
} sontek_sensor_t;


//test_t is a struct type defined in "testing.h" used to run Sontek tests, but not technically a function
test_t Sontek_test(void);


/*=========================================================================
*   App interface initialization
*=========================================================================*/
// "main" function for app, will return 0 to reset timer flag
uint8 App_Sontek(void);
// initialize function to put message (command) into the Sontek inbox 'data wheel'
void Sontek_Update(char * message);
//flag for Sontek app being on or not
uint8 Sontek_enabled;
//initialize inbox for Sontek to read and send messages
extern char Sontek_inbox[100];
// Function declaration
sontek_sensor_t sontek_sensor_take_reading(void);

#endif /* SONTEK_H__ */

/* [] END OF FILE */
