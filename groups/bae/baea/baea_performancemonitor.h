// baea_performancemonitor.h                                          -*-C++-*-
#ifndef INCLUDED_BAEA_PERFORMANCEMONITOR
#define INCLUDED_BAEA_PERFORMANCEMONITOR

//@PURPOSE: Provide a mechanism to collect process performance measures.
//
//@CLASSES:
//                  baea_PerformanceMonitor: monitor process performance
//      baea_PerformanceMonitor::Statistics: performance stats
//   baea_PerformanceMonitor::ConstIterator: stats iteration
//
//@AUTHOR: Matthew Millett (mmillett2)
//
//@DESCRIPTION: This component provides an application developer the means to
// collect and report performance statistics for an arbitrary number of
// processes running on the local machine.  The following table describes the
// measures collecting by this component.  Note that all the collected measures
// are specific to the monitored process and do not refer to any system-wide
// measurement.
//..
// Measure           Identifier           Description
// -------           ----------           -----------
// User CPU Time     BAEA_CPU_TIME_USER   Total amount of time spent executing
//                                        instructions in user mode.
//
// System CPU Time   BAEA_CPU_TIME_SYSTEM Total amount of time spent executing
//                                        instructions in kernel mode.
//
// CPU Time          BAEA_CPU_TIME        The sum of User and System CPU times.
//
// User CPU %        BAEA_CPU_UTIL_USER   Percentage of elapsed CPU time this
//                                        process spent executing instructions
//                                        in user mode.
//
// System CPU %      BAEA_CPU_UTIL_SYSTEM Percentage of elapsed CPU time this
//                                        process spent executing instructions
//                                        in kernel mode.
//
// CPU %             BAEA_CPU_UTIL        Sum of User CPU % and System CPU %.
//
// Resident Size     BAEA_RESIDENT_SIZE   Number of mega-bytes of physical 
//                                        memory used by the process.
//
// Virtual Size      BAEA_VIRTUAL_SIZE    The size of the heap, in 
//                                        mega-bytes. This value does not
//                                        include the size of the address
//                                        space mapped to files (anonymous or
//                                        otherwise.)
//
// Thread Count      BAEA_NUM_THREADS     Number of threads executing in the
//                                        process.
//
// Page Faults       BAEA_NUM_PAGEFAULTS  Total number of page faults incurred
//                                        throughout the lifetime of the
//                                        process.
//..
//
// Iterator Invalidation
// ---------------------
// Registration of new pids does not invalidate existing iterators.
// Unregistration of pid only invalidates iterators pointing to the statistics
// for the pid being unregistered, all other iterators remain valid.
//
// Thread Safety
// -------------
// This class is completely thread safe.
//
// Usage
// -----
// The following example shows how to monitor the currently executing process
// and produce a formatted report of the collected measures after a certain
// interval.
//..
//  // Instantiate a scheduler used by the performance monitor to schedule
//  // collection events.
//  bcep_TimerEventScheduler scheduler;
//  scheduler.start();
//
//  // Create the performance monitor, monitoring the current process and
//  // auto-collecting statistics every second.
//  baea_PerformanceMonitor perfmon(&scheduler, 1.0);
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
//      bcemt_ThreadUtil::microSleep(0, 10);
//
//      for (baea_PerformanceMonitor::ConstIterator it  = perfmon.begin();
//                                                  it != perfmon.end();
//                                                ++it) {
//          const baea_PerformanceMonitor::Statistics& stats = *it;
//          bsl::cout << "Pid = " << stats.pid() << ":\n";
//          stats.print(bsl::cout);
//      }
//  }
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BCEP_TIMEREVENTSCHEDULER
#include <bcep_timereventscheduler.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BCEMT_READLOCKGUARD
#include <bcemt_readlockguard.h>
#endif

#ifndef INCLUDED_BCEMT_RWMUTEX
#include <bcemt_rwmutex.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

                       // ========================
                       // class PerformanceMonitor
                       // ========================

