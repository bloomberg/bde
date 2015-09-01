// bdlt_serialdateimputil.t.cpp                                       -*-C++-*-
#include <bdlt_serialdateimputil.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_stopwatch.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test consists of a suite of static member functions
// (pure functions) that perform basic validations and conversions on three
// date representations (year/month/day, year/day-of-year, and serial) per the
// proleptic Gregorian calendar.  Several of the methods use cached data to
// improve performance, and these methods must be exhaustively tested to ensure
// that the cache is accurate.
//
// The general plan is that each method is tested against a set of tabulated
// test vectors in all test modes.  In addition, exhaustive loop-based tests
// are performed on most of the methods over a range of years that is
// determined by verbosity level.  The methods exhaustively tested are done so
// over an abridged range of years by default, and over the entire range of
// valid years in 'veryVerbose' mode (see 'yearRangeFlag').  The abridged range
// of years is defined such that the entire cache is exercised (in any test
// mode).
//
// Several negatively-numbered test cases are also provided, one of which
// (test case -1) is used to generate the cache (for insertion into the '.cpp'
// file).  The other negative tests (-2 through -7) compare the running times
// of the 'NoCache' methods versus that of their cached counterparts.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] static bool isLeapYear(int year);
// [ 4] static int  lastDayOfMonth(int year, int month);
// [ 3] static int  numLeapYears(int year1, int year2);
// [ 5] static bool isValidSerial(int serialDay);
// [ 5] static bool isValidYearDay(int year, int dayOfYear);
// [ 5] static bool isValidYearMonthDay(int year, int month, int day);
// [ 5] static bool isValidYearMonthDayNoCache(int y, int m, int day);
// [ 9] static int  ydToSerial(int year, int dayOfYear);
// [ 6] static int  ymdToSerial(int year, int month, int day);
// [ 6] static int  ymdToSerialNoCache(int year, int month, int day);
// [10] static int  serialToDayOfYear(int serialDay);
// [10] static void serialToYd(int *year, int *dayOfYear, int serialDay);
// [ 7] static int  ymdToDayOfYear(int year, int month, int day);
// [11] static int  serialToDay(int serialDay);
// [11] static int  serialToDayNoCache(int serialDay);
// [11] static int  serialToMonth(int serialDay);
// [11] static int  serialToMonthNoCache(int serialDay);
// [10] static int  serialToYear(int serialDay);
// [10] static int  serialToYearNoCache(int serialDay);
// [11] static void serialToYmd(int *y, int *m, int *d, int sD);
// [11] static void serialToYmdNoCache(int *y, int *m, int *d, int sD);
// [ 8] static int  ydToDay(int year, int dayOfYear);
// [ 8] static void ydToMd(int *month, int *day, int year, int dayOfYear);
// [ 8] static int  ydToMonth(int year, int dayOfYear);
// [12] static int  serialToDayOfWeek(int serialDay);
// [12] static int  ydToDayOfWeek(int year, int dayOfYear);
// [12] static int  ymdToDayOfWeek(int year, int month, int day);
// ----------------------------------------------------------------------------
// [13] USAGE EXAMPLE 1
// [14] USAGE EXAMPLE 2
// [ 1] CONCERN: The global constants used for testing are correct.
// [ *] CONCERN: Precondition violations are detected when enabled.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ *] CONCERN: In no case does memory come from the default allocator.
// [-1] CACHE GENERATOR
// [-2] PERFORMANCE TEST: isValidYearMonthDay[NoCache]
// [-3] PERFORMANCE TEST: ymdToSerial[NoCache]
// [-4] PERFORMANCE TEST: serialToYmd[NoCache]
// [-5] PERFORMANCE TEST: serialToYear[NoCache]
// [-6] PERFORMANCE TEST: serialToMonth[NoCache]
// [-7] PERFORMANCE TEST: serialToDay[NoCache]

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

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlt::SerialDateImpUtil Util;

// ============================================================================
//                     GLOBAL CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

enum {
    k_MIN_MONTH                 =       1,
    k_MAX_MONTH                 =      12,

    k_MIN_YEAR                  =       1,
    k_MAX_YEAR                  =    9999,

    k_MIN_SERIAL                =       1,  // 0001/01/01 (earliest valid date)
    k_MAX_SERIAL                = 3652059,  // 9999/12/31 (latest valid date)

    k_MIN_DAYOFWEEK             =       2,  // 0001/01/01 was a Monday.

    k_SHORT_RANGE_MIN_YEAR      =    1699,  // "reasonable" history
    k_SHORT_RANGE_MAX_YEAR      =    2201,  // "reasonable" future

    k_SHORT_RANGE_MIN_SERIAL    =  620183,  // 1699/01/01
    k_SHORT_RANGE_MAX_SERIAL    =  803898,  // 2201/12/31

    k_SHORT_RANGE_MIN_DAYOFWEEK =       5,  // 1699/01/01 was a Thursday.
};

// ============================================================================
//                              USAGE EXAMPLE
// ----------------------------------------------------------------------------

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FABC01

