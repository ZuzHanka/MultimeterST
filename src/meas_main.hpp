/*
 * meas_control.hpp
 *
 *  Created on: Jul 17, 2021
 *      Author: zuzhanka
 */

#ifndef MEAS_MAIN_HPP_
#define MEAS_MAIN_HPP_


#include "avg_filter.hpp"
#include "bsp.hpp"

#include <cstdint>


class MeasurementMain
{
public:
	static const size_t TERMINAL_WIDTH = 80;

	MeasurementMain()
	{
		set_no_measurements();
		reset_loop_counter();
	}

	void loop();
	void reset_loop_counter()
	{
		m_loop_counter = 0;
	}
	bool loop_continues_condition()
	{
		return m_loop_counter < m_no_measurements;
	}
	void adc_callback();

protected:
//	bool send(const char * message)
//	{
//		return terminal_transmit(message, strlen(message));
//	}
	void set_no_measurements();
	void print_measured();

private:
	uint16_t adc_samples[ADC_CHANNELS];
	uint16_t adc_samples2print[ADC_CHANNELS];

	uint16_t m_no_measurements;
	volatile uint16_t m_loop_counter;
	bool m_print_measured = false;

	char m_buffer_avg[TERMINAL_WIDTH + 1] = { 0 };
};


#endif /* MEAS_MAIN_HPP_ */
