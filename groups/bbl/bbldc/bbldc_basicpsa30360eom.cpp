// bbldc_basicpsa30360eom.cpp                                         -*-C++-*-
#include <bbldc_basicpsa30360eom.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bbldc_basicpsa30360eom_cpp,"$Id$ $CSID$")

#include <bdlt_date.h>
#include <bdlt_serialdateimputil.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace bbldc {

// STATIC METHODS
inline
static bool isLastDayOfFebruary(int year, int month, int day)
    // Return 'true' if the specified 'day' of the specified 'month' in the
    // specified 'year' is the last day of February for that 'year', and
    // 'false' otherwise.  The behavior is undefined unless 'year', 'month',
    // and 'day' represent a valid 'bdlt::Date' value.
{
    BSLS_ASSERT_SAFE(bdlt::SerialDateImpUtil::
                                        isValidYearMonthDay(year, month, day));

    return 2 == month
        && (   29 == day
            || (28 == day && !bdlt::SerialDateImpUtil::isLeapYear(year)));
}

inline
static int max(int lhs, int rhs)
    // Return the maximum of the specified 'lhs' and 'rhs' values.
{
    return lhs > rhs ? lhs : rhs;
}

static int computeDaysDiff(const bdlt::Date& beginDate,
                           const bdlt::Date& endDate)
    // Return the number of days between the specified 'beginDate' and
    // 'endDate' according to the PSA 30/360 end-of-month day-count convention.
    // If 'beginDate <= endDate', then the result is non-negative.  Note that
    // reversing the order of 'beginDate' and 'endDate' negates the result.
{
    int y1, m1, d1, y2, m2, d2;
    int negationFlag = beginDate > endDate;
    if (negationFlag) {
        endDate.getYearMonthDay(  &y1, &m1, &d1);
        beginDate.getYearMonthDay(&y2, &m2, &d2);
    } else {
        beginDate.getYearMonthDay(&y1, &m1, &d1);
        endDate.getYearMonthDay(  &y2, &m2, &d2);
    }

    // This implementation is coded to look exactly like the description as it
    // appears in the PSA document:

    if (isLastDayOfFebruary(y1, m1, d1)) {
        d1 = 30;
    }
    if (31 == d1) {
        d1 = 30;
    }

    if (30 == d1 && 31 == d2) {
        d2 = 30;
    }

    int result = max((y2 - y1) * 360 + (m2 - m1) * 30 + d2 - d1, 0);

    if (negationFlag) {
        result = -result;
    }

    return result;
}

                            // ------------------
                            // struct Psa30360Eom
                            // ------------------

// CLASS METHODS
int BasicPsa30360Eom::daysDiff(const bdlt::Date& beginDate,
                               const bdlt::Date& endDate)
{
    return computeDaysDiff(beginDate, endDate);
}

double BasicPsa30360Eom::yearsDiff(const bdlt::Date& beginDate,
                                   const bdlt::Date& endDate)
{
    return static_cast<double>(computeDaysDiff(beginDate, endDate)) / 360.0;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
