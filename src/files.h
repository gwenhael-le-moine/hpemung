#ifndef __PFILES_H
#define __PFILES_H

#include "types.h"

extern char WorkingPath[ 512 ];

extern int file_size( char* name );
extern void load_file_on_stack( char* name );

extern void rom_init( void );
extern void rom_exit( void );

extern void ram_init( void );
extern void ram_exit( void );

extern void ports_init( void );
extern void ports_exit( void );

#endif
