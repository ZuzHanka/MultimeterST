/* Includes ------------------------------------------------------------------*/

#include "bsp.hpp"

extern "C"
{
#include "main.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_ll_tim.h"
}

/* Presunut do BSP ---------------------------------------------------*/

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
DAC_HandleTypeDef hdac1;
ADC_HandleTypeDef hadc;
DMA_HandleTypeDef hdma_adc;

TIM_HandleTypeDef * const pwm_htim[CHANNEL_PWM_COUNT] = { &htim3, &htim4 };
float pwm_duty[CHANNEL_PWM_COUNT] = { 0.0, 0.0 };

/* Constants ---------------------------------------------------------*/

// timeout for one character receive
static const uint32_t TIMEOUT = 10;

// printed voltmeter channel names
const char * adc_ch_names[CHANNEL_COUNT] =
{
		"PC0",
		"PC1",
		"PC2",
		"PC3",
		"TEMP",
		"VDDA"
};

const adc_conf_t adc_tester_channels[6] =
{
		{
				ADC_CHANNEL_6,
				ADC12_IN6_GPIO_Port,
				ADC12_IN6_Pin
		},
		{
				ADC_CHANNEL_7,
				ADC12_IN7_GPIO_Port,
				ADC12_IN7_Pin
		},
		{
				ADC_CHANNEL_8,
				ADC12_IN8_GPIO_Port,
				ADC12_IN8_Pin
		},
		{
				ADC_CHANNEL_9,
				ADC12_IN9_GPIO_Port,
				ADC12_IN9_Pin
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
				ADC_CHANNEL_8,
				ADC12_IN8_GPIO_Port,
				ADC12_IN8_Pin
		}
};

// printed PWM channel names
const char * pwm_ch_names[CHANNEL_PWM_COUNT] =
{
		"D5", // PB4
		"D10" // PB6
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

static const uint32_t adc_ranks[16] =
		{
				ADC_REGULAR_RANK_1,
				ADC_REGULAR_RANK_2,
				ADC_REGULAR_RANK_3,
				ADC_REGULAR_RANK_4,
				ADC_REGULAR_RANK_5,
				ADC_REGULAR_RANK_6,
				ADC_REGULAR_RANK_7,
				ADC_REGULAR_RANK_8,
				ADC_REGULAR_RANK_9,
				ADC_REGULAR_RANK_10,
				ADC_REGULAR_RANK_11,
				ADC_REGULAR_RANK_12,
				ADC_REGULAR_RANK_13,
				ADC_REGULAR_RANK_14,
				ADC_REGULAR_RANK_15,
				ADC_REGULAR_RANK_16
		};

void adc_init(ADC_TypeDef * adc, const adc_conf_t adc_conf[], size_t chan_count)
{
	// Max ADC clock: 72MHz
	// Max sampling rate: 5.14MHz (fast channel), 4.8MHz (slow channel)
	// 12bit approximation: 12.5 ticks

	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
	if ((adc == ADC1) || (adc == ADC2))
	{
	    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
	    PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1; // 72MHz
	}
	else
	{
	    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC34;
	    PeriphClkInit.Adc34ClockSelection = RCC_ADC34PLLCLK_DIV1; // 72MHz
	}
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}

	/* Peripheral clock enable */
	if ((adc == ADC1) || (adc == ADC2))
	{
	    __HAL_RCC_ADC12_CLK_ENABLE();
	}
	else
	{
	    __HAL_RCC_ADC34_CLK_ENABLE();
	}
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

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
	/* DMA interrupt init */
	/* ADC DMA Init */
	if (adc == ADC1)
	{
		__HAL_RCC_DMA1_CLK_ENABLE();
		HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
		hdma_adc.Instance = DMA1_Channel1;
	}
	else if (adc == ADC2)
	{
		__HAL_RCC_DMA2_CLK_ENABLE();
		HAL_NVIC_SetPriority(DMA2_Channel1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA2_Channel1_IRQn);
		hdma_adc.Instance = DMA2_Channel1;
	}
	else if (adc == ADC3)
	{
		__HAL_RCC_DMA2_CLK_ENABLE();
		HAL_NVIC_SetPriority(DMA2_Channel5_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA2_Channel5_IRQn);
		hdma_adc.Instance = DMA2_Channel5;
	}
	else if (adc == ADC4)
	{
		__HAL_RCC_DMA2_CLK_ENABLE();
		HAL_NVIC_SetPriority(DMA2_Channel3_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA2_Channel2_IRQn);
		hdma_adc.Instance = DMA2_Channel2;
	}
	else
	{
		Error_Handler();
	}
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
    hadc.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC3;
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
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.SamplingTime = ADC_SAMPLETIME_601CYCLES_5;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
	for (size_t i = 0; i < chan_count; ++i)
	{
	    sConfig.Rank = adc_ranks[i];
	    sConfig.Channel = adc_conf[i].adc_channel;
	    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
	    {
	      Error_Handler();
	    }
	}
}

bool adc_run(uint32_t period_us)
{
	HAL_StatusTypeDef hal_status = HAL_OK;

	if (hal_status == HAL_OK)
	{
		hal_status = HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);
	}
	__HAL_TIM_SET_AUTORELOAD(&htim1, (period_us - 1));
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

