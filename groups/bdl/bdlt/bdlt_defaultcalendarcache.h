// bdlt_defaultcalendarcache.h                                        -*-C++-*-
#ifndef INCLUDED_BDLT_DEFAULTCALENDARCACHE
#define INCLUDED_BDLT_DEFAULTCALENDARCACHE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a process-wide default 'bdlt::CalendarCache' object.
//
//@CLASSES:
//  bdlt::DefaultCalendarCache: namespace for managing a default calendar cache
//
//@SEE_ALSO: bdlt_calendarcache, bdlt_calendarloader
//
//@DESCRIPTION: This component provides a namespace,
// 'bdlt::DefaultCalendarCache', for utility functions that initialize, provide
// access to, and ultimately destroy, a default 'bdlt::CalendarCache' object.
// The default cache is initialized by calling the (overloaded) 'initialize'
// class method to which a concrete calendar loader and memory allocator must
// be supplied.  The cache is destroyed by the 'destroy' class method.  Note
// that the calendar-naming convention in effect for the default cache is
// determined by the loader supplied to 'initialize'.
//
// A timeout may be established for the default cache by supplying an optional
// 'bsls::TimeInterval' value to 'initialize'.  When a timeout is in effect for
// the default cache, a request for a calendar from the cache may incur the
// reloading of the calendar if the one in the cache has expired (i.e., the
// time interval defined by the timeout value has elapsed since the calendar
// was last loaded into the cache).  Calendars will not expire in this fashion
// if the default cache is not provided with a timeout at initialization.
//
// Although the cache may be initialized and destroyed multiple times during
// the lifetime of a process, the expected usage is that the cache would be
// initialized *once*, typically in 'main' before other threads have been
// created, and destroyed just prior to program termination.  Regardless, the
// lifetimes of the calendar loader and memory allocator supplied to
// 'initialize' must extend beyond the following (matching) call to 'destroy'.
// While the default calendar cache is in the initialized state, the 'instance'
// method returns an address providing modifiable access to the cache.
// Otherwise, 'instance' returns 0.
//
// !WARNING!: Clients should be aware that the address returned by 'instance'
// becomes invalid by a subsequent call to 'destroy'.
//
///Thread Safety
///-------------
// The 'bdlt::DefaultCalendarCache' class is fully thread-safe (see
// 'bsldoc_glossary') provided that the allocator supplied to 'initialize' and
// the default allocator in effect during the lifetime of the default cache are
// both fully thread-safe.
//
///Usage
///-----
// The following example illustrates how to use 'bdlt::DefaultCalendarCache'.
//
///Example 1: Using 'bdlt::DefaultCalendarCache'
///- - - - - - - - - - - - - - - - - - - - - - -
// 'bdlt::DefaultCalendarCache' has a particularly simple interface.  This
// example shows how to use each of its three methods.
//
// A hypothetical calendar loader is assumed, 'MyCalendarLoader', the details
// of which are not important other than that it supports calendars identified
// by "DE", "FR", and "US", which nominally identify the major holidays in
// Germany, France, and the United States, respectively.  Furthermore, we cite
// two specific dates of interest: 2011/07/04, which was a holiday in the US
// (Independence Day), but not in France, and 2011/07/14, which was a holiday
// in France (Bastille Day), but not in the US.
//
// First, we create a calendar loader, an instance of 'MyCalendarLoader', and
// use it, in turn, to initialize the default calendar cache.  A memory
// allocator must also be explicitly supplied to the 'initialize' method.  The
// global allocator is suitable in this case (see 'bslma_default'):
//..
//  static MyCalendarLoader loader;
//
//  int rc = bdlt::DefaultCalendarCache::initialize(
//                                          &loader,
//                                          bslma::Default::globalAllocator());
//  assert(!rc);
//..
// Note that declaring 'loader' to be 'static' ensures that it remains valid
// until the cache is destroyed.  Also note that initialization of the cache
// would typically be done in 'main' before other threads have been created.
//
// Next, we obtain the address of the default calendar cache using the
// 'instance' class method:
//..
//  bdlt::CalendarCache *cachePtr = bdlt::DefaultCalendarCache::instance();
//  assert(cachePtr);
//..
// Then, we retrieve the calendar identified by "US" from the default cache,
// and verify that 2011/07/04 is recognized as a holiday in the "US" calendar,
// whereas 2011/07/14 is not:
//..
//  bsl::shared_ptr<const bdlt::Calendar> us = cachePtr->getCalendar("US");
//  assert( us->isHoliday(bdlt::Date(2011, 7,  4)));
//  assert(!us->isHoliday(bdlt::Date(2011, 7, 14)));
//..
// Next, we fetch the calendar identified by "FR", this time verifying that
// 2011/07/14 is recognized as a holiday in the "FR" calendar, but 2011/07/04
// is not:
//..
//  bsl::shared_ptr<const bdlt::Calendar> fr = cachePtr->getCalendar("FR");
//  assert(!fr->isHoliday(bdlt::Date(2011, 7,  4)));
//  assert( fr->isHoliday(bdlt::Date(2011, 7, 14)));
//..
// Finally, we destroy the default calendar cache:
//..
//  bdlt::DefaultCalendarCache::destroy();
//  assert(!bdlt::DefaultCalendarCache::instance());
//..
// Note that destruction of the default cache would typically be done in 'main'
// just prior to program termination.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace bdlt {

