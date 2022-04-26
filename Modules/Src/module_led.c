/**
 * LED Management module
 * module_led.c
 *
 *  Created on: Apr 21, 2022
 *      Author: Matheus Sozza
 */

#include "gpio.h"
#include "module_led.h"

void LED1_toggle()
{
	HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
}

void LED2_toggle()
{
	HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
}
