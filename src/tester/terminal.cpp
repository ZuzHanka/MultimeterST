#include "terminal.hpp"

#include "bsp.hpp"
#include "calculations.hpp"

#include <cstdio>
#include <cstring>


/* Constants ---------------------------------------------------------*/
const char version[20] = "1.2.0";


/* Functions ---------------------------------------------------------*/
void Terminal::loop()
{
	if (m_print_measured)
	{
		m_print_measured = false;
		print_measured();
	}
}

void Terminal::set_no_measurements(uint16_t value)
{
	m_no_measurements = value;
}

void Terminal::set_recharge_loops(uint16_t value)
{
	m_recharge_loops = value;
}

void Terminal::print_measured()
{
	// value set to DAC
	float dac_V = convert_mV2V(dac_sample2print);

	// number of user ADC voltmeter channels
	int user_volt_channels = sizeof(Channel_ordered);

	// read and convert actual ADC values here
	float adc_V1[ADC_CHANNELS];
	float adc_V2[ADC_CHANNELS];

	// get values from ADC
	for (int ch = 0; ch < ADC_CHANNELS; ch++)
	{
		if (ch == CHANNEL_TEMP)
		{
			adc_V1[ch] = convert2celsius(adc_samples2print1[ch]);
			adc_V2[ch] = convert2celsius(adc_samples2print2[ch]);
		}
		else
		{
			adc_V1[ch] = convert_mV2V(adc_samples2print1[ch]);
			adc_V2[ch] = convert_mV2V(adc_samples2print2[ch]);
		}
	}

	char buffer_avg[TERMINAL_WIDTH + 1] = " ";
	buffer_avg[TERMINAL_WIDTH] = '\0';
	char aux_buffer[TERMINAL_WIDTH + 1];
	aux_buffer[TERMINAL_WIDTH] = '\0';

	// number of measurements for print
	snprintf(aux_buffer, TERMINAL_WIDTH, "%d  ", m_no_measurements);
	strcat(buffer_avg, aux_buffer);

	// DAC value for print
	snprintf(aux_buffer, TERMINAL_WIDTH, "%5.3f  ", dac_V);
	strcat(buffer_avg, aux_buffer);

	// ADC values for print
	for (int ch = 0; ch < user_volt_channels; ch++)
	{
		snprintf(aux_buffer, TERMINAL_WIDTH, "%5.3f  ", adc_V1[Channel_ordered[ch]]);
		strcat(buffer_avg, aux_buffer);
	}
	for (int ch = 0; ch < user_volt_channels; ch++)
	{
		snprintf(aux_buffer, TERMINAL_WIDTH, "%5.3f  ", adc_V2[Channel_ordered[ch]]);
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
			if (m_loop_counter >= m_no_measurements)
			{
				for (int ch = 0; ch < ADC_CHANNELS; ch++)
				{
					adc_samples2print2[ch] = adc_samples[ch];
					dac_sample2print = m_dac_mV;
				}
				m_print_measured = true;
				m_process = STOP;
			}
			break;
		}
		case START :
		{
			for (int ch = 0; ch < ADC_CHANNELS; ch++)
			{
				adc_samples2print1[ch] = adc_samples[ch];
			}
//			m_print_measured = true;
			set_switch(true);
			m_process = RUNNING;
			break;
		}
		case RECHARGING :
		{
			m_loop_counter++;
			if (m_loop_counter >= m_recharge_loops)
			{
				reset_loop_counter();
				m_process = START;
			}
			break;
		}
		case CHARGED :
		{
			break;
		}
		case CHARGING :
		{
			m_loop_counter++;
			if (m_loop_counter >= 3 * m_no_measurements)  // needs more loops to charge
			{
				for (int ch = 0; ch < ADC_CHANNELS; ch++)
				{
					adc_samples2print2[ch] = adc_samples[ch];
					dac_sample2print = m_dac_mV;
				}
//				m_print_measured = true;
				m_process = CHARGED;
			}
			break;
		}
		case PRESTART :
		{
			for (int ch = 0; ch < ADC_CHANNELS; ch++)
			{
				adc_samples2print1[ch] = adc_samples[ch];
			}
//			m_print_measured = true;
			set_switch(true);
			m_process = CHARGING;
			break;
		}
		default : {}
	}

	dac_set_value(m_dac_mV);
}



