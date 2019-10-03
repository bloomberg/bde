// bslmf_functionpointertraits.t.cpp                                  -*-C++-*-

#include <bslmf_functionpointertraits.h>

#include <bslmf_issame.h>    // for testing only
#include <bslmf_typelist.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>      // print, fprintf
#include <stdlib.h>     // atoi

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//  TBD
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] FUNCTION POINTER TYPES
// [ 3] 'VOID' FUNCTION POINTER TYPES
// [ 4] VARARGS FUNCTION POINTER TYPES

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
typedef T1*  (*TestEllipsisFunc1)(T1,...);
typedef T2*  (*TestEllipsisFunc2)(T1, T2,...);
typedef T3*  (*TestEllipsisFunc3)(T1, T2, T3,...);
typedef T4*  (*TestEllipsisFunc4)(T1, T2, T3, T4,...);
typedef T5*  (*TestEllipsisFunc5)(T1, T2, T3, T4, T5,...);
typedef T6*  (*TestEllipsisFunc6)(T1, T2, T3, T4, T5, T6,...);
typedef T7*  (*TestEllipsisFunc7)(T1, T2, T3, T4, T5, T6, T7,...);
typedef T8*  (*TestEllipsisFunc8)(T1, T2, T3, T4, T5, T6, T7, T8,...);
typedef T9*  (*TestEllipsisFunc9)(T1, T2, T3, T4, T5, T6, T7, T8, T9,...);
typedef T10* (*TestEllipsisFunc10)(T1, T2, T3, T4, T5, T6, T7, T8, T9,
                                   T10,...);
typedef T11* (*TestEllipsisFunc11)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                                   T11,...);
typedef T12* (*TestEllipsisFunc12)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                                   T11, T12,...);
typedef T13* (*TestEllipsisFunc13)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                                   T11, T12, T13,...);
typedef T14* (*TestEllipsisFunc14)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                                   T11, T12, T13, T14,...);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
typedef T0 (*TestFunc0NE)() noexcept;
typedef T1 (*TestFunc1NE)(T1) noexcept;
typedef T2 (*TestFunc2NE)(T1, T2) noexcept;
typedef T3 (*TestFunc3NE)(T1, T2, T3) noexcept;
typedef T4 (*TestFunc4NE)(T1, T2, T3, T4) noexcept;
typedef T5 (*TestFunc5NE)(T1, T2, T3, T4, T5) noexcept;
typedef T6 (*TestFunc6NE)(T1, T2, T3, T4, T5, T6) noexcept;
typedef T7 (*TestFunc7NE)(T1, T2, T3, T4, T5, T6, T7) noexcept;
typedef T8 (*TestFunc8NE)(T1, T2, T3, T4, T5, T6, T7, T8) noexcept;
typedef T9 (*TestFunc9NE)(T1, T2, T3, T4, T5, T6, T7, T8, T9) noexcept;
typedef T10 (*TestFunc10NE)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) noexcept;
typedef T11 (*TestFunc11NE)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                            T11) noexcept;
typedef T12 (*TestFunc12NE)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
                            T12) noexcept;
typedef T13 (*TestFunc13NE)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12,
                            T13) noexcept;
typedef T14 (*TestFunc14NE)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12,
                            T13, T14) noexcept;

typedef void (*TestVoidFunc0NE)() noexcept;
typedef void (*TestVoidFunc1NE)(T1) noexcept;
typedef void (*TestVoidFunc2NE)(T1, T2) noexcept;
typedef void (*TestVoidFunc3NE)(T1, T2, T3) noexcept;
typedef void (*TestVoidFunc4NE)(T1, T2, T3, T4) noexcept;
typedef void (*TestVoidFunc5NE)(T1, T2, T3, T4, T5) noexcept;
typedef void (*TestVoidFunc6NE)(T1, T2, T3, T4, T5, T6) noexcept;
typedef void (*TestVoidFunc7NE)(T1, T2, T3, T4, T5, T6, T7) noexcept;
typedef void (*TestVoidFunc8NE)(T1, T2, T3, T4, T5, T6, T7, T8) noexcept;
typedef void (*TestVoidFunc9NE)(T1, T2, T3, T4, T5, T6, T7, T8, T9) noexcept;
typedef void (*TestVoidFunc10NE)(T1, T2, T3, T4, T5, T6, T7, T8, T9,
                                 T10) noexcept;
