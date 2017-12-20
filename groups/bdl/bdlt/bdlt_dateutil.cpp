// bdlt_dateutil.cpp                                                  -*-C++-*-
#include <bdlt_dateutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_dateutil_cpp,"$Id$ $CSID$")

namespace BloombergLP {
namespace bdlt {
namespace {

int dayOfWeekDifference(DayOfWeek::Enum day1, DayOfWeek::Enum day2)
    // Return the difference in number of days from the specified 'day1' to the
    // specified 'day2'.
{
    if (day1 > day2) {
        return 7 - day1 + day2;                                       // RETURN
    }
    else {
        return day2 - day1;                                           // RETURN
    }
}

}  // close unnamed namespace

                             // ---------------
                             // struct DateUtil
                             // ---------------

// PRIVATE CLASS METHODS
Date DateUtil::addYearsEomEndOfFebruary(const Date& original, int numYears)
{
    // Implementation note: The complete 'addYearsEom' was too long to be
    // implemented as an 'inline' function.  However, performance testing
    // showed that this factoring of 'addYearsEom', where the trivial and
    // common scenario is implemented inline, and the special case at the end
    // of February is implemented out-of-line (in the .cpp) file, was 18%
    // faster than a fully out-of-line implementation.  This test was compiled
    // using gcc-4.8.2, with optimization level -O2, on a machine with an
    // Intel(R) Xeon(R) CPU X5670 @ 2.93GHz processor.

    BSLS_ASSERT_SAFE(2 == original.month());
    BSLS_ASSERT_SAFE(28 == original.day() || 29 == original.day());

    const int newYear = original.year() + numYears;
    const int eom     = SerialDateImpUtil::isLeapYear(original.year())
                        ? 29
                        : 28;

    if (original.day() == eom) {
        const int newEom = SerialDateImpUtil::isLeapYear(newYear) ? 29 : 28;
        return Date(newYear, original.month(), newEom);               // RETURN
    }

    // This function can only be called for Feb 28 & 29, so if the preceding
    // 'if' condition was 'false', then 'original.day()' must be Feb 28 (in a
    // leap year).

    return Date(newYear, original.month(), original.day());
}

// CLASS METHODS
Date DateUtil::addMonthsEom(const Date& original, int numMonths)
{
    const int totalMonths = original.year() * 12 + original.month() +
                                                                 numMonths - 1;

    const int newYear  = totalMonths / 12;
    const int newMonth = totalMonths % 12 + 1;

    BSLS_ASSERT_SAFE(1 <= newYear);
    BSLS_ASSERT_SAFE(newYear <= 9999);

    const int eom    = SerialDateImpUtil::lastDayOfMonth(original.year(),
                                                         original.month());
    const int newEom = SerialDateImpUtil::lastDayOfMonth(newYear,
                                                         newMonth);

    if (original.day() == eom) {
        return Date(newYear, newMonth, newEom);                       // RETURN
    }
    else {
        if (newEom < original.day()) {
            return Date(newYear, newMonth, newEom);                   // RETURN
        }
        else {
            return Date(newYear, newMonth, original.day());           // RETURN
        }
    }
}

Date DateUtil::addMonthsNoEom(const Date& original, int numMonths)
{
    const int totalMonths = original.year() * 12 + original.month()
                                                               + numMonths - 1;

    const int newYear  = totalMonths / 12;
    const int newMonth = totalMonths % 12 + 1;

    BSLS_ASSERT_SAFE(1 <= newYear);
    BSLS_ASSERT_SAFE(newYear <= 9999);

    const int newEom = SerialDateImpUtil::lastDayOfMonth(newYear, newMonth);

    if (newEom < original.day()) {
        return Date(newYear, newMonth, newEom);                       // RETURN
    }
    else {
        return Date(newYear, newMonth, original.day());               // RETURN
    }
}

Date DateUtil::lastDayOfWeekInMonth(int             year,
                                    int             month,
                                    DayOfWeek::Enum dayOfWeek)
{
    BSLS_ASSERT_SAFE(1 <= year);   BSLS_ASSERT_SAFE(year  <= 9999);
    BSLS_ASSERT_SAFE(1 <= month);  BSLS_ASSERT_SAFE(month <= 12);

    const int eom = SerialDateImpUtil::lastDayOfMonth(year, month);

    return previousDayOfWeekInclusive(dayOfWeek, Date(year, month, eom));
}

Date DateUtil::nextDayOfWeek(DayOfWeek::Enum dayOfWeek, const Date& date)
{
    const Date d = date + 1;

    return d + dayOfWeekDifference(d.dayOfWeek(), dayOfWeek);
}

Date DateUtil::nextDayOfWeekInclusive(DayOfWeek::Enum dayOfWeek,
                                      const Date&     date)
{
    return date + dayOfWeekDifference(date.dayOfWeek(), dayOfWeek);
}

Date DateUtil::nthDayOfWeekInMonth(int             year,
                                   int             month,
                                   DayOfWeek::Enum dayOfWeek,
                                   int             n)
{
    BSLS_ASSERT_SAFE( 1 <= year);   BSLS_ASSERT_SAFE(year  <= 9999);
    BSLS_ASSERT_SAFE( 1 <= month);  BSLS_ASSERT_SAFE(month <= 12);
    BSLS_ASSERT_SAFE( 0 != n);
    BSLS_ASSERT_SAFE(-5 <= n);      BSLS_ASSERT_SAFE(n     <= 5);

    if (n > 0) {
        const Date date = nextDayOfWeekInclusive(dayOfWeek,
                                                 Date(year, month, 1));

        return date + 7 * (n - 1);                                    // RETURN
    }

    const int  eom = SerialDateImpUtil::lastDayOfMonth(year, month);
    const Date date = previousDayOfWeekInclusive(dayOfWeek,
                                                 Date(year, month, eom));

    return date + 7 * (n + 1);
}

Date DateUtil::previousDayOfWeek(DayOfWeek::Enum dayOfWeek, const Date& date)
{
    const Date d = date - 1;

    return d - dayOfWeekDifference(dayOfWeek, d.dayOfWeek());
}

Date DateUtil::previousDayOfWeekInclusive(DayOfWeek::Enum dayOfWeek,
                                          const Date&     date)
{
    return date - dayOfWeekDifference(dayOfWeek, date.dayOfWeek());
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
