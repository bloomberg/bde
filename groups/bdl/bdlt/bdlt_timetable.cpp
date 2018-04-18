// bdlt_timetable.cpp                                                 -*-C++-*-
#include <bdlt_timetable.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_timetable_cpp,"$Id$ $CSID$")

#include <bdlt_dateutil.h>

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdlt {

                       // ============================
                       // class Timetable_ResetProctor
                       // ============================

class Timetable_ResetProctor {
    // This class implements a proctor that, unless its 'release' method has
    // previously been invoked, automatically invokes 'reset' on a 'Timetable'
    // upon destruction.

    // DATA
    Timetable *d_timetable_p;  // managed timetable

  private:
    // NOT IMPLEMENTED
    Timetable_ResetProctor(const Timetable_ResetProctor&);
    Timetable_ResetProctor& operator=(const Timetable_ResetProctor&);

  public:
    // CREATORS
    Timetable_ResetProctor(Timetable *timetable)
        // Create a 'reset' proctor that conditionally manages the specified
        // 'timetable'.
    : d_timetable_p(timetable)
    {
        BSLS_ASSERT_SAFE(timetable);
    }

    ~Timetable_ResetProctor()
        // Destroy this object and, if 'release' has not been invoked, invoke
        // the managed timetable's 'reset' method.
    {
        if (d_timetable_p) {
            d_timetable_p->reset();
        }
    }

    // MANIPULATORS
    void release()
        // Release from management the timetable currently managed by this
        // proctor.  If there is no managed timetable, this method has no
        // effect.
    {
        d_timetable_p = 0;
    }
};

                        // -------------------------
                        // class TimetableTransition
                        // -------------------------

// ACCESSORS

                             // Aspects

bsl::ostream& TimetableTransition::print(bsl::ostream& stream,
                                         int           level,
                                         int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);    // 'true' -> suppress '['
    stream << d_code << '@' << d_datetime;
    printer.end(true);      // 'true' -> suppress ']'

    return stream;
}

                           // -------------------
                           // class Timetable_Day
                           // -------------------

// MANIPULATORS
bool Timetable_Day::addTransition(const Time& time, int code)
{
    BSLS_ASSERT(24 > time.hour());

    BSLS_ASSERT(0 <= code || Timetable::k_UNSET_TRANSITION_CODE == code);

    int previousFinalCode = finalTransitionCode();

    bsl::vector<Timetable_CompactableTransition>::iterator iter =
                                        bsl::lower_bound(d_transitions.begin(),
                                                         d_transitions.end(),
                                                         time);
    if (iter == d_transitions.end()) {
        d_transitions.emplace_back(time, code);
    }
    else if (iter->d_time != time) {
        d_transitions.insert(iter,
                             Timetable_CompactableTransition(time, code));
    }
    else {
        iter->d_code = code;
    }

    return previousFinalCode != finalTransitionCode();
}

bool Timetable_Day::removeTransition(const Time& time)
{
    bsl::vector<Timetable_CompactableTransition>::iterator iter =
                                        bsl::lower_bound(d_transitions.begin(),
                                                         d_transitions.end(),
                                                         time);

    if (iter == d_transitions.end() || iter->d_time != time) {
        return false;                                                 // RETURN
    }

    int previousFinalCode = finalTransitionCode();

    d_transitions.erase(iter);

    return previousFinalCode != finalTransitionCode();
}

// ACCESSORS
int Timetable_Day::transitionCodeInEffect(const Time& time) const
{
    BSLS_ASSERT(24 > time.hour());

    bsl::vector<Timetable_CompactableTransition>::const_iterator iter =
                                        bsl::upper_bound(d_transitions.begin(),
                                                         d_transitions.end(),
                                                         time);
    if (iter == d_transitions.end()) {
        return finalTransitionCode();                                 // RETURN
    }
    else if (iter == d_transitions.begin()) {
        return d_initialTransitionCode;                               // RETURN
    }

    --iter;

    return iter->d_code;
}

                             // ---------------
                             // class Timetable
                             // ---------------

// CREATORS
Timetable::Timetable(bslma::Allocator *basicAllocator)
: d_firstDate(9999, 12, 31)
, d_lastDate(1, 1, 1)
, d_initialTransitionCode(k_UNSET_TRANSITION_CODE)
, d_timetable(basicAllocator)
{
}

Timetable::Timetable(const Date&       firstDate,
                     const Date&       lastDate,
                     int               initialTransitionCode,
                     bslma::Allocator *basicAllocator)
: d_initialTransitionCode(initialTransitionCode)
, d_timetable(basicAllocator)
{
    BSLS_ASSERT(firstDate <= lastDate);

    BSLS_ASSERT(                         0 <= initialTransitionCode
                || k_UNSET_TRANSITION_CODE == initialTransitionCode);

    setValidRange(firstDate, lastDate);
}

