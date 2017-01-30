// bbldc_basicisdaactualactual.cpp                                    -*-C++-*-
#include <bbldc_basicisdaactualactual.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bbldc_basicisdaactualactual_cpp,"$Id$ $CSID$")

#include <bdlt_serialdateimputil.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace bbldc {

                       // ----------------------------
                       // struct BasicIsdaActualActual
                       // ----------------------------

// CLASS METHODS
double BasicIsdaActualActual::yearsDiff(const bdlt::Date& beginDate,
                                        const bdlt::Date& endDate)
{
    const bool negationFlag = beginDate > endDate;

    const bdlt::Date minDate = negationFlag ? endDate   : beginDate;
    const bdlt::Date maxDate = negationFlag ? beginDate : endDate;

    const int minYear = minDate.year();
    const int maxYear = maxDate.year();

    const int daysInMinYear =
                            365 + bdlt::SerialDateImpUtil::isLeapYear(minYear);
    const int daysInMaxYear =
                            365 + bdlt::SerialDateImpUtil::isLeapYear(maxYear);

    const int yDiff = maxYear - minYear - 1;
    const int minYearDayDiff = bdlt::Date(minYear + 1, 1, 1) - minDate;
    const int maxYearDayDiff = maxDate - bdlt::Date(maxYear, 1, 1);
    double    result = (yDiff * daysInMinYear * daysInMaxYear
    		         + minYearDayDiff * daysInMaxYear
					 + maxYearDayDiff * daysInMinYear)
					 / static_cast<double>(daysInMinYear * daysInMaxYear);
    if (negationFlag) {
        result = -result;
    }
    return result;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
