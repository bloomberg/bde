// bbldcu_isma30360.cpp                                               -*-C++-*-
#include <bbldcu_isma30360.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bbldcu_isma30360_cpp,"$Id$ $CSID$")

#include <bdlt_date.h>

namespace BloombergLP {
namespace bbldcu {

// STATIC METHODS
static int computeDaysDiff(const bdlt::Date& beginDate,
                           const bdlt::Date& endDate)
    // Return the number of days between the specified  'beginDate' and
    // 'endDate' according to the ISMA 30/360 convention.  If
    // 'beginDate <= endDate', then the result is non-negative.  Note that
    // reversing the order of 'beginDate' and 'endDate' negates the result.
{
    int yBegin, mBegin, dBegin, yEnd, mEnd, dEnd;

    beginDate.getYearMonthDay(&yBegin, &mBegin, &dBegin);
    endDate.getYearMonthDay(&yEnd, &mEnd, &dEnd);

    if (31 == dBegin) {
        dBegin = 30;
    }

    if (31 == dEnd) {
        dEnd = 30;
    }

    return (yEnd - yBegin) * 360 + (mEnd - mBegin) * 30 + dEnd - dBegin;
}

                           // ----------------
                           // struct Isma30360
                           // ----------------

// CLASS METHODS
int Isma30360::daysDiff(const bdlt::Date& beginDate,
                        const bdlt::Date& endDate)
{
    return computeDaysDiff(beginDate, endDate);
}

double Isma30360::yearsDiff(const bdlt::Date& beginDate,
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
