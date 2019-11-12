/*******************************************************************************
* File Name: Debug_PM.c
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

#include "Debug.h"


/***************************************
* Local data allocation
***************************************/

static Debug_BACKUP_STRUCT  Debug_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: Debug_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the Debug_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Debug_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Debug_SaveConfig(void)
{
    #if(Debug_CONTROL_REG_REMOVED == 0u)
        Debug_backup.cr = Debug_CONTROL_REG;
    #endif /* End Debug_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Debug_RestoreConfig
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
*  Debug_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling Debug_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void Debug_RestoreConfig(void)
{
    #if(Debug_CONTROL_REG_REMOVED == 0u)
        Debug_CONTROL_REG = Debug_backup.cr;
    #endif /* End Debug_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Debug_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The Debug_Sleep() API saves the current component state. Then it
*  calls the Debug_Stop() function and calls 
*  Debug_SaveConfig() to save the hardware configuration.
*  Call the Debug_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Debug_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Debug_Sleep(void)
{
    #if(Debug_RX_ENABLED || Debug_HD_ENABLED)
        if((Debug_RXSTATUS_ACTL_REG  & Debug_INT_ENABLE) != 0u)
        {
            Debug_backup.enableState = 1u;
        }
        else
        {
            Debug_backup.enableState = 0u;
        }
    #else
        if((Debug_TXSTATUS_ACTL_REG  & Debug_INT_ENABLE) !=0u)
        {
            Debug_backup.enableState = 1u;
        }
        else
        {
            Debug_backup.enableState = 0u;
        }
    #endif /* End Debug_RX_ENABLED || Debug_HD_ENABLED*/

    Debug_Stop();
    Debug_SaveConfig();
}


/*******************************************************************************
* Function Name: Debug_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  Debug_Sleep() was called. The Debug_Wakeup() function
*  calls the Debug_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  Debug_Sleep() function was called, the Debug_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Debug_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Debug_Wakeup(void)
{
    Debug_RestoreConfig();
    #if( (Debug_RX_ENABLED) || (Debug_HD_ENABLED) )
        Debug_ClearRxBuffer();
    #endif /* End (Debug_RX_ENABLED) || (Debug_HD_ENABLED) */
    #if(Debug_TX_ENABLED || Debug_HD_ENABLED)
        Debug_ClearTxBuffer();
    #endif /* End Debug_TX_ENABLED || Debug_HD_ENABLED */

    if(Debug_backup.enableState != 0u)
    {
        Debug_Enable();
    }
}


/* [] END OF FILE */
