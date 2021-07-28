/*******************************************************************************
* File Name: DO_ControlReg_PM.c
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

#include "DO_ControlReg.h"

/* Check for removal by optimization */
#if !defined(DO_ControlReg_Sync_ctrl_reg__REMOVED)

static DO_ControlReg_BACKUP_STRUCT  DO_ControlReg_backup = {0u};

    
/*******************************************************************************
* Function Name: DO_ControlReg_SaveConfig
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
void DO_ControlReg_SaveConfig(void) 
{
    DO_ControlReg_backup.controlState = DO_ControlReg_Control;
}


/*******************************************************************************
* Function Name: DO_ControlReg_RestoreConfig
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
void DO_ControlReg_RestoreConfig(void) 
{
     DO_ControlReg_Control = DO_ControlReg_backup.controlState;
}


/*******************************************************************************
* Function Name: DO_ControlReg_Sleep
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
void DO_ControlReg_Sleep(void) 
{
    DO_ControlReg_SaveConfig();
}


/*******************************************************************************
* Function Name: DO_ControlReg_Wakeup
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
void DO_ControlReg_Wakeup(void)  
{
    DO_ControlReg_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
