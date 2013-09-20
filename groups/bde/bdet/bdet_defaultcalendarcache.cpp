// bdet_defaultcalendarcache.cpp                                      -*-C++-*-
#include <bdet_defaultcalendarcache.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdet_defaultcalendarcache_cpp,"$Id$ $CSID$")

#include <bdet_calendarcache.h>
#include <bdet_calendarloader.h>  // for testing only
#include <bdet_date.h>            // for testing only

#include <bsls_assert.h>
#include <bsls_atomicoperations.h>
#include <bsls_objectbuffer.h>

#include <bsl_new.h>

namespace BloombergLP {

// LOCAL TYPES

enum {
    SPIN_UNLOCKED = 0,  // value indicating that the spin lock is unlocked
    SPIN_LOCKED   = 1   // value indicating that the spin lock is locked
};

// STATIC DATA

static
bsls::AtomicOperations::AtomicTypes::Int     g_spinLock = { SPIN_UNLOCKED };
    // 'g_spinLock' provides a spin lock for ensuring mutually-exclusive write
    // access to 'g_cachePtr' within the 'initialize' and 'destroy' methods.

static
bsls::AtomicOperations::AtomicTypes::Pointer g_cachePtr = { 0 };
    // 'g_cachePtr' holds the address of the default calendar cache.  Its value
    // is non-zero if the cache is currently in the initialized state (i.e.,
    // constructed and available for use), and 0 otherwise.

static
bsls::ObjectBuffer<bdet_CalendarCache>       g_buffer;
    // 'g_buffer' provides the "footprint" within which the default calendar
    // cache is constructed.

// STATIC HELPER FUNCTIONS

static
void acquireSpinLock(bsls::AtomicOperations::AtomicTypes::Int *spinLock)
    // Acquire the lock on the specified 'spinLock'.  The behavior is undefined
    // unless the calling thread does not currently hold the lock on
    // 'spinLock'.
{
    do {
        if (SPIN_UNLOCKED == bsls::AtomicOperations::getIntAcquire(spinLock)) {
            if (SPIN_UNLOCKED == bsls::AtomicOperations::swapIntAcqRel(
                                                                spinLock,
                                                                SPIN_LOCKED)) {
                return;                                               // RETURN
            }
        }
    } while (1);
}

static
void releaseSpinLock(bsls::AtomicOperations::AtomicTypes::Int *spinLock)
    // Release the lock on the specified 'spinLock'.  The behavior is undefined
    // unless the lock on 'spinLock' is currently held by the calling thread.
{
    bsls::AtomicOperations::setIntRelease(spinLock, SPIN_UNLOCKED);
}

static
int initializePrivate(bdet_CalendarLoader      *loader,
                      bool                      hasTimeOutFlag,
                      const bdet_TimeInterval&  timeout,
                      bslma::Allocator         *allocator)
{
    BSLS_ASSERT(loader);
    BSLS_ASSERT(allocator);

    int rc = 1;  // FAILURE

    acquireSpinLock(&g_spinLock);

    if (!bsls::AtomicOperations::getPtrAcquire(&g_cachePtr)) {
        // Exception-safety assumption: the 'bdet_CalendarCache' constructor
        // does not allocate memory, so there is no need to proctor the spin
        // lock.

        if (hasTimeOutFlag) {
            new (g_buffer.buffer()) bdet_CalendarCache(loader,
                                                       timeout,
                                                       allocator);
        }
        else {
            new (g_buffer.buffer()) bdet_CalendarCache(loader, allocator);
        }

        bsls::AtomicOperations::setPtrRelease(&g_cachePtr, g_buffer.buffer());

        rc = 0;  // SUCCESS
    }

    releaseSpinLock(&g_spinLock);

    return rc;
}

                        // -------------------------------
                        // class bdet_DefaultCalendarCache
                        // -------------------------------

// CLASS METHODS
int bdet_DefaultCalendarCache::initialize(bdet_CalendarLoader *loader,
                                          bslma::Allocator    *allocator)
{
    return initializePrivate(loader, false, bdet_TimeInterval(0), allocator);
}

int bdet_DefaultCalendarCache::initialize(bdet_CalendarLoader      *loader,
                                          const bdet_TimeInterval&  timeout,
                                          bslma::Allocator         *allocator)
{
    BSLS_ASSERT(0 <= timeout.seconds());

    return initializePrivate(loader, true, timeout, allocator);
}

void bdet_DefaultCalendarCache::destroy()
{
    acquireSpinLock(&g_spinLock);

    if (bsls::AtomicOperations::getPtrAcquire(&g_cachePtr)) {

        g_buffer.object().~bdet_CalendarCache();

        bsls::AtomicOperations::setPtrRelease(&g_cachePtr, 0);
    }

    releaseSpinLock(&g_spinLock);
}

bdet_CalendarCache *bdet_DefaultCalendarCache::instance()
{
    return static_cast<bdet_CalendarCache *>(
       const_cast<void *>(bsls::AtomicOperations::getPtrAcquire(&g_cachePtr)));
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
