// bslma_mallocfreeallocator.t.cpp                                    -*-C++-*-

#include <bslma_mallocfreeallocator.h>

#include <bslma_allocator.h>       // for testing only

#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

#ifdef BDE_BUILD_TARGET_EXC
#include <new>
#endif

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing an allocator that uses 'std::malloc' and 'std::free' instead
// of global 'new' and 'delete'.  Since there is no way of intercepting the
// 'std::malloc' and 'std::free' methods, we indirectly check for proper source
// of memory allocation by overriding global operator 'new' and 'delete', and
// check whether they are invoked.  If no calls to global 'new' and 'delete'
// are made, we assume 'std::malloc' and 'std::free' are used.
//-----------------------------------------------------------------------------
// [ 1] bslma::MallocFreeAllocator()
// [ 1] ~bslma::MallocFreeAllocator()
// [ 1] void *allocate(size_type size)
// [ 1] void deallocate((void *address)
// [ 2] void *allocate(size_type size)   // allocate 0
// [ 2] void deallocate((void *address)  // deallocate 0
// [ 3] static bslma::MallocFreeAllocator& singleton()
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE

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
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)e

//=============================================================================
//                          GLOBALS FOR TESTING
//-----------------------------------------------------------------------------

static    bool globalVeryVeryVerbose            = false;

static    int  globalNewCalledCount             = 0;
static    int  globalNewCalledCountIsEnabled    = 0;
static size_t  globalNewCalledLastArg           = 0;

static    int  globalDeleteCalledCount          = 0;
static    int  globalDeleteCalledCountIsEnabled = 0;
static   void *globalDeleteCalledLastArg        = 0;

//=============================================================================
//                 REDEFINED GLOBAL OPERATOR NEW AND DELETE
//-----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
void *operator new(size_t size) throw(std::bad_alloc)
#else
void *operator new(size_t size)
#endif
    // Trace use of global operator new.  Note that we must use printf
    // to avoid recursion.
{
    void *addr = malloc(size);

    if (globalNewCalledCountIsEnabled) {
        ++globalNewCalledCount;
        globalNewCalledLastArg = size;
    }

    if (globalVeryVeryVerbose) {
        printf("global new called -");
        if (!globalNewCalledCountIsEnabled) {
            printf(" not");
        }
        printf(" enabled - count = %d: %p\n", globalNewCalledCount, addr);
    }

    return addr;
}

#ifdef BDE_BUILD_TARGET_EXC
void operator delete(void *address) throw()
#else
void operator delete(void *address)
#endif
    // Trace use of global operator delete.
{
    if (globalDeleteCalledCountIsEnabled) {
        ++globalDeleteCalledCount;
        globalDeleteCalledLastArg = address;
    }

    if (globalVeryVeryVerbose) {
        printf("global delete called -");
        if (!globalDeleteCalledCountIsEnabled) {
            printf(" not");
        }
        printf(" enabled - count = %d: freeing %p\n",
               globalDeleteCalledCount, address);
    }

    free(address);
}

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------
// This component is intended to be used when the use of 'new' and 'delete' are
// not desirable, such as the case of 'bslma::TestAllocator'.  Instead of using
// 'bslma::Default' which uses the 'bslma::NewDeleteAllocator', this component
// can be used to bypass the use of 'new' and 'delete'.
//
// The following example demonstrates the use of this component for a user
// defined allocator instead of using the default allocator:
//..
//  // my_allocator.h
//  // ...
//
    class my_Allocator : public bslma::Allocator {
//      // This class provides a mechanism for allocation and deallocation.
//
//      // DATA
        bslma::Allocator *d_allocator_p;  // allocator (held, not owned)
//
      public:
//      // CREATORS
        my_Allocator(bslma::Allocator *basicAllocator = 0);
//          // Create a 'my_Allcoator'.  Optionally specify 'basicAllocator' to
//          // supply memory.  If 'basicAllocator' is 0, the
//          // 'bslma::MallocFreeAllocator' will be used.
//
        ~my_Allocator();
//          // Destroy this allocator.  Note that the behavior of destroying an
//          // allocator while memory is allocated from it is not specified.
//          // (Unless you *know* that it is valid to do so, don't!)
//
//      // MANIPULATORS
        void *allocate(size_type size);
//          // Return a newly allocated block of memory of (at least) the
//          // specified positive 'size' (bytes).  If 'size' is 0, a null
//          // pointer is returned with no effect.  Note that the alignment of
//          // the address returned is the maximum alignment for any
//          // fundamental type defined for this platform.
//
        void deallocate(void *address);
//          // Return the memory at the specified 'address' back to this
//          // allocator.  If 'address' is 0, this function has no effect.  The
//          // behavior is undefined if 'address' was not allocated using this
//          // allocator, or has already been deallocated.
    };
