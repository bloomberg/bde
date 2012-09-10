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
//  BSLS_BSLTESTUTIL_LOOP_ASSERT(I, X): print args if '!X'
//  BSLS_BSLTESTUTIL_LOOP2_ASSERT(I, J, X): print args if '!X'
//  BSLS_BSLTESTUTIL_LOOP3_ASSERT(I, J, K, X): print args if '!X'
//  BSLS_BSLTESTUTIL_LOOP4_ASSERT(I, J, K, L, X): print args if '!X'
//  BSLS_BSLTESTUTIL_LOOP5_ASSERT(I, J, K, L, M, X): print args if '!X'
//  BSLS_BSLTESTUTIL_LOOP6_ASSERT(I, J, K, L, M, N, X): print args if '!X'
//  BSLS_BSLTESTUTIL_Q(X): quote identifier literally
//  BSLS_BSLTESTUTIL_P(X): print identifier and value
//  BSLS_BSLTESTUTIL_P_(X): print identifer and value without '\n'
//  BSLS_BSLTESTUTIL_L_: current line number
//  BSLS_BSLTESTUTIL_T_: print tab without '\n'
//
//@AUTHOR: Alisdair Meredith (ameredit)
//
//@DESCRIPTION: This component provides the standard printing macros used in
// BDE-style test drivers ('ASSERT', 'LOOP_ASSERT', 'ASSERTV', 'P', 'Q', 'L',
// and 'T') for components in the 'bsl' package group.
//
// Many components in the 'bsl' package group reside below the standard
// library; therefore, hierarchical design dictates that the test driver for
// these components shall not use 'iostream' (which is part of the standard
// library), and instead they shall only rely on the 'printf' function to print
// objects' values.  Using 'printf' over 'iostream' has the following
// disadvantages:
//
//: o The 'printf' function requires a format string to specify the way to
//:   print an object; so, unlike 'iostream', printing different types of
//:   objects using 'printf' requires different synatxes due to the need for
//:   different format strings.
//:
//: o While the format strings for built-in types can be included as part of
//:   the standard boiler plate code of the test driver, printing a
//:   user-defined type often requires additional code that is not part of the
//:   standard boilerplate.
//
// This component provides solutions to the these issues by (1) encapsulating
// all the standard printing macros in a single place and (2) providing a way
// to extend the supplied macros to support user-defined types.
//
// The macros in this component use a class method template,
// 'BslTestUtil::callDebugprint', to print the value of an object of the
// parameterized type, along with an optional leading string and an optional
// trailing string, to the console.  The value of the object of the
// parameterized type will be printed using a free function named 'debugprint'.
//
// The macros defined in this component natively support built-in type through
// the 'debugprint' function overloads for these types defined in this
// component.  The macros can be extended support additional user-defined types
// by defining function overloads for 'debugprint' that takes a single
// parameter of each user-defined type, in the same namespace in which the
// user-defined type is defined.  See the second usage example for more
// details.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Writing a test driver
/// - - - - - - - - - - - - - - - -
// First, we write a component to test, which provides a utility class:
//..
//  struct xyza_BslExampleUtil {
//      // This utility class provides sample functionality to demonstrate how
//      // a test driver might be written validating its only method.
//
//      static int fortyTwo();
//          // Return the integer value '42'.
//  };
//
//  inline
//  int xyza_BslExampleUtil::fortyTwo()
//  {
//      return 42;
//  }
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
// Finally, we write the test case for the 'static' 'fortyTwo' method, using
// the (standard) abbreviated macro names we have just defined.
//..
//  case 2: {
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
//    const int value = bsls::BslTestUtil_FortyTwo::value();
//    if (verbose) P(value);
//    LOOP_ASSERT(value, 42 == value);
//  } break;
//..
///Example 2: Adding Support For A New User-Defined Type
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we define a new user-defined type, 'MyType':
//..
//  namespace MyNamespace {
//
//  class MyType {
//      // This empty class provides a type intended to show how the macros in
//      // 'bsls_bsltestutil' can be extended to support a new user-defined
//      // type.
//  };
//..
// Then, we define a function 'debugprint' that prints the value of an 'MyType'
// object to standard out in the same namespace in which 'MyType' is defined
// (in this case, we will simply print a string literal for simplicity):
//..
//  void debugprint(const MyType& v)
//  {
//      printf("MyType Value");
//  }
//
//  }  // close 'MyNamespace'
//..
// Now, we create a 'MyType' object and call the 'BSLS_BSLTESTUTIL_P'
// specifying the object as the argument:
//..
//  MyType obj;
//  BSLS_BSLTESTUTIL_P(obj);
//..
// Finally, we observe the console output:
//..
//  obj = MyType Value
//..
                       // =================
                       // Macro Definitions
                       // =================

