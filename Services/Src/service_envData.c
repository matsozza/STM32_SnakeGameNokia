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
uint8_t recordSamples = 0, idxSample = 0, dataAvailable = 0, errCode = 0;
uint16_t ambTemp = 0, ambHumidity = 0;

volatile uint32_t samplePrev=0, sampleCurr=0, samplePerDurn = 0;
volatile uint16_t streamBuffer[85] = {0};
volatile uint8_t dataBuffer[40] = {0};

/* Internal functions includes --------------------------------------------------*/

static void _queryStart();
static void _queryEnd();
static void _streamStart();
static void _streamEnd();
static uint8_t _decodeStream();
static void _publishStreamResult(uint8_t errCode);
void _IO_setPinAsInput();
void _IO_setPinAsOutput();
void _TIM_setHighResPeriod(uint16_t period);
uint16_t _misc_samplePer(uint32_t samplePrev, uint32_t sampleCurr);

/* Functions implementation --------------------------------------------------*/

// ********************* API Consumption *********************
void serviceEnvData_serviceInit()
{
	idxSample = 0;
	HAL_TIM_Base_Start_IT(&ENVDATA_COUNTER_LOWRES); // Start query timer
}

int16_t serviceEnvData_getAmbTemperature()
{
	if(dataAvailable & 0b01 > 0) // Data available for consumption
	{
		dataAvailable = dataAvailable & 0b10; // Clear bit 0
		return ambTemp;
	}
	else // Data not available for consumption
	{
		return 0xFFFF;		
	}
}

int16_t serviceEnvData_getAmbHumidity()
{
	if(dataAvailable & 0b10 > 0) // Data available for consumption
	{
		dataAvailable = dataAvailable & 0b01; // Clear bit 1
		return ambHumidity;
	}
	else // Data not available for consumption
	{
		return 0xFFFF;		
	}
}

// ********************* Interruption callbacks *********************
void serviceEnvData_TIM_PeriodElapsedCallback_LowRes() // Called when a query is to be requested (every ~2.5s)
{
	_queryStart();
}

void serviceEnvData_TIM_PeriodElapsedCallback_HighRes() // Called when high-res counter is expired, both for queryEnd and streamingEnd (different periods)
{
	if(!recordSamples)
	{
		_queryEnd();
	}
	else
	{
		_streamEnd();
		errCode |= _decodeStream();
		_publishStreamResult(errCode);
		
		// Restart low res query timer
		HAL_TIM_Base_Start_IT(&ENVDATA_COUNTER_LOWRES);
	}
}

void serviceEnvData_GPIO_EXTI_Callback(uint16_t GPIO_Pin) // DATA Pin interruption (either while INPUT or OUTPUT mode)
{
	_streamStart();
}

// ********************* Local methods for requesting & buffering received data *********************
static void _queryStart()
{
	// Stop low res query timer
	HAL_TIM_Base_Stop_IT(&ENVDATA_COUNTER_LOWRES); 
	
	// Start global values
	idxSample = 0;
	errCode = 0;
	recordSamples = 0;
	memset(streamBuffer, 0, sizeof(streamBuffer));
	samplePrev = sampleCurr;
	sampleCurr = DWT->CYCCNT;

	// Set pin as output
	_IO_setPinAsOutput();
	
	// Set DHT22 DATA pin to LOW
	HAL_GPIO_WritePin(ENV_DATA_GPIO_Port, ENV_DATA_Pin, GPIO_PIN_RESET);

	// Set high-res counter to 900us and start it 
	_TIM_setHighResPeriod(900);
	HAL_TIM_Base_Start_IT(&ENVDATA_COUNTER_HIGHRES);

	#if ENVDATA_DEBUG_LVL_USART >=1
	char debugMsg[24];
	sprintf(debugMsg, "\n\rQUERY_ST\n\r");
	USART2_addToQueue(debugMsg);
	#endif
}

static void _queryEnd()
{
	// Stop high-res counter
	HAL_TIM_Base_Stop_IT(&ENVDATA_COUNTER_HIGHRES); 

	// Set pin to high level
	HAL_GPIO_WritePin(ENV_DATA_GPIO_Port, ENV_DATA_Pin, GPIO_PIN_SET);

	// Set pin to input
	recordSamples = 1;
	_IO_setPinAsInput();
	
	// Set high-res counter to 10ms (10000us) and start it 
	_TIM_setHighResPeriod(10000);
	HAL_TIM_Base_Start_IT(&ENVDATA_COUNTER_HIGHRES);

	#if ENVDATA_DEBUG_LVL_USART >=1
	char debugMsg[24];
	sprintf(debugMsg, "QUERY_END\n\r");
	USART2_addToQueue(debugMsg);
	#endif	
}

static void _streamStart()
{
	if(idxSample<86){
		// Update period buffers
		samplePrev = sampleCurr;
		sampleCurr = DWT->CYCCNT;

		// Calculate period duration
		samplePerDurn = _misc_samplePer(samplePrev, sampleCurr);
		
		streamBuffer[idxSample] = (uint16_t) samplePerDurn;
		idxSample++;
	}
	else{
		return; // Avoid stack overflow
	}
}

static void _streamEnd()
{
	// Stop high-res counter
	HAL_TIM_Base_Stop_IT(&ENVDATA_COUNTER_HIGHRES);

	// Start low res query timer
	//HAL_TIM_Base_Start_IT(&ENVDATA_COUNTER_LOWRES);

	#if ENVDATA_DEBUG_LVL_USART >=2
	char debugMsg[24];
	sprintf(debugMsg, "STREAM_END\n\r");
	USART2_addToQueue(debugMsg);
	_debug_printBuffers();
	#endif
}

