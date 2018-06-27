// bdlt_timetablecache.h                                              -*-C++-*-
#ifndef INCLUDED_BDLT_TIMETABLECACHE
#define INCLUDED_BDLT_TIMETABLECACHE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an efficient cache for read-only 'bdlt::Timetable' objects.
//
//@CLASSES:
// bdlt::TimetableCache: cache for read-only timetables loaded on demand
//
//@SEE_ALSO: bdlt_timetable, bdlt_timetableloader
//
//@DESCRIPTION: This component defines the 'bdlt::TimetableCache' class, a
// cache for read-only 'bdlt::Timetable' objects.  The 'bdlt::TimetableCache'
// class defines two methods for fetching timetables from the cache: a
// manipulator called 'getTimetable' and an accessor called 'lookupTimetable'.
// Timetables are identified by name using C-style strings, and both retrieval
// methods return a 'bsl::shared_ptr<const bdlt::Timetable>'.
//
// The first time a timetable is requested from the cache using the
// 'getTimetable' manipulator, the identified timetable is loaded into the
// cache using the loader that was supplied upon construction of the cache (see
// 'bdlt_timetableloader'); a reference to that newly-loaded timetable is then
// returned.  Subsequent requests for the same timetable, using either the
// 'getTimetable' or 'lookupTimetable' method, are efficiently satisfied by
// returning references to the cached instance.  The 'lookupTimetable' accessor
// differs from the 'getTimetable' manipulator in that when a request is made
// through the accessor for a timetable that is *not* present in the cache, the
// timetable is not loaded as a side-effect.  In this case, an empty
// 'bsl::shared_ptr<const bdlt::Timetable>' is returned instead, which is
// effectively a null pointer.  Note that the timetable-naming convention in
// effect for a given cache is determined by the concrete loader supplied at
// construction of the cache.
//
// Timetables stored in a cache can be explicitly invalidated; the 'invalidate'
// method is used to invalidate a single timetable and 'invalidateAll'
// invalidates all timetables in the cache.  Invalidated timetables are removed
// from the cache.  However, a timetable that has been invalidated in the cache
// remains valid to all outstanding references to it, obtained via earlier
// calls to the 'getTimetable' and 'lookupTimetable' methods, until all of
// those references have been destroyed.  Note that a subsequent request, using
// the 'getTimetable' manipulator, for a timetable that has been invalidated
// incurs the overhead of once again loading that timetable into the cache.
//
// Timetables can also be invalidated on the basis of a timeout.  To use this
// feature of 'bdlt::TimetableCache', a 'bsls::TimeInterval' timeout must be
// supplied at construction.  When a timeout is in effect for a cache, requests
// for a timetable from the cache using the 'getTimetable' manipulator may
// incur the reloading of the timetable if the one in the cache has expired
// (i.e., the time interval defined by the timeout value has elapsed since the
// timetable was last loaded).  In the case of the 'lookupTimetable' accessor,
// an empty 'bsl::shared_ptr<const bdlt::Timetable>' is returned if the
// requested timetable is found to have expired.
//
///Thread Safety
///-------------
// The 'bdlt::TimetableCache' class is fully thread-safe (see
// 'bsldoc_glossary') provided that the allocator supplied at construction and
// the default allocator in effect during the lifetime of cache objects are
// both fully thread-safe.
//
///Usage
///-----
// The following example illustrates how to use a 'bdlt::TimetableCache'.
//
///Example 1: Using a 'bdlt::TimetableCache'
/// - - - - - - - - - - - - - - - - - - - -
// This example shows basic use of a 'bdlt::TimetableCache' object.
//
// In this example, we assume a hypothetical timetable loader,
// 'MyTimetableLoader', the details of which are not important other than that
// it supports timetables identified by "ZERO", "ONE", and "TWO".  Furthermore,
// the value of the initial transition code for each of these timetables is
// given by the timetable's name (e.g., if 'Z' has the value of the timetable
// identified as "ZERO", then '0 == Z.initialTransitionCode()').
//
// First, we create a timetable loader, an instance of 'MyTimetableLoader', and
// use it, in turn, to create a cache.  For the purposes of this example, it is
// sufficient to let the cache use the default allocator:
//..
//  MyTimetableLoader    loader;
//  bdlt::TimetableCache cache(&loader);
//..
// Next, we retrieve the timetable 'twoA', identified by "TWO", verify that the
// loading of that timetable into the cache was successful ('twoA.get()' is
// non-null), and verify that 2 is the value of the initial transition code for
// timetable "TWO":
//..
//  bsl::shared_ptr<const bdlt::Timetable> twoA = cache.getTimetable("TWO");
//
//  assert(twoA.get());
//  assert(2 == twoA->initialTransitionCode());
//..
// Then, we fetch the timetable identified by "ONE", this time verifying that 1
// is the value of the initial transition code for the "ONE" timetable:
//..
//  bsl::shared_ptr<const bdlt::Timetable> oneA = cache.getTimetable("ONE");
//
//  assert(oneA.get());
//  assert(1 == oneA->initialTransitionCode());
//..
// Next, we retrieve the "ONE" timetable again, this time via the
// 'lookupTimetable' accessor, and note that the request is satisfied by the
// timetable that is already in the cache:
//..
//  const bdlt::TimetableCache& readonlyCache = cache;
//
//  bsl::shared_ptr<const bdlt::Timetable> oneB =
//                                        readonlyCache.lookupTimetable("ONE");
//
//  assert(oneA.get() == oneB.get());
//..
// Then, we invalidate the "TWO" timetable in the cache and immediately fetch
// it again.  The call to 'invalidate' removed the "TWO" timetable from the
// cache, so it had to be reloaded into the cache to satisfy the request:
//..
//  int numInvalidated = cache.invalidate("TWO");
//  assert(1 == numInvalidated);
//
//  bsl::shared_ptr<const bdlt::Timetable> twoB = cache.getTimetable("TWO");
//
//  assert(twoB.get() != twoA.get());
//  assert(twoB.get());
//  assert(2 == twoB->initialTransitionCode());
//..
// Next, all timetables in the cache are invalidated, then reloaded:
//..
//  numInvalidated = cache.invalidateAll();
//  assert(2 == numInvalidated);
//
//  bsl::shared_ptr<const bdlt::Timetable> twoC = cache.getTimetable("TWO");
//
//  assert(twoC.get() != twoA.get());
//  assert(twoC.get() != twoB.get());
//  assert(twoC.get());
//  assert(2 == twoC->initialTransitionCode());
//
//  bsl::shared_ptr<const bdlt::Timetable> oneC = cache.getTimetable("ONE");
//
//  assert(oneC.get() != oneA.get());
//  assert(oneC.get() != oneB.get());
//  assert(oneC.get());
//  assert(1 == oneC->initialTransitionCode());
//..
// Now, verify that references to timetables that were invalidated in the cache
// are still valid for clients that obtained references to them before they
// were made invalid:
//..
//  assert(1 == oneA->initialTransitionCode());
//  assert(1 == oneB->initialTransitionCode());
//
//  assert(2 == twoA->initialTransitionCode());
//  assert(2 == twoB->initialTransitionCode());
//..
// When 'twoA', 'twoB', 'oneA', and 'oneB' go out of scope, the resources used
// by the timetables to which they refer are automatically reclaimed.
//
// Finally, using the 'lookupTimetable' accessor, we attempt to retrieve a
// timetable that has not yet been loaded into the cache, but that we *know* to
// be supported by the timetable loader.  Since the 'lookupTimetable' accessor
// does not load timetables into the cache as a side-effect, the request fails:
//..
//  bsl::shared_ptr<const bdlt::Timetable> zero =
//                                       readonlyCache.lookupTimetable("ZERO");
//
//  assert(!zero.get());
//..
//
///Example 2: A Timetable Cache with a Timeout
/// - - - - - - - - - - - - - - - - - - - - -
// This second example shows the effects on a 'bdlt::TimetableCache' object
// that is constructed to have a timeout value.  Note that the following
// snippets of code assume a platform-independent 'sleepSeconds' method that
// sleeps for the specified number of seconds.
//
// First, we create a timetable loader and a timetable cache.  The cache is
// constructed to have a timeout of 3 seconds.  Of course, such a short timeout
// is inappropriate for production use, but it is necessary for illustrating
// the effects of a timeout in this example.  As in example 1 (above), we again
// let the cache use the default allocator:
//..
//  MyTimetableLoader           loader;
//  bdlt::TimetableCache        cache(&loader, bsls::TimeInterval(3, 0));
//  const bdlt::TimetableCache& readonlyCache = cache;
//..
// Next, we retrieve the timetable identified by "ZERO" from the cache:
//..
//  bsl::shared_ptr<const bdlt::Timetable> zeroA = cache.getTimetable("ZERO");
//
//  assert(zeroA.get());
//..
// Next, we sleep for 2 seconds before retrieving the "ONE" timetable:
//..
//  sleepSeconds(2);
//
//  bsl::shared_ptr<const bdlt::Timetable> oneA = cache.getTimetable("ONE");
//
//  assert(oneA.get());
//..
// Next, we sleep for 2 more seconds before attempting to retrieve the "ZERO"
// timetable again, this time using the 'lookupTimetable' accessor.  Since the
// cumulative sleep time exceeds the timeout value established for the cache
// when it was constructed, the "ZERO" timetable has expired; hence, it has
// been removed from the cache:
//..
//  sleepSeconds(2);
//
//  bsl::shared_ptr<const bdlt::Timetable> zeroB =
//                                       readonlyCache.lookupTimetable("ZERO");
//
//  assert(!zeroB.get());
//..
// Next, we verify that the "ONE" timetable is still available in the cache:
//..
//  bsl::shared_ptr<const bdlt::Timetable> oneB =
//                                        readonlyCache.lookupTimetable("ONE");
//
//  assert(oneA.get() == oneB.get());
//..
// Finally, we sleep for an additional 2 seconds and verify that the "ONE"
// timetable has also expired:
//..
//  sleepSeconds(2);
//
//  bsl::shared_ptr<const bdlt::Timetable> oneC =
//                                        readonlyCache.lookupTimetable("ONE");
//
//  assert(!oneC.get());
//..

