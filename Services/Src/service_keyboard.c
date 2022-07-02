/*
 * External Matricial Keyboard Service
 * service_keyboard.c
 *
 *  Created on: June 20, 2022
 *      Author: Matheus Sozza
 */

/* Private includes ----------------------------------------------------------*/
#include "service_keyboard.h"
#include "service_cycleDelay.h"
#include "gpio.h"

/* External variables includes -----------------------------------------------*/

/* Internal variables includes -----------------------------------------------*/
uint8_t serviceKeyboard_groundedRow;
uint8_t serviceKeyboard_groundedCol;
const char serviceKeyboard_lookupTable[4][4] = {
                                                {'1', '2', '3', 'A'},
                                                {'4', '5', '6', 'B'},
                                                {'7', '8', '9', 'C'},
                                                {'*', '0', '#', 'D'}
                                                };
serviceKeyboardInput_t  serviceKeyboardInput;

/* Internal functions includes -----------------------------------------------*/
static void serviceKeyboard_enableKeyInterrupts(uint8_t Enable);
static char serviceKeyboard_lookUpValue();
static void serviceKeyboard_getGroundedCol();
static void serviceKeyboard_getGroundedRow(uint16_t GPIO_Pin);
static void serviceKeyboard_configPins_rowsAsInputs();
static void serviceKeyboard_configPins_colsAsInputs();

/* Functions implementation --------------------------------------------------*/

// Callback function for GPIO implemented locally in service file
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	//If last input still not consumed by task, ignore other inputs
	if(!serviceKeyboardInput.inputConsumed)
	{
		return;
	}	
	
	// Disable Keyboard interrupts
	serviceKeyboard_enableKeyInterrupts(0);

	// Get the row corresponding to the pressed key
	serviceKeyboard_getGroundedRow(GPIO_Pin);
	
	// Reconfigure the GPIOs to get the column of the pressed key
	serviceKeyboard_configPins_colsAsInputs();
	serviceKeyboard_getGroundedCol();

	// Move GPIOs back to orig. config, interruptible rows as inputs
	//serviceKeyboard_configPins_rowsAsInputs();// TODO debounce this logic
	
	// Get pressed key from lookUp table
	serviceKeyboardInput.inputKey = serviceKeyboard_lookUpValue();
	serviceKeyboardInput.inputConsumed = 0;
	serviceKeyboardInput.GPIO_Pin = GPIO_Pin;
	
	// Reenable Keyboard interrupts
	//serviceKeyboard_enableKeyInterrupts(1);
	
}

char serviceKeyboard_consumeKey()
{
	serviceKeyboardInput.inputConsumed = 1;
	serviceKeyboard_enableKeyInterrupts(1);
	serviceKeyboard_configPins_rowsAsInputs();
	return serviceKeyboardInput.inputKey;
}

static void serviceKeyboard_enableKeyInterrupts(uint8_t Enable)
{
	if(Enable)
	{
		HAL_NVIC_EnableIRQ(EXTI4_IRQn);
		HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	}
	else
	{
		HAL_NVIC_DisableIRQ(EXTI4_IRQn);
		HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
		HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
	}
}

static char serviceKeyboard_lookUpValue()
{
	// Get column Idx
	uint8_t colIdx;
	switch(serviceKeyboard_groundedCol)
	{
		case 0b0001:
			colIdx = 0;
			break;
		case 0b0010:
			colIdx = 1;
			break;
		case 0b100:
			colIdx = 2;
			break;
		case 0b1000:
			colIdx = 3;
			break;
		default:
			colIdx=0;
	}

	// Get row Idx
	uint8_t rowIdx;
	switch(serviceKeyboard_groundedRow)
	{
		case 0b0001:
			rowIdx = 0;
			break;
		case 0b0010:
			rowIdx = 1;
			break;
		case 0b100:
			rowIdx = 2;
			break;
		case 0b1000:
			rowIdx = 3;
			break;
		default:
			rowIdx=0;
	}

	//Lookup value
	return serviceKeyboard_lookupTable[rowIdx][colIdx];

}

static void serviceKeyboard_getGroundedRow(uint16_t GPIO_Pin)
{
    switch(GPIO_Pin)
    {
		case EXTKEYBOARD_PIN0_Pin:
			serviceKeyboard_groundedRow = 0b0001;
			break;
		case EXTKEYBOARD_PIN1_Pin:
			serviceKeyboard_groundedRow = 0b0010;
			break;
		case EXTKEYBOARD_PIN2_Pin:
			serviceKeyboard_groundedRow = 0b0100;
			break;
		case EXTKEYBOARD_PIN3_Pin:
			serviceKeyboard_groundedRow = 0b1000;
			break;
		default:
			serviceKeyboard_groundedRow = 0b0000;
    }
}