class CalendarCache;
class CalendarLoader;

                      // ==========================
                      // class DefaultCalendarCache
                      // ==========================

struct DefaultCalendarCache {
    // This 'struct' provides a namespace for functions that manage the
    // lifetime of, and access to, a process-wide default 'bdlt::CalendarCache'
    // object.  The default cache is initialized by an explicit call to the
    // 'initialize' class method, and destroyed by the 'destroy' class method.
    // The default cache may be initialized and destroyed multiple times during
    // the lifetime of a process.  The lifetimes of the calendar loader and
    // memory allocator supplied to 'initialize' must extend beyond the
    // following (matching) call to 'destroy'.
    //
    // All methods of this 'struct' are fully thread-safe (see
    // 'bsldoc_glossary').

    // CLASS METHODS
    static void destroy();
        // Destroy the default 'CalendarCache' object managed by this class.
        // If the default cache is not in the initialized state, this method
        // has no effect.  Note that all addresses returned by earlier calls to
        // 'instance' are invalidated by this method.

    static int initialize(CalendarLoader   *loader,
                          bslma::Allocator *allocator);
        // Initialize the default 'CalendarCache' object managed by this class
        // to use the specified 'loader' to obtain calendars, to have no
        // timeout, and to use the specified 'allocator' to supply memory.  If
        // the default cache is already in the initialized state, this method
        // has no effect.  Return 0 on success, and a non-zero value otherwise.
        // The behavior is undefined unless 'loader' and 'allocator' remain
        // valid until a subsequent call to 'destroy'.

    static int initialize(CalendarLoader            *loader,
                          const bsls::TimeInterval&  timeout,
                          bslma::Allocator          *allocator);
        // Initialize the default 'CalendarCache' object managed by this class
        // to use the specified 'loader' to obtain calendars, to have the
        // specified 'timeout', and to use the specified 'allocator' to supply
        // memory.  If the default cache is already in the initialized state,
        // this method has no effect.  Return 0 on success, and a non-zero
        // value otherwise.  The behavior is undefined unless 'loader' and
        // 'allocator' remain valid until a subsequent call to 'destroy', and
        // 'bsls::TimeInterval(0) <= timeout <= bsls::TimeInterval(INT_MAX)'.
        // Note that a 'timeout' value of 0 indicates that a calendar will be
        // loaded into the default cache by *each* (successful) call to
        // 'CalendarCache::getCalendar' on the cache returned by 'instance'.

    static CalendarCache *instance();
        // Return an address providing modifiable access to the default
        // 'CalendarCache' object managed by this class, if the default cache
        // is in the initialized state, and 0 otherwise.  The cache obtains
        // calendars using the loader that was supplied to the 'initialize'
        // method.  Note that the returned address is invalidated by a
        // subsequent call to 'destroy'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

}  // close package namespace
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
