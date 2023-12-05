/*******************************************************************************
 System Tasks File

  File Name:
    tasks.c

  Summary:
    This file contains source code necessary to maintain system's polled tasks.

  Description:
    This file contains source code necessary to maintain system's polled tasks.
    It implements the "SYS_Tasks" function that calls the individual "Tasks"
    functions for all polled MPLAB Harmony modules in the system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    polled in the system.  These handles are passed into the individual module
    "Tasks" functions to identify the instance of the module to maintain.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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

#include "configuration.h"
#include "definitions.h"
#include "sys_tasks.h"

typedef enum {
    state_main_menu = 0,
    state_bin_counter,
    state_adc_read,
}state_t;
state_t current_state;

// *****************************************************************************
// *****************************************************************************
// Section: RTOS "Tasks" Routine
// *****************************************************************************
// *****************************************************************************

/* Handle for the APP_MAIN_MENU_HANDLER_Tasks. */
TaskHandle_t xAPP_MAIN_MENU_HANDLER_Tasks;

static void lAPP_MAIN_MENU_HANDLER_Tasks(  void *pvParameters  )
{   
    uint32_t cmd;
    
    const char main_menu[] = "\n=======================\r\n"
                                                 "|      Main Menu      |\r\n"
                                                 "=======================\r\n"
                                                 ".Binary Counter ----> 0\r\n"
                                                 ".Read ADC Value ----> 1\r\n";
    
    xQueueSend(q_data_print, (void*)main_menu, portMAX_DELAY);      
    
    while(true)
    {
        xTaskNotifyWait(0, 0, &cmd, portMAX_DELAY);      
        switch (cmd) {
            case 0:
                current_state = state_bin_counter;    
                xTaskNotify(xAPP_BINARY_COUNTER_HANDLER_Tasks, 0, eSetValueWithOverwrite);            
                break;
                
            case 1:
                current_state = state_adc_read;       
                xTaskNotify(xAPP_ADC_HANDLER_Tasks, 0, eNoAction);
                break;
                
            case 2:
                current_state = state_main_menu;        
                xQueueSend(q_data_print, (void*)main_menu, portMAX_DELAY);   
                break;
                
            default:
                continue;
        }
    }
}


/* Handle for the APP_PRINT_SCREEN_HANDLER_Tasks. */
TaskHandle_t xAPP_PRINT_SCREEN_HANDLER_Tasks;
static void lAPP_PRINT_SCREEN_HANDLER_Tasks(  void *pvParameters  )
{      
    char buffer[130] = "";
    while(true)
    {         
        xQueueReceive(q_data_print, (void*)buffer, portMAX_DELAY);
        printf("\e[1;1H\e[2J%s", buffer); //\033[0;0H
    }
}


/* Handle for the APP_BUTTON_HANDLER_Tasks. */
volatile uint32_t sendCommand;
void EXT_handler(EXTERNAL_INT_PIN pin, uintptr_t context) {
    BaseType_t xHigherPriorityTaskWoken = 0;
    if (pin == EXTERNAL_INT_4) {
        sendCommand = 0;
    }
    else if (pin == EXTERNAL_INT_1) {
        sendCommand = 1;
    }
    else if (pin == EXTERNAL_INT_2) {
        sendCommand = 2;
    }
    xTaskNotifyFromISR(xAPP_MAIN_MENU_HANDLER_Tasks, sendCommand, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}
TaskHandle_t xAPP_BUTTON_HANDLER_Tasks;
static void lAPP_BUTTON_HANDLER_Tasks(  void *pvParameters  )
{   
    EVIC_ExternalInterruptCallbackRegister(EXTERNAL_INT_4,  EXT_handler, (uintptr_t)NULL);
    EVIC_ExternalInterruptEnable(EXTERNAL_INT_4);
    EVIC_ExternalInterruptCallbackRegister(EXTERNAL_INT_1,  EXT_handler, (uintptr_t)NULL);
    EVIC_ExternalInterruptEnable(EXTERNAL_INT_1);
    EVIC_ExternalInterruptCallbackRegister(EXTERNAL_INT_2,  EXT_handler, (uintptr_t)NULL);
    EVIC_ExternalInterruptEnable(EXTERNAL_INT_2);
    while(true)
    {
          ;
    }
}


/* Handle for the APP_BINARY_COUNTER_HANDLER_Tasks. */
TaskHandle_t xAPP_BINARY_COUNTER_HANDLER_Tasks;

static void lAPP_BINARY_COUNTER_HANDLER_Tasks(  void *pvParameters  )
{   
    uint32_t counter = 0;
    const char counter_menu[] = "\n==========================\r\n"
                                                     "|      Counter Menu      |\r\n"
                                                     "==========================\r\n";
    while(true)
    {
        xTaskNotifyWait(0, 0, &counter, portMAX_DELAY);          
        while (current_state == state_bin_counter) {   
            xQueueSend(q_data_print, (void*)counter_menu, portMAX_DELAY);
            vTaskDelay(10);
            counter++;
            if (counter > 255) counter = 0;    
            printf("Counter data transfer to AVR: %d   \r", counter);
            xTaskNotify(xAPP_SEND_DATA_HANDLER_Tasks, counter, eSetValueWithOverwrite);      
            vTaskDelay(240);
        }  
    }
}


/* Handle for the APP_ADC_HANDLER_Tasks. */

TaskHandle_t xAPP_ADC_HANDLER_Tasks;

static void lAPP_ADC_HANDLER_Tasks(  void *pvParameters  )
{   
    uint16_t adc_value = 0;
    const char ADC_Menu[] = "\n==========================\r\n"
                                                "|      ADCRead Menu      |\r\n"
                                                "==========================\r\n";
    TMR3_Start();
    while(true)
    {
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY); 
        
        while (current_state == state_adc_read) {    
            xQueueSend(q_data_print, (void*)ADC_Menu, portMAX_DELAY);  
            vTaskDelay(10);            
            if (ADCHS_ChannelResultIsReady(ADCHS_CH0)) {
                adc_value = ADCHS_ChannelResultGet(ADCHS_CH0); 
                printf("ADC data transfer to AVR: %d   \r", adc_value);
                xTaskNotify(xAPP_SEND_DATA_HANDLER_Tasks, adc_value, eSetValueWithOverwrite);
                vTaskDelay(240);
            }              
        }        
    }
}



