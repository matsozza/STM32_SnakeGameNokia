/*
 * External Matricial Keyboard Service
 * service_keyboard.h
 *
 * Pins 0 to 3 are rows, initialized as Inputs
 * Pins 4 to 7 are columns, initialized as Outputs
 *
 * Pins 0 to 3 trigger interruptions when grounded
 *
 *  Created on: June 20, 2022
 *      Author: Matheus Sozza
 */



#ifndef INC_CORE_SERVICES_KEYBOARD_H_
#define INC_CORE_SERVICES_KEYBOARD_H_

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"
#include "gpio.h"

/* Defines -------------------------------------------------------------------*/

// EXTKEYBOARD_PIN0 PB8
// EXTKEYBOARD_PIN1 PB6
// EXTKEYBOARD_PIN2 PB4
// EXTKEYBOARD_PIN3 PG15
// EXTKEYBOARD_PIN4 PG13
// EXTKEYBOARD_PIN5 PG11
// EXTKEYBOARD_PIN6 PG9
// EXTKEYBOARD_PIN7 PD6


/* Typedef declare -----------------------------------------------------------*/


/* Function prototypes -------------------------------------------------------*/
void serviceKeyboard_configPins_getRow();
void serviceKeyboard_configPins_getCol();

#endif /* INC_CORE_SERVICES_KEYBOARD_H_ */


