

#include "project.h"
#include <stdlib.h>
#include <string.h>


typedef struct { 
    char  test_name[128];//Name of test
    uint8 status; //retursn status code, 0=FAIl, 1=PASS, Also suppoert custom codes
    char  reason[256];//string that expalins reason for test status 
} test_t;