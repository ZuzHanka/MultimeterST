/* Includes ------------------------------------------------------------------*/

#include "avg_filter.hpp"
#include "bsp.hpp"
#include "calculations.hpp"
#include "terminal.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>

/* Constants ---------------------------------------------------------*/
const uint16_t DAC_VOLTAGE_MIN_mV = 0;  // minimum value set to DAC [mV]
const uint16_t DAC_VOLTAGE_MAX_mV = 300;  // maximum value set to DAC [mV]
const uint16_t DAC_VOLTAGE_STEP_mV = 100;  // step rising value set to DAC [mV]

const uint16_t NO_MEASUREMENTS = 10000;  // number of integration rounds


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
	terminal.set_no_measurements(NO_MEASUREMENTS);
	terminal.m_process = IDLE;
	set_switch(false);
	terminal.reset_loop_counter();

	uint16_t dac_voltage_mV = DAC_VOLTAGE_MIN_mV;

	while (dac_voltage_mV <= DAC_VOLTAGE_MAX_mV)
	{
		// 1 measurement round
		terminal.set_dac_mV(dac_voltage_mV);
		delay(3000);
		terminal.m_process = READY;

		while (terminal.m_process != IDLE)
		{
			terminal.loop();
		}

		dac_voltage_mV += DAC_VOLTAGE_STEP_mV;
	}
}