Timetable::Timetable(const Timetable&  original,
                     bslma::Allocator *basicAllocator)
: d_firstDate(original.d_firstDate)
, d_lastDate(original.d_lastDate)
, d_initialTransitionCode(original.d_initialTransitionCode)
, d_timetable(original.d_timetable, basicAllocator)
{
}

// MANIPULATORS
void Timetable::addTransition(const Date& date, const Time& time, int code)
{
    BSLS_ASSERT(24 >  time.hour());
    BSLS_ASSERT(isInRange(date));

    BSLS_ASSERT(0 <= code || k_UNSET_TRANSITION_CODE == code);

    Timetable_ResetProctor proctor(this);

    bsl::size_t   index = date - d_firstDate;
    Timetable_Day daily(d_timetable[index]);
    bool          roll  = daily.addTransition(time, code);

    d_timetable.replace(index, daily);

    ++index;
    while (roll && index < d_timetable.length())  {
        daily = d_timetable[index];
        roll  = daily.setInitialTransitionCode(code);

        d_timetable.replace(index, daily);

        ++index;
    }

    proctor.release();
}

void Timetable::addTransitions(const DayOfWeek::Enum& dayOfWeek,
                               const Time&            time,
                               int                    code,
                               const Date&            firstDate,
                               const Date&            lastDate)
{
    BSLS_ASSERT(24 > time.hour());
    BSLS_ASSERT(firstDate <= lastDate);
    BSLS_ASSERT(isInRange(firstDate));
    BSLS_ASSERT(isInRange(lastDate));

    BSLS_ASSERT(0 <= code || k_UNSET_TRANSITION_CODE == code);

    Date correctedFirstDate = DateUtil::nextDayOfWeekInclusive(dayOfWeek,
                                                               firstDate);
    Date correctedLastDate  = DateUtil::previousDayOfWeekInclusive(dayOfWeek,
                                                                   lastDate);

    for (Date date = correctedFirstDate;
         date <= correctedLastDate;
         date += 7) {
        addTransition(date, time, code);
    }
}

void Timetable::removeTransition(const Date& date, const Time& time)
{
    BSLS_ASSERT(24 > time.hour());
    BSLS_ASSERT(isInRange(date));

    Timetable_ResetProctor proctor(this);

    bsl::size_t   index = date - d_firstDate;
    Timetable_Day daily(d_timetable[index]);
    bool          roll  = daily.removeTransition(time);

    d_timetable.replace(index, daily);

    int code = daily.finalTransitionCode();

    ++index;
    while (roll && index < d_timetable.length())  {
        daily = d_timetable[index];
        roll  = daily.setInitialTransitionCode(code);

        d_timetable.replace(index, daily);

        ++index;
    }

    proctor.release();
}

void Timetable::removeTransitions(const Date& date)
{
    BSLS_ASSERT(isInRange(date));

    Timetable_ResetProctor proctor(this);

    bsl::size_t   index = date - d_firstDate;
    Timetable_Day daily(d_timetable[index]);
    bool          roll  = daily.removeAllTransitions();

    d_timetable.replace(index, daily);

    int code = daily.finalTransitionCode();

    ++index;
    while (roll && index < d_timetable.length())  {
        daily = d_timetable[index];
        roll  = daily.setInitialTransitionCode(code);

        d_timetable.replace(index, daily);

        ++index;
    }

    proctor.release();
}

void Timetable::removeTransitions(const DayOfWeek::Enum& dayOfWeek,
                                  const Time&            time,
                                  const Date&            firstDate,
                                  const Date&            lastDate)
{
    BSLS_ASSERT(24 > time.hour());
    BSLS_ASSERT(firstDate <= lastDate);
    BSLS_ASSERT(isInRange(firstDate));
    BSLS_ASSERT(isInRange(lastDate));

    Date correctedFirstDate = DateUtil::nextDayOfWeekInclusive(dayOfWeek,
                                                               firstDate);
    Date correctedLastDate  = DateUtil::previousDayOfWeekInclusive(dayOfWeek,
                                                                   lastDate);

    for (Date date = correctedFirstDate;
         date <= correctedLastDate;
         date += 7) {
        removeTransition(date, time);
    }
}

void Timetable::setInitialTransitionCode(int code)
{
    BSLS_ASSERT(0 <= code || k_UNSET_TRANSITION_CODE == code);

    d_initialTransitionCode = code;

    if (   0 < d_timetable.length()
        && d_timetable.front().initialTransitionCode()
                                                  != d_initialTransitionCode) {
        Timetable_ResetProctor proctor(this);

        bsl::size_t   index = 0;
        Timetable_Day daily(d_timetable[index]);
        bool          roll  = daily.setInitialTransitionCode(code);

        d_timetable.replace(index, daily);

        ++index;
        while (roll && index < d_timetable.length())  {
            daily = d_timetable[index];
            roll  = daily.setInitialTransitionCode(code);

            d_timetable.replace(index, daily);

            ++index;
        }

        proctor.release();
    }
}

