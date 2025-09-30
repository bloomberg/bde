// balb_performancemonitor.h                                          -*-C++-*-
#ifndef INCLUDED_BALB_PERFORMANCEMONITOR
#define INCLUDED_BALB_PERFORMANCEMONITOR

#include <bsls_ident.h>
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
// processes running on the local machine.  The `balb::PerformanceMonitor`
// provides the mechanism for doing so, the
// `balb::PerformanceMonitor::Statistics` class holds the data and the
// `balb::PerformanceMonitor::ConstIterator` class is used to iterate over the
// data.  The following table describes the measures collecting by this
// component.  Note that all the collected measures are specific to the
// monitored process and do not refer to any system-wide measurement.
// ```
// Measure           Identifier           Description
// -------           ----------           -----------
// User CPU Time     e_CPU_TIME_USER      Total amount of time spent executing
//                                        instructions in user mode.
//
// System CPU Time   e_CPU_TIME_SYSTEM    Total amount of time spent executing
//                                        instructions in kernel mode.
//
// CPU Time          e_CPU_TIME           The sum of User and System CPU times.
//
// User CPU %        e_CPU_UTIL_USER      Percentage of elapsed CPU time this
//                                        process spent executing instructions
//                                        in user mode.
//
// System CPU %      e_CPU_UTIL_SYSTEM    Percentage of elapsed CPU time this
//                                        process spent executing instructions
//                                        in kernel mode.
//
// CPU %             e_CPU_UTIL           Sum of User CPU % and System CPU %.
//
// Resident Size     e_RESIDENT_SIZE      Number of mega-bytes of physical
//                                        memory used by the process.
//
// Virtual Size      e_VIRTUAL_SIZE       The size of the heap, in
//                                        mega-bytes. This value does not
//                                        include the size of the address
//                                        space mapped to files (anonymous or
//                                        otherwise.)
//
// Thread Count      e_NUM_THREADS        Number of threads executing in the
//                                        process.
//
// Page Faults       e_NUM_PAGEFAULTS     Total number of page faults incurred
//                                        throughout the lifetime of the
//                                        process.
// ```
//
///OS-Specific Permissions
///-----------------------
// Various OSs might require specific permissions in order to inspect processes
// other than the current process.  For example, on Darwin, users other than
// root can only inspect processes running under the user with which the
// current process is running.  This error condition will be indicated by a
// non-zero return value from `registerPid`.
//
///Unsupported Platform/Compiler mode - Solaris/g++ <= 11/32-bits
///--------------------------------------------------------
// Note that this component is not supported when building in 32-bit mode
// with the g++ compiler version 11 or below on Solaris, due to Solaris's
// `procfs.h` header not supporting that compiler in 32-bit mode.
// (DRQS 170291732)
//
///Iterator Invalidation
///---------------------
// Registration of new pids does not invalidate existing iterators.
// Unregistration of a pid invalidates only iterators pointing to the
// statistics for the pid being unregistered.  Additionally, unregistering a
// pid with a `balb::PerformanceMonitor` object invalidates all references to
// `Statistics` objects retrieved from those iterators.  All other iterators
// remain valid.
//
///Thread Safety
///-------------
// The classes `balb::PerformanceMonitor` and
// `balb::PerformanceMonitor::Statistics`, provided by this component, are
// both independently fully *thread-safe* (see `bsldoc_glossary`).  However,
// `balb::PerformanceMonitor::ConstIterator` is only *const* *thread-safe*,
// meaning it is not safe to access or modify a `ConstIterator` in one thread
// while another thread modifies the same object.  As unregistering a pid with
// a `balb::PerformanceMonitor` object invalidates iterators (see {Iterator
// Invalidation}), external synchronization is needed if `unregisterPid` is
// called concurrently to iterating over statistics.  Also, in a multi-threaded
// context, a `Statistics` object accessed via a reference (or pointer) from
// a `ConstIterator` object may have its statistics updated at any time by a
// call to `collect` or `resetStatistics` in another thread.   If consistent
// access is needed to multiple items in a set of statistics, then the user
// should copy the statistics object, and then inspect the copy at their
// leisure.
//
// Notice that this component was implemented with particular usage patterns in
// mind, which are captured in the usage examples.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of `balb::PerformanceMonitor`
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example shows how to monitor the currently executing process
// and produce a formatted report of the collected measures after a certain
// interval.
//
// First, we instantiate a scheduler used by the performance monitor to
// schedule collection events.
// ```
// bdlmt::TimerEventScheduler scheduler;
// scheduler.start();
// ```
// Then, we create the performance monitor, monitoring the current process and
// auto-collecting statistics every second.
// ```
// balb::PerformanceMonitor perfmon(&scheduler, 1.0);
// int                      rc  = perfmon.registerPid(0, "perfmon");
// const int                pid = bdls::ProcessUtil::getProcessId();
//
// assert(0 == rc);
// assert(1 == perfmon.numRegisteredPids());
// ```
// Next, we print a formatted report of the performance statistics collected
// for each pid every 5 seconds for half a minute.  Note, that `Statistics`
// object can be simultaneously modified by scheduler callback and accessed via
// a `ConstIterator`.  To ensure that the call to `Statistics::print` outputs
// consistent data from a single update of the statistics for this process, we
// create a local copy (copy construction is guaranteed to be thread-safe).
// ```
// for (int i = 0; i < 6; ++i) {
//     bslmt::ThreadUtil::microSleep(0, 5);
//
//     balb::PerformanceMonitor::ConstIterator    it    = perfmon.find(0);
//     const balb::PerformanceMonitor::Statistics stats = *it;
//
//     assert(pid == stats.pid());
//
//     bsl::cout << "PID = " << stats.pid() << ":\n";
//     stats.print(bsl::cout);
// }
// ```
// Finally, we unregister the process and stop the scheduler to cease
// collecting statistics for this process.  It is safe to call `unregisterPid`
// here, because we don't have any `ConstIterators` objects or references to
// `Statistics` objects.
// ```
// rc  = perfmon.unregisterPid(pid);
//
// assert(0 == rc);
// assert(0 == perfmon.numRegisteredPids());
//
// scheduler.stop();
// ```

