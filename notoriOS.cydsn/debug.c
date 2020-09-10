#include "debug.h"
#include <stdarg.h> // handles variable argument list

char    BB_fileName[30] = "blackbox.txt";

// function to start UART debug
void debug_start(){
    // if debug flag = 1 then start UART
    #if USE_DEBUG
       Debug_UART_Start();
       //setvbuf(stdout, NULL, _IOFBF, Debug_UART_TX_BUFFER_SIZE);//this sets the printf() bufefr to zero, so there are not delays
       //setbuf(stdout, NULL);
    #endif
}

// function to stop UART debug
void debug_stop(){
    // if debug flag = 1 then stop UART
    #if USE_DEBUG
        Debug_UART_Stop();
    #endif
}

// function to sleep UART debug
void debug_sleep(){
    // if debug flag = 1 and printf still writing, sleep UART
    #if USE_DEBUG

        // Check if UART is busy writing and waiit, if needed
        // Debug_UART_ReadTxStatus() reads the status register for the TX portion of the UART.
        if(!(Debug_UART_ReadTxStatus() & Debug_UART_TX_STS_FIFO_EMPTY)){
            CyDelay(1u);
        }
    // This is the preferred API to prepare the component for sleep. 
    // The Debug_UART_Sleep() API saves the current component state. Then it calls the Debug_UART_Stop() function and calls Debug_UART_SaveConfig() to save the hardware configuration.
    Debug_UART_Sleep(); 
   #endif
   
}

// function to wakeup UART debug
void debug_wakeup(){
    // if debug flag = 1, wakeup UART debug and turn off stillWriting flag
    #if USE_DEBUG
        Debug_UART_Wakeup();
    #endif
}

// if debug flag = 1, and using the GCC compiler, revise the _write() function so that when we call printf() it will write to UART
// NOTE: For this to work HEAP size in "System" must be set to 0x300 or more
// Stack and heap are both a part of memory. Stack holds functions, variables, etc. Heap sits on top of the stack.
// If heap is too small, then you get a stack overflow. FOr printing, need to have a bigger heap. BK played around until got correct size.
// If have memory problems, play around with heap size.
#if USE_DEBUG
    
    
// This will allow you to use printf(), as you would in regulart c programs
// careful though, printf() is sometimes not safe in embedded sytems
// may want to use printd() function below
int _write(int file, char *ptr, int len)
{
    int i;
    file = file;
    for (i = 0; i < len; i++)
    {   
        // Puts a byte of data into the transmit buffer to be sent when the bus is available.
        Debug_UART_PutChar(*ptr++);
    }
    
    return (len);
}

#endif

// Create function to hijack printf to add an epoch timestamp to print output
// Whenever a function is declared to have an indeterminate number of arguments, in place of the last argument use "..."
// to tell the compiler the function should accept however many arguments that the programmer uses, as long as it is equal 
// to at least the number of variables declared. 
void printNotif(uint8 type, char* format, ...){
    
    char debug_string[MAX_DEBUG_STRING_LENGTH]; // string to be printed
    
    // if debug flag = 1, then print debug statements, otherwise don't print anything
    #if USE_DEBUG
   
        // if it is an error, warning, or startup notification and debug_level >= 0, then print them
        if ((type == NOTIF_TYPE_ERROR || type == NOTIF_TYPE_WARNING  || type == NOTIF_TYPE_STARTUP) && updatable_parameters.debug_level >= 0){
            printd("{ ");
            printd("\"time\":\"%ld\" " , getTimeStamp());
            
            if (type == NOTIF_TYPE_ERROR){
                printd("\"event\":\"error\" \"value\":\"");
            }else if(type == NOTIF_TYPE_WARNING){
                printd("\"event\":\"warning\" \"value\":\"");
            } else{
                printd("\"event\":\"startup\" \"value\":\"");
            }
        
        // else if it is an event notification and debug level >= 1, then print everything
        }else if (type == NOTIF_TYPE_EVENT && updatable_parameters.debug_level >= 1){
            printd("{ ");
            printd("\"time\":\"%ld\" " , getTimeStamp());
            printd("\"event\":\"notif\" \"value\":\"");

        // else do not print and exit the function    
        }else{
            return;
        }
        
        va_list argptr; // create variable argprt of the type va_list from stdarg.h
        va_start(argptr, format); // from stdarg.h: the va_start() macro is invoked to initialize ap to the beginning of the list before any calls to va_arg().
        vsnprintf(debug_string, MAX_DEBUG_STRING_LENGTH, format, argptr); // send text to buffer
        Debug_UART_PutString(debug_string); // Sends a NULL terminated string to the TX buffer for transmission
        va_end(argptr); // the va_end() macro is used to clean up; it invalidates ap for use (unless va_start() or va_copy() is invoked again).
        printd("\"}\r\n");
        // Get clock time and save to timeStamp
        long timeStamp = getTimeStamp();
        char c_timeStamp[32];
        snprintf(c_timeStamp,sizeof(c_timeStamp),"%ld",timeStamp);
        SD_write(BB_fileName,"a+",c_timeStamp); // Write timestamp to SD Card  
        SD_write(BB_fileName,"a+",debug_string); // Write data to SD Card  

    #endif 
}

