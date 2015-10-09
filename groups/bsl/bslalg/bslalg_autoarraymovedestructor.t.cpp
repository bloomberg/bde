// bslalg_autoarraymovedestructor.t.cpp                               -*-C++-*-

#include <bslalg_autoarraymovedestructor.h>

#include <bslalg_autoarraydestructor.h>          // for testing only
#include <bslalg_hastrait.h>                     // for testing only
#include <bslalg_scalarprimitives.h>             // for testing only

#include <bslma_allocator.h>                     // for testing only
#include <bslma_default.h>                       // for testing only
#include <bslma_testallocator.h>                 // for testing only
#include <bslma_testallocatorexception.h>        // for testing only
#include <bslma_usesbslmaallocator.h>            // for testing only
#include <bslmf_isbitwisemoveable.h>             // for testing only
#include <bsls_alignmentutil.h>                  // for testing only
#include <bsls_assert.h>
#include <bsls_asserttest.h>                     // for testing only
#include <bsls_bsltestutil.h>
#include <bsls_stopwatch.h>                      // for testing only

#include <ctype.h>      // 'isalpha'
#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>     // 'strlen'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component to be tested provides a proctor to help with exception-safety
// guarantees.  The test sequence is very simple: we only have to ascertain
// that the index computation is correct and that the proctor does destroy its
// guarded range unless 'release has been called.  We use a test type that
// monitors the number of constructions and destructions, and that allocates in
// order to take advantage of the standard 'bslma' exception test.
//-----------------------------------------------------------------------------
// [ 2] bslalg::AutoArrayMoveDestructor(T *b, T *e);
// [ 2] ~bslalg::AutoArrayMoveDestructor();
// [ 2] T *advance();
// [ 2] T *begin() const;
// [ 2] T *middle() const;
// [ 2] T *end() const;
// [ 2] T *destination() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE

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
//                  GLOBAL TYPEDEFS/CONSTANTS/TYPES FOR TESTING
//-----------------------------------------------------------------------------

// TYPES
class TestType;

typedef TestType                      T;    // uses 'bslma' allocators

// STATIC DATA
const int MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

static int numDefaultCtorCalls = 0;
static int numCharCtorCalls    = 0;
static int numCopyCtorCalls    = 0;
static int numAssignmentCalls  = 0;
static int numDestructorCalls  = 0;

bslma::TestAllocator *Z;  // initialized at the start of main()

//=============================================================================
//                            USAGE EXAMPLE
//-----------------------------------------------------------------------------

// Note that the class 'TestType' is used both in the usage example and in
// some other cases.

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Doubling the Length of an Array
/// - - - - - - - - - - - - - - - - - - - - -
// First, we create the class 'TestType', which is bitwise-movable and
// allocates memory upon construction:
//..
                               // ==============
                               // class TestType
                               // ==============
//
class TestType {
    // This test type contains a 'char' in some allocated storage.  It
    // counts the number of default and copy constructions, assignments,
    // and destructions.  It has no traits other than using a 'bslma'
    // allocator.  It could have the bit-wise moveable traits but we defer
    // that trait to the 'MoveableTestType'.
//
    char             *d_data_p;
    bslma::Allocator *d_allocator_p;
//
  public:
    // CREATORS
    explicit TestType(bslma::Allocator *basicAllocator = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        ++numDefaultCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = '?';
    }
//
    explicit TestType(char c, bslma::Allocator *basicAllocator = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        ++numCharCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = c;
    }
//
    TestType(const TestType&   original,
             bslma::Allocator *basicAllocator = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        ++numCopyCtorCalls;
        if (&original != this) {
            d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
            *d_data_p = *original.d_data_p;
        }
    }
//
    ~TestType()
    {
        ++numDestructorCalls;
        *d_data_p = '_';
        d_allocator_p->deallocate(d_data_p);
        d_data_p = 0;
    }
//
    // MANIPULATORS
    TestType& operator=(const TestType& rhs)
    {
        ++numAssignmentCalls;
        if (&rhs != this) {
            char *newData = (char *)d_allocator_p->allocate(sizeof(char));
            *d_data_p = '_';
            d_allocator_p->deallocate(d_data_p);
            d_data_p  = newData;
            *d_data_p = *rhs.d_data_p;
        }
        return *this;
    }
//
    void setDatum(char c) { *d_data_p = c; }
//
    // ACCESSORS
    char datum() const { return *d_data_p; }

