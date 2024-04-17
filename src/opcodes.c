#include <string.h>

#include "types.h"
#include "config.h"
#include "cpu.h"
#include "bus.h"
#include "keyboard.h"
#include "opcodes.h"
#include "opinline.h"

//		      P  WP  XS   X   S   M   B   W
static int fs[ 16 ] = { 0, 0, 2, 0, 15, 3, 0, 0, 0, 0, 2, 0, 15, 3, 0, 0 };
static int fl[ 16 ] = { 1, 1, 1, 3, 1, 12, 2, 16, 1, 1, 1, 3, 1, 12, 2, 16 };
//			      0	 1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
static const int regr[ 16 ] = { A, B, C, D, B, C, A, C, A, B, C, D, B, C, A, C };
static const int regs[ 16 ] = { B, C, A, C, A, B, C, D, B, C, A, C, A, B, C, D };
static const int regt[ 16 ] = { A, B, C, D, A, B, C, D, B, C, A, C, A, B, C, D };
static const int regu[ 16 ] = { A, B, C, D, B, C, A, C, B, C, A, C, A, B, C, D };
static const int regv[ 16 ] = { B, C, A, C, A, B, C, D, A, B, C, D, B, C, A, C };

#define REGr( i ) ( cpu.reg[ regr[ i ] ] )
#define REGs( i ) ( cpu.reg[ regs[ i ] ] )
#define REGt( i ) ( cpu.reg[ regt[ i ] ] )
#define REGu( i ) ( cpu.reg[ regu[ i ] ] )
#define REGv( i ) ( cpu.reg[ regv[ i ] ] )
#define REGrF( i, f ) ( REGr( i ) + fs[ f ] )
#define REGsF( i, f ) ( REGs( i ) + fs[ f ] )
#define REGtF( i, f ) ( REGt( i ) + fs[ f ] )
#define REGuF( i, f ) ( REGu( i ) + fs[ f ] )
#define REGvF( i, f ) ( REGv( i ) + fs[ f ] )

static inline void update_fields( void )
{
    fs[ 0 ] = fs[ 8 ] = cpu.p;
    fl[ 1 ] = fl[ 9 ] = cpu.p + 1;
}

static void op00( byte* opc ) // RTNSXM
{
    cpu.hst |= HST_XM;
    cpu.pc = rstk_pop();
    cpu.cycles += 9;
}

static void op01( byte* opc ) // RTN
{
    cpu.pc = rstk_pop();
    cpu.cycles += 9;
}

static void op02_3( byte* opc ) // RTNSC / RTNCC
{
    cpu.carry = ( opc[ 1 ] & 1 ) ? false : true;
    cpu.pc = rstk_pop();
    cpu.cycles += 9;
}

static void op04_5( byte* opc ) // SETHEX / SETDEC
{
    cpu.dec = ( opc[ 1 ] & 1 ) ? true : false;
    cpu.pc += 2;
    cpu.cycles += 3;
}

static void op06( byte* opc ) // RSTK=C
{
    rstk_push( nib_to_unsigned( cpu.reg[ C ], 5 ) );
    cpu.pc += 2;
    cpu.cycles += 8;
}

static void op07( byte* opc ) // C=RSTK
{
    unsigned_to_nib( cpu.reg[ C ], rstk_pop(), 5 );
    cpu.pc += 2;
    cpu.cycles += 8;
}

static void op08( byte* opc ) // CLRST
{
    reg_zero( cpu.st, 3 );
    cpu.pc += 2;
    cpu.cycles += 5;
}

static void op09( byte* opc ) // C=ST
{
    reg_cpy( cpu.reg[ C ], cpu.st, 3 );
    cpu.pc += 2;
    cpu.cycles += 5;
}

static void op0A( byte* opc ) // ST=C
{
    reg_cpy( cpu.st, cpu.reg[ C ], 3 );
    cpu.pc += 2;
    cpu.cycles += 5;
}

static void op0B( byte* opc ) // CSTEX
{
    reg_ex( cpu.reg[ C ], cpu.st, 3 );
    cpu.pc += 2;
    cpu.cycles += 5;
}

static void op0C( byte* opc ) // P=P+1
{
    if ( cpu.p != 0xF ) {
        cpu.p++;
        cpu.carry = false;
    } else {
        cpu.p = 0;
        cpu.carry = true;
    }
    update_fields();
    cpu.pc += 2;
    cpu.cycles += 3;
}

static void op0D( byte* opc ) // P=P-1
{
    if ( cpu.p ) {
        cpu.p--;
        cpu.carry = false;
    } else {
        cpu.p = 0xF;
        cpu.carry = true;
    }
    update_fields();
    cpu.pc += 2;
    cpu.cycles += 3;
}

static void op0E( byte* opc ) // r=r&s f/A / r=r!s f/A
{
    int len = ( opc[ 2 ] == 0xF ) ? 5 : fl[ opc[ 2 ] ];
    if ( !( opc[ 3 ] & 8 ) ) {
        alu_and( REGrF( opc[ 3 ], opc[ 2 ] ), REGsF( opc[ 3 ], opc[ 2 ] ), len );
    } else {
        alu_or( REGrF( opc[ 3 ], opc[ 2 ] ), REGsF( opc[ 3 ], opc[ 2 ] ), len );
    }
    cpu.pc += 4;
    cpu.cycles += 4 + len;
}

static void op0F( byte* opc ) // RTI
{
    // TODO: Implement RTI
    cpu.inte = true;
    cpu.pc = rstk_pop();
    cpu.cycles += 9;
    if ( ( cpu.keyintp && cpu.keyscan ) || kbd_on ) {
        cpu.keyintp = false;
        cpu_interrupt();
    }
}

static void op10( byte* opc ) // Ri=r
{
    int i = ( opc[ 2 ] & 7 ) > 4 ? opc[ 2 ] & 3 : opc[ 2 ] & 7;
    reg_cpy( cpu.reg_r[ i ], cpu.reg[ ( opc[ 2 ] & 8 ) >> 2 ], 16 );
    cpu.pc += 3;
    cpu.cycles += 19;
}

static void op11( byte* opc ) // r=Ri
{
    int i = ( opc[ 2 ] & 7 ) > 4 ? opc[ 2 ] & 3 : opc[ 2 ] & 7;
    reg_cpy( cpu.reg[ ( opc[ 2 ] & 8 ) >> 2 ], cpu.reg_r[ i ], 16 );
    cpu.pc += 3;
    cpu.cycles += 19;
}

static void op12( byte* opc ) // rRiEX
{
    int i = ( opc[ 2 ] & 7 ) > 4 ? opc[ 2 ] & 3 : opc[ 2 ] & 7;
    reg_ex( cpu.reg[ ( opc[ 2 ] & 8 ) >> 2 ], cpu.reg_r[ i ], 16 );
    cpu.pc += 3;
    cpu.cycles += 19;
}

static void op13a( byte* opc ) // Di=r / rDiEX
{
    address tmp = cpu.d[ opc[ 2 ] & 1 ];
    cpu.d[ opc[ 2 ] & 1 ] = nib_to_unsigned( cpu.reg[ ( opc[ 2 ] & 4 ) >> 1 ], 5 );
    if ( opc[ 2 ] & 2 )
        unsigned_to_nib( cpu.reg[ ( opc[ 2 ] & 4 ) >> 1 ], tmp, 5 );
    cpu.pc += 3;
    cpu.cycles += 8;
}

