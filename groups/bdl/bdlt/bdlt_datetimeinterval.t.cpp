// bdlt_datetimeinterval.t.cpp                                        -*-C++-*-
#include <bdlt_datetimeinterval.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bslx_byteinstream.h>
#include <bslx_byteoutstream.h>
#include <bslx_instreamfunctions.h>
#include <bslx_outstreamfunctions.h>
#include <bslx_testinstream.h>
#include <bslx_testinstreamexception.h>
#include <bslx_testoutstream.h>
#include <bslx_versionfunctions.h>

#include <bsl_climits.h>     // 'INT_MAX', 'INT_MIN'
#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>     // 'memcmp'
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <cmath>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The component under test defines a single value-semantic class that
// implements a time interval having millisecond resolution.  The time interval
// is maintained internally as a (signed) 64-bit integer representing the total
// number of milliseconds comprising the interval.  The value of the time
// interval is accessed either in terms of its total-millisecond representation
// (via 'totalMilliseconds'), or in terms of its (derived) canonical five-field
// representation (via 'days', 'hours', 'minutes', 'seconds', and
// 'milliseconds').
//
// We will therefore follow our standard ten-case approach to testing
// value-semantic types, except that we will verify test apparatus in case 3
// (in lieu of the generator function, 'gg'), with the default constructor,
// (trivial) destructor, and primary manipulator ('setTotalMilliseconds')
// tested fully in case 2.
//
// Primary Manipulators:
//: o 'setTotalMilliseconds'
//
// Basic Accessors:
//: o 'days'
//: o 'hours'
//: o 'minutes'
//: o 'seconds'
//: o 'milliseconds'
//: o 'totalMilliseconds'
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR/MANIPULATOR/OPERATOR ptr./ref. parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o No memory is ever allocated from the default allocator.
//: o Precondition violations are detected in appropriate build modes.
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [  ] static const bsls::Types::Int64 k_MILLISECONDS_MAX = ...;
// [  ] static const bsls::Types::Int64 k_MILLISECONDS_MIN = ...;
//
// CLASS METHODS
// [10] static int maxSupportedBdexVersion(int versionSelector);
//
// CREATORS
// [ 2] DatetimeInterval();
// [11] DatetimeInterval(int d, Int64 h = 0, m = 0, s = 0, ms = 0);
// [ 7] DatetimeInterval(const DatetimeInterval& original);
// [ 2] ~DatetimeInterval();
//
// MANIPULATORS
// [ 9] DatetimeInterval& operator=(const DatetimeInterval& rhs);
// [18] DatetimeInterval& operator+=(const DatetimeInterval& rhs);
// [18] DatetimeInterval& operator-=(const DatetimeInterval& rhs);
// [12] void setInterval(int d, Int64 h = 0, m = 0, s = 0, ms = 0);
// [13] void setTotalDays(int days);
// [13] void setTotalHours(Int64 hours);
// [13] void setTotalMinutes(Int64 minutes);
// [13] void setTotalSeconds(Int64 seconds);
// [ 2] void setTotalMilliseconds(Int64 milliseconds);
// [16] void addInterval(int d, Int64 h = 0, m = 0, s = 0, ms = 0);
// [15] void addDays(int days);
// [15] void addHours(Int64 hours);
// [15] void addMinutes(Int64 minutes);
// [15] void addSeconds(Int64 seconds);
// [15] void addMilliseconds(Int64 milliseconds);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
//
// ACCESSORS
// [ 4] int days() const;
// [ 4] int hours() const;
// [ 4] int minutes() const;
// [ 4] int seconds() const;
// [ 4] int milliseconds() const;
// [14] int totalDays() const;
// [14] Int64 totalHours() const;
// [14] Int64 totalMinutes() const;
// [14] Int64 totalSeconds() const;
// [14] double totalSecondsAsDouble() const;
// [ 4] Int64 totalMilliseconds() const;
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
//
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [19] DatetimeInterval operator+(const DatetimeInterval& lhs, rhs);
// [19] DatetimeInterval operator-(const DatetimeInterval& lhs, rhs);
// [20] DatetimeInterval operator-(const DatetimeInterval& value);
// [ 6] bool operator==(const DatetimeInterval& lhs, rhs);
// [ 6] bool operator!=(const DatetimeInterval& lhs, rhs);
// [17] bool operator< (const DatetimeInterval& lhs, rhs);
// [17] bool operator<=(const DatetimeInterval& lhs, rhs);
// [17] bool operator> (const DatetimeInterval& lhs, rhs);
// [17] bool operator>=(const DatetimeInterval& lhs, rhs);
// [ 5] ostream& operator<<(ostream &os, const DatetimeInterval& object);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [21] USAGE EXAMPLE
// [ 3] TEST APPARATUS
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ *] CONCERN: In no case does memory come from the default allocator.
// [20] CONCERN: All ctor/manip./free op. ptr./ref. params. are 'const'.
// [14] CONCERN: All accessor methods are declared 'const'.
// [20] CONCERN: Precondition violations are detected when enabled.
// [ 8] Reserved for 'swap' testing.

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

typedef bdlt::DatetimeInterval Obj;
typedef bslx::TestInStream     In;
typedef bslx::TestOutStream    Out;

#define VERSION_SELECTOR 20140601

typedef bsls::Types::Int64     Int64;

const Int64 k_MSECS_PER_SEC  = bdlt::TimeUnitRatio::k_MILLISECONDS_PER_SECOND;
const Int64 k_MSECS_PER_MIN  = bdlt::TimeUnitRatio::k_MILLISECONDS_PER_MINUTE;
const Int64 k_MSECS_PER_HOUR = bdlt::TimeUnitRatio::k_MILLISECONDS_PER_HOUR;
const Int64 k_MSECS_PER_DAY  = bdlt::TimeUnitRatio::k_MILLISECONDS_PER_DAY;

const int   k_DAYS_MAX  = INT_MAX;
const int   k_DAYS_MIN  = INT_MIN;

const Int64 k_HOURS_MAX =   24 * static_cast<Int64>(k_DAYS_MAX) +  23;
const Int64 k_HOURS_MIN =   24 * static_cast<Int64>(k_DAYS_MIN) -  23;

const Int64 k_MINS_MAX  =   60 *                    k_HOURS_MAX +  59;
const Int64 k_MINS_MIN  =   60 *                    k_HOURS_MIN -  59;

const Int64 k_SECS_MAX  =   60 *                    k_MINS_MAX  +  59;
const Int64 k_SECS_MIN  =   60 *                    k_MINS_MIN  -  59;

const Int64 k_MSECS_MAX = 1000 *                    k_SECS_MAX  + 999;
const Int64 k_MSECS_MIN = 1000 *                    k_SECS_MIN  - 999;

// Verify PUBLIC CLASS DATA

BSLMF_ASSERT(k_MSECS_MAX == Obj::k_MILLISECONDS_MAX);
BSLMF_ASSERT(k_MSECS_MIN == Obj::k_MILLISECONDS_MIN);

// ============================================================================
//                                 TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(true == bsl::is_trivially_copyable<Obj>::value);

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static
Int64 flds2Msecs(int d, Int64 h = 0, Int64 m = 0, Int64 s = 0, Int64 ms = 0)
    // Return the sum of the specified 'd' days and the optionally specified
    // 'h' hours, 'm' minutes, 's' seconds, and 'ms' milliseconds, expressed as
    // total milliseconds.  The behavior is undefined unless the resulting sum
    // is within the range '[k_MSECS_MIN .. k_MSECS_MAX]'.
{
    Int64 totalMsecs = ms;  // accumulate milliseconds

    totalMsecs += s * k_MSECS_PER_SEC;   // convert & accumulate seconds
    totalMsecs += m * k_MSECS_PER_MIN;   // convert & accumulate minutes
    totalMsecs += h * k_MSECS_PER_HOUR;  // convert & accumulate hours
    totalMsecs += d * k_MSECS_PER_DAY;   // convert & accumulate days

    BSLS_ASSERT(k_MSECS_MIN <= totalMsecs);
    BSLS_ASSERT(               totalMsecs <= k_MSECS_MAX);

    return totalMsecs;
}

static
Int64 abs64(Int64 value)
    // Return the absolute value of the specified 64-bit 'value'.  The behavior
    // is undefined unless 'value' is greater than the minimum 64-bit integer
    // value.
{
    ASSERT(value > -0x7fffffffffffffffLL - 1);

    return value < 0 ? -value : value;
}

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define DEFAULT DATA used by test cases 4 and 14.

struct DefaultDataRow {
    int   d_line;   // source line number
    int   d_days;
    Int64 d_hours;
    Int64 d_mins;
    Int64 d_secs;
    Int64 d_msecs;
};

static
const DefaultDataRow DEFAULT_DATA[] =
{
    //LINE      DAYS      HOURS     MINUTES     SECONDS     MILLISECONDS
    //----      ----      -----     -------     -------     ------------

    // default (must be first)
    { L_,          0,         0,          0,          0,               0 },

    // positive time intervals
    { L_,          0,         0,          0,          0,             999 },
    { L_,          0,         0,          0,         59,               0 },
    { L_,          0,         0,         59,          0,               0 },
    { L_,          0,        23,          0,          0,               0 },
    { L_, k_DAYS_MAX,         0,          0,          0,               0 },
    { L_, k_DAYS_MAX,        23,         59,         59,             999 },
    { L_,          1,         2,          3,          4,               5 },
    { L_,      23469,        13,         56,         19,             200 },

    // negative time intervals
    { L_,          0,         0,          0,          0,            -999 },
    { L_,          0,         0,          0,        -59,               0 },
    { L_,          0,         0,        -59,          0,               0 },
    { L_,          0,       -23,          0,          0,               0 },
    { L_, k_DAYS_MIN,         0,          0,          0,               0 },
    { L_, k_DAYS_MIN,       -23,        -59,        -59,            -999 },
    { L_,         -5,        -4,         -3,         -2,              -1 },
    { L_,     -23469,       -13,        -56,        -19,            -200 },
};
const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

// Define ALTernate DATA used by test cases 11, 12, and 16.

struct AltDataRow {
    int   d_line;   // source line number
    int   d_nargs;
    int   d_days;
    Int64 d_hours;
    Int64 d_mins;
    Int64 d_secs;
    Int64 d_msecs;
};

