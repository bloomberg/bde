// bdlt_calendarutil.cpp                                              -*-C++-*-
#include <bdlt_calendarutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_calendarutil_cpp,"$Id$ $CSID$")

#include <bdlt_date.h>
#include <bdlt_serialdateimputil.h>

namespace BloombergLP {
namespace bdlt {

                           // ===================
                           // struct CalendarUtil
                           // ===================

int CalendarUtil::addBusinessDaysIfValid(
                                        bdlt::Date            *result,
                                        const bdlt::Date&      original,
                                        const bdlt::Calendar&  calendar,
                                        int                    numBusinessDays)
{
    BSLS_ASSERT(result);

    enum { e_SUCCESS = 0, e_OUT_OF_RANGE = 1 };

    if (!calendar.isInRange(original)) {
        return e_OUT_OF_RANGE;                                        // RETURN
    }

    unsigned int absNumBusDays = numBusinessDays >= 0
                               ? numBusinessDays
                               : -numBusinessDays;

    // declare forward and reverse iterators

    unsigned int count = calendar.isBusinessDay(original) ? 0 : 1;

    if (numBusinessDays < 0) {
        // an iterator to the business day on or before original

        bdlt::Calendar::BusinessDayConstReverseIterator rit =
                                         calendar.rbeginBusinessDays(original);

        while (rit != calendar.rendBusinessDays() && count < absNumBusDays) {
            ++rit;
            ++count;
        }

        if (rit == calendar.rendBusinessDays()) {
            return e_OUT_OF_RANGE;                                    // RETURN
        }

        *result = *rit;
    }
    else {
        // an iterator to the business day on or after 'original'

        bdlt::Calendar::BusinessDayConstIterator fit =
                                          calendar.beginBusinessDays(original);

        while (fit != calendar.endBusinessDays() && count < absNumBusDays) {
            ++fit;
            ++count;
        }

        if (fit == calendar.endBusinessDays()) {
            return e_OUT_OF_RANGE;                                    // RETURN
        }

        *result = *fit;
    }

    return e_SUCCESS;
}

int CalendarUtil::nthBusinessDayOfMonthOrMaxIfValid(
                                               bdlt::Date            *result,
                                               const bdlt::Calendar&  calendar,
                                               int                    year,
                                               int                    month,
                                               int                    n)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(year  >= 1 && year  <= 9999);
    BSLS_ASSERT(month >= 1 && month <= 12);
    BSLS_ASSERT(n != 0);

    enum { e_SUCCESS = 0, e_OUT_OF_RANGE = 1, e_NOT_FOUND = 2 };

    bdlt::Date monthStart(year, month, 1);

    bdlt::Date monthEnd(year,
                        month,
                        bdlt::SerialDateImpUtil::lastDayOfMonth(year, month));

    if (!(calendar.isInRange(monthStart) && calendar.isInRange(monthEnd))) {
        return e_OUT_OF_RANGE;                                        // RETURN
    }

    bdlt::Date countStart = (n > 0 ? monthStart : monthEnd);

    if (n < 0) {
        // Create an iterator to the business day on or before 'countStart'.

        bdlt::Calendar::BusinessDayConstReverseIterator rit =
                                       calendar.rbeginBusinessDays(countStart);
        ++n;

        // The 'calendar' must have at least one business day in the specified
        // month and the whole month must be in the valid range if the
        // 'calendar'.

        if (rit == calendar.rendBusinessDays() || *rit < monthStart) {
            return e_NOT_FOUND;                                       // RETURN
        }

        while (n < 0) {
            ++rit;

            if (rit == calendar.rendBusinessDays() || *rit < monthStart ) {
                --rit;
                break;
            }

            ++n;
        }

        *result = *rit;
    }
    else {
        // Create an iterator to the business day on or after 'countStart'.

        bdlt::Calendar::BusinessDayConstIterator fit =
                                        calendar.beginBusinessDays(countStart);
        --n;

        // The 'calendar' must have at least one business day in the specified
        // month and the whole month must be in the valid range if the
        // 'calendar'.

        if (fit == calendar.endBusinessDays() || *fit > monthEnd) {
            return e_NOT_FOUND;                                       // RETURN
        }

        while (n > 0) {
            ++fit;

            if (fit == calendar.endBusinessDays() || *fit > monthEnd) {
                --fit;
                break;
            }

            --n;
        }

        *result = *fit;
    }

