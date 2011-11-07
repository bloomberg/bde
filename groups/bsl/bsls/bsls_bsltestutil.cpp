// bsls_bsltestutil.cpp                                               -*-C++-*-
#include <bsls_bsltestutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <stdio.h>        // 'printf'

namespace BloombergLP
{

void bsls_BslTestUtil::flush()
{
    fflush(stdout);
}

void bsls_BslTestUtil::printString(const char *s)
{
    printf("%s", s);
}

void bsls_BslTestUtil::printTab()
{
   putchar('\t');
   flush();
}

// Fundamental-type-specific print functions.
void bsls_BslTestUtil::printValue(bool b)
{
    printf(b ? "true" : "false");
}

void bsls_BslTestUtil::printValue(char c)
{
    printf("%c", c);
}

void bsls_BslTestUtil::printValue(unsigned char c)
{
    printf("%c", c);
}

void bsls_BslTestUtil::printValue(signed char c)
{
    printf("%c", c);
}

void bsls_BslTestUtil::printValue(short val)
{
    printf("%d", (int)val);
}

void bsls_BslTestUtil::printValue(unsigned short val)
{
    printf("%d", (int)val);
}

void bsls_BslTestUtil::printValue(int val)
{
    printf("%d", val);
}

void bsls_BslTestUtil::printValue(unsigned int val)
{
    printf("%u", val);
}

void bsls_BslTestUtil::printValue(long val)
{
    printf("%ld", val);
}

void bsls_BslTestUtil::printValue(unsigned long val)
{
    printf("%lu", val);
}

void bsls_BslTestUtil::printValue(long long val)
{
    printf("%lld", val);
}

void bsls_BslTestUtil::printValue(unsigned long long val)
{
    printf("%llu", val);
}

void bsls_BslTestUtil::printValue(float val)
{
    printf("'%f'", (double)val);
}

void bsls_BslTestUtil::printValue(double val)
{
    printf("'%f'", val);
}

void bsls_BslTestUtil::printValue(long double val)
{
    printf("'%Lf'", val);
}

void bsls_BslTestUtil::printValue(const char *s)
{
   printf("\"%s\"", s);
}

void bsls_BslTestUtil::printValue(char *s)
{
   printf("\"%s\"", s);
}

void bsls_BslTestUtil::printValue(void *p)
{
   printf("%p", p);
}

void bsls_BslTestUtil::printValue(const void *p)
{
   printf("%p", p);
}

}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
