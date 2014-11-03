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
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
