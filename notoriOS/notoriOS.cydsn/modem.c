//Methods for Telit modem 
#include "modem.h"
#include "notoriOS.h"
#include "debug.h"
#include <math.h>
#include <stdio.h>

char uart_received_string[1024];
int16 uart_string_index=0;

#define DEFAULT_AT_TIMEOUT 2000u // Send command and wait 2000 ms
#define NO_AT_TIMEOUT 0u // Send command, no timeout/ don't wait 

#define TIMEOUT_NETWORK_CONNECT 200 //seconds
#define TIMEOUT_IP_ACQUIRE 230 //seconds (cummulatinve) add to TIMEOUT_NETWORK_CONNECT

CY_ISR_PROTO(isr_telit_rx); // Declares a custom ISR function "isr_telit_rx" using the CY_ISR_PROTO macro instead of modifying the auto-generated code

// Define the custom ISR function "isr_telit_rx" using the CY_ISR macro
// Get each character from Telit and saves it to uart_received_string
CY_ISR(isr_telit_rx){      
    // UART_Telit_GetRxBufferSize() returns the number of received bytes available in the RX buffer.
    while(UART_Telit_GetRxBufferSize()==1){
        uart_received_string[uart_string_index]=UART_Telit_GetChar();
        uart_string_index++;
    }

}

// Initialize the modem
void modem_intilize(){
    // Don't do much except make sure that all the modem pins are pulled low
    // Also make sure that the TX PIN is fully disabled
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
    // Disable the RX Pin interrupt, and only enable it during long timeouts
    // Rx_Telit_SetInterruptMode(): Configures the interrupt mode for each of the Pins component's pins
    // Rx_Telit_0_INTR selects the first pin in the list, and Rx_Telit_INTR_NONE disables the Rx pin interrupt
    Rx_Telit_SetInterruptMode(Rx_Telit_0_INTR,Rx_Telit_INTR_NONE);
    
}


// To be used when the modem power is to be "killed" conpletely to conserve power
void pins_configure_inactive(){
   
   Telit_ControlReg_Write(0u); // Physically disconect UART pin
   Rx_Telit_SetDriveMode(PIN_DM_STRONG); // Set drive mode

   Pin_Telit_pwr_Write(OFF); // Kill power to modem (pulls pin low).

   // Pull down all of these, just in case (turns the off).
   Rx_Telit_Write(OFF); 
   Tx_Telit_Write(OFF);
   Pin_Telit_ONOFF_Write(OFF);
   Pin_Telit_SWRDY_Write(OFF);
    
}

// Turns on power and configures all UART pin modes
void pins_configure_active(){
    
    Rx_Telit_SetDriveMode(PIN_DM_DIG_HIZ); // Power up. Set RX pin to High Impedance Digital mode.
    Telit_ControlReg_Write(1u); // Pull Telit_ControlReg pin high (turns it on).
    Pin_Telit_pwr_Write(ON); // Pulls Telit_pwr pin high (turns it on).
    CyDelay(100u); // Give the MOSFET a few ms to stalbilise power 
}

// Power down the modem -- completely "kill" modem to conserve power.
void modem_power_down(){
    // AT Command to modem for Software Shut Down - #SHDN
    // \r is carriage return, it is necessary for the modem to know it got a command
    at_write_command("AT#SHDN\r","OK",5000u); 
   
    // Soft power cycle makes sure that the modem is all the way on (like holding on button on for a few seconds)
    modem_soft_power_cycle();
    CyDelay(1000u);
    
    UART_Telit_Stop(); // Disables the UART operation.
    isr_telit_rx_Stop(); // Disables and removes the interrupt.
    pins_configure_inactive(); // We want the modem power to be "killed" completely to conserve power.
    modem_state = MODEM_STATE_OFF;
    
}

