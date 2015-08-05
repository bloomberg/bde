// bslmf_typelist.t.cpp                                               -*-C++-*-

#include <bslmf_typelist.h>

#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//  This test driver verifies each of the 21 typelist template classes provided
//  by 'bslmf_typelist'.  Each template is instantiated with the appropriate
//  number of distinct types.  Each type will be test to ensure that the type
//  defined by it's corresponding Type<N> typedef and TypeOf<N> typedef are
//  correct.
//-----------------------------------------------------------------------------
// [ 2] bslmf::TypeList
// [ 2] bslmf::TypeListTypeOf
// [ 3] bslmf::TypeList0
// [ 3] bslmf::TypeList1
// [ 3] bslmf::TypeList2
// [ 3] bslmf::TypeList3
// [ 3] bslmf::TypeList4
// [ 3] bslmf::TypeList5
// [ 3] bslmf::TypeList6
// [ 3] bslmf::TypeList7
// [ 3] bslmf::TypeList8
// [ 3] bslmf::TypeList9
// [ 3] bslmf::TypeList10
// [ 3] bslmf::TypeList11
// [ 3] bslmf::TypeList12
// [ 3] bslmf::TypeList13
// [ 3] bslmf::TypeList14
// [ 3] bslmf::TypeList15
// [ 3] bslmf::TypeList16
// [ 3] bslmf::TypeList17
// [ 3] bslmf::TypeList18
// [ 3] bslmf::TypeList19
// [ 3] bslmf::TypeList20
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE

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

typedef bslmf::TypeListNil Nil;

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
struct T15 {};
struct T16 {};
struct T17 {};
struct T18 {};
struct T19 {};
struct T20 {};

