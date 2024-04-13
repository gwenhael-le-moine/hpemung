#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>       // dirname
#include <unistd.h>       // readlink
#include <linux/limits.h> // PATH_MAX
#include <sys/stat.h>

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

int file_size( char* filename )
{
    memset( WorkingPath, 0, sizeof( WorkingPath ) );
    getExePath();

    FILE* f;
    char fullpath[ 1024 ];
    sprintf( fullpath, "%s/%s", WorkingPath, filename );
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

int read_mem_file( char* filename, nibble* mem, int size )
{
    struct stat st;
    FILE* fp;
    byte* tmp_mem;
    byte rbyte;
    int i, j;

    if ( NULL == ( fp = fopen( filename, "r" ) ) ) {
        /* if ( verbose ) */
        /*     fprintf( stderr, "ct open %s\n", filename ); */
        return 0;
    }

    if ( stat( filename, &st ) < 0 ) {
        /* if ( verbose ) */
        /*     fprintf( stderr, "can\'t stat %s\n", filename ); */
        return 0;
    }

    if ( st.st_size == size ) {
        /*
         * size is same as memory size, old version file
         */
        if ( fread( mem, 1, ( size_t )size, fp ) != ( unsigned long )size ) {
            /* if ( verbose ) */
            /*     fprintf( stderr, "can\'t read %s\n", filename ); */
            fclose( fp );
            return 0;
        }
    } else {
        /*
         * size is different, check size and decompress memory
         */

        if ( st.st_size != size / 2 ) {
            /* if ( verbose ) */
            /*     fprintf( stderr, "strange size %s, expected %d, found %ld\n", filename, size / 2, st.st_size ); */
            fclose( fp );
            return 0;
        }

        if ( NULL == ( tmp_mem = ( byte* )malloc( ( size_t )st.st_size ) ) ) {
            for ( i = 0, j = 0; i < size / 2; i++ ) {
                if ( 1 != fread( &rbyte, 1, 1, fp ) ) {
                    /* if ( verbose ) */
                    /*     fprintf( stderr, "can\'t read %s\n", filename ); */
                    fclose( fp );
                    return 0;
                }
                mem[ j++ ] = ( nibble )( ( int )rbyte & 0xf );
                mem[ j++ ] = ( nibble )( ( ( int )rbyte >> 4 ) & 0xf );
            }
        } else {
            if ( fread( tmp_mem, 1, ( size_t )size / 2, fp ) != ( unsigned long )( size / 2 ) ) {
                /* if ( verbose ) */
                /*     fprintf( stderr, "can\'t read %s\n", filename ); */
                fclose( fp );
                free( tmp_mem );
                return 0;
            }

            for ( i = 0, j = 0; i < size / 2; i++ ) {
                mem[ j++ ] = ( nibble )( ( int )tmp_mem[ i ] & 0xf );
                mem[ j++ ] = ( nibble )( ( ( int )tmp_mem[ i ] >> 4 ) & 0xf );
            }

            free( tmp_mem );
        }
    }

    fclose( fp );

    /* if ( verbose ) */
    /*     printf( "read %s\n", filename ); */

    return 1;
}

int write_mem_file( char* filename, nibble* mem, int size )
{
    FILE* fp;
    byte* tmp_mem;
    byte rbyte;
    int i, j;

    if ( NULL == ( fp = fopen( filename, "w" ) ) ) {
        /* if ( verbose ) */
        /*     fprintf( stderr, "can\'t open %s\n", filename ); */
        return 0;
    }

    if ( NULL == ( tmp_mem = ( byte* )malloc( ( size_t )size / 2 ) ) ) {
        for ( i = 0, j = 0; i < size / 2; i++ ) {
            rbyte = ( mem[ j++ ] & 0x0f );
            rbyte |= ( mem[ j++ ] << 4 ) & 0xf0;
            if ( 1 != fwrite( &rbyte, 1, 1, fp ) ) {
                /* if ( verbose ) */
                /*     fprintf( stderr, "can\'t write %s\n", filename ); */
                fclose( fp );
                return 0;
            }
        }
    } else {
        for ( i = 0, j = 0; i < size / 2; i++ ) {
            tmp_mem[ i ] = ( mem[ j++ ] & 0x0f );
            tmp_mem[ i ] |= ( mem[ j++ ] << 4 ) & 0xf0;
        }

        if ( fwrite( tmp_mem, 1, ( size_t )size / 2, fp ) != ( unsigned long )size / 2 ) {
            /* if ( verbose ) */
            /*     fprintf( stderr, "can\'t write %s\n", filename ); */
            fclose( fp );
            free( tmp_mem );
            return 0;
        }

        free( tmp_mem );
    }

    fclose( fp );

    /* if ( verbose ) */
    /*     printf( "wrote %s\n", filename ); */

    return 1;
}

void load_file_on_stack( char* filename )
{
    FILE* f;
    byte* buf;
    byte* obj;
    int i, j;
    int fsize;
    address size;

    fsize = file_size( filename );
    if ( fsize < 11 ) // "PHPH48-X" + prologue (8 + 2.5)
        return;

    buf = malloc( fsize );
    if ( !buf )
        return;

    f = fopen( filename, "r" );
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

void rom_init( char* filename )
{
    int size;
    byte *buf, *ptr1, *ptr2;
    FILE* f;

    size = file_size( filename );
    if ( !size ) {
        printf( "ERROR: Can't read ROM file size\n" );
        exit( 0x10 );
    }
    if ( size != 256 * 1024 && size != 512 * 1024 && size != 1024 * 1024 ) {
        printf( "ERROR: ROM file size invalid\n" );
        exit( 0x11 );
    }
    buf = malloc( size );
    if ( !buf ) {
        printf( "ERROR: can't malloc ROM\n" );
        exit( 0x12 );
    }

    char fullpath[ 1024 ];
    sprintf( fullpath, "%s/%s", WorkingPath, filename );

    f = fopen( fullpath, "r" );
    if ( !f ) {
        printf( "ERROR: can't open ROM file\n" );
        exit( 0x13 );
    }
    if ( ( int )fread( buf, sizeof( char ), size, f ) != size ) { // pack_fread
        printf( "ERROR: can't read ROM file\n" );
        exit( 0x14 );
    }
    // pack_fclose(f);
    fclose( f );
    if ( buf[ 0 ] & 0xF0 || buf[ 1 ] & 0xF0 ) {
        if ( size == 1024 * 1024 ) {
            printf( "ERROR: wrong ROM\n" );
            exit( 0x15 );
        }
        buf = realloc( buf, size * 2 );
        if ( !buf ) {
            printf( "ERROR: can't realloc ROM\n" );
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

void ram_init( char* filename )
{
    bus_info.ram_data = malloc( ram_size );

    int filesize = file_size( filename );
    fprintf( stderr, "filesize(%i) == ram_size(%i)\n", filesize, ram_size );
    if ( filesize * 2 == ram_size )
        read_mem_file( filename, bus_info.ram_data, ram_size );
    else {
        byte* buf = malloc( ram_size );
        if ( !buf )
            exit( 0x20 );

        memset( buf, 0, ram_size );
        bus_info.ram_data = buf;
    }

    bus_info.ram_mask = ram_size - 1;
}

void ram_exit( char* filename )
{
    write_mem_file( filename, bus_info.ram_data, ram_size );

    free( bus_info.ram_data );
    bus_info.ram_data = NULL;
    bus_info.ram_mask = 0x00000;
}

void ports_init( char* filename1, char* filename2 )
{
    // ce1 = bank switcher
    bus_info.ce1_data = NULL;
    bus_info.ce1_mask = 0x0007F;
    bus_info.ce1_r_o = true;
    bus_info.ce1_bs = true;

    // ce2 = port1 (plugged)
    bus_info.ce2_data = malloc( PORT1_SIZE );

    int filesize = file_size( filename1 );
    if ( filesize * 2 == PORT1_SIZE )
        read_mem_file( filename1, bus_info.ce2_data, ram_size );

    bus_info.ce2_mask = PORT1_SIZE - 1;
    bus_info.ce2_r_o = false;

    // nce3 = port2 (plugged)
    port2 = malloc( PORT2_SIZE );

    filesize = file_size( filename2 );
    if ( filesize * 2 == PORT2_SIZE )
        read_mem_file( filename2, port2, PORT2_SIZE );
    port2mask = PORT2_SIZE - 1;
    bus_info.nce3_data = port2;
    bus_info.nce3_mask = port2mask & 0x3FFFF;
    bus_info.nce3_r_o = false;

    bus_info.ben = false;
    current_bank = 0;
}

void ports_exit( char* filename1, char* filename2 )
{
    write_mem_file( filename1, bus_info.ce2_data, PORT1_SIZE );
    write_mem_file( filename2, port2, PORT2_SIZE );
}
