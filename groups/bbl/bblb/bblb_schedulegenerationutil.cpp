// bblb_schedulegenerationutil.cpp                                    -*-C++-*-
#include <bblb_schedulegenerationutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bblb_schedulegenerationutil_cpp,"$Id$ $CSID$")

#include <bdlt_calendar.h>
#include <bdlt_calendarutil.h>
#include <bdlt_date.h>
#include <bdlt_dateutil.h>
#include <bdlt_serialdateimputil.h>

#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>

namespace BloombergLP {
namespace bblb {

// For 'bdlt::Date', the valid range of years is [1, 9999] and months is
// [1, 12].

static const int k_MAX_SERIAL_MONTH = (9999*12 + 12) - 1;
static const int k_MIN_SERIAL_MONTH = (1*12 + 1) - 1;

// Helper macros to convert to and from serial month.

#define YM2SERIAL(Y,M) 12*(Y) + (M) - 1
#define SERIAL2Y(S) (S)/12
#define SERIAL2M(S) (S)%12 + 1

static
inline void computeSerialMonthAndDay(int               *serialMonth,
                                     int               *day,
                                     const bdlt::Date&  date)
    // Load in the specified 'serialMonth' the serial month value computed from
    // the specified 'date' and load into 'day' the value of 'date.day()'.
{
    int year;
    int month;
    date.getYearMonthDay(&year, &month, day);
    *serialMonth = YM2SERIAL(year, month);
}

static
inline bdlt::Date getDayOfMonth(int year, int month, int day, int dayOfFeb)
    // If the specified 'month' represents February and the specified
    // 'dayOfFeb' is non-zero, return the smaller of 'dayOfFeb' and the last
    // day of the 'month' in the specified 'year'.  Otherwise, return the
    // day-of-month that is the smaller of the specified 'day' and the last day
    // of the 'month in 'year'.  The behavior is undefined unless
    // '1 <= year <= 9999', '1 <= month <= 12', and the resulting date is a
    // valid 'bdlt::Date'.
{
    BSLS_ASSERT(1 <= year  && 9999 >= year);
    BSLS_ASSERT(1 <= month &&   12 >= month);

    int last = bdlt::SerialDateImpUtil::lastDayOfMonth(year, month);

    int resultDay = bsl::min(last,
                             (dayOfFeb == 0 || month != 2) ? day : dayOfFeb);

    BSLS_ASSERT(bdlt::Date::isValidYearMonthDay(year, month, resultDay));

    return bdlt::Date(year, month, resultDay);
}

static
inline int rationalFloor(int numerator, int denominator)
    // Return the largest integer smaller than the rational number represented
    // by the ratio of the specified 'numerator' and the specified
    // 'denominator', without the use of floating-point calculations.  The
    // behavior is undefined unless 'denominator > 0'.
{
    BSLS_ASSERT(denominator > 0);

    return ((numerator < 0 && numerator % denominator != 0)
            ? numerator/denominator - 1
            : numerator/denominator);
}

static
inline int rationalCeiling(int numerator, int denominator)
    // Return the smallest integer larger than the rational number represented
    // by the ratio of the specified 'numerator' and the specified
    // 'denominator', without the use of floating-point calculations.  The
    // behavior is undefined unless 'denominator > 0'.
{
    BSLS_ASSERT(denominator > 0);

    return ((numerator > 0 && numerator % denominator != 0)
            ? numerator/denominator + 1
            : numerator/denominator);
}

static
int computeMonthRange(int *startSerialMonth,
                      int *endSerialMonth,
                      int  earliestSerialMonth,
                      int  latestSerialMonth,
                      int  exampleSerialMonth,
                      int  intervalInMonths)
    // Load, into the specified 'startSerialMonth' and the specified
    // 'endSerialMonth', the bounds of the range of serial months that are at
    // integral multiples of the specified 'intervalInMonths' from the
    // specified 'exampleSerialMonth' and are bounded by the specified
    // 'earliestSerialMonth' and the specified 'latestSerialMonth'.  Return 0
    // on success, and a non-zero value without modifying 'startSerialMonth' or
    // 'endSerialMonth' if these values would exceed the valid range for a
    // serial month.  The behavior is undefined unless
    // 'k_MIN_SERIAL_MONTH <= exampleSerialMonth <= k_MAX_SERIAL_MONTH', and
    // 'earliestSerialMonth <= latestSerialMonth'.
{
    BSLS_ASSERT(   k_MAX_SERIAL_MONTH >= exampleSerialMonth
                && k_MIN_SERIAL_MONTH <= exampleSerialMonth);
    BSLS_ASSERT(earliestSerialMonth <= latestSerialMonth);

    // Return codes.

    enum { e_VALID_RANGE = 0, e_OUT_OF_RANGE = 1 };

    // Compute how many periods away the start and end are.

    int startCount = rationalCeiling(earliestSerialMonth - exampleSerialMonth,
                                     intervalInMonths);

    int endCount   = rationalFloor(latestSerialMonth - exampleSerialMonth,
                                   intervalInMonths);

    // Get corresponding serial months.

    int startSerialMonthCandidate =
                            exampleSerialMonth + startCount * intervalInMonths;

    int endSerialMonthCandidate =
                              exampleSerialMonth + endCount * intervalInMonths;

    // A note on validity of 'startSerialMonth' and 'endSerialMonth'.
    //
    // By construction, 'startSerialMonthCandidate' is bounded below by
    // 'earliestSerialMonth' and 'endSerialMonthCandidate' is bounded above by
    // 'latestSerialMonth'.
    //
    // 'startSerialMonthCandidate', however, is not bounded above, e.g.: If
    // 'earliestSerialMonth == k_MAX_SERIAL_MONTH',
    // 'exampleSerialMonth == k_MAX_SERIAL_MONTH - 1', and
    // 'intervalInMonths == 4' then 'startCount == 1', which implies,
    // 'startSerialMonth == k_MAX_SERIAL_MONTH + 3'.
    //
    // Similarly 'endSerialMonthCandidate' is not bounded below, e.g.: If
    // 'latestSerialMonth == k_MIN_SERIAL_MONTH',
    // 'exampleSerialMonth == k_MIN_SERIAL_MONTH + 1', and
    // 'intervalInMonths == 4' then 'endCount == -1', which implies,
    // 'endSerialMonth == k_MIN_SERIAL_MONTH - 3'.
    //

    if (   startSerialMonthCandidate > k_MAX_SERIAL_MONTH
        || endSerialMonthCandidate < k_MIN_SERIAL_MONTH) {
        return e_OUT_OF_RANGE;                                        // RETURN
    }

    // Load the results.

    *startSerialMonth = startSerialMonthCandidate;
    *endSerialMonth   = endSerialMonthCandidate;

    return e_VALID_RANGE;
}

static
int adjustMonthRange(int *startSerialMonth,
                     int *endSerialMonth,
                     int  startDay,
                     int  endDay,
                     int  earliestDay,
                     int  latestDay,
                     int  earliestSerialMonth,
                     int  latestSerialMonth,
                     int  intervalInMonths)
    // Increment the value of the specified '*startSerialMonth' by the
    // specified 'intervalInMonths' if its value is the same as the specified
    // 'earliestSerialMonth' and the specified 'startDay' is smaller than the
    // specified 'earliestDay'.  Decrement the value of '*endSerialMonth' by
    // the 'intervalInMonths' if its value is the same as the specified
    // 'latestSerialMonth' and the specified 'endDay' is larger than the
    // specified 'latestDay'.  Return 0 on success, and a non-zero value
    // without modifying 'startSerialMonth' or 'endSerialMonth' if these values
    // would exceed the valid range for a serial month.  The behavior is
    // undefined unless
    // 'k_MIN_SERIAL_MONTH <= *startSerialMonth <= k_MAX_SERIAL_MONTH',
    // 'k_MIN_SERIAL_MONTH <= *endSerialMonth <= k_MAX_SERIAL_MONTH',
    // 'k_MIN_SERIAL_MONTH <= earliestSerialMonth <= k_MAX_SERIAL_MONTH',
    // '1 <= startDay <= 31', and '1 <= endDay <= 31'.
{
    BSLS_ASSERT(   *startSerialMonth <= k_MAX_SERIAL_MONTH
                && *startSerialMonth >= k_MIN_SERIAL_MONTH);
    BSLS_ASSERT(   *endSerialMonth <= k_MAX_SERIAL_MONTH
                && *endSerialMonth >= k_MIN_SERIAL_MONTH);
    BSLS_ASSERT(   earliestSerialMonth <= k_MAX_SERIAL_MONTH
                && earliestSerialMonth >= k_MIN_SERIAL_MONTH);
    BSLS_ASSERT(   latestSerialMonth <= k_MAX_SERIAL_MONTH
                && latestSerialMonth >= k_MIN_SERIAL_MONTH);
    BSLS_ASSERT(1 <= startDay && 31 >= startDay);
    BSLS_ASSERT(1 <= endDay   && 31 >= endDay);

    // Return codes
    enum { e_VALID_RANGE = 0, e_OUT_OF_RANGE = 1 };

    int startSerialMonthCandidate = *startSerialMonth;
    int endSerialMonthCandidate   = *endSerialMonth;

    if (   startSerialMonthCandidate == earliestSerialMonth
        && startDay < earliestDay) {
        startSerialMonthCandidate += intervalInMonths;
    }

    if (endSerialMonthCandidate == latestSerialMonth && endDay > latestDay) {
        endSerialMonthCandidate -= intervalInMonths;
    }

    if (   startSerialMonthCandidate > k_MAX_SERIAL_MONTH
        || endSerialMonthCandidate   < k_MIN_SERIAL_MONTH) {
        return e_OUT_OF_RANGE;                                        // RETURN
    }

    *startSerialMonth = startSerialMonthCandidate;
    *endSerialMonth   = endSerialMonthCandidate;

    return e_VALID_RANGE;
}

