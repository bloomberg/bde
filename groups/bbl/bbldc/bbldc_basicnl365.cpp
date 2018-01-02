// bbldc_basicnl365.cpp                                               -*-C++-*-
#include <bbldc_basicnl365.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bbldc_basicnl365_cpp,"$Id$ $CSID$")

namespace BloombergLP {
namespace bbldc {

                            // -----------------
                            // struct BasicNl365
                            // -----------------

// CLASS METHODS
int BasicNl365::daysDiff(const bdlt::Date& beginDate,
                         const bdlt::Date& endDate)
{
    static int s_daysInMonthCorrection[13] =
                                     { 0, 0, 0, 3, 3, 4, 4, 5, 5, 5, 6, 6, 7 };

    int y1, m1, d1, y2, m2, d2;

    beginDate.getYearMonthDay(&y1, &m1, &d1);
    endDate.getYearMonthDay(  &y2, &m2, &d2);

    return (y2 - y1) * 365
         + (m2 - m1) * 31
         - (s_daysInMonthCorrection[m2] - s_daysInMonthCorrection[m1])
         +  d2 - d1;
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
