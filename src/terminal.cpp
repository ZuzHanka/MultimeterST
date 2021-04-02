#include "terminal.hpp"

#include "bsp.hpp"
#include "calculations.hpp"

#include <cstdio>
#include <cstring>


const char version[20] = "1.1.0";

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
		if (m_voltmeter_logging)
		{
			print_advanced(0, 0, CLEAR_SCREEN, "");
		}
		else if (m_voltmeter_no_samples_mode)
		{
			welcome();
			print_setup();
			print_help(1);
			update_generator();
		}
		else
		{
			welcome();
			print_setup();
			print_help(0);
			update_generator();
		}
	}

	if (was_key_pressed && (m_voltmeter_logging == false))
	{
		print_status();
		print_from_keyboard();
		if (m_voltmeter_no_samples_mode)
		{
			print_help(1);
		}
		else
		{
			print_help(0);
		}
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

bool Terminal::print_help(uint8_t help_spec)
{
	bool success = true;
	uint8_t row = 10;

	//	success = success && print_advanced(row++, 2, CLEAR_LINE | YELLOW, "");

	switch (help_spec)
	{
	case (1) :  // get number from keyboard
		{
			success = success && print_advanced(row++, 2, CLEAR_LINE | YELLOW, "0 .. 9 - write numbers");
			success = success && print_advanced(row++, 2, CLEAR_LINE | YELLOW, "n - set number of samples per average");
			success = success && print_advanced(row++, 2, CLEAR_LINE | YELLOW, "q - leave without change");
			success = success && print_advanced(row++, 2, CLEAR_LINE | YELLOW, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | YELLOW, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | YELLOW, " ");
		}
		break;

	default :  // basic
		{
			success = success && print_advanced(row++, 2, CLEAR_LINE | YELLOW, "q - stop current mode / return");
			success = success && print_advanced(row++, 2, CLEAR_LINE | YELLOW, "SPACE - redraw screen");
			success = success && print_advanced(row++, 2, CLEAR_LINE | YELLOW, "z - toggle zero / normal mode (set zero to current value)");
			success = success && print_advanced(row++, 2, CLEAR_LINE | YELLOW, "l - start / stop logging");
			success = success && print_advanced(row++, 2, CLEAR_LINE | YELLOW, "n - set number of samples per average");
			success = success && print_advanced(row++, 2, CLEAR_LINE | YELLOW, " ");
		}
		break;
	}
	return success;
}

bool Terminal::welcome()
{
	bool success = true;
	success = success && print_advanced(1, 33, CLEAR_SCREEN | BOLD | UNDERLINE | BRIGHT | YELLOW, "Multimeter ST");
	success = success && print_advanced(2, 37, BRIGHT | YELLOW, version);
	return success;
}

bool Terminal::print_setup()
{
	const size_t TERMINAL_WIDTH = 80;
	char buffer[TERMINAL_WIDTH + 1];
	buffer[TERMINAL_WIDTH] = '\0';
	snprintf(buffer, TERMINAL_WIDTH, "%5d", AvgFilter::get_no_samples());
	uint8_t row = 0;

	bool success = true;
	row = 3;

	success = success && print_advanced(row, 2, CLEAR_LINE | YELLOW, "Application:");
	success = success && print_advanced(row, 15, BRIGHT | BOLD | YELLOW, "Voltmeter");
	success = success && print_advanced(row++, 66, YELLOW, "VDDA:");
	success = success && print_advanced(row, 2, CLEAR_LINE | YELLOW, "Sample frequency:     Hz");
	success = success && print_advanced(row, 20, BRIGHT | BOLD | YELLOW, "100");
	success = success && print_advanced(row++, 66, YELLOW, "TEMP:");
	success = success && print_advanced(row, 2, CLEAR_LINE | YELLOW, "Samples per average:");
	success = success && print_advanced(row, 22, BRIGHT | BOLD | YELLOW, buffer);

	row = 16;

	success = success && print_advanced(row, 2, CLEAR_LINE | YELLOW, "Application:");
	success = success && print_advanced(row, 15, BRIGHT | BOLD | YELLOW, "Generator");
	success = success && print_advanced(row, 2, CLEAR_LINE | YELLOW, "PWM1:");
	success = success && print_advanced(row, 10, YELLOW, "frequency:        Hz");
	success = success && print_advanced(row++, 33, YELLOW, "duty cycle:     %");
	success = success && print_advanced(row, 2, CLEAR_LINE | YELLOW, "PWM2:");
	success = success && print_advanced(row, 10, YELLOW, "frequency:        Hz");
	success = success && print_advanced(row++, 33, YELLOW, "duty cycle:     %");

	return success;
}

void Terminal::update_voltmeter()
{
	if ((m_voltmeter_no_samples_mode == false) && (m_no_from_keybord > 0))
	{
		AvgFilter::set_no_samples(m_no_from_keybord);
		print_setup();
		update_generator();
		m_voltmeter_no_samples_mode = false;
		m_read_int = false;
		m_no_from_keybord = 0;
	}

	uint16_t is_new_average = 0;
	for (int ch=0; ch<ADC_CHANNELS; ch++)
	{
		if (avgf[ch].is_new_average()) is_new_average++;
	}

	if (is_new_average == ADC_CHANNELS)
	{
		// number of user ADC voltmeter channels
		int user_volt_channels = sizeof(Channel_ordered);
		// number of user ADC voltmeter differential channels
		int user_diff_channels = user_volt_channels - 1;

		// read and convert actual ADC values here
		float avg[ADC_CHANNELS];
		float diff[user_diff_channels];

		const size_t TERMINAL_WIDTH = 80;
		char buffer_avg[TERMINAL_WIDTH + 1] = " ";
		buffer_avg[TERMINAL_WIDTH] = '\0';
		char buffer_diff[TERMINAL_WIDTH + 1] = " ";
		buffer_diff[TERMINAL_WIDTH] = '\0';
		const char * formatstring;
		char aux_buffer[TERMINAL_WIDTH + 1];
		aux_buffer[TERMINAL_WIDTH] = '\0';


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
		// apply difference
		for (int ch=0; ch<user_diff_channels; ch++)
		{
			diff[ch] = compute_Vdiff(avg[Channel_ordered[ch + 1]], avg[Channel_ordered[ch]]);
		}

		// apply function zero (optional) and compute values for print
		if (m_voltmeter_zero_mode)
		{
			if (m_voltmeter_zero_mode_avg_update)
			{
				m_voltmeter_zero_mode_avg_update = false;
				for (int ch=0; ch<user_volt_channels; ch++)
				{
					m_zero_avg[Channel_ordered[ch]] = avg[Channel_ordered[ch]];
				}
				for (int ch=0; ch<user_diff_channels; ch++)
				{
					m_zero_diff_avg[ch] = diff[ch];
				}
			}
			aux_buffer[0] = ' ';
			for (int ch=0; ch<user_volt_channels; ch++)
			{
				char aux_string[27] = "";
				strcat(aux_string, adc_ch_names[Channel_ordered[ch]]);
				strcat(aux_string, ": %6.3f V     ");
				formatstring = (m_voltmeter_logging) ? "%6.3f," : aux_string;
				snprintf(aux_buffer, TERMINAL_WIDTH, formatstring, avg[Channel_ordered[ch]] - m_zero_avg[Channel_ordered[ch]]);
				if ((m_voltmeter_logging) && (ch == (user_volt_channels - 1)))
					{
						strcat(aux_buffer, "\n\r");
					}
				strcat(buffer_avg, aux_buffer);
			}
			aux_buffer[0] = ' ';
			for (int ch=0; ch<user_diff_channels; ch++)
			{
				char aux_string[27] = "";
				strcat(aux_string, adc_ch_names[Channel_ordered[ch + 1]]);
				strcat(aux_string, "-");
				strcat(aux_string, adc_ch_names[Channel_ordered[ch]]);
				strcat(aux_string, ": %6.3f V     ");
				formatstring = (m_voltmeter_logging) ? "%6.3f," : aux_string;
				snprintf(aux_buffer, TERMINAL_WIDTH, formatstring, diff[ch] - m_zero_diff_avg[ch]);
				if ((m_voltmeter_logging) && (ch == (user_diff_channels - 1)))
					{
						strcat(aux_buffer, "\n\r");
					}
				strcat(buffer_diff, aux_buffer);
			}
		}
		else
		{
			aux_buffer[0] = ' ';
			for (int ch=0; ch<user_volt_channels; ch++)
			{
				char aux_string[27] = "";
				strcat(aux_string, adc_ch_names[Channel_ordered[ch]]);
				strcat(aux_string, ": %6.3f V     ");
				formatstring = (m_voltmeter_logging) ? "%6.3f," : aux_string;
				snprintf(aux_buffer, TERMINAL_WIDTH, formatstring, avg[Channel_ordered[ch]]);
				if ((m_voltmeter_logging) && (ch == (user_volt_channels - 1)))
					{
						strcat(aux_buffer, "\n\r");
					}
				strcat(buffer_avg, aux_buffer);
			}
			aux_buffer[0] = ' ';
			for (int ch=0; ch<user_diff_channels; ch++)
			{
				char aux_string[27] = "";
				strcat(aux_string, adc_ch_names[Channel_ordered[ch + 1]]);
				strcat(aux_string, "-");
				strcat(aux_string, adc_ch_names[Channel_ordered[ch]]);
				strcat(aux_string, ": %6.3f V     ");
				formatstring = (m_voltmeter_logging) ? "%6.3f," : aux_string;
				snprintf(aux_buffer, TERMINAL_WIDTH, formatstring, diff[ch]);
				if ((m_voltmeter_logging) && (ch == (user_diff_channels - 1)))
					{
						strcat(aux_buffer, "\n\r");
					}
				strcat(buffer_diff, aux_buffer);
			}
		}

		if (m_voltmeter_logging)
		{
			send(buffer_avg);
		}
		else
		{
			print_advanced(7, 1, CLEAR_LINE | BOLD | BRIGHT | CYAN, buffer_avg);
			print_advanced(8, 1, CLEAR_LINE | BOLD | BRIGHT | CYAN, buffer_diff);
			snprintf(buffer_avg, TERMINAL_WIDTH, "%6.3f V ", avg[CHANNEL_VDDA]);
			print_advanced(3, 72, CYAN, buffer_avg);
			snprintf(buffer_avg, TERMINAL_WIDTH, "%4.1f *C ", avg[CHANNEL_TEMP]);
			print_advanced(4, 73, CYAN, buffer_avg);
			// set cursor to the bottom right corner
			print_advanced(24, 80, 0, "");
		}
	}
}

void Terminal::update_generator()
{
	if (m_voltmeter_logging == false)
	{
		const size_t TERMINAL_WIDTH = 80;
		char buffer[TERMINAL_WIDTH + 1];
		buffer[TERMINAL_WIDTH] = '\0';
		const char * formatstring;

		uint16_t duty1 = 333;
		uint16_t duty2 = 666;
		uint32_t freq = 2000;

		formatstring = "%5d";
		snprintf(buffer, TERMINAL_WIDTH, formatstring, pwm_get_freq());
		print_advanced(23, 25, BOLD | BRIGHT | YELLOW, buffer);

		pwm_set_duty(CHANNEL_PWM1, duty1);
		pwm_set_duty(CHANNEL_PWM2, duty2);
		pwm_set_freq(freq);

		formatstring = "%7d";
		snprintf(buffer, TERMINAL_WIDTH, formatstring, freq);
		print_advanced(16, 20, BOLD | BRIGHT | YELLOW, buffer);
		snprintf(buffer, TERMINAL_WIDTH, formatstring, freq);
		print_advanced(17, 20, BOLD | BRIGHT | YELLOW, buffer);

		formatstring = "%3d";
		snprintf(buffer, TERMINAL_WIDTH, formatstring, duty1);
		print_advanced(16, 45, BOLD | BRIGHT | YELLOW, buffer);
		snprintf(buffer, TERMINAL_WIDTH, formatstring, duty2);
		print_advanced(17, 45, BOLD | BRIGHT | YELLOW, buffer);

		formatstring = "%5d";
		snprintf(buffer, TERMINAL_WIDTH, formatstring, freq);
		print_advanced(23, 45, BOLD | BRIGHT | YELLOW, buffer);

		print_advanced(24, 80, 0, "");
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
			case ' ' :  // SPACE
				{
					if (m_voltmeter_logging == false)
					{
						valid_key = true;
						m_redraw_screen = true;
					}
				}
				break;

			case 'Q' :
			case 'q' :
				{
					valid_key = true;
					if (m_voltmeter_logging)
					{
						m_voltmeter_logging = false;
						m_redraw_screen = true;
						set_status("Voltmeter logging finished.");
					}
					else if (m_voltmeter_no_samples_mode)
					{
						m_voltmeter_no_samples_mode = false;
						m_read_int = false;
						m_no_from_keybord = 0;
						m_from_keyboard_message = nullptr;
						set_status("Number of samples per average was NOT set.");
					}
					else if (m_voltmeter_zero_mode)  // || m_voltmeter_diff_mode
					{
						m_voltmeter_zero_mode = false;
//						m_voltmeter_diff_mode = false;
						set_status("Voltmeter switched to default mode.");
					}
					else
					{
						// TODO: Leave Voltmeter -> main menu
						valid_key = false;
//						set_status("Unsupported key pressed!");
					}
				}
				break;

			case 'L' :
			case 'l' :
				{
					if (m_voltmeter_no_samples_mode == false)
					{
						valid_key = true;
						m_voltmeter_logging = !m_voltmeter_logging;
						m_redraw_screen = true;
						if (m_voltmeter_logging == false)
						{
							set_status("Voltmeter logging finished.");
						}
					}
				}
				break;

//			case 'D' :
//			case 'd' :
//				{
//					if ((m_voltmeter_logging == false) && (m_voltmeter_no_samples_mode == false))
//					{
//						valid_key = true;
//						m_voltmeter_diff_mode = !m_voltmeter_diff_mode;
//						m_voltmeter_zero_mode = false;
//						if (m_voltmeter_diff_mode)
//						{
//							set_status("Voltmeter switched to differential mode.");
//						}
//						else
//						{
//							set_status("Voltmeter switched to default mode.");
//						}
//					}
//				}
//				break;

			case 'Z' :
			case 'z' :
				{
					if ((m_voltmeter_logging == false) && (m_voltmeter_no_samples_mode == false))
					{
						valid_key = true;
						m_voltmeter_zero_mode = !m_voltmeter_zero_mode;
						if (m_voltmeter_zero_mode)
						{
							m_voltmeter_zero_mode_avg_update = true;
//							if (m_voltmeter_diff_mode)
//							{
//								set_status("Voltmeter switched to zero differential mode.");
//							}
//							else
//							{
								set_status("Voltmeter switched to zero mode.");
//							}
						}
						else
						{
							m_voltmeter_zero_mode_avg_update = false;
//							if (m_voltmeter_diff_mode)
//							{
//								set_status("Voltmeter switched to differential mode.");
//							}
//							else
//							{
								set_status("Voltmeter switched to default mode.");
//							}
						}
					}
				}
				break;

			case 'N' :
			case 'n' :
				{
					if (m_voltmeter_logging == false)
					{
						valid_key = true;
						m_voltmeter_no_samples_mode = !m_voltmeter_no_samples_mode;
						m_read_int = (m_voltmeter_no_samples_mode) ? true : false;
						if (m_voltmeter_no_samples_mode)
						{
							set_status("Set number of samples per average (1 .. 9999):");
						}
						else
						{
							m_from_keyboard_message = nullptr;
							if (m_no_from_keybord < 1)
							{
								m_no_from_keybord = 1;
								set_status("Minimum number of samples per average was set (1).");
							}
							else
							{
								set_status("Number of samples per average was set.");
							}
						}
					}
				}
				break;

			case '0' :
			case '1' :
			case '2' :
			case '3' :
			case '4' :
			case '5' :
			case '6' :
			case '7' :
			case '8' :
			case '9' :
				{
					if (m_read_int)
					{
						valid_key = true;
						m_no_from_keybord = 10 * m_no_from_keybord + (key - '0');
						if (m_no_from_keybord > 9999)
						{
							set_status("Number of samples per period reached maximum (9999).");
							m_no_from_keybord = 9999;
							m_from_keyboard_message = nullptr;
						}
						else
						{
							const size_t TERMINAL_WIDTH = 80;
							static char buffer[TERMINAL_WIDTH + 1];
							buffer[TERMINAL_WIDTH] = '\0';
							snprintf(buffer, TERMINAL_WIDTH, "%5d", m_no_from_keybord);
							set_status("Set number of samples per average (1 .. 9999):");
							set_from_keyboard(buffer);
						}
					}
				}
				break;

			default:
//				set_status("Unsupported key pressed!");
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
		print_advanced(24, 2, CLEAR_LINE | YELLOW, m_status_message);
	}
	else
	{
		print_advanced(24, 2, CLEAR_LINE | YELLOW, "");
	}
}

void Terminal::set_from_keyboard(const char * message)
{
	m_from_keyboard_message = message;
}

void Terminal::print_from_keyboard()
{
	if (m_from_keyboard_message != nullptr)
	{
		print_advanced(24, 50, BRIGHT |BOLD |RED, m_from_keyboard_message);
	}
}


// Called when buffer is completely filled
void Terminal::adc_callback() {
	for (int ch=0; ch<ADC_CHANNELS; ch++)
	{
		int16_t sample = adc_get_sample_mV(ch);
		avgf[ch].update(sample);
	}
}



