#include "level_sensor.h"
#include <assert.h>     // Implements a runtime assertion, which can be used to verify assumptions made by the program and print a diagnostic message if this assumption is false. 
#include <stdint.h>     // Defines a set of integral type aliases with specific width requirements, along with macros specifying their limits and macro functions to create values of these types.
#include <stdbool.h>    // Contains four macros for a Boolean data type.
#include <math.h>       // Designed for basic mathematical operations. Most of the functions involve the use of floating point numbers. 
#include <stdio.h>      // Defines three variable types, several macros, and various functions for performing input and output.
#include "level_sensor.h"
#include "notoriOS.h"
#include "project.h"


/* === Driver Functions === */

//sensor realated global calls




// Circular buffer of recently received chars.
// Initialize all the members of the array uart_chars to 0. 
static char uart_chars[257] = {0};
static uint8_t num_chars = 0;

static const char *line = uart_chars;
// Only pointers can be null, where null is a special address used to signify that the pointer is not pointing to any valid address in memory.
// Pointers are not initialized to null by default, you must do so if you want them initialized.
static const char *match = NULL;
static const char *pattern = NULL;


// This is only defined in c file so this function will return the location in memory where that variable is but not access to the variable. 
const char *uart_string(void) {
    // Variables in front of a variable is casting 
    // uart_chars may be a different variable type, make sure to cast it as a char
  return (char*)uart_chars;
}

// Return the number of characters of the level sensor reading
uint8_t uart_len(void) {
  return num_chars;
}

// Clears the sensor data 
void uart_clear(void) {
  // UART_ClearRxBuffer(): Clears the receiver memory buffer and hardware RX FIFO of all received data.
  Level_Sensor_UART_ClearRxBuffer();
  // void *memset(void *str, int c, size_t n) where str is a pointer to the block of memory to fill;  c is the value to be set. 
  // The value is passed as an int, but the function fills the block of memory using the unsigned char conversion of this value.
  // n is the number of bytes to be set to the value.
  memset((void*)uart_chars, 0, 256);
  num_chars = 0;
}

// ISR: Interrupt Service Routine
CY_ISR(Level_Sensor_ISR) {
    /* Store received char in uart_chars.
    Returns the last received byte of data. Level_Sensor_UART_GetChar() is designed for ASCII characters and returns a uint8 where 
    1 to 255 are values for valid characters and 0 indicats an error occurred or no data is present.*/
    char rx_char = Level_Sensor_UART_GetChar();
    // UART_GetChar() returns data, then set the entire uart_chars array to rx_char.
    if (rx_char) uart_chars[num_chars++] = rx_char;

    /* Check pattern at end of line */
    if (rx_char == '\r') {
        // strstr(): char *strstr(const char *haystack, const char *needle). haystack - main C string to be scanned. needle - the small string to be searched with-in haystack string.
        match = strstr(line, pattern);
        line = uart_chars + num_chars;
    }
}

// takes level sensor measurements and calculates average level sensor reading
level_sensor_t level_sensor_take_reading(){
    
    // level_sensor_t is a new data type we defined in level_sensor.h. We then use that data type to define a structure variable level_sensor_output.
    level_sensor_t level_sensor_output;
    level_sensor_output.num_invalid_readings = 0;
    level_sensor_output.num_valid_readings = 0;
    
    uart_clear();
    
    // Start sensor power, uart, and isr
    Level_Sensor_Power_Write(ON);   // Pulls Level Sensor power pin high (turns it on).
    CyDelay(200u); // Sensor bootup sequence ~200ms -- senosor will spit out ID info, but we don't need that
    Level_Sensor_RX_SetDriveMode(PIN_DM_DIG_HIZ); // RX_SetDriveMode(): Sets the drive mode for each of the Pins component's pins. PIN_DM_DIG_HIZ: High Impedance Digital.
    Level_Sensor_UART_Start(); // Level_Sensor_UART_Start() sets the initVar variable, calls the Level_Sensor_UART_Init() function, and then calls the Level_Sensor_UART_Enable() function.
    Level_Sensor_ISR_StartEx(Level_Sensor_ISR); // Sets up the interrupt and enables it.
    
    // Wait to take  reading
    // Sensor will take ~200ms to send first reading, and 200ms for subsequent readings
    CyDelay(2000u); // Take a few seconds to get readings, 2 secs ~10 readings
    
    //stop all sensor processes
    Level_Sensor_UART_Stop(); // Disables the UART operation.
    Level_Sensor_ISR_Stop(); // Disables and removes the interrupt.
    Level_Sensor_Power_Write(OFF); // Pulls Level Sensor Power pin low (turns it off).
    // RX_SetDriveMode(): Sets the drive mode for each of the Pins component's pins.
    // String Drive Mode means it will only be outputting a voltage (don't send any data) when pulled low.
    // UART in PSOC starts high. Then pulls low to get data. When turn sensor off, still using power because UART still high.
    // So turning it to Strong Drive has it stay low.
    Level_Sensor_RX_SetDriveMode(PIN_DM_STRONG);
    // Stops writing the value to the physical port.
    // Just because turn power of sensor off, pin could still be high and getting power. This ensures it stays low.
    Level_Sensor_RX_Write(OFF);

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
                level_sensor_output.all_level_readings[level_sensor_output.num_valid_readings] = reading;
                // If number of valid readings less than 11 total readings, increase the count of valid readings
                if(level_sensor_output.num_valid_readings < N_READINGS){
                    level_sensor_output.num_valid_readings++;
                }else{
                 break;   
                }
                
            }else{
                // Otherwise increase the count of invalid readings
                level_sensor_output.num_invalid_readings++;
            }
        }
      }
      token = strtok(NULL, "R"); // This "NULL" says to continue where you left off last time.
   }
    // Calculate median level reading
    level_sensor_output.level_reading = find_median(level_sensor_output.all_level_readings,level_sensor_output.num_valid_readings);
    
    return level_sensor_output;
}

// Level sensor test
test_t level_sensor_test(){
    
    test_t test; // test_t is a new data type we defined in test.h. We then use that data type to define a structure variable test
    test.status = 0; // set test status to zero
    snprintf(test.test_name,sizeof(test.test_name),"TEST_LEVEL_SENSOR");

    // level_sensor_t is a new data type we defined in level_sensor.h. We then use that data type to define a structure variable sensor
    level_sensor_t sensor = level_sensor_take_reading();
    
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


// function to calculate the median of the array, after array is sorted
uint16 find_median(int16 array[] , uint8 n)
{
    int16 median=0;
    sort(array,n);
    
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
void sort(int16 a[],int16 n) { 
   int16 i,j;

   for(i = 0;i < n-1;i++) {
      for(j = 0;j < n-i-1;j++) {
         if(a[j] > a[j+1])
            swap(&a[j],&a[j+1]); // swap the address of a[j] and a[j+1]
      }
   }
}

// function to swap elements in the level readings array in order to sort them
void swap(int16 *p,int16 *q) {
   int16 t;
   
   t=*p; 
   *p=*q; 
   *q=t;
}


/* === Main Loop === */

/*
void run_ultrasonic(const sensor_t *config) {

    float average = 0;
    uint8_t n = N_READINGS;
    for (uint8_t i = 0; i < n; ++i) {
      float reading = level_sensor_take_reading(config);
      if (isnan(reading)) continue;
      average += reading;
      if (!TAKE_AVERAGE) n = 1;
    }
    average /= n;
  
}*/