                      // -----------------------------
                      // struct ScheduleGenerationUtil
                      // -----------------------------

void ScheduleGenerationUtil::generateFromDayInterval(
                                       bsl::vector<bdlt::Date> *schedule,
                                       const bdlt::Date&        earliest,
                                       const bdlt::Date&        latest,
                                       const bdlt::Date&        example,
                                       int                      intervalInDays)
{
    BSLS_ASSERT(schedule);
    BSLS_ASSERT(earliest <= latest);
    BSLS_ASSERT(1 <= intervalInDays);

    schedule->clear();

    int startCount = rationalCeiling(earliest - example, intervalInDays);
    int endCount   = rationalFloor(latest - example, intervalInDays);

    schedule->reserve(abs(endCount - startCount) + 1);

    for (int count = startCount; count <= endCount; count++) {
        schedule->push_back(example + intervalInDays * count);
    }
}

void ScheduleGenerationUtil::generateFromDayOfMonth(
                                     bsl::vector<bdlt::Date> *schedule,
                                     const bdlt::Date&        earliest,
                                     const bdlt::Date&        latest,
                                     int                      exampleYear,
                                     int                      exampleMonth,
                                     int                      intervalInMonths,
                                     int                      targetDayOfMonth,
                                     int                      targetDayOfFeb)
{
    BSLS_ASSERT(schedule);
    BSLS_ASSERT(earliest <= latest);
    BSLS_ASSERT(1 <= intervalInMonths);
    BSLS_ASSERT(1 <= exampleYear      && 9999 >= exampleYear);
    BSLS_ASSERT(1 <= exampleMonth     &&   12 >= exampleMonth);
    BSLS_ASSERT(1 <= targetDayOfMonth &&   31 >= targetDayOfMonth);
    BSLS_ASSERT(0 <= targetDayOfFeb   &&   29 >= targetDayOfFeb);

    schedule->clear();

    int earliestSerialMonth;
    int earliestDay;
    computeSerialMonthAndDay(&earliestSerialMonth, &earliestDay, earliest);

    int latestSerialMonth;
    int latestDay;
    computeSerialMonthAndDay(&latestSerialMonth, &latestDay, latest);

    int startSerialMonth;
    int endSerialMonth;

    if (computeMonthRange(&startSerialMonth,
                          &endSerialMonth,
                          earliestSerialMonth,
                          latestSerialMonth,
                          YM2SERIAL(exampleYear, exampleMonth),
                          intervalInMonths)) {
        // empty schedule

        return;                                                       // RETURN
    }

    int startDay = getDayOfMonth(SERIAL2Y(startSerialMonth),
                                 SERIAL2M(startSerialMonth),
                                 targetDayOfMonth,
                                 targetDayOfFeb).day();

    int endDay   = getDayOfMonth(SERIAL2Y(endSerialMonth),
                                 SERIAL2M(endSerialMonth),
                                 targetDayOfMonth,
                                 targetDayOfFeb).day();

    if (adjustMonthRange(&startSerialMonth,
                         &endSerialMonth,
                         startDay,
                         endDay,
                         earliestDay,
                         latestDay,
                         earliestSerialMonth,
                         latestSerialMonth,
                         intervalInMonths)) {
        // empty schedule

        return;                                                       // RETURN
    }

    schedule->reserve((endSerialMonth - startSerialMonth)
                                                       / intervalInMonths + 1);

    for (int sm = startSerialMonth;
         sm <= endSerialMonth;
         sm += intervalInMonths) {
        schedule->push_back(getDayOfMonth(SERIAL2Y(sm),
                                          SERIAL2M(sm),
                                          targetDayOfMonth,
                                          targetDayOfFeb));
    }
}

void ScheduleGenerationUtil::generateFromBusinessDayOfMonth(
                             bsl::vector<bdlt::Date> *schedule,
                             const bdlt::Date&        earliest,
                             const bdlt::Date&        latest,
                             int                      exampleYear,
                             int                      exampleMonth,
                             int                      intervalInMonths,
                             const bdlt::Calendar&    calendar,
                             int                      targetBusinessDayOfMonth)
{
    BSLS_ASSERT(schedule);
    BSLS_ASSERT(earliest <= latest);
    BSLS_ASSERT(1 <= intervalInMonths);
    BSLS_ASSERT(1 <= exampleYear    && 9999 >= exampleYear);
    BSLS_ASSERT(1 <= exampleMonth   &&   12 >= exampleMonth);
    BSLS_ASSERT(   -31 <= targetBusinessDayOfMonth
                &&  31 >= targetBusinessDayOfMonth
                &&   0 != targetBusinessDayOfMonth);

    schedule->clear();

    int earliestSerialMonth;
    int earliestDay;
    computeSerialMonthAndDay(&earliestSerialMonth, &earliestDay, earliest);

    int latestSerialMonth;
    int latestDay;
    computeSerialMonthAndDay(&latestSerialMonth, &latestDay, latest);

    int startSerialMonth;
    int endSerialMonth;

    if (computeMonthRange(&startSerialMonth,
                          &endSerialMonth,
                          earliestSerialMonth,
                          latestSerialMonth,
                          YM2SERIAL(exampleYear, exampleMonth),
                          intervalInMonths)) {
        // empty schedule

        return;                                                       // RETURN
    }

    int startDay;
    {
        bdlt::Date rv;

        if (bdlt::CalendarUtil::nthBusinessDayOfMonthOrMaxIfValid(
                                                   &rv,
                                                   calendar,
                                                   SERIAL2Y(startSerialMonth),
                                                   SERIAL2M(startSerialMonth),
                                                   targetBusinessDayOfMonth)) {
            // empty schedule

            return;                                                   // RETURN
        }
        startDay = rv.day();
    }

    int endDay;
    {
        bdlt::Date rv;

        if (bdlt::CalendarUtil::nthBusinessDayOfMonthOrMaxIfValid(
                                                   &rv,
                                                   calendar,
                                                   SERIAL2Y(endSerialMonth),
                                                   SERIAL2M(endSerialMonth),
                                                   targetBusinessDayOfMonth)) {
            // empty schedule

            return;                                                   // RETURN
        }
        endDay = rv.day();
    }

    if (adjustMonthRange(&startSerialMonth,
                         &endSerialMonth,
                         startDay,
                         endDay,
                         earliestDay,
                         latestDay,
                         earliestSerialMonth,
                         latestSerialMonth,
                         intervalInMonths)) {
        // empty schedule

        return;                                                       // RETURN
    }

    schedule->reserve((endSerialMonth - startSerialMonth)
                                                       / intervalInMonths + 1);

    for (int sm = startSerialMonth;
         sm <= endSerialMonth;
         sm += intervalInMonths) {
        bdlt::Date rv;
        if (bdlt::CalendarUtil::nthBusinessDayOfMonthOrMaxIfValid(
                                                   &rv,
                                                   calendar,
                                                   SERIAL2Y(sm),
                                                   SERIAL2M(sm),
                                                   targetBusinessDayOfMonth)) {
            // empty schedule

            schedule->clear();
            return;                                                   // RETURN
        }
        schedule->push_back(rv);
    }
}

void ScheduleGenerationUtil::generateFromDayOfWeekAfterDayOfMonth(
                                     bsl::vector<bdlt::Date> *schedule,
                                     const bdlt::Date&        earliest,
                                     const bdlt::Date&        latest,
                                     int                      exampleYear,
                                     int                      exampleMonth,
                                     int                      intervalInMonths,
                                     bdlt::DayOfWeek::Enum    dayOfWeek,
                                     int                      dayOfMonth)
{
    BSLS_ASSERT(schedule);
    BSLS_ASSERT(earliest <= latest);
    BSLS_ASSERT(1 <= intervalInMonths);
    BSLS_ASSERT(1 <= exampleYear    && 9999 >= exampleYear);
    BSLS_ASSERT(1 <= exampleMonth   &&   12 >= exampleMonth);
    BSLS_ASSERT(1 <= dayOfMonth     &&   31 >= dayOfMonth);

    schedule->clear();

    int earliestSerialMonth;
    int earliestDay;
    computeSerialMonthAndDay(&earliestSerialMonth, &earliestDay, earliest);

    int latestSerialMonth;
    int latestDay;
    computeSerialMonthAndDay(&latestSerialMonth, &latestDay, latest);

    int startSerialMonth;
    int endSerialMonth;

    if (computeMonthRange(&startSerialMonth,
                          &endSerialMonth,
                          earliestSerialMonth,
                          latestSerialMonth,
                          YM2SERIAL(exampleYear, exampleMonth),
                          intervalInMonths)) {
        // empty schedule

        return;                                                       // RETURN
    }

    int startDay;
    int endDay;
    {
        int startYear  = SERIAL2Y(startSerialMonth);
        int startMonth = SERIAL2M(startSerialMonth);
        int endYear    = SERIAL2Y(endSerialMonth);
        int endMonth   = SERIAL2M(endSerialMonth);

        if (   !bdlt::Date::isValidYearMonthDay(startYear,
                                                startMonth,
                                                dayOfMonth)
            || !bdlt::Date::isValidYearMonthDay(endYear,
                                                endMonth,
                                                dayOfMonth)) {
            // empty schedule

            return;                                                   // RETURN
        }

        startDay = bdlt::DateUtil::nextDayOfWeekInclusive(
                                                 dayOfWeek,
                                                 bdlt::Date(startYear,
                                                            startMonth,
                                                            dayOfMonth)).day();

        endDay   = bdlt::DateUtil::nextDayOfWeekInclusive(
                                                dayOfWeek,
                                                bdlt::Date(endYear,
                                                           endMonth,
                                                           dayOfMonth)).day();
    }

    if (adjustMonthRange(&startSerialMonth,
                         &endSerialMonth,
                         startDay,
                         endDay,
                         earliestDay,
                         latestDay,
                         earliestSerialMonth,
                         latestSerialMonth,
                         intervalInMonths)) {
        // empty schedule

        return;                                                       // RETURN
    }

    schedule->reserve((endSerialMonth - startSerialMonth)
                                                       / intervalInMonths + 1);

    for (int sm = startSerialMonth;
         sm <= endSerialMonth;
         sm += intervalInMonths) {
        int year  = SERIAL2Y(sm);
        int month = SERIAL2M(sm);

        if (!bdlt::Date::isValidYearMonthDay(year, month, dayOfMonth)) {
            // empty schedule

            schedule->clear();
            return;                                                   // RETURN
        }

        schedule->push_back(bdlt::DateUtil::nextDayOfWeekInclusive(
                                                      dayOfWeek,
                                                      bdlt::Date(year,
                                                                 month,
                                                                 dayOfMonth)));
    }
}

void ScheduleGenerationUtil::generateFromDayOfWeekInMonth(
                                     bsl::vector<bdlt::Date> *schedule,
                                     const bdlt::Date&        earliest,
                                     const bdlt::Date&        latest,
                                     int                      exampleYear,
                                     int                      exampleMonth,
                                     int                      intervalInMonths,
                                     bdlt::DayOfWeek::Enum    dayOfWeek,
                                     int                      occurrenceWeek)
{
    BSLS_ASSERT(schedule);
    BSLS_ASSERT(earliest <= latest);
    BSLS_ASSERT(1 <= intervalInMonths);
    BSLS_ASSERT(1 <= exampleYear    && 9999 >= exampleYear);
    BSLS_ASSERT(1 <= exampleMonth   &&   12 >= exampleMonth);
    BSLS_ASSERT(1 <= occurrenceWeek &&    4 >= occurrenceWeek);

    schedule->clear();

    int earliestSerialMonth;
    int earliestDay;
    computeSerialMonthAndDay(&earliestSerialMonth, &earliestDay, earliest);

    int latestSerialMonth;
    int latestDay;
    computeSerialMonthAndDay(&latestSerialMonth, &latestDay, latest);

    int startSerialMonth;
    int endSerialMonth;

    if (computeMonthRange(&startSerialMonth,
                          &endSerialMonth,
                          earliestSerialMonth,
                          latestSerialMonth,
                          YM2SERIAL(exampleYear, exampleMonth),
                          intervalInMonths)) {
        // empty schedule

        return;                                                       // RETURN
    }

    int startDay = bdlt::DateUtil::nthDayOfWeekInMonth(
                                                    SERIAL2Y(startSerialMonth),
                                                    SERIAL2M(startSerialMonth),
                                                    dayOfWeek,
                                                    occurrenceWeek).day();

    int endDay   = bdlt::DateUtil::nthDayOfWeekInMonth(
                                                      SERIAL2Y(endSerialMonth),
                                                      SERIAL2M(endSerialMonth),
                                                      dayOfWeek,
                                                      occurrenceWeek).day();
    if (adjustMonthRange(&startSerialMonth,
                         &endSerialMonth,
                         startDay,
                         endDay,
                         earliestDay,
                         latestDay,
                         earliestSerialMonth,
                         latestSerialMonth,
                         intervalInMonths)) {
        // empty schedule

        return;                                                       // RETURN
    }

    schedule->reserve((endSerialMonth - startSerialMonth)
                                                       / intervalInMonths + 1);

    for (int sm = startSerialMonth;
         sm <= endSerialMonth;
         sm += intervalInMonths) {
        schedule->push_back(bdlt::DateUtil::nthDayOfWeekInMonth(
                                                              SERIAL2Y(sm),
                                                              SERIAL2M(sm),
                                                              dayOfWeek,
                                                              occurrenceWeek));
    }
}

}  // close package namespace
}  // close enterprise namespace

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
