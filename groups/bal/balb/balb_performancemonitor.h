// balb_performancemonitor.h                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALB_PERFORMANCEMONITOR
#define INCLUDED_BALB_PERFORMANCEMONITOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism to collect process performance measures.
//
//@CLASSES:
//   balb::PerformanceMonitor: monitor process performance
//   balb::PerformanceMonitor::Statistics: performance stats
//   balb::PerformanceMonitor::ConstIterator: stats iteration
//
//@DESCRIPTION: This component provides an application developer the means to
// collect and report performance statistics for an arbitrary number of
// processes running on the local machine.  The 'balb::PerformanceMonitor'
// provides the mechanism for doing so, the
// 'balb::PerformanceMonitor::Statistics' class holds the data and the
// 'balb::PerformanceMonitor::ConstIterator' class is used to iterate over the
// data.  The following table describes the measures collecting by this
// component.  Note that all the collected measures are specific to the
// monitored process and do not refer to any system-wide measurement.
//..
// Measure           Identifier           Description
// -------           ----------           -----------
// User CPU Time     e_CPU_TIME_USER   Total amount of time spent executing
//                                        instructions in user mode.
//
// System CPU Time   e_CPU_TIME_SYSTEM Total amount of time spent executing
//                                        instructions in kernel mode.
//
// CPU Time          e_CPU_TIME        The sum of User and System CPU times.
//
// User CPU %        e_CPU_UTIL_USER   Percentage of elapsed CPU time this
//                                        process spent executing instructions
//                                        in user mode.
//
// System CPU %      e_CPU_UTIL_SYSTEM Percentage of elapsed CPU time this
//                                        process spent executing instructions
//                                        in kernel mode.
//
// CPU %             e_CPU_UTIL        Sum of User CPU % and System CPU %.
//
// Resident Size     e_RESIDENT_SIZE   Number of mega-bytes of physical
//                                        memory used by the process.
//
// Virtual Size      e_VIRTUAL_SIZE    The size of the heap, in
//                                        mega-bytes. This value does not
//                                        include the size of the address
//                                        space mapped to files (anonymous or
//                                        otherwise.)
//
// Thread Count      e_NUM_THREADS     Number of threads executing in the
//                                        process.
//
// Page Faults       e_NUM_PAGEFAULTS  Total number of page faults incurred
//                                        throughout the lifetime of the
//                                        process.
//..
//
///OS-Specific Permissions
///-----------------------
// Various OSs might require specific permissions in order to inspect processes
// other than the current process.  For example, on Darwin, users other than
// root can only inspect processes running under the user with which the
// current process is running.  This error condition will be indicated by a
// non-zero return value from 'registerPid'.
//
///Iterator Invalidation
///---------------------
// Registration of new pids does not invalidate existing iterators.
// Unregistration of pid only invalidates iterators pointing to the statistics
// for the pid being unregistered, all other iterators remain valid.
//
///Thread Safety
///-------------
// This class is completely thread safe.
//
///Usage
///-----
// The following example shows how to monitor the currently executing process
// and produce a formatted report of the collected measures after a certain
// interval.
//..
//  // Instantiate a scheduler used by the performance monitor to schedule
//  // collection events.
//  bdlmt::TimerEventScheduler scheduler;
//  scheduler.start();
//
//  // Create the performance monitor, monitoring the current process and
//  // auto-collecting statistics every second.
//  balb::PerformanceMonitor perfmon(&scheduler, 1.0);
//
//  // Assume the existence of three pids, 1000, 1001, and 1002, running on
//  // the local machine.
//  perfmon.registerPid(1000, "task1");
//  perfmon.registerPid(1001, "task2");
//  perfmon.registerPid(1003, "task3");
//
//  // Print a formatted report of the performance statistics collected for
//  // each pid every 10 seconds for one minute.
//  for (int i = 0; i < 6; ++i) {
//      bslmt::ThreadUtil::microSleep(0, 10);
//
//      for (balb::PerformanceMonitor::ConstIterator it  = perfmon.begin();
//                                                  it != perfmon.end();
//                                                ++it) {
//          const balb::PerformanceMonitor::Statistics& stats = *it;
//          bsl::cout << "Pid = " << stats.pid() << ":\n";
//          stats.print(bsl::cout);
//      }
//  }
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSLMT_READLOCKGUARD
#include <bslmt_readlockguard.h>
#endif

