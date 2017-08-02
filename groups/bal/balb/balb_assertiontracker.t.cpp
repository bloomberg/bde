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

#include <bsls_logseverity.h>

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
// [ 2] AssertionTracker(bsls::Assert::Handler, bslma::Allocator *);
// [ 2] bslma::Allocator *allocator() const;
// [ 2] void assertionDetected(const char *, const char *, int);
// [ 2] void setReportingCallback(ReportingCallback cb);
// [ 2] void setMaxAssertions(int value);
// [ 2] void setMaxLocations(int value);
// [ 2] void setMaxStackTracesPerLocation(int value);
// [ 2] void setReportingFrequency(ReportingFrequency value);
// [ 2] void setReportingSeverity(bsls::LogSeverity::Enum value);
// [ 2] ReportingCallback reportingCallback() const;
// [ 2] int maxAssertions() const;
// [ 2] int maxLocations() const;
// [ 2] int maxStackTracesPerLocation() const;
// [ 2] ReportingFrequency reportingFrequency() const;
// [ 2] bsls::LogSeverity::Enum reportingSeverity() const;
// [ 2] void preserveConfiguration(int *, int *, int *, int *, int *);
// [ 2] ConfigurationCallback configurationCallback();
// [ 2] void setConfigurationCallback(ConfigurationCallback);
// [ 3] void reportAssertion(...);
// [ 3] void reportAllRecordedStackTraces() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] RECURSION
// [ 5] USAGE EXAMPLE

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

int defaultAssertionCount;
void defaultHandler(const char *, const char *, int)
    // Function to be installed as the default handler.
{
    ++defaultAssertionCount;
}

int handledAssertionCount;
void reporter(int,
              int,
              const char *,
              const char *,
              int,
              const bsl::vector<void *>&)
    // Function to be installed as the assertion tracker reporter.
{
    ++handledAssertionCount;
}

void trigger1(Obj& t, const char *a, const char *f, int l, ...)
    // Invoke the specified 't' with the specified 'a', 'f', and 'l';
{
    volatile int n = l;
    t.assertionDetected(a, f, n);
}

void trigger2(Obj& t, const char *a, const char *f, int l, ...)
    // Invoke 'trigger1' with the specified 't', 'a', 'f', and 'l';
{
    volatile int n = l;
    trigger1(t, a, f, n);
}

void trigger3(Obj& t, const char *a, const char *f, int l, ...)
    // Invoke 'trigger1' with the specified 't', 'a', 'f', and 'l';
{
    volatile int n = l;
    trigger1(t, a, f, n);
}

void assertingReporter(Obj                        *t,
                       int                          ,
                       int                          ,
                       const char                 *a,
                       const char                 *f,
                       int                         l,
                       const bsl::vector<void *>&   )
    // A reporter that itself triggers an assertion, invoking the specified 't'
    // with the specified 'a', 'f', and 'l'.
{
    t->assertionDetected(a, f, l);
}

static bool localConfigurationCalled;