static void op13b( byte* opc ) // Di=rS / rDiEXS
{
    address tmp = cpu.d[ opc[ 2 ] & 1 ];
    cpu.d[ opc[ 2 ] & 1 ] &= 0xF0000;
    cpu.d[ opc[ 2 ] & 1 ] |= nib_to_unsigned( cpu.reg[ ( opc[ 2 ] & 4 ) >> 1 ], 4 );
    if ( opc[ 2 ] & 2 )
        unsigned_to_nib( cpu.reg[ ( opc[ 2 ] & 4 ) >> 1 ], tmp, 4 );
    cpu.pc += 3;
    cpu.cycles += 7;
}

static void op14( byte* opc ) // DATi=r A/B / r=DATi A/B
{
    if ( !( opc[ 2 ] & 2 ) ) {
        bus_write( cpu.reg[ ( opc[ 2 ] & 4 ) >> 1 ], cpu.d[ opc[ 2 ] & 1 ], ( opc[ 2 ] & 8 ) ? 2 : 5 );
    } else {
        bus_read( cpu.reg[ ( opc[ 2 ] & 4 ) >> 1 ], cpu.d[ opc[ 2 ] & 1 ], ( opc[ 2 ] & 8 ) ? 2 : 5 );
        cpu.cycles++;
    }
    cpu.pc += 3;
    cpu.cycles += ( opc[ 2 ] & 8 ) ? 14 : 17;
}

static void op15a( byte* opc ) // DATi=r f / r=DATi f
{
    if ( !( opc[ 2 ] & 2 ) ) {
        bus_write( cpu.reg[ ( opc[ 2 ] & 4 ) >> 1 ] + fs[ opc[ 3 ] ], cpu.d[ opc[ 2 ] & 1 ], fl[ opc[ 3 ] ] );
    } else {
        bus_read( cpu.reg[ ( opc[ 2 ] & 4 ) >> 1 ] + fs[ opc[ 3 ] ], cpu.d[ opc[ 2 ] & 1 ], fl[ opc[ 3 ] ] );
        cpu.cycles++;
    }
    cpu.pc += 4;
    cpu.cycles += 16 + fl[ opc[ 3 ] ];
}

static void op15b( byte* opc ) // DATi=r n / r=DATi n
{
    if ( !( opc[ 2 ] & 2 ) ) {
        bus_write( cpu.reg[ ( opc[ 2 ] & 4 ) >> 1 ], cpu.d[ opc[ 2 ] & 1 ], opc[ 3 ] + 1 );
    } else {
        bus_read( cpu.reg[ ( opc[ 2 ] & 4 ) >> 1 ], cpu.d[ opc[ 2 ] & 1 ], opc[ 3 ] + 1 );
        cpu.cycles++;
    }
    cpu.pc += 4;
    cpu.cycles += 15 + opc[ 3 ] + 1;
}

static void op16_7( byte* opc ) // Di=Di+
{
    cpu.d[ opc[ 1 ] & 1 ] += opc[ 2 ] + 1;
    if ( cpu.d[ opc[ 1 ] & 1 ] & ~0xFFFFF ) {
        cpu.d[ opc[ 1 ] & 1 ] &= 0xFFFFF;
        cpu.carry = true;
    } else {
        cpu.carry = false;
    }
    cpu.pc += 3;
    cpu.cycles += 7;
}

static void op18_C( byte* opc ) // Di=Di-
{
    cpu.d[ ( opc[ 1 ] >> 2 ) & 1 ] -= opc[ 2 ] + 1;
    if ( cpu.d[ ( opc[ 1 ] >> 2 ) & 1 ] & ~0xFFFFF ) {
        cpu.d[ ( opc[ 1 ] >> 2 ) & 1 ] &= 0xFFFFF;
        cpu.carry = true;
    } else {
        cpu.carry = false;
    }
    cpu.pc += 3;
    cpu.cycles += 7;
}

static void op19_D( byte* opc ) // Di=(2)
{
    cpu.d[ ( opc[ 1 ] >> 2 ) & 1 ] &= 0xFFF00;
    cpu.d[ ( opc[ 1 ] >> 2 ) & 1 ] |= nib_to_unsigned( opc + 2, 2 );
    cpu.pc += 4;
    cpu.cycles += 4;
}

static void op1A_E( byte* opc ) // Di=(4)
{
    cpu.d[ ( opc[ 1 ] >> 2 ) & 1 ] &= 0xF0000;
    cpu.d[ ( opc[ 1 ] >> 2 ) & 1 ] |= nib_to_unsigned( opc + 2, 4 );
    cpu.pc += 6;
    cpu.cycles += 6;
}

static void op1B_F( byte* opc ) // Di=(5)
{
    cpu.d[ ( opc[ 1 ] >> 2 ) & 1 ] = nib_to_unsigned( opc + 2, 5 );
    cpu.pc += 7;
    cpu.cycles += 7;
}

static void op2( byte* opc ) // P=
{
    cpu.p = opc[ 1 ];
    update_fields();
    cpu.pc += 2;
    cpu.cycles += 2;
}

static void op3( byte* opc ) // LC
{
    load( cpu.reg[ C ], opc + 2, cpu.p, opc[ 1 ] + 1 );
    cpu.pc += 2 + opc[ 1 ] + 1;
    cpu.cycles += 3 + opc[ 1 ] + 1;
}

static void op4( byte* opc ) // GOC
{
    goyes( opc, 1 );
    cpu.cycles += 3;
}

static void op5( byte* opc ) // GONC
{
    cpu.carry = !cpu.carry;
    goyes( opc, 1 );
    cpu.carry = !cpu.carry;
    cpu.cycles += 3;
}

static void op6( byte* opc ) // GOTO
{
    cpu.pc += nib_to_signed( opc + 1, 3 ) + 1;
    cpu.cycles += 11;
}

static void op7( byte* opc ) // GOSUB
{
    rstk_push( cpu.pc + 4 );
    cpu.pc += nib_to_signed( opc + 1, 3 ) + 4;
    cpu.cycles += 12;
}

static void op800_1( byte* opc ) // OUT=CS / OUT=C
{
    reg_cpy( cpu.out, cpu.reg[ C ], opc[ 2 ] ? 3 : 1 );
    cpu.pc += 3;
    cpu.cycles += opc[ 2 ] ? 6 : 4;
    kbd_out_changed();
}

static void op802_3( byte* opc ) // r=IN
{
    reg_cpy( cpu.reg[ ( opc[ 2 ] & 1 ) << 1 ], cpu.in, 4 );
    if ( !( cpu.pc & 1 ) )
        cpu.pc += 3; // Saturn bug!
    cpu.cycles += 7;
}

static void op804( byte* opc ) // UNCNFG
{
    bus_unconfigure( nib_to_unsigned( cpu.reg[ C ], 5 ) );
    cpu.pc += 3;
    cpu.cycles += 12;
}

static void op805( byte* opc ) // CONFIG
{
    bus_configure( nib_to_unsigned( cpu.reg[ C ], 5 ) );
    cpu.pc += 3;
    cpu.cycles += 11;
}

static void op806( byte* opc ) // C=ID
{
    unsigned_to_nib( cpu.reg[ C ], bus_get_id(), 5 );
    cpu.pc += 3;
    cpu.cycles += 11;
}