static
const AltDataRow ALT_DATA[] =
{
    //LINE N        DAYS        HOURS    MINUTES    SECONDS     MILLISECONDS
    //---- -        ----        -----    -------    -------     ------------
        // N = 1
    { L_,  1,          0,           0,         0,         0,               0 },

    { L_,  1,          1,           0,         0,         0,               0 },
    { L_,  1,      13027,           0,         0,         0,               0 },
    { L_,  1, k_DAYS_MAX,           0,         0,         0,               0 },

    { L_,  1,         -1,           0,         0,         0,               0 },
    { L_,  1,     -42058,           0,         0,         0,               0 },
    { L_,  1, k_DAYS_MIN,           0,         0,         0,               0 },

        // N = 2
    { L_,  2,          0,           0,         0,         0,               0 },

    { L_,  2,          0,           1,         0,         0,               0 },
    { L_,  2,          0,          24,         0,         0,               0 },
    { L_,  2,          0, k_HOURS_MAX,         0,         0,               0 },

    { L_,  2,          1,           0,         0,         0,               0 },
    { L_,  2,        366,           0,         0,         0,               0 },
    { L_,  2, k_DAYS_MAX,           0,         0,         0,               0 },

    { L_,  2,        333,         457,         0,         0,               0 },

    { L_,  2,
          k_DAYS_MAX - 1,          47,         0,         0,               0 },

    { L_,  2, k_DAYS_MAX,          23,         0,         0,               0 },


    { L_,  2,          0,          -1,         0,         0,               0 },
    { L_,  2,          0,         -24,         0,         0,               0 },
    { L_,  2,          0, k_HOURS_MIN,         0,         0,               0 },

    { L_,  2,         -1,           0,         0,         0,               0 },
    { L_,  2,       -366,           0,         0,         0,               0 },
    { L_,  2, k_DAYS_MIN,           0,         0,         0,               0 },

    { L_,  2,       -333,        -457,         0,         0,               0 },

    { L_,  2,
          k_DAYS_MIN + 1,         -47,         0,         0,               0 },

    { L_,  2, k_DAYS_MIN,         -23,         0,         0,               0 },

    { L_,  2,     -23469,         256,         0,         0,               0 },
    { L_,  2,          5,          -4,         0,         0,               0 },

        // N = 3
    { L_,  3,          0,           0,          0,        0,               0 },

    { L_,  3,          0,           0,          1,        0,               0 },
    { L_,  3,          0,           0,         59,        0,               0 },
    { L_,  3,          0,          24,         60,        0,               0 },
    { L_,  3,        100,         200,        300,        0,               0 },

    { L_,  3,          0,           1,
                                  k_MINS_MAX - 60,        0,               0 },

    { L_,  3,          0,           0, k_MINS_MAX,        0,               0 },

    { L_,  3,          0,
                      k_HOURS_MAX - 1,        119,        0,               0 },

    { L_,  3,          0, k_HOURS_MAX,         59,        0,               0 },
    { L_,  3, k_DAYS_MAX,          23,         59,        0,               0 },

    { L_,  3,          0,           0,         -1,        0,               0 },
    { L_,  3,          0,           0,        -59,        0,               0 },
    { L_,  3,          0,         -24,        -60,        0,               0 },
    { L_,  3,       -100,        -200,       -300,        0,               0 },

    { L_,  3,          0,          -1,
                                  k_MINS_MIN + 60,        0,               0 },

    { L_,  3,          0,           0, k_MINS_MIN,        0,               0 },

    { L_,  3,          0,
                      k_HOURS_MIN + 1,       -119,        0,               0 },

    { L_,  3,          0, k_HOURS_MIN,        -59,        0,               0 },
    { L_,  3, k_DAYS_MIN,         -23,        -59,        0,               0 },

    { L_,  3,     -23469,         256,       -212,        0,               0 },
    { L_,  3,          5,          -4,          3,        0,               0 },

        // N = 4
    { L_,  4,          0,           0,          0,        0,               0 },

    { L_,  4,          0,           0,          0,        1,               0 },
    { L_,  4,          0,           0,         59,       59,               0 },
    { L_,  4,          0,          24,         60,       60,               0 },
    { L_,  4,        100,         200,        300,      400,               0 },

    { L_,  4,          0,           0,          1,
                                            k_SECS_MAX - 60,               0 },

    { L_,  4,          0,           0,          0,
                                                 k_SECS_MAX,               0 },

    { L_,  4,          0,           0,
                                   k_MINS_MAX - 1,      119,               0 },

    { L_,  4,          0,           0, k_MINS_MAX,       59,               0 },

    { L_,  4, k_DAYS_MAX,          23,         59,       59,               0 },

    { L_,  4,          0,           0,          0,       -1,               0 },
    { L_,  4,          0,           0,        -59,      -59,               0 },
    { L_,  4,          0,         -24,        -60,      -60,               0 },
    { L_,  4,       -100,        -200,       -300,     -400,               0 },

    { L_,  4,          0,           0,         -1,
                                            k_SECS_MIN + 60,               0 },

    { L_,  4,          0,           0,          0,
                                                 k_SECS_MIN,               0 },

    { L_,  4,          0,           0,
                                   k_MINS_MIN + 1,     -119,               0 },

    { L_,  4,          0,           0, k_MINS_MIN,      -59,               0 },

    { L_,  4, k_DAYS_MIN,         -23,        -59,      -59,               0 },

    { L_,  4,     -23469,         256,       -212,       77,               0 },
    { L_,  4,          5,          -4,          3,       -2,               0 },

        // N = 5
    { L_,  5,          0,           0,          0,        0,               0 },

    { L_,  5,          0,           0,          0,        0,               1 },
    { L_,  5,          0,           0,          0,       59,             999 },
    { L_,  5,          0,           0,         59,       59,             999 },
    { L_,  5,          0,          24,         60,       60,            1000 },
    { L_,  5,        100,         200,        300,      400,             500 },

    { L_,  5,          0,           0,          0,        1,
                                                          k_MSECS_MAX - 1000 },

    { L_,  5,          0,           0,          0,        0,
                                                                 k_MSECS_MAX },

    { L_,  5,          0,           0,          0,
                                             k_SECS_MAX - 1,            1999 },

    { L_,  5,          0,           0,          0,
                                                 k_SECS_MAX,             999 },

    { L_,  5, k_DAYS_MAX,          23,         59,       59,             999 },

    { L_,  5,          0,           0,          0,        0,              -1 },
    { L_,  5,          0,           0,          0,      -59,            -999 },
    { L_,  5,          0,           0,        -59,      -59,            -999 },
    { L_,  5,          0,         -24,        -60,      -60,           -1000 },
    { L_,  5,       -100,        -200,       -300,     -400,            -500 },

    { L_,  5,          0,           0,          0,       -1,
                                                          k_MSECS_MIN + 1000 },

    { L_,  5,          0,           0,          0,        0,
                                                                 k_MSECS_MIN },
    { L_,  5,          0,           0,          0,
                                          k_SECS_MIN + 1000,           -1999 },

    { L_,  5,          0,           0,          0,
                                                 k_SECS_MIN,            -999 },

    { L_,  5, k_DAYS_MIN,         -23,        -59,       -59,           -999 },

    { L_,  5,     -23469,         256,       -212,        77,            -13 },
    { L_,  5,          5,          -4,          3,        -2,              1 },
};
const int ALT_NUM_DATA = sizeof ALT_DATA / sizeof *ALT_DATA;

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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: This test driver is reusable w/other, similar components.

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator         defaultAllocator("default",
                                                  veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);

    switch (test) { case 0:
      case 21: {
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
///Example 1: Basic 'bdlt::DatetimeInterval' Usage
///- - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create and use a 'bdlt::DatetimeInterval'
// object.
//
// First, create an object 'i1' having the default value:
//..
    bdlt::DatetimeInterval i1;         ASSERT(  0 == i1.days());
                                       ASSERT(  0 == i1.hours());
                                       ASSERT(  0 == i1.minutes());
                                       ASSERT(  0 == i1.seconds());
                                       ASSERT(  0 == i1.milliseconds());
//..
// Then, set the value of 'i1' to -5 days, and then add 16 hours to that value:
//..
    i1.setTotalDays(-5);
    i1.addHours(16);                   ASSERT( -4 == i1.days());
                                       ASSERT( -8 == i1.hours());
                                       ASSERT(  0 == i1.minutes());
                                       ASSERT(  0 == i1.seconds());
                                       ASSERT(  0 == i1.milliseconds());
//..
// Next, create 'i2' as a copy of 'i1':
//..
    bdlt::DatetimeInterval i2(i1);     ASSERT( -4 == i2.days());
                                       ASSERT( -8 == i2.hours());
                                       ASSERT(  0 == i2.minutes());
                                       ASSERT(  0 == i2.seconds());
                                       ASSERT(  0 == i2.milliseconds());
//..
// Then, add 2 days and 4 seconds to the value of 'i2' (in two steps), and
// confirm that 'i2' has a value that is greater than that of 'i1':
//..
    i2.addDays(2);
    i2.addSeconds(4);                  ASSERT( -2 == i2.days());
                                       ASSERT( -7 == i2.hours());
                                       ASSERT(-59 == i2.minutes());
                                       ASSERT(-56 == i2.seconds());
                                       ASSERT(  0 == i2.milliseconds());
                                       ASSERT(i2 > i1);
//..
// Next, add 2 days and 4 seconds to the value of 'i1' in one step by using the
// 'addInterval' method, and confirm that 'i1' now has the same value as 'i2':
//..
    i1.addInterval(2, 0, 0, 4);        ASSERT(i2 == i1);
//..
// Finally, write the value of 'i2' to 'stdout':
//..
if (veryVerbose)
    bsl::cout << i2 << bsl::endl;
//..
// The output operator produces the following format on 'stdout':
//..
//  -2_07:59:56.000
//..

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // UNARY MINUS OPERATOR
        //   Ensure that the result object is the negation of that of the
        //   operand.
        //
        // Concerns:
        //: 1 The object that is returned has a time interval that represents
        //:   the negation of that of the operand.
        //:
        //: 2 The operator accepts the contractually specified range of
        //:   argument values.
        //:
        //: 3 The value of the source object is not modified.
        //:
        //: 4 Non-modifiable objects can be negated (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 5 The operator's signature and return type are standard.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the address of 'operator-' to initialize a function pointer
        //:   having the appropriate signature and return type for the free
        //:   unary minus operator defined in this component.  (C-4..5)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their five-field
        //:   representation.
        //:
        //: 3 For each row 'R' in the table of P-2:  (C-1..3)
        //:
        //:   1 Use the value constructor to create a 'const' 'Obj', 'X', with
        //:     the value from 'R'; also use the copy constructor to create a
        //:     'const' 'Obj', 'XX', from 'X'.
        //:
        //:   2 Use the copy constructor to create a 'const' 'Obj', 'Y', from
        //:     the unary negation of 'X'.
        //:
        //:   3 Use the field-based accessors to verify that the value of 'Y'
        //:     is as expected.  (C-1..2)
        //:
        //:   4 Use the equality-comparison operator to verify that 'X' still
        //:     has the same value as that of 'XX'.  (C-3)
        //:
        //:   5 For added assurance, verify that the total-milliseconds
        //:     representation of 'Y' is the negation of that of 'X'.
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid total-millisecond values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-6)
        //
        // Testing:
        //   DatetimeInterval operator-(const DatetimeInterval& value);
        //   CONCERN: All ctor/manip./free op. ptr./ref. params. are 'const'.
        //   CONCERN: Precondition violations are detected when enabled.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "UNARY MINUS OPERATOR" << endl
                          << "====================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj (*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorNeg = bdlt::operator-;

            (void)operatorNeg;  // quash potential compiler warnings
        }

        if (verbose) cout << "\nUse a table of distinct object values."
                          << endl;

        static const struct {
            int d_line;   // source line number
            int d_days;
            int d_hours;
            int d_mins;
            int d_secs;
            int d_msecs;
        } DATA[] = {
            //LINE       DAYS    HOURS     MINUTES    SECONDS    MILLISECONDS
            //----       ----    -----     -------    -------    ------------

            // default
            { L_,          0,       0,          0,         0,              0 },

            // positive time intervals
            { L_,          0,       0,          0,         0,            999 },
            { L_,          0,       0,          0,        59,              0 },
            { L_,          0,       0,         59,         0,              0 },
            { L_,          0,      23,          0,         0,              0 },
            { L_, k_DAYS_MAX,       0,          0,         0,              0 },
            { L_, k_DAYS_MAX,      23,         59,        59,            999 },
            { L_,          1,       2,          3,         4,              5 },
            { L_,      23469,      13,         56,        19,            200 },

            // negative time intervals
            { L_,          0,       0,          0,         0,           -999 },
            { L_,          0,       0,          0,       -59,              0 },
            { L_,          0,       0,        -59,         0,              0 },
            { L_,          0,     -23,          0,         0,              0 },
            { L_,
              k_DAYS_MIN + 1,       0,          0,         0,              0 },
            { L_,
              k_DAYS_MIN + 1,     -23,        -59,       -59,           -999 },
            { L_,         -5,      -4,         -3,        -2,             -1 },
            { L_,     -23469,     -13,        -56,       -19,           -200 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_line;
            const int   DAYS  = DATA[ti].d_days;
            const Int64 HOURS = DATA[ti].d_hours;
            const Int64 MINS  = DATA[ti].d_mins;
            const Int64 SECS  = DATA[ti].d_secs;
            const Int64 MSECS = DATA[ti].d_msecs;

            const Obj X(DAYS, HOURS, MINS, SECS, MSECS);

            const Obj XX(X);

            if (veryVerbose) { T_ P_(X) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE, Obj(), X, Obj() == X)
                firstFlag = false;
            }

            const Obj Y(-X);

            if (veryVerbose) { T_ T_ P(Y) }

            LOOP_ASSERT(LINE, -DAYS  == Y.days());
            LOOP_ASSERT(LINE, -HOURS == Y.hours());
            LOOP_ASSERT(LINE, -MINS  == Y.minutes());
            LOOP_ASSERT(LINE, -SECS  == Y.seconds());
            LOOP_ASSERT(LINE, -MSECS == Y.milliseconds());

            LOOP3_ASSERT(LINE, XX, X, XX == X);

            LOOP_ASSERT(LINE, -X.totalMilliseconds() == Y.totalMilliseconds());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            {
                Obj mX(k_DAYS_MIN + 1, -23, -59, -59, -999); const Obj& X = mX;

                ASSERT_SAFE_PASS(-X);

                mX.addMilliseconds(-1);
                ASSERT_SAFE_FAIL(-X);
            }
        }

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // ARITHMETIC FREE OPERATORS (+, -)
        //   Ensure that each operator correctly computes the underlying
        //   total-milliseconds representation of the result object.
        //
        // Concerns:
        //: 1 The object that is returned by the free 'operator+' ('operator-')
        //:   has a time interval that represents the sum (difference) of those
        //:   of the two operands.
        //:
        //: 2 Each operator accepts the contractually specified range of
        //:   argument values.
        //:
        //: 3 The values of the two source objects supplied to each operator
        //:   are not modified.
        //:
        //: 4 Non-modifiable objects can be added and subtracted (i.e., objects
        //:   or references providing only non-modifiable access).
        //:
        //: 5 The operators' signatures and return types are standard.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator+' and 'operator-' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free binary arithmetic
        //:   operators defined in this component.  (C-4..5)
        //:
        //: 2 Using the table-driven technique to test 'operator+', specify a
        //:   set of object value triplets (one per row) in terms of the
        //:   total-milliseconds representations of three objects:
        //:   '(LHS, RHS, SUM = LHS + RHS)'.
        //:
        //: 3 For each row 'R' in the table of P-2:  (C-1..3)
        //:
        //:   1 Use the default constructor and 'setTotalMilliseconds' to
        //:     create two objects, 'X' and 'Y', respectively having the
        //:     values 'LHS' and 'RHS' from 'R'.
        //:
        //:   2 Use the copy constructor to create a 'const' 'Obj', 'XX', from
        //:     'X', and a 'const' 'Obj', 'YY', from 'Y'.
        //:
        //:   3 Use the copy constructor to create a 'const' 'Obj', 'Z', from
        //:     'X + Y'.
        //:
        //:   4 Use the 'totalMilliseconds' accessor to verify that 'Z' has the
        //:     expected 'SUM' from 'R'.  (C-1..2)
        //:
        //:   5 Use the equality-comparison operator to verify that 'X' and 'Y'
        //:     still have the same values as that of 'XX' and 'YY',
        //:     respectively.  (C-3)
        //:
        //: 4 Repeat steps similar to those described in P-2..3 to test
        //:   'operator-' except that, this time, the rows in the table are
        //:   defined as '(LHS, RHS, DIFF = LHS - RHS)' (P-2) and 'Z' is
        //:   constructed from 'X - Y' (P-3.3).
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid total-millisecond values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-6)
        //
        // Testing:
        //   DatetimeInterval operator+(const DatetimeInterval& lhs, rhs);
        //   DatetimeInterval operator-(const DatetimeInterval& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ARITHMETIC FREE OPERATORS (+, -)" << endl
                          << "================================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            typedef Obj (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorAdd = bdlt::operator+;
            operatorPtr operatorSub = bdlt::operator-;

            (void)operatorAdd;  // quash potential compiler warnings
            (void)operatorSub;
        }

        if (verbose) cout << "\nTesting 'operator+'." << endl;
        {
            static const struct {
                int   d_line;      // source line number
                Int64 d_lhsMsecs;  // lhs total milliseconds
                Int64 d_rhsMsecs;  // rhs total milliseconds
                Int64 d_sumMsecs;  // lhs + rhs total milliseconds
            } DATA[] = {
                //LINE  LHS MILLISECONDS  RHS MILLISECONDS  SUM MILLISECONDS
                //----  ----------------  ----------------  ----------------
                { L_,                 0,                0,                 0 },
                { L_,                 1,                0,                 1 },
                { L_,                 0,             1200,              1200 },
                { L_,             13027,           -42058,    13027 + -42058 },
                { L_,            -32546,            32546,                 0 },
                { L_,           INT_MIN,          INT_MAX,                -1 },
                { L_,   k_MSECS_MAX / 2,  k_MSECS_MAX / 2,   k_MSECS_MAX - 1 },
                { L_,   k_MSECS_MIN / 2,  k_MSECS_MIN / 2,   k_MSECS_MIN + 1 },
                { L_,   k_MSECS_MAX - 1,                1,       k_MSECS_MAX },
                { L_,               -27, k_MSECS_MIN + 27,       k_MSECS_MIN },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE      = DATA[ti].d_line;
                const Int64 LHS_MSECS = DATA[ti].d_lhsMsecs;
                const Int64 RHS_MSECS = DATA[ti].d_rhsMsecs;
                const Int64 SUM_MSECS = DATA[ti].d_sumMsecs;

                Obj mX;  const Obj& X = mX;
                mX.setTotalMilliseconds(LHS_MSECS);

                Obj mY;  const Obj& Y = mY;
                mY.setTotalMilliseconds(RHS_MSECS);

                const Obj XX(X);
                const Obj YY(Y);

                if (veryVerbose) { T_ P_(X) P_(Y) }

                const Obj Z(X + Y);

                if (veryVerbose) { T_ T_ P(Z) }

                LOOP3_ASSERT(LINE, SUM_MSECS, Z.totalMilliseconds(),
                             SUM_MSECS == Z.totalMilliseconds());

                LOOP3_ASSERT(LINE, XX, X, XX == X);
                LOOP3_ASSERT(LINE, YY, Y, YY == Y);
            }
        }

        if (verbose) cout << "\nTesting 'operator-'." << endl;
        {
            static const struct {
                int   d_line;       // source line number
                Int64 d_lhsMsecs;   // lhs total milliseconds
                Int64 d_rhsMsecs;   // rhs total milliseconds
                Int64 d_diffMsecs;  // lhs - rhs total milliseconds
            } DATA[] = {
                //LINE  LHS MILLISECONDS  RHS MILLISECONDS  DIFF MILLISECONDS
                //----  ----------------  ----------------  -----------------
                { L_,                 0,                0,                 0 },
                { L_,                 1,                0,                 1 },
                { L_,                 0,             1200,             -1200 },
                { L_,             42058,            13027,     42058 - 13027 },
                { L_,           INT_MIN,          INT_MAX,
                                         2 * static_cast<Int64>(INT_MIN) + 1 },
                { L_,   k_MSECS_MAX / 2,  k_MSECS_MAX / 2,                 0 },
                { L_,   k_MSECS_MAX - 3,               -3,       k_MSECS_MAX },
                { L_, k_MSECS_MIN + 999,              999,       k_MSECS_MIN },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE       = DATA[ti].d_line;
                const Int64 LHS_MSECS  = DATA[ti].d_lhsMsecs;
                const Int64 RHS_MSECS  = DATA[ti].d_rhsMsecs;
                const Int64 DIFF_MSECS = DATA[ti].d_diffMsecs;

                Obj mX;  const Obj& X = mX;
                mX.setTotalMilliseconds(LHS_MSECS);

                Obj mY;  const Obj& Y = mY;
                mY.setTotalMilliseconds(RHS_MSECS);

                const Obj XX(X);
                const Obj YY(Y);

                if (veryVerbose) { T_ P_(X) P_(Y) }

                const Obj Z(X - Y);

                if (veryVerbose) { T_ T_ P(Z) }

                LOOP3_ASSERT(LINE, DIFF_MSECS, Z.totalMilliseconds(),
                             DIFF_MSECS == Z.totalMilliseconds());

                LOOP3_ASSERT(LINE, XX, X, XX == X);
                LOOP3_ASSERT(LINE, YY, Y, YY == Y);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'operator+'" << endl;
            {
                Obj mX;  const Obj& X = mX;
                Obj mY;  const Obj& Y = mY;

                mX.setTotalMilliseconds(k_MSECS_MAX - 1);
                mY.setTotalMilliseconds(1);

                                                  ASSERT_SAFE_PASS(X + Y);
                mY.addMilliseconds(1);            ASSERT_SAFE_FAIL(X + Y);

                mX.setTotalMilliseconds(k_MSECS_MIN + 1);
                mY.setTotalMilliseconds(-1);

                                                  ASSERT_SAFE_PASS(X + Y);
                mY.addMilliseconds(-1);           ASSERT_SAFE_FAIL(X + Y);
            }

            if (veryVerbose) cout << "\t'operator-'" << endl;
            {
                Obj mX;  const Obj& X = mX;
                Obj mY;  const Obj& Y = mY;

                mX.setTotalMilliseconds(k_MSECS_MAX - 1);
                mY.setTotalMilliseconds(-1);

                                                  ASSERT_SAFE_PASS(X - Y);
                mY.addMilliseconds(-1);           ASSERT_SAFE_FAIL(X - Y);

                mX.setTotalMilliseconds(k_MSECS_MIN + 1);
                mY.setTotalMilliseconds(1);

                                                  ASSERT_SAFE_PASS(X - Y);
                mY.addMilliseconds(1);            ASSERT_SAFE_FAIL(X - Y);
            }
        }

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // ARITHMETIC ASSIGNMENT OPERATORS (+=, -=)
        //   Ensure that each operator correctly adjusts the underlying
        //   total-milliseconds representation of the object.
        //
        // Concerns:
        //: 1 Each compound assignment operator can change the value of any
        //:   modifiable target object based on any source object that does not
        //:   violate the method's documented preconditions.
        //:
        //: 2 The signatures and return types are standard.
        //:
        //: 3 The reference returned from each operator is to the target object
        //:   (i.e., '*this').
        //:
        //: 4 The value of the source object is not modified, unless it is an
        //:   alias for the target object.
        //:
        //: 5 A compound assignment of an object to itself behaves as expected
        //:   (alias-safety).
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator+=' and 'operator-=' to
        //:   initialize member-function pointers having the appropriate
        //:   signatures and return types for the two compound assignment
        //:   operators defined in this component.  (C-2)
        //:
        //: 2 Using the table-driven technique to test 'operator+=', specify a
        //:   set of object value triplets (one per row) in terms of the
        //:   total-milliseconds representations of three objects:
        //:   '(LHS, RHS, SUM = LHS + RHS)'.
        //:
        //: 3 For each row 'R' in the table of P-2:  (C-1, 3..4)
        //:
        //:   1 Use the default constructor and 'setTotalMilliseconds' to
        //:     create a modifiable object, 'mX', having the value 'LHS' from
        //:     'R'.
        //:
        //:   2 Use the default constructor and 'setTotalMilliseconds' to
        //:     create an object, 'Z', having the value 'RHS' from 'R'; also
        //:     use the copy constructor to create a 'const' 'Obj', 'ZZ', from
        //:     'Z'.
        //:
        //:   3 Use the default constructor and 'setTotalMilliseconds' to
        //:     create an object, 'W', having the value 'SUM' from 'R'.
        //:
        //:   4 Use the '+=' compound assignment operator to add 'Z' to 'mX'
        //:     ('mX += Z').
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.  (C-3)
        //:
        //:   6 Use the equality-comparison operator to verify that: (C-1, 4)
        //:
        //:     1 The target object, 'mX', now has the same value as that of
        //:       'W'.  (C-1)
        //:
        //:     2 'Z' still has the same value as that of 'ZZ'.  (C-4)
        //:
        //: 4 Repeat steps similar to those described in P-2..3 to test
        //:   'operator-=' except that, this time, the rows in the table are
        //:   defined as '(LHS, RHS, DIFF = LHS - RHS)' (P-2) and the operation
        //:   in P-3.4 is 'mX -= Z'.
        //:
        //: 5 To test the alias-safety of 'operator+=', use the table-driven
        //:   technique to specify a set of distinct object values (one per
        //:   row) in terms of their total-milliseconds representation.
        //:
        //: 6 For each row 'R' in the table of P-5:  (C-5)
        //:
        //:   1 Use the default constructor and 'setTotalMilliseconds' to
        //:     create a modifiable 'Obj', 'mX', having the value from 'R'.
        //:
        //:   2 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   3 Use the default constructor and 'setTotalMilliseconds' to
        //:     create an object, 'W', having the value twice that from 'R'.
        //:
        //:   4 Use the '+=' compound assignment operator to add 'Z' to 'mX'
        //:     ('mX += Z').
        //:
        //:   5 Use the equality-comparison operator to verify that the target
        //:     object, 'mX', now has the same value as that of 'W'.  (C-5)
        //:
        //: 7 Repeat steps similar to those described in P-5..6 to test the
        //:   alias-safety of 'operator-=' except that, this time, 'W' is left
        //:   in its default constructed state (P-6.3) and the operation in
        //:   P-6.4 is 'mX -= Z'.
        //:
        //: 8 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid total-millisecond values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-6)
        //
        // Testing:
        //   DatetimeInterval& operator+=(const DatetimeInterval& rhs);
        //   DatetimeInterval& operator-=(const DatetimeInterval& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                         << "ARITHMETIC ASSIGNMENT OPERATORS (+=, -=)" << endl
                         << "========================================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorAddAssignment = &Obj::operator+=;
            operatorPtr operatorSubAssignment = &Obj::operator-=;

            (void)operatorAddAssignment;  // quash potential compiler warning
            (void)operatorSubAssignment;
        }

        if (verbose) cout << "\nTesting 'operator+='." << endl;
        {
            static const struct {
                int   d_line;      // source line number
                Int64 d_lhsMsecs;  // lhs total milliseconds
                Int64 d_rhsMsecs;  // rhs total milliseconds
                Int64 d_sumMsecs;  // lhs + rhs total milliseconds
            } DATA[] = {
                //LINE  LHS MILLISECONDS  RHS MILLISECONDS  SUM MILLISECONDS
                //----  ----------------  ----------------  ----------------
                { L_,                 0,                0,                 0 },
                { L_,                 1,                0,                 1 },
                { L_,                 0,             1200,              1200 },
                { L_,             13027,           -42058,    13027 + -42058 },
                { L_,            -32546,            32546,                 0 },
                { L_,           INT_MIN,          INT_MAX,                -1 },
                { L_,   k_MSECS_MAX / 2,  k_MSECS_MAX / 2,   k_MSECS_MAX - 1 },
                { L_,   k_MSECS_MIN / 2,  k_MSECS_MIN / 2,   k_MSECS_MIN + 1 },
                { L_,   k_MSECS_MAX - 1,                1,       k_MSECS_MAX },
                { L_,               -27, k_MSECS_MIN + 27,       k_MSECS_MIN },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE      = DATA[ti].d_line;
                const Int64 LHS_MSECS = DATA[ti].d_lhsMsecs;
                const Int64 RHS_MSECS = DATA[ti].d_rhsMsecs;
                const Int64 SUM_MSECS = DATA[ti].d_sumMsecs;

                Obj mX;  const Obj& X = mX;
                mX.setTotalMilliseconds(LHS_MSECS);

                Obj mZ;  const Obj& Z = mZ;
                mZ.setTotalMilliseconds(RHS_MSECS);

                const Obj ZZ(Z);

                if (veryVerbose) { T_ P_(X) P_(Z) }

                Obj mW;  const Obj& W = mW;
                mW.setTotalMilliseconds(SUM_MSECS);

                if (veryVerbose) { T_ T_ P(W) }

                Obj *mR = &(mX += Z);

                LOOP3_ASSERT(LINE,  W,   X,  W == X);
                LOOP3_ASSERT(LINE, mR, &mX, mR == &mX);
                LOOP3_ASSERT(LINE, ZZ,   Z, ZZ == Z);
            }
        }

        if (verbose) cout << "\t'operator+=' self-assignment." << endl;
        {
            static const struct {
                int   d_line;      // source line number
                Int64 d_lhsMsecs;  // lhs total milliseconds
            } DATA[] = {
                //LINE  LHS MILLISECONDS
                //----  ----------------
                { L_,                  0 },
                { L_,                  1 },
                { L_,              13027 },
                { L_,             -32546 },
                { L_,            INT_MAX },
                { L_,            INT_MIN },
                { L_,    k_MSECS_MAX / 2 },
                { L_,    k_MSECS_MIN / 2 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE      = DATA[ti].d_line;
                const Int64 LHS_MSECS = DATA[ti].d_lhsMsecs;

                Obj mX;  const Obj& X = mX;
                mX.setTotalMilliseconds(LHS_MSECS);

                const Obj& Z = mX;

                if (veryVerbose) { T_ P_(X) }

                Obj mW;  const Obj& W = mW;
                mW.setTotalMilliseconds(2 * LHS_MSECS);

                if (veryVerbose) { T_ T_ P(W) }

                Obj *mR = &(mX += Z);

                LOOP3_ASSERT(LINE,  W,   X,  W == X);
                LOOP3_ASSERT(LINE, mR, &mX, mR == &mX);
            }
        }

        if (verbose) cout << "\nTesting 'operator-='." << endl;
        {
            static const struct {
                int   d_line;       // source line number
                Int64 d_lhsMsecs;   // lhs total milliseconds
                Int64 d_rhsMsecs;   // rhs total milliseconds
                Int64 d_diffMsecs;  // lhs - rhs total milliseconds
            } DATA[] = {
                //LINE  LHS MILLISECONDS  RHS MILLISECONDS  DIFF MILLISECONDS
                //----  ----------------  ----------------  -----------------
                { L_,                 0,                0,                 0 },
                { L_,                 1,                0,                 1 },
                { L_,                 0,             1200,             -1200 },
                { L_,             42058,           -13027,     42058 + 13027 },
                { L_,           INT_MIN,          INT_MAX,
                                         2 * static_cast<Int64>(INT_MIN) + 1 },
                { L_,   k_MSECS_MAX / 2,  k_MSECS_MAX / 2,                 0 },
                { L_,   k_MSECS_MAX - 3,               -3,       k_MSECS_MAX },
                { L_, k_MSECS_MIN + 999,              999,       k_MSECS_MIN },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE       = DATA[ti].d_line;
                const Int64 LHS_MSECS  = DATA[ti].d_lhsMsecs;
                const Int64 RHS_MSECS  = DATA[ti].d_rhsMsecs;
                const Int64 DIFF_MSECS = DATA[ti].d_diffMsecs;

                Obj mX;  const Obj& X = mX;
                mX.setTotalMilliseconds(LHS_MSECS);

                Obj mZ;  const Obj& Z = mZ;
                mZ.setTotalMilliseconds(RHS_MSECS);

                const Obj ZZ(Z);

                if (veryVerbose) { T_ P_(X) P_(Z) }

                Obj mW;  const Obj& W = mW;
                mW.setTotalMilliseconds(DIFF_MSECS);

                if (veryVerbose) { T_ T_ P(W) }

                Obj *mR = &(mX -= Z);

                LOOP3_ASSERT(LINE,  W,   X,  W == X);
                LOOP3_ASSERT(LINE, mR, &mX, mR == &mX);
                LOOP3_ASSERT(LINE, ZZ,   Z, ZZ == Z);
            }
        }

        if (verbose) cout << "\t'operator-=' self-assignment." << endl;
        {
            static const struct {
                int   d_line;      // source line number
                Int64 d_lhsMsecs;  // lhs total milliseconds
            } DATA[] = {
                //LINE  LHS MILLISECONDS
                //----  ----------------
                { L_,                  0 },
                { L_,                  1 },
                { L_,               -999 },
                { L_,              42058 },
                { L_,            INT_MAX },
                { L_,            INT_MIN },
                { L_,        k_MSECS_MAX },
                { L_,        k_MSECS_MIN },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE      = DATA[ti].d_line;
                const Int64 LHS_MSECS = DATA[ti].d_lhsMsecs;

                Obj mX;  const Obj& X = mX;
                mX.setTotalMilliseconds(LHS_MSECS);

                const Obj& Z = mX;

                if (veryVerbose) { T_ P_(X) }

                const Obj W;

                if (veryVerbose) { T_ T_ P(W) }

                Obj *mR = &(mX -= Z);

                LOOP3_ASSERT(LINE,  W,   X,  W == X);
                LOOP3_ASSERT(LINE, mR, &mX, mR == &mX);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'operator+='" << endl;
            {
                Obj mX;
                Obj mY;  const Obj& Y = mY;

                mX.setTotalMilliseconds(k_MSECS_MAX - 1);
                mY.setTotalMilliseconds(1);

                                                  ASSERT_SAFE_PASS(mX += Y);
                mY.addMilliseconds(1);            ASSERT_SAFE_FAIL(mX += Y);

                mX.setTotalMilliseconds(k_MSECS_MIN + 1);
                mY.setTotalMilliseconds(-1);

                                                  ASSERT_SAFE_PASS(mX += Y);
                mY.addMilliseconds(-1);           ASSERT_SAFE_FAIL(mX += Y);
            }

            if (veryVerbose) cout << "\t'operator-='" << endl;
            {
                Obj mX;
                Obj mY;  const Obj& Y = mY;

                mX.setTotalMilliseconds(k_MSECS_MAX - 1);
                mY.setTotalMilliseconds(-1);

                                                  ASSERT_SAFE_PASS(mX -= Y);
                mY.addMilliseconds(-1);           ASSERT_SAFE_FAIL(mX -= Y);

                mX.setTotalMilliseconds(k_MSECS_MIN + 1);
                mY.setTotalMilliseconds(1);

                                                  ASSERT_SAFE_PASS(mX -= Y);
                mY.addMilliseconds(1);            ASSERT_SAFE_FAIL(mX -= Y);
            }
        }

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // RELATIONAL-COMPARISON OPERATORS (<, <=, >, >=)
        //   Ensure that each operator defines the correct relationship between
        //   the underlying total-milliseconds representations of its operands.
        //
        // Concerns:
        //: 1 An object 'X' is in relation to an object 'Y' as the
        //:   total-milliseconds representation of 'X' is in relation to the
        //:   total-milliseconds representation of 'Y'.
        //:
        //: 2 'false == (X <  X)' (i.e., irreflexivity).
        //:
        //: 3 'true  == (X <= X)' (i.e., reflexivity).
        //:
        //: 4 'false == (X >  X)' (i.e., irreflexivity).
        //:
        //: 5 'true  == (X >= X)' (i.e., reflexivity).
        //:
        //: 6 If 'X < Y', then '!(Y < X)' (i.e., asymmetry).
        //:
        //: 7 'X <= Y' if and only if 'X < Y' exclusive-or 'X == Y'.
        //:
        //: 8 If 'X > Y', then '!(Y > X)' (i.e., asymmetry).
        //:
        //: 9 'X >= Y' if and only if 'X > Y' exclusive-or 'X == Y'.
        //:
        //:10 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., all relational-comparison operators are free functions).
        //:
        //:11 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //:12 The relational-comparison operators' signatures and return types
        //:   are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator<', 'operator<=',
        //:   'operator>', and 'operator>=' to initialize function pointers
        //:   having the appropriate signatures and return types for the four
        //:   homogeneous, free relational-comparison operators defined in this
        //:   component.  (C-10..12)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their total-milliseconds
        //:   representation.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1..9)
        //:
        //:   1 Use the default constructor and 'setTotalMilliseconds' to
        //:     create an object, 'W', having the value from 'R1'.
        //:
        //:   2 Using 'W', verify the anti-reflexive (reflexive) property of
        //:     '<' and '>' ('<=' and '>=') in the presence of aliasing.
        //:     (C-2..5)
        //:
        //:   3 For each row 'R2' in the table of P-2:  (C-1, 6..9)
        //:
        //:     1 Use the default constructor and 'setTotalMilliseconds' to
        //:       create an object, 'X', having the value from 'R1'.
        //:
        //:     2 Use the default constructor and 'setTotalMilliseconds' to
        //:       create a second object, 'Y', having the value from 'R2'.
        //:
        //:     3 Record, in 'EXP', whether or not an object set to the value
        //:       from 'R1' is expected to compare less than a (distinct)
        //:       object set to the value from 'R2'.
        //:
        //:     4 Using 'X' and 'Y', verify the asymmetric property and
        //:       expected return value for '<'.  (C-6)
        //:
        //:     5 Record, in 'EXP', whether or not an object set to the value
        //:       from 'R1' is expected to compare less or equal to a
        //:       (distinct) object set to the value from 'R2'.
        //:
        //:     6 Using 'X' and 'Y', verify the expected return value for '<='.
        //:       (C-7)
        //:
        //:     7 Record, in 'EXP', whether or not an object set to the value
        //:       from 'R1' is expected to compare greater than a (distinct)
        //:       object set to the value from 'R2'.
        //:
        //:     8 Using 'X' and 'Y', verify the asymmetric property and
        //:       expected return value for '>'.  (C-8)
        //:
        //:     9 Record, in 'EXP', whether or not an object set to the value
        //:       from 'R1' is expected to compare greater than or equal to a
        //:       (distinct) object set to the value from 'R2'.
        //:
        //:    10 Using 'X' and 'Y', verify the expected return value for '>='.
        //:       (C-1, 9)
        //
        // Testing:
        //   bool operator< (const DatetimeInterval& lhs, rhs);
        //   bool operator<=(const DatetimeInterval& lhs, rhs);
        //   bool operator> (const DatetimeInterval& lhs, rhs);
        //   bool operator>=(const DatetimeInterval& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                   << "RELATIONAL-COMPARISON OPERATORS (<, <=, >, >=)" << endl
                   << "==============================================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorLt = bdlt::operator<;
            operatorPtr operatorLe = bdlt::operator<=;
            operatorPtr operatorGt = bdlt::operator>;
            operatorPtr operatorGe = bdlt::operator>=;

            (void)operatorLt;  // quash potential compiler warnings
            (void)operatorLe;
            (void)operatorGt;
            (void)operatorGe;
        }

        static const struct {
            int   d_line;        // source line number
            Int64 d_totalMsecs;
        } DATA[] = {
            //LINE   TOTAL MILLISECONDS
            //----   ------------------
            { L_,                    0 },

            { L_,                    1 },
            { L_,                13027 },
            { L_,              INT_MAX },
            { L_,          k_MSECS_MAX },

            { L_,                   -1 },
            { L_,               -42058 },
            { L_,              INT_MIN },
            { L_,          k_MSECS_MIN },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   ILINE        = DATA[ti].d_line;
            const Int64 ITOTAL_MSECS = DATA[ti].d_totalMsecs;

            if (veryVerbose) { T_ P_(ILINE) P(ITOTAL_MSECS) }

            // Ensure an object compares correctly with itself (alias test).
            {
                Obj mW;  const Obj& W = mW;
                mW.setTotalMilliseconds(ITOTAL_MSECS);

                LOOP2_ASSERT(ILINE, W, !(W <  W));
                LOOP2_ASSERT(ILINE, W,   W <= W);
                LOOP2_ASSERT(ILINE, W, !(W >  W));
                LOOP2_ASSERT(ILINE, W,   W >= W);
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int   JLINE        = DATA[tj].d_line;
                const Int64 JTOTAL_MSECS = DATA[tj].d_totalMsecs;

                if (veryVerbose) { T_ P_(JLINE) P(JTOTAL_MSECS) }

                Obj mX;  const Obj& X = mX;
                mX.setTotalMilliseconds(ITOTAL_MSECS);

                Obj mY;  const Obj& Y = mY;
                mY.setTotalMilliseconds(JTOTAL_MSECS);

                if (veryVerbose) { T_ T_ P_(X) P(Y) }

                // Verify 'operator<'.

                {
                    const bool EXP = ITOTAL_MSECS < JTOTAL_MSECS;

                    LOOP4_ASSERT(ILINE, JLINE, X, Y, EXP == (X < Y));

                    if (EXP) {
                        LOOP4_ASSERT(ILINE, JLINE, Y, X, !(Y < X));
                    }
                }

                // Verify 'operator<='.

                {
                    const bool EXP = ITOTAL_MSECS <= JTOTAL_MSECS;

                    LOOP4_ASSERT(ILINE, JLINE, X, Y, EXP == (X <= Y));
                    LOOP4_ASSERT(ILINE, JLINE, X, Y,
                                 EXP == ((X < Y) ^ (X == Y)));
                }

                // Verify 'operator>'.

                {
                    const bool EXP = ITOTAL_MSECS > JTOTAL_MSECS;

                    LOOP4_ASSERT(ILINE, JLINE, X, Y, EXP == (X > Y));

                    if (EXP) {
                        LOOP4_ASSERT(ILINE, JLINE, Y, X, !(Y > X));
                    }
                }

                // Verify 'operator>='.

                {
                    const bool EXP = ITOTAL_MSECS >= JTOTAL_MSECS;

                    LOOP4_ASSERT(ILINE, JLINE, X, Y, EXP == (X >= Y));
                    LOOP4_ASSERT(ILINE, JLINE, X, Y,
                                 EXP == ((X > Y) ^ (X == Y)));
                }
            }
        }

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // 'addInterval' MANIPULATOR
        //   Ensure that the method correctly adjusts the underlying
        //   total-milliseconds representation of the object.
        //
        // Concerns:
        //: 1 The 'addInterval' method correctly updates the object state from
        //:   the state on entry and the supplied arguments.
        //:
        //: 2 'addInterval' accepts the contractually specified range of
        //:   argument values.
        //:
        //: 3 The optional 'hours', 'minutes', 'seconds', and 'milliseconds'
        //:   parameters each have the correct default value (0).
        //:
        //: 4 'addInterval' accepts time intervals that are specified using a
        //:    mix of positive, negative, and zero values for the 'days',
        //:    'hours', 'minutes', 'seconds', and 'milliseconds' fields.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of distinct object values (one per row) in terms
        //:     of their five-field representation.
        //:
        //:   2 Additionally, provide a (five-valued) column, 'NARGS',
        //:     indicating the number of significant field values in each row
        //:     of the table, where 'NARGS' is in the range '[1 .. 5]'.
        //:
        //: 2 For each row 'R1' in the table of P-1:  (C-1..4)
        //:
        //:   1 Use the value constructor to create a 'const' 'Obj', 'W', with
        //:     the value from 'R1'.
        //:
        //:   2 Use the default constructor to create two objects, 'X' and 'Y'.
        //:
        //:   3 Use the 'addInterval' method to add the value from 'R1' to 'X',
        //:     supplying only 'NARGS' arguments to the method (letting the
        //:     remaining '5 - NARGS' arguments take their default values).
        //:
        //:   4 Also use the 'addInterval' method to add the value from 'R1' to
        //:     'Y', but this time passing '5 - NARGS' trailing 0 arguments
        //:     explicitly.
        //:
        //:   5 Use the equality-comparison operator to verify that 'W' and 'X'
        //:     have the same value, and that 'X' and 'Y' have the same value.
        //:
        //:   6 For each row 'R2' in the table of P-1:  (C-1..4)
        //:
        //:     1 Use the default constructor and 'setTotalMilliseconds' to
        //:       create an object, 'Z', having the value that is the sum of
        //:       values from 'R1' and 'R2'.
        //:
        //:     2 Use the copy constructor to create two objects, 'U' and 'V',
        //:       from 'W'.
        //:
        //:     3 Use the 'addInterval' method to add the value from 'R2' to
        //:       'U', supplying only 'NARGS' arguments to the method (letting
        //:       the remaining '5 - NARGS' arguments take their default
        //:       values).
        //:
        //:     4 Also use the 'addInterval' method to add the value from 'R2'
        //:       to 'V', but this time passing '5 - NARGS' trailing 0
        //:       arguments explicitly.
        //:
        //:     5 Use the equality-comparison operator to verify that 'Z' and
        //:       'U' have the same value, and that 'U' and 'V' have the same
        //:       value.  (C-1..4)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid total-millisecond values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-5)
        //
        // Testing:
        //   void addInterval(int d, Int64 h = 0, m = 0, s = 0, ms = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'addInterval' MANIPULATOR" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nUse a table of distinct object values."
                          << endl;

        const int          NUM_DATA        = ALT_NUM_DATA;
        const AltDataRow (&DATA)[NUM_DATA] = ALT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   ILINE  = DATA[ti].d_line;
            const int   INARGS = DATA[ti].d_nargs;
            const int   IDAYS  = DATA[ti].d_days;
            const Int64 IHOURS = DATA[ti].d_hours;
            const Int64 IMINS  = DATA[ti].d_mins;
            const Int64 ISECS  = DATA[ti].d_secs;
            const Int64 IMSECS = DATA[ti].d_msecs;

            if (veryVeryVerbose) {
                T_ P_(ILINE) P_(IDAYS) P_(IHOURS) P_(IMINS) P_(ISECS) P(IMSECS)
            }

            const Obj W(IDAYS, IHOURS, IMINS, ISECS, IMSECS);

            Obj mX;  const Obj& X = mX;
            Obj mY;  const Obj& Y = mY;

            switch (INARGS) {
              case 1: {
                mX.addInterval(IDAYS);
                mY.addInterval(IDAYS,      0,     0,     0,      0);
              } break;
              case 2: {
                mX.addInterval(IDAYS, IHOURS);
                mY.addInterval(IDAYS, IHOURS,     0,     0,      0);
              } break;
              case 3: {
                mX.addInterval(IDAYS, IHOURS, IMINS);
                mY.addInterval(IDAYS, IHOURS, IMINS,     0,      0);
              } break;
              case 4: {
                mX.addInterval(IDAYS, IHOURS, IMINS, ISECS);
                mY.addInterval(IDAYS, IHOURS, IMINS, ISECS,      0);
              } break;
              case 5: {
                mX.addInterval(IDAYS, IHOURS, IMINS, ISECS, IMSECS);
                mY.addInterval(IDAYS, IHOURS, IMINS, ISECS, IMSECS);
              } break;
              default: {
                LOOP_ASSERT(INARGS, !"Bad 'INARGS' value.");
              } break;
            }

            if (veryVerbose) { T_ P_(W) P_(X) T_ P(Y) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(ILINE, Obj(), X, Obj() == X)
                firstFlag = false;
            }

            LOOP3_ASSERT(ILINE, W, X, W == X);
            LOOP3_ASSERT(ILINE, X, Y, X == Y);

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int   JLINE  = DATA[tj].d_line;
                const int   JNARGS = DATA[tj].d_nargs;
                const int   JDAYS  = DATA[tj].d_days;
                const Int64 JHOURS = DATA[tj].d_hours;
                const Int64 JMINS  = DATA[tj].d_mins;
                const Int64 JSECS  = DATA[tj].d_secs;
                const Int64 JMSECS = DATA[tj].d_msecs;

                if (veryVeryVerbose) {
                    T_ P_(JLINE)
                    P_(JDAYS) P_(JHOURS) P_(JMINS) P_(JSECS) P(JMSECS)
                }

                const Int64 TOTAL_MSECS = W.totalMilliseconds()
                             + flds2Msecs(JDAYS, JHOURS, JMINS, JSECS, JMSECS);

                if (k_MSECS_MIN > TOTAL_MSECS || k_MSECS_MAX < TOTAL_MSECS) {
                    if (veryVeryVerbose) {
                        cout << "\tSkipping data that would overflow 'Int64'."
                             << endl;
                    }
                    continue;
                }

                Obj mZ;  const Obj& Z = mZ;
                mZ.setTotalMilliseconds(TOTAL_MSECS);

                Obj mU(W);  const Obj& U = mU;
                Obj mV(W);  const Obj& V = mV;

                switch (JNARGS) {
                  case 1: {
                    mU.addInterval(JDAYS);
                    mV.addInterval(JDAYS,      0,     0,     0,      0);
                  } break;
                  case 2: {
                    mU.addInterval(JDAYS, JHOURS);
                    mV.addInterval(JDAYS, JHOURS,     0,     0,      0);
                  } break;
                  case 3: {
                    mU.addInterval(JDAYS, JHOURS, JMINS);
                    mV.addInterval(JDAYS, JHOURS, JMINS,     0,      0);
                  } break;
                  case 4: {
                    mU.addInterval(JDAYS, JHOURS, JMINS, JSECS);
                    mV.addInterval(JDAYS, JHOURS, JMINS, JSECS,      0);
                  } break;
                  case 5: {
                    mU.addInterval(JDAYS, JHOURS, JMINS, JSECS, JMSECS);
                    mV.addInterval(JDAYS, JHOURS, JMINS, JSECS, JMSECS);
                  } break;
                  default: {
                    LOOP_ASSERT(JNARGS, !"Bad 'JNARGS' value.");
                  } break;
                }

                if (veryVerbose) { T_ P_(Z) P_(U) T_ P(V) }

                LOOP3_ASSERT(JLINE, Z, U, Z == U);
                LOOP3_ASSERT(JLINE, U, V, U == V);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

#ifdef RESET_X
#undef RESET_X
#endif
#define RESET_X mX.setTotalMilliseconds(0)

            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.addInterval(0, k_HOURS_MIN - 1));
                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, k_HOURS_MIN    ));

                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, k_HOURS_MAX    ));
                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(0, k_HOURS_MAX + 1));

                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(0, 0, k_MINS_MIN - 1));
                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, 0, k_MINS_MIN    ));

                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, 0, k_MINS_MAX    ));
                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(0, 0, k_MINS_MAX + 1));

                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(0, 0, 0, k_SECS_MIN - 1));
                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, 0, 0, k_SECS_MIN    ));

                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, 0, 0, k_SECS_MAX    ));
                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(0, 0, 0, k_SECS_MAX + 1));

                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(0, 0, 0, 0, k_MSECS_MIN - 1));
                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, 0, 0, 0, k_MSECS_MIN    ));

                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, 0, 0, 0, k_MSECS_MAX    ));
                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(0, 0, 0, 0, k_MSECS_MAX + 1));
            }

            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.addInterval(k_DAYS_MIN, -24));
                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(k_DAYS_MIN, -23));

                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(k_DAYS_MAX,  23));
                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(k_DAYS_MAX,  24));

                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(k_DAYS_MIN, -23, -60));
                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(k_DAYS_MIN, -23, -59));

                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(k_DAYS_MAX,  23,  59));
                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(k_DAYS_MAX,  23,  60));

                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(k_DAYS_MIN, -23, -59, -60));
                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(k_DAYS_MIN, -23, -59, -59));

                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(k_DAYS_MAX,  23,  59,  59));
                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(k_DAYS_MAX,  23,  59,  60));

                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(k_DAYS_MIN, -23, -59, -59,
                                                                       -1000));
                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(k_DAYS_MIN, -23, -59, -59,
                                                                        -999));

                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(k_DAYS_MAX,  23,  59,  59,
                                                                         999));
                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(k_DAYS_MAX,  23,  59,  59,
                                                                        1000));

                mX.setTotalDays(k_DAYS_MIN + 1);
                ASSERT_SAFE_FAIL(mX.addInterval(-2, 0, 0, 0, 0));

                mX.setTotalDays(k_DAYS_MIN + 1);
                ASSERT_SAFE_PASS(mX.addInterval(-1, 0, 0, 0, 0));

                mX.setTotalDays(k_DAYS_MAX - 1);
                ASSERT_SAFE_PASS(mX.addInterval( 1, 0, 0, 0, 0));

                mX.setTotalDays(k_DAYS_MAX - 1);
                ASSERT_SAFE_FAIL(mX.addInterval( 2, 0, 0, 0, 0));
            }

            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.addInterval(0, k_HOURS_MIN, -60));
                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, k_HOURS_MIN, -59));

                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, k_HOURS_MAX,  59));
                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(0, k_HOURS_MAX,  60));

                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(0, k_HOURS_MIN, -59, -60));
                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, k_HOURS_MIN, -59, -59));

                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, k_HOURS_MAX,  59,  59));
                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(0, k_HOURS_MAX,  59,  60));

                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(0, k_HOURS_MIN, -59, -59,
                                                                       -1000));
                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, k_HOURS_MIN, -59, -59,
                                                                        -999));

                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, k_HOURS_MAX,  59,  59,
                                                                         999));
                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(0, k_HOURS_MAX,  59,  59,
                                                                        1000));

                mX.setTotalHours(k_HOURS_MIN + 1);
                ASSERT_SAFE_FAIL(mX.addInterval(0, -2, 0, 0, 0));

                mX.setTotalHours(k_HOURS_MIN + 1);
                ASSERT_SAFE_PASS(mX.addInterval(0, -1, 0, 0, 0));

                mX.setTotalHours(k_HOURS_MAX - 1);
                ASSERT_SAFE_PASS(mX.addInterval(0,  1, 0, 0, 0));

                mX.setTotalHours(k_HOURS_MAX - 1);
                ASSERT_SAFE_FAIL(mX.addInterval(0,  2, 0, 0, 0));
            }

            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.addInterval(0, 0, k_MINS_MIN, -60));
                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, 0, k_MINS_MIN, -59));

                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, 0, k_MINS_MAX,  59));
                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(0, 0, k_MINS_MAX,  60));

                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(0, 0, k_MINS_MIN, -59, -1000));
                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, 0, k_MINS_MIN, -59,  -999));

                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, 0, k_MINS_MAX,  59,   999));
                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(0, 0, k_MINS_MAX,  59,  1000));

                mX.setTotalMinutes(k_MINS_MIN + 1);
                ASSERT_SAFE_FAIL(mX.addInterval(0, 0, -2, 0, 0));

                mX.setTotalMinutes(k_MINS_MIN + 1);
                ASSERT_SAFE_PASS(mX.addInterval(0, 0, -1, 0, 0));

                mX.setTotalMinutes(k_MINS_MAX - 1);
                ASSERT_SAFE_PASS(mX.addInterval(0, 0,  1, 0, 0));

                mX.setTotalMinutes(k_MINS_MAX - 1);
                ASSERT_SAFE_FAIL(mX.addInterval(0, 0,  2, 0, 0));
            }

            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.addInterval(0, 0, 0, k_SECS_MIN, -1000));
                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, 0, 0, k_SECS_MIN,  -999));

                RESET_X;
                ASSERT_SAFE_PASS(mX.addInterval(0, 0, 0, k_SECS_MAX,   999));
                RESET_X;
                ASSERT_SAFE_FAIL(mX.addInterval(0, 0, 0, k_SECS_MAX,  1000));

                mX.setTotalSeconds(k_SECS_MIN + 1);
                ASSERT_SAFE_FAIL(mX.addInterval(0, 0, 0, -2, 0));

                mX.setTotalSeconds(k_SECS_MIN + 1);
                ASSERT_SAFE_PASS(mX.addInterval(0, 0, 0, -1, 0));

                mX.setTotalSeconds(k_SECS_MAX - 1);
                ASSERT_SAFE_PASS(mX.addInterval(0, 0, 0,  1, 0));

                mX.setTotalSeconds(k_SECS_MAX - 1);
                ASSERT_SAFE_FAIL(mX.addInterval(0, 0, 0,  2, 0));
            }

            {
                Obj mX;

                mX.setTotalMilliseconds(k_MSECS_MIN + 1);
                ASSERT_SAFE_FAIL(mX.addInterval(0, 0, 0, 0, -2));

                mX.setTotalMilliseconds(k_MSECS_MIN + 1);
                ASSERT_SAFE_PASS(mX.addInterval(0, 0, 0, 0, -1));

                mX.setTotalMilliseconds(k_MSECS_MAX - 1);
                ASSERT_SAFE_PASS(mX.addInterval(0, 0, 0, 0,  1));

                mX.setTotalMilliseconds(k_MSECS_MAX - 1);
                ASSERT_SAFE_FAIL(mX.addInterval(0, 0, 0, 0,  2));
            }

