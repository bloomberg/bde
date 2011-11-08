// bsls_bsltestutil.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLS_BSLTESTUTIL
#define INCLUDED_BSLS_BSLTESTUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide low level test utilities that do not use <iostream>.
//
//@CLASSES:
//
//@AUTHOR: Alisdair Meredith (ameredit) 
//
//@DESCRIPTION: This component ...
//
///Usage
///-----
 
//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
# define BSLS_BSLTESTUTIL_LOOP_ASSERT(I,X) { \
    if (!(X)) { bsls_BslTestUtil::debugPrint(#I ": ", I, ",\t"); \
                aSsErT(!(X), #X, __LINE__); } }

# define BSLS_BSLTESTUTIL_LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsls_BslTestUtil::debugPrint(#I ": ", I, "\t");  \
                bsls_BslTestUtil::debugPrint(#J ": ", J, ",\t"); \
                aSsErT(!(X), #X, __LINE__); } }

#define BSLS_BSLTESTUTIL_LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { bsls_BslTestUtil::debugPrint(#I ": ", I, "\t");  \
                bsls_BslTestUtil::debugPrint(#J ": ", J, "\t");  \
                bsls_BslTestUtil::debugPrint(#K ": ", K, ",\t"); \
                aSsErT(!(X), #X, __LINE__); } }

#define BSLS_BSLTESTUTIL_LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { bsls_BslTestUtil::debugPrint(#I ": ", I, "\t");  \
                bsls_BslTestUtil::debugPrint(#J ": ", J, "\t");  \
                bsls_BslTestUtil::debugPrint(#K ": ", K, "\t");  \
                bsls_BslTestUtil::debugPrint(#L ": ", L, ",\t"); \
                aSsErT(!(X), #X, __LINE__); } }

#define BSLS_BSLTESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X) { \
    if (!(X)) { bsls_BslTestUtil::debugPrint(#I ": ", I, "\t");  \
                bsls_BslTestUtil::debugPrint(#J ": ", J, "\t");  \
                bsls_BslTestUtil::debugPrint(#K ": ", K, "\t");  \
                bsls_BslTestUtil::debugPrint(#L ": ", L, "\t");  \
                bsls_BslTestUtil::debugPrint(#M ": ", M, ",\t"); \
                aSsErT(!(X), #X, __LINE__); } }

#define BSLS_BSLTESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X) { \
    if (!(X)) { bsls_BslTestUtil::debugPrint(#I ": ", I, "\t");  \
                bsls_BslTestUtil::debugPrint(#J ": ", J, "\t");  \
                bsls_BslTestUtil::debugPrint(#K ": ", K, "\t");  \
                bsls_BslTestUtil::debugPrint(#L ": ", L, "\t");  \
                bsls_BslTestUtil::debugPrint(#M ": ", M, "\t");  \
                bsls_BslTestUtil::debugPrint(#N ": ", N, ",\t"); \
                aSsErT(!(X), #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define BSLS_BSLTESTUTIL_Q(X)  bsls_BslTestUtil::printString("<| " #X " |>\n");
    // Quote identifier literally.

#define BSLS_BSLTESTUTIL_P(X)  bsls_BslTestUtil::debugPrint(#X " = ", X, "\n");
    // Print identifier and vue.

#define BSLS_BSLTESTUTIL_P_(X) bsls_BslTestUtil::debugPrint(#X " = ", X, ", ");
    // P(X) without '\n'.

#define BSLS_BSLTESTUTIL_L_ __LINE__                     // current Line number
#define BSLS_BSLTESTUTIL_T_ bsls_BslTestUtil::printTab();
    // Print a tab (w/o newline).

//-----------------------------------------------------------------------------

namespace BloombergLP {

struct bsls_BslTestUtil
{
    // This class provides a namespace for utilities that are useful when
    // writing a test driver that may not use the standard C++ iostream
    // facilities.  This is a typical requirement for test drivers in the 'bsl'
    // package group.

  public:
        
    static void debugPrint(const char *s, bool v, const char *t);
        // Print a message to the console consististing of the specified
        // initial string 's', followed by the specified value 'v' formatted
        // as a string, followed by the specified trailing string 't', then
        // 'flush' the stream to ensure the text is written.

    static void debugPrint(const char *s, char v, const char *t);
    static void debugPrint(const char *s, signed char v, const char *t);
    static void debugPrint(const char *s, unsigned char v, const char *t);
    static void debugPrint(const char *s, short v, const char *t);
    static void debugPrint(const char *s, unsigned short v, const char *t);
    static void debugPrint(const char *s, int v, const char *t);
    static void debugPrint(const char *s, unsigned int v, const char *t);
    static void debugPrint(const char *s, long v, const char *t);
    static void debugPrint(const char *s, unsigned long v, const char *t);
    static void debugPrint(const char *s, long long v, const char *t);
    static void debugPrint(const char *s, unsigned long long v, const char *t);
        // Print a message to the console consististing of the specified
        // initial string 's', followed by the specified value 'v' formatted
        // as a string, followed by the specified trailing string 't', then
        // 'flush' the stream to ensure the text is written.

    static void debugPrint(const char *s, float v, const char *t);
    static void debugPrint(const char *s, double v, const char *t);
    static void debugPrint(const char *s, long double v, const char *t);
        // Print a message to the console consististing of the specified
        // initial string 's', followed by the specified value 'v' formatted
        // as a string enclosed by single-quote characters ('), followed by the
        // specified trailing string 't', then 'flush' the stream to ensure the
        // text is written.

    static void debugPrint(const char *s, char *v, const char *t);
    static void debugPrint(const char *s, const char *v, const char *t);
        // Print a message to the console consististing of the specified
        // initial string 's', followed by the specified string 'v' enclosed by
        // quote characters ("), followed by the specified trailing string 't',
        // then 'flush' the stream to ensure the text is written.

    static void debugPrint(const char *s, void *p, const char *t);
    static void debugPrint(const char *s, const void *p, const char *t);
        // Print a message to the console consististing of the specified
        // initial string 's', followed by the specified pointer address 'p'
        // formatted as a hexadecimal value, followed by the specified trailing
        // string 't', then 'flush' the stream to ensure the text is written.

    static void printString(const char *s);
        // Print the specified string 's' to the console.  Note that the stream
        // is *not* flushed.

    static void printTab();
        // Print a tab character to the console, and then 'flush' the stream.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
