// bdlt_calendarcache.cpp                                             -*-C++-*-
#include <bdlt_calendarcache.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_calendarcache_cpp,"$Id$ $CSID$")

#include <bdlt_calendarloader.h>
#include <bdlt_date.h>            // for testing only
#include <bdlt_packedcalendar.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_climits.h>      // 'INT_MAX'
#include <bsl_functional.h>

namespace BloombergLP {
namespace bdlt {

namespace {

// STATIC HELPER FUNCTIONS
static
bool hasExpired(const bsl::time_t& interval,
                const bsl::time_t& loadTime)
    // Return 'true' if at least the specified time 'interval' has elapsed
    // since the specified 'loadTime', and 'false' otherwise.
{
    const bsl::time_t now = bsl::time(0);
    BSLS_ASSERT(static_cast<bsl::time_t>(-1) != now);

    const bsl::time_t elapsedTime = now - loadTime;

    return elapsedTime >= interval ? true : false;
}

}  // close unnamed namespace

                     // =========================
                     // class CalendarCache_Entry
                     // =========================

class CalendarCache_Entry {
    // This class defines the type of objects that are inserted into the
    // calendar cache.  Each entry contains a shared pointer to a read-only
    // calendar and the time at which that calendar was loaded.  Note that an
    // explicit allocator is *required* to create a entry object.

    // DATA
    bsl::shared_ptr<const Calendar> d_ptr;       // shared pointer to
                                                 // out-of-place instance

    bsl::time_t                     d_loadTime;  // time when calendar was
                                                 // loaded

  public:
    // CREATORS
    CalendarCache_Entry();
        // Create an empty cache entry object.  Note that an empty cache entry
        // is never actually inserted into the cache.

    CalendarCache_Entry(Calendar         *calendar,
                        bsl::time_t       loadTime,
                        bslma::Allocator *allocator);
        // Create a cache entry object for managing the specified 'calendar'
        // that was loaded at the specified 'loadTime' using the specified
        // 'allocator'.  The behavior is undefined unless 'calendar' uses
        // 'allocator' to obtain memory.

    CalendarCache_Entry(const CalendarCache_Entry& original);
        // Create a cache entry object having the value of the specified
        // 'original' object.

    ~CalendarCache_Entry();
        // Destroy this cache entry object.

    // MANIPULATORS
    CalendarCache_Entry& operator=(const CalendarCache_Entry&);
        // Assign to this cache entry object the value of the specified 'rhs'
        // object, and return a reference providing modifiable access to this
        // object.

    // ACCESSORS
    bsl::shared_ptr<const Calendar> get() const;
        // Return a shared pointer providing non-modifiable access to the
        // calendar referred to by this cache entry object.

    bsl::time_t loadTime() const;
        // Return the time at which the calendar referred to by this cache
        // entry object was loaded.
};

                     // -------------------------
                     // class CalendarCache_Entry
                     // -------------------------

// CREATORS
CalendarCache_Entry::CalendarCache_Entry()
: d_ptr()
, d_loadTime(0)
{
}

CalendarCache_Entry::CalendarCache_Entry(Calendar         *calendar,
                                         bsl::time_t       loadTime,
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

bsl::time_t CalendarCache_Entry::loadTime() const
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
, d_timeOut(static_cast<bsl::time_t>(timeout.seconds()))
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

            if (!d_hasTimeOutFlag
             || !hasExpired(d_timeOut, iter->second.loadTime())) {
                return iter->second.get();                            // RETURN
            }
            else {
                d_cache.erase(iter);
            }
        }
    }

    // Load calendar identified by 'calendarName'.

    PackedCalendar packedCalendar;  // temporary, so use default allocator

    if (d_loader_p->load(&packedCalendar, calendarName)) {
        return bsl::shared_ptr<const Calendar>();                     // RETURN
    }

    // Create out-of-place calendar that will be managed by 'bsl::shared_ptr'.

    Calendar *calendarPtr = new (*d_allocator_p) Calendar(packedCalendar,
                                                          d_allocator_p);

    CalendarCache_Entry entry(calendarPtr, bsl::time(0), d_allocator_p);

    BSLS_ASSERT(static_cast<bsl::time_t>(-1) != entry.loadTime());

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

        if (!d_hasTimeOutFlag
         || !hasExpired(d_timeOut, iter->second.loadTime())) {
            return iter->second.get();                                // RETURN
        }
        else {
            d_cache.erase(iter);
        }
    }

    return bsl::shared_ptr<const Calendar>();
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
