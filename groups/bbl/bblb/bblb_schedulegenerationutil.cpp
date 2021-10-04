// bblb_schedulegenerationutil.cpp                                    -*-C++-*-
#include <bblb_schedulegenerationutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bblb_schedulegenerationutil_cpp,"$Id$ $CSID$")

#include <bdlt_calendar.h>
#include <bdlt_calendarutil.h>
#include <bdlt_date.h>
#include <bdlt_dateutil.h>
#include <bdlt_serialdateimputil.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>

namespace {
namespace u {

using namespace BloombergLP;
using namespace bblb;

template <class VECTOR, class ELEMENT_TYPE>
struct IsVector {
    static const bool value =
                  bsl::is_same<VECTOR, bsl::vector<     ELEMENT_TYPE> >::value
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
               || bsl::is_same<VECTOR, std::pmr::vector<ELEMENT_TYPE> >::value
#endif
               || bsl::is_same<VECTOR, std::vector<     ELEMENT_TYPE> >::value;
};

// For 'bdlt::Date', the valid range of years is [1, 9999] and months is
// [1, 12].

const int k_MAX_SERIAL_MONTH = (9999*12 + 12) - 1;
const int k_MIN_SERIAL_MONTH = (1*12 + 1) - 1;

// Helper macros to convert to and from serial month.

#define U_YM2SERIAL(Y,M) 12*(Y) + (M) - 1
#define U_SERIAL2Y(S) (S)/12
#define U_SERIAL2M(S) (S)%12 + 1

inline
void computeSerialMonthAndDay(int               *serialMonth,
                              int               *day,
                              const bdlt::Date&  date)
    // Load in the specified 'serialMonth' the serial month value computed from
    // the specified 'date' and load into 'day' the value of 'date.day()'.
{
    int year;
    int month;
    date.getYearMonthDay(&year, &month, day);
    *serialMonth = U_YM2SERIAL(year, month);
}

inline
bdlt::Date getDayOfMonth(int year, int month, int day, int dayOfFeb)
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

inline
int rationalFloor(int numerator, int denominator)
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

inline
int rationalCeiling(int numerator, int denominator)
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
    BSLS_ASSERT(   u::k_MAX_SERIAL_MONTH >= exampleSerialMonth
                && u::k_MIN_SERIAL_MONTH <= exampleSerialMonth);
    BSLS_ASSERT(earliestSerialMonth <= latestSerialMonth);

    // Return codes.

    enum { e_VALID_RANGE = 0, e_OUT_OF_RANGE = 1 };

    // Compute how many periods away the start and end are.

    int startCount = u::rationalCeiling(earliestSerialMonth-exampleSerialMonth,
                                        intervalInMonths);

    int endCount   = u::rationalFloor(latestSerialMonth - exampleSerialMonth,
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

    if (   startSerialMonthCandidate > u::k_MAX_SERIAL_MONTH
        || endSerialMonthCandidate < u::k_MIN_SERIAL_MONTH) {
        return e_OUT_OF_RANGE;                                        // RETURN
    }

    // Load the results.

    *startSerialMonth = startSerialMonthCandidate;
    *endSerialMonth   = endSerialMonthCandidate;

    return e_VALID_RANGE;
}

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
    BSLS_ASSERT(   *startSerialMonth <= u::k_MAX_SERIAL_MONTH
                && *startSerialMonth >= u::k_MIN_SERIAL_MONTH);
    BSLS_ASSERT(   *endSerialMonth <= u::k_MAX_SERIAL_MONTH
                && *endSerialMonth >= u::k_MIN_SERIAL_MONTH);
    BSLS_ASSERT(   earliestSerialMonth <= u::k_MAX_SERIAL_MONTH
                && earliestSerialMonth >= u::k_MIN_SERIAL_MONTH);
    BSLS_ASSERT(   latestSerialMonth <= u::k_MAX_SERIAL_MONTH
                && latestSerialMonth >= u::k_MIN_SERIAL_MONTH);
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

