/**********************************************************************
 * main.c - Main firmware (ATmega16/ATmega32 version)                 *
 * Version 1.00                                                       *
 **********************************************************************
 * rump is copyright (C) 2008 Chris Lee <clee@mg8.org>                *
 * based on c64key, copyright (C) 2006-2007 Mikkel Holm Olsen         *
 * based on HID-Test by Christian Starkjohann, Objective Development  *
 **********************************************************************
 * rump (Real USB Model-M PCB) is Free Software; you can redistribute *
 * and/or modify it under the terms of the OBDEV lice,nse, as found   *
 * in the license.txt file.                                           *
 *                                                                    *
 * rump is distributed in the hope that it will be useful, but        *
 * WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      *
 * OBDEV license for further details.                                 *
 **********************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <string.h>

/* Now included from the makefile */
//#include "keycodes.h"

#include "usbdrv.h"
#define DEBUG_LEVEL 0
#include "oddebug.h"

/* Hardware documentation:
 * ATmega-16 @12.000 MHz
 *
 * XT1..XT2: 12MHz X-tal
 * PA0..PA7: Keyboard matrix row0..row7 (pins J4.1 -> J4.8)
 * PC0..PC7: Keyboard matrix row16..row8 (pins J4.16 -> J4.9)
 * PB0..PB7: Keyboard matrix col0..col7 (pins J3.1 -> J3.8)
 * PD0     : D- USB negative (needs appropriate zener-diode and resistors)
 * PD2/INT0: D+ USB positive (needs appropriate zener-diode and resistors)
 *
 * USB Connector:
 * -------------
 *  1 (red)    +5V
 *  2 (white)  DATA-
 *  3 (green)  DATA+
 *  4 (black)  GND
 *    
 *
 *                       +---[1K5r]---- +5V
 *                       |
 *      USB              |                          ATmega-16
 *                       |
 *      (D-)-------+-----+-------------[68r]------- PD0
 *                 |
 *      (D+)-------|-----+-------------[68r]------- PD2/INT0
 *                 |     |
 *                 _     _
 *                 ^     ^  2 x 3.6V 
 *                 |     |  zener to GND
 *                 |     |
 *                GND   GND
 */

/* The LED states */
#define LED_NUM     0x01
#define LED_CAPS    0x02
#define LED_SCROLL  0x04
#define LED_COMPOSE 0x08
#define LED_KANA    0x10


/* Originally used as a mask for the modifier bits, but now also
   used for other x -> 2^x conversions (lookup table). */
const char modmask[8] PROGMEM = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };


/* USB report descriptor (length is defined in usbconfig.h)
   This has been changed to conform to the USB keyboard boot protocol */
char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] PROGMEM = {
	0x05, 0x01,            // USAGE_PAGE (Generic Desktop)
	0x09, 0x06,            // USAGE (Keyboard)
	0xa1, 0x01,            // COLLECTION (Application)
	0x05, 0x07,            //   USAGE_PAGE (Keyboard)
	0x19, 0xe0,            //   USAGE_MINIMUM (Keyboard LeftControl)
	0x29, 0xe7,            //   USAGE_MAXIMUM (Keyboard Right GUI)
	0x15, 0x00,            //   LOGICAL_MINIMUM (0)
	0x25, 0x01,            //   LOGICAL_MAXIMUM (1)
	0x75, 0x01,            //   REPORT_SIZE (1)
	0x95, 0x08,            //   REPORT_COUNT (8)
	0x81, 0x02,            //   INPUT (Data,Var,Abs)
	0x95, 0x01,            //   REPORT_COUNT (1)
	0x75, 0x08,            //   REPORT_SIZE (8)
	0x81, 0x03,            //   INPUT (Cnst,Var,Abs)
	0x95, 0x05,            //   REPORT_COUNT (5)
	0x75, 0x01,            //   REPORT_SIZE (1)
	0x05, 0x08,            //   USAGE_PAGE (LEDs)
	0x19, 0x01,            //   USAGE_MINIMUM (Num Lock)
	0x29, 0x05,            //   USAGE_MAXIMUM (Kana)
	0x91, 0x02,            //   OUTPUT (Data,Var,Abs)
	0x95, 0x01,            //   REPORT_COUNT (1)
	0x75, 0x03,            //   REPORT_SIZE (3)
	0x91, 0x03,            //   OUTPUT (Cnst,Var,Abs)
	0x95, 0x06,            //   REPORT_COUNT (6)
	0x75, 0x08,            //   REPORT_SIZE (8)
	0x15, 0x00,            //   LOGICAL_MINIMUM (0)
	0x25, 0x65,            //   LOGICAL_MAXIMUM (101)
	0x05, 0x07,            //   USAGE_PAGE (Keyboard)
	0x19, 0x00,            //   USAGE_MINIMUM (Reserved (no event indicated))
	0x29, 0x65,            //   USAGE_MAXIMUM (Keyboard Application)
	0x81, 0x00,            //   INPUT (Data,Ary,Abs)
	0xc0                   // END_COLLECTION
};

