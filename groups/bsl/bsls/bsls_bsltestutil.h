// bsls_bsltestutil.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLS_BSLTESTUTIL
#define INCLUDED_BSLS_BSLTESTUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide test utilities for 'bsl' that do not use <iostream>.
//
//@CLASSES:
//   bsls_BslTestUtil: utilities to aid writing 'bsl' test drivers
//
//@AUTHOR: Alisdair Meredith (ameredit)
//
//@DESCRIPTION: This component provides a set of macros and utility functions
// to support writing test drivers in the 'bsl' package group that is layered
// below the standard library, restricting the use of 'iostreams'.  The inent
// is to use only the macros listed below to implement the standard test driver
// macros inside a given component.  It is required that the "host" test driver
// define the standard 'aSsErT' function.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Writing a test driver
/// - - - - - - - - - - - - - - - -
// First we write a component to test, which might hold the following utility
// class.
//..
//  struct bsls_BslTestUtil_FortyTwo {
//      // This utility class provides sample functionality to demonstrate
//      // how a test driver might be written validating its only method.
//
//      static int value();
//          // Return the integer value '42'.
//  };
//
//  inline
//  int bsls_BslTestUtil_FortyTwo::value()
//  {
//      return 42;
//  }
//..
// Then, we can write a test driver for this component.  We start by providing
// the standard BDE assert test macro.
//..
//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
//  static int testStatus = 0;
//
//  static void aSsErT(bool b, const char *s, int i)
//  {
//      if (b) {
//          printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
//          if (testStatus >= 0 && testStatus <= 100) ++testStatus;
//      }
//  }
//
//# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//..
// Next, we define the standard print and LOOP_ASSERT macros, as aliases to the
// macros defined by this component.
//..
//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------
//#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
//#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
//#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
//#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
//#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
//#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
//
//#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
//#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
//#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
//#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
//#define L_  BSLS_BSLTESTUTIL_L_  // current Line number
//..
// Finally, we write the test case for the 'value' function of this component,
// using the (standard) abbreviated macro names we have just defined.
//..
//    case 2: {
//      // --------------------------------------------------------------------
//      // TESTING USAGE EXAMPLE
//      //
//      // Concerns
//      //: 1 The usage example provided in the component header file must
//      //:   compile, link, and run on all platforms as shown.
//      //
//      // Plan:
//      //: 1 Incorporate usage example from header into driver, remove leading
//      //:   comment characters, and replace 'assert' with 'ASSERT'.  (C-1)
//      //
//      // Testing:
//      //   USAGE EXAMPLE
//      // --------------------------------------------------------------------
//
//      if (verbose) printf("\nTESTING USAGE EXAMPLE"
//                          "\n---------------------\n");
//
//      const int value = bsls_BslTestUtil_FortyTwo::value();
//      if (verbose) P(value);
//      LOOP_ASSERT(value, 42 == value);
//    } break;
//..
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

    static void debugPrint(const char *s, bool b, const char *t);
        // Print a message to the console consististing of the specified
        // initial string 's', followed by the string "true" if the specified
        // 'b' is true, or the string "false" otherwise, followed by the
        // specified trailing string 't', then 'flush' the stream to ensure the
        // text is written.

    static void debugPrint(const char *s, char c, const char *t);
        // Print a message to the console consististing of the specified
        // initial string 's', followed by the specified character 'c' enclosed
        // by single-quote characters ('), followed by the specified trailing
        // string 't', then 'flush' the stream to ensure the text is written.

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
        // initial string 's', followed by the specified integer value 'v'
        // formatted as a string, followed by the specified trailing string
        // 't', then 'flush' the stream to ensure the text is written.

    static void debugPrint(const char *s, float v, const char *t);
    static void debugPrint(const char *s, double v, const char *t);
    static void debugPrint(const char *s, long double v, const char *t);
        // Print a message to the console consististing of the specified
        // initial string 's', followed by the specified value 'v' formatted
        // as a string enclosed by single-quote characters ('), followed by the
        // specified trailing string 't', then 'flush' the stream to ensure the
        // text is written.

    static void debugPrint(const char *s, char *str, const char *t);
    static void debugPrint(const char *s, const char *str, const char *t);
        // Print a message to the console consististing of the specified
        // initial string 's', followed by the specified string 'str' enclosed
        // by quote characters ("), followed by the specified trailing string
        // 't', then 'flush' the stream to ensure the text is written.

    static void debugPrint(const char *s, void *p, const char *t);
    static void debugPrint(const char *s, const void *p, const char *t);
        // Print a message to the console consististing of the specified
        // initial string 's', followed by the specified memory address 'p'
        // formatted as a hexadecimal integer, followed by the specified
        // trailing string 't', then 'flush' the stream to ensure the text is
        // written.

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
