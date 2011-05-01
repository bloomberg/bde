// baetzo_zoneinfocache.h                                             -*-C++-*-
#ifndef INCLUDED_BAETZO_ZONEINFOCACHE
#define INCLUDED_BAETZO_ZONEINFOCACHE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a cache for time-zone information.
//
//@CLASSES:
//  baetzo_ZoneinfoCache: a cache for time-zone information
//
//@AUTHOR: Henry Verschell (hverschell), Stefano Pacifico (spacifico1)
//
//@SEE_ALSO: baetzo_zoneinfo, baetzo_defaultzoneinfocache
//
//@DESCRIPTION: This component defines a class, 'baetzo_ZoneinfoCache', that
// serves as a cache of 'baetzo_Zoneinfo' objects.  A time-zone cache is
// supplied, on construction, with a 'baetzo_Loader' object to obtain time-zone
// information objects from some data source.  Invocation of the 'getZoneinfo'
// method for a specified time zone returns the address of either previously
// cached data, or if that data is not cache-resident, a new loaded
// 'baetzo_Zoneinfo' object, which is cached for use in subsequent calls to
// 'getZoneinfo' and 'lookupZoneinfo'.  Addresses returned by either of these
// methods are valid for the lifetime of the cache.
//
///Thread Safety
///-------------
// 'baetzo_ZoneinfoCache' is fully *thread-safe*, meaning that all non-creator
// operations on an object can be safely invoked simultaneously from multiple
// threads.
//
///Usage
///-----
// In this section, we demonstrate creating a 'baetzo_ZoneinfoCache' object
// and using it to access time zone information.
//
///Example 1: Creating a Concrete 'baetzo_Loader'
/// - - - - - - - - - - - - - - - - - - - - - - -
// A 'baetzo_ZoneinfoCache' object is provided a 'baetzo_Loader' on
// construction.  The 'loader' is used to populate the cache per user requests
// via the 'getZoneinfo' method.  In this example, we use a 'TestLoader'
// implementation of the 'baetzo_Loader' protocol, based on the
// 'baetzo_testloader' component.  In this example, our test loader is
// explicitly primed with responses for requests for certain time-zone
// identifiers.  Note that, in practice, a 'loader' typically obtains time-zone
// information from some external data store (e.g., see
// 'baetzo_datafileloader').
//
// We start by creating and initializing a couple of example time zone
// information objects.  Note that the 'baetzo_Zoneinfo' objects below are
// illustrative, and contain no actual time zone information:
//..
//  baetzo_Zoneinfo newYorkZoneinfo;
//  newYorkZoneinfo.setIdentifier("America/New_York");
//
//  baetzo_Zoneinfo londonZoneinfo;
//  londonZoneinfo.setIdentifier("Europe/London");
//..
// Next we create a description of Eastern Standard Time (EST) and Greenwich
// Mean Time (GMT):
//..
//  baetzo_LocalTimeDescriptor est(-5 * 60 * 60, false, "EST");
//  baetzo_LocalTimeDescriptor gmt(           0, false, "GMT");
//..
// Then we set the initial transition for 'newYorkZoneinfo' to Eastern Standard
// Time, and the initial transition for 'londonZoneinfo' to Greenwich Mean
// Time.  Note that such an initial transition is required for a
// 'baetzo_Zoneinfo' object to be considered Well-Formed (see
// 'isWellFormed'):
//..
//  bsls_Types::Int64 firstTime = bdetu_Epoch::convertToTimeT64(
//                                                     bdet_Datetime(1, 1, 1));
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
// 'baetzo_Zoneinfo' objects for New York and London:
//..
//  baetzo_Zoneinfo newYorkResult;
//  int rc = testLoader.loadTimeZone(&newYorkResult, "America/New_York");
//  assert(0 == rc);
//  assert(newYorkZoneinfo == newYorkResult);
//
//  baetzo_Zoneinfo londonResult;
//  rc = testLoader.loadTimeZone(&londonResult, "Europe/London");
//  assert(0 == rc);
//  assert(londonZoneinfo == londonResult);
//..
//
///Example 2: Creating and Using a 'baetzo_ZoneinfoCache'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we create a 'baetzo_ZoneinfoCache', and use it to access
// time zone information for several time zones.
//
// We start by creating a 'baetzo_ZoneinfoCache' object supplied with the
// address of the 'TestLoader' we populated in the preceding example:
//..
//  baetzo_ZoneinfoCache cache(&testLoader);
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
//  const baetzo_Zoneinfo *newYork = cache.getZoneinfo(&rc,
//                                                     "America/New_York");
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
//  const baetzo_Zoneinfo *london = cache.getZoneinfo(&rc, "Europe/London");
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
//  assert(baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID == rc);
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAETZO_LOADER
#include <baetzo_loader.h>
#endif

#ifndef INCLUDED_BAETZO_ZONEINFO
#include <baetzo_zoneinfo.h>
#endif

#ifndef INCLUDED_BCEMT_RWMUTEX
#include <bcemt_rwmutex.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                        // ==========================
                        // class baetzo_ZoneinfoCache
                        // ==========================

