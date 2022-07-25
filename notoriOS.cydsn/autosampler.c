/**
 * @file autosampler.c
 * @brief Implements functions for ISCO autosampler
 * @author Brandon Wong, Matt Bartos, and Meagan Tobias
 * @version TODO
 * @date 2022-07-18
 */

#include "autosampler.h"
#include "notoriOS.h"
//#include "debug.h" included before but probs covered by "notoriOS.h"
 
/*
prototype bottle_count interrupt 
isr_SamplerCounter is the address of the function to run when the isr is enabled 
(see CY_ISR(isr_SampleCounter) at end of this file)
(CY_ISR_PROTO() is used to customize what happens intead of the automated code)
*/
CY_ISR_PROTO(isr_SampleCounter);

/*=========================================================================
*   Variable initialization
*=========================================================================*/
// Declare variables bottle_count1 might not be used since count2 is used in the take_Sample()
uint8 bottle_count = 0, SampleCount = 0, SampleCount1 = 0, autosampler_state;;
// to keep track of bottle count
uint8 *count  = &bottle_count;


/*=========================================================================
*   App interface functions
*=========================================================================*/

uint8 App_Autosampler(){
    long timeStamp = getTimeStamp();
    /*-=============================================================================== 
    This was going to do the when to sample calcs here, but might as well do it in .py script instead
    
    long timestamp = getTimeStamp(); //store current time

    // store string from autosampler for SampleAt header
     char * compare_location;
     compare_location = strstr(autosampler_inbox, "SampleAt:"); 
    
     if (compare_location != NULL){ // we got a command
        extract_string(autosampler_inbox,"SampleAt:",",",SamplerTime)

    //--- check if type float to changes sampler frequency time --> might impact notorios.c part with setting times
    // -> actually this should all be done in the script reading and send sampler freq calculated in .py
        sscanf(SamplerTime, "%f", &samp_time_float);
     }
     *==============================================================================*/
    
    /*
    Might want to eventually add dealing with multiple future sample times being sent 
        this could reduce number of needed re-connects or if the node misses a check in
    */

   /*
    also might want to add to take a voltage reading since make measurements might not do it at the right time anymore
   */

    /*
     this will have take_sample and then push it all to the wheel 
     (instead of in makeMeasurements in norotios.c)
    /=====================================
    
    bottle_count = autosampler_take_sample(count);
    count = &bottle_count;

    if(bottle_count != 0){
        //Print all measurements to serial and the SD card to file called data.txt

        snprintf(value,sizeof(value),"%d",bottle_count);
        printNotif(NOTIF_TYPE_EVENT,"bottle_count=%s",value);            
        pushData("autosampler_bottle_count",value,timeStamp);

        SD_write(Data_fileName, "a+", c_timeStamp);
        SD_write(Data_fileName, "a+", " bottle_count: ");
        SD_write(Data_fileName, "a+", value);
        SD_write(Data_fileName, "a+", " ");   //adds space
    }
    */

    char * compare_location;
    int test_delay = 0;
    compare_location = strstr(autosampler_inbox,"ON"); 
    if(compare_location!=NULL){
        test_delay = 1000;
        printNotif(NOTIF_TYPE_EVENT,"\n\n------------  Short Blink ---------------\n\n");   
    }
    
    compare_location = strstr(autosampler_inbox,"OFF"); 
   if(compare_location!=NULL){
        test_delay = 5000;
        printNotif(NOTIF_TYPE_EVENT,"\n\n------------  Long Blink ---------------\n\n");
    }   

    //clears inbox maybe need to do? check other apps
    autosampler_inbox[0] = '\0';
    printNotif(NOTIF_TYPE_EVENT,"Autosampler flash time: %d",test_delay);
    // FOR INITIAL CLOUD TESTING WILL JUST BLINK LED LIGHT OFF PIN
    autosampler_power_on(); 
    CyDelay(test_delay);
    autosampler_power_off(); 
    CyDelay(test_delay);
    autosampler_power_on(); 
    CyDelay(test_delay);
    autosampler_power_off(); 

    bottle_count += 1;

    // To store readings
    char s_bottle_count[10];
    snprintf(s_bottle_count,sizeof(s_bottle_count),"%d",bottle_count);
    printNotif(NOTIF_TYPE_EVENT,"bottle_count=%s",s_bottle_count);            
    pushData("autosampler_bottle_count",s_bottle_count,timeStamp);

    
    // the autosampler is enabled with every connection and turned off after every sample collected
    return 0u;
}
 
