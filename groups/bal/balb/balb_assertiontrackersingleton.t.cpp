// balb_assertiontrackersingleton.t.cpp                               -*-C++-*-

#include <balb_assertiontrackersingleton.h>
#include <balb_assertiontracker.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bslim_testutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_utility.h>

using namespace BloombergLP;
using namespace bsl;
using namespace bdlf::PlaceHolders;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// Test this thing.
// ----------------------------------------------------------------------------
// CLASS METHODS
// CREATORS
// MANIPULATORS
// ACCESSORS
// ----------------------------------------------------------------------------
// [ 1] void failTracker(const char *, const char *, int);
// [ 1] TRACKER *singleton();
// [ 1] TRACKER *createAndInstallSingleton();
// [ 2] TRACKER *createAndInstallSingleton(BregCallback);
// [ 2] void bregCallbackAdapter(BregCallback, int*,int*,int*,int*,int*);
// [ 3] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
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

#define ASSERT BSLIM_TESTUTIL_ASSERT
#define ASSERTV BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q BSLIM_TESTUTIL_Q    // Quote identifier literally.
#define P BSLIM_TESTUTIL_P    // Print identifier and value.
#define P_ BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//            GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

typedef balb::AssertionTrackerSingleton<balb::AssertionTracker> Obj;

// 'MSVC' optimizations (like omitting frame pointers, loop unrolling, and
// inlining) can interfere with the gathering of stack traces and with the
// (perhaps overly simplistic) expectations of the tests below.  With
// optimization enabled, this test driver is observed to fail some of its
// stack-trace based tests unless this pragma is specified.
//
// The use of functions with variadic argument lists and volatile automatic
// variables below is also meant to discourage optimizations that interfere
// with standard stack frames.

#ifdef BSLS_PLATFORM_CMP_MSVC
#pragma optimize("gsty", off)
#endif

void bregSimulator(int *b1, int *b2, int *b3)
    // Fill the specified 'b1', 'b2', and 'b3' values with simulated 'BREG'
    // values.
{
    *b1 = 1;
    *b2 = 4;
    *b3 = 20;
}

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Count Number of Assertions Triggered
///- - - - - - - - - - - - - - - - - - - - - - - -
// We wish to count how many times assertions trigger, doing no other handling
// and allowing the program to continue running (note that this is normally
// forbidden in production).  We can use 'AssertionTrackerSingleton' to set up
// an assertion handler to do this.
//
// First, we create a class to do the counting.
//..
    class AssertionCounter {
        // PRIVATE DATA
        int d_assertion_counter;  // Number of assertions seen.

      public:
        // PUBLIC TYPES
        typedef void *ConfigurationCallback;  // We're not using it.

        // PUBLIC CREATORS
        explicit AssertionCounter(bsls::Assert::Handler   = 0,
                                  void                  * = 0,
                                  void                  * = 0);
            // Create an 'AssertionCounter' object.  We ignore the fallback
            // handler, configuration callback, and allocator parameters.

        // PUBLIC MANIPULATORS
        void assertionDetected(const char *, const char *, int);
            // Function called when assertion failure occurs.  We ignore the
            // text, file, and line parameters.

        // PUBLIC ACCESSORS
        int getAssertionCount() const;
            // Return the number of assertions we have seen.
    };
//..
// Then, we implement the member functions of the 'AssertionCounter' class.
//..
    AssertionCounter::AssertionCounter(bsls::Assert::Handler, void *, void *)
    : d_assertion_counter(0)
    {
    }

    void AssertionCounter::assertionDetected(const char *, const char *, int)
    {
        ++d_assertion_counter;
    }

    int AssertionCounter::getAssertionCount() const
    {
        return d_assertion_counter;
    }
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int                 test = (argc > 1) ? atoi(argv[1]) : 1;
    bool             verbose = (argc > 2); (void)verbose;
    bool         veryVerbose = (argc > 3); (void)veryVerbose;
    bool     veryVeryVerbose = (argc > 4); (void)veryVeryVerbose;
    bool veryVeryVeryVerbose = (argc > 5); (void)veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE"
                             "\n=============\n";
// Next, we set up an instance of this class to be the installed assertion
// using 'AssertionTrackerSingleton'.  Note that this needs to be done early in
// 'main()' before any other handlers are installed.  If another handler has
// already been installed, a null pointer will be returned, and we assert that
// this does not happen.
//..
    AssertionCounter *ac_p = balb::AssertionTrackerSingleton<AssertionCounter>
        ::createAndInstallSingleton();
    ASSERT(ac_p);
