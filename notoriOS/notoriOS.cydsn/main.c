#include "main.h"

//this files holds all the "business" callbacks, which are abstracted from the main OS


void makeMeasurements(){
     LED_Write(1u);
     CyDelay(100u);
     LED_Write(0u);
}

