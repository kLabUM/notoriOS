//Methods for Telit modem 
#include "modem.h"
#include "notoriOS.h"

void modem_intilize(){
    //don't do much except make sure that all the modem pins are pulled low
    //also make sure that the TX PIN is fully disabled
    power_down();
    
}

void power_down(){
    Rx_Telit_SetDriveMode(PIN_DM_DIG_HIZ);
    Telit_ControlReg_Write(1u);
}

//this puts all the modem pints into a state that won't leak power
//pleasde call restore_pins() to bring them abck to function when they are needed for UART
void power_up(){
   
   //set drive modes
   Telit_ControlReg_Write(0u);
   Rx_Telit_SetDriveMode(PIN_DM_STRONG);

   //pull down all of these, just in case
   Pin_Telit_pwr_Write(OFF);
   Rx_Telit_Write(OFF); 
   Tx_Telit_Write(OFF);
   Pin_Telit_ONOFF_Write(OFF);
   Pin_Telit_SWRDY_Write(OFF);
    
   
}

