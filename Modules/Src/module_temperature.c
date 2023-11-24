/**
 * Temperature module - runs in 500ms task, prints to LCD upper part
 * module_temperature.c
 * 
 * 
 *  Created on: June 17, 2022
 *      Author: Matheus Sozza
 */


/* Private includes ----------------------------------------------------------*/
#include "module_temperature.h"

/* External variables includes -----------------------------------------------*/

/* Internal variables includes -----------------------------------------------*/
enum moduleTemperatureState_e moduleTemperatureState = MODTEMP_NOT_INIT;
LCD_displayBuffer_t *moduleTemperature_LCD_displayBuffer;

/* Internal functions includes -----------------------------------------------*/
static void _moduleTemperature_stateTransition(uint8_t Activate);
static void _moduleTemperature_stateFunction(LCD_displayBuffer_t *LCD_displayBuffer);
static void _moduleTemperature_sendToLCD();
static void _clearPrintArea();
static void _get_CPUTemp();
static void _getAmbTemp();
static void _get_AmbHumidity();

/* Functions implementation --------------------------------------------------*/
void moduleTemperature_runTask(LCD_displayBuffer_t *LCD_displayBuffer,uint8_t Activate)
{
	// State-machine instructions
	_moduleTemperature_stateFunction(LCD_displayBuffer);

	// State-machine transitions
	_moduleTemperature_stateTransition(Activate);
}

static void _moduleTemperature_stateTransition(uint8_t Activate)
{
	if(Activate && moduleTemperatureState == MODTEMP_NOT_INIT)
	{
		moduleTemperatureState = MODTEMP_INIT_RUNNING;
	}
	else if(Activate && moduleTemperatureState == MODTEMP_INIT_STOPPED)
	{
		moduleTemperatureState = MODTEMP_INIT_RUNNING;
	}
	else if(!Activate && moduleTemperatureState == MODTEMP_INIT_RUNNING)
	{
		moduleTemperatureState = MODTEMP_INIT_STOPPED;
	}
}

static void _moduleTemperature_stateFunction(LCD_displayBuffer_t *LCD_displayBuffer)
{
	
	if(moduleTemperatureState == MODTEMP_NOT_INIT)
	{
		tempSensor_init();
		moduleTemperature_LCD_displayBuffer = LCD_displayBuffer;
	}
	else if(moduleTemperatureState == MODTEMP_INIT_STOPPED)
	{
		//Empty
	}
	else if(moduleTemperatureState == MODTEMP_INIT_RUNNING)
	{
		_moduleTemperature_sendToLCD();
	}
}

static void _moduleTemperature_sendToLCD()
{
	static uint8_t majorTick = 0, minorTick = 0;

	switch(majorTick)
	{
		case 0:
			_get_CPUTemp();
			break;
		case 1:
			_getAmbTemp();
			break;
		case 2:
			_get_AmbHumidity();
			break;
		default:
			break;
	}
	minorTick = (minorTick+1) % 6;
	if (minorTick == 5) majorTick = (majorTick+1) % 3;
}

static void _clearPrintArea()
{
	// Clear print area
	for(uint8_t idxRow = 0 ; idxRow < 8 ; idxRow++)
	{
		for(uint8_t idxCol = 0 ; idxCol < 84 ; idxCol++)
		{
			LCD_Buffer_setPixel(moduleTemperature_LCD_displayBuffer,  idxRow,  idxCol, 0);
		}
	}		
}

static void _get_CPUTemp()
{
	static double dataDoubleFil = -255;

	if(tempSensor_checkForData())
	{
		double dataDouble = tempSensor_getTempValue();

		_clearPrintArea();

		// Print first part of string
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'C',0,0);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'P',0,6);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'U',0,12);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, ' ',0,18);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'T',0,24);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'e',0,30);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'm',0,36);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'p',0,42);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, ':',0,48);
		
		// Filter the temperature values
		dataDoubleFil = (dataDoubleFil==-255)? dataDouble : 0.95*dataDoubleFil + 0.05*dataDouble;

		// Create a string to print numeric vals
		char readTempStr[5];
		gcvt(dataDoubleFil, 5, &readTempStr);

		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer,'C',0,78);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer,(char)'º',0,74);
		
		uint8_t currColIdx = 74;
		for(int8_t idx=3; idx>=0; idx--)
		{
			LCD_Char_t char2Write = findCorrespondingChar(readTempStr[idx]);
		
			// If found, print the character (ignore the '?' chars)
			if(char2Write.ASCII_Char != '?')
			{
				// Set cursor to correct column
				currColIdx-= char2Write.bitmap_total_bytes;

				//Print corresponding character
				LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer,readTempStr[idx],0,currColIdx);
			}
		}
	}
}

