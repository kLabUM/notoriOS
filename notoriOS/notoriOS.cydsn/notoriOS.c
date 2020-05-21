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
alarm alarmSync;
uint8 timeToSync;
uint8 timeToSycnRemoteParams;


// This function must always be called (when the Sleep Timer's interrupt
// is disabled or enabled) after wake up to clear the ctw_int status bit.
// It is required to call SleepTimer_GetStatus() within 1 ms (1 clock cycle
// of the ILO) after CTW event occurred.
CY_ISR(Wakeup_ISR) {
    SleepTimer_GetStatus();
}



uint8 syncData();               //syncs data with server 
uint8 configureRemoteParams();  //syncs RTC with cell network and obtains meta-DB params
uint8 makeMeasurements();

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
   
    
    //collect system info and store in struct (modem ID, silicon ID, etc)
      /* An array of char elements for the resulting string to be stored */
    
    
    //get unique ID of PSOC chip, this can be concatenated with the MODEM IDs to generate a unique string for the entire system
    uint32 uniqueId[2];
    CyGetUniqueId(uniqueId); 
    snprintf(system_info.chip_uniqueId,sizeof(system_info.chip_uniqueId),"%X-%X",(unsigned int)uniqueId[0],(unsigned int)uniqueId[1]);
    //the modem will track/popualte it's own IDs, so just point to them for now
    system_info.modem_info = &modem_info;
    
    //configure server endpoints -- these should obviosuly be provided  remotely by the meta data-base server
    snprintf(system_settings.ep_host,sizeof(system_settings.ep_host),"%s","data.open-storm.org");
    system_settings.ep_port = 8086;
    snprintf(system_settings.ep_user,sizeof(system_settings.ep_user),"%s","generic_node");
    snprintf(system_settings.ep_pswd,sizeof(system_settings.ep_pswd),"%s","MakeFloodsCurrents");
    snprintf(system_settings.ep_database,sizeof(system_settings.ep_database),"%s","ARB");
    snprintf(system_settings.node_id,sizeof(system_settings.node_id),"%s","GGB000");
        
    
    
    alarmMeasure = CreateAlarm(1u,ALARM_TYPE_MINUTE,ALARM_TYPE_CONTINUOUS);
    timeToMeasure = 1u;
    
    alarmSync = CreateAlarm(5u,ALARM_TYPE_MINUTE,ALARM_TYPE_CONTINUOUS);
    timeToSync = 1u;
    
    timeToSycnRemoteParams = 1u;
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
    
    //the priority of these tasks is determined by the if statements
    //for example, taking measurements should always precede data transmisison, even if both fire
    //getting meta-DB info and system configs precedes all
    //that said, the modem is always qeried to check if it needs anything
    
    if(timeToSycnRemoteParams){
        timeToSycnRemoteParams = configureRemoteParams();//will return 0 when done sending data
    }
    else if(timeToMeasure){
        timeToMeasure = makeMeasurements();//will return 0 when done sending data
    }else if(timeToSync){
            timeToSync = syncData();
    }
    
    //printNotif(NOTIF_TYPE_EVENT,"Measure CNT %d, Sync CNT %d, Data Wheel CNT %d",alarmMeasure.currentCountDownValue,alarmSync.currentCountDownValue,sizeOfDataStack());
    
    
    //check modem state machine on every loop

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
    
  
    
    modem_sleep();
    // Prepares system clocks for the Sleep mode

    
    //important, call this last because we want to sleep uart before shutting down
 
    debug_sleep();
    
    CyPmSaveClocks();
    
    // Switch to the Sleep Mode for the other devices:
    //  - PM_SLEEP_TIME_NONE: wakeup time is defined by Sleep Timer
    //  - PM_SLEEP_SRC_CTW :  wakeup on CTW sources is allowed
     //  - If real-time clock is used, it will also wake the device
    //can also wake up from a pin intterup (PICU). This is useful with the moderm UART
    //CyPmHibernate();
   //CyPmSleep(PM_SLEEP_TIME_NONE, PM_SLEEP_SRC_CTW & PM_SLEEP_SRC_PICU);

    CyPmSleep(PM_SLEEP_TIME_NONE, PM_SLEEP_SRC_CTW & PM_SLEEP_SRC_ONE_PPS);

  
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
         //flag new task as "ready" and pass off to workworkworkworkwork()
        timeToMeasure = 1u;
        //printNotif(NOTIF_TYPE_EVENT,"Measure Alarm");
        
    }
    if(AlarmReady(&alarmSync,alarmType))
    {
        //create new task and pass off to workworkworkworkwork()
        timeToSync = 1u;
         //printNotif(NOTIF_TYPE_EVENT,"Sync Alarm");
    }
    
}
        
