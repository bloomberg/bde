// balb_assertiontracker.t.cpp                                        -*-C++-*-

#include <balb_assertiontracker.h>

#include <bslim_testutil.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bslmt_barrier.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>

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
// [ 1] BREATHING TEST
// [ 2] AssertionTracker(bsls::Assert::Handler, bslma::Allocator *);
// [ 2] bslma::Allocator *allocator() const;
// [ 2] void operator()(const char *text, const char *file, int line);
// [ 2] void setReportingCallback(ReportingCallback cb);
// [ 2] void setMaxAssertions(int value);
// [ 2] void setMaxLocations(int value);
// [ 2] void setMaxStackTracesPerLocation(int value);
// [ 2] void setOnEachAssertion(bool value);
// [ 2] void setOnNewLocation(bool value);
// [ 2] void setOnNewStackTrace(bool value);
// [ 2] ReportingCallback reportingCallback() const;
// [ 2] int maxAssertions() const;
// [ 2] int maxLocations() const;
// [ 2] int maxStackTracesPerLocation() const;
// [ 2] bool onEachAssertion() const;
// [ 2] bool onNewLocation() const;
// [ 2] bool onNewStackTrace() const;
// [ 3] void reportAssertion(...);
// [ 3] void reportAllStackTraces() const;

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

typedef balb::AssertionTracker Obj;

int defaultAssertionCount;
void defaultHandler(const char *, const char *, int)
    // Function to be installed as the default handler.
{
    ++defaultAssertionCount;
}

int handledAssertionCount;
void reporter(int, const char *, const char *, int, const bsl::vector<void *>&)
    // Function to be installed as the assertion tracker reporter.
{
    ++handledAssertionCount;
}

void trigger1(Obj& t, const char *a, const char *f, int l)
    // Invoke the specified 't' with the specified 'a', 'f', and 'l';
{
    t(a, f, l);
}

void trigger2(Obj& t, const char *a, const char *f, int l)
    // Invoke 'trigger1' with the specified 't', 'a', 'f', and 'l';
{
    trigger1(t, a, f, l);
}

