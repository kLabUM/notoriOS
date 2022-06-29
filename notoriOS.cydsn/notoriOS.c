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


// This function must always be called (when the Sleep Timer's interrupt
// is disabled or enabled) after wake up to clear the ctw_int status bit.
// It is required to call SleepTimer_GetStatus() within 1 ms (1 clock cycle
// of the ILO) after CTW event occurred.
CY_ISR(Wakeup_ISR) {
    // Returns the state of the Sleep Timer's status register, and clears the pending interrupt status bit. 
    SleepTimer_GetStatus();
}


uint8 syncData();               // Syncs data with server 
uint8 configureRemoteParams();  // Syncs RTC with cell network and obtains meta-DB params
uint8 makeMeasurements();       // Takes sensor measurements
char  Data_fileName[30] = "data.txt";
uint8 try_counter;


// ==============================================
// Ready or not, here I come, you can't hide
// Gonna find you and take it slowly
//      - The Fugees (1996)
//
// Initialize and critical hardware or routines 
// ==============================================
void ReadyOrNot()
{
    
    isr_SleepTimer_StartEx(Wakeup_ISR); // Start Sleep ISR
    SleepTimer_Start();                 // Start SleepTimer Component
    
    RTC_WriteIntervalMask(0b11111111);  // Configures what interval handlers will be called from the RTC ISR (0b means binary #)
    RTC_Start();                        // Enables the RTC component
    debug_start();                      // Start UART debug
    
    modem_initialize();                 // Initialize the modem
    
    updatable_parameters_initialize();  // Initialize sampling, measurement, and debug frequencies 
   
    
    // Collect system info and store in struct (modem ID, silicon ID, etc)
      /* An array of char elements for the resulting string to be stored */
    
    
    // Get unique ID of PSOC chip, this can be concatenated with the MODEM IDs to generate a unique string for the entire system
    uint32 uniqueId[2];
    // Returns the 64-bit unique ID of the device.
    CyGetUniqueId(uniqueId); 
    snprintf(system_info.chip_uniqueId,sizeof(system_info.chip_uniqueId),"%X-%X",(unsigned int)uniqueId[0],(unsigned int)uniqueId[1]);
    // The modem will track/popualte it's own IDs, so just point to them for now
    system_info.modem_info = &modem_info;
    
    // Configure server endpoints -- these should obviosuly be provided  remotely by the meta data-base server
    // testing server - travis and meagan june of 2022
    snprintf(system_settings.ep_host,sizeof(system_settings.ep_host),"%s","ec2-18-222-229-124.us-east-2.compute.amazonaws.com");
    
    // production server
    //snprintf(system_settings.ep_host,sizeof(system_settings.ep_host),"%s","malcom.open-storm.org");
    

    system_settings.ep_port = 5000;
    // Don't need the following for new nodes
    //snprintf(system_settings.ep_user,sizeof(system_settings.ep_user),"%s","generic_node");
    //snprintf(system_settings.ep_pswd,sizeof(system_settings.ep_pswd),"%s","MakeFloodsCurrents");
    //snprintf(system_settings.ep_database,sizeof(system_settings.ep_database),"%s","ARB");
    //snprintf(system_settings.node_id,sizeof(system_settings.node_id),"%s","GGB000");
        
    
    // Create a continuous alarm called alarmMeasure that triggers every 10 min (default) to take measurements
    alarmMeasure = CreateAlarm(updatable_parameters.measure_time,ALARM_TYPE_MINUTE,ALARM_TYPE_CONTINUOUS);
    timeToMeasure = 1u;
    // Create a continuous alarm called alarmSync that triggers every 60 min (default) to sync the data to database
    alarmSync = CreateAlarm(updatable_parameters.sync_time,ALARM_TYPE_MINUTE,ALARM_TYPE_CONTINUOUS);
    timeToSync = 1u;
    
    timeToSycnRemoteParams = 0u;//set to 1 if you want to get modem IDs and time -- no need to do this if you run tests first
    
    // App Initializations
    alarmAppLED = CreateAlarm(updatable_parameters.App_LED_freq,ALARM_TYPE_MINUTE,ALARM_TYPE_CONTINUOUS);
    timeToAppLED = 1u;
    
    alarmLevelSensor = CreateAlarm(updatable_parameters.Level_Sensor_freq,ALARM_TYPE_MINUTE, ALARM_TYPE_CONTINUOUS);
    timetoLevelSensor = 1u;
    
    // Initialize the try counter to 0
    uint8 try_counter = 0;
}


