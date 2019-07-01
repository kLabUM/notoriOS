/* ========================================
 *
 * OPEN-STORM.ORG
 * notoriOS
 * A non-premptive operating system for node.
 *
 * "To all the teachers that told me I'd never amount to nothin"
 *      -Notorious BIG (1994)
 *
 * ========================================
*/
#include "project.h"
#include "notoriOS.h"


//global variables
alarm alarmMeasure;
uint8 timeToMeasure;
alarm alarmMeasure2;
uint8 timeToMeasure2;


// This function must always be called (when the Sleep Timer's interrupt
// is disabled or enabled) after wake up to clear the ctw_int status bit.
// It is required to call SleepTimer_GetStatus() within 1 ms (1 clock cycle
// of the ILO) after CTW event occurred.
CY_ISR(Wakeup_ISR) {
    SleepTimer_GetStatus();
}


// ==============================================
// Ready or not, here I come, you can't hide
// Gonna find you and take it slowly
//      - The Fugees (1996)
//
// Initialize and critical hardware or routines 
// ==============================================
void ReadyOrNot()
{
    isr_SleepTimer_StartEx(Wakeup_ISR);// Start Sleep ISR
    SleepTimer_Start();             // Start SleepTimer Component
    RTC_WriteIntervalMask(0b11111111);
    RTC_Start();
    
    alarmMeasure = CreateAlarm(1,ALARM_TYPE_SECOND,ALARM_TYPE_CONTINUOUS);
    timeToMeasure = 0;
    alarmMeasure2 = CreateAlarm(5,ALARM_TYPE_SECOND,ALARM_TYPE_CONTINUOUS);
    timeToMeasure = 0;
    
}

// ==============================================
// You see me I be work, work, work, work, work, work 
//      - Rhiana, feat Drake (2016)
//
// Prorcesses core tasks
// ==============================================
int WorkWorkWorkWorkWorkWork()
{
    if(timeToMeasure){
        LED_Write(1u);
        CyDelay(500u);
        LED_Write(0u);
        timeToMeasure = 0u;
    }
    if(timeToMeasure2){
        LED_Write(1u);
        CyDelay(1000u);
        LED_Write(0u);
        CyDelay(1000u);
        LED_Write(1u);
        CyDelay(1000u);
        LED_Write(0u);
        CyDelay(1000u);
        timeToMeasure2 = 0u;
    }
    
    return 0;   
}


// ==============================================
// sippin on gin and juice, Laid back (with my mind on my money and my money on my mind)
//      - Snoop Dogg (1993)
//
// Put all hardware to sleep and go into low power mode to conserve Amps
// ==============================================
void LayBack()
{
    // Prepares system clocks for the Sleep mode
    CyPmSaveClocks();
    
    // Switch to the Sleep Mode for the other devices:
    //  - PM_SLEEP_TIME_NONE: wakeup time is defined by Sleep Timer
    //  - PM_SLEEP_SRC_CTW :  wakeup on CTW sources is allowed
     //  - If real-time clock is used, it will also wake the device
    CyPmSleep(PM_SLEEP_TIME_NONE, PM_SLEEP_SRC_CTW);
  
    // Restore clock configuration
    CyPmRestoreClocks();
    
}


// ==============================================
// I never sleep, 'cause sleep is the cousin of death
//      - NAS (1994)
//
// This is the scheduler. It's called from various interrupts and timers,
// ...in particular from fuctions with in the real-tiem clock interrupts (RTC_INT.c)
// It will add tasks to the work queue,
// which will be executed eventually by WorkWorkWorkWorkWorkWork()
// ==============================================
void AyoItsTime(uint8 alarmType)
{

    if(AlarmReady(&alarmMeasure,alarmType))
    {
         //create new task and pass off to workworkworkworkwork()
        //pass off to work work
        timeToMeasure = 1u;
    }
    if(AlarmReady(&alarmMeasure2,alarmType))
    {
         //create new task and pass off to workworkworkworkwork()
        //pass off to work work
        timeToMeasure2 = 1u;
    }
    
}
        
uint8 AlarmReady(alarm * alarmToBeUpdated, uint8 alarmType)
{
    alarmToBeUpdated->currentCountDownValue--;
    if(alarmToBeUpdated->currentCountDownValue == 0){
        ResetAlarm(alarmToBeUpdated);
        return 1u;
    } 
    else if(alarmToBeUpdated->countDownResetCondition == alarmType){
        ResetAlarm(alarmToBeUpdated);
    }
    
    return 0u;
}

void ResetAlarm(alarm * alarmToBeReset)
{
    alarmToBeReset->currentCountDownValue = alarmToBeReset->countDownValue;
}

// ==============================================
// Creates a new alarm, which will be handeled by another cutions
// ==============================================

alarm CreateAlarm(uint16 countDownValue, uint8 countDownType,uint8 countDownResetCondition)
{
    alarm newAlarm;
    
    newAlarm.countDownValue = countDownValue;
    newAlarm.countDownType = countDownType;
    newAlarm.countDownResetCondition = countDownResetCondition;
    
    //if you want a reccuring alarm (e.g. resets freshly each hour), then add
    //account for already exprired time using the RTC. Otherwise, set to the countDownValue
    newAlarm.currentCountDownValue = countDownValue;
    
    return newAlarm;
    
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    ReadyOrNot();

    for(;;)
    {
       if( ! WorkWorkWorkWorkWorkWork() )
       {
            LayBack();
       }
    }
}






/* [] END OF FILE */