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

enum envDataState_e envDataState = ENVDATA_IDLE;
uint8_t idxSample = 0;
uint8_t intError = 0;
uint16_t ambTemp = 0;
uint16_t ambHumidity = 0;
uint8_t ambVld = 0;
volatile uint32_t samplePrev, sampleCurr, samplePerDurn;
uint32_t debugBuffer[100];
uint8_t streamBuffer[40];

/* Internal functions includes --------------------------------------------------*/
void _serviceEnvData_stateTransition();
void _serviceEnvData_stateFunction();
void _serviceEnvData_queryStart();
void _serviceEnvData_queryEnd();
void _serviceData_streaming();
void _serviceData_checking();
void _IO_setPinAsInput();
void _IO_setPinAsOutput();
uint32_t _misc_samplePer(uint32_t samplePrev, uint32_t sampleCurr);

/* Functions implementation --------------------------------------------------*/

void serviceEnvData_TIM_PeriodElapsedCallback_LowRes() // Called when a query is to be requested
{
	#if ENVDATA_DEBUG_LVL_USART >=1
	char debugMsg[25];
	sprintf(debugMsg, "--> %d\n\r\n\r", (int)idxSample);
	USART2_addToQueue(debugMsg);

	for(uint16_t idx = 0; idx < 86 ; idx++)
	{
		sprintf(debugMsg, " %d ", (int)debugBuffer[idx]);
		USART2_addToQueue(debugMsg);
	}
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

	// Call stream processing function
	//_serviceData_stateFunction();
	_serviceEnvData_stateTransition();

	return;
}

void _serviceEnvData_queryStart()
{
	// Set pin as output
	_IO_setPinAsOutput();
	
	// Set DHT22 DATA pin to LOW
	HAL_GPIO_WritePin(ENV_DATA_GPIO_Port, ENV_DATA_Pin, GPIO_PIN_RESET);

	// Set counter to 800us and start counter 
	HAL_TIM_Base_Start_IT(&ENVDATA_COUNTER_HIGHRES);

	// Start timer values
	idxSample = 0;
	intError = 0;
	samplePrev = sampleCurr;
	sampleCurr = ENVDATA_CLK_SRC;
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

void _serviceEnvData_stateTransition()
{
	if(intError != 0) // If any internal error happens
	{
		envDataState = ENVDATA_ERR;
	}
	else if(envDataState == ENVDATA_IDLE && idxSample == 1)
	{
		envDataState = ENVDATA_QUERYING;
	}
	else if(envDataState == ENVDATA_QUERYING && idxSample == 2 )
	{
		envDataState = (samplePerDurn>800 && samplePerDurn<1200) ? ENVDATA_QUERY_OK : ENVDATA_ERR;
	}
	else if (envDataState == ENVDATA_QUERY_OK && idxSample == 4)
	{
		envDataState = (samplePerDurn>65 && samplePerDurn<95) ? ENVDATA_QUERY_ACK1 : ENVDATA_ERR;
	}
	else if (envDataState == ENVDATA_QUERY_ACK1 && idxSample == 5)
	{
		envDataState = (samplePerDurn>65 && samplePerDurn<95) ? ENVDATA_STREAMING : ENVDATA_ERR;
	}
	else if(envDataState == ENVDATA_STREAMING)
	{
		if(idxSample == 86)
		{
			envDataState = ENVDATA_CHECKING;
		}
		else
		{
			_serviceData_streaming();
		}
	}
	else if (envDataState == ENVDATA_CHECKING)
	{
		_serviceData_checking();
		envDataState = ENVDATA_IDLE;
	}
	else if (envDataState == ENVDATA_ERR)
	{
		ambVld = 0;
		envDataState = ENVDATA_IDLE;
	}
}

/*
void _serviceData_stateFunction()
{
	if(envDataState == ENVDATA_STREAMING)
	{
		_serviceData_streaming();
	}
	else if(envDataState == ENVDATA_CHECKING)
	{
		_serviceData_checking();
	}
}
*/


void _serviceData_streaming()
{
	static uint8_t idxBuf = 0;
	static uint8_t edgeNum = 0;
	static uint8_t edgePer[2] = {0,0};

	if(edgeNum == 0){
		edgeNum = 1;
		edgePer[0] = samplePerDurn;
	}
	else{
		edgeNum = 0;
		edgePer[1] = samplePerDurn;

		// Bit 0 - 50us + 26us
		// Bit 1 - 50us + 70us
		if(edgePer[0] > 40 && edgePer[0] < 60 &&
		   edgePer[1] > 15 && edgePer[1] < 35)
		{
			streamBuffer[idxBuf++] = 0;
		}
		else if(edgePer[0] > 40 && edgePer[0] < 60 &&
				edgePer[1] > 60 && edgePer[1] < 80)
		{
			streamBuffer[idxBuf++] = 1;
		}
		else
		{
			intError = 1; //Period error
		}
	}

	// Reset counter for next cycle if finished OR error
	if(idxBuf == 40 || intError != 0)
	{
		idxBuf = 0;
		edgeNum = 0;
	}
}

void _serviceData_checking()
{
	uint8_t tHi=0, tLo=0, hHi=0, hLo=0, checkRec;

	// Convert from buffer to integer values
	for(uint8_t idx = 0; idx < 8 ; idx++)
	{
		hHi |= (  (uint8_t)(streamBuffer[7 - idx] > 0) << idx);
		hLo |= (  (uint8_t)(streamBuffer[15 - idx] > 0) << idx);
		tHi |= (  (uint8_t)(streamBuffer[23 - idx] > 0) << idx);
		tLo |= (  (uint8_t)(streamBuffer[31 - idx] > 0) << idx);
		checkRec |= (  (uint8_t)(streamBuffer[39 - idx] > 0) << idx);
	}

	// Perform checksum validation
	uint16_t checkCalc = (uint16_t)tHi + (uint16_t)tLo + (uint16_t)hHi + (uint16_t)hLo;

	if((uint8_t)(checkCalc & 0xff) != checkRec)
	{
		intError = 2;
		return;
	}
	
	// Fill global variables with temperature and humidity values
	ambTemp = (tHi << 8 ) | tLo;
	ambHumidity = (hHi << 8 ) | hLo;
	ambVld = 1;
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
