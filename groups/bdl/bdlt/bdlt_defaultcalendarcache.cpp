// bdlt_defaultcalendarcache.cpp                                      -*-C++-*-
#include <bdlt_defaultcalendarcache.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_defaultcalendarcache_cpp,"$Id$ $CSID$")

#include <bdlt_calendarcache.h>
#include <bdlt_calendarloader.h>  // for testing only
#include <bdlt_date.h>            // for testing only

#include <bsls_assert.h>
#include <bsls_atomicoperations.h>
#include <bsls_bsllock.h>
#include <bsls_bslonce.h>
#include <bsls_objectbuffer.h>

#include <bsl_climits.h>  // 'INT_MAX'
#include <bsl_new.h>

namespace BloombergLP {
namespace bdlt {

namespace {

// STATIC DATA

static
bsls::AtomicOperations::AtomicTypes::Pointer g_cachePtr = { 0 };
    // 'g_cachePtr' holds the address of the default calendar cache.  Its value
    // is non-zero if the cache is currently in the initialized state (i.e.,
    // constructed and available for use), and 0 otherwise.

static
bsls::ObjectBuffer<CalendarCache>            g_buffer;
    // 'g_buffer' provides the "footprint" within which the default calendar
    // cache is constructed.

// STATIC HELPER FUNCTIONS

static
bsls::BslLock *getLock()
    // Return the address of the lock used to initialize and destroy the
    // default calendar cache in a thread-safe manner.
{
    static bsls::BslLock *theLockPtr = 0;

    static bsls::BslOnce      once = BSLS_BSLONCE_INITIALIZER;
           bsls::BslOnceGuard onceGuard;

    if (onceGuard.enter(&once)) {
        static bsls::BslLock theLock;
        theLockPtr = &theLock;
    }

    return theLockPtr;
}

static
int initializePrivate(CalendarLoader            *loader,
                      bool                       hasTimeOutFlag,
                      const bsls::TimeInterval&  timeout,
                      bslma::Allocator          *allocator)
    // Initialize the default 'bdlt::CalendarCache' object managed by
    // 'bdlt::DefaultCalendarCache' to use the specified 'loader' to obtain
    // calendars and the specified 'allocator' to supply memory.  If the
    // specified 'hasTimeOutFlag' is 'true', initialize the default cache to
    // have the specified 'timeout'.  Otherwise, initialize the default cache
    // to have no timeout.  If the default cache is already in the initialized
    // state, this method has no effect.  Return 0 on success, and a non-zero
    // value otherwise.  The behavior is undefined unless 'loader' and
    // 'allocator' remain valid until a subsequent call to
    // 'bdlt::DefaultCalendarCache::destroy', and
    // 'bsls::TimeInterval(0) <= timeout <= bsls::TimeInterval(INT_MAX)'.
{
    BSLS_ASSERT(loader);
    BSLS_ASSERT(allocator);
    BSLS_ASSERT(bsls::TimeInterval(0) <= timeout);
    BSLS_ASSERT(timeout <= bsls::TimeInterval(INT_MAX));

    int rc = 1;  // FAILURE

    bsls::BslLockGuard lockGuard(getLock());

    if (!bsls::AtomicOperations::getPtrAcquire(&g_cachePtr)) {

        if (hasTimeOutFlag) {
            new (g_buffer.buffer()) CalendarCache(loader, timeout, allocator);
        }
        else {
            new (g_buffer.buffer()) CalendarCache(loader, allocator);
        }

        bsls::AtomicOperations::setPtrRelease(&g_cachePtr, g_buffer.buffer());

        rc = 0;  // SUCCESS
    }

    return rc;
}

}  // close unnamed namespace

                        // --------------------------
                        // class DefaultCalendarCache
                        // --------------------------

// CLASS METHODS
void DefaultCalendarCache::destroy()
{
    bsls::BslLockGuard lockGuard(getLock());

    if (bsls::AtomicOperations::getPtrAcquire(&g_cachePtr)) {

        g_buffer.object().~CalendarCache();

        bsls::AtomicOperations::setPtrRelease(&g_cachePtr, 0);
    }
}

int DefaultCalendarCache::initialize(CalendarLoader   *loader,
                                     bslma::Allocator *allocator)
{
    return initializePrivate(loader, false, bsls::TimeInterval(0), allocator);
}

int DefaultCalendarCache::initialize(CalendarLoader            *loader,
                                     const bsls::TimeInterval&  timeout,
                                     bslma::Allocator          *allocator)
{
    return initializePrivate(loader, true, timeout, allocator);
}

CalendarCache *DefaultCalendarCache::instance()
{
    return static_cast<CalendarCache *>(
       const_cast<void *>(bsls::AtomicOperations::getPtrAcquire(&g_cachePtr)));
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
