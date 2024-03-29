/* Includes ------------------------------------------------------------------*/

#include "meas_main.hpp"
#include "bsp.hpp"
#include "calculations.hpp"





#include <cstdint>

/* Variables ---------------------------------------------------------*/
MeasurementMain measure = MeasurementMain();

/* Functions ---------------------------------------------------------*/
void adc_callback()
{
	measure.adc_callback();

	// Uncomment to measure leakage current - stop ADC trigger timer.
//	adc_trigger_stop();
}

extern "C" void multimeter_main()
{
	adc_init(ADC1, adc_slave_channels, 1);
//	adc_init(ADC1, adc_slave_channels, ADC_CONF_LENGTH(adc_slave_channels));

	(void) adc_run(100);
	measure.reset_loop_counter();

	while (true)
	{
		measure.loop();
	}
}
