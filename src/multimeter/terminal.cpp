#include "terminal.hpp"

#include "bsp.hpp"
#include "calculations.hpp"

#include <cstdio>
#include <cstring>


const char version[20] = "1.2.0";

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
		if (m_application_voltmeter)
		{
			if (m_voltmeter_logging)
			{
				print_advanced(0, 0, CLEAR_SCREEN, "");
			}
			else if (m_voltmeter_no_samples_mode)
			{
				welcome();
				print_voltmeter();
				print_generator();
				print_help(11);
			}
			else
			{
				welcome();
				print_voltmeter();
				print_generator();
				print_help(1);
			}
		}
		else if (m_application_generator)
		{
			if (m_generator_dac_mode)
			{
				welcome();
				print_voltmeter();
				print_generator();
				if (m_read_int && m_read_sign)
				{
					print_help(20);
				}
				else if (m_read_int)
				{
					print_help(200);
				}
				else if (m_read_sign)
				{
					print_help(201);
				}
			}
			else if (m_generator_freq_mode)
			{
				welcome();
				print_voltmeter();
				print_generator();
				if (m_read_int && m_read_sign)
				{
					print_help(21);
				}
				else if (m_read_int)
				{
					print_help(210);
				}
				else if (m_read_sign)
				{
					print_help(211);
				}
			}
			else if (m_generator_duty_mode)
			{
				welcome();
				print_voltmeter();
				print_generator();
				if (m_read_int && m_read_sign)
				{
					print_help(22);
				}
				else if (m_read_int)
				{
					print_help(220);
				}
				else if (m_read_sign)
				{
					print_help(221);
				}
			}
			else
			{
				welcome();
				print_voltmeter();
				print_generator();
				print_help(2);
			}
		}
		else
		{
			welcome();
			print_voltmeter();
			print_generator();
			print_help(0);
		}
	}

	if (was_key_pressed && (m_voltmeter_logging == false))
	{
		print_status();
		print_from_keyboard();
		if (m_application_voltmeter)
		{
			if (m_voltmeter_no_samples_mode)
			{
				print_help(11);
			}
			else
			{
				print_help(1);
			}
		}
		else if (m_application_generator)
		{
			if (m_generator_dac_mode)
			{
				if (m_read_int && m_read_sign)
				{
					print_help(20);
				}
				else if (m_read_int)
				{
					print_help(200);
				}
				else if (m_read_sign)
				{
					print_help(201);
				}
			}
			else if (m_generator_freq_mode)
			{
				if (m_read_int && m_read_sign)
				{
					print_help(21);
				}
				else if (m_read_int)
				{
					print_help(210);
				}
				else if (m_read_sign)
				{
					print_help(211);
				}
			}
			else if (m_generator_duty_mode)
			{
				if (m_read_int && m_read_sign)
				{
					print_help(22);
				}
				else if (m_read_int)
				{
					print_help(220);
				}
				else if (m_read_sign)
				{
					print_help(221);
				}
			}
			else
			{
				print_help(2);
			}
		}
		else
		{
			print_help(0);
		}
	}

	update_voltmeter();
	update_generator();
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

