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
    const int beginYear = beginDate.year();
    const int endYear   = endDate.year();

    const double daysInBeginYear =
                        365.0 + bdlt::SerialDateImpUtil::isLeapYear(beginYear);
    const double daysInEndYear   =
                        365.0 + bdlt::SerialDateImpUtil::isLeapYear(endYear);

    return static_cast<double>(endYear - beginYear - 1)
         + static_cast<double>(bdlt::Date(beginYear + 1, 1, 1) - beginDate)
                                                              / daysInBeginYear
         + static_cast<double>(endDate - bdlt::Date(endYear, 1, 1))
                                                               / daysInEndYear;
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
