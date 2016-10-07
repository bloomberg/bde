// balb_performancemonitor.t.cpp                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <balb_performancemonitor.h>

#include <bslim_testutil.h>

#include <bslmt_threadutil.h>

#include <bdls_processutil.h>

#include <bdlt_currenttime.h>
#include <bdlt_datetimeinterval.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_deque.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_stdexcept.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bslma_default.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bsls_platform.h>
#include <bsls_types.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <sys/mman.h>
#include <sys/times.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#include <windows.h>
#else
#include <bsl_c_errno.h>
#include <bsl_c_signal.h>
#include <unistd.h>
#include <sys/wait.h>
#endif

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] int numRegisteredPids() const
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE
// [ 4] CONCERN: The Process Start Time is Reasonable
// [ 5] CONCERN: Statistics are Reset Correctly (DRQS 49280976)
// [-1] TESTING VIRTUAL SIZE AND RESIDENT SIZE
// [-3] DUMMY TEST CASE
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

// ============================================================================
//                       HELPER FUNCTIONS AND CLASSES
// ----------------------------------------------------------------------------

namespace processSupport {

#ifdef BSLS_PLATFORM_OS_UNIX

typedef int ProcessHandle;

ProcessHandle exec(bsl::string command, bsl::vector<bsl::string> arguments)
    // Execute in a child process the program located at the specified
    // 'command' path, with the specified 'arguments'.  Return a
    // 'ProcessHandle' object identifying the child process.
{
    typedef bsl::vector<bsl::string> Args;

    ProcessHandle handle;
    handle = fork();

    if (0 == handle) {
        // child process

        bsl::vector<char *>  argvec;

        argvec.push_back(&command[0]);     // N.B. Assumes that 'bsl::string'
                                           // contents are always
                                           // null-terminated.
                                           //
                                           // Same assumption applies below.

        for (Args::iterator i = arguments.begin(); i != arguments.end(); ++i) {
            argvec.push_back(&(*i)[0]);
        }
        argvec.push_back(0);

        execv(argvec[0], argvec.data());

        BSLS_ASSERT_OPT(0 && "execv failed");
    }

    return handle;
}

int getId(const ProcessHandle& handle)
    // Return the integer value of the process ID associated with the process
    // identified by the specified 'handle'.
{
    return handle;
}

int terminateProcess(const ProcessHandle& handle)
    // Terminate the process identified by the specified 'handle'.  Return '0'
    // on success and a non-zero value on failure.
{
    if (kill(handle, SIGTERM)) {
        return errno;                                                 // RETURN
    }

    return 0;
}

#elif BSLS_PLATFORM_OS_WINDOWS

typedef PROCESS_INFORMATION ProcessHandle;

ProcessHandle exec(bsl::string command, bsl::vector<bsl::string> arguments)
    // Execute in a child process the program located at the specified
    // 'command' path, with the specified 'arguments'.  Return a
    // 'ProcessHandle' object identifying the child process.
{
    BSLMF_ASSERT(sizeof(DWORD) == sizeof(int));

    typedef bsl::vector<bsl::string> Args;

    STARTUPINFO sui;
    GetStartupInfo(&sui);

    // Need to have 'PROCESS_QUERY_INFORMATION' for 'GetExitCodeProcess'
    // and 'PROCESS_TERMINATE' for 'TerminateProcess'.
    //
    // Empirically, these permissions seem to be granted for child processes.

    ProcessHandle handle;

    const char *app = command.data();

    bsl::string commandLine;
    for (Args::iterator i = arguments.begin(); i != arguments.end(); ++i) {
        if (arguments.begin() != i) {
            commandLine.append(" ");
        }
        commandLine.append(*i);
    }

    // N.B. The 'lpCommandLine' argument of 'CreateProcess' must be mutable,
    // per requirements of 'CreateProcessEx' (cited in documentation for
    // 'CreateProcess').
    //
    // https://msdn.microsoft.com/en-us/ library/ms682425.aspx

    char *cmd = &commandLine[0];

    bool  rc  = CreateProcess(app,       // lpApplicationName
                              cmd,       // lpCommandLine
                              NULL,      // lpProcessAttributes
                              NULL,      // lpThreadAttributes
                              true,      // bInheritHandles
                              0,         // dwCreationFlags
                              NULL,      // lpEnvironment
                              NULL,      // lpCurrentDirectory
                              &sui,      // lpStartupInfo - in
                              &handle);  // lpProcessInformation - out

    if (!rc) {
        // Following the behavior of UNIX 'fork', failure to create process is
        // indicated by '-1 == getId(handle)',

        handle.dwProcessId = static_cast<DWORD>(-1);
    }

    return handle;
}

int getId(const ProcessHandle& handle)
    // Return the integer value of the process ID associated with the process
    // identified by the specified 'handle'.
{
    BSLMF_ASSERT(sizeof(DWORD) == sizeof(int));

    return static_cast<int>(handle.dwProcessId);
}

int terminateProcess(const ProcessHandle& handle)
    // Terminate the process identified by the specified 'handle'.  Return '0'
    // on success and a non-zero value on failure.
{
    BSLMF_ASSERT(sizeof(DWORD) == sizeof(int));

    if (!TerminateProcess(handle.hProcess, 0)) {
        DWORD exitCode;
        if (GetExitCodeProcess(handle.hProcess, &exitCode)) {
            return static_cast<int>(exitCode);
        }
        else {
            return -1;                                                // RETURN
        }
    }

    if (! (CloseHandle(handle.hProcess) && CloseHandle(handle.hThread))) {
            return -1;                                                // RETURN
    }

    return 0;
}

#else
#error "Unknown OS type."
#endif

}  // close namespace processSupport

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

