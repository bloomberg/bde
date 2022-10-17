// bdlt_calendar.cpp                                                  -*-C++-*-
#include <bdlt_calendar.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_calendar_cpp,"$Id$ $CSID$")

#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdlt {

// The implementation requires that the return value for an unsuccessful
// 'bdlc::BitArray::find*', when cast to an 'int', is -1.
BSLMF_ASSERT(-1 == static_cast<int>(bdlc::BitArray::k_INVALID_INDEX));

                              // --------------
                              // class Calendar
                              // --------------

// PRIVATE MANIPULATORS
void Calendar::synchronizeCache()
{
    const int length = d_packedCalendar.length();
    d_nonBusinessDays.setLength(length);
    if (length) {
        d_nonBusinessDays.assignAll0();
        for (PackedCalendar::HolidayConstIterator it =
                                              d_packedCalendar.beginHolidays();
             it != d_packedCalendar.endHolidays();
             ++it) {
            d_nonBusinessDays.assign1(*it - d_packedCalendar.firstDate());
        }

        // Update 'd_nonBusinessDays' with weekend days information from
        // 'd_packedCalendar'.

        PackedCalendar::WeekendDaysTransitionConstIterator nextTransIt
                              = d_packedCalendar.beginWeekendDaysTransitions();

        while (nextTransIt != d_packedCalendar.endWeekendDaysTransitions()) {
            PackedCalendar::WeekendDaysTransitionConstIterator
                                                  currentTransIt = nextTransIt;

            if (currentTransIt->first > d_packedCalendar.lastDate()) {
                break;
            }

            ++nextTransIt;

            Date lastDate;
            if (nextTransIt == d_packedCalendar.endWeekendDaysTransitions() ||
                nextTransIt->first > d_packedCalendar.lastDate()) {
                lastDate = d_packedCalendar.lastDate();
            }
            else {
                lastDate = nextTransIt->first - 1;
                if (lastDate < d_packedCalendar.firstDate()) {
                    continue;
                }
            }
            Date firstDate =
                           currentTransIt->first < d_packedCalendar.firstDate()
                           ? d_packedCalendar.firstDate()
                           : currentTransIt->first;

            int lastDateIndex  = lastDate  - d_packedCalendar.firstDate();
            int firstDateIndex = firstDate - d_packedCalendar.firstDate();

            int firstDayOfWeek = static_cast<int>(firstDate.dayOfWeek());

            for (DayOfWeekSet::const_iterator wdIt =
                                                currentTransIt->second.begin();
                 wdIt != currentTransIt->second.end();
                 ++wdIt) {

                int weekendDay = static_cast<int>(*wdIt);
                int weekendDateIndex =
                        firstDateIndex + (weekendDay - firstDayOfWeek + 7) % 7;

                while (weekendDateIndex <= lastDateIndex) {
                    d_nonBusinessDays.assign1(weekendDateIndex);
                    weekendDateIndex += 7;
                }
            }
        }
    }
}

// PRIVATE ACCESSORS
bool Calendar::isCacheSynchronized() const
{
    if (d_packedCalendar.length() !=
                                static_cast<int>(d_nonBusinessDays.length())) {
        return false;                                                 // RETURN
    }

    if (0 == d_packedCalendar.length()) {
        return true;                                                  // RETURN
    }

    PackedCalendar::BusinessDayConstIterator iter =
                                          d_packedCalendar.beginBusinessDays();
    PackedCalendar::BusinessDayConstIterator endIter =
                                            d_packedCalendar.endBusinessDays();

    int offset = static_cast<int>(d_nonBusinessDays.find0AtMinIndex(0));
    while (iter != endIter) {
        if (offset != *iter - d_packedCalendar.firstDate()) {
            return false;                                             // RETURN
        }
        ++iter;
        offset = static_cast<int>(
                                d_nonBusinessDays.find0AtMinIndex(offset + 1));
    }

    return 0 > offset;
}

// CREATORS
Calendar::Calendar(bslma::Allocator *basicAllocator)
: d_packedCalendar(basicAllocator)
, d_nonBusinessDays(basicAllocator)
{
}

Calendar::Calendar(const Date&       firstDate,
                   const Date&       lastDate,
                   bslma::Allocator *basicAllocator)
