//Methods for Telit modem 
#include "modem.h"
#include "notoriOS.h"
#include "debug.h"

char uart_received_string[1024];
uint8_t uart_string_index=0;

#define DEFAULT_AT_TIMEOUT 2000u
#define NO_AT_TIMEOUT 0u

#define TIMEOUT_NETWORK_CONNECT 200 //seconds
#define TIMEOUT_IP_ACQUIRE 230 //seconds (cummulatinve) add to TIMEOUT_NETWORK_CONNECT

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
    modem_state = 0;
    //initilize states
    modem_info.imei[0] = '\0';
    modem_info.model_id[0] = '\0';
    modem_info.sim_id[0] = '\0';
    modem_stats.sq = 0;
    modem_stats.rssi = 0;
    modem_stats.time_to_acquire_ip = 0;
    modem_stats.time_to_network_connect = 0;
    //disable the RX Pin interrupt, and only enable it during long timeouts
    Rx_Telit_SetInterruptMode(Rx_Telit_0_INTR,Rx_Telit_INTR_NONE);
    
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
    CyDelay(100u);//give the MOSFET a few ms to stalbilise power 
}

void modem_power_down(){
    
   at_write_command("AT#SHDN\r","OK",5000u); 
    
   modem_soft_power_cycle();
   CyDelay(1000u);
    
   UART_Telit_Stop();
   isr_telit_rx_Stop();
   pins_configure_inactive();
   modem_state = MODEM_STATE_OFF;
    
}

//this puts all the modem pints into a state that won't leak power
//pleasde call restore_pins() to bring them abck to function when they are needed for UART
uint8 modem_power_up(){

    pins_configure_active();//turns on power and configures all UART pin modes
    UART_Telit_Start();
    isr_telit_rx_StartEx(isr_telit_rx);

    modem_soft_power_cycle();
    
    //turn off ECHO command
    uint8 at_ready = 0;
    uint8 attemps = 0;
    
    for(uint8 attemps = 0; attemps<3; attemps++){
        if(at_write_command("ATE0\r","OK",DEFAULT_AT_TIMEOUT)){
            at_ready = 1;
            break;
        }else{
            modem_soft_power_cycle();
        }
    }
    
    if(at_ready == 1){
        printNotif(NOTIF_TYPE_EVENT,"Modem ready for AT commands after %d attempt(s).",attemps);
        modem_state = MODEM_STATE_STARTUP;
    }else{
        printNotif(NOTIF_TYPE_ERROR,"No response from modem.");
        modem_state = MODEM_STATE_OFF;
        modem_power_down();
        return 0u;
    }
    return 1u;
}

void modem_soft_power_cycle(){
    Pin_Telit_ONOFF_Write(1u);
    CyDelay(2000u);//give modem a sec to boot
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
        
        //if no timeout, no need to wait for response
        if(timeout == NO_AT_TIMEOUT){
            printNotif(NOTIF_TYPE_EVENT,"Modem sent AT command without timeout: %s\r\n", commands);
            return 1u;
        }
      
     
        for(i=0;i<interval;i++){ 
            CyDelay((uint32) delay);
            compare_location=strstr(uart_received_string,expected_response);        
            if(compare_location!=NULL) {
                if (at_attempt == 0){
                   // printNotif(NOTIF_TYPE_EVENT,"Modem Received expected AT Response on first Try: %s\r\n", uart_received_string);
                }else{
                   //printNotif(NOTIF_TYPE_WARNING,"Modem required %d tries to receive AT Response: %s\r\n", (at_attempt+1), uart_received_string);
                 
                }
                 return(1);
            }
            
        }   
      
    }
   // printNotif(NOTIF_TYPE_ERROR,"Modem AT response timeout: %s\r\n", commands);
          
    return(0);    
}

test_t modem_test(){
    
    test_t test;
    modem_power_up();
    //tbd 
    modem_power_down();
    
    
    return test;
}