void Autosampler_Update(char * message){
   strcpy(autosampler_inbox, message);
}
 
test_t autosampler_test(){     

    /*
    IDEA TO TAKE A SAMPLE ON RESET AS OLD VERSION KIND OF HACK BY HAVING IT IN INITIALIZE
    MAYBE EVEN HAVE SOMETHING ON DAY TO MEASURE EVENT TIME TO SEE IF IT SEEMS LONG ENOUGH
    */  
   test_t test; // test_t is a new data type we defined in test.h. We then use that data type to define a structure variable test
   test.status = 0; // set test status to zero
   snprintf(test.test_name,sizeof(test.test_name),"TEST_AUTOSAMPLER");
   //Example of code for printing error messages of test if desired
//    snprintf(test.reason,sizeof(test.reason),"nWANTED=%d,nMEDIAN=%d.",
//        N_READINGS,
//        sensor.level_reading
//    );
 
   return test;
}

/*=========================================================================
*
**   Sampler interface functions
*
*=========================================================================*/

/*=========================================================================
*   Start/stop interupt service routine (isr) to count bottle number 
*   by running defined function  CY_ISR(isr_SampleCounter)
*=========================================================================*/
void autosampler_start(){
    isr_SampleCounter_StartEx(isr_SampleCounter);       
    BottleCounter_Start();
    autosampler_state = AUTOSAMPLER_STATE_OFF;
    
}

void autosampler_stop() {
    isr_SampleCounter_Stop();
    BottleCounter_Stop();
    autosampler_state = AUTOSAMPLER_STATE_OFF;
}

/*=========================================================================
*  Turn on and off the sampler through power pin
*=========================================================================*/
void autosampler_power_on() {
    Pin_Sampler_Power_Write(1u);
    CyDelay(1000u);//give the sampler time to boot
    autosampler_state = AUTOSAMPLER_STATE_IDLE;
}

void autosampler_power_off() {
    Pin_Sampler_Power_Write(0u);
    autosampler_state = AUTOSAMPLER_STATE_OFF;
}

/*=========================================================================
*   Take sample   
*   @param count is last known bottle number
*   @return count2 is current bottle filled
*=========================================================================*/