// This puts all the modem pints into a state that won't leak power
// Please call restore_pins() to bring them abck to function when they are needed for UART
uint8 modem_power_up(){

    pins_configure_active(); // Turns on power and configures all UART pin modes
    UART_Telit_Start(); // Sets the initVar variable, calls the UART_Telit_Init() function, and then calls the UART_Telit_Enable() function.
    isr_telit_rx_StartEx(isr_telit_rx); // Sets up the interrupt and enables it. 

    modem_soft_power_cycle(); // Soft power cycle makes sure that the modem is all the way on (like holding on button on for a few seconds).
    
    // Turn off ECHO command (doesn't repeat your command back to you).
    uint8 at_ready = 0;
    uint8 attempts = 0;
    // Create variable boot_time of the current time stamp.
    long boot_time = getTimeStamp();
    
    //it takes the 4G modem 10+secs (max 20) to boot up, so keep hitting it with AT commands until it responds
    for(uint8 attempts = 0; attempts<3; attempts++){
        
        for(uint8 pwr_check = 0; pwr_check<20; pwr_check++){
            // AT command ATE0 disables the command echo (doesn't repeat your command back to you).
            if(at_write_command("ATE0\r","OK",1000u)){
                at_ready = 1;
                break;
            }
        }
            
        if(at_ready == 1){
            break;
        }else{
            // Soft power cycle makes sure that the modem is all the way on (like holding on button on for a few seconds).
            modem_soft_power_cycle();
        }
    }
    // Calculate boot up time and save to variable boot_time.
    boot_time = getTimeStamp() - boot_time;
    printNotif(NOTIF_TYPE_EVENT,"Modem booot time: %d",boot_time);
    
    if(at_ready == 1){
        printNotif(NOTIF_TYPE_EVENT,"Modem ready for AT commands after %d attempt(s).",attempts);
        modem_state = MODEM_STATE_STARTUP;
    }else{
        printNotif(NOTIF_TYPE_ERROR,"No response from modem.");
        modem_state = MODEM_STATE_OFF;
        // Power down the modem -- completely "kill" modem to conserve power.
        modem_power_down();
        return 0u;
    }
    return 1u;
}

// Puts modem in soft power cycle -- modem has power but isn't all the way on, so soft power cycle makes sure that it is all the way on (like holding on button on for a few seconds)
void modem_soft_power_cycle(){
    Pin_Telit_ONOFF_Write(1u); // Pulls Telit_ONOFF pin high
    CyDelay(2000u); // Give modem a sec to boot
    Pin_Telit_ONOFF_Write(0u); // Pulls Telit_ONOFF pin low
    
}

// Clear and reset the uart_received_string variable and uart_string_index for the next transmission.
void uart_string_reset(){
    uart_string_index=0;
    // memset(void *str, int c, size_t n) copies the character c (an unsigned char) to the first n characters of the string pointed to, by the argument str.
    memset(uart_received_string,'\0',sizeof(uart_received_string));
    // Clears the receiver memory buffer and hardware RX FIFO of all received data.
    UART_Telit_ClearRxBuffer();
}

// Modem controlled  via  the  serial  interface  using  the  standard  AT commands.
// AT is an ATTENTION command and is used as a prefix to other parameters in a string. 
// Sends commands to the modem
uint8_t at_write_command(char* commands, char* expected_response,uint32_t timeout){

    uint32 at_attempt = 0;
    
    for(at_attempt = 0; at_attempt < 3; at_attempt++) {
        
        char* compare_location;
        uint32_t delay=10;
        uint32_t i, interval=timeout/delay;        
        
        // Clear and reset the uart_received_string variable and uart_string_index for the next transmission.
        uart_string_reset();
        
        // Sends a NULL terminated string to the TX buffer for transmission.
        UART_Telit_PutString(commands);
        
        
        // If no timeout, no need to wait for response
        if(timeout == NO_AT_TIMEOUT){
            printNotif(NOTIF_TYPE_EVENT,"Modem sent AT command without timeout: %s\r\n", commands);
            return 1u;
        }
      
     
        for(i=0;i<interval;i++){ 
            CyDelay((uint32) delay);
            // strstr(const char *haystack, const char *needle) function finds the first occurrence of the substring needle in the string haystack.
            // Returns a pointer to the first occurrence in haystack of any of the entire sequence of characters specified in needle, or a null pointer if the sequence is not present in haystack.
            compare_location=strstr(uart_received_string,expected_response);        
            if(compare_location!=NULL) {
                if (at_attempt == 0){
                    // we add \n for new line to make it easier to decipher
                    printNotif(NOTIF_TYPE_EVENT,"Modem Received expected AT Response on first Try:\nCommand: %s\nReceived: %s\r\n", commands,uart_received_string);
                }else{
                    printNotif(NOTIF_TYPE_WARNING,"Modem required %d tries to receive AT Response: %s\r\n", (at_attempt+1), uart_received_string);
                 
                }
                return(1);
            }
            
        }  
        
        // Something went wrong, restart UART
        printNotif(NOTIF_TYPE_ERROR,"Could not write AT command: %s\n Received: %s\n", commands,uart_received_string);
      
    }
   // printNotif(NOTIF_TYPE_ERROR,"Modem AT response timeout: %s\r\n", commands);
          
    return(0);    
}

