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


static struct gpio_t r5_adc = GPIO(3, 15);
static struct gpio_t r5_sply = GPIO(2, 11);
static struct gpio_t r6_sply = GPIO(2, 0);
static struct gpio_t gnd_alt = GPIO(0, 4);
// static struct gpio_t tms = GPIO(0, 15);
static struct gpio_t tms_alt = GPIO(2, 12);
static struct gpio_t vctl = GPIO(1, 5);
static struct gpio_t vtgt = GPIO(1, 4);


#define R5_ADC (P7_7) // GPIO3[15]
#define R6_ADC (ADC0) // This isn't used, mostly a comment
#define R5_SPLY (P5_2) // GPIO2[11]
#define R6_SPLY (P4_0) // GPIO2[0]
#define GND_ALT (P1_0) // GPIO0[4]
#define TMS (P1_20) // GPIO0[15]
#define TMS_ALT (P5_3) // GPIO2[12]
#define VCTL (P1_12) // GPIO1[5]
#define VTGT (P1_11) // GPIO1[4]

// Luckily all are pinmux function 0
#define GPIO_MODE SCU_GPIO_FAST | SCU_GPIO_PUP | SCU_CONF_FUNCTION0

// Initialise the pins for EUT/BAT detect and R5/E6 check
void init_tests(void) {
	scu_pinmux(R5_ADC, GPIO_MODE);
	scu_pinmux(R5_SPLY, GPIO_MODE);
	scu_pinmux(R6_SPLY, GPIO_MODE);
	scu_pinmux(GND_ALT, GPIO_MODE);
	scu_pinmux(TMS, GPIO_MODE);
	scu_pinmux(TMS_ALT, GPIO_MODE);
	scu_pinmux(VCTL, GPIO_MODE);
	scu_pinmux(VTGT, GPIO_MODE);
    gpio_input(&r5_adc);
    gpio_output(&r5_sply);
    gpio_output(&r6_sply);
    gpio_input(&gnd_alt);
    gpio_input(&tms_alt);
    gpio_input(&vctl);
    gpio_input(&vtgt);
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

// This is Dominic trying to narrow down the values for R5
// These values need to be fixed
#define R5_HIGH 0x60
#define R5_LOW 0x30

uint8_t r5_test() {
    uint8_t pins = 1;
	uint8_t clkdiv = 45;
	uint8_t clks = 0x2;
    uint8_t result = FAIL;
	uint16_t value;
    gpio_write(&r5_sply, 1);

    ADC0_CR = ADC_CR_SEL((uint32_t) pins) |
    ADC_CR_CLKDIV((uint32_t) clkdiv) |
    ADC_CR_CLKS((uint32_t) clks) |
    ADC_CR_PDN;
    ADC0_CR |= ADC_CR_START(1);

    while(!(ADC0_DR6 & ADC_DR_DONE));
    value = (ADC0_DR6>>6) & 0x3ff;

    if((value >= R5_LOW) && (value <= R5_HIGH))
        result = PASS;

    gpio_write(&r5_sply, 0);
    show_test_result(result, r5_leds);
	return result;
}

// This is Dominic trying to narrow down the values for R6
// These values need to be fixed
#define R6_HIGH 0x60
#define R6_LOW 0x30

uint8_t r6_test() {
    uint8_t pins = 1 << 6;
	uint8_t clkdiv = 45;
	uint8_t clks = 0x2;
    uint8_t result = FAIL;
	uint16_t value;
    gpio_write(&r6_sply, 1);

    ADC0_CR = ADC_CR_SEL((uint32_t) pins) |
    ADC_CR_CLKDIV((uint32_t) clkdiv) |
    ADC_CR_CLKS((uint32_t) clks) |
    ADC_CR_PDN;
    ADC0_CR |= ADC_CR_START(1);

    while(!(ADC0_DR0 & ADC_DR_DONE));
    value = (ADC0_DR0>>6) & 0x3ff;

    if((value >= R6_LOW) && (value <= R6_HIGH))
        result = PASS;

    gpio_write(&r6_sply, 0);
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
	jtag430_writeflash_bulk(0xF800, badge_firmware_len/2, buffer);
    result = PASS;
    show_test_result(result, program_leds);
	return result;
}

uint8_t verify() {
	uint8_t result = PASS;
    uint16_t *buffer = (uint16_t*) badge_firmware;
    uint16_t i;
    for(i = 0; i < badge_firmware_len/2; i++) {
        if(jtag430_readmem(0xF800+(i*2)) != buffer[i]) {
            result = FAIL;
            break;
        }
    }
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