template <class LIST>
struct ListAsserter0 {
    static void listAssert()
    {
        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1>
struct ListAsserter1 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1, typename LIST::Type1>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1, class A2>
struct ListAsserter2 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1, typename LIST::Type1>::value));
        ASSERT(1 == (bsl::is_same<A2, typename LIST::Type2>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename bslmf::TypeListTypeOf<2, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename LIST::template TypeOf<2>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A2,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<3, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1, class A2, class A3>
struct ListAsserter3 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1, typename LIST::Type1>::value));
        ASSERT(1 == (bsl::is_same<A2, typename LIST::Type2>::value));
        ASSERT(1 == (bsl::is_same<A3, typename LIST::Type3>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename bslmf::TypeListTypeOf<2, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename bslmf::TypeListTypeOf<3, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename LIST::template TypeOf<2>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename LIST::template TypeOf<3>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A2,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A3,
              typename bslmf::TypeListTypeOf<3, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<4, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1, class A2, class A3, class A4>
struct ListAsserter4 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1, typename LIST::Type1>::value));
        ASSERT(1 == (bsl::is_same<A2, typename LIST::Type2>::value));
        ASSERT(1 == (bsl::is_same<A3, typename LIST::Type3>::value));
        ASSERT(1 == (bsl::is_same<A4, typename LIST::Type4>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename bslmf::TypeListTypeOf<2, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename bslmf::TypeListTypeOf<3, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename bslmf::TypeListTypeOf<4, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename LIST::template TypeOf<2>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename LIST::template TypeOf<3>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename LIST::template TypeOf<4>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A2,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A3,
              typename bslmf::TypeListTypeOf<3, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A4,
              typename bslmf::TypeListTypeOf<4, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<5, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1, class A2, class A3, class A4, class A5>
struct ListAsserter5 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1, typename LIST::Type1>::value));
        ASSERT(1 == (bsl::is_same<A2, typename LIST::Type2>::value));
        ASSERT(1 == (bsl::is_same<A3, typename LIST::Type3>::value));
        ASSERT(1 == (bsl::is_same<A4, typename LIST::Type4>::value));
        ASSERT(1 == (bsl::is_same<A5, typename LIST::Type5>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename bslmf::TypeListTypeOf<2, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename bslmf::TypeListTypeOf<3, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename bslmf::TypeListTypeOf<4, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename bslmf::TypeListTypeOf<5, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename LIST::template TypeOf<2>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename LIST::template TypeOf<3>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename LIST::template TypeOf<4>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename LIST::template TypeOf<5>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A2,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A3,
              typename bslmf::TypeListTypeOf<3, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A4,
              typename bslmf::TypeListTypeOf<4, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A5,
              typename bslmf::TypeListTypeOf<5, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<6, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1, class A2, class A3, class A4, class A5,
                      class A6>
struct ListAsserter6 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1, typename LIST::Type1>::value));
        ASSERT(1 == (bsl::is_same<A2, typename LIST::Type2>::value));
        ASSERT(1 == (bsl::is_same<A3, typename LIST::Type3>::value));
        ASSERT(1 == (bsl::is_same<A4, typename LIST::Type4>::value));
        ASSERT(1 == (bsl::is_same<A5, typename LIST::Type5>::value));
        ASSERT(1 == (bsl::is_same<A6, typename LIST::Type6>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename bslmf::TypeListTypeOf<2, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename bslmf::TypeListTypeOf<3, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename bslmf::TypeListTypeOf<4, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename bslmf::TypeListTypeOf<5, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename bslmf::TypeListTypeOf<6, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename LIST::template TypeOf<2>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename LIST::template TypeOf<3>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename LIST::template TypeOf<4>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename LIST::template TypeOf<5>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename LIST::template TypeOf<6>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A2,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A3,
              typename bslmf::TypeListTypeOf<3, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A4,
              typename bslmf::TypeListTypeOf<4, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A5,
              typename bslmf::TypeListTypeOf<5, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A6,
              typename bslmf::TypeListTypeOf<6, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<7, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7>
struct ListAsserter7 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1, typename LIST::Type1>::value));
        ASSERT(1 == (bsl::is_same<A2, typename LIST::Type2>::value));
        ASSERT(1 == (bsl::is_same<A3, typename LIST::Type3>::value));
        ASSERT(1 == (bsl::is_same<A4, typename LIST::Type4>::value));
        ASSERT(1 == (bsl::is_same<A5, typename LIST::Type5>::value));
        ASSERT(1 == (bsl::is_same<A6, typename LIST::Type6>::value));
        ASSERT(1 == (bsl::is_same<A7, typename LIST::Type7>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename bslmf::TypeListTypeOf<2, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename bslmf::TypeListTypeOf<3, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename bslmf::TypeListTypeOf<4, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename bslmf::TypeListTypeOf<5, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename bslmf::TypeListTypeOf<6, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename bslmf::TypeListTypeOf<7, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename LIST::template TypeOf<2>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename LIST::template TypeOf<3>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename LIST::template TypeOf<4>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename LIST::template TypeOf<5>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename LIST::template TypeOf<6>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename LIST::template TypeOf<7>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A2,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A3,
              typename bslmf::TypeListTypeOf<3, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A4,
              typename bslmf::TypeListTypeOf<4, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A5,
              typename bslmf::TypeListTypeOf<5, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A6,
              typename bslmf::TypeListTypeOf<6, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A7,
              typename bslmf::TypeListTypeOf<7, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<8, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7, class A8>
struct ListAsserter8 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1, typename LIST::Type1>::value));
        ASSERT(1 == (bsl::is_same<A2, typename LIST::Type2>::value));
        ASSERT(1 == (bsl::is_same<A3, typename LIST::Type3>::value));
        ASSERT(1 == (bsl::is_same<A4, typename LIST::Type4>::value));
        ASSERT(1 == (bsl::is_same<A5, typename LIST::Type5>::value));
        ASSERT(1 == (bsl::is_same<A6, typename LIST::Type6>::value));
        ASSERT(1 == (bsl::is_same<A7, typename LIST::Type7>::value));
        ASSERT(1 == (bsl::is_same<A8, typename LIST::Type8>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename bslmf::TypeListTypeOf<2, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename bslmf::TypeListTypeOf<3, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename bslmf::TypeListTypeOf<4, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename bslmf::TypeListTypeOf<5, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename bslmf::TypeListTypeOf<6, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename bslmf::TypeListTypeOf<7, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename bslmf::TypeListTypeOf<8, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename LIST::template TypeOf<2>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename LIST::template TypeOf<3>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename LIST::template TypeOf<4>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename LIST::template TypeOf<5>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename LIST::template TypeOf<6>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename LIST::template TypeOf<7>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename LIST::template TypeOf<8>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A2,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A3,
              typename bslmf::TypeListTypeOf<3, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A4,
              typename bslmf::TypeListTypeOf<4, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A5,
              typename bslmf::TypeListTypeOf<5, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A6,
              typename bslmf::TypeListTypeOf<6, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A7,
              typename bslmf::TypeListTypeOf<7, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A8,
              typename bslmf::TypeListTypeOf<8, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<9, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7, class A8, class A9>
struct ListAsserter9 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1, typename LIST::Type1>::value));
        ASSERT(1 == (bsl::is_same<A2, typename LIST::Type2>::value));
        ASSERT(1 == (bsl::is_same<A3, typename LIST::Type3>::value));
        ASSERT(1 == (bsl::is_same<A4, typename LIST::Type4>::value));
        ASSERT(1 == (bsl::is_same<A5, typename LIST::Type5>::value));
        ASSERT(1 == (bsl::is_same<A6, typename LIST::Type6>::value));
        ASSERT(1 == (bsl::is_same<A7, typename LIST::Type7>::value));
        ASSERT(1 == (bsl::is_same<A8, typename LIST::Type8>::value));
        ASSERT(1 == (bsl::is_same<A9, typename LIST::Type9>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename bslmf::TypeListTypeOf<2, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename bslmf::TypeListTypeOf<3, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename bslmf::TypeListTypeOf<4, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename bslmf::TypeListTypeOf<5, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename bslmf::TypeListTypeOf<6, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename bslmf::TypeListTypeOf<7, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename bslmf::TypeListTypeOf<8, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename bslmf::TypeListTypeOf<9, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename LIST::template TypeOf<2>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename LIST::template TypeOf<3>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename LIST::template TypeOf<4>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename LIST::template TypeOf<5>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename LIST::template TypeOf<6>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename LIST::template TypeOf<7>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename LIST::template TypeOf<8>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename LIST::template TypeOf<9>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A2,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A3,
              typename bslmf::TypeListTypeOf<3, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A4,
              typename bslmf::TypeListTypeOf<4, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A5,
              typename bslmf::TypeListTypeOf<5, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A6,
              typename bslmf::TypeListTypeOf<6, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A7,
              typename bslmf::TypeListTypeOf<7, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A8,
              typename bslmf::TypeListTypeOf<8, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A9,
              typename bslmf::TypeListTypeOf<9, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<Nil,
             typename bslmf::TypeListTypeOf<10, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<Nil,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7, class A8, class A9, class A10>
struct ListAsserter10 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1,  typename LIST::Type1>::value));
        ASSERT(1 == (bsl::is_same<A2,  typename LIST::Type2>::value));
        ASSERT(1 == (bsl::is_same<A3,  typename LIST::Type3>::value));
        ASSERT(1 == (bsl::is_same<A4,  typename LIST::Type4>::value));
        ASSERT(1 == (bsl::is_same<A5,  typename LIST::Type5>::value));
        ASSERT(1 == (bsl::is_same<A6,  typename LIST::Type6>::value));
        ASSERT(1 == (bsl::is_same<A7,  typename LIST::Type7>::value));
        ASSERT(1 == (bsl::is_same<A8,  typename LIST::Type8>::value));
        ASSERT(1 == (bsl::is_same<A9,  typename LIST::Type9>::value));
        ASSERT(1 == (bsl::is_same<A10, typename LIST::Type10>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename bslmf::TypeListTypeOf<2, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename bslmf::TypeListTypeOf<3, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename bslmf::TypeListTypeOf<4, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename bslmf::TypeListTypeOf<5, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename bslmf::TypeListTypeOf<6, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename bslmf::TypeListTypeOf<7, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename bslmf::TypeListTypeOf<8, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename bslmf::TypeListTypeOf<9, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename bslmf::TypeListTypeOf<10, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename LIST::template TypeOf<2>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename LIST::template TypeOf<3>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename LIST::template TypeOf<4>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename LIST::template TypeOf<5>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename LIST::template TypeOf<6>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename LIST::template TypeOf<7>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename LIST::template TypeOf<8>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename LIST::template TypeOf<9>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename LIST::template TypeOf<10>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A2,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A3,
              typename bslmf::TypeListTypeOf<3, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A4,
              typename bslmf::TypeListTypeOf<4, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A5,
              typename bslmf::TypeListTypeOf<5, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A6,
              typename bslmf::TypeListTypeOf<6, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A7,
              typename bslmf::TypeListTypeOf<7, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A8,
              typename bslmf::TypeListTypeOf<8, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A9,
              typename bslmf::TypeListTypeOf<9, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A10,
             typename bslmf::TypeListTypeOf<10, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<Nil,
             typename bslmf::TypeListTypeOf<11, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<Nil,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7, class A8, class A9, class A10,
                      class A11>
struct ListAsserter11 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1,  typename LIST::Type1>::value));
        ASSERT(1 == (bsl::is_same<A2,  typename LIST::Type2>::value));
        ASSERT(1 == (bsl::is_same<A3,  typename LIST::Type3>::value));
        ASSERT(1 == (bsl::is_same<A4,  typename LIST::Type4>::value));
        ASSERT(1 == (bsl::is_same<A5,  typename LIST::Type5>::value));
        ASSERT(1 == (bsl::is_same<A6,  typename LIST::Type6>::value));
        ASSERT(1 == (bsl::is_same<A7,  typename LIST::Type7>::value));
        ASSERT(1 == (bsl::is_same<A8,  typename LIST::Type8>::value));
        ASSERT(1 == (bsl::is_same<A9,  typename LIST::Type9>::value));
        ASSERT(1 == (bsl::is_same<A10, typename LIST::Type10>::value));
        ASSERT(1 == (bsl::is_same<A11, typename LIST::Type11>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename bslmf::TypeListTypeOf<2, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename bslmf::TypeListTypeOf<3, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename bslmf::TypeListTypeOf<4, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename bslmf::TypeListTypeOf<5, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename bslmf::TypeListTypeOf<6, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename bslmf::TypeListTypeOf<7, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename bslmf::TypeListTypeOf<8, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename bslmf::TypeListTypeOf<9, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename bslmf::TypeListTypeOf<10, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename bslmf::TypeListTypeOf<11, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename LIST::template TypeOf<2>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename LIST::template TypeOf<3>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename LIST::template TypeOf<4>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename LIST::template TypeOf<5>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename LIST::template TypeOf<6>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename LIST::template TypeOf<7>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename LIST::template TypeOf<8>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename LIST::template TypeOf<9>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename LIST::template TypeOf<10>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename LIST::template TypeOf<11>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A2,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A3,
              typename bslmf::TypeListTypeOf<3, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A4,
              typename bslmf::TypeListTypeOf<4, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A5,
              typename bslmf::TypeListTypeOf<5, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A6,
              typename bslmf::TypeListTypeOf<6, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A7,
              typename bslmf::TypeListTypeOf<7, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A8,
              typename bslmf::TypeListTypeOf<8, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A9,
              typename bslmf::TypeListTypeOf<9, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A10,
             typename bslmf::TypeListTypeOf<10, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A11,
             typename bslmf::TypeListTypeOf<11, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<Nil,
             typename bslmf::TypeListTypeOf<12, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<Nil,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1,  class A2, class A3, class A4, class A5,
                      class A6,  class A7, class A8, class A9, class A10,
                      class A11, class A12>
struct ListAsserter12 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1,  typename LIST::Type1>::value));
        ASSERT(1 == (bsl::is_same<A2,  typename LIST::Type2>::value));
        ASSERT(1 == (bsl::is_same<A3,  typename LIST::Type3>::value));
        ASSERT(1 == (bsl::is_same<A4,  typename LIST::Type4>::value));
        ASSERT(1 == (bsl::is_same<A5,  typename LIST::Type5>::value));
        ASSERT(1 == (bsl::is_same<A6,  typename LIST::Type6>::value));
        ASSERT(1 == (bsl::is_same<A7,  typename LIST::Type7>::value));
        ASSERT(1 == (bsl::is_same<A8,  typename LIST::Type8>::value));
        ASSERT(1 == (bsl::is_same<A9,  typename LIST::Type9>::value));
        ASSERT(1 == (bsl::is_same<A10, typename LIST::Type10>::value));
        ASSERT(1 == (bsl::is_same<A11, typename LIST::Type11>::value));
        ASSERT(1 == (bsl::is_same<A12, typename LIST::Type12>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename bslmf::TypeListTypeOf<2, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename bslmf::TypeListTypeOf<3, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename bslmf::TypeListTypeOf<4, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename bslmf::TypeListTypeOf<5, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename bslmf::TypeListTypeOf<6, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename bslmf::TypeListTypeOf<7, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename bslmf::TypeListTypeOf<8, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename bslmf::TypeListTypeOf<9, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename bslmf::TypeListTypeOf<10, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename bslmf::TypeListTypeOf<11, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A12,
                     typename bslmf::TypeListTypeOf<12, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename LIST::template TypeOf<2>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename LIST::template TypeOf<3>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename LIST::template TypeOf<4>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename LIST::template TypeOf<5>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename LIST::template TypeOf<6>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename LIST::template TypeOf<7>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename LIST::template TypeOf<8>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename LIST::template TypeOf<9>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename LIST::template TypeOf<10>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename LIST::template TypeOf<11>::Type>::value));
        ASSERT(1 == (bsl::is_same<A12,
                     typename LIST::template TypeOf<12>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A2,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A3,
              typename bslmf::TypeListTypeOf<3, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A4,
              typename bslmf::TypeListTypeOf<4, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A5,
              typename bslmf::TypeListTypeOf<5, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A6,
              typename bslmf::TypeListTypeOf<6, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A7,
              typename bslmf::TypeListTypeOf<7, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A8,
              typename bslmf::TypeListTypeOf<8, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A9,
              typename bslmf::TypeListTypeOf<9, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A10,
             typename bslmf::TypeListTypeOf<10, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A11,
             typename bslmf::TypeListTypeOf<11, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A12,
             typename bslmf::TypeListTypeOf<12, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A12,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<Nil,
             typename bslmf::TypeListTypeOf<13, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<Nil,
                   typename LIST::template TypeOf<13>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1,  class A2,  class A3, class A4, class A5,
                      class A6,  class A7,  class A8, class A9, class A10,
                      class A11, class A12, class A13>
struct ListAsserter13 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1,  typename LIST::Type1>::value));
        ASSERT(1 == (bsl::is_same<A2,  typename LIST::Type2>::value));
        ASSERT(1 == (bsl::is_same<A3,  typename LIST::Type3>::value));
        ASSERT(1 == (bsl::is_same<A4,  typename LIST::Type4>::value));
        ASSERT(1 == (bsl::is_same<A5,  typename LIST::Type5>::value));
        ASSERT(1 == (bsl::is_same<A6,  typename LIST::Type6>::value));
        ASSERT(1 == (bsl::is_same<A7,  typename LIST::Type7>::value));
        ASSERT(1 == (bsl::is_same<A8,  typename LIST::Type8>::value));
        ASSERT(1 == (bsl::is_same<A9,  typename LIST::Type9>::value));
        ASSERT(1 == (bsl::is_same<A10, typename LIST::Type10>::value));
        ASSERT(1 == (bsl::is_same<A11, typename LIST::Type11>::value));
        ASSERT(1 == (bsl::is_same<A12, typename LIST::Type12>::value));
        ASSERT(1 == (bsl::is_same<A13, typename LIST::Type13>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename bslmf::TypeListTypeOf<2, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename bslmf::TypeListTypeOf<3, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename bslmf::TypeListTypeOf<4, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename bslmf::TypeListTypeOf<5, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename bslmf::TypeListTypeOf<6, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename bslmf::TypeListTypeOf<7, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename bslmf::TypeListTypeOf<8, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename bslmf::TypeListTypeOf<9, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename bslmf::TypeListTypeOf<10, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename bslmf::TypeListTypeOf<11, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A12,
                     typename bslmf::TypeListTypeOf<12, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A13,
                     typename bslmf::TypeListTypeOf<13, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename LIST::template TypeOf<2>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename LIST::template TypeOf<3>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename LIST::template TypeOf<4>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename LIST::template TypeOf<5>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename LIST::template TypeOf<6>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename LIST::template TypeOf<7>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename LIST::template TypeOf<8>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename LIST::template TypeOf<9>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename LIST::template TypeOf<10>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename LIST::template TypeOf<11>::Type>::value));
        ASSERT(1 == (bsl::is_same<A12,
                     typename LIST::template TypeOf<12>::Type>::value));
        ASSERT(1 == (bsl::is_same<A13,
                     typename LIST::template TypeOf<13>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A2,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A3,
              typename bslmf::TypeListTypeOf<3, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A4,
              typename bslmf::TypeListTypeOf<4, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A5,
              typename bslmf::TypeListTypeOf<5, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A6,
              typename bslmf::TypeListTypeOf<6, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A7,
              typename bslmf::TypeListTypeOf<7, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A8,
              typename bslmf::TypeListTypeOf<8, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A9,
              typename bslmf::TypeListTypeOf<9, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A10,
             typename bslmf::TypeListTypeOf<10, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A11,
             typename bslmf::TypeListTypeOf<11, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A12,
             typename bslmf::TypeListTypeOf<12, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A13,
             typename bslmf::TypeListTypeOf<13, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A12,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A13,
                   typename LIST::template TypeOf<13>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<Nil,
             typename bslmf::TypeListTypeOf<14, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<Nil,
                   typename LIST::template TypeOf<14>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1,  class A2,  class A3,  class A4, class A5,
                      class A6,  class A7,  class A8,  class A9, class A10,
                      class A11, class A12, class A13, class A14>
struct ListAsserter14 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1,  typename LIST::Type1>::value));
        ASSERT(1 == (bsl::is_same<A2,  typename LIST::Type2>::value));
        ASSERT(1 == (bsl::is_same<A3,  typename LIST::Type3>::value));
        ASSERT(1 == (bsl::is_same<A4,  typename LIST::Type4>::value));
        ASSERT(1 == (bsl::is_same<A5,  typename LIST::Type5>::value));
        ASSERT(1 == (bsl::is_same<A6,  typename LIST::Type6>::value));
        ASSERT(1 == (bsl::is_same<A7,  typename LIST::Type7>::value));
        ASSERT(1 == (bsl::is_same<A8,  typename LIST::Type8>::value));
        ASSERT(1 == (bsl::is_same<A9,  typename LIST::Type9>::value));
        ASSERT(1 == (bsl::is_same<A10, typename LIST::Type10>::value));
        ASSERT(1 == (bsl::is_same<A11, typename LIST::Type11>::value));
        ASSERT(1 == (bsl::is_same<A12, typename LIST::Type12>::value));
        ASSERT(1 == (bsl::is_same<A13, typename LIST::Type13>::value));
        ASSERT(1 == (bsl::is_same<A14, typename LIST::Type14>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename bslmf::TypeListTypeOf<2, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename bslmf::TypeListTypeOf<3, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename bslmf::TypeListTypeOf<4, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename bslmf::TypeListTypeOf<5, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename bslmf::TypeListTypeOf<6, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename bslmf::TypeListTypeOf<7, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename bslmf::TypeListTypeOf<8, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename bslmf::TypeListTypeOf<9, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename bslmf::TypeListTypeOf<10, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename bslmf::TypeListTypeOf<11, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A12,
                     typename bslmf::TypeListTypeOf<12, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A13,
                     typename bslmf::TypeListTypeOf<13, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A14,
                     typename bslmf::TypeListTypeOf<14, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename LIST::template TypeOf<2>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename LIST::template TypeOf<3>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename LIST::template TypeOf<4>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename LIST::template TypeOf<5>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename LIST::template TypeOf<6>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename LIST::template TypeOf<7>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename LIST::template TypeOf<8>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename LIST::template TypeOf<9>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename LIST::template TypeOf<10>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename LIST::template TypeOf<11>::Type>::value));
        ASSERT(1 == (bsl::is_same<A12,
                     typename LIST::template TypeOf<12>::Type>::value));
        ASSERT(1 == (bsl::is_same<A13,
                     typename LIST::template TypeOf<13>::Type>::value));
        ASSERT(1 == (bsl::is_same<A14,
                     typename LIST::template TypeOf<14>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A2,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A3,
              typename bslmf::TypeListTypeOf<3, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A4,
              typename bslmf::TypeListTypeOf<4, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A5,
              typename bslmf::TypeListTypeOf<5, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A6,
              typename bslmf::TypeListTypeOf<6, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A7,
              typename bslmf::TypeListTypeOf<7, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A8,
              typename bslmf::TypeListTypeOf<8, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A9,
              typename bslmf::TypeListTypeOf<9, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A10,
             typename bslmf::TypeListTypeOf<10, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A11,
             typename bslmf::TypeListTypeOf<11, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A12,
             typename bslmf::TypeListTypeOf<12, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A13,
             typename bslmf::TypeListTypeOf<13, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A14,
             typename bslmf::TypeListTypeOf<14, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A12,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A13,
                   typename LIST::template TypeOf<13>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A14,
                   typename LIST::template TypeOf<14>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<Nil,
             typename bslmf::TypeListTypeOf<15, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<Nil,
                   typename LIST::template TypeOf<15>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1,  class A2,  class A3,  class A4,  class A5,
                      class A6,  class A7,  class A8,  class A9,  class A10,
                      class A11, class A12, class A13, class A14, class A15>
struct ListAsserter15 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1,  typename LIST::Type1>::value));
        ASSERT(1 == (bsl::is_same<A2,  typename LIST::Type2>::value));
        ASSERT(1 == (bsl::is_same<A3,  typename LIST::Type3>::value));
        ASSERT(1 == (bsl::is_same<A4,  typename LIST::Type4>::value));
        ASSERT(1 == (bsl::is_same<A5,  typename LIST::Type5>::value));
        ASSERT(1 == (bsl::is_same<A6,  typename LIST::Type6>::value));
        ASSERT(1 == (bsl::is_same<A7,  typename LIST::Type7>::value));
        ASSERT(1 == (bsl::is_same<A8,  typename LIST::Type8>::value));
        ASSERT(1 == (bsl::is_same<A9,  typename LIST::Type9>::value));
        ASSERT(1 == (bsl::is_same<A10, typename LIST::Type10>::value));
        ASSERT(1 == (bsl::is_same<A11, typename LIST::Type11>::value));
        ASSERT(1 == (bsl::is_same<A12, typename LIST::Type12>::value));
        ASSERT(1 == (bsl::is_same<A13, typename LIST::Type13>::value));
        ASSERT(1 == (bsl::is_same<A14, typename LIST::Type14>::value));
        ASSERT(1 == (bsl::is_same<A15, typename LIST::Type15>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename bslmf::TypeListTypeOf<2, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename bslmf::TypeListTypeOf<3, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename bslmf::TypeListTypeOf<4, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename bslmf::TypeListTypeOf<5, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename bslmf::TypeListTypeOf<6, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename bslmf::TypeListTypeOf<7, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename bslmf::TypeListTypeOf<8, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename bslmf::TypeListTypeOf<9, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename bslmf::TypeListTypeOf<10, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename bslmf::TypeListTypeOf<11, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A12,
                     typename bslmf::TypeListTypeOf<12, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A13,
                     typename bslmf::TypeListTypeOf<13, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A14,
                     typename bslmf::TypeListTypeOf<14, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A15,
                     typename bslmf::TypeListTypeOf<15, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename LIST::template TypeOf<2>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename LIST::template TypeOf<3>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename LIST::template TypeOf<4>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename LIST::template TypeOf<5>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename LIST::template TypeOf<6>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename LIST::template TypeOf<7>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename LIST::template TypeOf<8>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename LIST::template TypeOf<9>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename LIST::template TypeOf<10>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename LIST::template TypeOf<11>::Type>::value));
        ASSERT(1 == (bsl::is_same<A12,
                     typename LIST::template TypeOf<12>::Type>::value));
        ASSERT(1 == (bsl::is_same<A13,
                     typename LIST::template TypeOf<13>::Type>::value));
        ASSERT(1 == (bsl::is_same<A14,
                     typename LIST::template TypeOf<14>::Type>::value));
        ASSERT(1 == (bsl::is_same<A15,
                     typename LIST::template TypeOf<15>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A2,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A3,
              typename bslmf::TypeListTypeOf<3, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A4,
              typename bslmf::TypeListTypeOf<4, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A5,
              typename bslmf::TypeListTypeOf<5, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A6,
              typename bslmf::TypeListTypeOf<6, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A7,
              typename bslmf::TypeListTypeOf<7, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A8,
              typename bslmf::TypeListTypeOf<8, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A9,
              typename bslmf::TypeListTypeOf<9, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A10,
             typename bslmf::TypeListTypeOf<10, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A11,
             typename bslmf::TypeListTypeOf<11, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A12,
             typename bslmf::TypeListTypeOf<12, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A13,
             typename bslmf::TypeListTypeOf<13, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A14,
             typename bslmf::TypeListTypeOf<14, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A15,
             typename bslmf::TypeListTypeOf<15, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A12,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A13,
                   typename LIST::template TypeOf<13>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A14,
                   typename LIST::template TypeOf<14>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A15,
                   typename LIST::template TypeOf<15>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<Nil,
             typename bslmf::TypeListTypeOf<16, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<Nil,
                   typename LIST::template TypeOf<16>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1,  class A2,  class A3,  class A4,  class A5,
                      class A6,  class A7,  class A8,  class A9,  class A10,
                      class A11, class A12, class A13, class A14, class A15,
                      class A16>
struct ListAsserter16 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1,  typename LIST::Type1>::value));
        ASSERT(1 == (bsl::is_same<A2,  typename LIST::Type2>::value));
        ASSERT(1 == (bsl::is_same<A3,  typename LIST::Type3>::value));
        ASSERT(1 == (bsl::is_same<A4,  typename LIST::Type4>::value));
        ASSERT(1 == (bsl::is_same<A5,  typename LIST::Type5>::value));
        ASSERT(1 == (bsl::is_same<A6,  typename LIST::Type6>::value));
        ASSERT(1 == (bsl::is_same<A7,  typename LIST::Type7>::value));
        ASSERT(1 == (bsl::is_same<A8,  typename LIST::Type8>::value));
        ASSERT(1 == (bsl::is_same<A9,  typename LIST::Type9>::value));
        ASSERT(1 == (bsl::is_same<A10, typename LIST::Type10>::value));
        ASSERT(1 == (bsl::is_same<A11, typename LIST::Type11>::value));
        ASSERT(1 == (bsl::is_same<A12, typename LIST::Type12>::value));
        ASSERT(1 == (bsl::is_same<A13, typename LIST::Type13>::value));
        ASSERT(1 == (bsl::is_same<A14, typename LIST::Type14>::value));
        ASSERT(1 == (bsl::is_same<A15, typename LIST::Type15>::value));
        ASSERT(1 == (bsl::is_same<A16, typename LIST::Type16>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename bslmf::TypeListTypeOf<2, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename bslmf::TypeListTypeOf<3, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename bslmf::TypeListTypeOf<4, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename bslmf::TypeListTypeOf<5, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename bslmf::TypeListTypeOf<6, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename bslmf::TypeListTypeOf<7, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename bslmf::TypeListTypeOf<8, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename bslmf::TypeListTypeOf<9, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename bslmf::TypeListTypeOf<10, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename bslmf::TypeListTypeOf<11, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A12,
                     typename bslmf::TypeListTypeOf<12, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A13,
                     typename bslmf::TypeListTypeOf<13, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A14,
                     typename bslmf::TypeListTypeOf<14, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A15,
                     typename bslmf::TypeListTypeOf<15, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A16,
                     typename bslmf::TypeListTypeOf<16, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename LIST::template TypeOf<2>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename LIST::template TypeOf<3>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename LIST::template TypeOf<4>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename LIST::template TypeOf<5>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename LIST::template TypeOf<6>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename LIST::template TypeOf<7>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename LIST::template TypeOf<8>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename LIST::template TypeOf<9>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename LIST::template TypeOf<10>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename LIST::template TypeOf<11>::Type>::value));
        ASSERT(1 == (bsl::is_same<A12,
                     typename LIST::template TypeOf<12>::Type>::value));
        ASSERT(1 == (bsl::is_same<A13,
                     typename LIST::template TypeOf<13>::Type>::value));
        ASSERT(1 == (bsl::is_same<A14,
                     typename LIST::template TypeOf<14>::Type>::value));
        ASSERT(1 == (bsl::is_same<A15,
                     typename LIST::template TypeOf<15>::Type>::value));
        ASSERT(1 == (bsl::is_same<A16,
                     typename LIST::template TypeOf<16>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A2,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A3,
              typename bslmf::TypeListTypeOf<3, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A4,
              typename bslmf::TypeListTypeOf<4, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A5,
              typename bslmf::TypeListTypeOf<5, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A6,
              typename bslmf::TypeListTypeOf<6, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A7,
              typename bslmf::TypeListTypeOf<7, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A8,
              typename bslmf::TypeListTypeOf<8, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A9,
              typename bslmf::TypeListTypeOf<9, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A10,
             typename bslmf::TypeListTypeOf<10, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A11,
             typename bslmf::TypeListTypeOf<11, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A12,
             typename bslmf::TypeListTypeOf<12, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A13,
             typename bslmf::TypeListTypeOf<13, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A14,
             typename bslmf::TypeListTypeOf<14, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A15,
             typename bslmf::TypeListTypeOf<15, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A16,
             typename bslmf::TypeListTypeOf<16, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A12,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A13,
                   typename LIST::template TypeOf<13>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A14,
                   typename LIST::template TypeOf<14>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A15,
                   typename LIST::template TypeOf<15>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A16,
                   typename LIST::template TypeOf<16>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<Nil,
             typename bslmf::TypeListTypeOf<17, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<Nil,
                   typename LIST::template TypeOf<17>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1,  class A2,  class A3,  class A4,  class A5,
                      class A6,  class A7,  class A8,  class A9,  class A10,
                      class A11, class A12, class A13, class A14, class A15,
                      class A16, class A17>
struct ListAsserter17 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1,  typename LIST::Type1>::value));
        ASSERT(1 == (bsl::is_same<A2,  typename LIST::Type2>::value));
        ASSERT(1 == (bsl::is_same<A3,  typename LIST::Type3>::value));
        ASSERT(1 == (bsl::is_same<A4,  typename LIST::Type4>::value));
        ASSERT(1 == (bsl::is_same<A5,  typename LIST::Type5>::value));
        ASSERT(1 == (bsl::is_same<A6,  typename LIST::Type6>::value));
        ASSERT(1 == (bsl::is_same<A7,  typename LIST::Type7>::value));
        ASSERT(1 == (bsl::is_same<A8,  typename LIST::Type8>::value));
        ASSERT(1 == (bsl::is_same<A9,  typename LIST::Type9>::value));
        ASSERT(1 == (bsl::is_same<A10, typename LIST::Type10>::value));
        ASSERT(1 == (bsl::is_same<A11, typename LIST::Type11>::value));
        ASSERT(1 == (bsl::is_same<A12, typename LIST::Type12>::value));
        ASSERT(1 == (bsl::is_same<A13, typename LIST::Type13>::value));
        ASSERT(1 == (bsl::is_same<A14, typename LIST::Type14>::value));
        ASSERT(1 == (bsl::is_same<A15, typename LIST::Type15>::value));
        ASSERT(1 == (bsl::is_same<A16, typename LIST::Type16>::value));
        ASSERT(1 == (bsl::is_same<A17, typename LIST::Type17>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename bslmf::TypeListTypeOf<2, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename bslmf::TypeListTypeOf<3, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename bslmf::TypeListTypeOf<4, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename bslmf::TypeListTypeOf<5, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename bslmf::TypeListTypeOf<6, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename bslmf::TypeListTypeOf<7, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename bslmf::TypeListTypeOf<8, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename bslmf::TypeListTypeOf<9, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename bslmf::TypeListTypeOf<10, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename bslmf::TypeListTypeOf<11, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A12,
                     typename bslmf::TypeListTypeOf<12, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A13,
                     typename bslmf::TypeListTypeOf<13, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A14,
                     typename bslmf::TypeListTypeOf<14, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A15,
                     typename bslmf::TypeListTypeOf<15, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A16,
                     typename bslmf::TypeListTypeOf<16, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A17,
                     typename bslmf::TypeListTypeOf<17, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename LIST::template TypeOf<2>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename LIST::template TypeOf<3>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename LIST::template TypeOf<4>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename LIST::template TypeOf<5>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename LIST::template TypeOf<6>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename LIST::template TypeOf<7>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename LIST::template TypeOf<8>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename LIST::template TypeOf<9>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename LIST::template TypeOf<10>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename LIST::template TypeOf<11>::Type>::value));
        ASSERT(1 == (bsl::is_same<A12,
                     typename LIST::template TypeOf<12>::Type>::value));
        ASSERT(1 == (bsl::is_same<A13,
                     typename LIST::template TypeOf<13>::Type>::value));
        ASSERT(1 == (bsl::is_same<A14,
                     typename LIST::template TypeOf<14>::Type>::value));
        ASSERT(1 == (bsl::is_same<A15,
                     typename LIST::template TypeOf<15>::Type>::value));
        ASSERT(1 == (bsl::is_same<A16,
                     typename LIST::template TypeOf<16>::Type>::value));
        ASSERT(1 == (bsl::is_same<A17,
                     typename LIST::template TypeOf<17>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A2,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A3,
              typename bslmf::TypeListTypeOf<3, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A4,
              typename bslmf::TypeListTypeOf<4, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A5,
              typename bslmf::TypeListTypeOf<5, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A6,
              typename bslmf::TypeListTypeOf<6, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A7,
              typename bslmf::TypeListTypeOf<7, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A8,
              typename bslmf::TypeListTypeOf<8, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A9,
              typename bslmf::TypeListTypeOf<9, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A10,
             typename bslmf::TypeListTypeOf<10, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A11,
             typename bslmf::TypeListTypeOf<11, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A12,
             typename bslmf::TypeListTypeOf<12, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A13,
             typename bslmf::TypeListTypeOf<13, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A14,
             typename bslmf::TypeListTypeOf<14, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A15,
             typename bslmf::TypeListTypeOf<15, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A16,
             typename bslmf::TypeListTypeOf<16, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A17,
             typename bslmf::TypeListTypeOf<17, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A12,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A13,
                   typename LIST::template TypeOf<13>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A14,
                   typename LIST::template TypeOf<14>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A15,
                   typename LIST::template TypeOf<15>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A16,
                   typename LIST::template TypeOf<16>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A17,
                   typename LIST::template TypeOf<17>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<Nil,
             typename bslmf::TypeListTypeOf<18, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<Nil,
                   typename LIST::template TypeOf<18>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1,  class A2,  class A3,  class A4,  class A5,
                      class A6,  class A7,  class A8,  class A9,  class A10,
                      class A11, class A12, class A13, class A14, class A15,
                      class A16, class A17, class A18>
struct ListAsserter18 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1,  typename LIST::Type1>::value));
        ASSERT(1 == (bsl::is_same<A2,  typename LIST::Type2>::value));
        ASSERT(1 == (bsl::is_same<A3,  typename LIST::Type3>::value));
        ASSERT(1 == (bsl::is_same<A4,  typename LIST::Type4>::value));
        ASSERT(1 == (bsl::is_same<A5,  typename LIST::Type5>::value));
        ASSERT(1 == (bsl::is_same<A6,  typename LIST::Type6>::value));
        ASSERT(1 == (bsl::is_same<A7,  typename LIST::Type7>::value));
        ASSERT(1 == (bsl::is_same<A8,  typename LIST::Type8>::value));
        ASSERT(1 == (bsl::is_same<A9,  typename LIST::Type9>::value));
        ASSERT(1 == (bsl::is_same<A10, typename LIST::Type10>::value));
        ASSERT(1 == (bsl::is_same<A11, typename LIST::Type11>::value));
        ASSERT(1 == (bsl::is_same<A12, typename LIST::Type12>::value));
        ASSERT(1 == (bsl::is_same<A13, typename LIST::Type13>::value));
        ASSERT(1 == (bsl::is_same<A14, typename LIST::Type14>::value));
        ASSERT(1 == (bsl::is_same<A15, typename LIST::Type15>::value));
        ASSERT(1 == (bsl::is_same<A16, typename LIST::Type16>::value));
        ASSERT(1 == (bsl::is_same<A17, typename LIST::Type17>::value));
        ASSERT(1 == (bsl::is_same<A18, typename LIST::Type18>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename bslmf::TypeListTypeOf<2, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename bslmf::TypeListTypeOf<3, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename bslmf::TypeListTypeOf<4, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename bslmf::TypeListTypeOf<5, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename bslmf::TypeListTypeOf<6, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename bslmf::TypeListTypeOf<7, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename bslmf::TypeListTypeOf<8, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename bslmf::TypeListTypeOf<9, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename bslmf::TypeListTypeOf<10, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename bslmf::TypeListTypeOf<11, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A12,
                     typename bslmf::TypeListTypeOf<12, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A13,
                     typename bslmf::TypeListTypeOf<13, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A14,
                     typename bslmf::TypeListTypeOf<14, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A15,
                     typename bslmf::TypeListTypeOf<15, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A16,
                     typename bslmf::TypeListTypeOf<16, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A17,
                     typename bslmf::TypeListTypeOf<17, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A18,
                     typename bslmf::TypeListTypeOf<18, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename LIST::template TypeOf<2>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename LIST::template TypeOf<3>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename LIST::template TypeOf<4>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename LIST::template TypeOf<5>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename LIST::template TypeOf<6>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename LIST::template TypeOf<7>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename LIST::template TypeOf<8>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename LIST::template TypeOf<9>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename LIST::template TypeOf<10>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename LIST::template TypeOf<11>::Type>::value));
        ASSERT(1 == (bsl::is_same<A12,
                     typename LIST::template TypeOf<12>::Type>::value));
        ASSERT(1 == (bsl::is_same<A13,
                     typename LIST::template TypeOf<13>::Type>::value));
        ASSERT(1 == (bsl::is_same<A14,
                     typename LIST::template TypeOf<14>::Type>::value));
        ASSERT(1 == (bsl::is_same<A15,
                     typename LIST::template TypeOf<15>::Type>::value));
        ASSERT(1 == (bsl::is_same<A16,
                     typename LIST::template TypeOf<16>::Type>::value));
        ASSERT(1 == (bsl::is_same<A17,
                     typename LIST::template TypeOf<17>::Type>::value));
        ASSERT(1 == (bsl::is_same<A18,
                     typename LIST::template TypeOf<18>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A2,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A3,
              typename bslmf::TypeListTypeOf<3, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A4,
              typename bslmf::TypeListTypeOf<4, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A5,
              typename bslmf::TypeListTypeOf<5, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A6,
              typename bslmf::TypeListTypeOf<6, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A7,
              typename bslmf::TypeListTypeOf<7, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A8,
              typename bslmf::TypeListTypeOf<8, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A9,
              typename bslmf::TypeListTypeOf<9, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A10,
             typename bslmf::TypeListTypeOf<10, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A11,
             typename bslmf::TypeListTypeOf<11, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A12,
             typename bslmf::TypeListTypeOf<12, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A13,
             typename bslmf::TypeListTypeOf<13, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A14,
             typename bslmf::TypeListTypeOf<14, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A15,
             typename bslmf::TypeListTypeOf<15, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A16,
             typename bslmf::TypeListTypeOf<16, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A17,
             typename bslmf::TypeListTypeOf<17, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A18,
             typename bslmf::TypeListTypeOf<18, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A12,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A13,
                   typename LIST::template TypeOf<13>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A14,
                   typename LIST::template TypeOf<14>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A15,
                   typename LIST::template TypeOf<15>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A16,
                   typename LIST::template TypeOf<16>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A17,
                   typename LIST::template TypeOf<17>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A18,
                   typename LIST::template TypeOf<18>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<Nil,
             typename bslmf::TypeListTypeOf<19, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<Nil,
                   typename LIST::template TypeOf<19>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1,  class A2,  class A3,  class A4,  class A5,
                      class A6,  class A7,  class A8,  class A9,  class A10,
                      class A11, class A12, class A13, class A14, class A15,
                      class A16, class A17, class A18, class A19>
struct ListAsserter19 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1,  typename LIST::Type1>::value));
        ASSERT(1 == (bsl::is_same<A2,  typename LIST::Type2>::value));
        ASSERT(1 == (bsl::is_same<A3,  typename LIST::Type3>::value));
        ASSERT(1 == (bsl::is_same<A4,  typename LIST::Type4>::value));
        ASSERT(1 == (bsl::is_same<A5,  typename LIST::Type5>::value));
        ASSERT(1 == (bsl::is_same<A6,  typename LIST::Type6>::value));
        ASSERT(1 == (bsl::is_same<A7,  typename LIST::Type7>::value));
        ASSERT(1 == (bsl::is_same<A8,  typename LIST::Type8>::value));
        ASSERT(1 == (bsl::is_same<A9,  typename LIST::Type9>::value));
        ASSERT(1 == (bsl::is_same<A10, typename LIST::Type10>::value));
        ASSERT(1 == (bsl::is_same<A11, typename LIST::Type11>::value));
        ASSERT(1 == (bsl::is_same<A12, typename LIST::Type12>::value));
        ASSERT(1 == (bsl::is_same<A13, typename LIST::Type13>::value));
        ASSERT(1 == (bsl::is_same<A14, typename LIST::Type14>::value));
        ASSERT(1 == (bsl::is_same<A15, typename LIST::Type15>::value));
        ASSERT(1 == (bsl::is_same<A16, typename LIST::Type16>::value));
        ASSERT(1 == (bsl::is_same<A17, typename LIST::Type17>::value));
        ASSERT(1 == (bsl::is_same<A18, typename LIST::Type18>::value));
        ASSERT(1 == (bsl::is_same<A19, typename LIST::Type19>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename bslmf::TypeListTypeOf<2, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename bslmf::TypeListTypeOf<3, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename bslmf::TypeListTypeOf<4, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename bslmf::TypeListTypeOf<5, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename bslmf::TypeListTypeOf<6, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename bslmf::TypeListTypeOf<7, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename bslmf::TypeListTypeOf<8, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename bslmf::TypeListTypeOf<9, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename bslmf::TypeListTypeOf<10, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename bslmf::TypeListTypeOf<11, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A12,
                     typename bslmf::TypeListTypeOf<12, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A13,
                     typename bslmf::TypeListTypeOf<13, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A14,
                     typename bslmf::TypeListTypeOf<14, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A15,
                     typename bslmf::TypeListTypeOf<15, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A16,
                     typename bslmf::TypeListTypeOf<16, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A17,
                     typename bslmf::TypeListTypeOf<17, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A18,
                     typename bslmf::TypeListTypeOf<18, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A19,
                     typename bslmf::TypeListTypeOf<19, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename LIST::template TypeOf<2>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename LIST::template TypeOf<3>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename LIST::template TypeOf<4>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename LIST::template TypeOf<5>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename LIST::template TypeOf<6>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename LIST::template TypeOf<7>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename LIST::template TypeOf<8>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename LIST::template TypeOf<9>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename LIST::template TypeOf<10>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename LIST::template TypeOf<11>::Type>::value));
        ASSERT(1 == (bsl::is_same<A12,
                     typename LIST::template TypeOf<12>::Type>::value));
        ASSERT(1 == (bsl::is_same<A13,
                     typename LIST::template TypeOf<13>::Type>::value));
        ASSERT(1 == (bsl::is_same<A14,
                     typename LIST::template TypeOf<14>::Type>::value));
        ASSERT(1 == (bsl::is_same<A15,
                     typename LIST::template TypeOf<15>::Type>::value));
        ASSERT(1 == (bsl::is_same<A16,
                     typename LIST::template TypeOf<16>::Type>::value));
        ASSERT(1 == (bsl::is_same<A17,
                     typename LIST::template TypeOf<17>::Type>::value));
        ASSERT(1 == (bsl::is_same<A18,
                     typename LIST::template TypeOf<18>::Type>::value));
        ASSERT(1 == (bsl::is_same<A19,
                     typename LIST::template TypeOf<19>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A2,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A3,
              typename bslmf::TypeListTypeOf<3, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A4,
              typename bslmf::TypeListTypeOf<4, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A5,
              typename bslmf::TypeListTypeOf<5, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A6,
              typename bslmf::TypeListTypeOf<6, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A7,
              typename bslmf::TypeListTypeOf<7, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A8,
              typename bslmf::TypeListTypeOf<8, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A9,
              typename bslmf::TypeListTypeOf<9, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A10,
             typename bslmf::TypeListTypeOf<10, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A11,
             typename bslmf::TypeListTypeOf<11, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A12,
             typename bslmf::TypeListTypeOf<12, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A13,
             typename bslmf::TypeListTypeOf<13, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A14,
             typename bslmf::TypeListTypeOf<14, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A15,
             typename bslmf::TypeListTypeOf<15, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A16,
             typename bslmf::TypeListTypeOf<16, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A17,
             typename bslmf::TypeListTypeOf<17, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A18,
             typename bslmf::TypeListTypeOf<18, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A19,
             typename bslmf::TypeListTypeOf<19, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A12,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A13,
                   typename LIST::template TypeOf<13>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A14,
                   typename LIST::template TypeOf<14>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A15,
                   typename LIST::template TypeOf<15>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A16,
                   typename LIST::template TypeOf<16>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A17,
                   typename LIST::template TypeOf<17>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A18,
                   typename LIST::template TypeOf<18>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A19,
                   typename LIST::template TypeOf<19>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<Nil,
             typename bslmf::TypeListTypeOf<20, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<Nil,
                   typename LIST::template TypeOf<20>::TypeOrDefault>::value));
    }
};

