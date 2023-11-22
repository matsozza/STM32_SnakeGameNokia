/*
 * Environmental data (temperature and humidity) handling services
 * service_envData.h
 *
 *
 *  Created on: November 21, 2023
 *      Author: Matheus Sozza
 */

#ifndef INC_CORE_SERVICES_ENVDATA_H_
#define INC_CORE_SERVICES_ENVDATA_H_

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"
#include "gpio.h"

/* Defines -------------------------------------------------------------------*/
#define ENVDATA_DEBUG_LVL_USART 1
#define ENVDATA_COUNTER_HIGHRES htim4
#define ENVDATA_CLK_SRC DWT->CYCCNT
#define ENVDATA_CLK_FRQ 168 // freq. in MHz

/* Typedef declare -----------------------------------------------------------*/
enum envDataState_e
{
    ENVDATA_IDLE,
    ENVDATA_QUERYING,
    ENVDATA_QUERY_OK,
    ENVDATA_QUERY_ACK1,
    ENVDATA_STREAMING,
    ENVDATA_CHECKING,
    ENVDATA_DONE,
    ENVDATA_ERR
};

/* Function prototypes -------------------------------------------------------*/
void serviceEnvData_TIM_PeriodElapsedCallback_LowRes();
void serviceEnvData_TIM_PeriodElapsedCallback_HighRes();
void serviceEnvData_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#endif /* INC_CORE_SERVICES_ENVDATA_H_ */