void localConfiguration(int *, int *, int *, int *, int *)
    // A configuration function that does nothing, and says that it has been
    // called
{
    localConfigurationCalled = true;
}

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Historic Broken Assertions
///- - - - - - - - - - - - - - - - - - -
// We have a function that has been running without assertions enabled.  We
// have reason to believe that the assertions will trigger if enabled, but that
// the effects of the violations will mostly be benignly erased (which is why
// the problems have not been detected so far).  We want to enable the
// assertions and fix the places that are causing them to trigger, but we do
// not want the program to abort each time an assertion occurs because it will
// slow down the task of information gathering, and because we need to gather
// the information in production.  We can use 'bsls::AssertionTracker' for
// this purpose.
//
// First, we will place a local static 'AssertionTracker' object ahead of the
// function we want to instrument, and create custom assertion macros just for
// that function.
//..
    namespace {
    balb::AssertionTracker theTracker;
    #define TRACK_ASSERT(condition) do { if (!(condition)) { \
    theTracker.assertionDetected(#condition, __FILE__, __LINE__); } } while (0)
    }  // close unnamed namespace
//..
// Then, we define the function to be traced, and use the modified assertions.
//..
     void foo(int percentage)
         // Receive the specified 'percentage'.  The behavior is undefined
         // unless '0 < percentage < 100'.
     {
         TRACK_ASSERT(  0 < percentage);
         TRACK_ASSERT(100 > percentage);
     }
//..
// Next, we create some uses of the function that may trigger the assertions.
//..
     void useFoo()
         // Trigger assertions by passing invalid values to 'foo'.
     {
         for (int i = 0; i < 100; ++i) {
             foo(i);
         }
         for (int i = 100; i > 0; --i) {
             foo(i);
         }
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

    bslma::TestAllocator ta("test", veryVeryVeryVerbose);

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plans:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters and '{{{ Remove This }}}' section, and
        //:   replace 'assert' with 'ASSERT'.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << "\nUSAGE EXAMPLE"
                             "\n=============\n";

// Finally, we prepare to track the assertion failures.  We will have the
// tracker report into a string stream object so that we can examine it.  We
// configure the tracker, trigger the assertions, and verify that they have
// been correctly discovered.
//..
     bsl::ostringstream os;
     theTracker.setReportingCallback(
         bdlf::BindUtil::bind(balb::AssertionTracker::reportAssertion,
                              &os, _1, _2, _3, _4, _5, _6));
     theTracker.setReportingFrequency(
                                    balb::AssertionTracker::e_onEachAssertion);
     useFoo();
     bsl::string report = os.str();
     ASSERT(report.npos != report.find("0 < percentage"));
     ASSERT(report.npos != report.find("100 > percentage"));
//..
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // RECURSION TEST
        //   This case tests that the fallback handler is called if reporting
        //   an assertion itself asserts.
        //
        // Concerns:
        //: 1 If the installed reporting callback itself triggers an assertion,
        //:   we expect the fallback handler to be invoked (and not have the
        //:   tracking subsystem die a recursive death).
        //
        // Plans:
        //: 2 Install a handler that has the tracker as a bound argument, and
        //:   have it invoke the assertion handler of the tracker.  Verify that
        //:   the fallback handler is called.
        //
        // Testing:
        //   RECURSION
        // --------------------------------------------------------------------
        if (verbose) cout << "\nRECURSION TEST"
                             "\n==============\n";

        Obj Z(defaultHandler, Obj::preserveConfiguration, &ta);
        Z.setReportingCallback(bdlf::BindUtil::bind(
                               assertingReporter, &Z, _1, _2, _3, _4, _5, _6));
        handledAssertionCount = 0;
        defaultAssertionCount = 0;
        Z.assertionDetected("recursion", __FILE__, __LINE__);
        ASSERTV(handledAssertionCount, 0 == handledAssertionCount);
        ASSERTV(defaultAssertionCount, 1 == defaultAssertionCount);
      } break;
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
        //   void reportAllRecordedStackTraces() const;
        // --------------------------------------------------------------------
        if (verbose) cout << "\nREPORTER TEST"
                             "\n=============\n";

        bsl::ostringstream os;

        Obj Z(bsls::Assert::failureHandler(), Obj::preserveConfiguration, &ta);

        Z.setReportingCallback(bdlf::BindUtil::bind(
                          &Obj::reportAssertion, &os, _1, _2, _3, _4, _5, _6));

        for (int i = 0; i < 10; ++i) {
            if (veryVerbose) {
                P_(i) Q("assert 1")
            }
            Z.assertionDetected("0 && \"assert 1\"", __FILE__, __LINE__);
            for (int j = 0; j < 10; ++j) {
                if (veryVerbose) {
                    P_(i) P_(j) Q("assert 2")
                }
                Z.assertionDetected("0 && \"assert 2\"", __FILE__, __LINE__);
            }
        }

        bsl::string s = os.str();

        ASSERTV(s, s.npos != s.find(__FILE__));
        ASSERTV(s, s.npos != s.find(":1:0 && \"assert 1\":"));
        ASSERTV(s, s.npos != s.find(":1:0 && \"assert 2\":"));

        os.str("");
        Z.reportAllRecordedStackTraces();
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
        //   void assertionDetected(const char *, const char *, int);
        //   void setReportingCallback(ReportingCallback cb);
        //   void setMaxAssertions(int value);
        //   void setMaxLocations(int value);
        //   void setMaxStackTracesPerLocation(int value);
        //   void setReportingFrequency(ReportingFrequency value);
        //   void setReportingSeverity(bsls::LogSeverity::Enum value);
        //   ReportingCallback reportingCallback() const;
        //   int maxAssertions() const;
        //   int maxLocations() const;
        //   int maxStackTracesPerLocation() const;
        //   ReportingFrequency reportingFrequency() const;
        //   bsls::LogSeverity::Enum reportingSeverity() const;
        //   void preserveConfiguration(int *, int *, int *, int *, int *);
        //   ConfigurationCallback configurationCallback();
        //   void setConfigurationCallback(ConfigurationCallback);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nINVOCATION TEST"
                             "\n===============\n";
        {
            static const struct {
                int d_line;                       // line number
                int d_severity;                   // reporting severity
                int d_frequency;                  // reporting frequency
                int d_maxAssertions;              // max assertions handled
                int d_maxLocations;               // max locations handled
                int d_maxStackTracesPerLocation;  // max stacks handled
                int d_assertion_location[3][3];   // assertions to trigger1
                int d_expected_handled;
                int d_expected_default;
            } DATA[] = {
                {L_, 1, 2, -1, -1, -1, {1, 1, 1, 1, 1, 1, 1, 1, 1},  9,  0},
                {L_, 1, 1, -1, -1, -1, {2, 2, 2, 2, 2, 2, 2, 2, 2},  9,  0},
                {L_, 1, 0, -1, -1, -1, {2, 2, 2, 2, 2, 2, 2, 2, 2},  3,  0},
                {L_, 1, 2,  0,  0,  0, {1, 1, 1, 1, 1, 1, 1, 1, 1},  0,  9},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE      = DATA[ti].d_line;
                const int  SEVERITY  = DATA[ti].d_severity;
                const int  FREQ      = DATA[ti].d_frequency;
                const int  MAX_A     = DATA[ti].d_maxAssertions;
                const int  MAX_L     = DATA[ti].d_maxLocations;
                const int  MAX_S     = DATA[ti].d_maxStackTracesPerLocation;
                const int  EXP_H     = DATA[ti].d_expected_handled;
                const int  EXP_D     = DATA[ti].d_expected_default;
                const int(&AL)[3][3] = DATA[ti].d_assertion_location;

                if (veryVerbose) {
                    const char *const when[] = { "STACK ", "LOC ", "EACH " };
                    cout << LINE << " "
                         << "SEV " << SEVERITY << " "
                         << when[FREQ]
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

                Obj Z(defaultHandler, localConfiguration, &ta);
                ASSERT(&ta == Z.allocator());
                localConfigurationCalled = false;
                int a = 1, b = 1, c = 1, d = 1, e = 1;
                Z.configurationCallback()(&a, &b, &c, &d, &e);
                ASSERT(localConfigurationCalled);
                ASSERT(a == 1 && b == 1 && c == 1 && d == 1 && e == 1);
                Z.setConfigurationCallback(Obj::preserveConfiguration);
                localConfigurationCalled = false;
                Z.configurationCallback()(&a, &b, &c, &d, &e);
                ASSERT(!localConfigurationCalled);
                ASSERT(a == 1 && b == 1 && c == 1 && d == 1 && e == 1);
                Z.setReportingCallback(reporter);
                handledAssertionCount = 0;
                defaultAssertionCount = 0;

                Z.reportingCallback()(0, 0, 0, 0, 0, bsl::vector<void *>());
                ASSERT(1 == handledAssertionCount);
                handledAssertionCount = 0;

                Z.setReportingSeverity(bsls::LogSeverity::Enum(SEVERITY));
                ASSERT(SEVERITY == Z.reportingSeverity());
                Z.setReportingFrequency(Obj::ReportingFrequency(FREQ));
                ASSERT(FREQ == Z.reportingFrequency());
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

                if (veryVeryVerbose) {
                    Z.setReportingCallback(
                        bdlf::BindUtil::bind(&Obj::reportAssertion,
                                             (bsl::ostream *)0,
                                             _1,
                                             _2,
                                             _3,
                                             _4,
                                             _5,
                                             _6));
                    Z.reportAllRecordedStackTraces();
                }
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

        Obj Z(bsls::Assert::failureHandler(), Obj::preserveConfiguration, &ta);
        for (int i = 0; i < 10; ++i) {
            if (veryVerbose) {
                P_(i) Q("assertion 1")
            }
            Z.assertionDetected("assertion 1", __FILE__, __LINE__);
            for (int j = 0; j < 10; ++j) {
                if (veryVerbose) {
                    P_(i) P_(j) Q("assertion 2")
                }
                Z.assertionDetected("assertion 2", __FILE__, __LINE__);
            }
        }
        Z.reportAllRecordedStackTraces();
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