#undef RESET_X
        }

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // FIELD-SPECIFIC 'add*' MANIPULATORS
        //   Ensure that each method correctly adjusts the underlying
        //   total-milliseconds representation of the object.
        //
        // Concerns:
        //: 1 Each method correctly updates the object state from the state on
        //:   entry and the supplied argument.
        //:
        //: 2 Each manipulator accepts the contractually specified range of
        //:   argument values.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique to test 'addDays', specify a set
        //:   of triplets (one per row) in terms of: (a) the total-milliseconds
        //:   representation of the initial state of an object ('PRE_MSECS'),
        //:   (b) a number of 'DAYS', and (c) the total-milliseconds
        //:   representation of the object after 'DAYS' have been added to its
        //:   value ('POST_MSECS').
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..2)
        //:
        //:   1 Use the default constructor to create two objects, 'W' and 'X'.
        //:
        //:   2 Use the 'addDays' method to add the 'DAYS' value from 'R' to
        //:     'W'.
        //:
        //:   3 Also use the 'setTotalDays' method to set the value of 'X' to
        //:     the 'DAYS' value from 'R'.
        //:
        //:   4 Use the equality-comparison operator to verify that 'W' and 'X'
        //:     have the same value.
        //:
        //:   5 Use the default constructor and 'setTotalMilliseconds' to
        //:     create an object, 'Y', having the value 'PRE_MSECS' from 'R'.
        //:
        //:   6 Use the 'addDays' method to add the 'DAYS' value from 'R' to
        //:     'Y'.
        //:
        //:   7 Use the 'totalMilliseconds' accessor to verify that 'Y' now has
        //:     the expected 'POST_MSECS' value from 'R'.  (C-1..2)
        //:
        //: 3 Repeat steps similar to those described in P-1..2 to test
        //:   'addHours' ('addMinutes', 'addSeconds', 'addMilliseconds') except
        //:   that, this time: (a) the second column of the table (P-1) is a
        //:   specified number of 'HOURS' ('MINUTES', 'SECONDS',
        //:   'MILLISECONDS') instead of 'DAYS', (b) 'addHours' ('addMinutes',
        //:   'addSeconds', 'addMilliseconds') is applied instead of 'addDays'
        //:   (P-2.2, P-2.6), and (c) 'setTotalHours' ('setTotalMinutes',
        //:   'setTotalSeconds', 'setTotalMilliseconds') is applied instead of
        //:   'setTotalDays' (P-2.3).
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid total-millisecond values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-3)
        //
        // Testing:
        //   void addDays(int days);
        //   void addHours(Int64 hours);
        //   void addMinutes(Int64 minutes);
        //   void addSeconds(Int64 seconds);
        //   void addMilliseconds(Int64 milliseconds);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "FIELD-SPECIFIC 'add*' MANIPULATORS" << endl
                          << "==================================" << endl;

        if (verbose) cout << "\nTesting 'addDays'." << endl;
        {
            static const struct {
                int   d_line;       // source line number
                Int64 d_preMsecs;   // total milliseconds before 'addDays'
                int   d_days;       // number of days to add
                Int64 d_postMsecs;  // total milliseconds after 'addDays'
            } DATA[] = {
                //LINE  PRE MILLISECONDS        DAYS       POST MILLISECONDS
                //----  ----------------     -----------   -----------------
                { L_,                 1,              0,                   1 },

                { L_,               100,           1200,
                                                100 + k_MSECS_PER_DAY * 1200 },

                { L_,             13027,         -42058,
                                            13027 + k_MSECS_PER_DAY * -42058 },

                { L_,
               -32546 * k_MSECS_PER_DAY,          32546,                   0 },

                { L_,
            999 + 59 * k_MSECS_PER_SEC
                + 59 * k_MSECS_PER_MIN
                + 23 * k_MSECS_PER_HOUR,     k_DAYS_MAX,         k_MSECS_MAX },

                { L_,
          -999 + -59 * k_MSECS_PER_SEC
               + -59 * k_MSECS_PER_MIN
               + -23 * k_MSECS_PER_HOUR,     k_DAYS_MIN,         k_MSECS_MIN },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE       = DATA[ti].d_line;
                const Int64 PRE_MSECS  = DATA[ti].d_preMsecs;
                const int   DAYS       = DATA[ti].d_days;
                const Int64 POST_MSECS = DATA[ti].d_postMsecs;

                {
                    Obj mW;  const Obj& W = mW;
                    mW.addDays(DAYS);

                    Obj mX;  const Obj& X = mX;
                    mX.setTotalDays(DAYS);

                    if (veryVerbose) { T_ P_(DAYS) T_ P(W) }

                    LOOP2_ASSERT(LINE, DAYS, W == X);
                }

                {
                    Obj mY;  const Obj& Y = mY;
                    mY.setTotalMilliseconds(PRE_MSECS);

                    if (veryVerbose) { T_ P_(Y) T_ P_(DAYS) }

                    mY.addDays(DAYS);

                    if (veryVerbose) { T_ P(Y) }

                    LOOP3_ASSERT(LINE, POST_MSECS, Y.totalMilliseconds(),
                                 POST_MSECS == Y.totalMilliseconds());
                }
            }
        }

        if (verbose) cout << "\nTesting 'addHours'." << endl;
        {
            static const struct {
                int   d_line;       // source line number
                Int64 d_preMsecs;   // total milliseconds before 'addHours'
                Int64 d_hours;      // number of hours to add
                Int64 d_postMsecs;  // total milliseconds after 'addHours'
            } DATA[] = {
                //LINE  PRE MILLISECONDS        HOURS      POST MILLISECONDS
                //----  ----------------     -----------   -----------------
                { L_,                 1,              0,                   1 },

                { L_,               100,           1200,
                                               100 + k_MSECS_PER_HOUR * 1200 },

                { L_,             13027,         -42058,
                                           13027 + k_MSECS_PER_HOUR * -42058 },

                { L_,
              -32546 * k_MSECS_PER_HOUR,          32546,                   0 },

                { L_,
             999 + 59 * k_MSECS_PER_SEC
                 + 59 * k_MSECS_PER_MIN,    k_HOURS_MAX,         k_MSECS_MAX },

                { L_,
           -999 + -59 * k_MSECS_PER_SEC
                + -59 * k_MSECS_PER_MIN,    k_HOURS_MIN,         k_MSECS_MIN },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE       = DATA[ti].d_line;
                const Int64 PRE_MSECS  = DATA[ti].d_preMsecs;
                const Int64 HOURS      = DATA[ti].d_hours;
                const Int64 POST_MSECS = DATA[ti].d_postMsecs;

                {
                    Obj mW;  const Obj& W = mW;
                    mW.addHours(HOURS);

                    Obj mX;  const Obj& X = mX;
                    mX.setTotalHours(HOURS);

                    if (veryVerbose) { T_ P_(HOURS) T_ P(W) }

                    LOOP2_ASSERT(LINE, HOURS, W == X);
                }

                {
                    Obj mY;  const Obj& Y = mY;
                    mY.setTotalMilliseconds(PRE_MSECS);

                    if (veryVerbose) { T_ P_(Y) T_ P_(HOURS) }

                    mY.addHours(HOURS);

                    if (veryVerbose) { T_ P(Y) }

                    LOOP3_ASSERT(LINE, POST_MSECS, Y.totalMilliseconds(),
                                 POST_MSECS == Y.totalMilliseconds());
                }
            }
        }

        if (verbose) cout << "\nTesting 'addMinutes'." << endl;
        {
            static const struct {
                int   d_line;       // source line number
                Int64 d_preMsecs;   // total milliseconds before 'addMinutes'
                Int64 d_mins;       // number of minutes to add
                Int64 d_postMsecs;  // total milliseconds after 'addMinutes'
            } DATA[] = {
                //LINE  PRE MILLISECONDS       MINUTES     POST MILLISECONDS
                //----  ----------------     -----------   -----------------
                { L_,                 1,              0,                   1 },

                { L_,               100,           1200,
                                                100 + k_MSECS_PER_MIN * 1200 },

                { L_,             13027,         -42058,
                                            13027 + k_MSECS_PER_MIN * -42058 },

                { L_,
               -32546 * k_MSECS_PER_MIN,          32546,                   0 },

                { L_,
             999 + 59 * k_MSECS_PER_SEC,     k_MINS_MAX,         k_MSECS_MAX },

                { L_,
           -999 + -59 * k_MSECS_PER_SEC,     k_MINS_MIN,         k_MSECS_MIN },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE       = DATA[ti].d_line;
                const Int64 PRE_MSECS  = DATA[ti].d_preMsecs;
                const Int64 MINS       = DATA[ti].d_mins;
                const Int64 POST_MSECS = DATA[ti].d_postMsecs;

                {
                    Obj mW;  const Obj& W = mW;
                    mW.addMinutes(MINS);

                    Obj mX;  const Obj& X = mX;
                    mX.setTotalMinutes(MINS);

                    if (veryVerbose) { T_ P_(MINS) T_ P(W) }

                    LOOP2_ASSERT(LINE, MINS, W == X);
                }

                {
                    Obj mY;  const Obj& Y = mY;
                    mY.setTotalMilliseconds(PRE_MSECS);

                    if (veryVerbose) { T_ P_(Y) T_ P_(MINS) }

                    mY.addMinutes(MINS);

                    if (veryVerbose) { T_ P(Y) }

                    LOOP3_ASSERT(LINE, POST_MSECS, Y.totalMilliseconds(),
                                 POST_MSECS == Y.totalMilliseconds());
                }
            }
        }

        if (verbose) cout << "\nTesting 'addSeconds'." << endl;
        {
            static const struct {
                int   d_line;       // source line number
                Int64 d_preMsecs;   // total milliseconds before 'addSeconds'
                Int64 d_secs;       // number of seconds to add
                Int64 d_postMsecs;  // total milliseconds after 'addSeconds'
            } DATA[] = {
                //LINE  PRE MILLISECONDS    SECONDS        POST MILLISECONDS
                //----  ----------------  -----------      -----------------
                { L_,                 1,           0,                      1 },

                { L_,               100,        1200,
                                                100 + k_MSECS_PER_SEC * 1200 },

                { L_,             13027,      -42058,
                                            13027 + k_MSECS_PER_SEC * -42058 },

                { L_,
               -32546 * k_MSECS_PER_SEC,       32546,                      0 },

                { L_,               999,  k_SECS_MAX,            k_MSECS_MAX },
                { L_,              -999,  k_SECS_MIN,            k_MSECS_MIN },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE       = DATA[ti].d_line;
                const Int64 PRE_MSECS  = DATA[ti].d_preMsecs;
                const Int64 SECS       = DATA[ti].d_secs;
                const Int64 POST_MSECS = DATA[ti].d_postMsecs;

                {
                    Obj mW;  const Obj& W = mW;
                    mW.addSeconds(SECS);

                    Obj mX;  const Obj& X = mX;
                    mX.setTotalSeconds(SECS);

                    if (veryVerbose) { T_ P_(SECS) T_ P(W) }

                    LOOP2_ASSERT(LINE, SECS, W == X);
                }

                {
                    Obj mY;  const Obj& Y = mY;
                    mY.setTotalMilliseconds(PRE_MSECS);

                    if (veryVerbose) { T_ P_(Y) T_ P_(SECS) }

                    mY.addSeconds(SECS);

                    if (veryVerbose) { T_ P(Y) }

                    LOOP3_ASSERT(LINE, POST_MSECS, Y.totalMilliseconds(),
                                 POST_MSECS == Y.totalMilliseconds());
                }
            }
        }

        if (verbose) cout << "\nTesting 'addMilliseconds'." << endl;
        {
            static const struct {
                int   d_line;       // source line number
                Int64 d_preMsecs;   // total msecs. before 'addMilliseconds'
                Int64 d_msecs;      // number of milliseconds to add
                Int64 d_postMsecs;  // total msecs. after 'addMilliseconds'
            } DATA[] = {
                //LINE  PRE MILLISECONDS    MILLISECONDS    POST MILLISECONDS
                //----  ----------------  ----------------  -----------------
                { L_,                 1,                0,                 1 },
                { L_,               100,             1200,              1300 },
                { L_,             13027,           -42058,    13027 + -42058 },
                { L_,            -32546,            32546,                 0 },
                { L_,           INT_MIN,          INT_MAX,                -1 },
                { L_,   k_MSECS_MAX / 2,  k_MSECS_MAX / 2,   k_MSECS_MAX - 1 },
                { L_,   k_MSECS_MIN / 2,  k_MSECS_MIN / 2,   k_MSECS_MIN + 1 },
                { L_,   k_MSECS_MAX - 1,                1,       k_MSECS_MAX },
                { L_,               -27, k_MSECS_MIN + 27,       k_MSECS_MIN },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE       = DATA[ti].d_line;
                const Int64 PRE_MSECS  = DATA[ti].d_preMsecs;
                const Int64 MSECS      = DATA[ti].d_msecs;
                const Int64 POST_MSECS = DATA[ti].d_postMsecs;

                {
                    Obj mW;  const Obj& W = mW;
                    mW.addMilliseconds(MSECS);

                    Obj mX;  const Obj& X = mX;
                    mX.setTotalMilliseconds(MSECS);

                    if (veryVerbose) { T_ P_(MSECS) T_ P(W) }

                    LOOP2_ASSERT(LINE, MSECS, W == X);
                }

                {
                    Obj mY;  const Obj& Y = mY;
                    mY.setTotalMilliseconds(PRE_MSECS);

                    if (veryVerbose) { T_ P_(Y) T_ P_(MSECS) }

                    mY.addMilliseconds(MSECS);

                    if (veryVerbose) { T_ P(Y) }

                    LOOP3_ASSERT(LINE, POST_MSECS, Y.totalMilliseconds(),
                                 POST_MSECS == Y.totalMilliseconds());
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

#ifdef RESET_X
#undef RESET_X
#endif
#define RESET_X mX.setTotalMilliseconds(0)

            if (veryVerbose) cout << "\t'addDays'" << endl;
            {
                Obj mX;

                mX.setTotalDays(k_DAYS_MIN + 1);
                ASSERT_SAFE_FAIL(mX.addDays(-2));

                mX.setTotalDays(k_DAYS_MIN + 1);
                ASSERT_SAFE_PASS(mX.addDays(-1));

                mX.setTotalDays(k_DAYS_MAX - 1);
                ASSERT_SAFE_PASS(mX.addDays( 1));

                mX.setTotalDays(k_DAYS_MAX - 1);
                ASSERT_SAFE_FAIL(mX.addDays( 2));
            }

            if (veryVerbose) cout << "\t'addHours'" << endl;
            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.addHours(k_HOURS_MIN - 1));
                RESET_X;
                ASSERT_SAFE_PASS(mX.addHours(k_HOURS_MIN    ));

                RESET_X;
                ASSERT_SAFE_PASS(mX.addHours(k_HOURS_MAX    ));
                RESET_X;
                ASSERT_SAFE_FAIL(mX.addHours(k_HOURS_MAX + 1));

                mX.setTotalHours(k_HOURS_MIN + 1);
                ASSERT_SAFE_FAIL(mX.addHours(-2));

                mX.setTotalHours(k_HOURS_MIN + 1);
                ASSERT_SAFE_PASS(mX.addHours(-1));

                mX.setTotalHours(k_HOURS_MAX - 1);
                ASSERT_SAFE_PASS(mX.addHours( 1));

                mX.setTotalHours(k_HOURS_MAX - 1);
                ASSERT_SAFE_FAIL(mX.addHours( 2));
            }

            if (veryVerbose) cout << "\t'addMinutes'" << endl;
            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.addMinutes(k_MINS_MIN - 1));
                RESET_X;
                ASSERT_SAFE_PASS(mX.addMinutes(k_MINS_MIN    ));

                RESET_X;
                ASSERT_SAFE_PASS(mX.addMinutes(k_MINS_MAX    ));
                RESET_X;
                ASSERT_SAFE_FAIL(mX.addMinutes(k_MINS_MAX + 1));

                mX.setTotalMinutes(k_MINS_MIN + 1);
                ASSERT_SAFE_FAIL(mX.addMinutes(-2));

                mX.setTotalMinutes(k_MINS_MIN + 1);
                ASSERT_SAFE_PASS(mX.addMinutes(-1));

                mX.setTotalMinutes(k_MINS_MAX - 1);
                ASSERT_SAFE_PASS(mX.addMinutes( 1));

                mX.setTotalMinutes(k_MINS_MAX - 1);
                ASSERT_SAFE_FAIL(mX.addMinutes( 2));
            }

            if (veryVerbose) cout << "\t'addSeconds'" << endl;
            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.addSeconds(k_SECS_MIN - 1));
                RESET_X;
                ASSERT_SAFE_PASS(mX.addSeconds(k_SECS_MIN    ));

                RESET_X;
                ASSERT_SAFE_PASS(mX.addSeconds(k_SECS_MAX    ));
                RESET_X;
                ASSERT_SAFE_FAIL(mX.addSeconds(k_SECS_MAX + 1));

                mX.setTotalSeconds(k_SECS_MIN + 1);
                ASSERT_SAFE_FAIL(mX.addSeconds(-2));

                mX.setTotalSeconds(k_SECS_MIN + 1);
                ASSERT_SAFE_PASS(mX.addSeconds(-1));

                mX.setTotalSeconds(k_SECS_MAX - 1);
                ASSERT_SAFE_PASS(mX.addSeconds( 1));

                mX.setTotalSeconds(k_SECS_MAX - 1);
                ASSERT_SAFE_FAIL(mX.addSeconds( 2));
            }

            if (veryVerbose) cout << "\t'addMilliseconds'" << endl;
            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.addMilliseconds(k_MSECS_MIN - 1));
                RESET_X;
                ASSERT_SAFE_PASS(mX.addMilliseconds(k_MSECS_MIN    ));

                RESET_X;
                ASSERT_SAFE_PASS(mX.addMilliseconds(k_MSECS_MAX    ));
                RESET_X;
                ASSERT_SAFE_FAIL(mX.addMilliseconds(k_MSECS_MAX + 1));

                mX.setTotalMilliseconds(k_MSECS_MIN + 1);
                ASSERT_SAFE_FAIL(mX.addMilliseconds(-2));

                mX.setTotalMilliseconds(k_MSECS_MIN + 1);
                ASSERT_SAFE_PASS(mX.addMilliseconds(-1));

                mX.setTotalMilliseconds(k_MSECS_MAX - 1);
                ASSERT_SAFE_PASS(mX.addMilliseconds( 1));

                mX.setTotalMilliseconds(k_MSECS_MAX - 1);
                ASSERT_SAFE_FAIL(mX.addMilliseconds( 2));
            }

