// bdlt_packedcalendar.cpp                                            -*-C++-*-
#include <bdlt_packedcalendar.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_packedcalendar_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bslma_default.h>

#include <bsls_performancehint.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdlt {

// HELPER FUNCTION DATA
static
const unsigned char s_partialWeeks[8][7] =
    // The static 's_partialWeeks' array contains all the possible (43)
    // contiguous partial weeks:
    // [SUN], [MON], ..., [SUN, MON], [TUE, WED], ..., [SUN, MON, TUE], ...
    // Each value in the table represents the days of the week that should be
    // considered for a particular partial week.  Array values use the same
    // bitwise representation as 'bdlt::DayOfWeekSet' (bits '1 .. 7' reflect
    // '[ SUN, MON, ..., SAT ]').  The row number indicates the starting day
    // of the week of that partial week, and the column number indicates the
    // length of that partial week.  Note that the 0th row and column are
    // intentionally left empty to enable a more natural indexing operation.
    // See 'numWeekendDaysInRangeImp' below.
 { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
   {0x00, 0x02, 0x06, 0x0E, 0x1E, 0x3E, 0x7E},
   {0x00, 0x04, 0x0C, 0x1C, 0x3C, 0x7C, 0xFC},
   {0x00, 0x08, 0x18, 0x38, 0x78, 0xF8, 0xFA},
   {0x00, 0x10, 0x30, 0x70, 0xF0, 0xF2, 0xF6},
   {0x00, 0x20, 0x60, 0xE0, 0xE2, 0xE6, 0xEE},
   {0x00, 0x40, 0xC0, 0xC2, 0xC6, 0xCE, 0xDE},
   {0x00, 0x80, 0x82, 0x86, 0x8E, 0x9E, 0xBE} };

// HELPER FUNCTIONS
static
void addDayImp(Date                      *firstDate,
               Date                      *endDate,
               bdlc::PackedIntArray<int> *holidayOffsets,
               const bdlt::Date&          date)
    // Insert the specified 'date' into the range of the calendar object
    // represented by the specified 'firstDate', 'endDate', and
    // 'holidayOffsets'.  If the 'date' is outside the range of the calendar,
    // this range will be extended to include it and 'firstDate' or 'endDate'
    // will be appropriately modified.  Otherwise, this function has no effect.
{
    BSLS_ASSERT(firstDate);
    BSLS_ASSERT(endDate);
    BSLS_ASSERT(holidayOffsets);

    if (date < *firstDate) {
        const int delta = *firstDate - date;
        for (bsl::size_t i = 0; i < holidayOffsets->length(); ++i) {
            holidayOffsets->replace(i, holidayOffsets->operator[](i) + delta);
        }
        *firstDate = date;
    }
    if (date > *endDate) {
        *endDate = date;
    }
}

static
void appendHoliday(
                bdlc::PackedIntArray<int>                *resHolidayOffsets,
                bdlc::PackedIntArray<int>                *resHolidayCodesIndex,
                bdlc::PackedIntArray<int>                *resHolidayCodes,
                int                                       holidayOffset,
                PackedCalendar::HolidayCodeConstIterator  lhsHC,
                PackedCalendar::HolidayCodeConstIterator  lhsHCE)
    // Append, onto the specified 'resHolidayOffsets', 'resHolidayCodesIndex',
    // and 'resHolidayCodes', the holiday at the specified 'holidayOffset' with
    // the specified 'lhsHC .. lhsHCE' holiday codes.
{
    resHolidayOffsets->push_back(holidayOffset);
    resHolidayCodesIndex->push_back(
                                  static_cast<int>(resHolidayCodes->length()));
    while (lhsHC != lhsHCE) {
        resHolidayCodes->push_back(*lhsHC);
        ++lhsHC;
    }
}

static
void appendHoliday(
                bdlc::PackedIntArray<int>                *resHolidayOffsets,
                bdlc::PackedIntArray<int>                *resHolidayCodesIndex,
                bdlc::PackedIntArray<int>                *resHolidayCodes,
                int                                       holidayOffset,
                PackedCalendar::HolidayCodeConstIterator  lhsHC,
                PackedCalendar::HolidayCodeConstIterator  lhsHCE,
                PackedCalendar::HolidayCodeConstIterator  rhsHC,
                PackedCalendar::HolidayCodeConstIterator  rhsHCE)
    // Append, onto the specified 'resHolidayOffsets', 'resHolidayCodesIndex',
    // and 'resHolidayCodes', the holiday at the specified 'holidayOffset' with
    // the union of the specified 'lhsHC .. lhsHCE' holiday codes and the
    // specified 'rhsHC .. rhsHCE' holiday codes.
{
    resHolidayOffsets->push_back(holidayOffset);
    resHolidayCodesIndex->push_back(
                                  static_cast<int>(resHolidayCodes->length()));

    while (lhsHC != lhsHCE && rhsHC != rhsHCE) {
        if (*lhsHC < *rhsHC) {
            resHolidayCodes->push_back(*lhsHC);
            ++lhsHC;
        }
        else if (*lhsHC > *rhsHC) {
            resHolidayCodes->push_back(*rhsHC);
            ++rhsHC;
        }
        else {
            resHolidayCodes->push_back(*lhsHC);
            ++lhsHC;
            ++rhsHC;
        }
    }
    while (lhsHC != lhsHCE) {
        resHolidayCodes->push_back(*lhsHC);
        ++lhsHC;
    }
    while (rhsHC != rhsHCE) {
        resHolidayCodes->push_back(*rhsHC);
        ++rhsHC;
    }
}

static
bsl::ostream& indent(bsl::ostream& stream,
                     int           level,
                     int           spacesPerLevel)
    // Emit the number of spaces indicated by the absolute value of the
    // product of the specified 'level' and 'spacesPerLevel' to the specified
    // output 'stream' and return a reference to the modifiable 'stream'.  If
    // the 'level' is negative, this function has no effect.
{
    if (spacesPerLevel < 0) {
        spacesPerLevel = -spacesPerLevel;
    }

    int numSpaces = level * spacesPerLevel;

    static const char SPACES[]    = "                                        ";
    const int         SPACES_SIZE = sizeof(SPACES) - 1;

    while (SPACES_SIZE < numSpaces) {
        stream.write(SPACES, SPACES_SIZE);
        numSpaces -= SPACES_SIZE;
    }

    if (0 < numSpaces) {
        stream.write(SPACES, numSpaces);
    }
    return stream;
}

