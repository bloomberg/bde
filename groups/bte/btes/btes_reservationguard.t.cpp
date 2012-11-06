//btes_reservationguard.t.cpp                                              -*-C++-*-

#include <btes_reservationguard.h>

#include <bcemt_threadutil.h>

#include <bdema_managedptr.h>

#include <bdes_bitutil.h>

#include <bdet_timeinterval.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <btes_leakybucket.h>


using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a mechanism.
//
// Primary Manipulators:
// o 'submitReserved'
// o 'cancelReserved'
//
// Basic Accessors:
// o 'ptr'
// o 'unitsReserved'
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
//-----------------------------------------------------------------------------
//
// CREATORS
//  [4] ~btes_ReservationGuard();
//  [3] btes_ReservationGuard(TYPE* reserve, bsls_Types::Uint64 numOfUnits);
//
// MANIPULATORS
//  [6] void cancelReserved(bsls_Types::Uint64 numOfUnits);
//  [5] void submitReserved(bsls_Types::Uint64 numOfUnits);
//
// ACCESSORS
//  [3] bsls_Types::Uint64 unitsReserved() const;
//  [3] TYPE *ptr() const;
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [2] TEST APPARATUS
// [7] USAGE EXAMPLE
// [3] All accessor methods are declared 'const'.
// [*] All creator/manipulator ptr./ref. parameters are 'const'.
//=============================================================================

// ============================================================================
//                      BTES_RESERVATIOGUARD TEST HELPER
// ----------------------------------------------------------------------------

class my_Reserve {
    // This class provides a simulated reservation mechanism, which conforms to
    // the interface required by the btes_ReservationGuard class. It uses two
    // counters to track the numbers of submitted and reserved units.

    // DATA
    bsls_Types::Uint64 d_unitsSubmitted;
    bsls_Types::Uint64 d_unitsReserved;

public:

    // CREATORS
    my_Reserve();
        // Create a 'my_Reserve' object having zero 'unitsReserved' and
        // 'unitsSubmitted'

    // MANIPULATORS
    void reserve(bsls_Types::Uint64 numOfUnits);
        // Add the specified 'numOfUnits' to the 'unitsReserved' counter.

    void submitReserved(bsls_Types::Uint64 numOfUnits);
        // Subtract the specified 'numOfUnits' from 'unitsReserved' and add it
        // to 'unitsSubmitted', if 'numOfUnits' is less than 'unitsReserved'.
        // Otherwise, add 'unitsReserved' to 'unitsSubmitted' and set
        // 'unitsReserved' to 0.

    void cancelReserved(bsls_Types::Uint64 numOfUnits);
        // Subtract the specified 'numOfUnits' from 'unitsReserved', if
        // 'numOfUnits' is less than 'unitsReserved'. Otherwise, set
        // 'unitsReserved' to 0.

    void reset();
        // Reset this 'my_Reserve' object to its default-constructed state.

    // ACCESSORS
    bsls_Types::Uint64 unitsReserved() const;
        // Return the number of units currently reserved.