//..
// The constructor is implemented using 'bslma::MallocFreeAllocator'.
//..
//  // my_allocator.cpp
//  // ...
//
//  // CREATORS
    my_Allocator::my_Allocator(bslma::Allocator *basicAllocator)
    : d_allocator_p(basicAllocator
                    ? basicAllocator
                    : &bslma::MallocFreeAllocator::singleton())
    {
    }
//
//  // ...
//..
// When the 'basicAllocator' is not specified, the 'bslma::MallocFreeAllocator'
// will be used - which then calls 'std::malloc' and 'std::free' for allocating
// and deallocating memory.

//=============================================================================
//           Additional Functionality Needed to Complete Usage Test Case
//-----------------------------------------------------------------------------

my_Allocator::~my_Allocator()
{
}

// MANIPULATORS
void *my_Allocator::allocate(size_type size)
{
    return d_allocator_p->allocate(size);
}

void my_Allocator::deallocate(void *address)
{
    d_allocator_p->deallocate(address);
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    globalVeryVeryVerbose = veryVeryVerbose;

    (void)veryVerbose;           // suppress unused variable warning
    (void)veryVeryVeryVerbose;   // suppress unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);


    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Allocate memory using the default constructed 'my_Allocator', then
        //   ensure that global 'new' and 'delete' are not invoked.  Also
        //   construct 'my_Allocator' using a 'bslma::newDeleteAllocator', then
        //   ensure that global 'new' and 'delete' are invoked.
        //
        // Testing:
        //   Usage example
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        if (verbose) printf("\nCreating my_Allocator\n");

        my_Allocator alloc;

        if (verbose) printf("\nTesting allocate\n");

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalNewCalledLastArg);
        globalNewCalledCountIsEnabled = 1;
        void *addr1 = alloc.allocate(5);
        globalNewCalledCountIsEnabled = 0;

        ASSERT(0 != addr1);
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalNewCalledLastArg);

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalNewCalledLastArg);
        globalNewCalledCountIsEnabled = 1;
        void *addr2 = alloc.allocate(10);
        globalNewCalledCountIsEnabled = 0;

        ASSERT(0 != addr1);
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalNewCalledLastArg);

        if (verbose) printf("\nTesting deallocate\n");

        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);

        globalDeleteCalledCountIsEnabled = 1;
        alloc.deallocate(addr1);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);

        globalDeleteCalledCountIsEnabled = 1;
        alloc.deallocate(addr2);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // SINGLETON TEST
        //
        // Concerns:
        //   1. That the 'singleton' method returns a valid
        //      'bslma::MallocFreeAllocator' that can be used to allocate and
        //      deallocate memory using 'malloc' and 'free'.
        //
        //   2. Multiple invocations of 'singleton' return the same instance
        //      of 'bslma::MallocFreeAllocator'.
        //
        // Plan:
        //   Perform the same breathing test on the
        //   'bslma::MallocFreeAllocator' returned by the 'singleton' method.
        //   Also store the address of the instance returned by the
        //   'singleton' method.
        //
        // Testing:
        //   static bslma::MallocFreeAllocator& singleton()
        // --------------------------------------------------------------------

        if (verbose) printf("\nSINGLETON TEST"
                            "\n==============\n");

        if (verbose) printf("\nTesting 'singleton'\n");

        bslma::MallocFreeAllocator *alloc =
                                      &bslma::MallocFreeAllocator::singleton();
        bslma::MallocFreeAllocator *alloc2 =
                                      &bslma::MallocFreeAllocator::singleton();

        ASSERT(alloc == alloc2);

        if (veryVerbose) printf("\tTesting allocate\n");

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalNewCalledLastArg);
        globalNewCalledCountIsEnabled = 1;
        void *addr1 = alloc->allocate(5);
        globalNewCalledCountIsEnabled = 0;

        ASSERT(0 != addr1);
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalNewCalledLastArg);

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalNewCalledLastArg);
        globalNewCalledCountIsEnabled = 1;
        void *addr2 = alloc->allocate(10);
        globalNewCalledCountIsEnabled = 0;

        ASSERT(0 != addr1);
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalNewCalledLastArg);

        if (veryVerbose) printf("\tTesting deallocate\n");

        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);

        globalDeleteCalledCountIsEnabled = 1;
        alloc->deallocate(addr1);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);

        globalDeleteCalledCountIsEnabled = 1;
        alloc->deallocate(addr2);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // ALLOCATE / DEALLOCATE NULL TEST
        //
        // Concerns:
        //   That invoking 'allocate' with 0 size and 'deallocate' on 0 address
        //   succeeds.
        //
        // Plan:
        //   Create a 'bslma::MallocFreeAllocator' on the stack.  Then invoke
        //   'allocate' with 0 size and 'deallocate' on 0 address.
        //
        // Testing:
        //   void *allocate(size_type size)   // allocate 0
        //   void deallocate((void *address)  // deallocate 0
        // --------------------------------------------------------------------

        if (verbose) printf("\nALLOCATE / DEALLCOATE NULL TEST"
                            "\n===============================\n");

        if (verbose) printf("\nCreating allocator\n");

        bslma::MallocFreeAllocator alloc;

        if (verbose) printf("\nTesting allocate 0\n");

        void *address = alloc.allocate(0);
        ASSERT(0 == address);

        if (verbose) printf("\nTesting deallocate 0\n");

        alloc.deallocate(address);
        ASSERT(0 == address);

      } break;
      case 1: {
        // -----------------------------------------------------------------
        // BREATHING TEST:
        //
        // Concerns:
        //   That 'new' and 'delete' are not invoked by the
        //   'bslma::MallocFreeAllocator'.
        //
        // Plan:
        //   Create a malloc-free allocator on the program stack and verify
        //   that the global 'new' and 'delete' are not called per method
        //   invocation, and with the appropriate arguments.
        //
        // Testing:
        //    bslma::MallocFreeAllocator();
        //    ~bslma::MallocFreeAllocator();
        //    void *allocate(int size);
        //    void deallocate(void *address);
        // -----------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) printf("\nCreating allocator\n");

        bslma::MallocFreeAllocator alloc;

        if (verbose) printf("\nTesting allocate\n");

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalNewCalledLastArg);
        globalNewCalledCountIsEnabled = 1;
        void *addr1 = alloc.allocate(5);
        globalNewCalledCountIsEnabled = 0;

        ASSERT(0 != addr1);
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalNewCalledLastArg);

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalNewCalledLastArg);
        globalNewCalledCountIsEnabled = 1;
        void *addr2 = alloc.allocate(10);
        globalNewCalledCountIsEnabled = 0;

        ASSERT(0 != addr1);
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalNewCalledLastArg);

        if (verbose) printf("\nTesting deallocate\n");

        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);

        globalDeleteCalledCountIsEnabled = 1;
        alloc.deallocate(addr1);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);

        globalDeleteCalledCountIsEnabled = 1;
        alloc.deallocate(addr2);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);

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
