#include "pressure_transducer.h"
#include "notoriOS.h"
#include <stdio.h>
#include <string.h>



pressure_t pressure_take_readings(){
    
    pressure_t pressure;  // Create variable pressure of data structure pressure_t.
    
	Voltage_Enable_Write(ON);   // Flip on the ADC pin high (turns it on).
    
	CyDelay(10u);	    // 10 seconds delay to give time to flip on ADC pin.
    
    ADC_RestoreConfig();// Have to call this and save (See below). Otherwise ADC won't work through sleep mode
	
	ADC_Start();        // Start the ADC
    
    ADC_StartConvert(); // Forces the ADC to initiate a conversion. If in the "Single Sample" mode, one conversion will be performed then the ADC will halt.
    
    AMux_Start();       // Start the Analog Multiplexer
   
    float channels[AMux_CHANNELS];
    
    int32 readings[N_SAMPLES];  // Creates new int32 array called readings of size N_SAMPLES = 11
        
    AMux_Select(ADC_MUX_PRTRANS); // This functions first disconnects all channels then connects the given channel.
    
    for(uint16 i=0; i< N_SAMPLES; i++){
        readings[i] = ADC_Read32(); // When called, it will start ADC conversions, wait for the conversion to be complete, stop ADC conversion and return the result.
    }
    // Converts the ADC output to Volts as a floating point number. 
    // Get the median of readings and return that.
    
    channels[ADC_MUX_PRTRANS] = ADC_CountsTo_Volts(find_median32(readings, N_SAMPLES));    // Get median of readings and return that
        
    
    AMux_Stop();        // Disconnects all Analog Multiplex channels.
    ADC_StopConvert();  // Forces the ADC to stop all conversions.
    ADC_SaveConfig();   // Save the register configuration which are not retention.
    ADC_Stop();         // Stops and powers down the ADC component and the internal clock if the external clock is not selected.
    
    Voltage_Enable_Write(OFF);  // Pulls ADC pin low (turns it off).
    float offset = channels[0] - 1.024; // Should be 1.024 exactly. BK saw an offset when measuring voltages, did this as a hack to fix the issue for now.
    pressure.pressure_voltage = (channels[ADC_MUX_PRTRANS] * 11) - offset; // Voltage divider is (1/10) ratio, so multiply by 11
    
    pressure.valid = 1;
    
    return pressure;
}

// Test the pressure readings
test_t pressure_test(){
    // Create variable test of the data structure test_t
    test_t test;
    test.status = 0;
    snprintf(test.test_name,sizeof(test.test_name),"TEST_PRESSURE_VOLTAGES");
    
    pressure_t pressure = pressure_take_readings();
    
    snprintf(test.reason,sizeof(test.reason),"Pressure=%.3f",
            pressure.pressure_voltage);
    
    test.status = 1;
        
    return test;
}





