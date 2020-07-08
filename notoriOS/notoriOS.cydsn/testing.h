
#ifndef TEST_H__
#define TEST_H__

#include "project.h"
#include <stdlib.h> // defines four variable types, several macros, and various functions for performing general functions.
#include <string.h> // defines one variable type, one macro, and various functions for manipulating arrays of characters.

// Creates data structure test_t for testing
typedef struct { 
    // creates an array of characters (string) named test_name with 128 elements
    char  test_name[128];//Name of test
    uint8 status; //returns status code, 0=FAIl, 1=PASS, Also suppoert custom codes
    // creates an array of characters (string) named reason with 256 elements
    char  reason[256];//string that expalins reason for test status 
} test_t;

#endif /* TEST_H__*/