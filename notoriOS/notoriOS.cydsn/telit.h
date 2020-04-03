/* ========================================
 * ========================================
*/

#ifndef TELIT_H
#define TELIT_H   
    
#include <project.h>    

typedef enum{
    EVENT_CHECK_UART,
    EVENT_CHECK_NETWORK,
    EVENT_CHECK_PDP,
    EVENT_ACTIVATE_PDP,
    EVENT_REQUEST_MEID,
    EVENT_REQUEST_TIME,
    EVENT_PARSE_TIME,
    EVENT_OPEN_SOCKET,
    EVENT_REQUEST_SOCKET_STATUS,
    EVENT_PARSE_SOCKET_STATUS,
    EVENT_INIT_SEND,
    EVENT_SEND_MESSAGE,
    EVENT_RECEIVE_MESSAGE,
    EVENT_PARSE_MESSAGE,
    EVENT_CLOSE_SOCKET    
}MODEM_EVENTS_T;
    
typedef enum{
    STATE_AT_CMD_SENT,
    STATE_ECHO_CORRECT,
    STATE_ECHO_NOT_FOUND,
    STATE_ECHO_TIMEOUT
}MODEM_STATE_T;

typedef enum{
    ACTION_REQUEST_TIME,
    ACTION_PARSE_TIME,
    ACTION_CHECK_NETWORK,
    ACTION_CHECK_PDP,
    ACTION_ACTIVATE_PDP,
    ACTION_SOCKET_DIAL,
    ACTION_REQUEST_SOCKET_STATUS,
    ACTION_PARSE_SOCKET_STATUS,
    ACTION_INQUIRE_INDEX,
    ACTION_REQUEST_MEID,
    ACTION_CHECK_DEVICE_INDEX,
    ACTION_INIT_SEND,
    ACTION_SEND_STRING,
    ACTION_RECEIVE_MESSAGE,
    ACTION_PARSE_MESSAGE,
    ACTION_CLOSE_SOCKET,
}MODEM_ACTION_T;


typedef struct{
    MODEM_EVENTS_T modem_event;
    MODEM_STATE_T modem_state;
    MODEM_ACTION_T modem_action;    
    uint8_t echo_counter;
    uint8_t transaction_error_counter;
    char expected_response[8];
    uint8_t connection;
}MODEM_DESCRIPTOR_T;

typedef struct{    
    MODEM_EVENTS_T modem_event;
    MODEM_STATE_T modem_state;
    MODEM_ACTION_T modem_action; 
}EVENT_NODE_T;


#define MAX_ECHO_COUNT    250

uint8_t write_modem_echo();
void modem_check_state();
uint8_t at_write_command(char* commands, char* expected_response,uint32_t timeout);
uint8_t parse_meid(char * meid);
void modem_message();
char *url_encode(char *str);
uint8 parse_http_status(char* http_status, char* version, char* status_code, char* phrase);
uint8_t modem_startup();  
uint8_t modem_power_on();
void modem_scheduler_socket_dial();
uint8_t modem_check_signal_quality(int *rssi, int *fer);
uint8_t modem_report_hardware_identification();
uint8_t modem_report_software_version();
uint8_t modem_set_interface_style();
uint8 modem_set_flow_control(uint8 param);
uint8_t modem_report_product_identification();
void construct_generic_request(char* send_str, char* body, char* host, char* route,
                               int port, char* method, char* connection_type,
	                           char *extra_headers, int extra_len, char* http_protocol);

#endif


