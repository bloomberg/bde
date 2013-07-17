// bdet_calendarcache.h                                               -*-C++-*-
#ifndef INCLUDED_BDET_CALENDARCACHE
#define INCLUDED_BDET_CALENDARCACHE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an efficient cache for read-only 'bdet_Calendar' objects.
//
//@CLASSES:
//  bdet_CalendarCache: cache for read-only calendars that are loaded on demand
//  bdet_CalendarCacheEntryPtr: pointer to shared calendar cache entry
//
//@AUTHOR: Guillaume Morin (gmorin1), Yu Zhao (yzhao3)
//
//@SEE_ALSO: bdet_calendar, bdet_calendarloader
//
//@DESCRIPTION: This component provides the 'bdet_CalendarCache' class, a cache
// for 'bdet_Calendar' objects, and the 'bdet_CalendarCacheEntryPtr' class that
// provides access to individual calendars retrieved from the cache.  The
// 'bdet_CalendarCache' class defines two methods for fetching calendars from
// the cache, a manipulator called 'calendar' and an accessor also called
// 'calendar'.  Calendars are identified by name using C-style strings, and
// both retrieval methods return a 'bdet_CalendarCacheEntryPtr'.  Actual access
// to the (non-modifiable) 'bdet_Calendar' objects stored in the cache is
// through the 'operator->' method provided by 'bdet_CalendarCacheEntryPtr'.
//
// The first time a calendar is requested from the cache using the 'calendar'
// *manipulator*, the identified calendar is loaded into the cache using the
// loader that was supplied upon construction of the cache (see
// 'bdet_calendarloader'); a reference to that newly-loaded calendar is then
// returned.  Subsequent requests for the same calendar, using either
// 'calendar' method, are efficiently satisfied by returning references to the
// cached instance.  The 'calendar' accessor differs from its like-named
// manipulator in that when a request is made through the accessor for a
// calendar that is *not* present in the cache, the calendar is not loaded as
// a side-effect.  In this case, an empty 'bdet_CalendarCacheEntryPtr' is
// returned instead, which is effectively a null pointer.  Note that the
// calendar-naming convention in effect for a given cache is determined by the
// concrete loader supplied at construction of the cache.
//
// Calendars stored in a cache can be explicitly invalidated--the 'invalidate'
// method is used to invalidate a single calendar and 'invalidateAll'
// invalidates all calendars in the cache.  Invalidated calendars are removed
// from the cache.  However, a calendar that has been invalidated in the cache
// remains valid to all outstanding references to it, obtained via earlier
// calls to the 'calendar' methods, until all of those references have been
// destroyed.  Note that a subsequent request, using the 'calendar'
// manipulator, for a calendar that has been invalidated simply incurs the
// overhead of once again loading that calendar into the cache.
//
// Calendars can also be invalidated on the basis of a timeout.  To use this
// feature of 'bdet_CalendarCache', a 'bdet_TimeInterval' timeout must be
// supplied at construction.  When a timeout is in effect for a cache, requests
// for a calendar from the cache using the 'calendar' manipulator may incur the
// reloading of the calendar if the one in the cache has expired (i.e., the
// time interval defined by the timeout value has elapsed since the calendar
// was last loaded).  In the case of the 'calendar' *accessor*, an empty
// 'bdet_CalendarCacheEntryPtr' is returned if the requested calendar is found
// to have expired.
//
///Thread-Safety
///-------------
// The 'bdet_CalendarCacheEntryPtr' and 'bdet_CalendarCache' classes are fully
// thread-safe (see 'bsldoc_glossary').
//
///Usage
///-----
// The following examples illustrate how to use a 'bdet_CalendarCache'.
//
///Example 1: Using a 'bdet_CalendarCache'
///- - - - - - - - - - - - - - - - - - - -
// This first example shows basic use of a 'bdet_CalendarCache' object with no
// timeout value specified at construction.
//
// In this example and the next, we assume a hypothetical calendar loader,
// 'my_CalendarLoader', the details of which are not important other than that
// it supports calendars identified by "DE", "FR", and "US", which nominally
// identify the major holidays in Germany, France, and the United States,
// respectively.  Furthermore, we cite two specific dates of interest:
// 2011/07/04, which was a holiday in the US (Independence Day), but not in
// France, and 2011/07/14, which was a holiday in France (Bastille Day), but
// not in the US.
//
// First, we create a calendar loader, an instance of 'my_CalendarLoader', and
// use it, in turn, to create a cache.  For the purposes of this example, it is
// sufficient to let the cache use the default allocator:
//..
//  my_CalendarLoader  loader;
//  bdet_CalendarCache cache(&loader);
//..
// Next, we retrieve the calendar identified by "US", verify that the loading
// of that calendar into the cache was successful ('usA.ptr()' is non-null),
// and verify that 2011/07/04 is recognized as a holiday in the "US" calendar,
// whereas 2011/07/14 is not:
//..
//  bdet_CalendarCacheEntryPtr usA = cache.calendar("US");
//
//                             assert( usA.ptr());
//                             assert( usA->isHoliday(bdet_Date(2011, 7,  4)));
//                             assert(!usA->isHoliday(bdet_Date(2011, 7, 14)));
//..
// Next, we fetch the calendar identified by "FR", this time verifying that
// 2011/07/14 is recognized as a holiday in the "FR" calendar, but 2011/07/04
// is not:
//..
//  bdet_CalendarCacheEntryPtr frA = cache.calendar("FR");
//
//                             assert( frA.ptr());
//                             assert(!frA->isHoliday(bdet_Date(2011, 7,  4)));
//                             assert( frA->isHoliday(bdet_Date(2011, 7, 14)));
//..
// Next, we retrieve the "FR" calendar again, this time via the 'calendar'
// accessor, and note that the request is satisfied by the calendar that is
// already in the cache:
//..
//  const bdet_CalendarCache& readonlyCache = cache;
//
//  bdet_CalendarCacheEntryPtr frB = readonlyCache.calendar("FR");
//
//                             assert( frA.ptr() == frB.ptr());
//..
// Next, we invalidate the "US" calendar in the cache and immediately fetch it
// again.  The call to 'invalidate' removed the "US" calendar from the cache,
// so it had to be reloaded into the cache to satisfy the request:
//..
//  cache.invalidate("US");
//
//  bdet_CalendarCacheEntryPtr usB = cache.calendar("US");
//
//                             assert( usB.ptr() != usA.ptr());
//                             assert( usB.ptr());
//                             assert( usB->isHoliday(bdet_Date(2011, 7,  4)));
//                             assert(!usB->isHoliday(bdet_Date(2011, 7, 14)));
//..
// Next, all calendars in the cache are invalidated, then reloaded:
//..
//  cache.invalidateAll();
//
//  bdet_CalendarCacheEntryPtr usC = cache.calendar("US");
//
//                             assert( usC.ptr() != usA.ptr());
//                             assert( usC.ptr() != usB.ptr());
//                             assert( usC.ptr());
//                             assert( usC->isHoliday(bdet_Date(2011, 7,  4)));
//                             assert(!usC->isHoliday(bdet_Date(2011, 7, 14)));
//
//  bdet_CalendarCacheEntryPtr frC = cache.calendar("FR");
//
//                             assert( frC.ptr() != frA.ptr());
//                             assert( frC.ptr() != frB.ptr());
//                             assert( frC.ptr());
//                             assert(!frC->isHoliday(bdet_Date(2011, 7,  4)));
//                             assert( frC->isHoliday(bdet_Date(2011, 7, 14)));
//..
// Now, verify that references to calendars that were invalidated in the cache
// are still valid for clients that obtained references to them before they
// were made invalid:
//..
//                             assert( usA->isHoliday(bdet_Date(2011, 7,  4)));
//                             assert(!usA->isHoliday(bdet_Date(2011, 7, 14)));
//
//                             assert( usB->isHoliday(bdet_Date(2011, 7,  4)));
//                             assert(!usB->isHoliday(bdet_Date(2011, 7, 14)));
//
//                             assert(!frA->isHoliday(bdet_Date(2011, 7,  4)));
//                             assert( frA->isHoliday(bdet_Date(2011, 7, 14)));
//
//                             assert(!frB->isHoliday(bdet_Date(2011, 7,  4)));
//                             assert( frB->isHoliday(bdet_Date(2011, 7, 14)));
//..
// When 'usA', 'usB', 'frA', and 'frB' go out of scope, the resources used by
// the calendars to which they refer are automatically reclaimed.
//
// Finally, using the 'calendar' accessor, we attempt to retrieve a calendar
// that has not yet been loaded into the cache, but which we *know* to be
// supported by the calendar loader.  Since the 'calendar' accessor does not
// load calendars into the cache as a side-effect, the request fails:
//..
//  bdet_CalendarCacheEntryPtr de = readonlyCache.calendar("DE");
//
//                             assert(!de.ptr());
//..
///Example 2: A Calendar Cache with a Timeout
///- - - - - - - - - - - - - - - - - - - - -
// This second example shows the affects on a 'bdet_CalendarCache' object that
// is constructed to have a timeout value.  Note that the following snippets of
// code assume a platform-independent 'sleepSeconds' method that sleeps for the
// specified number of seconds.
//
// First, we create a calendar loader and a calendar cache.  The cache is
// constructed to have a timeout of 3 seconds.  Of course, such a short timeout
// is inappropriate for production use, but it is necessary for illustrating
// the affects of a timeout in this example.  As in example 1 (above), we again
// let the cache use the default allocator:
//..
//  my_CalendarLoader  loader;
//  bdet_CalendarCache cache(&loader, bdet_TimeInterval(3));
//  const bdet_CalendarCache& readonlyCache = cache;
//..
// Next, we retrieve the calendar identified by "DE" from the cache:
//..
//  bdet_CalendarCacheEntryPtr deA = cache.calendar("DE");
//
//                             assert( deA.ptr());
//..
// Next, we sleep for 2 seconds before retrieving the "FR" calendar:
//..
//  sleepSeconds(2);
//
//  bdet_CalendarCacheEntryPtr frA = cache.calendar("FR");
//
//                             assert( frA.ptr());
//..
// Next, we sleep for 2 more seconds before attempting to retrieve the "DE"
// calendar again, this time using the 'calendar' *accessor*.  Since the
// cumulative sleep time exceeds the timeout value established for the cache
// when it was constructed, the "DE" calendar has expired; hence, it has been
// removed from the cache:
//..
//  sleepSeconds(2);
//
//  bdet_CalendarCacheEntryPtr deB = readonlyCache.calendar("DE");
//
//                             assert(!deB.ptr());
//..
// Next, we verify that the "FR" calendar is still available in the cache:
//..
//  bdet_CalendarCacheEntryPtr frB = readonlyCache.calendar("FR");
//
//                             assert( frA.ptr() == frB.ptr());
//..
// Finally, we sleep for an additional 2 seconds and verify that the "FR"
// calendar has also expired:
//..
//  sleepSeconds(2);
//
//  bdet_CalendarCacheEntryPtr frC = readonlyCache.calendar("FR");
//
//                             assert(!frC.ptr());
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDET_CALENDAR
#include <bdet_calendar.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_MUTEXIMP
#include <bsls_muteximp.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

