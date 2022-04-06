/*******************************************************************************
* File Name: mux_controller_PM.c
* Version 1.80
*
* Description:
*  This file contains the setup, control, and status commands to support 
*  the component operation in the low power mode. 
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "mux_controller.h"

/* Check for removal by optimization */
#if !defined(mux_controller_Sync_ctrl_reg__REMOVED)

static mux_controller_BACKUP_STRUCT  mux_controller_backup = {0u};

    
/*******************************************************************************
* Function Name: mux_controller_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void mux_controller_SaveConfig(void) 
{
    mux_controller_backup.controlState = mux_controller_Control;
}


/*******************************************************************************
* Function Name: mux_controller_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*
*******************************************************************************/
void mux_controller_RestoreConfig(void) 
{
     mux_controller_Control = mux_controller_backup.controlState;
}


/*******************************************************************************
* Function Name: mux_controller_Sleep
********************************************************************************
*
* Summary:
*  Prepares the component for entering the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void mux_controller_Sleep(void) 
{
    mux_controller_SaveConfig();
}


/*******************************************************************************
* Function Name: mux_controller_Wakeup
********************************************************************************
*
* Summary:
*  Restores the component after waking up from the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void mux_controller_Wakeup(void)  
{
    mux_controller_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
