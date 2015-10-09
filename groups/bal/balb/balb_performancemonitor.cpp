// balb_performancemonitor.cpp                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balb_performancemonitor.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balb_performancemonitor_cpp,"$Id$ $CSID$")

#include <ball_log.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bslmt_writelockguard.h>

#include <bdlt_currenttime.h>
#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>

#include <bsl_algorithm.h>
#include <bsl_cmath.h>
#include <bsl_fstream.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>
#include <bsl_limits.h>
#include <bsl_sstream.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#if defined(BSLS_PLATFORM_OS_SOLARIS)
#include <procfs.h>
#elif defined(BSLS_PLATFORM_OS_AIX)
#include <sys/procfs.h>
#elif defined(BSLS_PLATFORM_OS_FREEBSD)
#include <bsl_string.h>
#elif defined(BSLS_PLATFORM_OS_DARWIN)
#include <libproc.h>
#elif defined(BSLS_PLATFORM_OS_HPUX)
#include <sys/pstat.h>
#include <sys/param.h>
#elif defined(BSLS_PLATFORM_OS_LINUX)
#include <dirent.h>
#include <sys/procfs.h>
#endif
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
#include <windows.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <psapi.h>
#pragma comment(lib, "pdh")
#pragma comment(lib, "psapi")
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#endif

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace {

typedef balb::PerformanceMonitor PM;

const char LOG_CATEGORY[] = "BAEA.PERFORMANCEMONITOR";

enum { INVALID_TIMER_HANDLE = -1 }; // invalid timer event scheduler handle

struct MeasureData {
    int         id;
    const char *tag;
    const char *name;
    const char *units;
    bool        hasAverage;
};

MeasureData s_measureData[PM::e_NUM_MEASURES] = {
  { PM::e_CPU_TIME,
            "CPU_TIME",        "CPU Time",        "s ", false },
  { PM::e_CPU_TIME_USER,
            "CPU_TIME_USER",   "User CPU Time",   "s ", false },
  { PM::e_CPU_TIME_SYSTEM,
            "CPU_TIME_SYSTEM", "System CPU Time", "s ", false },
  { PM::e_CPU_UTIL,
            "CPU_UTIL",        "CPU",             "% ", true  },
  { PM::e_CPU_UTIL_USER,
            "CPU_UTIL_USER",   "User CPU",        "% ", true  },
  { PM::e_CPU_UTIL_SYSTEM,
            "CPU_UTIL_SYSTEM", "System CPU",      "% ", true  },
  { PM::e_RESIDENT_SIZE,
            "RESIDENT_SIZE",   "Resident Size",   "Mb", true  },
  { PM::e_NUM_THREADS,
            "NUM_THREADS",     "Thread Count",    "  ", true  },
  { PM::e_NUM_PAGEFAULTS,
            "NUM_PAGEFAULTS",  "Page Faults",     "  ", false },
  { PM::e_VIRTUAL_SIZE,
            "VIRTUAL_SIZE",    "Virtual Size",    "Mb", true  }
};

bool nearlyEqual(double lhs, double rhs)
    // Return 'true' if the absolute value of the difference between the
    // specified 'lhs' and 'rhs' is less than
    // 'bsl::numeric_limits<double>::episilon'.
{
    return bsl::fabs(lhs - rhs) < bsl::numeric_limits<double>::epsilon();
}

#if defined(BSLS_PLATFORM_OS_UNIX)
int currentProcessPid()
    // Return an integer identifying the current process
{
    return static_cast<int>(getpid());
}
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
int currentProcessPid()
    // Return an integer identifying the current process
{
    return static_cast<int>(GetCurrentProcessId());
}
#endif

}  // close unnamed namespace

// PRIVATE TYPES

#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_CYGWIN)

namespace balb {
template <>
class PerformanceMonitor::Collector<bsls::Platform::OsLinux> {
    // Provide a specialization of the 'Collector' class template for the Linux
    // platform.  SunOS, AIX, and Linux all support the /proc filesystem, from
    // which we extract the performance measures for the monitored pid.  On
    // Linux, the layout and content of this file system differs from the
    // layout and content of the file system on SunOS or AIX, hence this
    // 'Collector' class template specialization.

    // Note that the Linux implementation is stateless.  However, the
    // 'Collector' template requires a constructor accepting a single
    // 'bslma::Allocator' argument.

    // PRIVATE TYPES

    struct ProcStatistics {
        // Describes the fields present in /proc/<pid>/stat.  For a complete
        // description of each field, see 'man proc'.
        //
        // Note that sizes of the data fields are defined in terms of scanf(3)
        // format specifiers, such as %d, %lu or %c.  There is no good way to
        // know if %lu is 32-bit wide or 64-bit, because the code can be built
        // in the -m32 mode making sizeof(unsigned long)==4 and executed on a
        // 64bit platform where the kernel thinks that %lu can represent 64-bit
        // wide integers.  Therefore we use 'Uint64' regardless of the build
        // configuration.

        typedef bsls::Types::Int64  LdType;
        typedef bsls::Types::Uint64 LuType;
        typedef bsls::Types::Uint64 LluType;

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

        ProcStatistics()
        : d_pid()
        , d_comm()
        , d_state()
        , d_ppid()
        , d_pgrp()
        , d_session()
        , d_tty_nr()
        , d_tpgid()
        , d_flags()
        , d_minflt()
        , d_cminflt()
        , d_majflt()
        , d_cmajflt()
        , d_utime()
        , d_stime()
        , d_cutime()
        , d_cstime()
        , d_priority()
        , d_nice()
        , d_numThreads()
        , d_itrealvalue()
        , d_starttime()
        , d_vsize()
        , d_rss()
        {
        }

        // Note that subsequent fields present in '/proc/<pid>/stat' are not
        // required for any collected measures.
    };

    int readProcStat(ProcStatistics *stats, int pid);
        // Load into the specified 'stats' result the fields present for the
        // specified 'pid' in the '/proc/<pid>/stat' virtual file.  Return 0 on
        // success or a non-zero value otherwise.

    // UNIMPLEMENTED
    Collector(const Collector &);             // = deleted
    Collector& operator=(const Collector &);  // = deleted

  public:
    // CREATORS
    Collector(bslma::Allocator *basicAllocator = 0);
        // Create an instance of this class.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    // METHODS

    int initialize(PerformanceMonitor::Statistics *stats,
                   int                             pid,
                   const bsl::string&              description);
        // Initialize the specified 'stats' to represent the specified 'pid'
        // having the specified user-defined 'description'.  Return 0 on
        // success or a non-zero value otherwise.

    int collect(PerformanceMonitor::Statistics *stats);
        // Load into the specified 'stats' the performance statistics collected
        // for the pid associated with 'stats'.  Return 0 on success or a
        // non-zero value otherwise.
};
}  // close package namespace

int balb::PerformanceMonitor::Collector<bsls::Platform::OsLinux>
::readProcStat(ProcStatistics *stats, int pid)
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    bsl::stringstream filename;
    filename << "/proc/" << pid << "/stat";

    bsl::ifstream ifs(filename.str().c_str());
    if (!ifs) {
        BALL_LOG_DEBUG << "Failed to open '" << filename.str() << "'"
                       << BALL_LOG_END;
        return -1;                                                    // RETURN
    }

    ifs >> stats->d_pid
        >> stats->d_comm
        >> stats->d_state
        >> stats->d_ppid
        >> stats->d_pgrp
        >> stats->d_session
        >> stats->d_tty_nr
        >> stats->d_tpgid
        >> stats->d_flags
        >> stats->d_minflt
        >> stats->d_cminflt
        >> stats->d_majflt
        >> stats->d_cmajflt
        >> stats->d_utime
        >> stats->d_stime
        >> stats->d_cutime
        >> stats->d_cstime
        >> stats->d_priority
        >> stats->d_nice
        >> stats->d_numThreads
        >> stats->d_itrealvalue
        >> stats->d_starttime
        >> stats->d_vsize
        >> stats->d_rss;

    return 0;
}

