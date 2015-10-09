// bbldc_periodicmaactualactual.cpp                                   -*-C++-*-
#include <bbldc_periodicmaactualactual.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bbldc_periodicmaactualactual_cpp,"$Id$ $CSID$")

#include <bdlt_serialdateimputil.h>

#include <bsls_assert.h>

#include <bsl_algorithm.h>

namespace BloombergLP {
namespace bbldc {

// STATIC HELPER FUNCTIONS

template <class ITER>
static bool isSortedAndUnique(const ITER& begin, const ITER& end)
    // Return 'true' if all values between the specified 'begin' and 'end'
    // iterators are unique and sorted from minimum to maximum value, and
    // 'false' otherwise.
{
    if (begin == end) {
        return true;                                                  // RETURN
    }

    ITER prev = begin;
    ITER at   = begin + 1;

    while (at != end) {
        if (*prev >= *at) {
            return false;                                             // RETURN
        }
        prev = at++;
    }

    return true;
}

                      // -----------------------------
                      // struct PeriodIcmaActualActual
                      // -----------------------------

// CLASS METHODS
double PeriodIcmaActualActual::yearsDiff(
                                 const bdlt::Date&              beginDate,
                                 const bdlt::Date&              endDate,
                                 const bsl::vector<bdlt::Date>& periodDate,
                                 double                         periodYearDiff)
{
    BSLS_ASSERT(periodDate.size() >= 2);
    BSLS_ASSERT(periodDate.front() <= beginDate);
    BSLS_ASSERT(                      beginDate <= periodDate.back());
    BSLS_ASSERT(periodDate.front() <= endDate);
    BSLS_ASSERT(                      endDate   <= periodDate.back());

    BSLS_ASSERT_SAFE(isSortedAndUnique(periodDate.begin(), periodDate.end()));

    if (beginDate == endDate) {
        return 0.0;                                                   // RETURN
    }

    double result;

    // Compute the negation flag and produce sorted dates.

    bool negationFlag = beginDate > endDate;

    bdlt::Date minDate;
    bdlt::Date maxDate;
    if (false == negationFlag) {
        minDate = beginDate;
        maxDate = endDate;
    }
    else {
        minDate = endDate;
        maxDate = beginDate;
    }

    // Find the period dates bracketing 'minDate'.

    bsl::vector<bdlt::Date>::const_iterator beginIter2 =
               bsl::upper_bound(periodDate.begin(), periodDate.end(), minDate);
    bsl::vector<bdlt::Date>::const_iterator beginIter1 = beginIter2 - 1;

    // Find the period dates bracketing 'maxDate'.

    bsl::vector<bdlt::Date>::const_iterator endIter2 =
               bsl::lower_bound(periodDate.begin(), periodDate.end(), maxDate);
    bsl::vector<bdlt::Date>::const_iterator endIter1 = endIter2 - 1;

    // Compute the fractional number of periods * 'periodYearDiff'.

    result = (  static_cast<double>(*beginIter2 - minDate) /
                                 static_cast<double>(*beginIter2 - *beginIter1)
              + static_cast<double>(endIter1 - beginIter2)
              + static_cast<double>(maxDate - *endIter1) /
                                    static_cast<double>(*endIter2 - *endIter1))
             * periodYearDiff;

    // Negate the value if necessary.

    if (negationFlag) {
        result = -result;
    }

    return result;
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
