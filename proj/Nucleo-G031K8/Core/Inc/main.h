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
#include "stm32g0xx_hal.h"

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
#define T_NRST_Pin GPIO_PIN_2
#define T_NRST_GPIO_Port GPIOF
#define T_VCP_TX_Pin GPIO_PIN_2
#define T_VCP_TX_GPIO_Port GPIOA
#define T_VCP_RX_Pin GPIO_PIN_3
#define T_VCP_RX_GPIO_Port GPIOA
#define LD3_Pin GPIO_PIN_6
#define LD3_GPIO_Port GPIOC
#define T_JTMS_Pin GPIO_PIN_13
#define T_JTMS_GPIO_Port GPIOA
#define T_JTCK_Pin GPIO_PIN_14
#define T_JTCK_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */
#define ADC1_IN0_Pin GPIO_PIN_0
#define ADC1_IN0_GPIO_Port GPIOA
#define ADC1_IN1_Pin GPIO_PIN_1
#define ADC1_IN1_GPIO_Port GPIOA
#define ADC1_IN4_Pin GPIO_PIN_4
#define ADC1_IN4_GPIO_Port GPIOA
#define ADC1_IN5_Pin GPIO_PIN_5
#define ADC1_IN5_GPIO_Port GPIOA
#define ADC1_IN6_Pin GPIO_PIN_6
#define ADC1_IN6_GPIO_Port GPIOA
#define ADC1_IN7_Pin GPIO_PIN_7
#define ADC1_IN7_GPIO_Port GPIOA
#define ADC1_IN8_Pin GPIO_PIN_0
#define ADC1_IN8_GPIO_Port GPIOB
#define ADC1_IN9_Pin GPIO_PIN_1
#define ADC1_IN9_GPIO_Port GPIOB
#define ADC1_IN10_Pin GPIO_PIN_2
#define ADC1_IN10_GPIO_Port GPIOB
#define ADC1_IN11_Pin GPIO_PIN_7
#define ADC1_IN11_GPIO_Port GPIOB
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
