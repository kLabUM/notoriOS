#include "sontek.h"
#include "debug.h" //for strextract

//Powers on, takes readings, powers off sontek sensor
sontek_t sontek_take_reading(){
     
    //-------------------- For default mode -----------------------------------
    //    //parameter names
    //    char* sontek_labels[] = {"Q","stage","v_mean","total_vol","depth","v_index","x_area","T","stat","v_x","v_z","v_x_left","v_x_right","v_bat","pitch","roll","pct_sub","ice","range","depth_adj","total_vol_pos","total_vol_neg","end_cell","snr1","snr2","snr3","snr4"};
    //    //initialize values to be able to tell if/what parameters are not returned
    //    float sontek_values[] = {-1.0, -2.0, -3.0, -4.0, -5.0, -6.0, -7.0, -8.0, -9.0, -10.0, -11.0, -12.0, -13.0, -14.0, -15.0, -16.0, -17.0, -18.0, -19.0, -20.0, -21.0, -22.0, -23.0, -24.0, -25.0, -26.0, -27.0};
     
    //-------------------- For beam profile mode -----------------------------------
    //parameter names line split by command used to read and for readability
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
    //initialize values to be able to tell if/what parameters are not returned
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
       //create SDI12_sensor struct of sontek specific values. Not declaring labels as struct sontek_t type is used for labels    
        SDI12_sensor sontek = {
            .nvars = SONTEK_NVARS,
            .address = SONTEK_ADDRESS,  //SDI12 address of sensor   
            .values = sontek_values //use same initial array as sontek
        };

    // sontek_t is a new data type we defined in sontek.h. We then use that data type to define a structure variable m_sontek.
    sontek_t m_sontek = {
        .nvars = SONTEK_NVARS,
        .flag = SONTEK_READING_INIT, //to be determined/changed later what this means
        .labels = sontek_labels,
        .values = sontek_values,
        .data0={-1.0, -2.0, -3.0, -4.0},
        .data1={-5.0, -6.0, -7.0, -8.0, -9.0},
        .data2={-10.0, -11.0, -12.0, -13.0, -14.0, -15.0},
        .data3={-16.0, -17.0, -18.0, -19.0, -20.0, -21.0},
        .data4={-22.0, -23.0, -24.0, -25.0, -26.0, -27.0},
        .data5={-28.0, -29.0, -30.0, -31.0, -32.0, -33.0},
        .data6={-34.0, -35.0, -36.0, -37.0, -38.0, -39.0},
        .data7={-40.0, -41.0, -42.0, -43.0, -44.0, -45.0},
        .data8={-46.0, -47.0, -48.0, -49.0},
        .data9={-50.0, 51.0, -52.0, -53.0, -54.0}
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
            //Trys to take a measurment and loops through differnt measurment commands 
            
/*--------Set for beam profile ("M" with D0 and D1, "M_" 1-8, D0 for all)--------*/
            //loop through all M commands
            for (uint8 k = 0; k < 9; k++){ 
                
                uint8 j=0;
               
                if (SDI12_take_measurement(&sontek, k, 0)){
                    m_sontek.flag = SONTEK_READING_VALID; // Successfully took a measurement
                    //set sontek_t struct values equal to SDI12_sensor struct values for all M_ + D0
                    if(k==0){for(j=0; j < MD1-MD0; j++){
                        m_sontek.data0[j] = sontek.values[j];}}
                    if(k==1){for(j=0; j < M2D0-M1D0; j++){
                         m_sontek.data2[j] = sontek.values[j];}}
                    if(k==2){for(j=0; j < M3D0-M2D0; j++){
                         m_sontek.data3[j] = sontek.values[j];}}
                    if(k==3){for(j=0; j < M4D0-M3D0; j++){
                         m_sontek.data4[j] = sontek.values[j];}}
                    if(k==4){for(j=0; j < M5D0-M4D0; j++){
                         m_sontek.data5[j] = sontek.values[j];}}
                    if(k==5){for(j=0; j < M6D0-M5D0; j++){
                         m_sontek.data6[j] = sontek.values[j];}}
                    if(k==6){for(j=0; j < M7D0-M6D0; j++){
                         m_sontek.data7[j] = sontek.values[j];}}
                    if(k==7){for(j=0; j < M8D0-M7D0; j++){
                         m_sontek.data8[j] = sontek.values[j];}}
                    if(k==8){for(j=0; j < SONTEK_NVARS-M8D0; j++){
                         m_sontek.data9[j] = sontek.values[j];}}
                    
                    //maybe start at j = 0 for all but then start .values as j+ instead of the minus things
                    //now concatonate data_ arrays into m_sontek.values
                    if(k==0){for(j=MD0; j < MD1; j++){
                        m_sontek.values[j] =  m_sontek.data0[j-MD0];}}
                    if(k==1){for(j=M1D0; j < M2D0; j++){
                        m_sontek.values[j] = m_sontek.data2[j-M1D0];}}
                    if(k==2){for(j=M2D0; j < M3D0; j++){
                        m_sontek.values[j] =  m_sontek.data3[j-M2D0];}}
                    if(k==3){for(j=M3D0; j < M4D0; j++){
                        m_sontek.values[j] =  m_sontek.data4[j-M3D0];}}
                    if(k==4){for(j=M4D0; j < M5D0; j++){
                        m_sontek.values[j] =  m_sontek.data5[j-M4D0];}}
                    if(k==5){for(j=M5D0; j < M6D0; j++){
                        m_sontek.values[j] =  m_sontek.data6[j-M5D0];}}
                    if(k==6){for(j=M6D0; j < M7D0; j++){
                        m_sontek.values[j] =  m_sontek.data7[j-M6D0];}}
                    if(k==7){for(j=M7D0; j < M8D0; j++){
                        m_sontek.values[j] =  m_sontek.data8[j-M7D0];}}
                    if(k==8){for(j=M8D0; j < SONTEK_NVARS; j++){
                        m_sontek.values[j] =  m_sontek.data9[j-M8D0];}}
                    
//                    if(k==0){for(j=MD0; j < MD1; j++){
//                        m_sontek.values[j] = sontek.values[j-MD0];}}
//                    if(k==1){for(j=M1D0; j < M2D0; j++){
//                        m_sontek.values[j] = sontek.values[j-M1D0];}}
//                    if(k==2){for(j=M2D0; j < M3D0; j++){
//                        m_sontek.values[j] = sontek.values[j-M2D0];}}
//                    if(k==3){for(j=M3D0; j < M4D0; j++){
//                        m_sontek.values[j] = sontek.values[j-M3D0];}}
//                    if(k==4){for(j=M4D0; j < M5D0; j++){
//                        m_sontek.values[j] = sontek.values[j-M4D0];}}
//                    if(k==5){for(j=M5D0; j < M6D0; j++){
//                        m_sontek.values[j] = sontek.values[j-M5D0];}}
//                    if(k==6){for(j=M6D0; j < M7D0; j++){
//                        m_sontek.values[j] = sontek.values[j-M6D0];}}
//                    if(k==7){for(j=M7D0; j < M8D0; j++){
//                        m_sontek.values[j] = sontek.values[j-M7D0];}}
//                    if(k==8){for(j=M8D0; j < SONTEK_NVARS; j++){
//                        m_sontek.values[j] = sontek.values[j-M8D0];}}
                } 
                else {
                    m_sontek.flag = SONTEK_READING_PARSE_ERROR; // SDI12 sensor powered on, but unable to parse response
                }

                // For "M" (k=0) send "D1" commands additionally
                if(k==0){
                    if (SDI12_take_measurement(&sontek, k, 1)){
                        m_sontek.flag = SONTEK_READING_VALID; // Successfully took a measurement
                        //set sontek_t struct values equal to SDI12_sensor struct values for M + D1 command
                        //--------change so it just sends a data request
                        for(j=0; j < M1D0-MD1; j++){
                             m_sontek.data1[j] = sontek.values[j];}
                        
                        for(j=MD1; j < M1D0; j++){
                            m_sontek.values[j] =  m_sontek.data1[j-MD1];}
                        
//                        for(j=MD1; j < M1D0; j++){
//                            m_sontek.values[j] = sontek.values[j-MD1];}
                    } 
                    else {
                        m_sontek.flag = SONTEK_READING_PARSE_ERROR; // SDI12 sensor powered on, but unable to parse response
                    }
////                    if (SDI12_take_measurement(&sontek, k, 1)){
////                        m_sontek.flag = SONTEK_READING_VALID; // Successfully took a measurement
////                        //set sontek_t struct values equal to SDI12_sensor struct values
////                        for(uint8 j; j < SONTEK_NVARS; j++){
//////fix indexing                            
////                            m_sontek.values[j] = sontek.values[j];
////                        }                     
////                    } 
////                    else {
////                        m_sontek.flag = SONTEK_READING_PARSE_ERROR; // SDI12 sensor powered on, but unable to parse response
////                    }
                }  
            }
            
            break;
        }
        
        else {
            m_sontek.flag = SONTEK_READING_NOT_RESPONDING; // SDI12 sensor failes active test and not responding. It may be powered off or have a different address
        }
    }
    //delay to give time for sontek to store readings onto sensor ---------------------- see if good enough
    CyDelay(SONTEK_WRITE_TIME);
    //shuts off sensor and turns of UART
    SDI12_Power_Write(0u);
    SDI12_stop();
            
    return m_sontek;
}