#include <balscm_version.h>

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
#include <bsla_deprecated.h>
#endif

#include <bdlmt_timereventscheduler.h>
#include <bdls_processutil.h>
#include <bdlt_datetime.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmt_readlockguard.h>
#include <bslmt_rwmutex.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_platform.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bsl_iosfwd.h>
#include <bsl_iterator.h>
#include <bsl_map.h>
#include <bsl_memory.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace balb {

                          // ========================
                          // class PerformanceMonitor
                          // ========================

/// Provides a mechanism to collect performance statistics for an arbitrary
/// number of processes running on the local machine.
class PerformanceMonitor {

  public:
    // TYPES

    // Enumerates the set of performance measures this class is capable of
    // monitoring.  Note that CPU utilization measures are calculated as a
    // ratio of CPU time to elapsed time between two consecutive calls to
    // `collect`, and therefore no value will be available if `collect` has
    // been called only once.  Note that not all measures are supported on all
    // platforms; for an unsupported measure, the behavior is as if the system
    // reports a zero value.
    enum Measure {
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
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BAEA_CPU_TIME        BSLA_DEPRECATED = e_CPU_TIME
      , BAEA_CPU_TIME_USER   BSLA_DEPRECATED = e_CPU_TIME_USER
      , BAEA_CPU_TIME_SYSTEM BSLA_DEPRECATED = e_CPU_TIME_SYSTEM
      , BAEA_CPU_UTIL        BSLA_DEPRECATED = e_CPU_UTIL
      , BAEA_CPU_UTIL_USER   BSLA_DEPRECATED = e_CPU_UTIL_USER
      , BAEA_CPU_UTIL_SYSTEM BSLA_DEPRECATED = e_CPU_UTIL_SYSTEM
      , BAEA_RESIDENT_SIZE   BSLA_DEPRECATED = e_RESIDENT_SIZE
      , BAEA_NUM_THREADS     BSLA_DEPRECATED = e_NUM_THREADS
      , BAEA_NUM_PAGEFAULTS  BSLA_DEPRECATED = e_NUM_PAGEFAULTS
      , BAEA_VIRTUAL_SIZE    BSLA_DEPRECATED = e_VIRTUAL_SIZE
      , BAEA_NUM_MEASURES    BSLA_DEPRECATED = e_NUM_MEASURES
      , CPU_TIME             BSLA_DEPRECATED = e_CPU_TIME
      , CPU_TIME_USER        BSLA_DEPRECATED = e_CPU_TIME_USER
      , CPU_TIME_SYSTEM      BSLA_DEPRECATED = e_CPU_TIME_SYSTEM
      , CPU_UTIL             BSLA_DEPRECATED = e_CPU_UTIL
      , CPU_UTIL_USER        BSLA_DEPRECATED = e_CPU_UTIL_USER
      , CPU_UTIL_SYSTEM      BSLA_DEPRECATED = e_CPU_UTIL_SYSTEM
      , RESIDENT_SIZE        BSLA_DEPRECATED = e_RESIDENT_SIZE
      , NUM_THREADS          BSLA_DEPRECATED = e_NUM_THREADS
      , NUM_PAGEFAULTS       BSLA_DEPRECATED = e_NUM_PAGEFAULTS
      , VIRTUAL_SIZE         BSLA_DEPRECATED = e_VIRTUAL_SIZE
      , NUM_MEASURES         BSLA_DEPRECATED = e_NUM_MEASURES
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

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
#elif defined(BSLS_PLATFORM_OS_UNIX)
    typedef bsls::Platform::OsUnix    OsType;
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    typedef bsls::Platform::OsWindows OsType;
#endif

