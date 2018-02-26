// bdlt_timetable.cpp                                                 -*-C++-*-
#include <bdlt_timetable.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_timetable_cpp,"$Id$ $CSID$")

#include <bdlt_dateutil.h>

#include <bslalg_swaputil.h>

#include <bslim_printer.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdlt {

                     // ================================
                     // class Timetable_RemoveAllProctor
                     // ================================

class Timetable_RemoveAllProctor {
    // This class implements a proctor that, unless its 'release' method has
    // previously been invoked, automatically invokes 'removeAll' on a
    // 'Timetable' upon destruction.

    // DATA
    Timetable *d_timetable_p;  // managed array

    // NOT IMPLEMENTED
    Timetable_RemoveAllProctor();
    Timetable_RemoveAllProctor(const Timetable_RemoveAllProctor&);
    Timetable_RemoveAllProctor& operator=(const Timetable_RemoveAllProctor&);

  public:
    // CREATORS
    Timetable_RemoveAllProctor(Timetable *timetable)
        // Create a 'removeAll' proctor that conditionally manages the
        // specified 'array' (if non-zero).
    : d_timetable_p(timetable)
    {
    }

    ~Timetable_RemoveAllProctor()
        // Destroy this object and, if 'release' has not been invoked, invoke
        // the managed array's 'removeAll' method.
    {
        if (d_timetable_p) {
            d_timetable_p->removeAll();
        }
    }

    // MANIPULATORS
    void release()
        // Release from management the array currently managed by this proctor.
        // If no array, this method has no effect.
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

Timetable::~Timetable()
{
}

// MANIPULATORS
void Timetable::addTransition(const Datetime& datetime, int transitionCode)
{
    BSLS_ASSERT(24 >  datetime.hour());
    BSLS_ASSERT(isInRange(datetime.date()));

    BSLS_ASSERT(                         0 <= transitionCode
                || k_UNSET_TRANSITION_CODE == transitionCode);

    Timetable_RemoveAllProctor proctor(this);

    bsl::size_t   index = datetime.date() - d_firstDate;
    Timetable_Day daily(d_timetable[index], d_timetable.allocator());
    bool          roll  = daily.addTransition(datetime, transitionCode);

    d_timetable.replace(index, daily);

    ++index;
    while (roll && index < d_timetable.length())  {
        daily = d_timetable[index];
        roll  = daily.setInitialTransitionCode(transitionCode);

        d_timetable.replace(index, daily);

        ++index;
    }

    proctor.release();
}

void Timetable::addTransition(const DayOfWeek::Enum& dayOfWeek,
                              const Time&            time,
                              int                    transitionCode,
                              const Date&            firstDate,
                              const Date&            lastDate)
{
    BSLS_ASSERT(24 > time.hour());
    BSLS_ASSERT(firstDate <= lastDate);
    BSLS_ASSERT(isInRange(firstDate));
    BSLS_ASSERT(isInRange(lastDate));

    BSLS_ASSERT(                         0 <= transitionCode
                || k_UNSET_TRANSITION_CODE == transitionCode);

    Date correctedFirstDate = DateUtil::nextDayOfWeekInclusive(dayOfWeek,
                                                               firstDate);
    Date correctedLastDate  = DateUtil::previousDayOfWeekInclusive(dayOfWeek,
                                                                   lastDate);

    for (Date date = correctedFirstDate;
         date <= correctedLastDate;
         date += 7) {
        addTransition(date, time, transitionCode);
    }
}

void Timetable::removeAllTransitions(const Date& date)
{
    BSLS_ASSERT(isInRange(date));

    Timetable_RemoveAllProctor proctor(this);

    bsl::size_t   index = date - d_firstDate;
    Timetable_Day daily(d_timetable[index], d_timetable.allocator());
    bool          roll  = daily.removeAllTransitions();

    d_timetable.replace(index, daily);

    int transitionCode = daily.finalTransitionCode();

    ++index;
    while (roll && index < d_timetable.length())  {
        daily = d_timetable[index];
        roll  = daily.setInitialTransitionCode(transitionCode);

        d_timetable.replace(index, daily);

        ++index;
    }

    proctor.release();
}

void Timetable::removeTransition(const Datetime& datetime)
{
    BSLS_ASSERT(24 > datetime.hour());
    BSLS_ASSERT(isInRange(datetime.date()));

    Timetable_RemoveAllProctor proctor(this);

    bsl::size_t   index = datetime.date() - d_firstDate;
    Timetable_Day daily(d_timetable[index], d_timetable.allocator());
    bool          roll  = daily.removeTransition(datetime);

    d_timetable.replace(index, daily);

    int transitionCode = daily.finalTransitionCode();

    ++index;
    while (roll && index < d_timetable.length())  {
        daily = d_timetable[index];
        roll  = daily.setInitialTransitionCode(transitionCode);

        d_timetable.replace(index, daily);

        ++index;
    }

    proctor.release();
}

void Timetable::removeTransition(const DayOfWeek::Enum& dayOfWeek,
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

void Timetable::setInitialTransitionCode(int transitionCode)
{
    BSLS_ASSERT(                         0 <= transitionCode
                || k_UNSET_TRANSITION_CODE == transitionCode);

    d_initialTransitionCode = transitionCode;

    if (   0 < d_timetable.length()
        && d_timetable.front().initialTransitionCode()
                                                  != d_initialTransitionCode) {
        Timetable_RemoveAllProctor proctor(this);

        bsl::size_t   index = 0;
        Timetable_Day daily(d_timetable[index], d_timetable.allocator());
        bool          roll  = daily.setInitialTransitionCode(transitionCode);

        d_timetable.replace(index, daily);

        ++index;
        while (roll && index < d_timetable.length())  {
            daily = d_timetable[index];
            roll  = daily.setInitialTransitionCode(transitionCode);

            d_timetable.replace(index, daily);

            ++index;
        }

        proctor.release();
    }
}

void Timetable::setValidRange(const Date& firstDate, const Date& lastDate)
{
    BSLS_ASSERT(firstDate <= lastDate);

    Timetable_RemoveAllProctor proctor(this);

    if (d_firstDate > d_lastDate) {
        // The 'Timetable' has the default constructed value.

        bsl::size_t n = static_cast<bsl::size_t>(lastDate - firstDate + 1);

        Timetable_Day daily(d_timetable.allocator());
        daily.setInitialTransitionCode(d_initialTransitionCode);

        for (bsl::size_t i = 0; i < n; ++i) {
            d_timetable.push_back(daily);
        }
    }
    else if (firstDate > d_lastDate || lastDate < d_firstDate) {
        // The 'Timetable' does not have the default constructed value and the
        // current valid range does not overlap the new range.

        int initialTransitionCode = d_initialTransitionCode;

        removeAll();

        d_initialTransitionCode = initialTransitionCode;

        bsl::size_t n = static_cast<bsl::size_t>(lastDate - firstDate + 1);

        Timetable_Day daily(d_timetable.allocator());
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

            Timetable_Day daily(d_timetable.allocator());
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

            Timetable_Day daily(d_timetable.allocator());
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
