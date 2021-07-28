#include "notoriOS.h"
// #include diss_oxy.h  - this was causing a multiple definition error
char do_received_string[1024];
int16 do_string_index=0;

// mostly copied from modem.c

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

DO_sensor_t DO_read(){
    
    DO_sensor_t output;
    
    DO_UART_Start();
    DO_UART_ClearRxBuffer();
    DO_ISR_StartEx(DO_ISR);
    Level_Sensor_Power_Write(ON); // Pulls pwr pin high (turns it on).
    DO_UART_PutString("C,0"); // turn off automated sampling
    CyDelay(2000u);
    
    
    // get the tokens we actually want which are readings

    for (int i = 0; i < 11; i++){
        DO_UART_PutString("R"); // take one reading
        CyDelay(1000u);
    }

    // eat the first two tokens because they're not measurements
    char *token;
    
    token = strtok(do_received_string, "\r");
    token = strtok(NULL, "\r"); // eat *RS/r
    token = strtok(NULL, "\r"); // eat RE/r
    uint8 i = 0;
    float32 reading;
    while( token != NULL ) {
        reading = atof(token);
        output.all_do_readings[i] = reading;
        token = strtok(NULL, "\r"); // This "NULL" says to continue where you left off last time.    
        i++;
    }

    // readings array should have eleven nice floats now
   
    output.do_reading = float_find_median(output.all_do_readings, 11);

    return output;
}

test_t DO_sensor_test(){

    
    DO_sensor_t results = DO_read();

    test_t test;

    test.status = 0; // set test status to zero
    snprintf(test.test_name,sizeof(test.test_name),"TEST_DO_SENSOR");

    
    /*
    DO_UART_Start();
    DO_ISR_StartEx(DO_ISR);
    Level_Sensor_Power_Write(ON); // Pulls pwr pin high (turns it on).
    CyDelay(6000u);
    DO_UART_PutString("i\r");
    
    CyDelay(6000u);
    DO_UART_PutString("Find\r");
    CyDelay(6000u);
    Level_Sensor_Power_Write(OFF);
    
    // as in level_sensor.c
    
    char *token;
    
    token = strtok(do_received_string, "\r");
        // walk through other tokens /
    while( token != NULL ) {
        float32 reading = atof(token);
        token = strtok(NULL, "\r"); // This "NULL" says to continue where you left off last time.
        printNotif(NOTIF_TYPE_EVENT,"%s",token);     
   }
    
    char excerpt[800];
    for (int i = 0; i < 800; i++){
        excerpt[i] = do_received_string[i];  
        if (i>797){
            printNotif(NOTIF_TYPE_EVENT,"asdf");
        }
    }

    Level_Sensor_Power_Write(OFF);
    */
    

    // test print
    
    snprintf(test.reason,sizeof(test.reason), 
    "median reading:%f \r\nall readings: %f\r\n%f\r\n%f\r\n%f\r\n%f\r\n%f\r\n%f\r\n%f\r\n%f\r\n%f\r\n%f\r\n",
    results.do_reading,
    results.all_do_readings[0], results.all_do_readings[1], results.all_do_readings[2], 
    results.all_do_readings[3], results.all_do_readings[4], results.all_do_readings[5], 
    results.all_do_readings[6], results.all_do_readings[7], results.all_do_readings[8], 
    results.all_do_readings[9], results.all_do_readings[10]
    );
    
    return test;
    
}

// code for finding medians duplicated from level_sensor because we need floats not longs

// function to calculate the median of the array, after array is sorted
float32 float_find_median(float32 array[] , uint8 n)
{
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