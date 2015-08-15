// bslma_destructorguard.t.cpp                                        -*-C++-*-

#include <bslma_destructorguard.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_bsltestutil.h>
#include <bsls_objectbuffer.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

#include <vector>       // std::vector for the usage example

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a proctor object to ensure that it destroys the proper
// sequence of contiguous objects (in unspecified order).  We achieve this goal
// by creating objects of a user-defined type that are each initialized with
// the address of a unique counter.  As each object is destroyed, its
// destructor increments the counter held by the object, indicating the number
// of times the object's destructor is called.  We create proctors to manage
// varying sequences (differing in origin and length) of such user-defined-type
// objects in an array.  After each proctor is destroyed, we verify that the
// corresponding counters of the objects managed by the proctor are modified.
//-----------------------------------------------------------------------------
// [2] bslma::DestructorGuard<TYPE>(&object)
// [2] ~bslma::DestructorGuard<TYPE>();
//-----------------------------------------------------------------------------
// [1] my_Class(&counter);
// [1] ~my_Class();
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
//                          HELPER CLASS FOR TESTING
//-----------------------------------------------------------------------------

class my_Class {
    // This object indicates that its destructor is called by incrementing the
    // counter it *holds* (provided at construction) in the destructor.
  private:
    int *d_counter_p; // Counter to be incremented at destruction

  public:
    // CREATORS
    my_Class(int *counter) : d_counter_p(counter) {}
        // Create this object and optionally specify the address of the
        // 'counter' to be held.

    ~my_Class() { ++*d_counter_p; }
        // Destroy this object.  Also increment this object's counter if it is
        // not 'null'.
};

//=============================================================================
//                  GLOBAL TYPEDEFS, CONSTANTS AND VARIABLES
//-----------------------------------------------------------------------------

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

// Workaround for optimization issue in xlC that mishandles pointer aliasing.
//   IV56864: ALIASING BEHAVIOUR FOR PLACEMENT NEW
//   http://www-01.ibm.com/support/docview.wss?uid=swg1IV56864
// Place this macro following each use of placment new.  Alternatively,
// compile with xlC_r -qalias=noansi, which reduces optimization opportunities
// across entire translation unit instead of simply across optimization fence.
// Update: issue is fixed in xlC 13.1 (__xlC__ >= 0x0d01).

#if defined(BSLS_PLATFORM_CMP_IBM) && BSLS_PLATFORM_CMP_VERSION < 0x0d01
    #define BSLMA_DESTRUCTORGUARD_XLC_PLACEMENT_NEW_FIX                       \
                             BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE
#else
    #define BSLMA_DESTRUCTORGUARD_XLC_PLACEMENT_NEW_FIX
#endif


// Suppose we have a situation where one of the two constructors will be
// called to create an object on the stack for performance reasons.  The
// construction thus occurs within either of the branches of an 'if'
// statement, so the object itself, to survive the end of the "then" or "else"
// block, must be constructed in a 'bsls::ObjectBuffer'.  Once constructed, the
// object would not be destroyed automatically, so to make sure it will be
// destroyed, we place it under the management of a 'bslma::DestructorGuard'.
// After that, we know that however the routine exits -- either by a return
// or as a result of an exception being thrown -- the object will be destroyed.

double usageExample(double startValue)
{
    bsls::ObjectBuffer<std::vector<double> > buffer;
    std::vector<double>& myVec = buffer.object();

    if (startValue >= 0) {
        new (&myVec) std::vector<double>(100, startValue);
    }
    else {
        new (&myVec) std::vector<double>();
    }
    BSLMA_DESTRUCTORGUARD_XLC_PLACEMENT_NEW_FIX;

    //***********************************************************
    // Note the use of the destructor guard on 'myVec' (below). *
    //***********************************************************

    bslma::DestructorGuard<std::vector<double> > guard(&myVec);
        // Note that regardless of how this routine terminates, 'myVec'
        // will be destroyed.

    myVec.push_back(3.0);
        // Note that 'push_back' could allocate memory and therefore may
        // throw.  However, if it does, 'myVec' will be destroyed
        // automatically along with 'guard'.

    if (myVec[0] >= 5.0) {
        return 5.0;                                                   // RETURN
            // Note that 'myVec' is automatically destroyed as the
            // function returns.
    }

    return myVec[myVec.size() / 2];
        // Note that 'myVec' is destroyed after the temporary containing
        // the return value is created.
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

    (void)veryVerbose;           // suppress unused variable warning
    (void)veryVeryVerbose;       // suppress unused variable warning
    (void)veryVeryVeryVerbose;   // suppress unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //   That the usage example compiles and produces the expected
        //   results given different inputs.
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        double result;

        result = usageExample(10.0);
        ASSERT(5.0 == result);

        result = usageExample(2.0);
        ASSERT(2.0 == result);

        result = usageExample(-1.0);
        ASSERT(3.0 == result);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Need to exercise basic functionality of 'bslma::DestructorGuard'.
        //   Note that since this class has only a c'tor and d'tor, the
        //   breathing test is a thorough test of the component.
        //
        // Plan:
        //   Have an instance of 'my_Class', a class which can be destructed
        //   multiple times and which keeps count of the number of times it has
        //   been destructed.  Use 'bslma::DestructorGuard' to destruct it
        //   several times and verify that the expected destructions occur.
        //
        // Testing:
        //   bslma::DestructorGuard()
        //   ~bslma::DestructorGuard()
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        int counter = 0;
        int i;
        {
            my_Class mC(&counter);

            for (i = 0; i < 10; ++i) {
                ASSERT(i == counter);

                bslma::DestructorGuard<my_Class> mX(&mC);

                ASSERT(i == counter);
            }
            ASSERT(i == counter);
        }
        ASSERT(i + 1 == counter);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // HELPER CLASS TEST
        //
        // Concerns:
        //   That the destructor for the helper class 'my_Class' increments
        //   the object's counter appropriately, and that there are no ill
        //   effects from calling the destructor on a given instance of
        //   my_Class multiple times.
        //
        // Plan:
        //   Several times, create an instance of 'my_Class' referring to
        //   'counter', verify that 'counter' gets incremented once for every
        //   instance of 'my_Class' going out of scope.  The in another loop
        //   iterate calling the destructor for the same 'my_Class' object
        //   several times, verifying that calling the destructor on the same
        //   object multiple times has the desired effect.
        //
        // Testing:
        //   my_Class(&counter);
        //   ~my_Class();
        // --------------------------------------------------------------------

        if (verbose) printf("\nHELPER CLASS TEST"
                            "\n=================\n");

        if (verbose) printf("Testing 'my_Class'.\n");
        {
            int counter = 0;
            const int NUM_TEST = 5;
            for (int i = 0; i < NUM_TEST; ++i) {
                LOOP_ASSERT(i, counter == i);
                my_Class mX(&counter);
            }
            ASSERT(NUM_TEST == counter);
        }

        {
            int counter = 0;
            const int NUM_TEST = 5;
            {
                my_Class mX(&counter);
                for (int i = 0; i < NUM_TEST; ++i) {
                    ASSERT(i == counter);
                    mX.~my_Class();
                }
                ASSERT(NUM_TEST == counter);
            }
            ASSERT(NUM_TEST + 1 == counter);
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
