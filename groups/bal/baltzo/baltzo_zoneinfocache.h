// baltzo_zoneinfocache.h                                             -*-C++-*-
#ifndef INCLUDED_BALTZO_ZONEINFOCACHE
#define INCLUDED_BALTZO_ZONEINFOCACHE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a cache for time-zone information.
//
//@CLASSES:
//  baltzo::ZoneinfoCache: a cache for time-zone information
//
//@SEE_ALSO: baltzo_zoneinfo, baltzo_defaultzoneinfocache
//
//@DESCRIPTION: This component defines a class, 'baltzo::ZoneinfoCache', that
// serves as a cache of 'baltzo::Zoneinfo' objects.  A time-zone cache is
// supplied, on construction, with a 'baltzo::Loader' object to obtain
// time-zone information objects from some data source.  Invocation of the
// 'getZoneinfo' method for a specified time zone returns the address of either
// previously cached data, or if that data is not cache-resident, a new loaded
// 'baltzo::Zoneinfo' object, which is cached for use in subsequent calls to
// 'getZoneinfo' and 'lookupZoneinfo'.  Addresses returned by either of these
// methods are valid for the lifetime of the cache.
//
///Thread Safety
///-------------
// 'baltzo::ZoneinfoCache' is fully *thread-safe*, meaning that all non-creator
// operations on an object can be safely invoked simultaneously from multiple
// threads.
//
///Usage
///-----
// In this section, we demonstrate creating a 'baltzo::ZoneinfoCache' object
// and using it to access time zone information.
//
///Example 1: Creating a Concrete 'baltzo::Loader'
///- - - - - - - - - - - - - - - - - - - - - - - -
// A 'baltzo::ZoneinfoCache' object is provided a 'baltzo::Loader' on
// construction.  The 'loader' is used to populate the cache per user requests
// via the 'getZoneinfo' method.  In this example, we use a 'TestLoader'
// implementation of the 'baltzo::Loader' protocol, based on the
// 'baltzo_testloader' component.  In this example, our test loader is
// explicitly primed with responses for requests for certain time-zone
// identifiers.  Note that, in practice, a 'loader' typically obtains time-zone
// information from some external data store (e.g., see
// 'baltzo_datafileloader').
//
// We start by creating and initializing a couple of example time zone
// information objects.  Note that the 'baltzo::Zoneinfo' objects below are
// illustrative, and contain no actual time zone information:
//..
//  baltzo::Zoneinfo newYorkZoneinfo;
//  newYorkZoneinfo.setIdentifier("America/New_York");
//
//  baltzo::Zoneinfo londonZoneinfo;
//  londonZoneinfo.setIdentifier("Europe/London");
//..
// Next we create a description of Eastern Standard Time (EST) and Greenwich
// Mean Time (GMT):
//..
//  baltzo::LocalTimeDescriptor est(-5 * 60 * 60, false, "EST");
//  baltzo::LocalTimeDescriptor gmt(           0, false, "GMT");
//..
// Then we set the initial transition for 'newYorkZoneinfo' to Eastern Standard
// Time, and the initial transition for 'londonZoneinfo' to Greenwich Mean
// Time.  Note that such an initial transition is required for a
// 'baltzo::Zoneinfo' object to be considered Well-Formed (see 'isWellFormed'):
//..
//  bsls::Types::Int64 firstTime = bdlt::EpochUtil::convertToTimeT64(
//                                                    bdlt::Datetime(1, 1, 1));
//  newYorkZoneinfo.addTransition(firstTime, est);
//  londonZoneinfo.addTransition(firstTime, gmt);
//..
// Next we create a 'TestLoader', and then populate it with our example time
// zone information objects:
//..
//  TestLoader testLoader;
//  testLoader.setTimeZone(newYorkZoneinfo);
//  testLoader.setTimeZone(londonZoneinfo);
//..
// Finally, we verify that 'testLoader' contains the configured
// 'baltzo::Zoneinfo' objects for New York and London:
//..
//  baltzo::Zoneinfo newYorkResult;
//  int rc = testLoader.loadTimeZone(&newYorkResult, "America/New_York");
//  assert(0 == rc);
//  assert(newYorkZoneinfo == newYorkResult);
//
//  baltzo::Zoneinfo londonResult;
//  rc = testLoader.loadTimeZone(&londonResult, "Europe/London");
//  assert(0 == rc);
//  assert(londonZoneinfo == londonResult);
//..
//
///Example 2: Creating and Using a 'baltzo::ZoneinfoCache'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we create a 'baltzo::ZoneinfoCache', and use it to access
// time zone information for several time zones.
//
// We start by creating a 'baltzo::ZoneinfoCache' object supplied with the
// address of the 'TestLoader' we populated in the preceding example:
//..
//  baltzo::ZoneinfoCache cache(&testLoader);
//..
// Next, we verify the newly constructed cache does not contain either New York
// or London:
//..
//  assert(0 == cache.lookupZoneinfo("America/New_York"));
//  assert(0 == cache.lookupZoneinfo("Europe/London"));
//..
// Then, we call 'getZoneinfo' to obtain the data for the New York time zone.
// Note that, because this is the first 'getZoneinfo' operation on the class,
// the time-zone data has not previously been retrieved, and the data must be
// loaded using the loader supplied at construction:
//..
//  const baltzo::Zoneinfo *newYork = cache.getZoneinfo(&rc,
//                                                      "America/New_York");
//
//  assert(0 == rc);
//  assert(0 != newYork);
//  assert("America/New_York" == newYork->identifier());
//..
// Next, we verify that a subsequent call 'lookupZoneinfo' for New York,
// returns the previously cached value.  However, a call to 'lookupZoneinfo'
// for London will return 0 because the value has not been cached:
//..
//  assert(newYork == cache.lookupZoneinfo("America/New_York"));
//  assert(0       == cache.lookupZoneinfo("Europe/London"));
//..
// Next, we call 'getZoneinfo' for London and verify that it returns the
// expected value:
//..
//  const baltzo::Zoneinfo *london = cache.getZoneinfo(&rc, "Europe/London");
//  assert(0 == rc);
//  assert(0 != london);
//  assert("Europe/London" == london->identifier());
//..
// Finally, we call 'getZoneinfo' with time zone identifier unknown to our
// 'TestLoader'.  The call to 'getZoneinfo' returns 0 because the time zone
// information cannot be loaded.  Examination of 'rc' shows indicates that the
// identifier is not supported:
//..
//  assert(0 == cache.getZoneinfo(&rc, "badId"));
//  assert(baltzo::ErrorCode::k_UNSUPPORTED_ID == rc);
//..

