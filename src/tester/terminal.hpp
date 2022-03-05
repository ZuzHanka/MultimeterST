#ifndef TERMINAL_HPP_
#define TERMINAL_HPP_


#include "bsp.hpp"

#include <cstdint>
#include <cstring>

enum MeasuringProcess {
	IDLE,       // do nothing
	SETDAC,     // set DAC value before charging
	PRESTART,   // capacitor uncharged -> save data for print
				// next ADC callback starts charging, switch on
	CHARGING,   // charging capacitor
    			// ADC callback reached N loops -> print
	CHARGED,    // reset loop counter
	RESETDAC,   // reset DAC value after charging
	RECHARGING, // wait to recharge
	START,      // capacitor uncharged -> save data for print
	            // next ADC callback starts measuring, switch ON
	RUNNING,    // measuring N loops
	            // ADC callback reached N loops -> print
	STOP        // switch OFF
};


class Terminal
{
public:
	static const size_t TERMINAL_WIDTH = 80;
	MeasuringProcess m_process = IDLE;

	Terminal()
	{
		set_no_measurements(1);
		reset_loop_counter();
	}

	void loop();
	void set_no_measurements(uint16_t value);
	void set_recharge_loops(uint16_t value);
	void reset_loop_counter()
	{
		m_loop_counter = 0;
	}
	void set_dac_mV(uint16_t value_mV);
	void adc_callback();

protected:
	bool send(const char * message)
	{
		return terminal_transmit(message, strlen(message));
	}
	void print_measured();

private:
	uint16_t adc_samples[ADC_CHANNELS];
	uint16_t adc_samples2print1[ADC_CHANNELS];
	uint16_t adc_samples2print2[ADC_CHANNELS];
	uint16_t dac_sample2print;

	uint16_t m_dac_mV;

	uint16_t m_no_measurements = 1;
	uint16_t m_recharge_loops = 1;
	volatile uint16_t m_loop_counter;
	bool m_print_measured = false;
};

#endif // TERMINAL_HPP_