///Example 2: Use to Implement a Value-Semantic Date Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Using the functions supplied in this component, we can easily implement a
// C++ class that represents abstract (*mathematical*) date values and performs
// common operations on them.  The internal representation could be any of the
// three supported by this component.  In this example, we choose to represent
// the date value internally as a "serial date".
//
// First, we define a partial interface of our date class, 'MyDate', omitting
// many methods, free operators, and 'friend' declarations that do not
// contribute substantively to illustrating use of this component:
//..
    class MyDate {
        // This class represents a valid date, in the proleptic Gregorian
        // calendar, in the range '[0001/01/01 .. 9999/12/31]'.

        // DATA
        int d_serialDate;  // 1 = 0001/01/01, 2 = 0001/01/02, etc.

        // FRIENDS
        friend bool operator==(const MyDate&, const MyDate&);
        // ...

      private:
        // PRIVATE CREATORS
        explicit MyDate(int serialDate);
            // Create a 'MyDate' object initialized with the value indicated by
            // the specified 'serialDate'.  The behavior is undefined unless
            // 'serialDate' represents a valid 'MyDate' value.

      public:
        // CLASS METHODS
        static bool isValid(int year, int month, int day);
            // Return 'true' if the specified 'year', 'month', and 'day'
            // represent a valid value for a 'MyDate' object, and 'false'
            // otherwise.

        // CREATORS
        MyDate();
            // Create a 'MyDate' object having the earliest supported valid
            // date value, i.e., "0001/01/01".

        MyDate(int year, int month, int day);
            // Create a 'MyDate' object having the value represented by the
            // specified 'year', 'month', and 'day'.  The behavior is undefined
            // unless 'isValid(year, month, day)' returns 'true'.

        // ...

        // MANIPULATORS

        // ...

        void setYearMonthDay(int year, int month, int day);
            // Set this 'MyDate' object to have the value represented by the
            // specified 'year', 'month', and 'day'.  The behavior is undefined
            // unless 'isValid(year, month, day)' returns 'true'.

        // ACCESSORS
        void getYearMonthDay(int *year, int *month, int *day) const;
            // Load, into the specified 'year', 'month', and 'day', the
            // individual attribute values of this 'MyDate' object.

        int day() const;
            // Return the day of the month in the range '[1 .. 31]' of this
            // 'MyDate' object.

        int month() const;
            // Return the month of the year in the range '[1 .. 12]' of this
            // 'MyDate' object.

        int year() const;
            // Return the year in the range '[1 .. 9999]' of this 'MyDate'
            // object.

        // ...
    };

    // FREE OPERATORS
    bool operator==(const MyDate& lhs, const MyDate& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' 'MyDate' objects have
        // the same value, and 'false' otherwise.  Two dates have the same
        // value if each of the corresponding 'year', 'month', and 'day'
        // attributes respectively have the same value.

    // ...
//..
// Then, we provide an implementation of the 'MyDate' methods and associated
// free operators declared above, using 'bsls_assert' to identify preconditions
// and invariants where appropriate.  Note the use of various
// 'bdlt::SerialDateImpUtil' functions in the code:
//..
    // PRIVATE CREATORS
    inline
    MyDate::MyDate(int serialDate)
    : d_serialDate(serialDate)
    {
        BSLS_ASSERT_SAFE(bdlt::SerialDateImpUtil::isValidSerial(d_serialDate));
    }

    // CLASS METHODS
    inline
    bool MyDate::isValid(int year, int month, int day)
    {
        return bdlt::SerialDateImpUtil::isValidYearMonthDay(year, month, day);
    }

    // CREATORS
    inline
    MyDate::MyDate()
    : d_serialDate(1)
    {
    }

    inline
    MyDate::MyDate(int year, int month, int day)
    : d_serialDate(bdlt::SerialDateImpUtil::ymdToSerial(year, month, day))
    {
        BSLS_ASSERT_SAFE(isValid(year, month, day));
    }

    // ...

    // MANIPULATORS

    // ...

    inline
    void MyDate::setYearMonthDay(int year, int month, int day)
    {
        BSLS_ASSERT_SAFE(isValid(year, month, day));

        d_serialDate = bdlt::SerialDateImpUtil::ymdToSerial(year, month, day);
    }

    // ACCESSORS
    inline
    void MyDate::getYearMonthDay(int *year, int *month, int *day) const
    {
        BSLS_ASSERT_SAFE(year);
        BSLS_ASSERT_SAFE(month);
        BSLS_ASSERT_SAFE(day);

        bdlt::SerialDateImpUtil::serialToYmd(year, month, day, d_serialDate);
    }

    inline
    int MyDate::day() const
    {
        return bdlt::SerialDateImpUtil::serialToDay(d_serialDate);
    }

    inline
    int MyDate::month() const
    {
        return bdlt::SerialDateImpUtil::serialToMonth(d_serialDate);
    }

    inline
    int MyDate::year() const
    {
        return bdlt::SerialDateImpUtil::serialToYear(d_serialDate);
    }

    // FREE OPERATORS
    inline
    bool operator==(const MyDate& lhs, const MyDate& rhs)
    {
        return lhs.d_serialDate == rhs.d_serialDate;
    }
//..

// BDE_VERIFY pragma: pop

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    // 'yearRangeFlag' determines the range of years over which exhaustive
    // testing is performed.  By default, that range is abridged (see 'enum'
    // above).  In 'veryVerbose' mode, the range includes '[1 .. 9999]'.  In
    // any case, the entire cache is always exercised.

    bool yearRangeFlag = !(argc > 3);

    switch (test) { case 0:
      case 14: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 2
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
        //   USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE 2" << endl
                          << "===============" << endl;

// Next, we illustrate basic use of our 'MyDate' class, starting with the
// creation of a default object, 'd1':
//..
    MyDate d1;                        ASSERT(   1 == d1.year());
                                      ASSERT(   1 == d1.month());
                                      ASSERT(   1 == d1.day());
//..
// Now, we set 'd1' to July 4, 1776 via the 'setYearMonthDay' method, but we
// first verify that it is a valid date using 'isValid':
//..
                                      ASSERT(MyDate::isValid(1776, 7, 4));
    d1.setYearMonthDay(1776, 7, 4);   ASSERT(1776 == d1.year());
                                      ASSERT(   7 == d1.month());
                                      ASSERT(   4 == d1.day());
//..
// Finally, using the value constructor, we create 'd2' to have the same value
// as 'd1':
//..
    MyDate d2(1776, 7, 4);            ASSERT(1776 == d2.year());
                                      ASSERT(   7 == d2.month());
                                      ASSERT(   4 == d2.day());
                                      ASSERT(  d1 == d2);
//..
// Note that equality comparison of 'MyDate' objects is very efficient, being
// comprised of a comparison of two 'int' values.  Similarly, the 'MyDate'
// methods and free operators (not shown) that add a (signed) number of days
// to a date are also very efficient.  However, one of the trade-offs of
// storing a date internally as a serial value is that operations involving
// conversion among the serial value and one or more of the 'year', 'month',
// and 'day' attributes (e.g., 'setYearMonthDay', 'getYearMonthDay') entail
// considerably more computation.

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1
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
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE 1" << endl
                          << "===============" << endl;

///Example 1: Use as a General Purpose Utility
///- - - - - - - - - - - - - - - - - - - - - -
// The primary purpose of this component is to support the implementation of a
// general-purpose, value-semantic (vocabulary) "Date" type.  However, it also
// provides many low-level utility functions suitable for direct use by other
// clients.  In this example we employ several of the functions from this
// component to ask questions about particular dates in one of the three
// supported formats.
//
// First, what day of the week was January 3, 2010?
//..
    ASSERT(2 == bdlt::SerialDateImpUtil::ymdToDayOfWeek(2010, 3, 1));
                                                             // 2 means Monday.
//..
// Then, was the year 2000 a leap year?
//..
    ASSERT(true == bdlt::SerialDateImpUtil::isLeapYear(2000));
                                                             // Yes, it was.
//..
// Next, was February 29, 1900 a valid date in history?
//..
    ASSERT(false == bdlt::SerialDateImpUtil::isValidYearMonthDay(1900, 2, 29));
                                                             // No, it was not.
//..
// Then, what was the last day of February in 1600?
//..
    ASSERT(29 == bdlt::SerialDateImpUtil::lastDayOfMonth(1600, 2));
                                                             // The 29th.
//..
// Next, how many leap years occurred from 1959 to 2012, inclusive?
//..
    ASSERT(14 == bdlt::SerialDateImpUtil::numLeapYears(1959, 2012));
                                                             // There were 14.
//..
// Now, on what day of the year will February 29, 2020 fall?
//..
    ASSERT(60 == bdlt::SerialDateImpUtil::ymdToDayOfYear(2020, 2, 29));
                                                             // The 60th one.
//..
// Finally, in what month did the 120th day of 2011 fall?
//..
    ASSERT(4 == bdlt::SerialDateImpUtil::ydToMonth(2011, 120));
                                                             // 4 means April.
//..

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING '{serial|yd|ymd}ToDayOfWeek'
        //   Ensure that the methods correctly map every date, in any of the
        //   three supported formats, to its corresponding day of the week.
        //
        // Concerns:
        //: 1 The 'serialToDayOfWeek' method correctly maps every valid serial
        //:   date to the corresponding day of the week.
        //:
        //: 2 The 'ydToDayOfWeek' method correctly maps every valid
        //:   year/day-of-year date to the corresponding day of the week.
        //:
        //: 3 The 'ymdToDayOfWeek' method correctly maps every valid
        //:   year/month/day date to the corresponding day of the week.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of (unique) valid
        //:   year/month/day dates (one per row), and the (integral) day (of
        //:   the week) results expected from 'ymdToDayOfWeek' when applied to
        //:   those tabulated dates.
        //:
        //: 2 Use the table described in P-1, and the (previously tested)
        //:   'ymdToDayOfYear' and 'ymdToSerial' methods, to verify,
        //:   respectively, that the results from 'ydToDayOfWeek' and
        //:   'serialToDayOfWeek' are as expected.
        //:
        //: 3 For further assurance, exhaustively test the three methods over a
        //:   range of years that is governed by the 'yearRangeFlag'.  (C-1..3)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   static int  serialToDayOfWeek(int serialDay);
        //   static int  ydToDayOfWeek(int year, int dayOfYear);
        //   static int  ymdToDayOfWeek(int year, int month, int day);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING '{serial|yd|ymd}ToDayOfWeek'" << endl
                          << "====================================" << endl;

        enum { SUN = 1, MON, TUE, WED, THU, FRI, SAT };

        {
            static const struct {
                int d_line;   // source line number
                int d_year;   // year under test
                int d_month;  // month under test
                int d_day;    // day under test
                int d_exp;    // expected value
            } DATA[] = {
                //LINE   YEAR   MONTH   DAY   EXPECTED
                //----   ----   -----   ---   --------
                { L_,       1,      1,    1,       MON },
                { L_,       1,      1,    2,       TUE },
                { L_,       1,      1,    3,       WED },
                { L_,       1,      1,    4,       THU },
                { L_,       1,      1,    5,       FRI },
                { L_,       1,      1,    6,       SAT },
                { L_,       1,      1,    7,       SUN },
                { L_,       1,      1,    8,       MON },

                { L_,       1,      2,    1,       THU },
                { L_,       2,      1,    1,       TUE },

                { L_,    1600,     12,   31,       SUN },

                { L_,    1752,      9,    2,       SAT },
                { L_,    1752,      9,    3,       SUN },
                { L_,    1752,      9,   13,       WED },
                { L_,    1752,      9,   14,       THU },

                { L_,    1999,     12,   31,       FRI },

                { L_,    2000,      1,    1,       SAT },
                { L_,    2000,      2,   28,       MON },
                { L_,    2000,      2,   29,       TUE },

                { L_,    9999,     12,   31,       FRI },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_line;
                const int YEAR  = DATA[ti].d_year;
                const int MONTH = DATA[ti].d_month;
                const int DAY   = DATA[ti].d_day;
                const int EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(LINE);  P_(YEAR);  P_(MONTH);  P_(DAY);  P(EXP);
                }

                LOOP_ASSERT(LINE,
                            EXP == Util::ymdToDayOfWeek(YEAR, MONTH, DAY));

                const int doy = Util::ymdToDayOfYear(YEAR, MONTH, DAY);

                LOOP_ASSERT(LINE, EXP == Util::ydToDayOfWeek(YEAR, doy));

                const int serial = Util::ymdToSerial(YEAR, MONTH, DAY);

                LOOP_ASSERT(LINE, EXP == Util::serialToDayOfWeek(serial));
            }
        }

        const int first = yearRangeFlag ? k_SHORT_RANGE_MIN_SERIAL
                                        : k_MIN_SERIAL;
        const int last  = yearRangeFlag ? k_SHORT_RANGE_MAX_SERIAL
                                        : k_MAX_SERIAL;

        int dow = yearRangeFlag ? k_SHORT_RANGE_MIN_DAYOFWEEK
                                : k_MIN_DAYOFWEEK;

        if (verbose) cout << "\tExhaustively testing serial years "
                          << first << '-' << last << '.' << endl;

        for (int s = first; s <= last; ++s) {

            int y = -1, m = -1, d = -1;
            Util::serialToYmd(&y, &m, &d, s);

            int doy = Util::serialToDayOfYear(s);

            if (veryVerbose && 0 == (s - 1) % 100) {
                P_(s);  P_(y);  P_(m);  P_(d);  P(doy);
            }

            LOOP2_ASSERT(s,       dow, dow == Util::serialToDayOfWeek(s));
            LOOP4_ASSERT(y, m, d, dow, dow == Util::ymdToDayOfWeek(y, m, d));
            LOOP3_ASSERT(y, doy,  dow, dow == Util::ydToDayOfWeek(y, doy));

            if (8 == ++dow) {
                dow = 1;
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (verbose) cout << "\t'serialToDayOfWeek'" << endl;
            {
                ASSERT_SAFE_FAIL(Util::serialToDayOfWeek(k_MIN_SERIAL - 1));
                ASSERT_SAFE_PASS(Util::serialToDayOfWeek(k_MIN_SERIAL    ));

                ASSERT_SAFE_PASS(Util::serialToDayOfWeek(k_MAX_SERIAL    ));
                ASSERT_SAFE_FAIL(Util::serialToDayOfWeek(k_MAX_SERIAL + 1));
            }

            if (verbose) cout << "\t'ydToDayOfWeek'" << endl;
            {
                ASSERT_SAFE_FAIL(Util::ydToDayOfWeek(k_MIN_YEAR - 1, 23));
                ASSERT_SAFE_PASS(Util::ydToDayOfWeek(k_MIN_YEAR    , 23));

                ASSERT_SAFE_PASS(Util::ydToDayOfWeek(k_MAX_YEAR    , 23));
                ASSERT_SAFE_FAIL(Util::ydToDayOfWeek(k_MAX_YEAR + 1, 23));

                ASSERT_SAFE_FAIL(Util::ydToDayOfWeek(1812,   0));
                ASSERT_SAFE_PASS(Util::ydToDayOfWeek(1812,   1));

                ASSERT_SAFE_PASS(Util::ydToDayOfWeek(1812, 366));
                ASSERT_SAFE_FAIL(Util::ydToDayOfWeek(1812, 367));

                ASSERT_SAFE_PASS(Util::ydToDayOfWeek(1813, 365));
                ASSERT_SAFE_FAIL(Util::ydToDayOfWeek(1813, 366));
            }

            if (verbose) cout << "\t'ymdToDayOfWeek'" << endl;
            {
                ASSERT_SAFE_FAIL(Util::ymdToDayOfWeek(k_MIN_YEAR - 1, 9, 23));
                ASSERT_SAFE_PASS(Util::ymdToDayOfWeek(k_MIN_YEAR    , 9, 23));

                ASSERT_SAFE_PASS(Util::ymdToDayOfWeek(k_MAX_YEAR    , 9, 23));
                ASSERT_SAFE_FAIL(Util::ymdToDayOfWeek(k_MAX_YEAR + 1, 9, 23));

                ASSERT_SAFE_FAIL(Util::ymdToDayOfWeek(1812, k_MIN_MONTH - 1,
                                                                         23));
                ASSERT_SAFE_PASS(Util::ymdToDayOfWeek(1812, k_MIN_MONTH    ,
                                                                         23));

                ASSERT_SAFE_PASS(Util::ymdToDayOfWeek(1812, k_MAX_MONTH    ,
                                                                         23));
                ASSERT_SAFE_FAIL(Util::ymdToDayOfWeek(1812, k_MAX_MONTH + 1,
                                                                         23));

                ASSERT_SAFE_FAIL(Util::ymdToDayOfWeek(1812, 9,  0));
                ASSERT_SAFE_PASS(Util::ymdToDayOfWeek(1812, 9,  1));

                ASSERT_SAFE_PASS(Util::ymdToDayOfWeek(1812, 9, 30));
                ASSERT_SAFE_FAIL(Util::ymdToDayOfWeek(1812, 9, 31));

                ASSERT_SAFE_PASS(Util::ymdToDayOfWeek(1812, 2, 29));
                ASSERT_SAFE_FAIL(Util::ymdToDayOfWeek(1812, 2, 30));

                ASSERT_SAFE_PASS(Util::ymdToDayOfWeek(1813, 2, 28));
                ASSERT_SAFE_FAIL(Util::ymdToDayOfWeek(1813, 2, 29));
            }
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'serialTo{Day|Month|Ymd}[NoCache]'
        //   Ensure that the methods correctly map every serial date to the
        //   corresponding year, month, and day (of the month).
        //
        // Concerns:
        //: 1 The 'serialToYmd[NoCache]' methods correctly map every valid
        //:   serial date to its corresponding year/month/day date.
        //:
        //: 2 The 'serialToMonth[NoCache]' methods correctly map every valid
        //:   serial date to the corresponding month.
        //:
        //: 3 The 'serialToDay[NoCache]' methods correctly map every valid
        //:   serial date to the corresponding day (of the month).
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of (unique) valid
        //:   serial dates (one per row), and the (integral) year, month, and
        //:   day (of the month) results expected from the four methods when
        //:   applied to those tabulated dates.  For further assurance,
        //:   exhaustively test the methods over a range of years that is
        //:   governed by the 'yearRangeFlag'.  (C-1..3)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   static int  serialToDay(int serialDay);
        //   static int  serialToDayNoCache(int serialDay);
        //   static int  serialToMonth(int serialDay);
        //   static int  serialToMonthNoCache(int serialDay);
        //   static void serialToYmd(int *y, int *m, int *d, int sD);
        //   static void serialToYmdNoCache(int *y, int *m, int *d, int sD);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'serialTo{Day|Month|Ymd}[NoCache]'"
                          << endl
                          << "=========================================="
                          << endl;

        if (verbose) cout << "\tDirect test vectors." << endl;
        {
            static const struct {
                int d_line;      // source line number
                int d_serial;    // serial date under test
                int d_expYear;   // expected year value
                int d_expMonth;  // expected month value
                int d_expDay;    // expected day value
            } DATA[] = {
                //                    <--- EXPECTED --->
                //LINE      SERIAL    YEAR   MONTH   DAY
                //----      ------    ----   -----   ---
                { L_,            1,      1,      1,    1 },
                { L_,            2,      1,      1,    2 },
                { L_,           32,      1,      2,    1 },
                { L_,          365,      1,     12,   31 },
                { L_,          366,      2,      1,    1 },
                { L_,          730,      2,     12,   31 },
                { L_,          731,      3,      1,    1 },
                { L_,         1095,      3,     12,   31 },

                { L_,         1096,      4,      1,    1 },
                { L_,         1460,      4,     12,   30 },
                { L_,         1461,      4,     12,   31 },
                { L_,         1462,      5,      1,    1 },

                { L_,        36160,    100,      1,    1 },
                { L_,        36523,    100,     12,   30 },
                { L_,        36524,    100,     12,   31 },
                { L_,        36525,    101,      1,    1 },

                { L_,       145732,    400,      1,    1 },
                { L_,       146096,    400,     12,   30 },
                { L_,       146097,    400,     12,   31 },
                { L_,       146098,    401,      1,    1 },

                { L_,       147193,    404,      1,    1 },
                { L_,       147557,    404,     12,   30 },
                { L_,       147558,    404,     12,   31 },
                { L_,       147559,    405,      1,    1 },

                { L_,       182257,    500,      1,    1 },
                { L_,       182620,    500,     12,   30 },
                { L_,       182621,    500,     12,   31 },
                { L_,       182622,    501,      1,    1 },

                { L_,       291829,    800,      1,    1 },
                { L_,       292193,    800,     12,   30 },
                { L_,       292194,    800,     12,   31 },
                { L_,       292195,    801,      1,    1 },

                { L_,       657071,   1799,     12,   31 },
                { L_,       657072,   1800,      1,    1 },
                { L_,       657436,   1800,     12,   31 },
                { L_,       657437,   1801,      1,    1 },
                { L_,       657801,   1801,     12,   31 },
                { L_,       657802,   1802,      1,    1 },

                { L_,       693595,   1899,     12,   31 },
                { L_,       693596,   1900,      1,    1 },
                { L_,       693960,   1900,     12,   31 },
                { L_,       693961,   1901,      1,    1 },
                { L_,       694325,   1901,     12,   31 },
                { L_,       694326,   1902,      1,    1 },

                { L_,       730119,   1999,     12,   31 },
                { L_,       730120,   2000,      1,    1 },
                { L_,       730485,   2000,     12,   31 },
                { L_,       730486,   2001,      1,    1 },
                { L_,       730850,   2001,     12,   31 },
                { L_,       730851,   2002,      1,    1 },

                { L_,       766644,   2099,     12,   31 },
                { L_,       766645,   2100,      1,    1 },
                { L_,       767009,   2100,     12,   31 },
                { L_,       767010,   2101,      1,    1 },
                { L_,       767374,   2101,     12,   31 },
                { L_,       767375,   2102,      1,    1 },

                { L_,       876216,   2399,     12,   31 },
                { L_,       876217,   2400,      1,    1 },
                { L_,       876582,   2400,     12,   31 },
                { L_,       876583,   2401,      1,    1 },
                { L_,       876947,   2401,     12,   31 },
                { L_,       876948,   2402,      1,    1 },

                { L_,      3650233,   9994,     12,   31 },
                { L_,      3650234,   9995,      1,    1 },
                { L_,      3650598,   9995,     12,   31 },
                { L_,      3650599,   9996,      1,    1 },
                { L_,      3650964,   9996,     12,   31 },
                { L_,      3650965,   9997,      1,    1 },
                { L_,      3651329,   9997,     12,   31 },
                { L_,      3651330,   9998,      1,    1 },
                { L_,      3651694,   9998,     12,   31 },
                { L_,      3651695,   9999,      1,    1 },
                { L_, k_MAX_SERIAL,   9999,     12,   31 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE     = DATA[ti].d_line;
                const int SERIAL   = DATA[ti].d_serial;
                const int EXPYEAR  = DATA[ti].d_expYear;
                const int EXPMONTH = DATA[ti].d_expMonth;
                const int EXPDAY   = DATA[ti].d_expDay;

                int yy = -1, mm = -1, dd = -1;
                Util::serialToYmd(&yy, &mm, &dd, SERIAL);

                if (veryVerbose) {
                    cout << setw(15) << SERIAL << ": "
                         << setw( 4) << yy     << " / "
                         << setw( 2) << mm     << " / "
                         << setw( 2) << dd     << endl;
                }

                LOOP_ASSERT(LINE, EXPYEAR  == yy);
                LOOP_ASSERT(LINE, EXPMONTH == mm);
                LOOP_ASSERT(LINE, EXPDAY   == dd);

                LOOP_ASSERT(LINE, EXPMONTH == Util::serialToMonth(SERIAL));
                LOOP_ASSERT(LINE, EXPDAY   == Util::serialToDay(SERIAL));

                yy = mm = dd = -1;
                Util::serialToYmdNoCache(&yy, &mm, &dd, SERIAL);

                if (veryVerbose) {
                    cout << setw(15) << SERIAL << ": "
                         << setw( 4) << yy     << " / "
                         << setw( 2) << mm     << " / "
                         << setw( 2) << dd     << endl;
                }

                LOOP_ASSERT(LINE, EXPYEAR  == yy);
                LOOP_ASSERT(LINE, EXPMONTH == mm);
                LOOP_ASSERT(LINE, EXPDAY   == dd);

                LOOP_ASSERT(LINE,
                            EXPMONTH == Util::serialToMonthNoCache(SERIAL));
                LOOP_ASSERT(LINE,
                            EXPDAY   == Util::serialToDayNoCache(SERIAL));
            }
        }

        const int first = yearRangeFlag ? k_SHORT_RANGE_MIN_YEAR : k_MIN_YEAR;
        const int last  = yearRangeFlag ? k_SHORT_RANGE_MAX_YEAR : k_MAX_YEAR;

        if (verbose) cout << "\tExhaustively testing years "
                          << first << '-' << last << '.' << endl;

        int serial =
                 (yearRangeFlag ? k_SHORT_RANGE_MIN_SERIAL : k_MIN_SERIAL) - 1;

        for (int y = first; y <= last; ++y) {
            for (int m = 1; m <= 12; ++m) {
                for (int d = 1; d <= 31; ++d) {
                    if (Util::isValidYearMonthDay(y, m, d)) {
                        ++serial;

                        if (veryVerbose && 0 == (serial - 1) % 100) {
                            P_(serial);  P_(y);  P_(m);  P(d);
                        }

                        int yy = -1, mm = -1, dd = -1;
                        Util::serialToYmd(&yy, &mm, &dd, serial);

                        LOOP2_ASSERT(y, yy, y == yy);
                        LOOP2_ASSERT(m, mm, m == mm);
                        LOOP2_ASSERT(d, dd, d == dd);

                        ASSERT(m == Util::serialToMonth(serial));
                        ASSERT(d == Util::serialToDay(serial));

                        yy = mm = dd = -1;
                        Util::serialToYmdNoCache(&yy, &mm, &dd, serial);

                        LOOP2_ASSERT(y, yy, y == yy);
                        LOOP2_ASSERT(m, mm, m == mm);
                        LOOP2_ASSERT(d, dd, d == dd);

                        ASSERT(m == Util::serialToMonthNoCache(serial));
                        ASSERT(d == Util::serialToDayNoCache(serial));
                    }
                }
            }
        }
        ASSERT(serial ==
                    (yearRangeFlag ? k_SHORT_RANGE_MAX_SERIAL : k_MAX_SERIAL));

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (verbose) cout << "\t'serialToYmd'" << endl;
            {
                int year, month, day;

                ASSERT_SAFE_FAIL(Util::serialToYmd(
                                           0, &month, &day, 1812));
                ASSERT_SAFE_PASS(Util::serialToYmd(
                                       &year, &month, &day, 1812));

                ASSERT_SAFE_FAIL(Util::serialToYmd(
                                       &year,      0, &day, 1812));
                ASSERT_SAFE_PASS(Util::serialToYmd(
                                       &year, &month, &day, 1812));

                ASSERT_SAFE_FAIL(Util::serialToYmd(
                                       &year, &month,    0, 1812));
                ASSERT_SAFE_PASS(Util::serialToYmd(
                                       &year, &month, &day, 1812));

                ASSERT_SAFE_FAIL(Util::serialToYmd(
                                       &year, &month, &day, k_MIN_SERIAL - 1));
                ASSERT_SAFE_PASS(Util::serialToYmd(
                                       &year, &month, &day, k_MIN_SERIAL    ));

                ASSERT_SAFE_PASS(Util::serialToYmd(
                                       &year, &month, &day, k_MAX_SERIAL    ));
                ASSERT_SAFE_FAIL(Util::serialToYmd(
                                       &year, &month, &day, k_MAX_SERIAL + 1));
            }

            if (verbose) cout << "\t'serialToYmdNoCache'" << endl;
            {
                int year, month, day;

                ASSERT_SAFE_FAIL(Util::serialToYmdNoCache(
                                                       0, &month, &day, 1812));
                ASSERT_SAFE_PASS(Util::serialToYmdNoCache(
                                                   &year, &month, &day, 1812));

                ASSERT_SAFE_FAIL(Util::serialToYmdNoCache(
                                                   &year,      0, &day, 1812));
                ASSERT_SAFE_PASS(Util::serialToYmdNoCache(
                                                   &year, &month, &day, 1812));

                ASSERT_SAFE_FAIL(Util::serialToYmdNoCache(
                                                   &year, &month,    0, 1812));
                ASSERT_SAFE_PASS(Util::serialToYmdNoCache(
                                                   &year, &month, &day, 1812));

                ASSERT_SAFE_FAIL(Util::serialToYmdNoCache(
                                       &year, &month, &day, k_MIN_SERIAL - 1));
                ASSERT_SAFE_PASS(Util::serialToYmdNoCache(
                                       &year, &month, &day, k_MIN_SERIAL    ));

                ASSERT_SAFE_PASS(Util::serialToYmdNoCache(
                                       &year, &month, &day, k_MAX_SERIAL    ));
                ASSERT_SAFE_FAIL(Util::serialToYmdNoCache(
                                       &year, &month, &day, k_MAX_SERIAL + 1));
            }

            if (verbose) cout << "\t'serialToMonth'" << endl;
            {
                ASSERT_SAFE_FAIL(Util::serialToMonth(k_MIN_SERIAL - 1));
                ASSERT_SAFE_PASS(Util::serialToMonth(k_MIN_SERIAL    ));

                ASSERT_SAFE_PASS(Util::serialToMonth(k_MAX_SERIAL    ));
                ASSERT_SAFE_FAIL(Util::serialToMonth(k_MAX_SERIAL + 1));
            }

            if (verbose) cout << "\t'serialToMonthNoCache'" << endl;
            {
                ASSERT_SAFE_FAIL(Util::serialToMonthNoCache(k_MIN_SERIAL - 1));
                ASSERT_SAFE_PASS(Util::serialToMonthNoCache(k_MIN_SERIAL    ));

                ASSERT_SAFE_PASS(Util::serialToMonthNoCache(k_MAX_SERIAL    ));
                ASSERT_SAFE_FAIL(Util::serialToMonthNoCache(k_MAX_SERIAL + 1));
            }

            if (verbose) cout << "\t'serialToDay'" << endl;
            {
                ASSERT_SAFE_FAIL(Util::serialToDay(k_MIN_SERIAL - 1));
                ASSERT_SAFE_PASS(Util::serialToDay(k_MIN_SERIAL    ));

                ASSERT_SAFE_PASS(Util::serialToDay(k_MAX_SERIAL    ));
                ASSERT_SAFE_FAIL(Util::serialToDay(k_MAX_SERIAL + 1));
            }

            if (verbose) cout << "\t'serialToDayNoCache'" << endl;
            {
                ASSERT_SAFE_FAIL(Util::serialToDayNoCache(k_MIN_SERIAL - 1));
                ASSERT_SAFE_PASS(Util::serialToDayNoCache(k_MIN_SERIAL    ));

                ASSERT_SAFE_PASS(Util::serialToDayNoCache(k_MAX_SERIAL    ));
                ASSERT_SAFE_FAIL(Util::serialToDayNoCache(k_MAX_SERIAL + 1));
            }
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'serialTo{DayOfYear|Yd|Year[NoCache]}'
        //   Ensure that the methods correctly map every serial date to the
        //   corresponding year and day (of the year).
        //
        // Concerns:
        //: 1 The 'serialToYd' method correctly maps every valid serial date to
        //:   the corresponding year and day of year.
        //:
        //: 2 The 'serialToDayOfYear' method correctly maps every valid serial
        //:   date to the corresponding day of year.
        //:
        //: 3 The 'serialToYear[NoCache]' methods correctly map every valid
        //:   serial date to the corresponding year.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of (unique) valid
        //:   serial dates (one per row), and the (integral) year and day of
        //:   year results expected from the three methods when applied to
        //:   those tabulated dates.  For further assurance, exhaustively test
        //:   the methods over a range of years that is governed by the
        //:   'yearRangeFlag'.  (C-1..3)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   static int  serialToDayOfYear(int serialDay);
        //   static void serialToYd(int *year, int *dayOfYear, int serialDay);
        //   static int  serialToYear(int serialDay);
        //   static int  serialToYearNoCache(int serialDay);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'serialTo{DayOfYear|Yd|Year[NoCache]}'"
                          << endl
                          << "=============================================="
                          << endl;

        if (verbose) cout << "\tDirect test vectors." << endl;
        {
            static const struct {
                int d_line;          // source line number
                int d_serial;        // serial date under test
                int d_expYear;       // expected year value
                int d_expDayOfYear;  // expected day of year value
            } DATA[] = {
                //                     <--- EXPECTED--->
                //LINE       SERIAL    YEAR  DAY OF YEAR
                //----       ------    ----  -----------
                { L_,             1,      1,           1 },
                { L_,             2,      1,           2 },
                { L_,            32,      1,          32 },
                { L_,           365,      1,         365 },
                { L_,           366,      2,           1 },
                { L_,           730,      2,         365 },
                { L_,           731,      3,           1 },
                { L_,          1095,      3,         365 },

                { L_,          1096,      4,           1 },
                { L_,          1460,      4,         365 },
                { L_,          1461,      4,         366 },
                { L_,          1462,      5,           1 },

                { L_,         36160,    100,           1 },
                { L_,         36523,    100,         364 },
                { L_,         36524,    100,         365 },
                { L_,         36525,    101,           1 },

                { L_,        145732,    400,           1 },
                { L_,        146096,    400,         365 },
                { L_,        146097,    400,         366 },
                { L_,        146098,    401,           1 },

                { L_,        147193,    404,           1 },
                { L_,        147557,    404,         365 },
                { L_,        147558,    404,         366 },
                { L_,        147559,    405,           1 },

                { L_,        182257,    500,           1 },
                { L_,        182620,    500,         364 },
                { L_,        182621,    500,         365 },
                { L_,        182622,    501,           1 },

                { L_,        291829,    800,           1 },
                { L_,        292193,    800,         365 },
                { L_,        292194,    800,         366 },
                { L_,        292195,    801,           1 },

                { L_,        639174,   1750,         365 },
                { L_,        639175,   1751,           1 },
                { L_,        639539,   1751,         365 },
                { L_,        639540,   1752,           1 },
                { L_,        639905,   1752,         366 },
                { L_,        639906,   1753,           1 },
                { L_,        640270,   1753,         365 },
                { L_,        640271,   1754,           1 },
                { L_,        640635,   1754,         365 },
                { L_,        640636,   1755,           1 },
                { L_,        641000,   1755,         365 },
                { L_,        641001,   1756,           1 },
                { L_,        641366,   1756,         366 },
                { L_,        641367,   1757,           1 },

                { L_,        657071,   1799,         365 },
                { L_,        657072,   1800,           1 },
                { L_,        657436,   1800,         365 },
                { L_,        657437,   1801,           1 },
                { L_,        657801,   1801,         365 },
                { L_,        657802,   1802,           1 },

                { L_,        693595,   1899,         365 },
                { L_,        693596,   1900,           1 },
                { L_,        693960,   1900,         365 },
                { L_,        693961,   1901,           1 },
                { L_,        694325,   1901,         365 },
                { L_,        694326,   1902,           1 },

                { L_,        730119,   1999,         365 },
                { L_,        730120,   2000,           1 },
                { L_,        730485,   2000,         366 },
                { L_,        730486,   2001,           1 },
                { L_,        730850,   2001,         365 },
                { L_,        730851,   2002,           1 },

                { L_,        766644,   2099,         365 },
                { L_,        766645,   2100,           1 },
                { L_,        767009,   2100,         365 },
                { L_,        767010,   2101,           1 },
                { L_,        767374,   2101,         365 },
                { L_,        767375,   2102,           1 },

                { L_,        876216,   2399,         365 },
                { L_,        876217,   2400,           1 },
                { L_,        876582,   2400,         366 },
                { L_,        876583,   2401,           1 },
                { L_,        876947,   2401,         365 },
                { L_,        876948,   2402,           1 },

                { L_,       3650233,   9994,         365 },
                { L_,       3650234,   9995,           1 },
                { L_,       3650598,   9995,         365 },
                { L_,       3650599,   9996,           1 },
                { L_,       3650964,   9996,         366 },
                { L_,       3650965,   9997,           1 },
                { L_,       3651329,   9997,         365 },
                { L_,       3651330,   9998,           1 },
                { L_,       3651694,   9998,         365 },
                { L_,       3651695,   9999,           1 },
                { L_,  k_MAX_SERIAL,   9999,         365 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE         = DATA[ti].d_line;
                const int SERIAL       = DATA[ti].d_serial;
                const int EXPYEAR      = DATA[ti].d_expYear;
                const int EXPDAYOFYEAR = DATA[ti].d_expDayOfYear;

                int yy = -1, dd = -1;
                Util::serialToYd(&yy, &dd, SERIAL);

                if (veryVerbose) {
                    cout << setw(15) << SERIAL << ": "
                         << setw( 4) << yy     << " / "
                         << setw( 3) << dd     << endl;
                }

                LOOP3_ASSERT(LINE, EXPYEAR,      yy, EXPYEAR      == yy);
                LOOP3_ASSERT(LINE, EXPDAYOFYEAR, dd, EXPDAYOFYEAR == dd);

                yy = Util::serialToYear(SERIAL);
                dd = Util::serialToDayOfYear(SERIAL);

                LOOP3_ASSERT(LINE, EXPYEAR,      yy, EXPYEAR      == yy);
                LOOP3_ASSERT(LINE, EXPDAYOFYEAR, dd, EXPDAYOFYEAR == dd);

                LOOP3_ASSERT(LINE, EXPYEAR, yy,
                             yy == Util::serialToYearNoCache(SERIAL));
            }
        }

        const int first = yearRangeFlag ? k_SHORT_RANGE_MIN_YEAR : k_MIN_YEAR;
        const int last  = yearRangeFlag ? k_SHORT_RANGE_MAX_YEAR : k_MAX_YEAR;

        if (verbose) cout << "\tExhaustively testing years "
                          << first << '-' << last << '.' << endl;

        int serial =
                 (yearRangeFlag ? k_SHORT_RANGE_MIN_SERIAL : k_MIN_SERIAL) - 1;

        for (int y = first; y <= last; ++y) {
            for (int doy = 1; doy <= 366; ++doy) {
                if (Util::isValidYearDay(y, doy)) {
                    ++serial;

                    if (veryVerbose && 0 == (serial - 1) % 100) {
                        P_(serial);  P_(y);  P(doy);
                    }

                    int yy = -1, dd = -1;
                    Util::serialToYd(&yy, &dd, serial);

                    LOOP2_ASSERT(  y, yy,   y == yy);
                    LOOP2_ASSERT(doy, dd, doy == dd);

                    yy = Util::serialToYear(serial);
                    dd = Util::serialToDayOfYear(serial);

                    LOOP2_ASSERT(  y, yy,   y == yy);
                    LOOP2_ASSERT(doy, dd, doy == dd);

                    ASSERT(yy == Util::serialToYearNoCache(serial));
                }
            }
        }
        ASSERT(serial ==
                    (yearRangeFlag ? k_SHORT_RANGE_MAX_SERIAL : k_MAX_SERIAL));

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (verbose) cout << "\t'serialToYd'" << endl;
            {
                int year, dayOfYear;

                ASSERT_SAFE_FAIL(Util::serialToYd(    0, &dayOfYear, 1812));
                ASSERT_SAFE_PASS(Util::serialToYd(&year, &dayOfYear, 1812));

                ASSERT_SAFE_FAIL(Util::serialToYd(&year,          0, 1812));
                ASSERT_SAFE_PASS(Util::serialToYd(&year, &dayOfYear, 1812));

                ASSERT_SAFE_FAIL(Util::serialToYd(
                                         &year, &dayOfYear, k_MIN_SERIAL - 1));
                ASSERT_SAFE_PASS(Util::serialToYd(
                                         &year, &dayOfYear, k_MIN_SERIAL    ));

                ASSERT_SAFE_PASS(Util::serialToYd(
                                         &year, &dayOfYear, k_MAX_SERIAL    ));
                ASSERT_SAFE_FAIL(Util::serialToYd(
                                         &year, &dayOfYear, k_MAX_SERIAL + 1));
            }

            if (verbose) cout << "\t'serialToDayOfYear'" << endl;
            {
                ASSERT_SAFE_FAIL(Util::serialToDayOfYear(k_MIN_SERIAL - 1));
                ASSERT_SAFE_PASS(Util::serialToDayOfYear(k_MIN_SERIAL    ));

                ASSERT_SAFE_PASS(Util::serialToDayOfYear(k_MAX_SERIAL    ));
                ASSERT_SAFE_FAIL(Util::serialToDayOfYear(k_MAX_SERIAL + 1));
            }

            if (verbose) cout << "\t'serialToYear'" << endl;
            {
                ASSERT_SAFE_FAIL(Util::serialToYear(k_MIN_SERIAL - 1));
                ASSERT_SAFE_PASS(Util::serialToYear(k_MIN_SERIAL    ));

                ASSERT_SAFE_PASS(Util::serialToYear(k_MAX_SERIAL    ));
                ASSERT_SAFE_FAIL(Util::serialToYear(k_MAX_SERIAL + 1));
            }

            if (verbose) cout << "\t'serialToYearNoCache'" << endl;
            {
                ASSERT_SAFE_FAIL(Util::serialToYearNoCache(k_MIN_SERIAL - 1));
                ASSERT_SAFE_PASS(Util::serialToYearNoCache(k_MIN_SERIAL    ));

                ASSERT_SAFE_PASS(Util::serialToYearNoCache(k_MAX_SERIAL    ));
                ASSERT_SAFE_FAIL(Util::serialToYearNoCache(k_MAX_SERIAL + 1));
            }
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'ydToSerial'
        //   Ensure that the method correctly maps every year/day-of-year date
        //   to its corresponding serial date.
        //
        // Concerns:
        //: 1 The method correctly maps every valid year/day-of-year date to
        //:   its corresponding serial date.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of (unique) valid
        //:   year/day-of-year dates (one per row), and the (integral) results
        //:   expected from 'ydToSerial' when applied to those tabulated dates.
        //:   For further assurance, exhaustively test 'ydToSerial' over a
        //:   range of years that is governed by the 'yearRangeFlag'.  (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-2)
        //
        // Testing:
        //   static int  ydToSerial(int year, int dayOfYear);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'ydToSerial'" << endl
                          << "====================" << endl;

        if (verbose) cout << "\tDirect test vectors." << endl;
        {
            static const struct {
                int d_line;       // source line number
                int d_year;       // year under test
                int d_dayOfYear;  // day of year under test
                int d_exp;        // expected value
            } DATA[] = {
                //LINE   YEAR   DAY OF YEAR       EXPECTED
                //----   ----   -----------       --------
                { L_,       1,            1,             1 },
                { L_,       1,          365,           365 },
                { L_,       2,            1,           366 },
                { L_,       2,          365,           730 },
                { L_,       3,          365,          1095 },
                { L_,       4,          365,          1460 },
                { L_,       4,          366,          1461 },
                { L_,       5,          365,          1826 },

                { L_,    1751,            1,        639175 },
                { L_,    1751,          365,        639539 },
                { L_,    1752,            1,        639540 },
                { L_,    1752,          355,        639894 },
                { L_,    1753,            1,        639906 },

                { L_,    1799,          365,        657071 },
                { L_,    1800,            1,        657072 },
                { L_,    1800,          365,        657436 },
                { L_,    1801,            1,        657437 },

                { L_,    1899,          365,        693595 },
                { L_,    1900,            1,        693596 },
                { L_,    1900,          365,        693960 },
                { L_,    1901,            1,        693961 },

                { L_,    1999,          365,        730119 },
                { L_,    2000,            1,        730120 },
                { L_,    2000,          366,        730485 },
                { L_,    2001,            1,        730486 },

                { L_,    2099,          365,        766644 },
                { L_,    2100,            1,        766645 },
                { L_,    2100,          365,        767009 },
                { L_,    2101,            1,        767010 },

                { L_,    2399,          365,        876216 },
                { L_,    2400,            1,        876217 },
                { L_,    2400,          366,        876582 },
                { L_,    2401,            1,        876583 },

                { L_,    9995,          365,       3650598 },
                { L_,    9996,            1,       3650599 },
                { L_,    9996,          366,       3650964 },
                { L_,    9997,            1,       3650965 },
                { L_,    9999,          365,  k_MAX_SERIAL },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE      = DATA[ti].d_line;
                const int YEAR      = DATA[ti].d_year;
                const int DAYOFYEAR = DATA[ti].d_dayOfYear;
                const int EXP       = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(LINE);  P_(YEAR);  P_(DAYOFYEAR);  P(EXP);
                }

                LOOP_ASSERT(LINE, EXP == Util::ydToSerial(YEAR, DAYOFYEAR));
            }
        }

        const int first = yearRangeFlag ? k_SHORT_RANGE_MIN_YEAR : k_MIN_YEAR;
        const int last  = yearRangeFlag ? k_SHORT_RANGE_MAX_YEAR : k_MAX_YEAR;

        if (verbose) cout << "\tExhaustively testing years "
                          << first << '-' << last << '.' << endl;

        int serial =
                 (yearRangeFlag ? k_SHORT_RANGE_MIN_SERIAL : k_MIN_SERIAL) - 1;

        for (int y = first; y <= last; ++y) {
            for (int m = 1; m <= 12; ++m) {
                for (int d = 1; d <= 31; ++d) {
                    if (Util::isValidYearMonthDay(y, m, d)) {
                        ++serial;

                        int doy = Util::ymdToDayOfYear(y, m, d);

                        if (veryVerbose && 0 == (serial - 1) % 100) {
                            P_(y);  P_(doy);  P(serial);
                        }

                        int ss = Util::ydToSerial(y, doy);

                        LOOP4_ASSERT(y, doy, ss, serial, ss == serial);
                    }
                }
            }
        }
        ASSERT(serial ==
                    (yearRangeFlag ? k_SHORT_RANGE_MAX_SERIAL : k_MAX_SERIAL));

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (verbose) cout << "\t'ydToSerial'" << endl;
            {
                ASSERT_SAFE_FAIL(Util::ydToSerial(k_MIN_YEAR - 1, 23));
                ASSERT_SAFE_PASS(Util::ydToSerial(k_MIN_YEAR    , 23));

                ASSERT_SAFE_PASS(Util::ydToSerial(k_MAX_YEAR    , 23));
                ASSERT_SAFE_FAIL(Util::ydToSerial(k_MAX_YEAR + 1, 23));

                ASSERT_SAFE_FAIL(Util::ydToSerial(1812,   0));
                ASSERT_SAFE_PASS(Util::ydToSerial(1812,   1));

                ASSERT_SAFE_PASS(Util::ydToSerial(1812, 366));
                ASSERT_SAFE_FAIL(Util::ydToSerial(1812, 367));

                ASSERT_SAFE_PASS(Util::ydToSerial(1813, 365));
                ASSERT_SAFE_FAIL(Util::ydToSerial(1813, 366));
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'ydTo{Day|Md|Month}'
        //   Ensure that the methods correctly map every year/day-of-year date
        //   to the corresponding month and day of the month.
        //
        // Concerns:
        //: 1 The 'ydToMd' method correctly maps every valid year/day-of-year
        //:   date to the corresponding month and day of the month.
        //:
        //: 2 The 'ydToMonth' method correctly maps every valid
        //:   year/day-of-year date to the corresponding month.
        //:
        //: 3 The 'ydToDay' method correctly maps every valid year/day-of-year
        //:   date to the corresponding day of the month.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of (unique) valid
        //:   year/day-of-year dates (one per row), and the (integral) month
        //:   and day (of the month) results expected from the three methods
        //:   when applied to those tabulated dates.  For further assurance,
        //:   exhaustively test the methods over a range of years that is
        //:   governed by the 'yearRangeFlag'.  (C-1..3)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   static int  ydToDay(int year, int dayOfYear);
        //   static void ydToMd(int *month, int *day, int year, int dayOfYear);
        //   static int  ydToMonth(int year, int dayOfYear);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'ydTo{Day|Md|Month}'" << endl
                          << "============================" << endl;

        if (verbose) cout << "\tDirect test vectors." << endl;
        {
            static const struct {
                int d_line;       // source line number
                int d_year;       // year under test
                int d_dayOfYear;  // day of year under test
                int d_expMonth;   // expected value for month
                int d_expDay;     // expected value for day
            } DATA[] = {
                //                            < EXPECTED >
                //LINE   YEAR   DAY OF YEAR   MONTH    DAY
                //----   ----   -----------   -----    ---
                { L_,       1,            1,      1,     1 },
                { L_,       1,          365,     12,    31 },
                { L_,       2,          365,     12,    31 },
                { L_,       3,          365,     12,    31 },
                { L_,       4,          365,     12,    30 },
                { L_,       4,          366,     12,    31 },
                { L_,       5,          365,     12,    31 },

                { L_,    1751,          365,     12,    31 },
                { L_,    1752,          355,     12,    20 },

                { L_,    1980,          365,     12,    30 },
                { L_,    1980,          366,     12,    31 },
                { L_,    1990,          365,     12,    31 },

                { L_,    1996,            1,      1,     1 },
                { L_,    1996,           31,      1,    31 },
                { L_,    1996,           32,      2,     1 },
                { L_,    1996,           59,      2,    28 },
                { L_,    1996,           60,      2,    29 },
                { L_,    1996,           61,      3,     1 },
                { L_,    1996,           91,      3,    31 },
                { L_,    1996,           92,      4,     1 },
                { L_,    1996,          121,      4,    30 },
                { L_,    1996,          122,      5,     1 },
                { L_,    1996,          152,      5,    31 },
                { L_,    1996,          153,      6,     1 },
                { L_,    1996,          182,      6,    30 },
                { L_,    1996,          183,      7,     1 },
                { L_,    1996,          213,      7,    31 },
                { L_,    1996,          214,      8,     1 },
                { L_,    1996,          244,      8,    31 },
                { L_,    1996,          245,      9,     1 },
                { L_,    1996,          274,      9,    30 },
                { L_,    1996,          275,     10,     1 },
                { L_,    1996,          305,     10,    31 },
                { L_,    1996,          306,     11,     1 },
                { L_,    1996,          335,     11,    30 },
                { L_,    1996,          336,     12,     1 },
                { L_,    1996,          366,     12,    31 },

                { L_,    2000,          365,     12,    30 },
                { L_,    2000,          366,     12,    31 },
                { L_,    2001,          365,     12,    31 },
                { L_,    2002,          365,     12,    31 },
                { L_,    2003,          365,     12,    31 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE      = DATA[ti].d_line;
                const int YEAR      = DATA[ti].d_year;
                const int DAYOFYEAR = DATA[ti].d_dayOfYear;
                const int EXPMONTH  = DATA[ti].d_expMonth;
                const int EXPDAY    = DATA[ti].d_expDay;

                if (veryVerbose) {
                    P_(LINE);  P_(YEAR);  P_(DAYOFYEAR);
                    P_(EXPMONTH);  P(EXPDAY);
                }

                int mm, dd;
                Util::ydToMd(&mm, &dd, YEAR, DAYOFYEAR);

                LOOP_ASSERT(LINE, EXPMONTH == mm);
                LOOP_ASSERT(LINE, EXPDAY   == dd);

                mm = Util::ydToMonth(YEAR, DAYOFYEAR);
                dd = Util::ydToDay(  YEAR, DAYOFYEAR);

                LOOP_ASSERT(LINE, EXPMONTH == mm);
                LOOP_ASSERT(LINE, EXPDAY   == dd);
            }
        }

        const int first = yearRangeFlag ? k_SHORT_RANGE_MIN_YEAR : k_MIN_YEAR;
        const int last  = yearRangeFlag ? k_SHORT_RANGE_MAX_YEAR : k_MAX_YEAR;

        if (verbose) cout << "\tExhaustively testing years "
                          << first << '-' << last << '.' << endl;

        int loopCount = 0;  // governs excessive verbosity

        for (int y = first; y <= last; ++y) {
            for (int m = 1; m <= 12; ++m) {
                for (int d = 1; d <= 31; ++d) {
                    if (Util::isValidYearMonthDay(y, m, d)) {

                        int doy = Util::ymdToDayOfYear(y, m, d);

                        if (veryVerbose && 0 == loopCount % 100) {
                            P_(y);  P_(m);  P_(d);  P(doy);
                        }
                        ++loopCount;

                        int mm = -1, dd = -1;
                        Util::ydToMd(&mm, &dd, y, doy);

                        LOOP4_ASSERT(y, doy, m, mm, m == mm);
                        LOOP4_ASSERT(y, doy, d, dd, d == dd);

                        mm = Util::ydToMonth(y, doy);
                        dd = Util::ydToDay(  y, doy);

                        LOOP4_ASSERT(y, doy, m, mm, m == mm);
                        LOOP4_ASSERT(y, doy, d, dd, d == dd);
                    }
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (verbose) cout << "\t'ydToMd'" << endl;
            {
                int month, day;

                ASSERT_SAFE_FAIL(Util::ydToMd(     0, &day, 1812, 23));
                ASSERT_SAFE_PASS(Util::ydToMd(&month, &day, 1812, 23));

                ASSERT_SAFE_FAIL(Util::ydToMd(&month,    0, 1812, 23));
                ASSERT_SAFE_PASS(Util::ydToMd(&month, &day, 1812, 23));

                ASSERT_SAFE_FAIL(Util::ydToMd(
                                            &month, &day, k_MIN_YEAR - 1, 23));
                ASSERT_SAFE_PASS(Util::ydToMd(
                                            &month, &day, k_MIN_YEAR    , 23));

                ASSERT_SAFE_PASS(Util::ydToMd(
                                            &month, &day, k_MAX_YEAR    , 23));
                ASSERT_SAFE_FAIL(Util::ydToMd(
                                            &month, &day, k_MAX_YEAR + 1, 23));

                ASSERT_SAFE_FAIL(Util::ydToMd(&month, &day, 1812,   0));
                ASSERT_SAFE_PASS(Util::ydToMd(&month, &day, 1812,   1));

                ASSERT_SAFE_PASS(Util::ydToMd(&month, &day, 1812, 366));
                ASSERT_SAFE_FAIL(Util::ydToMd(&month, &day, 1812, 367));

                ASSERT_SAFE_PASS(Util::ydToMd(&month, &day, 1813, 365));
                ASSERT_SAFE_FAIL(Util::ydToMd(&month, &day, 1813, 366));
            }

            if (verbose) cout << "\t'ydToMonth'" << endl;
            {
                ASSERT_SAFE_FAIL(Util::ydToMonth(k_MIN_YEAR - 1, 23));
                ASSERT_SAFE_PASS(Util::ydToMonth(k_MIN_YEAR    , 23));

                ASSERT_SAFE_PASS(Util::ydToMonth(k_MAX_YEAR    , 23));
                ASSERT_SAFE_FAIL(Util::ydToMonth(k_MAX_YEAR + 1, 23));

                ASSERT_SAFE_FAIL(Util::ydToMonth(1812,   0));
                ASSERT_SAFE_PASS(Util::ydToMonth(1812,   1));

                ASSERT_SAFE_PASS(Util::ydToMonth(1812, 366));
                ASSERT_SAFE_FAIL(Util::ydToMonth(1812, 367));

                ASSERT_SAFE_PASS(Util::ydToMonth(1813, 365));
                ASSERT_SAFE_FAIL(Util::ydToMonth(1813, 366));
            }

            if (verbose) cout << "\t'ydToDay'" << endl;
            {
                ASSERT_SAFE_FAIL(Util::ydToDay(k_MIN_YEAR - 1, 23));
                ASSERT_SAFE_PASS(Util::ydToDay(k_MIN_YEAR    , 23));

                ASSERT_SAFE_PASS(Util::ydToDay(k_MAX_YEAR    , 23));
                ASSERT_SAFE_FAIL(Util::ydToDay(k_MAX_YEAR + 1, 23));

                ASSERT_SAFE_FAIL(Util::ydToDay(1812,   0));
                ASSERT_SAFE_PASS(Util::ydToDay(1812,   1));

                ASSERT_SAFE_PASS(Util::ydToDay(1812, 366));
                ASSERT_SAFE_FAIL(Util::ydToDay(1812, 367));

                ASSERT_SAFE_PASS(Util::ydToDay(1813, 365));
                ASSERT_SAFE_FAIL(Util::ydToDay(1813, 366));
            }
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'ymdToDayOfYear'
        //   Ensure that the method correctly maps every year/month/day date to
        //   the corresponding day of the year.
        //
        // Concerns:
        //: 1 The method correctly maps every valid year/month/day date to its
        //:   corresponding day of the year.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of (unique) valid
        //:   year/month/day dates (one per row), and the (integral) results
        //:   expected from 'ymdToDayOfYear' when applied to those tabulated
        //:   dates.  For further assurance, exhaustively test 'ymdToDayOfYear'
        //:   over a range of years that is governed by the 'yearRangeFlag'.
        //:   (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-2)
        //
        // Testing:
        //   static int  ymdToDayOfYear(int year, int month, int day);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'ymdToDayOfYear'" << endl
                          << "========================" << endl;

        if (verbose) cout << "\tDirect test vectors." << endl;
        {
            static const struct {
                int d_line;   // source line number
                int d_year;   // year under test
                int d_month;  // month under test
                int d_day;    // day under test
                int d_exp;    // expected value
            } DATA[] = {
                //LINE   YEAR   MONTH    DAY   EXPECTED
                //----   ----   -----    ---   --------
                { L_,       1,      1,     1,         1 },
                { L_,       1,      1,     2,         2 },
                { L_,       1,      1,    30,        30 },
                { L_,       1,      1,    31,        31 },
                { L_,       1,      2,     1,        32 },
                { L_,       1,      2,    28,        59 },
                { L_,       1,      3,     1,        60 },
                { L_,       1,      3,    31,        90 },
                { L_,       1,      4,     1,        91 },
                { L_,       1,      4,    30,       120 },
                { L_,       1,      5,     1,       121 },
                { L_,       1,      5,    31,       151 },
                { L_,       1,      6,     1,       152 },
                { L_,       1,      6,    30,       181 },
                { L_,       1,      7,     1,       182 },
                { L_,       1,      7,    31,       212 },
                { L_,       1,      8,     1,       213 },
                { L_,       1,      8,    31,       243 },
                { L_,       1,      9,     1,       244 },
                { L_,       1,      9,    30,       273 },
                { L_,       1,     10,     1,       274 },
                { L_,       1,     10,    31,       304 },
                { L_,       1,     11,     1,       305 },
                { L_,       1,     11,    30,       334 },
                { L_,       1,     12,     1,       335 },
                { L_,       1,     12,    31,       365 },

                { L_,       2,      1,     1,         1 },
                { L_,       2,     12,    31,       365 },
                { L_,       3,      1,     1,         1 },
                { L_,       3,     12,    31,       365 },

                { L_,       4,      1,     1,         1 },
                { L_,       4,      1,     2,         2 },
                { L_,       4,      1,    30,        30 },
                { L_,       4,      1,    31,        31 },
                { L_,       4,      2,     1,        32 },
                { L_,       4,      2,    28,        59 },
                { L_,       4,      2,    29,        60 },
                { L_,       4,      3,     1,        61 },
                { L_,       4,      3,    31,        91 },
                { L_,       4,      4,     1,        92 },
                { L_,       4,      4,    30,       121 },
                { L_,       4,      5,     1,       122 },
                { L_,       4,      5,    31,       152 },
                { L_,       4,      6,     1,       153 },
                { L_,       4,      6,    30,       182 },
                { L_,       4,      7,     1,       183 },
                { L_,       4,      7,    31,       213 },
                { L_,       4,      8,     1,       214 },
                { L_,       4,      8,    31,       244 },
                { L_,       4,      9,     1,       245 },
                { L_,       4,      9,    30,       274 },
                { L_,       4,     10,     1,       275 },
                { L_,       4,     10,    31,       305 },
                { L_,       4,     11,     1,       306 },
                { L_,       4,     11,    30,       335 },
                { L_,       4,     12,     1,       336 },
                { L_,       4,     12,    31,       366 },

                { L_,       5,      1,     1,         1 },
                { L_,       5,     12,    31,       365 },

                { L_,    1601,     12,    31,       365 },
                { L_,    1721,     12,    31,       365 },
                { L_,    1749,     12,    31,       365 },
                { L_,    1750,     12,    31,       365 },
                { L_,    1751,     12,    31,       365 },

                { L_,    1752,      1,     1,         1 },
                { L_,    1752,      1,     2,         2 },
                { L_,    1752,      1,    30,        30 },
                { L_,    1752,      1,    31,        31 },
                { L_,    1752,      2,     1,        32 },
                { L_,    1752,      2,    28,        59 },
                { L_,    1752,      2,    29,        60 },
                { L_,    1752,      3,     1,        61 },
                { L_,    1752,      3,    31,        91 },
                { L_,    1752,      4,     1,        92 },
                { L_,    1752,      4,    30,       121 },
                { L_,    1752,      5,     1,       122 },
                { L_,    1752,      5,    31,       152 },
                { L_,    1752,      6,     1,       153 },
                { L_,    1752,      6,    30,       182 },
                { L_,    1752,      7,     1,       183 },
                { L_,    1752,      7,    31,       213 },
                { L_,    1752,      8,     1,       214 },
                { L_,    1752,      8,    31,       244 },
                { L_,    1752,      9,     1,       245 },
                { L_,    1752,      9,    30,       274 },
                { L_,    1752,     10,     1,       275 },
                { L_,    1752,     10,    31,       305 },
                { L_,    1752,     11,     1,       306 },
                { L_,    1752,     11,    30,       335 },
                { L_,    1752,     12,     1,       336 },
                { L_,    1752,     12,    31,       366 },

                { L_,    1753,     12,    31,       365 },
                { L_,    1754,     12,    31,       365 },
                { L_,    1755,     12,    31,       365 },

                { L_,    1756,      1,     1,         1 },
                { L_,    1756,      2,    28,        59 },
                { L_,    1756,      2,    29,        60 },
                { L_,    1756,      3,     1,        61 },
                { L_,    1756,      9,     1,       245 },
                { L_,    1756,      9,    30,       274 },
                { L_,    1756,     12,    31,       366 },

                { L_,    1757,     12,    31,       365 },
                { L_,    1758,     12,    31,       365 },
                { L_,    1759,     12,    31,       365 },
                { L_,    1760,     12,    31,       366 },

                { L_,    1799,     12,    31,       365 },
                { L_,    1800,     12,    31,       365 },
                { L_,    1801,     12,    31,       365 },
                { L_,    1802,     12,    31,       365 },

                { L_,    1899,     12,    31,       365 },
                { L_,    1900,     12,    31,       365 },
                { L_,    1901,     12,    31,       365 },
                { L_,    1902,     12,    31,       365 },

                { L_,    1999,     12,    31,       365 },
                { L_,    2000,     12,    31,       366 },
                { L_,    2001,     12,    31,       365 },
                { L_,    2002,     12,    31,       365 },

                { L_,    2099,     12,    31,       365 },
                { L_,    2100,     12,    31,       365 },
                { L_,    2101,     12,    31,       365 },
                { L_,    2102,     12,    31,       365 },

                { L_,    2399,     12,    31,       365 },
                { L_,    2400,     12,    31,       366 },
                { L_,    2401,     12,    31,       365 },
                { L_,    2402,     12,    31,       365 },

                { L_,    9995,     12,    31,       365 },
                { L_,    9996,     12,    31,       366 },
                { L_,    9997,     12,    31,       365 },
                { L_,    9998,     12,    31,       365 },
                { L_,    9999,     12,    31,       365 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_line;
                const int YEAR  = DATA[ti].d_year;
                const int MONTH = DATA[ti].d_month;
                const int DAY   = DATA[ti].d_day;
                const int EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(LINE);  P_(YEAR);  P_(MONTH);  P_(DAY);  P(EXP);
                }

                LOOP_ASSERT(LINE,
                            EXP == Util::ymdToDayOfYear(YEAR, MONTH, DAY));
            }
        }

        const int first = yearRangeFlag ? k_SHORT_RANGE_MIN_YEAR : k_MIN_YEAR;
        const int last  = yearRangeFlag ? k_SHORT_RANGE_MAX_YEAR : k_MAX_YEAR;

        if (verbose) cout << "\tExhaustively testing years "
                          << first << '-' << last << '.' << endl;

        int loopCount = 0;  // governs excessive verbosity

        for (int y = first; y <= last; ++y) {
            int doy = 0;

            for (int m = 1; m <= 12; ++m) {
                for (int d = 1; d <= 31; ++d) {
                    if (Util::isValidYearMonthDay(y, m, d)) {
                        ++doy;

                        if (veryVerbose && 0 == loopCount % 100) {
                            P_(y);  P_(m);  P_(d);  P(doy);
                        }
                        ++loopCount;

                        ASSERT(doy == Util::ymdToDayOfYear(y, m, d));
                    }
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (verbose) cout << "\t'ymdToDayOfYear'" << endl;
            {
                ASSERT_SAFE_FAIL(Util::ymdToDayOfYear(k_MIN_YEAR - 1, 9, 23));
                ASSERT_SAFE_PASS(Util::ymdToDayOfYear(k_MIN_YEAR    , 9, 23));

                ASSERT_SAFE_PASS(Util::ymdToDayOfYear(k_MAX_YEAR    , 9, 23));
                ASSERT_SAFE_FAIL(Util::ymdToDayOfYear(k_MAX_YEAR + 1, 9, 23));

                ASSERT_SAFE_FAIL(
                              Util::ymdToDayOfYear(1812, k_MIN_MONTH - 1, 23));
                ASSERT_SAFE_PASS(
                              Util::ymdToDayOfYear(1812, k_MIN_MONTH    , 23));

                ASSERT_SAFE_PASS(
                              Util::ymdToDayOfYear(1812, k_MAX_MONTH    , 23));
                ASSERT_SAFE_FAIL(
                              Util::ymdToDayOfYear(1812, k_MAX_MONTH + 1, 23));

                ASSERT_SAFE_FAIL(Util::ymdToDayOfYear(1812, 9,  0));
                ASSERT_SAFE_PASS(Util::ymdToDayOfYear(1812, 9,  1));

                ASSERT_SAFE_PASS(Util::ymdToDayOfYear(1812, 9, 30));
                ASSERT_SAFE_FAIL(Util::ymdToDayOfYear(1812, 9, 31));

                ASSERT_SAFE_PASS(Util::ymdToDayOfYear(1812, 2, 29));
                ASSERT_SAFE_FAIL(Util::ymdToDayOfYear(1812, 2, 30));

                ASSERT_SAFE_PASS(Util::ymdToDayOfYear(1813, 2, 28));
                ASSERT_SAFE_FAIL(Util::ymdToDayOfYear(1813, 2, 29));
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'ymdToSerial[NoCache]'
        //   Ensure that the methods correctly map every year/month/day date to
        //   its corresponding serial date.
        //
        // Concerns:
        //: 1 Both methods correctly map every valid year/month/day date to its
        //:   corresponding serial date.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of (unique) valid
        //:   year/month/day dates (one per row), and the (integral) results
        //:   expected from 'ymdToSerial[NoCache]' when applied to those
        //:   tabulated dates.  For further assurance, exhaustively test
        //:   'ymdToSerial[NoCache]' over a range of years that is governed by
        //:   the 'yearRangeFlag'.  (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-2)
        //
        // Testing:
        //   static int  ymdToSerial(int year, int month, int day);
        //   static int  ymdToSerialNoCache(int year, int month, int day);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'ymdToSerial[NoCache]'" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\tDirect test vectors." << endl;
        {
            static const struct {
                int d_line;   // source line number
                int d_year;   // year under test
                int d_month;  // month under test
                int d_day;    // day under test
                int d_exp;    // expected value
            } DATA[] = {
                //LINE   YEAR   MONTH    DAY       EXPECTED
                //----   ----   -----    ---       --------
                { L_,       1,      1,     1,             1 },
                { L_,       1,      1,     2,             2 },
                { L_,       1,      1,    30,            30 },
                { L_,       1,      1,    31,            31 },
                { L_,       1,      2,     1,            32 },
                { L_,       1,      2,    28,            59 },
                { L_,       1,      3,     1,            60 },
                { L_,       1,      3,    31,            90 },
                { L_,       1,      4,     1,            91 },
                { L_,       1,      4,    30,           120 },
                { L_,       1,      5,     1,           121 },
                { L_,       1,      5,    31,           151 },
                { L_,       1,      6,     1,           152 },
                { L_,       1,      6,    30,           181 },
                { L_,       1,      7,     1,           182 },
                { L_,       1,      7,    31,           212 },
                { L_,       1,      8,     1,           213 },
                { L_,       1,      8,    31,           243 },
                { L_,       1,      9,     1,           244 },
                { L_,       1,      9,    30,           273 },
                { L_,       1,     10,     1,           274 },
                { L_,       1,     10,    31,           304 },
                { L_,       1,     11,     1,           305 },
                { L_,       1,     11,    30,           334 },
                { L_,       1,     12,     1,           335 },
                { L_,       1,     12,    31,           365 },

                { L_,       2,      1,     1,           366 },
                { L_,       3,      1,     1,           731 },

                { L_,       4,      1,     1,          1096 },
                { L_,       4,      1,     2,          1097 },
                { L_,       4,      1,    30,          1125 },
                { L_,       4,      1,    31,          1126 },
                { L_,       4,      2,     1,          1127 },
                { L_,       4,      2,    28,          1154 },
                { L_,       4,      2,    29,          1155 },
                { L_,       4,      3,     1,          1156 },
                { L_,       4,      3,    31,          1186 },
                { L_,       4,      4,     1,          1187 },
                { L_,       4,      4,    30,          1216 },
                { L_,       4,      5,     1,          1217 },
                { L_,       4,      5,    31,          1247 },
                { L_,       4,      6,     1,          1248 },
                { L_,       4,      6,    30,          1277 },
                { L_,       4,      7,     1,          1278 },
                { L_,       4,      7,    31,          1308 },
                { L_,       4,      8,     1,          1309 },
                { L_,       4,      8,    31,          1339 },
                { L_,       4,      9,     1,          1340 },
                { L_,       4,      9,    30,          1369 },
                { L_,       4,     10,     1,          1370 },
                { L_,       4,     10,    31,          1400 },
                { L_,       4,     11,     1,          1401 },
                { L_,       4,     11,    30,          1430 },
                { L_,       4,     12,     1,          1431 },
                { L_,       4,     12,    31,          1461 },

                { L_,       5,      1,     1,          1462 },

                { L_,     100,     12,    31,         36524 },
                { L_,     101,      1,     1,         36525 },

                { L_,     400,     12,    31,        146097 },
                { L_,     401,      1,     1,        146098 },

                { L_,    1600,     12,    31,        584388 },
                { L_,    1601,      1,     1,        584389 },

                { L_,    1800,      1,     1,        657072 },
                { L_,    1800,     12,    31,        657436 },

                { L_,    1801,      1,     1,        657437 },
                { L_,    1801,     12,    31,        657801 },
                { L_,    1802,      1,     1,        657802 },

                { L_,    1899,     12,    31,        693595 },
                { L_,    1900,      1,     1,        693596 },
                { L_,    1900,     12,    31,        693960 },
                { L_,    1901,      1,     1,        693961 },
                { L_,    1901,     12,    31,        694325 },
                { L_,    1902,      1,     1,        694326 },

                { L_,    1999,     12,    31,        730119 },
                { L_,    2000,      1,     1,        730120 },
                { L_,    2000,     12,    31,        730485 },
                { L_,    2001,      1,     1,        730486 },
                { L_,    2001,     12,    31,        730850 },
                { L_,    2002,      1,     1,        730851 },

                { L_,    2099,     12,    31,        766644 },
                { L_,    2100,      1,     1,        766645 },
                { L_,    2100,     12,    31,        767009 },
                { L_,    2101,      1,     1,        767010 },
                { L_,    2101,     12,    31,        767374 },
                { L_,    2102,      1,     1,        767375 },

                { L_,    2399,     12,    31,        876216 },
                { L_,    2400,      1,     1,        876217 },
                { L_,    2400,     12,    31,        876582 },
                { L_,    2401,      1,     1,        876583 },
                { L_,    2401,     12,    31,        876947 },
                { L_,    2402,      1,     1,        876948 },

                { L_,    9995,      1,     1,       3650234 },
                { L_,    9995,     12,    31,       3650598 },
                { L_,    9996,      1,     1,       3650599 },
                { L_,    9996,     12,    31,       3650964 },
                { L_,    9997,      1,     1,       3650965 },
                { L_,    9997,     12,    31,       3651329 },
                { L_,    9998,      1,     1,       3651330 },
                { L_,    9998,     12,    31,       3651694 },
                { L_,    9999,      1,     1,       3651695 },
                { L_,    9999,     12,    31,  k_MAX_SERIAL },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_line;
                const int YEAR  = DATA[ti].d_year;
                const int MONTH = DATA[ti].d_month;
                const int DAY   = DATA[ti].d_day;
                const int EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(LINE);  P_(YEAR);  P_(MONTH);  P_(DAY);  P(EXP);
                }

                const int s1 = Util::ymdToSerial(YEAR, MONTH, DAY);
                const int s2 = Util::ymdToSerialNoCache(YEAR, MONTH, DAY);

                LOOP3_ASSERT(LINE, EXP, s1, EXP == s1);
                LOOP3_ASSERT(LINE, EXP, s2, EXP == s2);
            }
        }

        const int first = yearRangeFlag ? k_SHORT_RANGE_MIN_YEAR : k_MIN_YEAR;
        const int last  = yearRangeFlag ? k_SHORT_RANGE_MAX_YEAR : k_MAX_YEAR;

        if (verbose) cout << "\tExhaustively testing years "
                          << first << '-' << last << '.' << endl;

        int serial =
                 (yearRangeFlag ? k_SHORT_RANGE_MIN_SERIAL : k_MIN_SERIAL) - 1;

        for (int y = first; y <= last; ++y) {
            for (int m = 1; m <= 12; ++m) {
                for (int d = 1; d <= 31; ++d) {
                    if (Util::isValidYearMonthDay(y, m, d)) {
                        ++serial;

                        if (veryVerbose && 0 == (serial - 1) % 100) {
                            P_(y);  P_(m);  P_(d);  P(serial);
                        }

                        int s1 = Util::ymdToSerial(y, m, d);
                        int s2 = Util::ymdToSerialNoCache(y, m, d);

                        LOOP3_ASSERT(y, m, d, s1 == s2);
                        LOOP3_ASSERT(y, m, d, s2 == serial);
                    }
                }
            }
        }
        ASSERT(serial ==
                    (yearRangeFlag ? k_SHORT_RANGE_MAX_SERIAL : k_MAX_SERIAL));

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (verbose) cout << "\t'ymdToSerial'" << endl;
            {
                ASSERT_SAFE_FAIL(Util::ymdToSerial(k_MIN_YEAR - 1, 9, 23));
                ASSERT_SAFE_PASS(Util::ymdToSerial(k_MIN_YEAR    , 9, 23));

                ASSERT_SAFE_PASS(Util::ymdToSerial(k_MAX_YEAR    , 9, 23));
                ASSERT_SAFE_FAIL(Util::ymdToSerial(k_MAX_YEAR + 1, 9, 23));

                ASSERT_SAFE_FAIL(Util::ymdToSerial(1812, k_MIN_MONTH - 1, 23));
                ASSERT_SAFE_PASS(Util::ymdToSerial(1812, k_MIN_MONTH    , 23));

                ASSERT_SAFE_PASS(Util::ymdToSerial(1812, k_MAX_MONTH    , 23));
                ASSERT_SAFE_FAIL(Util::ymdToSerial(1812, k_MAX_MONTH + 1, 23));

                ASSERT_SAFE_FAIL(Util::ymdToSerial(1812, 9,  0));
                ASSERT_SAFE_PASS(Util::ymdToSerial(1812, 9,  1));

                ASSERT_SAFE_PASS(Util::ymdToSerial(1812, 9, 30));
                ASSERT_SAFE_FAIL(Util::ymdToSerial(1812, 9, 31));

                ASSERT_SAFE_PASS(Util::ymdToSerial(1812, 2, 29));
                ASSERT_SAFE_FAIL(Util::ymdToSerial(1812, 2, 30));

                ASSERT_SAFE_PASS(Util::ymdToSerial(1813, 2, 28));
                ASSERT_SAFE_FAIL(Util::ymdToSerial(1813, 2, 29));
            }

            if (verbose) cout << "\t'ymdToSerialNoCache'" << endl;
            {
                ASSERT_SAFE_FAIL(
                              Util::ymdToSerialNoCache(k_MIN_YEAR - 1, 9, 23));
                ASSERT_SAFE_PASS(
                              Util::ymdToSerialNoCache(k_MIN_YEAR    , 9, 23));

                ASSERT_SAFE_PASS(
                              Util::ymdToSerialNoCache(k_MAX_YEAR    , 9, 23));
                ASSERT_SAFE_FAIL(
                              Util::ymdToSerialNoCache(k_MAX_YEAR + 1, 9, 23));

                ASSERT_SAFE_FAIL(
                          Util::ymdToSerialNoCache(1812, k_MIN_MONTH - 1, 23));
                ASSERT_SAFE_PASS(
                          Util::ymdToSerialNoCache(1812, k_MIN_MONTH    , 23));

                ASSERT_SAFE_PASS(
                          Util::ymdToSerialNoCache(1812, k_MAX_MONTH    , 23));
                ASSERT_SAFE_FAIL(
                          Util::ymdToSerialNoCache(1812, k_MAX_MONTH + 1, 23));

                ASSERT_SAFE_FAIL(Util::ymdToSerialNoCache(1812, 9,  0));
                ASSERT_SAFE_PASS(Util::ymdToSerialNoCache(1812, 9,  1));

                ASSERT_SAFE_PASS(Util::ymdToSerialNoCache(1812, 9, 30));
                ASSERT_SAFE_FAIL(Util::ymdToSerialNoCache(1812, 9, 31));

                ASSERT_SAFE_PASS(Util::ymdToSerialNoCache(1812, 2, 29));
                ASSERT_SAFE_FAIL(Util::ymdToSerialNoCache(1812, 2, 30));

                ASSERT_SAFE_PASS(Util::ymdToSerialNoCache(1813, 2, 28));
                ASSERT_SAFE_FAIL(Util::ymdToSerialNoCache(1813, 2, 29));
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'isValid{Serial|YearDay|YearMonthDay}[NoCache]'
        //   Ensure that the methods accept (reject) valid (invalid) dates in
        //   any of the three supported formats.
        //
        // Concerns:
        //: 1 The 'isValidYearMonthDay[NoCache]' methods correctly categorize
        //:   every (year, month, day) triple as either a valid or an invalid
        //:   year/month/day date.
        //:
        //: 2 The 'isValidYearDay' method correctly categorizes every
        //:   (year, dayOfYear) pair as either a valid or an invalid
        //:   year/day-of-year date.
        //:
        //: 3 The 'isValidSerial' method correctly categorizes every 'int'
        //:   value as either a valid or an invalid serial date.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of (unique) valid
        //:   (year, month, day) triples (one per row), and the (boolean)
        //:   results expected from 'isValidYearMonthDay[NoCache]' when applied
        //:   to those tabulated triples.  For further assurance, exhaustively
        //:   test 'isValidYearMonthDay[NoCache]' over a range of years that is
        //:   governed by the 'yearRangeFlag'.  (C-1)
        //:
        //: 2 Using the table-driven technique, specify a set of (unique) valid
        //:   (year, dayOfYear) pairs (one per row), and the (boolean) results
        //:   expected from 'isValidYearDay' when applied to those
        //:   tabulated pairs.  For further assurance, exhaustively test
        //:   'isValidYearDay' over a range of years that is governed by
        //:   the 'yearRangeFlag'.  (C-2)
        //:
        //: 3 Using the table-driven technique, specify a set of (unique) valid
        //:   'int' values (one per row), and the (boolean) results expected
        //:   from 'isValidSerial' when applied to those tabulated values.
        //:   (C-3)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   static bool isValidSerial(int serialDay);
        //   static bool isValidYearDay(int year, int dayOfYear);
        //   static bool isValidYearMonthDay(int year, int month, int day);
        //   static bool isValidYearMonthDayNoCache(int y, int m, int day);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'isValid{Serial|YearDay|YearMonthDay}[NoCache]'"
                 << endl
                 << "======================================================="
                 << endl;

        if (verbose) cout << "\nTesting: 'isValidYearMonthDay[NoCache]'"
                          << endl;

        if (verbose) cout << "\tDirect test vectors." << endl;
        {
            static const struct {
                int d_line;   // source line number
                int d_year;   // year under test
                int d_month;  // month under test
                int d_day;    // day under test
                int d_exp;    // expected value
            } DATA[] = {
                //LINE    YEAR     MONTH       DAY   EXPECTED
                //----    ----     -----       ---   --------
                { L_,        0,        0,        0,         0 },
                { L_,        1,        1,        0,         0 },
                { L_,        1,        0,        1,         0 },
                { L_,        0,        1,        1,         0 },
                { L_,        1,        1,       -1,         0 },
                { L_,        1,        1,  INT_MIN,         0 },
                { L_,        1,       -1,        1,         0 },
                { L_,        1,  INT_MIN,        1,         0 },
                { L_,       -1,        1,        1,         0 },
                { L_,  INT_MIN,        1,        1,         0 },
                { L_,        0,       11,       30,         0 },
                { L_,        0,       12,       31,         0 },

                { L_,        1,        1,        1,         1 },
                { L_,        1,        1,       31,         1 },
                { L_,        2,        2,       28,         1 },
                { L_,        2,        2,       29,         0 },
                { L_,        2,        2,       30,         0 },
                { L_,        2,        3,        1,         1 },

                { L_,        4,        2,       28,         1 },
                { L_,        4,        2,       29,         1 },
                { L_,        4,        2,       30,         0 },
                { L_,        4,        3,        1,         1 },

                { L_,      100,        2,       28,         1 },
                { L_,      100,        2,       29,         0 },
                { L_,      100,        3,        1,         1 },

                { L_,      200,        2,       28,         1 },
                { L_,      200,        2,       29,         0 },
                { L_,      200,        3,        1,         1 },

                { L_,      400,        2,       28,         1 },
                { L_,      400,        2,       29,         1 },
                { L_,      400,        3,        1,         1 },

                { L_,      500,        2,       28,         1 },
                { L_,      500,        2,       29,         0 },
                { L_,      500,        3,        1,         1 },

                { L_,      800,        2,       28,         1 },
                { L_,      800,        2,       29,         1 },
                { L_,      800,        3,        1,         1 },

                { L_,     1600,        2,       29,         1 },
                { L_,     1700,        2,       29,         0 },
                { L_,     1800,        2,       29,         0 },
                { L_,     1900,        2,       29,         0 },
                { L_,     2000,        2,       29,         1 },
                { L_,     2100,        2,       29,         0 },

                { L_,     1752,        2,       28,         1 },
                { L_,     1752,        2,       29,         1 },
                { L_,     1752,        2,       30,         0 },

                { L_,     1752,        9,        3,         1 },
                { L_,     1752,        9,        4,         1 },
                { L_,     1752,        9,       13,         1 },
                { L_,     1752,        9,       14,         1 },
                { L_,     1752,        9,       30,         1 },
                { L_,     1752,        9,       31,         0 },

                { L_,     1900,        1,       30,         1 },
                { L_,     1900,        1,       31,         1 },
                { L_,     1900,        1,       32,         0 },

                { L_,     1900,        2,       28,         1 },
                { L_,     1900,        2,       29,         0 },
                { L_,     1900,        2,       30,         0 },

                { L_,     1900,        3,       30,         1 },
                { L_,     1900,        3,       31,         1 },
                { L_,     1900,        3,       32,         0 },

                { L_,     1900,        4,       30,         1 },
                { L_,     1900,        4,       31,         0 },
                { L_,     1900,        4,       32,         0 },

                { L_,     1900,        5,       30,         1 },
                { L_,     1900,        5,       31,         1 },
                { L_,     1900,        5,       32,         0 },

                { L_,     1900,        6,       30,         1 },
                { L_,     1900,        6,       31,         0 },
                { L_,     1900,        6,       32,         0 },

                { L_,     1900,        7,       30,         1 },
                { L_,     1900,        7,       31,         1 },
                { L_,     1900,        7,       32,         0 },

                { L_,     1900,        8,       30,         1 },
                { L_,     1900,        8,       31,         1 },
                { L_,     1900,        8,       32,         0 },

                { L_,     1900,        9,       30,         1 },
                { L_,     1900,        9,       31,         0 },
                { L_,     1900,        9,       32,         0 },

                { L_,     1900,       10,       30,         1 },
                { L_,     1900,       10,       31,         1 },
                { L_,     1900,       10,       32,         0 },

                { L_,     1900,       11,       30,         1 },
                { L_,     1900,       11,       31,         0 },
                { L_,     1900,       11,       32,         0 },

                { L_,     1900,       12,       30,         1 },
                { L_,     1900,       12,       31,         1 },
                { L_,     1900,       12,       32,         0 },

                { L_,     1996,        1,        1,         1 },
                { L_,     1996,        2,        1,         1 },
                { L_,     1996,        3,        1,         1 },
                { L_,     1996,        4,        1,         1 },
                { L_,     1996,        5,        1,         1 },
                { L_,     1996,        6,        1,         1 },
                { L_,     1996,        7,        1,         1 },
                { L_,     1996,        8,        1,         1 },
                { L_,     1996,        9,        1,         1 },
                { L_,     1996,       10,        1,         1 },
                { L_,     1996,       11,        1,         1 },
                { L_,     1996,       12,        1,         1 },
                { L_,     1996,       13,        1,         0 },

                { L_,     1997,        1,        1,         1 },
                { L_,     1997,        2,        1,         1 },
                { L_,     1997,        3,        1,         1 },
                { L_,     1997,        4,        1,         1 },
                { L_,     1997,        5,        1,         1 },
                { L_,     1997,        6,        1,         1 },
                { L_,     1997,        7,        1,         1 },
                { L_,     1997,        8,        1,         1 },
                { L_,     1997,        9,        1,         1 },
                { L_,     1997,       10,        1,         1 },
                { L_,     1997,       11,        1,         1 },
                { L_,     1997,       12,        1,         1 },
                { L_,     1997,       13,        1,         0 },

                { L_,     2000,        1,       30,         1 },
                { L_,     2000,        1,       31,         1 },
                { L_,     2000,        1,       32,         0 },

                { L_,     2000,        2,       28,         1 },
                { L_,     2000,        2,       29,         1 },
                { L_,     2000,        2,       30,         0 },

                { L_,     2000,        3,       30,         1 },
                { L_,     2000,        3,       31,         1 },
                { L_,     2000,        3,       32,         0 },

                { L_,     2000,        4,       30,         1 },
                { L_,     2000,        4,       31,         0 },
                { L_,     2000,        4,       32,         0 },

                { L_,     2000,        5,       30,         1 },
                { L_,     2000,        5,       31,         1 },
                { L_,     2000,        5,       32,         0 },

                { L_,     2000,        6,       30,         1 },
                { L_,     2000,        6,       31,         0 },
                { L_,     2000,        6,       32,         0 },

                { L_,     2000,        7,       30,         1 },
                { L_,     2000,        7,       31,         1 },
                { L_,     2000,        7,       32,         0 },

                { L_,     2000,        8,       30,         1 },
                { L_,     2000,        8,       31,         1 },
                { L_,     2000,        8,       32,         0 },

                { L_,     2000,        9,       30,         1 },
                { L_,     2000,        9,       31,         0 },
                { L_,     2000,        9,       32,         0 },

                { L_,     2000,       10,       30,         1 },
                { L_,     2000,       10,       31,         1 },
                { L_,     2000,       10,       32,         0 },

                { L_,     2000,       11,       30,         1 },
                { L_,     2000,       11,       31,         0 },
                { L_,     2000,       11,       32,         0 },

                { L_,     2000,       12,       30,         1 },
                { L_,     2000,       12,       31,         1 },
                { L_,     2000,       12,       32,         0 },

                { L_,     9999,        0,       10,         0 },
                { L_,     9999,       10,        0,         0 },
                { L_,     9999,       12,       31,         1 },
                { L_,    10000,        1,        1,         0 },
                { L_,  INT_MAX,        1,        1,         0 },
                { L_,        1,  INT_MAX,        1,         0 },
                { L_,        1,        1,  INT_MAX,         0 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_line;
                const int YEAR  = DATA[ti].d_year;
                const int MONTH = DATA[ti].d_month;
                const int DAY   = DATA[ti].d_day;
                const int EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(LINE);  P_(YEAR);  P_(MONTH);  P_(DAY);  P(EXP);
                }

                LOOP4_ASSERT(LINE, YEAR, MONTH, DAY,
                           EXP == Util::isValidYearMonthDay(YEAR, MONTH, DAY));

                LOOP4_ASSERT(LINE, YEAR, MONTH, DAY,
                    EXP == Util::isValidYearMonthDayNoCache(YEAR, MONTH, DAY));
            }
        }

        {
            const int first = yearRangeFlag ? k_SHORT_RANGE_MIN_YEAR :    -1;
            const int last  = yearRangeFlag ? k_SHORT_RANGE_MAX_YEAR : 10000;

            if (verbose) cout << "\tExhaustively testing years "
                              << first << '-' << last << '.' << endl;

            int loopCount = 0;  // governs excessive verbosity

            for (int y = first; y <= last; ++y) {
                for (int m = -1; m <= 13; ++m) {
                    for (int d = -1; d <= 32; ++d) {

                        bool isValid = y >= k_MIN_YEAR  && y <= k_MAX_YEAR
                                    && m >= k_MIN_MONTH && m <= k_MAX_MONTH
                                    && d >= 1
                                    && d <= Util::lastDayOfMonth(y, m);

                        if (veryVerbose && 0 == loopCount % 100) {
                            P_(isValid);  P_(y);  P_(m);  P(d);
                        }
                        ++loopCount;

                        int v1 = Util::isValidYearMonthDay(y, m, d);
                        int v2 = Util::isValidYearMonthDayNoCache(y, m, d);

                        LOOP3_ASSERT(y, m, d, isValid == v1);
                        LOOP3_ASSERT(y, m, d, isValid == v2);
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting: 'isValidYearDay'" << endl;

        if (verbose) cout << "\tDirect test vectors." << endl;
        {
            static const struct {
                int d_line;       // source line number
                int d_year;       // year under test
                int d_dayOfYear;  // day of year under test
                int d_exp;        // expected value
            } DATA[] = {
                //LINE    YEAR   DAY OF YEAR   EXPECTED
                //----    ----   -----------   --------
                { L_,        0,            0,         0 },
                { L_,        1,            0,         0 },
                { L_,        0,            1,         0 },
                { L_,        1,           -1,         0 },
                { L_,        1,      INT_MIN,         0 },
                { L_,       -1,            1,         0 },
                { L_,  INT_MIN,            1,         0 },
                { L_,        0,          365,         0 },
                { L_,        0,          366,         0 },

                { L_,        1,            1,         1 },
                { L_,        1,          365,         1 },
                { L_,        1,          366,         0 },
                { L_,        2,          365,         1 },
                { L_,        2,          366,         0 },
                { L_,        3,          365,         1 },
                { L_,        3,          366,         0 },
                { L_,        4,          365,         1 },
                { L_,        4,          366,         1 },
                { L_,        4,          367,         0 },
                { L_,        5,          365,         1 },
                { L_,        5,          366,         0 },

                { L_,     1752,          366,         1 },
                { L_,     1752,          367,         0 },
                { L_,     1753,          365,         1 },
                { L_,     1753,          366,         0 },
                { L_,     1754,          365,         1 },
                { L_,     1754,          366,         0 },
                { L_,     1755,          365,         1 },
                { L_,     1755,          366,         0 },
                { L_,     1756,          366,         1 },
                { L_,     1756,          367,         0 },

                { L_,     1899,          365,         1 },
                { L_,     1899,          366,         0 },
                { L_,     1900,          365,         1 },
                { L_,     1900,          366,         0 },
                { L_,     1901,          365,         1 },
                { L_,     1901,          366,         0 },
                { L_,     1902,          365,         1 },
                { L_,     1902,          366,         0 },
                { L_,     1903,          365,         1 },
                { L_,     1903,          366,         0 },
                { L_,     1904,          366,         1 },
                { L_,     1904,          367,         0 },
                { L_,     1905,          365,         1 },
                { L_,     1905,          366,         0 },

                { L_,     1999,          365,         1 },
                { L_,     1999,          366,         0 },
                { L_,     2000,          366,         1 },
                { L_,     2000,          367,         0 },
                { L_,     2001,          365,         1 },
                { L_,     2001,          366,         0 },
                { L_,     2002,          365,         1 },
                { L_,     2002,          366,         0 },
                { L_,     2003,          365,         1 },
                { L_,     2003,          366,         0 },
                { L_,     2004,          366,         1 },
                { L_,     2004,          367,         0 },
                { L_,     2005,          365,         1 },
                { L_,     2005,          366,         0 },

                { L_,     9999,            0,         0 },
                { L_,     9999,            1,         1 },
                { L_,     9999,          365,         1 },
                { L_,     9999,          366,         0 },
                { L_,     9999,      INT_MAX,         0 },
                { L_,    10000,          366,         0 },
                { L_,  INT_MAX,          365,         0 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE       = DATA[ti].d_line;
                const int YEAR       = DATA[ti].d_year;
                const int DAYOFYEAR  = DATA[ti].d_dayOfYear;
                const int EXP        = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(LINE);  P_(YEAR);  P_(DAYOFYEAR);  P(EXP);
                }

                LOOP3_ASSERT(LINE, YEAR, DAYOFYEAR,
                             EXP == Util::isValidYearDay(YEAR, DAYOFYEAR));
            }
        }

        {
            const int first = yearRangeFlag ? k_SHORT_RANGE_MIN_YEAR
                                            : k_MIN_YEAR;
            const int last  = yearRangeFlag ? k_SHORT_RANGE_MAX_YEAR
                                            : k_MAX_YEAR;

            if (verbose) cout << "\tExhaustively testing years "
                              << first << '-' << last << '.' << endl;

            int loopCount = 0;  // governs excessive verbosity

            for (int y = first; y <= last; ++y) {
                ASSERT(0 == Util::isValidYearDay(y, -y));
                ASSERT(0 == Util::isValidYearDay(y,  0));

                for (int doy = 1; doy <= 365; ++doy) {

                    if (veryVerbose && 0 == loopCount % 100) {
                        P_(y);  P(doy);
                    }
                    ++loopCount;

                    ASSERT(1 == Util::isValidYearDay(y, doy));
                }

                const bool isLeapYear = Util::isLeapYear(y);

                ASSERT(isLeapYear == Util::isValidYearDay(y, 366));
                ASSERT(         0 == Util::isValidYearDay(y, 367));
            }
        }

        if (verbose) cout << "\nTesting: 'isValidSerial'" << endl;
        {
            const int k_MID_SERIAL = (k_MAX_SERIAL - k_MIN_SERIAL) / 2 + 1;

            static const struct {
                int d_line;    // source line number
                int d_serial;  // serial date
                int d_exp;     // expected value
            } DATA[] = {
                //LINE       SERIAL       EXPECTED
                //----   --------------   --------
                { L_,    k_MIN_SERIAL-2,         0 },
                { L_,    k_MIN_SERIAL-1,         0 },
                { L_,    k_MIN_SERIAL  ,         1 },
                { L_,    k_MIN_SERIAL+1,         1 },
                { L_,    k_MIN_SERIAL+2,         1 },

                { L_,    k_MID_SERIAL-2,         1 },
                { L_,    k_MID_SERIAL-1,         1 },
                { L_,    k_MID_SERIAL  ,         1 },
                { L_,    k_MID_SERIAL+1,         1 },
                { L_,    k_MID_SERIAL+2,         1 },

                { L_,    k_MAX_SERIAL-2,         1 },
                { L_,    k_MAX_SERIAL-1,         1 },
                { L_,    k_MAX_SERIAL  ,         1 },
                { L_,    k_MAX_SERIAL+1,         0 },
                { L_,    k_MAX_SERIAL+2,         0 },

                { L_,         INT_MIN  ,         0 },
                { L_,         INT_MIN+1,         0 },
                { L_,         INT_MIN+2,         0 },

                { L_,         INT_MAX-2,         0 },
                { L_,         INT_MAX-1,         0 },
                { L_,         INT_MAX  ,         0 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE   = DATA[ti].d_line;
                const int SERIAL = DATA[ti].d_serial;
                const int EXP    = DATA[ti].d_exp;

                if (veryVerbose) { P_(LINE);  P_(SERIAL);  P(EXP); }

                LOOP2_ASSERT(LINE, SERIAL,
                             EXP == Util::isValidSerial(SERIAL));
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'lastDayOfMonth'
        //   Ensure that the method correctly identifies the last day of the
        //   month.
        //
        // Concerns:
        //: 1 The method correctly identifies the last day of the month for
        //:   every valid (year, month) pair.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of (unique) valid
        //:   (year, month) pairs (one per row), and the (integral) results
        //:   expected from 'lastDayOfMonth' when applied to those tabulated
        //:   pairs.  For further assurance, exhaustively test 'lastDayOfMonth'
        //:   over a range of years that is governed by the 'yearRangeFlag'.
        //:   (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-2)
        //
        // Testing:
        //   static int  lastDayOfMonth(int year, int month);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'lastDayOfMonth'" << endl
                          << "========================" << endl;

        if (verbose) cout << "\tDirect test vectors." << endl;
        {
            static const struct {
                int d_line;   // source line number
                int d_year;   // year under test
                int d_month;  // month under test
                int d_exp;    // expected value
            } DATA[] = {
                //LINE   YEAR   MONTH   EXPECTED
                //----   ----   -----   --------
                { L_,    1999,      1,        31 },
                { L_,    2000,      1,        31 },
                { L_,    2001,      1,        31 },
                { L_,    2002,      1,        31 },
                { L_,    2096,      1,        31 },
                { L_,    2100,      1,        31 },

                { L_,    1999,      2,        28 },
                { L_,    2000,      2,        29 },
                { L_,    2001,      2,        28 },
                { L_,    2002,      2,        28 },
                { L_,    2096,      2,        29 },
                { L_,    2100,      2,        28 },

                { L_,    1999,      3,        31 },
                { L_,    2000,      3,        31 },
                { L_,    2001,      3,        31 },
                { L_,    2002,      3,        31 },
                { L_,    2096,      3,        31 },
                { L_,    2100,      3,        31 },

                { L_,    1999,      4,        30 },
                { L_,    2000,      4,        30 },
                { L_,    2001,      4,        30 },
                { L_,    2002,      4,        30 },
                { L_,    2096,      4,        30 },
                { L_,    2100,      4,        30 },

                { L_,    1999,      5,        31 },
                { L_,    2000,      5,        31 },
                { L_,    2001,      5,        31 },
                { L_,    2002,      5,        31 },
                { L_,    2096,      5,        31 },
                { L_,    2100,      5,        31 },

                { L_,    1999,      6,        30 },
                { L_,    2000,      6,        30 },
                { L_,    2001,      6,        30 },
                { L_,    2002,      6,        30 },
                { L_,    2096,      6,        30 },
                { L_,    2100,      6,        30 },

                { L_,    1999,      7,        31 },
                { L_,    2000,      7,        31 },
                { L_,    2001,      7,        31 },
                { L_,    2002,      7,        31 },
                { L_,    2096,      7,        31 },
                { L_,    2100,      7,        31 },

                { L_,    1999,      8,        31 },
                { L_,    2000,      8,        31 },
                { L_,    2001,      8,        31 },
                { L_,    2002,      8,        31 },
                { L_,    2096,      8,        31 },
                { L_,    2100,      8,        31 },

                { L_,    1999,      9,        30 },
                { L_,    2000,      9,        30 },
                { L_,    2001,      9,        30 },
                { L_,    2002,      9,        30 },
                { L_,    2096,      9,        30 },
                { L_,    2100,      9,        30 },

                { L_,    1999,     10,        31 },
                { L_,    2000,     10,        31 },
                { L_,    2001,     10,        31 },
                { L_,    2002,     10,        31 },
                { L_,    2096,     10,        31 },
                { L_,    2100,     10,        31 },

                { L_,    1999,     11,        30 },
                { L_,    2000,     11,        30 },
                { L_,    2001,     11,        30 },
                { L_,    2002,     11,        30 },
                { L_,    2096,     11,        30 },
                { L_,    2100,     11,        30 },

                { L_,    1999,     12,        31 },
                { L_,    2000,     12,        31 },
                { L_,    2001,     12,        31 },
                { L_,    2002,     12,        31 },
                { L_,    2096,     12,        31 },
                { L_,    2100,     12,        31 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_line;
                const int YEAR  = DATA[ti].d_year;
                const int MONTH = DATA[ti].d_month;
                const int EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(LINE);  P_(YEAR);  P_(MONTH);  P(EXP);
                }

                LOOP3_ASSERT(LINE, YEAR, MONTH,
                             EXP == Util::lastDayOfMonth(YEAR, MONTH));
            }
        }

        const int first = yearRangeFlag ? k_SHORT_RANGE_MIN_YEAR : k_MIN_YEAR;
        const int last  = yearRangeFlag ? k_SHORT_RANGE_MAX_YEAR : k_MAX_YEAR;

        if (verbose) cout << "\tExhaustively testing years "
                          << first << '-' << last << '.' << endl;

        for (int y = first; y <= last; ++y) {
            for (int m = 1; m <= 12; ++m) {
                int exp = -1;

                switch (m) {
                  case  1: {  // JAN
                  case  3:    // MAR
                  case  5:    // MAY
                  case  7:    // JUL
                  case  8:    // AUG
                  case 10:    // OCT
                  case 12:    // DEC
                    exp = 31;
                  } break;
                  case  4: {  // APR
                  case  6:    // JUN
                  case  9:    // SEP
                  case 11:    // NOV
                    exp = 30;
                  } break;
                  case  2: {  // FEB
                    exp = Util::isLeapYear(y) ? 29 : 28;
                  } break;
                  default: {
                    BSLS_ASSERT_OPT(0 && "month value out of range");
                  } break;
                }

                if (veryVeryVerbose) { P_(y);  P_(m);  P(exp); }

                LOOP3_ASSERT(y, m, exp, exp == Util::lastDayOfMonth(y, m));
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (verbose) cout << "\t'lastDayOfMonth'" << endl;
            {
                ASSERT_SAFE_FAIL(Util::lastDayOfMonth(k_MIN_YEAR - 1, 9));
                ASSERT_SAFE_PASS(Util::lastDayOfMonth(k_MIN_YEAR    , 9));

                ASSERT_SAFE_PASS(Util::lastDayOfMonth(k_MAX_YEAR    , 9));
                ASSERT_SAFE_FAIL(Util::lastDayOfMonth(k_MAX_YEAR + 1, 9));

                ASSERT_SAFE_FAIL(Util::lastDayOfMonth(1812, k_MIN_MONTH - 1));
                ASSERT_SAFE_PASS(Util::lastDayOfMonth(1812, k_MIN_MONTH    ));

                ASSERT_SAFE_PASS(Util::lastDayOfMonth(1812, k_MAX_MONTH    ));
                ASSERT_SAFE_FAIL(Util::lastDayOfMonth(1812, k_MAX_MONTH + 1));
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'numLeapYears'
        //   Ensure that the method correctly calculates the number of leap
        //   years within any valid range of years.
        //
        // Concerns:
        //: 1 The method correctly computes the number of leap years within any
        //:   valid '[year1 .. year2]' range.
        //:
        //: 2 The rule regarding divisibility by { 4, 100, 400 } for
        //:   determining leap years is correctly applied.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of (unique) valid
        //:   '[year1 .. year2]' range values (one per row), and the (integral)
        //:   results expected from 'numLeapYears' when applied to those
        //:   tabulated values.  For further assurance, use the (already
        //:   proven) 'isLeapYear' function to verify the expected values.
        //:   (C-1..2)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-3)
        //
        // Testing:
        //   static int  numLeapYears(int year1, int year2);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'numLeapYears'" << endl
                          << "======================" << endl;

        {
            static const struct {
                int d_line;   // source line number
                int d_year1;  // 1st year in range under test
                int d_year2;  // 2nd year in range under test
                int d_exp;    // expected value
            } DATA[] = {
                //LINE   YEAR1   YEAR2   EXPECTED
                //----   -----   -----   --------
                { L_,        1,      1,         0 },
                { L_,        1,      2,         0 },
                { L_,        1,      3,         0 },
                { L_,        2,      2,         0 },
                { L_,        5,      7,         0 },
                { L_,        9,     11,         0 },
                { L_,       13,     15,         0 },
                { L_,       97,    103,         0 },
                { L_,      100,    100,         0 },
                { L_,      397,    399,         0 },
                { L_,      401,    403,         0 },
                { L_,      997,   1003,         0 },
                { L_,     1000,   1000,         0 },
                { L_,     1197,   1199,         0 },
                { L_,     1201,   1203,         0 },
                { L_,     9997,   9999,         0 },
                { L_,     9999,   9999,         0 },

                { L_,        1,      4,         1 },
                { L_,        1,      7,         1 },
                { L_,        4,      4,         1 },
                { L_,        5,      8,         1 },
                { L_,        5,     11,         1 },
                { L_,        8,      8,         1 },
                { L_,       93,    103,         1 },
                { L_,       96,     96,         1 },
                { L_,       97,    107,         1 },
                { L_,      104,    104,         1 },
                { L_,      397,    403,         1 },
                { L_,      400,    400,         1 },
                { L_,      993,   1003,         1 },
                { L_,      996,    996,         1 },
                { L_,     1193,   1199,         1 },
                { L_,     1196,   1196,         1 },
                { L_,     1197,   1203,         1 },
                { L_,     1200,   1200,         1 },
                { L_,     9993,   9996,         1 },
                { L_,     9993,   9999,         1 },
                { L_,     9996,   9996,         1 },
                { L_,     9996,   9999,         1 },

                { L_,        1,      8,         2 },
                { L_,        1,     11,         2 },
                { L_,        4,      8,         2 },
                { L_,        4,     11,         2 },
                { L_,       89,    103,         2 },
                { L_,       92,    103,         2 },
                { L_,       93,    104,         2 },
                { L_,      396,    403,         2 },
                { L_,      397,    404,         2 },
                { L_,      992,   1003,         2 },
                { L_,      993,   1004,         2 },
                { L_,     1196,   1203,         2 },
                { L_,     1197,   1204,         2 },
                { L_,     9992,   9999,         2 },

                { L_,        1,     99,        24 },
                { L_,        1,    103,        24 },
                { L_,        5,    107,        24 },
                { L_,      421,    523,        24 },
                { L_,      425,    527,        24 },
                { L_,     1361,   1463,        24 },
                { L_,     1365,   1467,        24 },

                { L_,        1,    399,        96 },
                { L_,        5,    403,        96 },
                { L_,      401,    799,        96 },
                { L_,      405,    803,        96 },

                { L_,      201,    603,        97 },
                { L_,      205,    607,        97 },
                { L_,     1041,   1443,        97 },
                { L_,     1045,   1447,        97 },

                { L_,        1,   9995,      2423 },
                { L_,        4,   9995,      2423 },
                { L_,        5,   9996,      2423 },
                { L_,        5,   9999,      2423 },

                { L_,        1,   9996,      2424 },
                { L_,        1,   9999,      2424 },
                { L_,        4,   9996,      2424 },
                { L_,        4,   9999,      2424 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_line;
                const int YEAR1 = DATA[ti].d_year1;
                const int YEAR2 = DATA[ti].d_year2;
                const int EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(LINE);  P_(YEAR1);  P_(YEAR2);  P(EXP);
                }

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -TP21

                int actual = 0;
                for (int y = YEAR1; y <= YEAR2; ++y) {
                    actual += Util::isLeapYear(y);
                }
                LOOP3_ASSERT(LINE, YEAR1, YEAR2, EXP == actual);

// BDE_VERIFY pragma: pop

                LOOP3_ASSERT(LINE, YEAR1, YEAR2,
                             EXP == Util::numLeapYears(YEAR1, YEAR2));
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (verbose) cout << "\t'numLeapYears'" << endl;
            {
                ASSERT_SAFE_FAIL(Util::numLeapYears(k_MIN_YEAR - 1, 1812));
                ASSERT_SAFE_PASS(Util::numLeapYears(k_MIN_YEAR    , 1812));

                ASSERT_SAFE_PASS(
                               Util::numLeapYears(k_MAX_YEAR    , k_MAX_YEAR));
                ASSERT_SAFE_FAIL(
                               Util::numLeapYears(k_MAX_YEAR + 1, k_MAX_YEAR));

                ASSERT_SAFE_FAIL(
                               Util::numLeapYears(k_MIN_YEAR, k_MIN_YEAR - 1));
                ASSERT_SAFE_PASS(
                               Util::numLeapYears(k_MIN_YEAR, k_MIN_YEAR    ));

                ASSERT_SAFE_PASS(Util::numLeapYears(1812, k_MAX_YEAR    ));
                ASSERT_SAFE_FAIL(Util::numLeapYears(1812, k_MAX_YEAR + 1));

                ASSERT_SAFE_FAIL(Util::numLeapYears(1813, 1812));
                ASSERT_SAFE_PASS(Util::numLeapYears(1812, 1812));
                ASSERT_SAFE_PASS(Util::numLeapYears(1812, 1813));
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'isLeapYear'
        //   Ensure that the method correctly identifies leap years.
        //
        // Concerns:
        //: 1 The method correctly identifies every year in the supported range
        //:   ('[1 .. 9999]') as either a leap year or a non-leap year.
        //:
        //: 2 The rule regarding divisibility by { 4, 100, 400 } is
        //:   implemented correctly.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of (unique) valid
        //:   year values (one per row), and the (boolean) results expected
        //:   from 'isLeapYear' when applied to those tabulated values.  For
        //:   further assurance, exhaustively test 'isLeapYear' over the entire
        //:   range of valid years.  (C-1..2)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-3)
        //
        // Testing:
        //   static bool isLeapYear(int year);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'isLeapYear'" << endl
                          << "====================" << endl;

        if (verbose) cout << "\tDirect test vectors." << endl;
        {
            static const struct {
                int d_line;  // source line number
                int d_year;  // year under test
                int d_exp;   // expected value
            } DATA[] = {
                //LINE   YEAR   EXPECTED
                //----   ----   --------
                { L_,       1,         0 },
                { L_,       2,         0 },
                { L_,       3,         0 },
                { L_,       4,         1 },
                { L_,       5,         0 },
                { L_,       8,         1 },
                { L_,      10,         0 },
                { L_,      96,         1 },
                { L_,      99,         0 },
                { L_,     100,         0 },
                { L_,     101,         0 },
                { L_,     104,         1 },
                { L_,     200,         0 },
                { L_,     300,         0 },
                { L_,     396,         1 },
                { L_,     399,         0 },
                { L_,     400,         1 },
                { L_,     401,         0 },
                { L_,     499,         0 },
                { L_,     500,         0 },
                { L_,     501,         0 },
                { L_,     600,         0 },
                { L_,     700,         0 },
                { L_,     799,         0 },
                { L_,     800,         1 },
                { L_,     801,         0 },
                { L_,     900,         0 },
                { L_,    1000,         0 },
                { L_,    1100,         0 },
                { L_,    1200,         1 },
                { L_,    1300,         0 },
                { L_,    1400,         0 },
                { L_,    1500,         0 },
                { L_,    1582,         0 },
                { L_,    1583,         0 },
                { L_,    1584,         1 },
                { L_,    1600,         1 },
                { L_,    1700,         0 },
                { L_,    1752,         1 },
                { L_,    1753,         0 },
                { L_,    1800,         0 },
                { L_,    1801,         0 },
                { L_,    1804,         1 },
                { L_,    1896,         1 },
                { L_,    1899,         0 },
                { L_,    1900,         0 },
                { L_,    1901,         0 },
                { L_,    1904,         1 },
                { L_,    1996,         1 },
                { L_,    1999,         0 },
                { L_,    2000,         1 },
                { L_,    2001,         0 },
                { L_,    2004,         1 },
                { L_,    2096,         1 },
                { L_,    2099,         0 },
                { L_,    2100,         0 },
                { L_,    2101,         0 },
                { L_,    2104,         1 },
                { L_,    2399,         0 },
                { L_,    2400,         1 },
                { L_,    2401,         0 },
                { L_,    7100,         0 },
                { L_,    8000,         1 },
                { L_,    9900,         0 },
                { L_,    9995,         0 },
                { L_,    9996,         1 },
                { L_,    9997,         0 },
                { L_,    9998,         0 },
                { L_,    9999,         0 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE = DATA[ti].d_line;
                const int YEAR = DATA[ti].d_year;
                const int EXP  = DATA[ti].d_exp;

                if (veryVerbose) { P_(LINE);  P_(YEAR);  P(EXP); }

                LOOP_ASSERT(LINE, EXP == Util::isLeapYear(YEAR));
            }
        }

        const int first = k_MIN_YEAR;
        const int last  = k_MAX_YEAR;

        if (verbose) cout << "\tExhaustively testing years "
                          << first << '-' << last << '.' << endl;

        for (int y = first; y <= last; ++y) {
            const bool isLeapFlag = Util::isLeapYear(y);

            if (veryVeryVerbose) { P_(y);  P(isLeapFlag); }

            // 1. All years not divisible by 4 are non-leap years.

            if (0 != y % 4) {
                LOOP2_ASSERT(y, isLeapFlag, !isLeapFlag);
            }

            // 2. All years divisible by 4 that are not century years are leap
            // years.

            if (0 == y % 4 && 0 != y % 100) {
                LOOP2_ASSERT(y, isLeapFlag,  isLeapFlag);
            }

            // 3. Century years are leap years iff they are divisible by 400.

            if (0 == y % 100) {
                LOOP2_ASSERT(y, isLeapFlag,  isLeapFlag == (0 == y % 400));
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (verbose) cout << "\t'isLeapYear'" << endl;
            {
                ASSERT_SAFE_FAIL(Util::isLeapYear(k_MIN_YEAR - 1));
                ASSERT_SAFE_PASS(Util::isLeapYear(k_MIN_YEAR    ));

                ASSERT_SAFE_PASS(Util::isLeapYear(k_MAX_YEAR    ));
                ASSERT_SAFE_FAIL(Util::isLeapYear(k_MAX_YEAR + 1));
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING GLOBAL CONSTANTS
        //   Ensure that the global constants whose values are not self-evident
        //   are correct.
        //
        // Concerns:
        //: 1 The "magic" values of 'k_MAX_SERIAL', 'k_SHORT_RANGE_MIN_SERIAL',
        //:   and 'k_SHORT_RANGE_MAX_SERIAL' are correct.
        //
        // Plan:
        //: 1 For completeness, assert axiomatic values and manifest
        //:   relationships among the constants.
        //:
        //: 2 Use the (as yet unproven) 'isLeapYear' function to calculate the
        //:   expected values of 'k_MAX_SERIAL', 'k_SHORT_RANGE_MIN_SERIAL',
        //:   and 'k_SHORT_RANGE_MAX_SERIAL'.  (Note that 'isLeapYear' is
        //:   thoroughly tested in case 2.)  (C-1)
        //
        // Testing:
        //   CONCERN: The global constants used for testing are correct.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING GLOBAL CONSTANTS" << endl
                          << "========================" << endl;

        if (verbose) cout << "\tAssert axiomatic values and relationships."
                          << endl;

        ASSERT(   1 == k_MIN_MONTH);
        ASSERT(  12 == k_MAX_MONTH);

        ASSERT(   1 == k_MIN_YEAR);
        ASSERT(9999 == k_MAX_YEAR);

        ASSERT(k_MIN_YEAR               < k_SHORT_RANGE_MIN_YEAR);
        ASSERT(k_SHORT_RANGE_MIN_YEAR   < k_SHORT_RANGE_MAX_YEAR);
        ASSERT(k_SHORT_RANGE_MAX_YEAR   < k_MAX_YEAR);

        ASSERT(500 <= k_SHORT_RANGE_MAX_YEAR - k_SHORT_RANGE_MIN_YEAR);

        ASSERT(   1 == k_MIN_SERIAL);

        ASSERT(k_MIN_SERIAL             < k_SHORT_RANGE_MIN_SERIAL);
        ASSERT(k_SHORT_RANGE_MIN_SERIAL < k_SHORT_RANGE_MAX_SERIAL);
        ASSERT(k_SHORT_RANGE_MAX_SERIAL < k_MAX_SERIAL);

        ASSERT(   2 == k_MIN_DAYOFWEEK);

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -TP21

        if (verbose) cout << "\tVerify non-axiomatic values." << endl;

        int computedSerialValue = k_MIN_SERIAL - 1;

        for (int y = k_MIN_YEAR; y <= k_MAX_YEAR; ++y) {
            computedSerialValue += Util::isLeapYear(y) ? 366 : 365;
        }
        ASSERT(k_MAX_SERIAL == computedSerialValue);

        computedSerialValue = k_MIN_SERIAL - 1;

        for (int y = k_MIN_YEAR; y < k_SHORT_RANGE_MIN_YEAR; ++y) {
            computedSerialValue += Util::isLeapYear(y) ? 366 : 365;
        }
        ASSERT(k_SHORT_RANGE_MIN_SERIAL == computedSerialValue + 1);

        for (int y = k_SHORT_RANGE_MIN_YEAR; y <= k_SHORT_RANGE_MAX_YEAR; ++y){
            computedSerialValue += Util::isLeapYear(y) ? 366 : 365;
        }
        ASSERT(k_SHORT_RANGE_MAX_SERIAL == computedSerialValue);

        int computedShortRangeMinDayOfWeek =
               (k_MIN_DAYOFWEEK + k_SHORT_RANGE_MIN_SERIAL - k_MIN_SERIAL) % 7;

        if (0 == computedShortRangeMinDayOfWeek) {
            computedShortRangeMinDayOfWeek = 1;
        }

        ASSERT(k_SHORT_RANGE_MIN_DAYOFWEEK == computedShortRangeMinDayOfWeek);

// BDE_VERIFY pragma: pop

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // CACHE GENERATOR
        //   Generate the C++ code (to be inserted into the '.cpp' file) that
        //   implements the cache.
        //
        // Testing:
        //   CACHE GENERATOR
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CACHE GENERATOR" << endl
                          << "===============" << endl;

        const int firstYear       = 1980;
        const int lastYear        = 2040;
        const int firstSerialDate = Util::ymdToSerialNoCache(firstYear, 1,  1);
        const int lastSerialDate  = Util::ymdToSerialNoCache(lastYear, 12, 31);
        const int numCachedYears  = lastYear - firstYear + 1;

        cout << "const int SerialDateImpUtil::s_firstCachedYear       = "
             << firstYear << ";" << endl;

        cout << "const int SerialDateImpUtil::s_lastCachedYear        = "
             << lastYear << ";" << endl;

        cout << "const int SerialDateImpUtil::s_firstCachedSerialDate = "
             << firstSerialDate << ";" << endl;

        cout << "const int SerialDateImpUtil::s_lastCachedSerialDate  = "
             << lastSerialDate << ";\n" << endl;

        // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

        cout << "const SerialDateImpUtil::YearMonthDay\n"
             << "    SerialDateImpUtil::s_cachedYearMonthDay[] = {"
             << endl;

        for (int i = firstSerialDate; i <= lastSerialDate; ++i) {
            cout << "  { "
                 << Util::serialToYearNoCache(i)  << ", " << setw(2)
                 << Util::serialToMonthNoCache(i) << ", " << setw(2)
                 << Util::serialToDayNoCache(i)   << " },";

            if ((i - firstSerialDate + 1) % 4 == 0) {
                cout << "\n";
            }
        }

        cout << "\n};\n" << endl;

        // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

        cout << "const int SerialDateImpUtil::s_cachedSerialDate["
             << numCachedYears << "][13] = {" << endl;

        for (int y = firstYear; y <= lastYear; ++y) {
            cout << "  { 0, ";

            for (int m = 1; m <= 12; ++m) {
                cout << setw(6) << Util::ymdToSerialNoCache(y, m, 1) - 1;

                if (m % 7 == 6) {
                    cout << ",\n       ";
                }
                else {
                    cout << ", ";
                }
            }

            cout << "  }," << endl;
        }

        cout << "};\n" << endl;

        // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

        cout << "const char SerialDateImpUtil::s_cachedDaysInMonth["
             << numCachedYears << "][13] = {" << endl;

        for (int y = firstYear; y <= lastYear; ++y) {
            cout << "  { 0";

            for (int m = 1; m <= 12; ++m) {
                cout << ", " << Util::lastDayOfMonth(y, m);
            }

            cout << " }," << endl;
        }

        cout << "};" << endl;

      } break;
      case -2: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST: 'isValidYearMonthDay[NoCache]'
        //   Manually verify that the cache provides a performance advantage.
        //
        // Testing:
        //   PERFORMANCE TEST: isValidYearMonthDay[NoCache]
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "PERFORMANCE TEST: 'isValidYearMonthDay[NoCache]'" << endl
                 << "================================================" << endl;

        int year = 2000, month = 2, day = 16;  // *must* be within cache

        const int NUM_ITERATIONS = 10000000;

        if (verbose)
            cout << "\nTesting 'isValidYearMonthDayNoCache(y, m, d)':" << endl;
        {
            bsls::Stopwatch sw;

            sw.start(true);
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                Util::isValidYearMonthDayNoCache(year, month, day);
                Util::isValidYearMonthDayNoCache(year, month, day);
                Util::isValidYearMonthDayNoCache(year, month, day);
                Util::isValidYearMonthDayNoCache(year, month, day);
                Util::isValidYearMonthDayNoCache(year, month, day);

                Util::isValidYearMonthDayNoCache(year, month, day);
                Util::isValidYearMonthDayNoCache(year, month, day);
                Util::isValidYearMonthDayNoCache(year, month, day);
                Util::isValidYearMonthDayNoCache(year, month, day);
                Util::isValidYearMonthDayNoCache(year, month, day);
            }
            sw.stop();

            if (verbose)
                cout << "\tUser time: " << sw.accumulatedUserTime() << endl;
        }

        if (verbose)
            cout << "\nTesting 'isValidYearMonthDay(y, m, d)':" << endl;
        {
            bsls::Stopwatch sw;

            sw.start(true);
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                Util::isValidYearMonthDay(year, month, day);
                Util::isValidYearMonthDay(year, month, day);
                Util::isValidYearMonthDay(year, month, day);
                Util::isValidYearMonthDay(year, month, day);
                Util::isValidYearMonthDay(year, month, day);

                Util::isValidYearMonthDay(year, month, day);
                Util::isValidYearMonthDay(year, month, day);
                Util::isValidYearMonthDay(year, month, day);
                Util::isValidYearMonthDay(year, month, day);
                Util::isValidYearMonthDay(year, month, day);
            }
            sw.stop();

            if (verbose)
                cout << "\tUser time: " << sw.accumulatedUserTime() << endl;
        }

      } break;
      case -3: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST: 'ymdToSerial[NoCache]'
        //   Manually verify that the cache provides a performance advantage.
        //
        // Testing:
        //   PERFORMANCE TEST: ymdToSerial[NoCache]
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "PERFORMANCE TEST: 'ymdToSerial[NoCache]'" << endl
                 << "========================================" << endl;

        int year = 2000, month = 2, day = 16;  // *must* be within cache

        const int NUM_ITERATIONS = 10000000;

        if (verbose)
            cout << "\nTesting 'ymdToSerialNoCache(y, m, d)':" << endl;
        {
            bsls::Stopwatch sw;

            sw.start(true);
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                Util::ymdToSerialNoCache(year, month, day);
                Util::ymdToSerialNoCache(year, month, day);
                Util::ymdToSerialNoCache(year, month, day);
                Util::ymdToSerialNoCache(year, month, day);
                Util::ymdToSerialNoCache(year, month, day);

                Util::ymdToSerialNoCache(year, month, day);
                Util::ymdToSerialNoCache(year, month, day);
                Util::ymdToSerialNoCache(year, month, day);
                Util::ymdToSerialNoCache(year, month, day);
                Util::ymdToSerialNoCache(year, month, day);
            }
            sw.stop();

            if (verbose)
                cout << "\tUser time: " << sw.accumulatedUserTime() << endl;
        }

        if (verbose)
            cout << "\nTesting 'ymdToSerial(y, m, d)':" << endl;
        {
            bsls::Stopwatch sw;

            sw.start(true);
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                Util::ymdToSerial(year, month, day);
                Util::ymdToSerial(year, month, day);
                Util::ymdToSerial(year, month, day);
                Util::ymdToSerial(year, month, day);
                Util::ymdToSerial(year, month, day);

                Util::ymdToSerial(year, month, day);
                Util::ymdToSerial(year, month, day);
                Util::ymdToSerial(year, month, day);
                Util::ymdToSerial(year, month, day);
                Util::ymdToSerial(year, month, day);
            }
            sw.stop();

            if (verbose)
                cout << "\tUser time: " << sw.accumulatedUserTime() << endl;
        }

      } break;
      case -4: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST: 'serialToYmd[NoCache]'
        //   Manually verify that the cache provides a performance advantage.
        //
        // Testing:
        //   PERFORMANCE TEST: serialToYmd[NoCache]
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "PERFORMANCE TEST: 'serialToYmd[NoCache]'" << endl
                 << "========================================" << endl;

        int year = 1900, month = 2, day = 16;  // arbitrary values

        int serial = Util::ymdToSerial(2002, 5, 13);  // *must* be within cache

        const int NUM_ITERATIONS = 10000000;

        if (verbose)
            cout << "\nTesting 'serialToYmdNoCache(&y, &m, &d, serialDay)':"
                 << endl;
        {
            bsls::Stopwatch sw;

            sw.start(true);
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                Util::serialToYmdNoCache(&year, &month, &day, serial);
                Util::serialToYmdNoCache(&year, &month, &day, serial);
                Util::serialToYmdNoCache(&year, &month, &day, serial);
                Util::serialToYmdNoCache(&year, &month, &day, serial);
                Util::serialToYmdNoCache(&year, &month, &day, serial);

                Util::serialToYmdNoCache(&year, &month, &day, serial);
                Util::serialToYmdNoCache(&year, &month, &day, serial);
                Util::serialToYmdNoCache(&year, &month, &day, serial);
                Util::serialToYmdNoCache(&year, &month, &day, serial);
                Util::serialToYmdNoCache(&year, &month, &day, serial);
            }
            sw.stop();

            if (verbose)
                cout << "\tUser time: " << sw.accumulatedUserTime() << endl;
        }

        if (verbose)
            cout << "\nTesting 'serialToYmd(&y, &m, &d, serialDay)':" << endl;
        {
            bsls::Stopwatch sw;

            sw.start(true);
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                Util::serialToYmd(&year, &month, &day, serial);
                Util::serialToYmd(&year, &month, &day, serial);
                Util::serialToYmd(&year, &month, &day, serial);
                Util::serialToYmd(&year, &month, &day, serial);
                Util::serialToYmd(&year, &month, &day, serial);

                Util::serialToYmd(&year, &month, &day, serial);
                Util::serialToYmd(&year, &month, &day, serial);
                Util::serialToYmd(&year, &month, &day, serial);
                Util::serialToYmd(&year, &month, &day, serial);
                Util::serialToYmd(&year, &month, &day, serial);
            }
            sw.stop();

            if (verbose)
                cout << "\tUser time: " << sw.accumulatedUserTime() << endl;
        }

      } break;
      case -5: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST: 'serialToYear[NoCache]'
        //   Manually verify that the cache provides a performance advantage.
        //
        // Testing:
        //   PERFORMANCE TEST: serialToYear[NoCache]
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "PERFORMANCE TEST: 'serialToYear[NoCache]'" << endl
                 << "=========================================" << endl;

        int serial = Util::ymdToSerial(2002, 5, 13);  // *must* be within cache

        const int NUM_ITERATIONS = 10000000;

        if (verbose)
            cout << "\nTesting 'serialToYearNoCache(serialDay)':" << endl;
        {
            bsls::Stopwatch sw;

            sw.start(true);
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                Util::serialToYearNoCache(serial);
                Util::serialToYearNoCache(serial);
                Util::serialToYearNoCache(serial);
                Util::serialToYearNoCache(serial);
                Util::serialToYearNoCache(serial);

                Util::serialToYearNoCache(serial);
                Util::serialToYearNoCache(serial);
                Util::serialToYearNoCache(serial);
                Util::serialToYearNoCache(serial);
                Util::serialToYearNoCache(serial);
            }
            sw.stop();

            if (verbose)
                cout << "\tUser time: " << sw.accumulatedUserTime() << endl;
        }

        if (verbose)
            cout << "\nTesting 'serialToYear(serialDay)':" << endl;
        {
            bsls::Stopwatch sw;

            sw.start(true);
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                Util::serialToYear(serial);
                Util::serialToYear(serial);
                Util::serialToYear(serial);
                Util::serialToYear(serial);
                Util::serialToYear(serial);

                Util::serialToYear(serial);
                Util::serialToYear(serial);
                Util::serialToYear(serial);
                Util::serialToYear(serial);
                Util::serialToYear(serial);
            }
            sw.stop();

            if (verbose)
                cout << "\tUser time: " << sw.accumulatedUserTime() << endl;
        }

      } break;
      case -6: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST: 'serialToMonth[NoCache]'
        //   Manually verify that the cache provides a performance advantage.
        //
        // Testing:
        //   PERFORMANCE TEST: serialToMonth[NoCache]
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "PERFORMANCE TEST: 'serialToMonth[NoCache]'" << endl
                 << "==========================================" << endl;

        int serial = Util::ymdToSerial(2002, 5, 13);  // *must* be within cache

        const int NUM_ITERATIONS = 10000000;

        if (verbose)
            cout << "\nTesting 'serialToMonthNoCache(serialDay)':" << endl;
        {
            bsls::Stopwatch sw;

            sw.start(true);
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                Util::serialToMonthNoCache(serial);
                Util::serialToMonthNoCache(serial);
                Util::serialToMonthNoCache(serial);
                Util::serialToMonthNoCache(serial);
                Util::serialToMonthNoCache(serial);

                Util::serialToMonthNoCache(serial);
                Util::serialToMonthNoCache(serial);
                Util::serialToMonthNoCache(serial);
                Util::serialToMonthNoCache(serial);
                Util::serialToMonthNoCache(serial);
            }
            sw.stop();

            if (verbose)
                cout << "\tUser time: " << sw.accumulatedUserTime() << endl;
        }

        if (verbose)
            cout << "\nTesting 'serialToMonth(serialDay)':" << endl;
        {
            bsls::Stopwatch sw;

            sw.start(true);
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                Util::serialToMonth(serial);
                Util::serialToMonth(serial);
                Util::serialToMonth(serial);
                Util::serialToMonth(serial);
                Util::serialToMonth(serial);

                Util::serialToMonth(serial);
                Util::serialToMonth(serial);
                Util::serialToMonth(serial);
                Util::serialToMonth(serial);
                Util::serialToMonth(serial);
            }
            sw.stop();

            if (verbose)
                cout << "\tUser time: " << sw.accumulatedUserTime() << endl;
        }

      } break;
      case -7: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST: 'serialToDay[NoCache]'
        //   Manually verify that the cache provides a performance advantage.
        //
        // Testing:
        //   PERFORMANCE TEST: serialToDay[NoCache]
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "PERFORMANCE TEST: 'serialToDay[NoCache]'" << endl
                 << "========================================" << endl;

        int serial = Util::ymdToSerial(2002, 5, 13);  // *must* be within cache

        const int NUM_ITERATIONS = 10000000;

        if (verbose)
            cout << "\nTesting 'serialToDayNoCache(serialDay)':" << endl;
        {
            bsls::Stopwatch sw;

            sw.start(true);
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                Util::serialToDayNoCache(serial);
                Util::serialToDayNoCache(serial);
                Util::serialToDayNoCache(serial);
                Util::serialToDayNoCache(serial);
                Util::serialToDayNoCache(serial);

                Util::serialToDayNoCache(serial);
                Util::serialToDayNoCache(serial);
                Util::serialToDayNoCache(serial);
                Util::serialToDayNoCache(serial);
                Util::serialToDayNoCache(serial);
            }
            sw.stop();

            if (verbose)
                cout << "\tUser time: " << sw.accumulatedUserTime() << endl;
        }

        if (verbose)
            cout << "\nTesting 'serialToDay(serialDay)':" << endl;
        {
            bsls::Stopwatch sw;

            sw.start(true);
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                Util::serialToDay(serial);
                Util::serialToDay(serial);
                Util::serialToDay(serial);
                Util::serialToDay(serial);
                Util::serialToDay(serial);

                Util::serialToDay(serial);
                Util::serialToDay(serial);
                Util::serialToDay(serial);
                Util::serialToDay(serial);
                Util::serialToDay(serial);
            }
            sw.stop();

            if (verbose)
                cout << "\tUser time: " << sw.accumulatedUserTime() << endl;
        }

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
