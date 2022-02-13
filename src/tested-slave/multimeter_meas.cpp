/* Includes ------------------------------------------------------------------*/

#include "avg_filter.hpp"
#include "bsp.hpp"
#include "calculations.hpp"





#include "meas_main.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>

/* Variables ---------------------------------------------------------*/
MeasurementMain measure = MeasurementMain();

/* Functions ---------------------------------------------------------*/
void adc_callback()
{
	measure.adc_callback();
}

extern "C" void multimeter_meas()
{
	(void) adc_run();
	measure.reset_loop_counter();
//	(void) adc_stop();
	while (true)
	{
		measure.loop();
	}
}
