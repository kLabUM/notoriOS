#include "level_sensor.h"
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include "level_sensor.h"
#include "notoriOS.h"
#include "project.h"


/* === Driver Functions === */

//sensor realated global calls




/* Circular buffer of recently received chars */
static char uart_chars[257] = {0};
static uint8_t num_chars = 0;

static const char *line = uart_chars;
static const char *match = NULL;
static const char *pattern = NULL;



const char *uart_string(void) {
  return (char*)uart_chars;
}

uint8_t uart_len(void) {
  return num_chars;
}

void uart_clear(void) {
  Level_Sensor_UART_ClearRxBuffer();
  memset((void*)uart_chars, 0, 256);
  num_chars = 0;
}

CY_ISR(Level_Sensor_ISR) {
  /* Store received char in uart_chars */
  char rx_char = Level_Sensor_UART_GetChar();
  if (rx_char) uart_chars[num_chars++] = rx_char;

  /* Check pattern at end of line */
  if (rx_char == '\r') {
    match = strstr(line, pattern);
    line = uart_chars + num_chars;
  }
}


level_sensor_t level_sensor_take_reading(){
    
    
    level_sensor_t level_sensor_output;
    level_sensor_output.num_invalid_readings = 0;
    level_sensor_output.num_valid_readings = 0;
    
    uart_clear();
    
    //start sensor power, uart, and isr
    Level_Sensor_Power_Write(ON);
    CyDelay(200u);//sensor bootup sequence ~200ms -- senosor will spit out ID info, but we don't need that
    Level_Sensor_RX_SetDriveMode(PIN_DM_DIG_HIZ);
    Level_Sensor_UART_Start();
    Level_Sensor_ISR_StartEx(Level_Sensor_ISR);
    
    //wait to take  reading
    //sensor will take ~200ms to send first reading, and 200ms for subsequent readings
    CyDelay(2000u);//take a few seconds to get readings, 2 secs ~10 readings
    
    //stop all sensor processes
    Level_Sensor_UART_Stop();
    Level_Sensor_ISR_Stop();
    Level_Sensor_Power_Write(OFF);
    Level_Sensor_RX_SetDriveMode(PIN_DM_STRONG);
    Level_Sensor_RX_Write(OFF);

    /* Return parsed reading */
    //parse string and put all values into array
    //the sensor will power on and do a few things, after which the readigs will follow with the seqeunce
    //readings are stashed between "R" and "\000"
    char *token;
    token = strtok(uart_chars, "R");
    /* walk through other tokens */
   while( token != NULL ) 
   {
      if(strstr(token, "\r") != NULL){//full reading is delimieted by "R" + reading + "\r"
        uint16 reading = atoi(token);
        if(reading != 0){
            if(reading != READING_INVALID){//invald readings by maxbotix
                level_sensor_output.all_level_readings[level_sensor_output.num_valid_readings] = reading;
                if(level_sensor_output.num_valid_readings < N_READINGS){
                    level_sensor_output.num_valid_readings++;
                }else{
                 break;   
                }
                
            }else{
                level_sensor_output.num_invalid_readings++;
            }
        }
      }
      token = strtok(NULL, "R");
   }
    //calculate median
    level_sensor_output.level_reading = find_median(level_sensor_output.all_level_readings,level_sensor_output.num_valid_readings);
 
    return level_sensor_output;
}


test_t level_sensor_test(){
    test_t test;
    test.status = 0;
    sprintf(test.test_name,"TEST_LEVEL_SENSOR");

    level_sensor_t sensor = level_sensor_take_reading();
    
    sprintf(test.reason,"nWANTED=%d,nRECEIVED=%d,nVALID=%d,nMEDIAN=%d.",
        N_READINGS,
        sensor.num_valid_readings+sensor.num_invalid_readings,
        sensor.num_valid_readings,
        sensor.level_reading
    );
   
    
    if(sensor.num_valid_readings == N_READINGS){//pass
        test.status = 1;
    }else if((sensor.num_valid_readings > 0) && (sensor.num_valid_readings < N_READINGS)){//pass
        test.status = 2;//not bad, since we got some OK readings
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
        median = (array[(n-1)/2] + array[n/2])/2.0;
    // if number of elements are odd
    else
        median = array[n/2];
    
    return median;
}


void sort(int16 a[],int16 n) { 
   int16 i,j;

   for(i = 0;i < n-1;i++) {
      for(j = 0;j < n-i-1;j++) {
         if(a[j] > a[j+1])
            swap(&a[j],&a[j+1]);
      }
   }
}

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