class baea_PerformanceMonitor {
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
    // bdes_platform component.  This type alias is used to specifically select
    // a particular template specialization of the 'Collector' template.

#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_CYGWIN)
    typedef bsls_Platform::OsLinux   OsType;
#elif defined(BDES_PLATFORM_OS_FREEBSD)
    typedef bsls_Platform::OsFreeBsd OsType;
#elif defined(BDES_PLATFORM_OS_DARWIN)
    typedef bsls_Platform::OsDarwin OsType;
/*
#elif defined(BSLS_PLATFORM_OS_HPUX)
typedef bsls_Platform::OsHpUx OsType;
*/
#elif defined(BSLS_PLATFORM_OS_UNIX)
    typedef bsls_Platform::OsUnix    OsType;
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    typedef bsls_Platform::OsWindows OsType;
#endif

    template <typename PLATFORM>
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

    typedef bcema_SharedPtr<CollectorType> CollectorPtr;
        // Defines a type alias for the shared pointer to the platform-specific
        // mechanism used to collect the performance measures for a pid.

    typedef bcema_SharedPtr<Statistics> StatisticsPtr;
        // Defines a type alias for the shared pointer to the platform-specific
        // mechanism used to collect the performance measures for a pid.

    typedef bsl::map<int, bsl::pair<StatisticsPtr, CollectorPtr> > PidMap;
        // Defines a type alias for the map of pids to their collected
        // statistics and associated platform-specific collector
        // implementation.

    // DATA
    PidMap                            d_pidMap;      // map of pid stats

    double                            d_interval;    // collection interval

    bcep_TimerEventScheduler         *d_scheduler_p; // scheduler of collection
                                                     // events (held)

    bcep_TimerEventScheduler::Handle  d_clock;       // handle to collection
                                                     // timer

    mutable bcemt_RWMutex             d_mapGuard;    // serializes write access
                                                     // to 'd_pidMap'

    bslma_Allocator                  *d_allocator_p; // supplies memory (held)

  private:
    // NOT IMPLEMENTED
    baea_PerformanceMonitor(const baea_PerformanceMonitor&);
    baea_PerformanceMonitor& operator=(const baea_PerformanceMonitor&);

  public:
    // TYPES
    enum Measure {
        // Enumerates the set of performance measures this class is capable of
        // monitoring.

        BAEA_CPU_TIME,          // CPU time (seconds)
        BAEA_CPU_TIME_USER,     // user CPU time (seconds)
        BAEA_CPU_TIME_SYSTEM,   // system CPU time (seconds)
        BAEA_CPU_UTIL,          // weighted CPU % (user + system)
        BAEA_CPU_UTIL_USER,     // weighted user CPU %
        BAEA_CPU_UTIL_SYSTEM,   // weighted system CPU %
        BAEA_RESIDENT_SIZE,     // number of MBs of physical memory 
        BAEA_NUM_THREADS,       // number of threads
        BAEA_NUM_PAGEFAULTS,    // number of pagefaults (major + minor)
        BAEA_VIRTUAL_SIZE,      // number of MBs in the heap
        BAEA_NUM_MEASURES
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , CPU_TIME        = BAEA_CPU_TIME
      , CPU_TIME_USER   = BAEA_CPU_TIME_USER
      , CPU_TIME_SYSTEM = BAEA_CPU_TIME_SYSTEM
      , CPU_UTIL        = BAEA_CPU_UTIL
      , CPU_UTIL_USER   = BAEA_CPU_UTIL_USER
      , CPU_UTIL_SYSTEM = BAEA_CPU_UTIL_SYSTEM
      , RESIDENT_SIZE   = BAEA_RESIDENT_SIZE
      , NUM_THREADS     = BAEA_NUM_THREADS
      , NUM_PAGEFAULTS  = BAEA_NUM_PAGEFAULTS
      , VIRTUAL_SIZE    = BAEA_VIRTUAL_SIZE
      , NUM_MEASURES    = BAEA_NUM_MEASURES
#endif
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
        int              d_pid;                   // process identifier
        bsl::string      d_description;           // process description

        bdet_Datetime    d_startTimeUtc;          // process start time, in
                                                  // UTC time

        bdet_TimeInterval d_startTime;            // process start time, since
                                                  // the system epoch

        double           d_elapsedTime;           // time elapsed since process
                                                  // startup

        bces_AtomicInt   d_numSamples;            // num samples taken

        double           d_lstData[BAEA_NUM_MEASURES]; // latest collected data
        double           d_minData[BAEA_NUM_MEASURES]; // min
        double           d_maxData[BAEA_NUM_MEASURES]; // max
        double           d_totData[BAEA_NUM_MEASURES]; // cumulative

