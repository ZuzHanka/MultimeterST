/* Includes ------------------------------------------------------------------*/

#include "avg_filter.hpp"
#include "bsp.hpp"
#include "calculations.hpp"
#include "terminal.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>

/* Constants ---------------------------------------------------------*/

/* Variables ---------------------------------------------------------*/
Terminal terminal = Terminal();

/* Functions ---------------------------------------------------------*/
void adc_callback()
{
	terminal.adc_callback();
}

extern "C" void multimeter_main() {
	(void) adc_run();
	(void) pwm_run();
	(void) dac_run();

	while (1) {
		terminal.loop();
	}
}