balb::PerformanceMonitor::Collector<bsls::Platform::OsLinux>
::Collector(bslma::Allocator *)
{
}

int balb::PerformanceMonitor::Collector<bsls::Platform::OsLinux>
::initialize(Statistics *stats, int pid, const bsl::string& description)
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    int rc;

    stats->d_pid         = pid;
    stats->d_description = description;

    ProcStatistics procStats;
    if (0 != (rc = readProcStat(&procStats, pid))) {
        BALL_LOG_DEBUG << "Failed to open /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << "), rc = " << rc
                       << BALL_LOG_END;
        return -1;                                                    // RETURN
    }

    static bsls::Types::Int64 bootTime = -1;
    if (bootTime < 0) {
        bsl::string line;

        bsl::ifstream bootFile("/proc/stat");

        while (getline(bootFile, line)) {
            if (line.length() > 0) {
                bsl::stringstream ss(line);
                bsl::string s;
                ss >> s;
                if ("btime" == s) {
                    if (ss >> bootTime) {
                        break;
                    }
                    else {
                        return -1;                                    // RETURN
                    }
                }
            }
        }

        if (-1 == bootTime) {
            // 'btime' not found

            return -1;                                                // RETURN
        }
    }

    int jiffiesPerSec = sysconf(_SC_CLK_TCK);
    bsls::Types::Int64 procStartTime =
                              bootTime + procStats.d_starttime / jiffiesPerSec;
                                                      // seconds since 1970 UTC

    stats->d_startTime = bsls::TimeInterval(procStartTime, 0);
    stats->d_startTimeUtc = bdlt::EpochUtil::epoch();
    stats->d_startTimeUtc.addSeconds(procStartTime);

    BALL_LOG_DEBUG << "PID " << stats->d_pid << " started approximately "
                   << stats->d_startTimeUtc << " (UTC)"
                   << BALL_LOG_END;

    return 0;
}

int balb::PerformanceMonitor::Collector<bsls::Platform::OsLinux>
::collect(Statistics *stats)
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&stats->d_guard);

    ProcStatistics procStats;
    if (0 != readProcStat(&procStats, stats->d_pid)) {
        BALL_LOG_DEBUG << "Failed to open /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BALL_LOG_END;
        return -1;                                                    // RETURN
    }

    // Discover the duration of a jiffy.

    static const int clockTicksPerSec = sysconf(_SC_CLK_TCK);

    // Discover the number of threads by enumerating the directories in the
    // /proc/<pid>/task directory.

    bsl::stringstream taskFilename;
    taskFilename << "/proc/" << stats->d_pid << "/task";
    dirent **entry = 0;
    int numThreads = 0;
    int numNodes = scandir(taskFilename.str().c_str(), &entry, 0, 0);
    for (int nodeIndex = 0; nodeIndex < numNodes; ++nodeIndex) {
        if (DT_DIR == entry[nodeIndex]->d_type) {
            ++numThreads;
        }
        bsl::free(entry[nodeIndex]);
    }
    bsl::free(entry);

    stats->d_lstData[e_NUM_THREADS] = numThreads;

    static const int pageSize = sysconf(_SC_PAGESIZE);

    stats->d_lstData[e_RESIDENT_SIZE] =
                        static_cast<double>(procStats.d_rss) * pageSize /
                        (1024 * 1024);

    stats->d_lstData[e_VIRTUAL_SIZE] =
                        static_cast<double>(procStats.d_vsize) / (1024 * 1024);

    double cpuTimeU = static_cast<double>(procStats.d_utime) /
                      clockTicksPerSec;
    double cpuTimeS = static_cast<double>(procStats.d_stime) /
                      clockTicksPerSec;

    // Calculate CPU utilization

    double deltaCpuTimeU = cpuTimeU - stats->d_lstData[e_CPU_TIME_USER];
    double deltaCpuTimeS = cpuTimeS - stats->d_lstData[e_CPU_TIME_SYSTEM];

    double elapsedTime = (bdlt::CurrentTime::now() - stats->d_startTime).
                                                        totalSecondsAsDouble();

    double dt = elapsedTime - stats->d_elapsedTime;

    double deltaCpuTimeT = deltaCpuTimeU + deltaCpuTimeS;

    if ((stats->d_numSamples != 0) && (dt > 0)) {
        stats->d_lstData[e_CPU_UTIL]        = deltaCpuTimeT / dt * 100.0;
        stats->d_lstData[e_CPU_UTIL_USER]   = deltaCpuTimeU / dt * 100.0;
        stats->d_lstData[e_CPU_UTIL_SYSTEM] = deltaCpuTimeS / dt * 100.0;
    }
    else {
        stats->d_lstData[e_CPU_UTIL]        = 0;
        stats->d_lstData[e_CPU_UTIL_USER]   = 0;
        stats->d_lstData[e_CPU_UTIL_SYSTEM] = 0;
    }

    stats->d_lstData[e_CPU_TIME_USER]   = cpuTimeU;
    stats->d_lstData[e_CPU_TIME_SYSTEM] = cpuTimeS;
    stats->d_lstData[e_CPU_TIME]        = cpuTimeU + cpuTimeS;

    stats->d_elapsedTime = elapsedTime;

    ++stats->d_numSamples;

    // Calculate averages for each measure

    for (int i = 0; i < e_NUM_MEASURES; ++i) {
        if (s_measureData[i].hasAverage) {

            stats->d_minData[i]  = bsl::min(stats->d_minData[i],
                                            stats->d_lstData[i]);

            stats->d_maxData[i]  = bsl::max(stats->d_maxData[i],
                                            stats->d_lstData[i]);

            stats->d_totData[i] += stats->d_lstData[i];
        }
    }

    BALL_LOG_TRACE << "Collected performance measures for PID "
                   << stats->d_pid << " (" << stats->d_description << ")"
                   << BALL_LOG_END;

    return 0;
}

#elif defined(BSLS_PLATFORM_OS_FREEBSD)

namespace balb {
template <>
class PerformanceMonitor::Collector<bsls::Platform::OsFreeBsd> {
    // Provide a specialization of the 'Collector' class template for the Linux
    // platform.  SunOS, AIX, and Linux all support the /proc filesystem, from
    // which we extract the performance measures for the monitored pid.  On
    // Linux, the layout and content of this file system differs from the
    // layout and content of the file system on SunOS or AIX, hence this
    // 'Collector' class template specialization.

    // Note that the FreeBSD implementation is stateless.  However, the
    // 'Collector' template requires a constructor accepting a single
    // 'bslma::Allocator' argument.

    // PRIVATE TYPES

    struct ProcStatistics {
        // Describes the fields present in /proc/<pid>/status.  For a complete
        // description of each field, see 'man procfs'.

        bsl::string   d_comm;             // filename of executable
        int           d_pid;              // process pid
        int           d_ppid;             // process's parent pid
        int           d_pgid;             // process group id
        int           d_session;          // process session id
        bsl::string   d_terminal;         // controlling terminal
        bsl::string   d_flags;            // flags

        // these 'long's have not been converted to 'int's because we are
        // unable to build on freebsd to test the results.

