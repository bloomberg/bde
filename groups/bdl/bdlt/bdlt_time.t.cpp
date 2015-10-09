// bdlt_time.t.cpp                                                    -*-C++-*-

#include <bdlt_time.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bsls_asserttest.h>

#include <bslx_byteinstream.h>
#include <bslx_byteoutstream.h>
#include <bslx_instreamfunctions.h>
#include <bslx_outstreamfunctions.h>
#include <bslx_testinstream.h>
#include <bslx_testinstreamexception.h>
#include <bslx_testoutstream.h>
#include <bslx_versionfunctions.h>

#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>     // 'strcmp'
#include <bsl_c_time.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_sstream.h>

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
//: o 'setTime'
//
// Basic Accessors:
//: o 'getTime'
//: o 'hour'
//: o 'minute'
//: o 'second'
//: o 'millisecond'
//
// This particular attribute class also provides a value constructor capable of
// creating an object in any state relevant for thorough testing, obviating the
// primitive generator function, 'gg', normally used for this purpose.  We will
// therefore follow our standard 10-case approach to testing value-semantic
// types except that we will leave case 3 empty.
//-----------------------------------------------------------------------------
//
// CLASS METHODS
// [16] bool isValid(int hour, int minute, int second, int ms);
// [10] static int maxSupportedBdexVersion(int versionSelector);
//
// CREATORS
// [ 2] Time();
// [11] Time(int hour, int minute, int second = 0, int millisecond = 0);
// [ 7] Time(const Time& original);
// [ 2] ~Time();
//
// MANIPULATORS
// [ 9] Time& operator=(const Time& rhs);
// [15] Time& operator+=(const DatetimeInterval& rhs);
// [15] Time& operator-=(const DatetimeInterval& rhs);
// [15] int addHours(int hours);
// [15] int addMinutes(int minutes);
// [15] int addSeconds(int seconds);
// [15] int addMilliseconds(int milliseconds);
// [15] int addInterval(const DatetimeInterval& interval);
// [15] int addTime(int hours, int minutes, int seconds, int msec);
// [12] void setHour(int hour);
// [12] void setMinute(int minute);
// [12] void setSecond(int second);
// [12] void setMillisecond(int millisecond);
// [ 2] void setTime(int hour, int min = 0, int sec = 0, int msec = 0);
// [17] int setTimeIfValid(int hr, int min = 0, int sec = 0, int ms = 0);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
//
// ACCESSORS
// [ 4] void getTime(int *hour, int *min, int *second, int *msec) const;
// [ 4] int hour() const;
// [ 4] int minute() const;
// [ 4] int second() const;
// [ 4] int millisecond() const;
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 5] ostream& print(ostream& os, int level = 0, int spl = 4) const;
//
// FREE OPERATORS
// [14] DatetimeInterval operator-(const Time& lhs, const Time& rhs);
// [ 6] bool operator==(const Time& lhs, const Time& rhs);
// [ 6] bool operator!=(const Time& lhs, const Time& rhs);
// [13] bool operator< (const Time& lhs, const Time& rhs);
// [13] bool operator<=(const Time& lhs, const Time& rhs);
// [13] bool operator> (const Time& lhs, const Time& rhs);
// [13] bool operator>=(const Time& lhs, const Time& rhs);
// [ 5] ostream& operator<<(ostream& stream, const Time& time);
// [15] Time operator+(const Time&, const DatetimeInterval&);
// [15] Time operator+(const DatetimeInterval&, const Time&);
// [15] Time operator-(const Time&, const DatetimeInterval&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] Obj& gg(Obj *object, const char *spec);
// [18] USAGE EXAMPLE
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

typedef bdlt::Time          Obj;
typedef bslx::TestInStream  In;
typedef bslx::TestOutStream Out;

