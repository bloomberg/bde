// btls_leakybucket.t.cpp                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btls_leakybucket.h>

#include <bslmt_threadutil.h>

#include <bslim_testutil.h>

#include <bdlt_currenttime.h>

#include <bsls_asserttest.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_c_math.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a mechanism.
//
// This class provides a value constructor capable of creating an object having
// any parameters.
//
// Primary Manipulators:
//: o Value constructor
//
// Basic Accessors:
//: o 'rate'
//: o 'capacity'
//: o 'lastUpdateTime'
//: o 'unitsInBucket'
//: o 'unitsReserved'
//
// Global Concerns:
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o ACCESSOR methods are 'const' thread-safe.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [17] static TimeInterval calculateTimeWindow(capacity, drainRate);
// [16] static bsls::Types::Uint64 calculateCapacity(drainRate, timeWindow);
// [14] calculateDrainTime(numOfUnits, drainRate, ceilFlag);
//
// CREATORS
// [ 3] btls::LeakyBucket(drainRate, window, currentTime);
//
// MANIPULATORS
// [ 6] void setRateAndCapacity(newRate, newCapacity);
// [ 5] void submit(bsls::Types::Uint64 numOfUnits);
// [ 5] void reserve(bsls::Types::Uint64 numOfUnits);
// [ 7] void updateState(const bsls::TimeInterval& currentTime);
// [ 8] bool wouldOverflow(currentTime);
// [10] void submitReserved(bsls::Types::Unit64 numOfUnits);
// [10] void cancelReserved(bsls::Types::Unit64 numOfUnits);
// [12] void resetStatistics();
// [13] void reset(const bsls::TimeInterval& currentTime);
// [15] bsls::TimeInterval calculateTimeToSubmit(currentTime);
//
// ACCESSORS
// [ 4] bsls::Types::Uint64 drainRate() const;
// [ 4] bsls::Types::Uint64 capacity() const;
// [ 4] bsls::Types::Uint64 unitsInBucket() const;
// [ 4] bsls::Types::Uint64 unitsReserved() const;
// [ 4] bsls::TimeInterval lastUpdateTime() const;
// [11] void LeakyBucket::getStatistics(smtUnits, unusedUnits) const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [19] USAGE EXAMPLE
// [ 4] All accessor methods are declared 'const'.
// [ *] All creator/manipulator ptr./ref. parameters are 'const'.
//=============================================================================

// ============================================================================
//                     BTES_RESERVATIOGUARD TEST HELPER
// ----------------------------------------------------------------------------

class mock_LB {
    // This mechanism mocks basic functionality of 'btls::LeakyBucket' and is
    // used to test the 'testLB' function.

    // DATA
    bsls::Types::Uint64 d_unitsInBucket;  // number of units currently in the
                                          // bucket

    bsls::TimeInterval   d_lastUpdateTime; // time of the last update

    bsls::TimeInterval   d_submitInterval; // minimum interval between
                                          // submitting units

    bsls::Types::Uint64 d_rate;           // drain rate in units per second.
                                          // Used for 'setRateAndCapacity' mock

    bsls::Types::Uint64 d_capacity;       // capacity in units.  Used for
                                          // 'setRateAndCapacity' mock
  public:
    // CREATORS
    mock_LB();
        // Create a 'mock_LB' object having rate of 1 unit per second, capacity
        // of 1 unit, lastUpdateTime of 0, submit interval of 0, and such that
        // 'unitsInBucket == 0'.

    mock_LB(bsls::TimeInterval submitInterval, bsls::TimeInterval currentTime);
        // Create a 'mock_LB' object, having rate of 1 unit per second,
        // capacity of 1, the specified 'submitInterval' the lastUpdateTime of
        // the specified 'currentTime' and such that 'unitsInBucket == 0'.

    // MANIPULATORS
    void setRateAndCapacity(bsls::Types::Uint64 newRate,
                            bsls::Types::Uint64 newCapacity);
        // Set the rate of this 'mock_LB' object to the specified 'newRate' and
        // the capacity to the specified 'newCapacity'.

    void reset(bsls::TimeInterval currentTime);
        // Set the 'lastUpdateTime' of this 'mock_LB' object to the specified
        // 'currentTime' and number of units in bucket to 0.

    void submit(bsls::Types::Uint64 numOfUnits);
        // Add the specified 'numOfUnits' to the 'unitsInBucket' counter.

    bool wouldOverflow(bsls::TimeInterval currentTime);
        // Return whether submitting 1 more unit at the specified 'currentTime'
        // would cause this object overflow.  Note that actually, 'mock_LB'
        // does not care about the capacity and simply allows submitting if the
        // time passed since last check exceeds the specified 'submitInterval'
        // and forbids submitting otherwise.

    bsls::TimeInterval calculateTimeToSubmit(bsls::TimeInterval currentTime);
        // Return the time interval that should pass until it will be possible
        // to submit any new unit into this leaky bucket, at the specified
        // 'currentTime'.  Return the time interval, calculated as difference
        // between time passed since last check and 'submitInterval', if the
        // time interval passed since last check is shorter than
        // 'submitInterval'.  Otherwise, return zero interval.

    // ACCESSORS
    bsls::Types::Uint64 unitsInBucket() const;
        // Return the number of units that are currently in the bucket.

    bsls::TimeInterval lastUpdateTime() const;
        // Return the time of last update of this 'mock_LB' object, as a time
        // interval, describing the moment in time the bucket was last updated.
        // The returned time interval uses the same reference point as the time
        // interval specified during construction or last invocation of the
        // 'reset' method.

    bsls::TimeInterval submitInterval() const;
        // Return the minimum time interval that must pass between submits.

    bsls::Types::Uint64 rate() const;
        // Return the drain rate in units per second.

    bsls::Types::Uint64 capacity() const;
        // Return the capacity in units.
};

// CREATORS
inline
mock_LB::mock_LB()
: d_unitsInBucket(0)
, d_lastUpdateTime(0,0)
, d_submitInterval(0,0)
, d_rate(1)
, d_capacity(1)
{}

inline
mock_LB::mock_LB(bsls::TimeInterval submitInterval,
                 bsls::TimeInterval currentTime)
: d_unitsInBucket(0)
, d_lastUpdateTime(currentTime)
, d_submitInterval(submitInterval)
, d_rate(1)
, d_capacity(1)
{}

// MANIPULATORS
inline
void mock_LB::setRateAndCapacity(bsls::Types::Uint64 newRate,
                                 bsls::Types::Uint64 newCapacity)
{
    d_rate     = newRate;
    d_capacity = newCapacity;
}

inline
void mock_LB::reset(bsls::TimeInterval currentTime)
{
    d_lastUpdateTime     = currentTime;
    d_unitsInBucket = 0;
}

inline
void mock_LB::submit(bsls::Types::Uint64 numOfUnits)
{
    d_unitsInBucket += numOfUnits;
}

inline
bool mock_LB::wouldOverflow(bsls::TimeInterval currentTime)
{
    bsls::TimeInterval delta = currentTime - d_lastUpdateTime;

    if (delta < d_submitInterval) {
        return true;                                                  // RETURN
    }

    d_lastUpdateTime = currentTime;

    return false;
}

inline
bsls::TimeInterval mock_LB::calculateTimeToSubmit(
                                                bsls::TimeInterval currentTime)
{
    bsls::TimeInterval delta = currentTime - d_lastUpdateTime;

    if (delta < d_submitInterval) {
        return d_submitInterval - delta;                              // RETURN
    }

    d_lastUpdateTime = currentTime;

    return bsls::TimeInterval(0);
}

// ACCESSORS
inline
bsls::Types::Uint64 mock_LB::unitsInBucket() const
{
    return d_unitsInBucket;
}

inline
bsls::TimeInterval mock_LB::lastUpdateTime() const
{
    return d_lastUpdateTime;
}

inline
bsls::TimeInterval mock_LB::submitInterval() const
{
    return d_submitInterval;
}

inline
bsls::Types::Uint64 mock_LB::rate() const
{
    return d_rate;
}

inline
bsls::Types::Uint64 mock_LB::capacity() const
{
    return d_capacity;
}

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

#define ASSERT_FAIL(expr) BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr) BSLS_ASSERTTEST_ASSERT_PASS(expr)
#define ASSERT_OPT_FAIL(expr) BSLS_ASSERTTEST_ASSERT_OPT_FAIL(expr)
#define ASSERT_OPT_PASS(expr) BSLS_ASSERTTEST_ASSERT_OPT_PASS(expr)
#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
typedef btls::LeakyBucket    Obj;
typedef bsls::TimeInterval   Ti;
typedef bsls::Types::Uint64 Uint64;
typedef unsigned int        Uint;

template<class T>
static Ti testLB(T&        object,
                 Uint64    rate,
                 Uint64    capacity,
                 Uint64    dataSize,
                 Uint64    chunkSize,
                 const Ti& minQueryInterval)
    // Simulate load generation on specified rate controlling 'object', having
    // the specified 'rate' and 'capacity', by modeling sending the specified
    // 'dataSize' divided on the chunks of the specified 'chunkSize', keeping
    // intervals between querying the 'object' not shorter than the specified
    // 'minQueryInterval'.  Return the total amount of time
{
    Uint64 dataSent = 0;

    Ti now(0.0);
    Ti begin(now);

    object.setRateAndCapacity(rate, capacity);

    Uint64 numSleeps=0;
    Uint64 loops = 0;

    while (dataSent < dataSize) {
        ++loops;

        if (object.wouldOverflow(now)) {

            // Query the rate controlling object, how long it should pass until
            // submitting more units is allowed.

            Ti timeToSubmit = object.calculateTimeToSubmit(now);

            if (0 != timeToSubmit) {

                // do not query LB with intervals, shorter than the specified
                // minimum interval.

                if(timeToSubmit < minQueryInterval) {
                    timeToSubmit = minQueryInterval;
                }
                now += timeToSubmit;
                ++numSleeps;
            }
            continue;
        }
        object.submit(chunkSize);
        dataSent += chunkSize;
    }

    Ti actual = now - begin;
    return actual;
}

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Controlling Network Traffic Generation
///-------------------------------------------------
// In some systems, data is processed faster than they are consumed by I/O
// interfaces.  This could lead to data loss due to the overflowing of the
// buffers where data is queued before being processed.  In other systems,
// generic resources are shared, and their consumption might need to be managed
// in order to guarantee quality-of-service (QOS).
//
// Suppose we have a network interface capable of transferring at a rate of
// 1024 byte/s and an application wants to transmit 5 KiB (5120 bytes) of data
// over that network in 20 different 256-bytes data chunks.  We want to send
// data over this interface and want to ensure the transmission uses on average
// less than 50% of the available bandwidth, or 512 byte/s.  In this way, other
// clients can still reasonably send and receive data using the same network
// interface.
//
// Further suppose that we have a function, 'sendData', that transmits a
// specified data buffer over that network interface:
//..
    bool sendData(const char *buffer, size_t dataSize)
        // Send the specified 'buffer' of the specified size 'dataSize' through
        // the network interface.  Return 'true' if data was sent successfully,
        // and 'false' otherwise.
    {
        (void) buffer;
        (void) dataSize;

        // In our example we don`t deal with actual data sending, so we assume
        // that the function sends data successfully and return true.
        return true;
    }
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int     test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 19: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

            if (verbose) cout << endl
                              << "TESTING USAGE EXAMPLE" << endl
                              << "=====================" << endl;
//..
// First, we create a leaky bucket having a drain rate of 512 bytes/s, a
// capacity of 2560 bytes, and a time origin set to the current time (as an
// interval from unix epoch).  Note that 'unit', the unit of measurement for
// leaky bucket, corresponds to 'byte' in this example:
//..
    bsls::Types::Uint64 rate     = 512;  // bytes/second
    bsls::Types::Uint64 capacity = 2560; // bytes
    bsls::TimeInterval  now      = bdlt::CurrentTime::now();
    btls::LeakyBucket   bucket(rate, capacity, now);
//..
// Then, we define a data buffer to be sent, the size of each data chunk, and
// the total size of the data to transmit:
//..
    char                buffer[5120];
    unsigned int        chunkSize  = 256;             // in bytes
    bsls::Types::Uint64 totalSize  = 20 * chunkSize;  // in bytes
    bsls::Types::Uint64 dataSent   = 0;               // in bytes

    // Load 'buffer'...
//..
// Notice that, for the sake of brevity, we elide the loading of 'buffer' with
// the data to be sent.
//
// Now, we send the chunks of data using a loop.  For each iteration, we check
// whether submitting another byte would cause the leaky bucket to overflow.
// If not, we send an additional chunk of data and submit the number of bytes
// sent to the leaky bucket.  Note that 'submit' is invoked only after the data
// has been sent.
//..
    char *data = buffer;
    while (dataSent < totalSize) {
        now = bdlt::CurrentTime::now();
        if (!bucket.wouldOverflow(now)) {
            if (true == sendData(data, chunkSize)) {
                data += chunkSize;
                bucket.submit(chunkSize);
                dataSent += chunkSize;
            }
        }
//..
// Finally, if submitting another byte will cause the leaky bucket to overflow,
// then we wait until the submission will be allowed by waiting for an amount
// time returned by the 'calculateTimeToSubmit' method:
//..
        else {
            bsls::TimeInterval timeToSubmit =
                                             bucket.calculateTimeToSubmit(now);

            // Round up the number of microseconds.
            bsls::Types::Uint64 uS = timeToSubmit.totalMicroseconds() +
                                 ((timeToSubmit.nanoseconds() % 1000) ? 1 : 0);
            bslmt::ThreadUtil::microSleep(static_cast<int>(uS));
        }
    }
