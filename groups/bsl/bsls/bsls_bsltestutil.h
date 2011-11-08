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
    // Print identifier and value.

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

  private:
    static void flush();
        // Commit any outstanding write operations to the standard console.

    // Fundamental-type-specific print functions.
    // One print function for each fundamental type.
    static void printValue(bool b);
    static void printValue(char c);
    static void printValue(unsigned char c);
    static void printValue(signed char c);
    static void printValue(short val);
    static void printValue(unsigned short val);
    static void printValue(int val);
    static void printValue(unsigned int val);
    static void printValue(long val);
    static void printValue(unsigned long val);
    static void printValue(long long val);
    static void printValue(unsigned long long val);
    static void printValue(float val);
    static void printValue(double val);
    static void printValue(long double val);
    static void printValue(char *s);
    static void printValue(const char *s);
    static void printValue(void *p);
    static void printValue(const void *p);
        // Print the specified argument to the console using the "C" library
        // function 'printf' and a format string appropriate for the type of
        // argument.

  public:
        
    template <typename BSLS_TYPE>
    static void debugPrint(const char      *s,
                          const BSLS_TYPE&  val,
                          const char       *nl);
        // Print a message to the console consististing of the specified
        // initial string 's', followed by the specified value 'val' formatted
        // as a string, followed by the specified trailing string 'nl', then
        // 'flush' the stream to ensure the text is written.  

    static void printString(const char *s);
        // Print the specified string 's' to the console.

    static void printTab();
        // Print a tab character to the console, and then 'flush' the stream.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

template <typename BSLS_TYPE>
void bsls_BslTestUtil::debugPrint(const char       *s,
                                  const BSLS_TYPE&  val,
                                  const char       *nl)
{
    printString(s);
    printValue(val);
    printString(nl);
    flush();
}

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
