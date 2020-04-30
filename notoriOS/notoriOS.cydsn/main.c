#include "main.h"

//this files holds all the "business" callbacks, which are abstracted from the main OS


void makeMeasurements(){
     LED_Write(1u);
     CyDelay(100u);
     LED_Write(0u);
}


/*
TODO:
    - data structure for modem/system settings (port, end point, etc)
    - sync time with cell-tower time
    - Write real maxbotixdata to server
    - test power
    - write data to SD card
    - bootloader?

*/
