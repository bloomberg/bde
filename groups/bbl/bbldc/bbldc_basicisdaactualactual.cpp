// bbldc_basicisdaactualactual.cpp                                    -*-C++-*-
#include <bbldc_basicisdaactualactual.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bbldc_basicisdaactualactual_cpp,"$Id$ $CSID$")

#include <bdlt_serialdateimputil.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

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

    const int daysInBeginYear =
                          365 + bdlt::SerialDateImpUtil::isLeapYear(beginYear);
    const int daysInEndYear =
                            365 + bdlt::SerialDateImpUtil::isLeapYear(endYear);

    const int yDiff = endYear - beginYear - 1;
    const int beginYearDayDiff = bdlt::Date(beginYear + 1, 1, 1) - beginDate;
    const int endYearDayDiff = endDate - bdlt::Date(endYear, 1, 1);
    const int numerator = yDiff * daysInBeginYear * daysInEndYear
                        + beginYearDayDiff * daysInEndYear
                        + endYearDayDiff * daysInBeginYear;
    const int denominator = daysInBeginYear * daysInEndYear;

#if defined(BSLS_PLATFORM_CMP_GNU) && (BSLS_PLATFORM_CMP_VERSION >= 50301)
    // Storing the result value in a 'volatile double' removes extra-precision
    // available in floating-point registers.

    const volatile double rv =
#else
    const double rv =
#endif
                      numerator / static_cast<double>(denominator);

    return rv;
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