void adc_trigger_stop()
{
	(void) HAL_TIM_Base_Stop(&htim1);
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
		hal_status = HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	}
	if (hal_status == HAL_OK)
	{
		hal_status = HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
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

float pwm_get_duty(uint32_t channel)
{
	uint32_t duty = LL_TIM_OC_GetCompareCH1(pwm_htim[channel]->Instance);
	uint32_t period = LL_TIM_GetAutoReload(pwm_htim[channel]->Instance);

	float f_duty = (float) (duty * 100) / (float) (period + 1);
	if (f_duty > 100)
	{
		f_duty = 100;
	}
	return f_duty;
}

void pwm_set_duty(uint32_t channel, uint32_t duty)
{
	uint32_t period = LL_TIM_GetAutoReload(pwm_htim[channel]->Instance);

	pwm_duty[channel] = (float) duty / 100.0;

	uint32_t round = 50;
	duty = (duty * (period + 1) + round) / 100;
	if (duty > UINT16_MAX)
	{
		duty = UINT16_MAX;
	}

	LL_TIM_OC_SetCompareCH1(pwm_htim[channel]->Instance, duty);
}

void pwm_duty_increment(uint32_t channel, int32_t increment_promile)
{
	int32_t duty = LL_TIM_OC_GetCompareCH1(pwm_htim[channel]->Instance);
	uint32_t period = LL_TIM_GetAutoReload(pwm_htim[channel]->Instance);

	int32_t increment = ((int32_t) (period + 1) * increment_promile) / 1000;
	if (increment == 0)
	{
		// Always increment at least minimal possible step.
		if (increment_promile > 0)
		{
			increment = 1;
		}
		else
		{
			increment = -1;
		}
	}

	duty += increment;
	if (duty > UINT16_MAX)
	{
		duty = UINT16_MAX;
	}
	else if (duty < 0)
	{
		duty = 0;
	}

	pwm_duty[channel] = (float) duty / (float) (period + 1);

	LL_TIM_OC_SetCompareCH1(pwm_htim[channel]->Instance, duty);
}

float pwm_get_freq(uint32_t channel)
{
	// uint32_t LL_TIM_GetPrescaler(TIM_TypeDef *TIMx)
	// uint32_t LL_TIM_GetAutoReload(TIM_TypeDef *TIMx)

	uint32_t prescaler = 0;
	uint32_t period = 0;

	prescaler = LL_TIM_GetPrescaler(pwm_htim[channel]->Instance);
	period = LL_TIM_GetAutoReload(pwm_htim[channel]->Instance);

	float freq = (float) TIMER_BASE_FREQUENCY_MAX / (float) ((prescaler + 1) * (period + 1));

	return freq;
}

void pwm_set_freq(uint32_t channel, uint32_t freq_Hz)
{
	// void LL_TIM_SetPrescaler(TIM_TypeDef *TIMx, uint32_t Prescaler)
	// void LL_TIM_SetAutoReload(TIM_TypeDef *TIMx, uint32_t AutoReload)

	uint32_t prescaler = (TIMER_BASE_FREQUENCY_MAX >> 16) / freq_Hz;  // = factor / (2^16 * freq)
	if (prescaler > UINT16_MAX)
	{
		prescaler = UINT16_MAX;
	}

	uint32_t presc_freq = freq_Hz * (prescaler + 1);
	uint32_t round = presc_freq / 2;
	uint32_t period = (TIMER_BASE_FREQUENCY_MAX + round) / presc_freq;
	if (period > UINT16_MAX)
	{
		period = UINT16_MAX;
	}
	else if (period <= 1)
	{
		period = 1;
	}
	else
	{
		period -= 1;
	}

	uint32_t duty = pwm_duty[channel] * (period + 1);
	if (duty > UINT16_MAX)
	{
		duty = UINT16_MAX;
	}

	LL_TIM_OC_SetCompareCH1(pwm_htim[channel]->Instance, duty);
	LL_TIM_SetPrescaler(pwm_htim[channel]->Instance, prescaler);
	LL_TIM_SetAutoReload(pwm_htim[channel]->Instance, period);
}

void pwm_freq_increment(uint32_t channel, int32_t increment_promile)
{
	// void LL_TIM_SetPrescaler(TIM_TypeDef *TIMx, uint32_t Prescaler)
	// void LL_TIM_SetAutoReload(TIM_TypeDef *TIMx, uint32_t AutoReload)

	uint32_t period = LL_TIM_GetAutoReload(pwm_htim[channel]->Instance);
	uint32_t prescaler = LL_TIM_GetPrescaler(pwm_htim[channel]->Instance);
	int64_t factor = (period + 1) * (prescaler + 1);

	int64_t factor_inc = (factor * increment_promile) / 1000;
	if (factor_inc == 0)
	{
		// Always increment at least minimal possible step.
		if (increment_promile > 0)
		{
			factor_inc = 1;
		}
		else
		{
			factor_inc = -1;
		}
	}
	// Note: increment frequency => decrement period.
	factor -= factor_inc;

	prescaler = ((uint32_t) factor) >> 16; // = factor / 2^16
	if (prescaler > UINT16_MAX)
	{
		prescaler = UINT16_MAX;
	}

	period = factor / (prescaler + 1);
	if (period > UINT16_MAX)
	{
		period = UINT16_MAX;
	}
	else if (period <= 1)
	{
		period = 1;
	}
	else
	{
		period -= 1;
	}

	uint32_t duty = pwm_duty[channel] * (period + 1);
	if (duty > UINT16_MAX)
	{
		duty = UINT16_MAX;
	}

	LL_TIM_OC_SetCompareCH1(pwm_htim[channel]->Instance, duty);
	LL_TIM_SetPrescaler(pwm_htim[channel]->Instance, prescaler);
	LL_TIM_SetAutoReload(pwm_htim[channel]->Instance, period);
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
