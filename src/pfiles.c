#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>       // dirname
#include <unistd.h>       // readlink
#include <linux/limits.h> // PATH_MAX

#include "color.h"
#include "gui.h"
#include "rpl.h"
#include "pfiles.h"

char WorkingPath[ 512 ];

void getExePath()
{
    char programPath[ 1024 ];
    char temp[ 1024 ];
    memset( programPath, 0, sizeof( programPath ) );
    memset( temp, 0, sizeof( temp ) );

    // setWorkingPath(programPath);
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
    if ( !f ) {
        return 0;
    }
    fseek( f, 0, SEEK_END );      // seek to end of file
    int size = ( int )ftell( f ); // get current file pointer
    fseek( f, 0, SEEK_SET );      // seek back to beginning of file
    // proceed with allocating memory and reading the file
    fclose( f );
    return size;
}

void load_file( char* name )
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

    // f = pack_fopen(name, "r");
    f = fopen( name, "r" );
    if ( !f ) {
        free( buf );
        return;
    }
    int res = ( int )fread( buf, sizeof( char ), fsize, f );
    if ( res != fsize ) { // pack_fread
        free( buf );
        // pack_fclose(f);
        fclose( f );
        return;
    }
    // pack_fclose(f);
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
