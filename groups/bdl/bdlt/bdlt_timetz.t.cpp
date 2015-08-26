// bdlt_timetz.t.cpp                                                  -*-C++-*-
#include <bdlt_timetz.h>

#include <bdlt_timeunitratio.h>

#include <bslim_testutil.h>
#include <bslma_default.h>
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
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test implements a single value-semantic class,
// 'bdlt::TimeTz', that represents a time value with a local time offset.  We
// will therefore follow our standard 10-case approach to testing
// value-semantic types.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [12] static bool isValid(const Time& localTime, int offset);
// [10] static int maxSupportedBdexVersion(int versionSelector);
//
// CREATORS
// [11] TimeTz();
// [ 2] TimeTz(const Time& localTime, int offset);
// [ 7] TimeTz(const TimeTz& original);
// [ 2] ~TimeTz();
//
// MANIPULATORS
// [ 9] TimeTz& operator=(const TimeTz& rhs);
// [13] void setTimeTz(const Time& localTime, int offset);
// [14] int setTimeTzIfValid(const Time& localTime, int offset);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
//
// ACCESSORS
// [ 4] Time localTime() const;
// [ 4] int offset() const;
// [15] Time utcTime() const;
// [ 5] bsl::ostream& print(bsl::ostream& stream, int l, int spl) const;
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const TimeTz& lhs, const TimeTz& rhs);
// [ 6] bool operator!=(const TimeTz& lhs, const TimeTz& rhs);
// [ 5] bsl::ostream& operator<<(bsl::ostream& stream, const TimeTz& rhs);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [18] USAGE EXAMPLE
// [ *] CONCERN: no use of global or default allocators
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
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlt::TimeTz        Obj;
typedef bslx::TestInStream  In;
typedef bslx::TestOutStream Out;

#define VERSION_SELECTOR 20140601

