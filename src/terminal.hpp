#ifndef TERMINAL_HPP_
#define TERMINAL_HPP_


#include "avg_filter.hpp"
#include "bsp.hpp"

#include <cstdint>


class Terminal
{
public:

	Terminal()
	{
	}

	void loop();
	void adc_callback();

protected:
	enum Decoration
	{
		BLACK = 0x001E,
		RED = 0x001F,
		GREEN = 0x0020,
		YELLOW = 0x0021,
		BLUE = 0x0022,
		MAGENTA = 0x0023,
		CYAN = 0x0024,
		WHITE = 0x0025,

		BRIGHT = 0x0040,
		COLOR = 0x003F,

		BOLD = 0x0080,
		UNDERLINE = 0x0100,
		REVERSED = 0x0200,

		CLEAR_SCREEN = 0x8000,
		CLEAR_LINE = 0x4000
	};

	bool send(const char * message)
	{
		return terminal_transmit(message, strlen(message));
	}
	bool set_cursor_position(uint8_t row, uint8_t col);
	bool text_decoration(uint32_t flags);
	bool print_advanced(uint8_t row, uint8_t col, uint32_t decoration, const char * message);
	bool print_help(uint16_t help_spec);
	bool welcome();
	void update_voltmeter();
	void update_generator();
	bool key_pressed();
	void set_status(const char * message);
	void print_status();
	void set_from_keyboard(const char * message);
	void print_from_keyboard();
	bool print_voltmeter();
	bool print_generator();
//	bool print_setup();

private:
	AvgFilter avgf[ADC_CHANNELS];

	bool m_redraw_screen = true;
	bool m_application_voltmeter = false;
	bool m_application_generator = false;
	bool m_voltmeter_logging = false;
	bool m_voltmeter_zero_mode = false;
	bool m_voltmeter_zero_mode_avg_update = false;
	bool m_generator_channel_upper = true;
	bool m_generator_channel_switched = false;
	bool m_generator_freq_mode = false;
	bool m_generator_duty_mode = false;
	bool m_generator_flag_duty_nofreq = true;
	const char * m_status_message = nullptr;
	const char * m_from_keyboard_message = nullptr;
	float m_zero_avg[ADC_CHANNELS];
	float m_zero_diff_avg[ADC_CHANNELS - 1];
	bool m_voltmeter_no_samples_mode = false;
	bool m_read_int = false;
	uint16_t m_no_from_keybord = 65535;
	bool m_read_sign = false;
	bool m_generator_step_up = false;
	bool m_generator_step_down = false;
};

#endif // TERMINAL_HPP_
