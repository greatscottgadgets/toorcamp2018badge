/*
 * This file is part of GreatFET (ish)
 */

#include <libopencm3/lpc43xx/scu.h>
#include <gpio_lpc.h>
#include "leds.h"
#include "tests.h"

static struct gpio_t eut_present = GPIO(2, 10);
static struct gpio_t eut_not_present = GPIO(1, 8);
static struct gpio_t eut_detect_fault = GPIO(1, 9);

static struct gpio_t bat_present = GPIO(2, 9);
static struct gpio_t bat_not_present = GPIO(0, 0);
static struct gpio_t bat_detect_fault = GPIO(1, 0);


static struct gpio_t r5_pass = GPIO(0, 1);
static struct gpio_t r5_fail = GPIO(1, 1);
test_t r5_leds = {
	.pass = &r5_pass,
	.fail = &r5_fail,
};

static struct gpio_t r6_pass = GPIO(2, 5);
static struct gpio_t r6_fail = GPIO(1, 2);
test_t r6_leds = {
	.pass = &r6_pass,
	.fail = &r6_fail,
};

static struct gpio_t mcu_pass = GPIO(0, 2);
static struct gpio_t mcu_fail = GPIO(2, 15);
test_t mcu_leds = {
	.pass = &mcu_pass,
	.fail = &mcu_fail,
};

static struct gpio_t capsense_pass = GPIO(2, 6);
static struct gpio_t capsense_fail = GPIO(1, 3);
test_t capsense_leds = {
	.pass = &capsense_pass,
	.fail = &capsense_fail,
};

static struct gpio_t jtag_pass = GPIO(1, 7);
static struct gpio_t jtag_fail = GPIO(0, 3);
test_t jtag_leds = {
	.pass = &jtag_pass,
	.fail = &jtag_fail,
};

static struct gpio_t program_pass = GPIO(1, 6);
static struct gpio_t program_fail = GPIO(2, 7);
test_t program_leds = {
	.pass = &program_pass,
	.fail = &program_fail,
};

static struct gpio_t verify_pass = GPIO(2, 14);
static struct gpio_t verify_fail = GPIO(0, 12);
test_t verify_leds = {
	.pass = &verify_pass,
	.fail = &verify_fail,
};

static struct gpio_t run_pass = GPIO(2, 13);
static struct gpio_t run_fail = GPIO(0, 13);
test_t run_leds = {
	.pass = &run_pass,
	.fail = &run_fail,
};

#define EUT_PRESENT (P5_1)
#define EUT_NOT_PRESENT (P1_5)
#define EUT_DETECT_FAULT (P1_6)

#define BAT_PRESENT (P5_0)
#define BAT_NOT_PRESENT (P0_0)
#define BAT_DETECT_FAULT (P1_7)

#define R5_PASS (P0_1)
#define R5_FAIL (P1_8)
#define R6_PASS (P4_5)
#define R6_FAIL (P1_9)

#define MCU_PASS (P1_15)
#define MCU_FAIL (P5_6)

#define CAPSENSE_PASS (P4_6)
#define CAPSENSE_FAIL (P1_10)

#define JTAG_PASS (P1_14)
#define JTAG_FAIL (P1_16)

#define PROGRAM_PASS (P1_13)
#define PROGRAM_FAIL (P5_7)

#define VERIFY_PASS (P5_5)
#define VERIFY_FAIL (P1_17)

#define RUN_PASS (P5_4)
#define RUN_FAIL (P1_18)

// Luckily all are pinmux function 0
#define GPIO_MODE SCU_GPIO_FAST | SCU_GPIO_PUP | SCU_CONF_FUNCTION0


void show_detect_result(uint8_t result, detect_t detect) {
    switch(result) {
        case PASS:
            gpio_write(detect.present, 1);
            gpio_write(detect.not_present, 0);
            gpio_write(detect.fault, 0);
            break;
        case FAIL:
            gpio_write(detect.present, 0);
            gpio_write(detect.not_present, 1);
            gpio_write(detect.fault, 0);
            break;
        case FAULT:
            gpio_write(detect.present, 0);
            gpio_write(detect.not_present, 0);
            gpio_write(detect.fault, 1);
            break;
        case CLEAR:
            gpio_write(detect.present, 0);
            gpio_write(detect.not_present, 0);
            gpio_write(detect.fault, 0);
            break;
        case ALL:
            gpio_write(detect.present, 1);
            gpio_write(detect.not_present, 1);
            gpio_write(detect.fault, 1);
            break;
    }
}

void show_test_result(uint8_t result, test_t test) {
    switch(result) {
        case PASS:
            gpio_write(test.pass, 1);
            gpio_write(test.fail, 0);
            break;
        case FAIL:
            gpio_write(test.pass, 0);
            gpio_write(test.fail, 1);
            break;
        case CLEAR:
            gpio_write(test.pass, 0);
            gpio_write(test.fail, 0);
            break;
        case ALL:
            gpio_write(test.pass, 1);
            gpio_write(test.fail, 1);
            break;
    }
}

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
