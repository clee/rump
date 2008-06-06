/*********************************************************************
 * main.c - Main firmware (ATmega16 version)                         *
 * $Id: $
 * Version 1.98ß                                                     *
 *********************************************************************
 * NOTE: This version is experimantal, and is an attempt to make a   *
 * composite device, consisting of both a keyboard and two joystikc- *
 * ports. So far I have not been able to make this work, but perhaps *
 * some day I get a chance to look at this again.                    *
 *********************************************************************
 * c64key is Copyright (C) 2006-2007 Mikkel Holm Olsen               *
 * based on HID-Test by Christian Starkjohann, Objective Development *
 *********************************************************************
 * Spaceman Spiff's Commodire 64 USB Keyboard (c64key for short) is  *
 * is free software; you can redistribute it and/or modify it under  *
 * the terms of the OBDEV license, as found in the licence.txt file. *
 *                                                                   *
 * c64key is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of    *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the     *
 * OBDEV license for further details.                                *
 *********************************************************************/

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
 * PB0..PB7: Keyboard matrix Row0..Row7 (pins 12,11,10,5,8,7,6,9 on C64 kbd)
 * PC0..PC7: Keyboard matrix Col0..Col7 (pins 13,19,18,17,16,15,14,20 on C64 kbd)
 * PD0     : D- USB negative (needs appropriate zener-diode and resistors)
 * PD1     : UART TX
 * PD2/INT0: D+ USB positive (needs appropriate zener-diode and resistors)
 * PD7     : Keyboard matrix Row8 (Restore key)
 *
 * USB Connector:
 * -------------
 *  1 (red)    +5V
 *              2 (white)  DATA-
 *              3 (green)  DATA+
 *              4 (black)  GND
 *                              
 *
 *
 *                                     VCC
 *                  +--[4k7]--+--[2k2]--+
 *      USB        GND        |                     ATmega-16
 *                            |
 *      (D-)-------+----------+--------[82r]------- PD0
 *                 |
 *      (D+)-------|-----+-------------[82r]------- PD2/INT0
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
const char modmask[8] PROGMEM = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
  };


/* USB report descriptor (length is defined in usbconfig.h)
   This has been changed to conform to the USB keyboard boot
   protocol */
char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] 
  PROGMEM = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0x95, 0x05,                    //   REPORT_COUNT (5)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0x05, 0x08,                    //   USAGE_PAGE (LEDs)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x95, 0x05,                    //   REPORT_COUNT (5)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x19, 0x01,                    //   USAGE_MINIMUM (Num Lock)
    0x29, 0x05,                    //   USAGE_MAXIMUM (Kana)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x03,                    //   REPORT_SIZE (3)
    0x91, 0x03,                    //   OUTPUT (Cnst,Var,Abs)
    0xc0,                          // END_COLLECTION
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x04,                    // USAGE (Joystick)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x15, 0xff,                    //     LOGICAL_MINIMUM (-1)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x75, 0x02,                    //     REPORT_SIZE (2)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x09, 0x01,                    //     USAGE (Button 1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xc0,                          //   END_COLLECTION
    0x95, 0x03,                    //   REPORT_COUNT (3)
    0x81, 0x01,                    //   INPUT (Cnst,Ary,Abs)
    0xc0                           // END_COLLECTION
};

const char decode_joy[16] PROGMEM = {
                /* This decodes the joystick positions (low 4 bits) */
                0b00001010, /*  0 RLDU - XX XX */
                0b00000110, /*  1 RLD_ - +1 XX */
                0b00001110, /*  2 RL_U - -1 XX */
                0b00000010, /*  3 RL__ -  0 XX */
                0b00001001, /*  4 R_DU - XX +1 */
                0b00000101, /*  5 R_D_ - +1 +1 */
                0b00001101, /*  6 R__U - -1 +1 */
                0b00000001, /*  7 R___ -  0 +1 */
                0b00001011, /*  8 _LDU - XX -1 */
                0b00000111, /*  9 _LD_ - +1 -1 */
                0b00001111, /* 10 _L_U - -1 -1 */
                0b00000011, /* 11 _L__ -  0 -1 */
                0b00001000, /* 12 __DU - XX  0 */
                0b00000100, /* 13 __D_ - +1  0 */
                0b00001100, /* 14 ___U - -1  0 */
                0b00000000  /* 15 ____ -  0  0 */
};

