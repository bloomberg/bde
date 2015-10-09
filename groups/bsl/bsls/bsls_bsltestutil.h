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
//  BSLS_BSLTESTUTIL_LOOP_ASSERT( I, X)               : print args if '!X'
//  BSLS_BSLTESTUTIL_LOOP2_ASSERT(I, J, X)            : print args if '!X'
//  BSLS_BSLTESTUTIL_LOOP3_ASSERT(I, J, K, X)         : print args if '!X'
//  BSLS_BSLTESTUTIL_LOOP4_ASSERT(I, J, K, L, X)      : print args if '!X'
//  BSLS_BSLTESTUTIL_LOOP5_ASSERT(I, J, K, L, M, X)   : print args if '!X'
//  BSLS_BSLTESTUTIL_LOOP6_ASSERT(I, J, K, L, M, N, X): print args if '!X'
//
//  BSLS_BSLTESTUTIL_Q(X) : quote identifier literally
//  BSLS_BSLTESTUTIL_P(X) : print identifier and value
//  BSLS_BSLTESTUTIL_P_(X): print identifier and value without '\n'
//  BSLS_BSLTESTUTIL_L_   : current line number
//  BSLS_BSLTESTUTIL_T_   : print tab without '\n'
//
//  BSLS_BSLTESTUTIL_FORMAT_ZU : 'printf' format for 'size_t'
//  BSLS_BSLTESTUTIL_FORMAT_TD : 'printf' format for 'ptrdiff_t'
//  BSLS_BSLTESTUTIL_FORMAT_I64: 'printf' format for unsigned 64-bit integers
//  BSLS_BSLTESTUTIL_FORMAT_U64: 'printf' format for signed 64-bit integers
//
//@DESCRIPTION: This component provides standard facilities for for components
// in the 'bsl' package group to produce test driver output, including the
// standard printing macros used in BDE-style test drivers ('ASSERT',
// 'LOOP_ASSERT', 'ASSERTV', 'P', 'Q', 'L', and 'T'), and a suite of
// cross-platform format strings for printing C++ or BDE-specific types with
// 'printf'.
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
//:   objects using 'printf' requires different syntaxes due to the need for
//:   different format strings.
//:
//: o While the format strings for built-in types can be included as part of
//:   the standard boiler plate code of the test driver, printing a
//:   user-defined type often requires additional code that is not part of the
//:   standard boilerplate.
//
// This component provides solutions to the these issues by (1) encapsulating
// all the standard printing macros in a single place, (2) providing a way to
// extend the supplied macros to support user-defined types, and (3) providing
// macros that resolve the correct 'printf' format strings for types that do
// not have standard, cross-platform format strings of their own.
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
///Example 1: Writing a Test Driver
/// - - - - - - - - - - - - - - - -
// First, we write a component to test, which provides a utility class:
//..
//  namespace bslabc {
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
//  }  // close package namespace
//..
// Then, we can write a test driver for this component.  We start by providing
// the standard BDE assert test macro:
//..
//  // ========================================================================
//  //                       STANDARD BDE ASSERT TEST MACRO
//  // ------------------------------------------------------------------------
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
// Next, we define the standard print and 'LOOP_ASSERT' macros, as aliases to
// the macros defined by this component:
//..
//  // ========================================================================
//  //                       STANDARD BDE TEST DRIVER MACROS
//  // ------------------------------------------------------------------------
//  #define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
//  #define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
//  #define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
//  #define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
//  #define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
//  #define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
//
//  #define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
//  #define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
//  #define P_  BSLS_BSLTESTUTIL_P_  // 'P(X)' without '\n'.
//  #define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
//  #define L_  BSLS_BSLTESTUTIL_L_  // current Line number
//..
// Now, using the (standard) abbreviated macro names we have just defined, we
// write a test function for the 'static' 'fortyTwo' method, to be called from
// a test case in a test driver.
//..
//  void testFortyTwo(bool verbose)
//  {
//      const int value = bslabc::BslExampleUtil::fortyTwo();
//      if (verbose) P(value);
//      LOOP_ASSERT(value, 42 == value);
//  }
//..
// Finally, when 'testFortyTwo' is called from a test case in verbose mode we
// observe the console output:
//..
//  value = 42
//..
//
///Example 2: Adding Support For A New User-Defined Type
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we define a new user-defined type, 'MyType':
//..
//  namespace xyza {
//
//  class MyType {
//      // This elided class provides a type intended to show how the macros in
//      // 'bsls_bsltestutil' can be extended to support a new user-defined
//      // type.
//
//    private:
//      // DATA
//      int d_value;  // the value of MyType
//
//      // ...
//
//    public:
//      // CREATORS
//
//      // ...
//
//      explicit MyType(int value);
//          // Create a 'MyType' object with 'd_value' set to the specified
//          // 'value'.
//
//      // ACCESSORS
//
//      // ...
//
//      int value() const;
//          // Return the value of 'd_value'.
//
//      // ...
//  };
//
//  // ...
//
//  inline
//  MyType::MyType(int value)
//  : d_value(value)
//  {
//  }
//
//  // ...
//
//  inline
//  int MyType::value() const
//  {
//      return d_value;
//  }
//..
// Then, in the same namespace in which 'MyType' is defined, we define a
// function 'debugprint' that prints the value of a 'MyType' object to the
// console.  (In this case, we will simply print a string literal for
// simplicity):
//..
//  void debugprint(const MyType& obj)
//  {
//      printf("MyType<%d>", obj.value());
//  }
//
//  }  // close namespace xyza
//..
// Now, using the (standard) abbreviated macro names previously defined, we
// write a test function for the 'MyType' constructor, to be called from a test
// case in a test driver.
//..
//  void testMyTypeSetValue(bool verbose) {
//      xyza::MyType obj(9);
//      if (verbose) P(obj);
//      LOOP_ASSERT(obj.value(), obj.value() == 9);
//  }
//..
// Finally, when 'testMyTypeSetValue' is called from a test case in verbose
// mode we observe the console output:
//..
//  obj = MyType<9>
//..
//
///Example 3: Printing Unusual Types with 'printf'
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are writing a test driver that needs to print out the contents of
// a complex data structure in 'veryVeryVerbose' mode.  The complex data
// structure contains, among other values, an array of block sizes, expressed
// as 'size_t'.  It would be very cumbersome, and visually confusing, to print
// each member of the array with either the 'P_' or 'Q_' standard output
// macros, so we elect to print out the array as a single string, following the
// pattern of '[ A, B, C, D, E, ... ]'.  This could be easily accomplished with
// multiple calls to 'printf', except that 'printf' has no cross-platform
// standard formatting string for 'size_t'.  We can use the
// 'BSLS_BSLTESTUTIL_FORMAT_ZU' macro to resolve the appropriate format string
// for us on each platform.
//
// First, we write a component to test, which provides an a utility that
// operates on a list of memory blocks.  Each block is a structure containing a
// base address, a block size, and a pointer to the next block in the list.
//..
//  namespace xyza {
//  struct Block {
//      // DATA
//      char   *d_address;
//      size_t  d_size;
//      Block  *d_next;
//
//      // ...
//  };
//
//  class BlockList {
//      // ...
//
//      // DATA
//      Block *d_head;
//
//      // ...
//
//    public:
//      // CREATORS
//      BlockList();
//      ~BlockList();
//
//      // MANIPULATORS
//
//      Block *begin();
//      Block *end();
//
//      void addBlock(size_t size);
//
//      // ...
//
//      // ACCESSORS
//      int length();
//
//      // ...
//  };
//
//  }  // close namespace xyza
//..
// Then, we write a test driver for this component.
//..
//  // ...
//
//  // ========================================================================
//  //                       STANDARD BDE TEST DRIVER MACROS
//  // ------------------------------------------------------------------------
//
//  // ...
//..
// Here, after defining the standard BDE test macros, we define a macro, 'ZU'
// for the platform-specific 'printf' format string for 'size_t':
//..
//  // ========================================================================
//  //                          PRINTF FORMAT MACROS
//  // ------------------------------------------------------------------------
//  #define ZU BSLS_BSLTESTUTIL_FORMAT_ZU
//..
// Note that, we could use 'BSLS_BSLTESTUTIL_FORMAT_ZU' as is, but it is more
// convenient to define 'ZU' locally as an abbreviation.
//
// Next, we write the test apparatus for the test driver, which includes a
// support function that prints the list of blocks in a 'BlockList' in a
// visually succinct form:
//..
//  void printBlockList(xyza::BlockList &list)
//  {
//      xyza::Block *blockPtr = list.begin();
//
//      printf("{\n");
//      while (blockPtr != list.end()) {
//..
// Here, we use 'ZU' as the format specifier for the 'size_t' in the 'printf'
// invocation. 'ZU' is the appropriate format specifier for 'size_t' on each
// supported platform.
//..
//          printf("\t{ address: %p,\tsize: " ZU " }",
//                 blockPtr->d_address,
//                 blockPtr->d_size);
//          blockPtr = blockPtr->d_next;
//
//          if (blockPtr) {
//              printf(",\n");
//          } else {
//              printf("\n");
//          }
//      }
//      printf("}\n");
//  }
//..
// Note that because we are looping through a number of blocks, formatting the
// output directly with 'printf' produces more readable output than we would
// get from callling the standard output macros.
//
// Calling 'printf' directly will yield output similar to:
//..
// {
//     { address: 0x012345600,    size: 32 },
//     ...
// }
//..
// while the standard output macros would have produced:
//..
// {
//     { blockPtr->d_address = 0x012345600,    blockPtr->d_size: 32 },
//     ...
// }
//..
// Now, we write a test function for one of our test cases, which provides a
// detailed trace of 'BlockList' contents:
//..
//  void testBlockListConstruction(bool veryVeryVerbose)
//  {
//      // ...
//
//      {
//          xyza::BlockList bl;
//
//          bl.addBlock(42);
//          bl.addBlock(19);
//          bl.addBlock(1024);
//
//          if (veryVeryVerbose) {
//              printBlockList(bl);
//          }
//
//          ASSERT(3 == bl.length());
//
//          // ...
//      }
//
//      // ...
//  }
//..
// Finally, when 'testBlockListConstruction' is called from a test case in
// 'veryVeryVerbose' mode, we observe console output similar to:
//..
//  {
//      { address: 0x012345600,    size: 42 },
//      { address: 0x012345610,    size: 19 },
//      { address: 0x012345620,    size: 1024 }
//  }
//..

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)
#   ifndef INCLUDED_STDDEF_H
#   include <stddef.h>
#   define INCLUDED_STDDEF_H
#   endif
#else
#   ifndef INCLUDED_STDINT_H
#   include <stdint.h>
#   define INCLUDED_STDINT_H
#   endif
#endif

                       // =================
                       // Macro Definitions
                       // =================

