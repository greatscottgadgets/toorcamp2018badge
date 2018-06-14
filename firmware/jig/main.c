/*
 * This file is part of GreatFET
 */

#include <stddef.h>

#include <libopencm3/cm3/vector.h>
#include <libopencm3/lpc43xx/m4/nvic.h>

#include <greatfet_core.h>


int main(void) {
	cpu_clock_init();
	cpu_clock_pll1_max_speed();
	pin_setup();
	rtc_init();

	/* Do things here */

	return 0;
}
