// bbldc_terminatedisda30360eom.cpp                                   -*-C++-*-
#include <bbldc_terminatedisda30360eom.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bbldc_terminatedisda30360eom_cpp,"$Id$ $CSID$")

#include <bdlt_date.h>
#include <bdlt_serialdateimputil.h>

namespace BloombergLP {
namespace bbldc {

                      // -----------------------------
                      // struct TerminatedIsda30360Eom
                      // -----------------------------

// CLASS METHODS
int TerminatedIsda30360Eom::daysDiff(const bdlt::Date& beginDate,
                                     const bdlt::Date& endDate,
                                     const bdlt::Date& terminationDate)
{
    int y1, m1, d1, y2, m2, d2;

    bool negationFlag;

    if (beginDate <= endDate) {
        beginDate.getYearMonthDay(&y1, &m1, &d1);
        endDate.getYearMonthDay(  &y2, &m2, &d2);
        negationFlag = false;
    }
    else {
        endDate.getYearMonthDay(  &y1, &m1, &d1);
        beginDate.getYearMonthDay(&y2, &m2, &d2);
        negationFlag = true;
    }

    if (d1 == bdlt::SerialDateImpUtil::lastDayOfMonth(y1, m1)) {
        d1 = 30;
    }
    if (   d2 == bdlt::SerialDateImpUtil::lastDayOfMonth(y2, m2)
        && (   m2 != 2
            || terminationDate != (negationFlag ? beginDate : endDate))) {
        d2 = 30;
    }

    int result = (y2 - y1) * 360 + (m2 - m1) * 30 + d2 - d1;

    if (negationFlag) {
        result = -result;
    }

    return result;
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