#ifndef BSLS_PLATFORM_OS_WINDOWS
namespace test {

class MmapAllocator : public bslma::Allocator {

    typedef bsl::map<void*, bsl::size_t> MapType;

    MapType           d_map;
    bslma::Allocator *d_allocator_p;

    // UNIMPLEMENTED
    MmapAllocator(const MmapAllocator &);             // = deleted
    MmapAllocator& operator=(const MmapAllocator &);  // = deleted

  public:
    MmapAllocator(bslma::Allocator *basicAllocator = 0)
    : d_map(basicAllocator)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    virtual void *allocate(size_type size)
    {
#if defined(BSLS_PLATFORM_OS_SOLARIS)
        // Run 'pmap -xs <pid>' to see a tabular description of the memory
        // layout of a process.

        void *result = mmap(0,
                            size,
                            PROT_READ | PROT_WRITE,
                            MAP_ANON | MAP_PRIVATE,
                            -1,
                            0);

        BSLS_ASSERT(reinterpret_cast<void*>(-1) != result);

        d_map.insert(bsl::make_pair(result, size));

        return result;

#elif defined(BSLS_PLATFORM_OS_AIX)

        void *result = mmap(0,
                            size,
                            PROT_READ | PROT_WRITE,
                            MAP_ANON | MAP_PRIVATE,
                            -1,
                            0);

        BSLS_ASSERT(reinterpret_cast<void*>(-1) != result);

        d_map.insert(bsl::make_pair(result, size));

        return result;

#elif defined(BSLS_PLATFORM_OS_HPUX)

        void *result = mmap(0,
                            size,
                            PROT_READ | PROT_WRITE,
                            MAP_ANONYMOUS | MAP_PRIVATE,
                            -1,
                            0);

        BSLS_ASSERT(reinterpret_cast<void*>(-1) != result);

        d_map.insert(bsl::make_pair(result, size));

        return result;

#elif defined(BSLS_PLATFORM_OS_LINUX)  \
   || defined(BSLS_PLATFORM_OS_DARWIN) \
   || defined(BSLS_PLATFORM_OS_CYGWIN)
        return d_allocator_p->allocate(size);
#else
#error Not implemented.
#endif
    }

    virtual void deallocate(void *address)
    {
#if defined(BSLS_PLATFORM_OS_LINUX)  \
 || defined(BSLS_PLATFORM_OS_DARWIN) \
 || defined(BSLS_PLATFORM_OS_CYGWIN)
        d_allocator_p->deallocate(address);
#else
        MapType::iterator it = d_map.find(address);
        BSLS_ASSERT(it != d_map.end());

        bsl::size_t size = it->second;

        int rc = munmap(static_cast<char*>(address), size);
        BSLS_ASSERT(-1 != rc);
#endif
    }
};

}  // close namespace test

