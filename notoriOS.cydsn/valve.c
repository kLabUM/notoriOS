#include "valve.h"
#include "notoriOS.h"


test_t valve_test(){
 
     
    test_t test; // test_t is a new data type we defined in test.h. We then use that data type to define a structure variable test
    test.status = 0; // set test status to zero
    snprintf(test.test_name,sizeof(test.test_name),"TEST_VALVE");
    
    for (int pivots = 0; pivots < 2; pivots++){

        
        for (int i = 0; i < 5; i++){
            
            Valve_OPEN_Write(1u);
            CyDelay(5000u);
            //Valve_OPEN_Write(0u);
            
            Valve_CLOSE_Write(1u);
            CyDelay(5000u);
            //Valve_CLOSE_Write(0u);
        }

        // work in checks to make sure valve is moving (and position reading)
        // will need to be able to read the position in order to do this
        // if valve is not moving, return 0 and report an error (probably a blockage or insufficient voltage)

        
    }

    // if test passes, set status to 1
    test.status = 1;
    snprintf(test.reason,sizeof(test.reason),"TODO");
    return test;
    
}