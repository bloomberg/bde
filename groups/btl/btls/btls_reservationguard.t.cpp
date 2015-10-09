// btls_reservationguard.t.cpp                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btls_reservationguard.h>

#include <btls_leakybucket.h>  // for testing only

#include <bslmt_threadutil.h>

#include <bslim_testutil.h>

#include <bdlt_currenttime.h>

#include <bsls_asserttest.h>
#include <bsls_timeinterval.h>

#include <bsl_climits.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a mechanism.
//
// Primary Manipulators:
//: o 'submitReserved'
//: o 'cancelReserved'
//
// Basic Accessors:
//: o 'ptr'
//: o 'unitsReserved'
//
// This class also provides a value constructor capable of creating an object
// having any parameters.
//
// Global Concerns:
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o ACCESSOR methods are 'const' thread-safe.
// ----------------------------------------------------------------------------
//
// CREATORS
//  [4] ~btls::ReservationGuard();
//  [3] btls::ReservationGuard(TYPE* reserve, bsls::Types::Uint64 numOfUnits);
//
// MANIPULATORS
//  [6] void cancelReserved(bsls::Types::Uint64 numOfUnits);
//  [5] void submitReserved(bsls::Types::Uint64 numOfUnits);
//
// ACCESSORS
//  [3] bsls::Types::Uint64 unitsReserved() const;
//  [3] TYPE *ptr() const;
// ----------------------------------------------------------------------------
// [1] BREATHING TEST
// [2] TEST APPARATUS
// [7] USAGE EXAMPLE
// [3] All accessor methods are declared 'const'.
// [*] All creator/manipulator ptr./ref. parameters are 'const'.
// ============================================================================

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
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

class my_Reserve {
    // This class provides a mock rate controllering mechanism which conforms
    // to the interface required by 'btls::ReservationGuard'.

    // DATA
    bsls::Types::Uint64 d_unitsSubmitted;
    bsls::Types::Uint64 d_unitsReserved;

public:
    // CREATORS
    my_Reserve();
        // Create a 'my_Reserve' object having zero 'unitsReserved' and
        // 'unitsSubmitted'

    // MANIPULATORS
    void reserve(bsls::Types::Uint64 numUnits);
        // Add the specified 'numUnits' to the 'unitsReserved' counter.

    void submitReserved(bsls::Types::Uint64 numUnits);
        // Subtract the specified 'numUnits' from 'unitsReserved' and add it to
        // 'unitsSubmitted', if 'numUnits' is less than 'unitsReserved'.
        // Otherwise, add 'unitsReserved' to 'unitsSubmitted' and set
        // 'unitsReserved' to 0.

    void cancelReserved(bsls::Types::Uint64 numUnits);
        // Subtract the specified 'numUnits' from 'unitsReserved' if 'numUnits'
        // is less than 'unitsReserved'; otherwise, set 'unitsReserved' to 0.

    void reset();
        // Reset this 'my_Reserve' object to its default-constructed state.

    // ACCESSORS
    bsls::Types::Uint64 unitsReserved() const;
        // Return the number of units reserved.

    bsls::Types::Uint64 unitsSubmitted() const;
        // Return the number of units submitted.
};

// CREATORS
inline
my_Reserve::my_Reserve()
: d_unitsSubmitted(0)
, d_unitsReserved(0)
{
}

// MANIPULATORS
inline
void my_Reserve::reserve(bsls::Types::Uint64 numUnits)
{
    d_unitsReserved += numUnits;
}

inline
void my_Reserve::submitReserved(bsls::Types::Uint64 numUnits)
{
    if (numUnits < d_unitsReserved) {
        d_unitsReserved -= numUnits;
    }
    else {
        d_unitsReserved = 0;
    }
    d_unitsSubmitted += numUnits;
}

inline
void my_Reserve::cancelReserved(bsls::Types::Uint64 numUnits)
{
    if (numUnits < d_unitsReserved) {
        d_unitsReserved -= numUnits;
    }
    else {
        d_unitsReserved = 0;
    }
}

inline
void my_Reserve::reset()
{
    d_unitsReserved  = 0;
    d_unitsSubmitted = 0;
}

// ACCESSORS
inline
bsls::Types::Uint64 my_Reserve::unitsReserved() const
{
    return d_unitsReserved;
}

inline
bsls::Types::Uint64 my_Reserve::unitsSubmitted() const
{
    return d_unitsSubmitted;
}

