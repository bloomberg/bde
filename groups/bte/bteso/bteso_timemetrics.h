// bteso_timemetrics.h           -*-C++-*-
#ifndef INCLUDED_BTESO_TIMEMETRICS
#define INCLUDED_BTESO_TIMEMETRICS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a thread-safe time-based categorized metrics.
//
//@AUTHOR: Andrei Basov (abasov)
//
//@CLASSES:
//  bteso_TimeMetrics: accumulator for a categories' times.
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a clock that measures time and
// accumulates it into one of two categories ('BTESO_IO_BOUND' or
// 'BTESO_CPU_BOUND').
//
///Thread-safety
///-------------
// Thread-enabled -- different threads can access an instance of
// 'bteso_TimeMetrics' safely.
//
///Usage
///-----
// The usage is very simple.  First create a metrics object, keeping two
// categories, and initially measuring time in the 'CPU' category:
//..
//  bteso_TimeMetrics metrics(bteso_TimeMetrics::BTESO_MIN_NUM_CATEGORIES,
//                            bteso_TimeMetrics::BTESO_CPU_CATEGORY);
//..
// In order to measure time spent in I/O, e.g., doing 'select' calls:
//..
//  // perform initializations for 'select'
//  metrics.switchTo(IO_CATEGORY);
//  // do some IO, e.g., 'select'
//..
// To switch to measuring CPU time, e.g., doing event dispatch:
//..
//  metrics.switchTo(CPU_CATEGORY);
//  // dispatch events
//..
// At the end of the computation, or periodically, one may report the time
// spent in each category as follows:
//..
//  bsl::cout << "The total time spent in IO was "
//       << metrics.percentage(bteso_TimeMetrics::BTESO_IO_CATEGORY)
//       << bsl::endl;
//
//  bsl::cout << "The total time spent in CPU was "
//       << metrics.percentage(bteso_TimeMetrics::BTESO_CPU_CATEGORY)
//       << bsl::endl;
//..
// This metrics may be reset to its initial state by:
//..
//  metrics.resetAllTimers();
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BCES_ATOMICUTIL
#include <bces_atomicutil.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                           // =======================
                           // class bteso_TimeMetrics
                           // =======================

class bteso_TimeMetrics {
    // This class provides a set of metrics data and a mechanism to accumulate
    // time into one of two categories ('BTESO_IO_BOUND', or
    // 'BTESO_CPU_BOUND').  Use the method 'switchTo' to switch back and forth
    // between these categories.

    // DATA
    bsl::vector<bdet_TimeInterval> d_categoryStartTimes;
    bsl::vector<int>               d_categoryTimes;

    int                            d_currentCategory;
    int                            d_currentTotal;

    mutable bcemt_Mutex            d_dataLock;

  private:
    // NOT IMPLEMENTED
    bteso_TimeMetrics(const bteso_TimeMetrics& original);
    bteso_TimeMetrics& operator=(const bteso_TimeMetrics& rhs);

  public:
    // PUBLIC TYPES
    enum {
        BTESO_IO_BOUND  = 0,  // the processing unit (i.e., thread or process)
                              // is blocked in an IO operation

        BTESO_CPU_BOUND = 1,  // the processing unit (i.e., thread or process)
                              // is executing CPU-bound block of code

        BTESO_MIN_NUM_CATEGORIES = BTESO_CPU_BOUND + 1
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , IO_BOUND           = BTESO_IO_BOUND
      , CPU_BOUND          = BTESO_CPU_BOUND
      , MIN_NUM_CATEGORIES = BTESO_MIN_NUM_CATEGORIES
#endif
    };

    // CREATORS
    bteso_TimeMetrics(int              numCategories,
                      int              initialCategory,
                      bslma_Allocator *basicAllocator = 0);
        // Create a metrics that distinguishes between the specified
        // 'numCategories' different categories having the specified
        // 'initialCategory'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // MIN_NUM_CATEGORIES <= numCategories and
        // 0 <= initialCategory < numCategories.

    ~bteso_TimeMetrics();

    // MANIPULATORS
    int percentage(int category);
        // Return the percent value in the range [0..100] (inclusive) that
        // reflects the percentage of time spent for the specified 'category'
        // since the last time this metrics was reset (or creation, if reset
        // never happened).

    void switchTo(int category);
        // Switch from the current category to the specified 'category'.
        // The behavior is undefined unless 0 <= category < numCategories().

    void resetAll();
        // Reset the time values for each category to 0.

    void resetStartTimes();
        // Reset the start times for each category to
        // 'bdetu_SystemTime::now()'.

    // ACCESSORS
    int currentCategory() const;
        // Return the current time category.

    int numCategories() const;
        // Return the total number of categories.
};

//-----------------------------------------------------------------------------
//                      INLINE FUNCTIONS' DEFINITIONS
//-----------------------------------------------------------------------------

inline
int bteso_TimeMetrics::numCategories() const
{
    return d_categoryTimes.size();
}

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
