#include <cstdint>

/* Function prototypes -----------------------------------------------*/

float convert2celsius(int16_t value_mV);
float convert_mV2V(int16_t value_mV);
float compute_Vdiff(float value1_V, float value2_V);

float convert2duty(uint16_t value);
uint16_t duty2intvalue(uint16_t duty);
float convert2freq(uint16_t value);
uint16_t freq2intvalue(uint16_t freq);

