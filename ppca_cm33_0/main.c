/*****************************************************************************
* File Name        : main.c
*
* Description      : This source file contains the main routine for multi-
*                    core data transfer application in the PPCA Core 0
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
#include "cycfg_peripherals.h"
#include "cy_ipc_drv.h"
/* IPC */
#include "ipc_config.h"
/*Shared memory*/
#include "sm_vars.h"

/******************************************************************************
* Macros
*******************************************************************************/
/* Shared memory addresses in M4 shared memory space (0x20040000-0x20043FFF) */
/* All cores can access M4 shared memory for inter-core communication */
#define PPCA_CPU0_M4_VAR_ADDRESS 0x20040400  /* Used by this core (CPU0) */

#define LOOP_DELAY_MS 1000

/*******************************************************************************
* Global Variables
*******************************************************************************/
volatile int32_t *ADC_read_val   = (int32_t *)PPCA_CPU0_M4_VAR_ADDRESS;
volatile int32_t *new_data       = (int32_t *)PPCA_CPU0_M4_VAR_ADDRESS + 1;
volatile uint32_t pot_data       = 0;

/* ADC POT ISR */
cy_stc_sysint_t adc_pot_intr_config =
{
    .intrSrc = ppca_0_epu_0_IRQ_EPU_0,
    .intrPriority = 1U,
};

#if (ENABLE_IPC_COMMUNICATION)

volatile int aquired_ipc = 0, released_ipc = 0, is_busy = 0, ipc_notify=0;

/* IPC ISR*/
cy_stc_sysint_t ipc_irq_config =
{
    .intrSrc = ppca_ipc_1_IRQn,
    .intrPriority = 3U
};
#endif


/*******************************************************************************
* Function Prototypes
*******************************************************************************/
void adc_isr();

#if (ENABLE_IPC_COMMUNICATION)
void ipc_irq_handler(void);
#endif

/*******************************************************************************
* Function Definitions
*******************************************************************************/

/*******************************************************************************
* Function Name: main
*********************************************************************************
* Summary:
* This is the main function for PPCA CPU 0. It performs the initialization of 
* the variables used in the code. Also, it reads the ADC read data, in periodic 
* intervals and copies the data to the shared memory for consumed by other CPU.
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

    /* Configure the EPU interrupt to the PPCA CPU0 on conversion completion of the ADC */
    Cy_PPCA_EPU_InterruptSourceSelect(ppca_0_epu_0_EPU_IRQ0_HW, false, epuIrqSrc0);
    Cy_PPCA_EPU_SetInterruptMask(ppca_0_epu_0_EPU_IRQ0_HW);

    /* ADC filter interrupt initialization */
    Cy_SysInt_Init(&adc_pot_intr_config, &adc_isr);
    NVIC_EnableIRQ(adc_pot_intr_config.intrSrc);

#if (ENABLE_IPC_COMMUNICATION)
    /* Initialized IPC */
    Cy_IPC_Drv_SetInterruptMask (PPCA_IPC_INTR_LINE_CORE0, REL_MASK, NOTIFY_MASK);

    Cy_SysInt_Init(&ipc_irq_config, &ipc_irq_handler);
    NVIC_ClearPendingIRQ((IRQn_Type)ipc_irq_config.intrSrc);
    NVIC_EnableIRQ((IRQn_Type)ipc_irq_config.intrSrc);
#endif

    /* Enable interrupts */
    __enable_irq();

    /* Software start to TCPWM unit */
    Cy_TCPWM_TriggerStart_Single(ADC_TRIG_PWM_HW, ADC_TRIG_PWM_NUM);

    for(;;)
    {
          if(*new_data == 1)
          {
             *ADC_read_val = pot_data;
#if (ENABLE_IPC_COMMUNICATION)
               /* Sending data from IPC */
             Cy_IPC_Drv_LockAcquire(PPCA_IPC_CHANNEL);
             Cy_IPC_Drv_WriteDataValue(PPCA_IPC_CHANNEL, pot_data);
             Cy_IPC_Drv_AcquireNotify(PPCA_IPC_CHANNEL, (PPCA_IPC_NOTIFY_CORE1));
             while(released_ipc != 1){};
              if (released_ipc)
             {
                  released_ipc = 0;
             }
#else
               sm_adc_data = pot_data;
               sm_data_ready = 1;
#endif
          }

    }
}

/*******************************************************************************
* Function Name: adc_isr
*********************************************************************************
* Summary:
* This is the interrupt service routine triggered after completing the ADC
* conversion. It reads ADC data and writes it into the shared memory.
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
void adc_isr()
{
    /* Clear interrupt. */
    Cy_PPCA_EPU_ClearInterrupt(ppca_0_epu_0_EPU_IRQ0_HW);

    /* read the potentiometer data */
    pot_data = Cy_PPCA_ADC_Read_ADC_Data(ADC_POT_HW, 4);

     *new_data = 1;
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
     if(Cy_IPC_Drv_GetInterruptStatus(PPCA_IPC_INTR_LINE_CORE0) & (REL_BIT_POS))
          released_ipc = 1;

     if(Cy_IPC_Drv_GetInterruptStatus(PPCA_IPC_INTR_LINE_CORE0) & (NOTIFY_BIT_POS))
          ipc_notify = 1;

     Cy_IPC_Drv_ClearInterrupt(PPCA_IPC_INTR_LINE_CORE0, REL_MASK, NOTIFY_MASK);

}
#endif
