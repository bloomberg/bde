// bdecs_packedcalendar.cpp                                           -*-C++-*-
#include <bdecs_packedcalendar.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdecs_packedcalendar_cpp,"$Id$ $CSID$")

#include <bdet_date.h>
#include <bdeu_print.h>

#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_functional.h>

namespace BloombergLP {

// STATIC HELPER FUNCTIONS
static
void addDayImp(bdet_Date        *firstDate,
               bdet_Date        *endDate,
               bsl::vector<int> *holidayOffsets,
               const bdet_Date&  date)
    // Insert the specified 'date' into the range of the calendar object
    // represented by the specified 'firstDate', 'endDate', and
    // 'holidayOffsets'.  If the specified 'date' is outside the range of the
    // calendar, this range will be extended to include it and 'firstDate' or
    // 'endDate' will be appropriately modified.  Otherwise, this function has
    // no effect.
{
    BSLS_ASSERT(firstDate);
    BSLS_ASSERT(endDate);
    BSLS_ASSERT(holidayOffsets);

    if (date < *firstDate) {
        bsl::transform(holidayOffsets->begin(), holidayOffsets->end(),
                       holidayOffsets->begin(),
                       bsl::bind2nd(bsl::plus<int>(), *firstDate - date));
        *firstDate = date;
    }
    if (date > *endDate) {
        *endDate = date;
    }
}