bool Terminal::print_help(uint16_t help_spec)
{
	bool success = true;
	uint8_t row = 16;

	//	success = success && print_advanced(row++, 2, CLEAR_LINE | YELLOW, "");

	switch (help_spec)
	{
		case (1) :  // voltmeter active
		{
			uint16_t color = YELLOW;
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "SPACE : redraw screen");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "q : stop current mode / return");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "z : toggle zero / normal mode (set zero to current value)");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "l : start / stop logging");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "n : set number of samples per average");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
		}
		break;

		case (11) :  // voltmeter - set number of samples from keyboard
		{
			uint16_t color = YELLOW;
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "SPACE : redraw screen");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "0 .. 9 : write numbers");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "n : set number of samples per average");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "q : leave without change");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
		}
		break;

		case (2) :  // generator active
		{
			uint16_t color = YELLOW;
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "SPACE : redraw screen");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "q : stop current mode / return");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "s : switch PWM channels to edit settings");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "f : set frequency for active PWM channel");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "d : set duty cycle for active PWM channel");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "v : set DAC voltage");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "t : trigger PWM channels synchronization");
		}
		break;

		case (20) :  // generator - set DAC voltage from keyboard
		{
			uint16_t color = YELLOW;
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "SPACE : redraw screen");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "0 .. 9 : write numbers");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "+ or - : slightly modify voltage [mV]");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "v or q : stop voltage edit mode");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
		}
		break;

		case (200) :  // generator - set DAC voltage from keyboard - enter digits
		{
			uint16_t color = YELLOW;
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "SPACE : redraw screen");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "0 .. 9 : write numbers");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "v : set voltage [mV]");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "q : leave without change");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
		}
		break;

		case (201) :  // generator - set DAC voltage from keyboard - enter signs
		{
			uint16_t color = YELLOW;
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "SPACE : redraw screen");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "+ or - : slightly modify frequency");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "v or q : stop voltage edit mode");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
		}
		break;

		case (21) :  // generator - set frequency from keyboard
		{
			uint16_t color = YELLOW;
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "SPACE : redraw screen");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "0 .. 9 : write numbers");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "+ or - : slightly modify frequency");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "f or q : stop frequency mode");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
		}
		break;

		case (210) :  // generator - set frequency from keyboard - enter digits
		{
			uint16_t color = YELLOW;
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "SPACE : redraw screen");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "0 .. 9 : write numbers");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "f : set frequency");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "q : leave without change");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
		}
		break;

		case (211) :  // generator - set frequency from keyboard - enter signs
		{
			uint16_t color = YELLOW;
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "SPACE : redraw screen");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "+ or - : slightly modify frequency");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "f or q : stop frequency mode");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
		}
		break;

		case (22) :  // generator - set duty cycle from keyboard
		{
			uint16_t color = YELLOW;
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "SPACE : redraw screen");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "0 .. 9 : write numbers");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "+ or - : slightly modify duty cycle");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "d or q : stop duty cycle mode");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
		}
		break;

		case (220) :  // generator - set duty cycle from keyboard - enter digits
		{
			uint16_t color = YELLOW;
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "SPACE : redraw screen");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "0 .. 9 : write numbers");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "d : set duty cycle");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "q : leave without change");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
		}
		break;

		case (221) :  // generator - set duty cycle from keyboard - enter signs
		{
			uint16_t color = YELLOW;
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "SPACE : redraw screen");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "+ or - : slightly modify duty cycle");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "d or q : stop duty cycle mode");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
		}
		break;

		default :  // basic - voltmeter/generator inactive
		{
			uint16_t color = YELLOW;
			// success = success && print_advanced(row++, 2, CLEAR_LINE | color, "q - stop current mode / return");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "SPACE : redraw screen");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "v : select application Voltmeter");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, "g : select application Generator");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
			success = success && print_advanced(row++, 2, CLEAR_LINE | color, " ");
		}
		break;
	}
	return success;
}

bool Terminal::welcome()
{
	bool success = true;
	success = success && print_advanced(1, 30, CLEAR_SCREEN | BOLD | UNDERLINE | BRIGHT | WHITE, "Multimeter ST ");
	success = success && print_advanced(1, 44, BOLD | UNDERLINE | BRIGHT | WHITE, version);
	success = success && print_advanced(2, 34, BOLD | UNDERLINE | BRIGHT | WHITE, board_name);
	return success;
}

bool Terminal::print_voltmeter()
{
	const size_t TERMINAL_WIDTH = 80;
	char buffer[TERMINAL_WIDTH + 1];
	buffer[TERMINAL_WIDTH] = '\0';
	snprintf(buffer, TERMINAL_WIDTH, "%5d", AvgFilter::get_no_samples());
	uint8_t row = 0;

	bool success = true;
	row = 3;
	uint16_t color = (m_application_voltmeter) ? (YELLOW) : (WHITE);

	success = success && print_advanced(row, 2, CLEAR_LINE | color, "Application:");
	success = success && print_advanced(row, 15, BRIGHT | BOLD | color, "Voltmeter");
	success = success && print_advanced(row++, 66, WHITE, "VDDA:");
	success = success && print_advanced(row, 2, CLEAR_LINE | color, "Sample frequency:     Hz");
	success = success && print_advanced(row, 20, BRIGHT | BOLD | color, "100");
	success = success && print_advanced(row++, 66, WHITE, "TEMP:");
	success = success && print_advanced(row, 2, CLEAR_LINE | color, "Samples per average:");
	success = success && print_advanced(row, 22, BRIGHT | BOLD | color, buffer);

	return success;
}

