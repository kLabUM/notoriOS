/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

/* [] END OF FILE */
#include "LED.h"
#include "notoriOS.h"
#include "debug.h"

uint8 App_LED(){
    
    //LED_Write(!LED_Read()); <- local blinky
    // printNotif(NOTIF_TYPE_EVENT,"\n\n------------  BLINK ---------------\n\n");
    
    char * compare_location;
    
    compare_location = strstr(app_led_inbox,"ON");        
    if(compare_location!=NULL){
        LED_Write(1u);
        app_led_inbox[0] = '\0';
        pushData("App_LED_State", "1", getTimeStamp());
        printNotif(NOTIF_TYPE_EVENT,"\n\n------------  BLINK ON ---------------\n\n");
        
    }
    
    compare_location = strstr(app_led_inbox,"OFF");        
    if(compare_location!=NULL){
        LED_Write(0u);
        app_led_inbox[0] = '\0';
        pushData("App_LED_State", "0", getTimeStamp());
        printNotif(NOTIF_TYPE_EVENT,"\n\n------------  BLINK OFF ---------------\n\n");
    }
    
    return 0u;
}

void App_LED_Update(char * message){
    strcpy(app_led_inbox, message);
}

