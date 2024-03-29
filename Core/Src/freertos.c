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
#include "task_manager.h"
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

/* USER CODE END Variables */
osThreadId task100msHandle;
osThreadId task500msHandle;
osMessageQId queueUSART2Handle;
osMessageQId queueLCDHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void startTask100ms(void const * argument);
void startTask500ms(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void vApplicationIdleHook(void);

/* USER CODE BEGIN 2 */
__weak void vApplicationIdleHook(void)
{
	taskManager_idleTask_init();
	for (;;)
	{
		taskManager_idleTask_run();
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
void MX_FREERTOS_Init(void) {
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
  osMessageQDef(queueUSART2, 256, uint32_t); // Size of message addresses
  queueUSART2Handle = osMessageCreate(osMessageQ(queueUSART2), NULL);

  /* definition and creation of queueLCD */
  osMessageQDef(queueLCD, 512, uint32_t);
  queueLCDHandle = osMessageCreate(osMessageQ(queueLCD), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	osPoolDef(mPoolUSART2Handle, 256, USART_message_t); // Size of message contents
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
void startTask100ms(void const * argument)
{
  /* USER CODE BEGIN startTask100ms */
	taskManager_100ms_init();
	uint8_t taskCounts;
	for (;;)
	{
		taskManager_100ms_run();

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
void startTask500ms(void const * argument)
{
  /* USER CODE BEGIN startTask500ms */
	taskManager_500ms_init();
	for (;;)
	{
		taskManager_500ms_run();
		osThreadSuspend(task500msHandle);
	}
  /* USER CODE END startTask500ms */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */
