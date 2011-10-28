// bsls_testutil.cpp                                                  -*-C++-*-
#include <bsls_testutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <stdio.h>        // 'printf'

namespace BloombergLP
{

void bsls_TestUtil::flush()
{
    fflush(stdout);
}

void bsls_TestUtil::printString(const char *s)
{
    printf("%s", s);
}

void bsls_TestUtil::printTab()
{
   putchar('\t');
   flush();
}

// Fundamental-type-specific print functions.
void bsls_TestUtil::printValue(bool b)
{
    printf(b ? "true" : "false");
}

void bsls_TestUtil::printValue(char c)
{
    printf("%c", c);
}

void bsls_TestUtil::printValue(unsigned char c)
{
    printf("%c", c);
}

void bsls_TestUtil::printValue(signed char c)
{
    printf("%c", c);
}

void bsls_TestUtil::printValue(short val)
{
    printf("%d", (int)val);
}

void bsls_TestUtil::printValue(unsigned short val)
{
    printf("%d", (int)val);
}

void bsls_TestUtil::printValue(int val)
{
    printf("%d", val);
}

void bsls_TestUtil::printValue(unsigned int val)
{
    printf("%u", val);
}

void bsls_TestUtil::printValue(long val)
{
    printf("%ld", val);
}

void bsls_TestUtil::printValue(unsigned long val)
{
    printf("%lu", val);
}

void bsls_TestUtil::printValue(long long val)
{
    printf("%lld", val);
}

void bsls_TestUtil::printValue(unsigned long long val)
{
    printf("%llu", val);
}

void bsls_TestUtil::printValue(float val)
{
    printf("'%f'", (double)val);
}

void bsls_TestUtil::printValue(double val)
{
    printf("'%f'", val);
}

void bsls_TestUtil::bsls_TestUtil::printValue(long double val)
{
    printf("'%Lf'", val);
}

void bsls_TestUtil::printValue(const char *s)
{
   printf("\"%s\"", s);
}

void bsls_TestUtil::printValue(char *s)
{
   printf("\"%s\"", s);
}

void bsls_TestUtil::printValue(void *p)
{
   printf("%p", p);
}

void bsls_TestUtil::printValue(const void *p)
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
