// bslmf_memberfunctionpointertraits.t.cpp                            -*-C++-*-

#include <bslmf_memberfunctionpointertraits.h>

#include <bslmf_issame.h>    // for testing only
#include <bslmf_typelist.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>           // printf
#include <stdlib.h>          // atoi

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
// [1 ] Breathing test

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

#define ASSERT_SAME(T1,T2) ASSERT((1 == bsl::is_same<T1,T2>::value))

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

typedef T0 (T0::*TestFunc0)();
typedef T1 (T1::*TestFunc1)(T1);
typedef T2 (T2::*TestFunc2)(T1, T2);
typedef T3 (T3::*TestFunc3)(T1, T2, T3);
typedef T4 (T4::*TestFunc4)(T1, T2, T3, T4);
typedef T5 (T5::*TestFunc5)(T1, T2, T3,T4, T5);
typedef T6 (T6::*TestFunc6)(T1, T2, T3, T4, T5, T6);
typedef T7 (T7::*TestFunc7)(T1, T2, T3, T4, T5, T6, T7);
typedef T8 (T8::*TestFunc8)(T1, T2, T3, T4, T5, T6, T7, T8);
typedef T9 (T9::*TestFunc9)(T1, T2, T3, T4, T5, T6, T7, T8, T9);
typedef T10 (T10::*TestFunc10)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10);
typedef T11 (T11::*TestFunc11)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11);
typedef T12 (T12::*TestFunc12)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
                               T12);
typedef T13 (T13::*TestFunc13)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
                               T12, T13);
typedef T14 (T14::*TestFunc14)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
                               T12, T13, T14);

typedef T0 (T0::*TestFunc0C)() const;
typedef T1 (T1::*TestFunc1C)(T1) const;
typedef T2 (T2::*TestFunc2C)(T1, T2) const;
typedef T3 (T3::*TestFunc3C)(T1, T2, T3) const;
typedef T4 (T4::*TestFunc4C)(T1, T2, T3, T4) const;
typedef T5 (T5::*TestFunc5C)(T1, T2, T3,T4, T5) const;
typedef T6 (T6::*TestFunc6C)(T1, T2, T3, T4, T5, T6) const;
typedef T7 (T7::*TestFunc7C)(T1, T2, T3, T4, T5, T6, T7) const;
typedef T8 (T8::*TestFunc8C)(T1, T2, T3, T4, T5, T6, T7, T8) const;
typedef T9 (T9::*TestFunc9C)(T1, T2, T3, T4, T5, T6, T7, T8, T9) const;
typedef T10 (T10::*TestFunc10C)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) const;
typedef T11 (T11::*TestFunc11C)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                                T11) const;
typedef T12 (T12::*TestFunc12C)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
                                T12) const;
typedef T13 (T13::*TestFunc13C)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
                                T12, T13) const;
typedef T14 (T14::*TestFunc14C)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
                                T12, T13, T14) const;

typedef T0 (T0::*TestFunc0V)() volatile;
typedef T1 (T1::*TestFunc1V)(T1) volatile;
typedef T2 (T2::*TestFunc2V)(T1, T2) volatile;
typedef T3 (T3::*TestFunc3V)(T1, T2, T3) volatile;
typedef T4 (T4::*TestFunc4V)(T1, T2, T3, T4) volatile;
typedef T5 (T5::*TestFunc5V)(T1, T2, T3,T4, T5) volatile;
typedef T6 (T6::*TestFunc6V)(T1, T2, T3, T4, T5, T6) volatile;
typedef T7 (T7::*TestFunc7V)(T1, T2, T3, T4, T5, T6, T7) volatile;
typedef T8 (T8::*TestFunc8V)(T1, T2, T3, T4, T5, T6, T7, T8) volatile;
typedef T9 (T9::*TestFunc9V)(T1, T2, T3, T4, T5, T6, T7, T8, T9) volatile;
typedef T10 (T10::*TestFunc10V)(T1, T2, T3, T4, T5, T6, T7, T8, T9,
                                T10) volatile;
typedef T11 (T11::*TestFunc11V)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                                T11) volatile;
typedef T12 (T12::*TestFunc12V)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
                                T12) volatile;
typedef T13 (T13::*TestFunc13V)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
                                T12, T13) volatile;
