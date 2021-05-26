#include "voltages.h"
#include "notoriOS.h"
#include <math.h>

// Function to take voltage readings and save them in the voltage_t datatype
voltage_t voltage_take_readings(){
    
    voltage_t voltage;  // Create variable voltage of data structure voltage_t.
     
	Battery_Voltage_Enable_Write(ON);   // Flip on the Battery Voltage ADC pin high (turns it on).
    Pressure_Voltage_Enable_Write(ON);  // Flip on the Pressure Transducer Voltage ADC pin high (turns it on).
    
	CyDelay(10u);	    // 10 seconds delay to give time to flip on ADC pin.
    
    AMux_Start();       // Start the Analog Multiplexer
 
    ADC_RestoreConfig();// Have to call this and save (See below). Otherwise ADC won't work through sleep mode
    
    ADC_Start();        // Start the ADC
    
    float channels[AMux_CHANNELS];
    
    for(uint8 c = 0; c< AMux_CHANNELS + 1; c++) // Sweep the MUX Channels
    {
        
        int32 readings[N_SAMPLES];  // Creates new int32 array called readings of size N_SAMPLES = 11
        
        AMux_Select(c); // This functions first disconnects all channels then connects the given channel.
        
        for(uint16 i=0; i< N_SAMPLES; i++){
            
            readings[i] = ADC_Read32(); // When called, it will start ADC conversions, wait for the conversion to be complete, stop ADC conversion and return the result.
        }
        
        // Converts the ADC output to Volts as a floating point number. 
        // Get the median of readings and return that.
        channels[c] = ADC_CountsTo_Volts(find_median32(readings,N_SAMPLES));    // Get median of readings and return that  
    }
    
    AMux_Stop();        // Disconnects all Analog Multiplex channels.
    
    ADC_SaveConfig();   // Save the register configuration which are not retention.
    
    ADC_Stop();         // Stops and powers down the ADC component and the internal clock if the external clock is not selected.
    
    Battery_Voltage_Enable_Write(OFF);  // Pulls Battery ADC pin low (turns it off).
    Pressure_Voltage_Enable_Write(OFF);  // Pulls Pressure Transducer ADC pin low (turns it off).
    
    float offset = channels[0] - 1.024; // Should be 1.024 exactly. BK saw an offset when measuring voltages, did this as a hack to fix the issue for now.
    voltage.voltage_battery = (channels[ADC_MUX_VBAT] * 11) - offset; // Voltage divider is (1/10) ratio, so multiply by 11
    voltage.voltage_solar = channels[ADC_MUX_VSOL] - offset; // Just want voltage here
    // Voltage across sensor resistor gives 100mV drop for max current, which is 800mA
    //voltage.voltage_charge_current = 0.1*(channels[ADC_MUX_CHRG] - voltage.voltage_battery);
    voltage.voltage_pressure = channels[ADC_MUX_PRTRANS]; // Pressure transducer reading

    voltage.valid = 1;
    
    return voltage;
}

// Function to take Pressure sensor readings and save them in the pressure_t datatype
pressure_t pressure_sensor_readings(){
    
    pressure_t pressure;  // Create variable voltage of data structure voltage_t.
    float channels[AMux_CHANNELS];
    int32 readings[N_SAMPLES];  // Creates new int32 array called readings of size N_SAMPLES = 11
    
    AMux_Start();       // Start the Analog Multiplexer
 
    ADC_RestoreConfig();// Have to call this and save (See below). Otherwise ADC won't work through sleep mode
    
    ADC_Start();        // Start the ADC
    
    AMux_Select(ADC_MUX_PRTRANS); // This functions first disconnects all channels then connects the given channel.
    
    for(uint16 i=0; i< N_SAMPLES; i++){   
        readings[i] = ADC_Read32(); // When called, it will start ADC conversions, wait for the conversion to be complete, stop ADC conversion and return the result.
    }
    
    // Converts the ADC output to Volts as a floating point number. 
    // Get the median of readings and return that.
    pressure.pressure_voltage_off = ADC_CountsTo_Volts(find_median32(readings,N_SAMPLES));    // Get median of readings and return that
    pressure.pressure_voltage_off = floorf(pressure.pressure_voltage_off * 100) / 100;
    
    Pressure_Voltage_Enable_Write(ON);  // Flip on the Pressure Transducer Voltage ADC pin high (turns it on).
    
	CyDelay(10u);	    // 10 seconds delay to give time to flip on ADC pin.
  
    for(uint16 i=0; i< N_SAMPLES; i++){   
        readings[i] = ADC_Read32(); // When called, it will start ADC conversions, wait for the conversion to be complete, stop ADC conversion and return the result.
    }
    
    // Converts the ADC output to Volts as a floating point number. 
    // Get the median of readings and return that.
    pressure.pressure_voltage_on = ADC_CountsTo_Volts(find_median32(readings,N_SAMPLES));    // Get median of readings and return that
    
    AMux_Stop();        // Disconnects all Analog Multiplex channels.
    
    ADC_SaveConfig();   // Save the register configuration which are not retention.
    
    ADC_Stop();         // Stops and powers down the ADC component and the internal clock if the external clock is not selected.
    
    Pressure_Voltage_Enable_Write(OFF);  // Pulls Pressure Transducer ADC pin low (turns it off).
    // Calculate the pressure transducer current output.
    // i=V/R where i= currrent, V= voltage, and R= resistance= 150 ohms. Divide by 1000 to get current in mA.
    // Should fall between 4 and 20mA
    pressure.pressure_current = (pressure.pressure_voltage_on/ 150.0)*1000; 
    // Calculate the estimated depth from the pressure transducer.
    // d = (Dmax*V/0.016R) - (Dmax/4) where d= depth, V= voltage, R= resistance= 150 ohms, Dmax = sensor max depth = 10 ft.
    // Should fall between 0 and 10 ft
    pressure.pressure_depth = ((10.0*pressure.pressure_voltage_on)/(0.016*150))-(10.0/4.0);   
    return pressure;
}

