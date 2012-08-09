// bdecs_packedcalendar.cpp                                           -*-C++-*-
#include <bdecs_packedcalendar.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdecs_packedcalendar_cpp,"$Id$ $CSID$")

#include <bdet_date.h>
#include <bdeu_print.h>

#include <bslalg_swaputil.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_functional.h>

namespace BloombergLP {

namespace {

// HELPER FUNCTIONS
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

int numWeekendDaysInRangeImp(const bdet_Date&         firstDate,
                             const bdet_Date&         endDate,
                             const bdec_DayOfWeekSet& weekendDays)
// Return the number of days in the range starting from the specified
// 'firstDate' to the date immediately before 'endDate' whoes day-of-week are
// in the specified 'weekendDays' set.  Note that this function returns 0 if
// 'endDate < firstDate'.
{
    const int len = firstDate < endDate ? endDate - firstDate : 0;

    // Assume that there is a partial week of 0-6 days followed by some number
    // of full weeks.  First calculate the full weeks then test every day in
    // the partial week.

    int numWeekendDays = (len / 7) * weekendDays.length();
    int dayOfWeek      = (int) firstDate.dayOfWeek();


    // The switch statement unrolls a loop that repeats the same number of
    // times as the number of days in the first partial week.

    switch (len % 7) {
      case 6:
        numWeekendDays += weekendDays.isMember(
                                  static_cast<bdet_DayOfWeek::Day>(dayOfWeek));
        dayOfWeek = dayOfWeek % 7 + 1;
      case 5:
        numWeekendDays += weekendDays.isMember(
                                  static_cast<bdet_DayOfWeek::Day>(dayOfWeek));
        dayOfWeek = dayOfWeek % 7 + 1;
      case 4:
        numWeekendDays += weekendDays.isMember(
                                  static_cast<bdet_DayOfWeek::Day>(dayOfWeek));
        dayOfWeek = dayOfWeek % 7 + 1;
      case 3:
        numWeekendDays += weekendDays.isMember(
                                  static_cast<bdet_DayOfWeek::Day>(dayOfWeek));
        dayOfWeek = dayOfWeek % 7 + 1;
      case 2:
        numWeekendDays += weekendDays.isMember(
                                  static_cast<bdet_DayOfWeek::Day>(dayOfWeek));
        dayOfWeek = dayOfWeek % 7 + 1;
      case 1:
        numWeekendDays += weekendDays.isMember(
                                  static_cast<bdet_DayOfWeek::Day>(dayOfWeek));
        dayOfWeek = dayOfWeek % 7 + 1;
    }

    return numWeekendDays;
}

typedef bsl::vector<bsl::pair<bdet_Date, bdec_DayOfWeekSet> > WTransitions;

void intersectWeekendDaysTransitions(WTransitions        *result,
                                     const WTransitions&  lhs,
                                     const WTransitions&  rhs)
// Load, into the specified 'result', the intersection of the specified 'lhs'
// weekend-days transitions and the specified 'rhs' weekend-days transitions.
{
    if (lhs.empty() || rhs.empty()) {
        return;
    }

    WTransitions::const_iterator l = lhs.begin();
    WTransitions::const_iterator r = rhs.begin();

    WTransitions::const_iterator lprev = lhs.end();
    WTransitions::const_iterator rprev = rhs.end();

    while (l != lhs.end() && r != rhs.end()) {

        if (l->first < r->first) {
            if (rprev == rhs.end()) {

                // The intersection is empty.

                result->push_back(WTransitions::value_type(
                                               l->first, bdec_DayOfWeekSet()));
            }
            else {
                WTransitions::value_type val = *l;
                val.second &= rprev->second;
                result->push_back(val);
            }
            lprev = l;
            ++l;
        }
        else if (r->first < l->first) {
            if (lprev == lhs.end()) {

                // The intersection is empty.

                result->push_back(WTransitions::value_type(
                                               r->first, bdec_DayOfWeekSet()));
            }
            else {
                WTransitions::value_type val = *r;
                val.second &= lprev->second;
                result->push_back(val);
            }
            rprev = r;
            ++r;
        }
        else {
            WTransitions::value_type val = *l;
            val.second &= r->second;
            result->push_back(val);
            lprev = l;
            rprev = r;
            ++l;
            ++r;
        }
    }

    while (l != lhs.end()) {
        WTransitions::value_type val = *l;
        val.second &= rprev->second;
        result->push_back(val);
        ++l;
    }

    while (r != rhs.end()) {
        WTransitions::value_type val = *r;
        val.second &= lprev->second;
        result->push_back(val);
        ++r;
    }
}


void unionWeekendDaysTransitions(WTransitions *result,
                                 const WTransitions& lhs,
                                 const WTransitions& rhs)
// Load, into the specified 'result', the union of the specified 'lhs'
// weekend-days transitions and the specified 'rhs' weekend-days transitions.
{
    if (lhs.empty()) {
        *result = rhs;
        return;
    }

    if (rhs.empty()) {
        *result = lhs;
        return;
    }

    WTransitions::const_iterator l = lhs.begin();
    WTransitions::const_iterator r = rhs.begin();
    WTransitions::const_iterator lprev = lhs.end();
    WTransitions::const_iterator rprev = rhs.end();

    while (l != lhs.end() && r != rhs.end()) {

        if (l->first < r->first) {
            if (rprev == rhs.end()) {
                result->push_back(*l);
            }
            else {
                WTransitions::value_type val = *l;
                val.second |= rprev->second;
                result->push_back(val);
            }
            lprev = l;
            ++l;
        }
        else if (r->first < l->first) {
            if (lprev == lhs.end()) {
                result->push_back(*r);
            }
            else {
                WTransitions::value_type val = *r;
                val.second |= lprev->second;
                result->push_back(val);
            }
            rprev = r;
            ++r;
        }
        else {
            WTransitions::value_type val = *l;
            val.second |= r->second;
            result->push_back(val);
            lprev = l;
            rprev = r;
            ++l;
            ++r;
        }
    }

    while (l != lhs.end()) {
        WTransitions::value_type val = *l;
        val.second |= rprev->second;
        result->push_back(val);
        ++l;
    }

    while (r != rhs.end()) {
        WTransitions::value_type val = *r;
        val.second |= lprev->second;
        result->push_back(val);
        ++r;
    }
}

const bdecs_PackedCalendar::WeekendDaysTransition& getEmptyTransition()
    // Return a reference providing non-modifiable access to a weekend-days
    // transition having a start date of 1/1/1 and an empty set of weekend
    // days.
{
    static bdecs_PackedCalendar::WeekendDaysTransition
                             transition(bdet_Date(1,1,1), bdec_DayOfWeekSet());
    return transition;
}


}  // close unnamed namespace

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