// Test modem -- needs fleshed out
test_t modem_test(){
    
    test_t test; // test_t is a new data type we defined in test.h. We then use that data type to define a structure variable test
    test.status = 0;
    // Print test name and test reason
    snprintf(test.test_name,sizeof(test.test_name),"TEST_MODEM");
    snprintf(test.reason,sizeof(test.reason),"No reponse from modem.");
   
    //try the modem 60*100 (60 secs)
    for(int i = 0; i< 60; i++){
        // Function to startup the modem, connect to the network, and then hand it off to sleep.
        modem_process_tasks();
         
        if(modem_get_state() == MODEM_STATE_OFF){
            // This puts all the modem points into a state that won't leak power
            modem_power_up();
            // Should put this on a max_try counter, so we don't just keep trying to connect over and over
            
        }else if(modem_get_state() == MODEM_STATE_READY){
        
        
        // Get time, and if it looks good, set the RTC with it
        long network_time = modem_get_network_time();
        if(network_time != 0){
            setTime(network_time); // Set the system time 
        }
            
        snprintf(test.reason,sizeof(test.reason),"Modem connected to network.");
        test.status = 1;
        // Power down the modem -- completely "kill" modem to conserve power.
        modem_power_down();
        break;
        
      }
        CyDelay(100u);
        
    }
    
    return test;
}

// Extract string from the UART
uint8 extract_string(char* from, const char* beginMarker, const char* endMarker,  char* target){
    
        char *a, *b;
  
        // Expect the UART to contain something like "+CSQ: 17,99\r\n\r\nOK"
        // - Search for "+CSQ: ".  Place the starting pointer, a, at the resulting index
        a = strstr(from,beginMarker);
        if (a == NULL) {
            //puts("+CSQ: not found in uart_received_string");
            a = from;
        }
        // The strlen() function calculates the length of a given string.
        a += strlen(beginMarker);
        
        // - Find the end marking and offset by 1
        // strstr(const char *haystack, const char *needle) function finds the first occurrence of the substring needle in the string haystack.
        // Returns a pointer to the first occurrence in haystack of any of the entire sequence of characters specified in needle, or a null pointer if the sequence is not present in haystack.
        b = strstr(a,endMarker);
        if (b == NULL) {
            // Puts("Comma not found in uart_received_string");
            b=from+strlen(from);
        }        
        
        // Parse the strings and store the respective rssi and fer values
        strncpy(target,a,b-a);
        target[b-a] = '\0';
        return 1u;
        
}

// Function to startup the modem, connect to the network, and then hand it off to sleep
uint8 modem_process_tasks(){
    // If modem state is "startup" then get the modem start time, configure modem settings, and change the modem state to "waiting for network"
    if(modem_state == MODEM_STATE_STARTUP){
        
        modem_start_time_stamp = getTimeStamp(); // Get timestamp for modem start time
        modem_configure_settings(); // Configure modem settings
        modem_state = MODEM_STATE_WAITING_FOR_NETWORK; // Change modem state 
        return 1u; // Keep going, don't hand off to sleep yet
        
    }
    // Else if modem state is "waiting for network" 
    else if(modem_state == MODEM_STATE_WAITING_FOR_NETWORK){
        // If modem connected to the cell network, save/print the time to connect to the network
        if(is_connected_to_cell_network()){
      
             modem_stats.time_to_network_connect = (int)(getTimeStamp() - (int32)modem_start_time_stamp);
             printNotif(NOTIF_TYPE_EVENT,"Time to connecto to network: %d seconds",
                                            modem_stats.time_to_network_connect);
            get_cell_network_stats(); // Get cell network stats
            set_up_internet_connection(); // Setup the internet connection
            modem_state = MODEM_STATE_WAITING_FOR_IP; // Change modem state 
        }

        //power off if we times out shold only take ~30 secs to connect to network
        if((int)(getTimeStamp()-(int32)modem_start_time_stamp) > TIMEOUT_NETWORK_CONNECT){
            modem_state = MODEM_STATE_OFF; // Change modem state
            modem_power_down(); // Power down the modem -- completely "kill" modem to conserve power
            printNotif(NOTIF_TYPE_ERROR,"Modem timed out on network connect");
            
        }
        
        return 0u;//ok to hand off to sleep
        
    // Else if modem state is "waiting for IP"
    }else if(modem_state == MODEM_STATE_WAITING_FOR_IP){
        // If the modem is connected to the internet, change modem state to "ready", save/print time to acquire IP address
        if(is_connected_to_internet()){
            modem_state = MODEM_STATE_READY; // Change modem state
            // Save/print time to acquire IP address
            modem_stats.time_to_acquire_ip = (int)(getTimeStamp() - (int32)modem_start_time_stamp);
             printNotif(NOTIF_TYPE_EVENT,"Acquired IP address: %d seconds",
                                            modem_stats.time_to_acquire_ip);
        }
        
        // Power off if we times out shold only take ~30 secs to connect to network
        if((int)(getTimeStamp()-(int32)modem_start_time_stamp) > TIMEOUT_IP_ACQUIRE){
            modem_state = MODEM_STATE_OFF;
            modem_power_down(); // Power down the modem -- completely "kill" modem to conserve power
            printNotif(NOTIF_TYPE_ERROR,"Modem timed out on IP address acquire.");
      
        }
        return 0u;//ok to hand off to sleep
    }
    
    return 0u;
    
}