    /// Forward declares a class template for a performance measure
    /// collector for a parameterized `PLATFORM`.  This class template is
    /// never defined.  Instead, we define explicit specializations for
    /// supported platforms.  Any attempt to compile this component on
    /// unsupported platforms will result in a compile-time error.
    template <class PLATFORM>
    class Collector;

    /// Defines a type alias for the type that defines the private platform-
    /// specific mechanism used to collect the performance measures for a
    /// pid.
    typedef Collector<OsType> CollectorType;

    /// Defines a type alias for the shared pointer to the platform-specific
    /// mechanism used to collect the performance measures for a pid.
    typedef bsl::shared_ptr<CollectorType> CollectorPtr;

    /// Defines a type alias for the shared pointer to the platform-specific
    /// mechanism used to collect the performance measures for a pid.
    typedef bsl::shared_ptr<Statistics> StatisticsPtr;

    /// Defines a type alias for the map of pids to their collected
    /// statistics and associated platform-specific collector
    /// implementation.
    typedef bsl::map<int, bsl::pair<StatisticsPtr, CollectorPtr> > PidMap;

    /// Enumeration used to distinguish rate and non-rate measures.  A rate
    /// measure (i.e. a CPU utilization rate) is calculated by dividing another
    /// quantity by the elapsed time between two collections.
    enum MeasureType {
        e_NON_RATE_MEASURE,
        e_RATE_MEASURE,
        e_NUM_MEASURE_TYPES
    };

    /// This struct is defined in the .cpp file.
    struct MeasureData;

    // CLASS DATA

    /// This array contains the properties of all measures defined by this
    /// class.
    static const MeasureData s_measureData[e_NUM_MEASURES];

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

    /// Defines the performance statistics collected for a monitored
    /// process.  Note that this class is not fully value-semantic.  It is
    /// intended to provide a read-only view of a set of collected
    /// performance statistics.
    class Statistics {

        // FRIENDS
        friend class Collector<OsType>;
            // Grants write-access to the specific 'Collector' instantiation
            // for the current platform.

        // DATA
        int                    d_pid;
                                 // process identifier