        mutable bcemt_RWMutex
                         d_guard;                 // serialize write access

    private:
        // NOT IMPLEMENTED
        Statistics(const Statistics&);
        Statistics& operator=(const Statistics&);

    public:
        // CREATORS
        explicit Statistics(bslma_Allocator *basicAllocator = 0);
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

        const bdet_Datetime& startupTime() const;
            // Return the startup time in Coordinated Universal Time.

        void print(bsl::ostream& os) const;
            // Print all collected statistics to the specified 'os' stream.

        void print(bsl::ostream& os, Measure measure) const;
            // Print the specified 'measure' to the specified 'os' stream.

        void print(bsl::ostream& os, const char *measureIdentifier) const;
            // Print the specified 'measureDescription' to the specified 'os'
            // stream.  The value of measureIdentifier should be a string
            // literal corresponding to the desired measure enumerator, e.g.,
            // 'BAEA_CPU_TIME'.
    };

    class ConstIterator {
        // Provide a mechanism that models the "Forward Iterator" concept
        // over a collection of non-modifiable performance statistics.

        // FRIENDS

        friend class baea_PerformanceMonitor;  // grant access to the
                                               // private constructor

        // INSTANCE DATA

        PidMap::const_iterator  d_it;          // wrapped iterator
        bcemt_RWMutex          *d_mapGuard_p;  // serialize access to the map

        // PRIVATE CREATORS
        explicit ConstIterator(PidMap::const_iterator  it,
                               bcemt_RWMutex          *mutex);
            // Create an instance of this class that wraps the specified 'it'
            // iterator protected by the specified 'mutex'.

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
            // function is undefined unless this iterator is dereferencible.

        ConstIterator operator++(int);
            // Advance this iterator to refer to the next collection of
            // statistics for a monitored pid and return the iterator pointing
            // to the previous modifiable value type.  The behavior of this
            // function is undefined unless this iterator is dereferencible.

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

    // CREATORS
    explicit
    baea_PerformanceMonitor(bslma_Allocator *basicAllocator = 0);
        // Create an instance of this class to collect performance statistics
        // on demand (via the 'collect' method).  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    baea_PerformanceMonitor(bcep_TimerEventScheduler *scheduler,
                            double                    interval,
                            bslma_Allocator          *basicAllocator = 0);
        // Create an instance of this class that uses the specified 'scheduler'
        // to automatically collect performance statistics every specified
        // 'interval' (specified in seconds).  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  Note that a
        // non-positive 'interval' value indicates that performance statistics
        // should *not* be automatically collected--in this case the user is
        // responsible for manually calling the 'collect' function.

    ~baea_PerformanceMonitor();
        // Destroy this object.

    // MANIPULATORS
    int registerPid(int pid, const bsl::string& description);
        // Register the specified process 'pid' having the specified
        // user-defined 'description' with this performance monitor.  After
        // registration, performance statistics will be collected for the
        // 'pid' upon invocation of the 'collect' function.  Note that a pid
        // value of zero is interpreted as the pid of the current process.
        // Return 0 on success or a non-zero value otherwise.

    int unregisterPid(int pid);
        // Unregister the specified process 'pid' from the performance monitor.
        // After unregistration, to statistics for the 'pid' will be no longer
        // be available unless the pid is re-registered with the performance
        // monitor through calling 'registerPid'.  Note that a pid value of
        // zero is interpreted as the pid of the current process.  Return
        // 0 on success or a non-zero value otherwise.

    void setCollectionInterval(double interval);
        // Set the specified time 'interval', in seconds, after which
        // statistics for each registered pid are automatically collected.
        // The behavior is undefined unless a scheduler was supplied at
        // the construction of this performance monitor.  Note that a
        // non-positive 'interval' value indicates that performance statistics
        // should not be automatically collected--in this case the user is
        // responsible for manually calling the 'collect' function.

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
        // Return an iterator that represents the end of the sequence of
        // sets of collected performance statistics.

    ConstIterator find(int pid) const;
        // Return the iterator pointing to the set of collected performance
        // statistics for the specified 'pid' if the specified 'pid' has been
        // registered with this performance monitor through the 'registerPid'
        // function, otherwise return 'end()'.

    int numRegisteredPids() const;
        // Return the number of processes registered for statistics collection.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

