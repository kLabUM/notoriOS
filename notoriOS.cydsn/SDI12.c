/**
 * @file SDI12.c
 * @brief Implements functions for SDI12 sensors.
 * @author Brandon Wong
 * @version TODO
 * @date 2017-10-28
 */

#include "SDI12.h"
#include "debug.h"
//#include "misc.h"   ----- probably don't need anymore

//*========== #BEGIN Define SDI12 sensors ==========//
//----------   SDI12 Array   ----------//
#define N_SDI12_SENSORS     3           // Total number of sensors defined
SDI12_sensor sensors[N_SDI12_SENSORS];  // GOTO zip_SDI12()

//Defined in sontek.c
////----------  (1/3) Sontek Default Mode  ----------//
//char* sontek_labels[] = {"Q","stage","v_mean","total_vol","depth","v_index","x_area","T","stat","v_x","v_z","v_x_left","v_x_right","v_bat","pitch","roll","pct_sub","ice","range","depth_adj","total_vol_pos","total_vol_neg","end_cell","snr1","snr2","snr3","snr4"};
//float sontek_values[] = {-1.0, -2.0, -3.0, -4.0, -5.0, -6.0, -7.0, -8.0, -9.0, -10.0, -11.0, -12.0, -13.0, -14.0, -15.0, -16.0, -17.0, -18.0, -19.0, -20.0, -21.0, -22.0, -23.0, -24.0, -25.0, -26.0, -27.0};

////All SDI12 sensors also initialized here for zip command

//----------  (1/3) Sontek Default Mode  ----------//
char* sontek_labels[] = {"temp","pressure","stage","end_cell",\
    "v_beam1","v_beam2","v_b1_b2_mag","avg_SNR_b1_b2","Q",\
    "v_b1_c1","v_b2_c1","v_b1_c2","v_b2_c2","v_b1_c3","v_b2_c3",\
    "avg_amp1","avg_amp2","avg_amp3","avg_SNR1","avg_SNR2","avg_SNR3"\
    "v_b1_c4","v_b2_c4","v_b1_c5","v_b2_c5","v_b1_c6","v_b2_c6",\
    "avg_amp4","avg_amp5","avg_amp6","avg_SNR4","avg_SNR5","avg_SNR6"\
    "v_b1_c7","v_b2_c7","v_b1_c8","v_b2_c8","v_b1_c9","v_b2_c9",\
    "avg_amp7","avg_amp8","avg_amp9","avg_SNR7","avg_SNR8","avg_SNR9",\
    "v_b1_c10","v_b2_c10","avg_amp10","avg_SNR10"\
    "v_bat","MB_free","Q","total_vol","ice"};
float sontek_values[] = {-1.0, -2.0, -3.0, -4.0,\
        -5.0, -6.0, -7.0, -8.0, -9.0,\
        -10.0, -11.0, -12.0, -13.0, -14.0, -15.0,\
        -16.0, -17.0, -18.0, -19.0, -20.0, -21.0,\
        -22.0, -23.0, -24.0, -25.0, -26.0, -27.0,\
        -28.0, -29.0, -30.0, -31.0, -32.0, -33.0,\
        -34.0, -35.0, -36.0, -37.0, -38.0, -39.0,\
        -40.0, -41.0, -42.0, -43.0, -44.0, -45.0,\
        -46.0, -47.0, -48.0, -49.0,\
        -50.0, 51.0, -52.0, -53.0, -54.0};
SDI12_sensor sontek_SDI12 = {
    .nvars = SONTEK_NVARS,
    .address = SONTEK_ADDRESS,
    .labels = sontek_labels,
    .values = sontek_values
};

//----------  (2/3) Solinst  ----------//
char* labels[] = {"solinst_T","solinst_P"};
float values[] = {-99.0, -98.0};
SDI12_sensor solinst_SDI12 = { 
    .nvars   = SONLIST_NVARS,
    .address = SONLIST_ADDRESS,
    .labels  = labels,
    .values  = values
    // additional metadata should initialize to 0 or '\0' //
}; 

//----------  (3/3) Decagon  ----------//
char* GS3_labels[] = {"GS3_1","GS3_2","GS3_3"};
float GS3_values[] = {-97.0, -95.0, -94.9};
SDI12_sensor GS3_SDI12 = {
    .nvars   = DECAGON_NVARS,
    .address = DECAGON_ADDRESS,
    .labels  = GS3_labels,
    .values  = GS3_values        
    // additional metadata should initialize to 0 or '\0' //
};  
// ========== #END   Define SDI12 sensors ==========//*/

