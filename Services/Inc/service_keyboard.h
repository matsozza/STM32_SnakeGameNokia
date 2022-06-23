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

/* Typedef declare -----------------------------------------------------------*/
typedef struct{
    char inputKey;
    uint8_t input_UID;
} serviceKeyboardInput_t;

/* Function prototypes -------------------------------------------------------*/
char serviceKeyboard_lookUpValue();
void serviceKeyboard_getGroundedCol();
void serviceKeyboard_getGroundedRow(uint16_t GPIO_Pin);
void serviceKeyboard_configPins_rowsAsInputs();
void serviceKeyboard_configPins_colsAsInputs();

#endif /* INC_CORE_SERVICES_KEYBOARD_H_ */


