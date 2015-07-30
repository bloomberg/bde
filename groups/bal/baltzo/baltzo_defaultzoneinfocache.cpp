// baltzo_defaultzoneinfocache.cpp                                    -*-C++-*-
#include <baltzo_defaultzoneinfocache.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baltzo_defaultzoneinfocache_cpp,"$Id$ $CSID$")

#include <baltzo_datafileloader.h>
#include <baltzo_testloader.h>  // for testing only

#include <ball_log.h>

#include <bdlqq_once.h>

#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_c_stdlib.h>    // 'getenv'

namespace BloombergLP {

static const char LOG_CATEGORY[] = "BAETZO.DEFAULTZONEINFOCACHE";

// Potential locations of TZ Database time-zone information.
static const char *BAETZO_DATA_LOCATIONS[] = {
#ifndef BSLS_PLATFORM_OS_WINDOWS
    "/opt/bb/share/zoneinfo/"      // Bloomberg standard data location
  , "/bb/data/datetime/zoneinfo/"  // deprecated Bloomberg stnd data location
  , "/usr/share/zoneinfo/"         // Unix standard location (Linux, OSX)
  , "/usr/share/lib/zoneinfo/"     // Solaris standard location
  , 0
#else
    0
#endif
};

// STATIC DATA
static baltzo::ZoneinfoCache *systemSingletonCachePtr = 0;  // default sys cache
static baltzo::ZoneinfoCache *userSingletonCachePtr   = 0;  // default usr cache

// STATIC HELPER FUNCTIONS
static
baltzo::ZoneinfoCache *initSystemDefaultCache()
    // Return the address of a modifiable singleton default, system-wide,
    // default-cache instance, initializing that instance if this method has
    // not perviously been called.  Subsequent calls to this method return the
    // previously created (singleton) instance with no other effect.  This
    // methods is *not* thread safe.
{
    bslma::Allocator *allocator = bslma::Default::globalAllocator();

    static baltzo::DataFileLoader loader(allocator);
    loader.configureRootPath(
                   baltzo::DefaultZoneinfoCache::defaultZoneinfoDataLocation());

    static baltzo::ZoneinfoCache cache(&loader, allocator);

    return &cache;
}

namespace baltzo {
                        // ---------------------------------
                        // class DefaultZoneinfoCache
                        // ---------------------------------

// PRIVATE CLASS METHODS
ZoneinfoCache *DefaultZoneinfoCache::instance()
{
    if (userSingletonCachePtr) {
        return userSingletonCachePtr;                                 // RETURN
    }

    BDLQQ_ONCE_DO {
        if (0 == systemSingletonCachePtr) {
            systemSingletonCachePtr = initSystemDefaultCache();
        }
    }
    return systemSingletonCachePtr;
}

// CLASS METHODS
const char *DefaultZoneinfoCache::defaultZoneinfoDataLocation()
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);
    const char *envValue = getenv("BDE_ZONEINFO_ROOT_PATH");

    if (0 != envValue) {
        if (!DataFileLoader::isPlausibleZoneinfoRootPath(envValue)) {
            BALL_LOG_ERROR << "Environment variable 'BDE_ZONEINFO_ROOT_PATH' "
                           << "does not refer to a directory containing "
                           << "time-zone information data ("
                           << envValue << ")."
                           << BALL_LOG_END;
        }
        else {
            BALL_LOG_INFO <<"Environment variable 'BDE_ZONEINFO_ROOT_PATH' "
                          <<"set to ' " << envValue << "'."
                          << BALL_LOG_END;
        }
        return envValue;                                              // RETURN
    }

    for (const char **pathPtr = BAETZO_DATA_LOCATIONS; *pathPtr; ++pathPtr) {
        if (DataFileLoader::isPlausibleZoneinfoRootPath(*pathPtr)) {
            BALL_LOG_INFO << "The environment variable "
                          << "'BDE_ZONEINFO_ROOT_PATH' has not been set. "
                          << "falling back on default location: "
                          << *pathPtr
                          << BALL_LOG_END;
            return *pathPtr;                                          // RETURN
         }
    }

    BALL_LOG_INFO
               << "The environment variable 'BDE_ZONEINFO_ROOT_PATH' is unset "
               << "and time-zone information files are not available in "
               << "any default location.  Falling back on the current "
               << "working directory of the task."
               << BALL_LOG_END;
    return ".";
}

void DefaultZoneinfoCache::loadDefaultZoneinfoDataLocations(
                                          bsl::vector<const char *> *locations)
{
    BSLS_ASSERT(locations);

    for (const char **pathPtr = BAETZO_DATA_LOCATIONS; *pathPtr; ++pathPtr) {
        locations->push_back(*pathPtr);
    }
}

ZoneinfoCache *DefaultZoneinfoCache::setDefaultCache(
                                                   ZoneinfoCache *cache)
{
    ZoneinfoCache *previous = userSingletonCachePtr;
    userSingletonCachePtr = cache;
    return previous;
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