typedef btls::ReservationGuard<my_Reserve> Obj;
typedef bsls::TimeInterval                 Ti;
typedef bsls::Types::Uint64               Uint64;
typedef unsigned int                      uint;

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Guarding units reservation in operations with btls::LeakyBucket
///--------------------------------------------------------------------------
// Suppose that we are limiting the rate of network traffic generation using a
// 'btls::LeakyBucket' object.  We send data buffer over a network interface
// using the 'mySendData' function:
//..
    static bsls::Types::Uint64 mySendData(size_t dataSize)
        // Send a specified 'dataSize' amount of data over the network.  Return
        // the amount of data actually sent.  Throw an exception if a network
        // failure is detected.
    {
        // In our example we don`t deal with actual data sending, so we assume
        // that the function has sent certain amount of data (3/4 of the
        // requested amount) successfully.
        return (dataSize * 3) >> 2;
    }
//..
// Notice that the 'mySendData' function may throw an exception; therefore, we
// should wait until 'mySendData' returns before indicating the amount of data
// sent to the leaky bucket.

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int     test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

// Further suppose that multiple threads are sending network data and sharing
// the same leaky bucket.  If every thread simply checks for overflowing of the
// leaky bucket, send data, and then submit to the leaky bucket, then the rate
// of data usage may exceed the limits imposed by the leaky bucket due to race
// conditions.  We can avoid the this issue by reserving the amount of data
// immediately after checking whether the leaky bucket has overflown and submit
// the reserved amount after the data has been sent.  However, this process
// could lead to the loss of the reserved units (effectively decreasing the
// leaky bucket's capacity) if 'mySendData' throws an exception.
// 'btls::ReservationGuard' is designed to resolve this issue.
//
// First, we define the size of each data chunk and the total size of the data
// to send:
//..
    const unsigned int  CHUNK_SIZE = 256;
    bsls::Types::Uint64 bytesSent  = 0;
    bsls::Types::Uint64 totalSize  = 10 * 1024; // in bytes
//..
// Then, we create a 'btls::LeakyBucket' object to limit the rate of data
// transmission:
//..
    bsls::Types::Uint64 rate     = 512;
    bsls::Types::Uint64 capacity = 1536;
    bsls::TimeInterval  now      = bdlt::CurrentTime::now();
    btls::LeakyBucket   bucket(rate, capacity, now);
//..
// Next, we send the chunks of data using a loop.  For each iteration, we check
// whether submitting another byte would cause the leaky bucket to overflow:
//..
    while (bytesSent < totalSize) {
        now = bdlt::CurrentTime::now();
        if (!bucket.wouldOverflow(now)) {
//..
// Now, if the leaky bucket would not overflow, we create a
// 'btls::ReservationGuard' object to reserve the amount of data to be sent:
//..
            btls::ReservationGuard<btls::LeakyBucket> guard(&bucket,
                                                            CHUNK_SIZE);
//..
// Then, we use the 'mySendData' function to send the data chunk over the
// network.  After the data had been sent, we submit the amount of reserved
// data that was actually sent:
//..
            bsls::Types::Uint64 result;
            result = mySendData(CHUNK_SIZE);
            bytesSent += result;
            guard.submitReserved(result);
//..
// Note that we do not have manually cancel any remaining units reserved by the
// 'btls::ReservationGuard' object either because 'mySendData' threw an
// exception, or the data was only partially sent, because when the guard
// object goes out of scope, all remaining reserved units will be automatically
// cancelled.
//..
        }
//..
// Finally, if submitting another byte will cause the leaky bucket to overflow,
// then we wait until the submission will be allowed by waiting for an amount
// time returned by the 'calculateTimeToSubmit' method:
//..
        else {
            bsls::TimeInterval timeToSubmit =
                                             bucket.calculateTimeToSubmit(now);
            bsls::Types::Uint64 uS = timeToSubmit.totalMicroseconds() +
                                   (timeToSubmit.nanoseconds() % 1000) ? 1 : 0;
            bslmt::ThreadUtil::microSleep(static_cast<int>(uS));
        }
    }
