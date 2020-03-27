/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
/**
 * @file sdcard.c
 * @brief  Write to an SD card.
 * @author Brandon Wong
 * @version 0.0.01
 * @date 2019-05-15
 */
#include <stdio.h>
#include "sd_card.h"
#include "notoriOS.h"

// Preallocate card
char    SD_dir[10]      = {'\0'};
char    SD_body[3000]   = {'\0'};
char    SD_filename[30] = "Filename.txt";
char    SD_filemode[5]  = "a+";

void SD_power_up();
void SD_power_down();

///NOTE -- IF SD CARD is not plugged it, the board will drawe more power.



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
 *
 * @param dir_name is the name of the directory to create
 *
 * @return 1u on success, 0u otherwise
 */
uint8 SD_mkdir(char* dir_name)
{
    uint8 status = 9; // initialize to a value not 0 or -1
    
    // Wakeup the SD Card component
    SD_power_up();
    
    sprintf(SD_dir,"%.8s",dir_name); 
    status = (uint8) ( 1+FS_MkDir(SD_dir) );
    // The arithmetic makes it 0 if a failure, 1 if successful
    
    // Turn off the SD Card
    SD_power_down();
    
    return status;
}

/**
 * @brief Delete a file
 *
 * @param dir_name is the name of the directory to create
 *
 * @return 1u on success, 0u otherwise
 */
uint8 SD_rmfile(const char * fileName)
{
    uint8 status = 9; // initialize to a value not 0 or -1
    
    // Wakeup the SD Card component
    SD_power_up();
    
   
    status = (uint8) ( 1+FS_Remove(fileName));
    // The arithmetic makes it 0 if a failure, 1 if successful
    
    // Turn off the SD Card
    SD_power_down();
    
    return status;
}



/**
 * @brief Write data to the SD card
 *
 * @param fileName is the name of the file to write to
 * @param pMode is the write mode
 * @param pData is the data to be written
 *
 * @return 1u on success, 0u otherwise
 */
uint8 SD_write(const char * fileName, const char * pMode, const void * pData)
{
    uint8 status = 9u; // initialize to a value not 0 or 1
    FS_FILE *pFile;

    // Wakeup the SD Card component
    SD_power_up();
    
    pFile = FS_FOpen(fileName, pMode);
    
    // If the SD card doesn't open, try reinitializing the filesystem
    if (pFile == 0){
        FS_DeInit();
        FS_Init();
        pFile = FS_FOpen(fileName, pMode);
    }
    
    // Note: We are using the "No Long File Name (LFN)" library,
    //  so names are limited to 8 characters and 3 bytes for extensions
    if (pFile != 0) {
        FS_Write(pFile, pData, strlen(pData));
        status = (uint8) ( -1*FS_FClose(pFile) )+1;
        // FS_FClose returns 1 if there's a failure, 0 if successful
        // The arithmetic makes it 0 if a failure, 1 if successful
    }
    
    // Turn off the SD Card
    SD_power_down();

    
    return status;
}

/**
 * @brief Write data to the SD card
 *
 * @param fileName is the name of the file to write to
 * @param pMode is the write mode
 * @param pData is the data to be written
 *
 * @return 1u on success, 0u otherwise
 */
uint8 SD_read(const char * fileName,  void * pData, unsigned long NumBytes)
{
    uint8 status = 9u; // initialize to a value not 0 or 1
    FS_FILE *pFile;

    // Powerup the SD Card component
    SD_power_up();
    
    pFile = FS_FOpen(fileName, "r");
    
    // If the SD card doesn't open, try reinitializing the filesystem
    if (pFile == 0){
        FS_DeInit();
        FS_Init();
        pFile = FS_FOpen(fileName, "r");
    }
    
    // Note: We are using the "No Long File Name (LFN)" library,
    //  so names are limited to 8 characters and 3 bytes for extensions
    if (pFile != 0) {
        FS_Read(pFile,pData,NumBytes);
        status = (uint8) ( -1*FS_FClose(pFile) )+1;
        // FS_FClose returns 1 if there's a failure, 0 if successful
        // The arithmetic makes it 0 if a failure, 1 if successful
    }
    
    // power down the SD card
    SD_power_down();
    
    return status;
}

void SD_power_up(){

    emFile_Wakeup();
    
    // Turn on the SD Card
    SD_Card_Power_Write(ON);
    FS_Init();  
}

void SD_power_down(){
    // Turn off the SD Card
    SD_Card_Power_Write(OFF);   
    emFile_Sleep();
    FS_DeInit(); 
    emFile_miso0_SetDriveMode(PIN_DM_DIG_HIZ);
    emFile_mosi0_SetDriveMode(PIN_DM_DIG_HIZ);
    emFile_sclk0_SetDriveMode(PIN_DM_DIG_HIZ);
    emFile_SPI0_CS_SetDriveMode(PIN_DM_DIG_HIZ);

    emFile_miso0_Write(OFF);
    emFile_mosi0_Write(OFF);
    emFile_sclk0_Write(OFF);
    emFile_SPI0_CS_Write(OFF);
//    SD_Chip_Detect_Write(OFF);
}


test_t SD_card_test(){
    //create a  file, write random number, read number from file, delete directory from file
    test_t test;
    sprintf(test.test_name,"TEST_SD_CARD\r");
    test.status = 0;
    
    char data[] = "TEST_FAIL";
    char fileName[] = "test.txt";
    
    SD_rmfile(fileName);
    SD_write(fileName,"w","TEST_PASS");
    SD_read(fileName,data,sizeof(data));
    test.status = SD_rmfile(fileName);
    
    if(test.status == 1){
        sprintf(test.reason,"SUCCESS WRITE READ DELETE FILE\r");
    }else{
       sprintf(test.reason,"FAILED TO WRITE READ DELETE FILE\r"); 
    }
    
    return test;
    
}


    

