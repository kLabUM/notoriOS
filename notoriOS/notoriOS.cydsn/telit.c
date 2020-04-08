/* ========================================

 * ========================================
*/

#include <telit.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"

char main_host[]="52.205.129.200";
uint16_t main_port=8086;
char database[]="MPM";
char uart_received_string[1024];
uint8_t uart_string_index=0;
char device_index[3]={"\0"};
char MEID[16]={"\0"};
MODEM_DESCRIPTOR_T modem={EVENT_CHECK_UART,STATE_ECHO_CORRECT,ACTION_CHECK_NETWORK,0,0,"",0};

/* This matrix describes what action to execute when the modem returns the
 * the right response STATE_ECHO_CORRECT, or when the modem never returns the
 * the right response STATE_ECHO_TIMEOUT, given the current EVENT.
 * When the modem is at STATE_CMD_SENT or STATE_ECHO_WRONG, it will wait till
 * the correct response is received or timeout.*/
EVENT_NODE_T EVENT_NODE_LIST[]={
       
    {EVENT_CHECK_UART,STATE_ECHO_CORRECT,ACTION_CHECK_NETWORK},     
            
    {EVENT_CHECK_NETWORK,STATE_ECHO_CORRECT,ACTION_REQUEST_TIME},
    {EVENT_CHECK_NETWORK,STATE_ECHO_TIMEOUT,ACTION_CHECK_NETWORK},     
    
    {EVENT_REQUEST_TIME,STATE_ECHO_CORRECT,ACTION_PARSE_TIME},
    //{EVENT_REQUEST_TIME,STATE_ECHO_TIMEOUT,ACTION_CHECK_NETWORK},
    {EVENT_REQUEST_TIME,STATE_ECHO_TIMEOUT,ACTION_REQUEST_TIME}, // BPW
    
    /* this process doesn't have reponse, so echo is set to CORRECT*/
    {EVENT_PARSE_TIME,STATE_ECHO_CORRECT,ACTION_CHECK_PDP},   
    
    {EVENT_CHECK_PDP,STATE_ECHO_CORRECT,ACTION_CHECK_DEVICE_INDEX},
    {EVENT_CHECK_PDP,STATE_ECHO_TIMEOUT,ACTION_ACTIVATE_PDP},
    
    {EVENT_ACTIVATE_PDP,STATE_ECHO_CORRECT,ACTION_CHECK_DEVICE_INDEX},
    {EVENT_ACTIVATE_PDP,STATE_ECHO_TIMEOUT,ACTION_CHECK_NETWORK},         
    
    {EVENT_REQUEST_MEID,STATE_ECHO_CORRECT,ACTION_INQUIRE_INDEX},
    {EVENT_REQUEST_MEID,STATE_ECHO_TIMEOUT,ACTION_REQUEST_MEID},    
    
    {EVENT_OPEN_SOCKET,STATE_ECHO_CORRECT,ACTION_INIT_SEND},
    {EVENT_OPEN_SOCKET,STATE_ECHO_TIMEOUT,ACTION_REQUEST_SOCKET_STATUS},
    
    {EVENT_REQUEST_SOCKET_STATUS,STATE_ECHO_CORRECT,ACTION_PARSE_SOCKET_STATUS},
    {EVENT_REQUEST_SOCKET_STATUS,STATE_ECHO_TIMEOUT,ACTION_CHECK_NETWORK},
    
    {EVENT_PARSE_SOCKET_STATUS,STATE_ECHO_CORRECT,ACTION_INIT_SEND},
    {EVENT_PARSE_SOCKET_STATUS,STATE_ECHO_TIMEOUT,ACTION_SOCKET_DIAL},
    
    {EVENT_INIT_SEND,STATE_ECHO_CORRECT,ACTION_SEND_STRING},
    {EVENT_INIT_SEND,STATE_ECHO_TIMEOUT,ACTION_CHECK_NETWORK},
        
    /* proceed to receive messages from server regardless of send status*/
    {EVENT_SEND_MESSAGE,STATE_ECHO_CORRECT,ACTION_RECEIVE_MESSAGE},
    {EVENT_SEND_MESSAGE,STATE_ECHO_TIMEOUT,ACTION_RECEIVE_MESSAGE},
    
    /* if no message is received, i.e. TIMEOUT, the empty message will
     * also be parsed as communication error*/    
    {EVENT_RECEIVE_MESSAGE,STATE_ECHO_CORRECT,ACTION_PARSE_MESSAGE},
    {EVENT_RECEIVE_MESSAGE,STATE_ECHO_TIMEOUT,ACTION_PARSE_MESSAGE},
    
    /* this process doesn't have reponse, so echo is set to correct*/
    {EVENT_PARSE_MESSAGE,STATE_ECHO_CORRECT,ACTION_CLOSE_SOCKET},
    
    {EVENT_CLOSE_SOCKET,STATE_ECHO_CORRECT,ACTION_REQUEST_TIME},
    {EVENT_CLOSE_SOCKET,STATE_ECHO_TIMEOUT,ACTION_CLOSE_SOCKET}        
    
};