/* This buffer holds the last values of the scanned keyboard matrix */
static uchar bitbuf[NUMROWS]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

/* The ReportBuffer contains the USB report sent to the PC */
static uchar reportBuffer[8];    /* buffer for HID reports */
static uchar idleRate;           /* in 4 ms units */
static uchar protocolVer=1;      /* 0 is the boot protocol, 1 is report protocol */

static void hardwareInit(void) {
  PORTA = 0xFF;                 /* Port A are joystick inputs - enable pull-ups */
                DDRA  = 0x00;                   /* Port A is input */

  PORTB = 0xFF;   /* Port B are row drivers - enable pull-up */
  DDRB  = 0x00;   /* Port B is input */

  PORTC = 0xFF;   /* activate all pull-ups */
  DDRC  = 0x00;   /* all pins input */
  
  PORTD = 0xfa;   /* 1111 1010 bin: activate pull-ups except on USB lines */
  DDRD  = 0x07;   /* 0000 0111 bin: all pins input except USB (-> USB reset) */

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
  uchar reportIndex=1; /* First available report entry is 2 */
  uchar retval=0;
  uchar row,data,key, modkeys;
  volatile uchar col, mask;
  static uchar debounce=5;

                uchar d=PINA;
                reportBuffer[7]=pgm_read_byte(&decode_joy[d&0x0F])|((d&0x10)^0x10);

  for (row=0;row<NUMROWS;++row) { /* Scan all rows */
    if (row<=7) {
                                                DDRD&=~(1<<7); /* PD7 as input*/
                                                PORTD|=(1<<7); /* Enable pull-up */
                                  data=pgm_read_byte(&modmask[row]);
                                                DDRB=data;
                                                PORTB=~data;
                                } else { /* Must be row 8 */
                                                DDRB=0;
                                                PORTB=0xFF;
                                                DDRD|=(1<<7); /* select PD7 = row8 */
                                                PORTD&=~(1<<7);
                                }
                                
                                _delay_us(30); /* Used to be small loop, but the compiler optimized it away ;-) */
    
    data=PINC;
    if (data^bitbuf[row]) { 
                                                debounce=10; /* If a change was detected, activate debounce counter */
    }
    bitbuf[row]=data; /* Store the result */
                }

                if (debounce==1) { /* Debounce counter expired */
                  modkeys=0;
                                memset(reportBuffer,0,sizeof(reportBuffer)); /* Clear report buffer */
                                for (row=0;row<NUMROWS;++row) { /* Process all rows for key-codes */
                                                data=bitbuf[row]; /* Restore buffer */
                                                
                                                if (data!=0xFF) { /* Anything on this row? - optimization */
                                                                for (col=0,mask=1;col<8;++col,mask<<=1) { /* yes - check individual bits */
                                                                                if (!(data&mask)) { /* Key detected */
                                                                                                key=pgm_read_byte(&keymap[row][col]); /* Read keyboard map */
                                                                                                if (key>KEY_Special) { /* Special handling of shifted keys */
                                                                                                  /* Modifiers have not been decoded yet - handle manually */
                                                                                                                uchar keynum=key-(KEY_Special+1);
                                                                                                                if ((bitbuf[4]&0b01000000)&& /* Rshift */
                                                                                                                                   ((bitbuf[7]&0b00000010)||(key>=SPC_crsrud))) {/* Lshift */
                                                                                                                                key=pgm_read_byte(&spec_keys[keynum][0]); /* Unmodified */
                                                                                                                                modkeys=pgm_read_byte(&spec_keys[keynum][1]);
                                                                                                                } else {
                                                                                                                                key=pgm_read_byte(&spec_keys[keynum][2]); /* Shifted */
                                                                                                                                modkeys=pgm_read_byte(&spec_keys[keynum][3]);
                                                                                                                }
                                                                                                }               else if (key>KEY_Modifiers) { /* Is this a modifier key? */
                                                                                                                reportBuffer[0]|=pgm_read_byte(&modmask[key-(KEY_Modifiers+1)]);
                                                                                                                key=0;
                                                                                                }
                                                                                                if (key) { /* Normal keycode should be added to report */
                                                                                                                if (++reportIndex>=sizeof(reportBuffer)) { /* Too many keycodes - rollOver */
                                                                                                                                if (!retval&0x02) { /* Only fill buffer once */
                                                                                                                                                memset(reportBuffer+2, KEY_errorRollOver, sizeof(reportBuffer)-2);
                                                                                                                                                retval|=2; /* continue decoding to get modifiers */
                                                                                                                                }
                                                                                                                } else {
                                                                                                                                reportBuffer[reportIndex]=key; /* Set next available entry */
                                                                                                                }
                                                                                                }
                                                                                }
                                                                }
                                                }
                                }
                                if (modkeys&0x80) { /* Clear RSHIFT */
                                  reportBuffer[0]&=~0x20;
                                }
                                if (modkeys&0x08) { /* Clear LSHIFT */
                                  reportBuffer[0]&=~0x02;
                                }
                                reportBuffer[0]|=modkeys&0x77; /* Set other modifiers */

                                retval|=1; /* Must have been a change at some point, since debounce is done */
  }
                if (debounce) debounce--; /* Count down, but avoid underflow */
  return 1; //retval;
}

