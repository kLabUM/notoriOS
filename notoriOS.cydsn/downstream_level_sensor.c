#include "downstream_level_sensor.h"
#include "notoriOS.h"

// Circular buffer of recently received chars.
// Initialize all the members of the array uart_chars to 0. 
static char uart_chars[257] = {0};
static uint8_t num_chars = 0;

static const char *line = uart_chars;
// Only pointers can be null, where null is a special address used to signify that the pointer is not pointing to any valid address in memory.
// Pointers are not initialized to null by default, you must do so if you want them initialized.
static const char *match = NULL;
static const char *pattern = NULL;

// ISR: Interrupt Service Routine
CY_ISR(Downstream_Level_Sensor_ISR) {
    /* Store received char in uart_chars.
    Returns the last received byte of data. Level_Sensor_UART_GetChar() is designed for ASCII characters and returns a uint8 where 
    1 to 255 are values for valid characters and 0 indicats an error occurred or no data is present.*/
    char rx_char = Downstream_Level_Sensor_UART_GetChar();
    // UART_GetChar() returns data, then set the entire uart_chars array to rx_char.
    if (rx_char) uart_chars[num_chars++] = rx_char;

    /* Check pattern at end of line */
    if (rx_char == '\r') {
        // strstr(): char *strstr(const char *haystack, const char *needle). haystack - main C string to be scanned. needle - the small string to be searched with-in haystack string.
        match = strstr(line, pattern);
        line = uart_chars + num_chars;
    }
}

void Downstream_Level_Sensor_Update(char * message){
    strcpy(downstream_level_sensor_inbox, message);
}

uint8 downstream_level_sensor(){
    char * compare_location;
    
    compare_location = strstr(downstream_level_sensor_inbox,"OFF");        
    if(compare_location!=NULL){
        return 0; // this app is disabled, don't do anything
    }
    

    compare_location = strstr(downstream_level_sensor_inbox,"ON");        
    if(compare_location!=NULL){

        // below is copied and adapted from makeMeasurements in modem.c 
        // (duplicated so that normal depth and GI nodes still work)
        
        // Get clock time and save to timeStamp
        long timeStamp = getTimeStamp();
        char c_timeStamp[32];
        snprintf(c_timeStamp,sizeof(c_timeStamp),"%ld",timeStamp);
        
        // Holds string for value that will be written 
        char value[DATA_MAX_KEY_LENGTH];
        
        // level_sensor_t is a new data type we defined in level_sensor.h. We then use that data type to define a structure variable m_level_sensor
        level_sensor_t m_level_sensor;
        
        // Take level sensor readings and save them to m_level_sensor
        m_level_sensor = downstream_level_sensor_take_reading();

        // If the number of valid level sensor readings is greater than 0, then print the level sensor reading, and push the data to the data wheel
        if(m_level_sensor.num_valid_readings > 0){
            snprintf(value,sizeof(value),"%d",m_level_sensor.level_reading);
            printNotif(NOTIF_TYPE_EVENT,"downstream_maxbotix_depth=%s",value);
            pushData("downstream_maxbotix_depth",value,timeStamp);
            
            // Print measurement to SD card to file called data.txt
            SD_write("data.txt", "a+", c_timeStamp);
            SD_write("data.txt", "a+", " downstream_maxbotix_depth: ");
            SD_write("data.txt", "a+", value);
            SD_write("data.txt", "a+", " ");
        }else{
            printNotif(NOTIF_TYPE_ERROR,"Could not get valid readings from downstream Maxbotix.");
            pushData("downstream_maxbotix_depth","-1",timeStamp); // error code, never get this from a sensor
        }
        

        return 0;
    }
    
    return 0; // i think this needs to be rewritten to be more like a light switch
    // i.e. it stays on until you turn it off.
}