    void print() const
    {
        if (d_data_p) {
            ASSERT(isalpha(*d_data_p));
            printf("%c (int: %d)\n", *d_data_p, (int)*d_data_p);
        } else {
            printf("VOID\n");
        }
    }
};
//
// FREE OPERATORS
bool operator==(const TestType& lhs, const TestType& rhs)
{
    return lhs.datum() == rhs.datum();
}
//
// TRAITS
namespace BloombergLP {
//
namespace bslma {
template <> struct UsesBslmaAllocator<TestType> : bsl::true_type {};
}  // close namespace bslma
//
namespace bslmf {
template <> struct IsBitwiseMoveable<TestType> : bsl::true_type {};
}  // close namespace bslmf
//
}  // close enterprise namespace
//..
// Then, we define the function 'insertItems' which uses
// 'AutoArrayMoveDestructor' to ensure that if an exception is thrown (e.g.,
// when allocating memory), the array will be left in a state where it has the
// same number of elements, in the same location, as when the function begin
// (though not necessarily the same value).
//..
void insertItems(TestType         *start,
                 TestType         *divider,
                 const TestType    value,
                 bslma::Allocator *allocator)
    // The memory in the specified range '[ start, divider )' contains
    // valid elements, and the range of valid elements is to be doubled by
    // inserting 'divider - start' copies of the specified 'value' at
    // 'start', shifting the existing valid values back in memory.  Assume
    // that following the pointer 'divider' is sufficient uninitialized
    // memory, and that the type 'TestType' is bitwise-movable
    // ('AutoArrayMoveDestructor' will only work bitwise-movable types).
{
    TestType *finish = divider + (divider - start);
//
    BSLMF_ASSERT(bslmf::IsBitwiseMoveable< TestType>::value);
    BSLMF_ASSERT(bslma::UsesBslmaAllocator<TestType>::value);

    // The range '[ start, divider )' contains valid elements.  The range
    // '[ divider, finish )' is of equal length and contains uninitialized
    // memory.  We want to insert 'divider - start' copies of the specified
    // 'value' at the front half of the range '[ start, finish )', moving
    // the exising elements back to make room for them.  Note that the copy
    // c'tor of 'TestType' allocates memory and may throw, so we have to
    // leave the array in a somewhat predicatable state if we do throw.
    // What the bslalg::AutoArrayMoveDestructor will do is guarantee that,
    // if it is destroyed before the insertion is complete, the range
    // '[ start, divider )' will contain valid elements, and that no other
    // valid elements will exist.
    //
    // Note that the existing elements, which are bitwise-moveable, may be
    // *moved* about the container without the possibility of throwing an
    // exception, but the newly inserted elements must be copy-constructed
    // (requiring memory allocation).
    //
    // First, move the valid elements from '[ start, divider )' to
    // '[ divider, finish )'.  This can be done without risk of a throw
    // occurring.
//
    std::memcpy(divider, start, (divider - start) * sizeof(TestType));
//
    bslalg::AutoArrayMoveDestructor<TestType> guard(start,
                                                    divider,
                                                    divider,
                                                    finish);
//
    while (guard.middle() < guard.end()) {
        // Call the copy c'tor, which may throw.
//
        new (guard.destination()) TestType(value, allocator);
//
        // 'guard.advance()' increments 'guard.destination()' and
        // 'guard.middle()' by one.
//
        guard.advance();
    }
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

    (void)veryVeryVerbose;      // suppress warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
    Z = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns: That the usage example compiles and runs as expected.
        //
        // Note: It was decided not to copy this part of the usage example to
        // the .h file, but it is useful here for testing that the usage
        // example works.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING USAGE EXAMPLE\n"
                            "=====================\n");

        // Next, within the 'main' function of our task, we create our 'value'
        // object, whose value with be 'v', to be inserted into the front of
        // our range.

        TestType value('v');

        // Then, we create a test allocator, and use it to allocate memory for
        // an array of 'TestType' objects:

        bslma::TestAllocator ta;

        TestType *array = (TestType *) ta.allocate(10 * sizeof(TestType));

        // Next, we construct the first 5 elements of the array to have the
        // values 'ABCDE'.

        TestType *p = array;
        new (p++) TestType('A', &ta);
        new (p++) TestType('B', &ta);
        new (p++) TestType('C', &ta);
        new (p++) TestType('D', &ta);
        new (p++) TestType('E', &ta);

        // Then, we record the number of outstanding blocks in the allocator:

        const bsls::Types::Int64 N = ta.numBlocksInUse();

        // Next, we enter an 'exception test' block, which will repetetively
        // enter a block of code, catching exceptions throw by the test
        // allocator 'ta' on each iteration:

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta)

            // Then, we observe that even if we've just caught an exception
            // and re-entered the block, the amount of memory outstanding is
            // unchanged from before we entered the block.

            ASSERT(ta.numBlocksInUse() == N);

            // Note that when we threw, some of the values of the 5 elements of
            // the array may have been changed to 'v', otherwise they will be
            // unchanged.

            // Next, we re-initiailize those elements that have been
            // overwritten in the last pass with 'value' to their values before
            // we entered the block:

            if ('v' == array[0].datum()) array[0].setDatum('A');
            if ('v' == array[1].datum()) array[1].setDatum('B');
            if ('v' == array[2].datum()) array[2].setDatum('C');
            if ('v' == array[3].datum()) array[3].setDatum('D');
            if ('v' == array[4].datum()) array[4].setDatum('E');

            // Then, we call 'insertItems', which may throw:

            insertItems(array, p, value, &ta);

            // Next, we exit the except testing block.

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        // Now, we verify:
        //: 1 Since each 'TestType' object allocates one block and
        //:   'insertItems' created 5 more 'TestType' objects, that we have
        //:   allocated exactly 5 more blocks of memory.
        //: 2 That the values of the elements of the array are as expected.

        ASSERT(ta.numBlocksInUse() == N + 5);

        ASSERT('v' == array[0].datum());
        ASSERT('v' == array[1].datum());
        ASSERT('v' == array[2].datum());
        ASSERT('v' == array[3].datum());
        ASSERT('v' == array[4].datum());
        ASSERT('A' == array[5].datum());
        ASSERT('B' == array[6].datum());
        ASSERT('C' == array[7].datum());
        ASSERT('D' == array[8].datum());
        ASSERT('E' == array[9].datum());

        // Finally, we destroy our array and check the allocator to verify no
        // memory was leaked:

        for (int i = 0; i < 10; ++i) {
            array[i].~TestType();
        }
        ta.deallocate(array);

        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'class AutoArrayDestructor'
        //
        // Concerns:  That the guard frees guarded memory properly upon
        //   exceptions.
        //
        // Plan: After asserting that the interface behaves as intended and
        //   that the guard destruction indeed frees the memory, we set up an
        //   exception test that ensures that the guard indeed correctly guards
        //   a varying portion of an array.
        //
        // Testing:
        //   AutoArrayMoveDestructor(T *d, T *b, T *m, T *e);
        //   ~AutoArrayMoveDestructor();
        //   T *advance();
        //   T *begin() const;
        //   T *middle() const;
        //   T *end() const;
        //   T *destination() const;
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING class bslalg::AutoArrayMoveDestructor"
                   "\n=============================================\n");

        const int GUARD_SIZE = 8;
        const int MAX_SIZE   = 2 * GUARD_SIZE;  // do not change

        static union {
            char                                d_raw[MAX_SIZE * sizeof(T)];
            bsls::AlignmentUtil::MaxAlignedType d_align;
        } u;
        T *buf = (T *) (void *) &u.d_raw[0];

        if (verbose)
            printf("\tSimple interface test.\n");
        {
            bslalg::AutoArrayMoveDestructor<T> mG(&buf[GUARD_SIZE],
                                                  &buf[0],
                                                  &buf[0],
                                                  &buf[GUARD_SIZE]);
            const bslalg::AutoArrayMoveDestructor<T>& G = mG;

            ASSERT(&buf[0]          == G.begin());
            ASSERT(&buf[0]          == G.middle());
            ASSERT(&buf[GUARD_SIZE] == G.end());
            ASSERT(&buf[GUARD_SIZE] == G.destination());

            for (int i = 0; i < GUARD_SIZE; ++i) {
                mG.advance();

                ASSERT(&buf[GUARD_SIZE + i + 1] == G.destination());
                ASSERT(&buf[0]                  == G.begin());
                ASSERT(&buf[i + 1]              == G.middle());
                ASSERT(&buf[GUARD_SIZE]         == G.end());
            }

            ASSERT(G.middle() == G.end());  // no destruction, no move
        }
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        if (verbose)
            printf("\tException test.\n");

        {
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            {
                bslalg::AutoArrayDestructor<T> mExcGuard(&buf[0], &buf[0]);

                char c = 'a';
                for (int i = 0; i < GUARD_SIZE; ++i, ++c) {
                    new (&buf[i]) T(c, Z);
                    mExcGuard.moveEnd(1);
                    if (veryVerbose) { buf[i].print(); }
                }

                {
                    bslalg::AutoArrayMoveDestructor<T> mG(&buf[GUARD_SIZE],
                                                          &buf[0],
                                                          &buf[0],
                                                          &buf[GUARD_SIZE]);
                    const bslalg::AutoArrayMoveDestructor<T>& G = mG;
                        // guards as follows (upon destruction): destroys first
                        // portion of first half, move second portion of first
                        // half beyond constructed elements in second half

                    mExcGuard.moveBegin(GUARD_SIZE);
                    mExcGuard.moveEnd(MAX_SIZE - GUARD_SIZE); // second half

                    ASSERT(&buf[GUARD_SIZE] == G.destination());
                    ASSERT(&buf[0]          == G.begin());
                    ASSERT(&buf[0]          == G.middle());
                    ASSERT(&buf[GUARD_SIZE] == G.end());

                    for (int i = 0; i < GUARD_SIZE; ++i, ++c) {
                        new (&buf[GUARD_SIZE + i]) T(c, Z);
                        mG.advance();

                        ASSERT(&buf[0]                  == G.begin());
                        ASSERT(&buf[i + 1]              == G.middle());
                        ASSERT(&buf[GUARD_SIZE]         == G.end());
                        if (veryVerbose) { buf[GUARD_SIZE + i].print(); }
                    }
                    ASSERT(G.middle() == G.end());
                }
                mExcGuard.moveBegin(-GUARD_SIZE);  // whole range
            }
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   This test exercises the component but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        const int GUARD_SIZE = 8;
        const int MAX_SIZE   = 2 * GUARD_SIZE;  // do not change

        static union {
            char                                d_raw[MAX_SIZE * sizeof(T)];
            bsls::AlignmentUtil::MaxAlignedType d_align;
        } u;
        T *buf = (T *) (void *) &u.d_raw[0];

        {
            bslalg::AutoArrayMoveDestructor<T> mG(&buf[GUARD_SIZE],
                                                  &buf[0],
                                                  &buf[1],
                                                  &buf[GUARD_SIZE]);
            const bslalg::AutoArrayMoveDestructor<T>& G = mG;

            ASSERT(&buf[GUARD_SIZE] == G.destination());
            ASSERT(&buf[0]          == G.begin());
            ASSERT(&buf[1]          == G.middle());
            ASSERT(&buf[GUARD_SIZE] == G.end());

            for (int i = 1; i < GUARD_SIZE; ++i) {
                mG.advance();

                ASSERT(&buf[GUARD_SIZE + i] == G.destination());
                ASSERT(&buf[0]              == G.begin());
                ASSERT(&buf[i + 1]          == G.middle());
                ASSERT(&buf[GUARD_SIZE]     == G.end());
            }

            ASSERT(G.middle() == G.end());  // no destruction, no move
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
