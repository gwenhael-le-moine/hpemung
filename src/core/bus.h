#ifndef __BUS_H
#define __BUS_H

#include <stdbool.h>

#include "types.h"

typedef struct {
    byte* rom_data;
    address rom_mask;

    address hdw_base;
    bool hdw_cfg : true;

    address ram_size;
    address ram_base;
    bool ram_sz_cfg : true;
    bool ram_cfg : true;
    byte* ram_data;
    address ram_mask;

    address ce1_size;
    address ce1_base;
    bool ce1_sz_cfg : true;
    bool ce1_cfg : true;
    bool ce1_r_o : true;
    bool ce1_bs : true;
    byte* ce1_data;
    address ce1_mask;

    address ce2_size;
    address ce2_base;
    bool ce2_sz_cfg : true;
    bool ce2_cfg : true;
    bool ce2_r_o : true;
    byte* ce2_data;
    address ce2_mask;

    address nce3_size;
    address nce3_base;
    bool nce3_sz_cfg : true;
    bool nce3_cfg : true;
    bool nce3_r_o : true;
    byte* nce3_data;
    address nce3_mask;

    bool da19 : true;
    bool ben : true;
    dword map_cnt;
} BusInfo;

extern BusInfo bus_info;

extern word crc;

// FAST_PEEK_MAX must not be greater than the size of the hdw registers (64)
#define FAST_PEEK_MAX 64

extern void bus_read( byte* buf, address adr, address len );
extern void bus_write( byte* buf, address adr, address len );
extern byte* bus_fast_peek( byte* buf, address adr, address* len );

extern void bus_remap( void );
extern void bus_configure( address adr );
extern void bus_unconfigure( address adr );
extern void bus_reset( void );
extern address bus_get_id( void );

#endif
