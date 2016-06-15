// bslma_stdtestallocator.t.cpp                                       -*-C++-*-

#include <bslma_stdtestallocator.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bsls_bsltestutil.h>

#include <limits>
#include <new>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

// TBD: fix this up
//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// An allocator is a value-semantic type whose value consists of a single
// pointer to a 'bslma::Allocator' object (its underlying "mechanism").  This
// pointer can be set at construction (and if 0 is passed, then it uses
// 'bslma_default' to substitute a pointer to the currently installed default
// allocator), and it can be accessed through the 'mechanism' accessor.  It
// cannot be reset, however, since normally an allocator does not change during
// the lifetime of an object.  A 'bsl::allocator' is parameterized by the type
// that it allocates, and that influences the behavior of several manipulators
// and accessors, mainly depending on the size of that type.  The same
// 'bsl::allocator" can be re-parameterized for another type ("rebound") using
// the 'rebind' nested template.
//
// Although 'bsl::allocator' is a value-semantic type, the fact that its value
// is fixed at construction and not permitted to change let us relax the usual
// concerns of a typical value-semantic type.  Our specific concerns are that
// an allocator constructed with a certain underlying mechanism actually uses
// that mechanism to allocate memory, and that its rebound versions also do.
// Another concern is that the 'max_size' is the maximum possible size for that
// type (i.e., it is impossible to meaningfully pass in a larger size), and
// that the 'size_type' is unsigned, the 'difference_type' is signed, and
// generally all the requirements of C++ standard allocators are met (20.1.2
// [allocator.requirements]).
//-----------------------------------------------------------------------------
// [ 3] StdTestAllocator();
// [ 3] StdTestAllocator(bslma::Allocator *);
// [ 3] StdTestAllocator(const StdTestAllocator&);
// [ 3] StdTestAllocator(const StdTestAllocator<U>&);
// [  ] ~StdTestAllocator();
//
// Modifiers
// [  ] StdTestAllocator& operator=(const allocator& rhs);
// [  ] pointer allocate(size_type n, const void *hint = 0);
// [  ] void deallocate(pointer p, size_type n = 1);
// [  ] void construct(pointer p, const TYPE& val);
// [  ] void destroy(pointer p);
//
// Accessors
// [  ] pointer address(reference x) const;
// [  ] const_pointer address(const_reference x) const;
// [ 4] bslma::Allocator *mechanism() const;
// [ 4] size_type max_size() const;
//
// Nested types
// [ 5] StdTestAllocator::size_type
// [ 5] StdTestAllocator::difference_type
// [ 5] StdTestAllocator::pointer;
// [ 5] StdTestAllocator::const_pointer;
// [ 5] StdTestAllocator::reference;
// [ 5] StdTestAllocator::const_reference;
// [ 5] StdTestAllocator::value_type;
// [ 5] template rebind<U>::other
//
// Free functions (operators)
// [ 4] bool operator==(StdTestAllocator<T>,  StdTestAllocator<T>);
// [  ] bool operator==(StdTestAllocator<T1>,  StdTestAllocator<T2>);
// [ 4] bool operator==(bslma::Allocator *, StdTestAllocator<T>);
// [ 4] bool operator==(StdTestAllocator<T>,  bslma::Allocator*);
// [  ] bool operator!=(StdTestAllocator<T1>,  StdTestAllocator<T2>);
// [  ] bool operator!=(bslma::Allocator *, StdTestAllocator<T>);
// [  ] bool operator!=(StdTestAllocator<T>,  bslma::Allocator*);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE
// [ 2] bsl::is_trivially_copyable<StdTestAllocator>
// [ 2] bslmf::IsBitwiseEqualityComparable<sl::allocator>
// [ 2] bslmf::IsBitwiseMoveable<StdTestAllocator>

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

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                           PRINTF FORMAT MACROS
// ----------------------------------------------------------------------------
#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                            USAGE EXAMPLE
//-----------------------------------------------------------------------------

                              // ===============
                              // struct MyObject
                              // ===============

struct MyObject
{
    // A non-trivial-sized object.