static
int numWeekendDaysInRangeImp(const Date&         firstDate,
                             const Date&         lastDate,
                             const DayOfWeekSet& weekendDays)
    // Return the number of days in the range starting from the specified
    // 'firstDate' to 'lastDate' whose day-of-week are in the specified
    // 'weekendDays' set.  Note that this function returns 0 if
    // 'lastDate < firstDate'.
{
    const int len = firstDate <= lastDate ? lastDate - firstDate + 1 : 0;

    // 'numWeekendDays' is the lower bound of the value we are interested in.
    const int numWeekendDays = (len / 7) * weekendDays.length();

    const int dayOfWeek = static_cast<int>(firstDate.dayOfWeek());

    // Get the (precomputed) partial week at the end of the range to be
    // considered.
    const DayOfWeekSet& myPartialWeek = *reinterpret_cast<const DayOfWeekSet*>(
                                        &(s_partialWeeks[dayOfWeek][len % 7]));
    const DayOfWeekSet  mySet = myPartialWeek & weekendDays;

    return numWeekendDays + mySet.length();
}

typedef bsl::vector<bsl::pair<Date, DayOfWeekSet> > WTransitions;

static
void intersectWeekendDaysTransitions(WTransitions        *result,
                                     const WTransitions&  lhs,
                                     const WTransitions&  rhs)
    // Load, into the specified 'result', the intersection of the specified
    // 'lhs' weekend-days transitions and the specified 'rhs' weekend-days
    // transitions.
{
    if (lhs.empty() || rhs.empty()) {
        return;                                                       // RETURN
    }

    WTransitions::const_iterator lhsWT = lhs.begin();
    WTransitions::const_iterator rhsWT = rhs.begin();

    bdlt::DayOfWeekSet lhsWeekends;
    bdlt::DayOfWeekSet rhsWeekends;

    while (lhsWT != lhs.end() || rhsWT != rhs.end()) {
        WTransitions::value_type val;
        if (lhsWT != lhs.end()
         && (rhsWT == rhs.end() || lhsWT->first < rhsWT->first)) {
            val.first = lhsWT->first;
            lhsWeekends = lhsWT->second;
            ++lhsWT;
        }
        else if (lhsWT == lhs.end() || lhsWT->first > rhsWT->first) {
            val.first = rhsWT->first;
            rhsWeekends = rhsWT->second;
            ++rhsWT;
        }
        else {
            val.first = lhsWT->first;
            lhsWeekends = lhsWT->second;
            rhsWeekends = rhsWT->second;
            ++lhsWT;
            ++rhsWT;
        }
        val.second = (lhsWeekends & rhsWeekends);
        if ((result->empty() && !val.second.isEmpty())
         || (!result->empty() && val.second != result->back().second)) {
            result->push_back(val);
        }
    }
}

static
void unionWeekendDaysTransitions(WTransitions        *result,
                                 const WTransitions&  lhs,
                                 const WTransitions&  rhs)
    // Load, into the specified 'result', the union of the specified 'lhs'
    // weekend-days transitions and the specified 'rhs' weekend-days
    // transitions.
{
    if (lhs.empty()) {
        *result = rhs;
        return;                                                       // RETURN
    }

    if (rhs.empty()) {
        *result = lhs;
        return;                                                       // RETURN
    }

    WTransitions::const_iterator lh = lhs.begin();
    WTransitions::const_iterator r  = rhs.begin();
    WTransitions::const_iterator lprev = lhs.end();
    WTransitions::const_iterator rprev = rhs.end();

    while (lh != lhs.end() && r != rhs.end()) {

        if (lh->first < r->first) {
            if (rprev == rhs.end()) {
                result->push_back(*lh);
            }
            else {
                WTransitions::value_type val = *lh;
                val.second |= rprev->second;
                result->push_back(val);
            }
            lprev = lh;
            ++lh;
        }
        else if (r->first < lh->first) {
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
            WTransitions::value_type val = *lh;
            val.second |= r->second;
            result->push_back(val);
            lprev = lh;
            rprev = r;
            ++lh;
            ++r;
        }
    }

    while (lh != lhs.end()) {
        WTransitions::value_type val = *lh;
        val.second |= rprev->second;
        result->push_back(val);
        ++lh;
    }

    while (r != rhs.end()) {
        WTransitions::value_type val = *r;
        val.second |= lprev->second;
        result->push_back(val);
        ++r;
    }
}

                          // --------------------
                          // class PackedCalendar
                          // --------------------

// PRIVATE CLASS METHODS
void PackedCalendar::intersectHolidays(
                               bdlc::PackedIntArray<int> *resHolidayOffsets,
                               bdlc::PackedIntArray<int> *resHolidayCodesIndex,
                               bdlc::PackedIntArray<int> *resHolidayCodes,
                               const PackedCalendar&      lhs,
                               const PackedCalendar&      rhs,
                               const bdlt::Date           firstDate,
                               const bdlt::Date           lastDate)
{
    BSLS_ASSERT(firstDate <= lastDate);

    OffsetsConstIterator iter;

    iter = bdlc::PackedIntArrayUtil::lowerBound(lhs.d_holidayOffsets.begin(),
                                                lhs.d_holidayOffsets.end(),
                                                firstDate - lhs.d_firstDate);
    PackedCalendar::HolidayConstIterator lhsH(iter, lhs.d_firstDate);

    iter = bdlc::PackedIntArrayUtil::upperBound(lhs.d_holidayOffsets.begin(),
                                                lhs.d_holidayOffsets.end(),
                                                lastDate - lhs.d_firstDate);
    PackedCalendar::HolidayConstIterator lhsHE(iter, lhs.d_firstDate);

    iter = bdlc::PackedIntArrayUtil::lowerBound(rhs.d_holidayOffsets.begin(),
                                                rhs.d_holidayOffsets.end(),
                                                firstDate - rhs.d_firstDate);
    PackedCalendar::HolidayConstIterator rhsH(iter, rhs.d_firstDate);

    iter = bdlc::PackedIntArrayUtil::upperBound(rhs.d_holidayOffsets.begin(),
                                                rhs.d_holidayOffsets.end(),
                                                lastDate - rhs.d_firstDate);
    PackedCalendar::HolidayConstIterator rhsHE(iter, rhs.d_firstDate);

    while (lhsH != lhsHE && rhsH != rhsHE) {
        if (*lhsH == *rhsH) {
            appendHoliday(resHolidayOffsets,
                          resHolidayCodesIndex,
                          resHolidayCodes,
                          *lhsH - firstDate,
                          lhs.beginHolidayCodes(lhsH),
                          lhs.endHolidayCodes(lhsH),
                          rhs.beginHolidayCodes(rhsH),
                          rhs.endHolidayCodes(rhsH));
            ++lhsH;
            ++rhsH;
        }
        else if (*lhsH < *rhsH) {
            ++lhsH;
        }
        else {
            ++rhsH;
        }
    }
}

