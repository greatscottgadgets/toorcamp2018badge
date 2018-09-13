/*
 * This file is part of GreatFET (ish)
 */

#include <stddef.h>

#include <libopencm3/cm3/vector.h>
#include <libopencm3/lpc43xx/m4/nvic.h>
#include <greatfet_core.h>

#include "leds.h"
#include "tests.h"

enum test_state
{
	BADGE    = 0,
	BATTERY  = 1,
	R5       = 2,
    R6       = 3,
    CAPSENSE = 4,
	MCU      = 5,
	JTAG     = 7,
	PROGRAM  = 8,
	VERIFY   = 9,
	RUN      = 10
};

void pause(void) {
	delay(10000000);
}

static uint8_t state = BADGE;

void error_handler(void) {
	while(badge_detect() == PASS)
		pause();
	clear_leds();
	state = BADGE;
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
	while(1) {
		while(state == BADGE) {
			if(badge_detect() == PASS)
				state = JTAG;
			pause();
		}

		if(state == BATTERY) {
			// Detect battery and enable power it if not present
			if(battery_detect() == FAULT) {
				error_handler();
			}
			pause();
			state = R5;
		}

		if(state == R5) {
			if(r5_test() == PASS) {
				state = R6;
			} else {
				error_handler();
			}
			pause();
		}

		if(state == R6) {
			if(r6_test() == PASS) {
				state = CAPSENSE;
			} else {
				error_handler();
			}
			pause();
		}

		if(state == CAPSENSE) {
			if(capsense_test() == PASS) {
				state = MCU;
			} else {
				error_handler();
			}
			pause();
		}

		if(state == MCU) {
			if(mcu_test() == PASS) {
				state = JTAG;
			} else {
				error_handler();
			}
			pause();
		}

		if(state == JTAG) {
			if(jtag_test() == PASS) {
				state = PROGRAM;
			} else {
				error_handler();
			}
			pause();
		}

		if(state == PROGRAM) {
			if(program() == PASS) {
				state = VERIFY;
			} else {
				error_handler();
			}
			pause();
		}

		if(state == VERIFY) {
			if(verify() == PASS) {
				state = RUN;
			} else {
				error_handler();
			}
			pause();
		}

		if(state == RUN) {
			run();
			error_handler();
		}
	}
	return 0;
}
