//#include "valve.h"
#include "notoriOS.h"

// throughout, the percentages recorded in reference to the valve position ought to be
// percent open-ness, not percent closed

// app interface
uint8 App_Valve(){
    long timestamp = getTimeStamp();
    
    char * compare_location;
    compare_location = strstr(valve_inbox, "Open:");
    float pos_des_float = 1.7;
    
  
    
    if (compare_location != NULL){ // we got a command
        char valve_diameter_charstring[10];
        //extract_string(valve_inbox,"Valve_Diameter_Inches:",",",valve_diameter_charstring);
        //sscanf(valve_diameter_charstring, "%f", &valve_diameter);
        extract_string(valve_inbox,"Valve_Type:",",",valve_type);
        
        
        compare_location = strstr(valve_type,"LinAc");
        if (compare_location != NULL){
            printNotif(NOTIF_TYPE_ERROR, "\r\n\r\nWARNING for Linear Actuator (Two-Wire Actuation Scheme)\r\n\r\nDo not connect programmer and 12V battery at the same time. Programmer may bring VDD1 and VDD2 both high which will cause a short.\r\n\r\n");
        }
        
        // in the case of the linear actuator, take open percent to mean percent retracted
        char position_desired[10];
        extract_string(valve_inbox,"Open:",",",position_desired); 
        sscanf(position_desired, "%f", &pos_des_float);
        uint8 success = move_valve(pos_des_float,1);
        char s_success[4];
        
        // let the server know whether the move was successful
        snprintf(s_success, sizeof(s_success), "%u", success);
        pushData("Valve_move_successful:", s_success, timestamp);
        
        // and the requested position
        pushData("Valve_Desired_Position:",position_desired, timestamp);
        // push up the current position. need to know valve type to do this
        float32 current_pos = read_Valve_pos();
        char s_current_pos[5];
        snprintf(s_current_pos, sizeof(s_current_pos), "%f",current_pos);
        pushData("Valve_Current_Position:",s_current_pos, timestamp);
        
        // if we've got a second linear actuator in this node
        compare_location = strstr(valve_type, "LinAc2");
        if (compare_location != NULL){
            printNotif(NOTIF_TYPE_ERROR, "\nDual Linear Actuator\n");
            char position_desired[10];
            extract_string(valve_inbox,"Open2:",",",position_desired);
            sscanf(position_desired,"%f",&pos_des_float);
            uint8 success = move_valve(pos_des_float,2);
            char s_success[4];
            // let the server know whether the move was successful
            snprintf(s_success, sizeof(s_success), "%u", success);
            pushData("Valve2_move_successful:", s_success, timestamp);
            
            // and the requested position
            pushData("Valve2_Desired_Position:",position_desired, timestamp);
            // push up the current position. need to know valve type to do this
            float32 current_pos = read_Valve_pos();
            char s_current_pos[5];
            snprintf(s_current_pos, sizeof(s_current_pos), "%f",current_pos);
            pushData("Valve2_Current_Position:",s_current_pos, timestamp);
                
        
        }
        
        // calculate and post discharge after moving
        //float32 flow = calculate_discharge(current_pos);
        //char s_flow[10];
        //snprintf(s_flow, sizeof(s_flow), "%f",flow);
        //pushData("Valve_Discharge_CFS:",s_flow, timestamp);
        // i think this is better handled from the server side
    }
    
    return 0; 
}


void Valve_Update(char * message){
    strcpy(valve_inbox, message);
}


