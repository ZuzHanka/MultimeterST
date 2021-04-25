/*
 * bsp.cpp
 *
 *  Created on: 5 Dec 2020
 *      Author: ZuzHanka
 */

/* Includes ------------------------------------------------------------------*/

#include "bsp.hpp"

#include "stm32f3xx_hal.h"
#include "stm32f3xx_ll_tim.h"

/* Presunut do BSP ---------------------------------------------------*/

extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim3;
extern DAC_HandleTypeDef hdac1;


/* Constants ---------------------------------------------------------*/

// timeout for one character receive
static const uint32_t TIMEOUT = 10;

// printed voltmeter channel names
const char * adc_ch_names[CHANNEL_COUNT] =
{
		"A0",
		"A1",
		"A4",
		"A5",
		"TEMP",
		"VDDA"
};

// printed PWM channel names
const char * pwm_ch_names[CHANNEL_PWM_COUNT] =
{
		"D10",
		"D12"
};

// printed DAC channel names
const char * dac_ch_names[CHANNEL_DAC_COUNT] =
{
		"A2"
};

/* Variables ---------------------------------------------------------*/

// ADC ring buffer (1 sample for each channel)
uint16_t adc_buf[CHANNEL_COUNT];

uint16_t adc_vdda_mV = 3300;


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
		hal_status = HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	}
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
		hal_status = HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adc_buf, ADC_CHANNELS);
	}

	return hal_status == HAL_OK;
}

bool dac_run(void)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
	if (hal_status == HAL_OK)
	{
		hal_status = HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
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
		hal_status = HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
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

	if (channel == CHANNEL_VDDA)
	{
		adc_vdda_mV = voltage;
	}

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

uint32_t pwm_get_duty(uint32_t channel)
{
	uint32_t duty = 0;
	if (channel == CHANNEL_PWM1)
	{
		duty = LL_TIM_OC_GetCompareCH1(htim8.Instance);
	}
	if (channel == CHANNEL_PWM2)
	{
		duty =  LL_TIM_OC_GetCompareCH1(htim3.Instance);
	}
	return duty;
}

void pwm_set_duty(uint32_t channel, uint32_t duty)
{
	if (channel == CHANNEL_PWM1)
	{
		LL_TIM_OC_SetCompareCH1(htim8.Instance, duty);
	}
	if (channel == CHANNEL_PWM2)
	{
		LL_TIM_OC_SetCompareCH1(htim3.Instance, duty);
	}
}

uint32_t pwm_get_freq(uint32_t channel)
{
	// uint32_t LL_TIM_GetPrescaler(TIM_TypeDef *TIMx)
	uint32_t freq = 0;
	if (channel == CHANNEL_PWM1)
	{
		freq =  (LL_TIM_GetPrescaler(htim8.Instance) + 1);
	}
	if (channel == CHANNEL_PWM2)
	{
		freq =  (LL_TIM_GetPrescaler(htim3.Instance) + 1);
	}
	return freq;
}

void pwm_set_freq(uint32_t channel, uint32_t freq)
{
	// void LL_TIM_SetPrescaler(TIM_TypeDef *TIMx, uint32_t Prescaler)
	if (channel == CHANNEL_PWM1)
	{
		LL_TIM_SetPrescaler(htim8.Instance, (freq - 1));
	}
	if (channel == CHANNEL_PWM2)
	{
		LL_TIM_SetPrescaler(htim3.Instance, (freq - 1));
	}
}

uint32_t dac_get_value(void)
{
	return HAL_DAC_GetValue(&hdac1, CHANNEL_DAC1);
}

void dac_set_value(uint16_t value_mV)
{
	uint32_t value = (4096UL * value_mV) / adc_vdda_mV;
	HAL_DAC_SetValue(&hdac1, CHANNEL_DAC1, DAC_ALIGN_12B_R, value);
}