//..
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'cancelReserved'
        //
        // Concerns:
        //: 1 The method cancels the specified number of units in the
        //:   object specified at construction.
        //:
        //: 2 The method decrements 'unitsReserved' counter.
        //:
        //: 3 The method submits no units.
        //
        // Testing:
        //   void cancelReserved(bsls::Types::Uint64 numOfUnits);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CLASS METHOD 'cancelReserved'" << endl
                          << "=============================" << endl;

        const Uint64 INIT_RESERVE = 42;

        struct {

            int    d_line;
            Uint64 d_unitsToReserve;
            Uint64 d_unitsToCancel;
            Uint64 d_numOfCancels;

        } DATA[] = {

         //  LINE   RESERVE            CANCEL       NUM_OF_CANCELS
         //  ----  --------         -------------   --------------
            {L_,   42,              10,             1},
            {L_,   ULLONG_MAX / 2, ULLONG_MAX / 4,  2},
            {L_,   50,              1,              49}
        };
        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {

            const Uint64 LINE      = DATA[ti].d_line;
            const Uint64 RESERVE   = DATA[ti].d_unitsToReserve;
            const Uint64 CANCEL    = DATA[ti].d_unitsToCancel;
            const Uint64 N_CANCELS = DATA[ti].d_numOfCancels;

            my_Reserve h;
            h.reserve(INIT_RESERVE);

            {
                Obj x(&h, RESERVE);

                for (unsigned int i = 1; i <= N_CANCELS; i++) {

                    bsls::Types::Uint64 expGuardReserve =
                        (i*CANCEL > RESERVE ? 0 : (RESERVE - i*CANCEL));

                    bsls::Types::Uint64 expReserve = INIT_RESERVE +
                        expGuardReserve;

                    x.cancelReserved(CANCEL);

                    // C-1

                    LOOP_ASSERT(LINE, expReserve == h.unitsReserved());

                    // C-3

                    LOOP_ASSERT(LINE, 0 == h.unitsSubmitted());

                    // C-2

                    LOOP_ASSERT(LINE, expGuardReserve ==
                                x.unitsReserved());
                }
            }

            LOOP_ASSERT(LINE, INIT_RESERVE == h.unitsReserved());

        }

        if (verbose) cout << endl << "Negative Testing" << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            my_Reserve r;
            Obj y(&r, 42);

            ASSERT_SAFE_FAIL(y.cancelReserved(43));
            ASSERT_SAFE_PASS(y.cancelReserved(42));
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'submitReserved'
        //
        // Concerns:
        //: 1 The method submits the specified number of units into the
        //:   object specified at construction.
        //:
        //: 2 The method decrements the 'unitsReserved' counter.
        //
        // Testing:
        //   void submitReserved(bsls::Types::Uint64 numOfUnits);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CLASS METHOD 'submitReserved'" << endl
                          << "=============================" << endl;

        struct {
            int    d_line;
            Uint64 d_unitsToReserve;
            Uint64 d_unitsToSubmit;
            Uint64 d_numOfSubmits;
        } DATA[] = {
         //  LINE   RESERVE          SUBMIT        NUM_OF_SUBMITS
         //  ----  --------        -------------   --------------
            {L_,   42,             10,              1},
            {L_,   1000,           750,             1},
            {L_,   ULLONG_MAX / 2, ULLONG_MAX / 4,  2}
        };
        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {

            const Uint64 LINE      = DATA[ti].d_line;
            const Uint64 RESERVE   = DATA[ti].d_unitsToReserve;
            const Uint64 SUBMIT    = DATA[ti].d_unitsToSubmit;
            const Uint64 N_SUBMITS = DATA[ti].d_numOfSubmits;

            my_Reserve h;

            {
                Obj x(&h, RESERVE);

                for (unsigned int i = 1; i <= N_SUBMITS; i++) {

                    bsls::Types::Uint64 expReserve =
                        i*SUBMIT > RESERVE ? 0 : (RESERVE - i*SUBMIT);

                    x.submitReserved(SUBMIT);

                    // C-1

                    LOOP_ASSERT(LINE, i*SUBMIT   == h.unitsSubmitted());
                    LOOP_ASSERT(LINE, expReserve == h.unitsReserved());

                    // C-2

                    LOOP_ASSERT(LINE, expReserve == x.unitsReserved());
                }
            }

        }

        if (verbose) cout << endl << "Negative Testing" << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            my_Reserve r;
            Obj y(&r, 42);

            ASSERT_SAFE_FAIL(y.submitReserved(43));
            ASSERT_SAFE_PASS(y.submitReserved(42));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // DTOR
        //
        // Concerns:
        //: 1 DTOR cancels all 'unitsReserved' in the 'reserve' object,
        //:   specified at the construction.
        //:
        //: 2 DTOR submits no units.
        //:
        //: 3 DTOR operates correctly if no units were reserved at
        //:   creation.
        //
        // Testing:
        //   ~btls::ReservationGuard();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DTOR" << endl
                          << "====" << endl;

        const bsls::Types::Uint64 INIT_SUBMITTED = 5;
        const bsls::Types::Uint64 INIT_RESERVED  = 37;

        struct {

            int    d_line;
            Uint64 d_unitsToReserve;

        } DATA[] = {

         //  LINE   RESERVE
         //  ----  --------
            {L_,   42,          },
            {L_,   1000,        },
            {L_,   0,           }, // C-3
            {L_,   ULLONG_MAX/2 },
        };
        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

        my_Reserve h;
        h.reserve(INIT_SUBMITTED+INIT_RESERVED);
        h.submitReserved(INIT_SUBMITTED);

        for (int ti = 0; ti < NUM_DATA; ++ti) {

            const Uint64 LINE    = DATA[ti].d_line;
            const Uint64 RESERVE = DATA[ti].d_unitsToReserve;

            {
                Obj x(&h, RESERVE);
                LOOP_ASSERT(LINE,
                            INIT_RESERVED + RESERVE == h.unitsReserved());
            }

            LOOP_ASSERT(LINE, INIT_SUBMITTED == h.unitsSubmitted()); // C-1
            LOOP_ASSERT(LINE, INIT_RESERVED  == h.unitsReserved());  // C-2
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATOR (BOOTSTRAP)
        //
        // Concerns:
        //: 1 Units in the specified 'reserve' are reserved during the
        //:   construction
        //:
        //: 2 'ptr' returns pointer to the 'reserve' object, specified during
        //:   the construction
        //:
        //: 3 'unitsReserved' return the number of units reserved in the
        //:   specified 'reserve' object
        //:
        //: 4 'unitsReserved' is represented with 64-bit integral type
        //:
        //: 5 Specifying wrong parameters for constructor causes certain
        //:   behavior in specific build configuration.
        //
        // Testing:
        //   btls::ReservationGuard(TYPE*               reserve,
        //                         bsls::Types::Uint64 numOfUnits);
        //   TYPE* ptr() const;
        //   bsls::Types::Uint64 unitsReserved() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATOR (BOOTSTRAP)" << endl
                          << "===============================" << endl;

        my_Reserve h;

        Obj x(&h, 1000);

        ASSERT(1000 == h.unitsReserved()); // C-1
        ASSERT(1000 == x.unitsReserved()); // C-3

        ASSERT(&h   == x.ptr()); // C-2

        h.reset();

        // C-4

        Obj y(&h, ULLONG_MAX);
        ASSERT(ULLONG_MAX == h.unitsReserved());
        ASSERT(ULLONG_MAX == y.unitsReserved());

        // C-5

        if (verbose) cout << endl << "Negative Testing" << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_FAIL(Obj x(0,42));
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //
        // Concerns:
        //: 1 'Reserve' method adds units to 'unitsReserved'.
        //:
        //: 2 'Reserve' method does not affect 'unitsSubmitted'.
        //:
        //: 3 'submitReserved' method adds units to 'unitsSubmitted' and
        //:   subtracts units from 'unitsReserved'.
        //:
        //: 4 'cancelReserved' method subtracts units from 'unitsReserved'.
        //:
        //: 5 'cancelReserved' method does not affect 'unitsSubmitted'.
        //:
        //: 6 'unitsSubmitted' is represented by 64-bit unsigned integral
        //:   type.
        //:
        //: 7 'unitsReserved' is represented by 64-bit unsigned integral
        //:   type.
        //:
        //: 8 'unitsReserved' and 'unitSubmitted' are wired to the
        //:   different places.
        //:
        //: 9 'reset' method resets the object to its default-constructed
        //:   state.
        //
        // Testing:
        //   class 'my_Reserve'
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST APPARATUS" << endl
                          << "==============" << endl;

        my_Reserve x;

        ASSERT(0 == x.unitsReserved());
        ASSERT(0 == x.unitsSubmitted());

        x.reserve(42);
        ASSERT(42 == x.unitsReserved());  // C-1
        ASSERT(0  == x.unitsSubmitted()); // C-2

        // C-3, C-9

        x.submitReserved(20);
        ASSERT(22 == x.unitsReserved());
        ASSERT(20 == x.unitsSubmitted());

        x.cancelReserved(22);
        ASSERT(0  == x.unitsReserved());  // C-4
        ASSERT(20 == x.unitsSubmitted()); // C-5

        // C-6, C-7

        my_Reserve y;

        y.reserve(ULLONG_MAX);
        ASSERT(ULLONG_MAX == y.unitsReserved());

        y.submitReserved(ULLONG_MAX/4);
        ASSERT(ULLONG_MAX/4              == y.unitsSubmitted());
        ASSERT(ULLONG_MAX - ULLONG_MAX/4 == y.unitsReserved());

        y.cancelReserved(ULLONG_MAX/4);
        ASSERT(ULLONG_MAX - 2*(ULLONG_MAX/4) == y.unitsReserved());

        // C-9

        y.reset();
        ASSERT(0 == y.unitsReserved());
        ASSERT(0 == y.unitsSubmitted());

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Plan:
        //  Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //  This "test" *exercises* basic functionality, but *tests*
        //  nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        my_Reserve h;

        {
            Obj x(&h,500);

            ASSERT(500 == h.unitsReserved());

            x.submitReserved(200);
            ASSERT(200 == h.unitsSubmitted());
            ASSERT(300 == h.unitsReserved());
        }

        ASSERT(0   == h.unitsReserved());
        ASSERT(200 == h.unitsSubmitted());

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
