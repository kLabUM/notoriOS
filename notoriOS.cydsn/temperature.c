#include "notoriOS.h"
#include "temperature.h"

// has data logger, no need to take in individual readings
// clear sensor data
void TEMP_clear() {
    TEMP_UART_ClearRxBuffer();
}


// initialize all parameters to take reading
void TEMP_Talk() {
    TEMP_UART_Start();
    TEMP_RX_SetDriveMode(PIN_DM_DIG_HIZ);
    TEMP_Sensor_Power_Write(ON);
    CyDelay(1000u);
}

// turn off parameters if not needed
void TEMP_Stop() {
    TEMP_Sensor_Power_Write(OFF);
    TEMP_UART_Stop();
    TEMP_RX_SetDriveMode(PIN_DM_STRONG);
    TEMP_RX_Write(OFF);
    TEMP_TX_Write(OFF);
}

// temperature calibration 
uint8 TEMP_cal() {

    TEMP_sensor_t readings = TEMP_Read();


    TEMP_UART_PutString("C,0\r");
    CyDelay(1000u);
    TEMP_UART_PutString("Cal\r");
    CyDelay(2000u);
    TEMP_Stop();

    CyDelay(1000u);
    TEMP_sensor_t calibrated = TEMP_Read();

    calibrated.TEMP_reading;
    if(calibrated.TEMP_reading >= 100) {
        printNotif(NOTIF_TYPE_EVENT, "The sensor has been calibrated to boiling temperature");
        return 1;
    }

    printNotif(NOTIF_TYPE_EVENT, "Please calibrate sensor again");
    return 0;
}

uint8 Temp_Debug() {
    
    int debug = TEMP_UART_PutString("C,0\r");
    CyDelay(1000u);
    printNotif(NOTIF_TYPE_EVENT, "One time value is" + debug);
}

