// baetzo_defaultzoneinfocache.cpp                                    -*-C++-*-
#include <baetzo_defaultzoneinfocache.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baetzo_defaultzoneinfocache_cpp,"$Id$ $CSID$")

#include <baetzo_datafileloader.h>
#include <baetzo_testloader.h>  // for testing only

#include <bael_log.h>

#include <bcemt_once.h>

#include <bdesu_fileutil.h>

#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_c_stdlib.h>    // 'getenv'

namespace BloombergLP {

static const char LOG_CATEGORY[] = "BAETZO.DEFAULTZONEINFOCACHE";

// Potential locations of TZ Database time-zone information.
static const char *BAETZO_DATA_LOCATIONS[] = {
#ifndef BSLS_PLATFORM__OS_WINDOWS
    "/bb/data/datetime/zoneinfo/"
  , "/usr/share/lib/zoneinfo/"
  , 0
#else
    0
#endif
};

// STATIC DATA
static baetzo_ZoneinfoCache *systemSingletonCachePtr = 0;  // default sys cache
static baetzo_ZoneinfoCache *userSingletonCachePtr   = 0;  // default usr cache

// STATIC HELPER FUNCTIONS
static
baetzo_ZoneinfoCache *initSystemDefaultCache()
    // Return the address of a modifiable singleton default, system-wide,
    // default-cache instance, initializing that instance if this method has
    // not perviously been called.  Subsequent calls to this method return the
    // previously created (singleton) instance with no other effect.  This
    // methods is *not* thread safe.
{
    bslma_Allocator *allocator = bslma_Default::globalAllocator();

    static baetzo_DataFileLoader loader(allocator);
    loader.configureRootPath(
                   baetzo_DefaultZoneinfoCache::defaultZoneinfoDataLocation());

    static baetzo_ZoneinfoCache cache(&loader, allocator);

    return &cache;
}

                        // ---------------------------------
                        // class baetzo_DefaultZoneinfoCache
                        // ---------------------------------

// PRIVATE CLASS METHODS
baetzo_ZoneinfoCache *baetzo_DefaultZoneinfoCache::instance()
{
    if (userSingletonCachePtr) {
        return userSingletonCachePtr;                                 // RETURN
    }

    BCEMT_ONCE_DO {
        if (0 == systemSingletonCachePtr) {
            systemSingletonCachePtr = initSystemDefaultCache();
        }
    }
    return systemSingletonCachePtr;
}

// CLASS METHODS
const char *baetzo_DefaultZoneinfoCache::defaultZoneinfoDataLocation()
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    const char *envValue = getenv("BDE_ZONEINFO_ROOT_PATH");

    if (0 != envValue) {
        if (!baetzo_DataFileLoader::isPlausibleZoneinfoRootPath(envValue)) {
            BAEL_LOG_ERROR << "Environment variable 'BDE_ZONEINFO_ROOT_PATH' "
                           << "does not refer to a directory containing "
                           << "time-zone information data ("
                           << envValue << ")."
                           << BAEL_LOG_END;
        }
        else {
            BAEL_LOG_INFO <<"Environment variable 'BDE_ZONEINFO_ROOT_PATH' "
                          <<"set to ' " << envValue << "'."
                          << BAEL_LOG_END;
        }
        return envValue;                                              // RETURN
    }

    for (const char **pathPtr = BAETZO_DATA_LOCATIONS; *pathPtr; ++pathPtr) {
        if (baetzo_DataFileLoader::isPlausibleZoneinfoRootPath(*pathPtr)) {
            BAEL_LOG_INFO << "The environment variable "
                          << "'BDE_ZONEINFO_ROOT_PATH' has not been set. "
                          << "falling back on default location: "
                          << *pathPtr
                          << BAEL_LOG_END;
            return *pathPtr;                                          // RETURN
         }
    }

    BAEL_LOG_INFO
               << "The environment variable 'BDE_ZONEINFO_ROOT_PATH' is unset "
               << "and time-zone information files are not available in "
               << "any default location.  Falling back on the current "
               << "working directory of the task."
               << BAEL_LOG_END;
    return ".";
}

void baetzo_DefaultZoneinfoCache::loadDefaultZoneinfoDataLocations(
                                          bsl::vector<const char *> *locations)
{
    BSLS_ASSERT(locations);

    for (const char **pathPtr = BAETZO_DATA_LOCATIONS; *pathPtr; ++pathPtr) {
        locations->push_back(*pathPtr);
    }
}

baetzo_ZoneinfoCache *baetzo_DefaultZoneinfoCache::setDefaultCache(
                                                   baetzo_ZoneinfoCache *cache)
{
    baetzo_ZoneinfoCache *previous = userSingletonCachePtr;
    userSingletonCachePtr = cache;
    return previous;
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
