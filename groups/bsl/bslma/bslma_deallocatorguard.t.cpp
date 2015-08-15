// bslma_deallocatorguard.t.cpp                                       -*-C++-*-

#include <bslma_deallocatorguard.h>

#include <bslma_allocator.h>       // for testing only
#include <bslma_testallocator.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a guard object to ensure that, when the guard object goes
// out of scope, it deallocates the correct memory address with the allocator
// it holds.  We use two allocators in this module: 'bslma::TestAllocator', and
// a local 'TestAllocator', used to verify that the guard object can work with
// an ALLOCATOR type that is not derived from 'bslma::Allocator'.  The
// local 'TestAllocator' allocator has a 'deallocate' method which is
// instrumented to record the most recent memory address used to invoke the
// method.  We initialize the 'bslma::DeallocatorGuard' guard object with this
// allocator and verify that when the guard object is destroyed the expected
// memory address is recorded in the allocator.
//-----------------------------------------------------------------------------
// [2] bslma::DeallocatorGuard<ALLOCATOR>();    // with TestAllocator
// [2] ~bslma::DeallocatorGuard<ALLOCATOR>();   // with TestAllocator
// [3] bslma::DeallocatorGuard<ALLOCATOR>();    // with bslma::TestAllocator
// [3] ~bslma::DeallocatorGuard<ALLOCATOR>();   // with bslma::TestAllocator
//-----------------------------------------------------------------------------
// [1] Ensure local helper class TestAllocator works as expected.
// [4] usage example

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
//                          HELPER CLASS FOR TESTING
//-----------------------------------------------------------------------------

namespace BloombergLP {

class TestAllocator {
    int   d_isDeallocateCalled;    // set if 'deallocate' has bee called
    void *d_lastDeallocateAddress; // last memory address deallocated

  public:
    // CREATORS
    TestAllocator() : d_isDeallocateCalled(0), d_lastDeallocateAddress(0) {}
        // Create this object.

    ~TestAllocator() {}
        // Destroy this object.

    // MANIPULATORS
    void deallocate(void *address);
        // Record the specified 'address' and set an internal flag to indicate
        // this method has been called.

    // ACCESSORS
    void *lastDeallocateAddress() const { return d_lastDeallocateAddress; }
        // Return the last memory address that 'deallocate' was invoked with.
        // Return 'null' if 'deallocate' has never been called on this object.

    bool isDeallocateCalled() const { return d_isDeallocateCalled; }
        // Return 'true' if 'deallocate' has been called on this object, and
        // 'false' otherwise.
};

void TestAllocator::deallocate(void *address)
{
    d_lastDeallocateAddress = address;
    d_isDeallocateCalled = 1;
}

}  // close enterprise namespace

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------
// A 'bslma::DeallocatorGuard' can be used to ensure that a dynamically
// allocated raw memory resource is safely deallocated in the presense of
// multiple return satements or exceptions in an exception-neutral way (i.e.,
// without the need for 'try'/'catch' blocks).  In this simple example,
// consider the function 'evaluatePassword' which attempts to determine how
// secure a given password might be:
//..
    double evaluatePassword(const char *password, bslma::Allocator *allocator);
        // Evaluate the strength of the specified 'password', using the
        // specified 'allocator' to supply memory for evaluation.  Return a
        // real value in the rage [0.0 .. 1.0] where 0.0 indicates the weakest
        // password, and 1.0 the strongest.
//..
// This function will be implemented in terms of three *exception* *neutral*
// subroutines, each of which operates on a writable copy of the
// null-terminated password, (perturbing its contents slightly) and requiring
// unbounded amounts of scratch memory (to be allocated and deallocated from a
// supplied allocator):
//..
    int subroutine1(char *inOut, bslma::Allocator *allocator);
    int subroutine2(char *inOut, bslma::Allocator *allocator);
    int subroutine3(char *inOut, bslma::Allocator *allocator);
//..
// A final subroutine is then used to determine and return the score:
//..
    double finalSubroutine(const char *result);
//..
// The top-level routine is implemented as follows:
//..
    double evaluatePassword(const char *password, bslma::Allocator *allocator)
    {

        // Set up local writable copy of password in buffer.

        size_t size = strlen(password) + 1;
        char *buffer = (char *)allocator->allocate(size);
        memcpy(buffer, password, size);

        //**************************************************************
        //* Note the use of the deallocator guard on 'buffer' (below). *
        //**************************************************************

        bslma::DeallocatorGuard<bslma::Allocator> guard(buffer, allocator);

        // Process and evaluate the supplied password.

        if (0 != subroutine1(buffer, allocator)) {
            return 0.0;                                               // RETURN
        }
        if (0 != subroutine2(buffer, allocator)) {
            return 0.2;                                               // RETURN
        }
        if (0 != subroutine3(buffer, allocator)) {
            return 0.4;                                               // RETURN
        }

        return finalSubroutine(buffer);

    }  // note that 'buffer' is deallocated at end of block regardless
//..
// Notice that if any of the initial (numbered) subroutines returns a non-zero
// status value, the top-level 'evaluatePassword' routine returns immediately
// with a predetermined score.  Moreover, each of these routines
// may encounter a 'bad_alloc' exception should the supplied allocator fail to
// return the requested memory.  Even if all of these subroutines evaluates
// successfully, the score calculated using 'finalEval' is returned directly by
// 'evaluatePassword', yet we still need to deallocate 'buffer'.  By guarding
// buffer with a 'bslma::DeallocatorGuard' as shown above, all of these issues
// are fully addressed, and the top-level routine is also *exception* *neutral*
// as desired.

