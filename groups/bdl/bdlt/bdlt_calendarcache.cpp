// bdlt_calendarcache.cpp                                             -*-C++-*-
#include <bdlt_calendarcache.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_calendarcache_cpp,"$Id$ $CSID$")

#include <bdlt_calendarloader.h>
#include <bdlt_currenttime.h>
#include <bdlt_date.h>            // for testing only
#include <bdlt_packedcalendar.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_climits.h>      // 'INT_MAX'
#include <bsl_functional.h>

namespace BloombergLP {
namespace bdlt {

                        // -------------------------
                        // class CalendarCache_Entry
                        // -------------------------

// CREATORS
CalendarCache_Entry::CalendarCache_Entry()
: d_ptr()
, d_loadTime()
{
}

CalendarCache_Entry::CalendarCache_Entry(Calendar         *calendar,
                                         Datetime          loadTime,
                                         bslma::Allocator *allocator)
: d_ptr(calendar, allocator)
, d_loadTime(loadTime)
{
    BSLS_ASSERT(calendar);
    BSLS_ASSERT(allocator);
}

CalendarCache_Entry::CalendarCache_Entry(const CalendarCache_Entry& original)
: d_ptr(original.d_ptr)
, d_loadTime(original.d_loadTime)
{
}

CalendarCache_Entry::~CalendarCache_Entry()
{
}

// MANIPULATORS
CalendarCache_Entry& CalendarCache_Entry::operator=(
                                                const CalendarCache_Entry& rhs)
{
    d_ptr      = rhs.d_ptr;
    d_loadTime = rhs.d_loadTime;

    return *this;
}

// ACCESSORS
bsl::shared_ptr<const Calendar> CalendarCache_Entry::get() const
{
    return d_ptr;
}

Datetime CalendarCache_Entry::loadTime() const
{
    return d_loadTime;
}

                           // -------------------
                           // class CalendarCache
                           // -------------------

// CREATORS
CalendarCache::CalendarCache(CalendarLoader   *loader,
                             bslma::Allocator *basicAllocator)

// We have to supply 'bsl::less<key>()' because 'bsl::map' does not have a
// constructor that takes only an allocator.

: d_cache(bsl::less<bsl::string>(), basicAllocator)
, d_loader_p(loader)
, d_timeOut(0)
, d_hasTimeOutFlag(false)
, d_lock()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(loader);
}

CalendarCache::CalendarCache(CalendarLoader            *loader,
                             const bsls::TimeInterval&  timeout,
                             bslma::Allocator          *basicAllocator)

// We have to supply 'bsl::less<key>()' because 'bsl::map' does not have a
// constructor that takes only an allocator.

: d_cache(bsl::less<bsl::string>(), basicAllocator)
, d_loader_p(loader)
, d_timeOut(0, 0, 0, 0, timeout.totalMilliseconds())
, d_hasTimeOutFlag(true)
, d_lock()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(loader);
    BSLS_ASSERT(bsls::TimeInterval(0) <= timeout);
    BSLS_ASSERT(timeout <= bsls::TimeInterval(INT_MAX));
}

CalendarCache::~CalendarCache()
{
}

// MANIPULATORS
bsl::shared_ptr<const Calendar>
CalendarCache::getCalendar(const char *calendarName)
{
    BSLS_ASSERT(calendarName);

    {
        bsls::BslLockGuard lockGuard(&d_lock);

        CacheIterator iter = d_cache.find(calendarName);

        if (iter != d_cache.end()) {
            if (   !d_hasTimeOutFlag
                || d_timeOut > CurrentTime::utc() - iter->second.loadTime()) {
                return iter->second.get();                            // RETURN
            }
            else {
                d_cache.erase(iter);
            }
        }
    }

    // Load calendar identified by 'calendarName'.

    PackedCalendar packedCalendar;  // temporary, so use default allocator

    const Datetime timestamp = CurrentTime::utc();

    if (d_loader_p->load(&packedCalendar, calendarName)) {
        return bsl::shared_ptr<const Calendar>();                     // RETURN
    }

    // Create out-of-place calendar that will be managed by 'bsl::shared_ptr'.

    Calendar *calendarPtr = new (*d_allocator_p) Calendar(packedCalendar,
                                                          d_allocator_p);

    CalendarCache_Entry entry(calendarPtr, timestamp, d_allocator_p);

    // Insert newly-loaded calendar into cache if another thread hasn't done so
    // already.

    bsls::BslLockGuard lockGuard(&d_lock);

    ConstCacheIterator iter = d_cache.find(calendarName);

    // Here, we assume that the time elapsed between the last check and the
    // loading of the calendar is insignificant compared to the timeout, so we
    // will simply return the entry in the cache if it has been inserted by
    // another thread.

    if (iter != d_cache.end()) {
        return iter->second.get();                                    // RETURN
    }

    d_cache[calendarName] = entry;

    return entry.get();
}

int CalendarCache::invalidate(const char *calendarName)
{
    BSLS_ASSERT(calendarName);

    bsls::BslLockGuard lockGuard(&d_lock);

    CacheIterator iter = d_cache.find(calendarName);

    if (iter != d_cache.end()) {
        d_cache.erase(iter);

        return 1;                                                     // RETURN
    }

    return 0;
}

int CalendarCache::invalidateAll()
{
    bsls::BslLockGuard lockGuard(&d_lock);

    const int numInvalidated = static_cast<int>(d_cache.size());

    d_cache.clear();

    return numInvalidated;
}

// ACCESSORS
bsl::shared_ptr<const Calendar>
CalendarCache::lookupCalendar(const char *calendarName) const
{
    BSLS_ASSERT(calendarName);

    bsls::BslLockGuard lockGuard(&d_lock);

    CacheIterator iter = d_cache.find(calendarName);

    if (iter != d_cache.end()) {
        if (   !d_hasTimeOutFlag
            || d_timeOut > CurrentTime::utc() - iter->second.loadTime()) {
            return iter->second.get();                                // RETURN
        }
        else {
            d_cache.erase(iter);
        }
    }

    return bsl::shared_ptr<const Calendar>();
}

Datetime CalendarCache::lookupLoadTime(const char *calendarName) const
{
    BSLS_ASSERT(calendarName);

    bsls::BslLockGuard lockGuard(&d_lock);

    CacheIterator iter = d_cache.find(calendarName);

    if (iter != d_cache.end()) {
        if (   !d_hasTimeOutFlag
            || d_timeOut > CurrentTime::utc() - iter->second.loadTime()) {
            return iter->second.loadTime();                           // RETURN
        }
        else {
            d_cache.erase(iter);
        }
    }

    return Datetime();
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
