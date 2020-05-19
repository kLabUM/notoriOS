//drivers for UART debug 

#include "debug.h"
#include <stdarg.h>



void debug_start(){
    #if USE_DEBUG
       Debug_UART_Start();
       //setvbuf(stdout, NULL, _IOFBF, Debug_UART_TX_BUFFER_SIZE);//this sets the printf() bufefr to zero, so there are not delays
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

    //check if UART is busy writing and waiit, if needed
    if(!(Debug_UART_ReadTxStatus() & Debug_UART_TX_STS_FIFO_EMPTY)){
        CyDelay(1u);
    }
    
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
    
    
//this will allow you to use printf(), as you would in regulart c programs
//careful though, printf() is sometimes not safe in embedded sytems
// may want to use printd() function below
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

//use like printf, but this will add an depoch timestamp to the printput
void printNotif(uint8 type, char* format, ...){
    #if USE_DEBUG


    //basically, just hijack printf and inject the timestamp infront
    printd("{ ");
    printd("\"time\":\"%ld\" " , getTimeStamp());
    if(type == NOTIF_TYPE_EVENT){
        printd("\"event\":\"notif\" \"value\":\"");
    }else if(type == NOTIF_TYPE_WARNING){
        printd("\"event\":\"warning\" \"value\":\"");
    }else if(type == NOTIF_TYPE_ERROR){
        printd("\"event\":\"error\" \"value\":\"");
    }else{
       printd("\"event\":\"undefined\" \"value\":\""); 
    }
   
    
    va_list argptr;
    va_start(argptr, format);
    char debug_string[MAX_DEBUG_STRING_LENGTH];
    vsnprintf(debug_string,MAX_DEBUG_STRING_LENGTH,format, argptr);
    Debug_UART_PutString(debug_string);
    va_end(argptr);

    printd("\"}\r\n");
    #endif
    
}

void printTestStatus(test_t test){
#if USE_DEBUG
    printd("{ ");
    printd("\"time\":\"%ld\" " , getTimeStamp());
    
    printd("\"event\":\"test\" ");
    printd("\"name\":\"%s\" ", test.test_name);
    printd("\"status\":\"%d\" ", test.status);
    printd("\"reason\":\"%s\" ", test.reason);

    printd("}\r\n");
    #endif
   
}


//returns local times in epoch seconds (seconds since Jan 1, 1970)
//can also cast this to a t_of_day struct
long getTimeStamp(){
    
    RTC_TIME_DATE *localTime = RTC_ReadTime();
   
    struct tm t;
    time_t t_of_day;

    t.tm_sec = localTime->Sec;
    t.tm_min = localTime->Min;
    t.tm_hour = localTime->Hour;
    t.tm_year = localTime->Year - 1900;  // Year - 1900
    t.tm_mon = localTime->Month;           // Month, where 0 = jan
    t.tm_mday = localTime->DayOfMonth;          // Day of the month
    t.tm_isdst = 0;        // Is Daylight Savings on? 1 = yes, 0 = no, -1 = unknown
    
    t_of_day = mktime(&t);

    return (long) t_of_day;
}


void stripEscapeCharacters(char *string){

  char * pch;
  pch=strchr(string,'\n');
  while (pch!=NULL)
  {
    string[pch-string+1] = 0;
    pch=strchr(pch+1,'s');
  }
}

char *strextract(const char input_str[], char output_str[],
                 const char search_start[], const char search_end[]) {
    if (input_str == NULL) return NULL;
    char *begin, *end = NULL;

    if ((begin = strstr(input_str, search_start))) {
        begin += strlen(search_start);
        if ((end = strstr(begin, search_end))) {
            strncpy(output_str, begin, end - begin);
            output_str[end - begin] = '\0';
        }
    }

    return end;
}

//out attmpt at a safe (from buffer overflow) version of printf     
void printd(char* format, ...){
    
    va_list argptr;
    va_start(argptr, format);
    
    char debug_string[MAX_DEBUG_STRING_LENGTH];
    vsnprintf(debug_string,MAX_DEBUG_STRING_LENGTH,format, argptr);
    Debug_UART_PutString(debug_string);
    
    
    va_end(argptr);
    
}