              // --------------------------------------------
              // class baea_PerformanceMonitor::ConstIterator
              // --------------------------------------------

// CREATORS
inline
baea_PerformanceMonitor::ConstIterator::ConstIterator()
: d_it()
{
}

inline
baea_PerformanceMonitor::ConstIterator::ConstIterator(
               baea_PerformanceMonitor::PidMap::const_iterator  it,
               bcemt_RWMutex                                   *mapGuard)
: d_it(it)
, d_mapGuard_p(mapGuard)
{
}

// ACCESSORS
inline
baea_PerformanceMonitor::ConstIterator::reference
baea_PerformanceMonitor::ConstIterator::operator*() const
{
    return *d_it->second.first;
}

inline
baea_PerformanceMonitor::ConstIterator::pointer
baea_PerformanceMonitor::ConstIterator::operator->() const
{
    return d_it->second.first.ptr();
}

// MANIPULATORS
inline
baea_PerformanceMonitor::ConstIterator&
baea_PerformanceMonitor::ConstIterator::operator++()
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(d_mapGuard_p);
    ++d_it;
    return *this;
}

inline
baea_PerformanceMonitor::ConstIterator
baea_PerformanceMonitor::ConstIterator::operator++(int)
{
    baea_PerformanceMonitor::ConstIterator temp = *this;
    ++*this;
    return temp;
}

// ACCESSORS
inline
bool baea_PerformanceMonitor::ConstIterator::operator==(
                       const baea_PerformanceMonitor::ConstIterator& rhs) const
{
    return d_it == rhs.d_it;
}

inline
bool baea_PerformanceMonitor::ConstIterator::operator!=(
                       const baea_PerformanceMonitor::ConstIterator& rhs) const
{
    return d_it != rhs.d_it;
}

              // -----------------------------------------
              // class baea_PerformanceMonitor::Statistics
              // -----------------------------------------

// ACCESSORS
inline
double baea_PerformanceMonitor::Statistics::latestValue(Measure measure) const
{
    BSLS_ASSERT_SAFE(measure >= 0 && measure < BAEA_NUM_MEASURES);

    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_guard);
    return d_lstData[measure];
}

inline
double baea_PerformanceMonitor::Statistics::minValue(Measure measure) const
{
    BSLS_ASSERT_SAFE(measure >= 0 && measure < BAEA_NUM_MEASURES);

    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_guard);
    return d_minData[measure];
}

inline
double baea_PerformanceMonitor::Statistics::maxValue(Measure measure) const
{
    BSLS_ASSERT_SAFE(measure >= 0 && measure < BAEA_NUM_MEASURES);

    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_guard);
    return d_maxData[measure];
}

inline
double baea_PerformanceMonitor::Statistics::avgValue(Measure measure) const
{
    BSLS_ASSERT_SAFE(measure >= 0 && measure < BAEA_NUM_MEASURES);

    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_guard);
    return d_totData[measure] / d_numSamples;
}

inline
int baea_PerformanceMonitor::Statistics::pid() const
{
    return d_pid;
}

inline
const bsl::string& baea_PerformanceMonitor::Statistics::description() const
{
    return d_description;
}

inline
double baea_PerformanceMonitor::Statistics::elapsedTime() const
{
    return d_elapsedTime;
}

inline
const bdet_Datetime& baea_PerformanceMonitor::Statistics::startupTime() const
{
    return d_startTimeUtc;
}

                   // -----------------------------
                   // class baea_PerformanceMonitor
                   // -----------------------------

// ACCESSORS
inline
baea_PerformanceMonitor::ConstIterator
baea_PerformanceMonitor::begin() const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_mapGuard);
    return ConstIterator(d_pidMap.begin(), &d_mapGuard);
}

inline
baea_PerformanceMonitor::ConstIterator
baea_PerformanceMonitor::end() const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_mapGuard);
    return ConstIterator(d_pidMap.end(), &d_mapGuard);
}

inline
baea_PerformanceMonitor::ConstIterator
baea_PerformanceMonitor::find(int pid) const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_mapGuard);
    return ConstIterator(d_pidMap.find(pid), &d_mapGuard);
}

inline
int
baea_PerformanceMonitor::numRegisteredPids() const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_mapGuard);
    return static_cast<int>(d_pidMap.size());
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
