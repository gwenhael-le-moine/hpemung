#include "types.h"
#include "emulator.h"
#include "bus.h"
#include "opcodes.h"
#include "cpu.h"
#include <stdio.h>

Cpu cpu;

#define MAX_OPC_LEN 21

void cpu_interrupt( void )
{
    printf( "cpu_interrupt\n" );
    if ( cpu.inte ) {
        cpu.inte = false;
        cpu.rstk_ptr = ( cpu.rstk_ptr - 1 ) & 7;
        cpu.rstk[ cpu.rstk_ptr ] = cpu.pc;
        cpu.pc = 0x0000F;
    }
}

static void decode( byte* ptr )
{
    Opcode* op = opcodes;
    int i = 0;

    while ( op[ ptr[ i ] ].next ) {
        op = op[ ptr[ i ] ].next;
        i++;
    }
    if ( op[ ptr[ i ] ].exec ) {
        op[ ptr[ i ] ].exec( ptr );
        cpu.pc &= 0xFFFFF;
        cpu.inst_cnt++;
    } else {
        emulator_set_state( EMULATOR_STOP );
    }
}

void execute_instruction( void )
{
    static byte buffer[ FAST_PEEK_MAX ];
    static byte* ptr;
    static dword old_map_cnt;
    static address len;
    static address adr;

    if ( cpu.pc < adr || adr + len < cpu.pc + MAX_OPC_LEN || bus_info.map_cnt != old_map_cnt || !ptr ) {
        len = MAX_OPC_LEN;
        adr = cpu.pc;
        ptr = bus_fast_peek( buffer, adr, &len );
        old_map_cnt = bus_info.map_cnt;
        if ( ptr == buffer ) { // Not direct memory access
            old_map_cnt--;     // Force new peek next time
        }
    }
    decode( ptr + cpu.pc - adr );
}
