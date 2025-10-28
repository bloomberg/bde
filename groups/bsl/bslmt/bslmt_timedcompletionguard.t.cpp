// bslmt_timedcompletionguard.t.cpp                                   -*-C++-*-
#include <bslmt_timedcompletionguard.h>

#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bslmt_threadutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <bsl_cstring.h>  // `strcmp`
#include <bsl_cstdlib.h>  // `atoi`
#include <bsl_iostream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//                              --------
// The component under test implements a thread-enabled time completion guard.
// The primary manipulators are the methods for initiating a timed completion
// guard (`guard`) and releaseing a guard (`release`).  The basic accessors are
// the methods for obtaining the allocator (`get_allocator`) and whether the
// guard is active (`isGuarding`).
//
// Global Concerns:
//  - The test driver is robust w.r.t. reuse in other, similar components.
//  - ACCESSOR methods are declared `const`.
//  - CREATOR & MANIPULATOR pointer/reference parameters are declared `const`.
//  - No memory is ever allocated from the global allocator.
//  - Any allocated memory is always from the object allocator.
//  - An object's value is independent of the allocator used to supply memory.
//  - Injected exceptions are safely propagated during memory allocation.
//  - Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//  - All explicit memory allocations are presumed to use the global, default,
//    or object allocator.
//  - ACCESSOR methods are `const` thread-safe.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] TimedCompletionGuard();
// [ 2] explicit TimedCompletionGuard(const allocator_type& bA);
// [ 2] TimedCompletionGuard(Handler h, const allocator_type& bA = {});
// [ 2] ~TimedCompletionGuard();
//
// MANIPULATORS
// [ 2] int guard(const bsls::TimeInterval& d, const bsl::string_view& t);
// [ 2] void release();
// [ 4] int updateText(const bsl::string_view& text);
//
// ACCESSORS
// [ 3] allocator_type get_allocator() const;
// [ 3] bool isGuarding() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

BSLA_MAYBE_UNUSED void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)

// ============================================================================
//                   GLOBAL STRUCTS/FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static bsl::string *s_copiedText_p;

/// Assign the value in the specified `text` to `*s_copiedText_p`.
void copyTextHandler(const char *text)
{
    *s_copiedText_p = text;
}

/// Assign the value in the specified `text` concatenated with a period to
/// `*s_copiedText_p`.
void otherCopyTextHandler(const char *text)
{
    *s_copiedText_p = bsl::string(text) + ".";
}

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslmt::TimedCompletionGuard Obj;

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Guarding Work on a Thread
///- - - - - - - - - - - - - - - - - -
// In the following example a `bslmt::TimedCompletionGuard` is used as a
// testing aid verify work done on a thread completes in a specified amount of
// time.  The work is done in two parts, but the actual tasks are not relevant
// to the example and is elided.
//
// First, define the two thread functions containing the work to be done:
// ```
    /// Do some work based upon the specified `arg`.
    void *myWorkPart1(void *arg)
    {
        // do some stuff...
        (void)arg;
        return 0;
    }

    /// Do some other work based upon the specified `arg`.
    void *myWorkPart2(void *arg)
    {
        // do some other stuff...
        (void)arg;
        return 0;
    }
// ```

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    BSLA_MAYBE_UNUSED int                 test = argc > 1 ? atoi(argv[1]) : 0;
    BSLA_MAYBE_UNUSED bool             verbose = argc > 2;
    BSLA_MAYBE_UNUSED bool         veryVerbose = argc > 3;
    BSLA_MAYBE_UNUSED bool     veryVeryVerbose = argc > 4;
    BSLA_MAYBE_UNUSED bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    s_copiedText_p = new bsl::string(&defaultAllocator);

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

// Next, we create and configure `tcg` to allow both parts of the work one
// second to complete and verify the guard was started successfully:
// ```
    bslmt::TimedCompletionGuard tcg;

    ASSERT(0 == tcg.guard(bsls::TimeInterval(1, 0), "first part"));
// ```
// Then, we create a thread to execute the first part of the work, and join the
// created thread when the work completes:
// ```
    {
        bslmt::ThreadUtil::Handle workerHandle;

        ASSERT(0 == bslmt::ThreadUtil::create(&workerHandle, myWorkPart1, 0));

        bslmt::ThreadUtil::join(workerHandle);
    }