// Checks network status, writes commands to the modem, then extracts string from UART
uint8 is_connected_to_cell_network(){
    // AT command AT+CREG? is the Network Registration Report to check network status -- need '0,0' response to continue
    printNotif(NOTIF_TYPE_EVENT,"Checking AT+CREG?");
    // create new uint8 variable stust and set to equal the output of the function at_write_command to the modem
    uint8 status = at_write_command("AT+CREG?\r", "OK",DEFAULT_AT_TIMEOUT);
    
    printNotif(NOTIF_TYPE_EVENT,"Extracting string AT+CREG?");
    printNotif(NOTIF_TYPE_EVENT,"uart_received_string: %s",uart_received_string);
    
    // If the modem has a status
    if(status){
        // Create a character array of length 10 called creg
        char creg[10];
        // Extract string from the UART
        extract_string(uart_received_string,": ","\r",creg);
        printNotif(NOTIF_TYPE_EVENT,"Registered to network, CREG: %s",creg);
        printNotif(NOTIF_TYPE_EVENT,"Done extracing string AT+CREG?");
    
        // Search creg for "0,1", if "0,1" exists, it will return 1u otherwise it will return NULL
        // If it returns 1u and not NULL, then return 1u
        if(strstr(creg,"0,1")!=NULL){
            //time_network_connect = getTimeStamp();
            return 1u;
        }
    }
    // If the modem doesn't have a status, return 0u
    return 0u;
        
}

// Connect the modem to the internet
void set_up_internet_connection(){
    
    //port 3
   //<connId>,<srMode>,<dataMode>, 
    //at_write_command("AT#SCFGEXT=1,1,0,0,0,0\r", "OK",DEFAULT_AT_TIMEOUT);////LTE modem relevant
    
    //<connId>,<cid>,<pktSz>,<maxTo>,<connTo>,<txTo>
    //at_write_command("AT#SCFG=1,3,300,90,600,50\r", "OK",DEFAULT_AT_TIMEOUT); //LTE modem relevant
    // <cid>,<stat>
   
    // AT command SGACT is Context Activation command is used to activate the specified PDP context, followed by binding data application to the PS network.
    at_write_command("AT#SGACT=1,1\r\n", "OK",1000u);
    
}

// Checks if the modem is connected to the internet
uint8 is_connected_to_internet(){
    // AT command SGACT? reports the range for the parameters <cid> and <stat>
    // cid: is the Packet Data Protocol (PDP) Context Identifier which specifies the particular PDP context definition. PDP context is a data structure that allows the device to transmit. data using Internet Protocol. It includes the device's IP address, IMSI and additional. parameters to properly route data to and from the network.
    // stat: gives the context staus (0 deactivated and 1 activated)
    // Check for 3,1 if Verizon 
    if(at_write_command("AT#SGACT?\r", "1,1",DEFAULT_AT_TIMEOUT)){
        return 1u;
    }
    return 0u;  
}

