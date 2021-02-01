/*
 * bsp_conf.hpp
 *
 *  Created on: Feb 1, 2021
 *      Author: jan.humpl
 */

#ifndef BSP_CONF_HPP_
#define BSP_CONF_HPP_

#include <cstdint>
#include <cstring>

enum Channel {
	CHANNEL_1,
	CHANNEL_2,
	CHANNEL_3,
	CHANNEL_4,
	CHANNEL_TEMP,
	CHANNEL_VREFINT,
	CHANNEL_COUNT,
	ADC_CHANNELS = CHANNEL_COUNT	// Number of active channels.
};

// ADC bits
static const uint8_t ADC_BITS = 12;
// internal reference voltage [V]
static const int32_t VREFINT = 1210;
// temperature sensor average slope [mV/*C]
static const float AVG_SLOPE = 2.5;
// temperature sensor voltage at 25*C [mV]
static const uint16_t V25 = 760;

#endif /* BSP_CONF_HPP_ */