typedef T14 (T14::*TestFunc14V)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
                                T12, T13, T14) volatile;

typedef T0 (T0::*TestFunc0CV)() const volatile;
typedef T1 (T1::*TestFunc1CV)(T1) const volatile;
typedef T2 (T2::*TestFunc2CV)(T1, T2) const volatile;
typedef T3 (T3::*TestFunc3CV)(T1, T2, T3) const volatile;
typedef T4 (T4::*TestFunc4CV)(T1, T2, T3, T4) const volatile;
typedef T5 (T5::*TestFunc5CV)(T1, T2, T3,T4, T5) const volatile;
typedef T6 (T6::*TestFunc6CV)(T1, T2, T3, T4, T5, T6) const volatile;
typedef T7 (T7::*TestFunc7CV)(T1, T2, T3, T4, T5, T6, T7) const volatile;
typedef T8 (T8::*TestFunc8CV)(T1, T2, T3, T4, T5, T6, T7, T8) const volatile;
typedef T9 (T9::*TestFunc9CV)(T1, T2, T3, T4, T5, T6, T7, T8,
                             T9) const volatile;
typedef T10 (T10::*TestFunc10CV)(T1, T2, T3, T4, T5, T6, T7, T8, T9,
                                 T10) const volatile;
typedef T11 (T11::*TestFunc11CV)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                                 T11) const volatile;
typedef T12 (T12::*TestFunc12CV)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
                                 T12) const volatile;
typedef T13 (T13::*TestFunc13CV)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
                                 T12, T13) const volatile;
typedef T14 (T14::*TestFunc14CV)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
                                 T12, T13, T14) const volatile;

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
    typedef void (*VoidFunc0)();
//..
// and the following 'struct' with the following members:
//..
    struct MyTestClass {
        static void voidFunc0() {}
        int func1(int) { return 0; }
        int func2(int, int) { return 1; }
    };
//..
// In order to deduce the types of 'voidFunc0' and 'func1', we will use the
// C++ template system to get two auxiliary functions:
//..
    template <class TYPE>
    void checkNotMemberFunctionPointer(TYPE object)
    {
        (void) object;
        ASSERT(0 == bslmf::IsMemberFunctionPointer<TYPE>::value);
    }

    template <class RET, class ARGS, class TYPE>
    void checkMemberFunctionPointer(TYPE object)
    {
        (void) object;
        ASSERT(1 == bslmf::IsMemberFunctionPointer<TYPE>::value);
        typedef typename bslmf::MemberFunctionPointerTraits<TYPE>::ResultType
            ResultType;
        typedef typename bslmf::MemberFunctionPointerTraits<TYPE>::ArgumentList
            ArgumentList;
        ASSERT(1 == (bsl::is_same<ResultType, RET>::value));
        ASSERT(1 == (bsl::is_same<ArgumentList, ARGS>::value));
    }
