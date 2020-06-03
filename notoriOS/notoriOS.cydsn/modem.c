//Methods for Telit modem 
#include "modem.h"
#include "notoriOS.h"
#include "debug.h"
#include <math.h>
#include <stdio.h>

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
    long boot_time = getTimeStamp();
    
    //it takes the 4G modem 10+secs (max 20) to boot up, so keep hitting it with AT commands until it responds
    for(uint8 attempts = 0; attempts<3; attempts++){
        
        for(uint8 pwr_check = 0; pwr_check<20; pwr_check++){
            if(at_write_command("ATE0\r","OK",1000u)){
                at_ready = 1;
                break;
            }
        }
            
        if(at_ready == 1){
            break;
        }else{
            modem_soft_power_cycle();
        }
    }
    
    boot_time = getTimeStamp() - boot_time;
    printNotif(NOTIF_TYPE_EVENT,"Modem booot time: %d",boot_time);
    
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
                   printNotif(NOTIF_TYPE_EVENT,"Modem Received expected AT Response on first Try:\nCommand: %s\nReceived: %s\r\n", commands,uart_received_string);
                }else{
                   printNotif(NOTIF_TYPE_WARNING,"Modem required %d tries to receive AT Response: %s\r\n", (at_attempt+1), uart_received_string);
                 
                }
                 return(1);
            }
            
        }  
        
        //somehitng went wrong, restart UARt
        printNotif(NOTIF_TYPE_ERROR,"Could not write AT command: %s\n Received: %s\n", commands,uart_received_string);
      
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
    printNotif(NOTIF_TYPE_EVENT,"Checking AT+CREG?");
    uint8 status = at_write_command("AT+CREG?\r", "OK",DEFAULT_AT_TIMEOUT);
    
    printNotif(NOTIF_TYPE_EVENT,"Extracting string AT+CREG?");
    printNotif(NOTIF_TYPE_EVENT,"uart_received_string: %s",uart_received_string);
    
    if(status){
        char creg[10];
        
        extract_string(uart_received_string,": ","\r",creg);
        printNotif(NOTIF_TYPE_EVENT,"Registered to network, CREG: %s",creg);
        printNotif(NOTIF_TYPE_EVENT,"Done extracing string AT+CREG?");
       
        if(strstr(creg,"0,1")!=NULL){
            //time_network_connect = getTimeStamp();
            return 1u;
        }
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
   
    at_write_command("AT#SGACT=1,1\r\n", "OK",1000u);

    

    
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

//returns 0 if modem can't get time
long modem_get_network_time(){
    
    //modem reposen format (careful to include the quotes "")
    //+CCLK: "20/05/01,10:48:33-16" the "-16" is the number of quarter hourts (15 mins) from GMT time .. int this case subtract 4 hrs
    //ex: 14:48 GMT

    if(modem_state != MODEM_STATE_READY){
        printNotif(NOTIF_TYPE_ERROR,"Modem not ready to get time. Not conncted to network.");
        return 0;   
    }
    long epoch_time = 0;
    
    if(at_write_command("AT+CCLK?\r","OK",DEFAULT_AT_TIMEOUT)){
        char network_time[30];
        extract_string(uart_received_string,": \"","\"\r",network_time);
        
        ///////////////////  
        struct tm t;
        
        int year, month, day, hour, minute, second, gmt_offset = 0;
      
        if(sscanf(network_time,"%d/%d/%d,%d:%d:%d-%d",&year,&month,&day,&hour,&minute,&second,&gmt_offset) == 7){
            gmt_offset = gmt_offset*(900);
        }else if(sscanf(network_time,"%d/%d/%d,%d:%d:%d+%d",&year,&month,&day,&hour,&minute,&second,&gmt_offset) == 7){
            gmt_offset = gmt_offset*(-900);
        }else{
            printNotif(NOTIF_TYPE_ERROR,"Could not parse modem time: %s.",network_time);
            return 0;   
        }
        
        t.tm_sec = second;
        t.tm_min = minute;
        t.tm_hour = hour;
        t.tm_year = year + 100;    // Year since 1900
        t.tm_mon = month-1;        // Month, where 0 = jan
        t.tm_mday = day;           // Day of the month
        t.tm_isdst = 0;            // Is Daylight Savings on? 1 = yes, 0 = no, -1 = unknown
    
        epoch_time = mktime(&t) + gmt_offset;
    }

    
    return epoch_time;
}




uint8 modem_get_state(){
    return modem_state;
}

void modem_sleep(){
    if(modem_get_state() != MODEM_STATE_OFF){

        Telit_ControlReg_Sleep();
        UART_Telit_Sleep();
        
        //set interrupt on RX pin to wakeup chip from sleep if UART comm starts
       // Rx_Telit_SetInterruptMode(Rx_Telit_0_INTR,Rx_Telit_INTR_FALLING);
     }
}

void modem_wakeup(){
     if(modem_get_state() != MODEM_STATE_OFF){
        Telit_ControlReg_Wakeup();
        UART_Telit_Wakeup();
        //remove the need for this pin itterupt
        //Rx_Telit_SetInterruptMode(Rx_Telit_0_INTR,Rx_Telit_INTR_NONE);
        //if woken up from pin, go ahead and delay for 100ms to allow modem UART to deliver the message
        if(modem_get_state() == MODEM_STATE_WAITING_FOR_IP){
            CyDelay(100u);
        }
        
     }
    
}


    
//retunrs lat,lom,alt data from Assited GPS (AGPS) system
gps_t modem_get_gps_coordinates(){
    gps_t gps;
    gps.altitude = 0;
    gps.longitude = 0;
    gps.latitude = 0;
    gps.time_to_lock =0;
    gps.valid = 0;
    
    //AGPS will only work if we're connected to network, so don't call this fuction otherwise
    if(modem_state != MODEM_STATE_READY){
        return gps;//where valid = 0
    }
    
    long lock_time = getTimeStamp();
    
    //Eenable this for thr LTER module
    //enable GPS Low-Dropout Regulator
    at_write_command("AT#GPIO=5,1,1,0\r","OK",DEFAULT_AT_TIMEOUT);
    
    //configure SUPL connection params
    at_write_command("AT$SUPLSEC=1\r","OK",DEFAULT_AT_TIMEOUT);
    
    //use SUPL2.0
    at_write_command("AT$SUPLV=2\r","OK",DEFAULT_AT_TIMEOUT);
    
    //enable AGPS service
    at_write_command("AT$AGPSEN=1\r","OK",DEFAULT_AT_TIMEOUT);
    
    //chose SUPL server, use google publiuc service
    at_write_command("AT$SLP=1,\"supl.google.com:7275\"\r","OK",DEFAULT_AT_TIMEOUT);
    
    //pdp context has to be lockeed to carrier for this to work
    
    //unlcok context since it needs to be used by modem
    at_write_command("AT#SGACT=1,0\r\n", "OK",DEFAULT_AT_TIMEOUT);
    
    at_write_command("AT$LCSLK=1,1\r","OK",DEFAULT_AT_TIMEOUT);//ATT
    //at_write_command("AT$LCSLK=1,3\r","OK",DEFAULT_AT_TIMEOUT);//Verizon
    
    //relock the context
    at_write_command("AT#SGACT=1,1\r\n", "OK",10000u);
    
    //start AGPS service
    at_write_command("AT$GPSSLSR=1,1,,,,,2\r","OK",DEFAULT_AT_TIMEOUT); 
    
    
    //start the AGPS service
    //will keep receiving a buynch of commas without numbers, until GPS locks
    //good lock will retunr a string with "N" and "W" coordinates
    
    
 
    uint8 gps_status = 0;
    for(uint16 i=0;i<200;i++){
        gps_status = at_write_command("AT$GPSACP\r","N",DEFAULT_AT_TIMEOUT);//ATT
        if(gps_status){
            break;   
        }
        CyDelay(1000u);//wait for a second -- BLOCKING. Could be more elegant to incorporate this into the modem state machine
    }
    
    
    //parse data here
    
    //example $GPSACP: 210926.000,4217.8003N,08341.3158W,2.3,258.2,3,17.8,4.8,2.6,220520,05,00
    float lat;
    float lon;
    float alt;
    
    char *s1 = strstr(uart_received_string,"$GPSACP: ");
    int match = sscanf(s1,"$GPSACP: %*f,%fN,%fW,%*f,%f,%*s",&lat,&lon,&alt);
    if(match >= 3){
        lat = floorf(lat/100) + fmod(lat,100)/60;
        lon = -(floorf(lon/100) + fmod(lon,100)/60);
        gps.altitude = alt;
        gps.latitude = lat;
        gps.longitude = lon;
        gps.valid = 1;
    }
  
    
    //disable LDO, just in case
    at_write_command("AT#GPIO=5,0,1,0\r","OK",DEFAULT_AT_TIMEOUT);
    
    //flush modem's GPS data, just in case
    at_write_command("AT$GPSR=0\r","OK",DEFAULT_AT_TIMEOUT);
    
    
    
    
    
    /*relevant to 3g modem, can ignore
    
    //Setthe GPS Quality of Servicep arameters. Inorder,theyare<horiz_accuracy(inmeters)>,<vertical_accuracy(inmeters)>,<response_time(inseconds)>,<age_of_location_info (in seconds)>:
    //AT$GPSQOS=50,50,100,0
    at_write_command("AT$GPSQOS=50,50,100,0\r","OK",DEFAULT_AT_TIMEOUT);
    
    //SettheSUPLLocationPlatform(SLP)addressoftheserverthatwillprovidetheA-GPS data:
    at_write_command("AT$LCSSLP=1,\"supl.google.com\"\r","OK",DEFAULT_AT_TIMEOUT);
    
    //unlock contedt, simnce its being used by modem
    at_write_command("AT#SGACT=1,0\r\n", "OK",DEFAULT_AT_TIMEOUT);
    //Lock context 1 for LoCation Services (LCS) use only
    at_write_command("AT$LCSLK=1,1\r","OK",5000u);
    
    //unlock contedt, simnce its being used by modem
    at_write_command("AT#SGACT=1,1\r\n", "OK",10000u);
    
    //EnableunsolicitedNMEAGPSdataintheformatof<NMEASENTENCE><CR>
    at_write_command("AT$GPSNMUN=2,1,0,0,0,0,0\r","OK",DEFAULT_AT_TIMEOUT);
    
    //StartGPSlocationservicerequest.Sendingthiscommandsets$GPSP=1
   uint8 gps_status = 0;
    for(uint16 i=0;i<200;i++){
        gps_status = at_write_command("AT$GPSSLSR=1,1,,,,,0,255\r","N",3000u);
        if(gps_status){
            break;   
        }
  }
    
    //$GPGGA,030307.691,4217.7923,N,08341.3240,W,1,05,2.25,225.0,M,,M,,0000*6E
    //$GPGGA,213852.000,4600.8755,N,09327.7008,W,1,05,3.00,292.0,M,,M,,0000*6E
    if(gps_status){
        float lat,lon,alt = 0;
        if(sscanf(uart_received_string,"%*s,%f,N,%f,W,%*s",&lat,&lon) > 0){
            gps.altitude = alt;
            gps.latitude = lat/60;
            gps.longitude = lon/60;
            gps.valid = 1;
        }
        
    }
    
    //turn off gps
    //AT$GPSP=0
    at_write_command("AT$GPSP=0\r","OK",DEFAULT_AT_TIMEOUT);
    
    */

    
    lock_time = getTimeStamp() - lock_time;
    gps.time_to_lock = lock_time;
    printNotif(NOTIF_TYPE_EVENT,"Time to lock GPS: %d", gps.time_to_lock);
    
    return gps;
    
}











