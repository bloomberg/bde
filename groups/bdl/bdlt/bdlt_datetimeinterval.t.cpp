// bdlt_datetimeinterval.t.cpp                                        -*-C++-*-
#include <bdlt_datetimeinterval.h>

#include <bdlt_date.h>
#include <bdlt_timeunitratio.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_log.h>
#include <bsls_platform.h>
#include <bsls_review.h>
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
#include <bsl_cmath.h>
#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>     // 'memcmp'
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test implements a single, simply constrained
// (value-semantic) attribute class.  The Primary Manipulators and Basic
// Accessors are therefore, respectively, the attribute setters and getters,
// each of which follows our standard unconstrained attribute-type naming
// conventions: 'setAttributeName' and 'attributeName'.
//
// Primary Manipulator:
//: o 'setInterval'
//
// Basic Accessors:
//: o 'days'
//: o 'fractionalDayInMicroseconds'
//
// This particular attribute class also provides a value constructor capable of
// creating an object in any state relevant for thorough testing, obviating the
// primitive generator function, 'gg', normally used for this purpose.  We will
// therefore follow our standard 10-case approach to testing value-semantic
// types except that we will leave case 3 empty.
//-----------------------------------------------------------------------------
//
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
// [17] DatetimeInterval& operator+=(const DatetimeInterval& rhs);
// [17] DatetimeInterval& operator-=(const DatetimeInterval& rhs);
// [ 2] void setInterval(int d, Int64 h = 0, m = 0, s = 0, ms = 0);
// [12] void setTotalDays(int days);
// [12] void setTotalHours(Int64 hours);
// [12] void setTotalMinutes(Int64 minutes);
// [12] void setTotalSeconds(Int64 seconds);
// [12] void setTotalSecondsFromDouble(double seconds);
// [12] void setTotalMilliseconds(Int64 milliseconds);
// [12] void setTotalMicroseconds(Int64 microseconds);
// [15] void addInterval(int d, Int64 h = 0, m = 0, s = 0, ms = 0);
// [16] void addDays(int days);
// [16] void addHours(Int64 hours);
// [16] void addMinutes(Int64 minutes);
// [16] void addSeconds(Int64 seconds);
// [16] void addMilliseconds(Int64 milliseconds);
// [16] void addMicroseconds(Int64 microseconds);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
//
// ACCESSORS
// [ 4] int days() const;
// [ 4] Int64 fractionDayInMicroseconds() const;
// [14] int hours() const;
// [14] int minutes() const;
// [14] int seconds() const;
// [14] int milliseconds() const;
// [14] int microseconds() const;
// [14] int totalDays() const;
// [14] Int64 totalHours() const;
// [14] Int64 totalMinutes() const;
// [14] Int64 totalSeconds() const;
// [14] double totalSecondsAsDouble() const;
// [14] Int64 totalMilliseconds() const;
// [14] Int64 totalMicroseconds() const;
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
//
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [18] DatetimeInterval operator+(const DatetimeInterval& lhs, rhs);
// [18] DatetimeInterval operator-(const DatetimeInterval& lhs, rhs);
// [19] DatetimeInterval operator-(const DatetimeInterval& value);
// [ 6] bool operator==(const DatetimeInterval& lhs, rhs);
// [ 6] bool operator!=(const DatetimeInterval& lhs, rhs);
// [13] bool operator< (const DatetimeInterval& lhs, rhs);
// [13] bool operator<=(const DatetimeInterval& lhs, rhs);
// [13] bool operator> (const DatetimeInterval& lhs, rhs);
// [13] bool operator>=(const DatetimeInterval& lhs, rhs);
// [ 5] ostream& operator<<(ostream &os, const DatetimeInterval& object);
// [20] void hashAppend(HASHALG&, const DatetimeInterval&);
#ifndef BDE_OPENSOURCE_PUBLICATION  // pending deprecation
// DEPRECATED
// [10] static int maxSupportedBdexVersion();
#endif // BDE_OPENSOURCE_PUBLICATION -- pending deprecation
#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
// [10] static int maxSupportedVersion();
// [ 5] bsl::ostream& streamOut(bsl::ostream& stream) const;
#endif // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [21] USAGE EXAMPLE
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

#define VERSION_SELECTOR 99991231

typedef bsls::Types::Int64     Int64;

const Int64 k_USECS_PER_DAY  = bdlt::TimeUnitRatio::k_US_PER_D;
const Int64 k_USECS_PER_SEC  = bdlt::TimeUnitRatio::k_US_PER_S;

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

const Int64 k_USECS_MAX = bsl::numeric_limits<Int64>::max();
const Int64 k_USECS_MIN = bsl::numeric_limits<Int64>::min();

// Verify PUBLIC CLASS DATA

BSLMF_ASSERT(k_MSECS_MAX == Obj::k_MILLISECONDS_MAX);
BSLMF_ASSERT(k_MSECS_MIN == Obj::k_MILLISECONDS_MIN);

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

struct DefaultDataRow {
    int   d_line;
    int   d_days;
    Int64 d_hours;
    Int64 d_minutes;
    Int64 d_seconds;
    Int64 d_msecs;
    Int64 d_usecs;
    int   d_expDays;
    Int64 d_expUsecs;
};

static const DefaultDataRow DEFAULT_DATA[] =
{
    //LN  DAYS   HOURS  MIN  SEC  MSEC    USEC      EXP_D         EXP_US
    //--  -----  -----  ---  ---  ----  --------  ----------  --------------
    { L_,     0,     0,   0,   0,    0,        0,          0,            0LL },

    { L_,     0,     0,   0,   0,    0,        1,          0,            1LL },
    { L_,     0,     0,   0,   0,    1,        0,          0,         1000LL },
    { L_,     0,     0,   0,   1,    0,        0,          0,      1000000LL },
    { L_,     0,     0,   1,   0,    0,        0,          0,     60000000LL },
    { L_,     0,     1,   0,   0,    0,        0,          0,   3600000000LL },
    { L_,     1,     0,   0,   0,    0,        0,          1,            0LL },

    { L_,     1,     0,   0,   0,    0,        1,          1,            1LL },
    { L_,     1,     0,   0,   0,    1,        0,          1,         1000LL },
    { L_,     1,     0,   0,   1,    0,        0,          1,      1000000LL },
    { L_,     1,     0,   1,   0,    0,        0,          1,     60000000LL },
    { L_,     1,     1,   0,   0,    0,        0,          1,   3600000000LL },

    { L_,     0,    24,   0,   0,    0,        1,          1,            1LL },
    { L_,     0,    24,   0,   0,    1,        0,          1,         1000LL },
    { L_,     0,    24,   0,   1,    0,        0,          1,      1000000LL },
    { L_,     0,    24,   1,   0,    0,        0,          1,     60000000LL },
    { L_,     0,    25,   0,   0,    0,        0,          1,   3600000000LL },

    { L_, k_DAYS_MAX, 0,  0,   0,    0,        0, k_DAYS_MAX,            0LL },
    { L_, k_DAYS_MIN, 0,  0,   0,    0,        0, k_DAYS_MIN,            0LL },

    { L_,     0, k_HOURS_MAX, 0, 0,  0,        0, k_DAYS_MAX,  82800000000LL },
    { L_,     0, k_HOURS_MIN, 0, 0,  0,        0, k_DAYS_MIN, -82800000000LL },

    { L_,     0,     0, k_MINS_MAX, 0, 0,      0, k_DAYS_MAX,  86340000000LL },
    { L_,     0,     0, k_MINS_MIN, 0, 0,      0, k_DAYS_MIN, -86340000000LL },

    { L_,     0,     0,   0, k_SECS_MAX, 0,    0, k_DAYS_MAX,  86399000000LL },
    { L_,     0,     0,   0, k_SECS_MIN, 0,    0, k_DAYS_MIN, -86399000000LL },

    { L_,     0,     0,   0,   0, k_MSECS_MAX, 0, k_DAYS_MAX,  86399999000LL },
    { L_,     0,     0,   0,   0, k_MSECS_MIN, 0, k_DAYS_MIN, -86399999000LL },

    { L_,     0,     0,   0,   0,    0, k_USECS_MAX,
                                                   106751991,  14454775807LL },
    { L_,     0,     0,   0,   0,    0, k_USECS_MIN,
                                                  -106751991, -14454775808LL },

    // Note that MAX + MIN is -1 day.

    { L_, k_DAYS_MAX, k_HOURS_MIN, k_MINS_MAX, k_SECS_MIN, 0, 0,
                                                          -2, -82859000000LL },
    { L_, k_DAYS_MAX, k_HOURS_MIN, k_MINS_MIN, k_SECS_MAX, 0, 0,
                                                          -2, -82741000000LL },
    { L_, k_DAYS_MIN, k_HOURS_MAX, k_MINS_MAX, k_SECS_MIN, 0, 0,
                                                          -1,  -3659000000LL },
    { L_, k_DAYS_MIN, k_HOURS_MAX, k_MINS_MIN, k_SECS_MAX, 0, 0,
                                                          -1,  -3541000000LL },

    { L_,     0, k_HOURS_MAX, k_MINS_MIN, k_SECS_MAX, k_MSECS_MIN, 0,
                                                          -2,  -3540999000LL },
    { L_,     0, k_HOURS_MAX, k_MINS_MIN, k_SECS_MIN, k_MSECS_MAX, 0,
                                                          -2,  -3539001000LL },
    { L_,     0, k_HOURS_MIN, k_MINS_MAX, k_SECS_MAX, k_MSECS_MIN, 0,
                                                          -1, -82860999000LL },
    { L_,     0, k_HOURS_MIN, k_MINS_MAX, k_SECS_MIN, k_MSECS_MAX, 0,
                                                          -1, -82859001000LL },
};

const int DEFAULT_NUM_DATA =
                  static_cast<int>(sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA);

// ============================================================================
//                                 TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(true == bsl::is_trivially_copyable<Obj>::value);

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

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

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
                                       ASSERT(  0 == i1.microseconds());
//..
// Then, set the value of 'i1' to -5 days, and then add 16 hours to that value:
//..
    i1.setTotalDays(-5);
    i1.addHours(16);                   ASSERT( -4 == i1.days());
                                       ASSERT( -8 == i1.hours());
                                       ASSERT(  0 == i1.minutes());
                                       ASSERT(  0 == i1.seconds());
                                       ASSERT(  0 == i1.milliseconds());
                                       ASSERT(  0 == i1.microseconds());
