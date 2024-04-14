#ifndef __PFILES_H
#define __PFILES_H

extern void get_absolute_working_dir_path();

extern int file_size( char* filename );
extern void load_file_on_stack( char* filename );

extern void rom_init( char* filename );
extern void rom_exit();

extern void ram_init( char* filename );
extern void ram_exit( char* filename );

extern void ports_init( char* filename1, char* filename2 );
extern void ports_exit( char* filename1, char* filename2 );

#endif
