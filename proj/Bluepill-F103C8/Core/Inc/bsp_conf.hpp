#ifndef INC_BSP_CONF_HPP_
#define INC_BSP_CONF_HPP_

extern "C"
{
#include "main.h"
}

#include <cstdint>

const char board_name[30] = "Bluepill-F103C8";

// ADC channels
enum Channel {
	CHANNEL_1,
	CHANNEL_2,
	CHANNEL_3,
	CHANNEL_4,
	CHANNEL_TEMP,
	CHANNEL_VREFINT,
	CHANNEL_COUNT,
	ADC_CHANNELS = CHANNEL_COUNT,		// Number of active channels.
	CHANNEL_VDDA = CHANNEL_VREFINT		// Return Vdda instead of Vrefint value.
};

static constexpr uint8_t Channel_ordered[] =
{
		CHANNEL_1,
		CHANNEL_2,
		CHANNEL_3,
		CHANNEL_4
};

extern const char * adc_ch_names[CHANNEL_COUNT];

struct adc_conf_t
{
	uint32_t adc_channel;
	GPIO_TypeDef * gpio_port;
	uint16_t gpio_pin;
};

#define ADC_CONF_LENGTH(adc_conf) (sizeof(adc_conf) / sizeof(adc_conf_t))

extern const adc_conf_t adc_tester_channels[6];

extern const adc_conf_t adc_slave_channels[3];

// ADC bits
static const uint8_t ADC_BITS = 12;
// temperature sensor average slope [mV/*C]
static const float AVG_SLOPE = 4.3;
// temperature sensor voltage at 25*C [mV]
static const uint16_t V25 = 1430;

// PWM channels
enum Channel_PWM {
	CHANNEL_PWM1,
	CHANNEL_PWM2,
	CHANNEL_PWM_COUNT
};

extern const char * pwm_ch_names[CHANNEL_PWM_COUNT];

// generator PWM - timer settings
// TODO specify correct values for this MCU
static const uint16_t COUNTER_PERIOD = 7199;
static const uint16_t FREQUENCY_MAX = (72000000 / (COUNTER_PERIOD + 1));

// DAC channels
enum Channel_DAC {
	CHANNEL_DAC_COUNT
};

extern const char ** dac_ch_names;

#endif /* INC_BSP_CONF_HPP_ */