    bsls_Types::Uint64 unitsSubmitted() const;
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
void my_Reserve::reserve(bsls_Types::Uint64 numOfUnits)
{
    d_unitsReserved += numOfUnits;
}

inline
void my_Reserve::submitReserved(bsls_Types::Uint64 numOfUnits)
{
    if (numOfUnits < d_unitsReserved) {
        d_unitsReserved -= numOfUnits;
    }
    else {
        d_unitsReserved = 0;
    }
    d_unitsSubmitted += numOfUnits;
}

inline
void my_Reserve::cancelReserved(bsls_Types::Uint64 numOfUnits)
{
    if (numOfUnits < d_unitsReserved) {
        d_unitsReserved -= numOfUnits;
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
bsls_Types::Uint64 my_Reserve::unitsReserved() const
{
    return d_unitsReserved;
}

inline
bsls_Types::Uint64 my_Reserve::unitsSubmitted() const
{
    return d_unitsSubmitted;
}

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//=============================================================================

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP0_ASSERT ASSERT
#define LOOP1_ASSERT LOOP_ASSERT

//=============================================================================
//                  STANDARD BDE VARIADIC ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)   N
#define NUM_ARGS(...) NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1, 0, "")

#define LOOPN_ASSERT_IMPL(N, ...) LOOP ## N ## _ASSERT(__VA_ARGS__)
#define LOOPN_ASSERT(N, ...)      LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...) LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef btes_ReservationGuard<my_Reserve> Obj;

typedef bdet_TimeInterval   Ti;
typedef bsls_Types::Uint64  Uint64;
typedef unsigned int        uint;

// Function used for testing usage example.

static bsls_Types::Uint64 sendData(size_t dataSize)
    // Try to send a specified 'dataSize' amount of data over the network
    // return the number of bytes that have been actually sent.

{
//..
// In our example we don`t deal with actual data sending, we assume that
// the function has sent certain amount of data (3/4 of the requested amount)
// successfully and returns.
//..
   return (dataSize*3)>>2;
}
//..

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    // bool     veryVerbose = argc > 3;
    // bool veryVeryVerbose = argc > 4;

    switch (test) {

        case 7: {
            // ----------------------------------------------------------------
            // USAGE EXAMPLE
            //   The usage example provided in the component header file must
            //   compile, link, and run on all platforms as shown.
            //
            // Testing:
            //   USAGE EXAMPLE
            //-----------------------------------------------------------------

            if (verbose) cout << endl << "TESTING USAGE EXAMPLE" << endl
                                      << "=====================" << endl;
//..
// First we define the size of each data chunk, and the total size of the data
// to transmit:
//..
  const unsigned int CHUNK_SIZE = 256;
  bsls_Types::Uint64 bytesSent  = 0;
  bsls_Types::Uint64 sizeOfData = 10 * 1024; // in bytes
//..
// Next, we create a 'btes_LeakyBucket' object having the required parameters:
//..
  bsls_Types::Uint64 rate     = 512;
  bsls_Types::Uint64 capacity = 1536;
//..
//..
  bdet_TimeInterval now = bdetu_SystemTime::now();
  btes_LeakyBucket  bucket(rate, capacity, now);
//..
//  Then we define a loop in which we test:
//..
  while (bytesSent < sizeOfData) {

      now = bdetu_SystemTime::now();
//..
// Now, for each iteration we check whether submitting another chunk into the
// bucket would cause overflow:
//..
      if (!bucket.wouldOverflow(CHUNK_SIZE, now)) {
//..
// If sending data is allowed, we reserve units in the leaky bucket for one
// data chunk.  We create a 'btes_ReservationGuard' object, specifying the
// 'btes_LeakyBucket' object, in which we want to reserve units and the
// number of units, we want to reserve:
//..
          btes_ReservationGuard<btes_LeakyBucket> guard(&bucket,
                                                        CHUNK_SIZE);
//..
// Next, we are trying to send data chunk over the network and submit the
// number of units corresponding to the amount of data that was actually sent:
//..
          bsls_Types::Uint64 result;
          result = sendData(CHUNK_SIZE);
          bytesSent += result;
          guard.submitReserved(result);
//..
// We do not care about the units that possibly remained unused, because they
// will be cancelled automatically, when the scope of the 'guard' will be left
// and the destructor will be called. If 'mySendData' throws an exception, all
// units will be also returned to the 'leakyBucket' object:
//..
      }
//..
// In case submitting the data chunk would cause overflow, we invoke
// the 'calculateTimeToSubmit' method to determine how much time we need to
// wait before submitting the data chunk without overflowing the bucket
// We round up the number of microseconds in time interval.
//..
      else {

          bdet_TimeInterval timeToSubmit = bucket.calculateTimeToSubmit(now);
          bsls_Types::Uint64 uS = timeToSubmit.totalMicroseconds() +
                                  (timeToSubmit.nanoseconds() % 1000) ? 1 : 0;
          bcemt_ThreadUtil::microSleep(uS);
      }
  }

        } break;

        case 6: {
            // ----------------------------------------------------------------
            // CLASS METHOD 'cancelReserved'
            //
            // Concerns:
            //   1 The method cancels the specified number of units in the
            //     object specified at construction.
            //
            //   2 The method decrements 'unitsReserved' counter.
            //
            //   3 The method submits no units.
            //
            // Testing:
            //   void cancelReserved(bsls_Types::Uint64 numOfUnits);
            //-----------------------------------------------------------------

            if (verbose) cout << endl << "TESTING: 'cancelReserved'" << endl
                                      << "=========================" << endl;

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

                        bsls_Types::Uint64 expGuardReserve =
                               (i*CANCEL > RESERVE ? 0 : (RESERVE - i*CANCEL));

                        bsls_Types::Uint64 expReserve = INIT_RESERVE +
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
                bsls_AssertFailureHandlerGuard hG(
                    bsls_AssertTest::failTestDriver);

                my_Reserve r;
                Obj y(&r, 42);

                ASSERT_SAFE_FAIL(y.cancelReserved(43));
                ASSERT_SAFE_PASS(y.cancelReserved(42));
            }

        } break;

        case 5: {
            // ----------------------------------------------------------------
            // CLASS METHOD 'submitReserved'
            //
            // Concerns:
            //   1 The method submits the specified number of units into the
            //     object specified at construction.
            //
            //   2 The method decrements the 'unitsReserved' counter.
            //
            // Testing:
            //   void submitReserved(bsls_Types::Uint64 numOfUnits);
            //-----------------------------------------------------------------

            if (verbose) cout << endl << "TESTING: 'submitReserved'" << endl
                                      << "=========================" << endl;

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

                        bsls_Types::Uint64 expReserve =
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
                bsls_AssertFailureHandlerGuard hG(
                    bsls_AssertTest::failTestDriver);

                my_Reserve r;
                Obj y(&r, 42);

                ASSERT_SAFE_FAIL(y.submitReserved(43));
                ASSERT_SAFE_PASS(y.submitReserved(42));
            }


        } break;

        case 4: {
            // ----------------------------------------------------------------
            // DTOR
            //
            // Concerns:
            //   1 DTOR cancels all 'unitsReserved' in the 'reserve' object,
            //     specified at the construction.
            //
            //   2 DTOR submits no units.
            //
            //   3 DTOR operates correctly if no units were reserved at
            //     creation.
            //
            // Testing:
            //   ~btes_ReservationGuard();
            //-----------------------------------------------------------------

            if (verbose) cout << endl << "TESTING: DTOR" << endl
                                      << "=============" << endl;

            const bsls_Types::Uint64 INIT_SUBMITTED = 5;
            const bsls_Types::Uint64 INIT_RESERVED  = 37;

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
            // ----------------------------------------------------------------
            // PRIMARY MANIPULATOR (BOOTSTRAP)
            //
            // Concerns:
            //   1 Units in the specified 'reserve' are reserved during the
            //     construction
            //
            //   2 'ptr' returns pointer to the 'reserve' object, specified
            //     during the construction
            //
            //   3 'unitsReserved' return the number of units reserved in the
            //     specified 'reserve' object
            //
            //   4 'unitsReserved' is represented with 64-bit integral type
            //
            //   5 Specifying wrong parameters for constructor causes certain
            //     behavior in specific build configuration.
            //
            // Testing:
            //   btes_ReservationGuard(TYPE*              reserve,
            //                         bsls_Types::Uint64 numOfUnits);
            //   TYPE* ptr() const;
            //   bsls_Types::Uint64 unitsReserved() const;
            //-----------------------------------------------------------------

            if (verbose) cout <<
                endl << "BOOTSTRAP 'btes_ReservationGuard'" << endl
                     << "=================================" << endl;

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
                bsls_AssertFailureHandlerGuard hG(
                    bsls_AssertTest::failTestDriver);

                ASSERT_SAFE_FAIL(Obj x(0,42));
            }

        } break;