//..
// Finally, we will trigger some assertions and verify that we are counting
// them correctly.
//..
    BSLS_ASSERT_ASSERT(0 && "assertion 1");
    BSLS_ASSERT_ASSERT(0 && "assertion 2");
    ASSERT(ac_p->getAssertionCount() == 2);
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BREG CALLBACK TEST
        //   This case tests creating the singleton using the 'breg' version of
        //   configuration.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to install an assertion
        //:   handler and process assertions.
        //
        // Plan:
        //: 1 Create a singleton 'AssertionTracker' object, trigger some
        //:   assertions, and verify that they are noticed by having the
        //:   assertion handler write them to a string stream and then
        //:   examining the contents of the stream.
        //
        // Testing:
        //   TRACKER *createAndInstallSingleton(BregCallback);
        //   void bregCallbackAdapter(BregCallback, int*,int*,int*,int*,int*);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "BREG CALLBACK TEST\n"
                    "==================\n";
        {
            bsl::ostringstream      os;
            balb::AssertionTracker *at_p =
                Obj::createAndInstallSingleton(bregSimulator);

            int a = 0, b = 0, c = 0, d = 0, e = 0;
            Obj::bregCallbackAdapter(bregSimulator, &a, &b, &c, &d, &e);
            ASSERT(0 == a && 0 == b && 0 != c && 0 != d && 0 == e);
            a = b = c = d = e = 0;
            at_p->configurationCallback()(&a, &b, &c, &d, &e);
            ASSERT(0 == a && 0 == b && 0 != c && 0 != d && 0 == e);

            ASSERT(at_p);
            at_p->setReportingCallback(
                bdlf::BindUtil::bind(&balb::AssertionTracker::reportAssertion,
                                     &os, _1, _2, _3, _4, _5, _6));

            for (int i = 0; i < 10; ++i) {
                if (veryVeryVerbose) { P_(i) Q("assert 1") }
                BSLS_ASSERT_ASSERT(0 && "assert 1");
                for (int j = 0; j < 10; ++j) {
                    if (veryVeryVerbose) { P_(i) P_(j) Q("assert 2") }
                    BSLS_ASSERT_ASSERT(0 && "assert 2");
                }
            }

            for (int i = 0; i < 2; ++i) {
                if (veryVeryVerbose) { P_(i) Q("assert 3") }
                Obj::failTracker("0 && \"assert 3\"", __FILE__, __LINE__);
            }

            bsl::string s = os.str();

            ASSERTV(s, s.npos != s.find(__FILE__));

            ASSERTV(s, s.npos != s.find(":1:0 && \"assert 1\":"))
            ASSERTV(s, s.npos != s.find(":1:0 && \"assert 2\":"))
            ASSERTV(s, s.npos != s.find(":1:0 && \"assert 3\":"))

            os.str("");
            at_p->reportAllRecordedStackTraces();
            s = os.str();

            ASSERTV(s, s.npos != s.find(":10:0 && \"assert 1\":"))
            ASSERTV(s, s.npos != s.find(":100:0 && \"assert 2\":"))
            ASSERTV(s, s.npos != s.find(":2:0 && \"assert 3\":"))
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to install an assertion
        //:   handler and process assertions.
        //
        // Plan:
        //: 1 Create a singleton 'AssertionTracker' object, trigger some
        //:   assertions, and verify that they are noticed by having the
        //:   assertion handler write them to a string stream and then
        //:   examining the contents of the stream.
        //
        // Testing:
        //   void failTracker(const char *, const char *, int);
        //   TRACKER *createAndInstallSingleton();
        //   TRACKER *singleton();
        // --------------------------------------------------------------------

        if (verbose)
            cout << "BREATHING TEST\n"
                    "==============\n";
        {
            bsl::ostringstream      os;
            balb::AssertionTracker *at_p = Obj::createAndInstallSingleton();

            ASSERT(at_p);
            ASSERT(Obj::singleton() == at_p);
            at_p->setReportingCallback(
                bdlf::BindUtil::bind(&balb::AssertionTracker::reportAssertion,
                                     &os, _1, _2, _3, _4, _5, _6));

            for (int i = 0; i < 10; ++i) {
                if (veryVeryVerbose) { P_(i) Q("assert 1") }
                BSLS_ASSERT_ASSERT(0 && "assert 1");
                for (int j = 0; j < 10; ++j) {
                    if (veryVeryVerbose) { P_(i) P_(j) Q("assert 2") }
                    BSLS_ASSERT_ASSERT(0 && "assert 2");
                }
            }

            for (int i = 0; i < 2; ++i) {
                if (veryVeryVerbose) { P_(i) Q("assert 3") }
                Obj::failTracker("0 && \"assert 3\"", __FILE__, __LINE__);
            }

            bsl::string s = os.str();

            ASSERTV(s, s.npos != s.find(__FILE__));

            ASSERTV(s, s.npos != s.find(":1:0 && \"assert 1\":"))
            ASSERTV(s, s.npos != s.find(":1:0 && \"assert 2\":"))
            ASSERTV(s, s.npos != s.find(":1:0 && \"assert 3\":"))

            os.str("");
            at_p->reportAllRecordedStackTraces();
            s = os.str();

            ASSERTV(s, s.npos != s.find(":10:0 && \"assert 1\":"))
            ASSERTV(s, s.npos != s.find(":100:0 && \"assert 2\":"))
            ASSERTV(s, s.npos != s.find(":2:0 && \"assert 3\":"))
        }
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

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