#ifndef INCLUDED_BSLMT_RWMUTEX
#include <bslmt_rwmutex.h>
#endif

#ifndef INCLUDED_BDLMT_TIMEREVENTSCHEDULER
#include <bdlmt_timereventscheduler.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BDLT_DATETIME
#include <bdlt_datetime.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

namespace balb {
                          // ========================
                          // class PerformanceMonitor
                          // ========================

class PerformanceMonitor {
    // Provides a mechanism to collect performance statistics for an arbitrary
    // number of processes running on the local machine.

  public:
    // FRIENDS
    class Statistics;
    friend class Statistics;
        // Grant visibility of private types to 'Statistics'.

    class ConstIterator;
    friend class ConstIterator;
        // Grant visibility of private types to 'ConstIterator'.

  private:
    // PRIVATE TYPES

    // Defines a type alias for the operating system type discovered by the
    // 'bsls::platform' component.  This type alias is used to specifically
    // select a particular template specialization of the 'Collector' template.

#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_CYGWIN)
    typedef bsls::Platform::OsLinux   OsType;
#elif defined(BSLS_PLATFORM_OS_FREEBSD)
    typedef bsls::Platform::OsFreeBsd OsType;
#elif defined(BSLS_PLATFORM_OS_DARWIN)
    typedef bsls::Platform::OsDarwin  OsType;
/*
#elif defined(BSLS_PLATFORM_OS_HPUX)
typedef bsls::Platform::OsHpUx OsType;
*/
#elif defined(BSLS_PLATFORM_OS_UNIX)
    typedef bsls::Platform::OsUnix    OsType;
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    typedef bsls::Platform::OsWindows OsType;
#endif

    template <class PLATFORM>
    class Collector;
        // Forward declares a class template for a performance measure
        // collector for a parameterized 'PLATFORM'.  This class template is
        // never defined.  Instead, we define explicit specializations for
        // supported platforms.  Any attempt to compile this component on
        // unsupported platforms will result in a compile-time error.

    typedef Collector<OsType> CollectorType;
        // Defines a type alias for the type that defines the private platform-
        // specific mechanism used to collect the performance measures for a
        // pid.

    typedef bsl::shared_ptr<CollectorType> CollectorPtr;
        // Defines a type alias for the shared pointer to the platform-specific
        // mechanism used to collect the performance measures for a pid.

    typedef bsl::shared_ptr<Statistics> StatisticsPtr;
        // Defines a type alias for the shared pointer to the platform-specific
        // mechanism used to collect the performance measures for a pid.

    typedef bsl::map<int, bsl::pair<StatisticsPtr, CollectorPtr> > PidMap;
        // Defines a type alias for the map of pids to their collected
        // statistics and associated platform-specific collector
        // implementation.

    // DATA
    PidMap                              d_pidMap;       // map of pid stats

    double                              d_interval;     // collection interval

    bdlmt::TimerEventScheduler         *d_scheduler_p;  // scheduler of
                                                        // collection events
                                                        // (held)

    bdlmt::TimerEventScheduler::Handle  d_clock;        // handle to collection
                                                        // timer

    mutable bslmt::RWMutex              d_mapGuard;     // serializes write
                                                        // access to 'd_pidMap'

    bslma::Allocator                   *d_allocator_p;  // supplies memory
                                                        // (held)

  private:
    // NOT IMPLEMENTED
    PerformanceMonitor(const PerformanceMonitor&);
    PerformanceMonitor& operator=(const PerformanceMonitor&);

  public:
    // TYPES
    enum Measure {
        // Enumerates the set of performance measures this class is capable of
        // monitoring.

        e_CPU_TIME,          // CPU time (seconds)
        e_CPU_TIME_USER,     // user CPU time (seconds)
        e_CPU_TIME_SYSTEM,   // system CPU time (seconds)
        e_CPU_UTIL,          // weighted CPU % (user + system)
        e_CPU_UTIL_USER,     // weighted user CPU %
        e_CPU_UTIL_SYSTEM,   // weighted system CPU %
        e_RESIDENT_SIZE,     // number of MBs of physical memory
        e_NUM_THREADS,       // number of threads
        e_NUM_PAGEFAULTS,    // number of pagefaults (major + minor)
        e_VIRTUAL_SIZE,      // number of MBs in the heap
        e_NUM_MEASURES
    };

