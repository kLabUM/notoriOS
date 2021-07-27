#include "notoriOS.h"
#include "diss_oxy.h"
#include "level_sensor.h"

char do_received_string[1024];
int16 do_string_index=0;


CY_ISR_PROTO(DO_ISR); // Declares a custom ISR function "isr_telit_rx" using the CY_ISR_PROTO macro instead of modifying the auto-generated code

// Define the custom ISR function "isr_telit_rx" using the CY_ISR macro
// Get each character from Telit and saves it to uart_received_string
CY_ISR(DO_ISR){ 
    printNotif(NOTIF_TYPE_EVENT, "entered isr");
    // UART_Telit_GetRxBufferSize() returns the number of received bytes available in the RX buffer.
    while(Diss_Oxygen_UART_GetRxBufferSize() ==1){
        do_received_string[do_string_index]=UART_Telit_GetChar();
        do_string_index++;
        //printNotif(NOTIF_TYPE_EVENT, "appended char");
    }
   

}

test_t DO_sensor_test(){

    test_t test;
    
    uart_clear();
    // forked and edited from level_sensor.c
    test.status = 0; // set test status to zero
    snprintf(test.test_name,sizeof(test.test_name),"TEST_DO_SENSOR");
   
    
    // Start sensor power, uart, and isr
    Level_Sensor_Power_Write(ON);   // Pulls Level Sensor power pin high (turns it on).
    CyDelay(200u); // Sensor bootup sequence ~200ms -- senosor will spit out ID info, but we don't need that
    DO_RX_SetDriveMode(PIN_DM_DIG_HIZ); // RX_SetDriveMode(): Sets the drive mode for each of the Pins component's pins. PIN_DM_DIG_HIZ: High Impedance Digital.
    Diss_Oxygen_UART_Start(); // Level_Sensor_UART_Start() sets the initVar variable, calls the Level_Sensor_UART_Init() function, and then calls the Level_Sensor_UART_Enable() function.
    DO_ISR_StartEx(DO_ISR); // Sets up the interrupt and enables it.
    Diss_Oxygen_UART_PutString("C,1\r\n");
    

    for (int i = 0;i < 2;i++){
        //Diss_Oxygen_UART_PutString("R\r\n");
        printNotif(NOTIF_TYPE_EVENT, "int: %i",Diss_Oxygen_UART_GetChar());
        printNotif(NOTIF_TYPE_EVENT, "char: %c",Diss_Oxygen_UART_GetChar());
        printNotif(NOTIF_TYPE_EVENT, "int: %li",Diss_Oxygen_UART_GetByte());
        printNotif(NOTIF_TYPE_EVENT, "char: %c",Diss_Oxygen_UART_GetByte());
        printNotif(NOTIF_TYPE_EVENT, "int: %i",Diss_Oxygen_UART_ReadRxData());
        printNotif(NOTIF_TYPE_EVENT, "char: %c",Diss_Oxygen_UART_ReadRxData());
        printNotif(NOTIF_TYPE_EVENT, "%s", do_received_string);
        printNotif(NOTIF_TYPE_EVENT, "%c", do_received_string);
        printNotif(NOTIF_TYPE_EVENT, "%c", do_received_string[0]);
        printNotif(NOTIF_TYPE_EVENT, "*****************");
        Diss_Oxygen_UART_ClearRxBuffer(); // clear the buffer
        CyDelay(2000u);
    }
    
    //stop all sensor processes
    Diss_Oxygen_UART_Stop(); // Disables the UART operation.
    DO_ISR_Stop(); // Disables and removes the interrupt.
    Level_Sensor_Power_Write(OFF); // Pulls Level Sensor Power pin low (turns it off).
    // RX_SetDriveMode(): Sets the drive mode for each of the Pins component's pins.
    // String Drive Mode means it will only be outputting a voltage (don't send any data) when pulled low.
    // UART in PSOC starts high. Then pulls low to get data. When turn sensor off, still using power because UART still high.
    // So turning it to Strong Drive has it stay low.
    DO_RX_SetDriveMode(PIN_DM_STRONG);
    // Stops writing the value to the physical port.
    // Just because turn power of sensor off, pin could still be high and getting power. This ensures it stays low.
    DO_RX_Write(OFF);
    snprintf(test.reason,sizeof(test.reason), "%c",Diss_Oxygen_UART_ReadRxStatus());

    return test;
}