#include "types.h"

unsigned int nib_to_unsigned( byte* nib, int len )
{
    int x = 0;

    while ( len-- ) {
        x <<= 4;
        x |= nib[ len ];
    }
    return x;
}

void unsigned_to_nib( byte* nib, int x, int len )
{
    while ( len-- ) {
        *nib++ = x & 0xF;
        x >>= 4;
    }
}