    class Statistics {
        // Defines the performance statistics collected for a monitored
        // process.  Note that this class is not fully value-semantic.  It is
        // intended to provide a read-only view of a set of collected
        // performance statistics.

        // FRIENDS
        friend class Collector<OsType>;
            // Grants write-access to the specific 'Collector' instantiation
            // for the current platform.

        // DATA
        int                d_pid;                      // process identifier
        bsl::string        d_description;              // process description

        bdlt::Datetime     d_startTimeUtc;             // process start time,
                                                       // in UTC time

        bsls::TimeInterval d_startTime;                // process start time,
                                                       // since the system
                                                       // epoch

        double             d_elapsedTime;              // time elapsed since
                                                       // process startup

        bsls::AtomicInt    d_numSamples;               // num samples taken

        double             d_lstData[e_NUM_MEASURES];  // latest collected data
        double             d_minData[e_NUM_MEASURES];  // min
        double             d_maxData[e_NUM_MEASURES];  // max
        double             d_totData[e_NUM_MEASURES];  // cumulative

        mutable bslmt::RWMutex d_guard;  // serialize write access

    private:
        // NOT IMPLEMENTED
        Statistics(const Statistics&);
        Statistics& operator=(const Statistics&);

    public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(Statistics,
                                       bslma::UsesBslmaAllocator);


        // CREATORS
        explicit Statistics(bslma::Allocator *basicAllocator = 0);
            // Create an instance of this class.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        // MANIPULATORS
        void reset();
            // Reset the min, max, and average values collected for each
            // measure.

        // ACCESSORS
        double latestValue(Measure measure) const;
            // Return the latest collected value for the specified 'measure'.

        double minValue(Measure measure) const;
            // Return the minimum collected value for the specified 'measure'.

        double maxValue(Measure measure) const;
            // Return the maximum collected value for the specified 'measure'.

        double avgValue(Measure measure) const;
            // Return the average of the collected values for the specified
            // 'metric'.

        int pid() const;
            // Return the pid for which these statistics were collected.

        const bsl::string& description() const;
            // Return the user-supplied description of the process identified
            // by the result of the 'pid()' function.

        double elapsedTime() const;
            // Return the number of seconds (in wall time) that have elapsed
            // since the startup the process identified by the result of the
            // 'pid()' function.

        const bdlt::Datetime& startupTime() const;
            // Return the startup time in Coordinated Universal Time.

        void print(bsl::ostream& os) const;
            // Print all collected statistics to the specified 'os' stream.

        void print(bsl::ostream& os, Measure measure) const;
            // Print the specified 'measure' to the specified 'os' stream.

        void print(bsl::ostream& os, const char *measureIdentifier) const;
            // Print the specified 'measureIdentifier' to the specified 'os'
            // stream.  The value of 'measureIdentifier' should be a string
            // literal corresponding to the desired measure enumerator, e.g.,
            // 'e_CPU_TIME'.
    };

    class ConstIterator {
        // Provide a mechanism that models the "Forward Iterator" concept over
        // a collection of non-modifiable performance statistics.

        // FRIENDS

        friend class PerformanceMonitor;  // grant access to the private
                                          // constructor

        // INSTANCE DATA

        PidMap::const_iterator  d_it;          // wrapped iterator
        bslmt::RWMutex         *d_mapGuard_p;  // serialize access to the map

        // PRIVATE CREATORS
        explicit ConstIterator(PidMap::const_iterator  it,
                               bslmt::RWMutex         *mapGuard);
            // Create an instance of this class that wraps the specified 'it'
            // iterator protected by the specified 'mapGuard'.

      public:
        // TYPES
        typedef bsl::forward_iterator_tag iterator_category;
            // Defines a type alias for the tag type that represents the
            // iterator concept this class models.

        typedef Statistics value_type;
            // Defines a type alias for the type of the result of dereferencing
            // this iterator.

        typedef bsl::ptrdiff_t difference_type;
            // Defines a type alias for the type of the result of the
            // difference between the addresses of two value types.

        typedef const Statistics* pointer;
            // Defines a type alias for a pointer to this iterator's value
            // type.

