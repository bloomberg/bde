// bdlt_timetablecache.cpp                                            -*-C++-*-
#include <bdlt_timetablecache.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_timetablecache_cpp,"$Id$ $CSID$")

#include <bdlt_timetableloader.h>
#include <bdlt_currenttime.h>
#include <bdlt_date.h>            // for testing only
#include <bdlt_packedtimetable.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_climits.h>      // 'INT_MAX'
#include <bsl_functional.h>

namespace BloombergLP {
namespace bdlt {

                        // --------------------------
                        // class TimetableCache_Entry
                        // --------------------------

// CREATORS
TimetableCache_Entry::TimetableCache_Entry()
: d_ptr()
, d_loadTime()
{
}

TimetableCache_Entry::TimetableCache_Entry(Timetable         *timetable,
                                         Datetime          loadTime,
                                         bslma::Allocator *allocator)
: d_ptr(timetable, allocator)
, d_loadTime(loadTime)
{
    BSLS_ASSERT(timetable);
    BSLS_ASSERT(allocator);
}

TimetableCache_Entry::TimetableCache_Entry(
                                          const TimetableCache_Entry& original)
: d_ptr(original.d_ptr)
, d_loadTime(original.d_loadTime)
{
}

TimetableCache_Entry::~TimetableCache_Entry()
{
}

// MANIPULATORS
TimetableCache_Entry& TimetableCache_Entry::operator=(
                                               const TimetableCache_Entry& rhs)
{
    d_ptr      = rhs.d_ptr;
    d_loadTime = rhs.d_loadTime;

    return *this;
}

// ACCESSORS
bsl::shared_ptr<const Timetable> TimetableCache_Entry::get() const
{
    return d_ptr;
}

Datetime TimetableCache_Entry::loadTime() const
{
    return d_loadTime;
}

                           // --------------------
                           // class TimetableCache
                           // --------------------

// CREATORS
TimetableCache::TimetableCache(TimetableLoader   *loader,
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

TimetableCache::TimetableCache(TimetableLoader            *loader,
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
    BSLS_ASSERT(bsls::TimeInterval() <= timeout);
    BSLS_ASSERT(timeout <= bsls::TimeInterval(INT_MAX, 0));
}

TimetableCache::~TimetableCache()
{
}

// MANIPULATORS
bsl::shared_ptr<const Timetable>
TimetableCache::getTimetable(const char *timetableName)
{
    BSLS_ASSERT(timetableName);

    {
        bsls::BslLockGuard lockGuard(&d_lock);

        CacheIterator iter = d_cache.find(timetableName);

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

    // Load timetable identified by 'timetableName'.

    PackedTimetable packedTimetable;  // temporary, so use default allocator

    const Datetime timestamp = CurrentTime::utc();

    if (d_loader_p->load(&packedTimetable, timetableName)) {
        return bsl::shared_ptr<const Timetable>();                    // RETURN
    }

    // Create out-of-place timetable that will be managed by 'bsl::shared_ptr'.

    Timetable *timetablePtr = new (*d_allocator_p) Timetable(packedTimetable,
                                                          d_allocator_p);

    TimetableCache_Entry entry(timetablePtr, timestamp, d_allocator_p);

    // Insert newly-loaded timetable into cache if another thread hasn't done
    // so already.

    bsls::BslLockGuard lockGuard(&d_lock);

    ConstCacheIterator iter = d_cache.find(timetableName);

    // Here, we assume that the time elapsed between the last check and the
    // loading of the timetable is insignificant compared to the timeout, so we
    // will simply return the entry in the cache if it has been inserted by
    // another thread.

    if (iter != d_cache.end()) {
        return iter->second.get();                                    // RETURN
    }

    d_cache[timetableName] = entry;

    return entry.get();
}

int TimetableCache::invalidate(const char *timetableName)
{
    BSLS_ASSERT(timetableName);

    bsls::BslLockGuard lockGuard(&d_lock);

    CacheIterator iter = d_cache.find(timetableName);

    if (iter != d_cache.end()) {
        d_cache.erase(iter);

        return 1;                                                     // RETURN
    }

    return 0;
}

int TimetableCache::invalidateAll()
{
    bsls::BslLockGuard lockGuard(&d_lock);

    const int numInvalidated = static_cast<int>(d_cache.size());

    d_cache.clear();

    return numInvalidated;
}

// ACCESSORS
bsl::shared_ptr<const Timetable>
TimetableCache::lookupTimetable(const char *timetableName) const
{
    BSLS_ASSERT(timetableName);

    bsls::BslLockGuard lockGuard(&d_lock);

    CacheIterator iter = d_cache.find(timetableName);

    if (iter != d_cache.end()) {
        if (   !d_hasTimeOutFlag
            || d_timeOut > CurrentTime::utc() - iter->second.loadTime()) {
            return iter->second.get();                                // RETURN
        }
        else {
            d_cache.erase(iter);
        }
    }

    return bsl::shared_ptr<const Timetable>();
}

Datetime TimetableCache::lookupLoadTime(const char *timetableName) const
{
    BSLS_ASSERT(timetableName);

    bsls::BslLockGuard lockGuard(&d_lock);

    CacheIterator iter = d_cache.find(timetableName);

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
