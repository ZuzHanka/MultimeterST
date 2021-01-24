#include "terminal.hpp"

#include "bsp.hpp"
#include "calculations.hpp"
#include "communication.hpp"

#include <cstdio>
#include <cstring>


AvgFilter Terminal::avgf[ADC_CHANNELS];

void Terminal::loop()
{
	uint16_t is_new_average = 0;
	for (int ch=0; ch<ADC_CHANNELS; ch++)
	{
		if (avgf[ch].is_new_average()) is_new_average++;
	}
	if (is_new_average == ADC_CHANNELS)
	{
		float avg[ADC_CHANNELS];
		float diff[ADC_CHANNELS - 1];
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
		for (int ch=0; ch<ADC_CHANNELS - 1; ch++)
		{
			diff[ch] = compute_Vdiff(avg[ch + 1], avg[ch]);
		}

		const size_t TERMINAL_WIDTH = 80;
		char buffer[TERMINAL_WIDTH + 1];
		buffer[TERMINAL_WIDTH] = '\0';

		snprintf(buffer, TERMINAL_WIDTH, "V1: %7.3f V     V2: %7.3f V     V3: %7.3f V     V4: %7.3f V",
			  avg[CHANNEL_1], avg[CHANNEL_2], avg[CHANNEL_3], avg[CHANNEL_4]);
		send_msg(buffer, "\x1b[3;0f\x1b[2K\x1b[36;1m", "\x1b[0m");
		snprintf(buffer, TERMINAL_WIDTH, "V2-V1: %7.3f V     V3-V2: %7.3f V     V4-V3: %7.3f V",
			  diff[CHANNEL_1], diff[CHANNEL_2], diff[CHANNEL_3]);
		send_msg(buffer, "\x1b[4;0f\x1b[2K\x1b[36;1m", "\x1b[0m");
		snprintf(buffer, TERMINAL_WIDTH, "Temp: %5.1f *C",
				avg[CHANNEL_TEMP]);
		send_msg(buffer, "\x1b[5;0f\x1b[2K\x1b[36;1m", "\x1b[0m");
	}

}

// Called when buffer is completely filled
void Terminal::adc_callback() {
	for (int ch=0; ch<ADC_CHANNELS; ch++)
	{
		int16_t sample = convert2milivolt(adc_get_sample(ch), adc_get_sample(CHANNEL_VREFINT));
		avgf[ch].update(sample);
	}
}

void adc_callback()
{
	Terminal::adc_callback();
}
