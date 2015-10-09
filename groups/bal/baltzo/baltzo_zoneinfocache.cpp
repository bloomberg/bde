// baltzo_zoneinfocache.cpp                                           -*-C++-*-
#include <baltzo_zoneinfocache.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baltzo_zoneinfocache_cpp,"$Id$ $CSID$")

#include <ball_log.h>

#include <baltzo_errorcode.h>         // for testing only
#include <baltzo_zoneinfocache.h>
#include <baltzo_zoneinfoutil.h>

#include <bslmt_readlockguard.h>
#include <bslmt_writelockguard.h>

#include <bslma_allocator.h>
#include <bslma_rawdeleterproctor.h>

#include <bslmf_assert.h>

#include <bsl_ostream.h>
#include <bsl_set.h>
#include <bsl_string.h>

namespace BloombergLP {

static const char LOG_CATEGORY[] = "BALTZO.ZONEINFOCACHE";

                            // -------------------
                            // class ZoneinfoCache
                            // -------------------

// CREATORS
baltzo::ZoneinfoCache::~ZoneinfoCache()
{
    for (ZoneinfoMap::iterator it  = d_cache.begin();
                               it != d_cache.end();
                               ++it) {
        BSLS_ASSERT(0 != it->second);
        d_allocator_p->deleteObject(it->second);
    }
}

// MANIPULATORS
const baltzo::Zoneinfo *baltzo::ZoneinfoCache::getZoneinfo(
                                                        int        *rc,
                                                        const char *timeZoneId)
{
    BSLS_ASSERT(0 != rc);
    BSLS_ASSERT(0 != timeZoneId);

    enum {
        // Define the failure status value.
        FAILURE = -1
    };

    BSLMF_ASSERT(static_cast<int>(ErrorCode::k_UNSUPPORTED_ID) !=
                 static_cast<int>(FAILURE));

    const Zoneinfo *result = lookupZoneinfo(timeZoneId);

    if (0 != result) {
        *rc = 0;
        return result;                                                // RETURN
    }

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_lock);

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
            if (ErrorCode::k_UNSUPPORTED_ID != *rc) {
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
const baltzo::Zoneinfo *baltzo::ZoneinfoCache::lookupZoneinfo(
                                                  const char *timeZoneId) const
{
    BSLS_ASSERT(0 != timeZoneId);

    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_lock);

    ZoneinfoMap::const_iterator it = d_cache.find(timeZoneId);
    if (d_cache.end() != it) {
        return it->second;                                            // RETURN
    }
    return 0;
}

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