        bsl::string            d_description;
                                 // process description

        bdlt::Datetime         d_startTimeUtc;
                                 // process start time, in UTC time

        bsls::TimeInterval     d_startTime;
                                 // process start time, since the system
                                 // epoch

        double                 d_elapsedTime;
                                 // time elapsed since process startup

        int                    d_numSamples[e_NUM_MEASURE_TYPES];
                                 // num samples taken, indexed by measure type

        double                 d_lstData[e_NUM_MEASURES];
                                 // latest collected data

        double                 d_minData[e_NUM_MEASURES];
                                 // min

        double                 d_maxData[e_NUM_MEASURES];
                                 // max

        double                 d_totData[e_NUM_MEASURES];
                                 // cumulative

        mutable bslmt::RWMutex d_guard;
                                 // serialize write access

      private:
        // NOT IMPLEMENTED
        Statistics& operator=(const Statistics&);

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(Statistics,
                                       bslma::UsesBslmaAllocator);

        // CREATORS

        /// Create an instance of this class.  Optionally specify a
        /// `basicAllocator` used to supply memory.  If `basicAllocator` is
        /// 0, the currently installed default allocator is used.
        explicit Statistics(bslma::Allocator *basicAllocator = 0);

        /// Create a `Statistics` object aggregating the same statistics
        /// values as the specified `original` object.  Optionally specify a
        /// `basicAllocator` used to supply memory.  If `basicAllocator` is
        /// 0, the currently installed default allocator is used.
        Statistics(const Statistics&  original,
                   bslma::Allocator  *basicAllocator = 0);

        // MANIPULATORS

        /// Reset this object to the state in which no samples have been
        /// collected.  Note that although this method is public, it can't be
        /// called directly by users, since `PerformanceMonitor` provides only
        /// const access to statistics.
        void reset();

        // ACCESSORS

        /// Return the latest collected value for the specified `measure`, or 0
        /// if no values are yet available.
        double latestValue(Measure measure) const;

        /// Return the minimum collected value for the specified `measure`, or
        /// a large positive value if no values have been collected.
        double minValue(Measure measure) const;

        /// Return the maximum collected value for the specified `measure`, or
        /// a large negative value if no values have been collected.
        double maxValue(Measure measure) const;

        /// Return the average of the collected values for the specified
        /// `metric`, or an unspecified value if no values have been collected.
        double avgValue(Measure measure) const;

        /// Return the pid for which these statistics were collected.
        int pid() const;

        /// Return the user-supplied description of the process identified
        /// by the result of the `pid()` function.
        const bsl::string& description() const;

        /// Return the number of seconds (in wall time) that have elapsed
        /// since the startup the process identified by the result of the
        /// `pid()` function.
        double elapsedTime() const;

        /// Return the startup time in Coordinated Universal Time.
        const bdlt::Datetime& startupTime() const;

        /// Print all collected statistics to the specified `os` stream.
        void print(bsl::ostream& os) const;

        /// Print the specified `measure` to the specified `os` stream.
        void print(bsl::ostream& os, Measure measure) const;

        /// Print the specified `measureIdentifier` to the specified `os`
        /// stream.  The value of `measureIdentifier` should be a string
        /// literal corresponding to the desired measure enumerator, e.g.,
        /// `e_CPU_TIME`.
        void print(bsl::ostream& os, const char *measureIdentifier) const;
    };

    /// Provide a mechanism that models the "Forward Iterator" concept over
    /// a collection of non-modifiable performance statistics.
    class ConstIterator {

        // FRIENDS
        friend class PerformanceMonitor;  // grant access to the private
                                          // constructor

        // DATA
        PidMap::const_iterator  d_it;          // wrapped iterator
        bslmt::RWMutex         *d_mapGuard_p;  // serialize access to the map

        // PRIVATE CREATORS

        /// Create an instance of this class that wraps the specified `it`
        /// iterator protected by the specified `mapGuard`.
        explicit ConstIterator(PidMap::const_iterator  it,
                               bslmt::RWMutex         *mapGuard);