        unsigned long d_processStartSecs; // process start time secs
        unsigned long d_processStartMSecs;// process start time microsecs
        unsigned long d_userTimeSecs;     // user start time secs
        unsigned long d_userTimeMSecs;    // user start time microsecs
        unsigned long d_sysTimeSecs;      // sys start time secs
        unsigned long d_sysTimeMSecs;     // sys start time microsecs

        ProcStatistics()
        : d_comm()
        , d_pid()
        , d_ppid()
        , d_pgid()
        , d_session()
        , d_terminal()
        , d_flags()
        , d_processStartSecs()
        , d_processStartMSecs()
        , d_userTimeSecs()
        , d_userTimeMSecs()
        , d_sysTimeSecs()
        , d_sysTimeMSecs()
        {
        }

        // Note that subsequent fields present in '/proc/<pid>/status' are not
        // required for any collected measures.
    };

    int readProcStat(ProcStatistics *stats, int pid);
        // Load into the specified 'stats' result the fields present in the
        // '/proc/<pid>/status' virtual file for the specified 'pid'.  Return 0
        // on success or a non-zero value otherwise.

  public:
    // CREATORS
    Collector(bslma::Allocator *basicAllocator = 0);
        // Create an instance of this class.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    // METHODS

    int initialize(Statistics *stats, int pid, const bsl::string &description);
        // Initialize the specified 'stats' to represent the specified 'pid'
        // having the specified user-defined 'description'.  Return 0 on
        // success or a non-zero value otherwise.

    int collect(Statistics *stats);
        // Load into the specified 'stats' the performance statistics collected
        // for the pid associated with 'stats'.  Return 0 on success or a
        // non-zero value otherwise.
};
}  // close package namespace

int balb::PerformanceMonitor::Collector<bsls::Platform::OsFreeBsd>
::readProcStat(ProcStatistics *stats, int pid)
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    bsl::stringstream filename;
    filename << "/proc/" << pid << "/status";

    bsl::ifstream ifs(filename.str().c_str());
    if (!ifs) {
        BALL_LOG_DEBUG << "Failed to open '" << filename.str() << "'"
                       << BALL_LOG_END;
        return -1;
    }

    bsl::string str((bsl::istreambuf_iterator<char>(ifs)),
                     bsl::istreambuf_iterator<char>());

    bsl::stringstream ss(str);
    ss >> stats->d_comm;
    ss >> stats->d_pid;
    ss >> stats->d_ppid;
    ss >> stats->d_pgid;
    ss >> stats->d_session;
    ss >> stats->d_terminal;
    ss >> stats->d_flags;
    ss >> stats->d_processStartSecs;
    char sep;
    ss >> sep;
    ss >> stats->d_processStartMSecs;
    ss >> stats->d_userTimeSecs;
    ss >> sep;
    ss >> stats->d_userTimeMSecs;
    ss >> stats->d_sysTimeSecs;
    ss >> sep;
    ss >> stats->d_sysTimeMSecs;

    return 0;
}

balb::PerformanceMonitor::Collector<bsls::Platform::OsFreeBsd>
::Collector(bslma::Allocator *)
{
}

int balb::PerformanceMonitor::Collector<bsls::Platform::OsFreeBsd>
::initialize(Statistics *stats, int pid, const bsl::string& description)
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    int rc;

    stats->d_pid         = pid;
    stats->d_description = description;

    ProcStatistics procStats;
    if (0 != (rc = readProcStat(&procStats, pid))) {
        BALL_LOG_DEBUG << "Failed to open /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << "), rc = " << rc
                       << BALL_LOG_END;
        return -1;
    }

    //  bsls::TimeInterval startTime(procStats.d_processStartSecs,
    //                               procStats.d_processStartMSecs * 1000);

    stats->d_startTime = bsls::TimeInterval(procStats.d_processStartSecs, 0);
    stats->d_startTimeUtc = bdlt::EpochUtil::epoch();
    stats->d_startTimeUtc.addSeconds(stats->d_startTime.seconds());

    BALL_LOG_DEBUG << "PID " << stats->d_pid << " started approximately "
                   << stats->d_startTimeUtc << " (UTC)"
                   << BALL_LOG_END;

    return 0;

}

int balb::PerformanceMonitor::Collector<bsls::Platform::OsFreeBsd>
::collect(Statistics *stats)
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&stats->d_guard);

    ProcStatistics procStats;
    if (0 != readProcStat(&procStats, stats->d_pid)) {
        BALL_LOG_DEBUG << "Failed to open /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BALL_LOG_END;
        return -1;
    }

    stats->d_lstData[e_NUM_THREADS]   = 0;

    stats->d_lstData[e_RESIDENT_SIZE] = 0;
    stats->d_lstData[e_VIRTUAL_SIZE]  = 0;

    double cpuTimeU = static_cast<double>(procStats.d_userTimeSecs) +
                      static_cast<double>(procStats.d_userTimeMSecs) /
                      static_cast<double>(1000000);
    double cpuTimeS = static_cast<double>(procStats.d_sysTimeSecs) +
                      static_cast<double>(procStats.d_sysTimeMSecs) /
                      static_cast<double>(1000000);
    // Calculate CPU utilization

    double deltaCpuTimeU = cpuTimeU - stats->d_lstData[e_CPU_TIME_USER];
    double deltaCpuTimeS = cpuTimeS - stats->d_lstData[e_CPU_TIME_SYSTEM];

    // Inaccurate for the first sample but this doesn't affect our results,
    // as the CPU utilization is not calculated until the second sample is
    // taken.
    double elapsedTime = bdlt::CurrentTime::now().totalSecondsAsDouble();

    double dt = elapsedTime - stats->d_elapsedTime;

    if ((stats->d_numSamples != 0) && (dt > 0)) {
        stats->d_lstData[e_CPU_UTIL] = (deltaCpuTimeU + deltaCpuTimeS)
                                   / dt
                                   * 100.0;

        stats->d_lstData[e_CPU_UTIL_USER]   = deltaCpuTimeU / dt * 100.0;
        stats->d_lstData[e_CPU_UTIL_SYSTEM] = deltaCpuTimeS / dt * 100.0;
    }
    else {
        stats->d_lstData[e_CPU_UTIL]        = 0.0;
        stats->d_lstData[e_CPU_UTIL_USER]   = 0.0;
        stats->d_lstData[e_CPU_UTIL_SYSTEM] = 0.0;
    }

    stats->d_lstData[e_CPU_TIME_USER]   = cpuTimeU;
    stats->d_lstData[e_CPU_TIME_SYSTEM] = cpuTimeS;
    stats->d_lstData[e_CPU_TIME]        = cpuTimeU + cpuTimeS;

    stats->d_elapsedTime = elapsedTime;

    ++stats->d_numSamples;

    // Calculate averages for each measure

    for (int i = 0; i < e_NUM_MEASURES; ++i) {
        if (s_measureData[i].hasAverage) {

            stats->d_minData[i]  = bsl::min(stats->d_minData[i],
                                            stats->d_lstData[i]);

            stats->d_maxData[i]  = bsl::max(stats->d_maxData[i],
                                            stats->d_lstData[i]);

            stats->d_totData[i] += stats->d_lstData[i];
        }
    }

    BALL_LOG_TRACE << "Collected performance measures for PID "
                   << stats->d_pid << " (" << stats->d_description << ")"
                   << BALL_LOG_END;

    return 0;
}

#elif defined(BSLS_PLATFORM_OS_DARWIN)

namespace balb {
template <>
class PerformanceMonitor::Collector<bsls::Platform::OsDarwin> {
    // Provide a specialization of the 'Collector' class template for the
    // Darwin platform.