class bdet_CalendarLoader;
class bdet_CalendarCache_EntryPtrRep;

                      // ================================
                      // class bdet_CalendarCacheEntryPtr
                      // ================================

class bdet_CalendarCacheEntryPtr {
    // This class provides access, via its 'operator->', to a shared calendar
    // cache entry.  The lifetime of each shared calendar is managed by a
    // reference-counted representation object.  Accordingly, references to a
    // shared calendar via instances of this class remain valid even after the
    // calendar has been removed from the cache from which the references were
    // first obtained.
    //
    // This class is fully thread-safe (see 'bsldoc_glossary').

    // DATA
    bdet_CalendarCache_EntryPtrRep *d_rep_p;  // pointer to shared rep (held,
                                              // not owned)

    // FRIENDS
    friend class bdet_CalendarCache;

  private:
    // PRIVATE CREATORS
    bdet_CalendarCacheEntryPtr(bdet_CalendarCache_EntryPtrRep *rep);
        // Create a calendar cache entry pointer that refers to the specified
        // 'rep', and atomically increase the number of references to 'rep' by
        // 1.

    // PRIVATE ACCESSORS
    const bdet_CalendarCache_EntryPtrRep *rep() const;
        // Return an address providing non-modifiable access to the shared
        // representation object referred to by this calendar cache entry
        // pointer, if any, and 0 otherwise.