      public:
        // TYPES

        /// Defines a type alias for the tag type that represents the
        /// iterator concept this class models.
        typedef bsl::forward_iterator_tag iterator_category;

        /// Defines a type alias for the type of the result of dereferencing
        /// this iterator.
        typedef Statistics value_type;

        /// Defines a type alias for the type of the result of the
        /// difference between the addresses of two value types.
        typedef bsl::ptrdiff_t difference_type;

        /// Defines a type alias for a pointer to this iterator's value
        /// type.
        typedef const Statistics* pointer;

        /// Defines a type alias for a reference to this iterator's value
        /// type.
        typedef const Statistics& reference;

        // CREATORS

        /// Create an instance of this class having an invalid value.
        ConstIterator();

        // MANIPULATORS

        /// Advance this iterator to refer to the next collection of
        /// statistics for a monitored pid and return a reference to the
        /// modifiable value type of this iterator.  If there is no next
        /// collection of statistics, this iterator will be set equal to
        /// `end()`.  The behavior of this function is undefined unless this
        /// iterator is dereferenceable.
        ConstIterator& operator++();

        /// Advance this iterator to refer to the next collection of
        /// statistics for a monitored pid and return the iterator pointing
        /// to the previous modifiable value type.  If there is no next
        /// collection of statistics, this iterator will be set equal to
        /// `end()`.  The behavior of this function is undefined unless this
        /// iterator is dereferenceable.
        ConstIterator operator++(int);

        // ACCESSORS

        /// Return a reference to the non-modifiable value type of this
        /// iterator.
        reference operator*() const;

        /// Return a reference to the non-modifiable value type of this
        /// iterator.
        pointer operator->() const;

        /// Return `true` if the specified `rhs` iterator points to the
        /// same instance of the iterator's value type as "this" iterator,
        /// and `false` otherwise.  The behavior of this function is
        /// undefined unless the `rhs` iterator and "this" iterator both
        /// iterate over the same collection of Statistics.
        bool operator==(const ConstIterator& rhs) const;

        /// Return `true` if the specified `rhs` iterator does not point to
        /// the same instance of the iterator's value type as "this"
        /// iterator, and `false` otherwise.  The behavior of this function
        /// is undefined unless the `rhs` iterator and "this" iterator both
        /// iterate over the same collection of Statistics.
        bool operator!=(const ConstIterator& rhs) const;
    };

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(PerformanceMonitor,
                                   bslma::UsesBslmaAllocator);

    // CREATORS

    /// Create an instance of this class to collect performance statistics
    /// on demand (via the `collect` method).  Optionally specify a
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is 0,
    /// the currently installed default allocator is used.
    explicit
    PerformanceMonitor(bslma::Allocator *basicAllocator = 0);

    /// Create an instance of this class that uses the specified `scheduler`
    /// to automatically collect performance statistics every specified
    /// `interval` (specified in seconds).  Optionally specify a
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is 0,
    /// the currently installed default allocator is used.  A non-positive
    /// `interval` value indicates that performance statistics should *not* be
    /// automatically collected--in this case the user is responsible for
    /// manually calling the `collect` function.
    PerformanceMonitor(bdlmt::TimerEventScheduler *scheduler,
                       double                      interval,
                       bslma::Allocator           *basicAllocator = 0);

    /// Destroy this object.
    ~PerformanceMonitor();

    // MANIPULATORS

    /// Register the specified process `pid` having the specified user-defined
    /// `description` with this performance monitor.  After registration,
    /// performance statistics will be collected for the `pid` upon invocation
    /// of the `collect` function.  A `pid` value of zero is translated to the
    /// current process id.  Return 0 on success or a non-zero value otherwise.
    int registerPid(int pid, const bsl::string& description);

    /// Unregister the specified process `pid` from the performance monitor.
    /// After unregistration, to statistics for the `pid` will be no longer be
    /// available unless the `pid` is re-registered with the performance
    /// monitor through calling `registerPid`.  A `pid` value of zero is
    /// translated to the current process id.  Return 0 on success or a
    /// non-zero value otherwise.
    int unregisterPid(int pid);

