/*
 * meas_control.cpp
 *
 *  Created on: Jul 17, 2021
 *      Author: zuzhanka
 */


#include "meas_main.hpp"
#include "bsp.hpp"
#include "calculations.hpp"

#include <cstdint>


/* Constants ---------------------------------------------------------*/
const uint16_t NO_MEASUREMENTS = 1000;


/* Functions ---------------------------------------------------------*/
void MeasurementMain::loop()
{
//	update_voltmeter();
//	print_voltmeter();
}

void MeasurementMain::update_voltmeter()
{
	uint16_t is_new_average = 0;
	for (int ch=0; ch<ADC_CHANNELS; ch++)
	{
		if (avgf[ch].is_new_average()) is_new_average++;
	}

	if (is_new_average == ADC_CHANNELS)
	{
		// number of user ADC voltmeter channels
		int user_volt_channels = sizeof(Channel_ordered);

		// read and convert actual ADC values here
		float avg[ADC_CHANNELS];

		// get values from ADC
		for (int ch=0; ch<ADC_CHANNELS; ch++)
		{
			if (ch == CHANNEL_TEMP)
			{
				avg[ch] = convert2celsius(avgf[ch].get());
			}
			else
			{
				avg[ch] = convert_mV2V(avgf[ch].get());
			}
		}

		// compute values for print
		m_buffer_avg[0] = '\0';
		const char * formatstring;
		char aux_buffer[TERMINAL_WIDTH + 1];
		for (int ch = 0; ch < user_volt_channels; ch++)
		{
			formatstring = "%6.3f,";
			snprintf(aux_buffer, TERMINAL_WIDTH, formatstring, avg[Channel_ordered[ch]]);
			strcat(m_buffer_avg, aux_buffer);
		}
		snprintf(aux_buffer, TERMINAL_WIDTH, "%6dx\n", m_loop_counter);  // or m_no_measurements ?
		strcat(m_buffer_avg, aux_buffer);
	}
}

void MeasurementMain::print_voltmeter()
{
	send(m_buffer_avg);
}

void MeasurementMain::set_no_measurements()
{
	m_no_measurements = NO_MEASUREMENTS;
}

// Called when buffer is completely filled
void MeasurementMain::adc_callback()
{
//	for (int ch = 0; ch < ADC_CHANNELS; ch++)
//	{
//		int16_t sample = adc_get_sample_mV(ch);
//		avgf[ch].update(sample);
//	}
//	if (get_switch())
//	{
//		m_loop_counter++;
//	}
}
