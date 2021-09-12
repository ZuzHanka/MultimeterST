#include "terminal.hpp"

#include "bsp.hpp"
#include "calculations.hpp"

#include <cstdio>
#include <cstring>


/* Constants ---------------------------------------------------------*/
const char version[20] = "1.2.0";

const uint16_t NO_MEASUREMENTS = 1000;


/* Functions ---------------------------------------------------------*/
void Terminal::loop()
{
	if (m_print_measured)
	{
		print_measured();
		m_print_measured = false;
	}
}

void Terminal::set_no_measurements()
{
	m_no_measurements = NO_MEASUREMENTS;
}

void Terminal::print_measured()
{
	// value set to DAC
	float dac_V = convert_mV2V(m_dac_mV);

	// number of user ADC voltmeter channels
	int user_volt_channels = sizeof(Channel_ordered);

	// read and convert actual ADC values here
//	float avg[ADC_CHANNELS];
	float adc_V[ADC_CHANNELS];

	// get values from ADC
	for (int ch = 0; ch < ADC_CHANNELS; ch++)
	{
		if (ch == CHANNEL_TEMP)
		{
//			avg[ch] = convert2celsius(avgf_measured[ch].get());
			adc_V[ch] = convert2celsius(adc_samples2print[ch]);
		}
		else
		{
//			avg[ch] = convert_mV2V(avgf_measured[ch].get());
			adc_V[ch] = convert_mV2V(adc_samples2print[ch]);
		}
	}

	char buffer_avg[TERMINAL_WIDTH + 1] = " ";
	buffer_avg[TERMINAL_WIDTH] = '\0';
	char aux_buffer[TERMINAL_WIDTH + 1];
	aux_buffer[TERMINAL_WIDTH] = '\0';

	// number of measurements for print
	snprintf(aux_buffer, TERMINAL_WIDTH, "%9d, ", m_no_measurements);
	strcat(buffer_avg, aux_buffer);

	// DAC value for print
	snprintf(aux_buffer, TERMINAL_WIDTH, "%6.3f, ", dac_V);
	strcat(buffer_avg, aux_buffer);

	// ADC values for print
	for (int ch = 0; ch < user_volt_channels; ch++)
	{
//		snprintf(aux_buffer, TERMINAL_WIDTH, "%6.3f, ", avg[Channel_ordered[ch]]);
		snprintf(aux_buffer, TERMINAL_WIDTH, "%6.3f, ", adc_V[Channel_ordered[ch]]);
		strcat(buffer_avg, aux_buffer);
	}
	strcat(buffer_avg, "\n");

	// print
	send(buffer_avg);
}

void Terminal::set_dac_mV(uint16_t value_mV)
{
	m_dac_mV = value_mV;
}

// Called when buffer is completely filled
void Terminal::adc_callback() {
	for (int ch = 0; ch < ADC_CHANNELS; ch++)
	{
//		avgf[ch].update(adc_get_sample_mV(ch));
		adc_samples[ch] = adc_get_sample_mV(ch);
	}

	switch (m_process)
	{
		case STOP :
		{
			set_switch(false);
			reset_loop_counter();
			m_process = IDLE;
			break;
		}
		case RUNNING :
		{
			m_loop_counter++;
			if (loop_continues_condition() == false)
			{
				for (int ch = 0; ch < ADC_CHANNELS; ch++)
				{
//					avgf_measured[ch] = avgf[ch];
					adc_samples2print[ch] = adc_samples[ch];
				}
				m_print_measured = true;
				m_process = STOP;
			}
			break;
		}
		case START :
		{
			set_switch(true);
			m_process = RUNNING;
			break;
		}
		case READY :
		{
			for (int ch = 0; ch < ADC_CHANNELS; ch++)
			{
//				avgf_measured[ch] = avgf[ch];
				adc_samples2print[ch] = adc_samples[ch];
			}
			m_print_measured = true;
			m_process = START;
			break;
		}
		default : {}
	}

	dac_set_value(m_dac_mV);
}



