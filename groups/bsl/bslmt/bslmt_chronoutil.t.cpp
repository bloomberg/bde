// bslmt_chronoutil.t.cpp                                             -*-C++-*-

#include <bslmt_chronoutil.h>

#include <bslim_testutil.h>

#include <bsl_iostream.h>

#include <stdlib.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
// The component under test provides a utility for using 'bsl::chrono' clocks
// to specify timeouts for calls to 'timedWait' on synchronization primitives.
// We will test this call with several different dummy synchronization
// primitives: one that always succeeds ('TimedWaitSuccess'), one that always
// fails ('TimedWaitFailure'), and one that always times out
// ('TimedWaitTimeout').
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [3] bsls::TimeInterval ChronoUtil::durationToTimeInterval(duration);
// [1] bool isMatchingClock<CLOCK>(bsls::SystemClockType::Enum cT);
// [2] int timedWait(PRIMITIVE *, const time_point&);
// [2] int timedWait(PRIMITIVE *, ARG_TYPE *, const time_point&);
// [4] USAGE EXAMPLE

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
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -MN03

            // ==================
            // class AnotherClock
            // ==================

class AnotherClock {
    // 'AnotherClock' is a C++11-compatible clock that is very similar to
    // 'bsl::chrono::steady_clock'.  The only difference is that it uses a
    // different epoch; it begins 10000 "ticks" after the beginning of
    // 'steady_clock's epoch.

  private:
    typedef bsl::chrono::steady_clock base_clock;

  public:
    typedef base_clock::duration                  duration;
    typedef base_clock::rep                       rep;
    typedef base_clock::period                    period;
    typedef bsl::chrono::time_point<AnotherClock> time_point;

    static const bool is_steady = base_clock::is_steady;

    // CLASS METHODS
    static time_point now();
        // Return a time point representing the time since the beginning of the
        // epoch.
};

// CLASS METHODS
AnotherClock::time_point AnotherClock::now()
{
    base_clock::duration ret = base_clock::now().time_since_epoch();
    return AnotherClock::time_point(ret - duration(10000));
}

            // ===============
            // class HalfClock
            // ===============

class HalfClock {
    // 'HalfClock' is a C++11-compatible clock that is very similar to
    // 'bsl::chrono::steady_clock'.  The difference is that it runs "half as
    // fast" as 'steady_clock'.

  private:
    typedef bsl::chrono::steady_clock base_clock;

  public:
    typedef base_clock::duration               duration;
    typedef base_clock::rep                    rep;
    typedef base_clock::period                 period;
    typedef bsl::chrono::time_point<HalfClock> time_point;

    static const bool is_steady = base_clock::is_steady;

    // CLASS METHODS
    static time_point now();
        // Return a time point representing the time since the beginning of the
        // epoch.
};

// CLASS METHODS
HalfClock::time_point HalfClock::now()
{
    base_clock::duration ret = base_clock::now().time_since_epoch();
    return HalfClock::time_point(ret/2);
}


            // ======================
            // class TimedWaitSuccess
            // ======================

///Prologue: Create a Synchronization Primitive
/// - - - - - - - - - - - - - - - - - - - - - -
// First, we define the interface of 'TimedWaitSuccess':
//..
class TimedWaitSuccess {
    // 'TimedWaitSuccess' is a synchronization primitive that always succeeds.

  private:
    // DATA
    bsls::SystemClockType::Enum d_clockType;

  public:
    // TYPES
    enum { e_TIMED_OUT = 1 };

    // CREATORS
    explicit
    TimedWaitSuccess(bsls::SystemClockType::Enum clockType
                                      = bsls::SystemClockType::e_REALTIME);
        // Create a 'TimedWaitSuccess' object.  Optionally specify a
        // 'clockType' indicating the type of the system clock against which
        // the 'bsls::TimeInterval' 'absTime' timeouts passed to the
        // 'timedWait' method are to be interpreted.  If 'clockType' is not
        // specified then the realtime system clock is used.

    // MANIPULATORS
    int timedWait(const bsls::TimeInterval&);
        // Return 0 immediately.  Note that this is for demonstration and
        // testing purposes only.

    // ACCESSORS
    bsls::SystemClockType::Enum clockType() const;
        // Return the clock type used for timeouts.
};
//..
// Then, we implement the creator.  All it has to do is remember the
// 'clockType' that was passed to it:
//..
inline
TimedWaitSuccess::TimedWaitSuccess(bsls::SystemClockType::Enum clockType)
: d_clockType(clockType)
{
}

//..
// Next, we implement the 'timedWait' function.  In this simplistic primitive,
// this function always succeeds:
//..
// MANIPULATORS
inline
int TimedWaitSuccess::timedWait(const bsls::TimeInterval&)
{
    return 0;
}