// The buffer and index are dependent on eachother. This buffer is implemented
// as a circular buffer to avoid overflow.
static char SDI12_uart_buf[257] = {'\0'};
static uint8_t SDI12_buf_idx = 0u;

// unsigned ints for taking measurements
uint8         i = 0u; // for iterating through sensor measurements
uint32 ms_delay = 0u; // for delaying the code while measurements are being taken

// string for constructing the command sent to the SDI12 sensor
char command[100] = {'\0'};

// string for take_measurement command sent to the SDI12 sensor
char take_meas[2] = {'\0'};
// string for read_measurement command sent to the SDI12 sensor
char read_meas[2] = {'\0'};

// string and pointer to store and parse SDI12 measurement results
char value_str[100] = {'\0'}, delay_str[10] = {'\0'}, *ptr_end, *sign; 


void SDI12_start(){
    SDI12_UART_Start();
    isr_SDI12_StartEx(isr_SDI12_data); 
}

void SDI12_stop(){
    isr_SDI12_Stop(); 
    SDI12_UART_Stop();
}

uint8_t clear_str(char *str) {
    memset(str, '\0', strlen(str));
    return 1u;
}


                
                
/**
 * @brief 
 */
void SDI12_uart_clear_string() {
    SDI12_UART_ClearRxBuffer();
    memset(SDI12_uart_buf, '\0', sizeof(SDI12_uart_buf));
    SDI12_buf_idx = 0u;
}

char* sensors_uart_get_string() {
    return SDI12_uart_buf; 
}


/**
 * @brief Send an SDI12-formatted command. A command always starts with
 *        the address of the sensor, followed by the command, and is terminated by 
 *        and exclamation point: <address><command>!
 * 
 * @param command String of the command to send
 *
 */
void SDI12_send_command(char command[]) {
    // Wake up the sensors
    // Pull the Data pin high for 12 milliseconds
    SDI12_control_reg_Write(1u);
    CyDelayUs(12000u);  // 12 milliseconds = 12 x 10^3 microseconds

    // Pull the Data pin low for 8.4 milliseconds
    SDI12_control_reg_Write(0u);
    CyDelayUs(8400u);   // 8.4 milliseconds = 8.4 x 10^3 microseconds
        
    // Write the command to the Data pin
    SDI12_UART_PutString(command);
}

/**
 * @brief Check if an SDI12 sensor is on, given its address
 * 
 * @param sensor SDI12_sensor that we want to check the status of
 *
 * @return 1u if active, 0u if inactive
 */
uint8 SDI12_is_active(SDI12_sensor* sensor) {
    /* 1. Request if SDI12 sensor is active */
    clear_str(command);
    
    /* 2. Construct the command <address><command><!> */
    sprintf(command,"%s%s%s",(*sensor).address,ACK_ACTIVE,"!");
    SDI12_uart_clear_string();
    
    /* 3. Send the command to the sensor */
    SDI12_send_command(command);
    
    /* 4. Parse the received data, if any */
    /* TODO: This can be shortened using a for-loop like in SDI12_take_measurement() */
    CyDelay(1000u); // Delay for readings to be transmitted
    
    // Copy the SDI12 UART buffer into temporary buffer
    // *TODO: Parse sensor address from the response
    clear_str(value_str);
    if (strextract(SDI12_uart_buf, value_str, "!" ,"\r\n")) {
        // If "\r\n" is in the response, the sensor is active
        return 1u;
    }
    
    return 0u;
}

/** 
 * @brief Change the address of an SDI12 sensor
 *
 * @param sensor SDI12_sensor that we want to change the address of
 * @param new_address String (single char, either 0-9, a-z, A-Z) of the desired address
 *
 * @return 1u if successful, 0u if unsuccessful
 */
uint8 SDI12_change_address(SDI12_sensor* sensor, char new_address[]) {
 
    /* 1. Request measurement from addressed SDI12 sensor */
    clear_str(command);
    sprintf(command,"%s%s%s%s",(*sensor).address,CHANGE_ADDR,new_address,"!");
    
    SDI12_uart_clear_string();
    SDI12_send_command(command);    
        
    // Delay max 1000 ms
    // Look for <new address><CR><LF> in the buffer
    clear_str(value_str);
    sprintf(value_str, "%s\r\n", new_address);
    for (i = 0; i < 200; i++) { 
        CyDelay(5u);
        if ( strstr(SDI12_uart_buf, value_str) ) {
            (*sensor).address = new_address;
            return 1u;
        }
    }
    
    return 0u;
}