CY_ISR(isr_telit_rx){      
    while(UART_Telit_GetRxBufferSize()==1){
        uart_received_string[uart_string_index]=UART_Telit_GetChar();
        uart_string_index++;
    }       
}

void adjust_time(){
    char  num[10] = {'\0'};
    char *ptr=strstr(uart_received_string,"#CCLK: ");
    /*if(ptr!=NULL){
        
        ptr+=sizeof("#CCLK: ");
        
        strncpy(num,ptr,2);
        *(num+2)='\0';
        time.year=atoi(num)+2000;
        
        ptr=ptr+3;
        strncpy(num,ptr,2);
        *(num+2)='\0';
        time.month=atoi(num);
        
        ptr=ptr+3; 
        strncpy(num,ptr,2);
        *(num+2)='\0';
        time.day=atoi(num);
        
        ptr=ptr+3; 
        strncpy(num,ptr,2);
        *(num+2)='\0';
        time.hour=atoi(num);
        
        ptr=ptr+3; 
        strncpy(num,ptr,2);
        *(num+2)='\0';
        time.min=atoi(num);
        
        ptr=ptr+3; 
        strncpy(num,ptr,2);
        *(num+2)='\0';
        time.sec=atoi(num);
        memset(uart_received_string,'\0',sizeof(uart_received_string));
    }*/   
}

void modem_scheduler_adjust_system_time(){
    modem.modem_event=EVENT_PARSE_TIME;
    modem.modem_state=STATE_ECHO_CORRECT;
    adjust_time();      
}

void uart_string_reset(){
    uart_string_index=0;
    memset(uart_received_string,'\0',sizeof(uart_received_string));
    UART_Telit_ClearRxBuffer();
}

// --------------------------------------------
//
//
void modem_send_at_command(char * cmd){
    uart_string_reset();
    UART_Telit_PutString(cmd);    
    modem.modem_state=STATE_AT_CMD_SENT;
    modem.echo_counter=0;
}
//
//
// --------------------------------------------

void modem_compare_echo(){
    char* compare_location;
    compare_location=strstr(uart_received_string,modem.expected_response);
    if(compare_location!=NULL)modem.modem_state=STATE_ECHO_CORRECT;
    else {
        modem.modem_state=STATE_ECHO_NOT_FOUND;
        modem.echo_counter++;
        if(modem.echo_counter>=MAX_ECHO_COUNT)modem.modem_state=STATE_ECHO_TIMEOUT;
    }    
}

void modem_scheduler_request_meid(){
    modem_send_at_command("AT+GSN=1\r");
    sprintf(modem.expected_response,"OK");
    modem.modem_event=EVENT_REQUEST_MEID;
}

void modem_scheduler_init_index(){
    if(parse_meid(MEID)==1)modem_scheduler_socket_dial();
    else modem_scheduler_request_meid();    
}

void modem_check_device_index(){
    modem.connection=1;
    if(device_index[0]=='\0'){
        modem_scheduler_request_meid();
    }
    else modem_scheduler_socket_dial();
}

//
// #######################################
//
void modem_scheduler_request_time(){ 
    modem_send_at_command("AT#CCLK?\r");
    sprintf(modem.expected_response,"OK");
    modem.modem_event=EVENT_REQUEST_TIME;
}


void modem_scheduler_check_network(){
    modem_send_at_command("AT+CGREG?\r");
    sprintf(modem.expected_response,",1");  
    modem.modem_event=EVENT_CHECK_NETWORK;
}
//
// #######################################
//
void modem_scheduler_check_PDP(){
    modem_send_at_command("AT#SGACT?\r");
    sprintf(modem.expected_response,",1");   
    modem.modem_event=EVENT_CHECK_PDP;
}

void modem_scheduler_activate_PDP(){
    modem_send_at_command("AT#SGACT=3,1\r");
    sprintf(modem.expected_response,"OK"); 
    modem.modem_event=EVENT_ACTIVATE_PDP;
}

void modem_scheduler_socket_dial(){
    /* if no averaged data are available to be sent, return directly*/
    /*if(avg_data_body.data_avg_status!=1){
        memset(uart_received_string,'\0',sizeof(uart_received_string));
        return;
    }poipoi*/
    char socket_dial_str[128];
    /* port and ip address are defined in data.c*/
    sprintf(socket_dial_str,"AT#SD=3,0,%d,%s,0,0,1\r",main_port,main_host);                    
        modem_send_at_command(socket_dial_str); 
    sprintf(modem.expected_response,"OK");    
    modem.modem_event=EVENT_OPEN_SOCKET;
}

void modem_scheduler_socket_status(){
    modem_send_at_command("AT#SS=3\r");
    sprintf(modem.expected_response,"OK");
    modem.modem_event=EVENT_REQUEST_SOCKET_STATUS;
}

