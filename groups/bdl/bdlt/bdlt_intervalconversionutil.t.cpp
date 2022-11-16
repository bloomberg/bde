// bdlt_intervalconversionutil.t.cpp                                  -*-C++-*-

#include <bdlt_intervalconversionutil.h>

#include <bslim_testutil.h>

#include <bsls_asserttest.h>
#include <bsls_review.h>
#include <bsls_systemtime.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test is a utility, containing independent, pure
// functions having no (physical) interdependencies.  Therefore, each function
// can be tested independently, in arbitrary order.
//
// Since the component under test is a conversion utility, the test data should
// be selected so that it (1) tests the limits of both the input type and the
// output type, and (2) crosses the boundaries of *representation* of the
// output type.
//
// Test case 2 performs a configurable pseudo-random test.  For details on how
// to provide configuration parameters to that test case, see the test case
// overview.
// ----------------------------------------------------------------------------
// [ 2] void convertToDatetimeInterval(DatetimeInterval *, TimeInterval&);
// [ 1] void convertToTimeInterval(TimeInterval *, DatetimeInterval& );
// ----------------------------------------------------------------------------
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

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlt::IntervalConversionUtil Util;
typedef bdlt::TimeUnitRatio Ratio;

// ============================================================================
//                 USAGE EXAMPLE SUPPORT TYPES AND FUNCTIONS
// ----------------------------------------------------------------------------

void displayTime(const bdlt::DatetimeInterval& timeSinceEpoch)
    // Print the specified 'timeSinceEpoch' in a human-readable format.
{
    (void) timeSinceEpoch;  // Suppress compiler warning.
}

// ============================================================================
//                      HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

// The maximum number of random round-trip tests that will be done case 2 is
// the number of possible 'bdlt::Datetime' values.  Note that 'INT_MAX' times
// 'microseconds per day' is too big to fit in a 64-bit value.

const bsls::Types::Uint64 maxTestCycles = (1ULL << 32) * 24 * 60 * 60 * 999;

bsls::Types::Uint64 random(bsls::Types::Uint64 seed)
    // Return the pseudo-random unsigned number following the specified 'seed'
    // in a sequence of pseudo-random numbers that cycles through all values in
    // the closed range '[ MIN_MS .. MAX_MS ]' where 'MIN_MS' and 'MAX_MS' are
    // the minimum (negative) and maximum numbers of milliseconds that can be
    // represented by a 'DatetimeInterval'.
{
    static const bsls::Types::Uint64 m = maxTestCycles;
        // factors of m: 2, 3, 5, 37
    static const bsls::Types::Uint64 c = 608862947LL;
        // 'c' is floor(sqrt(m))
        // factors of c: 7, 11, 173, 45707
    static const bsls::Types::Uint64 a = 675837871171LL;
        // 'a - 1' is chosen simply as the product of all factors of m and c.
        // factors of a - 1: 2, 3, 5, 37, 7, 11, 173, 45707

    // Since c and m are relatively prime, and a - 1 is divisible by all prime
    // factors of m, this function will cycle only once every 'm' iterations.

    return (a * seed + c) % m;
}

void loopMeter(bsls::Types::Uint64 index,
               bsls::Types::Uint64 length,
               int size = 50)
    // Create a visual display for a computation of the specified 'length' and
    // emit updates to 'cerr' as appropriate for the specified 'index'.
    // Optionally specify the 'size' of the display.  This function should be
    // called immediately after the 'for'-loop with 'index' and 'length', and
    // again at the end with 'length' and 'length':
    //..
    //  for (int i = 0; i < n, ++i) {
    //
    //      loopMeter(i, n);  // <== HERE
    //
    //      // ... (body of loop)
    //
    //  }
    //
    //  loopMeter(n, n);      // <== HERE
    //..
    // The behavior is undefined unless '0 < size', and 'index <= length'.
    // Note that it is expected that indices will be presented in order from 0
    // to 'length', inclusive, without intervening output to 'stderr'; however,
    // intervening output to 'stdout' may be redirected productively.
{
    ASSERT(0 < size);
    ASSERT(index <= length);

    if (0 == index) {           // beginning of the loop
        bsl::cerr << "     |";
        for (int i = 1; i < size; ++i) {
            bsl::cerr << (i % 5 ? '-' : '+');
        }
        bsl::cerr << "|\nBEGIN." << bsl::flush;
    }
    else {                      // middle of the loop
        int t1 = int((static_cast<double>(index - 1) * size)
                                          / static_cast<double>(length) + 0.5);
        int t2 = int((static_cast<double>(index)     * size)
                                          / static_cast<double>(length) + 0.5);
        int dt = t2 - t1;       // accumulated ticks (but no accumulated error)

        for (int i = 0; i < dt; ++i) {
           bsl::cerr << '.';
        }
        bsl::cerr << bsl::flush;
    }

    if (index == length) {      // end of the loop
        bsl::cerr << "END" << bsl::endl;
    }
}

