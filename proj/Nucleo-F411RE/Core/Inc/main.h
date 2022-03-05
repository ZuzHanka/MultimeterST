/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define ADC1_IN10_Pin GPIO_PIN_0
#define ADC1_IN10_GPIO_Port GPIOC
#define ADC1_IN11_Pin GPIO_PIN_1
#define ADC1_IN11_GPIO_Port GPIOC
#define ADC1_IN12_Pin GPIO_PIN_2
#define ADC1_IN12_GPIO_Port GPIOC
#define ADC1_IN13_Pin GPIO_PIN_3
#define ADC1_IN13_GPIO_Port GPIOC
#define ADC1_IN0_Pin GPIO_PIN_0
#define ADC1_IN0_GPIO_Port GPIOA
#define ADC1_IN1_Pin GPIO_PIN_1
#define ADC1_IN1_GPIO_Port GPIOA
#define ADC1_IN4_Pin GPIO_PIN_4
#define ADC1_IN4_GPIO_Port GPIOA
#define ADC1_IN6_Pin GPIO_PIN_6
#define ADC1_IN6_GPIO_Port GPIOA
#define ADC1_IN7_Pin GPIO_PIN_7
#define ADC1_IN7_GPIO_Port GPIOA
#define ADC1_IN14_Pin GPIO_PIN_4
#define ADC1_IN14_GPIO_Port GPIOC
#define ADC1_IN15_Pin GPIO_PIN_5
#define ADC1_IN15_GPIO_Port GPIOC
#define ADC1_IN8_Pin GPIO_PIN_0
#define ADC1_IN8_GPIO_Port GPIOB
#define ADC1_IN9_Pin GPIO_PIN_1
#define ADC1_IN9_GPIO_Port GPIOB
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
