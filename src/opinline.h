#ifndef __OPINLINE_H
#define __OPINLINE_H

#include <string.h>

#include "types.h"
#include "cpu.h"

inline void load( byte* reg, byte* data, int start, int len )
{
    if ( start + len <= 16 ) {
        memcpy( reg + start, data, len );
    } else {
        memcpy( reg + start, data, 16 - start );
        memcpy( reg, data + ( 16 - start ), len - ( 16 - start ) );
    }
}

inline unsigned int nib_to_unsigned( byte* nib, int len )
{
    int x = 0;

    while ( len-- ) {
        x <<= 4;
        x |= nib[ len ];
    }
    return x;
}

inline int nib_to_signed( byte* nib, int len )
{
    int x;

    len--;
    x = nib[ len ];
    if ( x & 8 )
        x -= 16;
    while ( len-- ) {
        x <<= 4;
        x |= nib[ len ];
    }
    return x;
}

inline void unsigned_to_nib( byte* nib, int x, int len )
{
    while ( len-- ) {
        *nib++ = x & 0xF;
        x >>= 4;
    }
}

inline address rstk_pop( void )
{
    address adr = cpu.rstk[ cpu.rstk_ptr ];
    cpu.rstk[ cpu.rstk_ptr ] = 0x00000;
    cpu.rstk_ptr = ( cpu.rstk_ptr + 1 ) & 7;
    return adr;
}

inline void rstk_push( address adr )
{
    cpu.rstk_ptr = ( cpu.rstk_ptr - 1 ) & 7;
    cpu.rstk[ cpu.rstk_ptr ] = adr & 0xFFFFF;
}

inline void goyes( byte* opc, int offset )
{
    if ( cpu.carry ) {
        address rel = nib_to_signed( opc + offset, 2 );
        if ( rel ) {
            cpu.pc += rel + offset;
        } else {
            cpu.pc = rstk_pop();
        }
        cpu.cycles += 7;
    } else {
        cpu.pc += offset + 2;
    }
}

inline void reg_zero( byte* reg, int len ) { memset( reg, 0, len ); }

inline void reg_bit( byte* reg, int bit, int value )
{
    if ( value ) {
        reg[ bit >> 2 ] |= 1 << ( bit & 3 );
    } else {
        reg[ bit >> 2 ] &= ~( 1 << ( bit & 3 ) );
    }
}

inline void reg_cpy( byte* dest, byte* src, int len ) { memcpy( dest, src, len ); }

inline void reg_ex( byte* reg1, byte* reg2, int len )
{
    byte tmp[ 16 ];

    memcpy( tmp, reg1, len );
    memcpy( reg1, reg2, len );
    memcpy( reg2, tmp, len );
}

inline void comp_bit_zero( byte* reg, int bit ) { cpu.carry = ( reg[ bit >> 2 ] & ( 1 << ( bit & 3 ) ) ) ? false : true; }

inline void comp_zero( byte* reg, int len )
{
    while ( len-- ) {
        if ( *reg++ ) {
            cpu.carry = false;
            return;
        }
    }
    cpu.carry = true;
}

inline void comp_eq( byte* reg1, byte* reg2, int len )
{
    while ( len-- ) {
        if ( *reg1++ != *reg2++ ) {
            cpu.carry = false;
            return;
        }
    }
    cpu.carry = true;
}

inline void comp_gt( byte* reg1, byte* reg2, int len )
{
    while ( --len && reg1[ len ] == reg2[ len ] )
        ;
    cpu.carry = ( reg1[ len ] > reg2[ len ] ) ? true : false;
}

inline void alu_add( byte* dest, byte* src, int len )
{
    byte c = 0;
    byte base = cpu.dec ? 10 : 16;

    while ( len-- ) {
        if ( *dest >= base )
            *dest &= 7;
        *dest += *src + c;
        if ( *dest >= base ) {
            *dest -= base;
            c = 1;
        } else {
            c = 0;
        }
        dest++;
        src++;
    }
    cpu.carry = c ? true : false;
}

inline void alu_sub( byte* dest, byte* src, int len )
{
    byte c = 0;
    byte base = cpu.dec ? 10 : 16;

    while ( len-- ) {
        *dest -= *src + c;
        if ( *dest & 0xF0 ) {
            *dest += base;
            c = 1;
        } else {
            c = 0;
        }
        dest++;
        src++;
    }
    cpu.carry = c ? true : false;
}

