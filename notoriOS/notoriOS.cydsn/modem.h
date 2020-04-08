//modem header
#ifndef MODEM_H
#define MODEM_H   
    
#include <project.h>  
#include "testing.h"

void modem_intilize();
void modem_power_up();
void modem_power_down();
void modem_soft_power_cycle();
void pins_configure_inactive();
void pins_configure_active();
test_t modem_test();
    
#endif