typedef void (*TestVoidFunc11NE)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                                 T11) noexcept;
typedef void (*TestVoidFunc12NE)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
                                 T12) noexcept;
typedef void (*TestVoidFunc13NE)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
                                 T12, T13) noexcept;
typedef void (*TestVoidFunc14NE)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
                                 T12, T13, T14) noexcept;

typedef T0*  (*TestEllipsisFunc0NE)(...) noexcept;
typedef T1*  (*TestEllipsisFunc1NE)(T1,...) noexcept;
typedef T2*  (*TestEllipsisFunc2NE)(T1, T2,...) noexcept;
typedef T3*  (*TestEllipsisFunc3NE)(T1, T2, T3,...) noexcept;
typedef T4*  (*TestEllipsisFunc4NE)(T1, T2, T3, T4,...) noexcept;
typedef T5*  (*TestEllipsisFunc5NE)(T1, T2, T3, T4, T5,...) noexcept;
typedef T6*  (*TestEllipsisFunc6NE)(T1, T2, T3, T4, T5, T6,...) noexcept;
typedef T7*  (*TestEllipsisFunc7NE)(T1, T2, T3, T4, T5, T6, T7,...) noexcept;
typedef T8*  (*TestEllipsisFunc8NE)(T1, T2, T3, T4, T5, T6, T7,
                                    T8,...) noexcept;
typedef T9*  (*TestEllipsisFunc9NE)(T1, T2, T3, T4, T5, T6, T7, T8,
                                    T9,...) noexcept;
typedef T10* (*TestEllipsisFunc10NE)(T1, T2, T3, T4, T5, T6, T7, T8, T9,
                                     T10,...) noexcept;
typedef T11* (*TestEllipsisFunc11NE)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                                     T11,...) noexcept;
typedef T12* (*TestEllipsisFunc12NE)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                                     T11, T12,...) noexcept;
typedef T13* (*TestEllipsisFunc13NE)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                                     T11, T12, T13,...) noexcept;
typedef T14* (*TestEllipsisFunc14NE)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                                     T11, T12, T13, T14,...) noexcept;