// ==============================================
// You see me I be work, work, work, work, work, work 
//      - Rihanna, feat Drake (2016)
//
// Prorcesses core tasks
// ==============================================
int WorkWorkWorkWorkWorkWork()
{
    
    
    
    // The priority of these tasks is determined by the if statements
    // For example, taking measurements should always precede data transmisison, even if both fire
    // Getting meta-DB info and system configs precedes all
    // That said, the modem is always qeried to check if it needs anything
    
    // Checks to see if the timetoSyncRemoteParms flag is set
    if(timeToSycnRemoteParams){
        timeToSycnRemoteParams = configureRemoteParams(); // Will return 0 when done sending data
    }
    // Checks to see if the timetoMeasure flag is set
    else if(timeToMeasure){
        // on custom nodes makeMeasurements will just be battery voltages
        timeToMeasure = makeMeasurements(); // Will return 0 when done sending data
    }  
    // custom nodes
    else if(updatable_parameters.node_type == NODE_TYPE_CUSTOM){
        if(timeToAppLED){
            timeToAppLED = App_LED();
        }
        if(timetoLevelSensor){
            timetoLevelSensor = level_sensor();
        }
        // add other custom apps below
    }
    if(timeToSync){
        timeToSync = syncData();
    } 
    

    
    // Print the countdown to the next alarm
    //printNotif(NOTIF_TYPE_EVENT,"Measure CNT %d, Sync CNT %d, Data Wheel CNT %d",alarmMeasure.currentCountDownValue,alarmSync.currentCountDownValue,sizeOfDataStack());
    
    
    // Check modem state machine on every loop
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
  
    modem_sleep();  // Prepares system clocks for the Sleep mode
    
    // Important, call this last because we want to sleep uart before shutting down
    debug_sleep();  // Function to sleep UART debug
    
    CyPmSaveClocks(); // Called to prep system to sleep/hiberanate. Saves all states of clocking system.
    
    // Switch to the Sleep Mode for the other devices:
    //  - PM_SLEEP_TIME_NONE: wakeup time is defined by Sleep Timer
    //  - PM_SLEEP_SRC_CTW :  wakeup on CTW sources is allowed
    //  - If real-time clock is used, it will also wake the device
    //can also wake up from a pin intterup (PICU). This is useful with the moderm UART
    //CyPmHibernate();
    //CyPmSleep(PM_SLEEP_TIME_NONE, PM_SLEEP_SRC_CTW & PM_SLEEP_SRC_PICU);

    CyPmSleep(PM_SLEEP_TIME_NONE, PM_SLEEP_SRC_CTW & PM_SLEEP_SRC_ONE_PPS); // Puts the part into the Sleep state.

    CyPmRestoreClocks();    // Restore clock configuration.

    debug_wakeup();         // Function to wakeup UART debug.
    modem_wakeup();         // Function to wakeup modem.

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
    // Checks if the alarm is up and if so, sets the flag.
    if(AlarmReady(&alarmMeasure,alarmType))
    {
         // Flag new task as "ready" and pass off to workworkworkworkwork()
        timeToMeasure = 1u;
        //printNotif(NOTIF_TYPE_EVENT,"Measure Alarm");
        
    }
    if(AlarmReady(&alarmSync,alarmType))
    {
        // Create new task and pass off to workworkworkworkwork()
        timeToSync = 1u;
         //printNotif(NOTIF_TYPE_EVENT,"Sync Alarm");
    }
    if(AlarmReady(&alarmAppLED,alarmType))
    {
        // Create new task and pass off to workworkworkworkwork()
        timeToAppLED = 1u;
         //printNotif(NOTIF_TYPE_EVENT,"Sync Alarm");
    }
    if(AlarmReady(&alarmLevelSensor, alarmType)){
        timetoLevelSensor = 1u;
    }
    
}


