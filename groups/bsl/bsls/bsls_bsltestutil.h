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
//  bsls::BslTestUtil: utilities to aid writing 'bsl' test drivers
//
//@MACROS:
//  BSLS_BSLTESTUTIL_LOOP_ASSERT(I, X)
//  BSLS_BSLTESTUTIL_LOOP2_ASSERT(I, J, X)
//  BSLS_BSLTESTUTIL_LOOP3_ASSERT(I, J, K, X)
//  BSLS_BSLTESTUTIL_LOOP4_ASSERT(I, J, K, L, X)
//  BSLS_BSLTESTUTIL_LOOP5_ASSERT(I, J, K, L, M, X)
//  BSLS_BSLTESTUTIL_LOOP6_ASSERT(I, J, K, L, M, N, X)
//  BSLS_BSLTESTUTIL_Q(X)
//  BSLS_BSLTESTUTIL_P(X)
//  BSLS_BSLTESTUTIL_P_(X)
//  BSLS_BSLTESTUTIL_L_
//  BSLS_BSLTESTUTIL_T_
//
//@AUTHOR: Alisdair Meredith (ameredit)
//
//@DESCRIPTION: This component provides a set of macros and utility functions
// to support writing test drivers in the 'bsl' package group.  This utility is
// necessary because parts of 'bsl' reside below the standard library,
// precluding the use of 'iostreams'.  The intent is for the clients of this
// component to use only the macros listed below to implement standard test
// driver macros.  Instead of the standard 'ASSERT' macro, it is required that
// a client test driver define the standard 'aSsErT' function.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Writing a test driver
/// - - - - - - - - - - - - - - - -
// First, we write a component to test, which provides a utility class:
//..
//  namespace bslexample {
//
//  struct BslExampleUtil {
//      // This utility class provides sample functionality to demonstrate how
//      // a test driver might be written validating its only method.
//
//      static int fortyTwo();
//          // Return the integer value '42'.
//  };
//
//  inline
//  int BslExampleUtil::fortyTwo()
//  {
//      return 42;
//  }
//
//  } // close package namespace
//..
// Then, we can write a test driver for this component.  We start by providing
// the standard BDE assert test macro:
//..
//  //=========================================================================
//  //                       STANDARD BDE ASSERT TEST MACRO
//  //-------------------------------------------------------------------------
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
//  # define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//..
// Next, we define the standard print and LOOP_ASSERT macros, as aliases to the
// macros defined by this component:
//..
//  //=========================================================================
//  //                       STANDARD BDE TEST DRIVER MACROS
//  //-------------------------------------------------------------------------
//  #define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
//  #define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
//  #define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
//  #define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
//  #define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
//  #define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
//
//  #define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
//  #define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
//  #define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
//  #define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
//  #define L_  BSLS_BSLTESTUTIL_L_  // current Line number
//..
// Then, we define the test driver for the 'static' 'fortyTwo' method, uwing
// the (standard) abbreviated macro names we have just defined.
//..
//  void testFortyTwo(bool verbose)
//  {
//      const int value = bslexample::BslExampleUtil::fortyTwo();
//      if (verbose) P(value);
//      LOOP_ASSERT(value, 42 == value);
//  }
//..
// Finally, we write the test case calling our test driver.
//..
//  case 9: {
//    // --------------------------------------------------------------------
//    // TESTING USAGE EXAMPLE
//    //
//    // Concerns
//    //: 1 The usage example provided in the component header file must
//    //:   compile, link, and run on all platforms as shown.
//    //
//    // Plan:
//    //: 1 Incorporate usage example from header into driver, remove leading
//    //:   comment characters, and replace 'assert' with 'ASSERT'.  (C-1)
//    //
//    // Testing:
//    //   USAGE EXAMPLE
//    // --------------------------------------------------------------------
//
//    if (verbose) printf("\nTESTING USAGE EXAMPLE"
//                        "\n---------------------\n");
//
//    testFortyTwo(verbose);
//  } break;
//..

                       // =================
                       // Macro Definitions
                       // =================

