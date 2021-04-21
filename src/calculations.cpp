#include "calculations.hpp"

#include "bsp.hpp"


float convert2celsius(int16_t value_mV) {
	return adc_mV_to_Celsius(value_mV);
}

float convert_mV2V(int16_t value_mV)
{
	return value_mV / 1000.0;
}

float compute_Vdiff(float value1_V, float value2_V)
{
	float out_value = value1_V - value2_V;
	return out_value;
}

float convert2duty(uint16_t value)
{
	return ((float) value) / (COUNTER_PERIOD + 1) * 100.0;
}

uint16_t duty2intvalue(uint16_t duty)
{
	return (uint16_t) ((COUNTER_PERIOD + 1) / 100.0 * duty);
}

float convert2freq(uint16_t value)
{
	return ((float) FREQUENCY_MAX / ((float) value));
}

uint16_t freq2intvalue(uint16_t freq)
{
	return (uint16_t) (FREQUENCY_MAX / freq);
}
