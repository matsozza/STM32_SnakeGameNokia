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
#include "service_uart.h"
#include "module_led.h"
#include "module_uart.h"
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
/* USER CODE END Variables */
osThreadId task100msHandle;
osThreadId task500msHandle;
osMessageQId queueUSART2Handle;
osPoolId  mpool;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void startTask100ms(void const * argument);
void startTask500ms(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
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

  //Variables initialization
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
  osMessageQDef(queueUSART2, 10, USART_message_t);
  queueUSART2Handle = osMessageCreate(osMessageQ(queueUSART2), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  osPoolDef(mpool, 10, USART_message_t);
  mpool = osPoolCreate(osPool(mpool));
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
  /* Infinite loop */
  for(;;)
  {
	//UART_printTestString();
	UART_printMsg("100ms Task!");
	LED2_toggle();
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
  /* Infinite loop */
  for(;;)
  {
	LED1_toggle();
	//UART_printLEDString();
	UART_printMsg("500ms Task!");
    osDelay(500);
  }
  /* USER CODE END startTask500ms */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
