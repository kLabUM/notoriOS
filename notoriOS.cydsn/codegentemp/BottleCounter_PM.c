/*******************************************************************************
* File Name: BottleCounter_PM.c  
* Version 3.0
*
*  Description:
*    This file provides the power management source code to API for the
*    Counter.  
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "BottleCounter.h"

static BottleCounter_backupStruct BottleCounter_backup;


/*******************************************************************************
* Function Name: BottleCounter_SaveConfig
********************************************************************************
* Summary:
*     Save the current user configuration
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  BottleCounter_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void BottleCounter_SaveConfig(void) 
{
    #if (!BottleCounter_UsingFixedFunction)

        BottleCounter_backup.CounterUdb = BottleCounter_ReadCounter();

        #if(!BottleCounter_ControlRegRemoved)
            BottleCounter_backup.CounterControlRegister = BottleCounter_ReadControlRegister();
        #endif /* (!BottleCounter_ControlRegRemoved) */

    #endif /* (!BottleCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: BottleCounter_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  BottleCounter_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void BottleCounter_RestoreConfig(void) 
{      
    #if (!BottleCounter_UsingFixedFunction)

       BottleCounter_WriteCounter(BottleCounter_backup.CounterUdb);

        #if(!BottleCounter_ControlRegRemoved)
            BottleCounter_WriteControlRegister(BottleCounter_backup.CounterControlRegister);
        #endif /* (!BottleCounter_ControlRegRemoved) */

    #endif /* (!BottleCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: BottleCounter_Sleep
********************************************************************************
* Summary:
*     Stop and Save the user configuration
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  BottleCounter_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void BottleCounter_Sleep(void) 
{
    #if(!BottleCounter_ControlRegRemoved)
        /* Save Counter's enable state */
        if(BottleCounter_CTRL_ENABLE == (BottleCounter_CONTROL & BottleCounter_CTRL_ENABLE))
        {
            /* Counter is enabled */
            BottleCounter_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            BottleCounter_backup.CounterEnableState = 0u;
        }
    #else
        BottleCounter_backup.CounterEnableState = 1u;
        if(BottleCounter_backup.CounterEnableState != 0u)
        {
            BottleCounter_backup.CounterEnableState = 0u;
        }
    #endif /* (!BottleCounter_ControlRegRemoved) */
    
    BottleCounter_Stop();
    BottleCounter_SaveConfig();
}


/*******************************************************************************
* Function Name: BottleCounter_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*  
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  BottleCounter_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void BottleCounter_Wakeup(void) 
{
    BottleCounter_RestoreConfig();
    #if(!BottleCounter_ControlRegRemoved)
        if(BottleCounter_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            BottleCounter_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!BottleCounter_ControlRegRemoved) */
    
}


/* [] END OF FILE */