uint8 AlarmReady(alarm * alarmToBeUpdated, uint8 alarmType)
{
    if(alarmToBeUpdated->countDownType == alarmType){
       
       alarmToBeUpdated->currentCountDownValue--;
       if(alarmToBeUpdated->currentCountDownValue <= 0){
            ResetAlarm(alarmToBeUpdated);
            return 1u;
        } 
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

    
    // test_t t_modem = modem_test();
    test_t t_influx = influx_test();
    printTestStatus(t_influx);
    test_t t_level_sensor = level_sensor_test();//test level sensor  
    printTestStatus(t_level_sensor);
    test_t t_voltages = voltages_test();
    printTestStatus(t_voltages);
    test_t t_sd_card = SD_card_test();
    printTestStatus(t_sd_card);
  
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

//syncs data with server
//retunrns zero when done
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
    
    //if there's no data, not need to do anything
    if(sizeOfDataStack() == 0){
        return 0u;
    }
    
    //create request body, in this case influx
    //place body into HTTP request header
    //fire up modem and get it out
       
     
     if(modem_get_state() == MODEM_STATE_OFF){
            
            modem_power_up();
            //should put this on a max_try counter, so we don't just keep trying to connect over and over
            
      }else if(modem_get_state() == MODEM_STATE_READY){
            printNotif(NOTIF_TYPE_EVENT,"Modem is ready.");
            
            
            http_request[0] = '\0';
            http_body[0] = '\0';
            http_route[0] = '\0';
            char *base = "write";
       
            
            
            //construct HTPP request
            printNotif(NOTIF_TYPE_EVENT,"Begin HTTP post.");
            
            construct_influx_route(http_route,base,system_settings.ep_user,system_settings.ep_pswd,system_settings.ep_database);
            printNotif(NOTIF_TYPE_EVENT,"HTTP route: %s", http_route);
            
            construct_influx_write_body(http_body,system_settings.node_id);
            
            
            construct_generic_HTTP_request(http_request,http_body,system_settings.ep_host,http_route,system_settings.ep_port,"POST","Close","",0,"1.1");
            printNotif(NOTIF_TYPE_EVENT,"Full HTTP Request: %s", http_request);
            
            
            
            
           
            //push request to modem and escaope with <ctrl+z> escape sequence
            //open port and begin command line sequence
            char portConfig[200];
            uint8 status = 0u;
            snprintf(portConfig,sizeof(portConfig),"AT#SD=1,0,%d,\"%s\",0,0,1\r",system_settings.ep_port,system_settings.ep_host);
            printNotif(NOTIF_TYPE_EVENT,"%s",portConfig);
            status = at_write_command(portConfig,"OK",10000u);
            status = at_write_command("AT#SSEND=1\r\n",   ">", 1000u);
            //append <ctrl+z> escape sequence to http_request to exit modem command line
            strncat(http_request, "\032", 1); 
            status = at_write_command(http_request, "NO CARRIER", 5000u);
            
            //read recevied buyffe
            //a good write will return code "204 No Content"
            status = at_write_command("AT#SRECV=1,1000\r","204 No Content",5000u);
            //printNotif(NOTIF_TYPE_EVENT,"Received SRECV: %s",uart_received_string);
           
      
            //if it worked, clear the queue and clock how long the end-to-end tx took
            if(status == 1u){
                Clear_Data_Stack();
                int send_time = (int)(getTimeStamp()-(int32)modem_start_time_stamp);
                char s_send_time[10];
                snprintf(s_send_time,sizeof(s_send_time),"%d",send_time);
                pushData("modem_tx_time",s_send_time,getTimeStamp());
            }
            
           
            
       
             
            //get time, and if it looks good, set the RTC with it
            long network_time = modem_get_network_time();
            if(network_time != 0){
               setTime(network_time);
            }
            
            //if transmitted, flush the data stack and shut down modem
            //if not -- keep the statck and try next time

            
            modem_power_down();
            
            return 0u;
      }
      
      return 1;//not done yet
}

//syncs with cell-tower clock and check meta-DB
//retunrns zero when done
uint8 configureRemoteParams(){
    
    if(modem_get_state() == MODEM_STATE_OFF){
            
        modem_power_up();
            
    }else if(modem_get_state() == MODEM_STATE_READY){
        
        //this is where we will interfasce with some sort of broker that assings node IDs
        
        //get time, and if it looks good, set the RTC with it
        long network_time = modem_get_network_time();
        if(network_time != 0){
            setTime(network_time);
        }
        
        //get GPS coordinates
        //gps_t gps = modem_get_gps_coordinates();
        
       modem_power_down();
        return 0u;
        
    }
    
    return 1u;
}

//makes sensor measurments 
//retunrns zero when done
uint8 makeMeasurements(){
   
    level_sensor_t m_level_sensor;
    voltage_t m_voltage;
    
    
    //clock time
    long timeStamp = getTimeStamp();
    //holds string for value that will be written 
    char value[DATA_MAX_KEY_LENGTH];
    
    m_level_sensor = level_sensor_take_reading();
 
   
    if(m_level_sensor.num_valid_readings > 0){
        snprintf(value,sizeof(value),"%d",m_level_sensor.level_reading);
        printNotif(NOTIF_TYPE_EVENT,"maxbotix_depth=%s",value);
        pushData("maxbotix_depth",value,timeStamp);
    }else{
        pushData("maxbotix_depth","error",timeStamp);
    }
    
    
    m_voltage = voltage_take_readings();
    if(m_voltage.valid){
        snprintf(value,sizeof(value),"%.2f",m_voltage.voltage_battery);
        printNotif(NOTIF_TYPE_EVENT,"v_bat=%s",value);
        pushData("v_bat",value,timeStamp);
    }else{
        pushData("v_bat","error",timeStamp);
    }
    
    return 0u;
}


uint8 setTime(long timeStamp){
    
    if(timeStamp <=0){
        printNotif(NOTIF_TYPE_ERROR,"Error setting sytem time.");
        return 0;  
    }
    
    struct tm * t;
    //time(&timeStamp);
    t = gmtime(&timeStamp);
    
    RTC_TIME_DATE localTime;
    localTime.Sec = t->tm_sec;
    localTime.Min = t->tm_min;
    localTime.Hour = t->tm_hour;
    localTime.Year = t->tm_year + 1900;
    localTime.Month = t->tm_mon;
    localTime.DayOfMonth = t->tm_mday;

    RTC_WriteTime(&localTime);
   
    return 1;
}




/* [] END OF FILE */
