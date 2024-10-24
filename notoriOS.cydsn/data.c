// ==============================================
//  I got stacks on stacks (on stacks)
//      - Soulja Boy (2011)
//
// This defines methods for the core data stack, which will be syunced with the server
// ==============================================
#include "data.h"
#include "notoriOS.h"


// ==============================================
//  Push It
//      - Salt-n-Peppa (1987)
//
// This pushes data onto the stack
// ==============================================
uint8 pushData(char * key, char *value, int32 timestamp){
    // Print data key, value and timestamp
    snprintf(data[dataPointsInStack].key,sizeof(data[dataPointsInStack].key), "%s", key);
    snprintf(data[dataPointsInStack].value,sizeof(data[dataPointsInStack].value), "%s", value);
    data[dataPointsInStack].timeStamp = timestamp;
    
    // Track the number of actual (dataPointsInStack) and desired (dataPointsInStackPointer) data points in the stack
    dataPointsInStack++;
    dataPointsInStackDesired++;
    
    // Circular buffer
    // If stack is full, loop around and rewrite over the first point.
    // Keep datPointsInStackDesired increasing to track total number of desired points.
    // Both reset to zero once buffer is flushed ( see Clear_Data_Stack() ).
    
    if(dataPointsInStack >= DATA_MAX_VALUES){    
        dataPointsInStack = 0;
        buffer_overflow = true;
        printNotif(NOTIF_TYPE_ERROR,"Data stack full. Begin overwrite of circular buffer.");
        return 0; // If stack full, and points need to be deleted, return 0 
    }
    
    return 1; // If data pushed succesfully, return 1
    
}

// Initalize Data Stack -- sets counter and pointer to zero
void Initialize_Data_Stack(){
    Clear_Data_Stack(); // Clear out whatever data was in the stack.
}

// Clear the data stack
void Clear_Data_Stack(){
    for(uint16 i;i<DATA_MAX_VALUES;i++){
        data[i].key[0] = '\0';
        data[i].value[0] = '\0';
        data[i].timeStamp = 0;
    }
    dataPointsInStack = 0;
    dataPointsInStackDesired = 0;
    buffer_overflow = false;
   
}

// Return the desired number of points in the stack
uint16 sizeOfDataStackDesired(){
    return dataPointsInStackDesired;
}

// Return the actual number of points in the stack 
uint16 sizeOfDataStack(){
    return dataPointsInStack;
}

// This function could really use a test
void construct_generic_HTTP_request(char* request, char* body, char* host, char* route,
                               int port, char* method, char* connection_type,
	                           char *extra_headers, int extra_len, char* http_protocol){

    snprintf(request,sizeof(http_request),"%s /%s HTTP/%s\r\n", method, route, http_protocol);
    snprintf(request,sizeof(http_request),"%s%s%s%s%d%s%s%s%s",
            request, // 1
            "Host: ", host, ":", port, "\r\n", // 2 3 4 5 6
            "Connection: ", connection_type, "\r\n"); // 7 8 9
	if (extra_headers && strlen(extra_headers) > 0){
		snprintf(request,sizeof(http_request), "%s%s", request, extra_headers);
	}
	if (strcmp(method, "GET") != 0){
		snprintf(request,sizeof(http_request), "%s%s%s%d%s",
			request,
            "Content-Type: text/plain\r\n", // 10
            "Content-Length: ", (extra_len + strlen(body)), //11 12 (Extra len should be 2 for flask server)
            "\r\n\r\n"); // 13 14 15
	}
	snprintf(request,sizeof(http_request), "%s%s\r\n", request, body); 
    
}
                            
// Construct middle layer called Malcom (aka Malcom in the Middle)                                                        
unsigned int construct_malcom_body(char* body){
   
    body[0] = '\0';
    
    // Construct influx body
    // if the buffer didn't overflow, send however many data points we have in the buffer
    if (buffer_overflow == false){
        for(uint16 i = 0;i < sizeOfDataStack();i++){
            // Note that influx uses a timestamp in nano-seconds, so we have to add 9 zeros
            snprintf(body,sizeof(http_body), "%s%s, value=%s %ld000000000\n", body, data[i].key, data[i].value, data[i].timeStamp);
        }
    }else{
        // if buffer did overflow, then send the max number of values allowed in the buffer
        for(uint16 i = 0;i < DATA_MAX_VALUES;i++){
            // Note that influx uses a timestamp in nano-seconds, so we have to add 9 zeros
            snprintf(body,sizeof(http_body), "%s%s, value=%s %ld000000000\n", body, data[i].key, data[i].value, data[i].timeStamp);
        }
    }
        
        
    // Get the length of the influx body and save to request_len
    unsigned int request_len = strlen(body);
   
    return request_len; // Return the length of the influx body
}

// Construct the route to the Malcom middle layer
void construct_malcom_route(char* route, char* base, char* device, char* hash){
    route[0] = '\0';
    snprintf(route,sizeof(http_route), "%s%s?d=%s&h=%s", route, base, device, hash); 
}




