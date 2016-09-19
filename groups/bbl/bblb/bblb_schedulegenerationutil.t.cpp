// bblb_schedulegenerationutil.t.cpp                                  -*-C++-*-
#include <bblb_schedulegenerationutil.h>

#include <bdlt_calendar.h>
#include <bdlt_calendarloader.h>
#include <bdlt_date.h>
#include <bdlt_dateutil.h>
#include <bdlt_dayofweek.h>
#include <bdlt_dayofweekset.h>
#include <bdlt_packedcalendar.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test consists of several static member functions that
// provide a way to generate an infinite series of dates and compute a subset
// of it.  The general plan is that the methods are tested against a set of
// tabulated test vectors, and negative tests for preconditions are conducted.
// The test vectors cover the permutations of interest of relations of the type
// 'earliest <= example <= latest' as well as edge cases for the resulting
// schedule.
// ----------------------------------------------------------------------------
// [ 2] generateFromDayInterval(s, e, l, example, interval);
// [ 3] generateFromDayOfMonth(s, e, l, eY, eM, i, tDOM, tDOF);
// [ 4] generateFromBusinessDayOfMonth(s, e, l, c, eY, eM, i, tBDOM);
// [ 5] generateFromDayOfWeekAfterDayOfMonth(s, e, l, d, eY, eM, i, DOM);
// [ 6] generateFromDayOfWeekInMonth(s, e, l, d, eY, eM, i, oW);
// ----------------------------------------------------------------------------
// [ 7] USAGE EXAMPLE
// [ 1] toString(output, date)
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
//             NON-STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define DAY(X) bdlt::DayOfWeek::e_##X       // Shorten qualified name

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bblb::ScheduleGenerationUtil Obj;

// ============================================================================
//                           TEST FUNCTIONS
// ----------------------------------------------------------------------------

static
void toString(bsl::ostringstream *output, const bsl::vector<bdlt::Date>& date)
    // Load, into the specified 'output' stream a comma-separated string
    // representation of the specified 'date' vector.
{
    output->str(std::string());
    for (bsl::size_t i = 0; i < date.size(); i++) {
        (*output) << bdlt::DateUtil::convertToYYYYMMDD(date[i]) << ",";
    }
    output->flush();
}

// ============================================================================
//                            TEST CLASSES
// ----------------------------------------------------------------------------

class TestCalendarLoader : public bdlt::CalendarLoader
    // Define a calendar loader that can be used to create a calendar with
    // valid range '[1/1/2000, 1/1/2020]' containing Saturday and Sunday as
    // weekend days and a fixed set of holidays in each year.  Note that
    // holidays are chosen arbitrarily, since exhaustive range of calendar
    // states is already tested in the 'CalendarUtil' component for
    // 'nthBusDayOfMonthOrMax' to which the routine
    // 'generateFromBusinessDayOfMonth' delegates.
{
  public:
    // CREATORS
    ~TestCalendarLoader()
        // Destroy this object.
    {
    }

    int load(bdlt::PackedCalendar *result, const char * /* calendarName */)
        // Load, into the specified 'result', the calendar corresponding to the
        // specified 'calendarName'.  Return 0 on success, and a non-zero value
        // otherwise.  If the return value is 1, the calendar was not found and
        // '*result' is unchanged.  If the return value is any other non-zero
        // value, then some other error occurred, and the state of the object
        // pointed to by 'result' is valid, but its value is undefined.
    {
        const int startYear = 2000;
        const int endYear = 2020;

        result->setValidRange(bdlt::Date(startYear, 1, 1),
                              bdlt::Date(endYear,   1, 1));

        static const int HOLIDAYS[][2] = {
            // Month Day
            // -----  ---
            {      1,  23 },
            {      2,   4 },
            {      2,  25 },
            {      2,  27 },
            {      2,  28 },
            {      5,   1 },
            {      6,  24 },
            {      8,  26 },
            {     10,  28 },
            {     11,  22 },
            {     11,  25 },
            {     12,  25 },
            {     12,  31 },
        };

        size_t NUM_HOLIDAYS = sizeof HOLIDAYS / sizeof *HOLIDAYS;

        result->addWeekendDay(DAY(SAT));
        result->addWeekendDay(DAY(SUN));

        for (int year = startYear; year <= endYear; year++) {
            for (size_t i = 0; i < NUM_HOLIDAYS; i++) {
                result->addHoliday(bdlt::Date(
                                        year, HOLIDAYS[i][0], HOLIDAYS[i][1]));
            }
        }

        return 0;
    }
};

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int         test = argc > 1 ? atoi(argv[1]) : 0;
    bool     verbose = argc > 2;
    bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Generating a Schedule
///- - - - - - - - - - - - - - - -
// Suppose that we want to determine the sequence of dates that are:
//   * integral multiples of 9 months away from July 2007,
//   * on the 23rd day of the month,
//   * and within the closed interval '[02/01/2012, 02/28/2015]'.
//
// First, we define the inputs and output to the schedule generation function:
//..
    bdlt::Date earliest(2012, 2,  1);
    bdlt::Date   latest(2015, 2, 28);
    bdlt::Date  example(2007, 7, 23);

    bsl::vector<bdlt::Date> schedule;
