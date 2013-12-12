// bslma_managedptr_factorydeleter.t.cpp                              -*-C++-*-
#include <bslma_managedptr_factorydeleter.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//                             ---------
// [ 3] void deleter(obj, factory)
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] Test machinery

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
int testStatus = 0;

namespace {

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }

}

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

//=============================================================================
//                         HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class MyTestObject {
    // This test-class serves three purposes.  It provides a base class for the
    // test classes in this test driver, so that derived -> base conversions
    // can be tested.  It also signals when its destructor is run by
    // incrementing an externally managed counter, supplied when each object
    // is created.  Finally, it exposes an internal data structure that can be
    // use to demonstrate the 'bslma::ManagedPtr' aliasing facility.

    // DATA
    volatile int *d_deleteCounter_p;
    mutable int   d_value[2];

  public:
    // CREATORS
    explicit MyTestObject(int *counter);

    // Use compiler-generated copy constructor and assignment operator
    // MyTestObject(MyTestObject const& orig);
    // MyTestObject operator=(MyTestObject const& orig);

    virtual ~MyTestObject();
        // Destroy this object.

    // ACCESSORS
    int *valuePtr(int index = 0) const;

    volatile int *deleteCounter() const;
};

MyTestObject::MyTestObject(int *counter)
: d_deleteCounter_p(counter)
, d_value()
{
}

MyTestObject::~MyTestObject()
{
    ++(*d_deleteCounter_p);
}

inline
int *MyTestObject::valuePtr(int index) const
{
    BSLS_ASSERT_SAFE(2 > index);

    return d_value + index;
}