test_t valve_test(){
    App_Valve(); // need to get the valve type in order to measure position
    printNotif(NOTIF_TYPE_EVENT, "Current Valve Type: %s", valve_type);
                
    uint8 valve_id = 1u; // this test function isn't being used right now anyhow
    // make valve_id an argument if you'd like to be able to test both valves
    
    // use more than just read_pos so we can see the actual voltages and print those
    
    // activate 12V battery
    Level_Sensor_Power_Write(ON);

         
    test_t test; // test_t is a new data type we defined in test.h. We then use that data type to define a structure variable test
    test.status = 0; // set test status to zero
    snprintf(test.test_name,sizeof(test.test_name),"TEST_VALVE");

    float32 positions[8];
    
    voltage_t voltages[8];
    
    // all the way open
    move_valve(1,1);
    // take a position reading to verify that it's all the way open (later)
    positions[0] = read_Valve_pos(valve_id);
    voltages[0] = voltage_take_readings();
    // do the same for closed
    test.status = move_valve(0,1);
    // take a position reading to verify that it's all the way closed (later)
    positions[1] = read_Valve_pos(valve_id);
    voltages[1] = voltage_take_readings();
    if (test.status){ // don't do this if it's jammed (didn't close successfully)
        move_valve(0.125,1);
        positions[2] = read_Valve_pos(valve_id);
        
        move_valve(0.25,1);
        positions[3] = read_Valve_pos(valve_id);
        
        move_valve(0.375,1);
        positions[4] = read_Valve_pos(valve_id);

        move_valve(0.5,1);
        positions[5] = read_Valve_pos(valve_id);

        move_valve(0.675,1);
        positions[6] = read_Valve_pos(valve_id);

        move_valve(0.75,1);
        positions[7] = read_Valve_pos(valve_id);
    }

    // in test reason report the sequence of positions
    snprintf(test.reason,sizeof(test.reason),"positions:%f:%f:%f:%f:%f:%f:%f:%f", 
        positions[0],positions[1],positions[2],positions[3],
        positions[4],positions[5],positions[6],positions[7]
    );
   
    
    if (!test.status){
        snprintf(test.reason,sizeof(test.reason),"valve jammed or battery dead" );
    }
    // deactivate 12V battery
    Level_Sensor_Power_Write(OFF);
    

    return test;

}

// TODO: going to have to edit the multiplexer to get this working
float32 read_Valve_pos(uint8 valve_id){
    
    // similar flow to voltage_take_readings() in voltages.c
    
    valve_position_t readings;
    
    if (valve_id < 2){
        Valve_POS_Power_Write(1);
    }
    else{
        Downstream_Level_Sensor_Power_Write(1u);
    }
    
    
    CyDelay(10u);	    // 10 seconds delay to give time to flip on ADC pin.
    
    AMux_Start();       // Start the Analog Multiplexer
 
    ADC_RestoreConfig();// Have to call this and save (See below). Otherwise ADC won't work through sleep mode
    
    ADC_Start();        // Start the ADC
    
    float channels[AMux_CHANNELS];
    // valve stuff starts at the fourth channel
    for(uint8 c = 4; c< AMux_CHANNELS + 1; c++) // Sweep the MUX Channels
    {
        
        int32 readings[N_SAMPLES];  // Creates new int32 array called readings of size N_SAMPLES = 11
        
        AMux_Select(c); // This functions first disconnects all channels then connects the given channel.
        
        for(uint16 i=0; i< N_SAMPLES; i++){
            
            readings[i] = ADC_Read32(); // When called, it will start ADC conversions, wait for the conversion to be complete, stop ADC conversion and return the result.
        }
        
        // Converts the ADC output to Volts as a floating point number. 
        // Get the median of readings and return that.
        channels[c] = ADC_CountsTo_Volts(find_median32(readings,N_SAMPLES));    // Get median of readings and return that  
    }
    
    AMux_Stop();        // Disconnects all Analog Multiplex channels.
    
    ADC_SaveConfig();   // Save the register configuration which are not retention.
    
    ADC_Stop();         // Stops and powers down the ADC component and the internal clock if the external clock is not selected.
    
    // if valve type put voltage across valve potentiometer
    if (valve_id < 2){
        Valve_POS_Power_Write(0);
    }
    else {
        Downstream_Level_Sensor_Power_Write(0u);
    }
    
 
    readings.valve_pos_reading = channels[ADC_MUX_Valve_POS_reading]; // blue wire reading (opened percentage)
    readings.valve_pos_power = channels[ADC_MUX_Valve_POS_Power]; // brown wire power supply to potentiometer
    // same pin as Valve_POS_Power
    float32 reading_offset = -0.1;
    char * compare_location;
    compare_location = strstr(valve_type, "Butterfly");
    if (compare_location != NULL){
        reading_offset = 0.94;
        return (readings.valve_pos_reading/readings.valve_pos_power)/reading_offset; // divide because of the non-potentiometer resistance
        // this code is written for the blue rotating dynaquip valve, not the linear actuator
        
    }
    compare_location = strstr(valve_type,"LinAc");
    if (compare_location != NULL){
        float32 raw_pos = readings.valve_pos_reading / readings.valve_pos_power;
        
        // fully retracted (open) is about 0.739
        // fully extended (closed) is about 0.0384
        float32 processed_reading = raw_pos/0.7 - 0.045;
        
        return processed_reading;
    }
    
    
    return -1;
}

