#include "notoriOS.h"
// #include diss_oxy.h  - this was causing a multiple definition error
char do_received_string[1024];
int16 do_string_index=0;

// adapted from modem.c and level_sensor.c

CY_ISR_PROTO(DO_ISR); // Declares a custom ISR function "isr_telit_rx" using the CY_ISR_PROTO macro instead of modifying the auto-generated code

// Define the custom ISR function "isr_telit_rx" using the CY_ISR macro
// Get each character from Telit and saves it to uart_received_string
CY_ISR(DO_ISR){      
    // UART_Telit_GetRxBufferSize() returns the number of received bytes available in the RX buffer.

    while(DO_UART_GetRxBufferSize()==1){
        //char rx = DO_UART_GetChar();
        do_received_string[do_string_index]=DO_UART_GetChar();
        //printNotif(NOTIF_TYPE_EVENT, "received char: %c", rx);
        do_string_index++;

    }

}


// Clears the sensor data 
void do_uart_clear(void) {
  // UART_ClearRxBuffer(): Clears the receiver memory buffer and hardware RX FIFO of all received data.
  DO_UART_ClearRxBuffer();
  // void *memset(void *str, int c, size_t n) where str is a pointer to the block of memory to fill;  c is the value to be set. 
  // The value is passed as an int, but the function fills the block of memory using the unsigned char conversion of this value.
  // n is the number of bytes to be set to the value.
  memset((void*)do_received_string, 0, 1023);
  do_string_index = 0;
}

// get everything ready to communicate
void DO_start_talking(){
    DO_UART_Start();
    DO_RX_SetDriveMode(PIN_DM_DIG_HIZ); // RX_SetDriveMode(): Sets the drive mode for each of the Pins component's pins. PIN_DM_DIG_HIZ: High Impedance Digital.
    DO_UART_ClearRxBuffer();
    DO_ISR_StartEx(DO_ISR);
    Level_Sensor_Power_Write(ON); // Pulls pwr pin high (turns it on).
    CyDelay(1000u); // reset sequence

}

// pull everything low to stop power leaks
void DO_stop_talking(){

    Level_Sensor_Power_Write(OFF);
    DO_UART_Stop();
    DO_RX_SetDriveMode(PIN_DM_STRONG); // to die
    DO_RX_Write(OFF);
    DO_TX_Write(OFF);
    DO_ISR_Stop();

}

DO_sensor_t DO_read(){

    do_uart_clear();
    DO_sensor_t output;
    
    DO_start_talking();
    //char excerpt[80]; // for debugging
    DO_UART_PutString("C,0\r"); // turn off automated sampling
    
    // turn "*OK" response off
    CyDelay(1000u);
    DO_UART_PutString("*OK,0\r");
    CyDelay(2000u);
    /*
    for (uint i = 0; i < 80; i++){
        excerpt[i] = do_received_string[i];  
        if (i>78u){
            printNotif(NOTIF_TYPE_EVENT,"asdf");
        }
    }
    */
    DO_UART_ClearRxBuffer();
    do_uart_clear(); // get rid of anything we've received so far
    // should just be the startup messages 

    // take 11 readings
    for (int i = 0; i < 11; i++){
        DO_UART_PutString("R\r"); // take one reading
        CyDelay(1200u); // docs say readings take one second
    }


    char *token;    
    token = strtok(do_received_string, "\r");

    int i = 0;
    float32 reading;
    while( token != NULL ) {
        reading = atof(token);
        output.all_do_readings[i] = reading;
        token = strtok(NULL, "\r"); // This "NULL" says to continue where you left off last time.    
        i++;
    }

    // readings array should have eleven nice floats now
    // TODO: check this and throw an error if not
   
    output.do_reading = float_find_median(output.all_do_readings, 11);
    
    // turn "*OK" response back on (this is default and useful for debugging)
    DO_UART_PutString("*OK,1\r");
    CyDelay(1000u);
    DO_stop_talking();
    

    
    
    return output;
}