  public:
    // CREATORS
    bdet_CalendarCacheEntryPtr();
        // Create an empty calendar cache entry pointer, i.e., one that does
        // not refer to any calendar.

    bdet_CalendarCacheEntryPtr(const bdet_CalendarCacheEntryPtr& original);
        // Create a calendar cache entry pointer that refers to the same shared
        // calendar as the specified 'original' object.

    ~bdet_CalendarCacheEntryPtr();
        // Destroy this calendar cache entry pointer.

    // MANIPULATORS
    bdet_CalendarCacheEntryPtr& operator=(
                                        const bdet_CalendarCacheEntryPtr& rhs);
        // Make this calendar cache entry pointer refer to the same shared
        // calendar as the specified 'rhs', and return a reference providing
        // modifiable access to this object.

    // ACCESSORS
    const bdet_Calendar *operator->() const;
        // Return an address providing non-modifiable access to the shared
        // calendar referred to by this object, if any, and 0 otherwise.

    const bdet_Calendar *ptr() const;
        // Return an address providing non-modifiable access to the shared
        // calendar referred to by this object, if any, and 0 otherwise.
};

                        // ========================
                        // class bdet_CalendarCache
                        // ========================

class bdet_CalendarCache {
    // This class implements an efficient cache of *read*-*only*
    // 'bdet_Calendar' objects that are loaded into the cache, using a calendar
    // loader supplied at construction, as a side-effect of the 'calendar'
    // manipulator.  Calendars in the cache can be invalidated, and removed
    // from the cache as a consequence, with the 'invalidate' and
    // 'invalidateAll' methods.  Similarly, calendars in the cache can be made
    // to expire based on an optional timeout constructor argument, with
    // expired calendars also being removed from the cache.  The
    // 'bdet_CalendarCacheEntryPtr' objects returned from the 'calendar'
    // methods allow for the safe removal of calendars from the cache that may
    // still have outstanding references to them.
    //
    // This container is *exception* *neutral* with no guarantee of rollback:
    // if an exception is thrown during the invocation of a method on a
    // pre-existing instance, the container is left in a valid state, but its
    // value is undefined.  In no event is memory leaked.
    //
    // This class is fully thread-safe (see 'bsldoc_glossary').

