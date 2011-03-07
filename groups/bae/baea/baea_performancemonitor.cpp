// baea_performancemonitor.cpp                                        -*-C++-*-
#include <baea_performancemonitor.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baea_performancemonitor_cpp,"$Id$ $CSID$")

#include <bdef_bind.h>
#include <bdef_function.h>
#include <bdef_placeholder.h>
#include <bslma_allocator.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bdet_datetime.h>
#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

#include <bcemt_writelockguard.h>

#include <bael_log.h>

#include <bsl_algorithm.h>
#include <bsl_cmath.h>
#include <bsl_fstream.h>
#include <bsl_iterator.h>
#include <bsl_iostream.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#if defined(BSLS_PLATFORM__OS_UNIX)
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#if defined(BSLS_PLATFORM__OS_SOLARIS)
#include <procfs.h>
#elif defined(BSLS_PLATFORM__OS_AIX)
#include <sys/procfs.h>
#elif defined(BSLS_PLATFORM__OS_FREEBSD)
#include <bsl_string.h>
#elif defined(BSLS_PLATFORM__OS_HPUX)
#include <sys/pstat.h>
#include <sys/param.h>
#elif defined(BSLS_PLATFORM__OS_LINUX)
#include <bsl_cstdio.h>
#include <dirent.h>
#include <sys/procfs.h>
#endif
#elif defined(BSLS_PLATFORM__OS_WINDOWS)
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

typedef baea_PerformanceMonitor PM;

const char LOG_CATEGORY[] = "BAEA.PERFORMANCEMONITOR";

enum { INVALID_TIMER_HANDLE = -1 }; // invalid timer event scheduler handle

struct MeasureData {
    int id;
    const char *tag;
    const char *name;
    const char *units;
    bool hasAverage;
};

MeasureData s_measureData[PM::BAEA_NUM_MEASURES] = {
  { PM::BAEA_CPU_TIME,
            "CPU_TIME",        "CPU Time",        "s ", false },
  { PM::BAEA_CPU_TIME_USER,
            "CPU_TIME_USER",   "User CPU Time",   "s ", false },
  { PM::BAEA_CPU_TIME_SYSTEM,
            "CPU_TIME_SYSTEM", "System CPU Time", "s ", false },
  { PM::BAEA_CPU_UTIL,
            "CPU_UTIL",        "CPU",             "% ", true  },
  { PM::BAEA_CPU_UTIL_USER,
            "CPU_UTIL_USER",   "User CPU",        "% ", true  },
  { PM::BAEA_CPU_UTIL_SYSTEM,
            "CPU_UTIL_SYSTEM", "System CPU",      "% ", true  },
  { PM::BAEA_RESIDENT_SIZE,
            "RESIDENT_SIZE",   "Resident Size",   "Mb", true  },
  { PM::BAEA_NUM_THREADS,
            "NUM_THREADS",     "Thread Count",    "  ", true  },
  { PM::BAEA_NUM_PAGEFAULTS,
            "NUM_PAGEFAULTS",  "Page Faults",     "  ", false },
};

bool nearlyEqual(double lhs, double rhs)
{
    return bsl::fabs(lhs - rhs) < bsl::numeric_limits<double>::epsilon();
}

#if defined(BSLS_PLATFORM__OS_UNIX)
int currentProcessPid() { return (int) getpid(); }
#elif defined(BSLS_PLATFORM__OS_WINDOWS)
int currentProcessPid() { return (int) GetCurrentProcessId(); }
#endif

}  // close unnamed namespace

// PRIVATE TYPES

#if defined(BSLS_PLATFORM__OS_LINUX)

template <>
class baea_PerformanceMonitor::Collector<bsls_Platform::OsLinux> {
    // Provide a specialization of the 'Collector' class template for the Linux
    // platform.  SunOS, AIX, and Linux all support the /proc filesystem, from
    // which we extract the performance measures for the monitored pid.  On
    // Linux, the layout and content of this file system differs from the
    // layout and content of the file system on SunOS or AIX, hence this
    // 'Collector' class template specialization.

    // Note that the Linux implementation is stateless.  However, the
    // 'Collector' template requires a constructor accepting a single
    // 'bslma_Allocator' argument.

    // PRIVATE TYPES

    struct ProcStatistics {
        // Describes the fields present in /proc/<pid>/stat.  For a complete
        // description of each field, see 'man proc'.

        int           d_pid;             // process pid
        bsl::string   d_comm;            // filename of executable
        char          d_state;           // process state
        int           d_ppid;            // process's parent pid
        int           d_pgrp;            // process group id
        int           d_session;         // process session id
        int           d_tty_nr;          // the tty used by the process
        int           d_tpgid;           // tty owner's group id
        unsigned int  d_flags;           // kernel flags
        unsigned int  d_minflt;          // num minor page faults
        unsigned int  d_cminflt;         // num minor page faults - children
        unsigned int  d_majflt;          // num major page faults
        unsigned int  d_cmajflt;         // num major page faults - children
        unsigned int  d_utime;           // num jiffies in user mode
        unsigned int  d_stime;           // num jiffies in kernel mode
        int           d_cutime;          // num jiffies, user mode, children
        int           d_cstime;          // num jiffies, kernel mode, children
        int           d_priority;        // standard nice value, plus fifteen
        int           d_nice;            // nice value
        int           d_unused;          // reserved value, always 0
        int           d_itrealvalue;     // num jiffies before next SIGALRM
        unsigned int  d_starttime;       // time in jiffies since system boot
        unsigned int  d_vsize;           // virtual memory size, in bytes
        int           d_rss;             // resident set size, in pages

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
        , d_unused()
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
        // Load into the specified 'stats' result the fields present in the
        // '/proc/<pid>/stat' virtual file for the specified 'pid'.  Return 0
        // on success or a non-zero value otherwise.