                        // --------------------------
                        // class bdecs_PackedCalendar
                        // --------------------------

// PRIVATE MANIPULATORS
int bdecs_PackedCalendar::addHolidayImp(const int offset)
{
    // Insertion must be in linear time if the holidays are added in-order.

    const int len = d_holidayOffsets.size();

    if (0 == len) {
        d_holidayOffsets.reserve(1);
        d_holidayCodesIndex.reserve(1);
        d_holidayOffsets.push_back(offset);
        d_holidayCodesIndex.push_back(0);
        return 0;
    }
    if (d_holidayOffsets.back() < offset) {
        d_holidayOffsets.reserve(len + 1);
        d_holidayCodesIndex.reserve(len + 1);
        d_holidayOffsets.push_back(offset);
        d_holidayCodesIndex.push_back(d_holidayCodes.size());
        return len;
    }

    OffsetsIterator it = bsl::lower_bound(d_holidayOffsets.begin(),
                                          d_holidayOffsets.end(),
                                          offset);

    BSLS_ASSERT(it != d_holidayOffsets.end());

    if (offset != *it) {
        const int shift = it - d_holidayOffsets.begin();
        d_holidayOffsets.reserve(len + 1);
        d_holidayCodesIndex.reserve(len + 1);

        // The calls to 'reserve' invalidated 'it'.

        it = d_holidayOffsets.begin();
        it += shift;
        it = d_holidayOffsets.insert(it, offset);

        CodesIndexIterator jt = d_holidayCodesIndex.begin();
        jt += shift;
        d_holidayCodesIndex.insert(jt, *jt);
    }
    return it - d_holidayOffsets.begin();
}

void bdecs_PackedCalendar::intersectNonBusinessDaysImp(
                                const bdecs_PackedCalendar& other,
                                bool                        fixIfDeltaPositive)

{
    const int delta = other.d_firstDate - d_firstDate;

    // We'll use these variables only if 'fixIfDeltaPositive' is true.

    const bdet_Date& newFirstDate = delta > 0
                                    ? other.d_firstDate
                                    : d_firstDate;
    const bdet_Date& newLastDate  = other.d_lastDate > d_lastDate
                                    ? d_lastDate
                                    : other.d_lastDate;

    // By reserving this space, we speed up the insertion process since we
    // invalidate fewer iterators and make the process exception-neutral with
    // rollback.

    d_holidayOffsets.reserve(other.d_holidayOffsets.size() +
                                                      d_holidayOffsets.size());
    d_holidayCodesIndex.reserve(other.d_holidayCodesIndex.size() +
                                                   d_holidayCodesIndex.size());
    d_holidayCodes.reserve(other.d_holidayCodes.size() +
                                                        d_holidayCodes.size());

    bdec_DayOfWeekSet oldWeekendDays = d_weekendDays;
    d_weekendDays &= other.d_weekendDays;

    OffsetsConstReverseIterator r = d_holidayOffsets.rbegin(); // "read"
                                                               // iterator

    OffsetsConstReverseIterator c = other.d_holidayOffsets.rbegin();
                                                               // "compare"
                                                               // iterator

    // "end" iterators of both 'd_holidayOffsets'

    OffsetsConstReverseIterator rend = d_holidayOffsets.rend();
    OffsetsConstReverseIterator cend = other.d_holidayOffsets.rend();

    // We initialize all new elements to 'd_holidayCodes.size()' so the indexes
    // are good for the last holiday.

    d_holidayOffsets.resize(other.d_holidayOffsets.size() +
                                                      d_holidayOffsets.size());
    d_holidayCodesIndex.resize(other.d_holidayCodesIndex.size() +
                                                    d_holidayCodesIndex.size(),
                               d_holidayCodes.size());
    d_holidayCodes.resize(other.d_holidayCodes.size() + d_holidayCodes.size());

    // We will use the following non-'const' iterators to update the vectors.

    OffsetsReverseIterator    mOffsets    = d_holidayOffsets.rbegin();
    CodesIndexReverseIterator mCodesIndex = d_holidayCodesIndex.rbegin();
    CodesReverseIterator      mCodes      = d_holidayCodes.rbegin();

    int lastCodeIndex = d_holidayCodes.size(); // last valid code index

    while (c != cend || r != rend) {
        OffsetsConstReverseIterator *read = 0;
        const bdecs_PackedCalendar *readCalendar = 0;

        if (c != cend && r != rend && *r == (*c + delta)) {
            // The holiday is in both calendars.

            *mOffsets = *r;
        }
        else if (c == cend || (r != rend && (*r > (*c + delta)))) {
            bdet_Date h = d_firstDate + *r;
            if (h > newLastDate || h < newFirstDate
             || false == other.d_weekendDays.isMember(h.dayOfWeek())) {
                ++r;
                continue;
            }

            // The holiday is a holiday in this calendar and a weekend day
            // in 'other'.

            read = &r;
            readCalendar = this;
            *mOffsets = *r;
        }
        else {
            bdet_Date h = other.d_firstDate + *c;
            if (h > newLastDate || h < newFirstDate
             || false == oldWeekendDays.isMember(h.dayOfWeek())) {
                ++c;
                continue;
            }

            // The holiday is a holiday in 'other' and a weekend day in this
            // calendar.

            read = &c;
            readCalendar = &other;
            *mOffsets = *c + delta;
        }

        // We need to adjust the value of the offset if the 'd_firstDate' is
        // changing.

        if ((fixIfDeltaPositive  && delta > 0)
         || (!fixIfDeltaPositive && delta < 0)) {
            *mOffsets -= delta;
        }

        const CodesReverseIterator mCodesStart = mCodes; // Keep the current
                                                         // value of mCodes to
                                                         // compute later the
                                                         // number of codes for
                                                         // this holiday.

        // *** UNION OF THE HOLIDAY CODES

        if (read) {

            // Find the range of holiday codes in this calendar.

            const OffsetsConstIterator base = read->base() - 1;
            CodesConstIterator codesBegin = readCalendar->
                beginHolidayCodes(base);
            CodesConstIterator codes = readCalendar->endHolidayCodes(base);
            while (codes != codesBegin) {
                --codes;
                *mCodes = *codes;
                ++mCodes;
            }
        }
        else {
            const OffsetsConstIterator rbase = r.base() - 1;
            const CodesConstReverseIterator lhs(endHolidayCodes(rbase));
            const CodesConstReverseIterator lhse(beginHolidayCodes(rbase));

            const OffsetsConstIterator cbase = c.base() - 1;
            const CodesConstReverseIterator rhs(other.endHolidayCodes(cbase));
            const CodesConstReverseIterator rhse(
                                              other.beginHolidayCodes(cbase));

            mCodes = bsl::set_union(lhs, lhse, rhs, rhse, mCodes,
                                    bsl::greater<int>());
        }

        lastCodeIndex -= (mCodes - mCodesStart);

        BSLS_ASSERT(lastCodeIndex >= 0
                 && (OffsetsSizeType) lastCodeIndex <= d_holidayCodes.size());

        *mCodesIndex = lastCodeIndex;

        BSLS_ASSERT(mCodesIndex != d_holidayCodesIndex.rend());
        BSLS_ASSERT(mOffsets != d_holidayOffsets.rend());

        ++mCodesIndex;
        ++mOffsets;

        if (read) {
            ++*read;
        }
        else {
            ++r;
            ++c;
        }
    }

    // Remove the unused part of the vectors.

    d_holidayOffsets.erase(d_holidayOffsets.begin(), mOffsets.base());
    d_holidayCodesIndex.erase(d_holidayCodesIndex.begin(), mCodesIndex.base());
    d_holidayCodes.erase(d_holidayCodes.begin(),
                         d_holidayCodes.begin() + lastCodeIndex);

    // Fix the codes index array, all the indexes are off by the number of
    // elements we remove.

    if (d_holidayCodesIndex.begin() != d_holidayCodesIndex.end()) {
        const int adjustment = *d_holidayCodesIndex.begin();
        if (adjustment != 0) {
            bsl::transform(d_holidayCodesIndex.begin(),
                           d_holidayCodesIndex.end(),
                           d_holidayCodesIndex.begin(),
                           bsl::bind2nd(bsl::minus<int>(), adjustment));
        }
    }

    BSLS_ASSERT(d_holidayOffsets.size() == d_holidayCodesIndex.size());
    BSLS_ASSERT(d_holidayOffsets.empty()
      || (OffsetsSizeType)d_holidayCodesIndex.back() <= d_holidayCodes.size());
}

void bdecs_PackedCalendar::intersectBusinessDaysImp(
                                const bdecs_PackedCalendar& other,
                                bool                        fixIfDeltaPositive)

{
    const int delta = other.d_firstDate - d_firstDate;

    // We'll use these variables only if 'fixIfDeltaPositive' is true.

    const bdet_Date& newFirstDate = delta > 0
                                    ? other.d_firstDate
                                    : d_firstDate;
    const bdet_Date& newLastDate  = other.d_lastDate > d_lastDate
                                    ? d_lastDate
                                    : other.d_lastDate;

    // By reserving this space, we speed up the insertion process since we
    // invalidate fewer iterators and make the process exception-neutral with
    // rollback.

    d_holidayOffsets.reserve(other.d_holidayOffsets.size() +
                                                      d_holidayOffsets.size());
    d_holidayCodesIndex.reserve(other.d_holidayCodesIndex.size() +
                                                   d_holidayCodesIndex.size());
    d_holidayCodes.reserve(other.d_holidayCodes.size() +
                                                        d_holidayCodes.size());

    d_weekendDays |= other.d_weekendDays;

    OffsetsConstReverseIterator r = d_holidayOffsets.rbegin(); // "read"
                                                               // iterator

    OffsetsConstReverseIterator c = other.d_holidayOffsets.rbegin();
                                                               // "compare"
                                                               // iterator

    // "end" iterators of both d_holidayOffsets

    OffsetsConstReverseIterator rend = d_holidayOffsets.rend();
    OffsetsConstReverseIterator cend = other.d_holidayOffsets.rend();

    // We initialize all new elements to 'd_holidayCodes.size()' so the indexes
    // are good for the last holiday.

    d_holidayOffsets.resize(other.d_holidayOffsets.size() +
                                                      d_holidayOffsets.size());
    d_holidayCodesIndex.resize(other.d_holidayCodesIndex.size() +
                                                    d_holidayCodesIndex.size(),
                               d_holidayCodes.size());
    d_holidayCodes.resize(other.d_holidayCodes.size() + d_holidayCodes.size());

    // We will use the following non-const iterators to update the vectors.

    OffsetsReverseIterator mOffsets = d_holidayOffsets.rbegin();
    CodesIndexReverseIterator mCodesIndex = d_holidayCodesIndex.rbegin();
    CodesReverseIterator mCodes = d_holidayCodes.rbegin();

    int lastCodeIndex = d_holidayCodes.size(); // last valid code index

    while (c != cend || r != rend) {
        OffsetsConstReverseIterator *read = 0;
        const bdecs_PackedCalendar *readCalendar = 0;

        if (c == cend || (r != rend && *r > (*c + delta))) {
            if (fixIfDeltaPositive) {
                bdet_Date h = d_firstDate + *r;
                if (h > newLastDate || h < newFirstDate) {
                    ++r;
                    continue;
                }
            }
            read = &r;
            readCalendar = this;
            *mOffsets = *r;
        }

        // Note that we are also checking for duplicates in the next block.

        if (r == rend || (c != cend && (*c + delta) >= *r)) {
            BSLS_ASSERT(!read);
            BSLS_ASSERT(!readCalendar);

            if (fixIfDeltaPositive) {
                bdet_Date h = other.d_firstDate + *c;
                if (h > newLastDate || h < newFirstDate) {
                    ++c;
                    continue;
                }
            }

            // If this holiday is present in 'r' and 'c', we will not set read
            // and 'readCalendar'.

            if (r == rend || *r != (*c + delta)) {
                read = &c;
                readCalendar = &other;
            }
            *mOffsets = *c + delta;
        }

        // We need to adjust the value of the offset if the 'd_firstDate' is
        // changing.

        if ((fixIfDeltaPositive  && delta > 0)
         || (!fixIfDeltaPositive && delta < 0)) {
            *mOffsets -= delta;
        }

        const CodesReverseIterator mCodesStart = mCodes; // Keep the current
                                                         // value of mCodes to
                                                         // compute later the
                                                         // number of codes for
                                                         // this holiday.

        // *** UNION OF THE HOLIDAY CODES

        if (read) {

            // Find the range of holiday codes in this calendar.

            const OffsetsConstIterator base = read->base() - 1;
            CodesConstIterator codesBegin = readCalendar->
                beginHolidayCodes(base);
            CodesConstIterator codes = readCalendar->endHolidayCodes(base);
            while (codes != codesBegin) {
                --codes;
                *mCodes = *codes;
                ++mCodes;
            }
        }
        else {
            const OffsetsConstIterator rbase = r.base() - 1;
            const CodesConstReverseIterator lhs(endHolidayCodes(rbase));
            const CodesConstReverseIterator lhse(beginHolidayCodes(rbase));

            const OffsetsConstIterator cbase = c.base() - 1;
            const CodesConstReverseIterator rhs(other.endHolidayCodes(cbase));
            const CodesConstReverseIterator rhse(
                                              other.beginHolidayCodes(cbase));

            mCodes = bsl::set_union(lhs, lhse, rhs, rhse, mCodes,
                                    bsl::greater<int>());
        }

        lastCodeIndex -= (mCodes - mCodesStart);

        BSLS_ASSERT(lastCodeIndex >= 0
                 && (OffsetsSizeType)lastCodeIndex <= d_holidayCodes.size());

        *mCodesIndex = lastCodeIndex;

        BSLS_ASSERT(mCodesIndex != d_holidayCodesIndex.rend());
        BSLS_ASSERT(mOffsets != d_holidayOffsets.rend());

        ++mCodesIndex;
        ++mOffsets;

        if (read) {
            ++*read;
        }
        else {
            ++r;
            ++c;
        }
    }

    // Remove the unused part of the vectors.

    d_holidayOffsets.erase(d_holidayOffsets.begin(), mOffsets.base());
    d_holidayCodesIndex.erase(d_holidayCodesIndex.begin(), mCodesIndex.base());
    d_holidayCodes.erase(d_holidayCodes.begin(),
                         d_holidayCodes.begin() + lastCodeIndex);

    // Fix the codes index array, all the indexes are off by the number of
    // elements we remove.

    if (d_holidayCodesIndex.begin() != d_holidayCodesIndex.end()) {
        const int adjustment = *d_holidayCodesIndex.begin();
        if (adjustment != 0) {
            bsl::transform(d_holidayCodesIndex.begin(),
                           d_holidayCodesIndex.end(),
                           d_holidayCodesIndex.begin(),
                           bsl::bind2nd(bsl::minus<int>(), adjustment));
        }
    }

    BSLS_ASSERT(d_holidayOffsets.size() == d_holidayCodesIndex.size());
    BSLS_ASSERT(d_holidayOffsets.empty()
      || (OffsetsSizeType)d_holidayCodesIndex.back() <= d_holidayCodes.size());
}

// CREATORS
bdecs_PackedCalendar::bdecs_PackedCalendar(bslma_Allocator *basicAllocator)
: d_firstDate(9999, 12, 31)
, d_lastDate(1, 1, 1)
, d_holidayOffsets(basicAllocator)
, d_holidayCodesIndex(basicAllocator)
, d_holidayCodes(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

bdecs_PackedCalendar::bdecs_PackedCalendar(const bdet_Date&  firstDate,
                                           const bdet_Date&  lastDate,
                                           bslma_Allocator  *basicAllocator)
: d_firstDate(firstDate <= lastDate ? firstDate : bdet_Date(9999, 12, 31))
, d_lastDate(firstDate <= lastDate ? lastDate : bdet_Date(1, 1, 1))
, d_holidayOffsets(basicAllocator)
, d_holidayCodesIndex(basicAllocator)
, d_holidayCodes(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

bdecs_PackedCalendar::bdecs_PackedCalendar(
                                   const bdecs_PackedCalendar&  original,
                                   bslma_Allocator             *basicAllocator)
: d_firstDate(original.d_firstDate)
, d_lastDate(original.d_lastDate)
, d_weekendDays(original.d_weekendDays)
, d_holidayOffsets(original.d_holidayOffsets, basicAllocator)
, d_holidayCodesIndex(original.d_holidayCodesIndex, basicAllocator)
, d_holidayCodes(original.d_holidayCodes, basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

bdecs_PackedCalendar::~bdecs_PackedCalendar()
{
    BSLS_ASSERT(d_holidayOffsets.size() == d_holidayCodesIndex.size());
    BSLS_ASSERT(d_holidayOffsets.empty()
      || (OffsetsSizeType)d_holidayCodesIndex.back() <= d_holidayCodes.size());
}

// MANIPULATORS
bdecs_PackedCalendar&
bdecs_PackedCalendar::operator=(const bdecs_PackedCalendar& rhs)
{
    bdecs_PackedCalendar(rhs, d_allocator_p).swap(*this);
    return *this;
}

void bdecs_PackedCalendar::swap(bdecs_PackedCalendar& other)
{
    // 'swap' is undefined for objects with non-equal allocators.
    BSLS_ASSERT(d_allocator_p == other.d_allocator_p);

    using bsl::swap;
    swap(d_firstDate, other.d_firstDate);
    swap(d_lastDate, other.d_lastDate);
    swap(d_weekendDays, other.d_weekendDays);
    swap(d_holidayOffsets, other.d_holidayOffsets);
    swap(d_holidayCodesIndex, other.d_holidayCodesIndex);
    swap(d_holidayCodes, other.d_holidayCodes);
}

void bdecs_PackedCalendar::addDay(const bdet_Date& date)
{
    addDayImp(&d_firstDate, &d_lastDate, &d_holidayOffsets, date);
}

void bdecs_PackedCalendar::addHoliday(const bdet_Date& date)
{
    addDayImp(&d_firstDate, &d_lastDate, &d_holidayOffsets, date);
    addHolidayImp(date - d_firstDate);

    BSLS_ASSERT(d_holidayOffsets.size() == d_holidayCodesIndex.size());
}

int bdecs_PackedCalendar::addHolidayIfInRange(const bdet_Date& date)
{
    if (isInRange(date)) {
        addHolidayImp(date - d_firstDate);
        return 0;
    }
    return -1;
}

void bdecs_PackedCalendar::addHolidayCode(const bdet_Date& date,
                                          int              holidayCode)
{
    addDayImp(&d_firstDate, &d_lastDate, &d_holidayOffsets, date);
    const int index = addHolidayImp(date - d_firstDate);
    const OffsetsIterator holiday = d_holidayOffsets.begin() + index;
    const CodesIterator b = beginHolidayCodes(holiday);
    const CodesIterator e = endHolidayCodes(holiday);

    CodesIterator it = bsl::lower_bound(b, e, holidayCode);
    if (it == e || holidayCode != *it) {
        it = d_holidayCodes.insert(it, holidayCode);

        // Since we inserted a code in the codes vectors, all the indexes for
        // the holidays following 'date' will be off by one; let's fix that.

        bsl::transform(d_holidayCodesIndex.begin() + index + 1,
                       d_holidayCodesIndex.end(),
                       d_holidayCodesIndex.begin() + index + 1,
                       bsl::bind2nd(bsl::plus<int>(), 1));
    }
    BSLS_ASSERT(d_holidayOffsets.size() == d_holidayCodesIndex.size());
    BSLS_ASSERT(d_holidayOffsets.empty()
      || (OffsetsSizeType)d_holidayCodesIndex.back() <= d_holidayCodes.size());
}

int bdecs_PackedCalendar::addHolidayCodeIfInRange(const bdet_Date& date,
                                                  int              holidayCode)
{
    if (isInRange(date)) {
        addHolidayCode(date, holidayCode);
        return 0;
    }
    return -1;
}

void
bdecs_PackedCalendar::intersectBusinessDays(const bdecs_PackedCalendar& other)
{
    // The 'true' indicates that we want to intersect the range.

    intersectBusinessDaysImp(other, true);

    d_firstDate = d_firstDate > other.d_firstDate
                  ? d_firstDate
                  : other.d_firstDate;
    d_lastDate  = d_lastDate < other.d_lastDate
                  ? d_lastDate
                  : other.d_lastDate;

    // Normalize "empty" calendars.

    if (d_firstDate > d_lastDate) {
        BSLS_ASSERT(d_holidayOffsets.empty());
        BSLS_ASSERT(d_holidayCodesIndex.empty());
        BSLS_ASSERT(d_holidayCodes.empty());

        setValidRange(d_firstDate, d_lastDate);
    }
}

void
bdecs_PackedCalendar::intersectNonBusinessDays(
                                             const bdecs_PackedCalendar& other)
{
    // The 'true' indicates that we want to intersect the range.

    intersectNonBusinessDaysImp(other, true);

    const int firstDelta = other.d_firstDate - d_firstDate;
    const int lastDelta  = other.d_lastDate - d_lastDate;
    if (firstDelta > 0) {
        d_firstDate += firstDelta;
    }
    if (lastDelta < 0) {
        d_lastDate += lastDelta;
    }

    // Normalize "empty" calendars.

    if (d_firstDate > d_lastDate) {
        BSLS_ASSERT(d_holidayOffsets.empty());
        BSLS_ASSERT(d_holidayCodesIndex.empty());
        BSLS_ASSERT(d_holidayCodes.empty());

        setValidRange(d_firstDate, d_lastDate);
    }
}

void bdecs_PackedCalendar::unionBusinessDays(const bdecs_PackedCalendar& other)
{
    // The 'false' indicates that we do not want to intersect the range, but
    // instead unite it.

    intersectNonBusinessDaysImp(other, false);

    const int firstDelta = other.d_firstDate - d_firstDate;
    const int lastDelta  = other.d_lastDate - d_lastDate;

    if (firstDelta < 0) {
        d_firstDate += firstDelta;
    }
    if (lastDelta > 0) {
        d_lastDate += lastDelta;
    }

}

void
bdecs_PackedCalendar::unionNonBusinessDays(const bdecs_PackedCalendar& other)
{
    // The 'false' indicates that we do not want to intersect the range, but
    // instead unite it.

    intersectBusinessDaysImp(other, false);

    d_firstDate = d_firstDate < other.d_firstDate
                  ? d_firstDate
                  : other.d_firstDate;
    d_lastDate  = d_lastDate > other.d_lastDate
                  ? d_lastDate
                  : other.d_lastDate;
}

void bdecs_PackedCalendar::removeHoliday(const bdet_Date& date)
{
    const int offset = date - d_firstDate;
    const OffsetsIterator oit = bsl::lower_bound(d_holidayOffsets.begin(),
                                                 d_holidayOffsets.end(),
                                                 offset);

    if (oit != d_holidayOffsets.end() && *oit == offset) {
        const CodesIterator b = beginHolidayCodes(oit);
        const CodesIterator e = endHolidayCodes(oit);
        const int codesRemoved = e - b;

        d_holidayCodes.erase(b, e);
        CodesIndexIterator cit = d_holidayCodesIndex.begin() +
                                             (oit - d_holidayOffsets.begin());
        bsl::transform(cit + 1, d_holidayCodesIndex.end(), cit + 1,
                       bsl::bind2nd(bsl::minus<int>(), codesRemoved));

        cit = d_holidayCodesIndex.begin() + (oit - d_holidayOffsets.begin());
        d_holidayCodesIndex.erase(cit, cit + 1);
        d_holidayOffsets.erase(oit, oit + 1);
    }
    BSLS_ASSERT(d_holidayOffsets.size() == d_holidayCodesIndex.size());
    BSLS_ASSERT(d_holidayOffsets.empty()
      || (OffsetsSizeType)d_holidayCodesIndex.back() <= d_holidayCodes.size());
}

void bdecs_PackedCalendar::removeHolidayCode(const bdet_Date& date,
                                             int              holidayCode)
{
    const int offset = date - d_firstDate;
    const OffsetsIterator oit = bsl::lower_bound(d_holidayOffsets.begin(),
                                                 d_holidayOffsets.end(),
                                                 offset);

    if (oit != d_holidayOffsets.end() && *oit == offset) {
        const CodesIterator b = beginHolidayCodes(oit);
        const CodesIterator e = endHolidayCodes(oit);

        CodesIterator cit = bsl::lower_bound(b, e, holidayCode);
        if (cit != e && *cit == holidayCode) {
            d_holidayCodes.erase(cit, cit + 1);
            const int shift = oit - d_holidayOffsets.begin() + 1;
            bsl::transform(d_holidayCodesIndex.begin() + shift,
                           d_holidayCodesIndex.end(),
                           d_holidayCodesIndex.begin() + shift,
                           bsl::bind2nd(bsl::minus<int>(), 1));
        }
    }
    BSLS_ASSERT(d_holidayOffsets.size() == d_holidayCodesIndex.size());
    BSLS_ASSERT(d_holidayOffsets.empty()
      || (OffsetsSizeType)d_holidayCodesIndex.back() <= d_holidayCodes.size());
}

void bdecs_PackedCalendar::removeAll()
{
    d_firstDate.setYearMonthDay(9999, 12, 31);
    d_lastDate.setYearMonthDay(1, 1, 1);
    d_weekendDays.removeAll();
    d_holidayOffsets.clear();
    d_holidayCodesIndex.clear();
    d_holidayCodes.clear();
}

void bdecs_PackedCalendar::setValidRange(const bdet_Date& firstDate,
                                         const bdet_Date& lastDate)
{
    if (firstDate > lastDate) {
        bdec_DayOfWeekSet weekendDays = d_weekendDays;
        removeAll();
        d_weekendDays = weekendDays;
        return;
    }

    if (lastDate < d_firstDate || firstDate > d_lastDate) {
        d_holidayOffsets.clear();
        d_holidayCodesIndex.clear();
        d_holidayCodes.clear();
        d_firstDate = firstDate;
        d_lastDate = lastDate;
        return;
    }

    OffsetsIterator b  = d_holidayOffsets.begin();
    OffsetsIterator e  = d_holidayOffsets.end();
    OffsetsIterator it = bsl::lower_bound(b, e, lastDate - d_firstDate + 1);

    if (it != e) {
        BSLS_ASSERT(lastDate <= d_lastDate);

        CodesIndexIterator jt(d_holidayCodesIndex.begin());
        jt += (it - b);
        CodesIterator kt(d_holidayCodes.begin());
        kt += *jt;
        d_holidayCodes.erase(kt, d_holidayCodes.end());
        d_holidayCodesIndex.erase(jt, d_holidayCodesIndex.end());
        d_holidayOffsets.erase(it, d_holidayOffsets.end());
    }
    d_lastDate = lastDate;

    if (d_firstDate == firstDate) {
        return;
    }

    if (d_firstDate < firstDate) {
        b = d_holidayOffsets.begin();
        e  = d_holidayOffsets.end();

        // We're looking for all holidays lower and up to firstDate -
        // d_firstDate - 1.  upper_bound will return an iterator one past the
        // last element we want to remove.

        it = bsl::upper_bound(b, e, firstDate - d_firstDate - 1);
        CodesIndexIterator jt = d_holidayCodesIndex.begin();
        jt += (it - b);
        CodesIterator kt = d_holidayCodes.begin();
        kt += (it != e) ? *jt : d_holidayCodes.size();

        d_holidayCodes.erase(d_holidayCodes.begin(), kt);
        d_holidayCodesIndex.erase(d_holidayCodesIndex.begin(), jt);
        d_holidayOffsets.erase(d_holidayOffsets.begin(), it);
    }

    if (d_holidayCodesIndex.begin() != d_holidayCodesIndex.end()) {
        const int adjustment = *d_holidayCodesIndex.begin();
        if (adjustment != 0) {
            bsl::transform(d_holidayCodesIndex.begin(),
                           d_holidayCodesIndex.end(),
                           d_holidayCodesIndex.begin(),
                           bsl::bind2nd(bsl::minus<int>(), adjustment));
        }
    }

    if (firstDate != d_firstDate
     && d_holidayOffsets.begin() != d_holidayOffsets.end()) {
        bsl::transform(d_holidayOffsets.begin(), d_holidayOffsets.end(),
                       d_holidayOffsets.begin(),
                       bsl::bind2nd(bsl::minus<int>(),
                                    firstDate - d_firstDate));
    }

    d_firstDate = firstDate;

    BSLS_ASSERT((int)d_holidayOffsets.size() <= (lastDate - firstDate + 1));
    BSLS_ASSERT(d_holidayOffsets.size() == d_holidayCodesIndex.size());
    BSLS_ASSERT(d_holidayOffsets.empty()
      || (OffsetsSizeType)d_holidayCodesIndex.back() <= d_holidayCodes.size());
}

void bdecs_PackedCalendar::swap(bdecs_PackedCalendar *other)
{
    this->swap(*other);
}

// ACCESSORS
bdecs_PackedCalendar::HolidayCodeConstIterator
bdecs_PackedCalendar::beginHolidayCodes(const bdet_Date& date) const
{
    BSLS_ASSERT(isInRange(date));

    const int offset = date - d_firstDate;
    const OffsetsConstIterator offsetBegin = d_holidayOffsets.begin();
    const OffsetsConstIterator offsetEnd   = d_holidayOffsets.end();
    const OffsetsConstIterator i = bsl::lower_bound(offsetBegin,
                                                    offsetEnd,
                                                    offset);
    int iterIndex;
    if (i == offsetEnd || *i != offset) {
        iterIndex = d_holidayCodes.size();
    }
    else {
        iterIndex = d_holidayCodesIndex[i - offsetBegin];
    }
    return HolidayCodeConstIterator(d_holidayCodes.begin() + iterIndex);
}

bdecs_PackedCalendar::HolidayCodeConstIterator
bdecs_PackedCalendar::endHolidayCodes(const bdet_Date& date) const
{
    BSLS_ASSERT(isInRange(date));

    const int offset = date - d_firstDate;
    const OffsetsConstIterator offsetBegin = d_holidayOffsets.begin();
    const OffsetsConstIterator offsetEnd   = d_holidayOffsets.end();
    const OffsetsConstIterator i = bsl::lower_bound(offsetBegin,
                                                    offsetEnd,
                                                    offset);
    int iterIndex;
    if (i == offsetEnd || *i != offset) {
        iterIndex = d_holidayCodes.size();
    }
    else {
        iterIndex = i != (offsetEnd - 1)
                    ? d_holidayCodesIndex[i - offsetBegin + 1]
                    : d_holidayCodes.size();
    }
    return HolidayCodeConstIterator(d_holidayCodes.begin() + iterIndex);
}

int bdecs_PackedCalendar::numHolidayCodes(const bdet_Date& date) const
{
    BSLS_ASSERT(isInRange(date));

    const OffsetsConstIterator it = bsl::lower_bound(d_holidayOffsets.begin(),
                                                     d_holidayOffsets.end(),
                                                     date - d_firstDate);
    if (it == d_holidayOffsets.end() || *it != (date - d_firstDate)) {
        return 0;
    }

    return endHolidayCodes(it) - beginHolidayCodes(it);
}

int bdecs_PackedCalendar::numNonBusinessDays() const
{
    const int length = d_holidayOffsets.size();
    int sum = numWeekendDaysInRange() + length;

    for (OffsetsConstIterator it = d_holidayOffsets.begin();
                                          it != d_holidayOffsets.end(); ++it) {
        sum -= isWeekendDay(d_firstDate + *it);
    }

    return sum;
}

bsl::ostream& bdecs_PackedCalendar::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;
    }

    const char NL = spacesPerLevel >= 0 ? '\n' : ' ';

    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    stream << "{" <<  NL;

    if (spacesPerLevel >= 0) {
        bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    }
    stream << "[ " << d_firstDate << ", " << d_lastDate << " ]" << NL;

    if (spacesPerLevel >= 0) {
        bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    }
    d_weekendDays.print(stream, -1, -1);
    stream << NL;

    for (OffsetsConstIterator i = d_holidayOffsets.begin();
                                            i != d_holidayOffsets.end(); ++i) {

        if (spacesPerLevel >= 0) {
            bdeu_Print::indent(stream, level + 1, spacesPerLevel);
        }
        stream << (d_firstDate + *i);

        const CodesConstIterator b = beginHolidayCodes(i);
        const CodesConstIterator e = endHolidayCodes(i);
        if (b != e) {
            stream << " {" << NL;
        }

        for (CodesConstIterator j = b; j != e; ++j) {
            if (spacesPerLevel >= 0) {
                bdeu_Print::indent(stream, level + 2, spacesPerLevel);
            }
            stream << *j << NL;
        }

        if (spacesPerLevel >= 0) {
            bdeu_Print::indent(stream, level + 1, spacesPerLevel);
        }
        if (b != e) {
            stream << "}";
        }

        stream << NL;
    }
    if (spacesPerLevel >= 0) {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }
    stream << "}";

    if (spacesPerLevel >= 0) {
        stream << NL;
    }

    return stream;
}

int bdecs_PackedCalendar::numWeekendDaysInRange() const
{
    const int len = length();

    // Assume that there is a partial week of 0-6 days followed by some number
    // of full weeks.  First calculate the full weeks then test every day in
    // the partial week.

    int numWeekendDays = (len / 7) * numWeekendDaysInWeek();
    int dayOfWeek      = (int)d_firstDate.dayOfWeek();

    switch (len % 7) {
      case 6:
        numWeekendDays += d_weekendDays.isMember(
                                               (bdet_DayOfWeek::Day)dayOfWeek);
        dayOfWeek = dayOfWeek % 7 + 1;
      case 5:
        numWeekendDays += d_weekendDays.isMember(
                                               (bdet_DayOfWeek::Day)dayOfWeek);
        dayOfWeek = dayOfWeek % 7 + 1;
      case 4:
        numWeekendDays += d_weekendDays.isMember(
                                               (bdet_DayOfWeek::Day)dayOfWeek);
        dayOfWeek = dayOfWeek % 7 + 1;
      case 3:
        numWeekendDays += d_weekendDays.isMember(
                                               (bdet_DayOfWeek::Day)dayOfWeek);
        dayOfWeek = dayOfWeek % 7 + 1;
      case 2:
        numWeekendDays += d_weekendDays.isMember(
                                               (bdet_DayOfWeek::Day)dayOfWeek);
        dayOfWeek = dayOfWeek % 7 + 1;
      case 1:
        numWeekendDays += d_weekendDays.isMember(
                                               (bdet_DayOfWeek::Day)dayOfWeek);
        dayOfWeek = dayOfWeek % 7 + 1;
    }

    return numWeekendDays;
}

// FREE OPERATORS
bool operator==(const bdecs_PackedCalendar& lhs,
                const bdecs_PackedCalendar& rhs)
{
    return lhs.d_firstDate         == rhs.d_firstDate
        && lhs.d_lastDate          == rhs.d_lastDate
        && lhs.d_weekendDays       == rhs.d_weekendDays
        && lhs.d_holidayOffsets    == rhs.d_holidayOffsets
        && lhs.d_holidayCodesIndex == rhs.d_holidayCodesIndex
        && lhs.d_holidayCodes      == rhs.d_holidayCodes;
}

bool operator!=(const bdecs_PackedCalendar& lhs,
                const bdecs_PackedCalendar& rhs)
{
    return lhs.d_firstDate         != rhs.d_firstDate
        || lhs.d_lastDate          != rhs.d_lastDate
        || lhs.d_weekendDays       != rhs.d_weekendDays
        || lhs.d_holidayOffsets    != rhs.d_holidayOffsets
        || lhs.d_holidayCodesIndex != rhs.d_holidayCodesIndex
        || lhs.d_holidayCodes      != rhs.d_holidayCodes;
}

                // ---------------------------------------------------
                // class bdecs_PackedCalendar_BusinessDayConstIterator
                // ---------------------------------------------------

// PRIVATE MANIPULATORS
void bdecs_PackedCalendar_BusinessDayConstIterator::nextBusinessDay()
{
    BSLS_ASSERT((d_calendar_p->firstDate() + d_currentOffset).dayOfWeek() ==
                                                                  d_dayOfWeek);
    BSLS_ASSERT(false == d_endFlag);

    const int lastOffset =
                          d_calendar_p->lastDate() - d_calendar_p->firstDate();

    while (d_currentOffset < lastOffset) {
        ++d_currentOffset;
        d_dayOfWeek = d_dayOfWeek != bdet_DayOfWeek::BDET_SAT
                      ? (bdet_DayOfWeek::Day)(1 + d_dayOfWeek)
                      : bdet_DayOfWeek::BDET_SUN;
        if (d_offsetIter == d_calendar_p->d_holidayOffsets.end()
         || d_currentOffset < *d_offsetIter) {
            if (!d_calendar_p->isWeekendDay(d_dayOfWeek)) {

                // We found the next business day.

                return;
            }
        }
        else {
            ++d_offsetIter;
        }
    }

    // We did not find a next business day.  This iterator becomes an 'end'
    // iterator.

    d_endFlag = true;
}

void bdecs_PackedCalendar_BusinessDayConstIterator::previousBusinessDay()
{
    BSLS_ASSERT((d_calendar_p->firstDate() + d_currentOffset).dayOfWeek() ==
                                                                  d_dayOfWeek);

    while (d_currentOffset >= 0) {
        if (false == d_endFlag) {
            --d_currentOffset;
            d_dayOfWeek = d_dayOfWeek != bdet_DayOfWeek::BDET_SUN
                          ? (bdet_DayOfWeek::Day)(d_dayOfWeek - 1)
                          : bdet_DayOfWeek::BDET_SAT;
        }
        else {
            d_endFlag = false;
        }

        if (d_offsetIter != d_calendar_p->d_holidayOffsets.begin()) {
            OffsetsConstIterator iter = d_offsetIter;
            --iter;
            if (d_currentOffset > *iter) {
                if (!d_calendar_p->isWeekendDay(d_dayOfWeek)) {

                    // We found the previous business day.

                    return;
                }
            }
            else {
                --d_offsetIter;
            }

        }
        else if (!d_calendar_p->isWeekendDay(d_dayOfWeek)) {

            // We found the previous business day.

            return;
        }
    }

    // No previous business day was found.  This is undefined behavior.

    BSLS_ASSERT(0);
}

// CREATORS
bdecs_PackedCalendar_BusinessDayConstIterator::
bdecs_PackedCalendar_BusinessDayConstIterator(
                                       const bdecs_PackedCalendar& calendar,
                                       const bdet_Date&            startDate,
                                       bool                        endIterFlag)
: d_calendar_p(&calendar)
, d_dayOfWeek(startDate.dayOfWeek())
, d_currentOffset(startDate - calendar.firstDate())
, d_endFlag(false)
{
    if (calendar.firstDate() > calendar.lastDate()) {
        d_endFlag = true;
        return;
    }

    d_offsetIter = bsl::lower_bound(calendar.d_holidayOffsets.begin(),
                                    calendar.d_holidayOffsets.end(),
                                    d_currentOffset);

    bool businessDayFlag = true;

    // Adjust 'd_offsetIter' to reference the first holiday after
    // 'd_currentOffset'.

    if (d_offsetIter != calendar.d_holidayOffsets.end()
     && *d_offsetIter == d_currentOffset) {
        businessDayFlag = false;
        ++d_offsetIter;
    }

    if (calendar.isWeekendDay(d_dayOfWeek)) {
        businessDayFlag = false;
    }

    if (false == businessDayFlag || true == endIterFlag) {
        // If 'startDate' is not a business day, move 'd_currentDate' to the
        // next business day.  If 'endIterFlag' is true, this constructor is
        // called from 'endBusinessDays' or 'endBusinessDays(bdet_Date& date)'.
        // We need to move the iterator to point to one element past the first
        // business day that occurs on or before the specified 'startDate'.  So
        // call 'nextBusinessDay' to move the iterator to the next element.

        nextBusinessDay();
    }
}

// MANIPULATORS
bdecs_PackedCalendar_BusinessDayConstIterator&
bdecs_PackedCalendar_BusinessDayConstIterator::operator=(
                      const bdecs_PackedCalendar_BusinessDayConstIterator& rhs)
{
    d_offsetIter    = rhs.d_offsetIter;
    d_calendar_p    = rhs.d_calendar_p;
    d_dayOfWeek     = rhs.d_dayOfWeek;
    d_currentOffset = rhs.d_currentOffset;
    d_endFlag       = rhs.d_endFlag;

    BSLS_ASSERT_SAFE(
     (d_calendar_p->firstDate() + d_currentOffset).dayOfWeek() == d_dayOfWeek);

    return *this;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const bdecs_PackedCalendar& calendar)
{
    calendar.print(stream, 0, -1);
    return stream;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
