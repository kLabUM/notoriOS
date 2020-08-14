#include "main.h"

//this files holds all the "business" callbacks, which are abstracted from the main OS





/*
TODO:

    X initilizwe modem parameters and cell tower time at bootup
    X Write real maxbotixdata to server
    X fix string header length constant
    X implement 4G modem
    X implement a backup timer, so sync shuts down if it can't get a signal with network
    X fix battery voltage reading
    - Throw error if string length exceeds size
    X GPS lock implementation 
    - start alarm on 0 (on the hour), rather then arbitrary time
    - Implement backup protocol. Fallback from AT#SD and reset context, Fall back to AT#SD using AT#SH=1? if nothing received from server
    - Sizing oh HTTP/heard/body could be fixed. Uses a lot of memory now. 
    - test power
    - bootloader
    - implement modem backup timer and make it so that the modem spends time in sleep mode
    - Read form influx anbd parse  Meta DB
    - test power
    - write data to SD card
    - watchdog -- built-in is too short (2sec), so may need to build one using a timer

*/