// ==============================================
// Determines when an alarm is ready to trigger, which will be handeled by another cutions
// ==============================================
uint8 AlarmReady(alarm * alarmToBeUpdated, uint8 alarmType)
{
    // if the countDownType matches the alarmType
    if(alarmToBeUpdated->countDownType == alarmType){
       
        // In the Post-decrement old value is first used in a expression and then old value will be decrement by 1.
        // Decrement the count down value by 1
        alarmToBeUpdated->currentCountDownValue--;
        // If the current count is 0, then reset the alarm
        if(alarmToBeUpdated->currentCountDownValue <= 0){
            ResetAlarm(alarmToBeUpdated);
            return 1u;
        } 
    }
    
    return 0u; // Otherwise return 0
}

// ==============================================
// Resets an alarm, which will be handeled by other executions
// ==============================================
void ResetAlarm(alarm * alarmToBeReset)
{
    // resets the alarm when the current count down value equals the set alarm count down value
    alarmToBeReset->currentCountDownValue = alarmToBeReset->countDownValue;
}

// ==============================================
// Creates a new alarm, which will be handeled by another cutions
// ==============================================
alarm CreateAlarm(uint16 countDownValue, uint8 countDownType,uint8 countDownResetCondition)
{
    // Creates variable newAlarm of the data structure type alarm
    alarm newAlarm;
    
    // Defines the different params of an alarm: # of ticks, alarm type (sec, min, etc.), & continuous or not
    newAlarm.countDownValue = countDownValue;
    newAlarm.countDownType = countDownType;
    newAlarm.countDownResetCondition = countDownResetCondition;
    
    // If you want a reccuring alarm (e.g. resets freshly each hour), then add
    // Account for already exprired time using the RTC. Otherwise, set to the countDownValue
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

    //test_t t_influx = influx_test();
    //printTestStatus(t_influx);
    
    printNotif(NOTIF_TYPE_STARTUP,"-------------BEGIN TESTS---------------\n\n");
    
    // Test level sensor
    test_t t_level_sensor = level_sensor_test();  
    printTestStatus(t_level_sensor);
    
    // Test voltages
    test_t t_voltages = voltages_test();
    printTestStatus(t_voltages);
 
    // Test voltages
    test_t t_pressure = pressure_sensor_test();
    printTestStatus(t_pressure);
    
    // Test SD card
    test_t t_sd_card = SD_card_test();
    printTestStatus(t_sd_card);
    
    // Test modem
    test_t t_modem = modem_test();
    printNotif(NOTIF_TYPE_STARTUP,"MEID=%s, SIMID=%s, DEVICEID=%s",modem_info.imei,modem_info.sim_id,system_info.chip_uniqueId);
    printTestStatus(t_modem);
    
    printNotif(NOTIF_TYPE_STARTUP,"\n\n-------------END TESTS---------------\n\n");

}

// ==============================================
// Processes Core Tasks
// ==============================================
int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    ReadyOrNot(); // Initialize 
    
    ChickityCheckYourselfBeforeYouWreckYourself(); // Run tests for all components on bootup
   
    for(;;) // (;;) means an infinite loop
    {
        
        if( ! WorkWorkWorkWorkWorkWork() ) // Complete the work
        {
           LayBack(); // Lay back (sleep) when there isn't work
        }
    }
}