static void serviceKeyboard_getGroundedCol()
{
	serviceKeyboard_groundedCol = 0b1111;
	
	//Read column 0 / Pin4
	uint8_t colGrounded = HAL_GPIO_ReadPin(EXTKEYBOARD_PIN4_GPIO_Port, EXTKEYBOARD_PIN4_Pin);
	if(colGrounded ==0)
	{
		serviceKeyboard_groundedCol = 0b0001;
		return; //Early return, save time
	}

	//Read column 0 / Pin4
	colGrounded = HAL_GPIO_ReadPin(EXTKEYBOARD_PIN5_GPIO_Port, EXTKEYBOARD_PIN5_Pin);
	if(colGrounded ==0)
	{
		serviceKeyboard_groundedCol = 0b0010;
		return; //Early return, save time
	}

	//Read column 0 / Pin4
	colGrounded = HAL_GPIO_ReadPin(EXTKEYBOARD_PIN6_GPIO_Port, EXTKEYBOARD_PIN6_Pin);
	if(colGrounded ==0)
	{
		serviceKeyboard_groundedCol = 0b0100;
		return; //Early return, save time
	}

	//Read column 0 / Pin4
	colGrounded = HAL_GPIO_ReadPin(EXTKEYBOARD_PIN7_GPIO_Port, EXTKEYBOARD_PIN7_Pin);
	if(colGrounded ==0)
	{
		serviceKeyboard_groundedCol = 0b1000;
		return; //Last return, worst case
	}
}

static void serviceKeyboard_configPins_rowsAsInputs()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	// Configure the pins connected to the rows as interruptible inputs
	GPIO_InitStruct.Pin = EXTKEYBOARD_PIN3_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(EXTKEYBOARD_PIN3_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = EXTKEYBOARD_PIN2_Pin|EXTKEYBOARD_PIN1_Pin|EXTKEYBOARD_PIN0_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	//Configure the pins connected to the columns as low level outputs
	GPIO_InitStruct.Pin = EXTKEYBOARD_PIN7_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(EXTKEYBOARD_PIN7_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = EXTKEYBOARD_PIN6_Pin|EXTKEYBOARD_PIN5_Pin|EXTKEYBOARD_PIN4_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	// Set all cols to RESET - Ground
	HAL_GPIO_WritePin(EXTKEYBOARD_PIN4_GPIO_Port, EXTKEYBOARD_PIN4_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EXTKEYBOARD_PIN5_GPIO_Port, EXTKEYBOARD_PIN5_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EXTKEYBOARD_PIN6_GPIO_Port, EXTKEYBOARD_PIN6_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EXTKEYBOARD_PIN7_GPIO_Port, EXTKEYBOARD_PIN7_Pin, GPIO_PIN_RESET);
}

static void serviceKeyboard_configPins_colsAsInputs()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	// Configure the pins connected to the rows as low level outputs
	GPIO_InitStruct.Pin = EXTKEYBOARD_PIN3_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(EXTKEYBOARD_PIN3_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = EXTKEYBOARD_PIN2_Pin|EXTKEYBOARD_PIN1_Pin|EXTKEYBOARD_PIN0_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	// Set all rows to RESET - Ground
	HAL_GPIO_WritePin(EXTKEYBOARD_PIN0_GPIO_Port, EXTKEYBOARD_PIN0_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EXTKEYBOARD_PIN1_GPIO_Port, EXTKEYBOARD_PIN1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EXTKEYBOARD_PIN2_GPIO_Port, EXTKEYBOARD_PIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EXTKEYBOARD_PIN3_GPIO_Port, EXTKEYBOARD_PIN3_Pin, GPIO_PIN_RESET);

	//Configure the pins connected to the columns as inputs
	GPIO_InitStruct.Pin = EXTKEYBOARD_PIN7_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(EXTKEYBOARD_PIN7_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = EXTKEYBOARD_PIN6_Pin|EXTKEYBOARD_PIN5_Pin|EXTKEYBOARD_PIN4_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	// Time delay for GPIO configuration to take effect
	CYCLE_DELAY_40_TICKS();
}