/* This buffer holds the last values of the scanned keyboard matrix */
static uchar bitbuf[NUMROWS] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

/* The ReportBuffer contains the USB report sent to the PC */
static uchar reportBuffer[8];    /* buffer for HID reports */
static uchar idleRate;           /* in 4 ms units */
static uchar protocolVer = 1;    /* 0 is boot protocol, 1 is report protocol */

static void hardwareInit(void) {
	/*
	 * Theory of operation:
	 * Initially, all keyboard scan keys are set as inputs, but with
	 * pullups.  This causes them to be "weak" +5 outputs.
	 * To scan, we set one line as a low output, causing it to "win"
	 * over the weak outputs.
	 */
	PORTA = 0xFF;   /* Port A = J4 pins 1-8 - enable pull-up */
	DDRA  = 0x00;   /* Port A is input */

	PORTB = 0xFF;   /* Port B are row drivers - enable pull-up */
	DDRB  = 0x00;   /* Port B is input */

	PORTC = 0xFF;   /* activate all pull-ups */
	DDRC  = 0x00;   /* all pins input */

	PORTD = 0x00;   /* 0000 0000 bin: leave PORTD alone */
	DDRD  = 0x05;   /* 0000 0101 bin: these pins are for USB output */

	/* USB Reset by device only required on Watchdog Reset */
	_delay_us(11);   /* delay >10ms for USB reset */ 

	DDRD = 0x02;    /* 0000 0010 bin: remove USB reset condition */
	/* configure timer 0 for a rate of 12M/(1024 * 256) = 45.78 Hz (~22ms) */
	TCCR0 = 5;      /* timer 0 prescaler: 1024 */
}

/* This function scans the entire keyboard, debounces the keys, and
   if a key change has been found, a new report is generated, and the
   function returns true to signal the transfer of the report. */
static uchar scankeys(void) {   
	uchar reportIndex = 1; /* First available report entry is 2 */
	uchar retval = 0;
	uchar row, data, key, modkeys;
	volatile uchar col, mask;
	static uchar debounce = 5;

	/* Scan all rows */
	for (row = 0; row < NUMROWS; ++row) {
		// Load the scan byte mask from modmask
		data = pgm_read_byte(&modmask[row & 7]);
		if (row <= 7) {
			/* Rows 0 -> 7 */
			// Port C to weak pullups
			DDRC  = 0x00;
			PORTC = 0xFF;
			// Scan on A
			DDRA  = data;
			PORTA = ~data;
		} else {
			/* Rows 8 -> 15 */
			// Port A to weak pullups
			DDRA  = 0x00;
			PORTA = 0xFF;
			// Scan on C
			DDRC  = data;
			PORTC = ~data;
		}

		/* Used to be small loop, but the compiler optimized it away ;-) */
		_delay_us(30);

		// Read column output on B.
		data = PINB;

		/* If a change was detected, activate debounce counter */
		if (data ^ bitbuf[row]) {
			debounce = 10; 
		}

		/* Store the result */
		bitbuf[row] = data; 
	}

	/* Count down, but avoid underflow */
	if (debounce > 1) {
		debounce--;
		return retval;
	}

	modkeys = 0;
	/* Clear report buffer */
	memset(reportBuffer, 0, sizeof(reportBuffer));

	/* Process all rows for key-codes */
	for (row = 0; row < NUMROWS; ++row) {
		/* Anything on this row? - if not, skip it */
		if (0xFF == bitbuf[row]) { continue; }

		/* Restore buffer */
		data = bitbuf[row];

		for (col = 0, mask = 1; col < 8; ++col, mask <<= 1) {
			/* If no key detected, jump to the next column */
			if (data & mask) { continue; }

			/* Read keyboard map */
			key = pgm_read_byte(&keymap[row][col]);

			/* Is this a modifier key? */
			if (key > KEY_Modifiers) {
				reportBuffer[0] |= pgm_read_byte(&modmask[key - (KEY_Modifiers + 1)]);
				continue;
			}

			/* Too many keycodes - rollOver */
			if (++reportIndex < sizeof(reportBuffer)) {
				/* Set next available entry */
				reportBuffer[reportIndex] = key;
 				continue;
			}

			/* Only fill buffer once */
			if (!retval & 0x02) {
				memset(reportBuffer + 2, KEY_errorRollOver, sizeof(reportBuffer) - 2);
				/* continue decoding to get modifiers */
				retval |= 2;
			}
		}
	}

	/* Clear RSHIFT */
	if (modkeys & 0x80)
		reportBuffer[0] &= ~0x20;

	/* Clear LSHIFT */
	if (modkeys & 0x08)
		reportBuffer[0] &= ~0x02;

	/* Set other modifiers */
	reportBuffer[0] |= modkeys & 0x77;

	/* Must have been a change at some point, since debounce is done */
	retval |= 1;

	return retval;
}