// ==============================================
// Syncs data with server
// Returns 0 when done
// ==============================================
uint8 syncData(){
    
    /*
    //at_write_command("AT#SCFG?\r","OK",1000);
    uint8 check = 0;
    //"AT#SD=,0,80,\”www.google.com\”,0,0,0\r"
    check = at_write_command("AT#SD=1,0,8086,\"data.open-storm.org\",0,0,1\r","OK",10000u);
    check = at_write_command("AT#SSEND=1\r\n",   ">", 1000u);
    check = at_write_command("POST /write?db=ARB&u=generic_node&p=MakeFloodsCurrents HTTP/1.1\r\nHost: data.open-storm.org:8086\r\nConnection: Close\r\nContent-Length: 39\r\nContent-Type: plain/text\r\n\r\nmaxbotix_depth,node_id=GGB000 value=111\r\n\r\n\032", "NO CARRIER", 10000u);
    */

    // Blink LED so we know it's about to sync data
    LED_Write(1u);
    CyDelay(100u);
    LED_Write(0u);

    
    // If there's no data, not need to do anything
    if(sizeOfDataStack() == 0){        
        return 0u;
        }
    
    // Create request body, in this case influx
    // Place body into HTTP request header
    // Fire up modem and get it out
     
    
    // Try up to 2 times to connect to the modem
    if(modem_get_state() == MODEM_STATE_OFF){

        try_counter++;
        	
        if(try_counter <= 2){
            modem_power_up();
        }else{
            try_counter = 0;
            return 0u;
            }
            
    }else if(modem_get_state() == MODEM_STATE_READY){
        printNotif(NOTIF_TYPE_EVENT,"Modem is ready.");
        
        http_request[0] = '\0';
        http_body[0] = '\0';
        http_route[0] = '\0';
        //char *base = "write";
        
        // Push cell strength data (RSRP: reference signal received power and RSCP: received signal code power)
        char s_rsrp[DATA_MAX_KEY_LENGTH];
        snprintf(s_rsrp,sizeof(s_rsrp),"%d",modem_stats.rsrp);
        pushData("rsrp", s_rsrp, getTimeStamp());
        
        char s_rscp[DATA_MAX_KEY_LENGTH];
        snprintf(s_rscp,sizeof(s_rscp),"%d",modem_stats.rscp);
        pushData("rscp", s_rscp, getTimeStamp());
        
        // Push syncData try_counter to know how many tries it tried to connect
        char c_try_counter[20];
        snprintf(c_try_counter,sizeof(c_try_counter),"%d",try_counter);
        pushData("try_counter",c_try_counter,getTimeStamp());
        
        // Get size of the actual data stack count and push it to the stack
        // If the buffer didn't overflow, then the data_count_sent is dataPointsInStack
        // Otherwise, if the buffer overflows, then the data_count_sent is DATA_MAX_COUNT
        if(buffer_overflow == false){
            uint16 data_count_sent = sizeOfDataStack();
            char c_data_count_sent[20];
            snprintf(c_data_count_sent,sizeof(c_data_count_sent),"%d",data_count_sent);
            pushData("data_count_sent",c_data_count_sent,getTimeStamp());
        }else{
            char c_data_count_sent[20];
            snprintf(c_data_count_sent,sizeof(c_data_count_sent),"%d",DATA_MAX_VALUES);
            pushData("data_count_sent",c_data_count_sent,getTimeStamp());
        }
        
        // Get size of the desired data stack count and push it to the stack
        uint16 data_count_desired = sizeOfDataStackDesired();
        char c_data_count_desired[20];
        // subtract 1 to disclude data_count_sent so that it focuses only on the actual data
        snprintf(c_data_count_desired,sizeof(c_data_count_desired),"%d", data_count_desired-1);
        pushData("data_count_desired",c_data_count_desired,getTimeStamp());
        
        // Construct HTPP request
        printNotif(NOTIF_TYPE_EVENT,"Begin HTTP post.");
        
        // Old influx API
        // Construct_influx_route(http_route,base,system_settings.ep_user,system_settings.ep_pswd,system_settings.ep_database);
        construct_malcom_route(http_route,"api/v1/write",modem_info.imei,CURRENT_COMMIT);
        printNotif(NOTIF_TYPE_EVENT,"HTTP route: %s", http_route);
        
        //OLD INFLUX API
        //construct_influx_write_body(http_body,system_settings.node_id);
        construct_malcom_body(http_body);
        construct_generic_HTTP_request(http_request,http_body,system_settings.ep_host,http_route,system_settings.ep_port,"POST","Close","",0,"1.1");
        printNotif(NOTIF_TYPE_EVENT,"Full HTTP Request: %s", http_request);
        
        // Push request to modem and escape with <ctrl+z> escape sequence
        // Open port and begin command line sequence
        char portConfig[200];
        uint8 status = 0u;
        
        snprintf(portConfig,sizeof(portConfig),"AT#SD=1,0,%d,\"%s\",0,0,1\r",system_settings.ep_port,system_settings.ep_host);
        printNotif(NOTIF_TYPE_EVENT,"%s",portConfig);
        status = at_write_command(portConfig,"OK",10000u);
        
        // create character pointer to chunk through the http_request
        char *chunk;
        // Set chunk to start where http_request starts
        chunk = http_request;
        // variable for the number of packets to send
        int8 packets;
        packets = ceil(nextafter((strlen(http_request)/MAX_BYTES_SENT),MAX_BYTES_SENT));
        printNotif(NOTIF_TYPE_EVENT, "http_request len: %d", strlen(http_request));
        printNotif(NOTIF_TYPE_EVENT, "# of packets: %d", packets);
        // while the size of the chunk of the data we are sending to the server is smaller than the total data that needs to be sent
        for(int8 packet = 0; packet < packets; packet++){
            // Create character array of 1000 characters for sending chunks of the http_request
            char http_chunk[1001];
            // AT command #SSEND= is an execution command that permits, while the module is in command mode, to send data through a connected socket.
            // To complete the operation, send Ctrl-Z char to exit
            status = at_write_command("AT#SSEND=1\r\n", ">", 1000u);
            // Print the first 1000 characters of http_request into http_chunk
            snprintf(http_chunk, MAX_BYTES_SENT, "%s",chunk);
            // Append 1 character "<ctrl+z> escape sequence" to http_request to exit modem command line
            strncat(http_chunk, "\032", 1); 
            printNotif(NOTIF_TYPE_EVENT,"chunk %d: %s", packet, http_chunk);
            // Send data to server
            if ((packet+1) != packets){
                status = at_write_command(http_chunk, "OK", 5000u);
                // Move the pointer forward 
                chunk = chunk + MAX_BYTES_SENT - 1;
            }else{
                status = at_write_command(http_chunk, "SRING", 5000u);
            }
            
        }
        
        // Read received buffer
        // A good write will return code "OS-OK"
        // AT command #SRECV= is an execution command that permits users to read data arrived through a connection socket
        // = 1 means the UPD information is enabled: data are read just until the end of the UDP datagram and the response carries information about the remote IP address and port and about the remaining bytes in the datagram.
        status = at_write_command("AT#SRECV=1,1000\r","OS-OK",5000u);
        //printNotif(NOTIF_TYPE_EVENT,"Received SRECV: %s",uart_received_string);
    
  
        // If it worked, clear the queue and clock how long the end-to-end tx took
        if(status == 1u){
            get_updated_parameters_from_malcom();
            Clear_Data_Stack();
            int send_time = (int)(getTimeStamp()-(int32)modem_start_time_stamp);
            char s_send_time[10];
            snprintf(s_send_time,sizeof(s_send_time),"%d",send_time);
            pushData("modem_tx_time",s_send_time,getTimeStamp());
        }else{
            printNotif(NOTIF_TYPE_ERROR, "OS-NO: Could not receive data from server.");
        }
         
        // Get time, and if it looks good, set the RTC with it
        long network_time = modem_get_network_time();
        if(network_time != 0){
           setTime(network_time);
        }
        
        // If transmitted, flush the data stack and shut down modem
        // If not -- keep the statck and try next time
        // Power down the modem -- completely "kill" modem to conserve power
        modem_power_down();
        
        try_counter = 0u;
        return 0u;
    }
      
      return 1u; // Not done yet
}