static void op807( byte* opc ) // SHUTDN
{
    // TODO: Fix SHUTDN
    cpu.shutdown = config.allow_shutdn && ( !cpu.in[ 0 ] && !cpu.in[ 1 ] && !cpu.in[ 3 ] );

    cpu.pc += 3;
    cpu.cycles += 5;
}

static void op8080( byte* opc ) // INTON
{
    cpu.keyscan = true;
    cpu.pc += 4;
    cpu.cycles += 5;
    if ( cpu.keyintp ) {
        cpu.keyintp = false;
        cpu_interrupt();
    }
}

static void op8081( byte* opc ) // RSI
{
    // TODO: Implement RSI
    // Note: Is the opcode 80810?
    // if (opc[4] == 0)
    cpu.pc += 5;
    cpu.cycles += 6;
}

static void op8082( byte* opc ) // LA
{
    load( cpu.reg[ A ], opc + 5, cpu.p, opc[ 4 ] + 1 );
    cpu.pc += 5 + opc[ 4 ] + 1;
    cpu.cycles += 6 + opc[ 4 ] + 1;
}

static void op8084_5_8_9( byte* opc ) // rBIT=b
{
    reg_bit( cpu.reg[ ( opc[ 3 ] & 8 ) >> 2 ], opc[ 4 ], opc[ 3 ] & 1 );
    cpu.pc += 5;
    cpu.cycles += 6;
}

static void op8086_7_A_B( byte* opc ) // ?rBIT=b
{
    comp_bit_zero( cpu.reg[ ( opc[ 3 ] & 8 ) >> 2 ], opc[ 4 ] );
    if ( opc[ 3 ] & 1 )
        cpu.carry = !cpu.carry;
    goyes( opc, 5 );
    cpu.cycles += 9;
}

static void op808C_E( byte* opc ) // PC=(r)
{
    byte tmp[ 5 ];
    bus_read( tmp, nib_to_unsigned( cpu.reg[ opc[ 3 ] & 2 ], 5 ), 5 );
    cpu.pc = nib_to_unsigned( tmp, 5 );
    cpu.cycles += 23;
}

static void op808F( byte* opc ) // INTOFF
{
    // TODO: Implement INTOFF
    cpu.keyscan = false;
    cpu.pc += 4;
    cpu.cycles += 5;
}

static void op809( byte* opc ) // C+P+1
{
    alu_add_con( cpu.reg[ C ], cpu.p, 0, 5 );
    cpu.pc += 3;
    cpu.cycles += 8;
}

static void op80A( byte* opc ) // RESET
{
    bus_reset();
    cpu.pc += 3;
    cpu.cycles += 6;
}

static void op80C( byte* opc ) // C=P
{
    cpu.reg[ C ][ opc[ 3 ] ] = cpu.p;
    cpu.pc += 4;
    cpu.cycles += 6;
}

static void op80D( byte* opc ) // P=C
{
    cpu.p = cpu.reg[ C ][ opc[ 3 ] ];
    update_fields();
    cpu.pc += 4;
    cpu.cycles += 6;
}

static void op80F( byte* opc ) // CPEX
{
    byte tmp = cpu.reg[ C ][ opc[ 3 ] ];
    cpu.reg[ C ][ opc[ 3 ] ] = cpu.p;
    cpu.p = tmp;
    update_fields();
    cpu.pc += 4;
    cpu.cycles += 6;
}

static void op810_3( byte* opc ) // rSLC
{
    alu_slc( cpu.reg[ opc[ 2 ] & 3 ], 16 );
    cpu.pc += 3;
    cpu.cycles += 21;
}

static void op814_7( byte* opc ) // rSRC
{
    alu_src( cpu.reg[ opc[ 2 ] & 3 ], 16 );
    cpu.pc += 3;
    cpu.cycles += 21;
}

static void op818( byte* opc ) // r=r+CON f/A / r=r-CON f/A
{
    int len = ( opc[ 3 ] == 0xF ) ? 5 : ( ( fl[ opc[ 3 ] ] == 1 ) ? 17 : fl[ opc[ 3 ] ] );
    // Note: What happens if opc[4]&4
    if ( !( opc[ 4 ] & 8 ) ) {
        alu_add_con( cpu.reg[ opc[ 4 ] & 3 ], opc[ 5 ], fs[ opc[ 3 ] ], len );
    } else {
        alu_sub_con( cpu.reg[ opc[ 4 ] & 3 ], opc[ 5 ], fs[ opc[ 3 ] ], len );
    }
    cpu.pc += 6;
    cpu.cycles += 5 + len;
}

static void op819( byte* opc ) // rSRB f/A
{
    int len = ( opc[ 3 ] == 0xF ) ? 5 : fl[ opc[ 3 ] ];
    alu_srb( cpu.reg[ opc[ 4 ] & 3 ] + fs[ opc[ 3 ] ], len );
    cpu.pc += 5;
    cpu.cycles += 20;
}

static void op81Af0( byte* opc ) // Ri=r f/A
{
    int i = ( opc[ 5 ] & 7 ) > 4 ? opc[ 5 ] & 3 : opc[ 5 ] & 7;
    int len = ( opc[ 3 ] == 0xF ) ? 5 : fl[ opc[ 3 ] ];
    reg_cpy( cpu.reg_r[ i ] + fs[ opc[ 3 ] ], cpu.reg[ ( opc[ 5 ] & 8 ) >> 2 ] + fs[ opc[ 3 ] ], len );
    cpu.pc += 6;
    cpu.cycles += 6 + len;
}

static void op81Af1( byte* opc ) // r=Ri f/A
{
    int i = ( opc[ 5 ] & 7 ) > 4 ? opc[ 5 ] & 3 : opc[ 5 ] & 7;
    int len = ( opc[ 3 ] == 0xF ) ? 5 : fl[ opc[ 3 ] ];
    reg_cpy( cpu.reg[ ( opc[ 5 ] & 8 ) >> 2 ] + fs[ opc[ 3 ] ], cpu.reg_r[ i ] + fs[ opc[ 3 ] ], len );
    cpu.pc += 6;
    cpu.cycles += 6 + len;
}

static void op81Af2( byte* opc ) // rRiEX f/A
{
    int i = ( opc[ 5 ] & 7 ) > 4 ? opc[ 5 ] & 3 : opc[ 5 ] & 7;
    int len = ( opc[ 3 ] == 0xF ) ? 5 : fl[ opc[ 3 ] ];
    reg_ex( cpu.reg[ ( opc[ 5 ] & 8 ) >> 2 ] + fs[ opc[ 3 ] ], cpu.reg_r[ i ] + fs[ opc[ 3 ] ], len );
    cpu.pc += 6;
    cpu.cycles += 6 + len;
}

static void op81B2_3( byte* opc ) // PC=r
{
    cpu.pc = nib_to_unsigned( cpu.reg[ ( opc[ 3 ] & 1 ) << 1 ], 5 );
    cpu.cycles += 16;
}

static void op81B4_5( byte* opc ) // r=PC
{
    cpu.pc += 4;
    unsigned_to_nib( cpu.reg[ ( opc[ 3 ] & 1 ) << 1 ], cpu.pc, 5 );
    cpu.cycles += 9;
}

