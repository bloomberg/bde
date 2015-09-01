// bdlt_delegatingdateimputil.t.cpp                                   -*-C++-*-
#include <bdlt_delegatingdateimputil.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

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
// All but three of the functions in the component under test delegate to
// corresponding functions in one of two other utility components depending on
// which calendar mode is in effect.  To test these delegating functions it is
// sufficient to use argument values that produce different results between the
// two calendar modes.  The three functions that pertain to setting and testing
// the calendar mode are trivially tested in case 1.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] static bool isLeapYear(int year);
// [ 2] static int  lastDayOfMonth(int year, int month);
// [ 2] static int  numLeapYears(int year1, int year2);
// [ 3] static bool isValidSerial(int serialDay);
// [ 3] static bool isValidYearDay(int year, int dayOfYear);
// [ 3] static bool isValidYearMonthDay(int year, int month, int day);
// [ 3] static bool isValidYearMonthDayNoCache(int y, int m, int day);
// [ 4] static int  ydToSerial(int year, int dayOfYear);
// [ 4] static int  ymdToSerial(int year, int month, int day);
// [ 4] static int  ymdToSerialNoCache(int year, int month, int day);
// [ 5] static int  serialToDayOfYear(int serialDay);
// [ 5] static void serialToYd(int *year, int *dayOfYear, int serialDay);
// [ 5] static int  ymdToDayOfYear(int year, int month, int day);
// [ 6] static int  serialToDay(int serialDay);
// [ 6] static int  serialToDayNoCache(int serialDay);
// [ 6] static int  serialToMonth(int serialDay);
// [ 6] static int  serialToMonthNoCache(int serialDay);
// [ 6] static int  serialToYear(int serialDay);
// [ 6] static int  serialToYearNoCache(int serialDay);
// [ 6] static void serialToYmd(int *y, int *m, int *d, int sD);
// [ 6] static void serialToYmdNoCache(int *y, int *m, int *d, int sD);
// [ 6] static int  ydToDay(int year, int dayOfYear);
// [ 6] static void ydToMd(int *month, int *day, int year, int dayOfYear);
// [ 6] static int  ydToMonth(int year, int dayOfYear);
// [ 7] static int  serialToDayOfWeek(int serialDay);
// [ 7] static int  ydToDayOfWeek(int year, int dayOfYear);
// [ 7] static int  ymdToDayOfWeek(int year, int month, int day);
// [ 1] static void disableProlepticGregorianMode();
// [ 1] static void enableProlepticGregorianMode();
// [ 1] static bool isProlepticGregorianMode();
// ----------------------------------------------------------------------------

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

typedef bdlt::DelegatingDateImpUtil Util;

typedef bdlt::PosixDateImpUtil      PosixUtil;
typedef bdlt::SerialDateImpUtil     SerialUtil;

