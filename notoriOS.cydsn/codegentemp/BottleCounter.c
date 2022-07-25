/*******************************************************************************
* File Name: BottleCounter.c  
* Version 3.0
*
*  Description:
*     The Counter component consists of a 8, 16, 24 or 32-bit counter with
*     a selectable period between 2 and 2^Width - 1.  
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

uint8 BottleCounter_initVar = 0u;


/*******************************************************************************
* Function Name: BottleCounter_Init
********************************************************************************
* Summary:
*     Initialize to the schematic state
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void BottleCounter_Init(void) 
{
        #if (!BottleCounter_UsingFixedFunction && !BottleCounter_ControlRegRemoved)
            uint8 ctrl;
        #endif /* (!BottleCounter_UsingFixedFunction && !BottleCounter_ControlRegRemoved) */
        
        #if(!BottleCounter_UsingFixedFunction) 
            /* Interrupt State Backup for Critical Region*/
            uint8 BottleCounter_interruptState;
        #endif /* (!BottleCounter_UsingFixedFunction) */
        
        #if (BottleCounter_UsingFixedFunction)
            /* Clear all bits but the enable bit (if it's already set for Timer operation */
            BottleCounter_CONTROL &= BottleCounter_CTRL_ENABLE;
            
            /* Clear the mode bits for continuous run mode */
            #if (CY_PSOC5A)
                BottleCounter_CONTROL2 &= ((uint8)(~BottleCounter_CTRL_MODE_MASK));
            #endif /* (CY_PSOC5A) */
            #if (CY_PSOC3 || CY_PSOC5LP)
                BottleCounter_CONTROL3 &= ((uint8)(~BottleCounter_CTRL_MODE_MASK));                
            #endif /* (CY_PSOC3 || CY_PSOC5LP) */
            /* Check if One Shot mode is enabled i.e. RunMode !=0*/
            #if (BottleCounter_RunModeUsed != 0x0u)
                /* Set 3rd bit of Control register to enable one shot mode */
                BottleCounter_CONTROL |= BottleCounter_ONESHOT;
            #endif /* (BottleCounter_RunModeUsed != 0x0u) */
            
            /* Set the IRQ to use the status register interrupts */
            BottleCounter_CONTROL2 |= BottleCounter_CTRL2_IRQ_SEL;
            
            /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
            BottleCounter_RT1 &= ((uint8)(~BottleCounter_RT1_MASK));
            BottleCounter_RT1 |= BottleCounter_SYNC;     
                    
            /*Enable DSI Sync all all inputs of the Timer*/
            BottleCounter_RT1 &= ((uint8)(~BottleCounter_SYNCDSI_MASK));
            BottleCounter_RT1 |= BottleCounter_SYNCDSI_EN;

        #else
            #if(!BottleCounter_ControlRegRemoved)
            /* Set the default compare mode defined in the parameter */
            ctrl = BottleCounter_CONTROL & ((uint8)(~BottleCounter_CTRL_CMPMODE_MASK));
            BottleCounter_CONTROL = ctrl | BottleCounter_DEFAULT_COMPARE_MODE;
            
            /* Set the default capture mode defined in the parameter */
            ctrl = BottleCounter_CONTROL & ((uint8)(~BottleCounter_CTRL_CAPMODE_MASK));
            
            #if( 0 != BottleCounter_CAPTURE_MODE_CONF)
                BottleCounter_CONTROL = ctrl | BottleCounter_DEFAULT_CAPTURE_MODE;
            #else
                BottleCounter_CONTROL = ctrl;
            #endif /* 0 != BottleCounter_CAPTURE_MODE */ 
            
            #endif /* (!BottleCounter_ControlRegRemoved) */
        #endif /* (BottleCounter_UsingFixedFunction) */
        
        /* Clear all data in the FIFO's */
        #if (!BottleCounter_UsingFixedFunction)
            BottleCounter_ClearFIFO();
        #endif /* (!BottleCounter_UsingFixedFunction) */
        
        /* Set Initial values from Configuration */
        BottleCounter_WritePeriod(BottleCounter_INIT_PERIOD_VALUE);
        #if (!(BottleCounter_UsingFixedFunction && (CY_PSOC5A)))
            BottleCounter_WriteCounter(BottleCounter_INIT_COUNTER_VALUE);
        #endif /* (!(BottleCounter_UsingFixedFunction && (CY_PSOC5A))) */
        BottleCounter_SetInterruptMode(BottleCounter_INIT_INTERRUPTS_MASK);
        
        #if (!BottleCounter_UsingFixedFunction)
            /* Read the status register to clear the unwanted interrupts */
            (void)BottleCounter_ReadStatusRegister();
            /* Set the compare value (only available to non-fixed function implementation */
            BottleCounter_WriteCompare(BottleCounter_INIT_COMPARE_VALUE);
            /* Use the interrupt output of the status register for IRQ output */
            
            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            BottleCounter_interruptState = CyEnterCriticalSection();
            
            BottleCounter_STATUS_AUX_CTRL |= BottleCounter_STATUS_ACTL_INT_EN_MASK;
            
            /* Exit Critical Region*/
            CyExitCriticalSection(BottleCounter_interruptState);
            
        #endif /* (!BottleCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: BottleCounter_Enable
