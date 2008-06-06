/*********************************************************************
 * key_dk_dk.h - Keyboard mapping Danish C64 keyboard to Danish      *
 * keyboard setting on the PC side.                                  *
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
#ifndef KEYMAP_H
#define KEYMAP_H
#include <avr/pgmspace.h>

/* Number of rows in keyboard matrix */
#define NUMROWS 9

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
  KEY_equal,    // =
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
  
  /* Other keys that need special handling -
     These are looked up in the table spec_keys because they do not
     generate the same scan-code in the shifted and unshifted state,
     and some may need to alter the shift-state to generate the
     correct character code on the PC */
  KEY_Special,
  SPC_4,
  SPC_7,
  SPC_0,
  SPC_plus,
  SPC_minus,
  SPC_pound,
  SPC_home,
  SPC_del,
  SPC_ast,
  SPC_equal,
  SPC_comma,
  SPC_dot,
  SPC_slash,
  SPC_crsrud,
  SPC_crsrlr,
  SPC_F1,
  SPC_F3,
  SPC_F5,
  SPC_F7
};

/* The keymap for the Commodore 64 keyboard with Danish keys mapping
   to a PC with danish keyboard mapping */
const unsigned char keymap[NUMROWS][8] PROGMEM = { // Danish keymap
    {SPC_del, KEY_3, KEY_5, SPC_7, KEY_9, SPC_plus, SPC_pound, KEY_1}, // row0
    {KEY_enter, KEY_W, KEY_R, KEY_Y, KEY_I, KEY_P, SPC_ast, KEY_esc}, // row1
    {SPC_crsrlr, KEY_A, KEY_D, KEY_G, KEY_J, KEY_L, KEY_ping, MOD_LCTRL}, // row2
    {SPC_F7, SPC_4, KEY_6, KEY_8, SPC_0, SPC_minus, SPC_home, KEY_2}, // row3
    {SPC_F1, KEY_Z, KEY_C, KEY_B, KEY_M, SPC_dot, MOD_RSHIFT, KEY_spc}, // row4
    {SPC_F3, KEY_S, KEY_F, KEY_H, KEY_K, KEY_smcol, SPC_equal, MOD_LALT}, // row5
    {SPC_F5, KEY_E, KEY_T, KEY_U, KEY_O, KEY_lbr, KEY_rbr, KEY_Q}, // row6
    {SPC_crsrud, MOD_LSHIFT, KEY_X, KEY_V, KEY_N, SPC_comma, SPC_slash, MOD_RALT}, // row7
    {MOD_RCTRL, 0, 0, 0, 0, 0, 0, 0} // Imaginary row8 is for restore
  };

/* Special keys that need to generate different scan-codes for unshifted
   and shifted states, or that need to alter the modifier keys. 
   Since the LGUI and RGUI bits are not used, these signify that the
   left and right shift states should be deleted from report, so
     0x88 means clear both shift flags
     0x00 means do not alter shift states
     0xC8 means clear both shifts and set L_ALT */
const unsigned char spec_keys[19][4] PROGMEM = {
  { KEY_4,       0x00, KEY_4,       0xC8}, // SPEC_4 - shift-4 is $
  { KEY_7,       0x00, KEY_bckslsh, 0x88}, // SPEC_7 - shift-7 is '
  { KEY_0,       0x00, KEY_2,       0xC8}, // SPEC_0 - shift-0 is @
  { KEY_minus,   0x88, KEY_minus,   0x88}, // SPC_plus 
  { KEY_slash,   0x88, KEY_0,       0x8A}, // SPC_minus - "-" and "="
  { KEY_dot,     0x8A, KEY_dot,     0x8A}, // SPC_pound - ":"
  { KEY_home,    0x80, KEY_end,     0x80}, // SPC_home - home and end
  { KEY_bckspc,  0x00, KEY_del,     0x88}, // SPC_del - backspace and delete
  { KEY_bckslsh, 0x82, KEY_bckslsh, 0x8A}, // SPC_ast - "*" (Asterix)
  { KEY_comma,   0x82, KEY_comma,   0x8A}, // SPC_equal - ";"
  { KEY_comma,   0x80, KEY_Euro2,   0x88}, // SPC_comma - "," and "<"
  { KEY_dot,     0x80, KEY_Euro2,   0x8A}, // SPC_dot - "." and ">"
  { KEY_7,       0x82, KEY_minus,   0x8A}, // SPC_slash - "/" and "?"
  { KEY_darr,    0x80, KEY_uarr,    0x80}, // SPC_crsrud - cursor down/up
  { KEY_rarr,    0x80, KEY_larr,    0x80}, // SPC_crsrlr - cursor right/left
  { KEY_F1,      0x80, KEY_F2,      0x80}, // SPC_F1 - F1 and F2
  { KEY_F3,      0x80, KEY_F4,      0x80}, // SPC_F3 - F3 and F4
  { KEY_F5,      0x80, KEY_F6,      0x80}, // SPC_F5 - F5 and F6
  { KEY_F7,      0x80, KEY_F8,      0x80}, // SPC_F7 - F7 and F8
};
#endif
