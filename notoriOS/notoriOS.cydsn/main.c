#include "main.h"

//this files holds all the "business" callbacks, which are abstracted from the main OS


void makeMeasurements(){
     LED_Write(1u);
     CyDelay(100u);
     LED_Write(0u);
}


/*
TODO:

    - initilizwe modem parameters and cell tower time at bootup
    - GPS lock
    - Write real maxbotixdata to server
    - watchdog -- built-in is too short (2sec), so may need to build one using a timer
    - test power
    - write data to SD card
    - bootloader
    - test power

*/

