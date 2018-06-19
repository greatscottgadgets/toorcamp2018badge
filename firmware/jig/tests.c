/*
 * This file is part of GreatFET (ish)
 */

#include <libopencm3/lpc43xx/adc.h>
#include <libopencm3/lpc43xx/scu.h>
#include <gpio_lpc.h>

#include <jtag430.h>
#include <jtag.h>

#include"tests.h"
#include "badge_firmware.h"
#include"leds.h"


// static struct gpio_t r5_adc = GPIO(, );
static struct gpio_t r6_adc = GPIO(3, 15);
// static struct gpio_t bat_detect_fault = GPIO(, );


#define R5_TEST (P7_7)
// #define R6_TEST (P)
// #define EUT_DETECT_FAULT (P)

// Luckily all are pinmux function 0
#define GPIO_MODE SCU_GPIO_FAST | SCU_GPIO_PUP | SCU_CONF_FUNCTION0

// Initialise the pins for EUT/BAT detect and R5/E6 check
void init_tests(void) {
	// scu_pinmux(EUT_PRESENT, GPIO_MODE);
	// scu_pinmux(EUT_NOT_PRESENT, GPIO_MODE);
	// scu_pinmux(EUT_DETECT_FAULT, GPIO_MODE);
    ;
}

uint8_t badge_detect() {
	uint8_t result = FAIL;
    show_detect_result(result, badge_leds);
	return result;
}

uint8_t battery_detect() {
	uint8_t result = FAIL;
    show_detect_result(result, battery_leds);
	return result;
}

uint8_t r5_test() {
    uint8_t pins = 1;
	uint8_t clkdiv = 45;
	uint8_t clks = 0x2;
    uint8_t result = FAIL;
	uint16_t value;

    // ADC1_CR = ADC_CR_SEL((uint32_t) pins) |
    // ADC_CR_CLKDIV((uint32_t) clkdiv) |
    // ADC_CR_CLKS((uint32_t) clks) |
    // ADC_CR_PDN;
    // ADC1_CR |= ADC_CR_START(1);

    // while(!(ADC1_DR6 & ADC_DR_DONE));
    // value = (ADC1_DR6>>6) & 0x3ff;

    show_test_result(result, r5_leds);
	return result;
}

uint8_t r6_test() {
    uint8_t pins = 1;
	uint8_t clkdiv = 45;
	uint8_t clks = 0x2;
    uint8_t result = FAIL;
	uint16_t value;

    // ADC0_CR = ADC_CR_SEL((uint32_t) pins) |
    // ADC_CR_CLKDIV((uint32_t) clkdiv) |
    // ADC_CR_CLKS((uint32_t) clks) |
    // ADC_CR_PDN;
    // ADC0_CR |= ADC_CR_START(1);

    // while(!(ADC0_DR0 & ADC_DR_DONE));
    // value = (ADC0_DR0>>6) & 0x3ff;

    show_test_result(result, r6_leds);
	return result;
}

uint8_t mcu_test() {
	uint8_t result = FAIL;
    show_test_result(result, mcu_leds);
	return result;
}

uint8_t capsense_test() {
	uint8_t result = FAIL;
    show_test_result(result, capsense_leds);
	return result;
}

uint8_t jtag_test() {
    uint8_t result = FAIL;
	jtag_setup();
	uint8_t jtagid = jtag430_start_reset_halt();
	if(jtagid != 0x89) {
		// Try a second time
		jtagid = jtag430_start_reset_halt();
	}
	if(jtagid == 0x89) {
		result = PASS;
	}
    show_test_result(result, jtag_leds);
	return result;
}

uint8_t program() {
    uint8_t result = FAIL;
	// ERASE MSP430 flash
	// These values are taken from GoodFET jtag430.c
	jtag430_eraseflash(0xA506,0xFFFE,0x3000,0);

	// Write firmware
	uint16_t *buffer = (uint16_t*) badge_firmware;
	jtag430_writeflash_bulk(0xF800, 1024, buffer);
    result = PASS;
    show_test_result(result, program_leds);
	return result;
}

uint8_t verify() {
	uint8_t result = FAIL;
    show_test_result(result, verify_leds);
	return result;
}

uint8_t run() {
	// Put in to RUN mode
	jtag430_stop();
	// I don't really know how we detect success or failure here
	// So we'll succeed for now
	uint8_t result = PASS;
    show_test_result(result, run_leds);
	return result;
}
