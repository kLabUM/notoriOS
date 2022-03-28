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

TEMP_t TEMP_read(){
    TEMP_clear();
    TEMP_t tf;
    
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

    // readings array should have eleven nice floats now
    // TODO: check this and throw an error if not
   
    
    // turn "*OK" response back on (this is default and useful for debugging)
    DO_UART_PutString("*OK,1\r");
    CyDelay(1000u);
    DO_stop_talking();
    return tf;
}

test_t TEMP_test(){
    TEMP_UART_PutString("Factory\r"); // take one reading
    CyDelay(2000u); // docs say readings take one second
    
    TEMP_t values = TEMP_read();

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

