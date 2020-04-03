

#ifndef _DEBUG_H
#define _DEBUG_H
 
#include "project.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "testing.h"
    
    
long getTimeStamp();

//set to 1 if debug data will be written to UART
#define USE_DEBUG 1
#define DEBUG_STRING_LENGTH 100
    
char debug_string[DEBUG_STRING_LENGTH];


/* Add an explicit reference to the floating point printf library to allow
the usage of floating point conversion specifier */
#if defined (__GNUC__)
    asm (".global _printf_float");
#endif

void debug_start();
void debug_stop();
void debug_sleep();
void debug_wakeup();
void printNotif(char* format, ...);
void printTestStatus(test_t test);
void stripEscapeCharacters(char *string);
    
    
#endif