void trigger3(Obj& t, const char *a, const char *f, int l)
    // Invoke 'trigger1' with the specified 't', 'a', 'f', and 'l';
{
    trigger1(t, a, f, l);
}

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

    bslma::TestAllocator ta("test", veryVeryVeryVerbose);

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // REPORTER TEST
        //   This case tests the default reporting function of the assertion
        //   tracker and its 'iterateAll' method.
        //
        // Concerns:
        //: 1 Assertions are correctly printed to the stream passed to the
        //:   reporting function.
        //:
        //: 2 The 'iterateAll' method sends correct counts.
        //
        // Plan:
        //: 1 Use an 'ostringstream' as the reporting stream and examine its
        //:   contents for correct usage.
        //:
        //: 2 Call 'iterateAll' and examine the contents of the stream.
        //
        // Testing:
        //   void reportAssertion(...);
        //   void reportAllStackTraces() const;
        // --------------------------------------------------------------------
        if (verbose) cout << "\nREPORTER TEST"
                             "\n=============\n";

        bsl::ostringstream os;

        Obj Z(bsls::Assert::failureHandler(), &ta);

        Z.setReportingCallback(bdlf::BindUtil::bind(&Obj::reportAssertion,
                                                    &os,
                                                    bdlf::PlaceHolders::_1,
                                                    bdlf::PlaceHolders::_2,
                                                    bdlf::PlaceHolders::_3,
                                                    bdlf::PlaceHolders::_4,
                                                    bdlf::PlaceHolders::_5));

        for (int i = 0; i < 10; ++i) {
            Z("0 && \"assert 1\"", __FILE__, __LINE__);
            for (int j = 0; j < 10; ++j) {
                Z("0 && \"assert 2\"", __FILE__, __LINE__);
            }
        }

        bsl::string s = os.str();

        ASSERTV(s, s.npos != s.find(__FILE__));
        ASSERTV(s, s.npos != s.find(":1:0 && \"assert 1\":"));
        ASSERTV(s, s.npos != s.find(":1:0 && \"assert 2\":"));

        os.str("");
        Z.reportAllStackTraces();
        s = os.str();

        ASSERTV(s, s.npos != s.find(":10:0 && \"assert 1\":"));
        ASSERTV(s, s.npos != s.find(":100:0 && \"assert 2\":"));
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // INVOCATION TEST
        //   This case tests that the assertion tracker invokes its callback
        //   and fallback handlers the correct number of times, including when
        //   various limits are exceeded, and obeys the parameters describing
        //   when the handlers are called.
        //
        // Concerns:
        //: 1 The tracker will invoke the fallback handler when a limit on the
        //:   number of assertions is set and the limit is exceeded.
        //:
        //: 2 The tracker will invoke the callback for each assertion not over
        //:   the limit when that is enabled.
        //:
        //: 3 The tracker will invoke the fallback handler when a limit on the
        //:   number of locations is set and the limit is exceeded.
        //:
        //: 4 The tracker will invoke the callback for each new assertion
        //:   location not over the limit when that is enabled.
        //:
        //: 5 The tracker will invoke the fallback handler when a limit on the
        //:   number of stack traces per location is set and the limit is
        //:   exceeded.
        //:
        //: 6 The tracker will invoke the callback for each new assertion stack
        //:   not over the limit when that is enabled.
        //:
        //: 7 The value-semantic aspects of the tracker work; the attributes
        //:   that represent the callback, reporting, and limits can be set and
        //:   retrieved correctly.
        //
        // Plan:
        //: 1 Create a table-based test where each table entry describes the
        //:   reporting modes that are enabled, the limits to be observed, an
        //:   array with counts specifying how many times each of 3 assertions
        //:   should be triggered from 3 different stack traces, and finally
        //:   the number of times each we expect the callback and the fallback
        //:   handler to be invoked.  Set up callback and fallback handlers
        //:   that count calls to themselves, set up the assertion tracker and
        //:   trigger1 the assertions based on the counts, and verify that the
        //:   number of calls match.  (C-1..6)
        //:
        //: 2 When setting attributes on the tracker, read them back and verify
        //:   that they are returned as they were set.  (C-7)
        //
        // Testing:
        //   AssertionTracker(bsls::Assert::Handler, bslma::Allocator *);
        //   bslma::Allocator *allocator() const;
        //   void operator()(const char *text, const char *file, int line);
        //   void setReportingCallback(ReportingCallback cb);
        //   void setMaxAssertions(int value);
        //   void setMaxLocations(int value);
        //   void setMaxStackTracesPerLocation(int value);
        //   void setOnEachAssertion(bool value);
        //   void setOnNewLocation(bool value);
        //   void setOnNewStackTrace(bool value);
        //   ReportingCallback reportingCallback() const;
        //   int maxAssertions() const;
        //   int maxLocations() const;
        //   int maxStackTracesPerLocation() const;
        //   bool onEachAssertion() const;
        //   bool onNewLocation() const;
        //   bool onNewStackTrace() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nINVOCATION TEST"
                             "\n===============\n";
        {
            static const struct {
                int  d_line;                       // line number
                bool d_onEachAssertion;            // report each assertion
                bool d_onNewLocation;              // report each new location
                bool d_onNewStackTrace;            // report each new stack
                int  d_maxAssertions;              // max assertions handled
                int  d_maxLocations;               // max locations handled
                int  d_maxStackTracesPerLocation;  // max stacks handled
                int  d_assertion_location[3][3];   // assertions to trigger1
                int  d_expected_handled;
                int  d_expected_default;
            } DATA[] = {
                {L_, 1, 0, 0, -1, -1, -1, {1, 1, 1, 1, 1, 1, 1, 1, 1},  9,  0},
                {L_, 0, 1, 0, -1, -1, -1, {2, 2, 2, 2, 2, 2, 2, 2, 2},  3,  0},
                {L_, 0, 0, 1, -1, -1, -1, {2, 2, 2, 2, 2, 2, 2, 2, 2},  9,  0},
                {L_, 1, 1, 1,  0,  0,  0, {1, 1, 1, 1, 1, 1, 1, 1, 1},  0,  9},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE      = DATA[ti].d_line;
                const bool EACH      = DATA[ti].d_onEachAssertion;
                const bool LOC       = DATA[ti].d_onNewLocation;
                const bool STACK     = DATA[ti].d_onNewStackTrace;
                const int  MAX_A     = DATA[ti].d_maxAssertions;
                const int  MAX_L     = DATA[ti].d_maxLocations;
                const int  MAX_S     = DATA[ti].d_maxStackTracesPerLocation;
                const int  EXP_H     = DATA[ti].d_expected_handled;
                const int  EXP_D     = DATA[ti].d_expected_default;
                const int(&AL)[3][3] = DATA[ti].d_assertion_location;

                if (veryVerbose) {
                    cout << LINE << " "
                         << &"!Each "    [EACH]
                         << &"!NewLoc "  [LOC]
                         << &"!NewStack "[STACK]
                         << "MAXA " << MAX_A << " "
                         << "MAXL " << MAX_L << " "
                         << "MAXS " << MAX_S << "\n"
                         << "CALLS"
                         << " " << AL[0][0]
                         << " " << AL[0][1]
                         << " " << AL[0][2]
                         << " " << AL[1][0]
                         << " " << AL[1][1]
                         << " " << AL[1][2]
                         << " " << AL[2][0]
                         << " " << AL[2][1]
                         << " " << AL[2][2]
                         << "\n"
                         << "EXPECTED HANDLED " << EXP_H << " "
                         << "EXPECTED DEFAULT " << EXP_D << "\n";
                }

                Obj Z(defaultHandler, &ta);
                ASSERT(&ta == Z.allocator());
                Z.setReportingCallback(reporter);
                handledAssertionCount = 0;
                defaultAssertionCount = 0;

                Z.reportingCallback()(0, 0, 0, 0, bsl::vector<void *>());
                ASSERT(1 == handledAssertionCount);
                handledAssertionCount = 0;

                Z.setOnEachAssertion(EACH);
                ASSERT(EACH == Z.onEachAssertion());
                Z.setOnNewLocation(LOC);
                ASSERT(LOC == Z.onNewLocation());
                Z.setOnNewStackTrace(STACK);
                ASSERT(STACK == Z.onNewStackTrace());
                Z.setMaxAssertions(MAX_A);
                ASSERT(MAX_A == Z.maxAssertions());
                Z.setMaxLocations(MAX_L);
                ASSERT(MAX_L == Z.maxLocations());
                Z.setMaxStackTracesPerLocation(MAX_S);
                ASSERT(MAX_S == Z.maxStackTracesPerLocation());

                const char *a0 = "assertion 0", *f0 = __FILE__ "0";
                const char *a1 = "assertion 1", *f1 = __FILE__ "1";
                const char *a2 = "assertion 2", *f2 = __FILE__ "2";
                for (int i = 0; i < AL[0][0]; ++i)
                    trigger1(Z, a0, f0, LINE);
                for (int i = 0; i < AL[0][1]; ++i)
                    trigger2(Z, a0, f0, LINE);
                for (int i = 0; i < AL[0][2]; ++i)
                    trigger3(Z, a0, f0, LINE);
                for (int i = 0; i < AL[1][0]; ++i)
                    trigger1(Z, a1, f1, LINE);
                for (int i = 0; i < AL[1][1]; ++i)
                    trigger2(Z, a1, f1, LINE);
                for (int i = 0; i < AL[1][2]; ++i)
                    trigger3(Z, a1, f1, LINE);
                for (int i = 0; i < AL[2][0]; ++i)
                    trigger1(Z, a2, f2, LINE);
                for (int i = 0; i < AL[2][1]; ++i)
                    trigger2(Z, a2, f2, LINE);
                for (int i = 0; i < AL[2][2]; ++i)
                    trigger3(Z, a2, f2, LINE);

                ASSERTV(LINE, EXP_H, handledAssertionCount,
                        EXP_H == handledAssertionCount);
                ASSERTV(LINE, EXP_D, defaultAssertionCount,
                        EXP_D == defaultAssertionCount);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class can be invoked with the assertion failure triple, and
        //:   it can iterate over the accumulated assertions.
        //
        // Plan:
        //: 1 Create an assertion tracker object and invoke a few of its
        //:   methods.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============\n";
        {
            Obj Z(bsls::Assert::failureHandler(), &ta);
            for (int i = 0; i < 10; ++i) {
                Z("assertion 1", __FILE__, __LINE__);
                for (int j = 0; j < 10; ++j) {
                    Z("assertion 2", __FILE__, __LINE__);
                }
            }
            Z.reportAllStackTraces();
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
