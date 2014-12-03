#include <stdlib.h>


int
main()
{
    char *buf;
    buf= malloc( 100 );

    free( buf );

    return 0;
}
