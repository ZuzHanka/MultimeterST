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

extern "C" void multimeter_main()
{
	adc_init(ADC1, ADC_EXTERNALTRIGCONV_T1_CC3, adc_tester_channels, ADC_CONF_LENGTH(adc_tester_channels));
	dac_init();

	(void) adc_run(10000);
	(void) pwm_run();
	(void) dac_run();

	while (1) {
		terminal.loop();
	}
}