template <class LIST, class A1,  class A2,  class A3,  class A4,  class A5,
                      class A6,  class A7,  class A8,  class A9,  class A10,
                      class A11, class A12, class A13, class A14, class A15,
                      class A16, class A17, class A18, class A19, class A20>
struct ListAsserter20 {
    static void listAssert()
    {
        ASSERT(1 == (bsl::is_same<A1,  typename LIST::Type1>::value));
        ASSERT(1 == (bsl::is_same<A2,  typename LIST::Type2>::value));
        ASSERT(1 == (bsl::is_same<A3,  typename LIST::Type3>::value));
        ASSERT(1 == (bsl::is_same<A4,  typename LIST::Type4>::value));
        ASSERT(1 == (bsl::is_same<A5,  typename LIST::Type5>::value));
        ASSERT(1 == (bsl::is_same<A6,  typename LIST::Type6>::value));
        ASSERT(1 == (bsl::is_same<A7,  typename LIST::Type7>::value));
        ASSERT(1 == (bsl::is_same<A8,  typename LIST::Type8>::value));
        ASSERT(1 == (bsl::is_same<A9,  typename LIST::Type9>::value));
        ASSERT(1 == (bsl::is_same<A10, typename LIST::Type10>::value));
        ASSERT(1 == (bsl::is_same<A11, typename LIST::Type11>::value));
        ASSERT(1 == (bsl::is_same<A12, typename LIST::Type12>::value));
        ASSERT(1 == (bsl::is_same<A13, typename LIST::Type13>::value));
        ASSERT(1 == (bsl::is_same<A14, typename LIST::Type14>::value));
        ASSERT(1 == (bsl::is_same<A15, typename LIST::Type15>::value));
        ASSERT(1 == (bsl::is_same<A16, typename LIST::Type16>::value));
        ASSERT(1 == (bsl::is_same<A17, typename LIST::Type17>::value));
        ASSERT(1 == (bsl::is_same<A18, typename LIST::Type18>::value));
        ASSERT(1 == (bsl::is_same<A19, typename LIST::Type19>::value));
        ASSERT(1 == (bsl::is_same<A20, typename LIST::Type20>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename bslmf::TypeListTypeOf<1, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename bslmf::TypeListTypeOf<2, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename bslmf::TypeListTypeOf<3, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename bslmf::TypeListTypeOf<4, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename bslmf::TypeListTypeOf<5, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename bslmf::TypeListTypeOf<6, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename bslmf::TypeListTypeOf<7, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename bslmf::TypeListTypeOf<8, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename bslmf::TypeListTypeOf<9, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename bslmf::TypeListTypeOf<10, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename bslmf::TypeListTypeOf<11, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A12,
                     typename bslmf::TypeListTypeOf<12, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A13,
                     typename bslmf::TypeListTypeOf<13, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A14,
                     typename bslmf::TypeListTypeOf<14, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A15,
                     typename bslmf::TypeListTypeOf<15, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A16,
                     typename bslmf::TypeListTypeOf<16, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A17,
                     typename bslmf::TypeListTypeOf<17, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A18,
                     typename bslmf::TypeListTypeOf<18, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A19,
                     typename bslmf::TypeListTypeOf<19, LIST>::Type>::value));
        ASSERT(1 == (bsl::is_same<A20,
                     typename bslmf::TypeListTypeOf<20, LIST>::Type>::value));

        ASSERT(1 == (bsl::is_same<A1,
                     typename LIST::template TypeOf<1>::Type>::value));
        ASSERT(1 == (bsl::is_same<A2,
                     typename LIST::template TypeOf<2>::Type>::value));
        ASSERT(1 == (bsl::is_same<A3,
                     typename LIST::template TypeOf<3>::Type>::value));
        ASSERT(1 == (bsl::is_same<A4,
                     typename LIST::template TypeOf<4>::Type>::value));
        ASSERT(1 == (bsl::is_same<A5,
                     typename LIST::template TypeOf<5>::Type>::value));
        ASSERT(1 == (bsl::is_same<A6,
                     typename LIST::template TypeOf<6>::Type>::value));
        ASSERT(1 == (bsl::is_same<A7,
                     typename LIST::template TypeOf<7>::Type>::value));
        ASSERT(1 == (bsl::is_same<A8,
                     typename LIST::template TypeOf<8>::Type>::value));
        ASSERT(1 == (bsl::is_same<A9,
                     typename LIST::template TypeOf<9>::Type>::value));
        ASSERT(1 == (bsl::is_same<A10,
                     typename LIST::template TypeOf<10>::Type>::value));
        ASSERT(1 == (bsl::is_same<A11,
                     typename LIST::template TypeOf<11>::Type>::value));
        ASSERT(1 == (bsl::is_same<A12,
                     typename LIST::template TypeOf<12>::Type>::value));
        ASSERT(1 == (bsl::is_same<A13,
                     typename LIST::template TypeOf<13>::Type>::value));
        ASSERT(1 == (bsl::is_same<A14,
                     typename LIST::template TypeOf<14>::Type>::value));
        ASSERT(1 == (bsl::is_same<A15,
                     typename LIST::template TypeOf<15>::Type>::value));
        ASSERT(1 == (bsl::is_same<A16,
                     typename LIST::template TypeOf<16>::Type>::value));
        ASSERT(1 == (bsl::is_same<A17,
                     typename LIST::template TypeOf<17>::Type>::value));
        ASSERT(1 == (bsl::is_same<A18,
                     typename LIST::template TypeOf<18>::Type>::value));
        ASSERT(1 == (bsl::is_same<A19,
                     typename LIST::template TypeOf<19>::Type>::value));
        ASSERT(1 == (bsl::is_same<A20,
                     typename LIST::template TypeOf<20>::Type>::value));

        ASSERT(1 == (
              bsl::is_same<A1,
              typename bslmf::TypeListTypeOf<1, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A2,
              typename bslmf::TypeListTypeOf<2, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A3,
              typename bslmf::TypeListTypeOf<3, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A4,
              typename bslmf::TypeListTypeOf<4, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A5,
              typename bslmf::TypeListTypeOf<5, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A6,
              typename bslmf::TypeListTypeOf<6, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A7,
              typename bslmf::TypeListTypeOf<7, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A8,
              typename bslmf::TypeListTypeOf<8, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
              bsl::is_same<A9,
              typename bslmf::TypeListTypeOf<9, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A10,
             typename bslmf::TypeListTypeOf<10, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A11,
             typename bslmf::TypeListTypeOf<11, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A12,
             typename bslmf::TypeListTypeOf<12, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A13,
             typename bslmf::TypeListTypeOf<13, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A14,
             typename bslmf::TypeListTypeOf<14, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A15,
             typename bslmf::TypeListTypeOf<15, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A16,
             typename bslmf::TypeListTypeOf<16, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A17,
             typename bslmf::TypeListTypeOf<17, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A18,
             typename bslmf::TypeListTypeOf<18, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A19,
             typename bslmf::TypeListTypeOf<19, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<A20,
             typename bslmf::TypeListTypeOf<20, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::value));
        ASSERT(1 == (
                    bsl::is_same<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A12,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A13,
                   typename LIST::template TypeOf<13>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A14,
                   typename LIST::template TypeOf<14>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A15,
                   typename LIST::template TypeOf<15>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A16,
                   typename LIST::template TypeOf<16>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A17,
                   typename LIST::template TypeOf<17>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A18,
                   typename LIST::template TypeOf<18>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A19,
                   typename LIST::template TypeOf<19>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<A20,
                   typename LIST::template TypeOf<20>::TypeOrDefault>::value));

        // Test out-of-bounds indices.

        ASSERT(1 == (
              bsl::is_same<Nil,
              typename bslmf::TypeListTypeOf<0, LIST>::TypeOrDefault>::value));
        ASSERT(1 == (
             bsl::is_same<Nil,
             typename bslmf::TypeListTypeOf<21, LIST>::TypeOrDefault>::value));

        ASSERT(1 == (
                    bsl::is_same<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::value));
        ASSERT(1 == (
                   bsl::is_same<Nil,
                   typename LIST::template TypeOf<21>::TypeOrDefault>::value));
    }
};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    (void)veryVerbose;
    (void)veryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // ------------------------------------------------------------------
        // TESTING BSLMF_TYPELIST0 .. BSLMF_TYPELIST20
        //
        // Plan:
        //
        // Testing:
        //   bslmf::TypeList0
        //   bslmf::TypeList1
        //   bslmf::TypeList2
        //   bslmf::TypeList3
        //   bslmf::TypeList4
        //   bslmf::TypeList5
        //   bslmf::TypeList6
        //   bslmf::TypeList7
        //   bslmf::TypeList8
        //   bslmf::TypeList9
        //   bslmf::TypeList10
        //   bslmf::TypeList11
        //   bslmf::TypeList12
        //   bslmf::TypeList13
        //   bslmf::TypeList14
        //   bslmf::TypeList15
        //   bslmf::TypeList16
        //   bslmf::TypeList17
        //   bslmf::TypeList18
        //   bslmf::TypeList19
        //   bslmf::TypeList20
        // ------------------------------------------------------------------

        if (verbose) printf("\nTESTING BSLMF_TYPELIST0 .. BSLMF_TYPELIST20"
                            "\n===========================================\n");

        typedef bslmf::TypeList0 LIST0;
        ASSERT(0 == LIST0::LENGTH);
        ListAsserter0<LIST0>::listAssert();

        typedef bslmf::TypeList1<T1> LIST1;
        ASSERT(1 == LIST1::LENGTH);
        ListAsserter1<LIST1,T1>::listAssert();

        typedef bslmf::TypeList2<T1,T2> LIST2;
        ASSERT(2 == LIST2::LENGTH);
        ListAsserter2<LIST2,T1,T2>::listAssert();

        typedef bslmf::TypeList3<T1,T2,T3> LIST3;
        ASSERT(3 == LIST3::LENGTH);
        ListAsserter3<LIST3,T1,T2,T3>::listAssert();

        typedef bslmf::TypeList4<T1,T2,T3,T4> LIST4;
        ASSERT(4 == LIST4::LENGTH);
        ListAsserter4<LIST4,T1,T2,T3,T4>::listAssert();

        typedef bslmf::TypeList5<T1,T2,T3,T4,T5> LIST5;
        ASSERT(5 == LIST5::LENGTH);
        ListAsserter5<LIST5,T1,T2,T3,T4,T5>::listAssert();

        typedef bslmf::TypeList6<T1,T2,T3,T4,T5,T6> LIST6;
        ASSERT(6 == LIST6::LENGTH);
        ListAsserter6<LIST6,T1,T2,T3,T4,T5,T6>::listAssert();

        typedef bslmf::TypeList7<T1,T2,T3,T4,T5,T6,T7> LIST7;
        ASSERT(7 == LIST7::LENGTH);
        ListAsserter7<LIST7,T1,T2,T3,T4,T5,T6,T7>::listAssert();

        typedef bslmf::TypeList8<T1,T2,T3,T4,T5,T6,T7,T8> LIST8;
        ASSERT(8 == LIST8::LENGTH);
        ListAsserter8<LIST8,T1,T2,T3,T4,T5,T6,T7,T8>::listAssert();

        typedef bslmf::TypeList9<T1,T2,T3,T4,T5,T6,T7,T8,T9> LIST9;
        ASSERT(9 == LIST9::LENGTH);
        ListAsserter9<LIST9,T1,T2,T3,T4,T5,T6,T7,T8,T9>::listAssert();

        typedef bslmf::TypeList10<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10> LIST10;
        ASSERT(10 == LIST10::LENGTH);
        ListAsserter10<LIST10,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>::listAssert();

        typedef bslmf::TypeList11<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11> LIST11;
        ASSERT(11 == LIST11::LENGTH);
        ListAsserter11<LIST11,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::
                                                                  listAssert();

        typedef bslmf::TypeList12<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>
                                                                        LIST12;
        ASSERT(12 == LIST12::LENGTH);
        ListAsserter12<LIST12,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>::
                                                                  listAssert();

        typedef bslmf::TypeList13<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13>
                                                                        LIST13;
        ASSERT(13 == LIST13::LENGTH);
        ListAsserter13<LIST13,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13>::
                                                                  listAssert();

        typedef bslmf::TypeList14<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,
                                                                   T14> LIST14;
        ASSERT(14 == LIST14::LENGTH);
        ListAsserter14<LIST14,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,
                                                            T14>::listAssert();

        typedef bslmf::TypeList15<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,
                                                               T14,T15> LIST15;
        ASSERT(15 == LIST15::LENGTH);
        ListAsserter15<LIST15,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                            T15>::listAssert();

        typedef bslmf::TypeList16<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,
                                                           T14,T15,T16> LIST16;
        ASSERT(16 == LIST16::LENGTH);
        ListAsserter16<LIST16,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                        T15,T16>::listAssert();

        typedef bslmf::TypeList17<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,
                                                       T14,T15,T16,T17> LIST17;
        ASSERT(17 == LIST17::LENGTH);
        ListAsserter17<LIST17,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                    T15,T16,T17>::listAssert();

        typedef bslmf::TypeList18<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,
                                                   T14,T15,T16,T17,T18> LIST18;
        ASSERT(18 == LIST18::LENGTH);
        ListAsserter18<LIST18,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                T15,T16,T17,T18>::listAssert();

        typedef bslmf::TypeList19<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,
                                               T14,T15,T16,T17,T18,T19> LIST19;
        ASSERT(19 == LIST19::LENGTH);
        ListAsserter19<LIST19,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                            T15,T16,T17,T18,T19>::listAssert();

        typedef bslmf::TypeList20<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,
                                           T14,T15,T16,T17,T18,T19,T20> LIST20;
        ASSERT(20 == LIST20::LENGTH);
        ListAsserter20<LIST20,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                        T15,T16,T17,T18,T19,T20>::listAssert();

      } break;
      case 2: {
        // ------------------------------------------------------------------
        // TESTING BSLMF_TYPELIST
        //
        // Plan:
        //
        // Testing:
        //   bslmf::TypeList
        //   bslmf::TypeListTypeOf
        // ------------------------------------------------------------------

        if (verbose) printf("\nTESTING BSLMF_TYPELIST"
                            "\n======================\n");

        typedef bslmf::TypeList<> LIST0;
        ASSERT(0 == LIST0::LENGTH);
        ListAsserter0<LIST0>::listAssert();

        typedef bslmf::TypeList<T1> LIST1;
        ASSERT(1 == LIST1::LENGTH);
        ListAsserter1<LIST1,T1>::listAssert();

        typedef bslmf::TypeList<T1,T2> LIST2;
        ASSERT(2 == LIST2::LENGTH);
        ListAsserter2<LIST2,T1,T2>::listAssert();

        typedef bslmf::TypeList<T1,T2,T3> LIST3;
        ASSERT(3 == LIST3::LENGTH);
        ListAsserter3<LIST3,T1,T2,T3>::listAssert();

        typedef bslmf::TypeList<T1,T2,T3,T4> LIST4;
        ASSERT(4 == LIST4::LENGTH);
        ListAsserter4<LIST4,T1,T2,T3,T4>::listAssert();

        typedef bslmf::TypeList<T1,T2,T3,T4,T5> LIST5;
        ASSERT(5 == LIST5::LENGTH);
        ListAsserter5<LIST5,T1,T2,T3,T4,T5>::listAssert();

        typedef bslmf::TypeList<T1,T2,T3,T4,T5,T6> LIST6;
        ASSERT(6 == LIST6::LENGTH);
        ListAsserter6<LIST6,T1,T2,T3,T4,T5,T6>::listAssert();

        typedef bslmf::TypeList<T1,T2,T3,T4,T5,T6,T7> LIST7;
        ASSERT(7 == LIST7::LENGTH);
        ListAsserter7<LIST7,T1,T2,T3,T4,T5,T6,T7>::listAssert();

        typedef bslmf::TypeList<T1,T2,T3,T4,T5,T6,T7,T8> LIST8;
        ASSERT(8 == LIST8::LENGTH);
        ListAsserter8<LIST8,T1,T2,T3,T4,T5,T6,T7,T8>::listAssert();

        typedef bslmf::TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9> LIST9;
        ASSERT(9 == LIST9::LENGTH);
        ListAsserter9<LIST9,T1,T2,T3,T4,T5,T6,T7,T8,T9>::listAssert();

        typedef bslmf::TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10> LIST10;
        ASSERT(10 == LIST10::LENGTH);
        ListAsserter10<LIST10,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>::listAssert();

        typedef bslmf::TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11> LIST11;
        ASSERT(11 == LIST11::LENGTH);
        ListAsserter11<LIST11,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::
                                                                  listAssert();

        typedef bslmf::TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12> LIST12;
        ASSERT(12 == LIST12::LENGTH);
        ListAsserter12<LIST12,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>::
                                                                  listAssert();

        typedef bslmf::TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13>
                                                                        LIST13;
        ASSERT(13 == LIST13::LENGTH);
        ListAsserter13<LIST13,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13>::
                                                                  listAssert();

        typedef bslmf::TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14>
                                                                        LIST14;
        ASSERT(14 == LIST14::LENGTH);
        ListAsserter14<LIST14,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14>::
                                                                  listAssert();

        typedef bslmf::TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                                   T15> LIST15;
        ASSERT(15 == LIST15::LENGTH);
        ListAsserter15<LIST15,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                            T15>::listAssert();

        typedef bslmf::TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                               T15,T16> LIST16;
        ASSERT(16 == LIST16::LENGTH);
        ListAsserter16<LIST16,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                        T15,T16>::listAssert();

        typedef bslmf::TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                           T15,T16,T17> LIST17;
        ASSERT(17 == LIST17::LENGTH);
        ListAsserter17<LIST17,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                    T15,T16,T17>::listAssert();

        typedef bslmf::TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                       T15,T16,T17,T18> LIST18;
        ASSERT(18 == LIST18::LENGTH);
        ListAsserter18<LIST18,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                T15,T16,T17,T18>::listAssert();

        typedef bslmf::TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                   T15,T16,T17,T18,T19> LIST19;
        ASSERT(19 == LIST19::LENGTH);
        ListAsserter19<LIST19,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                            T15,T16,T17,T18,T19>::listAssert();

        typedef bslmf::TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                               T15,T16,T17,T18,T19,T20> LIST20;
        ASSERT(20 == LIST20::LENGTH);
        ListAsserter20<LIST20,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                        T15,T16,T17,T18,T19,T20>::listAssert();

      } break;
      case 1: {
        // ------------------------------------------------------------------
        // BREATHING TEST
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // ------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        {
            typedef int    INT;
            typedef double DOUBLE;
            ASSERT(1 == (bsl::is_same<INT, INT>::value));
            ASSERT(0 == (bsl::is_same<INT, DOUBLE>::value));

            ASSERT(1 == (bsl::is_same<T1, T1>::value));
            ASSERT(0 == (bsl::is_same<T1, T2>::value));
        }

        {
            typedef bslmf::TypeList<T1, T2> LIST;
            ASSERT(2 == LIST::LENGTH);

            ASSERT(1 == (bsl::is_same<T1, LIST::Type1>::value));
            ASSERT(1 == (bsl::is_same<T2, LIST::Type2>::value));

            ASSERT(1 == (bsl::is_same<T1,
                         bslmf::TypeListTypeOf<1, LIST>::Type>::value));

            ASSERT(1 == (bsl::is_same<T2, LIST::TypeOf<2>::Type>::value));
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
