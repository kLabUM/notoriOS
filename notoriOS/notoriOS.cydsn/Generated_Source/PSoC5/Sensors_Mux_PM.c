/*******************************************************************************
* File Name: Sensors_Mux_PM.c
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

#include "Sensors_Mux.h"

/* Check for removal by optimization */
#if !defined(Sensors_Mux_Sync_ctrl_reg__REMOVED)

static Sensors_Mux_BACKUP_STRUCT  Sensors_Mux_backup = {0u};

    
/*******************************************************************************
* Function Name: Sensors_Mux_SaveConfig
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
void Sensors_Mux_SaveConfig(void) 
{
    Sensors_Mux_backup.controlState = Sensors_Mux_Control;
}


/*******************************************************************************
* Function Name: Sensors_Mux_RestoreConfig
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
void Sensors_Mux_RestoreConfig(void) 
{
     Sensors_Mux_Control = Sensors_Mux_backup.controlState;
}


/*******************************************************************************
* Function Name: Sensors_Mux_Sleep
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
void Sensors_Mux_Sleep(void) 
{
    Sensors_Mux_SaveConfig();
}


/*******************************************************************************
* Function Name: Sensors_Mux_Wakeup
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
void Sensors_Mux_Wakeup(void)  
{
    Sensors_Mux_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