    /// Set the specified time `interval`, in seconds, after which statistics
    /// for each registered pid are automatically collected.  The behavior is
    /// undefined unless a scheduler was supplied at the construction of this
    /// performance monitor.  A non-positive `interval` value indicates that
    /// performance statistics should not be automatically collected--in this
    /// case the user is responsible for manually calling the `collect`
    /// function.
    void setCollectionInterval(double interval);

    /// Collect performance statistics for each registered pid.
    void collect();

    /// Reset the collected min, max, and average values collected for each
    /// measure for each monitored process.  Note that this method will call
    /// `collect` once for each monitored process, so the statistics after this
    /// method returns will be based on the values collected thereby.
    void resetStatistics();

    // ACCESSORS

    /// Return an iterator positioned at the first set of collected
    /// performance statistics.
    ConstIterator begin() const;

    /// Return an iterator that represents the end of the sequence of sets
    /// of collected performance statistics.
    ConstIterator end() const;

    /// Return the iterator pointing to the set of collected performance
    /// statistics for the specified `pid` if `pid` has been registered with
    /// this performance monitor through the `registerPid` function, otherwise
    /// return `end()`.  A `pid` value of zero is translated to the current
    /// process id.
    ConstIterator find(int pid) const;

    /// Return the number of processes registered for statistics collection.
    int numRegisteredPids() const;
};

#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_CYGWIN)

/// Describes the fields present in /proc/<pid>/stat.  For a complete
/// description of each field, see `man proc`.
///
/// Note that sizes of the data fields are defined in terms of scanf(3)
/// format specifiers, such as %d, %lu or %c.  There is no good way to know
/// if %lu is 32-bit wide or 64-bit, because the code can be built in the
/// -m32 mode making sizeof(unsigned long)==4 and executed on a 64bit
/// platform where the kernel thinks that %lu can represent 64-bit wide
/// integers.  Therefore we use `Uint64` regardless of the build
/// configuration.
struct PerformanceMonitor_LinuxProcStatistics {

    // PUBLIC TYPES
    typedef bsls::Types::Int64  LdType;
    typedef bsls::Types::Uint64 LuType;
    typedef bsls::Types::Uint64 LluType;

    // PUBLIC DATA
    int           d_pid;             // process pid
    bsl::string   d_comm;            // filename of executable
    char          d_state;           // process state
    int           d_ppid;            // process's parent pid
    int           d_pgrp;            // process group id
    int           d_session;         // process session id
    int           d_tty_nr;          // the tty used by the process
    int           d_tpgid;           // tty owner's group id
    unsigned int  d_flags;           // kernel flags
    LuType        d_minflt;          // num minor page faults
    LuType        d_cminflt;         // num minor page faults - children
    LuType        d_majflt;          // num major page faults
    LuType        d_cmajflt;         // num major page faults - children
    LuType        d_utime;           // num jiffies in user mode
    LuType        d_stime;           // num jiffies in kernel mode
    LdType        d_cutime;          // num jiffies, user mode, children
    LdType        d_cstime;          // num jiffies, kernel mode, children
    LdType        d_priority;        // standard nice value, plus fifteen
    LdType        d_nice;            // nice value
    LdType        d_numThreads;      // number of threads (since Linux 2.6)
    LdType        d_itrealvalue;     // num jiffies before next SIGALRM
    LluType       d_starttime;       // time in jiffies since system boot
    LuType        d_vsize;           // virtual memory size, in bytes
    LdType        d_rss;             // resident set size, in pages

    // Note that subsequent fields present in '/proc/<pid>/stat' are not
    // required for any collected measures.

    // CLASS METHOD

    /// For the specified process id `pid`, load the contents of the file
    /// `/proc/<pid>/stat` into the specified `buffer`.  Return 0 on success
    /// and a non-zero value otherwise.
    static int readProcStatString(bsl::string *buffer, int pid);

