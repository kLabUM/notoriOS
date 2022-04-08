#include "notoriOS.h"
//#include "water_quality.h"

char wq_received_string[1024];
int16 wq_string_index = 0;

CY_ISR_PROTO(WQ_ISR); // Declares a custom ISR function "isr_telit_rx" using the CY_ISR_PROTO macro instead of modifying the auto-generated code

// Define the custom ISR function "isr_telit_rx" using the CY_ISR macro
// Get each character from Telit and saves it to uart_received_string
CY_ISR(WQ_ISR){      
    // UART_Telit_GetRxBufferSize() returns the number of received bytes available in the RX buffer.

    while(WQ_UART_GetRxBufferSize()== 1){
        //char rx = DO_UART_GetChar();
        wq_received_string[wq_string_index] = WQ_UART_GetChar();
        //printNotif(NOTIF_TYPE_EVENT, "received char: %c", rx);
        wq_string_index ++;

    }

}

// Clears the sensor data 
void wq_uart_clear(void) {
  // UART_ClearRxBuffer(): Clears the receiver memory buffer and hardware RX FIFO of all received data.
  WQ_UART_ClearRxBuffer();
  // void *memset(void *str, int c, size_t n) where str is a pointer to the block of memory to fill;  c is the value to be set. 
  // The value is passed as an int, but the function fills the block of memory using the unsigned char conversion of this value.
  // n is the number of bytes to be set to the value.
  memset((void*)wq_received_string, 0, 1023);
  wq_string_index = 0;
}

// get everything ready to communicate
void wq_start_talking(){
    // I think the below two lines are not necessary for digital multiplexers, just analog
    //RXmux_Start();       // Start the Digital Multiplexer
    //TXmux_Start();       // Start the Digital Multiplexer
    
    WQ_UART_Start();
    
    // sensor specific calls
    DO_RX_SetDriveMode(PIN_DM_DIG_HIZ); // RX_SetDriveMode(): Sets the drive mode for each of the Pins component's pins. PIN_DM_DIG_HIZ: High Impedance Digital.
    TEMP_RX_SetDriveMode(PIN_DM_DIG_HIZ); // RX_SetDriveMode(): Sets the drive mode for each of the Pins component's pins. PIN_DM_DIG_HIZ: High Impedance Digital.
    
    WQ_UART_ClearRxBuffer();
    WQ_ISR_StartEx(WQ_ISR);
    Level_Sensor_Power_Write(ON); // Pulls pwr pin high (turns it on).
    CyDelay(1000u); // reset sequence

}

// pull everything low to stop power leaks
void wq_stop_talking(){

    Level_Sensor_Power_Write(OFF);
    WQ_UART_Stop();
    
    // sensor specific calls
    DO_RX_SetDriveMode(PIN_DM_STRONG); // to die
    TEMP_RX_SetDriveMode(PIN_DM_STRONG); // to die
    DO_RX_Write(OFF);
    TEMP_RX_Write(OFF);
    DO_TX_Write(OFF);
    TEMP_TX_Write(OFF);
    
    WQ_ISR_Stop();

}

wq_sensors_t wq_take_reading(){

    wq_uart_clear();
    wq_sensors_t output;
    
    wq_start_talking();
    
    rx_mux_controller_Wakeup();
    tx_mux_controller_Wakeup();
    
    // first, everyone shut up
    for (uint8 channel = 0; channel < N_params; channel++){
        
        rx_mux_controller_Write(channel);
        tx_mux_controller_Write(channel); // talking and listening on the same channel
        //char excerpt[80]; // for debugging
        WQ_UART_PutString("C,0\r"); // turn off automated sampling
        
        // turn "*OK" response off
        CyDelay(1000u);
        WQ_UART_PutString("*OK,0\r");


    }
    // CyDelay(2000u);

    wq_uart_clear(); // get rid of anything we've received so far
    // now let's take some readings
    for (uint8 channel = 0; channel < N_params; channel++){
        
        rx_mux_controller_Write(channel);
        tx_mux_controller_Write(channel); // talking and listening on the same channel
        
        // take 11 readings
        for (int i = 0; i < 11; i++){
            WQ_UART_PutString("R\r"); // take one reading
            CyDelay(1200u); // docs say readings take one second
        }
        
        char *token;    
        token = strtok(wq_received_string, "\r");

        int i = 0;
        float32 reading;
        while( token != NULL ) {
            reading = atof(token);
            if (channel < 1){ // we're reading DO
                output.all_do_readings[i] = reading;
            }
            else if (channel < 2) { // we're reading temperature
                output.all_temp_readings[i] = reading;
            }
            
            token = strtok(NULL, "\r"); // This "NULL" says to continue where you left off last time.    
            i++;
        }
        
        wq_uart_clear();

        // readings array should have eleven nice floats now
        // TODO: check this and throw an error if not
        
    }
    
    rx_mux_controller_Sleep();
    tx_mux_controller_Sleep();
   
    output.do_reading = float_find_median(output.all_do_readings, 11);
    output.temp_reading = float_find_median(output.all_temp_readings, 11);
    
    // turn "*OK" response back on (this is default and useful for debugging)
    for (uint8 channel = 0; channel < N_params; channel++){
        
        rx_mux_controller_Write(channel);
        tx_mux_controller_Write(channel); // talking and listening on the same channel
        WQ_UART_PutString("*OK,1\r");

    }
    
    
    CyDelay(1000u);
    wq_stop_talking();
    
    return output;
}