// takes level sensor measurements and calculates average level sensor reading
level_sensor_t downstream_level_sensor_take_reading(){
    
    // level_sensor_t is a new data type we defined in level_sensor.h. We then use that data type to define a structure variable level_sensor_output.
    level_sensor_t Downstream_Level_Sensor_output;
    Downstream_Level_Sensor_output.num_invalid_readings = 0;
    Downstream_Level_Sensor_output.num_valid_readings = 0;
    
    uart_clear();
    
    // Start sensor power, uart, and isr
    Downstream_Level_Sensor_Power_Write(1u);   // Pulls Level Sensor power pin high (turns it on).
    CyDelay(200u); // Sensor bootup sequence ~200ms -- senosor will spit out ID info, but we don't need that
    Downstream_Level_Sensor_RX_SetDriveMode(PIN_DM_DIG_HIZ); // RX_SetDriveMode(): Sets the drive mode for each of the Pins component's pins. PIN_DM_DIG_HIZ: High Impedance Digital.
    Downstream_Level_Sensor_UART_Start(); // Downstream_Level_Sensor_UART_Start() sets the initVar variable, calls the Downstream_Level_Sensor_UART_Init() function, and then calls the Downstream_Level_Sensor_UART_Enable() function.
    Downstream_Level_Sensor_ISR_StartEx(Downstream_Level_Sensor_ISR); // Sets up the interrupt and enables it.
    
    // Wait to take  reading
    // Sensor will take ~200ms to send first reading, and 200ms for subsequent readings
    CyDelay(2000u); // Take a few seconds to get readings, 2 secs ~10 readings
    
    //stop all sensor processes
    Downstream_Level_Sensor_UART_Stop(); // Disables the UART operation.
    Downstream_Level_Sensor_ISR_Stop(); // Disables and removes the interrupt.
    Downstream_Level_Sensor_Power_Write(0u); // Pulls Level Sensor Power pin low (turns it off).
    // RX_SetDriveMode(): Sets the drive mode for each of the Pins component's pins.
    // String Drive Mode means it will only be outputting a voltage (don't send any data) when pulled low.
    // UART in PSOC starts high. Then pulls low to get data. When turn sensor off, still using power because UART still high.
    // So turning it to Strong Drive has it stay low.
    Downstream_Level_Sensor_RX_SetDriveMode(PIN_DM_STRONG);
    // Stops writing the value to the physical port.
    // Just because turn power of sensor off, pin could still be high and getting power. This ensures it stays low.
    Downstream_Level_Sensor_RX_Write(0u);

    /* Return parsed reading */
    // Parse string and put all values into array
    // The sensor will power on and do a few things, after which the readigs will follow with the seqeunce
    // Readings are stashed between "R" and "\000"
    char *token;
    // strtok(): breaks uart_chars into smaller string when sees "R".
    token = strtok(uart_chars, "R");
    /* walk through other tokens */
   while( token != NULL ) 
   {
      if(strstr(token, "\r") != NULL){ // Full reading is delimieted by "R" + reading + "\r"
        // int atoi(const char *str) converts the string argument str to an integer (type int).
        uint16 reading = atoi(token);
        if(reading != 0){
            if(reading != READING_INVALID){ // Invald readings by maxbotix
                // Set the level readings array of the length of number of valid readings equal to the reading
                Downstream_Level_Sensor_output.all_level_readings[Downstream_Level_Sensor_output.num_valid_readings] = reading;
                // If number of valid readings less than 11 total readings, increase the count of valid readings
                if(Downstream_Level_Sensor_output.num_valid_readings < N_READINGS){
                    Downstream_Level_Sensor_output.num_valid_readings++;
                }else{
                 break;   
                }
                
            }else{
                // Otherwise increase the count of invalid readings
                Downstream_Level_Sensor_output.num_invalid_readings++;
            }
        }
      }
      token = strtok(NULL, "R"); // This "NULL" says to continue where you left off last time.
   }
    // Calculate median level reading
    Downstream_Level_Sensor_output.level_reading = find_median(Downstream_Level_Sensor_output.all_level_readings,Downstream_Level_Sensor_output.num_valid_readings);
    
    return Downstream_Level_Sensor_output;
}

// Level sensor test
test_t downstream_level_sensor_test(){
    
    test_t test; // test_t is a new data type we defined in test.h. We then use that data type to define a structure variable test
    test.status = 0; // set test status to zero
    snprintf(test.test_name,sizeof(test.test_name),"TEST_DOWNSTREAM_LEVEL_SENSOR");

    // level_sensor_t is a new data type we defined in level_sensor.h. We then use that data type to define a structure variable sensor
    level_sensor_t sensor = downstream_level_sensor_take_reading();
    
    // print the max number of readings (11), the number of actual readings, the number of valid readings, and the median level reading
    snprintf(test.reason,sizeof(test.reason),"nWANTED=%d,nRECEIVED=%d,nVALID=%d,nMEDIAN=%d.",
        N_READINGS,
        sensor.num_valid_readings+sensor.num_invalid_readings,
        sensor.num_valid_readings,
        sensor.level_reading
    );
   
    // if the number valid readings is the max number of readings (11), then the test passes with test status = 1
    if(sensor.num_valid_readings == N_READINGS){//pass
        test.status = 1;
    // if the number of valid readings is greater than zero and less than the max number of readings (11), then test passes with test status = 2
    }else if((sensor.num_valid_readings > 0) && (sensor.num_valid_readings < N_READINGS)){//pass
        test.status = 2; //not bad, since we got some OK readings
    }
        
    return test;
}