//..
// Notice that we wait by putting the thread into a sleep state instead of
// using busy-waiting to better optimize for multi-threaded applications.
//..
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // FUNCTIONALITY
        //   Ensure that 'btes::LeaktBucket' can keep the specified load
        //   rate when used in real application.
        //
        // Concerns:
        //: 1 'btls::LeakyBucket' keeps specified load rate and allows
        //:   deviation from the specified rate no bigger than the
        //:   'capacity' divided by total amount of sent data.
        //
        // Plan:
        //: 1 Using table-driven technique:
        //:
        //:   1 Define the set of values, containing values of 'rate' and
        //:     'capacity' attributes, size of chunks, data is divided on,
        //:     test duration and the values of maximum deviation of actual
        //:     time it took to send data from the specified test
        //:     duration.
        //:
        //: 2 Use the 'testLB' function to simulate operations with LB
        //:   in actual application with different parameters
        //:   ('rate' and 'capacity').
        //:
        //: 3 Verify that that the difference between the specified and
        //:   measured rate does not exceed allowed limits.
        //
        // Testing:
        //   void submit(unsigned int numOfUnits);
        //   bool wouldOverflow(currentTime);
        //   bsls::TimeInterval calculateTimeToSubmit(currentTime);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "FUNCTIONALITY" << endl
                          << "=============" << endl;

        const Ti CREATION_TIME;
        const Uint64 M = 1024*1024;
        const Uint64 G = 1024LL*1024*1024;

        struct {
            int    d_line;
            Uint64 d_drainRate;
            Uint64 d_capacity;
            Uint64 d_chunkSize;
            Uint64 d_dataSize;
            Ti d_expectedDuration;
        } DATA[] = {

        // LINE  RATE    CAPACITY    CHUNK  DATA_SIZE EXP_DUR
        // ----  ----    ---------   -----  --------- -------
          {  L_,   1*M,        1000,   1000,      1*M,  Ti( 1)},
          {  L_,   1*M,       10000,   1000,   1*M*10,  Ti(10)},

          // 10 MUnits/second rate
          {  L_,   10*M,      10000,   1000,  10*M*10,  Ti(10)},
          {  L_,   10*M,     100000,   1500,  10*M*10,  Ti(10)},

          // Low rate
          {  L_,     50,          5,    10,     50*15,  Ti(15)},
          {  L_,     50,          5,     5,     50*15,  Ti(15)},
          {  L_,     10,          1,     2,     10*10,  Ti(10)},

          // high rate (35 G Units/second)
          {  L_, 35LL*G,        4*M,  1400,    35LL*G,  Ti( 1)}
        };
        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

        for(int ti=0; ti < NUM_DATA; ti++) {

            const int    LINE         = DATA[ti].d_line;
            const Uint64 RATE         = DATA[ti].d_drainRate;
            const Uint64 CAPACITY     = DATA[ti].d_capacity;
            const Uint64 CHUNK_SIZE   = DATA[ti].d_chunkSize;
            const Uint64 DATA_SIZE    = DATA[ti].d_dataSize;
            const Ti     EXP_DURATION = DATA[ti].d_expectedDuration;

            Obj x(1, 1, Ti(0));
            Ti actualDuration = testLB<Obj>(x,
                                            RATE,
                                            CAPACITY,
                                            DATA_SIZE,
                                            CHUNK_SIZE,
                                            Ti(0,5000));

            // Calculate the deviation in percentage.
            double maxNegDev = -static_cast<double>(CAPACITY * 100)
                             / static_cast<double>(DATA_SIZE);

            double dev = 100 *
                (EXP_DURATION.totalSecondsAsDouble() -
                 actualDuration.totalSecondsAsDouble())
                / EXP_DURATION.totalSecondsAsDouble();

            // Check if the allowed speed was exceeded.
            if (dev < 0) {
                LOOP_ASSERT(LINE, dev >= maxNegDev);
            }
            else {
                LOOP_ASSERT(LINE, dev <= 2.5);
            }
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'calculateTimeWindow'
        //   Ensure that the class method calculates the equivalent time
        //   window, using the specified 'capacity' and 'drainRate'.
        //
        // Concerns:
        //: 1 The method correctly calculates the time window using given rate
        //:   and capacity.
        //:
        //: 2 The time window is calculated with +1 ns precision.
        //:
        //: 3 If the calculated time window is 0, the time interval of 1
        //:   nanosecond is returned.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of values, containing values of drain rate,
        //:     capacity, and the expected value of corresponding time window
        //:     including boundary values corresponding to every range of
        //:     values that each individual attribute can independently attain.
        //:
        //:   2 Additionally provide values, that would allow to test rounding.
        //:     (C-2..3)
        //:
        //: 2 For each row of the table, defined in P-1 invoke the
        //:   'calculateTimeWindow' class method and verify the returned value.
        //:   (C-1)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to call this function with
        //:   invalid parameters (using the 'BSLS_ASSERTTEST_*' macros).  (C-4)
        //
        // Testing:
        //   static TimeInterval calculateTimeWindow(capacity, drainRate);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'calculateTimeWindow'" << endl
                          << "=====================" << endl;

        const Uint64 MAX_R = ULLONG_MAX;
        const Uint64 M     = 1000000;
        const Uint64 G     = 1000000000;

        // Numbers of units to be consumed during different intervals at
        // maximum rate.

        const Uint64 U_NS   = MAX_R / G;

        struct {
            Uint    d_line;
            Uint64  d_drainRate;
            Uint64  d_capacity;
            Ti      d_expectedWindow;

        } DATA[] = {

        //  LINE       RATE          CAP             EXP_WINDOW
        //  ----    -----------  -----------   -------------------------
            {L_,          1000,         100,   Ti(        0, 100000000)},
            {L_,          10*M,          21,   Ti(        0,      2100)},
            {L_,          10*M,           1,   Ti(        0,       100)},

            {L_,         MAX_R,           1,   Ti(        0,         1)},
            {L_,             1,   LLONG_MAX,   Ti(LLONG_MAX,         0)},
            {L_,         MAX_R,  ULLONG_MAX,   Ti(        1,         0)},
            {L_,         MAX_R,        U_NS,   Ti(        0,         1)}
        };

        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

        for(int ti = 0; ti < NUM_DATA; ti++) {

            const Uint64 LINE        = DATA[ti].d_line;
            const Uint64 CAPACITY    = DATA[ti].d_capacity;
            const Uint64 RATE        = DATA[ti].d_drainRate;
            const Ti EXPECTED_WINDOW = DATA[ti].d_expectedWindow;

            LOOP_ASSERT(LINE, EXPECTED_WINDOW ==
                        Obj::calculateTimeWindow(RATE, CAPACITY));
        }

        // C-4
        if (verbose) cout << endl << "Negative Testing" << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            // Rate is 0.
            ASSERT_FAIL(Obj::calculateTimeWindow(0,    0));
            ASSERT_FAIL(Obj::calculateTimeWindow(0,    1000));

            // The resulting time interval can not be represented by
            // bdet_TimeInterval.

            ASSERT_FAIL(Obj::calculateTimeWindow(1, ULLONG_MAX));
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'calculateCapacity'
        //   Ensure that the class method calculates the equivalent capacity of
        //   the leaky bucket, using specified 'drainRate' and 'timeWindow'.
        //
        // Concerns:
        //: 1 The method calculates capacity using given rate and time window.
        //:
        //: 2 The calculated capacity is rounded down.
        //:
        //: 3 If calculated capacity is 0, the capacity of 1 is returned.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        //:lan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of values, containing values os drain rate, time
        //:     window and the expected value of the capacity, including
        //:     boundary values corresponding to every range of values that
        //:     each individual attribute can independently attain.
        //:
        //:   2 Additionally provide values, that would allow to test rounding.
        //:     (C-2..3)
        //:
        //: 2 For each row of the table, defined in P-1 invoke the
        //:   'calculateCapacity' class method and verify the returned value.
        //:   (C-1)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to call this function with
        //:   invalid parameters (using the 'BSLS_ASSERTTEST_*'
        //:   macros).  (C-4)
        //
        // Testing:
        //   static Uint64 calculateCapacity(drainRate, timeWindow);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'calculateCapacity'" << endl
                          << "===================" << endl;

        const Uint64 MAX_R = ULLONG_MAX;
        const Uint64 G     = 1000000000;

        // Numbers of units to be consumed during different intervals at
        // maximum rate.
        const Uint64 U_NS  = MAX_R / G;

        struct {
            Uint    d_line;
            Uint64  d_drainRate;
            Ti      d_timeWindow;
            Uint64  d_expectedCapacity;
        } DATA[] = {
        //  LINE    RATE             WINDOW              EXP_CAPACITY
        //  ----    -----    -----------------------    --------------
            {L_,    1000,    Ti(        2, 500000000),         2500},
            // C-1
            {L_,      10,    Ti(        2, 110000000),           21},
            // C-2
            {L_,       5,    Ti(        0,  10000000),            1},
            // C-3
            {L_,       1,    Ti(LLONG_MAX,         0),    LLONG_MAX},

            {L_, ULLONG_MAX, Ti(        1,         0),   ULLONG_MAX},
            {L_, ULLONG_MAX, Ti(        0,         1),         U_NS}
        };

        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

        for(int ti = 0; ti < NUM_DATA; ti++) {
            const Uint64 LINE              = DATA[ti].d_line;
            const Uint64 RATE              = DATA[ti].d_drainRate;
            const Ti     WINDOW            = DATA[ti].d_timeWindow;
            const Uint64 EXPECTED_CAPACITY = DATA[ti].d_expectedCapacity;

            LOOP_ASSERT(LINE, EXPECTED_CAPACITY ==
                                         Obj::calculateCapacity(RATE, WINDOW));
        }

        // C-4
        if (verbose) cout << endl << "Negative Testing" << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            ASSERT_FAIL(Obj::calculateCapacity(ULLONG_MAX, Ti(2)));
            ASSERT_FAIL(Obj::calculateCapacity(1000, Ti(-1)));
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // 'calculateTimeToSubmit'
        //   Ensure that 'calculateTimeToSubmit' calculates wait interval,
        //   taking reserved units into account and correctly updates object
        //   state in contractually specified case.
        //
        // Concerns:
        //: 1 The method calculates time till next submission correctly.
        //:
        //: 2 If the calculated interval is shorter than 1 nanosecond, it is
        //:   rounded to 1 nanosecond.
        //:
        //: 3 The method returns zero interval when something can be submitted
        //:   right now, does not update object state in this case.
        //:
        //: 4 If number of units in the bucket exceeds the capacity, the
        //:   method updates the 'lastUpdateTime' time and number of units in
        //:   the bucket.
        //:
        //: 5 The manipulator takes number of reserved units into account.
        //:
        //: 6 The manipulator does not affect the number of reserved
        //:   units.
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using table-driven technique:
        //:
        //:   1 Define the set of values, containing the values of 'rate' and
        //:     'capacity' attributes, number of units to be submitted and
        //:     reserved, initial lastUpdateTime, time of invoking the
        //:     'calculateTimeToSubmit' manipulator, expected time interval
        //:     before submitting more units and expected values of
        //:     'unitsInBucket' and 'lastUpdateTime' attributes after
        //:     'calculateTimeToSubmit' invocation.
        //:
        //: 2 For each row of the table described in P-1
        //:
        //:   1 Create an object having the specified parameters, using the
        //:     value constructor.
        //:
        //:   2 Submit and reserve specified number of units, using the
        //:     'submit' and 'reserve' manipulators.  (C-5)
        //:
        //:   3 Invoke the 'calculateTimeToSubmit' manipulator and verify
        //:     the returned time interval.  (C-1..3)
        //:
        //:   4 Verify the value of 'lastUpdateTime' and 'unitsInBucket'
        //:     attributes.  (C-4)
        //:
        //:   6 Verify the value of 'unitsReserved' attribute.  (C-6)
        //:
        //:   5 Invoke the 'wouldOverflow' manipulator and verify, that
        //:     after waiting for the calculated time submitting more
        //:     units is allowed.  Take into account that the number of
        //:     microseconds in the returned time interval is rounded
        //:     down.  (C-1)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-7)
        //
        // Testing:
        //   bsls::TimeInterval calculateTimeToSubmit(
        //                           const bsls::TimeInterval& currentTime);
        // --------------------------------------------------------------------

        if (verbose) cout <<
                         endl << "TESTING: 'calculateTimeToSubmit'" << endl
                              << "================================" << endl;

        const Uint64 M = 1000000;
        const Uint64 G = 1000000000;

        // C-1

        struct {
            int    d_line;
            Uint64 d_drainRate;
            Uint64 d_capacity;
            Uint64 d_unitsToSubmit;
            Uint64 d_unitsToReserve;
            Ti     d_creationTime;
            Ti     d_checkTime;
            Ti     d_expectedWait;
            Uint64 d_expectedUnits;
            Ti     d_expectedUpdate;
        } DATA[] = {

//  LINE RATE CAP    SUB  RSRV CREATE    CHECK     EXP_WAIT     EXP_U EXP_UPD_T
//  ---- ---- ----  ----- ---- -------   ------   -----------   ----- ---------
  // C-3
  { L_, 1000, 1000,    0,  0, Ti(  0),  Ti(0.5),  Ti(       0),    0, Ti(  0)},
  { L_, 1000, 1000, 1000,  0, Ti(  0),  Ti(  0),  Ti(   0.001), 1000, Ti(  0)},
  { L_, 1000, 1000, 2000,  0, Ti(  0),  Ti(0.5),  Ti(   0.501), 1500, Ti(0.5)},
  // C-2
  { L_, 10*M, 10*M, 10*M,  0, Ti(0.5),  Ti(0.5),  Ti(  0, 100), 10*M, Ti(0.5)},
  { L_, 10*G, 10*G, 10*G,  0, Ti(0.5),  Ti(0.5),  Ti(    0, 1), 10*G, Ti(0.5)},
  // C-5, C-6
  { L_,  100, 1500, 1000, 750, Ti(  0),  Ti(1.5),  Ti(    1.01), 850, Ti(1.5)},
  { L_, 1000,  500, 1000, 500, Ti(  0),  Ti(0.5),  Ti(   0.501), 500, Ti(0.5)}
        };
        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

        for(int ti = 0; ti < NUM_DATA; ti++) {
            const Uint64 LINE             = DATA[ti].d_line;
            const Uint64 UNITS_TO_SUBMIT  = DATA[ti].d_unitsToSubmit;
            const Uint64 UNITS_TO_RESERVE = DATA[ti].d_unitsToReserve;
            const Uint64 RATE             = DATA[ti].d_drainRate;
            const Uint64 CAPACITY         = DATA[ti].d_capacity;
            const Ti CREATION_TIME        = DATA[ti].d_creationTime;
            const Ti CHECK_TIME           = DATA[ti].d_checkTime;
            const Ti EXPECTED_WAIT        = DATA[ti].d_expectedWait;
            const Uint64 EXPECTED_UNITS   = DATA[ti].d_expectedUnits;
            const Ti     EXPECTED_UPDATE  = DATA[ti].d_expectedUpdate;

            Obj x(RATE, CAPACITY, CREATION_TIME);
            x.reserve(UNITS_TO_RESERVE);
            x.submit(UNITS_TO_SUBMIT);

            LOOP_ASSERT(LINE, EXPECTED_WAIT ==
                        x.calculateTimeToSubmit(CHECK_TIME));

            LOOP_ASSERT(LINE, EXPECTED_UPDATE  == x.lastUpdateTime());
            LOOP_ASSERT(LINE, EXPECTED_UNITS   == x.unitsInBucket());
            LOOP_ASSERT(LINE, UNITS_TO_RESERVE == x.unitsReserved());

            if (UNITS_TO_RESERVE < CAPACITY) {
                LOOP_ASSERT(LINE,
                            false ==
                                  x.wouldOverflow(CHECK_TIME + EXPECTED_WAIT));
            }
        }

        // C-7
        if (verbose) cout << endl << "Negative Testing" << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj mA(1, 1, Ti(0));

            const Ti TA = Ti(LLONG_MIN,     -999999999);
            const Ti TB = Ti(LLONG_MIN + 1, -999999999);

            mA.submit(10000);

            ASSERT_FAIL(mA.calculateTimeToSubmit(TA));
            ASSERT_PASS(mA.calculateTimeToSubmit(TB));

            const Ti MAX_TI = Ti(LLONG_MAX, 999999999);

            Obj mB(1, 1, MAX_TI);

            mB.submit(10000);

            ASSERT_FAIL(mB.calculateTimeToSubmit(TA));
            ASSERT_PASS(mB.calculateTimeToSubmit(Ti(0)));
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // 'calculateTimeToSubmit', CLASS METHOD 'calculateDrainTime'
        //   Ensure that the 'calculateTimeToSubmit' manipulator calculates the
        //   wait interval until next submission with nanosecond precision and
        //   'calculateDrainTime' class method calculates the time required to
        //   drain the specified number of units at the specified rate
        //   correctly.
        //
        // Concerns:
        //: 1 The 'calculateTimeToSubmit' manipulator calculates the wait
        //:   interval according to contractually specified behavior.
        //:
        //: 2 The 'calculateDrainTime' class method calculates the time
        //:   required to drain the specified number of units at the specified
        //:   rate correctly.
        //
        // Plan:
        //: 1 Using table-driven technique:
        //:
        //:   1 Define the set of values, containing the value of 'rate'
        //:     attribute, number of units, that must be drained before
        //:     submitting more units will be allowed and the expected time
        //:     interval before submitting more units.
        //:
        //: 2 For each row of the table described in P-1
        //:
        //:   1 Create an object having the capacity of 1 units and the
        //:     specified drain rate, using the value constructor.
        //:
        //:   2 Submit units, using the 'submit' manipulator.
        //:
        //:   3 Invoke the 'calculateTimeToSubmit' manipulator and verify the
        //:     returned time interval.  (C-1)
        //:
        //:   4 Invoke the 'calculateDrainTime' class method and verify the
        //:     returned time interval.  (C-2)
        //:
        //:   5 Invoke the 'wouldOverflow' manipulator and verify, that after
        //:     waiting for the calculated time submitting more units is
        //:     allowed.  Take into account that the number of microseconds in
        //:     the returned time interval is rounded down.
        //
        // Testing:
        //   calculateDrainTime(numOfUnits, drainRate, ceilFlag);
        //   bsls::TimeInterval calculateTimeToSubmit(currentTime);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'calculateTimeToSubmit'" << endl
                          << "=======================" << endl;

        const Uint64 MAX_R = ULLONG_MAX;
        const Uint64 Gb    = 1 << 30;
        const Uint64 G     = 1000000000;
        const Uint64 M     = 1000000;

        const Ti MNS_T(0, 999999999);

        // Numbers of units to be consumed during different intervals at
        // maximum rate.
        const Uint64 U_5NS  = (MAX_R / G) * 5 + ((MAX_R % G) * 5) / G;
        const Uint64 U_20NS = (MAX_R / G) * 20 + ((MAX_R % G) * 20) / G;
        const Uint64 U_MNS  = (MAX_R / G) * 999999999 +
                                                 ((MAX_R % G) * 999999999) / G;
        const Uint64 U_NS   = MAX_R / G;

        static const struct {
            int         d_line;
            Uint64      d_rate;
            Uint64      d_units;
            Ti          d_expTime;
        } DATA[] = {
         //  LINE     RATE       BACKLOG UNITS             EXP_TIME
         //  ----  ----------    ---------------   --------------------------
            { L_,   ULLONG_MAX,   ULLONG_MAX - 1,  Ti(          1,         0)},

            { L_,   ULLONG_MAX,   ULLONG_MAX -
              1000000,    Ti(          1,         0)},

            { L_,   ULLONG_MAX,   ULLONG_MAX -
              500000000,  Ti(          1,         0)},

            { L_,   ULLONG_MAX,   ULLONG_MAX -
              750000000,  Ti(          1,         0)},

            { L_,   ULLONG_MAX,                1,  Ti(          0,         1)},

            { L_,   100 * G +
              500 * M,      ULLONG_MAX,      Ti(  183549692, 275716932)},

            { L_,   100000 * G +
              500 * M,      ULLONG_MAX,      Ti(     184466, 518404504)},

            { L_,   ULLONG_MAX,   ULLONG_MAX -
              1000000000,      Ti(          1,         0)},

            { L_,   ULLONG_MAX,   ULLONG_MAX -
              U_NS - 2,        Ti(          0, 999999999)},

            { L_,   ULLONG_MAX,   ULLONG_MAX -
              U_NS - 1,        Ti(          0, 999999999)},

            { L_,   ULLONG_MAX,   ULLONG_MAX -
              U_NS + U_NS/2,   Ti(          1,         0)},

            { L_,   ULLONG_MAX,   ULLONG_MAX -
              U_NS + U_NS/2 +
              1,               Ti(          1,         0)},

            { L_,   ULLONG_MAX,   ULLONG_MAX -
              U_NS + U_NS/2 -
              1,               Ti(          1,         0)},

            { L_,   ULLONG_MAX,   U_NS,            Ti(          0,         1)},
            { L_,   ULLONG_MAX,   U_5NS,           Ti(          0,         5)},
            { L_,   ULLONG_MAX,   U_20NS,          Ti(          0,        20)},
            { L_,   ULLONG_MAX,   U_MNS,           Ti(          0, 999999999)},

            { L_,          150,           15,      Ti(          0, 100000000)},

            { L_,         1701,         1500,      Ti(          0, 881834216)},

            { L_,         150*G,        15*G,      Ti(          0, 100000000)},

            { L_,    ULLONG_MAX,  ULLONG_MAX -
              1 - U_5NS,       Ti(          0, 999999995)},

            { L_,          G+1,   ULLONG_MAX,     Ti(18446744055LL,262807560)},
            { L_,          G-1,   ULLONG_MAX,     Ti(18446744092LL,156295708)},
            { L_,            G,   ULLONG_MAX,     Ti(18446744073LL,709551615)},

            { L_,          Gb+1,  ULLONG_MAX,      Ti(17179869168LL,      14)},
            { L_,          Gb-1,  ULLONG_MAX,      Ti(17179869200LL,      14)},
            { L_,          Gb+1,     50*Gb+1,      Ti(         49, 999999955)},
            { L_,          Gb+1,     50*Gb+5,      Ti(         49, 999999959)},
            { L_,          Gb-1,     50*Gb+1,      Ti(         50,        48)},
            { L_,          Gb-1,     50*Gb+5,      Ti(         50,        52)},
            { L_,          Gb-42,    50*Gb+1,      Ti(         50,      1957)},
            { L_,          Gb+42,    50*Gb+5,      Ti(         49, 999998049)}
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE      = DATA[ti].d_line;
            const Uint64      RATE      = DATA[ti].d_rate;
            const Uint64      UNITS     = DATA[ti].d_units;
            const Ti          EXP_T     = DATA[ti].d_expTime;

            Obj x(RATE, 1, Ti(0));
            x.submit(UNITS);

            Ti t  = x.calculateTimeToSubmit(Ti(0));
            Ti dT = btls::LeakyBucket::calculateDrainTime(UNITS,
                                                         RATE,
                                                         true);

            LOOP_ASSERT(LINE, EXP_T == dT);
            LOOP_ASSERT(LINE, EXP_T == t);
            LOOP_ASSERT(LINE, false == x.wouldOverflow(t));
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'reset'
        //   Ensure that the 'reset' manipulator resets object to its initial
        //   state.
        //
        // Concerns:
        //: 1 The values of 'rate' and 'capacity attributes' is not affected
        //:   by the 'reset' method.
        //:
        //: 2 'reset' method resets the object to its default-constructed
        //:   state and sets 'lastUpdateTime' correctly.
        //:
        //: 3 'reset' method updates the value of
        //:   'statisticsCollectionStartTime' attribute and resets the
        //:   statistics counter.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Define the values of object parameters ('rate' and 'capacity'
        //:   attributes) that will be used throughout the test.
        //:
        //: 2 Using table-driven technique:
        //:
        //:   1 Define the set of values, containing the time of object
        //:     creation, number of units to be submitted and the time
        //:     of resetting object state, including the boundary values
        //:     corresponding to every range of values that each
        //:     individual attribute can independently attain.
        //:
        //: 3 For each row of the table described in P-2
        //:
        //:   1 Create an object with the specified parameters using the value
        //:     constructor.
        //:
        //:   2 Submit and reserve units, using the 'submit' and 'reserve'
        //:     manipulator.
        //:
        //:   3 Invoke the 'reset' manipulator.
        //:
        //:   4 Verify the object attributes that must not be affected by the
        //:     'reset' manipulator.  (C-1)
        //:
        //:   5 Verify the object attributes that are to be reset by the
        //:     'reset' manipulator.  (C-2..3)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   void reset(const bsls::TimeInterval& currentTime);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'reset'" << endl
                          << "=======" << endl;

        Uint64 usedUnits       = 0;
        Uint64 unusedUnits     = 0;
        const Uint64 CAPACITY(1000);
        const Uint64 RATE(1000);

        const Ti MAX_TI = Ti(LLONG_MAX,      999999999);
        const Ti MIN_TI = Ti(LLONG_MIN + 1, -999999999);

        struct {
            int    d_line;
            Ti     d_creationTime;
            Uint64 d_units;
            Ti     d_resetTime;
        } DATA[] = {
          //  LINE  CREATION TIME    UNITS    RESET TIME
          //  ----  -------------  ---------  ----------
            {  L_,         Ti( 0),         0,     Ti( 0) },
            {  L_,         Ti( 0),      1000,     Ti( 0) },
            {  L_,         Ti( 0),      2000,     Ti( 0) },
            {  L_,         Ti(50),         0,     Ti(60) },
            {  L_,         Ti(50),      1000,     Ti(60) },
            {  L_,         Ti(50),         0,     Ti( 0) },
            {  L_,         Ti(50),      1000,     Ti( 0) },

            {  L_,         Ti( 0), LLONG_MAX,     Ti( 0) },
            {  L_,         MAX_TI,      1000,     Ti( 0) },
            {  L_,         Ti( 0),      1000,     MAX_TI },
            {  L_,         MIN_TI,      1000,     Ti( 0) },
            {  L_,         Ti( 0),      1000,     MIN_TI },
        };
        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

        for(int ti = 0; ti < NUM_DATA; ti++) {
            const int    LINE          = DATA[ti].d_line;
            const Ti     CREATION_TIME = DATA[ti].d_creationTime;
            const Uint64 UNITS         = DATA[ti].d_units;
            const Ti     RESET_TIME    = DATA[ti].d_resetTime;

            Obj x(RATE, CAPACITY, CREATION_TIME);
            x.submit(UNITS);
            x.reserve(UNITS);

            x.updateState(RESET_TIME);
            x.reset(RESET_TIME);

            // C-1
            LOOP_ASSERT(LINE, RATE     == x.drainRate());
            LOOP_ASSERT(LINE, CAPACITY == x.capacity());

            // C-2
            LOOP_ASSERT(LINE, 0          == x.unitsInBucket());
            LOOP_ASSERT(LINE, 0          == x.unitsReserved());
            LOOP_ASSERT(LINE, RESET_TIME == x.lastUpdateTime());

            // C-3
            x.getStatistics(&usedUnits, &unusedUnits);

            LOOP_ASSERT(LINE, 0          == usedUnits);
            LOOP_ASSERT(LINE, 0          == unusedUnits);
            LOOP_ASSERT(LINE, RESET_TIME == x.statisticsCollectionStartTime());

        }

        // C-4
        if (verbose) cout << endl << "Negative Testing" << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj mA(1, 1, Ti(0));

            const Ti TA = Ti(LLONG_MIN,     -999999999);
            const Ti TB = Ti(LLONG_MIN + 1, -999999999);

            ASSERT_SAFE_FAIL(mA.reset(TA));
            ASSERT_SAFE_PASS(mA.reset(TB));
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // 'resetStatistics'
        //   Ensure that the 'resetStatistics' manipulator resets the object
        //   statistics to its default-constructed state.
        //
        // Concerns:
        //: 1 'resetStatistics' resets unit statistics counter to 0.
        //:
        //: 2 'resetStatistics' updates 'statisticsCollectionStartTime' time
        //:    correctly.
        //:
        //: 3 'resetStatistics' does not alter object state except for
        //:   submitted units counter and 'statisticsCollectionStartTime'
        //:   time.
        //
        // Plan:
        //: 1 Define the object parameters.
        //:
        //: 2 Create an object using the value constructor.
        //:
        //: 3 Submit some units and verify values returned by the
        //:   'getStatistics' accessor.
        //:
        //: 4 Invoke the 'resetStatistics' manipulator.
        //:
        //: 5 Invoke the 'getStatistics' accessor.  Verify returned values.
        //:   (C-1)
        //:
        //: 6 Verify value of the 'statisticsCollectionStartTime' attribute.
        //:   (C-2)
        //:
        //: 7 Verify the values of other object attributes ensure, that they
        //:   were not affected by the 'resetStatistics' manipulator.  (C-3)
        //
        // Testing:
        //   void resetStatistics();
        // --------------------------------------------------------------------

        if (verbose) cout <<
                         endl << "'resetStatistics'" << endl
                              << "=================" << endl;

        Uint64 usedUnits       = 0;
        Uint64 unusedUnits     = 0;
        const Ti CREATION_TIME = Ti(0.0);
        const Ti UPD_TIME      = Ti(0.75);
        const Uint UNITS       = 500;
        const Uint64 RATE      = 1000;
        const Uint64 CAPACITY  = 1000;
        const Uint64 EXP_USED  = UNITS;

        const Uint64 EXP_UNUSED = (Uint64) floor((
             UPD_TIME - CREATION_TIME).totalSecondsAsDouble()*RATE) - EXP_USED;

        Obj x(RATE, CAPACITY, CREATION_TIME);
        x.submit(UNITS);
        x.updateState(UPD_TIME);

        x.getStatistics(&usedUnits, &unusedUnits);
        ASSERT(EXP_USED      == usedUnits);
        ASSERT(EXP_UNUSED    == unusedUnits);
        ASSERT(CREATION_TIME == x.statisticsCollectionStartTime());

        x.submit(UNITS);
        x.resetStatistics();

        // C-1
        x.getStatistics(&usedUnits, &unusedUnits);
        ASSERT(0 == usedUnits);
        ASSERT(0 == unusedUnits);

        // C-2
        ASSERT(UPD_TIME == x.statisticsCollectionStartTime());

        // C-3
        ASSERT(RATE     == x.drainRate());
        ASSERT(CAPACITY == x.capacity());
        ASSERT(UPD_TIME == x.lastUpdateTime());
        ASSERT(UNITS    == x.unitsInBucket());
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // 'getStatistics'
        //   Ensure, that the 'getStatistics' method correctly calculate
        //   numbers of used and unused units.
        //
        // Concerns:
        //: 1 'getStatistics' returns 0 for a new object, created by value
        //:   CTOR.
        //:
        //: 2 'getStatistics' returns correct numbers of used and unused units
        //:   after a sequence of 'submit' and 'updateState' calls.
        //:
        //: 3 Specifying invalid parameters for 'getStatistics' causes certain
        //:   behavior in special build configuration.
        //:
        //: 4 Statistics is calculated for interval between
        //:   'statisticsCollectionStartTime' and 'lastUpdateTime'.
        //:
        //: 5 Statistics is calculated correctly, if time specified to
        //:   'updateState' precedes 'statisticsCollectionStartTime'.
        //
        // Plan:
        //: 1 Construct the object using the value constructor and verify the
        //:   values returned by the 'getStatistics' method.
        //:
        //: 2 Using table-driven technique:
        //:
        //:   1 Define the set of values, containing the 'rate' parameter,
        //:     number of units to submit at each iteration, interval between
        //:     'updateState' invocations, number of 'submit' and
        //:     'updateState' invocations and expected numbers of used and
        //:     unused units after the foregoing operations.
        //:
        //: 3 For each row of the table described in P-2
        //:
        //:   1 Create an object with the specified parameters.
        //:
        //:   2 Execute the inner loop, invoking 'submit' and 'updateState'
        //:     methods the specified number of times.
        //:
        //:   3 Invoke the 'getStatistics' method and verify the returned
        //:     numbers of used and unused units.
        //:
        //: 4 Create an object, submit some units, invoke the 'updateState'
        //:   manipulator several times and verify the values returned by the
        //:   'getStatistics' method between the 'updateState' invocations.
        //:
        //: 5 Create an object specifying lastUpdateTime 'T1', submit some
        //:   units, invoke the 'updateState' manipulator specifying
        //:   lastUpdateTime 'T2', that is before 'T1' and verify the values
        //:   returned by 'getStatistics'.  Invoke 'updateState' again,
        //:   specifying lastUpdateTime 'T3', that is after 'T2', verify the
        //:   values, returned by 'getStatistics'.
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks
        //:   are triggered for invalid parameters.
        //
        // Testing:
        //   void LeakyBucket::getStatistics(smtUnits, unusedUnits) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'getStatistics'" << endl
                          << "===============" << endl;

        Uint64 usedUnits   = 0;
        Uint64 unusedUnits = 0;

        const Uint64 MAX_R = ULLONG_MAX;
        const Uint64 G     = 1000000000;

        const Ti MNS_T(0, 999999999);

        // Numbers of units to be consumed during different intervals at
        // maximum rate.

        const Uint64 U_1S   = ULLONG_MAX;
        const Uint64 U_5NS  = (MAX_R / G) * 5 + ((MAX_R % G) * 5) / G;
        const Uint64 U_20NS = (MAX_R / G) * 20 + ((MAX_R % G) * 20) / G;
        const Uint64 U_MNS  = (MAX_R / G) * 999999999 +
            ((MAX_R % G) * 999999999) / G;
        const Uint64 U_NS   = MAX_R / G;

        // C-1
        if (verbose) cout
                         << endl
                         << "Testing: statistics after construction "
                         "using value ctor"
                         << endl;
        {
            Obj x(1000, 1000, Ti(42.4242));

            x.getStatistics(&usedUnits, &unusedUnits);
            ASSERT(0 == usedUnits);
            ASSERT(0 == unusedUnits);
        }

        // C-2
        if (verbose) cout << endl
                          << "Testing: statistics calculation"
                          << endl;
        {
            const Uint64 CAPACITY = 1000;

            struct {
                int    d_line;
                Uint64 d_drainRate;
                Uint64 d_units;
                Ti     d_creationTime;
                Ti     d_updateInterval;
                int    d_NumOfUpdates;
                Uint64 d_expectedUsed;
                Uint64 d_expectedUnused;
            } DATA[] = {

     //  LINE RATE   UNITS    TCREATE  UPDATE_INT N_UPD USED_UNITS UNUSED_UNITS
     //  ---- ----- -------   -------  ---------- ----- ---------- ------------
        {L_, 1000,  1000,    Ti( 0),  Ti( 0.01),  10,    10000,            0},
        {L_, 1000,   100,    Ti( 0),  Ti(  0.5),   5,      500,         2000},
        {L_, 100,      0,    Ti(10),  Ti(  0.1),  20,        0,          200},
        {L_, 1000,   500,    Ti( 0),  Ti(    0),   5,     2500,            0},
        // Testing operation at high speed
        {L_, MAX_R,    0,    Ti( 0),  Ti( 0, 1),   5,        0,        U_5NS},
        {L_, MAX_R,    0,    Ti( 0),      MNS_T,   1,        0,        U_MNS},
        {L_, MAX_R, 1000,    Ti( 0),  Ti( 0, 5),   4,     4000,      U_20NS -
         4000},
        {L_, MAX_R, U_1S,    Ti( 0),  Ti(    1),   1,     U_1S,            0},
        {L_, MAX_R, U_NS,    Ti( 0),  Ti(    1),   1,     U_NS,  U_1S - U_NS}
      };
      const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);
      for (int ti = 0; ti < NUM_DATA; ++ti) {
         const int    LINE            = DATA[ti].d_line;
         const Uint64 RATE            = DATA[ti].d_drainRate;
                const Uint64 UNITS           = DATA[ti].d_units;
                const Ti     CREATION_TIME   = DATA[ti].d_creationTime;
                const Ti     UPDATE_INTERVAL = DATA[ti].d_updateInterval;
                const int    NUM_OF_UPDATES  = DATA[ti].d_NumOfUpdates;
                const Uint64 EXPECTED_USED   = DATA[ti].d_expectedUsed;
                const Uint64 EXPECTED_UNUSED = DATA[ti].d_expectedUnused;

                Obj x(RATE, CAPACITY, CREATION_TIME);
                Ti currentTime(CREATION_TIME);

                for(int i = 0; i < NUM_OF_UPDATES; ++i) {
                    currentTime += UPDATE_INTERVAL;
                    x.submit(UNITS);
                    x.updateState(currentTime);
                }

                x.getStatistics(&usedUnits, &unusedUnits);
                LOOP_ASSERT(LINE, EXPECTED_USED == usedUnits);
                LOOP_ASSERT(LINE, EXPECTED_UNUSED == unusedUnits);

            }
        }

        // C-3
        if (verbose) cout << endl << "Negative Testing" << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj x(1, 1, Ti(0));

            ASSERT_SAFE_FAIL(x.getStatistics(0,&unusedUnits));
            ASSERT_SAFE_FAIL(x.getStatistics(&usedUnits,0));
            ASSERT_SAFE_FAIL(x.getStatistics(0,0));
        }

        // C-4
        if (verbose) cout << endl
                          << "Testing statistics collection interval"
                          << endl;
        {
            Obj x(1000, 1000, Ti(0));
            x.submit(1000);

            x.getStatistics(&usedUnits, &unusedUnits);
            ASSERT(0 == usedUnits);
            ASSERT(0 == unusedUnits);

            x.updateState(Ti(0.1));
            x.getStatistics(&usedUnits, &unusedUnits);
            ASSERT(1000 == usedUnits);
            ASSERT(0    == unusedUnits);

            x.updateState(Ti(10));
            x.getStatistics(&usedUnits, &unusedUnits);
            ASSERT(1000 == usedUnits);
            ASSERT(9000 == unusedUnits);

            x.resetStatistics();
            x.updateState(Ti(15));
            x.getStatistics(&usedUnits, &unusedUnits);
            ASSERT(0    == usedUnits);
            ASSERT(5000 == unusedUnits);
        }

        // C-5
        if (verbose) cout << endl
                          << "Testing statistics collection, "
                         "time goes backwards"
                          << endl;
        {
            Obj x(1000, 1000, Ti(10));

            x.submit(500);
            x.updateState(Ti(5));

            x.getStatistics(&usedUnits, &unusedUnits);
            ASSERT(500 == usedUnits);
            ASSERT(0   == unusedUnits);

            x.updateState(Ti(8));

            x.getStatistics(&usedUnits, &unusedUnits);
            ASSERT(500  == usedUnits);
            ASSERT(2500 == unusedUnits);
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // 'cancelReserved', 'submitReserved'
        //   Ensure that 'cancelReserved', 'submitReserved' manipulators
        //   correctly update 'unitsReserved' and 'unitsInBucket' attributes.
        //
        // Concerns:
        //: 1 'cancelReserved' decrements 'unitsReserved', without
        //:    affecting any other attributes.
        //:
        //: 2 'submitReserved' decrements 'unitsReserved' and increments
        //:   'unitsInBucket'
        //:
        //: 3 'submitReserved' submits units disregarding state of object.
        //:
        //: 4 QoI: Asserted precondition violations in the
        //:   'submitReserved' manipulator are detected when enabled.
        //
        // Plan:
        //: 1 Define the object parameters, that will be used throughout
        //:   the test.  The 'rate' and 'capacity' parameters do not affect
        //:   the behavior of 'submitReserved' and 'cancelReserved'
        //:   manipulators, so they are used for the whole test set.
        //:
        //: 2 Using the table-driven technique:
        //:
        //:   1 Define the set of values, containing number of units to be
        //:     reserved and the numbers of units to be submitted and
        //:     canceled from the reservation.
        //:
        //: 3 For each row in the table, defined in P-2:
        //:
        //:   1 Create an object having the specified parameters.
        //:
        //:   2 Reserve the specified number of units,
        //:
        //:   3 Invoke the 'submitReserved' and 'cancelReserved'
        //:     manipulators with the specified numbers of units to be
        //:     submitted and canceled.
        //:
        //:   4 Verify values of the 'unitsReserved' and 'unitsSubmitted'
        //:     attributes.  (C-1..3)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks
        //:   are triggered for invalid attribute values, but not
        //:   triggered for adjacent valid ones (using the
        //:   'BSLS_ASSERTTEST_*' macros).  (C-4)
        //
        // Testing:
        //    void submitReserved(bsls::Types::Unit64 numOfUnits);
        //    void cancelReserved(bsls::Types::Unit64 numOfUnits);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'submitReserved', 'cancelReserved'" << endl
                          << "==================================" << endl;

        const Uint64 RATE     = 1000;
        const Uint64 CAPACITY = 1000;
        const Uint64 MAX_U    = ULLONG_MAX;

        struct {
            int    d_line;
            Uint64 d_unitsToReserve;
            Uint64 d_unitsToSubmit;
            Uint64 d_unitsToCancel;
            Uint64 d_expectedUnitsReserved;
            Uint64 d_expectedUnitsInBucket;
        } DATA[] = {

         //  LINE  RESERVE   SUBMIT  CANCEL  EXP_RES   EXP_SUB
         //  ----  -------  -------  ------- --------  ---------

            {L_,    1000,     300,     0,       700,     300},
            {L_,    1000,     500,   500,         0,     500},

            {L_,   MAX_U,       0,     0,     MAX_U,       0},
            {L_,   MAX_U,   MAX_U,     0,         0,   MAX_U},
            {L_,   MAX_U, MAX_U/2, MAX_U/2,       1, MAX_U/2},
        };

        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

        for(int ti=0; ti<NUM_DATA; ti++) {
            const Uint64 LINE             = DATA[ti].d_line;
            const Uint64 UNITS_TO_RESERVE = DATA[ti].d_unitsToReserve;
            const Uint64 UNITS_TO_SUBMIT  = DATA[ti].d_unitsToSubmit;
            const Uint64 UNITS_TO_CANCEL  = DATA[ti].d_unitsToCancel;
            const Uint64 EXPECTED_UNITS_RESERVED =
                                              DATA[ti].d_expectedUnitsReserved;
            const Uint64 EXPECTED_UNITS_IN_BUCKET =
                                              DATA[ti].d_expectedUnitsInBucket;

            Obj x(RATE, CAPACITY, Ti(0));
            x.reserve(UNITS_TO_RESERVE);

            x.submitReserved(UNITS_TO_SUBMIT);
            x.cancelReserved(UNITS_TO_CANCEL);

            LOOP_ASSERT(LINE, EXPECTED_UNITS_RESERVED  == x.unitsReserved());
            LOOP_ASSERT(LINE, EXPECTED_UNITS_IN_BUCKET == x.unitsInBucket());
        }

        // C-3
        Obj x(1000, 1000, Ti(0));

        x.reserve(1500);
        x.submit(1200);

        ASSERT(1200 == x.unitsInBucket());
        ASSERT(1500 == x.unitsReserved());

        x.submitReserved(1000);

        ASSERT(2200 == x.unitsInBucket());
        ASSERT(500  == x.unitsReserved());

        // C-4
        if (verbose) cout << endl << "Negative Testing" <<endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj y(CAPACITY, RATE, Ti(0));
            y.reserve(1000);

            ASSERT_SAFE_FAIL(y.cancelReserved(1001));
            ASSERT_SAFE_PASS(y.cancelReserved(1000));

            Obj z(CAPACITY, RATE, Ti(0));
            z.reserve(1000);

            ASSERT_SAFE_FAIL(z.submitReserved(1001));
            ASSERT_SAFE_PASS(z.submitReserved(1000));
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // 'wouldOverflow', 'submit', 'reserve' call sequence
        //   Ensure that 'wouldOverflow', 'submit', 'reserve' manipulators
        //   operate correctly, once invoked sequentially.
        //
        // Concerns:
        //: 1 'wouldOverflow', 'submit', 'reserve' methods operate
        //:    correctly, once invoked sequentially.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Define the set of values, the row per each test case,
        //:     containing the values for 'rate', 'capacity' and
        //:     'lastUpdateTime' attributes, numbers of units to submit and
        //:     reserve, the time interval between checking, whether
        //:     submitting more units is allowed at current time, the expected
        //:     number of allowed 'submit' operations and the expected number
        //:     of units, after performing the foregoing number of 'submit'
        //:     invocations.
        //:
        //: 2 For each row in the table, defined in P-1:
        //:
        //:   1 Create an object having specified parameters using the value
        //:     constructor.
        //:
        //:   2 Invoke the 'reserve' manipulator to reserve the specified
        //:     number of units.
        //:
        //:   3 Execute in a loop until 'wouldOverflow' manipulator will
        //:     return 'true', and invoke the 'submit' manipulator.  Increment
        //:     the time that is passed to 'wouldOverflow' on each iteration
        //:     to simulate time flow.  (C-1)
        //:
        //:   4 Verify the value of 'unitsInBucket' attributes and the actual
        //:     number of loop iterations.
        //
        // Testing:
        //    bool wouldOverflow(currentTime);
        //    void submit(bsls::Types::Unit64 numOfUnits);
        //    void reserve(bsls::Types::Unit64 numOfUnits);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "'wouldOverflow', 'submit', 'reserve' call sequence"
                 << endl
                 << "=================================================="
                 << endl;
        {

            struct {
                int    d_line;
                Uint64 d_drainRate;
                Uint64 d_capacity;
                Uint64 d_unitsToSubmit;
                Uint64 d_unitsToReserve;
                Ti     d_creationTime;
                Ti     d_checkInterval;
                int    d_expectedNumOfSubmits;
                Uint64 d_expectedFinalUnits;
            } DATA[] = {

       //  LINE RATE1 CAPACITY   SUBMIT RSRV TCREATE  CHECK_INT  # SUBMIT EXP_U
       //  ---- ----- --------   ------ ---- -------  ---------- -------- -----
          {L_, 1000,    500,      100,  50, Ti(0),   Ti(   0),     5,     500},
          {L_, 1000,   1000,      300, 150, Ti(0),   Ti( 0.1),     5,    1000},
          {L_,   10,     10,        1,   0, Ti(10),  Ti(0.01),     11,     10}
            };
            const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

            for(int ti=0; ti<NUM_DATA; ti++) {
                const Uint64 LINE             = DATA[ti].d_line;
                const Uint64 UNITS            = DATA[ti].d_unitsToSubmit;
                const Uint64 UNITS_TO_RESERVE = DATA[ti].d_unitsToReserve;
                const Uint64 RATE             = DATA[ti].d_drainRate;
                const Uint64 CAPACITY         = DATA[ti].d_capacity;
                const Ti     CREATION_TIME    = DATA[ti].d_creationTime;
                const Ti     CHECK_INTERVAL   = DATA[ti].d_checkInterval;

                const Uint64 EXPECTED_NUM_OF_SUBMITS
                                             = DATA[ti].d_expectedNumOfSubmits;
                const Uint64 EXPECTED_FINAL_UNITS
                                             = DATA[ti].d_expectedFinalUnits;

                Obj x(RATE, CAPACITY, CREATION_TIME);
                Ti  currentCheck(CREATION_TIME);
                unsigned int i = 0;
                x.reserve(UNITS_TO_RESERVE);

                while(!x.wouldOverflow(currentCheck)) {

                    x.submit(UNITS);
                    currentCheck += CHECK_INTERVAL;
                    i++;
                }

                LOOP3_ASSERT(LINE, EXPECTED_NUM_OF_SUBMITS, i,
                             EXPECTED_NUM_OF_SUBMITS == i);
                LOOP3_ASSERT(LINE, EXPECTED_FINAL_UNITS, x.unitsInBucket(),
                             EXPECTED_FINAL_UNITS == x.unitsInBucket());
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // 'wouldOverflow'
        //   Ensure that manipulator returns the correct result based on the
        //   specified parameters and current object state and correctly
        //   updates the state of object in the contractually specified cases.
        //
        // Concerns:
        //: 1 The method returns true, if there is room for the specified
        //:   number of units and false otherwise.
        //:
        //: 2 The method invokes 'updateState', if needed.
        //:
        //: 4 The method does not change the state of object, if the time has
        //:   not changed.
        //:
        //: 5 If specified time is before last update time, 'wouldOverflow'
        //:   updates 'lastUpdateTime' time and does not recalculate number of
        //:   units.
        //:
        //: 6 The method takes reserved units into account.
        //:
        //: 7 The method correctly handles the case when 'numOfUnits +
        //:   d_unitsInBucket + d_unitsReserved' can not be represented by 64
        //:   bit integral type.
        //:
        //: 8 QoI: Asserted precondition violations in the 'wouldOverflow'
        //:    manipulator are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Define the set of values, one row per each test case,
        //:     containing the values for 'rate', 'capacity', initial value
        //:     for the 'lastUpdateTime' attribute, and numbers of units to
        //:     submit and reserve, the time interval between updating object
        //:     state, number of units to be submitted and to be reserved, the
        //:     time of check, result of check and the expected values of
        //:     'unitsInBucket' and 'lastUpdateTime' attributes after
        //:     checking.
        //:
        //: 2 For each row in the table, defined in P-1:
        //:
        //:   1 Create an object, having the specified parameters using the
        //:     value constructor.
        //:
        //:   2 Invoke the 'submit' and 'reserve' manipulators.
        //:
        //:   3 Invoke the 'wouldOverflow' manipulator and verify the returned
        //:     value.  (C-1, C-6)
        //:
        //:   4 Verify the values of 'unitsInBucket', 'unitsReserved' and
        //:     'lastUpdateTime' attributes.  (C-2..5, C-7)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-8)
        //
        // Testing:
        //    bool wouldOverflow(numOfUnits, currentTime);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'wouldOverflow'" << endl
                          << "===============" << endl;

        // C-1, C-2
        {
            const Uint64 MAX_R = ULLONG_MAX;
            const Uint64 MAX_U = ULLONG_MAX;

            const Uint64 EU_C6 = MAX_U - 250;
            const Uint64 U_C7  = MAX_U / 2;
            const Uint64 EU_C7 = U_C7 - 1000;

            // Number of units, drained during 1 ns at maximum allowed rate

            const Uint64 U_NS = MAX_U / 1000000000;

            // Number of units, drained during 1 ms at maximum allowed rate

            const Uint64 U_MS = MAX_U / 1000;

            const Ti MAX_T(LLONG_MAX, 999999999);

            struct {
                int    d_line;
                Uint64 d_drainRate;
                Uint64 d_capacity;
                Uint64 d_unitsToSubmit;
                Uint64 d_unitsToReserve;
                Ti     d_creationTime;
                Ti     d_checkTime;
                bool   d_checkResult;
                Uint64 d_expectedUnits;
                Ti     d_expectedUpdate;
            } DATA[] = {

// LINE RATE  CAP   SUBMIT RSRV CREATE   CHECK    CHK_RES EXP_U EXP_UPD
// ---- ----- ----- ------ ---- -------  ------   ------- ----- -------
   {L_, 1000, 1000,    0,   0,  Ti(0),   Ti(  0),  false,    0, Ti(  0)},
   {L_, 1000, 1000, 1000, 500,  Ti(0),   Ti(  0),   true, 1000, Ti(  0)},
   // C-4
   {L_, 1000, 1000, 1000, 500,  Ti(0),   Ti(  0),   true, 1000, Ti(  0)},
   {L_, 1000, 1000,    0,   0,  Ti(0),   Ti(  0),  false,    0, Ti(  0)},
   {L_, 1000, 1000,  500, 250,  Ti(0),   Ti(  1),  false,    0, Ti(  1)},
   {L_, 1000, 1000, 1000, 500,  Ti(0),   Ti(  1),  false,    0, Ti(  1)},
   {L_,  500, 1000,  500, 250,  Ti(0),   Ti(0.5),  false,  250, Ti(0.5)},
   // C-5
   {L_, 1000, 1000, 1000, 500,  Ti(5),   Ti(0.5),   true, 1000, Ti(0.5)},
   // C-7
   {L_, 1000, 1000, MAX_U,    0, Ti(0),  Ti(0.25), true, EU_C6, Ti(0.25)},
   {L_, 1000, 1000,  U_C7, U_C7, Ti(0),  Ti(   1), true, EU_C7, Ti(   1)},

   // Testing operation at high rate
   {L_, MAX_R, 1000, MAX_U - 500, 500, Ti(0), Ti(0, 1),
    true, MAX_U - 500 - U_NS, Ti( 0,1)},
   {L_, MAX_R, 1000, MAX_U - 500,   500, Ti(0), Ti(0, 1),
    true, MAX_U - 500 - U_NS, Ti( 0,1)},
   {L_, MAX_R, 1000, MAX_U - 500, 500, Ti(0), Ti(0.001),
    true, MAX_U - 500 - U_MS, Ti(0.001)},
   {L_, MAX_R, 1000, MAX_U - 500, 500, Ti(0), MAX_T,
    false,  0, MAX_T}
            };

            const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

            for(int ti = 0; ti < NUM_DATA; ti++) {
                const Uint64 LINE             = DATA[ti].d_line;
                const Uint64 UNITS_TO_SUBMIT  = DATA[ti].d_unitsToSubmit;
                const Uint64 UNITS_TO_RESERVE = DATA[ti].d_unitsToReserve;
                const Uint64 RATE             = DATA[ti].d_drainRate;
                const Uint64 CAPACITY         = DATA[ti].d_capacity;
                const Ti     CREATION_TIME    = DATA[ti].d_creationTime;
                const bool   RESULT           = DATA[ti].d_checkResult;
                const Ti     CHECK_TIME       = DATA[ti].d_checkTime;
                const Uint64 EXPECTED_UNITS   = DATA[ti].d_expectedUnits;
                const Ti     EXPECTED_UPDATE  = DATA[ti].d_expectedUpdate;

                Obj x(RATE, CAPACITY, CREATION_TIME);
                x.submit(UNITS_TO_SUBMIT);

                // C-6
                x.reserve(UNITS_TO_RESERVE);

                LOOP_ASSERT(LINE,
                            RESULT == x.wouldOverflow(CHECK_TIME));
                LOOP_ASSERT(LINE, EXPECTED_UNITS    == x.unitsInBucket());
                LOOP_ASSERT(LINE, EXPECTED_UPDATE   == x.lastUpdateTime());
                LOOP_ASSERT(LINE, UNITS_TO_RESERVE  == x.unitsReserved());
            }
        }

        // C-8
        if (verbose) cout << endl << "Negative Testing" << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj mA(1, 1, Ti(0));

            const Ti TA = Ti(LLONG_MIN,     -999999999);
            const Ti TB = Ti(LLONG_MIN + 1, -999999999);

            ASSERT_FAIL(mA.wouldOverflow(TA));
            ASSERT_PASS(mA.wouldOverflow(TB));

            const Ti MAX_TI = Ti(LLONG_MAX, 999999999);

            Obj mB(1, 1, MAX_TI);

            ASSERT_FAIL(mB.wouldOverflow(TA));
            ASSERT_PASS(mB.wouldOverflow(Ti(0)));
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // 'updateState'
        //   Ensure that manipulator correctly updates the state of object
        //   based on the specified time, and current state of object.
        //
        // Concerns:
        //: 1 The 'updateState' manipulator sets 'lastUpdateTime' attribute to
        //:   the specified value.
        //:
        //: 2 The 'updateState' manipulator calculates the number of units to
        //:   be drained from the leaky bucket according to the contractually
        //:   specified behavior.
        //:
        //: 3 The manipulator correctly handles case, when number of units to
        //:   drain is fractional and carries the fractional part to next
        //:   'updateState' call.
        //:
        //: 4 If the specified time is before the last update time,
        //:   'updateState' updates the last update time and does not
        //:   recalculate number of units.
        //:
        //: 5 The manipulator does not affect value of the 'unitsReserved'
        //:   attribute.
        //:
        //: 6 The manipulator updates value of the
        //:   'statisticsCollectionStartTime' attribute if the specified time
        //:   is before its current value and does not affect it otherwise.
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Define the 'capacity' attribute value, that will be used
        //:   throughout the test.  It is constant, because it does not affect
        //:   the behavior of 'updateState' manipulator.
        //:
        //: 2 Using the table-driven technique:
        //:
        //:   1 Define the set of values, the row per each test case,
        //:     containing the values for 'rate' and 'lastUpdateTime'
        //:     attributes and numbers of units to submit and reserve, the
        //:     time interval between updating object state, number of
        //:     'updateState' invocations and the value of 'unitsInBucket'
        //:     attribute, expected after a sequence of 'updateState'
        //:     invocations.  (C-2, C-3)
        //:
        //: 3 For each row in the table, defined in P-2:
        //:
        //:   1 Create an object having the specified parameters using the
        //:     value ctor.
        //:
        //:   2 Invoke 'submit' and 'reserve' methods to alter the state of
        //:     object.
        //:
        //:   3 Execute the inner loop, invoking the 'updateState' manipulator
        //:     specified number of times with the specified time intervals
        //:     and verify that the value of 'lastUpdateTime' attribute is
        //:     updated correctly.
        //:
        //:   4 Compare the value, returned by the 'unitsInBucket' with the
        //:     expected value.
        //:
        //:   5 Verify, that the value of 'unitsReserved' attribute was not
        //:     altered during the 'updateState' manipulator invocation.
        //:     (C-5)
        //:
        //: 4 Invoke 'updateState', specifying time that is before the value
        //:   of 'lastUpdateTime' attribute.  (C-4)
        //:
        //: 5 Invoke 'updateState', specifying time that is before the value
        //:   of 'statisticsCollectionStartTime' attribute.  (C-6)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-7)
        //
        // Testing:
        //   void updateState(const bsls::TimeInterval& currentTime);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'updateState'" << endl
                          << "=============" << endl;

        const Uint64 CAPACITY(1000);
        const Uint64 MAX_RATE = ULLONG_MAX;
        const Ti     MAX_TI   = bsls::TimeInterval(LLONG_MAX, 999999999);
        const Uint64 G        = 1000000000;

        // Number of units remaining in bucket with maximum allowed drain rate,
        // after 3 updates with 1ns interval.
        const Uint64 MR_EXP = MAX_RATE -
                                      (MAX_RATE / 1000000000) * 3 -
                                      (MAX_RATE % 1000000000) * 3 / 1000000000;

        const Uint64 MI_EXP = Uint64(LLONG_MAX) + 1;

        // C-1, C-2
        if (verbose) cout << endl << "Testing 'updateState'" << endl;
        {
            // C-2, C-5
            struct {
                int    d_line;
                Uint64 d_drainRate;
                Uint64 d_unitsToSubmit;
                Uint64 d_unitsToReserve;
                Ti     d_creationTime;
                Ti     d_drainInterval;
                int    d_NumOfDrains;
                Uint64 d_expectedUnits;
            } DATA[] = {
      // LINE  RATE     SUB      RSRV  TCREATE  DRAIN_INT    DRAIN EXP_UNITS
      // ----  -------- -------  ----  -------  ----------   ----- ---------
        {L_,     1000,      1000,    0,  Ti( 0),  Ti(  0.01),   10,    900},
        {L_,     1000,      1500,    0,  Ti( 0),  Ti(   0.5),    5,      0},
        {L_,       10,        10,    0,  Ti(10),  Ti(  0.16),    1,      9},
        {L_,       10,        10,    0,  Ti(10),  Ti(  0.16),    2,      7},
        {L_,        1,        10,    0,  Ti(10),  Ti( 0.999),    1,     10},
        {L_,        1,        10,    0,  Ti(10),  Ti(   0.3),    3,     10},
        {L_,        1,        10,    0,  Ti(10),  Ti(   0.3),    4,      9},
        {L_,        1,        10,    0,  Ti(10),  Ti(     0),   20,     10},
        {L_,      100,      1000,  500,  Ti( 0),  Ti(   1.5),    3,    550},
         // Checking operation at maximum allowed rate.
         {L_, MAX_RATE,  MAX_RATE,    0,  Ti( 0),  Ti( 0, 1),     3, MR_EXP},
        {L_, MAX_RATE,      1000, 1000,  Ti( 0),  Ti( 0.001),    1,      0},
        {L_, MAX_RATE,  MAX_RATE,    0,  Ti( 0),  Ti( 0.001), 1000,      0},
        {L_, MAX_RATE,  MAX_RATE,    0,  Ti( 0),  Ti(    10),    1,      0},
         // Checking operation at minimum allowed rate.
        {L_,        1,  LLONG_MAX,   0,  Ti( 0),      MAX_TI,       1,      0},
        {L_,        1, ULLONG_MAX,   0,  Ti( 0),      MAX_TI,       1, MI_EXP},
        {L_,        1,          1,   0,  Ti( 0),  Ti(0, 500),  G/500 -
         1,           1},
        {L_,        1,          1,   0,  Ti( 0),  Ti(0, 500),  G/500,       0},
        // C-3  Checking operation with short update intervals.
        {L_,  1000000,      1000, 1000,  Ti( 0),  Ti(  0, 1),  999,   1000},
        {L_,  1000000,      1000, 1000,  Ti( 0),  Ti(  0, 1), 1000,    999},
        {L_,  1000000,      1000, 1000,  Ti( 0),  Ti(  0, 1), 2000,    998}
            };

            const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE             = DATA[ti].d_line;
                const Uint64 RATE             = DATA[ti].d_drainRate;
                const Uint64 UNITS_TO_SUBMIT  = DATA[ti].d_unitsToSubmit;
                const Uint64 UNITS_TO_RESERVE = DATA[ti].d_unitsToReserve;
                const Ti     CREATION_TIME    = DATA[ti].d_creationTime;
                const Ti     DRAIN_INTERVAL   = DATA[ti].d_drainInterval;
                const int    NUM_OF_DRAINS    = DATA[ti].d_NumOfDrains;
                const Uint64 EXPECTED_UNITS   = DATA[ti].d_expectedUnits;

                Obj x(RATE, CAPACITY, CREATION_TIME);
                x.reserve(UNITS_TO_RESERVE);
                x.submit(UNITS_TO_SUBMIT);
                Ti currentTime(CREATION_TIME);

                for(int i = 0; i < NUM_OF_DRAINS; ++i) {
                    currentTime += DRAIN_INTERVAL;
                    x.updateState(currentTime);

                    // C-1
                    LOOP_ASSERT(LINE, x.lastUpdateTime() == currentTime);
                }

                LOOP_ASSERT(LINE, x.unitsInBucket() == EXPECTED_UNITS);

                // C-5
                LOOP_ASSERT(LINE, x.unitsReserved() == UNITS_TO_RESERVE);

                // C-6
                LOOP_ASSERT(LINE,
                            x.statisticsCollectionStartTime()
                                                             == CREATION_TIME);
            }
        }

        // C-4
        if (verbose) cout << endl
                          << "Testing 'updateState', time goes backwards"
                          << endl;
        {
            const Ti  CURRENT_TIME(10);
            Ti        UPDATE_TIME(1.0);

            Obj x(1000, CAPACITY, CURRENT_TIME);
            x.submit(1000);

            ASSERT(CURRENT_TIME == x.lastUpdateTime());

            x.updateState(UPDATE_TIME);
            ASSERT(UPDATE_TIME == x.lastUpdateTime());
            ASSERT(1000 == x.unitsInBucket());

            // C-6
            ASSERT(UPDATE_TIME == x.statisticsCollectionStartTime());
        }

        // C-5
        if (verbose) cout << endl
                          << "Testing 'updateState' with reservation"
                          << endl;
        {
            Obj x(1000, CAPACITY, Ti(1.0));
            x.reserve(500);
            x.submit(1000);

            x.updateState(Ti(-1.0));
            ASSERT(500 == x.unitsReserved());
            x.updateState(Ti(10.0));
            ASSERT(500 == x.unitsReserved());
        }

        // C-7
        if (verbose) cout << endl << "Negative Testing" << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj mA(1, 1, Ti(0));

            const Ti TA = Ti(LLONG_MIN,     -999999999);
            const Ti TB = Ti(LLONG_MIN + 1, -999999999);

            ASSERT_FAIL(mA.updateState(TA));
            ASSERT_PASS(mA.updateState(TB));

            const Ti MAX_TI = Ti(LLONG_MAX, 999999999);

            Obj mB(1, 1, MAX_TI);

            ASSERT_FAIL(mB.updateState(TA));
            ASSERT_PASS(mB.updateState(Ti(0)));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // 'setRateAndCapacity'
        //   Ensure that 'rate' and 'capacity' attributes may be set without
        //   altering object state.
        //
        // Concerns:
        //: 1 'setRateAndCapacity' manipulator can set the relevant attributes
        //:   to any values that does not violate the documented
        //:   preconditions.
        //:
        //: 2 Invoking the 'setRateAndCapacity' manipulator does not alter the
        //:   values of 'unitsReserved' and 'unitsSubmitted' attributes.
        //:
        //: 3 QoI: Asserted precondition violations in the 'submit'
        //:   manipulator are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Define the set of object attributes, including boundary
        //:     values, corresponding to every range of values that each
        //:     individual attribute can independently attain.
        //:
        //: 2 For each row in the table, defined in P-1:
        //:
        //:   1 Create an object using the value ctor, having a drain rate of 1
        //:     and capacity of 1.  Use 'setRateAndCapacity' to set the drain
        //:     rate and capacity to the values of 'RATE1' and 'CAPACITY1'
        //:     columns.  Verify that the drain rate and capacity have been
        //:     correctly updated.  (C-1)
        //
        //:   2 Create another object using the value ctor, having the
        //:     parameters specified in 'RATE1' and 'CAPACITY1' columns.
        //:
        //:   3 Invoke 'submit' and 'reserve' methods to alter the state of
        //:     object.
        //:
        //:   4 Invoke the 'setRateAndCapacity' manipulator with the arguments
        //:     specified in 'RATE2' and 'CAPACITY2' columns.  Verify that the
        //:     drain rate and capacity have been correctly updated.  (C-2)
        //:
        //:   5 Verify that the values of 'unitsInBucket' and 'unitsReserved'
        //:     attributes were not altered by the previous
        //:     'setRateAndCapacity' invocation.  (C-2)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-3)
        //
        // Testing:
        //    void setRateAndCapacity(newRate, newCapacity);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'setRateAndCapacity'" << endl
                          << "====================" << endl;

        const Uint64 MAX_RATE = ULLONG_MAX;
        const Uint64 MAX_CAP  = ULLONG_MAX;

        const Ti MAX_T(LLONG_MAX, 999999999);

        if (verbose)
            cout << "\nUsing a table of distinct object values." << endl;

        // C-1
        struct {
            int    d_line;
            Uint64 d_rate1;
            Uint64 d_capacity1;
            Uint64 d_units;
            Ti     d_creationTime;
            Uint64 d_rate2;
            Uint64 d_capacity2;
        } DATA[] = {
        // LINE    RATE1  CAPACITY1       UNITS     TCREATE    RATE2     CAP_2
        // ----   ------- ----------    ----------- -------   -------- --------
          {L_,     1000,      1000,             0, Ti(10),       500,       1},
          {L_,     1000,         1,            42, Ti(10),       500,     500},
          {L_, MAX_RATE,      1000,          1000, Ti( 1),         1,       1},
          {L_,        1,   MAX_CAP,          1000, Ti( 2),         1,       1},
          {L_,        1,      1000,     MAX_CAP/2, Ti( 3),         1,       1},
          {L_,        1,      1000,          1000,  MAX_T,         1,       1},
          {L_,        1,      1000,          1000, Ti( 4),  MAX_RATE,       1},
          {L_,        1,      1000,          1000, Ti( 5),         1, MAX_CAP},
          {L_,     1000,         1,     MAX_CAP/2, Ti( 0),  MAX_RATE, MAX_CAP}
        };
        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE            = DATA[ti].d_line;
            const Uint64 RATE1           = DATA[ti].d_rate1;
            const Uint64 CAPACITY1       = DATA[ti].d_capacity1;
            const Uint64 UNITS_SUBMITTED = DATA[ti].d_units;
            const Uint64 UNITS_RESERVED  = DATA[ti].d_units/2;
            const Ti     CREATION_TIME   = DATA[ti].d_creationTime;
            const Uint64 RATE2           = DATA[ti].d_rate2;
            const Uint64 CAPACITY2       = DATA[ti].d_capacity2;

            {
                Obj x(1, 1, CREATION_TIME);
                x.setRateAndCapacity(RATE1,CAPACITY1);

                LOOP_ASSERT(LINE, RATE1           == x.drainRate());
                LOOP_ASSERT(LINE, CAPACITY1       == x.capacity());
                LOOP_ASSERT(LINE, CREATION_TIME   == x.lastUpdateTime());
            }

            {
                Obj x(RATE1,CAPACITY1,CREATION_TIME);
                x.submit(UNITS_SUBMITTED);
                x.reserve(UNITS_RESERVED);

                x.setRateAndCapacity(RATE2,CAPACITY2);

                LOOP_ASSERT(LINE, RATE2           == x.drainRate());
                LOOP_ASSERT(LINE, CAPACITY2       == x.capacity());
                LOOP_ASSERT(LINE, CREATION_TIME   == x.lastUpdateTime());

                LOOP_ASSERT(LINE, UNITS_SUBMITTED == x.unitsInBucket());
                LOOP_ASSERT(LINE, UNITS_RESERVED  == x.unitsReserved());
            }
        }

        if (verbose) cout << endl << "Negative Testing" <<endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                bsls::AssertTest::failTestDriver);
            Obj x(1, 1, Ti(0));

            ASSERT_SAFE_FAIL(x.setRateAndCapacity(0, 1));
            ASSERT_SAFE_FAIL(x.setRateAndCapacity(1, 0));
            ASSERT_SAFE_FAIL(x.setRateAndCapacity(0, 0));

            ASSERT_SAFE_PASS(x.setRateAndCapacity(1, 1));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // 'submit', 'reserve'
        //   Ensure that 'unitsInBucket' and 'unitsReserved' attributes can be
        //   altered and accessed correctly.
        //
        // Concerns:
        //: 1 'submit' adds the specified number of units to the value of
        //:   'unitsInBucket' attribute.
        //:
        //: 2 'unitsInBucket' accessor returns the attribute value.
        //:
        //: 3 'submit' submits units disregarding current state of object
        //:    (number of units already submitted).
        //:
        //: 4 'submit' can submit number of units, exceeding the capacity.
        //:
        //: 5 QoI: Asserted precondition violations in the 'submit'
        //:   manipulator are detected when enabled.
        //:
        //: 6 'reserve' add units to the object`s internal reservation
        //:    counter.
        //:
        //: 7 'unitsReserved' accessor returns the attribute value
        //:
        //: 8 'reserve' adds the specified number of units to the value of
        //:    'unitsInBucket' attribute.
        //:
        //: 9 'reserve' can reserve the number of units, exceeding the
        //:    capacity.
        //:
        //: 10 QoI: Asserted precondition violations in the 'reserve'
        //:    manipulator are detected when enabled.
        //
        // Plan:
        //: 1 Define the constant object parameters ('capacity' and 'rate').
        //:   These parameters will be used throughout the whole test, because
        //:   they do not affect the behavior of tested methods.
        //:
        //: 2 Using the table-driven technique:
        //:
        //:   1 Define the set of values, one row per set, containing number
        //:     of units to submit or reserve, number of 'submit' or 'reserve'
        //:     invocations and the expected number of units submitted and
        //:     reserved after the specified number of invocations of the
        //:     foregoing methods.  Include rows with boundary values in the
        //:     test set.
        //:
        //: 3 For each row in the table, defined in P-2:
        //:
        //:   1 Create the object using the defined parameters.
        //:
        //:   2 Execute the inner loop, invoking 'submit' method the
        //:     specified number of times with the specified number of
        //:     units
        //:
        //:   3 Compare the value, returned by 'unitsInBucket' accessor
        //:     with the expected value from the table.  (C-1..4)
        //:
        //: 4 Repeat the steps defined in P-3 for 'reserve' and
        //:   'unitsReserved' functions.  (C-6..9)
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks
        //:   are triggered for invalid parameter values, but not
        //:   triggered for adjacent valid ones (using the
        //:   'BSLS_ASSERTTEST_*' macros).  (C-5, C-10)
        //
        // Testing:
        //   void submit(bsls::Types::Uint64 numOfUnits);
        //   void reserve(bsls::Types::Uint64 numOfUnits);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING: 'submit', 'reserve'" << endl
                 << "============================" << endl;

        const Ti     CREATION_TIME(0);
        const Uint64 CAPACITY(1);
        const Uint64 RATE(1000);

        // C-1, C-2
        struct {
            int      d_line;
            Uint64   d_units;
            unsigned d_numOfSubmits;
            Uint64   d_expectedUnits;
        } DATA[] = {
            // LINE     UNITS    SUBMITS    EXPECTED_UNITS
            // ----  ----------  -------  --------------------
              {L_,       1000,     1,                   1000},
              {L_,        250,     4,                   1000},
              {L_,          1,     4,                      4},
              {L_,          1,     1,                      1},
              {L_,        100,     2,                    200},
              {L_,       2000,     4,                   2000},
              {L_,   UINT_MAX,     4,     Uint64(UINT_MAX)*4}, // C-4
              {L_, ULLONG_MAX,     1,             ULLONG_MAX}  // C-3
        };
        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

        if (verbose) cout<<endl<<"Testing 'submit', 'unitsInBucket'"<<endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const Uint64 LINE           = DATA[ti].d_line;
                const int    NUM_OF_SUBMITS = DATA[ti].d_numOfSubmits;
                const Uint64 UNITS          = DATA[ti].d_units;

                Obj x(RATE, CAPACITY, CREATION_TIME);

                for(int i = 1; i <= NUM_OF_SUBMITS; ++i) {
                    x.submit(UNITS);
                    LOOP_ASSERT(LINE,x.unitsInBucket() == Uint64(i)*UNITS);
                }
            }

            // C-1, C-2
            Obj x(1000,1,Ti(0));

            ASSERT(0 == x.unitsInBucket());

            x.submit(42);
            ASSERT(42 == x.unitsInBucket());

            x.submit(0);
            ASSERT(42 == x.unitsInBucket());

            x.submit(999);
            ASSERT(1041 == x.unitsInBucket());

            x.submit(3001);
            ASSERT(4042 == x.unitsInBucket());

        }
        if (verbose) cout<<endl<<"Testing 'reserve','unitsReserved'"<<endl;
        {
            // C-6, C-7
            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const Uint64 LINE           = DATA[ti].d_line;
                const int    NUM_OF_SUBMITS = DATA[ti].d_numOfSubmits;
                const Uint64 UNITS          = DATA[ti].d_units;

                Obj x(RATE, CAPACITY, CREATION_TIME);

                for(int i = 1; i <= NUM_OF_SUBMITS; ++i) {
                    x.reserve(UNITS);
                    LOOP_ASSERT(LINE,x.unitsReserved() == Uint64(i)*UNITS);
                }
            }

            // C-6, C-7
            Obj x(1000,1,Ti(0));

            ASSERT(0 == x.unitsReserved());

            x.reserve(42);
            ASSERT(42 == x.unitsReserved());

            x.reserve(0);
            ASSERT(42 == x.unitsReserved());

            x.reserve(999);
            ASSERT(1041 == x.unitsReserved());

            x.reserve(3001);
            ASSERT(4042 == x.unitsReserved());
        }

        // C-5, C-10
        if (verbose) cout << endl << "Negative Testing" << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj x1(1000, 1, Ti(0));
            Obj x2(1000, 1, Ti(0));

            x1.submit(ULLONG_MAX-10);
            x2.submit(ULLONG_MAX-10);

            ASSERT_SAFE_FAIL(x1.submit( 11));
            ASSERT_SAFE_FAIL(x1.reserve(11));

            ASSERT_SAFE_PASS(x1.submit( 10));
            ASSERT_SAFE_PASS(x2.reserve(10));

            Obj y1(1000, 1, Ti(0));
            Obj y2(1000, 1, Ti(0));

            y1.reserve(ULLONG_MAX-10);
            y2.reserve(ULLONG_MAX-10);

            ASSERT_SAFE_FAIL(y1.submit( 11));
            ASSERT_SAFE_FAIL(y1.reserve(11));

            ASSERT_SAFE_PASS(y1.submit( 10));
            ASSERT_SAFE_PASS(y2.reserve(10));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure that each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding
        //:   attribute of the object.
        //:
        //: 2 Each accessor method is declared 'const'
        //
        // Plan:
        //: 1 Create a 'btls::LeakyBucket' object, using value constructor.
        //:
        //: 2 Call the accessors using 'const' reference.  Compare the
        //:   values of the object attributes, returned by accessors with
        //:   the values specified to the constructor.  (C-1..2)
        //:
        //: 3 Using the loop-based approach:
        //:
        //:   1 Create the set of object attribute values.
        //:
        //: 4 For each row in the table, defined in P-3
        //:
        //:   1 Construct the object using the value constructor.
        //:
        //:   2 Compare the values of 'rate' and 'capacity 'attributes,
        //:     returned by accessors with the values specified to the
        //:     constructor.  (C-1)
        //:
        //:   3 Call the accessors for the other attributes and compare
        //:     the returned values with the contractually specified
        //:     default values  (C-1)
        //
        // Testing:
        //   bsls::Types::Uint64 drainRate() const;
        //   bsls::Types::Uint64 capacity() const;
        //   bsls::Types::Uint64 unitsInBucket() const;
        //   bsls::Types::Uint64 unitsReserved() const;
        //   bsls::TimeInterval lastUpdateTime() const;
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "BASIC ACCESSORS"
                          << endl << "===============" << endl;

        Obj        x(1, 1, Ti(0));
        const Obj& X = x; // C-2
        Uint64     usedUnits;
        Uint64     unusedUnits;

        ASSERT(1     == X.drainRate());
        ASSERT(1     == X.capacity());
        ASSERT(0     == X.unitsInBucket());
        ASSERT(0     == X.unitsReserved());
        ASSERT(Ti(0) == X.lastUpdateTime());
        ASSERT(Ti(0) == X.statisticsCollectionStartTime());

        X.getStatistics(&usedUnits, &unusedUnits);
        ASSERT(0 == usedUnits);
        ASSERT(0 == unusedUnits);

        struct {
            int    d_line;
            Uint64 d_drainRate;
            Uint64 d_capacity;
            Ti     d_creationTime;
        } DATA[] = {
            //  LINE  RATE          CAPACITY  TCREATE
            //  ---- -----         ---------- --------
               {L_, 1000,                 1, Ti(10)},
               {L_, 1000,               100, Ti( 0)},
               {L_, ULLONG_MAX,  ULLONG_MAX, Ti( 0)},
               {L_, ULLONG_MAX,        1000, Ti( 0)},
               {L_, 1000,        ULLONG_MAX, Ti( 0)},
               {L_, ULLONG_MAX,           1, Ti( 0)},
               {L_, 1,           ULLONG_MAX, Ti( 0)}
        };
        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {

            // C-1
            const Uint64 LINE         = DATA[ti].d_line;
            const Uint64 RATE         = DATA[ti].d_drainRate;
            const Uint64 CAPACITY     = DATA[ti].d_capacity;
            const Ti CREATION_TIME    = DATA[ti].d_creationTime;

            Obj x(RATE, CAPACITY, CREATION_TIME);
            const Obj& X = x;

            // C-2
            LOOP_ASSERT(LINE, RATE          == X.drainRate());
            LOOP_ASSERT(LINE, CAPACITY      == X.capacity());
            LOOP_ASSERT(LINE, 0             == X.unitsInBucket());
            LOOP_ASSERT(LINE, 0             == X.unitsReserved());
            LOOP_ASSERT(LINE, CREATION_TIME == X.lastUpdateTime());
            LOOP_ASSERT(LINE, CREATION_TIME ==
                                            X.statisticsCollectionStartTime());

            X.getStatistics(&usedUnits, &unusedUnits);
            ASSERT(0 == usedUnits);
            ASSERT(0 == unusedUnits);
        }
      } break;
      case 3: {
        //---------------------------------------------------------------------
        // VALUE CTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The value constructor can create an object having any value that
        //:   does not violate the constructor's documented preconditions.
        //:
        //: 2 The newly created object is in appropriate state.
        //:
        //: 3 Any argument can be 'const'.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the loop-based approach:
        //:
        //:   1 Create the set of object attribute values.
        //:
        //: 2 For each row in the table, defined in P-1
        //:
        //:   1 Construct the object using the value constructor.
        //:
        //:   2 Compare the values of 'rate' and 'capacity' attributes,
        //:     returned by accessors with the values specified to the
        //:     constructor.  (C-1..2)
        //:
        //:   3 Compare the values of other attributes with the contractually
        //:     specified default values (C-3)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   btls::LeakyBucket(drainRate, window, currentTime);
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "VALUE CTOR"
                          << endl << "==========" << endl;

        const Uint64 BIG_VAL = 0xFFFFFFFFULL * 16;

        if (verbose) cout <<
                         "\nUsing a table of distinct object values." << endl;
        {
            // C-1
            struct {
                int    d_line;
                Uint64 d_drainRate;
                Uint64 d_capacity;
                Ti     d_creationTime;
            } DATA[] = {
                //  LINE       RATE   CAPACITY   TCREATE
                //  ----       -----  ---------- --------
                  {  L_,       1000,          1,   Ti(10) },
                  {  L_,       1000,        100,   Ti( 0) },
                  {  L_,       1000,    BIG_VAL,   Ti(50) },
                  {  L_, ULLONG_MAX, ULLONG_MAX,   Ti( 0) },
                  {  L_, ULLONG_MAX,       1000,   Ti( 0) },
                  {  L_,       1000, ULLONG_MAX,   Ti( 0) },
                  {  L_, ULLONG_MAX,          1,   Ti( 0) },
                  {  L_,          1, ULLONG_MAX,   Ti( 0) },
            };
            const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                // C-2
                const Uint64 LINE     = DATA[ti].d_line;
                const Uint64 RATE     = DATA[ti].d_drainRate;
                const Uint64 CAPACITY = DATA[ti].d_capacity;
                const Ti CR_TIME      = DATA[ti].d_creationTime;

                Obj x(RATE, CAPACITY, CR_TIME);

                LOOP_ASSERT(LINE, RATE     == x.drainRate());
                LOOP_ASSERT(LINE, CAPACITY == x.capacity());

                // C-3
                LOOP_ASSERT(LINE, CR_TIME  == x.lastUpdateTime());
                LOOP_ASSERT(LINE, 0        == x.unitsInBucket());
                LOOP_ASSERT(LINE, CR_TIME  ==
                                            x.statisticsCollectionStartTime());
            }
        }

        // C-4
        if (verbose) cout << endl << "Negative Testing" << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            // Drain Rate

            ASSERT_OPT_FAIL(Obj x(0, 1000, Ti(0)));
            ASSERT_OPT_PASS(Obj x(1, 1000, Ti(0)));

            // Capacity

            ASSERT_OPT_FAIL(Obj x(1,    0, Ti(0)));
            ASSERT_OPT_PASS(Obj x(1,    1, Ti(0)));

            // Current Time

            const Ti TA = Ti(LLONG_MIN,              0);
            const Ti TB = Ti(LLONG_MIN,     -999999999);

            const Ti TC = Ti(LLONG_MIN + 1, -999999999);
            const Ti TD = Ti(LLONG_MIN + 1, +999999999);

            ASSERT_FAIL(Obj x(1, 1, TA));
            ASSERT_FAIL(Obj x(1, 1, TB));

            ASSERT_PASS(Obj x(1, 1, TC));
            ASSERT_PASS(Obj x(1, 1, TD));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //   Ensure that the 'mock_LB' class and the 'testLB' template
        //   function operate correctly.
        //
        // Concerns:
        //: 1 Default ctor constructs a 'mock_LB' object having the
        //:   contractually specified attributes.
        //:
        //: 2 Value ctor constructs a 'mock_LB' object having the
        //:   contractually specified attributes.
        //:
        //: 3 The newly created object is in the correct state.
        //:
        //: 4 Accessors of 'mock_LB' class return values of corresponding
        //:   attributes.
        //:
        //: 5 'setRateAndCapacity' manipulator sets the 'rate' and
        //:   'capacity' attributes to the specified values.
        //:
        //: 6 'wouldOverflow' manipulator updates the 'lastUpdateTime'
        //:   attribute value, if needed.
        //:
        //: 7 'wouldOverflow' return 'true' if difference between
        //:   'lastUpdateTime' and 'currentTime' is less than
        //:   'submitInterval' and returns 'false' otherwise.
        //:
        //: 8 'calculateTimeToSubmit' calculates the time interval, that
        //:   should pass until submitting more units is allowed correctly.
        //:
        //: 9 'submit' manipulator updates the value of 'unitsInBucket'
        //:   attribute.
        //:
        //: 10 'testLB' function invokes 'setRateAndCapacity' on the
        //:   specified object with the specified parameters.
        //:
        //: 11 'testLB' function submits units by chunks and keeps
        //:    intervals between the 'submit' operations according
        //:    to the value returned by 'calculateTimeToSubmit'.
        //:
        //: 12 'testLB' calculates test duration correctly.
        //
        // Plan:
        //: 1 Create a 'mock_LB' object using the default ctor.
        //:   (C-1, C-3..4)
        //:
        //: 2 Verify the 'mock_LB' attributes.
        //:   (C-2, C-3..4)
        //:
        //: 3 Invoke the 'wouldOverflow' manipulator and verify the
        //:   returned value.
        //:
        //: 4 Invoke the 'calculateTimeToSubmit' manipulator and
        //:   verify the returned value.
        //:
        //: 5 Create a 'mock_LB' object using the value ctor and
        //:   perform steps, described in P-2..4.
        //:
        //: 6 Verify the returned value of 'wouldOverflow' manipulator invoked
        //:   with a time interval shorter than the 'submitInterval' specified
        //:   during construction.  (C-7)
        //:
        //: 7 Verify the returned value of 'calculateTimeToSubmit' manipulator,
        //:   invoked with a time interval, longer or equal to the
        //:   'submitInterval', specified during construction.  (C-8)
        //:
        //: 8 Invoke the 'submit' manipulator and verify the value of the
        //:   'unitsInBucket' attribute.  (C-9)
        //:
        //: 9 Invoke the 'setRateAndCapacity' manipulator and verify the values
        //:   of 'rate' and 'capacity' attributes after the invocation.  (C-5)
        //:
        //: 10 Using the table-driven technique:
        //:
        //:   1 Define the set of values containing the 'mock_LB' object
        //:     parameters, data chunk size and total data size used for
        //:     simulating load generation, and minimum time interval between
        //:     submissions.
        //:
        //: 11 For each row in the table, described in P-10:
        //:
        //:    1 Create a 'mock_LB' object, having the specified
        //:      parameters.
        //:
        //:    2 Invoke the 'testLB' function with the specified arguments.
        //:
        //:    3 Verify the test duration, returned by the 'testLB' function
        //:      and the 'mock_LB' object attributes after the invocation of
        //:      function.  (C-10..12)
        //
        // Testing:
        //   TEST APPARATUS
        //---------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST APPARATUS" << endl
                          << "==============" << endl;

        if (verbose) cout << endl << "testing mock_LB" << endl;
        {
            // C-1
            mock_LB x;

            // C-3
            ASSERT(Ti(0) == x.lastUpdateTime());
            ASSERT(0     == x.unitsInBucket());

            ASSERT(Ti(0) == x.submitInterval());

            ASSERT(1 == x.rate());
            ASSERT(1 == x.capacity());

            ASSERT(false == x.wouldOverflow(Ti(0)));
            ASSERT(Ti(0) == x.calculateTimeToSubmit(Ti(0)));

            // C-2
            mock_LB y(Ti(0, 10000), Ti(0));

            // C-3
            ASSERT(Ti(0) == y.lastUpdateTime());
            ASSERT(0     == y.unitsInBucket());

            ASSERT(Ti( 0, 10000) == y.submitInterval());

            ASSERT(1 == y.rate());
            ASSERT(1 == y.capacity());

            ASSERT(        true == y.wouldOverflow(Ti(0)));
            ASSERT(Ti(0, 10000) == y.calculateTimeToSubmit(Ti(0)));

            ASSERT(Ti(0, 3000)  == y.calculateTimeToSubmit(Ti(0,7000)));
            ASSERT(       false == y.wouldOverflow(Ti(0, 10000)));

            x.submit(1500);
            ASSERT(1500 == x.unitsInBucket());

            x.setRateAndCapacity(1000, 100);

            ASSERT(1000 == x.rate());
            ASSERT(100  == x.capacity());
        }

        if (verbose) cout << endl << "testing TestLB" << endl;
        {
            const Uint64 M     = 1000000;
            const Uint64 G     = 1000000000;

            struct {
                int    d_line;
                Uint64 d_drainRate;
                Uint64 d_capacity;
                Uint64 d_chunkSize;
                Uint64 d_dataSize;
                Ti     d_submitInterval;
                Ti     d_expDur;
            } DATA[] = {
        // LINE RATE   CAP    CHUNK  DATA_SIZE INTERVAL     EXP_DUR
        // ---- ------ ------ ------ --------- ------------ ----------------
        {  L_,   1000,   100,  1000,    50000, Ti(0, 5000), Ti(0,    250000) },
        {  L_,   1000,   100,  1000,    50000,       Ti(0), Ti(           0) },
        {  L_,   10*M, 10000,  1000,  10*M*10, Ti(0, 5000), Ti(0, 500000000) },
        {  L_,     50,     5,    10,    50*15,       Ti(1), Ti(          75) },
        {  L_,     10,     1,     2,    10*10,       Ti(5), Ti(         250) },
        {  L_, 35LL*G,   4*M,  1400,    35000, Ti(0, 5000), Ti(0,    125000) }
            };
            const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const int    LINE            = DATA[ti].d_line;
                const Uint64 RATE            = DATA[ti].d_drainRate;
                const Uint64 CAPACITY        = DATA[ti].d_capacity;
                const Uint64 CHUNK_SIZE      = DATA[ti].d_chunkSize;
                const Uint64 DATA_SIZE       = DATA[ti].d_dataSize;
                const Ti     SUBMIT_INTERVAL = DATA[ti].d_submitInterval;
                const Ti     EXP_DUR         = DATA[ti].d_expDur;

                mock_LB x(SUBMIT_INTERVAL, Ti(0));

                Ti dur = testLB<mock_LB>(x,
                                         RATE,
                                         CAPACITY,
                                         DATA_SIZE,
                                         CHUNK_SIZE,
                                         Ti(0));

                LOOP_ASSERT(LINE, DATA_SIZE == x.unitsInBucket());
                LOOP_ASSERT(LINE, RATE      == x.rate());
                LOOP_ASSERT(LINE, CAPACITY  == x.capacity());
                LOOP_ASSERT(LINE, EXP_DUR   == x.lastUpdateTime());

                ASSERT(EXP_DUR   == dur);
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent cases
        //
        // Plan:
        //: 1 Create an object, using the value ctor.
        //:
        //: 2 Invoke the 'setRateAndCapacity' manipulator.
        //:
        //: 3 Invoke the 'rate' and 'capacity' accessors and check the
        //:   returned values.
        //:
        //: 4 Invoke the 'submit' and 'reserve' manipulators.
        //:
        //: 5 Invoke the 'unitsInBucket' and 'unitsReserved' accessors
        //:   and check the returned value.
        //:
        //: 6 Invoke the 'wouldOverflow' and 'calculateTimeToSubmit'
        //:   manipulators.
        //:
        //: 7 Invoke the 'updateState' manipulator.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        Obj x(1, 1, Ti(0));
        Ti  currentTime(1);
        ASSERT(1 == x.drainRate());
        ASSERT(1 == x.capacity());
        ASSERT(0 == x.lastUpdateTime());

        x.setRateAndCapacity(1000, 1000);
        x.reset(currentTime);

        ASSERT(1000 == x.drainRate());
        ASSERT(1000 == x.capacity());

        x.submit(500);
        x.reserve(250);
        ASSERT(500 == x.unitsInBucket());
        ASSERT(250 == x.unitsReserved());

        ASSERT(false == x.wouldOverflow(currentTime));
        ASSERT(Ti(0) == x.calculateTimeToSubmit(currentTime));
        x.submitReserved(250);
        x.submit(750);

        ASSERT(0         == x.unitsReserved());
        ASSERT(1500      == x.unitsInBucket());

        ASSERT(Ti(0, 501000000) == x.calculateTimeToSubmit(currentTime));

        currentTime.addMilliseconds(500);
        ASSERT(true == x.wouldOverflow(currentTime));

        currentTime.addMilliseconds(100);
        x.updateState(currentTime);
        ASSERT(900 == x.unitsInBucket());

        x.submit(100);
        ASSERT(1000 == x.unitsInBucket());
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