/**
 * @brief Take a measurement for an SDI12 sensor, given its address
 * 
 * @param sensor SDI12_sensor that we want to request measurements from
 * @param take_meas int8 is the take measurement command
 * @param read_meas int8 is the read measurement command
 *
 * @ return 1u if measurements were successfully taken, 0u if there was a communication error
 */
uint8 SDI12_take_measurement(SDI12_sensor* sensor, int8 take_meas_number, int8 read_meas_number) {
    
    /* 0. Turn char take_meas and read_meas into strings */
    
    //The default M command does not have a number after (unlike the default D command is D0)
    clear_str(take_meas);
    if(take_meas_number == 0){sprintf(take_meas,"%s","M");}
    else{sprintf(take_meas,"%s%i","M",take_meas_number);}

    clear_str(read_meas);
    sprintf(read_meas,"%s%i","D",read_meas_number);
    
    /* 1. Request measurement from addressed SDI12 sensor */
    clear_str(command);
    sprintf(command,"%s%s%s",(*sensor).address,take_meas,"!");
    
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
    // TODO: When taking concurrent measurements, look for <address><CR><LF>
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
    sprintf(command,"%s%s%s",(*sensor).address,read_meas,"!");   
    
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
    for (i = 0; i < (*sensor).nvars; i++) {
        (*sensor).values[i] = (float) strtod(ptr_end, &ptr_end);
    }
    
    return 1u;
    
}

/**
 * @brief TODO: Take a concurrent measurement for an SDI12 sensor, given its address
 * 
 * @param sensor SDI12_sensor that we want to request measurements from
 *
 * @ return if sensor is active
 */
uint8 SDI12_take_concurrent_measurement(SDI12_sensor* sensor) {
    /* TODO */
    /*
     * Concurrent measurements enable measurements to be triggered for
     * multiple sensors, rather than doing one at a time.
     * 
     * Requires SDI12 V1.2 or greater.
     *
     * Currently returning a dummy response for now.
     */
    return SDI12_is_active(sensor);
}

/**
 * @brief Obtain detailed info about an SDI12 sensor, given its address
 * 
 * @param sensor SDI12_sensor that we want to request info from
 *
 * @ return 1u if successful, 0u if unsuccessful
 */
uint8 SDI12_info(SDI12_sensor* sensor) {

    /* 1. Request measurement from addressed SDI12 sensor */
    clear_str(command);
    sprintf(command,"%s%s%s",(*sensor).address,INFO,"!");
    
    SDI12_uart_clear_string();
    SDI12_send_command(command);    

    clear_str(value_str);
    
    // Look for <CR><LF> in the buffer since <CR><LR> terminates all responses, if there is one 
    // Delay max 1000 ms while doing so
    for (i = 0; i < 200; i++) { 
        CyDelay(5u);
        if (strextract(SDI12_uart_buf, value_str, "!" ,"\r\n")) {
        // If "\r\n" is in the response, then the sensor is active
            
            ptr_end = value_str;
            // Attempt to parse the information and store in "sensor"
            // Move the pointer past the address 
            ptr_end += strlen((*sensor).address);
            
            // SDI-12 specification (2 chars)
            strncpy((*sensor).v_SDI12, ptr_end, 2);
            ptr_end += 2;
            
            // Vendor identification (8 chars)
            strncpy((*sensor).vendor, ptr_end, 8);
            ptr_end += 8;
            
            // Sensor Model (6 chars)
            strncpy((*sensor).model, ptr_end, 6);
            ptr_end += 6;
            
            // Sensor Version (3 chars)
            strncpy((*sensor).version, ptr_end, 3);
            ptr_end += 3;
            
            // Sensor Serial number / misc info (up to 13 chars)
            strncpy((*sensor).serial, ptr_end, 13);
            
            // Assume successful if we reached this point
            return 1u;
        }
    }
    
    return 0u;
}