// Get cellular network stats 
void get_cell_network_stats(){
    // Gets RSSI and FER values
    // RSSI (Received Signal Strength Indicator) is a measurement of how well your device can hear a signal from an access point or router. It’s a value that is useful for determining if you have enough signal to get a good wireless connection.
    // RSRQ (Reference Signal Received Quality) (or SQ for short) gives the signal quality. 
    for(uint8 attempts =0; attempts <10; attempts++){
        // AT command CSQ checks Signal Quality
        at_write_command("AT+CSQ\r", "OK",DEFAULT_AT_TIMEOUT);
        
        char csq[10]; // Create a character array of length 10 called csq 
        // Extract string from the UART
        extract_string(uart_received_string,": ","\r",csq);
        printNotif(NOTIF_TYPE_EVENT,"Network STts: %s",csq);
        
        char *token; // Create a character variable pointer
        // strtok(): breaks "csq" into smaller string when sees ","
        token = strtok(csq,",");
        // If token is not NULL then set modem_stats.rssi equal to the integer "token"
        // int atoi(const char *str) converts the string argument str to an integer (type int).
        if(token != NULL){
            modem_stats.rssi = atoi(token);
        }
        // This "NULL" says to continue where you left off last time.
        token = strtok(NULL,",");
        if(token != NULL){
            modem_stats.sq = atoi(token);
        }
        // If rssi is not equal to 99 OR rssi is not equal to 0, then break.
        if(modem_stats.rssi != 99 || modem_stats.rssi !=0){
            break;
        }
    }
}
    
    
// Configure the modem settings
void modem_configure_settings(){
    
    // Save/print modem model ID number
    if(modem_info.model_id[0] == '\0'){
        // AT command #CGMM returns the "device model identification" code with the command echo.
        if(at_write_command("AT#CGMM\r","OK",DEFAULT_AT_TIMEOUT)){
            // Extract modem model ID number string from the UART
            extract_string(uart_received_string,": ","\r",modem_info.model_id);
            // Print modem model ID number
            printNotif(NOTIF_TYPE_EVENT,"Modem Model: %s",modem_info.model_id);
        }
    }
    
   // Save/print SIM-card ID
   if(modem_info.sim_id[0] == '\0'){
        // AT command #CCID is an execution command that reads the SIM number 
        if(at_write_command("AT#CCID\r", "OK", DEFAULT_AT_TIMEOUT)){
            // Extract SIM-card ID string from the UART
            extract_string(uart_received_string,": ","\r",modem_info.sim_id);
            printNotif(NOTIF_TYPE_EVENT,"SIM ID: %s",modem_info.sim_id);
        }
   }

    // Save/print modem IMEI
    if(modem_info.imei[0] == '\0'){
        // AT command #CGSN returns the product serial number, identified as the IMEI of the mobile, with the command echo. 
        if(at_write_command("AT+CGSN\r","OK",DEFAULT_AT_TIMEOUT)){
            // Extract modem IMEI number string from the UART
            extract_string(uart_received_string,": ","\r",modem_info.imei);
            // Print modem IMEI number
            printNotif(NOTIF_TYPE_EVENT,"IMEI: %s",modem_info.imei);
        } 
    }
    
    // Configure protocol and cellular end point
    // AT command +CGDCONT defines the PDP context parameter values for a PDP context identified by the (local) context identification parameter <cid>. 
    at_write_command("AT+CGDCONT=1,\"IP\",\"wireless.twilio.com\"\r\n", "OK", DEFAULT_AT_TIMEOUT);
    
    // Check if modem power savings is enabled. If so, disable it so we can send data.
    // AT command +CFUN? is a read command that reports the current setting of <fun> which is the power saving function mode
    if(at_write_command("AT+CFUN?\r", "OK",DEFAULT_AT_TIMEOUT)){
        char CFUN[10]; // Create a character array of length 10 called CFUN 
        // Extract UART string recieved from the modem
        extract_string(uart_received_string,": ","\r",CFUN);
        // Print UART string recieved from the modem
        printNotif(NOTIF_TYPE_EVENT,"CFUN: %s",CFUN);
        // Search CRUN for "1", if "1" exists, it will return 1u otherwise it will return NULL
        // If it returns NULL, then write another command to the modem
        if(strstr(CFUN,"1") == NULL){
            // AT command +CFUN= is a set command which selects the level of functionality in the module.
            // = 1 means mobile full functionality with power saving disabled 
            at_write_command("AT+CFUN=1\r", "OK",DEFAULT_AT_TIMEOUT);
        }
    }

    
}

