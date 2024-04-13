#ifndef __PFILES_H
#define __PFILES_H

#include "types.h"

extern char WorkingPath[ 512 ];

extern int file_size( char* name );
extern void load_file_on_stack( char* name );

#endif
