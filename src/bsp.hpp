/*
 * bsp.hpp
 *
 *  Created on: 5 Dec 2020
 *      Author: ZuzHanka
 */

#ifndef BSP_HPP_
#define BSP_HPP_

#include <cstdint>
#include <cstring>

enum Channel {
	CHANNEL_1,
	CHANNEL_2,
	CHANNEL_3,
	CHANNEL_4,
	CHANNEL_TEMP,
	CHANNEL_VREFINT,
	CHANNEL_COUNT
};

// ADC bits
static const uint8_t ADC_BITS = 12;
// ADC number of active channels
static const uint8_t ADC_CHANNELS = CHANNEL_COUNT;
// internal reference voltage [V]
static const int32_t VREFINT = 1210;
// temperature sensor average slope [mV/*C]
static const float AVG_SLOPE = 2.5;
// temperature sensor voltage at 25*C [mV]
static const uint16_t V25 = 760;

size_t terminal_receive(char * buff, size_t buff_size);
bool terminal_transmit(char * buff, size_t buff_size);

bool adc_run(void);
uint16_t adc_get_sample(uint8_t channel);

/* Callbacks -----------------------------------------------------------------*/

void adc_callback(void);

#endif /* BSP_HPP_ */