bool Terminal::print_generator()
{
	const size_t TERMINAL_WIDTH = 80;
	char buffer[TERMINAL_WIDTH + 1];
	buffer[TERMINAL_WIDTH] = '\0';
	uint8_t row = 0;

	bool success = true;
	row = 10;
	uint16_t color = (m_application_generator) ? (YELLOW) : (WHITE);
	const char * ch_active = (m_generator_channel_upper) ? (pwm_ch_names[CHANNEL_PWM1]) : (pwm_ch_names[CHANNEL_PWM2]);

	success = success && print_advanced(row, 2, CLEAR_LINE | color, "Application:");
	success = success && print_advanced(row++, 15, BRIGHT | BOLD | color, "Generator");

	success = success && print_advanced(row, 2, CLEAR_LINE | color, "Channel to edit:");
	success = success && print_advanced(row++, 20, BRIGHT | BOLD | color, ch_active);

	for (int ch=0; ch<CHANNEL_PWM_COUNT; ch++)
	{
		success = success && print_advanced(row, 2, CLEAR_LINE | BRIGHT | BOLD | CYAN, pwm_ch_names[ch]);
		success = success && print_advanced(row, 1 + sizeof(pwm_ch_names[ch]), BRIGHT | BOLD | CYAN, ":");
		success = success && print_advanced(row, 10, BRIGHT | BOLD | CYAN, "PWM");

		snprintf(buffer, TERMINAL_WIDTH, "%12.3f", pwm_get_freq(ch));
		success = success && print_advanced(row, 14, BRIGHT | BOLD | CYAN, buffer);
		success = success && print_advanced(row, 14 + 13, BRIGHT | BOLD | CYAN, "Hz");

		snprintf(buffer, TERMINAL_WIDTH, "%7.3f", pwm_get_duty(ch));
		success = success && print_advanced(row, 33, BRIGHT | BOLD | CYAN, buffer);
		success = success && print_advanced(row++, 33 + 8, BRIGHT | BOLD | CYAN, "%");
	}

	for (int ch=0; ch<CHANNEL_DAC_COUNT; ch++)
	{
		success = success && print_advanced(row, 2, CLEAR_LINE | BRIGHT | BOLD | CYAN, dac_ch_names[ch]);
		success = success && print_advanced(row, 1 + sizeof(dac_ch_names[ch]), BRIGHT | BOLD | CYAN, ":");
		success = success && print_advanced(row, 10, BRIGHT | BOLD | CYAN, "DAC");

		snprintf(buffer, TERMINAL_WIDTH, "%5.3f", convert_mV2V(m_dac_mV));
		success = success && print_advanced(row, 21, BRIGHT | BOLD | CYAN, buffer);
		success = success && print_advanced(row++, 21 + 6, BRIGHT | BOLD | CYAN, "V");
	}

	return success;
}