uchar expectReport=0;
uchar LEDstate=0;

uchar           usbFunctionSetup(uchar data[8]) {
  usbRequest_t *rq = (void *)data;
  usbMsgPtr = reportBuffer;
  if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* class request type */
    if(rq->bRequest == USBRQ_HID_GET_REPORT){  
                                  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
      /* we only have one report type, so don't look at wValue */
      return sizeof(reportBuffer);
                                }else if(rq->bRequest == USBRQ_HID_SET_REPORT){
                                                if (rq->wLength.word == 1) { /* We expect one byte reports */
                                                                expectReport=1;
                                                                return 0xFF; /* Call usbFunctionWrite with data */
                                                }  
    }else if(rq->bRequest == USBRQ_HID_GET_IDLE){
      usbMsgPtr = &idleRate;
      return 1;
    }else if(rq->bRequest == USBRQ_HID_SET_IDLE){
      idleRate = rq->wValue.bytes[1];
    }else if(rq->bRequest == USBRQ_HID_GET_PROTOCOL) {
                                                if (rq->wValue.bytes[1] < 1) {
                                                                protocolVer = rq->wValue.bytes[1];
                                                }
                                }else if(rq->bRequest == USBRQ_HID_SET_PROTOCOL) {
                                                usbMsgPtr = &protocolVer;
                                                return 1;
                                }
  }
                return 0;
}

uchar usbFunctionWrite(uchar *data, uchar len) {
  if ((expectReport)&&(len==1)) {
                                LEDstate=data[0]; /* Get the state of all 5 LEDs */
                                if (LEDstate&LED_CAPS) { /* Check state of CAPS lock LED */
                                  PORTD|=0x02;
                                } else {
                                                PORTD&=~0x02;
                                }
                                expectReport=0;
                                return 1;
                }
                expectReport=0;
                return 0x01;
}

int             main(void) {
  uchar   updateNeeded = 0;
  uchar   idleCounter = 0;

                wdt_enable(WDTO_2S); /* Enable watchdog timer 2s */
  hardwareInit(); /* Initialize hardware (I/O) */
                
                odDebugInit();

                usbInit(); /* Initialize USB stack processing */
                sei(); /* Enable global interrupts */
                
                for(;;){        /* Main loop */
                                wdt_reset(); /* Reset the watchdog */
                                usbPoll(); /* Poll the USB stack */

    updateNeeded|=scankeys(); /* Scan the keyboard for changes */
                                
                                /* Check timer if we need periodic reports */
    if(TIFR & (1<<TOV0)){
      TIFR = 1<<TOV0; /* Reset flag */
      if(idleRate != 0){ /* Do we need periodic reports? */
        if(idleCounter > 4){ /* Yes, but not yet */
          idleCounter -= 5;   /* 22 ms in units of 4 ms */
        }else{ /* Yes, it is time now */
          updateNeeded = 1;
          idleCounter = idleRate;
        }
      }
    }
                                
                                /* If an update is needed, send the report */
    if(updateNeeded && usbInterruptIsReady()){
                                                updateNeeded = 0;
                                                usbSetInterrupt(reportBuffer, 2); //sizeof(reportBuffer));
    }
                }
                return 0;
}