//=============================================================================
//           Additional Functionality Needed to Complete Usage Test Case
//-----------------------------------------------------------------------------
int subroutine1(char * /* inOut */, bslma::Allocator * /*allocator */)
{
    return 0;
}

int subroutine2(char * /* inOut */, bslma::Allocator * /*allocator */)
{
    return 0;
}

int subroutine3(char * /* inOut */, bslma::Allocator * /* allocator */) {
    return 0;
}

double finalSubroutine(const char * /* result */) {
    return 1.0;
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

    (void)veryVeryVerbose;       // suppress unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Call the routine 'evaluatePassword'.
        //
        // Testing:
        //   usageExample();
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        const char *password = "hello";

        double result = evaluatePassword(password, &ta);
        (void) result;

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING WITH bslma::TestAllocator
        //
        // Concerns:
        //   That the 'bslma::DeallocatorGuard' works correctly with a
        //   'bslma::Allocator'.
        //
        // Plan:
        //   Use the built-in capabilities of 'bslma::TestAllocator' to verify
        //   that 'bslma::DeallocatorGuard' deallocates memory when it is
        //   supposed to.
        //
        // Testing:
        //   bslma::DeallocatorGuard<ALLOCATOR>(memory, originalAllocator);
        //   ~bslma::DeallocatorGuard<ALLOCATOR>();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST WITH bslma::TestAllocator"
                            "\n==============================\n");

        bslma::TestAllocator alloc;
        for (int i = 1; i <= 100; ++i) {
            {
                void *p = alloc.allocate(i);

                bslma::DeallocatorGuard<bslma::Allocator> guard(p, &alloc);

                ASSERT(alloc.numBytesInUse() == i);
            }

            ASSERT(alloc.numBytesInUse() == 0);
        }

        ASSERT(alloc.numBytesInUse() == 0);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CTOR / DTOR TEST
        //
        // Concerns:
        //   That the guard calls the allocator's 'deallocate' method upon
        //   destruction, and that the guard can work with an allocator that
        //   provides only the minimal interface this component's contract
        //   requires.
        //
        // Plan:
        //   Create 'bslma::DeallocatorGuard' guard objects holding
        //   'TestAllocator' objects and varying memory addresses.  Verify that
        //   when the guard object goes out of scope the allocator object
        //   contains the expected memory address.
        //
        // Testing:
        //   bslma::DeallocatorGuard<ALLOCATOR>(memory, originalAllocator);
        //   ~bslma::DeallocatorGuard<ALLOCATOR>();
        // --------------------------------------------------------------------

        if (verbose) printf("\nCTOR / DTOR TEST"
                            "\n================\n");

        const void *DATA[] = {(void *) 1, (void *) 2, (void *) 3 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) printf("Testing constructor.\n");
        int di;
        for (di = 0; di < NUM_DATA; ++di) {
            TestAllocator a;
            const TestAllocator &A = a;
            void *addr = const_cast<void *>(DATA[di]);
            const void *ADDR = addr;
            {
                const bslma::DeallocatorGuard<TestAllocator> X(addr, &a);
            }
            if (veryVerbose) { T_ P(ADDR); }
            LOOP_ASSERT(di, ADDR == A.lastDeallocateAddress());
            LOOP_ASSERT(di, A.isDeallocateCalled());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // HELPER CLASS TEST
        //
        // Concerns:
        //   That the local 'TestAllocator' class correctly recoders the
        //   memory addresses it deallocates, and correctly records whether
        //   'deallocate' has been called.
        //
        // Plan:
        //   Create a 'TestAllocator' object and call its 'deallocate' method
        //   with varying memory address.  Verify that 'lastDeallocateAddress'
        //   returns the expected memory addresses.  Also verify that
        //   'isDeallocateCalled' indicates the proper state of the
        //   'TestAllocator' object before and after 'deallocate' is called.
        //
        // Testing:
        //   TestAllocator::TestAllocator();
        //   TestAllocator::~TestAllocator();
        //   void TestAllocator::deallocate(address);
        //   int TestAllocator::isDeallocateCalled();
        //   void *TestAllocator::lastDeallocateAddress();
        // --------------------------------------------------------------------

        if (verbose) printf("\nHELPER CLASS TEST"
                            "\n=================\n");

        if (verbose) printf("Testing 'TestAllocator'.\n");

        const void *DATA[] = {(void *) 0, (void *) 1, (void *) 2, (void *) 3 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        TestAllocator mX;    const TestAllocator &X = mX;
        ASSERT(0 == X.lastDeallocateAddress());
        ASSERT(!X.isDeallocateCalled());
        for (int di = 0; di < NUM_DATA; ++di) {
            void *addr = const_cast<void *>(DATA[di]);
            const void *ADDR = addr;
            mX.deallocate(addr);
            if (veryVerbose) { T_ P(ADDR); }
            LOOP_ASSERT(di, ADDR == X.lastDeallocateAddress());
            LOOP_ASSERT(di, X.isDeallocateCalled());
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