#include <bdlscm_version.h>

#include <bdlt_timetable.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimeinterval.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_integralconstant.h>

#include <bslmt_mutex.h>

#include <bsls_timeinterval.h>

#include <bsl_map.h>
#include <bsl_memory.h>  // 'bsl::shared_ptr'
#include <bsl_string.h>

namespace BloombergLP {
namespace bdlt {

class TimetableLoader;
class TimetableCache_Entry;

                        // ==========================
                        // class TimetableCache_Entry
                        // ==========================

// IMPLEMENTATION NOTE: The Sun Studio 12.3 compiler does not support 'map's
// holding types that are incomplete at the point of declaration of a data
// member.  Other compilers allow us to complete 'TimetableCache_Entry' at a
// later point in the code, but before any operation (such as 'insert') that
// would require the type to be complete.  If we did not have to support this
// compiler, this whole class could be defined in the .cpp file; as it stands,
// it *must* be defined before class 'TimetableCache'.

class TimetableCache_Entry {
    // This class defines the type of objects that are inserted into the
    // timetable cache.  Each entry contains a shared pointer to a read-only
    // timetable and the time at which that timetable was loaded.  Note that an
    // explicit allocator is *required* to create an entry object.

    // DATA
    bsl::shared_ptr<const Timetable> d_ptr;       // shared pointer to
                                                  // out-of-place instance