volatile int* MyTestObject::deleteCounter() const
{
    return d_deleteCounter_p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class CountedStackDeleter
{
    // DATA
    volatile int *d_deleteCounter_p;

  private:
    // NOT IMPLEMENTED
    CountedStackDeleter(const CountedStackDeleter&); //=delete;
    CountedStackDeleter& operator=(const CountedStackDeleter&); //=delete;

  public:
    // CREATORS
    explicit CountedStackDeleter(int *counter) : d_deleteCounter_p(counter) {}

    //! ~CountedStackDeleter();
        // Destroy this object.

    // ACCESSORS
    volatile int *deleteCounter() const { return d_deleteCounter_p; }

    void deleteObject(void *) const
    {
        ++*d_deleteCounter_p;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}  // close unnamed namespace

//=============================================================================
//                  TEST PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&da);

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // TESTING bslma::ManagedPtr_FactoryDeleter
        //
        // Concerns:
        //: 1 'bslma::ManagedPtr_FactoryDeleter<T,U>::deleter(obj, factory)'
        //:   calls the 'deleteObject' method through the passed pointer to a
        //:   'factory' of type 'U', with the argument 'obj' which is cast to a
        //:   pointer to type 'T'.
        //:
        //: 2 The 'deleter' method can be used as a deleter policy by
        //:   'bslma::ManagedPtr'.
        //:
        //: 3 The 'deleter' method asserts in safe builds if passed a null
        //:   pointer for either argument.
        //:
        //
        // Plan:
        //: 1 blah ...
        //
        // Testing:
        //    ManagedPtr_FactoryDeleter<T,U>::deleter(obj, factory)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING bslma::ManagedPtr_FactoryDeleter"
                            "\n----------------------------------------\n");

        typedef bslma::ManagedPtr_FactoryDeleter<MyTestObject,
                                             CountedStackDeleter > TestFactory;

        if (verbose) printf("\tConfirm the deleter does not destroy the "
                             "passsed object\n");

        {
            int deleteCount = 0;
            MyTestObject t(&deleteCount);

            int parallelCount = 0;
            CountedStackDeleter factory(&parallelCount);

            TestFactory::deleter(&t, &factory);
            LOOP_ASSERT(deleteCount, 0 == deleteCount);
            LOOP_ASSERT(parallelCount, 1 == parallelCount);
        }

#if 0
        if (verbose) printf("\tConfirm the deleter can be registered with "
                             "a managed pointer\n");

        {
            bslma::TestAllocatorMonitor gam(&globalAllocator);
            bslma::TestAllocatorMonitor dam(&da);

            int deleteCount = 0;
            CountedStackDeleter factory(&deleteCount);

            int dummy = 0;
            MyTestObject x(&dummy);
            MyTestObject y(&dummy);

            bslma::ManagedPtr<MyTestObject> p(&x, &factory,
                                             &doNothingDeleter);

            p.load(&y, &factory, &TestFactory::deleter);

            ASSERT(dam.isInUseSame());
            ASSERT(gam.isInUseSame());
        }
#endif

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\tNegative testing\n");

        {
            bsls::AssertTestHandlerGuard guard;

            int deleteCount = 0;
            MyTestObject t(&deleteCount);

            int parallelCount = 0;
            CountedStackDeleter factory(&parallelCount);

            ASSERT_SAFE_FAIL(TestFactory::deleter(0, &factory));
            ASSERT_SAFE_FAIL(TestFactory::deleter(&t, 0));
            ASSERT_SAFE_FAIL(TestFactory::deleter(0, 0));
            ASSERT_SAFE_PASS(TestFactory::deleter(&t, &factory));
        }
#else
        if (verbose) printf("\tNegative testing disabled due to lack of "
                             "exception support\n");
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TEST MACHINERY
        //
        // Concerns:
        //: 1 'MyTestObject' objects do not allocate any memory from the
        //:   default allocator nor from the global allocator for any of their
        //:   operations.
        //:
        //: 2 'MyTestObject' objects, created with a pointer to an integer,
        //:   increment the referenced integer exactly once when they are
        //:   destroyed.
        //:
        //: 3 'MyTestObject' objects, created by copying another 'MyTestObject'
        //:   object, increment the integer referenced by the original object
        //:   exactly once when destroyed.
        //
        // Plan:
        //: 1 Install test allocator monitors to verify that neither the global
        //:   nor default allocators allocate any memory executing this test
        //:   case.
        //:
        //: 2 For each test-class type:
        //:   1 Initialize an 'int' counter to zero
        //:   2 Create a object of tested type, having the address of the 'int'
        //:     counter.
        //:   3 Confirm the test object 'deleterCounter' points to the 'int'
        //:     counter.
        //:   4 Confirm the 'int' counter value has not changed.
        //:   5 Destroy the test object and confirm the 'int' counter value
        //:     has incremented by exactly 1.
        //:   6 Create a second object of tested type, having the address of
        //:     the 'int' counter.
        //:   7 Create a copy of the second test object, and confirm both test
        //:     object's 'deleterCount' point to the same 'int' counter.
        //:   8 Confirm the 'int' counter value has not changed.
        //:   9 Destroy one test object, and confirm test 'int' counter is
        //:     incremented exactly once.
        //:  10 Destroy the other test object, and confirm test 'int' counter
        //:     is incremented exactly once.
        //:
        //: 3 Verify that no unexpected memory was allocated by inspecting the
        //:   allocator guards.
        //
        // Testing:
        //    class MyTestObject
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TEST MACHINERY"
                            "\n----------------------\n");

        if (verbose) printf("\tTest class MyTestObject\n");

        bslma::TestAllocatorMonitor gam(&globalAllocator);
        bslma::TestAllocatorMonitor dam(&da);

        int destructorCount = 0;
        {
            MyTestObject mt(&destructorCount);
            ASSERT(&destructorCount == mt.deleteCounter());
            LOOP_ASSERT(destructorCount, 0 == destructorCount);
        }
        LOOP_ASSERT(destructorCount, 1 == destructorCount);

        destructorCount = 0;
        {
            MyTestObject mt1(&destructorCount);
            {
                MyTestObject mt2 = mt1;
                ASSERT(&destructorCount == mt1.deleteCounter());
                ASSERT(&destructorCount == mt2.deleteCounter());
                LOOP_ASSERT(destructorCount, 0 == destructorCount);
            }
            LOOP_ASSERT(destructorCount, 1 == destructorCount);
        }
        ASSERT(2 == destructorCount);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1. That the functions exist with the documented signatures.
        //   2. That the basic functionality works as documented.
        //
        // Plan:
        //   Exercise each function in turn and devise an elementary test
        //   sequence to ensure that the basic functionality is as documented.
        //
        // Testing:
        //   This test exercises basic functionality but *tests* *nothing*.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n--------------\n");

        printf("Nothing tested yet.\n");
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