    if (   startSerialMonthCandidate > u::k_MAX_SERIAL_MONTH
        || endSerialMonthCandidate   < u::k_MIN_SERIAL_MONTH) {
        return e_OUT_OF_RANGE;                                        // RETURN
    }

    *startSerialMonth = startSerialMonthCandidate;
    *endSerialMonth   = endSerialMonthCandidate;

    return e_VALID_RANGE;
}

template <class VECTOR>
inline
void generateFromDayIntervalImp(VECTOR            *schedule,
                                const bdlt::Date&  earliest,
                                const bdlt::Date&  latest,
                                const bdlt::Date&  example,
                                int                intervalInDays)
    // Load, into the specified 'schedule', the chronologically increasing
    // sequence of unique dates that are integral multiples of the specified
    // 'intervalInDays' away from the specified 'example' date, and within the
    // specified closed-interval '[earliest, latest]'.  The behavior is
    // undefined unless 'earliest <= latest' and '1 <= intervalInDays'.
{
    BSLS_ASSERT(schedule);
    BSLS_ASSERT(earliest <= latest);
    BSLS_ASSERT(1 <= intervalInDays);

    BSLMF_ASSERT((u::IsVector<VECTOR, bdlt::Date>::value));

    schedule->clear();

    int startCount = u::rationalCeiling(earliest - example, intervalInDays);
    int endCount   = u::rationalFloor(latest - example, intervalInDays);

    schedule->reserve(bsl::abs(endCount - startCount) + 1);

    for (int count = startCount; count <= endCount; count++) {
        schedule->push_back(example + intervalInDays * count);
    }
}

template <class VECTOR>
inline
void generateFromDayOfMonthImp(VECTOR            *schedule,
                               const bdlt::Date&  earliest,
                               const bdlt::Date&  latest,
                               int                exampleYear,
                               int                exampleMonth,
                               int                intervalInMonths,
                               int                targetDayOfMonth,
                               int                targetDayOfFeb)
    // Load, into the specified 'schedule', the chronologically increasing
    // sequence of unique dates that are on the specified 'targetDayOfMonth'
    // (or the last day of the month if 'targetDayOfMonth' would be past the
    // end of the month), integral multiples of the specified
    // 'intervalInMonths' away from the specified 'exampleYear' and
    // 'exampleMonth', and within the specified closed-interval
    // '[earliest, latest]'.  Optionally specify 'targetDayOfFeb' to replace
    // 'targetDayOfMonth' whenever the month of a 'schedule' entry is February.
    // The behavior is undefined unless 'earliest <= latest',
    // '1 <= exampleYear <= 9999', '1 <= exampleMonth <= 12',
    // '1 <= intervalInMonths', '1 <= targetDayOfMonth <= 31', and
    // '0 <= targetDayOfFeb <= 29'.
{
    BSLS_ASSERT(schedule);
    BSLS_ASSERT(earliest <= latest);
    BSLS_ASSERT(1 <= intervalInMonths);
    BSLS_ASSERT(1 <= exampleYear      && 9999 >= exampleYear);
    BSLS_ASSERT(1 <= exampleMonth     &&   12 >= exampleMonth);
    BSLS_ASSERT(1 <= targetDayOfMonth &&   31 >= targetDayOfMonth);
    BSLS_ASSERT(0 <= targetDayOfFeb   &&   29 >= targetDayOfFeb);

    BSLMF_ASSERT((u::IsVector<VECTOR, bdlt::Date>::value));

    schedule->clear();

    int earliestSerialMonth;
    int earliestDay;
    u::computeSerialMonthAndDay(&earliestSerialMonth, &earliestDay, earliest);

    int latestSerialMonth;
    int latestDay;
    u::computeSerialMonthAndDay(&latestSerialMonth, &latestDay, latest);

    int startSerialMonth;
    int endSerialMonth;

    if (u::computeMonthRange(&startSerialMonth,
                             &endSerialMonth,
                             earliestSerialMonth,
                             latestSerialMonth,
                             U_YM2SERIAL(exampleYear, exampleMonth),
                             intervalInMonths)) {
        // empty schedule

        return;                                                       // RETURN
    }

    int startDay = u::getDayOfMonth(U_SERIAL2Y(startSerialMonth),
                                    U_SERIAL2M(startSerialMonth),
                                    targetDayOfMonth,
                                    targetDayOfFeb).day();

    int endDay   = u::getDayOfMonth(U_SERIAL2Y(endSerialMonth),
                                    U_SERIAL2M(endSerialMonth),
                                    targetDayOfMonth,
                                    targetDayOfFeb).day();

    if (u::adjustMonthRange(&startSerialMonth,
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
        schedule->push_back(u::getDayOfMonth(U_SERIAL2Y(sm),
                                             U_SERIAL2M(sm),
                                             targetDayOfMonth,
                                             targetDayOfFeb));
    }
}

template <class VECTOR>
inline
void generateFromBusinessDayOfMonthImp(
                             VECTOR                *schedule,
                             const bdlt::Date&      earliest,
                             const bdlt::Date&      latest,
                             int                    exampleYear,
                             int                    exampleMonth,
                             int                    intervalInMonths,
                             const bdlt::Calendar&  calendar,
                             int                    targetBusinessDayOfMonth)
    // Load, into the specified 'schedule', the chronologically increasing
    // sequence of unique dates that are on the specified
    // 'targetBusinessDayOfMonth' (or the highest count possible in the
    // resulting month), integral multiples of the specified 'intervalInMonths'
    // away from the specified 'exampleYear' and 'exampleMonth', and within the
    // specified closed-interval '[earliest, latest]'.  Business days, as per
    // the specified 'calendar', are counted, if 'targetBusinessDayOfMonth' is
    // positive, from and including the chronologically earliest business day
    // to chronologically later business days, and if
    // 'targetBusinessDayOfMonth' is negative, from and including the
    // chronologically latest business day to chronologically earlier business
    // days.  If any of the months required for the schedule do not have a
    // business day, return an empty 'schedule'.  The behavior is undefined
    // unless 'earliest <= latest', '1 <= exampleYear <= 9999',
    // '1 <= exampleMonth <= 12', '1 <= intervalInMonths', and
    // '1 <= abs(targetBusinessDayOfMonth) <= 31'.
{
    BSLS_ASSERT(schedule);
    BSLS_ASSERT(earliest <= latest);
    BSLS_ASSERT(1 <= intervalInMonths);
    BSLS_ASSERT(1 <= exampleYear    && 9999 >= exampleYear);
    BSLS_ASSERT(1 <= exampleMonth   &&   12 >= exampleMonth);
    BSLS_ASSERT(   -31 <= targetBusinessDayOfMonth
                &&  31 >= targetBusinessDayOfMonth
                &&   0 != targetBusinessDayOfMonth);

    BSLMF_ASSERT((u::IsVector<VECTOR, bdlt::Date>::value));

    schedule->clear();

    int earliestSerialMonth;
    int earliestDay;
    u::computeSerialMonthAndDay(&earliestSerialMonth, &earliestDay, earliest);

    int latestSerialMonth;
    int latestDay;
    u::computeSerialMonthAndDay(&latestSerialMonth, &latestDay, latest);

    int startSerialMonth;
    int endSerialMonth;

    if (u::computeMonthRange(&startSerialMonth,
                             &endSerialMonth,
                             earliestSerialMonth,
                             latestSerialMonth,
                             U_YM2SERIAL(exampleYear, exampleMonth),
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
                                                  U_SERIAL2Y(startSerialMonth),
                                                  U_SERIAL2M(startSerialMonth),
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
                                                   U_SERIAL2Y(endSerialMonth),
                                                   U_SERIAL2M(endSerialMonth),
                                                   targetBusinessDayOfMonth)) {
            // empty schedule

            return;                                                   // RETURN
        }
        endDay = rv.day();
    }

    if (u::adjustMonthRange(&startSerialMonth,
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
                                                   U_SERIAL2Y(sm),
                                                   U_SERIAL2M(sm),
                                                   targetBusinessDayOfMonth)) {
            // empty schedule

            schedule->clear();
            return;                                                   // RETURN
        }
        schedule->push_back(rv);
    }
}