#include <balscm_version.h>

#include <baltzo_loader.h>
#include <baltzo_zoneinfo.h>

#include <bdlb_cstringless.h>

#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_rwmutex.h>

#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_review.h>

#include <bsl_map.h>

namespace BloombergLP {
namespace baltzo {

                            // ===================
                            // class ZoneinfoCache
                            // ===================

class ZoneinfoCache {
    // This class provides an efficient mechanism for retrieving information
    // about a given time zone.  The first time a client requests information
    // for some time-zone identifier, that information is loaded (using the
    // 'Loader' supplied at construction), returned to the client (via a
    // 'const' pointer value), and cached for future use.  Subsequent requests
    // for the same time-zone return the cached information.  The returned
    // values are valid for the lifetime of this object.
    //
    // This class:
    //: o is *exception-neutral*
    //: o is *fully* *thread-safe*
    // For terminology see 'bsldoc_glossary'.

  public:
    // TYPES
    typedef bsl::allocator<char> allocator_type;

  private:
    // PRIVATE TYPES
    typedef bsl::map<const char *, Zoneinfo *, bdlb::CStringLess> ZoneinfoMap;

    // DATA
    ZoneinfoMap             d_cache;      // cached time-zone info, indexed by
                                          // time-zone id

    Loader                 *d_loader_p;   // loader used to obtain time-zone
                                          // information (held, not owned)

