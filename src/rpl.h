#ifndef __RPL_H
#define __RPL_H

#include "types.h"

int rpl_object_size( byte* obj );
address rpl_make_temp( address size );
void rpl_push( address adr );
int rpl_push_object( byte* obj, address size );

#endif