void parse_socket_status(){
    modem.modem_event=EVENT_PARSE_SOCKET_STATUS;
    char *a;
    uint8_t ss;    
    // Expect the UART to contain something like "\r\n#SS: 1,0\r\n"
    // - Search for "#SS:".  Place the starting pointer, a, at the resulting index
    a = strstr(uart_received_string,"#SS: 3,");
    if (a == NULL) {
        modem.modem_state=STATE_ECHO_TIMEOUT;
        return;
    }
    a += strlen("#SS: 3,");

    ss = *a-'0';
    if(ss!=2)modem.modem_state=STATE_ECHO_TIMEOUT;
    modem.modem_state=STATE_ECHO_CORRECT;
}

void modem_scheduler_init_send(){    
    modem_send_at_command("AT#SSEND=3\r");
    sprintf(modem.expected_response,">");
    modem.modem_event=EVENT_INIT_SEND;
}

void modem_scheduler_send_message(){
    char send_str[1024]={'\0'};
    
    /*this message is to get device index*/
    if(device_index[0]=='\0'){        
        char main_query[128]={'\0'};
        sprintf(main_query,"query?db=%s&q="
                "SELECT index from device where MEID=\'%s\'",database,MEID);
        char *query_encoded=url_encode(main_query);
        construct_generic_request(send_str,"",main_host,query_encoded,main_port,
            "GET","Close","",0,"1.1");  
    }
    else {
        char body[1024]={'\0'};
        char write_route[32]={'\0'};
        sprintf(write_route,"write?db=%s",database);    
        construct_generic_request(send_str,body,main_host,write_route,main_port,
            "POST","Close","",0,"1.1");
        //avg_data_body.data_avg_status=0;        
    }
    sprintf(modem.expected_response,"SRING: 1");
    modem_send_at_command(send_str);    
    modem.modem_event=EVENT_SEND_MESSAGE;    
}

void modem_scheduler_receive_send_echo(){
    modem_send_at_command("AT#SRECV=3,1500\r");
    if(device_index[0]=='\0') sprintf(modem.expected_response,"values");
    else sprintf(modem.expected_response,"OK");
    modem.modem_event=EVENT_RECEIVE_MESSAGE;
}

void modem_scheduler_parse_http_response(){
    modem.modem_event=EVENT_PARSE_MESSAGE;
    if(device_index[0]=='\0'){
        char * pend=strstr(uart_received_string,"]]"),*pstart=pend;
        while(*pstart!=',')pstart--;            
        pstart++;
        *pend='\0';            
        sprintf(device_index,"%s",pstart);
        modem.modem_state=STATE_ECHO_CORRECT;
        memset(uart_received_string,'\0',sizeof(uart_received_string));
        return;
    }
      
    char status_code[5]={'\0'};
    parse_http_status(uart_received_string, (char*) NULL, status_code, (char*) NULL);
    if(status_code[0]!='2'){
        modem.transaction_error_counter++; 
        if(modem.transaction_error_counter>=3){
            modem.transaction_error_counter=0;
            modem.modem_event=EVENT_CHECK_UART;/* check network again*/
        }
    }
    modem.modem_state=STATE_ECHO_CORRECT;
    memset(uart_received_string,'\0',sizeof(uart_received_string));
}

void modem_scheduler_close_socket(){
    modem_send_at_command("AT#SH=3\r");
    sprintf(modem.expected_response,"OK");
    modem.modem_event=EVENT_CLOSE_SOCKET;
    
    if(modem.transaction_error_counter>=3){
        modem.transaction_error_counter=0;
        modem.modem_event=EVENT_CHECK_NETWORK;       
    }
    
}

void modem_check_action(){    
    uint8_t i=0;    
    while((modem.modem_event!=EVENT_NODE_LIST[i].modem_event)
    ||(modem.modem_state!=EVENT_NODE_LIST[i].modem_state))i++;
    modem.modem_action=EVENT_NODE_LIST[i].modem_action;    
}

void modem_select_action(){
    modem_check_action();
    switch(modem.modem_action){
        
        case ACTION_CHECK_DEVICE_INDEX:
        modem_check_device_index();
        break;
        
        case ACTION_REQUEST_MEID:
        modem_scheduler_request_meid();
        break;
            
        case ACTION_INQUIRE_INDEX:
        modem_scheduler_init_index();
        break;
        
        case ACTION_REQUEST_TIME:
        modem_scheduler_request_time();
        break;
        
        case ACTION_PARSE_TIME:
        modem_scheduler_adjust_system_time();
        break;
        
        case ACTION_CHECK_NETWORK:
        modem_scheduler_check_network();
        break;
        
        case ACTION_CHECK_PDP:
        modem_scheduler_check_PDP();
        break;
     
        case ACTION_ACTIVATE_PDP:
        modem_scheduler_activate_PDP();
        break;                
        
        case ACTION_SOCKET_DIAL:
        modem_scheduler_socket_dial();
        break;
        
        case ACTION_REQUEST_SOCKET_STATUS:
        modem_scheduler_socket_status();
        break;
        
        case ACTION_PARSE_SOCKET_STATUS:
        parse_socket_status();
        break;
                
        case ACTION_INIT_SEND:
        modem_scheduler_init_send();
        break;
        
        case ACTION_SEND_STRING:
        modem_scheduler_send_message();
        break;
        
        case ACTION_RECEIVE_MESSAGE:
        modem_scheduler_receive_send_echo();
        break;
        
        case ACTION_PARSE_MESSAGE:
        modem_scheduler_parse_http_response();
        break;
        
        case ACTION_CLOSE_SOCKET:
        modem_scheduler_close_socket();
        break;                
    }
}

