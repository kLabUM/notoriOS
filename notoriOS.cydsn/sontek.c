#include "sontek.h"
#include "notoriOS.h"


uint8 App_Sontek(){
	//In notorios.c will return timeToSontek flag back to 0 (off) until next time
return 0u;
}


void Sontek_Update(char * message){
   strcpy(Sontek_inbox, message);
}


test_t Sontek_test(){       
   test_t test; // test_t is a new data type we defined in test.h. We then use that data type to define a structure variable test
   test.status = 0; // set test status to zero
   snprintf(test.test_name,sizeof(test.test_name),"TEST_Sontek");
   //Example of code for printing error messages of test if desired
   snprintf(test.reason,sizeof(test.reason),"nWANTED=%d,nMEDIAN=%d.",
       N_READINGS,
       sensor.level_reading
   );

   return test;
}
