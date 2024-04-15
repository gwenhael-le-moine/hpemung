#ifndef __FILES_H
#define __FILES_H

extern void get_absolute_working_dir_path();

extern int file_size( char* filename );
extern void load_file_on_stack( char* filename );

extern void rom_init( char* filename );
extern void ram_init( char* filename );
extern void ports_init( char* filename1, char* filename2 );
extern void cpu_init( char* filename );
extern void bus_init( char* filename );

extern void rom_exit();
extern void ram_exit( char* filename );
extern void ports_exit( char* filename1, char* filename2 );
extern void cpu_exit( char* filename );
extern void bus_exit( char* filename );

#endif
