#include "notoriOS.h"
#include "temperature.h"

char temperature_received_string[1024];
int16 temperature_string_index=0;

CY_ISR_PROTO(TEMP_ISR); 
CY_ISR(TEMP_ISR){      

    while(TEMP_UART_GetRxBufferSize()==1){
        //char rx = DO_UART_GetChar();
        temperature_received_string[temperature_string_index]=TEMP_UART_GetChar();
        //printNotif(NOTIF_TYPE_EVENT, "received char: %c", rx);
        temperature_string_index++;
    }

}

// has data logger, no need to take in individual readings
// clear sensor data
void TEMP_clear() {
    TEMP_UART_ClearRxBuffer();
    memset((void*)temperature_received_string, 0, 1023);
    temperature_string_index = 0;
}

// initialize all parameters to take reading
void TEMP_Talk() {
    TEMP_UART_Start();
    TEMP_RX_SetDriveMode(PIN_DM_DIG_HIZ);
    TEMP_UART_ClearRxBuffer();
    TEMP_ISR_StartEx(TEMP_ISR);
    Power_VDD1_Write(ON);
    CyDelay(1000u);
}

// turn off parameters if not needed
void TEMP_Stop() {
    Power_VDD1_Write(OFF);
    TEMP_UART_Stop();
    TEMP_RX_SetDriveMode(PIN_DM_STRONG);
    TEMP_RX_Write(OFF);
    TEMP_TX_Write(OFF);
    TEMP_ISR_Stop();
}

TEMP_sensor_t Temp_read(){

    TEMP_clear();
    TEMP_sensor_t values;
    
    TEMP_Talk();
    //char excerpt[80]; // for debugging
    DO_UART_PutString("C,0\r"); // turn off automated sampling
    
    // turn "*OK" response off
    CyDelay(1000u);
    TEMP_UART_PutString("*OK,0\r");
    CyDelay(2000u);
    /*
    for (uint i = 0; i < 80; i++){
        excerpt[i] = do_received_string[i];  
        if (i>78u){
            printNotif(NOTIF_TYPE_EVENT,"asdf");
        }
    }
    */
    TEMP_UART_ClearRxBuffer();
    TEMP_clear(); // get rid of anything we've received so far
    // should just be the startup messages 

    // take 11 readings
    for (int i = 0; i < 11; i++){
        TEMP_UART_PutString("R\r"); // take one reading
        CyDelay(1200u); // docs say readings take one second
    }


    char *token;    
    token = strtok(temperature_received_string, "\r");

    int i = 0;
    float32 reading;
    while( token != NULL ) {
        reading = atof(token);
        values.all_TEMP_readings[i] = reading;
        token = strtok(NULL, "\r"); // This "NULL" says to continue where you left off last time.    
        i++;
    }

    // readings array should have eleven nice floats now
    // TODO: check this and throw an error if not
   
    values.TEMP_reading = float_find_median(values.all_TEMP_readings, 11);
    
    // turn "*OK" response back on (this is default and useful for debugging)
    TEMP_UART_PutString("*OK,1\r");
    CyDelay(1000u);
    TEMP_Stop();
    

    
    
    return values;
}

test_t TEMP_sensor_test(){
    TEMP_clear();
    char excerpt[300]; // for debugging
    // only call this in the lab for initial setup (calibration to air)
    TEMP_UART_PutString("Factory\r"); // take one reading
    CyDelay(2000u); // docs say readings take one second
    // DO_cal() should be commented out in the main repo and never called in the field
    //DO_cal();
    
    TEMP_sensor_t tresults = TEMP_Read();

    test_t test;
    
        test.status = 0;
        // test reason  
        snprintf(test.reason,sizeof(test.reason), 
        "DO readings invalid\r\nDetails:\r\nmedian reading:%f \r\nall readings (11): %f : %f : %f : %f : %f : %f : %f : %f : %f : %f : %f",
        tresults.TEMP_reading,
        tresults.all_TEMP_readings[0], tresults.all_TEMP_readings[1], tresults.all_TEMP_readings[2], 
        tresults.all_TEMP_readings[3], tresults.all_TEMP_readings[4], tresults.all_TEMP_readings[5], 
        tresults.all_TEMP_readings[6], tresults.all_TEMP_readings[7], tresults.all_TEMP_readings[8], 
        tresults.all_TEMP_readings[9], tresults.all_TEMP_readings[10]
        );
        return test;

    // for now, getting something besides zeros is valid
    test.status = 1; // set test status to zero
    snprintf(test.test_name,sizeof(test.test_name),"TEST_DO_SENSOR");
   
    
    for (uint i = 0; i < 300; i++){
        excerpt[i] = temperature_received_string[i];  
    }
    

    return test;
    
}

