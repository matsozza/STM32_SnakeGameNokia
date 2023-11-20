/*
 * Flash Memory handling services
 * service_flashMem.c
 *
 *
 *  Created on: July 20, 2022
 *      Author: Matheus Sozza
 */

/* Private includes ----------------------------------------------------------*/
#include "service_flashMem.h"

/* External variables includes -----------------------------------------------*/

/* Internal variables includes -----------------------------------------------*/

/* Functions implementation --------------------------------------------------*/

int8_t flashMem_writeByte(uint8_t Data, uint32_t Address, uint32_t Offset)
{
	if(HAL_FLASH_Unlock() == HAL_OK)
	{
		uint32_t finalAddr = Address + Offset;
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, finalAddr, (uint64_t)Data) == HAL_OK)
		{
			HAL_FLASH_Lock();
			return 0;
		}
	}
	HAL_FLASH_Lock();
	return -1;
}

int8_t flashMem_writeHalfWord(uint16_t Data, uint32_t Address, uint32_t Offset)
{
	if(HAL_FLASH_Unlock() == HAL_OK)
	{
		uint32_t finalAddr = Address + Offset;
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, finalAddr, (uint64_t)Data) == HAL_OK)
		{
			HAL_FLASH_Lock();
			return 0;
		}
	}
	HAL_FLASH_Lock();
	return -1;
}

uint8_t flashMem_getByte(uint32_t Address, uint32_t Offset)
{
	return *((uint8_t*) (Address + Offset));
}

uint16_t flashMem_getHalfWord(uint32_t Address, uint32_t Offset)
{
	return *((uint16_t*) (Address + Offset));
}

int8_t flashMem_eraseSector()
{
	if(HAL_FLASH_Unlock() == HAL_OK)
	{
		FLASH_Erase_Sector(FLASHMEM_DEFAULT_SECTOR, FLASHMEM_DEFAULT_VRANGE);
		HAL_FLASH_Lock();
		return 0;
	}
	return -1;
}
