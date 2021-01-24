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

/* Functions ---------------------------------------------------------*/

extern "C" void multimeter_main() {
	(void) adc_run();

	while (1) {
		Terminal::loop();
	}
}
