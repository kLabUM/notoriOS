#include "valve.h"
#include "notoriOS.h"


test_t valve_test(){
 
     
    test_t test; // test_t is a new data type we defined in test.h. We then use that data type to define a structure variable test
    test.status = 0; // set test status to zero
    snprintf(test.test_name,sizeof(test.test_name),"TEST_VALVE");

    float32 positions[8];
    
    voltage_t voltages[8];
    
    // all the way open
    Power_VDD1_Write(1u);
    CyDelay(20000u); // check to confirm this is long enough
    Power_VDD1_Write(0u);
    // take a position reading and verify that it's all the way open
    positions[0] = read_Valve_pos();
    voltages[0] = voltage_take_readings();
    // do the same for closed
    Power_VDD2_Write(1u);
    CyDelay(20000u);
    Power_VDD2_Write(0u);
    // take a position reading and verify that it's all the way closed
    positions[1] = read_Valve_pos();
    voltages[1] = voltage_take_readings();
    // initial displacement
    // open valve on
    Power_VDD1_Write(1u);
    CyDelay(8000u);
    //open valve off
    Power_VDD1_Write(0u);

    // these readings should repeat consistently somewhere in the middle range
    
    for (int i = 2; i < 8; i++){
        /*
        if ((i+1)%2){
            // open valve on
            Power_VDD1_Write(1u);

            CyDelay(4000u);
     
            //open valve off
            Power_VDD1_Write(0u);

            CyDelay(4000u);
        }
        else if(i%2){
            //close valve on
            Power_VDD2_Write(1u);

            CyDelay(4000u);

            //close valve off
            Power_VDD2_Write(0u);

            CyDelay(4000u);
        }
        else {
            test.status=0;
            snprintf(test.reason,sizeof(test.reason),"failed in pivot");  
            return test;
        }
        */
        // for now just slowly open
        // open valve on
        Power_VDD1_Write(1u);

        CyDelay(4000u);

        //open valve off
        Power_VDD1_Write(0u);

        CyDelay(4000u);
        // read position and record this
        positions[i] = read_Valve_pos();
        voltages[i] = voltage_take_readings();
        
    }

    // if valve is not moving, return 0 and report an error (probably a blockage or insufficient voltage)

    // in test reason report the sequence of positions
    // should look like 100% open - 0% open - X% - Y% - X% - Y% - X% - Y%
    // this is what determines whether the test "passes"
    // because not only is the valve opening and closing properly, we're also measuring it properly

    // if test passes, set status to 1
    if (positions[0] < 0.1 && positions[1] > 0.90){
        test.status = 1;
    }
    else{
        test.status=0;
        snprintf(test.reason,sizeof(test.reason),"not fully closed or not fully opened");
        return test;
    }
    for (int i = 0; i < 8; i++){
        positions[i] = positions[i]*100;
    }
    // print the series of pivot positions
    snprintf(test.reason,sizeof(test.reason),"voltages_blue_brown:%f:%f:%f:%f:%f:%f:%f:%f", 
        voltages[0].voltage_valve_pos_blue, voltages[0].voltage_valve_pos_brown, 
        voltages[1].voltage_valve_pos_blue, voltages[1].voltage_valve_pos_brown,
        voltages[2].voltage_valve_pos_blue, voltages[2].voltage_valve_pos_brown,
        voltages[3].voltage_valve_pos_blue, voltages[3].voltage_valve_pos_brown
    );
   
    return test;
    
    // ratio of blue/brown is 0 when fully open and 1 when fully closed
    
}

float32 read_Valve_pos(){
    
    
    
    return voltage_take_readings().voltage_valve_pos_blue/voltage_take_readings().voltage_valve_pos_brown;
}