uint8 move_valve(float32 position_desired, uint8 valve_id){
    
    // activate 12V battery
    //Level_Sensor_Power_SetDriveMode(PIN_DM_RES_UP);
    Level_Sensor_Power_Write(1u);
    //Pressure_Voltage_Enable_Write(ON);
    
    
    float32 prev_pos =  1.5;
    
    // are we already there? (wihtin a tolerance)
    if ( fabs(read_Valve_pos(valve_id) - position_desired) < 0.05){
        printNotif(NOTIF_TYPE_ERROR,"Already at command ::: position requested: %f : current position: %f", position_desired,read_Valve_pos(valve_id));
        Level_Sensor_Power_Write(0u);
        return 1;
    }
    

    uint8 counter = 0;
    printNotif(NOTIF_TYPE_ERROR, "current_position: %f",  read_Valve_pos(valve_id));
               
    // is the desired position more closed?
    if( read_Valve_pos(valve_id) > position_desired){
        
        

        
        // while loop
        // continuously measure the position (measurement should be much faster than movement)
        // once we're within 5 percent of desired (can tighten this later) exit this do-while loop
        while(fabs(read_Valve_pos(valve_id) - position_desired) > 0.03){
            prev_pos = read_Valve_pos(valve_id);
            // turn the closing pin high (for linAc, "closing" = "extending", even if we're controlling a weir)
            if (valve_id < 2){
                Power_VDD2_Write(1u);
            }
            else{ // other valve
                Power_VBAT2_Write(1u);
            }
            if (strstr(valve_type,"LinAc") != NULL){
                CyDelay(1000u);
            }
            if (strstr(valve_type,"Butterfly") != NULL){
                CyDelay(1000u);
            }
            
            
            if (valve_id < 2){
                Power_VDD2_Write(0u);
            }
            else{ // other valve
                Power_VBAT2_Write(0u);
            }
            CyDelay(1000u);
            // without setting off false "stuck" errors
            // a shorter delay will mean a more accurate setting
            // are we moving? - if not at least one percent moved then no            
            printNotif(NOTIF_TYPE_ERROR,"prev_pos: %f, current: %f", prev_pos,read_Valve_pos(valve_id));
            // not moving or moving in the wrong direction (floating pins)
            
            // did we move in the right direction? if not, increment a counter
            if (fabs(read_Valve_pos(valve_id)) > fabs(prev_pos)){
                printNotif(NOTIF_TYPE_ERROR,"moving wrong direction");
                counter +=1;
            }
            if (counter > 5){
                // turn the closing pin low
                if (valve_id < 2){
                    Power_VDD2_Write(0u);
                }
                else{ // other valve
                    Power_VBAT2_Write(0u);
                }
                // deactivate 12V battery
                //Pressure_Voltage_Enable_Write(OFF);
                Level_Sensor_Power_Write(0u);
                printNotif(NOTIF_TYPE_ERROR, "move_valve failed ::: requested_position: %f", position_desired);
                return 0;   
            }
            
            // did we go by it? if so, stop
            if (fabs(read_Valve_pos(valve_id)) < position_desired) {
                printNotif(NOTIF_TYPE_ERROR,"went past command. stopping");
                // turn the closing pin low
                if (valve_id < 2){
                    Power_VDD2_Write(0u);
                }
                else{ // other valve
                    Power_VBAT2_Write(0u);
                }
                // deactivate 12V battery
                //Level_Sensor_Power_Write(OFF);
                Level_Sensor_Power_Write(0u);
                printNotif(NOTIF_TYPE_ERROR,"move_valve successful ::: position requested: %f : position reached: %f", position_desired,read_Valve_pos(valve_id));
    
                return 1;
            }
                
            
            
            //if (fabs(prev_pos - read_Valve_pos()) < 0.02 || read_Valve_pos() > prev_pos){
                // turn the closing pin low
                //Power_VDD2_Write(0u);
                // deactivate 12V battery
                //Level_Sensor_Power_Write(OFF);
                //printNotif(NOTIF_TYPE_ERROR, "move_valve failed ::: requested_position: %f", position_desired);
                //return 0;
            //}
        }
        
        
        // turn the closing pin low
        if (valve_id < 2){
            Power_VDD2_Write(0u);
        }
        else{ // other valve
            Power_VBAT2_Write(0u);
        }
        
        
        // read valve position once more and confirm we're where we want to be
        // if not return 0
        if(fabs(read_Valve_pos(valve_id) - position_desired) > 0.05){
            // deactivate 12V battery
            //Level_Sensor_Power_Write(OFF);
            Level_Sensor_Power_Write(0u);
            printNotif(NOTIF_TYPE_ERROR, "move_valve failed ::: requested_position: %f", position_desired);
            return 0;
        }
        
    }
    // or more open?
    else if( read_Valve_pos(valve_id) < position_desired ){
        
        // while loop
        
        // once we're within 5 percent of desired (can tighten this later) exit this do-while loop
        while(fabs(read_Valve_pos(valve_id) - position_desired) > 0.03){
            prev_pos = read_Valve_pos(valve_id);
            // turn the opening pin high
            if (valve_id < 2){
                Power_VDD1_Write(1u);
                }
            else{ // other valve
                Power_VBAT1_Write(1u);
            }
            if (strstr(valve_type,"LinAc") != NULL){
                CyDelay(1000u);
            }
            if (strstr(valve_type,"Butterfly") != NULL){
                CyDelay(1000u);
            }
            if (valve_id < 2){
                Power_VDD1_Write(0u);
            }
            else{ // other valve (weir)
                Power_VBAT1_Write(0u);
            }
            CyDelay(1000u);
            printNotif(NOTIF_TYPE_ERROR,"prev_pos: %f, current: %f", prev_pos,read_Valve_pos(valve_id));
            
            // did we move in the right direction? if not, increment a counter
            if (fabs(read_Valve_pos(valve_id)) < fabs(prev_pos)){
                printNotif(NOTIF_TYPE_ERROR,"moving wrong direction");
                counter +=1;
            }
            if (counter > 5){
                // turn the opening pin low
                if (valve_id < 2){
                    Power_VDD1_Write(0u);
                }
                else{ // other valve
                    Power_VBAT1_Write(0u);
                }
                // deactivate 12V battery
                //Power_VDD1_Write(OFF);
                Level_Sensor_Power_Write(0u);
                printNotif(NOTIF_TYPE_ERROR, "move_valve failed ::: requested_position: %f", position_desired);
                return 0;   
            }
            
            // did we go by it? if so, stop
            if (fabs(read_Valve_pos(valve_id)) > position_desired) {
                printNotif(NOTIF_TYPE_ERROR,"went past command. stopping");
                // turn the opening pin low
                if (valve_id < 2){
                    Power_VDD1_Write(0u);
                }
                else{ // other valve
                    Power_VBAT1_Write(0u);
                }
                // deactivate 12V battery
                //Level_Sensor_Power_Write(OFF);
                Level_Sensor_Power_Write(0u);
                printNotif(NOTIF_TYPE_ERROR,"move_valve successful ::: position requested: %f : position reached: %f", position_desired,read_Valve_pos(valve_id));
    
                return 1;
            }
                
            
            
            // are we moving? (staying still or wrong direction)
            //if (fabs(prev_pos - read_Valve_pos()) < 0.02 || read_Valve_pos() < prev_pos){
                // turn the opening pin low
                //Power_VDD1_Write(0u);
                // deactivate 12V battery
                //Level_Sensor_Power_Write(OFF);
                //printNotif(NOTIF_TYPE_ERROR, "move_valve failed ::: requested_position: %f", position_desired);
                //return 0;
            //}
        }
        
        // turn the opening pin low
        if (valve_id < 2){
            Power_VDD1_Write(0u);
        }
        else{ // other valve
            Power_VBAT1_Write(0u);
        }

        // read valve position once more and confirm we're where we want to be
        // if not return 0
        if(fabs(read_Valve_pos(valve_id) - position_desired) > 0.05){
            // deactivate 12V battery
            //Level_Sensor_Power_Write(OFF);
            Level_Sensor_Power_Write(0u);
            printNotif(NOTIF_TYPE_ERROR, "move_valve failed ::: requested_position: %f", position_desired);
            return 0;
        }
        
        
    }

    
    // deactivate 12V battery
    //Level_Sensor_Power_Write(OFF);
    Level_Sensor_Power_Write(0u);
    printNotif(NOTIF_TYPE_ERROR,"move_valve successful ::: position requested: %f : position reached: %f", position_desired,read_Valve_pos(valve_id));
    return 1; // everything worked fine
}
/*
void valve_level_controller(int16 level_reading){

    // these controls should be site specific. i.e. if site_id = ARB016 cutoff = 1400 mm
    printNotif(NOTIF_TYPE_EVENT, "level_controller using level_reading:%d", level_reading);
    if(level_reading > 1500){ // water level is at least 1.5 meters below the cone
        move_valve(0,1);
    }
    else {
        move_valve(1,1);
    }
}
*/

