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
#include "service_tempSensor.h"
#include "service_lcd.h"
#include "service_lcd_fonts.h"
#include <stdlib.h>
#include <string.h>

/* External variables includes -----------------------------------------------*/
extern LCD_displayBuffer_t *LCD_displayBuffer01;

/* Internal variables includes -----------------------------------------------*/
enum moduleTemperatureState_e moduleTemperatureState = NOT_INIT;

/* Internal functions includes -----------------------------------------------*/
static void _moduleTemperature_stateTransition(uint8_t Activate);
static void _moduleTemperature_stateFunction();
static void _moduleTemperature_sendToLCD();

/* Functions implementation --------------------------------------------------*/
void moduleTemperature_runTask(uint8_t Activate)
{
	// State-machine instructions
	_moduleTemperature_stateFunction();

	// State-machine transitions
	_moduleTemperature_stateTransition(Activate);
}

static void _moduleTemperature_stateTransition(uint8_t Activate)
{
	if(Activate && moduleTemperatureState == NOT_INIT)
	{
		moduleTemperatureState = INIT_RUNNING;
	}
	else if(Activate && moduleTemperatureState == INIT_STOPPED)
	{
		moduleTemperatureState = INIT_RUNNING;
	}
	else if(!Activate && moduleTemperatureState == INIT_RUNNING)
	{
		moduleTemperatureState = INIT_STOPPED;
	}
}

static void _moduleTemperature_stateFunction()
{
	
	if(moduleTemperatureState == NOT_INIT)
	{
		tempSensor_init();
	}
	else if(moduleTemperatureState == INIT_STOPPED)
	{
		//Empty
	}
	else if(moduleTemperatureState == INIT_RUNNING)
	{
		_moduleTemperature_sendToLCD();
	}
}

static void _moduleTemperature_sendToLCD()
{
	if(!tempSensor_checkForData()){
		return;
	}

	double readTempDouble = tempSensor_getTempValue();
	char readTempStr[5];
	gcvt(readTempDouble, 5, &readTempStr);

	LCD_Buffer_setCursor(LCD_displayBuffer01, 0, 78); // 83-5
	LCD_Buffer_writeASCIIChar(LCD_displayBuffer01,'C');
	LCD_Buffer_setCursor(LCD_displayBuffer01, 0, 74); // 83-5-4
	LCD_Buffer_writeASCIIChar(LCD_displayBuffer01,'º');
	
	uint8_t currColIdx = 76;

	for(int8_t idx=3; idx>=0; idx--)
	{
		LCD_Char_t char2Write = findCorrespondingChar(readTempStr[idx]);
	
		// If found, print the character (ignore the '?')
		if(char2Write.ASCII_Char != '?')
		{
			// Set cursor to correct column
			currColIdx-= char2Write.bitmap_total_bytes;
			LCD_Buffer_setCursor(LCD_displayBuffer01,0,currColIdx);

			//Print corresponding character
			LCD_Buffer_writeASCIIChar(LCD_displayBuffer01,readTempStr[idx]);
		}

	}
	
	
}