//returns 0 if modem can't get time
long modem_get_network_time(){
    
    //modem reposen format (careful to include the quotes "")
    //+CCLK: "20/05/01,10:48:33-16" the "-16" is the number of quarter hourts (15 mins) from GMT time .. int this case subtract 4 hrs
    //ex: 14:48 GMT

    // If modem_state is not "ready", then print error and return 0
    if(modem_state != MODEM_STATE_READY){
        printNotif(NOTIF_TYPE_ERROR,"Modem not ready to get time. Not conncted to network.");
        return 0;   
    }
    long epoch_time = 0; // Create variable of long time and set equal to 0
    
    // AT command +CCLK? is a read command that returns the current setting of the real-time clock in the format <time>.
    if(at_write_command("AT+CCLK?\r","OK",DEFAULT_AT_TIMEOUT)){
        char network_time[30]; // Create a character array of length 30 called network_time 
        // Extract UART string recieved from the modem and save to variable network_time
        extract_string(uart_received_string,": \"","\"\r",network_time);
        
        ///////////////////  
        struct tm t; // Creates variable t of struct tm
        
        // Creates a bunch of variables of type int and sets them to 0
        int year, month, day, hour, minute, second, gmt_offset = 0;
      
        // Determines the GMT offset 
        // int sscanf(const char *str, const char *format, ...) reads formatted input from a string
        // On success, the function returns the number of variables filled. In the case of an input failure before any data could be successfully read, EOF is returned.
        if(sscanf(network_time,"%d/%d/%d,%d:%d:%d-%d",&year,&month,&day,&hour,&minute,&second,&gmt_offset) == 7){
            gmt_offset = gmt_offset*(900);
        }else if(sscanf(network_time,"%d/%d/%d,%d:%d:%d+%d",&year,&month,&day,&hour,&minute,&second,&gmt_offset) == 7){
            gmt_offset = gmt_offset*(-900);
        }else{
            printNotif(NOTIF_TYPE_ERROR,"Could not parse modem time: %s.",network_time);
            return 0;  // Return 0 if cannot get time 
        }
        
        // Sets each variable to the correct time 
        t.tm_sec = second;
        t.tm_min = minute;
        t.tm_hour = hour;
        t.tm_year = year + 100;    // Year since 1900
        t.tm_mon = month-1;        // Month, where 0 = jan
        t.tm_mday = day;           // Day of the month
        t.tm_isdst = 0;            // Is Daylight Savings on? 1 = yes, 0 = no, -1 = unknown
    
        // Set variable epoch-time to be equal to the local time plus the GMT offset
        // the mktime function converts a broken-down local time (pointed to by timeptr) and returns a pointer to a calendar time.
        epoch_time = mktime(&t) + gmt_offset;
    }
    
    return epoch_time;
}

// Get the modem state
uint8 modem_get_state(){
    return modem_state;
}

// Set modem to sleep
void modem_sleep(){
    // If the modem_state is not "off" then put it to sleep
    if(modem_get_state() != MODEM_STATE_OFF){

        Telit_ControlReg_Sleep(); // Prepares the modem for entering the low power mode.
        UART_Telit_Sleep(); // Saves the current component state, calls the UART_Telit_Stop() function and calls UART_Telit_SaveConfig() to save the hardware configuration.
        
        // Set interrupt on RX pin to wakeup chip from sleep if UART comm starts
       // Rx_Telit_SetInterruptMode(Rx_Telit_0_INTR,Rx_Telit_INTR_FALLING);
     }
}