void PackedCalendar::unionHolidays(
                               bdlc::PackedIntArray<int> *resHolidayOffsets,
                               bdlc::PackedIntArray<int> *resHolidayCodesIndex,
                               bdlc::PackedIntArray<int> *resHolidayCodes,
                               const PackedCalendar&      lhs,
                               const PackedCalendar&      rhs,
                               const bdlt::Date           firstDate,
                               const bdlt::Date           lastDate)
{
    BSLS_ASSERT(firstDate <= lastDate);

    OffsetsConstIterator iter;

    iter = bdlc::PackedIntArrayUtil::lowerBound(lhs.d_holidayOffsets.begin(),
                                                lhs.d_holidayOffsets.end(),
                                                firstDate - lhs.d_firstDate);
    PackedCalendar::HolidayConstIterator lhsH(iter, lhs.d_firstDate);

    iter = bdlc::PackedIntArrayUtil::upperBound(lhs.d_holidayOffsets.begin(),
                                                lhs.d_holidayOffsets.end(),
                                                lastDate - lhs.d_firstDate);
    PackedCalendar::HolidayConstIterator lhsHE(iter, lhs.d_firstDate);

    iter = bdlc::PackedIntArrayUtil::lowerBound(rhs.d_holidayOffsets.begin(),
                                                rhs.d_holidayOffsets.end(),
                                                firstDate - rhs.d_firstDate);
    PackedCalendar::HolidayConstIterator rhsH(iter, rhs.d_firstDate);

    iter = bdlc::PackedIntArrayUtil::upperBound(rhs.d_holidayOffsets.begin(),
                                                rhs.d_holidayOffsets.end(),
                                                lastDate - rhs.d_firstDate);
    PackedCalendar::HolidayConstIterator rhsHE(iter, rhs.d_firstDate);

    while (lhsH != lhsHE && rhsH != rhsHE) {
        if (*lhsH == *rhsH) {
            appendHoliday(resHolidayOffsets,
                          resHolidayCodesIndex,
                          resHolidayCodes,
                          *lhsH - firstDate,
                          lhs.beginHolidayCodes(lhsH),
                          lhs.endHolidayCodes(lhsH),
                          rhs.beginHolidayCodes(rhsH),
                          rhs.endHolidayCodes(rhsH));
            ++lhsH;
            ++rhsH;
        }
        else if (*lhsH < *rhsH) {
            appendHoliday(resHolidayOffsets,
                          resHolidayCodesIndex,
                          resHolidayCodes,
                          *lhsH - firstDate,
                          lhs.beginHolidayCodes(lhsH),
                          lhs.endHolidayCodes(lhsH));
            ++lhsH;
        }
        else {
            appendHoliday(resHolidayOffsets,
                          resHolidayCodesIndex,
                          resHolidayCodes,
                          *rhsH - firstDate,
                          rhs.beginHolidayCodes(rhsH),
                          rhs.endHolidayCodes(rhsH));
            ++rhsH;
        }
    }

    while (lhsH != lhsHE) {
        appendHoliday(resHolidayOffsets,
                      resHolidayCodesIndex,
                      resHolidayCodes,
                      *lhsH - firstDate,
                      lhs.beginHolidayCodes(lhsH),
                      lhs.endHolidayCodes(lhsH));
        ++lhsH;
    }

    while (rhsH != rhsHE) {
        appendHoliday(resHolidayOffsets,
                      resHolidayCodesIndex,
                      resHolidayCodes,
                      *rhsH - firstDate,
                      rhs.beginHolidayCodes(rhsH),
                      rhs.endHolidayCodes(rhsH));
        ++rhsH;
    }
}

// PRIVATE MANIPULATORS
int PackedCalendar::addHolidayImp(int offset)
{
    BSLS_ASSERT(0 <= offset && d_lastDate - d_firstDate >= offset);

    // Insertion must be in linear time if the holidays are added in-order.

    const int len = static_cast<int>(d_holidayOffsets.length());

    if (0 == len) {
        d_holidayOffsets.reserveCapacity(1);
        d_holidayCodesIndex.reserveCapacity(1);
        d_holidayOffsets.push_back(offset);
        d_holidayCodesIndex.push_back(0);
        return 0;                                                     // RETURN
    }
    if (d_holidayOffsets.back() < offset) {
        d_holidayOffsets.reserveCapacity(len + 1);
        d_holidayCodesIndex.reserveCapacity(len + 1);
        d_holidayOffsets.push_back(offset);
        d_holidayCodesIndex.push_back(
                                    static_cast<int>(d_holidayCodes.length()));
        return len;                                                   // RETURN
    }

    OffsetsConstIterator it = bdlc::PackedIntArrayUtil::lowerBound(
                                                      d_holidayOffsets.begin(),
                                                      d_holidayOffsets.end(),
                                                      offset);

    BSLS_ASSERT(it != d_holidayOffsets.end());

    if (offset != *it) {
        const bsl::ptrdiff_t shift = it - d_holidayOffsets.begin();
        d_holidayOffsets.reserveCapacity(len + 1);
        d_holidayCodesIndex.reserveCapacity(len + 1);

        // The calls to 'reserveCapacity' invalidated 'it'.

        it = d_holidayOffsets.begin();
        it += shift;
        it = d_holidayOffsets.insert(it, offset);

        CodesIndexConstIterator holidayCodesIndexIter
                                                 = d_holidayCodesIndex.begin();
        holidayCodesIndexIter += shift;
        d_holidayCodesIndex.insert(holidayCodesIndexIter,
                                   *holidayCodesIndexIter);
    }
    return static_cast<int>(it - d_holidayOffsets.begin());
}

