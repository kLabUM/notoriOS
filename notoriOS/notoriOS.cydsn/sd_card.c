/**
 * @file sdcard.c
 * @brief  Write to an SD card.
 * @author Brandon Wong
 * @version 0.0.01
 * @date 2019-05-15
 */
#include <stdio.h>  // Defines three variable types, several macros, and various functions for performing input and output.
#include "sd_card.h"
#include "notoriOS.h"

// Preallocate card
char    SD_dir[10]      = {'\0'};
char    SD_body[3000]   = {'\0'};
char    SD_filename[30] = "Filename.txt";
char    SD_filemode[5]  = "a+";


void SD_power_up();
void SD_power_down();

///NOTE -- IF SD CARD is not plugged it, the board will draw more power.


/** SD_init()
* @brief Initialize the file system for the SD card
* @param Null
* @return void
*/
void SD_init()
{
    /* Uncomment to enable support for long file names
     * Note: To use long file names (LFN) support on PSoC 5LP devices, you must call
     * FS_FAT_SupportLFN(). For PSoC 3 devices, this feature is enabled by default.
     */
    //FS_FAT_SupportLFN();    
}

/**
 * @brief Make a directory on the SD card
 * @param dir_name is the name of the directory to create
 * @return 1u on success, 0u otherwise
 */
uint8 SD_mkdir(char* dir_name)
{
    uint8 status = 9; // Initialize to a value not 0 or -1
    
    SD_power_up(); // Wakeup the SD Card component
    // The snprintf() function formats and stores a series of characters and values in the array buffer.
    snprintf(SD_dir,sizeof(SD_dir),"%.8s",dir_name); 
    
    // FS_MKDir() creates a directory using FS.h.
    // Typecasting (1+FS_MkDir(SD_dir)) into an uint8 datatype
    // This makes it 0 if it's a failture, 1 if it's successful
    status = (uint8) ( 1+FS_MkDir(SD_dir) );
    
    SD_power_down(); // Turn off the SD Card
    
    return status;
}

/**
 * @brief Delete a file
 * @param dir_name is the name of the directory to create
 * @return 1u on success, 0u otherwise
 */
uint8 SD_rmfile(const char * fileName)
{
    uint8 status = 9; // initialize to a value not 0 or -1

    SD_power_up(); // Wakeup the SD Card component
    
    // FS_Remove() removes a file using FS.h.
    // Typecasting (1+FS_Remove(fileName)) into an uint8 datatype
    // This makes it 0 if it's a failture, 1 if it's successful
    status = (uint8) ( 1+FS_Remove(fileName));
    
    SD_power_down(); // Turn off the SD Card
    
    return status;
}

/**
 * @brief Write data to the SD card
 * @param fileName is the name of the file to write to
 * @param pMode is the write mode
 * @param pData is the data to be written
 * @return 1u on success, 0u otherwise
 */
uint8 SD_write(const char * fileName, const char * pMode, const void * pData)
{
    uint8 status = 9u; // Initialize to a value not 0 or 1
    FS_FILE *pFile; // FS_FILE data structure defined in FS.h

    SD_power_up(); // Wakeup the SD Card component
    
    // FS_FOpen() opens an existing file or creates a new one.
    // fileName is the file to be opened or created. It's a 0-termianted string that cannot be NULL.
    // pMode indicates how the file should be opened. It's a 0-terminated string that cannot be NULL.
    pFile = FS_FOpen(fileName, pMode);
    
    // If the SD card doesn't open, try reinitializing the filesystem
    if (pFile == 0){
        FS_DeInit(); // FS_DeInit() frees allocated resources
        FS_Init(); // FS_Init() starts the file system
        pFile = FS_FOpen(fileName, pMode);
    }
    
    // Note: We are using the "No Long File Name (LFN)" library,
    //  so names are limited to 8 characters and 3 bytes for extensions
    if (pFile != 0) {
        // FS_Write() writes data to file: pFile is handle to open file, pData is data to be written to file, and strlen(pData) is the number of bytes to be written to file.
        FS_Write(pFile, pData, strlen(pData));
        
        // FS_FClose() closes an opened file using FS.h. It returns 1 if there's a failure, 0 if successful.
        // Typecasting (1+FS_FClose(pFile)) into an uint8 datatype
        // This makes it 0 if it's a failture, 1 if it's successful
        status = (uint8) ( -1*FS_FClose(pFile) )+1;
    }
    
    SD_power_down(); // Turn off the SD Card

    
    return status;
}