template <class VECTOR>
inline
void generateFromDayOfWeekAfterDayOfMonthImp(
                                     VECTOR                *schedule,
                                     const bdlt::Date&      earliest,
                                     const bdlt::Date&      latest,
                                     int                    exampleYear,
                                     int                    exampleMonth,
                                     int                    intervalInMonths,
                                     bdlt::DayOfWeek::Enum  dayOfWeek,
                                     int                    dayOfMonth)
    // Load, into the specified 'schedule', the chronologically increasing
    // sequence of unique dates that are on the specified 'dayOfWeek' on or
    // after the specified 'dayOfMonth', integral multiples of the specified
    // 'intervalInMonths' away from the specified 'exampleYear' and
    // 'exampleMonth', and within the specified closed-interval
    // '[earliest, latest]'.  If any of the months required for the schedule
    // have fewer than 'dayOfMonth' days, return an empty 'schedule'.  The
    // behavior is undefined unless 'earliest <= latest',
    // '1 <= exampleYear <= 9999', '1 <= exampleMonth <= 12',
    // '1 <= intervalInMonths', and '1 <= dayOfMonth <= 31'.
{
    BSLS_ASSERT(schedule);
    BSLS_ASSERT(earliest <= latest);
    BSLS_ASSERT(1 <= intervalInMonths);
    BSLS_ASSERT(1 <= exampleYear    && 9999 >= exampleYear);
    BSLS_ASSERT(1 <= exampleMonth   &&   12 >= exampleMonth);
    BSLS_ASSERT(1 <= dayOfMonth     &&   31 >= dayOfMonth);

    BSLMF_ASSERT((u::IsVector<VECTOR, bdlt::Date>::value));

    schedule->clear();

    int earliestSerialMonth;
    int earliestDay;
    u::computeSerialMonthAndDay(&earliestSerialMonth, &earliestDay, earliest);

    int latestSerialMonth;
    int latestDay;
    u::computeSerialMonthAndDay(&latestSerialMonth, &latestDay, latest);

    int startSerialMonth;
    int endSerialMonth;

    if (u::computeMonthRange(&startSerialMonth,
                             &endSerialMonth,
                             earliestSerialMonth,
                             latestSerialMonth,
                             U_YM2SERIAL(exampleYear, exampleMonth),
                             intervalInMonths)) {
        // empty schedule

        return;                                                       // RETURN
    }

    int startDay;
    int endDay;
    {
        int startYear  = U_SERIAL2Y(startSerialMonth);
        int startMonth = U_SERIAL2M(startSerialMonth);
        int endYear    = U_SERIAL2Y(endSerialMonth);
        int endMonth   = U_SERIAL2M(endSerialMonth);

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

    if (u::adjustMonthRange(&startSerialMonth,
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
        int year  = U_SERIAL2Y(sm);
        int month = U_SERIAL2M(sm);

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

template <class VECTOR>
inline
void generateFromDayOfWeekInMonthImp(VECTOR                *schedule,
                                     const bdlt::Date&      earliest,
                                     const bdlt::Date&      latest,
                                     int                    exampleYear,
                                     int                    exampleMonth,
                                     int                    intervalInMonths,
                                     bdlt::DayOfWeek::Enum  dayOfWeek,
                                     int                    occurrenceWeek)
    // Load, into the specified 'schedule', the chronologically increasing
    // sequence of unique dates that are on the specified 'dayOfWeek' of the
    // specified 'occurrenceWeek' of the month, integral multiples of the
    // specified 'intervalInMonths' away from the specified 'exampleYear' and
    // 'exampleMonth', and within the specified closed-interval
    // '[earliest, latest]'.  The behavior is undefined unless
    // 'earliest <= latest', '1 <= exampleYear <= 9999',
    // '1 <= exampleMonth <= 12', '1 <= intervalInMonths', and
    // '1 <= occurrenceWeek <= 4'.
{
    BSLS_ASSERT(schedule);
    BSLS_ASSERT(earliest <= latest);
    BSLS_ASSERT(1 <= intervalInMonths);
    BSLS_ASSERT(1 <= exampleYear    && 9999 >= exampleYear);
    BSLS_ASSERT(1 <= exampleMonth   &&   12 >= exampleMonth);
    BSLS_ASSERT(1 <= occurrenceWeek &&    4 >= occurrenceWeek);

    BSLMF_ASSERT((u::IsVector<VECTOR, bdlt::Date>::value));

    schedule->clear();

    int earliestSerialMonth;
    int earliestDay;
    u::computeSerialMonthAndDay(&earliestSerialMonth, &earliestDay, earliest);

    int latestSerialMonth;
    int latestDay;
    u::computeSerialMonthAndDay(&latestSerialMonth, &latestDay, latest);

    int startSerialMonth;
    int endSerialMonth;

    if (u::computeMonthRange(&startSerialMonth,
                             &endSerialMonth,
                             earliestSerialMonth,
                             latestSerialMonth,
                             U_YM2SERIAL(exampleYear, exampleMonth),
                             intervalInMonths)) {
        // empty schedule

        return;                                                       // RETURN
    }

    int startDay = bdlt::DateUtil::nthDayOfWeekInMonth(
                                                  U_SERIAL2Y(startSerialMonth),
                                                  U_SERIAL2M(startSerialMonth),
                                                  dayOfWeek,
                                                  occurrenceWeek).day();

    int endDay   = bdlt::DateUtil::nthDayOfWeekInMonth(
                                                    U_SERIAL2Y(endSerialMonth),
                                                    U_SERIAL2M(endSerialMonth),
                                                    dayOfWeek,
                                                    occurrenceWeek).day();
    if (u::adjustMonthRange(&startSerialMonth,
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
                                                              U_SERIAL2Y(sm),
                                                              U_SERIAL2M(sm),
                                                              dayOfWeek,
                                                              occurrenceWeek));
    }
}

}  // close namespace u
}  // close unnamed namespace


namespace BloombergLP {
namespace bblb {

                      // -----------------------------
                      // struct ScheduleGenerationUtil
                      // -----------------------------

void ScheduleGenerationUtil::generateFromDayInterval(
                                  bsl::vector<bdlt::Date>      *schedule,
                                  const bdlt::Date&             earliest,
                                  const bdlt::Date&             latest,
                                  const bdlt::Date&             example,
                                  int                           intervalInDays)
{
    u::generateFromDayIntervalImp(schedule,
                                  earliest,
                                  latest,
                                  example,
                                  intervalInDays);
}

void ScheduleGenerationUtil::generateFromDayInterval(
                                  std::vector<bdlt::Date>      *schedule,
                                  const bdlt::Date&             earliest,
                                  const bdlt::Date&             latest,
                                  const bdlt::Date&             example,
                                  int                           intervalInDays)
{
    u::generateFromDayIntervalImp(schedule,
                                  earliest,
                                  latest,
                                  example,
                                  intervalInDays);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
void ScheduleGenerationUtil::generateFromDayInterval(
                                  std::pmr::vector<bdlt::Date> *schedule,
                                  const bdlt::Date&             earliest,
                                  const bdlt::Date&             latest,
                                  const bdlt::Date&             example,
                                  int                           intervalInDays)
{
    u::generateFromDayIntervalImp(schedule,
                                  earliest,
                                  latest,
                                  example,
                                  intervalInDays);
}
#endif

void ScheduleGenerationUtil::generateFromDayOfMonth(
                                bsl::vector<bdlt::Date>      *schedule,
                                const bdlt::Date&             earliest,
                                const bdlt::Date&             latest,
                                int                           exampleYear,
                                int                           exampleMonth,
                                int                           intervalInMonths,
                                int                           targetDayOfMonth,
                                int                           targetDayOfFeb)
{
    u::generateFromDayOfMonthImp(schedule,
                                 earliest,
                                 latest,
                                 exampleYear,
                                 exampleMonth,
                                 intervalInMonths,
                                 targetDayOfMonth,
                                 targetDayOfFeb);
}

void ScheduleGenerationUtil::generateFromDayOfMonth(
                                std::vector<bdlt::Date>      *schedule,
                                const bdlt::Date&             earliest,
                                const bdlt::Date&             latest,
                                int                           exampleYear,
                                int                           exampleMonth,
                                int                           intervalInMonths,
                                int                           targetDayOfMonth,
                                int                           targetDayOfFeb)
{
    u::generateFromDayOfMonthImp(schedule,
                                 earliest,
                                 latest,
                                 exampleYear,
                                 exampleMonth,
                                 intervalInMonths,
                                 targetDayOfMonth,
                                 targetDayOfFeb);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
void ScheduleGenerationUtil::generateFromDayOfMonth(
                                std::pmr::vector<bdlt::Date> *schedule,
                                const bdlt::Date&             earliest,
                                const bdlt::Date&             latest,
                                int                           exampleYear,
                                int                           exampleMonth,
                                int                           intervalInMonths,
                                int                           targetDayOfMonth,
                                int                           targetDayOfFeb)
{
    u::generateFromDayOfMonthImp(schedule,
                                 earliest,
                                 latest,
                                 exampleYear,
                                 exampleMonth,
                                 intervalInMonths,
                                 targetDayOfMonth,
                                 targetDayOfFeb);
}
#endif

void ScheduleGenerationUtil::generateFromBusinessDayOfMonth(
                        bsl::vector<bdlt::Date>      *schedule,
                        const bdlt::Date&             earliest,
                        const bdlt::Date&             latest,
                        int                           exampleYear,
                        int                           exampleMonth,
                        int                           intervalInMonths,
                        const bdlt::Calendar&         calendar,
                        int                           targetBusinessDayOfMonth)
{
    u::generateFromBusinessDayOfMonthImp(schedule,
                                         earliest,
                                         latest,
                                         exampleYear,
                                         exampleMonth,
                                         intervalInMonths,
                                         calendar,
                                         targetBusinessDayOfMonth);
}

void ScheduleGenerationUtil::generateFromBusinessDayOfMonth(
                        std::vector<bdlt::Date>      *schedule,
                        const bdlt::Date&             earliest,
                        const bdlt::Date&             latest,
                        int                           exampleYear,
                        int                           exampleMonth,
                        int                           intervalInMonths,
                        const bdlt::Calendar&         calendar,
                        int                           targetBusinessDayOfMonth)
{
    u::generateFromBusinessDayOfMonthImp(schedule,
                                         earliest,
                                         latest,
                                         exampleYear,
                                         exampleMonth,
                                         intervalInMonths,
                                         calendar,
                                         targetBusinessDayOfMonth);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
void ScheduleGenerationUtil::generateFromBusinessDayOfMonth(
                        std::pmr::vector<bdlt::Date> *schedule,
                        const bdlt::Date&             earliest,
                        const bdlt::Date&             latest,
                        int                           exampleYear,
                        int                           exampleMonth,
                        int                           intervalInMonths,
                        const bdlt::Calendar&         calendar,
                        int                           targetBusinessDayOfMonth)
{
    u::generateFromBusinessDayOfMonthImp(schedule,
                                         earliest,
                                         latest,
                                         exampleYear,
                                         exampleMonth,
                                         intervalInMonths,
                                         calendar,
                                         targetBusinessDayOfMonth);
}
#endif

void ScheduleGenerationUtil::generateFromDayOfWeekAfterDayOfMonth(
                                bsl::vector<bdlt::Date>      *schedule,
                                const bdlt::Date&             earliest,
                                const bdlt::Date&             latest,
                                int                           exampleYear,
                                int                           exampleMonth,
                                int                           intervalInMonths,
                                bdlt::DayOfWeek::Enum         dayOfWeek,
                                int                           dayOfMonth)
{
    u::generateFromDayOfWeekAfterDayOfMonthImp(schedule,
                                               earliest,
                                               latest,
                                               exampleYear,
                                               exampleMonth,
                                               intervalInMonths,
                                               dayOfWeek,
                                               dayOfMonth);
}

void ScheduleGenerationUtil::generateFromDayOfWeekAfterDayOfMonth(
                                std::vector<bdlt::Date>      *schedule,
                                const bdlt::Date&             earliest,
                                const bdlt::Date&             latest,
                                int                           exampleYear,
                                int                           exampleMonth,
                                int                           intervalInMonths,
                                bdlt::DayOfWeek::Enum         dayOfWeek,
                                int                           dayOfMonth)
{
    u::generateFromDayOfWeekAfterDayOfMonthImp(schedule,
                                               earliest,
                                               latest,
                                               exampleYear,
                                               exampleMonth,
                                               intervalInMonths,
                                               dayOfWeek,
                                               dayOfMonth);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
void ScheduleGenerationUtil::generateFromDayOfWeekAfterDayOfMonth(
                                std::pmr::vector<bdlt::Date> *schedule,
                                const bdlt::Date&             earliest,
                                const bdlt::Date&             latest,
                                int                           exampleYear,
                                int                           exampleMonth,
                                int                           intervalInMonths,
                                bdlt::DayOfWeek::Enum         dayOfWeek,
                                int                           dayOfMonth)
{
    u::generateFromDayOfWeekAfterDayOfMonthImp(schedule,
                                               earliest,
                                               latest,
                                               exampleYear,
                                               exampleMonth,
                                               intervalInMonths,
                                               dayOfWeek,
                                               dayOfMonth);
}
#endif

void ScheduleGenerationUtil::generateFromDayOfWeekInMonth(
                                bsl::vector<bdlt::Date>      *schedule,
                                const bdlt::Date&             earliest,
                                const bdlt::Date&             latest,
                                int                           exampleYear,
                                int                           exampleMonth,
                                int                           intervalInMonths,
                                bdlt::DayOfWeek::Enum         dayOfWeek,
                                int                           occurrenceWeek)
{
    u::generateFromDayOfWeekInMonthImp(schedule,
                                       earliest,
                                       latest,
                                       exampleYear,
                                       exampleMonth,
                                       intervalInMonths,
                                       dayOfWeek,
                                       occurrenceWeek);
}

void ScheduleGenerationUtil::generateFromDayOfWeekInMonth(
                                std::vector<bdlt::Date>      *schedule,
                                const bdlt::Date&             earliest,
                                const bdlt::Date&             latest,
                                int                           exampleYear,
                                int                           exampleMonth,
                                int                           intervalInMonths,
                                bdlt::DayOfWeek::Enum         dayOfWeek,
                                int                           occurrenceWeek)
{
    u::generateFromDayOfWeekInMonthImp(schedule,
                                       earliest,
                                       latest,
                                       exampleYear,
                                       exampleMonth,
                                       intervalInMonths,
                                       dayOfWeek,
                                       occurrenceWeek);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
void ScheduleGenerationUtil::generateFromDayOfWeekInMonth(
                                std::pmr::vector<bdlt::Date> *schedule,
                                const bdlt::Date&             earliest,
                                const bdlt::Date&             latest,
                                int                           exampleYear,
                                int                           exampleMonth,
                                int                           intervalInMonths,
                                bdlt::DayOfWeek::Enum         dayOfWeek,
                                int                           occurrenceWeek)
{
    u::generateFromDayOfWeekInMonthImp(schedule,
                                       earliest,
                                       latest,
                                       exampleYear,
                                       exampleMonth,
                                       intervalInMonths,
                                       dayOfWeek,
                                       occurrenceWeek);
}
#endif

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