// CREATORS
PackedCalendar::PackedCalendar(bslma::Allocator *basicAllocator)
: d_firstDate(9999, 12, 31)
, d_lastDate(1, 1, 1)
, d_weekendDaysTransitions(basicAllocator)
, d_holidayOffsets(basicAllocator)
, d_holidayCodesIndex(basicAllocator)
, d_holidayCodes(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

PackedCalendar::PackedCalendar(const Date&       firstDate,
                               const Date&       lastDate,
                               bslma::Allocator *basicAllocator)
: d_firstDate(firstDate)
, d_lastDate(lastDate)
, d_weekendDaysTransitions(basicAllocator)
, d_holidayOffsets(basicAllocator)
, d_holidayCodesIndex(basicAllocator)
, d_holidayCodes(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(firstDate <= lastDate);
}

PackedCalendar::PackedCalendar(const PackedCalendar&  original,
                               bslma::Allocator      *basicAllocator)
: d_firstDate(original.d_firstDate)
, d_lastDate(original.d_lastDate)
, d_weekendDaysTransitions(original.d_weekendDaysTransitions, basicAllocator)
, d_holidayOffsets(original.d_holidayOffsets, basicAllocator)
, d_holidayCodesIndex(original.d_holidayCodesIndex, basicAllocator)
, d_holidayCodes(original.d_holidayCodes, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

PackedCalendar::~PackedCalendar()
{
    BSLS_ASSERT(d_holidayOffsets.length() == d_holidayCodesIndex.length());
    BSLS_ASSERT(d_holidayOffsets.isEmpty()
             || d_holidayCodesIndex.back() <=
                                    static_cast<int>(d_holidayCodes.length()));
    BSLS_ASSERT(!d_holidayOffsets.isEmpty() || d_holidayCodes.isEmpty());
}

// MANIPULATORS
PackedCalendar& PackedCalendar::operator=(const PackedCalendar& rhs)
{
    PackedCalendar(rhs, d_allocator_p).swap(*this);
    return *this;
}

void PackedCalendar::addDay(const Date& date)
{
    addDayImp(&d_firstDate, &d_lastDate, &d_holidayOffsets, date);
}

void PackedCalendar::addHoliday(const Date& date)
{
    addDayImp(&d_firstDate, &d_lastDate, &d_holidayOffsets, date);
    addHolidayImp(date - d_firstDate);

    BSLS_ASSERT(d_holidayOffsets.length() == d_holidayCodesIndex.length());
}

void PackedCalendar::addHolidayCode(const Date& date, int holidayCode)
{
    addDayImp(&d_firstDate, &d_lastDate, &d_holidayOffsets, date);

    const int                  index   = addHolidayImp(date - d_firstDate);
    const OffsetsConstIterator holiday = d_holidayOffsets.begin() + index;
    const CodesConstIterator   b       = beginHolidayCodes(holiday);
    const CodesConstIterator   e       = endHolidayCodes(holiday);

    CodesConstIterator it =
                      bdlc::PackedIntArrayUtil::lowerBound(b, e, holidayCode);
    if (it == e || holidayCode != *it) {
        it = d_holidayCodes.insert(it, holidayCode);

        // Since we inserted a code in the codes vectors, all the indexes for
        // the holidays following 'date' will be off by one; let's fix that.

        for (bsl::size_t i = index + 1;
             i < d_holidayCodesIndex.length();
             ++i) {
            d_holidayCodesIndex.replace(i, d_holidayCodesIndex[i] + 1);
        }
    }

    BSLS_ASSERT(d_holidayOffsets.length() == d_holidayCodesIndex.length());
    BSLS_ASSERT(d_holidayOffsets.isEmpty()
             || d_holidayCodesIndex.back() <=
                                    static_cast<int>(d_holidayCodes.length()));
}

void PackedCalendar::addWeekendDay(DayOfWeek::Enum weekendDay)
{
    BSLS_ASSERT(d_weekendDaysTransitions.empty()
                || (1           == d_weekendDaysTransitions.size() &&
                    Date(1,1,1) == d_weekendDaysTransitions[0].first));

    if (d_weekendDaysTransitions.empty()) {
        DayOfWeekSet weekendDays;
        weekendDays.add(weekendDay);
        d_weekendDaysTransitions.push_back(
                              WeekendDaysTransition(Date(1,1,1), weekendDays));
    }
    else {
        d_weekendDaysTransitions[0].second.add(weekendDay);
    }
}

void PackedCalendar::addWeekendDays(const DayOfWeekSet& weekendDays)
{
    BSLS_ASSERT(d_weekendDaysTransitions.empty()
                || (1           == d_weekendDaysTransitions.size() &&
                    Date(1,1,1) == d_weekendDaysTransitions[0].first));

    if (d_weekendDaysTransitions.empty()) {
        d_weekendDaysTransitions.push_back(
                         WeekendDaysTransition(Date(1,1,1), weekendDays));
    }
    else {
        d_weekendDaysTransitions[0].second |= weekendDays;
    }
}

void PackedCalendar::addWeekendDaysTransition(const Date&         startDate,
                                              const DayOfWeekSet& weekendDays)
{
    WeekendDaysTransition newTransition(startDate, weekendDays);

    WeekendDaysTransitionSequence::iterator it =
                             bsl::lower_bound(d_weekendDaysTransitions.begin(),
                                              d_weekendDaysTransitions.end(),
                                              newTransition,
                                              WeekendDaysTransitionLess());

    if (it != d_weekendDaysTransitions.end() && it->first == startDate) {
        it->second = weekendDays;
    }
    else {
        d_weekendDaysTransitions.insert(it, newTransition);
    }
}

void PackedCalendar::intersectBusinessDays(const PackedCalendar& other)
{
    bdlt::Date firstDate = d_firstDate >= other.d_firstDate
                           ? d_firstDate
                           : other.d_firstDate;
    bdlt::Date lastDate  = d_lastDate <= other.d_lastDate
                           ? d_lastDate
                           : other.d_lastDate;

    bsl::vector<WeekendDaysTransition> weekendDaysTransitions(d_allocator_p);
    bdlc::PackedIntArray<int>          holidayOffsets(d_allocator_p);
    bdlc::PackedIntArray<int>          holidayCodesIndex(d_allocator_p);
    bdlc::PackedIntArray<int>          holidayCodes(d_allocator_p);

    unionWeekendDaysTransitions(&weekendDaysTransitions,
                                d_weekendDaysTransitions,
                                other.d_weekendDaysTransitions);

    if (firstDate <= lastDate) {
        unionHolidays(&holidayOffsets,
                      &holidayCodesIndex,
                      &holidayCodes,
                      *this,
                      other,
                      firstDate,
                      lastDate);
    }
    else {
        firstDate = Date(9999, 12, 31);
        lastDate  = Date(   1,  1,  1);
    }

    bslalg::SwapUtil::swap(&d_firstDate,         &firstDate);
    bslalg::SwapUtil::swap(&d_lastDate,          &lastDate);
    bslalg::SwapUtil::swap(&d_weekendDaysTransitions,
                                                 &weekendDaysTransitions);
    bslalg::SwapUtil::swap(&d_holidayOffsets,    &holidayOffsets);
    bslalg::SwapUtil::swap(&d_holidayCodesIndex, &holidayCodesIndex);
    bslalg::SwapUtil::swap(&d_holidayCodes,      &holidayCodes);
}

void PackedCalendar::intersectNonBusinessDays(const PackedCalendar& other)
{
    bdlt::Date firstDate = d_firstDate >= other.d_firstDate
                           ? d_firstDate
                           : other.d_firstDate;
    bdlt::Date lastDate  = d_lastDate <= other.d_lastDate
                           ? d_lastDate
                           : other.d_lastDate;

    bsl::vector<WeekendDaysTransition> weekendDaysTransitions(d_allocator_p);
    bdlc::PackedIntArray<int>          holidayOffsets(d_allocator_p);
    bdlc::PackedIntArray<int>          holidayCodesIndex(d_allocator_p);
    bdlc::PackedIntArray<int>          holidayCodes(d_allocator_p);

    intersectWeekendDaysTransitions(&weekendDaysTransitions,
                                    d_weekendDaysTransitions,
                                    other.d_weekendDaysTransitions);

    if (firstDate <= lastDate) {
        intersectHolidays(&holidayOffsets,
                          &holidayCodesIndex,
                          &holidayCodes,
                          *this,
                          other,
                          firstDate,
                          lastDate);
    }
    else {
        firstDate = Date(9999, 12, 31);
        lastDate  = Date(   1,  1,  1);
    }

    bslalg::SwapUtil::swap(&d_firstDate,         &firstDate);
    bslalg::SwapUtil::swap(&d_lastDate,          &lastDate);
    bslalg::SwapUtil::swap(&d_weekendDaysTransitions,
                                                 &weekendDaysTransitions);
    bslalg::SwapUtil::swap(&d_holidayOffsets,    &holidayOffsets);
    bslalg::SwapUtil::swap(&d_holidayCodesIndex, &holidayCodesIndex);
    bslalg::SwapUtil::swap(&d_holidayCodes,      &holidayCodes);
}

void PackedCalendar::removeAll()
{
    d_firstDate.setYearMonthDay(9999, 12, 31);
    d_lastDate.setYearMonthDay(1, 1, 1);
    d_weekendDaysTransitions.clear();
    d_holidayOffsets.removeAll();
    d_holidayCodesIndex.removeAll();
    d_holidayCodes.removeAll();
}

void PackedCalendar::removeHoliday(const Date& date)
{
    const int offset = date - d_firstDate;

    const OffsetsConstIterator oit = bdlc::PackedIntArrayUtil::lowerBound(
                                                      d_holidayOffsets.begin(),
                                                      d_holidayOffsets.end(),
                                                      offset);

    if (oit != d_holidayOffsets.end() && *oit == offset) {
        const CodesConstIterator b = beginHolidayCodes(oit);
        const CodesConstIterator e = endHolidayCodes(oit);

        d_holidayCodes.remove(b, e);

        const bsl::ptrdiff_t offset = oit - d_holidayOffsets.begin();

        const bsl::ptrdiff_t delta = e - b;
        for (bsl::size_t i = offset + 1;
             i < d_holidayCodesIndex.length();
             ++i) {
            d_holidayCodesIndex.replace(
                          i, static_cast<int>(d_holidayCodesIndex[i] - delta));
        }

        CodesIndexConstIterator cit = d_holidayCodesIndex.begin() + offset;

        d_holidayCodesIndex.remove(cit, cit + 1);
        d_holidayOffsets.remove(oit, oit + 1);
    }
    BSLS_ASSERT(d_holidayOffsets.length() == d_holidayCodesIndex.length());
    BSLS_ASSERT(d_holidayOffsets.isEmpty()
             || d_holidayCodesIndex.back() <=
                                    static_cast<int>(d_holidayCodes.length()));
}

void PackedCalendar::removeHolidayCode(const Date& date, int holidayCode)
{
    const int offset = date - d_firstDate;

    const OffsetsConstIterator oit = bdlc::PackedIntArrayUtil::lowerBound(
                                                      d_holidayOffsets.begin(),
                                                      d_holidayOffsets.end(),
                                                      offset);

    if (oit != d_holidayOffsets.end() && *oit == offset) {
        const CodesConstIterator b   = beginHolidayCodes(oit);
        const CodesConstIterator e   = endHolidayCodes(oit);
        const CodesConstIterator cit =
                       bdlc::PackedIntArrayUtil::lowerBound(b, e, holidayCode);

        if (cit != e && *cit == holidayCode) {
            d_holidayCodes.remove(cit, cit + 1);
            const bsl::size_t shift = oit - d_holidayOffsets.begin() + 1;
            for (bsl::size_t i = shift;
                 i < d_holidayCodesIndex.length();
                 ++i) {
                d_holidayCodesIndex.replace(i, d_holidayCodesIndex[i] - 1);
            }
        }
    }

    BSLS_ASSERT(d_holidayOffsets.length() == d_holidayCodesIndex.length());
    BSLS_ASSERT(d_holidayOffsets.isEmpty()
             || d_holidayCodesIndex.back() <=
                                    static_cast<int>(d_holidayCodes.length()));
}

void PackedCalendar::setValidRange(const Date& firstDate, const Date& lastDate)
{
    BSLS_ASSERT(firstDate <= lastDate);

    if (lastDate < d_firstDate || firstDate > d_lastDate) {
        d_holidayOffsets.removeAll();
        d_holidayCodesIndex.removeAll();
        d_holidayCodes.removeAll();
        d_firstDate = firstDate;
        d_lastDate = lastDate;

        return;                                                       // RETURN
    }

    OffsetsConstIterator b  = d_holidayOffsets.begin();
    OffsetsConstIterator e  = d_holidayOffsets.end();
    OffsetsConstIterator it =
              bdlc::PackedIntArrayUtil::lowerBound(b,
                                                   e,
                                                   lastDate - d_firstDate + 1);

    if (it != e) {
        BSLS_ASSERT(lastDate <= d_lastDate);

        CodesIndexConstIterator
                            holidayCodesIndexIter(d_holidayCodesIndex.begin());
        holidayCodesIndexIter += (it - b);
        CodesConstIterator holidayCodesIter(d_holidayCodes.begin());
        holidayCodesIter += *holidayCodesIndexIter;
        d_holidayCodes.remove(holidayCodesIter, d_holidayCodes.end());
        d_holidayCodesIndex.remove(holidayCodesIndexIter,
                                   d_holidayCodesIndex.end());
        d_holidayOffsets.remove(it, d_holidayOffsets.end());
    }
    d_lastDate = lastDate;

    if (d_firstDate == firstDate) {
        return;                                                       // RETURN
    }

    if (d_firstDate < firstDate) {
        b = d_holidayOffsets.begin();
        e = d_holidayOffsets.end();

        // We're looking for all holidays lower and up to
        // 'firstDate - d_firstDate - 1'.  'upperBound' will return an iterator
        // one past the last element we want to remove.

        it = bdlc::PackedIntArrayUtil::
                                 upperBound(b, e, firstDate - d_firstDate - 1);
        CodesIndexConstIterator holidayCodesIndexIter =
                                                   d_holidayCodesIndex.begin();
        holidayCodesIndexIter += (it - b);
        CodesConstIterator holidayCodesIter = d_holidayCodes.begin();
        holidayCodesIter += (it != e)
                            ? *holidayCodesIndexIter
                            : d_holidayCodes.length();

        d_holidayCodes.remove(d_holidayCodes.begin(), holidayCodesIter);
        d_holidayCodesIndex.remove(d_holidayCodesIndex.begin(),
                                   holidayCodesIndexIter);
        d_holidayOffsets.remove(d_holidayOffsets.begin(), it);
    }

    if (d_holidayCodesIndex.begin() != d_holidayCodesIndex.end()) {
        const bsl::size_t adjustment = *d_holidayCodesIndex.begin();
        if (adjustment != 0) {
            for (bsl::size_t i = 0; i < d_holidayCodesIndex.length(); ++i) {
                d_holidayCodesIndex.replace(
                     i, static_cast<int>(d_holidayCodesIndex[i] - adjustment));
            }
        }
    }

    if (firstDate != d_firstDate
        && d_holidayOffsets.begin() != d_holidayOffsets.end()) {
        const int adjustment = firstDate - d_firstDate;
        for (bsl::size_t i = 0; i < d_holidayOffsets.length(); ++i) {
            d_holidayOffsets.replace(i, d_holidayOffsets[i] - adjustment);
        }
    }

    d_firstDate = firstDate;

    BSLS_ASSERT((int)d_holidayOffsets.length() <= (lastDate - firstDate + 1));
    BSLS_ASSERT(d_holidayOffsets.length() == d_holidayCodesIndex.length());
    BSLS_ASSERT(d_holidayOffsets.isEmpty()
             || d_holidayCodesIndex.back() <=
                                    static_cast<int>(d_holidayCodes.length()));
}

void PackedCalendar::unionBusinessDays(const PackedCalendar& other)
{
    bdlt::Date firstDate = d_firstDate <= other.d_firstDate
                           ? d_firstDate
                           : other.d_firstDate;
    bdlt::Date lastDate  = d_lastDate >= other.d_lastDate
                           ? d_lastDate
                           : other.d_lastDate;

    bsl::vector<WeekendDaysTransition> weekendDaysTransitions(d_allocator_p);
    bdlc::PackedIntArray<int>          holidayOffsets(d_allocator_p);
    bdlc::PackedIntArray<int>          holidayCodesIndex(d_allocator_p);
    bdlc::PackedIntArray<int>          holidayCodes(d_allocator_p);

    intersectWeekendDaysTransitions(&weekendDaysTransitions,
                                    d_weekendDaysTransitions,
                                    other.d_weekendDaysTransitions);

    if (firstDate <= lastDate) {
        intersectHolidays(&holidayOffsets,
                          &holidayCodesIndex,
                          &holidayCodes,
                          *this,
                          other,
                          firstDate,
                          lastDate);
    }
    else {
        firstDate = Date(9999, 12, 31);
        lastDate  = Date(   1,  1,  1);
    }

    bslalg::SwapUtil::swap(&d_firstDate,         &firstDate);
    bslalg::SwapUtil::swap(&d_lastDate,          &lastDate);
    bslalg::SwapUtil::swap(&d_weekendDaysTransitions,
                                                 &weekendDaysTransitions);
    bslalg::SwapUtil::swap(&d_holidayOffsets,    &holidayOffsets);
    bslalg::SwapUtil::swap(&d_holidayCodesIndex, &holidayCodesIndex);
    bslalg::SwapUtil::swap(&d_holidayCodes,      &holidayCodes);
}

void
PackedCalendar::unionNonBusinessDays(const PackedCalendar& other)
{
    bdlt::Date firstDate = d_firstDate <= other.d_firstDate
                           ? d_firstDate
                           : other.d_firstDate;
    bdlt::Date lastDate  = d_lastDate >= other.d_lastDate
                           ? d_lastDate
                           : other.d_lastDate;

    bsl::vector<WeekendDaysTransition> weekendDaysTransitions(d_allocator_p);
    bdlc::PackedIntArray<int>          holidayOffsets(d_allocator_p);
    bdlc::PackedIntArray<int>          holidayCodesIndex(d_allocator_p);
    bdlc::PackedIntArray<int>          holidayCodes(d_allocator_p);

    unionWeekendDaysTransitions(&weekendDaysTransitions,
                                d_weekendDaysTransitions,
                                other.d_weekendDaysTransitions);

    if (firstDate <= lastDate) {
        unionHolidays(&holidayOffsets,
                      &holidayCodesIndex,
                      &holidayCodes,
                      *this,
                      other,
                      firstDate,
                      lastDate);
    }
    else {
        firstDate = Date(9999, 12, 31);
        lastDate  = Date(   1,  1,  1);
    }

    bslalg::SwapUtil::swap(&d_firstDate,         &firstDate);
    bslalg::SwapUtil::swap(&d_lastDate,          &lastDate);
    bslalg::SwapUtil::swap(&d_weekendDaysTransitions,
                                                 &weekendDaysTransitions);
    bslalg::SwapUtil::swap(&d_holidayOffsets,    &holidayOffsets);
    bslalg::SwapUtil::swap(&d_holidayCodesIndex, &holidayCodesIndex);
    bslalg::SwapUtil::swap(&d_holidayCodes,      &holidayCodes);
}

                                  // Aspects

void PackedCalendar::swap(PackedCalendar& other)
{
    // 'swap' is undefined for objects with non-equal allocators.

    BSLS_ASSERT(d_allocator_p == other.d_allocator_p);

    bslalg::SwapUtil::swap(&d_firstDate,         &other.d_firstDate);
    bslalg::SwapUtil::swap(&d_lastDate,          &other.d_lastDate);
    bslalg::SwapUtil::swap(&d_weekendDaysTransitions,
                           &other.d_weekendDaysTransitions);
    bslalg::SwapUtil::swap(&d_holidayOffsets,    &other.d_holidayOffsets);
    bslalg::SwapUtil::swap(&d_holidayCodesIndex, &other.d_holidayCodesIndex);
    bslalg::SwapUtil::swap(&d_holidayCodes,      &other.d_holidayCodes);
}

// ACCESSORS
PackedCalendar::HolidayCodeConstIterator
                      PackedCalendar::beginHolidayCodes(const Date& date) const
{
    BSLS_ASSERT(isHoliday(date));

    const int                  offset      = date - d_firstDate;
    const OffsetsConstIterator offsetBegin = d_holidayOffsets.begin();
    const OffsetsConstIterator offsetEnd   = d_holidayOffsets.end();
    const OffsetsConstIterator i = bdlc::PackedIntArrayUtil::lowerBound(
                                                                   offsetBegin,
                                                                   offsetEnd,
                                                                   offset);

    bsl::size_t iterIndex;
    if (i == offsetEnd || *i != offset) {
        iterIndex = d_holidayCodes.length();
    }
    else {
        iterIndex = d_holidayCodesIndex[i - offsetBegin];
    }

    return HolidayCodeConstIterator(d_holidayCodes.begin() + iterIndex);
}

PackedCalendar::HolidayCodeConstIterator
                        PackedCalendar::endHolidayCodes(const Date& date) const
{
    BSLS_ASSERT(isHoliday(date));

    const int                  offset      = date - d_firstDate;
    const OffsetsConstIterator offsetBegin = d_holidayOffsets.begin();
    const OffsetsConstIterator offsetEnd   = d_holidayOffsets.end();
    const OffsetsConstIterator i = bdlc::PackedIntArrayUtil::lowerBound(
                                                                   offsetBegin,
                                                                   offsetEnd,
                                                                   offset);

    bsl::size_t iterIndex;
    if (i == offsetEnd || *i != offset) {
        iterIndex = d_holidayCodes.length();
    }
    else {
        iterIndex = i != offsetEnd - 1
                    ? d_holidayCodesIndex[i - offsetBegin + 1]
                    : d_holidayCodes.length();
    }

    return HolidayCodeConstIterator(d_holidayCodes.begin() + iterIndex);
}

int PackedCalendar::getNextBusinessDay(Date        *nextBusinessDay,
                                       const Date&  date,
                                       int          nth) const
{
    BSLS_ASSERT(nextBusinessDay);
    BSLS_ASSERT(Date(9999, 12, 31) > date);
    BSLS_ASSERT(isInRange(date + 1));
    BSLS_ASSERT(0 < nth);

    enum { e_SUCCESS = 0, e_FAILURE = 1 };

    BusinessDayConstIterator iter = beginBusinessDays(date + 1);
    if (iter == endBusinessDays()) {
        return e_FAILURE;                                             // RETURN
    }
    while (--nth) {
        ++iter;
        if (iter == endBusinessDays()) {
            return e_FAILURE;                                         // RETURN
        }
    }
    *nextBusinessDay = *iter;
    return e_SUCCESS;
}

bool PackedCalendar::isWeekendDay(const Date& date) const
{
    if (d_weekendDaysTransitions.empty()) {
        return false;                                                 // RETURN
    }

    DayOfWeekSet dummySet;

    WeekendDaysTransitionSequence::const_iterator it =
                        bsl::upper_bound(d_weekendDaysTransitions.begin(),
                                         d_weekendDaysTransitions.end(),
                                         WeekendDaysTransition(date, dummySet),
                                         WeekendDaysTransitionLess());

    if (it == d_weekendDaysTransitions.begin()) {
        return false;                                                 // RETURN
    }

    --it;

    return it->second.isMember(date.dayOfWeek());
}

int PackedCalendar::numBusinessDays(const Date& beginDate,
                                    const Date& endDate) const
{
    BSLS_ASSERT(isInRange(beginDate));
    BSLS_ASSERT(isInRange(endDate));
    BSLS_ASSERT(beginDate <= endDate);

    int                      num     = 0;
    BusinessDayConstIterator iter    = beginBusinessDays(beginDate);
    BusinessDayConstIterator endIter = endBusinessDays();
    while (iter != endIter && *iter <= endDate) {
        ++num;
        ++iter;
    }
    return num;
}

int PackedCalendar::numHolidayCodes(const Date& date) const
{
    BSLS_ASSERT(isInRange(date));

    const OffsetsConstIterator it = bdlc::PackedIntArrayUtil::lowerBound(
                                                     d_holidayOffsets.begin(),
                                                     d_holidayOffsets.end(),
                                                     date - d_firstDate);
    if (it == d_holidayOffsets.end() || *it != (date - d_firstDate)) {
        return 0;                                                     // RETURN
    }

    return static_cast<int>(endHolidayCodes(it) - beginHolidayCodes(it));
}

int PackedCalendar::numNonBusinessDays() const
{
    bsl::size_t sum = numHolidays() + numWeekendDaysInRange();

    for (OffsetsConstIterator it = d_holidayOffsets.begin();
                                          it != d_holidayOffsets.end(); ++it) {
        sum -= isWeekendDay(d_firstDate + *it);
    }

    return static_cast<int>(sum);
}

int PackedCalendar::numWeekendDaysInRange() const
{
    if (d_weekendDaysTransitions.empty() || d_firstDate > d_lastDate) {
        return 0;                                                     // RETURN
    }

    // Find the nearest transition that has a date less than or equal to the
    // first date of the calendar and compute the 'firstDate' from which to
    // count weekend days.

    WeekendDaysTransitionSequence::const_iterator iter;
    Date                                          firstDate;

    {
        DayOfWeekSet dummySet;
        iter = bsl::upper_bound(d_weekendDaysTransitions.begin(),
                                d_weekendDaysTransitions.end(),
                                WeekendDaysTransition(d_firstDate, dummySet),
                                WeekendDaysTransitionLess());

        if (iter != d_weekendDaysTransitions.begin()) {
            --iter;
            firstDate = d_firstDate;
        }
        else {
            firstDate = iter->first;
        }
    }

    // Count the number of weekend days.

    int numWeekendDays = 0;

    do {
        const DayOfWeekSet& weekendDays = iter->second;
        Date                lastDate;

        ++iter;
        if (iter != d_weekendDaysTransitions.end()
         && iter->first <= d_lastDate) {
            lastDate = iter->first - 1;
        }
        else {
            lastDate = d_lastDate;
        }

        numWeekendDays += numWeekendDaysInRangeImp(firstDate,
                                                   lastDate,
                                                   weekendDays);

        if (iter == d_weekendDaysTransitions.end()) {
            break;
        }

        firstDate = iter->first;

    } while (firstDate <= d_lastDate);

    return numWeekendDays;
}

                                  // Aspects

bsl::ostream& PackedCalendar::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(stream.bad())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return stream;                                                // RETURN
    }
    const char NL = spacesPerLevel >= 0 ? '\n' : ' ';

    if (level < 0) {
        level = -level;
    }
    else {
        indent(stream, level, spacesPerLevel);
    }

    stream << "{" << NL;

    if (spacesPerLevel >= 0) {
        indent(stream, level + 1, spacesPerLevel);
    }
    stream << "[ " << d_firstDate << ", " << d_lastDate << " ]" << NL;

    if (spacesPerLevel >= 0) {
        indent(stream, level + 1, spacesPerLevel);
    }

    stream << "[ ";
    WeekendDaysTransitionConstIterator iter = beginWeekendDaysTransitions();
    while (iter != endWeekendDaysTransitions())
    {
        stream << iter->first << " : ";
        iter->second.print(stream, -1, -1);
        if (++iter != endWeekendDaysTransitions()) {
            stream << ", ";
        }
    }
    if (!d_weekendDaysTransitions.empty()) {
        stream << " ";
    }
    stream << "]" << NL;

    for (OffsetsConstIterator i = d_holidayOffsets.begin();
                                            i != d_holidayOffsets.end(); ++i) {

        if (spacesPerLevel >= 0) {
            indent(stream, level + 1, spacesPerLevel);
        }
        stream << (d_firstDate + *i);

        const CodesConstIterator b = beginHolidayCodes(i);
        const CodesConstIterator e = endHolidayCodes(i);
        if (b != e) {
            stream << " {" << NL;

            for (CodesConstIterator j = b; j != e; ++j) {
                if (spacesPerLevel >= 0) {
                    indent(stream, level + 2, spacesPerLevel);
                }
                stream << *j << NL;
            }

            if (spacesPerLevel >= 0) {
                indent(stream, level + 1, spacesPerLevel);
            }

            stream << "}";
        }

        stream << NL;
    }
    if (spacesPerLevel >= 0) {
        indent(stream, level, spacesPerLevel);
    }
    stream << "}";

    if (spacesPerLevel >= 0) {
        stream << NL;
    }

    return stream;
}

}  // close package namespace