#define VERSION_SELECTOR 20140601

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

    switch (test) { case 0:
      case 18: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
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

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic 'bdlt::Time' Usage
///- - - - - - - - - - - - - - - - - -
// This example demonstrates how to create and use a 'bdlt::Time' object.
//
// First, create an object 't1' having the default value, and then verify that
// it represents the value 24:00:00.000:
//..
    bdlt::Time t1;               ASSERT(24 == t1.hour());
                                 ASSERT( 0 == t1.minute());
                                 ASSERT( 0 == t1.second());
                                 ASSERT( 0 == t1.millisecond());
//..
// Then, set 't1' to the value 2:34pm (14:34:00.000):
//..
    t1.setTime(14, 34);          ASSERT(14 == t1.hour());
                                 ASSERT(34 == t1.minute());
                                 ASSERT( 0 == t1.second());
                                 ASSERT( 0 == t1.millisecond());
//..
// Next, use 'setTimeIfValid' to attempt to assign the invalid value 24:15 to
// 't1', then verify the method returns an error status and the value of 't1'
// is unmodified:
//..
    int ret = t1.setTimeIfValid(24, 15);
                                 ASSERT( 0 != ret);          // 24:15 is not
                                                             // valid

                                 ASSERT(14 == t1.hour());    // no effect
                                 ASSERT(34 == t1.minute());  // on the
                                 ASSERT( 0 == t1.second());  // object
                                 ASSERT( 0 == t1.millisecond());
//..
// Then, create 't2' as a copy of 't1':
//..
    bdlt::Time t2(t1);            ASSERT(t1 == t2);
//..
// Next, add 5 minutes and 7 seconds to the value of 't2' (in two steps), and
// confirm the value of 't2':
//..
    t2.addMinutes(5);
    t2.addSeconds(7);
                                 ASSERT(14 == t2.hour());
                                 ASSERT(39 == t2.minute());
                                 ASSERT( 7 == t2.second());
                                 ASSERT( 0 == t2.millisecond());
//..
// Then, subtract 't1' from 't2' to yield a 'bdlt::DatetimeInterval' 'dt'
// representing the time-interval between those two times, and verify the value
// of 'dt' is 5 minutes and 7 seconds (or 307 seconds):
//..
    bdlt::DatetimeInterval dt = t2 - t1;
                                 ASSERT(307 == dt.totalSeconds());
//..
// Finally, stream the value of 't2' to 'stdout':
//..
if (veryVerbose)
    bsl::cout << t2 << bsl::endl;
//..
// The streaming operator produces the following output on 'stdout':
//..
//  14:39:07.000
//..

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'setTimeIfValid'
        //   Verify the method validates the arguments and, if valid, assigns
        //   the expected value.
        //
        // Concerns:
        //: 1 Each of the four integer fields are verified as valid values.
        //:
        //: 2 If the input is not valid, the value is unmodified.
        //:
        //: 3 If the input is valid, the value must be correctly set.
        //:
        //: 4 The special value 'hour == 24' is considered valid and the value
        //:   is correctly assigned.
        //
        // Plan:
        //: 1 Construct a table of valid and invalid inputs and compare results
        //:   to expected "valid" values.  (C-1..4)
        //
        // Testing:
        //   int setTimeIfValid(int hr, int min = 0, int sec = 0, int ms = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'setTimeIfValid'" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nTesting 'setTimeIfValid'." << endl;
        {
            static const struct {
                int d_lineNum;      // source line number
                int d_hour;         // specification hour
                int d_minute;       // specification minute
                int d_second;       // specification second
                int d_millisecond;  // specification millisecond
                int d_valid;        // expected return value
            } DATA[] = {
                //LINE HOUR   MIN  SEC   MSEC   VALID
                //---- ----   ---  ---   ----   ------
                { L_,     0,    0,   0,     0,    1   },

                { L_,     0,    0,   0,    -1,    0   },
                { L_,     0,    0,   0,   999,    1   },
                { L_,     0,    0,   0,  1000,    0   },

                { L_,     0,    0,  -1,     0,    0   },
                { L_,     0,    0,  59,     0,    1   },
                { L_,     0,    0,  60,     0,    0   },

                { L_,     0,   -1,   0,     0,    0   },
                { L_,     0,   59,   0,     0,    1   },
                { L_,     0,   60,   0,     0,    0   },

                { L_,    -1,    0,   0,     0,    0   },
                { L_,    23,    0,   0,     0,    1   },
                { L_,    24,    0,   0,     0,    1   },
                { L_,    25,    0,   0,     0,    0   },

                { L_,    24,    0,   0,     1,    0   },
                { L_,    24,    0,   1,     0,    0   },
                { L_,    24,    1,   0,     0,    0   },

                { L_,    23,   59,  59,   999,    1   },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE         = DATA[i].d_lineNum;
                const int HOUR         = DATA[i].d_hour;
                const int MINUTE       = DATA[i].d_minute;
                const int SECOND       = DATA[i].d_second;
                const int MILLISECOND  = DATA[i].d_millisecond;
                const int VALID        = DATA[i].d_valid;

                if (veryVerbose) { T_; P_(LINE);   P_(VALID);
                                       P_(HOUR);   P_(MINUTE);
                                       P_(SECOND); P(MILLISECOND);
                }
                Obj x;  const Obj& X = x;
                if (1 == VALID) {
                    const Obj R(HOUR, MINUTE, SECOND, MILLISECOND);
                    LOOP_ASSERT(LINE,
                                0 == x.setTimeIfValid(HOUR,
                                                          MINUTE,
                                                          SECOND,
                                                          MILLISECOND));
                    LOOP_ASSERT(LINE, R == X);

                    if (veryVerbose) { P_(VALID);  P_(R);  P(X); }
                }
                else {
                    const Obj R;
                    LOOP_ASSERT(LINE,
                                -1 == x.setTimeIfValid(HOUR,
                                                           MINUTE,
                                                           SECOND,
                                                           MILLISECOND));
                    LOOP_ASSERT(LINE, R == X);

                    if (veryVerbose) { P_(VALID);  P_(R);  P(X); }
                }
            }
        }

        if (verbose) {
            cout << "\nTesting that 'setTimeIfValid' works with just one "
                 << "argument." << endl;
        }
        {
            Obj x; const Obj& X = x;
            x.setTimeIfValid(22);

            if (veryVerbose) P(X);
            ASSERT(22 == X.hour());
            ASSERT( 0 == X.minute());
            ASSERT( 0 == X.second());
            ASSERT( 0 == X.millisecond());
        }

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'isValid'
        //   Verify the method correctly determines if the specified time is
        //   valid.  The special case of 'hour == 24' must also be verified.
        //
        // Concerns:
        //: 1 Each field is tested to ensure the specified time is valid.
        //:
        //: 2 The special value of 'hour == 24' is valid.
        //
        // Plan:
        //: 1 Construct a table of valid and invalid inputs and compare results
        //:   to the expected values.  (C-1,2)
        //
        // Testing:
        //   bool isValid(int hour, int minute, int second, int ms);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'isValid'" << endl
                          << "=================" << endl;

        if (verbose) cout << "\nTesting 'isValid'." << endl;
        {
            static const struct {
                int d_lineNum;      // source line number
                int d_hour;         // specification hour
                int d_minute;       // specification minute
                int d_second;       // specification second
                int d_millisecond;  // specification millisecond
                int d_valid;        // expected return value
            } DATA[] = {
                //LINE HOUR   MIN  SEC   MSEC   VALID
                //---- ----   ---  ---   ----   ------
                { L_,     0,    0,   0,     0,    1   },

                { L_,     0,    0,   0,    -1,    0   },
                { L_,     0,    0,   0,   999,    1   },
                { L_,     0,    0,   0,  1000,    0   },

                { L_,     0,    0,  -1,     0,    0   },
                { L_,     0,    0,  59,     0,    1   },
                { L_,     0,    0,  60,     0,    0   },

                { L_,     0,   -1,   0,     0,    0   },
                { L_,     0,   59,   0,     0,    1   },
                { L_,     0,   60,   0,     0,    0   },

                { L_,    -1,    0,   0,     0,    0   },
                { L_,    23,    0,   0,     0,    1   },
                { L_,    24,    0,   0,     0,    1   },
                { L_,    25,    0,   0,     0,    0   },

                { L_,    24,    0,   0,     1,    0   },
                { L_,    24,    0,   1,     0,    0   },
                { L_,    24,    1,   0,     0,    0   },

                { L_,    23,   59,  59,   999,    1   },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE         = DATA[i].d_lineNum;
                const int HOUR         = DATA[i].d_hour;
                const int MINUTE       = DATA[i].d_minute;
                const int SECOND       = DATA[i].d_second;
                const int MILLISECOND  = DATA[i].d_millisecond;
                const int VALID        = DATA[i].d_valid;

                if (veryVerbose) { T_; P_(LINE);   P_(VALID);
                                       P_(HOUR);   P_(MINUTE);
                                       P_(SECOND); P(MILLISECOND);
                }
                LOOP_ASSERT(LINE,
                            VALID == bdlt::Time::isValid(HOUR,   MINUTE,
                                                         SECOND, MILLISECOND));
            }
        }

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING INTERVAL ADD METHODS
        //   Verify the 'add*' methods, addition operators, and subtraction
        //   operators for intervals work as expected.
        //
        // Concerns:
        //: 1 The numerical constants used to generate the modified object
        //:   value and the return value are correct.
        //:
        //: 2 The correct result is obtained.
        //:
        //: 3 The default value, 24:00:00.000, is correctly handled.
        //
        // Plan:
        //: 1 Test 'addTime' explicitly using tabulated data.  Specifically,
        //:   specify an arbitrary (but convenient) non-default value as an
        //:   initial value, and also use the default value 24:00:00.000.
        //:   Specify a set of (h, m, s, ms) tuples to be used as arguments to
        //:   'addTime' and verify that both the modified object value and the
        //:   return value are correct for each of the two initial values.
        //:
        //: 2 Use the tested 'addTime' as an oracle to test the other four
        //:   methods in a loop-based test.  Specifically, specify a (negative)
        //:   start value, a (positive) stop value, and a step size for each of
        //:   the four fields (h, m, s, ms).  Loop over each of these four
        //:   ranges, calling the appropriate 'add' method on the default
        //:   object value and a second non-default object value, and using
        //:   'addTime' as an oracle to verify the resulting object values and
        //:   return values for the 'add' method under test.  (C-1..3)
        //
        // Testing:
        //   int addHours(int hours);
        //   int addMinutes(int minutes);
        //   int addSeconds(int seconds);
        //   int addMilliseconds(int milliseconds);
        //   int addInterval(const DatetimeInterval& interval);
        //   int addTime(int hours, int minutes, int seconds, int msec);
        //   Time& operator+=(const DatetimeInterval& rhs);
        //   Time& operator-=(const DatetimeInterval& rhs);
        //   Time operator+(const Time&, const DatetimeInterval&);
        //   Time operator+(const DatetimeInterval&, const Time&);
        //   Time operator-(const Time&, const DatetimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING INTERVAL ADD METHODS" << endl
                          << "============================" << endl;

        if (verbose) {
            cout << "\nTesting 'addTime' with the default initial value."
                 << endl;
        }
        {
            static const struct {
                int d_lineNum;       // source line number
                int d_hours;         // hours to add
                int d_minutes;       // minutes to add
                int d_seconds;       // seconds to add
                int d_msecs;         // milliseconds to add
                int d_expDays;       // expected return value (days)
                int d_expHour;       // expected hour value
                int d_expMinute;     // expected minute value
                int d_expSecond;     // expected second value
                int d_expMsec;       // expected milliseconds value
            } DATA[] = {
        //        - - - - - - time added - - - - - -   ---expected values---
        //LINE     H       M         S           MS    DAYS  H   M   S   MS
        //------   --      --        --          ---   ----  --  --  --  ---
        { L_,       0,      0,        0,           0,     0,  0,  0,  0,   0 },
        { L_,       0,      0,        0,           1,     0,  0,  0,  0,   1 },
        { L_,       0,      0,        0,          -1,    -1, 23, 59, 59, 999 },
        { L_,       0,      0,        0,        1000,     0,  0,  0,  1,   0 },
        { L_,       0,      0,        0,       60000,     0,  0,  1,  0,   0 },
        { L_,       0,      0,        0,     3600000,     0,  1,  0,  0,   0 },
        { L_,       0,      0,        0,    86400000,     1,  0,  0,  0,   0 },
        { L_,       0,      0,        0,   -86400000,    -1,  0,  0,  0,   0 },
        { L_,       0,      0,        0,   864000000,    10,  0,  0,  0,   0 },

        { L_,       0,      0,        1,           0,     0,  0,  0,  1,   0 },
        { L_,       0,      0,       -1,           0,    -1, 23, 59, 59,   0 },
        { L_,       0,      0,       60,           0,     0,  0,  1,  0,   0 },
        { L_,       0,      0,     3600,           0,     0,  1,  0,  0,   0 },
        { L_,       0,      0,    86400,           0,     1,  0,  0,  0,   0 },
        { L_,       0,      0,   -86400,           0,    -1,  0,  0,  0,   0 },
        { L_,       0,      0,   864000,           0,    10,  0,  0,  0,   0 },

        { L_,       0,      1,        0,           0,     0,  0,  1,  0,   0 },
        { L_,       0,     -1,        0,           0,    -1, 23, 59,  0,   0 },
        { L_,       0,     60,        0,           0,     0,  1,  0,  0,   0 },
        { L_,       0,   1440,        0,           0,     1,  0,  0,  0,   0 },
        { L_,       0,  -1440,        0,           0,    -1,  0,  0,  0,   0 },
        { L_,       0,  14400,        0,           0,    10,  0,  0,  0,   0 },

        { L_,       1,      0,        0,           0,     0,  1,  0,  0,   0 },
        { L_,      -1,      0,        0,           0,    -1, 23,  0,  0,   0 },
        { L_,      24,      0,        0,           0,     1,  0,  0,  0,   0 },
        { L_,     -24,      0,        0,           0,    -1,  0,  0,  0,   0 },
        { L_,     240,      0,        0,           0,    10,  0,  0,  0,   0 },

        { L_,      24,   1440,    86400,    86400000,     4,  0,  0,  0,   0 },
        { L_,      24,   1440,    86400,   -86400000,     2,  0,  0,  0,   0 },
        { L_,      24,   1440,   -86400,   -86400000,     0,  0,  0,  0,   0 },
        { L_,      24,  -1440,   -86400,   -86400000,    -2,  0,  0,  0,   0 },
        { L_,     -24,  -1440,   -86400,   -86400000,    -4,  0,  0,  0,   0 },
        { L_,      25,   1441,    86401,    86400001,     4,  1,  1,  1,   1 },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE     = DATA[i].d_lineNum;
                const int HOURS    = DATA[i].d_hours;
                const int MINUTES  = DATA[i].d_minutes;
                const int SECONDS  = DATA[i].d_seconds;
                const int MSECS    = DATA[i].d_msecs;

                const int EXP_DAYS = DATA[i].d_expDays;
                const int EXP_HR   = DATA[i].d_expHour;
                const int EXP_MIN  = DATA[i].d_expMinute;
                const int EXP_SEC  = DATA[i].d_expSecond;
                const int EXP_MSEC = DATA[i].d_expMsec;

                Obj x;  const Obj& X = x;

                if (veryVerbose) { T_;  P_(X); }

                int RETURN_VALUE = x.addTime(HOURS, MINUTES, SECONDS, MSECS);

                const Obj EXP(EXP_HR, EXP_MIN, EXP_SEC, EXP_MSEC);

                if (veryVerbose) { P_(X);  P_(EXP);  P(RETURN_VALUE); }

                LOOP_ASSERT(LINE, EXP      == X);
                LOOP_ASSERT(LINE, EXP_DAYS == RETURN_VALUE);
            }
        }

        if (verbose) {
            cout << "\nTesting 'addTime' with a non-default initial value."
                 << endl;
        }
        {
            static const struct {
                int d_lineNum;       // source line number
                int d_hours;         // hours to add
                int d_minutes;       // minutes to add
                int d_seconds;       // seconds to add
                int d_msecs;         // milliseconds to add
                int d_expDays;       // expected return value (days)
                int d_expHour;       // expected hour value
                int d_expMinute;     // expected minute value
                int d_expSecond;     // expected second value
                int d_expMsec;       // expected milliseconds value
            } DATA[] = {
        //        - - - - - - time added - - - - - -   ---expected values---
        //LINE     H       M         S           MS    DAYS  H   M   S   MS
        //------   --      --        --          ---   ----  --  --  --  ---
        { L_,       0,      0,        0,           0,     0, 12,  0,  0,   0 },
        { L_,       0,      0,        0,           1,     0, 12,  0,  0,   1 },
        { L_,       0,      0,        0,          -1,     0, 11, 59, 59, 999 },
        { L_,       0,      0,        0,        1000,     0, 12,  0,  1,   0 },
        { L_,       0,      0,        0,       60000,     0, 12,  1,  0,   0 },
        { L_,       0,      0,        0,     3600000,     0, 13,  0,  0,   0 },
        { L_,       0,      0,        0,    86400000,     1, 12,  0,  0,   0 },
        { L_,       0,      0,        0,   -86400000,    -1, 12,  0,  0,   0 },
        { L_,       0,      0,        0,   864000000,    10, 12,  0,  0,   0 },

        { L_,       0,      0,        1,           0,     0, 12,  0,  1,   0 },
        { L_,       0,      0,       -1,           0,     0, 11, 59, 59,   0 },
        { L_,       0,      0,       60,           0,     0, 12,  1,  0,   0 },
        { L_,       0,      0,     3600,           0,     0, 13,  0,  0,   0 },
        { L_,       0,      0,    86400,           0,     1, 12,  0,  0,   0 },
        { L_,       0,      0,   -86400,           0,    -1, 12,  0,  0,   0 },
        { L_,       0,      0,   864000,           0,    10, 12,  0,  0,   0 },

        { L_,       0,      1,        0,           0,     0, 12,  1,  0,   0 },
        { L_,       0,     -1,        0,           0,     0, 11, 59,  0,   0 },
        { L_,       0,     60,        0,           0,     0, 13,  0,  0,   0 },
        { L_,       0,   1440,        0,           0,     1, 12,  0,  0,   0 },
        { L_,       0,  -1440,        0,           0,    -1, 12,  0,  0,   0 },
        { L_,       0,  14400,        0,           0,    10, 12,  0,  0,   0 },

        { L_,       1,      0,        0,           0,     0, 13,  0,  0,   0 },
        { L_,      -1,      0,        0,           0,     0, 11,  0,  0,   0 },
        { L_,      24,      0,        0,           0,     1, 12,  0,  0,   0 },
        { L_,     -24,      0,        0,           0,    -1, 12,  0,  0,   0 },
        { L_,     240,      0,        0,           0,    10, 12,  0,  0,   0 },

        { L_,      24,   1440,    86400,    86400000,     4, 12,  0,  0,   0 },
        { L_,      24,   1440,    86400,   -86400000,     2, 12,  0,  0,   0 },
        { L_,      24,   1440,   -86400,   -86400000,     0, 12,  0,  0,   0 },
        { L_,      24,  -1440,   -86400,   -86400000,    -2, 12,  0,  0,   0 },
        { L_,     -24,  -1440,   -86400,   -86400000,    -4, 12,  0,  0,   0 },
        { L_,      25,   1441,    86401,    86400001,     4, 13,  1,  1,   1 },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            const Obj INITIAL(12, 0, 0, 0);  // arbitrary but convenient value

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE     = DATA[i].d_lineNum;
                const int HOURS    = DATA[i].d_hours;
                const int MINUTES  = DATA[i].d_minutes;
                const int SECONDS  = DATA[i].d_seconds;
                const int MSECS    = DATA[i].d_msecs;

                const int EXP_DAYS = DATA[i].d_expDays;
                const int EXP_HR   = DATA[i].d_expHour;
                const int EXP_MIN  = DATA[i].d_expMinute;
                const int EXP_SEC  = DATA[i].d_expSecond;
                const int EXP_MSEC = DATA[i].d_expMsec;

                Obj x(INITIAL);  const Obj& X = x;

                if (veryVerbose) { T_;  P_(X); }

                int RETURN_VALUE = x.addTime(HOURS, MINUTES, SECONDS, MSECS);

                const Obj EXP(EXP_HR, EXP_MIN, EXP_SEC, EXP_MSEC);

                if (veryVerbose) { P_(X);  P_(EXP);  P(RETURN_VALUE); }

                LOOP_ASSERT(LINE, EXP      == X);
                LOOP_ASSERT(LINE, EXP_DAYS == RETURN_VALUE);
            }
        }

        {
            const Obj I1;            // default initial object value
            const Obj I2(12, 0, 0);  // non-default initial object value

            const int START_HOURS = -250;
            const int STOP_HOURS  =  250;
            const int STEP_HOURS  =   25;

            const int START_MINS  = -15000;
            const int STOP_MINS   =  15000;
            const int STEP_MINS   =   1500;

            const int START_SECS  = -900000;
            const int STOP_SECS   =  900000;
            const int STEP_SECS   =   90000;

            const int START_MSECS = -900000000;
            const int STOP_MSECS  =  900000000;
            const int STEP_MSECS  =   90000000;

            if (verbose) cout << "\nTesting 'addHours'." << endl;
            for (int hi = START_HOURS; hi <= STOP_HOURS; hi += STEP_HOURS) {
                Obj x1(I1);  const Obj &X1 = x1;

                Obj x2(I2);  const Obj &X2 = x2;

                Obj y1(I1);  const Obj &Y1 = y1;

                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) { T_;  P_(X1);  P(X2); }

                const int RX1 = x1.addHours(hi);
                const int RX2 = x2.addHours(hi);
                const int RY1 = y1.addTime(hi, 0, 0, 0);
                const int RY2 = y2.addTime(hi, 0, 0, 0);

                if (veryVerbose) { T_;  P_(X1);  P_(X2);  P_(RX1);  P(RX2); }

                LOOP_ASSERT(hi, Y1 == X1);  LOOP_ASSERT(hi, RY1 == RX1);
                LOOP_ASSERT(hi, Y2 == X2);  LOOP_ASSERT(hi, RY2 == RX2);
            }

            if (verbose) cout << "\nTesting 'addMinutes'." << endl;
            for (int mi = START_MINS; mi <= STOP_MINS; mi += STEP_MINS) {
                Obj x1(I1);  const Obj &X1 = x1;

                Obj x2(I2);  const Obj &X2 = x2;

                Obj y1(I1);  const Obj &Y1 = y1;

                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) {T_;   P_(X1);  P(X2); }

                const int RX1 = x1.addMinutes(mi);
                const int RX2 = x2.addMinutes(mi);
                const int RY1 = y1.addTime(0, mi, 0, 0);
                const int RY2 = y2.addTime(0, mi, 0, 0);

                if (veryVerbose) { T_;  P_(X1);  P_(X2);  P_(RX1);  P(RX2); }

                LOOP_ASSERT(mi, Y1 == X1);  LOOP_ASSERT(mi, RY1 == RX1);
                LOOP_ASSERT(mi, Y2 == X2);  LOOP_ASSERT(mi, RY2 == RX2);
            }

            if (verbose) cout << "\nTesting 'addSeconds'." << endl;
            for (int si = START_SECS; si <= STOP_SECS; si += STEP_SECS) {
                Obj x1(I1);  const Obj &X1 = x1;

                Obj x2(I2);  const Obj &X2 = x2;

                Obj y1(I1);  const Obj &Y1 = y1;

                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) { T_;  P_(X1);  P(X2); }

                const int RX1 = x1.addSeconds(si);
                const int RX2 = x2.addSeconds(si);
                const int RY1 = y1.addTime(0, 0, si, 0);
                const int RY2 = y2.addTime(0, 0, si, 0);

                if (veryVerbose) { T_;  P_(X1);  P_(X2);  P_(RX1);  P(RX2); }

                LOOP_ASSERT(si, Y1 == X1);  LOOP_ASSERT(si, RY1 == RX1);
                LOOP_ASSERT(si, Y2 == X2);  LOOP_ASSERT(si, RY2 == RX2);
            }

            if (verbose) cout << "\nTesting 'addMilliseconds'." << endl;
            for (int msi = START_MSECS; msi <= STOP_MSECS; msi += STEP_MSECS) {
                Obj x1(I1);  const Obj &X1 = x1;

                Obj x2(I2);  const Obj &X2 = x2;

                Obj y1(I1);  const Obj &Y1 = y1;

                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) { T_;  P_(X1);  P(X2); }

                const int RX1 = x1.addMilliseconds(msi);
                const int RX2 = x2.addMilliseconds(msi);
                const int RY1 = y1.addTime(0, 0, 0, msi);
                const int RY2 = y2.addTime(0, 0, 0, msi);

                if (veryVerbose) { T_;  P_(X1);  P_(X2);  P_(RX1);  P(RX2); }

                LOOP_ASSERT(msi, Y1 == X1);  LOOP_ASSERT(msi, RY1 == RX1);
                LOOP_ASSERT(msi, Y2 == X2);  LOOP_ASSERT(msi, RY2 == RX2);
            }

            if (verbose) cout << "\nTesting 'addInterval'." << endl;
            for (int msi = START_MSECS; msi <= STOP_MSECS; msi += STEP_MSECS) {
                Obj x1(I1);  const Obj &X1 = x1;

                Obj x2(I2);  const Obj &X2 = x2;

                Obj y1(I1);  const Obj &Y1 = y1;

                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) { T_;  P_(X1);  P(X2); }

                const bdlt::DatetimeInterval INTERVAL(0, 0, 0, 0, msi);

                const int RX1 = x1.addInterval(INTERVAL);
                const int RX2 = x2.addInterval(INTERVAL);
                const int RY1 = y1.addTime(0, 0, 0, msi);
                const int RY2 = y2.addTime(0, 0, 0, msi);

                if (veryVerbose) { T_;  P_(X1);  P_(X2);  P_(RX1);  P(RX2); }

                LOOP_ASSERT(msi, Y1 == X1);  LOOP_ASSERT(msi, RY1 == RX1);
                LOOP_ASSERT(msi, Y2 == X2);  LOOP_ASSERT(msi, RY2 == RX2);
            }

            if (verbose) {
                cout << "\nTesting 'operator+=' and 'operator-='." << endl;
            }
            for (int msi = START_MSECS; msi <= STOP_MSECS; msi += STEP_MSECS) {
                Obj x1(I1);  const Obj &X1 = x1;

                Obj x2(I2);  const Obj &X2 = x2;

                Obj y1(I1);  const Obj &Y1 = y1;

                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) { T_;  P_(X1);  P(X2); }

                const bdlt::DatetimeInterval INTERVAL(0, 0, 0, 0, msi);

                ASSERT(&x1 == &(x1 += INTERVAL));
                ASSERT(&x2 == &(x2 += INTERVAL));
                y1.addTime(0, 0, 0, msi);
                y2.addTime(0, 0, 0, msi);

                if (veryVerbose) { T_;  P_(X1);  P(X2); }

                LOOP_ASSERT(msi, Y1 == X1);
                LOOP_ASSERT(msi, Y2 == X2);

                ASSERT(&x1 == &(x1 -= INTERVAL));
                ASSERT(&x2 == &(x2 -= INTERVAL));
                y1.addTime(0, 0, 0, -msi);
                y2.addTime(0, 0, 0, -msi);

                LOOP_ASSERT(msi, Y1 == X1);
                LOOP_ASSERT(msi, Y2 == X2);
            }

            if (verbose) {
                cout << "\nTesting 'operator+' and 'operator-'." << endl;
            }
            for (int msi = START_MSECS; msi <= STOP_MSECS; msi += STEP_MSECS) {

                const bdlt::DatetimeInterval INTERVAL(0, 0, 0, 0, msi);

                {
                    Obj y1(I1);  const Obj &Y1 = y1;

                    Obj y2(I2);  const Obj &Y2 = y2;

                    Obj x1 = I1 + INTERVAL;  const Obj &X1 = x1;

                    Obj x2 = I2 + INTERVAL;  const Obj &X2 = x2;

                    y1.addTime(0, 0, 0, msi);
                    y2.addTime(0, 0, 0, msi);

                    LOOP_ASSERT(msi, Y1 == X1);
                    LOOP_ASSERT(msi, Y2 == X2);

                    Obj x3 = x1 - INTERVAL;  const Obj &X3 = x3;

                    Obj x4 = x2 - INTERVAL;  const Obj &X4 = x4;

                    y1.addTime(0, 0, 0, -msi);
                    y2.addTime(0, 0, 0, -msi);

                    if (veryVerbose) { P_(X1) P_(Y1) P_(X2) P(Y2); }

                    LOOP_ASSERT(msi, Y1 == X3);
                    LOOP_ASSERT(msi, Y2 == X4);
                }

                {
                    Obj y1(I1);  const Obj &Y1 = y1;

                    Obj y2(I2);  const Obj &Y2 = y2;

                    Obj x1 = INTERVAL + I1;  const Obj &X1 = x1;

                    Obj x2 = INTERVAL + I2;  const Obj &X2 = x2;

                    y1.addTime(0, 0, 0, msi);
                    y2.addTime(0, 0, 0, msi);

                    if (veryVerbose) { P_(X1) P_(Y1) P_(X2) P(Y2); }

                    LOOP_ASSERT(msi, Y1 == X1);
                    LOOP_ASSERT(msi, Y2 == X2);
                }
            }

        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj mV;
            Obj mW;
            Obj mX;
            Obj mY;
            Obj mZ(1);

            const bdlt::DatetimeInterval INTERVAL_A(0, 0, 0, 0, 0);
            const bdlt::DatetimeInterval INTERVAL_B(
                                              bsl::numeric_limits<int>::min());
            const bdlt::DatetimeInterval INTERVAL_C(
                                               bsl::numeric_limits<int>::max(),
                                               23,
                                               59,
                                               59,
                                               999);
            const bdlt::DatetimeInterval INTERVAL_D(
                                               bsl::numeric_limits<int>::min(),
                                               0,
                                               0,
                                               0,
                                               -1);
            const bdlt::DatetimeInterval INTERVAL_E(
                                               bsl::numeric_limits<int>::max(),
                                               23);

            ASSERT_SAFE_PASS(mV.addInterval(INTERVAL_A));
            ASSERT_SAFE_PASS(mW.addInterval(INTERVAL_B));
            ASSERT_SAFE_PASS(mX.addInterval(INTERVAL_C));
            ASSERT_SAFE_FAIL(mY.addInterval(INTERVAL_D));
            ASSERT_SAFE_FAIL(mZ.addInterval(INTERVAL_E));
        }

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING SUBTRACTION OPERATOR
        //   Verify the subtraction operator produces the expected result.
        //
        // Concerns:
        //: 1 Verify this operator performs a subtraction on the underlying
        //:   integer total-milliseconds representation and returns the
        //:   DatetimeInterval initialized with the result.
        //:
        //: 2 The default value, 24:00:00.000, is correctly handled.
        //
        // Plan:
        //: 1 Specify a set of object value pairs S.  For each (x1, x2) in S,
        //:   verify that both x2 - x1 and x1 - x2 produce the expected
        //:   results.  (C-1,2)
        //
        // Testing:
        //   DatetimeInterval operator-(const Time& lhs, const Time& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING SUBTRACTION OPERATOR" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting 'operator-'." << endl;
        {
            static const struct {
                int d_lineNum;     // source line number
                int d_hours2;      // lhs time hours
                int d_minutes2;    // lhs time minutes
                int d_seconds2;    // lhs time seconds
                int d_msecs2;      // lhs time milliseconds
                int d_hours1;      // rhs time hours
                int d_minutes1;    // rhs time minutes
                int d_seconds1;    // rhs time seconds
                int d_msecs1;      // rhs time milliseconds
                int d_expMsec;     // expected difference (msec)
            } DATA[] = {
                //        -- lhs time --    -- rhs time --
                //LINE    H   M   S   MS    H   M   S   MS     EXPECTED MSEC
                //------  --  --  --  ---   --  --  --  ---    -------------
                { L_,      0,  0,  0,   0,   0,  0,  0,   0,             0  },
                { L_,     24,  0,  0,   0,   0,  0,  0,   0,             0  },
                { L_,      0,  0,  0,   0,  24,  0,  0,   0,             0  },
                { L_,     24,  0,  0,   0,  24,  0,  0,   0,             0  },

                { L_,      0,  0,  0,   1,   0,  0,  0,   0,             1  },
                { L_,      0,  0,  0,   0,   0,  0,  0,   1,            -1  },
                { L_,      0,  0,  0,   1,  24,  0,  0,   0,             1  },
                { L_,     24,  0,  0,   0,   0,  0,  0,   1,            -1  },

                { L_,      0,  0,  1,   0,   0,  0,  0,   0,          1000  },
                { L_,      0,  0,  0,   0,   0,  0,  1,   0,         -1000  },
                { L_,      0,  0,  1,   0,  24,  0,  0,   0,          1000  },
                { L_,     24,  0,  0,   0,   0,  0,  1,   0,         -1000  },

                { L_,      0,  1,  0,   0,   0,  0,  0,   0,         60000  },
                { L_,      0,  0,  0,   0,   0,  1,  0,   0,        -60000  },
                { L_,      0,  1,  0,   0,  24,  0,  0,   0,         60000  },
                { L_,     24,  0,  0,   0,   0,  1,  0,   0,        -60000  },

                { L_,      1,  0,  0,   0,   0,  0,  0,   0,       3600000  },
                { L_,      0,  0,  0,   0,   1,  0,  0,   0,      -3600000  },
                { L_,      1,  0,  0,   0,  24,  0,  0,   0,       3600000  },
                { L_,     24,  0,  0,   0,   1,  0,  0,   0,      -3600000  },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE     = DATA[i].d_lineNum;
                const int HOURS2   = DATA[i].d_hours2;
                const int MINUTES2 = DATA[i].d_minutes2;
                const int SECONDS2 = DATA[i].d_seconds2;
                const int MSECS2   = DATA[i].d_msecs2;
                const int HOURS1   = DATA[i].d_hours1;
                const int MINUTES1 = DATA[i].d_minutes1;
                const int SECONDS1 = DATA[i].d_seconds1;
                const int MSECS1   = DATA[i].d_msecs1;
                const int EXP_MSEC = DATA[i].d_expMsec;

                const Obj X2(HOURS2, MINUTES2, SECONDS2, MSECS2);
                const Obj X1(HOURS1, MINUTES1, SECONDS1, MSECS1);

                const bdlt::DatetimeInterval INTERVAL1(X2 - X1);
                const bdlt::DatetimeInterval INTERVAL2(X1 - X2);

                if (veryVerbose) {
                    T_;  P_(X2);  P_(X1);  P(INTERVAL1.totalMilliseconds());
                }

                LOOP_ASSERT(LINE,  EXP_MSEC == INTERVAL1.totalMilliseconds());
                LOOP_ASSERT(LINE, -EXP_MSEC == INTERVAL2.totalMilliseconds());
            }
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING RELATIONAL OPERATORS
        //   Verify the relational operators evaluate correctly.
        //
        // Concerns:
        //: 1 Each operator invokes the corresponding operator on the
        //:   underlying integer total milliseconds correctly.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Specify an ordered set 'S' of unique object values.  For each
        //:   '(u, v)' in the set 'S x S', verify the result of 'u OP v' for
        //:   each 'OP' in '{<, <=, >=, >}'.  (C-1)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   bool operator< (const Time& lhs, const Time& rhs);
        //   bool operator<=(const Time& lhs, const Time& rhs);
        //   bool operator>=(const Time& lhs, const Time& rhs);
        //   bool operator> (const Time& lhs, const Time& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING RELATIONAL OPERATORS" << endl
                          << "============================" << endl;

        if (verbose) {
            cout << "\nTesting 'operator<', 'operator<=', 'operator>=', "
                 << "and 'operator>'." << endl;
        }
        {
            static const struct {
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },  {  0,  0,  0,   1  },
                {  0,  0,  0, 999  },  {  0,  0,  1,   0  },
                {  0,  0, 59, 999  },  {  0,  1,  0,   0  },
                {  0,  1,  0,   1  },  {  0, 59, 59, 999  },
                {  1,  0,  0,   0  },  {  1,  0,  0,   1  },
                { 23,  0,  0,   0  },  { 23, 22, 21, 209  },
                { 23, 22, 21, 210  },  { 23, 59, 59, 999  },
            };

            const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                    / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj        v(VALUES[i].d_hour,
                             VALUES[i].d_minute,
                             VALUES[i].d_second,
                             VALUES[i].d_msec);
                const Obj& V = v;

                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj        u(VALUES[j].d_hour,
                                 VALUES[j].d_minute,
                                 VALUES[j].d_second,
                                 VALUES[j].d_msec);
                    const Obj& U = u;
                    if (veryVerbose) { T_;  P_(i);  P_(j);  P_(V);  P(U); }
                    LOOP2_ASSERT(i, j, (j <  i) == (U <  V));
                    LOOP2_ASSERT(i, j, (j <= i) == (U <= V));
                    LOOP2_ASSERT(i, j, (j >= i) == (U >= V));
                    LOOP2_ASSERT(i, j, (j >  i) == (U >  V));
                }
            }
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj mX(1, 0, 0, 0);  const Obj X = mX;

            Obj mY(1, 0, 0, 0);  const Obj Y = mY;

            Obj mZ;              const Obj Z = mZ;

            ASSERT_SAFE_PASS(X <  Y);
            ASSERT_SAFE_FAIL(X <  Z);
            ASSERT_SAFE_FAIL(Z <  X);

            ASSERT_SAFE_PASS(X <= Y);
            ASSERT_SAFE_FAIL(X <= Z);
            ASSERT_SAFE_FAIL(Z <= X);

            ASSERT_SAFE_PASS(X >= Y);
            ASSERT_SAFE_FAIL(X >= Z);
            ASSERT_SAFE_FAIL(Z >= X);

            ASSERT_SAFE_PASS(X >  Y);
            ASSERT_SAFE_FAIL(X >  Z);
            ASSERT_SAFE_FAIL(Z >  X);
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
        //: 3 When the initial value is the default value, the methods work as
        //:   expected.
        //:
        //: 4 The special case of 'setHour(24)' sets the object to the default
        //:   value.
        //:
        //: 5 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For a set of independent test values not including the
        //:   default value (24:00:00.000), use the default constructor to
        //:   create an object and use the 'set' manipulators to set its value.
        //:   Verify the value using the basic accessors.  (C-1,2)
        //:
        //: 2 Confirm the correct behavior of the 'set' methods when
        //:   setting from the value 24:00:00.000.  (C-3)
        //:
        //: 3 Verify 'setHour(24)' results in the default value for the object.
        //:   (C-4)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-5)
        //
        // Testing:
        //   void setHour(int hour);
        //   void setMinute(int minute);
        //   void setSecond(int second);
        //   void setMillisecond(int millisecond);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ADDITIONAL 'set' MANIPULATORS" << endl
                          << "=====================================" << endl;

        if (verbose) cout << "\nTesting 'setXXX' methods." << endl;

        if (verbose) cout << "\tFor ordinary computational values." << endl;
        {
            static const struct {
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },  {  0,  0,  0, 999  },
                {  0,  0, 59,   0  },  {  0, 59,  0,   0  },
                { 23,  0,  0,   0  },  { 23, 22, 21, 209  },
                { 23, 22, 21, 210  },  // 24:00:00.000 NOT tested here
            };

            const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                    / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int HOUR   = VALUES[i].d_hour;
                const int MINUTE = VALUES[i].d_minute;
                const int SECOND = VALUES[i].d_second;
                const int MSEC   = VALUES[i].d_msec;

                Obj x;  const Obj& X = x;
                x.setHour(HOUR);
                x.setMinute(MINUTE);
                x.setSecond(SECOND);
                x.setMillisecond(MSEC);
                if (veryVerbose) {
                    T_; P_(HOUR); P_(MINUTE); P_(SECOND); P_(MSEC); P(X);
                }
                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, SECOND == X.second());
                LOOP_ASSERT(i, MSEC   == X.millisecond());
            }
            if (veryVerbose) cout << endl;
        }

        if (verbose) cout << "\tSetting from value 24:00:00.000." << endl;
        {
            const Obj R24;             // Reference object (24:00:00.000)

            Obj x;  const Obj& X = x;  if (veryVerbose) { T_;  P_(X); }
            x.setMinute(0);            if (veryVerbose) P(X);
            ASSERT( 0 == x.hour());    ASSERT(  0 == x.minute());
            ASSERT( 0 == x.second());  ASSERT(  0 == x.millisecond());

            x = R24;                   if (veryVerbose) { T_;  P_(X); }
            x.setMinute(59);           if (veryVerbose) P(X);
            ASSERT( 0 == x.hour());    ASSERT( 59 == x.minute());
            ASSERT( 0 == x.second());  ASSERT(  0 == x.millisecond());

            x = R24;                   if (veryVerbose) { T_;  P_(X); }
            x.setSecond(0);            if (veryVerbose) P(X);
            ASSERT( 0 == x.hour());    ASSERT(  0 == x.minute());
            ASSERT( 0 == x.second());  ASSERT(  0 == x.millisecond());

            x = R24;                   if (veryVerbose) { T_;  P_(X); }
            x.setSecond(59);           if (veryVerbose) P(X);
            ASSERT( 0 == x.hour());    ASSERT(  0 == x.minute());
            ASSERT(59 == x.second());  ASSERT(  0 == x.millisecond());

            x = R24;                   if (veryVerbose) { T_;  P_(X); }
            x.setMillisecond(0);       if (veryVerbose) P(X);
            ASSERT( 0 == x.hour());    ASSERT(  0 == x.minute());
            ASSERT( 0 == x.second());  ASSERT(  0 == x.millisecond());

            x = R24;                   if (veryVerbose) { T_;  P_(X); }
            x.setMillisecond(999);     if (veryVerbose) { P(X); cout << endl; }
            ASSERT( 0 == x.hour());    ASSERT(  0 == x.minute());
            ASSERT( 0 == x.second());  ASSERT(999 == x.millisecond());
        }

        if (verbose) cout << "\tSetting to value 24:00:00.000." << endl;
        {
            const Obj R(23, 22, 21, 209); // Reference object (21:22:21.209)

            Obj x(R); const Obj& X = x;if (veryVerbose) { T_;  P_(X); }
            x.setHour(24);             if (veryVerbose) { P(X); cout << endl; }
            ASSERT(24 == x.hour());    ASSERT(  0 == x.minute());
            ASSERT( 0 == x.second());  ASSERT(  0 == x.millisecond());

        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj mX;

            ASSERT_SAFE_FAIL(mX.setHour(-1));
            ASSERT_SAFE_PASS(mX.setHour( 0));
            ASSERT_SAFE_PASS(mX.setHour(24));
            ASSERT_SAFE_FAIL(mX.setHour(25));

            ASSERT_SAFE_FAIL(mX.setMinute(-1));
            ASSERT_SAFE_PASS(mX.setMinute( 0));
            ASSERT_SAFE_PASS(mX.setMinute(59));
            ASSERT_SAFE_FAIL(mX.setMinute(60));

            ASSERT_SAFE_FAIL(mX.setSecond(-1));
            ASSERT_SAFE_PASS(mX.setSecond( 0));
            ASSERT_SAFE_PASS(mX.setSecond(59));
            ASSERT_SAFE_FAIL(mX.setSecond(60));

            ASSERT_SAFE_FAIL(mX.setMillisecond(  -1));
            ASSERT_SAFE_PASS(mX.setMillisecond(   0));
            ASSERT_SAFE_PASS(mX.setMillisecond( 999));
            ASSERT_SAFE_FAIL(mX.setMillisecond(1000));
        }


      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZING CONSTRUCTOR
        //   Verify the initializing constructor works as expected.
        //
        // Concerns:
        //: 1 The two required parameters and two optional parameters are
        //:   multiplied by the appropriate factors when initializing the
        //:   internal total-milliseconds integer representation.
        //:
        //: 2 The default value 24:00:00.000 must be constructible explicitly.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Specify a set 'S' of times as '(h, m, s, ms)' tuples having
        //:   widely varying values.  For each '(h, m, s, ms)' in 'S',
        //:   construct an object 'X' using all four arguments and an object
        //:   'Y' using the first three arguments, and verify that 'X' and 'Y'
        //:   have the expected values.  (C-1)
        //:
        //: 2 Test explicitly that the initializing constructor can create an
        //:   object having the default value.  (C-2)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   Time(int hour, int minute, int second = 0, int millisecond = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING INITIALIZING CONSTRUCTOR" << endl
                          << "================================" << endl;

        if (verbose) cout << "\nFor ordinary computational values." << endl;
        {
            static const struct {
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },  {  0,  0,  0, 999  },
                {  0,  0, 59,   0  },  {  0, 59,  0,   0  },
                { 23,  0,  0,   0  },  { 23, 22, 21, 209  },
                { 23, 22, 21, 210  },  { 23, 59, 59, 999  },
            };

            const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                    / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int HOUR   = VALUES[i].d_hour;
                const int MINUTE = VALUES[i].d_minute;
                const int SECOND = VALUES[i].d_second;
                const int MSEC   = VALUES[i].d_msec;

                Obj x(HOUR, MINUTE, SECOND, MSEC);  const Obj& X = x;

                Obj y(HOUR, MINUTE, SECOND);        const Obj& Y = y;

                Obj z(HOUR, MINUTE);                const Obj& Z = z;

                if (veryVerbose) {
                    T_; P_(HOUR); P_(MINUTE); P_(SECOND); P_(MSEC); P_(X);
                    P_(Y)  P(Z);
                }
                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, SECOND == X.second());
                LOOP_ASSERT(i, MSEC   == X.millisecond());
                LOOP_ASSERT(i, HOUR   == Y.hour());
                LOOP_ASSERT(i, MINUTE == Y.minute());
                LOOP_ASSERT(i, SECOND == Y.second());
                LOOP_ASSERT(i, 0      == Y.millisecond());
                LOOP_ASSERT(i, HOUR   == Z.hour());
                LOOP_ASSERT(i, MINUTE == Z.minute());
                LOOP_ASSERT(i, 0      == Z.second());
                LOOP_ASSERT(i, 0      == Z.millisecond());
            }
        }

        if (verbose) cout << "\nFor the default values." << endl;
        {
            Obj d;               const Obj& D = d;

            Obj x(24, 0, 0, 0);  const Obj& X = x;

            Obj y(24, 0, 0);     const Obj& Y = y;

            Obj z(24, 0);        const Obj& Z = z;

            if (veryVerbose) { T_;  P_(D);  P_(X);  P_(Y)  P(Z); }

            ASSERT(D == X);
            ASSERT(D == Y);
            ASSERT(D == Z);
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_PASS(Obj mX(24, 0, 0, 0));
            ASSERT_SAFE_FAIL(Obj mX(24, 0, 0, 1));
            ASSERT_SAFE_FAIL(Obj mX(24, 0, 1, 0));
            ASSERT_SAFE_FAIL(Obj mX(24, 1, 0, 0));

            ASSERT_SAFE_FAIL(Obj mX(-1, 0, 0, 0));
            ASSERT_SAFE_PASS(Obj mX( 0, 0, 0, 0));
            ASSERT_SAFE_PASS(Obj mX(23, 0, 0, 0));
            ASSERT_SAFE_FAIL(Obj mX(25, 0, 0, 0));

            ASSERT_SAFE_FAIL(Obj mX(0, -1, 0, 0));
            ASSERT_SAFE_PASS(Obj mX(0,  0, 0, 0));
            ASSERT_SAFE_PASS(Obj mX(0, 59, 0, 0));
            ASSERT_SAFE_FAIL(Obj mX(0, 60, 0, 0));

            ASSERT_SAFE_FAIL(Obj mX(0, 0, -1, 0));
            ASSERT_SAFE_PASS(Obj mX(0, 0,  0, 0));
            ASSERT_SAFE_PASS(Obj mX(0, 0, 59, 0));
            ASSERT_SAFE_FAIL(Obj mX(0, 0, 60, 0));

            ASSERT_SAFE_FAIL(Obj mX(0, 0, 0,   -1));
            ASSERT_SAFE_PASS(Obj mX(0, 0, 0,    0));
            ASSERT_SAFE_PASS(Obj mX(0, 0, 0,  999));
            ASSERT_SAFE_FAIL(Obj mX(0, 0, 0, 1000));
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
        const Obj VA(  0,  0,  0,   0);
        const Obj VB(  0,  0,  0, 999);
        const Obj VC(  0,  0, 59,   0);
        const Obj VD(  0, 59,  0,   0);
        const Obj VE( 23,  0,  0,   0);
        const Obj VF( 23, 22, 21, 209);
        const Obj VG( 24,  0,  0,   0);

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

                    LOOP_ASSERT(i, (   (      0 <= T1.hour()
                                        &&   24 >  T1.hour()
                                        &&    0 <= T1.minute()
                                        &&   60 >  T1.minute()
                                        &&    0 <= T1.second()
                                        &&   60 >  T1.second()
                                        &&    0 <= T1.millisecond()
                                        && 1000 >  T1.millisecond())
                                    || (     24 == T1.hour()
                                        &&    0 == T1.minute()
                                        &&    0 == T1.second()
                                        &&    0 == T1.millisecond())));

                    LOOP_ASSERT(i, (   (      0 <= T2.hour()
                                        &&   24 >  T2.hour()
                                        &&    0 <= T2.minute()
                                        &&   60 >  T2.minute()
                                        &&    0 <= T2.second()
                                        &&   60 >  T2.second()
                                        &&    0 <= T2.millisecond()
                                        && 1000 >  T2.millisecond())
                                    || (     24 == T2.hour()
                                        &&    0 == T2.minute()
                                        &&    0 == T2.second()
                                        &&    0 == T2.millisecond())));

                    LOOP_ASSERT(i, (   (      0 <= T3.hour()
                                        &&   24 >  T3.hour()
                                        &&    0 <= T3.minute()
                                        &&   60 >  T3.minute()
                                        &&    0 <= T3.second()
                                        &&   60 >  T3.second()
                                        &&    0 <= T3.millisecond()
                                        && 1000 >  T3.millisecond())
                                    || (     24 == T3.hour()
                                        &&    0 == T3.minute()
                                        &&    0 == T3.second()
                                        &&    0 == T3.millisecond())));

                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        if (verbose) {
            cout << "\tOn corrupted data." << endl;
        }

        const Obj W;                   // default value
        const Obj X(0, 1, 0, 0);       // original (control)
        const Obj Y(0, 0, 0, 1);       // new (streamed-out)

        // Verify the three objects are distinct.
        ASSERT(W != X);
        ASSERT(W != Y);
        ASSERT(X != Y);

        const int SERIAL_Y = 1;       // internal rep. of 'Y'

        if (verbose) {
            cout << "\t\tGood stream (for control)." << endl;
        }
        {
            Out out(VERSION_SELECTOR, &allocator);
            out.putInt32(SERIAL_Y);  // Stream out "new" value.
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
            out.putInt32(SERIAL_Y);  // Stream out "new" value.

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
            out.putInt32(SERIAL_Y);  // Stream out "new" value.

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
            out.putInt32(-1);  // Stream out "new" value.

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
            out.putInt32(24*60*60*1000 + 1);  // Stream out "new" value.

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
                int         d_lineNum;      // source line number
                int         d_hour;         // specification hour
                int         d_minute;       // specification minute
                int         d_second;       // specification second
                int         d_millisecond;  // specification millisecond
                int         d_version;      // version to stream with
                int         d_length;       // expect output length
                const char *d_fmt_p;        // expected output format
            } DATA[] = {
                //LINE  HOUR  MIN  SEC    MS   VER  LEN  FORMAT
                //----  ----  ---  ---  -----  ---  ---  -------------------
                { L_,     14,  10,   2,   117,  1,   4,  "\x03\x0a\x3b\x05"  },
                { L_,     20,   8,  27,   983,  1,   4,  "\x04\x52\x62\x4f"  }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_lineNum;
                const int         HOUR        = DATA[i].d_hour;
                const int         MINUTE      = DATA[i].d_minute;
                const int         SECOND      = DATA[i].d_second;
                const int         MILLISECOND = DATA[i].d_millisecond;
                const int         VERSION     = DATA[i].d_version;
                const int         LEN         = DATA[i].d_length;
                const char *const FMT         = DATA[i].d_fmt_p;

                // Test using class methods.

                {
                    Obj        mX(HOUR, MINUTE, SECOND, MILLISECOND);
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
                    Obj        mX(HOUR, MINUTE, SECOND, MILLISECOND);
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
        //   Time& operator=(const Time& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ASSIGNMENT OPERATOR" << endl
                          << "===========================" << endl;

        if (verbose) cout << "\nTesting Assignment u = V." << endl;
        {
            static const struct {
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },  {  0,  0,  0, 999  },
                {  0,  0, 59,   0  },  {  0, 59,  0,   0  },
                { 23,  0,  0,   0  },  { 23, 22, 21, 209  },
                { 23, 22, 21, 210  },  { 24,  0,  0,   0  },
            };

            const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                    / sizeof *VALUES);

            int i;
            for (i = 0; i < NUM_VALUES; ++i) {
                Obj v;  const Obj& V = v;
                v.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);
                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj u;  const Obj& U = u;
                    u.setTime(VALUES[j].d_hour,    VALUES[j].d_minute,
                              VALUES[j].d_second,  VALUES[j].d_msec);
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

            for (i = 0; i < NUM_VALUES; ++i) {
                Obj u;  const Obj& U = u;
                u.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);
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

        if (verbose) cout << "Not implemented for 'bdlt::Time'." << endl;

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
        // Testing:
        //   Time(const Time& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING COPY CONSTRUCTOR" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nTesting copy constructor." << endl;
        {
            static const struct {
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },  {  0,  0,  0, 999  },
                {  0,  0, 59,   0  },  {  0, 59,  0,   0  },
                { 23,  0,  0,   0  },  { 23, 22, 21, 209  },
                { 23, 22, 21, 210  },  { 24,  0,  0,   0  },
            };

            const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                    / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj x;  const Obj& X = x;
                x.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);
                Obj w;  const Obj& W = w;
                w.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);

                Obj y(X);  const Obj &Y = y;
                if (veryVerbose) { T_;  P_(W);  P_(X);  P(Y); }
                LOOP_ASSERT(i, X == W);
                LOOP_ASSERT(i, Y == W);
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //   The equality and inequality operators work as expected.
        //
        // Concerns:
        //: 1 Any subtle variation in value must be detected by the equality
        //:   operators.  The test data have variations in each input
        //:   parameter, even though tested methods convert the input before
        //:   the underlying equality operator on a single pair of 'int's is
        //:   invoked.
        //:
        //: 2 The default value (24:00:00.000) is handled by the methods
        //:   correctly.
        //
        // Plan:
        //: 1 Specify a set S of unique object values having various minor or
        //:   subtle differences, but also including the default value.  Verify
        //:   the correctness of 'operator==' and 'operator!=' using all
        //:   elements (u, v) of the cross product S X S.  (C-1,2)
        //
        // Testing:
        //   bool operator==(const Time& lhs, const Time& rhs);
        //   bool operator!=(const Time& lhs, const Time& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING EQUALITY OPERATORS" << endl
                          << "==========================" << endl;

        if (verbose) {
            cout << "\nCompare each pair of values (u, v) in S X S." << endl;
        }
        {
            static const struct {
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },
                {  0,  0,  0,   1  },  {  0,  0,  1,   0  },
                {  0,  1,  0,   0  },  {  1,  0,  0,   0  },

                { 23, 22, 21, 209  },
                { 23, 22, 21, 208  },  { 23, 22, 20, 209  },
                { 23, 21, 21, 209  },  { 22, 22, 21, 209  },

                { 24,  0,  0,   0  },  // 24:00:00.000 explicitly included
            };

            const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                    / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj u;  const Obj& U = u;
                u.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);
                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj v;  const Obj& V = v;
                    v.setTime(VALUES[j].d_hour,   VALUES[j].d_minute,
                              VALUES[j].d_second, VALUES[j].d_msec);
                    bool isSame = i == j;
                    if (veryVerbose) { T_;  P_(i);  P_(j);  P_(U);  P(V); }
                    LOOP2_ASSERT(i, j,  isSame == (U == V));
                    LOOP2_ASSERT(i, j, !isSame == (U != V));
                    LOOP2_ASSERT(i, j,  isSame == (V == U));
                    LOOP2_ASSERT(i, j, !isSame == (V != U));
                }
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING PRINT
        //   Verify the methods produce the expected output format.
        //
        // Concerns:
        //: 1 Methods produce expected output format.
        //
        // Plan:
        //: 1 For a small set of objects, use 'ostringstream' to write the
        //:   object's value to a string buffer and then compare to expected
        //:   output format.  (C-1)
        //
        // Testing:
        //   ostream& print(ostream& os, int level = 0, int spl = 4) const;
        //   ostream& operator<<(ostream& stream, const Time& time);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PRINT" << endl
                          << "=============" << endl;

        if (verbose) cout << "\nTesting 'print'." << endl;
        {
#define NL "\n"
            static const struct {
                int         d_lineNum;  // source line number
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //LINE  INDENT +/-  SPACES  FORMAT                // ADJUST
                //----  ----------  ------  --------------
                { L_,    0,         -1,     "24:00:00.000"                   },

                { L_,    0,          0,     "24:00:00.000"              NL   },

                { L_,    0,          2,     "24:00:00.000"              NL   },

                { L_,    1,          1,     " 24:00:00.000"             NL   },

                { L_,    1,          2,     "  24:00:00.000"            NL   },

                { L_,   -1,          2,     "24:00:00.000"              NL   },

                { L_,   -2,          1,     "24:00:00.000"              NL   },

                { L_,    2,          1,     "  24:00:00.000"            NL   },

                { L_,    1,          3,     "   24:00:00.000"           NL   },
            };
#undef NL
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            const int SIZE = 128;  // Must be big enough to hold output string.

            const char Z1 = static_cast<char>(0xFF);  // Value 1 used for an
                                                      // unset 'char'.

            const char Z2 = static_cast<char>(0x00);  // Value 2 used for an
                                                      // unset 'char'.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);

            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            Obj mX;  const Obj& X = mX;  // 24:00:00.000

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
//              const int         LINE = DATA[ti].d_lineNum; // unused
                const int         IND  = DATA[ti].d_indent;
                const int         SPL  = DATA[ti].d_spaces;
                const char *const FMT  = DATA[ti].d_fmt_p;

                if (veryVerbose) {
                    cout << "EXPECTED FORMAT:" << endl << FMT << endl;
                }
                ostringstream out1(bsl::string(CTRL_BUF1, SIZE));
                X.print(out1, IND, SPL) << ends;
                ostringstream out2(bsl::string(CTRL_BUF2, SIZE));
                X.print(out2, IND, SPL) << ends;
                if (veryVerbose) {
                    cout << "ACTUAL FORMAT:" << endl << out1.str() << endl;
                }

                const int SZ = static_cast<int>(strlen(FMT)) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(ti, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(ti,
                            Z1 == out1.str()[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti,
                            Z2 == out2.str()[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, 0 == strcmp(out1.str().c_str(), FMT));
                LOOP_ASSERT(ti, 0 == strcmp(out2.str().c_str(), FMT));
                LOOP_ASSERT(ti, 0 == memcmp(out1.str().c_str() + SZ,
                                            CTRL_BUF1 + SZ,
                                            REST));
                LOOP_ASSERT(ti, 0 == memcmp(out2.str().c_str() + SZ,
                                            CTRL_BUF2 + SZ,
                                            REST));
            }
        }

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                int         d_hour;     // hour field value
                int         d_minute;   // minute field value
                int         d_second;   // second field value
                int         d_msec;     // millisecond field value
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //LINE  HOUR   MIN    SEC    MSEC     OUTPUT FORMAT
                //----  -----  ----   ----   -----    --------------
                { L_,      0,    0,     0,      0,    "00:00:00.000"     },
                { L_,      0,    0,     0,    999,    "00:00:00.999"     },
                { L_,      0,    0,    59,      0,    "00:00:59.000"     },
                { L_,      0,   59,     0,      0,    "00:59:00.000"     },
                { L_,     23,    0,     0,      0,    "23:00:00.000"     },
                { L_,     23,   22,    21,    209,    "23:22:21.209"     },
                { L_,     23,   22,    21,    210,    "23:22:21.210"     },
                { L_,     24,    0,     0,      0,    "24:00:00.000"     },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            const int SIZE = 1000;     // Must be able to hold output string.

            const char XX = static_cast<char>(0xFF);  // Value used for an
                                                      // unset 'char'.

            char        mCtrlBuf[SIZE];  memset(mCtrlBuf, XX, SIZE);
            const char *CTRL_BUF = mCtrlBuf; // Used for extra character check.

            for (int di = 0; di < NUM_DATA;  ++di) {
                const int         LINE   = DATA[di].d_lineNum;
                const int         HOUR   = DATA[di].d_hour;
                const int         MINUTE = DATA[di].d_minute;
                const int         SECOND = DATA[di].d_second;
                const int         MSEC   = DATA[di].d_msec;
                const char *const FMT    = DATA[di].d_fmt_p;

                Obj x;  const Obj& X = x;
                x.setTime(HOUR, MINUTE, SECOND, MSEC);

                if (veryVerbose) cout << "\tEXPECTED FORMAT: " << FMT << endl;
                ostringstream out(std::string(CTRL_BUF, SIZE));
                out << X << ends;
                if (veryVerbose) {
                    cout << "\tACTUAL FORMAT:   " << out.str() << endl;
                }

                const int SZ = static_cast<int>(strlen(FMT)) + 1;
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE,
                            XX == out.str()[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, 0 == memcmp(out.str().c_str(), FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(out.str().c_str() + SZ,
                                              CTRL_BUF + SZ,
                                              SIZE - SZ));
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   Verify the basic accessors work as expected.
        //
        // Concerns:
        //: 1 Each accessor performs the appropriate arithmetic to convert
        //:   the internal total-milliseconds representation to the
        //:   four-parameter (h, m, s, ms) representation.
        //:
        //: 2 The 'getTime' accessor must respond appropriately to null-pointer
        //:   arguments, specifically not affecting other (non-null) arguments
        //:   to be loaded with appropriate values.
        //
        // Plan:
        //: 1 For each of a sequence of unique object values, verify that each
        //:   of the basic accessors returns the correct value.  (C-1)
        //:
        //: 2 For each of a sequence of unique object values, verify that the
        //:   'getTime' method with various null arguments produces the
        //:   expected behavior.  (C-2)
        //
        // Testing:
        //   void getTime(int *hour, int *min, int *second, int *msec) const;
        //   int hour() const;
        //   int minute() const;
        //   int second() const;
        //   int millisecond() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING BASIC ACCESSORS" << endl
                          << "=======================" << endl;

        if (verbose) {
            cout << "\nTesting 'getTime', 'hour()', 'minute()', 'second()', "
                 << "and 'millisecond()'." << endl;
        }
        {
            static const struct {
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },  {  0,  0,  0, 999  },
                {  0,  0, 59,   0  },  {  0, 59,  0,   0  },
                { 23,  0,  0,   0  },  { 23, 22, 21, 209  },
                { 23, 22, 21, 210  },  { 24,  0,  0,   0  },
            };

            const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                    / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int HOUR   = VALUES[i].d_hour;
                const int MINUTE = VALUES[i].d_minute;
                const int SECOND = VALUES[i].d_second;
                const int MSEC   = VALUES[i].d_msec;
                int       h, m, s, ms;

                Obj x;  const Obj& X = x;
                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                X.getTime(&h, &m, &s, &ms);

                if (veryVerbose) {
                    T_;  P_(HOUR);    P_(h);  P_(MINUTE);  P_(m);
                         P_(SECOND);  P_(s);  P_(MSEC);    P(ms);
                    T_;  P(X);
                }

                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, SECOND == X.second());
                LOOP_ASSERT(i, MSEC   == X.millisecond());

                LOOP_ASSERT(i, HOUR   == h);
                LOOP_ASSERT(i, MINUTE == m);
                LOOP_ASSERT(i, SECOND == s);
                LOOP_ASSERT(i, MSEC   == ms);
            }
        }

        if (verbose) cout << "\nTesting 'getTime' with null args." << endl;
        {
            static const struct {
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },  {  0,  0,  0, 999  },
                {  0,  0, 59,   0  },  {  0, 59,  0,   0  },
                { 23,  0,  0,   0  },  { 23, 22, 21, 209  },
                { 23, 22, 21, 210  },  { 24,  0,  0,   0  },
            };

            const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                    / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int HOUR   = VALUES[i].d_hour;
                const int MINUTE = VALUES[i].d_minute;
                const int SECOND = VALUES[i].d_second;
                const int MSEC   = VALUES[i].d_msec;

                int h, m, s, ms;

                Obj x;  const Obj& X = x;

                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                X.getTime(&h, 0, 0, 0);
                if (veryVerbose) { T_;  P_(HOUR);  P_(h);  P(X); }
                LOOP_ASSERT(i, HOUR   == h);

                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                X.getTime(&h, 0, 0);
                if (veryVerbose) { T_;  P_(HOUR);  P_(h);  P(X); }
                LOOP_ASSERT(i, HOUR   == h);

                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                X.getTime(0, &m, 0, 0);
                if (veryVerbose) { T_;  P_(MINUTE);  P_(m);  P(X); }
                LOOP_ASSERT(i, MINUTE == m);

                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                X.getTime(0, &m, 0);
                if (veryVerbose) { T_;  P_(MINUTE);  P_(m);  P(X); }
                LOOP_ASSERT(i, MINUTE == m);

                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                X.getTime(0, 0, &s, 0);
                if (veryVerbose) { T_;  P_(SECOND);  P_(s);  P(X); }
                LOOP_ASSERT(i, SECOND == s);

                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                X.getTime(0, 0, &s);
                if (veryVerbose) { T_;  P_(SECOND);  P_(s);  P(X); }
                LOOP_ASSERT(i, SECOND == s);

                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                X.getTime(0, 0, 0, &ms);
                if (veryVerbose) { T_;  P_(MSEC);  P_(ms);  P(X); }
                LOOP_ASSERT(i, MSEC   == ms);

                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                X.getTime(0, 0, 0);
                if (veryVerbose) { T_;  P_(0);  P(X);  cout << endl; }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'gg'
        //   Void for 'bdlt_time'.
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
        //:   factors to convert the four-parameter input representation to the
        //:   internal total-milliseconds representation.
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
        //   Time();
        //   ~Time();
        //   void setTime(int hour, int min = 0, int sec = 0, int msec = 0);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING PRIMARY MANIPULATORS (BOOTSTRAP)" << endl
                 << "========================================" << endl;
        }

        if (verbose) cout << "\nTesting default constructor." << endl;

        Obj x;  const Obj& X = x;

        ASSERT(24 == X.hour());          ASSERT(0 == X.minute());
        ASSERT( 0 == X.second());        ASSERT(0 == X.millisecond());

        if (verbose) cout << "\nTesting 'setTime'." << endl;
        {
            static const struct {
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },  {  0,  0,  0, 999  },
                {  0,  0, 59,   0  },  {  0, 59,  0,   0  },
                { 23,  0,  0,   0  },  { 23, 22, 21, 209  },
                { 23, 22, 21, 210  },  { 24,  0,  0,   0  },
            };

            const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                    / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int HOUR   = VALUES[i].d_hour;
                const int MINUTE = VALUES[i].d_minute;
                const int SECOND = VALUES[i].d_second;
                const int MSEC   = VALUES[i].d_msec;

                Obj x;  const Obj& X = x;
                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                if (veryVerbose) {
                    T_; P_(HOUR); P_(MINUTE); P_(SECOND); P_(MSEC); P(X);
                }
                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, SECOND == X.second());
                LOOP_ASSERT(i, MSEC   == X.millisecond());

                x.setTime(HOUR, MINUTE, SECOND);
                if (veryVerbose) {
                    T_; P_(HOUR); P_(MINUTE); P_(SECOND); P(X);
                }
                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, SECOND == X.second());
                LOOP_ASSERT(i, 0      == X.millisecond());

                x.setTime(HOUR, MINUTE);
                if (veryVerbose) { T_;  P_(HOUR);  P_(MINUTE);  P(X); }
                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, 0      == X.second());
                LOOP_ASSERT(i, 0      == X.millisecond());

                x.setTime(HOUR);
                if (veryVerbose) { T_;  P_(HOUR);  P(X); }
                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, 0      == X.minute());
                LOOP_ASSERT(i, 0      == X.second());
                LOOP_ASSERT(i, 0      == X.millisecond());
            }
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj mX;

            ASSERT_SAFE_PASS(mX.setTime(24, 0, 0, 0));
            ASSERT_SAFE_FAIL(mX.setTime(24, 0, 0, 1));
            ASSERT_SAFE_FAIL(mX.setTime(24, 0, 1, 0));
            ASSERT_SAFE_FAIL(mX.setTime(24, 1, 0, 0));

            ASSERT_SAFE_FAIL(mX.setTime(-1, 0, 0, 0));
            ASSERT_SAFE_PASS(mX.setTime( 0, 0, 0, 0));
            ASSERT_SAFE_PASS(mX.setTime(23, 0, 0, 0));
            ASSERT_SAFE_FAIL(mX.setTime(25, 0, 0, 0));

            ASSERT_SAFE_FAIL(mX.setTime(0, -1, 0, 0));
            ASSERT_SAFE_PASS(mX.setTime(0,  0, 0, 0));
            ASSERT_SAFE_PASS(mX.setTime(0, 59, 0, 0));
            ASSERT_SAFE_FAIL(mX.setTime(0, 60, 0, 0));

            ASSERT_SAFE_FAIL(mX.setTime(0, 0, -1, 0));
            ASSERT_SAFE_PASS(mX.setTime(0, 0,  0, 0));
            ASSERT_SAFE_PASS(mX.setTime(0, 0, 59, 0));
            ASSERT_SAFE_FAIL(mX.setTime(0, 0, 60, 0));

            ASSERT_SAFE_FAIL(mX.setTime(0, 0, 0,   -1));
            ASSERT_SAFE_PASS(mX.setTime(0, 0, 0,    0));
            ASSERT_SAFE_PASS(mX.setTime(0, 0, 0,  999));
            ASSERT_SAFE_FAIL(mX.setTime(0, 0, 0, 1000));
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

        const int HA = 1, MA = 2, SA = 3, mA = 4;  // h, m, s, ms values for VA
        const int HB = 5, MB = 6, SB = 7, mB = 8;  // h, m, s, ms values for VB
        const int HC = 9, MC = 9, SC = 9, mC = 9;  // h, m, s, ms values for VC

        int h, m, s, ms;                   // reusable variables for 'get' call

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(HA, MA, SA, mA);  const Obj& X1 = mX1;
        if (verbose) { T_;  P(X1); }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        h = 0;  m = 0;  s = 0;  ms = 0;
        X1.getTime(&h, &m, &s, &ms);
        ASSERT(HA == h);  ASSERT(MA == m);  ASSERT(SA == s);  ASSERT(mA == ms);
        ASSERT(HA == X1.hour());            ASSERT(MA == X1.minute());
        ASSERT(SA == X1.second());          ASSERT(mA == X1.millisecond());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { T_;  P(X2); }

        if (verbose) cout << "\ta. Check the initial state of x2." << endl;
        h = 0;  m = 0;  s = 0;  ms = 0;
        X2.getTime(&h, &m, &s, &ms);
        ASSERT(HA == h);  ASSERT(MA == m);  ASSERT(SA == s);  ASSERT(mA == ms);
        ASSERT(HA == X2.hour());            ASSERT(MA == X2.minute());
        ASSERT(SA == X2.second());          ASSERT(mA == X2.millisecond());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;
        mX1.setTime(HB, MB, SB, mB);
        if (verbose) { T_;  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        h = 0;  m = 0;  s = 0;  ms = 0;
        X1.getTime(&h, &m, &s, &ms);
        ASSERT(HB == h);  ASSERT(MB == m);  ASSERT(SB == s);  ASSERT(mB == ms);
        ASSERT(HB == X1.hour());            ASSERT(MB == X1.minute());
        ASSERT(SB == X1.second());          ASSERT(mB == X1.millisecond());

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
        h = 0;  m = 0;  s = 0;  ms = 0;
        X3.getTime(&h, &m, &s, &ms);
        ASSERT(24 == h);  ASSERT(0 == m);  ASSERT(0 == s);  ASSERT(0 == ms);

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
        h = 0;  m = 0;  s = 0;  ms = 0;
        X4.getTime(&h, &m, &s, &ms);
        ASSERT(24 == h);  ASSERT(0 == m);  ASSERT(0 == s);  ASSERT(0 == ms);

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;
        mX3.setTime(HC, MC, SC, mC);
        if (verbose) { T_;  P(X3); }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        h = 0;  m = 0;  s = 0;  ms = 0;
        X3.getTime(&h, &m, &s, &ms);
        ASSERT(HC == h);  ASSERT(MC == m);  ASSERT(SC == s);  ASSERT(mC == ms);
        ASSERT(HC == X3.hour());            ASSERT(MC == X3.minute());
        ASSERT(SC == X3.second());          ASSERT(mC == X3.millisecond());

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
        h = 0;  m = 0;  s = 0;  ms = 0;
        X2.getTime(&h, &m, &s, &ms);
        ASSERT(HB == h);  ASSERT(MB == m);  ASSERT(SB == s);  ASSERT(mB == ms);
        ASSERT(HB == X2.hour());            ASSERT(MB == X2.minute());
        ASSERT(SB == X2.second());          ASSERT(mB == X2.millisecond());

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
        h = 0;  m = 0;  s = 0;  ms = 0;
        X2.getTime(&h, &m, &s, &ms);
        ASSERT(HC == h);  ASSERT(MC == m);  ASSERT(SC == s);  ASSERT(mC == ms);
        ASSERT(HC == X2.hour());            ASSERT(MC == X2.minute());
        ASSERT(SC == X2.second());          ASSERT(mC == X2.millisecond());

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
        h = 0;  m = 0;  s = 0;  ms = 0;
        X1.getTime(&h, &m, &s, &ms);
        ASSERT(HB == h);  ASSERT(MB == m);  ASSERT(SB == s);  ASSERT(mB == ms);
        ASSERT(HB == X1.hour());            ASSERT(MB == X1.minute());
        ASSERT(SB == X1.second());          ASSERT(mB == X1.millisecond());

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