//bool Terminal::print_setup()
//{
//	const size_t TERMINAL_WIDTH = 80;
//	char buffer[TERMINAL_WIDTH + 1];
//	buffer[TERMINAL_WIDTH] = '\0';
//	snprintf(buffer, TERMINAL_WIDTH, "%5d", AvgFilter::get_no_samples());
//	uint8_t row = 0;
//
//	bool success = true;
//	row = 3;
//
//	success = success && print_advanced(row, 2, CLEAR_LINE | YELLOW, "Application:");
//	success = success && print_advanced(row, 15, BRIGHT | BOLD | YELLOW, "Voltmeter");
//	success = success && print_advanced(row++, 66, YELLOW, "VDDA:");
//	success = success && print_advanced(row, 2, CLEAR_LINE | YELLOW, "Sample frequency:     Hz");
//	success = success && print_advanced(row, 20, BRIGHT | BOLD | YELLOW, "100");
//	success = success && print_advanced(row++, 66, YELLOW, "TEMP:");
//	success = success && print_advanced(row, 2, CLEAR_LINE | YELLOW, "Samples per average:");
//	success = success && print_advanced(row, 22, BRIGHT | BOLD | YELLOW, buffer);
//
//	row = 17;
//
//	success = success && print_advanced(row, 2, CLEAR_LINE | YELLOW, "Application:");
//	success = success && print_advanced(row++, 15, BRIGHT | BOLD | YELLOW, "Generator");
//	success = success && print_advanced(row, 2, CLEAR_LINE | YELLOW, "PWM1:");
//	success = success && print_advanced(row, 10, YELLOW, "frequency:        Hz");
//	success = success && print_advanced(row++, 33, YELLOW, "duty cycle:     %");
//	success = success && print_advanced(row, 2, CLEAR_LINE | YELLOW, "PWM2:");
//	success = success && print_advanced(row, 10, YELLOW, "frequency:        Hz");
//	success = success && print_advanced(row++, 33, YELLOW, "duty cycle:     %");
//
//	return success;
//}

