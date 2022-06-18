/**
 * Temperature sensor reader
 * service_tempSensor.h
 *
 *  Created on: June 17, 2022
 *      Author: Matheus Sozza
 */

#ifndef INC_CORE_SERVICES_TEMPSENSOR_H_
#define INC_CORE_SERVICES_TEMPSENSOR_H_

/* Includes ------------------------------------------------------------------*/
#include "adc.h"

/* Defines -------------------------------------------------------------------*/
#define TS_CAL1 (*(uint16_t*) 0x1FFF7A2C) // Calibration value for 30 degrees
#define TS_CAL2 (*(uint16_t*) 0x1FFF7A2E) // Calibration value for 110 degrees

#define CONV_SLOPE   (double)(110-30)/(double)(TS_CAL2 - TS_CAL1) //Slope coeff. for conversion
#define CONV_LINCOFF (double)(30-(CONV_SLOPE* (double)TS_CAL1)) //Linear coeff. for conversion

//Starts temperature sensor reading
#define tempSensor_init() HAL_ADC_Start(&hadc1) 

// Get state of conversion / if data is ready
#define tempSensor_checkForData() (hadc1.Instance->SR & 2)

//Get raw value stored in the temp. snsr
//#define tempSensor_getRawValue() HAL_ADC_GetValue(&hadc1) 
#define tempSensor_getRawValue() HAL_ADC_GetValue(&hadc1) 

//Get converted value stored in the temp. snsr
#define tempSensor_getTempValue() { \
(double)tempSensor_getRawValue() * \
(double)CONV_SLOPE + (double)CONV_LINCOFF} 

/* Typedef declare -----------------------------------------------------------*/


/* Function prototypes -------------------------------------------------------*/

#endif /* INC_CORE_SERVICES_TEMPSENSOR_H_ */