// Function to calculate the pressure transducer current and depth estimate.
pressure_t pressure_calculations(voltage_t voltage){
    // Create variable pressure of data structure type pressure_t.
    pressure_t pressure;
    
    // Calculate the pressure transducer current output.
    // i=V/R where i= currrent, V= voltage, and R= resistance= 150 ohms. Divide by 1000 to get current in mA.
    // Should fall between 4 and 20mA
    pressure.pressure_current = (voltage.voltage_pressure/ 150.0)/1000; 
    // Calculate the estimated depth from the pressure transducer.
    // d = (Dmax*V/0.016R) - (Dmax/4) where d= depth, V= voltage, R= resistance= 150 ohms, Dmax = sensor max depth = 10 ft.
    // Should fall between 0 and 10 ft
    pressure.pressure_depth = ((10.0*voltage.voltage_pressure)/(0.016*150))-(10.0/4.0); 
    return pressure;
}
    
    

// Function to sort the elements in the level readings array
void sort32(int32 a[],int32 n) { 
   int32 i,j;

   for(i = 0;i < n-1;i++) {
      for(j = 0;j < n-i-1;j++) {
         if(a[j] > a[j+1])
            swap32(&a[j],&a[j+1]);  // Swap the address of a[j] and a[j+1]
      }
   }
}

// Function to swap elements in the level readings array in order to sort them
void swap32(int32 *p,int32 *q){
   int32 t;
   
   t=*p; 
   *p=*q; 
   *q=t;
}

// Function to calculate the median of the array, after array is sorted
int32 find_median32(int32 array[] , int32 n)
{
    int32 median=0;
    sort32(array,n);
    
    // If number of elements are even
    if(n%2 == 0)
        // Median is the average of the  two middle sorted numbers
        median = (array[(n-1)/2] + array[n/2])/2.0;
    // If number of elements are odd
    else
        // Median is the the middle sorted number. 
        median = array[n/2];
    
    return median;
}

// Test the voltage readings
test_t voltages_test(){
    // Create variable test of the data structure test_t
    test_t test;
    test.status = 0;
    snprintf(test.test_name,sizeof(test.test_name),"TEST_ANALOG_VOLTAGES");
    
    voltage_t voltage = voltage_take_readings();
    
    //do some checks here if you want the test to meet some voltage requirement
    
    snprintf(test.reason,sizeof(test.reason),"VBAT=%.3f,VSOL=%.3f,CHRG=%.3f,PRESSURE=%.3f",
            voltage.voltage_battery,
            voltage.voltage_solar,
            voltage.voltage_charge_current,
            voltage.voltage_pressure);
    
    test.status = 1;
    
    return test;
}

// Test the pressure sensor readings
test_t pressure_sensor_test(){
    // Create variable test of the data structure test_t
    test_t test;
    test.status = 0;
    snprintf(test.test_name,sizeof(test.test_name),"TEST_PRESSURE_SENSOR");
    
    pressure_t pressure = pressure_sensor_readings();
    
    //do some checks here if you want the test to meet some voltage requirement
    
    snprintf(test.reason,sizeof(test.reason),"PRESSURE=%.3f,PRES_CURRENT=%.3f,PRES_DEPTH=%.3f",
            pressure.pressure_voltage_on,
            pressure.pressure_current,
            pressure.pressure_depth);
    pressure.pressure_voltage_on = floorf(pressure.pressure_voltage_on * 100) / 100;
    if (pressure.pressure_voltage_off != pressure.pressure_voltage_on) {
        test.status = 1;
    }
    
    return test;
}



