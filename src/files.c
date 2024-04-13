#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>       // dirname
#include <unistd.h>       // readlink
#include <linux/limits.h> // PATH_MAX

#include "gui.h"
#include "rpl.h"
#include "files.h"
#include "bus.h"
#include "ports.h"

extern byte current_bank;
extern byte* port2;
extern address port2mask;

char WorkingPath[ 512 ];

static address ram_size = 256 * 1024; // in nibbles, not bytes!

void getExePath()
{
    char programPath[ 1024 ];
    char temp[ 1024 ];
    memset( programPath, 0, sizeof( programPath ) );
    memset( temp, 0, sizeof( temp ) );

    char result[ PATH_MAX ];
    ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
    const char* path;
    if ( count != -1 ) {
        path = dirname( result );
        strcpy( programPath, path );
    }

    memset( WorkingPath, 0, sizeof( WorkingPath ) );
    strcpy( WorkingPath, programPath );
}

int file_size( char* name )
{
    memset( WorkingPath, 0, sizeof( WorkingPath ) );
    getExePath();

    FILE* f;
    char fullpath[ 1024 ];
    sprintf( fullpath, "%s/%s", WorkingPath, name );
    printf( "%s\n", fullpath );
    f = fopen( fullpath, "r" );
    if ( !f )
        return 0;

    fseek( f, 0, SEEK_END );      // seek to end of file
    int size = ( int )ftell( f ); // get current file pointer
    fseek( f, 0, SEEK_SET );      // seek back to beginning of file
    // proceed with allocating memory and reading the file
    fclose( f );
    return size;
}

void load_file_on_stack( char* name )
{
    FILE* f;
    byte* buf;
    byte* obj;
    int i, j;
    int fsize;
    address size;

    fsize = file_size( name );
    if ( fsize < 11 ) // "PHPH48-X" + prologue (8 + 2.5)
        return;

    buf = malloc( fsize );
    if ( !buf )
        return;

    f = fopen( name, "r" );
    if ( !f ) {
        free( buf );
        return;
    }
    int res = ( int )fread( buf, sizeof( char ), fsize, f );
    if ( res != fsize ) {
        free( buf );
        fclose( f );
        return;
    }
    fclose( f );

    if ( memcmp( buf, "HPHP48-", 7 ) ) {
        free( buf );
        return;
    }

    obj = malloc( ( fsize - 8 ) * 2 );
    if ( !obj ) {
        free( buf );
        return;
    }

    for ( i = 8, j = 0; i < fsize; i++ ) {
        obj[ j++ ] = buf[ i ] & 0x0F;
        obj[ j++ ] = ( buf[ i ] >> 4 ) & 0x0F;
    }
    free( buf );

    size = rpl_object_size( obj );
    if ( size > ( fsize - 8 ) * 2 ) {
        free( obj );
        return;
    }
    rpl_push_object( obj, size );
    free( obj );
}

void rom_init( void )
{
    int size;
    char* name = "rom";
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

void ram_init( void )
{
    byte* buf = malloc( ram_size );
    if ( !buf )
        exit( 0x20 );

    memset( buf, 0, ram_size );
    bus_info.ram_data = buf;
    bus_info.ram_mask = ram_size - 1;
}

void ram_exit( void )
{
    free( bus_info.ram_data );
    bus_info.ram_data = NULL;
    bus_info.ram_mask = 0x00000;
}

void ports_init( void )
{
    // ce1 = bank switcher
    bus_info.ce1_data = NULL;
    bus_info.ce1_mask = 0x0007F;
    bus_info.ce1_r_o = true;
    bus_info.ce1_bs = true;

    // ce2 = port1 (plugged)
    bus_info.ce2_data = malloc( PORT1_SIZE );
    bus_info.ce2_mask = PORT1_SIZE - 1;
    bus_info.ce2_r_o = false;

    // nce3 = port2 (plugged)
    port2 = malloc( PORT2_SIZE );
    port2mask = PORT2_SIZE - 1;
    bus_info.nce3_data = port2;
    bus_info.nce3_mask = port2mask & 0x3FFFF;
    bus_info.nce3_r_o = false;

    bus_info.ben = false;
    current_bank = 0;
}

void ports_exit( void ) {}
