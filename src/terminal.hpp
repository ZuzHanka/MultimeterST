#ifndef TERMINAL_HPP_
#define TERMINAL_HPP_


#include "avg_filter.hpp"
#include "bsp.hpp"

#include <cstdint>


class Terminal
{
public:
	static void loop();
	static void adc_callback();
private:
	static AvgFilter avgf[ADC_CHANNELS];

};

#endif // TERMINAL_HPP_
