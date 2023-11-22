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
    ENVDATA_QUERY_START,
    ENVDATA_QUERY_WAIT,
    ENVDATA_QUERY_CONFIRMED,
    ENVDATA_QUERY_ACKNOWLEDGED1,
    ENVDATA_QUERY_ACKNOWLEDGED2,
    ENVDATA_TX_DATA1,
    ENVDATA_TX_DATA2,
    ENVDATA_TX_DATA3,
    ENVDATA_TX_DATA4,
    ENVDATA_TX_DATA5,
    ENVDATA_TX_END    
};

enum envDataITSrc_e
{
    ENVDATA_SRC_LOWRES,
    ENVDATA_SRC_HIGHRES,
    ENVDATA_SRC_EXTI
};

/* Function prototypes -------------------------------------------------------*/
void serviceEnvData_TIM_PeriodElapsedCallback_LowRes();
void serviceEnvData_TIM_PeriodElapsedCallback_HighRes();
void serviceEnvData_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#endif /* INC_CORE_SERVICES_ENVDATA_H_ */