///** specilized SDI12 take reading to take dafual mode setting for sontek sensor
// * @brief Take a measurement specifically for sontek sensor, given its address. Special case of SDI_take_measurement() in SDI12.c
// * 
// * @param sensor SDI12_sensor that we want to request measurements from
// *
// * @ return 1u if measurements were successfully taken, 0u if there was a communication error
// */
//uint8 SDI12_sontek_take_measurement(SDI12_sensor* sensor) {
//    
//    // The buffer and index are dependent on eachother. This buffer is implemented as a circular buffer to avoid overflow.
//    static char SDI12_uart_buf[257] = {'\0'};
//    static uint8_t SDI12_buf_idx = 0u;
//    
//    // unsigned ints for taking measurements
//    uint8         i = 0u; // for iterating through sensor measurements
//    uint8         j = 0u; //for iterating through M commands
//    uint8         k = 0u; //for iterating through D commands
//    uint32 ms_delay = 0u; // for delaying the code while measurements are being taken
//
//    // string for constructing the command sent to the SDI12 sensor
//    char command[100] = {'\0'};
//
//    // string and pointer to store and parse SDI12 measurement results
//    char value_str[100] = {'\0'}, delay_str[10] = {'\0'}, *ptr_end, *sign; 
//    
//    //First the M command needs to be sent and read, then M1 and M2 requests extra data that was already measured
//    for(j = 0; j<3; j++){
//        /* 1. Request measurement from addressed SDI12 sensor */
//        clear_str(command);
//        //determines which M commands to send
//        if(j==0){                                                                  
//            sprintf(command,"%s%s%s",(*sensor).address,TAKE_MEASUREMENT,"!");
//        }
//        else if(j==1){
//            sprintf(command,"%s%s%s",(*sensor).address,TAKE_MEASUREMENT_1,"!");
//        }
//        else if(j==2){
//            sprintf(command,"%s%s%s",(*sensor).address,TAKE_MEASUREMENT_2,"!");
//        }
//        
//        SDI12_uart_clear_string();
//        SDI12_send_command(command);
//        
//        /* 2. Delay for measurement to be taken */
//        /*
//         * The address sensor responds within 15.0 ms, returning the
//         * maximum time until the measurement data will be ready and the
//         * number of data values it will return
//         */
//        clear_str(value_str); 
//        clear_str(delay_str);
//        
//        // supposedly response time is 15 ms, but delaying max 1000 ms to be safe
//        // TODO: When taking concurrent measurements, look for <address><CR><LF> -- maybe add this feature
//        for (i = 0; i < 200; i++) { 
//            CyDelay(5u);
//            if ( strextract(SDI12_uart_buf, value_str, "!","\r\n") ) {
//                break;
//            }
//        }
//        
//        /* Delay while measurements are being prepared */
//        /*
//         * The returned format for <address>M! is atttn<CR><LF>
//         * (NOTE: The returned format for <address>C! is atttnn<CR><LF> -- this will be useful when requesting 
//         *        concurrent measurements in the future)
//         * Where,
//         * -    a: sensor address
//         * -  ttt: maximum time until measurement is ready
//         * - n(n): number of data values that will be returned
//         */
//        strncpy(delay_str, value_str+strlen((*sensor).address), 3); // shift ptr by strlen(...) to skip address byte
//        ms_delay = ( (uint32) strtod(delay_str,(char**) NULL) ) * 1000u;
//        CyDelay(ms_delay);
//        
//        /* 3. Request data from SDI12 sensor */
//        clear_str(command);    
//        clear_str(value_str);
//        //Need to cycle through D0, D1, D2 to retrieve all 9 parameters of specified M command
//        for(k = 0; k<3; k++){                 
//            if(k==0){
//                sprintf(command,"%s%s%s",(*sensor).address,READ_MEASUREMENT,"!");
//            }
//            else if(k==1){
//                sprintf(command,"%s%s%s",(*sensor).address,READ_MEASUREMENT_1,"!");
//            }
//            else if(k==2){
//                sprintf(command,"%s%s%s",(*sensor).address,READ_MEASUREMENT_2,"!");
//            }
//            
//            SDI12_uart_clear_string();
//            SDI12_send_command(command);
//            
//            // Delay a maximum of 1000 ms as readings are transmitted
//            for (i = 0; i < 200; i++) {
//                CyDelay(5u);
//                
//                // Copy the SDI12 UART buffer into temporary buffer, value_str
//                // and break once "\r\n" is returned
//                if ( strextract(SDI12_uart_buf, value_str, "!","\r\n") ) {
//                    break;
//                }
//            }
//            
//            
//            /* 4. Parse the received data */    
//            // Check for "+" or "-" signs to ensure data was sent
//            // First check for "+"
//            sign = strstr(value_str,"+");
//            if (sign == NULL) {
//                // If there is no + sign, check for "-"
//                sign = strstr(value_str,"-");
//                
//                // The absence of "+" or "-" indicates no data was sent yet
//                // Return 0 and try querying the sensor again later
//                if (sign == NULL) {
//                    for (i = 0; i < (*sensor).nvars; i++) {
//                        (*sensor).values[i] = -9999.0f;
//                    }
//                    return 0u;
//                }
//            }
//            
//            // Set the pointer to the start of the returned results
//            // and shift forward past the address byte
//            ptr_end = value_str + strlen((*sensor).address);
//            
//            // Parse and store all values returned from the sensor
//            uint8 max = 0u; //to set high end of values index for specified M and D commands 
//            //for M command sent
//            if(j==0){
//                //D1
//                if(k==0){i=0; max=4+i;}//add the +i for the max so that the range of i matches with the var index for sontek
//                //D2
//                else if(k==1){i=4;max=4+i;}
//                //D3
//                else if(k==2){i=8;max=1+i;}
//                //puts parsed data into struct, parameters 1-9
//                for (; i < max; i++) {  //i is initialized and set before the for loop, so the first entry is blank followed by a ";"                           
//                    (*sensor).values[i] = (float) strtod(ptr_end, &ptr_end);
//                }
//            }
//            //for M1 command
//            else if(j==1){
//                //D0
//                if(k==0){i=9; max=4+i;}
//                //D1
//                else if(k==1){i=13; max=4+i;}
//                //D2
//                else if(k==2){i=17; max=1+i;}
//                //parameters 10-18 (or 1-9 M1 parameters in manual)
//                for (; i < max; i++) {                             
//                    (*sensor).values[i] = (float) strtod(ptr_end, &ptr_end);
//                }
//            }
//            //for M2 command
//            else if(j==2){
//                //D0
//                if(k==0){i=18; max=4+i;}
//                //D1
//                else if(k==1){i=22; max=4+i;}
//                //D2
//                else if(k==2){i=26; max=1+i;}
//                //parameters 19-27 (or 1-9 M2 parameters in manual)
//                for (; i < max; i++) {                             
//                    (*sensor).values[i] = (float) strtod(ptr_end, &ptr_end);
//                }
//            }
//        }
//    }
//    return 1u; //returns true if successful
//    
//}




/* [] END OF FILE */
