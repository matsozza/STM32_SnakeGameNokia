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
serviceKeyboard_configPins_getRow()
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // Configure the pins connected to the rows as interruptible inputs
  GPIO_InitStruct.Pin = EXTKEYBOARD_PIN3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(EXTKEYBOARD_PIN3_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = EXTKEYBOARD_PIN2_Pin|EXTKEYBOARD_PIN1_Pin|EXTKEYBOARD_PIN0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  //Configure the pins connected to the columns as low level outputs
  GPIO_InitStruct.Pin = EXTKEYBOARD_PIN7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(EXTKEYBOARD_PIN7_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = EXTKEYBOARD_PIN6_Pin|EXTKEYBOARD_PIN5_Pin|EXTKEYBOARD_PIN4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

serviceKeyboard_configPins_getCol()
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // Configure the pins connected to the rows as low level outputs
  GPIO_InitStruct.Pin = EXTKEYBOARD_PIN3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(EXTKEYBOARD_PIN3_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = EXTKEYBOARD_PIN2_Pin|EXTKEYBOARD_PIN1_Pin|EXTKEYBOARD_PIN0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  //Configure the pins connected to the columns as interruptible inputs
  GPIO_InitStruct.Pin = EXTKEYBOARD_PIN7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(EXTKEYBOARD_PIN7_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = EXTKEYBOARD_PIN6_Pin|EXTKEYBOARD_PIN5_Pin|EXTKEYBOARD_PIN4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}




#endif /* INC_CORE_SERVICES_KEYBOARD_H_ */