uint8 extract_string(char* from, const char* beginMarker, const char* endMarker,  char* target){
    
        char *a, *b;
  
        // Expect the UART to contain something like "+CSQ: 17,99\r\n\r\nOK"
        // - Search for "+CSQ: ".  Place the starting pointer, a, at the resulting index
        a = strstr(from,beginMarker);
        if (a == NULL) {
            //puts("+CSQ: not found in uart_received_string");
            a = from;
        }
        
        a += strlen(beginMarker);
        
        // - Find the end marking and offset by 1
        b = strstr(a,endMarker);
        if (b == NULL) {
            //puts("Comma not found in uart_received_string");
            b=from+strlen(from);
        }        
        
        // Parse the strings and store the respective rssi and fer values
        strncpy(target,a,b-a);
        target[b-a] = '\0';
        return 1u;
        
}


uint8 modem_process_tasks(){
    
    if(modem_state == MODEM_STATE_STARTUP){
        
        modem_start_time_stamp = getTimeStamp();
        modem_configure_settings();
        modem_state = MODEM_STATE_WAITING_FOR_NETWORK;
        return 1u;//keep going, don't hand off to sleep yet
        
    }
    else if(modem_state == MODEM_STATE_WAITING_FOR_NETWORK){
        
        if(is_connected_to_cell_network()){
             modem_stats.time_to_network_connect = (int)(getTimeStamp() - (int32)modem_start_time_stamp);
             printNotif(NOTIF_TYPE_EVENT,"Time to connecto to network: %d seconds",
                                            modem_stats.time_to_network_connect);
            get_cell_network_stats();
            set_up_internet_connection();
            modem_state = MODEM_STATE_WAITING_FOR_IP;
        }
        //power off if we times out shold only take ~30 secs to connect to network
        if((int)(getTimeStamp()-(int32)modem_start_time_stamp) > TIMEOUT_NETWORK_CONNECT){
            modem_state = MODEM_STATE_OFF;
            modem_power_down();
            printNotif(NOTIF_TYPE_ERROR,"Modem timed out on network connect");
            
        }
        
        return 0u;//ok to hand off to sleep

    }else if(modem_state == MODEM_STATE_WAITING_FOR_IP){
        if(is_connected_to_internet()){
            modem_state = MODEM_STATE_READY;
            modem_stats.time_to_acquire_ip = (int)(getTimeStamp() - (int32)modem_start_time_stamp);
             printNotif(NOTIF_TYPE_EVENT,"Acquired IP address: %d seconds",
                                            modem_stats.time_to_acquire_ip);
        }
        
        //power off if we times out shold only take ~30 secs to connect to network
        if((int)(getTimeStamp()-(int32)modem_start_time_stamp) > TIMEOUT_IP_ACQUIRE){
            modem_state = MODEM_STATE_OFF;
            modem_power_down();
            printNotif(NOTIF_TYPE_ERROR,"Modem timed out on IP address acquire.");
      
        }
        return 0u;//ok to hand off to sleep
    }
    
    return 0u;
    
}

uint8 is_connected_to_cell_network(){
    //check network status -- need '0,0' response to continue
    at_write_command("AT+CREG?\r", "OK",DEFAULT_AT_TIMEOUT);
    
    char creg[10];
    extract_string(uart_received_string,": ","\r",creg);
    printNotif(NOTIF_TYPE_EVENT,"Registered to network, CREG: %s",creg);
   
    if(strstr(creg,"0,1")!=NULL){
        //time_network_connect = getTimeStamp();
        return 1u;
    }
    
    return 0u;
        
}

void set_up_internet_connection(){
    
    //port 3
   //<connId>,<srMode>,<dataMode>, 
    //at_write_command("AT#SCFGEXT=1,1,0,0,0,0\r", "OK",DEFAULT_AT_TIMEOUT);////LTE modem relevant
    
    //<connId>,<cid>,<pktSz>,<maxTo>,<connTo>,<txTo>
    //at_write_command("AT#SCFG=1,3,300,90,600,50\r", "OK",DEFAULT_AT_TIMEOUT); //LTE modem relevant
    // <cid>,<stat>
   
    at_write_command("AT#SGACT=1,1\r\n", "OK",NO_AT_TIMEOUT);

    

    
}

