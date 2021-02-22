#include "sontek.h"

//Takes sontek measurements
sontek_t sontek_take_reading(){
    
     // sontek_t is a new data type we defined in sontek.h. We then use that data type to define a structure variable sontek_output.
    sontek_t sontek_output;
    sontek_output.valid_readings = 0; //to be determined/changed later what this means
    
    
    uint8 sontek_nvars = 26; //number of parameters the sontek sensor outputs
    char* sontek_address = "0"; //SDI12 address of sensor
    //parameter names
    char* sontek_labels[] = {"Q","stage","v_mean","total_vol","depth","v_index","x_area","T","stat","v_x","v_z","v_x_left","v_x_right","v_bat","pitch","roll","pct_sub","ice","range","depth_adj","total_vol_pos","total_vol_neg","end_cell","snr1","snr2","snr3","snr4"};
    //initialize values to be able to tell if/what parameters are not returned
    float sontek_values[] = {-1.0, -2.0, -3.0, -4.0, -5.0, -6.0, -7.0, -8.0, -9.0, -10.0, -11.0, -12.0, -13.0, -14.0, -15.0, -16.0, -17.0, -18.0, -19.0, -20.0, -21.0, -22.0, -23.0, -24.0, -25.0, -26.0, -27.0};
    //create SDI12_sensor struct of sontek specific values
    SDI12_sensor sontek = {
        .nvars = sontek_nvars,
        .address = sontek_address,
        .labels = sontek_labels,
        .values = sontek_values
    };
    
    //Start UART connection and power up sensor
    SDI12_start();
    SDI12_Power_Write(1u);
    
    uint8 valid = 0; // Reinitialize sensor status
    
    //Will try up to 10 times (1/second) to connect to sontek sensor
    for (uint8 i = 0; i < 10; i++) {
        CyDelay(1000u);
            
        //Checks if sontek sensor is on    
        if (SDI12_is_active(&sontek)){
            //valid = SDI12_change_address(&sensors[0],"0");  -----comment out copied code
            valid = SDI12_info(&sontek); CyDelay(500u); // ------ I don't know what this does/it's purpose 
            //Trys to take a measurment
            if (SDI12_take_measurement(&sontek)) {
                valid = 1; // Successfully took a measurement
                break;                    
            } 
            else {
                valid = -2; // SDI12 sensor powered on, but unable to parse response
            } 
        }
        
        else {
            valid = -1; // SDI12 sensor not responding. It may be powered off or have a different address
        }
    }
            
    //shuts off sensor and turns of UART
    SDI12_Power_Write(0u);
    SDI12_stop();
    
    // this isn't set to any of the measurements yet
    return sontek_output;
}
    
    

/* [] END OF FILE */
