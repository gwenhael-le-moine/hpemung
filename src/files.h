#ifndef __PFILES_H
#define __PFILES_H

#include "types.h"

extern char WorkingPath[ 512 ];

extern int file_size( char* filename );
extern void load_file_on_stack( char* filename );

extern void rom_init( char* filename );
extern void rom_exit( void );

extern void ram_init( char* filename );
extern void ram_exit( char* filename );

extern void ports_init( char* filename1, char* filename2 );
extern void ports_exit( char* filename1, char* filename2 );

#endif
