/*
 * This file is part of GreatFET (ish)
 */

#ifndef __LEDS_H__
#define __LEDS_H__

#include <gpio.h>

typedef struct {
	gpio_t present;
	gpio_t not_present;
	gpio_t fault;
} detect_t;

typedef struct {
	gpio_t pass;
	gpio_t fail;
} test_t;

extern test_t r5_leds;
extern test_t r6_leds;
extern test_t mcu_leds;
extern test_t capsense_leds;
extern test_t jtag_leds;
extern test_t program_leds;
extern test_t verify_leds;
extern test_t run_leds;

void show_detect_result(uint8_t result, detect_t detect);
void show_test_result(uint8_t result, test_t test);
void init_leds(void);

#endif /* __LEDS_H__ */