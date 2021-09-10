/*
 * bsp_conf.hpp
 *
 *  Created on: Feb 1, 2021
 *      Author: jan.humpl
 */

#ifndef INC_BSP_CONF_HPP_
#define INC_BSP_CONF_HPP_

#include <cstdint>

const char board_name[30] = "Nucleo-F303RE";

// ADC channels
enum Channel {
	CHANNEL_1,
	CHANNEL_2,
	CHANNEL_3,
	CHANNEL_4,
	CHANNEL_VREFINT,
	CHANNEL_TEMP,
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

// ADC bits
static const uint8_t ADC_BITS = 12;
// temperature sensor average slope [mV/*C]
static const float AVG_SLOPE = 4.3;
// temperature sensor voltage at 25*C [mV]
static const uint16_t V25 = 1430;


#endif /* INC_BSP_CONF_HPP_ */
