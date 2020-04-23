#include "influx.h"

                        
                        
unsigned int generate_influx_write_body(char* body, char *node_id){
   
    unsigned int length = 0;
    body = "";
    
    //construct influx body
    for(uint16 i;i<DATA_MAX_VALUES;i++){
         sprintf(body, "%s%s,node_id=%s, value=%s %ld\n", body, data[i].key, node_id, data[i].value, data[i].timeStamp);
    }
    
    unsigned int request_len = strlen(body);
   
    length = strlen(body);
    return length;
}