    WeekendDaysTransitionSequence newWeekendDaysTransitions(
                               this->d_weekendDaysTransitions.get_allocator());
    newWeekendDaysTransitions.reserve(
      d_weekendDaysTransitions.size() + other.d_weekendDaysTransitions.size());
    intersectWeekendDaysTransitions(&newWeekendDaysTransitions,
                                    d_weekendDaysTransitions,
                                    other.d_weekendDaysTransitions);

    // bdec_DayOfWeekSet oldWeekendDays = d_weekendDays;
    // d_weekendDays &= other.d_weekendDays;

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
             || false == other.isWeekendDay(h)) {
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
             || false == this->isWeekendDay(h)) {
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
    d_weekendDaysTransitions.swap(newWeekendDaysTransitions);
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

    WeekendDaysTransitionSequence newWeekendDaysTransitions(
                               this->d_weekendDaysTransitions.get_allocator());
    newWeekendDaysTransitions.reserve(
      d_weekendDaysTransitions.size() + other.d_weekendDaysTransitions.size());
    unionWeekendDaysTransitions(&newWeekendDaysTransitions,
                                d_weekendDaysTransitions,
                                other.d_weekendDaysTransitions);
    d_weekendDaysTransitions.swap(newWeekendDaysTransitions);

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
, d_weekendDaysTransitions(basicAllocator)
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
, d_weekendDaysTransitions(basicAllocator)
{
}

bdecs_PackedCalendar::bdecs_PackedCalendar(
                                   const bdecs_PackedCalendar&  original,
                                   bslma_Allocator             *basicAllocator)
: d_firstDate(original.d_firstDate)
, d_lastDate(original.d_lastDate)
, d_holidayOffsets(original.d_holidayOffsets, basicAllocator)
, d_holidayCodesIndex(original.d_holidayCodesIndex, basicAllocator)
, d_holidayCodes(original.d_holidayCodes, basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_weekendDaysTransitions(original.d_weekendDaysTransitions, basicAllocator)
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

    bslalg_SwapUtil::swap(&d_firstDate,         &other.d_firstDate);
    bslalg_SwapUtil::swap(&d_lastDate,          &other.d_lastDate);
    bslalg_SwapUtil::swap(&d_holidayOffsets,    &other.d_holidayOffsets);
    bslalg_SwapUtil::swap(&d_holidayCodesIndex, &other.d_holidayCodesIndex);
    bslalg_SwapUtil::swap(&d_holidayCodes,      &other.d_holidayCodes);
    bslalg_SwapUtil::swap(&d_weekendDaysTransitions,
                          &other.d_weekendDaysTransitions);
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
    d_holidayOffsets.clear();
    d_holidayCodesIndex.clear();
    d_holidayCodes.clear();
    d_weekendDaysTransitions.clear();
}

void bdecs_PackedCalendar::setValidRange(const bdet_Date& firstDate,
                                         const bdet_Date& lastDate)
{
    if (firstDate > lastDate) {
        WeekendDaysTransitionSequence weekendDaysTransitions(
                                     d_weekendDaysTransitions.get_allocator());
        d_weekendDaysTransitions.swap(weekendDaysTransitions);
        removeAll();
        d_weekendDaysTransitions.swap(weekendDaysTransitions);

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

    stream << "[ ";
    WeekendDaysTransitionConstIterator itr = beginWeekendDaysTransitions();
    while(itr != endWeekendDaysTransitions())
    {
        stream << itr->first << " : ";
        itr->second.print(stream, -1, -1);
        if (++itr != endWeekendDaysTransitions()) {
            stream << ", ";
        }
    }
    stream << " ]" << NL;  // extra space for we always have one transition at
                           // 1/1/1

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
    if (d_weekendDaysTransitions.empty()) {
        return 0;
    }

    BSLS_ASSERT_SAFE(d_weekendDaysTransitions[0].first == bdet_Date(1,1,1));

    // Find the first transition that has a date less than or equal to the
    // first date of calendar.

    bdec_DayOfWeekSet dummySet;
    WeekendDaysTransitionSequence::const_iterator itr =
                            bsl::upper_bound(d_weekendDaysTransitions.begin(),
                                             d_weekendDaysTransitions.end(),
                                             WeekendDaysTransition(d_firstDate,
                                                                   dummySet),
                                             WeekendDaysTransitionLess());
    --itr;

    int numWeekendDays = 0;

    bdet_Date firstDate = d_firstDate;
    do {
        const bdec_DayOfWeekSet& weekendDays = itr->second;
        bdet_Date endDate;

        ++itr;
        if (itr != d_weekendDaysTransitions.end() &&
                                                    itr->first <= d_lastDate) {
            endDate = itr->first;
        }
        else {
            endDate = d_lastDate + 1;
        }

        numWeekendDays +=
                     numWeekendDaysInRangeImp(firstDate, endDate, weekendDays);

        firstDate = itr->first;
    } while (itr != d_weekendDaysTransitions.end() &&
                                                     itr->first <= d_lastDate);

    return numWeekendDays;
}

// FREE OPERATORS
bool operator==(const bdecs_PackedCalendar& lhs,
                const bdecs_PackedCalendar& rhs)
{
    if (!(1 == lhs.numWeekendDaysTransitions() &&
          1 == rhs.numWeekendDaysTransitions() &&
          *lhs.beginWeekendDaysTransitions() ==
                                         *rhs.beginWeekendDaysTransitions())) {
        if (lhs.d_weekendDaysTransitions != rhs.d_weekendDaysTransitions) {
            return false;
        }
    }

    return lhs.d_firstDate         == rhs.d_firstDate
        && lhs.d_lastDate          == rhs.d_lastDate
        && lhs.d_holidayOffsets    == rhs.d_holidayOffsets
        && lhs.d_holidayCodesIndex == rhs.d_holidayCodesIndex
        && lhs.d_holidayCodes      == rhs.d_holidayCodes;

}


// FREE OPERATORS
bool operator!=(const bdecs_PackedCalendar& lhs,
                const bdecs_PackedCalendar& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream& operator<<(bsl::ostream&               stream,
                         const bdecs_PackedCalendar& calendar)
{
    calendar.print(stream, 0, -1);
    return stream;
}

                // ---------------------------------------------------
                // class bdecs_PackedCalendar_BusinessDayConstIterator
                // ---------------------------------------------------

// PRIVATE MANIPULATORS
void bdecs_PackedCalendar_BusinessDayConstIterator::nextBusinessDay()
{
    BSLS_ASSERT(false == d_endFlag);

    const int lastOffset =
                          d_calendar_p->lastDate() - d_calendar_p->firstDate();

    while (d_currentOffset < lastOffset) {
        ++d_currentOffset;

        if (d_offsetIter == d_calendar_p->d_holidayOffsets.end()
         || d_currentOffset < *d_offsetIter) {
            if (!d_calendar_p->isWeekendDay(
                                d_calendar_p->firstDate() + d_currentOffset)) {

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

    while (d_currentOffset >= 0) {
        if (false == d_endFlag) {
            --d_currentOffset;
        }
        else {
            d_endFlag = false;
        }

        if (d_offsetIter != d_calendar_p->d_holidayOffsets.begin()) {
            OffsetsConstIterator iter = d_offsetIter;
            --iter;
            if (d_currentOffset > *iter) {
                if (!d_calendar_p->isWeekendDay(
                                d_calendar_p->firstDate() + d_currentOffset)) {

                    // We found the previous business day.

                    return;
                }
            }
            else {
                --d_offsetIter;
            }

        }
        else if (!d_calendar_p->isWeekendDay(
                                d_calendar_p->firstDate() + d_currentOffset)) {

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

    if (calendar.isWeekendDay(calendar.firstDate() + d_currentOffset)) {
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
    d_currentOffset = rhs.d_currentOffset;
    d_endFlag       = rhs.d_endFlag;

    return *this;
}

void bdecs_PackedCalendar::addWeekendDay(bdet_DayOfWeek::Day weekendDay)
{
    BSLS_ASSERT(d_weekendDaysTransitions.size() <= 1);

    if (d_weekendDaysTransitions.empty()) {
        bdec_DayOfWeekSet weekendDays;
        weekendDays.add(weekendDay);
        d_weekendDaysTransitions.push_back(
                         WeekendDaysTransition(bdet_Date(1,1,1), weekendDays));
    }
    else {
        BSLS_ASSERT(d_weekendDaysTransitions[0].first == bdet_Date(1,1,1));

        d_weekendDaysTransitions[0].second.add(weekendDay);
    }
}

void bdecs_PackedCalendar::addWeekendDays(const bdec_DayOfWeekSet& weekendDays)
{
    BSLS_ASSERT(d_weekendDaysTransitions.size() <= 1);

    if (d_weekendDaysTransitions.empty()) {
        d_weekendDaysTransitions.push_back(
                         WeekendDaysTransition(bdet_Date(1,1,1), weekendDays));
    }
    else {
        BSLS_ASSERT(d_weekendDaysTransitions[0].first == bdet_Date(1,1,1));

        d_weekendDaysTransitions[0].second |= weekendDays;
    }
}

void bdecs_PackedCalendar::addWeekendDaysTransition(
                                          const bdet_Date& date,
                                          const bdec_DayOfWeekSet& weekendDays)
{
    if (d_weekendDaysTransitions.empty()) {
        d_weekendDaysTransitions.push_back(
                                   WeekendDaysTransition(bdet_Date(1,1,1),
                                                         bdec_DayOfWeekSet()));
    }

    WeekendDaysTransition newTransition(date, weekendDays);

    WeekendDaysTransitionSequence::iterator it =
                             bsl::lower_bound(d_weekendDaysTransitions.begin(),
                                              d_weekendDaysTransitions.end(),
                                              newTransition,
                                              WeekendDaysTransitionLess());

    if (it != d_weekendDaysTransitions.end() && it->first == date) {
        it->second = weekendDays;
    }
    else {
        d_weekendDaysTransitions.insert(it, newTransition);
    }
}

// ACCESSORS
bool bdecs_PackedCalendar::isWeekendDay(const bdet_Date& date) const
{
    if (d_weekendDaysTransitions.empty()) {
        return false;
    }

    bdec_DayOfWeekSet dummySet;

    WeekendDaysTransitionSequence::const_iterator it =
                        bsl::upper_bound(d_weekendDaysTransitions.begin(),
                                         d_weekendDaysTransitions.end(),
                                         WeekendDaysTransition(date, dummySet),
                                         WeekendDaysTransitionLess());

    if (it == d_weekendDaysTransitions.begin()) {

        // The specified 'date' is prior to the first weekend-days transition
        // (this shouldn't happen because the default transition is always at
        // 1/1/1).

        return false;
    }
    else {
        --it;
        return it->second.isMember(date.dayOfWeek());
    }
}

            // --------------------------------------------------------------
            // class bdecs_PackedCalendar_WeekendDaysTransitionConstIterator
            // --------------------------------------------------------------

// PRIVATE CREATORS
bdecs_PackedCalendar_WeekendDaysTransitionConstIterator::
bdecs_PackedCalendar_WeekendDaysTransitionConstIterator(
                                          const bdecs_PackedCalendar& calendar,
                                          bool                        endFlag)
: d_calendar_p(&calendar)
, d_implicitTransitionEndFlag(endFlag)
{
    if (endFlag) {
        d_position = d_calendar_p->d_weekendDaysTransitions.end();
    }
    else {
        d_position = d_calendar_p->d_weekendDaysTransitions.begin();
    }
}

// PRIVATE ACCESSORS
bool
bdecs_PackedCalendar_WeekendDaysTransitionConstIterator::
                                                     implicitTransition() const
{
    return d_calendar_p->d_weekendDaysTransitions.empty();
}

bdecs_PackedCalendar_WeekendDaysTransitionConstIterator&
bdecs_PackedCalendar_WeekendDaysTransitionConstIterator::operator++()
{
    if (implicitTransition() && !d_implicitTransitionEndFlag) {
        d_implicitTransitionEndFlag = true;
    }
    else {
        ++d_position;
    }
    return *this;
}

bdecs_PackedCalendar_WeekendDaysTransitionConstIterator&
bdecs_PackedCalendar_WeekendDaysTransitionConstIterator::operator--()
{
    if (implicitTransition() && d_implicitTransitionEndFlag) {
        d_implicitTransitionEndFlag = false;
    }
    else {
        --d_position;
    }
    return *this;
}

const bdecs_PackedCalendar::WeekendDaysTransition&
bdecs_PackedCalendar_WeekendDaysTransitionConstIterator::operator*() const
{
    if (implicitTransition() && !d_implicitTransitionEndFlag) {
        return getEmptyTransition();
    }
    return *d_position;
}

const bdecs_PackedCalendar::WeekendDaysTransition*
bdecs_PackedCalendar_WeekendDaysTransitionConstIterator::operator->() const
{
    if (implicitTransition() && !d_implicitTransitionEndFlag) {
        return &(getEmptyTransition());
    }
    return &(*d_position);
}

// FREE OPERATORS

bool operator==(
            const bdecs_PackedCalendar_WeekendDaysTransitionConstIterator& lhs,
            const bdecs_PackedCalendar_WeekendDaysTransitionConstIterator& rhs)
{
    if (lhs.d_calendar_p != rhs.d_calendar_p) {
        return false;
    }

    if (lhs.implicitTransition())
    {
        return lhs.d_implicitTransitionEndFlag ==
               rhs.d_implicitTransitionEndFlag &&
               lhs.d_position == rhs.d_position;
    }

    return lhs.d_position == rhs.d_position;
}

bool operator!=(
            const bdecs_PackedCalendar_WeekendDaysTransitionConstIterator& lhs,
            const bdecs_PackedCalendar_WeekendDaysTransitionConstIterator& rhs)
{
    return !(lhs == rhs);
}

bdecs_PackedCalendar_WeekendDaysTransitionConstIterator
operator++(bdecs_PackedCalendar_WeekendDaysTransitionConstIterator& iter,
           int)
{
    bdecs_PackedCalendar_WeekendDaysTransitionConstIterator temp = iter;
    ++iter;
    return temp;
}

bdecs_PackedCalendar_WeekendDaysTransitionConstIterator
operator--(bdecs_PackedCalendar_WeekendDaysTransitionConstIterator& iter,
           int)
{
    bdecs_PackedCalendar_WeekendDaysTransitionConstIterator temp = iter;
    --iter;
    return temp;
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
