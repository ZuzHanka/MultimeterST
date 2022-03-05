/*
 * bsp.cpp
 *
 *  Created on: 5 Dec 2020
 *      Author: ZuzHanka
 */

/* Includes ------------------------------------------------------------------*/

#include "bsp.hpp"

#include "main.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_ll_tim.h"

/* Presunut do BSP ---------------------------------------------------*/

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim3;
DAC_HandleTypeDef hdac1;
ADC_HandleTypeDef hadc;
DMA_HandleTypeDef hdma_adc;

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
		"VDDA"
		"TEMP",
};

const adc_conf_t adc_tester_channels[6] =
{
		{
				ADC_CHANNEL_1,
				ADC1_IN1_GPIO_Port,
				ADC1_IN1_Pin
		},
		{
				ADC_CHANNEL_2,
				ADC1_IN2_GPIO_Port,
				ADC1_IN2_Pin
		},
		{
				ADC_CHANNEL_7,
				ADC12_IN7_GPIO_Port,
				ADC12_IN7_Pin
		},
		{
				ADC_CHANNEL_6,
				ADC12_IN6_GPIO_Port,
				ADC12_IN6_Pin
		},
		{
				ADC_CHANNEL_VREFINT,
				nullptr,
				0
		},
		{
				ADC_CHANNEL_TEMPSENSOR,
				nullptr,
				0
		}
};

const adc_conf_t adc_slave_channels[3] =
{
		{
				ADC_CHANNEL_1,
				ADC1_IN1_GPIO_Port,
				ADC1_IN1_Pin
		},
		{
				ADC_CHANNEL_2,
				ADC1_IN2_GPIO_Port,
				ADC1_IN2_Pin
		},
		{
				ADC_CHANNEL_7,
				ADC12_IN7_GPIO_Port,
				ADC12_IN7_Pin
		}
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

void adc_init(ADC_TypeDef * adc, uint32_t adc_trigger, const adc_conf_t adc_conf[], size_t chan_count)
{
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12|RCC_PERIPHCLK_ADC34;
	PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
	PeriphClkInit.Adc34ClockSelection = RCC_ADC34PLLCLK_DIV1;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}

	/* Peripheral clock enable */
	__HAL_RCC_ADC12_CLK_ENABLE();


	/**ADC GPIO Configuration*/
    GPIO_InitTypeDef GPIO_InitStruct = {0};
	for (size_t i = 0; i < chan_count; ++i)
	{
		if (adc_conf[i].gpio_port != nullptr)
		{
			GPIO_InitStruct.Pin = adc_conf[i].gpio_pin;
			GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			HAL_GPIO_Init(adc_conf[i].gpio_port, &GPIO_InitStruct);
		}
	}

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Channel1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

	/* ADC DMA Init */
	hdma_adc.Instance = DMA1_Channel1;
	hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
	hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	hdma_adc.Init.Mode = DMA_CIRCULAR;
	hdma_adc.Init.Priority = DMA_PRIORITY_LOW;
	if (HAL_DMA_Init(&hdma_adc) != HAL_OK)
	{
		Error_Handler();
	}
	__HAL_LINKDMA(&hadc,DMA_Handle,hdma_adc);


    /** Common config
    */
    hadc.Instance = adc;
    hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    hadc.Init.Resolution = ADC_RESOLUTION_12B;
    hadc.Init.ScanConvMode = ADC_SCAN_ENABLE;
    hadc.Init.ContinuousConvMode = DISABLE;
    hadc.Init.DiscontinuousConvMode = DISABLE;
    hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISINGFALLING;
    hadc.Init.ExternalTrigConv = adc_trigger;
    hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc.Init.NbrOfConversion = chan_count;
    hadc.Init.DMAContinuousRequests = ENABLE;
    hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    hadc.Init.LowPowerAutoWait = DISABLE;
    hadc.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    if (HAL_ADC_Init(&hadc) != HAL_OK)
    {
      Error_Handler();
    }
    /** Configure the ADC multi-mode
    */
    ADC_MultiModeTypeDef multimode = {0};
    multimode.Mode = ADC_MODE_INDEPENDENT;
    if (HAL_ADCEx_MultiModeConfigChannel(&hadc, &multimode) != HAL_OK)
    {
      Error_Handler();
    }
    /** Configure Channels */
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.SamplingTime = ADC_SAMPLETIME_601CYCLES_5;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
	for (size_t i = 0; i < chan_count; ++i)
	{
	    sConfig.Channel = adc_conf[i].adc_channel;
	    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
	    {
	      Error_Handler();
	    }
	    sConfig.Rank++;
	}
}

bool adc_run(void)
{
	HAL_StatusTypeDef hal_status = HAL_OK;

	if (hal_status == HAL_OK)
	{
		hal_status = HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);
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
		hal_status = HAL_ADC_Start_DMA(&hadc, (uint32_t*) adc_buf, ADC_CHANNELS);
	}

	return hal_status == HAL_OK;
}

bool adc_start(void)
{
	HAL_StatusTypeDef hal_status = HAL_OK;

	if (hal_status == HAL_OK)
	{
		hal_status = HAL_ADC_Start_DMA(&hadc, (uint32_t*) adc_buf, ADC_CHANNELS);
	}

	return hal_status == HAL_OK;
}

bool adc_stop(void)
{
	HAL_StatusTypeDef hal_status = HAL_OK;

	if (hal_status == HAL_OK)
	{
		hal_status = HAL_ADC_Stop_DMA(&hadc);
//		HAL_ADC_DeInit(&hadc1);
	}

	return hal_status == HAL_OK;
}

void dac_init(void)
{
    /* Peripheral clock enable */
    __HAL_RCC_DAC1_CLK_ENABLE();

    /**DAC1 GPIO Configuration
    PA4     ------> DAC1_OUT1
    */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = ADC2_IN1_DAC1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADC2_IN1_DAC1_GPIO_Port, &GPIO_InitStruct);

    /** DAC Initialization
    */
    DAC_ChannelConfTypeDef sConfig = {0};
    hdac1.Instance = DAC1;
    if (HAL_DAC_Init(&hdac1) != HAL_OK)
    {
      Error_Handler();
    }
    /** DAC channel OUT1 config
    */
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
    {
      Error_Handler();
    }
}

void dac_deinit()
{
    /* Peripheral clock disable */
    __HAL_RCC_DAC1_CLK_DISABLE();

    /**DAC1 GPIO Configuration
    PA4     ------> DAC1_OUT1
    */
    HAL_GPIO_DeInit(ADC2_IN1_DAC1_GPIO_Port, ADC2_IN1_DAC1_Pin);
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

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* p_hadc)
{
	if (p_hadc == &hadc)
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

void set_switch(bool value)
{
	GPIO_PinState pin_state = value ? GPIO_PIN_SET : GPIO_PIN_RESET;
	HAL_GPIO_WritePin(GPIOA, SWITCH_Pin, pin_state);
}

bool get_switch(void)
{
	GPIO_PinState pin_state = HAL_GPIO_ReadPin(GPIOA, SWITCH_Pin);
	return pin_state == GPIO_PIN_SET;
}

void delay(uint32_t time_ms)
{
	HAL_Delay(time_ms);
}
