#include "voltages.h"
#include "notoriOS.h"
#include <stdio.h>
#include <string.h>



voltage_t voltage_take_readings(){
    
    voltage_t voltage;
    /* flip on the ADC pin */
	Battery_Voltage_Enable_Write(ON);
	CyDelay(10u);	
    
	/* Start the ADC */
	ADC_Start(); 
    AMux_Start();
    
    
    float channels[AMux_CHANNELS];
    //sweep the MUX Channels
    for(uint8 c = 0; c< AMux_CHANNELS; c++)
    {
        int32 readings[N_SAMPLES];
        AMux_Select(c);
        for(uint8 i=0; i< N_SAMPLES; i++){
            readings[i] = ADC_Read32();
        }
        channels[c] = ADC_CountsTo_Volts(find_median32(readings,N_SAMPLES));//get median of readings and return that
        
    }
    
    AMux_Stop();
    ADC_Stop();
    
    Battery_Voltage_Enable_Write(OFF);
    
    voltage.voltage_battery = channels[ADC_MUX_VBAT] * 11; //voltage divider is (1/10) ratio, so multiply by 11
    voltage.voltage_solar = channels[ADC_MUX_VSOL]; //just want voltage here
    //votage across sense resistor gives 100mV drop for max current, which is 800mA
    //voltage.voltage_charge_current = 0.1*(channels[ADC_MUX_CHRG] - voltage.voltage_battery);
    
    voltage.valid = 1;
    
    
    return voltage;
}


void sort32(int32 a[],int32 n) { 
   int32 i,j;

   for(i = 0;i < n-1;i++) {
      for(j = 0;j < n-i-1;j++) {
         if(a[j] > a[j+1])
            swap32(&a[j],&a[j+1]);
      }
   }
}

void swap32(int32 *p,int32 *q){
   int32 t;
   
   t=*p; 
   *p=*q; 
   *q=t;
}

// function to calculate the median of the array, after array is sorted
int32 find_median32(int32 array[] , int32 n)
{
    int32 median=0;
    sort32(array,n);
    
    // if number of elements are even
    if(n%2 == 0)
        median = (array[(n-1)/2] + array[n/2])/2.0;
    // if number of elements are odd
    else
        median = array[n/2];
    
    return median;
}

test_t voltages_test(){
    
    test_t test;
    test.status = 0;
    sprintf(test.test_name,"TEST_ANALOG_VOLTAGES");
    
    voltage_t voltage = voltage_take_readings();
    
    //do some checks here if you want the test to meet some voltage requrement
    //poipoi
    
    sprintf(test.reason,"VBAT=%.3f,VSOL=%.3f,CHRG=%.3f",
            voltage.voltage_battery,
            voltage.voltage_solar,
            voltage.voltage_charge_current);
    
    test.status = 1;
        
    return test;
}





