/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include <string.h>
#include "service_tempSensor.h"
#include "service_uart.h"
#include "service_lcd.h"
#include "module_snake.h"
#include "module_temperature.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

osPoolId mPoolUSART2Handle;
osPoolId mPoolLCDHandle;

// Common IOs / interfaces for RTOS
extern LCD_displayBuffer_t *LCD_displayBuffer01;

/* USER CODE END Variables */
osThreadId task100msHandle;
osThreadId task500msHandle;
osMessageQId queueUSART2Handle;
osMessageQId queueLCDHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void startTask100ms(void const *argument);
void startTask500ms(void const *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);

/* Hook prototypes */
void vApplicationIdleHook(void);

/* USER CODE BEGIN 2 */
__weak void vApplicationIdleHook(void)
{
	volatile uint32_t queueUSART2_msgWaiting = 0;
	volatile uint32_t queueLCD_msgWaiting = 0;
	for (;;)
	{
		// ** Check and consume USART2 queue
		queueUSART2_msgWaiting = osMessageWaiting(queueUSART2Handle); // Check items on USART2 queue
		if (queueUSART2_msgWaiting > 0)
			USART2_consumeFromQueue(); // Consume USART2 Queue items

		// ** Check and consume LCD queue
		queueLCD_msgWaiting = osMessageWaiting(queueLCDHandle); // Check items on LCD queue
		if (queueLCD_msgWaiting > 0)
			LCD_Queue_consumeBytes(); // Consume LCD Queue items
	}
}
/* USER CODE END 2 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
	*ppxIdleTaskStackBuffer = &xIdleStack[0];
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
	/* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
	/* USER CODE BEGIN Init */
	/* USER CODE END Init */

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* Create the queue(s) */
	/* definition and creation of queueUSART2 */
	osMessageQDef(queueUSART2, 5, uint32_t); // USART_message_t);
	queueUSART2Handle = osMessageCreate(osMessageQ(queueUSART2), NULL);

	/* definition and creation of queueLCD */
	osMessageQDef(queueLCD, 512, uint32_t); // LCD_dataPackage_t);
	queueLCDHandle = osMessageCreate(osMessageQ(queueLCD), NULL);

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	osPoolDef(mPoolUSART2Handle, 15, USART_message_t);
	mPoolUSART2Handle = osPoolCreate(osPool(mPoolUSART2Handle));

	osPoolDef(mPoolLCDHandle, 512, LCD_dataPackage_t);
	mPoolLCDHandle = osPoolCreate(osPool(mPoolLCDHandle));
	/* USER CODE END RTOS_QUEUES */

	/* Create the thread(s) */
	/* definition and creation of task100ms */
	osThreadDef(task100ms, startTask100ms, osPriorityNormal, 0, 128);
	task100msHandle = osThreadCreate(osThread(task100ms), NULL);

	/* definition and creation of task500ms */
	osThreadDef(task500ms, startTask500ms, osPriorityAboveNormal, 0, 128);
	task500msHandle = osThreadCreate(osThread(task500ms), NULL);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* USER CODE END RTOS_THREADS */
}

/* USER CODE BEGIN Header_startTask100ms */
/**
 * @brief  Function implementing the task100ms thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_startTask100ms */
void startTask100ms(void const *argument)
{
	/* USER CODE BEGIN startTask100ms */
	/* Infinite loop */
	volatile uint8_t taskCounts = 0;
	for (;;)
	{
		// Task activities
		USART2_addToQueue("100ms Task!\n\r");
		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin); // Blink LED2 at task time

		moduleSnake_runTask(LCD_displayBuffer01,1);
		moduleSnake_autoPlay();

		LCD_Buffer_sendToQueue(LCD_displayBuffer01);

		// Caller for task500ms
		taskCounts = (uint8_t)((taskCounts + 1) % 5);
		if (taskCounts == 4)
			osThreadResume(task500msHandle);
		osDelay(100);
	}
	/* USER CODE END startTask100ms */
}

/* USER CODE BEGIN Header_startTask500ms */
/**
 * @brief Function implementing the task500ms thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_startTask500ms */
void startTask500ms(void const *argument)
{
	/* USER CODE BEGIN startTask500ms */

	int a=0; 

	/* Infinite loop */
	for (;;)
	{
		// Task activities
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		USART2_addToQueue("500ms Task!\n\r"); // Blink LED1 at task time

		if(a)
		{
			LCD_Buffer_writeASCIIChar(LCD_displayBuffer01, '@',0,0);
			LCD_Buffer_writeASCIIChar(LCD_displayBuffer01, 'm',0,6);
			LCD_Buffer_writeASCIIChar(LCD_displayBuffer01, 'a',0,12);
			LCD_Buffer_writeASCIIChar(LCD_displayBuffer01, 't',0,18);
			LCD_Buffer_writeASCIIChar(LCD_displayBuffer01, 's',0,24);
			LCD_Buffer_writeASCIIChar(LCD_displayBuffer01, 'o',0,30);
			LCD_Buffer_writeASCIIChar(LCD_displayBuffer01, 'z',0,36);
			LCD_Buffer_writeASCIIChar(LCD_displayBuffer01, 'z',0,42);
				LCD_Buffer_writeASCIIChar(LCD_displayBuffer01, 'a',0,48);
			a=0;
		}
		else
		{	
			LCD_Buffer_writeASCIIChar(LCD_displayBuffer01, 'S',0,0);
			LCD_Buffer_writeASCIIChar(LCD_displayBuffer01, 'n',0,6);
			LCD_Buffer_writeASCIIChar(LCD_displayBuffer01, 'a',0,12);
			LCD_Buffer_writeASCIIChar(LCD_displayBuffer01, 'k',0,18);
			LCD_Buffer_writeASCIIChar(LCD_displayBuffer01, 'e',0,24);
			LCD_Buffer_writeASCIIChar(LCD_displayBuffer01, 'G',0,30);
			LCD_Buffer_writeASCIIChar(LCD_displayBuffer01, 'a',0,36);
			LCD_Buffer_writeASCIIChar(LCD_displayBuffer01, 'm',0,42);
			LCD_Buffer_writeASCIIChar(LCD_displayBuffer01, 'e',0,48);
			a=1;
		}

		moduleTemperature_runTask(LCD_displayBuffer01,1);

		LCD_Buffer_sendToQueue(LCD_displayBuffer01);
		osThreadSuspend(task500msHandle);
	}
	/* USER CODE END startTask500ms */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */
