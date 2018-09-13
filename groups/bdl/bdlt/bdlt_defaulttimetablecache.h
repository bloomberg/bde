// bdlt_defaulttimetablecache.h                                       -*-C++-*-
#ifndef INCLUDED_BDLT_DEFAULTTIMETABLECACHE
#define INCLUDED_BDLT_DEFAULTTIMETABLECACHE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a process-wide default 'bdlt::TimetableCache' object.
//
//@CLASSES:
//  bdlt::DefaultTimetableCache: namespace managing a default timetable cache
//
//@AUTHOR:
//
//@SEE_ALSO: bdlt_timetablecache, bdlt_timetableloader
//
//@DESCRIPTION: This component provides a namespace,
// 'bdlt::DefaultTimetableCache', for utility functions that initialize,
// provide access to, and ultimately destroy, a default 'bdlt::TimetableCache'
// object.  The default cache is initialized by calling the (overloaded)
// 'initialize' class method to which a concrete timetable loader and memory
// allocator must be supplied.  The cache is destroyed by the 'destroy' class
// method.  Note that the timetable-naming convention in effect for the default
// cache is determined by the loader supplied to 'initialize'.
//
// A timeout may be established for the default cache by supplying an optional
// 'bsls::TimeInterval' value to 'initialize'.  When a timeout is in effect for
// the default cache, a request for a timetable from the cache may incur the
// reloading of the timetable if the one in the cache has expired (i.e., the
// time interval defined by the timeout value has elapsed since the timetable
// was last loaded into the cache).  Timetables will not expire in this fashion
// if the default cache is not provided with a timeout at initialization.
//
// Although the cache may be initialized and destroyed multiple times during
// the lifetime of a process, the expected usage is that the cache would be
// initialized *once*, typically in 'main' before other threads have been
// created, and destroyed just prior to program termination.  Regardless, the
// lifetimes of the timetable loader and memory allocator supplied to
// 'initialize' must extend beyond the following (matching) call to 'destroy'.
// While the default timetable cache is in the initialized state, the
// 'instance' method returns an address providing modifiable access to the
// cache.  Otherwise, 'instance' returns 0.
//
// !WARNING!: Clients should be aware that the address returned by 'instance'
// becomes invalid by a subsequent call to 'destroy'.
//
///Thread Safety
///-------------
// The 'bdlt::DefaultTimetableCache' class is fully thread-safe (see
// 'bsldoc_glossary') provided that the allocator supplied to 'initialize' and
// the default allocator in effect during the lifetime of the default cache are
// both fully thread-safe.
//
///Usage
///-----
// The following example illustrates how to use 'bdlt::DefaultTimetableCache'.
//
///Example 1: Using 'bdlt::DefaultTimetableCache'
///- - - - - - - - - - - - - - - - - - - - - - -
// 'bdlt::DefaultTimetableCache' has a particularly simple interface.  This
// example shows how to use each of its three methods.
//
// In this example, we assume a hypothetical timetable loader,
// 'MyTimetableLoader', the details of which are not important other than that
// it supports timetables identified by "ZERO", "ONE", and "TWO".  Furthermore,
// the value of the initial transition code for each of these timetables is
// given by the timetable's name (e.g., if 'Z' has the value of the timetable
// identified as "ZERO", then '0 == Z.initialTransitionCode()').
//
// First, we create a timetable loader, an instance of 'MyTimetableLoader', and
// use it, in turn, to initialize the default timetable cache.  A memory
// allocator must also be explicitly supplied to the 'initialize' method.  The
// global allocator is suitable in this case (see 'bslma_default'):
//..
//  static MyTimetableLoader loader;
//
//  int rc = bdlt::DefaultTimetableCache::initialize(
//                                          &loader,
//                                          bslma::Default::globalAllocator());
//  assert(!rc);
//..
// Note that declaring 'loader' to be 'static' ensures that it remains valid
// until the cache is destroyed.  Also note that initialization of the cache
// would typically be done in 'main' before other threads have been created.
//
// Next, we obtain the address of the default timetable cache using the
// 'instance' class method:
//..
//  bdlt::TimetableCache *cachePtr = bdlt::DefaultTimetableCache::instance();
//  assert(cachePtr);
//..
// Then, we retrieve the timetable identified by "TWO" from the default cache
// and verify that 2 is the value of the initial transition code:
//..
//  bsl::shared_ptr<const bdlt::Timetable> two = cachePtr->getTimetable("TWO");
//  assert(2 == two->initialTransitionCode());
//..
// Next, we fetch the timetable identified by "ONE", this time verifying that 1
// is the value of the initial transition code for the "ONE" timetable:
//..
//  bsl::shared_ptr<const bdlt::Timetable> one = cachePtr->getTimetable("ONE");
//  assert(1 == one->initialTransitionCode());
//..
// Finally, we destroy the default timetable cache:
//..
//  bdlt::DefaultTimetableCache::destroy();
//  assert(!bdlt::DefaultTimetableCache::instance());
//..
// Note that destruction of the default cache would typically be done in 'main'
// just prior to program termination.

