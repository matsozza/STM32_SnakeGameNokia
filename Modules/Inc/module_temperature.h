/**
 * Temperature module - runs in 500ms task, prints to LCD upper part
 * module_snake.c
 * 
 * 
 *  Created on: June 17, 2022
 *      Author: Matheus Sozza
 */

#ifndef INC_CORE_MODULE_TEMPERATURE_H_
#define INC_CORE_MODULE_TEMPERATURE_H_

/* Includes ------------------------------------------------------------------*/
#include "service_tempSensor.h"

/* Defines ------------------------------------------------------------------*/


/* Typedef declare -----------------------------------------------------------*/
enum moduleTemperatureState_e
{
    NOT_INIT,
    INIT_STOPPED,
    INIT_RUNNING
};

/* Function prototypes -------------------------------------------------------*/
void moduleTemperature_runTask(uint8_t Activate);

#endif /* INC_CORE_MODULE_TEMPERATURE_H_ */