uint8 is_connected_to_internet(){
    
    //check for 3,1 if verizon
    if(at_write_command("AT#SGACT?\r", "1,1",DEFAULT_AT_TIMEOUT)){
        return 1u;
    }
 
    return 0u;
    
}

void get_cell_network_stats(){
    //gets RSSI and FER values
    for(uint8 attempts =0; attempts <10; attempts++){
        at_write_command("AT+CSQ\r", "OK",DEFAULT_AT_TIMEOUT);
        
        char csq[10];
        extract_string(uart_received_string,": ","\r",csq);
        printNotif(NOTIF_TYPE_EVENT,"Network STts: %s",csq);
        
        char *token;
        token = strtok(csq,",");
        if(token != NULL){
            modem_stats.rssi = atoi(token);
        }
        token = strtok(NULL,",");
        if(token != NULL){
            modem_stats.sq = atoi(token);
        }
        if(modem_stats.rssi != 99 || modem_stats.rssi !=0){
            break;
        }
    }
}
    
    

void modem_configure_settings(){
    
    //get modem model number
    if(modem_info.model_id[0] == '\0'){
        if(at_write_command("AT#CGMM\r","OK",DEFAULT_AT_TIMEOUT)){
            extract_string(uart_received_string,": ","\r",modem_info.model_id);
            printNotif(NOTIF_TYPE_EVENT,"Modem Model: %s",modem_info.model_id);
        }
    }
    
   //get SIM-card ID
   if(modem_info.sim_id[0] == '\0'){
        if(at_write_command("AT#CCID\r", "OK", DEFAULT_AT_TIMEOUT)){
            extract_string(uart_received_string,": ","\r",modem_info.sim_id);
            printNotif(NOTIF_TYPE_EVENT,"SIM ID: %s",modem_info.sim_id);
        }
   }

    //getm modem IMEI
    if(modem_info.imei[0] == '\0'){
        if(at_write_command("AT+CGSN\r","OK",DEFAULT_AT_TIMEOUT)){
            extract_string(uart_received_string,": ","\r",modem_info.imei);
            printNotif(NOTIF_TYPE_EVENT,"IMEI: %s",modem_info.imei);
        } 
    }
    
    //configfure protocol and cellular end point
    at_write_command("AT+CGDCONT=1,\"IP\",\"wireless.twilio.com\"\r\n", "OK", DEFAULT_AT_TIMEOUT);
    
    //Check if modem power savings is enabled. If so, disable it so we can send data.
    if(at_write_command("AT+CFUN?\r", "OK",DEFAULT_AT_TIMEOUT)){
        char CFUN[10];
        extract_string(uart_received_string,": ","\r",CFUN);
        printNotif(NOTIF_TYPE_EVENT,"CFUN: %s",CFUN);
        if(strstr(CFUN,"1") == NULL){
            at_write_command("AT+CFUN=1\r", "OK",DEFAULT_AT_TIMEOUT);
        }
    }

    
}



uint8 modem_get_state(){
    return modem_state;
}

void modem_sleep(){
    if(modem_get_state() != MODEM_STATE_OFF){
        UART_Telit_Sleep();
        //set interrupt on RX pin to wakeup chip from sleep if UART comm starts
       // Rx_Telit_SetInterruptMode(Rx_Telit_0_INTR,Rx_Telit_INTR_FALLING);
     }
}

void modem_wakeup(){
    
     if(modem_get_state() != MODEM_STATE_OFF){
        UART_Telit_Wakeup();
        //remove the need for this pin itterupt
        //Rx_Telit_SetInterruptMode(Rx_Telit_0_INTR,Rx_Telit_INTR_NONE);
        //if woken up from pin, go ahead and delay for 100ms to allow modem UART to deliver the message
        if(modem_get_state() == MODEM_STATE_WAITING_FOR_IP){
            CyDelay(100u);
            uint8 i; 
            i++;
        }
        
     }
    
}

    

    











