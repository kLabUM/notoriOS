/*******************************************************************************
* File Name: Level_Sensor_ISR.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_Level_Sensor_ISR_H)
#define CY_ISR_Level_Sensor_ISR_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void Level_Sensor_ISR_Start(void);
void Level_Sensor_ISR_StartEx(cyisraddress address);
void Level_Sensor_ISR_Stop(void);

CY_ISR_PROTO(Level_Sensor_ISR_Interrupt);

void Level_Sensor_ISR_SetVector(cyisraddress address);
cyisraddress Level_Sensor_ISR_GetVector(void);

void Level_Sensor_ISR_SetPriority(uint8 priority);
uint8 Level_Sensor_ISR_GetPriority(void);

void Level_Sensor_ISR_Enable(void);
uint8 Level_Sensor_ISR_GetState(void);
void Level_Sensor_ISR_Disable(void);

void Level_Sensor_ISR_SetPending(void);
void Level_Sensor_ISR_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the Level_Sensor_ISR ISR. */
#define Level_Sensor_ISR_INTC_VECTOR            ((reg32 *) Level_Sensor_ISR__INTC_VECT)

/* Address of the Level_Sensor_ISR ISR priority. */
#define Level_Sensor_ISR_INTC_PRIOR             ((reg8 *) Level_Sensor_ISR__INTC_PRIOR_REG)

/* Priority of the Level_Sensor_ISR interrupt. */
#define Level_Sensor_ISR_INTC_PRIOR_NUMBER      Level_Sensor_ISR__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable Level_Sensor_ISR interrupt. */
#define Level_Sensor_ISR_INTC_SET_EN            ((reg32 *) Level_Sensor_ISR__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the Level_Sensor_ISR interrupt. */
#define Level_Sensor_ISR_INTC_CLR_EN            ((reg32 *) Level_Sensor_ISR__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the Level_Sensor_ISR interrupt state to pending. */
#define Level_Sensor_ISR_INTC_SET_PD            ((reg32 *) Level_Sensor_ISR__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the Level_Sensor_ISR interrupt. */
#define Level_Sensor_ISR_INTC_CLR_PD            ((reg32 *) Level_Sensor_ISR__INTC_CLR_PD_REG)


#endif /* CY_ISR_Level_Sensor_ISR_H */


/* [] END OF FILE */