// ```
// Next, we update the text displayed if the duration should expire to reflect
// the second portion of the work:
// ```
    tcg.updateText("second part");
// ```
// Now, we use a thread to execute the second part of the work:
// ```
    {
        bslmt::ThreadUtil::Handle workerHandle;

        ASSERT(0 == bslmt::ThreadUtil::create(&workerHandle, myWorkPart2, 0));

        bslmt::ThreadUtil::join(workerHandle);
    }
// ```
// Finally, we release the guard:
// ```
    tcg.release();
// ```
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TEST `updateText`
        //   The concern is `updateText` operates as expected.
        //
        // Concerns:
        // 1. The `updateText` method changes the supplied expiration text.
        //
        // 2. The `updateText` method return the correct value.
        //
        // Plan:
        // 1. Create objects using `copyTextHandler` and use the `guard` method
        //    to initiate guarding.  User `updateText` to change the expiration
        //    text and verify the return value.  (C-1)
        //
        // Testing:
        //   int updateText(const bsl::string_view& text);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST `updateText`" << endl
                          << "=================" << endl;
        {
            *s_copiedText_p = "";

            Obj mX(copyTextHandler);

            ASSERT(0 != mX.updateText(""));

            ASSERT(0 == mX.guard(bsls::TimeInterval(0, 400000000), "a"));

            ASSERT(0 == mX.updateText("b"));

            bslmt::ThreadUtil::microSleep(200000);

            ASSERT(0 == mX.updateText("c"));

            bslmt::ThreadUtil::microSleep(300000);

            ASSERT(*s_copiedText_p == "c");

            ASSERT(0 == mX.guard(bsls::TimeInterval(0, 200000000), "a"));

            ASSERT(0 == mX.updateText("b"));

            bslmt::ThreadUtil::microSleep(300000);

            ASSERT(*s_copiedText_p == "b");
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS TEST
        //   The concern is the basic accessors:
        //      - get_allocator
        //      - isGuarding
        //   operate as expected.
        //
        // Concerns:
        // 1. The used allocator is returned by `get_allocator`.
        //
        // 2. The `isGuarding` method returns the current state.
        //
        // Plan:
        // 1. Create objects with various allocators and verify `get_allocator`
        //    returns the correct allocator.  (C-1)
        //
        // 2. Create objects using `copyTextHandler` and use the `guard` method
        //    to initiate guarding.  Interleave the `isGuarding` accessor to
        //    verify when guarding is active.  (C-2)
        //
        // Testing:
        //   allocator_type get_allocator() const;
        //   bool isGuarding() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS TEST" << endl
                          << "====================" << endl;

        if (verbose) cout << "\nTesting `get_allocator`." << endl;
        {
            {
                Obj mX;  const Obj& X = mX;

                ASSERT(&defaultAllocator == X.get_allocator());
            }
            {
                Obj mX(&globalAllocator);  const Obj& X = mX;

                ASSERT(&globalAllocator == X.get_allocator());
            }
            {
                bslma::TestAllocator ta("text", veryVeryVeryVerbose);

                Obj mX(&ta);  const Obj& X = mX;

                ASSERT(&ta == X.get_allocator());
            }
        }

        if (verbose) cout << "\nTesting `isGuarding`." << endl;
        {
            Obj mX(copyTextHandler);  const Obj& X = mX;

            ASSERT(false == X.isGuarding());

            ASSERT(0 == mX.guard(bsls::TimeInterval(0, 400000000), "a"));

            ASSERT(true  == X.isGuarding());

            bslmt::ThreadUtil::microSleep(200000);

            ASSERT(true  == X.isGuarding());

            bslmt::ThreadUtil::microSleep(300000);

            ASSERT(false == X.isGuarding());

            ASSERT(0 == mX.guard(bsls::TimeInterval(0, 400000000), "a"));

            ASSERT(true  == X.isGuarding());

            mX.release();

            ASSERT(false == X.isGuarding());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CREATORS AND PRIMARY MANIPULATORS TEST
        //   The basic concern is the constructors, destructor, and primary
        //   manipulators:
        //      - guard
        //      - release
        //   operate as expected.
        //
        // Concerns:
        // 1. The constructors create store the handler if specified and have
        //    the internal memory management system hooked up properly so that
        //    *all* internally allocated memory draws from the same
        //    user-supplied allocator whenever one is specified.
        //
        // 2. The method `guard` initiates guarding with the expected
        //    parameters.
        //
        // 3. The method `release` terminates guarding.
        //
        // 4. Memory is not leaked by any method and the destructor properly
        //    deallocates the residual allocated memory.
        //
        // 5. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Create objects using the constructors with and without
        //    supplying a handler and passing in an allocator, verify the
        //    handler supplied is used, the allocator is stored using the
        //    (untested) `get_allocator` accessor, and verify all allocations
        //    are done from this allocator.  (C-1)
        //
        // 2. Create objects using `copyTextHandler` and
        //    `otherCopyTextHandler`, use the `guard` method to initiate
        //    guarding, and verify the resultant text to ensure the guard
        //    worked as expected.  Use the (untested) `isGuarding` accessor to
        //    verify when guarding is active.  (C-2)
        //
        // 3. Repeat the tests from P-2 but use `release` to prevent the
        //    durations from expiring.  (C-3)
        //
        // 4. Use a supplied `bslma::TestAllocator` that goes out-of-scope
        //    at the conclusion of each test to ensure all memory is returned
        //    to the allocator.  (C-4)
        //
        // 5. Verify defensive checks are triggered for invalid values.  (C-5)
        //
        // Testing:
        //   TimedCompletionGuard();
        //   explicit TimedCompletionGuard(const allocator_type& bA);
        //   TimedCompletionGuard(Handler h, const allocator_type& bA = {});
        //   ~TimedCompletionGuard();
        //   int guard(const bsls::TimeInterval& d, const bsl::string_view& t);
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CREATORS AND PRIMARY MANIPULATORS TEST" << endl
                          << "======================================" << endl;

        bsl::string longString;
        {
            longString = "abc";
            for (bsl::size_t i = 0; i < sizeof(bsl::string); ++i) {
                longString += " ";
            }
        }

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;
        {
            {
                bsls::Types::Int64 na = defaultAllocator.numAllocations();

                Obj mX;  const Obj& X = mX;

                ASSERT(&defaultAllocator == X.get_allocator());

                ASSERT(false == X.isGuarding());

                ASSERT(na == defaultAllocator.numAllocations());

                ASSERT(0 == mX.guard(bsls::TimeInterval(1, 0), longString));

                ASSERT(true == X.isGuarding());

                mX.release();

                ASSERT(false == X.isGuarding());

                ASSERT(na < defaultAllocator.numAllocations());
            }
            {
                bslma::TestAllocator ta("text", veryVeryVeryVerbose);

                {
                    Obj mX(&globalAllocator);  const Obj& X = mX;

                    ASSERT(&globalAllocator == X.get_allocator());

                    ASSERT(false == X.isGuarding());
                }
                {
                    bsls::Types::Int64 na = ta.numAllocations();

                    Obj mX(&ta);  const Obj& X = mX;

                    ASSERT(&ta == X.get_allocator());

                    ASSERT(false == X.isGuarding());

                    ASSERT(0 == mX.guard(bsls::TimeInterval(1, 0),
                                         longString));

                    ASSERT(true == X.isGuarding());

                    mX.release();

                    ASSERT(false == X.isGuarding());

                    ASSERT(na < ta.numAllocations());
                }
            }
            {
                bslma::TestAllocator ta("text", veryVeryVeryVerbose);

                {
                    bsls::Types::Int64 na = defaultAllocator.numAllocations();

                    Obj mX(copyTextHandler);  const Obj& X = mX;

                    ASSERT(&defaultAllocator == X.get_allocator());

                    ASSERT(false == X.isGuarding());

                    ASSERT(na == defaultAllocator.numAllocations());

                    ASSERT(0 == mX.guard(bsls::TimeInterval(1, 0),
                                         longString));

                    ASSERT(true == X.isGuarding());

                    mX.release();

                    ASSERT(false == X.isGuarding());

                    ASSERT(na < defaultAllocator.numAllocations());
                }
                {
                    Obj        mX(copyTextHandler, &globalAllocator);
                    const Obj& X = mX;

                    ASSERT(&globalAllocator == X.get_allocator());

                    ASSERT(false == X.isGuarding());
                }
                {
                    bsls::Types::Int64 na = ta.numAllocations();

                    Obj        mX(copyTextHandler, &ta);
                    const Obj& X = mX;

                    ASSERT(&ta == X.get_allocator());

                    ASSERT(false == X.isGuarding());

                    ASSERT(0 == mX.guard(bsls::TimeInterval(1, 0),
                                         longString));

                    ASSERT(true == X.isGuarding());

                    mX.release();

                    ASSERT(false == X.isGuarding());

                    ASSERT(na < ta.numAllocations());
                }
            }
        }

        if (verbose) cout << "\nTesting `guard`."
                          << endl;
        {
            {
                Obj mX(&copyTextHandler);  const Obj& X = mX;

                {
                    *s_copiedText_p = "";

                    ASSERT(false == X.isGuarding());

                    ASSERT(0 == mX.guard(bsls::TimeInterval(0, 100000000),
                                         "a"));

                    ASSERT(true            == X.isGuarding());
                    ASSERT(*s_copiedText_p == "");

                    bslmt::ThreadUtil::microSleep(200000);
                    ASSERT(*s_copiedText_p == "a");

                    ASSERT(false == X.isGuarding());
                }
                {
                    *s_copiedText_p = "";

                    ASSERT(false == X.isGuarding());

                    ASSERT(0 == mX.guard(bsls::TimeInterval(0, 400000000),
                                         "b"));

                    ASSERT(true            == X.isGuarding());
                    ASSERT(*s_copiedText_p == "");

                    bslmt::ThreadUtil::microSleep(200000);
                    ASSERT(*s_copiedText_p == "");

                    bslmt::ThreadUtil::microSleep(300000);
                    ASSERT(*s_copiedText_p == "b");

                    ASSERT(false == X.isGuarding());
                }
            }
            {
                Obj mX(&otherCopyTextHandler);  const Obj& X = mX;

                {
                    *s_copiedText_p = "";

                    ASSERT(false == X.isGuarding());

                    ASSERT(0 == mX.guard(bsls::TimeInterval(0, 100000000),
                                         "a"));

                    ASSERT(true            == X.isGuarding());
                    ASSERT(*s_copiedText_p == "");

                    bslmt::ThreadUtil::microSleep(200000);
                    ASSERT(*s_copiedText_p == "a.");

                    ASSERT(false == X.isGuarding());
                }
            }
        }

        if (verbose) cout << "\nTesting `release`."
                          << endl;
        {
            {
                Obj mX(&copyTextHandler);  const Obj& X = mX;

                {
                    *s_copiedText_p = "";

                    ASSERT(false == X.isGuarding());

                    ASSERT(0 == mX.guard(bsls::TimeInterval(0, 100000000),
                                         "a"));

                    ASSERT(true            == X.isGuarding());
                    ASSERT(*s_copiedText_p == "");

                    mX.release();

                    ASSERT(false           == X.isGuarding());
                    ASSERT(*s_copiedText_p == "");

                    bslmt::ThreadUtil::microSleep(200000);
                    ASSERT(*s_copiedText_p == "");

                    ASSERT(false == X.isGuarding());
                }
                {
                    *s_copiedText_p = "";

                    ASSERT(false == X.isGuarding());

                    ASSERT(0 == mX.guard(bsls::TimeInterval(0, 400000000),
                                         "b"));

                    ASSERT(true            == X.isGuarding());
                    ASSERT(*s_copiedText_p == "");

                    mX.release();

                    ASSERT(false           == X.isGuarding());
                    ASSERT(*s_copiedText_p == "");

                    bslmt::ThreadUtil::microSleep(200000);
                    ASSERT(*s_copiedText_p == "");

                    bslmt::ThreadUtil::microSleep(300000);
                    ASSERT(*s_copiedText_p == "");

                    ASSERT(false == X.isGuarding());
                }
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX(copyTextHandler);

            ASSERT_PASS(mX.guard(bsls::TimeInterval( 1), "a"));
            ASSERT_FAIL(mX.guard(bsls::TimeInterval( 0), "a"));
            ASSERT_FAIL(mX.guard(bsls::TimeInterval(-1), "a"));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Instantiate an object and verify basic functionality.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        {
            Obj mX(&copyTextHandler);

            ASSERT(0 == mX.guard(bsls::TimeInterval(0, 100000000), "a"));

            ASSERT(*s_copiedText_p == "");
            bslmt::ThreadUtil::microSleep(250000);
            ASSERT(*s_copiedText_p == "a");

            ASSERT(0 == mX.guard(bsls::TimeInterval(1, 0), "b"));
        }
        ASSERT(*s_copiedText_p == "a");
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    delete s_copiedText_p;

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