#include <bdlscm_version.h>

#include <bsls_timeinterval.h>

namespace BloombergLP {
namespace bslma { class Allocator; }
namespace bdlt {

class TimetableCache;
class TimetableLoader;

                       // ===========================
                       // class DefaultTimetableCache
                       // ===========================

struct DefaultTimetableCache {
    // This 'struct' provides a namespace for functions that manage the
    // lifetime of, and access to, a process-wide default
    // 'bdlt::TimetableCache' object.  The default cache is initialized by an
    // explicit call to the 'initialize' class method, and destroyed by the
    // 'destroy' class method.  The default cache may be initialized and
    // destroyed multiple times during the lifetime of a process.  The
    // lifetimes of the timetable loader and memory allocator supplied to
    // 'initialize' must extend beyond the following (matching) call to
    // 'destroy'.
    //
    // All methods of this 'struct' are fully thread-safe (see
    // 'bsldoc_glossary').

    // CLASS METHODS
    static void destroy();
        // Destroy the default 'TimetableCache' object managed by this class.
        // If the default cache is not in the initialized state, this method
        // has no effect.  Note that all addresses returned by earlier calls to
        // 'instance' are invalidated by this method.

    static int initialize(TimetableLoader  *loader,
                          bslma::Allocator *allocator);
        // Initialize the default 'TimetableCache' object managed by this class
        // to use the specified 'loader' to obtain timetables, to have no
        // timeout, and to use the specified 'allocator' to supply memory.  If
        // the default cache is already in the initialized state, this method
        // has no effect.  Return 0 on success, and a non-zero value otherwise.
        // The behavior is undefined unless 'loader' and 'allocator' remain
        // valid until a subsequent call to 'destroy'.

    static int initialize(TimetableLoader           *loader,
                          const bsls::TimeInterval&  timeout,
                          bslma::Allocator          *allocator);
        // Initialize the default 'TimetableCache' object managed by this class
        // to use the specified 'loader' to obtain timetables, to have the
        // specified 'timeout', and to use the specified 'allocator' to supply
        // memory.  If the default cache is already in the initialized state,
        // this method has no effect.  Return 0 on success, and a non-zero
        // value otherwise.  The behavior is undefined unless 'loader' and
        // 'allocator' remain valid until a subsequent call to 'destroy', and
        // 'bsls::TimeInterval() <= timeout <= bsls::TimeInterval(INT_MAX, 0)'.
        // Note that a 'timeout' value of 0 indicates that a timetable will be
        // loaded into the default cache by *each* (successful) call to
        // 'TimetableCache::getTimetable' on the cache returned by 'instance'.

    static TimetableCache *instance();
        // Return an address providing modifiable access to the default
        // 'TimetableCache' object managed by this class, if the default cache
        // is in the initialized state, and 0 otherwise.  The cache obtains
        // timetables using the loader that was supplied to the 'initialize'
        // method.  Note that the returned address is invalidated by a
        // subsequent call to 'destroy'.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

}  // close package namespace
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
