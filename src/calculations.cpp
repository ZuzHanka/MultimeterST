#include "calculations.hpp"

#include "bsp.hpp"


float convert2celsius(int16_t value_mV) {
	float out_value = ((float) (value_mV - V25) / AVG_SLOPE) + 25.0;
	return out_value;
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
