/*********************************************************************
 * ibm_1391401.h - Keymap for US IBM 1391401 keyboard layout         *
 *********************************************************************
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
 *********************************************************************/

#ifndef KEYMAP_H
#define KEYMAP_H
#include <avr/pgmspace.h>

/* Number of rows in IBM keyboard matrix */
#define NUMROWS 16

/* The USB keycodes are enumerated here - the first part is simply
   an enumeration of the allowed scan-codes used for USB HID devices */
enum keycodes {
  KEY__=0,
  KEY_errorRollOver,
  KEY_POSTfail,
  KEY_errorUndefined,
  KEY_A,        // 4
  KEY_B,
  KEY_C,
  KEY_D,
  KEY_E,
  KEY_F,
  KEY_G, 
  KEY_H,
  KEY_I,
  KEY_J,
  KEY_K,
  KEY_L,
  KEY_M,        // 0x10
  KEY_N,
  KEY_O,
  KEY_P,
  KEY_Q, 
  KEY_R,
  KEY_S,
  KEY_T,
  KEY_U,
  KEY_V,
  KEY_W,
  KEY_X,
  KEY_Y,
  KEY_Z,
  KEY_1,
  KEY_2,
  KEY_3,        // 0x20
  KEY_4,
  KEY_5,
  KEY_6,
  KEY_7,
  KEY_8,
  KEY_9,
  KEY_0,        // 0x27
  KEY_enter,
  KEY_esc,
  KEY_bckspc,   // backspace
  KEY_tab,
  KEY_spc,      // space
  KEY_minus,    // - (and _)
  KEY_equal,    // = (and +)
  KEY_lbr,      // [
  KEY_rbr,      // ]  -- 0x30
  KEY_bckslsh,  // \ (and |)
  KEY_hash,     // Non-US # and ~
  KEY_smcol,    // ; (and :)
  KEY_ping,     // ' and "
  KEY_grave,    // Grave accent and tilde
  KEY_comma,    // , (and <)
  KEY_dot,      // . (and >)
  KEY_slash,    // / (and ?)
  KEY_cpslck,   // capslock
  KEY_F1,
  KEY_F2,
  KEY_F3,
  KEY_F4,
  KEY_F5,
  KEY_F6, 
  KEY_F7,       // 0x40
  KEY_F8,
  KEY_F9,
  KEY_F10,
  KEY_F11,
  KEY_F12,
  KEY_PrtScr,
  KEY_scrlck,
  KEY_break,
  KEY_ins,
  KEY_home,
  KEY_pgup,
  KEY_del,
  KEY_end,
  KEY_pgdn,
  KEY_rarr, 
  KEY_larr,     // 0x50
  KEY_darr,
  KEY_uarr,
  KEY_numlock,
  KEY_KPslash,
  KEY_KPast,
  KEY_KPminus,
  KEY_KPplus,
  KEY_KPenter,
  KEY_KP1,
  KEY_KP2,
  KEY_KP3,
  KEY_KP4,
  KEY_KP5,
  KEY_KP6,
  KEY_KP7,
  KEY_KP8,      // 0x60
  KEY_KP9,
  KEY_KP0,
  KEY_KPcomma,
  KEY_Euro2,

  /* These are NOT standard USB HID - handled specially in decoding,
     so they will be mapped to the modifier byte in the USB report */
  KEY_Modifiers,
  MOD_LCTRL,    // 0x01
  MOD_LSHIFT,   // 0x02
  MOD_LALT,     // 0x04
  MOD_LGUI,     // 0x08
  MOD_RCTRL,    // 0x10
  MOD_RSHIFT,   // 0x20
  MOD_RALT,     // 0x40
  MOD_RGUI,     // 0x80
};

const unsigned char keymap[NUMROWS][8] PROGMEM = { // IBM Model M keymap
    { 0,           0,          MOD_LCTRL, 0,          0,          0,           MOD_RCTRL,   0 },			// row  1
    { 0,           MOD_LSHIFT, 0,         0,          0,          0,           MOD_RSHIFT,  0 },			// row  2
    { KEY_esc,     KEY_tab,    KEY_grave, KEY_1,      KEY_Q,      KEY_A,       KEY_Z,       0 },			// row  3
    { 0,           KEY_cpslck, KEY_F1,    KEY_2,      KEY_W,      KEY_S,       KEY_X,       0 },			// row  4
    { KEY_F4,      KEY_F3,     KEY_F2,    KEY_3,      KEY_E,      KEY_D,       KEY_C,       0 },			// row  5
    { KEY_G,       KEY_T,      KEY_5,     KEY_4,      KEY_R,      KEY_F,       KEY_V,       KEY_B },		// row  6
    { KEY_F5,      KEY_bckspc, KEY_F9,    KEY_F10,    0,          KEY_bckslsh, KEY_enter,   KEY_spc },		// row  7
    { KEY_H,       KEY_Y,      KEY_6,     KEY_7,      KEY_U,      KEY_J,       KEY_M,       KEY_N },		// row  8
	//	These rows are in reverse order because PORTC is wired backwards.
    { MOD_LALT,    0,          0,         KEY_PrtScr, KEY_scrlck, 0,           0,           MOD_RALT },		// row 16
	{ KEY_uarr,    0,          KEY_home,  KEY_end,    KEY_KPplus, KEY_KPenter, KEY_break,   KEY_larr },		// row 15
    { KEY_KPcomma, KEY_KP6,    KEY_pgup,  KEY_pgdn,   KEY_KP9,    KEY_KP3,     KEY_KPast,   KEY_KPminus },	// row 14
    { KEY_KP0,     KEY_KP5,    KEY_ins,   KEY_F12,    KEY_KP8,    KEY_KP2,     KEY_KPslash, KEY_rarr },		// row 13
    { 0,           KEY_KP4,    KEY_del,   KEY_F11,    KEY_KP7,    KEY_KP1,     KEY_numlock, KEY_darr },		// row 12
    { KEY_ping,    KEY_lbr,    KEY_minus, KEY_0,      KEY_P,      KEY_smcol,   0,           KEY_slash },	// row 11
    { 0,           KEY_F7,     KEY_F8,    KEY_9,      KEY_O,      KEY_L,       KEY_dot,     0 },			// row 10
	{ KEY_F6,      KEY_rbr,    KEY_equal, KEY_8,      KEY_I,      KEY_K,       KEY_comma,   0 }				// row  9
};

#endif
