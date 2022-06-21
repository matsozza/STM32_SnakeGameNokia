/*
 * External Matricial Keyboard Service
 * service_keyboard.c
 *
 *  Created on: June 20, 2022
 *      Author: Matheus Sozza
 */

/* Private includes ----------------------------------------------------------*/

/* External variables includes -----------------------------------------------*/

/* Internal variables includes -----------------------------------------------*/

/* Functions implementation --------------------------------------------------*/
void serviceKeyboard_configPins_getRow()
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

void serviceKeyboard_configPins_getCol()
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
