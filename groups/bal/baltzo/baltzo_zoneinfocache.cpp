// baltzo_zoneinfocache.cpp                                           -*-C++-*-
#include <baltzo_zoneinfocache.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baltzo_zoneinfocache_cpp,"$Id$ $CSID$")

#include <ball_log.h>

#include <baltzo_errorcode.h>         // for testing only
#include <baltzo_zoneinfocache.h>
#include <baltzo_zoneinfoutil.h>

#include <bdlmtt_readlockguard.h>
#include <bdlmtt_writelockguard.h>

#include <bslma_allocator.h>
#include <bslma_rawdeleterproctor.h>

namespace BloombergLP {

static const char LOG_CATEGORY[] = "BAETZO.ZONEINFOCACHE";

namespace baltzo {
                        // --------------------------
                        // class ZoneinfoCache
                        // --------------------------

// CREATORS
ZoneinfoCache::~ZoneinfoCache()
{

    for (ZoneinfoMap::iterator it  = d_cache.begin();
                               it != d_cache.end();
                               ++it) {
        BSLS_ASSERT(0 != it->second);
        d_allocator_p->deleteObject(it->second);
    }
}

// MANIPULATORS
const Zoneinfo *ZoneinfoCache::getZoneinfo(
                                                        int        *rc,
                                                        const char *timeZoneId)
{
    BSLS_ASSERT(0 != rc);
    BSLS_ASSERT(0 != timeZoneId);

    enum {
        // Define the failure status value.
        FAILURE = -1
    };

    BSLMF_ASSERT(static_cast<int>(ErrorCode::BAETZO_UNSUPPORTED_ID) !=
                 static_cast<int>(FAILURE));

    const Zoneinfo *result = lookupZoneinfo(timeZoneId);

    if (0 != result) {
        *rc = 0;
        return result;                                                // RETURN
    }

    bdlmtt::WriteLockGuard<bdlmtt::RWMutex> guard(&d_lock);

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
        BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

        // Create a proctor for the new time zone value.

        Zoneinfo *newTimeZonePtr =
                           new (*d_allocator_p) Zoneinfo(d_allocator_p);

        bslma::RawDeleterProctor<Zoneinfo, bslma::Allocator>
                                        proctor(newTimeZonePtr, d_allocator_p);

        *rc = d_loader_p->loadTimeZone(newTimeZonePtr, timeZoneId);
        if (0 != *rc) {
            if (ErrorCode::BAETZO_UNSUPPORTED_ID != *rc) {
                BALL_LOG_ERROR << "Unexpected error code loading time zone "
                               << timeZoneId << ": " << *rc
                               << BALL_LOG_END;
            }
            return 0;                                                 // RETURN
        }
        if (!ZoneinfoUtil::isWellFormed(*newTimeZonePtr)) {
            BALL_LOG_ERROR << "Loaded zone info object for "
                           << timeZoneId << " is not well-formed"
                           << BALL_LOG_END;
            *rc = FAILURE;
            return 0;                                                 // RETURN
        }

        if (newTimeZonePtr->identifier() != timeZoneId) {
            BALL_LOG_ERROR << "Loaded time zone id "
                           << newTimeZonePtr->identifier() << " does not match"
                           << " requested id: " << timeZoneId
                           << BALL_LOG_END;
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
const Zoneinfo *ZoneinfoCache::lookupZoneinfo(
                                                  const char *timeZoneId) const
{
    BSLS_ASSERT(0 != timeZoneId);

    bdlmtt::ReadLockGuard<bdlmtt::RWMutex> guard(&d_lock);

    ZoneinfoMap::const_iterator it = d_cache.find(timeZoneId);
    if (d_cache.end() != it) {
        return it->second;                                            // RETURN
    }
    return 0;
}
}  // close package namespace

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