  private:
    // NOT IMPLEMENTED

  public:
    // CREATORS

    /// Default construct all fields in this object.
    PerformanceMonitor_LinuxProcStatistics();

    // PerformanceMonitor_LinuxProcStatistics(
    //                const PerformanceMonitor_LinuxProcStatistics&) = default;

    // MANIPULATORS
    // PerformanceMonitor_LinuxProcStatistics& operator=(
    //            const PerformanceMonitor_LinuxProcStatistics& rhs) = default;
        // Copy all fields of the specified 'rhs' to this object, and return a
        // reference to it.

    /// Parse the specified `procStatString` and populate all the fields in
    /// this `struct`.  Check that the specified `pid` matches the `pid`
    /// field in the string.  Return 0 on success and a non-zero value
    /// otherwise.
    int parseProcStatString(const bsl::string& procStatString, int pid);
};

#endif

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

               // ---------------------------------------
               // class PerformanceMonitor::ConstIterator
               // ---------------------------------------

// CREATORS
inline
PerformanceMonitor::ConstIterator::ConstIterator()
: d_it()
{
}

inline
PerformanceMonitor::ConstIterator::ConstIterator(
                          PerformanceMonitor::PidMap::const_iterator  it,
                          bslmt::RWMutex                             *mapGuard)
: d_it(it)
, d_mapGuard_p(mapGuard)
{
}

// ACCESSORS
inline
PerformanceMonitor::ConstIterator::reference
PerformanceMonitor::ConstIterator::operator*() const
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(d_mapGuard_p);
    return *d_it->second.first;
}

inline
PerformanceMonitor::ConstIterator::pointer
PerformanceMonitor::ConstIterator::operator->() const
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(d_mapGuard_p);
    return d_it->second.first.get();
}

// MANIPULATORS
inline
PerformanceMonitor::ConstIterator&
PerformanceMonitor::ConstIterator::operator++()
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(d_mapGuard_p);
    ++d_it;
    return *this;
}

inline
PerformanceMonitor::ConstIterator
PerformanceMonitor::ConstIterator::operator++(int)
{
    PerformanceMonitor::ConstIterator temp = *this;
    ++*this;
    return temp;
}

// ACCESSORS
inline
bool PerformanceMonitor::ConstIterator::operator==(
                                                const ConstIterator& rhs) const
{
    return d_it == rhs.d_it;
}

inline
bool PerformanceMonitor::ConstIterator::operator!=(
                                                const ConstIterator& rhs) const
{
    return d_it != rhs.d_it;
}

                 // ------------------------------------
                 // class PerformanceMonitor::Statistics
                 // ------------------------------------

// ACCESSORS
inline
double PerformanceMonitor::Statistics::latestValue(Measure measure) const
{
    BSLS_ASSERT_SAFE(measure >= 0 && measure < e_NUM_MEASURES);

    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_guard);
    return d_lstData[measure];
}

inline
double PerformanceMonitor::Statistics::minValue(Measure measure) const
{
    BSLS_ASSERT_SAFE(measure >= 0 && measure < e_NUM_MEASURES);

    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_guard);
    return d_minData[measure];
}

inline
double PerformanceMonitor::Statistics::maxValue(Measure measure) const
{
    BSLS_ASSERT_SAFE(measure >= 0 && measure < e_NUM_MEASURES);

    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_guard);
    return d_maxData[measure];
}

inline
int PerformanceMonitor::Statistics::pid() const
{
    return d_pid;
}

inline
const bsl::string& PerformanceMonitor::Statistics::description() const
{
    return d_description;
}

inline
double PerformanceMonitor::Statistics::elapsedTime() const
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_guard);
    return d_elapsedTime;
}

inline
const bdlt::Datetime& PerformanceMonitor::Statistics::startupTime() const
{
    return d_startTimeUtc;
}

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
    if (0 == pid) {
        pid = bdls::ProcessUtil::getProcessId();
    }

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