//..
// Next, we implement the 'clockType' function, which returns the underlying
// 'bsls::SystemClockType::Enum' that this primitive uses:
//..
// ACCESSORS
inline
bsls::SystemClockType::Enum TimedWaitSuccess::clockType() const
{
    return d_clockType;
}
//..


            // ==============================
            // class TimedWaitSuccessExtraArg
            // ==============================

class TimedWaitSuccessExtraArg {
    // 'TimedWaitSuccessExtraArg' is a synchronization primitive that always
    // succeeds.  It differs from 'TimedWaitSuccessExtraArg' in that the
    // 'timedWait' method takes an extra pointer argument.

  private:
    // DATA
    bsls::SystemClockType::Enum d_clockType;

  public:
    // TYPES
    enum { e_TIMED_OUT = 1 };

    // CREATORS
    explicit
    TimedWaitSuccessExtraArg(bsls::SystemClockType::Enum clockType
                                          = bsls::SystemClockType::e_REALTIME);
        // Create a 'TimedWaitSuccessExtraArg' object.  Optionally specify a
        // 'clockType' indicating the type of the system clock against which
        // the 'bsls::TimeInterval' 'absTime' timeouts passed to the
        // 'timedWait' method are to be interpreted.  If 'clockType' is not
        // specified then the realtime system clock is used.

    // MANIPULATORS
    int timedWait(void *, const bsls::TimeInterval&);
        // Return 0 immediately.

    // ACCESSORS
    bsls::SystemClockType::Enum clockType() const;
        // Return the clock type used for timeouts.
};

// CREATORS
TimedWaitSuccessExtraArg::TimedWaitSuccessExtraArg(
                                         bsls::SystemClockType::Enum clockType)
: d_clockType(clockType)
{
}

// MANIPULATORS
int TimedWaitSuccessExtraArg::timedWait(void *, const bsls::TimeInterval&)
{
    return 0;
}

// ACCESSORS
bsls::SystemClockType::Enum TimedWaitSuccessExtraArg::clockType() const
{
    return d_clockType;
}

            // ======================
            // class TimedWaitFailure
            // ======================

class TimedWaitFailure {
    // 'TimedWaitFailure' is synchronization primitive that always fails.

  private:
    bsls::SystemClockType::Enum d_clockType;

  public:
    enum { e_TIMED_OUT = 1 };

    explicit
    TimedWaitFailure(bsls::SystemClockType::Enum clockType
                                          = bsls::SystemClockType::e_REALTIME);
        // Create a 'TimedWaitFailure' object.  Optionally specify a
        // 'clockType' indicating the type of the system clock against which
        // the 'bsls::TimeInterval' 'absTime' timeouts passed to the
        // 'timedWait' method are to be interpreted.  If 'clockType' is not
        // specified then the realtime system clock is used.

    // MANIPULATORS
    int timedWait(const bsls::TimeInterval&);
        // Return -1 immediately.

    int timedWait(void *, const bsls::TimeInterval&);
        // Return -2 immediately.

    // ACCESSORS
    bsls::SystemClockType::Enum clockType() const;
        // Return the clock type used for timeouts.
};

TimedWaitFailure::TimedWaitFailure(bsls::SystemClockType::Enum clockType)
: d_clockType(clockType)
{
}

// MANIPULATORS
int TimedWaitFailure::timedWait(const bsls::TimeInterval&)
{
    return -1;
}

int TimedWaitFailure::timedWait(void *, const bsls::TimeInterval&)
{
    return -2;
}

// ACCESSORS
bsls::SystemClockType::Enum TimedWaitFailure::clockType() const
{
    return d_clockType;
}

            // ======================
            // class TimedWaitTimeout
            // ======================

class TimedWaitTimeout {
    // 'TimedWaitTimeout' is synchronization primitive that always times out.
    // Calls to 'timedWait' always sleep for 250ms and then return
    // 'e_TIMED_OUT'. It also has an accessor 'numCalls' which records how many
    // times 'timedWait' has been called on this object.

  private:
    bsls::SystemClockType::Enum d_clockType;
    int                         d_numCalls;

  public:
    enum { e_TIMED_OUT = 1 };

    explicit
    TimedWaitTimeout(bsls::SystemClockType::Enum clockType
                                          = bsls::SystemClockType::e_REALTIME);
        // Create a 'TimedWaitTimeout' object.  Optionally specify a
        // 'clockType' indicating the type of the system clock against which
        // the 'bsls::TimeInterval' 'absTime' timeouts passed to the
        // 'timedWait' method are to be interpreted.  If 'clockType' is not
        // specified then the realtime system clock is used.

    // MANIPULATORS
    int timedWait(const bsls::TimeInterval&);
        // Return 'e_TIMED_OUT' after 1/4 second.

