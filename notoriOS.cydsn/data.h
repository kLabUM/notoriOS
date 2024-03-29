#ifndef _DATA_H
#define _DATA_H
    
#include <string.h>     // defines one variable type, one macro, and various functions for manipulating arrays of characters.
#include <stdlib.h>     // defines four variable types, several macros, and various functions for performing general functions.
#include <stdio.h>      // defines three variable types, several macros, and various functions for performing input and output.
#include <math.h>       // designed for basic mathematical operations. Most of the functions involve the use of floating point numbers. 
#include <stdbool.h>    // defines four macros for a Boolean data type
#include "project.h"
#include "debug.h"
    

#define DATA_MAX_VALUES 150     // max values that will ever be part of one node id. think of this as the number of sensors. 
#define DATA_MAX_KEY_LENGTH 20  

// Boolean variable for buffer overflow
bool buffer_overflow; 
    
// Creates data structure for the data wheel including the data point name and value as well as a timestamp
typedef struct{
    char key[DATA_MAX_KEY_LENGTH];
    char value[DATA_MAX_KEY_LENGTH];
    int32 timeStamp;
    
}key_value_t;

uint8 pushData(char * key, char * value, int32 timestamp);
uint16 sizeOfDataStack();
uint16 sizeOfDataStackDesired();

void Initialize_Data_Stack();
void Clear_Data_Stack();

uint16 dataPointsInStack; 
uint16 dataPointsInStackDesired; 
key_value_t data[DATA_MAX_VALUES];

void construct_generic_HTTP_request(char* request, char* body, char* host, char* route,
                               int port, char* method, char* connection_type,
	                           char *extra_headers, int extra_len, char* http_protocol);

unsigned int construct_malcom_body(char* body);
void construct_malcom_route(char* route, char* base, char* device, char* hash);

#endif