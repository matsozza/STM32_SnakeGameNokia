/**
 * UART Management module
 * module_uart.c
 *
 *  Created on: Apr 22, 2022
 *      Author: Matheus Sozza
 */

#include <stdlib.h>
#include <string.h>
#include "usart.h"
#include "module_uart.h"
#include "service_uart.h"

char *testStr, *testStrCat, *numberStr;

void UART_printTestString()
{
	  testStr = malloc(sizeof(char)*16);
	  testStrCat = malloc(sizeof(char)*32);
	  numberStr = malloc(sizeof(char)*32);

	  strcpy(testStr,"TestStr ");
	  //USART2_sendString(testStr);


	  free(testStr);
	  free(testStrCat);
	  free(numberStr);
}

void UART_printLEDString()
{
	  testStr = malloc(sizeof(char)*16);
	  testStrCat = malloc(sizeof(char)*32);
	  numberStr = malloc(sizeof(char)*32);

	  strcpy(testStr,"-->LedString ");
	  USART2_sendString(testStr);


	  free(testStr);
	  free(testStrCat);
	  free(numberStr);
}

void UART_printMsg(char *msg)
{
	  USART2_addToQueue(msg);
}