static void op81C_F( byte* opc ) // rSRB
{
    alu_srb( cpu.reg[ opc[ 2 ] & 3 ], 16 );
    cpu.pc += 3;
    cpu.cycles += 20;
}

static void op82( byte* opc ) // HST=0
{
    cpu.hst &= ~opc[ 2 ];
    cpu.pc += 3;
    cpu.cycles += 3;
}

static void op83( byte* opc ) // ?HST=0
{
    cpu.carry = ( cpu.hst & opc[ 2 ] ) ? false : true;
    goyes( opc, 3 );
    cpu.cycles += 6;
}

static void op84_5( byte* opc ) // ST=b
{
    reg_bit( cpu.st, opc[ 2 ], opc[ 1 ] & 1 );
    cpu.pc += 3;
    cpu.cycles += 4;
}

static void op86_7( byte* opc ) // ?ST=b
{
    comp_bit_zero( cpu.st, opc[ 2 ] );
    if ( opc[ 1 ] & 1 )
        cpu.carry = !cpu.carry;
    goyes( opc, 3 );
    cpu.cycles += 7;
}

static void op88_9( byte* opc ) // ?P# / ?P=
{
    cpu.carry = ( cpu.p == opc[ 2 ] ) == ( opc[ 1 ] & 1 ) ? true : false;
    goyes( opc, 3 );
    cpu.cycles += 6;
}

static void op8A( byte* opc ) // ?u=v A / ?u#v A / ?u=0 A / ?u#0 A
{
    if ( !( opc[ 2 ] & 8 ) ) {
        comp_eq( REGu( opc[ 2 ] ), REGv( opc[ 2 ] ), 5 );
    } else {
        comp_zero( cpu.reg[ opc[ 2 ] & 3 ], 5 );
    }
    if ( opc[ 2 ] & 4 )
        cpu.carry = !cpu.carry;
    goyes( opc, 3 );
    cpu.cycles += 11;
}

static void op8B( byte* opc ) // ?u>v A / u<v A / u>=v A / u<=v A
{
    comp_gt( REGu( opc[ 2 ] ), REGv( opc[ 2 ] ), 5 );
    if ( opc[ 2 ] & 8 )
        cpu.carry = !cpu.carry;
    goyes( opc, 3 );
    cpu.cycles += 11;
}

static void op8C( byte* opc ) // GOLONG
{
    cpu.pc += nib_to_signed( opc + 2, 4 ) + 2;
    cpu.cycles += 14;
}

static void op8D( byte* opc ) // GOVLNG
{
    cpu.pc = nib_to_unsigned( opc + 2, 5 );
    cpu.cycles += 14;
}

static void op8E( byte* opc ) // GOSUBL
{
    rstk_push( cpu.pc + 6 );
    cpu.pc += nib_to_signed( opc + 2, 4 ) + 6;
    cpu.cycles += 14;
}

static void op8F( byte* opc ) // GOSBVL
{
    rstk_push( cpu.pc + 7 );
    cpu.pc = nib_to_unsigned( opc + 2, 5 );
    cpu.cycles += 15;
}

static void op9a( byte* opc ) // ?u=v f / ?u#v f / ?u=0 f / ?u#0 f
{
    if ( !( opc[ 2 ] & 8 ) ) {
        comp_eq( REGuF( opc[ 2 ], opc[ 1 ] ), REGvF( opc[ 2 ], opc[ 1 ] ), fl[ opc[ 1 ] ] );
    } else {
        comp_zero( cpu.reg[ opc[ 2 ] & 3 ] + fs[ opc[ 1 ] ], fl[ opc[ 1 ] ] );
    }
    if ( opc[ 2 ] & 4 )
        cpu.carry = !cpu.carry;
    goyes( opc, 3 );
    cpu.cycles += 6 + fl[ opc[ 1 ] ];
}

static void op9b( byte* opc ) // ?u>v f / u<v f / u>=v f / u<=v f
{
    comp_gt( REGuF( opc[ 2 ], opc[ 1 ] ), REGvF( opc[ 2 ], opc[ 1 ] ), fl[ opc[ 1 ] ] );
    if ( opc[ 2 ] & 8 )
        cpu.carry = !cpu.carry;
    goyes( opc, 3 );
    cpu.cycles += 6 + fl[ opc[ 1 ] ];
}

static void opAa( byte* opc ) // t=t+v f / t=t-1 f
{
    switch ( opc[ 2 ] & 0xC ) {
        case 0x0:
        case 0x4:
        case 0x8:
            alu_add( REGtF( opc[ 2 ], opc[ 1 ] ), REGvF( opc[ 2 ], opc[ 1 ] ), fl[ opc[ 1 ] ] );
            break;
        case 0xC:
            alu_dec( REGtF( opc[ 2 ], opc[ 1 ] ), fl[ opc[ 1 ] ] );
            break;
    }
    cpu.pc += 3;
    cpu.cycles += 3 + fl[ opc[ 1 ] ];
}

static void opAb( byte* opc ) // t=0 f / t=r f / trEX f
{
    switch ( opc[ 2 ] & 0xC ) {
        case 0x0:
            reg_zero( REGtF( opc[ 2 ], opc[ 1 ] ), fl[ opc[ 1 ] ] );
            break;
        case 0x4:
        case 0x8:
            reg_cpy( REGtF( opc[ 2 ], opc[ 1 ] ), REGrF( opc[ 2 ], opc[ 1 ] ), fl[ opc[ 1 ] ] );
            break;
        case 0xC:
            reg_ex( REGtF( opc[ 2 ], opc[ 1 ] ), REGrF( opc[ 2 ], opc[ 1 ] ), fl[ opc[ 1 ] ] );
            break;
    }
    cpu.pc += 3;
    cpu.cycles += 3 + fl[ opc[ 1 ] ];
}

static void opBa( byte* opc ) // t=t-v f / t=t+1 f / t=v-t f
{
    switch ( opc[ 2 ] & 0xC ) {
        case 0x0:
        case 0x8:
            alu_sub( REGtF( opc[ 2 ], opc[ 1 ] ), REGvF( opc[ 2 ], opc[ 1 ] ), fl[ opc[ 1 ] ] );
            break;
        case 0x4:
            alu_inc( REGtF( opc[ 2 ], opc[ 1 ] ), fl[ opc[ 1 ] ] );
            break;
        case 0xC:
            alu_sub2( REGtF( opc[ 2 ], opc[ 1 ] ), REGvF( opc[ 2 ], opc[ 1 ] ), fl[ opc[ 1 ] ] );
            break;
    }
    cpu.pc += 3;
    cpu.cycles += 3 + fl[ opc[ 1 ] ];
}

static void opBb0_3( byte* opc ) // rSL f
{
    alu_sl( cpu.reg[ opc[ 2 ] & 3 ] + fs[ opc[ 1 ] ], fl[ opc[ 1 ] ] );
    cpu.pc += 3;
    cpu.cycles += 3 + fl[ opc[ 1 ] ];
}

static void opBb4_7( byte* opc ) // rSR f
{
    alu_sr( cpu.reg[ opc[ 2 ] & 3 ] + fs[ opc[ 1 ] ], fl[ opc[ 1 ] ] );
    cpu.pc += 3;
    cpu.cycles += 3 + fl[ opc[ 1 ] ];
}