#define BSLS_BSLTESTUTIL_ASSERT(X)                                            \
    { aSsErT(!(X), #X, __LINE__); }

#define BSLS_BSLTESTUTIL_LOOP0_ASSERT                                         \
    BSLS_BSLTESTUTIL_ASSERT

#define BSLS_BSLTESTUTIL_LOOP_ASSERT(I,X) {                                   \
    if (!(X)) { bsls::BslTestUtil::callDebugprint(I, #I ": ", ",\t");         \
                aSsErT(!(X), #X, __LINE__); } }

#define BSLS_BSLTESTUTIL_LOOP1_ASSERT                                         \
    BSLS_BSLTESTUTIL_LOOP_ASSERT

#define BSLS_BSLTESTUTIL_LOOP2_ASSERT(I,J,X) {                                \
    if (!(X)) { bsls::BslTestUtil::callDebugprint(I, #I ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(J, #J ": ", ",\t");         \
                aSsErT(!(X), #X, __LINE__); } }

#define BSLS_BSLTESTUTIL_LOOP3_ASSERT(I,J,K,X) {                              \
    if (!(X)) { bsls::BslTestUtil::callDebugprint(I, #I ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(J, #J ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(K, #K ": ", ",\t");         \
                aSsErT(!(X), #X, __LINE__); } }

#define BSLS_BSLTESTUTIL_LOOP4_ASSERT(I,J,K,L,X) {                            \
    if (!(X)) { bsls::BslTestUtil::callDebugprint(I, #I ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(J, #J ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(K, #K ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(L, #L ": ", ",\t");         \
                aSsErT(!(X), #X, __LINE__); } }

#define BSLS_BSLTESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X) {                          \
    if (!(X)) { bsls::BslTestUtil::callDebugprint(I, #I ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(J, #J ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(K, #K ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(L, #L ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(M, #M ": ", ",\t");         \
                aSsErT(!(X), #X, __LINE__); } }

#define BSLS_BSLTESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X) {                        \
    if (!(X)) { bsls::BslTestUtil::callDebugprint(I, #I ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(J, #J ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(K, #K ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(L, #L ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(M, #M ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(N, #N ": ", ",\t");         \
                aSsErT(!(X), #X, __LINE__); } }

// The 'BSLS_BSLTESTUTIL_EXPAND' macro is required to workaround a
// pre-proccessor issue on windows that prevents __VA_ARGS__ to be expanded in
// the definition of 'BSLS_BSLTESTUTIL_NUM_ARGS'
#define BSLS_BSLTESTUTIL_EXPAND(X)                                            \
    X

#define BSLS_BSLTESTUTIL_NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)        \
    N

#define BSLS_BSLTESTUTIL_NUM_ARGS(...)                                        \
    BSLS_BSLTESTUTIL_EXPAND(BSLS_BSLTESTUTIL_NUM_ARGS_IMPL(                   \
                                            __VA_ARGS__, 5, 4, 3, 2, 1, 0, ""))

#define BSLS_BSLTESTUTIL_LOOPN_ASSERT_IMPL(N, ...)                            \
    BSLS_BSLTESTUTIL_EXPAND(BSLS_BSLTESTUTIL_LOOP ## N ## _ASSERT(__VA_ARGS__))

#define BSLS_BSLTESTUTIL_LOOPN_ASSERT(N, ...)                                 \
    BSLS_BSLTESTUTIL_LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define BSLS_BSLTESTUTIL_ASSERTV(...)                                         \
    BSLS_BSLTESTUTIL_LOOPN_ASSERT(                                            \
                           BSLS_BSLTESTUTIL_NUM_ARGS(__VA_ARGS__), __VA_ARGS__)


#define BSLS_BSLTESTUTIL_Q(X)                                                 \
                       bsls::BslTestUtil::printStringNoFlush("<| " #X " |>\n");
    // Quote identifier literally.

#define BSLS_BSLTESTUTIL_P(X)                                                 \
                          bsls::BslTestUtil::callDebugprint(X, #X " = ", "\n");
    // Print identifier and its value.

#define BSLS_BSLTESTUTIL_P_(X)                                                \
                          bsls::BslTestUtil::callDebugprint(X, #X " = ", ", ");
    // P(X) without '\n'.

#define BSLS_BSLTESTUTIL_L_ __LINE__
    // current Line number

#define BSLS_BSLTESTUTIL_T_ bsls::BslTestUtil::printTab();
    // Print a tab (w/o newline).

namespace BloombergLP {
namespace bsls {

                   // ==================
                   // struct BslTestUtil
                   // ==================


struct BslTestUtil {
    // This class provides a namespace for utilities that are useful when
    // writing a test driver that is not permitted to use the standard C++
    // iostream facilities, which is typical of test drivers in the 'bsl'
    // package group.

    // CLASS METHODS
    static void flush();
        // Write any unwritten text in the output buffer to 'stdout'.

    static void printStringNoFlush(const char *s);
        // Print to the console the specified string, 's'.  Note that the
        // underlying stream is *not* flushed.

    static void printTab();
        // Print to the console a tab character, and then 'flush' the
        // underlying stream to ensure the text is written.

    template <class TYPE>
    static void callDebugprint(const TYPE& object,
                               const char *leadingString = 0,
                               const char *trailingString = 0);
        // Print the value of the specified 'object' of the parameterized
        // 'TYPE' to the console.  Optionally specify a 'leadingString', which
        // will be printed before 'object', and a 'trailingString', which will
        // be printed after 'object'.  If 'leadingString' is 0, then nothing
        // will be printed before 'object'.  If 'trailingString' is 0, then
        // nothing will be printed after 'object'.
};

// FREE FUNCTIONS
void debugprint(bool v);
    // Print to the console the string "true" if the specified 'v' is true,
    // and the string "false" otherwise.

void debugprint(char v);
    // Print to the console the specified character, 'v', enclosed by
    // single-quote characters (').

void debugprint(signed char v);
void debugprint(unsigned char v);
void debugprint(short v);
void debugprint(unsigned short v);
void debugprint(int v);
void debugprint(unsigned int v);
void debugprint(long v);
void debugprint(unsigned long v);
void debugprint(long long v);
void debugprint(unsigned long long v);
    // Print to the console the specified integer value, 'v', formatted as
    // a string.

void debugprint(float v);
void debugprint(double v);
void debugprint(long double v);
    // Print to the console the specified value, 'v', formatted as a string
    // enclosed by single-quote characters (').

void debugprint(const char *v);
void debugprint(char *v);
    // Print to the console the specified string, 'v', enclosed by quote
    // characters (").

void debugprint(const void *v);
    // Print to the console the specified memory address, 'v', formatted as
    // a hexadecimal integer.

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                   // ------------------
                   // struct BslTestUtil
                   // ------------------

// CLASS METHODS
template <class TYPE>
void BslTestUtil::callDebugprint(const TYPE& obj,
                                 const char *leadingString,
                                 const char *trailingString)
{
    if (leadingString) {
        BloombergLP::bsls::BslTestUtil::printStringNoFlush(leadingString);
    }

    debugprint(obj);

    if (trailingString) {
        BloombergLP::bsls::BslTestUtil::printStringNoFlush(trailingString);
    }
    flush();
}

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