: d_packedCalendar(firstDate, lastDate, basicAllocator)
, d_nonBusinessDays(basicAllocator)
{
    d_nonBusinessDays.setLength(d_packedCalendar.length(), 0);
}

Calendar::Calendar(const PackedCalendar&  packedCalendar,
                   bslma::Allocator      *basicAllocator)
: d_packedCalendar(packedCalendar, basicAllocator)
, d_nonBusinessDays(basicAllocator)
{
    synchronizeCache();
}

Calendar::Calendar(const Calendar& original, bslma::Allocator *basicAllocator)
: d_packedCalendar(original.d_packedCalendar, basicAllocator)
, d_nonBusinessDays(original.d_nonBusinessDays, basicAllocator)
{
}

Calendar::~Calendar()
{
    BSLS_PRECONDITIONS_BEGIN();
    BSLS_ASSERT_SAFE(isCacheSynchronized());
    BSLS_PRECONDITIONS_END();
}

// MANIPULATORS
void Calendar::addDay(const Date& date)
{
    if (0 == length()) {
        setValidRange(date, date);
    }
    else if (date < d_packedCalendar.firstDate()) {
        setValidRange(date, d_packedCalendar.lastDate());
    }
    else if (date > d_packedCalendar.lastDate()) {
        setValidRange(d_packedCalendar.firstDate(), date);
    }
}

void Calendar::addHoliday(const Date& date)
{
    if (0 == length()) {
        d_nonBusinessDays.reserveCapacity(1);
        reserveHolidayCapacity(1);
        d_packedCalendar.addHoliday(date);
        synchronizeCache();
    }
    else if (date < d_packedCalendar.firstDate()) {
        d_nonBusinessDays.reserveCapacity(
                                       d_packedCalendar.lastDate() - date + 1);
        reserveHolidayCapacity(numHolidays() + 1);
        d_packedCalendar.addHoliday(date);
        synchronizeCache();
    }
    else if (date > d_packedCalendar.lastDate()) {
        d_nonBusinessDays.reserveCapacity(
                                      date - d_packedCalendar.firstDate() + 1);
        reserveHolidayCapacity(numHolidays() + 1);
        d_packedCalendar.addHoliday(date);
        synchronizeCache();
    }
    else {
        reserveHolidayCapacity(numHolidays() + 1);
        d_packedCalendar.addHoliday(date);
        d_nonBusinessDays.assign1(date - d_packedCalendar.firstDate());
    }
}

void Calendar::addHolidayCode(const Date& date, int holidayCode)
{
    if (0 == length()) {
        d_nonBusinessDays.reserveCapacity(1);
        reserveHolidayCapacity(1);
        reserveHolidayCodeCapacity(1);
        d_packedCalendar.addHolidayCode(date, holidayCode);
        synchronizeCache();
    }
    else if (date < d_packedCalendar.firstDate()) {
        d_nonBusinessDays.reserveCapacity(
                                       d_packedCalendar.lastDate() - date + 1);
        reserveHolidayCapacity(numHolidays() + 1);
        reserveHolidayCodeCapacity(numHolidayCodesTotal() + 1);
        d_packedCalendar.addHolidayCode(date, holidayCode);
        synchronizeCache();
    }
    else if (date > d_packedCalendar.lastDate()) {
        d_nonBusinessDays.reserveCapacity(
                                      date - d_packedCalendar.firstDate() + 1);
        reserveHolidayCapacity(numHolidays() + 1);
        reserveHolidayCodeCapacity(numHolidayCodesTotal() + 1);
        d_packedCalendar.addHolidayCode(date, holidayCode);
        synchronizeCache();
    }
    else {
        reserveHolidayCapacity(numHolidays() + 1);
        reserveHolidayCodeCapacity(numHolidayCodesTotal() + 1);
        d_packedCalendar.addHolidayCode(date, holidayCode);
        d_nonBusinessDays.assign1(date - d_packedCalendar.firstDate());
    }
}

void Calendar::addWeekendDay(DayOfWeek::Enum weekendDay)
{
    d_packedCalendar.addWeekendDay(weekendDay);

    if (length()) {
        int weekendDayIndex = (static_cast<int>(weekendDay)
                             - static_cast<int>(
                                      d_packedCalendar.firstDate().dayOfWeek())
                             + 7)
                            % 7;
        while (weekendDayIndex < length()) {
            d_nonBusinessDays.assign1(weekendDayIndex);
            weekendDayIndex += 7;
        }
    }
}

