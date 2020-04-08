//Methods for Telit modem 
#include "modem.h"
#include "notoriOS.h"
#include "debug.h"

char uart_received_string[1024];
uint8_t uart_string_index=0;

CY_ISR_PROTO(isr_telit_rx);

CY_ISR(isr_telit_rx){      
    while(UART_Telit_GetRxBufferSize()==1){
        uart_received_string[uart_string_index]=UART_Telit_GetChar();
        uart_string_index++;
    }       
}


void modem_intilize(){
    //don't do much except make sure that all the modem pins are pulled low
    //also make sure that the TX PIN is fully disabled
    pins_configure_inactive();
}


//to be used when the modem power is to be "killed" conpletely to conserve power
void pins_configure_inactive(){
   
    //physically disconect UART pin
   Telit_ControlReg_Write(0u);
   //set drive mode
   Rx_Telit_SetDriveMode(PIN_DM_STRONG);

   //kill power to modem
   Pin_Telit_pwr_Write(OFF);

   //pull down all of these, just in case
   Rx_Telit_Write(OFF); 
   Tx_Telit_Write(OFF);
   Pin_Telit_ONOFF_Write(OFF);
   Pin_Telit_SWRDY_Write(OFF);
    
}

void pins_configure_active(){
    //powerup
    Rx_Telit_SetDriveMode(PIN_DM_DIG_HIZ);
    Telit_ControlReg_Write(1u);
    Pin_Telit_pwr_Write(ON);
    CyDelay(1000u);//give modem a sec to power up and rails to stabilize
}

void modem_power_down(){
    
   modem_soft_power_cycle();
   CyDelay(5000u);
    
   UART_Telit_Stop();
   isr_telit_rx_Stop();
   pins_configure_inactive();
    
}

//this puts all the modem pints into a state that won't leak power
//pleasde call restore_pins() to bring them abck to function when they are needed for UART
void modem_power_up(){

    pins_configure_active();//turns on power and configures all UART pin modes
    UART_Telit_Start();
    isr_telit_rx_StartEx(isr_telit_rx);

    modem_soft_power_cycle();
    
}

void modem_soft_power_cycle(){
    Pin_Telit_ONOFF_Write(1u);
    CyDelay(2500u);
    Pin_Telit_ONOFF_Write(0u);
    
}


void uart_string_reset(){
    uart_string_index=0;
    memset(uart_received_string,'\0',sizeof(uart_received_string));
    UART_Telit_ClearRxBuffer();
}

uint8_t at_write_command(char* commands, char* expected_response,uint32_t timeout){

    uint32 at_attempt = 0;
    
    for(at_attempt = 0; at_attempt < 3; at_attempt++) {
        
        char* compare_location;
        uint32_t delay=10;
        uint32_t i, interval=timeout/delay;        
        
        uart_string_reset();
        UART_Telit_PutString(commands);
      
       
        for(i=0;i<interval;i++){        
            compare_location=strstr(uart_received_string,expected_response);        
            if(compare_location!=NULL) {
                if (at_attempt == 0){
                    printNotif(NOTIF_TYPE_EVENT,"Modem Received expected AT Response on first Try: %s\r\n", uart_received_string);
                }else{
                   printNotif(NOTIF_TYPE_WARNING,"Modem required %d tries to receive AT Response: %s\r\n", (at_attempt+1), uart_received_string);
                 
                }
                 return(1);
            }
            CyDelay((uint32) delay);
        }   
        CyDelay((uint32) delay);
    }
    printNotif(NOTIF_TYPE_ERROR,"Modem AT response timeout: %s\r\n", commands);
          
    return(0);    
}

test_t modem_test(){
    
    test_t test;
    modem_power_up();
    at_write_command("AT\r","OK",2000u);
    
    
    modem_power_down();
    
    return test;
}