#endif // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES

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

    setbuf(stdout, NULL);       // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
          usageExample();
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // VARARGS FUNCTION POINTER TYPES
        //   This test exercises  'const', 'volatile', and 'const volatile'
        //   function pointer types that have C-style varargs  with the trait.
        //
        // Concerns:
        //   1. That the functions exist with the documented signatures.
        //   2. That the basic functionality works as documented.
        //
        // Plan:
        //   Instantiate the trait type with each function pointer type and
        //   verify that the contents of the trait type are correct.
        //
        // Testing:
        //   VARARGS FUNCTION POINTER TYPES
        // --------------------------------------------------------------------

        if (verbose) printf("\nVARARGS FUNCTION POINTER TYPES"
                            "\n==============================\n");

        {
            typedef TestEllipsisFunc0 const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList0 ListType;
            typedef T0 *FuncType(...);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T0 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc1 volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList1<T1> ListType;
            typedef T1 *FuncType(T1,...);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T1 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc2 const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList2<T1,T2> ListType;
            typedef T2 *FuncType(T1,T2,...);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T2 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc3 const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList3<T1,T2,T3> ListType;
            typedef T3 *FuncType(T1,T2,T3,...);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T3 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc4 volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList4<T1,T2,T3,T4> ListType;
            typedef T4 *FuncType(T1,T2,T3,T4,...);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T4 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc5 const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList5<T1,T2,T3,T4,T5> ListType;
            typedef T5 *FuncType(T1,T2,T3,T4,T5,...);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T5 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc6 const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList6<T1,T2,T3,T4,T5,T6> ListType;
            typedef T6 *FuncType(T1,T2,T3,T4,T5,T6,...);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T6 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc7 volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList7<T1,T2,T3,T4,T5,T6,T7> ListType;
            typedef T7 *FuncType(T1,T2,T3,T4,T5,T6,T7,...);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T7 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc8 const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList8<T1,T2,T3,T4,T5,T6,T7,T8> ListType;
            typedef T8 *FuncType(T1,T2,T3,T4,T5,T6,T7,T8,...);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T8 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc9 const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList9<T1,T2,T3,T4,T5,T6,T7,T8,T9> ListType;
            typedef T9 *FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,...);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T9 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc10 volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList10<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>
                  ListType;
            typedef T10 *FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,...);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T10 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc11 const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList11<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>
                  ListType;
            typedef T11 *FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,...);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T11 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc12 const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList12<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>
                  ListType;
            typedef T12 *FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,...);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T12 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc13 volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList13<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13>  ListType;
            typedef T13 *FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13,...);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T13 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc14 const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList14<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13,T14> ListType;
            typedef T14 *FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13,T14,...);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T14 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
        {
            typedef TestEllipsisFunc0NE const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList0 ListType;
            typedef T0 *FuncType(...) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T0 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc1NE volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList1<T1> ListType;
            typedef T1 *FuncType(T1,...) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T1 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc2NE const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList2<T1,T2> ListType;
            typedef T2 *FuncType(T1,T2,...) noexcept;
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T2 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc3NE const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList3<T1,T2,T3> ListType;
            typedef T3 *FuncType(T1,T2,T3,...) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T3 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc4NE volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList4<T1,T2,T3,T4> ListType;
            typedef T4 *FuncType(T1,T2,T3,T4,...) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T4 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc5NE const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList5<T1,T2,T3,T4,T5> ListType;
            typedef T5 *FuncType(T1,T2,T3,T4,T5,...) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T5 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc6NE const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList6<T1,T2,T3,T4,T5,T6> ListType;
            typedef T6 *FuncType(T1,T2,T3,T4,T5,T6,...) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T6 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc7NE volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList7<T1,T2,T3,T4,T5,T6,T7> ListType;
            typedef T7 *FuncType(T1,T2,T3,T4,T5,T6,T7,...) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T7 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc8NE const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList8<T1,T2,T3,T4,T5,T6,T7,T8> ListType;
            typedef T8 *FuncType(T1,T2,T3,T4,T5,T6,T7,T8,...) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T8 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc9NE const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList9<T1,T2,T3,T4,T5,T6,T7,T8,T9> ListType;
            typedef T9 *FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,...) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T9 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc10NE volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList10<T1,T2,T3,T4,T5,T6,T7,T8,T9,
                                      T10> ListType;
            typedef T10 *FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,...) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T10 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc11NE const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList11<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,
                                      T11> ListType;
            typedef T11 *FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,
                                  T11,...) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T11 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc12NE const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList12<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                      T12> ListType;
            typedef T12 *FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                  T12,...) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T12 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc13NE volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList13<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                      T12,T13>  ListType;
            typedef T13 *FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                  T12,T13,...) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T13 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestEllipsisFunc14NE const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList14<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                      T12,T13,T14> ListType;
            typedef T14 *FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                  T12,T13,T14,...) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::e_IS_VARARG);
            ASSERT_SAME(T14 *, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
#endif  // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'VOID' FUNCTION POINTER TYPES
        //   This test exercises 'void'-returning  'const', 'volatile', and
        //   'const volatile' function pointer types with the trait.
        //
        // Concerns:
        //   1. That the functions exist with the documented signatures.
        //   2. That the basic functionality works as documented.
        //
        // Plan:
        //   Instantiate the trait type with each function pointer type and
        //   verify that the contents of the trait type are correct.
        //
        // Testing:
        //   'VOID' FUNCTION POINTER TYPES
        // --------------------------------------------------------------------

        if (verbose) printf("\n'VOID' FUNCTION POINTER TYPES"
                            "\n=============================\n");

        {
            typedef TestVoidFunc0 const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList0 ListType;
            typedef void FuncType();
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc1 volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList1<T1> ListType;
            typedef void FuncType(T1);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc2 const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList2<T1,T2> ListType;
            typedef void FuncType(T1,T2);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc3 const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList3<T1,T2,T3> ListType;
            typedef void FuncType(T1,T2,T3);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc4 volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList4<T1,T2,T3,T4> ListType;
            typedef void FuncType(T1,T2,T3,T4);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc5 const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList5<T1,T2,T3,T4,T5> ListType;
            typedef void FuncType(T1,T2,T3,T4,T5);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc6 const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList6<T1,T2,T3,T4,T5,T6> ListType;
            typedef void FuncType(T1,T2,T3,T4,T5,T6);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc7 volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList7<T1,T2,T3,T4,T5,T6,T7> ListType;
            typedef void FuncType(T1,T2,T3,T4,T5,T6,T7);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc8 const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList8<T1,T2,T3,T4,T5,T6,T7,T8> ListType;
            typedef void FuncType(T1,T2,T3,T4,T5,T6,T7,T8);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc9 const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList9<T1,T2,T3,T4,T5,T6,T7,T8,T9> ListType;
            typedef void FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc10 volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList10<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>
                  ListType;
            typedef void FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc11 const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList11<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>
                  ListType;
            typedef void FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc12 const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList12<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>
                  ListType;
            typedef void FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc13 volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList13<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13>  ListType;
            typedef void FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc14 const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList14<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13,T14> ListType;
            typedef void FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13,T14);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
        {
            typedef TestVoidFunc0NE const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList0 ListType;
            typedef void FuncType() noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc1NE volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList1<T1> ListType;
            typedef void FuncType(T1) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc2NE const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList2<T1,T2> ListType;
            typedef void FuncType(T1,T2) noexcept;
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc3NE const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList3<T1,T2,T3> ListType;
            typedef void FuncType(T1,T2,T3) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc4NE volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList4<T1,T2,T3,T4> ListType;
            typedef void FuncType(T1,T2,T3,T4) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc5NE const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList5<T1,T2,T3,T4,T5> ListType;
            typedef void FuncType(T1,T2,T3,T4,T5) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc6NE const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList6<T1,T2,T3,T4,T5,T6> ListType;
            typedef void FuncType(T1,T2,T3,T4,T5,T6) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc7NE volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList7<T1,T2,T3,T4,T5,T6,T7> ListType;
            typedef void FuncType(T1,T2,T3,T4,T5,T6,T7) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc8NE const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList8<T1,T2,T3,T4,T5,T6,T7,T8> ListType;
            typedef void FuncType(T1,T2,T3,T4,T5,T6,T7,T8) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc9NE const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList9<T1,T2,T3,T4,T5,T6,T7,T8,T9> ListType;
            typedef void FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc10NE volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList10<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10> ListType;
            typedef void FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc11NE const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList11<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,
                                      T11> ListType;
            typedef void FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc12NE const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList12<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                      T12> ListType;
            typedef void FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                  T12) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc13NE volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList13<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                      T12,T13>  ListType;
            typedef void FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                  T12,T13) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestVoidFunc14NE const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList14<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                      T12,T13,T14> ListType;
            typedef void FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                  T12,T13,T14) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(void, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
#endif  // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // FUNCTION POINTER TYPES
        //   This test exercises 'const', 'volatile', and 'const volatile'
        //   function pointer types with the trait.
        //
        // Concerns:
        //   1. That the functions exist with the documented signatures.
        //   2. That the basic functionality works as documented.
        //
        // Plan:
        //   Instantiate the trait type with each function pointer type and
        //   verify that the contents of the trait type are correct.
        //
        // Testing:
        //   FUNCTION POINTER TYPES
        // --------------------------------------------------------------------

        if (verbose) printf("\nFUNCTION POINTER TYPES"
                            "\n======================\n");

        {
            typedef TestFunc0 const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList0 ListType;
            typedef T0 FuncType();
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T0, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc1 volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList1<T1> ListType;
            typedef T1 FuncType(T1);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T1, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc2 const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList2<T1,T2> ListType;
            typedef T2 FuncType(T1,T2);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T2, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc3 const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList3<T1,T2,T3> ListType;
            typedef T3 FuncType(T1,T2,T3);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T3, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc4 volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList4<T1,T2,T3,T4> ListType;
            typedef T4 FuncType(T1,T2,T3,T4);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T4, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc5 const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList5<T1,T2,T3,T4,T5> ListType;
            typedef T5 FuncType(T1,T2,T3,T4,T5);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T5, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc6 const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList6<T1,T2,T3,T4,T5,T6> ListType;
            typedef T6 FuncType(T1,T2,T3,T4,T5,T6);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T6, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc7 volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList7<T1,T2,T3,T4,T5,T6,T7> ListType;
            typedef T7 FuncType(T1,T2,T3,T4,T5,T6,T7);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T7, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc8 const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList8<T1,T2,T3,T4,T5,T6,T7,T8> ListType;
            typedef T8 FuncType(T1,T2,T3,T4,T5,T6,T7,T8);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T8, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc9 const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList9<T1,T2,T3,T4,T5,T6,T7,T8,T9> ListType;
            typedef T9 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T9, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc10 volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList10<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>
                  ListType;
            typedef T10 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T10, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc11 const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList11<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>
                  ListType;
            typedef T11 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T11, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc12 const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList12<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>
                  ListType;
            typedef T12 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T12, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc13 volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList13<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13>  ListType;
            typedef T13 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T13, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc14 const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList14<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13,T14> ListType;
            typedef T14 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13,T14);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T14, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
        {
            typedef TestFunc0NE const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList0 ListType;
            typedef T0 FuncType() noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T0, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc1NE volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList1<T1> ListType;
            typedef T1 FuncType(T1) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T1, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc2NE const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList2<T1,T2> ListType;
            typedef T2 FuncType(T1,T2) noexcept;
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T2, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc3NE const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList3<T1,T2,T3> ListType;
            typedef T3 FuncType(T1,T2,T3) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T3, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc4NE volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList4<T1,T2,T3,T4> ListType;
            typedef T4 FuncType(T1,T2,T3,T4) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T4, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc5NE const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList5<T1,T2,T3,T4,T5> ListType;
            typedef T5 FuncType(T1,T2,T3,T4,T5) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T5, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc6NE const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList6<T1,T2,T3,T4,T5,T6> ListType;
            typedef T6 FuncType(T1,T2,T3,T4,T5,T6) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T6, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc7NE volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList7<T1,T2,T3,T4,T5,T6,T7> ListType;
            typedef T7 FuncType(T1,T2,T3,T4,T5,T6,T7) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T7, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc8NE const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList8<T1,T2,T3,T4,T5,T6,T7,T8> ListType;
            typedef T8 FuncType(T1,T2,T3,T4,T5,T6,T7,T8) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T8, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc9NE const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList9<T1,T2,T3,T4,T5,T6,T7,T8,T9> ListType;
            typedef T9 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T9, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc10NE volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList10<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10> ListType;
            typedef T10 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T10, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc11NE const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList11<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,
                                      T11> ListType;
            typedef T11 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T11, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc12NE const FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList12<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                      T12> ListType;
            typedef T12 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                 T12) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T12, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc13NE volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList13<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                      T12,T13>  ListType;
            typedef T13 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                 T12,T13) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T13, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc14NE const volatile FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList14<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                      T12,T13,T14> ListType;
            typedef T14 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                 T12,T13,T14) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T14, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