    // DATA
    int  d_i;
    char d_s[10];
};

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

    (void) veryVerbose;
    (void) veryVeryVerbose;

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:  The usage example must compile end execute without
        //   errors.
        //
        // Plan:  Copy-paste the usage example and replace 'assert' by
        //   'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        bslma::TestAllocator ta("default for usage", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard allocGuard(&ta);

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        if (verbose) {
            printf("This test has not yet been implemented.\n");
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING NESTED TYPES
        //
        // Concerns:
        //   o that 'size_type' is unsigned while 'difference_type' is signed.
        //   o that size_type and difference_type are the right size (i.e.,
        //     they can represent any difference of pointers in the memory
        //     model)
        //   o that all other types exist and are as specified by the C++
        //     standard
        //   o that if Y is X::rebind<U>::other, then Y::rebind<T>::other is
        //     the same type as X
        //
        // Plan: The testing is straightforward and follows the concerns.
        //
        // Testing:
        //   bslma::StdTestAllocator::size_type
        //   bslma::StdTestAllocator::difference_type
        //   bslma::StdTestAllocator::pointer;
        //   bslma::StdTestAllocator::const_pointer;
        //   bslma::StdTestAllocator::reference;
        //   bslma::StdTestAllocator::const_reference;
        //   bslma::StdTestAllocator::value_type;
        //   template rebind<U>::other
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING NESTED TYPES"
                            "\n====================\n");

        typedef bslma::StdTestAllocator<int>   AI;
        typedef bslma::StdTestAllocator<float> AF;

        if (verbose) printf("\tTesting 'size_type'.\n");
        {
            ASSERT(sizeof(AI::size_type) == sizeof(int*));

            ASSERT(0 < ~(AI::size_type)0);
        }

        if (verbose) printf("\tTesting 'difference_type'.\n");
        {
            ASSERT(sizeof(AI::difference_type) == sizeof(int*));

            ASSERT(0 > ~(AI::difference_type)0);
        }

        if (verbose) printf("\tTesting 'pointer'.\n");
        {
            ASSERT((bsl::is_same<AI::pointer, int*>::value));
            ASSERT((bsl::is_same<AF::pointer, float*>::value));
        }

        if (verbose) printf("\tTesting 'const_pointer'.\n");
        {
            ASSERT((bsl::is_same<AI::const_pointer, const int*>::value));
            ASSERT((bsl::is_same<AF::const_pointer, const float*>::value));
        }

        if (verbose) printf("\tTesting 'reference'.\n");
        {
            ASSERT((bsl::is_same<AI::reference, int&>::value));
            ASSERT((bsl::is_same<AF::reference, float&>::value));
        }

        if (verbose) printf("\tTesting 'const_reference'.\n");
        {
            ASSERT((bsl::is_same<AI::const_reference, const int&>::value));
            ASSERT((bsl::is_same<AF::const_reference, const float&>::value));
        }

        if (verbose) printf("\tTesting 'value_type'.\n");
        {
            ASSERT((bsl::is_same<AI::value_type, int>::value));
            ASSERT((bsl::is_same<AF::value_type, float>::value));
        }

        if (verbose) printf("\tTesting 'rebind'.\n");
        {
            ASSERT((bsl::is_same<AI::rebind<int  >::other, AI>::value));
            ASSERT((bsl::is_same<AI::rebind<float>::other, AF>::value));
            ASSERT((bsl::is_same<AF::rebind<int  >::other, AI>::value));
            ASSERT((bsl::is_same<AF::rebind<float>::other, AF>::value));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS
        //
        // Concerns:
        //   o that the correct 'bslma::Allocator*' is returned by 'mechanism'.
        //   o that the result of 'max_size' fits and represents the maximum
        //     possible number of bytes in a 'bslma::Allocator::size_type'.
        //   o that all comparisons exist and resolve to comparing the
        //     mechanisms.
        //
        // Plan: The concerns are straightforward to test.
        //
        // Testing:
        //   bslma::Allocator *mechanism() const;
        //   size_type max_size() const;
        //   bool operator==(StdTestAllocator<T>,  StdTestAllocator<T>);
        //   bool operator==(bslma::Allocator *, StdTestAllocator<T>);
        //   bool operator==(StdTestAllocator<T>,  bslma::Allocator*);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ACCESSORS"
                            "\n=================\n");

        bslma::TestAllocator da("default");
        bslma::Default::setDefaultAllocatorRaw(&da);

        if (verbose) printf("\tTesting 'mechanism()'.\n");
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);

            bslma::StdTestAllocator<int>  ai1;
            ASSERT(&da == ai1.mechanism());
            bslma::StdTestAllocator<int>  ai2(&ta);
            ASSERT(&ta  == ai2.mechanism());
            bslma::StdTestAllocator<int>  ai4(0);
            ASSERT(&da == ai4.mechanism());

            bslma::StdTestAllocator<double>  ad1;
            ASSERT(&da == ai1.mechanism());
            bslma::StdTestAllocator<double>  ad2(&ta);
            ASSERT(&ta  == ai2.mechanism());
            bslma::StdTestAllocator<double>  ad4(0);
            ASSERT(&da == ai4.mechanism());

            bslma::StdTestAllocator<int>  ai5(ad2);
            ASSERT(&ta  == ai5.mechanism());
            bslma::StdTestAllocator<double> ad5(ai2);
            ASSERT(&ta  == ad5.mechanism());
        }

        if (verbose) printf("\tTesting 'max_size()'.\n");
        {
            typedef bslma::Allocator::size_type bsize;

            bslma::StdTestAllocator<char> charAlloc;
            bsize cas = charAlloc.max_size();

            // verify that max_size() is the largest positive integer of type
            // size_type
            LOOP_ASSERT(cas, cas > 0);
            LOOP_ASSERT(cas, cas == std::numeric_limits<bsize>::max());

            if (verbose) {
                printf("cas = " ZU "\n", cas);
            }

            bslma::StdTestAllocator<MyObject> objAlloc;

            // Detect problem with MSVC in 64-bit mode, which can't do 64-bit
            // int arithmetic correctly for enums.
            ASSERT(objAlloc.max_size() < charAlloc.max_size());

            bsize oas = objAlloc.max_size();
            bsize oass = oas * sizeof(MyObject);
            bsize oassplus = oass + sizeof(MyObject);
            LOOP_ASSERT(oas, oas > 0);
            LOOP_ASSERT(oass, oass < cas);
            LOOP_ASSERT(oass, oass > oas);         // no overflow
            LOOP_ASSERT(oassplus, oassplus < oas); // overflow

            if (verbose) {
                printf("\tAs unsigned long: oas = " ZU ", oass = " ZU ", "
                       "oassplus = " ZU ".\n", oas, oass, oassplus);
            }
        }

        if (verbose) printf("\tTesting 'operator=='.\n");
        {
            bslma::TestAllocator ta1(veryVeryVeryVerbose);
            bslma::TestAllocator ta2(veryVeryVeryVerbose);

            bslma::StdTestAllocator<int>  ai1(&ta1);
            bslma::StdTestAllocator<int>  ai2(&ta2);

            bslma::StdTestAllocator<double> ad1(&ta1);
            bslma::StdTestAllocator<double> ad2(&ta2);

            // One of lhs or rhs is 'bslma::Allocator *'.

            ASSERT(&ta1 == ai1);  ASSERT(ai1 == &ta1);
            ASSERT(&ta2 != ai1);  ASSERT(ai1 != &ta2);

            ASSERT(&ta1 == ai1);  ASSERT(ai1 == &ta1);
            ASSERT(&ta2 != ai1);  ASSERT(ai1 != &ta2);

            ASSERT(&ta1 == ad1);  ASSERT(ad1 == &ta1);
            ASSERT(&ta2 != ad1);  ASSERT(ad1 != &ta2);

            ASSERT(&ta1 == ad1);  ASSERT(ad1 == &ta1);
            ASSERT(&ta2 != ad1);  ASSERT(ad1 != &ta2);

            // Both lhs and rhs are 'bslma::StdTestAllocator'.

            ASSERT(ai1 == ai1);  ASSERT(ai1 != ai2);
            ASSERT(ai1 == ad1);  ASSERT(ai1 != ad2);

            ASSERT(ad1 == ai1);  ASSERT(ad1 != ai2);
            ASSERT(ad1 == ad1);  ASSERT(ad1 != ad2);

            ASSERT(ai2 != ai1);  ASSERT(ai2 == ai2);
            ASSERT(ai2 != ad1);  ASSERT(ai2 == ad2);

            ASSERT(ad2 != ai1);  ASSERT(ad2 == ai2);
            ASSERT(ad2 != ad1);  ASSERT(ad2 == ad2);
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //
        // Concerns:
        //   o that an allocator can be constructed from the various
        //     constructors and that it uses the correct mechanism object.
        //   o that an allocator can be constructed from an allocator to a
        //     different type
        //
        // Plan:  We construct a number of allocators from various mechanisms,
        //   and test that they do compare equal to the selected mechanism.
        //   Copy constructed allocators have to compare equal to their
        //   original values.
        //
        // Testing:
        //   StdTestAllocator();
        //   StdTestAllocator(bslma::Allocator *);
        //   StdTestAllocator(const StdTestAllocator&);
        //   StdTestAllocator(const StdTestAllocator<U>&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONSTRUCTORS"
                            "\n====================\n");

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&da);

        bslma::TestAllocator ta("test case 3", veryVeryVeryVerbose);

        bslma::StdTestAllocator<int>  ai1;        ASSERT(&da  == ai1);
        bslma::StdTestAllocator<int>  ai2(&ta);   ASSERT(&ta  == ai2);
        bslma::StdTestAllocator<int>  ai3(ai2);   ASSERT(&ta  == ai3);
        bslma::StdTestAllocator<int>  ai4(0);     ASSERT(&da == ai4);

        bslma::StdTestAllocator<double> ad1;        ASSERT(&da == ad1);
        bslma::StdTestAllocator<double> ad2(&ta);   ASSERT(&ta  == ad2);
        bslma::StdTestAllocator<double> ad3(ad2);   ASSERT(&ta  == ad3);
        bslma::StdTestAllocator<double> ad4(0);     ASSERT(&da == ad4);

        bslma::StdTestAllocator<int>  ai5(ad2);     ASSERT(ad2  == ai5);
        bslma::StdTestAllocator<double> ad5(ai2);   ASSERT(ai2  == ad5);

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TRAITS
        //
        // Concerns:
        //   That an allocator has the proper traits defined.
        //
        // Plan: Since it does not matter what type 'StdTestAllocator' is
        // instantiated with, use 'int' and test for each expected trait.
        // Note that 'void' also needs to be tested since it is a
        // specialization.
        //
        // Testing:
        //   bsl::is_trivially_copyable<StdTestAllocator>
        //   bslmf::IsBitwiseEqualityComparable<StdTestAllocator>
        //   bslmf::IsBitwiseMoveable<StdTestAllocator>
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TRAITS"
                            "\n==============\n");

        ASSERT((bslmf::IsBitwiseMoveable<bslma::StdTestAllocator<int> >::value));
        ASSERT((bsl::is_trivially_copyable<
                                         bslma::StdTestAllocator<int> >::value));
        ASSERT((bslmf::IsBitwiseEqualityComparable<
                                         bslma::StdTestAllocator<int> >::value));

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //    This test case exercises the component but *tests* nothing.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma::TestAllocator ta("breathing test", veryVeryVeryVerbose);

#if 0
        my_FixedSizeArray<int, bslma::StdTestAllocator<int> > a1(5, &ta);

        ASSERT(5 == a1.length());
//        ASSERT(bslma::Default::defaultAllocator() == a1.allocator());
        ASSERT(&ta == a1.allocator());

        for (int i = 0; i < a1.length(); ++i) {
            a1[i] = i + 1;
        }

        my_CountingAllocator countingAlloc;
        my_FixedSizeArray<int, bslma::StdTestAllocator<int> > a2(a1,
                                                               &countingAlloc);

        ASSERT(a1 == a2);
        ASSERT(a1.allocator() != a2.allocator());
        ASSERT(&countingAlloc == a2.allocator());
        ASSERT(1 == countingAlloc.blocksOutstanding());

        // Test that this will compile:

        bslma::StdTestAllocator<void> voidAlloc(&countingAlloc);
#endif
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the default allocator.
    ASSERTV(defaultAllocator.numBlocksTotal(),
            0 == defaultAllocator.numBlocksTotal());

    // CONCERN: In no case does memory come from the global allocator.
    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

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
