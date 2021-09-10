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

uint32_t get_tick(void);

size_t terminal_receive(char * buff, size_t buff_size);
bool terminal_transmit(const char * buff, size_t buff_size);

bool adc_run(void);
bool adc_start(void);
bool adc_stop(void);
uint16_t adc_get_sample_mV(uint8_t channel);
float adc_mV_to_Celsius(int16_t value_mV);
void set_switch(bool value);
bool get_switch(void);
void delay(uint32_t time_ms);

/* Callbacks -----------------------------------------------------------------*/

void adc_callback(void);

#endif /* BSP_HPP_ */
