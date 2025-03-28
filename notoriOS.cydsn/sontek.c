#include "sontek.h"
#include "notoriOS.h"

// Simple random number generator implementation
static uint32 rand_seed = 1;
uint32 CyGetRand(void) {
    rand_seed = rand_seed * 1103515245 + 12345;
    return (uint32)(rand_seed / 65536) % 32768;
}

// Initialize global variables
uint8 Sontek_enabled = 0;
char Sontek_inbox[100] = {0};

uint8 App_Sontek(void){
	//In notorios.c will return timeToSontek flag back to 0 (off) until next time
return 0u;
}


void Sontek_Update(char * message){
   strcpy(Sontek_inbox, message);
}


test_t Sontek_test(void){       
   test_t test; // test_t is a new data type we defined in test.h. We then use that data type to define a structure variable test
   test.status = 0; // set test status to zero
   snprintf(test.test_name,sizeof(test.test_name),"TEST_Sontek");
   
   sontek_sensor_t reading = sontek_sensor_take_reading();
   snprintf(test.reason,sizeof(test.reason),"reading=%d,status=%d",
       reading.reading, reading.status);

   return test;
}


// takes level sensor measurements and calculates average level sensor reading
sontek_sensor_t sontek_sensor_take_reading(void){
    
    // level_sensor_t is a new data type we defined in level_sensor.h. We then use that data type to define a structure variable level_sensor_output.
    sontek_sensor_t sontek_sensor_output;

   // Generate random number between 0-65535
    uint16 reading = (uint16)(CyGetRand() % 65536);
    sontek_sensor_output.reading = reading;
    sontek_sensor_output.status = 1;  // 1 for success
    return sontek_sensor_output;
}


/* [] END OF FILE */
