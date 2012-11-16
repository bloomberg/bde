// bslmf_functionpointertraits.t.cpp                                  -*-C++-*-

#include <bslmf_functionpointertraits.h>

#include <bslmf_typelist.h>  // for testing only
#include <bslmf_issame.h>    // for testing only

#include <cstdlib>           // atoi()
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//  This test driver verifies each of the 21 typelist template classes provided
//  by bslmf_typelist.  Each template is instantiated with the appropriate
//  number of distinct types.  Each type will be test to be sure that the type
//  defined by it's corresponding Type<N> typedef and TypeOf<N> typedef are
//  correct.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [1 ] Breathing test
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
   if (c) {
       cout << "Error " << __FILE__ << "(" << i << "): " << s
            << "    (failed)" << endl;
       if (0 <= testStatus && testStatus <= 100) ++testStatus;
   }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define ASSERT_SAME(T1,T2) ASSERT((1 == bsl::is_same<T1,T2>::value))

//=============================================================================
//                  STANDARD BDEX EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
#define BEGIN_BSLX_EXCEPTION_TEST                                           \
{                                                                           \
    {                                                                       \
        static int firstTime = 1;                                           \
        if (veryVerbose && firstTime)  cout <<                              \
            "### BDEX EXCEPTION TEST -- (ENABLED) --" << endl;              \
        firstTime = 0;                                                      \
    }                                                                       \
    if (veryVeryVerbose) cout <<                                            \
        "### Begin bdex exception test." << endl;                           \
    int bdexExceptionCounter = 0;                                           \
    static int bdexExceptionLimit = 100;                                    \
    testInStream.setInputLimit(bdexExceptionCounter);                       \
    do {                                                                    \
        try {

#define END_BSLX_EXCEPTION_TEST                                             \
        } catch (bslx_TestInStreamException& e) {                           \
            if (veryVerbose && bdexExceptionLimit || veryVeryVerbose)       \
            {                                                               \
                --bdexExceptionLimit;                                       \
                cout << "(" <<                                              \
                bdexExceptionCounter << ')';                                \
                if (veryVeryVerbose) { cout << " BSLX_EXCEPTION: "          \
                    << "input limit = " << bdexExceptionCounter << ", "     \
                    << "last data type = " << e.dataType();                 \
                }                                                           \
                else if (0 == bdexExceptionLimit) {                         \
                    cout << " [ Note: 'bdexExceptionLimit' reached. ]";     \
                }                                                           \
                cout << endl;                                               \
            }                                                               \
            testInStream.setInputLimit(++bdexExceptionCounter);             \
            continue;                                                       \
        }                                                                   \
        testInStream.setInputLimit(-1);                                     \
        break;                                                              \
    } while (1);                                                            \
    if (veryVeryVerbose) cout <<                                            \
        "### End bdex exception test." << endl;                             \
}
#else
#define BEGIN_BSLX_EXCEPTION_TEST                                           \
{                                                                           \
    static int firstTime = 1;                                               \
    if (verbose && firstTime) { cout <<                                     \
        "### BDEX EXCEPTION TEST -- (NOT ENABLED) --" << endl;              \
        firstTime = 0;                                                      \
    }                                                                       \
}
#define END_BSLX_EXCEPTION_TEST
#endif

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
struct T0  {};
struct T1  {};
struct T2  {};
struct T3  {};
struct T4  {};
struct T5  {};
struct T6  {};
struct T7  {};
struct T8  {};
struct T9  {};
struct T10 {};
struct T11 {};
struct T12 {};
struct T13 {};
struct T14 {};

typedef T0 (*TestFunc0)();
typedef T1 (*TestFunc1)(T1);
typedef T2 (*TestFunc2)(T1, T2);
typedef T3 (*TestFunc3)(T1, T2, T3); //
typedef T4 (*TestFunc4)(T1, T2, T3, T4);
typedef T5 (*TestFunc5)(T1, T2, T3,T4, T5);
typedef T6 (*TestFunc6)(T1, T2, T3, T4, T5, T6);
typedef T7 (*TestFunc7)(T1, T2, T3, T4, T5, T6, T7);
typedef T8 (*TestFunc8)(T1, T2, T3, T4, T5, T6, T7, T8);
typedef T9 (*TestFunc9)(T1, T2, T3, T4, T5, T6, T7, T8, T9);
typedef T10 (*TestFunc10)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10);
typedef T11 (*TestFunc11)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11);
typedef T12 (*TestFunc12)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12);
typedef T13 (*TestFunc13)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12,
                          T13);
