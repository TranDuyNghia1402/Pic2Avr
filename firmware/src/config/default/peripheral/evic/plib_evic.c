/*******************************************************************************
  EVIC PLIB Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    plib_evic.c

  Summary:
    EVIC PLIB Source File

  Description:
    None

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

#include "device.h"
#include "plib_evic.h"
#include "interrupts.h"


volatile static EXT_INT_PIN_CALLBACK_OBJ extInt1CbObj;
volatile static EXT_INT_PIN_CALLBACK_OBJ extInt2CbObj;
volatile static EXT_INT_PIN_CALLBACK_OBJ extInt4CbObj;
// *****************************************************************************
// *****************************************************************************
// Section: IRQ Implementation
// *****************************************************************************
// *****************************************************************************

void EVIC_Initialize( void )
{
    INTCONSET = _INTCON_MVEC_MASK;

    /* Set up priority and subpriority of enabled interrupts */
    IPC2SET = 0x4U | 0x0U;  /* EXTERNAL_1:  Priority 1 / Subpriority 0 */
    IPC3SET = 0x400U | 0x0U;  /* EXTERNAL_2:  Priority 1 / Subpriority 0 */
    IPC3SET = 0x40000U | 0x0U;  /* TIMER_3:  Priority 1 / Subpriority 0 */
    IPC5SET = 0x4000000U | 0x0U;  /* EXTERNAL_4:  Priority 1 / Subpriority 0 */
    IPC14SET = 0x4U | 0x0U;  /* UART2_FAULT:  Priority 1 / Subpriority 0 */
    IPC14SET = 0x400U | 0x0U;  /* UART2_RX:  Priority 1 / Subpriority 0 */
    IPC14SET = 0x40000U | 0x0U;  /* UART2_TX:  Priority 1 / Subpriority 0 */

    /* Initialize External interrupt 1 callback object */
    extInt1CbObj.callback = NULL;
    /* Initialize External interrupt 2 callback object */
    extInt2CbObj.callback = NULL;
    /* Initialize External interrupt 4 callback object */
    extInt4CbObj.callback = NULL;

}

void EVIC_SourceEnable( INT_SOURCE source )
{
    volatile uint32_t *IECx = (volatile uint32_t *) (&IEC0 + ((0x10U * (source / 32U)) / 4U));
    volatile uint32_t *IECxSET = (volatile uint32_t *)(IECx + 2U);

    *IECxSET = 1UL << (source & 0x1fU);
}

void EVIC_SourceDisable( INT_SOURCE source )
{
    volatile uint32_t *IECx = (volatile uint32_t *) (&IEC0 + ((0x10U * (source / 32U)) / 4U));
    volatile uint32_t *IECxCLR = (volatile uint32_t *)(IECx + 1U);

    *IECxCLR = 1UL << (source & 0x1fU);
}

bool EVIC_SourceIsEnabled( INT_SOURCE source )
{
    volatile uint32_t *IECx = (volatile uint32_t *) (&IEC0 + ((0x10U * (source / 32U)) / 4U));

    return (((*IECx >> (source & 0x1fU)) & 0x01U) != 0U);
}

bool EVIC_SourceStatusGet( INT_SOURCE source )
{
    volatile uint32_t *IFSx = (volatile uint32_t *)(&IFS0 + ((0x10U * (source / 32U)) / 4U));

    return (((*IFSx >> (source & 0x1fU)) & 0x1U) != 0U);
}

void EVIC_SourceStatusSet( INT_SOURCE source )
{
    volatile uint32_t *IFSx = (volatile uint32_t *) (&IFS0 + ((0x10U * (source / 32U)) / 4U));
    volatile uint32_t *IFSxSET = (volatile uint32_t *)(IFSx + 2U);

    *IFSxSET = 1UL << (source & 0x1fU);
}

void EVIC_SourceStatusClear( INT_SOURCE source )
{
    volatile uint32_t *IFSx = (volatile uint32_t *) (&IFS0 + ((0x10U * (source / 32U)) / 4U));
    volatile uint32_t *IFSxCLR = (volatile uint32_t *)(IFSx + 1U);

    *IFSxCLR = 1UL << (source & 0x1fU);
}

void EVIC_INT_Enable( void )
{
   (void) __builtin_enable_interrupts();
}

bool EVIC_INT_Disable( void )
{
    uint32_t processorStatus;

    /* Save the processor status and then Disable the global interrupt */
    processorStatus = ( uint32_t )__builtin_disable_interrupts();

    /* return the interrupt status */
    return ((processorStatus & 0x01U) != 0U);
}

void EVIC_INT_Restore( bool state )
{
    if (state)
    {
        /* restore the state of CP0 Status register before the disable occurred */
       (void) __builtin_enable_interrupts();
    }
}

void EVIC_ExternalInterruptEnable( EXTERNAL_INT_PIN extIntPin )
{
    IEC0SET = (uint32_t)extIntPin;
}

void EVIC_ExternalInterruptDisable( EXTERNAL_INT_PIN extIntPin )
{
    IEC0CLR = (uint32_t)extIntPin;
}

bool EVIC_ExternalInterruptCallbackRegister(
    EXTERNAL_INT_PIN extIntPin,
    const EXTERNAL_INT_PIN_CALLBACK callback,
    uintptr_t context
)
{
    bool status = true;
    switch  (extIntPin)
        {
        case EXTERNAL_INT_1:
            extInt1CbObj.callback = callback;
            extInt1CbObj.context  = context;
            break;
        case EXTERNAL_INT_2:
            extInt2CbObj.callback = callback;
            extInt2CbObj.context  = context;
            break;
        case EXTERNAL_INT_4:
            extInt4CbObj.callback = callback;
            extInt4CbObj.context  = context;
            break;
        default:
            status = false;
            break;
        }

    return status;
}


// *****************************************************************************
/* Function:
    void EXTERNAL_1_InterruptHandler(void)

  Summary:
    Interrupt Handler for External Interrupt pin 1.

  Remarks:
    It is an internal function called from ISR, user should not call it directly.
*/
void __attribute__((used)) EXTERNAL_1_InterruptHandler(void)
{
    uintptr_t context_var;

    IFS0CLR = _IFS0_INT1IF_MASK;

    if(extInt1CbObj.callback != NULL)
    {
        context_var = extInt1CbObj.context;
        extInt1CbObj.callback (EXTERNAL_INT_1, context_var);
    }
}


// *****************************************************************************
/* Function:
    void EXTERNAL_2_InterruptHandler(void)

  Summary:
    Interrupt Handler for External Interrupt pin 2.

  Remarks:
    It is an internal function called from ISR, user should not call it directly.
*/
void __attribute__((used)) EXTERNAL_2_InterruptHandler(void)
{
    uintptr_t context_var;

    IFS0CLR = _IFS0_INT2IF_MASK;

    if(extInt2CbObj.callback != NULL)
    {
        context_var = extInt2CbObj.context;
        extInt2CbObj.callback (EXTERNAL_INT_2, context_var);
    }
}


// *****************************************************************************
/* Function:
    void EXTERNAL_4_InterruptHandler(void)

  Summary:
    Interrupt Handler for External Interrupt pin 4.

  Remarks:
    It is an internal function called from ISR, user should not call it directly.
*/
void __attribute__((used)) EXTERNAL_4_InterruptHandler(void)
{
    uintptr_t context_var;

    IFS0CLR = _IFS0_INT4IF_MASK;

    if(extInt4CbObj.callback != NULL)
    {
        context_var = extInt4CbObj.context;
        extInt4CbObj.callback (EXTERNAL_INT_4, context_var);
    }
}


/* End of file */