  public:
    // CREATORS
    Collector(bslma::Allocator *basicAllocator = 0);
        // Create an instance of this class.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    // MANIPULATORS
    int initialize(Statistics *stats, int pid, const bsl::string &description);
        // Initialize the specified 'stats' to represent the specified 'pid'
        // having the specified user-defined 'description'.  Return 0 on
        // success or a non-zero value otherwise.

    int collect(Statistics *stats);
        // Load into the specified 'stats' the performance statistics collected
        // for the pid associated with 'stats'.  Return 0 on success or a
        // non-zero value otherwise.
};
}  // close package namespace

// CREATORS
balb::PerformanceMonitor::Collector<bsls::Platform::OsDarwin>
::Collector(bslma::Allocator *)
{
}

// MANIPULATORS
int balb::PerformanceMonitor::Collector<bsls::Platform::OsDarwin>
::initialize(Statistics *stats, int pid, const bsl::string &description)
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    stats->d_pid         = pid;
    stats->d_description = description;

    proc_bsdinfo bi;

    int nb = proc_pidinfo(pid,
                          PROC_PIDTBSDINFO,
                          0,
                          &bi,
                          PROC_PIDTBSDINFO_SIZE);
    if (PROC_PIDTBSDINFO_SIZE != nb) {
        BALL_LOG_DEBUG << "Failed to call proc_pidinfo, nb = " << nb
                       << ", expected = " << PROC_PIDTBSDINFO_SIZE
                       << BALL_LOG_END;
        return -1;                                                    // RETURN
    }

    stats->d_startTime = bsls::TimeInterval(bi.pbi_start_tvsec,
                                            bi.pbi_start_tvusec * 1000);

    stats->d_startTimeUtc = bdlt::EpochUtil::epoch();
    stats->d_startTimeUtc.addSeconds(stats->d_startTime.seconds());
    stats->d_startTimeUtc.addMilliseconds(
                                   stats->d_startTime.nanoseconds() / 1000000);

    BALL_LOG_DEBUG << "PID " << stats->d_pid << " started approximately "
                   << stats->d_startTimeUtc << " (UTC)"
                   << BALL_LOG_END;

    return 0;
}

int balb::PerformanceMonitor::Collector<bsls::Platform::OsDarwin>
::collect(Statistics *stats)
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&stats->d_guard);

    proc_taskinfo ti;

    int nb = proc_pidinfo(stats->d_pid,
                          PROC_PIDTASKINFO,
                          0,
                          &ti,
                          PROC_PIDTASKINFO_SIZE);
    if (PROC_PIDTASKINFO_SIZE != nb) {
        BALL_LOG_DEBUG << "Failed to call proc_pidinfo, nb = " << nb
                       << ", expected = " << PROC_PIDTASKINFO_SIZE
                       << BALL_LOG_END;
        return -1;                                                    // RETURN
    }

    stats->d_lstData[e_NUM_THREADS]   = ti.pti_threadnum;
    stats->d_lstData[e_RESIDENT_SIZE] = ti.pti_resident_size / 1048576.0;
    stats->d_lstData[e_VIRTUAL_SIZE]  = ti.pti_virtual_size  / 1048576.0;

    const double cpuTimeU = ti.pti_total_user   / 1e9;
    const double cpuTimeS = ti.pti_total_system / 1e9;
    const double cpuTime  = cpuTimeU + cpuTimeS;

    const double deltaCpuTimeU =
                               cpuTimeU - stats->d_lstData[e_CPU_TIME_USER];
    const double deltaCpuTimeS =
                             cpuTimeS - stats->d_lstData[e_CPU_TIME_SYSTEM];
    const double deltaCpuTime  = deltaCpuTimeU + deltaCpuTimeS;

    // Inaccurate for the first sample, but this doesn't affect our results as
    // the CPU utilization is not calculated until the second sample is taken.
    const double elapsedTime = bdlt::CurrentTime::now().totalSecondsAsDouble();
    const double dt          = elapsedTime - stats->d_elapsedTime;

    if ((stats->d_numSamples != 0) && (dt > 0)) {
        stats->d_lstData[e_CPU_UTIL]        = deltaCpuTime  / dt * 100.0;
        stats->d_lstData[e_CPU_UTIL_USER]   = deltaCpuTimeU / dt * 100.0;
        stats->d_lstData[e_CPU_UTIL_SYSTEM] = deltaCpuTimeS / dt * 100.0;
    }
    else {
        stats->d_lstData[e_CPU_UTIL]        = 0.0;
        stats->d_lstData[e_CPU_UTIL_USER]   = 0.0;
        stats->d_lstData[e_CPU_UTIL_SYSTEM] = 0.0;
    }

    stats->d_lstData[e_CPU_TIME]        = cpuTime;
    stats->d_lstData[e_CPU_TIME_USER]   = cpuTimeU;
    stats->d_lstData[e_CPU_TIME_SYSTEM] = cpuTimeS;

    stats->d_elapsedTime                   = elapsedTime;

    ++stats->d_numSamples;

    // Calculate averages for each measure

    for (int i = 0; i < e_NUM_MEASURES; ++i) {
        if (s_measureData[i].hasAverage) {
            stats->d_minData[i]  = bsl::min(stats->d_minData[i],
                                            stats->d_lstData[i]);
            stats->d_maxData[i]  = bsl::max(stats->d_maxData[i],
                                            stats->d_lstData[i]);
            stats->d_totData[i] += stats->d_lstData[i];
        }
    }

    BALL_LOG_TRACE << "Collected performance measures for PID "
                   << stats->d_pid << " (" << stats->d_description << ")"
                   << BALL_LOG_END;

    return 0;
}

#elif defined(BSLS_PLATFORM_OS_UNIX)

namespace balb {
template <>
class PerformanceMonitor::Collector<bsls::Platform::OsUnix> {
    // Provide a specialization of the 'Collector' class template for
    // UNIX-based platforms.  SunOS, AIX, and Linux all support the /proc
    // filesystem, from which we extract the performance measures for the
    // monitored pid.

    // Note that the UNIX-based implementation is stateless.  However, the
    // 'Collector' template requires a constructor accepting a single
    // 'bslma::Allocator' argument.

  public:
    // CREATORS
    Collector(bslma::Allocator *basicAllocator = 0);
        // Create an instance of this class.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    // METHODS

    int initialize(Statistics *stats, int pid, const bsl::string &description);
        // Initialize the specified 'stats' to represent the specified 'pid'
        // having the specified user-defined 'description'.  Return 0 on
        // success or a non-zero value otherwise.

    int collect(Statistics *stats);
        // Load into the specified 'stats' the performance statistics collected
        // for the pid associated with 'stats'.  Return 0 on success or a
        // non-zero value otherwise.
};
}  // close package namespace

balb::PerformanceMonitor::Collector<bsls::Platform::OsUnix>
::Collector(bslma::Allocator *)
{
}

int balb::PerformanceMonitor::Collector<bsls::Platform::OsUnix>
::initialize(Statistics *stats, int pid, const bsl::string &description)
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    stats->d_pid         = pid;
    stats->d_description = description;

#ifndef BSLS_PLATFORM_OS_HPUX
    bsl::stringstream procfsInfo;
    procfsInfo << "/proc/" << stats->d_pid << "/psinfo";

    int fd = open(procfsInfo.str().c_str(), O_RDONLY);
    if (fd == -1) {
        BALL_LOG_DEBUG << "Failed to open /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BALL_LOG_END;
        return -1;
    }
    psinfo_t info;
    if (sizeof info != read(fd, &info, sizeof info)) {
        BALL_LOG_DEBUG << "Failed to read /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BALL_LOG_END;
        return -1;
    }

    stats->d_startTime = bsls::TimeInterval(info.pr_start.tv_sec,
                                            info.pr_start.tv_nsec);

    close(fd);
