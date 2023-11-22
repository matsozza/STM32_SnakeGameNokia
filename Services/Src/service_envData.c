/*
 * Environmental data (temperature and humidity) handling services
 * service_envData.c
 *
 *
 *  Created on: November 21, 2023
 *      Author: Matheus Sozza
 */

/* Private includes ----------------------------------------------------------*/
#include "service_envData.h"
#include "service_uart.h"
#include "tim.h"

/* External variables includes -----------------------------------------------*/

/* Internal variables includes -----------------------------------------------*/

enum envDataState_e envDataState = ENVDATA_QUERY_START;
uint8_t idxSample = 0;
volatile uint32_t samplePrev, sampleCurr, samplePerDurn;
uint32_t debugBuffer[100];

/* Internal functions includes --------------------------------------------------*/
void _serviceEnvData_stateFunction(enum envDataITSrc_e callerSrc);
void _serviceEnvData_queryStart();
void _serviceEnvData_queryEnd();
void _IO_setPinAsInput();
void _IO_setPinAsOutput();
uint32_t _misc_samplePer(uint32_t samplePrev, uint32_t sampleCurr);

/* Functions implementation --------------------------------------------------*/

void serviceEnvData_TIM_PeriodElapsedCallback_LowRes() // Called when a query is to be requested
{
	#if ENVDATA_DEBUG_LVL_USART >=1
	char debugMsg[25];
	sprintf(debugMsg, "--> %d\n\r", (int)idxSample);
	USART2_addToQueue(debugMsg);
	#endif

	_serviceEnvData_queryStart();
}

void serviceEnvData_TIM_PeriodElapsedCallback_HighRes() // Called when query start counter (~800ms) is expired
{
	_serviceEnvData_queryEnd();
}

void serviceEnvData_GPIO_EXTI_Callback(uint16_t GPIO_Pin) // DATA Pin interruption (either while INPUT or OUTPUT mode)
{
	// Update period buffers
	samplePrev = sampleCurr;
	sampleCurr = ENVDATA_CLK_SRC;
	samplePerDurn = _misc_samplePer(samplePrev, sampleCurr);

	// Update sample counters
	debugBuffer[idxSample] = samplePerDurn;
	idxSample++;
	return;
}

void _serviceEnvData_queryStart()
{
	// Start values
	idxSample = 0;
	samplePrev = sampleCurr;
	sampleCurr = ENVDATA_CLK_SRC;

	// Set pin as output
	_IO_setPinAsOutput();
	
	// Set DHT22 DATA pin to LOW
	HAL_GPIO_WritePin(ENV_DATA_GPIO_Port, ENV_DATA_Pin, GPIO_PIN_RESET);

	// Set counter to 800us and start counter 
	HAL_TIM_Base_Start_IT(&ENVDATA_COUNTER_HIGHRES); 
}

void _serviceEnvData_queryEnd()
{
	// Set pin to high level
	HAL_GPIO_WritePin(ENV_DATA_GPIO_Port, ENV_DATA_Pin, GPIO_PIN_SET);

	// Stop high-res counter
	HAL_TIM_Base_Stop_IT(&ENVDATA_COUNTER_HIGHRES); 

	// Set pin to input
	_IO_setPinAsInput();
}

void _IO_setPinAsOutput()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	// Configure the DHT22 data pin as high level outputs
	GPIO_InitStruct.Pin = ENV_DATA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(ENV_DATA_GPIO_Port, &GPIO_InitStruct);

	// Set control pin to high level (pull-up architecture)
	HAL_GPIO_WritePin(ENV_DATA_GPIO_Port, ENV_DATA_Pin, GPIO_PIN_SET);
}

void _IO_setPinAsInput()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	// Configure the DHT22 data pin as interruptible input
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING; // Any transition will trigger the interruption
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Pin = ENV_DATA_Pin;
	HAL_GPIO_Init(ENV_DATA_GPIO_Port, &GPIO_InitStruct);
}

// Returns the interval between two interruption in us (microseconds)
uint32_t _misc_samplePer(uint32_t samplePrev, uint32_t sampleCurr)
{
	if(sampleCurr >= samplePrev)
	{
		return (sampleCurr - samplePrev) / (uint32_t) ENVDATA_CLK_FRQ;
	}
	else
	{
		return ((0xFFFFFFFF - samplePrev) + sampleCurr) / (uint32_t) ENVDATA_CLK_FRQ;
	}
}