void report(int                bufferSize,
            bsls::Types::Int64 currentBytesInUse,
            bsls::Types::Int64 peakBytesInUse,
            double             virtualSize,
            double             residentSize)
{
    bsl::cout << "BufferSize        = "
              << bsl::setprecision(8)
              << bufferSize
              << " ("
              << bsl::setprecision(8)
              << (((double)(bufferSize)) / (1024 * 1024))
              << " MB)"
              << bsl::endl;

    bsl::cout << "CurrentBytesInUse = "
              << bsl::setprecision(8)
              << currentBytesInUse
              << " ("
              << bsl::setprecision(8)
              << (((double)(currentBytesInUse)) / (1024 * 1024))
              << " MB)"
              << bsl::endl;

    bsl::cout << "PeakBytesInUse   = "
              << bsl::setprecision(8)
              << peakBytesInUse
              << " ("
              << bsl::setprecision(8)
              << (((double)(peakBytesInUse)) / (1024 * 1024))
              << " MB)"
              << bsl::endl;

    bsl::cout << "VirtualSize      = "
              << bsl::setprecision(8)
              << virtualSize * 1024 * 1024
              << " ("
              << bsl::setprecision(8)
              << (((double)(virtualSize)))
              << " MB)"
              << bsl::endl;

    bsl::cout << "ResidentSize     = "
              << bsl::setprecision(8)
              << residentSize * 1024 * 1024
              << " ("
              << bsl::setprecision(8)
              << (((double)(residentSize)))
              << " MB)"
              << bsl::endl;
}
#endif

double wasteCpuTime()
    // Just take up a measurable amount of cpu time.  Try 100 clock ticks (1.0
    // seconds or less).
{
#ifdef BSLS_PLATFORM_OS_UNIX
    struct tms tmsBuffer;
    int rc = times(&tmsBuffer);
    BSLS_ASSERT(-1 != rc);
    int startTime = tmsBuffer.tms_utime + tmsBuffer.tms_stime;

    double x = 1.0;
    do {
        x /= 0.999999;
        x -= 0.000001;
        rc = times(&tmsBuffer);
        BSLS_ASSERT(-1 != rc);
    } while (tmsBuffer.tms_utime + tmsBuffer.tms_stime - startTime < 50);

    return x;
#else
    bslmt::ThreadUtil::microSleep(0, 1);
    return 1.0;
#endif
}

