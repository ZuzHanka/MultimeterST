/* Includes ------------------------------------------------------------------*/

#include "avg_filter.hpp"
#include "bsp.hpp"
#include "calculations.hpp"
#include "terminal.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>

/* Constants ---------------------------------------------------------*/
const uint16_t NO_MEASUREMENTS = 10000;  // number of integration rounds

const uint16_t DAC_VOLTAGE_MIN_mV = 0;  // minimum value set to DAC [mV]
const uint16_t DAC_VOLTAGE_MAX_mV = 3200;  // maximum value set to DAC [mV]
const uint16_t DAC_VOLTAGE_STEP_mV = 100;  // step rising value set to DAC [mV]

// voltage close to 0V - reverted current
const uint16_t DAC_VOLTAGE_REVERT_mV = 500;  // max voltage [mV]
const uint16_t DAC_VOLTAGE_PRECHARGE_mV = 3000;  // precharge value set to DAC [mV]
const uint16_t DAC_RECHARGE_LOOPS = 50;  // recharge loops after voltage reset just before measuring

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

	(void) adc_run(100);
	(void) dac_run();

	// init
	terminal.set_no_measurements(NO_MEASUREMENTS);
	terminal.set_recharge_loops(DAC_RECHARGE_LOOPS);
	terminal.m_process = IDLE;
	set_switch(false);
	terminal.reset_loop_counter();

	uint16_t dac_voltage_mV = DAC_VOLTAGE_MIN_mV;

	while (dac_voltage_mV <= DAC_VOLTAGE_MAX_mV)
	{// 1 measurement round

		if (dac_voltage_mV <= DAC_VOLTAGE_REVERT_mV)
		{
			// reset capacitor
			terminal.set_dac_mV(DAC_VOLTAGE_PRECHARGE_mV);
			delay(1000);
			// prepare recharge
			terminal.m_process = SETDAC;

			// charge capacitor
			while (terminal.m_process != CHARGED)
			{
				terminal.loop();
			}

			// prepare measuring
			terminal.set_dac_mV(dac_voltage_mV);
			terminal.m_process = RESETDAC;
		}
		else
		{
			// reset capacitor
			terminal.set_dac_mV(dac_voltage_mV);
			delay(1000);
			// prepare measuring
			terminal.m_process = START;
		}

		// measure
		while (terminal.m_process != IDLE)
		{
			terminal.loop();
		}

		dac_voltage_mV += DAC_VOLTAGE_STEP_mV;
	}
}