  public:
    // CREATORS
    Collector(bslma_Allocator *basicAllocator = 0);
        // Create an instance of this class.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    // METHODS

    int initialize(baea_PerformanceMonitor::Statistics *stats,
                   int                                  pid,
                   const bsl::string&                   description);
        // Initialize the specified 'stats' to represent the specified 'pid'
        // having the specified user-defined 'description'.  Return 0 on
        // success or a non-zero value otherwise.

    int collect(baea_PerformanceMonitor::Statistics *stats);
        // Load into the specified 'stats' the performance statistics collected
        // for the pid associated with 'stats'.  Return 0 on success or a
        // non-zero value otherwise.
};

int baea_PerformanceMonitor::Collector<bsls_Platform::OsLinux>::readProcStat(
                                                         ProcStatistics *stats,
                                                         int             pid)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    bsl::stringstream filename;
    filename << "/proc/" << pid << "/stat";

    bsl::ifstream ifs(filename.str().c_str());
    if (!ifs) {
        BAEL_LOG_DEBUG << "Failed to open '" << filename.str() << "'"
                       << BAEL_LOG_END;
        return -1;
    }

    bsl::string str((bsl::istreambuf_iterator<char>(ifs)),
                     bsl::istreambuf_iterator<char>());

    bsl::stringstream ss(str);
    ss >> stats->d_pid;
    ss >> stats->d_comm;
    ss >> stats->d_state;
    ss >> stats->d_ppid;
    ss >> stats->d_pgrp;
    ss >> stats->d_session;
    ss >> stats->d_tty_nr;
    ss >> stats->d_tpgid;
    ss >> stats->d_flags;
    ss >> stats->d_minflt;
    ss >> stats->d_cminflt;
    ss >> stats->d_majflt;
    ss >> stats->d_cmajflt;
    ss >> stats->d_utime;
    ss >> stats->d_stime;
    ss >> stats->d_cutime;
    ss >> stats->d_cstime;
    ss >> stats->d_priority;
    ss >> stats->d_nice;
    ss >> stats->d_unused;
    ss >> stats->d_itrealvalue;
    ss >> stats->d_starttime;
    ss >> stats->d_vsize;
    ss >> stats->d_rss;

    return 0;
}

baea_PerformanceMonitor::Collector<bsls_Platform::OsLinux>::Collector(
                                                             bslma_Allocator *)
{
}

int baea_PerformanceMonitor::Collector<bsls_Platform::OsLinux>::initialize(
                              baea_PerformanceMonitor::Statistics *stats,
                              int                                  pid,
                              const bsl::string&                   description)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    int rc;

    stats->d_pid         = pid;
    stats->d_description = description;

    ProcStatistics procStats;
    if (0 != (rc = readProcStat(&procStats, pid))) {
        BAEL_LOG_DEBUG << "Failed to open /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << "), rc = " << rc
                       << BAEL_LOG_END;
        return -1;
    }

    // TBD: system start time - procStats.d_starttime

    stats->d_startTime = bdet_TimeInterval(0, 0);

    stats->d_startTimeUtc.setYearMonthDay(1970, 1, 1);
    stats->d_startTimeUtc.addSeconds(stats->d_startTime.seconds());

    BAEL_LOG_DEBUG << "PID " << stats->d_pid << " started approximately "
                   << stats->d_startTimeUtc << " (UTC)"
                   << BAEL_LOG_END;

    return 0;

}

int baea_PerformanceMonitor::Collector<bsls_Platform::OsLinux>::collect(
                                    baea_PerformanceMonitor::Statistics *stats)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&stats->d_guard);

    ProcStatistics procStats;
    if (0 != readProcStat(&procStats, stats->d_pid)) {
        BAEL_LOG_DEBUG << "Failed to open /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BAEL_LOG_END;
        return -1;
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

    stats->d_lstData[BAEA_NUM_THREADS]   = numThreads;

    static const int pageSize = sysconf(_SC_PAGESIZE);

    stats->d_lstData[BAEA_RESIDENT_SIZE] = (double) procStats.d_rss
                                    * pageSize
                                    / (1024 * 1024);

    double cpuTimeU = (double) procStats.d_utime / clockTicksPerSec;
    double cpuTimeS = (double) procStats.d_stime / clockTicksPerSec;

    // Calculate CPU utilization

    double deltaCpuTimeU = cpuTimeU - stats->d_lstData[BAEA_CPU_TIME_USER];
    double deltaCpuTimeS = cpuTimeS - stats->d_lstData[BAEA_CPU_TIME_SYSTEM];

    // Inaccurate for the first sample but this doesn't affect our results,
    // as the CPU utilization is not calculated until the second sample is
    // taken.
    double elapsedTime = bdetu_SystemTime::now().totalSecondsAsDouble();

    double dt = elapsedTime - stats->d_elapsedTime;

    if ((stats->d_numSamples != 0) && (dt > 0)) {
        stats->d_lstData[BAEA_CPU_UTIL] = (deltaCpuTimeU + deltaCpuTimeS)
                                   / dt
                                   * 100.0;

        stats->d_lstData[BAEA_CPU_UTIL_USER]   = deltaCpuTimeU / dt * 100.0;
        stats->d_lstData[BAEA_CPU_UTIL_SYSTEM] = deltaCpuTimeS / dt * 100.0;
    }
    else {
        stats->d_lstData[BAEA_CPU_UTIL]        = 0.0;
        stats->d_lstData[BAEA_CPU_UTIL_USER]   = 0.0;
        stats->d_lstData[BAEA_CPU_UTIL_SYSTEM] = 0.0;
    }

    stats->d_lstData[BAEA_CPU_TIME_USER]   = cpuTimeU;
    stats->d_lstData[BAEA_CPU_TIME_SYSTEM] = cpuTimeS;
    stats->d_lstData[BAEA_CPU_TIME]        = cpuTimeU + cpuTimeS;

    stats->d_elapsedTime = elapsedTime;

    ++stats->d_numSamples;

    // Calculate averages for each measure

    for (int i = 0; i < BAEA_NUM_MEASURES; ++i) {
        if (s_measureData[i].hasAverage) {

            stats->d_minData[i]  = bsl::min(stats->d_minData[i],
                                            stats->d_lstData[i]);

            stats->d_maxData[i]  = bsl::max(stats->d_maxData[i],
                                            stats->d_lstData[i]);

            stats->d_totData[i] += stats->d_lstData[i];
        }
    }

    BAEL_LOG_TRACE << "Collected performance measures for PID "
                   << stats->d_pid << " (" << stats->d_description << ")"
                   << BAEL_LOG_END;

    return 0;
}