/* Handle for the APP_SEND_DATA_HANDLER_Tasks. */

TaskHandle_t xAPP_SEND_DATA_HANDLER_Tasks;

static void lAPP_SEND_DATA_HANDLER_Tasks(  void *pvParameters  )
{   
    uint32_t data_receive;
    char buffer[4];
    while(true)
    {
        xTaskNotifyWait(0, 0, &data_receive, portMAX_DELAY);
        sprintf(buffer,"%d", data_receive);
        UART2_Write(buffer, sizeof(buffer));
    }
}


// *****************************************************************************
// *****************************************************************************
// Section: System "Tasks" Routine
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void SYS_Tasks ( void )

  Remarks:
    See prototype in system/common/sys_module.h.
*/
void SYS_Tasks ( void )
{
    /* Maintain system services */
    
    
    /* Maintain Device Drivers */
    

    /* Maintain Middleware & Other Libraries */

    
    /* Maintain the application's state machine. */
        /* Create OS Thread for APP_MAIN_MENU_HANDLER_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_MAIN_MENU_HANDLER_Tasks,
                "APP_MAIN_MENU_HANDLER_Tasks",
                1024,
                NULL,
                1,
                &xAPP_MAIN_MENU_HANDLER_Tasks);

    /* Create OS Thread for APP_PRINT_SCREEN_HANDLER_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_PRINT_SCREEN_HANDLER_Tasks,
                "APP_PRINT_SCREEN_HANDLER_Tasks",
                1024,
                NULL,
                1,
                &xAPP_PRINT_SCREEN_HANDLER_Tasks);

    /* Create OS Thread for APP_BUTTON_HANDLER_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_BUTTON_HANDLER_Tasks,
                "APP_BUTTON_HANDLER_Tasks",
                1024,
                NULL,
                1,
                &xAPP_BUTTON_HANDLER_Tasks);

    /* Create OS Thread for APP_BINARY_COUNTER_HANDLER_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_BINARY_COUNTER_HANDLER_Tasks,
                "APP_BINARY_COUNTER_HANDLER_Tasks",
                1024,
                NULL,
                1,
                &xAPP_BINARY_COUNTER_HANDLER_Tasks);

    /* Create OS Thread for APP_ADC_HANDLER_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_ADC_HANDLER_Tasks,
                "APP_ADC_HANDLER_Tasks",
                1024,
                NULL,
                1,
                &xAPP_ADC_HANDLER_Tasks);

    /* Create OS Thread for APP_SEND_DATA_HANDLER_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lAPP_SEND_DATA_HANDLER_Tasks,
                "APP_SEND_DATA_HANDLER_Tasks",
                1024,
                NULL,
                1,
                &xAPP_SEND_DATA_HANDLER_Tasks);


    /* Start RTOS Scheduler. */
    
     /**********************************************************************
     * Create all Threads for APP Tasks before starting FreeRTOS Scheduler *
     ***********************************************************************/
    vTaskStartScheduler(); /* This function never returns. */

}

/*******************************************************************************
 End of File
 */

