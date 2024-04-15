#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>       // dirname
#include <unistd.h>       // readlink
#include <linux/limits.h> // PATH_MAX
#include <sys/stat.h>
#include <pwd.h>

#include "rpl.h"
#include "bus.h"
#include "cpu.h"
#include "config.h"
#include "types.h"

#define MAX_LENGTH_FILENAME 2048

extern byte current_bank;
extern byte* port2;
extern address port2mask;

char absolute_working_dir_path[ MAX_LENGTH_FILENAME ];

static address ram_size = 256 * 1024; // in nibbles, not bytes!

static address port1_size = ( 256 * 1024 );  /* 128Kio in nibbles */
static address port2_size = ( 8192 * 1024 ); /* 4Mio in nibbles */

void get_absolute_working_dir_path()
{
    if ( absolute_working_dir_path[ 0 ] == '/' )
        return;
    if ( config.verbose )
        fprintf( stderr, "Finding and creating absolute_working_dir_path\n" );

    memset( absolute_working_dir_path, 0, sizeof( absolute_working_dir_path ) );
    char* xdg_config_home = getenv( "XDG_CONFIG_HOME" );

    if ( xdg_config_home ) {
        if ( config.verbose )
            fprintf( stderr, "XDG_CONFIG_HOME is %s\n", xdg_config_home );

        strcpy( absolute_working_dir_path, xdg_config_home );
        strcat( absolute_working_dir_path, "/" );
    } else {
        char* home = getenv( "HOME" );

        if ( home ) {
            if ( config.verbose )
                fprintf( stderr, "HOME is %s\n", home );

            strcpy( absolute_working_dir_path, home );
            strcat( absolute_working_dir_path, "/.config/" );
        } else {
            struct passwd* pwd = getpwuid( getuid() );

            if ( pwd ) {
                if ( config.verbose )
                    fprintf( stderr, "pwd->pw_dir is %s\n", pwd->pw_dir );

                strcpy( absolute_working_dir_path, pwd->pw_dir );
                strcat( absolute_working_dir_path, "/" );
            } else {
                if ( config.verbose )
                    fprintf( stderr, "can\'t figure out your home directory, "
                                     "trying /tmp\n" );

                strcpy( absolute_working_dir_path, "/tmp" );
            }
        }
    }

    strcat( absolute_working_dir_path, config.progname );

    if ( absolute_working_dir_path[ strlen( absolute_working_dir_path ) ] != '/' )
        strcat( absolute_working_dir_path, "/" );

    if ( config.verbose )
        fprintf( stderr, "SET absolute_working_dir_path = %s\n", absolute_working_dir_path );

    struct stat st;
    bool make_dir = false;
    if ( stat( absolute_working_dir_path, &st ) == -1 ) {
        if ( errno == ENOENT ) {
            make_dir = true;
        } else {
            if ( config.verbose )
                fprintf( stderr, "can\'t stat %s, saving to /tmp\n", absolute_working_dir_path );
            strcpy( absolute_working_dir_path, "/tmp" );
        }
    } else {
        if ( !S_ISDIR( st.st_mode ) ) {
            if ( config.verbose )
                fprintf( stderr, "%s is no directory, saving to /tmp\n", absolute_working_dir_path );
            strcpy( absolute_working_dir_path, "/tmp" );
        }
    }

    if ( make_dir ) {
        if ( mkdir( absolute_working_dir_path, 0777 ) == -1 ) {
            if ( config.verbose )
                fprintf( stderr, "can\'t mkdir %s, saving to /tmp\n", absolute_working_dir_path );
            strcpy( absolute_working_dir_path, "/tmp" );
        }
    }
}

/* void getExePath() */
/* { */
/*     char programPath[ MAX_LENGTH_FILENAME ]; */
/*     char temp[ MAX_LENGTH_FILENAME ]; */
/*     memset( programPath, 0, sizeof( programPath ) ); */
/*     memset( temp, 0, sizeof( temp ) ); */