#elif defined(BSLS_PLATFORM__OS_FREEBSD)

template <>
class baea_PerformanceMonitor::Collector<bsls_Platform::OsFreeBsd> {
    // Provide a specialization of the 'Collector' class template for the Linux
    // platform.  SunOS, AIX, and Linux all support the /proc filesystem, from
    // which we extract the performance measures for the monitored pid.  On
    // Linux, the layout and content of this file system differs from the
    // layout and content of the file system on SunOS or AIX, hence this
    // 'Collector' class template specialization.

    // Note that the FreeBSD implementation is stateless.  However, the
    // 'Collector' template requires a constructor accepting a single
    // 'bslma_Allocator' argument.

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
    Collector(bslma_Allocator *basicAllocator = 0);
        // Create an instance of this class.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    // METHODS

    int initialize(baea_PerformanceMonitor::Statistics *stats,
                   int                                  pid,
                   const bsl::string&                   description);
        // Initialize the specified 'stats' to represent the specified 'pid'
        // having the specified user-defined 'description'.  Return 0 on
        // success or a non-zero value otherwise.

    int collect(baea_PerformanceMonitor::Statistics *stats);
        // Load into the specified 'stats' the performance statistics collected
        // for the pid associated with 'stats'.  Return 0 on success or a
        // non-zero value otherwise.
};

int baea_PerformanceMonitor::Collector<bsls_Platform::OsFreeBsd>::readProcStat(
                                                         ProcStatistics *stats,
                                                         int             pid)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    bsl::stringstream filename;
    filename << "/proc/" << pid << "/status";

    bsl::ifstream ifs(filename.str().c_str());
    if (!ifs) {
        BAEL_LOG_DEBUG << "Failed to open '" << filename.str() << "'"
                       << BAEL_LOG_END;
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

#if 0
    bsl::cout << "comm " << stats->d_comm << ", "
           << "d_pid " << stats->d_pid << ", "
           << "d_ppid " << stats->d_ppid << ", "
           << "d_pgid " << stats->d_pgid << ", "
           << "d_session " << stats->d_session << ", "
           << "d_terminal " << stats->d_terminal << ", "
           << "d_flags " << stats->d_flags << ", "
           << "d_processStartSecs " << stats->d_processStartSecs << ", "
           << "d_processStartMSecs " << stats->d_processStartMSecs << ", "
           << "d_userTimeSecs " << stats->d_userTimeSecs << ", "
           << "d_userTimeMSecs " << stats->d_userTimeMSecs << ", "
           << "d_sysTimeSecs " << stats->d_sysTimeSecs << ", "
           << "d_sysTimeMSecs " << stats->d_sysTimeMSecs << ", "
           << bsl::endl;
#endif
    return 0;
}

baea_PerformanceMonitor::Collector<bsls_Platform::OsFreeBsd>::Collector(
                                                             bslma_Allocator *)
{
}

int baea_PerformanceMonitor::Collector<bsls_Platform::OsFreeBsd>::initialize(
                              baea_PerformanceMonitor::Statistics *stats,
                              int                                  pid,
                              const bsl::string&                   description)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    int rc;

    stats->d_pid         = pid;
    stats->d_description = description;

    ProcStatistics procStats;
    if (0 != (rc = readProcStat(&procStats, pid))) {
        BAEL_LOG_DEBUG << "Failed to open /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << "), rc = " << rc
                       << BAEL_LOG_END;
        return -1;
    }

    bdet_TimeInterval startTime(procStats.d_processStartSecs,
                                procStats.d_processStartMSecs * 1000);
    stats->d_startTimeUtc.setYearMonthDay(1970, 1, 1);
    stats->d_startTimeUtc.addSeconds(stats->d_startTime.seconds());

    BAEL_LOG_DEBUG << "PID " << stats->d_pid << " started approximately "
                   << stats->d_startTimeUtc << " (UTC)"
                   << BAEL_LOG_END;

    return 0;

}

