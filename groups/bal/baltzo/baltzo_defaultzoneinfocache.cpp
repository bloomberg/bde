// baltzo_defaultzoneinfocache.cpp                                    -*-C++-*-
#include <baltzo_defaultzoneinfocache.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baltzo_defaultzoneinfocache_cpp,"$Id$ $CSID$")

#include <baltzo_datafileloader.h>
#include <baltzo_testloader.h>  // for testing only

#include <bslmt_once.h>

#include <bslma_default.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_log.h>
#include <bsls_platform.h>

#include <bsl_cstdlib.h>    // 'getenv'
#include <bsl_ostream.h>

namespace {
namespace u {

using namespace BloombergLP;

template <class CCHAR_VECTOR>
struct IsCCharVector {
    static const bool value =
             bsl::is_same<CCHAR_VECTOR, bsl::vector<const char *> >::value
          || bsl::is_same<CCHAR_VECTOR, std::vector<const char *> >::value
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
          || bsl::is_same<CCHAR_VECTOR, std::pmr::vector<const char *> >::value
#endif
    ;
};

// Potential locations of TZ Database time-zone information.
static const char *BALTZO_DATA_LOCATIONS[] = {
#ifndef BSLS_PLATFORM_OS_WINDOWS
    "/opt/bbinfra/share/zoneinfo/",// Bloomberg standard data location
    "/opt/bb/share/zoneinfo/",     // deprecated Bloomberg stnd data location
    "/bb/data/datetime/zoneinfo/", // deprecated Bloomberg stnd data location
    "/usr/share/zoneinfo/",        // Unix standard location (Linux, OSX)
    "/usr/share/lib/zoneinfo/",    // Solaris standard location
#endif
    0    // 0 length arrays not allowed
};
enum { k_NUM_BALTZO_DATA_LOCATIONS = sizeof  BALTZO_DATA_LOCATIONS /
                                     sizeof *BALTZO_DATA_LOCATIONS - 1 };

// STATIC DATA
static baltzo::ZoneinfoCache *systemSingletonCachePtr = 0; // default sys cache
static baltzo::ZoneinfoCache *userSingletonCachePtr   = 0; // default usr cache

// STATIC HELPER FUNCTIONS
static
baltzo::ZoneinfoCache *initSystemDefaultCache()
    // Return the address of a modifiable singleton default, system-wide,
    // default-cache instance, initializing that instance if this method has
    // not previously been called.  Subsequent calls to this method return the
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

template <class CCHAR_VECTOR>
inline
void loadDefaultZoneinfoDataLocationsImp(CCHAR_VECTOR *locations)
{
    BSLS_ASSERT(locations);

    BSLMF_ASSERT(u::IsCCharVector<CCHAR_VECTOR>::value);

    locations->insert(locations->end(),
                      BALTZO_DATA_LOCATIONS + 0,
                      BALTZO_DATA_LOCATIONS + k_NUM_BALTZO_DATA_LOCATIONS);
}

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace baltzo {

                         // --------------------------
                         // class DefaultZoneinfoCache
                         // --------------------------

// PRIVATE CLASS METHODS
ZoneinfoCache *DefaultZoneinfoCache::instance()
{
    if (u::userSingletonCachePtr) {
        return u::userSingletonCachePtr;                              // RETURN
    }

    BSLMT_ONCE_DO {
        if (0 == u::systemSingletonCachePtr) {
            u::systemSingletonCachePtr = u::initSystemDefaultCache();
        }
    }

    return u::systemSingletonCachePtr;
}

// CLASS METHODS
const char *DefaultZoneinfoCache::defaultZoneinfoDataLocation()
{
    const char *envValue = getenv("BDE_ZONEINFO_ROOT_PATH");

    if (0 != envValue) {
        if (!DataFileLoader::isPlausibleZoneinfoRootPath(envValue)) {
            BSLS_LOG_ERROR("Environment variable 'BDE_ZONEINFO_ROOT_PATH' "
                           "does not refer to a directory containing "
                           "time-zone information data (%s).",
                           envValue);
        }
        else {
            BSLS_LOG_INFO("Environment variable 'BDE_ZONEINFO_ROOT_PATH' "
                          "set to '%s'.", envValue);
        }
        return envValue;                                              // RETURN
    }

    for (int ii = 0; ii < u::k_NUM_BALTZO_DATA_LOCATIONS; ++ii) {
        const char * const path = u::BALTZO_DATA_LOCATIONS[ii];

        if (DataFileLoader::isPlausibleZoneinfoRootPath(path)) {
            BSLS_LOG_INFO("The environment variable "
                          "'BDE_ZONEINFO_ROOT_PATH' has not been set. "
                          "falling back on default location: %s", path);
            return path;                                              // RETURN
        }
    }

    BSLS_LOG_INFO("The environment variable 'BDE_ZONEINFO_ROOT_PATH' is unset "
                  "and time-zone information files are not available in "
                  "any default location.  Falling back on the current "
                  "working directory of the task.");
    return ".";
}

void DefaultZoneinfoCache::loadDefaultZoneinfoDataLocations(
                                     bsl::vector<const char *>      *locations)
{
    u::loadDefaultZoneinfoDataLocationsImp(locations);
}

void DefaultZoneinfoCache::loadDefaultZoneinfoDataLocations(
                                     std::vector<const char *>      *locations)
{
    u::loadDefaultZoneinfoDataLocationsImp(locations);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
void DefaultZoneinfoCache::loadDefaultZoneinfoDataLocations(
                                     std::pmr::vector<const char *> *locations)
{
    u::loadDefaultZoneinfoDataLocationsImp(locations);
}
#endif

ZoneinfoCache *DefaultZoneinfoCache::setDefaultCache(ZoneinfoCache *cache)
{
    ZoneinfoCache *previous = u::userSingletonCachePtr;
    u::userSingletonCachePtr = cache;
    return previous;
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