static void opBb8_B( byte* opc ) // r=-r f
{
    alu_neg( cpu.reg[ opc[ 2 ] & 3 ] + fs[ opc[ 1 ] ], fl[ opc[ 1 ] ] );
    cpu.pc += 3;
    cpu.cycles += 3 + fl[ opc[ 1 ] ];
}

static void opBbC_F( byte* opc ) // r=-r-1 f
{
    alu_not( cpu.reg[ opc[ 2 ] & 3 ] + fs[ opc[ 1 ] ], fl[ opc[ 1 ] ] );
    cpu.pc += 3;
    cpu.cycles += 3 + fl[ opc[ 1 ] ];
}

static void opC( byte* opc ) // t=t+v A / t=t-1 A
{
    switch ( opc[ 1 ] & 0xC ) {
        case 0x0:
        case 0x4:
        case 0x8:
            alu_add( REGt( opc[ 1 ] ), REGv( opc[ 1 ] ), 5 );
            break;
        case 0xC:
            alu_dec( REGt( opc[ 1 ] ), 5 );
            break;
    }
    cpu.pc += 2;
    cpu.cycles += 7;
}

static void opD( byte* opc ) // t=0 A / t=r A / trEX A
{
    switch ( opc[ 1 ] & 0xC ) {
        case 0x0:
            reg_zero( REGt( opc[ 1 ] ), 5 );
            break;
        case 0x4:
        case 0x8:
            reg_cpy( REGt( opc[ 1 ] ), REGr( opc[ 1 ] ), 5 );
            break;
        case 0xC:
            reg_ex( REGt( opc[ 1 ] ), REGr( opc[ 1 ] ), 5 );
            break;
    }
    cpu.pc += 2;
    cpu.cycles += 7;
}

static void opE( byte* opc ) // t=t-v A / t=t+1 A / t=v-t A
{
    switch ( opc[ 1 ] & 0xC ) {
        case 0x0:
        case 0x8:
            alu_sub( REGt( opc[ 1 ] ), REGv( opc[ 1 ] ), 5 );
            break;
        case 0x4:
            alu_inc( REGt( opc[ 1 ] ), 5 );
            break;
        case 0xC:
            alu_sub2( REGt( opc[ 1 ] ), REGv( opc[ 1 ] ), 5 );
            break;
    }
    cpu.pc += 2;
    cpu.cycles += 7;
}

static void opF0_3( byte* opc ) // rSL f
{
    alu_sl( cpu.reg[ opc[ 1 ] & 3 ], 5 );
    cpu.pc += 2;
    cpu.cycles += 7;
}

static void opF4_7( byte* opc ) // rSR f
{
    alu_sr( cpu.reg[ opc[ 1 ] & 3 ], 5 );
    cpu.pc += 2;
    cpu.cycles += 7;
}

static void opF8_B( byte* opc ) // r=-r A
{
    alu_neg( cpu.reg[ opc[ 1 ] & 3 ], 5 );
    cpu.pc += 2;
    cpu.cycles += 7;
}

static void opFC_F( byte* opc ) // r=-r-1 A
{
    alu_not( cpu.reg[ opc[ 1 ] & 3 ], 5 );
    cpu.pc += 2;
    cpu.cycles += 7;
}

static Opcode opcodes0[ 16 ] = {
    {.exec = op00,   .next = NULL, .dissasm = "RTNSXM"            },
    {.exec = op01,   .next = NULL, .dissasm = "RTN"               },
    {.exec = op02_3, .next = NULL, .dissasm = "RTNSC"             },
    {.exec = op02_3, .next = NULL, .dissasm = "RTNCC"             },
    {.exec = op04_5, .next = NULL, .dissasm = "SETHEX"            },
    {.exec = op04_5, .next = NULL, .dissasm = "SETDEC"            },
    {.exec = op06,   .next = NULL, .dissasm = "RSTK=C"            },
    {.exec = op07,   .next = NULL, .dissasm = "C=RSTK"            },
    {.exec = op08,   .next = NULL, .dissasm = "CLRST"             },
    {.exec = op09,   .next = NULL, .dissasm = "C=ST"              },
    {.exec = op0A,   .next = NULL, .dissasm = "ST=C"              },
    {.exec = op0B,   .next = NULL, .dissasm = "CSTEX"             },
    {.exec = op0C,   .next = NULL, .dissasm = "P=P+1"             },
    {.exec = op0D,   .next = NULL, .dissasm = "P=P-1"             },
    {.exec = op0E,   .next = NULL, .dissasm = "%r3=%r3%&3%s3 %F2A"},
    {.exec = op0F,   .next = NULL, .dissasm = "RTI"               }
};

static Opcode opcodes13[ 16 ] = {
    {.exec = op13a, .next = NULL, .dissasm = "D0=A"  },
    {.exec = op13a, .next = NULL, .dissasm = "D1=A"  },
    {.exec = op13a, .next = NULL, .dissasm = "AD0EX" },
    {.exec = op13a, .next = NULL, .dissasm = "AD1EX" },
    {.exec = op13a, .next = NULL, .dissasm = "D0=C"  },
    {.exec = op13a, .next = NULL, .dissasm = "D1=C"  },
    {.exec = op13a, .next = NULL, .dissasm = "CD0EX" },
    {.exec = op13a, .next = NULL, .dissasm = "CD1EX" },
    {.exec = op13b, .next = NULL, .dissasm = "D0=AS" },
    {.exec = op13b, .next = NULL, .dissasm = "D1=AS" },
    {.exec = op13b, .next = NULL, .dissasm = "AD0EXS"},
    {.exec = op13b, .next = NULL, .dissasm = "AD1EXS"},
    {.exec = op13b, .next = NULL, .dissasm = "D0=CS" },
    {.exec = op13b, .next = NULL, .dissasm = "D1=CS" },
    {.exec = op13b, .next = NULL, .dissasm = "CD0EXS"},
    {.exec = op13b, .next = NULL, .dissasm = "CD1EXS"}
};

static Opcode opcodes15[ 16 ] = {
    {.exec = op15a, .next = NULL, .dissasm = "%D2 %F3"  },
    {.exec = op15a, .next = NULL, .dissasm = "%D2 %F3"  },
    {.exec = op15a, .next = NULL, .dissasm = "%D2 %F3"  },
    {.exec = op15a, .next = NULL, .dissasm = "%D2 %F3"  },
    {.exec = op15a, .next = NULL, .dissasm = "%D2 %F3"  },
    {.exec = op15a, .next = NULL, .dissasm = "%D2 %F3"  },
    {.exec = op15a, .next = NULL, .dissasm = "%D2 %F3"  },
    {.exec = op15a, .next = NULL, .dissasm = "%D2 %F3"  },
    {.exec = op15b, .next = NULL, .dissasm = "%D2 %I3+1"},
    {.exec = op15b, .next = NULL, .dissasm = "%D2 %I3+1"},
    {.exec = op15b, .next = NULL, .dissasm = "%D2 %I3+1"},
    {.exec = op15b, .next = NULL, .dissasm = "%D2 %I3+1"},
    {.exec = op15b, .next = NULL, .dissasm = "%D2 %I3+1"},
    {.exec = op15b, .next = NULL, .dissasm = "%D2 %I3+1"},
    {.exec = op15b, .next = NULL, .dissasm = "%D2 %I3+1"},
    {.exec = op15b, .next = NULL, .dissasm = "%D2 %I3+1"}
};