typedef T14 (*TestFunc14)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12,
                          T13, T14);

typedef void (*TestVoidFunc0)();
typedef void (*TestVoidFunc1)(T1);
typedef void (*TestVoidFunc2)(T1, T2);
typedef void (*TestVoidFunc3)(T1, T2, T3);
typedef void (*TestVoidFunc4)(T1, T2, T3, T4);
typedef void (*TestVoidFunc5)(T1, T2, T3,T4, T5);
typedef void (*TestVoidFunc6)(T1, T2, T3, T4, T5, T6);
typedef void (*TestVoidFunc7)(T1, T2, T3, T4, T5, T6, T7);
typedef void (*TestVoidFunc8)(T1, T2, T3, T4, T5, T6, T7, T8);
typedef void (*TestVoidFunc9)(T1, T2, T3, T4, T5, T6, T7, T8, T9);
typedef void (*TestVoidFunc10)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10);
typedef void (*TestVoidFunc11)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11);
typedef void (*TestVoidFunc12)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
                               T12);
typedef void (*TestVoidFunc13)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
                               T12, T13);
typedef void (*TestVoidFunc14)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
                               T12, T13, T14);

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

// Define the following function types:
//..
    typedef int  (*IntFunctionIntIntPtr)(int, int);
    typedef void (*VoidFunc0)();