        typedef const Statistics& reference;
            // Defines a type alias for a reference to this iterator's value
            // type.

        // CREATORS
        ConstIterator();
            // Create an instance of this class having an invalid value.

        // MANIPULATORS
        ConstIterator& operator++();
            // Advance this iterator to refer to the next collection of
            // statistics for a monitored pid and return a reference to the
            // modifiable value type of this iterator.  The behavior of this
            // function is undefined unless this iterator is dereferenceable.

        ConstIterator operator++(int);
            // Advance this iterator to refer to the next collection of
            // statistics for a monitored pid and return the iterator pointing
            // to the previous modifiable value type.  The behavior of this
            // function is undefined unless this iterator is dereferenceable.

        // ACCESSORS
        reference operator*() const;
            // Return a reference to the non-modifiable value type of this
            // iterator.

        pointer operator->() const;
            // Return a reference to the non-modifiable value type of this
            // iterator.

        bool operator==(const ConstIterator& rhs) const;
            // Return 'true' if the specified 'rhs' iterator points to the
            // same instance of the iterator's value type as "this" iterator,
            // and 'false' otherwise.  The behavior of this function is
            // undefined unless the 'rhs' iterator and "this" iterator both
            // iterate over the same collection of Statistics.

        bool operator!=(const ConstIterator& rhs) const;
            // Return 'true' if the specified 'rhs' iterator does not point to
            // the same instance of the iterator's value type as "this"
            // iterator, and 'false' otherwise.  The behavior of this function
            // is undefined unless the 'rhs' iterator and "this" iterator both
            // iterate over the same collection of Statistics.
    };

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(PerformanceMonitor,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit
    PerformanceMonitor(bslma::Allocator *basicAllocator = 0);
        // Create an instance of this class to collect performance statistics
        // on demand (via the 'collect' method).  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    PerformanceMonitor(bdlmt::TimerEventScheduler *scheduler,
                       double                      interval,
                       bslma::Allocator           *basicAllocator = 0);
        // Create an instance of this class that uses the specified 'scheduler'
        // to automatically collect performance statistics every specified
        // 'interval' (specified in seconds).  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  Note that a
        // non-positive 'interval' value indicates that performance statistics
        // should *not* be automatically collected--in this case the user is
        // responsible for manually calling the 'collect' function.

    ~PerformanceMonitor();
        // Destroy this object.

    // MANIPULATORS
    int registerPid(int pid, const bsl::string& description);
        // Register the specified process 'pid' having the specified
        // user-defined 'description' with this performance monitor.  After
        // registration, performance statistics will be collected for the
        // 'pid' upon invocation of the 'collect' function.  Note that a 'pid'
        // value of zero is interpreted as the 'pid' of the current process.
        // Return 0 on success or a non-zero value otherwise.

    int unregisterPid(int pid);
        // Unregister the specified process 'pid' from the performance monitor.
        // After unregistration, to statistics for the 'pid' will be no longer
        // be available unless the 'pid' is re-registered with the performance
        // monitor through calling 'registerPid'.  Note that a 'pid' value of
        // zero is interpreted as the 'pid' of the current process.  Return
        // 0 on success or a non-zero value otherwise.

    void setCollectionInterval(double interval);
        // Set the specified time 'interval', in seconds, after which
        // statistics for each registered pid are automatically collected.  The
        // behavior is undefined unless a scheduler was supplied at the
        // construction of this performance monitor.  Note that a non-positive
        // 'interval' value indicates that performance statistics should not be
        // automatically collected--in this case the user is responsible for
        // manually calling the 'collect' function.

    void collect();
        // Collect performance statistics for each registered pid.

    void resetStatistics();
        // Reset the collected min, max, and average values collected for each
        // measure for each monitored process.

    // ACCESSORS
    ConstIterator begin() const;
        // Return an iterator positioned at the first set of collected
        // performance statistics.

    ConstIterator end() const;
        // Return an iterator that represents the end of the sequence of sets
        // of collected performance statistics.

    ConstIterator find(int pid) const;
        // Return the iterator pointing to the set of collected performance
        // statistics for the specified 'pid' if 'pid' has been registered with
        // this performance monitor through the 'registerPid' function,
        // otherwise return 'end()'.