// Wakeup the modem
void modem_wakeup(){
    // If modem state is not off then wake it up
    if(modem_get_state() != MODEM_STATE_OFF){
        Telit_ControlReg_Wakeup(); // Restores the modem after waking up from the low power mode.
        // Calls the UART_Telit_RestoreConfig() function to restore the configuration.
        // If the modem was enabled before the UART_Telit_Sleep() function was called, the UART_Telit_Wakeup() function will also re-enable the component.
        UART_Telit_Wakeup();
        // Remove the need for this pin itterupt
        //Rx_Telit_SetInterruptMode(Rx_Telit_0_INTR,Rx_Telit_INTR_NONE);
        
        // If woken up from pin, go ahead and delay for 100ms to allow modem UART to deliver the message
        if(modem_get_state() == MODEM_STATE_WAITING_FOR_IP){
            CyDelay(100u);
        }
        
     }
    
}


    
// Returns lat,lom,alt data from Assited GPS (AGPS) system
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
    
    // Enable this for thr LTER module
    // Enable GPS Low-Dropout Regulator
    at_write_command("AT#GPIO=5,1,1,0\r","OK",DEFAULT_AT_TIMEOUT);
    
    // Configure SUPL connection params
    at_write_command("AT$SUPLSEC=1\r","OK",DEFAULT_AT_TIMEOUT);
    
    // Use SUPL2.0
    at_write_command("AT$SUPLV=2\r","OK",DEFAULT_AT_TIMEOUT);
    
    // Enable AGPS service
    at_write_command("AT$AGPSEN=1\r","OK",DEFAULT_AT_TIMEOUT);
    
    // Chose SUPL server, use google publiuc service
    at_write_command("AT$SLP=1,\"supl.google.com:7275\"\r","OK",DEFAULT_AT_TIMEOUT);
    
    //pdp context has to be lockeed to carrier for this to work
    
    // Unlock context since it needs to be used by modem
    at_write_command("AT#SGACT=1,0\r\n", "OK",DEFAULT_AT_TIMEOUT);
    
    at_write_command("AT$LCSLK=1,1\r","OK",DEFAULT_AT_TIMEOUT);//ATT
    //at_write_command("AT$LCSLK=1,3\r","OK",DEFAULT_AT_TIMEOUT);//Verizon
    
    // Relock the context
    at_write_command("AT#SGACT=1,1\r\n", "OK",10000u);
    
    //start AGPS service
    at_write_command("AT$GPSSLSR=1,1,,,,,2\r","OK",DEFAULT_AT_TIMEOUT); 
    
    
    // Start the AGPS service
    // Will keep receiving a buynch of commas without numbers, until GPS locks
    // Good lock will retunr a string with "N" and "W" coordinates
    
    
 
    uint8 gps_status = 0;
    for(uint16 i=0;i<200;i++){
        gps_status = at_write_command("AT$GPSACP\r","N",DEFAULT_AT_TIMEOUT);//ATT
        if(gps_status){
            break;   
        }
        CyDelay(1000u);// Wait for a second -- BLOCKING. Could be more elegant to incorporate this into the modem state machine
    }
    
    //parse data here
    
    //example $GPSACP: 210926.000,4217.8003N,08341.3158W,2.3,258.2,3,17.8,4.8,2.6,220520,05,00
    float lat;
    float lon;
    float alt;
    
    // Look for $GPSACP:  in uart_received_string and save to sl
    // char *strstr(const char *haystack, const char *needle) function finds the first occurrence of the substring needle in the string haystack.
    char *s1 = strstr(uart_received_string,"$GPSACP: ");
    // Look in sl for lat, long, alt and save in match 
    // int sscanf(const char *str, const char *format, ...) reads formatted input from a string.
    int match = sscanf(s1,"$GPSACP: %*f,%fN,%fW,%*f,%f,%*s",&lat,&lon,&alt);
    if(match >= 3){
        // Calculate lat and lon
        // floorf(x) calculates the largest integer that is less than or equal to x. 
        // fmod(double x, double y) returns the remainder of x divided by y.
        lat = floorf(lat/100) + fmod(lat,100)/60;
        lon = -(floorf(lon/100) + fmod(lon,100)/60);
        gps.altitude = alt; // Save alt, lat, and lon to the variable gps.
        gps.latitude = lat;
        gps.longitude = lon;
        gps.valid = 1;
    }
    
    // Disable LDO, just in case
    at_write_command("AT#GPIO=5,0,1,0\r","OK",DEFAULT_AT_TIMEOUT);
    
    // Flush modem's GPS data, just in case
    at_write_command("AT$GPSR=0\r","OK",DEFAULT_AT_TIMEOUT);
    
    
    
    
    
    /* Relevant to 3g modem, can ignore
    
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

    // Calculate time to lock into GPS location and save in variable lock_time
    lock_time = getTimeStamp() - lock_time;
    gps.time_to_lock = lock_time;
    printNotif(NOTIF_TYPE_EVENT,"Time to lock GPS: %d", gps.time_to_lock);
    
    return gps;
    
}
