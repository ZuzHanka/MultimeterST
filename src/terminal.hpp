#ifndef TERMINAL_HPP_
#define TERMINAL_HPP_


#include "avg_filter.hpp"
#include "bsp.hpp"

#include <cstdint>


class Terminal
{
public:

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

	static void loop();
	static void adc_callback();

protected:
	static bool send(const char * message)
	{
		return terminal_transmit(message, strlen(message));
	}
	static bool set_cursor_position(uint8_t row, uint8_t col);
	static bool text_decoration(uint32_t flags);
	static bool print_advanced(uint8_t row, uint8_t col, uint32_t decoration, const char * message);
	static bool print_help();
	static bool welcome();

private:
	static AvgFilter avgf[ADC_CHANNELS];

	static bool m_redraw_screen;
};

#endif // TERMINAL_HPP_
