// bdlt_date.t.cpp                                                    -*-C++-*-
#include <bdlt_date.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
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

#include <bsl_climits.h>     // 'INT_MAX', 'INT_MIN'
#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>     // 'memcmp', 'strcmp'
#include <bsl_iostream.h>
#include <bsl_sstream.h>


using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines a complex-constrained, value-semantic type
// (for terminology, see 'bsldoc_glossary'), 'bdlt::Date', most of whose
// non-trivial functionality is provided by a fully-tested implementation
// utility component ('bdlt_serialdateimputil').  No allocator is involved.
// Moreover, the state of a date object is identically its value.  As such, our
// testing concerns are (safely) limited to the mechanical functioning of the
// various methods and free operators, and exception neutrality during BDEX
// streaming.
//
// The underlying representation of a date is a single (32-bit) "serial date"
// integer.  However, the class interface represents a date value as either two
// or three integer fields, i.e., '(year, dayOfYear)' or '(year, month, day)',
// respectively.  Thus, it is always necessary to verify that each of the
// parameters is correctly forwarded to the appropriate implementation utility
// functions.
//
// Note that the choice of Primary Manipulators and Basic Accessors (below)
// comports with the common view that '(year, month, day)' is the canonical
// representation of a date.  Consequently, we restrict ourselves to the
// three-integer representation of dates in the first ten test cases.  The
// '(year, dayOfYear)' methods are first encountered in case 12.
//
// Primary Manipulators:
//: o Value constructor 'Date(int year, int month, int day)'
//
// Basic Accessors:
//: o 'day'
//: o 'month'
//: o 'year'
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR/MANIPULATOR/OPERATOR ptr./ref. parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o No memory is ever allocated from the default allocator.
//: o Precondition violations are detected in appropriate build modes.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [13] static bool isValidYearDay(int year, int dayOfYear);
// [13] static bool isValidYearMonthDay(int year, int month, int day);
// [10] static int maxSupportedBdexVersion(int versionSelector);
//
// CREATORS
// [11] Date();
// [12] Date(int year, int dayOfYear);
// [ 2] Date(int year, int month, int day);
// [ 7] Date(const Date& original);
// [ 2] ~Date();
//
// MANIPULATORS
// [ 9] Date& operator=(const Date& rhs);
// [15] Date& operator+=(int numDays);
// [15] Date& operator-=(int numDays);
// [14] Date& operator++();
// [14] Date& operator--();
// [19] int addDaysIfValid(int numDays);
// [12] void setYearDay(int year, int dayOfYear);
// [13] int setYearDayIfValid(int year, int dayOfYear);
// [11] void setYearMonthDay(int year, int month, int day);
// [13] int setYearMonthDayIfValid(int year, int month, int day);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
//
// ACCESSORS
// [ 4] int day() const;
// [12] int dayOfYear() const;
// [ 4] int month() const;
// [ 4] int year() const;
// [18] bdlt::DayOfWeek::Enum dayOfWeek() const;
// [12] void getYearDay(int *year, int *dayOfYear) const;
// [11] void getYearMonthDay(int *year, int *month, int *day) const;
// [18] bdlt::MonthOfYear::Enum monthOfYear() const;
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const Date& lhs, const Date& rhs);
// [ 6] bool operator!=(const Date& lhs, const Date& rhs);
// [ 5] ostream& operator<<(ostream &os, const Date& object);
// [17] bool operator< (const Date& lhs, const Date& rhs);
// [17] bool operator<=(const Date& lhs, const Date& rhs);
// [17] bool operator> (const Date& lhs, const Date& rhs);
// [17] bool operator>=(const Date& lhs, const Date& rhs);
// [14] Date operator++(Date& date, int);
// [14] Date operator--(Date& date, int);
// [15] Date operator+(const Date& date, int numDays);
// [15] Date operator+(int numDays, const Date& date);
// [15] Date operator-(const Date& date, int numDays);
// [16] int operator-(const Date& lhs, const Date& rhs);
// [20] hashAppend(HASHALG& hashAlg, const Date& date);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [21] USAGE EXAMPLE
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ *] CONCERN: In no case does memory come from the default allocator.
// [17] CONCERN: All ctor/manip./free op. ptr./ref. params are 'const'.
// [18] CONCERN: All accessor methods are declared 'const'.
// [15] CONCERN: Precondition violations are detected when enabled.
// [ 3] Reserved for 'gg' generator function.
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

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlt::Date          Obj;
typedef bslx::TestInStream  In;
typedef bslx::TestOutStream Out;

#define VERSION_SELECTOR 20140601


// ============================================================================
//                                 TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(true == bsl::is_trivially_copyable<Obj>::value);

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define DEFAULT DATA for test cases that use a single year/month/day
// representation for a date per test vector.

struct DefaultDataRow {
    int d_line;   // source line number
    int d_year;   // year under test
    int d_month;  // month under test
    int d_day;    // day under test
};

static
const DefaultDataRow DEFAULT_DATA[] =
{
    //LINE   YEAR   MONTH   DAY
    //----   ----   -----   ---
    { L_,       1,      1,    1 },
    { L_,       1,      1,    2 },
    { L_,       1,      2,    1 },
    { L_,       2,      1,    1 },
    { L_,      10,      4,    5 },
    { L_,      10,     10,   11 },
    { L_,     100,      6,    7 },
    { L_,     100,     11,   12 },
    { L_,    1000,      8,    9 },
    { L_,    1100,      1,   31 },
    { L_,    1200,      2,   29 },
    { L_,    1300,      3,   31 },
    { L_,    1400,      4,   30 },
    { L_,    1500,      5,   31 },
    { L_,    1600,      6,   30 },
    { L_,    1700,      7,   31 },
    { L_,    1800,      8,   31 },
    { L_,    1900,      9,   30 },
    { L_,    2000,     10,   31 },
    { L_,    2100,     11,   30 },
    { L_,    2200,     12,   31 },
    { L_,    9999,      1,    1 },
    { L_,    9999,      2,   28 },
    { L_,    9999,     11,   30 },
    { L_,    9999,     12,   30 },
    { L_,    9999,     12,   31 },
};
const int DEFAULT_NUM_DATA =
                  static_cast<int>(sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA);

// Define ALTernate DATA for test cases that use two year/day-of-year
// representations for dates per test vector, and the difference (in days)
// between those two dates.

struct AltDataRow {
    int d_line;     // source line number
    int d_V_year;   // V's year
    int d_V_day;    // V's day of year
    int d_W_year;   // W's year
    int d_W_day;    // W's day of year
    int d_numDays;  // number of days to add ('W - V')
};

static
const AltDataRow ALT_DATA[] =
{
    //       <-- V --->    <-- W --->    (W - V)
    //LINE   YEAR   DAY    YEAR   DAY   NUM DAYS
    //----   ----   ---    ----   ---   --------
    { L_,       1,    1,      1,    1,         0 },
    { L_,    1803,   27,   1803,   27,         0 },
    { L_,    9999,  365,   9999,  365,         0 },

    { L_,       1,    1,      1,    2,         1 },
    { L_,      10,   59,     10,   60,         1 },
    { L_,    9999,  364,   9999,  365,         1 },

    { L_,     100,   90,    100,   92,         2 },

    { L_,    1000,  120,   1000,  124,         4 },

    { L_,    1100,  151,   1100,  181,        30 },

    { L_,    9999,  334,   9999,  365,        31 },

    { L_,    1000,  120,   1000,  152,        32 },
    { L_,    1200,  182,   1200,  214,        32 },

    { L_,    1999,   59,   2000,   58,       364 },

    { L_,    1000,    1,   1001,    1,       365 },
    { L_,    1998,   59,   1999,   59,       365 },

    { L_,    1200,    1,   1201,    1,       366 },
    { L_,    1997,   59,   1998,   60,       366 },

    { L_,    1996,   59,   1997,   61,       368 },

    { L_,    1999,   59,   2002,   59,      1096 },

    { L_,       1,    1,   9999,  365,   3652058 },
};
const int ALT_NUM_DATA = static_cast<int>(sizeof ALT_DATA / sizeof *ALT_DATA);

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

    // CONCERN: This test driver is reusable w/other, similar components.

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
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
///Example 1: Basic Use of 'bdlt::Date'
/// - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to create and use a
// 'bdlt::Date' object.
//
// First, we create a default date 'd1':
//..
    bdlt::Date d1;           ASSERT(   1 == d1.year());
                             ASSERT(   1 == d1.month());
                             ASSERT(   1 == d1.day());
//..
// Next, we set 'd1' to July 4, 1776:
//..
    d1.setYearMonthDay(1776, 7, 4);
                             ASSERT(1776 == d1.year());
                             ASSERT(   7 == d1.month());
                             ASSERT(   4 == d1.day());
//..
// We can also use 'setYearMonthDayIfValid' if we are not sure whether a
// particular year/month/day combination constitutes a valid 'bdlt::Date'.  For
// example, if we want to set 'd1' to '1900/02/29', and it turns out that year
// 1900 was not a leap year (it wasn't), there will be no effect on the current
// value of the object:
//..
    int ret = d1.setYearMonthDayIfValid(1900, 2, 29);
                             ASSERT(   0 != ret);         // 1900 not leap year
                             ASSERT(1776 == d1.year());   // no effect on 'd1'
                             ASSERT(   7 == d1.month());
                             ASSERT(   4 == d1.day());
//..
// Then, from 'd1', we can determine the day of the year, and the day of the
// week, of July 4, 1776:
//..
    int dayOfYear = d1.dayOfYear();
                             ASSERT( 186 == dayOfYear);

    bdlt::DayOfWeek::Enum dayOfWeek = d1.dayOfWeek();
                             ASSERT(bdlt::DayOfWeek::e_THU == dayOfWeek);
//..
// Next, we create a 'bdlt::Date' object, 'd2', using the year/day-of-year
// representation for dates:
//..
    bdlt::Date d2(1776, dayOfYear);
                             ASSERT(1776 == d2.year());
                             ASSERT( 186 == d2.dayOfYear());
                             ASSERT(   7 == d2.month());
                             ASSERT(   4 == d2.day());
                             ASSERT(  d1 == d2);
//..
// Then, we add six days to the value of 'd2':
//..
    d2 += 6;                 ASSERT(1776 == d2.year());
                             ASSERT(   7 == d2.month());
                             ASSERT(  10 == d2.day());
//..
// Now, we subtract 'd1' from 'd2', storing the (signed) difference in days
// (a.k.a. *Actual* difference) in 'daysDiff':
//..
    int daysDiff = d2 - d1;  ASSERT(   6 == daysDiff);
//..
// Finally, we stream the value of 'd2' to 'stdout':
//..
if (verbose)
    bsl::cout << d2 << bsl::endl;
