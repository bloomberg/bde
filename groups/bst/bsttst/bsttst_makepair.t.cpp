// bsttst_makepair.t.cpp                                              -*-C++-*-

#include <bsttst_makepair.h>

#include <bslalg_typetraits.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_cstdio.h>      // printf
#include <bsl_cstdlib.h>     // atoi
#include <bsl_iostream.h>
#include <bsl_utility.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//-----------------------------------------------------------------------------
// [1] bsl::pair<T1, T2> bsl::make_pair(const T1&, const T2&);
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_                                    // Print tab.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bslalg::TypeTraitBitwiseMoveable              BitwiseMoveable;
typedef bslalg::TypeTraitBitwiseCopyable              BitwiseCopyable;
typedef bslalg::TypeTraitHasTrivialDefaultConstructor TrivialConstructor;
typedef bslalg::TypeTraitUsesBslmaAllocator           UsesBdemaAllocator;

//=============================================================================
//                          HELPER CLASS FOR TESTING
//-----------------------------------------------------------------------------

struct my_MoveAbandonBdema
{
    BSLALG_DECLARE_NESTED_TRAITS2(my_MoveAbandonBdema,
                                  BitwiseMoveable,
                                  UsesBdemaAllocator);
    my_MoveAbandonBdema() { }
    my_MoveAbandonBdema(const my_MoveAbandonBdema&, bslma::Allocator*) { }
};

struct my_CopyTrivial
{
    BSLALG_DECLARE_NESTED_TRAITS2(my_CopyTrivial,
                                  BitwiseCopyable,
                                  TrivialConstructor);
};

struct my_NoTraits
{
};

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1 : {
        // --------------------------------------------------------------------
        // TESTING make_pair
        //
        // Concerns:
        // - Calling bsl::make_pair on types with and without allocators
        //   returns pair of correct type.
        //
        // Plan:
        // - Call 'bsl::make_pair' with a number of types, each having one
        //   or more of the type traits in which we are interested.
        // - Setting the default allocator to a test allocator, verify that no
        //   memory is leaked.
        //
        // Testing:
        //     template <typename T1, typename T2>
        //     pair<T1, T2> make_pair(const T1& a, const T2& b);
        // --------------------------------------------------------------------

        if (verbose) bsl::printf("\nTESTING make_pair"
                                 "\n=================\n");

        bslma::TestAllocator ta;
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            bsl::printf("Testing make_pair returning "
                        "bsl::pair<my_NoTraits, my_NoTraits>\n");
        }
        {
            bslma::DefaultAllocatorGuard ag(&ta);
            my_NoTraits v1; my_NoTraits v2;
            typedef bsl::pair<my_NoTraits, my_NoTraits> Obj;
            Obj x = bsl::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            bsl::printf("Testing make_pair returning "
                        "bsl::pair<my_MoveAbandonBdema, "
                        "my_MoveAbandonBdema>\n");
        }
        {
            bslma::DefaultAllocatorGuard ag(&ta);
            my_MoveAbandonBdema v1; my_MoveAbandonBdema v2;
            typedef bsl::pair<my_MoveAbandonBdema, my_MoveAbandonBdema> Obj;
            Obj x = bsl::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            bsl::printf("Testing make_pair returning "
                        "bsl::pair<my_CopyTrivial, my_CopyTrivial>\n");
        }
        {
            bslma::DefaultAllocatorGuard ag(&ta);
            my_CopyTrivial v1; my_CopyTrivial v2;
            typedef bsl::pair<my_CopyTrivial, my_CopyTrivial> Obj;
            Obj x = bsl::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            bsl::printf("Testing make_pair returning "
                        "bsl::pair<my_CopyTrivial, my_MoveAbandonBdema>\n");
        }
        {
            bslma::DefaultAllocatorGuard ag(&ta);
            my_CopyTrivial v1; my_MoveAbandonBdema v2;
            typedef bsl::pair<my_CopyTrivial, my_MoveAbandonBdema> Obj;
            Obj x = bsl::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            bsl::printf("Testing make_pair returning "
                        "bsl::pair<my_MoveAbandonBdema, my_CopyTrivial>\n");
        }
        {
            bslma::DefaultAllocatorGuard ag(&ta);
            my_MoveAbandonBdema v1; my_CopyTrivial v2;
            typedef bsl::pair<my_MoveAbandonBdema, my_CopyTrivial> Obj;
            Obj x = bsl::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            bsl::printf("Testing make_pair returning "
                        "bsl::pair<my_MoveAbandonBdema, my_NoTraits>\n");
        }
        {
            bslma::DefaultAllocatorGuard ag(&ta);
            my_MoveAbandonBdema v1; my_NoTraits v2;
            typedef bsl::pair<my_MoveAbandonBdema, my_NoTraits> Obj;
            Obj x = bsl::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            bsl::printf("Testing make_pair returning "
                        "bsl::pair<my_CopyTrivial, my_NoTraits>\n");
        }
        {
            bslma::DefaultAllocatorGuard ag(&ta);
            my_CopyTrivial v1; my_NoTraits v2;
            typedef bsl::pair<my_CopyTrivial, my_NoTraits> Obj;
            Obj x = bsl::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            bsl::printf("Testing make_pair returning "
                        "bsl::pair<my_MoveAbandonBdema, int>\n");
        }
        {
            bslma::DefaultAllocatorGuard ag(&ta);
            my_MoveAbandonBdema v1; int v2 = 4;
            typedef bsl::pair<my_MoveAbandonBdema, int> Obj;
            Obj x = bsl::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            bsl::printf("Testing make_pair returning "
                        "bsl::pair<int, my_CopyTrivial>\n");
        }
        {
            bslma::DefaultAllocatorGuard ag(&ta);
            int v1 = 4; my_CopyTrivial v2;
            typedef bsl::pair<int, my_CopyTrivial> Obj;
            Obj x = bsl::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            bsl::printf("Testing make_pair returning "
                        "bsl::pair<pair<int, int>, pair<int, int> >\n");
        }
        {
            bslma::DefaultAllocatorGuard ag(&ta);
            bsl::pair<int, int> v1; bsl::pair<int, int> v2;
            typedef bsl::pair<bsl::pair<int, int>, bsl::pair<int, int> > Obj;
            Obj x = bsl::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        typedef bsl::pair<int, int> IntPair;
        typedef bsl::pair<const IntPair, IntPair> IntPairPair;
        IntPair y = bsl::make_pair(2, 3);
        ASSERT(2 == y.first);
        ASSERT(3 == y.second);
        IntPairPair z(y, IntPair());
        ASSERT(2 == z.first.first);
        ASSERT(3 == z.first.second);

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
