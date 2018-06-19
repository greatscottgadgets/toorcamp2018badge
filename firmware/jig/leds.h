/*
 * This file is part of GreatFET (ish)
 */

#ifndef __LEDS_H__
#define __LEDS_H__

static struct gpio_t eut_present = GPIO(2, 10);
static struct gpio_t eut_not_present = GPIO(1, 8);
static struct gpio_t eut_detect_fault = GPIO(1, 9);

static struct gpio_t bat_present = GPIO(2, 9);
static struct gpio_t bat_not_present = GPIO(0, 0);
static struct gpio_t bat_detect_fault = GPIO(1, 0);

static struct gpio_t r5_pass = GPIO(0, 1);
static struct gpio_t r5_fail = GPIO(1, 1);
static struct gpio_t r6_pass = GPIO(2, 5);
static struct gpio_t r6_fail = GPIO(1, 2);

static struct gpio_t mcu_pass = GPIO(0, 2);
static struct gpio_t mcu_fail = GPIO(2, 15);

static struct gpio_t capsense_pass = GPIO(2, 6);
static struct gpio_t capsense_fail = GPIO(1, 3);

static struct gpio_t jtag_pass = GPIO(1, 7);
static struct gpio_t jtag_fail = GPIO(0, 3);

static struct gpio_t program_pass = GPIO(1, 6);
static struct gpio_t program_fail = GPIO(2, 7);

static struct gpio_t verify_pass = GPIO(2, 14);
static struct gpio_t verify_fail = GPIO(0, 12);

static struct gpio_t run_pass = GPIO(2, 13);
static struct gpio_t run_fail = GPIO(0, 13);

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

#endif /* __LEDS_H__ */