void Calendar::addWeekendDays(const DayOfWeekSet& weekendDays)
{
    if (!weekendDays.isEmpty()) {
        for (DayOfWeekSet::iterator it = weekendDays.begin();
             it != weekendDays.end();
             ++it) {
            addWeekendDay(*it);
        }
    }
    else {
        d_packedCalendar.addWeekendDays(weekendDays);
    }
}

void Calendar::unionBusinessDays(const PackedCalendar& other)
{
    int newLength;
    if (length() && other.length()) {
        newLength = length();
        if (other.firstDate() < firstDate()) {
            newLength += firstDate() - other.firstDate();
        }
        if (other.lastDate() > lastDate()) {
            newLength += other.lastDate() - lastDate();
        }
    }
    else {
        newLength = length() + other.length();
    }

    d_nonBusinessDays.reserveCapacity(newLength);
    d_packedCalendar.unionBusinessDays(other);
    synchronizeCache();
}

void Calendar::unionNonBusinessDays(const PackedCalendar& other)
{
    int newLength;
    if (length() && other.length()) {
        newLength = length();
        if (other.firstDate() < firstDate()) {
            newLength += firstDate() - other.firstDate();
        }
        if (other.lastDate() > lastDate()) {
            newLength += other.lastDate() - lastDate();
        }
    }
    else {
        newLength = length() + other.length();
    }

    d_nonBusinessDays.reserveCapacity(newLength);
    d_packedCalendar.unionNonBusinessDays(other);
    synchronizeCache();
}