// ==============================================
// Syncs with cell-tower clock and check meta-DB
// Returns 0 when done
// ==============================================
uint8 configureRemoteParams(){
    
    if(modem_get_state() == MODEM_STATE_OFF){
        // This puts all the modem points into a state that won't leak power    
        modem_power_up();
            
    }else if(modem_get_state() == MODEM_STATE_READY){
        
        // This is where we will interfasce with some sort of broker that assings node IDs
        
        // Get time, and if it looks good, set the RTC with it
        long network_time = modem_get_network_time();
        if(network_time != 0){
            setTime(network_time);
        }
        
        // Get GPS coordinates
        //gps_t gps = modem_get_gps_coordinates();
        // Power down the modem -- completely "kill" modem to conserve power
        modem_power_down();
        return 0u;
        
    }
    
    return 1u;
}


// ==============================================
// Make sensor measurements, which will be handeled by another cutions
// Returns 0 when finished
// ==============================================
uint8 makeMeasurements(){
    
    // Get clock time and save to timeStamp
    long timeStamp = getTimeStamp();
    char c_timeStamp[32];
    snprintf(c_timeStamp,sizeof(c_timeStamp),"%ld",timeStamp);
    
    // Holds string for value that will be written 
    char value[DATA_MAX_KEY_LENGTH];
    
    // who i am determines what I'm going to do 
    // this is primarily for interpretability on grafana dashboards when developing custom nodes
    char c_node_type[5];
    itoa(updatable_parameters.node_type,c_node_type,10);
    pushData("Current_Node_Type",c_node_type ,getTimeStamp());
    
    // If node type is depth node, take level sensor measurements
    
    if(updatable_parameters.node_type == NODE_TYPE_DEPTH){
        
        // level_sensor_t is a new data type we defined in level_sensor.h. We then use that data type to define a structure variable m_level_sensor
        level_sensor_t m_level_sensor;
        
        // Take level sensor readings and save them to m_level_sensor
        m_level_sensor = level_sensor_take_reading();
    
        // If the number of valid level sensor readings is greater than 0, then print the level sensor reading, and push the data to the data wheel
        if(m_level_sensor.num_valid_readings > 0){
            snprintf(value,sizeof(value),"%d",m_level_sensor.level_reading);
            printNotif(NOTIF_TYPE_EVENT,"maxbotix_depth=%s",value);
            pushData("maxbotix_depth",value,timeStamp);
            
            // Print measurement to SD card to file called data.txt
            SD_write(Data_fileName, "a+", c_timeStamp);
            SD_write(Data_fileName, "a+", " maxbotix_depth: ");
            SD_write(Data_fileName, "a+", value);
            SD_write(Data_fileName, "a+", " ");
        }else{
            printNotif(NOTIF_TYPE_ERROR,"Could not get valid readings from Maxbotix.");
            //pushData("maxbotix_depth","error",timeStamp);
        }
    }
    
    
    // voltage_t is a new data type we defined in voltages.h. We then use that data type to define a structure variable m_voltage
    voltage_t m_voltage;
    
    // Take voltage readings and save them to m_voltage
    m_voltage = voltage_take_readings();
    
    // If the reading is valid, print the voltage battery and pressure transducer reading and push the data to the data wheel
    if(m_voltage.valid){
        // battery voltage data
        snprintf(value,sizeof(value),"%.2f",m_voltage.voltage_battery);
        printNotif(NOTIF_TYPE_EVENT,"v_bat=%s",value);
        pushData("v_bat",value,timeStamp);
        
        // Print measurement to SD card to file called data.txt
        SD_write(Data_fileName, "a+", " ");
        SD_write(Data_fileName, "a+", c_timeStamp);
        SD_write(Data_fileName, "a+", " vbat: ");
        SD_write(Data_fileName, "a+", value);
        
        // If node type is green infrastructure node, take pressure transducer measurements
         
        if(updatable_parameters.node_type == NODE_TYPE_GREENINFRASTRUCTURE){
            
            // pressure transducer voltage (V) data
            //snprintf(value,sizeof(value),"%.2f",m_voltage.voltage_pressure);
            //printNotif(NOTIF_TYPE_EVENT,"pressure_voltage=%s",value);
            //pushData("pressure_voltage",value,timeStamp);
            
            // pressure_t is a new data type we defined in voltages.h. We then use that data type to define a structure variable m_pressure
            pressure_t m_pressure;
            
            // Make pressure transducer calculators for current and depth
            m_pressure = pressure_calculations(m_voltage);
            
            // pressure transducer current (mA) data
            snprintf(value,sizeof(value),"%.2f",m_pressure.pressure_current);
            printNotif(NOTIF_TYPE_EVENT,"pressure_current=%s",value);
            pushData("pressure_current",value,timeStamp);
            
            // Print measurement to SD card to file called data.txt
            SD_write(Data_fileName, "a+", " ");
            SD_write(Data_fileName, "a+", c_timeStamp);
            SD_write(Data_fileName, "a+", " pressure_current: ");
            SD_write(Data_fileName, "a+", value);
            
            // pressure transducer depth (ft) data
            snprintf(value,sizeof(value),"%.2f",m_pressure.pressure_depth);
            printNotif(NOTIF_TYPE_EVENT,"pressure_depth=%s",value);
            pushData("pressure_depth",value,timeStamp);
            
            // Print measurement to SD card to file called data.txt
            SD_write(Data_fileName, "a+", " ");
            SD_write(Data_fileName, "a+", c_timeStamp);
            SD_write(Data_fileName, "a+", " pressure_depth: ");
            SD_write(Data_fileName, "a+", value);
        }    
    }else{
        printNotif(NOTIF_TYPE_ERROR,"Could not get valid readings from ADC.");
        
    }
        
    
    return 0u;
}