/*
// this should only be called if both level_sensor and
// downstream level sensor are enabled
float32 calculate_discharge(float32 current_position){
    level_sensor_t downstream_level = downstream_level_sensor_take_reading();
    level_sensor_t upstream_level = level_sensor_take_reading();
    
    // in millimeters
    float32 differential_head = (downstream_level.level_reading - upstream_level.level_reading);
    // the cones measure distance to water, so there's a sign reversal
    
    // the area of the orifice of the valve should probably come down from malcom
    // as should the discharge coefficient C_d 
    // because these are both properties of the site / valve we're deployed at
    // for now just make up some values
    float32 area = 3.1415*pow(valve_diameter,2);
    area = area*645.16; // convert square inches to square mm
    
    float32 C_d = discharge_coefficient(current_position); // C_d should be a function of open percentage
    
    
    float32 gravity = 9810; // mm/s^2
    float32 discharge = C_d*area*sqrt(2*gravity*differential_head);
    return discharge;
    
    
}

// based on: https://www.mydatabook.org/fluid-mechanics/flow-coefficient-opening-and-closure-curves-of-butterfly-valves/
// first column is nominal diameter
// second column is cfs / mm of water
float32 butterfly_max_Cv[17][2] = {
    {6,0.003007516137},
    {8,0.005461016144},
    {10,0.008864258089},
    {12,0.01321724197},
    {14,0.01851996779},
    {16,0.02485158071},
    {18,0.03229122589},
    {20,0.04083890334},
    {24,0.06133750017},
    {26,0.07344670988},
    {28,0.08690138733},
    {30,0.1016223874},
    {32,0.1177680003},
    {36,0.1544913553},
    {40,0.1973088876},
    {42,0.2211315812},
    {48,0.3032051137}
};

float32 discharge_coefficient(float32 current_position){

    char * compare_location; // for finding stuff
    
    // update valve diameter
    char s_diam[10];
    float32 diam;
    compare_location = strstr(valve_inbox, "Valve_Diameter_Inches");
    if (compare_location != NULL){
        extract_string(valve_inbox,"Valve_Diameter_Inches:",",",s_diam); // grab level app frequency
        sscanf(s_diam, "%f", &diam);
        valve_diameter = diam;
    }
    
    
    // valve type
    compare_location = strstr(valve_inbox, "Butterfly");
    if (compare_location != NULL){
        // get the max C_v
        uint8 valve_diam_index = 0;
        while (butterfly_max_Cv[valve_diam_index][0] + 0.5 < valve_diameter){
            valve_diam_index+=1;
        }
        float32 max_Cv = butterfly_max_Cv[valve_diam_index][1];
        // portion of that
        float32 Cv = max_Cv*butterfly_Cv_curve(current_position);
        return Cv;
    }
        
    
    
    // just as an example of another kind of valve
    compare_location = strstr(valve_inbox, "Sluice_Gate");
    
    return current_position;
}

float32 butterfly_Cv_curve(float32 current_position){
    float32 portion_of_Cv_max = 0.747*current_position - 3.12*pow(current_position,2) + 7.61*pow(current_position,3) - 4.22*pow(current_position,4);
    if (portion_of_Cv_max > 1.0){
        portion_of_Cv_max = 1.0;
    }
    else if (portion_of_Cv_max < 0.0){
        portion_of_Cv_max = 0.0;
    }
    return portion_of_Cv_max;

}
*/