//..
// The streaming operator produces:
//..
//  10JUL1776
//..
// on 'stdout'.

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING: hashAppend
        //
        // Concerns:
        //: 1 Hashes different inputs differently
        //
        //: 2 Hashes equal inputs identically
        //
        //: 3 Works for const and non-const dates
        //
        // Plan:
        //: 1 Brute force test of a few hand picked values, ensuring that
        //    hashes of equivalent values match and hashes of unequal values do
        //    not.
        //
        // Testing:
        //     hashAppend(HASHALG& hashAlg, const Date&  date);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING 'hashAppend'"
                          << "\n====================\n";

        if (verbose) cout << "Brute force test of several dates." << endl;
        {
            typedef ::BloombergLP::bslh::Hash<> Hasher;

            bdlt::Date d1; // P-1
            bdlt::Date d2(1999, 12, 31);
            bdlt::Date d3(1999, 12, 31);
            bdlt::Date d4(1, 1, 2);
            const bdlt::Date d5(1, 1, 2);
            const bdlt::Date d6(1, 1, 3);

            Hasher hasher;
            Hasher::result_type a1 = hasher(d1), a2 = hasher(d2),
                                a3 = hasher(d3), a4 = hasher(d4),
                                a5 = hasher(d5), a6 = hasher(d6);

            if (veryVerbose) {
                cout << "\tHash of " << d1 << " is " << a1 << endl;
                cout << "\tHash of " << d2 << " is " << a2 << endl;
                cout << "\tHash of " << d3 << " is " << a3 << endl;
                cout << "\tHash of " << d4 << " is " << a4 << endl;
                cout << "\tHash of " << d5 << " is " << a5 << endl;
                cout << "\tHash of " << d6 << " is " << a6 << endl;
            }

            ASSERT(a1 != a2);
            ASSERT(a1 != a3);
            ASSERT(a1 != a4);
            ASSERT(a1 != a5);
            ASSERT(a1 != a6);
            if (veryVerbose) {
                cout << "\td1/d2: " << int(a1 != a2)
                     << ", d1/d3: " << int(a1 != a3)
                     << ", d1/d4: " << int(a1 != a4)
                     << ", d1/d5: " << int(a1 != a5)
                     << ", d1/d6: " << int(a1 != a6) << endl;
            }
            ASSERT(a2 == a3);
            ASSERT(a2 != a4);
            ASSERT(a2 != a5);
            ASSERT(a2 != a6);
            if (veryVerbose) {
                cout << "\td2/d3: " << int(a2 != a3)
                     << ", d2/d4: " << int(a2 != a4)
                     << ", d2/d5: " << int(a2 != a5)
                     << ", d2/d6: " << int(a2 != a6) << endl;
            }
            ASSERT(a3 != a4);
            ASSERT(a3 != a5);
            ASSERT(a3 != a6);
            if (veryVerbose) {
                cout << "\td3/d4: " << int(a3 != a4)
                     << ", d3/d5: " << int(a3 != a5)
                     << ", d3/d6: " << int(a3 != a6) << endl;
            }
            ASSERT(a4 == a5);
            ASSERT(a4 != a6);
            if (veryVerbose) {
                cout << "\td4/d5: " << int(a4 != a5)
                     << ", d4/d6: " << int(a4 != a6) << endl;
            }
            ASSERT(a5 != a6);
            if (veryVerbose) {
                cout << "\td5/d6: " << int(a5 != a6) << endl;
            }
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // 'addDaysIfValid' METHOD
        //   Ensure that the method correctly adjusts object state by the
        //   specified (signed) number of days only if the result is valid.
        //
        // Concerns:
        //: 1 The method has the same effect on the object as 'operator+=' when
        //:   the addition results in a valid date.
        //:
        //: 2 The method has no effect on the object if the addition would not
        //:   result in a valid date.
        //:
        //: 3 The method accepts any 'int' value.
        //:
        //: 4 The method returns 0 on success, and a non-zero value on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct date
        //:   object value pairs (one pair per row) in terms of their
        //:   year/day-of-year representations, and the non-negative number of
        //:   days ('NUM_DAYS') that the second date value is advanced from
        //:   that of the first date value.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1)
        //:
        //:   1 Use the 2-argument value constructor to create two 'const'
        //:     objects, 'V' and 'W', having the first and second date values
        //:     from 'R', respectively.
        //:
        //:   2 Use the copy constructor to create a modifiable object, 'mX',
        //:     from 'V'.
        //:
        //:   3 Verify that the 'addDaysIfValid' method, when invoked on 'mX'
        //:     and passed 'NUM_DAYS' from 'R', returns the expected value (0).
        //:
        //:   4 Verify, using the equality-comparison operator, that 'mX' now
        //:     has the same value as that of 'W'.  (C-1)
        //:
        //:   5 Repeat steps similar to those described in P-2.2..4 except
        //:     that, this time, create 'mX' from 'W' and pass '-NUM_DAYS' to
        //:     'addDaysIfValid'.  Verify that 'addDaysIfValid' still returns
        //:     0, but that 'mX' now has the same value as that of 'V'.  (C-1)
        //:
        //: 3 Using the table-driven technique, specify a set of distinct date
        //:   object values (one per row) in terms of their year/day-of-year
        //:   representations, and a (signed) number of days ('NUM_DAYS') that
        //:   when added to the date would produce an invalid value.
        //:
        //: 4 For each row 'R' in the table of P-3:  (C-2..4)
        //:
        //:   1 Use the 2-argument value constructor to create a 'const'
        //:     object, 'W', having the date value from 'R'.
        //:
        //:   2 Use the copy constructor to create a modifiable object, 'mX',
        //:     from 'W'.
        //:
        //:   3 Verify that the 'addDaysIfValid' method, when invoked on 'mX'
        //:     and passed 'NUM_DAYS' from 'R', returns the expected value
        //:     (non-zero).  (C-3..4)
        //:
        //:   4 Verify, using the equality-comparison operator, that 'mX' still
        //:     has the same value as that of 'W'.  (C-2)
        //
        // Testing:
        //   int addDaysIfValid(int numDays);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'addDaysIfValid' METHOD" << endl
                          << "=======================" << endl;

        if (verbose) cout << "Testing 'addDaysIfValid' success." << endl;
        {
            const int NUM_DATA                 = ALT_NUM_DATA;
            const AltDataRow (&DATA)[NUM_DATA] = ALT_DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE     = DATA[ti].d_line;
                const int V_YEAR   = DATA[ti].d_V_year;
                const int V_DAY    = DATA[ti].d_V_day;
                const int W_YEAR   = DATA[ti].d_W_year;
                const int W_DAY    = DATA[ti].d_W_day;
                const int NUM_DAYS = DATA[ti].d_numDays;

                const Obj V(V_YEAR, V_DAY);
                const Obj W(W_YEAR, W_DAY);

                if (veryVerbose) { T_ P_(LINE) P_(V) P_(W) P(NUM_DAYS) }

                {
                    Obj mX(V);  const Obj& X = mX;

                    LOOP_ASSERT(LINE, 0 == mX.addDaysIfValid(NUM_DAYS));
                    LOOP_ASSERT(LINE, W == X);

                    if (veryVeryVerbose) { T_ T_ P(X) }
                }

                {
                    Obj mX(W);  const Obj& X = mX;

                    LOOP_ASSERT(LINE, 0 == mX.addDaysIfValid(-NUM_DAYS));
                    LOOP_ASSERT(LINE, V == X);

                    if (veryVeryVerbose) { T_ T_ P(X) }
                }
            }
        }

        if (verbose) cout << "Testing 'addDaysIfValid' failure." << endl;
        {
            static const struct {
                int d_line;     // source line number
                int d_year;     // year under test
                int d_day;      // day of year under test
                int d_numDays;  // number of days to add
            } DATA[] = {
                //LINE   YEAR    DAY    NUM DAYS
                //----   ----    ---    --------
                { L_,       1,     1,    INT_MIN },
                { L_,    9999,   365,    INT_MIN },

                { L_,    9999,   365,   -3652059 },

                { L_,       1,   365,       -365 },
                { L_,       2,     1,       -366 },

                { L_,       1,    32,        -32 },

                { L_,       1,    31,        -31 },

                { L_,       1,     2,         -2 },

                { L_,       1,     1,         -1 },

                { L_,    9999,   365,          1 },

                { L_,    9999,   364,          2 },

                { L_,    9999,   334,         32 },

                { L_,    9999,     1,        365 },

                { L_,    9998,   365,        366 },

                { L_,       1,     1,    3652059 },

                { L_,       1,     1,    INT_MAX },
                { L_,    9999,   365,    INT_MAX },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE     = DATA[ti].d_line;
                const int YEAR     = DATA[ti].d_year;
                const int DAY      = DATA[ti].d_day;
                const int NUM_DAYS = DATA[ti].d_numDays;

                const Obj W(YEAR, DAY);

                if (veryVerbose) { T_ P_(LINE) P_(W) P(NUM_DAYS) }

                Obj mX(W);  const Obj& X = mX;

                LOOP_ASSERT(LINE, 0 != mX.addDaysIfValid(NUM_DAYS));
                LOOP_ASSERT(LINE, W == X);

                if (veryVeryVerbose) { T_ T_ P(X) }
            }
        }

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // 'dayOfWeek' AND 'monthOfYear' METHODS
        //   Ensure that the correct day of the week and month of the year are
        //   returned for any date.
        //
        // Concerns:
        //: 1 For any date, the correct day of the week is returned by
        //:   'dayOfWeek'.
        //:
        //: 2 For any date, the correct month of the year is returned by
        //:   'monthOfYear'.
        //:
        //: 3 The methods are declared 'const'.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their year/month/day
        //:   representation, and the 'bdlt::DayOfWeek::Enum' and
        //:   'bdlt::MonthOfYear::Enum' values expected from the methods
        //:   'dayOfWeek' and 'monthOfYear' when applied to those tabulated
        //:   dates.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..3)
        //:
        //:   1 Create a 'const' object 'X' using the 3-argument value
        //:     constructor.
        //:
        //:   2 Verify that 'dayOfWeek', invoked on 'X', returns the expected
        //:     value.  (C-1)
        //:
        //:   3 Verify that 'monthOfYear', invoked on 'X', returns the expected
        //:     value.  (C-2..3)
        //
        // Testing:
        //   bdlt::DayOfWeek::Enum dayOfWeek() const;
        //   bdlt::MonthOfYear::Enum monthOfYear() const;
        //   CONCERN: All accessor methods are declared 'const'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'dayOfWeek' AND 'monthOfYear' METHODS" << endl
                          << "=====================================" << endl;

        typedef bdlt::DayOfWeek   DOW;
        typedef bdlt::MonthOfYear MOY;

        static const struct {
            int       d_line;    // source line number
            int       d_year;    // year under test
            int       d_month;   // month under test
            int       d_day;     // day under test
            DOW::Enum d_expDOW;  // expected day of week
            MOY::Enum d_expMOY;  // expected month of year
        } DATA[] = {
            //LINE   YEAR   MONTH   DAY    EXPECTED DAY OF WEEK  EXPECTED MONTH
            //----   ----   -----   ---    --------------------  --------------
            { L_,       1,      1,    1,   DOW::e_MON,           MOY::e_JAN },
            { L_,       1,      1,    2,   DOW::e_TUE,           MOY::e_JAN },
            { L_,       1,      1,    3,   DOW::e_WED,           MOY::e_JAN },
            { L_,       1,      1,    4,   DOW::e_THU,           MOY::e_JAN },
            { L_,       1,      1,    5,   DOW::e_FRI,           MOY::e_JAN },
            { L_,       1,      1,    6,   DOW::e_SAT,           MOY::e_JAN },
            { L_,       1,      1,    7,   DOW::e_SUN,           MOY::e_JAN },
            { L_,       1,      1,    8,   DOW::e_MON,           MOY::e_JAN },

            { L_,       1,      2,    1,   DOW::e_THU,           MOY::e_FEB },
            { L_,       2,      1,    1,   DOW::e_TUE,           MOY::e_JAN },

            { L_,    1600,     12,   31,   DOW::e_SUN,           MOY::e_DEC },

            { L_,    1752,      9,    2,   DOW::e_SAT,           MOY::e_SEP },
            { L_,    1752,      9,    3,   DOW::e_SUN,           MOY::e_SEP },
            { L_,    1752,      9,    8,   DOW::e_FRI,           MOY::e_SEP },
            { L_,    1752,      9,   13,   DOW::e_WED,           MOY::e_SEP },
            { L_,    1752,      9,   14,   DOW::e_THU,           MOY::e_SEP },

            { L_,    1999,     12,   31,   DOW::e_FRI,           MOY::e_DEC },

            { L_,    2000,      1,    1,   DOW::e_SAT,           MOY::e_JAN },
            { L_,    2000,      2,   28,   DOW::e_MON,           MOY::e_FEB },
            { L_,    2000,      2,   29,   DOW::e_TUE,           MOY::e_FEB },

            { L_,    9999,     12,   31,   DOW::e_FRI,           MOY::e_DEC },

            { L_,    2014,      1,    1,   DOW::e_WED,           MOY::e_JAN },
            { L_,    2014,      2,    1,   DOW::e_SAT,           MOY::e_FEB },
            { L_,    2014,      3,    1,   DOW::e_SAT,           MOY::e_MAR },
            { L_,    2014,      4,    1,   DOW::e_TUE,           MOY::e_APR },
            { L_,    2014,      5,    1,   DOW::e_THU,           MOY::e_MAY },
            { L_,    2014,      6,    1,   DOW::e_SUN,           MOY::e_JUN },
            { L_,    2014,      7,    1,   DOW::e_TUE,           MOY::e_JUL },
            { L_,    2014,      8,    1,   DOW::e_FRI,           MOY::e_AUG },
            { L_,    2014,      9,    1,   DOW::e_MON,           MOY::e_SEP },
            { L_,    2014,     10,    1,   DOW::e_WED,           MOY::e_OCT },
            { L_,    2014,     11,    1,   DOW::e_SAT,           MOY::e_NOV },
            { L_,    2014,     12,    1,   DOW::e_MON,           MOY::e_DEC },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int       LINE    = DATA[ti].d_line;
            const int       YEAR    = DATA[ti].d_year;
            const int       MONTH   = DATA[ti].d_month;
            const int       DAY     = DATA[ti].d_day;
            const DOW::Enum EXP_DOW = DATA[ti].d_expDOW;
            const MOY::Enum EXP_MOY = DATA[ti].d_expMOY;

            if (veryVerbose) {
                T_ P_(LINE) P_(YEAR) P_(MONTH) P_(DAY) P_(EXP_DOW) P(EXP_MOY)
            }


            const Obj X(YEAR, MONTH, DAY);

            if (veryVeryVerbose) {
                T_ T_ P_(X) P_(X.dayOfWeek()) P(X.monthOfYear())
            }

            LOOP_ASSERT(LINE, EXP_DOW == X.dayOfWeek());
            LOOP_ASSERT(LINE, EXP_MOY == X.monthOfYear());
        }

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // RELATIONAL-COMPARISON OPERATORS (<, <=, >, >=)
        //   Ensure that each operator defines the correct relationship between
        //   any two date values.
        //
        // Concerns:
        //: 1 An object 'X' is in relation to an object 'Y' as the
        //:   year/day-of-year representation of 'X' is in relation to the
        //:   year/day-of-year representation of 'Y'.
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
        //:   object values (one per row) in terms of their year/day-of-year
        //:   representation.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1..9)
        //:
        //:   1 Use the 2-argument value constructor to create a 'const'
        //:     object, 'W', having the value from 'R1'.
        //:
        //:   2 Using 'W', verify the anti-reflexive (reflexive) property of
        //:     '<' and '>' ('<=' and '>=') in the presence of aliasing.
        //:     (C-2..5)
        //:
        //:   3 For each row 'R2' in the table of P-2:  (C-1, 6..9)
        //:
        //:     1 Use the 2-argument value constructor to create a 'const'
        //:       object, 'X', having the value from 'R1', and a second 'const'
        //:       object, 'Y', having the value from 'R2'.
        //:
        //:     2 Record, in 'EXP', whether or not an object set to the value
        //:       from 'R1' is expected to compare less than a (distinct)
        //:       object set to the value from 'R2'.
        //:
        //:     3 Using 'X' and 'Y', verify the asymmetric property and
        //:       expected return value for '<'.  (C-6)
        //:
        //:     4 Record, in 'EXP', whether or not an object set to the value
        //:       from 'R1' is expected to compare less or equal to a
        //:       (distinct) object set to the value from 'R2'.
        //:
        //:     5 Using 'X' and 'Y', verify the expected return value for '<='.
        //:       (C-7)
        //:
        //:     6 Record, in 'EXP', whether or not an object set to the value
        //:       from 'R1' is expected to compare greater than a (distinct)
        //:       object set to the value from 'R2'.
        //:
        //:     7 Using 'X' and 'Y', verify the asymmetric property and
        //:       expected return value for '>'.  (C-8)
        //:
        //:     8 Record, in 'EXP', whether or not an object set to the value
        //:       from 'R1' is expected to compare greater than or equal to a
        //:       (distinct) object set to the value from 'R2'.
        //:
        //:     9 Using 'X' and 'Y', verify the expected return value for '>='.
        //:       (C-1, 9)
        //
        // Testing:
        //   bool operator< (const Date& lhs, const Date& rhs);
        //   bool operator<=(const Date& lhs, const Date& rhs);
        //   bool operator>=(const Date& lhs, const Date& rhs);
        //   bool operator> (const Date& lhs, const Date& rhs);
        //   CONCERN: All ctor/manip./free op. ptr./ref. params are 'const'.
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

        if (verbose) cout << "\nTesting operators '<', '<=', '>', '>='."
                          << endl;

        static const struct {
            int d_line;  // source line number
            int d_year;  // year under test
            int d_day;   // day of year under test
        } DATA[] = {
            //LINE   YEAR   DAY
            //----   ----   ---
            { L_,       1,    1 },
            { L_,       1,    2 },
            { L_,       1,  365 },
            { L_,       2,    1 },
            { L_,       4,  366 },
            { L_,       5,    1 },
            { L_,       9,    9 },
            { L_,       9,   10 },
            { L_,      10,    9 },
            { L_,      10,   10 },
            { L_,      99,    1 },
            { L_,      99,   99 },
            { L_,      99,  100 },
            { L_,      99,  365 },
            { L_,     100,    1 },
            { L_,     100,   99 },
            { L_,     100,  100 },
            { L_,     100,  365 },
            { L_,    1000,   99 },
            { L_,    1000,  100 },
            { L_,    9998,  365 },
            { L_,    9999,    1 },
            { L_,    9999,  364 },
            { L_,    9999,  365 },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int ILINE = DATA[ti].d_line;
            const int IYEAR = DATA[ti].d_year;
            const int IDAY  = DATA[ti].d_day;

            if (veryVerbose) { T_ P_(ILINE) P_(IYEAR) P(IDAY) }

            // Ensure an object compares correctly with itself (alias test).
            {
                const Obj W(IYEAR, IDAY);

                LOOP2_ASSERT(ILINE, W, !(W <  W));
                LOOP2_ASSERT(ILINE, W,   W <= W);
                LOOP2_ASSERT(ILINE, W, !(W >  W));
                LOOP2_ASSERT(ILINE, W,   W >= W);
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int JLINE = DATA[tj].d_line;
                const int JYEAR = DATA[tj].d_year;
                const int JDAY  = DATA[tj].d_day;

                if (veryVerbose) { T_ T_ P_(JLINE) P_(JYEAR) P(JDAY) }

                const Obj X(IYEAR, IDAY);
                const Obj Y(JYEAR, JDAY);

                if (veryVeryVerbose) { T_ T_ T_ P_(X) P(Y) }

                // Verify 'operator<'.

                {
                    const bool EXP = (IYEAR  < JYEAR)
                                  || (IYEAR == JYEAR && IDAY < JDAY);

                    LOOP4_ASSERT(ILINE, JLINE, X, Y, EXP == (X < Y));

                    if (EXP) {
                        LOOP4_ASSERT(ILINE, JLINE, Y, X, !(Y < X));
                    }
                }

                // Verify 'operator<='.

                {
                    const bool EXP = (IYEAR  < JYEAR)
                                  || (IYEAR == JYEAR && IDAY <= JDAY);

                    LOOP4_ASSERT(ILINE, JLINE, X, Y, EXP == (X <= Y));
                    LOOP4_ASSERT(ILINE, JLINE, X, Y,
                                 EXP == ((X < Y) ^ (X == Y)));
                }

                // Verify 'operator>'.

                {
                    const bool EXP = (IYEAR  > JYEAR)
                                  || (IYEAR == JYEAR && IDAY > JDAY);

                    LOOP4_ASSERT(ILINE, JLINE, X, Y, EXP == (X > Y));

                    if (EXP) {
                        LOOP4_ASSERT(ILINE, JLINE, Y, X, !(Y > X));
                    }
                }

                // Verify 'operator>='.

                {
                    const bool EXP = (IYEAR  > JYEAR)
                                  || (IYEAR == JYEAR && IDAY >= JDAY);

                    LOOP4_ASSERT(ILINE, JLINE, X, Y, EXP == (X >= Y));
                    LOOP4_ASSERT(ILINE, JLINE, X, Y,
                                 EXP == ((X > Y) ^ (X == Y)));
                }
            }
        }

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // DATE DIFFERENCE OPERATOR
        //   Ensure that the operator calculates the correct (signed) integral
        //   difference between any two dates.
        //
        // Concerns:
        //: 1 The value that is returned represents the (signed) number of days
        //:   between the 'lhs' and 'rhs' date values.
        //:
        //: 2 The operator accepts every combination of argument values.
        //:
        //: 3 The values of the two source objects supplied to the operator
        //:   are not modified.
        //:
        //: 4 Non-modifiable objects can be subtracted (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 5 The operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the address of 'operator-' to initialize a function pointer
        //:   having the appropriate signature and return type for the free
        //:   subtraction operator defined in this component.  (C-4..5)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct date
        //:   object value pairs (one pair per row) in terms of their
        //:   year/day-of-year representations, and the non-negative number of
        //:   days that the second date value is advanced from that of the
        //:   first date value ('DELTA').
        //:
        //: 3 For each row 'R' in the table of P-1:  (C-1..3)
        //:
        //:   1 Use the 2-argument value constructor to create two 'const'
        //:     objects, 'V' and 'W', having the first and second date values
        //:     from 'R', respectively.
        //:
        //:   2 Use the copy constructor to create a 'const' 'Obj', 'VV', from
        //:     'V', and a 'const' 'Obj', 'WW', from 'W'.
        //:
        //:   3 Verify that 'W - V' returns 'DELTA' and that 'V - W' returns
        //:     '-DELTA'.  (C-1..2)
        //:
        //:   4 Use the equality-comparison operator to verify that 'V' and 'W'
        //:     still have the same values as those of 'VV' and 'WW',
        //:     respectively.  (C-3)
        //
        // Testing:
        //   int operator-(const Date& lhs, const Date& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DATE DIFFERENCE OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                "\nAssign the address of the operator to a variable." << endl;
        {
            typedef int (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorSub = bdlt::operator-;

            (void)operatorSub;  // quash potential compiler warnings
        }

        if (verbose) cout << "\nTesting 'operator-(Date, Date)'." << endl;
        {
            const int NUM_DATA                 = ALT_NUM_DATA;
            const AltDataRow (&DATA)[NUM_DATA] = ALT_DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE   = DATA[ti].d_line;
                const int V_YEAR = DATA[ti].d_V_year;
                const int V_DAY  = DATA[ti].d_V_day;
                const int W_YEAR = DATA[ti].d_W_year;
                const int W_DAY  = DATA[ti].d_W_day;
                const int DELTA  = DATA[ti].d_numDays;

                const Obj V(V_YEAR, V_DAY);
                const Obj W(W_YEAR, W_DAY);

                if (veryVerbose) { T_ P_(LINE) P_(V) P_(W) P(DELTA) }

                const Obj VV(V);
                const Obj WW(W);

                LOOP_ASSERT(LINE,  DELTA == W - V);
                LOOP_ASSERT(LINE, -DELTA == V - W);

                LOOP_ASSERT(LINE, VV == V);
                LOOP_ASSERT(LINE, WW == W);
            }
        }

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // ARITHMETIC ASSIGNMENT (+=, -=) AND FREE ARITHMETIC (+, -) OPERATORS
        //   Ensure that the operators produce objects whose values differ from
        //   initial dates by the specified (signed) number of days.
        //
        // Concerns:
        //: 1 Each compound assignment operator can change the value of any
        //:   modifiable target object based on any source 'numDays' value that
        //:   does not violate the method's documented preconditions.
        //:
        //: 2 The reference returned from each compound assignment operator is
        //:   to the target object (i.e., '*this').
        //:
        //: 3 The signatures and return types of the compound assignment
        //:   operators are standard.
        //:
        //: 4 The object returned from the free 'operator+' ('operator-') has
        //:   a date value that represents the sum (difference) of those of the
        //:   two operands.
        //:
        //: 5 Each of the free operators accepts the contractually specified
        //:   range of argument values.
        //:
        //: 6 The value of the source date object supplied to each of the free
        //:   operators is not modified.
        //:
        //: 7 Non-modifiable objects can be added and subtracted by the free
        //:   operators (i.e., objects or references providing only
        //:   non-modifiable access).
        //:
        //: 8 The free operators' signatures and return types are standard.
        //:
        //: 9 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator+=' and 'operator-=' to
        //:   initialize member-function pointers having the appropriate
        //:   signatures and return types for the two compound assignment
        //:   operators defined in this component.  (C-3)
        //:
        //: 2 Use the respective addresses of 'operator+' and 'operator-' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the three non-homogeneous, free binary
        //:   arithmetic operators defined in this component.  (C-7..8)
        //:
        //: 3 Using the table-driven technique, specify a set of distinct date
        //:   object value pairs (one pair per row) in terms of their
        //:   year/day-of-year representations, and the non-negative number of
        //:   days ('NUM_DAYS') that the second date value ('W') is advanced
        //:   from that of the first date value ('V').
        //:
        //: 4 For each row 'R' in the table of P-3:  (C-1..2)
        //:
        //:   1 Use the 2-argument value constructor to create two 'const'
        //:     objects, 'V' and 'W', having the first and second date values
        //:     from 'R', respectively.
        //:
        //:   2 To test 'operator+=':
        //:
        //:     1 Use the copy constructor to create a modifiable object, 'mX',
        //:       from 'V'.
        //:
        //:     2 Use the '+=' compound assignment operator to add 'NUM_DAYS'
        //:       to 'mX' ('mX += NUM_DAYS').
        //:
        //:     3 Verify that the address of the return value is the same as
        //:       that of 'mX'.
        //:
        //:     4 Use the equality-comparison operator to verify that the
        //:       target object, 'mX', now has the same value as that of 'W'.
        //:
        //:     5 Repeat steps similar to those described in P-4.2.1..4 except
        //:       that, this time, create 'mX' from 'W', add '-NUM_DAYS' to
        //:       'mX' ('mX += -NUM_DAYS'), and verify that 'mX' now has the
        //:       same value as that of 'V'.
        //:
        //:   3 To test 'operator-=':  (C-1..2)
        //:
        //:     1 Use the copy constructor to create a modifiable object, 'mX',
        //:       from 'W'.
        //:
        //:     2 Use the '-=' compound assignment operator to subtract
        //:       'NUM_DAYS' from 'mX' ('mX -= NUM_DAYS').
        //:
        //:     3 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-2)
        //:
        //:     4 Use the equality-comparison operator to verify that the
        //:       target object, 'mX', now has the same value as that of 'V'.
        //:
        //:     5 Repeat steps similar to those described in P-4.3.1..4 except
        //:       that, this time, create 'mX' from 'V', subtract '-NUM_DAYS'
        //:       from 'mX' ('mX -= -NUM_DAYS'), and verify that 'mX' now has
        //:       the same value as that of 'W'.  (C-1)
        //:
        //: 5 For each row 'R' in the table of P-3:  (C-4..6)
        //:
        //:   1 Use the 2-argument value constructor to create two 'const'
        //:     objects, 'V' and 'W', having the first and second date values
        //:     from 'R', respectively.
        //:
        //:   2 Use the copy constructor to create a 'const' 'Obj', 'VV', from
        //:     'V', and a 'const' 'Obj', 'WW', from 'W'.
        //:
        //:   3 To test free 'operator+':
        //:
        //:     1 Bind the return values of 'V + NUM_DAYS' and 'NUM_DAYS + V'
        //:       to references 'X' and 'Y', respectively, providing only
        //:       non-modifiable access.
        //:
        //:     2 Use the equality-comparison operator to verify that:
        //:
        //:       1 'X' and 'Y' have the same value as that of 'W'.
        //:
        //:       2 'V' still has the same value as that of 'VV'.
        //:
        //:     3 Repeat steps similar to those described in P-5.3.1..2 except
        //:       that, this time, bind the return values of 'W + -NUM_DAYS'
        //:       and '-NUM_DAYS + W' to 'X' and 'Y', respectively; then verify
        //:       that 'X' and 'Y' have the same value as that of 'V', and that
        //:       'W' still has the same value as that of 'WW'.
        //:
        //:   4 To test free 'operator-':  (C-4..6)
        //:
        //:     1 Bind the return value of 'W - NUM_DAYS' to a reference 'X'
        //:       providing only non-modifiable access.
        //:
        //:     2 Use the equality-comparison operator to verify that:
        //:
        //:       1 'X' has the same value as that of 'V'.
        //:
        //:       2 'W' still has the same value as that of 'WW'.
        //:
        //:     3 Repeat steps similar to those described in P-5.4.1..2 except
        //:       that, this time, bind the return value of 'V - -NUM_DAYS' to
        //:       'X'; then verify that 'X' has the same value as that of 'W',
        //:       and that 'V' still has the same value as that of 'VV'.
        //:       (C-4..6)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid date and 'numDays' values, but not
        //:   triggered for adjacent valid ones (using the 'BSLS_ASSERTTEST_*'
        //:   macros).  (C-9)
        //
        // Testing:
        //   Date& operator+=(int numDays);
        //   Date& operator-=(int numDays);
        //   Date operator+(const Date& date, int numDays);
        //   Date operator+(int numDays, const Date& date);
        //   Date operator-(const Date& date, int numDays);
        //   CONCERN: Precondition violations are detected when enabled.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
       << "ARITHMETIC ASSIGNMENT (+=, -=) AND FREE ARITHMETIC (+, -) OPERATORS"
                          << endl
       << "==================================================================="
                          << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            typedef Obj& (Obj::*memberOperatorPtr)(int);

            typedef Obj (*freeOperatorPtr1)(const Obj&, int);
            typedef Obj (*freeOperatorPtr2)(int, const Obj&);

            // Verify that the signatures and return types are standard.

            memberOperatorPtr operatorAddAssign = &Obj::operator+=;
            memberOperatorPtr operatorSubAssign = &Obj::operator-=;

            freeOperatorPtr1  operatorAdd1      = bdlt::operator+;
            freeOperatorPtr2  operatorAdd2      = bdlt::operator+;
            freeOperatorPtr1  operatorSub       = bdlt::operator-;

            (void)operatorAddAssign;  // quash potential compiler warning
            (void)operatorSubAssign;

            (void)operatorAdd1;
            (void)operatorAdd2;
            (void)operatorSub;
        }

        {
            const int NUM_DATA                 = ALT_NUM_DATA;
            const AltDataRow (&DATA)[NUM_DATA] = ALT_DATA;

            if (verbose) cout << "\nTesting 'operator+='." << endl;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE     = DATA[ti].d_line;
                const int V_YEAR   = DATA[ti].d_V_year;
                const int V_DAY    = DATA[ti].d_V_day;
                const int W_YEAR   = DATA[ti].d_W_year;
                const int W_DAY    = DATA[ti].d_W_day;
                const int NUM_DAYS = DATA[ti].d_numDays;

                const Obj V(V_YEAR, V_DAY);
                const Obj W(W_YEAR, W_DAY);

                if (veryVerbose) { T_ P_(LINE) P_(V) P_(W) P(NUM_DAYS) }

                {
                    Obj mX(V);  const Obj& X = mX;

                    Obj *mR = &(mX += NUM_DAYS);

                    LOOP_ASSERT(LINE,  W == X);
                    LOOP_ASSERT(LINE, mR == &mX);
                }

                {
                    Obj mX(W);  const Obj& X = mX;

                    Obj *mR = &(mX += -NUM_DAYS);

                    LOOP_ASSERT(LINE,  V == X);
                    LOOP_ASSERT(LINE, mR == &mX);
                }
            }

            if (verbose) cout << "\nTesting 'operator-='." << endl;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE     = DATA[ti].d_line;
                const int V_YEAR   = DATA[ti].d_V_year;
                const int V_DAY    = DATA[ti].d_V_day;
                const int W_YEAR   = DATA[ti].d_W_year;
                const int W_DAY    = DATA[ti].d_W_day;
                const int NUM_DAYS = DATA[ti].d_numDays;

                const Obj V(V_YEAR, V_DAY);
                const Obj W(W_YEAR, W_DAY);

                if (veryVerbose) { T_ P_(LINE) P_(V) P_(W) P(NUM_DAYS) }

                {
                    Obj mX(W);  const Obj& X = mX;

                    Obj *mR = &(mX -= NUM_DAYS);

                    LOOP_ASSERT(LINE,  V == X);
                    LOOP_ASSERT(LINE, mR == &mX);
                }

                {
                    Obj mX(V);  const Obj& X = mX;

                    Obj *mR = &(mX -= -NUM_DAYS);

                    LOOP_ASSERT(LINE,  W == X);
                    LOOP_ASSERT(LINE, mR == &mX);
                }
            }

            if (verbose) cout << "\nTesting 'operator+'." << endl;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE     = DATA[ti].d_line;
                const int V_YEAR   = DATA[ti].d_V_year;
                const int V_DAY    = DATA[ti].d_V_day;
                const int W_YEAR   = DATA[ti].d_W_year;
                const int W_DAY    = DATA[ti].d_W_day;
                const int NUM_DAYS = DATA[ti].d_numDays;

                const Obj V(V_YEAR, V_DAY);
                const Obj W(W_YEAR, W_DAY);

                const Obj VV(V);
                const Obj WW(W);

                if (veryVerbose) { T_ P_(LINE) P_(V) P_(W) P(NUM_DAYS) }

                {
                    const Obj& X = V + NUM_DAYS;
                    const Obj& Y = NUM_DAYS + V;

                    LOOP_ASSERT(LINE, W == X);
                    LOOP_ASSERT(LINE, W == Y);
                    LOOP_ASSERT(LINE, V == VV);
                }

                {
                    const Obj& X = W + -NUM_DAYS;
                    const Obj& Y = -NUM_DAYS + W;

                    LOOP_ASSERT(LINE, V == X);
                    LOOP_ASSERT(LINE, V == Y);
                    LOOP_ASSERT(LINE, W == WW);
                }
            }

            if (verbose) cout << "\nTesting 'operator-(Date, int)'." << endl;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE     = DATA[ti].d_line;
                const int V_YEAR   = DATA[ti].d_V_year;
                const int V_DAY    = DATA[ti].d_V_day;
                const int W_YEAR   = DATA[ti].d_W_year;
                const int W_DAY    = DATA[ti].d_W_day;
                const int NUM_DAYS = DATA[ti].d_numDays;

                const Obj V(V_YEAR, V_DAY);
                const Obj W(W_YEAR, W_DAY);

                const Obj VV(V);
                const Obj WW(W);

                if (veryVerbose) { T_ P_(LINE) P_(V) P_(W) P(NUM_DAYS) }

                {
                    const Obj& X = W - NUM_DAYS;

                    LOOP_ASSERT(LINE, V == X);
                    LOOP_ASSERT(LINE, W == WW);
                }

                {
                    const Obj& X = V - -NUM_DAYS;

                    LOOP_ASSERT(LINE, W == X);
                    LOOP_ASSERT(LINE, V == VV);
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (verbose) cout << "\t'operator+='" << endl;
            {
                const Obj V(1, 1);
                {
                    Obj mX(V);  ASSERT_SAFE_PASS(mX +=        0);
                    Obj mY(V);  ASSERT_SAFE_FAIL(mY +=       -1);
                }

                {
                    Obj mX(V);  ASSERT_SAFE_PASS(mX +=  3652058);
                    Obj mY(V);  ASSERT_SAFE_FAIL(mY +=  3652059);
                }

                const Obj W(9999, 365);
                {
                    Obj mX(W);  ASSERT_SAFE_PASS(mX +=        0);
                    Obj mY(W);  ASSERT_SAFE_FAIL(mY +=        1);
                }

                {
                    Obj mX(W);  ASSERT_SAFE_PASS(mX += -3652058);
                    Obj mY(W);  ASSERT_SAFE_FAIL(mY += -3652059);
                }
            }

            if (verbose) cout << "\t'operator-='" << endl;
            {
                const Obj V(1, 1);
                {
                    Obj mX(V);  ASSERT_SAFE_PASS(mX -=        0);
                    Obj mY(V);  ASSERT_SAFE_FAIL(mY -=        1);
                }

                {
                    Obj mX(V);  ASSERT_SAFE_PASS(mX -= -3652058);
                    Obj mY(V);  ASSERT_SAFE_FAIL(mY -= -3652059);
                }

                const Obj W(9999, 365);
                {
                    Obj mX(W);  ASSERT_SAFE_PASS(mX -=        0);
                    Obj mY(W);  ASSERT_SAFE_FAIL(mY -=       -1);
                }

                {
                    Obj mX(W);  ASSERT_SAFE_PASS(mX -=  3652058);
                    Obj mY(W);  ASSERT_SAFE_FAIL(mY -=  3652059);
                }
            }

            if (verbose) cout << "\t'operator+(Date, int)'" << endl;
            {
                const Obj V(1, 1);

                ASSERT_SAFE_PASS(V +        0);
                ASSERT_SAFE_FAIL(V +       -1);

                ASSERT_SAFE_PASS(V +  3652058);
                ASSERT_SAFE_FAIL(V +  3652059);

                const Obj W(9999, 365);

                ASSERT_SAFE_PASS(W +        0);
                ASSERT_SAFE_FAIL(W +        1);

                ASSERT_SAFE_PASS(W + -3652058);
                ASSERT_SAFE_FAIL(W + -3652059);
            }

            if (verbose) cout << "\t'operator+(int, Date)'" << endl;
            {
                const Obj V(1, 1);

                ASSERT_SAFE_PASS(       0 + V);
                ASSERT_SAFE_FAIL(      -1 + V);

                ASSERT_SAFE_PASS( 3652058 + V);
                ASSERT_SAFE_FAIL( 3652059 + V);

                const Obj W(9999, 365);

                ASSERT_SAFE_PASS(       0 + W);
                ASSERT_SAFE_FAIL(       1 + W);

                ASSERT_SAFE_PASS(-3652058 + W);
                ASSERT_SAFE_FAIL(-3652059 + W);
            }

            if (verbose) cout << "\t'operator-(Date, int)'" << endl;
            {
                const Obj V(1, 1);

                ASSERT_SAFE_PASS(V -        0);
                ASSERT_SAFE_FAIL(V -        1);

                ASSERT_SAFE_PASS(V - -3652058);
                ASSERT_SAFE_FAIL(V - -3652059);

                const Obj W(9999, 365);

                ASSERT_SAFE_PASS(W -        0);
                ASSERT_SAFE_FAIL(W -       -1);

                ASSERT_SAFE_PASS(W -  3652058);
                ASSERT_SAFE_FAIL(W -  3652059);
            }
        }

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // INCREMENT AND DECREMENT OPERATORS
        //   Ensure that the operators correctly adjust date values by one day.
        //
        // Concerns:
        //: 1 The member and free '++' ('--') operators increment (decrement)
        //:   the value of the target date object by one day.
        //:
        //: 2 The reference returned from the member operators is to the target
        //:   object (i.e., '*this').
        //:
        //: 3 The date object returned from the free operators has the same
        //:   value as the target object prior to the operation.
        //:
        //: 4 The operators' signatures and return types are standard.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the respective addresses of 'Obj::operator++' and
        //:   'Obj::operator--' to initialize member-function pointers having
        //:   the appropriate signatures and return types for the member
        //:   pre-increment and pre-decrement operators defined in this
        //:   component.
        //:
        //: 2 Use the respective addresses of 'bdlt::operator++' and
        //:   'bdlt::operator--' to initialize function pointers having the
        //:   appropriate signatures and return types for the free
        //:   post-increment and post-decrement operators defined in this
        //:   component.  (C-4)
        //:
        //: 3 Using the table-driven technique, specify a set of distinct date
        //:   object value pairs (one pair per row) in terms of their
        //:   year/day-of-year representations such that the second date value
        //:   is advanced exactly one day from that of the first date value.
        //:
        //: 4 To test the pre-increment (member) operator, for each row 'R' in
        //:   the table of P-3:
        //:
        //:   1 Use the 2-argument value constructor to create two 'const'
        //:     objects, 'V' and 'W', having the first and second date values
        //:     from 'R', respectively.
        //:
        //:   2 Use the copy constructor to create a modifiable object, 'mX',
        //:     from 'V'.
        //:
        //:   3 Apply the pre-increment operator to 'mX'.
        //:
        //:   4 Use the equality-comparison operator to verify that 'mX' now
        //:     has the same value as that of 'W'.
        //:
        //:   5 Verify that the address of the return value is the same as
        //:     that of 'mX'.
        //:
        //: 5 To test the pre-decrement (member) operator, for each row 'R' in
        //:   the table of P-3:  (C-2)
        //:
        //:   1 Use the 2-argument value constructor to create two 'const'
        //:     objects, 'V' and 'W', having the first and second date values
        //:     from 'R', respectively.
        //:
        //:   2 Use the copy constructor to create a modifiable object, 'mX',
        //:     from 'W'.
        //:
        //:   3 Apply the pre-decrement operator to 'mX'.
        //:
        //:   4 Use the equality-comparison operator to verify that 'mX' now
        //:     has the same value as that of 'V'.
        //:
        //:   5 Verify that the address of the return value is the same as
        //:     that of 'mX'.  (C-2)
        //:
        //: 6 To test the post-increment (free) operator, for each row 'R' in
        //:   the table of P-3:
        //:
        //:   1 Use the 2-argument value constructor to create two 'const'
        //:     objects, 'V' and 'W', having the first and second date values
        //:     from 'R', respectively.
        //:
        //:   2 Use the copy constructor to create a modifiable object, 'mX',
        //:     from 'V'.
        //:
        //:   3 Apply the post-increment operator to 'mX', binding the return
        //:     value to a reference, 'Y', providing only non-modifiable
        //:     access.
        //:
        //:   4 Use the equality-comparison operator to verify that 'mX' now
        //:     has the same value as that of 'W', but 'Y' has the same value
        //:     as that of 'V'.
        //:
        //: 7 To test the post-decrement (free) operator, for each row 'R' in
        //:   the table of P-3:  (C-1, 3)
        //:
        //:   1 Use the 2-argument value constructor to create two 'const'
        //:     objects, 'V' and 'W', having the first and second date values
        //:     from 'R', respectively.
        //:
        //:   2 Use the copy constructor to create a modifiable object, 'mX',
        //:     from 'W'.
        //:
        //:   3 Apply the post-decrement operator to 'mX', binding the return
        //:     value to a reference, 'Y', providing only non-modifiable
        //:     access.
        //:
        //:   4 Use the equality-comparison operator to verify that 'mX' now
        //:     has the same value as that of 'V', but 'Y' has the same value
        //:     as that of 'W'.  (C-1, 3)
        //:
        //: 8 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid date values, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-5)
        //
        // Testing:
        //   Date& operator++();
        //   Date& operator--();
        //   Date operator++(Date& date, int);
        //   Date operator--(Date& date, int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "INCREMENT AND DECREMENT OPERATORS" << endl
                          << "=================================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            typedef Obj& (Obj::*memberOperatorPtr)();

            typedef Obj (*freeOperatorPtr)(Obj&, int);

            // Verify that the signatures and return types are standard.

            memberOperatorPtr memberIncrement = &Obj::operator++;
            memberOperatorPtr memberDecrement = &Obj::operator--;

            freeOperatorPtr   freeIncrement   = bdlt::operator++;
            freeOperatorPtr   freeDecrement   = bdlt::operator--;

            (void)memberIncrement;  // quash potential compiler warning
            (void)memberDecrement;

            (void)freeIncrement;
            (void)freeDecrement;
        }

        {
            static const struct {
                int d_line;    // source line number
                int d_V_year;  // V's year
                int d_V_day;   // V's day of year
                int d_W_year;  // W's year
                int d_W_day;   // W's day of year
            } DATA[] = {
                //       <-- V --->    <-- W --->
                //LINE   YEAR   DAY    YEAR   DAY
                //----   ----   ---    ----   ---
                { L_,       1,    1,      1,    2 },
                { L_,       1,  365,      2,    1 },
                { L_,       4,   59,      4,   60 },
                { L_,       4,   60,      4,   61 },
                { L_,       4,  366,      5,    1 },
                { L_,      10,   59,     10,   60 },
                { L_,     100,   90,    100,   91 },
                { L_,     100,  365,    101,    1 },
                { L_,     400,   59,    400,   60 },
                { L_,     400,   60,    400,   61 },
                { L_,     400,  366,    401,    1 },
                { L_,     700,   31,    700,   32 },
                { L_,     800,   60,    800,   61 },
                { L_,     900,   90,    900,   91 },
                { L_,    1000,  120,   1000,  121 },
                { L_,    1100,  151,   1100,  152 },
                { L_,    1200,  182,   1200,  183 },
                { L_,    1300,  212,   1300,  213 },
                { L_,    1400,  243,   1400,  244 },
                { L_,    1500,  273,   1500,  274 },
                { L_,    1600,  305,   1600,  306 },
                { L_,    1700,  334,   1700,  335 },
                { L_,    1800,  365,   1801,    1 },
                { L_,    2000,   59,   2000,   60 },
                { L_,    2001,   59,   2001,   60 },
                { L_,    2004,   59,   2004,   60 },
                { L_,    2100,   59,   2100,   60 },
                { L_,    2400,   59,   2400,   60 },
                { L_,    9998,  365,   9999,    1 },
                { L_,    9999,  364,   9999,  365 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            if (verbose) cout << "\nTesting member 'operator++'." << endl;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE   = DATA[ti].d_line;
                const int V_YEAR = DATA[ti].d_V_year;
                const int V_DAY  = DATA[ti].d_V_day;
                const int W_YEAR = DATA[ti].d_W_year;
                const int W_DAY  = DATA[ti].d_W_day;

                const Obj V(V_YEAR, V_DAY);
                const Obj W(W_YEAR, W_DAY);

                LOOP_ASSERT(LINE, V != W);

                if (veryVerbose) { T_ P_(LINE) P_(V) P(W) }

                Obj mX(V);  const Obj& X = mX;

                Obj *mR = &(++mX);

                if (veryVeryVerbose) { T_ T_ P(X) }

                LOOP_ASSERT(LINE,  W == X);
                LOOP_ASSERT(LINE, mR == &mX);
            }

            if (verbose) cout << "\nTesting member 'operator--'." << endl;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE   = DATA[ti].d_line;
                const int V_YEAR = DATA[ti].d_V_year;
                const int V_DAY  = DATA[ti].d_V_day;
                const int W_YEAR = DATA[ti].d_W_year;
                const int W_DAY  = DATA[ti].d_W_day;

                const Obj V(V_YEAR, V_DAY);
                const Obj W(W_YEAR, W_DAY);

                LOOP_ASSERT(LINE, V != W);

                if (veryVerbose) { T_ P_(LINE) P_(V) P(W) }

                Obj mX(W);  const Obj& X = mX;

                Obj *mR = &(--mX);

                if (veryVeryVerbose) { T_ T_ P(X) }

                LOOP_ASSERT(LINE,  V == X);
                LOOP_ASSERT(LINE, mR == &mX);
            }

            if (verbose) cout << "\nTesting free 'operator++'." << endl;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE   = DATA[ti].d_line;
                const int V_YEAR = DATA[ti].d_V_year;
                const int V_DAY  = DATA[ti].d_V_day;
                const int W_YEAR = DATA[ti].d_W_year;
                const int W_DAY  = DATA[ti].d_W_day;

                const Obj V(V_YEAR, V_DAY);
                const Obj W(W_YEAR, W_DAY);

                LOOP_ASSERT(LINE, V != W);

                if (veryVerbose) { T_ P_(LINE) P_(V) P(W) }

                Obj mX(V);  const Obj& X = mX;

                const Obj& Y = mX++;

                if (veryVeryVerbose) { T_ T_ P_(X) P(Y) }

                LOOP_ASSERT(LINE, W == X);
                LOOP_ASSERT(LINE, V == Y);
            }

            if (verbose) cout << "\nTesting free 'operator--'." << endl;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE   = DATA[ti].d_line;
                const int V_YEAR = DATA[ti].d_V_year;
                const int V_DAY  = DATA[ti].d_V_day;
                const int W_YEAR = DATA[ti].d_W_year;
                const int W_DAY  = DATA[ti].d_W_day;

                const Obj V(V_YEAR, V_DAY);
                const Obj W(W_YEAR, W_DAY);

                LOOP_ASSERT(LINE, V != W);

                if (veryVerbose) { T_ P_(LINE) P_(V) P(W) }

                Obj mX(W);  const Obj& X = mX;

                const Obj& Y = mX--;

                if (veryVeryVerbose) { T_ T_ P_(X) P(Y) }

                LOOP_ASSERT(LINE, V == X);
                LOOP_ASSERT(LINE, W == Y);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (verbose) cout << "\tMember 'operator++'" << endl;
            {
                const Obj V(9999, 364);
                const Obj W(9999, 365);

                Obj mX(V);  const Obj& X = mX;
                Obj mY;     const Obj& Y = mY;

                ASSERT_SAFE_PASS(mY = ++mX);
                ASSERT(W == X);
                ASSERT(W == Y);

                ASSERT_SAFE_FAIL(mY = ++mX);
                ASSERT(W == X);
                ASSERT(W == Y);
            }

            if (verbose) cout << "\tMember 'operator--'" << endl;
            {
                const Obj V(1, 1);
                const Obj W(1, 2);

                Obj mX(W);  const Obj& X = mX;
                Obj mY;     const Obj& Y = mY;

                ASSERT_SAFE_PASS(mY = --mX);
                ASSERT(V == X);
                ASSERT(V == Y);

                ASSERT_SAFE_FAIL(mY = --mX);
                ASSERT(V == X);
                ASSERT(V == Y);
            }

            if (verbose) cout << "\tFree 'operator++'" << endl;
            {
                const Obj V(9999, 364);
                const Obj W(9999, 365);

                Obj mX(V);  const Obj& X = mX;
                Obj mY;     const Obj& Y = mY;

                ASSERT_SAFE_PASS(mY = mX++);
                ASSERT(W == X);
                ASSERT(V == Y);

                ASSERT_SAFE_FAIL(mY = mX++);
                ASSERT(W == X);
                ASSERT(V == Y);
            }

            if (verbose) cout << "\tFree 'operator--'" << endl;
            {
                const Obj V(1, 1);
                const Obj W(1, 2);

                Obj mX(W);  const Obj& X = mX;
                Obj mY;     const Obj& Y = mY;

                ASSERT_SAFE_PASS(mY = mX--);
                ASSERT(V == X);
                ASSERT(W == Y);

                ASSERT_SAFE_FAIL(mY = mX--);
                ASSERT(V == X);
                ASSERT(W == Y);
            }
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // 'isValid*' METHODS AND CONDITIONAL SETTERS
        //   Ensure that the methods correctly discriminate between valid and
        //   invalid date representations.
        //
        // Concerns:
        //: 1 The 'isValidYearDay' method correctly categorizes every
        //:   '(year, dayOfYear)' pair as either a valid or an invalid
        //:   year/day-of-year date.
        //:
        //: 2 The 'isValidYearMonthDay' method correctly categorizes every
        //:   '(year, month, day)' triple as either a valid or an invalid
        //:   year/month/day date.
        //:
        //: 3 'setYearDayIfValid' can set an object to have any valid date
        //:   value.
        //:
        //: 4 'setYearDayIfValid' is not affected by the state of the object on
        //:   entry.
        //:
        //: 5 'setYearDayIfValid' has no effect on the object if the supplied
        //:   '(year, dayOfYear)' pair is not a valid year/day-of-year date.
        //:
        //: 6 'setYearDayIfValid' returns 0 on success, and a non-zero value on
        //:   failure.
        //:
        //: 7 'setYearMonthDayIfValid' can set an object to have any valid date
        //:   value.
        //:
        //: 8 'setYearMonthDayIfValid' is not affected by the state of the
        //:   object on entry.
        //:
        //: 9 'setYearMonthDayIfValid' has no effect on the object if the
        //:   supplied (year, month, day) triple is not a valid year/month/day
        //:   date.
        //:
        //:10 'setYearMonthDayIfValid' returns 0 on success, and a non-zero
        //:   value on failure.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   *candidate* year/day-of-year date representations, and a flag
        //:   value indicating whether the year/day-of-year values represent a
        //:   valid date object.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1, 3..6)
        //:
        //:   1 Verify that the 'isValidYearDay' method, when invoked on the
        //:     year/day-of-year values from 'R', returns the expected value.
        //:     (C-1)
        //:
        //:   2 Use the 2-argument value constructor to create a modifiable
        //:     object, 'mX', having a value distinct from any of the (valid)
        //:     values in 'R'.
        //:
        //:   3 Verify that the 'setYearDayIfValid' method, when invoked on
        //:     'mX' and passed the year/day-of-year values from 'R', returns
        //:     the expected value.  (C-6)
        //:
        //:   4 Verify, using the equality-comparison operator, that 'mX' has
        //:     the expected value, i.e., 'mX' has the value from 'R' when
        //:     'setYearDayIfValid' returns 0, and its value is unchanged
        //:     otherwise.  (C-3..5)
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   *candidate* year/month/day date representations, and a flag
        //:   value indicating whether the year/month/day values represent a
        //:   valid date object.
        //:
        //: 4 For each row 'R' in the table of P-3:  (C-2, 7..10)
        //:
        //:   1 Verify that the 'isValidYearMonthDay' method, when invoked on
        //:     the year/month/day values from 'R', returns the expected value.
        //:     (C-2)
        //:
        //:   2 Use the 3-argument value constructor to create a modifiable
        //:     object, 'mX', having a value distinct from any of the (valid)
        //:     values in 'R'.
        //:
        //:   3 Verify that the 'setYearMonthDayIfValid' method, when invoked
        //:     on 'mX' and passed the year/month/day values from 'R', returns
        //:     the expected value.  (C-10)
        //:
        //:   4 Verify, using the equality-comparison operator, that 'mX' has
        //:     the expected value, i.e., 'mX' has the value from 'R' when
        //:     'setYearMonthDayIfValid' returns 0, and its value is unchanged
        //:     otherwise.  (C-7..9)
        //
        // Testing:
        //   static bool isValidYearDay(int year, int dayOfYear);
        //   static bool isValidYearMonthDay(int year, int month, int day);
        //   int setYearDayIfValid(int year, int dayOfYear);
        //   int setYearMonthDayIfValid(int year, int month, int day);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'isValid*' METHODS AND CONDITIONAL SETTERS"
                          << endl
                          << "=========================================="
                          << endl;

        if (verbose)
            cout << "\nTesting 'isValidYearDay(year, dayOfYear)'"
                    " and 'setYearDayIfValid'."
                 << endl;
        {
            static const struct {
                int d_line;  // source line number
                int d_year;  // year under test
                int d_day;   // day under test
                int d_exp;   // expected value
            } DATA[] = {
                //LINE     YEAR      DAY      EXP
                //----   -------   -------    ---
                { L_,    INT_MIN,  INT_MIN,     0 },
                { L_,    INT_MIN,        1,     0 },
                { L_,         -1,        1,     0 },

                { L_,          0,        0,     0 },
                { L_,          0,        1,     0 },
                { L_,          0,      365,     0 },

                { L_,          1,  INT_MIN,     0 },
                { L_,          1,       -1,     0 },
                { L_,          1,        0,     0 },
                { L_,          1,        1,     1 },
                { L_,          1,      365,     1 },
                { L_,          1,      366,     0 },
                { L_,          1,  INT_MAX,     0 },

                { L_,          2,        0,     0 },
                { L_,          2,        1,     1 },
                { L_,          2,      365,     1 },
                { L_,          2,      366,     0 },

                { L_,          4,        0,     0 },
                { L_,          4,        1,     1 },
                { L_,          4,      366,     1 },
                { L_,          4,      367,     0 },

                { L_,        100,        0,     0 },
                { L_,        100,        1,     1 },
                { L_,        100,      365,     1 },
                { L_,        100,      366,     0 },

                { L_,        400,        0,     0 },
                { L_,        400,        1,     1 },
                { L_,        400,      366,     1 },
                { L_,        400,      367,     0 },

                { L_,       1000,        0,     0 },
                { L_,       1000,        1,     1 },
                { L_,       1000,      365,     1 },
                { L_,       1000,      366,     0 },

                { L_,       9999,  INT_MIN,     0 },
                { L_,       9999,        0,     0 },
                { L_,       9999,        1,     1 },
                { L_,       9999,      365,     1 },
                { L_,       9999,      366,     0 },
                { L_,       9999,  INT_MAX,     0 },

                { L_,      10000,        1,     0 },
                { L_,      10000,      365,     0 },

                { L_,    INT_MAX,        1,     0 },
                { L_,    INT_MAX,  INT_MAX,     0 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE = DATA[ti].d_line;
                const int YEAR = DATA[ti].d_year;
                const int DAY  = DATA[ti].d_day;
                const int EXP  = DATA[ti].d_exp;

                if (veryVerbose) { T_ P_(LINE) P_(YEAR) P_(DAY) P(EXP) }

                LOOP_ASSERT(LINE, EXP == Obj::isValidYearDay(YEAR, DAY));

                Obj mX(1133, 275);  const Obj& X = mX;

                const Obj W(X);  // control

                if (1 == EXP) {
                    const Obj V(YEAR, DAY);

                    LOOP_ASSERT(LINE, 0 == mX.setYearDayIfValid(YEAR, DAY));
                    LOOP_ASSERT(LINE, V == X);

                    if (veryVeryVerbose) { T_ T_ P_(V) P(X) }
                }
                else {
                    LOOP_ASSERT(LINE, 0 != mX.setYearDayIfValid(YEAR, DAY));
                    LOOP_ASSERT(LINE, W == X);

                    if (veryVeryVerbose) { T_ T_ P_(W) P(X) }
                }
            }
        }

        if (verbose)
            cout << "\nTesting 'isValidYearMonthDay(year, month, day)'"
                    " and 'setYearMonthDayIfValid'."
                 << endl;
        {
            static const struct {
                int d_line;   // source line number
                int d_year;   // year under test
                int d_month;  // month under test
                int d_day;    // day under test
                int d_exp;    // expected value
            } DATA[] = {
                //LINE     YEAR     MONTH      DAY      EXP
                //----   -------   -------   -------    ---
                { L_,    INT_MIN,        1,        1,     0 },
                { L_,         -1,        1,        1,     0 },
                { L_,          0,        1,        1,     0 },

                { L_,          1,  INT_MIN,        1,     0 },
                { L_,          1,       -1,        1,     0 },
                { L_,          1,        0,        1,     0 },

                { L_,          1,        1,  INT_MIN,     0 },
                { L_,          1,        1,       -1,     0 },
                { L_,          1,        1,        0,     0 },

                { L_,          1,        1,        1,     1 },
                { L_,          1,        1,       31,     1 },
                { L_,          1,        1,  INT_MAX,     0 },
                { L_,          1,        2,       28,     1 },
                { L_,          1,        2,       29,     0 },
                { L_,          1,       12,       31,     1 },
                { L_,          1,       12,       32,     0 },
                { L_,          1,       13,        1,     0 },
                { L_,          1,  INT_MAX,        1,     0 },

                { L_,          4,        2,       28,     1 },
                { L_,          4,        2,       29,     1 },
                { L_,          4,        2,       30,     0 },

                { L_,        100,        2,       28,     1 },
                { L_,        100,        2,       29,     0 },

                { L_,        400,        2,       28,     1 },
                { L_,        400,        2,       29,     1 },
                { L_,        400,        2,       30,     0 },

                { L_,       1000,        2,       28,     1 },
                { L_,       1000,        2,       29,     0 },

                { L_,       2003,        1,       31,     1 },
                { L_,       2003,        1,       32,     0 },
                { L_,       2003,        2,       28,     1 },
                { L_,       2003,        2,       29,     0 },
                { L_,       2003,        3,       31,     1 },
                { L_,       2003,        3,       32,     0 },
                { L_,       2003,        4,       30,     1 },
                { L_,       2003,        4,       31,     0 },
                { L_,       2003,        5,       31,     1 },
                { L_,       2003,        5,       32,     0 },
                { L_,       2003,        6,       30,     1 },
                { L_,       2003,        6,       31,     0 },
                { L_,       2003,        7,       31,     1 },
                { L_,       2003,        7,       32,     0 },
                { L_,       2003,        8,       31,     1 },
                { L_,       2003,        8,       32,     0 },
                { L_,       2003,        9,       30,     1 },
                { L_,       2003,        9,       31,     0 },
                { L_,       2003,       10,       31,     1 },
                { L_,       2003,       10,       32,     0 },
                { L_,       2003,       11,       30,     1 },
                { L_,       2003,       11,       31,     0 },
                { L_,       2003,       12,       31,     1 },
                { L_,       2003,       12,       32,     0 },

                { L_,       2004,        1,       31,     1 },
                { L_,       2004,        1,       32,     0 },
                { L_,       2004,        2,       29,     1 },
                { L_,       2004,        2,       30,     0 },
                { L_,       2004,        3,       31,     1 },
                { L_,       2004,        3,       32,     0 },
                { L_,       2004,        4,       30,     1 },
                { L_,       2004,        4,       31,     0 },
                { L_,       2004,        5,       31,     1 },
                { L_,       2004,        5,       32,     0 },
                { L_,       2004,        6,       30,     1 },
                { L_,       2004,        6,       31,     0 },
                { L_,       2004,        7,       31,     1 },
                { L_,       2004,        7,       32,     0 },
                { L_,       2004,        8,       31,     1 },
                { L_,       2004,        8,       32,     0 },
                { L_,       2004,        9,       30,     1 },
                { L_,       2004,        9,       31,     0 },
                { L_,       2004,       10,       31,     1 },
                { L_,       2004,       10,       32,     0 },
                { L_,       2004,       11,       30,     1 },
                { L_,       2004,       11,       31,     0 },
                { L_,       2004,       12,       31,     1 },
                { L_,       2004,       12,       32,     0 },

                { L_,       9999,        0,        1,     0 },
                { L_,       9999,        1,        0,     0 },
                { L_,       9999,        1,        1,     1 },
                { L_,       9999,        2,       28,     1 },
                { L_,       9999,        2,       29,     0 },
                { L_,       9999,       12,       31,     1 },
                { L_,       9999,       12,       32,     0 },
                { L_,       9999,       13,       30,     0 },

                { L_,      10000,        1,        1,     0 },
                { L_,    INT_MAX,        1,        1,     0 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_line;
                const int YEAR  = DATA[ti].d_year;
                const int MONTH = DATA[ti].d_month;
                const int DAY   = DATA[ti].d_day;
                const int EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    T_ P_(LINE) P_(YEAR) P_(MONTH) P_(DAY) P(EXP)
                }

                LOOP_ASSERT(LINE,
                            EXP == Obj::isValidYearMonthDay(YEAR, MONTH, DAY));

                Obj mX(1133, 10, 2);  const Obj& X = mX;

                const Obj W(X);  // control

                if (1 == EXP) {
                    const Obj V(YEAR, MONTH, DAY);

                    LOOP_ASSERT(LINE, 0 == mX.setYearMonthDayIfValid(YEAR,
                                                                     MONTH,
                                                                     DAY));
                    LOOP_ASSERT(LINE, V == X);

                    if (veryVeryVerbose) { T_ T_ P_(V) P(X) }
                }
                else {
                    LOOP_ASSERT(LINE, 0 != mX.setYearMonthDayIfValid(YEAR,
                                                                     MONTH,
                                                                     DAY));
                    LOOP_ASSERT(LINE, W == X);

                    if (veryVeryVerbose) { T_ T_ P_(W) P(X) }
                }
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // 2-ARGUMENT VALUE CTOR AND METHODS
        //   Ensure that we can put an object into any valid initial state and
        //   that the accessors properly interpret object state.
        //
        // Concerns:
        //: 1 The 2-argument value constructor can create an object to have any
        //:   value that does not violate the method's documented
        //:   preconditions.
        //:
        //: 2 The 'setYearDay' method can set an object to have any value that
        //:   does not violate the method's documented preconditions.
        //:
        //: 3 'setYearDay' is not affected by the state of the object on entry.
        //:
        //: 4 The 'dayOfYear' method returns the value of the day-of-year
        //:   attribute of the date.
        //:
        //: 5 The 'getYearDay' method returns the values of the
        //:   year/day-of-year attributes of the date.
        //:
        //: 6 The 'dayOfYear' and 'getYearDay' accessor methods are declared
        //:   'const'.
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their (equivalent)
        //:   year/day-of-year and year/month/day representations.
        //:
        //: 2 For each row 'R1' in the table of P-1:  (C-2..4)
        //:
        //:   1 Use the 3-argument value constructor to create a 'const'
        //:     object, 'W', having the value specified by the year/month/day
        //:     representation in 'R1'.
        //:
        //:   2 Create an object 'X' using the default constructor.
        //:
        //:   3 Use the 'setYearDay' manipulator to set 'X' to the value
        //:     specified by the year/day-of-year representation in 'R1'.
        //:
        //:   4 Use the equality-comparison operator to verify that 'X' has the
        //:     same value as that of 'W'.  (C-2)
        //:
        //:   5 Verify that the 'year' and 'dayOfYear' accessors applied to 'X'
        //:     return the expected values.  (C-4)
        //:
        //:   6 For each row 'R2' in the table of P-1:  (C-3)
        //:
        //:     1 Use the 3-argument value constructor to create a modifiable
        //:       object, 'mY', having the value specified by the
        //:       year/month/day representation in 'R2'.
        //:
        //:     2 Use 'setYearDay' to set 'mY' to the value specified by the
        //:       year/day-of-year representation in 'R1'.
        //:
        //:     3 Use the equality-comparison operator to verify that 'mY' has
        //:       the same value as that of 'W'.  (C-3)
        //:
        //:     4 Verify that the 'year' and 'dayOfYear' accessors applied to
        //:       'mY' return the expected values.
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their year/day-of-year
        //:   representation.
        //:
        //: 4 For each row 'R' in the table of P-3:  (C-1, 5..6)
        //:
        //:   1 Create an object 'W' using the default constructor, then use
        //:     'setYearDay' to set 'W' to the value in 'R'.
        //:
        //:   2 Also use the 2-argument value constructor to create a 'const'
        //:     object, 'X', having the value from 'R'.
        //:
        //:   3 Use the equality-comparison operator to verify that 'X' has the
        //:     same value as that of 'W'.  (C-1)
        //:
        //:   4 Verify that 'getYearDay', invoked on 'X', returns the same two
        //:     values as are individually returned by the 'year' and
        //:     'dayOfYear' accessors.  (C-5..6)
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid year/day-of-year attribute values, but not
        //:   triggered for adjacent valid ones (using the 'BSLS_ASSERTTEST_*'
        //:   macros).  (C-7)
        //
        // Testing:
        //   Date(int year, int dayOfYear);
        //   void setYearDay(int year, int dayOfYear);
        //   int dayOfYear() const;
        //   void getYearDay(int *year, int *dayOfYear) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "2-ARGUMENT VALUE CTOR AND METHODS" << endl
                          << "=================================" << endl;

        if (verbose) cout << "\nTesting 'setYearDay' and 'dayOfYear'." << endl;
        {
            static const struct {
                int d_line;        // source line number
                int d_year;        // year under test
                int d_day;         // day (of year) under test
                int d_month;       // month under test
                int d_dayOfMonth;  // day of month under test
            } DATA[] = {
                //LINE   YEAR   DAY   MONTH   DAY OF MONTH
                //----   ----   ---   -----   ------------
                { L_,       1,    1,      1,        1 },
                { L_,       1,    2,      1,        2 },
                { L_,       1,   32,      2,        1 },
                { L_,       2,    1,      1,        1 },
                { L_,      10,   95,      4,        5 },
                { L_,      10,  284,     10,       11 },
                { L_,     100,  158,      6,        7 },
                { L_,     100,  316,     11,       12 },
                { L_,    1000,  221,      8,        9 },
                { L_,    1100,   31,      1,       31 },
                { L_,    1200,   60,      2,       29 },
                { L_,    1300,   90,      3,       31 },
                { L_,    1400,  120,      4,       30 },
                { L_,    1500,  151,      5,       31 },
                { L_,    1600,  182,      6,       30 },
                { L_,    1700,  212,      7,       31 },
                { L_,    1800,  243,      8,       31 },
                { L_,    1900,  273,      9,       30 },
                { L_,    2000,  305,     10,       31 },
                { L_,    2100,  334,     11,       30 },
                { L_,    2200,  365,     12,       31 },
                { L_,    9999,    1,      1,        1 },
                { L_,    9999,   59,      2,       28 },
                { L_,    9999,  334,     11,       30 },
                { L_,    9999,  364,     12,       30 },
                { L_,    9999,  365,     12,       31 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int ILINE       = DATA[ti].d_line;
                const int IYEAR       = DATA[ti].d_year;
                const int IDAY        = DATA[ti].d_day;
                const int IMONTH      = DATA[ti].d_month;
                const int IDAYOFMONTH = DATA[ti].d_dayOfMonth;

                if (veryVerbose) {
                    T_ P_(ILINE) P_(IYEAR) P_(IDAY) P_(IMONTH) P(IDAYOFMONTH)
                }

                const Obj W(IYEAR, IMONTH, IDAYOFMONTH);

                Obj mX;  const Obj& X = mX;
                mX.setYearDay(IYEAR, IDAY);

                if (veryVeryVerbose) { T_ T_ P_(W) P(X) }

                LOOP_ASSERT(ILINE, W     == X);
                LOOP_ASSERT(ILINE, IYEAR == X.year());
                LOOP_ASSERT(ILINE, IDAY  == X.dayOfYear());

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int JLINE       = DATA[ti].d_line;
                    const int JYEAR       = DATA[ti].d_year;
                    const int JDAY        = DATA[ti].d_day;
                    const int JMONTH      = DATA[ti].d_month;
                    const int JDAYOFMONTH = DATA[ti].d_dayOfMonth;

                    if (veryVerbose) {
                        T_ T_ P_(JLINE)
                        P_(JYEAR) P_(JDAY) P_(JMONTH) P(JDAYOFMONTH)
                    }

                    Obj mY(JYEAR, JMONTH, JDAYOFMONTH);  const Obj& Y = mY;

                    if (veryVeryVerbose) { T_ T_ T_ P_(Y) }

                    mY.setYearDay(IYEAR, IDAY);

                    if (veryVeryVerbose) { T_ T_ T_ P(Y) }

                    LOOP_ASSERT(ILINE, W     == Y);
                    LOOP_ASSERT(ILINE, IYEAR == Y.year());
                    LOOP_ASSERT(ILINE, IDAY  == Y.dayOfYear());
                }
            }
        }

        if (verbose)
            cout << "\nTesting 'Date(year, dayOfYear)' and 'getYearDay'."
                 << endl;
        {
            static const struct {
                int d_line;  // source line number
                int d_year;  // year under test
                int d_day;   // day-of-year under test
            } DATA[] = {
                //LINE   YEAR   DAY
                //----   ----   ---
                { L_,       1,    1 },
                { L_,       1,    2 },
                { L_,       1,   32 },
                { L_,       2,    1 },
                { L_,      10,   95 },
                { L_,      10,  284 },
                { L_,     100,  158 },
                { L_,     100,  316 },
                { L_,    1000,  221 },
                { L_,    1100,   31 },
                { L_,    1200,   60 },
                { L_,    1300,   90 },
                { L_,    1400,  120 },
                { L_,    1500,  151 },
                { L_,    1600,  182 },
                { L_,    1700,  212 },
                { L_,    1800,  243 },
                { L_,    1900,  273 },
                { L_,    2000,  305 },
                { L_,    2100,  334 },
                { L_,    2200,  365 },
                { L_,    9999,    1 },
                { L_,    9999,   59 },
                { L_,    9999,  334 },
                { L_,    9999,  364 },
                { L_,    9999,  365 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE = DATA[ti].d_line;
                const int YEAR = DATA[ti].d_year;
                const int DAY  = DATA[ti].d_day;

                if (veryVerbose) { T_ P_(LINE) P_(YEAR) P(DAY) }

                Obj mW;  const Obj& W = mW;
                mW.setYearDay(YEAR, DAY);

                const Obj X(YEAR, DAY);

                if (veryVeryVerbose) { T_ T_ P_(W) P(X) }

                LOOP_ASSERT(LINE, W == X);

                int y = -1, d = -1;
                X.getYearDay(&y, &d);

                LOOP_ASSERT(LINE, y == X.year());
                LOOP_ASSERT(LINE, d == X.dayOfYear());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (verbose) cout << "\t'Date(year, dayOfYear)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj(      1,       1));

                ASSERT_SAFE_FAIL_RAW(Obj(      0,       1));
                ASSERT_SAFE_FAIL_RAW(Obj(     -1,       1));
                ASSERT_SAFE_FAIL_RAW(Obj(INT_MIN,       1));

                ASSERT_SAFE_FAIL_RAW(Obj(      1,       0));
                ASSERT_SAFE_FAIL_RAW(Obj(      1,      -1));
                ASSERT_SAFE_FAIL_RAW(Obj(      1, INT_MIN));

                ASSERT_SAFE_PASS_RAW(Obj(   9999,     365));

                ASSERT_SAFE_FAIL_RAW(Obj(  10000,     365));
                ASSERT_SAFE_FAIL_RAW(Obj(INT_MAX,     365));

                ASSERT_SAFE_FAIL_RAW(Obj(   9999,     366));
                ASSERT_SAFE_FAIL_RAW(Obj(   9999, INT_MAX));
            }

            if (verbose) cout << "\t'setYearDay'" << endl;
            {
                Obj mX(1827, 85);  const Obj& X = mX;

                ASSERT_SAFE_PASS(mX.setYearDay(      1,       1));
                ASSERT(1 == X.year());
                ASSERT(1 == X.dayOfYear());

                ASSERT_SAFE_FAIL(mX.setYearDay(      0,       1));
                ASSERT_SAFE_FAIL(mX.setYearDay(     -1,       1));
                ASSERT_SAFE_FAIL(mX.setYearDay(INT_MIN,       1));

                ASSERT_SAFE_FAIL(mX.setYearDay(      1,       0));
                ASSERT_SAFE_FAIL(mX.setYearDay(      1,      -1));
                ASSERT_SAFE_FAIL(mX.setYearDay(      1, INT_MIN));

                ASSERT(1 == X.year());
                ASSERT(1 == X.dayOfYear());

                ASSERT_SAFE_PASS(mX.setYearDay(   9999,     365));
                ASSERT(9999 == X.year());
                ASSERT( 365 == X.dayOfYear());

                ASSERT_SAFE_FAIL(mX.setYearDay(  10000,     365));
                ASSERT_SAFE_FAIL(mX.setYearDay(INT_MAX,     365));

                ASSERT_SAFE_FAIL(mX.setYearDay(   9999,     366));
                ASSERT_SAFE_FAIL(mX.setYearDay(   9999, INT_MAX));

                ASSERT(9999 == X.year());
                ASSERT( 365 == X.dayOfYear());
            }

            if (verbose) cout << "\t'getYearDay'" << endl;
            {
                int y = -1, d = -1;
                const Obj X(1827, 85);

                ASSERT_SAFE_PASS(X.getYearDay(&y, &d));
                ASSERT(1827 == y);
                ASSERT(  85 == d);

                y = d = -1;

                ASSERT_SAFE_FAIL(X.getYearDay( 0, &d));
                ASSERT_SAFE_FAIL(X.getYearDay(&y,  0));
                ASSERT(  -1 == y);
                ASSERT(  -1 == d);
            }
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR AND 3-ARGUMENT SETTER & GETTER
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value), that we can use the
        //   3-argument setter to put an object into any state, and that the
        //   3-argument getter properly interprets object state.
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //:
        //: 2 The 'setYearMonthDay' method can set an object to have any value
        //:   that does not violate the method's documented preconditions.
        //:
        //: 3 'setYearMonthDay' is not affected by the state of the object on
        //:   entry.
        //:
        //: 4 The 'getYearMonthDay' method returns the values of the
        //:   year/month/day attributes of the object.
        //:
        //: 5 The 'getYearMonthDay' accessor method is declared 'const'.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create an object using the default constructor.  Verify, using
        //:   the 'year', 'month', and 'day' accessors, that the resulting
        //:   object has a value of 0001/01/01.  (C-1)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their year/month/day
        //:   representation.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-2..5)
        //:
        //:   1 Use the 3-argument value constructor to create a modifiable
        //:     object, 'mX', from 'R1'.
        //:
        //:   2 For each row 'R2' in the table of P-2:  (C-2..5)
        //:
        //:     1 Use the 3-argument value constructor to create a 'const'
        //:       object, 'W', having the value from 'R2'.
        //:
        //:     2 Use 'setYearMonthDay' to set 'mX' to the value specified
        //:       in 'R2'.
        //:
        //:     3 Use the equality-comparison operator to verify that 'mX' now
        //:       has the same value as that of 'W'.  (C-2..3)
        //:
        //:     4 Verify that 'getYearMonthDay', invoked on a reference
        //:       providing only 'const' access to 'mX', returns the same three
        //:       values as are individually returned by the 'year', 'month',
        //:       and 'day' basic accessors.  (C-4..5)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid year/month/day attribute values, but not
        //:   triggered for adjacent valid ones (using the 'BSLS_ASSERTTEST_*'
        //:   macros).  (C-6)
        //
        // Testing:
        //   Date();
        //   void setYearMonthDay(int year, int month, int day);
        //   void getYearMonthDay(int *year, int *month, int *day) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DEFAULT CTOR AND 3-ARGUMENT SETTER & GETTER"
                          << endl
                          << "==========================================="
                          << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;
        {
            const Obj X;

            if (veryVerbose) { T_ P(X) }

            ASSERT(1 == X.year());
            ASSERT(1 == X.month());
            ASSERT(1 == X.day());
        }

        if (verbose)
            cout << "\nTesting 'setYearMonthDay' and 'getYearMonthDay'."
                 << endl;
        {
            if (verbose) cout << "\nUse a table of distinct object values."
                              << endl;

            const int NUM_DATA                     = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int ILINE  = DATA[ti].d_line;
                const int IYEAR  = DATA[ti].d_year;
                const int IMONTH = DATA[ti].d_month;
                const int IDAY   = DATA[ti].d_day;

                if (veryVerbose) { T_ P_(ILINE) P_(IYEAR) P_(IMONTH) P(IDAY) }

                Obj mX(IYEAR, IMONTH, IDAY);  const Obj& X = mX;

                if (veryVerbose) { T_ T_ P(X) }

                LOOP_ASSERT(ILINE, IYEAR  == X.year());
                LOOP_ASSERT(ILINE, IMONTH == X.month());
                LOOP_ASSERT(ILINE, IDAY   == X.day());

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int JLINE  = DATA[tj].d_line;
                    const int JYEAR  = DATA[tj].d_year;
                    const int JMONTH = DATA[tj].d_month;
                    const int JDAY   = DATA[tj].d_day;

                    if (veryVerbose) {
                        T_ T_ P_(JLINE) P_(JYEAR) P_(JMONTH) P(JDAY)
                    }

                    const Obj W(JYEAR, JMONTH, JDAY);

                    mX.setYearMonthDay(JYEAR, JMONTH, JDAY);

                    if (veryVeryVerbose) { T_ T_ P_(W) P(X) }

                    LOOP_ASSERT(JLINE, W == X);

                    int y = -1, m = -1, d = -1;
                    X.getYearMonthDay(&y, &m, &d);

                    LOOP_ASSERT(JLINE, y == X.year());
                    LOOP_ASSERT(JLINE, m == X.month());
                    LOOP_ASSERT(JLINE, d == X.day());
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (verbose) cout << "\t'setYearMonthDay'" << endl;
            {
                Obj mX(1827, 3, 26);  const Obj& X = mX;

                ASSERT_SAFE_PASS(mX.setYearMonthDay(      1,       1,      1));
                ASSERT(1 == X.year());
                ASSERT(1 == X.month());
                ASSERT(1 == X.day());

                ASSERT_SAFE_FAIL(mX.setYearMonthDay(      0,       1,      1));
                ASSERT_SAFE_FAIL(mX.setYearMonthDay(     -1,       1,      1));
                ASSERT_SAFE_FAIL(mX.setYearMonthDay(INT_MIN,       1,      1));

                ASSERT_SAFE_FAIL(mX.setYearMonthDay(      1,      13,      1));
                ASSERT_SAFE_FAIL(mX.setYearMonthDay(      1,       0,      1));
                ASSERT_SAFE_FAIL(mX.setYearMonthDay(      1,      -1,      1));
                ASSERT_SAFE_FAIL(mX.setYearMonthDay(      1, INT_MIN,      1));

                ASSERT_SAFE_FAIL(mX.setYearMonthDay(      1,       1,     32));
                ASSERT_SAFE_FAIL(mX.setYearMonthDay(      1,       1,      0));
                ASSERT_SAFE_FAIL(mX.setYearMonthDay(      1,       1,     -1));
                ASSERT_SAFE_FAIL(mX.setYearMonthDay(      1,       1,INT_MIN));

                ASSERT(1 == X.year());
                ASSERT(1 == X.month());
                ASSERT(1 == X.day());

                ASSERT_SAFE_PASS(mX.setYearMonthDay(   9999,      12,     31));
                ASSERT(9999 == X.year());
                ASSERT(  12 == X.month());
                ASSERT(  31 == X.day());

                ASSERT_SAFE_FAIL(mX.setYearMonthDay(   9999,       0,     31));

                ASSERT_SAFE_FAIL(mX.setYearMonthDay(   9999,      12,      0));
                ASSERT_SAFE_FAIL(mX.setYearMonthDay(  10000,      12,     31));
                ASSERT_SAFE_FAIL(mX.setYearMonthDay(INT_MAX,      12,     31));

                ASSERT_SAFE_FAIL(mX.setYearMonthDay(   9999,      13,     31));
                ASSERT_SAFE_FAIL(mX.setYearMonthDay(   9999, INT_MAX,     31));

                ASSERT_SAFE_FAIL(mX.setYearMonthDay(   9999,      12,     32));
                ASSERT_SAFE_FAIL(mX.setYearMonthDay(   9999,      12,INT_MAX));

                ASSERT_SAFE_FAIL(mX.setYearMonthDay(   2000,       4,     31));
                ASSERT_SAFE_FAIL(mX.setYearMonthDay(   2000,       2,     30));
                ASSERT_SAFE_FAIL(mX.setYearMonthDay(   2001,       2,     29));

                ASSERT(9999 == X.year());
                ASSERT(  12 == X.month());
                ASSERT(  31 == X.day());
            }

            if (verbose) cout << "\t'getYearMonthDay'" << endl;
            {
                int y = -1, m = -1, d = -1;
                const Obj X(1827, 3, 26);

                ASSERT_SAFE_PASS(X.getYearMonthDay(&y, &m, &d));
                ASSERT(1827 == y);
                ASSERT(   3 == m);
                ASSERT(  26 == d);

                y = m = d = -1;

                ASSERT_SAFE_FAIL(X.getYearMonthDay( 0, &m, &d));
                ASSERT_SAFE_FAIL(X.getYearMonthDay(&y,  0, &d));
                ASSERT_SAFE_FAIL(X.getYearMonthDay(&y, &m,  0));
                ASSERT(  -1 == y);
                ASSERT(  -1 == m);
                ASSERT(  -1 == d);
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
        const Obj VA(   1,  1,  1);
        const Obj VB(   1,  1,  2);
        const Obj VC(   3,  4,  7);
        const Obj VD(2012,  4,  7);
        const Obj VE(2014,  6, 14);
        const Obj VF(2014, 10, 22);
        const Obj VG(9999, 12, 31);

        // Array object used in various stream tests.
        const Obj VALUES[]   = { VA, VB, VC, VD, VE, VF, VG };
        const int NUM_VALUES =
                              static_cast<int>(sizeof VALUES / sizeof *VALUES);

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
                const int         LOD = static_cast<int>(out.length());

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
            const int         LOD = static_cast<int>(out.length());
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
            const int         LOD = static_cast<int>(out.length());
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
            const int         LOD1 = static_cast<int>(out.length());

            Out& rvOut2 = bdexStreamOut(out, X2, VERSION);
            ASSERT(&out == &rvOut2);
            const int         LOD2 = static_cast<int>(out.length());

            Out& rvOut3 = bdexStreamOut(out, X3, VERSION);
            ASSERT(&out == &rvOut3);
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

                    LOOP_ASSERT(i, bdlt::Date::isValidYearDay(T1.year(),
                                                              T1.dayOfYear()));
                    LOOP_ASSERT(i, bdlt::Date::isValidYearDay(T2.year(),
                                                              T2.dayOfYear()));
                    LOOP_ASSERT(i, bdlt::Date::isValidYearDay(T3.year(),
                                                              T3.dayOfYear()));

                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        if (verbose) {
            cout << "\tOn corrupted data." << endl;
        }

        const Obj W;                // default value
        const Obj X(2, 1, 1);       // original (control)
        const Obj Y(3, 1, 1);       // new (streamed-out)

        // Verify the three objects are distinct.
        ASSERT(W != X);
        ASSERT(W != Y);
        ASSERT(X != Y);

        const int SERIAL_Y = 733;   // streamed rep. of 'Y'

        if (verbose) {
            cout << "\t\tGood stream (for control)." << endl;
        }
        {
            Out out(VERSION_SELECTOR, &allocator);
            out.putInt24(SERIAL_Y);  // Stream out "new" value.
            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

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
            out.putInt24(SERIAL_Y);  // Stream out "new" value.

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
            const char version = 2 ; // too large (current version is 1)

            Out out(VERSION_SELECTOR, &allocator);
            out.putInt24(SERIAL_Y);  // Stream out "new" value.

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
            cout << "\t\tValue too small." << endl;
        }
        {
            Out out(VERSION_SELECTOR, &allocator);
            out.putInt24(0);  // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

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
            out.putInt24(3652062);  // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

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
                int         d_lineNum;  // source line number
                int         d_year;     // specification year
                int         d_month;    // specification month
                int         d_day;      // specification day
                int         d_version;  // version to stream with
                int         d_length;   // expect output length
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //LINE  YEAR  MONTH  DAY  VER  LEN  FORMAT
                //----  ----  -----  ---  ---  ---  ---------------
                { L_,      1,     1,   1,   1,   3,  "\x00\x00\x01"  },
                { L_,   2014,    10,  22,   1,   3,  "\x0b\x39\x2a"  },
                { L_,   2016,     8,  27,   1,   3,  "\x0b\x3b\xcd"  }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         YEAR    = DATA[i].d_year;
                const int         MONTH   = DATA[i].d_month;
                const int         DAY     = DATA[i].d_day;
                const int         VERSION = DATA[i].d_version;
                const int         LEN     = DATA[i].d_length;
                const char *const FMT     = DATA[i].d_fmt_p;

                // Test using class methods.

                {
                    Obj        mX(YEAR, MONTH, DAY);
                    const Obj& X = mX;

                    bslx::ByteOutStream  out(VERSION_SELECTOR, &allocator);
                    bslx::ByteOutStream& rvOut = X.bdexStreamOut(out, VERSION);
                    LOOP_ASSERT(LINE, &out == &rvOut);

                    LOOP_ASSERT(LINE, LEN == static_cast<int>(out.length()));
                    LOOP_ASSERT(LINE, 0 == memcmp(out.data(), FMT, LEN));

                    if (verbose && memcmp(out.data(), FMT, LEN)) {
                        const char *hex = "0123456789abcdef";
                        P_(LINE);
                        for (int j = 0; j < static_cast<int>(out.length());
                                                                         ++j) {
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
                    Obj        mX(YEAR, MONTH, DAY);
                    const Obj& X = mX;

                    using bslx::OutStreamFunctions::bdexStreamOut;

                    bslx::ByteOutStream  out(VERSION_SELECTOR, &allocator);
                    bslx::ByteOutStream& rvOut = bdexStreamOut(out,
                                                               X,
                                                               VERSION);
                    LOOP_ASSERT(LINE, &out == &rvOut);

                    LOOP_ASSERT(LINE, LEN == static_cast<int>(out.length()));
                    LOOP_ASSERT(LINE, 0 == memcmp(out.data(), FMT, LEN));

                    if (verbose && memcmp(out.data(), FMT, LEN)) {
                        const char *hex = "0123456789abcdef";
                        P_(LINE);
                        for (int j = 0; j < static_cast<int>(out.length());
                                                                         ++j) {
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
        //:   object values (one per row) in terms of their year/month/day
        //:   representation.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1, 3..4)
        //:
        //:   1 Use the 3-argument value constructor to create two 'const'
        //:     objects, 'Z' and 'ZZ', both having the value from 'R1'.
        //:
        //:   2 For each row 'R2' in the table of P-2:  (C-1, 3..4)
        //:
        //:     1 Use the 3-argument value constructor to create a modifiable
        //:       'Obj', 'mX', having the value from 'R2'.
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
        //:   1 Use the 3-argument value constructor to create a modifiable
        //:     'Obj', 'mX', having the value from 'R1', and a 'const' 'Obj',
        //:     'ZZ', also having the value from 'R1'.
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
        //   Date& operator=(const Date& rhs);
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

        if (verbose) cout << "\nUse a table of distinct object values."
                          << endl;

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int ILINE  = DATA[ti].d_line;
            const int IYEAR  = DATA[ti].d_year;
            const int IMONTH = DATA[ti].d_month;
            const int IDAY   = DATA[ti].d_day;

            const Obj Z( IYEAR, IMONTH, IDAY);
            const Obj ZZ(IYEAR, IMONTH, IDAY);

            if (veryVerbose) { T_ P_(ILINE) P(Z) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(ILINE, Obj(1, 1, 1), Z, Obj(1, 1, 1) == Z);
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int JLINE  = DATA[tj].d_line;
                const int JYEAR  = DATA[tj].d_year;
                const int JMONTH = DATA[tj].d_month;
                const int JDAY   = DATA[tj].d_day;

                Obj mX(JYEAR, JMONTH, JDAY);  const Obj& X = mX;

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
                Obj mX(IYEAR, IMONTH, IDAY);

                const Obj ZZ(IYEAR, IMONTH, IDAY);

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

        if (verbose) cout << "Not implemented for 'bdlt::Date'." << endl;

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
        //:   object values (one per row) in terms of their year/month/day
        //:   representation.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..3)
        //:
        //:   1 Use the 3-argument value constructor to create two 'const'
        //:     objects, 'Z' and 'ZZ', both having the value from 'R'.
        //:
        //:   2 Use the copy constructor to create an object 'X' from 'Z'.
        //:     (C-2)
        //:
        //:   3 Use the equality-comparison operator to verify that:  (C-1, 3)
        //:
        //:     1 'X' has the same value as that of 'Z'.  (C-1)
        //:
        //:     2 'Z' still has the same value as that of 'ZZ'.  (C-3)
        //
        // Testing:
        //   Date(const Date& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout << "\nUse a table of distinct object values."
                          << endl;

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE  = DATA[ti].d_line;
            const int YEAR  = DATA[ti].d_year;
            const int MONTH = DATA[ti].d_month;
            const int DAY   = DATA[ti].d_day;

            if (veryVerbose) { T_ P_(LINE) P_(YEAR) P_(MONTH) P(DAY) }

            const Obj Z( YEAR, MONTH, DAY);
            const Obj ZZ(YEAR, MONTH, DAY);

            if (veryVerbose) { T_ T_ P_(Z) P(ZZ) }

            const Obj X(Z);

            if (veryVerbose) { T_ T_ T_ P(X) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE, Obj(1, 1, 1), X, Obj(1, 1, 1) == X)
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
        //:   corresponding year/month/day representations compare equal.
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
        //:   object values (one per row) in terms of their year/month/day
        //:   representation.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1..6)
        //:
        //:   1 Use the 3-argument value constructor to create a 'const'
        //:     object, 'W', having the value from 'R1'.
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
        //:     2 Use the 3-argument value constructor to create a 'const'
        //:       object, 'X', having the value from 'R1', and a second 'const'
        //:       object, 'Y', having the value from 'R2'.
        //:
        //:     3 Using 'X' and 'Y', verify the commutativity property and
        //:       expected return value for both '==' and '!='.  (C-1, 4..6)
        //
        // Testing:
        //   bool operator==(const Date& lhs, const Date& rhs);
        //   bool operator!=(const Date& lhs, const Date& rhs);
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
            int d_line;   // source line number
            int d_year;   // year under test
            int d_month;  // month under test
            int d_day;    // day under test
        } DATA[] = {
            //LINE   YEAR   MONTH   DAY
            //----   ----   -----   ---
            { L_,       1,      1,    1 },
            { L_,       1,      1,    2 },
            { L_,       1,      2,    1 },
            { L_,       2,      1,    1 },
            { L_,    9998,     12,   31 },
            { L_,    9999,     12,   30 },
            { L_,    9999,     12,   31 },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int ILINE  = DATA[ti].d_line;
            const int IYEAR  = DATA[ti].d_year;
            const int IMONTH = DATA[ti].d_month;
            const int IDAY   = DATA[ti].d_day;

            if (veryVerbose) { T_ P_(ILINE) P_(IYEAR) P_(IMONTH) P(IDAY) }

            // Ensure an object compares correctly with itself (alias test).
            {
                const Obj W(IYEAR, IMONTH, IDAY);

                LOOP2_ASSERT(ILINE, W,   W == W);
                LOOP2_ASSERT(ILINE, W, !(W != W));

                // Ensure the first row of the table contains the
                // default-constructed value.

                static bool firstFlag = true;
                if (firstFlag) {
                    LOOP3_ASSERT(ILINE, Obj(1, 1, 1), W, Obj(1, 1, 1) == W)
                    firstFlag = false;
                }
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int JLINE  = DATA[tj].d_line;
                const int JYEAR  = DATA[tj].d_year;
                const int JMONTH = DATA[tj].d_month;
                const int JDAY   = DATA[tj].d_day;

                if (veryVerbose) {
                    T_ T_ P_(JLINE) P_(JYEAR) P_(JMONTH) P(JDAY)
                }

                const bool EXP = ti == tj;  // expected for equality comparison

                const Obj X(IYEAR, IMONTH, IDAY);
                const Obj Y(JYEAR, JMONTH, JDAY);

                if (veryVerbose) { T_ T_ T_ P_(EXP) P_(X) P(Y) }

                // Verify expected against basic accessor comparison.

                if (EXP) {
                    LOOP4_ASSERT(ILINE, JLINE, X, Y, X.year()  == Y.year()
                                                  && X.month() == Y.month()
                                                  && X.day()   == Y.day());
                }
                else {
                    LOOP4_ASSERT(ILINE, JLINE, X, Y, X.year()  != Y.year()
                                                  || X.month() != Y.month()
                                                  || X.day()   != Y.day());
                }

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
        //:     corresponding year/month/day attribute, and various values for
        //:     the two formatting parameters, along with the expected output.
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
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   ostream& operator<<(ostream &os, const Date& object);
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

            int         d_year;
            int         d_month;
            int         d_day;

            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 } -->  4 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL      Y   M   D  EXP
        //---- - ---   ----  --  --  ---

        { L_,  0,  0,  1914,  7, 28, "28JUL1914"              NL },

        { L_,  0,  1,  1914,  7, 28, "28JUL1914"              NL },

        { L_,  0, -1,  1914,  7, 28, "28JUL1914"                 },

        { L_,  0, -8,  1914,  7, 28, "28JUL1914"              NL },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } -->  6 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL      Y   M   D  EXP
        //---- - ---   ----  --  --  ---

        { L_,  3,  0,  1914,  7, 28, "28JUL1914"              NL },

        { L_,  3,  2,  1914,  7, 28, "      28JUL1914"        NL },

        { L_,  3, -2,  1914,  7, 28, "      28JUL1914"           },

        { L_,  3, -8,  1914,  7, 28, "            28JUL1914"  NL },

        { L_, -3,  0,  1914,  7, 28, "28JUL1914"              NL },

        { L_, -3,  2,  1914,  7, 28, "28JUL1914"              NL },

        { L_, -3, -2,  1914,  7, 28, "28JUL1914"                 },

        { L_, -3, -8,  1914,  7, 28, "28JUL1914"              NL },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }            -->  1 expected o/p
        // -----------------------------------------------------------------

        //LINE L SPL      Y   M   D  EXP
        //---- - ---   ----  --  --  ---

        { L_,  2,  3,   721, 12,  9, "      09DEC0721"        NL },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 }   x { -8 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL      Y   M   D  EXP
        //---- - ---   ----  --  --  ---

        { L_, -8, -8,  1914,  7, 28, "28JUL1914"              NL },

        { L_, -8, -8,   721, 12,  9, "09DEC0721"              NL },

        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 }   x { -9 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL      Y   M   D  EXP
        //---- - ---   ----  --  --  ---

        { L_, -9, -9,  1914,  7, 28, "28JUL1914"                 },

        { L_, -9, -9,   721, 12,  9, "09DEC0721"                 },

#undef NL

        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const int         L     = DATA[ti].d_level;
                const int         SPL   = DATA[ti].d_spacesPerLevel;
                const int         YEAR  = DATA[ti].d_year;
                const int         MONTH = DATA[ti].d_month;
                const int         DAY   = DATA[ti].d_day;
                const char *const EXP   = DATA[ti].d_expected_p;

                if (veryVerbose) {
                    T_ P_(LINE) P_(L) P(SPL)
                    T_ P_(YEAR) P_(MONTH) P(DAY)
                }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                const Obj X(YEAR, MONTH, DAY);

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
                    bslma::TestAllocator da("default", veryVeryVeryVerbose);
                    bslma::DefaultAllocatorGuard dag(&da);

                    // Verify output is formatted as expected.

                    if (veryVeryVerbose) { P(os.str()) }

                    LOOP3_ASSERT(LINE, EXP, os.str(), EXP == os.str());
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
        //: 1 Each of the three basic accessors returns the value of the
        //:   corresponding year/month/day attribute of the date.
        //:
        //: 2 Each basic accessor method is declared 'const'.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their year/month/day
        //:   representation.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..2)
        //:
        //:   1 Use the 3-argument value constructor to create a 'const'
        //:     object, 'X', having the value from 'R'.
        //:
        //:   2 Verify that each basic accessor, invoked on 'X', returns the
        //:     expected value.  (C-1..2)
        //
        // Testing:
        //   int day() const;
        //   int month() const;
        //   int year() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nUse a table of distinct object values."
                          << endl;

        if (verbose) cout <<
                "\nVerify all basic accessors report expected values." << endl;

        if (verbose) cout << "\nUse a table of distinct object values."
                          << endl;

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE  = DATA[ti].d_line;
            const int YEAR  = DATA[ti].d_year;
            const int MONTH = DATA[ti].d_month;
            const int DAY   = DATA[ti].d_day;

            const Obj X(YEAR, MONTH, DAY);

            if (veryVerbose) { T_ P_(LINE) P_(YEAR) P_(MONTH) P_(DAY) P(X) }

            LOOP_ASSERT(LINE, YEAR  == X.year());
            LOOP_ASSERT(LINE, MONTH == X.month());
            LOOP_ASSERT(LINE, DAY   == X.day());
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTION 'gg'
        //   There is no 'gg' function for this component.
        //
        // Testing:
        //   Reserved for 'gg' generator function.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GENERATOR FUNCTION 'gg'" << endl
                          << "=======================" << endl;

        if (verbose) cout << "No 'gg' function for 'bdlt::Date'." << endl;

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // VALUE CTOR & DTOR
        //   Ensure that we can use the 3-argument value constructor to create
        //   an object having any state relevant for thorough testing, and use
        //   the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 The 3-argument value constructor can create an object to have any
        //:   value that does not violate the method's documented
        //:   preconditions.
        //:
        //: 2 An object can be safely destroyed.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the 3-argument value constructor, create an object to have
        //:   the default value.  Verify, using the (as yet unproven) 'year',
        //:   'month', and 'day' accessors, that the resulting object has a
        //:   value of 0001/01/01.
        //:
        //: 2 Let the object created in P-1 go out of scope.
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their year/month/day
        //:   representation.
        //:
        //: 4 For each row 'R' in the table of P-3:  (C-1..2)
        //:
        //:   1 Use the 3-argument value constructor to create a 'const'
        //:     object, 'X', having the value from 'R'.
        //:
        //:   2 Verify, using the 'year', 'month', and 'day' accessors, that
        //:     'X' has the expected value.  (C-1)
        //:
        //:   3 Let the object created in P-4.1 go out of scope.  (C-2)
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid year/month/day attribute values, but not
        //:   triggered for adjacent valid ones (using the 'BSLS_ASSERTTEST_*'
        //:   macros).  (C-3)
        //
        // Testing:
        //   Date(int year, int month, int day);
        //   ~Date();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CTOR & DTOR" << endl
                          << "=================" << endl;

        if (verbose) cout << "\nTesting default value." << endl;
        {
            const Obj X(1, 1, 1);

            if (veryVerbose) { T_ P(X) }

            ASSERT(1 == X.year());
            ASSERT(1 == X.month());
            ASSERT(1 == X.day());
        }

        if (verbose) cout << "\nTesting 'Date(year, month, day)'." << endl;
        {
            if (verbose) cout << "\nUse a table of distinct object values."
                              << endl;

            const int NUM_DATA                     = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_line;
                const int YEAR  = DATA[ti].d_year;
                const int MONTH = DATA[ti].d_month;
                const int DAY   = DATA[ti].d_day;

                if (veryVerbose) { T_ P_(LINE) P_(YEAR) P_(MONTH) P(DAY) }

                const Obj X(YEAR, MONTH, DAY);

                if (veryVeryVerbose) { T_ T_ P(X) }

                LOOP_ASSERT(LINE, YEAR  == X.year());
                LOOP_ASSERT(LINE, MONTH == X.month());
                LOOP_ASSERT(LINE, DAY   == X.day());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (verbose) cout << "\t'Date(year, month, day)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj(      1,       1,       1));

                ASSERT_SAFE_FAIL_RAW(Obj(      0,       1,       1));
                ASSERT_SAFE_FAIL_RAW(Obj(     -1,       1,       1));
                ASSERT_SAFE_FAIL_RAW(Obj(INT_MIN,       1,       1));

                ASSERT_SAFE_FAIL_RAW(Obj(      1,      13,       1));
                ASSERT_SAFE_FAIL_RAW(Obj(      1,       0,       1));
                ASSERT_SAFE_FAIL_RAW(Obj(      1,      -1,       1));
                ASSERT_SAFE_FAIL_RAW(Obj(      1, INT_MIN,       1));

                ASSERT_SAFE_FAIL_RAW(Obj(      1,       1,      32));
                ASSERT_SAFE_FAIL_RAW(Obj(      1,       1,       0));
                ASSERT_SAFE_FAIL_RAW(Obj(      1,       1,      -1));
                ASSERT_SAFE_FAIL_RAW(Obj(      1,       1, INT_MIN));

                ASSERT_SAFE_PASS_RAW(Obj(   9999,      12,      31));

                ASSERT_SAFE_FAIL_RAW(Obj(   9999,       0,      31));

                ASSERT_SAFE_FAIL_RAW(Obj(   9999,      12,       0));
                ASSERT_SAFE_FAIL_RAW(Obj(  10000,      12,      31));
                ASSERT_SAFE_FAIL_RAW(Obj(INT_MAX,      12,      31));

                ASSERT_SAFE_FAIL_RAW(Obj(   9999,      13,      31));
                ASSERT_SAFE_FAIL_RAW(Obj(   9999, INT_MAX,      31));

                ASSERT_SAFE_FAIL_RAW(Obj(   9999,      12,      32));
                ASSERT_SAFE_FAIL_RAW(Obj(   9999,      12, INT_MAX));

                ASSERT_SAFE_FAIL_RAW(Obj(   2000,       4,      31));
                ASSERT_SAFE_FAIL_RAW(Obj(   2000,       2,      30));
                ASSERT_SAFE_FAIL_RAW(Obj(   2001,       2,      29));
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

        const int A_YEAR = 1827, A_MONTH = 3, A_DAY = 26;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object 'w' (default ctor)."
                             "\t\t{ w:0             }" << endl;

        Obj mW;  const Obj& W = mW;

        if (veryVerbose) cout << "\ta. Check initial value of 'w'." << endl;
        if (veryVeryVerbose) { T_ P(W) }

        ASSERT(1 == W.year());
        ASSERT(1 == W.month());
        ASSERT(1 == W.day());

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

        ASSERT(1 == X.year());
        ASSERT(1 == X.month());
        ASSERT(1 == X.day());

        if (veryVerbose) cout <<
                     "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set 'x' to 'A' (value distinct from 0)."
                             "\t\t{ w:0 x:A         }" << endl;

        mX.setYearMonthDay(A_YEAR, A_MONTH, A_DAY);

        if (veryVerbose) cout << "\ta. Check new value of 'x'." << endl;
        if (veryVeryVerbose) { T_ P(X) }

        ASSERT(A_YEAR  == X.year());
        ASSERT(A_MONTH == X.month());
        ASSERT(A_DAY   == X.day());

        if (veryVerbose) cout <<
                     "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 4. Create an object 'y' (init. to 'A')."
                             "\t\t{ w:0 x:A y:A     }" << endl;

        Obj mY(A_YEAR, A_MONTH, A_DAY);  const Obj& Y = mY;

        if (veryVerbose) cout << "\ta. Check initial value of 'y'." << endl;
        if (veryVeryVerbose) { T_ P(Y) }

        ASSERT(A_YEAR  == Y.year());
        ASSERT(A_MONTH == Y.month());
        ASSERT(A_DAY   == Y.day());

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

        ASSERT(A_YEAR  == Z.year());
        ASSERT(A_MONTH == Z.month());
        ASSERT(A_DAY   == Z.day());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        ASSERT(1 == (Z == Y));        ASSERT(0 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Set 'z' to 0 (the default value)."
                             "\t\t\t{ w:0 x:A y:A z:0 }" << endl;

        mZ.setYearMonthDay(1, 1, 1);

        if (veryVerbose) cout << "\ta. Check new value of 'z'." << endl;
        if (veryVeryVerbose) { T_ P(Z) }

        ASSERT(1 == Z.year());
        ASSERT(1 == Z.month());
        ASSERT(1 == Z.day());

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

        ASSERT(A_YEAR  == W.year());
        ASSERT(A_MONTH == W.month());
        ASSERT(A_DAY   == W.day());

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

        ASSERT(1 == W.year());
        ASSERT(1 == W.month());
        ASSERT(1 == W.day());

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

        ASSERT(A_YEAR  == X.year());
        ASSERT(A_MONTH == X.month());
        ASSERT(A_DAY   == X.day());

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
