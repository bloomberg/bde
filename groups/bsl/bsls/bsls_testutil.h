// bsls_testutil.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLS_TESTUTIL
#define INCLUDED_BSLS_TESTUTIL

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
# define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsls_TestUtil::dbg_print(#I ": ", I, ",\t"); \
                aSsErT(!(X), #X, __LINE__); } }

# define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsls_TestUtil::dbg_print(#I ": ", I, "\t"); \
                bsls_TestUtil::dbg_print(#J ": ", J, ",\t"); \
                aSsErT(!(X), #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { bsls_TestUtil::dbg_print(#I ": ", I, "\t"); \
                bsls_TestUtil::dbg_print(#J ": ", J, "\t"); \
                bsls_TestUtil::dbg_print(#K ": ", K, ",\t"); \
                aSsErT(!(X), #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { bsls_TestUtil::dbg_print(#I ": ", I, "\t"); \
                bsls_TestUtil::dbg_print(#J ": ", J, "\t"); \
                bsls_TestUtil::dbg_print(#K ": ", K, "\t"); \
                bsls_TestUtil::dbg_print(#L ": ", L, ",\t"); \
                aSsErT(!(X), #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
    if (!(X)) { bsls_TestUtil::dbg_print(#I ": ", I, "\t"); \
                bsls_TestUtil::dbg_print(#J ": ", J, "\t"); \
                bsls_TestUtil::dbg_print(#K ": ", K, "\t"); \
                bsls_TestUtil::dbg_print(#L ": ", L, "\t"); \
                bsls_TestUtil::dbg_print(#M ": ", M, ",\t"); \
                aSsErT(!(X), #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
    if (!(X)) { bsls_TestUtil::dbg_print(#I ": ", I, "\t"); \
                bsls_TestUtil::dbg_print(#J ": ", J, "\t"); \
                bsls_TestUtil::dbg_print(#K ": ", K, "\t"); \
                bsls_TestUtil::dbg_print(#L ": ", L, "\t"); \
                bsls_TestUtil::dbg_print(#M ": ", M, "\t"); \
                bsls_TestUtil::dbg_print(#N ": ", N, ",\t"); \
                aSsErT(!(X), #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X)  bsls_TestUtil::printString("<| " #X " |>\n");
    // Quote identifier literally.

#define P(X)  bsls_TestUtil::dbg_print(#X " = ", X, "\n");
    // Print identifier and value.

#define P_(X) bsls_TestUtil::dbg_print(#X " = ", X, ", ");
    // P(X) without '\n'.

#define L_ __LINE__                         // current Line number
#define T_ bsls_TestUtil::printTab();       // Print a tab (w/o newline).

//-----------------------------------------------------------------------------

namespace BloombergLP {

struct bsls_TestUtil
{
  private:
    static void flush();

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
    static void printValue(const char *s);
    static void printValue(char *s);
    static void printValue(void *p);
    static void printValue(const void *p);

  public:
        
    template <typename T>
    static void dbg_print(const char *s, const T& val, const char *nl);

    static void printString(const char *s);

    static void printTab();
};

// Generic debug print function (3-arguments).
template <typename T>
void bsls_TestUtil::dbg_print(const char *s, const T& val, const char *nl)
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
