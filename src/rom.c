#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>       // dirname
#include <unistd.h>       // readlink
#include <linux/limits.h> // PATH_MAX

#include "types.h"
#include "bus.h"
#include "rom.h"
#include "pfiles.h"

void rom_init( void )
{
    int size;
    char* name = "hpemu.rom";
    byte *buf, *ptr1, *ptr2;
    FILE* f;

    size = file_size( name );
    if ( !size ) {
        printf( "rom_init failed\n" );
        exit( 0x10 );
    }
    if ( size != 256 * 1024 && size != 512 * 1024 && size != 1024 * 1024 ) {
        printf( "rom_init failed2..\n" );
        exit( 0x11 );
    }
    buf = malloc( size );
    if ( !buf ) {
        printf( "rom_init failed3..\n" );
        exit( 0x12 );
    }

    char fullpath[ 1024 ];
    sprintf( fullpath, "%s/%s", WorkingPath, name );

    // f = pack_fopen(name, "r");
    f = fopen( fullpath, "r" );
    if ( !f ) {
        printf( "rom_init failed4..\n" );
        exit( 0x13 );
    }
    int r = ( int )fread( buf, sizeof( char ), size, f );
    if ( r != size ) { // pack_fread
        printf( "rom_init failed5..\n" );
        exit( 0x14 );
    }
    // pack_fclose(f);
    fclose( f );
    if ( buf[ 0 ] & 0xF0 || buf[ 1 ] & 0xF0 ) {
        if ( size == 1024 * 1024 ) {
            printf( "rom_init failed6..\n" );
            exit( 0x15 );
        }
        buf = realloc( buf, size * 2 );
        if ( !buf ) {
            printf( "rom_init failed7..\n" );
            exit( 0x16 );
        }
        ptr1 = buf + size - 1;
        size *= 2;
        ptr2 = buf + size - 1;
        do {
            *( ptr2-- ) = ( *ptr1 >> 4 ) & 0x0F;
            *( ptr2-- ) = *( ptr1-- ) & 0x0F;
        } while ( ptr1 != ptr2 );
    }
    bus_info.rom_data = buf;
    bus_info.rom_mask = size - 1;
    printf( "rom_init succeed!\n" );
}

void rom_exit( void )
{
    free( bus_info.rom_data );
    bus_info.rom_data = NULL;
    bus_info.rom_mask = 0x00000;
}