#else
    pst_status status;
    int rc = pstat_getproc(&status, sizeof status, 0, stats->d_pid);
    if (-1 == rc)
    {
        BALL_LOG_DEBUG << "Failed to read /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BALL_LOG_END;
        return -1;
    }

    // TBD only seconds on HPUX??
    stats->d_startTime = bsls::TimeInterval(status.pst_start, 0);
#endif

    stats->d_startTimeUtc = bdlt::EpochUtil::epoch();
    stats->d_startTimeUtc.addSeconds(stats->d_startTime.seconds());

    BALL_LOG_DEBUG << "PID " << stats->d_pid << " started approximately "
                   << stats->d_startTimeUtc << " (UTC)"
                   << BALL_LOG_END;

    return 0;
}

int balb::PerformanceMonitor::Collector<bsls::Platform::OsUnix>
::collect(Statistics *stats)
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&stats->d_guard);

    int numThreads;
    double cpuTimeU;
    double cpuTimeS;

#ifndef BSLS_PLATFORM_OS_HPUX
    int fd;

    bsl::stringstream procfsInfo;
    procfsInfo << "/proc/" << stats->d_pid << "/psinfo";

    fd = open(procfsInfo.str().c_str(), O_RDONLY);
    if (fd == -1) {
        BALL_LOG_DEBUG << "Failed to open /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BALL_LOG_END;
        return -1;
    }

    psinfo_t info;
    if (sizeof info != read(fd, &info, sizeof info)) {
        BALL_LOG_DEBUG << "Failed to read /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BALL_LOG_END;
        return -1;
    }

    numThreads = info.pr_nlwp;

    stats->d_lstData[e_NUM_THREADS]   = static_cast<double>(numThreads);
    stats->d_lstData[e_RESIDENT_SIZE] = static_cast<double>(info.pr_rssize)
                                           / 1024; // in Kb

    close(fd);

    bsl::stringstream procfsStatus;
    procfsStatus << "/proc/" << stats->d_pid << "/status";

    fd = open(procfsStatus.str().c_str(), O_RDONLY);
    if (fd == -1) {
        BALL_LOG_DEBUG << "Failed to open /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BALL_LOG_END;
        return -1;
    }

    pstatus_t status;
    if (sizeof status != read(fd, &status, sizeof status)) {
        BALL_LOG_DEBUG << "Failed to read /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BALL_LOG_END;
        return -1;
    }

    stats->d_lstData[e_VIRTUAL_SIZE]  =
        static_cast<double>(status.pr_brksize) / (1024 * 1024);

    cpuTimeU = bsls::TimeInterval(static_cast<double>(status.pr_utime.tv_sec),
                                 static_cast<double>(status.pr_utime.tv_nsec)).
                                                     totalSecondsAsDouble();

    cpuTimeS = bsls::TimeInterval(static_cast<double>(status.pr_stime.tv_sec),
                                 static_cast<double>(status.pr_stime.tv_nsec)).
                                                     totalSecondsAsDouble();

    close(fd);
#else
    int rc;

    struct pst_static pstatic;
    rc = pstat_getstatic(&pstatic, sizeof pstatic, 1, 0);
    if (-1 == rc) {
        BALL_LOG_DEBUG << "Failed to call 'pstat_getstatic' for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BALL_LOG_END;
        return -1;
    }

    pst_status status;
    rc = pstat_getproc(&status, sizeof status, 0, stats->d_pid);
    if (-1 == rc)
    {
        BALL_LOG_DEBUG << "Failed to call 'pstat_getproc' for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BALL_LOG_END;
        return -1;
    }
    numThreads = status.pst_nlwps;

    stats->d_lstData[e_NUM_THREADS]   = static_cast<double>(numThreads);
    stats->d_lstData[e_RESIDENT_SIZE] =
                  static_cast<double>(status.pst_rssize) * pstatic.page_size
                   / (1024 * 1024);

#if defined(BAEA_PERFORMANCE_MONITOR_DEBUG)
    BALL_LOG_TRACE<< "\nreal data                  = "
                  << static_cast<double>((status.pst_dsize) *
                                         pstatic.page_size) / (1024 * 1024)
                  << "\nreal text                  = "
                  << static_cast<double>((status.pst_tsize) *
                                         pstatic.page_size) / (1024 * 1024)
                  << "\nreal stack                 = "
                  << static_cast<double>((status.pst_ssize)
                                         * pstatic.page_size) / (1024 * 1024)
                  << "\nreal shared memory         = "
                  << static_cast<double>((status.pst_shmsize) *
                                         pstatic.page_size) / (1024 * 1024)
                  << "\nreal memory mapped files   = "
                  << static_cast<double>((status.pst_mmsize)
                                         * pstatic.page_size) / (1024 * 1024)
                  << "\nreal U-area                = "
                  << static_cast<double>((status.pst_usize)
                                         * pstatic.page_size) / (1024 * 1024)
                  << "\nreal device mapping        = "
                  << static_cast<double>((status.pst_iosize)
                                         * pstatic.page_size) / (1024 * 1024)
                  << "\nvirt data                  = "
                  << static_cast<double>((status.pst_vdsize)
                                         * pstatic.page_size) / (1024 * 1024)
                  << "\nvirt text                  = "
                  << static_cast<double>((status.pst_vtsize)
                                         * pstatic.page_size) / (1024 * 1024)
                  << "\nvirt stack                 = "
                  << static_cast<double>((status.pst_vssize)
                                         * pstatic.page_size) / (1024 * 1024)
                  << "\nvirt shared memory         = "
                  << static_cast<double>((status.pst_vshmsize)
                                         * pstatic.page_size) / (1024 * 1024)
                  << "\nvirt memory mapped files   = "
                  << static_cast<double>((status.pst_vmmsize)
                                         * pstatic.page_size) / (1024 * 1024)
                  << "\nvirt U-area                = "
                  << static_cast<double>((status.pst_vusize)
                                         * pstatic.page_size) / (1024 * 1024)
                  << "\nvirt device mapping        = "
                  << static_cast<double>((status.pst_viosize)
                                         * pstatic.page_size) / (1024 * 1024)
                  << BALL_LOG_END;