test_t DO_sensor_test(){
    do_uart_clear();
    char excerpt[300]; // for debugging
    // only call this in the lab for initial setup (calibration to air)
    DO_UART_PutString("Factory\r"); // take one reading
    CyDelay(2000u); // docs say readings take one second
    // DO_cal() should be commented out in the main repo and never called in the field
    DO_cal();
    
    DO_sensor_t results = DO_read();

    test_t test;
    
    if (results.do_reading < 0.01){
        test.status = 0;
        // test reason  
        snprintf(test.reason,sizeof(test.reason), 
        "DO readings invalid\r\nDetails:\r\nmedian reading:%f \r\nall readings (11): %f : %f : %f : %f : %f : %f : %f : %f : %f : %f : %f",
        results.do_reading,
        results.all_do_readings[0], results.all_do_readings[1], results.all_do_readings[2], 
        results.all_do_readings[3], results.all_do_readings[4], results.all_do_readings[5], 
        results.all_do_readings[6], results.all_do_readings[7], results.all_do_readings[8], 
        results.all_do_readings[9], results.all_do_readings[10]
        );
        return test;
    }

    // for now, getting something besides zeros is valid
    test.status = 1; // set test status to zero
    snprintf(test.test_name,sizeof(test.test_name),"TEST_DO_SENSOR");
   
    
    // test reason  
    snprintf(test.reason,sizeof(test.reason), 
    "median reading:%f \r\nall readings (11): %f : %f : %f : %f : %f : %f : %f : %f : %f : %f : %f",
    results.do_reading,
    results.all_do_readings[0], results.all_do_readings[1], results.all_do_readings[2], 
    results.all_do_readings[3], results.all_do_readings[4], results.all_do_readings[5], 
    results.all_do_readings[6], results.all_do_readings[7], results.all_do_readings[8], 
    results.all_do_readings[9], results.all_do_readings[10]
    );
    
    for (uint i = 0; i < 300; i++){
        excerpt[i] = do_received_string[i];  
    }
    

    return test;
    
}

uint8 DO_cal(){
    do_uart_clear(); // forget what you think you know
    char excerpt[300]; // for debugging
    for (int i = 0; i < 2; i++){
        DO_sensor_t readings = DO_read();
        // I'll take convergence as the range less than 5 percent the median
        // this is arbitrary but information we can already generate
        fsort(readings.all_do_readings,DO_N_READINGS); // sort it
        float32 range = readings.all_do_readings[DO_N_READINGS-1] - readings.all_do_readings[0];
        
        for (uint i = 0; i < 300; i++){
            excerpt[i] = do_received_string[i];  
        }
        
        // we did it in less than two tries so it's converging quickly as expected
        // send calibration command and return 1 indicating success
        if (range <= (0.05*readings.do_reading)){
            
            DO_start_talking();
            /*
            
            //DO_UART_PutString("*OK,1\r"); // are you hearing me?
            // 
            //CyDelay(1000u); 
            */
            DO_UART_PutString("C,0\r"); // turn off automated sampling
            CyDelay(1000u); 
            DO_UART_PutString("Cal,clear\r"); // clear existing calibration data
            CyDelay(1000u);
            DO_UART_PutString("Cal\r"); // calibrate to atmospheric oxygen
            /*CyDelay(1000u); 
            // "temp, salinity, and pressure compensation values have no effect on calibration"
            DO_UART_PutString("T,?\r"); // compensated temp value?
            CyDelay(1000u); 
            //DO_UART_PutString("T,20\r"); // for some reason temp was set to 0 C
            //CyDelay(1000u); 
            //DO_UART_PutString("T,?\r"); // compensated temp value?
            //CyDelay(1000u); 
            DO_UART_PutString("S,?\r"); // salinity?
            CyDelay(1000u); 
            DO_UART_PutString("P,?\r"); // pressure?
            */
            CyDelay(2000u); 
            DO_stop_talking();
            // "After calibration is complete, you should see readings between 9.09 - 9.2 mg/l
            // if temperature, salinity, and pressure compensation are at default values"
            

            
            CyDelay(1000u);
            DO_sensor_t calibrated = DO_read();
            calibrated.all_do_readings;
            if (calibrated.do_reading > 9.0 && calibrated.do_reading < 9.3){
                printNotif(NOTIF_TYPE_EVENT, "Successfully calibrated DO sensor");
                return 1;
            }
            // if we didn't get what we expected, let us know what we did get
            printNotif(NOTIF_TYPE_ERROR, "calibrated DO reading: %f ::: Expected [9.0,9.3] mg/L", calibrated.do_reading);           
            return 0;
            
        }
    
    }

    printNotif(NOTIF_TYPE_ERROR, "DO readings failed to converge");
    return 0;   
}


// code for finding medians duplicated from level_sensor because we need floats not longs

// function to calculate the median of the array, after array is sorted
float32 float_find_median(float32 array[] , uint8 n){
    float32 median=0;
    fsort(array,n);
    
    // if number of elements are even
    if(n%2 == 0)
        // median is the average of the  two middle sorted numbers
        median = (array[(n-1)/2] + array[n/2])/2.0;
    // if number of elements are odd
    else
        // median is the the middle sorted number 
        median = array[n/2];
    
    return median;
}

// function to sort the elements in the level readings array
void fsort(float32 a[],uint8 n) { 
   uint8 i,j;

   for(i = 0;i < n-1;i++) {
      for(j = 0;j < n-i-1;j++) {
         if(a[j] > a[j+1])
            fswap(&a[j],&a[j+1]); // swap the address of a[j] and a[j+1]
      }
   }
}

// function to swap elements in the level readings array in order to sort them
void fswap(float32 *p,float32 *q) {
   float32 t;
   
   t=*p; 
   *p=*q; 
   *q=t;
}