    Datetime                         d_loadTime;  // time when timetable was
                                                  // loaded

  public:
    // CREATORS
    TimetableCache_Entry();
        // Create an empty cache entry object.  Note that an empty cache entry
        // is never actually inserted into the cache.

    TimetableCache_Entry(Timetable        *timetable,
                         const Datetime&   loadTime,
                         bslma::Allocator *allocator);
        // Create a cache entry object for managing the specified 'timetable'
        // that was loaded at the specified 'loadTime' using the specified
        // 'allocator'.  The behavior is undefined unless 'timetable' uses
        // 'allocator' to obtain memory.

    TimetableCache_Entry(const TimetableCache_Entry& original);
        // Create a cache entry object having the value of the specified
        // 'original' object.

    ~TimetableCache_Entry();
        // Destroy this cache entry object.

    // MANIPULATORS
    TimetableCache_Entry& operator=(const TimetableCache_Entry& rhs);
        // Assign to this cache entry object the value of the specified 'rhs'
        // object, and return a reference providing modifiable access to this
        // object.

    // ACCESSORS
    bsl::shared_ptr<const Timetable> get() const;
        // Return a shared pointer providing non-modifiable access to the
        // timetable referred to by this cache entry object.

    Datetime loadTime() const;
        // Return the time at which the timetable referred to by this cache
        // entry object was loaded.
};

                           // ====================
                           // class TimetableCache
                           // ====================

class TimetableCache {
    // This class implements an efficient cache of *read-only*
    // 'bdlt::Timetable' objects that are loaded into the cache, using a
    // timetable loader supplied at construction, as a side-effect of the
    // 'getTimetable' manipulator.  Timetables in the cache can be invalidated,
    // and removed from the cache via the 'invalidate' and 'invalidateAll'
    // methods.  In addition, timetables in the cache can be made to expire
    // based on a timeout that may be optionally supplied at construction.  The
    // 'bsl::shared_ptr<const bdlt::Timetable>' objects returned from the
    // 'getTimetable' and 'lookupTimetable' methods allow for the safe removal
    // of timetables from the cache that may still have outstanding references
    // to them.
    //
    // This container is *exception* *neutral* with no guarantee of rollback:
    // if an exception is thrown during the invocation of a method on a
    // pre-existing instance, the container is left in a valid state, but its
    // value is undefined.  In no event is memory leaked.
    //
    // This class is fully thread-safe (see 'bsldoc_glossary').

    // DATA
    mutable bsl::map<bsl::string, TimetableCache_Entry>
                           d_cache;           // cache of (name, handle) pairs

