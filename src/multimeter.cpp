/* Includes ------------------------------------------------------------------*/

#include "avg_filter.hpp"
#include "bsp.hpp"
#include "calculations.hpp"
#include "communication.hpp"

#include <cstdint>
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



/* Functions ---------------------------------------------------------*/

// Called when buffer is completely filled
void adc_callback(void) {
  int16_t sample[ADC_BUF_LEN];
  for (int ch=0; ch<ADC_BUF_LEN; ch++)
  {
	  sample[ch] = convert2milivolt(adc_get_sample(ch), adc_get_sample(CHANNEL_VREFINT));
  }

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
			  float avg1 = convert_mV2V(avgf1.get());
			  float avg2 = convert_mV2V(avgf2.get());
			  float avg3 = convert_mV2V(avgf3.get());
			  float avg4 = convert_mV2V(avgf4.get());
			  float avgtemp = convert2celsius(avgftemp.get());

			  float diff21 = compute_Vdiff(avg2, avg1);
			  float diff32 = compute_Vdiff(avg3, avg2);
			  float diff43 = compute_Vdiff(avg4, avg3);

			  char buffer[250];
			  sprintf(buffer, "V1: %7.3f V     V2: %7.3f V     V3: %7.3f V     V4: %7.3f V",
					  avg1, avg2, avg3, avg4);
			  send_msg(buffer, "\x1b[3;0f\x1b[2K\x1b[36;1m", "\x1b[0m");
			  sprintf(buffer, "V2-V1: %7.3f V     V3-V2: %7.3f V     V4-V3: %7.3f V",
					  diff21, diff32, diff43);
			  send_msg(buffer, "\x1b[4;0f\x1b[2K\x1b[36;1m", "\x1b[0m");
			  sprintf(buffer, "Temp: %5.1f *C",
					  avgtemp);
			  send_msg(buffer, "\x1b[5;0f\x1b[2K\x1b[36;1m", "\x1b[0m");
		  }
	  }
}
