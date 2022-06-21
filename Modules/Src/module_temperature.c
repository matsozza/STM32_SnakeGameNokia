/**
 * Temperature module - runs in 500ms task, prints to LCD upper part
 * module_snake.c
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
	static double readTempDoubleFil = -255;

	if(tempSensor_checkForData())
	{
		double readTempDouble = tempSensor_getTempValue();

		// Filter the temperature values
		readTempDoubleFil = (readTempDoubleFil==-255)? readTempDouble : 0.95*readTempDoubleFil + 0.05*readTempDouble;

		// Create a string to print
		char readTempStr[5];
		gcvt(readTempDoubleFil, 5, &readTempStr);

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



