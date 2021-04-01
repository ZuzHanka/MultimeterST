/*
 * bsp.cpp
 *
 *  Created on: 5 Dec 2020
 *      Author: ZuzHanka
 */

/* Includes ------------------------------------------------------------------*/

#include "bsp.hpp"

#include "stm32f3xx_hal.h"

/* Presunut do BSP ---------------------------------------------------*/

extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim8;


/* Constants ---------------------------------------------------------*/

// timeout for one character receive
static const uint32_t TIMEOUT = 10;

const char * adc_ch_names[CHANNEL_COUNT] =
{
		"A0",
		"A1",
		"TEMP",
		"VDDA",
		"A2",
		"A3"
};

/* Variables ---------------------------------------------------------*/

// ADC ring buffer (1 sample for each channel)
uint16_t adc_buf[CHANNEL_COUNT];

/* Functions ---------------------------------------------------------*/

uint32_t get_tick(void)
{
	return HAL_GetTick();
}

size_t terminal_receive(char * buff, size_t buff_size)
{
	size_t len = 0;
	while (buff_size > 0)
	{
		HAL_StatusTypeDef status = HAL_UART_Receive(&huart2, (uint8_t*) buff, 1, TIMEOUT);
		// HAL_UART_Transmit(&huart2, buff, 1, 10);
		if (status == HAL_OK)
		{
			buff++;
			buff_size--;
			len++;
		}
		else
		{
			break;
		}
	}
	return len;
}

bool terminal_transmit(const char * buff, size_t buff_size)
{
	return HAL_OK == HAL_UART_Transmit(&huart2, (uint8_t*) buff, buff_size, TIMEOUT * buff_size);
}

bool adc_run(void)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
	if (hal_status == HAL_OK)
	{
		hal_status = HAL_TIM_Base_Start(&htim1);
	}
	if (hal_status == HAL_OK)
	{
		hal_status = HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_3);
	}	
	if (hal_status == HAL_OK)
	{
		hal_status = HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &adc_buf[ADC1_IDX], ADC1_CHANNELS);
	}
	if (hal_status == HAL_OK)
	{
		hal_status = HAL_ADC_Start_DMA(&hadc2, (uint32_t*) &adc_buf[ADC2_IDX], ADC2_CHANNELS);
	}
	if (hal_status == HAL_OK)
	{
		hal_status = HAL_ADC_Start_DMA(&hadc3, (uint32_t*) &adc_buf[ADC3_IDX], ADC3_CHANNELS);
	}

	return hal_status == HAL_OK;
}

bool pwm_run(void)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
	if (hal_status == HAL_OK)
	{
		hal_status = HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
	}
	if (hal_status == HAL_OK)
	{
		hal_status = HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
	}

	return hal_status == HAL_OK;

}


uint16_t adc_get_sample_mV(uint8_t channel)
{
	volatile const uint16_t * p_VREFINT_CAL = (uint16_t*) 0x1FFFF7BA;

	// Vdda = 3.3 V * VREFINT_CAL / VREFINT_DATA
	uint64_t numerator = 3300ULL * (*p_VREFINT_CAL);
	uint32_t denominator = adc_buf[CHANNEL_VREFINT];

	// VCHANNELx = Vdda * ADCx_DATA / FULL_SCALE
	if (channel != CHANNEL_VDDA)
	{
		numerator = numerator * adc_buf[channel];
		uint32_t full_scale = (1UL << ADC_BITS) - 1;
		denominator = denominator * full_scale;
	}

	uint16_t voltage = numerator / denominator;

	return voltage;
}

float adc_mV_to_Celsius(int16_t value_mV)
{
	float out_value = ((float) (V25 - value_mV) / AVG_SLOPE) + 25.0;
	return out_value;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc == &hadc1)
	{
		adc_callback();
	}
}
