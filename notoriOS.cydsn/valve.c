#include "valve.h"
#include "notoriOS.h"

// throughout, the percentages recorded in reference to the valve position ought to be
// percent open-ness, not percent closed

test_t valve_test(){

    // activate 12V battery
    Pressure_Voltage_Enable_Write(ON);

    
     
    test_t test; // test_t is a new data type we defined in test.h. We then use that data type to define a structure variable test
    test.status = 0; // set test status to zero
    snprintf(test.test_name,sizeof(test.test_name),"TEST_VALVE");

    float32 positions[8];
    
    voltage_t voltages[8];
    
    // all the way open
    move_valve(1);
    // take a position reading to verify that it's all the way open (later)
    positions[0] = read_Valve_pos();
    voltages[0] = voltage_take_readings();
    // do the same for closed
    test.status = move_valve(0);
    // take a position reading to verify that it's all the way closed (later)
    positions[1] = read_Valve_pos();
    voltages[1] = voltage_take_readings();
    if (test.status){ // don't do this if it's jammed (didn't close successfully)
        move_valve(0.125);
        positions[2] = read_Valve_pos();
        
        move_valve(0.25);
        positions[3] = read_Valve_pos();
        
        move_valve(0.375);
        positions[4] = read_Valve_pos();

        move_valve(0.5);
        positions[5] = read_Valve_pos();

        move_valve(0.675);
        positions[6] = read_Valve_pos();

        move_valve(0.75);
        positions[7] = read_Valve_pos();
    }

    // in test reason report the sequence of positions
    snprintf(test.reason,sizeof(test.reason),"open_voltage:%f:::closed_voltage:%f:::pot_voltage:%f::::positions:%f:%f:%f:%f:%f:%f:%f:%f", 
        voltages[0].voltage_valve_pos, voltages[1].voltage_valve_pos, voltages[0].voltage_valve_pot_power,
        positions[0],positions[1],positions[2],positions[3],
        positions[4],positions[5],positions[6],positions[7]
    );
   
    
    if (!test.status){
        snprintf(test.reason,sizeof(test.reason),"valve jammed or battery dead" );
    }
    // deactivate 12V battery
    Pressure_Voltage_Enable_Write(OFF);
    
    
    //just for now since I can't get the modem to error in the way I want it to
    valve_level_controller(level_sensor_take_reading().level_reading);
    
    return test;

}

float32 read_Valve_pos(){
    // take analog voltage readings
    voltage_t readings = voltage_take_readings();
    return (readings.voltage_valve_pos/readings.voltage_valve_pot_power)/0.94; // divide because of the non-pot resistance   
    
}

uint8 move_valve(float32 position_desired){
    
    // activate 12V battery
    Pressure_Voltage_Enable_Write(ON);
    
    // this uses "go until you're there" rather than pulsing and checking
    // pulsing and checking could set up oscillations in the controller
    // pulsing and checking would only be necessary if measurement consumed a similar amount of time to moving
    
    float32 prev_pos =  1.5;
    
    // are we already there? (wihtin a tolerance)
    if ( fabs(read_Valve_pos() - position_desired) < 0.05){
        return 1;
    }

    // is the desired position more closed?
    if( read_Valve_pos() > position_desired){
        
        // turn the closing pin high
        Power_VDD2_Write(1u);

        
        // while loop
        // continuously measure the position (measurement should be much faster than movement)
        // once we're within 5 percent of desired (can tighten this later) exit this do-while loop
        while(fabs(read_Valve_pos() - position_desired) > 0.03){
            prev_pos = read_Valve_pos();
            CyDelay(1000u);
            // are we moving?
            if (fabs(prev_pos - read_Valve_pos()) < 0.0005){
                // turn the closing pin low
                Power_VDD2_Write(0u);
                // deactivate 12V battery
                Pressure_Voltage_Enable_Write(OFF);
                return 0;
            }
        }
        
        
        // turn the closing pin low
        Power_VDD2_Write(0u);
        
        
        // read valve position once more and confirm we're where we want to be
        // if not return 0
        if(fabs(read_Valve_pos() - position_desired) > 0.05){
            // deactivate 12V battery
            Pressure_Voltage_Enable_Write(OFF);
            return 0;
        }
        
    }
    // or more open?
    else if(read_Valve_pos() < position_desired){
        
        // turn the opening pin high
        Power_VDD1_Write(1u);
        
        // while loop
        // continuously measure the position (measurement should be much faster than movement)
        // once we're within 5 percent of desired (can tighten this later) exit this do-while loop
        while(fabs(read_Valve_pos() - position_desired) > 0.03){
            prev_pos = read_Valve_pos();
            CyDelay(1000u);
            // are we moving?
            if (fabs(prev_pos - read_Valve_pos()) < 0.0005){
                // turn the opening pin low
                Power_VDD1_Write(0u);
                // deactivate 12V battery
                Pressure_Voltage_Enable_Write(OFF);
                return 0;
            }
        }
        
        // turn the opening pin low
         Power_VDD1_Write(0u);
        
        
        // read valve position once more and confirm we're where we want to be
        // if not return 0
        if(fabs(read_Valve_pos() - position_desired) > 0.05){
            // deactivate 12V battery
            Pressure_Voltage_Enable_Write(OFF);
            return 0;
        }
        
        
    }

    
    // deactivate 12V battery
    Pressure_Voltage_Enable_Write(OFF);
    
    return 1; // everything worked fine
}

void valve_level_controller(int16 level_reading){
    printNotif(NOTIF_TYPE_EVENT, "level_controller using level_reading:%d", level_reading);
    if(level_reading > 500){
        move_valve(0);
    }
    else {
        move_valve(1);
    }
}