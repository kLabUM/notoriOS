
// If I haven't defined my file, then define it, if it is defined, then don't try to do it again (stops infinte loop of the compiler).
#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdio.h> //defines three variable types, several macros, and various functions for performing input and output.
#include <stdlib.h> // defines four variable types, several macros, and various functions for performing general functions.
#include <time.h> // defines four variable types, two macro and various functions for manipulating date and time.
#include "testing.h"
#include "project.h"
#include "sd_card.h"
    
// function to get timestamp  
long getTimeStamp();

// Define variables for UART debug
#define USE_DEBUG 1 //set to 1 if debug data will be written to UART
#define MAX_DEBUG_STRING_LENGTH 2048

#define NOTIF_TYPE_EVENT 0u
#define NOTIF_TYPE_WARNING 1u
#define NOTIF_TYPE_ERROR 2u
    


/* Add an explicit reference to the floating point printf library to allow
the usage of floating point conversion specifier */
#if defined (__GNUC__)
    asm (".global _printf_float");
#endif

// functions to start, stop, sleep and wakeup UART debug
void debug_start();
void debug_stop();
void debug_sleep();
void debug_wakeup();

// functions needed for UART debugging
void printd(char* format, ...);
void printNotif(uint8 type, char* format, ...);
void printTestStatus(test_t test);
void stripEscapeCharacters(char *string);
char *strextract(const char input_str[], char output_str[],
                 const char search_start[], const char search_end[]);

    
    
#endif
