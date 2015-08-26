// bdlt_datetime.t.cpp                                                -*-C++-*-
#include <bdlt_datetime.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_asserttest.h>

#include <bslx_byteinstream.h>
#include <bslx_byteoutstream.h>
#include <bslx_instreamfunctions.h>
#include <bslx_outstreamfunctions.h>
#include <bslx_testinstream.h>
#include <bslx_testinstreamexception.h>
#include <bslx_testoutstream.h>
#include <bslx_versionfunctions.h>

#include <bsl_algorithm.h>  // 'min', 'max'
#include <bsl_cstdlib.h>    // 'atoi'
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The component under test is implemented using contained 'bdlt::Date' and
// 'bdlt::Time' objects.  As such, many concerns of a typical value-semantic
// component may be relaxed.  In particular, aside from BDEX streaming, there
// are only two accessors, 'date' and 'time', which permit the user to invoke
// the underlying accessors directly; minimal testing is required of these.
// Similarly, most of the datetime manipulators are implemented using the
// underlying contained-object manipulators; only the collaboration between
// time and date addition exposes new functionality.
//
// Note that if the implementation is ever changed, this test driver should be
// re-analyzed -- and perhaps rewritten -- BEFORE the implementation is
// altered.
//
// Primary Manipulators:
//: o 'setYearMonthDay'
//: o 'setTime'
//
// Basic Accessors
//: o 'date'
//: o 'time'
//
// Global Concerns:
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o No memory is ever allocated from the default allocator.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o ACCESSOR methods are 'const' thread-safe.
//:
//: o Individual attribute types are presumed to be *alias-safe*; hence, only
//:   certain methods require the testing of this property:
//:   o copy-assignment
//
// ----------------------------------------------------------------------------
// CLASS METHODS
// [17] bool isValid(int y, int m, int d, int h, int m, int s, int ms);
// [10] static int maxSupportedBdexVersion(int versionSelector);
//
// CREATORS
// [ 2] Datetime();
// [11] Datetime(const Date& date);
// [11] Datetime(const Date& date, const Time& time);
// [11] Datetime(int y, int m, int d, int h, int m, int s, int ms);
// [ 7] Datetime(const Datetime& original);
// [ 2] ~Datetime();
//
// MANIPULATORS
// [ 9] Datetime& operator=(const Datetime& rhs);
// [15] Datetime& operator+=(const DatetimeInterval& rhs);
// [15] Datetime& operator-=(const DatetimeInterval& rhs);
// [12] void setDatetime(int, int, int, int, int, int, int);
// [18] int setDatetimeIfValid(int, int, int, int, int, int, int);
// [20] void setDate(const Date& date);
// [19] void setYearMonthDay(int year, int month, int day);
// [19] void setYearDay(int year, int dayOfYear);
// [20] void setTime(const Time& time);
// [ 2] void setTime(int hour, int min = 0, int sec = 0, int ms = 0);
// [12] void setHour(int hour);
// [12] void setMinute(int minute);
// [12] void setSecond(int second);
// [12] void setMillisecond(int millisecond);
// [14] void addTime(Int64 hours, Int64 mins, Int64 secs, Int64 msecs);
// [21] void addDays(int days);
// [14] void addHours(Int64 hours);
// [14] void addMinutes(Int64 minutes);
// [14] void addSeconds(Int64 seconds);
// [14] void addMilliseconds(Int64 milliseconds);
//
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
//
// ACCESSORS
// [ 4] Date date() const;
// [ 4] Time time() const;
// [ 4] int year() const;
// [ 4] int month() const;
// [ 4] int day() const;
// [ 4] int dayOfYear() const;
// [ 4] DayOfWeek::Day dayOfWeek() const;
// [ 4] int hour() const;
// [ 4] int minute() const;
// [ 4] int second() const;
// [ 4] int millisecond() const;
// [ 5] int printToBuffer(char *result, int size) const;
//
// [ 5] ostream& print(ostream& os, int level = 0, int spl = 4) const;
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
//
// FREE OPERATORS
// [16] Datetime operator+(const Datetime&, const DatetimeInterval&);
// [16] Datetime operator+(const DatetimeInterval&, const Datetime&);
// [16] Datetime operator-(const Datetime&, const DatetimeInterval&);
// [16] DatetimeInterval operator-(const Datetime&, const Datetime&);
// [ 6] bool operator==(const Datetime& lhs, const Datetime& rhs);
// [ 6] bool operator!=(const Datetime& lhs, const Datetime& rhs);
// [13] bool operator< (const Datetime& lhs, const Datetime& rhs);
// [13] bool operator<=(const Datetime& lhs, const Datetime& rhs);
// [13] bool operator> (const Datetime& lhs, const Datetime& rhs);
// [13] bool operator>=(const Datetime& lhs, const Datetime& rhs);
//
// [ 5] ostream& operator<<(ostream &stream, const Datetime &object);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [23] USAGE EXAMPLE
// [ 2] BOOTSTRAP: void setYearMonthDay(int year, int month, int day);
// [ *] CONCERN: In no case does memory come from the default allocator.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ *] CONCERN: There is no temporary allocation from any allocator.
// [ *] CONCERN: All creator/manipulator ptr./ref. parameters are 'const'.
// [ 4] CONCERN: All accessor methods are declared 'const'.
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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlt::Datetime         Obj;

typedef bdlt::Date             Date;
typedef bdlt::DatetimeInterval DatetimeInterval;
typedef bdlt::DayOfWeek::Enum  DayOfWeek;
typedef bdlt::Time             Time;

typedef bsls::Types::Int64     Int64;

typedef bslx::TestInStream     In;
typedef bslx::TestOutStream    Out;

#define VERSION_SELECTOR 20140601

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define 'DEFAULT_DATA' used by test cases 7, 8, 9, and 11.

struct DefaultDataRow {
    int d_line;
    int d_year;
    int d_month;
    int d_day;
    int d_hour;
    int d_minute;
    int d_second;
    int d_msec;
};