********************************************************************************
* Summary:
*     Enable the Counter
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Side Effects: 
*   If the Enable mode is set to Hardware only then this function has no effect 
*   on the operation of the counter.
*
*******************************************************************************/
void BottleCounter_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (BottleCounter_UsingFixedFunction)
        BottleCounter_GLOBAL_ENABLE |= BottleCounter_BLOCK_EN_MASK;
        BottleCounter_GLOBAL_STBY_ENABLE |= BottleCounter_BLOCK_STBY_EN_MASK;
    #endif /* (BottleCounter_UsingFixedFunction) */  
        
    /* Enable the counter from the control register  */
    /* If Fixed Function then make sure Mode is set correctly */
    /* else make sure reset is clear */
    #if(!BottleCounter_ControlRegRemoved || BottleCounter_UsingFixedFunction)
        BottleCounter_CONTROL |= BottleCounter_CTRL_ENABLE;                
    #endif /* (!BottleCounter_ControlRegRemoved || BottleCounter_UsingFixedFunction) */
    
}


/*******************************************************************************
* Function Name: BottleCounter_Start
********************************************************************************
* Summary:
*  Enables the counter for operation 
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Global variables:
*  BottleCounter_initVar: Is modified when this function is called for the  
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void BottleCounter_Start(void) 
{
    if(BottleCounter_initVar == 0u)
    {
        BottleCounter_Init();
        
        BottleCounter_initVar = 1u; /* Clear this bit for Initialization */        
    }
    
    /* Enable the Counter */
    BottleCounter_Enable();        
}