    mutable bslmt::RWMutex  d_lock;       // cache access synchronization

    allocator_type          d_allocator;  // allocator used to supply memory

    // NOT IMPLEMENTED
    ZoneinfoCache(const ZoneinfoCache&);
    ZoneinfoCache& operator=(const ZoneinfoCache&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ZoneinfoCache,
                                   bslma::UsesBslmaAllocator);
        // 'ZoneinfoCache' is allocator-aware.

    // CREATORS
    explicit ZoneinfoCache(
                          Loader                *loader,
                          const allocator_type&  allocator = allocator_type());
        // Create an empty cache of time-zone information that will use the
        // specified 'loader' to populate the cache, as-needed, with time zone
        // information.  Optionally specify an 'allocator' (e.g., the address
        // of a 'bslma::Allocator' object) to supply memory; otherwise, the
        // default allocator is used.  In order the populate the cache for a
        // time zone identifier, 'loader' must return a 'Zoneinfo' object that
        // is well-formed (see 'ZoneinfoUtil::isWellFormed') and whose
        // 'identifier' matches the supplied time zone identifier.

    ~ZoneinfoCache();
        // Destroy this object.

    // MANIPULATORS
    const Zoneinfo *getZoneinfo(const char *timeZoneId);
    const Zoneinfo *getZoneinfo(int *rc, const char *timeZoneId);
        // Return the address of the non-modifiable 'Zoneinfo' object
        // describing the time zone identified by the specified 'timeZoneId',
        // or 0 if the operation does not succeed.  If the information for
        // 'timeZoneId' has not been previously cached, then attempt to
        // populate this object using the 'loader' supplied at construction.
        // Optionally specify the address of an integer, 'rc', in which to load
        // the return code for this operation.  If 'rc' is specified, load 0
        // into 'rc' if the operation succeeds, 'ErrorCode::k_UNSUPPORTED_ID'
        // if the time-zone identifier is not supported, and a negative value
        // if the operation does not succeed for any other reason.  If the
        // returned address is non-zero, the Zoneinfo object returned is
        // guaranteed to be well-formed (i.e., 'ZoneinfoUtil::isWellFormed'
        // will return 'true' if called with the returned value), and remain
        // valid for the lifetime of this object.  The behavior is undefined if
        // 'rc' is 0.

    // ACCESSORS
    const Zoneinfo *lookupZoneinfo(const char *timeZoneId) const;
        // Return the address of the non-modifiable cached description of the
        // time zone identified by the specified 'timeZoneId', and 0 if
        // information for 'timeZoneId' has not previously been cached (by a
        // call to 'getZoneinfo').  If the returned address is non-zero, the
        // Zoneinfo object returned is guaranteed to be well-formed (i.e.,
        // 'ZoneinfoUtil::isWellFormed will return 'true' if called with the
        // returned value), and remain valid for the lifetime of this object.

    allocator_type get_allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the default
        // allocator in effect at construction is used.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                            // -------------------
                            // class ZoneinfoCache
                            // -------------------

// CREATORS
inline
ZoneinfoCache::ZoneinfoCache(Loader *loader, const allocator_type&  allocator)
: d_cache(allocator)
, d_loader_p(loader)
, d_allocator(allocator)
{
    BSLS_ASSERT(0 != loader);
}

// MANIPULATORS
inline
const Zoneinfo *ZoneinfoCache::getZoneinfo(const char *timeZoneId)
{
    BSLS_ASSERT(0 != timeZoneId);

    int rc;
    return getZoneinfo(&rc, timeZoneId);
}

inline
ZoneinfoCache::allocator_type ZoneinfoCache::get_allocator() const
{
    return d_allocator;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