static const DefaultDataRow DEFAULT_DATA[] =
{
    //LINE YEAR MON DAY  HR MIN SEC MSEC
    //---- ---- --- ---  -- --- --- ----
    {  L_,    1,  1,  1, 24,  0,  0,   0 },  // default object
    {  L_,    1,  1,  1,  0,  0,  0,   0 },  // start of epoch
    {  L_,   10,  4,  5,  0,  0,  0, 999 },
    {  L_,  100,  6,  7,  0,  0, 59,   0 },
    {  L_, 1000,  8,  9,  0, 59,  0,   0 },
    {  L_, 2000,  2, 29, 23,  0,  0,   0 },
    {  L_, 2002,  7,  4, 21, 22, 23, 209 },
    {  L_, 2003,  8,  5, 21, 22, 23, 210 },
    {  L_, 2004,  9,  3, 22, 44, 55, 888 },
    {  L_, 9999, 12, 31, 23, 59, 59, 999 },  // end of epoch
    {  L_,    1,  1,  2, 24,  0,  0,   0 }   // 24 on 1/1/2
};
const int DEFAULT_NUM_DATA =
                  static_cast<int>(sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA);

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

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);
    bslma::TestAllocatorMonitor gam(&globalAllocator);

    switch (test) { case 0:
      case 23: {
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
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// Values represented by objects of type 'bdlt::Datetime' are used widely in
// practice.  The values of the individual fields resulting from a
// default-constructed 'bdlt::Datetime' object, 'dt', are
// "0001/01/01_24:00:00.000":
//..
    bdlt::Datetime dt;          ASSERT( 1 == dt.date().year());
                                ASSERT( 1 == dt.date().month());
                                ASSERT( 1 == dt.date().day());
                                ASSERT(24 == dt.time().hour());
                                ASSERT( 0 == dt.time().minute());
                                ASSERT( 0 == dt.time().second());
                                ASSERT( 0 == dt.time().millisecond());
//..
// We can then set 'dt' to have a specific value, say, 8:43pm on January 6,
// 2013:
//..
    dt.setDatetime(2013, 1, 6, 20, 43);
                                ASSERT(2013 == dt.date().year());
                                ASSERT(   1 == dt.date().month());
                                ASSERT(   6 == dt.date().day());
                                ASSERT(  20 == dt.time().hour());
                                ASSERT(  43 == dt.time().minute());
                                ASSERT(   0 == dt.time().second());
                                ASSERT(   0 == dt.time().millisecond());
//..
// Now suppose we add 6 hours and 9 seconds to this value.  There is more than
// one way to do it:
//..
    bdlt::Datetime dt2(dt);
    dt2.addHours(6);
    dt2.addSeconds(9);
                                ASSERT(2013 == dt2.date().year());
                                ASSERT(   1 == dt2.date().month());
                                ASSERT(   7 == dt2.date().day());
                                ASSERT(   2 == dt2.time().hour());
                                ASSERT(  43 == dt2.time().minute());
                                ASSERT(   9 == dt2.time().second());
                                ASSERT(   0 == dt2.time().millisecond());

    bdlt::Datetime dt3(dt);
    dt3.addTime(6, 0, 9);
                                ASSERT(dt2 == dt3);
//..
// Notice that (in both cases) the date changed as a result of adding time;
// however, changing just the date never affects the time:
//..
    dt3.addDays(10);
                                ASSERT(2013 == dt3.date().year());
                                ASSERT(   1 == dt3.date().month());
                                ASSERT(  17 == dt3.date().day());
                                ASSERT(   2 == dt3.time().hour());
                                ASSERT(  43 == dt3.time().minute());
                                ASSERT(   9 == dt3.time().second());
                                ASSERT(   0 == dt3.time().millisecond());
//..
// We can also add more than a day's worth of time:
//..
    dt2.addHours(240);
                                ASSERT(dt3 == dt2);
//..
// The individual arguments can also be negative:
//..
    dt2.addTime(-246, 0, -10, 1000);  // -246 h, -10 s, +1000 ms
                                ASSERT(dt == dt2);
//..
// Finally, we stream the value of 'dt2' to 'stdout':
//..
if (veryVerbose)
    bsl::cout << dt2 << bsl::endl;
//..
// The streaming operator produces the following output on 'stdout':
//..
//  06JAN2013_20:43:00.000
//..
//
///Example 2: Creating a Schedule of Equal Time Intervals
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Calculations involving date and time values are difficult to get correct
// manually; consequently, people tend to schedule events on natural time
// boundaries (e.g., on the hour) even if that is sub-optimal.  Having a class
// such as 'bdlt::Datetime' makes doing date and time calculations trivial.
//
// Suppose one wants to divide into an arbitrary interval such as the time
// between sunset and sunrise into an arbitrary number (say 7) of equal
// intervals (perhaps to use as a duty roster for teams making astronomical
// observations).
//
// First, we create objects containing values for the start and end of the time
// interval:
//..
    bdlt::Datetime  sunset(2014, 6, 26, 20, 31, 23); // New York City
    bdlt::Datetime sunrise(2014, 6, 27,  5, 26, 51); // New York City
//..
// Then, we calculate the length of each shift in milliseconds (for good
// precision).  Note that the difference of 'sunrise' and 'sunset' creates a
// temporary 'bdlt::DatetimeInterval' object:
//..
    const int                numShifts = 7;
    const bsls::Types::Int64 shiftLengthInMsec
                                       = (sunrise - sunset).totalMilliseconds()
                                       / numShifts;
//..
// Now, we calculate (and print to 'stdout') the beginning and end times for
// each shift:
//..
    for (int i = 0; i <= numShifts; ++i) {
        bdlt::Datetime startOfShift(sunset);
        startOfShift.addMilliseconds(shiftLengthInMsec * i);
if (veryVerbose)
        bsl::cout << startOfShift << bsl::endl;
    }
//..
// Finally, we observe:
//..
//  26JUN2014_20:31:23.000
//  26JUN2014_21:47:52.714
//  26JUN2014_23:04:22.428
//  27JUN2014_00:20:52.142
//  27JUN2014_01:37:21.856
//  27JUN2014_02:53:51.570
//  27JUN2014_04:10:21.284
//  27JUN2014_05:26:50.998
//..
// Notice how our objects (since they manage both "date" and "time of day"
// parts of each point in time) seamlessly handle the transition between the
// two days.

      } break;
      case 22: {
       // Deprecated test case.  Do not remove.
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING 'addDays'
        //
        // Concerns:
        //: 1 The method changes the object's "date" part by the specified
        //:   number of days.
        //:
        //: 2 The method works irrespective of the initial state of the object.
        //:
        //: 3 The method does not change the "time" part.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct a table of substantial and varied differences in value
        //:   that spans the range of 'Datetime' values and includes the
        //:   default value.  The table will be used to create a series of test
        //:   objects.  Also construct an array of integer values that will be
        //:   used as argument methods.
        //:
        //: 2 Test the cross product of the objects defined by the table (P-1)
        //:   and day (delta) values.  Vet the values against overflow of the
        //:   epoch and adjust accordingly.  Invoke the object's 'addDays'
        //:   method with the (possibly adjusted) days value and confirm that
        //:   the date has been increased (decreased) by the expected number of
        //:   days.  Also confirm that the "time" part equals that of the
        //:   original value.  (C-1..3)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to perform operations that
        //:   would overflow the valid range of 'Datetime' values.
        //:   (using the 'BSLS_ASSERTTEST_*' macros).  (C-4)
        //
        // Testing:
        //   void addDays(int days);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'addDays'" << endl
                          << "=================" << endl;

        static const struct {
            int d_year;
            int d_month;
            int d_day;
            int d_hour;
            int d_minute;
            int d_second;
            int d_msec;
        } VALUES[] = {
            {    1,  1,  1, 24,  0,  0,   0 },  // default
            {    1,  1,  1,  0,  0,  0,   0 },  // start of epoch
            {   10,  4,  5,  0,  0,  0, 999 },
            {  100,  6,  7,  0,  0, 59,   0 },
            { 1000,  8,  9,  0, 59,  0,   0 },
            { 2000,  2, 29, 23,  0,  0,   0 },
            { 2002,  7,  4, 21, 22, 23, 209 },
            { 2003,  8,  5, 21, 22, 23, 210 },
            { 2004,  9,  3, 22, 44, 55, 888 },
            { 9999, 12, 31, 23, 59, 59, 999 },  // end of epoch

            // values with 24 == hour
            {    1,  1,  1, 24,  0,  0,   0 },
            {    1,  1,  2, 24,  0,  0,   0 },
            {   10,  4,  5, 24,  0,  0,   0 },
            {  100,  6,  7, 24,  0,  0,   0 },
            { 1000,  8,  9, 24,  0,  0,   0 },
            { 2000,  2, 29, 24,  0,  0,   0 },
            { 2002,  7,  4, 24,  0,  0,   0 },
            { 2003,  8,  5, 24,  0,  0,   0 },
            { 2004,  9,  3, 24,  0,  0,   0 },
            { 9999, 12, 31, 24,  0,  0,   0 },
        };
        const int NUM_VALUES =
                              static_cast<int>(sizeof VALUES / sizeof *VALUES);

        const int DAYS[] = { -10000,
                              -1000,
                               -100,
                                 -1,
                                  0,
                                  1,
                                100,
                               1000,
                              10000
                            };
        const int NUM_DAYS = static_cast<int>(sizeof DAYS / sizeof *DAYS);

        const Obj startOfEpoch(   1,  1,  1,   0,  0,  0,   0);
        const Obj   endOfEpoch(9999, 12, 31,  23, 59, 59, 999);

        for (int i = 0; i < NUM_VALUES; ++i) {
            const int YEAR   = VALUES[i].d_year;
            const int MONTH  = VALUES[i].d_month;
            const int DAY    = VALUES[i].d_day;
            const int HOUR   = VALUES[i].d_hour;
            const int MINUTE = VALUES[i].d_minute;
            const int SECOND = VALUES[i].d_second;
            const int MSEC   = VALUES[i].d_msec;

            if (veryVerbose) {
                T_ P_(YEAR)
                   P_(MONTH)
                   P_(DAY)
                   P_(HOUR)
                   P_(MINUTE)
                   P_(SECOND)
                   P(MSEC)
            }

            const Obj R(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, MSEC);

            for (int j = 0; j < NUM_DAYS; ++j) {
                const int D = DAYS[j];

                Obj mU(R);  const Obj& U = mU;

                const int DELTA = D > 0
                                ? bsl::min(D,   endOfEpoch.date() - U.date())
                                : bsl::max(D, startOfEpoch.date() - U.date());

                if (veryVerbose) { T_ T_ P_(U) P_(D) P(DELTA) }

                mU.addDays(DELTA);

                LOOP2_ASSERT(i, j, U.date() - R.date() == DELTA);
                LOOP2_ASSERT(i, j, R.time()            == U.time());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const Obj startOfEpoch(   1,  1,  1,   0,  0,  0,   0);
            const Obj   endOfEpoch(9999, 12, 31,  23, 59, 59, 999);

            if (veryVerbose) cout << "\t'addDays'" << endl;
            {
                Obj x0(startOfEpoch); ASSERT_SAFE_FAIL(x0.addDays(-1));
                Obj x1(startOfEpoch); ASSERT_SAFE_PASS(x1.addDays( 0));
                Obj x2(startOfEpoch); ASSERT_SAFE_PASS(x2.addDays( 1));

                Obj y0(  endOfEpoch); ASSERT_SAFE_PASS(y0.addDays(-1));
                Obj y1(  endOfEpoch); ASSERT_SAFE_PASS(y1.addDays( 0));
                Obj y2(  endOfEpoch); ASSERT_SAFE_FAIL(y2.addDays( 1));
            }
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'setDate' AND 'setTime'
        //
        // Concerns:
        //: 1 The 'setDate' and 'setTime' methods work for all values of 'Date'
        //:   and 'Time', respectively.
        //:
        //: 2 The methods work irrespective of the initial state of the object.
        //:
        //: 3 The 'setDate' method does not change the "time" part, and the
        //:   'setTime' method does not change the "date" part.
        //:
        //: 4 The methods are alias-safe.
        //
        // Plan:
        //: 1 Construct a table of substantial and varied differences in value
        //:   that spans the range of 'Datetime' values and includes the
        //:   default value.  The table will be used to create a series of test
        //:   objects.  The table also provides a meaningful and convenient
        //:   source of date and time values to be used as arguments to the
        //:   'setDate' and 'setTime' methods, respectively.
        //:
        //: 2 For each value specified by the table (P-1), create a test object
        //:   and invoke the 'setDate' ('setTime') method using each of the
        //:   date (time) values from the table.  In each case confirm that the
        //:   "date" and "time" parts of the test object have the expected
        //:   values.  (C-1..3)
        //:
        //: 3 For each value specified by the table (P-1), create an object and
        //:   invoke its 'setDate' ('setTime') method using the "date" ("time")
        //:   part of that object as an argument.  Confirm that the object
        //:   value is unchanged.  (C-4)
        //
        // Testing:
        //   void setDate(const Date& date);
        //   void setTime(const Time& time);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'setDate' AND 'setTime'" << endl
                          << "===============================" << endl;

        static const struct {
            int d_year;
            int d_month;
            int d_day;
            int d_hour;
            int d_minute;
            int d_second;
            int d_msec;
        } VALUES[] = {
            {    1,  1,  1, 24,  0,  0,   0 },  // default
            {    1,  1,  1,  0,  0,  0,   0 },  // start of epoch
            {   10,  4,  5,  0,  0,  0, 999 },
            {  100,  6,  7,  0,  0, 59,   0 },
            { 1000,  8,  9,  0, 59,  0,   0 },
            { 2000,  2, 29, 23,  0,  0,   0 },
            { 2002,  7,  4, 21, 22, 23, 209 },
            { 2003,  8,  5, 21, 22, 23, 210 },
            { 2004,  9,  3, 22, 44, 55, 888 },
            { 9999, 12, 31, 23, 59, 59, 999 },  // end of epoch

            // values with 24 == hour
            {    1,  1,  1, 24,  0,  0,   0 },
            {    1,  1,  2, 24,  0,  0,   0 },
            {   10,  4,  5, 24,  0,  0,   0 },
            {  100,  6,  7, 24,  0,  0,   0 },
            { 1000,  8,  9, 24,  0,  0,   0 },
            { 2000,  2, 29, 24,  0,  0,   0 },
            { 2002,  7,  4, 24,  0,  0,   0 },
            { 2003,  8,  5, 24,  0,  0,   0 },
            { 2004,  9,  3, 24,  0,  0,   0 },
            { 9999, 12, 31, 24,  0,  0,   0 },
        };
        const int NUM_VALUES =
                              static_cast<int>(sizeof VALUES / sizeof *VALUES);

        if (verbose) cout << "\nTesting 'setDate' and 'setTime'" << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            const int YEAR1   = VALUES[i].d_year;
            const int MONTH1  = VALUES[i].d_month;
            const int DAY1    = VALUES[i].d_day;
            const int HOUR1   = VALUES[i].d_hour;
            const int MINUTE1 = VALUES[i].d_minute;
            const int SECOND1 = VALUES[i].d_second;
            const int MSEC1   = VALUES[i].d_msec;

            if (veryVerbose) {
                T_ P_(YEAR1)
                   P_(MONTH1)
                   P_(DAY1)
                   P_(HOUR1)
                   P_(MINUTE1)
                   P_(SECOND1)
                   P(MSEC1)
            }

            const Obj R(YEAR1, MONTH1, DAY1, HOUR1, MINUTE1, SECOND1, MSEC1);

            if (veryVerbose) { T_ P(R) }

            for (int j = 0; j < NUM_VALUES; ++j) {
                const int YEAR2   = VALUES[j].d_year;
                const int MONTH2  = VALUES[j].d_month;
                const int DAY2    = VALUES[j].d_day;
                const int HOUR2   = VALUES[j].d_hour;
                const int MINUTE2 = VALUES[j].d_minute;
                const int SECOND2 = VALUES[j].d_second;
                const int MSEC2   = VALUES[j].d_msec;

                const Date DATE(YEAR2, MONTH2, DAY2);
                const Time TIME(HOUR2, MINUTE2, SECOND2, MSEC2);

                Obj mU(R);  const Obj& U = mU;

                if (veryVerbose) { T_ T_ P_(U) P(DATE) }

                mU.setDate(DATE);

                LOOP2_ASSERT(i, j, DATE     == U.date());
                LOOP2_ASSERT(i, j, R.time() == U.time());

                Obj mV(R);  const Obj& V = mV;

                if (veryVerbose) { T_ T_ P_(V) P(TIME) }

                mV.setTime(TIME);

                LOOP2_ASSERT(i, j, R.date() == V.date());
                LOOP2_ASSERT(i, j, TIME     == V.time());
            }
        }

        if (verbose) cout << "\nTesting 'setDate' and 'setTime' Aliasing."
                          << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            const int YEAR   = VALUES[i].d_year;
            const int MONTH  = VALUES[i].d_month;
            const int DAY    = VALUES[i].d_day;
            const int HOUR   = VALUES[i].d_hour;
            const int MINUTE = VALUES[i].d_minute;
            const int SECOND = VALUES[i].d_second;
            const int MSEC   = VALUES[i].d_msec;

            if (veryVerbose) {
                T_ P_(YEAR)
                   P_(MONTH)
                   P_(DAY)
                   P_(HOUR)
                   P_(MINUTE)
                   P_(SECOND)
                   P(MSEC)
            }

            const Obj R(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, MSEC);

            Obj mU(R);  const Obj& U = mU;
            mU.setDate(U.date());
            LOOP_ASSERT(i, R == U);

            Obj mV(R);  const Obj& V = mV;
            mV.setTime(V.time());
            LOOP_ASSERT(i, R == V);
        }

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'setYearDay' AND 'setYearMonthDay'
        //
        // Concerns:
        //: 1 Each method sets the intended date value.
        //:
        //: 2 Each method leaves the time value unchanged.
        //:
        //: 3 The original value of the object is not relevant.
        //
        // Plan:
        //: 1 Construct a table of valid inputs and compare results with the
        //:   expected values.  (C-1)
        //:
        //: 2 Confirm that the time value of the test object is not changed.
        //:   (C-2)
        //:
        //: 3 Repeat the tests for a series of test objects that span the range
        //:   of valid 'Datetime' values and include the default constructed
        //:   object.
        //
        // Testing:
        //   void setYearMonthDay(int year, int month, int day);
        //   void setYearDay(int year, int dayOfYear);
        // --------------------------------------------------------------------

        if (verbose) cout
                       <<  endl
                       << "TESTING 'setYearDay' AND 'setYearMonthDay'" << endl
                       << "==========================================" << endl;

        const Date RD(2000, 2, 3);       // Ref date (02FEB2000)
        const Time RT(23, 22, 21, 209);  // Ref time (21:22:21.209)
        const Obj  RDT(RD, RT);

        Obj ARRAY[] = { Obj(),                              // default value
                        Obj(   1,  1,  1,  0,  0,  0,   0), // start of epoch
                        RDT,                                // arbitrary value
                        Obj(9999, 12, 31, 23, 59, 59, 999), // end of epoch
                        Obj(   1,  1,  2, 24,  0,  0,   0)  // 24 on 1/1/2
                      };
        const int NUM_ARRAY = static_cast<int>(sizeof ARRAY / sizeof *ARRAY);

        for (int i = 0; i < NUM_ARRAY; ++i) {
            const Obj OBJ = ARRAY[i];

            if (veryVerbose) { T_ P(OBJ) }

            static const struct {
                int d_line;
                int d_year;
                int d_dayOfYear;
                int d_expMonth;
                int d_expDay;
            } DATA[] = {
                //line no.  year   dayOfYr     exp  month   exp  day
                //-------   -----  -------     ----------   --------
                { L_,          1,       1,          1,         1 },
                { L_,          1,       2,          1,         2 },
                { L_,          1,      32,          2,         1 },
                { L_,          1,     365,         12,        31 },

                { L_,       1996,       1,          1,         1 },
                { L_,       1996,       2,          1,         2 },
                { L_,       1996,      32,          2,         1 },
                { L_,       1996,     365,         12,        30 },
                { L_,       1996,     366,         12,        31 },

                { L_,       9999,       1,          1,         1 },
                { L_,       9999,       2,          1,         2 },
                { L_,       9999,      32,          2,         1 },
                { L_,       9999,     365,         12,        31 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int di = 0; di < NUM_DATA; ++di) {
                const int LINE        = DATA[di].d_line;
                const int YEAR        = DATA[di].d_year;
                const int DAY_OF_YEAR = DATA[di].d_dayOfYear;
                const int MONTH       = DATA[di].d_expMonth;
                const int DAY         = DATA[di].d_expDay;

                // Validate the table entries.

                ASSERT(DAY_OF_YEAR == Date(YEAR, MONTH, DAY).dayOfYear());

                if (veryVerbose) {
                    T_ T_  P_(LINE)
                           P_(YEAR)
                           P_(DAY_OF_YEAR)
                           P_(MONTH)
                           P(DAY)
                }

                const Date ED(YEAR, MONTH, DAY);

                Obj mX(OBJ);  const Obj& X = mX;

                mX.setYearDay(YEAR, DAY_OF_YEAR);
                LOOP_ASSERT(LINE, ED         == X.date());
                LOOP_ASSERT(LINE, OBJ.time() == X.time());

                Obj mY(OBJ);  const Obj& Y = mY;

                mY.setYearMonthDay(YEAR, MONTH, DAY);
                LOOP_ASSERT(LINE, ED         == Y.date());
                LOOP_ASSERT(LINE, OBJ.time() == Y.time());
            }
        }

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'setDatetimeIfValid'
        //
        // Concerns:
        //: 1 Each of the arguments independently contribute to the calculation
        //:   of validity.
        //:
        //: 2 The return value on success is 0.
        //:
        //: 3 If the arguments are valid, the object is set accordingly;
        //:   otherwise, the object is unchanged.
        //:
        //: 4 The return value on failure is non-zero.
        //:
        //: 5 The results are identical irrespective of the initial value of
        //:   the object.
        //:
        //: 6 Each optional argument is set to the expected value (0).
        //:
        //
        // Plan:
        //: 1 Construct a table of valid and invalid inputs.
        //:
        //: 2 For a series of test objects that span the range of valid
        //:   'Datetime' values, including a default constructed object, invoke
        //:   the method under test using the values from the table in P-1, and
        //:   verify the results are as expected.  (C-1..5)
        //:
        //: 3 Construct a series of object pairs.  For each pair, invoke the
        //:   'setDatetimeIfValid' method by explicitly specifying the expected
        //:   default value for an optional argument for one object, and by not
        //:   omitting the optional argument for the other object.  The two
        //:   objects should compare equal.  For each pair of the series, omit
        //:   one more of the four optional arguments.  (C-6)
        //
        // Testing:
        //   int setDatetimeIfValid(int, int, int, int, int, int, int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'setDatetimeIfValid'" << endl
                          << "============================" << endl;

        const Date RD(2000, 2, 3);       // Ref date (02FEB2000)
        const Time RT(23, 22, 21, 209);  // Ref time (21:22:21.209)
        const Obj  RDT(RD.year(),
                       RD.month(),
                       RD.day(),
                       RT.hour(),
                       RT.minute(),
                       RT.second(),
                       RT.millisecond());

        Obj ARRAY[] = { Obj(),                              // default value
                        Obj(   1,  1,  1,  0,  0,  0,   0), // start of epoch
                        RDT,                                // arbitrary value
                        Obj(9999, 12, 31, 23, 59, 59, 999), // end of epoch
                        Obj(   1,  1,  2, 24,  0,  0,   0)  // 24 on 1/1/2
                      };
        const int NUM_ARRAY = static_cast<int>(sizeof ARRAY / sizeof *ARRAY);

        for (int i = 0; i < NUM_ARRAY; ++i) {
            const Obj OBJ = ARRAY[i];

            if (veryVerbose) { T_ P(OBJ) }

            static const struct {
                int d_line;
                int d_year;
                int d_month;
                int d_day;
                int d_hour;
                int d_minute;
                int d_second;
                int d_msec;
                int d_expected;
            } DATA[] = {
                //line year   mo   day   hr   min  sec   msec   valid
                //---- ----   --   ---   --   ---  ---   ----   ------
                { L_,     1,   1,   1,    0,    0,   0,     0,    1   },

                { L_,     1,   1,   1,    0,    0,   0,   999,    1   },
                { L_,     1,   1,   1,    0,    0,  59,     0,    1   },
                { L_,     1,   1,   1,    0,   59,   0,     0,    1   },
                { L_,     1,   1,   1,   23,    0,   0,     0,    1   },
                { L_,     1,   1,  31,    0,    0,   0,     0,    1   },
                { L_,     1,  12,   1,    0,    0,   0,     0,    1   },
                { L_,  9999,   1,   1,    0,    0,   0,     0,    1   },

                { L_,     1,   1,   1,   24,    0,   0,     0,    1   },
                { L_,     2,   1,   1,   24,    0,   0,     0,    1   },

                { L_,  1400,  10,   2,    7,   31,   2,    22,    1   },
                { L_,  2002,   8,  27,   14,    2,  48,   976,    1   },

                { L_,     1,   1,   1,    0,    0,   0,    -1,    0   },
                { L_,     1,   1,   1,    0,    0,  -1,     0,    0   },
                { L_,     1,   1,   1,    0,   -1,   0,     0,    0   },
                { L_,     1,   1,   1,   -1,    0,   0,     0,    0   },
                { L_,     1,   1,   0,    0,    0,   0,     0,    0   },
                { L_,     1,   0,   1,    0,    0,   0,     0,    0   },
                { L_,     0,   1,   1,    0,    0,   0,     0,    0   },

                { L_,     1,   1,   1,    0,    0,   0,  1000,    0   },
                { L_,     1,   1,   1,    0,    0,  60,     0,    0   },
                { L_,     1,   1,   1,    0,   60,   0,     0,    0   },
                { L_,     1,   1,   1,   25,    0,   0,     0,    0   },
                { L_,     1,   1,  32,    0,    0,   0,     0,    0   },
                { L_,     1,  13,   1,    0,    0,   0,     0,    0   },
                { L_, 10000,   1,   1,    0,    0,   0,     0,    0   },

                { L_,     1,   1,   1,   24,    0,   0,     1,    0   },
                { L_,     1,   1,   1,   24,    0,   1,     0,    0   },
                { L_,     1,   1,   1,   24,    1,   0,     0,    0   },

                { L_,     0,   0,   0,   -1,   -1,  -1,    -1,    0   },

                { L_,  1401,   2,  29,    7,   31,   2,    22,    0   },
                { L_,  2002,   2,  29,   14,    2,  48,   976,    0   },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE     = DATA[i].d_line;
                const int YEAR     = DATA[i].d_year;
                const int MONTH    = DATA[i].d_month;
                const int DAY      = DATA[i].d_day;
                const int HOUR     = DATA[i].d_hour;
                const int MINUTE   = DATA[i].d_minute;
                const int SECOND   = DATA[i].d_second;
                const int MSEC     = DATA[i].d_msec;
                const int EXPECTED = DATA[i].d_expected;

                Obj mX(OBJ);  const Obj& X = mX;

                if (1 == EXPECTED) {
                    const Obj R(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, MSEC);

                    LOOP_ASSERT(LINE,
                                0 == mX.setDatetimeIfValid(YEAR,
                                                           MONTH,
                                                           DAY,
                                                           HOUR,
                                                           MINUTE,
                                                           SECOND,
                                                           MSEC));
                    LOOP_ASSERT(LINE, R == X);

                    if (veryVerbose) { T_ T_ P_(EXPECTED) P_(R) P(X) }
                }
                else {
                    LOOP_ASSERT(LINE,
                                0 != mX.setDatetimeIfValid(YEAR,
                                                           MONTH,
                                                           DAY,
                                                           HOUR,
                                                           MINUTE,
                                                           SECOND,
                                                           MSEC));
                    LOOP_ASSERT(LINE, OBJ == X);

                    if (veryVerbose) { T_ T_ P_(EXPECTED) P_(OBJ) P(X) }
                }
            }
        }

        if (verbose) cout << "\nCheck default values of optional parameters."
                          << endl;
        {
            const int YEAR = 2;  // arbitrary, non-default values
            const int MON  = 3;
            const int DAY  = 4;
            const int HOUR = 5;
            const int MIN  = 6;
            const int SECS = 7;
            const int MSEC = 8;

            Obj mA0;  const Obj& A0 = mA0;
            Obj mA1;  const Obj& A1 = mA1;
            mA0.setDatetimeIfValid(YEAR, MON, DAY, HOUR, MIN, SECS, MSEC);
            mA1.setDatetimeIfValid(YEAR, MON, DAY, HOUR, MIN, SECS, MSEC);
            ASSERT(A0 == A1);

            Obj mB0;  const Obj& B0 = mB0;
            Obj mB1;  const Obj& B1 = mB1;
            mB0.setDatetimeIfValid(YEAR, MON, DAY, HOUR, MIN, SECS,    0);
            mB1.setDatetimeIfValid(YEAR, MON, DAY, HOUR, MIN, SECS);
            ASSERT(B0 == B1);

            Obj mC0;  const Obj& C0 = mC0;
            Obj mC1;  const Obj& C1 = mC1;
            mC0.setDatetimeIfValid(YEAR, MON, DAY, HOUR, MIN,   0,    0);
            mC1.setDatetimeIfValid(YEAR, MON, DAY, HOUR, MIN);
            ASSERT(C0 == C1);

            Obj mD0;  const Obj& D0 = mD0;
            Obj mD1;  const Obj& D1 = mD1;
            mD0.setDatetimeIfValid(YEAR, MON, DAY, HOUR,   0,   0,    0);
            mD1.setDatetimeIfValid(YEAR, MON, DAY, HOUR);
            ASSERT(D0 == D1);

            Obj mE0;  const Obj& E0 = mE0;
            Obj mE1;  const Obj& E1 = mE1;
            mE0.setDatetimeIfValid(YEAR, MON, DAY,    0,   0,   0,    0);
            mE1.setDatetimeIfValid(YEAR, MON, DAY);
            ASSERT(E0 == E1);
        }

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'isValid'
        //
        // Concerns:
        //: 1 Each of the seven arguments contribute to the 'isValid'
        //:   calculation.
        //:
        // Plan:
        //: 1 Construct a table of valid and invalid inputs and compare results
        //:   to the expected return value of 'isValid'.  The table entries
        //:   test the range limits for each field.  (C-1)
        //
        // Testing:
        //   bool isValid(int y, int m, int d, int h, int m, int s, int ms);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'isValid'" << endl
                          << "=================" << endl;

        static const struct {
            int d_line;
            int d_year;
            int d_month;
            int d_day;
            int d_hour;
            int d_minute;
            int d_second;
            int d_msec;
            int d_expected;
        } DATA[] = {
            //line year   mo   day   hr   min  sec   msec   exp
            //---- ----   --   ---   --   ---  ---   ----   ---

            // valid 'Obj' arguments
            { L_,     1,   1,   1,   24,    0,   0,     0,    1   }, // default

            { L_,     1,   1,   1,    0,    0,   0,     0,    1   }, // start
            { L_,     1,   1,   1,    0,    0,   0,   999,    1   },
            { L_,     1,   1,   1,    0,    0,  59,     0,    1   },
            { L_,     1,   1,   1,    0,   59,   0,     0,    1   },
            { L_,     1,   1,   1,   23,    0,   0,     0,    1   },
            { L_,     1,   1,  31,    0,    0,   0,     0,    1   },
            { L_,     1,  12,   1,    0,    0,   0,     0,    1   },
            { L_,  9999,   1,   1,    0,    0,   0,     0,    1   },
            { L_,  9999,  12,  31,   23,   59,  59,   999,    1   }, // end

            { L_,  1400,  10,   2,    7,   31,   2,    22,    1   }, // ad hoc
            { L_,  2002,   8,  27,   14,    2,  48,   976,    1   }, // ad hoc

            { L_,     1,   1,   2,   24,    0,   0,     0,    1   }, // 24 on 2

            // invalid 'Obj' arguments
            { L_,     1,   1,   1,    0,    0,   0,    -1,    0   },
            { L_,     1,   1,   1,    0,    0,  -1,     0,    0   },
            { L_,     1,   1,   1,    0,   -1,   0,     0,    0   },
            { L_,     1,   1,   1,   -1,    0,   0,     0,    0   },
            { L_,     1,   1,   0,    0,    0,   0,     0,    0   },
            { L_,     1,   0,   1,    0,    0,   0,     0,    0   },
            { L_,     0,   1,   1,    0,    0,   0,     0,    0   },

            { L_,     1,   1,   1,    0,    0,   0,  1000,    0   },
            { L_,     1,   1,   1,    0,    0,  60,     0,    0   },
            { L_,     1,   1,   1,    0,   60,   0,     0,    0   },
            { L_,     1,   1,   1,   25,    0,   0,     0,    0   },
            { L_,     1,   1,  32,    0,    0,   0,     0,    0   },
            { L_,     1,  13,   1,    0,    0,   0,     0,    0   },
            { L_, 10000,   1,   1,    0,    0,   0,     0,    0   },

            { L_,     1,   1,   1,   24,    0,   0,     1,    0   },
            { L_,     1,   1,   1,   24,    0,   1,     0,    0   },
            { L_,     1,   1,   1,   24,    1,   0,     0,    0   },

            { L_,     0,   0,   0,   -1,   -1,  -1,    -1,    0   },

            { L_,  1401,   2,  29,    7,   31,   2,    22,    0   }, // ad hoc
            { L_,  2002,   2,  29,   14,    2,  48,    976,   0   }, // ad hoc
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE     = DATA[i].d_line;
            const int YEAR     = DATA[i].d_year;
            const int MONTH    = DATA[i].d_month;
            const int DAY      = DATA[i].d_day;
            const int HOUR     = DATA[i].d_hour;
            const int MINUTE   = DATA[i].d_minute;
            const int SECOND   = DATA[i].d_second;
            const int MSEC     = DATA[i].d_msec;
            const int EXPECTED = DATA[i].d_expected;

            if (veryVerbose) {
                T_  P_(LINE) P_(YEAR)   P_(MONTH)  P(DAY)
                T_  P_(HOUR) P_(MINUTE) P_(SECOND) P_(MSEC) P(EXPECTED)
            }

            LOOP_ASSERT(LINE, EXPECTED == Obj::isValid(YEAR,
                                                       MONTH,
                                                       DAY,
                                                       HOUR,
                                                       MINUTE,
                                                       SECOND,
                                                       MSEC));
        }

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING ADDITION AND SUBTRACTION OPERATORS
        //
        // Concerns:
        //: 1 The signature and return type of both addition and assignment
        //:   operator functions are as expected.
        //:
        //: 2 The addition operation is commutative for the allowed inputs.
        //:
        //: 3 The subtraction operation of 'Datetime' objects is
        //:   anticommutative for the allowed inputs
        //:
        //: 4 The addition and subtraction operations work for the maximum
        //:   values.
        //:
        //: 5 The addition and subtraction operations change 'hour()' from
        //:   24 to 0 before modifying the value.
        //
        // Plan:
        //: 1 Use the addresses of the addition and subtraction operator
        //:   functions to initialize function pointers having the appropriate
        //:   signatures and return types.  (C-1)
        //:
        //: 2 Create tables of date value pairs and time value pairs.
        //:
        //:   1 The entries include pairs whose differences cross field
        //:     boundaries of the "date" and "time" parts.
        //:
        //:   2 The entries include pairs with no differences.
        //:
        //: 3 Use the pairs of date and time values to construct 'Datetime'
        //:   objects.
        //:
        //:   1 Take calculate the differences between those objects (in
        //:     different orders) and verify that each result matches the
        //:     difference expected from their constituent date and time
        //:     values.  (C-3)
        //:
        //:   2 Apply the differences calculated in P-3.1 to reconstruct the
        //:     original objects (C-2).
        //:
        //: 4 Calculate the interval between the largest and smallest
        //:   'Datetime' values, then, in a series of ad hoc tests, confirm
        //:   that adding that value to the smallest 'Datetime' results in the
        //:   largest, and *vice* *versa*.  (C-4)
        //
        // Testing:
        //   Datetime operator+(const Datetime&, const DatetimeInterval&);
        //   Datetime operator+(const DatetimeInterval&, const Datetime&);
        //   Datetime operator-(const Datetime&, const DatetimeInterval&);
        //   DatetimeInterval operator-(const Datetime&, const Datetime&);
        // --------------------------------------------------------------------

        if (verbose) cout
                       << endl
                       << "TESTING ADDITION AND SUBTRACTION OPERATORS" << endl
                       << "==========================================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            // Verify that the signatures and return types are standard.

            typedef Obj              (*operatorPtrAddObjDti)(
                                                      const Obj&,
                                                      const DatetimeInterval&);
            typedef Obj              (*operatorPtrAddDtiObj)(
                                                       const DatetimeInterval&,
                                                       const Obj&);
            typedef DatetimeInterval (*operatorPtrDiffObj)(const Obj&,
                                                           const Obj&);
            typedef Obj              (*operatorPtrDiffDti)(
                                                      const Obj&,
                                                      const DatetimeInterval&);
             operatorPtrAddObjDti operatorAddObjDti =
                            static_cast<operatorPtrAddObjDti>(bdlt::operator+);
             operatorPtrAddDtiObj operatorAddDtiObj =
                            static_cast<operatorPtrAddDtiObj>(bdlt::operator+);

            operatorPtrDiffObj operatorDiffObj =
                              static_cast<operatorPtrDiffObj>(bdlt::operator-);
            operatorPtrDiffDti operatorDiffDti =
                              static_cast<operatorPtrDiffDti>(bdlt::operator-);

            (void)operatorAddObjDti;  // quash potential compiler warnings
            (void)operatorAddDtiObj;
            (void)operatorDiffObj;
            (void)operatorDiffDti;
        }

        if (verbose) cout << "\nCreate table of date-value pairs." << endl;

        static const struct {
            int d_line;
            int d_year1;
            int d_month1;
            int d_day1;

            int d_numDays;  // second - first

            int d_year2;
            int d_month2;
            int d_day2;
        } DATE_DATA[] = {
            //          - - - -first- - - -           - - - second - - -
            //line no.  year   month   day   numDays  year   month   day
            //-------   -----  -----  -----  -------  -----  -----  -----
            { L_,          1,     1,     1,      0,      1,     1,     1 },

            { L_,          1,     1,     1,      1,      1,     1,     2 },
            { L_,         10,     2,    28,      1,     10,     3,     1 },
            { L_,        100,     3,    31,      2,    100,     4,     2 },

            { L_,       1000,     4,    30,      4,   1000,     5,     4 },
            { L_,       1000,     6,     1,    -31,   1000,     5,     1 },
            { L_,       1001,     1,     1,   -366,   1000,     1,     1 },
            { L_,       1100,     5,    31,     30,   1100,     6,    30 },
            { L_,       1200,     6,    30,     32,   1200,     8,     1 },

            { L_,       1996,     2,    28,    367,   1997,     3,     1 },
            { L_,       1997,     2,    28,    366,   1998,     3,     1 },
            { L_,       1998,     2,    28,    365,   1999,     2,    28 },
            { L_,       1999,     2,    28,    364,   2000,     2,    27 },
            { L_,       1999,     2,    28,   1096,   2002,     2,    28 },
            { L_,       2002,     2,    28,  -1096,   1999,     2,    28 },
        };
        const int NUM_DATE_DATA =
                        static_cast<int>(sizeof DATE_DATA / sizeof *DATE_DATA);

        if (verbose) cout << "\nCreate table of time-value pairs." << endl;

        static const struct {
            int d_line;
            int d_hours2;
            int d_minutes2;
            int d_seconds2;
            int d_msecs2;

            int d_hours1;
            int d_minutes1;
            int d_seconds1;
            int d_msecs1;
            int d_expMsec;
        } TIME_DATA[] = {
            //        -- lhs time --    -- rhs time --
            //line #  h   m   s   ms    h   m   s   ms     Expected msec
            //------  --  --  --  ---   --  --  --  ---    -------------
            { L_,      0,  0,  0,   0,   0,  0,  0,   0,             0  },

            { L_,      0,  0,  0,   1,   0,  0,  0,   0,             1  },
            { L_,      0,  0,  0,   0,   0,  0,  0,   1,            -1  },

            { L_,      0,  0,  1,   0,   0,  0,  0,   0,          1000  },
            { L_,      0,  0,  0,   0,   0,  0,  1,   0,         -1000  },

            { L_,      0,  1,  0,   0,   0,  0,  0,   0,         60000  },
            { L_,      0,  0,  0,   0,   0,  1,  0,   0,        -60000  },

            { L_,      1,  0,  0,   0,   0,  0,  0,   0,       3600000  },
            { L_,      0,  0,  0,   0,   1,  0,  0,   0,      -3600000  },
        };
        const int NUM_TIME_DATA =
                        static_cast<int>(sizeof TIME_DATA / sizeof *TIME_DATA);

        if (verbose) cout
                 << "\nTest operators on combined date and time value pairs."
                 << endl;

        for (int di = 0; di < NUM_DATE_DATA ; ++di) {
            const int YEAR1  = DATE_DATA[di].d_year1;
            const int MONTH1 = DATE_DATA[di].d_month1;
            const int DAY1   = DATE_DATA[di].d_day1;
            const int YEAR2  = DATE_DATA[di].d_year2;
            const int MONTH2 = DATE_DATA[di].d_month2;
            const int DAY2   = DATE_DATA[di].d_day2;

            Date date1(YEAR1, MONTH1, DAY1);
            Date date2(YEAR2, MONTH2, DAY2);

            Int64 dateDiff = Int64(date2 - date1) * (24 * 60 * 60 * 1000);

            if (veryVerbose) { T_ P_(date1)
                                  P_(date2)
                                  P(dateDiff)
            }

            for (int ti = 0; ti < NUM_TIME_DATA; ++ti) {
                const int LINE     = TIME_DATA[ti].d_line;
                const int HOURS2   = TIME_DATA[ti].d_hours2;
                const int MINUTES2 = TIME_DATA[ti].d_minutes2;
                const int SECONDS2 = TIME_DATA[ti].d_seconds2;
                const int MSECS2   = TIME_DATA[ti].d_msecs2;
                const int HOURS1   = TIME_DATA[ti].d_hours1;
                const int MINUTES1 = TIME_DATA[ti].d_minutes1;
                const int SECONDS1 = TIME_DATA[ti].d_seconds1;
                const int MSECS1   = TIME_DATA[ti].d_msecs1;
                const int EXP_MSEC = TIME_DATA[ti].d_expMsec;

                const Time time1(HOURS2, MINUTES2, SECONDS2, MSECS2);
                const Time time2(HOURS1, MINUTES1, SECONDS1, MSECS1);

                const Obj X1(date1, time1);
                const Obj X2(date2, time2);

                const Obj X3(date1, time2);
                const Obj X4(date2, time1);

                if (veryVerbose) {
                     T_ T_ P(X1)
                     T_ T_ P(X2)
                     T_ T_ P(X3)
                     T_ T_ P(X4)
                }

                const DatetimeInterval INTERVAL1(X1 - X2);
                const DatetimeInterval INTERVAL2(X2 - X1);
                const DatetimeInterval INTERVAL3(X3 - X4);
                const DatetimeInterval INTERVAL4(X4 - X3);

                if (veryVerbose) {
                    T_ T_  P(INTERVAL1.totalMilliseconds());
                    T_ T_  P(INTERVAL2.totalMilliseconds());
                    T_ T_  P(INTERVAL3.totalMilliseconds());
                    T_ T_  P(INTERVAL4.totalMilliseconds());
                }

                LOOP_ASSERT(LINE,
                     EXP_MSEC - dateDiff == INTERVAL1.totalMilliseconds());

                LOOP_ASSERT(LINE,
                    -EXP_MSEC + dateDiff == INTERVAL2.totalMilliseconds());

                LOOP_ASSERT(LINE,
                    -EXP_MSEC - dateDiff == INTERVAL3.totalMilliseconds());

                LOOP_ASSERT(LINE,
                     EXP_MSEC + dateDiff == INTERVAL4.totalMilliseconds());

                if (veryVerbose) {
                    T_ P_(X3) P_(X4) P(X4 - INTERVAL4)
                }

                LOOP_ASSERT(LINE, X1 == INTERVAL1 + X2);
                LOOP_ASSERT(LINE, X1 == X2 + INTERVAL1);
                LOOP_ASSERT(LINE, X1 == X2 - INTERVAL2);

                LOOP_ASSERT(LINE, X2 == INTERVAL2 + X1);
                LOOP_ASSERT(LINE, X2 == X1 + INTERVAL2);
                LOOP_ASSERT(LINE, X2 == X1 - INTERVAL1);

                LOOP_ASSERT(LINE, X3 == INTERVAL3 + X4);
                LOOP_ASSERT(LINE, X3 == X4 + INTERVAL3);
                LOOP_ASSERT(LINE, X3 == X4 - INTERVAL4);

                LOOP_ASSERT(LINE, X4 == INTERVAL4 + X3);
                LOOP_ASSERT(LINE, X4 == X3 + INTERVAL4);
                LOOP_ASSERT(LINE, X4 == X3 - INTERVAL3);
            }
        }

        const Obj startOfEpoch(   1,  1,  1,   0,  0,  0,   0);
        const Obj   endOfEpoch(9999, 12, 31,  23, 59, 59, 999);

        if (verbose) cout
          << "\nTest largest supported time interval changes." << endl;
        {
            const int              numDaysEpoch =   endOfEpoch.date()
                                                - startOfEpoch.date();
            const DatetimeInterval delta(numDaysEpoch, 23, 59, 59, 999);

            if (veryVerbose) { P(startOfEpoch) P(endOfEpoch) P(delta) }

            const Obj X(startOfEpoch);
            const Obj Y(  endOfEpoch);

            ASSERT( delta == Y - X);
            ASSERT(-delta == X - Y);

            ASSERT(Y == X + delta);
            ASSERT(Y == delta + X);
            ASSERT(X == Y - delta);
        }

        if (verbose) cout
          << "\nTest handling of hour 24." << endl;
        {
            Obj mX;  const Obj& X = mX;
            Obj mY;  const Obj& Y = mY;

            const DatetimeInterval zero(0, 0, 0, 0, 0);

            mX += zero;
            mY -= zero;

            ASSERT(Obj(1, 1, 1, 0, 0, 0, 0) == X);
            ASSERT(Obj(1, 1, 1, 0, 0, 0, 0) == Y);
        }

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING ARITHMETIC ASSIGNMENT OPERATORS
        //
        // Concerns:
        //: 1 The signature and return type of both arithmetic assignment
        //:   operator functions are standard.
        //:
        //: 2 The value of 'Datetime' object can be changed by an arbitrary
        //:   'DatetimeInterval' (valid) value, subject to the constraint that
        //:   the valid 'Datetime' range is not exceeded.
        //:
        //:   1 Differences in "time" part are propagated to the "date" part as
        //:     needed.
        //:
        //:   2 The arithmetic assignment operators work for the largest
        //:     possible changes in the value.
        //:
        //: 3 The arithmetic assignment operations are inverse functions.
        //:
        //: 4 The addition and subtraction operations change 'hour()' from
        //:   24 to 0 before modifying the value.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the addresses of the two arithmetic assignment operator
        //:   functions to initialize function pointers having the appropriate
        //:   signatures and return types.  (C-1)
        //:
        //: 2 Using a table-driven test, iterate through the cross product of
        //:   representative sets 'Datetime' values and 'DatetimeInterval'
        //:   values (to be applied as a "delta" to the 'Datetime' values).
        //:   The resulting values cover the (valid) range of 'Datetime' values
        //:   (C-2) and include cases that trigger changes in the "date" part
        //:   (C-2.1).
        //:
        //:   1 For each pair, use 'operator+=' to apply the delta to the base
        //:     value and compare the result to that obtained by adding the
        //:     equivalent number of milliseconds using the 'addMilliseconds'
        //:     method (fully tested in case 14).
        //:
        //:   2 Apply the delta using 'operator-=' and confirm that the
        //:     original value is recovered.
        //:
        //:   3 Repeat the sequence in reverse order 'operator-=', the
        //:     'operator+='.  (C-3)
        //:
        //:   4 Vet each calculation against overflow of the valid range of
        //:     'Datetime'.
        //:
        //: 3 Calculate the interval between the largest and smallest
        //:   'Datetime' values, then confirm that adding that value to the
        //:   smallest 'Datetime' results in the largest, and *vice* *versa*.
        //:   (C-2.2)
        //:
        //: 4 Use ad hoc tests to convert 'Datetime()' to
        //:   'Datetime(1, 1, 1, 0, 0, 0, 0' by adding (subtracting) a zero
        //:   interval.  (C-4)
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to perform operations that
        //:   would overflow the valid range of 'Datetime' values.
        //:   (using the 'BSLS_ASSERTTEST_*' macros).  (C-5)
        //
        // Testing:
        //   Datetime& operator+=(const DatetimeInterval& rhs);
        //   Datetime& operator-=(const DatetimeInterval& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ARITHMETIC ASSIGNMENT OPERATORS" << endl
                          << "=======================================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const DatetimeInterval&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorPlusEquals  = &Obj::operator+=;
            operatorPtr operatorMinusEquals = &Obj::operator-=;

            (void)operatorPlusEquals;   // quash potential compiler warnings
            (void)operatorMinusEquals;
        }

        if (verbose) cout << "\nCreate table of 'Datetime' values." << endl;

        static const struct {
            int d_year;
            int d_month;
            int d_day;
            int d_hour;
            int d_minute;
            int d_second;
            int d_msec;
        } INITIAL_VALUES[] = {
            {    5,  1,  1,   0,  0,  0,   0 },
            {   10,  4,  5,   0,  0,  0,   1 },
            {  100,  6,  7,   0,  0,  0, 999 },
            { 1000,  8,  9,   0,  0,  1,   0 },
            { 2000,  1, 31,   0,  0, 59, 999 },
            { 2002,  7,  4,   0,  1,  0,   0 },
            { 2002, 12, 31,   0,  1,  0,   1 },
            { 2003,  1,  1,   0, 59, 59, 999 },
            { 2003,  1,  2,   1,  0,  0,   0 },
            { 2003,  8,  5,   1,  0,  0,   1 },
            { 2003,  8,  6,  23,  0,  0,   0 },
            { 2003,  8,  7,  23, 22, 21, 209 },
            { 2004,  9,  3,  23, 22, 21, 210 },
            { 2004,  9,  4,  23, 22, 21, 211 },
            { 9990, 12, 31,  23, 59, 59, 999 },
         };
        const int NUM_INIT_VALUES =
              static_cast<int>(sizeof INITIAL_VALUES / sizeof *INITIAL_VALUES);

        if (verbose) cout
                     << "\nCreate table of 'DatetimeInterval' values." << endl;

        static const struct {
            int d_days;
            int d_hours;
            int d_minutes;
            int d_seconds;
            int d_msecs;
        } INTERVAL_VALUES[] = {
            { -1001,    0,   0,   0,    0 },
            { -1000,  -23, -59, -59, -999 },
            { -1000,  -23, -59, -59, -998 },
            {   -10,   25,  80,  70,   -1 },
            {   -10,   25,  80,  70,    0 },
            {   -10,   25,  80,  70,    1 },
            {    -1,    0,  -1,   0,    0 },
            {    -1,    0,   0,   0,    0 },
            {    -1,    0,   1,   0,    0 },
            {     0,    0,   0,   0,    0 },
            {     1,    0,  -1,   0,    0 },
            {     1,    0,   0,   0,    0 },
            {     1,    0,   1,   0,    0 },
            {    10,   25,  80,  70,   -1 },
            {    10,   25,  80,  70,    0 },
            {    10,   25,  80,  70,    1 },
            {  1000,   23,  59,  59,  998 },
            {  1000,   23,  59,  59,  999 },
            {  1001,    0,   0,   0,    0 },
         };
        const int NUM_INTERVAL_VALUES =
            static_cast<int>(sizeof INTERVAL_VALUES / sizeof *INTERVAL_VALUES);

        const Obj startOfEpoch(   1,  1,  1,   0,  0,  0,   0);
        const Obj   endOfEpoch(9999, 12, 31,  23, 59, 59, 999);

        if (verbose) cout
              << "\nApply each 'DatetimeInterval' to each 'Datetime'." << endl;

        for (int i = 0; i < NUM_INIT_VALUES; ++i) {
            const int YEAR   = INITIAL_VALUES[i].d_year;
            const int MONTH  = INITIAL_VALUES[i].d_month;
            const int DAY    = INITIAL_VALUES[i].d_day;
            const int HOUR   = INITIAL_VALUES[i].d_hour;
            const int MINUTE = INITIAL_VALUES[i].d_minute;
            const int SECOND = INITIAL_VALUES[i].d_second;
            const int MSEC   = INITIAL_VALUES[i].d_msec;

            const Obj R(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, MSEC);

            if (veryVerbose) P(R);

            for (int j = 0; j < NUM_INTERVAL_VALUES; ++j) {
                const int DAYS    = INTERVAL_VALUES[i].d_days;
                const int HOURS   = INTERVAL_VALUES[i].d_hours;
                const int MINUTES = INTERVAL_VALUES[i].d_minutes;
                const int SECONDS = INTERVAL_VALUES[i].d_seconds;
                const int MSECS   = INTERVAL_VALUES[i].d_msecs;

                Obj x(R);  const Obj& X = x;
                Obj y(R);  const Obj& Y = y;

                DatetimeInterval delta(DAYS, HOURS, MINUTES, SECONDS, MSECS);
                if (veryVerbose) { P(delta); }

                if (delta > DatetimeInterval(0)) {
                    DatetimeInterval maxDelta  =   endOfEpoch - x;
                    ASSERT(delta < maxDelta);
                } else {
                    DatetimeInterval minDelta  = startOfEpoch - x;
                    ASSERT(minDelta < delta);
                }

                x += delta; // 'operator+='

                y.addMilliseconds(delta.totalMilliseconds());

                if (veryVerbose) { P_(X);  P(Y); }

                LOOP2_ASSERT(i, j, Y == X);

                x -= delta; // 'operator-=' to undo the change, yielding 'R'

                LOOP2_ASSERT(i, j, R == X);

                // Repeat operations in reverse order.
                Obj u(R);  const Obj& U = u;
                Obj v(R);  const Obj& V = v;

                if (delta < DatetimeInterval(0)) {
                    DatetimeInterval maxDelta  = endOfEpoch - x;
                    ASSERT(delta < maxDelta);
                } else {
                    DatetimeInterval minDelta  = startOfEpoch - x;
                    ASSERT(minDelta < delta);
                }

                u -= delta;  // 'operator-='

                v.addMilliseconds(-delta.totalMilliseconds());

                if (veryVerbose) { P_(U);  P(V); }

                LOOP2_ASSERT(i, j, V == U);

                u += delta;  // 'operator+=' to undo the change, yielding 'R'

                LOOP2_ASSERT(i, j, R == U);
            }
        }

        if (verbose) cout
          << "\nTest largest supported time interval changes." << endl;
        {
            const int              numDaysEpoch =   endOfEpoch.date()
                                                - startOfEpoch.date();
            const DatetimeInterval delta(numDaysEpoch, 23, 59, 59, 999);

            if (veryVerbose) { P(startOfEpoch)
                               P(  endOfEpoch)
                               P(delta)
                              }

            Obj x(startOfEpoch);  const Obj& X = x;

            if (veryVerbose) { P(x) }
            x += delta;
            if (veryVerbose) { P(x) }

            ASSERT(endOfEpoch == X);

            Obj y(endOfEpoch);  const Obj& Y = y;

            if (veryVerbose) { P(y) }
            y -= delta;
            if (veryVerbose) { P(y) }

            ASSERT(startOfEpoch == Y);
        }

        if (verbose) cout
          << "\nTest handling of hour 24." << endl;
        {
            Obj x;  const Obj& X = x;
            Obj y;  const Obj& Y = y;

            const DatetimeInterval zero(0, 0, 0, 0, 0);

            x += zero;
            y -= zero;

            ASSERT(Obj(1, 1, 1, 0, 0, 0, 0) == X);
            ASSERT(Obj(1, 1, 1, 0, 0, 0, 0) == Y);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const DatetimeInterval zero;
            const DatetimeInterval oneMsec(0, 0, 0, 0, 1);

            if (veryVerbose) cout << "\t'operator+='" << endl;
            {
//              Obj x0(startOfEpoch); ASSERT_SAFE_FAIL(x0 += (-oneMsec));
//              ProlepticGregorianMode assert squelched

                Obj x1(startOfEpoch); ASSERT_SAFE_PASS(x1 += (    zero));
                Obj x2(startOfEpoch); ASSERT_SAFE_PASS(x2 += ( oneMsec));

                Obj y0(  endOfEpoch); ASSERT_SAFE_PASS(y0 += (-oneMsec));
                Obj y1(  endOfEpoch); ASSERT_SAFE_PASS(y1 += (    zero));
                Obj y2(  endOfEpoch); ASSERT_SAFE_FAIL(y2 += ( oneMsec));
            }

            if (veryVerbose) cout << "\t'operator-='" << endl;
            {
                Obj x0(startOfEpoch); ASSERT_SAFE_PASS(x0 -= (-oneMsec));
                Obj x1(startOfEpoch); ASSERT_SAFE_PASS(x1 -= (    zero));
//              Obj x2(startOfEpoch); ASSERT_SAFE_FAIL(x2 -= ( oneMsec));
//              ProlepticGregorianMode assert squelched

                Obj y0(  endOfEpoch); ASSERT_SAFE_FAIL(y0 -= (-oneMsec));
                Obj y1(  endOfEpoch); ASSERT_SAFE_PASS(y1 -= (    zero));
                Obj y2(  endOfEpoch); ASSERT_SAFE_PASS(y2 -= ( oneMsec));
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'add' METHODS
        //
        // Concerns:
        //: 1 Each 'add' method correctly forwards its arguments to the
        //:   corresponding operation on the "time" part.
        //:
        //: 2 Changes to the "time" part that cross day boundaries correctly
        //:   propagate to the "date" part.
        //:
        //: 3 The 'addTime' method can be used as an oracle for testing the
        //:   other 'add' methods.
        //:
        //: 3 The optional arguments to 'addTime' have the expected default
        //:   values.
        //:
        //: 4 Invocations of these methods on default constructed objects
        //:   behave as if the object had been constructed with a 'Time' value
        //:   of 00:00:00.000.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Thoroughly test the 'addTime' method, then use that method as an
        //:   oracle for tests of 'addHours', 'addMinutes', 'addSeconds', and
        //:   'addMilliseconds' methods.  (C-1,3)
        //:
        //:   1 Using a table-driven test, perform a series of 'addTime'
        //:     invocations on a object created from a single, reference date,
        //:     and compare the result to the calculated expected value.
        //:
        //:   2 The 'hour', 'minute', 'second', and 'millisecond' values used
        //:     include positive, negative, and 0 values.  The non-zero values
        //:     used are of sufficient magnitude to demonstrate propagation of
        //:     changes to the other "time" fields and, in some cases, to the
        //:     "date" part.
        //:
        //: 2 Using loop-based tests: (C-1,2)
        //:
        //:   1 Apply equivalent time adjustments to two newly constructed
        //:     objects having the same reference datetime value.  Use
        //:     'addTime' method for one object and the 'addHours' method for
        //:     the other then compare for equality.  The set of time
        //:     adjustments include positive, negative, and 0 values, and are
        //:     of sufficient magnitude to cross date boundaries.
        //:
        //:   2 Perform tests analogous to P-1.1 for the 'addMinutes',
        //:     'addSeconds', and 'addMilliseconds' methods.
        //:
        //: 4 Create a series of object pairs having the same reference
        //:   datetime value and identically adjust the time value of each of
        //:   those pairs using the 'setTime' method. Construct and then set
        //:   the values of "date" and "time" parts using the 'setDatetime'
        //:   method.  As we go through the five pairs of objects, we invoke
        //:   the 'setTime' method with one fewer of the optional arguments for
        //:   one of the objects and invoke 'setTime' with the expected default
        //:   values for the other object.  The two objects must compare equal.
        //:   (C-3)
        //:
        //: 5 For each of the methods under test, construct a pair of objects:
        //:   one default constructed, the other constructed with the value
        //:   "0001/01/01_00:00:00.000".  Use the method under test to perform
        //:   a non-zero adjustment both of these objects and compare the
        //:   objects for equality.  (C-4)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to perform operations that
        //:   would overflow the valid range of 'Datetime' values.
        //:   (using the 'BSLS_ASSERTTEST_*' macros).  (C-5)
        //
        // Testing:
        //   void addTime(Int64 hours, Int64 mins, Int64 secs, Int64 msecs);
        //   void addHours(Int64 hours);
        //   void addMinutes(Int64 minutes);
        //   void addSeconds(Int64 seconds);
        //   void addMilliseconds(Int64 milliseconds);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'add' METHODS" << endl
                          << "=====================" << endl;

        {
            static const struct {
                int d_line;          // source line number
                int d_hours;         // hours to add
                int d_minutes;       // minutes to add
                int d_seconds;       // seconds to add
                int d_msecs;         // milliseconds to add
                int d_expDays;       // expected whole days added to object
                int d_expHour;       // expected hour value
                int d_expMinute;     // expected minute value
                int d_expSecond;     // expected second value
                int d_expMsec;       // expected milliseconds value
            } DATA[] = {
    //----------^
    //        - - - - - - time added - - - - - -   ---expected values---
    //line #   h       m         s           ms    days  h   m   s   ms
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

    //----------v
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            if (verbose) cout << "\nTesting: 'addTime'" << endl;

            const int REFERENCE_YEAR  = 2000;
            const int REFERENCE_MONTH =    1;
            const int REFERENCE_DAY   =   15;

            const Date INITIAL_DATE(REFERENCE_YEAR,
                                    REFERENCE_MONTH,
                                    REFERENCE_DAY);
            const Time INITIAL_TIME(0, 0, 0, 0);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE     = DATA[i].d_line;
                const int HOURS    = DATA[i].d_hours;
                const int MINUTES  = DATA[i].d_minutes;
                const int SECONDS  = DATA[i].d_seconds;
                const int MSECS    = DATA[i].d_msecs;

                const int EXP_DAYS = DATA[i].d_expDays;
                const int EXP_HR   = DATA[i].d_expHour;
                const int EXP_MIN  = DATA[i].d_expMinute;
                const int EXP_SEC  = DATA[i].d_expSecond;
                const int EXP_MSEC = DATA[i].d_expMsec;

                Obj mX(INITIAL_DATE, INITIAL_TIME);  const Obj& X = mX;
                if (veryVerbose) { T_  P_(X) }

                mX.addTime(HOURS, MINUTES, SECONDS, MSECS);

                const Date EXP_D(REFERENCE_YEAR,
                                 REFERENCE_MONTH,
                                 REFERENCE_DAY + EXP_DAYS);
                const Time EXP_T(EXP_HR,
                                 EXP_MIN,
                                 EXP_SEC,
                                 EXP_MSEC);
                const Obj EXP(EXP_D, EXP_T);

                if (veryVerbose) { P_(X)  P(EXP) }

                LOOP_ASSERT(LINE, EXP == X);
            }
        }

        if (verbose) cout
                  << "\nCheck 'addTime' default values of optional parameters."
                  << endl;
        {
            // Reference 'Datetime'

            const int YEAR   = 2;  // arbitrary, non-default values
            const int MONTH  = 3;
            const int DAY    = 4;
            const int HOUR   = 5;
            const int MINUTE = 6;
            const int SECOND = 7;
            const int MSEC   = 8;

            const Obj IDT(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, MSEC);

            // Adjustment Values, none corresponding to the default values.
            const int HOURS   = 100;
            const int MINUTES = 200;
            const int SECONDS = 300;
            const int MSECS   = 400;

            Obj mA0(IDT);  const Obj& A0 = mA0;
            Obj mA1(IDT);  const Obj& A1 = mA1;
            mA0.addTime(HOURS, MINUTES, SECONDS, MSECS);
            mA1.addTime(HOURS, MINUTES, SECONDS, MSECS);
            ASSERT(A0 == A1);

            Obj mB0(IDT);  const Obj& B0 = mB0;
            Obj mB1(IDT);  const Obj& B1 = mB1;
            mB0.addTime(HOURS, MINUTES, SECONDS,     0);
            mB1.addTime(HOURS, MINUTES, SECONDS);
            ASSERT(B0 == B1);

            Obj mC0(IDT);  const Obj& C0 = mC0;
            Obj mC1(IDT);  const Obj& C1 = mC1;
            mC0.addTime(HOURS, MINUTES,       0,     0);
            mC1.addTime(HOURS, MINUTES);
            ASSERT(C0 == C1);

            Obj mD0(IDT);  const Obj& D0 = mD0;
            Obj mD1(IDT);  const Obj& D1 = mD1;
            mD0.addTime(HOURS,       0,       0,      0);
            mD1.addTime(HOURS);
            ASSERT(D0 == D1);
        }


        if (verbose) cout
             << "\nTest 'add' methods on default constructed objects." << endl;
        {
            // Adjustment Values, none corresponding to the default values.
            const int HOURS   = 100;
            const int MINUTES = 200;
            const int SECONDS = 300;
            const int MSECS   = 400;

            Obj mA0;                    const Obj& A0 = mA0;
            Obj mA1(1, 1, 1, 0, 0, 0);  const Obj& A1 = mA1;
            mA0.addTime(HOURS, MINUTES, SECONDS, MSECS);
            mA1.addTime(HOURS, MINUTES, SECONDS, MSECS);
            ASSERT(A0 == A1);

            Obj mB0;                    const Obj& B0 = mB0;
            Obj mB1(1, 1, 1, 0, 0, 0);  const Obj& B1 = mB1;
            mB0.addHours(HOURS);
            mB1.addHours(HOURS);
            ASSERT(B0 == B1);

            Obj mC0;                    const Obj& C0 = mC0;
            Obj mC1(1, 1, 1, 0, 0, 0);  const Obj& C1 = mC1;
            mC0.addMinutes(MINUTES);
            mC1.addMinutes(MINUTES);
            ASSERT(C0 == C1);

            Obj mD0;                    const Obj& D0 = mD0;
            Obj mD1(1, 1, 1, 0, 0, 0);  const Obj& D1 = mD1;
            mD0.addSeconds(SECONDS);
            mD1.addSeconds(SECONDS);
            ASSERT(D0 == D1);

            Obj mE0;                    const Obj& E0 = mE0;
            Obj mE1(1, 1, 1, 0, 0, 0);  const Obj& E1 = mE1;
            mE0.addMilliseconds(MSECS);
            mE1.addMilliseconds(MSECS);
            ASSERT(E0 == E1);
        }

        {
            const int REFERENCE_YEAR  = 2000;
            const int REFERENCE_MONTH =    1;
            const int REFERENCE_DAY   =   15;

            const Date ID(REFERENCE_YEAR, REFERENCE_MONTH, REFERENCE_DAY);
            const Time IT(0, 0, 0, 0);

            if (verbose) cout << "\nTesting: 'addHours'" << endl;

            const int START_HOURS = -250;
            const int STOP_HOURS  =  250;
            const int STEP_HOURS  =   25;

            for (int hi = START_HOURS; hi <= STOP_HOURS; hi += STEP_HOURS) {
                Obj x(ID, IT);  const Obj &X = x;
                Obj y(ID, IT);  const Obj &Y = y;

                if (veryVerbose) { T_  P_(X) }

                x.addHours(hi);
                y.addTime(hi, 0, 0, 0);

                if (veryVerbose) { P_(X);  P_(Y);  P(hi); }

                LOOP_ASSERT(hi, Y == X);
            }

            if (verbose) cout << "\nTesting: 'addMinutes'" << endl;

            const int START_MINS  = -15000;
            const int STOP_MINS   =  15000;
            const int STEP_MINS   =   1500;

            for (int mi = START_MINS; mi <= STOP_MINS; mi += STEP_MINS) {
                Obj x(ID, IT);  const Obj &X = x;
                Obj y(ID, IT);  const Obj &Y = y;

                if (veryVerbose) { T_  P_(X) }

                x.addMinutes(mi);
                y.addTime(0, mi, 0, 0);

                if (veryVerbose) { P_(X)  P_(Y)  P(mi) }

                LOOP_ASSERT(mi, Y == X);
            }

            if (verbose) cout << "\nTesting: 'addSeconds'" << endl;

            const int START_SECS = -900000;
            const int STOP_SECS  =  900000;
            const int STEP_SECS  =   90000;

            for (int si = START_SECS; si <= STOP_SECS; si += STEP_SECS) {
                Obj x(ID, IT);  const Obj &X = x;
                Obj y(ID, IT);  const Obj &Y = y;

                if (veryVerbose) { T_  P_(X) }

                x.addSeconds(si);
                y.addTime(0, 0, si, 0);
                if (veryVerbose) { P_(X)  P_(Y)  P(si) }

                LOOP_ASSERT(si, Y == X);
            }

            if (verbose) cout << "\nTesting: 'addMilliseconds'" << endl;

            const int START_MSECS = -900000000;
            const int STOP_MSECS  =  900000000;
            const int STEP_MSECS  =   90000000;

            for (int msi = START_MSECS; msi <= STOP_MSECS; msi += STEP_MSECS) {
                Obj x(ID, IT);  const Obj &X = x;
                Obj y(ID, IT);  const Obj &Y = y;

                if (veryVerbose) { T_  P_(X) }

                x.addMilliseconds(msi);
                y.addTime(0, 0, 0, msi);

                if (veryVerbose) { P_(X)  P_(Y)  P(msi) }

                LOOP_ASSERT(msi, Y == X);
            }

        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const Obj startOfEpoch(   1,  1,  1,   0,  0,  0,   0);
            const Obj   endOfEpoch(9999, 12, 31,  23, 59, 59, 999);

            if (veryVerbose) cout << "\t'addTime'" << endl;
            {
//              Obj x0(startOfEpoch); ASSERT_SAFE_FAIL(x0.addTime(0,0,0, -1));
//              ProlepticGregorianMode assert squelched

                Obj x1(startOfEpoch); ASSERT_SAFE_PASS(x1.addTime(0,0,0,  0));
                Obj x2(startOfEpoch); ASSERT_SAFE_PASS(x2.addTime(0,0,0,  1));

                Obj y0(  endOfEpoch); ASSERT_SAFE_PASS(y0.addTime(0,0,0, -1));
                Obj y1(  endOfEpoch); ASSERT_SAFE_PASS(y1.addTime(0,0,0,  0));
                Obj y2(  endOfEpoch); ASSERT_SAFE_FAIL(y2.addTime(0,0,0,  1));
            }

            if (veryVerbose) cout << "\t'addHours'" << endl;
            {
                Obj x0(startOfEpoch); ASSERT_SAFE_FAIL(x0.addHours(-1));
                Obj x1(startOfEpoch); ASSERT_SAFE_PASS(x1.addHours( 0));
                Obj x2(startOfEpoch); ASSERT_SAFE_PASS(x2.addHours( 1));

                Obj y0(  endOfEpoch); ASSERT_SAFE_PASS(y0.addHours(-1));
                Obj y1(  endOfEpoch); ASSERT_SAFE_PASS(y1.addHours( 0));
                Obj y2(  endOfEpoch); ASSERT_SAFE_FAIL(y2.addHours( 1));
            }

            if (veryVerbose) cout << "\t'addMinutes'" << endl;
            {
                Obj x0(startOfEpoch); ASSERT_SAFE_FAIL(x0.addMinutes(-1));
                Obj x1(startOfEpoch); ASSERT_SAFE_PASS(x1.addMinutes( 0));
                Obj x2(startOfEpoch); ASSERT_SAFE_PASS(x2.addMinutes( 1));

                Obj y0(  endOfEpoch); ASSERT_SAFE_PASS(y0.addMinutes(-1));
                Obj y1(  endOfEpoch); ASSERT_SAFE_PASS(y1.addMinutes( 0));
                Obj y2(  endOfEpoch); ASSERT_SAFE_FAIL(y2.addMinutes( 1));
            }

            if (veryVerbose) cout << "\t'addSeconds'" << endl;
            {
                Obj x0(startOfEpoch); ASSERT_SAFE_FAIL(x0.addSeconds(-1));
                Obj x1(startOfEpoch); ASSERT_SAFE_PASS(x1.addSeconds( 0));
                Obj x2(startOfEpoch); ASSERT_SAFE_PASS(x2.addSeconds( 1));

                Obj y0(  endOfEpoch); ASSERT_SAFE_PASS(y0.addSeconds(-1));
                Obj y1(  endOfEpoch); ASSERT_SAFE_PASS(y1.addSeconds( 0));
                Obj y2(  endOfEpoch); ASSERT_SAFE_FAIL(y2.addSeconds( 1));
            }

            if (veryVerbose) cout << "\t'addMilliseconds'" << endl;
            {
                Obj x0(startOfEpoch); ASSERT_SAFE_FAIL(x0.addMilliseconds(-1));
                Obj x1(startOfEpoch); ASSERT_SAFE_PASS(x1.addMilliseconds( 0));
                Obj x2(startOfEpoch); ASSERT_SAFE_PASS(x2.addMilliseconds( 1));

                Obj y0(  endOfEpoch); ASSERT_SAFE_PASS(y0.addMilliseconds(-1));
                Obj y1(  endOfEpoch); ASSERT_SAFE_PASS(y1.addMilliseconds( 0));
                Obj y2(  endOfEpoch); ASSERT_SAFE_FAIL(y2.addMilliseconds( 1));
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING RELATIONAL OPERATORS
        //
        // Concerns:
        //: 1 The signature and return type of each relational operator
        //:   function are standard.
        //:
        //: 2 Each relational operator function reports the intended logical
        //:   relationship.
        //:
        //: 3 Each field of the constituent "date" and "time" parts contributes
        //:   to the calculation of the logical relationships.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the addresses of the four relational operator functions to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types.  (C-1)
        //:
        //: 2 Specify an array of unique object values such that:
        //:
        //:   1 The elements are sorted in increasing order, thereby allowing
        //:     us to calculate from the indexes of any two elements in the
        //:     array their expected logical relation.  (C-2)
        //:
        //:   2 The sequence of object values contain some elements that differ
        //:     in only one of the seven fields.
        //:
        //:   4 The start and end of the range of valid 'Date' and 'Time'
        //:     values are included.
        //:
        //:   5 The default object value is *not* included in the array because
        //:     relational comparisons for objects having the default 'Time'
        //:     value are not defined.
        //:
        //: 3 Use each pair of element values in the array to construct
        //:   elements, calculate each of the four supported relational values
        //:   for those elements, and compare that result with the expected
        //:   result based on the relative position in the array of the
        //:   objects' element values.
        //:
        //: 4 Note that ubiquitous test allocators are already installed as the
        //:   global allocator and the default allocator.
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to relationally compare objects
        //:   having the default constructed value.  (using the
        //:   'BSLS_ASSERTTEST_*' macros).  (C-5)
        //
        // Testing:
        //  bool operator< (const Datetime& lhs, const Datetime& rhs);
        //  bool operator<=(const Datetime& lhs, const Datetime& rhs);
        //  bool operator> (const Datetime& lhs, const Datetime& rhs);
        //  bool operator>=(const Datetime& lhs, const Datetime& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING RELATIONAL OPERATORS" << endl
                          << "============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorLt = bdlt::operator<;
            operatorPtr operatorGt = bdlt::operator>;
            operatorPtr operatorLe = bdlt::operator<=;
            operatorPtr operatorGe = bdlt::operator>=;

            (void)operatorLt;  // quash potential compiler warnings
            (void)operatorGt;
            (void)operatorLe;
            (void)operatorGe;
        }

        if (verbose) cout <<
                  "\nCreate a sorted table of distinct object values." << endl;

        static const struct {
            int d_year;
            int d_month;
            int d_day;
            int d_hour;
            int d_minute;
            int d_second;
            int d_msec;
        } VALUES[] = {

            // default omitted

            {    1,  1,  1,   0,  0,  0,   0 },  // start of epoch

            {    1,  1,  1,   0,  0,  0,   1 },  // each field participates
            {    1,  1,  1,   0,  0,  1,   0 },
            {    1,  1,  1,   0,  1,  0,   0 },
            {    1,  1,  1,   1,  0,  0,   0 },
            {    1,  1,  2,   0,  0,  0,   0 },
            {    1,  2,  1,   0,  0,  0,   0 },
            {    2,  1,  1,   0,  0,  0,   0 },

            {   10,  4,  5,   0,  0,  0,   1 },
            {  100,  6,  7,   0,  0,  0, 999 },
            { 1000,  8,  9,   0,  0,  1,   0 },
            { 2000,  1, 31,   0,  0, 59, 999 },
            { 2002,  7,  4,   0,  1,  0,   0 },
            { 2002, 12, 31,   0,  1,  0,   1 },
            { 2003,  1,  1,   0, 59, 59, 999 },
            { 2003,  1,  2,   1,  0,  0,   0 },
            { 2003,  8,  5,   1,  0,  0,   1 },
            { 2003,  8,  6,  23,  0,  0,   0 },
            { 2003,  8,  7,  23, 22, 21, 209 },
            { 2004,  9,  3,  23, 22, 21, 210 },
            { 2004,  9,  4,  23, 22, 21, 211 },
            { 9999, 12, 31,  23, 59, 59, 999 }   // end of epoch
         };
        const int NUM_VALUES =
                              static_cast<int>(sizeof VALUES / sizeof *VALUES);

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            const int YEAR1   = VALUES[i].d_year;
            const int MONTH1  = VALUES[i].d_month;
            const int DAY1    = VALUES[i].d_day;
            const int HOUR1   = VALUES[i].d_hour;
            const int MINUTE1 = VALUES[i].d_minute;
            const int SECOND1 = VALUES[i].d_second;
            const int MSEC1   = VALUES[i].d_msec;

            const Obj V(YEAR1, MONTH1, DAY1, HOUR1, MINUTE1, SECOND1, MSEC1);

            for (int j = 0; j < NUM_VALUES; ++j) {
                const int YEAR2   = VALUES[j].d_year;
                const int MONTH2  = VALUES[j].d_month;
                const int DAY2    = VALUES[j].d_day;
                const int HOUR2   = VALUES[j].d_hour;
                const int MINUTE2 = VALUES[j].d_minute;
                const int SECOND2 = VALUES[j].d_second;
                const int MSEC2   = VALUES[j].d_msec;

                const Obj U(YEAR2, MONTH2, DAY2,
                            HOUR2, MINUTE2, SECOND2, MSEC2);

                if (veryVerbose) { T_  P_(i)  P_(j)  P_(V)  P(U) }

                LOOP2_ASSERT(i, j, (j <  i) == (U <  V));
                LOOP2_ASSERT(i, j, (j <= i) == (U <= V));
                LOOP2_ASSERT(i, j, (j >= i) == (U >= V));
                LOOP2_ASSERT(i, j, (j >  i) == (U >  V));
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const Obj    defaultObject;
            const Obj nonDefaultObject(1, 1, 2);

            if (veryVerbose) cout << "\t'operator<'" << endl;
            {
                ASSERT_SAFE_PASS(nonDefaultObject <  nonDefaultObject);
                ASSERT_SAFE_FAIL(nonDefaultObject <     defaultObject);
                ASSERT_SAFE_FAIL(   defaultObject <  nonDefaultObject);
                ASSERT_SAFE_FAIL(   defaultObject <     defaultObject);
            }

            if (veryVerbose) cout << "\t'operator<='" << endl;
            {
                ASSERT_SAFE_PASS(nonDefaultObject <= nonDefaultObject);
                ASSERT_SAFE_FAIL(nonDefaultObject <=    defaultObject);
                ASSERT_SAFE_FAIL(   defaultObject <= nonDefaultObject);
                ASSERT_SAFE_FAIL(   defaultObject <=    defaultObject);
            }

            if (veryVerbose) cout << "\t'operator>'" << endl;
            {
                ASSERT_SAFE_PASS(nonDefaultObject >  nonDefaultObject);
                ASSERT_SAFE_FAIL(nonDefaultObject >     defaultObject);
                ASSERT_SAFE_FAIL(   defaultObject >  nonDefaultObject);
                ASSERT_SAFE_FAIL(   defaultObject >     defaultObject);
            }

            if (veryVerbose) cout << "\t'operator>='" << endl;
            {
                ASSERT_SAFE_PASS(nonDefaultObject >= nonDefaultObject);
                ASSERT_SAFE_FAIL(nonDefaultObject >=    defaultObject);
                ASSERT_SAFE_FAIL(   defaultObject >= nonDefaultObject);
                ASSERT_SAFE_FAIL(   defaultObject >=    defaultObject);
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING INDIVIDUAL TIME-'set'-ING MANIPULATORS
        //
        // Concerns:
        //: 1 Each of the time-only manipulators correctly forwards its
        //:   arguments to the appropriate manipulator of the constituent
        //:   'Time' object.
        //:
        //:   1 When the "time" part has a non-default value, each of the time-
        //:     setting manipulators changes it's intended time field (e.g.,
        //:     hours, milliseconds) and no other (see C-2)
        //:
        //:   2 None of the time-setting manipulators change the "date" part.
        //:
        //: 2 None of these manipulators, except the 'setDatetime' method,
        //:   alters the "date" part of the object.
        //:
        //: 3 The 'setDatetime' method correctly forwards its arguments to the
        //:   appropriate manipulators of the constituent 'Date' and 'Time'
        //:   objects.
        //:
        //: 4 The 'setDatetime' method defines the same optional parameters and
        //:   provides the same default values as the seven-argument value
        //:   constructor.
        //:
        //: 5 The methods have the same effect regardless of the object's
        //:   initial value.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For a set of independent test values that do not include the
        //:   default 'Time' value (24:00:00.000), use the default constructor
        //:   to create an object and use the time-only "set" manipulators to
        //:   set its value.  Verify the value using the basic accessors after
        //:   each individual "time" field is set.  Repeat the tests for a
        //:   series of objects that span the range of valid 'Datetime' values,
        //:   but excluding the default constructed object (see P-2).
        //:
        //: 2 Create a series of objects having a time "part" equal to 'Time()'
        //:   (24:00:00.000) and confirm using values from the valid bounding
        //:   range of each "time" field that using any of the individual
        //:   time-setting manipulators both sets the specified value (e.g.,
        //:   minute, second) *and* sets the hour field to 0.  Then create an
        //:   object having non-zero values for "time" fields and confirm that
        //:   'setHour(24)' sets that specified value *and* sets all other
        //:   fields to 0.  (C-1..2)
        //:
        //: 3 For each set of values used in testing the seven-argument value
        //:   constructor, create and compare two objects for equality.  One is
        //:   created by the value constructor (proven earlier), the other by
        //:   using the seven-argument 'setDatetime' method of a test object.
        //:   Use a series of test objects that span the range of valid
        //:   'Datetime' values, *including* the default constructed object.
        //:   (C-3, 5)
        //:
        //: 4 Create a series of five object pairs using the default
        //:   constructor and then set the values of the "date" and "time"
        //:   parts using the 'setDatetime' method.  As we go through the five
        //:   pairs of objects, we invoke the 'setDatetime' method with one
        //:   fewer of the four optional arguments for one of the objects and
        //:   invoke 'setDatetime' with the expected default values (of the
        //:   optional arguments) for the other object.  Verify that the two
        //:   objects compare equal.  (C-4)
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to invoke methods with
        //:   arguments that are outside the valid ranges defined in the
        //:   contracts (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   void setHour(int hour);
        //   void setMinute(int minute);
        //   void setSecond(int second);
        //   void setMillisecond(int millisecond);
        //   void setDatetime(int, int, int, int, int, int, int);
        // --------------------------------------------------------------------

        if (verbose) cout
                   << endl
                   << "TESTING INDIVIDUAL TIME-'set'-ING MANIPULATORS" << endl
                   << "==============================================" << endl;

        const Date RD(2000, 2, 3);       // Ref date (02FEB2000)
        const Time RT(23, 22, 21, 209);  // Ref time (21:22:21.209)
        const Obj  RDT(RD.year(),
                       RD.month(),
                       RD.day(),
                       RT.hour(),
                       RT.minute(),
                       RT.second(),
                       RT.millisecond());

        Obj ARRAY1[] = {  // default value excluded
                         Obj(   1,  1,  1,  0,  0,  0,   0), // start of epoch
                         RDT,                                // arbitrary value
                         Obj(9999, 12, 31, 23, 59, 59, 999)  // end of epoch
                      };
        const int NUM_ARRAY1 =
                              static_cast<int>(sizeof ARRAY1 / sizeof *ARRAY1);

        if (verbose) cout << "\nTesting time-'set' methods." << endl;
        if (verbose) cout << "\tFor ordinary computational values." << endl;

        for (int i = 0; i < NUM_ARRAY1; ++i) {
            const Obj OBJ = ARRAY1[i];

            if (veryVerbose) { T_ P(OBJ) }

            static const struct {
                int d_hour;
                int d_minute;
                int d_second;
                int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },
                {  0,  0,  0, 999  },
                {  0,  0, 59,   0  },
                {  0, 59,  0,   0  },
                { 23,  0,  0,   0  },
                { 23, 22, 21, 209  },  // an ad-hoc value
                { 23, 59, 59, 999  },  // 24:00:00.000 NOT tested here
            };

            const int NUM_VALUES =
                              static_cast<int>(sizeof VALUES / sizeof *VALUES);

            for (int j = 0; i < NUM_VALUES; ++i) {
                const int HOUR   = VALUES[j].d_hour;
                const int MINUTE = VALUES[j].d_minute;
                const int SECOND = VALUES[j].d_second;
                const int MSEC   = VALUES[j].d_msec;

                Obj x(OBJ);  const Obj& X = x;

                if (veryVerbose) {
                    T_ T_ P_(HOUR) P_(MINUTE) P_(SECOND) P_(MSEC) P(X)
                }

                x.setHour(HOUR);
                LOOP2_ASSERT(i, j, OBJ.date()        == X.date());
                LOOP2_ASSERT(i, j, HOUR              == X.hour());
                LOOP2_ASSERT(i, j, OBJ.minute()      == X.minute());
                LOOP2_ASSERT(i, j, OBJ.second()      == X.second());
                LOOP2_ASSERT(i, j, OBJ.millisecond() == X.millisecond());

                x.setMinute(MINUTE);
                LOOP2_ASSERT(i, j, OBJ.date()        == X.date());
                LOOP2_ASSERT(i, j, HOUR              == X.hour());
                LOOP2_ASSERT(i, j, MINUTE            == X.minute());
                LOOP2_ASSERT(i, j, OBJ.second()      == X.second());
                LOOP2_ASSERT(i, j, OBJ.millisecond() == X.millisecond());

                x.setSecond(SECOND);
                LOOP2_ASSERT(i, j, OBJ.date()        == X.date());
                LOOP2_ASSERT(i, j, HOUR              == X.hour());
                LOOP2_ASSERT(i, j, MINUTE            == X.minute());
                LOOP2_ASSERT(i, j, SECOND            == X.second());
                LOOP2_ASSERT(i, j, OBJ.millisecond() == X.millisecond());

                x.setMillisecond(MSEC);
                LOOP2_ASSERT(i, j, OBJ.date()        == X.date());
                LOOP2_ASSERT(i, j, HOUR              == X.hour());
                LOOP2_ASSERT(i, j, MINUTE            == X.minute());
                LOOP2_ASSERT(i, j, SECOND            == X.second());
                LOOP2_ASSERT(i, j, MSEC              == X.millisecond());
            }
        }

        if (verbose) cout << "\tSetting from value 24:00:00.000." << endl;
        {
            const Obj R24;   // Reference object, time = 24:00:00.000
            ASSERT(  1 == R24.year());
            ASSERT(  1 == R24.month());
            ASSERT(  1 == R24.day());
            ASSERT( 24 == R24.hour());
            ASSERT(  0 == R24.minute());
            ASSERT(  0 == R24.second());
            ASSERT(  0 == R24.millisecond());

            Obj x;  const Obj& X = x;  if (veryVerbose) { T_  P_(X) }

            x = R24;                   if (veryVerbose) { T_  P_(X) }
            ASSERT(        24 == X.hour());
            x.setMinute(0);            if (veryVerbose) P(X);
            ASSERT(R24.date() == X.date());
            ASSERT(         0 == X.hour());     // Now 0.
            ASSERT(         0 == X.minute());
            ASSERT(         0 == X.second());
            ASSERT(         0 == X.millisecond());

            x = R24;                   if (veryVerbose) { T_  P_(X) }
            ASSERT(        24 == X.hour());
            x.setMinute(59);           if (veryVerbose) P(X);
            ASSERT(R24.date() == X.date());
            ASSERT(         0 == X.hour());     // Now 0.
            ASSERT(        59 == X.minute());
            ASSERT(         0 == X.second());
            ASSERT(         0 == X.millisecond());

            x = R24;                   if (veryVerbose) { T_ P_(X) }
            ASSERT(        24 == X.hour());
            x.setSecond(0);            if (veryVerbose) P(X);
            ASSERT(R24.date() == X.date());
            ASSERT(         0 == X.hour());     // Now 0.
            ASSERT(         0 == X.minute());
            ASSERT(         0 == X.second());
            ASSERT(         0 == X.millisecond());

            x = R24;                   if (veryVerbose) { T_ P_(X) }
            ASSERT(        24 == X.hour());
            x.setSecond(59);           if (veryVerbose) P(X);
            ASSERT(R24.date() == X.date());
            ASSERT(         0 == X.hour());     // Now 0.
            ASSERT(         0 == X.minute());
            ASSERT(        59 == X.second());
            ASSERT(         0 == X.millisecond());

            x = R24;                   if (veryVerbose) { T_ P_(X) }
            ASSERT(        24 == X.hour());
            x.setMillisecond(0);       if (veryVerbose) P(X);
            ASSERT(R24.date() == X.date());
            ASSERT(         0 == X.hour());     // Now 0.
            ASSERT(         0 == X.minute());
            ASSERT(         0 == X.second());
            ASSERT(         0 == X.millisecond());

            x = R24;                   if (veryVerbose) { T_ P_(X) }
            ASSERT(        24 == X.hour());
            x.setMillisecond(999);     if (veryVerbose) { P(X); cout << endl; }
            ASSERT(R24.date() == X.date());
            ASSERT(         0 == X.hour());     // Now 0.
            ASSERT(         0 == X.minute());
            ASSERT(         0 == X.second());
            ASSERT(       999 == X.millisecond());
        }

        if (verbose) cout << "\tSetting to value 24:00:00.000." << endl;
        {
            const Date DATE(RD);
            Obj mX(DATE, RT);  const Obj& X = mX;

                                       if (veryVerbose) { T_;  P_(X); }
            mX.setHour(24);            if (veryVerbose) { P(X); cout << endl; }
            ASSERT(DATE == X.date());
            ASSERT(24   == X.time().hour());
            ASSERT( 0   == X.time().minute());
            ASSERT( 0   == X.time().second());
            ASSERT( 0   == X.time().millisecond());
        }

        if (verbose)
            cout << "\nTesting: 'setDatetime(7 x int)'" << endl;

        Obj ARRAY2[] = { Obj(),                              // default value
                         Obj(   1,  1,  1,  0,  0,  0,   0), // start of epoch
                         RDT,                                // arbitrary value
                         Obj(9999, 12, 31, 23, 59, 59, 999)  // end of epoch
                      };
        const int NUM_ARRAY2 =
                              static_cast<int>(sizeof ARRAY2 / sizeof *ARRAY2);

        for (int i = 0; i < NUM_ARRAY2; ++i) {
            const Obj OBJ = ARRAY2[i];

            if (veryVerbose) { T_ P(OBJ) }

            static const struct {
                int d_year;
                int d_month;
                int d_day;
                int d_hour;
                int d_minute;
                int d_second;
                int d_msec;
            } VALUES[] = {
                {    1,  1,  1,  24,  0,  0,   0 },  // default

                {    1,  1,  1,   0,  0,  0,   0 },  // start of epoch
                {    1,  1,  2,  24,  0,  0,   0 },
                {   10,  4,  5,   0,  0,  0, 999 },
                {  100,  6,  7,   0,  0, 59,   0 },
                { 1000,  8,  9,   0, 59,  0,   0 },
                { 1100,  1, 31,  23,  0,  0,   0 },
                { 2000, 10, 31,  23, 22, 21, 209 },
                { 2100, 11, 30,  23, 22, 21, 210 },
                { 2200, 12, 31,  23, 22, 21, 211 },
                { 2400, 12, 31,  23, 22, 21, 211 },
                { 9999, 12, 31,  23, 59, 59, 999 },  // end of epoch
            };
            const int NUM_VALUES =
                              static_cast<int>(sizeof VALUES / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES ; ++i) {
                const int YEAR   = VALUES[i].d_year;
                const int MONTH  = VALUES[i].d_month;
                const int DAY    = VALUES[i].d_day;
                const int HOUR   = VALUES[i].d_hour;
                const int MINUTE = VALUES[i].d_minute;
                const int SECOND = VALUES[i].d_second;
                const int MSEC   = VALUES[i].d_msec;

                if (veryVerbose) {
                    T_ T_  P_(YEAR) P_(MONTH) P(DAY)
                    T_ T_  P_(HOUR) P_(MINUTE) P_(SECOND) P(MSEC)
                }

                const Obj R(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, MSEC);

                Obj mX(OBJ);  const Obj& X = mX;

                mX.setDatetime(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, MSEC);

                if (veryVerbose) { T_ T_  P_(R)  P(X) }
                LOOP_ASSERT(i, R == X);
            }
        }

        if (verbose) cout << "\nCheck default values of optional parameters."
                          << endl;
        {
            const int YEAR   = 2;  // arbitrary, non-default values
            const int MONTH  = 3;
            const int DAY    = 4;
            const int HOUR   = 5;
            const int MINUTE = 6;
            const int SECOND = 7;
            const int MSEC   = 8;

            Obj mA0;  const Obj& A0 = mA0;
            Obj mA1;  const Obj& A1 = mA1;
            mA0.setDatetime(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, MSEC);
            mA1.setDatetime(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, MSEC);
            ASSERT(A0 == A1);

            Obj mB0;  const Obj& B0 = mB0;
            Obj mB1;  const Obj& B1 = mB1;
            mB0.setDatetime(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND,    0);
            mB1.setDatetime(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND);
            ASSERT(B0 == B1);

            Obj mC0;  const Obj& C0 = mC0;
            Obj mC1;  const Obj& C1 = mC1;
            mC0.setDatetime(YEAR, MONTH, DAY, HOUR, MINUTE,      0,    0);
            mC1.setDatetime(YEAR, MONTH, DAY, HOUR, MINUTE);
            ASSERT(C0 == C1);

            Obj mD0;  const Obj& D0 = mD0;
            Obj mD1;  const Obj& D1 = mD1;
            mD0.setDatetime(YEAR, MONTH, DAY, HOUR,       0,      0,   0);
            mD1.setDatetime(YEAR, MONTH, DAY, HOUR);
            ASSERT(D0 == D1);

            Obj mE0;  const Obj& E0 = mE0;
            Obj mE1;  const Obj& E1 = mE1;
            mE0.setDatetime(YEAR, MONTH, DAY,    0,       0,      0,    0);
            mE1.setDatetime(YEAR, MONTH, DAY);
            ASSERT(E0 == E1);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'setHour'" << endl;
            {
                Obj x0; ASSERT_SAFE_FAIL(x0.setHour(-1));
                Obj x1; ASSERT_SAFE_PASS(x1.setHour( 0));
                Obj x2; ASSERT_SAFE_PASS(x2.setHour(23));
                Obj x3; ASSERT_SAFE_PASS(x3.setHour(24)); // Is default object.
                Obj x4; ASSERT_SAFE_FAIL(x4.setHour(25));

                Obj nonDefault(1, 1, 2);  const Obj& nD = nonDefault;

                Obj y0(nD); ASSERT_SAFE_PASS(y0.setHour(23));
                Obj y1(nD); ASSERT_SAFE_PASS(y1.setHour(24));
            }

            if (veryVerbose) cout << "\t'setMinute'" << endl;
            {
                Obj x0; ASSERT_SAFE_FAIL(x0.setMinute(-1));
                Obj x1; ASSERT_SAFE_PASS(x1.setMinute( 0));
                Obj x2; ASSERT_SAFE_PASS(x2.setMinute(59));
                Obj x4; ASSERT_SAFE_FAIL(x4.setMinute(60));
            }

            if (veryVerbose) cout << "\t'setSecond'" << endl;
            {
                Obj x0; ASSERT_SAFE_FAIL(x0.setSecond(-1));
                Obj x1; ASSERT_SAFE_PASS(x1.setSecond( 0));
                Obj x2; ASSERT_SAFE_PASS(x2.setSecond(59));
                Obj x4; ASSERT_SAFE_FAIL(x4.setSecond(60));
            }

            if (veryVerbose) cout << "\t'setMillisecond'" << endl;
            {
                Obj x0; ASSERT_SAFE_FAIL(x0.setMillisecond(  -1));
                Obj x1; ASSERT_SAFE_PASS(x1.setMillisecond(   0));
                Obj x2; ASSERT_SAFE_PASS(x2.setMillisecond( 999));
                Obj x4; ASSERT_SAFE_FAIL(x4.setMillisecond(1000));
            }

            if (veryVerbose) cout << "\t'setDatetime'" << endl;
            {
                Obj x0;
                ASSERT_SAFE_PASS(x0.setDatetime(1,  1,  1,  1,  0,  0,   0));
                Obj x1;
                ASSERT_SAFE_FAIL(x1.setDatetime(0,  1,  1,  1,  0,  0,   0));
                Obj x2;
                ASSERT_SAFE_FAIL(x2.setDatetime(1, 13,  1,  1,  0,  0,   0));
                Obj x3;
                ASSERT_SAFE_FAIL(x3.setDatetime(1,  1, 32,  1,  0,  0,   0));
                Obj x4;
                ASSERT_SAFE_FAIL(x4.setDatetime(1,  1,  1, 25,  0,  0,   0));
                Obj x5;
                ASSERT_SAFE_FAIL(x5.setDatetime(1,  1,  1,  1, 60,  0,   0));
                Obj x6;
                ASSERT_SAFE_FAIL(x6.setDatetime(1,  1,  1,  1,  0, 60,   0));
                Obj x7;
                ASSERT_SAFE_FAIL(x7.setDatetime(1,  1,  1,  1,  0,  0,  -1));
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING VALUE CONSTRUCTORS
        //
        // Concerns:
        //: 1 An object created by any of the value constructors has the same
        //:   value as a default created object whose attributes have been set
        //:   to those same values.
        //:
        //: 2 All optional constructor parameters have their expected values.
        //:
        //: 3 A 'Date' object can be converted to a 'Datetime' object having
        //:   the expected value.  (The single-value value constructor is
        //:   not 'explicit'.)
        //
        // Plan:
        //: 1 Specify a set of (unique) valid object values (one per row) in
        //:   terms of their individual attributes, including (a) first, the
        //:   default value, and (b) boundary values corresponding to every
        //:   range of values that each individual attribute can independently
        //:   attain.
        //:
        //: 2 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-1:  (C-1)
        //:
        //:   1 Create a 'const' 'Obj' 'X', using the default constructor and
        //:     primary manipulators, as a reference for objects created using
        //:     the value constructors.
        //:
        //:   2 Use the row attributes to create 'const' objects 'A', 'B', and
        //:     'C' using the three value constructors.
        //:
        //:   3 Compare 'A', 'B', and 'C' with 'X' using the equality operator.
        //:
        //: 3 Create a series of object pairs using the value constructor that
        //:   allows optional parameters.  The initial object of the series has
        //:   no value parameter equal to the corresponding default parameter.
        //:   For each subsequent pair in the series, we omit an additional
        //:   optional parameter then compare to an object created by
        //:   explicitly specifying the expected default values in the value
        //:   constructor.  The two objects, one created with omitted optional
        //:   parameters, the other created with all parameters explicitly
        //:   specified, must compare equal.  (C-2)
        //:
        //: 4 Assign a 'Date' object to a 'Datetime' object.  As there is no
        //:   such assignment operator, the 'Date' object must be converted to
        //:   a 'Datetime' object for the assignment to compile.  Confirm that
        //:   the "time" part has the expected value, "00:00:00.000".
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to construct objects from both
        //:   valid and invalid combinations of attributes.  (using the
        //:   'BSLS_ASSERTTEST_*' macros).  (C-7)
        //
        // Testing:
        //   Datetime(const Date& date);
        //   Datetime(const Date& date, const Time& time);
        //   Datetime(int y, int m, int d, int h, int m, int s, int ms);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING VALUE CONSTRUCTORS" << endl
                          << "==========================" << endl;

        if (verbose) cout << "\nUse a table of distinct object values."
                          << endl;
        {
            const int                    NUM_DATA  = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE   = DATA[ti].d_line;
                const int YEAR   = DATA[ti].d_year;
                const int MONTH  = DATA[ti].d_month;
                const int DAY    = DATA[ti].d_day;
                const int HOUR   = DATA[ti].d_hour;
                const int MINUTE = DATA[ti].d_minute;
                const int SECOND = DATA[ti].d_second;
                const int MSEC   = DATA[ti].d_msec;

                if (veryVerbose) {
                    T_  P_(YEAR) P_(MONTH)  P(DAY)
                    T_  P_(HOUR) P_(MINUTE) P_(SECOND) P(MSEC)
                }

                Obj mX;  const Obj& X = mX;
                mX.setYearMonthDay(YEAR, MONTH, DAY);
                mX.setTime(HOUR, MINUTE, SECOND, MSEC);

                const Obj A(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, MSEC);

                LOOP3_ASSERT(LINE, X, A, X == A);

                Date DATE(YEAR, MONTH, DAY);
                Time TIME(HOUR, MINUTE, SECOND, MSEC);

                const Obj B(DATE, TIME);

                LOOP3_ASSERT(LINE, X, B, X == B);

                Obj mY;  const Obj& Y = mY;
                mY.setYearMonthDay(YEAR, MONTH, DAY);
                mY.setTime(0, 0, 0, 0);

                const Obj C(DATE);

                LOOP3_ASSERT(LINE, Y, C, Y == C);
            }
        }

        if (verbose) cout << "\nCheck default values of optional parameters."
                          << endl;
        {
            const int YEAR   = 2;  // arbitrary, non-default values
            const int MONTH  = 3;
            const int DAY    = 4;
            const int HOUR   = 5;
            const int MINUTE = 6;
            const int SECOND = 7;
            const int MSEC   = 8;

            const Obj A0(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, MSEC);
            const Obj A1(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, MSEC);
            ASSERT(A0 == A1);

            const Obj B0(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND,    0);
            const Obj B1(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND);
            ASSERT(B0 == B1);

            const Obj C0(YEAR, MONTH, DAY, HOUR, MINUTE,      0,    0);
            const Obj C1(YEAR, MONTH, DAY, HOUR, MINUTE);
            ASSERT(C0 == C1);

            const Obj D0(YEAR, MONTH, DAY, HOUR,      0,      0,    0);
            const Obj D1(YEAR, MONTH, DAY, HOUR);
            ASSERT(D0 == D1);

            const Obj E0(YEAR, MONTH, DAY,    0,      0,      0,    0);
            const Obj E1(YEAR, MONTH, DAY);
            ASSERT(E0 == E1);
        }

        if (verbose) cout << "\nTest conversion from 'Date'." << endl;
        {
            const Date DATE(2014, 10, 11);   // arbitrary value
            const Time TIME(0, 0, 0, 0);

             Obj x;  const Obj& X = x;

             x = DATE;  // 'Date' converted to 'Datetime' and assigned.

             ASSERT(DATE == X.date());
             ASSERT(TIME == X.time());
         }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'CTOR(y, m, d, h, m, s, ms)'" << endl;
            {
                ASSERT_SAFE_PASS(    Obj(1,  1,  1,  1,  0,  0,   0));
                ASSERT_SAFE_FAIL_RAW(Obj(0,  1,  1,  1,  0,  0,   0));
                ASSERT_SAFE_FAIL_RAW(Obj(1, 13,  1,  1,  0,  0,   0));
                ASSERT_SAFE_FAIL_RAW(Obj(1,  1, 32,  1,  0,  0,   0));
                ASSERT_SAFE_FAIL_RAW(Obj(1,  1,  1, 25,  0,  0,   0));
                ASSERT_SAFE_FAIL_RAW(Obj(1,  1,  1,  1, 60,  0,   0));
                ASSERT_SAFE_FAIL_RAW(Obj(1,  1,  1,  1,  0, 60,   0));
                ASSERT_SAFE_FAIL_RAW(Obj(1,  1,  1,  1,  0,  0,  -1));
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
        const Date DA(   1,  1,  1);    const Time TA( 24,  0,  0,   0);
        const Date DB(   1,  1,  1);    const Time TB(  0,  0,  0,   0);
        const Date DC(1776,  7,  4);    const Time TC(  0,  0,  0, 999);
        const Date DD(1956,  4, 30);    const Time TD(  0,  0, 59,   0);
        const Date DE(1958,  4, 30);    const Time TE(  0, 59,  0,   0);
        const Date DF(2002,  3, 25);    const Time TF( 23,  0,  0,   0);
        const Date DG(9999, 12, 31);    const Time TG( 23, 22, 21, 209);

        const Obj VA(DA, TA);
        const Obj VB(DB, TB);
        const Obj VC(DC, TC);
        const Obj VD(DD, TD);
        const Obj VE(DE, TE);
        const Obj VF(DF, TF);
        const Obj VG(DG, TG);

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

                    LOOP_ASSERT(i,
                                bdlt::Date::isValidYearDay(T1.date().year(),
                                                       T1.date().dayOfYear()));

                    LOOP_ASSERT(i, (   (      0 <= T1.time().hour()
                                        &&   24 >  T1.time().hour()
                                        &&    0 <= T1.time().minute()
                                        &&   60 >  T1.time().minute()
                                        &&    0 <= T1.time().second()
                                        &&   60 >  T1.time().second()
                                        &&    0 <= T1.time().millisecond()
                                        && 1000 >  T1.time().millisecond())
                                    || (     24 == T1.time().hour()
                                        &&    0 == T1.time().minute()
                                        &&    0 == T1.time().second()
                                        &&    0 == T1.time().millisecond())));

                    LOOP_ASSERT(i,
                                bdlt::Date::isValidYearDay(T2.date().year(),
                                                       T2.date().dayOfYear()));

                    LOOP_ASSERT(i, (   (      0 <= T2.time().hour()
                                        &&   24 >  T2.time().hour()
                                        &&    0 <= T2.time().minute()
                                        &&   60 >  T2.time().minute()
                                        &&    0 <= T2.time().second()
                                        &&   60 >  T2.time().second()
                                        &&    0 <= T2.time().millisecond()
                                        && 1000 >  T2.time().millisecond())
                                    || (     24 == T2.time().hour()
                                        &&    0 == T2.time().minute()
                                        &&    0 == T2.time().second()
                                        &&    0 == T2.time().millisecond())));

                    LOOP_ASSERT(i,
                                bdlt::Date::isValidYearDay(T3.date().year(),
                                                       T3.date().dayOfYear()));

                    LOOP_ASSERT(i, (   (      0 <= T3.time().hour()
                                        &&   24 >  T3.time().hour()
                                        &&    0 <= T3.time().minute()
                                        &&   60 >  T3.time().minute()
                                        &&    0 <= T3.time().second()
                                        &&   60 >  T3.time().second()
                                        &&    0 <= T3.time().millisecond()
                                        && 1000 >  T3.time().millisecond())
                                    || (     24 == T3.time().hour()
                                        &&    0 == T3.time().minute()
                                        &&    0 == T3.time().second()
                                        &&    0 == T3.time().millisecond())));

                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        if (verbose) {
            cout << "\tOn corrupted data." << endl;
        }

        const Obj W;                   // default value
        const Obj X(1, 1, 1, 0);       // original (control)
        const Obj Y(1, 1, 1, 1);       // new (streamed-out)

        // Verify the three objects are distinct.
        ASSERT(W != X);
        ASSERT(W != Y);
        ASSERT(X != Y);

        if (verbose) {
            cout << "\t\tGood stream (for control)." << endl;
        }
        {
            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            bdexStreamOut(out, bdlt::Date(1, 1, 1), VERSION);
            bdexStreamOut(out, bdlt::Time(1), VERSION);

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

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            bdexStreamOut(out, bdlt::Date(1, 1, 1), VERSION);
            bdexStreamOut(out, bdlt::Time(1), VERSION);

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

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            bdexStreamOut(out, bdlt::Date(1, 1, 1), VERSION);
            bdexStreamOut(out, bdlt::Time(1), VERSION);

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
             cout << "\nWire format direct tests." << endl;
        }
        {
            static const struct {
                int         d_lineNum;      // source line number
                int         d_year;         // specification year
                int         d_month;        // specification month
                int         d_day;          // specification day
                int         d_hour;         // specification hour
                int         d_minute;       // specification minute
                int         d_second;       // specification second
                int         d_millisecond;  // specification millisecond
                int         d_version;      // version to stream with
                int         d_length;       // expect output length
                const char *d_fmt_p;        // expected output format
            } DATA[] = {
//LINE  YEAR  MON  DAY  HR  MIN  SEC   MS  VER  LEN  FORMAT
//----  ----  ---  ---  --  ---  ---  ---  ---  ---  -------------------
{ L_,      1,   1,   1, 14,  10,   2, 117,   1,   7,
                                              "\x00\x00\x01\x03\x0a\x3b\x05" },
{ L_,      1,   1,   1, 20,   8,  27, 983,   1,   7,
                                              "\x00\x00\x01\x04\x52\x62\x4f" },
{ L_,   2014,   7,  12,  5,  32,  14,  72,   1,   7,
                                              "\x0b\x38\xc4\x01\x30\x2b\x78" }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_lineNum;
                const int         YEAR        = DATA[i].d_year;
                const int         MONTH       = DATA[i].d_month;
                const int         DAY         = DATA[i].d_day;
                const int         HOUR        = DATA[i].d_hour;
                const int         MINUTE      = DATA[i].d_minute;
                const int         SECOND      = DATA[i].d_second;
                const int         MILLISECOND = DATA[i].d_millisecond;
                const int         VERSION     = DATA[i].d_version;
                const int         LEN         = DATA[i].d_length;
                const char *const FMT         = DATA[i].d_fmt_p;

                // Test using class methods.

                {
                    Obj mX(YEAR, MONTH, DAY,
                           HOUR, MINUTE, SECOND, MILLISECOND);
                    const Obj& X = mX;

                    bslx::ByteOutStream  out(VERSION_SELECTOR, &allocator);
                    bslx::ByteOutStream& rvOut = X.bdexStreamOut(out, VERSION);
                    LOOP_ASSERT(LINE, &out == &rvOut);

                    LOOP_ASSERT(LINE, LEN == static_cast<int>(out.length()));
                    LOOP_ASSERT(LINE, 0 == memcmp(out.data(), FMT, LEN));

                    if (verbose && memcmp(out.data(), FMT, LEN)) {
                        const char *hex = "0123456789abcdef";
                        P_(LINE);
                        for (int j = 0;
                             j < static_cast<int>(out.length()); ++j) {
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
                    Obj mX(YEAR, MONTH, DAY,
                           HOUR, MINUTE, SECOND, MILLISECOND);
                    const Obj& X = mX;

                    using bslx::OutStreamFunctions::bdexStreamOut;

                    bslx::ByteOutStream  out(VERSION_SELECTOR, &allocator);
                    bslx::ByteOutStream& rvOut = bdexStreamOut(out,
                                                               X,
                                                               VERSION);
                    LOOP_ASSERT(LINE, &out == &rvOut);

                    LOOP_ASSERT(LINE, LEN == static_cast<int>(out.length()));
                    LOOP_ASSERT(LINE, 0   == memcmp(out.data(), FMT, LEN));

                    if (verbose && memcmp(out.data(), FMT, LEN)) {
                        const char *hex = "0123456789abcdef";
                        P_(LINE);
                        for (int j = 0;
                             j < static_cast<int>(out.length()); ++j) {
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
        //: 2 Specify a set of (unique) valid object values (one per row) in
        //:   terms of their individual attributes, including (a) first, the
        //:   default value, and (b) boundary values corresponding to every
        //:   range of values that each individual attribute can independently
        //:   attain.
        //:
        //: 3 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-2:
        //:
        //:   1 Use the default constructor, primary manipulators, and the
        //:     copy-constructor create two 'const' 'Obj', 'Z' and 'ZZ', each
        //:     having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-2:
        //:
        //:   3 For each of the iterations (P-3.2):
        //:
        //:     1 Use the default constructor and primary manipulators to
        //:       create a modifiable 'Obj', 'mX', having the value 'W'.
        //:
        //:     2 Assign 'mX' from 'Z'.
        //:
        //:     3 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-3)
        //:
        //:     4 Use the equality-comparison operator to verify that:
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-4)
        //:
        //: 4 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3: (C-5)
        //:
        //:   1 Use the default constructor and primary manipulators to create
        //:     a modifiable 'Obj' 'mX' and a 'const' 'Obj' 'ZZ'.
        //:
        //:   2 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   3 Assign 'mX' from 'Z'.
        //:
        //:   4 Verify that the address of the return value is the same as that
        //:     of 'mX'.  (C-1)
        //:
        //:   5 Use the equality-comparison operator to verify that the target
        //:     object, 'Z', still has the same value as that of 'ZZ'.  (C-4)
        //
        // Testing:
        //   Datetime& operator=(const Datetime& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ASSIGNMENT OPERATOR" << endl
                          << "===========================" << endl;

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
        {
            const int                    NUM_DATA  = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE1   = DATA[ti].d_line;
                const int YEAR1   = DATA[ti].d_year;
                const int MONTH1  = DATA[ti].d_month;
                const int DAY1    = DATA[ti].d_day;
                const int HOUR1   = DATA[ti].d_hour;
                const int MINUTE1 = DATA[ti].d_minute;
                const int SECOND1 = DATA[ti].d_second;
                const int MSEC1   = DATA[ti].d_msec;

                if (veryVerbose) {
                    T_  P_(YEAR1) P_(MONTH1)  P(DAY1)
                    T_  P_(HOUR1) P_(MINUTE1) P_(SECOND1) P(MSEC1)
                }

                Obj mZ;  const Obj& Z = mZ;
                mZ.setYearMonthDay(YEAR1, MONTH1, DAY1);
                mZ.setTime(HOUR1, MINUTE1, SECOND1, MSEC1);

                const Obj ZZ(Z);

                if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

                // Ensure the first row of the table contains the
                // default-constructed value.

                static bool firstFlag = true;
                if (firstFlag) {
                    LOOP3_ASSERT(LINE1, Obj(), Z, Obj() == Z);
                    firstFlag = false;
                }

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int LINE2   = DATA[tj].d_line;
                    const int YEAR2   = DATA[tj].d_year;
                    const int MONTH2  = DATA[tj].d_month;
                    const int DAY2    = DATA[tj].d_day;
                    const int HOUR2   = DATA[tj].d_hour;
                    const int MINUTE2 = DATA[tj].d_minute;
                    const int SECOND2 = DATA[tj].d_second;
                    const int MSEC2   = DATA[tj].d_msec;

                    if (veryVerbose) {
                        T_  P_(YEAR2) P_(MONTH2)  P(DAY2)
                        T_  P_(HOUR2) P_(MINUTE2) P_(SECOND2) P(MSEC2)
                    }

                    Obj mX;  const Obj& X = mX;
                    mX.setYearMonthDay(YEAR2, MONTH2, DAY2);
                    mX.setTime(HOUR2, MINUTE2, SECOND2, MSEC2);

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    LOOP4_ASSERT(LINE1, LINE2, Z, X,
                                 (Z == X) == (LINE1 == LINE2));

                    Obj *mR = &(mX = Z);

                    LOOP4_ASSERT(LINE1, LINE2,  Z,   X,  Z == X);
                    LOOP4_ASSERT(LINE1, LINE2, mR, &mX, mR == &mX);

                    LOOP4_ASSERT(LINE1, LINE2, ZZ, Z, ZZ == Z);
                }

                // self-assignment

                {
                    Obj mX;  const Obj& X = mX;
                    mX.setYearMonthDay(YEAR1, MONTH1, DAY1);
                    mX.setTime(HOUR1, MINUTE1, SECOND1, MSEC1);

                    const Obj  ZZ(X);
                    const Obj& Z = mX;

                    LOOP3_ASSERT(LINE1, ZZ, Z, ZZ == Z);

                    Obj *mR = &(mX = Z);

                    LOOP3_ASSERT(LINE1, ZZ,   Z, ZZ == Z);
                    LOOP3_ASSERT(LINE1, mR, &mX, mR == &mX);
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

        if (verbose) cout << "Not implemented for 'bdlt::Datetime'." << endl;

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
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
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1..12)
        //:
        //:   1 Use the default constructor and the primary manipulators to
        //:     create two 'const' 'Obj', 'Z' and 'ZZ', each having the value
        //:     'V'.
        //:
        //:   2 Use 'const' 'Obj' 'Z' to copy-construct object 'X'.  Using a
        //:     'const' object as the source is a compile-time check of C-2.
        //:
        //:   3 Use the equality-comparison operator to verify that:
        //:
        //:       1 The newly constructed object, 'X', has the same value as
        //:         that of 'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-3)
        //
        // Testing:
        //   Datetime(const Datetime& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING COPY CONSTRUCTOR" << endl
                          << "========================" << endl;

        const int                    NUM_DATA  = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE   = DATA[ti].d_line;
            const int YEAR   = DATA[ti].d_year;
            const int MONTH  = DATA[ti].d_month;
            const int DAY    = DATA[ti].d_day;
            const int HOUR   = DATA[ti].d_hour;
            const int MINUTE = DATA[ti].d_minute;
            const int SECOND = DATA[ti].d_second;
            const int MSEC   = DATA[ti].d_msec;

            if (veryVerbose) {
                T_  P_(YEAR) P_(MONTH)  P(DAY)
                T_  P_(HOUR) P_(MINUTE) P_(SECOND) P(MSEC)
            }

            Obj mZZ;  const Obj& ZZ = mZZ;  // control
            mZZ.setYearMonthDay(YEAR, MONTH, DAY);
            mZZ.setTime(HOUR, MINUTE, SECOND, MSEC);

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE, Obj(), ZZ, Obj() == ZZ);
                firstFlag = false;
            }

            Obj mZ;  const Obj& Z = mZ;
            mZ.setYearMonthDay(YEAR, MONTH, DAY);
            mZ.setTime(HOUR, MINUTE, SECOND, MSEC);

            const Obj X(Z);

            if (veryVerbose) { T_  P_(X) P_(Z) P(ZZ) }

            LOOP_ASSERT(LINE, X == Z);
            LOOP_ASSERT(LINE, Z == ZZ);
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compares
        //:   equal.
        //:
        //: 2 All salient attributes participate in the comparison.
        //:
        //: 3 'true  == (X == X)' (i.e., identity)
        //:
        //: 4 'false == (X != X)' (i.e., identity)
        //:
        //: 5 'X == Y' if and only if 'Y == X' (i.e., commutativity)
        //:
        //: 6 'X != Y' if and only if 'Y != X' (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if '!(X == Y)'
        //:
        //: 8 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 9 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //:10 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //:
        //:11 The equality operator's signature and return type are standard.
        //:
        //:12 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.  (C-8..9, 11..12)
        //:
        //: 2 Note that ubiquitous test allocators are already installed as the
        //:   global allocator and the default allocator.
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their individual salient
        //:   attributes such that for each salient attribute, there exists a
        //:   pair of rows that differ (slightly) in only the column
        //:   corresponding to that attribute (subject to the [complex]
        //:   constraints on those attributes).  Note that none of the
        //:   attributes of these objects allocate memory.
        //:
        //: 4 For each row 'R1' in the table of P-3: (C-1..7)
        //:
        //:   1 Create a single object and use it to verify the reflexive
        //:     (anti-reflexive) property of equality (inequality) in the
        //:     presence of aliasing.  (C-3..4)
        //:
        //:   2 For each row 'R2' in the table of P-3: (C-1..2, 5..7)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects created
        //:       from 'R1' and 'R2', respectively, are expected to have the
        //:       same value.
        //:
        //:     2 Create an object 'X', having the value 'R1'.
        //:
        //:     3 Create an object 'Y', having the value 'R2'.
        //:
        //:     4 Verify the commutativity property and expected return value
        //:       for both '==' and '!='.  (C-1..2, 5..7)
        //:
        //: 3 As for all test cases, the default and global allocators defined
        //:   just under 'main' are checked for any use by objects under test.
        //:   (C-10)
        //
        // Testing:
        //   bool operator==(const Datetime& lhs, const Datetime& rhs);
        //   bool operator!=(const Datetime& lhs, const Datetime& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING EQUALITY-COMPARISON OPERATORS" << endl
                          << "=====================================" << endl;

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
            "\nCreate a table of distinct, but similar object values." << endl;

        static const struct {
            int d_line;
            int d_year;
            int d_month;
            int d_day;
            int d_hour;
            int d_minute;
            int d_second;
            int d_msec;
        } DATA[] = {

            // There are two sets of values in this table.  The first row of
            // each represents a "baseline" object value and the each of the
            // subsequent rows in each differ (slightly) in exactly one salient
            // attribute.

            //LINE YEAR      MONTH   DAY     HOUR    MINUTE  SECOND  MSEC
            //---- ----      ------  ------  ------  ------  ------  -------
            { L_,    1    ,  1    ,  1    ,  24    ,  0    ,  0    ,   0     },
            { L_,    1    ,  1    ,  1    ,   0    ,  0    ,  0    ,   0     },

            { L_,    1    ,  1    ,  1    ,  23    ,  0    ,  0    ,   0     },
            { L_,    1    ,  1    ,  1    ,  23    ,  0    ,  0    ,   0 + 1 },
            { L_,    1    ,  1    ,  1    ,  23    ,  0    ,  0 + 1,   0     },
            { L_,    1    ,  1    ,  1    ,  23    ,  0 + 1,  0    ,   0     },
                                          // 23 + 1 equals 24 (done earlier)
            { L_,    1    ,  1    ,  1 + 1,  23    ,  0    ,  0    ,   0     },
            { L_,    1    ,  1 + 1,  1    ,  23    ,  0    ,  0    ,   0     },
            { L_,    1 + 1,  1    ,  1    ,  23    ,  0    ,  0    ,   0     },

            { L_, 9999    , 12    , 31    ,  23    , 59    , 59    , 999     },
            { L_, 9999    , 12    , 31    ,  23    , 59    , 59    , 999 - 1 },
            { L_, 9999    , 12    , 31    ,  23    , 59    , 59 - 1, 999     },
            { L_, 9999    , 12    , 31    ,  23    , 59 - 1, 59    , 999     },
            { L_, 9999    , 12    , 31    ,  23 - 1, 59    , 59    , 999     },
            { L_, 9999    , 12    , 31 - 1,  23    , 59    , 59    , 999     },
            { L_, 9999    , 12 - 1, 31 - 1,  23    , 59    , 59    , 999     },
            { L_, 9999 - 1, 12    , 31    ,  23    , 59    , 59    , 999     },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE1   = DATA[ti].d_line;
            const int YEAR1   = DATA[ti].d_year;
            const int MONTH1  = DATA[ti].d_month;
            const int DAY1    = DATA[ti].d_day;
            const int HOUR1   = DATA[ti].d_hour;
            const int MINUTE1 = DATA[ti].d_minute;
            const int SECOND1 = DATA[ti].d_second;
            const int MSEC1   = DATA[ti].d_msec;

            if (veryVerbose) {
                T_  P_(YEAR1) P_(MONTH1)  P(DAY1)
                T_  P_(HOUR1) P_(MINUTE1) P_(SECOND1) P(MSEC1)
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int LINE2   = DATA[tj].d_line;
                const int YEAR2   = DATA[tj].d_year;
                const int MONTH2  = DATA[tj].d_month;
                const int DAY2    = DATA[tj].d_day;
                const int HOUR2   = DATA[tj].d_hour;
                const int MINUTE2 = DATA[tj].d_minute;
                const int SECOND2 = DATA[tj].d_second;
                const int MSEC2   = DATA[tj].d_msec;

                if (veryVerbose) {
                    T_ T_ P_(YEAR2) P_(MONTH2)  P(DAY2)
                    T_ T_ P_(HOUR2) P_(MINUTE2) P_(SECOND2) P(MSEC2)
                }

                const bool EXP = ti == tj;  // expected for equality comparison

                Obj mX;  const Obj& X = mX;
                mX.setYearMonthDay(YEAR1, MONTH1, DAY1);
                mX.setTime(HOUR1, MINUTE1, SECOND1, MSEC1);

                Obj mY;  const Obj& Y = mY;
                mY.setYearMonthDay(YEAR2, MONTH2, DAY2);
                mY.setTime(HOUR2, MINUTE2, SECOND2, MSEC2);

                if (veryVerbose) { T_ T_ T_ P_(EXP) P_(X) P(Y) }

                LOOP4_ASSERT(LINE1, LINE2, X, Y,  EXP == (X == Y));
                LOOP4_ASSERT(LINE1, LINE2, Y, X,  EXP == (Y == X));

                LOOP4_ASSERT(LINE1, LINE2, X, Y, !EXP == (X != Y));
                LOOP4_ASSERT(LINE1, LINE2, Y, X, !EXP == (Y != X));
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
        //:   1 The two attributes (i.e, the "date" and "time" portions) always
        //:     appear on a single line, separated by a '_' character.
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
        //   ostream& operator<<(ostream &stream, const Datetime &object);
        //   int printToBuffer(char *result, int size) const;
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

        const Date DA(   1,  1,  1);  const Time TA( 0,  0,  0,   0);
        const Date DZ(9999, 12, 31);  const Time TZ(23, 59, 59, 999);

        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spacesPerLevel;
            Date        d_date;
            Time        d_time;
            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 } -->  4 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL  DATE  TIME  EXPECTED
        //---- - ---  ---   ----  --------
        { L_,  0,  0,  DA,    TA, "01JAN0001_00:00:00.000" NL },
        { L_,  0,  1,  DA,    TA, "01JAN0001_00:00:00.000" NL },
        { L_,  0, -1,  DA,    TA, "01JAN0001_00:00:00.000"    },

        { L_,  0, -8,  DA,    TA, "01JAN0001_00:00:00.000" NL }, // default

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } -->  6 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL  DATE  TIME  EXPECTED
        //---- - ---  ---   ----  --------
        { L_,  3,  0,  DA,    TA, "01JAN0001_00:00:00.000"             NL },
        { L_,  3,  2,  DA,    TA, "      01JAN0001_00:00:00.000"       NL },
        { L_,  3, -2,  DA,    TA, "      01JAN0001_00:00:00.000"          },

        { L_,  3, -8,  DA,    TA, "            01JAN0001_00:00:00.000" NL },
                                                                 // default

        { L_, -3,  0,  DA,    TA, "01JAN0001_00:00:00.000" NL },
        { L_, -3,  2,  DA,    TA, "01JAN0001_00:00:00.000" NL },
        { L_, -3, -2,  DA,    TA, "01JAN0001_00:00:00.000"    },

        { L_, -3, -8,  DA,    TA, "01JAN0001_00:00:00.000" NL }, // default

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }            -->  1 expected o/p
        // -----------------------------------------------------------------

        //LINE L SPL  DATE  TIME  EXPECTED
        //---- - ---  ---   ----  --------
        { L_,  2,  3,   DZ,   TZ, "      31DEC9999_23:59:59.999" NL },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 }   x { -8 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL  DATE  TIME  EXPECTED
        //---- - ---  ---   ----  --------
        { L_, -8, -8,  DA,    TA, "01JAN0001_00:00:00.000" NL }, // default
        { L_, -8, -8,  DZ,    TZ, "31DEC9999_23:59:59.999" NL }, // default

        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 }   x { -9 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL  DATE  TIME  EXPECTED
        //---- - ---  ---   ----  --------
        { L_, -9, -9,  DA,    TA, "01JAN0001_00:00:00.000" },  // 'operator<<'
        { L_, -9, -9,  DZ,    TZ, "31DEC9999_23:59:59.999" },  // 'operator<<'

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
                const Date        DATE   = DATA[ti].d_date;
                const Time        TIME   = DATA[ti].d_time;
                const char *const EXP    = DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(L) P_(SPL) P_(DATE) P(TIME) }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                Obj x;  const Obj& X = x;
                x.setYearMonthDay(DATE.year(), DATE.month(), DATE.day());
                x.setTime(TIME.hour(),
                          TIME.minute(),
                          TIME.second(),
                          TIME.millisecond());

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
                int         d_year;
                int         d_month;
                int         d_day;
                int         d_hour;
                int         d_minute;
                int         d_second;
                int         d_msec;
                int         d_numBytes;
                const char *d_expected_p;
            } DATA[] = {
      //--------^
      //LINE YEAR MON DAY  HR MIN SEC MSEC  LIMIT  EXPECTED
      //---- ---- --- ---  -- --- --- ----  -----  -------------------------
      { L_,    1,  1,  1,   0,  0,  0,   0,   100,  "01JAN0001_00:00:00.000" },
      { L_, 1999,  1,  1,  23, 22, 21, 209,   100,  "01JAN1999_23:22:21.209" },
      { L_, 2000,  2,  1,  23, 22, 21, 210,   100,  "01FEB2000_23:22:21.210" },
      { L_, 2001,  3,  1,  23, 22, 21, 211,   100,  "01MAR2001_23:22:21.211" },
      { L_, 9999, 12, 31,  23, 59, 59, 999,   100,  "31DEC9999_23:59:59.999" },
      { L_, 9999, 12, 31,  23, 59, 59, 999,     0,  "" },
      { L_, 9999, 12, 31,  23, 59, 59, 999,     1,  "" },
      { L_, 9999, 12, 31,  23, 59, 59, 999,     2,  "3" },
      { L_, 9999, 12, 31,  23, 59, 59, 999,    10,  "31DEC9999" },
      { L_, 9999, 12, 31,  23, 59, 59, 999,    22,  "31DEC9999_23:59:59.99"  },
      { L_, 9999, 12, 31,  23, 59, 59, 999,    23,  "31DEC9999_23:59:59.999" },
      { L_, 9999, 12, 31,  23, 59, 59, 999,    24,  "31DEC9999_23:59:59.999" },
      //--------v
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
                const int         YEAR     = DATA[ti].d_year;
                const int         MONTH    = DATA[ti].d_month;
                const int         DAY      = DATA[ti].d_day;
                const int         HOUR     = DATA[ti].d_hour;
                const int         MINUTE   = DATA[ti].d_minute;
                const int         SECOND   = DATA[ti].d_second;
                const int         MSEC     = DATA[ti].d_msec;
                const int         LIMIT    = DATA[ti].d_numBytes;
                const char *const EXPECTED = DATA[ti].d_expected_p;

                if (veryVerbose) {
                    T_  P_(YEAR)
                        P_(MONTH)
                        P(DAY)
                    T_  P_(HOUR)
                        P_(MINUTE)
                        P_(SECOND)
                        P(MSEC)
                    T_  P_(LIMIT)
                    T_  P(EXPECTED)
                }

                char buf[BUF_SIZE];

                // Preset 'buf' to "unset" values.
                memset(buf, XX, sizeof(buf));

                Obj x;  const Obj& X = x;
                x.setYearMonthDay(YEAR, MONTH, DAY);
                x.setTime(HOUR, MINUTE, SECOND, MSEC);

                char      *p = buf + sizeof(buf)/2;
                const int  RC = X.printToBuffer(p, LIMIT);

                LOOP2_ASSERT(LINE, RC, 22 == RC);  // Should always return 22
                                                   // because size of datetime
                                                   // format is fixed.


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
                    LOOP3_ASSERT(LINE, buf, EXPECTED,
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
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'printToBuffer' method" << endl;
            {
                const int SIZE = 128;
                char      buf[SIZE];

                const Obj X;

                ASSERT_SAFE_PASS(X.printToBuffer(buf, SIZE));
                ASSERT_SAFE_PASS(X.printToBuffer(buf,  0  ));
                ASSERT_SAFE_FAIL(X.printToBuffer(0,   SIZE));
                ASSERT_SAFE_FAIL(X.printToBuffer(buf, -1  ));
                ASSERT_SAFE_FAIL(X.printToBuffer(0,   -1  ));
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS
        //
        // Concerns:
        //: 1 All ACCESSOR methods are declared 'const'.
        //:
        //: 2 The two basic accessors provide appropriate references to the
        //:   (fully-tested) contained "date" and "time" parts.
        //:
        //: 3 The the nine accessors to the fields of the "date" and "time"
        //:   parts are forwarded to the the accessors of those parts.
        //
        // Plan:
        //: 1 Invoke each ACCESSOR via a reference providing non-modifiable
        //:   access to the object.  Compilation of this test driver confirms
        //:   that each ACCESS is 'const'-qualified.
        //:
        //: 2 Define a sequence of 'int' values for 'year', 'month', ...
        //:   'second', 'millisecond', each valid for its corresponding field
        //:   in in the "date" or "time" part.
        //:
        //:   1 Create a default object and use the primary manipulators to set
        //:     its value.
        //:
        //:   2 Confirm that the 'date()' and 'time()' methods refer to an
        //:     object equal to one independently created from those same 'int'
        //:     input values.  (C-2)
        //:
        //:   3 Confirm that the nine field accessors return values that match
        //:     the corresponding fields of the "date" and "time" parts of the
        //:     object.
        //
        // Testing:
        //   Date date() const;
        //   Time time() const;
        //   int year() const;
        //   int month() const;
        //   int day() const;
        //   int dayOfYear() const;
        //   DayOfWeek::Day dayOfWeek() const;
        //   int hour() const;
        //   int minute() const;
        //   int second() const;
        //   int millisecond() const;
        //   CONCERN: All accessor methods are declared 'const'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ACCESSORS" << endl
                          << "=================" << endl;

        if (verbose) cout << "\nTesting: 'date', 'time', ..." << endl;
        {
            static const struct {
                int d_year;
                int d_month;
                int d_day;
                int d_hour;
                int d_minute;
                int d_second;
                int d_msec;
            } VALUES[] = {
                {    1,  1,  1,     0,  0,  0,   0  },
                { 1700,  7, 31,    23, 22, 21, 206  },
                { 1800,  8, 31,    23, 22, 21, 207  },
                { 1900,  9, 30,    23, 22, 21, 208  },
                { 2000, 10, 31,    23, 22, 21, 209  },
                { 2100, 11, 30,    23, 22, 21, 210  },
                { 9999, 12, 31,    23, 59, 59, 999  },

                {    1,  1,  1,    24,  0,  0,   0  },
            };

            const int NUM_VALUES =
                              static_cast<int>(sizeof VALUES / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int YEAR   = VALUES[i].d_year;
                const int MONTH  = VALUES[i].d_month;
                const int DAY    = VALUES[i].d_day;
                const int HOUR   = VALUES[i].d_hour;
                const int MINUTE = VALUES[i].d_minute;
                const int SECOND = VALUES[i].d_second;
                const int MSEC   = VALUES[i].d_msec;

                Obj x;  const Obj& X = x;
                x.setYearMonthDay(YEAR, MONTH, DAY);
                x.setTime(HOUR, MINUTE, SECOND, MSEC);

                if (veryVerbose) {
                    T_  P_(YEAR)
                        P_(MONTH)
                        P(DAY)
                    T_  P_(HOUR)
                        P_(MINUTE)
                        P_(SECOND)
                        P(MSEC)
                }
                LOOP_ASSERT(i, Date(YEAR, MONTH,  DAY) == X.date());
                LOOP_ASSERT(i, Time(HOUR,
                                    MINUTE,
                                    SECOND,
                                    MSEC)              == X.time());

                LOOP_ASSERT(i, X.date().year()         == X.year());
                LOOP_ASSERT(i, X.date().month()        == X.month());
                LOOP_ASSERT(i, X.date().day()          == X.day());
                LOOP_ASSERT(i, X.date().dayOfYear()    == X.dayOfYear());
                LOOP_ASSERT(i, X.date().dayOfWeek()    == X.dayOfWeek());
                LOOP_ASSERT(i, X.time().hour()         == X.hour());
                LOOP_ASSERT(i, X.time().minute()       == X.minute());
                LOOP_ASSERT(i, X.time().second()       == X.second());
                LOOP_ASSERT(i, X.time().millisecond()  == X.millisecond());
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'gg'
        //   Void for 'bdlt::Datetime'.
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //   Reserved for testing generator function 'gg' .
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING GENERATOR FUNCTION 'gg'" << endl
                          << "===============================" << endl;

        if (verbose) cout << "void for 'bdlt::Datetime'." << endl;

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING DEFAULT CTOR, DTOR, AND PRIMARY MANIPULATORS
        //
        // Concerns:
        //: 1 A default constructed object has the expected value.
        //:
        //: 2 Each primary manipulator forwards its input to the corresponding
        //:   (fully-tested) member object ("date" or "time", as appropriate).
        //:
        //: 3 The default arguments for 'setTime' have the expected value (0).
        //:
        //: 4 Each primary manipulator sets one part of the object ("date" or
        //:   "time", as appropriate) and has no effect on the other.
        //:
        //: 5 The object can be destroyed.
        //
        // Plan:
        //: 1 Default construct an object and check its value using the basic
        //:   accessors.  (C-1)
        //:
        //: 2 Define a sequence of independent test values that explore the
        //:   boundaries of valid values for the "date" and "time" parts of the
        //:   object.  Use the default constructor to create an object, the
        //:   primary manipulators to set its value, and the basic accessors to
        //:   verify its value.  (C-2)
        //:
        //:   1 For each test, confirm that the value of the "other part"
        //:     remains unchanged.  (C-4)
        //:
        //: 3 Confirm that each primary manipulator leaves the other "part"
        //:   unchanged.
        //:
        //: 4 The destructor is exercised on each test object as it leaves
        //:   scope.  (C-5)
        //:
        //: 5 Construct a series of object pairs.  For each pair, invoke the
        //:   'setDatetimeIfValid' method by explicitly specifying the expected
        //:   default value for an optional argument for one object, and by not
        //:   omitting the optional argument for the other object.  The two
        //:   objects should compare equal.  For each pair of the series, omit
        //:   one or more of the four optional arguments.  (C-3)
        //
        // Testing:
        //   Datetime();
        //   ~Datetime();
        //   BOOTSTRAP: void setYearMonthDay(int year, int month, int day);
        //   void setTime(int hour, int min = 0, int sec = 0, int ms = 0);
        // --------------------------------------------------------------------

        if (verbose) cout
             << endl
             << "TESTING DEFAULT CTOR, DTOR, AND PRIMARY MANIPULATORS" << endl
             << "====================================================" << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;

        Obj x;  const Obj& X = x;        if (veryVerbose) P(X);

        ASSERT( 1 == X.date().year());
        ASSERT( 1 == X.date().month());
        ASSERT( 1 == X.date().day());
        ASSERT(24 == X.time().hour());
        ASSERT( 0 == X.time().minute());
        ASSERT( 0 == X.time().second());
        ASSERT( 0 == X.time().millisecond());

        if (verbose) cout << "\nTesting 'setTime'." << endl;
        {
            static const struct {
                int d_hour;
                int d_minute;
                int d_second;
                int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },
                {  0,  0,  0, 999  },
                {  0,  0, 59,   0  },
                {  0, 59,  0,   0  },
                { 23,  0,  0,   0  },
                { 23, 22, 21, 209  },  // an ad-hoc value
                { 23, 59, 59, 999  },

                { 24,  0,  0,   0  },
            };
            const int NUM_VALUES =
                              static_cast<int>(sizeof VALUES / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int HOUR   = VALUES[i].d_hour;
                const int MINUTE = VALUES[i].d_minute;
                const int SECOND = VALUES[i].d_second;
                const int MSEC   = VALUES[i].d_msec;

                if (veryVerbose) {
                    T_  P_(HOUR)
                        P_(MINUTE)
                        P_(SECOND)
                        P(MSEC)
                    T_  P_(X.time().hour())
                        P_(X.time().minute())
                        P_(X.time().second())
                        P(X.time().millisecond())
                }

                Obj x;  const Obj& X = x;
                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                LOOP_ASSERT(i, HOUR   == X.time().hour());
                LOOP_ASSERT(i, MINUTE == X.time().minute());
                LOOP_ASSERT(i, SECOND == X.time().second());
                LOOP_ASSERT(i, MSEC   == X.time().millisecond());
                LOOP_ASSERT(i, Date() == X.date());
            }
        }

        if (verbose) cout << "\nCheck default values of optional parameters."
                          << endl;
        {
            const int HOUR = 5; // arbitrary, non-default values
            const int MIN  = 6;
            const int SECS = 7;
            const int MSEC = 8;

            Obj mA0;  const Obj& A0 = mA0;
            Obj mA1;  const Obj& A1 = mA1;
            mA0.setTime(HOUR, MIN, SECS, MSEC);
            mA1.setTime(HOUR, MIN, SECS, MSEC);
            ASSERT(A0 == A1);

            Obj mB0;  const Obj& B0 = mB0;
            Obj mB1;  const Obj& B1 = mB1;
            mB0.setTime(HOUR, MIN, SECS,    0);
            mB1.setTime(HOUR, MIN, SECS);
            ASSERT(B0 == B1);

            Obj mC0;  const Obj& C0 = mC0;
            Obj mC1;  const Obj& C1 = mC1;
            mC0.setTime(HOUR, MIN,    0,    0);
            mC1.setTime(HOUR, MIN);
            ASSERT(C0 == C1);

            Obj mD0;  const Obj& D0 = mD0;
            Obj mD1;  const Obj& D1 = mD1;
            mD0.setTime(HOUR,   0,    0,    0);
            mD1.setTime(HOUR);
            ASSERT(D0 == D1);
        }

        if (verbose) cout << "\nTesting 'setYearMonthDay'." << endl;
        {
            static const struct {
                int d_year;
                int d_month;
                int d_day;
            } VALUES[] = {
                {    1,  1,  1 },
                {   10,  4,  5 },
                {  100,  6,  7 },
                { 1000,  8,  9 },
                { 1100,  1, 31 },
                { 1200,  2, 29 },
                { 1300,  3, 31 },
                { 1400,  4, 30 },
                { 1500,  5, 31 },
                { 1600,  6, 30 },
                { 1700,  7, 31 },
                { 1800,  8, 31 },
                { 1900,  9, 30 },
                { 2000, 10, 31 },
                { 2100, 11, 30 },
                { 2200, 12, 31 },
                { 2400, 12, 31 },
                { 9999, 12, 31 },
            };

            const int NUM_VALUES =
                              static_cast<int>(sizeof VALUES / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int YEAR  = VALUES[i].d_year;
                const int MONTH = VALUES[i].d_month;
                const int DAY   = VALUES[i].d_day;

                Obj mX;  const Obj& X = mX;

                mX.setYearMonthDay(YEAR, MONTH, DAY);

                if (veryVerbose) { T_ P_(YEAR)
                                      P_(MONTH)
                                      P(DAY)
                                   T_ P_(X.date().year())
                                      P_(X.date().month())
                                      P(X.date().day())
                }

                LOOP_ASSERT(i, YEAR   == X.date().year());
                LOOP_ASSERT(i, MONTH  == X.date().month());
                LOOP_ASSERT(i, DAY    == X.date().day());
                LOOP_ASSERT(i, Time() == X.time());
            }
        }

        if (verbose) cout << "\nTest independence of primary manipulators."
                          << endl;
        {
            const int YRA = 1, MOA = 2, DAA = 3;           // y, m, d for A
            const int YRB = 4, MOB = 5, DAB = 6;           // y, m, d for B

            const int HRA = 1, MIA = 2, SCA = 3, MSA = 4;  // h, m, s, ms for A
            const int HRB = 5, MIB = 6, SCB = 7, MSB = 8;  // h, m, s, ms for B

            Obj mX;  const Obj& X = mX;

            mX.setYearMonthDay(YRA, MOA, DAA);

            mX.setTime(HRA, MIA, SCA, MSA);
            ASSERT(YRA == X.date().year());
            ASSERT(MOA == X.date().month());
            ASSERT(DAA == X.date().day());
            ASSERT(HRA == X.time().hour());
            ASSERT(MIA == X.time().minute());
            ASSERT(SCA == X.time().second());
            ASSERT(MSA == X.time().millisecond());

            mX.setTime(HRB, MIB, SCB, MSB);
            ASSERT(YRA == X.date().year());
            ASSERT(MOA == X.date().month());
            ASSERT(DAA == X.date().day());
            ASSERT(HRB == X.time().hour());
            ASSERT(MIB == X.time().minute());
            ASSERT(SCB == X.time().second());
            ASSERT(MSB == X.time().millisecond());

            mX.setYearMonthDay(YRB, MOB, DAB);
            ASSERT(YRB == X.date().year());
            ASSERT(MOB == X.date().month());
            ASSERT(DAB == X.date().day());
            ASSERT(HRB == X.time().hour());
            ASSERT(MIB == X.time().minute());
            ASSERT(SCB == X.time().second());
            ASSERT(MSB == X.time().millisecond());
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
        //: 1 Create an object 'w' (init to 'A').   { w:A }
        //: 2 Create an object 'x' (copy from 'w'). { w:A x:A }
        //: 3 Set 'w' to 'B'.                       { w:B x:A }
        //: 4 Create an object 'y' (default ctor).  { w:B x:A y:D }
        //: 5 Create an object 'z' (copy from 'y'). { w:B x:A y:D z:D}
        //: 6 Set 'y' to 'C'.                       { w:B x:A y:C z:D}
        //: 7 Assign 'x' from 'w'.                  { w:B x:B y:C z:D}
        //: 8 Assign 'x' from 'y'.                  { w:B x:C y:C z:D}
        //: 9 Assign 'w' from 'w' (aliasing).       { w:B x:B y:C z:D}
        //:10 Exercise the constructor that takes a 'Date' object, but sets the
        //:   time to the default value.  Confirm that the time can
        //:   subsequently be set to a new value.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Values for testing

        const int YRA = 1, MOA = 2, DAA = 3;           // y, m, d for A
        const int YRB = 4, MOB = 5, DAB = 6;           // y, m, d for B
        const int YRC = 7, MOC = 8, DAC = 9;           // y, m, d for C

        const int HRA = 1, MIA = 2, SCA = 3, MSA = 4;  // h, m, s, ms for A
        const int HRB = 5, MIB = 6, SCB = 7, MSB = 8;  // h, m, s, ms for B
        const int HRC = 9, MIC = 9, SCC = 9, MSC = 9;  // h, m, s, ms for C

        const Date DA(YRA, MOA, DAA),
                   DB(YRB, MOB, DAB),
                   DC(YRC, MOC, DAC);

        const Time TA(HRA, MIA, SCA, MSA),
                   TB(HRB, MIB, SCB, MSB),
                   TC(HRC, MIC, SCC, MSC);

        int year, month, day;              // reusable variables for 'get' call
        int h, m, s, ms;                   // reusable variables for 'get' call

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object 'w' (init to 'A')."
                             "\t\t{ w:A }" << endl;

        Obj mX1(DA, TA);  const Obj& X1 = mX1;
        if (verbose) { T_  P(X1); }

        if (veryVerbose) cout << "\ta. Check initial state of 'w'." << endl;

        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X1.date().getYearMonthDay(&year, &month, &day);
        X1.time().getTime(&h, &m, &s, &ms);
        ASSERT(YRA == year);     ASSERT(MOA == month);     ASSERT(DAA == day);
        ASSERT(HRA == h);        ASSERT(MIA == m);
        ASSERT(SCA == s);        ASSERT(MSA == ms);
        ASSERT(YRA == X1.date().year());
        ASSERT(MOA == X1.date().month());
        ASSERT(DAA == X1.date().day());
        ASSERT(HRA == X1.time().hour());
        ASSERT(MIA == X1.time().minute());
        ASSERT(SCA == X1.time().second());
        ASSERT(MSA == X1.time().millisecond());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: 'w' <op> 'w'." << endl;

        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object 'x' (copy from 'w')."
                             "\t\t{ w:A x:A }" << endl;

        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { T_  P(X2); }

        if (veryVerbose) cout <<
                                "\ta. Check the initial state of 'x'." << endl;

        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X2.date().getYearMonthDay(&year, &month, &day);
        X2.time().getTime(&h, &m, &s, &ms);
        ASSERT(YRA == year);     ASSERT(MOA == month);     ASSERT(DAA == day);
        ASSERT(HRA == h);        ASSERT(MIA == m);
        ASSERT(SCA == s);        ASSERT(MSA == ms);
        ASSERT(YRA == X2.date().year());
        ASSERT(MOA == X2.date().month());
        ASSERT(DAA == X2.date().day());
        ASSERT(HRA == X2.time().hour());
        ASSERT(MIA == X2.time().minute());
        ASSERT(SCA == X2.time().second());
        ASSERT(MSA == X2.time().millisecond());

        if (veryVerbose) cout <<
                     "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set 'w' to a new value 'B'."
                             "\t\t\t{ w:B x:A }" << endl;

        mX1.setDate(DB);
        mX1.setTime(HRB, MIB, SCB, MSB);

        if (veryVeryVerbose) { T_  P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of 'w'." << endl;

        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X1.date().getYearMonthDay(&year, &month, &day);
        X1.time().getTime(&h, &m, &s, &ms);
        ASSERT(YRB == year);     ASSERT(MOB == month);     ASSERT(DAB == day);
        ASSERT(HRB == h);        ASSERT(MIB == m);
        ASSERT(SCB == s);        ASSERT(MSB == ms);
        ASSERT(YRB == X1.date().year());
        ASSERT(MOB == X1.date().month());
        ASSERT(DAB == X1.date().day());
        ASSERT(HRB == X1.time().hour());
        ASSERT(MIB == X1.time().minute());
        ASSERT(SCB == X1.time().second());
        ASSERT(MSB == X1.time().millisecond());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: 'w' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create a default object 'y()'."
                             "\t\t{ w:B x:A y:D }" << endl;

        Obj mX3;  const Obj& X3 = mX3;
        if (veryVeryVerbose) { T_  P(X3); }

        if (veryVerbose) cout << "\ta. Check initial state of 'y'." << endl;

        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X3.date().getYearMonthDay(&year, &month, &day);
        X3.time().getTime(&h, &m, &s, &ms);
        ASSERT(1 == year);     ASSERT(1 == month);     ASSERT(1 == day);
        ASSERT(24 == h);  ASSERT(0 == m);  ASSERT(0 == s);  ASSERT(0 == ms);

        if (veryVerbose) cout <<
            "\tb. Try equality operators: 'y' <op> 'w', 'x', 'y'." << endl;

        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 5. Create an object 'z' (copy from 'y')."
                             "\t\t{ w:A x:A y:D  z:D }" << endl;

        Obj mX4(X3);  const Obj& X4 = mX4;

        if (veryVeryVerbose) { T_  P(X4); }

        if (veryVerbose) cout << "\ta. Check initial state of 'z'." << endl;

        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X4.date().getYearMonthDay(&year, &month, &day);
        X4.time().getTime(&h, &m, &s, &ms);
        ASSERT(1 == year);     ASSERT(1 == month);     ASSERT(1 == day);
        ASSERT(24 == h);  ASSERT(0 == m);  ASSERT(0 == s);  ASSERT(0 == ms);

        if (veryVerbose) cout
            << "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'."
            << endl;

        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Set 'y' to a new value 'C'."
                             "\t\t\t{ w:B x:A y:C z:D }" << endl;

        mX3.setDate(DC);
        mX3.setTime(HRC, MIC, SCC, MSC);

        if (verbose) { T_  P(X3); }

        if (veryVerbose) cout << "\ta. Check new state of 'y'." << endl;

        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X3.date().getYearMonthDay(&year, &month, &day);
        X3.time().getTime(&h, &m, &s, &ms);
        ASSERT(YRC == year);     ASSERT(MOC == month);     ASSERT(DAC == day);
        ASSERT(HRC == h);        ASSERT(MIC == m);
        ASSERT(SCC == s);        ASSERT(MSC == ms);
        ASSERT(YRC == X3.date().year());
        ASSERT(MOC == X3.date().month());
        ASSERT(DAC == X3.date().day());
        ASSERT(HRC == X3.time().hour());
        ASSERT(MIC == X3.time().minute());
        ASSERT(SCC == X3.time().second());
        ASSERT(MSC == X3.time().millisecond());

        if (veryVerbose) cout
             << "\tb. Try equality operators: 'y' <op> 'w', 'x', 'y', 'z'."
             << endl;

        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 7. Assign 'x' from 'w'."
                             "\t\t\t\t{ w:B x:B y:C z:D }" << endl;

        mX2 = X1;
        if (veryVeryVerbose) { T_  P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of 'x'." << endl;

        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X2.date().getYearMonthDay(&year, &month, &day);
        X2.time().getTime(&h, &m, &s, &ms);
        ASSERT(YRB == year);     ASSERT(MOB == month);     ASSERT(DAB == day);
        ASSERT(HRB == h);        ASSERT(MIB == m);
        ASSERT(SCB == s);        ASSERT(MSB == ms);
        ASSERT(YRB == X2.date().year());
        ASSERT(MOB == X2.date().month());
        ASSERT(DAB == X2.date().day());
        ASSERT(HRB == X2.time().hour());
        ASSERT(MIB == X2.time().minute());
        ASSERT(SCB == X2.time().second());
        ASSERT(MSB == X2.time().millisecond());

        if (veryVerbose) cout
            << "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'."
            << endl;

        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 8. Assign 'x' from 'y'."
                             "\t\t\t\t{ w:B x:C y:C z:D }" << endl;

        mX2 = X3;
        if (veryVeryVerbose) { T_  P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of 'x'." << endl;

        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X2.date().getYearMonthDay(&year, &month, &day);
        X2.time().getTime(&h, &m, &s, &ms);
        ASSERT(YRC == year);     ASSERT(MOC == month);     ASSERT(DAC == day);
        ASSERT(HRC == h);        ASSERT(MIC == m);
        ASSERT(SCC == s);        ASSERT(MSC == ms);
        ASSERT(YRC == X2.date().year());
        ASSERT(MOC == X2.date().month());
        ASSERT(DAC == X2.date().day());
        ASSERT(HRC == X2.time().hour());
        ASSERT(MIC == X2.time().minute());
        ASSERT(SCC == X2.time().second());
        ASSERT(MSC == X2.time().millisecond());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x <op> w, x, y, z." << endl;

        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 9. Assign 'w' from 'w' (aliasing)."
                             "\t\t\t{ w:B x:C y:C z:D }" << endl;

        mX1 = X1;

        if (veryVeryVerbose) { T_  P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of 'w'." << endl;

        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X1.date().getYearMonthDay(&year, &month, &day);
        X1.time().getTime(&h, &m, &s, &ms);
        ASSERT(YRB == year);     ASSERT(MOB == month);     ASSERT(DAB == day);
        ASSERT(HRB == h);        ASSERT(MIB == m);
        ASSERT(SCB == s);        ASSERT(MSB == ms);
        ASSERT(YRB == X1.date().year());
        ASSERT(MOB == X1.date().month());
        ASSERT(DAB == X1.date().day());
        ASSERT(HRB == X1.time().hour());
        ASSERT(MIB == X1.time().minute());
        ASSERT(SCB == X1.time().second());
        ASSERT(MSB == X1.time().millisecond());

        if (veryVerbose) cout
            << "\tb. Try equality operators: 'w' <op> 'w', 'x', 'y', 'z'."
            << endl;

        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
                     "\n10. Create an object 'x5' initialized to a "
                     "date, but with the time value \"00:00:00.000\"." << endl;

        Obj mX5(DB);  const Obj &X5 = mX5;

        if (veryVeryVerbose) { T_  P(X5); }

        if (veryVerbose) cout << "\ta. Check the state of 'x5'." << endl;

        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X5.date().getYearMonthDay(&year, &month, &day);
        X5.time().getTime(&h, &m, &s, &ms);
        ASSERT(YRB == year);     ASSERT(MOB == month);     ASSERT(DAB == day);
        ASSERT(  0 == h);        ASSERT(  0 == m);
        ASSERT(  0 == s);        ASSERT(  0 == ms);
        ASSERT(YRB == X5.date().year());
        ASSERT(MOB == X5.date().month());
        ASSERT(DAB == X5.date().day());
        ASSERT(  0 == X5.time().hour());
        ASSERT(  0 == X5.time().minute());
        ASSERT(  0 == X5.time().second());
        ASSERT(  0 == X5.time().millisecond());

        if (veryVerbose) cout << "\tb. Modify the time field of 'x5'." << endl;

        mX5.setTime(HRB, MIB, SCB, MSB);
        if (verbose) { T_  P(X5); }
        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X5.date().getYearMonthDay(&year, &month, &day);
        X5.time().getTime(&h, &m, &s, &ms);
        ASSERT(YRB == year);     ASSERT(MOB == month);     ASSERT(DAB == day);
        ASSERT(HRB == h);        ASSERT(MIB == m);
        ASSERT(SCB == s);        ASSERT(MSB == ms);
        ASSERT(YRB == X5.date().year());
        ASSERT(MOB == X5.date().month());
        ASSERT(DAB == X5.date().day());
        ASSERT(HRB == X5.time().hour());
        ASSERT(MIB == X5.time().minute());
        ASSERT(SCB == X5.time().second());
        ASSERT(MSB == X5.time().millisecond());

        ASSERT(1 == (X5 == X1));        ASSERT(0 == (X5 != X1));
        ASSERT(0 == (X5 == X2));        ASSERT(1 == (X5 != X2));
        ASSERT(0 == (X5 == X3));        ASSERT(1 == (X5 != X3));
        ASSERT(0 == (X5 == X4));        ASSERT(1 == (X5 != X4));
        ASSERT(1 == (X5 == X5));        ASSERT(0 == (X5 != X5));

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the default allocator.

    ASSERT(dam.isTotalSame());

    // CONCERN: In no case does memory come from the global allocator.

    ASSERT(gam.isTotalSame());

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
