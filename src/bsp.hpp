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
bool dac_run(void);
bool pwm_run(void);
uint16_t adc_get_sample_mV(uint8_t channel);
float adc_mV_to_Celsius(int16_t value_mV);
uint32_t pwm_get_duty(uint32_t channel);
void pwm_set_duty(uint32_t channel, uint32_t duty);
uint32_t pwm_get_freq(uint32_t channel);
void pwm_set_freq(uint32_t channel, uint32_t freq);
uint32_t dac_get_value(void);
void dac_set_value(uint16_t value_mV);
void set_switch(bool value);
bool get_switch(void);
void delay(uint32_t time_ms);

/* Callbacks -----------------------------------------------------------------*/

void adc_callback(void);

#endif /* BSP_HPP_ */
