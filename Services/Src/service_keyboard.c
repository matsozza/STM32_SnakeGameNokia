/*
 * External Matricial Keyboard Service
 * service_keyboard.c
 *
 *  Created on: June 20, 2022
 *      Author: Matheus Sozza
 */

/* Private includes ----------------------------------------------------------*/
#include "service_keyboard.h"
#include "service_uart.h"
#include "service_cycleDelay.h"
#include "gpio.h"
#include "tim.h"

/* External variables includes -----------------------------------------------*/

/* Internal variables includes -----------------------------------------------*/
uint8_t serviceKeyboard_busyFlag = 0;
uint8_t serviceKeyboard_colReadAttempts;
uint8_t serviceKeyboard_pressedRow;
uint8_t serviceKeyboard_pressedCol;
const char serviceKeyboard_lookupTable[4][4] = {
                                                {'1', '2', '3', 'A'},
                                                {'4', '5', '6', 'B'},
                                                {'7', '8', '9', 'C'},
                                                {'*', '0', '#', 'D'}
                                                };
serviceKeyboardInput_t  serviceKeyboardInput = {'x', 1 };

/* Internal functions includes -----------------------------------------------*/
static void serviceKeyboard_enableKeyInterrupts(uint8_t Enable);
static char serviceKeyboard_lookUpValue();
static uint8_t serviceKeyboard_getPressedCol();
static uint8_t serviceKeyboard_getPressedRow(uint16_t GPIO_Pin);
static void serviceKeyboard_configPins_rowsAsInputs();
static void serviceKeyboard_configPins_colsAsInputs();

/* Functions implementation --------------------------------------------------*/

// STEP 1 - Callback function for GPIO click
void serviceKeyboard_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	//If last input still not consumed by task, ignore other inputs
	if(serviceKeyboardInput.inputConsumed && !serviceKeyboard_busyFlag)
	{
		serviceKeyboard_colReadAttempts = 0;
		serviceKeyboard_busyFlag=1;

		// Get the row corresponding to the pressed key
		serviceKeyboard_pressedRow = serviceKeyboard_getPressedRow(GPIO_Pin);

		// Start debounce timer
		HAL_TIM_Base_Start_IT(&EXTKEYBOARD_TIMER_HANDLE);

		// Reconfigure the GPIOs to get the column of the pressed key
		serviceKeyboard_configPins_colsAsInputs();
	}
}

// STEP 2 - Callback function for debouncer TIMER
void serviceKeyboard_TIM_PeriodElapsedCallback()
{
	// Stop timer
	HAL_TIM_Base_Stop_IT(&EXTKEYBOARD_TIMER_HANDLE);

	// Get the column corresponding to the pressed key
	serviceKeyboard_pressedCol = serviceKeyboard_getPressedCol();
	char readKey = serviceKeyboard_lookUpValue();

	// Check if a column has been detected
	if(readKey != '<' && readKey != '>' )
	{
		serviceKeyboardInput.inputKey = readKey;
		serviceKeyboardInput.inputConsumed = 0;
		
		#if EXTKEYBOARD_DEBUG_LVL_USART >=1
			char debugMsg[25];
			sprintf(debugMsg, "SKB_ColAttempt:%d\n\r", serviceKeyboard_colReadAttempts);
			USART2_addToQueue(debugMsg);
		#endif
	}
	else if(serviceKeyboard_colReadAttempts < EXTKEYBOARD_MAX_COL_READ_ATTEMPT)
	{
		serviceKeyboard_colReadAttempts++;
		HAL_StatusTypeDef tim_status = HAL_TIM_Base_Start_IT(&EXTKEYBOARD_TIMER_HANDLE);
		return;
	}
	else
	{
		#if EXTKEYBOARD_DEBUG_LVL_USART >=1
			char debugMsg[15];
			sprintf(debugMsg, "SKB_LostKey\n\r", serviceKeyboard_colReadAttempts);
			USART2_addToQueue(debugMsg);
		#endif
	}

	// Move GPIOs back to orig. config, interruptible rows as inputs, wait next click
	serviceKeyboard_configPins_rowsAsInputs();
	serviceKeyboard_busyFlag=0;
	serviceKeyboard_colReadAttempts=0;
}

char serviceKeyboard_consumeKey()
{
	if(!serviceKeyboardInput.inputConsumed)
	{
		// Flag the input as consumed
		serviceKeyboardInput.inputConsumed = 1;

		#if EXTKEYBOARD_DEBUG_LVL_USART >=1
		// USART feedback about pressed key
		char strKey[16];
		sprintf(strKey, "SKB_KeyPress:%c \n\r",serviceKeyboardInput.inputKey);
		USART2_addToQueue(&strKey);
		#endif

		return serviceKeyboardInput.inputKey;
	}
	else
	{
		return 'x'; // No input to be consumed
	}
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
	int8_t colIdx;
	switch(serviceKeyboard_pressedCol)
	{
		case 0b0001:
			colIdx = 0;
			break;
		case 0b0010:
			colIdx = 1;
			break;
		case 0b0100:
			colIdx = 2;
			break;
		case 0b1000:
			colIdx = 3;
			break;
		default:
			colIdx=-1;
	}

	// Get row Idx
	int8_t rowIdx;
	switch(serviceKeyboard_pressedRow)
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
			rowIdx=-1;
	}

	//Lookup value
	if(colIdx<0)
	{
		return '<';
	}
	else if(rowIdx<0)
	{
		return '>';
	}

	return serviceKeyboard_lookupTable[rowIdx][colIdx];
}

