// baetzo_zoneinfocache.cpp                                           -*-C++-*-
#include <baetzo_zoneinfocache.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baetzo_zoneinfocache_cpp,"$Id$ $CSID$")

#include <baetzo_errorcode.h>         // for testing only
#include <baetzo_zoneinfocache.h>
#include <baetzo_zoneinfoutil.h>

#include <bcemt_readlockguard.h>
#include <bcemt_writelockguard.h>

#include <bslma_allocator.h>
#include <bslma_rawdeleterproctor.h>

namespace BloombergLP {

                        // --------------------------
                        // class baetzo_ZoneinfoCache
                        // --------------------------

// CREATORS
baetzo_ZoneinfoCache::~baetzo_ZoneinfoCache()
{

    for (ZoneinfoMap::iterator it  = d_cache.begin();
                               it != d_cache.end();
                               ++it) {
        BSLS_ASSERT(0 != it->second);
        d_allocator_p->deleteObject(it->second);
    }
}

// MANIPULATORS
const baetzo_Zoneinfo *baetzo_ZoneinfoCache::getZoneinfo(
                                                        int        *rc,
                                                        const char *timeZoneId)
{
    BSLS_ASSERT(0 != rc);
    BSLS_ASSERT(0 != timeZoneId);

    enum {
        // Define the failure status value.
        FAILURE = -1
    };

    BSLMF_ASSERT(baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID != FAILURE);

    const baetzo_Zoneinfo *result = lookupZoneinfo(timeZoneId);

    if (0 != result) {
        *rc = 0;
        return result;                                                // RETURN
    }

    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_lock);

    // We use 'lower_bound' to return the position where the 'timeZoneId'
    // should be (even if it is not in the map), so that it can be used as an
    // insertion hint.

    ZoneinfoMap::iterator it = d_cache.lower_bound(timeZoneId);

    if (d_cache.end() != it && !(d_cache.key_comp()(timeZoneId, it->first))) {
        // 'timeZoneId' must have been added to the map between the call to
        // 'lookupTimeZone', and the acquisition of the write-lock on 'd_lock'.

        BSLS_ASSERT(0 != it->second);
        *rc    = 0;
        result = it->second;
    }
    else {
        // Create a proctor for the new time zone value.

        baetzo_Zoneinfo *newTimeZonePtr =
                           new (*d_allocator_p) baetzo_Zoneinfo(d_allocator_p);

        bslma_RawDeleterProctor<baetzo_Zoneinfo, bslma_Allocator>
                                        proctor(newTimeZonePtr, d_allocator_p);

        *rc = d_loader_p->loadTimeZone(newTimeZonePtr, timeZoneId);
        if (0 != *rc) {
            return 0;                                                 // RETURN
        }

        if (!baetzo_ZoneinfoUtil::isWellFormed(*newTimeZonePtr)) {
            *rc = FAILURE;
            return 0;                                                 // RETURN
        }

        d_cache.insert(
                  it,
                  ZoneinfoMap::value_type(newTimeZonePtr->identifier().c_str(),
                                          newTimeZonePtr));
        result = newTimeZonePtr;

        // The pointer has been copied, so the the proctor must release
        // ownership.

        proctor.release();
    }

    return result;
}

// ACCESSORS
const baetzo_Zoneinfo *baetzo_ZoneinfoCache::lookupZoneinfo(
                                                  const char *timeZoneId) const
{
    BSLS_ASSERT(0 != timeZoneId);

    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_lock);

    ZoneinfoMap::const_iterator it = d_cache.find(timeZoneId);
    if (d_cache.end() != it) {
        return it->second;                                            // RETURN
    }
    return 0;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