//..
// Now, we invoke the 'generateFromDayOfMonth' routine to obtain the subset of
// dates:
//..
    bblb::ScheduleGenerationUtil::generateFromDayOfMonth(
                                                    &schedule,
                                                    earliest,
                                                    latest,
                                                    example.year(),
                                                    example.month(),
                                                    9,    // 'intervalInMonths'
                                                    23);  // 'targetDayOfMonth'
//..
// Finally, we assert that the generated schedule is what we expect:
//..
    ASSERT(4 == schedule.size());
    ASSERT(bdlt::Date(2012, 10, 23) == schedule[0]);
    ASSERT(bdlt::Date(2013,  7, 23) == schedule[1]);
    ASSERT(bdlt::Date(2014,  4, 23) == schedule[2]);
    ASSERT(bdlt::Date(2015,  1, 23) == schedule[3]);
//..
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'generateFromDayOfWeekInMonth'
        //
        // Concerns:
        //: 1 The method produces the expected result values.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Test permutations of interest for the method's arguments.  (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for argument values.  (C-2)
        //
        // Testing:
        //   generateFromDayOfWeekInMonth(s, e, l, d, eY, eM, i, oW);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'generateFromDayOfWeekInMonth'" << endl
                          << "======================================" << endl;

        static const struct {
            int                    d_lineNum;
            int                    d_earliestYYYYMMDD;
            int                    d_latestYYYYMMDD;
            bdlt::DayOfWeek::Enum  d_dayOfWeek;
            int                    d_exampleYear;
            int                    d_exampleMonth;
            int                    d_intervalInMonths;
            int                    d_ocurrenceWeek;
            const char            *d_expectedOutputString_p;
        } INPUT[] = {
            //LN  earliest   latest   dayOfWk   exYr  eM  I   O   EXP
            //--  --------  --------  --------  ----  --  --  --  ----------
            { L_, 20140505, 20170203, DAY(SUN), 2001,  2,  9,  3,
                                      "20140817,20150517,20160221,20161120," },
            { L_, 20150123, 20190203, DAY(FRI), 2019,  2, 17,  4,
                                                        "20160422,20170922," },
            { L_, 20150123, 20190204, DAY(MON), 2019,  2, 12,  1,
                             "20150202,20160201,20170206,20180205,20190204," },
            { L_, 20150123, 20170203, DAY(FRI), 1968,  6,  4,  4,
                    "20150227,20150626,20151023,20160226,20160624,20161028," },
            { L_, 20010123, 20170203, DAY(FRI), 2052,  2, 27,  4,
           "20020823,20041126,20070223,20090522,20110826,20131122,20160226," },
            { L_, 20150123, 20150123, DAY(FRI), 2015,  1,  1,  4,
                                                                 "20150123," },
            { L_, 20150123, 20150123, DAY(SAT), 2015,  1,  1,  4, ""         },
            { L_, 99991231, 99991231, DAY(FRI), 9999, 12,  1,  1, ""         },
        };

        const size_t NUM_DATA = sizeof INPUT / sizeof *INPUT;

        bsl::vector<bdlt::Date> schedule;

        for (size_t di = 0; di < NUM_DATA; ++di)
        {
            const int  LINE     = INPUT[di].d_lineNum;
            bdlt::Date earliest = bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                 INPUT[di].d_earliestYYYYMMDD);
            bdlt::Date latest   = bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                   INPUT[di].d_latestYYYYMMDD);

            const bdlt::DayOfWeek::Enum dayOfWeek = INPUT[di].d_dayOfWeek;

            const int   exampleYear    = INPUT[di].d_exampleYear;
            const int   exampleMonth   = INPUT[di].d_exampleMonth;
            const int   interval       = INPUT[di].d_intervalInMonths;
            const int   ocurrenceWeek  = INPUT[di].d_ocurrenceWeek;
            const char* expectedOutput = INPUT[di].d_expectedOutputString_p;

            Obj::generateFromDayOfWeekInMonth(&schedule,
                                              earliest,
                                              latest,
                                              exampleYear,
                                              exampleMonth,
                                              interval,
                                              dayOfWeek,
                                              ocurrenceWeek);

            bsl::ostringstream output;
            toString(&output, schedule);

            if (veryVerbose) { P_(LINE) P_(output.str()) P(expectedOutput); }

            LOOP3_ASSERT(di,
                         output.str(),
                         expectedOutput,
                         output.str() == expectedOutput);
        }

        // negative tests

        if (verbose) cout << "\nNegative Testing." << endl;
        {

            bsls::AssertFailureHandlerGuard g(
                                             bsls::AssertTest::failTestDriver);

            bdlt::Date d(2015,1,23);

            // 'earliest <= latest'

            ASSERT_FAIL(Obj::generateFromDayOfWeekInMonth(
                                  &schedule, d, d-1, 2010, 2, 1, DAY(FRI), 1));
            ASSERT_PASS(Obj::generateFromDayOfWeekInMonth(
                                    &schedule, d, d, 2010, 2, 1, DAY(FRI), 1));

            // 'exampleYear'

            ASSERT_FAIL(Obj::generateFromDayOfWeekInMonth(
                                      &schedule, d, d,  0, 2, 1, DAY(FRI), 1));
            ASSERT_PASS(Obj::generateFromDayOfWeekInMonth(
                                      &schedule, d, d,  1, 2, 1, DAY(FRI), 1));
            ASSERT_PASS(Obj::generateFromDayOfWeekInMonth(
                                    &schedule, d, d, 9999, 2, 1, DAY(FRI), 1));
            ASSERT_FAIL(Obj::generateFromDayOfWeekInMonth(
                                   &schedule, d, d, 10000, 2, 1, DAY(FRI), 1));

            // 'exampleMonth'

            ASSERT_FAIL(Obj::generateFromDayOfWeekInMonth(
                                   &schedule, d, d, 2010,  0, 1, DAY(FRI), 1));
            ASSERT_PASS(Obj::generateFromDayOfWeekInMonth(
                                   &schedule, d, d, 2010,  1, 1, DAY(FRI), 1));
            ASSERT_PASS(Obj::generateFromDayOfWeekInMonth(
                                   &schedule, d, d, 2010, 12, 1, DAY(FRI), 1));
            ASSERT_FAIL(Obj::generateFromDayOfWeekInMonth(
                                   &schedule, d, d, 2010, 13, 1, DAY(FRI), 1));

            // 'interval'

            ASSERT_FAIL(Obj::generateFromDayOfWeekInMonth(
                                    &schedule, d, d, 2010, 2, 0, DAY(FRI), 1));
            ASSERT_PASS(Obj::generateFromDayOfWeekInMonth(
                                    &schedule, d, d, 2010, 2, 1, DAY(FRI), 1));

            // 'ocurrenceWeek'

            ASSERT_FAIL(Obj::generateFromDayOfWeekInMonth(
                                    &schedule, d, d, 2010, 2, 1, DAY(FRI), 0));
            ASSERT_PASS(Obj::generateFromDayOfWeekInMonth(
                                    &schedule, d, d, 2010, 2, 1, DAY(FRI), 1));
            ASSERT_PASS(Obj::generateFromDayOfWeekInMonth(
                                    &schedule, d, d, 2010, 2, 1, DAY(FRI), 4));
            ASSERT_FAIL(Obj::generateFromDayOfWeekInMonth(
                                    &schedule, d, d, 2010, 2, 1, DAY(FRI), 5));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'generateFromDayOfWeekAfterDayOfMonth'
        //
        // Concerns:
        //: 1 The method produces the expected result values.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Test permutations of interest for the method's arguments.  (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for argument values.  (C-2)
        //
        // Testing:
        //   generateFromDayOfWeekAfterDayOfMonth(s, e, l, d, eY, eM, i, DOM);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'generateFromDayOfWeekAfterDayOfMonth'" << endl
                 << "==============================================" << endl;
        }

        static const struct {
            int                    d_lineNum;
            int                    d_earliestYYYYMMDD;
            int                    d_latestYYYYMMDD;
            bdlt::DayOfWeek::Enum  d_dayOfWeek;
            int                    d_exampleYear;
            int                    d_exampleMonth;
            int                    d_intervalInMonths;
            int                    d_dayOfMonth;
            const char            *d_expectedOutputString_p;
        } INPUT[] = {
            //LN  earliest   latest   dayOfWk   exYr  eM  I   DM  EXP
            //--  --------  --------  --------  ----  --  --  --  ----------
            { L_, 20140505, 20170203, DAY(WED), 2001,  2,  9,  9,
                                      "20140813,20150513,20160210,20161109," },
            { L_, 20150123, 20190203, DAY(FRI), 2019,  2, 17,  1,
                                               "20160401,20170901,20190201," },
            { L_, 20150123, 20190203, DAY(FRI), 2019,  2, 17,  2,
                                                        "20160408,20170908," },
            { L_, 20150123, 20190204, DAY(MON), 2019,  2, 12, 31, ""         },
            { L_, 20150123, 20190204, DAY(MON), 2019,  2, 12, 28,
                             "20150302,20160229,20170306,20180305,20190304," },
            { L_, 20010123, 20170203, DAY(FRI), 2052,  2, 27,  4,
           "20020809,20041105,20070209,20090508,20110805,20131108,20160205," },
            { L_, 20150123, 20150123, DAY(FRI), 2015,  1,  1, 20,
                                                                 "20150123," },
            { L_, 20150123, 20150123, DAY(SAT), 2015,  1,  1, 20, ""         },
            { L_, 20150123, 20160123, DAY(SAT), 2015,  1,  1, 31, ""         },
            { L_, 99991231, 99991231, DAY(FRI), 9999, 12,  1,  1, ""         },
        };

        const size_t NUM_DATA = sizeof INPUT / sizeof *INPUT;

        bsl::vector<bdlt::Date> schedule;

        for (size_t di = 0; di < NUM_DATA; ++di)
        {
            const int  LINE     = INPUT[di].d_lineNum;
            bdlt::Date earliest = bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                 INPUT[di].d_earliestYYYYMMDD);
            bdlt::Date latest   = bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                   INPUT[di].d_latestYYYYMMDD);

            const bdlt::DayOfWeek::Enum dayOfWeek = INPUT[di].d_dayOfWeek;

            const int   exampleYear    = INPUT[di].d_exampleYear;
            const int   exampleMonth   = INPUT[di].d_exampleMonth;
            const int   interval       = INPUT[di].d_intervalInMonths;
            const int   dayOfMonth     = INPUT[di].d_dayOfMonth;
            const char* expectedOutput = INPUT[di].d_expectedOutputString_p;

            Obj::generateFromDayOfWeekAfterDayOfMonth(&schedule,
                                                      earliest,
                                                      latest,
                                                      exampleYear,
                                                      exampleMonth,
                                                      interval,
                                                      dayOfWeek,
                                                      dayOfMonth);

            bsl::ostringstream output;
            toString(&output, schedule);

            if (veryVerbose) { P_(LINE) P_(output.str()) P(expectedOutput); }

            LOOP3_ASSERT(di,
                         output.str(),
                         expectedOutput,
                         output.str() == expectedOutput);
        }

        // negative tests

        if (verbose) cout << "\nNegative Testing." << endl;
        {

            bsls::AssertFailureHandlerGuard g(
                                             bsls::AssertTest::failTestDriver);

            bdlt::Date d(2015,1,23);

            // 'earliest <= latest'

            ASSERT_FAIL(Obj::generateFromDayOfWeekAfterDayOfMonth(
                                  &schedule, d, d-1, 2010, 2, 1, DAY(FRI), 1));
            ASSERT_PASS(Obj::generateFromDayOfWeekAfterDayOfMonth(
                                    &schedule, d, d, 2010, 2, 1, DAY(FRI), 1));

            // 'exampleYear'

            ASSERT_FAIL(Obj::generateFromDayOfWeekAfterDayOfMonth(
                                       &schedule, d, d, 0, 2, 1, DAY(FRI), 1));
            ASSERT_PASS(Obj::generateFromDayOfWeekAfterDayOfMonth(
                                      &schedule, d, d,  1, 2, 1, DAY(FRI), 1));
            ASSERT_PASS(Obj::generateFromDayOfWeekAfterDayOfMonth(
                                    &schedule, d, d, 9999, 2, 1, DAY(FRI), 1));
            ASSERT_FAIL(Obj::generateFromDayOfWeekAfterDayOfMonth(
                                   &schedule, d, d, 10000, 2, 1, DAY(FRI), 1));

            // 'exampleMonth'

            ASSERT_FAIL(Obj::generateFromDayOfWeekAfterDayOfMonth(
                                    &schedule, d, d, 2010, 0, 1, DAY(FRI), 1));
            ASSERT_PASS(Obj::generateFromDayOfWeekAfterDayOfMonth(
                                    &schedule, d, d, 2010, 1, 1, DAY(FRI), 1));
            ASSERT_PASS(Obj::generateFromDayOfWeekAfterDayOfMonth(
                                   &schedule, d, d, 2010, 12, 1, DAY(FRI), 1));
            ASSERT_FAIL(Obj::generateFromDayOfWeekAfterDayOfMonth(
                                   &schedule, d, d, 2010, 13, 1, DAY(FRI), 1));

            // 'interval'

            ASSERT_FAIL(Obj::generateFromDayOfWeekAfterDayOfMonth(
                                    &schedule, d, d, 2010, 2, 0, DAY(FRI), 1));
            ASSERT_PASS(Obj::generateFromDayOfWeekAfterDayOfMonth(
                                    &schedule, d, d, 2010, 2, 1, DAY(FRI), 1));

            // 'dayOfMonth'

            ASSERT_FAIL(Obj::generateFromDayOfWeekAfterDayOfMonth(
                                    &schedule, d, d, 2010, 2, 1, DAY(FRI), 0));
            ASSERT_PASS(Obj::generateFromDayOfWeekAfterDayOfMonth(
                                    &schedule, d, d, 2010, 2, 1, DAY(FRI), 1));
            ASSERT_PASS(Obj::generateFromDayOfWeekAfterDayOfMonth(
                                   &schedule, d, d, 2010, 2, 1, DAY(FRI), 31));
            ASSERT_FAIL(Obj::generateFromDayOfWeekAfterDayOfMonth(
                                   &schedule, d, d, 2010, 2, 1, DAY(FRI), 32));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'generateFromBusinessDayOfMonth'
        //
        // Concerns:
        //: 1 The method produces the expected result values.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Test permutations of interest for the method's arguments.  (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for argument values.  (C-2)
        //
        // Testing:
        //   generateFromBusinessDayOfMonth(s, e, l, c, eY, eM, i, tBDOM);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'generateFromBusinessDayOfMonth'" << endl
                 << "========================================" << endl;
        }

        bdlt::PackedCalendar noHolidays(bdlt::Date(2000, 1, 1),
                                       bdlt::Date(2020, 1, 1));
        bdlt::PackedCalendar noHolidaysLarge(bdlt::Date(   1,  1,  1),
                                            bdlt::Date(9999, 12, 31));
        bdlt::PackedCalendar weekendsAndHolidays;
        bdlt::PackedCalendar noBusinessDays(bdlt::Date(2000, 1, 1),
                                           bdlt::Date(2020, 1, 1));
        bdlt::PackedCalendar noLateBusinessDays(bdlt::Date(2000, 1, 1),
                                               bdlt::Date(2020, 1, 1));

        TestCalendarLoader loader;
        loader.load(&weekendsAndHolidays, "");

        noBusinessDays.addWeekendDay(bdlt::DayOfWeek::e_SUN);
        noBusinessDays.addWeekendDay(bdlt::DayOfWeek::e_MON);
        noBusinessDays.addWeekendDay(bdlt::DayOfWeek::e_TUE);
        noBusinessDays.addWeekendDay(bdlt::DayOfWeek::e_WED);
        noBusinessDays.addWeekendDay(bdlt::DayOfWeek::e_THU);
        noBusinessDays.addWeekendDay(bdlt::DayOfWeek::e_FRI);
        noBusinessDays.addWeekendDay(bdlt::DayOfWeek::e_SAT);

        {
            bdlt::DayOfWeekSet dows;

            dows.add(bdlt::DayOfWeek::e_SUN);
            dows.add(bdlt::DayOfWeek::e_MON);
            noLateBusinessDays.addWeekendDaysTransition(
                                                 bdlt::Date(2016, 6, 1), dows);

            dows.add(bdlt::DayOfWeek::e_TUE);
            dows.add(bdlt::DayOfWeek::e_WED);
            dows.add(bdlt::DayOfWeek::e_THU);
            dows.add(bdlt::DayOfWeek::e_FRI);
            dows.add(bdlt::DayOfWeek::e_SAT);
            noLateBusinessDays.addWeekendDaysTransition(
                                                 bdlt::Date(2016, 1, 1), dows);
            noLateBusinessDays.addWeekendDaysTransition(
                                                 bdlt::Date(2017, 1, 1), dows);
        }

        bdlt::Calendar cal1(noHolidays);
        bdlt::Calendar cal2(weekendsAndHolidays);
        bdlt::Calendar cal3(noBusinessDays);
        bdlt::Calendar cal4(noLateBusinessDays);
        bdlt::Calendar cal5(noHolidaysLarge);

        static const struct {
            int                   d_lineNum;
            int                   d_earliestYYYYMMDD;
            int                   d_latestYYYYMMDD;
            const bdlt::Calendar *d_calendar_p;
            int                   d_exampleYear;
            int                   d_exampleMonth;
            int                   d_intervalInMonths;
            int                   d_targetBusinessDayOfMonth;
            const char           *d_expectedOutputString_p;
        } INPUT[] = {
            //LN  earliest   latest    cal   exYr  eM  I   tBDOM  EXP
            //--  --------  --------  -----  ----  --  --  -----  ----------
            { L_, 20140505, 20170203, &cal1, 2001,  2,  9,    -4,
                                      "20140828,20150528,20160226,20161127," },
            { L_, 20150123, 20190203, &cal1, 2019,  2, 17,     4,
                                                        "20160404,20170904," },
            { L_, 20150123, 20190204, &cal1, 2019,  2, 12,    25,
                                      "20150225,20160225,20170225,20180225," },
            { L_, 20150123, 20170203, &cal1, 1968,  6,  4,    -1,
                    "20150228,20150630,20151031,20160229,20160630,20161031," },
            { L_, 20010123, 20170203, &cal1, 2052,  2, 27,     1,
           "20020801,20041101,20070201,20090501,20110801,20131101,20160201," },
            { L_, 20150123, 20150223, &cal1, 2015,  1,  1,     4,
                                                                 "20150204," },
            { L_, 20150123, 20150123, &cal1, 2015,  1,  1,     4,""          },
            { L_, 20140505, 20170203, &cal2, 2001,  2,  9,    -4,
                                      "20140825,20150526,20160223,20161124," },
            { L_, 20150123, 20190203, &cal2, 2019,  2, 17,     4,
                                                        "20160406,20170906," },
            { L_, 20150123, 20190204, &cal2, 2019,  2, 12,    25,
                                      "20150226,20160229,20170224,20180226," },
            { L_, 20150123, 20170203, &cal2, 1968,  6,  4,    -1,
                    "20150226,20150630,20151030,20160229,20160630,20161031," },
            { L_, 20010123, 20170203, &cal2, 2052,  2, 27,     1,
           "20020801,20041101,20070201,20090504,20110801,20131101,20160201," },
            { L_, 20150123, 20150223, &cal2, 2015,  1,  1,     4,
                                                                 "20150206," },
            { L_, 20150123, 20150123, &cal2, 2015,  1,  1,     4, ""         },
            { L_, 20150123, 20190204, &cal3, 2019,  2, 12,    25, ""         },
            { L_, 20150123, 20190204, &cal4, 2019,  2, 12,    25, ""         },
            { L_, 20150123, 20160604, &cal4, 2019,  2,  1,    25, ""         },
            { L_, 99991231, 99991231, &cal5, 9999, 12,  1,     1, ""         },
        };

        const size_t NUM_DATA = sizeof INPUT / sizeof *INPUT;

        bsl::vector<bdlt::Date> schedule;

        for (size_t di = 0; di < NUM_DATA; ++di) {
            const int  LINE     = INPUT[di].d_lineNum;
            bdlt::Date earliest = bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                 INPUT[di].d_earliestYYYYMMDD);
            bdlt::Date latest  = bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                   INPUT[di].d_latestYYYYMMDD);

            const bdlt::Calendar *calendar = INPUT[di].d_calendar_p;

            const int   exampleYear    = INPUT[di].d_exampleYear;
            const int   exampleMonth   = INPUT[di].d_exampleMonth;
            const int   interval       = INPUT[di].d_intervalInMonths;
            const int   targetBusDay   = INPUT[di].d_targetBusinessDayOfMonth;
            const char *expectedOutput = INPUT[di].d_expectedOutputString_p;

            Obj::generateFromBusinessDayOfMonth(&schedule,
                                                earliest,
                                                latest,
                                                exampleYear,
                                                exampleMonth,
                                                interval,
                                                *calendar,
                                                targetBusDay);

            bsl::ostringstream output;
            toString(&output, schedule);

            if (veryVerbose) { P_(LINE) P_(output.str()) P(expectedOutput); }

            LOOP3_ASSERT(LINE,
                         output.str(),
                         expectedOutput,
                         output.str() == expectedOutput);
        }

        // negative tests

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard g(
                                             bsls::AssertTest::failTestDriver);

            bdlt::Date d(2015, 1, 23);

            // 'earliest <= latest'

            ASSERT_FAIL(Obj::generateFromBusinessDayOfMonth(
                                    &schedule, d, d - 1, 2010, 2, 1, cal1, 1));
            ASSERT_PASS(Obj::generateFromBusinessDayOfMonth(
                                        &schedule, d, d, 2010, 2, 1, cal1, 1));

            // 'exampleYear'

            ASSERT_FAIL(Obj::generateFromBusinessDayOfMonth(
                                           &schedule, d, d, 0, 2, 1, cal1, 1));
            ASSERT_PASS(Obj::generateFromBusinessDayOfMonth(
                                           &schedule, d, d, 1, 2, 1, cal1, 1));
            ASSERT_PASS(Obj::generateFromBusinessDayOfMonth(
                                        &schedule, d, d, 9999, 2, 1, cal1, 1));
            ASSERT_FAIL(Obj::generateFromBusinessDayOfMonth(
                                       &schedule, d, d, 10000, 2, 1, cal1, 1));

            // 'exampleMonth'

            ASSERT_FAIL(Obj::generateFromBusinessDayOfMonth(
                                        &schedule, d, d, 2010, 0, 1, cal1, 1));
            ASSERT_PASS(Obj::generateFromBusinessDayOfMonth(
                                        &schedule, d, d, 2010, 1, 1, cal1, 1));
            ASSERT_PASS(Obj::generateFromBusinessDayOfMonth(
                                       &schedule, d, d, 2010, 12, 1, cal1, 1));
            ASSERT_FAIL(Obj::generateFromBusinessDayOfMonth(
                                       &schedule, d, d, 2010, 13, 1, cal1, 1));

            // 'interval'

            ASSERT_FAIL(Obj::generateFromBusinessDayOfMonth(
                                        &schedule, d, d, 2010, 2, 0, cal1, 1));
            ASSERT_PASS(Obj::generateFromBusinessDayOfMonth(
                                        &schedule, d, d, 2010, 2, 1, cal1, 1));

            // 'targetBusinessDayOfMonth'

            ASSERT_PASS(Obj::generateFromBusinessDayOfMonth(
                                       &schedule, d, d, 2010, 2, 1, cal1, -1));
            ASSERT_FAIL(Obj::generateFromBusinessDayOfMonth(
                                        &schedule, d, d, 2010, 2, 1, cal1, 0));
            ASSERT_PASS(Obj::generateFromBusinessDayOfMonth(
                                        &schedule, d, d, 2010, 2, 1, cal1, 1));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'generateFromDayOfMonth'
        //
        // Concerns:
        //: 1 The method produces the expected result values.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Test permutations of interest for the method's arguments.  (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for argument values.  (C-2)
        //
        // Testing:
        //   generateFromDayOfMonth(s, e, l, eY, eM, i, tDOM, tDOF);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'generateFromDayOfMonth'" << endl
                          << "================================" << endl;

        static const struct {
            int         d_lineNum;
            int         d_earliestYYYYMMDD;
            int         d_latestYYYYMMDD;
            int         d_exampleYear;
            int         d_exampleMonth;
            int         d_intervalInMonths;
            int         d_targetDayOfMonth;
            int         d_targetDayOfFeb;
            const char *d_expectedOutputString_p;
        } INPUT[] = {
            //LN  earliest   latest   exYr  eM  I   tDOM  tDOF  EXP
            //--  --------  --------  ----  --  --  ----  ----  ------------
            { L_, 20140505, 20170203, 2001,  2,  9,   31,    0,
                                      "20140831,20150531,20160229,20161130," },
            { L_, 20150123, 20190203, 2019,  2, 17,    1,    0,
                                               "20160401,20170901,20190201," },
            { L_, 20150123, 20190204, 2019,  2, 12,   25,    0,
                                      "20150225,20160225,20170225,20180225," },
            { L_, 20150123, 20170203, 1968,  6,  4,   31,    0,
                    "20150228,20150630,20151031,20160229,20160630,20161031," },
            { L_, 20010123, 20170203, 2052,  2, 27,   29,    0,
           "20020829,20041129,20070228,20090529,20110829,20131129,20160229," },
            { L_, 20150123, 20150123, 2015,  1,  1,   23,    0,
                                                                 "20150123," },
            { L_, 20150123, 20150123, 2015,  1,  1,   15,    0, ""           },
            { L_, 20140505, 20170203, 2001,  2,  9,   31,   28,
                                      "20140831,20150531,20160228,20161130," },
            { L_, 20150123, 20190203, 2019,  2, 17,    1,   29,
                                                        "20160401,20170901," },
            { L_, 20150123, 20190204, 2019,  2, 12,   25,   29,
                                      "20150228,20160229,20170228,20180228," },
            { L_, 20150123, 20170203, 1968,  6,  4,   31,   28,
                    "20150228,20150630,20151031,20160228,20160630,20161031," },
            { L_, 20010123, 20170203, 2052,  2, 27,   29,   28,
           "20020829,20041129,20070228,20090529,20110829,20131129,20160228," },
            { L_, 20150123, 20150123, 2015,  1,  1,   23,   28,
                                                                 "20150123," },
            { L_, 20150123, 20150123, 2015,  1,  1,   15,   28, ""           },
            { L_, 99991231, 99991231, 9999, 12,  1,   15,   28, ""           },
        };

        const size_t NUM_DATA = sizeof INPUT / sizeof *INPUT;

        bsl::vector<bdlt::Date> schedule;

        for (size_t di = 0; di < NUM_DATA; ++di)
        {
            const int  LINE     = INPUT[di].d_lineNum;
            bdlt::Date earliest = bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                 INPUT[di].d_earliestYYYYMMDD);
            bdlt::Date latest   = bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                   INPUT[di].d_latestYYYYMMDD);

            const int   exampleYear      = INPUT[di].d_exampleYear;
            const int   exampleMonth     = INPUT[di].d_exampleMonth;
            const int   interval         = INPUT[di].d_intervalInMonths;
            const int   targetDayOfMonth = INPUT[di].d_targetDayOfMonth;
            const int   targetDayOfFeb   = INPUT[di].d_targetDayOfFeb;
            const char *expectedOutput   = INPUT[di].d_expectedOutputString_p;

            Obj::generateFromDayOfMonth(&schedule,
                                        earliest,
                                        latest,
                                        exampleYear,
                                        exampleMonth,
                                        interval,
                                        targetDayOfMonth,
                                        targetDayOfFeb);

            bsl::ostringstream output;
            toString(&output, schedule);

            if (veryVerbose) { P_(LINE) P_(output.str()) P(expectedOutput); }

            LOOP3_ASSERT(di,
                         output.str(),
                         expectedOutput,
                         output.str() == expectedOutput);
        }

        // negative tests

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard g(
                                             bsls::AssertTest::failTestDriver);

            bdlt::Date d(2015, 1, 23);

            // 'earliest <= latest'

            ASSERT_FAIL(Obj::generateFromDayOfMonth(
                                       &schedule, d, d - 1, 2010, 2, 1, 1, 1));
            ASSERT_PASS(Obj::generateFromDayOfMonth(
                                           &schedule, d, d, 2010, 2, 1, 1, 1));

            // 'exampleYear'

            ASSERT_FAIL(Obj::generateFromDayOfMonth(
                                              &schedule, d, d, 0, 2, 1, 1, 1));
            ASSERT_PASS(Obj::generateFromDayOfMonth(
                                              &schedule, d, d, 1, 2, 1, 1, 1));
            ASSERT_PASS(Obj::generateFromDayOfMonth(
                                           &schedule, d, d, 9999, 2, 1, 1, 1));
            ASSERT_FAIL(Obj::generateFromDayOfMonth(
                                          &schedule, d, d, 10000, 2, 1, 1, 1));

            // 'exampleMonth'

            ASSERT_FAIL(Obj::generateFromDayOfMonth(
                                           &schedule, d, d, 2010, 0, 1, 1, 1));
            ASSERT_PASS(Obj::generateFromDayOfMonth(
                                           &schedule, d, d, 2010, 1, 1, 1, 1));
            ASSERT_PASS(Obj::generateFromDayOfMonth(
                                          &schedule, d, d, 2010, 12, 1, 1, 1));
            ASSERT_FAIL(Obj::generateFromDayOfMonth(
                                          &schedule, d, d, 2010, 13, 1, 1, 1));

            // 'interval'

            ASSERT_FAIL(Obj::generateFromDayOfMonth(
                                           &schedule, d, d, 2010, 2, 0, 1, 1));
            ASSERT_PASS(Obj::generateFromDayOfMonth(
                                           &schedule, d, d, 2010, 2, 1, 1, 1));

            // 'targetDayOfMonth'

            ASSERT_FAIL(Obj::generateFromDayOfMonth(
                                           &schedule, d, d, 2010, 2, 1, 0, 1));
            ASSERT_PASS(Obj::generateFromDayOfMonth(
                                           &schedule, d, d, 2010, 2, 1, 1, 1));
            ASSERT_PASS(Obj::generateFromDayOfMonth(
                                          &schedule, d, d, 2010, 2, 1, 31, 1));
            ASSERT_FAIL(Obj::generateFromDayOfMonth(
                                          &schedule, d, d, 2010, 2, 1, 32, 1));

            // 'targetDayOfFeb'

            ASSERT_FAIL(Obj::generateFromDayOfMonth(
                                          &schedule, d, d, 2010, 2, 1, 3, -1));
            ASSERT_PASS(Obj::generateFromDayOfMonth(
                                           &schedule, d, d, 2010, 2, 1, 3, 0));
            ASSERT_PASS(Obj::generateFromDayOfMonth(
                                           &schedule, d, d, 2010, 2, 1, 3, 1));
            ASSERT_PASS(Obj::generateFromDayOfMonth(
                                          &schedule, d, d, 2010, 2, 1, 3, 29));
            ASSERT_FAIL(Obj::generateFromDayOfMonth(
                                          &schedule, d, d, 2010, 2, 1, 3, 30));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'generateFromDayInterval'
        //
        // Concerns:
        //: 1 The method produces the expected result values.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Test permutations of interest for the method's arguments.  (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for argument values.  (C-2)
        //
        // Testing:
        //   generateFromDayInterval(s, e, l, example, interval);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'generateFromDayInterval'" << endl
                          << "=================================" << endl;

        static const struct {
            int         d_lineNum;
            int         d_earliestYYYYMMDD;
            int         d_latestYYYYMMDD;
            int         d_exampleYYYYMMDD;
            int         d_interval;
            const char *d_expectedOutputString_p;
        } INPUT[] = {
            //LN  earliest   latest   example    I   EXP
            //--  --------  --------  --------  ---  -----------------------
            { L_, 20140505, 20140512, 20010201,   1,
  "20140505,20140506,20140507,20140508,20140509,20140510,20140511,20140512," },
            { L_, 20150123, 20150203, 20190205,   7, "20150127,20150203,"    },
            { L_, 20150123, 20150304, 20190223,  14,
                                               "20150131,20150214,20150228," },
            { L_, 20150123, 20150703, 19680607,  30,
                    "20150201,20150303,20150402,20150502,20150601,20150701," },
            { L_, 20010123, 20070203, 20520201, 365,
                    "20010213,20020213,20030213,20040213,20050212,20060212," },
            { L_, 20150123, 20150125, 20150122,   3, "20150125,"             },
            { L_, 20150123, 20150123, 20150124,   2, ""                      },
        };

        const size_t NUM_DATA = sizeof INPUT / sizeof *INPUT;

        bsl::vector<bdlt::Date> schedule;

        for (size_t di = 0; di < NUM_DATA; ++di) {
            const int  LINE     = INPUT[di].d_lineNum;
            bdlt::Date earliest = bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                 INPUT[di].d_earliestYYYYMMDD);
            bdlt::Date latest   = bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                   INPUT[di].d_latestYYYYMMDD);
            bdlt::Date example = bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                  INPUT[di].d_exampleYYYYMMDD);

            const int   interval       = INPUT[di].d_interval;
            const char* expectedOutput = INPUT[di].d_expectedOutputString_p;

            Obj::generateFromDayInterval(
                               &schedule, earliest, latest, example, interval);

            bsl::ostringstream output;
            toString(&output, schedule);

            if (veryVerbose) { P_(LINE) P_(output.str()) P(expectedOutput); }

            LOOP3_ASSERT(di,
                         output.str(),
                         expectedOutput,
                         output.str() == expectedOutput);
        }

        // negative tests

        if (veryVerbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard g(
                                             bsls::AssertTest::failTestDriver);

            bdlt::Date d(2015, 1, 23);

            // 'earliest <= latest'

            ASSERT_FAIL(Obj::generateFromDayInterval(
                                                   &schedule, d, d - 1, d, 2));
            ASSERT_PASS(Obj::generateFromDayInterval(&schedule, d, d, d, 2));

            // 'interval'

            ASSERT_FAIL(Obj::generateFromDayInterval(&schedule, d, d, d, 0));
            ASSERT_PASS(Obj::generateFromDayInterval(&schedule, d, d, d, 1));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'toString'
        //
        // Concerns:
        //: 1 The method produces the expected result values.
        //
        // Plan:
        //: 1 Test permutations of interest for the method's arguments.  (C-1)
        //
        // Testing:
        //   toString(output, date)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'toString'" << endl
                          << "==================" << endl;

        bsl::vector<bdlt::Date> date;
        bsl::ostringstream      output;

        toString(&output, date);
        ASSERTV(output.str(), output.str() == "");

        date.push_back(bdlt::Date(2014,  7,  3));

        toString(&output, date);
        ASSERTV(output.str(), output.str() == "20140703,");

        date.push_back(bdlt::Date(2012,  9, 12));

        toString(&output, date);
        ASSERTV(output.str(), output.str() == "20140703,20120912,");

        date.push_back(bdlt::Date(2015,  3,  6));

        toString(&output, date);
        ASSERTV(output.str(), output.str() == "20140703,20120912,20150306,");

        date.clear();

        toString(&output, date);
        ASSERTV(output.str(), output.str() == "");
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
// Copyright 2016 Bloomberg Finance L.P.
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
