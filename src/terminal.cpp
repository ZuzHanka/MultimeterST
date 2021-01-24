#include "terminal.hpp"

#include "bsp.hpp"
#include "calculations.hpp"
#include "communication.hpp"

#include <cstdio>
#include <cstring>

const char Terminal::BLACK[] = "\x1b[30m";
const char Terminal::RED[] = "\x1b[31m";
const char Terminal::GREEN[] = "\x1b[32m";
const char Terminal::YELLOW[] = "\x1b[33m";
const char Terminal::BLUE[] = "\x1b[34m";
const char Terminal::MAGENTA[] = "\x1b[35m";
const char Terminal::CYAN[] = "\x1b[36m";
const char Terminal::WHITE[] = "\x1b[37m";

const char Terminal::BRIGHT_BLACK[] = "\x1b[30;1m";
const char Terminal::BRIGHT_RED[] = "\x1b[31;1m";
const char Terminal::BRIGHT_GREEN[] = "\x1b[32;1m";
const char Terminal::BRIGHT_YELLOW[] = "\x1b[33;1m";
const char Terminal::BRIGHT_BLUE[] = "\x1b[34;1m";
const char Terminal::BRIGHT_MAGENTA[] = "\x1b[35;1m";
const char Terminal::BRIGHT_CYAN[] = "\x1b[36;1m";
const char Terminal::BRIGHT_WHITE[] = "\x1b[37;1m";

const char Terminal::RESET[] = "\x1b[0m";

const char Terminal::BOLD[] = "\x1b[1m";
const char Terminal::UNDERLINE[] = "\x1b[4m";
const char Terminal::REVERSED[] = "\x1b[7m";

const char Terminal::CLEAR_SCREEN[] = "\x1b[2J";
// n=0 clears from cursor until end of screen,
// n=1 clears from cursor to beginning of screen
// n=2 clears entire screen
const char Terminal::CLEAR_LINE[] = "\x1b[2K";
// n=0 clears from cursor to end of line
// n=1 clears from cursor to start of line
// n=2 clears entire line

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

		print_help();

		const size_t TERMINAL_WIDTH = 80;
		char buffer[TERMINAL_WIDTH + 1];
		buffer[TERMINAL_WIDTH] = '\0';

		snprintf(buffer, TERMINAL_WIDTH, "V1: %7.3f V     V2: %7.3f V     V3: %7.3f V     V4: %7.3f V",
			  avg[CHANNEL_1], avg[CHANNEL_2], avg[CHANNEL_3], avg[CHANNEL_4]);
		print_advanced(3, 0, BRIGHT_CYAN, buffer);
		snprintf(buffer, TERMINAL_WIDTH, "V2-V1: %7.3f V     V3-V2: %7.3f V     V4-V3: %7.3f V",
			  diff[CHANNEL_1], diff[CHANNEL_2], diff[CHANNEL_3]);
		print_advanced(4, 0, BRIGHT_CYAN, buffer);
		snprintf(buffer, TERMINAL_WIDTH, "Temp: %5.1f *C",
				avg[CHANNEL_TEMP]);
		print_advanced(5, 0, BRIGHT_CYAN, buffer);
	}
}

bool Terminal::set_cursor_position(uint8_t row, uint8_t col)
{
	constexpr char SET_POSITION[] = "\x1b[%d;%dH";
	char buff[11];
	snprintf(buff, sizeof(buff), SET_POSITION, row, col);
	return send(buff);
}

bool Terminal::print_advanced(uint8_t row, uint8_t col, const char * color, const char * message)
{
	bool success = true;
	success = success && set_cursor_position(row, col);
	success = success && send(CLEAR_LINE);
	success = success && send(color);
	success = success && send(message);
	success = success && send(RESET);
	return success;
}

bool Terminal::print_help()
{
	uint8_t row = 10;
	bool success = true;
//	success = success && print_advanced(row++, 0, WHITE, "");
	success = success && print_advanced(row++, 0, WHITE, "d - toggle differential/normal mode");
	success = success && print_advanced(row++, 0, WHITE, "q - stop logging");
	success = success && print_advanced(row++, 0, WHITE, "l - start logging");
	success = success && print_advanced(row++, 0, WHITE, "z - set zero to current value");
	success = success && print_advanced(row++, 0, WHITE, "r - reset zero to default");
	success = success && print_advanced(row++, 0, WHITE, "n - set number of samples per average");
	return success;
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


