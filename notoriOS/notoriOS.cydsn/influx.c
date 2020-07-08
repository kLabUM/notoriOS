#include "influx.h"  
#include "notoriOS.h"
                        
// Prepare data from data wheel to send to influx
unsigned int construct_influx_write_body(char* body, char *node_id){
   
    body[0] = '\0'; // Set the array variable body to be 0
 
    // Get data from the data stack and put into http body to send to influx
    for(uint16 i = 0;i<sizeOfDataStack();i++){
        // Note that influx uses a timestamp in nano-seconds, so we have to add 9 zeros
        snprintf(body,sizeof(http_body), "%s%s,node_id=%s value=%s %ld000000000\n", body, data[i].key, node_id, data[i].value, data[i].timeStamp);
    }
    
    // The strlen() function calculates the length of a given string. The strlen() function takes a string as an argument and returns its length. The returned value is of type long int. It is defined in the <string.h> header file.
    unsigned int request_len = strlen(body);
   

    return request_len;
}

// Create the route to connect to the influx database
void construct_influx_route(char* route, char* base, char* user, char* pass, char* database){
    // Detting a string to \0 is an easy way to set the string to be 0 because \0 denotes the end of a string
    route[0] = '\0';
    snprintf(route,sizeof(http_route), "%s%s?u=%s&p=%s&db=%s", route, base, user, pass, database); 
}

// Test the influx server
test_t influx_test(){
    test_t test; // Create variable test of data structure test_t
    test.status = 0u;
    snprintf(test.test_name,sizeof(test.test_name),"INFLUX");
    
    //we'll create a few fake test, generate a string and compare it to a known value
    char *expected_string = "test_reading_1,node_id=XYZ10, value=000 12345\ntest_reading_2,node_id=XYZ10, value=111 67890\r\n";
    
    Clear_Data_Stack();
    pushData("test_reading_1", "000", 12345);
    pushData("test_reading_2", "111", 67890);
    char test_write_body[200];
    construct_influx_write_body(test_write_body, "XYZ10");
    
    // Will return zero if strings are identical.
    // int strcmp(const char *str1, const char *str2) compares the string pointed to, by str1 to the string pointed to by str2.
    int compare = strcmp(test_write_body,expected_string);
    
    // If strings identical, change test status to 1
    if(compare == 0){
        test.status = 1u;
        snprintf(test.reason,sizeof(test.reason),"Influx body generated correctly.");
    }else{
        snprintf(test.reason,sizeof(test.reason),"Influx body could not be generated correctly.");
    }
    
    Clear_Data_Stack(); // Clear the data stack
    
    return test;
}