#define BSLS_BSLTESTUTIL_ASSERT(X)                                            \
    { aSsErT(!(X), #X, __LINE__); }

#define BSLS_BSLTESTUTIL_LOOP0_ASSERT                                         \
    BSLS_BSLTESTUTIL_ASSERT

#define BSLS_BSLTESTUTIL_LOOP_ASSERT(I,X) {                                   \
    if (!(X)) { bsls::BslTestUtil::callDebugprint(I, #I ": ", "\n");          \
                aSsErT(true, #X, __LINE__); } }

#define BSLS_BSLTESTUTIL_LOOP1_ASSERT                                         \
    BSLS_BSLTESTUTIL_LOOP_ASSERT

#define BSLS_BSLTESTUTIL_LOOP2_ASSERT(I,J,X) {                                \
    if (!(X)) { bsls::BslTestUtil::callDebugprint(I, #I ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(J, #J ": ", "\n");          \
                aSsErT(true, #X, __LINE__); } }

#define BSLS_BSLTESTUTIL_LOOP3_ASSERT(I,J,K,X) {                              \
    if (!(X)) { bsls::BslTestUtil::callDebugprint(I, #I ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(J, #J ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(K, #K ": ", "\n");          \
                aSsErT(true, #X, __LINE__); } }

#define BSLS_BSLTESTUTIL_LOOP4_ASSERT(I,J,K,L,X) {                            \
    if (!(X)) { bsls::BslTestUtil::callDebugprint(I, #I ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(J, #J ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(K, #K ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(L, #L ": ", "\n");          \
                aSsErT(true, #X, __LINE__); } }

#define BSLS_BSLTESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X) {                          \
    if (!(X)) { bsls::BslTestUtil::callDebugprint(I, #I ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(J, #J ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(K, #K ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(L, #L ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(M, #M ": ", "\n");          \
                aSsErT(true, #X, __LINE__); } }

#define BSLS_BSLTESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X) {                        \
    if (!(X)) { bsls::BslTestUtil::callDebugprint(I, #I ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(J, #J ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(K, #K ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(L, #L ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(M, #M ": ", "\t");          \
                bsls::BslTestUtil::callDebugprint(N, #N ": ", "\n");          \
                aSsErT(true, #X, __LINE__); } }

// The 'BSLS_BSLTESTUTIL_EXPAND' macro is required to workaround a
// pre-processor issue on windows that prevents __VA_ARGS__ to be expanded in
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

// STANDARD TEST DRIVER OUTPUT MACROS
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

// PRINTF FORMAT MACROS
#if defined(BSLS_PLATFORM_CMP_MSVC)
#  define BSLS_BSLTESTUTIL_FORMAT_ZU "%Iu"
#else
#  define BSLS_BSLTESTUTIL_FORMAT_ZU "%zu"
#endif
    // Provide a platform-independent way to specify a 'size_t' format for
    // printf

#if defined(BSLS_PLATFORM_CMP_MSVC)
#  define BSLS_BSLTESTUTIL_FORMAT_TD "%Id"
#else
#  define BSLS_BSLTESTUTIL_FORMAT_TD "%td"
#endif
    // Provide a platform-independent way to specify a 'ptrdiff_t' format for
    // printf

#if defined(BSLS_PLATFORM_CMP_MSVC)
#  define BSLS_BSLTESTUTIL_FORMAT_I64 "%I64d"
#else
#  define BSLS_BSLTESTUTIL_FORMAT_I64 "%lld"
#endif
    // Provide a platform-independent way to specify a signed 64-bit integer
    // format for printf

#if defined(BSLS_PLATFORM_CMP_MSVC)
#  define BSLS_BSLTESTUTIL_FORMAT_U64 "%I64u"
#else
#  define BSLS_BSLTESTUTIL_FORMAT_U64 "%llu"
#endif
    // Provide a platform-independent way to specify an unsigned 64-bit integer
    // format for printf

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
void debugprint(const volatile char *v);
void debugprint(volatile char *v);
    // Print to the console the specified string, 'v', enclosed by quote
    // characters ("), unless 'v' is null, in which case print '(null)'
    // (without quotes of any kind).

void debugprint(void *v);
void debugprint(volatile void *v);
void debugprint(const void *v);
void debugprint(const volatile void *v);
    // Print to the console the specified memory address, 'v', formatted as
    // a hexadecimal integer.

template <class RESULT>
void debugprint(RESULT (*v)());
    // Print to the console the specified function pointer, 'v', formatted as a
    // hexadecimal integer. On some platforms (notably Windows), a function
    // pointer is treated differently from an object pointer, and the compiler
    // will not be able to determine which 'void *' overload of 'debugprint'
    // should be used for a function pointer. Therefore an overload of
    // 'debugprint' is provided specifically for function pointers. Because the
    // type signature of a function pointer varies with its return type as well
    // as with its argument list, a template function is used, to provide
    // matches for all return types.

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

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

// FREE FUNCTIONS

template <class RESULT>
void bsls::debugprint(RESULT (*v)())
{
    uintptr_t address = reinterpret_cast<uintptr_t>(v);
    debugprint(reinterpret_cast<void *>(address));
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