    TimetableLoader       *d_loader_p;        // timetable loader (held, not
                                              // owned)

    DatetimeInterval       d_timeOut;         // timeout value; ignored unless
                                              // 'd_hasTimeOutFlag' is 'true'

    bool                   d_hasTimeOutFlag;  // 'true' if this cache has a
                                              // timeout value and 'false'
                                              // otherwise

    mutable bslmt::Mutex   d_lock;            // guard access to cache

    bslma::Allocator      *d_allocator_p;     // memory allocator (held, not
                                              // owned)

  private:
    // PRIVATE TYPES
    typedef bsl::map<bsl::string, TimetableCache_Entry>::iterator
                                                                 CacheIterator;

    typedef bsl::map<bsl::string, TimetableCache_Entry>::const_iterator
                                                            ConstCacheIterator;

  private:
    // NOT IMPLEMENTED
    TimetableCache(const TimetableCache&);
    TimetableCache& operator=(const TimetableCache&);

  public:
    // CREATORS
    explicit
    TimetableCache(TimetableLoader  *loader,
                   bslma::Allocator *basicAllocator = 0);
        // Create an empty timetable cache that uses the specified 'loader' to
        // load timetables on demand and has no timeout.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  Timetables
        // loaded into this cache remain valid for retrieval until they have
        // been explicitly invalidated (via either the 'invalidate' or
        // 'invalidateAll' methods), or until this object is destroyed.  The
        // behavior is undefined unless 'loader' remains valid throughout the
        // lifetime of this cache.

    TimetableCache(TimetableLoader           *loader,
                   const bsls::TimeInterval&  timeout,
                   bslma::Allocator          *basicAllocator = 0);
        // Create an empty timetable cache that uses the specified 'loader' to
        // load timetables on demand and has the specified 'timeout' interval
        // indicating the length of time that timetables remain valid for
        // subsequent retrieval from the cache after they have been loaded.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless
        // 'bsls::TimeInterval() <= timeout <= bsls::TimeInterval(INT_MAX, 0)',
        // and 'loader' remains valid throughout the lifetime of this cache.
        // Note that a 'timeout' value of 0 indicates that a timetable will be
        // loaded into the cache by *each* (successful) call to the
        // 'getTimetable' method.

    ~TimetableCache();
        // Destroy this object.

    // MANIPULATORS
    bsl::shared_ptr<const Timetable> getTimetable(const char *timetableName);
        // Return a shared pointer providing non-modifiable access to the
        // timetable having the specified 'timetableName' in this timetable
        // cache, loading the timetable into the cache using the loader that
        // was supplied at construction if the timetable is not already present
        // in the cache or if the timetable has expired (i.e., per a timeout
        // optionally supplied at construction).  If the loader fails, whether
        // in loading a timetable for the first time or in reloading a
        // timetable that has expired, return an empty shared pointer.

    int invalidate(const char *timetableName);
        // Invalidate the timetable having the specified 'timetableName' in
        // this timetable cache, and remove it from the cache.  If a timetable
        // having 'timetableName' is not present in this cache, this method has
        // no effect.  Return the number of timetables that were invalidated.
        // Note that a timetable that has been invalidated in the cache remains
        // valid to all outstanding references to it, obtained via earlier
        // calls to the 'getTimetable' and 'lookupTimetable' methods, until all
        // of those references have been destroyed.

    int invalidateAll();
        // Invalidate all timetables in this timetable cache, and remove them
        // from the cache.  Return the number of timetables that were
        // invalidated.  Note that a timetable that has been invalidated in the
        // cache remains valid to all outstanding references to it, obtained
        // via earlier calls to the 'getTimetable' and 'lookupTimetable'
        // methods, until all of those references have been destroyed.

    // ACCESSORS
    bsl::shared_ptr<const Timetable>
    lookupTimetable(const char *timetableName) const;
        // Return a shared pointer providing non-modifiable access to the
        // timetable having the specified 'timetableName' in this timetable
        // cache.  If the timetable having 'timetableName' is not found in the
        // cache, or if the timetable has expired (i.e., per a timeout
        // optionally supplied at construction), return an empty shared
        // pointer.

    Datetime lookupLoadTime(const char *timetableName) const;
        // Return the datetime, in Coordinated Universal Time (UTC), at which
        // the timetable having the specified 'timetableName' was loaded into
        // this timetable cache.  If the timetable having 'timetableName' is
        // not found in the cache, or if the timetable has expired (i.e., per a
        // timeout optionally supplied at construction), return 'Datetime()'.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

}  // close package namespace
}  // close enterprise namespace

// TRAITS

namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<bdlt::TimetableCache> : bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace

#endif

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