// ==============================================
// Set the system time
// Returns 0 if there is an error, returns 1 if successful
// ==============================================
uint8 setTime(long timeStamp){
    // If timestamp is <= 0 print error statement and return 0
    if(timeStamp <=0){
        printNotif(NOTIF_TYPE_ERROR,"Error setting system time.");
        return 0;  
    }
    // Create a new pointer t of data structure tm
    struct tm * t;
    // Converts a calendar time (pointed to by timer) and returns a pointer to a structure containing a UTC (or Greenwich Mean Time) value.
    t = gmtime(&timeStamp);
    
    // Create variable localTime of data structure RTC_TIME_DATE
    RTC_TIME_DATE localTime;
    localTime.Sec = t->tm_sec;
    localTime.Min = t->tm_min;
    localTime.Hour = t->tm_hour;
    localTime.Year = t->tm_year + 1900;
    localTime.Month = t->tm_mon;
    localTime.DayOfMonth = t->tm_mday;

    // Writes time and date values as current time and date.
    RTC_WriteTime(&localTime);
    
    // Return 1 when the system time was set properly.
    return 1;
}

//downloads and stores files on SD card



// ==============================================
// Will they love it, will they hate it
// Never fade it, I evade it
// Cause I never really want to complicate it
//      - Logic, Upgraded (2015)
//
// downloads new firmware, stores it on SD card, and reboots the system to allow bootloader 
// returns 0 if modem is connected, 1 if the file downlaod failed, and does not return otherwise (because it reboots)
// ==============================================

