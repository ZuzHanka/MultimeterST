/* Includes ------------------------------------------------------------------*/

#include "bsp.hpp"

extern "C"
{
#include "main.h"
#include "stm32g0xx_hal.h"
}

/* Presunut do BSP ---------------------------------------------------*/

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim1;
ADC_HandleTypeDef hadc;
DMA_HandleTypeDef hdma_adc;

/* Constants ---------------------------------------------------------*/

// timeout for one character receive
static const uint32_t TIMEOUT = 10;

// printed voltmeter channel names
const char * adc_ch_names[CHANNEL_COUNT] =
{
		"PA1",
		"PB0",
		"PC1",
		"PC0",
		"TEMP",
		"VDDA"
};

const adc_conf_t adc_tester_channels[6] =
{
		{
				ADC_CHANNEL_6,
				ADC1_IN6_GPIO_Port,
				ADC1_IN6_Pin
		},
		{
				ADC_CHANNEL_8,
				ADC1_IN8_GPIO_Port,
				ADC1_IN8_Pin
		},
		{
				ADC_CHANNEL_9,
				ADC1_IN9_GPIO_Port,
				ADC1_IN9_Pin
		},
		{
				ADC_CHANNEL_10,
				ADC1_IN10_GPIO_Port,
				ADC1_IN10_Pin
		},
		{
				ADC_CHANNEL_TEMPSENSOR,
				nullptr,
				0
		},
		{
				ADC_CHANNEL_VREFINT,
				nullptr,
				0
		}
};

const adc_conf_t adc_slave_channels[3] =
{
		{
				ADC_CHANNEL_9,
				ADC1_IN9_GPIO_Port,
				ADC1_IN9_Pin
		},
		{
				ADC_CHANNEL_8,
				ADC1_IN8_GPIO_Port,
				ADC1_IN8_Pin
		},
		{
				ADC_CHANNEL_10,
				ADC1_IN10_GPIO_Port,
				ADC1_IN10_Pin
		}
};

// printed PWM channel names
// TODO: fix values for this MCU
const char * pwm_ch_names[CHANNEL_PWM_COUNT] =
{
		"D10",
		"D12"
};

// dummy: printed DAC channel names
const char ** dac_ch_names = nullptr;

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

static const uint32_t adc_ranks[16] =
		{
				ADC_REGULAR_RANK_1,
				ADC_REGULAR_RANK_2,
				ADC_REGULAR_RANK_3,
				ADC_REGULAR_RANK_4,
				ADC_REGULAR_RANK_5,
				ADC_REGULAR_RANK_6,
				ADC_REGULAR_RANK_7,
				ADC_REGULAR_RANK_8
		};

void adc_init(ADC_TypeDef * adc, const adc_conf_t adc_conf[], size_t chan_count)
{
	// Max ADC clock: 35MHz
	// Max sampling rate: 2.5MHz
	// 12bit approximation: 12.5 ticks
	//
	// Current setting:
	//   SYS CLK:		64MHz
	//   ADC DIV: 		2
	//   ADC CLK: 		32MHz
	//   ADC Sample:	160.5

	/** Initializes the peripherals clocks */
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK; // 64MHz
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}

	/* Peripheral clock enable */
	__HAL_RCC_ADC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

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
	hdma_adc.Init.Request = DMA_REQUEST_ADC1;
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

    /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    */
    hadc.Instance = adc;
    hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc.Init.Resolution = ADC_RESOLUTION_12B;
    hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc.Init.ScanConvMode = ADC_SCAN_ENABLE;
    hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    hadc.Init.LowPowerAutoWait = DISABLE;
    hadc.Init.LowPowerAutoPowerOff = DISABLE;
    hadc.Init.ContinuousConvMode = DISABLE;
    hadc.Init.NbrOfConversion = chan_count;
    hadc.Init.DiscontinuousConvMode = DISABLE;
    hadc.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T1_CC4;
    hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISINGFALLING;
    hadc.Init.DMAContinuousRequests = ENABLE;
    hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    hadc.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_160CYCLES_5;
    hadc.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_160CYCLES_5;
    hadc.Init.OversamplingMode = DISABLE;
    hadc.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
    if (HAL_ADC_Init(&hadc) != HAL_OK)
    {
    	Error_Handler();
    }

    /** Configure Channels */
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
	for (size_t i = 0; i < chan_count; ++i)
	{
	    sConfig.Rank = adc_ranks[i];
	    sConfig.Channel = adc_conf[i].adc_channel;
	    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
	    {
	      Error_Handler();
	    }
	    sConfig.Rank++;
	}
}

bool adc_run(uint32_t period_us)
{
	HAL_StatusTypeDef hal_status = HAL_OK;

	if (hal_status == HAL_OK)
	{
		hal_status = HAL_ADCEx_Calibration_Start(&hadc);
	}
	__HAL_TIM_SET_AUTORELOAD(&htim1, (period_us - 1));
	if (hal_status == HAL_OK)
	{
		hal_status = HAL_TIM_Base_Start(&htim1);
	}
	if (hal_status == HAL_OK)
	{
		hal_status = HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_4);
	}
	if (hal_status == HAL_OK)
	{
		hal_status = HAL_ADC_Start_DMA(&hadc, (uint32_t*) adc_buf, CHANNEL_COUNT);
	}

	return hal_status == HAL_OK;
}