# define BSLS_BSLTESTUTIL_LOOP_ASSERT(I,X) { \
    if (!(X)) { bsls::BslTestUtil::debugPrint(#I ": ", I, "\n"); \
                aSsErT(!(X), #X, __LINE__); } }

# define BSLS_BSLTESTUTIL_LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsls::BslTestUtil::debugPrint(#I ": ", I, "\t");  \
                bsls::BslTestUtil::debugPrint(#J ": ", J, "\n"); \
                aSsErT(!(X), #X, __LINE__); } }

#define BSLS_BSLTESTUTIL_LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { bsls::BslTestUtil::debugPrint(#I ": ", I, "\t");  \
                bsls::BslTestUtil::debugPrint(#J ": ", J, "\t");  \
                bsls::BslTestUtil::debugPrint(#K ": ", K, "\n"); \
                aSsErT(!(X), #X, __LINE__); } }

#define BSLS_BSLTESTUTIL_LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { bsls::BslTestUtil::debugPrint(#I ": ", I, "\t");  \
                bsls::BslTestUtil::debugPrint(#J ": ", J, "\t");  \
                bsls::BslTestUtil::debugPrint(#K ": ", K, "\t");  \
                bsls::BslTestUtil::debugPrint(#L ": ", L, "\n"); \
                aSsErT(!(X), #X, __LINE__); } }

#define BSLS_BSLTESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X) { \
    if (!(X)) { bsls::BslTestUtil::debugPrint(#I ": ", I, "\t");  \
                bsls::BslTestUtil::debugPrint(#J ": ", J, "\t");  \
                bsls::BslTestUtil::debugPrint(#K ": ", K, "\t");  \
                bsls::BslTestUtil::debugPrint(#L ": ", L, "\t");  \
                bsls::BslTestUtil::debugPrint(#M ": ", M, "\n"); \
                aSsErT(!(X), #X, __LINE__); } }

#define BSLS_BSLTESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X) { \
    if (!(X)) { bsls::BslTestUtil::debugPrint(#I ": ", I, "\t");  \
                bsls::BslTestUtil::debugPrint(#J ": ", J, "\t");  \
                bsls::BslTestUtil::debugPrint(#K ": ", K, "\t");  \
                bsls::BslTestUtil::debugPrint(#L ": ", L, "\t");  \
                bsls::BslTestUtil::debugPrint(#M ": ", M, "\t");  \
                bsls::BslTestUtil::debugPrint(#N ": ", N, "\n"); \
                aSsErT(!(X), #X, __LINE__); } }

#define BSLS_BSLTESTUTIL_Q(X)                                                 \
                       bsls::BslTestUtil::printStringNoFlush("<| " #X " |>\n");
    // Quote identifier literally.

#define BSLS_BSLTESTUTIL_P(X) bsls::BslTestUtil::debugPrint(#X " = ", X, "\n");
    // Print identifier and its value.

#define BSLS_BSLTESTUTIL_P_(X)                                                \
                              bsls::BslTestUtil::debugPrint(#X " = ", X, ", ");
    // P(X) without '\n'.

#define BSLS_BSLTESTUTIL_L_ __LINE__
    // current Line number

#define BSLS_BSLTESTUTIL_T_ bsls::BslTestUtil::printTab();
    // Print a tab (w/o newline).

namespace BloombergLP {

namespace bsls {

                                  //===================
                                  // struct BslTestUtil
                                  //===================


struct BslTestUtil {
    // This class provides a namespace for utilities that are useful when
    // writing a test driver that is not permitted to use the standard C++
    // iostream facilities, which is typical of test drivers in the 'bsl'
    // package group.

  public:
    // CLASS METHODS
    static void debugPrint(const char *s, bool b, const char *t);
        // Print a message to the console, consististing of the specified
        // initial string, 's', followed by the string "true" if the specified
        // 'b' is true, and the string "false" otherwise, followed by the
        // specified trailing string 't'; then 'flush' the underlying stream to
        // ensure the text is written.

    static void debugPrint(const char *s, char c, const char *t);
        // Print a message to the console consististing of the specified
        // initial string, 's', followed by the specified character, 'c',
        // enclosed by single-quote characters ('), followed by the specified
        // trailing string, 't'; then 'flush' the stream to ensure the text is
        // written.

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
        // Print a message to the console, consististing of the specified
        // initial string, 's', followed by the specified integer value, 'v',
        // formatted as a string, followed by the specified trailing string,
        // 't'; then 'flush' the underlying stream to ensure the text is
        // written.

    static void debugPrint(const char *s, float v, const char *t);
    static void debugPrint(const char *s, double v, const char *t);
    static void debugPrint(const char *s, long double v, const char *t);
        // Print a message to the console, consististing of the specified
        // initial string, 's', followed by the specified value, 'v', formatted
        // as a string enclosed by single-quote characters ('), followed by the
        // specified trailing string 't'; then 'flush' the underlying stream to
        // ensure the text is written.

    static void debugPrint(const char *s, char *str, const char *t);
    static void debugPrint(const char *s, const char *str, const char *t);
        // Print a message to the console, consististing of the specified
        // initial string, 's', followed by the specified string, 'str',
        // enclosed by quote characters ("), followed by the specified trailing
        // string, 't'; then 'flush' the underlying stream to ensure the text
        // is written.

    static void debugPrint(const char *s, void *p, const char *t);
    static void debugPrint(const char *s, const void *p, const char *t);
        // Print a message to the console, consististing of the specified
        // initial string, 's', followed by the specified memory address, 'p',
        // formatted as a hexadecimal integer, followed by the specified
        // trailing string, 't'; then 'flush' the underlying stream to ensure
        // the text is written.

    static void printStringNoFlush(const char *s);
        // Print the specified string, 's', to the console.  Note that the
        // underlying stream is *not* flushed.

    static void printTab();
        // Print a tab character to the console, and then 'flush' the
        // underlying stream to ensure the text is written.
};

}  // close package namespace

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