void Terminal::update_voltmeter()
{
	if ((m_application_voltmeter) && (m_voltmeter_no_samples_mode == false) && (m_no_from_keybord < MAX_NO_FROM_KEYBOARD))
	{
		AvgFilter::set_no_samples(m_no_from_keybord);
		print_voltmeter();
		m_voltmeter_no_samples_mode = false;
		m_read_int = false;
		m_no_from_keybord = MAX_NO_FROM_KEYBOARD;
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
						strcat(aux_buffer, "\n");
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
						strcat(aux_buffer, "\n");
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
						strcat(aux_buffer, "\n");
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
						strcat(aux_buffer, "\n");
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
		if ((m_application_generator) && (m_no_from_keybord < MAX_NO_FROM_KEYBOARD) && (m_read_int)
				&& (m_generator_duty_mode == false) && (m_generator_freq_mode == false)
				&& (m_generator_dac_mode == false))
		{
			if (m_generator_flag_voltage)
			{
				m_dac_mV = m_no_from_keybord;
				m_generator_flag_voltage = false;
			}
			else
			{
				if (m_generator_flag_duty_nofreq)
				{
					if (m_generator_channel_upper)
					{
						pwm_set_duty(CHANNEL_PWM1, m_no_from_keybord);
					}
					else
					{
						pwm_set_duty(CHANNEL_PWM2, m_no_from_keybord);
					}
				}
				else
				{
					if (m_generator_channel_upper)
					{
						pwm_set_freq(CHANNEL_PWM1, m_no_from_keybord);
					}
					else
					{
						pwm_set_freq(CHANNEL_PWM2, m_no_from_keybord);
					}
				}
			}
			m_read_int = false;
			m_no_from_keybord = MAX_NO_FROM_KEYBOARD;
			print_generator();
		}

		if ((m_application_generator) && ((m_generator_step_up) || (m_generator_step_down)))
		{
			if (m_generator_flag_voltage)
			{
				if ((m_generator_step_up) && (m_dac_mV < 3300))
				{
					m_dac_mV++;
					dac_set_value(m_dac_mV);
				}
				if ((m_generator_step_down) && (m_dac_mV > 0))
				{
					m_dac_mV--;
					dac_set_value(m_dac_mV);
				}
			}
			else
			{
				if (m_generator_flag_duty_nofreq)
				{
					if (m_generator_channel_upper)
					{
						float value = pwm_get_duty(CHANNEL_PWM1);
						if ((m_generator_step_up) && (value < 100))
						{
							pwm_duty_increment(CHANNEL_PWM1, 1);
						}
						if ((m_generator_step_down) && (value > 0))
						{
							pwm_duty_increment(CHANNEL_PWM1, -1);
						}
					}
					else
					{
						float value = pwm_get_duty(CHANNEL_PWM2);
						if ((m_generator_step_up) && (value < 100))
						{
							pwm_duty_increment(CHANNEL_PWM2, 1);
						}
						if ((m_generator_step_down) && (value > 0))
						{
							pwm_duty_increment(CHANNEL_PWM2, -1);
						}
					}
				}
				else
				{
					if (m_generator_channel_upper)
					{
						float value = pwm_get_freq(CHANNEL_PWM1);
						if ((m_generator_step_up) && (value < 1000000))
						{
							pwm_freq_increment(CHANNEL_PWM1, 1);
						}
						if ((m_generator_step_down) && (value > 1))
						{
							pwm_freq_increment(CHANNEL_PWM1, -1);
						}
					}
					else
					{
						float value = pwm_get_freq(CHANNEL_PWM2);
						if ((m_generator_step_up) && (value < 1000000))
						{
							pwm_freq_increment(CHANNEL_PWM2, 1);
						}
						if ((m_generator_step_down) && (value > 1))
						{
							pwm_freq_increment(CHANNEL_PWM2, -1);
						}
					}
				}
			}
			m_generator_step_up = false;
			m_generator_step_down = false;
			print_generator();
		}

		if (m_generator_channel_switched)
		{
			m_generator_channel_switched = false;
			print_generator();
		}

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
						m_no_from_keybord = MAX_NO_FROM_KEYBOARD;
						m_from_keyboard_message = nullptr;
						set_status("Number of samples per average was NOT set.");
					}
					else if (m_voltmeter_zero_mode)
					{
						m_voltmeter_zero_mode = false;
						set_status("Voltmeter switched to default mode.");
					}
					else if (m_generator_dac_mode)
					{
						m_generator_dac_mode = false;
						m_read_int = false;
						m_no_from_keybord = MAX_NO_FROM_KEYBOARD;
						m_from_keyboard_message = nullptr;
						if (m_read_sign)
						{
							m_read_sign = false;
							set_status("Voltage edit mode finished.");
						}
						else
						{
							set_status("Voltage was NOT set.");
						}
					}
					else if (m_generator_freq_mode)
					{
						m_generator_freq_mode = false;
						m_read_int = false;
						m_no_from_keybord = MAX_NO_FROM_KEYBOARD;
						m_from_keyboard_message = nullptr;
						if (m_read_sign)
						{
							m_read_sign = false;
							set_status("Frequency mode finished.");
						}
						else
						{
							set_status("Frequency was NOT set.");
						}
					}
					else if (m_generator_duty_mode)
					{
						m_generator_duty_mode = false;
						m_read_int = false;
						m_no_from_keybord = MAX_NO_FROM_KEYBOARD;
						m_from_keyboard_message = nullptr;
						if (m_read_sign)
						{
							m_read_sign = false;
							set_status("Duty cycle mode finished.");
						}
						else
						{
							set_status("Duty cycle was NOT set.");
						}
					}
					else if (m_application_voltmeter)
					{
						m_application_voltmeter = false;
						m_redraw_screen = true;
						set_status("Application Voltmeter left.");
					}
					else if (m_application_generator)
					{
						m_application_generator = false;
						m_redraw_screen = true;
						set_status("Application Generator left.");
					}
					else
					{
						// TODO: Leave Voltmeter -> main menu
						valid_key = false;
//						set_status("Unsupported key pressed!");
					}
				}
				break;

			case 'G' :
			case 'g' :
			{
				if ((m_application_generator == false) && (m_application_voltmeter == false))
				{
					valid_key = true;
					m_application_generator = true;
					m_redraw_screen = true;
					set_status("Application Generator selected.");
				}
				break;
			}

			case 'V' :
			case 'v' :
			{
				if ((m_application_generator == false) && (m_application_voltmeter == false))
				{
					valid_key = true;
					m_application_voltmeter = true;
					m_redraw_screen = true;
					set_status("Application Voltmeter selected.");
				}
				if ((m_application_generator) && (m_generator_freq_mode == false) && (m_generator_duty_mode == false))
				{
					valid_key = true;
					m_generator_dac_mode = !m_generator_dac_mode;
					if (m_generator_dac_mode)
					{
						m_read_int = true;
						m_read_sign = true;
						m_no_from_keybord = 0;
						m_generator_flag_voltage = true;
						set_status("Set voltage in mV (0 .. 3300):");
					}
					else
					{
						if (m_no_from_keybord < 0)
						{
							m_no_from_keybord = 0;
							set_status("Minimum voltage was set.");
						}
						else if (m_read_sign)
						{
							m_read_int = false;
							m_no_from_keybord = MAX_NO_FROM_KEYBOARD;
							set_status("Voltage edit mode finished.");
						}
						else
						{
							set_status("Voltage was set.");
						}
						m_read_sign = false;
						m_from_keyboard_message = nullptr;
					}
				}
				break;
			}

			case 'L' :
			case 'l' :
				{
					if ((m_application_voltmeter) && (m_voltmeter_no_samples_mode == false))
					{
						valid_key = true;
						m_voltmeter_logging = !m_voltmeter_logging;
						m_redraw_screen = true;
						if (m_voltmeter_logging == false)
						{
							set_status("Voltmeter logging finished.");
						}
						else
						{
							send("\n");
						}
					}
				}
				break;

			case 'Z' :
			case 'z' :
				{
					if ((m_application_voltmeter) && (m_voltmeter_logging == false) && (m_voltmeter_no_samples_mode == false))
					{
						valid_key = true;
						m_voltmeter_zero_mode = !m_voltmeter_zero_mode;
						if (m_voltmeter_zero_mode)
						{
							m_voltmeter_zero_mode_avg_update = true;
								set_status("Voltmeter switched to zero mode.");
						}
						else
						{
							m_voltmeter_zero_mode_avg_update = false;
								set_status("Voltmeter switched to default mode.");
						}
					}
				}
				break;

			case 'N' :
			case 'n' :
				{
					if ((m_application_voltmeter) && (m_voltmeter_logging == false))
					{
						valid_key = true;
						m_voltmeter_no_samples_mode = !m_voltmeter_no_samples_mode;
						m_read_int = (m_voltmeter_no_samples_mode) ? true : false;
						if (m_voltmeter_no_samples_mode)
						{
							m_no_from_keybord = 0;
							set_status("Set number of samples per average (1 .. 1000):");
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

			case 'S' :
			case 's' :
				{
					if ((m_application_generator) && (m_generator_dac_mode == false)
							&& (m_generator_duty_mode == false) && (m_generator_freq_mode == false))
					{
						valid_key = true;
						m_generator_channel_upper = !m_generator_channel_upper;
						m_generator_channel_switched = true;
						set_status("Active channel changed.");
					}
				}
				break;

			case 'D' :
			case 'd' :
				{
					if ((m_application_generator) && (m_generator_freq_mode == false) && (m_generator_dac_mode == false))
					{
						valid_key = true;
						m_generator_duty_mode = !m_generator_duty_mode;
						if (m_generator_duty_mode)
						{
							m_read_int = true;
							m_read_sign = true;
							m_no_from_keybord = 0;
							m_generator_flag_duty_nofreq = true;
							set_status("Set duty cycle for edited channel (0 .. 100):");
						}
						else
						{
							if (m_no_from_keybord < 0)
							{
								m_no_from_keybord = 0;
								set_status("Minimum duty cycle was set.");
							}
							else if (m_read_sign)
							{
								m_read_int = false;
								m_no_from_keybord = MAX_NO_FROM_KEYBOARD;
								set_status("Duty cycle mode finished.");
							}
							else
							{
								set_status("Duty cycle was set.");
							}
							m_read_sign = false;
							m_from_keyboard_message = nullptr;
						}
					}
				}
				break;

			case 'F' :
			case 'f' :
				{
					if ((m_application_generator) && (m_generator_duty_mode == false) && (m_generator_dac_mode == false))
					{
						valid_key = true;
						m_generator_freq_mode = !m_generator_freq_mode;
						if (m_generator_freq_mode)
						{
							m_read_int = true;
							m_read_sign = true;
							m_no_from_keybord = 0;
							m_generator_flag_duty_nofreq = false;
							set_status("Set frequency for edited channel (1 .. 1000000):");
						}
						else
						{
							if (m_no_from_keybord < 1)
							{
								m_no_from_keybord = 1;
								set_status("Minimum frequency from keyboard was set.");
							}
							else if (m_read_sign)
							{
								m_read_int = false;
								m_no_from_keybord = MAX_NO_FROM_KEYBOARD;
								set_status("Frequency mode finished.");
							}
							else
							{
								set_status("Frequency was set.");
							}
							m_read_sign = false;
							m_from_keyboard_message = nullptr;
						}
					}
				}
				break;

			case 'T' :
			case 't' :
			{
				if ((m_application_generator) && (m_generator_freq_mode == false) && (m_generator_duty_mode == false))
				{
					valid_key = true;
					pwm_synchronize();
					set_status("PWM channels synchronized.");
				}
				break;
			}

			case '+' :
				{
					if ((m_application_generator) && (m_read_sign))
					{
						valid_key = true;
						m_read_int = false;
						m_generator_step_up = true;
						if (m_generator_dac_mode)
						{
							set_status("Voltage slightly increased.");
						}
						if (m_generator_duty_mode)
						{
							set_status("Duty cycle slightly increased.");
						}
						if (m_generator_freq_mode)
						{
							set_status("Frequency slightly increased.");
						}
					}
				}
				break;

			case '-' :
				{
					if ((m_application_generator) && (m_read_sign))
					{
						valid_key = true;
						m_read_int = false;
						m_generator_step_down = true;
						if (m_generator_dac_mode)
						{
							set_status("Voltage slightly decreased.");
						}
						if (m_generator_duty_mode)
						{
							set_status("Duty cycle slightly decreased.");
						}
						if (m_generator_freq_mode)
						{
							set_status("Frequency slightly decreased.");
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
					if ((m_application_voltmeter) && (m_read_int))
					{
						valid_key = true;
						m_no_from_keybord = 10 * m_no_from_keybord + (key - '0');
						if (m_no_from_keybord > 1000)
						{
							set_status("Number of samples per period reached maximum (1000).");
							m_no_from_keybord = 1000;
							m_from_keyboard_message = nullptr;
						}
						else
						{
							const size_t TERMINAL_WIDTH = 80;
							static char buffer[TERMINAL_WIDTH + 1];
							buffer[TERMINAL_WIDTH] = '\0';
							snprintf(buffer, TERMINAL_WIDTH, "%5ld", m_no_from_keybord);
							set_status("Set number of samples per average (1 .. 1000):");
							set_from_keyboard(buffer);
						}
					}
					if ((m_application_generator) && (m_read_int))
					{
						valid_key = true;
						m_read_sign = false;
						m_no_from_keybord = 10 * m_no_from_keybord + (key - '0');
						if ((m_generator_duty_mode) && (m_no_from_keybord > 100))
						{
							set_status("Duty cycle reached maximum (100).");
							m_no_from_keybord = 100;
							m_from_keyboard_message = nullptr;
						}
						else if ((m_generator_freq_mode) && (m_no_from_keybord > 1000000))
						{
							set_status("Frequency reached maximum (1000000).");
							m_no_from_keybord = 1000000;
							m_from_keyboard_message = nullptr;
						}
						else if ((m_generator_dac_mode) && (m_no_from_keybord > 3300))
						{
							set_status("Voltage [mV] reached maximum (3300).");
							m_no_from_keybord = 3300;
							m_from_keyboard_message = nullptr;
						}
						else
						{
							const size_t TERMINAL_WIDTH = 80;
							static char buffer[TERMINAL_WIDTH + 1];
							buffer[TERMINAL_WIDTH] = '\0';
							snprintf(buffer, TERMINAL_WIDTH, "%8ld", m_no_from_keybord);
							if (m_generator_duty_mode)
							{
								set_status("Set duty cycle for edited channel (0 .. 100):");
							}
							if (m_generator_freq_mode)
							{
								set_status("Set frequency for edited channel (1 .. 1000000):");
							}
							if (m_generator_dac_mode)
							{
								set_status("Set voltage for DAC (0 .. 3300):");
							}
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
		print_advanced(24, 2, CLEAR_LINE | WHITE, m_status_message);
	}
	else
	{
		print_advanced(24, 2, CLEAR_LINE | WHITE, " ");
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
	dac_set_value(m_dac_mV);
}



