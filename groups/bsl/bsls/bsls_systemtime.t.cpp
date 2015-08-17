// bsls_systemtime.t.cpp                                              -*-C++-*-

#include <bsls_systemtime.h>

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#include <windows.h>
#else
#include <unistd.h>
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1600
// stdint.h is only available starting is VS2010.
typedef unsigned long long int64_t;
#else
#include <stdint.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace BloombergLP;
using namespace std;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// 'bsls_systemtime' defines 'static' functions for obtaining the current clock
// time.  The operations for obtaining the real-time clock can be tested
// against the C library function 'time'.  Operations for obtaining the
// monotonic clock simply test that the monotonic clock is a uniformly
// increasing value whose relative difference between calls is similar to the
// relative difference between calls to the C library function 'time'.  A
// couple negative test cases are defined to allow stress testing of
// monotonicity in ways that are not compatible with automated testing.
//-----------------------------------------------------------------------------
// [ 3] TimeInterval now(SystemClockType::Enum);
// [ 2] TimeInterval nowMonotonicClock();
// [ 1] TimeInterval nowRealtimeClock();
//-----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
// [-1] CONCERN: STRESS TEST FOR MONOTONICITY
// [-2] CONCERN: MONOTONICITY UNDER SYSTEM CLOCK CHANGES

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
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bsls::SystemClockType::Enum ClockType;
typedef bsls::SystemTime            Obj;
typedef bsls::TimeInterval          TimeInterval;
typedef bsls::SystemClockType       SystemClockType;

// ============================================================================
//                      HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace bsls {

void debugprint(const bsls::TimeInterval& timeInterval)
    // Print the specified 'timeInterval' to the console.  Note that this free
    // function overload works in coordination with 'bsls_bsltestutil'.
{
    debugprint(timeInterval.totalSecondsAsDouble());
}

}  // close package namespace
}  // close enterprise namespace

#if defined(BSLS_PLATFORM_OS_WINDOWS)
void sleep(unsigned int seconds)
{
    Sleep(seconds * 1000);
}
#endif

// ============================================================================
//                          MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void)     veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

///Example 1: Getting Current Wall Clock Time
/// - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use this utility component
// to obtain the system time by calling 'now' and 'nowRealtimeClock'.
//
// First, we call 'nowRealtimeClock', and set 't1', to the current time
// according to the real-time clock:
//..
    bsls::TimeInterval t1 = bsls::SystemTime::nowRealtimeClock();
//
    ASSERT(bsls::TimeInterval() != t1);
//..
// Next, we sleep for 1 second:
//..
    sleep(1);
//..
// Now, we call 'now', and supply 'e_REALTIME' to indicate a real-time clock
// value should be returned, and then set 't2' to the current time according
// to the real-time clock:
//..
    bsls::TimeInterval t2 = bsls::SystemTime::now(
                                            bsls::SystemClockType::e_REALTIME);
//
    ASSERT(bsls::TimeInterval() != t2);
//..
// Finally, we verify the interval between 't1' and 't2' is close to 1 second:
//..
    bsls::TimeInterval interval = t2 - t1;