/**
 * @brief Write data to the SD card
 * @param fileName is the name of the file to write to
 * @param pMode is the write mode
 * @param pData is the data to be written
 * @return 1u on success, 0u otherwise
 */
uint8 SD_read(const char * fileName,  void * pData, unsigned long NumBytes)
{
    uint8 status = 9u; // initialize to a value not 0 or 1
    FS_FILE *pFile; // FS_FILE data structure defined in FS.h

    SD_power_up(); // Powerup the SD Card component
    
    // FS_FOpen() opens an existing file or creates a new one.
    // fileName is the file to be opened or created. It's a 0-terminated string that cannot be NULL.
    // "r" indicates how the file should be opened, in this case, for reading.
    pFile = FS_FOpen(fileName, "r");
    
    // If the SD card doesn't open, try reinitializing the filesystem
    if (pFile == 0){
        FS_DeInit();// FS_DeInit() frees allocated resources
        FS_Init(); // FS_Init() starts the file system
        pFile = FS_FOpen(fileName, "r");
    }
    
    // Note: We are using the "No Long File Name (LFN)" library,
    //  so names are limited to 8 characters and 3 bytes for extensions
    if (pFile != 0) {
        // FS_Read() reads data from a file: pFile is handle to an opened file, pData is the buffer to receive the read data, and strlen(pData) is NumBytes is the # of bytes to read.
        FS_Read(pFile,pData,NumBytes);
        
        // FS_FClose() closes an opened file using FS.h. It returns 1 if there's a failure, 0 if successful.
        // Typecasting (1+FS_FClose(pFile)) into an uint8 datatype
        // This makes it 0 if it's a failture, 1 if it's successful
        status = (uint8) ( -1*FS_FClose(pFile) )+1;
    }
    
    SD_power_down(); // Power down the SD card
    
    return status;
}

// Power up SD Card
void SD_power_up(){

    emFile_Wakeup(); // Prepare SPIM Components to wake up.
    
    SD_Card_Power_Write(ON); // Turn on the SD Card (pulls it high).
    FS_Init(); // FS_Init() starts the file system
}

// Power down SD Card
void SD_power_down(){
    
    SD_Card_Power_Write(OFF);  // Turn off the SD Card (pulls it low). 
    emFile_Sleep(); // emFIle_Sleep() prepare emFile to go to sleep.
    FS_DeInit(); // FS_DeInit() frees allocated resources
    
    emFile_miso0_Write(OFF); // emFile_miso0_Write(): Assigns a new value to the digital port's data output register. MISO: Master In Slave Out (DataOut Output Pin).
    emFile_mosi0_Write(OFF); // emFile_mosi0_Write(): Assigns a new value to the digital port's data output register. MOSI: Master Out Slave In (DataIn Input Pin).
    emFile_sclk0_Write(OFF); // emFile_sclk0_Write(): Assigns a new value to the digital port's data output register. SCLK:  Clock signal (Input Pin).
    emFile_SPI0_CS_Write(OFF); // emFile_SPIO_CS_Write(): Assigns a new value to the digital port's data output register.  
    // SD_Chip_Detect_Write(OFF);
}

// Testing the SD Card - create a  file, write random number, read number from file, delete directory from file
test_t SD_card_test(){
    
    // test_t is a new data type we defined in test.h. We then use that data type to define a structure variable test
    test_t test;
    snprintf(test.test_name,sizeof(test.test_name),"TEST_SD_CARD");
    test.status = 0;
    
    char data[] = "TEST_FAIL";
    char fileName[] = "test.txt";
    
    SD_rmfile(fileName); // Remove file from SD Card
    SD_write(fileName,"w","TEST_PASS"); // Write data to SD Card
    SD_read(fileName,data,sizeof(data)); // Read data from the SD card
    test.status = SD_rmfile(fileName);
    
    if(test.status == 1){
        snprintf(test.reason,sizeof(test.reason),"SUCCESS WRITE READ DELETE FILE");
    }else{
       snprintf(test.reason,sizeof(test.reason),"FAILED TO WRITE READ DELETE FILE"); 
    }
    
    return test;
    
}


    