void Temp_Debug() {
    
    TEMP_clear();
    TEMP_Talk();
    char excerpt[300]; // for debugging
    // take 11 readings
    for (int i = 0; i < 11; i++){
        TEMP_UART_PutString("R\r"); // take one reading
        CyDelay(1200u); // docs say readings take one second
    }
    TEMP_UART_PutString("*OK,1\r");
    CyDelay(1000u);
    TEMP_UART_PutString("C,1\r");
    CyDelay(5000u);
    for (uint i = 0; i < 300; i++){
        excerpt[i] = temperature_received_string[i];  
    }
    printNotif(NOTIF_TYPE_EVENT, "Temperature Sensor is Initialized");
    
    TEMP_Stop();
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







/*
T_sensor_t TEMP_read(){
    TEMP_clear();
    T_sensor_t tf;
    
    TEMP_Talk();
    TEMP_UART_PutString("C,0\r"); // turn off automated sampling
    
    // turn "*OK" response off
    CyDelay(1000u);
    TEMP_UART_PutString("*OK,0\r");
    CyDelay(2000u);

    TEMP_UART_ClearRxBuffer();
    TEMP_clear(); // get rid of anything we've received so far
    // should just be the startup messages 

    // take 11 readings
    for (int i = 0; i < 11; i++){
        TEMP_UART_PutString("R\r"); // take one reading
        CyDelay(1200u); // docs say readings take one second
    }
    char *token;    
    token = strtok(temperature_received_string, "\r");

    int i = 0;
    float32 reading;
    while( token != NULL ) {
        reading = atof(token);
        tf.all_TEMP_readings[i] = reading;
        token = strtok(NULL, "\r"); // This "NULL" says to continue where you left off last time.    
        i++;
    }

    tf.TEMP_reading = float_find_median(tf.all_TEMP_readings, 11);
    
    // turn "*OK" response back on (this is default and useful for debugging)
    TEMP_UART_PutString("*OK,1\r");
    CyDelay(1000u);
    TEMP_Stop();
    return tf;
}

test_t TEMP_test(){
    TEMP_UART_PutString("Factory\r"); // take one reading
    CyDelay(2000u); // docs say readings take one second
    
    T_sensor_t values = TEMP_read();

   test_t test;
    
    // test reason  
    snprintf(test.reason,sizeof(test.reason), 
    "DO readings invalid\r\nDetails:\r\nmedian reading:%f \r\nall readings (11): %f : %f : %f : %f : %f : %f : %f : %f : %f : %f : %f",
    values.TEMP_reading,
    values.all_TEMP_readings[0], values.all_TEMP_readings[1], values.all_TEMP_readings[2], 
    values.all_TEMP_readings[3], values.all_TEMP_readings[4], values.all_TEMP_readings[5], 
    values.all_TEMP_readings[6], values.all_TEMP_readings[7], values.all_TEMP_readings[8], 
    values.all_TEMP_readings[9], values.all_TEMP_readings[10]
    );
    
    return test;
 
    test.status = 1; // set test status to zero
    snprintf(test.test_name,sizeof(test.test_name),"TEST_TEMP_SENSOR");

    return test;
    
}


void Temp_Debug() {
    
    TEMP_clear();
    TEMP_Talk();
    char excerpt[300]; // for debugging
    // take 11 readings
    for (int i = 0; i < 11; i++){
        TEMP_UART_PutString("R\r"); // take one reading
        CyDelay(1200u); // docs say readings take one second
    }
    TEMP_UART_PutString("*OK,1\r");
    CyDelay(1000u);
    TEMP_UART_PutString("C,1\r");
    CyDelay(5000u);
    for (uint i = 0; i < 300; i++){
        excerpt[i] = temperature_received_string[i];  
    }
    printNotif(NOTIF_TYPE_EVENT, "Temperature Sensor is Initialized");
    
    TEMP_Stop();
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

*/