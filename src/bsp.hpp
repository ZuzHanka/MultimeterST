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

uint32_t get_tick(void);

size_t terminal_receive(char * buff, size_t buff_size);
bool terminal_transmit(const char * buff, size_t buff_size);

void adc_init(ADC_TypeDef * adc, const adc_conf_t adc_conf[], size_t chan_count);
bool adc_run(uint32_t period_us);
void adc_trigger_stop();
void dac_init(void);
bool dac_run(void);
bool pwm_run(void);
void pwm_synchronize();
uint16_t adc_get_sample_mV(uint8_t channel);
float adc_mV_to_Celsius(int16_t value_mV);
float pwm_get_duty(uint32_t channel);
void pwm_set_duty(uint32_t channel, uint32_t duty);
void pwm_duty_increment(uint32_t channel, int32_t increment_promile);
float pwm_get_freq(uint32_t channel);
void pwm_set_freq(uint32_t channel, uint32_t freq);
void pwm_freq_increment(uint32_t channel, int32_t increment_promile);
uint32_t dac_get_value(void);
void dac_set_value(uint16_t value_mV);
void set_switch(bool value);
bool get_switch(void);
void delay(uint32_t time_ms);

/* Callbacks -----------------------------------------------------------------*/

void adc_callback(void);

#endif /* BSP_HPP_ */
