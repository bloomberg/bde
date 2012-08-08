// bsls_bsltestutil.cpp                                               -*-C++-*-
#include <bsls_bsltestutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <stdio.h>  // for 'printf'

namespace BloombergLP {
namespace bsls {

                   // ------------------
                   // struct BslTestUtil
                   // ------------------
// CLASS METHODS
void BslTestUtil::flush()
{
    fflush(stdout);
}

void BslTestUtil::printStringNoFlush(const char *s)
{
    printf("%s", s);
}

void BslTestUtil::printTab()
{
    putchar('\t');
    flush();
}


// FREE FUNCTIONS
void debugprint(bool v)
{
    printf(v ? "true" : "false");
}

void debugprint(char v)
{
    printf("'%c'", v);
}

void debugprint(signed char v)
{
    printf("%hhd", v);
}

void debugprint(unsigned char v)
{
    printf("%hhd", v);
}

void debugprint(short v)
{
    printf("%hd", (int)v);
}

void debugprint(unsigned short v)
{
    printf("%hu", (int)v);
}

void debugprint(int v)
{
    printf("%d", v);
}

void debugprint(unsigned int v)
{
    printf("%u", v);
}

void debugprint(long v)
{
    printf("%ld", v);
}

void debugprint(unsigned long v)
{
    printf("%lu", v);
}

void debugprint(long long v)
{
    printf("%lld", v);
}

void debugprint(unsigned long long v)
{
    printf("%llu", v);
}

void debugprint(float v)
{
    printf("'%g'", (double)v);
}

void debugprint(double v)
{
    printf("'%g'", v);
}

void debugprint(long double v)
{
    // Note that the "C" standard requires the inconsistent upper case 'L' in
    // the format string below, while requiring lower case 'l' characters in
    // the format strings for integer values above.

    printf("'%Lg'", v);
}

void debugprint(const char *v)
{
    printf("\"%s\"", v);
}

void debugprint(char *v)
{
    printf("\"%s\"", v);
}

void debugprint(const void *v)
{
    printf("%p", v);
}

}  // close package namespace
}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
