#include "terminal.hpp"

#include "bsp.hpp"
#include "calculations.hpp"

#include <cstdio>
#include <cstring>


static const char ESC_RESET[] = "\x1b[0m";

static const char ESC_BOLD[] = "\x1b[1m";
static const char ESC_UNDERLINE[] = "\x1b[4m";
static const char ESC_REVERSED[] = "\x1b[7m";

static const char ESC_CLEAR_SCREEN[] = "\x1b[2J";
// n=0 clears from cursor until end of screen,
// n=1 clears from cursor to beginning of screen
// n=2 clears entire screen
static const char ESC_CLEAR_LINE[] = "\x1b[2K";
// n=0 clears from cursor to end of line
// n=1 clears from cursor to start of line
// n=2 clears entire line


void Terminal::loop()
{
	bool was_key_pressed = key_pressed();

	if (m_redraw_screen)
	{
		m_redraw_screen = false;
		welcome();
		print_help();
	}

	if (was_key_pressed)
	{
		print_status();
	}

	update_voltmeter();
}

bool Terminal::set_cursor_position(uint8_t row, uint8_t col)
{
	constexpr char SET_POSITION[] = "\x1b[%d;%dH";
	char buff[11];
	snprintf(buff, sizeof(buff), SET_POSITION, row, col);
	return send(buff);
}

bool Terminal::text_decoration(uint32_t flags)
{
	bool success = true;

	if ((flags & CLEAR_SCREEN) != 0)
	{
		success = success && send(ESC_CLEAR_SCREEN);
	}

	if ((flags & CLEAR_LINE) != 0)
	{
		success = success && send(ESC_CLEAR_LINE);
	}

	if ((flags & COLOR) != 0)
	{
		uint8_t color = (uint8_t) (flags & COLOR);
		uint8_t bright = (uint8_t) (flags & BRIGHT);

		char buff[10];
		if (bright != 0)
		{
			snprintf(buff, sizeof(buff), "\x1b[%d;1m", color);
		}
		else
		{
			snprintf(buff, sizeof(buff), "\x1b[%dm", color);
		}
		success = success && send(buff);
	}

	if ((flags & BOLD) != 0)
	{
		success = success && send(ESC_BOLD);
	}

	if ((flags & UNDERLINE) != 0)
	{
		success = success && send(ESC_UNDERLINE);
	}

	if ((flags & REVERSED) != 0)
	{
		success = success && send(ESC_REVERSED);
	}

	return success;
}

bool Terminal::print_advanced(uint8_t row, uint8_t col, uint32_t decoration, const char * message)
{
	bool success = true;
	success = success && set_cursor_position(row, col);
	success = text_decoration(decoration);
	success = success && send(message);
	success = success && send(ESC_RESET);
	return success;
}

bool Terminal::print_help()
{
	uint8_t row = 10;
	bool success = true;
//	success = success && print_advanced(row++, 0, CLEAR_LINE | WHITE, "");
	success = success && print_advanced(row++, 0, CLEAR_LINE | WHITE, "d - toggle differential/normal mode");
	success = success && print_advanced(row++, 0, CLEAR_LINE | WHITE, "q - stop logging");
	success = success && print_advanced(row++, 0, CLEAR_LINE | WHITE, "l - start logging");
	success = success && print_advanced(row++, 0, CLEAR_LINE | WHITE, "z - set zero to current value");
	success = success && print_advanced(row++, 0, CLEAR_LINE | WHITE, "r - reset zero to default");
	success = success && print_advanced(row++, 0, CLEAR_LINE | WHITE, "n - set number of samples per average");
	return success;
}

bool Terminal::welcome()
{
	bool success = true;
	success = success && print_advanced(0, 32, CLEAR_SCREEN | BOLD | UNDERLINE | BRIGHT | YELLOW, "Multimeter ST");
	return success;
}

