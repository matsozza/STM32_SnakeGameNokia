/*
 * Task Manager module
 * task_manager.c
 *
 * Manages the methods for tasks
 *
 *  Created on: July 20, 2022
 *      Author: Matheus Sozza
 */

#ifndef INC_CORE_TASKS_MANAGER_H_
#define INC_CORE_TASKS_MANAGER_H_

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "service_tempSensor.h"
#include "service_uart.h"
#include "service_lcd.h"
#include "service_keyboard.h"
#include "module_snake.h"
#include "module_temperature.h"

/* Defines -------------------------------------------------------------------*/

/* Typedef declare -----------------------------------------------------------*/

/* Constants Declare ---------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
void taskManager_100ms_init();
void taskManager_100ms_run();
void taskManager_500ms_init();
void taskManager_500ms_run();
void taskManager_idleTask_init();
void taskManager_idleTask_run();

#endif /* INC_CORE_TASKS_MANAGER_H_ */