static uint8_t _decodeStream()
{
	uint8_t idxS = 0, idxD = 0, state = 0, edgeNum = 0;
	uint8_t edgePer[2] = {0,0};

	for(idxS = 0; idxS < 86; idxS++)
	{
		switch(state)
		{
			case 0: // Query (low side ~900ms)
				if(streamBuffer[idxS] > 800 && streamBuffer[idxS] < 1200) state++;
				break;

			case 1: // Acknowledge 1 (80us)
				if(streamBuffer[idxS] > 60 && streamBuffer[idxS] < 100) state++;
				break;

			case 2: // Acknowledge 2 (80us)
				if(!(streamBuffer[idxS] > 60 && streamBuffer[idxS] < 100)) return 0b0100; // Error not ACK2
				state++;
				break;

			case 3: // Decode stream
				if(edgeNum==0)
				{
					edgeNum = 1;
					edgePer[0] = streamBuffer[idxS];
				}
				else
				{
					edgeNum = 0;
					edgePer[1] = streamBuffer[idxS];	

					// Bit 0 - 50us + 26us
					// Bit 1 - 50us + 70us
					if(edgePer[0]>40 && edgePer[0]<60 && edgePer[1]>15 && edgePer[1]<35)
					{
						dataBuffer[idxD++] = 0;
					}
					else if(edgePer[0]>40 && edgePer[0]<60 && edgePer[1]>60 && edgePer[1]<80)
					{
						dataBuffer[idxD++] = 1;
					}
					else
					{
						return 0b0001; // NOK invalid data
					}
				}
				if(idxD==40) state++;				
				break;

			case 4: // End of transmission
				if (streamBuffer[idxS] > 40 && streamBuffer[idxS] < 60)
				{
					return 0b0000;// End of transmission succesfully achieved
				}
				else
				{
					return 0b0010; // Missing end of transmission
				}

			default:
				break;
		}
	}

	return 0b1000; // Error no valid stream found / completed
}

static void _publishStreamResult(uint8_t errCode)
{
	uint8_t tHi=0, tLo=0, hHi=0, hLo=0, checkRec=0;

	// Convert from buffer to integer values
	for(uint8_t idx = 0; idx < 8 ; idx++)
	{
		hHi |= ((uint8_t)(dataBuffer[7 - idx] > 0) << idx);
		hLo |= ((uint8_t)(dataBuffer[15 - idx] > 0) << idx);
		tHi |= ((uint8_t)(dataBuffer[23 - idx] > 0) << idx);
		tLo |= ((uint8_t)(dataBuffer[31 - idx] > 0) << idx);
		checkRec |= ((uint8_t)(dataBuffer[39 - idx] > 0) << idx);
	}

	// Perform checksum validation
	uint8_t checkCalc = (uint8_t)((uint16_t)tHi + (uint16_t)tLo + (uint16_t)hHi + (uint16_t)hLo) & 0XFF;

	if(checkCalc != checkRec) errCode |= 0b10000; // Checksum error

	#if ENVDATA_DEBUG_LVL_USART >=1
	char debugMsg[24];
	sprintf(debugMsg, "CHK_%d_%d\n\r", checkRec, checkCalc);
	USART2_addToQueue(debugMsg);
	#endif

	if(errCode == 0)
	{
		ambTemp = (tHi<<8) | tLo;
		ambHumidity = (hHi<<8) | hLo;
		dataAvailable = 1;

		#if ENVDATA_DEBUG_LVL_USART >=1
		char debugMsg[24];
		sprintf(debugMsg, "T_%d_H_%d\n\r", ambTemp, ambHumidity);
		USART2_addToQueue(debugMsg);
		#endif
	}
	else
	{
		ambTemp = 0xFFFF;
		ambHumidity = 0xFFFF;
		dataAvailable = 0;

		#if ENVDATA_DEBUG_LVL_USART >=1
		char debugMsg[24];
		sprintf(debugMsg, "ERR_%d\n\r", errCode);
		USART2_addToQueue(debugMsg);
		#endif
	}
}

// ********************* Low level hardware, timers & pins manipulation *********************

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

void _TIM_setHighResPeriod(uint16_t period)
{
	ENVDATA_COUNTER_HIGHRES.Instance->ARR = period; // Change period directly in timer register
}

// ********************* Miscellaneous methods *********************

// Returns the interval between two interruption in us (microseconds)
uint16_t _misc_samplePer(uint32_t samplePrev, uint32_t sampleCurr)
{
	if(sampleCurr >= samplePrev) return (uint16_t) ((sampleCurr - samplePrev) / (uint32_t) ENVDATA_CLK_FRQ);

	return (uint16_t) (((0xFFFFFFFF - samplePrev) + sampleCurr) / (uint32_t) ENVDATA_CLK_FRQ);
}

#if ENVDATA_DEBUG_LVL_USART >=2
void _debug_printBuffers()
{
	char debugMsg[24];
	sprintf(debugMsg, "nSamples: %d\n\r",idxSample);
	USART2_addToQueue(debugMsg);

	sprintf(debugMsg, "Stream:\n\r");
	USART2_addToQueue(debugMsg);
	for(uint8_t idxB = 0; idxB < 86; idxB++)
	{
		sprintf(debugMsg, "%d ", streamBuffer[idxB]);
		USART2_addToQueue(debugMsg);
	}
	sprintf(debugMsg, "\n\r");
	USART2_addToQueue(debugMsg);

	sprintf(debugMsg, "Data:\n\r");
	USART2_addToQueue(debugMsg);
	for(uint8_t idxB = 0; idxB < 40; idxB++)
	{
		sprintf(debugMsg, "%d ", dataBuffer[idxB]);
		USART2_addToQueue(debugMsg);
	}
	sprintf(debugMsg, "\n\r");
	USART2_addToQueue(debugMsg);
}
#endif