inline void alu_sub2( byte* dest, byte* src, int len )
{
    byte c = 0;
    byte base = cpu.dec ? 10 : 16;

    while ( len-- ) {
        *dest = *src - ( *dest + c );
        if ( *dest & 0xF0 ) {
            *dest += base;
            c = 1;
        } else {
            c = 0;
        }
        dest++;
        src++;
    }
    cpu.carry = c ? true : false;
}

inline void alu_add_con( byte* reg, byte con, int i, int len )
{
    reg[ i ] += con;
    while ( len-- ) {
        reg[ i ]++;
        if ( !( reg[ i ] & 0xF0 ) ) {
            cpu.carry = false;
            return;
        }
        reg[ i ] -= 16;
        i = ( i + 1 ) & 0xF;
    }
    cpu.carry = true;
}

inline void alu_sub_con( byte* reg, byte con, int i, int len )
{
    reg[ i ] -= con;
    while ( len-- ) {
        reg[ i ]--;
        if ( !( reg[ i ] & 0xF0 ) ) {
            cpu.carry = false;
            return;
        }
        reg[ i ] += 16;
        i = ( i + 1 ) & 0xF;
    }
    cpu.carry = true;
}

inline void alu_inc( byte* reg, int len )
{
    if ( cpu.dec ) {
        byte c = 1;
        while ( len-- ) {
            if ( *reg >= 10 )
                *reg &= 7;
            *reg += c;
            if ( *reg >= 10 ) {
                *reg -= 10;
                c = 1;
            } else {
                c = 0;
            }
            reg++;
        }
        cpu.carry = c ? true : false;
    } else {
        while ( len-- ) {
            ( *reg )++;
            if ( !( *reg & 0xF0 ) ) {
                cpu.carry = false;
                return;
            }
            *reg -= 16;
            reg++;
        }
        cpu.carry = true;
    }
}

inline void alu_dec( byte* reg, int len )
{
    byte base = cpu.dec ? 10 : 16;

    while ( len-- ) {
        ( *reg )--;
        if ( !( *reg & 0xF0 ) ) {
            cpu.carry = false;
            return;
        }
        *reg += base;
        reg++;
    }
    cpu.carry = true;
}

inline void alu_neg( byte* reg, int len )
{
    byte base = cpu.dec ? 10 : 16;

    while ( len && *reg == 0 ) {
        reg++;
        len--;
    }
    cpu.carry = len ? true : false;
    if ( cpu.carry ) {
        *reg = base - *reg;
        if ( *reg & 0xF0 )
            *reg &= 7;
        reg++;
        len--;
        base--;
        while ( len-- ) {
            *reg = base - *reg;
            if ( *reg & 0xF0 )
                *reg &= 7;
            reg++;
        }
    }
}

inline void alu_not( byte* reg, int len )
{
    byte base = cpu.dec ? 9 : 15;

    while ( len-- ) {
        *reg = base - *reg;
        if ( *reg & 0xF0 )
            *reg &= 7;
        reg++;
    }
    cpu.carry = false;
}

inline void alu_and( byte* dest, byte* src, int len )
{
    while ( len-- ) {
        *dest++ &= *src++;
    }
}

inline void alu_or( byte* dest, byte* src, int len )
{
    while ( len-- ) {
        *dest++ |= *src++;
    }
}

inline void alu_sl( byte* reg, int len )
{
    while ( --len ) {
        reg[ len ] = reg[ len - 1 ];
    }
    reg[ 0 ] = 0;
}

inline void alu_slc( byte* reg, int len )
{
    byte tmp = reg[ len - 1 ];

    while ( --len ) {
        reg[ len ] = reg[ len - 1 ];
    }
    reg[ 0 ] = tmp;
}

inline void alu_sr( byte* reg, int len )
{
    if ( reg[ 0 ] )
        cpu.hst |= HST_SB;

    while ( --len ) {
        reg[ 0 ] = reg[ 1 ];
        reg++;
    }
    reg[ 0 ] = 0;
}

inline void alu_src( byte* reg, int len )
{
    byte tmp = reg[ 0 ];

    while ( --len ) {
        reg[ 0 ] = reg[ 1 ];
        reg++;
    }
    reg[ 0 ] = tmp;
}

inline void alu_srb( byte* reg, int len )
{
    if ( *reg & 1 )
        cpu.hst |= HST_SB;

    while ( --len ) {
        *reg >>= 1;
        if ( reg[ 1 ] & 1 )
            *reg |= 8;
        reg++;
    }
    *reg >>= 1;
}

#endif
