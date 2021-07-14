#include "valve.h"
#include "notoriOS.h"


test_t valve_test(){
 
     
    test_t test; // test_t is a new data type we defined in test.h. We then use that data type to define a structure variable test
    test.status = 0; // set test status to zero
    snprintf(test.test_name,sizeof(test.test_name),"TEST_VALVE");

    // all the way open
    Power_VDD1_Write(1u);
    CyDelay(20000u); // check to confirm this is long enough
    Power_VDD1_Write(0u);
    // take a position reading and verify that it's all the way open
    
    
    // do the same for closed
    Power_VDD2_Write(1u);
    CyDelay(20000u);
    Power_VDD2_Write(0u);
    // take a position reading and verify that it's all the way closed
    
    // these readings should repeat consistently somewhere in the middle range
    // initial displacement
    // open valve on
    Power_VDD1_Write(1u);
    CyDelay(4000u);
    //open valve off
    Power_VDD1_Write(0u);
        
    for (int i = 0; i < 3; i++){
        
        
        // open valve on
        Power_VDD1_Write(1u);

        CyDelay(4000u);
 
        //open valve off
        Power_VDD1_Write(0u);
        
        CyDelay(4000u);
        // read position and record this
        
        //close valve on
        Power_VDD2_Write(1u);

        CyDelay(4000u);

        //close valve off
        Power_VDD2_Write(0u);
        
        CyDelay(4000u);
        // read position and record this
    }

    // if valve is not moving, return 0 and report an error (probably a blockage or insufficient voltage)

    // in test reason report the sequence of positions
    // should look like 100% open - 0% open - X% - Y% - X% - Y% - X% - Y%

    // if test passes, set status to 1
    test.status = 1;
    snprintf(test.reason,sizeof(test.reason),"TODO");
    
   
    return test;
    
}