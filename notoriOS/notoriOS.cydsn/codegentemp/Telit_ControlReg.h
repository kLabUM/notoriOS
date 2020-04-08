/*******************************************************************************
* File Name: Telit_ControlReg.h  
* Version 1.80
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CONTROL_REG_Telit_ControlReg_H) /* CY_CONTROL_REG_Telit_ControlReg_H */
#define CY_CONTROL_REG_Telit_ControlReg_H

#include "cyfitter.h"

#if ((CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3) || \
     (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC4) || \
     (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC5))
    #include "cytypes.h"
#else
    #include "syslib/cy_syslib.h"
#endif

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 controlState;

} Telit_ControlReg_BACKUP_STRUCT;


/***************************************
*         Function Prototypes 
***************************************/

void    Telit_ControlReg_Write(uint8 control) ;
uint8   Telit_ControlReg_Read(void) ;

void Telit_ControlReg_SaveConfig(void) ;
void Telit_ControlReg_RestoreConfig(void) ;
void Telit_ControlReg_Sleep(void) ; 
void Telit_ControlReg_Wakeup(void) ;


/***************************************
*            Registers        
***************************************/

/* Control Register */
#define Telit_ControlReg_Control        (* (reg8 *) Telit_ControlReg_Sync_ctrl_reg__CONTROL_REG )
#define Telit_ControlReg_Control_PTR    (  (reg8 *) Telit_ControlReg_Sync_ctrl_reg__CONTROL_REG )

#endif /* End CY_CONTROL_REG_Telit_ControlReg_H */


/* [] END OF FILE */
