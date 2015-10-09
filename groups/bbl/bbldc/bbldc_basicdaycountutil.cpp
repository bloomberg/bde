// bbldc_basicdaycountutil.cpp                                        -*-C++-*-
#include <bbldc_basicdaycountutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bbldc_basicdaycountutil_cpp,"$Id$ $CSID$")

#include <bbldc_basicactual360.h>
#include <bbldc_basicactual365fixed.h>
#include <bbldc_basicisdaactualactual.h>
#include <bbldc_basicisma30360.h>
#include <bbldc_basicpsa30360eom.h>
#include <bbldc_basicsia30360eom.h>
#include <bbldc_basicsia30360neom.h>

#include <bdlt_date.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace bbldc {

                         // ------------------------
                         // struct BasicDayCountUtil
                         // ------------------------

// CLASS METHODS
int BasicDayCountUtil::daysDiff(const bdlt::Date&        beginDate,
                                const bdlt::Date&        endDate,
                                DayCountConvention::Enum convention)
{
    int numDays;

    switch (convention) {
      case DayCountConvention::e_ACTUAL_360: {
        numDays = bbldc::BasicActual360::daysDiff(beginDate, endDate);
      } break;
      case DayCountConvention::e_ACTUAL_365_FIXED: {
        numDays = bbldc::BasicActual365Fixed::daysDiff(beginDate, endDate);
      } break;
      case DayCountConvention::e_ISDA_ACTUAL_ACTUAL: {
        numDays = bbldc::BasicIsdaActualActual::daysDiff(beginDate, endDate);
      } break;
      case DayCountConvention::e_ISMA_30_360: {
        numDays = bbldc::BasicIsma30360::daysDiff(beginDate, endDate);
      } break;
      case DayCountConvention::e_PSA_30_360_EOM: {
        numDays = bbldc::BasicPsa30360Eom::daysDiff(beginDate, endDate);
      } break;
      case DayCountConvention::e_SIA_30_360_EOM: {
        numDays = bbldc::BasicSia30360Eom::daysDiff(beginDate, endDate);
      } break;
      case DayCountConvention::e_SIA_30_360_NEOM: {
        numDays = bbldc::BasicSia30360Neom::daysDiff(beginDate, endDate);
      } break;
      default: {
        BSLS_ASSERT_OPT(0 && "Unrecognized convention");
        numDays = 0;
      } break;
    }

    return numDays;
}

bool BasicDayCountUtil::isSupported(DayCountConvention::Enum convention)
{
    bool rv = true;

    switch (convention) {
      case DayCountConvention::e_ACTUAL_360:
      case DayCountConvention::e_ACTUAL_365_FIXED:
      case DayCountConvention::e_ISDA_ACTUAL_ACTUAL:
      case DayCountConvention::e_ISMA_30_360:
      case DayCountConvention::e_PSA_30_360_EOM:
      case DayCountConvention::e_SIA_30_360_EOM:
      case DayCountConvention::e_SIA_30_360_NEOM: {
      } break;
      default: {
        rv = false;
      } break;
    }

    return rv;
}

double BasicDayCountUtil::yearsDiff(const bdlt::Date&        beginDate,
                                    const bdlt::Date&        endDate,
                                    DayCountConvention::Enum convention)
{
    double numYears;

    switch (convention) {
      case DayCountConvention::e_ACTUAL_360: {
        numYears = bbldc::BasicActual360::yearsDiff(beginDate, endDate);
      } break;
      case DayCountConvention::e_ACTUAL_365_FIXED: {
        numYears = bbldc::BasicActual365Fixed::yearsDiff(beginDate, endDate);
      } break;
      case DayCountConvention::e_ISDA_ACTUAL_ACTUAL: {
        numYears = bbldc::BasicIsdaActualActual::yearsDiff(beginDate, endDate);
      } break;
      case DayCountConvention::e_ISMA_30_360: {
        numYears = bbldc::BasicIsma30360::yearsDiff(beginDate, endDate);
      } break;
      case DayCountConvention::e_PSA_30_360_EOM: {
        numYears = bbldc::BasicPsa30360Eom::yearsDiff(beginDate, endDate);
      } break;
      case DayCountConvention::e_SIA_30_360_EOM: {
        numYears = bbldc::BasicSia30360Eom::yearsDiff(beginDate, endDate);
      } break;
      case DayCountConvention::e_SIA_30_360_NEOM: {
        numYears = bbldc::BasicSia30360Neom::yearsDiff(beginDate, endDate);
      } break;
      default: {
        BSLS_ASSERT_OPT(0 && "Unrecognized convention");
        numYears = 0.0;
      } break;
    }

    return numYears;
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