//..
// Next, create 'i2' as a copy of 'i1':
//..
    bdlt::DatetimeInterval i2(i1);     ASSERT( -4 == i2.days());
                                       ASSERT( -8 == i2.hours());
                                       ASSERT(  0 == i2.minutes());
                                       ASSERT(  0 == i2.seconds());
                                       ASSERT(  0 == i2.milliseconds());
                                       ASSERT(  0 == i2.microseconds());
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
                                       ASSERT(  0 == i2.microseconds());
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
//  -2_07:59:56.000000
//..

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING: hashAppend
        //
        // Concerns:
        //: 1 Hope that different inputs hash differently
        //: 2 Verify that equal inputs hash identically
        //: 3 Works for 'const' and non-'const' values
        //
        // Plan:
        //: 1 Use a table specifying a set of distinct objects, verify that
        //:   hashes of equivalent objects match and hashes on unequal objects
        //:   do not.
        //
        // Testing:
        //    void hashAppend(HASHALG&, const DatetimeInterval&);
        // --------------------------------------------------------------------
        if (verbose)
            cout << "\nTESTING 'hashAppend'"
                 << "\n====================\n";

        typedef ::BloombergLP::bslh::Hash<> Hasher;
        typedef Hasher::result_type         HashType;
        Hasher                              hasher;

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

            if (veryVerbose) { T_ P_(ILINE) P(ITOTAL_MSECS) }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int   JLINE        = DATA[tj].d_line;
                const Int64 JTOTAL_MSECS = DATA[tj].d_totalMsecs;

                if (veryVerbose) { T_ T_ P_(JLINE) P(JTOTAL_MSECS) }

                Obj mX;  const Obj& X = mX;
                mX.setTotalMilliseconds(ITOTAL_MSECS);

                Obj mY;  const Obj& Y = mY;
                mY.setTotalMilliseconds(JTOTAL_MSECS);

                HashType hX = hasher(X);
                HashType hY = hasher(Y);

                if (veryVerbose) { T_ T_ P_(JLINE) P_(hX) P(hY) }

                LOOP4_ASSERT(ILINE, JLINE, hX, hY, (ti == tj) == (hX == hY));
            }
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // UNARY MINUS OPERATOR
        //   Ensure that the result object is the negation of that of the
        //   operand.
        //
        // Concerns:
        //: 1 The free operator work as expected.
        //:
        //: 2 The signature and return type are standard.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For a sequence of independent test values, use the value
        //:   constructor to create an object of specified value.  Use the
        //:   free operator to compute a second object.  Verify the value of
        //:   this object by comparing to an object, created with the value
        //:   constructor, with the expected value.  (C-1)
        //:
        //: 2 Directly verify the function signature.  (C-2)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   DatetimeInterval operator-(const DatetimeInterval& value);
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

        if (verbose) cout << "\nTesting unary minus operators."
                          << endl;
        {
            const int                    NUM_DATA  = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   DAYS      = DATA[i].d_days;
                const Int64 HOURS     = DATA[i].d_hours;
                const Int64 MINUTES   = DATA[i].d_minutes;
                const Int64 SECONDS   = DATA[i].d_seconds;
                const Int64 MSECS     = DATA[i].d_msecs;
                const Int64 USECS     = DATA[i].d_usecs;

                Obj mX(DAYS,
                       HOURS,
                       MINUTES,
                       SECONDS,
                       MSECS,
                       USECS);

                const Obj& X = mX;

                if (X.days() > INT_MIN) {

                    Obj mEXP(-X.days(),
                             0,
                             0,
                             0,
                             0,
                             -X.fractionalDayInMicroseconds());

                    const Obj& EXP = mEXP;

                    if (veryVerbose) { T_ P_(X) P(EXP);  }

                    Obj mZ;  const Obj& Z = mZ;

                    mZ = -X;

                    ASSERTV(X, Z, EXP, EXP == Z);
                }
            }
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            ASSERT_SAFE_FAIL(-Obj(INT_MIN));
            ASSERT_SAFE_PASS(-Obj(INT_MAX));
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // ARITHMETIC FREE OPERATORS (+, -)
        //   Ensure that each operator correctly computes the returned object.
        //
        // Concerns:
        //: 1 The free operators work as expected.
        //:
        //: 2 The signatures and return types are standard.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For a sequence of independent test values, use the value
        //:   constructor to create two objects of specified value.  Use the
        //:   free operator to compute a third object.  Verify the value of
        //:   this object by comparing to an object, created with the value
        //:   constructor, with the expected value.  (C-1)
        //:
        //: 2 Directly verify the function signatures.  (C-2)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-3)
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

        if (verbose) cout << "\nTesting arithmetic free operators."
                          << endl;
        {
            const int                    NUM_DATA  = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   IDAYS      = DATA[i].d_days;
                const Int64 IHOURS     = DATA[i].d_hours;
                const Int64 IMINUTES   = DATA[i].d_minutes;
                const Int64 ISECONDS   = DATA[i].d_seconds;
                const Int64 IMSECS     = DATA[i].d_msecs;
                const Int64 IUSECS     = DATA[i].d_usecs;

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int   JDAYS      = DATA[j].d_days;
                    const Int64 JHOURS     = DATA[j].d_hours;
                    const Int64 JMINUTES   = DATA[j].d_minutes;
                    const Int64 JSECONDS   = DATA[j].d_seconds;
                    const Int64 JMSECS     = DATA[j].d_msecs;
                    const Int64 JUSECS     = DATA[j].d_usecs;

                    {
                        Int64 TOTAL_DAYS =
                                          static_cast<Int64>(DATA[i].d_expDays)
                                        + DATA[j].d_expDays;

                        // Note that the following test is slightly more
                        // restrictive than necessary.

                        if (TOTAL_DAYS > INT_MIN && TOTAL_DAYS < INT_MAX) {
                            Obj mX(IDAYS,
                                   IHOURS,
                                   IMINUTES,
                                   ISECONDS,
                                   IMSECS,
                                   IUSECS);

                            const Obj& X = mX;

                            Obj mY(JDAYS,
                                   JHOURS,
                                   JMINUTES,
                                   JSECONDS,
                                   JMSECS,
                                   JUSECS);

                            const Obj& Y = mY;

                            Obj mEXP(X.days() + Y.days(),
                                     0,
                                     0,
                                     0,
                                     0,
                                     X.fractionalDayInMicroseconds()
                                            + Y.fractionalDayInMicroseconds());

                            const Obj& EXP = mEXP;

                            if (veryVerbose) { T_ P_(X) P_(Y) P(EXP);  }

                            Obj mZ;  const Obj& Z = mZ;

                            mZ = X + Y;

                            ASSERTV(X, Z, EXP, EXP == Z);
                        }
                    }

                    {
                        Int64 TOTAL_DAYS =
                                          static_cast<Int64>(DATA[i].d_expDays)
                                        - DATA[j].d_expDays;

                        // Note that the following test is slightly more
                        // restrictive than necessary.

                        if (TOTAL_DAYS > INT_MIN && TOTAL_DAYS < INT_MAX) {
                            Obj mX(IDAYS,
                                   IHOURS,
                                   IMINUTES,
                                   ISECONDS,
                                   IMSECS,
                                   IUSECS);

                            const Obj& X = mX;

                            Obj mY(JDAYS,
                                   JHOURS,
                                   JMINUTES,
                                   JSECONDS,
                                   JMSECS,
                                   JUSECS);

                            const Obj& Y = mY;

                            Obj mEXP(X.days() - Y.days(),
                                     0,
                                     0,
                                     0,
                                     0,
                                     X.fractionalDayInMicroseconds()
                                            - Y.fractionalDayInMicroseconds());

                            const Obj& EXP = mEXP;

                            if (veryVerbose) { T_ P_(X) P_(Y) P(EXP);  }

                            Obj mZ;  const Obj& Z = mZ;

                            mZ = X - Y;

                            ASSERTV(X, Z, EXP, EXP == Z);
                        }
                    }
                }
            }
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            ASSERT_FAIL(Obj(INT_MIN) + Obj(-1));
            ASSERT_PASS(Obj(INT_MIN) + Obj( 0));
            ASSERT_PASS(Obj(INT_MIN) + Obj( 1));
            ASSERT_PASS(Obj(INT_MAX) + Obj(-1));
            ASSERT_PASS(Obj(INT_MAX) + Obj( 0));
            ASSERT_FAIL(Obj(INT_MAX) + Obj( 1));

            ASSERT_PASS(Obj(INT_MIN) - Obj(-1));
            ASSERT_PASS(Obj(INT_MIN) - Obj( 0));
            ASSERT_FAIL(Obj(INT_MIN) - Obj( 1));
            ASSERT_FAIL(Obj(INT_MAX) - Obj(-1));
            ASSERT_PASS(Obj(INT_MAX) - Obj( 0));
            ASSERT_PASS(Obj(INT_MAX) - Obj( 1));
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // ARITHMETIC ASSIGNMENT OPERATORS (+=, -=)
        //   Ensure that each operator correctly adjusts the underlying
        //   representation of the object.
        //
        // Concerns:
        //: 1 The manipulators work as expected, including for self-assignment.
        //:
        //: 2 The signatures and return types are standard.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For a sequence of independent test values, use the value
        //:   constructor to create two objects of specified value.  Use the
        //:   manipulator to adjust the value of the first obect.  Verify the
        //:   value by comparing to an object, created with the value
        //:   constructor, with the expected value.  (C-1)
        //:
        //: 2 Directly verify the function signatures.  (C-2)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-3)
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

        if (verbose) cout << "\nTesting arithmetic assignment operators."
                          << endl;
        {
            const int                    NUM_DATA  = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   IDAYS      = DATA[i].d_days;
                const Int64 IHOURS     = DATA[i].d_hours;
                const Int64 IMINUTES   = DATA[i].d_minutes;
                const Int64 ISECONDS   = DATA[i].d_seconds;
                const Int64 IMSECS     = DATA[i].d_msecs;
                const Int64 IUSECS     = DATA[i].d_usecs;

                { // Verify self-assignment works correctly.
                    {
                        Int64 TOTAL_DAYS =
                                     static_cast<Int64>(DATA[i].d_expDays) * 2;

                        // Note that the following test is slightly more
                        // restrictive than necessary.

                        if (TOTAL_DAYS > INT_MIN && TOTAL_DAYS < INT_MAX) {
                            Obj mX(IDAYS,
                                   IHOURS,
                                   IMINUTES,
                                   ISECONDS,
                                   IMSECS,
                                   IUSECS);

                            const Obj& X = mX;

                            Obj mEXP(X.days() * 2,
                                     0,
                                     0,
                                     0,
                                     0,
                                     X.fractionalDayInMicroseconds() * 2);

                            const Obj& EXP = mEXP;

                            if (veryVerbose) { T_ P_(X) P(EXP);  }

                            mX += X;

                            ASSERTV(X, EXP, EXP == X);
                        }
                    }

                    {
                        Obj mX(IDAYS,
                               IHOURS,
                               IMINUTES,
                               ISECONDS,
                               IMSECS,
                               IUSECS);

                        const Obj& X = mX;

                        Obj mEXP;  const Obj& EXP = mEXP;

                        if (veryVerbose) { T_ P_(X) P(EXP);  }

                        mX -= X;

                        ASSERTV(X, EXP, EXP == X);
                    }
                }

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int   JDAYS      = DATA[j].d_days;
                    const Int64 JHOURS     = DATA[j].d_hours;
                    const Int64 JMINUTES   = DATA[j].d_minutes;
                    const Int64 JSECONDS   = DATA[j].d_seconds;
                    const Int64 JMSECS     = DATA[j].d_msecs;
                    const Int64 JUSECS     = DATA[j].d_usecs;

                    {
                        Int64 TOTAL_DAYS =
                                          static_cast<Int64>(DATA[i].d_expDays)
                                        + DATA[j].d_expDays;

                        // Note that the following test is slightly more
                        // restrictive than necessary.

                        if (TOTAL_DAYS > INT_MIN && TOTAL_DAYS < INT_MAX) {
                            Obj mX(IDAYS,
                                   IHOURS,
                                   IMINUTES,
                                   ISECONDS,
                                   IMSECS,
                                   IUSECS);

                            const Obj& X = mX;

                            Obj mY(JDAYS,
                                   JHOURS,
                                   JMINUTES,
                                   JSECONDS,
                                   JMSECS,
                                   JUSECS);

                            const Obj& Y = mY;

                            Obj mEXP(X.days() + Y.days(),
                                     0,
                                     0,
                                     0,
                                     0,
                                     X.fractionalDayInMicroseconds()
                                            + Y.fractionalDayInMicroseconds());

                            const Obj& EXP = mEXP;

                            if (veryVerbose) { T_ P_(X) P_(Y) P(EXP);  }

                            mX += Y;

                            ASSERTV(X, EXP, EXP == X);
                        }
                    }

                    {
                        Int64 TOTAL_DAYS =
                                          static_cast<Int64>(DATA[i].d_expDays)
                                        - DATA[j].d_expDays;

                        // Note that the following test is slightly more
                        // restrictive than necessary.

                        if (TOTAL_DAYS > INT_MIN && TOTAL_DAYS < INT_MAX) {
                            Obj mX(IDAYS,
                                   IHOURS,
                                   IMINUTES,
                                   ISECONDS,
                                   IMSECS,
                                   IUSECS);

                            const Obj& X = mX;

                            Obj mY(JDAYS,
                                   JHOURS,
                                   JMINUTES,
                                   JSECONDS,
                                   JMSECS,
                                   JUSECS);

                            const Obj& Y = mY;

                            Obj mEXP(X.days() - Y.days(),
                                     0,
                                     0,
                                     0,
                                     0,
                                     X.fractionalDayInMicroseconds()
                                            - Y.fractionalDayInMicroseconds());

                            const Obj& EXP = mEXP;

                            if (veryVerbose) { T_ P_(X) P_(Y) P(EXP);  }

                            mX -= Y;

                            ASSERTV(X, EXP, EXP == X);
                        }
                    }
                }
            }
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            ASSERT_FAIL(Obj(INT_MIN) += Obj(-1));
            ASSERT_PASS(Obj(INT_MIN) += Obj( 0));
            ASSERT_PASS(Obj(INT_MIN) += Obj( 1));
            ASSERT_PASS(Obj(INT_MAX) += Obj(-1));
            ASSERT_PASS(Obj(INT_MAX) += Obj( 0));
            ASSERT_FAIL(Obj(INT_MAX) += Obj( 1));

            ASSERT_PASS(Obj(INT_MIN) -= Obj(-1));
            ASSERT_PASS(Obj(INT_MIN) -= Obj( 0));
            ASSERT_FAIL(Obj(INT_MIN) -= Obj( 1));
            ASSERT_FAIL(Obj(INT_MAX) -= Obj(-1));
            ASSERT_PASS(Obj(INT_MAX) -= Obj( 0));
            ASSERT_PASS(Obj(INT_MAX) -= Obj( 1));
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING ADDITIONAL 'add' MANIPULATORS
        //   Verify the 'add*' methods work as expected.
        //
        // Concerns:
        //: 1 The numerical constants used to generate the modified object
        //:   value are correct.
        //:
        //: 2 The correct object value is obtained.
        //
        //: 3 The expected value is returned.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For a set of independent test values, use the default
        //:   constructor to create an object and use the 'add' manipulators to
        //:   adjust its value.  Verify the values using the 'setInterval'
        //:   manipulator.  (C-1,2)
        //:
        //: 2 At each step in P-1 compare the address of the objected
        //:   referenced by the return value to the address of the object under
        //:   test.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   void addDays(int days);
        //   void addHours(Int64 hours);
        //   void addMinutes(Int64 minutes);
        //   void addSeconds(Int64 seconds);
        //   void addMilliseconds(Int64 milliseconds);
        //   void addMicroseconds(Int64 microseconds);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ADDITIONAL 'add' MANIPULATORS" << endl
                          << "=====================================" << endl;

        if (verbose) cout << "\nTesting 'addXXX' methods." << endl;
        {
            {
                const char *testing = "'addDays'";
                int         DATA[] = { -1, 0, 1 };
                bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

                for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                    Obj mX;  const Obj& X = mX;
                    Obj mY;  const Obj& Y = mY;

                    Obj& RETVAL = mX.addDays(DATA[i]);
                    mY.setInterval(DATA[i]);
                    ASSERTV(testing, DATA[i], X == Y);

                    if (veryVeryVerbose) { T_ T_ Q(return value: Days A) }
                    ASSERTV(X, &RETVAL, &X, &RETVAL == &X);

                    RETVAL = mX.addDays(DATA[i]);
                    mY.setInterval(DATA[i] * 2);
                    ASSERTV(testing, DATA[i], X == Y);

                    if (veryVeryVerbose) { T_ T_ Q(return value: Days B) }
                    ASSERTV(X, &RETVAL, &X, &RETVAL == &X);
                }
            }

            {
                const char *testing = "'addHours'";
                Int64       DATA[] = { -1, 0, 1 };
                bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

                for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                    Obj mX;  const Obj& X = mX;
                    Obj mY;  const Obj& Y = mY;

                    Obj& RETVAL = mX.addHours(DATA[i]);
                    mY.setInterval(0, DATA[i]);
                    ASSERTV(testing, DATA[i], X == Y);

                    if (veryVeryVerbose) { T_ T_ Q(return value: Hour A) }
                    ASSERTV(X, &RETVAL, &X, &RETVAL == &X);

                    RETVAL = mX.addHours(DATA[i]);
                    mY.setInterval(0, DATA[i] * 2);
                    ASSERTV(testing, DATA[i], X == Y);

                    if (veryVeryVerbose) { T_ T_ Q(return value: Hour B) }
                    ASSERTV(X, &RETVAL, &X, &RETVAL == &X);
                }
            }

            {
                const char *testing = "'addMinutes'";
                Int64       DATA[] = { -1, 0, 1 };
                bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

                for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                    Obj mX;  const Obj& X = mX;
                    Obj mY;  const Obj& Y = mY;

                    Obj& RETVAL = mX.addMinutes(DATA[i]);
                    mY.setInterval(0, 0, DATA[i]);
                    ASSERTV(testing, DATA[i], X == Y);

                    if (veryVeryVerbose) { T_ T_ Q(return value: Mins A) }
                    ASSERTV(X, &RETVAL, &X, &RETVAL == &X);

                    RETVAL = mX.addMinutes(DATA[i]);
                    mY.setInterval(0, 0, DATA[i] * 2);
                    ASSERTV(testing, DATA[i], X == Y);

                    if (veryVeryVerbose) { T_ T_ Q(return value: Mins B) }
                    ASSERTV(X, &RETVAL, &X, &RETVAL == &X);
                }
            }

            {
                const char *testing = "'addSeconds'";
                Int64       DATA[] = { -1, 0, 1 };
                bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

                for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                    Obj mX;  const Obj& X = mX;
                    Obj mY;  const Obj& Y = mY;

                    Obj& RETVAL = mX.addSeconds(DATA[i]);
                    mY.setInterval(0, 0, 0, DATA[i]);
                    ASSERTV(testing, DATA[i], X == Y);

                    if (veryVeryVerbose) { T_ T_ Q(return value: Secs A) }
                    ASSERTV(X, &RETVAL, &X, &RETVAL == &X);

                    RETVAL = mX.addSeconds(DATA[i]);
                    mY.setInterval(0, 0, 0, DATA[i] * 2);
                    ASSERTV(testing, DATA[i], X == Y);

                    if (veryVeryVerbose) { T_ T_ Q(return value: Secs B) }
                    ASSERTV(X, &RETVAL, &X, &RETVAL == &X);
                }
            }

            {
                const char *testing = "'addMilliseconds'";
                Int64       DATA[] = { -1, 0, 1 };
                bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

                for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                    Obj mX;  const Obj& X = mX;
                    Obj mY;  const Obj& Y = mY;

                    Obj& RETVAL = mX.addMilliseconds(DATA[i]);
                    mY.setInterval(0, 0, 0, 0, DATA[i]);
                    ASSERTV(testing, DATA[i], X == Y);

                    if (veryVeryVerbose) { T_ T_ Q(return value: Msec A) }
                    ASSERTV(X, &RETVAL, &X, &RETVAL == &X);

                    RETVAL = mX.addMilliseconds(DATA[i]);
                    mY.setInterval(0, 0, 0, 0, DATA[i] * 2);
                    ASSERTV(testing, DATA[i], X == Y);

                    if (veryVeryVerbose) { T_ T_ Q(return value: Msec B) }
                    ASSERTV(X, &RETVAL, &X, &RETVAL == &X);
                }
            }

            {
                const char *testing = "'addMicroseconds'";
                Int64       DATA[] = { -1, 0, 1 };
                bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

                for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                    Obj mX;  const Obj& X = mX;
                    Obj mY;  const Obj& Y = mY;

                    Obj& RETVAL = mX.addMicroseconds(DATA[i]);
                    mY.setInterval(0, 0, 0, 0, 0, DATA[i]);
                    ASSERTV(testing, DATA[i], X == Y);

                    if (veryVeryVerbose) { T_ T_ Q(return value: Usec A) }
                    ASSERTV(X, &RETVAL, &X, &RETVAL == &X);

                    RETVAL = mX.addMicroseconds(DATA[i]);
                    mY.setInterval(0, 0, 0, 0, 0, DATA[i] * 2);
                    ASSERTV(testing, DATA[i], X == Y);

                    if (veryVeryVerbose) { T_ T_ Q(return value: Usec B) }
                    ASSERTV(X, &RETVAL, &X, &RETVAL == &X);
                }
            }
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            ASSERT_FAIL(Obj(INT_MIN, -23, -59, -59, -999, -999).addDays(-1));
            ASSERT_PASS(Obj(INT_MIN, -23, -59, -59, -999, -999).addDays( 0));
            ASSERT_PASS(Obj(INT_MIN, -23, -59, -59, -999, -999).addDays( 1));
            ASSERT_PASS(Obj(INT_MAX,  23,  59,  59,  999,  999).addDays(-1));
            ASSERT_PASS(Obj(INT_MAX,  23,  59,  59,  999,  999).addDays( 0));
            ASSERT_FAIL(Obj(INT_MAX,  23,  59,  59,  999,  999).addDays( 1));

            ASSERT_FAIL(Obj(INT_MIN, -23, -59, -59, -999, -999).addHours(-1));
            ASSERT_PASS(Obj(INT_MIN, -23, -59, -59, -999, -999).addHours( 0));
            ASSERT_PASS(Obj(INT_MIN, -23, -59, -59, -999, -999).addHours( 1));
            ASSERT_PASS(Obj(INT_MAX,  23,  59,  59,  999,  999).addHours(-1));
            ASSERT_PASS(Obj(INT_MAX,  23,  59,  59,  999,  999).addHours( 0));
            ASSERT_FAIL(Obj(INT_MAX,  23,  59,  59,  999,  999).addHours( 1));

            ASSERT_FAIL(
                       Obj(INT_MIN, -23, -59, -59, -999, -999).addMinutes(-1));
            ASSERT_PASS(
                       Obj(INT_MIN, -23, -59, -59, -999, -999).addMinutes( 0));
            ASSERT_PASS(
                       Obj(INT_MIN, -23, -59, -59, -999, -999).addMinutes( 1));
            ASSERT_PASS(
                       Obj(INT_MAX,  23,  59,  59,  999,  999).addMinutes(-1));
            ASSERT_PASS(
                       Obj(INT_MAX,  23,  59,  59,  999,  999).addMinutes( 0));
            ASSERT_FAIL(
                       Obj(INT_MAX,  23,  59,  59,  999,  999).addMinutes( 1));

            ASSERT_FAIL(
                       Obj(INT_MIN, -23, -59, -59, -999, -999).addSeconds(-1));
            ASSERT_PASS(
                       Obj(INT_MIN, -23, -59, -59, -999, -999).addSeconds( 0));
            ASSERT_PASS(
                       Obj(INT_MIN, -23, -59, -59, -999, -999).addSeconds( 1));
            ASSERT_PASS(
                       Obj(INT_MAX,  23,  59,  59,  999,  999).addSeconds(-1));
            ASSERT_PASS(
                       Obj(INT_MAX,  23,  59,  59,  999,  999).addSeconds( 0));
            ASSERT_FAIL(
                       Obj(INT_MAX,  23,  59,  59,  999,  999).addSeconds( 1));

            ASSERT_FAIL(
                  Obj(INT_MIN, -23, -59, -59, -999, -999).addMilliseconds(-1));
            ASSERT_PASS(
                  Obj(INT_MIN, -23, -59, -59, -999, -999).addMilliseconds( 0));
            ASSERT_PASS(
                  Obj(INT_MIN, -23, -59, -59, -999, -999).addMilliseconds( 1));
            ASSERT_PASS(
                  Obj(INT_MAX,  23,  59,  59,  999,  999).addMilliseconds(-1));
            ASSERT_PASS(
                  Obj(INT_MAX,  23,  59,  59,  999,  999).addMilliseconds( 0));
            ASSERT_FAIL(
                  Obj(INT_MAX,  23,  59,  59,  999,  999).addMilliseconds( 1));

            ASSERT_FAIL(
                  Obj(INT_MIN, -23, -59, -59, -999, -999).addMicroseconds(-1));
            ASSERT_PASS(
                  Obj(INT_MIN, -23, -59, -59, -999, -999).addMicroseconds( 0));
            ASSERT_PASS(
                  Obj(INT_MIN, -23, -59, -59, -999, -999).addMicroseconds( 1));
            ASSERT_PASS(
                  Obj(INT_MAX,  23,  59,  59,  999,  999).addMicroseconds(-1));
            ASSERT_PASS(
                  Obj(INT_MAX,  23,  59,  59,  999,  999).addMicroseconds( 0));
            ASSERT_FAIL(
                  Obj(INT_MAX,  23,  59,  59,  999,  999).addMicroseconds( 1));
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'addInterval'
        //   Verify the manipulator work as expected.
        //
        // Concerns:
        //: 1 The separate time fields must be multiplied by the appropriate
        //:   factors to convert the six-parameter input representation to the
        //:   internal representation and added to the current value, with
        //:   appropriate handling of potential overflow.
        //:
        //: 2 The expected value is returned.
        //:
        //: 3 The default values are correctly defined.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For a sequence of independent test values, use the value
        //:   constructor to create an object of a specified value.  Add an
        //:   interval to this value using the manipulator.  Verify the value
        //:   using the basic accessors.  (C-1)
        //:
        //: 2 At each step in P-1 compare the address of the objected
        //:   referenced by the return value to the address of the object under
        //:   test.  (C-2)
        //:
        //: 3 Directly verify the value of defaulted arguments.  (C-3)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   void addInterval(int d, Int64 h = 0, m = 0, s = 0, ms = 0);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'addInterval'" << endl
                 << "=====================" << endl;
        }

        if (verbose) cout << "\nTesting 'addInterval'." << endl;
        {
            const int                    NUM_DATA  = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   IDAYS      = DATA[i].d_days;
                const Int64 IHOURS     = DATA[i].d_hours;
                const Int64 IMINUTES   = DATA[i].d_minutes;
                const Int64 ISECONDS   = DATA[i].d_seconds;
                const Int64 IMSECS     = DATA[i].d_msecs;
                const Int64 IUSECS     = DATA[i].d_usecs;

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int   JDAYS      = DATA[j].d_days;
                    const Int64 JHOURS     = DATA[j].d_hours;
                    const Int64 JMINUTES   = DATA[j].d_minutes;
                    const Int64 JSECONDS   = DATA[j].d_seconds;
                    const Int64 JMSECS     = DATA[j].d_msecs;
                    const Int64 JUSECS     = DATA[j].d_usecs;

                    Int64 TOTAL_DAYS = static_cast<Int64>(DATA[i].d_expDays)
                                     + DATA[j].d_expDays;

                    // Note that the following test is slightly more
                    // restrictive than necessary.

                    if (TOTAL_DAYS > INT_MIN && TOTAL_DAYS < INT_MAX) {
                        Obj mX(IDAYS,
                               IHOURS,
                               IMINUTES,
                               ISECONDS,
                               IMSECS,
                               IUSECS);

                        const Obj& X = mX;

                        Obj mY(JDAYS,
                               JHOURS,
                               JMINUTES,
                               JSECONDS,
                               JMSECS,
                               JUSECS);

                        const Obj& Y = mY;

                        Obj mEXP(X.days() + Y.days(),
                                 0,
                                 0,
                                 0,
                                 0,
                                 X.fractionalDayInMicroseconds()
                                            + Y.fractionalDayInMicroseconds());

                        const Obj& EXP = mEXP;

                        if (veryVerbose) { T_ P_(X) P_(Y) P(EXP);  }

                        Obj& RETVAL = mX.addInterval(JDAYS,
                                                     JHOURS,
                                                     JMINUTES,
                                                     JSECONDS,
                                                     JMSECS,
                                                     JUSECS);

                        ASSERTV(X, Y, EXP, EXP == X);

                        if (veryVeryVerbose) {
                            T_ T_ Q(Check return value)
                        }
                        ASSERTV(X, &RETVAL, &X, &RETVAL == &X);
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting default values." << endl;
        {
            {
                Obj mX;  const Obj& X = mX;

                mX.addInterval(1, 1, 1, 1, 1);

                ASSERT(1 == X.days());
                ASSERT(1 == X.hours());
                ASSERT(1 == X.minutes());
                ASSERT(1 == X.seconds());
                ASSERT(1 == X.milliseconds());
                ASSERT(0 == X.microseconds());
            }
            {
                Obj mX;  const Obj& X = mX;

                mX.addInterval(1, 1, 1, 1);

                ASSERT(1 == X.days());
                ASSERT(1 == X.hours());
                ASSERT(1 == X.minutes());
                ASSERT(1 == X.seconds());
                ASSERT(0 == X.milliseconds());
                ASSERT(0 == X.microseconds());
            }
            {
                Obj mX;  const Obj& X = mX;

                mX.addInterval(1, 1, 1);

                ASSERT(1 == X.days());
                ASSERT(1 == X.hours());
                ASSERT(1 == X.minutes());
                ASSERT(0 == X.seconds());
                ASSERT(0 == X.milliseconds());
                ASSERT(0 == X.microseconds());
            }
            {
                Obj mX;  const Obj& X = mX;

                mX.addInterval(1, 1);

                ASSERT(1 == X.days());
                ASSERT(1 == X.hours());
                ASSERT(0 == X.minutes());
                ASSERT(0 == X.seconds());
                ASSERT(0 == X.milliseconds());
                ASSERT(0 == X.microseconds());
            }
            {
                Obj mX;  const Obj& X = mX;

                mX.addInterval(1);

                ASSERT(1 == X.days());
                ASSERT(0 == X.hours());
                ASSERT(0 == X.minutes());
                ASSERT(0 == X.seconds());
                ASSERT(0 == X.milliseconds());
                ASSERT(0 == X.microseconds());
            }
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            ASSERT_PASS(Obj().addInterval(0));

            ASSERT_PASS(Obj().addInterval(k_DAYS_MAX));
            ASSERT_FAIL(Obj().addInterval(k_DAYS_MAX, 24));

            ASSERT_PASS(Obj().addInterval(k_DAYS_MIN));
            ASSERT_FAIL(Obj().addInterval(k_DAYS_MIN, -24));

            ASSERT_FAIL(Obj().addInterval(0, k_HOURS_MAX, k_MINS_MAX));
            ASSERT_PASS(Obj().addInterval(0, k_HOURS_MAX, 0));
            ASSERT_PASS(Obj().addInterval(0, k_HOURS_MAX, k_MINS_MIN));

            ASSERT_PASS(Obj().addInterval(0, k_HOURS_MIN, k_MINS_MAX));
            ASSERT_PASS(Obj().addInterval(0, k_HOURS_MIN, 0));
            ASSERT_FAIL(Obj().addInterval(0, k_HOURS_MIN, k_MINS_MIN));
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS
        //   Verify the accessors work as expected.
        //
        // Concerns:
        //: 1 Each accessor performs the appropriate arithmetic to convert
        //:   the internal representation.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For each of a sequence of unique object values, verify that each
        //:   of the basic accessors returns the correct value.  (C-1)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   int hours() const;
        //   int minutes() const;
        //   int seconds() const;
        //   int milliseconds() const;
        //   int microseconds() const;
        //   int totalDays() const;
        //   Int64 totalHours() const;
        //   Int64 totalMinutes() const;
        //   Int64 totalSeconds() const;
        //   double totalSecondsAsDouble() const;
        //   Int64 totalMilliseconds() const;
        //   Int64 totalMicroseconds() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ACCESSORS" << endl
                          << "=================" << endl;

        {
            static const struct {
                int   d_line;
                int   d_days;
                Int64 d_hours;
                Int64 d_minutes;
                Int64 d_seconds;
                Int64 d_msecs;
                Int64 d_usecs;
            } DATA[] = {
                //LN  D   H   M   S   MS   US
                //--  --  --  --  --  ---  ---
                { L_,  0,  0,  0,  0,   0,   0 },
                { L_,  1,  2,  3,  4,   5,   6 },
                { L_, -1, -2, -3, -4,  -5,  -6 }
            };
            const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                const int   LINE      = DATA[i].d_line;
                const int   DAYS      = DATA[i].d_days;
                const Int64 HOURS     = DATA[i].d_hours;
                const Int64 MINUTES   = DATA[i].d_minutes;
                const Int64 SECONDS   = DATA[i].d_seconds;
                const Int64 MSECS     = DATA[i].d_msecs;
                const Int64 USECS     = DATA[i].d_usecs;

                Obj        mX(DAYS, HOURS, MINUTES, SECONDS, MSECS, USECS);
                const Obj& X = mX;

                if (veryVerbose) { T_ P(X); }

                LOOP_ASSERT(LINE, HOURS   == X.hours());
                LOOP_ASSERT(LINE, MINUTES == X.minutes());
                LOOP_ASSERT(LINE, SECONDS == X.seconds());
                LOOP_ASSERT(LINE, MSECS   == X.milliseconds());
                LOOP_ASSERT(LINE, USECS   == X.microseconds());

                LOOP_ASSERT(LINE, DAYS == X.totalDays());

                LOOP_ASSERT(LINE,
                            X.totalDays() * 24 + HOURS == X.totalHours());

                LOOP_ASSERT(LINE,
                            X.totalHours() * 60 + MINUTES == X.totalMinutes());

                LOOP_ASSERT(LINE,
                            X.totalMinutes() * 60 + SECONDS ==
                                                             X.totalSeconds());

                LOOP_ASSERT(LINE,
                            X.totalSeconds() * 1000 + MSECS ==
                                                        X.totalMilliseconds());

                LOOP_ASSERT(LINE,
                            X.totalMilliseconds() * 1000 + USECS ==
                                                        X.totalMicroseconds());

                const volatile double DBL_SECS1 = static_cast<double>(
                              X.totalMicroseconds()) / (1.0 * k_USECS_PER_SEC);

                const volatile double DBL_SECS2 = X.totalSecondsAsDouble();

                LOOP_ASSERT(LINE, DBL_SECS1 == DBL_SECS2);
            }
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            ASSERT_SAFE_PASS(Obj(0).totalMicroseconds());

            ASSERT_SAFE_FAIL(Obj(0,
                                 0,
                                 0,
                                 0,
                                 k_USECS_MAX / 1000,
                                 k_USECS_MAX % 1000 + 1).totalMicroseconds());
            ASSERT_SAFE_PASS(Obj(0,
                                 0,
                                 0,
                                 0,
                                 k_USECS_MAX / 1000,
                                 k_USECS_MAX % 1000    ).totalMicroseconds());
            ASSERT_SAFE_PASS(Obj(0,
                                 0,
                                 0,
                                 0,
                                 k_USECS_MAX / 1000,
                                 k_USECS_MAX % 1000 - 1).totalMicroseconds());

            ASSERT_SAFE_PASS(Obj(0,
                                 0,
                                 0,
                                 0,
                                 k_USECS_MIN / 1000,
                                 k_USECS_MIN % 1000 + 1).totalMicroseconds());
            ASSERT_SAFE_PASS(Obj(0,
                                 0,
                                 0,
                                 0,
                                 k_USECS_MIN / 1000,
                                 k_USECS_MIN % 1000    ).totalMicroseconds());

#if !defined(BSLS_PLATFORM_CMP_SUN) \
 || !defined(BDE_BUILD_TARGET_OPT) \
 || BSLS_PLATFORM_CMP_VERSION >= 0x5140

            // Older versions of the Sun compiler (e.g., 5.12.3 and 5.12.4)
            // fail to compile the 'BSLS_ASSERT' corresponding to the
            // following 'ASSERT_SAFE_FAIL' correctly in optimized builds.

            ASSERT_SAFE_FAIL(Obj(0,
                                 0,
                                 0,
                                 0,
                                 k_USECS_MIN / 1000,
                                 k_USECS_MIN % 1000 - 1).totalMicroseconds());

#endif
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING RELATIONAL OPERATORS
        //   Verify the relational operators evaluate correctly.
        //
        // Concerns:
        //: 1 Each operator implements the corresponding operators on the
        //:   underlying attributes correctly.
        //
        // Plan:
        //: 1 Specify an ordered set 'S' of unique object values.  For each
        //:   '(u, v)' in the set 'S x S', verify the result of 'u OP v' for
        //:   each 'OP' in '{<, <=, >=, >}'.  (C-1)
        //
        // Testing:
        //   bool operator< (const DatetimeInterval& lhs, rhs);
        //   bool operator<=(const DatetimeInterval& lhs, rhs);
        //   bool operator> (const DatetimeInterval& lhs, rhs);
        //   bool operator>=(const DatetimeInterval& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING RELATIONAL OPERATORS" << endl
                          << "============================" << endl;

        if (verbose) {
            cout << "\nTesting 'operator<', 'operator<=', 'operator>=', "
                 << "and 'operator>'." << endl;
        }

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

        const int                    NUM_DATA  = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   ILINE      = DATA[i].d_line;
            const int   IDAYS      = DATA[i].d_days;
            const Int64 IHOURS     = DATA[i].d_hours;
            const Int64 IMINUTES   = DATA[i].d_minutes;
            const Int64 ISECONDS   = DATA[i].d_seconds;
            const Int64 IMSECS     = DATA[i].d_msecs;
            const Int64 IUSECS     = DATA[i].d_usecs;

            // Ensure an object compares correctly with itself (alias test).
            {
                Obj mW;  const Obj& W = mW;

                mW.setInterval(IDAYS,
                               IHOURS,
                               IMINUTES,
                               ISECONDS,
                               IMSECS,
                               IUSECS);

                if (veryVerbose) { T_ P_(ILINE) P(W) }

                LOOP2_ASSERT(ILINE, W, !(W <  W));
                LOOP2_ASSERT(ILINE, W,   W <= W);
                LOOP2_ASSERT(ILINE, W, !(W >  W));
                LOOP2_ASSERT(ILINE, W,   W >= W);

                // Ensure the first row of the table contains the
                // default-constructed value.

                static bool firstFlag = true;
                if (firstFlag) {
                    LOOP3_ASSERT(ILINE, Obj(), W, Obj() == W)
                    firstFlag = false;
                }
            }

            for (int j = 0; j < NUM_DATA; ++j) {
                const int   JLINE      = DATA[j].d_line;
                const int   JDAYS      = DATA[j].d_days;
                const Int64 JHOURS     = DATA[j].d_hours;
                const Int64 JMINUTES   = DATA[j].d_minutes;
                const Int64 JSECONDS   = DATA[j].d_seconds;
                const Int64 JMSECS     = DATA[j].d_msecs;
                const Int64 JUSECS     = DATA[j].d_usecs;

                Obj mX;  const Obj& X = mX;

                mX.setInterval(IDAYS,
                               IHOURS,
                               IMINUTES,
                               ISECONDS,
                               IMSECS,
                               IUSECS);

                Obj mY;  const Obj& Y = mY;

                mY.setInterval(JDAYS,
                               JHOURS,
                               JMINUTES,
                               JSECONDS,
                               JMSECS,
                               JUSECS);

                if (veryVerbose) { T_ P_(JLINE) P(Y) }

                const bool LT = (   X.days() < Y.days()
                                 || (   X.days() == Y.days()
                                     && X.fractionalDayInMicroseconds()
                                           < Y.fractionalDayInMicroseconds()));
                const bool LE = (X == Y) || LT;
                const bool GT = !LE;
                const bool GE = !LT;

                if (veryVerbose) { T_ T_ T_ P_(X) P(Y) }

                // Verify value.

                LOOP4_ASSERT(ILINE, JLINE, X, Y, LT == (X <  Y));
                LOOP4_ASSERT(ILINE, JLINE, Y, X, LE == (X <= Y));
                LOOP4_ASSERT(ILINE, JLINE, Y, X, GT == (X >  Y));
                LOOP4_ASSERT(ILINE, JLINE, Y, X, GE == (X >= Y));
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING ADDITIONAL 'set' MANIPULATORS
        //   Verify the 'set*' methods work as expected.
        //
        // Concerns:
        //: 1 The numerical constants used to generate the modified object
        //:   value are correct.
        //:
        //: 2 The correct object value is obtained.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For a set of independent test values, use the default
        //:   constructor to create an object and use the 'set' manipulators to
        //:   set its value.  Verify the values using the 'setInterval'
        //:   manipulator.  (C-1,2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   void setTotalDays(int days);
        //   void setTotalHours(Int64 hours);
        //   void setTotalMinutes(Int64 minutes);
        //   void setTotalSeconds(Int64 seconds);
        //   void setTotalSecondsFromDouble(double seconds);
        //   void setTotalMilliseconds(Int64 milliseconds);
        //   void setTotalMicroseconds(Int64 microseconds);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ADDITIONAL 'set' MANIPULATORS" << endl
                          << "=====================================" << endl;

        if (verbose) cout << "\nTesting 'setXXX' methods." << endl;
        {
            Obj mX;  const Obj& X = mX;
            Obj mY;  const Obj& Y = mY;

            {
                const char *testing = "'setTotalDays'";
                int         DATA[] = { INT_MIN, -1, 0, 1, INT_MAX };
                bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

                for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                    mX.setTotalDays(DATA[i]);
                    mY.setInterval(DATA[i]);
                    ASSERTV(testing, DATA[i], X == Y);
                }
            }

            {
                const char *testing = "'setTotalHours'";
                Int64       DATA[] = { k_HOURS_MIN, -1, 0, 1, k_HOURS_MAX };
                bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

                for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                    mX.setTotalHours(DATA[i]);
                    mY.setInterval(0, DATA[i]);
                    ASSERTV(testing, DATA[i], X == Y);
                }
            }

            {
                const char *testing = "'setTotalMinutes'";
                Int64       DATA[] = { k_MINS_MIN, -1, 0, 1, k_MINS_MAX };
                bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

                for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                    mX.setTotalMinutes(DATA[i]);
                    mY.setInterval(0, 0, DATA[i]);
                    ASSERTV(testing, DATA[i], X == Y);
                }
            }

            {
                const char *testing = "'setTotalSeconds(Int64)'";
                Int64       DATA[] = { k_SECS_MIN, -1, 0, 1, k_SECS_MAX };
                bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

                for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                    mX.setTotalSeconds(DATA[i]);
                    mY.setInterval(0, 0, 0, DATA[i]);
                    ASSERTV(testing, DATA[i], X == Y);
                }
            }

            {
                const char *testing = "'setTotalSecondsFromDouble(double)'";

                static const double k_DELTA = 1.0e-20;
                    // Small delta that double can represent around 1.0e-6

                static const struct {
                    int    d_line;          // source line number
                    double d_secsFrom;
                    Int64  d_secsExpected;
                    Int64  d_usecsExpected;
                } DATA[] = {
                    //LN         SECONDS FROM (DOUBLE)        EXP_SEC   EXP_US
                    //--  --------------------------------  ----------- ------
                    // Rounding tests
                    { L_, -1.0 / k_USECS_PER_SEC - k_DELTA,  0,          -1  },
                    { L_, -1.0 / k_USECS_PER_SEC,            0,          -1  },
                    { L_, -1.0 / k_USECS_PER_SEC + k_DELTA,  0,          -1  },
                    { L_, -0.5 / k_USECS_PER_SEC - k_DELTA,  0,          -1  },
                    { L_, -0.5 / k_USECS_PER_SEC,            0,          -1  },
                    { L_, -0.5 / k_USECS_PER_SEC + k_DELTA,  0,           0  },
                    { L_, -0.0 / k_USECS_PER_SEC - k_DELTA,  0,           0  },
                    { L_, -0.0 / k_USECS_PER_SEC,            0,           0  },
                    { L_,  0.0 / k_USECS_PER_SEC,            0,           0  },
                    { L_,  0.0 / k_USECS_PER_SEC + k_DELTA,  0,           0  },
                    { L_,  0.5 / k_USECS_PER_SEC - k_DELTA,  0,           0  },
                    { L_,  0.5 / k_USECS_PER_SEC,            0,           1  },
                    { L_,  0.5 / k_USECS_PER_SEC + k_DELTA,  0,           1  },
                    { L_,  1.0 / k_USECS_PER_SEC - k_DELTA,  0,           1  },
                    { L_,  1.0 / k_USECS_PER_SEC,            0,           1  },
                    { L_,  1.0 / k_USECS_PER_SEC + k_DELTA,  0,           1  },

                    // Largest value capable of maintaining 1us properly
                    { L_, -8589934591.000001,              -8589934591LL, -1 },
                    { L_,  8589934591.000001,               8589934591LL,  1 },

                    // Largest value that can be stored in DateTimeInterval
                    { L_, k_SECS_MIN,                        k_SECS_MIN,  0  },
                    { L_, k_SECS_MAX,                        k_SECS_MAX,  0  },
                };
                bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

                for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                    const int LINE = DATA[i].d_line;
                    mX.setTotalSecondsFromDouble(DATA[i].d_secsFrom);
                    mY.setInterval(0, 0, 0, DATA[i].d_secsExpected, 0,
                                                      DATA[i].d_usecsExpected);
                    ASSERTV(testing, LINE, X, Y, X == Y);
                }
            }

            {
                const char *testing = "'setTotalMilliseconds'";
                Int64       DATA[] = { k_MSECS_MIN, -1, 0, 1, k_MSECS_MAX };
                bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

                for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                    mX.setTotalMilliseconds(DATA[i]);
                    mY.setInterval(0, 0, 0, 0, DATA[i]);
                    ASSERTV(testing, DATA[i], X == Y);
                }
            }

            {
                const char *testing = "'setTotalMicroseconds'";
                Int64       DATA[] = { k_USECS_MIN, -1, 0, 1, k_USECS_MAX };
                bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

                for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                    mX.setTotalMicroseconds(DATA[i]);
                    mY.setInterval(0, 0, 0, 0, 0, DATA[i]);
                    ASSERTV(testing, DATA[i], X == Y);
                }
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZING CONSTRUCTOR
        //   Verify the initializing constructor works as expected.
        //
        // Concerns:
        //: 1 The separate time fields must be multiplied by the appropriate
        //:   factors to convert the six-parameter input representation to the
        //:   internal representation, with appropriate handling of potential
        //:   overflow.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For a sequence of independent test values, use the value
        //:   constructor to create an object.  Verify the value using the
        //:   basic accessors.  (C-1)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   DatetimeInterval(int d, Int64 h = 0, m = 0, s = 0, ms = 0);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING PRIMARY MANIPULATORS (BOOTSTRAP)" << endl
                 << "========================================" << endl;
        }

        if (verbose) cout << "\nTesting value constructor." << endl;
        {
            const int                    NUM_DATA  = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE      = DATA[i].d_line;
                const int   DAYS      = DATA[i].d_days;
                const Int64 HOURS     = DATA[i].d_hours;
                const Int64 MINUTES   = DATA[i].d_minutes;
                const Int64 SECONDS   = DATA[i].d_seconds;
                const Int64 MSECS     = DATA[i].d_msecs;
                const Int64 USECS     = DATA[i].d_usecs;
                const int   EXP_DAYS  = DATA[i].d_expDays;
                const Int64 EXP_USECS = DATA[i].d_expUsecs;

                Obj        x(DAYS, HOURS, MINUTES, SECONDS, MSECS, USECS);
                const Obj& X = x;

                if (veryVerbose) {
                    T_;
                    P_(DAYS);
                    P_(HOURS);
                    P_(MINUTES);
                    P_(SECONDS);
                    P_(MSECS);
                    P_(USECS);
                    P(X);
                }

                LOOP_ASSERT(LINE, EXP_DAYS == X.days());
                LOOP_ASSERT(LINE,
                            EXP_USECS == X.fractionalDayInMicroseconds());

                if (0 == USECS) {
                    Obj        x1(DAYS, HOURS, MINUTES, SECONDS, MSECS);
                    const Obj& X1 = x1;

                    LOOP_ASSERT(LINE, EXP_DAYS == X1.days());
                    LOOP_ASSERT(LINE,
                                EXP_USECS == X1.fractionalDayInMicroseconds());

                    if (0 == MSECS) {
                        Obj        x2(DAYS, HOURS, MINUTES, SECONDS);
                        const Obj& X2 = x2;

                        LOOP_ASSERT(LINE, EXP_DAYS == X2.days());
                        LOOP_ASSERT(LINE,
                                    EXP_USECS ==
                                             X2.fractionalDayInMicroseconds());

                        if (0 == SECONDS) {
                            Obj        x3(DAYS, HOURS, MINUTES);
                            const Obj& X3 = x3;

                            LOOP_ASSERT(LINE, EXP_DAYS == X3.days());
                            LOOP_ASSERT(LINE,
                                        EXP_USECS ==
                                             X3.fractionalDayInMicroseconds());

                            if (0 == MINUTES) {
                                Obj        x4(DAYS, HOURS);
                                const Obj& X4 = x4;

                                LOOP_ASSERT(LINE, EXP_DAYS == X4.days());
                                LOOP_ASSERT(LINE,
                                            EXP_USECS ==
                                             X4.fractionalDayInMicroseconds());

                                if (0 == HOURS) {
                                    Obj        x5(DAYS);
                                    const Obj& X5 = x5;

                                    LOOP_ASSERT(LINE, EXP_DAYS == X5.days());
                                    LOOP_ASSERT(LINE,
                                                EXP_USECS ==
                                             X5.fractionalDayInMicroseconds());
                                }
                            }
                        }
                    }
                }
            }
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            ASSERT_PASS(Obj(0));

            ASSERT_PASS(Obj(k_DAYS_MAX));
            ASSERT_FAIL(Obj(k_DAYS_MAX, 24));

            ASSERT_PASS(Obj(k_DAYS_MIN));
            ASSERT_FAIL(Obj(k_DAYS_MIN, -24));

            ASSERT_FAIL(Obj(0, k_HOURS_MAX, k_MINS_MAX));
            ASSERT_PASS(Obj(0, k_HOURS_MAX, 0));
            ASSERT_PASS(Obj(0, k_HOURS_MAX, k_MINS_MIN));

            ASSERT_PASS(Obj(0, k_HOURS_MIN, k_MINS_MAX));
            ASSERT_PASS(Obj(0, k_HOURS_MIN, 0));
            ASSERT_FAIL(Obj(0, k_HOURS_MIN, k_MINS_MIN));
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
#ifndef BDE_OPENSOURCE_PUBLICATION  // pending deprecation
        //   static int maxSupportedBdexVersion();
