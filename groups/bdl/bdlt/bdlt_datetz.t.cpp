// bdlt_datetz.t.cpp                                                  -*-C++-*-
#include <bdlt_datetz.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bsls_assert.h>
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
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                   TEST PLAN
// ============================================================================
//
//                                   Overview
//                                   --------
// The component under test implements a single value-semantic class,
// 'bdlt::DateTz', that represents a date value with a local time offset.
//
// Primary Manipulators:
//: o VALUE CONSTRUCTOR
//
// Basic Accessors:
//: o 'localDate'
//: o 'offset'
//
// This particular class provides a value constructor capable of creating an
// object in any state relevant for thorough testing.  The value constructor
// serves as our primary manipulator and also obviates the primitive generator
// function, 'gg', normally used for this purpose.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [12] static bool isValid(const Date& localDate, int offset);
// [10] static int maxSupportedBdexVersion(int versionSelector);
//
// CREATORS
// [11] DateTz();
// [ 7] DateTz(const DateTz& original);
// [ 2] DateTz(const Date& localDate, int offset);
// [ 2] ~DateTz();
//
// MANIPULATORS
// [ 9] Date& operator=(const Date& rhs);
// [11] void setDateTz(const Date& localDate, int offset);
// [12] bool setDateTzIfValid(const Date& localDate, int offset);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
//
// ACCESSORS
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [13] Datetime utcStartTime() const;
// [ 4] Date localDate() const;
// [ 4] int offset() const;
// [ 5] bsl::ostream& print(bsl::ostream& stream, int l, int spl) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const DateTz& lhs, const DateTz& rhs);
// [ 6] bool operator!=(const DateTz& lhs, const DateTz& rhs);
// [ 5] bsl::ostream& operator<<(bsl::ostream&, const DateTz&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [16] USAGE EXAMPLE
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

