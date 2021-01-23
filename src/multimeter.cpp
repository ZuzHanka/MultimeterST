/* Includes ------------------------------------------------------------------*/

#include "avg_filter.hpp"
#include "bsp.hpp"
#include "communication.hpp"

#include <cstdio>
#include <cstring>

/* Constants ---------------------------------------------------------*/

/* Variables ---------------------------------------------------------*/

// Filters
AvgFilter avgf1 = AvgFilter();
AvgFilter avgf2 = AvgFilter();
AvgFilter avgf3 = AvgFilter();
AvgFilter avgf4 = AvgFilter();
AvgFilter avgftemp = AvgFilter();


/* Function prototypes -----------------------------------------------*/

int16_t convert2volt(uint16_t adc_value, uint16_t adc_ref);
int16_t convert2celsius(int16_t volt_value);
uint32_t compute_Udiff(uint8_t channel1, uint8_t channel0);

/* Functions ---------------------------------------------------------*/

int16_t convert2volt(uint16_t adc_value, uint16_t adc_ref) {
	uint32_t out_value = ((uint32_t) adc_value * VREFINT) / (adc_ref);
	return (uint16_t) out_value;
}

int16_t convert2celsius(int16_t volt_value) {
	uint32_t out_value = ((volt_value - V25) / AVG_SLOPE) + 25;
	return (uint16_t) out_value;
}

// Called when buffer is completely filled
void adc_callback(void) {
  int16_t sample[ADC_BUF_LEN];
  for (int ch=0; ch<ADC_BUF_LEN; ch++)
  {
	  sample[ch] = convert2volt(adc_get_sample(ch), adc_get_sample(CHANNEL_VREFINT));
  }
  sample[CHANNEL_TEMP] = convert2celsius(sample[CHANNEL_TEMP]);

  avgf1.update(sample[CHANNEL_1]);
  avgf2.update(sample[CHANNEL_2]);
  avgf3.update(sample[CHANNEL_3]);
  avgf4.update(sample[CHANNEL_4]);
  avgftemp.update(sample[CHANNEL_TEMP]);
}

extern "C" void multimeter_main() {
	  (void) welcome();
	  (void) adc_run();

	  while (1) {
		  if (avgf1.is_new_average() && avgf2.is_new_average() && avgf3.is_new_average() && avgf4.is_new_average() && avgftemp.is_new_average())
		  {
			  char buffer[250];
			  sprintf(buffer, "V1: %4d mV \tV2: %4d mV \tV3: %4d mV \tV4: %4d mV \tTemp: %4d *C",
					  avgf1.get(), avgf2.get(), avgf3.get(), avgf4.get(), avgftemp.get());
			  send_msg(buffer, "\x1b[3;0f\x1b[2K\x1b[36;1m", "\x1b[0m");
		  }
	  }
}