#endif // BDE_OPENSOURCE_PUBLICATION -- pending deprecation
#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
        //   static int maxSupportedVersion();
#endif // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22
        // --------------------------------------------------------------------

        // Allocator to use instead of the default allocator.
        bslma::TestAllocator allocator("bslx", veryVeryVeryVerbose);

        if (verbose) cout << endl
                          << "TESTING BDEX STREAMING" << endl
                          << "======================" << endl;

        // Scalar object values used in various stream tests.
        const Obj VA(0,  0,  0,  0,   0,   0);
        const Obj VB(0,  0,  0,  0,   0, 999);
        const Obj VC(0,  0,  0,  0, 999,   0);
        const Obj VD(0,  0,  0, 59,   0,   0);
        const Obj VE(0,  0, 59,  0,   0,   0);
        const Obj VF(0, 23,  0,  0,   0,   0);
        const Obj VG(1, 23, 22, 21, 209, 212);

        // Array object used in various stream tests.
        const Obj VALUES[]   = { VA, VB, VC, VD, VE, VF, VG };
        const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                / sizeof *VALUES);

        if (verbose) {
            cout << "\nTesting 'maxSupportedBdexVersion'." << endl;
        }
        {
            ASSERT(1 == Obj::maxSupportedBdexVersion(0));
            ASSERT(2 == Obj::maxSupportedBdexVersion(20170401));
            ASSERT(2 == Obj::maxSupportedBdexVersion(VERSION_SELECTOR));

            using bslx::VersionFunctions::maxSupportedBdexVersion;

            ASSERT(1 == maxSupportedBdexVersion(reinterpret_cast<Obj *>(0),
                                                0));
            ASSERT(2 == maxSupportedBdexVersion(reinterpret_cast<Obj *>(0),
                                                20170401));
            ASSERT(2 == maxSupportedBdexVersion(reinterpret_cast<Obj *>(0),
                                                VERSION_SELECTOR));
        }

        const int VERSIONS[] = { 1, 2 };
        const int NUM_VERSIONS = static_cast<int>(  sizeof VERSIONS
                                                  / sizeof *VERSIONS);

        for (int versionIndex = 0;
             versionIndex < NUM_VERSIONS;
             ++versionIndex) {
            const int VERSION = VERSIONS[versionIndex];

            if (verbose) {
                cout << "\nTesting Version " << VERSION << "." << endl;
            }

            if (verbose) {
                cout << "\tDirect initial trial of 'bdexStreamOut' and "
                     << "(valid) 'bdexStreamIn'." << endl;
            }
            {
                const Obj X(VC);
                Out       out(VERSION_SELECTOR, &allocator);

                Out& rvOut = X.bdexStreamOut(out, VERSION);
                ASSERT(&out == &rvOut);
                ASSERT(out);

                const char *const OD  = out.data();
                const int         LOD = static_cast<int>(out.length());

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

            // We will use the stream free functions provided by 'bslx', as
            // opposed to the class member functions, since the 'bslx'
            // implementation gives priority to the free function
            // implementations; we want to test what will be used.
            // Furthermore, toward making this test case more reusable in other
            // components, from here on we generally use the 'bdexStreamIn' and
            // 'bdexStreamOut' free functions that are defined in the 'bslx'
            // package rather than call the like-named member functions
            // directly.

            if (verbose) {
                cout << "\tThorough test using stream free functions."
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
                    LOOP_ASSERT(i, out);
                    const char *const OD  = out.data();
                    const int         LOD = static_cast<int>(out.length());

                    // Verify that each new value overwrites every old value
                    // and that the input stream is emptied, but remains valid.

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

                        if (1 == VERSION) {
                            // Version 1 loses microseconds; replace.

                            mT.addMicroseconds(X.microseconds());
                        }

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
                const int         LOD = static_cast<int>(out.length());
                ASSERT(0 == LOD);

                for (int i = 0; i < NUM_VALUES; ++i) {
                    In in(OD, LOD);
                    LOOP_ASSERT(i, in);
                    LOOP_ASSERT(i, in.isEmpty());

                    // Ensure that reading from an empty or invalid input
                    // stream leaves the stream invalid and the target object
                    // unchanged.

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
                ASSERT(out);

                const char *const OD  = out.data();
                const int         LOD = static_cast<int>(out.length());
                ASSERT(0 < LOD);

                for (int i = 0; i < NUM_VALUES; ++i) {
                    In in(OD, LOD);
                    in.invalidate();
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, !in.isEmpty());

                    // Ensure that reading from a non-empty, invalid input
                    // stream leaves the stream invalid and the target object
                    // unchanged.

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
                ASSERT(out);
                const int         LOD1 = static_cast<int>(out.length());

                Out& rvOut2 = bdexStreamOut(out, X2, VERSION);
                ASSERT(&out == &rvOut2);
                ASSERT(out);
                const int         LOD2 = static_cast<int>(out.length());

                Out& rvOut3 = bdexStreamOut(out, X3, VERSION);
                ASSERT(&out == &rvOut3);
                ASSERT(out);
                const int         LOD3 = static_cast<int>(out.length());
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
                            if (1 == VERSION) {
                                // Version 1 loses microseconds; replace.

                                mT1.addMicroseconds(X1.microseconds());
                            }
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
                            if (1 == VERSION) {
                                // Version 1 loses microseconds; replace.

                                mT1.addMicroseconds(X1.microseconds());
                            }
                            LOOP_ASSERT(i, &in == &rvIn1);
                            LOOP_ASSERT(i,  in);
                            LOOP_ASSERT(i, X1 == T1);
                            In& rvIn2 = bdexStreamIn(in, mT2, VERSION);
                            if (1 == VERSION) {
                                // Version 1 loses microseconds; replace.

                                mT2.addMicroseconds(X2.microseconds());
                            }
                            LOOP_ASSERT(i, &in == &rvIn2);
                            LOOP_ASSERT(i,  in);
                            LOOP_ASSERT(i, X2 == T2);
                            In& rvIn3 = bdexStreamIn(in, mT3, VERSION);
                            LOOP_ASSERT(i, &in == &rvIn3);
                            LOOP_ASSERT(i, !in);
                            if (LOD2 <= i) LOOP_ASSERT(i, W3 == T3);
                        }

                        // Verify the objects are in a valid state.
                        LOOP_ASSERT(i,
                                    bdlt::TimeUnitRatio::k_US_PER_D >
                                            -T1.fractionalDayInMicroseconds());
                        LOOP_ASSERT(i,
                                    bdlt::TimeUnitRatio::k_US_PER_D >
                                             T1.fractionalDayInMicroseconds());
                        LOOP_ASSERT(i, (0 <= T1.totalDays() && 0 <=
                                            T1.fractionalDayInMicroseconds())
                                    || (0 >= T1.totalDays() && 0 >=
                                            T1.fractionalDayInMicroseconds()));

                        LOOP_ASSERT(i,
                                    bdlt::TimeUnitRatio::k_US_PER_D >
                                            -T2.fractionalDayInMicroseconds());
                        LOOP_ASSERT(i,
                                    bdlt::TimeUnitRatio::k_US_PER_D >
                                             T2.fractionalDayInMicroseconds());
                        LOOP_ASSERT(i, (0 <= T2.totalDays() && 0 <=
                                            T2.fractionalDayInMicroseconds())
                                    || (0 >= T2.totalDays() && 0 >=
                                            T2.fractionalDayInMicroseconds()));

                        LOOP_ASSERT(i,
                                    bdlt::TimeUnitRatio::k_US_PER_D >
                                            -T3.fractionalDayInMicroseconds());
                        LOOP_ASSERT(i,
                                    bdlt::TimeUnitRatio::k_US_PER_D >
                                             T3.fractionalDayInMicroseconds());
                        LOOP_ASSERT(i, (0 <= T3.totalDays() && 0 <=
                                            T3.fractionalDayInMicroseconds())
                                    || (0 >= T3.totalDays() && 0 >=
                                            T3.fractionalDayInMicroseconds()));
                    } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
                }
            }
        }

        if (verbose) {
            cout << "\tOn corrupted data." << endl;
        }

        const Obj W;                      // default value
        const Obj X(0, 1, 0, 0, 0);       // original (control)
        const Obj Y(0, 0, 0, 0, 1);       // new (streamed-out)

        // Verify the three objects are distinct.
        ASSERT(W != X);
        ASSERT(W != Y);
        ASSERT(X != Y);

        if (verbose) {
            cout << "\t\tGood stream (for control)." << endl;
        }
        {
            // Version 1.

            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            out.putInt64(1);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, 1);
            ASSERT(&in == &rvIn);
            ASSERT(in);
            ASSERT(Y == T);
        }
        {
            // Version 2.

            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            out.putInt32(0);
            out.putInt64(1000);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, 2);
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

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            out.putInt64(1);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

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
            const char version = 3;  // too large (current version is 2)

            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            out.putInt64(1);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

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
            cout << "\t\tBad value." << endl;
        }
        {
            // Version 1.  Value too small.

            Out out(VERSION_SELECTOR, &allocator);
            out.putInt64(k_MSECS_MIN - 1);  // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, 1);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }
        {
            // Version 1.  Value too large.

            Out out(VERSION_SELECTOR, &allocator);
            out.putInt64(k_MSECS_MAX + 1);  // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, 1);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }
        {
            // Version 2.  Microseconds too small.

            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            out.putInt32(0);
            out.putInt64(-bdlt::TimeUnitRatio::k_US_PER_D);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, 2);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }
        {
            // Version 2.  Microseconds way too small.

            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            out.putInt32(0);
            out.putInt64(k_USECS_MIN);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, 2);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }
        {
            // Version 2.  Microseconds too large.

            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            out.putInt32(0);
            out.putInt64(bdlt::TimeUnitRatio::k_US_PER_D);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, 2);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }
        {
            // Version 2.  Positive days, negative microseconds.

            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            out.putInt32( 1);
            out.putInt64(-1);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, 2);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }
        {
            // Version 2.  Negative days, positive microseconds.

            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            out.putInt32(-1);
            out.putInt64( 1);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, 2);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
             cout << "\nWire format direct tests." << endl;
        }
        {
            static const struct {
                int          d_lineNum;       // source line number
                Int64        d_microseconds;  // spec. microseconds
                int          d_version;       // version to stream with
                bsl::size_t  d_length;        // expect output length
                const char  *d_fmt_p;         // expected output format
            } DATA[] = {
                //LN  US        V  LEN  FORMAT
                //--  --------  -  ---  -------------------
                { L_,        0, 1,   8, "\x00\x00\x00\x00\x00\x00\x00\x00" },
                { L_,     1000, 1,   8, "\x00\x00\x00\x00\x00\x00\x00\x01" },
                { L_,   256000, 1,   8, "\x00\x00\x00\x00\x00\x00\x01\x00" },
                { L_, 65536000, 1,   8, "\x00\x00\x00\x00\x00\x01\x00\x00" },

                { L_,        0, 2,  12,
                        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" },
                { L_,        1, 2,  12,
                        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01" },
                { L_,      256, 2,  12,
                        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00" },
                { L_,    65536, 2,  12,
                        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00" },
                { L_, k_USECS_PER_DAY,
                                2,  12,
                        "\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00" }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE         = DATA[i].d_lineNum;
                const Int64       MICROSECONDS = DATA[i].d_microseconds;
                const int         VERSION      = DATA[i].d_version;
                const bsl::size_t LEN          = DATA[i].d_length;
                const char *const FMT          = DATA[i].d_fmt_p;

                // Test using class methods.

                {
                    Obj        mX(0, 0, 0, 0, 0, MICROSECONDS);
                    const Obj& X = mX;

                    bslx::ByteOutStream  out(VERSION_SELECTOR, &allocator);
                    bslx::ByteOutStream& rvOut = X.bdexStreamOut(out, VERSION);
                    LOOP_ASSERT(LINE, &out == &rvOut);
                    LOOP_ASSERT(LINE, out);

                    LOOP_ASSERT(LINE, LEN == out.length());
                    LOOP_ASSERT(LINE, 0 == memcmp(out.data(), FMT, LEN));

                    if (verbose && memcmp(out.data(), FMT, LEN)) {
                        const char *hex = "0123456789abcdef";
                        P_(LINE);
                        for (bsl::size_t j = 0; j < out.length(); ++j) {
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
                    Obj        mX(0, 0, 0, 0, 0, MICROSECONDS);
                    const Obj& X = mX;

                    using bslx::OutStreamFunctions::bdexStreamOut;

                    bslx::ByteOutStream  out(VERSION_SELECTOR, &allocator);
                    bslx::ByteOutStream& rvOut = bdexStreamOut(out,
                                                               X,
                                                               VERSION);
                    LOOP_ASSERT(LINE, &out == &rvOut);
                    LOOP_ASSERT(LINE, out);

                    LOOP_ASSERT(LINE, LEN == out.length());
                    LOOP_ASSERT(LINE, 0 == memcmp(out.data(), FMT, LEN));

                    if (verbose && memcmp(out.data(), FMT, LEN)) {
                        const char *hex = "0123456789abcdef";
                        P_(LINE);
                        for (bsl::size_t j = 0; j < out.length(); ++j) {
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

#ifndef BDE_OPENSOURCE_PUBLICATION  // pending deprecation

        if (verbose) {
            cout << "\nTesting deprecated methods." << endl;
        }
        {
#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
            ASSERT(Obj::maxSupportedVersion()
                                           == Obj::maxSupportedBdexVersion(0));
#endif // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22
            ASSERT(Obj::maxSupportedBdexVersion()
                                           == Obj::maxSupportedBdexVersion(0));
        }

#endif // BDE_OPENSOURCE_PUBLICATION -- pending deprecation
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //   Verify the assignment operator works as expected.
        //
        // Concerns:
        //: 1 Any value is assignable to an object having any initial value
        //:   without affecting the rhs operand value.
        //:
        //: 2 Any object must be assignable to itself.
        //
        // Plan:
        //: 1 Construct and initialize a set S of (unique) objects with
        //:   substantial and varied differences in value.  Using all
        //:   combinations (u, v) in the cross product S x S, assign v to u and
        //:   assert that u == v and v is unchanged.  (C-1)
        //:
        //: 2 Test aliasing by assigning (a temporary copy of) each u to
        //:   itself and verifying that its value remains unchanged.  (C-2)
        //
        // Testing:
        //   DatetimeInterval& operator=(const DatetimeInterval& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ASSIGNMENT OPERATOR" << endl
                          << "===========================" << endl;

        if (verbose) cout << "\nTesting Assignment u = V." << endl;
        {
            const int                    NUM_DATA  = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            int i;
            for (i = 0; i < NUM_DATA; ++i) {
                Obj v;  const Obj& V = v;

                v.setInterval(DATA[i].d_days,
                              DATA[i].d_hours,
                              DATA[i].d_minutes,
                              DATA[i].d_seconds,
                              DATA[i].d_msecs,
                              DATA[i].d_usecs);

                for (int j = 0; j < NUM_DATA; ++j) {
                    Obj u;  const Obj& U = u;

                    v.setInterval(DATA[j].d_days,
                                  DATA[j].d_hours,
                                  DATA[j].d_minutes,
                                  DATA[j].d_seconds,
                                  DATA[j].d_msecs,
                                  DATA[j].d_usecs);

                    if (veryVerbose) { T_;  P_(V);  P_(U); }

                    Obj w(V);  const Obj &W = w;          // control

                    ASSERT(&u == &(u = V));

                    if (veryVerbose) P(U);

                    LOOP2_ASSERT(i, j, W == U);
                    LOOP2_ASSERT(i, j, W == V);
                }
            }

            if (verbose) {
                cout << "\nTesting assignment u = u (Aliasing)."
                     << endl;
            }

            for (i = 0; i < NUM_DATA; ++i) {
                Obj u;  const Obj& U = u;

                u.setInterval(DATA[i].d_days,
                              DATA[i].d_hours,
                              DATA[i].d_minutes,
                              DATA[i].d_seconds,
                              DATA[i].d_msecs,
                              DATA[i].d_usecs);

                Obj w(U);  const Obj &W = w;              // control

                ASSERT(&u == &(u = u));

                if (veryVerbose) { T_;  P_(U);  P(W); }

                LOOP_ASSERT(i, W == U);
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
        // TESTING COPY CONSTRUCTOR
        //   Verify the copy constructor works as expected.
        //
        // Concerns:
        //: 1 Any value must be able to be copy constructed without affecting
        //:   the argument.
        //
        // Plan:
        //: 1 Specify a set S of control objects with substantial and varied
        //:   differences in value.  For each object w in S, construct and
        //:   initialize an identically valued object x using the primary
        //:   manipulator, and copy construct an object y from x.  Use the
        //:   equality operator to assert that both x and y have the same value
        //:   as w.  (C-1)
        //
        //
        // Testing:
        //   DatetimeInterval(const DatetimeInterval& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING COPY CONSTRUCTOR" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nTesting copy constructor." << endl;
        {
            const int                    NUM_DATA  = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                Obj x;  const Obj& X = x;

                x.setInterval(DATA[i].d_days,
                              DATA[i].d_hours,
                              DATA[i].d_minutes,
                              DATA[i].d_seconds,
                              DATA[i].d_msecs,
                              DATA[i].d_usecs);

                Obj w;  const Obj& W = w;

                w.setInterval(DATA[i].d_days,
                              DATA[i].d_hours,
                              DATA[i].d_minutes,
                              DATA[i].d_seconds,
                              DATA[i].d_msecs,
                              DATA[i].d_usecs);

                Obj y(X);  const Obj& Y = y;

                if (veryVerbose) { T_;  P_(W);  P_(X);  P(Y); }

                LOOP_ASSERT(i, X == W);
                LOOP_ASSERT(i, Y == W);
            }
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

        const int                    NUM_DATA  = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   ILINE      = DATA[i].d_line;
            const int   IDAYS      = DATA[i].d_days;
            const Int64 IHOURS     = DATA[i].d_hours;
            const Int64 IMINUTES   = DATA[i].d_minutes;
            const Int64 ISECONDS   = DATA[i].d_seconds;
            const Int64 IMSECS     = DATA[i].d_msecs;
            const Int64 IUSECS     = DATA[i].d_usecs;

            // Ensure an object compares correctly with itself (alias test).
            {
                Obj mW;  const Obj& W = mW;

                mW.setInterval(IDAYS,
                               IHOURS,
                               IMINUTES,
                               ISECONDS,
                               IMSECS,
                               IUSECS);

                if (veryVerbose) { T_ P_(ILINE) P(W) }

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

            for (int j = 0; j < NUM_DATA; ++j) {
                const int   JLINE      = DATA[j].d_line;
                const int   JDAYS      = DATA[j].d_days;
                const Int64 JHOURS     = DATA[j].d_hours;
                const Int64 JMINUTES   = DATA[j].d_minutes;
                const Int64 JSECONDS   = DATA[j].d_seconds;
                const Int64 JMSECS     = DATA[j].d_msecs;
                const Int64 JUSECS     = DATA[j].d_usecs;

                Obj mX;  const Obj& X = mX;

                mX.setInterval(IDAYS,
                               IHOURS,
                               IMINUTES,
                               ISECONDS,
                               IMSECS,
                               IUSECS);

                Obj mY;  const Obj& Y = mY;

                mY.setInterval(JDAYS,
                               JHOURS,
                               JMINUTES,
                               JSECONDS,
                               JMSECS,
                               JUSECS);

                if (veryVerbose) { T_ P_(JLINE) P(Y) }

                const bool EXP = (   X.days() == Y.days()
                                  && X.fractionalDayInMicroseconds()
                                           == Y.fractionalDayInMicroseconds());

                if (veryVerbose) { T_ T_ T_ P_(EXP) P_(X) P(Y) }

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
        // PRINT, OUTPUT OPERATOR, AND 'printToBuffer'
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.  In
        //:   particular:
        //:
        //:   1 The attributes always appear on a single line.
        //:
        //:   2 A negative value of 'level' always suppresses all indentation
        //:     (since there is never a second line to indent).,
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)'.
        //:
        //: 4 The 'print' method signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values.
        //:
        //: 7 The output 'operator<<' signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the supplied 'ostream'.
        //:
        //: 9 The 'printToBuffer' method:
        //:   1 Writes in the expected format.
        //:   2 Never writes more than the specified limit.
        //:   3 Writes in the specified buffer.
        //:   4 QoI: Asserted precondition violations are detected when
        //:     enabled.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 7)
        //:
        //: 2 Using the table-driven technique: (C-1..3, 5..6, 8)
        //:
        //:   1 Define fourteen carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the two
        //:     formatting parameters, along with the expected output.
        //:
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A } x { 0 } x { 0, 1, -1, -8 } --> 3 expected o/ps
        //:     2 { A } x { 3, -3 } x { 0, 2, -2, -8 } --> 6 expected o/ps
        //:     3 { B } x { 2 } x { 3 } --> 1 expected o/p
        //:     4 { A B } x { -8 } x { -8 } --> 2 expected o/ps
        //:     5 { A B } x { -9 } x { -9 } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1: (C-1..3, 5..6, 8)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, (-9, -9), then
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
        //:   3 Test 'printToBuffer' using a table-driven approach.  (C-9)
        //:
        //:     1 Define an assortment of different input values and limits on
        //:       the number of bytes written.
        //:
        //:     2 For each input value, write the result into an over-sized
        //:       buffer that is pre-filled with an "unset" character.  Data is
        //:       written into the middle of the buffer.  After writing,
        //:       confirm that all characters outside the targeted range have
        //:       their initial value.
        //:
        //:     4 Verify that, in appropriate build modes, defensive checks are
        //:       triggered for invalid attribute values, but not triggered for
        //:       adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //
        // Testing:
        //   ostream& print(ostream& os, int level = 0, int spl = 4) const;
        //   ostream& operator<<(ostream &stream, const Time &object);
        //   int printToBuffer(char *result, int size, int precision) const;
#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
        //   ostream& streamOut(ostream& stream) const;
#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22
        // --------------------------------------------------------------------

        if (verbose) cout
                      << endl
                      << "PRINT, OUTPUT OPERATOR, AND 'printToBuffer'" << endl
                      << "===========================================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = bdlt::operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        const Obj TA( 0, 0,  0,  0,   0);
        const Obj TZ( 1, 23, 59, 59, 999);

        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spacesPerLevel;
            int         d_day;
            int         d_hour;
            int         d_minute;
            int         d_second;
            int         d_millisecond;
            int         d_microsecond;
            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"

    // ------------------------------------------------------------------
    // P-2.1.1: { A } x { 0 } x { 0, 1, -1, -8 } --> 4 expected o/ps
    // ------------------------------------------------------------------

    //LINE L SPL  D   H   M   S   MS   US   EXPECTED
    //---- - ---  --  --  --  --  ---  ---  --------
    { L_,  0,  0,  0,  0,  0,  0,   0,   0, "+0_00:00:00.000000" NL },
    { L_,  0,  1,  0,  0,  0,  0,   0,   0, "+0_00:00:00.000000" NL },
    { L_,  0, -1,  0,  0,  0,  0,   0,   0, "+0_00:00:00.000000"    },

    { L_,  0, -8,  0,  0,  0,  0,   0,   0, "+0_00:00:00.000000" NL },

    // ------------------------------------------------------------------
    // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } --> 6 expected o/ps
    // ------------------------------------------------------------------

    //LINE L SPL  D   H   M   S   MS   US   EXPECTED
    //---- - ---  --  --  --  --  ---  ---  --------
    { L_,  3,  0,  0,  0,  0,  0,   0,   0,
                                         "+0_00:00:00.000000"             NL },
    { L_,  3,  2,  0,  0,  0,  0,   0,   0,
                                         "      +0_00:00:00.000000"       NL },
    { L_,  3, -2,  0,  0,  0,  0,   0,   0,
                                         "      +0_00:00:00.000000"          },

    { L_,  3, -8,  0,  0,  0,  0,   0,   0,
                                         "            +0_00:00:00.000000" NL },

    { L_, -3,  0,  0,  0,  0,  0,   0,   0, "+0_00:00:00.000000" NL },
    { L_, -3,  2,  0,  0,  0,  0,   0,   0, "+0_00:00:00.000000" NL },
    { L_, -3, -2,  0,  0,  0,  0,   0,   0, "+0_00:00:00.000000"    },

    { L_, -3, -8,  0,  0,  0,  0,   0,   0, "+0_00:00:00.000000" NL },

    // -----------------------------------------------------------------
    // P-2.1.3: { B } x { 2 } x { 3 } --> 1 expected o/p
    // -----------------------------------------------------------------

    //LINE L SPL  D   H   M   S   MS   US   EXPECTED
    //---- - ---  --  --  --  --  ---  ---  --------
    { L_,  2,  3,  1, 23, 59, 59, 999, 999, "      +1_23:59:59.999999" NL },

    // -----------------------------------------------------------------
    // P-2.1.4: { A B } x { -8 } x { -8 } --> 2 expected o/ps
    // -----------------------------------------------------------------

    //LINE L SPL  D   H   M   S   MS   US   EXPECTED
    //---- - ---  --  --  --  --  ---  ---  --------
    { L_, -8, -8,  0,  0,  0,  0,   0,   0, "+0_00:00:00.000000" NL },
    { L_, -8, -8,  1, 23, 59, 59, 999, 999, "+1_23:59:59.999999" NL },

    // -----------------------------------------------------------------
    // P-2.1.5: { A B } x { -9 } x { -9 } --> 2 expected o/ps
    // -----------------------------------------------------------------

    //LINE L SPL  D   H   M   S   MS   US   EXPECTED
    //---- - ---  --  --  --  --  ---  ---  --------
    { L_, -9, -9,  0,  0,  0,  0,   0,   0, "+0_00:00:00.000000" },
    { L_, -9, -9,  1, 23, 59, 59, 999, 999, "+1_23:59:59.999999" }

#undef NL
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const int         L      = DATA[ti].d_level;
                const int         SPL    = DATA[ti].d_spacesPerLevel;
                const int         DAY    = DATA[ti].d_day;
                const int         HOUR   = DATA[ti].d_hour;
                const int         MINUTE = DATA[ti].d_minute;
                const int         SECOND = DATA[ti].d_second;
                const int         MSEC   = DATA[ti].d_millisecond;
                const int         USEC   = DATA[ti].d_microsecond;
                const char *const EXP    = DATA[ti].d_expected_p;

                if (veryVerbose) {
                    T_ P_(L) P_(SPL) P_(DAY) P_(HOUR) P_(MINUTE) P_(SECOND)
                                                              P_(MSEC) P(USEC);
                }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                Obj        x(DAY, HOUR, MINUTE, SECOND, MSEC, USEC);
                const Obj& X = x;

                bslma::TestAllocator oa("scratch",  veryVeryVeryVerbose);
                stringstream ss(&oa);

                if (-9 == L && -9 == SPL) {

                    // Verify supplied stream is returned by reference.

                    LOOP_ASSERT(LINE, &ss == &(ss << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    LOOP_ASSERT(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        LOOP_ASSERT(LINE, &ss == &X.print(ss, L, SPL));
                    }
                    else if (-8 != L) {
                        LOOP_ASSERT(LINE, &ss == &X.print(ss, L));
                    }
                    else {
                        LOOP_ASSERT(LINE, &ss == &X.print(ss));
                    }

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                // Verify output is formatted as expected.

                // Avoid invoking 'ss.str()' which returns a string by value
                // and may introduce use of the default allocator.

                bsl::string result(bsl::istreambuf_iterator<char>(ss),
                                   bsl::istreambuf_iterator<char>(),
                                   &oa);

                if (veryVeryVerbose) { P(result) }

                LOOP3_ASSERT(LINE, EXP, result, EXP == result);
            }
        }

        if (verbose) cout << "\nTesting 'printToBuffer'." << endl;
        {
            static const struct {
                int         d_line;
                int         d_day;
                int         d_hour;
                int         d_minute;
                int         d_second;
                int         d_msec;
                int         d_usec;
                int         d_precision;
                int         d_numBytes;
                int         d_expectedLength;
                const char *d_expected_p;
            } DATA[] = {
    //----------^
    //LN  DAY  HR  M   S   MS   US   PREC  LIMIT  EL         EXPECTED
    //--  ---  --  --  --  ---  ---  ----  -----  --  ----------------------
    { L_,   0,  0,  0,  0,   0,   0,    0,   100, 11, "+0_00:00:00"          },
    { L_,   0,  0,  0,  0,   0,   0,    1,   100, 13, "+0_00:00:00.0"        },
    { L_,   0,  0,  0,  0,   0,   0,    3,   100, 15, "+0_00:00:00.000"      },
    { L_,   0,  0,  0,  0,   0,   0,    6,   100, 18, "+0_00:00:00.000000"   },
    { L_,   0,  0,  0,  0,   0,   7,    0,   100, 11, "+0_00:00:00"          },
    { L_,   0,  0,  0,  0,   0,   7,    1,   100, 13, "+0_00:00:00.0"        },
    { L_,   0,  0,  0,  0,   0,   7,    3,   100, 15, "+0_00:00:00.000"      },
    { L_,   0,  0,  0,  0,   0,   7,    5,   100, 17, "+0_00:00:00.00000"    },
    { L_,   0,  0,  0,  0,   0,   7,    6,   100, 18, "+0_00:00:00.000007"   },
    { L_,   0,  0,  0,  0,   0,  17,    6,   100, 18, "+0_00:00:00.000017"   },
    { L_,   0,  0,  0,  0,   0, 317,    3,   100, 15, "+0_00:00:00.000"      },
    { L_,   0,  0,  0,  0,   0, 317,    4,   100, 16, "+0_00:00:00.0003"     },
    { L_,   0,  0,  0,  0,   0, 317,    5,   100, 17, "+0_00:00:00.00031"    },
    { L_,   0,  0,  0,  0,   0, 317,    6,   100, 18, "+0_00:00:00.000317"   },
    { L_,   0, 23, 22, 21, 209,   0,    6,   100, 18, "+0_23:22:21.209000"   },
    { L_,   0, 23, 22, 21, 210,   0,    6,   100, 18, "+0_23:22:21.210000"   },
    { L_,   0, 23, 22, 21, 211,   0,    6,   100, 18, "+0_23:22:21.211000"   },
    { L_,   0, 23, 59, 59, 999,   0,    6,   100, 18, "+0_23:59:59.999000"   },
    { L_,   0, 23, 59, 59, 999,   5,    6,   100, 18, "+0_23:59:59.999005"   },
    { L_,   0, 23, 59, 59, 999,  65,    6,   100, 18, "+0_23:59:59.999065"   },
    { L_,   0, 23, 59, 59, 999, 765,    0,   100, 11, "+0_23:59:59"          },
    { L_,   0, 23, 59, 59, 999, 765,    1,   100, 13, "+0_23:59:59.9"        },
    { L_,   0, 23, 59, 59, 999, 765,    2,   100, 14, "+0_23:59:59.99"       },
    { L_,   0, 23, 59, 59, 999, 765,    3,   100, 15, "+0_23:59:59.999"      },
    { L_,   0, 23, 59, 59, 999, 765,    4,   100, 16, "+0_23:59:59.9997"     },
    { L_,   0, 23, 59, 59, 999, 765,    5,   100, 17, "+0_23:59:59.99976"    },
    { L_,   0, 23, 59, 59, 999, 765,    6,   100, 18, "+0_23:59:59.999765"   },
    { L_,   0, 23, 59, 59, 999,   0,    6,     0, 18, ""                     },
    { L_,   0, 23, 59, 59, 999,   0,    6,     1, 18, ""                     },
    { L_,   0, 23, 59, 59, 999,   0,    6,     2, 18, "+"                    },
    { L_,   0, 23, 59, 59, 999,   0,    6,     3, 18, "+0"                   },
    { L_,   0, 23, 59, 59, 999,   0,    6,    13, 18, "+0_23:59:59."         },
    { L_,   0, 23, 59, 59, 999,   0,    6,    14, 18, "+0_23:59:59.9"        },
    { L_,   0, 23, 59, 59, 999,   0,    6,    15, 18, "+0_23:59:59.99"       },
    { L_,   0, 23, 59, 59, 999,   0,    6,    16, 18, "+0_23:59:59.999"      },
    { L_,   0, 23, 59, 59, 999,   0,    6,    17, 18, "+0_23:59:59.9990"     },
    { L_,   0, 23, 59, 59, 999,   0,    6,    18, 18, "+0_23:59:59.99900"    },
    { L_,   0, 23, 59, 59, 999,   0,    6,    19, 18, "+0_23:59:59.999000"   },

    { L_,   0,  0,  0,  0,   0,  -1,    5,   100, 17, "-0_00:00:00.00000"    },
    { L_,   0,  0,  0,  0,   0,  -1,    6,   100, 18, "-0_00:00:00.000001"   },
    { L_,  -1,  0,  0,  0,   0,   0,    5,   100, 17, "-1_00:00:00.00000"    },
    { L_,  -1,  0,  0,  0,   0,   0,    6,   100, 18, "-1_00:00:00.000000"   },
    { L_,  -1,  0,  0,  0,   0,  -1,    5,   100, 17, "-1_00:00:00.00000"    },
    { L_,  -1,  0,  0,  0,   0,  -1,    6,   100, 18, "-1_00:00:00.000001"   },

    { L_,   0,  0,  0,  0,   0,  -1,    0,     0, 11, ""                     },
    { L_,   0,  0,  0,  0,   0,  -1,    0,     1, 11, ""                     },
    { L_,   0,  0,  0,  0,   0,  -1,    0,     2, 11, "-"                    },
    { L_,   0,  0,  0,  0,   0,  -1,    0,    11, 11, "-0_00:00:0"           },
    { L_,   0,  0,  0,  0,   0,  -1,    0,    12, 11, "-0_00:00:00"          },

    { L_,   0,  0,  0,  0,   0,  -1,    1,     0, 13, ""                     },
    { L_,   0,  0,  0,  0,   0,  -1,    1,     1, 13, ""                     },
    { L_,   0,  0,  0,  0,   0,  -1,    1,     2, 13, "-"                    },
    { L_,   0,  0,  0,  0,   0,  -1,    1,    13, 13, "-0_00:00:00."         },
    { L_,   0,  0,  0,  0,   0,  -1,    1,    14, 13, "-0_00:00:00.0"        },

    { L_,   0,  0,  0,  0,   0,  -1,    6,     0, 18, ""                     },
    { L_,   0,  0,  0,  0,   0,  -1,    6,     1, 18, ""                     },
    { L_,   0,  0,  0,  0,   0,  -1,    6,     2, 18, "-"                    },
    { L_,   0,  0,  0,  0,   0,  -1,    6,    18, 18, "-0_00:00:00.00000"    },
    { L_,   0,  0,  0,  0,   0,  -1,    6,    19, 18, "-0_00:00:00.000001"   },

    { L_,  10,  0,  0,  0,   0,   0,    6,     0, 19, ""                     },
    { L_,  10,  0,  0,  0,   0,   0,    6,     1, 19, ""                     },
    { L_,  10,  0,  0,  0,   0,   0,    6,     2, 19, "+"                    },
    { L_,  10,  0,  0,  0,   0,   0,    6,     3, 19, "+1"                   },
    { L_,  10,  0,  0,  0,   0,   0,    6,     4, 19, "+10"                  },
    { L_,  10,  0,  0,  0,   0,   0,    6,     5, 19, "+10_"                 },
    { L_,  10,  0,  0,  0,   0,   0,    6,    19, 19, "+10_00:00:00.00000"   },
    { L_,  10,  0,  0,  0,   0,   0,    6,    20, 19, "+10_00:00:00.000000"  },

    { L_, 100,  0,  0,  0,   0,   0,    6,     0, 20, ""                     },
    { L_, 100,  0,  0,  0,   0,   0,    6,     1, 20, ""                     },
    { L_, 100,  0,  0,  0,   0,   0,    6,     2, 20, "+"                    },
    { L_, 100,  0,  0,  0,   0,   0,    6,     3, 20, "+1"                   },
    { L_, 100,  0,  0,  0,   0,   0,    6,     4, 20, "+10"                  },
    { L_, 100,  0,  0,  0,   0,   0,    6,     5, 20, "+100"                 },
    { L_, 100,  0,  0,  0,   0,   0,    6,     6, 20, "+100_"                },
    { L_, 100,  0,  0,  0,   0,   0,    6,    20, 20, "+100_00:00:00.00000"  },

    { L_, INT_MAX, 23, 59, 59, 999, 999, 6,   28, 27,
                                               "+2147483647_23:59:59.999999" },

    { L_, INT_MIN, -23, -59, -59, -999, -999, 6, 28, 27,
                                               "-2147483648_23:59:59.999999" },
    //----------v
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            const int  BUF_SIZE = 1000;               // Over-sized for output.
            const char XX       = static_cast<char>(0xFF);
                                                      // Used as "unset"
                                                      // character.
            char       mCtrlBuf[BUF_SIZE];
            memset(mCtrlBuf, XX, sizeof(mCtrlBuf));
            const char *const CTRL_BUF = mCtrlBuf;    // Referenced in overrun
                                                      // checks.

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE     = DATA[ti].d_line;
                const int         DAY      = DATA[ti].d_day;
                const int         HOUR     = DATA[ti].d_hour;
                const int         MINUTE   = DATA[ti].d_minute;
                const int         SECOND   = DATA[ti].d_second;
                const int         MSEC     = DATA[ti].d_msec;
                const int         USEC     = DATA[ti].d_usec;
                const int         PREC     = DATA[ti].d_precision;
                const int         LIMIT    = DATA[ti].d_numBytes;
                const int         EXP_LEN  = DATA[ti].d_expectedLength;
                const char *const EXPECTED = DATA[ti].d_expected_p;

                if (veryVerbose) {
                    T_  P_(DAY)
                        P_(HOUR)
                        P_(MINUTE)
                        P_(SECOND)
                        P_(MSEC)
                        P(USEC)
                    T_  P_(PREC)
                    T_  P_(LIMIT)
                    T_  P_(EXP_LEN) P(EXPECTED)
                }

                char buf[BUF_SIZE];

                // Preset 'buf' to "unset" values.
                memset(buf, XX, sizeof(buf));

                Obj        x(DAY, HOUR, MINUTE, SECOND, MSEC, USEC);
                const Obj& X = x;

                char      *p = buf + sizeof(buf)/2;
                const int  RC = X.printToBuffer(p, LIMIT, PREC);

                ASSERTV(LINE, EXP_LEN, RC, EXP_LEN == RC);

                const int LENGTH = 0 == LIMIT
                                   ? 0
                                   : static_cast<int>(strlen(p) + 1);
                LOOP_ASSERT(LINE, LENGTH <= LIMIT);

                if (veryVerbose) cout
                                   << "\tACTUAL FORMAT: "
                                   << (0 < LENGTH ? p : "<all-unset-expected>")
                                   << endl;
                LOOP_ASSERT(LINE, 0 == memcmp(buf, CTRL_BUF, p - buf));
                if (0 < LENGTH) {
                    LOOP3_ASSERT(LINE, p, EXPECTED,
                                 0 == memcmp(p, EXPECTED, LENGTH));
                }
                LOOP_ASSERT(LINE, 0 == memcmp(p + LENGTH,
                                              CTRL_BUF,
                                              (buf + sizeof(buf)) -
                                              (p   + LENGTH)));
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) cout << "\t'printToBuffer' method" << endl;
            {
                const int SIZE = 128;
                char      buf[SIZE];

                const Obj X;

                const int PRECISION = 6;

                ASSERT_PASS(X.printToBuffer(buf, SIZE, PRECISION));
                ASSERT_PASS(X.printToBuffer(buf,  0  , PRECISION));
                ASSERT_PASS(X.printToBuffer(buf, SIZE, 0));

                ASSERT_FAIL(X.printToBuffer(0,   SIZE, PRECISION));
                ASSERT_FAIL(X.printToBuffer(buf, -1  , PRECISION));
                ASSERT_FAIL(X.printToBuffer(0,   -1  , PRECISION));
                ASSERT_FAIL(X.printToBuffer(buf,  0  , -1));
                ASSERT_FAIL(X.printToBuffer(buf,  0  , PRECISION + 1));
            }
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22

        if (verbose) cout << "\nTesting 'streamOut'." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                int         d_day;      // day field value
                int         d_hour;     // hour field value
                int         d_minute;   // minute field value
                int         d_second;   // second field value
                int         d_msec;     // millisecond field value
                int         d_usec;     // microsecond field value
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //LN  DAY  HR   M    S    MSEC  USEC     OUTPUT FORMAT
                //--  ---  ---  ---  ---  ----  ----  -------------------
                { L_,   0,   0,   0,   0,    0,    0, "+0_00:00:00.000000" },
                { L_,   0,   0,   0,   0,    0,  999, "+0_00:00:00.000999" },
                { L_,   0,   0,   0,   0,  999,    0, "+0_00:00:00.999000" },
                { L_,   0,   0,   0,  59,    0,    0, "+0_00:00:59.000000" },
                { L_,   0,   0,  59,   0,    0,    0, "+0_00:59:00.000000" },
                { L_,   0,  23,   0,   0,    0,    0, "+0_23:00:00.000000" },
                { L_,   0,  23,  22,  21,  209,    0, "+0_23:22:21.209000" },
                { L_,   0,  23,  22,  21,  210,    0, "+0_23:22:21.210000" },
                { L_,   0,  23,  22,  21,  210,    1, "+0_23:22:21.210001" },
                { L_,   0,  23,  22,  21,  210,   17, "+0_23:22:21.210017" },
                { L_,   0,  23,  22,  21,  210,  412, "+0_23:22:21.210412" },
                { L_,   1,  23,  22,  21,  210,  412, "+1_23:22:21.210412" },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            const int SIZE = 1000;     // Must be able to hold output string.

            const char XX = static_cast<char>(0xFF);  // Value used for an
                                                      // unset 'char'.

            char        mCtrlBuf[SIZE];  memset(mCtrlBuf, XX, SIZE);
            const char *CTRL_BUF = mCtrlBuf; // Used for extra character check.

            for (int di = 0; di < NUM_DATA;  ++di) {
                const int         LINE   = DATA[di].d_lineNum;
                const int         DAY    = DATA[di].d_day;
                const int         HOUR   = DATA[di].d_hour;
                const int         MINUTE = DATA[di].d_minute;
                const int         SECOND = DATA[di].d_second;
                const int         MSEC   = DATA[di].d_msec;
                const int         USEC   = DATA[di].d_usec;
                const char *const FMT    = DATA[di].d_fmt_p;

                Obj        x(DAY, HOUR, MINUTE, SECOND, MSEC, USEC);
                const Obj& X = x;

                if (veryVerbose) cout << "\tEXPECTED FORMAT: " << FMT << endl;
                bslma::TestAllocator oa("scratch",  veryVeryVeryVerbose);
                stringstream out(bsl::string(CTRL_BUF, SIZE, &oa), &oa);
                X.streamOut(out);  out << ends;
                if (veryVerbose) {
                    cout << "\tACTUAL FORMAT:   " << out.str() << endl;
                }

                // Avoid invoking 'out.str()' which returns a string by value
                // and may introduce use of the default allocator.

                bsl::string result(bsl::istreambuf_iterator<char>(out),
                                   bsl::istreambuf_iterator<char>(),
                                   &oa);

                const int SZ = static_cast<int>(strlen(FMT)) + 1;
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE,
                            XX == result[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, 0 == memcmp(result.c_str(),
                                              FMT,
                                              SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(result.c_str() + SZ,
                                              CTRL_BUF + SZ,
                                              SIZE - SZ));
            }
        }

#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   Verify the basic accessors work as expected.
        //
        // Concerns:
        //: 1 Each accessor performs the appropriate arithmetic to convert
        //:   the internal representation to the two-parameter (d, us)
        //:   representation.
        //
        // Plan:
        //: 1 For each of a sequence of unique object values, verify that each
        //:   of the basic accessors returns the correct value.  (C-1)
        //
        // Testing:
        //   int days() const;
        //   Int64 fractionalDayInMicroseconds() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING BASIC ACCESSORS" << endl
                          << "=======================" << endl;

        if (verbose) {
            cout << "\nTesting 'days' and 'fractionalDayInMicroseconds'."
                 << endl;
        }
        {
            const int                    NUM_DATA  = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE      = DATA[i].d_line;
                const int   DAYS      = DATA[i].d_days;
                const Int64 HOURS     = DATA[i].d_hours;
                const Int64 MINUTES   = DATA[i].d_minutes;
                const Int64 SECONDS   = DATA[i].d_seconds;
                const Int64 MSECS     = DATA[i].d_msecs;
                const Int64 USECS     = DATA[i].d_usecs;
                const int   EXP_DAYS  = DATA[i].d_expDays;
                const Int64 EXP_USECS = DATA[i].d_expUsecs;

                Obj x;  const Obj& X = x;
                x.setInterval(DAYS, HOURS, MINUTES, SECONDS, MSECS, USECS);
                if (veryVerbose) {
                    T_;
                    P_(DAYS);
                    P_(HOURS);
                    P_(MINUTES);
                    P_(SECONDS);
                    P_(MSECS);
                    P_(USECS);
                    P(X);
                }

                LOOP_ASSERT(LINE, EXP_DAYS == X.days());
                LOOP_ASSERT(LINE,
                            EXP_USECS == X.fractionalDayInMicroseconds());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'gg'
        //   Void for 'bdlt_datetimeinterval'.
        //
        // Testing:
        //   Obj& gg(Obj *object, const char *spec);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING GENERATOR FUNCTION 'gg'" << endl
                 << "===============================" << endl;
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        //   Verify the primary manipulators work as expected.
        //
        // Concerns:
        //: 1 The separate time fields must be multiplied by the appropriate
        //:   factors to convert the six-parameter input representation to the
        //:   internal representation, with appropriate handling of potential
        //:   overflow.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Verify the default constructor by testing the value of the
        //:   resulting object.
        //:
        //: 2 For a sequence of independent test values, use the default
        //:   constructor to create a default object and use the primary
        //:   manipulator to set its value.  Verify the value using the basic
        //:   accessors.  Note that the destructor is exercised on each
        //:   configuration as the object being tested leaves scope (thereby
        //:   enabling assertions of internal invariants).  (C-1)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   DatetimeInterval();
        //   ~DatetimeInterval();
        //   void setInterval(int d, Int64 h = 0, m = 0, s = 0, ms = 0);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING PRIMARY MANIPULATORS (BOOTSTRAP)" << endl
                 << "========================================" << endl;
        }

        if (verbose) cout << "\nTesting default constructor." << endl;

        Obj x;  const Obj& X = x;

        ASSERT(0 == X.days());
        ASSERT(0 == X.fractionalDayInMicroseconds());

        if (verbose) cout << "\nTesting 'setInterval'." << endl;
        {
            const int                    NUM_DATA  = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE      = DATA[i].d_line;
                const int   DAYS      = DATA[i].d_days;
                const Int64 HOURS     = DATA[i].d_hours;
                const Int64 MINUTES   = DATA[i].d_minutes;
                const Int64 SECONDS   = DATA[i].d_seconds;
                const Int64 MSECS     = DATA[i].d_msecs;
                const Int64 USECS     = DATA[i].d_usecs;
                const int   EXP_DAYS  = DATA[i].d_expDays;
                const Int64 EXP_USECS = DATA[i].d_expUsecs;

                Obj x;  const Obj& X = x;

                x.setInterval(DAYS, HOURS, MINUTES, SECONDS, MSECS, USECS);

                if (veryVerbose) {
                    T_;
                    P_(DAYS);
                    P_(HOURS);
                    P_(MINUTES);
                    P_(SECONDS);
                    P_(MSECS);
                    P_(USECS);
                    P(X);
                }

                LOOP_ASSERT(LINE, EXP_DAYS == X.days());
                LOOP_ASSERT(LINE,
                            EXP_USECS == X.fractionalDayInMicroseconds());

                if (0 == USECS) {
                    x.setInterval(DAYS, HOURS, MINUTES, SECONDS, MSECS);

                    LOOP_ASSERT(LINE, EXP_DAYS == X.days());
                    LOOP_ASSERT(LINE,
                                EXP_USECS == X.fractionalDayInMicroseconds());

                    if (0 == MSECS) {
                        x.setInterval(DAYS, HOURS, MINUTES, SECONDS);

                        LOOP_ASSERT(LINE, EXP_DAYS == X.days());
                        LOOP_ASSERT(LINE,
                                    EXP_USECS ==
                                              X.fractionalDayInMicroseconds());

                        if (0 == SECONDS) {
                            x.setInterval(DAYS, HOURS, MINUTES);

                            LOOP_ASSERT(LINE, EXP_DAYS == X.days());
                            LOOP_ASSERT(LINE,
                                        EXP_USECS ==
                                              X.fractionalDayInMicroseconds());

                            if (0 == MINUTES) {
                                x.setInterval(DAYS, HOURS);

                                LOOP_ASSERT(LINE, EXP_DAYS == X.days());
                                LOOP_ASSERT(LINE,
                                            EXP_USECS ==
                                              X.fractionalDayInMicroseconds());

                                if (0 == HOURS) {
                                    x.setInterval(DAYS);

                                    LOOP_ASSERT(LINE, EXP_DAYS == X.days());
                                    LOOP_ASSERT(LINE,
                                                EXP_USECS ==
                                              X.fractionalDayInMicroseconds());
                                }
                            }
                        }
                    }
                }
            }
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX;

            ASSERT_PASS(mX.setInterval(0));

            ASSERT_PASS(mX.setInterval(k_DAYS_MAX));
            ASSERT_FAIL(mX.setInterval(k_DAYS_MAX, 24));

            ASSERT_PASS(mX.setInterval(k_DAYS_MIN));
            ASSERT_FAIL(mX.setInterval(k_DAYS_MIN, -24));

            ASSERT_FAIL(mX.setInterval(0, k_HOURS_MAX, k_MINS_MAX));
            ASSERT_PASS(mX.setInterval(0, k_HOURS_MAX, 0));
            ASSERT_PASS(mX.setInterval(0, k_HOURS_MAX, k_MINS_MIN));

            ASSERT_PASS(mX.setInterval(0, k_HOURS_MIN, k_MINS_MAX));
            ASSERT_PASS(mX.setInterval(0, k_HOURS_MIN, 0));
            ASSERT_FAIL(mX.setInterval(0, k_HOURS_MIN, k_MINS_MIN));
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
        //: 1 Create four test objects by using the default, initializing, and
        //:   copy constructors.
        //:
        //: 2 Exercise the basic value-semantic methods and the equality
        //:   operators using these test objects.
        //:
        //: 3 Invoke the primary manipulator, copy constructor, and assignment
        //:   operator without and with aliasing.
        //:
        //: 4 Use the basic accessors to verify the expected results.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const int DA = 1, USA =  1;  // day and microseconds for VA
        const int DB = 2, USB =  7;  // day and microseconds for VB
        const int DC = 3, USC = 21;  // day and microseconds for VC

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(DA, 0, 0, 0, 0, USA);  const Obj& X1 = mX1;
        if (verbose) { T_;  P(X1); }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(DA  == X1.days());
        ASSERT(USA == X1.fractionalDayInMicroseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { T_;  P(X2); }

        if (verbose) cout << "\ta. Check the initial state of x2." << endl;
        ASSERT(DA  == X2.days());
        ASSERT(USA == X2.fractionalDayInMicroseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;
        mX1.setInterval(DB, 0, 0, 0, 0, USB);
        if (verbose) { T_;  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(DB  == X1.days());
        ASSERT(USB == X1.fractionalDayInMicroseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create a default object x3()."
                             "\t\t{ x1:VB x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (verbose) { T_;  P(X3); }

        if (verbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(0 == X3.days());
        ASSERT(0 == X3.fractionalDayInMicroseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Create an object x4 (copy from x3)."
                             "\t\t{ x1:VA x2:VA x3:U  x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (verbose) { T_;  P(X4); }

        if (verbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(0 == X4.days());
        ASSERT(0 == X4.fractionalDayInMicroseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;
        mX3.setInterval(DC, 0, 0, 0, 0, USC);

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(DC  == X3.days());
        ASSERT(USC == X3.fractionalDayInMicroseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Assign x2 = x1."
                             "\t\t\t\t{ x1:VB x2:VB x3:VC x4:U }" << endl;
        mX2 = X1;
        if (verbose) { T_;  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(DB  == X2.days());
        ASSERT(USB == X2.fractionalDayInMicroseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x3."
                             "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;
        mX2 = X3;
        if (verbose) { T_;  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(DC  == X2.days());
        ASSERT(USC == X2.fractionalDayInMicroseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x1 = x1 (aliasing)."
                             "\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;

        mX1 = X1;
        if (verbose) { T_;  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(DB  == X1.days());
        ASSERT(USB == X1.fractionalDayInMicroseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));
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