typedef bdlt::DateTz        Obj;
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
      case 16: {
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
///Example 1: Representing Dates In Different Time Zones
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we need to compare dates in different time zones.  The
// 'bdlt::DateTz' type helps us to accomplish this.
//
// First, we default construct an object 'dateTz1', which has an offset of 0,
// implying that the object represents a date in the UTC time zone.
//..
    bdlt::DateTz dateTz1;
    ASSERT(0                   == dateTz1.offset());
    ASSERT(dateTz1.localDate() == dateTz1.utcStartTime().date());
    ASSERT(dateTz1.localDate() == bdlt::Date());
//..
// Notice the value of a default contructed 'bdlt::DateTz' object is the same
// as that of a default constructed 'bdlt::Date' object.
//
// Then, we construct two objects 'dateTz2' and 'dateTz3' to have a local date
// of 2013/12/31 in the EST time zone (UTC-5) and the pacific time zone (UTC-8)
// respectively:
//..
    bdlt::DateTz dateTz2 (bdlt::Date(2013, 12, 31), -5 * 60);
    bdlt::DateTz dateTz3 (bdlt::Date(2013, 12, 31), -8 * 60);
//..
// Next, we compare the local dates of the two 'DateTz' objects, and verify
// that they compare equal:
//..
    bdlt::Date localDate(2013, 12, 31);
    ASSERT(localDate == dateTz2.localDate());
    ASSERT(localDate == dateTz3.localDate());
//..
// Finally, we compare the starting time of the two 'DateTz' objects using the
// 'utcStartTime' method:
//..
    ASSERT(dateTz2.utcStartTime() < dateTz3.utcStartTime());
//..
      } break;
      case 15: {
        // Deprecated test case.  Do not remove.
      } break;
      case 14: {
        // Deprecated test case.  Do not remove.
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // 'utcStartTime' METHOD
        //
        // Concerns:
        //: 1 'utcStartTime' computes the correct UTC start time of the local
        //:   date accordingly to its timezone.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of dates, their
        //:   associated offset, and their starting UTC times. Verify that
        //:   'utcStartTime' return the expected value the following invariant
        //:   hold: 'utcStartTime() == localDate() - offset()'.  (C-1)
        //
        // Testing:
        //   Datetime utcStartTime() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'utcStartTime' METHOD" << endl
                          << "=====================" << endl;
        struct {
            int d_line;       // line number
            int d_year;       // year of date
            int d_month;      // month of date
            int d_day;        // day of month of date
            int d_offset;     // timezone offset
            int d_utcYear;    // expected UTC year
            int d_utcMonth;   // expected UTC month
            int d_utcDay;     // expected UTC day
            int d_utcHour;    // expected UTC hour
            int d_utcMinute;  // expected UTC minute
        } DATA[] = {
            //LINE YR MO D  OFF G_Y G_M G_D G_H G_M
            //---- -- -- -- --- --- --- --- --- ---
            { L_,   1, 1, 1,  0,  1,  1,  1,  0,  0 },
            { L_,   1, 1, 1, -1,  1,  1,  1,  0,  1 },
            { L_,   1, 1, 2,  1,  1,  1,  1, 23, 59 }
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE       = DATA[i].d_line;
            const int YEAR       = DATA[i].d_year;
            const int MONTH      = DATA[i].d_month;
            const int DAY        = DATA[i].d_day;
            const int OFFSET     = DATA[i].d_offset;
            const int UTC_YEAR   = DATA[i].d_utcYear;
            const int UTC_MONTH  = DATA[i].d_utcMonth;
            const int UTC_DAY    = DATA[i].d_utcDay;
            const int UTC_HOUR   = DATA[i].d_utcHour;
            const int UTC_MINUTE = DATA[i].d_utcMinute;

            if(veryVerbose) {
                T_ P_(LINE) P_(YEAR) P_(MONTH) P_(DAY) P_(OFFSET) P_(UTC_YEAR)
                          P_(UTC_MONTH) P_(UTC_DAY) P_(UTC_HOUR) P(UTC_MINUTE)
            }

            const bdlt::Date TEMP_DATE(YEAR, MONTH, DAY);
            const Obj X(TEMP_DATE, OFFSET);

            const bdlt::Datetime EXP1(UTC_YEAR,
                                      UTC_MONTH,
                                      UTC_DAY,
                                      UTC_HOUR,
                                      UTC_MINUTE);

            bdlt::Datetime exp2(bdlt::Datetime(X.localDate()));
            const bdlt::Datetime& EXP2 = exp2;
            exp2.addMinutes(-X.offset());

            if (veryVerbose) {
                T_  cout << "UTC START TIME: " << X.utcStartTime() << endl;
            }
            ASSERTV(LINE, EXP1 == X.utcStartTime());
            ASSERTV(LINE, EXP2 == X.utcStartTime());
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // 'isValid' AND 'setDateTzIfValid' METHODS
        //
        // Concerns:
        //: 1 'isValid' correctly determines whether a date and an associated
        //:   offset are valid attributes of a 'DateTz' object.
        //:
        //: 2 'setDateTzIfValid' sets the date and offset of a 'DateTz'
        //:   object to the specified values only if they are valid.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of valid and
        //:   invalid object values. For each value verify that 'isValid'
        //:   returns the correct result and 'setDateTzIfValid' behaves
        //:   correctly.  (C-1..2)
        //
        // Testing:
        //   static bool isValid(const Date& localDate, int offset);
        //   bool setDateTzIfValid(const Date& localDate, int offset);
        // --------------------------------------------------------------------

        // Set the assert handler to mute error coming from construction of an
        // invalid date.

        if (verbose)
            cout << endl << "'isValid' AND 'setDateTzIfValid' METHODS"
                 << endl << "========================================"
                 << endl;

        enum { MAX_TIMEZONE = 24 * 60 - 1,
               MTZ = MAX_TIMEZONE };

        struct {
            int d_line;     // line number
            int d_year;     // year of date
            int d_month;    // month of date
            int d_day;      // day of month of date
            int d_offset;   // timezone offset
            int d_isValid;  // is valid
        } DATA[] = {
            //LINE   YEAR   MO  DAY     OFF  VALID
            //----   ----   --  ---     ---  -----
            { L_,   1776,   7,   4,      0,      1 },
            { L_,   1776,   7,   4,   4*60,      1 },
            { L_,   1776,   7,   4,  -4*60,      1 },
            { L_,   1776,   7,   4,    MTZ,      1 },
            { L_,   1776,   7,   4,   -MTZ,      1 },
            { L_,   1776,   7,   4,  24*60,      0 },
            { L_,   1776,   7,   4, -24*60,      0 },
            { L_,   1776,   7,   4,  99*60,      0 },
            { L_,   1776,   7,   4, -99*60,      0 },
            { L_,      1,   1,   1,      0,      1 },
            { L_,      1,   1,   1,    MTZ,      1 },
            { L_,      1,   1,   1,   -MTZ,      1 },
            { L_,      1,   1,   1,  24*60,      0 },
            { L_,      1,   1,   1, -24*60,      0 },
            { L_,      1,   1,   1,  99*60,      0 },
            { L_,      1,   1,   1, -99*60,      0 } };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE     = DATA[di].d_line;
            const int YEAR     = DATA[di].d_year;
            const int MONTH    = DATA[di].d_month;
            const int DAY      = DATA[di].d_day;
            const int OFFSET   = DATA[di].d_offset;
            const int IS_VALID = DATA[di].d_isValid;

            if(veryVerbose) {
                T_ P_(LINE) P_(YEAR) P_(MONTH) P_(DAY) P_(OFFSET) P(IS_VALID)
            }

            const bdlt::Date SRC_DATE(YEAR, MONTH, DAY);
            ASSERTV(LINE,
                        IS_VALID == bdlt::DateTz::isValid(SRC_DATE, OFFSET));

            const bdlt::Date DEST_DATE(2003,  3, 18);
            bdlt::DateTz t(DEST_DATE, 5*60);

            int sts = t.setDateTzIfValid(SRC_DATE, OFFSET);
            ASSERTV(LINE, IS_VALID == !sts);

            if (IS_VALID) {
                ASSERTV(LINE, t.localDate().year()  == YEAR);
                ASSERTV(LINE, t.localDate().month() == MONTH);
                ASSERTV(LINE, t.localDate().day()   == DAY);
                ASSERTV(LINE, t.offset() == OFFSET);
            }
            else {
                // verify t was unchanged.

                ASSERTV(LINE, t.localDate().year()  == 2003);
                ASSERTV(LINE, t.localDate().month() == 3);
                ASSERTV(LINE, t.localDate().day()   == 18);
                ASSERTV(LINE, t.offset() == 5*60);
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR AND 'setDateTz' METHOD
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //:
        //: 2 The 'setDateTz' method sets the date and locale offset of the
        //:   object to the specified values.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Default construct an object and verify that the object has the
        //:   expected value.  (C-1)
        //:
        //: 2 Use the table-driven technique, specify a range of distinct date
        //:   and offset values. Pass each value to the 'setDateTz' method and
        //:   verify that the resulting object value is correct.  (C-2)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-3)
        //
        // Testing:
        //   DateTz();
        //   void setDateTz(const Date& localDate, int offset);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DEFAULT CTOR AND 'setDateTz' METHOD" << endl
                          << "===================================" << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;
        {
            Obj mX;  const Obj& X = mX;
            const bdlt::Date& LOCAL_DATE = X.localDate();

            ASSERT(1 == LOCAL_DATE.year());
            ASSERT(1 == LOCAL_DATE.month());
            ASSERT(1 == LOCAL_DATE.day());
            ASSERT(0 == X.offset());
        }

        if (verbose) cout << "\nTesting how 'setDateTz' forwards 'Date'."
                                                                       << endl;
        {
            // First let's make sure that the 'Date' object is passed down
            // correctly, let's use a 3 different offsets.

            static const struct {
            int d_line;     // line number
            int d_year;     // year of date
            int d_month;    // month of date
            int d_day;      // day of month of date
            int d_offset;   // timezone offset
            } DATA[] = {
                //LINE   YEAR    MO  DAY  OFFSET
                //----   ----    --  ---  ------
                { L_,       1,   1,   1,       0 },
                { L_,    1600,   6,  30,    5*60 },
                { L_,    9999,  12,  31,   23*60 }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE   = DATA[i].d_line;
                const int YEAR   = DATA[i].d_year;
                const int MONTH  = DATA[i].d_month;
                const int DAY    = DATA[i].d_day;
                const int OFFSET = DATA[i].d_offset;

                if (veryVerbose) { T_ P_(YEAR) P_(MONTH) P_(DAY) P(OFFSET) }

                const Obj ZZ(bdlt::Date(YEAR, MONTH, DAY), OFFSET);

                Obj mX;  const Obj& X = mX;
                mX.setDateTz(bdlt::Date(YEAR, MONTH, DAY), OFFSET);

                if (veryVeryVerbose) { T_ T_ P_(ZZ) P(X) }
                ASSERTV(LINE, ZZ == X);
            }
        }

        if (verbose) cout << "\nTesting 'setDateTz' with different offsets."
                                                                       << endl;
        {
            // Now we trust that the 'Date' construction part is reliable.
            // Let's verify that the offset part works fine.

            const int DATA[] = { -1439, -1339, -60, -1, 0, 1, 60, 1339, 1439 };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int YEAR   = 1;
                const int MONTH  = 1;
                const int DAY    = 1;
                const int OFFSET = DATA[i];

                if (veryVerbose) { T_ P_(YEAR) P_(MONTH) P_(DAY) P(OFFSET) }

                Obj mX;  const Obj& X = mX;
                bdlt::Date TEMP_DATE(YEAR, MONTH, DAY);
                mX.setDateTz(TEMP_DATE, OFFSET);

                if (veryVeryVerbose) { T_ T_ P(X) }

                const bdlt::Date& LOCAL_DATE = X.localDate();
                ASSERTV(i, YEAR   == LOCAL_DATE.year());
                ASSERTV(i, MONTH  == LOCAL_DATE.month());
                ASSERTV(i, DAY    == LOCAL_DATE.day());
                ASSERTV(i, OFFSET == X.offset());
            }
        }
        // Negative Testing
        if (verbose)
            cout << "\nTesting 'setDateTz' with invalid data (negative test)."
                 << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'timeout'" << endl;
            {
                Obj mX;
                ASSERT_SAFE_FAIL(mX.setDateTz(bdlt::Date(), 1440));
                ASSERT_SAFE_PASS(mX.setDateTz(bdlt::Date(), 1439));
                ASSERT_SAFE_FAIL(mX.setDateTz(bdlt::Date(), -1440));
                ASSERT_SAFE_PASS(mX.setDateTz(bdlt::Date(), -1439));
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

        if (verbose) cout << endl
                          << "TESTING BDEX STREAMING" << endl
                          << "======================" << endl;

        // Allocator to use instead of the default allocator.
        bslma::TestAllocator allocator("bslx", veryVeryVeryVerbose);

        // Scalar object values used in various stream tests.
        const bdlt::Date A(   1,  1,  1);
        const bdlt::Date B(   1,  1,  2);
        const bdlt::Date C(   3,  4,  7);
        const bdlt::Date D(2012,  4,  7);
        const bdlt::Date E(2014,  6, 14);
        const bdlt::Date F(2014, 10, 22);
        const bdlt::Date G(9999, 12, 31);

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

                    LOOP_ASSERT(i, -1440 < T1.offset() && 1440 > T1.offset());
                    LOOP_ASSERT(i, -1440 < T2.offset() && 1440 > T2.offset());
                    LOOP_ASSERT(i, -1440 < T3.offset() && 1440 > T3.offset());

                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        if (verbose) {
            cout << "\tOn corrupted data." << endl;
        }

        const Obj W;                           // default value
        const Obj X(bdlt::Date(1, 1, 1), -3);  // original (control)
        const Obj Y(bdlt::Date(1, 1, 1),  1);  // new (streamed-out)

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

            bdexStreamOut(out, bdlt::Date(1, 1, 1), VERSION);
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

            bdexStreamOut(out, bdlt::Date(1, 1, 1), VERSION);
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

            bdexStreamOut(out, bdlt::Date(1, 1, 1), VERSION);
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

            bdexStreamOut(out, bdlt::Date(1, 1, 1), VERSION);
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

            bdexStreamOut(out, bdlt::Date(1, 1, 1), VERSION);
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
                //----  ------  ---  ---  ------------------------------
                { L_,       -1,   1,   7, "\x00\x00\x01\xff\xff\xff\xff" },
                { L_,        0,   1,   7, "\x00\x00\x01\x00\x00\x00\x00" },
                { L_,        1,   1,   7, "\x00\x00\x01\x00\x00\x00\x01" }
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
                    Obj        mX(bdlt::Date(1, 1, 1), OFFSET);
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
                    Obj        mX(bdlt::Date(1, 1, 1), OFFSET);
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
        //   Date& operator=(const Date& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "COPY-ASSIGNMENT OPERATOR" << endl
                                  << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) cout << "\nTesting Assignment u = V." << endl;
        {
            static const struct {
                int d_line;    // line number
                int d_year;    // year of date
                int d_month;   // month of date
                int d_day;     // day of month of date
                int d_offset;  // timezone offset
            } DATA[] = {
                //LINE   YEAR    MO  DAY  OFFSET
                //----   ----    --  ---  ------
                { L_,      1,     1,   1,      0 },
                { L_,     10,     4,   5,      1 },
                { L_,    100,     6,   7,  -1439 },
                { L_,   1000,     8,   9,   1439 },
                { L_,   2000,     2,  29,     -1 },
                { L_,   2002,     7,   4,   1380 },
                { L_,   2003,     8,   5,  -1380 },
                { L_,   2004,     9,   3,   5*60 },
                { L_,   2020,     9,   9,  -5*60 },
                { L_,   9999,    12,  31,  -5*60 }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int V_LINE   = DATA[i].d_line;
                const int V_YEAR   = DATA[i].d_year;
                const int V_MONTH  = DATA[i].d_month;
                const int V_DAY    = DATA[i].d_day;
                const int V_OFFSET = DATA[i].d_offset;

                if (veryVerbose) {
                    T_ P_(V_LINE) P_(V_YEAR) P_(V_MONTH) P_(V_DAY) P(V_OFFSET)
                }

                const Obj V(bdlt::Date(V_YEAR, V_MONTH, V_DAY), V_OFFSET);

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int U_LINE   = DATA[j].d_line;
                    const int U_YEAR   = DATA[j].d_year;
                    const int U_MONTH  = DATA[j].d_month;
                    const int U_DAY    = DATA[j].d_day;
                    const int U_OFFSET = DATA[j].d_offset;

                    if (veryVerbose) {
                        T_ T_ P_(U_LINE) P_(U_YEAR) P_(U_MONTH) P_(U_DAY);
                        P(U_OFFSET);
                    }


                    Obj mU(bdlt::Date(U_YEAR, U_MONTH, U_DAY), U_OFFSET);
                    const Obj& U = mU;

                    const Obj ZZ(V);

                    if (veryVeryVerbose) { T_ T_ T_ P_(V) P_(U) P(ZZ) }

                    Obj *mR = &(mU = V);

                    if (veryVeryVerbose) { T_ T_ T_ P_(V) P_(U) P(ZZ) }
                    ASSERTV(V_LINE, U_LINE, mR == &U);
                    ASSERTV(V_LINE, U_LINE, ZZ == U);
                    ASSERTV(V_LINE, U_LINE, ZZ == V);
                }
            }

            if (verbose) cout << "Testing self-assignment" << endl;
            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE   = DATA[i].d_line;
                const int YEAR   = DATA[i].d_year;
                const int MONTH  = DATA[i].d_month;
                const int DAY    = DATA[i].d_day;
                const int OFFSET = DATA[i].d_offset;

                if (veryVerbose) { T_ P_(YEAR) P_(MONTH) P_(DAY) P(OFFSET) }

                Obj mU(bdlt::Date(YEAR, MONTH, DAY), OFFSET);
                const Obj& U = mU;

                const Obj ZZ(U);

                if (veryVeryVerbose) { T_ T_ P_(U) P(ZZ) }
                ASSERTV(LINE, ZZ == U);

                Obj *mR = &(mU = U);

                if (veryVeryVerbose) { T_ T_ P_(U) P(ZZ) }
                ASSERTV(LINE, mR == &U);
                ASSERTV(LINE, ZZ == U);
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

        if (verbose) cout << "Not implemented for 'bdlt::DateTz'." << endl;

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
        //   DateTz(const DateTz& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "COPY CONSTRUCTOR" << endl
                                  << "================" << endl;

        if (verbose) cout << "\nTesting copy constructor." << endl;
        {
            static const struct {
                int d_line;    // line number
                int d_year;    // year of date
                int d_month;   // month of date
                int d_day;     // day of month of date
                int d_offset;  // timezone offset
            } DATA[]= {
                //LINE   YEAR  MO  DAY  OFFSET
                //----   ----  --  ---  ------
                { L_,      1,  1,   1,       0 },
                { L_,     10,  4,   5,       1 },
                { L_,    100,  6,   7,   -1439 },
                { L_,   1000,  8,   9,    1439 },
                { L_,   2000,  2,  29,      -1 },
                { L_,   2002,  7,   4,    1380 },
                { L_,   2003,  8,   5,   -1380 },
                { L_,   2004,  9,   3,    5*60 },
                { L_,   9999, 12,  31,   -5*60 }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE   = DATA[i].d_line;
                const int YEAR   = DATA[i].d_year;
                const int MONTH  = DATA[i].d_month;
                const int DAY    = DATA[i].d_day;
                const int OFFSET = DATA[i].d_offset;

                if (veryVerbose) { T_ P_(YEAR) P_(MONTH) P_(DAY) P(OFFSET) }


                const Obj X(bdlt::Date(YEAR, MONTH, DAY), OFFSET);
                const Obj ZZ(bdlt::Date(YEAR, MONTH, DAY), OFFSET);
                const Obj Y(X);

                if (veryVerbose) { T_ T_ P_(X) P_(ZZ) P(Y) }
                ASSERTV(LINE, X == Y);
                ASSERTV(LINE, Y == ZZ);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
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
        //   bool operator==(const DateTz& lhs, const DateTz& rhs);
        //   bool operator!=(const DateTz& lhs, const DateTz& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "EQUALITY OPERATORS" << endl
                                  << "==================" << endl;

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

        if (verbose) cout <<
            "\nCompare each pair of values (u, v) in S X S." << endl;
        {
            static const struct {
                int d_line;    // line number
                int d_year;    // year of date
                int d_month;   // month of date
                int d_day;     // day of month of date
                int d_offset;  // timezone offset
            } DATA[] = {
                //LINE  YEAR  MO  DAY  OFFSET
                //----  ----  --  ---  ------
                { L_,     1,  1,   1,       0 },
                { L_,     1,  1,   2,       0 },
                { L_,     1,  2,   1,       0 },
                { L_,     2,  1,   1,    1439 },
                { L_,     1,  1,   1,      -1 },
                { L_,     1,  1,   2,       1 },
                { L_,     1,  2,   1,   -1439 },
                { L_,     2,  1,   1,    1438 },

                { L_,  9998, 12,  31,       0 },
                { L_,  9999, 11,  30,       0 },
                { L_,  9999, 12,  30,       0 },
                { L_,  9999, 12,  31,       0 },
                { L_,  9998, 12,  31,       1 },
                { L_,  9999, 11,  30,   -1439 },
                { L_,  9999, 12,  30,      -1 },
                { L_,  9999, 12,  31,    -143 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int U_LINE   = DATA[i].d_line;
                const int U_YEAR   = DATA[i].d_year;
                const int U_MONTH  = DATA[i].d_month;
                const int U_DAY    = DATA[i].d_day;
                const int U_OFFSET = DATA[i].d_offset;

                if (veryVerbose) {
                    T_ P(U_LINE) P_(U_YEAR) P_(U_MONTH) P_(U_DAY) P(U_OFFSET);
                }

                const bdlt::Date U_DATE(U_YEAR, U_MONTH, U_DAY);
                const Obj U(U_DATE, U_OFFSET);

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int V_LINE   = DATA[j].d_line;
                    const int V_YEAR   = DATA[j].d_year;
                    const int V_MONTH  = DATA[j].d_month;
                    const int V_DAY    = DATA[j].d_day;
                    const int V_OFFSET = DATA[j].d_offset;

                    if (veryVerbose) {
                        T_ T_ P_(V_LINE) P_(V_YEAR);
                        P_(V_MONTH) P_(V_DAY) P(V_OFFSET);
                    }

                    const bdlt::Date V_DATE(V_YEAR, V_MONTH, V_DAY);
                    const Obj V(V_DATE, V_OFFSET);

                    bool isSame = i == j;

                    if (veryVeryVerbose) { T_ T_ T_ P_(i) P_(j) P_(U) P(V) }
                    ASSERTV(U_LINE, V_LINE,  isSame == (U == V));
                    ASSERTV(U_LINE, V_LINE, !isSame == (U != V));
                    ASSERTV(U_LINE, V_LINE,  isSame == (V == U));
                    ASSERTV(U_LINE, V_LINE, !isSame == (V != U));
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
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 7)
        //:
        //: 2 Using the table-driven technique, define set of distinct
        //:   formatting parameters and the corresponding expected output
        //:   string for the 'print' method. If the value of the formatting
        //:   parameters, 'level' or 'spacesPerLevel' is -8, then the
        //:   parameters will be omitted in the method invocation.
        //:
        //:   1 Invoke the 'print' method passing the formatting parameters and
        //:     a 'const' object.
        //:
        //:   2 Verify the address of what is returned is that of the supplied
        //:     stream.  (C-5)
        //:
        //:   3 Verify that the output string has the expected value.
        //:     (C-1..2, 6)
        //:
        //: 3 Using the table-driven technique, define a set of distinct object
        //:   values and the expected string output of 'operator<<'.
        //:
        //:   1 Invoke 'operator<<' passing a 'const' object.
        //:
        //:   2 Verify the address of what is returned is that of the supplied
        //:     stream.  (C-8)
        //:
        //:   3 Verify that the output string has the expected value.  (C-3)
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream& stream, int l, int spl) const;
        //   bsl::ostream& operator<<(bsl::ostream&, const DateTz&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

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

        static const struct {
            int         d_lineNum;         // source line number
            int         d_level;           // indentation level
            int         d_spacesPerLevel;  // spaces per indentation level
            int         d_offset;          // tz offset
            const char *d_expected_p;      // expected output format
        } DATA[] = {
            //LINE  LEVEL  SPACES  OFFSET  FORMAT
            //----  -----  ------  ------  ------
            { L_,      0,     -1,      0,  "01JAN0001+0000"      },
            { L_,      0,      0,     15,  "01JAN0001+0015\n"    },
            { L_,      0,      2,     60,  "01JAN0001+0100\n"    },
            { L_,     -8,     -8,     60,  "01JAN0001+0100\n"    },
            { L_,      1,      1,     90,  " 01JAN0001+0130\n"   },
            { L_,      1,      2,    -20,  "  01JAN0001-0020\n"  },
            { L_,      1,      0,    -20,  "01JAN0001-0020\n"    },
            { L_,      1,     -1,    -20,  " 01JAN0001-0020"     },
            { L_,     -1,      2,   -330,  "01JAN0001-0530\n"    },
            { L_,     -1,      0,   -330,  "01JAN0001-0530\n"    },
            { L_,     -2,      1,    311,  "01JAN0001+0511\n"    },
            { L_,      2,      1,   1439,  "  01JAN0001+2359\n"  },
            { L_,      1,      3,  -1439,  "   01JAN0001-2359\n" },
            { L_,     -9,     -9,      0,  "01JAN0001+0000"      },
            { L_,     -9,     -9,     15,  "01JAN0001+0015"      },
            { L_,     -9,     -9,     60,  "01JAN0001+0100"      },
            { L_,     -9,     -9,     90,  "01JAN0001+0130"      },
            { L_,     -9,     -9,    -20,  "01JAN0001-0020"      },
            { L_,     -9,     -9,   -330,  "01JAN0001-0530"      },
            { L_,     -9,     -9,    311,  "01JAN0001+0511"      },
            { L_,     -9,     -9,   1439,  "01JAN0001+2359"      },
            { L_,     -9,     -9,  -1439,  "01JAN0001-2359"      },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const int         L    = DATA[ti].d_level;
                const int         SPL  = DATA[ti].d_spacesPerLevel;
                const int         OFF  = DATA[ti].d_offset;
                const char *const EXP  = DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(L) P_(SPL) P_(OFF) }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                bdlt::Date date; // 01JAN0001
                const Obj X(date, OFF);

                ostringstream os;

                // Verify supplied stream is returned by reference.

                if (-9 == L && -9 == SPL) {
                    ASSERTV(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    ASSERTV(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        ASSERTV(LINE, &os == &X.print(os, L, SPL));
                    }
                    else if (-8 != L) {
                        ASSERTV(LINE, &os == &X.print(os, L));
                    }
                    else {
                        ASSERTV(LINE, &os == &X.print(os));
                    }

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                // Verify output is formatted as expected.

                if (veryVeryVerbose) { P(os.str()) }

                ASSERTV(LINE, EXP, os.str(), EXP == os.str());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute of
        //:   the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //
        // Plan:
        //: 1 Use the table-driven technique, specify a range of distinct
        //:   object values. For each row 'R1' in the table:  (C-1..2)
        //:
        //:   1 Default construct an object and set the object's value to that
        //:     of 'R1' using the primary manipulator.
        //:
        //:   2 Invoke each basic accessor from a reference providing
        //:     non-modifiable access to the object created in P-1.1 and verify
        //:     that the return value is correct.  (C-1..2)
        //
        // Testing:
        //   Date localDate() const;
        //   int offset() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BASIC ACCESSORS" << endl
                                  << "===============" << endl;

        if (verbose) cout << "\n'localDate()', 'offset()'" << endl;
        {
            static const struct {
                int d_line;    // line number
                int d_year;    // year of date
                int d_month;   // month of date
                int d_day;     // day of month of date
                int d_offset;  // timezone offset
            } DATA[] = {
                //LINE   YEAR   MO  DAY  OFFSET
                //----   ----   --  ---  ------
                { L_,      1,   1,   1,   -1439 },
                { L_,     10,   4,   5,    1439 },
                { L_,    100,   6,   1,   -5*60 },
                { L_,   1000,   8,   9,   -1438 },
                { L_,   1100,   1,  31,    1380 },
                { L_,   1200,   2,  15,   -1380 },
                { L_,   1300,   3,  31,      -1 },
                { L_,   1400,   4,  30,    5*60 },
                { L_,   1600,   6,  30,       0 },
                { L_,   1500,   5,  15,   -1439 },
                { L_,   1700,   7,  31,    1*60 },
                { L_,   1900,   9,  30,       1 },
                { L_,   2000,  10,  31,       0 },
                { L_,   2200,  12,  31,    1438 },
                { L_,   2400,  12,   1,   -1*60 },
                { L_,   9999,  12,  31,    1439 }
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE   = DATA[i].d_line;
                const int YEAR   = DATA[i].d_year;
                const int MONTH  = DATA[i].d_month;
                const int DAY    = DATA[i].d_day;
                const int OFFSET = DATA[i].d_offset;

                if (veryVerbose) {
                    T_ P_(LINE) P_(YEAR) P_(MONTH) P_(DAY) P(OFFSET);
                }

                const bdlt::Date localDate(YEAR, MONTH, DAY);

                const Obj X(localDate, OFFSET);

                if (veryVeryVerbose) { T_ P_(LINE) P(X) }
                ASSERTV(LINE, localDate, X.localDate(),
                        localDate == X.localDate());
                ASSERTV(LINE, OFFSET, X.offset(),
                        OFFSET == X.offset());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMITIVE GENERATOR FUNCTIONS 'gg' and 'ggg':
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //  Reserved for testing of primitive generator functions.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                    << "PRIMITIVE GENERATOR FUNCTIONS 'gg' and 'ggg':" << endl
                    << "=============================================" << endl;

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATOR - VALUE CONSTRUCTOR
        //
        // Concerns:
        //: 1 The constructor correctly initializes the local date and time
        //:   zone offset to the specified values.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set S of dates and
        //:   offsets as (y, m, d, o) quadruplets having widely varying values.
        //:   For each (y, m, d, o) in S, construct a date object X and verify
        //:   that X has the expected value.  (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-2)
        //
        // Testing:
        //   DateTz(const Date& localDate, int offset);
        //   ~DateTz();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATOR - VALUE CONSTRUCTOR" << endl
                          << "=======================================" << endl;

        {
            static const struct {
                int d_line;    // line number
                int d_year;    // year of date
                int d_month;   // month of date
                int d_day;     // day of month of date
                int d_offset;  // timezone offset
            } DATA[] = {
                //LINE  YEAR  MO DAY  OFFSET
                //----  ----  -- ---  ------
                { L_,     1,  1,  1,       0 },
                { L_,    10,  4,  5,       1 },
                { L_,   100,  6,  7,   -1439 },
                { L_,  1000,  8,  9,    1439 },
                { L_,  2000,  2, 29,      -1 },
                { L_,  2002,  7,  4,    1380 },
                { L_,  2003,  8,  5,   -1380 },
                { L_,  2004,  9,  3,    5*60 },
                { L_,  9999, 12, 31,   -5*60 }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE   = DATA[i].d_line;
                const int YEAR   = DATA[i].d_year;
                const int MONTH  = DATA[i].d_month;
                const int DAY    = DATA[i].d_day;
                const int OFFSET = DATA[i].d_offset;

                if (veryVerbose) {
                    T_ P_(LINE) P_(YEAR) P_(MONTH) P_(DAY) P(OFFSET);
                }

                const bdlt::Date localDate(YEAR, MONTH, DAY);
                const Obj X(localDate, OFFSET);

                if (veryVeryVerbose) { T_ T_ P_(X) }
                ASSERTV(LINE, localDate, X.localDate(),
                        localDate == X.localDate());
                ASSERTV(LINE, OFFSET, X.offset(),
                        OFFSET == X.offset());
            }
        }

        // Negative Testing
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'timeout'" << endl;
            {
                ASSERT_SAFE_FAIL(Obj(bdlt::Date(),  1440));
                ASSERT_SAFE_PASS(Obj(bdlt::Date(),  1439));
                ASSERT_SAFE_FAIL(Obj(bdlt::Date(), -1440));
                ASSERT_SAFE_PASS(Obj(bdlt::Date(), -1439));
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
        //: 1 Execute each methods to verify functionality for simple case.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Values for testing

        const int YRA = 1, MOA = 2, DAA = 3;           // y, m, d for VA
        const int YRB = 4, MOB = 5, DAB = 6;           // y, m, d for VB
        const int YRC = 7, MOC = 8, DAC = 9;           // y, m, d for VC

        const bdlt::Date DA(YRA, MOA, DAA),
                         DB(YRB, MOB, DAB),
                         DC(YRC, MOC, DAC);

        const int OA = 60, OB = -300, OC = 270;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1.  Create an object x1 (init.  to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(DA, OA);  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta.  Check initial state of x1." << endl;
        ASSERT(DA == X1.localDate());
        ASSERT(OA == X1.offset());

        if (verbose) cout <<
            "\tb.  Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2.  Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta.  Check the initial state of x2." << endl;
        ASSERT(DA == X2.localDate());
        ASSERT(OA == X2.offset());

        if (verbose) cout <<
            "\tb.  Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3.  Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;
        mX1.setDateTz(DB, OB);
        if (verbose) { cout << '\t';  P(X1); }
        ASSERT(DB == X1.localDate());
        ASSERT(OB == X1.offset());

        if (verbose) cout << "\ta.  Check new state of x1." << endl;

        if (verbose) cout <<
            "\tb.  Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4.  Create a default object x3()."
                             "\t\t{ x1:VB x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout << "\ta.  Check initial state of x3." << endl;
        ASSERT(bdlt::Date() == X3.localDate());
        ASSERT(           0 == X3.offset());

        if (verbose) cout <<
            "\tb.  Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5.  Create an object x4 (copy from x3)."
                             "\t\t{ x1:VA x2:VA x3:U  x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout << "\ta.  Check initial state of x4." << endl;
        ASSERT(bdlt::Date() == X4.localDate());
        ASSERT(           0 == X4.offset());

        if (verbose) cout <<
            "\tb.  Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6.  Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;
        mX3.setDateTz(DC, OC);
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout << "\ta.  Check new state of x3." << endl;
        ASSERT(DC == X3.localDate());
        ASSERT(OC == X3.offset());

        if (verbose) cout <<
            "\tb.  Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7.  Assign x2 = x1."
                             "\t\t\t\t{ x1:VB x2:VB x3:VC x4:U }" << endl;
        mX2 = X1;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta.  Check new state of x2." << endl;
        ASSERT(DB == X2.localDate());
        ASSERT(OB == X2.offset());

        if (verbose) cout <<
            "\tb.  Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8.  Assign x2 = x3."
                             "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;
        mX2 = X3;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta.  Check new state of x2." << endl;
        ASSERT(DC == X2.localDate());
        ASSERT(OC == X2.offset());

        if (verbose) cout <<
            "\tb.  Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9.  Assign x1 = x1 (aliasing)."
                             "\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;

        mX1 = X1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta.  Check new state of x1." << endl;
        ASSERT(DB == X1.localDate());
        ASSERT(OB == X1.offset());

        if (verbose) cout <<
            "\tb.  Try equality operators: x1 <op> x1, x2, x3, x4." << endl;
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
