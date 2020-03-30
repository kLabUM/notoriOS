

#ifndef _DEBUG_H
#define _DEBUG_H
 
#include "project.h"
#include <stdio.h>
#include <stdlib.h>

//set to 1 if debug data will be written to UART
#define USE_DEBUG 1
#define DEBUG_STRING_LENGTH 100
    
char debug_string[DEBUG_STRING_LENGTH];

/*
#if USE_DEBUG
    #define PRINT_SERIAL(str) sprintf(debug_string,str);Debug_UART_PutString(debug_string);
#else
    #define DEBUG_WRITE(STR)  
#endif
*/

/* Add an explicit reference to the floating point printf library to allow
the usage of floating point conversion specifier */
#if defined (__GNUC__)
    asm (".global _printf_float");
#endif

void debug_start();
void debug_stop();
void debug_sleep();
void debug_wakeup();

    
    
#endif
