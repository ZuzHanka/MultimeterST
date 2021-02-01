/*
 * bsp.hpp
 *
 *  Created on: 5 Dec 2020
 *      Author: ZuzHanka
 */

#ifndef BSP_HPP_
#define BSP_HPP_

#include "bsp_conf.hpp"

#include <cstdint>
#include <cstring>

size_t terminal_receive(char * buff, size_t buff_size);
bool terminal_transmit(const char * buff, size_t buff_size);

bool adc_run(void);
uint16_t adc_get_sample(uint8_t channel);

/* Callbacks -----------------------------------------------------------------*/

void adc_callback(void);

#endif /* BSP_HPP_ */