#undef RESET_X
        }

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // 'total*' ACCESSORS
        //   Ensure each 'total*' accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the "total" value corresponding to the
        //:   units indicated by the method's name.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 The 'totalSecondsAsDouble' method does not lose precision in the
        //:   conversion to 'double' of reasonably large time intervals.
        //
        // Plan:
        //   In case 13, we demonstrated that each "total" accessor (with the
        //   exception of 'totalSecondsAsDouble') works properly when invoked
        //   immediately following an application of its corresponding "total"
        //   setter (e.g., 'setTotalMinutes' followed by 'totalMinutes').  Here
        //   we use a more varied set of object values to further corroborate
        //   that these accessors properly interpret object state, and that
        //   'totalSecondsAsDouble' works as expected.
        //
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their five-field
        //:   representation.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..3)
        //:
        //:   1 Use the value constructor to create a 'const' 'Obj', 'X', with
        //:     the value from 'R'.
        //:
        //:   2 Verify that each "total" accessor, invoked on 'X', returns the
        //:     expected value (in particular, is consistent with the value
        //:     returned by 'totalMilliseconds' invoked on 'X').  (C-1..2)
        //:
        //:   3 In the case of 'totalSecondsAsDouble', verification of that
        //:     method is restricted to those values that should not lose
        //:     precision when converted among 'Int64' and 'double'.  (C-3)
        //
        // Testing:
        //   int totalDays() const;
        //   Int64 totalHours() const;
        //   Int64 totalMinutes() const;
        //   Int64 totalSeconds() const;
        //   double totalSecondsAsDouble() const;
        //   CONCERN: All accessor methods are declared 'const'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'total*' ACCESSORS" << endl
                          << "==================" << endl;

        if (verbose) cout << "\nUse a table of distinct object values."
                          << endl;

        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE  = DATA[ti].d_line;
            const int DAYS  = DATA[ti].d_days;
            const int HOURS = DATA[ti].d_hours;
            const int MINS  = DATA[ti].d_mins;
            const int SECS  = DATA[ti].d_secs;
            const int MSECS = DATA[ti].d_msecs;

            const Obj X(DAYS, HOURS, MINS, SECS, MSECS);

            if (veryVerbose) {
                T_ P_(LINE) P_(DAYS) P_(HOURS) P_(MINS) P_(SECS) P_(MSECS)
                T_ P(X)
            }

            const Int64 TOTAL_MSECS = X.totalMilliseconds();

            LOOP_ASSERT(LINE,
                        TOTAL_MSECS / k_MSECS_PER_DAY  == X.totalDays());
            LOOP_ASSERT(LINE,
                        TOTAL_MSECS / k_MSECS_PER_HOUR == X.totalHours());
            LOOP_ASSERT(LINE,
                        TOTAL_MSECS / k_MSECS_PER_MIN  == X.totalMinutes());
            LOOP_ASSERT(LINE,
                        TOTAL_MSECS / k_MSECS_PER_SEC  == X.totalSeconds());

            const double DBL_SECS =
                    static_cast<double>(TOTAL_MSECS) / (1.0 * k_MSECS_PER_SEC);

#if defined(BSLS_PLATFORM_CPU_X86)                                            \
 && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))
    // This is necessary because on Linux, for some inexplicable reason, even
    // 'X.totalSecondsAsDouble() == X.totalSecondsAsDouble()' returns 'false'.
    // This is probably needed in order to force a narrowing of the value to a
    // 64-bit 'double' from the wider internal processor FP registers.

            volatile double DBL_SECS2 = X.totalSecondsAsDouble();
            LOOP_ASSERT(LINE, (0.0 == DBL_SECS && 0.0 == DBL_SECS2)
                                || fabs(DBL_SECS / DBL_SECS2 - 1.0) < 1.0e-15);

    // The last 'LOOP_ASSERT' is commented out due to a precision problem when
    // casting from 'double' to 'Int64'.  For example:

    // const double T = (double)TOTAL_MSECS / (1.0 * 1000);     // if 0.999
    // P((Int64)(((double)TOTAL_MSECS / (1.0 * 1000)) * 1000))  // prints 999
    // P((Int64)(T * 1000))                                     // prints 998

            if (abs64(TOTAL_MSECS) < static_cast<Int64>(1) << 53) {
                LOOP_ASSERT(LINE, static_cast<Int64>(DBL_SECS) ==
                                                             X.totalSeconds());
            //  LOOP_ASSERT(LINE, TOTAL_MSECS ==
            //                            static_cast<Int64>(DBL_SECS * 1000));
            }
#else
            LOOP_ASSERT(LINE, DBL_SECS == X.totalSecondsAsDouble());

            if (abs64(TOTAL_MSECS) < static_cast<Int64>(1) << 53) {
                LOOP_ASSERT(LINE, static_cast<Int64>(DBL_SECS) ==
                                                             X.totalSeconds());
                LOOP_ASSERT(LINE, TOTAL_MSECS ==
                                          static_cast<Int64>(DBL_SECS * 1000));
            }
