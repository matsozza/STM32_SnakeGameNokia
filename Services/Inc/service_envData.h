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


/* Defines -------------------------------------------------------------------*/
#define ENVDATA_DEBUG_LVL_USART 1

/* Typedef declare -----------------------------------------------------------*/


/* Function prototypes -------------------------------------------------------*/
void serviceEnvData_TIM_PeriodElapsedCallback();

#endif /* INC_CORE_SERVICES_ENVDATA_H_ */


