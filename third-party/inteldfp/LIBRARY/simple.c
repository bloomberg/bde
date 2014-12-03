#include <stdio.h>
#include <stdlib.h>

int foo( int __attribute__ (( aligned(128) )) fooarg );

int
main( void )
{
    printf( "Hello World!\n" );

    //char *buf= malloc( 300 );

    //free( buf );

    return 0;
}


int foo( int __attribute__ (( aligned(128) )) fooarg ) { return 0; }