    int timedWait(void *, const bsls::TimeInterval&);
        // Return 'e_TIMED_OUT' after 1/4 second.

    // ACCESSORS
    bsls::SystemClockType::Enum clockType() const;
        // Return the clock type used for timeouts.

    int numCalls() const;
        // Return the number of times that 'timedWait' has been called.
};

TimedWaitTimeout::TimedWaitTimeout(bsls::SystemClockType::Enum clockType)
: d_clockType(clockType)
, d_numCalls(0)
{
}

// MANIPULATORS
int TimedWaitTimeout::timedWait(const bsls::TimeInterval&)
{
    ++d_numCalls;
    return e_TIMED_OUT;
}

int TimedWaitTimeout::timedWait(void *, const bsls::TimeInterval&)
{
    ++d_numCalls;
    return e_TIMED_OUT;
}

// ACCESSORS
bsls::SystemClockType::Enum TimedWaitTimeout::clockType() const
{
    return d_clockType;
}

int TimedWaitTimeout::numCalls() const
{
    return d_numCalls;
}

// BDE_VERIFY pragma: pop
#endif


// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
// This example demonstrates use of 'bslmt::ChronoUtil::timedWait' to block on
// a synchronization primitive until either a condition is satisfied or a
// specified amount of time has elapsed.  We use a 'bsl::chrono::time_point' to
// specify the amount of time to wait.  To do this, we call
// 'bslmt::ChronoUtil::timedWait', passing in the timeout as an *absolute* time
// point.  In this example, we're using 'TimedWaitSuccess' as the
// synchronization primitive, and specifying the timeout using
// 'bsl::chrono::steady_clock'.
//
// First, we construct the 'TimedWaitSuccess' primitive; by default it uses the
// 'bsls' realtime system clock to measure time:
//..
        TimedWaitSuccess aPrimitive;
//..
// Then, we call 'bslmt::ChronoUtil::timedWait' to block on 'aPrimitive', while
// passing a timeout of "10 seconds from now", measured on the
// 'bsl::chrono::steady_clock':
//..
        int rc = bslmt::ChronoUtil::timedWait(
                 &aPrimitive,
                 bsl::chrono::steady_clock::now() + bsl::chrono::seconds(10));
//..
// When this call returns, one of three things will be true: (a) 'rc == 0',
// which means that the call succeeded before the timeout expired, (b)
// 'rc == TimedWaitSuccess::e_TIMED_OUT', which means that the call did not
// succeed before the timeout expired, or (c) rc equals some other value, which
// means that an error occurred.
//
// If the call to 'bslmt::ChronoUtil::timedWait' returned 'e_TIMED_OUT' then we
// are guaranteed that the current time *on the clock that the time point was
// defined on* is greater than the timeout value that was passed in.

        ASSERT(0 == rc);
#endif

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'durationToTimeInterval'
        //   Ensure the function converts values correctly.
        //
        // Concerns:
        //: 1 The template converts both integral-based and floating-point
        //:   based durations correctly.
        //
        // Plan:
        //: 1 Convert both integral and floating-point-based durations, and
        //:   check the results.  (C-1)
        //
        // Testing:
        //   bsls::TimeInterval ChronoUtil::durationToTimeInterval(duration);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'durationToTimeInterval'" << endl
                 << "================================" << endl;
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        using namespace bsl::chrono;

        // implicit and explicit conversions give the same answer
        ASSERT(bsls::TimeInterval(seconds(2)) ==
                    bslmt::ChronoUtil::durationToTimeInterval(seconds(2)));

        bsls::TimeInterval fp1 = bslmt::ChronoUtil::durationToTimeInterval(
                                                        duration<double>(4.0));
        bsls::TimeInterval fp2 = bslmt::ChronoUtil::durationToTimeInterval(
                                                        duration<double>(6.5));
        bsls::TimeInterval fp3 = bslmt::ChronoUtil::durationToTimeInterval(
                                                      duration<double>(8.275));
        bsls::TimeInterval fp4 = bslmt::ChronoUtil::durationToTimeInterval(
                                                       duration<double>(1.67));

        ASSERT(4 == fp1.seconds());
        ASSERT(0 == fp1.nanoseconds());
        ASSERT(6 == fp2.seconds());
        ASSERT(500000000 == fp2.nanoseconds());
        ASSERT(8 == fp3.seconds());
        ASSERT(274999999 <= fp3.nanoseconds());
        ASSERT(275000001 >= fp3.nanoseconds());
        ASSERT(1 == fp4.seconds());
        ASSERT(669999999 <= fp4.nanoseconds());
        ASSERT(670000001 >= fp4.nanoseconds());


        bsls::TimeInterval fp5 = bslmt::ChronoUtil::durationToTimeInterval(
                                                        duration<double>(4.0));
        bsls::TimeInterval fp6 = bslmt::ChronoUtil::durationToTimeInterval(
                                                        seconds(4));
        ASSERT(fp5 == fp6);

