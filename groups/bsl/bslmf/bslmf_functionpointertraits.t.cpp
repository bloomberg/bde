// bslmf_functionpointertraits.t.cpp                                  -*-C++-*-

#include <bslmf_functionpointertraits.h>

#include <bslmf_issame.h>    // for testing only
#include <bslmf_typelist.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>           // atoi()

using namespace BloombergLP;

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
// [ 1] BREATHING TEST

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                 COMPONENT-SPECIFIC TEST MACROS
//-----------------------------------------------------------------------------

#define ASSERT_SAME(T1,T2) ASSERT((1 == bsl::is_same<T1,T2>::value))

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
typedef T3 (*TestFunc3)(T1, T2, T3);
typedef T4 (*TestFunc4)(T1, T2, T3, T4);
typedef T5 (*TestFunc5)(T1, T2, T3, T4, T5);
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
typedef void (*TestVoidFunc5)(T1, T2, T3, T4, T5);
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

typedef T0*  (*TestEllipsisFunc0)(...);
typedef T1*  (*TestEllipsisFunc1)(T1...);
typedef T2*  (*TestEllipsisFunc2)(T1, T2...);
typedef T3*  (*TestEllipsisFunc3)(T1, T2, T3...);
typedef T4*  (*TestEllipsisFunc4)(T1, T2, T3, T4...);
typedef T5*  (*TestEllipsisFunc5)(T1, T2, T3, T4, T5...);
typedef T6*  (*TestEllipsisFunc6)(T1, T2, T3, T4, T5, T6...);
typedef T7*  (*TestEllipsisFunc7)(T1, T2, T3, T4, T5, T6, T7...);
typedef T8*  (*TestEllipsisFunc8)(T1, T2, T3, T4, T5, T6, T7, T8...);
typedef T9*  (*TestEllipsisFunc9)(T1, T2, T3, T4, T5, T6, T7, T8, T9...);
typedef T10* (*TestEllipsisFunc10)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10...);
typedef T11* (*TestEllipsisFunc11)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                                   T11...);
typedef T12* (*TestEllipsisFunc12)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                                   T11, T12...);
typedef T13* (*TestEllipsisFunc13)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                                   T11, T12, T13...);
typedef T14* (*TestEllipsisFunc14)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                                   T11, T12, T13, T14...);

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
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;          // eliminate unused variable warning
    (void) veryVeryVerbose;      // eliminate unused variable warning
    (void) veryVeryVeryVerbose;  // eliminate unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
          usageExample();
      } break;
      case 4: {
        {
            typedef TestEllipsisFunc0 const FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList0 ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T0 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestEllipsisFunc1 volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList1<T1> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T1 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestEllipsisFunc2 const volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList2<T1,T2> ListType;
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T2 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestEllipsisFunc3 const FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList3<T1,T2,T3> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T3 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestEllipsisFunc4 volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList4<T1,T2,T3,T4> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T4 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestEllipsisFunc5 const volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList5<T1,T2,T3,T4,T5> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T5 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestEllipsisFunc6 const FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList6<T1,T2,T3,T4,T5,T6> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T6 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestEllipsisFunc7 volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList7<T1,T2,T3,T4,T5,T6,T7> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T7 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestEllipsisFunc8 const volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList8<T1,T2,T3,T4,T5,T6,T7,T8> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T8 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestEllipsisFunc9 const FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList9<T1,T2,T3,T4,T5,T6,T7,T8,T9> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T9 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestEllipsisFunc10 volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList10<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>
                  ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T10 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestEllipsisFunc11 const volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList11<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>
                  ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T11 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestEllipsisFunc12 const FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList12<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>
                  ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T12 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestEllipsisFunc13 volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList13<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13>  ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T13 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestEllipsisFunc14 const volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList14<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13,T14> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T14 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
      } break;
      case 3: {
        {
            typedef TestVoidFunc0 const FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList0 ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestVoidFunc1 volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList1<T1> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestVoidFunc2 const volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList2<T1,T2> ListType;
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestVoidFunc3 const FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList3<T1,T2,T3> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestVoidFunc4 volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList4<T1,T2,T3,T4> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestVoidFunc5 const volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList5<T1,T2,T3,T4,T5> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestVoidFunc6 const FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList6<T1,T2,T3,T4,T5,T6> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestVoidFunc7 volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList7<T1,T2,T3,T4,T5,T6,T7> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestVoidFunc8 const volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList8<T1,T2,T3,T4,T5,T6,T7,T8> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestVoidFunc9 const FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList9<T1,T2,T3,T4,T5,T6,T7,T8,T9> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestVoidFunc10 volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList10<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>
                  ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestVoidFunc11 const volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList11<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>
                  ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestVoidFunc12 const FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList12<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>
                  ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestVoidFunc13 volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList13<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13>  ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestVoidFunc14 const volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList14<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13,T14> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
      } break;
      case 2: {
        {
            typedef TestFunc0 const FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList0 ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T0, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestFunc1 volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList1<T1> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T1, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestFunc2 const volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList2<T1,T2> ListType;
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T2, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestFunc3 const FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList3<T1,T2,T3> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T3, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestFunc4 volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList4<T1,T2,T3,T4> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T4, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestFunc5 const volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList5<T1,T2,T3,T4,T5> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T5, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestFunc6 const FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList6<T1,T2,T3,T4,T5,T6> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T6, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestFunc7 volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList7<T1,T2,T3,T4,T5,T6,T7> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T7, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestFunc8 const volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList8<T1,T2,T3,T4,T5,T6,T7,T8> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T8, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestFunc9 const FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList9<T1,T2,T3,T4,T5,T6,T7,T8,T9> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T9, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestFunc10 volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList10<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>
                  ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T10, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestFunc11 const volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList11<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>
                  ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T11, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestFunc12 const FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList12<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>
                  ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T12, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestFunc13 volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList13<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13>  ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T13, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef TestFunc14 const volatile FuncType;
            typedef bslmf::FunctionPointerTraits<FuncType> X;
            typedef bslmf::TypeList14<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13,T14> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T14, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This test exercises basic functionality but *tests* *nothing*.
        //
        // Concerns:
        //   1. That the functions exist with the documented signatures.
        //   2. That the basic functionality works as documented.
        //
        // Plan:
        //   Exercise each function in turn and devise an elementary test
        //   sequence to ensure that the basic functionality is as documented.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        {
            typedef bslmf::FunctionPointerTraits<TestFunc0> X;
            typedef bslmf::TypeList0 ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<TestFunc0>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T0, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef bslmf::FunctionPointerTraits<TestFunc1> X;
            typedef bslmf::TypeList1<T1> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<TestFunc1>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T1, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef bslmf::FunctionPointerTraits<TestFunc2> X;
            typedef bslmf::TypeList2<T1,T2> ListType;
            ASSERT(1==bslmf::IsFunctionPointer<TestFunc2>::value);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T2, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef bslmf::FunctionPointerTraits<TestFunc3> X;
            typedef bslmf::TypeList3<T1,T2,T3> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<TestFunc3>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T3, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef bslmf::FunctionPointerTraits<TestFunc4> X;
            typedef bslmf::TypeList4<T1,T2,T3,T4> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<TestFunc4>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T4, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef bslmf::FunctionPointerTraits<TestFunc5> X;
            typedef bslmf::TypeList5<T1,T2,T3,T4,T5> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<TestFunc5>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T5, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef bslmf::FunctionPointerTraits<TestFunc6> X;
            typedef bslmf::TypeList6<T1,T2,T3,T4,T5,T6> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<TestFunc6>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T6, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef bslmf::FunctionPointerTraits<TestFunc7> X;
            typedef bslmf::TypeList7<T1,T2,T3,T4,T5,T6,T7> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<TestFunc7>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T7, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef bslmf::FunctionPointerTraits<TestFunc8> X;
            typedef bslmf::TypeList8<T1,T2,T3,T4,T5,T6,T7,T8> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<TestFunc8>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T8, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef bslmf::FunctionPointerTraits<TestFunc9> X;
            typedef bslmf::TypeList9<T1,T2,T3,T4,T5,T6,T7,T8,T9> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<TestFunc9>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T9, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef bslmf::FunctionPointerTraits<TestFunc10> X;
            typedef bslmf::TypeList10<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>
                  ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<TestFunc10>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T10, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef bslmf::FunctionPointerTraits<TestFunc11> X;
            typedef bslmf::TypeList11<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>
                  ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<TestFunc11>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T11, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef bslmf::FunctionPointerTraits<TestFunc12> X;
            typedef bslmf::TypeList12<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>
                  ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<TestFunc12>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T12, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef bslmf::FunctionPointerTraits<TestFunc13> X;
            typedef bslmf::TypeList13<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13>  ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<TestFunc13>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T13, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
        {
            typedef bslmf::FunctionPointerTraits<TestFunc14> X;
            typedef bslmf::TypeList14<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13,T14> ListType;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<TestFunc14>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T14, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

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