long long controlledCpuBurn()
{
    volatile long long factorial = 1;
    bsls::TimeInterval begin = bdlt::CurrentTime::now();

    while (1) {

        for (int i = 1; i <= 20; ++i) {
            factorial *= i;
        }

        if ((bdlt::CurrentTime::now() - begin).totalSecondsAsDouble() > 1.0) {
            break;
        }
    }
    return factorial;
}

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // CONCERN: Statistics are Reset Correctly
        //
        // Concerns:
        //:  1 That CPU statics are reset to zero after calling
        //:    'resetStatistics'.
        //
        // Plan:
        //:  1 Create a performance monitor, collect statistics, reset the
        //:    statics, and verify that the returned statistics after the reset
        //:    are plausible (Note that drqs 49280976, reset statistics lead to
        //:    incorrectly determining the time since the last reset, and
        //:    wildly incorrect statics after a reset).
        //
        // Testing:
        //   CONCERN: Statistics are Reset Correctly (DRQS 49280976)
        // --------------------------------------------------------------------

        if (verbose) cout << "CONCERN: Statistics are Reset Correctly\n"
                          << "=======================================\n";

        if (verbose) cout << "\tPerform a reset and verify metrics"
                          << " are set to 0\n";
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);

            balb::PerformanceMonitor perfmon(&ta);

            int pid = bdls::ProcessUtil::getProcessId();

            int rc = perfmon.registerPid(pid, "perfmon");
            ASSERT(0 == rc);

            // Because of how CPU utilization is collected, the first collect
            // is always used as a baseline and hence is ignored
            perfmon.collect();

            balb::PerformanceMonitor::ConstIterator perfmonIter =
                                                             perfmon.find(pid);
            ASSERT(perfmonIter != perfmon.end());

            double userCpu = 0, systemCpu = 0, totalCpu = 0;

            // Burn some CPU
            controlledCpuBurn();
            perfmon.collect();

            userCpu = perfmonIter->avgValue(
                                    balb::PerformanceMonitor::e_CPU_UTIL_USER);
            systemCpu = perfmonIter->avgValue(
                                  balb::PerformanceMonitor::e_CPU_UTIL_SYSTEM);
            totalCpu = perfmonIter->avgValue(
                                         balb::PerformanceMonitor::e_CPU_UTIL);

            if (verbose) {
                cout << "  userCpu = " << userCpu
                     << ", systemCpu = " << systemCpu
                     << ", totalCpu = " << totalCpu
                     << endl;
            }

            // Reset our statistics
            perfmon.resetStatistics();

            // At this point, the average CPU utilizations should be zero

            // Sleep for 1 second (without consuming too much CPU)
            bslmt::ThreadUtil::microSleep(0, 1);
            perfmon.collect();

            // We called collect almost a second after restting statistics, so
            // our CPU utilization should be really low

            userCpu = perfmonIter->avgValue(
                                    balb::PerformanceMonitor::e_CPU_UTIL_USER);
            systemCpu = perfmonIter->avgValue(
                                  balb::PerformanceMonitor::e_CPU_UTIL_SYSTEM);
            totalCpu = perfmonIter->avgValue(
                                         balb::PerformanceMonitor::e_CPU_UTIL);

            if (verbose) {
                cout << "  userCpu = " << userCpu
                     << ", systemCpu = " << systemCpu
                     << ", totalCpu = " << totalCpu
                     << endl;
            }

            ASSERT(userCpu < 10);
            ASSERT(systemCpu < 10);
            ASSERT(totalCpu < 10);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PROCESS START TIME SANITY
        //
        // Concerns:
        //:  1 It appears that for some versions of Linux, either different
        //:    kernels or different distros, our estimation of process start
        //:    time will be wildly inaccurate.  Determine whether this is the
        //:    case.
        //
        // Testing:
        //   CONCERN: The Process Start Time is Reasonable
        // --------------------------------------------------------------------

        if (verbose) cout << "PROCESS START TIME SANITY\n"
                             "=========================\n";

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        balb::PerformanceMonitor perfmon(&ta);

        int rc = perfmon.registerPid(0, "perfmon");
        ASSERT(0 == rc);

        // process start time
        bdlt::Datetime st = perfmon.begin()->startupTime();

        bdlt::Datetime nowDt(1970, 1, 1);
        nowDt.addSeconds(static_cast<int>(
                             bdlt::CurrentTime::now().totalSecondsAsDouble()));

        bdlt::DatetimeInterval diff = nowDt - st;

        ASSERT(diff.totalSeconds() > -10);
        ASSERT(diff.totalSeconds() <  10);
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //:  1 The usage example shown in the component-level documentation
        //:    compiles and executes as expected.
        //
        // Plan:
        //:  1 Implement the test exactly as shown in the example. (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING USAGE EXAMPLE\n"
                             "=====================\n";

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        {
            // Instantiate a scheduler used by the performance monitor to
            // schedule collection events.
            bdlmt::TimerEventScheduler scheduler;
            scheduler.start();

            // Create the performance monitor, monitoring the current process
            // and auto-collecting statistics every second.
            balb::PerformanceMonitor perfmon(&scheduler, 1.0, &ta);

            if (0 != perfmon.registerPid(0, "mytask")) {
                ASSERT(!"Failed to register process with performance monitor");
                return testStatus;                                    // RETURN
            }

            // Print a formatted report of the performance statistics every 10
            // seconds for one minute.  Note that the report interval may vary
            // from the collection interval.
            for (int i = 0; i < 6; ++i) {
                bslmt::ThreadUtil::microSleep(0, 1);
                for (balb::PerformanceMonitor::ConstIterator
                                                         it  = perfmon.begin();
                                                         it != perfmon.end();
                                                       ++it)
                {
                    const balb::PerformanceMonitor::Statistics& stats = *it;

                    if (veryVerbose) {
                        bsl::cout << "Pid = " << stats.pid() << ":\n";
                        stats.print(bsl::cout);
                    }
                }
            }

            scheduler.stop();
        }
        ASSERT(0  < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'numRegisteredPids'
        //
        // Concerns:
        //:  1 'numRegisteredPids' reports the correct number of registered
        //:    processes.
        //:
        //:  2 'numRegisterPids' reports 0 before any processes are registered.
        //:
        //:  3 'numRegisterPids' increments by 1 every time a process is
        //:    registered.
        //:
        //:  4 'numRegisterPids' decrements by 1 every time a process is
        //:    un-registered.
        //
        // Plan:
        //:  1 Using the ad-doc approach, monitor this test driver process, and
        //:    one or more child processes, spawned as necessary, and check the
        //:    value returned by 'numRegisteredPids' before and after each
        //:    process is registered.  (C-1..3)
        //:
        //:  2 Un-register processes registered in step 1, and check the value
        //:    returned by 'numRegisteredPids'.  (C-1,4)
        //
        // Testing:
        //   int numRegisteredPids()
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'numRegisteredPids'\n"
                          << "===========================\n";

        bdlmt::TimerEventScheduler scheduler;
        balb::PerformanceMonitor   perfmon(&scheduler, 1.0);

        ASSERT(0 == perfmon.numRegisteredPids());

        // Register this test driver process.

        ASSERT(0 == perfmon.registerPid(0, "mytask"));
        ASSERT(1 == perfmon.numRegisteredPids());

        // Spawn as a child process another copy of this test driver, running
        // test case -3, which simply sleeps for one minute and exits.

        bsl::string              command(argv[0]);
        bsl::vector<bsl::string> arguments;

        arguments.push_back(bsl::string("-3"));

        if (veryVeryVerbose) {
            T_ P(command)
            for (bsl::vector<bsl::string>::iterator i = arguments.begin();
                 i != arguments.end();
                 ++i) {
                T_ T_ P(*i)
            }

        }

        processSupport::ProcessHandle handle =
                                      processSupport::exec(command, arguments);

        if (veryVeryVerbose) {
            T_ P_(bdls::ProcessUtil::getProcessId())
               P (processSupport::getId(handle))
        }

        // Register the spawned child process.

        int pid = processSupport::getId(handle);

        ASSERTV(pid, -1 != pid);

        ASSERT(0 == perfmon.registerPid(pid, "mytask2"));
        ASSERT(2 == perfmon.numRegisteredPids());

        // Un-register and terminate the child process.

        ASSERT(0 == perfmon.unregisterPid(pid));
        ASSERT(1 == perfmon.numRegisteredPids());

        int rc = processSupport::terminateProcess(handle);
        ASSERTV(rc, 0 == rc);

        ASSERT(0 == perfmon.unregisterPid(0));
        ASSERT(0 == perfmon.numRegisteredPids());
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //:  1 The class is sufficiently functional to enable comprehensive
        //:    testing in subsequent test cases.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                          << "==============\n";

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        {
            {
                balb::PerformanceMonitor perfmon(&ta);
                int                     rc;

                rc = perfmon.registerPid(0, "mytask");
                ASSERT(0 == rc);

                for (int i = 0; i < 6; ++i) {
                    wasteCpuTime();

                    perfmon.collect();

                    for (balb::PerformanceMonitor::ConstIterator
                                                        it  = perfmon.begin();
                                                        it != perfmon.end();
                                                      ++it)
                    {
                        const balb::PerformanceMonitor::Statistics &stats =
                                                                           *it;

                        if (veryVerbose) {
                            bsl::cout << "Pid = " << stats.pid() << ":\n";
                            stats.print(bsl::cout);
                        }
                    }
                }
            }

            {
                bdlmt::TimerEventScheduler scheduler;
                scheduler.start();

                balb::PerformanceMonitor perfmon(&scheduler, 1.0, &ta);
                int                     rc;

                rc = perfmon.registerPid(0, "mytask");
                ASSERT(0 == rc);

                for (int i = 0; i < 6; ++i) {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    for (balb::PerformanceMonitor::ConstIterator
                                                        it  = perfmon.begin();
                                                        it != perfmon.end();
                                                      ++it)
                    {
                        const balb::PerformanceMonitor::Statistics& stats =
                                                                           *it;

                        if (veryVerbose) {
                            bsl::cout << "Pid = " << stats.pid() << ":\n";
                            stats.print(bsl::cout);
                        }
                    }
                }

                scheduler.stop();
            }
        }
        ASSERT(0  < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
#ifndef BSLS_PLATFORM_OS_WINDOWS
      case -1:
      case -2: {
        // --------------------------------------------------------------------
        // TESTING VIRTUAL SIZE AND RESIDENT SIZE
        //
        // Testing:
        //   TESTING VIRTUAL SIZE AND RESIDENT SIZE
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING VIRTUAL SIZE AND RESIDENT SIZE\n"
                          << "======================================\n";

        bslma::NewDeleteAllocator newDeleteAllocator;
        test::MmapAllocator       mmapAllocator;

        bslma::Allocator *allocator;
        switch (test) {
        case -1: allocator = &newDeleteAllocator; break;
        case -2: allocator = &mmapAllocator;      break;
        }

        bslma::TestAllocator ta(veryVeryVeryVerbose, allocator);

        bslma::Default::setDefaultAllocator(&ta);
        bslma::Default::setGlobalAllocator(&ta);

        {
            balb::PerformanceMonitor perfmon(&ta);

            int                     bufferSize        = 0;

            double                  virtualSize       = 0;
            double                  residentSize      = 0;

            bsls::Types::Int64      currentBytesInUse = 0;
            bsls::Types::Int64      peakBytesInUse    = 0;

            balb::PerformanceMonitor::Measure virtualSizeMeasure =
                                  balb::PerformanceMonitor::e_VIRTUAL_SIZE;

            balb::PerformanceMonitor::Measure residentSizeMeasure =
                                  balb::PerformanceMonitor::e_RESIDENT_SIZE;

            perfmon.registerPid(0, "test");

            bufferSize = 1024 * 1024;

            for (int i = 0; i < 8; ++i) {

                {
                    bsl::deque<char> buffer(&ta);
                    buffer.resize(bufferSize);

                    perfmon.collect();

                    virtualSize =
                        perfmon.begin()->latestValue(virtualSizeMeasure);

                    residentSize =
                        perfmon.begin()->latestValue(residentSizeMeasure);

                    currentBytesInUse = ta.numBytesInUse();
                    peakBytesInUse    = bsl::max(peakBytesInUse,
                                                 ta.numBytesInUse());

                    bsl::cout << "--"
                              << bsl::endl;

                    report(bufferSize,
                           currentBytesInUse,
                           peakBytesInUse,
                           virtualSize,
                           residentSize);
                }

                bsl::size_t halfBufferSize = bufferSize / 2;

                {
                    bsl::deque<char> buffer(&ta);
                    buffer.resize(halfBufferSize);

                    perfmon.collect();

                    virtualSize =
                        perfmon.begin()->latestValue(virtualSizeMeasure);

                    residentSize =
                        perfmon.begin()->latestValue(residentSizeMeasure);

                    currentBytesInUse = ta.numBytesInUse();
                    peakBytesInUse    = bsl::max(peakBytesInUse,
                                                 ta.numBytesInUse());

                    bsl::cout << "**"
                              << bsl::endl;

                    report(halfBufferSize,
                           currentBytesInUse,
                           peakBytesInUse,
                           virtualSize,
                           residentSize);
                }

                bufferSize *= 2;
            }

            perfmon.collect();

            virtualSize =
                perfmon.begin()->latestValue(virtualSizeMeasure);

            residentSize =
                perfmon.begin()->latestValue(residentSizeMeasure);

            currentBytesInUse = ta.numBytesInUse();
            peakBytesInUse    = bsl::max(peakBytesInUse, ta.numBytesInUse());

            bsl::cout << "##"
                      << bsl::endl;

            report(0,
                   currentBytesInUse,
                   peakBytesInUse,
                   virtualSize,
                   residentSize);
        }
        ASSERT(0  < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
#endif
      case -3: {
        // --------------------------------------------------------------------
        // DUMMY TEST CASE
        //   Sleep for 60 seconds.
        //
        //   This test case provides a target for spawning a child process in
        //   test case 2.
        //
        // Concerns:
        //:  1 Test runs for at least 60 seconds.
        //
        // Plan:
        //:  1 Confirm empirically by observing a test run.  (C-1)
        //
        // Testing:
        //   DUMMY TEST CASE
        // --------------------------------------------------------------------

        if (verbose) cout << "DUMMY TEST CASE\n"
                          << "===============\n";

        bslmt::ThreadUtil::microSleep(0, 60);
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

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
