// bslalg_bidirectionallinklistnode.t.cpp                             -*-C++-*-
#include <bslalg_bidirectionalnode.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// Global Concerns:
//: o Pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated.
//: o Precondition violations are detected in appropriate build modes.
//-----------------------------------------------------------------------------
// MANIPULATORS
// [ 2] VALUE_TYPE& value();
// [ 2] const VALUE_TYPE& value() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
// [  ] CONCERN: 'value' can be constructed with 'allocator_traits'.

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
#ifdef BSLS_PLATFORM__CPU_32_BIT
#define SUFFICIENTLY_LONG_STRING "123456789012345678901234567890123"
#else  // 64_BIT
#define SUFFICIENTLY_LONG_STRING "12345678901234567890123456789012" \
                                 "123456789012345678901234567890123"
#endif
//BSLMF_ASSERT(sizeof SUFFICIENTLY_LONG_STRING > sizeof(bsl::string));

class TestType1 {
    static bool s_constructedFlag;

  public:
    TestType1() { s_constructedFlag = true; }
    static bool isConstructed() { return s_constructedFlag; }
    static void reset() { s_constructedFlag = false; }
};

bool TestType1::s_constructedFlag = false;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test                = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose             = argc > 2;
    bool veryVerbose         = argc > 3;
    bool veryVeryVerbose     = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: In no case is memory allocated from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
#if 0
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS AND BASIC ACCESSORS
        //
        // Concerns:
        //: 1 Manipulators can set value.
        //:
        //: 2 Accessor return value set by maninpulator.
        //:
        //: 3 Accessor is declared const.
        //
        // Plan:
        //: 1 Create a 'BidirectionalNode' with 'VALUE_TYPE' as 'int' and set 'value'
        //:   distinct numbers.  Verify the values are set with the accessor.
        //:
        //: 2 Create a 'BidirectionalNode' with a type that has a constructor that can
        //:   be verified if it has been invoked.  Verify that the constructor
        //:   is invoked when 'allocator_traits::construct' is used.
        //
        // Testing:
        //   VALUE_TYPE& value();
        //   const VALUE_TYPE& value() const;
        // --------------------------------------------------------------------
        bslma::TestAllocator da("default");
        bslma::TestAllocator oa("object");

        bslma::DefaultAllocatorGuard defaultGuard(&da);

        if (verbose) printf("\nTesting manipulator and accessor for 'int'.\n");
        {
            typedef int Type;
            typedef bslalg::BidirectionalNode<Type> Obj;

            //typedef bsl::allocator<Obj>          Alloc;
            //typedef bsl::allocator_traits<Alloc> AllocTraits;

            Alloc allocator(&oa);
            Obj *xPtr = AllocTraits::allocate(allocator, 1);
            Obj& mX = *xPtr; const Obj& X = mX;

            mX.value() = 0;
            ASSERTV(X.value(), 0 == X.value());

            mX.value() = 1;
            ASSERTV(X.value(), 1 == X.value());

            mX.value() = INT_MAX;
            ASSERTV(X.value(), INT_MAX == X.value());

            ASSERTV(0 == da.numBlocksTotal());
            ASSERTV(1 == oa.numBlocksInUse());

            mX.value().~Type();
            AllocTraits::deallocate(allocator, &mX, 1);
            ASSERTV(0 == oa.numBlocksInUse());
        }

        if (verbose) printf(
                         "\nTesting manipulator and accessor for 'string'.\n");
        {
            //typedef bsl::string    Type;
            typedef BidirectionalNode<Type> Obj;

            //typedef bsl::allocator<Obj>          Alloc;
            //typedef bsl::allocator_traits<Alloc> AllocTraits;

            Alloc allocator(&oa);
            Obj *xPtr = AllocTraits::allocate(allocator, 1);
            Obj& mX = *xPtr; const Obj& X = mX;

            AllocTraits::construct(allocator,
                                   bsls::Util::addressOf(mX.value()));

            const char D[] = "";
            const char A[] = "a_" SUFFICIENTLY_LONG_STRING;

            bslma::TestAllocator scratch("scratch");
            const bsl::string B("ABC", &scratch);

            Type value(&scratch);

            mX.value() = D;
            value = X.value();
            ASSERTV(value, D == value);
            ASSERTV(1 == oa.numBlocksInUse());

            mX.value() = A;
            value = X.value();
            ASSERTV(value, A == value);
            ASSERTV(2 == oa.numBlocksInUse());

            mX.value() = B;
            value = X.value();
            ASSERTV(value, B == value);
            ASSERTV(2 == oa.numBlocksInUse());

            ASSERTV(0 == da.numBlocksTotal());

            mX.value().~Type();
            AllocTraits::deallocate(allocator, &mX, 1);
            ASSERTV(0 == oa.numBlocksInUse());
        }
      } break;
#endif 
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma::TestAllocator da("default");
        bslma::DefaultAllocatorGuard defaultGuard(&da);

        typedef bslalg::BidirectionalNode<int> Obj;
        Obj *xPtr = static_cast<Obj *>(da.allocate(sizeof(Obj)));
        Obj& mX = *xPtr; const Obj& X = mX;

        mX.value() = 0;
        ASSERTV(X.value(), 0 == X.value());

        mX.value() = 1;
        ASSERTV(X.value(), 1 == X.value());

        da.deallocate(&mX);
        ASSERTV(0 == da.numBytesInUse());

       } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case is memory allocated from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