#endif

    stats->d_lstData[e_VIRTUAL_SIZE]  =
                     (static_cast<double>(status.pst_vdsize *
                                          pstatic.page_size) / (1024 * 1024);

    // TBD -- only seconds resolution??
    cpuTimeU = bsls::TimeInterval(static_cast<double>(status.pst_utime,) 0).
                                                        totalSecondsAsDouble();

    cpuTimeS = bsls::TimeInterval(static_cast<double>(status.pst_stime,) 0).
                                                        totalSecondsAsDouble();
#endif

    // Calculate CPU utilization

    double deltaCpuTimeU = cpuTimeU - stats->d_lstData[e_CPU_TIME_USER];
    double deltaCpuTimeS = cpuTimeS - stats->d_lstData[e_CPU_TIME_SYSTEM];

    double elapsedTime = (bdlt::CurrentTime::now() - stats->d_startTime).
                                                        totalSecondsAsDouble();

    double dt = elapsedTime - stats->d_elapsedTime;

    if ((stats->d_numSamples != 0) && (dt > 0)) {
        stats->d_lstData[e_CPU_UTIL] = (deltaCpuTimeU + deltaCpuTimeS)
                                   / dt
                                   * 100.0;

        stats->d_lstData[e_CPU_UTIL_USER]   = deltaCpuTimeU / dt * 100.0;
        stats->d_lstData[e_CPU_UTIL_SYSTEM] = deltaCpuTimeS / dt * 100.0;
    }
    else {
        stats->d_lstData[e_CPU_UTIL]        = 0.0;
        stats->d_lstData[e_CPU_UTIL_USER]   = 0.0;
        stats->d_lstData[e_CPU_UTIL_SYSTEM] = 0.0;
    }

    if (stats->d_lstData[e_CPU_UTIL] > numThreads * 100.0)
    {
        BALL_LOG_DEBUG << "Calculated impossible CPU utilization = "
                       << stats->d_lstData[e_CPU_UTIL]
                       << ", num threads = " << numThreads
                       << BALL_LOG_END;

        stats->d_lstData[e_CPU_UTIL] = 0.0;
        stats->d_lstData[e_CPU_UTIL_USER] = 0.0;
        stats->d_lstData[e_CPU_UTIL_SYSTEM] = 0.0;
    }

    stats->d_lstData[e_CPU_TIME_USER]   = cpuTimeU;
    stats->d_lstData[e_CPU_TIME_SYSTEM] = cpuTimeS;
    stats->d_lstData[e_CPU_TIME]        = cpuTimeU + cpuTimeS;

    stats->d_elapsedTime = elapsedTime;

    ++stats->d_numSamples;

    // Calculate averages for each measure

    for (int i = 0; i < e_NUM_MEASURES; ++i) {
        if (s_measureData[i].hasAverage) {

            stats->d_minData[i]  = bsl::min(stats->d_minData[i],
                                           stats->d_lstData[i]);

            stats->d_maxData[i]  = bsl::max(stats->d_maxData[i],
                                           stats->d_lstData[i]);

            stats->d_totData[i] += stats->d_lstData[i];
        }
    }

    BALL_LOG_TRACE << "Collected performance measures for PID "
                   << stats->d_pid << " (" << stats->d_description << ")"
                   << BALL_LOG_END;

    return 0;
}

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

namespace balb {
template <>
struct PerformanceMonitor::Collector<bsls::Platform::OsWindows> {
    // Provide a specialization of the 'Collector' class template for
    // Windows-based platforms.  These platforms expose performance statistics
    // through the Performance Data Helper (PDH) API.

    // PRIVATE CONSTANTS

    enum {
        // Enumerates the counters collected from the Windows PDH API.

        PROCESSOR_TIME,
        USER_PROCESSOR_TIME,
        THREAD_COUNT,
        WORKING_SET,
        VIRTUAL_SIZE,
        PAGEFAULTS_PER_SEC,
        ELAPSED_TIME,
        COUNTER_MAX
    };

    // INSTANCE DATA

    PDH_HQUERY                d_instanceQuery; // query to discover instance
    PDH_HQUERY                d_measureQuery;  // query to collect measures
    int                       d_instanceIndex; // process instance index
    bsl::vector<PDH_HCOUNTER> d_counters;      // array of collected measures

    // not implemented
    Collector(const Collector& other);
    Collector& operator=(const Collector&);

    // PRIVATE CLASS METHODS

    static bsl::string findModuleName(int pid);
        // Return the module name of the process with the specified 'pid',
        // without any trailing file extension.

    static int findInstanceIndexFromPid(PDH_HQUERY         query,
                                        const bsl::string& moduleName,
                                        int                pid);
        // Return the performance counter instance index for the "Process"
        // object for the specified 'pid' (having the specified 'moduleName')
        // using the specified 'hQuery' handle, or -1 if no such instance
        // index can be found.

    static int rebindCounters(bsl::vector<PDH_HCOUNTER> *counters,
                              PDH_HQUERY                 query,
                              const char                *name,
                              int                        instanceIndex);
        // Bind the specified 'counters' to the specified 'query' for the
        // specified process 'name' and 'instanceIndex'.  Return 0 on success
        // or a non-zero value otherwise.

  public:
    // CREATORS
    Collector(bslma::Allocator *basicAllocator = 0);
        // Create an instance of this class.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~Collector();
        // Destroy an instance of this class.

    // METHODS

    int initialize(Statistics *stats, int pid, const bsl::string &description);
        // Initialize the specified 'stats' to represent the specified 'pid'
        // having the specified user-defined 'description'.  Return 0 on
        // success or a non-zero value otherwise.

    int collect(Statistics *stats);
        // Load into the specified 'stats' the performance statistics collected
        // for the pid associated with 'stats'.  Return 0 on success or a
        // non-zero value otherwise.
};
}  // close package namespace

bsl::string balb::PerformanceMonitor::Collector<bsls::Platform::OsWindows>
::findModuleName(int pid)
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    DWORD accessRights = STANDARD_RIGHTS_READ
                       | PROCESS_QUERY_INFORMATION
                       | PROCESS_VM_READ;

    HANDLE process = OpenProcess(accessRights, FALSE, pid);

    if (0 == process) {
        BALL_LOG_ERROR << "Failed to open process handle to pid " << pid
                       << BALL_LOG_END;
        return "";
    }

    char moduleExeName[MAX_PATH];
    GetModuleBaseName(process, 0, moduleExeName, MAX_PATH);

    CloseHandle(process);

    const char *extension = bsl::strrchr(moduleExeName, '.');
    if (0 == extension)
        return bsl::string(moduleExeName);

    return bsl::string((const char*)moduleExeName, extension);
}

int balb::PerformanceMonitor::Collector<bsls::Platform::OsWindows>
::findInstanceIndexFromPid(PDH_HQUERY         query,
                           const bsl::string& moduleName,
                           int                pid)
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    PDH_STATUS rc;

    for (int instanceIndex = 0; true; ++instanceIndex)
    {
        PDH_COUNTER_PATH_ELEMENTS cpe = {
            0,
            "Process",
            (LPSTR) moduleName.c_str(),
            0,
            instanceIndex,
            "ID Process"
        };

        char  fullPath[MAX_PATH];
        DWORD pathSize = sizeof(fullPath);

        if (ERROR_SUCCESS != (rc = PdhMakeCounterPath(&cpe,
                                                      fullPath,
                                                      &pathSize,
                                                      0)))
        {
            return -1;
        }

        PDH_HCOUNTER counter;
        if (ERROR_SUCCESS != (rc = PdhAddCounter(query,
                                                 fullPath,
                                                 0,
                                                 &counter)))
        {
            return -1;
        }

        if (ERROR_SUCCESS != (rc= PdhCollectQueryData(query)))
        {
            break; // no data for this instance index
        }

        PDH_FMT_COUNTERVALUE value = {0};
        if (ERROR_SUCCESS != (rc = PdhGetFormattedCounterValue(counter,
                                                               PDH_FMT_LONG,
                                                               0,
                                                               &value)))
        {
            if (PDH_INVALID_DATA == rc || PDH_INVALID_HANDLE == rc)
                break;

            return -1;
        }

        PdhRemoveCounter(counter);

        if (pid == (int) value.longValue) {
            return instanceIndex;
        }
    }

    return -1;
}