static void _getAmbTemp()
{
	static double dataDoubleFil = -255;
	static uint8_t dataTimeout = 0;
	static uint16_t dataValue = 0;

	dataTimeout++;
	_clearPrintArea();

	// Print first part of string
	LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'A',0,0);
	LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'm',0,6);
	LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'b',0,12);
	LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, ' ',0,18);
	LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'T',0,24);
	LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'e',0,30);
	LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'm',0,36);
	LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'p',0,42);
	LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, ':',0,48);

	// Query data from external device
	uint16_t dataValue_query = serviceEnvData_getAmbTemperature();
	if(dataValue_query != 0xFFFF)
	{
		dataValue = dataValue_query; // If valid, send to printable value
		dataTimeout = 0; // Reset timeout counter
	}

	// Print second part of string
	if(dataTimeout < 15) // No error
	{
		double dataDouble = 0.1 * ((double) dataValue);
		
		// Filter the temperature values
		dataDoubleFil = (dataDoubleFil==-255)? dataDouble : 0*dataDoubleFil + 1*dataDouble; // No filtering for DHT22

		// Create a string to print numeric vals
		char readTempStr[5];
		gcvt(dataDoubleFil, 5, &readTempStr);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer,(char)'º',0,74);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer,'C',0,78);
		
		uint8_t currColIdx = 74;
		for(int8_t idx=3; idx>=0; idx--)
		{
			LCD_Char_t char2Write = findCorrespondingChar(readTempStr[idx]);
			// If found, print the character (ignore the '?' chars)
			if(char2Write.ASCII_Char != '?')
			{
				// Set cursor to correct column
				currColIdx-= char2Write.bitmap_total_bytes;
				//Print corresponding character
				LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer,readTempStr[idx],0,currColIdx);
			}
		}
	}
	else
	{
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer,(char)'E',0,62);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer,(char)'r',0,66);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer,(char)'r',0,70);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer,(char)'o',0,74);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer,(char)'r',0,78);
	}
}

static void _get_AmbHumidity()
{
	static double dataDoubleFil = -255;
	static uint8_t dataTimeout = 0;
	static uint16_t dataValue = 0;

	dataTimeout++;
	_clearPrintArea();

	// Print first part of string
	LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'A',0,0);
	LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'm',0,6);
	LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'b',0,12);
	LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, ' ',0,18);
	LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'H',0,24);
	LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'u',0,30);
	LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'm',0,36);
	LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, 'i',0,42);
	LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer, ':',0,48);

	// Query data from external device
	uint16_t dataValue_query = serviceEnvData_getAmbHumidity();
	if(dataValue_query != 0xFFFF)
	{
		dataValue = dataValue_query; // If valid, send to printable value
		dataTimeout = 0; // Reset timeout counter
	}

	// Print second part of string
	if(dataTimeout < 15) // No error
	{
		double dataDouble = 0.1 * ((double) dataValue);
		
		// Filter the temperature values
		dataDoubleFil = (dataDoubleFil==-255)? dataDouble : 0*dataDoubleFil + 1*dataDouble; // No filtering for DHT22

		// Create a string to print numeric vals
		char readTempStr[5];
		gcvt(dataDoubleFil, 5, &readTempStr);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer,(char)'%',0,74);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer,' ',0,78);
		
		uint8_t currColIdx = 74;
		for(int8_t idx=3; idx>=0; idx--)
		{
			LCD_Char_t char2Write = findCorrespondingChar(readTempStr[idx]);
			// If found, print the character (ignore the '?' chars)
			if(char2Write.ASCII_Char != '?')
			{
				// Set cursor to correct column
				currColIdx-= char2Write.bitmap_total_bytes;
				//Print corresponding character
				LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer,readTempStr[idx],0,currColIdx);
			}
		}
	}
	else
	{
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer,(char)'E',0,62);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer,(char)'r',0,66);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer,(char)'r',0,70);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer,(char)'o',0,74);
		LCD_Buffer_writeASCIIChar(moduleTemperature_LCD_displayBuffer,(char)'r',0,78);
	}
}



