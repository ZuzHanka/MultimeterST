/* Includes ------------------------------------------------------------------*/

#include "comlink.hpp"
#include "bsp.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>

/* Constants ---------------------------------------------------------*/

/* Variables ---------------------------------------------------------*/

/* Functions ---------------------------------------------------------*/
void adc_callback()
{
	// Do nothing.
}

extern "C" void project_main()
{
	(void) adc_run();

	while (1) {
		Comlink::response_loop();
	}
}
