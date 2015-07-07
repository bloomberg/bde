// bslma_defaultallocatorguard.t.cpp                                  -*-C++-*-

#include <bslma_defaultallocatorguard.h>

#include <bslma_allocator.h>               // for testing only
#include <bslma_default.h>                 // for testing only
#include <bslma_newdeleteallocator.h>      // for testing only
#include <bslma_testallocator.h>           // for testing only

#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test "guards" the process-wide default allocator, which
// is to say that an instance of this object copies the default allocator to
// a data member and installs a new default (from the constructor argument) on
// construction, and restores the original default allocator on destruction.
//
// The concerns and testing in a single-threaded environment are
// straightforward.
//-----------------------------------------------------------------------------
// CREATORS
// [ 1] bslma::DefaultAllocatorGuard(bslma::Allocator *temporary);
// [ 1] ~bslma::DefaultAllocatorGuard();
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE
//=============================================================================

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef bslma::DefaultAllocatorGuard Obj;

bool globalVerbose = false;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

class my_CountingAllocator : public bslma::Allocator
{
    int d_blocksOutstanding;
  public:
    my_CountingAllocator();
    ~my_CountingAllocator();

    virtual void *allocate(size_type size);
    virtual void deallocate(void *address);

    int blocksOutstanding() const { return d_blocksOutstanding; }
};

inline
my_CountingAllocator::my_CountingAllocator()
: d_blocksOutstanding(0)
{
}

inline
my_CountingAllocator::~my_CountingAllocator()
{
    if (0 < d_blocksOutstanding && globalVerbose) {
        printf("***ERROR: Memory Leak***\n"
               "%d block(s) leaked.\n"
               "Program aborting.\n", d_blocksOutstanding);
        ASSERT(0 || globalVerbose);
    }
}

void *my_CountingAllocator::allocate(size_type size)
{
    ++d_blocksOutstanding;
    return operator new(size);
}

void my_CountingAllocator::deallocate(void *address)
{
    --d_blocksOutstanding;
    operator delete(address);
}

class my_Container
{
    bslma::Allocator *d_allocator_p;
    int              *d_intArray;
  public:
    my_Container(bslma::Allocator* alloc = 0);

    // Containers don't typically have a function to return their
    // allocators, but this is useful for exposition:
    bslma::Allocator *allocator() const { return d_allocator_p; }
};

// Constructor
my_Container::my_Container(bslma::Allocator* alloc)
: d_allocator_p(bslma::Default::allocator(alloc))
{
    d_intArray = (int*) d_allocator_p->allocate(10 * sizeof(int));
}

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

    (void)veryVerbose;           // suppress unused variable warning
    (void)veryVeryVerbose;       // suppress unused variable warning

    globalVerbose = verbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove
        //   leading comment characters, and replace 'assert' with
        //   'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        {
            bslma::NewDeleteAllocator *na
                = &bslma::NewDeleteAllocator::singleton();
            ASSERT(na == bslma::Default::defaultAllocator());

            {
                my_CountingAllocator testAllocator;
                Obj guard(&testAllocator);
                ASSERT(&testAllocator == bslma::Default::defaultAllocator());

                // Create and test the object under test, which will use the
                // test allocator *by* *default*.

                // . . .

                void *arena = bslma::Default::defaultAllocator()->allocate(10);
                ASSERT(arena);
                // Oops -- this will leak.

                ASSERT(0 != testAllocator.blocksOutstanding());
            }
            ASSERT(na == bslma::Default::defaultAllocator());
        }

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1. The default allocator is bslma::NewDeleteAllocator
        //   2. The allocator function returns default allocator if passed-in
        //      allocator is null.
        //   3. The allocator function.
        //
        // Plan:
        //
        // Testing:
        //    bslma::DefaultAllocatorGuard(bslma::Allocator *temporary);
        //    ~bslma::DefaultAllocatorGuard();
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        {
            my_CountingAllocator defaultAllocator;

            bslma::Default::setDefaultAllocator(&defaultAllocator);
            ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

            {
                bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
                Obj guard(&testAllocator);
                ASSERT(&testAllocator == bslma::Default::defaultAllocator());
            }
            ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());
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
