/*
 * Copyright (c) 2012-2018, Great Scott Gadgets
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <msp430g2211.h>
#include <stdint.h>
#include "waveform.h"

#define NUM_LEDS 6
//#define NUM_LEDS 12
#define EXCITEMENT

/* alternative names for capacitive sense pads */
#define LOGO BIT6
#define P13 BIT6
#define HIDDEN BIT7
#define P12 BIT7

#define LEDS_OFF P1OUT = 0; P1DIR = 0xff

const uint8_t led_dir[] = {
		0x06, 0x06, 0x0a, 0x0a,
		0x0c, 0x0c, 0x03, 0x03,
		0x05, 0x05, 0x09, 0x09};
const uint8_t led_out[] = {
		0x04, 0x02, 0x08, 0x02,
		0x08, 0x04, 0x02, 0x01,
		0x04, 0x01, 0x08, 0x01};

#define NUM_SLOTS 4
#define CAPSENSE_HIGH_THRESHOLD 1200
#define CAPSENSE_LOW_THRESHOLD 600

volatile uint16_t lfsr = 0x6d61;
volatile uint16_t capsense_count = 0;
volatile uint16_t capsense_result = 0;
uint8_t capsense_latch = 0;

void set_led(uint8_t led)
{
	P1OUT = 0;
	P1DIR = led_dir[led];
	P1OUT = led_out[led];
}

void sleep(uint16_t duration)
{
	TA0CCR0 = (TAR + duration) & 0xffff;
	TA0CCTL0 |= CCIE;
	LPM0;
	TA0CCTL0 &= ~CCIE;
}

/*
 * call with one of:
 * WDT_ADLY_1000: approximately 6.4 s
 * WDT_ADLY_250: approximately 800 ms
 * WDT_ADLY_16: approximately 100 ms
 * WDT_ADLY_1_9: approximately 12.5 ms
 */
void long_sleep(uint16_t duration)
{
	WDTCTL = duration;
	IE1 |= WDTIE;
	LPM3;
}

/* courtesy of wikepedia */
inline void advance_lfsr(void)
{
	lfsr = (lfsr >> 1) ^ (-(lfsr & 1) & 0xB400);
}

/* PRNG */
uint16_t rand16(void)
{
	advance_lfsr();
	advance_lfsr();
	return lfsr;
}

uint16_t rand(uint8_t bits)
{
	return rand16() >> (16 - bits);
}

/* call with a randomly selected 1 or 0 */
void more_entropy(uint8_t input)
{
	if ((input) && (lfsr != 0xffff))
		lfsr = ~lfsr;
	advance_lfsr();
}

void test_leds(void)
{
	uint16_t i;

	for (i = 0; i < NUM_LEDS; i++) {
		set_led(i);
		long_sleep(WDT_ADLY_16);
	}
	for (i = 0; i < 0xffff; i++) {
		set_led(i % NUM_LEDS);
	}
	LEDS_OFF;
}

void party_mode(void)
{
	uint16_t i;
	uint8_t led = 0;

	for (i = 0x600; i; i--) {
		led = (led + 1 + rand(2)) % NUM_LEDS;
		set_led(led);
		long_sleep(WDT_ADLY_1_9);
		LEDS_OFF;
		long_sleep(WDT_ADLY_1_9);
		long_sleep(WDT_ADLY_1_9);
		long_sleep(WDT_ADLY_1_9);
	}
}

/*
 * Start capacitive sensing on one of the pins on Port 2.  Call this function
 * to start sensing, then wait for capsense_result to be set, then call
 * check_capsense() to check for a touch.
 *
 * Instead of waiting for capsense_result to be set, you can drop into LPM0 to
 * save power while waiting for completion of sensing.
 *
 * Call with pin set to LOGO or HIDDEN.  LOGO is the primary capsense pad (the
 * ToorCamp logo), also expansion pad P13.  HIDDEN is the secondary capsense
 * pad, expansion pad P12.
 */
void capsense(uint8_t pin) {
	if (HIDDEN & pin) {
		pin = HIDDEN;
	} else {
		pin = LOGO;
	}

	P2DIR = 0xff;
	P2OUT = pin;
	P2REN = 0;

	capsense_result = 0;
	P2IES = pin;

	/*
	 * Enable interrupts when the sense pin goes low.
	 * It should be okay to leave this enabled all the time, but you
	 * might want to disable it at some point.
	 */
	P2IE = pin;
	P2DIR &= ~pin;
	capsense_count = TA0R;
}

uint8_t check_capsense() {
	more_entropy(capsense_result & 1);
	if (CAPSENSE_HIGH_THRESHOLD < capsense_result) {
		if (0 == capsense_latch) {
			capsense_latch = 1;
			return 1;
		}
	} else if (CAPSENSE_LOW_THRESHOLD > capsense_result) {
		capsense_latch = 0;
	}
	return 0;
}