//
    ASSERT(bsls::TimeInterval(.9) <= interval &&
                                     interval <= bsls::TimeInterval(1.1));
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CLASS METHODS: 'now(SystemClockType::Enum)'
        //  Ensure the returned 'TimeInterval' value represents the current
        //  system time as per the specified 'SystemClockType::Enum'.
        //
        // Concerns:
        //: 1 'now(e_REALTIME)' provides the current "wall" time.
        //:
        //: 2 'now(e_MONOTONIC)' provides the current monotonic clock time.
        //
        // Plan:
        //: 1 Verify 'now(e_REALTIME)' closely approximates the value returned
        //:   by 'nowRealtimeClock'.  (C-1)
        //:
        //: 2 Verify 'now(e_MONOTONIC)' closely approximates the value returned
        //:   by 'nowMonotonicClock'.  (C-2)
        //
        // Testing:
        //  TimeInterval now(SystemClockType::Enum);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: 'now(SystemClockType::Enum)'"
                            "\n===========================================\n");

        if (veryVerbose) printf("\tCompare results w/ monotonic clock\n");
        {
            TimeInterval first  = Obj::nowMonotonicClock();
            TimeInterval second = Obj::now(SystemClockType::e_MONOTONIC);
            ASSERT(second - first  < TimeInterval(1, 0));
        }

        if (veryVerbose) printf("\tCompare results w/ real-time clock\n");
        {
            TimeInterval first  = Obj::nowRealtimeClock();
            TimeInterval second = Obj::now(SystemClockType::e_REALTIME);
            ASSERT(second - first  < TimeInterval(1, 0));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CLASS METHODS: 'nowMonotonicClock'
        //
        // Concerns:
        //: 1 Consecutive calls to 'nowMonotonicClock' measure time intervals
        //:   that match those measured by calls to the (previously tested)
        //:   'nowRealtimeClock'.
        //:
        //: 2 That consecutive values do not decrease.
        //:
        //: 3 QoI: The resolution of the monotonic clock is < 1 second.
        //
        // Plan:
        //: 1 Call 'nowMonotonicClock' in a loop for a couple seconds; verify
        //:   the results do not decrease between iterations, and that
        //:   increments of the clock are less than 1 second.  (C-1..3)
        //
        // Testing:
        //   TimeInterval nowMonotonicClock();
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: 'nowMonotonicClock'"
                            "\n==================================\n");

        if (veryVerbose) printf("\tCompare results w/ real-time clock\n");
        {
            // Test sequential values are increasing and have a resolution of
            // less than 1 second.

            TimeInterval ONE_SEC = TimeInterval(1, 0);
            TimeInterval realOrigin = Obj::nowRealtimeClock();
            TimeInterval monoOrigin = Obj::nowMonotonicClock();
            TimeInterval prev       = monoOrigin;
            TimeInterval now        = monoOrigin;

            while (TimeInterval(1.5) > now - monoOrigin) {
                if (veryVerbose) {
                    P_(prev); P(now);
                }

                now = Obj::nowMonotonicClock();

                ASSERT(prev <= now);
                ASSERT(prev + ONE_SEC > now);

                prev = now;
            }

            TimeInterval realInterval = Obj::nowRealtimeClock() - realOrigin;
            TimeInterval monoInterval = now - monoOrigin;
            TimeInterval delta        = monoInterval - realInterval;

            const TimeInterval TOLERANCE = TimeInterval(.1);

            ASSERT(-TOLERANCE <= delta && delta <= TOLERANCE);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // CLASS METHODS: 'nowRealtimeClock'
        //
        // Concerns:
        //: 1 'nowRealtimeClock' returns values that are intervals from the
        //:   Unix epoch.
        //:
        //: 2 'nowRealtimeClock' returns time values that are consistent with
        //:   the current wall clock time.
        //:
        //: 3 QoI: That consecutive values do not decrease (under normal
        //:   conditions).
        //:
        //: 4 QoI: The resolution of the real-time clock is < 1 second.
        //
        // Plan:
        //: 1 Call 'nowRealtimeClock' and verify the value returned, when
        //:   treated as an interval from the Unix epoch, corresponds to a
        //:   possible wall clock time.  (C-1)
        //:
        //: 2 Call 'nowRealtimeClock' and compare the value returned to an
        //:   oracle clock, i.e., the standard library function 'time'.  (C-2)
        //:
        //: 3 Call 'nowRealtimeClock' in a loop for a couple seconds; verify
        //:   the results do not decrease between iterations, and that
        //:   increments of the clock are less than 1 second.  (C-3..4)
        //
        // Testing:
        //   TimeInterval nowRealtimeClock();
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: 'nowRealtimeClock'"
                            "\n=================================\n");

        if (veryVerbose) printf("\tTest result is relative to Unix epoch\n");
        {
            const int64_t SEPT_27_2014         = 1411833584;
            const int64_t HUNDRED_YEARS_APPROX = 60ull * 60 * 24 * 365 * 100;

            TimeInterval t = Obj::nowRealtimeClock();

            ASSERT(SEPT_27_2014                        <= t.seconds());
            ASSERT(SEPT_27_2014 + HUNDRED_YEARS_APPROX >= t.seconds());
        }

        if (veryVerbose) printf("\tCompare results to 'time'\n");
        {
            time_t       timeValue       = time(0);
            TimeInterval systemTimeValue = Obj::nowRealtimeClock();

            ASSERT(timeValue - 1 <= systemTimeValue.seconds());
            ASSERT(timeValue + 1 >= systemTimeValue.seconds());
        }

        if (veryVerbose) printf("\tVerify sequential values'\n");
        {
            // Test sequential values are increasing and have a resolution of
            // less than 1 second.

            TimeInterval ONE_SEC = TimeInterval(1, 0);
            TimeInterval origin  = Obj::nowRealtimeClock();
            TimeInterval prev    = origin;
            TimeInterval now     = origin;

            while (TimeInterval(1.5) > now - origin) {
                if (veryVerbose) {
                    P_(prev); P(now);
                }

                now = Obj::nowRealtimeClock();

                ASSERT(prev <= now);
                ASSERT(prev + ONE_SEC > now);

                prev = now;
            }
        }
    } break;
    case -1: {
        // --------------------------------------------------------------------
        // CONCERN: STRESS TEST FOR MONOTONICITY
        //  Verify that each subsequent call to 'now' reports a time that is
        //  non-decreasing.
        //
        // Plan:
        //  Exercise the method in a loop a large, configurable number of
        //  times, verifying in each iteration that the system time is
        //  non-decreasing for all support clock types.
        //
        // Testing:
        //  CONCERN: STRESS TEST FOR MONOTONICITY
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nCONCERN: STRESS TEST FOR MONOTONICITY"
                   "\n=====================================\n");

        SystemClockType::Enum TYPES[] = {
            SystemClockType::e_REALTIME,
            SystemClockType::e_MONOTONIC
        };
        const int NUM_TYPES = sizeof TYPES / sizeof *TYPES;

        for (int type = 0; type < NUM_TYPES; ++type) {
            SystemClockType::Enum TYPE = TYPES[type];

            if (veryVerbose) {
                printf("\nType: %s\n", SystemClockType::toAscii(TYPE));
            }

            TimeInterval TOLERANCE(0.001); // 1ms

            enum {
                NUM_ITERATIONS         = 120,
                NUM_TEST_PER_ITERATION = 1000,
                OUTPUT_WIDTH           = 60,
                OUTPUT_FREQ = NUM_ITERATIONS / OUTPUT_WIDTH
            };

            int iterations = verbose ? atoi(argv[2]) : NUM_ITERATIONS;
            int output_freq = iterations < OUTPUT_WIDTH ? 1
                            : iterations / OUTPUT_WIDTH;
            int testsPerIteration = NUM_TEST_PER_ITERATION;

            if (veryVerbose) {
                testsPerIteration = atoi(argv[3]);
                const char P1[] = "0%";
                const char P2[] = "50%";
                const char P3[] = "100%";
                int hl = OUTPUT_WIDTH / 2;
                printf("%s", P1);

                for (unsigned i=0; i < hl - sizeof(P1) - sizeof(P2) + 4; ++i) {
                    printf("-");
                }
                printf("%s", P2);
                for (unsigned i = 0; i < hl - sizeof(P3); ++i) {
                    printf("-");
                }
                printf("%s\n", P3);
            }
            for (int i = 0; i < iterations; ++i) {
                TimeInterval prev = Obj::now(TYPE);
                for (int j = 0; j < testsPerIteration; ++j) {
                    TimeInterval now = Obj::now(TYPE);
                    if (prev > now) {
                        printf("*** Warning: system time is not "
                               "reliably monotonic on this platform\n."
                               "*** Allowing a tolerance of 1ms "
                               "in test driver.\n");
                        ASSERTV(i, j, prev, now, prev - TOLERANCE <= now);
                    }
                    ASSERTV(i, j, prev, now, prev <= now);
                    prev = now;
                }
                if (veryVerbose && 0 == i % output_freq) {
                    if (0 == i % (OUTPUT_WIDTH / 4 * output_freq)) {
                        printf("|");
                        fflush(stdout);
                    }
                    else {
                        printf("+");
                        fflush(stdout);
                    }
                }
            }
            if (veryVerbose) printf("\n");
        }
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // CONCERN: MONOTONICITY UNDER SYSTEM CLOCK CHANGES
        //  Changes to the system clock do not impact the monotonic clock.
        //
        // Plan:
        //  Record the current monotonoic and real-time clock times, pause the
        //  task to allow the current system time to be set to some time in
        //  the past, then capture the system times again.  Ensure that the
        //  second monotonic clock time is subsequent to the first, while the
        //  second real clock time is before the first.
        //
        // Testing:
        //  CONCERN: MONOTONICITY UNDER SYSTEM CLOCK CHANGES
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nCONCERN: MONOTONICITY UNDER SYSTEM CLOCK CHANGES"
                   "\n================================================\n");
        {
            TimeInterval t1Real      = Obj::nowRealtimeClock();
            TimeInterval t1Monotonic = Obj::nowMonotonicClock();

            int dummy;

            printf("Set the system clock back and then"
                   " enter a number to continue\n");
            scanf("%d", &dummy);

            TimeInterval t2Real      = Obj::nowRealtimeClock();
            TimeInterval t2Monotonic = Obj::nowMonotonicClock();

            ASSERTV(t1Real, t2Real, t2Real < t1Real);
            ASSERTV(t1Monotonic, t2Monotonic, t2Monotonic >= t2Monotonic);

            if (veryVerbose) {
                P(t2Real - t1Real);
                P(t2Monotonic - t1Monotonic);
            }
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
// Copyright 2014 Bloomberg Finance L.P.
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