void modem_check_state(){
    switch(modem.modem_state){
        case STATE_AT_CMD_SENT:
        modem_compare_echo();
        break;
        
        case STATE_ECHO_NOT_FOUND:
        modem_compare_echo();
        break;
        
        case STATE_ECHO_TIMEOUT:
        //sprintf(SD_Buffer,"%s%s",SD_Buffer,uart_received_string);          
        modem_select_action();
        break;
        
        case STATE_ECHO_CORRECT:
        //sprintf(SD_Buffer,"%s%s",SD_Buffer,uart_received_string); 
        modem_select_action();
        break;        
    }
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
                //sprintf(SD_Buffer,"%s%s",SD_Buffer,uart_received_string); 
                //strip newline characters and other stuff
                //stripEscapeCharacters(uart_received_string);
                //printNotif("Modem Sent %d/3: %s\r\n", (at_attempt+1), commands);
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
        //sprintf(SD_Buffer,"%s%s",SD_Buffer,uart_received_string);
    }
    printNotif(NOTIF_TYPE_ERROR,"Modem AT response timeout: %s\r\n", commands);
          
    return(0);    
}

uint8_t write_modem_echo(){
    if(uart_received_string[0]=='\0') return 0;
    char* compare_location=strstr(uart_received_string,"W");
    uart_string_index=0;
    memset(uart_received_string,'\0',sizeof(uart_received_string));
    if(compare_location!=NULL) return 1; 
    else return 0;
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *url_encode(char *str) {
    char *pstr, *buf = (char*) malloc(strlen(str) * 3 + 1), *pbuf = buf;
    for (pstr = str; *pstr != '\0'; pstr++) {
        if (*pstr == ' ') {
            *pbuf++ = '%', *pbuf++ = '2', *pbuf++ = '0';
        } else {
            *pbuf++ = *pstr;
        }
    }
    *pbuf = '\0';
    return buf;
}                   


uint8_t parse_meid(char* meid) {
/*
int modem_get_meid(char* meid)

Return the MEID of the cell module
- Tested for CC864-DUAL and DE910-DUAL

Example CC864-DUAL Conversation:
[Board] AT#MEID?
[Modem] #MEID: A10000,32B9F1C0

        OK

Example DE910-DUAL Conversation:
[Board] AT#MEID?
[Modem] #MEID: A1000049AB9082

        OK
*/
    
    // If successful, parse the string received over uart for the meid
    char *a, *b;
    
    // Test case:
    //char str1[] = "AT#MEID?\r\r\n#MEID: A10000,32B9F1C0\r\n\r\nOK";
    //strcpy(uart_received_string,str1);

    // Expect the UART to contain something like "\r\n#MEID: A10000,32B9F1C0\r\n\r\nOK"
    // - Search for "#MEID:".  Place the starting pointer, a, at the resulting index
    
    a = strstr(uart_received_string,"+GSN: ");
    if (a == NULL) {

        //puts("#MEID: not found in uart_received_string");
        return 0u;
    }
    // - Shift the pointer to the beginning of the MEID, i.e. "A10000,32B9F1C0\r\n\r\nOK"
    a += strlen("+GSN: ");
    
    // - Find the carriage return marking after "#MEID: " that follows the MEID
    b = strstr(a,"\r");
    if (b == NULL) {

        //puts("Carriage return not found in uart_received_string");
        return 0u;
    }
    
    // Parse the strings and store the MEID in *meid
    strncpy(meid,a,b-a);
    meid[b-a] = '\0';
    
    // In the case for modules like CC864-DUAL where "," is in the middle of the MEID,
    // remove the comma
    // * Assume only 1 comma needs to be removed
    a = strstr(meid,",");
    if (a != NULL) {
        memmove(a,a+1,strlen(meid)-1);
    } 

    memset(uart_received_string,'\0',sizeof(uart_received_string));        
    return 1;    
}

uint8_t modem_report_software_version(){
    return(at_write_command("AT+CGMR\r","OK",1000));
}

uint8_t modem_report_hardware_identification(){
    return(at_write_command("AT+CGMM\r","OK",1000));
}

uint8_t modem_report_product_identification(){
    return(at_write_command("AT+GSN=1\r","OK",1000));
}

uint8 modem_check_signal_quality(int *rssi, int *fer){
/* 

uint8 modem_check_signal_quality(uint8 *rssi)

Returns the received signal strength indication (rssi) of the modem
This value ranges from 0-31, or is 99 if unknown/undetectable

Also returns the frame rate error (fer) of the modem.  
This value ranges from 0-7, or is 99 if unknown/undetectable.
(From experience, fer is almost always 99)

Example conversation:
[Board] AT+CSQ
[Modem] +CSQ: 17,99

        OK
*/
    
    // Check for valid response from cellular module
    if(at_write_command("AT+CSQ\r","OK",1000u) == 1u){                  
        
        // If successful, parse the string received over uart for the rssi value
        char *a, *b, *c;
        char rssi_str[5] = {'\0'}, fer_str[5] = {'\0'};

        // Expect the UART to contain something like "+CSQ: 17,99\r\n\r\nOK"
        // - Search for "+CSQ: ".  Place the starting pointer, a, at the resulting index
        a = strstr(uart_received_string,"+CSQ: ");
        if (a == NULL) {
            //puts("+CSQ: not found in uart_received_string");
            return 0u;
        }
        // - Shift the pointer to the beginning of the rssi value, i.e. "17,99\r\n\r\n\OK"
        a += strlen("+CSQ: ");
        
        // - Find the comma marking after "+CSQ: " that follows the rssi value
        b = strstr(a,",");
        if (b == NULL) {
            //puts("Comma not found in uart_received_string");
            return 0u;
        }        

        // - Find the carriage return after the comma that follows the fer value
        c = strstr(b + strlen(","),"\r");
        if (c == NULL) {
            //puts("\r not found in uart_received_string");
            return 0u;
        }
        
        // Parse the strings and store the respective rssi and fer values
        strncpy(rssi_str,a,b-a);
        rssi_str[b-a] = '\0';
        // - Store the rssi value in *rssi
        *rssi = (uint8) strtol(rssi_str,(char **) NULL, 10); // Base 10

        strncpy(fer_str,b+strlen(","),c-b+strlen(","));
        fer_str[c-b+strlen(",")] = '\0';
        // - Store the fer value in *fer
        *fer = (uint8) strtol(fer_str,(char **) NULL, 10); // Base 10
        
        //printf("rssi: %d\r\n",rssi);
        //printf("fer: %d\r\n",fer);

        return 1u;
    }    
    
    return 0u;  
}

uint8_t modem_define_PDP(){
    return(at_write_command("AT+CGDCONT=3,\"IP\",\"n.ispsn\"\r","OK",2000)); // BPW
    //return(at_write_command("AT+CGDCONT=1,\"IPV4V6\",\"wholesale\"\r","OK",60000));
}

uint8 modem_set_flow_control(uint8 param){
    char cmd[100]={'\0'};   
    sprintf(cmd,"AT&K%u\r",param);
    return(at_write_command(cmd,"OK",1000u));
    //return(at_write_command("AT&K0\r","OK",1000u));
}

uint8 modem_set_error_reports(uint8 param){	
    char cmd[100]={'\0'};    
    sprintf(cmd,"AT+CMEE=%u\r",param);
    return(at_write_command(cmd,"OK",1000u));
    //return(at_write_command("AT+CMEE=2\r","OK",1000u));
}

uint8 modem_setup() {
/* Initialize configurations for the modem */
	// Set Error Reports to verbose mode
	return(modem_set_error_reports(2u));

}

uint8_t modem_check_network(){
    return(at_write_command("AT+CGREG?\r",",1",1000));
   
}

uint8_t modem_activate_pdp_context(){
    
    uint8_t status=at_write_command("AT#SGACT?\r",",1",1000u);
    if(status==1) return(1);
    else return(at_write_command("AT#SGACT=3,1\r","OK",2000u));        
}


uint8_t modem_connect(){
    /* check registration*/
    uint8_t network_status,counter=0;
    do{
        network_status=modem_check_network();        
        if(network_status==1) break;
        CyDelay(1000);
        counter++;
    }while(counter<3);
    
    counter=0;
    
    /* check or activate PDP context*/
    if(network_status==1){
        do{
            network_status=modem_activate_pdp_context();
           if(network_status==1)return(1);           
            CyDelay(1000);
            counter++;
        }while(counter<3);      
    }
    return(0);   
}


/*
Searches a string "http_status" for and attempts to parse the status line.
Leverages Status-Line protocol for HTTP/1.0 and HTTP/1.1

    Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF

(See https://www.w3.org/Protocols/HTTP/1.0/spec.html#Status-Line.
 SP is a space, CRLF is carriage return line feed)

Stores the results in "version", "status_code", and "phrase"
*/
uint8 parse_http_status(char* http_status, char* version, char* status_code, char* phrase) {
    char *a, *b;
    
    // Find the version and copy to "version"
    a = strstr(http_status,"HTTP/");
    if (a == NULL) {
        return 0u;
    }
    a += strlen("HTTP/");
    b = strstr(a," ");
    if (b == NULL) {
        return 0u;
    }
    strncpy(version,a,b-a);
    
    // Find the status code and copy to "status_code"
    a = b+1;
    b = strstr(a," ");
    if (b == NULL) {
        return 0u;
    }
    strncpy(status_code,a,b-a);
    //*status_code = (int) strtol(tmp_code,(char **) NULL, 10); 
    
    // Find the status phrase and copy to "phrase"
    a = b+1;
    b = strstr(a,"\r\n");
    if (b == NULL) {
        return 0u;
    }
    strncpy(phrase,a,b-a);
    
    return 1u;
}

void construct_generic_request(char* send_str, char* body, char* host, char* route,
                               int port, char* method, char* connection_type,
	                           char *extra_headers, int extra_len, char* http_protocol){

    sprintf(send_str,"%s /%s HTTP/%s\r\n", method, route, http_protocol);
    sprintf(send_str,"%s%s%s%s%d%s%s%s%s",
            send_str, // 1
            "Host: ", host, ":", port, "\r\n", // 2 3 4 5 6
            "Connection: ", connection_type, "\r\n"); // 7 8 9
	if (extra_headers && strlen(extra_headers) > 0){
		sprintf(send_str, "%s%s", send_str, extra_headers);
	}
	if (strcmp(method, "GET") != 0){
		sprintf(send_str, "%s%s%s%d%s%s",
			send_str,
            "Content-Type: text/plain\r\n", // 10
            "Content-Length: ", (extra_len + strlen(body)), //11 12 (Extra len should be 2 for flask server)
            "\r\n\r\n", body); // 13 14 15
	}
	sprintf(send_str, "%s%s", send_str, "\r\n\032"); 
}
  
void modem_start(){    
    /*enable UART and interrupts to receive data from the cell module*/ 
    UART_Telit_Start();
    isr_telit_rx_StartEx(isr_telit_rx);
   
    
}

// deinitialize modem
uint8 modem_stop(){
    
    uint8 status = 0;
    if(at_write_command("AT#SHDN\r","OK",1000u) != 1){
        Pin_Telit_ONOFF_Write(1u); /*power off telit cell module*/
        CyDelay(2500); 
        status = 1;
    }  
    
    /* it takes >=15s for the unit to shut down completely*/
    CyDelay(15000);
	Pin_Telit_ONOFF_Write(0u);			// Save energy by pulling down "push button"
    Pin_Telit_pwr_Write(0u);
	//Pin_Telit_SHDN_Write(0u);		// Save energy by pulling down "push button"
    UART_Telit_Stop();
    isr_telit_rx_Stop();

    return status;

}

uint8_t modem_power_on(){
       
    uint8_t counter=0;
    Pin_Telit_ONOFF_Write(0u);
    Pin_Telit_pwr_Write(1u);
    CyDelay(1000u);
    Pin_Telit_ONOFF_Write(1u);
    CyDelay(2500u);
    Pin_Telit_ONOFF_Write(0u);
 
    
    
    //ideally, PIN18 on the module is configured a a GPIO
    //This GPIO should pull low when the modem is pwoered up and ready
    //This will save time, otherwise, we need to wait for two full seconds
    /*if(Pin_Telit_SWRDY_Read()){
        
        Pin_Telit_ONOFF_Write(0u);
                    
      
        do{
            CyDelay(200);
            counter++;       
            if(counter>=255)return 0;                
        }while(Pin_Telit_SWRDY_Read());
        
        Pin_Telit_ONOFF_Write(1u); 
            
    }else{
         //if the GPIO feature was not configured, wait for 2 full secs and try to enable it for next time
         Pin_Telit_ONOFF_Write(0u); 
         CyDelay(2000); 
         Pin_Telit_ONOFF_Write(1u);
         at_write_command("AT#GPIO=2,1,1,1\r","OK",1000u);
    }*/
    
    
   
    for(counter=0;counter<3;counter++){            
    	if(at_write_command("AT\r","OK",1000u)==1){
            
            /* set proper parameters for modem 
             * so it will check network registration next*/
            modem.modem_event=EVENT_CHECK_UART;
            modem.modem_state=STATE_ECHO_CORRECT;
            return 1;
        }
    }
    
    return 0;
}

void modem_power_off(){
   
    if(at_write_command("AT#SHDN\r","OK",1000u) != 1){
        Pin_Telit_ONOFF_Write(1u); /*power off telit cell module*/
        CyDelay(2500);
        Pin_Telit_ONOFF_Write(0u);        
    }   
    
    /* it takes >=15s for the unit to shut down completely*/
    CyDelay(15000);
    Pin_Telit_pwr_Write(0u);

}

uint8_t modem_set_interface_style(){
    return(at_write_command("AT#SELINT=2\r","OK",1000));
}

uint8_t modem_set_LED(){
    if(at_write_command("AT#GPIO=1,0,2\r","OK",1000u)==1)return(1);
    return(2);
}

uint8_t modem_report_IMSI(){
    return(at_write_command("AT#CIMI\r","OK",1000u));
}

uint8_t modem_startup(){
    
    printNotif(NOTIF_TYPE_EVENT,"==========================Start Modem Test======================");
    int rssi = 0, fer = 0;
    uint8_t ready=0, iter=0;
    
    /*enable UART communication with Telit module*/
    printNotif(NOTIF_TYPE_EVENT,"Turn Modem On");
    modem_start();
    
    while(iter<3){
        ready=modem_power_on();
        if(ready) break;
        modem_power_off();
        iter++;
    }
    
    if (iter > 0){
        printNotif(NOTIF_TYPE_WARNING,"Modem turned on after %d tries", iter);
    }else{
        printNotif(NOTIF_TYPE_EVENT,"Modem turned on as expected.");
    }
    
    if(!ready) return 0;
    
    modem_set_flow_control(0);
   
    at_write_command("AT#CGMM\r","OK",1000u); //modem model
    at_write_command("AT+CGSN\r","OK",1000u); //IMEI
    char SIM[200];
    modem_get_ccid(SIM);
    printNotif(NOTIF_TYPE_EVENT,"SIM_ID=%s",SIM);
    
    
    // Enable verbose error reporting by the cell module
    // Fully write out the errors as opposed to sending an error code
    at_write_command("AT+CMEE=2\r\n", "OK", 1000u);
    
    at_write_command("AT+CMEE=2\r\n", "OK", 1000u);
    

    
    
    
    
    
        // Configure the APN settings
    at_write_command("AT+CGDCONT=1,\"IP\",\"wireless.twilio.com\"\r\n", "OK", 1000u); 
        //at_write_command(usr_command, "OK", 1000u);
        //at_write_command( pw_command, "OK", 1000u);
        //at_write_command(bnd_command, "OK", 1000u);
        
        // Get CCID of SIM
        //modem_get_ccid(CCID);
               
        // Get the current time from the modem
        //gettime
         // Check for valid response from cellular module
        if (at_write_command("AT+CCLK?\r", "OK", 1000u) == 1u) {
        // Expect the UART to contain something like
        // "#CCLK: "00/01/01,00:00:55+00"\r\n\r\nOK"
            char time[100];
                char *terminator =
            strextract(uart_received_string, time, "+CCLK: ", "\r\n");
            printNotif(NOTIF_TYPE_EVENT,"Modem Time Recevied: %s", time);
        }

        

        // Get the signal strength from the modem
        for (int n_csq = 0; n_csq < 3; n_csq++) {
             // Check for valid response from cellular module
            if (at_write_command("AT+CSQ\r", "OK", 1000u) == 1u) {
                // Expect the UART to contain something like
                // "\r\n+CSQ: 8,4\r\n\r\nOK"
                char strength[100];
                char *terminator =
                    strextract(uart_received_string, strength, "+CSQ: ", ",");
                     printNotif(NOTIF_TYPE_EVENT,"Signal Strength Recevied on %d/3 tries: %s",(n_csq+1), strength);
                    break;
            }
            CyDelay(1000u);
        }
        
            // Send the data to InfluxDB\// Get CCID of SIM

        
        //modem_post_to_influxdb("TEST");
        


    //  END  configure the modem APN
    
    
    
    

     /*at_write_command("AT#SIMDET=0\r","OK",1000);

    
     the SIM card connector doesn't have the SIMIN pin.
     * hence, the SIM card presence status is manually set to ignore
     * SIMIN pin, and act as "SIM inserted"    
  
     at_write_command("AT#SIMDET=1\r","OK",1000);
   
    
    modem_define_PDP();                       //modem_define_PDP(); 
    
    at_write_command("AT#SCFG=1,3,300,90,600,50\r","OK",1000);
         
    modem_check_network();
                                // BPW: Appears the modem either doesn't receive or respond every other time
                                //      First I tried repeating each command,
                                //      then I edited at_write_command and added an outer loop.
                                //      It appears to be a bandaid that fixes the problem, but perhaps
                                //      not the best solution for the long term
                                //
                                //      The state machine currently relies on each
                                //      response successfully being transmitted back from the modem
                                //      It may be a hardware issue?
                                //
                                //      The bandaid fix could be to update EVENT_NODE_LIST[]
                                //      so that the state machine doesn't always go back to the start but stays in the same state
                                //      whenever it received a blank response instead of the expected command
                                //      - Related functions to the state machine are:
                                //        modem_check_action(), modem_select_action(), and modem_check_state()
    modem_set_LED();            //modem_set_LED();
    modem_set_flow_control(0);  //modem_set_flow_control(0);
    modem_setup();              //modem_setup();
    modem_set_interface_style();//modem_set_interface_style();
    //modem_report_hardware_identification();
    modem_report_product_identification(); //modem_report_product_identification();
    
    parse_meid(MEID);
  
    //modem_report_software_version();
       
     it usually takes two queries to the SIM card before
     * its existence is acknowledged
    //for(i=0;i<3;i++){
       modem_report_IMSI();
    //}     
    
    at_write_command("AT#AUTOATT=0\r","OK",1000);
    at_write_command("AT+CGATT=1\r","",1000); // doesn't respond
    
    rssi = 0;
    fer = 0;
    modem_check_signal_quality(&rssi,&fer);
   // printNotif("Modem RSSI %d, FER %d: %s", rssi, fer);
    
    
    uint8 status = 0;
    status = modem_check_network();                    //modem_check_network(); 
    //printNotif("Network Check: Status=%d", status);
    
    at_write_command("AT#CCLK?\r","OK",1000); //at_write_command("AT#CCLK?\r","OK",1000);
    //at_write_command("AT#GPRS=1\r","OK",1000);
    if (at_write_command("AT#SGACT?\r",",0",1000)){
        at_write_command("AT#SCFG?\r","OK",1000);
        at_write_command("AT#SGACT=3,1\r","OK",1000);   
    }
    //at_write_command("AT#SCFG?\r","OK",1000);
    
    
    status = at_write_command("AT#SD=3,0,8086,\"data.open-storm.org\",0,0,1\r","OK",5000);
    //printNotif("Connect to Open Storm: Status=%d", status);
    
    status = at_write_command("AT#SSEND=3\r\n",   ">", 1000);
    
    status = at_write_command("POST /write?db=ARB&u=generic_node&p=MakeFloodsCurrents HTTP/1.1\r\nHost: data.open-storm.org:8086\r\nConnection: close\r\nContent-Length: 30\r\n\r\nv_bat,node_id=GGB003 value=8.5\r\n\r\n\032", "HTTP/1.1 204", 1000u);
    
    //printNotif("End Modem Test: Status=%d", status);
    //at_write_command("AT#SRECV=3,1000","OK",1000);


    
    adjust_time();*/
    
    return 1;           
}


uint8 modem_post_to_influxdb(char *body) {
    
    uint8 status = 100;
    
    // Construct the HTTP request:
    // POST /write?db=<db>&u=<user>&p<pass> HTTP/1.1
    // Host: <host>:<port>
    // Connection: Keep-Alive
    // Content-Length: <strlen(body)>
    // 
    // <body>
    
    // InfluxDB values
   char  request[3000]; 
char  at_command[3000]; 
char node_id[20]    = "GGB004";
char db[20]         = "ARB"; 
char user[20]       = "generic_node";
char pass[20]       = "MakeFloodsCurrents";
char host[20]       = "data.open-storm.org";
char port[20]       = "8086";   

    
    //sprintf(request,"POST /write?db=%s&u=%s&p=%s HTTP/1.1\r\nHost: %s:%s\r\nConnection: Keep-Alive\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n\032",
    //    db,user,pass,host,port,strlen(body),body);
    sprintf(request,"POST /write?db=%s&u=%s&p=%s HTTP/1.1\r\nHost: %s:%s\r\nConnection: close\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n\032",
        db,user,pass,host,port,strlen(body),body);
    // /!\ may need to replace spaces with %20 -- import url_encode from perfect-cell
    
    // Construct the Socket Dial AT command: 
    // AT#SD=1,0,<port>,"<host>",0,0,1

     // Construct the Socket Dial AT command: 
    // AT#SD=1,0,<port>,"<host>",0,0,1
    
    for (int i = 0; i<10; i++){
        printNotif(NOTIF_TYPE_EVENT,"MODEM Register try %d/10",i);
        if(at_write_command("AT+CREG?\r",",1",1000u)){
            break; 
        }
    }
    
    at_write_command("AT#SGACT=1,1\r\n","OK",10000u);
    sprintf(at_command,"AT#SD=1,0,%s,\"%s\",0,0,1\r\n",port,host);
    at_write_command( at_command,   "OK",10000u);
    at_write_command("AT#SSEND=1\r\n",   ">", 1000u);
    
    status = at_write_command(request, "HTTP/1.1 204", 1000u);
    at_write_command("AT#SH=1\r\n", "OK", 1000u);
    return status;
}

uint8 modem_get_ccid(char *ccid) {
    /*
    int modem_get_meid(char* ccid)

    Return the CCID of the cell module

    Example HE910/NL-SW-HSPA Conversation:
    [Board] AT+CCID
    [Modem] AT+CCID\r\n+CCID: 89014103279069593771\r\n\r\nOK
    */

    // Check for valid response from cellular module
    if (at_write_command("AT#CCID\r", "OK", 1000u) == 1u) {
        // Expect the UART to contain something like
        // "\r\n+CCID: 89014103279069593771\r\n\r\nOK"
        char *terminator =
            strextract(uart_received_string, ccid, "#CCID: ", "\r\n");

        return terminator != NULL;
    }

    return 0u;
}


