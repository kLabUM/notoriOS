#include "project.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "debug.h"

#define DATA_MAX_VALUES 250     //max vlaues that will ever be part of one node id. think of this as the numebr of senors
#define DATA_MAX_KEY_LENGTH 20  


typedef struct{
    char key[DATA_MAX_KEY_LENGTH];
    char value[DATA_MAX_KEY_LENGTH];
    int32 timeStamp;
}key_value_t;

uint8 pushData(char * key, char * value, int32 timestamp);
key_value_t popData();
uint16 sizeOfDataStack();

void Initialize_Data_Stack();
void Clear_Data_Stack();


uint16 dataPointsInStack; 
key_value_t data[DATA_MAX_VALUES];

void construct_generic_HTTP_request(char* request, char* body, char* host, char* route,
                               int port, char* method, char* connection_type,
	                           char *extra_headers, int extra_len, char* http_protocol);