uint8 upgraded(char *host, int port,  char *route){
    
    uint8 upgrade_status = 0;
    
   //AGPS will only work if we're connected to network, so don't call this fuction otherwise
    if(modem_state != MODEM_STATE_READY){
        return upgrade_status;//where valid = 0
    }
    
    char fileName[] = "upgrade.hex";
    //delete file, if one exisits already
    SD_rmfile(fileName);
    
    // Create http request
    construct_generic_HTTP_request(http_request,http_body,host,route,port,"POST","Close","",0,"1.1");
    printNotif(NOTIF_TYPE_EVENT,"Full HTTP Request: %s", http_request);
    // Create array portConfig of size 200 characters
    char portConfig[200];
    // Print port configurations
    snprintf(portConfig,sizeof(portConfig),"AT#SD=1,0,%d,\"%s\",0,0,1\r",port,host);
    printNotif(NOTIF_TYPE_EVENT,"%s",portConfig);
    // AT command to configure the port
    upgrade_status = at_write_command(portConfig,"OK",10000u);
    // AT command #SSEND= is an execution command that permits, while the modul eis in command mode, to send data through a connected socket.
    // To complete the operation, send Ctrl-Z char to exit
    upgrade_status = at_write_command("AT#SSEND=1\r\n",   ">", 1000u);
    // Append <ctrl+z> escape sequence to http_request to exit modem command line
    strncat(http_request, "\032", 1); 
    // AT command to send http request
    upgrade_status = at_write_command(http_request, "OK", 5000u);
    // AT command #SRECV= is an execution command that permits users to read data arrived through a connection socket
    // = 1 means the UPD information is enabled: data are read just until the end of the UDP datagram and the response carries information about the remote IP address and port and about the remaining bytes in the datagram.
    upgrade_status = at_write_command("AT#SRECV=1,1000\r","OS-OK",5000u);
    //SD_write(fileName,"w+",uart_received_string);
    
    
    return upgrade_status;
    
}




/* [] END OF FILE */