// ACCESSORS
int Calendar::getNextBusinessDay(Date        *nextBusinessDay,
                                 const Date&  date,
                                 int          nth) const
{
    BSLS_ASSERT(nextBusinessDay);
    BSLS_ASSERT(Date(9999, 12, 31) > date);
    BSLS_ASSERT(isInRange(date + 1));
    BSLS_ASSERT(0 < nth);

    enum { e_SUCCESS = 0, e_FAILURE = 1 };

    int offset = date - firstDate();
    while (nth) {
        offset = static_cast<int>(
                                d_nonBusinessDays.find0AtMinIndex(offset + 1));
        if (0 > offset) {
            return e_FAILURE;                                         // RETURN
        }
        --nth;
    }
    *nextBusinessDay = firstDate() + offset;

    return e_SUCCESS;
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE3.0

// DEPRECATED METHODS
Date Calendar::getNextBusinessDay(const Date& initialDate) const
{
    Date currentDate = initialDate;
    ++currentDate;

    if (0 == length()) {
        while (isWeekendDay(currentDate)) {
            ++currentDate;
        }

        return currentDate;                                           // RETURN
    }

    Date calendarFirstDate = firstDate();

    // For 'currentDate < calendarFirstDate', only weekend days are considered
    // non-business days.  Note that the following loop will run 6 times in the
    // worst case because we know at this point that this calendar has at least
    // one day which is not a weekend day.

    while (currentDate < calendarFirstDate) {
        if (0 == isWeekendDay(currentDate)) {
            return currentDate;                                       // RETURN
        }
        ++currentDate;
    }

    // If 'currentDate' is in the range of the calendar, we just need to lookup
    // in this calendar's cache.

    if (isInRange(currentDate)) {
        int offset = static_cast<int>(d_nonBusinessDays.find0AtMinIndex(
                                             currentDate - calendarFirstDate));
        if (offset != -1) {
            // Found the next business day within the valid range of this
            // calendar.

            return calendarFirstDate + offset;                        // RETURN
        }
        // No business day could be found in the range of the calendar.
        // Position 'currentDate' one day past the last valid date of this
        // calendar for the next lookup.

        currentDate = lastDate();
        ++currentDate;
    }

    // Starting from 'lastDate + 1', only weekend days are considered
    // non-business days.  Note that this loop will run 6 times in the worst
    // case because we know at this point that this calendar has at least one
    // day which is not a weekend day.

    while (isWeekendDay(currentDate)) {
        ++currentDate;
    }

    return currentDate;
}

Date Calendar::getNextBusinessDay(const Date& initialDate, int nth) const
{
    BSLS_ASSERT(nth >= 1);

    Date currentDate       = initialDate;
    ++currentDate;

    if (0 == length()) {
        while (1) {
            if (!isWeekendDay(currentDate)) {
                // Found a business day, so decrement 'nth'.

                --nth;
                if (0 == nth) {
                    // Found the next 'nth' business day after the last date.

                    break;
                }
            }
            ++currentDate;
        }

        return currentDate;                                           // RETURN
    }

    Date calendarFirstDate = firstDate();

    // Every time a new business day is found, 'nth' will be decremented by one
    // until it reaches 0, in which case we return the business day found.

    // For 'currentDate < calendarFirstDate', only weekend days are considered
    // holidays.  Note that the following loop will run '6 * nth' times in the
    // worst case because we know at this point that this calendar has at least
    // one day which is not a weekend day.

    while (currentDate < calendarFirstDate) {
        if (0 == isWeekendDay(currentDate)) {
            // Found a business day, so decrement 'nth'.

            --nth;
            if (0 == nth) {
                // Found the next 'nth' business day before the first date.

                return currentDate;                                   // RETURN
            }
        }
        ++currentDate;
    }
    int offset = currentDate - calendarFirstDate;

    // If 'currentDate' is in the range of the calendar, we just need to lookup
    // in this calendar's cache.

    while (isInRange(calendarFirstDate + offset)) {
        offset = static_cast<int>(d_nonBusinessDays.find0AtMinIndex(offset));
        if (offset != -1) {
            // Found the next business day within the valid range of this
            // calendar, so decrement 'nth'.

            --nth;
            if (0 == nth) {
                // Found the next 'nth' business day.

                return calendarFirstDate + offset;                    // RETURN
            }
            ++offset;
        }
        else {
            // No more business day could be found in the range of the
            // calendar.  Position 'currentDate' one day past the last valid
            // date of this calendar for the next lookup.

            break;
        }
    }

    // Position 'currentDate' one day past the last valid date of this calendar
    // for the next lookup only if 'currentDate' was within the valid range.

    if (isInRange(currentDate)) {
        currentDate = lastDate();
        ++currentDate;
    }

    // Starting from 'lastDate + 1', only weekend days are considered holidays.
    // Note that this loop will run '6 * nth' times in the worst case because
    // we know at this point that this calendar has at least one day which is
    // not a weekend day.

    while (1) {
        if (!isWeekendDay(currentDate)) {
            // Found a business day, so decrement 'nth'.

            --nth;
            if (0 == nth) {
                // Found the next 'nth' business day after the last date.

                break;
            }
        }
        ++currentDate;
    }

    return currentDate;
}

#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE3.0

                   // -----------------------------------
                   // class Calendar_BusinessDayConstIter
                   // -----------------------------------

// PRIVATE CREATORS
Calendar_BusinessDayConstIter::Calendar_BusinessDayConstIter(
                                     const bdlc::BitArray& nonBusinessDays,
                                     const Date&           firstDateOfCalendar,
                                     const Date&           startDate,
                                     bool                  endIterFlag)
: d_nonBusinessDays_p(&nonBusinessDays)
, d_firstDate(firstDateOfCalendar)
, d_currentOffset(startDate - firstDateOfCalendar)
{
    if (d_firstDate > startDate) {
        d_currentOffset = -1;
        return;                                                       // RETURN
    }
    BSLS_ASSERT(d_currentOffset >= 0);

    if (endIterFlag) {
        // This constructor is called from the overloaded 'endBusinessDays'
        // method.  If 'startDate' is the last date in the valid range of the
        // calendar, mark this iterator as an 'end' iterator and return.
        // Otherwise, advance the iterator to reference the next date so we can
        // find the next business day.

        if (d_currentOffset
                      == static_cast<int>(d_nonBusinessDays_p->length()) - 1) {
            d_currentOffset = -1;
            return;                                                   // RETURN
        }
        ++d_currentOffset;
    }

    d_currentOffset = static_cast<int>(
                        d_nonBusinessDays_p->find0AtMinIndex(d_currentOffset));
    if (0 > d_currentOffset) {
        d_currentOffset = -1;
    }
}

}  // close package namespace

// FREE FUNCTIONS
void bdlt::swap(Calendar& a, Calendar& b)
{
    if (a.allocator() == b.allocator()) {
        a.swap(b);

        return;                                                       // RETURN
    }

    Calendar futureA(b, a.allocator());
    Calendar futureB(a, b.allocator());

    futureA.swap(a);
    futureB.swap(b);
}

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