//=============================================================================
//                               MAIN PROGRAM
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

    // CONCERN: no use of global or default allocators

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    switch (test) { case 0:
      case 18: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
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

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Comparing Times from Multiple Time Zones
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// Some legacy systems may represent points in time as a combination of a local
// time-of-day plus an offset from UTC, with an underlying assumption that the
// dates on which points in time occur can be inferred from context.  Assuming
// that we know that two such times fall on the same (local) calendar date, we
// can determine whether or not the two times coincide by comparing their
// 'bdlt::TimeTz' representations.
//
// First, we define three 'bdlt::TimeTz' objects representing the time in three
// different time zones on the same (local) date:
//..
    bdlt::TimeTz newYorkTime(bdlt::Time(9, 30, 0, 0.0),
                             -5 * bdlt::TimeUnitRatio::k_MINUTES_PER_HOUR);
    bdlt::TimeTz chicagoTime(bdlt::Time(8, 30, 0, 0.0),
                             -6 * bdlt::TimeUnitRatio::k_MINUTES_PER_HOUR);
    bdlt::TimeTz phoenixTime(bdlt::Time(6, 30, 0, 0.0),
                             -7 * bdlt::TimeUnitRatio::k_MINUTES_PER_HOUR);
//..
// Then, we observe that the local times are distinct:
//..
    ASSERT(newYorkTime.localTime() != chicagoTime.localTime());
    ASSERT(chicagoTime.localTime() != phoenixTime.localTime());
    ASSERT(phoenixTime.localTime() != newYorkTime.localTime());
//..
// Next, we observe that 'newYorkTime' and 'chicagoTime' actually represent the
// same point in time:
//..
    ASSERT(newYorkTime.utcTime() == chicagoTime.utcTime());
//..
// Finally, we observe that 'phoenixTime' is one hour earlier than
// 'newYorkTime':
//..
    bdlt::DatetimeInterval delta =
                                 newYorkTime.utcTime() - phoenixTime.utcTime();

    ASSERT(0 == delta.days());
    ASSERT(1 == delta.hours());
    ASSERT(0 == delta.minutes());
    ASSERT(0 == delta.seconds());
    ASSERT(0 == delta.milliseconds());
//..
      } break;
      case 17: {
        // Deprecated test case.  Do not remove.
      } break;
      case 16: {
        // Deprecated test case.  Do not remove.
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // ACCESSOR 'utcTime'
        //
        // Concerns:
        //: 1 'utcTime' returns the local time value supplied to the
        //:   constructor, offset by 'offset' minutes.
        //:
        //: 2 'utcTime' returns the correct value when 'offset' is positive,
        //:   negative, or zero.
        //:
        //: 3 'utcTime' is 'const'.
        //
        // Plan:
        //: 1 Using the table-driven approach, construct objects with a set of
        //:   distinct values, and verify that 'utcTime' returns the expected
        //:   results.  (C-1..2)
        //:
        //: 2 Invoke 'utcTime' from a reference providing non-modifiable access
        //:   to the object.  (C-3)
        //
        // Testing:
        //   Time utcTime() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ACCESSOR 'utcTime'" << endl
                          << "==================" << endl;

        if (verbose) cout <<
                            "\nUse a table of distinct object values." << endl;
        {
            struct {
                int d_line;            // line number
                int d_hour;            // 'Time' hour attribute
                int d_minute;          // 'Time' minute attribute
                int d_second;          // 'Time' second attribute
                int d_millisecond;     // 'Time' millisecond attribute
                int d_offset;          // offset
                int d_expHour;         // expected 'Time' hour attribute
                int d_expMinute;       // expected 'Time' minute attribute
                int d_expSecond;       // expected 'Time' second attribute
                int d_expMillisecond;  // expected 'Time' millisecond attribute
            } DATA[] = {
                //   v---- INPUT -----v            v--- EXPECTED --v
                //LN HR    MN   SC   MS   OFFSET   HR   MN   SC   MS
                //-- ---  ---  ---  ----  ------  ---  ---  ---  ----
                {L_,  24,   0,   0,    0,      0,  24,   0,   0,    0 },

                {L_,   0,   0,   0,    0,      0,   0,   0,   0,    0 },
                {L_,  12,   0,   0,    0,      0,  12,   0,   0,    0 },
                {L_,  23,  59,  59,  999,      0,  23,  59,  59,  999 },

                {L_,   0,   0,   0,    0,      1,  23,  59,   0,    0 },
                {L_,  12,   0,   0,    0,      1,  11,  59,   0,    0 },
                {L_,  23,  59,  59,  999,      1,  23,  58,  59,  999 },

                {L_,   0,   0,   0,    0,     -1,   0,   1,   0,    0 },
                {L_,  12,   0,   0,    0,     -1,  12,   1,   0,    0 },
                {L_,  23,  59,  59,  999,     -1,   0,   0,  59,  999 },

                {L_,   0,   0,   0,    0,   1439,   0,   1,   0,    0 },
                {L_,  12,   0,   0,    0,   1439,  12,   1,   0,    0 },
                {L_,  23,  59,  59,  999,   1439,   0,   0,  59,  999 },

                {L_,   0,   0,   0,    0,  -1439,  23,  59,   0,    0 },
                {L_,  12,   0,   0,    0,  -1439,  11,  59,   0,    0 },
                {L_,  23,  59,  59,  999,  -1439,  23,  58,  59,  999 },

                {L_,  12,   0,   0,    0,    720,   0,   0,   0,    0 },
                {L_,  12,   0,   0,    0,    721,  23,  59,   0,    0 },
                {L_,  12,   0,   0,    0,    719,   0,   1,   0,    0 },
            };
            int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE            = DATA[ti].d_line;
                const int HOUR            = DATA[ti].d_hour;
                const int MINUTE          = DATA[ti].d_minute;
                const int SECOND          = DATA[ti].d_second;
                const int MILLISECOND     = DATA[ti].d_millisecond;
                const int OFFSET          = DATA[ti].d_offset;
                const int EXP_HOUR        = DATA[ti].d_expHour;
                const int EXP_MINUTE      = DATA[ti].d_expMinute;
                const int EXP_SECOND      = DATA[ti].d_expSecond;
                const int EXP_MILLISECOND = DATA[ti].d_expMillisecond;

                if (veryVerbose) {
                    T_
                    P_(LINE)
                    P_(HOUR)
                    P_(MINUTE)
                    P_(SECOND)
                    P_(MILLISECOND)
                    P (OFFSET)
                }

                // Prepare time values.

                const bdlt::Time TIME(
                                HOUR,     MINUTE,     SECOND,     MILLISECOND);
                const bdlt::Time EXPECTED(
                            EXP_HOUR, EXP_MINUTE, EXP_SECOND, EXP_MILLISECOND);

                // Construct an object.

                Obj mX(TIME, OFFSET); const Obj& X = mX;

                // Inspect attributes.

                ASSERTV(LINE, TIME, OFFSET, EXPECTED, EXPECTED == X.utcTime());
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // MANIPULATOR 'setTimeTzIfValid'
        //
        // Concerns:
        //: 1 'setTimeTzIfValid' sets the date and offset of a 'TimeTz'
        //:   object to the specified values only if they are valid.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of valid and
        //:   invalid object values.  For each value verify that
        //:   'setTimeTzIfValid' behaves correctly.  (C-1..2)
        //
        // Testing:
        //   int setTimeTzIfValid(const Time& localTime, int offset);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MANIPULATOR 'setTimeTzIfValid'" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\nUse a table of distinct inputs." << endl;
        {
            enum { MAX_TIMEZONE = 24 * 60 - 1,
                   MTZ = MAX_TIMEZONE };

            struct {
                int d_line;
                int d_hour;
                int d_minute;
                int d_second;
                int d_millisecond;
                int d_offset;
                int d_isValid;
            } DATA[] = {
                //LN   HOUR  MINUTE  SECOND  MILLISECOND    OFFSET  VALID
                //--   ----  ------  ------  -----------    ------  -----
                { L_,     2,      0,      0,           0,    -4*60,     1 },
                { L_,     0,      2,      0,           0,    -4*60,     1 },
                { L_,     0,      0,      2,           0,    -4*60,     1 },
                { L_,     0,      0,      0,           0,    -4*60,     1 },
                { L_,     2,      0,      0,           0,     -MTZ,     1 },
                { L_,     2,      0,      0,           0,      MTZ,     1 },
                { L_,    24,      0,      0,           0,        0,     1 },
                { L_,    24,      0,      0,           0,        1,     0 },
                { L_,    24,      0,      0,           0,     4*60,     0 },
                { L_,    24,      0,      0,           0,    -4*60,     0 },
                { L_,     2,      0,      0,           0,    24*60,     0 },
                { L_,     2,      0,      0,           0,   -24*60,     0 },
                { L_,     2,      0,      0,           0,    99*60,     0 },
                { L_,     2,      0,      0,           0,   -99*60,     0 },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            // default initial value

            for (int di = 0; di < NUM_DATA; ++di) {
                const int LINE        = DATA[di].d_line;
                const int HOUR        = DATA[di].d_hour;
                const int MINUTE      = DATA[di].d_minute;
                const int SECOND      = DATA[di].d_second;
                const int MILLISECOND = DATA[di].d_millisecond;
                const int OFFSET      = DATA[di].d_offset;
                const int IS_VALID    = DATA[di].d_isValid;

                if (veryVerbose) {
                    T_
                    P_(LINE)
                    P_(HOUR)
                    P_(MINUTE)
                    P_(SECOND)
                    P_(MILLISECOND)
                    P_(OFFSET)
                    P (IS_VALID)
                }

                const bdlt::Time TIME(HOUR, MINUTE, SECOND, MILLISECOND);

                bdlt::TimeTz timeTz;
                ASSERTV(LINE,
                       IS_VALID == !timeTz.setTimeTzIfValid(TIME, OFFSET));

                if (!IS_VALID) {
                    const bdlt::TimeTz NULL_TIME_TZ;
                    ASSERTV(LINE, NULL_TIME_TZ == timeTz);
                }
                else {
                    const bdlt::Time RESULT_TIME = timeTz.localTime();

                    ASSERTV(LINE, TIME, RESULT_TIME, TIME == RESULT_TIME);

                    ASSERTV(LINE, OFFSET == timeTz.offset());
                }
            }

            // non-default initial value

            for (int di = 0; di < NUM_DATA; ++di) {
                const int LINE        = DATA[di].d_line;
                const int HOUR        = DATA[di].d_hour;
                const int MINUTE      = DATA[di].d_minute;
                const int SECOND      = DATA[di].d_second;
                const int MILLISECOND = DATA[di].d_millisecond;
                const int OFFSET      = DATA[di].d_offset;
                const int IS_VALID    = DATA[di].d_isValid;

                if (veryVerbose) {
                    T_
                    P_(LINE)
                    P_(HOUR)
                    P_(MINUTE)
                    P_(SECOND)
                    P_(MILLISECOND)
                    P_(OFFSET)
                    P (IS_VALID)
                }

                const bdlt::Time TIME(HOUR, MINUTE, SECOND, MILLISECOND);

                const bdlt::Time INITIAL_TIME(1, 1, 1, 1);
                const int INITIAL_OFFSET = -12;

                bdlt::TimeTz timeTz(INITIAL_TIME, INITIAL_OFFSET);

                ASSERTV(LINE,
                       IS_VALID == !timeTz.setTimeTzIfValid(TIME, OFFSET));

                if (!IS_VALID) {
                    const bdlt::TimeTz INIT_TIME_TZ(INITIAL_TIME,
                                                    INITIAL_OFFSET);

                    ASSERTV(LINE, INIT_TIME_TZ == timeTz);
                }
                else {
                    const bdlt::Time RESULT_TIME = timeTz.localTime();

                    ASSERTV(LINE, TIME, RESULT_TIME, TIME == RESULT_TIME);

                    ASSERTV(LINE, OFFSET == timeTz.offset());
                }
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // MANIPULATOR 'setTimeTz'
        //
        // Concerns:
        //: 1 'setTimeTz' sets the local time and offset of the object to the
        //:   specified values.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the table-driven technique, specify a range of distinct time
        //:   and offset values. Pass each value to the 'setTimeTz' method and
        //:   verify that the resulting object value is correct.  (C-2)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-3)
        //
        // Testing:
        //   void setTimeTz(const Time& localTime, int offset);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MANIPULATOR 'setTimeTz'" << endl
                          << "=======================" << endl;

        if (verbose) cout << "\nUse a table of distinct inputs." << endl;
        {
            enum { MAX_TIMEZONE = 24 * 60 - 1,
                   MTZ = MAX_TIMEZONE };

            struct {
                int d_line;
                int d_hour;
                int d_minute;
                int d_second;
                int d_millisecond;
                int d_offset;
            } DATA[] = {
                //LN   HOUR  MINUTE  SECOND  MILLISECOND    OFFSET
                //--   ----  ------  ------  -----------    ------
                { L_,     2,      0,      0,           0,    -4*60 },
                { L_,     0,      2,      0,           0,    -4*60 },
                { L_,     0,      0,      2,           0,    -4*60 },
                { L_,     0,      0,      0,           2,    -4*60 },
                { L_,     0,      0,      0,           0,    -4*60 },
                { L_,     2,      0,      0,           0,     -MTZ },
                { L_,     2,      0,      0,           0,      MTZ },
                { L_,    24,      0,      0,           0,        0 },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            // default initial value

            for (int di = 0; di < NUM_DATA; ++di) {
                const int LINE        = DATA[di].d_line;
                const int HOUR        = DATA[di].d_hour;
                const int MINUTE      = DATA[di].d_minute;
                const int SECOND      = DATA[di].d_second;
                const int MILLISECOND = DATA[di].d_millisecond;
                const int OFFSET      = DATA[di].d_offset;

                if (veryVerbose) {
                    T_ P_(LINE) P_(HOUR) P_(MINUTE) P_(SECOND) P_(MILLISECOND)
                                                                      P(OFFSET)
                }

                const bdlt::Time TIME(HOUR, MINUTE, SECOND, MILLISECOND);

                bdlt::TimeTz timeTz;
                timeTz.setTimeTz(TIME, OFFSET);

                const bdlt::Time RESULT_TIME = timeTz.localTime();

                ASSERTV(LINE, TIME, RESULT_TIME, TIME == RESULT_TIME);

                ASSERTV(LINE, OFFSET == timeTz.offset());
            }

            // non-default initial value

            for (int di = 0; di < NUM_DATA; ++di) {
                const int LINE        = DATA[di].d_line;
                const int HOUR        = DATA[di].d_hour;
                const int MINUTE      = DATA[di].d_minute;
                const int SECOND      = DATA[di].d_second;
                const int MILLISECOND = DATA[di].d_millisecond;
                const int OFFSET      = DATA[di].d_offset;

                if (veryVerbose) {
                    T_ P_(LINE) P_(HOUR) P_(MINUTE) P_(SECOND) P_(MILLISECOND)
                                                                      P(OFFSET)
                }

                const bdlt::Time TIME(HOUR, MINUTE, SECOND, MILLISECOND);

                const bdlt::Time INITIAL_TIME(1, 1, 1, 1);
                const int INITIAL_OFFSET = -12;

                bdlt::TimeTz timeTz(INITIAL_TIME, INITIAL_OFFSET);

                timeTz.setTimeTz(TIME, OFFSET);

                const bdlt::Time RESULT_TIME = timeTz.localTime();

                ASSERTV(LINE, TIME, RESULT_TIME, TIME == RESULT_TIME);

                ASSERTV(LINE, OFFSET == timeTz.offset());
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const bdlt::Time TIME_UNSET;
            const bdlt::Time TIME_SET(23, 59, 59, 999);
            Obj timeTz;

            ASSERT_SAFE_PASS(timeTz.setTimeTz(TIME_SET,  1439));
            ASSERT_SAFE_FAIL(timeTz.setTimeTz(TIME_SET,  1440));

            ASSERT_SAFE_PASS(timeTz.setTimeTz(TIME_SET, -1439));
            ASSERT_SAFE_FAIL(timeTz.setTimeTz(TIME_SET, -1440));

            ASSERT_SAFE_PASS(timeTz.setTimeTz(TIME_UNSET,  0));
            ASSERT_SAFE_FAIL(timeTz.setTimeTz(TIME_UNSET,  1));
            ASSERT_SAFE_FAIL(timeTz.setTimeTz(TIME_UNSET, -1));
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'isValid'
        //
        // Concerns:
        //: 1 'isValid' correctly determines whether a date and an associated
        //:   offset are valid attributes of a 'TimeTz' object.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of valid and
        //:   invalid object values. For each value verify that 'isValid'
        //:   returns the correct result.  (C-1..2)
        //
        // Testing:
        //   static bool isValid(const Time& localTime, int offset);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CLASS METHOD 'isValid'" << endl
                          << "======================" << endl;

        if (verbose) cout << "\nUse a table of distinct input values." << endl;

        enum { MAX_TIMEZONE = 24 * 60 - 1,
               MTZ = MAX_TIMEZONE };

        struct {
            int d_line;
            int d_hour;
            int d_minute;
            int d_second;
            int d_millisecond;
            int d_offset;
            int d_isValid;
        } DATA[] = {
            //LN   HOUR  MINUTE  SECOND  MILLISECOND    OFFSET  VALID
            //--   ----  ------  ------  -----------    ------  -----
            { L_,     2,      0,      0,           0,    -4*60,     1 },
            { L_,     0,      2,      0,           0,    -4*60,     1 },
            { L_,     0,      0,      2,           0,    -4*60,     1 },
            { L_,     0,      0,      0,           0,    -4*60,     1 },
            { L_,     2,      0,      0,           0,     -MTZ,     1 },
            { L_,     2,      0,      0,           0,      MTZ,     1 },
            { L_,    24,      0,      0,           0,        0,     1 },
            { L_,    24,      0,      0,           0,        1,     0 },
            { L_,    24,      0,      0,           0,     4*60,     0 },
            { L_,    24,      0,      0,           0,    -4*60,     0 },
            { L_,     2,      0,      0,           0,    24*60,     0 },
            { L_,     2,      0,      0,           0,   -24*60,     0 },
            { L_,     2,      0,      0,           0,    99*60,     0 },
            { L_,     2,      0,      0,           0,   -99*60,     0 },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE        = DATA[di].d_line;
            const int HOUR        = DATA[di].d_hour;
            const int MINUTE      = DATA[di].d_minute;
            const int SECOND      = DATA[di].d_second;
            const int MILLISECOND = DATA[di].d_millisecond;
            const int OFFSET      = DATA[di].d_offset;
            const int IS_VALID    = DATA[di].d_isValid;

            if (veryVerbose) {
                T_
                P_(LINE)
                P_(HOUR)
                P_(MINUTE)
                P_(SECOND)
                P_(MILLISECOND)
                P_(OFFSET)
                P (IS_VALID)
            }

            bdlt::Time time;
            time.setTime(HOUR, MINUTE, SECOND, MILLISECOND);

            LOOP_ASSERT(LINE,
                        !IS_VALID == !bdlt::TimeTz::isValid(time, OFFSET));
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //
        // Plan:
        //: 1 Default construct an object and verify that the object has the
        //:   expected value.  (C-1)
        //
        // Testing:
        //   TimeTz();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DEFAULT CTOR" << endl
                          << "============" << endl;

        if (verbose) cout << "\nCheck default value." << endl;
        {
            const Obj        timeTz;
            const bdlt::Time TIME;

            ASSERTV(timeTz, TIME, TIME == timeTz.localTime());
            ASSERTV(timeTz, TIME, 0    == timeTz.offset());
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

        if (verbose) cout << endl
                          << "TESTING BDEX STREAMING" << endl
                          << "======================" << endl;

        // Allocator to use instead of the default allocator.
        bslma::TestAllocator allocator("bslx", veryVeryVeryVerbose);

        // Scalar object values used in various stream tests.
        const bdlt::Time A(24,  0,  0,   0);
        const bdlt::Time B( 0,  0,  0,   0);
        const bdlt::Time C( 9, 12, 24, 102);
        const bdlt::Time D( 9, 20, 30, 206);
        const bdlt::Time E(17, 34, 52, 503);
        const bdlt::Time F(23, 56, 57, 702);
        const bdlt::Time G(18, 59, 59, 999);

        const Obj VA(A,     0);
        const Obj VB(B,     1);
        const Obj VC(C,    -1);
        const Obj VD(D,   203);
        const Obj VE(E,  -507);
        const Obj VF(F,  1000);
        const Obj VG(G, -1100);

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

                    LOOP_ASSERT(i, (   -1440 < T1.offset()
                                    &&  1440 > T1.offset()
                                    && (   bdlt::Time() != T1.localTime()
                                        || 0 == T1.offset())));

                    LOOP_ASSERT(i, (   -1440 < T2.offset()
                                    &&  1440 > T2.offset()
                                    && (   bdlt::Time() != T2.localTime()
                                        || 0 == T2.offset())));

                    LOOP_ASSERT(i, (   -1440 < T3.offset()
                                    &&  1440 > T3.offset()
                                    && (   bdlt::Time() != T3.localTime()
                                        || 0 == T3.offset())));

                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        if (verbose) {
            cout << "\tOn corrupted data." << endl;
        }

        const Obj W;                     // default value
        const Obj X(bdlt::Time(0), -3);  // original (control)
        const Obj Y(bdlt::Time(0),  1);  // new (streamed-out)

        // Verify the three objects are distinct.
        ASSERT(W != X);
        ASSERT(W != Y);
        ASSERT(X != Y);

        const int SERIAL_Y = 1;       // internal rep. of 'Y.offset()'

        if (verbose) {
            cout << "\t\tGood stream (for control)." << endl;
        }
        {
            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            bdexStreamOut(out, bdlt::Time(0), VERSION);
            out.putInt32(SERIAL_Y);

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

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            bdexStreamOut(out, bdlt::Time(0), VERSION);
            out.putInt32(SERIAL_Y);

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

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            bdexStreamOut(out, bdlt::Time(0), VERSION);
            out.putInt32(SERIAL_Y);

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
            cout << "\t\tOffset too small." << endl;
        }
        {
            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            bdexStreamOut(out, bdlt::Time(0), VERSION);
            out.putInt32(-1440);

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
            cout << "\t\tOffset too large." << endl;
        }
        {
            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            bdexStreamOut(out, bdlt::Time(0), VERSION);
            out.putInt32(1440);

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
            cout << "\t\tOffset non-zero when must be zero." << endl;
        }
        {
            Out out(VERSION_SELECTOR, &allocator);
            using bslx::OutStreamFunctions::bdexStreamOut;

            bdexStreamOut(out, bdlt::Time(24), VERSION);
            out.putInt32(1);

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
                int         d_offset;       // specification offset
                int         d_version;      // version to stream with
                int         d_length;       // expect output length
                const char *d_fmt_p;        // expected output format
            } DATA[] = {
                //LINE  OFFSET  VER  LEN  FORMAT
                //----  ------  ---  ---  ----------------------------------
                { L_,       -1,   1,   8, "\x00\x00\x00\x00\xff\xff\xff\xff" },
                { L_,        0,   1,   8, "\x00\x00\x00\x00\x00\x00\x00\x00" },
                { L_,        1,   1,   8, "\x00\x00\x00\x00\x00\x00\x00\x01" }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_lineNum;
                const int         OFFSET      = DATA[i].d_offset;
                const int         VERSION     = DATA[i].d_version;
                const int         LEN         = DATA[i].d_length;
                const char *const FMT         = DATA[i].d_fmt_p;

                // Test using class methods.

                {
                    Obj        mX(bdlt::Time(0), OFFSET);
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
                    Obj        mX(bdlt::Time(0), OFFSET);
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
        //: 2 Using the table-driven technique, specify a set S of (unique)
        //:   objects with substantial and varied differences in value.
        //:   Construct and initialize all combinations (U, V) in the cross
        //:   product S x S, assign U from V, and verify the remaining
        //:   concerns.  (C-1, C-3..5)
        //
        // Testing:
        //   TimeTz& operator=(const TimeTz& rhs);
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

        if (verbose) cout << "\nCreate a table of distinct values." << endl;
        {
            struct {
                int d_line;         // line number
                int d_hour;         // 'Time' hour attribute
                int d_minute;       // 'Time' minute attribute
                int d_second;       // 'Time' second attribute
                int d_millisecond;  // 'Time' millisecond attribute
                int d_offset;       // offset
            } DATA[] = {
                //    v------------- TIME ------------v
                //LN  HOUR  MINUTE  SECOND  MILLISECOND  OFFSET
                //--  ----  ------  ------  -----------  ------
                {L_,    24,      0,      0,           0,      0 },

                {L_,    23,     59,     59,         999,      0 },
                {L_,     0,      0,      0,           0,      0 },

                {L_,    23,     59,     59,         999,      1 },
                {L_,     0,      0,      0,           0,      1 },

                {L_,    23,     59,     59,         999,     -1 },
                {L_,     0,      0,      0,           0,     -1 },

                {L_,    23,     59,     59,         999,   1439 },
                {L_,     0,      0,      0,           0,   1439 },

                {L_,    23,     59,     59,         999,  -1439 },
                {L_,     0,      0,      0,           0,  -1439 },
            };
            int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int X_LINE        = DATA[ti].d_line;
                const int X_HOUR        = DATA[ti].d_hour;
                const int X_MINUTE      = DATA[ti].d_minute;
                const int X_SECOND      = DATA[ti].d_second;
                const int X_MILLISECOND = DATA[ti].d_millisecond;
                const int X_OFFSET      = DATA[ti].d_offset;

                // Prepare inputs.

                const bdlt::Time X_TIME(X_HOUR, X_MINUTE, X_SECOND,
                                                                X_MILLISECOND);

                // Construct an object.

                Obj mX(X_TIME, X_OFFSET); const Obj& X = mX;

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int Y_LINE        = DATA[tj].d_line;
                    const int Y_HOUR        = DATA[tj].d_hour;
                    const int Y_MINUTE      = DATA[tj].d_minute;
                    const int Y_SECOND      = DATA[tj].d_second;
                    const int Y_MILLISECOND = DATA[tj].d_millisecond;
                    const int Y_OFFSET      = DATA[tj].d_offset;

                    if (veryVerbose) {
                        T_
                        P_(X_LINE)
                        P (Y_LINE)
                    }

                    // Prepare inputs.

                    const bdlt::Time Y_TIME(Y_HOUR, Y_MINUTE, Y_SECOND,
                                                                Y_MILLISECOND);

                    // Construct an object.

                    Obj mY(Y_TIME, Y_OFFSET); const Obj& Y = mY;

                    // normal case

                    {
                        // Assign new value.

                        Obj lhs(X); Obj rhs(Y);
                        Obj *addr = &(lhs = rhs);

                        // Check values.

                        ASSERTV(X_LINE, Y_LINE, lhs, Y, Y        == lhs);
                        ASSERTV(X_LINE, Y_LINE, rhs, Y, Y        == rhs);
                        ASSERTV(X_LINE, Y_LINE, addr, &lhs, &lhs == addr);
                    }

                }

                // aliasing case

                {
                    // Assign new value.

                    Obj lhs(X);
                    Obj *addr = &(lhs = lhs);

                    // Check values.

                    ASSERTV(X_LINE, lhs, X, X        == lhs);
                    ASSERTV(X_LINE, addr, &lhs, &lhs == addr);
                }
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

        if (verbose) cout << "Not implemented for 'bdlt::TimeTz'." << endl;

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
        //:   object values (one per row) in terms of their attributes.
        //:   Specify a set S whose elements have substantial and varied
        //:   differences in value.  For each element in S, copy construct a
        //:   new object from S and verify the concerns. (C-1..3)
        //
        // Testing:
        //   TimeTz(const TimeTz& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout << "\nCreate objects with expected values." << endl;
        {
            struct {
                int d_line;         // line number
                int d_hour;         // 'Time' hour attribute
                int d_minute;       // 'Time' minute attribute
                int d_second;       // 'Time' second attribute
                int d_millisecond;  // 'Time' millisecond attribute
                int d_offset;       // offset
            } DATA[] = {
                //    v----------- TIME -----------v
                //LN  HOUR  MINUTE  SECOND  MILLISECOND  OFFSET
                //--  ----  ------  ------  -----------  ------
                {L_,    24,      0,      0,           0,      0 },

                {L_,    23,     59,     59,         999,      0 },
                {L_,     0,      0,      0,           0,      0 },
                {L_,     0,      0,      0,           1,      0 },

                {L_,    23,     59,     59,         999,      1 },
                {L_,     0,      0,      0,           0,      1 },
                {L_,     0,      0,      0,           1,      1 },

                {L_,    23,     59,     59,         999,     -1 },
                {L_,     0,      0,      0,           0,     -1 },
                {L_,     0,      0,      0,           1,     -1 },

                {L_,    23,     59,     59,         999,   1439 },
                {L_,     0,      0,      0,           0,   1439 },
                {L_,     0,      0,      0,           1,   1439 },

                {L_,    23,     59,     59,         999,  -1439 },
                {L_,     0,      0,      0,           0,  -1439 },
                {L_,     0,      0,      0,           1,  -1439 },
            };
            int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE        = DATA[ti].d_line;
                const int HOUR        = DATA[ti].d_hour;
                const int MINUTE      = DATA[ti].d_minute;
                const int SECOND      = DATA[ti].d_second;
                const int MILLISECOND = DATA[ti].d_millisecond;
                const int OFFSET      = DATA[ti].d_offset;

                if (veryVerbose) {
                    T_
                    P_(LINE)
                    P_(HOUR)
                    P_(MINUTE)
                    P_(SECOND)
                    P_(MILLISECOND)
                    P(OFFSET)
                }

                // Prepare inputs.

                const bdlt::Time TIME(HOUR, MINUTE, SECOND, MILLISECOND);

                // Construct a reference object.

                Obj mREF(TIME, OFFSET); const Obj& REF = mREF;

                // Construct a test object.

                Obj mX(TIME, OFFSET); const Obj& X = mX;

                // Copy construct the test object.

                Obj mY(mX); const Obj& Y = mY;

                // Check that the original object has not changed value.

                ASSERTV(LINE, TIME, OFFSET, REF == X);

                // Inspect the copy.

                ASSERTV(LINE, TIME, OFFSET, X == Y);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compare
        //:   equal.
        //:
        //: 2 'true  == (X == X)'  (i.e., identity)
        //:
        //: 3 'false == (X != X)'  (i.e., identity)
        //:
        //: 4 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 5 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 6 'X != Y' if and only if '!(X == Y)'
        //:
        //: 7 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 8 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 9 The equality operator's signature and return type are standard.
        //:
        //:10 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:   (C-7..10)
        //:
        //: 2 Using the table-driven technique, specify a set S of unique
        //:   object values having various minor or subtle differences.  Verify
        //:   the correctness of 'operator==' and 'operator!=' using all
        //:   elements (u, v) of the cross product S X S.  (C-1..6)
        //
        // Testing:
        //   bool operator==(const TimeTz& lhs, const TimeTz& rhs);
        //   bool operator!=(const TimeTz& lhs, const TimeTz& rhs);
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

        if (verbose) cout << "\nCreate a table of distinct values." << endl;
        {
            struct {
                int d_line;         // line number
                int d_hour;         // 'Time' hour attribute
                int d_minute;       // 'Time' minute attribute
                int d_second;       // 'Time' second attribute
                int d_millisecond;  // 'Time' millisecond attribute
                int d_offset;       // offset
            } DATA[] = {
                //    v------------- TIME ------------v
                //LN  HOUR  MINUTE  SECOND  MILLISECOND  OFFSET
                //--  ----  ------  ------  -----------  ------
                {L_,    24,      0,      0,           0,      0 },

                {L_,    23,     59,     59,         999,      0 },
                {L_,     0,      0,      0,           0,      0 },

                {L_,    23,     59,     59,         999,      1 },
                {L_,     0,      0,      0,           0,      1 },

                {L_,    23,     59,     59,         999,     -1 },
                {L_,     0,      0,      0,           0,     -1 },

                {L_,    23,     59,     59,         999,   1439 },
                {L_,     0,      0,      0,           0,   1439 },

                {L_,    23,     59,     59,         999,  -1439 },
                {L_,     0,      0,      0,           0,  -1439 },
            };
            int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int X_LINE        = DATA[ti].d_line;
                const int X_HOUR        = DATA[ti].d_hour;
                const int X_MINUTE      = DATA[ti].d_minute;
                const int X_SECOND      = DATA[ti].d_second;
                const int X_MILLISECOND = DATA[ti].d_millisecond;
                const int X_OFFSET      = DATA[ti].d_offset;

                // Prepare inputs.

                const bdlt::Time X_TIME(X_HOUR, X_MINUTE, X_SECOND,
                                                                X_MILLISECOND);

                // Construct an object.

                Obj mX(X_TIME, X_OFFSET); const Obj& X = mX;

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int Y_LINE        = DATA[tj].d_line;
                    const int Y_HOUR        = DATA[tj].d_hour;
                    const int Y_MINUTE      = DATA[tj].d_minute;
                    const int Y_SECOND      = DATA[tj].d_second;
                    const int Y_MILLISECOND = DATA[tj].d_millisecond;
                    const int Y_OFFSET      = DATA[tj].d_offset;

                    if (veryVerbose) {
                        T_
                        P_(X_LINE)
                        P (Y_LINE)
                    }

                    // Prepare inputs.

                    const bdlt::Time Y_TIME(Y_HOUR, Y_MINUTE, Y_SECOND,
                                                                Y_MILLISECOND);

                    // Construct an object.

                    Obj mY(Y_TIME, Y_OFFSET); const Obj& Y = mY;

                    // Compare values.

                    bool EXP = (ti == tj);
                    ASSERTV(X_LINE, Y_LINE, X, Y,  EXP == (X == Y));
                    ASSERTV(X_LINE, Y_LINE, X, Y,  EXP == (Y == X));

                    ASSERTV(X_LINE, Y_LINE, X, Y, !EXP == (X != Y));
                    ASSERTV(X_LINE, Y_LINE, X, Y, !EXP == (Y != X));

                    // Aliasing case

                    ASSERTV(Y_LINE, Y,   Y == Y);
                    ASSERTV(Y_LINE, Y, !(Y != Y));
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)'.
        //
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
        //: 9 Padding provided by 'print' works *within* the padding provided
        //:   by setting the stream's output width.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 7)
        //:
        //: 2 Test that the 'print' method produces the expected results for
        //:   various values of 'level' and 'spacesPerLevel', with various
        //:   width settings for the output stream, and both left and right
        //:   stream alignment.  The time format is already extensively tested
        //:   in 'Time', so it will not be exhausted here.  (C-1, 2, 5, 9)
        //:
        //: 3 Test 'operator<<' using the 'print' method as an oracle.
        //:   (C-3, 7, 8)
        //
        // Testing:
        //   bsl::ostream& operator<<(bsl::ostream& stream, const TimeTz& rhs);
        //   bsl::ostream& print(bsl::ostream& stream, int l, int spl) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        // The global concern that we do not use the default allocator does not
        // hold in this test case, because we use 'stringstream' as part of our
        // test apparatus.

        bslma::TestAllocator
                   alternateDefaultAllocator("alternate", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocator(&alternateDefaultAllocator);

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

        if (verbose) cout << "\nTesting various print specifications." << endl;
        {
            static const struct {
                int         d_line;     // line number
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                int         d_offset;   // tz offset
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //LN  INDENT  SPACES  OFFSET            FORMAT
                //--  ------  ------  ------   ------------------------
                {L_,       0,     -1,      0,       "17:00:00.000+0000" },
                {L_,       0,      0,     15,     "17:00:00.000+0015\n" },
                {L_,       0,      2,     60,     "17:00:00.000+0100\n" },
                {L_,       1,      1,     90,    " 17:00:00.000+0130\n" },
                {L_,       1,      2,    -20,   "  17:00:00.000-0020\n" },
                {L_,      -1,      2,   -330,     "17:00:00.000-0530\n" },
                {L_,      -2,      1,    311,     "17:00:00.000+0511\n" },
                {L_,       2,      1,   1439,   "  17:00:00.000+2359\n" },
                {L_,       1,      3,  -1439,  "   17:00:00.000-2359\n" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE = DATA[ti].d_line;
                const int         IND  = DATA[ti].d_indent;
                const int         SPL  = DATA[ti].d_spaces;
                const int         OFF  = DATA[ti].d_offset;
                const char *const FMT  = DATA[ti].d_fmt_p;

                if (veryVerbose) {
                    T_ P_(LINE) P_(IND) P_(SPL) P_(OFF) P(FMT)
                }

                bdlt::Time time(17);  // 17:00:00.000
                Obj mX(time, OFF);  const Obj& X = mX;

                ostringstream output;
                ASSERTV(LINE, &output == &X.print(output, IND, SPL));

                ASSERTV(LINE, output.str(), FMT, FMT == output.str());
            }
        }

        if (verbose) cout << "\nTesting 'print' with "
                             "manipulators and left alignment." << endl;
        {
            static const struct {
                int         d_line;     // line number
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                int         d_offset;   // tz offset
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //LN  IND.  SPC.  OFFSET                FORMAT
                //--  ----  ----  ------  ----------------------------------
                {L_,     0,   -1,      0,   "17:00:00.000+0000@@@@@@@@@@@@@" },
                {L_,     0,    0,     15,  "17:00:00.000+0015\n@@@@@@@@@@@@" },
                {L_,     0,    2,     60,  "17:00:00.000+0100\n@@@@@@@@@@@@" },
                {L_,     1,   -1,      1,   " 17:00:00.000+0001@@@@@@@@@@@@" },
                {L_,     1,    0,      2,  "17:00:00.000+0002\n@@@@@@@@@@@@" },
                {L_,     1,    1,     90,  " 17:00:00.000+0130\n@@@@@@@@@@@" },
                {L_,     1,    2,    -20,  "  17:00:00.000-0020\n@@@@@@@@@@" },
                {L_,    -1,    0,      0,  "17:00:00.000+0000\n@@@@@@@@@@@@" },
                {L_,    -1,    2,   -330,  "17:00:00.000-0530\n@@@@@@@@@@@@" },
                {L_,    -2,    1,    311,  "17:00:00.000+0511\n@@@@@@@@@@@@" },
                {L_,    -1,   -2,   -330,   "17:00:00.000-0530@@@@@@@@@@@@@" },
                {L_,     2,    1,   1439,  "  17:00:00.000+2359\n@@@@@@@@@@" },
                {L_,     1,    3,  -1439,  "   17:00:00.000-2359\n@@@@@@@@@" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const char FILL_CHAR    = '@'; // Used for filling whitespaces due
                                           // to 'setw'.
            const int  FORMAT_WIDTH = 30;

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE = DATA[ti].d_line;
                const int         IND  = DATA[ti].d_indent;
                const int         SPL  = DATA[ti].d_spaces;
                const int         OFF  = DATA[ti].d_offset;
                const char *const FMT  = DATA[ti].d_fmt_p;

                if (veryVerbose) {
                    T_ P_(LINE) P_(IND) P_(SPL) P_(OFF) P(FMT)
                }

                bdlt::Time time(17);  // 17:00:00.000
                Obj mX(time, OFF);  const Obj& X = mX;

                ostringstream output;
                output << left << setfill(FILL_CHAR) << setw(FORMAT_WIDTH);
                ASSERTV(LINE, &output == &X.print(output, IND, SPL));

                ASSERTV(LINE, output.str(), FMT, FMT == output.str());
            }
        }

        if (verbose) cout << "\nTesting 'print' with "
                             "manipulators and right alignment." << endl;
        {
            static const struct {
                int         d_line;     // line number
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                int         d_offset;   // tz offset
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //LN  IND.  SPC.  OFFSET                FORMAT
                //--  ----  ----  ------  ----------------------------------
                {L_,     0,   -1,      0,   "@@@@@@@@@@@@@17:00:00.000+0000" },
                {L_,     0,    0,     15,  "@@@@@@@@@@@@17:00:00.000+0015\n" },
                {L_,     0,    2,     60,  "@@@@@@@@@@@@17:00:00.000+0100\n" },
                {L_,     1,   -1,      1,   "@@@@@@@@@@@@ 17:00:00.000+0001" },
                {L_,     1,    0,      2,  "@@@@@@@@@@@@17:00:00.000+0002\n" },
                {L_,     1,    1,     90,  "@@@@@@@@@@@ 17:00:00.000+0130\n" },
                {L_,     1,    2,    -20,  "@@@@@@@@@@  17:00:00.000-0020\n" },
                {L_,    -1,    0,      0,  "@@@@@@@@@@@@17:00:00.000+0000\n" },
                {L_,    -1,    2,   -330,  "@@@@@@@@@@@@17:00:00.000-0530\n" },
                {L_,    -2,    1,    311,  "@@@@@@@@@@@@17:00:00.000+0511\n" },
                {L_,    -1,   -2,   -330,   "@@@@@@@@@@@@@17:00:00.000-0530" },
                {L_,     2,    1,   1439,  "@@@@@@@@@@  17:00:00.000+2359\n" },
                {L_,     1,    3,  -1439,  "@@@@@@@@@   17:00:00.000-2359\n" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const char FILL_CHAR    = '@'; // Used for filling whitespaces due
                                           // to 'setw'.

            const int  FORMAT_WIDTH = 30;

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE = DATA[ti].d_line;
                const int         IND  = DATA[ti].d_indent;
                const int         SPL  = DATA[ti].d_spaces;
                const int         OFF  = DATA[ti].d_offset;
                const char *const FMT  = DATA[ti].d_fmt_p;

                if (veryVerbose) {
                    T_ P_(LINE) P_(IND) P_(SPL) P_(OFF) P(FMT)
                }

                bdlt::Time time(17);  // 17:00:00.000
                Obj mX(time, OFF);  const Obj& X = mX;

                ostringstream output;
                output << right << setfill(FILL_CHAR) << setw(FORMAT_WIDTH);
                ASSERTV(LINE, &output == &X.print(output, IND, SPL));

                ASSERTV(LINE, output.str(), FMT, FMT == output.str());
            }
        }

        if (verbose) cout << "\nTesting default values." << endl;
        {
            bdlt::Time time(17);  // 17:00:00.000
            Obj mX(time, 1234);  const Obj& X = mX;

            ostringstream output;
            ASSERT(&output == &X.print(output));

            ostringstream oracle;
            X.print(oracle, 0, 4);

            ASSERTV(output.str(), oracle.str(), oracle.str() == output.str());
        }

        if (verbose) cout << "\nTesting 'operator<<'" << endl;
        {
            static const struct {
                int         d_line;     // line number
                int         d_offset;   // tz offset
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //LN  OFFSET          FORMAT
                //--  ------   -------------------
                {L_,       0,  "17:00:00.000+0000" },
                {L_,      15,  "17:00:00.000+0015" },
                {L_,      60,  "17:00:00.000+0100" },
                {L_,      90,  "17:00:00.000+0130" },
                {L_,     -20,  "17:00:00.000-0020" },
                {L_,    -330,  "17:00:00.000-0530" },
                {L_,     311,  "17:00:00.000+0511" },
                {L_,    1439,  "17:00:00.000+2359" },
                {L_,   -1439,  "17:00:00.000-2359" },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int IND = 0;
            const int SPL = -1;

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const int         OFF    = DATA[ti].d_offset;
                const char *const FORMAT = DATA[ti].d_fmt_p;

                if (veryVerbose) {
                    T_ P_(LINE) P_(OFF) P(FORMAT)
                }

                bdlt::Time time(17);  // 17:00:00.000
                Obj mX(time, OFF);  const Obj& X = mX;

                const char FILL_CHAR = '@'; // Used for filling whitespaces due
                                            // to 'setw'.

                const int  FORMAT_WIDTH = 20;

                const int  OUTPUT_WIDTH = 17;  // expected width of bare
                                               // 'TimeTz' representation

                const string FILL(FORMAT_WIDTH - OUTPUT_WIDTH, FILL_CHAR);

                // Left fill
                {
                    ostringstream output;
                    ostringstream oracle;

                    const string EXPECTED = FILL + FORMAT;

                    output
                        << right << setfill(FILL_CHAR) << setw(FORMAT_WIDTH);
                    ASSERTV(LINE, &output == &(output << X));

                    oracle
                        << right << setfill(FILL_CHAR) << setw(FORMAT_WIDTH);
                    X.print(oracle, IND, SPL);

                    if (veryVeryVerbose) {
                        T_ P_(LINE)
                           P_(output.str())
                           P_(output.str().size())
                           P_(oracle.str())
                           P_(EXPECTED)
                           P (EXPECTED.size())
                    }

                    ASSERTV(LINE, output.str(), oracle.str(),
                                                 oracle.str() == output.str());

                    ASSERTV(LINE, output.str(), EXPECTED,
                                                     EXPECTED == output.str());
                }

                // Right fill
                {
                    ostringstream output;
                    ostringstream oracle;

                    const string EXPECTED = FORMAT + FILL;

                    output
                        << left << setfill(FILL_CHAR) << setw(FORMAT_WIDTH);
                    ASSERTV(LINE, &output == &(output << X));

                    oracle
                        << left << setfill(FILL_CHAR) << setw(FORMAT_WIDTH);
                    X.print(oracle, IND, SPL);

                    if (veryVeryVerbose) {
                        T_ P_(LINE)
                           P_(output.str())
                           P_(oracle.str())
                           P_(EXPECTED)
                    }

                    ASSERTV(LINE, output.str(), oracle.str(),
                                                 oracle.str() == output.str());

                    ASSERTV(LINE, output.str(), EXPECTED,
                                                     EXPECTED == output.str());
                }
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //
        // Concerns:
        //: 1 'offset' can return all values in the open range
        //:   '( -1440 .. 1400 )'.
        //:
        //: 2 'offset' returns the offset value supplied to the constructor.
        //:
        //: 3 'localTime' returns the local time value supplied to the
        //:   constructor.
        //:
        //: 4 'localTime' can return both set and unset ('24:00:00.000')
        //:   values.
        //:
        //: 5 All accessors are 'const'.
        //
        // Plan:
        //: 1 Using the table-driven approach, construct objects with a set of
        //:   distinct values, and verify that the functions under test return
        //:   the expected results.  (C-1..4)
        //:
        //: 2 Invoke each function under test from a reference providing
        //:   non-modifiable access to the object.  (C-5)
        //
        // Testing:
        //   Time localTime() const;
        //   int offset() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\n'localTime', 'offset'." << endl;
        {
            struct {
                int d_line;         // line number
                int d_hour;         // 'Time' hour attribute
                int d_minute;       // 'Time' minute attribute
                int d_second;       // 'Time' second attribute
                int d_millisecond;  // 'Time' millisecond attribute
                int d_offset;       // offset
            } DATA[] = {
                //    v-------------- TIME -----------v
                //LN  HOUR  MINUTE  SECOND  MILLISECOND  OFFSET
                //--  ----  ------  ------  -----------  ------

                // localTime
                {L_,    24,      0,      0,           0,      0 },

                {L_,     0,      0,      0,           0,      0 },

                {L_,     0,      0,      0,           1,      0 },
                {L_,     0,      0,      0,           2,      0 },
                {L_,     0,      0,      0,         998,      0 },
                {L_,     0,      0,      0,         999,      0 },
                {L_,     0,      0,      1,           0,      0 },
                {L_,     0,      0,      2,           0,      0 },
                {L_,     0,      0,     58,           0,      0 },
                {L_,     0,      0,     59,           0,      0 },
                {L_,     0,      1,      0,           0,      0 },
                {L_,     0,      2,      0,           0,      0 },
                {L_,     0,     58,      0,           0,      0 },
                {L_,     0,     59,      0,           0,      0 },
                {L_,     1,      0,      0,           0,      0 },
                {L_,     2,      0,      0,           0,      0 },
                {L_,    22,      0,      0,           0,      0 },
                {L_,    23,      0,      0,           0,      0 },

                // offset
                {L_,     0,      0,      0,           0,      0 },
                {L_,     0,      0,      0,           0,      1 },
                {L_,     0,      0,      0,           0,      2 },
                {L_,     0,      0,      0,           0,   1438 },
                {L_,     0,      0,      0,           0,   1439 },
                {L_,     0,      0,      0,           0,     -1 },
                {L_,     0,      0,      0,           0,     -2 },
                {L_,     0,      0,      0,           0,  -1438 },
                {L_,     0,      0,      0,           0,  -1439 },
            };
            int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE        = DATA[ti].d_line;
                const int HOUR        = DATA[ti].d_hour;
                const int MINUTE      = DATA[ti].d_minute;
                const int SECOND      = DATA[ti].d_second;
                const int MILLISECOND = DATA[ti].d_millisecond;
                const int OFFSET      = DATA[ti].d_offset;

                if (veryVerbose) {
                    T_
                    P_(LINE)
                    P_(HOUR)
                    P_(MINUTE)
                    P_(SECOND)
                    P_(MILLISECOND)
                    P(OFFSET)
                }

                // Prepare inputs.

                const bdlt::Time TIME(HOUR, MINUTE, SECOND, MILLISECOND);

                // Construct an object.

                Obj mX(TIME, OFFSET); const Obj& X = mX;

                // Inspect attributes.

                ASSERTV(LINE, TIME, OFFSET, TIME   == X.localTime());
                ASSERTV(LINE, TIME, OFFSET, OFFSET == X.offset());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //   Reserved for test apparatus.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST APPARATUS" << endl
                          << "==============" << endl;

        if (verbose) cout << "Not implemented." << endl;
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // VALUE CTOR & DTOR
        //
        // Concerns:
        //: 1 Object can be constructed from all valid combinations of
        //:   'localTime' and 'offset'.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using a table-based approach, iterate through representative
        //:   combinations of 'localTime' and 'offset', obtained through
        //:   category partitioning, and construct a 'TimeTz' object from each
        //:   combination.  Note that, since 'Time' has already been tested,
        //:   and the 'localTime' attribute is inspected only when comparing
        //:   its value to '24:00:00.0000', we need only two distinct values
        //:   for 'localTime'.  (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to construct an object with an
        //:   invalid combination of 'localTime' and 'offset', including
        //:   combinations where one of the two is invalid of itself (using the
        //:   'BSLS_ASSERTTEST_*' macros).  (C-2)
        //
        // Testing:
        //   TimeTz(const Time& localTime, int offset);
        //   ~TimeTz();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CTOR & DTOR" << endl
                          << "=================" << endl;

        if (verbose) cout << "\nTest value ctor with expected values." << endl;
        {
            struct {
                int d_line;         // line number
                int d_hour;         // 'Time' hour attribute
                int d_minute;       // 'Time' minute attribute
                int d_second;       // 'Time' second attribute
                int d_millisecond;  // 'Time' millisecond attribute
                int d_offset;       // offset
            } DATA[] = {
                //    v------------ TIME  ------------v
                //LN  HOUR  MINUTE  SECOND  MILLISECOND  OFFSET
                //--  ----  ------  ------  -----------  ------
                {L_,    24,      0,      0,           0,      0 },

                {L_,    23,     59,     59,         999,      0 },
                {L_,     0,      0,      0,           0,      0 },

                {L_,    23,     59,     59,         999,      1 },
                {L_,     0,      0,      0,           0,      1 },

                {L_,    23,     59,     59,         999,     -1 },
                {L_,     0,      0,      0,           0,     -1 },

                {L_,    23,     59,     59,         999,   1439 },
                {L_,     0,      0,      0,           0,   1439 },

                {L_,    23,     59,     59,         999,  -1439 },
                {L_,     0,      0,      0,           0,  -1439 },
            };
            int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE        = DATA[ti].d_line;
                const int HOUR        = DATA[ti].d_hour;
                const int MINUTE      = DATA[ti].d_minute;
                const int SECOND      = DATA[ti].d_second;
                const int MILLISECOND = DATA[ti].d_millisecond;
                const int OFFSET      = DATA[ti].d_offset;

                if (veryVerbose) {
                    T_
                    P_(LINE)
                    P_(HOUR)
                    P_(MINUTE)
                    P_(SECOND)
                    P_(MILLISECOND)
                    P (OFFSET)
                }

                // Prepare inputs.

                const bdlt::Time TIME(HOUR, MINUTE, SECOND, MILLISECOND);

                // Construct an object.

                Obj mX(TIME, OFFSET); const Obj& X = mX;

                // Inspect attributes.

                ASSERTV(LINE, TIME, OFFSET, TIME   == X.localTime());
                ASSERTV(LINE, TIME, OFFSET, OFFSET == X.offset());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const bdlt::Time TIME_UNSET;
            const bdlt::Time TIME_SET(23, 59, 59, 999);

            ASSERT_SAFE_PASS(Obj(TIME_SET,  1439));
            ASSERT_SAFE_FAIL(Obj(TIME_SET,  1440));

            ASSERT_SAFE_PASS(Obj(TIME_SET, -1439));
            ASSERT_SAFE_FAIL(Obj(TIME_SET, -1440));

            ASSERT_SAFE_PASS(Obj(TIME_UNSET,  0));
            ASSERT_SAFE_FAIL(Obj(TIME_UNSET,  1));
            ASSERT_SAFE_FAIL(Obj(TIME_UNSET, -1));
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
        //: 1 Create an object x1 (init. to VA).    { x1:VA }
        //: 2 Create an object x2 (copy from x1).   { x1:VA x2:VA }
        //: 3 Set x1 to VB.                         { x1:VB x2:VA }
        //: 4 Create an object x3 (default ctor).   { x1:VB x2:VA x3:U }
        //: 5 Create an object x4 (copy from x3).   { x1:VB x2:VA x3:U  x4:U }
        //: 6 Set x3 to VC.                         { x1:VB x2:VA x3:VC x4:U }
        //: 7 Assign x2 = x1.                       { x1:VB x2:VB x3:VC x4:U }
        //: 8 Assign x2 = x3.                       { x1:VB x2:VC x3:VC x4:U }
        //: 9 Assign x1 = x1 (aliasing).            { x1:VB x2:VB x3:VC x4:U }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Values for testing

        const int HRA = 1, MIA = 2, SCA = 3;  // h, m, s, ms for VA
        const int HRB = 5, MIB = 6, SCB = 7;  // h, m, s, ms for VB
        const int HRC = 9, MIC = 9, SCC = 9;  // h, m, s, ms for VC

        const bdlt::Time TA(HRA, MIA, SCA),
                         TB(HRB, MIB, SCB),
                         TC(HRC, MIC, SCC);

        const int OA = 60, OB = -300, OC = 270;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(TA, OA);  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(TA == X1.localTime());
        ASSERT(OA == X1.offset());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta. Check the initial state of x2." << endl;
        ASSERT(TA == X2.localTime());
        ASSERT(OA == X2.offset());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;
        mX1.setTimeTz(TB, OB);
        if (verbose) { cout << '\t';  P(X1); }
        ASSERT(TB == X1.localTime());
        ASSERT(OB == X1.offset());

        if (verbose) cout << "\ta. Check new state of x1." << endl;

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create a default object x3()."
                             "\t\t{ x1:VB x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(bdlt::Time() == X3.localTime());
        ASSERT(           0 == X3.offset());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Create an object x4 (copy from x3)."
                             "\t\t{ x1:VA x2:VA x3:U  x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(bdlt::Time() == X4.localTime());
        ASSERT(           0 == X4.offset());

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;
        mX3.setTimeTz(TC, OC);
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(TC == X3.localTime());
        ASSERT(OC == X3.offset());

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
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(TB == X2.localTime());
        ASSERT(OB == X2.offset());

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
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(TC == X2.localTime());
        ASSERT(OC == X2.offset());

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
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(TB == X1.localTime());
        ASSERT(OB == X1.offset());

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

    // CONCERN: no use of global or default allocators

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

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