template <class INTEGER_TYPE = int>
struct ConfigParser {
    // Provide a namespace for functions reading configuration values from a
    // string.

    static void parse(vector<INTEGER_TYPE> *result, const string& config);
        // Load into the specified '*result' the comma- and/or
        // whitespace-separated integer values, if any, of (template parameter)
        // type 'INTEGER_TYPE' found at the beginning of the specified 'config'
        // string.  Any part of 'config' that cannot be parsed as such a list
        // of integer values, and any following contents are ignored.
};

template <class INTEGER_TYPE>
void ConfigParser<INTEGER_TYPE>::parse(vector<INTEGER_TYPE> *result,
                                       const string&         config)
{
    // Turn the comma-separated values into a string with one value per line.

    string input(config, result->get_allocator());
    replace(input.begin(), input.end(), ',', ' ');

    // Put the values into a stream.

    stringstream stream(result->get_allocator());
    stream << input;

    INTEGER_TYPE value;
    while (stream >> value) {
        result->push_back(value);
    }
}

int expNanoseconds(int nanoseconds)
{
    bool sign = false;

    if ((sign = nanoseconds < 0)) {
        nanoseconds = -nanoseconds;
    }

    nanoseconds -= nanoseconds % 1000;

    return (sign ? -1 : +1) * nanoseconds;
}

}  // close unnamed namespace

// ============================================================================
//                          MAIN PROGRAM
// ----------------------------------------------------------------------------


