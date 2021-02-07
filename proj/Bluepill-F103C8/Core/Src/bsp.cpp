/*
 * bsp.cpp
 *
 *  Created on: 5 Dec 2020
 *      Author: ZuzHanka
 */

/* Includes ------------------------------------------------------------------*/

#include "bsp.hpp"

#include "stm32f1xx_hal.h"
#include "usbd_cdc_if.h"

/* Presunut do BSP ---------------------------------------------------*/

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim1;
extern UART_HandleTypeDef huart1;

/* Constants ---------------------------------------------------------*/

// timeout for one character receive
static const uint32_t TIMEOUT = 10;

/* Variables ---------------------------------------------------------*/

// ADC ring buffer (1 sample for each channel)
uint16_t adc_buf[CHANNEL_COUNT];

/* Functions ---------------------------------------------------------*/

size_t terminal_receive(char * buff, size_t buff_size)
{
	size_t len = 0;
	while (buff_size > 0)
	{
		HAL_StatusTypeDef status = HAL_UART_Receive(&huart1, (uint8_t*) buff, 1, TIMEOUT);
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
	(void) CDC_Transmit_FS((uint8_t*) buff, buff_size);
	return HAL_OK == HAL_UART_Transmit(&huart1, (uint8_t*) buff, buff_size, TIMEOUT * buff_size);
}

bool adc_run(void)
{
	terminal_transmit("start\n", 6);

	HAL_StatusTypeDef hal_status = HAL_TIM_Base_Start(&htim1);
	if (hal_status == HAL_OK)
	{
		hal_status = HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);
	}
	if (hal_status == HAL_OK)
	{
		hal_status = HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adc_buf, CHANNEL_COUNT);
	}

	return hal_status == HAL_OK;
}

uint16_t adc_get_sample_mV(uint8_t channel)
{
	// VREFINT == 1.2V (see datasheet)
	uint64_t numerator = 1200ULL;
	uint32_t denominator = adc_buf[CHANNEL_VREFINT];

	// Vdda = 1.2V * FULL_SCALE / VREFINT_DATA
	if (channel == CHANNEL_VDDA)
	{
		uint32_t full_scale = (1UL << ADC_BITS) - 1;
		numerator = numerator * full_scale;
	}
	// VCHANNELx = Vdda * ADCx_DATA / FULL_SCALE = 1.2V * ADCx_DATA / VREFINT_DATA
	else
	{
		numerator = numerator * adc_buf[channel];
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
	(void) hadc;
	adc_callback();
}

extern "C" void CDC_ReceiveCallback(uint8_t * buffer, uint32_t length)
{

}
