#ifndef VALVE_H
#define VALVE_H
#endif

#define ON 1u
#define OFF 0u

#include <project.h>
#include "testing.h"

void Valve_OPEN_Write(uint8);
void Valve_CLOSE_Write(uint8);

  
test_t valve_test();