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

static const uint32_t MAX_BSP_TIMEOUT = 0xFFFFFFFFU;

uint32_t get_tick();
void delay(uint32_t ticks);

size_t terminal_receive(char * buff, size_t buff_size);
bool terminal_transmit(const char * buff, size_t buff_size);

bool comlink_receive(uint8_t * buff, uint16_t buff_size, uint32_t timeout = MAX_BSP_TIMEOUT);
bool comlink_transmit(const uint8_t * buff, uint16_t buff_size);

bool adc_run();
bool pwm_run();
uint16_t adc_get_sample_mV(uint8_t channel);
float adc_mV_to_Celsius(int16_t value_mV);
uint32_t pwm_get_duty(uint32_t channel);
void pwm_set_duty(uint32_t channel, uint32_t duty);
uint32_t pwm_get_freq();
void pwm_set_freq(uint32_t freq);

/* Callbacks -----------------------------------------------------------------*/

void adc_callback(void);

#endif /* BSP_HPP_ */