    // DATA
    mutable bsl::map<bsl::string, bdet_CalendarCacheEntryPtr>
                            d_cache;           // cache of (name, handle) pairs

    bdet_CalendarLoader    *d_loader_p;        // calendar loader (held, not
                                               // owned)

    bdet_TimeInterval       d_timeOut;         // timeout value; ignored unless
                                               // 'd_hasTimeOutFlag' is 'true'

    bool                    d_hasTimeOutFlag;  // 'true' if this cache has a
                                               // timeout value and 'false'
                                               // otherwise

    mutable bsls::MutexImp  d_lock;            // guard access to cache

    bslma::Allocator       *d_allocator_p;     // memory allocator (held, not
                                               // owned)

  private:
    // PRIVATE TYPES
    typedef bsl::map<bsl::string, bdet_CalendarCacheEntryPtr>::iterator
                                                                 CacheIterator;

    typedef bsl::map<bsl::string, bdet_CalendarCacheEntryPtr>::const_iterator
                                                            ConstCacheIterator;

  private:
    // NOT IMPLEMENTED
    bdet_CalendarCache(const bdet_CalendarCache&);
    bdet_CalendarCache& operator=(const bdet_CalendarCache&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdet_CalendarCache,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    bdet_CalendarCache(bdet_CalendarLoader      *loader,
                       bslma::Allocator         *basicAllocator = 0);
    bdet_CalendarCache(bdet_CalendarLoader      *loader,
                       const bdet_TimeInterval&  timeout,
                       bslma::Allocator         *basicAllocator = 0);
        // Create an empty calendar cache that uses the specified 'loader' to
        // load calendars on demand.  Optionally specify a 'timeout' interval
        // indicating the length of time that calendars remain valid for
        // subsequent retrieval from the cache after they have been loaded.  If
        // 'timeout' is not specified, calendars loaded into this cache remain
        // valid for retrieval until they have been explicitly invalidated (via
        // either the 'invalidate' or 'invalidateAll' methods), or until this
        // object is destroyed.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // 'loader' remains valid throughout the lifetime of this cache, and
        // 'timeout' (if specified) fits in a 64-bit integer when converted to
        // milliseconds.  Note that a non-positive 'timeout' (if specified)
        // indicates that a calendar will be loaded into the cache by *each*
        // (successful) call to the 'calendar' manipulator.

    ~bdet_CalendarCache();
        // Destroy this object.

    // MANIPULATORS
    bdet_CalendarCacheEntryPtr calendar(const char *calendarName);
        // Return a cache entry pointer providing non-modifiable access to the
        // calendar having the specified 'calendarName' in this calendar cache,
        // loading the calendar into the cache using the loader that was
        // supplied at construction if the calendar is not already present in
        // the cache or if the calendar has expired (i.e., per a timeout
        // optionally supplied at construction).  If the loader fails, whether
        // in loading a calendar for the first time or in reloading a calendar
        // that has expired, return an empty cache entry pointer.

    void invalidate(const char *calendarName);
        // Invalidate the calendar having the specified 'calendarName' in this
        // calendar cache, and remove it from the cache.  If a calendar having
        // 'calendarName' is not present in this cache, this method has no
        // effect.  Note that a calendar that has been invalidated in the cache
        // remains valid to all outstanding references to it, obtained via
        // earlier calls to the 'calendar' methods, until all of those
        // references have been destroyed.

    void invalidateAll();
        // Invalidate all calendars in this calendar cache, and remove them
        // from the cache.  Note that a calendar that has been invalidated in
        // the cache remains valid to all outstanding references to it,
        // obtained via earlier calls to the 'calendar' methods, until all of
        // those references have been destroyed.

    // ACCESSORS
    bdet_CalendarCacheEntryPtr calendar(const char *calendarName) const;
        // Return a cache entry pointer providing non-modifiable access to the
        // calendar having the specified 'calendarName' in this calendar cache.
        // If the calendar having 'calendarName' is not found in the cache or
        // if the calendar has expired (i.e., per a timeout optionally supplied
        // at construction), return an empty cache entry pointer.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
