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
using namespace bsl;  // automatically added by script

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
// [ 1] TRACKER *singleton(bslma::Allocator * = 0);
// [ 2] USAGE EXAMPLE

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

int id(int i)
    // Return the specified 'i'.
{
    return i;
}

void trigger1()
    // Invoke assertion type 1.
{
    BSLS_ASSERT_ASSERT(0 && "assert 1");
}

void trigger2()
    // Invoke assertion type 2.
{
    BSLS_ASSERT_ASSERT(0 && "assert 2");
}

void trigger3()
    // Report assertion type 3.
{
    Obj::failTracker("0 && \"assert 3\"", __FILE__, __LINE__);
}

// Try to scotch inlining and loop unrolling
int  (*volatile id_p)(int)    = id;
void (*volatile trigger1_p)() = trigger1;
void (*volatile trigger2_p)() = trigger2;
void (*volatile trigger3_p)() = trigger3;

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
        // PUBLIC CREATORS
        explicit AssertionCounter(bsls::Assert::Handler = 0, void * = 0);
            // Create an 'AssertionCounter' object.  We ignore the fallback
            // handler and optional allocator parameters.

        // PUBLIC MANIPULATORS
        void operator()(const char *, const char *, int);
            // Function called when assertion failure occurs.  We ignore the
            // text, file, and line parameters.

        // PUBLIC ACCESSORS
        int getAssertionCount() const;
            // Return the number of assertions we have seen.
    };
//..
// Then, we implement the member functions of the 'AssertionCounter' class.
//..
    AssertionCounter::AssertionCounter(bsls::Assert::Handler, void *)
    : d_assertion_counter(0)
    {
    }

    void AssertionCounter::operator()(const char *, const char *, int)
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
      case 2: {
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
    AssertionCounter *ac_p =
        balb::AssertionTrackerSingleton<AssertionCounter>::singleton();
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
        //   TRACKER *singleton(bslma::Allocator * = 0);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "BREATHING TEST\n"
                    "==============\n";
        {
            bsl::ostringstream      os;
            balb::AssertionTracker *at_p = Obj::singleton();

            ASSERT(at_p);
            at_p->setReportingCallback(
                bdlf::BindUtil::bind(&balb::AssertionTracker::reportAssertion,
                                     &os,
                                     bdlf::PlaceHolders::_1,
                                     bdlf::PlaceHolders::_2,
                                     bdlf::PlaceHolders::_3,
                                     bdlf::PlaceHolders::_4,
                                     bdlf::PlaceHolders::_5));
            os << "\n";
            volatile int i, j;
            i = id_p(0);
            do {
                if (veryVeryVerbose) {
                    P_(i) Q("assert 1")
                }
                trigger1_p();
                j = id_p(0);
                do {
                    if (veryVeryVerbose) {
                        P_(i) P_(j) Q("assert 2")
                    }
                    trigger2_p();
                    j = id_p(j) + id_p(1);
                } while (id_p(j) < id_p(10));
                i = id_p(i) + id_p(1);
            } while (id_p(i) < id_p(10));
            i = id_p(0);
            do {
                if (veryVeryVerbose) {
                    P_(i) Q("assert 3")
                }
                trigger3_p();
                i = id_p(i) + id_p(1);
            } while (id_p(i) < id_p(2));

            bsl::string s = os.str();

            ASSERTV(s, s.npos != s.find(__FILE__));

            ASSERTV(s, s.npos != s.find(":1:0 && \"assert 1\":"))
            ASSERTV(s, s.npos != s.find(":1:0 && \"assert 2\":"))
            ASSERTV(s, s.npos != s.find(":1:0 && \"assert 3\":"))

            os.str("");
            at_p->reportAllStackTraces();
            s = os.str();

#ifdef BSLS_PLATFORM_OS_WINDOWS
            // In some Windows builds the stack traces appear to be split, with
            // a each assertion appearing twice with different stack traces. I
            // have seen this with cl-18, while cl-19 seems to work properly.
            // For now, just test that the assertions show up at all.
            ASSERTV(s, s.npos != s.find(":0 && \"assert 1\":"))
            ASSERTV(s, s.npos != s.find(":0 && \"assert 2\":"))
            ASSERTV(s, s.npos != s.find(":0 && \"assert 3\":"))
#else
            ASSERTV(s, s.npos != s.find(":10:0 && \"assert 1\":"))
            ASSERTV(s, s.npos != s.find(":100:0 && \"assert 2\":"))
            ASSERTV(s, s.npos != s.find(":2:0 && \"assert 3\":"))
#endif
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
