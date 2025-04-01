#include <stdio.h>      // Defines three variable types, several macros, and various functions for performing input and 
#include "project.h"
#include "testing.h"


//test_t is a struct type defined in “testing.h” used to run Sontek tests, but not technically a function
test_t Sontek_test();


/*=========================================================================
*   App interface initialization
*=========================================================================*/
// "main" function for app, will return 0 to reset timer flag
uint8 App_Sontek();
// initialize function to put message (command) into the Sontek inbox 'data wheel'
void Sontek_Update(char * message);
//flag for Sontek app being on or not
uint8 Sontek_enabled;
//initialize inbox for Sontek to read and send messages
char Sontek_inbox[100];
