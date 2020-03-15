
#ifndef TEST_H__
#define TEST_H__

#include "project.h"
#include <stdlib.h>
#include <string.h>


typedef struct { 
    char  test_name[128];//Name of test
    uint8 status; //returns status code, 0=FAIl, 1=PASS, Also suppoert custom codes
    char  reason[256];//string that expalins reason for test status 
} test_t;

#endif /* TEST_H__*/