// ============================================================================
//                     GLOBAL CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                              USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;  (void)veryVerbose;
    const bool     veryVeryVerbose = argc > 4;  (void)veryVeryVerbose;
    const bool veryVeryVeryVerbose = argc > 5;  (void)veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 7: {
        // --------------------------------------------------------------------
        // TESTING '*ToDayOfWeek'
        //
        // Concerns:
        //: 1 Each method delegates to the mode-specific implementation that is
        //:   currently in effect and produces the expected result.
        //
        // Plan:
        //: 1 In both calendar modes, exercise each method on data that
        //:   produces different results between the two modes.  Use the
        //:   underlying 'PosixDateImpUtil' and 'SerialDateImpUtil' as oracles
        //:   to verify the expected behavior.  (C-1)
        //
        // Testing:
        //   static int  serialToDayOfWeek(int serialDay);
        //   static int  ydToDayOfWeek(int year, int dayOfYear);
        //   static int  ymdToDayOfWeek(int year, int month, int day);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING '*ToDayOfWeek'" << endl
                          << "======================" << endl;

        enum { SUN = 1, MON, TUE, WED, THU, FRI, SAT };

        if (verbose) cout << "\nTesting 'serialToDayOfWeek'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            ASSERT(MON ==       Util::serialToDayOfWeek(1));
            ASSERT(SAT ==  PosixUtil::serial2weekday(   1));
            ASSERT(MON == SerialUtil::serialToDayOfWeek(1));
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            ASSERT(SAT ==       Util::serialToDayOfWeek(1));
            ASSERT(SAT ==  PosixUtil::serial2weekday(   1));
            ASSERT(MON == SerialUtil::serialToDayOfWeek(1));
        }

        if (verbose) cout << "\nTesting 'ydToDayOfWeek'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            ASSERT(WED ==       Util::ydToDayOfWeek(1, 3));
            ASSERT(MON ==  PosixUtil::yd2weekday(   1, 3));
            ASSERT(WED == SerialUtil::ydToDayOfWeek(1, 3));
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            ASSERT(MON ==       Util::ydToDayOfWeek(1, 3));
            ASSERT(MON ==  PosixUtil::yd2weekday(   1, 3));
            ASSERT(WED == SerialUtil::ydToDayOfWeek(1, 3));
        }

        if (verbose) cout << "\nTesting 'ymdToDayOfWeek'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            ASSERT(FRI ==       Util::ymdToDayOfWeek(1, 1, 5));
            ASSERT(WED ==  PosixUtil::ymd2weekday(   1, 1, 5));
            ASSERT(FRI == SerialUtil::ymdToDayOfWeek(1, 1, 5));
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            ASSERT(WED ==       Util::ymdToDayOfWeek(1, 1, 5));
            ASSERT(WED ==  PosixUtil::ymd2weekday(   1, 1, 5));
            ASSERT(FRI == SerialUtil::ymdToDayOfWeek(1, 1, 5));
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING TO YEAR-MONTH-DAY DATE
        //
        // Concerns:
        //: 1 Each method delegates to the mode-specific implementation that is
        //:   currently in effect and produces the expected result.
        //
        // Plan:
        //: 1 In both calendar modes, exercise each method on data that
        //:   produces different results between the two modes.  Use the
        //:   underlying 'PosixDateImpUtil' and 'SerialDateImpUtil' as oracles
        //:   to verify the expected behavior.  (C-1)
        //
        // Testing:
        //   static int  serialToDay(int serialDay);
        //   static int  serialToDayNoCache(int serialDay);
        //   static int  serialToMonth(int serialDay);
        //   static int  serialToMonthNoCache(int serialDay);
        //   static int  serialToYear(int serialDay);
        //   static int  serialToYearNoCache(int serialDay);
        //   static void serialToYmd(int *y, int *m, int *d, int sD);
        //   static void serialToYmdNoCache(int *y, int *m, int *d, int sD);
        //   static int  ydToDay(int year, int dayOfYear);
        //   static void ydToMd(int *month, int *day, int year, int dayOfYear);
        //   static int  ydToMonth(int year, int dayOfYear);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING TO YEAR-MONTH-DAY DATE" << endl
                          << "==============================" << endl;

        // In proleptic Gregorian (POSIX), serial value 36525 is 0101/01/01
        // (0100/12/31).

        if (verbose) cout << "\nTesting 'serialToDay[NoCache]'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            ASSERT( 1 ==       Util::serialToDay(       36525));
            ASSERT(31 ==  PosixUtil::serial2day(        36525));
            ASSERT( 1 == SerialUtil::serialToDay(       36525));

            ASSERT( 1 ==       Util::serialToDayNoCache(36525));
            ASSERT(31 ==  PosixUtil::serial2dayNoCache( 36525));
            ASSERT( 1 == SerialUtil::serialToDayNoCache(36525));
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            ASSERT(31 ==       Util::serialToDay(       36525));
            ASSERT(31 ==  PosixUtil::serial2day(        36525));
            ASSERT( 1 == SerialUtil::serialToDay(       36525));

            ASSERT(31 ==       Util::serialToDayNoCache(36525));
            ASSERT(31 ==  PosixUtil::serial2dayNoCache( 36525));
            ASSERT( 1 == SerialUtil::serialToDayNoCache(36525));
        }

        if (verbose) cout << "\nTesting 'serialToMonth[NoCache]'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            ASSERT( 1 ==       Util::serialToMonth(       36525));
            ASSERT(12 ==  PosixUtil::serial2month(        36525));
            ASSERT( 1 == SerialUtil::serialToMonth(       36525));

            ASSERT( 1 ==       Util::serialToMonthNoCache(36525));
            ASSERT(12 ==  PosixUtil::serial2monthNoCache( 36525));
            ASSERT( 1 == SerialUtil::serialToMonthNoCache(36525));
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            ASSERT(12 ==       Util::serialToMonth(       36525));
            ASSERT(12 ==  PosixUtil::serial2month(        36525));
            ASSERT( 1 == SerialUtil::serialToMonth(       36525));

            ASSERT(12 ==       Util::serialToMonthNoCache(36525));
            ASSERT(12 ==  PosixUtil::serial2monthNoCache( 36525));
            ASSERT( 1 == SerialUtil::serialToMonthNoCache(36525));
        }

        if (verbose) cout << "\nTesting 'serialToYear[NoCache]'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            ASSERT(101 ==       Util::serialToYear(       36525));
            ASSERT(100 ==  PosixUtil::serial2year(        36525));
            ASSERT(101 == SerialUtil::serialToYear(       36525));

            ASSERT(101 ==       Util::serialToYearNoCache(36525));
            ASSERT(100 ==  PosixUtil::serial2yearNoCache( 36525));
            ASSERT(101 == SerialUtil::serialToYearNoCache(36525));
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            ASSERT(100 ==       Util::serialToYear(       36525));
            ASSERT(100 ==  PosixUtil::serial2year(        36525));
            ASSERT(101 == SerialUtil::serialToYear(       36525));

            ASSERT(100 ==       Util::serialToYearNoCache(36525));
            ASSERT(100 ==  PosixUtil::serial2yearNoCache( 36525));
            ASSERT(101 == SerialUtil::serialToYearNoCache(36525));
        }

        if (verbose) cout << "\nTesting 'serialToYmd[NoCache]'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            int y, m, d;

                  Util::serialToYmd(       &y, &m, &d, 36525);
            ASSERT(101 == y);
            ASSERT(  1 == m);
            ASSERT(  1 == d);

             PosixUtil::serial2ymd(        &y, &m, &d, 36525);
            ASSERT(100 == y);
            ASSERT( 12 == m);
            ASSERT( 31 == d);

            SerialUtil::serialToYmd(       &y, &m, &d, 36525);
            ASSERT(101 == y);
            ASSERT(  1 == m);
            ASSERT(  1 == d);

                  Util::serialToYmdNoCache(&y, &m, &d, 36525);
            ASSERT(101 == y);
            ASSERT(  1 == m);
            ASSERT(  1 == d);

             PosixUtil::serial2ymdNoCache( &y, &m, &d, 36525);
            ASSERT(100 == y);
            ASSERT( 12 == m);
            ASSERT( 31 == d);

            SerialUtil::serialToYmdNoCache(&y, &m, &d, 36525);
            ASSERT(101 == y);
            ASSERT(  1 == m);
            ASSERT(  1 == d);
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            int y, m, d;

                  Util::serialToYmd(       &y, &m, &d, 36525);
            ASSERT(100 == y);
            ASSERT( 12 == m);
            ASSERT( 31 == d);

             PosixUtil::serial2ymd(        &y, &m, &d, 36525);
            ASSERT(100 == y);
            ASSERT( 12 == m);
            ASSERT( 31 == d);

            SerialUtil::serialToYmd(       &y, &m, &d, 36525);
            ASSERT(101 == y);
            ASSERT(  1 == m);
            ASSERT(  1 == d);

                  Util::serialToYmdNoCache(&y, &m, &d, 36525);
            ASSERT(100 == y);
            ASSERT( 12 == m);
            ASSERT( 31 == d);

             PosixUtil::serial2ymdNoCache( &y, &m, &d, 36525);
            ASSERT(100 == y);
            ASSERT( 12 == m);
            ASSERT( 31 == d);

            SerialUtil::serialToYmdNoCache(&y, &m, &d, 36525);
            ASSERT(101 == y);
            ASSERT(  1 == m);
            ASSERT(  1 == d);
        }

        if (verbose) cout << "\nTesting 'ydToDay'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            ASSERT( 1 ==       Util::ydToDay(100, 335));
            ASSERT(30 ==  PosixUtil::yd2day( 100, 335));
            ASSERT( 1 == SerialUtil::ydToDay(100, 335));
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            ASSERT(30 ==       Util::ydToDay(100, 335));
            ASSERT(30 ==  PosixUtil::yd2day( 100, 335));
            ASSERT( 1 == SerialUtil::ydToDay(100, 335));
        }

        if (verbose) cout << "\nTesting 'ydToMd'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            int m, d;

                  Util::ydToMd(&m, &d, 100, 335);
            ASSERT(12 == m);
            ASSERT( 1 == d);

             PosixUtil::yd2md( &m, &d, 100, 335);
            ASSERT(11 == m);
            ASSERT(30 == d);

            SerialUtil::ydToMd(&m, &d, 100, 335);
            ASSERT(12 == m);
            ASSERT( 1 == d);
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            int m, d;

                  Util::ydToMd(&m, &d, 100, 335);
            ASSERT(11 == m);
            ASSERT(30 == d);

             PosixUtil::yd2md( &m, &d, 100, 335);
            ASSERT(11 == m);
            ASSERT(30 == d);

            SerialUtil::ydToMd(&m, &d, 100, 335);
            ASSERT(12 == m);
            ASSERT( 1 == d);
        }

        if (verbose) cout << "\nTesting 'ydToMonth'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            ASSERT(12 ==       Util::ydToMonth(100, 335));
            ASSERT(11 ==  PosixUtil::yd2month( 100, 335));
            ASSERT(12 == SerialUtil::ydToMonth(100, 335));
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            ASSERT(11 ==       Util::ydToMonth(100, 335));
            ASSERT(11 ==  PosixUtil::yd2month( 100, 335));
            ASSERT(12 == SerialUtil::ydToMonth(100, 335));
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING TO YEAR-DAY DATE
        //
        // Concerns:
        //: 1 Each method delegates to the mode-specific implementation that is
        //:   currently in effect and produces the expected result.
        //
        // Plan:
        //: 1 In both calendar modes, exercise each method on data that
        //:   produces different results between the two modes.  Use the
        //:   underlying 'PosixDateImpUtil' and 'SerialDateImpUtil' as oracles
        //:   to verify the expected behavior.  (C-1)
        //
        // Testing:
        //   static int  serialToDayOfYear(int serialDay);
        //   static void serialToYd(int *year, int *dayOfYear, int serialDay);
        //   static int  ymdToDayOfYear(int year, int month, int day);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING TO YEAR-DAY DATE" << endl
                          << "========================" << endl;

        // In proleptic Gregorian (POSIX), serial value 36525 is 0101/001
        // (0100/366).

        if (verbose) cout << "\nTesting 'serialToDayOfYear'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            ASSERT(  1 ==       Util::serialToDayOfYear(36525));
            ASSERT(366 ==  PosixUtil::serial2dayOfYear( 36525));
            ASSERT(  1 == SerialUtil::serialToDayOfYear(36525));
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            ASSERT(366 ==       Util::serialToDayOfYear(36525));
            ASSERT(366 ==  PosixUtil::serial2dayOfYear( 36525));
            ASSERT(  1 == SerialUtil::serialToDayOfYear(36525));
        }

        if (verbose) cout << "\nTesting 'serialToYd'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            int y, d;

                  Util::serialToYd(&y, &d, 36525);
            ASSERT(101 == y);
            ASSERT(  1 == d);

             PosixUtil::serial2yd( &y, &d, 36525);
            ASSERT(100 == y);
            ASSERT(366 == d);

            SerialUtil::serialToYd(&y, &d, 36525);
            ASSERT(101 == y);
            ASSERT(  1 == d);
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            int y, d;

                  Util::serialToYd(&y, &d, 36525);
            ASSERT(100 == y);
            ASSERT(366 == d);

             PosixUtil::serial2yd( &y, &d, 36525);
            ASSERT(100 == y);
            ASSERT(366 == d);

            SerialUtil::serialToYd(&y, &d, 36525);
            ASSERT(101 == y);
            ASSERT(  1 == d);
        }

        if (verbose) cout << "\nTesting 'ymdToDayOfYear'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            ASSERT(60 ==       Util::ymdToDayOfYear(1700, 3, 1));
            ASSERT(61 ==  PosixUtil::ymd2dayOfYear (1700, 3, 1));
            ASSERT(60 == SerialUtil::ymdToDayOfYear(1700, 3, 1));
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            ASSERT(61 ==       Util::ymdToDayOfYear(1700, 3, 1));
            ASSERT(61 ==  PosixUtil::ymd2dayOfYear (1700, 3, 1));
            ASSERT(60 == SerialUtil::ymdToDayOfYear(1700, 3, 1));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING '*ToSerial'
        //
        // Concerns:
        //: 1 Each method delegates to the mode-specific implementation that is
        //:   currently in effect and produces the expected result.
        //
        // Plan:
        //: 1 In both calendar modes, exercise each method on data that
        //:   produces different results between the two modes.  Use the
        //:   underlying 'PosixDateImpUtil' and 'SerialDateImpUtil' as oracles
        //:   to verify the expected behavior.  (C-1)
        //
        // Testing:
        //   static int  ydToSerial(int year, int dayOfYear);
        //   static int  ymdToSerial(int year, int month, int day);
        //   static int  ymdToSerialNoCache(int year, int month, int day);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING '*ToSerial'" << endl
                          << "===================" << endl;

        if (verbose) cout << "\nTesting 'ydToSerial'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            ASSERT(36525 ==       Util::ydToSerial(101, 1));
            ASSERT(36526 ==  PosixUtil::yd2serial( 101, 1));
            ASSERT(36525 == SerialUtil::ydToSerial(101, 1));
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            ASSERT(36526 ==       Util::ydToSerial(101, 1));
            ASSERT(36526 ==  PosixUtil::yd2serial( 101, 1));
            ASSERT(36525 == SerialUtil::ydToSerial(101, 1));
        }

        // The maximum proleptic Gregorian (POSIX) serial value is 3652059
        // (3652061).

        if (verbose) cout << "\nTesting 'ymdToSerial[NoCache]'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            ASSERT(3652058 ==       Util::ymdToSerial(       9999, 12, 30));
            ASSERT(3652060 ==  PosixUtil::ymd2serial(        9999, 12, 30));
            ASSERT(3652058 == SerialUtil::ymdToSerial(       9999, 12, 30));

            ASSERT(3652058 ==       Util::ymdToSerialNoCache(9999, 12, 30));
            ASSERT(3652060 ==  PosixUtil::ymd2serialNoCache( 9999, 12, 30));
            ASSERT(3652058 == SerialUtil::ymdToSerialNoCache(9999, 12, 30));
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            ASSERT(3652060 ==       Util::ymdToSerial(       9999, 12, 30));
            ASSERT(3652060 ==  PosixUtil::ymd2serial(        9999, 12, 30));
            ASSERT(3652058 == SerialUtil::ymdToSerial(       9999, 12, 30));

            ASSERT(3652060 ==       Util::ymdToSerialNoCache(9999, 12, 30));
            ASSERT(3652060 ==  PosixUtil::ymd2serialNoCache( 9999, 12, 30));
            ASSERT(3652058 == SerialUtil::ymdToSerialNoCache(9999, 12, 30));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'isValid*'
        //
        // Concerns:
        //: 1 Each method delegates to the mode-specific implementation that is
        //:   currently in effect and produces the expected result.
        //
        // Plan:
        //: 1 In both calendar modes, exercise each method on data that
        //:   produces different results between the two modes.  Use the
        //:   underlying 'PosixDateImpUtil' and 'SerialDateImpUtil' as oracles
        //:   to verify the expected behavior.  (C-1)
        //
        // Testing:
        //   static bool isValidSerial(int serialDay);
        //   static bool isValidYearDay(int year, int dayOfYear);
        //   static bool isValidYearMonthDay(int year, int month, int day);
        //   static bool isValidYearMonthDayNoCache(int y, int m, int day);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'isValid*'" << endl
                          << "==================" << endl;

        // The maximum proleptic Gregorian (POSIX) serial value is 3652059
        // (3652061).

        if (verbose) cout << "\nTesting 'isValidSerial'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            ASSERT(false ==       Util::isValidSerial(    3652060));
            ASSERT(true  ==  PosixUtil::isValidSerialDate(3652060));
            ASSERT(false == SerialUtil::isValidSerial(    3652060));
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            ASSERT(true  ==       Util::isValidSerial(    3652060));
            ASSERT(true  ==  PosixUtil::isValidSerialDate(3652060));
            ASSERT(false == SerialUtil::isValidSerial(    3652060));
        }

        if (verbose) cout << "\nTesting 'isValidYearDay'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            ASSERT(false ==       Util::isValidYearDay(    1700, 366));
            ASSERT(true  ==  PosixUtil::isValidYearDayDate(1700, 366));
            ASSERT(false == SerialUtil::isValidYearDay(    1700, 366));
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            ASSERT(true  ==       Util::isValidYearDay(    1700, 366));
            ASSERT(true  ==  PosixUtil::isValidYearDayDate(1700, 366));
            ASSERT(false == SerialUtil::isValidYearDay(    1700, 366));
        }

        if (verbose) cout << "\nTesting 'isValidYearMonthDay[NoCache]'."
                          << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            ASSERT(false ==       Util::isValidYearMonthDay(1700, 2, 29));
            ASSERT(true  ==  PosixUtil::isValidCalendarDate(1700, 2, 29));
            ASSERT(false == SerialUtil::isValidYearMonthDay(1700, 2, 29));

            ASSERT(true  ==       Util::isValidYearMonthDayNoCache(
                                                            1752, 9, 10));
            ASSERT(false ==  PosixUtil::isValidCalendarDateNoCache(
                                                            1752, 9, 10));
            ASSERT(true  == SerialUtil::isValidYearMonthDayNoCache(
                                                            1752, 9, 10));
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            ASSERT(true  ==       Util::isValidYearMonthDay(1700, 2, 29));
            ASSERT(true  ==  PosixUtil::isValidCalendarDate(1700, 2, 29));
            ASSERT(false == SerialUtil::isValidYearMonthDay(1700, 2, 29));

            ASSERT(false ==       Util::isValidYearMonthDayNoCache(
                                                            1752, 9, 10));
            ASSERT(false ==  PosixUtil::isValidCalendarDateNoCache(
                                                            1752, 9, 10));
            ASSERT(true  == SerialUtil::isValidYearMonthDayNoCache(
                                                            1752, 9, 10));
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING LEAP YEAR FUNCTIONS
        //
        // Concerns:
        //: 1 Each method delegates to the mode-specific implementation that is
        //:   currently in effect and produces the expected result.
        //
        // Plan:
        //: 1 In both calendar modes, exercise each method on data that
        //:   produces different results between the two modes.  Use the
        //:   underlying 'PosixDateImpUtil' and 'SerialDateImpUtil' as oracles
        //:   to verify the expected behavior.  (C-1)
        //
        // Testing:
        //   static bool isLeapYear(int year);
        //   static int  lastDayOfMonth(int year, int month);
        //   static int  numLeapYears(int year1, int year2);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING LEAP YEAR FUNCTIONS" << endl
                          << "===========================" << endl;

        if (verbose) cout << "\nTesting 'isLeapYear'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            ASSERT(false ==       Util::isLeapYear(1700));
            ASSERT(true  ==  PosixUtil::isLeapYear(1700));
            ASSERT(false == SerialUtil::isLeapYear(1700));
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            ASSERT(true  ==       Util::isLeapYear(1700));
            ASSERT(true  ==  PosixUtil::isLeapYear(1700));
            ASSERT(false == SerialUtil::isLeapYear(1700));
        }

        if (verbose) cout << "\nTesting 'lastDayOfMonth'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            ASSERT(28 ==       Util::lastDayOfMonth(1700, 2));
            ASSERT(29 ==  PosixUtil::lastDayOfMonth(1700, 2));
            ASSERT(28 == SerialUtil::lastDayOfMonth(1700, 2));
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            ASSERT(29 ==       Util::lastDayOfMonth(1700, 2));
            ASSERT(29 ==  PosixUtil::lastDayOfMonth(1700, 2));
            ASSERT(28 == SerialUtil::lastDayOfMonth(1700, 2));
        }

        if (verbose) cout << "\nTesting 'numLeapYears'." << endl;
        {
            // proleptic Gregorian mode

            Util::enableProlepticGregorianMode();
            ASSERT(true == Util::isProlepticGregorianMode());

            ASSERT(0 ==       Util::numLeapYears(1699, 1701));
            ASSERT(1 ==  PosixUtil::numLeapYears(1699, 1701));
            ASSERT(0 == SerialUtil::numLeapYears(1699, 1701));
        }
        {
            // POSIX mode

            Util::disableProlepticGregorianMode();
            ASSERT(false == Util::isProlepticGregorianMode());

            ASSERT(1 ==       Util::numLeapYears(1699, 1701));
            ASSERT(1 ==  PosixUtil::numLeapYears(1699, 1701));
            ASSERT(0 == SerialUtil::numLeapYears(1699, 1701));
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING '*ProlepticGregorianMode'
        //
        // Concerns:
        //: 1 The 'disableProlepticGregorianMode' function sets the calendar
        //:   mode to POSIX (i.e., sets the underlying mode flag to 'false')
        //:   regardless of its current setting.
        //:
        //: 2 The 'enableProlepticGregorianMode' function sets the calendar
        //:   mode to proleptic Gregorian (i.e., sets the underlying mode flag
        //:   to 'true') regardless of its current setting.
        //:
        //: 3 The value returned by the 'isProlepticGregorianMode' function
        //:   indicates the calendar mode currently in effect ('true' for
        //:   proleptic Gregorian and 'false' for POSIX).
        //
        // Plan:
        //: 1 Cycle through a sequence of calls to the "disable" and "enable"
        //:   functions ensuring that each function is called at least once
        //:   when the current calendar mode is proleptic Gregorian and at
        //:   least once when the mode is POSIX.  Follow each call with a call
        //:   to 'isProlepticGregorianMode' to verify that the expected
        //:   calendar mode is in effect.  (C-1..3)
        //
        // Testing:
        //   static void disableProlepticGregorianMode();
        //   static void enableProlepticGregorianMode();
        //   static bool isProlepticGregorianMode();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING '*ProlepticGregorianMode'" << endl
                          << "=================================" << endl;

        Util::disableProlepticGregorianMode();
        ASSERT(false == Util::isProlepticGregorianMode());

        Util::disableProlepticGregorianMode();
        ASSERT(false == Util::isProlepticGregorianMode());

        Util::enableProlepticGregorianMode();
        ASSERT(true  == Util::isProlepticGregorianMode());

        Util::enableProlepticGregorianMode();
        ASSERT(true  == Util::isProlepticGregorianMode());

        Util::disableProlepticGregorianMode();
        ASSERT(false == Util::isProlepticGregorianMode());

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