uint8 autosampler_take_sample(uint8 *count){
    uint8 count2 = 0;
    
    autosampler_start();
    autosampler_power_on();    
        
    if (*count >= MAX_BOTTLE_COUNT) {  
        //No more bottles can be taken, don't try to take a sample      
        return 0;
    }
    
    uint32 i = 0u, delay = 100u, interval;
    
    // Reset the bottle Counter
    BottleCounter_WriteCounter(0u);
    autosampler_state = AUTOSAMPLER_STATE_BUSY;
            
    // Send PULSE_COUNT pulses @ 10Hz to trigger sampler
    for(i=0; i < PULSE_COUNT; i++){
        Pin_Sampler_Trigger_Write(1u);
        CyDelay(100u);
        Pin_Sampler_Trigger_Write(0u);
        CyDelay(100u);
    }
    
    interval =  2u*60*1000/delay;           // Wait Max of 2 Minutes for distributor arm to move
    
    for (i = 0; i < interval ; i++) {  
        CyDelay(delay);
        if (Pin_Sampler_Completed_Sample_Read()!=0) { // Event pin on autosampler is HI
            break;
        }
    }
    
    
    if (Pin_Sampler_Completed_Sample_Read() != 0) {
        
        interval =  4u*60*1000/delay;       // Wait Max of 4 Minutes for pumping to complete -- is it really?
        //interval = 5u*100/delay; //wait 0.5 second for pumping to compelete FOR TESTING ONLY
        for (i = 0; i < interval ; i++) { 
            CyDelay(delay);
            if (Pin_Sampler_Completed_Sample_Read()==0) { // Event pin on autosampler is HI
                break;
            }
        }
    }
    
    autosampler_state = AUTOSAMPLER_STATE_IDLE;
    
    //*count = BottleCount_Read(); //this function throws and error
    count2 = BottleCounter_ReadCounter(); //set count2 equal to the number of pulses sent for bottle count
    *count = count2;
                                                  
    autosampler_power_off(); 
    autosampler_stop();
    //to bypass everything and just get bottle countss
    // Get clock time and save to timeStamp
    long timeStamp = getTimeStamp();
    char c_timeStamp[32];
    snprintf(c_timeStamp,sizeof(c_timeStamp),"%ld",timeStamp);
    
    // Holds string for value that will be written 
    char value[DATA_MAX_KEY_LENGTH];
    snprintf(value,sizeof(value),"%d",count2);
    printNotif(NOTIF_TYPE_EVENT,"bottle_count=%s",value);
    return count2;
}

/*=========================================================================
*   TODO: from pre-notorios possible function
*=========================================================================*/

uint8 zip_autosampler(char *labels[], float readings[], uint8 *array_ix, int *autosampler_trigger, uint8 *bottle_count, uint8 max_size){
    // Ensure we don't access nonexistent array index
    uint8 nvars = 2;
    if(*array_ix + nvars >= max_size){
        return *array_ix;
    }
    (*autosampler_trigger) = 0u; // Update the value locally
    labels[*array_ix] = "autosampler_trigger"; // Update the database
    readings[*array_ix] = 0;
    (*array_ix)++;
        
    if (*bottle_count < MAX_BOTTLE_COUNT) {
	    labels[*array_ix] = "isco_bottle";
        autosampler_start();
        autosampler_power_on();                                    
        if (autosampler_take_sample(bottle_count) ) {
			readings[*array_ix] = *bottle_count;
		}
		else {
			// Use -1 to flag when a triggered sample failed
            readings[*array_ix] = -1;
		}                        
        autosampler_power_off(); 
        autosampler_stop();
	    (*array_ix)++;
		}
	else {
        //debug_write("bottle_count >= MAX_BOTTLE_COUNT");
		}
    return *array_ix;
}

CY_ISR(isr_SampleCounter){
    SampleCount1++;
}

//*  Steps for Testing the autosampler *//
// - Enter Debug Mode
// - Insert Breakpoint at main.c:154  --- this is for perfect cell
// - Run 
// - Set next instruction at data.c:185
// - Manually set autosampler_flag to 1
// - Manually set autosampler_trigger to 1
// - Insert Breakpoint at autosampler.c:102
// - Run
// - Set next instuction at autosampler.c:105
// - Insert Breakpoint at autosampler.c:114
// - Run
// - Insert Breakpoint at autosampler.c:52
// - The value *referenced* by [count] should be 0 (or less than MAX_BOTTLE_COUNT)
//   Check by hovering over the variable and cliking the [+] button to expand// 
// - NOTE: Insert Breakpoint at autosampler.c:72 if you want to 
//         skip waiting for the autosampler to respond
//         and then set next instruction at autosampler.c:91 
// - Insert Breakpoint at autosampler.c:96
// - Run
// - Check the value of [count2] at autosampler.c:94 to double check the bottle counter
//   by hovering over the variable
// - Run
// - Relay should click as it opens again, turning off the autosampler and the yellow light if it is enabled
// - Done

/* [] END OF FILE */