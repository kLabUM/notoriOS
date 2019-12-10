/*******************************************************************************
* File Name: Sensors_PM.c
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

#include "Sensors.h"


/***************************************
* Local data allocation
***************************************/

static Sensors_BACKUP_STRUCT  Sensors_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: Sensors_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the Sensors_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Sensors_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Sensors_SaveConfig(void)
{
    #if(Sensors_CONTROL_REG_REMOVED == 0u)
        Sensors_backup.cr = Sensors_CONTROL_REG;
    #endif /* End Sensors_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Sensors_RestoreConfig
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
*  Sensors_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling Sensors_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void Sensors_RestoreConfig(void)
{
    #if(Sensors_CONTROL_REG_REMOVED == 0u)
        Sensors_CONTROL_REG = Sensors_backup.cr;
    #endif /* End Sensors_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Sensors_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The Sensors_Sleep() API saves the current component state. Then it
*  calls the Sensors_Stop() function and calls 
*  Sensors_SaveConfig() to save the hardware configuration.
*  Call the Sensors_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Sensors_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Sensors_Sleep(void)
{
    #if(Sensors_RX_ENABLED || Sensors_HD_ENABLED)
        if((Sensors_RXSTATUS_ACTL_REG  & Sensors_INT_ENABLE) != 0u)
        {
            Sensors_backup.enableState = 1u;
        }
        else
        {
            Sensors_backup.enableState = 0u;
        }
    #else
        if((Sensors_TXSTATUS_ACTL_REG  & Sensors_INT_ENABLE) !=0u)
        {
            Sensors_backup.enableState = 1u;
        }
        else
        {
            Sensors_backup.enableState = 0u;
        }
    #endif /* End Sensors_RX_ENABLED || Sensors_HD_ENABLED*/

    Sensors_Stop();
    Sensors_SaveConfig();
}


/*******************************************************************************
* Function Name: Sensors_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  Sensors_Sleep() was called. The Sensors_Wakeup() function
*  calls the Sensors_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  Sensors_Sleep() function was called, the Sensors_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Sensors_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Sensors_Wakeup(void)
{
    Sensors_RestoreConfig();
    #if( (Sensors_RX_ENABLED) || (Sensors_HD_ENABLED) )
        Sensors_ClearRxBuffer();
    #endif /* End (Sensors_RX_ENABLED) || (Sensors_HD_ENABLED) */
    #if(Sensors_TX_ENABLED || Sensors_HD_ENABLED)
        Sensors_ClearTxBuffer();
    #endif /* End Sensors_TX_ENABLED || Sensors_HD_ENABLED */

    if(Sensors_backup.enableState != 0u)
    {
        Sensors_Enable();
    }
}


/* [] END OF FILE */