void Timetable::setValidRange(const Date& firstDate, const Date& lastDate)
{
    BSLS_ASSERT(firstDate <= lastDate);

    Timetable_ResetProctor proctor(this);

    if (d_firstDate > d_lastDate) {
        // The 'Timetable' has the default constructed value.

        bsl::size_t n = static_cast<bsl::size_t>(lastDate - firstDate + 1);

        Timetable_Day daily;
        daily.setInitialTransitionCode(d_initialTransitionCode);

        for (bsl::size_t i = 0; i < n; ++i) {
            d_timetable.push_back(daily);
        }
    }
    else if (firstDate > d_lastDate || lastDate < d_firstDate) {
        // The 'Timetable' does not have the default constructed value and the
        // current valid range does not overlap the new range.

        int initialTransitionCode = d_initialTransitionCode;

        reset();

        d_initialTransitionCode = initialTransitionCode;

        bsl::size_t n = static_cast<bsl::size_t>(lastDate - firstDate + 1);

        Timetable_Day daily;
        daily.setInitialTransitionCode(initialTransitionCode);

        for (bsl::size_t i = 0; i < n; ++i) {
            d_timetable.push_back(daily);
        }
    }
    else {
        // The 'Timetable' does not have the default constructed value and the
        // current valid range overlaps the new range.

        if (firstDate < d_firstDate) {
            bsl::size_t n = static_cast<bsl::size_t>(d_firstDate - firstDate);

            Timetable_Day daily;
            daily.setInitialTransitionCode(d_initialTransitionCode);

            for (bsl::size_t i = 0; i < n; ++i) {
                d_timetable.insert(0, daily);
            }

        }
        else if (firstDate > d_firstDate) {
            d_timetable.remove(0, firstDate - d_firstDate);
            setInitialTransitionCode(d_initialTransitionCode);
        }

        if (lastDate > d_lastDate) {
            bsl::size_t n = static_cast<bsl::size_t>(lastDate - d_lastDate);

            Timetable_Day daily;
            daily.setInitialTransitionCode(
                                     d_timetable.back().finalTransitionCode());

            for (bsl::size_t i = 0; i < n; ++i) {
                d_timetable.push_back(daily);
            }
        }
        else if (lastDate < d_lastDate) {
            bsl::size_t n = static_cast<bsl::size_t>(d_lastDate - lastDate);

            d_timetable.remove(d_timetable.length() - n, n);
        }
    }

    d_firstDate = firstDate;
    d_lastDate  = lastDate;

    proctor.release();
}

// ACCESSORS
Timetable::const_iterator Timetable::begin() const
{
    bsl::size_t dayIndex = 0;

    while (dayIndex < d_timetable.length()
        && 0 == d_timetable[dayIndex].size()) {
        ++dayIndex;
    }
    return Timetable_ConstIterator(*this, dayIndex, 0);
}

                             // Aspects

bsl::ostream& Timetable::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("firstDate", d_firstDate);
    printer.printAttribute("lastDate", d_lastDate);
    printer.printAttribute("initialTransitionCode", d_initialTransitionCode);

    if (length()) {
        printer.printValue(begin(), end());
    }

    printer.end();
    return stream;
}

                      // -----------------------------
                      // class Timetable_ConstIterator
                      // -----------------------------

// MANIPULATORS
Timetable_ConstIterator& Timetable_ConstIterator::operator++()
{
    BSLS_ASSERT(d_dayIndex < d_timetable_p->d_timetable.length());

    ++d_transitionIndex;
    while (d_dayIndex < d_timetable_p->d_timetable.length()
        && d_transitionIndex ==
                               d_timetable_p->d_timetable[d_dayIndex].size()) {
        d_transitionIndex = 0;
        ++d_dayIndex;
    }

    return *this;
}

Timetable_ConstIterator& Timetable_ConstIterator::operator--()
{
    BSLS_ASSERT(0 < d_dayIndex || 0 < d_transitionIndex);

    if (d_transitionIndex) {
        --d_transitionIndex;
    }
    else {
        --d_dayIndex;

        while (d_dayIndex
            && 0 == d_timetable_p->d_timetable[d_dayIndex].size()) {
            --d_dayIndex;
        }

        BSLS_ASSERT(0 < d_timetable_p->d_timetable[d_dayIndex].size());

        d_transitionIndex = d_timetable_p->d_timetable[d_dayIndex].size() - 1;
    }

    return *this;
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
