/* Includes ------------------------------------------------------------------*/

#include "avg_filter.hpp"
#include "bsp.hpp"
#include "calculations.hpp"
#include "terminal.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>

/* Constants ---------------------------------------------------------*/
const uint16_t DAC_VOLTAGE_mV = 3000;  // value set to DAC [mV]

/* Variables ---------------------------------------------------------*/
Terminal terminal = Terminal();

/* Functions ---------------------------------------------------------*/
void adc_callback()
{
	terminal.adc_callback();
}

extern "C" void multimeter_main()
{
	(void) adc_run();
	(void) pwm_run();
	(void) dac_run();

	// init
	terminal.m_process = IDLE;
	set_switch(false);
	terminal.reset_loop_counter();

	// 1 measurement round
	terminal.set_dac_mV(DAC_VOLTAGE_mV);
	delay(3000);
	terminal.m_process = READY;

	while (true)
	{
		terminal.loop();
	}
}
