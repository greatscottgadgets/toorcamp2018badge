/*
 * This file is part of GreatFET (ish)
 */

#include <stddef.h>

#include <libopencm3/cm3/vector.h>
#include <libopencm3/lpc43xx/m4/nvic.h>
#include <greatfet_core.h>

#include "leds.h"
#include "tests.h"


int main(void) {
	cpu_clock_init();
	cpu_clock_pll1_max_speed();
	pin_setup();
	init_leds();
	init_tests();

	/* Do things here 
	 * Obviously there should be some logic around this
	 */
	badge_detect();
	battery_detect();
	r5_test();
	r6_test();
	mcu_test();
	capsense_test();
	jtag_test();
	program();
	verify();
	run();
	return 0;
}
