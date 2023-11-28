/*
 * Task Manager module
 * task_manager.c
 *
 * Manages the methods for tasks
 *
 *  Created on: July 20, 2022
 *      Author: Matheus Sozza
 */

/* Private includes ----------------------------------------------------------*/
#include "task_manager.h"
#include "freertos.h"
#include "tim.h"

/* External variables includes -----------------------------------------------*/
// FreeRTOS featured variables
extern osPoolId mPoolUSART2Handle;
extern osPoolId mPoolLCDHandle;
extern osThreadId task100msHandle;
extern osThreadId task500msHandle;
extern osMessageQId queueUSART2Handle;
extern osMessageQId queueLCDHandle;

/* Internal variables includes -----------------------------------------------*/
// 500ms Task
int task500ms_LCDWrite;

// 100ms Task
char task100ms_keyPressed;

// Idle Task Variables
volatile uint32_t taskIdle_queueUSART2_msgWaiting;
volatile uint32_t taskIdle_queueLCD_msgWaiting;

// Common IOs / interfaces for RTOS
extern LCD_displayBuffer_t *LCD_displayBuffer01;

/* Functions implementation --------------------------------------------------*/
void taskManager_100ms_init()
{
	task100ms_keyPressed = 'x'; 
	flashMem_initService();
}

void taskManager_100ms_run()
{
	//USART2_addToQueue("-> 100ms Task!\n\r");
	HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin); // Blink LED2 at task time

	// ** Check and consume keyboard pressed key
	task100ms_keyPressed = serviceKeyboard_consumeKey();

	// ** Run RTOS modules
	moduleMainMenu_runTask(LCD_displayBuffer01,task100ms_keyPressed,1);


	// ** Send LCD Buffer to queue for further updating
	LCD_Buffer_sendToQueue(LCD_displayBuffer01);
}

void taskManager_500ms_init()
{
	task500ms_LCDWrite=0;
	serviceEnvData_serviceInit(); // Start DHT22 envData service
}

void taskManager_500ms_run()
{
	// Task activities
	HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
	//USART2_addToQueue("----> 500ms Task!\n\r"); // Blink LED1 at task time

	moduleTemperature_runTask(LCD_displayBuffer01,1);
	LCD_Buffer_sendToQueue(LCD_displayBuffer01);
}

void taskManager_idleTask_init()
{
	taskIdle_queueUSART2_msgWaiting = 0;
	taskIdle_queueLCD_msgWaiting = 0;
}

void taskManager_idleTask_run()
{
		// ** Check and consume USART2 queue
		taskIdle_queueUSART2_msgWaiting = osMessageWaiting(queueUSART2Handle); // Check USART2 queue
		if (taskIdle_queueUSART2_msgWaiting > 0)	USART2_consumeFromQueue(); // Consume USART2 Queue items

		// ** Check and consume LCD queue
		taskIdle_queueLCD_msgWaiting = osMessageWaiting(queueLCDHandle); // Check LCD queue
		if (taskIdle_queueLCD_msgWaiting > 0) LCD_Queue_consumeBytes(); // Consume LCD Queue items

}
