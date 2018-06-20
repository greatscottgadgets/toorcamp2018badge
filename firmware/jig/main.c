/*
 * This file is part of GreatFET (ish)
 */

#include <stddef.h>

#include <libopencm3/cm3/vector.h>
#include <libopencm3/lpc43xx/m4/nvic.h>
#include <greatfet_core.h>

#include "leds.h"
#include "tests.h"

void pause(void) {
	delay(10000000);
}

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
	pause();
	battery_detect();
	pause();
	// r5_test();
	pause();
	r6_test();
	pause();
	capsense_test();
	pause();
	mcu_test();
	pause();
	jtag_test();
	pause();
	program();
	pause();
	verify();
	pause();
	run();
	return 0;
}