#endif  // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This test exercises basic for simple function types and their
        //   corresponding 'noexcept' types.
        //
        // Concerns:
        //   1. That the functions exist with the documented signatures.
        //   2. That the basic functionality works as documented.
        //
        // Plan:
        //   Instantiate the trait type with each function pointer type and
        //   verify that the contents of the trait type are correct.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        {
            typedef TestFunc0 FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList0 ListType;
            typedef T0 FuncType();
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T0, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc1 FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList1<T1> ListType;
            typedef T1 FuncType(T1);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T1, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc2 FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList2<T1,T2> ListType;
            typedef T2 FuncType(T1,T2);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T2, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc3 FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList3<T1,T2,T3> ListType;
            typedef T3 FuncType(T1,T2,T3);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T3, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc4 FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList4<T1,T2,T3,T4> ListType;
            typedef T4 FuncType(T1,T2,T3,T4);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T4, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc5 FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList5<T1,T2,T3,T4,T5> ListType;
            typedef T5 FuncType(T1,T2,T3,T4,T5);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T5, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc6 FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList6<T1,T2,T3,T4,T5,T6> ListType;
            typedef T6 FuncType(T1,T2,T3,T4,T5,T6);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T6, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc7 FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList7<T1,T2,T3,T4,T5,T6,T7> ListType;
            typedef T7 FuncType(T1,T2,T3,T4,T5,T6,T7);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T7, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc8 FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList8<T1,T2,T3,T4,T5,T6,T7,T8> ListType;
            typedef T8 FuncType(T1,T2,T3,T4,T5,T6,T7,T8);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T8, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc9 FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList9<T1,T2,T3,T4,T5,T6,T7,T8,T9> ListType;
            typedef T9 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T9, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc10 FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList10<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>
                  ListType;
            typedef T10 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T10, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc11 FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList11<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>
                  ListType;
            typedef T11 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T11, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc12 FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList12<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>
                  ListType;
            typedef T12 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T12, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc13 FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList13<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13>  ListType;
            typedef T13 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T13, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc14 FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList14<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13,T14> ListType;
            typedef T14 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13,T14);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T14, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
        {
            typedef TestFunc0NE FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList0 ListType;
            typedef T0 FuncType() noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T0, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc1NE FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList1<T1> ListType;
            typedef T1 FuncType(T1) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T1, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc2NE FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList2<T1,T2> ListType;
            typedef T2 FuncType(T1,T2) noexcept;
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T2, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc3NE FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList3<T1,T2,T3> ListType;
            typedef T3 FuncType(T1,T2,T3) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T3, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc4NE FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList4<T1,T2,T3,T4> ListType;
            typedef T4 FuncType(T1,T2,T3,T4) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T4, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc5NE FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList5<T1,T2,T3,T4,T5> ListType;
            typedef T5 FuncType(T1,T2,T3,T4,T5) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T5, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc6NE FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList6<T1,T2,T3,T4,T5,T6> ListType;
            typedef T6 FuncType(T1,T2,T3,T4,T5,T6) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T6, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc7NE FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList7<T1,T2,T3,T4,T5,T6,T7> ListType;
            typedef T7 FuncType(T1,T2,T3,T4,T5,T6,T7) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T7, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc8NE FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList8<T1,T2,T3,T4,T5,T6,T7,T8> ListType;
            typedef T8 FuncType(T1,T2,T3,T4,T5,T6,T7,T8) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T8, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc9NE FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList9<T1,T2,T3,T4,T5,T6,T7,T8,T9> ListType;
            typedef T9 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T9, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc10NE FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList10<T1,T2,T3,T4,T5,T6,T7,T8,T9,
                                      T10> ListType;
            typedef T10 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T10, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc11NE FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList11<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,
                                      T11> ListType;
            typedef T11 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T11, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc12NE FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList12<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                      T12> ListType;
            typedef T12 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                 T12) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T12, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc13NE FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList13<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                      T12,T13>  ListType;
            typedef T13 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                 T12,T13) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T13, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
        {
            typedef TestFunc14NE FuncPtrType;
            typedef bslmf::FunctionPointerTraits<FuncPtrType> X;
            typedef bslmf::TypeList14<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                      T12,T13,T14> ListType;
            typedef T14 FuncType(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                                 T12,T13,T14) noexcept;
            ASSERT(1==X::IS_FUNCTION_POINTER);
            ASSERT(1==bslmf::IsFunctionPointer<FuncPtrType>::value);
            ASSERT(0==X::e_IS_VARARG);
            ASSERT_SAME(T14, X::ResultType);
            ASSERT_SAME(ListType, X::ArgumentList);
            ASSERT_SAME(FuncType, X::FuncType);
        }
#endif  // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
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