// Print debug test status
void printTestStatus(test_t test){
    // if debug flag = 1
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


// Returns local times in epoch seconds (seconds since Jan 1, 1970)
// Can also cast this to a t_of_day struct
long getTimeStamp(){
    // RTC_TIME_DATE * RTC_ReadTime(void) Description: Reads the current time and date. Parameters: None. Return Value: Pointer to the RTC_TIME_DATE.
    // Creates a variable named localTime that is a pointer to RTC_TIME_DATE. It saves the time and date into the variable localTime.
    RTC_TIME_DATE *localTime = RTC_ReadTime();
   
    struct tm t; // creates variable t of struct tm
    time_t t_of_day;

    // sets each variable to the correct time 
    t.tm_sec = localTime->Sec;
    t.tm_min = localTime->Min;
    t.tm_hour = localTime->Hour;
    t.tm_year = localTime->Year - 1900;  // Year - 1900
    t.tm_mon = localTime->Month;           // Month, where 0 = jan
    t.tm_mday = localTime->DayOfMonth;          // Day of the month
    t.tm_isdst = 0;        // Is Daylight Savings on? 1 = yes, 0 = no, -1 = unknown
    
    // the mktime() function converts a broken-down local time (pointed to by timeptr) and returns a pointer to a calendar time.
    // the syntax for the mktime function in the C Language is: time_t mktime(struct tm *timeptr);
    // creates variable t_of_day of type time_t
    t_of_day = mktime(&t); // determines a calendar time

    return (long) t_of_day;
}

// The C library function char *strchr(const char *str, int c) searches for the first occurrence of the character c (an unsigned char) in the string pointed to by the argument str.
// Format for strchr() function: char *strchr(const char *str, int c).
void stripEscapeCharacters(char *string){

  char * pch;
  pch=strchr(string,'\n');
  while (pch!=NULL)
  {
    string[pch-string+1] = 0;
    pch=strchr(pch+1,'s');
  }
}

// Extract
char *strextract(const char input_str[], char output_str[],
                 const char search_start[], const char search_end[]) {
    if (input_str == NULL) return NULL;
    char *begin, *end = NULL;

    // strstr(): char *strstr(const char *haystack, const char *needle). haystack - main C string to be scanned. needle - the small string to be searched with-in haystack string.
    if ((begin = strstr(input_str, search_start))) {
        // The strlen() function calculates the length of a given string. The strlen() function takes a string as an argument and returns its length. The returned value is of type long int. It is defined in the <string.h> header file.
        begin += strlen(search_start);
        if ((end = strstr(begin, search_end))) {
            // strncpy(: char *strncpy(char *dest, const char *src, size_t n). dest -  pointer to the destination array where the contents is to be copied. scr - the string to be copied. n - # of characters to be copied from source.
            strncpy(output_str, begin, end - begin);
            output_str[end - begin] = '\0';
        }
    }

    return end;
}

//outr attempt at a safe (from buffer overflow) version of printf     
void printd(char* format, ...){
    
    va_list argptr; // Create variable argptr of data structure va_list
    va_start(argptr, format); // void va_start(va_list ap, last_arg) initializes ap variable to be used with the va_arg and va_end macros.
    
    // Create character array debug_string of size MAX_DEBUG_STRING_LENGTH
    char debug_string[MAX_DEBUG_STRING_LENGTH];
    // The vsnprintf() used to write a formatted string to a string buffer.vsnprintf(debug_string,MAX_DEBUG_STRING_LENGTH,format, argptr);
    vsnprintf(debug_string,MAX_DEBUG_STRING_LENGTH,format, argptr);
    // Sends a NULL terminated string to the TX buffer for transmission.
    Debug_UART_PutString(debug_string);
    
    va_end(argptr); // macro void va_end(va_list ap) allows a function with variable arguments which used the va_start macro to return. 
    
}