void light_show(void)
{
	uint8_t i, j;
	uint8_t rand3, next;
	uint8_t excitement = 64; /* 0 through 7 */
	uint8_t climaxed = 0;
	uint8_t pin = LOGO;

	/*
	 * Flies are numbered 0 through 11.  There are four interleaved time slots.
	 * Zero or one fly may be active during a slot for a maximum of four flies
	 * active simultaneously.
	 */

	/* the fly that is selected for each slot */
	uint8_t fly[NUM_SLOTS] = {0, 1, 2, 3};

	/* the current phase (position within the waveform) for each slot */
	uint8_t phase[NUM_SLOTS] = {0, 0xff, 0xff, 0xff};

	uint16_t delay[NUM_SLOTS] = {0, 0, 0, 0};

	capsense(LOGO);

	while (1) {
		LEDS_OFF;
		if (delay[0] && delay[1] && delay[2] && delay[3]) {
			/* we're waiting on all four slots, so go into deep sleep or exit */
			if (0 == excitement) {
				return;
			}
			/* this is close to the right duration but is uncalibrated */
			long_sleep(WDT_ADLY_1_9);
		} else {
			/* one PWM period with four interleaved slots */
			for (i = 0; i < 0xff; i++) {
				for (j = 0; j < NUM_SLOTS; j++) {
					sleep(9);
					if (phase[j] == 0xff)
						/* mark this waveform as finished */
						delay[j] = 1;
					/* all LEDs off */
					P1OUT = 0;
					P1DIR = 0;
					if (waveform[phase[j]] > i) {
						/* turn on an LED */
						P1DIR = led_dir[fly[j]];
						P1OUT = led_out[fly[j]];
					}
				}
				/* about 45 microseconds elapsed */
			}
		}
		for (j = 0; NUM_SLOTS > j; j++) {
			if (delay[j]) {
				if (phase[j]) {
					/* waveform finished but we haven't reset for next time yet */

					/* select a random delay before next illumination in this slot */
#ifdef EXCITEMENT
					delay[j] = 1 + rand(12 - (excitement>>5)) + rand(12 - (excitement>>5));  //12 is about 6 illuminations per minute
					if (0 == climaxed) {
						excitement++;
					} else if ((j&1) && (0 < excitement)) {
						excitement--;
					}
					if (255 == excitement) {
						climaxed = 1;
					}
#else
					delay[j] = 1 + rand(9) + rand(9);
#endif

					/* select a random LED */
					rand3 = rand16() & 0x7;
					rand3 %= (NUM_LEDS - NUM_SLOTS);
					next = 0;
					while ((next == fly[0]) || (next == fly[1]) ||
							(next == fly[2]) || (next == fly[3]) || rand3--)
						next++;
					fly[j] = next;

					phase[j] = 0;
				}
				--delay[j];
			} else {
				phase[j]++;
			}
		}
		if (capsense_result) {
			if (check_capsense()) {
				if (HIDDEN == pin) {
					party_mode();
				}
				if (246 > excitement) {
					excitement += 8;
				} else {
					excitement = 255;
					climaxed = 1;
				}
				for (j = 0; NUM_SLOTS > j; j++) {
					if (delay[j]) {
						delay[j] = 0;
						break;
					}
				}
			}
			if (254 < excitement) {
				pin = HIDDEN;
			} else {
				pin = LOGO;
			}
			capsense(pin);
		}
		/* about 11.5 ms elapsed, so about 87 Hz PWM */
	}
}

int main(void)
{
	uint8_t bcsctl1_tmp;

	LEDS_OFF;

	/* stop watchdog timer */
	WDTCTL = WDTPW | WDTHOLD;

	/* approximate a 12 MHz clock based calibrated values for 1 MHz */
	DCOCTL = CALDCO_1MHZ;
	bcsctl1_tmp = CALBC1_1MHZ;
	if ((bcsctl1_tmp & 0x0f) > 6) {
		bcsctl1_tmp = (bcsctl1_tmp & 0xf0) | 6;
	}
	BCSCTL1 = bcsctl1_tmp + 8;

	/* set all Port2 pins to outputs */
	P2SEL = 0;
	P2DIR = 0xff;
	P2REN = 0;
	
	/* configure ACLK to use internal VLO/2 */
	BCSCTL1 |= DIVA_1;
	BCSCTL3 |= LFXT1S_2;

	/* set SMCLK to MCLK/8 */
	BCSCTL2 |= DIVS_3;

	/* configure Timer_A */
	TA0CTL = TASSEL_2 | MC_2; /* start timer on SMCLK */
	__eint();

	test_leds();

	while(1) {
		capsense(LOGO);
		LPM0;
		if (check_capsense()) {
			light_show();
		}
		long_sleep(WDT_ADLY_16);
	}
}

void __attribute__((interrupt(WDT_VECTOR))) WDT_ISR(void)
{
	LPM3_EXIT;
}

void __attribute__((interrupt(TIMER0_A0_VECTOR))) TIMER0_A0_ISR(void)
{
	TA0CCTL0 &= ~CCIFG;
	LPM0_EXIT;
}

void __attribute__((interrupt(PORT2_VECTOR))) PORT2_ISR(void)
{
	P2IFG = 0;
	capsense_result = TA0R - capsense_count;
	P2DIR = 0xff;
	LPM0_EXIT;
}