/*     char result[ PATH_MAX ]; */
/*     ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX ); */
/*     const char* path; */
/*     if ( count != -1 ) { */
/*         path = dirname( result ); */
/*         strcpy( programPath, path ); */
/*     } */

/*     memset( WorkingPath, 0, sizeof( WorkingPath ) ); */
/*     strcpy( WorkingPath, programPath ); */
/* } */

int file_size( char* filename )
{
    /* memset( WorkingPath, 0, sizeof( WorkingPath ) ); */
    /* getExePath(); */
    get_absolute_working_dir_path();

    FILE* f;
    f = fopen( filename, "r" );
    if ( !f )
        return 0;

    fseek( f, 0, SEEK_END );      // seek to end of file
    int size = ( int )ftell( f ); // get current file pointer
    fseek( f, 0, SEEK_SET );      // seek back to beginning of file
    // proceed with allocating memory and reading the file
    fclose( f );
    return size;
}

int read_mem_file( char* absolute_filename, nibble* mem, int size )
{
    struct stat st;
    FILE* fp;
    byte* tmp_mem;
    byte rbyte;
    int i, j;

    if ( NULL == ( fp = fopen( absolute_filename, "r" ) ) ) {
        if ( config.verbose )
            fprintf( stderr, "ct open %s\n", absolute_filename );
        return 0;
    }

    if ( stat( absolute_filename, &st ) < 0 ) {
        if ( config.verbose )
            fprintf( stderr, "can\'t stat %s\n", absolute_filename );
        return 0;
    }

    if ( st.st_size == size ) {
        /*
         * size is same as memory size, old version file
         */
        if ( fread( mem, 1, ( size_t )size, fp ) != ( unsigned long )size ) {
            if ( config.verbose )
                fprintf( stderr, "can\'t read %s\n", absolute_filename );
            fclose( fp );
            return 0;
        }
    } else {
        /*
         * size is different, check size and decompress memory
         */

        if ( st.st_size != size / 2 ) {
            if ( config.verbose )
                fprintf( stderr, "strange size %s, expected %d, found %ld\n", absolute_filename, size / 2, st.st_size );
            fclose( fp );
            return 0;
        }

        if ( NULL == ( tmp_mem = ( byte* )malloc( ( size_t )st.st_size ) ) ) {
            for ( i = 0, j = 0; i < size / 2; i++ ) {
                if ( 1 != fread( &rbyte, 1, 1, fp ) ) {
                    if ( config.verbose )
                        fprintf( stderr, "can\'t read %s\n", absolute_filename );
                    fclose( fp );
                    return 0;
                }
                mem[ j++ ] = ( nibble )( ( int )rbyte & 0xf );
                mem[ j++ ] = ( nibble )( ( ( int )rbyte >> 4 ) & 0xf );
            }
        } else {
            if ( fread( tmp_mem, 1, ( size_t )size / 2, fp ) != ( unsigned long )( size / 2 ) ) {
                if ( config.verbose )
                    fprintf( stderr, "can\'t read %s\n", absolute_filename );
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

    if ( config.verbose )
        printf( "read %s\n", absolute_filename );

    return 1;
}

int write_mem_file( char* absolute_filename, nibble* mem, int size )
{
    FILE* fp;
    byte* tmp_mem;
    byte rbyte;
    int i, j;

    if ( NULL == ( fp = fopen( absolute_filename, "w" ) ) ) {
        if ( config.verbose )
            fprintf( stderr, "can\'t open %s\n", absolute_filename );
        return 0;
    }

    if ( NULL == ( tmp_mem = ( byte* )malloc( ( size_t )size / 2 ) ) ) {
        for ( i = 0, j = 0; i < size / 2; i++ ) {
            rbyte = ( mem[ j++ ] & 0x0f );
            rbyte |= ( mem[ j++ ] << 4 ) & 0xf0;
            if ( 1 != fwrite( &rbyte, 1, 1, fp ) ) {
                if ( config.verbose )
                    fprintf( stderr, "can\'t write %s\n", absolute_filename );
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
            if ( config.verbose )
                fprintf( stderr, "can\'t write %s\n", absolute_filename );
            fclose( fp );
            free( tmp_mem );
            return 0;
        }

        free( tmp_mem );
    }

    fclose( fp );

    if ( config.verbose )
        printf( "wrote %s\n", absolute_filename );

    return 1;
}

/********************/
/* PUBLIC FUNCTIONS */
/********************/
void load_file_on_stack( char* filename )
{
    FILE* f;
    byte* buf;
    byte* obj;
    int i, j;
    int fsize;
    address size;

    char fullpath[ MAX_LENGTH_FILENAME ];
    get_absolute_working_dir_path();
    sprintf( fullpath, "%s%s", absolute_working_dir_path, filename );

    fsize = file_size( fullpath );
    if ( fsize < 11 ) // "PHPH48-X" + prologue (8 + 2.5)
        return;

    buf = malloc( fsize );
    if ( !buf )
        return;

    f = fopen( fullpath, "r" );
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

void bus_init( char* filename )
{
    char fullpath[ MAX_LENGTH_FILENAME ];
    get_absolute_working_dir_path();
    sprintf( fullpath, "%s%s", absolute_working_dir_path, filename );
    if ( config.verbose )
        fprintf( stderr, "fullpath = %s\n", fullpath );

    int filesize = file_size( fullpath );

    /* if ( filesize ) { */
    /*     FILE* fp; */

    /*     if ( NULL == ( fp = fopen( fullpath, "w" ) ) ) { */
    /*         if ( config.verbose ) */
    /*             fprintf( stderr, "can\'t open %s\n", fullpath ); */
    /*         return; */
    /*     } */

    /*     fwrite( &bus_info, sizeof( BusInfo ), 1, fp ); */

    /*     fclose( fp ); */
    /* } else */
    bus_reset();
}
void bus_exit( char* filename )
{
    char fullpath[ MAX_LENGTH_FILENAME ];
    get_absolute_working_dir_path();
    sprintf( fullpath, "%s%s", absolute_working_dir_path, filename );
    if ( config.verbose )
        fprintf( stderr, "fullpath = %s\n", fullpath );

    FILE* fp;

    if ( NULL == ( fp = fopen( fullpath, "w" ) ) ) {
        if ( config.verbose )
            fprintf( stderr, "can\'t open %s\n", fullpath );
        return;
    }

    fwrite( &bus_info, sizeof( BusInfo ), 1, fp );

    fclose( fp );
}

void cpu_init( char* filename )
{
    char fullpath[ MAX_LENGTH_FILENAME ];
    get_absolute_working_dir_path();
    sprintf( fullpath, "%s%s", absolute_working_dir_path, filename );
    if ( config.verbose )
        fprintf( stderr, "fullpath = %s\n", fullpath );

    int filesize = file_size( fullpath );

    if ( filesize ) {
        /* FILE* fp; */

        /* if ( NULL == ( fp = fopen( fullpath, "w" ) ) ) { */
        /*     if ( config.verbose ) */
        /*         fprintf( stderr, "can\'t open %s\n", fullpath ); */
        /*     return; */
        /* } */

        /* fwrite( &cpu, sizeof( Cpu ), 1, fp ); */

        /* fclose( fp ); */
    }
}
void cpu_exit( char* filename )
{
    char fullpath[ MAX_LENGTH_FILENAME ];
    get_absolute_working_dir_path();
    sprintf( fullpath, "%s%s", absolute_working_dir_path, filename );
    if ( config.verbose )
        fprintf( stderr, "fullpath = %s\n", fullpath );

    FILE* fp;

    if ( NULL == ( fp = fopen( fullpath, "w" ) ) ) {
        if ( config.verbose )
            fprintf( stderr, "can\'t open %s\n", fullpath );
        return;
    }

    fwrite( &cpu, sizeof( Cpu ), 1, fp );

    fclose( fp );
}

void rom_init( char* filename )
{
    int size;
    byte *buf, *ptr1, *ptr2;
    FILE* f;

    char fullpath[ MAX_LENGTH_FILENAME ];
    get_absolute_working_dir_path();
    sprintf( fullpath, "%s%s", absolute_working_dir_path, filename );
    if ( config.verbose )
        fprintf( stderr, "fullpath = %s\n", fullpath );

    size = file_size( fullpath );
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

    f = fopen( fullpath, "r" );
    if ( !f ) {
        printf( "ERROR: can't open ROM file\n" );
        exit( 0x13 );
    }
    if ( ( int )fread( buf, sizeof( char ), size, f ) != size ) { // pack_fread
        printf( "ERROR: can't read ROM file\n" );
        exit( 0x14 );
    }
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

    char fullpath[ MAX_LENGTH_FILENAME ];
    get_absolute_working_dir_path();
    sprintf( fullpath, "%s%s", absolute_working_dir_path, filename );
    if ( config.verbose )
        fprintf( stderr, "fullpath = %s\n", fullpath );

    int filesize = file_size( fullpath ) * 2;
    if ( config.verbose )
        fprintf( stderr, "filesize(%i) == ram_size(%i)\n", filesize, ram_size );
    if ( filesize == ram_size ) {
        if ( config.verbose )
            fprintf( stderr, "loading %s as RAM\n", fullpath );
        read_mem_file( fullpath, bus_info.ram_data, ram_size );
    } else {
        if ( config.verbose )
            fprintf( stderr, "Creating RAM\n" );

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
    fprintf( stderr, "\n\nfilename = %s\n", filename );
    char fullpath[ MAX_LENGTH_FILENAME ];
    get_absolute_working_dir_path();
    sprintf( fullpath, "%s%s", absolute_working_dir_path, filename );
    if ( config.verbose ) {
        fprintf( stderr, "fullpath = %s\n", fullpath );
        fprintf( stderr, "Saving RAM to %s\n", fullpath );
    }
    write_mem_file( fullpath, bus_info.ram_data, ram_size );

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
    bus_info.ce2_data = malloc( port1_size );

    char fullpath1[ MAX_LENGTH_FILENAME ];
    get_absolute_working_dir_path();
    sprintf( fullpath1, "%s%s", absolute_working_dir_path, filename1 );
    if ( config.verbose )
        fprintf( stderr, "fullpath = %s\n", fullpath1 );

    int filesize = file_size( fullpath1 ) * 2;
    if ( filesize == port1_size )
        read_mem_file( fullpath1, bus_info.ce2_data, ram_size );

    bus_info.ce2_mask = port1_size - 1;
    bus_info.ce2_r_o = false;

    // nce3 = port2 (plugged)
    port2 = malloc( port2_size );

    char fullpath2[ MAX_LENGTH_FILENAME ];
    get_absolute_working_dir_path();
    sprintf( fullpath2, "%s%s", absolute_working_dir_path, filename2 );
    if ( config.verbose )
        fprintf( stderr, "fullpath = %s\n", fullpath1 );

    filesize = file_size( fullpath2 ) * 2;
    if ( filesize == port2_size )
        read_mem_file( fullpath2, port2, port2_size );
    port2mask = port2_size - 1;
    bus_info.nce3_data = port2;
    bus_info.nce3_mask = port2mask & 0x3FFFF;
    bus_info.nce3_r_o = false;

    bus_info.ben = false;
    current_bank = 0;
}
void ports_exit( char* filename1, char* filename2 )
{
    char fullpath1[ MAX_LENGTH_FILENAME ];
    get_absolute_working_dir_path();
    sprintf( fullpath1, "%s%s", absolute_working_dir_path, filename1 );
    if ( config.verbose )
        fprintf( stderr, "fullpath = %s\n", fullpath1 );

    char fullpath2[ MAX_LENGTH_FILENAME ];
    get_absolute_working_dir_path();
    sprintf( fullpath2, "%s%s", absolute_working_dir_path, filename2 );
    if ( config.verbose )
        fprintf( stderr, "fullpath = %s\n", fullpath1 );

    write_mem_file( fullpath1, bus_info.ce2_data, port1_size );
    write_mem_file( fullpath2, port2, port2_size );
}
