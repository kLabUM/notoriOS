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


//pre-allocate some memory for the HTTP requests
#define MAX_HTTP_HEADER_LENFGTH 1000
char http_header[MAX_HTTP_HEADER_LENFGTH];
char http_body[DATA_MAX_KEY_LENGTH*DATA_MAX_VALUES*3+MAX_HTTP_HEADER_LENFGTH];
uint8 syncData();


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
    debug_start();
    
    modem_intilize();
    

    
    alarmMeasure = CreateAlarm(120u,ALARM_TYPE_SECOND,ALARM_TYPE_CONTINUOUS);
    timeToMeasure = 1;
    //alarmMeasure2 = CreateAlarm(10,ALARM_TYPE_SECOND,ALARM_TYPE_CONTINUOUS);
    //timeToMeasure = 0;
    
    
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
        timeToMeasure = syncData();//will return 0 when done sending data
    }
    //if(timeToMeasure2){
        //dosomething
    //}
    
    //check modem state machine 
    uint8 modem_status = modem_process_tasks();
    
   
    return 0u +  modem_status;  
}






// ==============================================
// sippin on gin and juice, Laid back (with my mind on my money and my money on my mind)
//      - Snoop Dogg (1993)
//
// Put all hardware to sleep and go into low power mode to conserve Amps
// ==============================================
void LayBack()
{
    
    debug_sleep();
    modem_sleep();
    // Prepares system clocks for the Sleep mode
    CyPmSaveClocks();
    
    // Switch to the Sleep Mode for the other devices:
    //  - PM_SLEEP_TIME_NONE: wakeup time is defined by Sleep Timer
    //  - PM_SLEEP_SRC_CTW :  wakeup on CTW sources is allowed
     //  - If real-time clock is used, it will also wake the device
    //can also wake up from a pin intterup (PICU). This is useful with the moderm UART
    //CyPmHibernate();
    //CyPmSleep(PM_SLEEP_TIME_NONE, PM_SLEEP_SRC_CTW & PM_SLEEP_SRC_PICU);
    CyPmSleep(PM_SLEEP_TIME_NONE, PM_SLEEP_SRC_CTW);
  
    // Restore clock configuration
    CyPmRestoreClocks();
    debug_wakeup();
    modem_wakeup();
    
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

// ==============================================
// Dropping bombs on your moms
//      - Ice Cube (1992)
//
// This is the global testing function, which runs all relevant tests
// If you make a new components, it could be good to call its tester function here 
// ==============================================
void ChickityCheckYourselfBeforeYouWreckYourself(){

   syncData();
    
    // test_t t_modem = modem_test();
    test_t t_influx = influx_test();
    printTestStatus(t_influx);
    test_t t_level_sensor = level_sensor_test();//test level sensor  
    printTestStatus(t_level_sensor);
    test_t t_sd_card = SD_card_test();
    printTestStatus(t_sd_card);
    test_t t_voltages = voltages_test();
    printTestStatus(t_voltages);
  
    int c = 0;
    c++;
    //test modem
    //test SD card
    //analog: test battery voltage, solar voltage, and solar current
      
  
}


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    ReadyOrNot();
    
    //run tests for all components on bootup
    ChickityCheckYourselfBeforeYouWreckYourself();
   
    for(;;)
    {
       if( ! WorkWorkWorkWorkWorkWork() )
       {
            LayBack();
       }
    }
}



uint8 syncData(){
    
    /*
    //at_write_command("AT#SCFG?\r","OK",1000);
    uint8 check = 0;
    //"AT#SD=,0,80,\”www.google.com\”,0,0,0\r"
    check = at_write_command("AT#SD=1,0,8086,\"data.open-storm.org\",0,0,1\r","OK",10000u);
    check = at_write_command("AT#SSEND=1\r\n",   ">", 1000u);
    check = at_write_command("POST /write?db=ARB&u=generic_node&p=MakeFloodsCurrents HTTP/1.1\r\nHost: data.open-storm.org:8086\r\nConnection: Close\r\nContent-Length: 39\r\nContent-Type: plain/text\r\n\r\nmaxbotix_depth,node_id=GGB000 value=111\r\n\r\n\032", "NO CARRIER", 10000u);
    */
    
    LED_Write(1u);
    CyDelay(100u);
    LED_Write(0u);
    
    http_header[0] = '\0';
    http_body[0] = '\0';
    char *host = "data.open-storm.org";
    int port = 8086;
    char *base = "write";
    char *user = "generic_node";
    char *pass = "MakeFloodsCurrents";
    char *database = "ARB";
    char *node_id = "GGB000";
    char route[100];
    
    //push some fake data
    pushData("maxbotix_depth","2000",12345);
    
    //construct HTPP request
    printNotif(NOTIF_TYPE_EVENT,"Begin HTTP test.");
    
    construct_influx_route(route,base,user,pass,database);
    printNotif(NOTIF_TYPE_EVENT,"HTTP route: %s", route);
    
    construct_influx_write_body(http_body,node_id);
    printNotif(NOTIF_TYPE_EVENT,"Full HTTP body: %s", http_body);
    
    construct_generic_HTTP_request(http_header,http_body,host,route,port,"POST","Close","",0,"1.1");
    printNotif(NOTIF_TYPE_EVENT,"HTTP header: %s", http_header);

        

    //create request body, in this case influx
    //place body into HTTP request header
    //fire up modem and get it out
       
     /*   
     if(modem_get_state() == MODEM_STATE_OFF){
            
            modem_power_up();
      }else if(modem_get_state() == MODEM_STATE_READY){
            printNotif(NOTIF_TYPE_EVENT,"Modem is ready.");
            syncData();
            int send_time = (int)(getTimeStamp()-(int32)modem_start_time_stamp);
            modem_power_down();
            return 0u;
      }
      */
      return 1;//not done yet
}



/* [] END OF FILE */
