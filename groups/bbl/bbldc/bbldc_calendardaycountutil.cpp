// bbldc_calendardaycountutil.cpp                                     -*-C++-*-
#include <bbldc_calendardaycountutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bbldc_calendardaycountutil_cpp,"$Id$ $CSID$")

#include <bbldc_calendarbus252.h>

#include <bdlt_calendar.h>
#include <bdlt_date.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace bbldc {

                       // ---------------------------
                       // struct CalendarDayCountUtil
                       // ---------------------------

// CLASS METHODS
int CalendarDayCountUtil::daysDiff(const bdlt::Date&        beginDate,
                                   const bdlt::Date&        endDate,
                                   const bdlt::Calendar&    calendar,
                                   DayCountConvention::Enum convention)
{
    BSLS_ASSERT(calendar.isInRange(beginDate));
    BSLS_ASSERT(calendar.isInRange(endDate));

    int numDays;

    switch (convention) {
      case DayCountConvention::e_CALENDAR_BUS_252: {
        numDays = bbldc::CalendarBus252::daysDiff(beginDate,
                                                  endDate,
                                                  calendar);
      } break;
      default: {
        BSLS_ASSERT_OPT(0 && "Unrecognized convention");
        numDays = 0;
      } break;
    }

    return numDays;
}

bool CalendarDayCountUtil::isSupported(DayCountConvention::Enum convention)
{
    bool rv = true;

    switch (convention) {
      case DayCountConvention::e_CALENDAR_BUS_252: {
      } break;
      default: {
        rv = false;
      } break;
    }

    return rv;
}

double CalendarDayCountUtil::yearsDiff(const bdlt::Date&        beginDate,
                                       const bdlt::Date&        endDate,
                                       const bdlt::Calendar&    calendar,
                                       DayCountConvention::Enum convention)
{
    BSLS_ASSERT(calendar.isInRange(beginDate));
    BSLS_ASSERT(calendar.isInRange(endDate));

    double numYears;

    switch (convention) {
      case DayCountConvention::e_CALENDAR_BUS_252: {
        numYears = bbldc::CalendarBus252::yearsDiff(beginDate,
                                                    endDate,
                                                    calendar);
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
