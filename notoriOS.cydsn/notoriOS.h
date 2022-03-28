/* ========================================
 *
 * OPEN-STORM.ORG
 *
 * ========================================
*/

#include "project.h"
#include "level_sensor.h"
#include "sd_card.h"
#include "voltages.h"
#include "debug.h"
#include "modem.h"
#include "data.h"
#include "influx.h"
#include "diss_oxy.h"
#include "temperature.h"
#include "commit.h"
#include <math.h>       // designed for basic mathematical operations. Most of the functions involve the use of floating point numbers. 

// Pre-allocate some memory for the HTTP requests
#define MAX_HTTP_HEADER_LENGTH 1000
#define MAX_ROUTE_LENGTH 200
#define MAX_BYTES_SENT 500
char http_request[DATA_MAX_KEY_LENGTH*DATA_MAX_VALUES*3+MAX_HTTP_HEADER_LENGTH];
char http_body[DATA_MAX_KEY_LENGTH*DATA_MAX_VALUES*3+MAX_HTTP_HEADER_LENGTH];
char http_route[MAX_ROUTE_LENGTH];

// Define alarm variables
#define ALARM_TYPE_SECOND       0u
#define ALARM_TYPE_MINUTE       1u
#define ALARM_TYPE_HOUR         2u
#define ALARM_TYPE_DAY          3u
#define ALARM_TYPE_WEEK         4u
#define ALARM_TYPE_MONTH        5u
#define ALARM_TYPE_YEAR         6u
#define ALARM_TYPE_CONTINUOUS   7u

// Generally useful macros
#define ON  1u
#define  OFF 0u

// Define max number of alarms that can be set
#define MAX_NUM_ALARMS 5u

// Define alarm data structure
typedef struct
{
    uint16 countDownValue;          //how many ticks.
    int16 currentCountDownValue;   //current value -- resets on 0
    uint8 countDownType;            //what time unit is used to do the counting (e.g. second, minutes, etc)
    uint8 countDownResetCondition;  //Is this a continuous countdown, or does it reset (e.g. every hour for even intervals)
    //void (*callbackFuc)();          //callback function that will be called when timer fires
    
} volatile alarm;

// Define system info data structure
typedef struct
{
    modem_info_t *modem_info;
    char chip_uniqueId[24]; 
    
}  sys_info_t;

// Create system_info variable of sys_info_t data type
sys_info_t system_info;

// Create data structure for system setting's
typedef struct
{
    // This is the site ID --where in the field the node is located (e.g ARB10)
    //char node_id[20];
    // End-point config
    char ep_host[100];
    int ep_port;
    //char ep_user[50];
    //char ep_pswd[100];
    //char ep_database[50];
    //char commit_hash[100];
    
}  sys_settings_t;

// Creates variable system_settings of structure type sys_settings_t
sys_settings_t system_settings;

/*//to be implemented
uint8 num_alarms;
uint8 num_repeating_alarms;
alarm alarms[MAX_NUM_ALARMS];
void InitAlarms();
uint8 ScheduleTask(uint16 countDownValue,uint8 countDownType,void (*callback));
uint8 ScheduleRepeatingTask(uint16 countDownValue,uint8 countDownType,void (*callback));
*/

// Define alarm functions
alarm CreateAlarm(uint16 countDownValue,uint8 countDownType,uint8 countDownResetCondition);
uint8 AlarmReady(alarm * alarmToBeUpdated, uint8 alarmType);
void ResetAlarm(alarm * alarmToBeReset);

// Sets the system time using epoch timestamp
uint8 setTime(long timeStamp);

// Define main functions
void ReadyOrNot();
int  WorkWorkWorkWorkWorkWork();
void LayBack();
void AyoItsTime(uint8 alarmType);
void ChickityCheckYourselfBeforeYouWreckYourself();
uint8 upgraded(char *host, int port,char *route);

// Global variables
alarm alarmMeasure;
uint8 timeToMeasure;
alarm alarmSync;
uint8 timeToSync;
uint8 timeToSycnRemoteParams;
uint8 hey;

/* [] END OF FILE */