int baea_PerformanceMonitor::Collector<bsls_Platform::OsFreeBsd>::collect(
                                    baea_PerformanceMonitor::Statistics *stats)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&stats->d_guard);

    ProcStatistics procStats;
    if (0 != readProcStat(&procStats, stats->d_pid)) {
        BAEL_LOG_DEBUG << "Failed to open /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BAEL_LOG_END;
        return -1;
    }

    stats->d_lstData[BAEA_NUM_THREADS]   = 0;

    stats->d_lstData[BAEA_RESIDENT_SIZE] = 0;

    double cpuTimeU = (double) procStats.d_userTimeSecs +
                      (double) procStats.d_userTimeMSecs / (double) 1000000;
    double cpuTimeS = (double) procStats.d_sysTimeSecs +
                      (double) procStats.d_sysTimeMSecs / (double) 1000000;

    // Calculate CPU utilization

    double deltaCpuTimeU = cpuTimeU - stats->d_lstData[BAEA_CPU_TIME_USER];
    double deltaCpuTimeS = cpuTimeS - stats->d_lstData[BAEA_CPU_TIME_SYSTEM];

    // Inaccurate for the first sample but this doesn't affect our results,
    // as the CPU utilization is not calculated until the second sample is
    // taken.
    double elapsedTime = bdetu_SystemTime::now().totalSecondsAsDouble();

    double dt = elapsedTime - stats->d_elapsedTime;

    if ((stats->d_numSamples != 0) && (dt > 0)) {
        stats->d_lstData[BAEA_CPU_UTIL] = (deltaCpuTimeU + deltaCpuTimeS)
                                   / dt
                                   * 100.0;

        stats->d_lstData[BAEA_CPU_UTIL_USER]   = deltaCpuTimeU / dt * 100.0;
        stats->d_lstData[BAEA_CPU_UTIL_SYSTEM] = deltaCpuTimeS / dt * 100.0;
    }
    else {
        stats->d_lstData[BAEA_CPU_UTIL]        = 0.0;
        stats->d_lstData[BAEA_CPU_UTIL_USER]   = 0.0;
        stats->d_lstData[BAEA_CPU_UTIL_SYSTEM] = 0.0;
    }

    stats->d_lstData[BAEA_CPU_TIME_USER]   = cpuTimeU;
    stats->d_lstData[BAEA_CPU_TIME_SYSTEM] = cpuTimeS;
    stats->d_lstData[BAEA_CPU_TIME]        = cpuTimeU + cpuTimeS;

    stats->d_elapsedTime = elapsedTime;

    ++stats->d_numSamples;

    // Calculate averages for each measure

    for (int i = 0; i < BAEA_NUM_MEASURES; ++i) {
        if (s_measureData[i].hasAverage) {

            stats->d_minData[i]  = bsl::min(stats->d_minData[i],
                                            stats->d_lstData[i]);

            stats->d_maxData[i]  = bsl::max(stats->d_maxData[i],
                                            stats->d_lstData[i]);

            stats->d_totData[i] += stats->d_lstData[i];
        }
    }

    BAEL_LOG_TRACE << "Collected performance measures for PID "
                   << stats->d_pid << " (" << stats->d_description << ")"
                   << BAEL_LOG_END;

    return 0;
}

#elif defined(BSLS_PLATFORM__OS_UNIX)

template <>
class baea_PerformanceMonitor::Collector<bsls_Platform::OsUnix> {
    // Provide a specialization of the 'Collector' class template for
    // UNIX-based platforms.  SunOS, AIX, and Linux all support the /proc
    // filesystem, from which we extract the performance measures for the
    // monitored pid.

    // Note that the UNIX-based implementation is stateless.  However, the
    // 'Collector' template requires a constructor accepting a single
    // 'bslma_Allocator' argument.

  public:
    // CREATORS
    Collector(bslma_Allocator *basicAllocator = 0);
        // Create an instance of this class.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    // METHODS

    int initialize(baea_PerformanceMonitor::Statistics *stats,
                   int                                  pid,
                   const bsl::string&                   description);
        // Initialize the specified 'stats' to represent the specified 'pid'
        // having the specified user-defined 'description'.  Return 0 on
        // success or a non-zero value otherwise.

    int collect(baea_PerformanceMonitor::Statistics *stats);
        // Load into the specified 'stats' the performance statistics collected
        // for the pid associated with 'stats'.  Return 0 on success or a
        // non-zero value otherwise.
};

baea_PerformanceMonitor::Collector<bsls_Platform::OsUnix>::Collector(
                                                             bslma_Allocator *)
{
}

int baea_PerformanceMonitor::Collector<bsls_Platform::OsUnix>::initialize(
                              baea_PerformanceMonitor::Statistics *stats,
                              int                                  pid,
                              const bsl::string&                   description)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    stats->d_pid         = pid;
    stats->d_description = description;

#ifndef BSLS_PLATFORM__OS_HPUX
    bsl::stringstream procfsInfo;
    procfsInfo << "/proc/" << stats->d_pid << "/psinfo";

    int fd = open(procfsInfo.str().c_str(), O_RDONLY);
    if (fd == -1) {
        BAEL_LOG_DEBUG << "Failed to open /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BAEL_LOG_END;
        return -1;
    }
    psinfo_t psinfo;
    if (sizeof(psinfo) != read(fd, &psinfo, sizeof(psinfo))) {
        BAEL_LOG_DEBUG << "Failed to read /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BAEL_LOG_END;
        return -1;
    }

    stats->d_startTime = bdet_TimeInterval(psinfo.pr_start.tv_sec,
                                           psinfo.pr_start.tv_nsec);

    close(fd);