uint8 zip_SDI12(int SDI12_flag, int8 take_meas_number, int8 read_meas_number) {

    char output[100] = {'\0'}; // For debugging
    uint8 i, j, k, valid;
    
    sensors[0] = sontek_SDI12;
    sensors[1] = solinst_SDI12;
    sensors[1] = GS3_SDI12;
    
    SDI12_start();
    SDI12_Power_Write(1u);
      
    // Iterate through each SDI12 sensor
    // TODO: Reconstruct to go thorugh each bit of SDI12_flag if
    //       SDI12_flag is used to flag multiple SDI-12 sensors
    //       (This assumes the number of bits < N_SDI12_SENSORS)
    for (k = 0; k < N_SDI12_SENSORS;  k++) {
        if (0x00000001 & SDI12_flag>>k)  {
            /*/ Ensure we don't access nonexistent array index
            if(*array_ix + (sensors[k]).nvars >= max_size){
                return *array_ix;
            }    
            //*/
            
            //* ---------- Take measurement(s) from k'th SDI-12 sesnor ---------- //
            valid = 0; // Reinitialize sensor status
            
            // Let the sensor power up max 10000 ms = 10 * 1000 ms
            for (i = 0; i < 50; i++) {
                CyDelay(200u);
                
                valid = SDI12_is_active(&sensors[k]); 
                if (valid == 1u){  
                    
                    LED_Write(0u);
                    
                    
                    //valid = SDI12_change_address(&sensors[0],"0");
                    CyDelay(200u); 
                    valid = SDI12_info(&sensors[k]); 
                    CyDelay(200u);

                    if (SDI12_take_measurement(&sensors[k],take_meas_number, read_meas_number)) {
                        valid = 1; // Successfully took a measurement
                        break;                    
                    } else {
                        // SDI12 sensor powered on, but unable to parse response
                        valid = -2;                    
                        for (j = 0; j < (sensors[k]).nvars; j++) {
                            (sensors[k]).values[j] = (float) valid;
                        }
                       
                    }            
                                            
                } else {
                    // SDI12 sensor not responding. It may be powered off or have a different address
                    valid = -1;
                    for (j = 0; j < (sensors[k]).nvars; j++) {
                        (sensors[k]).values[j] = (float) valid;
                    }
                    LED_Write(1u);
                    
                }
            }
            
            LED_Write(0u);
            
            //* ---------- Insert measurement(s) from k'th SDI-12 sesnor ---------- //
            // char[] output is used for debugging purposes
            clear_str(output); 
            
            // Insert current values in labels[] and readings[]
            for (j = 0; j < (sensors[k]).nvars; j++) {
                // char[] output is used for debugging purposes
                sprintf(output,"%s %s %f", output, (sensors[k]).labels[j], (sensors[k]).values[j]);
                
                /*/
                labels[*array_ix] = (sensors[k]).labels[j];
                readings[*array_ix] = (sensors[k]).values[j];
                (*array_ix) += 1;
                //*/
            }              
            
        }
          
    }
                
    SDI12_Power_Write(0u);
    SDI12_stop(); 
    
    return 0;//*array_ix;
}

