/*
 * This file is part of GreatFET (ish)
 */

#include <stddef.h>

#include <libopencm3/cm3/vector.h>
#include <libopencm3/lpc43xx/m4/nvic.h>
#include <libopencm3/lpc43xx/scu.h>

#include <gpio.h>
#include <gpio_lpc.h>
#include <greatfet_core.h>
#include <jtag430.h>
#include "badge_firmware.h"
#include "leds.h"

void init_leds(void) {
	scu_pinmux(EUT_PRESENT, GPIO_MODE);
	scu_pinmux(EUT_NOT_PRESENT, GPIO_MODE);
	scu_pinmux(EUT_DETECT_FAULT, GPIO_MODE);

	scu_pinmux(BAT_PRESENT, GPIO_MODE);
	scu_pinmux(BAT_NOT_PRESENT, GPIO_MODE);
	scu_pinmux(BAT_DETECT_FAULT, GPIO_MODE);

	scu_pinmux(R5_PASS, GPIO_MODE);
	scu_pinmux(R5_FAIL, GPIO_MODE);
	scu_pinmux(R6_PASS, GPIO_MODE);
	scu_pinmux(R6_FAIL, GPIO_MODE);

	scu_pinmux(MCU_PASS, GPIO_MODE);
	scu_pinmux(MCU_FAIL, GPIO_MODE);

	scu_pinmux(CAPSENSE_PASS, GPIO_MODE);
	scu_pinmux(CAPSENSE_FAIL, GPIO_MODE);

	scu_pinmux(JTAG_PASS, GPIO_MODE);
	scu_pinmux(JTAG_FAIL, GPIO_MODE);

	scu_pinmux(PROGRAM_PASS, GPIO_MODE);
	scu_pinmux(PROGRAM_FAIL, GPIO_MODE);

	scu_pinmux(VERIFY_PASS, GPIO_MODE);
	scu_pinmux(VERIFY_FAIL, GPIO_MODE);

	scu_pinmux(RUN_PASS, GPIO_MODE);
	scu_pinmux(RUN_FAIL, GPIO_MODE);

	gpio_output(&eut_present);
	gpio_output(&eut_not_present);
	gpio_output(&eut_detect_fault);
	gpio_output(&bat_present);
	gpio_output(&bat_not_present);
	gpio_output(&bat_detect_fault);
	gpio_output(&r5_pass);
	gpio_output(&r5_fail);
	gpio_output(&r6_pass);
	gpio_output(&r6_fail);
	gpio_output(&mcu_pass);
	gpio_output(&mcu_fail);
	gpio_output(&capsense_pass);
	gpio_output(&capsense_fail);
	gpio_output(&jtag_pass);
	gpio_output(&jtag_fail);
	gpio_output(&program_pass);
	gpio_output(&program_fail);
	gpio_output(&verify_pass);
	gpio_output(&verify_fail);
	gpio_output(&run_pass);
	gpio_output(&run_fail);
}

void write_firmware(void) {
	// Start up JTAG
	uint8_t jtagid = jtag430_start_reset_halt();

	// ERASE MSP430 flash
	// These values are taken from GoodFET jtag430.c
	jtag430_eraseflash(0xA506,0xFFFE,0x3000,0);

	// Write firmware
	uint16_t *buffer = (uint16_t*) badge_firmware;
	jtag430_writeflash_bulk(0xF800, 1024, buffer);

	// Put in to RUN mode
	jtag430_stop();
}

int main(void) {
	cpu_clock_init();
	cpu_clock_pll1_max_speed();
	pin_setup();
	init_leds();

	/* Do things here */

	return 0;
}