static Opcode opcodes1[ 16 ] = {
    {.exec = op10,   .next = NULL,      .dissasm = "R%i2=%a2"    },
    {.exec = op11,   .next = NULL,      .dissasm = "%a2=R%i2"    },
    {.exec = op12,   .next = NULL,      .dissasm = "%a2R%i2EX"   },
    {.exec = NULL,   .next = opcodes13, .dissasm = NULL          },
    {.exec = op14,   .next = NULL,      .dissasm = "%D2 %B2"     },
    {.exec = NULL,   .next = opcodes15, .dissasm = NULL          },
    {.exec = op16_7, .next = NULL,      .dissasm = "D0=D0+ %I2+1"},
    {.exec = op16_7, .next = NULL,      .dissasm = "D1=D1+ %I2+1"},
    {.exec = op18_C, .next = NULL,      .dissasm = "D0=D0- %I2+1"},
    {.exec = op19_D, .next = NULL,      .dissasm = "D0=(2) %X22" },
    {.exec = op1A_E, .next = NULL,      .dissasm = "D0=(4) %X24" },
    {.exec = op1B_F, .next = NULL,      .dissasm = "D0=(5) %X25" },
    {.exec = op18_C, .next = NULL,      .dissasm = "D1=D1- %I2+1"},
    {.exec = op19_D, .next = NULL,      .dissasm = "D1=(2) %X22" },
    {.exec = op1A_E, .next = NULL,      .dissasm = "D1=(4) %X24" },
    {.exec = op1B_F, .next = NULL,      .dissasm = "D1=(5) %X25" },
};

static Opcode opcodes808[ 16 ] = {
    {.exec = op8080,       .next = NULL, .dissasm = "INTON"                      },
    {.exec = op8081,       .next = NULL, .dissasm = "RSI"                        },
    {.exec = op8082,       .next = NULL, .dissasm = "LA #%N4"                    },
    {.exec = NULL,         .next = NULL, .dissasm = NULL                         },
    {.exec = op8084_5_8_9, .next = NULL, .dissasm = "ABIT=0 %I4"                 },
    {.exec = op8084_5_8_9, .next = NULL, .dissasm = "ABIT=1 %I4"                 },
    {.exec = op8086_7_A_B, .next = NULL, .dissasm = "?ABIT=0 %I4, %G52YES %T52+5"},
    {.exec = op8086_7_A_B, .next = NULL, .dissasm = "?ABIT=1 %I4, %G52YES %T52+5"},
    {.exec = op8084_5_8_9, .next = NULL, .dissasm = "CBIT=0 %I4"                 },
    {.exec = op8084_5_8_9, .next = NULL, .dissasm = "CBIT=1 %I4"                 },
    {.exec = op8086_7_A_B, .next = NULL, .dissasm = "?CBIT=0 %I4, %G52YES %T52+5"},
    {.exec = op8086_7_A_B, .next = NULL, .dissasm = "?CBIT=1 %I4, %G52YES %T52+5"},
    {.exec = op808C_E,     .next = NULL, .dissasm = "PC=(A)"                     },
    {.exec = NULL,         .next = NULL, .dissasm = NULL                         },
    {.exec = op808C_E,     .next = NULL, .dissasm = "PC=(C)"                     },
    {.exec = op808F,       .next = NULL, .dissasm = "INTOFF"                     }
};

static Opcode opcodes80[ 16 ] = {
    {.exec = op800_1, .next = NULL,       .dissasm = "OUT=CS"  },
    {.exec = op800_1, .next = NULL,       .dissasm = "OUT=C"   },
    {.exec = op802_3, .next = NULL,       .dissasm = "A=IN"    },
    {.exec = op802_3, .next = NULL,       .dissasm = "C=IN"    },
    {.exec = op804,   .next = NULL,       .dissasm = "UNCONFIG"},
    {.exec = op805,   .next = NULL,       .dissasm = "CONFIG"  },
    {.exec = op806,   .next = NULL,       .dissasm = "C=ID"    },
    {.exec = op807,   .next = NULL,       .dissasm = "SHUTDN"  },
    {.exec = NULL,    .next = opcodes808, .dissasm = NULL      },
    {.exec = op809,   .next = NULL,       .dissasm = "C+P+1"   },
    {.exec = op80A,   .next = NULL,       .dissasm = "RESET"   },
    {.exec = NULL,    .next = NULL,       .dissasm = NULL      },
    {.exec = op80C,   .next = NULL,       .dissasm = "C=P %I3" },
    {.exec = op80D,   .next = NULL,       .dissasm = "P=C %I3" },
    {.exec = NULL,    .next = NULL,       .dissasm = NULL      },
    {.exec = op80F,   .next = NULL,       .dissasm = "CPEX %I3"}
};

static Opcode opcodes81Af[ 16 ] = {
    {.exec = op81Af0, .next = NULL, .dissasm = "R%i5=%a5 %F3A" },
    {.exec = op81Af1, .next = NULL, .dissasm = "%a5=R%i5 %F3A" },
    {.exec = op81Af2, .next = NULL, .dissasm = "%a5R%i5EX %F3A"},
    {.exec = NULL,    .next = NULL, .dissasm = NULL            },
    {.exec = NULL,    .next = NULL, .dissasm = NULL            },
    {.exec = NULL,    .next = NULL, .dissasm = NULL            },
    {.exec = NULL,    .next = NULL, .dissasm = NULL            },
    {.exec = NULL,    .next = NULL, .dissasm = NULL            },
    {.exec = NULL,    .next = NULL, .dissasm = NULL            },
    {.exec = NULL,    .next = NULL, .dissasm = NULL            },
    {.exec = NULL,    .next = NULL, .dissasm = NULL            },
    {.exec = NULL,    .next = NULL, .dissasm = NULL            },
    {.exec = NULL,    .next = NULL, .dissasm = NULL            },
    {.exec = NULL,    .next = NULL, .dissasm = NULL            },
    {.exec = NULL,    .next = NULL, .dissasm = NULL            },
    {.exec = NULL,    .next = NULL, .dissasm = NULL            }
};

static Opcode opcodes81A[ 16 ] = {
    {.exec = NULL, .next = opcodes81Af, .dissasm = NULL},
    {.exec = NULL, .next = opcodes81Af, .dissasm = NULL},
    {.exec = NULL, .next = opcodes81Af, .dissasm = NULL},
    {.exec = NULL, .next = opcodes81Af, .dissasm = NULL},
    {.exec = NULL, .next = opcodes81Af, .dissasm = NULL},
    {.exec = NULL, .next = opcodes81Af, .dissasm = NULL},
    {.exec = NULL, .next = opcodes81Af, .dissasm = NULL},
    {.exec = NULL, .next = opcodes81Af, .dissasm = NULL},
    {.exec = NULL, .next = opcodes81Af, .dissasm = NULL},
    {.exec = NULL, .next = opcodes81Af, .dissasm = NULL},
    {.exec = NULL, .next = opcodes81Af, .dissasm = NULL},
    {.exec = NULL, .next = opcodes81Af, .dissasm = NULL},
    {.exec = NULL, .next = opcodes81Af, .dissasm = NULL},
    {.exec = NULL, .next = opcodes81Af, .dissasm = NULL},
    {.exec = NULL, .next = opcodes81Af, .dissasm = NULL},
    {.exec = NULL, .next = opcodes81Af, .dissasm = NULL}
};

