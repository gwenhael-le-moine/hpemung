#ifndef __BUS_H
#define __BUS_H

#include "types.h"

#define SEG_OF( adr ) ( ( adr ) >> 12 )
#define OFFSET_OF( adr ) ( ( adr ) & 0xFFF )
#define CAN_READ( adr ) ( read_map[ SEG_OF( adr ) ] != NULL )
#define CAN_WRITE( adr ) ( write_map[ SEG_OF( adr ) ] != NULL )
#define MAP_READ( adr ) ( read_map[ SEG_OF( adr ) ] + OFFSET_OF( adr ) )
#define MAP_WRITE( adr ) ( write_map[ SEG_OF( adr ) ] + OFFSET_OF( adr ) )

#define MIN( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )

typedef struct {
    address hdw_base;
    address ram_size;
    address ram_base;
    address ce1_size;
    address ce1_base;
    address ce2_size;
    address ce2_base;
    address nce3_size;
    address nce3_base;
    bool hdw_cfg : 1;
    bool ram_sz_cfg : 1;
    bool ram_cfg : 1;
    bool ce1_sz_cfg : 1;
    bool ce1_cfg : 1;
    bool ce2_sz_cfg : 1;
    bool ce2_cfg : 1;
    bool nce3_sz_cfg : 1;
    bool nce3_cfg : 1;
    bool ce1_r_o : 1;
    bool ce2_r_o : 1;
    bool nce3_r_o : 1;
    bool ce1_bs : 1;
    bool da19 : 1;
    bool ben : 1;
    byte* rom_data;
    byte* ram_data;
    byte* ce1_data;
    byte* ce2_data;
    byte* nce3_data;
    address rom_mask;
    address ram_mask;
    address ce1_mask;
    address ce2_mask;
    address nce3_mask;
    dword map_cnt;
} BusInfo;

extern BusInfo bus_info;

extern word crc;

// FAST_PEEK_MAX must not be greater than the size of the hdw registers (64)
#define FAST_PEEK_MAX 64

extern void bus_init( void );
extern void bus_exit( void );

extern void bus_read( byte* buf, address adr, address len );
extern void bus_write( byte* buf, address adr, address len );
extern byte* bus_fast_peek( byte* buf, address adr, address* len );

extern void bus_remap( void );
extern void bus_configure( address adr );
extern void bus_unconfigure( address adr );
extern void bus_reset( void );
extern address bus_get_id();

#endif