//..
// The following program should compile and run without errors:
//..
    void usageExample()
    {
        ASSERT(0 == bslmf::IsFunctionPointer<int>::value);
        ASSERT(0 == bslmf::IsFunctionPointer<int>::value);

        ASSERT(1 == bslmf::IsFunctionPointer<IntFunctionIntIntPtr>::value);
        typedef bslmf::FunctionPointerTraits<IntFunctionIntIntPtr>::ResultType
            ResultType1;
        ASSERT(1 == (bsl::is_same<ResultType1, int>::value));

        ASSERT(1 == (bslmf::IsFunctionPointer<VoidFunc0>::value));
        typedef bslmf::FunctionPointerTraits<VoidFunc0>::ResultType
            ResultType0;
        typedef bslmf::FunctionPointerTraits<VoidFunc0>::ArgumentList
            ArgList0;
        ASSERT(1 == (bsl::is_same<ResultType0, void>::value));
        ASSERT(1 == (bsl::is_same<ArgList0, bslmf::TypeList0>::value));
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    (void) verbose;          // eliminate unused variable warning
    (void) veryVerbose;      // eliminate unused variable warning
    (void) veryVeryVerbose;  // eliminate unused variable warning

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) {
      case 0:  // Zero is always the leading case.
      case 3: {
      } break;
      case 2: {
          usageExample();
      } break;
      case 1: {
          {
              typedef bslmf::FunctionPointerTraits<TestFunc0> X;
              typedef bslmf::TypeList0 ListType;
              ASSERT(1==X::IS_FUNCTION_POINTER);
              ASSERT(1==bslmf::IsFunctionPointer<TestFunc0>::value);
              ASSERT_SAME(T0, X::ResultType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::FunctionPointerTraits<TestFunc1> X;
              typedef bslmf::TypeList1<T1> ListType;
              ASSERT(1==X::IS_FUNCTION_POINTER);
              ASSERT(1==bslmf::IsFunctionPointer<TestFunc1>::value);
              ASSERT_SAME(T1, X::ResultType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::FunctionPointerTraits<TestFunc2> X;
              typedef bslmf::TypeList2<T1,T2> ListType;
              ASSERT(1==bslmf::IsFunctionPointer<TestFunc2>::value);
              ASSERT(1==X::IS_FUNCTION_POINTER);
              ASSERT_SAME(T2, X::ResultType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::FunctionPointerTraits<TestFunc3> X;
              typedef bslmf::TypeList3<T1,T2,T3> ListType;
              ASSERT(1==X::IS_FUNCTION_POINTER);
              ASSERT(1==bslmf::IsFunctionPointer<TestFunc3>::value);
              ASSERT_SAME(T3, X::ResultType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::FunctionPointerTraits<TestFunc4> X;
              typedef bslmf::TypeList4<T1,T2,T3,T4> ListType;
              ASSERT(1==X::IS_FUNCTION_POINTER);
              ASSERT(1==bslmf::IsFunctionPointer<TestFunc4>::value);
              ASSERT_SAME(T4, X::ResultType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

          {
              typedef bslmf::FunctionPointerTraits<TestFunc5> X;
              typedef bslmf::TypeList5<T1,T2,T3,T4,T5> ListType;
              ASSERT(1==X::IS_FUNCTION_POINTER);
              ASSERT(1==bslmf::IsFunctionPointer<TestFunc5>::value);
              ASSERT_SAME(T5, X::ResultType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::FunctionPointerTraits<TestFunc6> X;
              typedef bslmf::TypeList6<T1,T2,T3,T4,T5,T6> ListType;
              ASSERT(1==X::IS_FUNCTION_POINTER);
              ASSERT(1==bslmf::IsFunctionPointer<TestFunc6>::value);
              ASSERT_SAME(T6, X::ResultType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::FunctionPointerTraits<TestFunc7> X;
              typedef bslmf::TypeList7<T1,T2,T3,T4,T5,T6,T7> ListType;
              ASSERT(1==X::IS_FUNCTION_POINTER);
              ASSERT(1==bslmf::IsFunctionPointer<TestFunc7>::value);
              ASSERT_SAME(T7, X::ResultType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::FunctionPointerTraits<TestFunc8> X;
              typedef bslmf::TypeList8<T1,T2,T3,T4,T5,T6,T7,T8> ListType;
              ASSERT(1==X::IS_FUNCTION_POINTER);
              ASSERT(1==bslmf::IsFunctionPointer<TestFunc8>::value);
              ASSERT_SAME(T8, X::ResultType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::FunctionPointerTraits<TestFunc9> X;
              typedef bslmf::TypeList9<T1,T2,T3,T4,T5,T6,T7,T8,T9> ListType;
              ASSERT(1==X::IS_FUNCTION_POINTER);
              ASSERT(1==bslmf::IsFunctionPointer<TestFunc9>::value);
              ASSERT_SAME(T9, X::ResultType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::FunctionPointerTraits<TestFunc10> X;
              typedef bslmf::TypeList10<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>
                  ListType;
              ASSERT(1==X::IS_FUNCTION_POINTER);
              ASSERT(1==bslmf::IsFunctionPointer<TestFunc10>::value);
              ASSERT_SAME(T10, X::ResultType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::FunctionPointerTraits<TestFunc11> X;
              typedef bslmf::TypeList11<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>
                  ListType;
              ASSERT(1==X::IS_FUNCTION_POINTER);
              ASSERT(1==bslmf::IsFunctionPointer<TestFunc11>::value);
              ASSERT_SAME(T11, X::ResultType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::FunctionPointerTraits<TestFunc12> X;
              typedef bslmf::TypeList12<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>
                  ListType;
              ASSERT(1==X::IS_FUNCTION_POINTER);
              ASSERT(1==bslmf::IsFunctionPointer<TestFunc12>::value);
              ASSERT_SAME(T12, X::ResultType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

          {
              typedef bslmf::FunctionPointerTraits<TestFunc13> X;
              typedef bslmf::TypeList13<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13>  ListType;
              ASSERT(1==X::IS_FUNCTION_POINTER);
              ASSERT(1==bslmf::IsFunctionPointer<TestFunc13>::value);
              ASSERT_SAME(T13, X::ResultType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

          {
              typedef bslmf::FunctionPointerTraits<TestFunc14> X;
              typedef bslmf::TypeList14<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13,T14> ListType;
              ASSERT(1==X::IS_FUNCTION_POINTER);
              ASSERT(1==bslmf::IsFunctionPointer<TestFunc14>::value);
              ASSERT_SAME(T14, X::ResultType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }
    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