#endif
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // 'setTotal*' MANIPULATORS
        //   Ensure that each method correctly computes the underlying
        //   total-milliseconds representation of the object.
        //
        // Concerns:
        //: 1 Each manipulator can set an object to have any "total" value that
        //:   does not violate the method's documented preconditions.
        //:
        //: 2 Each manipulator is not affected by the state of the object on
        //:   entry.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 To test 'setTotalDays', use the table-driven technique to specify
        //:   a set of distinct object values (one per row) in terms of their
        //:   total-days representation.
        //:
        //: 2 For each row 'R1' in the table of P-1:  (C-1..2)
        //:
        //:   1 Use the default constructor and 'setTotalMilliseconds' to
        //:     create an object, 'W', having the value from 'R1' interpreted
        //:     as total milliseconds.  (This expedient reuse of 'R1' is for
        //:     giving 'W' a unique value in each iteration of the loop.)
        //:
        //:   2 For each row 'R2' in the table of P-1:  (C-1..2)
        //:
        //:     1 Use the copy constructor to create an object, 'X', from 'W'.
        //:
        //:     2 Use 'setTotalDays' to set 'X' to have the total number of
        //:       days from 'R2'.
        //:
        //:     3 Verify, using the 'totalMilliseconds' accessor, that the
        //:       new state of 'X' is consistent with 'R2'.
        //:
        //:     4 Also use the (as yet unproven) 'totalDays' accessor, and the
        //:       five field-based accessors, to further corroborate the state
        //:       of 'X'.  (C-1..2)
        //:
        //: 3 Repeat steps similar to those described in P-1..2 to test
        //:   'setTotalHours' ('setTotalMinutes', 'setTotalSeconds') except
        //:   that, this time: (a) the rows of the table (P-1) are in terms of
        //:   total hours (total minutes, total seconds) instead of total days,
        //:   (b) 'setTotalHours' ('setTotalMinutes', 'setTotalSeconds') is
        //:   applied instead of 'setTotalDays' (P-2.2.2), and (c) the
        //:   'totalHours' ('totalMinutes', 'totalSeconds') accessor is used
        //:   for verification instead of 'totalDays' (P-2.2.4).
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid total-millisecond values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-3)
        //
        // Testing:
        //   void setTotalDays(int days);
        //   void setTotalHours(Int64 hours);
        //   void setTotalMinutes(Int64 minutes);
        //   void setTotalSeconds(Int64 seconds);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'setTotal*' MANIPULATORS" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nTesting 'setTotalDays'." << endl;
        {
            static const struct {
                int d_line;       // source line number
                int d_totalDays;
            } DATA[] = {
                //LINE   TOTAL DAYS
                //----   ----------
                { L_,             0 },

                { L_,             1 },
                { L_,         13027 },
                { L_,    k_DAYS_MAX },

                { L_,            -1 },
                { L_,        -42058 },
                { L_,    k_DAYS_MIN },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   ILINE               = DATA[ti].d_line;
                const Int64 TOTAL_DAYS_AS_MSECS = DATA[ti].d_totalDays;

                Obj mW;  const Obj& W = mW;

                mW.setTotalMilliseconds(TOTAL_DAYS_AS_MSECS);
                if (veryVerbose) { T_ P_(W) P(TOTAL_DAYS_AS_MSECS) }

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int JLINE      = DATA[tj].d_line;
                    const int TOTAL_DAYS = DATA[tj].d_totalDays;

                    Obj mX(W);  const Obj& X = mX;

                    mX.setTotalDays(TOTAL_DAYS);
                    if (veryVeryVerbose) { T_ P_(X) P(TOTAL_DAYS) }

                    const Int64 TOTAL_MSECS = X.totalMilliseconds();

                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_DAYS,
                                 TOTAL_MSECS == TOTAL_DAYS  * k_MSECS_PER_DAY);
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_DAYS,
                                           0 == TOTAL_MSECS % k_MSECS_PER_DAY);

                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_DAYS,
                                                  TOTAL_DAYS == X.totalDays());

                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_DAYS,
                                               TOTAL_DAYS == X.days());
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_DAYS,
                                                        0 == X.hours());
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_DAYS,
                                                        0 == X.minutes());
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_DAYS,
                                                        0 == X.seconds());
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_DAYS,
                                                        0 == X.milliseconds());
                }
            }
        }

        if (verbose) cout << "\nTesting 'setTotalHours'." << endl;
        {
            static const struct {
                int   d_line;       // source line number
                Int64 d_totalHours;
            } DATA[] = {
                //LINE   TOTAL HOURS
                //----   -----------
                { L_,              0 },

                { L_,              1 },
                { L_,          13027 },
                { L_,    k_HOURS_MAX },

                { L_,             -1 },
                { L_,         -42058 },
                { L_,    k_HOURS_MIN },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   ILINE               = DATA[ti].d_line;
                const Int64 TOTAL_HOURS_AS_MSECS = DATA[ti].d_totalHours;

                Obj mW;  const Obj& W = mW;

                mW.setTotalMilliseconds(TOTAL_HOURS_AS_MSECS);
                if (veryVerbose) { T_ P_(W) P(TOTAL_HOURS_AS_MSECS) }

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int   JLINE      = DATA[tj].d_line;
                    const Int64 TOTAL_HOURS = DATA[tj].d_totalHours;

                    Obj mX(W);  const Obj& X = mX;

                    mX.setTotalHours(TOTAL_HOURS);
                    if (veryVeryVerbose) { T_ P_(X) P(TOTAL_HOURS) }

                    const Int64 TOTAL_MSECS = X.totalMilliseconds();

                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_HOURS,
                                TOTAL_MSECS == TOTAL_HOURS * k_MSECS_PER_HOUR);
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_HOURS,
                                          0 == TOTAL_MSECS % k_MSECS_PER_HOUR);

                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_HOURS,
                                                TOTAL_HOURS == X.totalHours());

                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_HOURS,
                                         TOTAL_HOURS / 24 == X.days());
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_HOURS,
                                         TOTAL_HOURS % 24 == X.hours());
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_HOURS,
                                                        0 == X.minutes());
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_HOURS,
                                                        0 == X.seconds());
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_HOURS,
                                                        0 == X.milliseconds());
                }
            }
        }

        if (verbose) cout << "\nTesting 'setTotalMinutes'." << endl;
        {
            static const struct {
                int   d_line;       // source line number
                Int64 d_totalMins;
            } DATA[] = {
                //LINE   TOTAL MINUTES
                //----   -------------
                { L_,                0 },

                { L_,                1 },
                { L_,            13027 },
                { L_,       k_MINS_MAX },

                { L_,               -1 },
                { L_,           -42058 },
                { L_,       k_MINS_MIN },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   ILINE               = DATA[ti].d_line;
                const Int64 TOTAL_MINS_AS_MSECS = DATA[ti].d_totalMins;

                Obj mW;  const Obj& W = mW;

                mW.setTotalMilliseconds(TOTAL_MINS_AS_MSECS);
                if (veryVerbose) { T_ P_(W) P(TOTAL_MINS_AS_MSECS) }

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int   JLINE      = DATA[tj].d_line;
                    const Int64 TOTAL_MINS = DATA[tj].d_totalMins;

                    Obj mX(W);  const Obj& X = mX;

                    mX.setTotalMinutes(TOTAL_MINS);
                    if (veryVeryVerbose) { T_ P_(X) P(TOTAL_MINS) }

                    const Int64 TOTAL_MSECS = X.totalMilliseconds();

                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_MINS,
                                 TOTAL_MSECS == TOTAL_MINS  * k_MSECS_PER_MIN);
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_MINS,
                                           0 == TOTAL_MSECS % k_MSECS_PER_MIN);

                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_MINS,
                                               TOTAL_MINS == X.totalMinutes());

                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_MINS,
                                     TOTAL_MINS / 60 / 24 == X.days());
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_MINS,
                                     TOTAL_MINS / 60 % 24 == X.hours());
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_MINS,
                                          TOTAL_MINS % 60 == X.minutes());
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_MINS,
                                                        0 == X.seconds());
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_MINS,
                                                        0 == X.milliseconds());
                }
            }
        }

        if (verbose) cout << "\nTesting 'setTotalSeconds'." << endl;
        {
            static const struct {
                int   d_line;       // source line number
                Int64 d_totalSecs;
            } DATA[] = {
                //LINE   TOTAL SECONDS
                //----   -------------
                { L_,                0 },

                { L_,                1 },
                { L_,            13027 },
                { L_,       k_SECS_MAX },

                { L_,               -1 },
                { L_,           -42058 },
                { L_,       k_SECS_MIN },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   ILINE               = DATA[ti].d_line;
                const Int64 TOTAL_SECS_AS_MSECS = DATA[ti].d_totalSecs;

                Obj mW;  const Obj& W = mW;

                mW.setTotalMilliseconds(TOTAL_SECS_AS_MSECS);
                if (veryVerbose) { T_ P_(W) P(TOTAL_SECS_AS_MSECS) }

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int   JLINE      = DATA[tj].d_line;
                    const Int64 TOTAL_SECS = DATA[tj].d_totalSecs;

                    Obj mX(W);  const Obj& X = mX;

                    mX.setTotalSeconds(TOTAL_SECS);
                    if (veryVeryVerbose) { T_ P_(X) P(TOTAL_SECS) }

                    const Int64 TOTAL_MSECS = X.totalMilliseconds();

                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_SECS,
                                 TOTAL_MSECS == TOTAL_SECS  * k_MSECS_PER_SEC);
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_SECS,
                                           0 == TOTAL_MSECS % k_MSECS_PER_SEC);

                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_SECS,
                                               TOTAL_SECS == X.totalSeconds());

                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_SECS,
                                TOTAL_SECS / 60 / 60 / 24 == X.days());
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_SECS,
                                TOTAL_SECS / 60 / 60 % 24 == X.hours());
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_SECS,
                                     TOTAL_SECS / 60 % 60 == X.minutes());
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_SECS,
                                          TOTAL_SECS % 60 == X.seconds());
                    LOOP3_ASSERT(ILINE, JLINE, TOTAL_SECS,
                                                        0 == X.milliseconds());
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'setTotalHours'" << endl;
            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.setTotalHours(k_HOURS_MIN - 1));
                ASSERT_SAFE_PASS(mX.setTotalHours(k_HOURS_MIN    ));

                ASSERT_SAFE_PASS(mX.setTotalHours(k_HOURS_MAX    ));
                ASSERT_SAFE_FAIL(mX.setTotalHours(k_HOURS_MAX + 1));
            }

            if (veryVerbose) cout << "\t'setTotalMinutes'" << endl;
            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.setTotalMinutes(k_MINS_MIN - 1));
                ASSERT_SAFE_PASS(mX.setTotalMinutes(k_MINS_MIN    ));

                ASSERT_SAFE_PASS(mX.setTotalMinutes(k_MINS_MAX    ));
                ASSERT_SAFE_FAIL(mX.setTotalMinutes(k_MINS_MAX + 1));
            }

            if (veryVerbose) cout << "\t'setTotalSeconds'" << endl;
            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.setTotalSeconds(k_SECS_MIN - 1));
                ASSERT_SAFE_PASS(mX.setTotalSeconds(k_SECS_MIN    ));

                ASSERT_SAFE_PASS(mX.setTotalSeconds(k_SECS_MAX    ));
                ASSERT_SAFE_FAIL(mX.setTotalSeconds(k_SECS_MAX + 1));
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // 'setInterval' MANIPULATOR
        //   Ensure that we can put an object into any valid state.
        //
        // Concerns:
        //: 1 The 'setInterval' method can set an object to have any value that
        //:   does not violate the method's documented preconditions.
        //:
        //: 2 'setInterval' is not affected by the state of the object on
        //:   entry.
        //:
        //: 3 The optional 'hours', 'minutes', 'seconds', and 'milliseconds'
        //:   parameters each have the correct default value (0).
        //:
        //: 4 'setInterval' accepts time intervals that are specified using a
        //:    mix of positive, negative, and zero values for the 'days',
        //:    'hours', 'minutes', 'seconds', and 'milliseconds' fields.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of distinct object values (one per row) in terms
        //:     of their five-field representation.
        //:
        //:   2 Additionally, provide a (five-valued) column, 'NARGS',
        //:     indicating the number of significant field values in each row
        //:     of the table, where 'NARGS' is in the range '[1 .. 5]'.
        //:
        //: 2 For each row 'R1' in the table of P-1:  (C-1..4)
        //:
        //:   1 Use the value constructor to create a 'const' 'Obj', 'W', with
        //:     the value from 'R1'.
        //:
        //:   2 Use the default constructor to create two objects, 'X' and 'Y'.
        //:
        //:   3 Use the 'setInterval' method to set the value of 'X' to the
        //:     value in 'R1', supplying only 'NARGS' arguments to the method
        //:     (letting the remaining '5 - NARGS' arguments take their default
        //:     values).
        //:
        //:   4 Also use the 'setInterval' method to set the value of 'Y' to
        //:     the value in 'R1', but this time passing '5 - NARGS' trailing 0
        //:     arguments explicitly.
        //:
        //:   5 Use the equality-comparison operator to verify that 'W' and 'X'
        //:     have the same value, and that 'X' and 'Y' have the same value.
        //:
        //:   6 For each row 'R2' in the table of P-2:  (C-1..4)
        //:
        //:     1 Use the value constructor to create a 'const' 'Obj', 'Z',
        //:       with the value from 'R2'.
        //:
        //:     2 Use the copy constructor to create two objects, 'U' and 'V',
        //:       from 'W'.
        //:
        //:     3 Use the 'setInterval' method to set the value of 'U' to the
        //:       value in 'R2', supplying only 'NARGS' arguments to the method
        //:       (letting the remaining '5 - NARGS' arguments take their
        //:       default values).
        //:
        //:     4 Also use the 'setInterval' method to set the value of 'V' to
        //:       the value in 'R2', but this time passing '5 - NARGS' trailing
        //:       0 arguments explicitly.
        //:
        //:     5 Use the equality-comparison operator to verify that 'Z' and
        //:       'U' have the same value, and that 'U' and 'V' have the same
        //:       value.  (C-1..4)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid total-millisecond values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-5)
        //
        // Testing:
        //   void setInterval(int d, Int64 h = 0, m = 0, s = 0, ms = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'setInterval' MANIPULATOR" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nUse a table of distinct object values."
                          << endl;

        const int          NUM_DATA        = ALT_NUM_DATA;
        const AltDataRow (&DATA)[NUM_DATA] = ALT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   ILINE  = DATA[ti].d_line;
            const int   INARGS = DATA[ti].d_nargs;
            const int   IDAYS  = DATA[ti].d_days;
            const Int64 IHOURS = DATA[ti].d_hours;
            const Int64 IMINS  = DATA[ti].d_mins;
            const Int64 ISECS  = DATA[ti].d_secs;
            const Int64 IMSECS = DATA[ti].d_msecs;

            const Obj W(IDAYS, IHOURS, IMINS, ISECS, IMSECS);

            Obj mX;  const Obj& X = mX;
            Obj mY;  const Obj& Y = mY;

            switch (INARGS) {
              case 1: {
                mX.setInterval(IDAYS);
                mY.setInterval(IDAYS,      0,     0,     0,      0);
              } break;
              case 2: {
                mX.setInterval(IDAYS, IHOURS);
                mY.setInterval(IDAYS, IHOURS,     0,     0,      0);
              } break;
              case 3: {
                mX.setInterval(IDAYS, IHOURS, IMINS);
                mY.setInterval(IDAYS, IHOURS, IMINS,     0,      0);
              } break;
              case 4: {
                mX.setInterval(IDAYS, IHOURS, IMINS, ISECS);
                mY.setInterval(IDAYS, IHOURS, IMINS, ISECS,      0);
              } break;
              case 5: {
                mX.setInterval(IDAYS, IHOURS, IMINS, ISECS, IMSECS);
                mY.setInterval(IDAYS, IHOURS, IMINS, ISECS, IMSECS);
              } break;
              default: {
                LOOP_ASSERT(INARGS, !"Bad 'INARGS' value.");
              } break;
            }

            if (veryVerbose) { T_ P_(X) T_ P(Y) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(ILINE, Obj(), X, Obj() == X)
                firstFlag = false;
            }

            LOOP3_ASSERT(ILINE, W, X, W == X);
            LOOP3_ASSERT(ILINE, X, Y, X == Y);

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int   JLINE  = DATA[tj].d_line;
                const int   JNARGS = DATA[tj].d_nargs;
                const int   JDAYS  = DATA[tj].d_days;
                const Int64 JHOURS = DATA[tj].d_hours;
                const Int64 JMINS  = DATA[tj].d_mins;
                const Int64 JSECS  = DATA[tj].d_secs;
                const Int64 JMSECS = DATA[tj].d_msecs;

                const Obj Z(JDAYS, JHOURS, JMINS, JSECS, JMSECS);

                Obj mU(W);  const Obj& U = mU;
                Obj mV(W);  const Obj& V = mV;

                switch (JNARGS) {
                  case 1: {
                    mU.setInterval(JDAYS);
                    mV.setInterval(JDAYS,      0,     0,     0,      0);
                  } break;
                  case 2: {
                    mU.setInterval(JDAYS, JHOURS);
                    mV.setInterval(JDAYS, JHOURS,     0,     0,      0);
                  } break;
                  case 3: {
                    mU.setInterval(JDAYS, JHOURS, JMINS);
                    mV.setInterval(JDAYS, JHOURS, JMINS,     0,      0);
                  } break;
                  case 4: {
                    mU.setInterval(JDAYS, JHOURS, JMINS, JSECS);
                    mV.setInterval(JDAYS, JHOURS, JMINS, JSECS,      0);
                  } break;
                  case 5: {
                    mU.setInterval(JDAYS, JHOURS, JMINS, JSECS, JMSECS);
                    mV.setInterval(JDAYS, JHOURS, JMINS, JSECS, JMSECS);
                  } break;
                  default: {
                    LOOP_ASSERT(JNARGS, !"Bad 'JNARGS' value.");
                  } break;
                }

                if (veryVerbose) { T_ P_(U) T_ P(V) }

                LOOP3_ASSERT(JLINE, Z, U, Z == U);
                LOOP3_ASSERT(JLINE, U, V, U == V);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.setInterval(0, k_HOURS_MIN - 1));
                ASSERT_SAFE_PASS(mX.setInterval(0, k_HOURS_MIN    ));

                ASSERT_SAFE_PASS(mX.setInterval(0, k_HOURS_MAX    ));
                ASSERT_SAFE_FAIL(mX.setInterval(0, k_HOURS_MAX + 1));

                ASSERT_SAFE_FAIL(mX.setInterval(0, 0, k_MINS_MIN - 1));
                ASSERT_SAFE_PASS(mX.setInterval(0, 0, k_MINS_MIN    ));

                ASSERT_SAFE_PASS(mX.setInterval(0, 0, k_MINS_MAX    ));
                ASSERT_SAFE_FAIL(mX.setInterval(0, 0, k_MINS_MAX + 1));

                ASSERT_SAFE_FAIL(mX.setInterval(0, 0, 0, k_SECS_MIN - 1));
                ASSERT_SAFE_PASS(mX.setInterval(0, 0, 0, k_SECS_MIN    ));

                ASSERT_SAFE_PASS(mX.setInterval(0, 0, 0, k_SECS_MAX    ));
                ASSERT_SAFE_FAIL(mX.setInterval(0, 0, 0, k_SECS_MAX + 1));

                ASSERT_SAFE_FAIL(mX.setInterval(0, 0, 0, 0, k_MSECS_MIN - 1));
                ASSERT_SAFE_PASS(mX.setInterval(0, 0, 0, 0, k_MSECS_MIN    ));

                ASSERT_SAFE_PASS(mX.setInterval(0, 0, 0, 0, k_MSECS_MAX    ));
                ASSERT_SAFE_FAIL(mX.setInterval(0, 0, 0, 0, k_MSECS_MAX + 1));
            }

            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.setInterval(k_DAYS_MIN, -24));
                ASSERT_SAFE_PASS(mX.setInterval(k_DAYS_MIN, -23));

                ASSERT_SAFE_PASS(mX.setInterval(k_DAYS_MAX,  23));
                ASSERT_SAFE_FAIL(mX.setInterval(k_DAYS_MAX,  24));

                ASSERT_SAFE_FAIL(mX.setInterval(k_DAYS_MIN, -23, -60));
                ASSERT_SAFE_PASS(mX.setInterval(k_DAYS_MIN, -23, -59));

                ASSERT_SAFE_PASS(mX.setInterval(k_DAYS_MAX,  23,  59));
                ASSERT_SAFE_FAIL(mX.setInterval(k_DAYS_MAX,  23,  60));

                ASSERT_SAFE_FAIL(mX.setInterval(k_DAYS_MIN, -23, -59, -60));
                ASSERT_SAFE_PASS(mX.setInterval(k_DAYS_MIN, -23, -59, -59));

                ASSERT_SAFE_PASS(mX.setInterval(k_DAYS_MAX,  23,  59,  59));
                ASSERT_SAFE_FAIL(mX.setInterval(k_DAYS_MAX,  23,  59,  60));

                ASSERT_SAFE_FAIL(mX.setInterval(k_DAYS_MIN, -23, -59, -59,
                                                                       -1000));
                ASSERT_SAFE_PASS(mX.setInterval(k_DAYS_MIN, -23, -59, -59,
                                                                        -999));

                ASSERT_SAFE_PASS(mX.setInterval(k_DAYS_MAX,  23,  59,  59,
                                                                         999));
                ASSERT_SAFE_FAIL(mX.setInterval(k_DAYS_MAX,  23,  59,  59,
                                                                        1000));
            }

            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.setInterval(0, k_HOURS_MIN, -60));
                ASSERT_SAFE_PASS(mX.setInterval(0, k_HOURS_MIN, -59));

                ASSERT_SAFE_PASS(mX.setInterval(0, k_HOURS_MAX,  59));
                ASSERT_SAFE_FAIL(mX.setInterval(0, k_HOURS_MAX,  60));

                ASSERT_SAFE_FAIL(mX.setInterval(0, k_HOURS_MIN, -59, -60));
                ASSERT_SAFE_PASS(mX.setInterval(0, k_HOURS_MIN, -59, -59));

                ASSERT_SAFE_PASS(mX.setInterval(0, k_HOURS_MAX,  59,  59));
                ASSERT_SAFE_FAIL(mX.setInterval(0, k_HOURS_MAX,  59,  60));

                ASSERT_SAFE_FAIL(mX.setInterval(0, k_HOURS_MIN, -59, -59,
                                                                       -1000));
                ASSERT_SAFE_PASS(mX.setInterval(0, k_HOURS_MIN, -59, -59,
                                                                        -999));

                ASSERT_SAFE_PASS(mX.setInterval(0, k_HOURS_MAX,  59,  59,
                                                                         999));
                ASSERT_SAFE_FAIL(mX.setInterval(0, k_HOURS_MAX,  59,  59,
                                                                        1000));
            }

            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.setInterval(0, 0, k_MINS_MIN, -60));
                ASSERT_SAFE_PASS(mX.setInterval(0, 0, k_MINS_MIN, -59));

                ASSERT_SAFE_PASS(mX.setInterval(0, 0, k_MINS_MAX,  59));
                ASSERT_SAFE_FAIL(mX.setInterval(0, 0, k_MINS_MAX,  60));

                ASSERT_SAFE_FAIL(mX.setInterval(0, 0, k_MINS_MIN, -59, -1000));
                ASSERT_SAFE_PASS(mX.setInterval(0, 0, k_MINS_MIN, -59,  -999));

                ASSERT_SAFE_PASS(mX.setInterval(0, 0, k_MINS_MAX,  59,   999));
                ASSERT_SAFE_FAIL(mX.setInterval(0, 0, k_MINS_MAX,  59,  1000));
            }

            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.setInterval(0, 0, 0, k_SECS_MIN, -1000));
                ASSERT_SAFE_PASS(mX.setInterval(0, 0, 0, k_SECS_MIN,  -999));

                ASSERT_SAFE_PASS(mX.setInterval(0, 0, 0, k_SECS_MAX,   999));
                ASSERT_SAFE_FAIL(mX.setInterval(0, 0, 0, k_SECS_MAX,  1000));
            }
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // VALUE CTOR
        //   Ensure that we can put an object into any valid initial state.
        //
        // Concerns:
        //: 1 The value constructor can create an object having any value that
        //:   does not violate the constructor's documented preconditions.
        //:
        //: 2 The optional 'hours', 'minutes', 'seconds', and 'milliseconds'
        //:   parameters each have the correct default value (0).
        //:
        //: 3 The value constructor accepts time intervals that are specified
        //:   using a mix of positive, negative, and zero values for the
        //:   'days', 'hours', 'minutes', 'seconds', and 'milliseconds' fields.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of distinct object values (one per row) in terms
        //:     of their five-field representation.
        //:
        //:   2 Additionally, provide a (five-valued) column, 'NARGS',
        //:     indicating the number of significant field values in each row
        //:     of the table, where 'NARGS' is in the range '[1 .. 5]'.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..3)
        //:
        //:   1 Use the value constructor to create an object, 'X', from the
        //:     value in 'R', supplying only 'NARGS' arguments to the
        //:     constructor (letting the remaining '5 - NARGS' arguments take
        //:     their default values).
        //:
        //:   2 Also use the value constructor to create a second object, 'Y',
        //:     from the value in 'R', but this time passing '5 - NARGS'
        //:     trailing 0 arguments explicitly.
        //:
        //:   3 Using the 'flds2Msecs' helper function, convert the five-field
        //:     representation in 'R' to its corresponding total-milliseconds
        //:     representation.
        //:
        //:   4 Use the 'totalMilliseconds' accessor to verify that 'X' has the
        //:     same time interval as computed in P-2.3.
        //:
        //:   5 Use the equality-comparison operator to verify that 'X' and 'Y'
        //:     have the same value.  (C-1..3)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid total-millisecond values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   DatetimeInterval(int d, Int64 h = 0, m = 0, s = 0, ms = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CTOR" << endl
                          << "==========" << endl;

        if (verbose) cout << "\nUse a table of distinct object values."
                          << endl;

        const int          NUM_DATA        = ALT_NUM_DATA;
        const AltDataRow (&DATA)[NUM_DATA] = ALT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_line;
            const int   NARGS = DATA[ti].d_nargs;
            const int   DAYS  = DATA[ti].d_days;
            const Int64 HOURS = DATA[ti].d_hours;
            const Int64 MINS  = DATA[ti].d_mins;
            const Int64 SECS  = DATA[ti].d_secs;
            const Int64 MSECS = DATA[ti].d_msecs;

            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);

            Obj *XPtr, *YPtr;

            switch (NARGS) {
              case 1: {
                XPtr = new (fa) Obj(DAYS);
                YPtr = new (fa) Obj(DAYS,     0,    0,    0,     0);
              } break;
              case 2: {
                XPtr = new (fa) Obj(DAYS, HOURS);
                YPtr = new (fa) Obj(DAYS, HOURS,    0,    0,     0);
              } break;
              case 3: {
                XPtr = new (fa) Obj(DAYS, HOURS, MINS);
                YPtr = new (fa) Obj(DAYS, HOURS, MINS,    0,     0);
              } break;
              case 4: {
                XPtr = new (fa) Obj(DAYS, HOURS, MINS, SECS);
                YPtr = new (fa) Obj(DAYS, HOURS, MINS, SECS,     0);
              } break;
              case 5: {
                XPtr = new (fa) Obj(DAYS, HOURS, MINS, SECS, MSECS);
                YPtr = new (fa) Obj(DAYS, HOURS, MINS, SECS, MSECS);
              } break;
              default: {
                LOOP_ASSERT(NARGS, !"Bad 'NARGS' value.");
              } break;
            }

            const Obj& X = *XPtr;
            const Obj& Y = *YPtr;

            if (veryVerbose) { T_ P_(X) T_ P(Y) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE, Obj(), X, Obj() == X)
                firstFlag = false;
            }

            const Int64 TOTAL_MSECS =
                                    flds2Msecs(DAYS, HOURS, MINS, SECS, MSECS);

            LOOP3_ASSERT(LINE, TOTAL_MSECS, X.totalMilliseconds(),
                         TOTAL_MSECS == X.totalMilliseconds());

            LOOP3_ASSERT(LINE, X, Y, X == Y);

            fa.deleteObject(XPtr);
            fa.deleteObject(YPtr);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            {
                ASSERT_SAFE_FAIL(Obj(0, k_HOURS_MIN - 1));
                ASSERT_SAFE_PASS(Obj(0, k_HOURS_MIN    ));

                ASSERT_SAFE_PASS(Obj(0, k_HOURS_MAX    ));
                ASSERT_SAFE_FAIL(Obj(0, k_HOURS_MAX + 1));

                ASSERT_SAFE_FAIL(Obj(0, 0, k_MINS_MIN - 1));
                ASSERT_SAFE_PASS(Obj(0, 0, k_MINS_MIN    ));

                ASSERT_SAFE_PASS(Obj(0, 0, k_MINS_MAX    ));
                ASSERT_SAFE_FAIL(Obj(0, 0, k_MINS_MAX + 1));

                ASSERT_SAFE_FAIL(Obj(0, 0, 0, k_SECS_MIN - 1));
                ASSERT_SAFE_PASS(Obj(0, 0, 0, k_SECS_MIN    ));

                ASSERT_SAFE_PASS(Obj(0, 0, 0, k_SECS_MAX    ));
                ASSERT_SAFE_FAIL(Obj(0, 0, 0, k_SECS_MAX + 1));

                ASSERT_SAFE_FAIL(Obj(0, 0, 0, 0, k_MSECS_MIN - 1));
                ASSERT_SAFE_PASS(Obj(0, 0, 0, 0, k_MSECS_MIN    ));

                ASSERT_SAFE_PASS(Obj(0, 0, 0, 0, k_MSECS_MAX    ));
                ASSERT_SAFE_FAIL(Obj(0, 0, 0, 0, k_MSECS_MAX + 1));
            }

            {
                ASSERT_SAFE_FAIL(Obj(k_DAYS_MIN, -24));
                ASSERT_SAFE_PASS(Obj(k_DAYS_MIN, -23));

                ASSERT_SAFE_PASS(Obj(k_DAYS_MAX,  23));
                ASSERT_SAFE_FAIL(Obj(k_DAYS_MAX,  24));

                ASSERT_SAFE_FAIL(Obj(k_DAYS_MIN, -23, -60));
                ASSERT_SAFE_PASS(Obj(k_DAYS_MIN, -23, -59));

                ASSERT_SAFE_PASS(Obj(k_DAYS_MAX,  23,  59));
                ASSERT_SAFE_FAIL(Obj(k_DAYS_MAX,  23,  60));

                ASSERT_SAFE_FAIL(Obj(k_DAYS_MIN, -23, -59, -60));
                ASSERT_SAFE_PASS(Obj(k_DAYS_MIN, -23, -59, -59));

                ASSERT_SAFE_PASS(Obj(k_DAYS_MAX,  23,  59,  59));
                ASSERT_SAFE_FAIL(Obj(k_DAYS_MAX,  23,  59,  60));

                ASSERT_SAFE_FAIL(Obj(k_DAYS_MIN, -23, -59, -59, -1000));
                ASSERT_SAFE_PASS(Obj(k_DAYS_MIN, -23, -59, -59,  -999));

                ASSERT_SAFE_PASS(Obj(k_DAYS_MAX,  23,  59,  59,   999));
                ASSERT_SAFE_FAIL(Obj(k_DAYS_MAX,  23,  59,  59,  1000));
            }

            {
                ASSERT_SAFE_FAIL(Obj(0, k_HOURS_MIN, -60));
                ASSERT_SAFE_PASS(Obj(0, k_HOURS_MIN, -59));

                ASSERT_SAFE_PASS(Obj(0, k_HOURS_MAX,  59));
                ASSERT_SAFE_FAIL(Obj(0, k_HOURS_MAX,  60));

                ASSERT_SAFE_FAIL(Obj(0, k_HOURS_MIN, -59, -60));
                ASSERT_SAFE_PASS(Obj(0, k_HOURS_MIN, -59, -59));

                ASSERT_SAFE_PASS(Obj(0, k_HOURS_MAX,  59,  59));
                ASSERT_SAFE_FAIL(Obj(0, k_HOURS_MAX,  59,  60));

                ASSERT_SAFE_FAIL(Obj(0, k_HOURS_MIN, -59, -59, -1000));
                ASSERT_SAFE_PASS(Obj(0, k_HOURS_MIN, -59, -59,  -999));

                ASSERT_SAFE_PASS(Obj(0, k_HOURS_MAX,  59,  59,   999));
                ASSERT_SAFE_FAIL(Obj(0, k_HOURS_MAX,  59,  59,  1000));
            }

            {
                ASSERT_SAFE_FAIL(Obj(0, 0, k_MINS_MIN, -60));
                ASSERT_SAFE_PASS(Obj(0, 0, k_MINS_MIN, -59));

                ASSERT_SAFE_PASS(Obj(0, 0, k_MINS_MAX,  59));
                ASSERT_SAFE_FAIL(Obj(0, 0, k_MINS_MAX,  60));

                ASSERT_SAFE_FAIL(Obj(0, 0, k_MINS_MIN, -59, -1000));
                ASSERT_SAFE_PASS(Obj(0, 0, k_MINS_MIN, -59,  -999));

                ASSERT_SAFE_PASS(Obj(0, 0, k_MINS_MAX,  59,   999));
                ASSERT_SAFE_FAIL(Obj(0, 0, k_MINS_MAX,  59,  1000));
            }

            {
                ASSERT_SAFE_FAIL(Obj(0, 0, 0, k_SECS_MIN, -1000));
                ASSERT_SAFE_PASS(Obj(0, 0, 0, k_SECS_MIN,  -999));

                ASSERT_SAFE_PASS(Obj(0, 0, 0, k_SECS_MAX,   999));
                ASSERT_SAFE_FAIL(Obj(0, 0, 0, k_SECS_MAX,  1000));
            }
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING BDEX STREAMING
        //   Verify the BDEX streaming implementation works correctly.
        //   Specific concerns include wire format, handling of stream states
        //   (valid, empty, invalid, incomplete, and corrupted), and exception
        //   neutrality.
        //
        // Concerns:
        //: 1 The class method 'maxSupportedBdexVersion' returns the correct
        //:   version to be used for the specified 'versionSelector'.
        //:
        //: 2 The 'bdexStreamOut' method is callable on a reference providing
        //:   only non-modifiable access.
        //:
        //: 3 For valid streams, externalization and unexternalization are
        //:   inverse operations.
        //:
        //: 4 For invalid streams, externalization leaves the stream invalid
        //:   and unexternalization does not alter the value of the object and
        //:   leaves the stream invalid.
        //:
        //: 5 Unexternalizing of incomplete, invalid, or corrupted data results
        //:   in a valid object of unspecified value and an invalidated stream.
        //:
        //: 6 The wire format of the object is as expected.
        //:
        //: 7 All methods are exception neutral.
        //:
        //: 8 The 'bdexStreamIn' and 'bdexStreamOut' methods return a reference
        //:   to the provided stream in all situations.
        //:
        //: 9 The initial value of the object has no affect on
        //:   unexternalization.
        //
        // Plan:
        //: 1 Test 'maxSupportedBdexVersion' explicitly.  (C-1)
        //:
        //: 2 All calls to the 'bdexStreamOut' accessor will be done from a
        //:   'const' object or reference and all calls to the 'bdexStreamOut'
        //:   free function (provided by 'bslx') will be supplied a 'const'
        //:   object or reference.  (C-2)
        //:
        //: 3 Perform a direct test of the 'bdexStreamOut' and 'bdexStreamIn'
        //:   methods (the rest of the testing will use the free functions
        //:   'bslx::OutStreamFunctions::bdexStreamOut' and
        //:   'bslx::InStreamFunctions::bdexStreamIn').
        //:
        //: 4 Define a set 'S' of test values to be used throughout the test
        //:   case.
        //:
        //: 5 For all '(u, v)' in the cross product 'S X S', stream the value
        //:   of 'u' into (a temporary copy of) 'v', 'T', and assert 'T == u'.
        //:   (C-3, 9)
        //:
        //: 6 For all 'u' in 'S', create a copy of 'u' and attempt to stream
        //:   into it from an invalid stream.  Verify after each attempt that
        //:   the object is unchanged and that the stream is invalid.  (C-4)
        //:
        //: 7 Write 3 distinct objects to an output stream buffer of total
        //:   length 'N'.  For each partial stream length from 0 to 'N - 1',
        //:   construct an input stream and attempt to read into objects
        //:   initialized with distinct values.  Verify values of objects
        //:   that are either successfully modified or left entirely
        //:   unmodified, and that the stream became invalid immediately after
        //:   the first incomplete read.  Finally, ensure that each object
        //:   streamed into is in some valid state.
        //:
        //: 8 Use the underlying stream package to simulate a typical valid
        //:   (control) stream and verify that it can be streamed in
        //:   successfully.  Then for each data field in the stream (beginning
        //:   with the version number), provide one or more similar tests with
        //:   that data field corrupted.  After each test, verify that the
        //:   object is in some valid state after streaming, and that the
        //:   input stream has become invalid.  (C-5)
        //:
        //: 9 Explicitly test the wire format.  (C-6)
        //:
        //:10 In all cases, confirm exception neutrality using the specially
        //:   instrumented 'bslx::TestInStream' and a pair of standard macros,
        //:   'BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN' and
        //:   'BSLX_TESTINSTREAM_EXCEPTION_TEST_END', which configure the
        //:   'bslx::TestInStream' object appropriately in a loop.  (C-7)
        //:
        //:11 In all cases, verify the return value of the tested method.
        //:   (C-8)
        //
        // Testing:
        //   static int maxSupportedBdexVersion(int versionSelector);
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // --------------------------------------------------------------------

        // Allocator to use instead of the default allocator.
        bslma::TestAllocator allocator("bslx", veryVeryVeryVerbose);

        if (verbose) cout << endl
                          << "TESTING BDEX STREAMING" << endl
                          << "======================" << endl;

        // Scalar object values used in various stream tests.
        const Obj VA(0);
        const Obj VB(1);
        const Obj VC(2);
        const Obj VD(3);
        const Obj VE(4);
        const Obj VF(5);
        const Obj VG(6);

        // Array object used in various stream tests.
        const Obj VALUES[]   = { VA, VB, VC, VD, VE, VF, VG };
        const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                / sizeof *VALUES);

        if (verbose) {
            cout << "\nTesting 'maxSupportedBdexVersion'." << endl;
        }
        {
            ASSERT(1 == Obj::maxSupportedBdexVersion(0));
            ASSERT(1 == Obj::maxSupportedBdexVersion(VERSION_SELECTOR));

            using bslx::VersionFunctions::maxSupportedBdexVersion;

            ASSERT(1 == maxSupportedBdexVersion(reinterpret_cast<Obj *>(0),
                                                0));
            ASSERT(1 == maxSupportedBdexVersion(reinterpret_cast<Obj *>(0),
                                                VERSION_SELECTOR));
        }

        const int VERSION = Obj::maxSupportedBdexVersion(0);

        if (verbose) {
            cout << "\nDirect initial trial of 'bdexStreamOut' and (valid) "
                 << "'bdexStreamIn' functionality." << endl;
        }
        {
            const Obj X(VC);
            Out       out(VERSION_SELECTOR, &allocator);

            Out& rvOut = X.bdexStreamOut(out, VERSION);
            ASSERT(&out == &rvOut);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            In in(OD, LOD);
            ASSERT(in);
            ASSERT(!in.isEmpty());

            Obj mT(VA);  const Obj& T = mT;
            ASSERT(X != T);

            In& rvIn = mT.bdexStreamIn(in, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(X == T);
            ASSERT(in);
            ASSERT(in.isEmpty());
        }

        // We will use the stream free functions provided by 'bslx', as opposed
        // to the class member functions, since the 'bslx' implementation gives
        // priority to the free function implementations; we want to test what
        // will be used.  Furthermore, toward making this test case more
        // reusable in other components, from here on we generally use the
        // 'bdexStreamIn' and 'bdexStreamOut' free functions that are defined
        // in the 'bslx' package rather than call the like-named member
        // functions directly.

        if (verbose) {
            cout << "\nThorough test using stream free functions."
                 << endl;
        }
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);

                Out out(VERSION_SELECTOR, &allocator);

                using bslx::OutStreamFunctions::bdexStreamOut;
                using bslx::InStreamFunctions::bdexStreamIn;

                Out& rvOut = bdexStreamOut(out, X, VERSION);
                LOOP_ASSERT(i, &out == &rvOut);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value and
                // that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    In in(OD, LOD);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());

                    Obj mT(VALUES[j]);  const Obj& T = mT;
                    LOOP2_ASSERT(i, j, (X == T) == (i == j));

                    BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                        in.reset();
                        In& rvIn = bdexStreamIn(in, mT, VERSION);
                        LOOP2_ASSERT(i, j, &in == &rvIn);
                    } BSLX_TESTINSTREAM_EXCEPTION_TEST_END

                    LOOP2_ASSERT(i, j, X == T);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        if (verbose) {
            cout << "\tOn empty streams and non-empty, invalid streams."
                 << endl;
        }

        // Verify correct behavior for empty streams (valid and invalid).

        {
            Out               out(VERSION_SELECTOR, &allocator);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                using bslx::InStreamFunctions::bdexStreamIn;

                const Obj  X(VALUES[i]);
                Obj        mT(X);
                const Obj& T = mT;
                LOOP_ASSERT(i, X == T);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();

                    // Stream is valid.
                    In& rvIn1 = bdexStreamIn(in, mT, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn1);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);

                    // Stream is invalid.
                    In& rvIn2 = bdexStreamIn(in, mT, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn2);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        // Verify correct behavior for non-empty, invalid streams.

        {
            Out               out(VERSION_SELECTOR, &allocator);

            using bslx::OutStreamFunctions::bdexStreamOut;
            Out& rvOut = bdexStreamOut(out, Obj(), VERSION);
            ASSERT(&out == &rvOut);

            const char *const OD  = out.data();
            const int         LOD = out.length();
            ASSERT(0 < LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);
                in.invalidate();
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, !in.isEmpty());

                // Ensure that reading from a non-empty, invalid input stream
                // leaves the stream invalid and the target object unchanged.

                using bslx::InStreamFunctions::bdexStreamIn;

                const Obj  X(VALUES[i]);
                Obj        mT(X);
                const Obj& T = mT;
                LOOP_ASSERT(i, X == T);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();
                    in.invalidate();
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, !in.isEmpty());

                    In& rvIn = bdexStreamIn(in, mT, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        if (verbose) {
            cout << "\tOn incomplete (but otherwise valid) data."
                 << endl;
        }
        {
            const Obj W1 = VA, X1 = VB;
            const Obj W2 = VB, X2 = VC;
            const Obj W3 = VC, X3 = VD;

            using bslx::OutStreamFunctions::bdexStreamOut;
            using bslx::InStreamFunctions::bdexStreamIn;

            Out out(VERSION_SELECTOR, &allocator);

            Out& rvOut1 = bdexStreamOut(out, X1, VERSION);
            ASSERT(&out == &rvOut1);
            const int         LOD1 = out.length();

            Out& rvOut2 = bdexStreamOut(out, X2, VERSION);
            ASSERT(&out == &rvOut2);
            const int         LOD2 = out.length();

            Out& rvOut3 = bdexStreamOut(out, X3, VERSION);
            ASSERT(&out == &rvOut3);
            const int         LOD3 = out.length();
            const char *const OD3  = out.data();

            for (int i = 0; i < LOD3; ++i) {
                In in(OD3, i);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();
                    LOOP_ASSERT(i, in);
                    LOOP_ASSERT(i, !i == in.isEmpty());

                    Obj mT1(W1);  const Obj& T1 = mT1;
                    Obj mT2(W2);  const Obj& T2 = mT2;
                    Obj mT3(W3);  const Obj& T3 = mT3;

                    if (i < LOD1) {
                        In& rvIn1 = bdexStreamIn(in, mT1, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn1);
                        LOOP_ASSERT(i, !in);
                        if (0 == i) LOOP_ASSERT(i, W1 == T1);
                        In& rvIn2 = bdexStreamIn(in, mT2, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn2);
                        LOOP_ASSERT(i, !in);
                        LOOP_ASSERT(i, W2 == T2);
                        In& rvIn3 = bdexStreamIn(in, mT3, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn3);
                        LOOP_ASSERT(i, !in);
                        LOOP_ASSERT(i, W3 == T3);
                    }
                    else if (i < LOD2) {
                        In& rvIn1 = bdexStreamIn(in, mT1, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn1);
                        LOOP_ASSERT(i,  in);
                        LOOP_ASSERT(i, X1 == T1);
                        In& rvIn2 = bdexStreamIn(in, mT2, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn2);
                        LOOP_ASSERT(i, !in);
                        if (LOD1 <= i) LOOP_ASSERT(i, W2 == T2);
                        In& rvIn3 = bdexStreamIn(in, mT3, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn3);
                        LOOP_ASSERT(i, !in);
                        LOOP_ASSERT(i, W3 == T3);
                    }
                    else {  // 'LOD2 <= i < LOD3'
                        In& rvIn1 = bdexStreamIn(in, mT1, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn1);
                        LOOP_ASSERT(i,  in);
                        LOOP_ASSERT(i, X1 == T1);
                        In& rvIn2 = bdexStreamIn(in, mT2, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn2);
                        LOOP_ASSERT(i,  in);
                        LOOP_ASSERT(i, X2 == T2);
                        In& rvIn3 = bdexStreamIn(in, mT3, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn3);
                        LOOP_ASSERT(i, !in);
                        if (LOD2 <= i) LOOP_ASSERT(i, W3 == T3);
                    }

                    // Verify the objects are in a valid state.

                    LOOP_ASSERT(i, (   k_MSECS_MIN <= T1.totalMilliseconds()
                                    && k_MSECS_MAX >= T1.totalMilliseconds()));

                    LOOP_ASSERT(i, (   k_MSECS_MIN <= T2.totalMilliseconds()
                                    && k_MSECS_MAX >= T2.totalMilliseconds()));

                    LOOP_ASSERT(i, (   k_MSECS_MIN <= T3.totalMilliseconds()
                                    && k_MSECS_MAX >= T3.totalMilliseconds()));

                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        if (verbose) {
            cout << "\tOn corrupted data." << endl;
        }

        const Obj W;                      // default value
        const Obj X(0, 0, 0, 0, 1);       // original (control)
        const Obj Y(0, 0, 0, 0, 2);       // new (streamed-out)

        // Verify the three objects are distinct.
        ASSERT(W != X);
        ASSERT(W != Y);
        ASSERT(X != Y);

        const int SERIAL_Y = 2;       // internal rep. of 'Y'

        if (verbose) {
            cout << "\t\tGood stream (for control)." << endl;
        }
        {
            Out out(VERSION_SELECTOR, &allocator);
            out.putInt64(SERIAL_Y);  // Stream out "new" value.
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(in);
            ASSERT(Y == T);
        }

        if (verbose) {
            cout << "\t\tBad version." << endl;
        }
        {
            const char version = 0; // too small ('version' must be >= 1)

            Out out(VERSION_SELECTOR, &allocator);
            out.putInt64(SERIAL_Y);  // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, version);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }
        {
            const char version = 2 ; // too large (current version is 1)

            Out out(VERSION_SELECTOR, &allocator);
            out.putInt64(SERIAL_Y);  // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, version);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
            cout << "\t\tValue too small." << endl;
        }
        {
            Out out(VERSION_SELECTOR, &allocator);
            out.putInt64(k_MSECS_MIN - 1);  // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
            cout << "\t\tValue too large." << endl;
        }
        {
            Out out(VERSION_SELECTOR, &allocator);
            out.putInt64(k_MSECS_MAX + 1);  // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
             cout << "\nWire format direct tests." << endl;
        }
        {
            static const struct {
                int         d_lineNum;       // source line number
                int         d_milliseconds;  // specification milliseconds
                int         d_version;       // version to stream with
                int         d_length;        // expect output length
                const char *d_fmt_p;         // expected output format
            } DATA[] = {
                //LINE  MS      VER  LEN  FORMAT
                //----  ------  ---  ---  -------------------
                { L_,        0,   1,   8, "\x00\x00\x00\x00\x00\x00\x00\x00" },
                { L_,        1,   1,   8, "\x00\x00\x00\x00\x00\x00\x00\x01" },
                { L_,      256,   1,   8, "\x00\x00\x00\x00\x00\x00\x01\x00" },
                { L_,    65536,   1,   8, "\x00\x00\x00\x00\x00\x01\x00\x00" }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE         = DATA[i].d_lineNum;
                const int         MILLISECONDS = DATA[i].d_milliseconds;
                const int         VERSION      = DATA[i].d_version;
                const int         LEN          = DATA[i].d_length;
                const char *const FMT          = DATA[i].d_fmt_p;

                // Test using class methods.

                {
                    Obj        mX(0, 0, 0, 0, MILLISECONDS);
                    const Obj& X = mX;

                    bslx::ByteOutStream  out(VERSION_SELECTOR, &allocator);
                    bslx::ByteOutStream& rvOut = X.bdexStreamOut(out, VERSION);
                    LOOP_ASSERT(LINE, &out == &rvOut);

                    LOOP_ASSERT(LINE, LEN == out.length());
                    LOOP_ASSERT(LINE, 0 == memcmp(out.data(), FMT, LEN));

                    if (verbose && memcmp(out.data(), FMT, LEN)) {
                        const char *hex = "0123456789abcdef";
                        P_(LINE);
                        for (int j = 0; j < out.length(); ++j) {
                            cout << "\\x"
                                 << hex[static_cast<unsigned char>
                                            ((*(out.data() + j) >> 4) & 0x0f)]
                                 << hex[static_cast<unsigned char>
                                                   (*(out.data() + j) & 0x0f)];
                        }
                        cout << endl;
                    }

                    Obj mY;  const Obj& Y = mY;

                    bslx::ByteInStream  in(out.data(), out.length());
                    bslx::ByteInStream& rvIn = mY.bdexStreamIn(in, VERSION);
                    LOOP_ASSERT(LINE, &in == &rvIn);
                    LOOP_ASSERT(LINE, X == Y);
                }

                // Test using free functions.

                {
                    Obj        mX(0, 0, 0, 0, MILLISECONDS);
                    const Obj& X = mX;

                    using bslx::OutStreamFunctions::bdexStreamOut;

                    bslx::ByteOutStream  out(VERSION_SELECTOR, &allocator);
                    bslx::ByteOutStream& rvOut = bdexStreamOut(out,
                                                               X,
                                                               VERSION);
                    LOOP_ASSERT(LINE, &out == &rvOut);

                    LOOP_ASSERT(LINE, LEN == out.length());
                    LOOP_ASSERT(LINE, 0 == memcmp(out.data(), FMT, LEN));

                    if (verbose && memcmp(out.data(), FMT, LEN)) {
                        const char *hex = "0123456789abcdef";
                        P_(LINE);
                        for (int j = 0; j < out.length(); ++j) {
                            cout << "\\x"
                                 << hex[static_cast<unsigned char>
                                            ((*(out.data() + j) >> 4) & 0x0f)]
                                 << hex[static_cast<unsigned char>
                                                   (*(out.data() + j) & 0x0f)];
                        }
                        cout << endl;
                    }

                    Obj mY;  const Obj& Y = mY;

                    using bslx::InStreamFunctions::bdexStreamIn;

                    bslx::ByteInStream  in(out.data(), out.length());
                    bslx::ByteInStream& rvIn = bdexStreamIn(in, mY, VERSION);
                    LOOP_ASSERT(LINE, &in == &rvIn);
                    LOOP_ASSERT(LINE, X == Y);
                }
            }
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The signature and return type are standard.
        //:
        //: 3 The reference returned is to the target object (i.e., '*this').
        //:
        //: 4 The value of the source object is not modified.
        //:
        //: 5 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-2)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their total-milliseconds
        //:   representation.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1, 3..4)
        //:
        //:   1 Use the default constructor and 'setTotalMilliseconds' to
        //:     create two 'const' 'Obj', 'Z' and 'ZZ', having the value from
        //:     'R1'.
        //:
        //:   2 For each row 'R2' in the table of P-2:  (C-1, 3..4)
        //:
        //:     1 Use the default constructor and 'setTotalMilliseconds' to
        //:       create a modifiable 'Obj', 'mX', having the value from 'R2'.
        //:
        //:     2 Assign 'mX' from 'Z'.
        //:
        //:     3 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-3)
        //:
        //:     4 Use the equality-comparison operator to verify that: (C-1, 4)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-4)
        //:
        //: 4 Repeat steps similar to those described in P-3 except that, this
        //:   time, there is no inner loop (as in P-3.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are created to have the value from 'R1'.  For each
        //:   'R1' in the table of P-2:  (C-5)
        //:
        //:   1 Use the default constructor and 'setTotalMilliseconds' to
        //:     create a modifiable 'Obj', 'mX', having the value from 'R1';
        //:     also use the default constructor and 'setTotalMilliseconds' to
        //:     create a 'const' 'Obj', 'ZZ', also having the value from 'R1'.
        //:
        //:   2 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   3 Assign 'mX' from 'Z'.
        //:
        //:   4 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   5 Use the equality-comparison operator to verify that the
        //:     target object, 'Z' ('mX'), still has the same value as that of
        //:     'ZZ'.  (C-5)
        //
        // Testing:
        //   DatetimeInterval& operator=(const DatetimeInterval& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY-ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        static const struct {
            int   d_line;        // source line number
            Int64 d_totalMsecs;
        } DATA[] = {
            //LINE   TOTAL MILLISECONDS
            //----   ------------------
            { L_,                     0 },

            { L_,                     1 },
            { L_,                 13027 },
            { L_,               INT_MAX },
            { L_,           k_MSECS_MAX },

            { L_,                    -1 },
            { L_,                -42058 },
            { L_,               INT_MIN },
            { L_,           k_MSECS_MIN },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   ILINE        = DATA[ti].d_line;
            const Int64 ITOTAL_MSECS = DATA[ti].d_totalMsecs;

            Obj mZ;   const Obj& Z  = mZ;
            mZ. setTotalMilliseconds(ITOTAL_MSECS);

            Obj mZZ;  const Obj& ZZ = mZZ;
            mZZ.setTotalMilliseconds(ITOTAL_MSECS);

            if (veryVerbose) { T_ P_(ILINE) P(Z) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(ILINE, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int   JLINE        = DATA[tj].d_line;
                const Int64 JTOTAL_MSECS = DATA[tj].d_totalMsecs;

                Obj mX;  const Obj& X = mX;
                mX.setTotalMilliseconds(JTOTAL_MSECS);

                if (veryVerbose) { T_ T_ P_(JLINE) P(X) }

                LOOP4_ASSERT(ILINE, JLINE, Z, X,
                             (Z == X) == (ILINE == JLINE));

                Obj *mR = &(mX = Z);

                LOOP4_ASSERT(ILINE, JLINE,  Z,   X,  Z == X);
                LOOP4_ASSERT(ILINE, JLINE, mR, &mX, mR == &mX);
                LOOP4_ASSERT(ILINE, JLINE, ZZ,   Z, ZZ == Z);
            }

            // self-assignment

            {
                Obj mX;
                mX. setTotalMilliseconds(ITOTAL_MSECS);

                Obj mZZ;  const Obj& ZZ = mZZ;
                mZZ.setTotalMilliseconds(ITOTAL_MSECS);

                const Obj& Z = mX;

                LOOP3_ASSERT(ILINE, ZZ,   Z, ZZ == Z);

                Obj *mR = &(mX = Z);

                LOOP3_ASSERT(ILINE, mR, &mX, mR == &mX);
                LOOP3_ASSERT(ILINE, ZZ,   Z, ZZ == Z);
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   Ensure that, when member and free 'swap' are implemented, we can
        //   exchange the values of any two objects.
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //  Reserved for 'swap' testing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTIONS" << endl
                          << "==============================" << endl;

        if (verbose) cout << "Not implemented for 'bdlt::DatetimeInterval'."
                                                                       << endl;

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor creates an object having the same value as
        //:   that of the supplied original object.
        //:
        //: 2 The original object is passed as a reference providing
        //:   non-modifiable access to that object.
        //:
        //: 3 The value of the original object is unchanged.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their total-milliseconds
        //:   representation.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..3)
        //:
        //:   1 Use the default constructor to create two objects, 'Z' and
        //:     'ZZ', then set both 'Z' and 'ZZ' to the value from 'R' (using
        //:     'setTotalMilliseconds').
        //:
        //:   2 Use the copy constructor to create an object 'X', supplying it
        //:     with a reference providing non-modifiable access to 'Z'.  (C-2)
        //:
        //:   3 Use the equality-comparison operator to verify that:  (C-1, 3)
        //:
        //:     1 'X' has the same value as that of 'Z'.  (C-1)
        //:
        //:     2 'Z' still has the same value as that of 'ZZ'.  (C-3)
        //
        // Testing:
        //   DatetimeInterval(const DatetimeInterval& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        static const struct {
            int   d_line;        // source line number
            Int64 d_totalMsecs;
        } DATA[] = {
            //LINE   TOTAL MILLISECONDS
            //----   ------------------
            { L_,                     0 },

            { L_,                     1 },
            { L_,                 13027 },
            { L_,               INT_MAX },
            { L_,           k_MSECS_MAX },

            { L_,                    -1 },
            { L_,                -42058 },
            { L_,               INT_MIN },
            { L_,           k_MSECS_MIN },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE        = DATA[ti].d_line;
            const Int64 TOTAL_MSECS = DATA[ti].d_totalMsecs;

            Obj mZ;   const Obj& Z  = mZ;
            mZ. setTotalMilliseconds(TOTAL_MSECS);

            Obj mZZ;  const Obj& ZZ = mZZ;
            mZZ.setTotalMilliseconds(TOTAL_MSECS);

            if (veryVerbose) { T_ P_(Z) P(ZZ) }

            const Obj X(Z);

            if (veryVerbose) { T_ T_ P(X) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE, Obj(), X, Obj() == X)
                firstFlag = false;
            }

            // Verify the value of the object.

            LOOP3_ASSERT(LINE, Z,  X,  Z == X);

            // Verify that the value of 'Z' has not changed.

            LOOP3_ASSERT(LINE, ZZ, Z, ZZ == Z);
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if their
        //:   corresponding total-milliseconds representations compare equal.
        //:
        //: 2 'true  == (X == X)' (i.e., identity).
        //:
        //: 3 'false == (X != X)' (i.e., identity).
        //:
        //: 4 'X == Y' if and only if 'Y == X' (i.e., commutativity).
        //:
        //: 5 'X != Y' if and only if 'Y != X' (i.e., commutativity).
        //:
        //: 6 'X != Y' if and only if '!(X == Y)'.
        //:
        //: 7 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 8 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 9 The equality-comparison operators' signatures and return types
        //:   are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.  (C-7..9)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their total-milliseconds
        //:   representation.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1..6)
        //:
        //:   1 Create an object 'W' using the default constructor, then set
        //:     'W' to the value from 'R1' (using 'setTotalMilliseconds').
        //:
        //:   2 Using 'W', verify the reflexive (anti-reflexive) property of
        //:     equality (inequality) in the presence of aliasing.  (C-2..3)
        //:
        //:   3 For each row 'R2' in the table of P-2:  (C-1, 4..6)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects set to
        //:       values from 'R1' and 'R2', respectively, are expected to have
        //:       the same value.
        //:
        //:     2 Create an object 'X' using the default constructor, then set
        //:       'X' to the value from 'R1' (using 'setTotalMilliseconds').
        //:
        //:     3 Create an object 'Y' using the default constructor, then set
        //:       'Y' to the value from 'R2'.
        //:
        //:     4 Using 'X' and 'Y', verify the commutativity property and
        //:       expected return value for both '==' and '!='.  (C-1, 4..6)
        //
        // Testing:
        //   bool operator==(const DatetimeInterval& lhs, rhs);
        //   bool operator!=(const DatetimeInterval& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = bdlt::operator==;
            operatorPtr operatorNe = bdlt::operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        static const struct {
            int   d_line;        // source line number
            Int64 d_totalMsecs;
        } DATA[] = {
            //LINE   TOTAL MILLISECONDS
            //----   ------------------
            { L_,                     0 },

            { L_,                     1 },
            { L_,                 13027 },
            { L_,               INT_MAX },
            { L_,           k_MSECS_MAX },

            { L_,                    -1 },
            { L_,                -42058 },
            { L_,               INT_MIN },
            { L_,           k_MSECS_MIN },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   ILINE        = DATA[ti].d_line;
            const Int64 ITOTAL_MSECS = DATA[ti].d_totalMsecs;

            if (veryVerbose) { T_ P_(ILINE) P(ITOTAL_MSECS) }

            // Ensure an object compares correctly with itself (alias test).
            {
                Obj mW;  const Obj& W = mW;
                mW.setTotalMilliseconds(ITOTAL_MSECS);

                LOOP2_ASSERT(ILINE, W,   W == W);
                LOOP2_ASSERT(ILINE, W, !(W != W));

                // Ensure the first row of the table contains the
                // default-constructed value.

                static bool firstFlag = true;
                if (firstFlag) {
                    LOOP3_ASSERT(ILINE, Obj(), W, Obj() == W)
                    firstFlag = false;
                }
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int   JLINE        = DATA[tj].d_line;
                const Int64 JTOTAL_MSECS = DATA[tj].d_totalMsecs;

                if (veryVerbose) { T_ T_ P_(JLINE) P(JTOTAL_MSECS) }

                const bool EXP = ti == tj;  // expected for equality comparison

                Obj mX;  const Obj& X = mX;
                mX.setTotalMilliseconds(ITOTAL_MSECS);

                Obj mY;  const Obj& Y = mY;
                mY.setTotalMilliseconds(JTOTAL_MSECS);

                if (veryVerbose) { T_ T_ T_ P_(EXP) P_(X) P(Y) }

                // Verify expected against total-milliseconds comparison.

                LOOP4_ASSERT(ILINE, JLINE, X, Y,
                      EXP == (X.totalMilliseconds() == Y.totalMilliseconds()));
                LOOP4_ASSERT(ILINE, JLINE, X, Y,
                     !EXP == (X.totalMilliseconds() != Y.totalMilliseconds()));

                // Verify value and commutativity.

                LOOP4_ASSERT(ILINE, JLINE, X, Y,  EXP == (X == Y));
                LOOP4_ASSERT(ILINE, JLINE, Y, X,  EXP == (Y == X));

                LOOP4_ASSERT(ILINE, JLINE, X, Y, !EXP == (X != Y));
                LOOP4_ASSERT(ILINE, JLINE, Y, X, !EXP == (Y != X));
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR (<<)
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)'.
        //:
        //: 4 The 'print' method's signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values (0 and 4, respectively).
        //:
        //: 7 The output 'operator<<'s signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the destination 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 7)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 5..6, 8)
        //:
        //:   1 Define fourteen carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding time interval field, and various values for the
        //:     two formatting parameters, along with the expected output.
        //:
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1, -8 } --> 3 expected o/ps
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2, -8 } --> 6 expected o/ps
        //:     3 { B   } x {  2     } x {  3            } --> 1 expected o/p
        //:     4 { A B } x { -8     } x { -8            } --> 2 expected o/ps
        //:     5 { A B } x { -9     } x { -9            } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5..6, 8)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, '(-9, -9)', then
        //:       invoke the 'operator<<' instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5, 8)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3, 6)
        //:
        //: 3 Using the table-driven technique, further corroborate that the
        //:   'print' method and 'operator<<' format object values correctly by
        //:   testing an additional set of time intervals (including extremal
        //:   values), but this time fixing the 'level' and 'spacesPerLevel'
        //:   arguments to 0 and -1, respectively.
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   ostream& operator<<(ostream &os, const DatetimeInterval& object);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR (<<)" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOut = bdlt::operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOut;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        static const struct {
            int         d_line;            // source line number
            int         d_level;
            int         d_spacesPerLevel;

            int         d_days;
            Int64       d_hours;
            Int64       d_mins;
            Int64       d_secs;
            Int64       d_msecs;

            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 } -->  4 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL   D   H   M   S   MS  EXP
        //---- - ---   -  --  --  --  ---  ---

        { L_,  0,  0,  1, 23, 59, 59, 999, "+1_23:59:59.999"              NL },

        { L_,  0,  1,  1, 23, 59, 59, 999, "+1_23:59:59.999"              NL },

        { L_,  0, -1,  1, 23, 59, 59, 999, "+1_23:59:59.999"                 },

        { L_,  0, -8,  1, 23, 59, 59, 999, "+1_23:59:59.999"              NL },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } -->  6 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL   D   H   M   S   MS  EXP
        //---- - ---   -  --  --  --  ---  ---

        { L_,  3,  0,  1, 23, 59, 59, 999, "+1_23:59:59.999"              NL },

        { L_,  3,  2,  1, 23, 59, 59, 999, "      +1_23:59:59.999"        NL },

        { L_,  3, -2,  1, 23, 59, 59, 999, "      +1_23:59:59.999"           },

        { L_,  3, -8,  1, 23, 59, 59, 999,
                                    "            +1_23:59:59.999"         NL },

        { L_, -3,  0,  1, 23, 59, 59, 999, "+1_23:59:59.999"              NL },

        { L_, -3,  2,  1, 23, 59, 59, 999, "+1_23:59:59.999"              NL },

        { L_, -3, -2,  1, 23, 59, 59, 999, "+1_23:59:59.999"                 },

        { L_, -3, -8,  1, 23, 59, 59, 999, "+1_23:59:59.999"              NL },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }            -->  1 expected o/p
        // -----------------------------------------------------------------

        //LINE L SPL   D   H   M   S   MS  EXP
        //---- - ---   -  --  --  --  ---  ---

        { L_,  2,  3, -2, -3, -9, -9, -99, "      -2_03:09:09.099"        NL },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 }   x { -8 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL   D   H   M   S   MS  EXP
        //---- - ---   -  --  --  --  ---  ---

        { L_, -8, -8,  1, 23, 59, 59, 999, "+1_23:59:59.999"              NL },

        { L_, -8, -8, -2, -3, -9, -9, -99, "-2_03:09:09.099"              NL },

        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 }   x { -9 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL   D   H   M   S   MS  EXP
        //---- - ---   -  --  --  --  ---  ---

        { L_, -9, -9,  1, 23, 59, 59, 999, "+1_23:59:59.999"                 },

        { L_, -9, -9, -2, -3, -9, -9, -99, "-2_03:09:09.099"                 },

#undef NL

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const int         L     = DATA[ti].d_level;
                const int         SPL   = DATA[ti].d_spacesPerLevel;
                const int         DAYS  = DATA[ti].d_days;
                const Int64       HOURS = DATA[ti].d_hours;
                const Int64       MINS  = DATA[ti].d_mins;
                const Int64       SECS  = DATA[ti].d_secs;
                const Int64       MSECS = DATA[ti].d_msecs;
                const char *const EXP   = DATA[ti].d_expected_p;

                if (veryVerbose) {
                    T_ P_(LINE) P_(L) P(SPL)
                    T_ P_(DAYS) P_(HOURS) P_(MINS) P_(SECS) P(MSECS)
                }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                const Int64 TOTAL_MSECS =
                                    flds2Msecs(DAYS, HOURS, MINS, SECS, MSECS);

                Obj mX;  const Obj& X = mX;
                mX.setTotalMilliseconds(TOTAL_MSECS);

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                ostringstream os(&oa);

                // Verify supplied stream is returned by reference.

                if (-9 == L && -9 == SPL) {
                    LOOP_ASSERT(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    LOOP_ASSERT(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        LOOP_ASSERT(LINE, &os == &X.print(os, L, SPL));
                    }
                    else if (-8 != L) {
                        LOOP_ASSERT(LINE, &os == &X.print(os, L));
                    }
                    else {
                        LOOP_ASSERT(LINE, &os == &X.print(os));
                    }

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                {
                    bslma::TestAllocator         da("default",
                                                    veryVeryVeryVerbose);
                    bslma::DefaultAllocatorGuard dag(&da);

                    // Verify output is formatted as expected.

                    if (veryVeryVerbose) { P(os.str()) }

                    LOOP3_ASSERT(LINE, EXP, os.str(), EXP == os.str());
                }
            }
        }

        if (verbose) cout << "\nVerify format of additional time intervals."
                          << endl;
        {
            static const struct {
                int         d_line;            // source line number

                int         d_days;
                Int64       d_hours;
                Int64       d_mins;
                Int64       d_secs;
                Int64       d_msecs;

                const char *d_expected_p;
            } DATA[] = {
                //LINE       D    H    M    S    MS   EXP
                //----      --   --   --   --   ---   ---

                { L_,        0,   0,   0,   0,    0,  "+0_00:00:00.000"      },

                { L_,        1,   0,   0,   0,    0,  "+1_00:00:00.000"      },
                { L_,        0,   2,   0,   0,    0,  "+0_02:00:00.000"      },
                { L_,        0,   0,   3,   0,    0,  "+0_00:03:00.000"      },
                { L_,        0,   0,   0,   4,    0,  "+0_00:00:04.000"      },
                { L_,        0,   0,   0,   0,    5,  "+0_00:00:00.005"      },

                { L_,       -1,   0,   0,   0,    0,  "-1_00:00:00.000"      },
                { L_,        0,  -2,   0,   0,    0,  "-0_02:00:00.000"      },
                { L_,        0,   0,  -3,   0,    0,  "-0_00:03:00.000"      },
                { L_,        0,   0,   0,  -4,    0,  "-0_00:00:04.000"      },
                { L_,        0,   0,   0,   0,   -5,  "-0_00:00:00.005"      },

                { L_,
                    k_DAYS_MAX,   0,   0,   0,    0,
                                                  "+2147483647_00:00:00.000" },
                { L_,
                    k_DAYS_MAX,  23,  59,  59,  999,
                                                  "+2147483647_23:59:59.999" },

                { L_,
                    k_DAYS_MIN,   0,   0,   0,    0,
                                                  "-2147483648_00:00:00.000" },
                { L_,
                    k_DAYS_MIN, -23, -59, -59, -999,
                                                  "-2147483648_23:59:59.999" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const int         DAYS  = DATA[ti].d_days;
                const Int64       HOURS = DATA[ti].d_hours;
                const Int64       MINS  = DATA[ti].d_mins;
                const Int64       SECS  = DATA[ti].d_secs;
                const Int64       MSECS = DATA[ti].d_msecs;
                const char *const EXP   = DATA[ti].d_expected_p;

                const int         L     =  0;
                const int         SPL   = -1;

                if (veryVerbose) {
                    T_ P_(LINE) P_(L) P(SPL)
                    T_ P_(DAYS) P_(HOURS) P_(MINS) P_(SECS) P(MSECS)
                }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                const Int64 TOTAL_MSECS =
                                    flds2Msecs(DAYS, HOURS, MINS, SECS, MSECS);

                Obj mX;  const Obj& X = mX;
                mX.setTotalMilliseconds(TOTAL_MSECS);

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                ostringstream os1(&oa);  // use with 'print'
                ostringstream os2(&oa);  // use with 'operator<<'

                LOOP_ASSERT(LINE, &os1 == &X.print(os1, L, SPL));
                LOOP_ASSERT(LINE, &os2 == &(os2 << X));

                {
                    bslma::TestAllocator         da("default",
                                                    veryVeryVeryVerbose);
                    bslma::DefaultAllocatorGuard dag(&da);

                    // Verify output is formatted as expected.

                    if (veryVeryVerbose) { T_ P_(os1.str()) T_ P(os2.str()) }

                    LOOP3_ASSERT(LINE, EXP, os1.str(), EXP == os1.str());
                    LOOP3_ASSERT(LINE, EXP, os2.str(), EXP == os2.str());
                }
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each of the five field-based accessors returns the value of the
        //:   corresponding field of the time interval.
        //:
        //: 2 The 'totalMilliseconds' accessor returns the value of the time
        //:   interval in milliseconds.
        //:
        //: 3 Each basic accessor method is declared 'const'.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their five-field
        //:   representation.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..3)
        //:
        //:   1 Using the 'flds2Msecs' helper function, convert the five-field
        //:     representation in 'R' to its corresponding total-milliseconds
        //:     representation.
        //:
        //:   2 Create an object 'X' using the default constructor.
        //:
        //:   3 Using 'setTotalMilliseconds', set 'X' to the value computed in
        //:     P-2.1.
        //:
        //:   4 Verify that each basic accessor, invoked on a reference
        //:     providing non-modifiable access to the object created in P-2.2,
        //:     returns the expected value.  (C-1..3)
        //
        // Testing:
        //   int days() const;
        //   int hours() const;
        //   int minutes() const;
        //   int seconds() const;
        //   int milliseconds() const;
        //   Int64 totalMilliseconds() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nUse a table of distinct object values."
                          << endl;

        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
                "\nVerify all basic accessors report expected values." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE  = DATA[ti].d_line;
            const int DAYS  = DATA[ti].d_days;
            const int HOURS = DATA[ti].d_hours;
            const int MINS  = DATA[ti].d_mins;
            const int SECS  = DATA[ti].d_secs;
            const int MSECS = DATA[ti].d_msecs;

            const Int64 TOTAL_MSECS =
                                    flds2Msecs(DAYS, HOURS, MINS, SECS, MSECS);

            Obj mX;  const Obj& X = mX;
            mX.setTotalMilliseconds(TOTAL_MSECS);

            if (veryVerbose) {
                T_ P_(LINE) P_(DAYS) P_(HOURS) P_(MINS) P_(SECS) P_(MSECS)
                T_ P_(TOTAL_MSECS) P(X)
            }

            LOOP_ASSERT(ti, DAYS        == X.days());
            LOOP_ASSERT(ti, HOURS       == X.hours());
            LOOP_ASSERT(ti, MINS        == X.minutes());
            LOOP_ASSERT(ti, SECS        == X.seconds());
            LOOP_ASSERT(ti, MSECS       == X.milliseconds());
            LOOP_ASSERT(ti, TOTAL_MSECS == X.totalMilliseconds());
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //   Ensure that the test helper function, 'flds2Msecs', correctly
        //   translates five-field representations of time intervals to their
        //   corresponding total-milliseconds representations.
        //
        // Concerns:
        //: 1 The 'flds2Msecs' helper function returns the correct
        //:   total-millisecond representation computed from the supplied
        //:   arguments.
        //:
        //: 2 'flds2Msecs' accepts the contractually specified range of
        //:   argument values.
        //:
        //: 3 The optional 'hours', 'minutes', 'seconds', and 'milliseconds'
        //:   parameters each have the correct default value (0).
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the table-driven technique to specify a set of numbers of
        //:   days (one per row).
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..3)
        //:
        //:   1 Record, in 'EXP', the number of milliseconds in the value
        //:     from 'R' (as computed by inline code).
        //:
        //:   2 Use the 'flds2Msecs' function to compute the number of
        //:     milliseconds in the value from 'R', supplying only one argument
        //:     to the function (letting the remaining four arguments take
        //:     their default values).
        //:
        //:   3 Use the 'flds2Msecs' function a second time to recompute the
        //:     number of milliseconds in the value from 'R', but this time
        //:     passing four trailing 0 arguments explicitly.
        //:
        //:   4 Verify that the values computed in P-2.2 and P-2.3 are the same
        //:     as that computed in P-2.1.  (C-1..3)
        //:
        //: 3 Repeat steps similar to those described in P-1..2 four times,
        //:   successively introducing an additional column to the tables (for
        //:   hours, minutes, seconds, and milliseconds), to test the supplying
        //:   of two, three, four, and five arguments to 'flds2Msecs'.
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid total-millisecond values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   TEST APPARATUS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST APPARATUS" << endl
                          << "==============" << endl;

        if (verbose) cout << "\nTesting 'flds2Msecs(d)'." << endl;
        {
            static const struct {
                int d_line;       // source line number
                int d_days;
            } DATA[] = {
                //LINE      DAYS
                //----   ----------
                { L_,             0 },

                { L_,             1 },
                { L_,         13027 },
                { L_,    k_DAYS_MAX },

                { L_,            -1 },
                { L_,        -42058 },
                { L_,    k_DAYS_MIN },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE = DATA[ti].d_line;
                const int DAYS = DATA[ti].d_days;

                if (veryVerbose) { T_ P(DAYS) }

                const Int64 EXP = DAYS * k_MSECS_PER_DAY;

                {
                    const Int64 ACT = flds2Msecs(DAYS);
                    LOOP3_ASSERT(LINE, EXP, ACT, EXP == ACT);
                }

                {
                    const Int64 ACT = flds2Msecs(DAYS, 0, 0, 0, 0);
                    LOOP3_ASSERT(LINE, EXP, ACT, EXP == ACT);
                }
            }
        }

        if (verbose) cout << "\nTesting 'flds2Msecs(d, h)'." << endl;
        {
            static const struct {
                int   d_line;   // source line number
                int   d_days;
                Int64 d_hours;
            } DATA[] = {
                //LINE      DAYS        HOURS
                //----      ----        -----
                { L_,          0,           0 },

                { L_,          0,           1 },
                { L_,          0,          24 },
                { L_,          0, k_HOURS_MAX },

                { L_,          1,           0 },
                { L_,        366,           0 },
                { L_, k_DAYS_MAX,           0 },

                { L_,        333,         457 },

                { L_,
                  k_DAYS_MAX - 1,          47 },

                { L_, k_DAYS_MAX,          23 },


                { L_,          0,          -1 },
                { L_,          0,         -24 },
                { L_,          0, k_HOURS_MIN },

                { L_,         -1,           0 },
                { L_,       -366,           0 },
                { L_, k_DAYS_MIN,           0 },

                { L_,       -333,        -457 },

                { L_,
                  k_DAYS_MIN + 1,         -47 },

                { L_, k_DAYS_MIN,         -23 },

                { L_,     -23469,         256 },
                { L_,          5,          -4 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_line;
                const int   DAYS  = DATA[ti].d_days;
                const Int64 HOURS = DATA[ti].d_hours;

                if (veryVerbose) { T_ P_(LINE) P_(DAYS) P(HOURS) }

                const Int64 EXP = DAYS  * k_MSECS_PER_DAY
                                + HOURS * k_MSECS_PER_HOUR;

                {
                    const Int64 ACT = flds2Msecs(DAYS, HOURS);
                    LOOP3_ASSERT(LINE, EXP, ACT, EXP == ACT);
                }

                {
                    const Int64 ACT = flds2Msecs(DAYS, HOURS, 0, 0, 0);
                    LOOP3_ASSERT(LINE, EXP, ACT, EXP == ACT);
                }
            }
        }

        if (verbose) cout << "\nTesting 'flds2Msecs(d, h, m)'." << endl;
        {
            static const struct {
                int   d_line;   // source line number
                int   d_days;
                Int64 d_hours;
                Int64 d_mins;
            } DATA[] = {
                //LINE      DAYS        HOURS     MINUTES
                //----      ----        -----     -------
                { L_,          0,           0,          0 },

                { L_,          0,           0,          1 },
                { L_,          0,           0,         59 },
                { L_,          0,          24,         60 },
                { L_,        100,         200,        300 },

                { L_,          0,           1,
                                          k_MINS_MAX - 60 },

                { L_,          0,           0, k_MINS_MAX },

                { L_,          0,
                              k_HOURS_MAX - 1,        119 },

                { L_,          0, k_HOURS_MAX,         59 },
                { L_, k_DAYS_MAX,          23,         59 },

                { L_,          0,           0,         -1 },
                { L_,          0,           0,        -59 },
                { L_,          0,         -24,        -60 },
                { L_,       -100,        -200,       -300 },

                { L_,          0,          -1,
                                          k_MINS_MIN + 60 },

                { L_,          0,           0, k_MINS_MIN },

                { L_,          0,
                              k_HOURS_MIN + 1,       -119 },

                { L_,          0, k_HOURS_MIN,        -59 },
                { L_, k_DAYS_MIN,         -23,        -59 },

                { L_,     -23469,         256,       -212 },
                { L_,          5,          -4,          3 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_line;
                const int   DAYS  = DATA[ti].d_days;
                const Int64 HOURS = DATA[ti].d_hours;
                const Int64 MINS  = DATA[ti].d_mins;

                if (veryVerbose) { T_ P_(LINE) P_(DAYS) P_(HOURS) P(MINS) }

                const Int64 EXP = DAYS  * k_MSECS_PER_DAY
                                + HOURS * k_MSECS_PER_HOUR
                                + MINS  * k_MSECS_PER_MIN;

                {
                    const Int64 ACT = flds2Msecs(DAYS, HOURS, MINS);
                    LOOP3_ASSERT(LINE, EXP, ACT, EXP == ACT);
                }

                {
                    const Int64 ACT = flds2Msecs(DAYS, HOURS, MINS, 0, 0);
                    LOOP3_ASSERT(LINE, EXP, ACT, EXP == ACT);
                }
            }
        }

        if (verbose) cout << "\nTesting 'flds2Msecs(d, h, m, s)'." << endl;
        {
            static const struct {
                int   d_line;   // source line number
                int   d_days;
                Int64 d_hours;
                Int64 d_mins;
                Int64 d_secs;
            } DATA[] = {
                //LINE      DAYS        HOURS     MINUTES    SECONDS
                //----      ----        -----     -------    -------
                { L_,          0,           0,          0,         0 },

                { L_,          0,           0,          0,         1 },
                { L_,          0,           0,         59,        59 },
                { L_,          0,          24,         60,        60 },
                { L_,        100,         200,        300,       400 },

                { L_,          0,           0,          1,
                                                     k_SECS_MAX - 60 },

                { L_,          0,           0,          0,
                                                          k_SECS_MAX },

                { L_,          0,           0,
                                           k_MINS_MAX - 1,       119 },

                { L_,          0,           0, k_MINS_MAX,        59 },

                { L_, k_DAYS_MAX,          23,         59,        59 },

                { L_,          0,           0,          0,        -1 },
                { L_,          0,           0,        -59,       -59 },
                { L_,          0,         -24,        -60,       -60 },
                { L_,       -100,        -200,       -300,      -400 },

                { L_,          0,           0,         -1,
                                                     k_SECS_MIN + 60 },

                { L_,          0,           0,          0,
                                                          k_SECS_MIN },
                { L_,          0,           0,
                                           k_MINS_MIN + 1,      -119 },

                { L_,          0,           0, k_MINS_MIN,       -59 },

                { L_, k_DAYS_MIN,         -23,        -59,       -59 },

                { L_,     -23469,         256,       -212,        77 },
                { L_,          5,          -4,          3,        -2 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_line;
                const int   DAYS  = DATA[ti].d_days;
                const Int64 HOURS = DATA[ti].d_hours;
                const Int64 MINS  = DATA[ti].d_mins;
                const Int64 SECS  = DATA[ti].d_secs;

                if (veryVerbose) {
                    T_ P_(LINE) P_(DAYS) P_(HOURS) P_(MINS) P(SECS)
                }

                const Int64 EXP = DAYS  * k_MSECS_PER_DAY
                                + HOURS * k_MSECS_PER_HOUR
                                + MINS  * k_MSECS_PER_MIN
                                + SECS  * k_MSECS_PER_SEC;

                {
                    const Int64 ACT = flds2Msecs(DAYS, HOURS, MINS, SECS);
                    LOOP3_ASSERT(LINE, EXP, ACT, EXP == ACT);
                }

                {
                    const Int64 ACT = flds2Msecs(DAYS, HOURS, MINS, SECS, 0);
                    LOOP3_ASSERT(LINE, EXP, ACT, EXP == ACT);
                }
            }
        }

        if (verbose) cout << "\nTesting 'flds2Msecs(d, h, m, s, ms)'." << endl;
        {
            static const struct {
                int   d_line;   // source line number
                int   d_days;
                Int64 d_hours;
                Int64 d_mins;
                Int64 d_secs;
                Int64 d_msecs;
            } DATA[] = {
                //LINE      DAYS    HOURS     MINUTES    SECONDS  MILLISECONDS
                //----      ----    -----     -------    -------  ------------
                { L_,          0,       0,          0,         0,          0 },

                { L_,          0,       0,          0,         0,          1 },
                { L_,          0,       0,          0,        59,        999 },
                { L_,          0,       0,         59,        59,        999 },
                { L_,          0,      24,         60,        60,       1000 },
                { L_,        100,     200,        300,       400,        500 },

                { L_,          0,       0,          0,         1,
                                                          k_MSECS_MAX - 1000 },

                { L_,          0,       0,          0,         0,
                                                                 k_MSECS_MAX },

                { L_,          0,       0,          0,
                                                  k_SECS_MAX - 1,       1999 },

                { L_,          0,       0,          0,
                                                      k_SECS_MAX,        999 },

                { L_, k_DAYS_MAX,      23,         59,        59,        999 },

                { L_,          0,       0,          0,         0,         -1 },
                { L_,          0,       0,          0,       -59,       -999 },
                { L_,          0,       0,        -59,       -59,       -999 },
                { L_,          0,     -24,        -60,       -60,      -1000 },
                { L_,       -100,    -200,       -300,      -400,       -500 },

                { L_,          0,       0,          0,        -1,
                                                          k_MSECS_MIN + 1000 },

                { L_,          0,       0,          0,         0,
                                                                 k_MSECS_MIN },
                { L_,          0,       0,          0,
                                               k_SECS_MIN + 1000,      -1999 },

                { L_,          0,       0,          0,
                                                      k_SECS_MIN,       -999 },

                { L_, k_DAYS_MIN,     -23,        -59,       -59,       -999 },

                { L_,     -23469,     256,       -212,        77,        -13 },
                { L_,          5,      -4,          3,        -2,          1 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_line;
                const int   DAYS  = DATA[ti].d_days;
                const Int64 HOURS = DATA[ti].d_hours;
                const Int64 MINS  = DATA[ti].d_mins;
                const Int64 SECS  = DATA[ti].d_secs;
                const Int64 MSECS = DATA[ti].d_msecs;

                if (veryVerbose) {
                    T_ P_(LINE) P_(DAYS) P_(HOURS) P_(MINS) P_(SECS) P(MSECS)
                }

                const Int64 EXP = DAYS  * k_MSECS_PER_DAY
                                + HOURS * k_MSECS_PER_HOUR
                                + MINS  * k_MSECS_PER_MIN
                                + SECS  * k_MSECS_PER_SEC
                                + MSECS;

                {
                    const Int64 ACT =
                                    flds2Msecs(DAYS, HOURS, MINS, SECS, MSECS);
                    LOOP3_ASSERT(LINE, EXP, ACT, EXP == ACT);
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            {
                ASSERT_FAIL(flds2Msecs(0, k_HOURS_MIN - 1));
                ASSERT_PASS(flds2Msecs(0, k_HOURS_MIN    ));

                ASSERT_PASS(flds2Msecs(0, k_HOURS_MAX    ));
                ASSERT_FAIL(flds2Msecs(0, k_HOURS_MAX + 1));

                ASSERT_FAIL(flds2Msecs(0, 0, k_MINS_MIN - 1));
                ASSERT_PASS(flds2Msecs(0, 0, k_MINS_MIN    ));

                ASSERT_PASS(flds2Msecs(0, 0, k_MINS_MAX    ));
                ASSERT_FAIL(flds2Msecs(0, 0, k_MINS_MAX + 1));

                ASSERT_FAIL(flds2Msecs(0, 0, 0, k_SECS_MIN - 1));
                ASSERT_PASS(flds2Msecs(0, 0, 0, k_SECS_MIN    ));

                ASSERT_PASS(flds2Msecs(0, 0, 0, k_SECS_MAX    ));
                ASSERT_FAIL(flds2Msecs(0, 0, 0, k_SECS_MAX + 1));

                ASSERT_FAIL(flds2Msecs(0, 0, 0, 0, k_MSECS_MIN - 1));
                ASSERT_PASS(flds2Msecs(0, 0, 0, 0, k_MSECS_MIN    ));

                ASSERT_PASS(flds2Msecs(0, 0, 0, 0, k_MSECS_MAX    ));
                ASSERT_FAIL(flds2Msecs(0, 0, 0, 0, k_MSECS_MAX + 1));
            }

            {
                ASSERT_FAIL(flds2Msecs(k_DAYS_MIN, -24));
                ASSERT_PASS(flds2Msecs(k_DAYS_MIN, -23));

                ASSERT_PASS(flds2Msecs(k_DAYS_MAX,  23));
                ASSERT_FAIL(flds2Msecs(k_DAYS_MAX,  24));

                ASSERT_FAIL(flds2Msecs(k_DAYS_MIN, -23, -60));
                ASSERT_PASS(flds2Msecs(k_DAYS_MIN, -23, -59));

                ASSERT_PASS(flds2Msecs(k_DAYS_MAX,  23,  59));
                ASSERT_FAIL(flds2Msecs(k_DAYS_MAX,  23,  60));

                ASSERT_FAIL(flds2Msecs(k_DAYS_MIN, -23, -59, -60));
                ASSERT_PASS(flds2Msecs(k_DAYS_MIN, -23, -59, -59));

                ASSERT_PASS(flds2Msecs(k_DAYS_MAX,  23,  59,  59));
                ASSERT_FAIL(flds2Msecs(k_DAYS_MAX,  23,  59,  60));

                ASSERT_FAIL(flds2Msecs(k_DAYS_MIN, -23, -59, -59, -1000));
                ASSERT_PASS(flds2Msecs(k_DAYS_MIN, -23, -59, -59,  -999));

                ASSERT_PASS(flds2Msecs(k_DAYS_MAX,  23,  59,  59,   999));
                ASSERT_FAIL(flds2Msecs(k_DAYS_MAX,  23,  59,  59,  1000));
            }

            {
                ASSERT_FAIL(flds2Msecs(0, k_HOURS_MIN, -60));
                ASSERT_PASS(flds2Msecs(0, k_HOURS_MIN, -59));

                ASSERT_PASS(flds2Msecs(0, k_HOURS_MAX,  59));
                ASSERT_FAIL(flds2Msecs(0, k_HOURS_MAX,  60));

                ASSERT_FAIL(flds2Msecs(0, k_HOURS_MIN, -59, -60));
                ASSERT_PASS(flds2Msecs(0, k_HOURS_MIN, -59, -59));

                ASSERT_PASS(flds2Msecs(0, k_HOURS_MAX,  59,  59));
                ASSERT_FAIL(flds2Msecs(0, k_HOURS_MAX,  59,  60));

                ASSERT_FAIL(flds2Msecs(0, k_HOURS_MIN, -59, -59, -1000));
                ASSERT_PASS(flds2Msecs(0, k_HOURS_MIN, -59, -59,  -999));

                ASSERT_PASS(flds2Msecs(0, k_HOURS_MAX,  59,  59,   999));
                ASSERT_FAIL(flds2Msecs(0, k_HOURS_MAX,  59,  59,  1000));
            }

            {
                ASSERT_FAIL(flds2Msecs(0, 0, k_MINS_MIN, -60));
                ASSERT_PASS(flds2Msecs(0, 0, k_MINS_MIN, -59));

                ASSERT_PASS(flds2Msecs(0, 0, k_MINS_MAX,  59));
                ASSERT_FAIL(flds2Msecs(0, 0, k_MINS_MAX,  60));

                ASSERT_FAIL(flds2Msecs(0, 0, k_MINS_MIN, -59, -1000));
                ASSERT_PASS(flds2Msecs(0, 0, k_MINS_MIN, -59,  -999));

                ASSERT_PASS(flds2Msecs(0, 0, k_MINS_MAX,  59,   999));
                ASSERT_FAIL(flds2Msecs(0, 0, k_MINS_MAX,  59,  1000));
            }

            {
                ASSERT_FAIL(flds2Msecs(0, 0, 0, k_SECS_MIN, -1000));
                ASSERT_PASS(flds2Msecs(0, 0, 0, k_SECS_MIN,  -999));

                ASSERT_PASS(flds2Msecs(0, 0, 0, k_SECS_MAX,   999));
                ASSERT_FAIL(flds2Msecs(0, 0, 0, k_SECS_MAX,  1000));
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value), use the primary
        //   manipulators to put that object into any state relevant for
        //   thorough testing, and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //:
        //: 2 An object can be safely destroyed.
        //:
        //: 3 The 'setTotalMilliseconds' method can set an object to have any
        //:   value that does not violate the method's documented
        //:   preconditions.
        //:
        //: 4 'setTotalMilliseconds' is not affected by the state of the object
        //:   on entry.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create an object using the default constructor.  Verify, using
        //:   the (as yet unproven) 'totalMilliseconds' accessor, that the
        //:   resulting object has a time interval of 0.  (C-1)
        //:
        //: 2 Let the object created in P-1 go out of scope.  (C-2)
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their total-milliseconds
        //:   representation.
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-3..4)
        //:
        //:   1 Create an object 'X' using the default constructor.
        //:
        //:   2 Using the 'setTotalMilliseconds' (primary) manipulator, set 'X'
        //:     to the value specified in 'R1'.
        //:
        //:   3 Verify, using 'totalMilliseconds', that 'X' has the expected
        //:     value.  (C-3)
        //:
        //:   4 For each row 'R2' in the table of P-3:  (C-4)
        //:
        //:     1 Create an object 'Y' using the default constructor.
        //:
        //:     2 Using 'setTotalMilliseconds', first set 'Y' to the value
        //:       specified in 'R2', then set it to the value from 'R1'.  After
        //:       each setting, verify that 'Y' has the expected value.  (C-4)
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid total-millisecond values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-5)
        //
        // Testing:
        //   DatetimeInterval();
        //   ~DatetimeInterval();
        //   void setTotalMilliseconds(Int64 milliseconds);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;
        {
            Obj mX;  const Obj& X = mX;
            if (veryVerbose) P(X)

            ASSERT(0 == X.totalMilliseconds());
        }

        if (verbose) cout << "\nTesting 'setTotalMilliseconds'." << endl;

        static const struct {
            int   d_line;        // source line number
            Int64 d_totalMsecs;
        } DATA[] = {
            //LINE   TOTAL MILLISECONDS
            //----   ------------------
            { L_,                     0 },

            { L_,                     1 },
            { L_,                 13027 },
            { L_,               INT_MAX },
            { L_,           k_MSECS_MAX },

            { L_,                    -1 },
            { L_,                -42058 },
            { L_,               INT_MIN },
            { L_,           k_MSECS_MIN },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   ILINE        = DATA[ti].d_line;
            const Int64 ITOTAL_MSECS = DATA[ti].d_totalMsecs;

            Obj mX;  const Obj& X = mX;

            mX.setTotalMilliseconds(ITOTAL_MSECS);
            if (veryVerbose) { T_ P_(X) P(ITOTAL_MSECS) }

            LOOP3_ASSERT(ILINE, ITOTAL_MSECS, X.totalMilliseconds(),
                         ITOTAL_MSECS == X.totalMilliseconds());

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int   JLINE        = DATA[tj].d_line;
                const Int64 JTOTAL_MSECS = DATA[tj].d_totalMsecs;

                Obj mY;  const Obj& Y = mY;

                mY.setTotalMilliseconds(JTOTAL_MSECS);
                if (veryVeryVerbose) { T_ P_(Y) P(JTOTAL_MSECS) }

                LOOP3_ASSERT(JLINE, JTOTAL_MSECS, Y.totalMilliseconds(),
                             JTOTAL_MSECS == Y.totalMilliseconds());

                mY.setTotalMilliseconds(ITOTAL_MSECS);
                if (veryVeryVerbose) { T_ P_(Y) P(ITOTAL_MSECS) }

                LOOP3_ASSERT(ILINE, ITOTAL_MSECS, Y.totalMilliseconds(),
                             ITOTAL_MSECS == Y.totalMilliseconds());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'setTotalMilliseconds'" << endl;
            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.setTotalMilliseconds(k_MSECS_MIN - 1));
                ASSERT_SAFE_PASS(mX.setTotalMilliseconds(k_MSECS_MIN    ));

                ASSERT_SAFE_PASS(mX.setTotalMilliseconds(k_MSECS_MAX    ));
                ASSERT_SAFE_FAIL(mX.setTotalMilliseconds(k_MSECS_MAX + 1));
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create an object 'w' (default ctor).      { w:0             }
        //: 2 Create an object 'x' (copy from 'w').     { w:0 x:0         }
        //: 3 Set 'x' to 'A' (value distinct from 0).   { w:0 x:A         }
        //: 4 Create an object 'y' (init. to 'A').      { w:0 x:A y:A     }
        //: 5 Create an object 'z' (copy from 'y').     { w:0 x:A y:A z:A }
        //: 6 Set 'z' to 0 (the default value).         { w:0 x:A y:A z:0 }
        //: 7 Assign 'w' from 'x'.                      { w:A x:A y:A z:0 }
        //: 8 Assign 'w' from 'z'.                      { w:0 x:A y:A z:0 }
        //: 9 Assign 'x' from 'x' (aliasing).           { w:0 x:A y:A z:0 }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        // Constants defining 'A' value for testing.

        const int A_DAYS = 1, A_HRS = 2, A_MINS = 3, A_SECS = 4, A_MSECS = 5;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object 'w' (default ctor)."
                             "\t\t{ w:0             }" << endl;

        Obj mW;  const Obj& W = mW;

        if (veryVerbose) cout << "\ta. Check initial value of 'w'." << endl;
        if (veryVeryVerbose) { T_ P(W) }

        ASSERT(0 == W.days());
        ASSERT(0 == W.hours());
        ASSERT(0 == W.minutes());
        ASSERT(0 == W.seconds());
        ASSERT(0 == W.milliseconds());

        if (veryVerbose) cout <<
                          "\tb. Try equality operators: 'w' <op> 'w'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object 'x' (copy from 'w')."
                             "\t\t{ w:0 x:0         }" << endl;

        Obj mX(W);  const Obj& X = mX;

        if (veryVerbose) cout <<
                                "\ta. Check the initial value of 'x'." << endl;
        if (veryVeryVerbose) { T_ P(X) }

        ASSERT(0 == X.days());
        ASSERT(0 == X.hours());
        ASSERT(0 == X.minutes());
        ASSERT(0 == X.seconds());
        ASSERT(0 == X.milliseconds());

        if (veryVerbose) cout <<
                     "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set 'x' to 'A' (value distinct from 0)."
                             "\t\t{ w:0 x:A         }" << endl;

        mX.setTotalDays(A_DAYS);
        mX.addHours(A_HRS);
        mX.addMinutes(A_MINS);
        mX.addSeconds(A_SECS);
        mX.addMilliseconds(A_MSECS);

        if (veryVerbose) cout << "\ta. Check new value of 'x'." << endl;
        if (veryVeryVerbose) { T_ P(X) }

        ASSERT(A_DAYS  == X.days());
        ASSERT(A_HRS   == X.hours());
        ASSERT(A_MINS  == X.minutes());
        ASSERT(A_SECS  == X.seconds());
        ASSERT(A_MSECS == X.milliseconds());

        if (veryVerbose) cout <<
                     "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 4. Create an object 'y' (init. to 'A')."
                             "\t\t{ w:0 x:A y:A     }" << endl;

        Obj mY(A_DAYS, A_HRS, A_MINS, A_SECS, A_MSECS);  const Obj& Y = mY;

        if (veryVerbose) cout << "\ta. Check initial value of 'y'." << endl;
        if (veryVeryVerbose) { T_ P(Y) }

        ASSERT(A_DAYS  == Y.days());
        ASSERT(A_HRS   == Y.hours());
        ASSERT(A_MINS  == Y.minutes());
        ASSERT(A_SECS  == Y.seconds());
        ASSERT(A_MSECS == Y.milliseconds());

        if (veryVerbose) cout <<
                "\tb. Try equality operators: 'y' <op> 'w', 'x', 'y'." << endl;

        ASSERT(0 == (Y == W));        ASSERT(1 == (Y != W));
        ASSERT(1 == (Y == X));        ASSERT(0 == (Y != X));
        ASSERT(1 == (Y == Y));        ASSERT(0 == (Y != Y));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 5. Create an object 'z' (copy from 'y')."
                             "\t\t{ w:0 x:A y:A z:A }" << endl;

        Obj mZ(Y);  const Obj& Z = mZ;

        if (veryVerbose) cout << "\ta. Check initial value of 'z'." << endl;
        if (veryVeryVerbose) { T_ P(Z) }

        ASSERT(A_DAYS  == Z.days());
        ASSERT(A_HRS   == Z.hours());
        ASSERT(A_MINS  == Z.minutes());
        ASSERT(A_SECS  == Z.seconds());
        ASSERT(A_MSECS == Z.milliseconds());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        ASSERT(1 == (Z == Y));        ASSERT(0 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Set 'z' to 0 (the default value)."
                             "\t\t\t{ w:0 x:A y:A z:0 }" << endl;

        mZ.setTotalMilliseconds(0);

        if (veryVerbose) cout << "\ta. Check new value of 'z'." << endl;
        if (veryVeryVerbose) { T_ P(Z) }

        ASSERT(0 == Z.days());
        ASSERT(0 == Z.hours());
        ASSERT(0 == Z.minutes());
        ASSERT(0 == Z.seconds());
        ASSERT(0 == Z.milliseconds());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (Z == W));        ASSERT(0 == (Z != W));
        ASSERT(0 == (Z == X));        ASSERT(1 == (Z != X));
        ASSERT(0 == (Z == Y));        ASSERT(1 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 7. Assign 'w' from 'x'."
                             "\t\t\t\t{ w:A x:A y:A z:0 }" << endl;

        mW = X;

        if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ P(W) }

        ASSERT(A_DAYS  == W.days());
        ASSERT(A_HRS   == W.hours());
        ASSERT(A_MINS  == W.minutes());
        ASSERT(A_SECS  == W.seconds());
        ASSERT(A_MSECS == W.milliseconds());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'w' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(1 == (W == X));        ASSERT(0 == (W != X));
        ASSERT(1 == (W == Y));        ASSERT(0 == (W != Y));
        ASSERT(0 == (W == Z));        ASSERT(1 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 8. Assign 'w' from 'z'."
                             "\t\t\t\t{ w:0 x:A y:A z:0 }" << endl;

        mW = Z;

        if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ P(W) }

        ASSERT(0 == W.days());
        ASSERT(0 == W.hours());
        ASSERT(0 == W.minutes());
        ASSERT(0 == W.seconds());
        ASSERT(0 == W.milliseconds());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'w' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(0 == (W == X));        ASSERT(1 == (W != X));
        ASSERT(0 == (W == Y));        ASSERT(1 == (W != Y));
        ASSERT(1 == (W == Z));        ASSERT(0 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 9. Assign 'x' from 'x' (aliasing)."
                             "\t\t\t{ w:0 x:A y:A z:0 }" << endl;

        mX = X;

        if (veryVerbose) cout << "\ta. Check (same) value of 'x'." << endl;
        if (veryVeryVerbose) { T_ P(X) }

        ASSERT(A_DAYS  == X.days());
        ASSERT(A_HRS   == X.hours());
        ASSERT(A_MINS  == X.minutes());
        ASSERT(A_SECS  == X.seconds());
        ASSERT(A_MSECS == X.milliseconds());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));
        ASSERT(1 == (X == Y));        ASSERT(0 == (X != Y));
        ASSERT(0 == (X == Z));        ASSERT(1 == (X != Z));

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    // CONCERN: In no case does memory come from the default allocator.

    LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                0 == defaultAllocator.numBlocksTotal());

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
