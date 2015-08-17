// btlso_timemetrics.h                                                -*-C++-*-
#ifndef INCLUDED_BTLSO_TIMEMETRICS
#define INCLUDED_BTLSO_TIMEMETRICS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide thread-safe time-based categorized metrics.
//
//@AUTHOR: Andrei Basov (abasov)
//
//@CLASSES:
//  btlso::TimeMetrics: accumulator for time-based categorized metrics
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a clock that measures time and
// accumulates it into one of two categories: 'e_IO_BOUND' or
// 'e_CPU_BOUND'.
//
///Thread-safety
///-------------
// Thread-enabled -- different threads can access an instance of
// 'btlso::TimeMetrics' safely.
//
///Usage
///-----
// The usage is very simple.  First create a metrics object, keeping two
// categories, and initially measuring time in the 'CPU' category:
//..
//  btlso::TimeMetrics metrics(btlso::TimeMetrics::e_MIN_NUM_CATEGORIES,
//                            btlso::TimeMetrics::BTESO_CPU_CATEGORY);
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
//       << metrics.percentage(btlso::TimeMetrics::BTESO_IO_CATEGORY)
//       << bsl::endl;
//
//  bsl::cout << "The total time spent in CPU was "
//       << metrics.percentage(btlso::TimeMetrics::BTESO_CPU_CATEGORY)
//       << bsl::endl;
//..
// This metrics may be reset to its initial state by:
//..
//  metrics.resetAll();
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BDLQQ_MUTEX
#include <bdlqq_mutex.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

#ifndef INCLUDED_BDLT_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace btlso {
                           // =======================
                           // class TimeMetrics
                           // =======================

class TimeMetrics {
    // This class provides a set of metrics data and a mechanism to accumulate
    // time into one of two categories: 'e_IO_BOUND' or 'e_CPU_BOUND'.
    // Use the method 'switchTo' to switch back and forth between these
    // categories.

    // DATA
    bsl::vector<bsls::TimeInterval> d_categoryStartTimes;
    bsl::vector<int>               d_categoryTimes;

    int                            d_currentCategory;
    int                            d_currentTotal;

    mutable bdlqq::Mutex            d_dataLock;

  private:
    // NOT IMPLEMENTED
    TimeMetrics(const TimeMetrics&);
    TimeMetrics& operator=(const TimeMetrics&);

  public:
    // PUBLIC TYPES
    enum {
        e_IO_BOUND           = 0  // the processing unit (i.e., thread or
                                  // process) is blocked in an IO operation

      , e_CPU_BOUND          = 1  // the processing unit (i.e., thread or
                                  // process) is executing CPU-bound block of
                                  // code

      , e_MIN_NUM_CATEGORIES = e_CPU_BOUND + 1
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BTESO_IO_BOUND           = e_IO_BOUND
      , BTESO_CPU_BOUND          = e_CPU_BOUND
      , BTESO_MIN_NUM_CATEGORIES = e_MIN_NUM_CATEGORIES
      , IO_BOUND           = e_IO_BOUND
      , CPU_BOUND          = e_CPU_BOUND
      , MIN_NUM_CATEGORIES = e_MIN_NUM_CATEGORIES
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    // CREATORS
    TimeMetrics(int               numCategories,
                      int               initialCategory,
                      bslma::Allocator *basicAllocator = 0);
        // Create a metrics that distinguishes between the specified
        // 'numCategories' different categories having the specified
        // 'initialCategory'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // 'e_MIN_NUM_CATEGORIES <= numCategories' and
        // '0 <= initialCategory < numCategories'.

    ~TimeMetrics();
        // Destroy this object.

    // MANIPULATORS
    int percentage(int category);
        // Return the percent value in the range [0..100] (inclusive) that
        // reflects the percentage of time spent for the specified 'category'
        // since the last time this metrics was reset (or creation, if reset
        // never happened).

    void switchTo(int category);
        // Switch from the current category to the specified 'category'.
        // The behavior is undefined unless '0 <= category < numCategories()'.

    void resetAll();
        // Reset the time values for each category to 0.

    void resetStartTimes();
        // Reset the start times for each category to
        // 'bdlt::CurrentTime::now()'.

    // ACCESSORS
    int currentCategory() const;
        // Return the current time category.

    int numCategories() const;
        // Return the total number of categories.
};

//-----------------------------------------------------------------------------
//                      INLINE FUNCTIONS' DEFINITIONS
//-----------------------------------------------------------------------------

                           // -----------------------
                           // class TimeMetrics
                           // -----------------------

inline
int TimeMetrics::numCategories() const
{
    return static_cast<int>(d_categoryTimes.size());
}
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