test_t wq_sensor_test(){
    wq_uart_clear();
    char excerpt[300]; // for debugging

    wq_sensors_t results = wq_take_reading();

    test_t test;
    
    // I don't think the way the tests fail is ideal, 
    // can't tell if temperature is working or not if DO fails currently
    
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
    
    if (results.temp_reading > 150){ // not sure what the error reading actually is
        // this cutoff threshold needs to be changed to reflect the documenation of the sensor
        test.status = 0;
        // test reason  
        snprintf(test.reason,sizeof(test.reason), 
        "Temperature readings invalid\r\nDetails:\r\nmedian reading:%f \r\nall readings (11): %f : %f : %f : %f : %f : %f : %f : %f : %f : %f : %f",
        results.temp_reading,
        results.all_temp_readings[0], results.all_temp_readings[1], results.all_temp_readings[2], 
        results.all_temp_readings[3], results.all_temp_readings[4], results.all_temp_readings[5], 
        results.all_temp_readings[6], results.all_temp_readings[7], results.all_temp_readings[8], 
        results.all_temp_readings[9], results.all_temp_readings[10]
        );
        return test;
    }

    // for now, getting something besides zeros is valid
    test.status = 1; // set test status to zero
    snprintf(test.test_name,sizeof(test.test_name),"TEST_wq_SENSOR");
   
    
    // test reason  
    snprintf(test.reason,sizeof(test.reason), 
    "median DO reading:%f \r\nmedian Temperature Reading: %f ",
    results.do_reading,
    results.temp_reading
    );
    
    for (uint i = 0; i < 300; i++){
        excerpt[i] = wq_received_string[i];  
    }
    

    return test;
    
}

// ref: https://atlas-scientific.com/kits/dissolved-oxygen-kit/
// DO EZO circuit datasheet -> UART calibration
// let DO probe sit exposed to air until readings stabilize, then send "cal" command
// make sure to put DO probe back in water after calibrated so it doesn't dry out
uint8 DO_cal(){
    wq_uart_clear(); // forget what you think you know
    char excerpt[300]; // for debugging
    
    
    for (int i = 0; i < 2; i++){
        wq_sensors_t readings = wq_take_reading();
        // I'll take convergence as the range less than 5 percent the median
        // this is arbitrary but information we can already generate
        fsort(readings.all_do_readings,N_READINGS); // sort it
        float32 range = readings.all_do_readings[N_READINGS-1] - readings.all_do_readings[0];
        
        for (uint i = 0; i < 300; i++){
            excerpt[i] = wq_received_string[i];  
        }
        
        // we did it in less than two tries so it's converging quickly as expected
        // send calibration command and return 1 indicating success
        if (range <= (0.05*readings.do_reading)){
            
            wq_start_talking();
            rx_mux_controller_Write(0);// DO is parameter 0
            tx_mux_controller_Write(0); // talking and listening on the same channel
        
            
            //DO_UART_PutString("*OK,1\r"); // are you hearing me?
            // 
            CyDelay(1000u); 
            //
            WQ_UART_PutString("C,0\r"); // turn off automated sampling
            CyDelay(1000u); 
            WQ_UART_PutString("Cal,clear\r"); // clear existing calibration data
            CyDelay(1000u);
            WQ_UART_PutString("Cal\r"); // calibrate to atmospheric oxygen

            CyDelay(2000u); 
            wq_stop_talking();
            // "After calibration is complete, you should see readings between 9.09 - 9.2 mg/l
            // if temperature, salinity, and pressure compensation are at default values"
            
      
            CyDelay(1000u);
            wq_sensors_t calibrated = wq_take_reading();
            //calibrated.all_do_readings;
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

// ref: https://atlas-scientific.com/kits/pt-1000-temperature-kit/
// EZO RTD circuit datasheet -> single point calibration
// use boiling water
uint8 Temperature_cal(){
    wq_uart_clear(); // forget what you think you know
    char excerpt[300]; // for debugging
    
    
    for (int i = 0; i < 2; i++){
        wq_sensors_t readings = wq_take_reading();
       
        for (uint i = 0; i < 300; i++){
            excerpt[i] = wq_received_string[i];  
        }
        
        // we did it in less than two tries so it's converging quickly as expected
        // send calibration command and return 1 indicating success
        // specify that reading is within one degree of 100 C
        if (fabs(readings.temp_reading-100) <= 1){
            
            wq_start_talking();
            rx_mux_controller_Write(1);//temperature is parameter 1
            tx_mux_controller_Write(1); // talking and listening on the same channel
        
          
            CyDelay(1000u); 
            //
            WQ_UART_PutString("C,0\r"); // turn off automated sampling
            CyDelay(1000u); 
            WQ_UART_PutString("Cal,clear\r"); // clear existing calibration data
            CyDelay(1000u);
            WQ_UART_PutString("Cal\r"); // temperature is currently reading correctly

            CyDelay(2000u); 
            wq_stop_talking();

            printNotif(NOTIF_TYPE_EVENT, "Successfully calibrated temperature sensor");
            return 1;
            
        }
        

    
    }
    wq_sensors_t readings = wq_take_reading();
    // if we didn't get what we expected, let us know what we did get
    printNotif(NOTIF_TYPE_ERROR, "temp reading: %f ::: Expected 100 +/- 1 C", readings.temp_reading);           
    return 0; 

}

// code for finding medians "duplicated" from level_sensor because we need *floats* not *longs*
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



