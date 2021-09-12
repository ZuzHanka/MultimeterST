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
const uint16_t NO_MEASUREMENTS = 10;


/* Functions ---------------------------------------------------------*/
void MeasurementMain::loop()
{
//	if (m_print_measured)
//	{
//		print_measured();
//		m_print_measured = false;
//	}
}

void MeasurementMain::print_measured()
{
	// number of user ADC voltmeter channels
	int user_volt_channels = sizeof(Channel_ordered);

	// read and convert actual ADC values here
	float adc_V[ADC_CHANNELS];

	// get values from ADC
	for (int ch = 0; ch < ADC_CHANNELS; ch++)
	{
		if (ch == CHANNEL_TEMP)
		{
			adc_V[ch] = convert2celsius(adc_samples2print[ch]);
		}
		else
		{
			adc_V[ch] = convert_mV2V(adc_samples2print[ch]);
		}
	}

	char buffer_avg[TERMINAL_WIDTH + 1] = " ";
	buffer_avg[TERMINAL_WIDTH] = '\0';
	char aux_buffer[TERMINAL_WIDTH + 1];
	aux_buffer[TERMINAL_WIDTH] = '\0';

	// number of measurements for print
	snprintf(aux_buffer, TERMINAL_WIDTH, "%d, ", m_loop_counter);
	strcat(buffer_avg, aux_buffer);

	// ADC values for print
	for (int ch = 0; ch < user_volt_channels; ch++)
	{
		snprintf(aux_buffer, TERMINAL_WIDTH, "%6.3f, ", adc_V[Channel_ordered[ch]]);
		strcat(buffer_avg, aux_buffer);
	}
	strcat(buffer_avg, "\n");

	// print
	send(buffer_avg);
}

void MeasurementMain::set_no_measurements()
{
	m_no_measurements = NO_MEASUREMENTS;
}

// Called when buffer is completely filled
void MeasurementMain::adc_callback()
{
	for (int ch = 0; ch < ADC_CHANNELS; ch++)
	{
		adc_samples[ch] = adc_get_sample_mV(ch);
	}
//	if (loop_continues_condition())
//	{
//		m_loop_counter++;
//		for (int ch = 0; ch < ADC_CHANNELS; ch++)
//		{
//			adc_samples2print[ch] = adc_samples[ch];
//		}
//		m_print_measured = true;
//	}
}