#endif

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'timedWait'
        //   Ensure the function calls the primitive as expected.
        //
        // Concerns:
        //: 1 The template forwards to the synchronization primitive correctly.
        //
        // Plan:
        //: 1 Directly verify the result of 'timedWait' throughout a sequence
        //:   of operations on the synchronization primitive.  (C-1)
        //
        // Testing:
        //   int timedWait(PRIMITIVE *, const time_point&);
        //   int timedWait(PRIMITIVE *, ARG_TYPE *, const time_point&);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'timedWait'" << endl
                 << "===================" << endl;
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        using namespace bsl::chrono;

        void *param = 0;

        TimedWaitSuccess  success;
        ASSERT(0 ==
                  bslmt::ChronoUtil::timedWait(&success, system_clock::now()));

        TimedWaitSuccessExtraArg  extraArg;
        ASSERT(0 ==
          bslmt::ChronoUtil::timedWait(&extraArg, param, system_clock::now()));

        TimedWaitFailure failure;
        ASSERT(-1 ==
                  bslmt::ChronoUtil::timedWait(&failure, system_clock::now()));
        ASSERT(-2 ==
           bslmt::ChronoUtil::timedWait(&failure, param, system_clock::now()));

        TimedWaitTimeout timeout1;
        TimedWaitTimeout timeout2;
        // Clocks match
        ASSERT(TimedWaitTimeout::e_TIMED_OUT ==
               bslmt::ChronoUtil::timedWait(&timeout1,
                                            system_clock::now() + seconds(1)));
        ASSERT(1 == timeout1.numCalls());
        ASSERT(TimedWaitTimeout::e_TIMED_OUT ==
               bslmt::ChronoUtil::timedWait(&timeout2, param,
                                            system_clock::now() + seconds(1)));
        ASSERT(1 == timeout2.numCalls());

        // Clocks don't match
        ASSERT(TimedWaitTimeout::e_TIMED_OUT ==
               bslmt::ChronoUtil::timedWait(&timeout1,
                                            steady_clock::now() + seconds(1)));
        ASSERT(2 < timeout1.numCalls());

        ASSERT(TimedWaitTimeout::e_TIMED_OUT ==
               bslmt::ChronoUtil::timedWait(&timeout2, param,
                                            steady_clock::now() + seconds(1)));
        ASSERT(2 < timeout2.numCalls());
#endif

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'isMatchingClockType'
        //   Ensure the accessor forwards as expected.
        //
        // Concerns:
        //: 1 The method correctly matches the (template parameter) 'CLOCK' to
        //:   the specified 'bsls::SystemClockType::Enum' value, for standard
        //:   and user-provided clocks.
        //
        // Plan:
        //: 1 Directly verify the result of 'isMatchingClock' with all standard
        //:   clocks and 'bsls::SystemClockType::Enum' values, and the test
        //:   clocks 'HalfClock' and 'AnotherClock'.  (C-1)
        //
        // Testing:
        //   bool isMatchingClock<CLOCK>(bsls::SystemClockType::Enum cT);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'isMatchingClockType'" << endl
                 << "=============================" << endl;
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        using namespace bsl::chrono;

        ASSERT( bslmt::ChronoUtil::isMatchingClock<system_clock>(
                                           bsls::SystemClockType::e_REALTIME));
        ASSERT(!bslmt::ChronoUtil::isMatchingClock<system_clock>(
                                          bsls::SystemClockType::e_MONOTONIC));

        ASSERT(!bslmt::ChronoUtil::isMatchingClock<steady_clock>(
                                           bsls::SystemClockType::e_REALTIME));
        ASSERT( bslmt::ChronoUtil::isMatchingClock<steady_clock>(
                                          bsls::SystemClockType::e_MONOTONIC));

        ASSERT(!bslmt::ChronoUtil::isMatchingClock<HalfClock>(
                                           bsls::SystemClockType::e_REALTIME));
        ASSERT(!bslmt::ChronoUtil::isMatchingClock<HalfClock>(
                                          bsls::SystemClockType::e_MONOTONIC));

        ASSERT(!bslmt::ChronoUtil::isMatchingClock<AnotherClock>(
                                           bsls::SystemClockType::e_REALTIME));
        ASSERT(!bslmt::ChronoUtil::isMatchingClock<AnotherClock>(
                                          bsls::SystemClockType::e_MONOTONIC));
#endif
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
// Copyright 2021 Bloomberg Finance L.P.
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