CY_ISR(isr_SDI12_data) {
    // hold the next char in the rx register as a temporary variable
    char rx_char = SDI12_UART_GetChar();

    // store the char in SDI12_uart_buf
    if (rx_char) {
        // unsigned ints don't `overflow`, they reset at 0 if they are
        // incremented one more than it's max value. It will be obvious if an
        // `overflow` occurs
        SDI12_uart_buf[SDI12_buf_idx++] = rx_char;
    }
}
//////* ----- old SDI12.c before doing reverting that does not profile view of sontek
////uint8 SDI12_sontek_average_take_measurement(SDI12_sensor* sensor) {
////        
////    for(uint8 j = 0; j<3; j++){
////        /* 1. Request measurement from addressed SDI12 sensor */
////        clear_str(command);
////        if(j==0){                                                                   
////            sprintf(command,"%s%s%s",(*sensor).address,TAKE_MEASUREMENT,"!");
////        }
////        else if(j==1){
////            sprintf(command,"%s%s%s",(*sensor).address,TAKE_MEASUREMENT_1,"!");
////        }
////        else if(j==2){
////            sprintf(command,"%s%s%s",(*sensor).address,TAKE_MEASUREMENT_2,"!");
////        }
////        
////        SDI12_uart_clear_string();
////        SDI12_send_command(command);
////        
////        /* 2. Delay for measurement to be taken */
////        /*
////         * The address sensor responds within 15.0 ms, returning the
////         * maximum time until the measurement data will be ready and the
////         * number of data values it will return
////         */
////        clear_str(value_str); 
////        clear_str(delay_str);
////        
////        // supposedly response time is 15 ms, but delaying max 1000 ms to be safe
////        // TODO: When taking concurrent measurements, look for <address><CR><LF>
////        for (i = 0; i < 200; i++) { 
////            CyDelay(5u);
////            if ( strextract(SDI12_uart_buf, value_str, "!","\r\n") ) {
////                break;
////            }
////        }
////        
////        /* Delay while measurements are being prepared */
////        /*
////         * The returned format for <address>M! is atttn<CR><LF>
////         * (NOTE: The returned format for <address>C! is atttnn<CR><LF> -- this will be useful when requesting 
////         *        concurrent measurements in the future)
////         * Where,
////         * -    a: sensor address
////         * -  ttt: maximum time until measurement is ready
////         * - n(n): number of data values that will be returned
////         */
////        strncpy(delay_str, value_str+strlen((*sensor).address), 3); // shift ptr by strlen(...) to skip address byte
////        ms_delay = ( (uint32) strtod(delay_str,(char**) NULL) ) * 1000u;
////        CyDelay(ms_delay);
////        
////        /* 3. Request data from SDI12 sensor */
////        //clear_str(command);    
////        //clear_str(value_str);
////        for(uint8 k = 0; k<3; k++){                 //-------------------------------added only will do D0 D1
////            clear_str(command);    
////            clear_str(value_str);
////            
////            if(k==0){
////                sprintf(command,"%s%s%s",(*sensor).address,READ_MEASUREMENT,"!");
////            }
////            else if(k==1){
////                sprintf(command,"%s%s%s",(*sensor).address,READ_MEASUREMENT_1,"!");
////            }
////            else if(k==2){
////                sprintf(command,"%s%s%s",(*sensor).address,READ_MEASUREMENT_2,"!");
////            }
////            
////            SDI12_uart_clear_string();
////            SDI12_send_command(command);
////            
////            // Delay a maximum of 1000 ms as readings are transmitted
////            for (i = 0; i < 200; i++) {
////                CyDelay(5u);
////                
////                // Copy the SDI12 UART buffer into temporary buffer, value_str
////                // and break once "\r\n" is returned
////                if ( strextract(SDI12_uart_buf, value_str, "!","\r\n") ) {
////                    break;
////                }
////            }
////            
////            
////            /* 4. Parse the received data */    
////            // Check for "+" or "-" signs to ensure data was sent
////            // First check for "+"
////            char* location = value_str;
////            sign = strstr(location,"+");
////            if (sign == NULL) {
////                // If there is no + sign, check for "-"
////                sign = strstr(location,"-");
////                
////                // The absence of "+" or "-" indicates no data was sent yet
////                // Return 0 and try querying the sensor again later
////                if (sign == NULL) {
////                    for (i = 0; i < (*sensor).nvars; i++) {
////                        (*sensor).values[i] = -9999.0f;
////                    }
////                    return 0u;
////                }
////            }
////            
////            // Set the pointer to the start of the returned results
////            // and shift forward past the address byte
////            ptr_end = value_str + strlen((*sensor).address);
////            
////            // Parse and store all values returned from the sensor
////            uint8 max = 0u;
////            if(j==0){
////                if(k==0){i=0; max=4+i;}///add the +i for the max so that the range of i matches with the var index for sontek
////                else if(k==1){i=4;max=4+i;}
////                else if(k==2){i=8;max=1+i;}
////                for (; i < max; i++) {                            
////                    (*sensor).values[i] = (float) strtod(ptr_end, &ptr_end);
////                }
////            }
////            else if(j==1){
////                if(k==0){i=9; max=4+i;}
////                else if(k==1){i=13; max=4+i;}
////                else if(k==2){i=17; max=1+i;}
////                for (; i < max; i++) {                            
////                    (*sensor).values[i] = (float) strtod(ptr_end, &ptr_end);
////                }
////            }
////            else if(j==2){
////                if(k==0){i=18; max=4+i;}
////                else if(k==1){i=22; max=4+i;}
////                else if(k==2){i=26; max=1+i;}
////                for (; i < max; i++) {                             //------------changes were made here
////                    (*sensor).values[i] = (float) strtod(ptr_end, &ptr_end);
////                }
////            }
////        }
////    }
////    return 1u;
////    
////}

/* [] END OF FILE */
