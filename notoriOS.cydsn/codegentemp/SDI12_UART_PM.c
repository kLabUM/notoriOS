/*******************************************************************************
* File Name: SDI12_UART_PM.c
* Version 2.50
*
* Description:
*  This file provides Sleep/WakeUp APIs functionality.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SDI12_UART.h"


/***************************************
* Local data allocation
***************************************/

static SDI12_UART_BACKUP_STRUCT  SDI12_UART_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: SDI12_UART_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the SDI12_UART_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  SDI12_UART_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SDI12_UART_SaveConfig(void)
{
    #if(SDI12_UART_CONTROL_REG_REMOVED == 0u)
        SDI12_UART_backup.cr = SDI12_UART_CONTROL_REG;
    #endif /* End SDI12_UART_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: SDI12_UART_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the nonretention control register except FIFO.
*  Does not restore the FIFO which is a set of nonretention registers.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  SDI12_UART_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling SDI12_UART_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void SDI12_UART_RestoreConfig(void)
{
    #if(SDI12_UART_CONTROL_REG_REMOVED == 0u)
        SDI12_UART_CONTROL_REG = SDI12_UART_backup.cr;
    #endif /* End SDI12_UART_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: SDI12_UART_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The SDI12_UART_Sleep() API saves the current component state. Then it
*  calls the SDI12_UART_Stop() function and calls 
*  SDI12_UART_SaveConfig() to save the hardware configuration.
*  Call the SDI12_UART_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  SDI12_UART_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SDI12_UART_Sleep(void)
{
    #if(SDI12_UART_RX_ENABLED || SDI12_UART_HD_ENABLED)
        if((SDI12_UART_RXSTATUS_ACTL_REG  & SDI12_UART_INT_ENABLE) != 0u)
        {
            SDI12_UART_backup.enableState = 1u;
        }
        else
        {
            SDI12_UART_backup.enableState = 0u;
        }
    #else
        if((SDI12_UART_TXSTATUS_ACTL_REG  & SDI12_UART_INT_ENABLE) !=0u)
        {
            SDI12_UART_backup.enableState = 1u;
        }
        else
        {
            SDI12_UART_backup.enableState = 0u;
        }
    #endif /* End SDI12_UART_RX_ENABLED || SDI12_UART_HD_ENABLED*/

    SDI12_UART_Stop();
    SDI12_UART_SaveConfig();
}


/*******************************************************************************
* Function Name: SDI12_UART_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  SDI12_UART_Sleep() was called. The SDI12_UART_Wakeup() function
*  calls the SDI12_UART_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  SDI12_UART_Sleep() function was called, the SDI12_UART_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  SDI12_UART_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SDI12_UART_Wakeup(void)
{
    SDI12_UART_RestoreConfig();
    #if( (SDI12_UART_RX_ENABLED) || (SDI12_UART_HD_ENABLED) )
        SDI12_UART_ClearRxBuffer();
    #endif /* End (SDI12_UART_RX_ENABLED) || (SDI12_UART_HD_ENABLED) */
    #if(SDI12_UART_TX_ENABLED || SDI12_UART_HD_ENABLED)
        SDI12_UART_ClearTxBuffer();
    #endif /* End SDI12_UART_TX_ENABLED || SDI12_UART_HD_ENABLED */

    if(SDI12_UART_backup.enableState != 0u)
    {
        SDI12_UART_Enable();
    }
}


/* [] END OF FILE */