static uint8_t serviceKeyboard_getPressedRow(uint16_t GPIO_Pin)
{
    uint8_t serviceKeyboard_pressedRow_loc;

	switch(GPIO_Pin)
    {
		case EXTKEYBOARD_PIN0_Pin:
			serviceKeyboard_pressedRow_loc = 0b0001;
			break;
		case EXTKEYBOARD_PIN1_Pin:
			serviceKeyboard_pressedRow_loc = 0b0010;
			break;
		case EXTKEYBOARD_PIN2_Pin:
			serviceKeyboard_pressedRow_loc = 0b0100;
			break;
		case EXTKEYBOARD_PIN3_Pin:
			serviceKeyboard_pressedRow_loc = 0b1000;
			break;
		default:
			serviceKeyboard_pressedRow_loc = 0b0000;
    }

	return serviceKeyboard_pressedRow_loc;
}

static uint8_t serviceKeyboard_getPressedCol()
{
	//Read column pins
	uint8_t colPressed_0 = HAL_GPIO_ReadPin(EXTKEYBOARD_PIN4_GPIO_Port, EXTKEYBOARD_PIN4_Pin)==0;
	uint8_t colPressed_1 = HAL_GPIO_ReadPin(EXTKEYBOARD_PIN5_GPIO_Port, EXTKEYBOARD_PIN5_Pin)==0;
	uint8_t colPressed_2 = HAL_GPIO_ReadPin(EXTKEYBOARD_PIN6_GPIO_Port, EXTKEYBOARD_PIN6_Pin)==0;
	uint8_t colPressed_3 = HAL_GPIO_ReadPin(EXTKEYBOARD_PIN7_GPIO_Port, EXTKEYBOARD_PIN7_Pin)==0;
	
	uint8_t colPressed = colPressed_0 + 2*colPressed_1 + 4*colPressed_2 + 8*colPressed_3;

	switch(colPressed)
    {
		case 1:
			return 0b0001;
			break;
		case 2:
			return 0b0010;
			break;
		case 4:
			return 0b0100;
			break;
		case 8:
			return 0b1000;
			break;
		default:
			return 0b0000;
    }
}

static void serviceKeyboard_configPins_rowsAsInputs()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	// Configure the pins connected to the rows as interruptible inputs
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Pin = EXTKEYBOARD_PIN3_Pin;
	HAL_GPIO_Init(EXTKEYBOARD_PIN3_GPIO_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = EXTKEYBOARD_PIN2_Pin|EXTKEYBOARD_PIN1_Pin|EXTKEYBOARD_PIN0_Pin;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	//Configure the pins connected to the columns as low level outputs
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Pin = EXTKEYBOARD_PIN7_Pin;
	HAL_GPIO_Init(EXTKEYBOARD_PIN7_GPIO_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = EXTKEYBOARD_PIN6_Pin|EXTKEYBOARD_PIN5_Pin|EXTKEYBOARD_PIN4_Pin;
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
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Pin = EXTKEYBOARD_PIN3_Pin;
	HAL_GPIO_Init(EXTKEYBOARD_PIN3_GPIO_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = EXTKEYBOARD_PIN2_Pin|EXTKEYBOARD_PIN1_Pin|EXTKEYBOARD_PIN0_Pin;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	// Set all rows to RESET - Ground
	HAL_GPIO_WritePin(EXTKEYBOARD_PIN0_GPIO_Port, EXTKEYBOARD_PIN0_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EXTKEYBOARD_PIN1_GPIO_Port, EXTKEYBOARD_PIN1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EXTKEYBOARD_PIN2_GPIO_Port, EXTKEYBOARD_PIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EXTKEYBOARD_PIN3_GPIO_Port, EXTKEYBOARD_PIN3_Pin, GPIO_PIN_RESET);

	//Configure the pins connected to the columns as inputs
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Pin = EXTKEYBOARD_PIN7_Pin;
	HAL_GPIO_Init(EXTKEYBOARD_PIN7_GPIO_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = EXTKEYBOARD_PIN6_Pin|EXTKEYBOARD_PIN5_Pin|EXTKEYBOARD_PIN4_Pin;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	// Time delay for GPIO configuration to take effect
	CYCLE_DELAY_80_TICKS();
	CYCLE_DELAY_40_TICKS();
}



