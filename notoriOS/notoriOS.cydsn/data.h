#include <string.h> //defines one variable type, one macro, and various functions for manipulating arrays of characters.
#include <stdlib.h> // defines four variable types, several macros, and various functions for performing general functions.
#include <stdio.h> //defines three variable types, several macros, and various functions for performing input and output.
#include "project.h"
#include "debug.h"

#ifndef DATA_H
#define DATA_H
    

#define DATA_MAX_VALUES 50     // max values that will ever be part of one node id. think of this as the number of sensors. 
#define DATA_MAX_KEY_LENGTH 20  


typedef struct{
    char key[DATA_MAX_KEY_LENGTH];
    char value[DATA_MAX_KEY_LENGTH];
    int32 timeStamp;
}key_value_t;

uint8 pushData(char * key, char * value, int32 timestamp);
uint16 sizeOfDataStack();

void Initialize_Data_Stack();
void Clear_Data_Stack();

uint16 dataPointsInStack; 
key_value_t *dataPointsInStackPointer;
key_value_t data[DATA_MAX_VALUES];

void construct_generic_HTTP_request(char* request, char* body, char* host, char* route,
                               int port, char* method, char* connection_type,
	                           char *extra_headers, int extra_len, char* http_protocol);

unsigned int construct_malcom_body(char* body);
void construct_malcom_route(char* route, char* base, char* device, char* hash);

#endif