class baetzo_ZoneinfoCache {
    // This class provides an efficient mechanism for retrieving information
    // about a given time zone.  The first time a client requests information
    // for some time-zone identifier, that information is loaded (using the
    // 'baetzo_Loader' supplied at construction), returned to the client (via a
    // 'const' pointer value), and cached for future use.  Subsequent requests
    // for the same time-zone return the cached information.  The returned
    // values are valid for the lifetime of this object.
    //
    // This class:
    //: o is *exception-neutral*
    //: o is *fully* *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // PRIVATE TYPES
    struct CStringLess {
        // This 'struct' implements an invocable function object that defines
        // an ordering on C-string values, allowing them to be included in
        // sorted containers such as 'bsl::map'.

        // ACCESSORS
        bool operator()(const char *lhs, const char *rhs) const;
            // Return 'true' if the value of the specified 'lhs' string is
            // less than (i.e., ordered before) the value of the specified
            // 'rhs' one, and 'false' otherwise.  The behavior is undefined
            // unless both 'lhs' and 'rhs' are null-terminated.
    };

    typedef bsl::map<const char *, baetzo_Zoneinfo *, CStringLess> ZoneinfoMap;

    // DATA
    ZoneinfoMap            d_cache;        // cached time-zone info, indexed by
                                           // time-zone id

    baetzo_Loader         *d_loader_p;     // loader used to obtain time-zone
                                           // information (held, not owned)

    mutable bcemt_RWMutex  d_lock;         // cache access synchronization

    bslma_Allocator       *d_allocator_p;  // allocator (held, not owned)

  private:
    // NOT IMPLEMENTED
    baetzo_ZoneinfoCache(const baetzo_ZoneinfoCache&);
    baetzo_ZoneinfoCache& operator=(const baetzo_ZoneinfoCache&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baetzo_ZoneinfoCache,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit baetzo_ZoneinfoCache(baetzo_Loader   *loader,
                                  bslma_Allocator *basicAllocator = 0);
        // Create an empty cache of time-zone information that will use the
        // specified 'loader' to populate the cache, as-needed, with time zone
        // information.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~baetzo_ZoneinfoCache();
        // Destroy this object.

    // MANIPULATORS
    const baetzo_Zoneinfo *getZoneinfo(const char *timeZoneId);
    const baetzo_Zoneinfo *getZoneinfo(int                *rc,
                                       const char *timeZoneId);
        // Return the address of the non-modifiable 'baetzo_Zoneinfo' object
        // describing the time zone identified by the specified 'timeZoneId',
        // or 0 if the operation does not succeed.  If the information for
        // 'timeZoneId' has not been previously cached, then attempt to
        // populate this object using the 'loader' supplied at construction.
        // Optionally specify the address of an integer, 'rc', in which to load
        // the return code for this operation.  If 'rc' is specified, load 0
        // into 'rc' if the operation succeeds,
        // 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID' if the time-zone
        // identifier is not supported, and a negative value if the operation
        // does not succeed for any other reason.  If the returned address is
        // non-zero, the Zoneinfo object returned is guaranteed to be
        // well-formed (i.e., 'baetzo_ZoneinfoUtil::isWellFormed will return
        // 'true' if called with the returned value), and remain valid for
        // the lifetime of this object.  The behavior is undefined if 'rc' is
        // 0.

    // ACCESSORS
    const baetzo_Zoneinfo *lookupZoneinfo(const char *timeZoneId) const;
        // Return the address of the non-modifiable cached description of the
        // time zone identified by the specified 'timeZoneId', and 0 if
        // information for 'timeZoneId' has not previously been cached (by a
        // call to 'getZoneinfo').  If the returned address is non-zero, the
        // Zoneinfo object returned is guaranteed to be well-formed (i.e.,
        // 'baetzo_ZoneinfoUtil::isWellFormed will return 'true' if called
        // with the returned value), and remain valid for the lifetime of this
        // object.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                   // ---------------------------------------
                   // class baetzo_ZoneinfoCache::CStringLess
                   // ---------------------------------------

// ACCESSORS
inline
bool baetzo_ZoneinfoCache::CStringLess::operator()(const char *lhs,
                                                   const char *rhs) const
{
    return bsl::strcmp(lhs, rhs) < 0;
}

                        // --------------------------
                        // class baetzo_ZoneinfoCache
                        // --------------------------

// CREATORS
inline
baetzo_ZoneinfoCache::baetzo_ZoneinfoCache(baetzo_Loader   *loader,
                                           bslma_Allocator *basicAllocator)
: d_cache(basicAllocator)
, d_loader_p(loader)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT_SAFE(0 != loader);
}

// MANIPULATORS
inline
const baetzo_Zoneinfo *baetzo_ZoneinfoCache::getZoneinfo(
                                                        const char *timeZoneId)
{
    BSLS_ASSERT_SAFE(0 != timeZoneId);

    int rc;
    return getZoneinfo(&rc, timeZoneId);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
