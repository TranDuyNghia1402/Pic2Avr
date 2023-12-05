/*******************************************************************************
  System Interrupts File

  Company:
    Microchip Technology Inc.

  File Name:
    interrupt_a.S

  Summary:
    Raw ISR definitions.

  Description:
    This file maps all the interrupt vectors to their corresponding
    implementations. If a particular module interrupt is used, then its ISR
    definition can be found in corresponding PLIB source file. If a module
    interrupt is not used, then its ISR implementation is mapped to dummy
    handler.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <xc.h>
#include "ISR_Support.h"

    .extern  EXTERNAL_1_Handler

    .section   .vector_8,code, keep
    .equ     __vector_dispatch_8, IntVectorEXTERNAL_1_Handler
    .global  __vector_dispatch_8
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorEXTERNAL_1_Handler

IntVectorEXTERNAL_1_Handler:
    portSAVE_CONTEXT
    la    s6,  EXTERNAL_1_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorEXTERNAL_1_Handler
    .extern  EXTERNAL_2_Handler

    .section   .vector_13,code, keep
    .equ     __vector_dispatch_13, IntVectorEXTERNAL_2_Handler
    .global  __vector_dispatch_13
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorEXTERNAL_2_Handler

IntVectorEXTERNAL_2_Handler:
    portSAVE_CONTEXT
    la    s6,  EXTERNAL_2_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorEXTERNAL_2_Handler
    .extern  TIMER_3_Handler

    .section   .vector_14,code, keep
    .equ     __vector_dispatch_14, IntVectorTIMER_3_Handler
    .global  __vector_dispatch_14
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorTIMER_3_Handler

IntVectorTIMER_3_Handler:
    portSAVE_CONTEXT
    la    s6,  TIMER_3_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorTIMER_3_Handler
    .extern  EXTERNAL_4_Handler

    .section   .vector_23,code, keep
    .equ     __vector_dispatch_23, IntVectorEXTERNAL_4_Handler
    .global  __vector_dispatch_23
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorEXTERNAL_4_Handler

IntVectorEXTERNAL_4_Handler:
    portSAVE_CONTEXT
    la    s6,  EXTERNAL_4_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorEXTERNAL_4_Handler
    .extern  UART2_FAULT_Handler

    .section   .vector_56,code, keep
    .equ     __vector_dispatch_56, IntVectorUART2_FAULT_Handler
    .global  __vector_dispatch_56
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorUART2_FAULT_Handler

IntVectorUART2_FAULT_Handler:
    portSAVE_CONTEXT
    la    s6,  UART2_FAULT_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorUART2_FAULT_Handler
    .extern  UART2_RX_Handler

    .section   .vector_57,code, keep
    .equ     __vector_dispatch_57, IntVectorUART2_RX_Handler
    .global  __vector_dispatch_57
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorUART2_RX_Handler

IntVectorUART2_RX_Handler:
    portSAVE_CONTEXT
    la    s6,  UART2_RX_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorUART2_RX_Handler
    .extern  UART2_TX_Handler

    .section   .vector_58,code, keep
    .equ     __vector_dispatch_58, IntVectorUART2_TX_Handler
    .global  __vector_dispatch_58
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorUART2_TX_Handler

IntVectorUART2_TX_Handler:
    portSAVE_CONTEXT
    la    s6,  UART2_TX_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorUART2_TX_Handler