uchar expectReport = 0;
uchar LEDstate = 0;

uchar usbFunctionSetup(uchar data[8]) {
	usbRequest_t *rq = (void *)data;
	usbMsgPtr = reportBuffer;
/* class request type */
	if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
		if (rq->bRequest == USBRQ_HID_GET_REPORT) {
			/* wValue: ReportType (highbyte), ReportID (lowbyte) */
			/* we only have one report type, so don't look at wValue */
			return sizeof(reportBuffer);
		} else if (rq->bRequest == USBRQ_HID_SET_REPORT) {
			/* We expect one byte reports */
			if (rq->wLength.word == 1) {
				/* Call usbFunctionWrite with data */
				expectReport = 1;
				return 0xFF;
			}  
		} else if (rq->bRequest == USBRQ_HID_GET_IDLE) {
			usbMsgPtr = &idleRate;
			return 1;
		} else if (rq->bRequest == USBRQ_HID_SET_IDLE) {
			idleRate = rq->wValue.bytes[1];
		} else if (rq->bRequest == USBRQ_HID_GET_PROTOCOL) {
			if (rq->wValue.bytes[1] < 1)
				protocolVer = rq->wValue.bytes[1];
		} else if (rq->bRequest == USBRQ_HID_SET_PROTOCOL) {
			usbMsgPtr = &protocolVer;
			return 1;
		}
	}
	return 0;
}

uchar usbFunctionWrite(uchar *data, uchar len) {
	if ((expectReport) && (len == 1)) {
		/* Get the state of all 5 LEDs */
		LEDstate = data[0];
		/* Check state of CAPS lock LED */
/* New idea: Let's not mess with PORTD.
		if (LEDstate & LED_CAPS) {
			PORTD |= 0x02;
		} else {
			PORTD &= ~0x02;
		}
*/
		expectReport = 0;
		return 0x01;
	}
	expectReport = 0;
	return 0x01;
}

int main(void) {
	uchar updateNeeded = 0;
	uchar idleCounter = 0;

	wdt_enable(WDTO_2S); /* Enable watchdog timer 2s */
	hardwareInit(); /* Initialize hardware (I/O) */

	odDebugInit();

	usbInit(); /* Initialize USB stack processing */
	sei(); /* Enable global interrupts */

	/* Main loop */
	for (;;) {
		/* Reset the watchdog */
		wdt_reset();
		/* Poll the USB stack */
		usbPoll();

		/* Scan the keyboard for changes */
		updateNeeded |= scankeys();

		/* Check timer if we need periodic reports */
		if (TIFR & (1 << TOV0)) {
			/* Reset flag */
			TIFR = 1 << TOV0;
			/* Do we need periodic reports? */
			if (idleRate != 0) {
				if (idleCounter > 4) {
					/* Yes, but not yet */
					/* 22 ms in units of 4 ms */
					idleCounter -= 5;
				} else {
					/* Yes, it is time now */
					updateNeeded = 1;
					idleCounter = idleRate;
				}
			}
		}

	/* If an update is needed, send the report */
		if(updateNeeded && usbInterruptIsReady()) {
			updateNeeded = 0;
			usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
		}
	}
	return 0;
}