#else
    pst_status psinfo;
    int rc = pstat_getproc(&psinfo, sizeof psinfo, 0, stats->d_pid);
    if (-1 == rc)
    {
        BAEL_LOG_DEBUG << "Failed to read /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BAEL_LOG_END;
        return -1;
    }
    // TBD only seconds on HPUX??
    stats->d_startTime = bdet_TimeInterval(psinfo.pst_start,
                                           0);
#endif

    stats->d_startTimeUtc.setYearMonthDay(1970, 1, 1);
    stats->d_startTimeUtc.addSeconds(stats->d_startTime.seconds());

    BAEL_LOG_DEBUG << "PID " << stats->d_pid << " started approximately "
                   << stats->d_startTimeUtc << " (UTC)"
                   << BAEL_LOG_END;

    return 0;
}

int baea_PerformanceMonitor::Collector<bsls_Platform::OsUnix>::collect(
                                    baea_PerformanceMonitor::Statistics *stats)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&stats->d_guard);

    int numThreads;
    double cpuTimeU;
    double cpuTimeS;

#ifndef BSLS_PLATFORM__OS_HPUX
    int fd;

    bsl::stringstream procfsInfo;
    procfsInfo << "/proc/" << stats->d_pid << "/psinfo";

    fd = open(procfsInfo.str().c_str(), O_RDONLY);
    if (fd == -1) {
        BAEL_LOG_DEBUG << "Failed to open /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BAEL_LOG_END;
        return -1;
    }

    psinfo_t psinfo;
    if (sizeof(psinfo) != read(fd, &psinfo, sizeof(psinfo))) {
        BAEL_LOG_DEBUG << "Failed to read /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BAEL_LOG_END;
        return -1;
    }

    numThreads = psinfo.pr_nlwp;

    stats->d_lstData[BAEA_NUM_THREADS]   = (double) numThreads;
    stats->d_lstData[BAEA_RESIDENT_SIZE] = (double) psinfo.pr_rssize // in Kb
                                           / 1024;

    close(fd);

    bsl::stringstream procfsStatus;
    procfsStatus << "/proc/" << stats->d_pid << "/status";

    fd = open(procfsStatus.str().c_str(), O_RDONLY);
    if (fd == -1) {
        BAEL_LOG_DEBUG << "Failed to open /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BAEL_LOG_END;
        return -1;
    }

    pstatus_t prstatus;
    if (sizeof(prstatus) != read(fd, &prstatus, sizeof(prstatus))) {
        BAEL_LOG_DEBUG << "Failed to read /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BAEL_LOG_END;
        return -1;
    }

    cpuTimeU = bdet_TimeInterval((double)prstatus.pr_utime.tv_sec,
                                        (double)prstatus.pr_utime.tv_nsec).
                                                        totalSecondsAsDouble();

    cpuTimeS = bdet_TimeInterval((double)prstatus.pr_stime.tv_sec,
                                        (double)prstatus.pr_stime.tv_nsec).
                                                        totalSecondsAsDouble();

    close(fd);
#else
    pst_status psinfo;
    int rc = pstat_getproc(&psinfo, sizeof psinfo, 0, stats->d_pid);
    if (-1 == rc)
    {
        BAEL_LOG_DEBUG << "Failed to read /proc filesystem for pid "
                       << stats->d_pid
                       << " (" << stats->d_description << ")"
                       << BAEL_LOG_END;
        return -1;
    }
    numThreads = psinfo.pst_nlwps;

    stats->d_lstData[BAEA_NUM_THREADS]   = (double) numThreads;
    stats->d_lstData[BAEA_RESIDENT_SIZE] = (double) psinfo.pst_rssize // in Kb
                                           / 1024;

    // TBD -- only seconds resolution??
    cpuTimeU = bdet_TimeInterval((double)psinfo.pst_utime, 0).
                                                        totalSecondsAsDouble();

    cpuTimeS = bdet_TimeInterval((double)psinfo.pst_stime, 0).
                                                        totalSecondsAsDouble();
