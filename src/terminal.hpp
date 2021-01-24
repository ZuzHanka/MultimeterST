#ifndef TERMINAL_HPP_
#define TERMINAL_HPP_


#include "avg_filter.hpp"
#include "bsp.hpp"

#include <cstdint>


class Terminal
{
public:
	static const char BLACK[];
	static const char RED[];
	static const char GREEN[];
	static const char YELLOW[];
	static const char BLUE[];
	static const char MAGENTA[];
	static const char CYAN[];
	static const char WHITE[];

	static const char BRIGHT_BLACK[];
	static const char BRIGHT_RED[];
	static const char BRIGHT_GREEN[];
	static const char BRIGHT_YELLOW[];
	static const char BRIGHT_BLUE[];
	static const char BRIGHT_MAGENTA[];
	static const char BRIGHT_CYAN[];
	static const char BRIGHT_WHITE[];

	static const char RESET[];

	static const char BOLD[];
	static const char UNDERLINE[];
	static const char REVERSED[];

	static const char CLEAR_SCREEN[];
	// n=0 clears from cursor until end of screen,
	// n=1 clears from cursor to beginning of screen
	// n=2 clears entire screen
	static const char CLEAR_LINE[];
	// n=0 clears from cursor to end of line
	// n=1 clears from cursor to start of line
	// n=2 clears entire line

	static void loop();
	static void adc_callback();

protected:
	static bool send(const char * message)
	{
		return terminal_transmit(message, strlen(message));
	}
	static bool set_cursor_position(uint8_t row, uint8_t col);
	static bool print_advanced(uint8_t row, uint8_t col, const char * color, const char * message);

private:
	static AvgFilter avgf[ADC_CHANNELS];

};

#endif // TERMINAL_HPP_
