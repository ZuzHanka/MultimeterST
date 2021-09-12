#ifndef TERMINAL_HPP_
#define TERMINAL_HPP_


#include "avg_filter.hpp"
#include "bsp.hpp"

#include <cstdint>


enum MeasuringProcess {
	IDLE,       // do nothing
	READY,      // capacitor uncharged -> print
	START,      // next ADC callback starts measuring, switch ON
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
		set_no_measurements();
		reset_loop_counter();
	}

	void loop();
	void reset_loop_counter()
	{
		m_loop_counter = 0;
	}
	bool loop_continues_condition()
	{
		return m_loop_counter < m_no_measurements;
	}
	void set_dac_mV(uint16_t value_mV);
	void adc_callback();

protected:
	bool send(const char * message)
	{
		return terminal_transmit(message, strlen(message));
	}
	void set_no_measurements();
	void print_measured();

private:
//	AvgFilter avgf[ADC_CHANNELS];
//	AvgFilter avgf_measured[ADC_CHANNELS];
	uint16_t adc_samples[ADC_CHANNELS];
	uint16_t adc_samples2print[ADC_CHANNELS];

	uint16_t m_dac_mV;

	uint16_t m_no_measurements;
	volatile uint16_t m_loop_counter;
	bool m_print_measured = false;
};

#endif // TERMINAL_HPP_