#endif

    // Calculate CPU utilization

    double deltaCpuTimeU = cpuTimeU - stats->d_lstData[BAEA_CPU_TIME_USER];
    double deltaCpuTimeS = cpuTimeS - stats->d_lstData[BAEA_CPU_TIME_SYSTEM];

    double elapsedTime = (bdetu_SystemTime::now() - stats->d_startTime).
                                                        totalSecondsAsDouble();

    double dt = elapsedTime - stats->d_elapsedTime;

    if ((stats->d_numSamples != 0) && (dt > 0)) {
        stats->d_lstData[BAEA_CPU_UTIL] = (deltaCpuTimeU + deltaCpuTimeS)
                                   / dt
                                   * 100.0;

        stats->d_lstData[BAEA_CPU_UTIL_USER]   = deltaCpuTimeU / dt * 100.0;
        stats->d_lstData[BAEA_CPU_UTIL_SYSTEM] = deltaCpuTimeS / dt * 100.0;
    }
    else {
        stats->d_lstData[BAEA_CPU_UTIL]        = 0.0;
        stats->d_lstData[BAEA_CPU_UTIL_USER]   = 0.0;
        stats->d_lstData[BAEA_CPU_UTIL_SYSTEM] = 0.0;
    }

    if (stats->d_lstData[BAEA_CPU_UTIL] > numThreads * 100.0)
    {
        BAEL_LOG_DEBUG << "Calculated impossible CPU utilization = "
                       << stats->d_lstData[BAEA_CPU_UTIL]
                       << ", num threads = " << numThreads
                       << BAEL_LOG_END;

        stats->d_lstData[BAEA_CPU_UTIL] = 0.0;
        stats->d_lstData[BAEA_CPU_UTIL_USER] = 0.0;
        stats->d_lstData[BAEA_CPU_UTIL_SYSTEM] = 0.0;
    }

    stats->d_lstData[BAEA_CPU_TIME_USER]   = cpuTimeU;
    stats->d_lstData[BAEA_CPU_TIME_SYSTEM] = cpuTimeS;
    stats->d_lstData[BAEA_CPU_TIME]        = cpuTimeU + cpuTimeS;

    stats->d_elapsedTime = elapsedTime;

    ++stats->d_numSamples;

    // Calculate averages for each measure

    for (int i = 0; i < BAEA_NUM_MEASURES; ++i) {
        if (s_measureData[i].hasAverage) {

            stats->d_minData[i]  = bsl::min(stats->d_minData[i],
                                           stats->d_lstData[i]);

            stats->d_maxData[i]  = bsl::max(stats->d_maxData[i],
                                           stats->d_lstData[i]);

            stats->d_totData[i] += stats->d_lstData[i];
        }
    }

    BAEL_LOG_TRACE << "Collected performance measures for PID "
                   << stats->d_pid << " (" << stats->d_description << ")"
                   << BAEL_LOG_END;

    return 0;
}

#elif defined(BSLS_PLATFORM__OS_WINDOWS)

template <>
struct baea_PerformanceMonitor::Collector<bsls_Platform::OsWindows> {
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
    Collector(bslma_Allocator *basicAllocator = 0);
        // Create an instance of this class.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~Collector();
        // Destroy an instance of this class.

    // METHODS

    int initialize(baea_PerformanceMonitor::Statistics *stats,
                   int                                  pid,
                   const bsl::string&                   description);
        // Initialize the specified 'stats' to represent the specified 'pid'
        // having the specified user-defined 'description'.  Return 0 on
        // success or a non-zero value otherwise.

    int collect(baea_PerformanceMonitor::Statistics *stats);
        // Load into the specified 'stats' the performance statistics collected
        // for the pid associated with 'stats'.  Return 0 on success or a
        // non-zero value otherwise.
};

bsl::string
baea_PerformanceMonitor::Collector<bsls_Platform::OsWindows>::findModuleName(
                                                                       int pid)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    DWORD accessRights = STANDARD_RIGHTS_READ
                       | PROCESS_QUERY_INFORMATION
                       | PROCESS_VM_READ;

    HANDLE process = OpenProcess(accessRights,
                                 FALSE,
                                 pid);

    if (0 == process) {
        BAEL_LOG_ERROR << "Failed to open process handle to pid " << pid
                       << BAEL_LOG_END;
        return "";
    }

    char moduleExeName[MAX_PATH];
    GetModuleBaseName(process, 0, moduleExeName, MAX_PATH);

    CloseHandle(process);

    const char *extension = bsl::strchr(moduleExeName, '.');
    if (0 == extension)
        return bsl::string(moduleExeName);

    return bsl::string((const char*)moduleExeName, extension);
}

int
baea_PerformanceMonitor::Collector<bsls_Platform::OsWindows>::
findInstanceIndexFromPid(PDH_HQUERY         query,
                         const bsl::string& moduleName,
                         int                pid)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

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

int
baea_PerformanceMonitor::Collector<bsls_Platform::OsWindows>::rebindCounters(
                                      bsl::vector<PDH_HCOUNTER> *counters,
                                      PDH_HQUERY                 query,
                                      const char                *name,
                                      int                        instanceIndex)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    BAEL_LOG_DEBUG << "Rebinding counters for new instance index "
                   << instanceIndex
                   << BAEL_LOG_END;

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
            BAEL_LOG_DEBUG << "Failed to make performance counter path"
                           << BAEL_LOG_END;
            return -1;
        }

        if (ERROR_SUCCESS != (rc = PdhAddCounter(query,
                                                 fullPath,
                                                 0,
                                                 &(*counters)[i])))
        {
            BAEL_LOG_DEBUG << "Failed to add performance counter to query"
                           << BAEL_LOG_END;
            return -1;
        }
    }

    return 0;
}

baea_PerformanceMonitor::Collector<bsls_Platform::OsWindows>::Collector(
                                               bslma_Allocator *basicAllocator)
: d_instanceQuery(0)
, d_measureQuery(0)
, d_instanceIndex(bsl::numeric_limits<int>::max())
, d_counters(basicAllocator)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    PDH_STATUS rc;

    if (ERROR_SUCCESS != (rc = PdhOpenQuery(0, 0, &d_instanceQuery))) {
        BAEL_LOG_WARN << "Failed to open performance query handle"
                      << BAEL_LOG_END;
    }

    if (ERROR_SUCCESS != (rc = PdhOpenQuery(0, 0, &d_measureQuery))) {
        BAEL_LOG_WARN << "Failed to open performance query handle"
                      << BAEL_LOG_END;
    }
}

