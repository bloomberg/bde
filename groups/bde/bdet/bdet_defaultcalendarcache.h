// bdet_defaultcalendarcache.h                                        -*-C++-*-
#ifndef INCLUDED_BDET_DEFAULTCALENDARCACHE
#define INCLUDED_BDET_DEFAULTCALENDARCACHE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a process-wide default 'bdet_CalendarCache' object.
//
//@CLASSES:
//  bdet_DefaultCalendarCache: namespace for managing a default calendar cache
//
//@AUTHOR:
//
//@SEE_ALSO: bdet_calendarcache, bdet_calendarloader
//
//@DESCRIPTION: This component provides a namespace,
// 'bdet_DefaultCalendarCache', for utility functions that initialize, provide
// access to, and ultimately destroy, a default 'bdet_CalendarCache' object.
// The default cache is initialized by calling the 'initialize' class method to
// which a concrete calendar loader and memory allocator must be supplied.  The
// cache is destroyed by the 'destroy' class method.  Note that the
// calendar-naming convention in effect for the default cache is determined by
// the loader supplied to 'initialize'.
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
///Thread-Safety
///-------------
// The 'bdet_DefaultCalendarCache' class is fully thread-safe (see
// 'bsldoc_glossary').
//
///Usage
///-----
// The following example illustrates how to use 'bdet_DefaultCalendarCache'.
//
///Example 1: Using 'bdet_DefaultCalendarCache'
///- - - - - - - - - - - - - - - - - - - - - -
// 'bdet_DefaultCalendarCache' has a particularly simple interface.  This
// example shows how to use each of its three methods.
//
// A hypothetical calendar loader is assumed, 'my_CalendarLoader', the details
// of which are not important other than that it supports calendars identified
// by "DE", "FR", and "US", which nominally identify the major holidays in
// Germany, France, and the United States, respectively.  Furthermore, we cite
// two specific dates of interest: 2011/07/04, which was a holiday in the US
// (Independence Day), but not in France, and 2011/07/14, which was a holiday
// in France (Bastille Day), but not in the US.
//
// First, we create a calendar loader, an instance of 'my_CalendarLoader', and
// use it, in turn, to initialize the default calendar cache.  A memory
// allocator must also be explicitly supplied to the 'initialize' method.  The
// global allocator is suitable in this case (see 'bslma_default'):
//..
//  static my_CalendarLoader loader;
//
//  bdet_DefaultCalendarCache::initialize(&loader,
//                                        bslma::Default::globalAllocator());
//..
// Note that declaring 'loader' to be 'static' ensures that it remains valid
// until the cache is destroyed.  Also note that initialization of the cache
// would typically be done in 'main' before other threads have been created.
//
// Next, we obtain the address of the default calendar cache using the
// 'instance' class method:
//..
//  bdet_CalendarCache *cachePtr = bdet_DefaultCalendarCache::instance();
//  assert(cachePtr);
//..
// Next, we retrieve the calendar identified by "US" from the default cache,
// and verify that 2011/07/04 is recognized as a holiday in the "US" calendar,
// whereas 2011/07/14 is not:
//..
//  bdet_CalendarCacheEntryPtr us = cachePtr->getCalendar("US");
//  assert( us->isHoliday(bdet_Date(2011, 7,  4)));
//  assert(!us->isHoliday(bdet_Date(2011, 7, 14)));
//..
// Next, we fetch the calendar identified by "FR", this time verifying that
// 2011/07/14 is recognized as a holiday in the "FR" calendar, but 2011/07/04
// is not:
//..
//  bdet_CalendarCacheEntryPtr fr = cachePtr->getCalendar("FR");
//  assert(!fr->isHoliday(bdet_Date(2011, 7,  4)));
//  assert( fr->isHoliday(bdet_Date(2011, 7, 14)));
//..
// Finally, we destroy the default calendar cache:
//..
//  bdet_DefaultCalendarCache::destroy();
//  assert(!bdet_DefaultCalendarCache::instance());
//..
// Note that destruction of the default cache would typically be done in 'main'
// just prior to program termination.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif


#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif


namespace BloombergLP {

namespace bslma { class Allocator; }

class bdet_CalendarCache;
class bdet_CalendarLoader;

                      // ===============================
                      // class bdet_DefaultCalendarCache
                      // ===============================

struct bdet_DefaultCalendarCache {
    // This 'struct' provides a namespace for functions that manage the
    // lifetime of, and access to, a process-wide default 'bdet_CalendarCache'
    // object.  The default cache is initialized by an explicit call to the
    // 'initialize' class method, and destroyed by the 'destroy' class method.
    // The default cache may be initialized and destroyed multiple times during
    // the lifetime of a process.  The lifetimes of the calendar loader and
    // memory allocator supplied to 'initialize' must extend beyond the
    // following (matching) call to 'destroy'.
    //
    // All methods of this 'struct' are fully thread-safe.

    // CLASS METHODS
    static int initialize(bdet_CalendarLoader *loader,
                          bslma::Allocator    *allocator);
        // Initialize the default 'bdet_CalendarCache' object managed by this
        // class that uses the specified 'loader' to obtain calendars, and uses
        // the specified 'allocator' to supply memory.  If the default cache is
        // already in the initialized state, this method has no effect.  Return
        // 0 on success, and a non-zero value otherwise.  The behavior is
        // undefined unless 'loader' and 'allocator' remain valid until a
        // subsequent call to 'destroy'.

    static int initialize(bdet_CalendarLoader      *loader,
                          const bdet_TimeInterval&  timeout,
                          bslma::Allocator         *allocator);
        // Initialize the default 'bdet_CalendarCache' object managed by this
        // class that uses the specified 'loader' to obtain calendars, have the
        // specified 'timeout', and uses the specified 'allocator' to supply
        // memory.  If the default cache is already in the initialized state,
        // this method has no effect.  Return 0 on success, and a non-zero
        // value otherwise.  The behavior is undefined unless 'loader' and
        // 'allocator' remain valid until a subsequent call to 'destroy'.

    static void destroy();
        // Destroy the default 'bdet_CalendarCache' object managed by this
        // class.  If the default cache is not in the initialized state, this
        // method has no effect.  Note that all addresses returned by earlier
        // calls to 'instance' are invalidated by this method.

    static bdet_CalendarCache *instance();
        // Return an address providing modifiable access to the default
        // 'bdet_CalendarCache' object managed by this class, if the default
        // cache is in the initialized state, and 0 otherwise.  The cache
        // obtains calendars using the loader that was supplied to the
        // 'initialize' method.  Note that the returned address is invalidated
        // by a subsequent call to 'destroy'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
