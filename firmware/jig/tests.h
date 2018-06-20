/*
 * This file is part of GreatFET (ish)
 */

#ifndef __TESTS_H__
#define __TESTS_H__

enum test_result
{
	PASS  = 0,
	FAIL  = 1,
	FAULT = 2,
    CLEAR = 3,
    ALL   = 4
};

// Initialise the pins for EUT/BAT detect and R5/E6 check
void init_tests(void);
// Test for badge presence using P7 and P14
uint8_t badge_detect(void);
// Check whether the battery is installed
uint8_t battery_detect(void);
// Use and ADC to check the voltage divider, and thus R5 value
uint8_t r5_test(void);
// Use and ADC to check the voltage divider, and thus R6 value
uint8_t r6_test(void);
// 
uint8_t capsense_test(void);
// Test whether the MCU is installed
uint8_t mcu_test(void);
// Initiate JTAG and check for correct ID
uint8_t jtag_test(void);
// Erase and program the badge
uint8_t program(void);
// Verify the programming of the badge
uint8_t verify(void);
// Exit JTAG mode and run the programmed firmware
uint8_t run(void);

#endif /* __TESTS_H__ */