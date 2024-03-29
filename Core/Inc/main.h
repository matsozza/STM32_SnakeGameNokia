/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_9
#define LED1_GPIO_Port GPIOF
#define LED2_Pin GPIO_PIN_10
#define LED2_GPIO_Port GPIOF
#define LCD_RST_Pin GPIO_PIN_4
#define LCD_RST_GPIO_Port GPIOA
#define LCD_CLK_Pin GPIO_PIN_5
#define LCD_CLK_GPIO_Port GPIOA
#define LCD_CE_Pin GPIO_PIN_6
#define LCD_CE_GPIO_Port GPIOA
#define LCD_DIN_Pin GPIO_PIN_7
#define LCD_DIN_GPIO_Port GPIOA
#define LCD_DC_Pin GPIO_PIN_4
#define LCD_DC_GPIO_Port GPIOC
#define ENV_DATA_Pin GPIO_PIN_12
#define ENV_DATA_GPIO_Port GPIOB
#define ENV_DATA_EXTI_IRQn EXTI15_10_IRQn
#define EXTKEYBOARD_PIN7_Pin GPIO_PIN_6
#define EXTKEYBOARD_PIN7_GPIO_Port GPIOD
#define EXTKEYBOARD_PIN6_Pin GPIO_PIN_9
#define EXTKEYBOARD_PIN6_GPIO_Port GPIOG
#define EXTKEYBOARD_PIN5_Pin GPIO_PIN_11
#define EXTKEYBOARD_PIN5_GPIO_Port GPIOG
#define EXTKEYBOARD_PIN4_Pin GPIO_PIN_13
#define EXTKEYBOARD_PIN4_GPIO_Port GPIOG
#define EXTKEYBOARD_PIN3_Pin GPIO_PIN_15
#define EXTKEYBOARD_PIN3_GPIO_Port GPIOG
#define EXTKEYBOARD_PIN3_EXTI_IRQn EXTI15_10_IRQn
#define EXTKEYBOARD_PIN2_Pin GPIO_PIN_4
#define EXTKEYBOARD_PIN2_GPIO_Port GPIOB
#define EXTKEYBOARD_PIN2_EXTI_IRQn EXTI4_IRQn
#define EXTKEYBOARD_PIN1_Pin GPIO_PIN_6
#define EXTKEYBOARD_PIN1_GPIO_Port GPIOB
#define EXTKEYBOARD_PIN1_EXTI_IRQn EXTI9_5_IRQn
#define EXTKEYBOARD_PIN0_Pin GPIO_PIN_8
#define EXTKEYBOARD_PIN0_GPIO_Port GPIOB
#define EXTKEYBOARD_PIN0_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
