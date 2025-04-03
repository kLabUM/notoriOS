/* [] START OF FILE */

#include <stdio.h>   //Defines some needed variables, macros, and functions
#include "project.h" //Include the header files of all PSOC generated files
#include "testing.h" //Include testing files for pass/fail type tests

//Get the test function from the testing.h file
test_t sontek_test();

// Initialize the app interface
uint8 App_Sontek(); //"main" function for app, will return 0 to reset timer flag (good form to claim C functions in the header file)
void Sontek_Update(char * message); //initialize function to put message (command) into the Sontek inbox 'data wheel'
uint8 sontek_enabled;   //flag variable for Sontek app being on or not
char Sontek_inbox[100]; //initialize inbox for Sontek to read and send messages

typedef struct { //defined to use in notoriOS.c makeMeasurements() and in sontek.c
    int16 sontek_reading;
} sontek_t;

sontek_t sontek_take_reading();

/* [] END OF FILE */
