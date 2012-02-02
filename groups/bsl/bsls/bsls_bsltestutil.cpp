// bsls_bsltestutil.cpp                                               -*-C++-*-
#include <bsls_bsltestutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <stdio.h>        // 'printf'

namespace
{

inline
void flush()
{
    fflush(stdout);
}

// Fundamental-type-specific print functions.
inline
void printValue(bool b)
{
    printf(b ? "true" : "false");
}

inline
void printValue(char c)
{
    printf("'%c'", c);
}

inline
void printValue(signed char c)
{
    printf("%hhd", c);
}

inline
void printValue(unsigned char c)
{
    printf("%hhu", c);
}

inline
void printValue(short val)
{
    printf("%hd", (int)val);
}

inline
void printValue(unsigned short val)
{
    printf("%hu", (int)val);
}

inline
void printValue(int val)
{
    printf("%d", val);
}

inline
void printValue(unsigned int val)
{
    printf("%u", val);
}

inline
void printValue(long val)
{
    printf("%ld", val);
}

inline
void printValue(unsigned long val)
{
    printf("%lu", val);
}

inline
void printValue(long long val)
{
    printf("%lld", val);
}

inline
void printValue(unsigned long long val)
{
    printf("%llu", val);
}

inline
void printValue(float val)
{
    printf("'%g'", (double)val);
}

inline
void printValue(double val)
{
    printf("'%g'", val);
}

inline
void printValue(long double val)
{
    // Note that the "C" standard requires the inconsistent upper case 'L' in
    // the format string below, while requiring lower case 'l' characters in
    // the format strings for integer values above.

    printf("'%Lg'", val);
}

inline
void printValue(const char *s)
{
    printf("\"%s\"", s);
}

inline
void printValue(char *s)
{
    printf("\"%s\"", s);
}

inline
void printValue(void *p)
{
    printf("%p", p);
}

inline
void printValue(const void *p)
{
    printf("%p", p);
}

template <typename BSLS_TYPE>
void doDebugPrint(const char *s, const BSLS_TYPE& v, const char *t)
{
    BloombergLP::bsls_BslTestUtil::printStringNoFlush(s);
    printValue(v);
    BloombergLP::bsls_BslTestUtil::printStringNoFlush(t);
    flush();
}

}  // close unnamed namespace

namespace BloombergLP
{

void bsls_BslTestUtil::debugPrint(const char *s, bool v, const char *t)
{
    doDebugPrint(s, v, t);
}

void bsls_BslTestUtil::debugPrint(const char *s, char v, const char *t)
{
    doDebugPrint(s, v, t);
}

void bsls_BslTestUtil::debugPrint(const char *s, signed char v, const char *t)
{
    doDebugPrint(s, v, t);
}

void
bsls_BslTestUtil::debugPrint(const char *s, unsigned char v, const char *t)
{
    doDebugPrint(s, v, t);
}

void bsls_BslTestUtil::debugPrint(const char *s, short v, const char *t)
{
    doDebugPrint(s, v, t);
}

void
bsls_BslTestUtil::debugPrint(const char *s, unsigned short v, const char *t)
{
    doDebugPrint(s, v, t);
}

void bsls_BslTestUtil::debugPrint(const char *s, int v, const char *t)
{
    doDebugPrint(s, v, t);
}

void bsls_BslTestUtil::debugPrint(const char *s, unsigned int v, const char *t)
{
    doDebugPrint(s, v, t);
}

void bsls_BslTestUtil::debugPrint(const char *s, long v, const char *t)
{
    doDebugPrint(s, v, t);
}

void
bsls_BslTestUtil::debugPrint(const char *s, unsigned long v, const char *t)
{
    doDebugPrint(s, v, t);
}

void bsls_BslTestUtil::debugPrint(const char *s, long long v, const char *t)
{
    doDebugPrint(s, v, t);
}

void bsls_BslTestUtil::debugPrint(const char         *s,
                                  unsigned long long  v,
                                  const char         *t)
{
    doDebugPrint(s, v, t);
}

void bsls_BslTestUtil::debugPrint(const char *s, float v, const char *t)
{
    doDebugPrint(s, v, t);
}

void bsls_BslTestUtil::debugPrint(const char *s, double v, const char *t)
{
    doDebugPrint(s, v, t);
}

void bsls_BslTestUtil::debugPrint(const char *s, long double v, const char *t)
{
    doDebugPrint(s, v, t);
}

void bsls_BslTestUtil::debugPrint(const char *s, char *v, const char *t)
{
    doDebugPrint(s, v, t);
}

void bsls_BslTestUtil::debugPrint(const char *s, const char *v, const char *t)
{
    doDebugPrint(s, v, t);
}

void bsls_BslTestUtil::debugPrint(const char *s, void *p, const char *t)
{
    doDebugPrint(s, p, t);
}

void bsls_BslTestUtil::debugPrint(const char *s, const void *p, const char *t)
{
    doDebugPrint(s, p, t);
}

void bsls_BslTestUtil::printStringNoFlush(const char *s)
{
    printf("%s", s);
}

void bsls_BslTestUtil::printTab()
{
    putchar('\t');
    flush();
}

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
