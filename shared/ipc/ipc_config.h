/*****************************************************************************
 * \file    ipc_config.h
 * \brief   IPC configuration header file.
 * 
 * (c) 2025, Infineon Technologies AG, 
 * or an affiliate of Infineon Technologies AG. All rights reserved.
 * 
 * This software, associated documentation and materials ("Software") is owned 
 * by Infineon Technologies AG or one of its affiliates ("Infineon") and is 
 * protected by and subject to worldwide patent protection, worldwide copyright 
 * laws, and international treaty provisions. Therefore, you may use this 
 * Software only as provided in the license agreement accompanying the software 
 * package from which you obtained this Software. If no license agreement 
 * applies, then any use, reproduction, modification, translation, or 
 * compilation of this Software is prohibited without the express written 
 * permission of Infineon.
 * 
 * Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE IS
 * PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING, 
 * BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF THIRD-PARTY RIGHTS 
 * AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A SPECIFIC 
 * USE/PURPOSE OR MERCHANTABILITY. Infineon reserves the right to make changes 
 * to the Software without notice. You are responsible for properly designing, 
 * programming, and testing the functionality and safety of your intended 
 * application of the Software, as well as complying with any legal requirements
 * related to its use. Infineon does not guarantee that the Software will be 
 * free from intrusion, data theft or loss, or other breaches ("Security 
 * Breaches"), and Infineon shall have no liability arising out of any Security 
 * Breaches. Unless otherwise explicitly approved by Infineon, the Software may 
 * not be used in any application where a failure of the Product or any 
 * consequences of the use thereof can reasonably be expected to result in 
 * personal injury.
 *
*******************************************************************************/

#ifndef IPC_CONFIG_H
#define IPC_CONFIG_H

#include "cy_pdl.h" 
#include "cycfg.h"

/* 
 * IPC channel 0 is used to demostrate hardware locking as well as 
 * cross cores interrupt
 */


/* IPC Channel used */
#define PPCA_IPC_CHANNEL            (IPC_STRUCT_Type *)PPCA_IPC_STRUCT0

/* IPC Interrupt lines for each core */
#define PPCA_IPC_INTR_LINE_MAIN     (IPC_INTR_STRUCT_Type *)PPCA_IPC_INTR_STRUCT0
#define PPCA_IPC_INTR_LINE_CORE0    (IPC_INTR_STRUCT_Type *)PPCA_IPC_INTR_STRUCT1
#define PPCA_IPC_INTR_LINE_CORE1    (IPC_INTR_STRUCT_Type *)PPCA_IPC_INTR_STRUCT2

/* IPC Interrupt lines configs for channel 0 */
#define REL_MASK                    1       // IPC channel 0 
#define NOTIFY_MASK                 1       // IPC channel 0 
#define REL_BIT_POS                 1 << 0     // IPC channel 0 
#define NOTIFY_BIT_POS              1 << 16     // IPC channel 0 

/* IPC Interrupt lines notification */
#define PPCA_IPC_NOTIFY_MAIN        1 << 0     // IPC interrupt line 0
#define PPCA_IPC_NOTIFY_CORE0       1 << 1     // IPC interrupt line 1
#define PPCA_IPC_NOTIFY_CORE1       1 << 2     // IPC interrupt line 2


#endif /* IPC_CONFIG_H */