    int numRegisteredPids() const;
        // Return the number of processes registered for statistics collection.
};
}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

               // ---------------------------------------------
               // class balb::PerformanceMonitor::ConstIterator
               // ---------------------------------------------

// CREATORS
inline
balb::PerformanceMonitor::ConstIterator::ConstIterator()
: d_it()
{
}

inline
balb::PerformanceMonitor::ConstIterator::ConstIterator(
               balb::PerformanceMonitor::PidMap::const_iterator  it,
               bslmt::RWMutex                                   *mapGuard)
: d_it(it)
, d_mapGuard_p(mapGuard)
{
}

// ACCESSORS
inline
balb::PerformanceMonitor::ConstIterator::reference
balb::PerformanceMonitor::ConstIterator::operator*() const
{
    return *d_it->second.first;
}

inline
balb::PerformanceMonitor::ConstIterator::pointer
balb::PerformanceMonitor::ConstIterator::operator->() const
{
    return d_it->second.first.get();
}

// MANIPULATORS
inline
balb::PerformanceMonitor::ConstIterator&
balb::PerformanceMonitor::ConstIterator::operator++()
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(d_mapGuard_p);
    ++d_it;
    return *this;
}

inline
balb::PerformanceMonitor::ConstIterator
balb::PerformanceMonitor::ConstIterator::operator++(int)
{
    balb::PerformanceMonitor::ConstIterator temp = *this;
    ++*this;
    return temp;
}

// ACCESSORS
inline
bool balb::PerformanceMonitor::ConstIterator::operator==(
                                                const ConstIterator& rhs) const
{
    return d_it == rhs.d_it;
}

inline
bool balb::PerformanceMonitor::ConstIterator::operator!=(
                                                const ConstIterator& rhs) const
{
    return d_it != rhs.d_it;
}

                 // ------------------------------------------
                 // class balb::PerformanceMonitor::Statistics
                 // ------------------------------------------

// ACCESSORS
inline
double balb::PerformanceMonitor::Statistics::latestValue(Measure measure) const
{
    BSLS_ASSERT_SAFE(measure >= 0 && measure < e_NUM_MEASURES);

    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_guard);
    return d_lstData[measure];
}

inline
double balb::PerformanceMonitor::Statistics::minValue(Measure measure) const
{
    BSLS_ASSERT_SAFE(measure >= 0 && measure < e_NUM_MEASURES);

    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_guard);
    return d_minData[measure];
}

inline
double balb::PerformanceMonitor::Statistics::maxValue(Measure measure) const
{
    BSLS_ASSERT_SAFE(measure >= 0 && measure < e_NUM_MEASURES);

    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_guard);
    return d_maxData[measure];
}

inline
double balb::PerformanceMonitor::Statistics::avgValue(Measure measure) const
{
    BSLS_ASSERT_SAFE(measure >= 0 && measure < e_NUM_MEASURES);

    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_guard);
    return d_totData[measure] / d_numSamples;
}

inline
int balb::PerformanceMonitor::Statistics::pid() const
{
    return d_pid;
}

inline
const bsl::string& balb::PerformanceMonitor::Statistics::description() const
{
    return d_description;
}

inline
double balb::PerformanceMonitor::Statistics::elapsedTime() const
{
    return d_elapsedTime;
}

inline
const bdlt::Datetime& balb::PerformanceMonitor::Statistics::startupTime() const
{
    return d_startTimeUtc;
}

namespace balb {
                          // ------------------------
                          // class PerformanceMonitor
                          // ------------------------

// ACCESSORS
inline
PerformanceMonitor::ConstIterator
PerformanceMonitor::begin() const
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_mapGuard);
    return ConstIterator(d_pidMap.begin(), &d_mapGuard);
}

inline
PerformanceMonitor::ConstIterator
PerformanceMonitor::end() const
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_mapGuard);
    return ConstIterator(d_pidMap.end(), &d_mapGuard);
}

inline
PerformanceMonitor::ConstIterator
PerformanceMonitor::find(int pid) const
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_mapGuard);
    return ConstIterator(d_pidMap.find(pid), &d_mapGuard);
}

inline
int
PerformanceMonitor::numRegisteredPids() const
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_mapGuard);
    return static_cast<int>(d_pidMap.size());
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