//..
// The following program should compile and run without errors:
//..
    void usageExample()
    {
        ASSERT(0 == bslmf::IsMemberFunctionPointer<int>::value);
        ASSERT(0 == bslmf::IsMemberFunctionPointer<int>::value);

        checkNotMemberFunctionPointer( &MyTestClass::voidFunc0);
        checkMemberFunctionPointer<int, bslmf::TypeList1<int> >(
                                                          &MyTestClass::func1);
        checkMemberFunctionPointer<int, bslmf::TypeList2<int, int> >(
                                                          &MyTestClass::func2);
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

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);       // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) {
      case 0:  // Zero is always the leading case.
      case 5: {
          usageExample();
      } break;
      case 4: {
          //------------------------------------------------------------------
          // TESTING POINTER TO MEMBER OF CONST VOLATILE CLASS
          //------------------------------------------------------------------
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc0CV> X;
              typedef bslmf::TypeList0 ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc0CV>::value);
              ASSERT_SAME(T0, X::ResultType);
              ASSERT_SAME(const volatile T0, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc1CV> X;
              typedef bslmf::TypeList1<T1> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc1CV>::value);
              ASSERT_SAME(T1, X::ResultType);
              ASSERT_SAME(const volatile T1, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc2CV> X;
              typedef bslmf::TypeList2<T1,T2> ListType;
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc2CV>::value);
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT_SAME(T2, X::ResultType);
              ASSERT_SAME(const volatile T2, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc3CV> X;
              typedef bslmf::TypeList3<T1,T2,T3> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc3CV>::value);
              ASSERT_SAME(T3, X::ResultType);
              ASSERT_SAME(const volatile T3, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc4CV> X;
              typedef bslmf::TypeList4<T1,T2,T3,T4> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc4CV>::value);
              ASSERT_SAME(T4, X::ResultType);
              ASSERT_SAME(const volatile T4, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc5CV> X;
              typedef bslmf::TypeList5<T1,T2,T3,T4,T5> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc5CV>::value);
              ASSERT_SAME(T5, X::ResultType);
              ASSERT_SAME(const volatile T5, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc6CV> X;
              typedef bslmf::TypeList6<T1,T2,T3,T4,T5,T6> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc6CV>::value);
              ASSERT_SAME(T6, X::ResultType);
              ASSERT_SAME(const volatile T6, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc7CV> X;
              typedef bslmf::TypeList7<T1,T2,T3,T4,T5,T6,T7> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc7CV>::value);
              ASSERT_SAME(T7, X::ResultType);
              ASSERT_SAME(const volatile T7, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc8CV> X;
              typedef bslmf::TypeList8<T1,T2,T3,T4,T5,T6,T7,T8> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc8CV>::value);
              ASSERT_SAME(T8, X::ResultType);
              ASSERT_SAME(const volatile T8, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc9CV> X;
              typedef bslmf::TypeList9<T1,T2,T3,T4,T5,T6,T7,T8,T9> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc9CV>::value);
              ASSERT_SAME(T9, X::ResultType);
              ASSERT_SAME(const volatile T9, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc10CV> X;
              typedef bslmf::TypeList10<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>
                  ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc10CV>::value);
              ASSERT_SAME(T10, X::ResultType);
              ASSERT_SAME(const volatile T10, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc11CV> X;
              typedef bslmf::TypeList11<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>
                  ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc11CV>::value);
              ASSERT_SAME(T11, X::ResultType);
              ASSERT_SAME(const volatile T11, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc12CV> X;
              typedef bslmf::TypeList12<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>
                  ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc12CV>::value);
              ASSERT_SAME(T12, X::ResultType);
              ASSERT_SAME(const volatile T12, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc13CV> X;
              typedef bslmf::TypeList13<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13>  ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc13CV>::value);
              ASSERT_SAME(T13, X::ResultType);
              ASSERT_SAME(const volatile T13, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc14CV> X;
              typedef bslmf::TypeList14<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13,T14> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc14CV>::value);
              ASSERT_SAME(T14, X::ResultType);
              ASSERT_SAME(const volatile T14, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

      } break;
      case 3: {
          //------------------------------------------------------------------
          // TESTING POINTER TO MEMBER OF VOLATILE CLASS
          //------------------------------------------------------------------
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc0V> X;
              typedef bslmf::TypeList0 ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc0V>::value);
              ASSERT_SAME(T0, X::ResultType);
              ASSERT_SAME(volatile T0, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc1V> X;
              typedef bslmf::TypeList1<T1> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc1V>::value);
              ASSERT_SAME(T1, X::ResultType);
              ASSERT_SAME(volatile T1, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc2V> X;
              typedef bslmf::TypeList2<T1,T2> ListType;
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc2V>::value);
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT_SAME(T2, X::ResultType);
              ASSERT_SAME(volatile T2, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc3V> X;
              typedef bslmf::TypeList3<T1,T2,T3> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc3V>::value);
              ASSERT_SAME(T3, X::ResultType);
              ASSERT_SAME(volatile T3, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc4V> X;
              typedef bslmf::TypeList4<T1,T2,T3,T4> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc4V>::value);
              ASSERT_SAME(T4, X::ResultType);
              ASSERT_SAME(volatile T4, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc5V> X;
              typedef bslmf::TypeList5<T1,T2,T3,T4,T5> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc5V>::value);
              ASSERT_SAME(T5, X::ResultType);
              ASSERT_SAME(volatile T5, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc6V> X;
              typedef bslmf::TypeList6<T1,T2,T3,T4,T5,T6> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc6V>::value);
              ASSERT_SAME(T6, X::ResultType);
              ASSERT_SAME(volatile T6, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc7V> X;
              typedef bslmf::TypeList7<T1,T2,T3,T4,T5,T6,T7> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc7V>::value);
              ASSERT_SAME(T7, X::ResultType);
              ASSERT_SAME(volatile T7, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc8V> X;
              typedef bslmf::TypeList8<T1,T2,T3,T4,T5,T6,T7,T8> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc8V>::value);
              ASSERT_SAME(T8, X::ResultType);
              ASSERT_SAME(volatile T8, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc9V> X;
              typedef bslmf::TypeList9<T1,T2,T3,T4,T5,T6,T7,T8,T9> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc9V>::value);
              ASSERT_SAME(T9, X::ResultType);
              ASSERT_SAME(volatile T9, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc10V> X;
              typedef bslmf::TypeList10<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>
                  ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc10V>::value);
              ASSERT_SAME(T10, X::ResultType);
              ASSERT_SAME(volatile T10, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc11V> X;
              typedef bslmf::TypeList11<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>
                  ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc11V>::value);
              ASSERT_SAME(T11, X::ResultType);
              ASSERT_SAME(volatile T11, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc12V> X;
              typedef bslmf::TypeList12<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>
                  ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc12V>::value);
              ASSERT_SAME(T12, X::ResultType);
              ASSERT_SAME(volatile T12, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc13V> X;
              typedef bslmf::TypeList13<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13>  ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc13V>::value);
              ASSERT_SAME(T13, X::ResultType);
              ASSERT_SAME(volatile T13, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc14V> X;
              typedef bslmf::TypeList14<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13,T14> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc14V>::value);
              ASSERT_SAME(T14, X::ResultType);
              ASSERT_SAME(volatile T14, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

      } break;
      case 2: {
          //------------------------------------------------------------------
          // TESTING POINTER TO MEMBER OF CONST CLASS
          //------------------------------------------------------------------
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc0C> X;
              typedef bslmf::TypeList0 ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc0C>::value);
              ASSERT_SAME(T0, X::ResultType);
              ASSERT_SAME(const T0, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc1C> X;
              typedef bslmf::TypeList1<T1> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc1C>::value);
              ASSERT_SAME(T1, X::ResultType);
              ASSERT_SAME(const T1, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc2C> X;
              typedef bslmf::TypeList2<T1,T2> ListType;
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc2C>::value);
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT_SAME(T2, X::ResultType);
              ASSERT_SAME(const T2, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc3C> X;
              typedef bslmf::TypeList3<T1,T2,T3> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc3C>::value);
              ASSERT_SAME(T3, X::ResultType);
              ASSERT_SAME(const T3, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc4C> X;
              typedef bslmf::TypeList4<T1,T2,T3,T4> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc4C>::value);
              ASSERT_SAME(T4, X::ResultType);
              ASSERT_SAME(const T4, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc5C> X;
              typedef bslmf::TypeList5<T1,T2,T3,T4,T5> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc5C>::value);
              ASSERT_SAME(T5, X::ResultType);
              ASSERT_SAME(const T5, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc6C> X;
              typedef bslmf::TypeList6<T1,T2,T3,T4,T5,T6> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc6C>::value);
              ASSERT_SAME(T6, X::ResultType);
              ASSERT_SAME(const T6, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc7C> X;
              typedef bslmf::TypeList7<T1,T2,T3,T4,T5,T6,T7> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc7C>::value);
              ASSERT_SAME(T7, X::ResultType);
              ASSERT_SAME(const T7, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc8C> X;
              typedef bslmf::TypeList8<T1,T2,T3,T4,T5,T6,T7,T8> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc8C>::value);
              ASSERT_SAME(T8, X::ResultType);
              ASSERT_SAME(const T8, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc9C> X;
              typedef bslmf::TypeList9<T1,T2,T3,T4,T5,T6,T7,T8,T9> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc9C>::value);
              ASSERT_SAME(T9, X::ResultType);
              ASSERT_SAME(const T9, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc10C> X;
              typedef bslmf::TypeList10<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>
                  ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc10C>::value);
              ASSERT_SAME(T10, X::ResultType);
              ASSERT_SAME(const T10, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc11C> X;
              typedef bslmf::TypeList11<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>
                  ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc11C>::value);
              ASSERT_SAME(T11, X::ResultType);
              ASSERT_SAME(const T11, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc12C> X;
              typedef bslmf::TypeList12<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>
                  ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc12C>::value);
              ASSERT_SAME(T12, X::ResultType);
              ASSERT_SAME(const T12, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc13C> X;
              typedef bslmf::TypeList13<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13>  ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc13C>::value);
              ASSERT_SAME(T13, X::ResultType);
              ASSERT_SAME(const T13, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc14C> X;
              typedef bslmf::TypeList14<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13,T14> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc14C>::value);
              ASSERT_SAME(T14, X::ResultType);
              ASSERT_SAME(const T14, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

      } break;
      case 1: {
          //------------------------------------------------------------------
          // TESTING POINTER TO MEMBER OF NON-CV CLASS
          //------------------------------------------------------------------
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc0> X;
              typedef bslmf::TypeList0 ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc0>::value);
              ASSERT_SAME(T0, X::ResultType);
              ASSERT_SAME(T0, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc1> X;
              typedef bslmf::TypeList1<T1> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc1>::value);
              ASSERT_SAME(T1, X::ResultType);
              ASSERT_SAME(T1, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc2> X;
              typedef bslmf::TypeList2<T1,T2> ListType;
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc2>::value);
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT_SAME(T2, X::ResultType);
              ASSERT_SAME(T2, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc3> X;
              typedef bslmf::TypeList3<T1,T2,T3> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc3>::value);
              ASSERT_SAME(T3, X::ResultType);
              ASSERT_SAME(T3, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc4> X;
              typedef bslmf::TypeList4<T1,T2,T3,T4> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc4>::value);
              ASSERT_SAME(T4, X::ResultType);
              ASSERT_SAME(T4, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc5> X;
              typedef bslmf::TypeList5<T1,T2,T3,T4,T5> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc5>::value);
              ASSERT_SAME(T5, X::ResultType);
              ASSERT_SAME(T5, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc6> X;
              typedef bslmf::TypeList6<T1,T2,T3,T4,T5,T6> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc6>::value);
              ASSERT_SAME(T6, X::ResultType);
              ASSERT_SAME(T6, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc7> X;
              typedef bslmf::TypeList7<T1,T2,T3,T4,T5,T6,T7> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc7>::value);
              ASSERT_SAME(T7, X::ResultType);
              ASSERT_SAME(T7, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc8> X;
              typedef bslmf::TypeList8<T1,T2,T3,T4,T5,T6,T7,T8> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc8>::value);
              ASSERT_SAME(T8, X::ResultType);
              ASSERT_SAME(T8, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc9> X;
              typedef bslmf::TypeList9<T1,T2,T3,T4,T5,T6,T7,T8,T9> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc9>::value);
              ASSERT_SAME(T9, X::ResultType);
              ASSERT_SAME(T9, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc10> X;
              typedef bslmf::TypeList10<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>
                  ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc10>::value);
              ASSERT_SAME(T10, X::ResultType);
              ASSERT_SAME(T10, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc11> X;
              typedef bslmf::TypeList11<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>
                  ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc11>::value);
              ASSERT_SAME(T11, X::ResultType);
              ASSERT_SAME(T11, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }
          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc12> X;
              typedef bslmf::TypeList12<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>
                  ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc12>::value);
              ASSERT_SAME(T12, X::ResultType);
              ASSERT_SAME(T12, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc13> X;
              typedef bslmf::TypeList13<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13>  ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc13>::value);
              ASSERT_SAME(T13, X::ResultType);
              ASSERT_SAME(T13, X::ClassType);
              ASSERT_SAME(ListType, X::ArgumentList);
          }

          {
              typedef bslmf::MemberFunctionPointerTraits<TestFunc14> X;
              typedef bslmf::TypeList14<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,
                  T12,T13,T14> ListType;
              ASSERT(1==X::IS_MEMBER_FUNCTION_PTR);
              ASSERT(1==bslmf::IsMemberFunctionPointer<TestFunc14>::value);
              ASSERT_SAME(T14, X::ResultType);
              ASSERT_SAME(T14, X::ClassType);
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
