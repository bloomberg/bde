// bdlt_epochutil.t.cpp                                               -*-C++-*-

#include <bdlt_epochutil.h>

#include <bdlt_datetime.h>
#include <bdlt_datetimeinterval.h>

#include <bslim_testutil.h>

#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_new.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_ctime.h>


using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver emphasizes a black-box approach, which is necessarily quite
// voluminous relative to what would be required given white-box knowledge.
// We use the standard table-based test case implementation techniques coupled
// with category partitioning to exercise these utility functions.  We also
// use loop-based, statistical methods to ensure inversion property where
// appropriate.
//-----------------------------------------------------------------------------
// [ 1] Dt& epoch();
// [ 3] Dt convertFromTimeT(bsl::time_t time);
// [ 3] void convertFromTimeT(Dt *result, time_t time);
// [ 3] bsl::time_t convertToTimeT(const Dt& dt);
// [ 3] int convertToTimeT(time_t *result, const Dt& dt);
// [ 2] Dt convertFromTimeT64(TimeT64 time);
// [ 2] int convertFromTimeT64(Dt *result, TimeT64 time);
// [ 2] TimeT64 convertToTimeT64(const Dt& dt);
// [ 2] void convertToTimeT64(TimeT64 *result, const Dt& dt);
// [ 4] Dt convertFromTimeInterval(const TI& tI);
// [ 4] void convertFromTimeInterval(Dt *result, const TI& tI);
// [ 4] TI convertToTimeInterval(const Dt& dt);
// [ 4] int convertToTimeInterval(TI *result, const Dt& dt);
// [ 5] Dt convertFromDatetimeInterval(const DtI& dtI);
// [ 5] void convertFromDatetimeInterval(Dt *result, const DtI& dtI);
// [ 5] DtI convertToDatetimeInterval(const Dt& dt);
// [ 5] int convertToDatetimeInterval(DtI *result, const Dt& dt);
//-----------------------------------------------------------------------------
// [ 6] USAGE EXAMPLE

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

typedef bdlt::EpochUtil     Util;
typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

int epochAddressIsNotZero = 0;
int epochBuffer[sizeof(bdlt::Datetime) / sizeof(int) + 1] = { 0 };

class EarlyEpochCopier
{
  public:
    EarlyEpochCopier();
        // Create an object of this type.

    static const bdlt::Datetime& copiedValue();
        // Return a reference providing non-modifiable access to the 'Datetime'
        // object stored in the 'epochBuffer'.
};

EarlyEpochCopier::EarlyEpochCopier()
{
    const bdlt::Datetime &epoch = bdlt::EpochUtil::epoch();

    // The following code wants to check that the 'epoch' method returns a
    // valid reference regardless of how early in the execution process this
    // method is called, since the epoch object is meant to be statically
    // initialized at load time.  However, the C++ standard says that a null
    // reference cannot exist in a well-defined program, so it's not clear that
    // this test could ever fail - compilers may even elide it.  It should work
    // with the naive implementation of doing what the code says, though.

    if (0 != &epoch) {
        epochAddressIsNotZero = 1;
        new(epochBuffer) bdlt::Datetime(bdlt::EpochUtil::epoch());
    }
}

const bdlt::Datetime &EarlyEpochCopier::copiedValue()
{
    // The following code violates C++ strict aliasing rules.  Sufficiently
    // naive compilers should yield the expected result, though.

    return *reinterpret_cast<bdlt::Datetime *>(epochBuffer);
}

// Direct compilers to move initialization earlier so we can verify the access
// actually works from ALL dynamic initializers.

#define INITATTR

#if defined(BSLS_PLATFORM_CMP_MSVC)
#pragma init_seg(compiler)
#elif defined(BSLS_PLATFORM_CMP_CLANG)
#if __has_attribute(init_priority)
#undef INITATTR
#define INITATTR __attribute__((init_priority(101)))
#endif
#elif defined(BSLS_PLATFORM_CMP_GNU) && defined(BSLS_PLATFORM_OS_LINUX)
#undef INITATTR
#define INITATTR __attribute__((init_priority(101)))
#elif defined(BSLS_PLATFORM_CMP_IBM)
#pragma priority(-2147482623)
#endif

EarlyEpochCopier earlyEpochCopier INITATTR;


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
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
///Example 1: Converting Between Various Representations of Time
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// When processing date/time data, we are often required to deal with a variety
// of ways in which to represent that data, and therefore we need to be able to
// convert between those representations.  We can use the methods contained in
// 'bdlt::EpochUtil' to do this.
//
// First, we set up date/time input values in a variety of formats.  We'll use
// 900ms past midnight of January 1, 2000 as the base date and time, dropping
// the 900ms if the resolution of a format doesn't support it:
//..
    const bsl::time_t            inputTime         (946684800);
    const bsls::TimeInterval     inputTimeInterval (946684800, 900000000);
    const bdlt::DatetimeInterval inputDatetimeInterval(
                                                  0, 0, 0, 0, 946684800900LL);
    const bdlt::Datetime         inputDatetime     (2000, 1, 1, 0, 0, 0, 900);
//..
// Then, we set up a set of output variables to receive converted values:
//..
    bsl::time_t            outputTime;
    bsls::TimeInterval     outputTimeInterval;
    bdlt::DatetimeInterval outputDatetimeInterval;
    bdlt::Datetime         outputDatetime;
//..
// Next, because 'bdlt::EpochUtil' uses 'bdlt::Datetime' as the common format
// for conversion, we will set up a pair of variables in this format to
// represent the values we expect to see:
//..
    const bdlt::Datetime epochDatetimeWithMs   (2000, 1, 1, 0, 0, 0, 900);
    const bdlt::Datetime epochDatetimeWithoutMs(2000, 1, 1, 0, 0, 0, 0);
//..
// Now, we perform a set of conversions to 'bdlt::Datetime' and verify that the
// results are correct.  We will use the conversion methods that return by
// value:
//..
    outputDatetime = bdlt::EpochUtil::convertFromTimeT(inputTime);
    ASSERT(epochDatetimeWithoutMs == outputDatetime);

    outputDatetime =
                   bdlt::EpochUtil::convertFromTimeInterval(inputTimeInterval);
    ASSERT(epochDatetimeWithMs    == outputDatetime);

    outputDatetime =
           bdlt::EpochUtil::convertFromDatetimeInterval(inputDatetimeInterval);
    ASSERT(epochDatetimeWithMs    == outputDatetime);
//..
// Finally, we perform a set of conversions from 'bdlt::Datetime' and verify
// that the results are correct.  This time, for variety, we will illustrate
// the conversion methods which return through an object pointer:
//..
    ASSERT(0 == bdlt::EpochUtil::convertToTimeT(&outputTime, inputDatetime));
    ASSERT(inputTime             == outputTime);

    ASSERT(0 == bdlt::EpochUtil::convertToTimeInterval(&outputTimeInterval,
                                                       inputDatetime));
    ASSERT(inputTimeInterval     == outputTimeInterval);

    ASSERT(0 == bdlt::EpochUtil::convertToDatetimeInterval(
                                                       &outputDatetimeInterval,
                                                       inputDatetime));
    ASSERT(inputDatetimeInterval == outputDatetimeInterval);
