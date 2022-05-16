/*******************************************************************************
* File Name: Downstream_Level_Sensor_UART_PM.c
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

#include "Downstream_Level_Sensor_UART.h"


/***************************************
* Local data allocation
***************************************/

static Downstream_Level_Sensor_UART_BACKUP_STRUCT  Downstream_Level_Sensor_UART_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: Downstream_Level_Sensor_UART_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the Downstream_Level_Sensor_UART_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Downstream_Level_Sensor_UART_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Downstream_Level_Sensor_UART_SaveConfig(void)
{
    #if(Downstream_Level_Sensor_UART_CONTROL_REG_REMOVED == 0u)
        Downstream_Level_Sensor_UART_backup.cr = Downstream_Level_Sensor_UART_CONTROL_REG;
    #endif /* End Downstream_Level_Sensor_UART_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Downstream_Level_Sensor_UART_RestoreConfig
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
*  Downstream_Level_Sensor_UART_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling Downstream_Level_Sensor_UART_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void Downstream_Level_Sensor_UART_RestoreConfig(void)
{
    #if(Downstream_Level_Sensor_UART_CONTROL_REG_REMOVED == 0u)
        Downstream_Level_Sensor_UART_CONTROL_REG = Downstream_Level_Sensor_UART_backup.cr;
    #endif /* End Downstream_Level_Sensor_UART_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Downstream_Level_Sensor_UART_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The Downstream_Level_Sensor_UART_Sleep() API saves the current component state. Then it
*  calls the Downstream_Level_Sensor_UART_Stop() function and calls 
*  Downstream_Level_Sensor_UART_SaveConfig() to save the hardware configuration.
*  Call the Downstream_Level_Sensor_UART_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Downstream_Level_Sensor_UART_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Downstream_Level_Sensor_UART_Sleep(void)
{
    #if(Downstream_Level_Sensor_UART_RX_ENABLED || Downstream_Level_Sensor_UART_HD_ENABLED)
        if((Downstream_Level_Sensor_UART_RXSTATUS_ACTL_REG  & Downstream_Level_Sensor_UART_INT_ENABLE) != 0u)
        {
            Downstream_Level_Sensor_UART_backup.enableState = 1u;
        }
        else
        {
            Downstream_Level_Sensor_UART_backup.enableState = 0u;
        }
    #else
        if((Downstream_Level_Sensor_UART_TXSTATUS_ACTL_REG  & Downstream_Level_Sensor_UART_INT_ENABLE) !=0u)
        {
            Downstream_Level_Sensor_UART_backup.enableState = 1u;
        }
        else
        {
            Downstream_Level_Sensor_UART_backup.enableState = 0u;
        }
    #endif /* End Downstream_Level_Sensor_UART_RX_ENABLED || Downstream_Level_Sensor_UART_HD_ENABLED*/

    Downstream_Level_Sensor_UART_Stop();
    Downstream_Level_Sensor_UART_SaveConfig();
}


/*******************************************************************************
* Function Name: Downstream_Level_Sensor_UART_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  Downstream_Level_Sensor_UART_Sleep() was called. The Downstream_Level_Sensor_UART_Wakeup() function
*  calls the Downstream_Level_Sensor_UART_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  Downstream_Level_Sensor_UART_Sleep() function was called, the Downstream_Level_Sensor_UART_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Downstream_Level_Sensor_UART_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Downstream_Level_Sensor_UART_Wakeup(void)
{
    Downstream_Level_Sensor_UART_RestoreConfig();
    #if( (Downstream_Level_Sensor_UART_RX_ENABLED) || (Downstream_Level_Sensor_UART_HD_ENABLED) )
        Downstream_Level_Sensor_UART_ClearRxBuffer();
    #endif /* End (Downstream_Level_Sensor_UART_RX_ENABLED) || (Downstream_Level_Sensor_UART_HD_ENABLED) */
    #if(Downstream_Level_Sensor_UART_TX_ENABLED || Downstream_Level_Sensor_UART_HD_ENABLED)
        Downstream_Level_Sensor_UART_ClearTxBuffer();
    #endif /* End Downstream_Level_Sensor_UART_TX_ENABLED || Downstream_Level_Sensor_UART_HD_ENABLED */

    if(Downstream_Level_Sensor_UART_backup.enableState != 0u)
    {
        Downstream_Level_Sensor_UART_Enable();
    }
}


/* [] END OF FILE */
