/*
 * bsp_conf.hpp
 *
 *  Created on: Feb 1, 2021
 *      Author: jan.humpl
 */

#ifndef INC_BSP_CONF_HPP_
#define INC_BSP_CONF_HPP_

#include <cstdint>

enum Channel {
	CHANNEL_1,
	CHANNEL_2,
	CHANNEL_TEMP,
	CHANNEL_VREFINT,
	CHANNEL_3,
	CHANNEL_4,
	CHANNEL_COUNT,
	ADC1_IDX = CHANNEL_1,			// Offset of ADC1 channels.
	ADC2_IDX = CHANNEL_VREFINT,		// Offset of ADC2 channels.
	ADC3_IDX = CHANNEL_4,			// Offset of ADC3 channels.
	ADC1_CHANNELS = 3,				// Number of ADC1 channels.
	ADC2_CHANNELS = 2,				// Number of ADC2 channels.
	ADC3_CHANNELS = 1,				// Number of ADC3 channels.
	ADC_CHANNELS = CHANNEL_4 + 1,	// Number of active channels.
	CHANNEL_VDDA = CHANNEL_VREFINT	// Return Vdda instead of Vrefint value.
};

static constexpr uint8_t Channel_ordered[] =
{
		CHANNEL_1,
		CHANNEL_2,
		CHANNEL_3,
		CHANNEL_4
};

extern const char * adc_ch_names[CHANNEL_COUNT];

// ADC bits
static const uint8_t ADC_BITS = 12;
// temperature sensor average slope [mV/*C]
static const float AVG_SLOPE = 4.3;
// temperature sensor voltage at 25*C [mV]
static const uint16_t V25 = 1430;

enum Channel_PWM {
	CHANNEL_PWM1,
	CHANNEL_PWM2,
	CHANNEL_PWM_COUNT
};

extern const char * pwm_ch_names[CHANNEL_PWM_COUNT];

#endif /* INC_BSP_CONF_HPP_ */
