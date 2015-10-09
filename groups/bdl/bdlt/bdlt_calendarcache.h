// bdlt_calendarcache.h                                               -*-C++-*-
#ifndef INCLUDED_BDLT_CALENDARCACHE
#define INCLUDED_BDLT_CALENDARCACHE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an efficient cache for read-only 'bdlt::Calendar' objects.
//
//@CLASSES:
// bdlt::CalendarCache: cache for read-only calendars that are loaded on demand
//
//@SEE_ALSO: bdlt_calendar, bdlt_calendarloader
//
//@DESCRIPTION: This component defines the 'bdlt::CalendarCache' class, a cache
// for read-only 'bdlt::Calendar' objects.  The 'bdlt::CalendarCache' class
// defines two methods for fetching calendars from the cache: a manipulator
// called 'getCalendar' and an accessor called 'lookupCalendar'.  Calendars are
// identified by name using C-style strings, and both retrieval methods return
// a 'bsl::shared_ptr<const bdlt::Calendar>'.
//
// The first time a calendar is requested from the cache using the
// 'getCalendar' manipulator, the identified calendar is loaded into the cache
// using the loader that was supplied upon construction of the cache (see
// 'bdlt_calendarloader'); a reference to that newly-loaded calendar is then
// returned.  Subsequent requests for the same calendar, using either the
// 'getCalendar' or 'lookupCalendar' method, are efficiently satisfied by
// returning references to the cached instance.  The 'lookupCalendar' accessor
// differs from the 'getCalendar' manipulator in that when a request is made
// through the accessor for a calendar that is *not* present in the cache, the
// calendar is not loaded as a side-effect.  In this case, an empty
// 'bsl::shared_ptr<const bdlt::Calendar>' is returned instead, which is
// effectively a null pointer.  Note that the calendar-naming convention in
// effect for a given cache is determined by the concrete loader supplied at
// construction of the cache.
//
// Calendars stored in a cache can be explicitly invalidated; the 'invalidate'
// method is used to invalidate a single calendar and 'invalidateAll'
// invalidates all calendars in the cache.  Invalidated calendars are removed
// from the cache.  However, a calendar that has been invalidated in the cache
// remains valid to all outstanding references to it, obtained via earlier
// calls to the 'getCalendar' and 'lookupCalendar' methods, until all of those
// references have been destroyed.  Note that a subsequent request, using the
// 'getCalendar' manipulator, for a calendar that has been invalidated incurs
// the overhead of once again loading that calendar into the cache.
//
// Calendars can also be invalidated on the basis of a timeout.  To use this
// feature of 'bdlt::CalendarCache', a 'bsls::TimeInterval' timeout must be
// supplied at construction.  When a timeout is in effect for a cache, requests
// for a calendar from the cache using the 'getCalendar' manipulator may incur
// the reloading of the calendar if the one in the cache has expired (i.e., the
// time interval defined by the timeout value has elapsed since the calendar
// was last loaded).  In the case of the 'lookupCalendar' accessor, an empty
// 'bsl::shared_ptr<const bdlt::Calendar>' is returned if the requested
// calendar is found to have expired.
//
///Thread Safety
///-------------
// The 'bdlt::CalendarCache' class is fully thread-safe (see 'bsldoc_glossary')
// provided that the allocator supplied at construction and the default
// allocator in effect during the lifetime of cache objects are both fully
// thread-safe.
//
///Usage
///-----
// The following example illustrates how to use a 'bdlt::CalendarCache'.
//
///Example 1: Using a 'bdlt::CalendarCache'
/// - - - - - - - - - - - - - - - - - - - -
// This example shows basic use of a 'bdlt::CalendarCache' object.
//
// In this example, we assume a hypothetical calendar loader,
// 'MyCalendarLoader', the details of which are not important other than that
// it supports calendars identified by "DE", "FR", and "US", which nominally
// identify the major holidays in Germany, France, and the United States,
// respectively.  Furthermore, we cite two specific dates of interest:
// 2011/07/04, which was a holiday in the US (Independence Day), but not in
// France, and 2011/07/14, which was a holiday in France (Bastille Day), but
// not in the US.  Note that neither of these dates were holidays in Germany.
//
// First, we create a calendar loader, an instance of 'MyCalendarLoader', and
// use it, in turn, to create a cache.  For the purposes of this example, it is
// sufficient to let the cache use the default allocator:
//..
//  MyCalendarLoader    loader;
//  bdlt::CalendarCache cache(&loader);
//..
// Next, we retrieve the calendar 'usA', identified by "US", verify that the
// loading of that calendar into the cache was successful ('usA.get()' is
// non-null), and verify that 2011/07/04 is recognized as a holiday in the "US"
// calendar, whereas 2011/07/14 is not:
//..
//  bsl::shared_ptr<const bdlt::Calendar> usA = cache.getCalendar("US");
//
//                            assert( usA.get());
//                            assert( usA->isHoliday(bdlt::Date(2011, 7,  4)));
//                            assert(!usA->isHoliday(bdlt::Date(2011, 7, 14)));
//..
// Then, we fetch the calendar identified by "FR", this time verifying that
// 2011/07/14 is recognized as a holiday in the "FR" calendar, but 2011/07/04
// is not:
//..
//  bsl::shared_ptr<const bdlt::Calendar> frA = cache.getCalendar("FR");
//
//                            assert( frA.get());
//                            assert(!frA->isHoliday(bdlt::Date(2011, 7,  4)));
//                            assert( frA->isHoliday(bdlt::Date(2011, 7, 14)));
//..
// Next, we retrieve the "FR" calendar again, this time via the
// 'lookupCalendar' accessor, and note that the request is satisfied by the
// calendar that is already in the cache:
//..
//  const bdlt::CalendarCache& readonlyCache = cache;
//
//  bsl::shared_ptr<const bdlt::Calendar> frB =
//                                          readonlyCache.lookupCalendar("FR");
//
//                            assert( frA.get() == frB.get());
//..
// Then, we invalidate the "US" calendar in the cache and immediately fetch it
// again.  The call to 'invalidate' removed the "US" calendar from the cache,
// so it had to be reloaded into the cache to satisfy the request:
//..
//  int numInvalidated = cache.invalidate("US");
//                            assert(1 == numInvalidated);
//
//  bsl::shared_ptr<const bdlt::Calendar> usB = cache.getCalendar("US");
//
//                            assert( usB.get() != usA.get());
//                            assert( usB.get());
//                            assert( usB->isHoliday(bdlt::Date(2011, 7,  4)));
//                            assert(!usB->isHoliday(bdlt::Date(2011, 7, 14)));
//..
// Next, all calendars in the cache are invalidated, then reloaded:
//..
//  numInvalidated = cache.invalidateAll();
//                            assert(2 == numInvalidated);
//
//  bsl::shared_ptr<const bdlt::Calendar> usC = cache.getCalendar("US");
//
//                            assert( usC.get() != usA.get());
//                            assert( usC.get() != usB.get());
//                            assert( usC.get());
//                            assert( usC->isHoliday(bdlt::Date(2011, 7,  4)));
//                            assert(!usC->isHoliday(bdlt::Date(2011, 7, 14)));
//
//  bsl::shared_ptr<const bdlt::Calendar> frC = cache.getCalendar("FR");
//
//                            assert( frC.get() != frA.get());
//                            assert( frC.get() != frB.get());
//                            assert( frC.get());
//                            assert(!frC->isHoliday(bdlt::Date(2011, 7,  4)));
//                            assert( frC->isHoliday(bdlt::Date(2011, 7, 14)));
//..
// Now, verify that references to calendars that were invalidated in the cache
// are still valid for clients that obtained references to them before they
// were made invalid:
//..
//                            assert( usA->isHoliday(bdlt::Date(2011, 7,  4)));
//                            assert(!usA->isHoliday(bdlt::Date(2011, 7, 14)));
//
//                            assert( usB->isHoliday(bdlt::Date(2011, 7,  4)));
//                            assert(!usB->isHoliday(bdlt::Date(2011, 7, 14)));
//
//                            assert(!frA->isHoliday(bdlt::Date(2011, 7,  4)));
//                            assert( frA->isHoliday(bdlt::Date(2011, 7, 14)));
//
//                            assert(!frB->isHoliday(bdlt::Date(2011, 7,  4)));
//                            assert( frB->isHoliday(bdlt::Date(2011, 7, 14)));
//..
// When 'usA', 'usB', 'frA', and 'frB' go out of scope, the resources used by
// the calendars to which they refer are automatically reclaimed.
//
// Finally, using the 'lookupCalendar' accessor, we attempt to retrieve a
// calendar that has not yet been loaded into the cache, but that we *know* to
// be supported by the calendar loader.  Since the 'lookupCalendar' accessor
// does not load calendars into the cache as a side-effect, the request fails:
//..
//  bsl::shared_ptr<const bdlt::Calendar> de =
//                                          readonlyCache.lookupCalendar("DE");
//
//                            assert(!de.get());
//..
//
///Example 2: A Calendar Cache with a Timeout
/// - - - - - - - - - - - - - - - - - - - - -
// This second example shows the affects on a 'bdlt::CalendarCache' object that
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
//  MyCalendarLoader           loader;
//  bdlt::CalendarCache        cache(&loader, bsls::TimeInterval(3));
//  const bdlt::CalendarCache& readonlyCache = cache;
//..
// Next, we retrieve the calendar identified by "DE" from the cache:
//..
//  bsl::shared_ptr<const bdlt::Calendar> deA = cache.getCalendar("DE");
//
//                            assert( deA.get());
//..
// Next, we sleep for 2 seconds before retrieving the "FR" calendar:
//..
//  sleepSeconds(2);
//
//  bsl::shared_ptr<const bdlt::Calendar> frA = cache.getCalendar("FR");
//
//                            assert( frA.get());
//..
// Next, we sleep for 2 more seconds before attempting to retrieve the "DE"
// calendar again, this time using the 'lookupCalendar' accessor.  Since the
// cumulative sleep time exceeds the timeout value established for the cache
// when it was constructed, the "DE" calendar has expired; hence, it has been
// removed from the cache:
//..
//  sleepSeconds(2);
//
//  bsl::shared_ptr<const bdlt::Calendar> deB =
//                                          readonlyCache.lookupCalendar("DE");
//
//                            assert(!deB.get());
//..
// Next, we verify that the "FR" calendar is still available in the cache:
//..
//  bsl::shared_ptr<const bdlt::Calendar> frB =
//                                          readonlyCache.lookupCalendar("FR");
//
//                            assert( frA.get() == frB.get());
//..
// Finally, we sleep for an additional 2 seconds and verify that the "FR"
// calendar has also expired:
//..
//  sleepSeconds(2);
//
//  bsl::shared_ptr<const bdlt::Calendar> frC =
//                                          readonlyCache.lookupCalendar("FR");
//
//                            assert(!frC.get());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_CALENDAR
#include <bdlt_calendar.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLS_BSLLOCK
#include <bsls_bsllock.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSL_CTIME
#include <bsl_ctime.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>  // 'bsl::shared_ptr'
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {
namespace bdlt {

class CalendarLoader;
class CalendarCache_Entry;

                        // ===================
                        // class CalendarCache
                        // ===================

class CalendarCache {
    // This class implements an efficient cache of *read-only*
    // 'bdlt::Calendar' objects that are loaded into the cache, using a
    // calendar loader supplied at construction, as a side-effect of the
    // 'getCalendar' manipulator.  Calendars in the cache can be invalidated,
    // and removed from the cache via the 'invalidate' and 'invalidateAll'
    // methods.  In addition, calendars in the cache can be made to expire
    // based on a timeout that may be optionally supplied at construction.  The
    // 'bsl::shared_ptr<const bdlt::Calendar>' objects returned from the
    // 'getCalendar' and 'lookupCalendar' methods allow for the safe removal of
    // calendars from the cache that may still have outstanding references to
    // them.
    //
    // This container is *exception* *neutral* with no guarantee of rollback:
    // if an exception is thrown during the invocation of a method on a
    // pre-existing instance, the container is left in a valid state, but its
    // value is undefined.  In no event is memory leaked.
    //
    // This class is fully thread-safe (see 'bsldoc_glossary').

    // DATA
    mutable bsl::map<bsl::string, CalendarCache_Entry>
                            d_cache;           // cache of (name, handle) pairs

    CalendarLoader         *d_loader_p;        // calendar loader (held, not
                                               // owned)

    bsl::time_t             d_timeOut;         // timeout value; ignored unless
                                               // 'd_hasTimeOutFlag' is 'true'

    bool                    d_hasTimeOutFlag;  // 'true' if this cache has a
                                               // timeout value and 'false'
                                               // otherwise

    mutable bsls::BslLock   d_lock;            // guard access to cache

    bslma::Allocator       *d_allocator_p;     // memory allocator (held, not
                                               // owned)

  private:
    // PRIVATE TYPES
    typedef bsl::map<bsl::string, CalendarCache_Entry>::iterator CacheIterator;

    typedef bsl::map<bsl::string, CalendarCache_Entry>::const_iterator
                                                            ConstCacheIterator;

  private:
    // NOT IMPLEMENTED
    CalendarCache(const CalendarCache&);
    CalendarCache& operator=(const CalendarCache&);

  public:
    // CREATORS
    explicit
    CalendarCache(CalendarLoader   *loader,
                  bslma::Allocator *basicAllocator = 0);
        // Create an empty calendar cache that uses the specified 'loader' to
        // load calendars on demand and has no timeout.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  Calendars loaded
        // into this cache remain valid for retrieval until they have been
        // explicitly invalidated (via either the 'invalidate' or
        // 'invalidateAll' methods), or until this object is destroyed.  The
        // behavior is undefined unless 'loader' and the indicated allocator
        // remain valid throughout the lifetime of this cache.

    CalendarCache(CalendarLoader            *loader,
                  const bsls::TimeInterval&  timeout,
                  bslma::Allocator          *basicAllocator = 0);
        // Create an empty calendar cache that uses the specified 'loader' to
        // load calendars on demand and has the specified 'timeout' interval
        // indicating the length of time that calendars remain valid for
        // subsequent retrieval from the cache after they have been loaded.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless 'loader' and the indicated
        // allocator remain valid throughout the lifetime of this cache, and
        // 'bsls::TimeInterval(0) <= timeout <= bsls::TimeInterval(INT_MAX)'.
        // Note that a 'timeout' value of 0 indicates that a calendar will be
        // loaded into the cache by *each* (successful) call to the
        // 'getCalendar' manipulator.

    ~CalendarCache();
        // Destroy this object.

    // MANIPULATORS
    bsl::shared_ptr<const Calendar> getCalendar(const char *calendarName);
        // Return a shared pointer providing non-modifiable access to the
        // calendar having the specified 'calendarName' in this calendar cache,
        // loading the calendar into the cache using the loader that was
        // supplied at construction if the calendar is not already present in
        // the cache or if the calendar has expired (i.e., per a timeout
        // optionally supplied at construction).  If the loader fails, whether
        // in loading a calendar for the first time or in reloading a calendar
        // that has expired, return an empty shared pointer.

    int invalidate(const char *calendarName);
        // Invalidate the calendar having the specified 'calendarName' in this
        // calendar cache, and remove it from the cache.  If a calendar having
        // 'calendarName' is not present in this cache, this method has no
        // effect.  Return the number of calendars that were invalidated.  Note
        // that a calendar that has been invalidated in the cache remains valid
        // to all outstanding references to it, obtained via earlier calls to
        // the 'getCalendar' and 'lookupCalendar' methods, until all of those
        // references have been destroyed.

    int invalidateAll();
        // Invalidate all calendars in this calendar cache, and remove them
        // from the cache.  Return the number of calendars that were
        // invalidated.  Note that a calendar that has been invalidated in the
        // cache remains valid to all outstanding references to it, obtained
        // via earlier calls to the 'getCalendar' and 'lookupCalendar' methods,
        // until all of those references have been destroyed.

    // ACCESSORS
    bsl::shared_ptr<const Calendar>
    lookupCalendar(const char *calendarName) const;
        // Return a shared pointer providing non-modifiable access to the
        // calendar having the specified 'calendarName' in this calendar cache.
        // If the calendar having 'calendarName' is not found in the cache, or
        // if the calendar has expired (i.e., per a timeout optionally supplied
        // at construction), return an empty shared pointer.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

}  // close package namespace
}  // close enterprise namespace

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<bdlt::CalendarCache> : bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace

#endif

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
