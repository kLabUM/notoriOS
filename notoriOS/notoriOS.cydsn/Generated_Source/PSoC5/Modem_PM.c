/*******************************************************************************
* File Name: Modem_PM.c
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

#include "Modem.h"


/***************************************
* Local data allocation
***************************************/

static Modem_BACKUP_STRUCT  Modem_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: Modem_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the Modem_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Modem_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Modem_SaveConfig(void)
{
    #if(Modem_CONTROL_REG_REMOVED == 0u)
        Modem_backup.cr = Modem_CONTROL_REG;
    #endif /* End Modem_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Modem_RestoreConfig
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
*  Modem_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling Modem_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void Modem_RestoreConfig(void)
{
    #if(Modem_CONTROL_REG_REMOVED == 0u)
        Modem_CONTROL_REG = Modem_backup.cr;
    #endif /* End Modem_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Modem_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The Modem_Sleep() API saves the current component state. Then it
*  calls the Modem_Stop() function and calls 
*  Modem_SaveConfig() to save the hardware configuration.
*  Call the Modem_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Modem_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Modem_Sleep(void)
{
    #if(Modem_RX_ENABLED || Modem_HD_ENABLED)
        if((Modem_RXSTATUS_ACTL_REG  & Modem_INT_ENABLE) != 0u)
        {
            Modem_backup.enableState = 1u;
        }
        else
        {
            Modem_backup.enableState = 0u;
        }
    #else
        if((Modem_TXSTATUS_ACTL_REG  & Modem_INT_ENABLE) !=0u)
        {
            Modem_backup.enableState = 1u;
        }
        else
        {
            Modem_backup.enableState = 0u;
        }
    #endif /* End Modem_RX_ENABLED || Modem_HD_ENABLED*/

    Modem_Stop();
    Modem_SaveConfig();
}


/*******************************************************************************
* Function Name: Modem_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  Modem_Sleep() was called. The Modem_Wakeup() function
*  calls the Modem_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  Modem_Sleep() function was called, the Modem_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Modem_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Modem_Wakeup(void)
{
    Modem_RestoreConfig();
    #if( (Modem_RX_ENABLED) || (Modem_HD_ENABLED) )
        Modem_ClearRxBuffer();
    #endif /* End (Modem_RX_ENABLED) || (Modem_HD_ENABLED) */
    #if(Modem_TX_ENABLED || Modem_HD_ENABLED)
        Modem_ClearTxBuffer();
    #endif /* End Modem_TX_ENABLED || Modem_HD_ENABLED */

    if(Modem_backup.enableState != 0u)
    {
        Modem_Enable();
    }
}


/* [] END OF FILE */
