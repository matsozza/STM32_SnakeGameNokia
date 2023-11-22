/*
 * Environmental data (temperature and humidity) handling services
 * service_envData.c
 *
 *
 *  Created on: November 21, 2023
 *      Author: Matheus Sozza
 */

/* Private includes ----------------------------------------------------------*/
#include "service_envData.h"
#include "service_uart.h"

/* External variables includes -----------------------------------------------*/

/* Internal variables includes -----------------------------------------------*/

/* Functions implementation --------------------------------------------------*/

void serviceEnvData_TIM_PeriodElapsedCallback()
{
	#if ENVDATA_DEBUG_LVL_USART >=1
	char debugMsg[25];
	sprintf(debugMsg, "ENVDATA_TIM3\n\r");
	USART2_addToQueue(debugMsg);
	#endif
}
