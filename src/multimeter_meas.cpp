/*
 * multimeter_meas.cpp
 *
 *  Created on: Jul 17, 2021
 *      Author: zuzhanka
 */


/* Includes ------------------------------------------------------------------*/

#include "avg_filter.hpp"
#include "bsp.hpp"
#include "calculations.hpp"
#include "meas_main.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>

/* Variables ---------------------------------------------------------*/
MeasurementMain measure = MeasurementMain();

/* Functions ---------------------------------------------------------*/
void adc_callback()
{
	measure.adc_callback();
//	(measure.loop_continues_condition()) ? (adc_start()) : (adc_stop());
}

extern "C" void multimeter_meas()
{
	(void) adc_run();
	measure.reset_loop_counter();
//	set_switch(false);
//	delay(5000);
	(void) adc_start();
//	set_switch(true);
	while (measure.loop_continues_condition()) {}
//	measure.loop();
}


