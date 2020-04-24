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
#include "influx.h"

#define ALARM_TYPE_SECOND       0u
#define ALARM_TYPE_MINUTE       1u
#define ALARM_TYPE_HOUR         2u
#define ALARM_TYPE_DAY          3u
#define ALARM_TYPE_WEEK         4u
#define ALARM_TYPE_MONTH        5u
#define ALARM_TYPE_YEAR         6u
#define ALARM_TYPE_CONTINUOUS   7u

//generallu usefull macros
#define ON  1u
#define  OFF 0u


#define MAX_NUM_ALARMS 5u
//uised to create alarms, which fire when a tasks needs doing

typedef struct
{
    uint16 countDownValue;          //how many ticks.
    uint16 currentCountDownValue;   //current value -- resets on 0
    uint8 countDownType;            //what time unit is used to do the counting (e.g. second, minutes, etc)
    uint8 countDownResetCondition;  //Is this a continuous countdown, or does it reset (e.g. every hour for even intervals)
    //void (*callbackFuc)();          //callback function that will be called when timer fires
    
} volatile alarm;

/*//to be implemented
uint8 num_alarms;
uint8 num_repeating_alarms;
alarm alarms[MAX_NUM_ALARMS];
void InitAlarms();
uint8 ScheduleTask(uint16 countDownValue,uint8 countDownType,void (*callback));
uint8 ScheduleRepeatingTask(uint16 countDownValue,uint8 countDownType,void (*callback));
*/

alarm CreateAlarm(uint16 countDownValue,uint8 countDownType,uint8 countDownResetCondition);
uint8 AlarmReady(alarm * alarmToBeUpdated, uint8 alarmType);
void ResetAlarm(alarm * alarmToBeReset);




void ReadyOrNot();
int  WorkWorkWorkWorkWorkWork();
void LayBack();
void AyoItsTime(uint8 alarmType);
void ChickityCheckYourselfBeforeYouWreckYourself();







/* [] END OF FILE */
