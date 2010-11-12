// bdemf_typelist.t.cpp            -*-C++-*-

#include <bdemf_typelist.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>    // atoi()

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//  This test driver verifies each of the 21 typelist template classes provided
//  by 'bdemf_typelist'.  Each template is instantiated with the appropriate
//  number of distict types.  Each type will be test to ensure that the type
//  defined by it's corresponding Type<N> typedef and TypeOf<N> typedef are
//  correct.
//-----------------------------------------------------------------------------
// [ 2] bdemf_TypeList
// [ 2] bdemf_TypeListTypeOf
// [ 3] bdemf_TypeList0
// [ 3] bdemf_TypeList1
// [ 3] bdemf_TypeList2
// [ 3] bdemf_TypeList3
// [ 3] bdemf_TypeList4
// [ 3] bdemf_TypeList5
// [ 3] bdemf_TypeList6
// [ 3] bdemf_TypeList7
// [ 3] bdemf_TypeList8
// [ 3] bdemf_TypeList9
// [ 3] bdemf_TypeList10
// [ 3] bdemf_TypeList11
// [ 3] bdemf_TypeList12
// [ 3] bdemf_TypeList13
// [ 3] bdemf_TypeList14
// [ 3] bdemf_TypeList15
// [ 3] bdemf_TypeList16
// [ 3] bdemf_TypeList17
// [ 3] bdemf_TypeList18
// [ 3] bdemf_TypeList19
// [ 3] bdemf_TypeList20
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
namespace {

    int testStatus = 0;