int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;  (void)             verbose;
    const bool         veryVerbose = argc > 3;  (void)         veryVerbose;
    const bool     veryVeryVerbose = argc > 4;  (void)     veryVeryVerbose;
    const bool veryVeryVeryVerbose = argc > 5;  (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Interfacing With an API That Uses 'bsls::TimeInterval'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Some APIs, such as 'bsls::SystemTime', use 'bsls::TimeInterval' in their
// interface.  In order to use those APIs in components implemented in terms of
// 'bdlt::DatetimeInterval', it is necessary to convert between the
// 'bsls::TimeInterval' and 'bdlt::DatetimeInterval' representations for a time
// interval.  This conversion can be accomplished conveniently using
// 'bdlt::IntervalConversionUtil'.
//
// Suppose we wish to pass the system time -- as returned by
// 'bsls::SystemTime::nowRealtimeClock' -- to a function that displays a time
// that is represented as a 'bdlt::DatetimeInterval' since the UNIX epoch.
//
// First, we include the declaration of the function that displays a
// 'bdlt::DatetimeInterval':
//..
    void displayTime(const bdlt::DatetimeInterval& timeSinceEpoch);
//..
// Then, we obtain the current system time from 'bsls::SystemTime', and store
// it in a 'bsls::TimeInterval':
//..
    bsls::TimeInterval systemTime = bsls::SystemTime::nowRealtimeClock();
//..
// Now, we convert the 'bsls::TimeInterval' into a 'bdlt::DatetimeInterval'
// using 'convertToDatetimeInterval':
//..
    bdlt::DatetimeInterval timeSinceEpoch =
           bdlt::IntervalConversionUtil::convertToDatetimeInterval(systemTime);

    ASSERT(timeSinceEpoch.totalMilliseconds() ==
                                               systemTime.totalMilliseconds());
//..
// Finally, we display the time by passing the converted value to
// 'displayTime':
//..
    displayTime(timeSinceEpoch);
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'convertToDatetimeInterval' METHOD
        //   The 'convertToDatetimeInterval' function converts an input
        //   'bsls::TimeInterval' into a 'bdlt::DatetimeInterval' by directly
        //   setting the milliseconds field of the 'bdlt::DatetimeInterval' to
        //   the total milliseconds of the source 'bsls::TimeInterval' value.
        //   Since the underlying methods of both types have already been
        //   tested, we need only confirm that the methods are being correctly
        //   called, and verify that the correct checks for undefined behavior
        //   are being made.
        //
        //   This test case includes a pseudo-randomly generated test of
        //   round-trip conversions from 'bdlt::DatetimeInterval' to
        //   'bsls::TimeInterval' and back.  By default, the test only performs
        //   one trillionth of the 2^59 possible combinations, but the size of
        //   the test and the random seed can be configured by supplying a
        //   custom seed and an optional sample divisor as a comma-separated
        //   list in the second argument to the test driver.  Therefore, to run
        //   the usual number of iterations with a custom random seed of '25',
        //   the test would be invoked as:
        //..
        //     $ test.t 2 25
        //..
        //   To run the test with a custom seed of '25' and do one billion
        //   combinations, the test would be invoked as:
        //..
        //     $ test.t 2 25,1000000000
        //..
        //   In no case will the test perform more than 2^59 combinations, so a
        //   full test (*NOT* recommended) can be conveniently performed by
        //   requesting 10^18 combinations:
        //..
        //     $ test.t 2 25,1000000000000000000
        //..
        //
        // Concerns:
        //: 1 That the conversion is done correctly for all valid values of
        //:   'bsls::TimeInterval'.
        //:
        //: 2 That the loss of precision is limited to losing the
        //:   sub-millisecond portion of the original value.
        //:
        //: 3 That loss of precision (i.e., rounding of microseconds and
        //:   nanoseconds toward 0) is consistent across millisecond +/- 1
        //:   nanosecond boundaries.
        //:
        //: 4 That round-trip conversion to 'bsls::TimeInterval' and back is
        //:   not lossy over the valid range common to both data types.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create several 'bsls::TimeInterval' objects with and without
        //:   nanosecond remainders, and convert them into
        //:   'bdlt::DatetimeInterval' objects using the
        //:   'convertToDatetimeInterval' method.  Compare the output to
        //:   expected values.  (C-1..3)
        //:
        //: 2 Create 'bsls::TimeInterval' objects having the maximum and
        //:   minimum values supported by 'bdlt::DatetimeInterval', and check
        //:   that they match the values of 'bdlt::DatetimeInterval' objects
        //:   constructed with the maximum and minimum allowable values.
        //:   (C-1-3)
        //:
        //: 3 Create several 'bdlt::DatetimeInterval' objects having values
        //:   across the valid range for the type, and convert them in a round
        //:   trip to 'bsls::TimeInterval' and back.  Confirm that the value at
        //:   the end of the round trip is equal to the original value.  (C-4)
        //:
        //: 4 Repeat the test from step 3 with a user-controlled number of
        //:   pseudo-random values.  (C-4)
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to pass a null pointer as
        //:   'result'.  (C-5)
        //
        // Testing:
        //   void convertToDatetimeInterval(DatetimeInterval *, TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'convertToDatetimeInterval' METHOD" << endl
                 << "==========================================" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct inputs and expected outputs." << endl;
        {
            static const struct {
                int                d_lineNum;   // source line number
                bsls::Types::Int64 d_seconds;   // second field value
                int                d_nsecs;     // nanosecond field value
                int                d_expDays;   // expected days value
                bsls::Types::Int64 d_expHours;  // expected hours value
                bsls::Types::Int64 d_expMins;   // expected minutes value
                bsls::Types::Int64 d_expSecs;   // expected seconds value
                bsls::Types::Int64 d_expMlSecs; // expected milliseconds value
                bsls::Types::Int64 d_expMcSecs; // expected microseconds value
            } DATA[] = {
    //|<--------^
      //   INPUT               INPUT     EXP  EXP  EXP  EXP EXC    EXP
      //LN SECONDS             NSECS    DAYS  HRS  MIN  SEC MLSEC  MCSEC
      //-- -------------  ----------  ------  ---  ---  --- -----  -----
      {L_,           0LL,          0,      0,   0,   0,   0,    0,     0},

                 // Millisecond/microsecond boundary
      {L_,           0LL,     999999,      0,   0,   0,   0,    0,   999},
      {L_,           0LL,    1000999,      0,   0,   0,   0,    0,  1000},
      {L_,           0LL,    1001999,      0,   0,   0,   0,    0,  1001},
      {L_,           0LL,    1000000,      0,   0,   0,   0,    1,     0},
      {L_,           0LL,    1000001,      0,   0,   0,   0,    1,     0},
      {L_,           0LL,    -999999,      0,   0,   0,   0,    0,  -999},
      {L_,           0LL,   -1000999,      0,   0,   0,   0,    0, -1000},
      {L_,           0LL,   -1001999,      0,   0,   0,   0,    0, -1001},
      {L_,           0LL,   -1000000,      0,   0,   0,   0,   -1,     0},
      {L_,           0LL,   -1000001,      0,   0,   0,   0,   -1,     0},

                     // Second/fraction boundary
      {L_,           0LL,  999999999,      0,   0,   0,   0,  999,   999},
      {L_,           1LL,          0,      0,   0,   0,   0,  999,  1000},
      {L_,           1LL,       1999,      0,   0,   0,   0,  999,  1001},
      {L_,           1LL,          0,      0,   0,   0,   1,    0,     0},
      {L_,           1LL,          1,      0,   0,   0,   1,    0,     0},
      {L_,           0LL, -999999999,      0,   0,   0,   0, -999,  -999},
      {L_,          -1LL,       -999,      0,   0,   0,   0, -999, -1000},
      {L_,          -1LL,      -1999,      0,   0,   0,   0, -999, -1001},
      {L_,          -1LL,          0,      0,   0,   0,  -1,    0,     0},
      {L_,          -1LL,         -1,      0,   0,   0,  -1,    0,     0},

           // Arbitrary values that express all DatetimeInterval fields
      {L_,  3000000011LL,    9999998,  34722,   5,  20,  11,    9,   999},
      {L_,  3000000011LL,    9999999,  34722,   5,  20,  11,    9,   999},
      {L_,  3000000011LL,   10000999,  34722,   5,  20,  11,    9,  1000},
      {L_,  3000000011LL,   10001500,  34722,   5,  20,  11,    9,  1001},
      {L_,  3000000011LL,   10000000,  34722,   5,  20,  11,   10,     0},
      {L_,  3000000011LL,   10000001,  34722,   5,  20,  11,   10,     0},
      {L_, -3000000011LL,   -9999999, -34722,  -5, -20, -11,   -9,  -999},
      {L_, -3000000011LL,  -10000999, -34722,  -5, -20, -11,   -9, -1000},
      {L_, -3000000011LL,  -10001999, -34722,  -5, -20, -11,   -9, -1001},
      {L_, -3000000011LL,   -9999998, -34722,  -5, -20, -11,   -9,  -999},
      {L_, -3000000011LL,  -10000000, -34722,  -5, -20, -11,  -10,     0},
      {L_, -3000000011LL,  -10000001, -34722,  -5, -20, -11,  -10,     0},

                                   // Limits

      {L_, INT_MAX * Ratio::k_SECONDS_PER_DAY + Ratio::k_SECONDS_PER_DAY,
                           -1 * Ratio::k_NANOSECONDS_PER_MILLISECOND,
                                      INT_MAX, 23,  59,  59,  999,    0},

      {L_, INT_MIN * Ratio::k_SECONDS_PER_DAY - Ratio::k_SECONDS_PER_DAY,
                           Ratio::k_NANOSECONDS_PER_MILLISECOND,
                                      INT_MIN,-23, -59, -59,-999,     0},

      {L_, INT_MAX * Ratio::k_SECONDS_PER_DAY + Ratio::k_SECONDS_PER_DAY,
                           -1 * Ratio::k_NANOSECONDS_PER_MICROSECOND,
                                      INT_MAX, 23,  59,  59,  999,  999},

      {L_, INT_MIN * Ratio::k_SECONDS_PER_DAY - Ratio::k_SECONDS_PER_DAY,
                           Ratio::k_NANOSECONDS_PER_MICROSECOND,
                                      INT_MIN,-23, -59, -59,-999,  -999},
    //^-------->|
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE       = DATA[i].d_lineNum;
                const bsls::Types::Int64 SECS       = DATA[i].d_seconds;
                const int                NSECS      = DATA[i].d_nsecs;
                const int                EXP_DAYS   = DATA[i].d_expDays;
                const bsls::Types::Int64 EXP_HOURS  = DATA[i].d_expHours;
                const bsls::Types::Int64 EXP_MINS   = DATA[i].d_expMins;
                const bsls::Types::Int64 EXP_SECS   = DATA[i].d_expSecs;
                const bsls::Types::Int64 EXP_MLSECS = DATA[i].d_expMlSecs;
                const bsls::Types::Int64 EXP_MCSECS = DATA[i].d_expMcSecs;

                bsls::TimeInterval ti(SECS, NSECS);

                bdlt::DatetimeInterval expected(EXP_DAYS,
                                                EXP_HOURS,
                                                EXP_MINS,
                                                EXP_SECS,
                                                EXP_MLSECS,
                                                EXP_MCSECS);

                if (veryVerbose) {
                    T_ P_(LINE) P_(ti) P(expected)
                }

                bdlt::DatetimeInterval dti =
                                           Util::convertToDatetimeInterval(ti);

                ASSERTV(LINE, expected, dti, expected == dti);

                int expNs = expNanoseconds(NSECS);

                bsls::TimeInterval expTi(SECS, expNs);

                bsls::TimeInterval convertedTi =
                                         Util::convertToTimeInterval(expected);

                ASSERTV(LINE, expTi, convertedTi, expTi == convertedTi);
            }
        }

        if (verbose) cout << "\nRound-Trip Conversions." << endl;
        {
            static const struct {
                int                d_line;
                int                d_days;   // days value
                bsls::Types::Int64 d_hours;  // hours value
                bsls::Types::Int64 d_mins;   // minutes value
                bsls::Types::Int64 d_secs;   // seconds value
                bsls::Types::Int64 d_mlSecs; // milliseconds value
                bsls::Types::Int64 d_mcSecs; // microseconds value
            } DATA[] = {
                //LN DAYS         HRS  MIN  SEC  MLSEC MCSEC
                //-- -----------  ---  ---  ---  ----- -----
                {L_,           0,   0,   0,   0,    0,     0},

                // Millisecond/microsecond boundary
                {L_,           0,   0,   0,   0,    0,     0},
                {L_,           0,   0,   0,   0,    1,     0},
                {L_,           0,   0,   0,   0,    0,     0},
                {L_,           0,   0,   0,   0,   -1,     0},

                {L_,           0,   0,   0,   0,    0,     1},
                {L_,           0,   0,   0,   0,    1,     1},
                {L_,           0,   0,   0,   0,    0,     1},
                {L_,           0,   0,   0,   0,   -1,    -1},

                {L_,           0,   0,   0,   0,    0,     0},
                {L_,           0,   0,   0,   0,    1,   999},
                {L_,           0,   0,   0,   0,    0,     0},
                {L_,           0,   0,   0,   0,   -1,  -999},

                // Second/fraction boundary
                {L_,           0,   0,   0,   0,  999,     0},
                {L_,           0,   0,   0,   1,    0,     0},
                {L_,           0,   0,   0,   0, -999,     0},
                {L_,           0,   0,   0,  -1,    0,     0},

                {L_,           0,   0,   0,   0,  999,     1},
                {L_,           0,   0,   0,   1,    0,     1},
                {L_,           0,   0,   0,   0, -999,    -1},
                {L_,           0,   0,   0,  -1,    0,    -1},

                {L_,           0,   0,   0,   0,  999,   999},
                {L_,           0,   0,   0,   1,    0,   999},
                {L_,           0,   0,   0,   0, -999,  -999},
                {L_,           0,   0,   0,  -1,    0,  -999},

                // Arbitrary values that express all DatetimeInterval fields
                {L_,       34722,   5,  20,  11,    9,     0},
                {L_,       34722,   5,  20,  11,    9,     0},
                {L_,       34722,   5,  20,  11,   10,     0},
                {L_,       34722,   5,  20,  11,   10,     0},
                {L_,      -34722,  -5, -20, -11,   -9,     0},
                {L_,      -34722,  -5, -20, -11,   -9,     0},
                {L_,      -34722,  -5, -20, -11,   -9,     0},
                {L_,      -34722,  -5, -20, -11,  -10,     0},
                {L_,      -34722,  -5, -20, -11,  -10,     0},

                {L_,       34722,   5,  20,  11,    9,     1},
                {L_,       34722,   5,  20,  11,    9,     1},
                {L_,       34722,   5,  20,  11,   10,     1},
                {L_,       34722,   5,  20,  11,   10,     1},
                {L_,      -34722,  -5, -20, -11,   -9,    -1},
                {L_,      -34722,  -5, -20, -11,   -9,    -1},
                {L_,      -34722,  -5, -20, -11,   -9,    -1},
                {L_,      -34722,  -5, -20, -11,  -10,    -1},
                {L_,      -34722,  -5, -20, -11,  -10,    -1},

                {L_,       34722,   5,  20,  11,    9,   999},
                {L_,       34722,   5,  20,  11,    9,   999},
                {L_,       34722,   5,  20,  11,   10,   999},
                {L_,       34722,   5,  20,  11,   10,   999},
                {L_,      -34722,  -5, -20, -11,   -9,  -999},
                {L_,      -34722,  -5, -20, -11,   -9,  -999},
                {L_,      -34722,  -5, -20, -11,   -9,  -999},
                {L_,      -34722,  -5, -20, -11,  -10,  -999},
                {L_,      -34722,  -5, -20, -11,  -10,  -999},

                // Limits
                {L_,     INT_MAX,  23,  59,  59,  999,   999},
                {L_,     INT_MIN, -23, -59, -59, -999,  -999},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE   = DATA[i].d_line;
                const int                DAYS   = DATA[i].d_days;
                const bsls::Types::Int64 HOURS  = DATA[i].d_hours;
                const bsls::Types::Int64 MINS   = DATA[i].d_mins;
                const bsls::Types::Int64 SECS   = DATA[i].d_secs;
                const bsls::Types::Int64 MLSECS = DATA[i].d_mlSecs;
                const bsls::Types::Int64 MCSECS = DATA[i].d_mcSecs;

                bdlt::DatetimeInterval originalValue(DAYS,
                                                     HOURS,
                                                     MINS,
                                                     SECS,
                                                     MLSECS,
                                                     MCSECS);

                if (veryVerbose) {
                    T_ P_(LINE) P(originalValue)
                }

                bsls::TimeInterval timeInterval =
                                    Util::convertToTimeInterval(originalValue);

                bdlt::DatetimeInterval finalValue =
                                 Util::convertToDatetimeInterval(timeInterval);

                ASSERTV(LINE, originalValue, timeInterval, finalValue,
                                                  finalValue == originalValue);
            }
        }

        if (verbose) cout << "\nPseudo-Random Round-Trip Stress Test." << endl;
        {
            // Set up the number of tests to perform, and the random seed.

            // By default, run only one trillionth of the test.

            bsls::Types::Uint64 period = maxTestCycles / 1000000000000ULL;
            bsls::Types::Uint64 seed   = 17;

            // Configure user-supplied values, if supplied.

            if (argc > 2) {
                vector<bsls::Types::Uint64> config;
                ConfigParser<bsls::Types::Uint64>::parse(&config,
                                                          string(argv[2]));

                // First configuration argument is the custom seed.

                if (0 < config.size() && config[0] != 0) {
                    seed = config[0];

                    if (verbose) {
                        T_ P(seed)
                    }
                }

                // Second configuration argument is the custom period.

                if (1 < config.size() && config[1] != 0) {
                    period = min(config[1], maxTestCycles);

                    if (verbose) {
                        T_ P(period)
                    }
                }
            }

            // Run the test 'period' times.

            for (bsls::Types::Uint64 i = 0; i < period; ++i) {
                if (verbose && !veryVerbose) { loopMeter(i, period); }

                seed = random(seed);

                // Extract milliseconds.

                bsls::Types::Int64 MCSECS = seed % 1000;

                seed = random(seed);

                bsls::Types::Int64 source =
                     static_cast<bsls::Types::Int64>(seed) - maxTestCycles / 2;

                // Extract milliseconds.

                bsls::Types::Int64 MLSECS = source % 1000;
                source /= 1000;

                // Extract seconds.

                bsls::Types::Int64 SECS = source % 60;
                source /= 60;

                // Extract minutes.

                bsls::Types::Int64 MINS = source % 60;
                source /= 60;

                // Extract hours.

                bsls::Types::Int64 HOURS = source % 24;
                source /= 24;

                // Extract days.

                int DAYS = static_cast<int>(source);

                // Now continue with a normal test.

                if (veryVerbose) {
                    T_ P_(i) P_(DAYS) P_(HOURS) P_(MINS) P_(SECS) P_(MLSECS)
                    P(MCSECS);
                }

                if (veryVeryVerbose) {
                    T_ T_ P_(period) P(seed)
                }

                bdlt::DatetimeInterval originalValue(DAYS,
                                                     HOURS,
                                                     MINS,
                                                     SECS,
                                                     MLSECS,
                                                     MCSECS);

                bsls::TimeInterval timeInterval =
                                    Util::convertToTimeInterval(originalValue);

                bdlt::DatetimeInterval finalValue =
                                 Util::convertToDatetimeInterval(timeInterval);

                ASSERTV(i,
                        period,
                        seed,
                        originalValue,
                        timeInterval,
                        finalValue,
                        finalValue == originalValue);
            }

            if (verbose && !veryVerbose) { loopMeter(period, period); }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            bdlt::DatetimeInterval maximalDtI(INT_MAX,  23,  59,  59,  999);
            bdlt::DatetimeInterval minimalDtI(INT_MIN, -23, -59, -59, -999);
            maximalDtI.addMicroseconds( 999);
            minimalDtI.addMicroseconds(-999);

            bsls::TimeInterval maximalTI =
                                       Util::convertToTimeInterval(maximalDtI);
            bsls::TimeInterval superMaximalTI = maximalTI;
            superMaximalTI.addNanoseconds(1);

            ASSERT_SAFE_PASS(Util::convertToDatetimeInterval(maximalTI));
            ASSERT_SAFE_FAIL(Util::convertToDatetimeInterval(superMaximalTI));

            bsls::TimeInterval minimalTI =
                                       Util::convertToTimeInterval(minimalDtI);
            bsls::TimeInterval subMinimalTI = minimalTI;
            subMinimalTI.addNanoseconds(-1);

            ASSERT_SAFE_PASS(Util::convertToDatetimeInterval(minimalTI));
            ASSERT_SAFE_FAIL(Util::convertToDatetimeInterval(subMinimalTI));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'convertToTimeInterval' METHOD
        //   The 'convertToTimeInterval' function converts a
        //   'bdlt::DatetimeInterval' into a 'bsls::TimeInterval' representing
        //   the same number of total milliseconds.
        //
        // Concerns:
        //: 1 That the conversion is done correctly.
        //
        // Plan:
        //: 1  Create several distinct 'bdlt::DatetimeInterval' objects, then
        //:   convert them to 'bsls::TimeInterval' objects.  Use the
        //:   'totalMilliseconds' accessors of 'bdlt::DatetimeInterval' and
        //:   'bsls::TimeInterval' as oracles to determine whether or not the
        //:   conversion was done correctly.  (C-1)
        //
        // Testing:
        //   void convertToTimeInterval(TimeInterval *, DatetimeInterval& );
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'convertToTimeInterval' METHOD" << endl
                          << "======================================" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct inputs, and check 'totalMilliseconds'."
                                                                       << endl;
        {
            static const struct {
                int                d_line;
                int                d_days;
                bsls::Types::Int64 d_hours;
                bsls::Types::Int64 d_minutes;
                bsls::Types::Int64 d_seconds;
                bsls::Types::Int64 d_mlsecs;
                bsls::Types::Int64 d_mcsecs;
            } DATA[] = {
                //LINE  DAYS    HOURS  MINUTES  SECONDS  MLSEC MCSEC
                //----  ------  -----  -------  -------  ---------------
                { L_,        0,     0,       0,       0,     0,    0},
                { L_,       -1,    -1,      -1,      -1,    -1,   -1},
                { L_,        0,     0,       0,       0,   999,    0},
                { L_,        0,     0,       0,       0,   999,  999},
                { L_,        0,     0,       0,       0,  -999,    0},
                { L_,        0,     0,       0,       0,  -999, -999},
                { L_,        0,     0,       0,      59,     0,    0},
                { L_,        0,     0,       0,     -59,     0,    0},
                { L_,        0,     0,      59,       0,     0,    0},
                { L_,        0,     0,     -59,       0,     0,    0},
                { L_,        0,    23,       0,       0,     0,    0},
                { L_,        0,   -23,       0,       0,     0,    0},
                { L_,       45,     0,       0,       0,     0,    0},
                { L_,      -45,     0,       0,       0,     0,    0},
                { L_,       45,    23,      59,      59,   999,    0},
                { L_,       45,    23,      59,      59,   999,  999},
                { L_,      -45,   -23,     -59,     -59,  -999,    0},
                { L_,      -45,   -23,     -59,     -59,  -999, -999},
                { L_,       45,    23,      22,      21,   206,    0},
                { L_,       45,    23,      22,      21,   206,    1},
                { L_,       45,    23,      22,      21,   207,    0},
                { L_,       45,    23,      22,      21,   207,  999},
                { L_,       45,    23,      22,      21,   208,    0},
                { L_,       45,    23,      22,      21,   208,  999},
                { L_,    10000,    23,      22,      21,   206,    0},
                { L_,    10000,    23,      22,      21,   206,  999},
                { L_,   -10000,   -23,     -22,     -21,  -206,    0},
                { L_,   -10000,   -23,     -22,     -21,  -206, -999},

                // Maximum and minimum possible values
                { L_,  INT_MAX,    23,      59,      59,   999,  999},
                { L_,  INT_MIN,   -23,     -59,     -59,  -999,  999},
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE                  = DATA[i].d_line;
                const int DAYS                  = DATA[i].d_days;
                const bsls::Types::Int64 HOURS  = DATA[i].d_hours;
                const bsls::Types::Int64 MINS   = DATA[i].d_minutes;
                const bsls::Types::Int64 SECS   = DATA[i].d_seconds;
                const bsls::Types::Int64 MLSECS = DATA[i].d_mlsecs;
                const bsls::Types::Int64 MCSECS = DATA[i].d_mcsecs;

                bdlt::DatetimeInterval dti(DAYS,
                                           HOURS,
                                           MINS,
                                           SECS,
                                           MLSECS,
                                           MCSECS);

                if (veryVerbose) {
                    T_ P_(LINE) P_(i) T_ P(dti)
                    T_ P_(DAYS) P_(HOURS) P_(MINS) P_(SECS) P_(MLSECS)
                    P(MCSECS);
                }

                bsls::TimeInterval ti = Util::convertToTimeInterval(dti);
                ASSERTV(LINE,
                        ti,
                        dti,
                        ti.totalMilliseconds() == dti.totalMilliseconds());
                ASSERTV(LINE,
                        ti,
                        dti,
                        ti.nanoseconds() == 1000 * 1000 * dti.milliseconds() +
                                                   1000 * dti.microseconds());
            }
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
