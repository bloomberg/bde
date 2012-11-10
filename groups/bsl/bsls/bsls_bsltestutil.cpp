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
    printf("%hhu", v);
}

void debugprint(short v)
{
    printf("%hd", v);
}

void debugprint(unsigned short v)
{
    printf("%hu", v);
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
    printf("'%f'", v);
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
    if (v) {
        printf("\"%s\"", v);
    } else {
        printf("(null)");
    }
}

void debugprint(char *v)
{
    if (v) {
        printf("\"%s\"", v);
    } else {
        printf("(null)");
    }
}

void debugprint(const volatile char *v)
{
    if (v) {
        printf("\"%s\"", v);
    } else {
        printf("(null)");
    }
}

void debugprint(volatile char *v)
{
    if (v) {
        printf("\"%s\"", v);
    } else {
        printf("(null)");
    }
}

void debugprint(void *v)
{
    printf("%p", v);
}

void debugprint(volatile void *v)
{
    printf("%p", v);
}

void debugprint(const void *v)
{
    printf("%p", v);
}

void debugprint(const volatile void *v)
{
    printf("%p", v);
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
