#include "sontek.h"
#include "debug.h" //for strextract

/**
 * @brief Take a measurement specifically for sontek sensor, given its address. Special case of SDI_take_measurement() in SDI12.c
 * 
 * @param sensor SDI12_sensor that we want to request measurements from
 *
 * @ return 1u if measurements were successfully taken, 0u if there was a communication error
 */
uint8 SDI12_sontek_take_measurement(SDI12_sensor* sensor) {
    
    // The buffer and index are dependent on eachother. This buffer is implemented as a circular buffer to avoid overflow.
    static char SDI12_uart_buf[257] = {'\0'};
    static uint8_t SDI12_buf_idx = 0u;
    
    // unsigned ints for taking measurements
    uint8         i = 0u; // for iterating through sensor measurements
    uint8         j = 0u; //for iterating through M commands
    uint8         k = 0u; //for iterating through D commands
    uint32 ms_delay = 0u; // for delaying the code while measurements are being taken

    // string for constructing the command sent to the SDI12 sensor
    char command[100] = {'\0'};

    // string and pointer to store and parse SDI12 measurement results
    char value_str[100] = {'\0'}, delay_str[10] = {'\0'}, *ptr_end, *sign; 
    
    //First the M command needs to be sent and read, then M1 and M2 requests extra data that was already measured
    for(j = 0; j<3; j++){
        /* 1. Request measurement from addressed SDI12 sensor */
        clear_str(command);
        //determines which M commands to send
        if(j==0){                                                                  
            sprintf(command,"%s%s%s",(*sensor).address,TAKE_MEASUREMENT,"!");
        }
        else if(j==1){
            sprintf(command,"%s%s%s",(*sensor).address,TAKE_MEASUREMENT_1,"!");
        }
        else if(j==2){
            sprintf(command,"%s%s%s",(*sensor).address,TAKE_MEASUREMENT_2,"!");
        }
        
        SDI12_uart_clear_string();
        SDI12_send_command(command);
        
        /* 2. Delay for measurement to be taken */
        /*
         * The address sensor responds within 15.0 ms, returning the
         * maximum time until the measurement data will be ready and the
         * number of data values it will return
         */
        clear_str(value_str); 
        clear_str(delay_str);
        
        // supposedly response time is 15 ms, but delaying max 1000 ms to be safe
        // TODO: When taking concurrent measurements, look for <address><CR><LF> -- maybe add this feature
        for (i = 0; i < 200; i++) { 
            CyDelay(5u);
            if ( strextract(SDI12_uart_buf, value_str, "!","\r\n") ) {
                break;
            }
        }
        
        /* Delay while measurements are being prepared */
        /*
         * The returned format for <address>M! is atttn<CR><LF>
         * (NOTE: The returned format for <address>C! is atttnn<CR><LF> -- this will be useful when requesting 
         *        concurrent measurements in the future)
         * Where,
         * -    a: sensor address
         * -  ttt: maximum time until measurement is ready
         * - n(n): number of data values that will be returned
         */
        strncpy(delay_str, value_str+strlen((*sensor).address), 3); // shift ptr by strlen(...) to skip address byte
        ms_delay = ( (uint32) strtod(delay_str,(char**) NULL) ) * 1000u;
        CyDelay(ms_delay);
        
        /* 3. Request data from SDI12 sensor */
        clear_str(command);    
        clear_str(value_str);
        //Need to cycle through D0, D1, D2 to retrieve all 9 parameters of specified M command
        for(k = 0; k<3; k++){                 
            if(k==0){
                sprintf(command,"%s%s%s",(*sensor).address,READ_MEASUREMENT,"!");
            }
            else if(k==1){
                sprintf(command,"%s%s%s",(*sensor).address,READ_MEASUREMENT_1,"!");
            }
            else if(k==2){
                sprintf(command,"%s%s%s",(*sensor).address,READ_MEASUREMENT_2,"!");
            }
            
            SDI12_uart_clear_string();
            SDI12_send_command(command);
            
            // Delay a maximum of 1000 ms as readings are transmitted
            for (i = 0; i < 200; i++) {
                CyDelay(5u);
                
                // Copy the SDI12 UART buffer into temporary buffer, value_str
                // and break once "\r\n" is returned
                if ( strextract(SDI12_uart_buf, value_str, "!","\r\n") ) {
                    break;
                }
            }
            
            
            /* 4. Parse the received data */    
            // Check for "+" or "-" signs to ensure data was sent
            // First check for "+"
            sign = strstr(value_str,"+");
            if (sign == NULL) {
                // If there is no + sign, check for "-"
                sign = strstr(value_str,"-");
                
                // The absence of "+" or "-" indicates no data was sent yet
                // Return 0 and try querying the sensor again later
                if (sign == NULL) {
                    for (i = 0; i < (*sensor).nvars; i++) {
                        (*sensor).values[i] = -9999.0f;
                    }
                    return 0u;
                }
            }
            
            // Set the pointer to the start of the returned results
            // and shift forward past the address byte
            ptr_end = value_str + strlen((*sensor).address);
            
            // Parse and store all values returned from the sensor
            uint8 max = 0u; //to set high end of values index for specified M and D commands 
            //for M command sent
            if(j==0){
                //D1
                if(k==0){i=0; max=4+i;}//add the +i for the max so that the range of i matches with the var index for sontek
                //D2
                else if(k==1){i=4;max=4+i;}
                //D3
                else if(k==2){i=8;max=1+i;}
                //puts parsed data into struct, parameters 1-9
                for (; i < max; i++) {  //i is initialized and set before the for loop, so the first entry is blank followed by a ";"                           
                    (*sensor).values[i] = (float) strtod(ptr_end, &ptr_end);
                }
            }
            //for M1 command
            else if(j==1){
                //D0
                if(k==0){i=9; max=4+i;}
                //D1
                else if(k==1){i=13; max=4+i;}
                //D2
                else if(k==2){i=17; max=1+i;}
                //parameters 10-18 (or 1-9 M1 parameters in manual)
                for (; i < max; i++) {                             
                    (*sensor).values[i] = (float) strtod(ptr_end, &ptr_end);
                }
            }
            //for M2 command
            else if(j==2){
                //D0
                if(k==0){i=18; max=4+i;}
                //D1
                else if(k==1){i=22; max=4+i;}
                //D2
                else if(k==2){i=26; max=1+i;}
                //parameters 19-27 (or 1-9 M2 parameters in manual)
                for (; i < max; i++) {                             
                    (*sensor).values[i] = (float) strtod(ptr_end, &ptr_end);
                }
            }
        }
    }
    return 1u; //returns true if successful
    
}
//Powers on, takes readings, powers off sontek sensor
sontek_t sontek_take_reading(){
    
     // sontek_t is a new data type we defined in sontek.h. We then use that data type to define a structure variable sontek_output.
    sontek_t sontek_output;                                                             //----------- maybe make m_sontek
    sontek_output.valid = SONTEK_READING_INIT; //to be determined/changed later what this means
    
    
    uint8 sontek_nvars = 27; //number of parameters the sontek sensor outputs -1
    char* sontek_address = "0"; //SDI12 address of sensor
    //parameter names
    char* sontek_labels[] = {"Q","stage","v_mean","total_vol","depth","v_index","x_area","T","stat","v_x","v_z","v_x_left","v_x_right","v_bat","pitch","roll","pct_sub","ice","range","depth_adj","total_vol_pos","total_vol_neg","end_cell","snr1","snr2","snr3","snr4"};
    //initialize values to be able to tell if/what parameters are not returned
    float sontek_values[] = {-1.0, -2.0, -3.0, -4.0, -5.0, -6.0, -7.0, -8.0, -9.0, -10.0, -11.0, -12.0, -13.0, -14.0, -15.0, -16.0, -17.0, -18.0, -19.0, -20.0, -21.0, -22.0, -23.0, -24.0, -25.0, -26.0, -27.0};
    //create SDI12_sensor struct of sontek specific values
    SDI12_sensor sontek = {
        .nvars = sontek_nvars,
        .address = sontek_address,
        .labels = sontek_labels,
        .values = sontek_values
    };
    
    //Start UART connection and power up sensor
    SDI12_start();
    SDI12_Power_Write(1u);
    
    //Will try up to 10 times (1/second) to connect to sontek sensor
    for (uint8 i = 0; i < 10; i++) {
        CyDelay(1000u);
            
        //Checks if sontek sensor is on    
        if (SDI12_is_active(&sontek)){
            //valid = SDI12_change_address(&sensors[0],"0");  -----comment out copied code
            SDI12_info(&sontek); CyDelay(500u); // ------ I don't know what this does/it's purpose 
            //Trys to take a measurment
            if (SDI12_take_measurement(&sontek)){
                sontek_output.valid = 1; // Successfully took a measurement
                sontek_output.depth = sontek.values[4];
               
                break;                    
            } 
            else {
                sontek_output.valid = 2; // SDI12 sensor powered on, but unable to parse response
            } 
        }
        
        else {
            sontek_output.valid = 3; // SDI12 sensor not responding. It may be powered off or have a different address
        }
    }
            
    //shuts off sensor and turns of UART
    SDI12_Power_Write(0u);
    SDI12_stop();
    
    // this isn't set to any of the measurements yet
    return sontek_output;
}
    
    

/* [] END OF FILE */