    void aSsErT(int c, const char *s, int i) {
        if (c) {
            printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
            if (testStatus >= 0 && testStatus <= 100) ++testStatus;
        }
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
#define L_ __LINE__                     // current Line number
#define T_ printf("\t");                // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdemf_TypeListNil Nil;

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

template <class A1, class A2>
struct IsSame {
    enum { VALUE = 0 };
};

template <class A>
struct IsSame<A,A> {
    enum { VALUE = 1 };
};

template <class LIST>
struct ListAsserter0 {
    static void listAssert()
    {
        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1>
struct ListAsserter1 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1, typename LIST::Type1>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1, class A2>
struct ListAsserter2 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1, typename LIST::Type1>::VALUE));
        ASSERT(1 == (IsSame<A2, typename LIST::Type2>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                        typename bdemf_TypeListTypeOf<2, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                            typename LIST::template TypeOf<2>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<3, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1, class A2, class A3>
struct ListAsserter3 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1, typename LIST::Type1>::VALUE));
        ASSERT(1 == (IsSame<A2, typename LIST::Type2>::VALUE));
        ASSERT(1 == (IsSame<A3, typename LIST::Type3>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                        typename bdemf_TypeListTypeOf<2, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                        typename bdemf_TypeListTypeOf<3, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                            typename LIST::template TypeOf<2>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                            typename LIST::template TypeOf<3>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
               typename bdemf_TypeListTypeOf<3, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<4, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1, class A2, class A3, class A4>
struct ListAsserter4 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1, typename LIST::Type1>::VALUE));
        ASSERT(1 == (IsSame<A2, typename LIST::Type2>::VALUE));
        ASSERT(1 == (IsSame<A3, typename LIST::Type3>::VALUE));
        ASSERT(1 == (IsSame<A4, typename LIST::Type4>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                        typename bdemf_TypeListTypeOf<2, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                        typename bdemf_TypeListTypeOf<3, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                        typename bdemf_TypeListTypeOf<4, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                            typename LIST::template TypeOf<2>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                            typename LIST::template TypeOf<3>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                            typename LIST::template TypeOf<4>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
               typename bdemf_TypeListTypeOf<3, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
               typename bdemf_TypeListTypeOf<4, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<5, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1, class A2, class A3, class A4, class A5>
struct ListAsserter5 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1, typename LIST::Type1>::VALUE));
        ASSERT(1 == (IsSame<A2, typename LIST::Type2>::VALUE));
        ASSERT(1 == (IsSame<A3, typename LIST::Type3>::VALUE));
        ASSERT(1 == (IsSame<A4, typename LIST::Type4>::VALUE));
        ASSERT(1 == (IsSame<A5, typename LIST::Type5>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                        typename bdemf_TypeListTypeOf<2, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                        typename bdemf_TypeListTypeOf<3, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                        typename bdemf_TypeListTypeOf<4, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                        typename bdemf_TypeListTypeOf<5, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                            typename LIST::template TypeOf<2>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                            typename LIST::template TypeOf<3>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                            typename LIST::template TypeOf<4>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                            typename LIST::template TypeOf<5>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
               typename bdemf_TypeListTypeOf<3, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
               typename bdemf_TypeListTypeOf<4, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
               typename bdemf_TypeListTypeOf<5, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<6, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1, class A2, class A3, class A4, class A5,
                      class A6>
struct ListAsserter6 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1, typename LIST::Type1>::VALUE));
        ASSERT(1 == (IsSame<A2, typename LIST::Type2>::VALUE));
        ASSERT(1 == (IsSame<A3, typename LIST::Type3>::VALUE));
        ASSERT(1 == (IsSame<A4, typename LIST::Type4>::VALUE));
        ASSERT(1 == (IsSame<A5, typename LIST::Type5>::VALUE));
        ASSERT(1 == (IsSame<A6, typename LIST::Type6>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                        typename bdemf_TypeListTypeOf<2, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                        typename bdemf_TypeListTypeOf<3, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                        typename bdemf_TypeListTypeOf<4, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                        typename bdemf_TypeListTypeOf<5, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                        typename bdemf_TypeListTypeOf<6, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                            typename LIST::template TypeOf<2>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                            typename LIST::template TypeOf<3>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                            typename LIST::template TypeOf<4>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                            typename LIST::template TypeOf<5>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                            typename LIST::template TypeOf<6>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
               typename bdemf_TypeListTypeOf<3, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
               typename bdemf_TypeListTypeOf<4, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
               typename bdemf_TypeListTypeOf<5, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
               typename bdemf_TypeListTypeOf<6, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<7, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7>
struct ListAsserter7 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1, typename LIST::Type1>::VALUE));
        ASSERT(1 == (IsSame<A2, typename LIST::Type2>::VALUE));
        ASSERT(1 == (IsSame<A3, typename LIST::Type3>::VALUE));
        ASSERT(1 == (IsSame<A4, typename LIST::Type4>::VALUE));
        ASSERT(1 == (IsSame<A5, typename LIST::Type5>::VALUE));
        ASSERT(1 == (IsSame<A6, typename LIST::Type6>::VALUE));
        ASSERT(1 == (IsSame<A7, typename LIST::Type7>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                        typename bdemf_TypeListTypeOf<2, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                        typename bdemf_TypeListTypeOf<3, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                        typename bdemf_TypeListTypeOf<4, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                        typename bdemf_TypeListTypeOf<5, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                        typename bdemf_TypeListTypeOf<6, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                        typename bdemf_TypeListTypeOf<7, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                            typename LIST::template TypeOf<2>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                            typename LIST::template TypeOf<3>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                            typename LIST::template TypeOf<4>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                            typename LIST::template TypeOf<5>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                            typename LIST::template TypeOf<6>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                            typename LIST::template TypeOf<7>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
               typename bdemf_TypeListTypeOf<3, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
               typename bdemf_TypeListTypeOf<4, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
               typename bdemf_TypeListTypeOf<5, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
               typename bdemf_TypeListTypeOf<6, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
               typename bdemf_TypeListTypeOf<7, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<8, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7, class A8>
struct ListAsserter8 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1, typename LIST::Type1>::VALUE));
        ASSERT(1 == (IsSame<A2, typename LIST::Type2>::VALUE));
        ASSERT(1 == (IsSame<A3, typename LIST::Type3>::VALUE));
        ASSERT(1 == (IsSame<A4, typename LIST::Type4>::VALUE));
        ASSERT(1 == (IsSame<A5, typename LIST::Type5>::VALUE));
        ASSERT(1 == (IsSame<A6, typename LIST::Type6>::VALUE));
        ASSERT(1 == (IsSame<A7, typename LIST::Type7>::VALUE));
        ASSERT(1 == (IsSame<A8, typename LIST::Type8>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                        typename bdemf_TypeListTypeOf<2, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                        typename bdemf_TypeListTypeOf<3, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                        typename bdemf_TypeListTypeOf<4, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                        typename bdemf_TypeListTypeOf<5, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                        typename bdemf_TypeListTypeOf<6, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                        typename bdemf_TypeListTypeOf<7, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                        typename bdemf_TypeListTypeOf<8, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                            typename LIST::template TypeOf<2>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                            typename LIST::template TypeOf<3>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                            typename LIST::template TypeOf<4>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                            typename LIST::template TypeOf<5>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                            typename LIST::template TypeOf<6>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                            typename LIST::template TypeOf<7>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                            typename LIST::template TypeOf<8>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
               typename bdemf_TypeListTypeOf<3, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
               typename bdemf_TypeListTypeOf<4, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
               typename bdemf_TypeListTypeOf<5, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
               typename bdemf_TypeListTypeOf<6, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
               typename bdemf_TypeListTypeOf<7, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
               typename bdemf_TypeListTypeOf<8, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<9, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7, class A8, class A9>
struct ListAsserter9 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1, typename LIST::Type1>::VALUE));
        ASSERT(1 == (IsSame<A2, typename LIST::Type2>::VALUE));
        ASSERT(1 == (IsSame<A3, typename LIST::Type3>::VALUE));
        ASSERT(1 == (IsSame<A4, typename LIST::Type4>::VALUE));
        ASSERT(1 == (IsSame<A5, typename LIST::Type5>::VALUE));
        ASSERT(1 == (IsSame<A6, typename LIST::Type6>::VALUE));
        ASSERT(1 == (IsSame<A7, typename LIST::Type7>::VALUE));
        ASSERT(1 == (IsSame<A8, typename LIST::Type8>::VALUE));
        ASSERT(1 == (IsSame<A9, typename LIST::Type9>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                        typename bdemf_TypeListTypeOf<2, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                        typename bdemf_TypeListTypeOf<3, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                        typename bdemf_TypeListTypeOf<4, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                        typename bdemf_TypeListTypeOf<5, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                        typename bdemf_TypeListTypeOf<6, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                        typename bdemf_TypeListTypeOf<7, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                        typename bdemf_TypeListTypeOf<8, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                        typename bdemf_TypeListTypeOf<9, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                            typename LIST::template TypeOf<2>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                            typename LIST::template TypeOf<3>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                            typename LIST::template TypeOf<4>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                            typename LIST::template TypeOf<5>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                            typename LIST::template TypeOf<6>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                            typename LIST::template TypeOf<7>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                            typename LIST::template TypeOf<8>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                            typename LIST::template TypeOf<9>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
               typename bdemf_TypeListTypeOf<3, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
               typename bdemf_TypeListTypeOf<4, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
               typename bdemf_TypeListTypeOf<5, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
               typename bdemf_TypeListTypeOf<6, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
               typename bdemf_TypeListTypeOf<7, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
               typename bdemf_TypeListTypeOf<8, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
               typename bdemf_TypeListTypeOf<9, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
              typename bdemf_TypeListTypeOf<10, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7, class A8, class A9, class A10>
struct ListAsserter10 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1,  typename LIST::Type1>::VALUE));
        ASSERT(1 == (IsSame<A2,  typename LIST::Type2>::VALUE));
        ASSERT(1 == (IsSame<A3,  typename LIST::Type3>::VALUE));
        ASSERT(1 == (IsSame<A4,  typename LIST::Type4>::VALUE));
        ASSERT(1 == (IsSame<A5,  typename LIST::Type5>::VALUE));
        ASSERT(1 == (IsSame<A6,  typename LIST::Type6>::VALUE));
        ASSERT(1 == (IsSame<A7,  typename LIST::Type7>::VALUE));
        ASSERT(1 == (IsSame<A8,  typename LIST::Type8>::VALUE));
        ASSERT(1 == (IsSame<A9,  typename LIST::Type9>::VALUE));
        ASSERT(1 == (IsSame<A10, typename LIST::Type10>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                        typename bdemf_TypeListTypeOf<2, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                        typename bdemf_TypeListTypeOf<3, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                        typename bdemf_TypeListTypeOf<4, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                        typename bdemf_TypeListTypeOf<5, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                        typename bdemf_TypeListTypeOf<6, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                        typename bdemf_TypeListTypeOf<7, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                        typename bdemf_TypeListTypeOf<8, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                        typename bdemf_TypeListTypeOf<9, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                       typename bdemf_TypeListTypeOf<10, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                            typename LIST::template TypeOf<2>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                            typename LIST::template TypeOf<3>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                            typename LIST::template TypeOf<4>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                            typename LIST::template TypeOf<5>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                            typename LIST::template TypeOf<6>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                            typename LIST::template TypeOf<7>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                            typename LIST::template TypeOf<8>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                            typename LIST::template TypeOf<9>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                            typename LIST::template TypeOf<10>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
               typename bdemf_TypeListTypeOf<3, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
               typename bdemf_TypeListTypeOf<4, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
               typename bdemf_TypeListTypeOf<5, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
               typename bdemf_TypeListTypeOf<6, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
               typename bdemf_TypeListTypeOf<7, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
               typename bdemf_TypeListTypeOf<8, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
               typename bdemf_TypeListTypeOf<9, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
              typename bdemf_TypeListTypeOf<10, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
              typename bdemf_TypeListTypeOf<11, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7, class A8, class A9, class A10,
                      class A11>
struct ListAsserter11 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1,  typename LIST::Type1>::VALUE));
        ASSERT(1 == (IsSame<A2,  typename LIST::Type2>::VALUE));
        ASSERT(1 == (IsSame<A3,  typename LIST::Type3>::VALUE));
        ASSERT(1 == (IsSame<A4,  typename LIST::Type4>::VALUE));
        ASSERT(1 == (IsSame<A5,  typename LIST::Type5>::VALUE));
        ASSERT(1 == (IsSame<A6,  typename LIST::Type6>::VALUE));
        ASSERT(1 == (IsSame<A7,  typename LIST::Type7>::VALUE));
        ASSERT(1 == (IsSame<A8,  typename LIST::Type8>::VALUE));
        ASSERT(1 == (IsSame<A9,  typename LIST::Type9>::VALUE));
        ASSERT(1 == (IsSame<A10, typename LIST::Type10>::VALUE));
        ASSERT(1 == (IsSame<A11, typename LIST::Type11>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                        typename bdemf_TypeListTypeOf<2, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                        typename bdemf_TypeListTypeOf<3, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                        typename bdemf_TypeListTypeOf<4, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                        typename bdemf_TypeListTypeOf<5, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                        typename bdemf_TypeListTypeOf<6, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                        typename bdemf_TypeListTypeOf<7, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                        typename bdemf_TypeListTypeOf<8, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                        typename bdemf_TypeListTypeOf<9, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                       typename bdemf_TypeListTypeOf<10, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                       typename bdemf_TypeListTypeOf<11, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                            typename LIST::template TypeOf<2>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                            typename LIST::template TypeOf<3>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                            typename LIST::template TypeOf<4>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                            typename LIST::template TypeOf<5>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                            typename LIST::template TypeOf<6>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                            typename LIST::template TypeOf<7>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                            typename LIST::template TypeOf<8>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                            typename LIST::template TypeOf<9>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                            typename LIST::template TypeOf<10>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                            typename LIST::template TypeOf<11>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
               typename bdemf_TypeListTypeOf<3, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
               typename bdemf_TypeListTypeOf<4, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
               typename bdemf_TypeListTypeOf<5, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
               typename bdemf_TypeListTypeOf<6, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
               typename bdemf_TypeListTypeOf<7, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
               typename bdemf_TypeListTypeOf<8, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
               typename bdemf_TypeListTypeOf<9, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
              typename bdemf_TypeListTypeOf<10, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
              typename bdemf_TypeListTypeOf<11, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
              typename bdemf_TypeListTypeOf<12, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1,  class A2, class A3, class A4, class A5,
                      class A6,  class A7, class A8, class A9, class A10,
                      class A11, class A12>
struct ListAsserter12 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1,  typename LIST::Type1>::VALUE));
        ASSERT(1 == (IsSame<A2,  typename LIST::Type2>::VALUE));
        ASSERT(1 == (IsSame<A3,  typename LIST::Type3>::VALUE));
        ASSERT(1 == (IsSame<A4,  typename LIST::Type4>::VALUE));
        ASSERT(1 == (IsSame<A5,  typename LIST::Type5>::VALUE));
        ASSERT(1 == (IsSame<A6,  typename LIST::Type6>::VALUE));
        ASSERT(1 == (IsSame<A7,  typename LIST::Type7>::VALUE));
        ASSERT(1 == (IsSame<A8,  typename LIST::Type8>::VALUE));
        ASSERT(1 == (IsSame<A9,  typename LIST::Type9>::VALUE));
        ASSERT(1 == (IsSame<A10, typename LIST::Type10>::VALUE));
        ASSERT(1 == (IsSame<A11, typename LIST::Type11>::VALUE));
        ASSERT(1 == (IsSame<A12, typename LIST::Type12>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                        typename bdemf_TypeListTypeOf<2, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                        typename bdemf_TypeListTypeOf<3, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                        typename bdemf_TypeListTypeOf<4, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                        typename bdemf_TypeListTypeOf<5, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                        typename bdemf_TypeListTypeOf<6, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                        typename bdemf_TypeListTypeOf<7, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                        typename bdemf_TypeListTypeOf<8, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                        typename bdemf_TypeListTypeOf<9, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                       typename bdemf_TypeListTypeOf<10, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                       typename bdemf_TypeListTypeOf<11, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A12,
                       typename bdemf_TypeListTypeOf<12, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                            typename LIST::template TypeOf<2>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                            typename LIST::template TypeOf<3>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                            typename LIST::template TypeOf<4>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                            typename LIST::template TypeOf<5>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                            typename LIST::template TypeOf<6>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                            typename LIST::template TypeOf<7>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                            typename LIST::template TypeOf<8>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                            typename LIST::template TypeOf<9>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                            typename LIST::template TypeOf<10>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                            typename LIST::template TypeOf<11>::Type>::VALUE));
        ASSERT(1 == (IsSame<A12,
                            typename LIST::template TypeOf<12>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
               typename bdemf_TypeListTypeOf<3, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
               typename bdemf_TypeListTypeOf<4, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
               typename bdemf_TypeListTypeOf<5, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
               typename bdemf_TypeListTypeOf<6, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
               typename bdemf_TypeListTypeOf<7, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
               typename bdemf_TypeListTypeOf<8, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
               typename bdemf_TypeListTypeOf<9, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
              typename bdemf_TypeListTypeOf<10, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
              typename bdemf_TypeListTypeOf<11, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A12,
              typename bdemf_TypeListTypeOf<12, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A12,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
              typename bdemf_TypeListTypeOf<13, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                   typename LIST::template TypeOf<13>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1,  class A2,  class A3, class A4, class A5,
                      class A6,  class A7,  class A8, class A9, class A10,
                      class A11, class A12, class A13>
struct ListAsserter13 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1,  typename LIST::Type1>::VALUE));
        ASSERT(1 == (IsSame<A2,  typename LIST::Type2>::VALUE));
        ASSERT(1 == (IsSame<A3,  typename LIST::Type3>::VALUE));
        ASSERT(1 == (IsSame<A4,  typename LIST::Type4>::VALUE));
        ASSERT(1 == (IsSame<A5,  typename LIST::Type5>::VALUE));
        ASSERT(1 == (IsSame<A6,  typename LIST::Type6>::VALUE));
        ASSERT(1 == (IsSame<A7,  typename LIST::Type7>::VALUE));
        ASSERT(1 == (IsSame<A8,  typename LIST::Type8>::VALUE));
        ASSERT(1 == (IsSame<A9,  typename LIST::Type9>::VALUE));
        ASSERT(1 == (IsSame<A10, typename LIST::Type10>::VALUE));
        ASSERT(1 == (IsSame<A11, typename LIST::Type11>::VALUE));
        ASSERT(1 == (IsSame<A12, typename LIST::Type12>::VALUE));
        ASSERT(1 == (IsSame<A13, typename LIST::Type13>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                        typename bdemf_TypeListTypeOf<2, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                        typename bdemf_TypeListTypeOf<3, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                        typename bdemf_TypeListTypeOf<4, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                        typename bdemf_TypeListTypeOf<5, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                        typename bdemf_TypeListTypeOf<6, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                        typename bdemf_TypeListTypeOf<7, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                        typename bdemf_TypeListTypeOf<8, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                        typename bdemf_TypeListTypeOf<9, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                       typename bdemf_TypeListTypeOf<10, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                       typename bdemf_TypeListTypeOf<11, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A12,
                       typename bdemf_TypeListTypeOf<12, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A13,
                       typename bdemf_TypeListTypeOf<13, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                            typename LIST::template TypeOf<2>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                            typename LIST::template TypeOf<3>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                            typename LIST::template TypeOf<4>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                            typename LIST::template TypeOf<5>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                            typename LIST::template TypeOf<6>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                            typename LIST::template TypeOf<7>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                            typename LIST::template TypeOf<8>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                            typename LIST::template TypeOf<9>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                            typename LIST::template TypeOf<10>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                            typename LIST::template TypeOf<11>::Type>::VALUE));
        ASSERT(1 == (IsSame<A12,
                            typename LIST::template TypeOf<12>::Type>::VALUE));
        ASSERT(1 == (IsSame<A13,
                            typename LIST::template TypeOf<13>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
               typename bdemf_TypeListTypeOf<3, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
               typename bdemf_TypeListTypeOf<4, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
               typename bdemf_TypeListTypeOf<5, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
               typename bdemf_TypeListTypeOf<6, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
               typename bdemf_TypeListTypeOf<7, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
               typename bdemf_TypeListTypeOf<8, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
               typename bdemf_TypeListTypeOf<9, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
              typename bdemf_TypeListTypeOf<10, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
              typename bdemf_TypeListTypeOf<11, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A12,
              typename bdemf_TypeListTypeOf<12, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A13,
              typename bdemf_TypeListTypeOf<13, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A12,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A13,
                   typename LIST::template TypeOf<13>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
              typename bdemf_TypeListTypeOf<14, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                   typename LIST::template TypeOf<14>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1,  class A2,  class A3,  class A4, class A5,
                      class A6,  class A7,  class A8,  class A9, class A10,
                      class A11, class A12, class A13, class A14>
struct ListAsserter14 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1,  typename LIST::Type1>::VALUE));
        ASSERT(1 == (IsSame<A2,  typename LIST::Type2>::VALUE));
        ASSERT(1 == (IsSame<A3,  typename LIST::Type3>::VALUE));
        ASSERT(1 == (IsSame<A4,  typename LIST::Type4>::VALUE));
        ASSERT(1 == (IsSame<A5,  typename LIST::Type5>::VALUE));
        ASSERT(1 == (IsSame<A6,  typename LIST::Type6>::VALUE));
        ASSERT(1 == (IsSame<A7,  typename LIST::Type7>::VALUE));
        ASSERT(1 == (IsSame<A8,  typename LIST::Type8>::VALUE));
        ASSERT(1 == (IsSame<A9,  typename LIST::Type9>::VALUE));
        ASSERT(1 == (IsSame<A10, typename LIST::Type10>::VALUE));
        ASSERT(1 == (IsSame<A11, typename LIST::Type11>::VALUE));
        ASSERT(1 == (IsSame<A12, typename LIST::Type12>::VALUE));
        ASSERT(1 == (IsSame<A13, typename LIST::Type13>::VALUE));
        ASSERT(1 == (IsSame<A14, typename LIST::Type14>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                        typename bdemf_TypeListTypeOf<2, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                        typename bdemf_TypeListTypeOf<3, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                        typename bdemf_TypeListTypeOf<4, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                        typename bdemf_TypeListTypeOf<5, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                        typename bdemf_TypeListTypeOf<6, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                        typename bdemf_TypeListTypeOf<7, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                        typename bdemf_TypeListTypeOf<8, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                        typename bdemf_TypeListTypeOf<9, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                       typename bdemf_TypeListTypeOf<10, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                       typename bdemf_TypeListTypeOf<11, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A12,
                       typename bdemf_TypeListTypeOf<12, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A13,
                       typename bdemf_TypeListTypeOf<13, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A14,
                       typename bdemf_TypeListTypeOf<14, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                            typename LIST::template TypeOf<2>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                            typename LIST::template TypeOf<3>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                            typename LIST::template TypeOf<4>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                            typename LIST::template TypeOf<5>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                            typename LIST::template TypeOf<6>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                            typename LIST::template TypeOf<7>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                            typename LIST::template TypeOf<8>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                            typename LIST::template TypeOf<9>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                            typename LIST::template TypeOf<10>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                            typename LIST::template TypeOf<11>::Type>::VALUE));
        ASSERT(1 == (IsSame<A12,
                            typename LIST::template TypeOf<12>::Type>::VALUE));
        ASSERT(1 == (IsSame<A13,
                            typename LIST::template TypeOf<13>::Type>::VALUE));
        ASSERT(1 == (IsSame<A14,
                            typename LIST::template TypeOf<14>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
               typename bdemf_TypeListTypeOf<3, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
               typename bdemf_TypeListTypeOf<4, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
               typename bdemf_TypeListTypeOf<5, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
               typename bdemf_TypeListTypeOf<6, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
               typename bdemf_TypeListTypeOf<7, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
               typename bdemf_TypeListTypeOf<8, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
               typename bdemf_TypeListTypeOf<9, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
              typename bdemf_TypeListTypeOf<10, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
              typename bdemf_TypeListTypeOf<11, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A12,
              typename bdemf_TypeListTypeOf<12, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A13,
              typename bdemf_TypeListTypeOf<13, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A14,
              typename bdemf_TypeListTypeOf<14, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A12,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A13,
                   typename LIST::template TypeOf<13>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A14,
                   typename LIST::template TypeOf<14>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
              typename bdemf_TypeListTypeOf<15, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                   typename LIST::template TypeOf<15>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1,  class A2,  class A3,  class A4,  class A5,
                      class A6,  class A7,  class A8,  class A9,  class A10,
                      class A11, class A12, class A13, class A14, class A15>
struct ListAsserter15 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1,  typename LIST::Type1>::VALUE));
        ASSERT(1 == (IsSame<A2,  typename LIST::Type2>::VALUE));
        ASSERT(1 == (IsSame<A3,  typename LIST::Type3>::VALUE));
        ASSERT(1 == (IsSame<A4,  typename LIST::Type4>::VALUE));
        ASSERT(1 == (IsSame<A5,  typename LIST::Type5>::VALUE));
        ASSERT(1 == (IsSame<A6,  typename LIST::Type6>::VALUE));
        ASSERT(1 == (IsSame<A7,  typename LIST::Type7>::VALUE));
        ASSERT(1 == (IsSame<A8,  typename LIST::Type8>::VALUE));
        ASSERT(1 == (IsSame<A9,  typename LIST::Type9>::VALUE));
        ASSERT(1 == (IsSame<A10, typename LIST::Type10>::VALUE));
        ASSERT(1 == (IsSame<A11, typename LIST::Type11>::VALUE));
        ASSERT(1 == (IsSame<A12, typename LIST::Type12>::VALUE));
        ASSERT(1 == (IsSame<A13, typename LIST::Type13>::VALUE));
        ASSERT(1 == (IsSame<A14, typename LIST::Type14>::VALUE));
        ASSERT(1 == (IsSame<A15, typename LIST::Type15>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                        typename bdemf_TypeListTypeOf<2, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                        typename bdemf_TypeListTypeOf<3, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                        typename bdemf_TypeListTypeOf<4, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                        typename bdemf_TypeListTypeOf<5, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                        typename bdemf_TypeListTypeOf<6, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                        typename bdemf_TypeListTypeOf<7, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                        typename bdemf_TypeListTypeOf<8, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                        typename bdemf_TypeListTypeOf<9, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                       typename bdemf_TypeListTypeOf<10, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                       typename bdemf_TypeListTypeOf<11, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A12,
                       typename bdemf_TypeListTypeOf<12, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A13,
                       typename bdemf_TypeListTypeOf<13, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A14,
                       typename bdemf_TypeListTypeOf<14, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A15,
                       typename bdemf_TypeListTypeOf<15, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                            typename LIST::template TypeOf<2>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                            typename LIST::template TypeOf<3>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                            typename LIST::template TypeOf<4>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                            typename LIST::template TypeOf<5>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                            typename LIST::template TypeOf<6>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                            typename LIST::template TypeOf<7>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                            typename LIST::template TypeOf<8>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                            typename LIST::template TypeOf<9>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                            typename LIST::template TypeOf<10>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                            typename LIST::template TypeOf<11>::Type>::VALUE));
        ASSERT(1 == (IsSame<A12,
                            typename LIST::template TypeOf<12>::Type>::VALUE));
        ASSERT(1 == (IsSame<A13,
                            typename LIST::template TypeOf<13>::Type>::VALUE));
        ASSERT(1 == (IsSame<A14,
                            typename LIST::template TypeOf<14>::Type>::VALUE));
        ASSERT(1 == (IsSame<A15,
                            typename LIST::template TypeOf<15>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
               typename bdemf_TypeListTypeOf<3, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
               typename bdemf_TypeListTypeOf<4, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
               typename bdemf_TypeListTypeOf<5, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
               typename bdemf_TypeListTypeOf<6, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
               typename bdemf_TypeListTypeOf<7, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
               typename bdemf_TypeListTypeOf<8, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
               typename bdemf_TypeListTypeOf<9, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
              typename bdemf_TypeListTypeOf<10, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
              typename bdemf_TypeListTypeOf<11, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A12,
              typename bdemf_TypeListTypeOf<12, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A13,
              typename bdemf_TypeListTypeOf<13, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A14,
              typename bdemf_TypeListTypeOf<14, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A15,
              typename bdemf_TypeListTypeOf<15, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A12,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A13,
                   typename LIST::template TypeOf<13>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A14,
                   typename LIST::template TypeOf<14>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A15,
                   typename LIST::template TypeOf<15>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
              typename bdemf_TypeListTypeOf<16, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                   typename LIST::template TypeOf<16>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1,  class A2,  class A3,  class A4,  class A5,
                      class A6,  class A7,  class A8,  class A9,  class A10,
                      class A11, class A12, class A13, class A14, class A15,
                      class A16>
struct ListAsserter16 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1,  typename LIST::Type1>::VALUE));
        ASSERT(1 == (IsSame<A2,  typename LIST::Type2>::VALUE));
        ASSERT(1 == (IsSame<A3,  typename LIST::Type3>::VALUE));
        ASSERT(1 == (IsSame<A4,  typename LIST::Type4>::VALUE));
        ASSERT(1 == (IsSame<A5,  typename LIST::Type5>::VALUE));
        ASSERT(1 == (IsSame<A6,  typename LIST::Type6>::VALUE));
        ASSERT(1 == (IsSame<A7,  typename LIST::Type7>::VALUE));
        ASSERT(1 == (IsSame<A8,  typename LIST::Type8>::VALUE));
        ASSERT(1 == (IsSame<A9,  typename LIST::Type9>::VALUE));
        ASSERT(1 == (IsSame<A10, typename LIST::Type10>::VALUE));
        ASSERT(1 == (IsSame<A11, typename LIST::Type11>::VALUE));
        ASSERT(1 == (IsSame<A12, typename LIST::Type12>::VALUE));
        ASSERT(1 == (IsSame<A13, typename LIST::Type13>::VALUE));
        ASSERT(1 == (IsSame<A14, typename LIST::Type14>::VALUE));
        ASSERT(1 == (IsSame<A15, typename LIST::Type15>::VALUE));
        ASSERT(1 == (IsSame<A16, typename LIST::Type16>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                        typename bdemf_TypeListTypeOf<2, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                        typename bdemf_TypeListTypeOf<3, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                        typename bdemf_TypeListTypeOf<4, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                        typename bdemf_TypeListTypeOf<5, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                        typename bdemf_TypeListTypeOf<6, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                        typename bdemf_TypeListTypeOf<7, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                        typename bdemf_TypeListTypeOf<8, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                        typename bdemf_TypeListTypeOf<9, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                       typename bdemf_TypeListTypeOf<10, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                       typename bdemf_TypeListTypeOf<11, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A12,
                       typename bdemf_TypeListTypeOf<12, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A13,
                       typename bdemf_TypeListTypeOf<13, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A14,
                       typename bdemf_TypeListTypeOf<14, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A15,
                       typename bdemf_TypeListTypeOf<15, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A16,
                       typename bdemf_TypeListTypeOf<16, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                            typename LIST::template TypeOf<2>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                            typename LIST::template TypeOf<3>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                            typename LIST::template TypeOf<4>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                            typename LIST::template TypeOf<5>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                            typename LIST::template TypeOf<6>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                            typename LIST::template TypeOf<7>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                            typename LIST::template TypeOf<8>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                            typename LIST::template TypeOf<9>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                            typename LIST::template TypeOf<10>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                            typename LIST::template TypeOf<11>::Type>::VALUE));
        ASSERT(1 == (IsSame<A12,
                            typename LIST::template TypeOf<12>::Type>::VALUE));
        ASSERT(1 == (IsSame<A13,
                            typename LIST::template TypeOf<13>::Type>::VALUE));
        ASSERT(1 == (IsSame<A14,
                            typename LIST::template TypeOf<14>::Type>::VALUE));
        ASSERT(1 == (IsSame<A15,
                            typename LIST::template TypeOf<15>::Type>::VALUE));
        ASSERT(1 == (IsSame<A16,
                            typename LIST::template TypeOf<16>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
               typename bdemf_TypeListTypeOf<3, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
               typename bdemf_TypeListTypeOf<4, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
               typename bdemf_TypeListTypeOf<5, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
               typename bdemf_TypeListTypeOf<6, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
               typename bdemf_TypeListTypeOf<7, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
               typename bdemf_TypeListTypeOf<8, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
               typename bdemf_TypeListTypeOf<9, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
              typename bdemf_TypeListTypeOf<10, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
              typename bdemf_TypeListTypeOf<11, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A12,
              typename bdemf_TypeListTypeOf<12, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A13,
              typename bdemf_TypeListTypeOf<13, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A14,
              typename bdemf_TypeListTypeOf<14, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A15,
              typename bdemf_TypeListTypeOf<15, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A16,
              typename bdemf_TypeListTypeOf<16, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A12,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A13,
                   typename LIST::template TypeOf<13>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A14,
                   typename LIST::template TypeOf<14>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A15,
                   typename LIST::template TypeOf<15>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A16,
                   typename LIST::template TypeOf<16>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
              typename bdemf_TypeListTypeOf<17, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                   typename LIST::template TypeOf<17>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1,  class A2,  class A3,  class A4,  class A5,
                      class A6,  class A7,  class A8,  class A9,  class A10,
                      class A11, class A12, class A13, class A14, class A15,
                      class A16, class A17>
struct ListAsserter17 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1,  typename LIST::Type1>::VALUE));
        ASSERT(1 == (IsSame<A2,  typename LIST::Type2>::VALUE));
        ASSERT(1 == (IsSame<A3,  typename LIST::Type3>::VALUE));
        ASSERT(1 == (IsSame<A4,  typename LIST::Type4>::VALUE));
        ASSERT(1 == (IsSame<A5,  typename LIST::Type5>::VALUE));
        ASSERT(1 == (IsSame<A6,  typename LIST::Type6>::VALUE));
        ASSERT(1 == (IsSame<A7,  typename LIST::Type7>::VALUE));
        ASSERT(1 == (IsSame<A8,  typename LIST::Type8>::VALUE));
        ASSERT(1 == (IsSame<A9,  typename LIST::Type9>::VALUE));
        ASSERT(1 == (IsSame<A10, typename LIST::Type10>::VALUE));
        ASSERT(1 == (IsSame<A11, typename LIST::Type11>::VALUE));
        ASSERT(1 == (IsSame<A12, typename LIST::Type12>::VALUE));
        ASSERT(1 == (IsSame<A13, typename LIST::Type13>::VALUE));
        ASSERT(1 == (IsSame<A14, typename LIST::Type14>::VALUE));
        ASSERT(1 == (IsSame<A15, typename LIST::Type15>::VALUE));
        ASSERT(1 == (IsSame<A16, typename LIST::Type16>::VALUE));
        ASSERT(1 == (IsSame<A17, typename LIST::Type17>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                        typename bdemf_TypeListTypeOf<2, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                        typename bdemf_TypeListTypeOf<3, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                        typename bdemf_TypeListTypeOf<4, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                        typename bdemf_TypeListTypeOf<5, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                        typename bdemf_TypeListTypeOf<6, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                        typename bdemf_TypeListTypeOf<7, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                        typename bdemf_TypeListTypeOf<8, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                        typename bdemf_TypeListTypeOf<9, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                       typename bdemf_TypeListTypeOf<10, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                       typename bdemf_TypeListTypeOf<11, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A12,
                       typename bdemf_TypeListTypeOf<12, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A13,
                       typename bdemf_TypeListTypeOf<13, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A14,
                       typename bdemf_TypeListTypeOf<14, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A15,
                       typename bdemf_TypeListTypeOf<15, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A16,
                       typename bdemf_TypeListTypeOf<16, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A17,
                       typename bdemf_TypeListTypeOf<17, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                            typename LIST::template TypeOf<2>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                            typename LIST::template TypeOf<3>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                            typename LIST::template TypeOf<4>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                            typename LIST::template TypeOf<5>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                            typename LIST::template TypeOf<6>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                            typename LIST::template TypeOf<7>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                            typename LIST::template TypeOf<8>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                            typename LIST::template TypeOf<9>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                            typename LIST::template TypeOf<10>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                            typename LIST::template TypeOf<11>::Type>::VALUE));
        ASSERT(1 == (IsSame<A12,
                            typename LIST::template TypeOf<12>::Type>::VALUE));
        ASSERT(1 == (IsSame<A13,
                            typename LIST::template TypeOf<13>::Type>::VALUE));
        ASSERT(1 == (IsSame<A14,
                            typename LIST::template TypeOf<14>::Type>::VALUE));
        ASSERT(1 == (IsSame<A15,
                            typename LIST::template TypeOf<15>::Type>::VALUE));
        ASSERT(1 == (IsSame<A16,
                            typename LIST::template TypeOf<16>::Type>::VALUE));
        ASSERT(1 == (IsSame<A17,
                            typename LIST::template TypeOf<17>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
               typename bdemf_TypeListTypeOf<3, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
               typename bdemf_TypeListTypeOf<4, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
               typename bdemf_TypeListTypeOf<5, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
               typename bdemf_TypeListTypeOf<6, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
               typename bdemf_TypeListTypeOf<7, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
               typename bdemf_TypeListTypeOf<8, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
               typename bdemf_TypeListTypeOf<9, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
              typename bdemf_TypeListTypeOf<10, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
              typename bdemf_TypeListTypeOf<11, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A12,
              typename bdemf_TypeListTypeOf<12, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A13,
              typename bdemf_TypeListTypeOf<13, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A14,
              typename bdemf_TypeListTypeOf<14, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A15,
              typename bdemf_TypeListTypeOf<15, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A16,
              typename bdemf_TypeListTypeOf<16, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A17,
              typename bdemf_TypeListTypeOf<17, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A12,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A13,
                   typename LIST::template TypeOf<13>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A14,
                   typename LIST::template TypeOf<14>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A15,
                   typename LIST::template TypeOf<15>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A16,
                   typename LIST::template TypeOf<16>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A17,
                   typename LIST::template TypeOf<17>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
              typename bdemf_TypeListTypeOf<18, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                   typename LIST::template TypeOf<18>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1,  class A2,  class A3,  class A4,  class A5,
                      class A6,  class A7,  class A8,  class A9,  class A10,
                      class A11, class A12, class A13, class A14, class A15,
                      class A16, class A17, class A18>
struct ListAsserter18 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1,  typename LIST::Type1>::VALUE));
        ASSERT(1 == (IsSame<A2,  typename LIST::Type2>::VALUE));
        ASSERT(1 == (IsSame<A3,  typename LIST::Type3>::VALUE));
        ASSERT(1 == (IsSame<A4,  typename LIST::Type4>::VALUE));
        ASSERT(1 == (IsSame<A5,  typename LIST::Type5>::VALUE));
        ASSERT(1 == (IsSame<A6,  typename LIST::Type6>::VALUE));
        ASSERT(1 == (IsSame<A7,  typename LIST::Type7>::VALUE));
        ASSERT(1 == (IsSame<A8,  typename LIST::Type8>::VALUE));
        ASSERT(1 == (IsSame<A9,  typename LIST::Type9>::VALUE));
        ASSERT(1 == (IsSame<A10, typename LIST::Type10>::VALUE));
        ASSERT(1 == (IsSame<A11, typename LIST::Type11>::VALUE));
        ASSERT(1 == (IsSame<A12, typename LIST::Type12>::VALUE));
        ASSERT(1 == (IsSame<A13, typename LIST::Type13>::VALUE));
        ASSERT(1 == (IsSame<A14, typename LIST::Type14>::VALUE));
        ASSERT(1 == (IsSame<A15, typename LIST::Type15>::VALUE));
        ASSERT(1 == (IsSame<A16, typename LIST::Type16>::VALUE));
        ASSERT(1 == (IsSame<A17, typename LIST::Type17>::VALUE));
        ASSERT(1 == (IsSame<A18, typename LIST::Type18>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                        typename bdemf_TypeListTypeOf<2, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                        typename bdemf_TypeListTypeOf<3, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                        typename bdemf_TypeListTypeOf<4, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                        typename bdemf_TypeListTypeOf<5, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                        typename bdemf_TypeListTypeOf<6, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                        typename bdemf_TypeListTypeOf<7, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                        typename bdemf_TypeListTypeOf<8, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                        typename bdemf_TypeListTypeOf<9, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                       typename bdemf_TypeListTypeOf<10, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                       typename bdemf_TypeListTypeOf<11, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A12,
                       typename bdemf_TypeListTypeOf<12, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A13,
                       typename bdemf_TypeListTypeOf<13, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A14,
                       typename bdemf_TypeListTypeOf<14, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A15,
                       typename bdemf_TypeListTypeOf<15, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A16,
                       typename bdemf_TypeListTypeOf<16, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A17,
                       typename bdemf_TypeListTypeOf<17, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A18,
                       typename bdemf_TypeListTypeOf<18, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                            typename LIST::template TypeOf<2>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                            typename LIST::template TypeOf<3>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                            typename LIST::template TypeOf<4>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                            typename LIST::template TypeOf<5>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                            typename LIST::template TypeOf<6>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                            typename LIST::template TypeOf<7>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                            typename LIST::template TypeOf<8>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                            typename LIST::template TypeOf<9>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                            typename LIST::template TypeOf<10>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                            typename LIST::template TypeOf<11>::Type>::VALUE));
        ASSERT(1 == (IsSame<A12,
                            typename LIST::template TypeOf<12>::Type>::VALUE));
        ASSERT(1 == (IsSame<A13,
                            typename LIST::template TypeOf<13>::Type>::VALUE));
        ASSERT(1 == (IsSame<A14,
                            typename LIST::template TypeOf<14>::Type>::VALUE));
        ASSERT(1 == (IsSame<A15,
                            typename LIST::template TypeOf<15>::Type>::VALUE));
        ASSERT(1 == (IsSame<A16,
                            typename LIST::template TypeOf<16>::Type>::VALUE));
        ASSERT(1 == (IsSame<A17,
                            typename LIST::template TypeOf<17>::Type>::VALUE));
        ASSERT(1 == (IsSame<A18,
                            typename LIST::template TypeOf<18>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
               typename bdemf_TypeListTypeOf<3, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
               typename bdemf_TypeListTypeOf<4, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
               typename bdemf_TypeListTypeOf<5, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
               typename bdemf_TypeListTypeOf<6, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
               typename bdemf_TypeListTypeOf<7, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
               typename bdemf_TypeListTypeOf<8, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
               typename bdemf_TypeListTypeOf<9, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
              typename bdemf_TypeListTypeOf<10, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
              typename bdemf_TypeListTypeOf<11, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A12,
              typename bdemf_TypeListTypeOf<12, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A13,
              typename bdemf_TypeListTypeOf<13, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A14,
              typename bdemf_TypeListTypeOf<14, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A15,
              typename bdemf_TypeListTypeOf<15, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A16,
              typename bdemf_TypeListTypeOf<16, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A17,
              typename bdemf_TypeListTypeOf<17, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A18,
              typename bdemf_TypeListTypeOf<18, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A12,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A13,
                   typename LIST::template TypeOf<13>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A14,
                   typename LIST::template TypeOf<14>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A15,
                   typename LIST::template TypeOf<15>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A16,
                   typename LIST::template TypeOf<16>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A17,
                   typename LIST::template TypeOf<17>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A18,
                   typename LIST::template TypeOf<18>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
              typename bdemf_TypeListTypeOf<19, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                   typename LIST::template TypeOf<19>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1,  class A2,  class A3,  class A4,  class A5,
                      class A6,  class A7,  class A8,  class A9,  class A10,
                      class A11, class A12, class A13, class A14, class A15,
                      class A16, class A17, class A18, class A19>
struct ListAsserter19 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1,  typename LIST::Type1>::VALUE));
        ASSERT(1 == (IsSame<A2,  typename LIST::Type2>::VALUE));
        ASSERT(1 == (IsSame<A3,  typename LIST::Type3>::VALUE));
        ASSERT(1 == (IsSame<A4,  typename LIST::Type4>::VALUE));
        ASSERT(1 == (IsSame<A5,  typename LIST::Type5>::VALUE));
        ASSERT(1 == (IsSame<A6,  typename LIST::Type6>::VALUE));
        ASSERT(1 == (IsSame<A7,  typename LIST::Type7>::VALUE));
        ASSERT(1 == (IsSame<A8,  typename LIST::Type8>::VALUE));
        ASSERT(1 == (IsSame<A9,  typename LIST::Type9>::VALUE));
        ASSERT(1 == (IsSame<A10, typename LIST::Type10>::VALUE));
        ASSERT(1 == (IsSame<A11, typename LIST::Type11>::VALUE));
        ASSERT(1 == (IsSame<A12, typename LIST::Type12>::VALUE));
        ASSERT(1 == (IsSame<A13, typename LIST::Type13>::VALUE));
        ASSERT(1 == (IsSame<A14, typename LIST::Type14>::VALUE));
        ASSERT(1 == (IsSame<A15, typename LIST::Type15>::VALUE));
        ASSERT(1 == (IsSame<A16, typename LIST::Type16>::VALUE));
        ASSERT(1 == (IsSame<A17, typename LIST::Type17>::VALUE));
        ASSERT(1 == (IsSame<A18, typename LIST::Type18>::VALUE));
        ASSERT(1 == (IsSame<A19, typename LIST::Type19>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                        typename bdemf_TypeListTypeOf<2, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                        typename bdemf_TypeListTypeOf<3, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                        typename bdemf_TypeListTypeOf<4, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                        typename bdemf_TypeListTypeOf<5, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                        typename bdemf_TypeListTypeOf<6, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                        typename bdemf_TypeListTypeOf<7, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                        typename bdemf_TypeListTypeOf<8, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                        typename bdemf_TypeListTypeOf<9, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                       typename bdemf_TypeListTypeOf<10, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                       typename bdemf_TypeListTypeOf<11, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A12,
                       typename bdemf_TypeListTypeOf<12, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A13,
                       typename bdemf_TypeListTypeOf<13, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A14,
                       typename bdemf_TypeListTypeOf<14, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A15,
                       typename bdemf_TypeListTypeOf<15, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A16,
                       typename bdemf_TypeListTypeOf<16, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A17,
                       typename bdemf_TypeListTypeOf<17, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A18,
                       typename bdemf_TypeListTypeOf<18, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A19,
                       typename bdemf_TypeListTypeOf<19, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                            typename LIST::template TypeOf<2>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                            typename LIST::template TypeOf<3>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                            typename LIST::template TypeOf<4>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                            typename LIST::template TypeOf<5>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                            typename LIST::template TypeOf<6>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                            typename LIST::template TypeOf<7>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                            typename LIST::template TypeOf<8>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                            typename LIST::template TypeOf<9>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                            typename LIST::template TypeOf<10>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                            typename LIST::template TypeOf<11>::Type>::VALUE));
        ASSERT(1 == (IsSame<A12,
                            typename LIST::template TypeOf<12>::Type>::VALUE));
        ASSERT(1 == (IsSame<A13,
                            typename LIST::template TypeOf<13>::Type>::VALUE));
        ASSERT(1 == (IsSame<A14,
                            typename LIST::template TypeOf<14>::Type>::VALUE));
        ASSERT(1 == (IsSame<A15,
                            typename LIST::template TypeOf<15>::Type>::VALUE));
        ASSERT(1 == (IsSame<A16,
                            typename LIST::template TypeOf<16>::Type>::VALUE));
        ASSERT(1 == (IsSame<A17,
                            typename LIST::template TypeOf<17>::Type>::VALUE));
        ASSERT(1 == (IsSame<A18,
                            typename LIST::template TypeOf<18>::Type>::VALUE));
        ASSERT(1 == (IsSame<A19,
                            typename LIST::template TypeOf<19>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
               typename bdemf_TypeListTypeOf<3, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
               typename bdemf_TypeListTypeOf<4, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
               typename bdemf_TypeListTypeOf<5, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
               typename bdemf_TypeListTypeOf<6, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
               typename bdemf_TypeListTypeOf<7, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
               typename bdemf_TypeListTypeOf<8, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
               typename bdemf_TypeListTypeOf<9, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
              typename bdemf_TypeListTypeOf<10, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
              typename bdemf_TypeListTypeOf<11, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A12,
              typename bdemf_TypeListTypeOf<12, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A13,
              typename bdemf_TypeListTypeOf<13, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A14,
              typename bdemf_TypeListTypeOf<14, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A15,
              typename bdemf_TypeListTypeOf<15, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A16,
              typename bdemf_TypeListTypeOf<16, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A17,
              typename bdemf_TypeListTypeOf<17, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A18,
              typename bdemf_TypeListTypeOf<18, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A19,
              typename bdemf_TypeListTypeOf<19, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A12,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A13,
                   typename LIST::template TypeOf<13>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A14,
                   typename LIST::template TypeOf<14>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A15,
                   typename LIST::template TypeOf<15>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A16,
                   typename LIST::template TypeOf<16>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A17,
                   typename LIST::template TypeOf<17>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A18,
                   typename LIST::template TypeOf<18>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A19,
                   typename LIST::template TypeOf<19>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
              typename bdemf_TypeListTypeOf<20, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                   typename LIST::template TypeOf<20>::TypeOrDefault>::VALUE));
    }
};

