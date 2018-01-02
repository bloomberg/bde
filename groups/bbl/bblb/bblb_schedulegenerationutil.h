// bblb_schedulegenerationutil.h                                      -*-C++-*-
#ifndef INCLUDED_BBLB_SCHEDULEGENERATIONUTIL
#define INCLUDED_BBLB_SCHEDULEGENERATIONUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functions for generating schedules of dates.
//
//@CLASSES:
//  bblb::ScheduleGenerationUtil: namespace for schedule generation functions
//
//@SEE ALSO:
//
//@DESCRIPTION: This component provides a 'struct',
// 'bblb::ScheduleGenerationUtil', that serves as a namespace for functions
// that generate a schedule; a set of dates limited to within a closed-interval
// date-range, represented by the specified 'earliest' and 'latest' dates.
// Typically, a schedule generation method can be defined by an algorithm
// specific to the method, an 'example' date, a closed-interval represented by
// an 'earliest' and a 'latest' date, and any other information required to
// determine the interval between successive dates in a schedule (for example,
// the number of days or months between successive dates).  The process of
// computing the dates within a schedule is exemplified using the following
// diagram:
//..
// ___________|__________________|___________________|________
//        'example'          'earliest'          'latest'
//            .___.___.___.___.___|___|___|___|___|___.
//                                ^   ^   ^   ^   ^
// 'interval':.___.               |   |   |   |   |
// 'schedule':                   [d0, d1, d2, d3, d4]
//..
// The schedule generated in the above diagram is '[d0, d1, d2, d3, d4]'.
// Notice that the 'example' date does not have to reside within the closed
// interval (the 'example' date is before the 'earliest' date in this diagram).
//
// More formally, the resulting 'schedule' is the subset of the infinite series
// of dates, defined by all dates separated by an integral multiple of
// intervals from the 'example' date, that reside within the closed-interval
// specified by 'earliest' and 'latest'.
//
// The following section provides a synopsis of the main functions provided in
// this component:
//..
//  'generateFromDayInterval'               Generate a schedule having an
//                                          interval of a fixed number of days.
//
//  'generateFromDayOfMonth'                Generate a schedule having an
//                                          interval of a fixed number of
//                                          months, with each date in the
//                                          schedule occuring on a specific day
//                                          of the month.
//
//  'generateFromBusinessDayOfMonth'        Generate a schedule having an
//                                          interval of a fixed number of
//                                          months, with each date in the
//                                          schedule occuring on a specific
//                                          business day of the month.
//
//  'generateFromDayOfWeekAfterDayOfMonth'  Generate a schedule having an
//                                          interval of a fixed number of
//                                          months, with each date in the
//                                          schedule occuring on a specific day
//                                          of the week on or after a specific
//                                          day of the month.
//
//  'generateFromDayOfWeekInMonth'          Generate a schedule having an
//                                          interval of a fixed number of
//                                          months, with each date in the
//                                          schedule occuring on a specific day
//                                          of the week in a specific week of
//                                          the month.
//..
//
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
//  bdlt::Date earliest(2012, 2,  1);
//  bdlt::Date   latest(2015, 2, 28);
//  bdlt::Date  example(2007, 7, 23);
//
//  bsl::vector<bdlt::Date> schedule;
//..
// Now, we invoke the 'generateFromDayOfMonth' routine to obtain the subset of
// dates:
//..
//  bblb::ScheduleGenerationUtil::generateFromDayOfMonth(
//                                                  &schedule,
//                                                  earliest,
//                                                  latest,
//                                                  example.year(),
//                                                  example.month(),
//                                                  9,    // 'intervalInMonths'
//                                                  23);  // 'targetDayOfMonth'
//..
// Finally, we assert that the generated schedule is what we expect:
//..
//  assert(4 == schedule.size());
//  assert(bdlt::Date(2012, 10, 23) == schedule[0]);
//  assert(bdlt::Date(2013,  7, 23) == schedule[1]);
//  assert(bdlt::Date(2014,  4, 23) == schedule[2]);
//  assert(bdlt::Date(2015,  1, 23) == schedule[3]);
//..

#ifndef INCLUDED_BBLSCM_VERSION
#include <bblscm_version.h>
#endif

#ifndef INCLUDED_BDLT_CALENDAR
#include <bdlt_calendar.h>
#endif

#ifndef INCLUDED_BDLT_DATE
#include <bdlt_date.h>
#endif

#ifndef INCLUDED_BDLT_DAYOFWEEK
#include <bdlt_dayofweek.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace bblb {

                      // =============================
                      // struct ScheduleGenerationUtil
                      // =============================

struct ScheduleGenerationUtil {
    // This 'struct' provides a namespace for utility functions that generate
    // schedules.

    // CLASS METHODS
    static void generateFromDayInterval(
                                      bsl::vector<bdlt::Date> *schedule,
                                      const bdlt::Date&        earliest,
                                      const bdlt::Date&        latest,
                                      const bdlt::Date&        example,
                                      int                      intervalInDays);
        // Load, into the specified 'schedule', the chronologically increasing
        // sequence of unique dates that are integral multiples of the
        // specified 'intervalInDays' away from the specified 'example' date,
        // and within the specified closed-interval '[earliest, latest]'.  The
        // behavior is undefined unless 'earliest <= latest' and
        // '1 <= intervalInDays'.