int balb::PerformanceMonitor::Collector<bsls::Platform::OsWindows>
::rebindCounters(bsl::vector<PDH_HCOUNTER> *counters,
                 PDH_HQUERY                 query,
                 const char                *name,
                 int                        instanceIndex)
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);
    BALL_LOG_DEBUG << "Rebinding counters for new instance index "
                   << instanceIndex
                   << BALL_LOG_END;

    if (!counters->empty()) {
        for (int i = 0; i < counters->size(); ++i) {
            PdhRemoveCounter((*counters)[i]);
        }
    }

    counters->resize(COUNTER_MAX);

    PDH_COUNTER_PATH_ELEMENTS counterPathElements[COUNTER_MAX] = {
        { 0, "Process", (char*) name, 0, instanceIndex, "% Processor Time" },
        { 0, "Process", (char*) name, 0, instanceIndex, "% User Time"      },
        { 0, "Process", (char*) name, 0, instanceIndex, "Thread Count"     },
        { 0, "Process", (char*) name, 0, instanceIndex, "Working Set"      },
        { 0, "Process", (char*) name, 0, instanceIndex, "Virtual Bytes"    },
        { 0, "Process", (char*) name, 0, instanceIndex, "Page Faults/sec"  },
        { 0, "Process", (char*) name, 0, instanceIndex, "Elapsed Time"     }
    };

    PDH_STATUS rc;

    for (int i = 0; i < COUNTER_MAX; ++i) {
        char  fullPath[MAX_PATH];
        DWORD pathSize = sizeof(fullPath);

        if (ERROR_SUCCESS != (rc = PdhMakeCounterPath(&counterPathElements[i],
                                                      fullPath,
                                                      &pathSize, 0)))
        {
            BALL_LOG_DEBUG << "Failed to make performance counter path"
                           << BALL_LOG_END;
            return -1;
        }

        if (ERROR_SUCCESS != (rc = PdhAddCounter(query,
                                                 fullPath,
                                                 0,
                                                 &(*counters)[i])))
        {
            BALL_LOG_DEBUG << "Failed to add performance counter to query"
                           << BALL_LOG_END;
            return -1;
        }
    }

    return 0;
}

balb::PerformanceMonitor::Collector<bsls::Platform::OsWindows>
::Collector(bslma::Allocator *basicAllocator)
: d_instanceQuery(0)
, d_measureQuery(0)
, d_instanceIndex(bsl::numeric_limits<int>::max())
, d_counters(basicAllocator)
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    PDH_STATUS rc;

    if (ERROR_SUCCESS != (rc = PdhOpenQuery(0, 0, &d_instanceQuery))) {
        BALL_LOG_WARN << "Failed to open performance query handle"
                      << BALL_LOG_END;
    }

    if (ERROR_SUCCESS != (rc = PdhOpenQuery(0, 0, &d_measureQuery))) {
        BALL_LOG_WARN << "Failed to open performance query handle"
                      << BALL_LOG_END;
    }
}

balb::PerformanceMonitor::Collector<bsls::Platform::OsWindows>::~Collector()
{
    PdhCloseQuery(d_measureQuery);
    PdhCloseQuery(d_instanceQuery);
}

int balb::PerformanceMonitor::Collector<bsls::Platform::OsWindows>
::initialize(Statistics *stats, int pid, const bsl::string &description)
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    stats->d_pid         = pid;
    stats->d_description = description;

    FILETIME creationFileTime, exitFileTime, kernelFileTime, userFileTime;
    GetProcessTimes(GetCurrentProcess(),
                    &creationFileTime,
                    &exitFileTime,
                    &kernelFileTime,
                    &userFileTime);

    SYSTEMTIME creationSystemTime;
    FileTimeToSystemTime(&creationFileTime, &creationSystemTime);

    stats->d_startTimeUtc = bdlt::Datetime(creationSystemTime.wYear,
                                           creationSystemTime.wMonth,
                                           creationSystemTime.wDay,
                                           creationSystemTime.wHour,
                                           creationSystemTime.wMinute,
                                           creationSystemTime.wSecond,
                                           creationSystemTime.wMilliseconds);

    bdlt::Datetime epoch(1970, 1, 1, 0, 0, 0, 0);

    stats->d_startTime = (stats->d_startTimeUtc - epoch).
                                                        totalSecondsAsDouble();

    BALL_LOG_DEBUG << "PID " << stats->d_pid << " started approximately "
                   << stats->d_startTimeUtc << " (UTC)"
                   << BALL_LOG_END;

    return 0;
}

int balb::PerformanceMonitor::Collector<bsls::Platform::OsWindows>
::collect(Statistics *stats)
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&stats->d_guard);

    bsl::string name  = findModuleName(stats->d_pid);
    int instanceIndex = findInstanceIndexFromPid(d_instanceQuery,
                                                 name,
                                                 stats->d_pid);

    BALL_LOG_TRACE << "Found instance index " << instanceIndex
                   << " for process '" << name << "', pid = "
                   << stats->d_pid
                   << BALL_LOG_END;

    PDH_STATUS rc;

    if (instanceIndex != d_instanceIndex) {
        if (0 != rebindCounters(&d_counters,
                                d_measureQuery,
                                name.c_str(),
                                instanceIndex))
        {
            return -1;
        }

        d_instanceIndex = instanceIndex;
    }

    if (ERROR_SUCCESS != (rc = PdhCollectQueryData(d_measureQuery))) {
        BALL_LOG_DEBUG << "Failed to collect performance measures"
                       << BALL_LOG_END;
        return -1;
    }

    PDH_FMT_COUNTERVALUE values[COUNTER_MAX];
    for (int i = 0; i < COUNTER_MAX; ++i) {
        if (ERROR_SUCCESS != (rc = PdhGetFormattedCounterValue(d_counters[i],
                                                               PDH_FMT_DOUBLE,
                                                               0,
                                                               &values[i])))
        {
            BALL_LOG_DEBUG << "Failed to collect performance measures"
                           << BALL_LOG_END;
        }
    }

    if (stats->d_numSamples != 0) {
        stats->d_lstData[e_CPU_UTIL]        = values[PROCESSOR_TIME]
                                           .doubleValue;
        stats->d_lstData[e_CPU_UTIL_USER]   = values[USER_PROCESSOR_TIME]
                                           .doubleValue;

        stats->d_lstData[e_CPU_UTIL_SYSTEM] =
                                          stats->d_lstData[e_CPU_UTIL]
                                        - stats->d_lstData[e_CPU_UTIL_USER];
    }

    stats->d_lstData[e_NUM_THREADS]     = values[THREAD_COUNT].doubleValue;

    stats->d_lstData[e_RESIDENT_SIZE]   = values[WORKING_SET].doubleValue
                                             / (1024 * 1024);

    stats->d_lstData[e_VIRTUAL_SIZE]    = values[VIRTUAL_SIZE].doubleValue
                                             / (1024 * 1024);

    FILETIME creationFileTime, exitFileTime, kernelFileTime, userFileTime;
    GetProcessTimes(GetCurrentProcess(),
                    &creationFileTime,
                    &exitFileTime,
                    &kernelFileTime,
                    &userFileTime);

    LARGE_INTEGER kernelTime          = { kernelFileTime.dwLowDateTime,
                                          kernelFileTime.dwHighDateTime };

    LARGE_INTEGER userTime            = { userFileTime.dwLowDateTime,
                                          userFileTime.dwHighDateTime };

    stats->d_lstData[e_CPU_TIME_SYSTEM] = double(userTime.QuadPart)
                                      / 10000000.0;

    stats->d_lstData[e_CPU_TIME_USER]   = double(userTime.QuadPart)
                                      / 10000000.0;

    stats->d_lstData[e_CPU_TIME_SYSTEM] =
                                        stats->d_lstData[e_CPU_TIME_USER]
                                      + stats->d_lstData[e_CPU_TIME_SYSTEM];

    double elapsedTime = values[ELAPSED_TIME].doubleValue;
    double deltaElapsedTime = elapsedTime - stats->d_elapsedTime;

    stats->d_lstData[e_NUM_PAGEFAULTS] +=
                                         values[PAGEFAULTS_PER_SEC].doubleValue
                                       * deltaElapsedTime;

    stats->d_elapsedTime = elapsedTime;

    ++stats->d_numSamples;

    // Calculate averages for each measure

    for (int i = 0; i < e_NUM_MEASURES; ++i) {
        if (s_measureData[i].hasAverage) {
            stats->d_minData[i]  = bsl::min(stats->d_minData[i],
                                            stats->d_lstData[i]);

            stats->d_maxData[i]  = bsl::max(stats->d_maxData[i],
                                            stats->d_lstData[i]);

            stats->d_totData[i] += stats->d_lstData[i];
        }
    }

    BALL_LOG_TRACE << "Collected performance measures for PID "
                   << stats->d_pid << " (" << stats->d_description << ")"
                   << BALL_LOG_END;

    return 0;
}

