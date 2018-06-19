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
//
uint8_t r5_test();
//
uint8_t r6_test();
//
uint8_t mcu_test();
//
uint8_t capsense_test();
//
uint8_t jtag_test();
//
uint8_t program();
//
uint8_t verify();
//
uint8_t run();

#endif /* __TESTS_H__ */