baea_PerformanceMonitor::Collector<bsls_Platform::OsWindows>::~Collector()
{
    PdhCloseQuery(d_measureQuery);
    PdhCloseQuery(d_instanceQuery);
}

int baea_PerformanceMonitor::Collector<bsls_Platform::OsWindows>::initialize(
                              baea_PerformanceMonitor::Statistics *stats,
                              int                                  pid,
                              const bsl::string&                   description)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

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

    stats->d_startTimeUtc = bdet_Datetime(creationSystemTime.wYear,
                                          creationSystemTime.wMonth,
                                          creationSystemTime.wDay,
                                          creationSystemTime.wHour,
                                          creationSystemTime.wMinute,
                                          creationSystemTime.wSecond,
                                          creationSystemTime.wMilliseconds);

    bdet_Datetime epoch(1970, 1, 1, 0, 0, 0, 0);

    stats->d_startTime = (stats->d_startTimeUtc - epoch).
                                                        totalSecondsAsDouble();

    BAEL_LOG_DEBUG << "PID " << stats->d_pid << " started approximately "
                   << stats->d_startTimeUtc << " (UTC)"
                   << BAEL_LOG_END;

    return 0;
}

int baea_PerformanceMonitor::Collector<bsls_Platform::OsWindows>::collect(
                                    baea_PerformanceMonitor::Statistics *stats)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&stats->d_guard);

    bsl::string name  = findModuleName(stats->d_pid);
    int instanceIndex = findInstanceIndexFromPid(d_instanceQuery,
                                                 name,
                                                 stats->d_pid);

    BAEL_LOG_TRACE << "Found instance index " << instanceIndex
                   << " for process '" << name << "', pid = "
                   << stats->d_pid
                   << BAEL_LOG_END;

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

        if (ERROR_SUCCESS != (rc = PdhCollectQueryData(d_measureQuery))) {
            BAEL_LOG_DEBUG << "Failed to collect performance measures"
                           << BAEL_LOG_END;
            return -1;
        }

        return 0;
    }

    if (ERROR_SUCCESS != (rc = PdhCollectQueryData(d_measureQuery))) {
        BAEL_LOG_DEBUG << "Failed to collect performance measures"
                       << BAEL_LOG_END;
        return -1;
    }

    PDH_FMT_COUNTERVALUE values[COUNTER_MAX];
    for (int i = 0; i < COUNTER_MAX; ++i) {
        if (ERROR_SUCCESS != (rc = PdhGetFormattedCounterValue(
                                  d_counters[i],
                                  PDH_FMT_DOUBLE,
                                  0,
                                  &values[i])))
        {
            BAEL_LOG_DEBUG << "Failed to collect performance measures"
                           << BAEL_LOG_END;
        }
    }

    if (stats->d_numSamples != 0) {
        stats->d_lstData[BAEA_CPU_UTIL]        = values[PROCESSOR_TIME]
                                           .doubleValue;
        stats->d_lstData[BAEA_CPU_UTIL_USER]   = values[USER_PROCESSOR_TIME]
                                           .doubleValue;

        stats->d_lstData[BAEA_CPU_UTIL_SYSTEM] =
                                          stats->d_lstData[BAEA_CPU_UTIL]
                                        - stats->d_lstData[BAEA_CPU_UTIL_USER];
    }

    stats->d_lstData[BAEA_NUM_THREADS]     = values[THREAD_COUNT].doubleValue;

    stats->d_lstData[BAEA_RESIDENT_SIZE]   = values[WORKING_SET].doubleValue
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

    stats->d_lstData[BAEA_CPU_TIME_SYSTEM] = double(userTime.QuadPart)
                                      / 10000000.0;

    stats->d_lstData[BAEA_CPU_TIME_USER]   = double(userTime.QuadPart)
                                      / 10000000.0;

    stats->d_lstData[BAEA_CPU_TIME_SYSTEM] =
                                        stats->d_lstData[BAEA_CPU_TIME_USER]
                                      + stats->d_lstData[BAEA_CPU_TIME_SYSTEM];

    double elapsedTime = values[ELAPSED_TIME].doubleValue;
    double deltaElapsedTime = elapsedTime - stats->d_elapsedTime;

    stats->d_lstData[BAEA_NUM_PAGEFAULTS] +=
                                         values[PAGEFAULTS_PER_SEC].doubleValue
                                       * deltaElapsedTime;

    stats->d_elapsedTime = elapsedTime;

    ++stats->d_numSamples;

    // Calculate averages for each measure

    for (int i = 0; i < BAEA_NUM_MEASURES; ++i) {
        if (s_measureData[i].hasAverage) {
            stats->d_minData[i]  = bsl::min(stats->d_minData[i],
                                            stats->d_lstData[i]);

            stats->d_maxData[i]  = bsl::max(stats->d_maxData[i],
                                            stats->d_lstData[i]);

            stats->d_totData[i] += stats->d_lstData[i];
        }
    }

    BAEL_LOG_TRACE << "Collected performance measures for PID "
                   << stats->d_pid << " (" << stats->d_description << ")"
                   << BAEL_LOG_END;

    return 0;
}

#endif

// CREATORS
baea_PerformanceMonitor::Statistics::Statistics(
        bslma_Allocator *basicAllocator)
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

void baea_PerformanceMonitor::Statistics::print(bsl::ostream& os) const
{
    for (int measure = 0; measure < BAEA_NUM_MEASURES; ++measure) {
        print(os, (Measure)measure);
    }
}