static Opcode opcodes81B[ 16 ] = {
    {.exec = NULL,     .next = NULL, .dissasm = NULL  },
    {.exec = NULL,     .next = NULL, .dissasm = NULL  },
    {.exec = op81B2_3, .next = NULL, .dissasm = "PC=A"},
    {.exec = op81B2_3, .next = NULL, .dissasm = "PC=C"},
    {.exec = op81B4_5, .next = NULL, .dissasm = "A=PC"},
    {.exec = op81B4_5, .next = NULL, .dissasm = "C=PC"},
    {.exec = NULL,     .next = NULL, .dissasm = NULL  },
    {.exec = NULL,     .next = NULL, .dissasm = NULL  },
    {.exec = NULL,     .next = NULL, .dissasm = NULL  },
    {.exec = NULL,     .next = NULL, .dissasm = NULL  },
    {.exec = NULL,     .next = NULL, .dissasm = NULL  },
    {.exec = NULL,     .next = NULL, .dissasm = NULL  },
    {.exec = NULL,     .next = NULL, .dissasm = NULL  },
    {.exec = NULL,     .next = NULL, .dissasm = NULL  },
    {.exec = NULL,     .next = NULL, .dissasm = NULL  },
    {.exec = NULL,     .next = NULL, .dissasm = NULL  }
};

static Opcode opcodes81[ 16 ] = {
    {.exec = op810_3, .next = NULL,       .dissasm = "ASLC"                     },
    {.exec = op810_3, .next = NULL,       .dissasm = "BSLC"                     },
    {.exec = op810_3, .next = NULL,       .dissasm = "CSLC"                     },
    {.exec = op810_3, .next = NULL,       .dissasm = "DSLC"                     },
    {.exec = op814_7, .next = NULL,       .dissasm = "ASRC"                     },
    {.exec = op814_7, .next = NULL,       .dissasm = "BSRC"                     },
    {.exec = op814_7, .next = NULL,       .dissasm = "CSRC"                     },
    {.exec = op814_7, .next = NULL,       .dissasm = "DSRC"                     },
    {.exec = op818,   .next = NULL,       .dissasm = "%u4=%u4%~4CON %F3A, %I6+1"},
    {.exec = op819,   .next = NULL,       .dissasm = "%u4SRB %F3A"              },
    {.exec = NULL,    .next = opcodes81A, .dissasm = NULL                       },
    {.exec = NULL,    .next = opcodes81B, .dissasm = NULL                       },
    {.exec = op81C_F, .next = NULL,       .dissasm = "ASRB"                     },
    {.exec = op81C_F, .next = NULL,       .dissasm = "BSRB"                     },
    {.exec = op81C_F, .next = NULL,       .dissasm = "CSRB"                     },
    {.exec = op81C_F, .next = NULL,       .dissasm = "DSRB"                     }
};

static Opcode opcodes8[ 16 ] = {
    {.exec = NULL,   .next = opcodes80, .dissasm = NULL                          },
    {.exec = NULL,   .next = opcodes81, .dissasm = NULL                          },
    {.exec = op82,   .next = NULL,      .dissasm = "HST=0 #%X21"                 },
    {.exec = op83,   .next = NULL,      .dissasm = "?HST=0 #%X21, %G32YES %T32+3"},
    {.exec = op84_5, .next = NULL,      .dissasm = "ST=0 %I2"                    },
    {.exec = op84_5, .next = NULL,      .dissasm = "ST=1 %I2"                    },
    {.exec = op86_7, .next = NULL,      .dissasm = "?ST=0 %I2, %G32YES %T32+3"   },
    {.exec = op86_7, .next = NULL,      .dissasm = "?ST=1 %I2, %G32YES %T32+3"   },
    {.exec = op88_9, .next = NULL,      .dissasm = "?P# %I2, %G32YES %T32+3"     },
    {.exec = op88_9, .next = NULL,      .dissasm = "?P= %I2, %G32YES %T32+3"     },
    {.exec = op8A,   .next = NULL,      .dissasm = "?%u2%#2%z2 A, %G32YES %T32+3"},
    {.exec = op8B,   .next = NULL,      .dissasm = "?%u2%>2%v2 A, %G32YES %T32+3"},
    {.exec = op8C,   .next = NULL,      .dissasm = "GOLONG %R24+2"               },
    {.exec = op8D,   .next = NULL,      .dissasm = "GOVLNG %X25"                 },
    {.exec = op8E,   .next = NULL,      .dissasm = "GOSUBL %R24+6"               },
    {.exec = op8F,   .next = NULL,      .dissasm = "GOSBVL %X25"                 },
};

static Opcode opcodes9[ 16 ] = {
    {.exec = op9a, .next = NULL, .dissasm = "?%u2%#2%z2 %F1, %G32YES %T32+3"},
    {.exec = op9a, .next = NULL, .dissasm = "?%u2%#2%z2 %F1, %G32YES %T32+3"},
    {.exec = op9a, .next = NULL, .dissasm = "?%u2%#2%z2 %F1, %G32YES %T32+3"},
    {.exec = op9a, .next = NULL, .dissasm = "?%u2%#2%z2 %F1, %G32YES %T32+3"},
    {.exec = op9a, .next = NULL, .dissasm = "?%u2%#2%z2 %F1, %G32YES %T32+3"},
    {.exec = op9a, .next = NULL, .dissasm = "?%u2%#2%z2 %F1, %G32YES %T32+3"},
    {.exec = op9a, .next = NULL, .dissasm = "?%u2%#2%z2 %F1, %G32YES %T32+3"},
    {.exec = op9a, .next = NULL, .dissasm = "?%u2%#2%z2 %F1, %G32YES %T32+3"},
    {.exec = op9b, .next = NULL, .dissasm = "?%u2%>2%v2 %F1, %G32YES %T32+3"},
    {.exec = op9b, .next = NULL, .dissasm = "?%u2%>2%v2 %F1, %G32YES %T32+3"},
    {.exec = op9b, .next = NULL, .dissasm = "?%u2%>2%v2 %F1, %G32YES %T32+3"},
    {.exec = op9b, .next = NULL, .dissasm = "?%u2%>2%v2 %F1, %G32YES %T32+3"},
    {.exec = op9b, .next = NULL, .dissasm = "?%u2%>2%v2 %F1, %G32YES %T32+3"},
    {.exec = op9b, .next = NULL, .dissasm = "?%u2%>2%v2 %F1, %G32YES %T32+3"},
    {.exec = op9b, .next = NULL, .dissasm = "?%u2%>2%v2 %F1, %G32YES %T32+3"},
    {.exec = op9b, .next = NULL, .dissasm = "?%u2%>2%v2 %F1, %G32YES %T32+3"}
};

