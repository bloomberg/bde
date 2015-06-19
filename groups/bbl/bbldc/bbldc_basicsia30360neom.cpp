// bbldc_basicsia30360neom.cpp                                        -*-C++-*-
#include <bbldc_basicsia30360neom.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bbldc_basicsia30360neom_cpp,"$Id$ $CSID$")

#include <bdlt_date.h>

namespace BloombergLP {
namespace bbldc {

// STATIC METHODS
inline
static int computeDaysDiff(const bdlt::Date& beginDate,
                           const bdlt::Date& endDate)
    // Return the number of days between the specified 'beginDate' and
    // 'endDate' according to the SIA 30/360 no-end-of-month day-count
    // convention.  If 'beginDate <= endDate', then the result is non-negative.
    // Note that reversing the order of 'beginDate' and 'endDate' negates the
    // result.
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

    if (31 == d1) {
        d1 = 30;
    }

    if (30 == d1 && 31 == d2) {
        d2 = 30;
    }

    int result = (y2 - y1) * 360 + (m2 - m1) * 30 + d2 - d1;

    if (negationFlag) {
        result = -result;
    }

    return result;
}

                         // ------------------------
                         // struct BasicSia30360Neom
                         // ------------------------

// CLASS METHODS
int BasicSia30360Neom::daysDiff(const bdlt::Date& beginDate,
                                const bdlt::Date& endDate)
{
    return computeDaysDiff(beginDate, endDate);
}

double BasicSia30360Neom::yearsDiff(const bdlt::Date& beginDate,
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
