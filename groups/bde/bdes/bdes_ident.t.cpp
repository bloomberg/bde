// bdes_ident.t.cpp    -*-C++-*-

///Usage
///-----
// Include 'bdes_ident.h' and use the BDES_IDENT macro.  For header files this
// should be done directly after the include guards, e.g., bdes_somefile.h:
//..
    // bdes_somefile.h            -*-C++-*-
    #ifndef INCLUDED_BDES_SOMEFILE
    #define INCLUDED_BDES_SOMEFILE

    #include <bdes_ident.h>
    BDES_IDENT("$Id: $")

    // ...

    #endif // INCLUDED_BDES_SOMEFILE
//..
// For cpp files it should be done directly after the comment for the file name
// and the language, e.g., bdes_somefile.cpp:
//..
    // bdes_ident.t.cpp           -*-C++-*-

    #include <bdes_ident.h>
    BDES_IDENT("$Id: $")
//..

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;

    // This component has no run-time-testable facilities.
    bsl::fprintf(stderr, "WARNING: CASE '%d' NOT FOUND.\n", test);
    return -1;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