/*******************************************************************************
* Function Name: BottleCounter_Stop
********************************************************************************
* Summary:
* Halts the counter, but does not change any modes or disable interrupts.
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Side Effects: If the Enable mode is set to Hardware only then this function
*               has no effect on the operation of the counter.
*
*******************************************************************************/
void BottleCounter_Stop(void) 
{
    /* Disable Counter */
    #if(!BottleCounter_ControlRegRemoved || BottleCounter_UsingFixedFunction)
        BottleCounter_CONTROL &= ((uint8)(~BottleCounter_CTRL_ENABLE));        
    #endif /* (!BottleCounter_ControlRegRemoved || BottleCounter_UsingFixedFunction) */
    
    /* Globally disable the Fixed Function Block chosen */
    #if (BottleCounter_UsingFixedFunction)
        BottleCounter_GLOBAL_ENABLE &= ((uint8)(~BottleCounter_BLOCK_EN_MASK));
        BottleCounter_GLOBAL_STBY_ENABLE &= ((uint8)(~BottleCounter_BLOCK_STBY_EN_MASK));
    #endif /* (BottleCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: BottleCounter_SetInterruptMode
********************************************************************************
* Summary:
* Configures which interrupt sources are enabled to generate the final interrupt
*
* Parameters:  
*  InterruptsMask: This parameter is an or'd collection of the status bits
*                   which will be allowed to generate the counters interrupt.   
*
* Return: 
*  void
*
*******************************************************************************/
void BottleCounter_SetInterruptMode(uint8 interruptsMask) 
{
    BottleCounter_STATUS_MASK = interruptsMask;
}


/*******************************************************************************
* Function Name: BottleCounter_ReadStatusRegister
********************************************************************************
* Summary:
*   Reads the status register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the status register
*
* Side Effects:
*   Status register bits may be clear on read. 
*
*******************************************************************************/
uint8   BottleCounter_ReadStatusRegister(void) 
{
    return BottleCounter_STATUS;
}


#if(!BottleCounter_ControlRegRemoved)
/*******************************************************************************
* Function Name: BottleCounter_ReadControlRegister
********************************************************************************
* Summary:
*   Reads the control register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the control register
*
*******************************************************************************/
uint8   BottleCounter_ReadControlRegister(void) 
{
    return BottleCounter_CONTROL;
}


/*******************************************************************************
* Function Name: BottleCounter_WriteControlRegister
********************************************************************************
* Summary:
*   Sets the bit-field of the control register.  This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the control register
*
*******************************************************************************/
void    BottleCounter_WriteControlRegister(uint8 control) 
{
    BottleCounter_CONTROL = control;
}

#endif  /* (!BottleCounter_ControlRegRemoved) */


#if (!(BottleCounter_UsingFixedFunction && (CY_PSOC5A)))
/*******************************************************************************
* Function Name: BottleCounter_WriteCounter
********************************************************************************
* Summary:
*   This funtion is used to set the counter to a specific value
*
* Parameters:  
*  counter:  New counter value. 
*
* Return: 
*  void 
*
*******************************************************************************/
void BottleCounter_WriteCounter(uint16 counter) \
                                   
{
    #if(BottleCounter_UsingFixedFunction)
        /* assert if block is already enabled */
        CYASSERT (0u == (BottleCounter_GLOBAL_ENABLE & BottleCounter_BLOCK_EN_MASK));
        /* If block is disabled, enable it and then write the counter */
        BottleCounter_GLOBAL_ENABLE |= BottleCounter_BLOCK_EN_MASK;
        CY_SET_REG16(BottleCounter_COUNTER_LSB_PTR, (uint16)counter);
        BottleCounter_GLOBAL_ENABLE &= ((uint8)(~BottleCounter_BLOCK_EN_MASK));
    #else
        CY_SET_REG16(BottleCounter_COUNTER_LSB_PTR, counter);
    #endif /* (BottleCounter_UsingFixedFunction) */
}
#endif /* (!(BottleCounter_UsingFixedFunction && (CY_PSOC5A))) */


/*******************************************************************************
* Function Name: BottleCounter_ReadCounter
********************************************************************************
* Summary:
* Returns the current value of the counter.  It doesn't matter
* if the counter is enabled or running.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint16) The present value of the counter.
*
*******************************************************************************/
uint16 BottleCounter_ReadCounter(void) 
{
    /* Force capture by reading Accumulator */
    /* Must first do a software capture to be able to read the counter */
    /* It is up to the user code to make sure there isn't already captured data in the FIFO */
    #if(BottleCounter_UsingFixedFunction)
		(void)CY_GET_REG16(BottleCounter_COUNTER_LSB_PTR);
	#else
		(void)CY_GET_REG8(BottleCounter_COUNTER_LSB_PTR_8BIT);
	#endif/* (BottleCounter_UsingFixedFunction) */
    
    /* Read the data from the FIFO (or capture register for Fixed Function)*/
    #if(BottleCounter_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(BottleCounter_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(BottleCounter_STATICCOUNT_LSB_PTR));
    #endif /* (BottleCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: BottleCounter_ReadCapture
********************************************************************************
* Summary:
*   This function returns the last value captured.
*
* Parameters:  
*  void
*
* Return: 
*  (uint16) Present Capture value.
*
*******************************************************************************/
uint16 BottleCounter_ReadCapture(void) 
{
    #if(BottleCounter_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(BottleCounter_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(BottleCounter_STATICCOUNT_LSB_PTR));
    #endif /* (BottleCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: BottleCounter_WritePeriod
********************************************************************************
* Summary:
* Changes the period of the counter.  The new period 
* will be loaded the next time terminal count is detected.
*
* Parameters:  
*  period: (uint16) A value of 0 will result in
*         the counter remaining at zero.  
*
* Return: 
*  void
*
*******************************************************************************/
void BottleCounter_WritePeriod(uint16 period) 
{
    #if(BottleCounter_UsingFixedFunction)
        CY_SET_REG16(BottleCounter_PERIOD_LSB_PTR,(uint16)period);
    #else
        CY_SET_REG16(BottleCounter_PERIOD_LSB_PTR, period);
    #endif /* (BottleCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: BottleCounter_ReadPeriod
********************************************************************************
* Summary:
* Reads the current period value without affecting counter operation.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint16) Present period value.
*
*******************************************************************************/
uint16 BottleCounter_ReadPeriod(void) 
{
    #if(BottleCounter_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(BottleCounter_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG16(BottleCounter_PERIOD_LSB_PTR));
    #endif /* (BottleCounter_UsingFixedFunction) */
}


#if (!BottleCounter_UsingFixedFunction)
/*******************************************************************************
* Function Name: BottleCounter_WriteCompare
********************************************************************************
* Summary:
* Changes the compare value.  The compare output will 
* reflect the new value on the next UDB clock.  The compare output will be 
* driven high when the present counter value compares true based on the 
* configured compare mode setting. 
*
* Parameters:  
*  Compare:  New compare value. 
*
* Return: 
*  void
*
*******************************************************************************/
void BottleCounter_WriteCompare(uint16 compare) \
                                   
{
    #if(BottleCounter_UsingFixedFunction)
        CY_SET_REG16(BottleCounter_COMPARE_LSB_PTR, (uint16)compare);
    #else
        CY_SET_REG16(BottleCounter_COMPARE_LSB_PTR, compare);
    #endif /* (BottleCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: BottleCounter_ReadCompare
********************************************************************************
* Summary:
* Returns the compare value.
*
* Parameters:  
*  void:
*
* Return: 
*  (uint16) Present compare value.
*
*******************************************************************************/
uint16 BottleCounter_ReadCompare(void) 
{
    return (CY_GET_REG16(BottleCounter_COMPARE_LSB_PTR));
}


#if (BottleCounter_COMPARE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: BottleCounter_SetCompareMode
********************************************************************************
* Summary:
*  Sets the software controlled Compare Mode.
*
* Parameters:
*  compareMode:  Compare Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void BottleCounter_SetCompareMode(uint8 compareMode) 
{
    /* Clear the compare mode bits in the control register */
    BottleCounter_CONTROL &= ((uint8)(~BottleCounter_CTRL_CMPMODE_MASK));
    
    /* Write the new setting */
    BottleCounter_CONTROL |= compareMode;
}
#endif  /* (BottleCounter_COMPARE_MODE_SOFTWARE) */


#if (BottleCounter_CAPTURE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: BottleCounter_SetCaptureMode
********************************************************************************
* Summary:
*  Sets the software controlled Capture Mode.
*
* Parameters:
*  captureMode:  Capture Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void BottleCounter_SetCaptureMode(uint8 captureMode) 
{
    /* Clear the capture mode bits in the control register */
    BottleCounter_CONTROL &= ((uint8)(~BottleCounter_CTRL_CAPMODE_MASK));
    
    /* Write the new setting */
    BottleCounter_CONTROL |= ((uint8)((uint8)captureMode << BottleCounter_CTRL_CAPMODE0_SHIFT));
}
#endif  /* (BottleCounter_CAPTURE_MODE_SOFTWARE) */


/*******************************************************************************
* Function Name: BottleCounter_ClearFIFO
********************************************************************************
* Summary:
*   This function clears all capture data from the capture FIFO
*
* Parameters:  
*  void:
*
* Return: 
*  None
*
*******************************************************************************/
void BottleCounter_ClearFIFO(void) 
{

    while(0u != (BottleCounter_ReadStatusRegister() & BottleCounter_STATUS_FIFONEMP))
    {
        (void)BottleCounter_ReadCapture();
    }

}
#endif  /* (!BottleCounter_UsingFixedFunction) */


/* [] END OF FILE */

