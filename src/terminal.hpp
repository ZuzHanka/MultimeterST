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
	bool print_help();
	bool welcome();
	void update_voltmeter();
	bool key_pressed();
	void set_status(const char * message);
	void print_status();

private:
	AvgFilter avgf[ADC_CHANNELS];
	bool m_redraw_screen = true;
	bool m_voltmeter_diff_mode = false;
	bool m_voltmeter_zero_mode = false;
	bool m_voltmeter_zero_mode_avg_update = false;
	const char * m_status_message = nullptr;
	float m_zero_avg[ADC_CHANNELS];
};

#endif // TERMINAL_HPP_