//..
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CONVERT BDLT_DATETIME TO/FROM BDLT_DATETIMEINTERVAL
        //
        // Concerns:
        //: 1 All bdlt::Datetime fields are converted properly.
        //:
        //: 2 Time = 24:00:00.000 converts to 00:00:00, not to 24:00:00.
        //:
        //: 3 Limit values of 'bdlt::Datetime' convert properly.
        //:
        //: 4 Datetime values producing negative 'bdlt::DatetimeInterval'
        //:   values fail.
        //:
        //: 5 Non-zero status value implies no change to result
        //:
        //: 6 All relevant fields from bdlt::DatetimeInterval are recovered
        //:   properly.
        //:
        //: 7 In safe mode, contract violations are detected.
        //
        // Plan:
        //: 1 Test convertToDatetimeInterval (C-1..5,7)
        //:   * Table-Based Implementation
        //:   * Category Partitioning Data Selection
        //:   * Orthogonal Perturbation:
        //:     * Unaltered Initial Values
        //:
        //:   Construct a table in which each input field, status, and output
        //:   value are represented in separate columns.  A failure status
        //:   implies that the result is not changed, which is verified within
        //:   the body of the loop on two separate initial values.  In safe
        //:   mode, check that assertions trigger.  Note that the body of the
        //:   main loop will also be used to partially test
        //:   'convertFromDatetimeInterval'.
        //:
        //: 2 Test convertFromDatetimeInterval (C-6,7)
        //:   * Reuse
        //:   * Table-Based Implementation
        //:   * Category Partitioning Data Selection
        //:   * Exploit proven inverse operation: 'convertToDatetimeInterval'
        //:   * Orthogonal Perturbation:
        //:     * Unaltered Initial Values
        //:
        //:     A) First, REUSE the table used to test
        //:        'convertToDatetimeInterval' to reverse every successful
        //:        conversion to 'bdlt::DatetimeInterval', and compare that
        //:        result against the initial input.
        //:
        //:     B) Second, create a separate table that explicitly converts
        //:        valid (non-negative) 'bdlt::DatetimeInterval' to
        //:        'bdlt::Datetime' values with input and individual output
        //:        fields represented as separate columns.  For each result,
        //:        that the input is the table's expected value is verified
        //:        via the proven 'convertToDatetimeInterval';
        //:        'convertFromDatetimeInterval' is then applied, and the
        //:        result of that calculation is compared with the expected
        //:        result values in the table.
        //:
        //: 3 Test 'convertToDatetimeInterval' and then
        //:   'convertFromDatetimeInterval' (C-1,6)
        //:   * Exploit Inverse Relationship
        //:   * Loop-Based Implementation]
        //:   * Pseudo-Random Data Selection
        //:   * Exploiting Inverse Relationship
        //:
        //:     Use a loop-based approach to verify that pseudo-randomly
        //:     selected, non-repeating datetime values that can be converted
        //:     to and then from 'bdlt::DatetimeInterval' objects result in
        //:     exactly the same object.  Note that the intermediate
        //:     'bdlt::DatetimeInterval' object is initialized to a different
        //:     "garbage" value on each iteration.
        //
        // Testing:
        //   int convertToDatetimeInterval(DtI *result, const Dt& dt);
        //   DtI convertToDatetimeInterval(const Dt& dt);
        //   void convertFromDatetimeInterval(Dt *result, const DtI& dtI);
        //   Dt convertFromDatetimeInterval(const DtI& dtI);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "CONVERT BDLT_DATETIME TO/FROM BDLT_DATETIMEINTERVAL"
                 << endl
                 << "==================================================="
                 << endl;

        enum { FAILURE = 1 };

        if (verbose) {
            cout << "\nbdlt::Datetime => bdlt::DatetimeInterval." << endl;
        }

        {
            static const struct {
                int                      d_lineNum;      // source line number
                int                      d_year;         // input
                int                      d_month;        // input
                int                      d_day;          // input
                int                      d_hour;         // input
                int                      d_minute;       // input
                int                      d_second;       // input
                int                      d_millisecond;  // input
                int                      d_status;       // value returned by
                                                         // function
                Int64                    d_retmsec;      // output
            } DATA[] = {
                   // <---------- input ---------->   <--expected--------->
                //lin year mon day hou min sec msec   s      msec
                //--- ---- --- --- --- --- --- ----   -      ----

                    // *** out-of-range input values fail ***      Note:
                //lin year mon day hou min sec msec   s      msec   ld =
                //--- ---- --- --- --- --- --- ----   -      ----   Leap Day
                { L_,    1,  1,  1,  0,  0,  0,   0,  FAILURE, 0 },

                { L_, 1869, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
                { L_, 1879, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
                { L_, 1883, 10, 20, 12, 49, 20, 123,  FAILURE, 0 },
                { L_, 1889, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
                { L_, 1899, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
                { L_, 1909, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
                { L_, 1919, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
                { L_, 1925,  5, 28,  5,  9, 40, 321,  FAILURE, 0 },
                { L_, 1929, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
                { L_, 1939, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
                { L_, 1944,  7,  8, 13, 18, 33, 951,  FAILURE, 0 },
                { L_, 1949, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
                { L_, 1959, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },

                { L_, 1969, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
                { L_, 1970,  1,  1,  0,  0,  0,   0,  0,       0 }, //  0

                { L_, 1980,  1,  1,  0,  0,  0,   0,  0,
                                                  315532800000LL }, //  2
                { L_, 1990,  1,  1,  0,  0,  0,   0,  0,
                                                  631152000000LL }, //  5
                { L_, 2000,  1,  1,  0,  0,  0,   0,  0,
                                                  946684800000LL }, //  7
                { L_, 2010,  1,  1,  0,  0,  0,   0,  0,
                                                 1262304000000LL }, // 10
                { L_, 2020,  1,  1,  0,  0,  0,   0,  0,
                                                 1577836800000LL }, // 12
                { L_, 2030,  1,  1,  0,  0,  0,   0,  0,
                                                 1893456000000LL }, // 15
                { L_, 2038,  1,  1,  0,  0,  0,   0,  0,
                                                 2145916800000LL }, // 17

                { L_, 2038,  1, 19,  0,  0,  0,   0,  0,
                                                 2147472000000LL }, // 17
                { L_, 2038,  1, 19,  3,  0,  0,   0,  0,
                                                 2147482800000LL }, // 17
                { L_, 2038,  1, 19,  3, 14,  0,   0,  0,
                                                 2147483640000LL }, // 17
                { L_, 2038,  1, 19,  3, 14,  7,   0,  0,
                                                 2147483647000LL }, // 17
                { L_, 2038,  1, 19,  3, 14,  8,   0,  0,
                                                 2147483648000LL }, // 17

                { L_, 2048,  1, 19,  3, 14,  8,   0,  0,
                                                 2463016448000LL }, // 19
                { L_, 2058,  1, 19,  3, 14,  8,   0,  0,
                                                 2778635648000LL }, // 22
                { L_, 2068,  1, 19,  3, 14,  8,   0,  0,
                                                 3094168448000LL }, // 24
                { L_, 2078,  1, 19,  3, 14,  8,   0,  0,
                                                 3409787648000LL }, // 27
                { L_, 2088,  1, 19,  3, 14,  8,   0,  0,
                                                 3725320448000LL }, // 29
                { L_, 2098,  1, 19,  3, 14,  8,   0,  0,
                                                 4040939648000LL }, // 32
                { L_, 2108,  1, 19,  3, 14,  8,   0,  0,
                                                 4356386048000LL }, // 33

                { L_, 9999, 12, 31,  0,  0,  0,   0,  0,
                                               253402214400000LL }, // 1947
                { L_, 9999, 12, 31, 23,  0,  0,   0,  0,
                                               253402297200000LL }, // 1947
                { L_, 9999, 12, 31, 23, 59,  0,   0,  0,
                                               253402300740000LL }, // 1947
                { L_, 9999, 12, 31, 23, 59, 59,   0,  0,
                                               253402300799000LL }, // 1947
                { L_, 9999, 12, 31, 23, 59, 59, 999,  0,
                                               253402300799999LL }, // 1947

                    // *** All fields are converted properly.  ***
                //lin year mon day hou min sec msec   s      msec
                //--- ---- --- --- --- --- --- ----   -      ----
                { L_, 1971,  1,  1,  0,  0,  0,  0,  0, 31536000000LL }, // yr
                { L_, 1972,  1,  1,  0,  0,  0,  0,  0, 63072000000LL }, // yr
                { L_, 1973,  1,  1,  0,  0,  0,  0,  0, 94694400000LL }, // yr

                { L_, 1971,  2,  1,  0,  0,  0,  0,  0, 34214400000LL }, // mon
                { L_, 1971,  3,  1,  0,  0,  0,  0,  0, 36633600000LL }, // mon
                { L_, 1971, 12,  1,  0,  0,  0,  0,  0, 60393600000LL }, // mon

                { L_, 1972,  2,  1,  0,  0,  0,  0,  0, 65750400000LL }, // mon
                { L_, 1972,  3,  1,  0,  0,  0,  0,  0, 68256000000LL }, // mon
                { L_, 1972, 12,  1,  0,  0,  0,  0,  0, 92016000000LL }, // mon

                { L_, 1972,  1, 30,  0,  0,  0,  0,  0, 65577600000LL }, // day
                { L_, 1972,  1, 31,  0,  0,  0,  0,  0, 65664000000LL }, // day
                { L_, 1972,  2, 29,  0,  0,  0,  0,  0, 68169600000LL }, // day

                { L_, 1972,  3, 31,  0,  0,  0,  0,  0, 70848000000LL }, // day
                { L_, 1972,  4, 30,  0,  0,  0,  0,  0, 73440000000LL }, // day
                { L_, 1972, 12, 31,  0,  0,  0,  0,  0, 94608000000LL }, // day

                { L_, 1972,  1,  1,  1,  0,  0,  0,  0, 63075600000LL }, // hr
                { L_, 1972,  1,  1, 23,  0,  0,  0,  0, 63154800000LL }, // hr

                { L_, 1972,  1,  1,  0,  1,  0,  0,  0, 63072060000LL }, // min
                { L_, 1972,  1,  1,  0, 59,  0,  0,  0, 63075540000LL }, // min

                { L_, 1972,  1,  1,  0,  0,  1,  0,  0, 63072001000LL }, // sec
                { L_, 1972,  1,  1,  0,  0, 59,  0,  0, 63072059000LL }, // sec

                    // *** The millisecond field is converted properly.  ***
                //lin year mon day hou min sec msec   s      msec
                //--- ---- --- --- --- --- --- ----   -      ----
                { L_, 1972,  1,  1,  0,  0,  0,   0,  0, 63072000000LL }, // ms
                { L_, 1972,  1,  1,  0,  0,  0,   1,  0, 63072000001LL }, // ms
                { L_, 1972,  1,  1,  0,  0,  0,   9,  0, 63072000009LL }, // ms
                { L_, 1972,  1,  1,  0,  0,  0,  10,  0, 63072000010LL }, // ms
                { L_, 1972,  1,  1,  0,  0,  0,  90,  0, 63072000090LL }, // ms
                { L_, 1972,  1,  1,  0,  0,  0,  99,  0, 63072000099LL }, // ms
                { L_, 1972,  1,  1,  0,  0,  0, 100,  0, 63072000100LL }, // ms
                { L_, 1972,  1,  1,  0,  0,  0, 900,  0, 63072000900LL }, // ms
                { L_, 1972,  1,  1,  0,  0,  0, 999,  0, 63072000999LL }, // ms

                    // *** Time = 24:00:00:000 converts to 00:00:00 ***
                //lin year mon day hou min sec msec   s      msec
                //--- ---- --- --- --- --- --- ----   -      ----
                { L_,    1,  1,  1, 24,  0,  0,   0,  FAILURE, 0 },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            // PERTURBATION: Arbitrary initial time values in order to verify
            //               "No Change" to 'result' on FAILURE.

            static const bsl::time_t INITIAL_VALUES[] = {
                // standard b-box int partition
                INT_MIN, INT_MIN + 1, -1, 0, 1, INT_MAX
            };

            const int NUM_INITIAL_VALUES =
              static_cast<int>(sizeof INITIAL_VALUES / sizeof *INITIAL_VALUES);

            // MAIN TEST-TABLE LOOP

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const int LINE   = DATA[ti].d_lineNum;
                const int YEAR   = DATA[ti].d_year;
                const int MONTH  = DATA[ti].d_month;
                const int DAY    = DATA[ti].d_day;
                const int HOUR   = DATA[ti].d_hour;
                const int MINUTE = DATA[ti].d_minute;
                const int SECOND = DATA[ti].d_second;
                const int MSEC   = DATA[ti].d_millisecond;
                const int STATUS = DATA[ti].d_status;

                const Int64 RETMSEC = DATA[ti].d_retmsec;

                if (veryVerbose) {
                    cout << "\n--------------------------------------" << endl;
                    P_(LINE) P_(YEAR) P_(MONTH) P(DAY)
                    T_ P_(HOUR) P_(MINUTE) P_(SECOND) P(MSEC)
                    T_ P_(STATUS) P(RETMSEC)
                }

                const bdlt::Datetime         INPUT(YEAR, MONTH, DAY,
                                                   HOUR, MINUTE, SECOND, MSEC);
                const bdlt::DatetimeInterval OUTPUT(0, 0, 0, 0, RETMSEC);

                if (veryVerbose) { P(INPUT) }

                ASSERTV(LINE, !!STATUS == STATUS); // double check

                for (int vi = 0; vi < NUM_INITIAL_VALUES; ++vi) {
                    const int CONTROL = static_cast<int>(INITIAL_VALUES[vi]);
                    if (veryVeryVerbose) { P(CONTROL) }
                    bdlt::DatetimeInterval       result(0, 0, 0, 0, CONTROL);
                    const bdlt::DatetimeInterval ORIGINAL(0, 0, 0, 0, CONTROL);

                    if (veryVerbose) { cout << "Before: "; P(result); }
                    int status =
                        Util::convertToDatetimeInterval(&result, INPUT);
                                                                        // TEST
                    if (veryVerbose) { cout << "After: "; P(result); }

                    ASSERTV(LINE, vi, STATUS, status, !STATUS == !status);
                                                                   // black-box

                    bdlt::DatetimeInterval resultDup(0, 0, 0, 0, CONTROL);
                    if (veryVerbose) { cout << "Before: "; P(resultDup); }

                    if (0 == STATUS) {
                        resultDup = Util::convertToDatetimeInterval(INPUT);
                    }

                    if (veryVerbose) { cout << "After: "; P(resultDup); }

                    if (STATUS != 0) {
                            // *** Bad status implies no change to result.  ***
                        ASSERTV(LINE, vi, CONTROL, result,
                                     ORIGINAL == result);
                        ASSERTV(LINE, vi, CONTROL, result,
                                     ORIGINAL == resultDup);
                    }
                    else {
                        ASSERTV(LINE, vi, OUTPUT, result, OUTPUT == result);
                        ASSERTV(LINE, vi, OUTPUT, result, OUTPUT == resultDup);
                    }

                       // *** REUSE THIS LOOP TO PARTIALLY TEST
                       //                       convertFromDatetimeInterval ***

                    if (vi) {  // Don't repeat more than once per row.
                        continue;
                    }

                    if (STATUS) {  // Conversion failed; nothing to reverse.
                        continue;
                    }

                    bdlt::Datetime    result2(1, 1, 1, 0, 0, 0, 0);
                                                                 // unreachable
                    bdlt::Datetime result2Dup(1, 1, 1, 0, 0, 0, 0);
                                                                 // unreachable

                    if (veryVerbose) { cout << "Before: "; P(result2) }
                    Util::convertFromDatetimeInterval(&result2, result);
                                                                        // TEST
                    if (veryVerbose) { cout << "After: "; P(result2) }

                    if (veryVerbose) { cout << "Before: "; P(result2Dup) }
                    result2Dup = Util::convertFromDatetimeInterval(resultDup);
                                                                        // TEST
                    if (veryVerbose) { cout << "After: "; P(result2Dup) }

                    if (veryVeryVerbose) { P(INPUT) }
                    bdlt::Datetime tmp2(INPUT);
                    tmp2.setHour(tmp2.hour() % 24);

                    const bdlt::Datetime INPUT2(tmp2);
                    if (veryVerbose) { P(INPUT2) }

                        // *** All relevant fields are recovered properly.  ***
                    ASSERTV(LINE, INPUT2, result2, INPUT2 == result2);
                    ASSERTV(LINE, INPUT2, result2Dup, INPUT2 == result2Dup);
                } // end for vi

                // NEGATIVE TEST
                {
                    bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

                    if (veryVerbose) {
                        cout << "Negative test - expect "
                             << (0 == STATUS ? "success " : "failure ");
                        P(INPUT)
                    }

                    if (0 == STATUS) {
                        ASSERT_SAFE_PASS(
                                       Util::convertToDatetimeInterval(INPUT));
                    }
                    else {
                        ASSERT_SAFE_FAIL(
                                       Util::convertToDatetimeInterval(INPUT));
                    }
                }
            } // end for ti
        }

        if (verbose) {
            cout << "\nbdlt::DatetimeInterval => bdlt::Datetime." << endl;
        }

        {
            static const struct {
                int d_lineNum;  // source line number

                Int64 d_input_ms;   // initial time_t value

                int d_year;    // expected field of result
                int d_month;   // expected field of result
                int d_day;     // expected field of result
                int d_hour;    // expected field of result
                int d_minute;  // expected field of result
                int d_second;  // expected field of result
                int d_msec;    // expected field of result

            } DATA[] = {           // <------- result ------->
                //lin   input value   year mon day hou min sec
                //---   -----------   ---- --- --- --- --- ---

                    // *** All relevant fields are recovered properly.  ***
                //lin       input (ms)  year mon day hou min sec  ms
                //---   --------------  ---- --- --- --- --- --- ---
                { L_,             1000, 1970,  1,  1,  0,  0,  1, 0 }, // sec
                { L_,            59000, 1970,  1,  1,  0,  0, 59, 0 },

                { L_,            60000, 1970,  1,  1,  0,  1,  0, 0 }, // min
                { L_,          3599000, 1970,  1,  1,  0, 59, 59, 0 }, //

                { L_,          3600000, 1970,  1,  1,  1,  0,  0, 0 }, // hour
                { L_,         86399000, 1970,  1,  1, 23, 59, 59, 0 },

                { L_,         86400000, 1970,  1,  2,  0,  0,  0, 0 }, // day
                { L_,       2147483647, 1970,  1, 25, 20, 31, 23,
                                                                647 }, // int
                { L_,          INT_MAX, 1970,  1, 25, 20, 31, 23,
                                                                647 },
                { L_,     2678399000LL, 1970,  1, 31, 23, 59, 59, 0 },

                { L_,     2678400000LL, 1970,  2,  1,  0,  0,  0, 0 }, // mon
                { L_,    31535999000LL, 1970, 12, 31, 23, 59, 59, 0 },

                { L_,    31536000000LL, 1971,  1,  1,  0,  0,  0, 0 }, // year
                { L_,    63072000000LL, 1972,  1,  1,  0,  0,  0, 0 }, // leap
                { L_,    94694400000LL, 1973,  1,  1,  0,  0,  0, 0 },
                { L_,   126230400000LL, 1974,  1,  1,  0,  0,  0, 0 },

                { L_,   441763200000LL, 1984,  1,  1,  0,  0,  0, 0 }, // dec.
                { L_,   757382400000LL, 1994,  1,  1,  0,  0,  0, 0 },
                { L_,  1072915200000LL, 2004,  1,  1,  0,  0,  0, 0 },
                { L_,  1388534400000LL, 2014,  1,  1,  0,  0,  0, 0 },
                { L_,  1704067200000LL, 2024,  1,  1,  0,  0,  0, 0 },
                { L_,  2019686400000LL, 2034,  1,  1,  0,  0,  0, 0 },

                { L_,  2021253247000LL, 2034,  1, 19,  3, 14,  7, 0 }, // year
                { L_,  2052789247000LL, 2035,  1, 19,  3, 14,  7, 0 },
                { L_,  2084325247000LL, 2036,  1, 19,  3, 14,  7, 0 }, // leap
                { L_,  2115947647000LL, 2037,  1, 19,  3, 14,  7, 0 },

                { L_,  2144805247000LL, 2037, 12, 19,  3, 14,  7, 0 }, // mon
                { L_,  2147480047000LL, 2038,  1, 19,  2, 14,  7, 0 }, // hour
                { L_,  2147483587000LL, 2038,  1, 19,  3, 13,  7, 0 }, // min
                { L_,  2147483646000LL, 2038,  1, 19,  3, 14,  6, 0 }, // sec

                { L_,  2463016448000LL, 2048,  1, 19,  3, 14,  8, 0 },
                { L_,  2778635648000LL, 2058,  1, 19,  3, 14,  8, 0 },
                { L_,  3094168448000LL, 2068,  1, 19,  3, 14,  8, 0 },
                { L_,  3409787648000LL, 2078,  1, 19,  3, 14,  8, 0 },
                { L_,  3725320448000LL, 2088,  1, 19,  3, 14,  8, 0 },
                { L_,  4040939648000LL, 2098,  1, 19,  3, 14,  8, 0 },
                { L_,  4356386048000LL, 2108,  1, 19,  3, 14,  8, 0 },
                { L_,253402214400000LL, 9999, 12, 31,  0,  0,  0, 0 },
                { L_,253402297200000LL, 9999, 12, 31, 23,  0,  0, 0 },
                { L_,253402300740000LL, 9999, 12, 31, 23, 59,  0, 0 },
                { L_,253402300799000LL, 9999, 12, 31, 23, 59, 59, 0 }, // MAX

                { L_,    63072000000LL, 1972,  1,  1,  0,  0,  0, 0 }, // ms
                { L_,    63072000001LL, 1972,  1,  1,  0,  0,  0, 1 },
                { L_,    63072000009LL, 1972,  1,  1,  0,  0,  0, 9 },
                { L_,    63072000010LL, 1972,  1,  1,  0,  0,  0, 10 },
                { L_,    63072000090LL, 1972,  1,  1,  0,  0,  0, 90 },
                { L_,    63072000100LL, 1972,  1,  1,  0,  0,  0, 100 },
                { L_,    63072000900LL, 1972,  1,  1,  0,  0,  0, 900 },
                { L_,    63072000999LL, 1972,  1,  1,  0,  0,  0, 999 },
             };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            // MAIN TEST-TABLE LOOP

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE     = DATA[ti].d_lineNum;

                const Int64 INPUT_MS = DATA[ti].d_input_ms;

                const int   YEAR     = DATA[ti].d_year;
                const int   MONTH    = DATA[ti].d_month;
                const int   DAY      = DATA[ti].d_day;
                const int   HOUR     = DATA[ti].d_hour;
                const int   MINUTE   = DATA[ti].d_minute;
                const int   SECOND   = DATA[ti].d_second;
                const int   MSEC     = DATA[ti].d_msec;

                if (veryVerbose) { T_ P(INPUT_MS) }

                bdlt::DatetimeInterval INPUT(0, 0, 0, 0, INPUT_MS);

                {
                    // Double check that the table itself is right.
                    bdlt::Datetime         dt(YEAR, MONTH, DAY,
                                              HOUR, MINUTE, SECOND, MSEC);
                    bdlt::DatetimeInterval ti;

                    int s = Util::convertToDatetimeInterval(&ti, dt);
                    ASSERTV(LINE, s, 0 == s);
                    ASSERTV(LINE, ti, INPUT, ti == INPUT);
                }

                bdlt::Datetime result(1, 2, 3, 4, 5, 6, 7);
                bdlt::Datetime resultDup(1, 2, 3, 4, 5, 6, 7);

                if (veryVerbose) { cout << "Before: "; P(result) }
                Util::convertFromDatetimeInterval(&result, INPUT);   // TEST
                if (veryVerbose) { cout << " After: "; P_(result) }

                if (veryVerbose) { cout << "Before: "; P(resultDup) }
                resultDup = Util::convertFromDatetimeInterval(INPUT);  // TEST
                if (veryVerbose) { cout << " After: "; P_(resultDup) }
                    // *** All relevant fields are recovered properly.  ***
                ASSERTV(LINE, YEAR,     result.year(),
                              YEAR   == result.year());
                ASSERTV(LINE, MONTH,    result.month(),
                              MONTH  == result.month());
                ASSERTV(LINE, DAY,      result.day(),
                              DAY    == result.day());
                ASSERTV(LINE, HOUR,     result.hour(),
                              HOUR   == result.hour());
                ASSERTV(LINE, MINUTE,   result.minute(),
                              MINUTE == result.minute());
                ASSERTV(LINE, SECOND,   result.second(),
                              SECOND == result.second());
                ASSERTV(LINE, MSEC,     result.millisecond(),
                              MSEC   == result.millisecond());

                ASSERTV(LINE, YEAR,     resultDup.year(),
                              YEAR   == resultDup.year());
                ASSERTV(LINE, MONTH,    resultDup.month(),
                              MONTH  == resultDup.month());
                ASSERTV(LINE, DAY,      resultDup.day(),
                              DAY    == resultDup.day());
                ASSERTV(LINE, HOUR,     resultDup.hour(),
                              HOUR   == resultDup.hour());
                ASSERTV(LINE, MINUTE,   resultDup.minute(),
                              MINUTE == resultDup.minute());
                ASSERTV(LINE, SECOND,   resultDup.second(),
                              SECOND == resultDup.second());
                ASSERTV(LINE, MSEC,     resultDup.millisecond(),
                              MSEC   == resultDup.millisecond());
            } // end for ti
        }

        if (verbose) {
            cout << "\nbdlt::DatetimeInterval => bdlt::Datetime => "
                    "bdlt::DatetimeInterval." << endl;
        }

        {
            unsigned int SIZE = unsigned(INT_MAX) + 1;

            if (veryVerbose) { P(SIZE) }

            unsigned int numTrials = 1000; // default with no v-v-verbose

            const int PRIME = 123456789; // at least relatively prime to size

            ASSERT(SIZE % PRIME);        // ensure SIZE not divisible by PRIME

            double percentCovered = 100 * double(numTrials) / SIZE;

            if (verbose) { T_ P_(numTrials) P(percentCovered) }

            const unsigned int STARTING_VALUE = 0;

            unsigned int pseudoRandomValue = STARTING_VALUE;

            for (unsigned int i = 0; i < numTrials; ++i) {
                // Ensure that there is no premature repetition; ok first time.

                ASSERTV(i, (STARTING_VALUE != pseudoRandomValue) != !i);

                bdlt::DatetimeInterval currentValue(0, 0, 0, 0,
                                                    pseudoRandomValue);
                pseudoRandomValue += PRIME;
                pseudoRandomValue %= SIZE;

                const bdlt::Datetime INITIAL_VALUE(
                                1 +  3 * i % 9999,
                                1 +  5 * i % 12,
                                1 +  7 * i % 28,
                                0 + 11 * i % 24,
                                0 + 13 * i % 60,
                                0 + 17 * i % 60,
                                0 + 19 * i % 1000); // pseudo random values
                if (veryVeryVerbose) { P(INITIAL_VALUE) }
                bdlt::Datetime tmp(INITIAL_VALUE);
                bdlt::Datetime tmpDup(INITIAL_VALUE);
                if (veryVeryVerbose) { cout << "Before: "; P(tmp) }
                Util::convertFromDatetimeInterval(&tmp, currentValue);
                if (veryVeryVerbose) { cout << " After: "; P(tmp) }

                if (veryVeryVerbose) { cout << "Before: "; P(tmpDup) }
                tmpDup = Util::convertFromDatetimeInterval(currentValue);
                if (veryVeryVerbose) { cout << " After: "; P(tmpDup) }

                bdlt::DatetimeInterval result(-1, -1, -1, -1, -1);
                                                               // out of bounds
                bdlt::DatetimeInterval resultDup(-1, -1, -1, -1, -1);
                                                               // out of bounds

                if (veryVeryVerbose) { cout<<"Before: "; P(result) }
                ASSERTV(i, 0 == Util::convertToDatetimeInterval(&result, tmp));
                if (veryVeryVerbose) { cout<<" After: "; P(result) }

                if (veryVeryVerbose) { cout<<"Before: "; P(resultDup) }
                resultDup = Util::convertToDatetimeInterval(tmpDup);
                if (veryVeryVerbose) { cout<<" After: "; P(resultDup) }

                ASSERTV(i, currentValue == result);
                ASSERTV(i, currentValue == resultDup);
            }
        }

        // Negative test for null pointers.
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);
            if (verbose) {
                cout << "Negative test: convertToDatetimeInterval(null, x)"
                     << endl;
            }
            ASSERT_SAFE_FAIL(
                         Util::convertToDatetimeInterval(0, bdlt::Datetime()));

            if (verbose) {
                cout << "Negative test: convertFromDatetimeInterval(null, x)"
                     << endl;
            }
            ASSERT_SAFE_FAIL(
               Util::convertFromDatetimeInterval(0, bdlt::DatetimeInterval()));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CONVERT BDLT_DATETIME TO/FROM BSLS_TIMEINTERVAL
        //
        // Concerns:
        //: 1 All bdlt::Datetime fields are converted properly.
        //:
        //: 2 Time = 24:00:00.000 converts to 00:00:00, not to 24:00:00.
        //:
        //: 3 Limit values of 'bdlt::Datetime' convert properly.
        //:
        //: 4 Datetime values producing negative 'bdlt::TimeInterval' values
        //:   fail.
        //:
        //: 5 Non-zero status value implies no change to result
        //:
        //: 6 All relevant fields from bdlt::TimeInterval are recovered
        //:   properly.
        //:
        //: 7 In safe mode, contract violations are detected.
        //
        // Plan:
        //: 1 Test convertToTimeInterval (C-1..5,7)
        //:   * Table-Based Implementation
        //:   * Category Partitioning Data Selection
        //:   * Orthogonal Perturbation:
        //:     * Unaltered Initial Values
        //:
        //:   Construct a table in which each input field, status, and output
        //:   value are represented in separate columns.  A failure status
        //:   implies that the result is not changed, which is verified within
        //:   the body of the loop on two separate initial values.  In safe
        //:   mode, check that assertions trigger.  Note that the body of the
        //:   main loop will also be used to partially test
        //:   'convertFromTimeInterval'.
        //:
        //: 2 Test convertFromTimeInterval (C-6,7)
        //:   * Reuse
        //:   * Table-Based Implementation
        //:   * Category Partitioning Data Selection
        //:   * Exploit proven inverse operation: 'convertToTimeInterval'
        //:   * Orthogonal Perturbation:
        //:     * Unaltered Initial Values
        //:
        //:     A) First, REUSE the table used to test 'convertToTimeInterval'
        //:        to reverse every successful conversion to
        //:        'bdlt::TimeInterval', and compare that result against
        //:        the initial input.
        //:
        //:     B) Second, create a separate table that explicitly converts
        //:        valid (non-negative) 'bdlt::TimeInterval' to
        //:        'bdlt::Datetime' values with input and individual output
        //:        fields represented as separate columns.  For each result,
        //:        that the input is the table's expected value is verified via
        //:        the proven 'convertToTimeInterval';
        //:        'convertFromTimeInterval' is then applied, and the result of
        //:        that calculation is compared with the expected result values
        //:        in the table.
        //:
        //: 3 Test 'convertToTimeInterval' and then
        //:   'convertFromTimeInterval' (C-1,6)
        //:   * Exploit Inverse Relationship
        //:   * Loop-Based Implementation
        //:   * Pseudo-Random Data Selection
        //:   * Exploiting Inverse Relationship
        //:
        //:     Use a loop-based approach to verify that pseudo-randomly
        //:     selected, non-repeating datetime values that can be converted
        //:     to and then from 'bdlt::TimeInterval' objects result in
        //:     exactly the same object.  Note that the intermediate
        //:     'bdlt::TimeInterval' object is initialized to a different
        //:     "garbage" value on each iteration.
        //
        // Testing:
        //   int convertToTimeInterval(TI *result, const Dt& dt);
        //   TI convertToTimeInterval(const Dt& dt);
        //   void convertFromTimeInterval(Dt *result, const TI& tI);
        //   Dt convertFromTimeInterval(const TI& tI);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONVERT BDLT_DATETIME TO/FROM BSLS_TIMEINTERVAL"
                          << endl
                          << "==============================================="
                          << endl;

        enum { FAILURE = 1 };

        if (verbose) {
            cout << "\nbdlt::Datetime => bsls::TimeInterval." << endl;
        }

        {
            static const struct {
                int   d_lineNum;      // source line number
                int   d_year;         // input
                int   d_month;        // input
                int   d_day;          // input
                int   d_hour;         // input
                int   d_minute;       // input
                int   d_second;       // input
                int   d_millisecond;  // input
                int   d_status;       // value returned by function
                Int64 d_retsec;       // output
                int   d_retnsec;      // output
            } DATA[] = {
                   // <---------- input ---------->   <--expected------------>
                //lin year mon day hou min sec msec   s      sec  nsec
                //--- ---- --- --- --- --- --- ----   -      ---  ----

                    // *** out-of-range input values fail ***          Note:
                //lin year mon day hou min sec msec   s      sec  nsec ld =
                //--- ---- --- --- --- --- --- ----   -      ---  ---- Leap Day
                { L_,    1,  1,  1,  0,  0,  0,   0,  FAILURE, 0,    0 },

                { L_, 1869, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
                { L_, 1879, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
                { L_, 1883, 10, 20, 12, 49, 20, 123,  FAILURE, 0,    0 },
                { L_, 1889, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
                { L_, 1899, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
                { L_, 1909, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
                { L_, 1919, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
                { L_, 1925,  5, 28,  5,  9, 40, 321,  FAILURE, 0,    0 },
                { L_, 1929, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
                { L_, 1939, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
                { L_, 1944,  7,  8, 13, 18, 33, 951,  FAILURE, 0,    0 },
                { L_, 1949, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
                { L_, 1959, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },

                { L_, 1969, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
                { L_, 1970,  1,  1,  0,  0,  0,   0,  0,       0,    0 }, //  0

                { L_, 1980,  1,  1,  0,  0,  0,   0,  0,  315532800, 0 }, //  2
                { L_, 1990,  1,  1,  0,  0,  0,   0,  0,  631152000, 0 }, //  5
                { L_, 2000,  1,  1,  0,  0,  0,   0,  0,  946684800, 0 }, //  7
                { L_, 2010,  1,  1,  0,  0,  0,   0,  0, 1262304000, 0 }, // 10
                { L_, 2020,  1,  1,  0,  0,  0,   0,  0, 1577836800, 0 }, // 12
                { L_, 2030,  1,  1,  0,  0,  0,   0,  0, 1893456000, 0 }, // 15
                { L_, 2038,  1,  1,  0,  0,  0,   0,  0, 2145916800, 0 }, // 17

                { L_, 2038,  1, 19,  0,  0,  0,   0,  0, 2147472000, 0 }, // 17
                { L_, 2038,  1, 19,  3,  0,  0,   0,  0, 2147482800, 0 }, // 17
                { L_, 2038,  1, 19,  3, 14,  0,   0,  0, 2147483640, 0 }, // 17
                { L_, 2038,  1, 19,  3, 14,  7,   0,  0, 2147483647, 0 }, // 17
                { L_, 2038,  1, 19,  3, 14,  8,   0,  0, 2147483648LL,
                                                                     0 }, // 17

                { L_, 2048,  1, 19,  3, 14,  8,   0,  0, 2463016448LL,
                                                                     0}, // 19
                { L_, 2058,  1, 19,  3, 14,  8,   0,  0, 2778635648LL,
                                                                     0}, // 22
                { L_, 2068,  1, 19,  3, 14,  8,   0,  0, 3094168448LL,
                                                                     0}, // 24
                { L_, 2078,  1, 19,  3, 14,  8,   0,  0, 3409787648LL,
                                                                     0}, // 27
                { L_, 2088,  1, 19,  3, 14,  8,   0,  0, 3725320448LL,
                                                                     0}, // 29
                { L_, 2098,  1, 19,  3, 14,  8,   0,  0, 4040939648LL,
                                                                     0}, // 32
                { L_, 2108,  1, 19,  3, 14,  8,   0,  0, 4356386048LL,
                                                                     0}, // 33

                { L_, 9999, 12, 31,  0,  0,  0,   0,  0, 253402214400LL,
                                                                   0 }, // 1947
                { L_, 9999, 12, 31, 23,  0,  0,   0,  0, 253402297200LL,
                                                                   0 }, // 1947
                { L_, 9999, 12, 31, 23, 59,  0,   0,  0, 253402300740LL,
                                                                   0 }, // 1947
                { L_, 9999, 12, 31, 23, 59, 59,   0,  0, 253402300799LL,
                                                                   0 }, // 1947
                { L_, 9999, 12, 31, 23, 59, 59, 999,  0, 253402300799LL,
                                                           999000000 }, // 1947

                    // *** All fields are converted properly.  ***
                //lin year mon day hou min sec msec  s      sec  nsec
                //--- ---- --- --- --- --- --- ----  -      ---  ----
                { L_, 1971,  1,  1,  0,  0,  0,  0,  0, 31536000, 0 }, // year
                { L_, 1972,  1,  1,  0,  0,  0,  0,  0, 63072000, 0 }, // year
                { L_, 1973,  1,  1,  0,  0,  0,  0,  0, 94694400, 0 }, // year

                { L_, 1971,  2,  1,  0,  0,  0,  0,  0, 34214400, 0 }, // month
                { L_, 1971,  3,  1,  0,  0,  0,  0,  0, 36633600, 0 }, // month
                { L_, 1971, 12,  1,  0,  0,  0,  0,  0, 60393600, 0 }, // month

                { L_, 1972,  2,  1,  0,  0,  0,  0,  0, 65750400, 0 }, // month
                { L_, 1972,  3,  1,  0,  0,  0,  0,  0, 68256000, 0 }, // month
                { L_, 1972, 12,  1,  0,  0,  0,  0,  0, 92016000, 0 }, // month

                { L_, 1972,  1, 30,  0,  0,  0,  0,  0, 65577600, 0 }, // day
                { L_, 1972,  1, 31,  0,  0,  0,  0,  0, 65664000, 0 }, // day
                { L_, 1972,  2, 29,  0,  0,  0,  0,  0, 68169600, 0 }, // day

                { L_, 1972,  3, 31,  0,  0,  0,  0,  0, 70848000, 0 }, // day
                { L_, 1972,  4, 30,  0,  0,  0,  0,  0, 73440000, 0 }, // day
                { L_, 1972, 12, 31,  0,  0,  0,  0,  0, 94608000, 0 }, // day

                { L_, 1972,  1,  1,  1,  0,  0,  0,  0, 63075600, 0 }, // hour
                { L_, 1972,  1,  1, 23,  0,  0,  0,  0, 63154800, 0 }, // hour

                { L_, 1972,  1,  1,  0,  1,  0,  0,  0, 63072060, 0 }, // min
                { L_, 1972,  1,  1,  0, 59,  0,  0,  0, 63075540, 0 }, // min

                { L_, 1972,  1,  1,  0,  0,  1,  0,  0, 63072001, 0 }, // sec
                { L_, 1972,  1,  1,  0,  0, 59,  0,  0, 63072059, 0 }, // sec

                    // *** The millisecond field is converted properly.  ***
                //lin year mon day hou min sec msec   s       sec  nsec
                //--- ---- --- --- --- --- --- ----   -       ---  ----
                { L_, 1972,  1,  1,  0,  0,  0,   0,  0, 63072000, 0 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,   1,  0, 63072000,
                                                             1000000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,   9,  0, 63072000,
                                                             9000000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,  10,  0, 63072000,
                                                            10000000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,  90,  0, 63072000,
                                                            90000000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,  99,  0, 63072000,
                                                            99000000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0, 100,  0, 63072000,
                                                           100000000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0, 900,  0, 63072000,
                                                           900000000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0, 999,  0, 63072000,
                                                           999000000 }, // msec

                    // *** Time = 24:00:00:000 converts to 00:00:00 ***
                //lin year mon day hou min sec msec  s       sec  nsec
                //--- ---- --- --- --- --- --- ----  -       ---  ----
                { L_,    1,  1,  1, 24,  0,  0,   0, FAILURE, 0,    0 },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            // PERTURBATION: Arbitrary initial time values in order to verify
            //               "No Change" to 'result' on FAILURE.

            static const bsl::time_t INITIAL_VALUES[] = {
                // standard b-box int partition
                INT_MIN, INT_MIN + 1, -1, 0, 1, INT_MAX
            };

            const int NUM_INITIAL_VALUES =
              static_cast<int>(sizeof INITIAL_VALUES / sizeof *INITIAL_VALUES);

            // MAIN TEST-TABLE LOOP

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE    = DATA[ti].d_lineNum;
                const int   YEAR    = DATA[ti].d_year;
                const int   MONTH   = DATA[ti].d_month;
                const int   DAY     = DATA[ti].d_day;
                const int   HOUR    = DATA[ti].d_hour;
                const int   MINUTE  = DATA[ti].d_minute;
                const int   SECOND  = DATA[ti].d_second;
                const int   MSEC    = DATA[ti].d_millisecond;
                const int   STATUS  = DATA[ti].d_status;
                const Int64 RETSEC  = DATA[ti].d_retsec;
                const int   RETNSEC = DATA[ti].d_retnsec;

                if (veryVerbose) {
                    cout << "\n--------------------------------------" << endl;
                    P_(LINE) P_(YEAR) P_(MONTH) P(DAY)
                    T_ P_(HOUR) P_(MINUTE) P_(SECOND) P(MSEC)
                    T_ P_(STATUS) P_(RETSEC) P(RETNSEC)
                }

                const bdlt::Datetime     INPUT(YEAR, MONTH, DAY,
                                               HOUR, MINUTE, SECOND, MSEC);
                const bsls::TimeInterval OUTPUT(RETSEC, RETNSEC);

                if (veryVerbose) { P(INPUT) }

                ASSERTV(LINE, !!STATUS == STATUS); // double check

                for (int vi = 0; vi < NUM_INITIAL_VALUES; ++vi) {
                    const int CONTROL = static_cast<int>(INITIAL_VALUES[vi]);
                    if (veryVeryVerbose) { P(CONTROL) }
                    bsls::TimeInterval       result(CONTROL, 0);
                    const bsls::TimeInterval ORIGINAL(CONTROL, 0);

                    if (veryVerbose) { cout << "Before: "; P(result) }
                    int status = Util::convertToTimeInterval(&result, INPUT);
                    if (veryVerbose) { cout << "After: "; P(result) }

                    ASSERTV(LINE, vi, STATUS, status, !STATUS == !status);
                                                                   // black-box

                    bsls::TimeInterval resultDup(CONTROL, 0);
                    if (veryVerbose) { cout << "Before: "; P(resultDup) }

                    if (0 == STATUS) {
                        resultDup = Util::convertToTimeInterval(INPUT);
                    }

                    if (veryVerbose) { cout << "After: "; P(resultDup) }

                    if (STATUS != 0) {
                            // *** Bad status implies no change to result.  ***
                        ASSERTV(LINE, vi, CONTROL, result,
                                     ORIGINAL == result);
                        ASSERTV(LINE, vi, CONTROL, result,
                                     ORIGINAL == resultDup);
                    }
                    else {
                        ASSERTV(LINE, vi, OUTPUT, result, OUTPUT == result);
                        ASSERTV(LINE, vi, OUTPUT, result, OUTPUT == resultDup);
                    }

                        // *** REUSE THIS LOOP TO PARTIALLY TEST
                        //                          convertFromTimeInterval ***

                    if (vi) {  // Don't repeat more than once per row.
                        continue;
                    }

                    if (STATUS) {  // Conversion failed; nothing to reverse.
                        continue;
                    }

                    bdlt::Datetime    result2(1, 1, 1, 0, 0, 0, 0);
                                                                 // unreachable
                    bdlt::Datetime result2Dup(1, 1, 1, 0, 0, 0, 0);
                                                                 // unreachable

                    if (veryVerbose) { cout << "Before: "; P(result2) }
                    Util::convertFromTimeInterval(&result2, result);
                    if (veryVerbose) { cout << "After: "; P(result2) }

                    if (veryVerbose) { cout << "Before: "; P(result2Dup) }
                    result2Dup = Util::convertFromTimeInterval(resultDup);
                    if (veryVerbose) { cout << "After: "; P(result2Dup) }

                    if (veryVeryVerbose) { P(INPUT) }
                    bdlt::Datetime tmp2(INPUT);
                    tmp2.setHour(tmp2.hour() % 24);

                    const bdlt::Datetime INPUT2(tmp2);
                    if (veryVerbose) { P(INPUT2) }

                        // *** All relevant fields are recovered properly.  ***
                    ASSERTV(LINE, INPUT2, result2, INPUT2 == result2);
                    ASSERTV(LINE, INPUT2, result2Dup, INPUT2 == result2Dup);
                } // end for vi

                // NEGATIVE TEST
                {
                    bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

                    if (veryVerbose) {
                        cout << "Negative test - expect "
                             << (0 == STATUS ? "success " : "failure ");
                        P(INPUT)
                    }

                    if (0 == STATUS) {
                        ASSERT_SAFE_PASS(Util::convertToTimeInterval(INPUT));
                    }
                    else {
                        ASSERT_SAFE_FAIL(Util::convertToTimeInterval(INPUT));
                    }
                }
            } // end for ti
        }

        if (verbose) {
            cout << "\nbsls::TimeInterval => bdlt::Datetime." << endl;
        }

        {
            static const struct {
                int   d_lineNum;   // source line number
                Int64 d_input_s;   // initial time_t value
                int   d_input_ns;  // initial time_t value
                int   d_year;      // expected field of result
                int   d_month;     // expected field of result
                int   d_day;       // expected field of result
                int   d_hour;      // expected field of result
                int   d_minute;    // expected field of result
                int   d_second;    // expected field of result
                int   d_msec;      // expected field of result
            } DATA[] = {           // <------- result ------->
                //lin   input value   year mon day hou min sec
                //---   -----------   ---- --- --- --- --- ---

                    // *** All relevant fields are recovered properly.  ***
                //lin     input (s) (ns) year mon day hou min sec  ms
                //---   ----------- ---- ---- --- --- --- --- --- ---
                { L_,             1,  0, 1970,  1,  1,  0,  0,  1, 0 }, // sec
                { L_,            59,  0, 1970,  1,  1,  0,  0, 59, 0 },

                { L_,            60,  0, 1970,  1,  1,  0,  1,  0, 0 }, // min
                { L_,          3599,  0, 1970,  1,  1,  0, 59, 59, 0 }, //

                { L_,          3600,  0, 1970,  1,  1,  1,  0,  0, 0 }, // hour
                { L_,         86399,  0, 1970,  1,  1, 23, 59, 59, 0 },

                { L_,         86400,  0, 1970,  1,  2,  0,  0,  0, 0 }, // day
                { L_,       2678399,  0, 1970,  1, 31, 23, 59, 59, 0 },

                { L_,       2678400,  0, 1970,  2,  1,  0,  0,  0, 0 }, // mon
                { L_,      31535999,  0, 1970, 12, 31, 23, 59, 59, 0 },

                { L_,      31536000,  0, 1971,  1,  1,  0,  0,  0, 0 }, // year
                { L_,      63072000,  0, 1972,  1,  1,  0,  0,  0, 0 }, // leap
                { L_,      94694400,  0, 1973,  1,  1,  0,  0,  0, 0 },
                { L_,     126230400,  0, 1974,  1,  1,  0,  0,  0, 0 },

                { L_,     441763200,  0, 1984,  1,  1,  0,  0,  0, 0 }, // dec.
                { L_,     757382400,  0, 1994,  1,  1,  0,  0,  0, 0 },
                { L_,    1072915200,  0, 2004,  1,  1,  0,  0,  0, 0 },
                { L_,    1388534400,  0, 2014,  1,  1,  0,  0,  0, 0 },
                { L_,    1704067200,  0, 2024,  1,  1,  0,  0,  0, 0 },
                { L_,    2019686400,  0, 2034,  1,  1,  0,  0,  0, 0 },

                { L_,    2021253247,  0, 2034,  1, 19,  3, 14,  7, 0 }, // year
                { L_,    2052789247,  0, 2035,  1, 19,  3, 14,  7, 0 },
                { L_,    2084325247,  0, 2036,  1, 19,  3, 14,  7, 0 }, // leap
                { L_,    2115947647,  0, 2037,  1, 19,  3, 14,  7, 0 },

                { L_,    2144805247,  0, 2037, 12, 19,  3, 14,  7, 0 }, // mon
                { L_,    2147480047,  0, 2038,  1, 19,  2, 14,  7, 0 }, // hour
                { L_,    2147483587,  0, 2038,  1, 19,  3, 13,  7, 0 }, // min
                { L_,    2147483646,  0, 2038,  1, 19,  3, 14,  6, 0 }, // sec

                { L_,    2147483647,  0, 2038,  1, 19,  3, 14,  7, 0 }, // int
                { L_,       INT_MAX,  0, 2038,  1, 19,  3, 14,  7, 0 },

                { L_,  2463016448LL,  0, 2048,  1, 19,  3, 14,  8, 0 },
                { L_,  2778635648LL,  0, 2058,  1, 19,  3, 14,  8, 0 },
                { L_,  3094168448LL,  0, 2068,  1, 19,  3, 14,  8, 0 },
                { L_,  3409787648LL,  0, 2078,  1, 19,  3, 14,  8, 0 },
                { L_,  3725320448LL,  0, 2088,  1, 19,  3, 14,  8, 0 },
                { L_,  4040939648LL,  0, 2098,  1, 19,  3, 14,  8, 0 },
                { L_,  4356386048LL,  0, 2108,  1, 19,  3, 14,  8, 0 },
                { L_, 253402214400LL, 0, 9999, 12, 31,  0,  0,  0, 0 },
                { L_, 253402297200LL, 0, 9999, 12, 31, 23,  0,  0, 0 },
                { L_, 253402300740LL, 0, 9999, 12, 31, 23, 59,  0, 0 },
                { L_, 253402300799LL, 0, 9999, 12, 31, 23, 59, 59, 0 }, // MAX

                { L_,      63072000,  1,
                                         1972,  1,  1,  0,  0,  0, 0 }, // ms
                { L_,      63072000, 10,
                                         1972,  1,  1,  0,  0,  0, 0 },
                { L_,      63072000, 100,
                                         1972,  1,  1,  0,  0,  0, 0 },
                { L_,      63072000, 1000,
                                         1972,  1,  1,  0,  0,  0, 0 },
                { L_,      63072000, 10000,
                                         1972,  1,  1,  0,  0,  0, 0 },
                { L_,      63072000, 100000,
                                         1972,  1,  1,  0,  0,  0, 0 },
                { L_,      63072000, 1000000,
                                         1972,  1,  1,  0,  0,  0, 1 },
                { L_,      63072000, 9000000,
                                         1972,  1,  1,  0,  0,  0, 9 },
                { L_,      63072000, 10000000,
                                         1972,  1,  1,  0,  0,  0, 10 },
                { L_,      63072000, 90000000,
                                         1972,  1,  1,  0,  0,  0, 90 },
                { L_,      63072000, 100000000,
                                         1972,  1,  1,  0,  0,  0, 100 },
                { L_,      63072000, 900000000,
                                         1972,  1,  1,  0,  0,  0, 900 },
                { L_,      63072000, 999000000,
                                         1972,  1,  1,  0,  0,  0, 999 },
                { L_,      63072000, 999123456,
                                         1972,  1,  1,  0,  0,  0, 999 },
                { L_,      63072000, 999654321,
                                         1972,  1,  1,  0,  0,  0, 999 },
                { L_,      63072000, 998999999,
                                         1972,  1,  1,  0,  0,  0, 998 },
                { L_,      63072000, 999999999,
                                         1972,  1,  1,  0,  0,  0, 999 },
             };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            // MAIN TEST-TABLE LOOP

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE     = DATA[ti].d_lineNum;

                const Int64 INPUT_S  = DATA[ti].d_input_s;
                const int   INPUT_NS = DATA[ti].d_input_ns;

                const int   YEAR     = DATA[ti].d_year;
                const int   MONTH    = DATA[ti].d_month;
                const int   DAY      = DATA[ti].d_day;
                const int   HOUR     = DATA[ti].d_hour;
                const int   MINUTE   = DATA[ti].d_minute;
                const int   SECOND   = DATA[ti].d_second;
                const int   MSEC     = DATA[ti].d_msec;

                if (veryVerbose) { T_ P_(INPUT_S) P(INPUT_NS) }

                bsls::TimeInterval INPUT(INPUT_S, INPUT_NS);
                bsls::TimeInterval INPUT2(INPUT_S,
                                          INPUT_NS / 1000000 * 1000000);

                {
                    // Double check that the table itself is right.
                    bdlt::Datetime     dt(YEAR, MONTH, DAY,
                                          HOUR, MINUTE, SECOND, MSEC);
                    bsls::TimeInterval ti;

                    int s = Util::convertToTimeInterval(&ti, dt);
                    ASSERTV(LINE, s, 0 == s);
                    ASSERTV(LINE, ti, INPUT2, ti == INPUT2);
                }

                bdlt::Datetime result(1, 2, 3, 4, 5, 6, 7);
                bdlt::Datetime resultDup(1, 2, 3, 4, 5, 6, 7);

                if (veryVerbose) { cout << "Before: "; P(result) }
                Util::convertFromTimeInterval(&result, INPUT);
                if (veryVerbose) { cout << " After: "; P_(result) }

                if (veryVerbose) { cout << "Before: "; P(resultDup) }
                resultDup = Util::convertFromTimeInterval(INPUT);
                if (veryVerbose) { cout << " After: "; P_(resultDup) }
                    // *** All relevant fields are recovered properly.  ***
                ASSERTV(LINE, YEAR,     result.year(),
                              YEAR   == result.year());
                ASSERTV(LINE, MONTH,    result.month(),
                              MONTH  == result.month());
                ASSERTV(LINE, DAY,      result.day(),
                              DAY    == result.day());
                ASSERTV(LINE, HOUR,     result.hour(),
                              HOUR   == result.hour());
                ASSERTV(LINE, MINUTE,   result.minute(),
                              MINUTE == result.minute());
                ASSERTV(LINE, SECOND,   result.second(),
                              SECOND == result.second());
                ASSERTV(LINE, MSEC,     result.millisecond(),
                              MSEC   == result.millisecond());

                ASSERTV(LINE, YEAR,     resultDup.year(),
                              YEAR   == resultDup.year());
                ASSERTV(LINE, MONTH,    resultDup.month(),
                              MONTH  == resultDup.month());
                ASSERTV(LINE, DAY,      resultDup.day(),
                              DAY    == resultDup.day());
                ASSERTV(LINE, HOUR,     resultDup.hour(),
                              HOUR   == resultDup.hour());
                ASSERTV(LINE, MINUTE,   resultDup.minute(),
                              MINUTE == resultDup.minute());
                ASSERTV(LINE, SECOND,   resultDup.second(),
                              SECOND == resultDup.second());
                ASSERTV(LINE, MSEC,     resultDup.millisecond(),
                              MSEC   == resultDup.millisecond());
            } // end for ti
        }

        if (verbose) {
            cout << "\nbsls::TimeInterval => bdlt::Datetime => "
                    "bsls::TimeInterval." << endl;
        }

        {
            unsigned int SIZE = unsigned(INT_MAX) + 1;

            if (veryVerbose) { P(SIZE) }
                                            // to allow abs value to count!
            unsigned int numTrials = 1000;  // default with no v-v-verbose

            const int PRIME = 123456789;  // at least relatively prime to size

            ASSERT(SIZE % PRIME);  // ensure SIZE not divisible by PRIME

            double percentCovered = 100 * double(numTrials) / SIZE;

            if (verbose) { T_ P_(numTrials) P(percentCovered) }

            const unsigned int STARTING_VALUE = 0;

            unsigned int pseudoRandomValue = STARTING_VALUE;

            for (unsigned int i = 0; i < numTrials; ++i) {
                // Ensure that there is no premature repetition; ok first time.

                ASSERTV(i, (STARTING_VALUE != pseudoRandomValue) != !i);

                bsls::TimeInterval currentValue(pseudoRandomValue, 0);
                pseudoRandomValue += PRIME;
                pseudoRandomValue %= SIZE;

                const bdlt::Datetime INITIAL_VALUE(
                                1 +  3 * i % 9999,
                                1 +  5 * i % 12,
                                1 +  7 * i % 28,
                                0 + 11 * i % 24,
                                0 + 13 * i % 60,
                                0 + 17 * i % 60,
                                0 + 19 * i % 1000); // pseudo random values
                if (veryVeryVerbose) { P(INITIAL_VALUE) }
                bdlt::Datetime tmp(INITIAL_VALUE);
                bdlt::Datetime tmpDup(INITIAL_VALUE);
                if (veryVeryVerbose) { cout << "Before: "; P(tmp) }
                Util::convertFromTimeInterval(&tmp, currentValue);
                if (veryVeryVerbose) { cout << " After: "; P(tmp) }

                if (veryVeryVerbose) { cout << "Before: "; P(tmpDup) }
                tmpDup = Util::convertFromTimeInterval(currentValue);
                if (veryVeryVerbose) { cout << " After: "; P(tmpDup) }

                bsls::TimeInterval result(-1, -1);    // out of bounds
                bsls::TimeInterval resultDup(-1, -1); // out of bounds

                if (veryVeryVerbose) { cout<<"Before: "; P(result) }
                ASSERTV(i, 0 == Util::convertToTimeInterval(&result, tmp));
                if (veryVeryVerbose) { cout<<" After: "; P(result) }

                if (veryVeryVerbose) { cout<<"Before: "; P(resultDup) }
                resultDup = Util::convertToTimeInterval(tmpDup);
                if (veryVeryVerbose) { cout<<" After: "; P(resultDup) }

                ASSERTV(i, currentValue == result);
                ASSERTV(i, currentValue == resultDup);
            }
        }

        // Negative test for null pointers.
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);
            if (verbose) {
                cout << "Negative test: convertToTimeInterval(null, x)"
                     << endl;
            }
            ASSERT_SAFE_FAIL(Util::convertToTimeInterval(0, bdlt::Datetime()));

            if (verbose) {
                cout << "Negative test: convertFromTimeInterval(null, x)"
                     << endl;
            }
            ASSERT_SAFE_FAIL(
                       Util::convertFromTimeInterval(0, bsls::TimeInterval()));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CONVERT BDLT_DATETIME TO/FROM TIME_T
        //
        // Concerns:
        //: 1 All bdlt::Datetime fields are converted properly.
        //:
        //: 2 Time = 24:00:00.000 converts to 00:00:00, not to 24:00:00.
        //:
        //: 3 The millisecond field is ignored.
        //:
        //: 4 Datetime values producing out-of-range time_t values fail:
        //:   a) time_t < 0
        //:   b) time_t > 2^31 - 1
        //:   c) internal 32-bit integer temporaries do not overflow.
        //:
        //: 5 Non-zero status value implies no change to result
        //:
        //: 6 All relevant fields from time_t are recovered properly.
        //:
        //: 7 Milliseconds are set to 0 when converting from time_t.
        //:
        //: 8 In safe mode, contract violations are detected.
        //
        // Plan:
        //: 1 Test convertToTimeT (C-1..5,8)
        //:   * Table-Based Implementation
        //:   * Category Partitioning Data Selection
        //:   * Orthogonal Perturbation:
        //:     * Unaltered Initial Values
        //:
        //:   Construct a table in which each input field, status, and output
        //:   value are represented in separate columns.  A failure status
        //:   implies that the result is not changed, which is verified within
        //:   the body of the loop on two separate initial values.  In safe
        //:   mode, check that assertions trigger.  Note that the body of the
        //:   main loop will also be used to partially test 'convertFromTimeT'.
        //:
        //: 2 Test convertFromTimeT (C-6..8)
        //:   * Reuse
        //:   * Table-Based Implementation
        //:   * Category Partitioning Data Selection
        //:   * Exploit proven inverse operation: 'convertToTimeT'
        //:   * Orthogonal Perturbation:
        //:     * Unaltered Initial Values
        //:
        //:     A) First, REUSE the table used to test 'convertToTimeT' to
        //:        reverse every successful conversion to 'time_t', and compare
        //:        that result against the initial input, except in cases where
        //:        milliseconds were initially non-zero.
        //:
        //:     B) Second, create a separate table that explicitly converts
        //:        valid (non-negative) 'time_t' to 'bdlt::Datetime' values
        //:        with input and individual output fields represented as
        //:        separate columns.  For each result, that the input is the
        //:        table's expected value is verified via the proven
        //:        'convertToTimeT'; 'convertFromTimeT' is then applied, and
        //:        the result of that calculation is compared with the expected
        //:        result values in the table.  Note that the 'millisecond'
        //:        field is necessarily 0, and is tested directly within the
        //:        main loop.
        //:
        //: 3 Test 'convertToTimeT' and then 'convertFromTimeT' (C-1,6)
        //:   * Exploit Inverse Relationship
        //:   * Loop-Based Implementation
        //:   * Pseudo-Random Data Selection
        //:   * Exploiting Inverse Relationship
        //:
        //:     Use a loop-based approach to verify that pseudo-randomly
        //:     selected, non-repeating datetime values that can be converted
        //:     to and then from 'struct tm' objects result in exactly the same
        //:     object.  Note that the intermediate 'time_t' object is
        //:     initialized to a different "garbage" value on each iteration.
        //
        // Testing:
        //   int convertToTimeT(time_t *result, const Dt& dt);
        //   bsl::time_t convertToTimeT(const Dt& dt);
        //   void convertFromTimeT(Dt *result, time_t time);
        //   Dt convertFromTimeT(bsl::time_t time);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONVERT BDLT_DATETIME TO/FROM TIME_T" << endl
                          << "====================================" << endl;

        enum { FAILURE = 1 };

        if (verbose) {
            cout << "\nbdlt::Datetime => time_t." << endl;
        }

        {
            static const struct {
                int d_lineNum;      // source line number
                int d_year;         // input
                int d_month;        // input
                int d_day;          // input
                int d_hour;         // input
                int d_minute;       // input
                int d_second;       // input
                int d_millisecond;  // input
                int d_status;       // value returned by function

                bsl::time_t d_time;      // output
            } DATA[] = {
                   // <---------- input ---------->   <--expected->
                //lin year mon day hou min sec msec   s      result
                //--- ---- --- --- --- --- --- ----   -      ------

                    // *** out-of-range input values fail ***          Note:
                //lin year mon day hou min sec msec   s      result    ld =
                //--- ---- --- --- --- --- --- ----   -      ------    Leap Day
                { L_,    1,  1,  1,  0,  0,  0,   0,  FAILURE,    0 },

                { L_, 1869, 12, 31, 23, 59, 59, 999,  FAILURE,    0 },
                { L_, 1879, 12, 31, 23, 59, 59, 999,  FAILURE,    0 },
                { L_, 1883, 10, 20, 12, 49, 20, 123,  FAILURE,    0 },
                { L_, 1889, 12, 31, 23, 59, 59, 999,  FAILURE,    0 },
                { L_, 1899, 12, 31, 23, 59, 59, 999,  FAILURE,    0 },
                { L_, 1909, 12, 31, 23, 59, 59, 999,  FAILURE,    0 },
                { L_, 1919, 12, 31, 23, 59, 59, 999,  FAILURE,    0 },
                { L_, 1925,  5, 28,  5,  9, 40, 321,  FAILURE,    0 },
                { L_, 1929, 12, 31, 23, 59, 59, 999,  FAILURE,    0 },
                { L_, 1939, 12, 31, 23, 59, 59, 999,  FAILURE,    0 },
                { L_, 1944,  7,  8, 13, 18, 33, 951,  FAILURE,    0 },
                { L_, 1949, 12, 31, 23, 59, 59, 999,  FAILURE,    0 },
                { L_, 1959, 12, 31, 23, 59, 59, 999,  FAILURE,    0 },

                { L_, 1969, 12, 31, 23, 59, 59, 999,  FAILURE,    0 },
                { L_, 1970,  1,  1,  0,  0,  0,   0,  0,          0 }, //  0ld

                { L_, 1980,  1,  1,  0,  0,  0,   0,  0,  315532800 }, //  2ld
                { L_, 1990,  1,  1,  0,  0,  0,   0,  0,  631152000 }, //  5ld
                { L_, 2000,  1,  1,  0,  0,  0,   0,  0,  946684800 }, //  7ld
                { L_, 2010,  1,  1,  0,  0,  0,   0,  0, 1262304000 }, // 10ld
                { L_, 2020,  1,  1,  0,  0,  0,   0,  0, 1577836800 }, // 12ld
                { L_, 2030,  1,  1,  0,  0,  0,   0,  0, 1893456000 }, // 15ld
                { L_, 2038,  1,  1,  0,  0,  0,   0,  0, 2145916800 }, // 17ld

                { L_, 2038,  1, 19,  0,  0,  0,   0,  0, 2147472000 }, // 17ld
                { L_, 2038,  1, 19,  3,  0,  0,   0,  0, 2147482800 }, // 17ld
                { L_, 2038,  1, 19,  3, 14,  0,   0,  0, 2147483640 }, // 17ld
                { L_, 2038,  1, 19,  3, 14,  7,   0,  0, 2147483647 }, // 17ld
                { L_, 2038,  1, 19,  3, 14,  8,   0,  FAILURE,    0 },

                { L_, 2048,  1, 19,  3, 14,  8,   0,  FAILURE,    0 },
                { L_, 2058,  1, 19,  3, 14,  8,   0,  FAILURE,    0 },
                { L_, 2068,  1, 19,  3, 14,  8,   0,  FAILURE,    0 },
                { L_, 2078,  1, 19,  3, 14,  8,   0,  FAILURE,    0 },
                { L_, 2088,  1, 19,  3, 14,  8,   0,  FAILURE,    0 },
                { L_, 2098,  1, 19,  3, 14,  8,   0,  FAILURE,    0 },
                { L_, 2108,  1, 19,  3, 14,  8,   0,  FAILURE,    0 },

                { L_, 9999, 12, 31, 23, 59, 59, 999,  FAILURE,    0 },

                    // *** All fields are converted properly.  ***
                //lin year mon day hou min sec msec   s      result
                //--- ---- --- --- --- --- --- ----   -      ------
                { L_, 1971,  1,  1,  0,  0,  0,   0,  0,   31536000 }, // year
                { L_, 1972,  1,  1,  0,  0,  0,   0,  0,   63072000 }, // year
                { L_, 1973,  1,  1,  0,  0,  0,   0,  0,   94694400 }, // year

                { L_, 1971,  2,  1,  0,  0,  0,   0,  0,   34214400 }, // month
                { L_, 1971,  3,  1,  0,  0,  0,   0,  0,   36633600 }, // month
                { L_, 1971, 12,  1,  0,  0,  0,   0,  0,   60393600 }, // month

                { L_, 1972,  2,  1,  0,  0,  0,   0,  0,   65750400 }, // month
                { L_, 1972,  3,  1,  0,  0,  0,   0,  0,   68256000 }, // month
                { L_, 1972, 12,  1,  0,  0,  0,   0,  0,   92016000 }, // month

                { L_, 1972,  1, 30,  0,  0,  0,   0,  0,   65577600 }, // day
                { L_, 1972,  1, 31,  0,  0,  0,   0,  0,   65664000 }, // day
                { L_, 1972,  2, 29,  0,  0,  0,   0,  0,   68169600 }, // day

                { L_, 1972,  3, 31,  0,  0,  0,   0,  0,   70848000 }, // day
                { L_, 1972,  4, 30,  0,  0,  0,   0,  0,   73440000 }, // day
                { L_, 1972, 12, 31,  0,  0,  0,   0,  0,   94608000 }, // day

                { L_, 1972,  1,  1,  1,  0,  0,   0,  0,   63075600 }, // hour
                { L_, 1972,  1,  1, 23,  0,  0,   0,  0,   63154800 }, // hour

                { L_, 1972,  1,  1,  0,  1,  0,   0,  0,   63072060 }, // min
                { L_, 1972,  1,  1,  0, 59,  0,   0,  0,   63075540 }, // min

                { L_, 1972,  1,  1,  0,  0,  1,   0,  0,   63072001 }, // sec
                { L_, 1972,  1,  1,  0,  0, 59,   0,  0,   63072059 }, // sec

                    // *** The millisecond field is ignored.  ***
                //lin year mon day hou min sec msec   s      result
                //--- ---- --- --- --- --- --- ----   -      ------
                { L_, 1972,  1,  1,  0,  0,  0,   0,  0,   63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,   1,  0,   63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,   9,  0,   63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,  10,  0,   63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,  90,  0,   63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,  99,  0,   63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0, 100,  0,   63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0, 900,  0,   63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0, 999,  0,   63072000 }, // msec

                    // *** Time = 24:00:00:000 converts to 00:00:00 ***
                //lin year mon day hou min sec msec   s      result
                //--- ---- --- --- --- --- --- ----   -      ------
                { L_,    1,  1,  1, 24,  0,  0,   0,  FAILURE,    0 },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            // PERTURBATION: Arbitrary initial time values in order to verify
            //               "No Change" to 'result' on FAILURE.

            static const bsl::time_t INITIAL_VALUES[] = {
                // standard b-box int partition
                INT_MIN, INT_MIN + 1, -1, 0, 1, INT_MAX
            };

            const int NUM_INITIAL_VALUES =
              static_cast<int>(sizeof INITIAL_VALUES / sizeof *INITIAL_VALUES);

            // MAIN TEST-TABLE LOOP

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const int LINE   = DATA[ti].d_lineNum;
                const int YEAR   = DATA[ti].d_year;
                const int MONTH  = DATA[ti].d_month;
                const int DAY    = DATA[ti].d_day;
                const int HOUR   = DATA[ti].d_hour;
                const int MINUTE = DATA[ti].d_minute;
                const int SECOND = DATA[ti].d_second;
                const int MSEC   = DATA[ti].d_millisecond;

                const int STATUS = DATA[ti].d_status;

                const int TIME   = static_cast<int>(DATA[ti].d_time);

                if (veryVerbose) {
                    cout << "\n--------------------------------------" << endl;
                    P_(LINE) P_(YEAR) P_(MONTH) P(DAY)
                    T_ P_(HOUR) P_(MINUTE) P_(SECOND) P(MSEC)
                    T_ P_(STATUS) P(TIME)
                }

                const bdlt::Datetime INPUT(YEAR, MONTH, DAY,
                                           HOUR, MINUTE, SECOND, MSEC);

                if (veryVerbose) { P(INPUT) }

                ASSERTV(LINE, !!STATUS == STATUS); // double check

                for (int vi = 0; vi < NUM_INITIAL_VALUES; ++vi) {
                    const bsl::time_t CONTROL = INITIAL_VALUES[vi];
                    if (veryVeryVerbose) { P(CONTROL) }
                    bsl::time_t result = CONTROL;

                    if (veryVerbose) { cout << "Before: "; P(result) }
                    int status = Util::convertToTimeT(&result, INPUT);
                    if (veryVerbose) { cout << "After: "; P(result) }

                    ASSERTV(LINE, vi, STATUS, status, !STATUS == !status);
                                                                   // black-box

                    bsl::time_t resultDup = CONTROL;
                    if (veryVerbose) { cout << "Before: "; P(resultDup) }

                    if (0 == STATUS) {
                        resultDup = Util::convertToTimeT(INPUT);
                    }

                    if (veryVerbose) { cout << "After: "; P(resultDup) }

                    if (STATUS != 0) {
                            // *** Bad status implies no change to result.  ***
                        ASSERTV(LINE, vi, CONTROL, result,
                                CONTROL == result);
                        ASSERTV(LINE, vi, CONTROL, result,
                                CONTROL == resultDup);
                    }
                    else {
                        ASSERTV(LINE, vi, TIME, result, TIME == result);
                        ASSERTV(LINE, vi, TIME, result, TIME == resultDup);
                    }

                        // *** REUSE THIS LOOP TO PARTIALLY TEST
                        //                                 convertFromTimeT ***

                    if (vi) {  // Don't repeat more than once per row.
                        continue;
                    }

                    if (STATUS) {  // Conversion failed; nothing to reverse.
                        continue;
                    }

                    bdlt::Datetime result2(9999, 12, 31, 23, 59, 59, 999);
                                                                 // unreachable
                    bdlt::Datetime result2Dup(9999, 12, 31, 23, 59, 59, 999);
                                                                 // unreachable

                    if (veryVerbose) { cout << "Before: "; P(result2) }
                    Util::convertFromTimeT(&result2, result);
                    if (veryVerbose) { cout << "After: "; P(result2) }

                    if (veryVerbose) { cout << "Before: "; P(result2Dup) }
                    result2Dup = Util::convertFromTimeT(resultDup);
                    if (veryVerbose) { cout << "After: "; P(result2Dup) }

                        // *** The millisecond field is always set to 0.  ***
                    if (veryVeryVerbose) { P(INPUT) }
                    bdlt::Datetime tmp2(INPUT);
                    tmp2.setMillisecond(0);

                        // *** There is no equivalent representation for
                        // 24:00:00 as a time_t
                    tmp2.setHour(tmp2.hour() % 24);

                    const bdlt::Datetime INPUT2(tmp2);
                    if (veryVerbose) { P(INPUT2) }
                    ASSERTV(LINE, INPUT2, 0 == INPUT2.millisecond());
                                                                // double check

                        // *** All relevant fields are recovered properly. ***
                    ASSERTV(LINE, INPUT2, result2, INPUT2 == result2);
                    ASSERTV(LINE, INPUT2, result2Dup, INPUT2 == result2Dup);
                } // end for vi

                // NEGATIVE TEST
                {
                    bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

                    if (veryVerbose) {
                        cout << "Negative test - expect "
                             << (0 == STATUS ? "success " : "failure ");
                        P(INPUT)
                    }

                    if (0 == STATUS) {
                        ASSERT_SAFE_PASS(Util::convertToTimeT(INPUT));
                    }
                    else {
                        ASSERT_SAFE_FAIL(Util::convertToTimeT(INPUT));
                    }
                }
            } // end for ti
        }

        if (verbose) {
            cout << "\ntime_t => bdlt::Datetime." << endl;
        }

        {
            static const struct {
                int d_lineNum; // source line number
                int d_input;   // initial time_t value
                int d_year;    // expected field of result
                int d_month;   // expected field of result
                int d_day;     // expected field of result
                int d_hour;    // expected field of result
                int d_minute;  // expected field of result
                int d_second;  // expected field of result
            } DATA[] = {           // <------- result ------->
                //lin   input value   year mon day hou min sec
                //---   -----------   ---- --- --- --- --- ---

                    // *** All relevant fields are recovered properly.  ***
                //lin   input value   year mon day hou min sec
                //---   -----------   ---- --- --- --- --- ---
                { L_,             1,  1970,  1,  1,  0,  0,  1 }, // sec
                { L_,            59,  1970,  1,  1,  0,  0, 59 },

                { L_,            60,  1970,  1,  1,  0,  1,  0 }, // min
                { L_,          3599,  1970,  1,  1,  0, 59, 59 }, //

                { L_,          3600,  1970,  1,  1,  1,  0,  0 }, // hour
                { L_,         86399,  1970,  1,  1, 23, 59, 59 },

                { L_,         86400,  1970,  1,  2,  0,  0,  0 }, // day
                { L_,       2678399,  1970,  1, 31, 23, 59, 59 },

                { L_,       2678400,  1970,  2,  1,  0,  0,  0 }, // mon
                { L_,      31535999,  1970, 12, 31, 23, 59, 59 },

                { L_,      31536000,  1971,  1,  1,  0,  0,  0 }, // year
                { L_,      63072000,  1972,  1,  1,  0,  0,  0 }, // lp.  yr.
                { L_,      94694400,  1973,  1,  1,  0,  0,  0 },
                { L_,     126230400,  1974,  1,  1,  0,  0,  0 },

                { L_,     441763200,  1984,  1,  1,  0,  0,  0 }, // decade
                { L_,     757382400,  1994,  1,  1,  0,  0,  0 },
                { L_,    1072915200,  2004,  1,  1,  0,  0,  0 },
                { L_,    1388534400,  2014,  1,  1,  0,  0,  0 },
                { L_,    1704067200,  2024,  1,  1,  0,  0,  0 },
                { L_,    2019686400,  2034,  1,  1,  0,  0,  0 },

                { L_,    2021253247,  2034,  1, 19,  3, 14,  7 }, // year
                { L_,    2052789247,  2035,  1, 19,  3, 14,  7 },
                { L_,    2084325247,  2036,  1, 19,  3, 14,  7 }, // lp.  yr.
                { L_,    2115947647,  2037,  1, 19,  3, 14,  7 },

                { L_,    2144805247,  2037, 12, 19,  3, 14,  7 }, // mon
                { L_,    2147480047,  2038,  1, 19,  2, 14,  7 }, // hour
                { L_,    2147483587,  2038,  1, 19,  3, 13,  7 }, // min
                { L_,    2147483646,  2038,  1, 19,  3, 14,  6 }, // sec

                { L_,    2147483647,  2038,  1, 19,  3, 14,  7 }, // top
                { L_,       INT_MAX,  2038,  1, 19,  3, 14,  7 },
             };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            // MAIN TEST-TABLE LOOP

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE   = DATA[ti].d_lineNum;
                const int INPUT  = DATA[ti].d_input;
                const int YEAR   = DATA[ti].d_year;
                const int MONTH  = DATA[ti].d_month;
                const int DAY    = DATA[ti].d_day;
                const int HOUR   = DATA[ti].d_hour;
                const int MINUTE = DATA[ti].d_minute;
                const int SECOND = DATA[ti].d_second;

                {
                    // Double check that the table itself is right.
                    bdlt::Datetime dt(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND);
                    bsl::time_t    tt;

                    int s = Util::convertToTimeT(&tt, dt);
                    ASSERTV(LINE, s, 0 == s);
                    ASSERTV(LINE, tt, INPUT, tt == INPUT);
                }

                bdlt::Datetime result(1, 2, 3, 4, 5, 6, 7);
                bdlt::Datetime resultDup(1, 2, 3, 4, 5, 6, 7);

                if (veryVerbose) { cout << "Before: "; P(result) }
                Util::convertFromTimeT(&result, INPUT);
                if (veryVerbose) { cout << " After: "; P_(result) }

                if (veryVerbose) { cout << "Before: "; P(resultDup) }
                resultDup = Util::convertFromTimeT(INPUT);
                if (veryVerbose) { cout << " After: "; P_(resultDup) }
                    // *** All relevant fields are recovered properly.  ***
                ASSERTV(LINE, YEAR,     result.year(),
                              YEAR   == result.year());
                ASSERTV(LINE, MONTH,    result.month(),
                              MONTH  == result.month());
                ASSERTV(LINE, DAY,      result.day(),
                              DAY    == result.day());
                ASSERTV(LINE, HOUR,     result.hour(),
                              HOUR   == result.hour());
                ASSERTV(LINE, MINUTE,   result.minute(),
                              MINUTE == result.minute());
                ASSERTV(LINE, SECOND,   result.second(),
                              SECOND == result.second());

                ASSERTV(LINE, YEAR,     resultDup.year(),
                              YEAR   == resultDup.year());
                ASSERTV(LINE, MONTH,    resultDup.month(),
                              MONTH  == resultDup.month());
                ASSERTV(LINE, DAY,      resultDup.day(),
                              DAY    == resultDup.day());
                ASSERTV(LINE, HOUR,     resultDup.hour(),
                              HOUR   == resultDup.hour());
                ASSERTV(LINE, MINUTE,   resultDup.minute(),
                              MINUTE == resultDup.minute());
                ASSERTV(LINE, SECOND,   resultDup.second(),
                              SECOND == resultDup.second());

                    // *** The millisecond field is always set to 0.  ***
                ASSERTV(LINE,      result.millisecond(),
                              0 == result.millisecond());
                ASSERTV(LINE,      resultDup.millisecond(),
                              0 == resultDup.millisecond());
            } // end for ti
        }

        if (verbose) {
            cout << "\ntime_t => bdlt::Datetime => time_t." << endl;
        }

        {
            unsigned int SIZE = unsigned(INT_MAX) + 1;

            if (veryVerbose) { P(SIZE) }
                                             // to allow abs value to count!
            unsigned int numTrials = 1000; // default with no v-v-verbose

            const int PRIME = 123456789; // at least relatively prime to size

            ASSERT(SIZE % PRIME);        // ensure SIZE not divisible by PRIME

            double percentCovered = 100 * double(numTrials) / SIZE;

            if (verbose) { T_ P_(numTrials) P(percentCovered) }

            const unsigned int STARTING_VALUE = 0;

            unsigned int pseudoRandomValue = STARTING_VALUE;

            for (unsigned int i = 0; i < numTrials; ++i) {
                // Ensure that there is no premature repetition; ok first time.

                ASSERTV(i, (STARTING_VALUE != pseudoRandomValue) != !i);

                bsl::time_t currentValue = pseudoRandomValue;
                pseudoRandomValue += PRIME;
                pseudoRandomValue %= SIZE;

                const bdlt::Datetime INITIAL_VALUE(
                                1 +  3 * i % 9999,
                                1 +  5 * i % 12,
                                1 +  7 * i % 28,
                                0 + 11 * i % 24,
                                0 + 13 * i % 60,
                                0 + 17 * i % 60,
                                0 + 19 * i % 1000); // pseudo random values
                if (veryVeryVerbose) { P(INITIAL_VALUE) }
                bdlt::Datetime tmp(INITIAL_VALUE);
                bdlt::Datetime tmpDup(INITIAL_VALUE);
                if (veryVeryVerbose) { cout << "Before: "; P(tmp) }
                Util::convertFromTimeT(&tmp, currentValue);
                if (veryVeryVerbose) { cout << " After: "; P(tmp) }

                if (veryVeryVerbose) { cout << "Before: "; P(tmpDup) }
                tmpDup = Util::convertFromTimeT(currentValue);
                if (veryVeryVerbose) { cout << " After: "; P(tmpDup) }

                bsl::time_t result = -int(i); // out of bounds
                bsl::time_t resultDup = -int(i); // out of bounds

                if (veryVeryVerbose) { cout<<"Before: "; P(result) }
                ASSERTV(i, 0 == Util::convertToTimeT(&result, tmp));
                if (veryVeryVerbose) { cout<<" After: "; P(result) }

                if (veryVeryVerbose) { cout<<"Before: "; P(resultDup) }
                resultDup = Util::convertToTimeT(tmpDup);
                if (veryVeryVerbose) { cout<<" After: "; P(resultDup) }

                ASSERTV(i, currentValue == result);
                ASSERTV(i, currentValue == resultDup);
            }
        }

        // Negative test for null pointers.
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);
            if (verbose) {
                cout << "Negative test: convertToTimeT(null, x)" << endl;
            }
            ASSERT_SAFE_FAIL(Util::convertToTimeT(0, bdlt::Datetime()));

            if (verbose) {
                cout << "Negative test: convertFromTimeT(null, x)" << endl;
            }
            ASSERT_SAFE_FAIL(Util::convertFromTimeT(0, bsl::time_t()));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONVERT BDLT_DATETIME TO/FROM BSLS_TYPES::INT64
        //
        // Concerns:
        //: 1 All bdlt::Datetime fields are converted properly.
        //:
        //: 2 Time = 24:00:00.000 converts to 00:00:00, not to 24:00:00.
        //:
        //: 3 The millisecond field is ignored.
        //:
        //: 4 Non-zero status value implies no change to result
        //:
        //: 5 In converting from 'bsls::Types::Int64', all relevant fields are
        //:   recovered properly.
        //:
        //: 6 The millisecond field is always set to 0.
        //:
        //: 7 'bsls::Types::Int64' values producing invalid 'Datetime' values
        //:   fail:
        //:   a) time_t <  -62135596800 (01/01/0001 - 00:00:00)
        //:   b) time_t > 2253402300799 (12/31/9999 - 23:59:59)
        //:   c) internal 64-bit integer temporaries do not overflow.
        //:
        //: 8 In safe mode, contract violations are detected.
        //
        // Plan:
        //: 1 Test convertToTimeT64 (C-1..4)
        //:   * Table-Based Implementation
        //:   * Category Partitioning Data Selection
        //:   * Orthogonal Perturbation:
        //:
        //:   Construct a table in which each input field and output value are
        //:   represented in separate columns.  Note that the body of the main
        //:   loop will also be used to partially test 'convertFromTimeT64'.
        //:
        //: 2 Test convertFromTimeT64 (C-5..8)
        //:   * Reuse
        //:   * Table-Based Implementation
        //:   * Category Partitioning Data Selection
        //:   * Exploit proven inverse operation: 'convertToTimeT64'
        //:   * Orthogonal Perturbation:
        //:     * Unaltered Initial Values
        //:
        //:     A) First, REUSE the table used to test 'convertToTimeT64'
        //:        to reverse every conversion to 'bsls::Types::Int64', and
        //:        compare that result against the initial input, except in
        //:        cases where milliseconds were initially non-zero.
        //:
        //:     B) Second, create a separate table that explicitly converts
        //:        valid 'bsls::Types::Int64' to 'bdlt::Datetime' values with
        //:        input and individual output fields represented as separate
        //:        columns.  For each result that yields a status equal to 0
        //:        (in a separate column) verify that the input is the table's
        //:        expected value via the proven 'convertToTimeT64';
        //:        'convertFromTimeT64' is then applied, and the result of that
        //:        calculation is compared with the expected result values
        //:        in the table.  For each result that yields a non-0 status,
        //:        verify that the result is unchanged from its original value
        //:        after 'convertFromTimeT64'.  In safe mode, check that
        //:        contract violations are detected.  Note that the
        //:        'millisecond' field is necessarily 0, and is tested directly
        //:        within the main loop.
        //:
        //: 3 Test 'convertToTimeT64' and then 'convertFromTimeT64' (C-1,5)
        //:   * Exploit Inverse Relationship
        //:   * Loop-Based Implementation
        //:   * Pseudo-Random Data Selection
        //:   * Exploiting Inverse Relationship
        //:
        //:     Use a loop-based approach to verify that pseudo-randomly
        //:     selected, non-repeating datetime values that can be converted
        //:     to and then from 'bsls::Types::Int64' objects result in exactly
        //:     the same object.  Note that the intermediate
        //:     'bsls::Types::Int64' instance is initialized to a different
        //:     "garbage" value on each iteration.
        //
        // Testing:
        //   Dt convertFromTimeT64(TimeT64 time);
        //   int convertFromTimeT64(Dt *result, TimeT64 time);
        //   TimeT64 convertToTimeT64(const Dt& dt);
        //   void convertToTimeT64(TimeT64 *result, const Dt& dt);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONVERT BDLT_DATETIME TO/FROM BSLS_TYPES::INT64"
                          << endl
                          << "==============================================="
                          << endl;


        enum { FAILURE = 1 };

        if (verbose) {
            cout << "\nbdlt::Datetime => bsls::Types::Int64" << endl;
        }

        {
            static const struct {
                int d_lineNum;      // source line number
                int d_year;         // input
                int d_month;        // input
                int d_day;          // input
                int d_hour;         // input
                int d_minute;       // input
                int d_second;       // input
                int d_millisecond;  // input

                bsls::Types::Int64  d_time;      // output

            } DATA[] = {
                   // <---------- input ---------->   <--expected->

                //lin year mon day hou min sec msec           result   ld =
                //--- ---- --- --- --- --- --- ----  --------------    Leap Day
                { L_,    1,  1,  1,  0,  0,  0,   0,   -62135596800LL },
                { L_, 1869, 12, 31, 23, 59, 59, 999,    -3155673601LL },
                { L_, 1879, 12, 31, 23, 59, 59, 999,    -2840140801LL },
                { L_, 1883, 10, 20, 12, 49, 20, 123,    -2720171440LL },
                { L_, 1889, 12, 31, 23, 59, 59, 999,    -2524521601LL },
                { L_, 1899, 12, 31, 23, 59, 59, 999,    -2208988801LL },
                { L_, 1909, 12, 31, 23, 59, 59, 999,    -1893456001 },
                { L_, 1919, 12, 31, 23, 59, 59, 999,    -1577923201 },
                { L_, 1925,  5, 28,  5,  9, 40, 321,    -1407351020 },
                { L_, 1929, 12, 31, 23, 59, 59, 999,    -1262304001 },
                { L_, 1939, 12, 31, 23, 59, 59, 999,     -946771201 },
                { L_, 1944,  7,  8, 13, 18, 33, 951,     -804163287 },
                { L_, 1949, 12, 31, 23, 59, 59, 999,     -631152001 },
                { L_, 1959, 12, 31, 23, 59, 59, 999,     -315619201 },

                { L_, 1969, 12, 31, 23, 59, 59, 999,             -1 },
                { L_, 1970,  1,  1,  0,  0,  0,   0,              0 }, //  0ld

                { L_, 1980,  1,  1,  0,  0,  0,   0,      315532800 }, //  2ld
                { L_, 1990,  1,  1,  0,  0,  0,   0,      631152000 }, //  5ld
                { L_, 2000,  1,  1,  0,  0,  0,   0,      946684800 }, //  7ld
                { L_, 2010,  1,  1,  0,  0,  0,   0,     1262304000 }, // 10ld
                { L_, 2020,  1,  1,  0,  0,  0,   0,     1577836800 }, // 12ld
                { L_, 2030,  1,  1,  0,  0,  0,   0,     1893456000 }, // 15ld
                { L_, 2038,  1,  1,  0,  0,  0,   0,     2145916800 }, // 17ld

                { L_, 2038,  1, 19,  0,  0,  0,   0,     2147472000 }, // 17ld
                { L_, 2038,  1, 19,  3,  0,  0,   0,     2147482800 }, // 17ld
                { L_, 2038,  1, 19,  3, 14,  0,   0,     2147483640 }, // 17ld
                { L_, 2038,  1, 19,  3, 14,  7,   0,     2147483647 }, // 17ld
                { L_, 2038,  1, 19,  3, 14,  8,   0,     2147483648LL },
                { L_, 2048,  1, 19,  3, 14,  8,   0,     2463016448LL },
                { L_, 2058,  1, 19,  3, 14,  8,   0,     2778635648LL },
                { L_, 2068,  1, 19,  3, 14,  8,   0,     3094168448LL },
                { L_, 2078,  1, 19,  3, 14,  8,   0,     3409787648LL },
                { L_, 2088,  1, 19,  3, 14,  8,   0,     3725320448LL },
                { L_, 2098,  1, 19,  3, 14,  8,   0,     4040939648LL },
                { L_, 2108,  1, 19,  3, 14,  8,   0,     4356386048LL },
                { L_, 9999, 12, 31, 23, 59, 57, 999,   253402300797LL },
                { L_, 9999, 12, 31, 23, 59, 58, 999,   253402300798LL },
                { L_, 9999, 12, 31, 23, 59, 59, 999,   253402300799LL },

                    // *** All fields are converted properly.  ***
                //lin year mon day hou min sec msec          result
                //--- ---- --- --- --- --- --- ----  --------------
                { L_, 1971,  1,  1,  0,  0,  0,   0,       31536000 }, // year
                { L_, 1972,  1,  1,  0,  0,  0,   0,       63072000 }, // year
                { L_, 1973,  1,  1,  0,  0,  0,   0,       94694400 }, // year

                { L_, 1971,  2,  1,  0,  0,  0,   0,       34214400 }, // month
                { L_, 1971,  3,  1,  0,  0,  0,   0,       36633600 }, // month
                { L_, 1971, 12,  1,  0,  0,  0,   0,       60393600 }, // month

                { L_, 1972,  2,  1,  0,  0,  0,   0,       65750400 }, // month
                { L_, 1972,  3,  1,  0,  0,  0,   0,       68256000 }, // month
                { L_, 1972, 12,  1,  0,  0,  0,   0,       92016000 }, // month

                { L_, 1972,  1, 30,  0,  0,  0,   0,       65577600 }, // day
                { L_, 1972,  1, 31,  0,  0,  0,   0,       65664000 }, // day
                { L_, 1972,  2, 29,  0,  0,  0,   0,       68169600 }, // day

                { L_, 1972,  3, 31,  0,  0,  0,   0,       70848000 }, // day
                { L_, 1972,  4, 30,  0,  0,  0,   0,       73440000 }, // day
                { L_, 1972, 12, 31,  0,  0,  0,   0,       94608000 }, // day

                { L_, 1972,  1,  1,  1,  0,  0,   0,       63075600 }, // hour
                { L_, 1972,  1,  1, 23,  0,  0,   0,       63154800 }, // hour

                { L_, 1972,  1,  1,  0,  1,  0,   0,       63072060 }, // min
                { L_, 1972,  1,  1,  0, 59,  0,   0,       63075540 }, // min

                { L_, 1972,  1,  1,  0,  0,  1,   0,       63072001 }, // sec
                { L_, 1972,  1,  1,  0,  0, 59,   0,       63072059 }, // sec

                    // *** The millisecond field is ignored.  ***
                //lin year mon day hou min sec msec          result
                //--- ---- --- --- --- --- --- ----  --------------
                { L_, 1972,  1,  1,  0,  0,  0,   0,       63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,   1,       63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,   9,       63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,  10,       63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,  90,       63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,  99,       63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0, 100,       63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0, 900,       63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0, 999,       63072000 }, // msec

                    // *** Time = 24:00:00:000 converts to 00:00:00 ***
                //lin year mon day hou min sec msec          result
                //--- ---- --- --- --- --- --- ----  --------------
                { L_,    1,  1,  1, 24,  0,  0,   0,   -62135596800LL },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            // PERTURBATION: Arbitrary initial time values in order to verify
            //               "No Change" to 'result' on FAILURE.

            static const bsl::time_t INITIAL_VALUES[] = {
                // standard b-box int partition
                INT_MIN, INT_MIN + 1, -1, 0, 1, INT_MAX
            };

            const int NUM_INITIAL_VALUES =
              static_cast<int>(sizeof INITIAL_VALUES / sizeof *INITIAL_VALUES);

            // MAIN TEST-TABLE LOOP

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const int LINE                = DATA[ti].d_lineNum;
                const int YEAR                = DATA[ti].d_year;
                const int MONTH               = DATA[ti].d_month;
                const int DAY                 = DATA[ti].d_day;
                const int HOUR                = DATA[ti].d_hour;
                const int MINUTE              = DATA[ti].d_minute;
                const int SECOND              = DATA[ti].d_second;
                const int MSEC                = DATA[ti].d_millisecond;

                const bsls::Types::Int64 TIME = DATA[ti].d_time;

                if (veryVerbose) {
                    cout << "\n--------------------------------------" << endl;
                    P_(LINE) P_(YEAR) P_(MONTH) P(DAY)
                    T_ P_(HOUR) P_(MINUTE) P_(SECOND) P(MSEC)
                    T_ P(TIME)
                }

                const bdlt::Datetime INPUT(YEAR, MONTH, DAY,
                                           HOUR, MINUTE, SECOND, MSEC);

                if (veryVerbose) { P(INPUT) }

                for (int vi = 0; vi < NUM_INITIAL_VALUES; ++vi) {
                    const bsl::time_t CONTROL = INITIAL_VALUES[vi];
                    if (veryVeryVerbose) { P(CONTROL) }
                    bsls::Types::Int64 result = CONTROL;

                    if (veryVerbose) { cout << "Before: "; P(result) }
                    Util::convertToTimeT64(&result, INPUT);  // TEST
                    if (veryVerbose) { cout << "After: "; P(result); }

                    ASSERTV(LINE, vi, TIME, result, TIME == result);

                    result = Util::convertToTimeT64(INPUT);

                    if (veryVerbose) { cout << "After: "; P(result) }

                    ASSERTV(LINE, vi, TIME, result, TIME == result);

                        // *** REUSE THIS LOOP TO PARTIALLY TEST
                        //                               convertFromTimeT64 ***

                    if (vi) {  // Don't repeat more than once per row.
                        continue;
                    }

                    bdlt::Datetime result2   (2222, 2, 22, 22, 22, 22, 222);
                    bdlt::Datetime result2Dup(2222, 2, 22, 22, 22, 22, 222);

                    if (veryVerbose) { cout << "Before: "; P(result2) }
                    Util::convertFromTimeT64(&result2, result);
                    if (veryVerbose) { cout << "After: "; P(result2) }

                    if (veryVerbose) { cout << "Before: "; P(result2Dup) }
                    result2Dup = Util::convertFromTimeT64(result);
                    if (veryVerbose) { cout << "After: "; P(result2Dup) }

                        // *** The millisecond field is always set to 0.  ***
                    if (veryVeryVerbose) { P(INPUT) }
                    bdlt::Datetime tmp2(INPUT);
                    tmp2.setMillisecond(0);

                        // *** There is no equivalent representation for
                        // 24:00:00 as a time_t
                    tmp2.setHour(tmp2.hour() % 24);

                    const bdlt::Datetime INPUT2(tmp2);
                    if (veryVerbose) { P(INPUT2) }
                    ASSERTV(LINE, INPUT2, 0 == INPUT2.millisecond());
                                                                // double check

                        // *** All relevant fields are recovered properly.  ***
                    ASSERTV(LINE, INPUT2, result2, INPUT2 == result2);
                    ASSERTV(LINE, INPUT2, result2Dup, INPUT2 == result2Dup);
                } // end for vi
            } // end for ti
        }

        if (verbose) {
            cout << "\ntime_t => bdlt::Datetime." << endl;
        }

        {
            bsl::numeric_limits<long long> LLONG_LIMITS;

            static const struct {
                int d_lineNum;               // source line number

                bsls::Types::Int64 d_input;   // initial time_t value

                int d_status;                // expected success
                int d_year;                  // expected field of result
                int d_month;                 // expected field of result
                int d_day;                   // expected field of result
                int d_hour;                  // expected field of result
                int d_minute;                // expected field of result
                int d_second;                // expected field of result

            } DATA[] = {           // <------- result ------->
                //lin   input value   year mon day hou min sec
                //---   -----------   ---- --- --- --- --- ---

                    // *** All relevant fields are recovered properly.  ***
                //lin   input value   s    year mon day hou min sec
                //--- -------------   -    ---- --- --- --- --- ---
                { L_,  LLONG_LIMITS.min(),
                                      FAILURE,0,  0,  0,  0,  0,  0 },
                { L_,  LLONG_MIN + 1, FAILURE,0,  0,  0,  0,  0,  0 },
                { L_, -62135596802LL, FAILURE,0,  0,  0,  0,  0,  0 },
                { L_, -62135596801LL, FAILURE,0,  0,  0,  0,  0,  0 },
                { L_, -62135596800LL, 0,      1,  1,  1,  0,  0,  0 },
                { L_, -62135596799LL, 0,      1,  1,  1,  0,  0,  1 },
                { L_, -62135596798LL, 0,      1,  1,  1,  0,  0,  2 },
                { L_,  -3155673601LL, 0,   1869, 12, 31, 23, 59, 59 },
                { L_,  -2840140801LL, 0,   1879, 12, 31, 23, 59, 59 },
                { L_,  -2720171440LL, 0,   1883, 10, 20, 12, 49, 20 },
                { L_,  -2524521601LL, 0,   1889, 12, 31, 23, 59, 59 },
                { L_,  -2208988801LL, 0,   1899, 12, 31, 23, 59, 59 },
                { L_,    -1893456001, 0,   1909, 12, 31, 23, 59, 59 },
                { L_,    -1577923201, 0,   1919, 12, 31, 23, 59, 59 },
                { L_,    -1407351020, 0,   1925,  5, 28,  5,  9, 40 },
                { L_,    -1262304001, 0,   1929, 12, 31, 23, 59, 59 },
                { L_,     -946771201, 0,   1939, 12, 31, 23, 59, 59 },
                { L_,     -804163287, 0,   1944,  7,  8, 13, 18, 33 },
                { L_,     -631152001, 0,   1949, 12, 31, 23, 59, 59 },
                { L_,     -315619201, 0,   1959, 12, 31, 23, 59, 59 },
                { L_,             -1, 0,   1969, 12, 31, 23, 59, 59 },
                { L_,              0, 0,   1970,  1,  1,  0,  0,  0 }, // sec
                { L_,              1, 0,   1970,  1,  1,  0,  0,  1 }, // sec
                { L_,             59, 0,   1970,  1,  1,  0,  0, 59 },

                { L_,             60, 0,   1970,  1,  1,  0,  1,  0 }, // min
                { L_,           3599, 0,   1970,  1,  1,  0, 59, 59 }, //

                { L_,           3600, 0,   1970,  1,  1,  1,  0,  0 }, // hour
                { L_,          86399, 0,   1970,  1,  1, 23, 59, 59 },

                { L_,          86400, 0,   1970,  1,  2,  0,  0,  0 }, // day
                { L_,        2678399, 0,   1970,  1, 31, 23, 59, 59 },

                { L_,        2678400, 0,   1970,  2,  1,  0,  0,  0 }, // mon
                { L_,       31535999, 0,   1970, 12, 31, 23, 59, 59 },

                { L_,       31536000, 0,   1971,  1,  1,  0,  0,  0 }, // year
                { L_,       63072000, 0,   1972,  1,  1,  0,  0,  0 }, // lp.yr
                { L_,       94694400, 0,   1973,  1,  1,  0,  0,  0 },
                { L_,      126230400, 0,   1974,  1,  1,  0,  0,  0 },

                { L_,      441763200, 0,   1984,  1,  1,  0,  0,  0 }, // 10yrs
                { L_,      757382400, 0,   1994,  1,  1,  0,  0,  0 },
                { L_,     1072915200, 0,   2004,  1,  1,  0,  0,  0 },
                { L_,     1388534400, 0,   2014,  1,  1,  0,  0,  0 },
                { L_,     1704067200, 0,   2024,  1,  1,  0,  0,  0 },
                { L_,     2019686400, 0,   2034,  1,  1,  0,  0,  0 },

                { L_,     2021253247, 0,   2034,  1, 19,  3, 14,  7 }, // year
                { L_,     2052789247, 0,   2035,  1, 19,  3, 14,  7 },
                { L_,     2084325247, 0,   2036,  1, 19,  3, 14,  7 }, // lp.yr
                { L_,     2115947647, 0,   2037,  1, 19,  3, 14,  7 },

                { L_,     2144805247, 0,   2037, 12, 19,  3, 14,  7 }, // mon
                { L_,     2147480047, 0,   2038,  1, 19,  2, 14,  7 }, // hour
                { L_,     2147483587, 0,   2038,  1, 19,  3, 13,  7 }, // min
                { L_,     2147483646, 0,   2038,  1, 19,  3, 14,  6 }, // sec

                { L_,     2147483647, 0,   2038,  1, 19,  3, 14,  7 }, // top
                { L_,        INT_MAX, 0,   2038,  1, 19,  3, 14,  7 },
                { L_,   2147483648LL, 0,   2038,  1, 19,  3, 14,  8 },
                { L_,   2463016448LL, 0,   2048,  1, 19,  3, 14,  8 },
                { L_,   2778635648LL, 0,   2058,  1, 19,  3, 14,  8 },
                { L_,   3094168448LL, 0,   2068,  1, 19,  3, 14,  8 },
                { L_,   3409787648LL, 0,   2078,  1, 19,  3, 14,  8 },
                { L_,   3725320448LL, 0,   2088,  1, 19,  3, 14,  8 },
                { L_,   4040939648LL, 0,   2098,  1, 19,  3, 14,  8 },
                { L_,   4356386048LL, 0,   2108,  1, 19,  3, 14,  8 },
                { L_, 253402300797LL, 0,   9999, 12, 31, 23, 59, 57 },
                { L_, 253402300798LL, 0,   9999, 12, 31, 23, 59, 58 },
                { L_, 253402300799LL, 0,   9999, 12, 31, 23, 59, 59 },
                { L_, 253402300800LL, FAILURE,0,  0,  0,  0,  0,  0 },
                { L_, 253402300801LL, FAILURE,0,  0,  0,  0,  0,  0 },
                { L_,  LLONG_MAX - 1, FAILURE,0,  0,  0,  0,  0,  0 },
                { L_,      LLONG_MAX, FAILURE,0,  0,  0,  0,  0,  0 },
             };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            // MAIN TEST-TABLE LOOP

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int                LINE   = DATA[ti].d_lineNum;

                const bsls::Types::Int64 INPUT  = DATA[ti].d_input;

                const int                STATUS = DATA[ti].d_status;

                const int                YEAR   = DATA[ti].d_year;
                const int                MONTH  = DATA[ti].d_month;
                const int                DAY    = DATA[ti].d_day;
                const int                HOUR   = DATA[ti].d_hour;
                const int                MINUTE = DATA[ti].d_minute;
                const int                SECOND = DATA[ti].d_second;

                if (veryVerbose) {
                    cout << "\n--------------------------------------" << endl;
                    P_(LINE) P_(INPUT) P(STATUS)
                    T_ P_(YEAR) P_(MONTH) P(DAY)
                    T_ P_(HOUR) P_(MINUTE) P(SECOND)
                }

                {
                    // Double check that the table itself is right.
                    ASSERTV(LINE, !!STATUS == STATUS);

                    // If the conversion is meaningful double check
                    if (!STATUS) {
                        bdlt::Datetime     dt(YEAR, MONTH, DAY,
                                              HOUR, MINUTE, SECOND);
                        bsls::Types::Int64 tt;
                        tt = Util::convertToTimeT64(dt);
                        ASSERTV(LINE, tt, INPUT, tt == INPUT);
                    }
                }

                bdlt::Datetime CONTROL(1, 2, 3, 4, 5, 6, 7);
                bdlt::Datetime result(CONTROL);
                bdlt::Datetime resultDup(CONTROL);

                if (veryVerbose) { cout << "Before: "; P(result) }
                int status = Util::convertFromTimeT64(&result, INPUT);
                if (veryVerbose) { cout << " After: "; P_(result) }
                ASSERTV(LINE, ti, STATUS, status, !STATUS == !status);
                                                                   // black-box

                if (0 == STATUS) {
                    resultDup = Util::convertFromTimeT64(INPUT);
                }

                if (veryVerbose) { cout << "After: "; P(resultDup) }

                if (STATUS != 0) {
                        // *** Bad status implies no change to result.  ***
                    ASSERTV(LINE, ti, CONTROL, result, CONTROL == result);
                    ASSERTV(LINE, ti, CONTROL, result, CONTROL == resultDup);
                }
                else {
                        // *** All relevant fields are recovered properly.  ***
                    ASSERTV(LINE, YEAR,     result.year(),
                                  YEAR   == result.year());
                    ASSERTV(LINE, MONTH,    result.month(),
                                  MONTH  == result.month());
                    ASSERTV(LINE, DAY,      result.day(),
                                  DAY    == result.day());
                    ASSERTV(LINE, HOUR,     result.hour(),
                                  HOUR   == result.hour());
                    ASSERTV(LINE, MINUTE,   result.minute(),
                                  MINUTE == result.minute());
                    ASSERTV(LINE, SECOND,   result.second(),
                                  SECOND == result.second());

                    ASSERTV(LINE, YEAR,     resultDup.year(),
                                  YEAR   == resultDup.year());
                    ASSERTV(LINE, MONTH,    resultDup.month(),
                                  MONTH  == resultDup.month());
                    ASSERTV(LINE, DAY,      resultDup.day(),
                                  DAY    == resultDup.day());
                    ASSERTV(LINE, HOUR,     resultDup.hour(),
                                  HOUR   == resultDup.hour());
                    ASSERTV(LINE, MINUTE,   resultDup.minute(),
                                  MINUTE == resultDup.minute());
                    ASSERTV(LINE, SECOND,   resultDup.second(),
                                  SECOND == resultDup.second());

                        // *** The millisecond field is always set to 0.  ***
                    ASSERTV(LINE,           result.millisecond(),
                                  0      == result.millisecond());
                    ASSERTV(LINE,           resultDup.millisecond(),
                                  0      == resultDup.millisecond());
                } // end for else

                // NEGATIVE TEST
                {
                    bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

                    if (veryVerbose) {
                        cout << "Negative test - expect "
                             << (0 == STATUS ? "success " : "failure ");
                        P(INPUT)
                    }

                    if (0 == STATUS) {
                        ASSERT_SAFE_PASS(Util::convertFromTimeT64(INPUT));
                    }
                    else {
                        ASSERT_SAFE_FAIL(Util::convertFromTimeT64(INPUT));
                    }
                }
            } // end for ti
        }

        if (verbose) {
            cout << "\ntime_t => bdlt::Datetime => time_t." << endl;
        }

        {
            unsigned int SIZE = unsigned(INT_MAX) + 1;

            if (veryVerbose) { P(SIZE)}
                                            // to allow abs value to count!
            unsigned int numTrials = 1000;  // default with no v-v-verbose

            const int PRIME = 123456789;  // at least relatively prime to size

            ASSERT(SIZE % PRIME);        // ensure SIZE not divisible by PRIME

            double percentCovered = 100 * double(numTrials) / SIZE;

            if (verbose) { T_ P_(numTrials) P(percentCovered) }

            const unsigned int STARTING_VALUE = 0;

            unsigned int pseudoRandomValue = STARTING_VALUE;

            for (unsigned int i = 0; i < numTrials; ++i) {
                // Ensure that there is no premature repetition; ok first time.

                ASSERTV(i, (STARTING_VALUE != pseudoRandomValue) != !i);

                bsl::time_t currentValue = pseudoRandomValue;
                pseudoRandomValue += PRIME;
                pseudoRandomValue %= SIZE;

                const bdlt::Datetime INITIAL_VALUE(
                                1 +  3 * i % 9999,
                                1 +  5 * i % 12,
                                1 +  7 * i % 28,
                                0 + 11 * i % 24,
                                0 + 13 * i % 60,
                                0 + 17 * i % 60,
                                0 + 19 * i % 1000);  // pseudo random values
                if (veryVeryVerbose) { P(INITIAL_VALUE) }
                bdlt::Datetime tmp(INITIAL_VALUE);
                bdlt::Datetime tmpDup(INITIAL_VALUE);
                if (veryVeryVerbose) { cout << "Before: "; P(tmp) }
                Util::convertFromTimeT64(&tmp, currentValue);
                if (veryVeryVerbose) { cout << " After: "; P(tmp) }

                if (veryVeryVerbose) { cout << "Before: "; P(tmpDup) }
                tmpDup = Util::convertFromTimeT64(currentValue);
                if (veryVeryVerbose) { cout << " After: "; P(tmpDup) }

                bsls::Types::Int64 result = LLONG_MIN;  // out of bounds

                if (veryVeryVerbose) { cout<<"Before: "; P(result) }
                result = Util::convertToTimeT64(tmpDup);
                if (veryVeryVerbose) { cout<<" After: "; P(result) }

                ASSERTV(i, currentValue == result);
            }
        }

        // Negative test for null pointers.
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);
            if (verbose) {
                cout << "Negative test: convertToTimeT64(null, x)" << endl;
            }
            ASSERT_SAFE_FAIL(Util::convertToTimeT64(0, bdlt::Datetime()));

            if (verbose) {
                cout << "Negative test: convertFromTimeT64(null, x)" << endl;
            }
            ASSERT_SAFE_FAIL(
                            Util::convertFromTimeT64(0, bsls::Types::Int64()));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // UNIX EPOCH DATETIME
        //   Test the function that returns the Unix epoch 'bdlt::Datetime'.
        //
        // Concerns:
        //: 1 That this function returns the epoch 'bdlt::Datetime' in a
        //:   thread-safe fashion.
        //
        // Plan:
        //: 1 We know that the implementation is thread-safe by design.
        //:   We want to verify only that the correct value is returned. (C-1)
        //
        // Testing:
        //   Dt& epoch();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "UNIX EPOCH DATETIME" << endl
                          << "===================" << endl;

        const bdlt::Datetime EPOCH(1970, 1, 1, 0);

        ASSERT(EPOCH == Util::epoch());
        ASSERT(epochAddressIsNotZero);
        ASSERT(EPOCH == EarlyEpochCopier::copiedValue());
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
