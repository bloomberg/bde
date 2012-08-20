// bsls_bsltestutil.cpp                                               -*-C++-*-
#include <bsls_bsltestutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <stdio.h>        // 'printf'

namespace
{

// ----------------------------------------------------------------------------
//                             INLINE FREE FUNCTIONS
// ----------------------------------------------------------------------------

void flush();
    // Flush stdout.

// Fundamental-type-specific print functions.

void printValue(bool val);
    // Print 'true' to stdout if the specified boolean 'val' is true, and print
    // 'false' otherwise.

void printValue(char val);
    // Print the specified character 'val' to stdout, surrounded by single
    // quotes.

void printValue(signed char val);
void printValue(unsigned char val);
void printValue(short val);
void printValue(unsigned short val);
void printValue(int val);
void printValue(unsigned int val);
void printValue(long val);
void printValue(unsigned long val);
void printValue(long long val);
void printValue(unsigned long long val);
void printValue(float val);
void printValue(double val);
void printValue(long double val);
    // Print the specified value 'val' to stdout, using the default 'printf'
    // format for the the primitive type of 'val'.

void printValue(const char *val);
void printValue(char *val);
    // Print the specified null-terminated byte string 'val' to stdout,
    // surrounded by double quotes.

void printValue(void *val);
void printValue(const void *val);
    // Print the specified pointer 'val' to stdout, using the default 'printf'
    // format for addresses.

template <typename BSLS_TYPE>
void doDebugPrint(const char *s, const BSLS_TYPE& v, const char *t);
    // Print the specified null-terminated byte string 's', followed by the
    // string representation of the specified 'v', formatted according to the
    // type-appropriate overload of 'printValue', followed by the specified
    // null-terminated byte string 't'.

inline
void flush()
{
    fflush(stdout);
}

// Fundamental-type-specific print functions.
inline
void printValue(bool val)
{
    printf(val ? "true" : "false");
}

inline
void printValue(char val)
{
    printf("'%c'", val);
}

inline
void printValue(signed char val)
{
    printf("%hhd", val);
}

inline
void printValue(unsigned char val)
{
    printf("%hhu", val);
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
    printf("%f", (double)val);
}

inline
void printValue(double val)
{
    printf("%g", val);
}

inline
void printValue(long double val)
{
    // Note that the "C" standard requires the inconsistent upper case 'L' in
    // the format string below, while requiring lower case 'l' characters in
    // the format strings for integer values above.

    printf("%Lg", val);
}

inline
void printValue(const char *val)
{
    printf("\"%s\"", val);
}

inline
void printValue(char *val)
{
    printf("\"%s\"", val);
}

inline
void printValue(void *val)
{
    printf("%p", val);
}

inline
void printValue(const void *val)
{
    printf("%p", val);
}

template <typename BSLS_TYPE>
void doDebugPrint(const char *s, const BSLS_TYPE& v, const char *t)
{
    BloombergLP::bsls::BslTestUtil::printStringNoFlush(s);
    printValue(v);
    BloombergLP::bsls::BslTestUtil::printStringNoFlush(t);
    flush();
}

}  // close unnamed namespace

namespace BloombergLP
{

namespace bsls {

void BslTestUtil::debugPrint(const char *s, bool b, const char *t)
{
    doDebugPrint(s, b, t);
}

void BslTestUtil::debugPrint(const char *s, char c, const char *t)
{
    doDebugPrint(s, c, t);
}

void BslTestUtil::debugPrint(const char *s, signed char v, const char *t)
{
    doDebugPrint(s, v, t);
}

void
BslTestUtil::debugPrint(const char *s, unsigned char v, const char *t)
{
    doDebugPrint(s, v, t);
}

void BslTestUtil::debugPrint(const char *s, short v, const char *t)
{
    doDebugPrint(s, v, t);
}

void
BslTestUtil::debugPrint(const char *s, unsigned short v, const char *t)
{
    doDebugPrint(s, v, t);
}

void BslTestUtil::debugPrint(const char *s, int v, const char *t)
{
    doDebugPrint(s, v, t);
}

void BslTestUtil::debugPrint(const char *s, unsigned int v, const char *t)
{
    doDebugPrint(s, v, t);
}

void BslTestUtil::debugPrint(const char *s, long v, const char *t)
{
    doDebugPrint(s, v, t);
}

void
BslTestUtil::debugPrint(const char *s, unsigned long v, const char *t)
{
    doDebugPrint(s, v, t);
}

void BslTestUtil::debugPrint(const char *s, long long v, const char *t)
{
    doDebugPrint(s, v, t);
}

void BslTestUtil::debugPrint(const char         *s,
                             unsigned long long  v,
                             const char         *t)
{
    doDebugPrint(s, v, t);
}

void BslTestUtil::debugPrint(const char *s, float v, const char *t)
{
    doDebugPrint(s, v, t);
}

void BslTestUtil::debugPrint(const char *s, double v, const char *t)
{
    doDebugPrint(s, v, t);
}

void BslTestUtil::debugPrint(const char *s, long double v, const char *t)
{
    doDebugPrint(s, v, t);
}

void BslTestUtil::debugPrint(const char *s, char *str, const char *t)
{
    doDebugPrint(s, str, t);
}

void BslTestUtil::debugPrint(const char *s, const char *str, const char *t)
{
    doDebugPrint(s, str, t);
}

void BslTestUtil::debugPrint(const char *s, void *p, const char *t)
{
    doDebugPrint(s, p, t);
}

void BslTestUtil::debugPrint(const char *s, const void *p, const char *t)
{
    doDebugPrint(s, p, t);
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
