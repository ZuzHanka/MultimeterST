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
#include "stm32l4xx_hal.h"

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
/* USER CODE BEGIN Private defines */
#define ADC123_IN1_Pin GPIO_PIN_0
#define ADC123_IN1_GPIO_Port GPIOC
#define ADC123_IN2_Pin GPIO_PIN_1
#define ADC123_IN2_GPIO_Port GPIOC
#define ADC123_IN3_Pin GPIO_PIN_2
#define ADC123_IN3_GPIO_Port GPIOC
#define ADC123_IN4_Pin GPIO_PIN_3
#define ADC123_IN4_GPIO_Port GPIOC
#define ADC12_IN5_Pin GPIO_PIN_0
#define ADC12_IN5_GPIO_Port GPIOA
#define ADC12_IN6_Pin GPIO_PIN_1
#define ADC12_IN6_GPIO_Port GPIOA
#define ADC12_IN9_Pin GPIO_PIN_4
#define ADC12_IN9_GPIO_Port GPIOA
#define ADC12_IN11_Pin GPIO_PIN_6
#define ADC12_IN11_GPIO_Port GPIOA
#define ADC12_IN12_Pin GPIO_PIN_7
#define ADC12_IN12_GPIO_Port GPIOA
#define ADC12_IN13_Pin GPIO_PIN_4
#define ADC12_IN13_GPIO_Port GPIOC
#define ADC12_IN14_Pin GPIO_PIN_5
#define ADC12_IN14_GPIO_Port GPIOC
#define ADC12_IN15_Pin GPIO_PIN_0
#define ADC12_IN15_GPIO_Port GPIOB
#define ADC12_IN16_Pin GPIO_PIN_1
#define ADC12_IN16_GPIO_Port GPIOB
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