static Opcode opcodesA[ 16 ] = {
    {.exec = opAa, .next = NULL, .dissasm = "%t2=%t2%+2%w2 %F1"},
    {.exec = opAa, .next = NULL, .dissasm = "%t2=%t2%+2%w2 %F1"},
    {.exec = opAa, .next = NULL, .dissasm = "%t2=%t2%+2%w2 %F1"},
    {.exec = opAa, .next = NULL, .dissasm = "%t2=%t2%+2%w2 %F1"},
    {.exec = opAa, .next = NULL, .dissasm = "%t2=%t2%+2%w2 %F1"},
    {.exec = opAa, .next = NULL, .dissasm = "%t2=%t2%+2%w2 %F1"},
    {.exec = opAa, .next = NULL, .dissasm = "%t2=%t2%+2%w2 %F1"},
    {.exec = opAa, .next = NULL, .dissasm = "%t2=%t2%+2%w2 %F1"},
    {.exec = opAb, .next = NULL, .dissasm = "%x2%=2%y2%E2 %F1" },
    {.exec = opAb, .next = NULL, .dissasm = "%x2%=2%y2%E2 %F1" },
    {.exec = opAb, .next = NULL, .dissasm = "%x2%=2%y2%E2 %F1" },
    {.exec = opAb, .next = NULL, .dissasm = "%x2%=2%y2%E2 %F1" },
    {.exec = opAb, .next = NULL, .dissasm = "%x2%=2%y2%E2 %F1" },
    {.exec = opAb, .next = NULL, .dissasm = "%x2%=2%y2%E2 %F1" },
    {.exec = opAb, .next = NULL, .dissasm = "%x2%=2%y2%E2 %F1" },
    {.exec = opAb, .next = NULL, .dissasm = "%x2%=2%y2%E2 %F1" }
};

static Opcode opcodesBb[ 16 ] = {
    {.exec = opBb0_3, .next = NULL, .dissasm = "ASL %F1"   },
    {.exec = opBb0_3, .next = NULL, .dissasm = "BSL %F1"   },
    {.exec = opBb0_3, .next = NULL, .dissasm = "CSL %F1"   },
    {.exec = opBb0_3, .next = NULL, .dissasm = "DSL %F1"   },
    {.exec = opBb4_7, .next = NULL, .dissasm = "ASR %F1"   },
    {.exec = opBb4_7, .next = NULL, .dissasm = "BSR %F1"   },
    {.exec = opBb4_7, .next = NULL, .dissasm = "CSR %F1"   },
    {.exec = opBb4_7, .next = NULL, .dissasm = "DSR %F1"   },
    {.exec = opBb8_B, .next = NULL, .dissasm = "A=-A %F1"  },
    {.exec = opBb8_B, .next = NULL, .dissasm = "B=-B %F1"  },
    {.exec = opBb8_B, .next = NULL, .dissasm = "C=-C %F1"  },
    {.exec = opBb8_B, .next = NULL, .dissasm = "D=-D %F1"  },
    {.exec = opBbC_F, .next = NULL, .dissasm = "A=-A-1 %F1"},
    {.exec = opBbC_F, .next = NULL, .dissasm = "B=-B-1 %F1"},
    {.exec = opBbC_F, .next = NULL, .dissasm = "C=-C-1 %F1"},
    {.exec = opBbC_F, .next = NULL, .dissasm = "D=-D-1 %F1"}
};

static Opcode opcodesB[ 16 ] = {
    {.exec = opBa, .next = NULL,      .dissasm = "%t2=%p2%-2%q2 %F1"},
    {.exec = opBa, .next = NULL,      .dissasm = "%t2=%p2%-2%q2 %F1"},
    {.exec = opBa, .next = NULL,      .dissasm = "%t2=%p2%-2%q2 %F1"},
    {.exec = opBa, .next = NULL,      .dissasm = "%t2=%p2%-2%q2 %F1"},
    {.exec = opBa, .next = NULL,      .dissasm = "%t2=%p2%-2%q2 %F1"},
    {.exec = opBa, .next = NULL,      .dissasm = "%t2=%p2%-2%q2 %F1"},
    {.exec = opBa, .next = NULL,      .dissasm = "%t2=%p2%-2%q2 %F1"},
    {.exec = opBa, .next = NULL,      .dissasm = "%t2=%p2%-2%q2 %F1"},
    {.exec = NULL, .next = opcodesBb, .dissasm = NULL               },
    {.exec = NULL, .next = opcodesBb, .dissasm = NULL               },
    {.exec = NULL, .next = opcodesBb, .dissasm = NULL               },
    {.exec = NULL, .next = opcodesBb, .dissasm = NULL               },
    {.exec = NULL, .next = opcodesBb, .dissasm = NULL               },
    {.exec = NULL, .next = opcodesBb, .dissasm = NULL               },
    {.exec = NULL, .next = opcodesBb, .dissasm = NULL               },
    {.exec = NULL, .next = opcodesBb, .dissasm = NULL               }
};

static Opcode opcodesF[ 16 ] = {
    {.exec = opF0_3, .next = NULL, .dissasm = "ASL A"   },
    {.exec = opF0_3, .next = NULL, .dissasm = "BSL A"   },
    {.exec = opF0_3, .next = NULL, .dissasm = "CSL A"   },
    {.exec = opF0_3, .next = NULL, .dissasm = "DSL A"   },
    {.exec = opF4_7, .next = NULL, .dissasm = "ASR A"   },
    {.exec = opF4_7, .next = NULL, .dissasm = "BSR A"   },
    {.exec = opF4_7, .next = NULL, .dissasm = "CSR A"   },
    {.exec = opF4_7, .next = NULL, .dissasm = "DSR A"   },
    {.exec = opF8_B, .next = NULL, .dissasm = "A=-A A"  },
    {.exec = opF8_B, .next = NULL, .dissasm = "B=-B A"  },
    {.exec = opF8_B, .next = NULL, .dissasm = "C=-C A"  },
    {.exec = opF8_B, .next = NULL, .dissasm = "D=-D A"  },
    {.exec = opFC_F, .next = NULL, .dissasm = "A=-A-1 A"},
    {.exec = opFC_F, .next = NULL, .dissasm = "B=-B-1 A"},
    {.exec = opFC_F, .next = NULL, .dissasm = "C=-C-1 A"},
    {.exec = opFC_F, .next = NULL, .dissasm = "D=-D-1 A"},
};

Opcode opcodes[ 16 ] = {
    {.exec = NULL, .next = opcodes0, .dissasm = NULL             },
    {.exec = NULL, .next = opcodes1, .dissasm = NULL             },
    {.exec = op2,  .next = NULL,     .dissasm = "P= %I1"         },
    {.exec = op3,  .next = NULL,     .dissasm = "LC #%N1"        },
    {.exec = op4,  .next = NULL,     .dissasm = "%G12C %T12+1"   },
    {.exec = op5,  .next = NULL,     .dissasm = "%G12NC %T12+1"  },
    {.exec = op6,  .next = NULL,     .dissasm = "GOTO %R13+1"    },
    {.exec = op7,  .next = NULL,     .dissasm = "GOSUB %R13+4"   },
    {.exec = NULL, .next = opcodes8, .dissasm = NULL             },
    {.exec = NULL, .next = opcodes9, .dissasm = NULL             },
    {.exec = NULL, .next = opcodesA, .dissasm = NULL             },
    {.exec = NULL, .next = opcodesB, .dissasm = NULL             },
    {.exec = opC,  .next = NULL,     .dissasm = "%t1=%t1%+1%w1 A"},
    {.exec = opD,  .next = NULL,     .dissasm = "%x1%=1%y1%E1 A" },
    {.exec = opE,  .next = NULL,     .dissasm = "%t1=%p1%-1%q1 A"},
    {.exec = NULL, .next = opcodesF, .dissasm = NULL             }
};