#endif

// CREATORS
balb::PerformanceMonitor::Statistics::Statistics(
                                              bslma::Allocator *basicAllocator)
: d_pid(0)
, d_description(basicAllocator)
, d_startTimeUtc()
, d_startTime()
, d_elapsedTime(0.0)
, d_numSamples(0)
, d_guard()
{
    reset();
}

void balb::PerformanceMonitor::Statistics::print(bsl::ostream& os) const
{
    for (int measure = 0; measure < e_NUM_MEASURES; ++measure) {
        print(os, (Measure)measure);
    }
}

void balb::PerformanceMonitor::Statistics::print(bsl::ostream& os,
                                                 Measure       measure) const
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_guard);

    os << bsl::fixed;
    os << bsl::setprecision(2);

    os << "PERF "
       << bsl::left << bsl::setw(18) << d_description
       << bsl::right << bsl::setw(10) << d_pid << " ";

    BSLS_ASSERT(measure >= 0 && measure < e_NUM_MEASURES);

    os << bsl::left << bsl::setw(20);
    os << s_measureData[measure].name;

    os << bsl::right << bsl::setw(5);
    os << d_numSamples;

    os << bsl::right << bsl::setw(16);
    os << d_lstData[measure]
       << s_measureData[measure].units;

    if (s_measureData[measure].hasAverage) {
        os << bsl::right << bsl::setw(14);
        if (nearlyEqual(d_minData[measure],
                        bsl::numeric_limits<double>::max()))
        {
            os << 0.0;
        }
        else
        {
            os << d_minData[measure];
        }
        os << s_measureData[measure].units;

        os << bsl::right << bsl::setw(14)
           << d_maxData[measure]
           << s_measureData[measure].units;

        os << bsl::right << bsl::setw(14)
           << d_totData[measure] / d_numSamples
           << s_measureData[measure].units;
    }
    else {
        os << bsl::right << bsl::setw(14) << "N/A";
        os << bsl::right << bsl::setw(16) << "N/A";
        os << bsl::right << bsl::setw(16) << "N/A";
    }

    os << bsl::endl;
}

void balb::PerformanceMonitor::Statistics::print(
                                        bsl::ostream&  os,
                                        const char    *measureIdentifier) const
{
    for (int measure = 0; measure < e_NUM_MEASURES; ++measure) {
        if (0 == bsl::strcmp(measureIdentifier, s_measureData[measure].tag)) {
            print(os, (Measure)measure);
            return;                                                   // RETURN
        }
    }

    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);
    BALL_LOG_WARN << "No measure matches description "
                     "'" << measureIdentifier << "'"
                  << BALL_LOG_END;
}

void balb::PerformanceMonitor::Statistics::reset()
{
    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_guard);

    d_numSamples = 0;

    bsl::memset(d_lstData, 0, sizeof d_lstData);
    bsl::memset(d_totData, 0, sizeof d_totData);

    bsl::fill(d_minData,
              d_minData + e_NUM_MEASURES,
              bsl::numeric_limits<double>::max());

    bsl::fill(d_maxData,
              d_maxData + e_NUM_MEASURES,
              bsl::numeric_limits<double>::min());
}

namespace balb {
PerformanceMonitor::PerformanceMonitor(bslma::Allocator *basicAllocator)
: d_pidMap(basicAllocator)
, d_interval(0)
, d_scheduler_p(0)
, d_clock(bdlmt::TimerEventScheduler::Handle(INVALID_TIMER_HANDLE))
, d_mapGuard()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

PerformanceMonitor::PerformanceMonitor(
                                    bdlmt::TimerEventScheduler *scheduler,
                                    double                      interval,
                                    bslma::Allocator           *basicAllocator)
: d_pidMap(basicAllocator)
, d_interval(interval)
, d_scheduler_p(scheduler)
, d_clock(bdlmt::TimerEventScheduler::Handle(INVALID_TIMER_HANDLE))
, d_mapGuard()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (interval > 0.0) {
        BSLS_ASSERT(d_scheduler_p);

        d_clock = d_scheduler_p->startClock(
                      bsls::TimeInterval(interval),
                      bdlf::BindUtil::bind(&PerformanceMonitor::collect, this),
                      bdlt::CurrentTime::now() + bsls::TimeInterval(interval));
    }
}

PerformanceMonitor::~PerformanceMonitor()
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    if (d_clock != INVALID_TIMER_HANDLE) {
        BSLS_ASSERT(d_scheduler_p);

        d_scheduler_p->cancelClock(d_clock, true);
    }

    while (!d_pidMap.empty()) {
        int pid = d_pidMap.begin()->second.first->pid();
        if (0 != unregisterPid(pid)) {
            BALL_LOG_WARN << "Failed to unregister PID " << pid
                          << BALL_LOG_END;
        }
    }
}

// MANIPULATORS

int PerformanceMonitor::registerPid(int pid, const bsl::string &description)
{
    if (pid == 0) {
        pid = currentProcessPid();
    }

    StatisticsPtr stats;
    stats.createInplace(d_allocator_p, d_allocator_p);

    CollectorPtr collector;
    collector.createInplace(d_allocator_p, d_allocator_p);

    if (0 != collector->initialize(stats.get(), pid, description)) {
        return -1;                                                    // RETURN
    }

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_mapGuard);
    d_pidMap.insert(bsl::make_pair(pid, bsl::make_pair(stats, collector)));

    return 0;
}

int PerformanceMonitor::unregisterPid(int pid)
{
    if (pid == 0) {
        pid = currentProcessPid();
    }

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_mapGuard);
    return (1 == d_pidMap.erase(pid)) ? 0 : -1;
}

void PerformanceMonitor::setCollectionInterval(double interval)
{
    BSLS_ASSERT(d_scheduler_p);

    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_mapGuard);

    if ((interval <= 0.0) && (d_clock != INVALID_TIMER_HANDLE)) {
        d_scheduler_p->cancelClock(d_clock, false);
        d_clock = INVALID_TIMER_HANDLE;
    }
    else {
        if (d_clock != INVALID_TIMER_HANDLE) {
            d_scheduler_p->cancelClock(d_clock, false);
        }

        d_clock = d_scheduler_p->startClock(
                      bsls::TimeInterval(interval),
                      bdlf::BindUtil::bind(&PerformanceMonitor::collect, this),
                      bdlt::CurrentTime::now() + bsls::TimeInterval(interval));
    }

    d_interval = interval;
}

void PerformanceMonitor::collect()
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_mapGuard);

    for (PidMap::iterator it  = d_pidMap.begin();
                          it != d_pidMap.end();
                        ++it)
    {
        StatisticsPtr& stats     = it->second.first;
        CollectorPtr&  collector = it->second.second;

        collector->collect(stats.get());
    }
}

void PerformanceMonitor::resetStatistics()
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_mapGuard);

    for (PidMap::iterator it  = d_pidMap.begin();
                          it != d_pidMap.end();
                        ++it)
    {
        it->second.first->reset();
    }

    guard.release()->unlock();

    collect();
}
}  // close package namespace

}  // close enterprise namespace

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