// FREE OPERATORS
bool bdlt::operator==(const PackedCalendar& lhs, const PackedCalendar& rhs)
{
    return lhs.d_firstDate              == rhs.d_firstDate
        && lhs.d_lastDate               == rhs.d_lastDate
        && lhs.d_weekendDaysTransitions == rhs.d_weekendDaysTransitions
        && lhs.d_holidayOffsets         == rhs.d_holidayOffsets
        && lhs.d_holidayCodesIndex      == rhs.d_holidayCodesIndex
        && lhs.d_holidayCodes           == rhs.d_holidayCodes;
}

bsl::ostream& bdlt::operator<<(bsl::ostream&         stream,
                               const PackedCalendar& calendar)
{
    calendar.print(stream, 0, -1);
    return stream;
}

namespace bdlt {

                  // ---------------------------------------------
                  // class PackedCalendar_BusinessDayConstIterator
                  // ---------------------------------------------

// PRIVATE CREATORS
PackedCalendar_BusinessDayConstIterator::
     PackedCalendar_BusinessDayConstIterator(const PackedCalendar& calendar,
                                             const Date&           startDate,
                                             bool                  endIterFlag)
: d_calendar_p(&calendar)
, d_endFlag(false)
{
    if (0 == calendar.length()) {
        d_endFlag = true;
        d_currentOffset = 0;
        return;                                                       // RETURN
    }

    d_currentOffset = startDate - calendar.firstDate();

    d_offsetIter = bdlc::PackedIntArrayUtil::lowerBound(
                                             calendar.d_holidayOffsets.begin(),
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
        // If 'startDate' is not a business day, move 'd_currentOffset' to the
        // next business day.  If 'endIterFlag' is true, this constructor is
        // called from 'endBusinessDays' or 'endBusinessDays(Date& date)'.  We
        // need to move the iterator to point to one element past the first
        // business day that occurs on or before the specified 'startDate'.  So
        // call 'nextBusinessDay' to move the iterator to the next element.

        nextBusinessDay();
    }
}

// PRIVATE MANIPULATORS
void PackedCalendar_BusinessDayConstIterator::nextBusinessDay()
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