    static void generateFromDayOfMonth(
                                  bsl::vector<bdlt::Date> *schedule,
                                  const bdlt::Date&        earliest,
                                  const bdlt::Date&        latest,
                                  int                      exampleYear,
                                  int                      exampleMonth,
                                  int                      intervalInMonths,
                                  int                      targetDayOfMonth,
                                  int                      targetDayOfFeb = 0);
        // Load, into the specified 'schedule', the chronologically increasing
        // sequence of unique dates that are on the specified
        // 'targetDayOfMonth' (or the last day of the month if
        // 'targetDayOfMonth' would be past the end of the month), integral
        // multiples of the specified 'intervalInMonths' away from the
        // specified 'exampleYear' and 'exampleMonth', and within the specified
        // closed-interval '[earliest, latest]'.  Optionally specify
        // 'targetDayOfFeb' to replace 'targetDayOfMonth' whenever the month of
        // a 'schedule' entry is February.  The behavior is undefined unless
        // 'earliest <= latest', '1 <= exampleYear <= 9999',
        // '1 <= exampleMonth <= 12', '1 <= intervalInMonths',
        // '1 <= targetDayOfMonth <= 31', and '0 <= targetDayOfFeb <= 29'.

    static void generateFromBusinessDayOfMonth(
                            bsl::vector<bdlt::Date> *schedule,
                            const bdlt::Date&        earliest,
                            const bdlt::Date&        latest,
                            int                      exampleYear,
                            int                      exampleMonth,
                            int                      intervalInMonths,
                            const bdlt::Calendar&    calendar,
                            int                      targetBusinessDayOfMonth);
        // Load, into the specified 'schedule', the chronologically increasing
        // sequence of unique dates that are on the specified
        // 'targetBusinessDayOfMonth' (or the highest count possible in the
        // resulting month), integral multiples of the specified
        // 'intervalInMonths' away from the specified 'exampleYear' and
        // 'exampleMonth', and within the specified closed-interval
        // '[earliest, latest]'.  Business days, as per the specified
        // 'calendar', are counted, if 'targetBusinessDayOfMonth' is positive,
        // from and including the chronologically earliest business day to
        // chronologically later business days, and if
        // 'targetBusinessDayOfMonth' is negative, from and including the
        // chronologically latest business day to chronologically earlier
        // business days.  If any of the months required for the schedule do
        // not have a business day, return an empty 'schedule'.  The behavior
        // is undefined unless 'earliest <= latest',
        // '1 <= exampleYear <= 9999', '1 <= exampleMonth <= 12',
        // '1 <= intervalInMonths', and
        // '1 <= abs(targetBusinessDayOfMonth) <= 31'.

    static void generateFromDayOfWeekAfterDayOfMonth(
                                     bsl::vector<bdlt::Date> *schedule,
                                     const bdlt::Date&        earliest,
                                     const bdlt::Date&        latest,
                                     int                      exampleYear,
                                     int                      exampleMonth,
                                     int                      intervalInMonths,
                                     bdlt::DayOfWeek::Enum    dayOfWeek,
                                     int                      dayOfMonth);
        // Load, into the specified 'schedule', the chronologically increasing
        // sequence of unique dates that are on the specified 'dayOfWeek' on or
        // after the specified 'dayOfMonth', integral multiples of the
        // specified 'intervalInMonths' away from the specified 'exampleYear'
        // and 'exampleMonth', and within the specified closed-interval
        // '[earliest, latest]'.  If any of the months required for the
        // schedule have fewer than 'dayOfMonth' days, return an empty
        // 'schedule'.  The behavior is undefined unless 'earliest <= latest',
        // '1 <= exampleYear <= 9999', '1 <= exampleMonth <= 12',
        // '1 <= intervalInMonths', and '1 <= dayOfMonth <= 31'.

    static void generateFromDayOfWeekInMonth(
                                     bsl::vector<bdlt::Date> *schedule,
                                     const bdlt::Date&        earliest,
                                     const bdlt::Date&        latest,
                                     int                      exampleYear,
                                     int                      exampleMonth,
                                     int                      intervalInMonths,
                                     bdlt::DayOfWeek::Enum    dayOfWeek,
                                     int                      occurrenceWeek);
        // Load, into the specified 'schedule', the chronologically increasing
        // sequence of unique dates that are on the specified 'dayOfWeek' of
        // the specified 'occurrenceWeek' of the month, integral multiples of
        // the specified 'intervalInMonths' away from the specified
        // 'exampleYear' and 'exampleMonth', and within the specified
        // closed-interval '[earliest, latest]'.  The behavior is undefined
        // unless 'earliest <= latest', '1 <= exampleYear <= 9999',
        // '1 <= exampleMonth <= 12', '1 <= intervalInMonths', and
        // '1 <= occurrenceWeek <= 4'.
};

}  // close package namespace
}  // close enterprise namespace

#endif

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