    return e_SUCCESS;
}

int CalendarUtil::shiftIfValid(bdlt::Date            *result,
                               const bdlt::Date&      original,
                               const bdlt::Calendar&  calendar,
                               ShiftConvention        convention)
{
    BSLS_ASSERT(result);

    enum { e_BAD_CONVENTION = -1, e_SUCCESS = 0 };

    switch (convention) { // switch on shift convention
      case e_FOLLOWING: {
        return shiftFollowingIfValid(result, original, calendar);     // RETURN
      } break;
      case e_PRECEDING : {
        return shiftPrecedingIfValid(result, original, calendar);     // RETURN
      } break;
      case e_MODIFIED_FOLLOWING : {
        return shiftModifiedFollowingIfValid(result,
                                             original,
                                             calendar);               // RETURN
      } break;
      case e_MODIFIED_PRECEDING :{
        return shiftModifiedPrecedingIfValid(result,
                                             original,
                                             calendar);               // RETURN
      } break;
      case e_UNADJUSTED : {
        *result = original;
        return e_SUCCESS;                                             // RETURN
      } break;
    }
    return e_BAD_CONVENTION;
}

int CalendarUtil::shiftIfValid(bdlt::Date            *result,
                               const bdlt::Date&      original,
                               const bdlt::Calendar&  calendar,
                               ShiftConvention        convention,
                               bdlt::DayOfWeek::Enum  specialDay,
                               bool                   extendSpecialDay,
                               ShiftConvention        specialConvention)
{
    BSLS_ASSERT(result);

    enum { e_OUT_OF_RANGE = 1 };

    bdlt::Date test(original);

    if (extendSpecialDay) {
        if (!calendar.isInRange(test)) {
            return e_OUT_OF_RANGE;                                    // RETURN
        }
        while (   test.dayOfWeek() != specialDay
               && calendar.isNonBusinessDay(test)) {
            ++test;
            if (!calendar.isInRange(test)) {
                return e_OUT_OF_RANGE;                                // RETURN
            }
        }
    }

    if (test.dayOfWeek() == specialDay) {
        return shiftIfValid(result,
                            original,
                            calendar,
                            specialConvention);                       // RETURN
    }

    return shiftIfValid(result, original, calendar, convention);
}

int CalendarUtil::shiftModifiedFollowingIfValid(
                                               bdlt::Date            *result,
                                               const bdlt::Date&      original,
                                               const bdlt::Calendar&  calendar)
{
    BSLS_ASSERT(result);

    enum {
        e_SUCCESS      = 0,
        e_BAD_INPUT    = 1,
        e_OUT_OF_RANGE = 2,
        e_NOT_FOUND    = 3
    };

    if (!calendar.isInRange(original)) {
        return e_BAD_INPUT;                                           // RETURN
    }

    bdlt::Date date(original);

    int lastDay = bdlt::SerialDateImpUtil::lastDayOfMonth(original.year(),
                                                          original.month());

    while (true) {
        if (calendar.isBusinessDay(date)) {
            *result = date;
            return e_SUCCESS;                                         // RETURN
        }

        if (date.day() == lastDay) {
            break;
        }

        if (date == calendar.lastDate()) {
            return e_OUT_OF_RANGE;                                    // RETURN
        }
        ++date;
    }

    // go in reverse
    date = original;
    while (true) {
        if (date == calendar.firstDate()) {
            return e_OUT_OF_RANGE;                                    // RETURN
        }
        --date;

        if (calendar.isBusinessDay(date)) {
            *result = date;
            return e_SUCCESS;                                         // RETURN
        }
    }

    return e_NOT_FOUND;
}

int CalendarUtil::shiftModifiedPrecedingIfValid(
                                               bdlt::Date            *result,
                                               const bdlt::Date&      original,
                                               const bdlt::Calendar&  calendar)
{
    BSLS_ASSERT(result);

    enum {
        e_SUCCESS      = 0,
        e_BAD_INPUT    = 1,
        e_OUT_OF_RANGE = 2,
        e_NOT_FOUND    = 3
    };

    if (!calendar.isInRange(original)) {
        return e_BAD_INPUT;                                           // RETURN
    }

    bdlt::Date date(original);

    int firstDay = 1;
    while (true) {
        if (calendar.isBusinessDay(date)) {
            *result = date;
            return e_SUCCESS;                                         // RETURN
        }

        if (date.day() == firstDay){
            break;
        }

        if (date == calendar.firstDate()) {
            return e_OUT_OF_RANGE;                                    // RETURN
        }
        --date;
    }

    // go in reverse

    date = original;
    while (true) {
        if (date == calendar.lastDate()){
            return e_OUT_OF_RANGE;                                    // RETURN
        }
        ++date;

        if (calendar.isBusinessDay(date)) {
            *result = date;
            return e_SUCCESS;                                         // RETURN
        }
    }

    return e_NOT_FOUND;
}

int CalendarUtil::subtractBusinessDaysIfValid(
                                        bdlt::Date            *result,
                                        const bdlt::Date&      original,
                                        const bdlt::Calendar&  calendar,
                                        int                    numBusinessDays)
{
    BSLS_ASSERT(result);

    enum { e_SUCCESS = 0, e_OUT_OF_RANGE = 1 };

    if (!calendar.isInRange(original)) {
        return e_OUT_OF_RANGE;                                        // RETURN
    }

    unsigned int absNumBusDays = numBusinessDays >= 0
                               ? numBusinessDays
                               : -numBusinessDays;

    // declare forward and reverse iterators

    unsigned int count = calendar.isBusinessDay(original) ? 0 : 1;

    if (numBusinessDays >= 0) {
        // an iterator to the business day on or before original

        bdlt::Calendar::BusinessDayConstReverseIterator rit =
                                         calendar.rbeginBusinessDays(original);

        while (rit != calendar.rendBusinessDays() && count < absNumBusDays) {
            ++rit;
            ++count;
        }

        if (rit == calendar.rendBusinessDays()) {
            return e_OUT_OF_RANGE;                                    // RETURN
        }

        *result = *rit;
    }
    else {
        // an iterator to the business day on or after 'original'

        bdlt::Calendar::BusinessDayConstIterator fit =
                                          calendar.beginBusinessDays(original);

        while (fit != calendar.endBusinessDays() && count < absNumBusDays) {
            ++fit;
            ++count;
        }

        if (fit == calendar.endBusinessDays()) {
            return e_OUT_OF_RANGE;                                    // RETURN
        }

        *result = *fit;
    }

    return e_SUCCESS;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
