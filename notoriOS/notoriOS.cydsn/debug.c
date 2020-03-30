//drivers for UART debug 

#include "debug.h"

void debug_start(){
    #if USE_DEBUG
       Debug_UART_Start();
       //setvbuf(stdout, NULL, _IOFBF, 0);//this sets the printf() bufefr to zero, so there are not delays
       //setbuf(stdout, NULL);
    #endif
}

void debug_stop(){
    #if USE_DEBUG
        Debug_UART_Stop();
    #endif
}

void debug_sleep(){
   #if USE_DEBUG
    CyDelay(1u);//need to delay for 1ms to allow printf to finish writing (it's non blocking)
    Debug_UART_Sleep(); 
   #endif
   
}

void debug_wakeup(){
    #if USE_DEBUG
        Debug_UART_Wakeup();
    #endif
}

/* For GCC compiler revise _write() function for printf functionality */
//this makes it so that when we call printf() it will write to UART
//NOTE: For this to work HEAP size in "System" must be set to 0x300 or more
#if USE_DEBUG
    
int _write(int file, char *ptr, int len)
{
    int i;
    file = file;
    for (i = 0; i < len; i++)
    {
        Debug_UART_PutChar(*ptr++);
    }

    return (len);
}

#endif