                return;                                               // RETURN
            }
        }
        else {
            ++d_offsetIter;
        }
    }

    // We did not find a next business day.  This iterator becomes an 'end'
    // iterator.  Note that 'd_currentOffset == lastOffset'.

    d_endFlag = true;
}

void PackedCalendar_BusinessDayConstIterator::previousBusinessDay()
{
    BSLS_ASSERT(*this != d_calendar_p->beginBusinessDays());

    if (false == d_endFlag) {
        --d_currentOffset;
    }
    else {
        d_endFlag = false;
    }

    while (d_currentOffset >= 0) {
        if (d_offsetIter != d_calendar_p->d_holidayOffsets.begin()) {
            OffsetsConstIterator iter = d_offsetIter;
            --iter;
            if (d_currentOffset > *iter) {
                if (!d_calendar_p->isWeekendDay(
                                d_calendar_p->firstDate() + d_currentOffset)) {

                    // We found the previous business day.

                    return;                                           // RETURN
                }
            }
            else {
                --d_offsetIter;
            }

        }
        else if (!d_calendar_p->isWeekendDay(
                                d_calendar_p->firstDate() + d_currentOffset)) {

            // We found the previous business day.

            return;                                                   // RETURN
        }

        --d_currentOffset;
    }

    // Since there is a holiday, this line can never be reached.

    BSLS_ASSERT_OPT(0 &&
            "Impossible scenario for PackedCalendar_BusinessDayConstIterator");
}

// MANIPULATORS
PackedCalendar_BusinessDayConstIterator&
PackedCalendar_BusinessDayConstIterator::operator=(
                            const PackedCalendar_BusinessDayConstIterator& rhs)
{
    d_offsetIter    = rhs.d_offsetIter;
    d_calendar_p    = rhs.d_calendar_p;
    d_currentOffset = rhs.d_currentOffset;
    d_endFlag       = rhs.d_endFlag;

    return *this;
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