void Terminal::update_voltmeter()
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

		if (m_voltmeter_diff_mode)
		{
			if (m_voltmeter_zero_mode)
			{
				if (m_voltmeter_zero_mode_avg_update)
				{
					m_voltmeter_zero_mode_avg_update = false;
					m_zero_avg[CHANNEL_1] = diff[CHANNEL_1];
					m_zero_avg[CHANNEL_2] = diff[CHANNEL_2];
					m_zero_avg[CHANNEL_3] = diff[CHANNEL_3];
				}
				snprintf(buffer, TERMINAL_WIDTH, "V2-V1: %7.3f V     V3-V2: %7.3f V     V4-V3: %7.3f V",
						diff[CHANNEL_1] - m_zero_avg[CHANNEL_1], diff[CHANNEL_2] - m_zero_avg[CHANNEL_2],
						diff[CHANNEL_3] - m_zero_avg[CHANNEL_3]);
			}
			else
			{
				snprintf(buffer, TERMINAL_WIDTH, "V2-V1: %7.3f V     V3-V2: %7.3f V     V4-V3: %7.3f V",
					  diff[CHANNEL_1], diff[CHANNEL_2], diff[CHANNEL_3]);
			}

			print_advanced(4, 0, CLEAR_LINE | BRIGHT | CYAN, buffer);
		}
		else
		{
			if (m_voltmeter_zero_mode)
			{
				if (m_voltmeter_zero_mode_avg_update)
				{
					m_voltmeter_zero_mode_avg_update = false;
					m_zero_avg[CHANNEL_1] = avg[CHANNEL_1];
					m_zero_avg[CHANNEL_2] = avg[CHANNEL_2];
					m_zero_avg[CHANNEL_3] = avg[CHANNEL_3];
					m_zero_avg[CHANNEL_4] = avg[CHANNEL_4];
				}
				snprintf(buffer, TERMINAL_WIDTH, "V1: %7.3f V     V2: %7.3f V     V3: %7.3f V     V4: %7.3f V",
					  avg[CHANNEL_1] - m_zero_avg[CHANNEL_1], avg[CHANNEL_2] - m_zero_avg[CHANNEL_2],
					  avg[CHANNEL_3] - m_zero_avg[CHANNEL_3], avg[CHANNEL_4] - m_zero_avg[CHANNEL_4]);
			}
			else
			{
				snprintf(buffer, TERMINAL_WIDTH, "V1: %7.3f V     V2: %7.3f V     V3: %7.3f V     V4: %7.3f V",
					  avg[CHANNEL_1], avg[CHANNEL_2], avg[CHANNEL_3], avg[CHANNEL_4]);
			}

			print_advanced(4, 0, CLEAR_LINE | BRIGHT | CYAN, buffer);
		}

		snprintf(buffer, TERMINAL_WIDTH, "%5.1f *C ",
				avg[CHANNEL_TEMP]);
		print_advanced(25, 70, CYAN, buffer);
		print_advanced(25, 80, 0, "");
	}
}

bool Terminal::key_pressed()
{
	char key;
	size_t recv_bytes = terminal_receive(&key, 1);

	if (recv_bytes == 1)
	{
		bool valid_key = false;

		switch (key)
		{
			case 'd' :
				{
					valid_key = true;
					m_voltmeter_diff_mode = !m_voltmeter_diff_mode;
					m_voltmeter_zero_mode = false;
					if (m_voltmeter_diff_mode)
					{
						set_status("Voltmeter switched to differential mode.");
					}
					else
					{
						set_status("Voltmeter switched to default mode.");
					}
				}
				break;

			case 'z' :
				{
					valid_key = true;
					m_voltmeter_zero_mode = !m_voltmeter_zero_mode;
					if (m_voltmeter_zero_mode)
					{
						m_voltmeter_zero_mode_avg_update = true;
						if (m_voltmeter_diff_mode)
						{
							set_status("Voltmeter switched to zero differential mode.");
						}
						else
						{
							set_status("Voltmeter switched to zero mode.");
						}
					}
					else
					{
						m_voltmeter_zero_mode_avg_update = false;
						if (m_voltmeter_diff_mode)
						{
							set_status("Voltmeter switched to differential mode.");
						}
						else
						{
							set_status("Voltmeter switched to default mode.");
						}
					}
				}
				break;

			default:
				set_status("Unsupported key pressed!");
				break;
		}

		return valid_key;
	}

	return false;
}

void Terminal::set_status(const char * message)
{
	m_status_message = message;
}

void Terminal::print_status()
{
	if (m_status_message != nullptr)
	{
		print_advanced(25, 0, CLEAR_LINE | WHITE, m_status_message);
	}
	else
	{
		print_advanced(25, 0, WHITE, "");
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



