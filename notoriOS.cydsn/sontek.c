/* [] START OF FILE */

#include "sontek.h"   //include the sontek header file where the app interface is initialized
#include "notoriOS.h" //include notorios header for general variables, app interface / alarm variables

void Sontek_Update(char * message){
    strcpy(Sontek_inbox, message);
}

// Sontek sensor test
test_t sontek_test(){
    
    test_t test; // test_t is a new data type we defined in testing.h. We then use that data type to define a structure variable test
    test.status = 0; // set test status to zero
    snprintf(test.test_name,sizeof(test.test_name),"TEST_SONTEK_SENSOR");

    // sontek_t is a new data type we defined in sontek.h. We then use that data type to define a structure variable sensor
    sontek_t sensor = sontek_take_reading();
    
    // print the "reading" which should just be the rand num
    snprintf(test.reason,sizeof(test.reason),"nRAND_NUM=%d.",
        sensor.sontek_reading
    );
   
    // if the random number exists since they should all be above 0, pass the sensor
    if(sensor.sontek_reading > -1){//pass
        test.status = 1;
    }
    
    return test;
}

uint8 App_Sontek(){
    char * compare_location;
    
    compare_location = strstr(Sontek_inbox,"OFF");        
    if(compare_location!=NULL){
        return 0; // this app is disabled, don't do anything
    }
compare_location = strstr(Sontek_inbox,"ON");        
    if(compare_location!=NULL){
        // below is copied and adapted from uint8 level_sensor() in level_sensor.c
        
        // Get clock time and save to timeStamp
        long timeStamp = getTimeStamp();
        char c_timeStamp[32];
        snprintf(c_timeStamp,sizeof(c_timeStamp),"%ld",timeStamp);
        
        // Holds string for any value that will be written, but here it is the sensor value specifically
        char value[DATA_MAX_KEY_LENGTH];
        
        // there was some code here to define sontek_t, but I did that in makeMeasurements() in notoriOS.c so I removed it        

        return 0;
    }
    
    return 0; // i think this needs to be rewritten to be more like a light switch
    // i.e. it stays on until you turn it off.
}

// Initialize random seed at program start
void sontek_init() {
    srand(time(NULL));
}

// makes a random number and returns it like a Sontek reading
sontek_t sontek_take_reading(){
    
    // Initialize random number generator if not already done
    static uint8 initialized = 0;
    if (!initialized) {
        sontek_init();
        initialized = 1;
    }
    
    // sontek_t is a new data type we defined in sontek.h. We then use that data type to define a structure variable sontek_output.
    sontek_t sontek_output;
    
    // Generate random number between 1 and 100
    int min = 1;
    int max = 100;
    int random_number = (rand() % (max - min + 1)) + min;
    sontek_output.sontek_reading = random_number;
    
    sontek_output.num_valid_readings = 1;  // Since we got a valid reading, set to 1
    
    return sontek_output;
}

/* [] END OF FILE */
