#include <cstdint>

/* Function prototypes -----------------------------------------------*/

int16_t convert2milivolt(uint16_t adc_value, uint16_t adc_ref);
float convert2celsius(int16_t value_mV);
float convert_mV2V(int16_t value_mV);
float compute_Vdiff(float value1_V, float value2_V);

