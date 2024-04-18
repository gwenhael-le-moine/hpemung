#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdbool.h>

// Keys
#define HPKEY_A 0
#define HPKEY_B 1
#define HPKEY_C 2
#define HPKEY_D 3
#define HPKEY_E 4
#define HPKEY_F 5

#define HPKEY_MTH 6
#define HPKEY_PRG 7
#define HPKEY_CST 8
#define HPKEY_VAR 9
#define HPKEY_UP 10
#define HPKEY_NXT 11

#define HPKEY_QUOTE 12
#define HPKEY_STO 13
#define HPKEY_EVAL 14
#define HPKEY_LEFT 15
#define HPKEY_DOWN 16
#define HPKEY_RIGHT 17

#define HPKEY_SIN 18
#define HPKEY_COS 19
#define HPKEY_TAN 20
#define HPKEY_SQRT 21
#define HPKEY_POWER 22
#define HPKEY_INV 23

#define HPKEY_ENTER 24
#define HPKEY_NEG 25
#define HPKEY_EEX 26
#define HPKEY_DEL 27
#define HPKEY_BS 28

#define HPKEY_ALPHA 29
#define HPKEY_7 30
#define HPKEY_8 31
#define HPKEY_9 32
#define HPKEY_DIV 33

#define HPKEY_SHL 34
#define HPKEY_4 35
#define HPKEY_5 36
#define HPKEY_6 37
#define HPKEY_MUL 38

#define HPKEY_SHR 39
#define HPKEY_1 40
#define HPKEY_2 41
#define HPKEY_3 42
#define HPKEY_MINUS 43

#define HPKEY_ON 44
#define HPKEY_0 45
#define HPKEY_PERIOD 46
#define HPKEY_SPC 47
#define HPKEY_PLUS 48

#define FIRST_HPKEY HPKEY_A
#define LAST_HPKEY HPKEY_PLUS
#define NB_KEYS ( LAST_HPKEY + 1 )

extern bool kbd_on;

extern void kbd_out_changed( void );

extern void press_key( int hpkey );
extern void release_key( int hpkey );

#endif