void adc_trigger_stop()
{
	(void) HAL_TIM_Base_Stop(&htim1);
}

void dac_init(void)
{
	// Dummy - no DAC.
}

bool dac_run(void)
{
	// Dummy - no DAC.
	return true;
}

// TODO: implementation for this MCU
bool pwm_run(void)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
//	if (hal_status == HAL_OK)
//	{
//		hal_status = HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
//	}
//	if (hal_status == HAL_OK)
//	{
//		hal_status = HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
//	}

	return hal_status == HAL_OK;
}

// TODO: implementation for this MCU
void pwm_synchronize()
{
	// Dummy.
}

uint16_t adc_get_sample_mV(uint8_t channel)
{
	volatile const uint16_t * p_VREFINT_CAL = (uint16_t*) 0x1FFF75AA;

	// Vdda = 3.3 V * VREFINT_CAL / VREFINT_DATA
	uint64_t numerator = 3000ULL * (*p_VREFINT_CAL);
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
// TODO: fix temperature calculations
/*
	volatile const uint16_t * p_TS_CAL1 = (uint16_t*) 0x1FFF75A8;
	volatile const uint16_t * p_TS_CAL2 = (uint16_t*) 0x1FFF75CA;
	const uint32_t TS_CAL1_TEMP = 30;
	const uint32_t TS_CAL2_TEMP = 130;

	//  				    TS_CAL2_TEMP - TS_CAL1_TEMP
	// Temperature in °C = ----------------------------- x ( value - TS_CAL1) + 30°C
	//                           TS_CAL2 - TS_CAL1

	uint64_t numerator = (TS_CAL2_TEMP - TS_CAL1_TEMP) * (value_mV - p_TS_CAL1);
	uint32_t denominator = *p_TS_CAL2 - *p_TS_CAL2;
 */

	float out_value = ((float) (value_mV - V25) / AVG_SLOPE) + 25.0;
	return out_value;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* p_hadc)
{
	if (p_hadc == &hadc)
	{
		adc_callback();
	}
}

// TODO: implementation for this MCU
float pwm_get_duty(uint32_t channel)
{
	uint32_t duty = 0;
//	if (channel == CHANNEL_PWM1)
//	{
//		duty = LL_TIM_OC_GetCompareCH1(htim8.Instance);
//	}
//	if (channel == CHANNEL_PWM2)
//	{
//		duty =  LL_TIM_OC_GetCompareCH1(htim3.Instance);
//	}
	return duty;
}

// TODO: implementation for this MCU
void pwm_set_duty(uint32_t channel, uint32_t duty)
{
//	if (channel == CHANNEL_PWM1)
//	{
//		LL_TIM_OC_SetCompareCH1(htim8.Instance, duty);
//	}
//	if (channel == CHANNEL_PWM2)
//	{
//		LL_TIM_OC_SetCompareCH1(htim3.Instance, duty);
//	}
}

// TODO: implementation for this MCU
void pwm_duty_increment(uint32_t channel, int32_t increment_promile)
{
	(void) channel;
	(void) increment_promile;
}

// TODO: implementation for this MCU
float pwm_get_freq(uint32_t channel)
{
	// uint32_t LL_TIM_GetPrescaler(TIM_TypeDef *TIMx)
	uint32_t freq = 0;
//	if (channel == CHANNEL_PWM1)
//	{
//		freq =  (LL_TIM_GetPrescaler(htim8.Instance) + 1);
//	}
//	if (channel == CHANNEL_PWM2)
//	{
//		freq =  (LL_TIM_GetPrescaler(htim3.Instance) + 1);
//	}
	return freq;
}

// TODO: implementation for this MCU
void pwm_freq_increment(uint32_t channel, int32_t increment_promile)
{
	(void) channel;
	(void) increment_promile;
}

// TODO: implementation for this MCU
void pwm_set_freq(uint32_t channel, uint32_t freq)
{
	// void LL_TIM_SetPrescaler(TIM_TypeDef *TIMx, uint32_t Prescaler)
//	if (channel == CHANNEL_PWM1)
//	{
//		LL_TIM_SetPrescaler(htim8.Instance, (freq - 1));
//	}
//	if (channel == CHANNEL_PWM2)
//	{
//		LL_TIM_SetPrescaler(htim3.Instance, (freq - 1));
//	}
}

uint32_t dac_get_value(void)
{
	// Dummy - no DAC.
	return 0;
}

void dac_set_value(uint16_t value_mV)
{
	// Dummy - no DAC.
	(void) value_mV;
}

void set_switch(bool value)
{
	// Dummy - no tester mode.
	(void) value;
}

bool get_switch(void)
{
	// Dummy - no tester mode.
	return false;
}

void delay(uint32_t time_ms)
{
	HAL_Delay(time_ms);
}