void baea_PerformanceMonitor::Statistics::print(bsl::ostream& os,
                                                Measure measure) const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_guard);

    os << bsl::fixed;
    os << bsl::setprecision(2);

    os << "PERF "
       << bsl::left << bsl::setw(18) << d_description
       << bsl::right << bsl::setw(10) << d_pid << " ";

    BSLS_ASSERT(measure >= 0 && measure < BAEA_NUM_MEASURES);

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

void baea_PerformanceMonitor::Statistics::print(
                                              bsl::ostream& os,
                                              const char    *measureIdentifier)
                                              const
{
    for (int measure = 0; measure < BAEA_NUM_MEASURES; ++measure) {
        if (0 == bsl::strcmp(measureIdentifier, s_measureData[measure].tag)) {
            print(os, (Measure)measure);
            return;
        }
    }

    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    BAEL_LOG_WARN << "No measure matches description "
                     "'" << measureIdentifier << "'"
                  << BAEL_LOG_END;
}

void baea_PerformanceMonitor::Statistics::reset()
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_guard);

    d_numSamples = 0;

    bsl::memset(d_lstData, 0, sizeof d_lstData);
    bsl::memset(d_totData, 0, sizeof d_totData);

    bsl::fill(d_minData,
              d_minData + BAEA_NUM_MEASURES,
              bsl::numeric_limits<double>::max());

    bsl::fill(d_maxData,
              d_maxData + BAEA_NUM_MEASURES,
              bsl::numeric_limits<double>::min());
}

baea_PerformanceMonitor::baea_PerformanceMonitor(
        bslma_Allocator *basicAllocator)
: d_pidMap(basicAllocator)
, d_interval(0)
, d_scheduler_p(0)
, d_clock(bcep_TimerEventScheduler::Handle(INVALID_TIMER_HANDLE))
, d_mapGuard()
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

baea_PerformanceMonitor::baea_PerformanceMonitor(
        bcep_TimerEventScheduler *scheduler,
        double                    interval,
        bslma_Allocator          *basicAllocator)
: d_pidMap(basicAllocator)
, d_interval(interval)
, d_scheduler_p(scheduler)
, d_clock(bcep_TimerEventScheduler::Handle(INVALID_TIMER_HANDLE))
, d_mapGuard()
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    if (interval > 0.0) {
        BSLS_ASSERT(d_scheduler_p);

        d_clock = d_scheduler_p->startClock(
                         bdet_TimeInterval(interval),
                         bdef_BindUtil::bind(&baea_PerformanceMonitor::collect,
                                             this),
                         bdetu_SystemTime::now() +
                         bdet_TimeInterval(interval));
    }
}

baea_PerformanceMonitor::~baea_PerformanceMonitor()
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    if (d_clock != INVALID_TIMER_HANDLE) {
        BSLS_ASSERT(d_scheduler_p);

        d_scheduler_p->cancelClock(d_clock, true);
    }

    while (!d_pidMap.empty()) {
        int pid = d_pidMap.begin()->second.first->pid();
        if (0 != unregisterPid(pid)) {
            BAEL_LOG_WARN << "Failed to unregister PID " << pid
                          << BAEL_LOG_END;
        }
    }
}

// MANIPULATORS

int baea_PerformanceMonitor::registerPid(int                pid,
                                         const bsl::string& description)
{
    if (pid == 0) {
        pid = currentProcessPid();
    }

    StatisticsPtr stats;
    stats.createInplace(d_allocator_p, d_allocator_p);

    CollectorPtr collector;
    collector.createInplace(d_allocator_p, d_allocator_p);

    if (0 != collector->initialize(stats.ptr(), pid, description)) {
        return -1;
    }

    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_mapGuard);
    d_pidMap.insert(bsl::make_pair(pid,
                                   bsl::make_pair(stats, collector)));

    return 0;
}

int baea_PerformanceMonitor::unregisterPid(int pid)
{
    if (pid == 0) {
        pid = currentProcessPid();
    }

    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_mapGuard);
    return (1 == d_pidMap.erase(pid)) ? 0 : -1;
}

void baea_PerformanceMonitor::setCollectionInterval(double interval)
{
    BSLS_ASSERT(d_scheduler_p);

    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_mapGuard);

    if ((interval <= 0.0) && (d_clock != INVALID_TIMER_HANDLE)) {
        d_scheduler_p->cancelClock(d_clock, false);
        d_clock = INVALID_TIMER_HANDLE;
    }
    else {
        if (d_clock != INVALID_TIMER_HANDLE) {
            d_scheduler_p->cancelClock(d_clock, false);
        }

        d_clock = d_scheduler_p->startClock(
                         bdet_TimeInterval(interval),
                         bdef_BindUtil::bind(&baea_PerformanceMonitor::collect,
                                             this),
                         bdetu_SystemTime::now() +
                         bdet_TimeInterval(interval));
    }

    d_interval = interval;
}

void baea_PerformanceMonitor::collect()
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_mapGuard);

    for (PidMap::iterator it  = d_pidMap.begin();
                          it != d_pidMap.end();
                        ++it)
    {
        StatisticsPtr& stats     = it->second.first;
        CollectorPtr&  collector = it->second.second;

        collector->collect(stats.ptr());
    }
}

void baea_PerformanceMonitor::resetStatistics()
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_mapGuard);

    for (PidMap::iterator it  = d_pidMap.begin();
                          it != d_pidMap.end();
                        ++it)
    {
        it->second.first->reset();
    }

    guard.release()->unlock();

    collect();
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