template <class LIST, class A1,  class A2,  class A3,  class A4,  class A5,
                      class A6,  class A7,  class A8,  class A9,  class A10,
                      class A11, class A12, class A13, class A14, class A15,
                      class A16, class A17, class A18, class A19, class A20>
struct ListAsserter20 {
    static void listAssert()
    {
        ASSERT(1 == (IsSame<A1,  typename LIST::Type1>::VALUE));
        ASSERT(1 == (IsSame<A2,  typename LIST::Type2>::VALUE));
        ASSERT(1 == (IsSame<A3,  typename LIST::Type3>::VALUE));
        ASSERT(1 == (IsSame<A4,  typename LIST::Type4>::VALUE));
        ASSERT(1 == (IsSame<A5,  typename LIST::Type5>::VALUE));
        ASSERT(1 == (IsSame<A6,  typename LIST::Type6>::VALUE));
        ASSERT(1 == (IsSame<A7,  typename LIST::Type7>::VALUE));
        ASSERT(1 == (IsSame<A8,  typename LIST::Type8>::VALUE));
        ASSERT(1 == (IsSame<A9,  typename LIST::Type9>::VALUE));
        ASSERT(1 == (IsSame<A10, typename LIST::Type10>::VALUE));
        ASSERT(1 == (IsSame<A11, typename LIST::Type11>::VALUE));
        ASSERT(1 == (IsSame<A12, typename LIST::Type12>::VALUE));
        ASSERT(1 == (IsSame<A13, typename LIST::Type13>::VALUE));
        ASSERT(1 == (IsSame<A14, typename LIST::Type14>::VALUE));
        ASSERT(1 == (IsSame<A15, typename LIST::Type15>::VALUE));
        ASSERT(1 == (IsSame<A16, typename LIST::Type16>::VALUE));
        ASSERT(1 == (IsSame<A17, typename LIST::Type17>::VALUE));
        ASSERT(1 == (IsSame<A18, typename LIST::Type18>::VALUE));
        ASSERT(1 == (IsSame<A19, typename LIST::Type19>::VALUE));
        ASSERT(1 == (IsSame<A20, typename LIST::Type20>::VALUE));

        ASSERT(1 == (IsSame<A1,
                        typename bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                        typename bdemf_TypeListTypeOf<2, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                        typename bdemf_TypeListTypeOf<3, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                        typename bdemf_TypeListTypeOf<4, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                        typename bdemf_TypeListTypeOf<5, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                        typename bdemf_TypeListTypeOf<6, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                        typename bdemf_TypeListTypeOf<7, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                        typename bdemf_TypeListTypeOf<8, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                        typename bdemf_TypeListTypeOf<9, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                       typename bdemf_TypeListTypeOf<10, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                       typename bdemf_TypeListTypeOf<11, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A12,
                       typename bdemf_TypeListTypeOf<12, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A13,
                       typename bdemf_TypeListTypeOf<13, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A14,
                       typename bdemf_TypeListTypeOf<14, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A15,
                       typename bdemf_TypeListTypeOf<15, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A16,
                       typename bdemf_TypeListTypeOf<16, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A17,
                       typename bdemf_TypeListTypeOf<17, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A18,
                       typename bdemf_TypeListTypeOf<18, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A19,
                       typename bdemf_TypeListTypeOf<19, LIST>::Type>::VALUE));
        ASSERT(1 == (IsSame<A20,
                       typename bdemf_TypeListTypeOf<20, LIST>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
                            typename LIST::template TypeOf<1>::Type>::VALUE));
        ASSERT(1 == (IsSame<A2,
                            typename LIST::template TypeOf<2>::Type>::VALUE));
        ASSERT(1 == (IsSame<A3,
                            typename LIST::template TypeOf<3>::Type>::VALUE));
        ASSERT(1 == (IsSame<A4,
                            typename LIST::template TypeOf<4>::Type>::VALUE));
        ASSERT(1 == (IsSame<A5,
                            typename LIST::template TypeOf<5>::Type>::VALUE));
        ASSERT(1 == (IsSame<A6,
                            typename LIST::template TypeOf<6>::Type>::VALUE));
        ASSERT(1 == (IsSame<A7,
                            typename LIST::template TypeOf<7>::Type>::VALUE));
        ASSERT(1 == (IsSame<A8,
                            typename LIST::template TypeOf<8>::Type>::VALUE));
        ASSERT(1 == (IsSame<A9,
                            typename LIST::template TypeOf<9>::Type>::VALUE));
        ASSERT(1 == (IsSame<A10,
                            typename LIST::template TypeOf<10>::Type>::VALUE));
        ASSERT(1 == (IsSame<A11,
                            typename LIST::template TypeOf<11>::Type>::VALUE));
        ASSERT(1 == (IsSame<A12,
                            typename LIST::template TypeOf<12>::Type>::VALUE));
        ASSERT(1 == (IsSame<A13,
                            typename LIST::template TypeOf<13>::Type>::VALUE));
        ASSERT(1 == (IsSame<A14,
                            typename LIST::template TypeOf<14>::Type>::VALUE));
        ASSERT(1 == (IsSame<A15,
                            typename LIST::template TypeOf<15>::Type>::VALUE));
        ASSERT(1 == (IsSame<A16,
                            typename LIST::template TypeOf<16>::Type>::VALUE));
        ASSERT(1 == (IsSame<A17,
                            typename LIST::template TypeOf<17>::Type>::VALUE));
        ASSERT(1 == (IsSame<A18,
                            typename LIST::template TypeOf<18>::Type>::VALUE));
        ASSERT(1 == (IsSame<A19,
                            typename LIST::template TypeOf<19>::Type>::VALUE));
        ASSERT(1 == (IsSame<A20,
                            typename LIST::template TypeOf<20>::Type>::VALUE));

        ASSERT(1 == (IsSame<A1,
               typename bdemf_TypeListTypeOf<1, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
               typename bdemf_TypeListTypeOf<2, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
               typename bdemf_TypeListTypeOf<3, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
               typename bdemf_TypeListTypeOf<4, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
               typename bdemf_TypeListTypeOf<5, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
               typename bdemf_TypeListTypeOf<6, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
               typename bdemf_TypeListTypeOf<7, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
               typename bdemf_TypeListTypeOf<8, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
               typename bdemf_TypeListTypeOf<9, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
              typename bdemf_TypeListTypeOf<10, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
              typename bdemf_TypeListTypeOf<11, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A12,
              typename bdemf_TypeListTypeOf<12, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A13,
              typename bdemf_TypeListTypeOf<13, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A14,
              typename bdemf_TypeListTypeOf<14, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A15,
              typename bdemf_TypeListTypeOf<15, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A16,
              typename bdemf_TypeListTypeOf<16, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A17,
              typename bdemf_TypeListTypeOf<17, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A18,
              typename bdemf_TypeListTypeOf<18, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A19,
              typename bdemf_TypeListTypeOf<19, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A20,
              typename bdemf_TypeListTypeOf<20, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<A1,
                    typename LIST::template TypeOf<1>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A2,
                    typename LIST::template TypeOf<2>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A3,
                    typename LIST::template TypeOf<3>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A4,
                    typename LIST::template TypeOf<4>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A5,
                    typename LIST::template TypeOf<5>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A6,
                    typename LIST::template TypeOf<6>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A7,
                    typename LIST::template TypeOf<7>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A8,
                    typename LIST::template TypeOf<8>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A9,
                    typename LIST::template TypeOf<9>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A10,
                   typename LIST::template TypeOf<10>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A11,
                   typename LIST::template TypeOf<11>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A12,
                   typename LIST::template TypeOf<12>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A13,
                   typename LIST::template TypeOf<13>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A14,
                   typename LIST::template TypeOf<14>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A15,
                   typename LIST::template TypeOf<15>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A16,
                   typename LIST::template TypeOf<16>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A17,
                   typename LIST::template TypeOf<17>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A18,
                   typename LIST::template TypeOf<18>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A19,
                   typename LIST::template TypeOf<19>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<A20,
                   typename LIST::template TypeOf<20>::TypeOrDefault>::VALUE));

        // Test out-of-bounds indices.

        ASSERT(1 == (IsSame<Nil,
               typename bdemf_TypeListTypeOf<0, LIST>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
              typename bdemf_TypeListTypeOf<21, LIST>::TypeOrDefault>::VALUE));

        ASSERT(1 == (IsSame<Nil,
                    typename LIST::template TypeOf<0>::TypeOrDefault>::VALUE));
        ASSERT(1 == (IsSame<Nil,
                   typename LIST::template TypeOf<21>::TypeOrDefault>::VALUE));
    }
};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // ------------------------------------------------------------------
        // TESTING BDEMF_TYPELIST0 .. BDEMF_TYPELIST20
        //
        // Plan:
        //
        // Testing:
        //   BDEMF_TYPELIST
        // ------------------------------------------------------------------

        if (verbose) printf("\nTESTING BDEMF_TYPELIST0 .. BDEMF_TYPELIST20"
                            "\n===========================================\n");

        typedef bdemf_TypeList0 LIST0;
        ASSERT(0 == LIST0::LENGTH);
        ListAsserter0<LIST0>::listAssert();

        typedef bdemf_TypeList1<T1> LIST1;
        ASSERT(1 == LIST1::LENGTH);
        ListAsserter1<LIST1,T1>::listAssert();

        typedef bdemf_TypeList2<T1,T2> LIST2;
        ASSERT(2 == LIST2::LENGTH);
        ListAsserter2<LIST2,T1,T2>::listAssert();

        typedef bdemf_TypeList3<T1,T2,T3> LIST3;
        ASSERT(3 == LIST3::LENGTH);
        ListAsserter3<LIST3,T1,T2,T3>::listAssert();

        typedef bdemf_TypeList4<T1,T2,T3,T4> LIST4;
        ASSERT(4 == LIST4::LENGTH);
        ListAsserter4<LIST4,T1,T2,T3,T4>::listAssert();

        typedef bdemf_TypeList5<T1,T2,T3,T4,T5> LIST5;
        ASSERT(5 == LIST5::LENGTH);
        ListAsserter5<LIST5,T1,T2,T3,T4,T5>::listAssert();

        typedef bdemf_TypeList6<T1,T2,T3,T4,T5,T6> LIST6;
        ASSERT(6 == LIST6::LENGTH);
        ListAsserter6<LIST6,T1,T2,T3,T4,T5,T6>::listAssert();

        typedef bdemf_TypeList7<T1,T2,T3,T4,T5,T6,T7> LIST7;
        ASSERT(7 == LIST7::LENGTH);
        ListAsserter7<LIST7,T1,T2,T3,T4,T5,T6,T7>::listAssert();

        typedef bdemf_TypeList8<T1,T2,T3,T4,T5,T6,T7,T8> LIST8;
        ASSERT(8 == LIST8::LENGTH);
        ListAsserter8<LIST8,T1,T2,T3,T4,T5,T6,T7,T8>::listAssert();

        typedef bdemf_TypeList9<T1,T2,T3,T4,T5,T6,T7,T8,T9> LIST9;
        ASSERT(9 == LIST9::LENGTH);
        ListAsserter9<LIST9,T1,T2,T3,T4,T5,T6,T7,T8,T9>::listAssert();

        typedef bdemf_TypeList10<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10> LIST10;
        ASSERT(10 == LIST10::LENGTH);
        ListAsserter10<LIST10,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>::listAssert();

        typedef bdemf_TypeList11<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11> LIST11;
        ASSERT(11 == LIST11::LENGTH);
        ListAsserter11<LIST11,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::
                                                                  listAssert();

        typedef bdemf_TypeList12<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>
                                                                        LIST12;
        ASSERT(12 == LIST12::LENGTH);
        ListAsserter12<LIST12,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>::
                                                                  listAssert();

        typedef bdemf_TypeList13<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13>
                                                                        LIST13;
        ASSERT(13 == LIST13::LENGTH);
        ListAsserter13<LIST13,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13>::
                                                                  listAssert();

        typedef bdemf_TypeList14<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,
                                                                   T14> LIST14;
        ASSERT(14 == LIST14::LENGTH);
        ListAsserter14<LIST14,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,
                                                            T14>::listAssert();

        typedef bdemf_TypeList15<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,
                                                               T14,T15> LIST15;
        ASSERT(15 == LIST15::LENGTH);
        ListAsserter15<LIST15,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                            T15>::listAssert();

        typedef bdemf_TypeList16<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,
                                                           T14,T15,T16> LIST16;
        ASSERT(16 == LIST16::LENGTH);
        ListAsserter16<LIST16,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                        T15,T16>::listAssert();

        typedef bdemf_TypeList17<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,
                                                       T14,T15,T16,T17> LIST17;
        ASSERT(17 == LIST17::LENGTH);
        ListAsserter17<LIST17,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                    T15,T16,T17>::listAssert();

        typedef bdemf_TypeList18<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,
                                                   T14,T15,T16,T17,T18> LIST18;
        ASSERT(18 == LIST18::LENGTH);
        ListAsserter18<LIST18,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                T15,T16,T17,T18>::listAssert();

        typedef bdemf_TypeList19<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,
                                               T14,T15,T16,T17,T18,T19> LIST19;
        ASSERT(19 == LIST19::LENGTH);
        ListAsserter19<LIST19,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                            T15,T16,T17,T18,T19>::listAssert();

        typedef bdemf_TypeList20<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,
                                           T14,T15,T16,T17,T18,T19,T20> LIST20;
        ASSERT(20 == LIST20::LENGTH);
        ListAsserter20<LIST20,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                        T15,T16,T17,T18,T19,T20>::listAssert();

      } break;
      case 2: {
        // ------------------------------------------------------------------
        // TESTING BDEMF_TYPELIST
        //
        // Plan:
        //
        // Testing:
        //   BDEMF_TYPELIST
        // ------------------------------------------------------------------

        if (verbose) printf("\nTESTING BDEMF_TYPELIST"
                            "\n======================\n");

        typedef bdemf_TypeList<> LIST0;
        ASSERT(0 == LIST0::LENGTH);
        ListAsserter0<LIST0>::listAssert();

        typedef bdemf_TypeList<T1> LIST1;
        ASSERT(1 == LIST1::LENGTH);
        ListAsserter1<LIST1,T1>::listAssert();

        typedef bdemf_TypeList<T1,T2> LIST2;
        ASSERT(2 == LIST2::LENGTH);
        ListAsserter2<LIST2,T1,T2>::listAssert();

        typedef bdemf_TypeList<T1,T2,T3> LIST3;
        ASSERT(3 == LIST3::LENGTH);
        ListAsserter3<LIST3,T1,T2,T3>::listAssert();

        typedef bdemf_TypeList<T1,T2,T3,T4> LIST4;
        ASSERT(4 == LIST4::LENGTH);
        ListAsserter4<LIST4,T1,T2,T3,T4>::listAssert();

        typedef bdemf_TypeList<T1,T2,T3,T4,T5> LIST5;
        ASSERT(5 == LIST5::LENGTH);
        ListAsserter5<LIST5,T1,T2,T3,T4,T5>::listAssert();

        typedef bdemf_TypeList<T1,T2,T3,T4,T5,T6> LIST6;
        ASSERT(6 == LIST6::LENGTH);
        ListAsserter6<LIST6,T1,T2,T3,T4,T5,T6>::listAssert();

        typedef bdemf_TypeList<T1,T2,T3,T4,T5,T6,T7> LIST7;
        ASSERT(7 == LIST7::LENGTH);
        ListAsserter7<LIST7,T1,T2,T3,T4,T5,T6,T7>::listAssert();

        typedef bdemf_TypeList<T1,T2,T3,T4,T5,T6,T7,T8> LIST8;
        ASSERT(8 == LIST8::LENGTH);
        ListAsserter8<LIST8,T1,T2,T3,T4,T5,T6,T7,T8>::listAssert();

        typedef bdemf_TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9> LIST9;
        ASSERT(9 == LIST9::LENGTH);
        ListAsserter9<LIST9,T1,T2,T3,T4,T5,T6,T7,T8,T9>::listAssert();

        typedef bdemf_TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10> LIST10;
        ASSERT(10 == LIST10::LENGTH);
        ListAsserter10<LIST10,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>::listAssert();

        typedef bdemf_TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11> LIST11;
        ASSERT(11 == LIST11::LENGTH);
        ListAsserter11<LIST11,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::
                                                                  listAssert();

        typedef bdemf_TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12> LIST12;
        ASSERT(12 == LIST12::LENGTH);
        ListAsserter12<LIST12,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>::
                                                                  listAssert();

        typedef bdemf_TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13>
                                                                        LIST13;
        ASSERT(13 == LIST13::LENGTH);
        ListAsserter13<LIST13,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13>::
                                                                  listAssert();

        typedef bdemf_TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14>
                                                                        LIST14;
        ASSERT(14 == LIST14::LENGTH);
        ListAsserter14<LIST14,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14>::
                                                                  listAssert();

        typedef bdemf_TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                                   T15> LIST15;
        ASSERT(15 == LIST15::LENGTH);
        ListAsserter15<LIST15,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                            T15>::listAssert();

        typedef bdemf_TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                               T15,T16> LIST16;
        ASSERT(16 == LIST16::LENGTH);
        ListAsserter16<LIST16,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                        T15,T16>::listAssert();

        typedef bdemf_TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                           T15,T16,T17> LIST17;
        ASSERT(17 == LIST17::LENGTH);
        ListAsserter17<LIST17,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                    T15,T16,T17>::listAssert();

        typedef bdemf_TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                       T15,T16,T17,T18> LIST18;
        ASSERT(18 == LIST18::LENGTH);
        ListAsserter18<LIST18,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                T15,T16,T17,T18>::listAssert();

        typedef bdemf_TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                                   T15,T16,T17,T18,T19> LIST19;
        ASSERT(19 == LIST19::LENGTH);
        ListAsserter19<LIST19,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
                                            T15,T16,T17,T18,T19>::listAssert();

        typedef bdemf_TypeList<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,
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
        //
        // ------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        {
            typedef int    INT;
            typedef double DOUBLE;
            ASSERT(1 == (IsSame<INT, INT>::VALUE));
            ASSERT(0 == (IsSame<INT, DOUBLE>::VALUE));

            ASSERT(1 == (IsSame<T1, T1>::VALUE));
            ASSERT(0 == (IsSame<T1, T2>::VALUE));
        }

        {
            typedef bdemf_TypeList<T1, T2> LIST;
            ASSERT(2 == LIST::LENGTH);

            ASSERT(1 == (IsSame<T1, LIST::Type1>::VALUE));
            ASSERT(1 == (IsSame<T2, LIST::Type2>::VALUE));

            ASSERT(1 == (IsSame<T1,
                                bdemf_TypeListTypeOf<1, LIST>::Type>::VALUE));

            ASSERT(1 == (IsSame<T2, LIST::TypeOf<2>::Type>::VALUE));
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