        case 2: {
            // ----------------------------------------------------------------
            // TEST APPARATUS
            //
            // Concerns:
            //   1 'Reserve' method adds units to 'unitsReserved'.
            //
            //   2 'Reserve' method does not affect 'unitsSubmitted'.
            //
            //   3 'submitReserved' method adds units to 'unitsSubmitted' and
            //     subtracts units from 'unitsReserved'.
            //
            //   4 'cancelReserved' method subtracts units from 'unitsReserved'.
            //
            //   5 'cancelReserved' method does not affect 'unitsSubmitted'.
            //
            //   6 'unitsSubmitted' is represented by 64-bit unsigned integral
            //     type.
            //
            //   7 'unitsReserved' is represented by 64-bit unsigned integral
            //     type.
            //
            //   8 'unitsReserved' and 'unitSubmitted' are wired to the
            //     different places.
            //
            //   9 'reset' method resets the object to its default-constructed
            //     state.
            //
            // Testing:
            //   class 'my_Reserve'
            //
            // ----------------------------------------------------------------

            if (verbose) cout << endl << "TESTING: test apparatus" << endl
                                      << "=======================" << endl;

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
            // ----------------------------------------------------------------
            // BREATHING TEST:
            //   Developers' Sandbox.
            //
            // Plan:
            //  Perform and ad-hoc test of the primary modifiers and accessors.
            //
            // Testing:
            //  This "test" *exercises* basic functionality, but *tests*
            //  nothing.
            // ----------------------------------------------------------------

            if (verbose) cout << endl << "BREATHING TEST" << endl
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
