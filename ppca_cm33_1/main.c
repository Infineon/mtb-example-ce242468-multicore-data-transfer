/*****************************************************************************
* File Name        : main.c
*
* Description      : This source file contains the main routine for multi-
*                    core data transfer application in the PPCA Core 1
*
* Related Document : See README.md
*
********************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/

#include "cy_pdl.h"
#include "cybsp.h"
/* IPC */
#include "ipc_config.h"
/*Shared memory*/
#include "sm_vars.h"

/*******************************************************************************
* Macros
********************************************************************************/
#define LOOP_DELAY_MS 200

/*******************************************************************************
* Global Variables
********************************************************************************/
#if (ENABLE_IPC_COMMUNICATION)
volatile int aquired_ipc = 0, released_ipc = 0, is_busy = 0, ipc_notify = 0;

cy_stc_sysint_t ipc_irq_config =
{
          .intrSrc = ppca_ipc_2_IRQn, 
          .intrPriority = 3U
};
#endif

/*******************************************************************************
* Function Prototypes
********************************************************************************/
#if (ENABLE_IPC_COMMUNICATION)
void ipc_irq_handler(void);
#endif

/*******************************************************************************
* Function Name: main
*********************************************************************************
* Summary:
* This is the main function for PPCA CPU 1. It performs the initialization of 
* the variables used in the code. Also, it reads the ADC read data, sent through 
* IPC or Shared memory and ADC value is programmed to DAC.
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{

#if (ENABLE_IPC_COMMUNICATION)
     uint32_t ipc_data;
    /* Initialized IPC */
    Cy_IPC_Drv_SetInterruptMask (PPCA_IPC_INTR_LINE_CORE1, REL_MASK, NOTIFY_MASK);

    Cy_SysInt_Init(&ipc_irq_config, &ipc_irq_handler);
    NVIC_ClearPendingIRQ((IRQn_Type)ipc_irq_config.intrSrc);
    NVIC_EnableIRQ((IRQn_Type)ipc_irq_config.intrSrc);
#endif

    /* Enable interrupts */
    __enable_irq();


     for(;;)
     {
#if (ENABLE_IPC_COMMUNICATION)
          /* Receiving data from IPC */
        if (ipc_notify)
        {
             ipc_notify = 0;
            ipc_data = Cy_IPC_Drv_ReadDataValue(PPCA_IPC_CHANNEL);
               /* Write the filter output to the DAC buffer */
               Cy_PPCA_DAC_Set_DACOut(DAC_FEEDBACK_HW, (uint16_t)ipc_data);
            // Cy_SysLib_Delay(500);
            Cy_IPC_Drv_ReleaseNotify(PPCA_IPC_CHANNEL, (PPCA_IPC_NOTIFY_CORE0));
        } 
#else
          if(sm_data_ready)
          {
               Cy_PPCA_DAC_Set_DACOut(DAC_FEEDBACK_HW, (uint16_t)sm_adc_data);
               sm_data_ready = 0;
          }
#endif
          //Cy_SysLib_Delay(LOOP_DELAY_MS);
     }
}

/*******************************************************************************
* Function Name: ipc_irq_handler
*********************************************************************************
* Summary:
* This is the interrupt service routine triggered after completing the IPC
* communication. It notifies the core about the new data or IPC release info.
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/

#if (ENABLE_IPC_COMMUNICATION)
void ipc_irq_handler(void)
{
     if(Cy_IPC_Drv_GetInterruptStatus(PPCA_IPC_INTR_LINE_CORE1) & (REL_BIT_POS))
          released_ipc = 1;

     if(Cy_IPC_Drv_GetInterruptStatus(PPCA_IPC_INTR_LINE_CORE1) & (NOTIFY_BIT_POS))
          ipc_notify = 1;

     Cy_IPC_Drv_ClearInterrupt(PPCA_IPC_INTR_LINE_CORE1, REL_MASK, NOTIFY_MASK);
}
#endif