// balb_performancemonitor.t.cpp                                      -*-C++-*-
#include <balb_performancemonitor.h>

#include <bdls_filesystemutil.h>
#include <bdls_processutil.h>

#include <bdlt_currenttime.h>
#include <bdlt_datetimeinterval.h>

#include <bsla_maybeunused.h>

#include <bslmt_barrier.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bsls_atomic.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cfloat.h>
#include <bsl_climits.h>
#include <bsl_cmath.h>
#include <bsl_cstdlib.h>
#include <bsl_deque.h>
#include <bsl_fstream.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_set.h>
#include <bsl_sstream.h>
#include <bsl_stdexcept.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/times.h>
#endif

#if defined(BSLS_PLATFORM_OS_SOLARIS)
#include <procfs.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#include <windows.h>
#else
#include <bsl_c_errno.h>
#include <bsl_c_signal.h>
#include <unistd.h>
#include <sys/wait.h>
#endif

// DRQS 170291732: This component cannot be built in 32-bit mode using g++ on
// Solaris
#if (defined(BSLS_PLATFORM_OS_SOLARIS)   \
       && defined(BSLS_PLATFORM_CMP_GNU) \
       && defined(BSLS_PLATFORM_CPU_32_BIT) \
       && BSLS_PLATFORM_CMP_VERSION < 120000)
int main()
{
    return -1;
}
#else

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
// Thread-safety concerns are not tested because of limitations of test
// apparatus and component's design.
// ----------------------------------------------------------------------------
//                            // ----------------
//                            // class Statistics
//                            // ----------------
// CREATORS
// [20] Statistics(Allocator *basicAllocator);
// [10] Statistics(const Statistics& orig, Allocator *basicAllocator);
//
// MANIPULATORS
// [20] void reset();
//
// ACCESSORS
// [19] double minValue(Measure measure) const;
// [19] double maxValue(Measure measure) const;
// [19] double avgValue(Measure measure) const;
// [16] double elapsedTime() const;
// [16] const bdlt::Datetime& startupTime() const;
// [20] void print(bsl::ostream& os) const;
// [20] void print(bsl::ostream& os, Measure measure) const;
// [20] void print(bsl::ostream& os, const char *measureIdentifier) const;
//
//                           // -------------------
//                           // class ConstIterator
//                           // -------------------
// CREATORS
// [15] ConstIterator();
//
// CLASS METHODS
// [ 6] ConstIterator& operator++();
// [ 6] ConstIterator operator++(int);
// [ 4] reference operator*() const;
// [ 4] pointer operator->() const;
// [ 7] bool operator==(const ConstIterator& rhs) const;
// [ 7] bool operator!=(const ConstIterator& rhs) const;
//
//                         // ------------------------
//                         // class PerformanceMonitor
//                         // ------------------------
// CREATORS
// [14] PerformanceMonitor(TimerEventScheduler *, double, Allocator *);
// [14] ~PerformanceMonitor();
//
// MANIPULATORS
// [15] int registerPid(int pid, const bsl::string& description);
// [15] int unregisterPid(int pid);
// [14] void setCollectionInterval(double interval);
// [19] void resetStatistics();
//
// CLASS METHODS
// [ 4] ConstIterator begin() const;
// [ 8] ConstIterator end() const;
// [ 9] ConstIterator find(int pid) const;
// [ 2] int numRegisteredPids() const
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [22] USAGE EXAMPLE
// [ 3] CONCERN: The Process Start Time is Reasonable
// [ 5] CONCERN: Statistics are Reset Correctly (DRQS 49280976)
// [12] CONCERN: Initial values of statistics (DRQS 180602553)
// [13] CONCERN: CPU times and start time are those of the process
// [17] CONCERN: Memory, thread count, and page fault measures
// [18] CONCERN: CPU utilization
// [21] CONCERN: Thread safety
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

typedef balb::PerformanceMonitor                Obj;
typedef balb::PerformanceMonitor::ConstIterator ObjIterator;

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

// Define `bsl::string` value long enough to ensure dynamic memory allocation.

#ifdef BSLS_PLATFORM_CPU_32_BIT
#define SUFFICIENTLY_LONG_STRING "123456789012345678901234567890123"
#else  // 64_BIT
#define SUFFICIENTLY_LONG_STRING "12345678901234567890123456789012" \
                                 "123456789012345678901234567890123"
#endif

BSLMF_ASSERT(sizeof SUFFICIENTLY_LONG_STRING > sizeof(bsl::string));

const char *const LONG_STRING    = "a_"   SUFFICIENTLY_LONG_STRING;

// ============================================================================
//                       HELPER FUNCTIONS AND CLASSES
// ----------------------------------------------------------------------------
namespace {

/// The structure to store values accessible via public interfaces of
/// `balb::PerformanceMonitor::Statistics` class.
struct StatisticsStorage
{
    int            d_pid;                           // process identifier

    bsl::string    d_description;                   // process description

    bdlt::Datetime d_startTimeUtc;                  // process start time, in
                                                    // UTC time

    double         d_elapsedTime;                   // time elapsed since
                                                    // process startup

    double         d_lstData[Obj::e_NUM_MEASURES];  // latest collected data

    double         d_minData[Obj::e_NUM_MEASURES];  // min value

    double         d_maxData[Obj::e_NUM_MEASURES];  // max value
};

}  // close unnamed namespace

namespace processSupport {

#ifdef BSLS_PLATFORM_OS_UNIX

typedef int ProcessHandle;

/// Execute in a child process the program located at the specified
/// `command` path, with the specified `arguments`.  Return a
/// `ProcessHandle` object identifying the child process.
ProcessHandle exec(bsl::string command, bsl::vector<bsl::string> arguments)
{
    typedef bsl::vector<bsl::string> Args;

    ProcessHandle handle;
    handle = fork();

    if (0 == handle) {
        // child process

        bsl::vector<char *>  argvec;

        argvec.push_back(&command[0]);     // N.B. Assumes that `bsl::string`
                                           // contents are always
                                           // null-terminated.
                                           //
                                           // Same assumption applies below.

        for (Args::iterator i = arguments.begin(); i != arguments.end(); ++i) {
            argvec.push_back(&(*i)[0]);
        }
        argvec.push_back(0);

        execv(argvec[0], argvec.data());

        BSLS_ASSERT_OPT_UNREACHABLE("execv failed");
    }

    return handle;
}

/// Return the integer value of the process ID associated with the process
/// identified by the specified `handle`.
int getId(const ProcessHandle& handle)
{
    return handle;
}

/// Terminate the process identified by the specified `handle` and wait for
/// its exit.  Return `0` on success and a non-zero value on failure.
int terminateProcess(const ProcessHandle& handle)
{
    if (kill(handle, SIGTERM)) {
        return errno;                                                 // RETURN
    }

    pid_t rc;
    do {
        rc = waitpid(handle, 0, 0);
    } while (rc == -1 && errno == EINTR);

    return rc == handle ? 0 : errno;
}

#elif BSLS_PLATFORM_OS_WINDOWS

typedef PROCESS_INFORMATION ProcessHandle;

// Execute in a child process the program located at the specified
// `command` path, with the specified `arguments`.  Return a
// `ProcessHandle` object identifying the child process.
ProcessHandle exec(bsl::string command, bsl::vector<bsl::string> arguments)
{
    BSLMF_ASSERT(sizeof(DWORD) == sizeof(int));

    typedef bsl::vector<bsl::string> Args;

    STARTUPINFO sui;
    GetStartupInfo(&sui);

    // Need to have `PROCESS_QUERY_INFORMATION` for `GetExitCodeProcess`
    // and `PROCESS_TERMINATE` for `TerminateProcess`.
    //
    // Empirically, these permissions seem to be granted for child processes.

    ProcessHandle handle;

    const char *app = command.data();

    bsl::string commandLine;
    commandLine.append(app);
    for (Args::iterator i = arguments.begin(); i != arguments.end(); ++i) {
        commandLine.append(" ");
        commandLine.append(*i);
    }

    // N.B. The `lpCommandLine` argument of `CreateProcess` must be mutable,
    // per requirements of `CreateProcessEx` (cited in documentation for
    // `CreateProcess`).
    //
    // https://msdn.microsoft.com/en-us/library/ms682425.aspx

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
        // Following the behavior of UNIX `fork`, failure to create process is
        // indicated by `-1 == getId(handle)`,

        handle.dwProcessId = static_cast<DWORD>(-1);
    }

    return handle;
}

// Return the integer value of the process ID associated with the process
// identified by the specified `handle`.
int getId(const ProcessHandle& handle)
{
    BSLMF_ASSERT(sizeof(DWORD) == sizeof(int));

    return static_cast<int>(handle.dwProcessId);
}

// Terminate the process identified by the specified `handle` and wait for
// its exit.  Return `0` on success and a non-zero value on failure.
int terminateProcess(const ProcessHandle& handle)
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

    const bool exited = WAIT_OBJECT_0 == WaitForSingleObject(handle.hProcess,
                                                            INFINITE);

    if (! (CloseHandle(handle.hProcess) && CloseHandle(handle.hThread))) {
            return -1;                                                // RETURN
    }

    return exited ? 0 : -1;
}

#else
#error "Unknown OS type."
#endif

}  // close namespace processSupport

#ifndef BSLS_PLATFORM_OS_WINDOWS
namespace test {

class MmapAllocator : public bslma::Allocator {

    typedef bsl::map<void*, bsl::size_t> MapType;

    MapType           d_map;
    bslma::Allocator *d_allocator_p;

  private:
    // NOT IMPLEMENTED
    MmapAllocator(const MmapAllocator &);             // = deleted
    MmapAllocator& operator=(const MmapAllocator &);  // = deleted

  public:
    // CREATOR
    explicit
    MmapAllocator(bslma::Allocator *basicAllocator = 0)
    : d_map(basicAllocator)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    // MANIPULATORS
    void *allocate(size_type size) BSLS_KEYWORD_OVERRIDE
    {
#if defined(BSLS_PLATFORM_OS_SOLARIS)
        // Run `pmap -xs <pid>` to see a tabular description of the memory
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

#elif defined(BSLS_PLATFORM_OS_LINUX)      \
   || defined(BSLS_PLATFORM_OS_DARWIN)     \
   || defined(BSLS_PLATFORM_OS_CYGWIN)     \
   || defined(BSLS_PLATFORM_OS_EMSCRIPTEN)
        return d_allocator_p->allocate(size);
#else
#error Not implemented.
#endif
    }

    void deallocate(void *address) BSLS_KEYWORD_OVERRIDE
    {
#if defined(BSLS_PLATFORM_OS_LINUX)      \
 || defined(BSLS_PLATFORM_OS_DARWIN)     \
 || defined(BSLS_PLATFORM_OS_CYGWIN)     \
 || defined(BSLS_PLATFORM_OS_EMSCRIPTEN)
        d_allocator_p->deallocate(address);
#else
        MapType::iterator it = d_map.find(address);
        BSLS_ASSERT(it != d_map.end());

        bsl::size_t size = it->second;

        BSLA_MAYBE_UNUSED int rc = munmap(static_cast<char*>(address), size);
        BSLS_ASSERT(-1 != rc);
#endif
    }
};

}  // close namespace test
#endif

namespace {
#ifndef BSLS_PLATFORM_OS_WINDOWS
void report(bsl::size_t        bufferSize,
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

/// Just take up a measurable amount of cpu time.  Try 100 clock ticks (1.0
/// seconds or less).
double wasteCpuTime()
{
#ifdef BSLS_PLATFORM_OS_UNIX
    struct tms tmsBuffer;

    BSLA_MAYBE_UNUSED clock_t rc = times(&tmsBuffer);
    BSLS_ASSERT(-1 != rc);

    clock_t startTime = tmsBuffer.tms_utime + tmsBuffer.tms_stime;

    double x = 1.0;
    do {
        x /= 0.999999;
        x -= 0.000001;
        rc = times(&tmsBuffer);
        BSLS_ASSERT(-1 != rc);
    } while (tmsBuffer.tms_utime + tmsBuffer.tms_stime - startTime < 20);

    return x;
#else
    bslmt::ThreadUtil::microSleep(100000, 0);
    return 1.0;
#endif
}

bsls::Types::Int64 controlledCpuBurn()
{
    bsls::Types::Uint64 factorial = 1;
    bsls::TimeInterval begin = bdlt::CurrentTime::now();

    while (1) {
        for (unsigned x = 1; x <= 20; ++x) {
            factorial *= x;
        }

        if ((bdlt::CurrentTime::now() - begin).totalSecondsAsDouble() > 1.0) {
            break;
        }
    }
    return factorial;
}

/// Return a copy of the specified `begin`, incremented by the specified `n`
/// elements.
ObjIterator advanceIt(const ObjIterator& begin, int n)
{
    ObjIterator ret = begin;
    for (int i = 0; i < n; ++i) {
        ++ret;
    }

    return ret;
}

/// Force both the user and system time counters to advance by performing I/O
/// and an expensive computation that can't be optimized.  (This isn't
/// guaranteed to work, so don't rely on it.)
void consumeUserAndSystemTime()
{
    bsl::string tmpFileName;

    bdls::FilesystemUtil::FileDescriptor tmpFile =
                  bdls::FilesystemUtil::createTemporaryFile(&tmpFileName, ".");

    unsigned int x = bdls::ProcessUtil::getProcessId();

    for (int i = 0; i < 100000; i++) {
        bdls::FilesystemUtil::write(tmpFile, &x, sizeof x);
        bdls::FilesystemUtil::seek(
                                  tmpFile,
                                  0,
                                  bdls::FilesystemUtil::e_SEEK_FROM_BEGINNING);
        bdls::FilesystemUtil::read(tmpFile, &x, sizeof x);
        for (int j = 0; j < 1000; ++j) {
            if (x % 2 == 0) x /= 2; else x = 3*x + 1;
        }
    }

    bdls::FilesystemUtil::close(tmpFile);
    bdls::FilesystemUtil::remove(tmpFileName);
}

/// Consume user time by performing an expensive computation, without I/O,
/// that can't be optimized.
void consumeUserTime()
{
    static bsls::AtomicUint sink(0);

    unsigned int x = bdls::ProcessUtil::getProcessId();

    for (int i = 0; i < 10000000; ++i) {
        if (x % 2 == 0) x /= 2; else x = 3*x + 1;
    }

    sink.storeRelaxed(x);
}

/// Load into the specified `userTime` and `systemTime` the user and system
/// CPU time, in seconds, consumed so far by this process, as reported directly
/// by the operating system.
void getOwnCpuTimes(double *userTime, double *systemTime)
{
#if defined(BSLS_PLATFORM_OS_WINDOWS)
    FILETIME creationTime, exitTime, kernelTime, userTimeFt;
    ASSERT(GetProcessTimes(GetCurrentProcess(),
                           &creationTime,
                           &exitTime,
                           &kernelTime,
                           &userTimeFt));

    ULARGE_INTEGER k = { kernelTime.dwLowDateTime, kernelTime.dwHighDateTime };
    ULARGE_INTEGER u = { userTimeFt.dwLowDateTime, userTimeFt.dwHighDateTime };

    *userTime   = static_cast<double>(u.QuadPart) / 1e7;
    *systemTime = static_cast<double>(k.QuadPart) / 1e7;
#else
    struct rusage usage;
    ASSERT(0 == getrusage(RUSAGE_SELF, &usage));

    *userTime   = static_cast<double>(usage.ru_utime.tv_sec)
                + static_cast<double>(usage.ru_utime.tv_usec) / 1e6;
    *systemTime = static_cast<double>(usage.ru_stime.tv_sec)
                + static_cast<double>(usage.ru_stime.tv_usec) / 1e6;
#endif
}

/// Capture native CPU times for an independent utilization calculation,
/// including on hosts that limit the CPU time available to us.
struct CpuSample {
    double d_user;
    double d_system;

    CpuSample()
    {
        getOwnCpuTimes(&d_user, &d_system);
    }
};

/// Return the native CPU time, in seconds, consumed between the specified
/// `before` and `after` samples.
double cpuTime(const CpuSample& before, const CpuSample& after)
{
    return after.d_user - before.d_user + after.d_system - before.d_system;
}

#if defined(BSLS_PLATFORM_OS_SOLARIS)
/// Return the native page-fault count for this process, or -1 if it cannot be
/// read.  Note that Solaris does not count every first touch of fresh
/// anonymous memory as a page fault.
double getOwnPageFaults()
{
    bsl::ostringstream path;
    path << "/proc/" << bdls::ProcessUtil::getProcessId() << "/usage";
    bsl::ifstream input(path.str().c_str(), bsl::ios::binary);
    prusage_t usage;
    input.read(reinterpret_cast<char *>(&usage), sizeof usage);
    ASSERT(input);
    if (!input) {
        return -1;                                                    // RETURN
    }
    return static_cast<double>(usage.pr_minf + usage.pr_majf);
}
#endif

bool isCumulative(int measure)
{
    return measure == Obj::e_CPU_TIME ||
           measure == Obj::e_CPU_TIME_USER ||
           measure == Obj::e_CPU_TIME_SYSTEM ||
           measure == Obj::e_NUM_PAGEFAULTS;
}

bool isRate(int measure)
{
    return measure == Obj::e_CPU_UTIL ||
           measure == Obj::e_CPU_UTIL_USER ||
           measure == Obj::e_CPU_UTIL_SYSTEM;
}

/// Consume CPU time on the calling thread for at least the specified
/// `seconds` of wall time.
void consumeCpu(double seconds)
{
    const double start = bdlt::CurrentTime::now().totalSecondsAsDouble();
    while (bdlt::CurrentTime::now().totalSecondsAsDouble() - start < seconds) {
        consumeUserTime();
    }
}

/// Return the whitespace-separated tokens of the specified `line`.
bsl::vector<bsl::string> tokenize(const bsl::string& line)
{
    bsl::vector<bsl::string> tokens;
    bsl::istringstream       iss(line);
    bsl::string              token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

/// Load into the specified `numNonRate` and `numRate` the number of non-rate
/// and rate samples collected in the specified `stats`, as reported by
/// `print`.  Note that `print` ends each line with the two numbers of
/// samples, the latest value, and three more values (or "N/A").
void getNumSamples(int *numNonRate, int *numRate, const Obj::Statistics& stats)
{
    bsl::ostringstream oss;
    stats.print(oss, Obj::e_NUM_THREADS);

    const bsl::vector<bsl::string> tokens = tokenize(oss.str());
    ASSERTV(oss.str(), 6 <= tokens.size());
    if (6 > tokens.size()) {
        *numNonRate = *numRate = -1;
        return;                                                       // RETURN
    }

    *numNonRate = bsl::atoi(tokens[tokens.size() - 6].c_str());
    *numRate    = bsl::atoi(tokens[tokens.size() - 5].c_str());
}

/// Return the number of non-rate samples collected in the specified `stats`.
int numSamples(const Obj::Statistics& stats)
{
    int numNonRate, numRate;
    getNumSamples(&numNonRate, &numRate, stats);
    return numNonRate;
}

/// Wait up to the specified `timeout` seconds for at least the specified
/// `count` non-rate samples to be collected in the specified `stats`.
/// Return `true` if they were, and `false` otherwise.
bool waitForSamples(const Obj::Statistics& stats, int count, double timeout)
{
    const double start = bdlt::CurrentTime::now().totalSecondsAsDouble();
    while (numSamples(stats) < count) {
        if (bdlt::CurrentTime::now().totalSecondsAsDouble() - start >
                                                                    timeout) {
            return false;                                             // RETURN
        }
        bslmt::ThreadUtil::microSleep(10000, 0);
    }
    return true;
}

/// Spawn a copy of the test driver at the specified `command` path, running
/// test case -3, which sleeps for half a minute without consuming CPU time,
/// and return a handle to the child process.
processSupport::ProcessHandle spawnSleeper(const bsl::string& command)
{
    bsl::vector<bsl::string> arguments;
    arguments.push_back(bsl::string("-3"));

    return processSupport::exec(command, arguments);
}

/// Wait up to the specified `timeout` seconds for the specified `counter` to
/// reach the specified `count`.  Return `true` if it did, and `false`
/// otherwise.
bool waitForCount(const bsls::AtomicInt& counter, int count, double timeout)
{
    const double start = bdlt::CurrentTime::now().totalSecondsAsDouble();
    while (counter < count) {
        if (bdlt::CurrentTime::now().totalSecondsAsDouble() - start >
                                                                    timeout) {
            return false;                                             // RETURN
        }
        bslmt::ThreadUtil::microSleep(1000, 0);
    }
    return true;
}

/// Consume CPU time for `d_seconds` of wall time (none if 0), increment
/// `*d_numReady_p`, and wait until `*d_release_p` is `true`, so that the
/// thread exists while the test collects.  Note that, unlike a barrier, this
/// does not hang the test if fewer threads start than expected.
struct ParkedThread {
    double            d_seconds;
    bsls::AtomicInt  *d_numReady_p;
    bsls::AtomicBool *d_release_p;

    void operator()() const
    {
        consumeCpu(d_seconds);
        ++*d_numReady_p;
        while (!*d_release_p) {
            bslmt::ThreadUtil::microSleep(1000, 0);
        }
    }
};

void ignoreClock()
{
}

/// Set the collection interval after waiting on the specified barriers.
struct CollectionIntervalSetter {
    Obj            *d_monitor_p;
    bslmt::Barrier *d_entered_p;
    bslmt::Barrier *d_release_p;
    double          d_interval;

    void operator()() const
    {
        if (d_entered_p) {
            d_entered_p->wait();
        }
        if (d_release_p) {
            d_release_p->wait();
        }
        d_monitor_p->setCollectionInterval(d_interval);
    }
};

/// Repeatedly perform one operation on a performance monitor until told to
/// stop, in order to exercise its thread safety.
struct MonitorExerciser {
    enum Operation {
        e_COLLECT,
        e_RESET,
        e_READ,
        e_REGISTER,
        e_SET_INTERVAL
    };

    Obj              *d_monitor_p;
    Operation         d_operation;
    int               d_childPid;
    bsls::AtomicBool *d_done_p;

    void operator()() const
    {
        const int selfPid = bdls::ProcessUtil::getProcessId();

        while (!*d_done_p) {
            switch (d_operation) {
              case e_COLLECT: {
                d_monitor_p->collect();
              } break;
              case e_RESET: {
                d_monitor_p->resetStatistics();
                bslmt::ThreadUtil::microSleep(1000, 0);
              } break;
              case e_READ: {
                const Obj::Statistics copy(*d_monitor_p->find(0));
                ASSERTV(copy.pid(), selfPid == copy.pid());
                for (int i = 0; i < Obj::e_NUM_MEASURES; ++i) {
                    const Obj::Measure m = static_cast<Obj::Measure>(i);
                    ASSERTV(i, copy.latestValue(m) >= 0);
                    (void)copy.minValue(m);
                    (void)copy.maxValue(m);
                    (void)copy.avgValue(m);
                }
                bsl::ostringstream oss;
                copy.print(oss);
              } break;
              case e_REGISTER: {
                if (0 == d_monitor_p->registerPid(d_childPid, "child")) {
                    ASSERT(0 == d_monitor_p->unregisterPid(d_childPid));
                }
                bslmt::ThreadUtil::microSleep(1000, 0);
              } break;
              case e_SET_INTERVAL: {
                d_monitor_p->setCollectionInterval(0.001);
                bslmt::ThreadUtil::microSleep(5000, 0);
                d_monitor_p->setCollectionInterval(0);
              } break;
            }
        }
    }
};

}  // close unnamed namespace

#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_CYGWIN)

namespace BetterParsingTest {

typedef balb::PerformanceMonitor_LinuxProcStatistics ProcStats;

/// Test parsing the specified `procStatStr`, expected to have a process id
/// matching the specified `pid`, expected to have a `comm` field matching
/// the optionally specified `expComm`, and where the return value of the
/// parse match 0 or not depending upon the optionally specified
/// `expRcZero`.
void testParseProcStatStr(const bsl::string&  procStatStr,
                          int                 pid,
                          const char         *expComm = 0,
                          bool                expRcZero = true)
{
    static const long clockTicksPerSec = sysconf(_SC_CLK_TCK);
    static const long pageSize         = sysconf(_SC_PAGESIZE);

    ProcStats procStats;
    int rc = procStats.parseProcStatString(procStatStr, pid);
    ASSERTV(expRcZero, rc, expRcZero == !rc);

    if (!expRcZero) {
        return;                                                       // RETURN
    }

    if (expComm) {
        ASSERT(expComm == procStats.d_comm);
    }

    const double cpuTimeU = static_cast<double>(procStats.d_utime) /
                            static_cast<double>(clockTicksPerSec);
    const double cpuTimeS = static_cast<double>(procStats.d_stime) /
                            static_cast<double>(clockTicksPerSec);

    ASSERTV(cpuTimeU, cpuTimeU < 3);

    ASSERTV(cpuTimeS, cpuTimeS < 1);

    const double resSize = static_cast<double>(procStats.d_rss)
                         * static_cast<double>(pageSize)
                         / (1024 * 1024);

    ASSERT(0.1 < resSize);

    const double virtSize =
                        static_cast<double>(procStats.d_vsize) / (1024 * 1024);

    ASSERTV(virtSize, 0.1 < virtSize);
    ASSERTV(resSize, virtSize, resSize <= virtSize);
}

}  // close namespace BetterParsingTest

#endif

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 22: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

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
    bdlmt::TimerEventScheduler scheduler;
    scheduler.start();
// ```
// Then, we create the performance monitor, monitoring the current process and
// auto-collecting statistics every second.
// ```
    balb::PerformanceMonitor perfmon(&scheduler, 1.0);
    int                      rc  = perfmon.registerPid(0, "perfmon");
    const int                pid = bdls::ProcessUtil::getProcessId();

    ASSERT(0 == rc);
    ASSERT(1 == perfmon.numRegisteredPids());
// ```
// Next, we print a formatted report of the performance statistics collected
// for each pid every 5 seconds for half a minute.  Note, that `Statistics`
// object can be simultaneously modified by scheduler callback and accessed via
// a `ConstIterator`.  To ensure that the call to `Statistics::print` outputs
// consistent data from a single update of the statistics for this process, we
// create a local copy (copy construction is guaranteed to be thread-safe).
// ```
    for (int i = 0; i < 6; ++i) {
        bslmt::ThreadUtil::microSleep(0, 5);

        balb::PerformanceMonitor::ConstIterator    it    = perfmon.find(0);
        const balb::PerformanceMonitor::Statistics stats = *it;

        ASSERT(pid == stats.pid());

        bsl::cout << "PID = " << stats.pid() << ":\n";
        stats.print(bsl::cout);
    }
// ```
// Finally, we unregister the process and stop the scheduler to cease
// collecting statistics for this process.  It is safe to call `unregisterPid`
// here, because we don't have any `ConstIterators` objects or references to
// `Statistics` objects.
// ```
    rc  = perfmon.unregisterPid(pid);

    ASSERT(0 == rc);
    ASSERT(0 == perfmon.numRegisteredPids());

    scheduler.stop();
// ```
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // CONCERN: THREAD SAFETY
        //
        // Concerns:
        // 1. `collect`, `resetStatistics`, `registerPid`, `unregisterPid`,
        //    `setCollectionInterval`, `find`, and the `Statistics` copy
        //    constructor and accessors can be called concurrently.
        //
        // Plan:
        // 1. Register this process and a child process with a monitor that
        //    has a scheduler.  For two seconds, in separate threads,
        //    repeatedly call `collect`; `resetStatistics`; copy and read the
        //    statistics of this process; register and unregister another
        //    child process; and start and stop automatic collection.  Verify
        //    that no assertion fails and that the monitor is consistent
        //    afterward.  (Run under a thread sanitizer to detect data races.)
        //    (C-1)
        //
        // Testing:
        //   CONCERN: Thread safety
        // --------------------------------------------------------------------

        if (verbose) cout << "CONCERN: THREAD SAFETY\n"
                             "======================\n";

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        processSupport::ProcessHandle child1 = spawnSleeper(argv[0]);
        processSupport::ProcessHandle child2 = spawnSleeper(argv[0]);

        const int CHILD1 = processSupport::getId(child1);
        const int CHILD2 = processSupport::getId(child2);
        ASSERTV(CHILD1, -1 != CHILD1);
        ASSERTV(CHILD2, -1 != CHILD2);

        {
            bdlmt::TimerEventScheduler scheduler(&ta);
            scheduler.start();

            Obj mX(&scheduler, 0, &ta);  const Obj& X = mX;

            ASSERT(0 == mX.registerPid(0, "self"));
            ASSERT(0 == mX.registerPid(CHILD1, "child1"));

            bsls::AtomicBool done(false);

            const MonitorExerciser::Operation OPERATIONS[] = {
                MonitorExerciser::e_COLLECT,
                MonitorExerciser::e_COLLECT,
                MonitorExerciser::e_RESET,
                MonitorExerciser::e_READ,
                MonitorExerciser::e_READ,
                MonitorExerciser::e_REGISTER,
                MonitorExerciser::e_SET_INTERVAL,
                MonitorExerciser::e_SET_INTERVAL
            };
            const int NUM_OPERATIONS = sizeof OPERATIONS / sizeof *OPERATIONS;

            bslmt::ThreadGroup threads(&ta);
            for (int i = 0; i < NUM_OPERATIONS; ++i) {
                const MonitorExerciser exerciser = { &mX,
                                                     OPERATIONS[i],
                                                     CHILD2,
                                                     &done };
                ASSERTV(i, 0 == threads.addThread(exerciser));
            }

            bslmt::ThreadUtil::microSleep(0, 2);
            done = true;
            threads.joinAll();

            mX.setCollectionInterval(0);

            ASSERTV(X.numRegisteredPids(), 2 == X.numRegisteredPids());
            ASSERT(X.end() != X.find(0));
            ASSERT(X.end() != X.find(CHILD1));
            ASSERT(X.end() == X.find(CHILD2));

            scheduler.stop();
        }
        ASSERTV(ta.numBytesInUse(), 0 == ta.numBytesInUse());

        ASSERT(0 == processSupport::terminateProcess(child1));
        ASSERT(0 == processSupport::terminateProcess(child2));
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING `Statistics` CREATORS, `reset`, AND `print`
        //
        // Concerns:
        // 1. A default-constructed `Statistics` object, and one that has been
        //    `reset`, has the documented initial values.
        //
        // 2. `print(os, measure)` prints one line containing the description,
        //    pid, measure name, numbers of samples, and latest value, followed
        //    by the min, max, and average values, or "N/A" for each if they
        //    are not available.
        //
        // 3. `print(os)` prints one line for each measure, in order.
        //
        // 4. `print(os, identifier)` prints the same as `print(os, measure)`
        //    for the measure named by `identifier`, and nothing if
        //    `identifier` names no measure.
        //
        // Plan:
        // 1. Create a `Statistics` object and verify its accessors.  Call
        //    `reset` and verify them again.  (C-1)
        //
        // 2. Register this process, and verify the output of `print` before
        //    any collection, after one collection, and after two collections.
        //    (C-2..4)
        //
        // Testing:
        //   Statistics(Allocator *basicAllocator);
        //   void reset();
        //   void print(bsl::ostream& os) const;
        //   void print(bsl::ostream& os, Measure measure) const;
        //   void print(bsl::ostream& os, const char *measureIdentifier) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING `Statistics` CREATORS, `reset`, AND"
                             " `print`\n"
                             "==========================================="
                             "========\n";

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        if (verbose) cout << "\tTesting initial values.\n";
        {
            Obj::Statistics mS(&ta);  const Obj::Statistics& S = mS;

            for (int pass = 0; pass < 2; ++pass) {
                ASSERTV(pass, 0   == S.pid());
                ASSERTV(pass, ""  == S.description());
                ASSERTV(pass, 0.0 == S.elapsedTime());

                for (int i = 0; i < Obj::e_NUM_MEASURES; ++i) {
                    const Obj::Measure m = static_cast<Obj::Measure>(i);

                    ASSERTV(pass, i, 0.0      ==  S.latestValue(m));
                    ASSERTV(pass, i, DBL_MAX  ==  S.minValue(m));
                    ASSERTV(pass, i, -DBL_MAX ==  S.maxValue(m));
                }

                mS.reset();
            }
        }
        ASSERTV(ta.numBytesInUse(), 0 == ta.numBytesInUse());

        if (verbose) cout << "\tTesting `print`.\n";
        {
            const char *const TAGS[] = {
                "CPU_TIME",       "CPU_TIME_USER",   "CPU_TIME_SYSTEM",
                "CPU_UTIL",       "CPU_UTIL_USER",   "CPU_UTIL_SYSTEM",
                "RESIDENT_SIZE",  "NUM_THREADS",     "NUM_PAGEFAULTS",
                "VIRTUAL_SIZE"
            };
            BSLMF_ASSERT(Obj::e_NUM_MEASURES == sizeof TAGS / sizeof *TAGS);

            const char *const NAMES[] = {
                "CPU Time",       "User CPU Time",   "System CPU Time",
                "CPU",            "User CPU",        "System CPU",
                "Resident Size",  "Thread Count",    "Page Faults",
                "Virtual Size"
            };
            BSLMF_ASSERT(Obj::e_NUM_MEASURES == sizeof NAMES / sizeof *NAMES);

            Obj mX(&ta);  const Obj& X = mX;
            ASSERT(0 == mX.registerPid(0, "printer"));

            const Obj::Statistics& S   = *X.find(0);
            const int              PID = bdls::ProcessUtil::getProcessId();

            for (int numCollections = 0; numCollections <= 2;
                                                            ++numCollections) {
                if (0 < numCollections) {
                    consumeCpu(1.1);
                    mX.collect();
                }

                int numNonRate, numRate;
                getNumSamples(&numNonRate, &numRate, S);

                if (veryVerbose) {
                    T_ P_(numCollections) P_(numNonRate) P(numRate)
                }

                ASSERTV(numCollections, numNonRate,
                        numCollections == numNonRate);

                bsl::ostringstream all;
                S.print(all);

                bsl::istringstream allLines(all.str());

                for (int i = 0; i < Obj::e_NUM_MEASURES; ++i) {
                    const Obj::Measure m = static_cast<Obj::Measure>(i);

                    bsl::ostringstream byMeasure;
                    S.print(byMeasure, m);

                    bsl::ostringstream byName;
                    S.print(byName, TAGS[i]);

                    bsl::string line;
                    ASSERTV(i, bsl::getline(allLines, line));

                    if (veryVeryVerbose) { T_ T_ P(byMeasure.str()) }

                    ASSERTV(i, line + "\n" == byMeasure.str());
                    ASSERTV(i, byName.str() == byMeasure.str());

                    // The line is: "PERF", description, pid, name (which may
                    // contain spaces), the numbers of non-rate and rate
                    // samples, the latest value, and the min, max, and average
                    // values or "N/A" for each.

                    const bsl::vector<bsl::string> tokens = tokenize(line);
                    const bsl::size_t              N      = tokens.size();

                    ASSERTV(i, line, 10 <= N);
                    if (10 > N) {
                        continue;                                   // CONTINUE
                    }

                    bsl::ostringstream pid;
                    pid << PID;

                    bsl::string name = tokens[3];
                    for (bsl::size_t j = 4; j < N - 6; ++j) {
                        name += ' ' + tokens[j];
                    }

                    bsl::ostringstream samples;
                    samples << numNonRate << ' ' << numRate;

                    ASSERTV(i, line, "PERF"    == tokens[0]);
                    ASSERTV(i, line, "printer" == tokens[1]);
                    ASSERTV(i, line, pid.str() == tokens[2]);
                    ASSERTV(i, line, name, NAMES[i] == name);
                    ASSERTV(i, line,
                            samples.str() == tokens[N - 6] + ' ' +
                                                               tokens[N - 5]);

                    const bool hasStats =
                         !isCumulative(i) && (isRate(i) ? 0 < numRate
                                                        : 0 < numNonRate);
                    const bool isNa = "N/A" == tokens[N - 3];

                    ASSERTV(i, line, hasStats, isNa, hasStats != isNa);
                    if (isNa) {
                        ASSERTV(i, line, "N/A" == tokens[N - 2]);
                        ASSERTV(i, line, "N/A" == tokens[N - 1]);
                    }
                }

                bsl::string extra;
                ASSERT(!bsl::getline(allLines, extra));

                bsl::ostringstream none;
                S.print(none, "NO_SUCH_MEASURE");
                ASSERTV(none.str(), none.str().empty());
            }
        }
        ASSERTV(ta.numBytesInUse(), 0 == ta.numBytesInUse());
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING MIN, MAX, AND AVERAGE VALUES AND `resetStatistics`
        //
        // Concerns:
        // 1. For each measure that is not cumulative, `minValue`, `maxValue`,
        //    and `avgValue` return the minimum, maximum, and average of the
        //    collected values.  For a rate measure, the values collected are
        //    those of the second and subsequent collections.
        //
        // 2. For each cumulative measure, `minValue` returns a large positive
        //    value, `maxValue` returns a large negative value, and `avgValue`
        //    returns 0.
        //
        // 3. `resetStatistics` resets the statistics of every registered
        //    process, and then collects once.
        //
        // Plan:
        // 1. Register this process.  Collect several times, consuming CPU time
        //    between collections, and record the latest value of each measure
        //    after each collection that produced a sample.  Verify the min,
        //    max, and average values against those recorded.  (C-1..2)
        //
        // 2. Register a child process too, and call `resetStatistics`.
        //    Verify the statistics of both processes.  (C-3)
        //
        // Testing:
        //   double minValue(Measure measure) const;
        //   double maxValue(Measure measure) const;
        //   double avgValue(Measure measure) const;
        //   void resetStatistics();
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING MIN, MAX, AND AVERAGE VALUES AND"
                             " `resetStatistics`\n"
                             "========================================"
                             "==================\n";

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        {
            Obj mX(&ta);  const Obj& X = mX;
            ASSERT(0 == mX.registerPid(0, "self"));

            const Obj::Statistics& S = *X.find(0);

            if (verbose) cout << "\tTesting min, max, and average.\n";

            const int NUM_COLLECTIONS = 5;

            bsl::vector<double> values[Obj::e_NUM_MEASURES];

            int prevNumRate = 0;
            for (int c = 0; c < NUM_COLLECTIONS; ++c) {
                if (0 < c) {
                    consumeCpu(0.3);
                }
                mX.collect();

                int numNonRate, numRate;
                getNumSamples(&numNonRate, &numRate, S);
                ASSERTV(c, numNonRate, c + 1 == numNonRate);

                for (int i = 0; i < Obj::e_NUM_MEASURES; ++i) {
                    if (!isRate(i) || numRate > prevNumRate) {
                        values[i].push_back(
                              S.latestValue(static_cast<Obj::Measure>(i)));
                    }
                }
                prevNumRate = numRate;
            }

            // The rate measures need elapsed time to advance between two
            // collections, which 1.2 seconds of collections guarantees.

            ASSERTV(prevNumRate, 0 < prevNumRate);

            for (int i = 0; i < Obj::e_NUM_MEASURES; ++i) {
                const Obj::Measure m = static_cast<Obj::Measure>(i);

                const double MIN = S.minValue(m);
                const double MAX = S.maxValue(m);
                const double AVG = S.avgValue(m);

                if (veryVerbose) { T_ P_(i) P_(MIN) P_(MAX) P(AVG) }

                if (isCumulative(i)) {
                    ASSERTV(i, MIN, DBL_MAX  == MIN);
                    ASSERTV(i, MAX, -DBL_MAX == MAX);
                    ASSERTV(i, AVG, 0.0      == AVG);
                    continue;                                       // CONTINUE
                }

                const bsl::vector<double>& V = values[i];
                if (V.empty()) {
                    continue;                                       // CONTINUE
                }

                double expMin = V[0], expMax = V[0], total = 0;
                for (bsl::size_t j = 0; j < V.size(); ++j) {
                    expMin  = bsl::min(expMin, V[j]);
                    expMax  = bsl::max(expMax, V[j]);
                    total  += V[j];
                }
                const double expAvg = total / static_cast<double>(V.size());

                ASSERTV(i, MIN, expMin, expMin == MIN);
                ASSERTV(i, MAX, expMax, expMax == MAX);
                ASSERTV(i, AVG, expAvg,
                        bsl::fabs(expAvg - AVG) <=
                                            1e-9 * bsl::max(1.0, expAvg));
            }

            if (verbose) cout << "\tTesting `resetStatistics`.\n";

            processSupport::ProcessHandle child = spawnSleeper(argv[0]);

            const int CHILD = processSupport::getId(child);
            ASSERTV(CHILD, -1 != CHILD);
            ASSERTV(CHILD, 0 == mX.registerPid(CHILD, "child"));

            mX.collect();
            consumeCpu(1.1);
            mX.collect();

            mX.resetStatistics();

            for (ObjIterator it = X.begin(); it != X.end(); ++it) {
                const Obj::Statistics& T = *it;

                int numNonRate, numRate;
                getNumSamples(&numNonRate, &numRate, T);

                ASSERTV(T.pid(), numNonRate, 1 == numNonRate);
                ASSERTV(T.pid(), numRate,    0 == numRate);

                for (int i = 0; i < Obj::e_NUM_MEASURES; ++i) {
                    const Obj::Measure m = static_cast<Obj::Measure>(i);

                    const double LATEST = T.latestValue(m);
                    const double MIN    = T.minValue(m);
                    const double MAX    = T.maxValue(m);
                    const double AVG    = T.avgValue(m);

                    if (isCumulative(i)) {
                        ASSERTV(T.pid(), i, MIN, DBL_MAX  == MIN);
                        ASSERTV(T.pid(), i, MAX, -DBL_MAX == MAX);
                        ASSERTV(T.pid(), i, AVG, 0.0      == AVG);
                    }
                    else if (isRate(i)) {
                        ASSERTV(T.pid(), i, LATEST, 0.0      == LATEST);
                        ASSERTV(T.pid(), i, MIN,    DBL_MAX  == MIN);
                        ASSERTV(T.pid(), i, MAX,    -DBL_MAX == MAX);
                    }
                    else {
                        ASSERTV(T.pid(), i, LATEST, MIN, LATEST == MIN);
                        ASSERTV(T.pid(), i, LATEST, MAX, LATEST == MAX);
                        ASSERTV(T.pid(), i, LATEST, AVG, LATEST == AVG);
                    }
                }
            }

            ASSERT(0 < S.latestValue(Obj::e_CPU_TIME));

            ASSERT(0 == mX.unregisterPid(CHILD));
            ASSERT(0 == processSupport::terminateProcess(child));
        }
        ASSERTV(ta.numBytesInUse(), 0 == ta.numBytesInUse());
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // CONCERN: CPU UTILIZATION
        //
        // Concerns:
        // 1. CPU utilization agrees with independently sampled native CPU
        //    times, even when the host limits the available CPU time.
        //
        // 2. The total CPU utilization is the sum of the user and system CPU
        //    utilizations.
        //
        // 3. The CPU utilization of a process consuming more than one CPU
        //    exceeds 100.
        //
        // Plan:
        // 1. Collect, consume CPU time on one thread for 1.5 seconds, and
        //    collect again, sampling native CPU times just before and just
        //    after each `collect`.  The CPU time behind the reported
        //    utilization lies between the native CPU time inside the two
        //    `collect` calls and the native CPU time around them.  Verify
        //    that the utilization, multiplied by the monitor's own sampling
        //    interval (the change in `elapsedTime`, verified in case 16),
        //    falls within those bounds, for total and user utilization.
        //    Verify that total utilization is the sum of the user and system
        //    utilizations.  (C-1..2)
        //
        // 2. If at least four CPUs are available, repeat on three threads,
        //    keeping them parked through collection.  Verify that
        //    utilization exceeds 150 when the native samples confirm that
        //    the workload received that much CPU time.  Release and join the
        //    threads.  (C-3)
        //
        // Testing:
        //   CONCERN: CPU utilization
        // --------------------------------------------------------------------

        if (verbose) cout << "CONCERN: CPU UTILIZATION\n"
                             "========================\n";

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        const unsigned int NUM_CPUS = bslmt::ThreadUtil::hardwareConcurrency();

        if (veryVerbose) { T_ P(NUM_CPUS) }

        // Note that a `collect` samples CPU times at some moment within the
        // call, and its remaining work (on Windows, a PDH instance lookup
        // that takes a large fraction of a second) uses an amount of CPU time
        // that depends on the load on the test host.  So the reported rate
        // cannot be compared with a native rate over a different window.
        // Bound it instead: CPU times never decrease, so the CPU time the
        // monitor saw lies between the native CPU time inside the `collect`
        // calls and the native CPU time around them, whatever the load.
        // `SLACK` allows for clock-tick rounding of the monitor's CPU times
        // (e.g., 10 ms on Linux).  A unit or scaling error fails every run.

        const double SLACK = 0.05;

        {
            Obj mX(&ta);  const Obj& X = mX;
            ASSERT(0 == mX.registerPid(0, "self"));

            const Obj::Statistics& S = *X.find(0);

            if (verbose) cout << "\tTesting one busy thread.\n";
            {
                const CpuSample outerBefore;
                mX.collect();
                const double    START = S.elapsedTime();
                const CpuSample before;
                consumeCpu(1.5);
                const CpuSample after;
                mX.collect();
                const CpuSample outerAfter;

                const double INTERVAL = S.elapsedTime() - START;
                ASSERTV(INTERVAL, 0.0 < INTERVAL);

                const double UTIL   = S.latestValue(Obj::e_CPU_UTIL);
                const double USER   = S.latestValue(Obj::e_CPU_UTIL_USER);
                const double SYSTEM = S.latestValue(Obj::e_CPU_UTIL_SYSTEM);

                if (veryVerbose) { T_ P_(UTIL) P_(USER) P(SYSTEM) }

                const double MAX_UTIL = 100.0 * bsl::max(1u, NUM_CPUS) + 10.0;

                const double CPU_TIME = UTIL / 100.0 * INTERVAL;
                const double MIN_CPU  = cpuTime(before, after) - SLACK;
                const double MAX_CPU  = cpuTime(outerBefore, outerAfter)
                                      + SLACK;
                ASSERTV(MIN_CPU, CPU_TIME, MAX_CPU,
                        MIN_CPU <= CPU_TIME && CPU_TIME <= MAX_CPU);
                ASSERTV(UTIL, MAX_UTIL, UTIL <= MAX_UTIL);

                const double USER_TIME = USER / 100.0 * INTERVAL;
                const double MIN_USER  = after.d_user - before.d_user - SLACK;
                const double MAX_USER  = outerAfter.d_user
                                       - outerBefore.d_user
                                       + SLACK;
                ASSERTV(MIN_USER, USER_TIME, MAX_USER,
                        MIN_USER <= USER_TIME && USER_TIME <= MAX_USER);
                ASSERTV(UTIL, 0.0 <= UTIL);
                ASSERTV(USER, 0.0 <= USER);
                ASSERTV(SYSTEM, 0.0 <= SYSTEM);
                ASSERTV(UTIL, USER, SYSTEM,
                        bsl::fabs(UTIL - (USER + SYSTEM)) <= 1e-6 * UTIL);
            }

            if (verbose) cout << "\tTesting three busy threads.\n";

            if (4 <= NUM_CPUS) {
                const int NUM_THREADS = 3;

                bsls::AtomicInt    numReady(0);
                bsls::AtomicBool   release(false);
                const ParkedThread consumer = { 1.5, &numReady, &release };

                const CpuSample outerBefore;
                mX.collect();
                const double    START = S.elapsedTime();
                const CpuSample before;

                bslmt::ThreadGroup threads(&ta);
                const int          NUM_STARTED =
                                   threads.addThreads(consumer, NUM_THREADS);
                ASSERTV(NUM_STARTED, NUM_THREADS == NUM_STARTED);
                ASSERT(waitForCount(numReady, NUM_STARTED, 30.0));
                const CpuSample after;

                mX.collect();
                const CpuSample outerAfter;

                const double UTIL     = S.latestValue(Obj::e_CPU_UTIL);
                const double INTERVAL = S.elapsedTime() - START;
                ASSERTV(INTERVAL, 0.0 < INTERVAL);

                release = true;
                threads.joinAll();

                if (veryVerbose) { T_ P(UTIL) }

                // Note that three busy threads yield about 300 on an idle
                // host.  On a heavily loaded host they share fewer CPUs: the
                // native lower bound then shows 150 or less, and the check is
                // skipped.  A rate capped at exactly 100 (e.g., PDH without
                // `PDH_FMT_NOCAP100` on Windows) fails every run.

                const double CPU_TIME = UTIL / 100.0 * INTERVAL;
                const double MIN_CPU  = cpuTime(before, after) - SLACK;
                const double MAX_CPU  = cpuTime(outerBefore, outerAfter)
                                      + SLACK;
                ASSERTV(MIN_CPU, CPU_TIME, MAX_CPU,
                        MIN_CPU <= CPU_TIME && CPU_TIME <= MAX_CPU);

                const double MIN_UTIL = MIN_CPU / INTERVAL * 100.0;
                if (150.0 < MIN_UTIL) {
                    ASSERTV(UTIL, MIN_UTIL, 150.0 < UTIL);
                }
                else if (verbose) {
                    cout << "\t\tSkipped utilization > 150: native samples "
                            "show only " << MIN_UTIL << ".\n";
                }
            }
            else if (verbose) {
                cout << "\t\tSkipped: fewer than four CPUs.\n";
            }
        }
        ASSERTV(ta.numBytesInUse(), 0 == ta.numBytesInUse());
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // CONCERN: MEMORY, THREAD COUNT, AND PAGE FAULT MEASURES
        //
        // Concerns:
        // 1. The thread count reflects threads that are created and joined.
        //
        // 2. The resident size and virtual size grow, in megabytes, by about
        //    the amount of memory allocated and touched.
        //
        // 3. The page fault count grows when fresh memory is touched.  On
        //    Solaris, which does not count every such fault, the reported
        //    count agrees with the native count.
        //
        // Plan:
        // 1. Collect, create threads that block on a barrier, and collect
        //    again.  Verify that the thread count grew by at least the number
        //    of threads.  Release and join the threads, collect, and verify
        //    that the thread count dropped.  (C-1)
        //
        // 2. Collect, allocate 128 MB and write to every byte, and collect
        //    again.  Verify that the resident size and virtual size grew by at
        //    least 64 MB, and that the page fault count grew.  (C-2..3)
        //
        // Testing:
        //   CONCERN: Memory, thread count, and page fault measures
        // --------------------------------------------------------------------

        if (verbose) cout << "CONCERN: MEMORY, THREAD COUNT, AND PAGE FAULT"
                             " MEASURES\n"
                             "=============================================="
                             "=========\n";

#if defined(BSLS_PLATFORM_OS_FREEBSD)
        if (verbose) cout << "\tSkipped: measures not supported on FreeBSD.\n";
#else
        bslma::TestAllocator ta(veryVeryVeryVerbose);

        {
            Obj mX(&ta);  const Obj& X = mX;
            ASSERT(0 == mX.registerPid(0, "self"));

            const Obj::Statistics& S = *X.find(0);

            if (verbose) cout << "\tTesting thread count.\n";
            {
                const int NUM_THREADS = 4;

                mX.collect();
                const double BASE = S.latestValue(Obj::e_NUM_THREADS);

                bsls::AtomicInt    numReady(0);
                bsls::AtomicBool   release(false);
                const ParkedThread blocked = { 0.0, &numReady, &release };

                bslmt::ThreadGroup threads(&ta);
                const int          NUM_STARTED =
                                     threads.addThreads(blocked, NUM_THREADS);
                ASSERTV(NUM_STARTED, NUM_THREADS == NUM_STARTED);
                ASSERT(waitForCount(numReady, NUM_STARTED, 30.0));

                mX.collect();
                const double RUNNING = S.latestValue(Obj::e_NUM_THREADS);

                release = true;
                threads.joinAll();

                mX.collect();
                const double JOINED = S.latestValue(Obj::e_NUM_THREADS);

                if (veryVerbose) { T_ P_(BASE) P_(RUNNING) P(JOINED) }

                ASSERTV(BASE, 1 <= BASE);
                ASSERTV(BASE, RUNNING, BASE + NUM_STARTED <= RUNNING);
                ASSERTV(RUNNING, JOINED, JOINED < RUNNING);
            }

            if (verbose) cout << "\tTesting memory sizes and page faults.\n";
            {
                const bsl::size_t SIZE_MB = 128;
                const bsl::size_t SIZE    = SIZE_MB * 1024 * 1024;

                mX.collect();
                const double RESIDENT   = S.latestValue(Obj::e_RESIDENT_SIZE);
                const double VIRTUAL    = S.latestValue(Obj::e_VIRTUAL_SIZE);
                const double PAGEFAULTS = S.latestValue(Obj::e_NUM_PAGEFAULTS);

                char *buffer = static_cast<char *>(ta.allocate(SIZE));

                // Write values that a memory compressor can't easily compress.

                unsigned int x = 12345;
                for (bsl::size_t i = 0; i < SIZE; ++i) {
                    x = x * 1103515245 + 12345;
                    buffer[i] = static_cast<char>(x >> 16);
                }

#if defined(BSLS_PLATFORM_OS_SOLARIS)
                const double faultsBefore = getOwnPageFaults();
#endif
                mX.collect();
#if defined(BSLS_PLATFORM_OS_SOLARIS)
                const double faultsAfter = getOwnPageFaults();
#endif
                const double NEW_RESIDENT = S.latestValue(
                                                        Obj::e_RESIDENT_SIZE);
                const double NEW_VIRTUAL  = S.latestValue(Obj::e_VIRTUAL_SIZE);
                const double NEW_PAGEFAULTS =
                                        S.latestValue(Obj::e_NUM_PAGEFAULTS);

                ta.deallocate(buffer);

                if (veryVerbose) {
                    T_ P_(RESIDENT)   P(NEW_RESIDENT)
                    T_ P_(VIRTUAL)    P(NEW_VIRTUAL)
                    T_ P_(PAGEFAULTS) P(NEW_PAGEFAULTS)
                }

                // Note that memory pressure on the test host limits the growth
                // of the resident size: the operating system pages out or
                // compresses the buffer before the second collection.  On
                // Solaris, the virtual size is the size of the heap, which
                // grows only when the allocator takes the buffer from the
                // heap.  An intermittent failure here points to the memory
                // load on the test host, or to the allocator.  A unit error
                // fails every run.

                const double MIN_GROWTH = SIZE_MB / 2;

                ASSERTV(RESIDENT, NEW_RESIDENT,
                        RESIDENT + MIN_GROWTH <= NEW_RESIDENT);

#if !defined(BSLS_PLATFORM_OS_AIX)
                // AIX is excluded: its virtual size is the size of the heap,
                // which this allocation need not grow, and it does not report
                // page faults.

                ASSERTV(VIRTUAL, NEW_VIRTUAL,
                        VIRTUAL + MIN_GROWTH <= NEW_VIRTUAL);
#if defined(BSLS_PLATFORM_OS_SOLARIS)
                ASSERTV(NEW_PAGEFAULTS, faultsBefore,
                        faultsBefore <= NEW_PAGEFAULTS);
                ASSERTV(NEW_PAGEFAULTS, faultsAfter,
                        NEW_PAGEFAULTS <= faultsAfter);
                ASSERTV(PAGEFAULTS, NEW_PAGEFAULTS,
                        PAGEFAULTS <= NEW_PAGEFAULTS);
#else
                ASSERTV(PAGEFAULTS, 0 < PAGEFAULTS);
                ASSERTV(PAGEFAULTS, NEW_PAGEFAULTS,
                        PAGEFAULTS < NEW_PAGEFAULTS);
#endif
#endif
            }
        }
        ASSERTV(ta.numBytesInUse(), 0 == ta.numBytesInUse());
#endif
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING `elapsedTime` AND `startupTime`
        //
        // Concerns:
        // 1. `elapsedTime` is 0 before any collection.
        //
        // 2. After a collection, `elapsedTime` is the time elapsed since the
        //    process started, as of that collection.
        //
        // 3. The `startupTime` of a child process is the time at which it was
        //    spawned.
        //
        // Plan:
        // 1. Register this process and verify that `elapsedTime` is 0.  (C-1)
        //
        // 2. Collect, and verify that `elapsedTime` is within a tolerance of
        //    the current time minus `startupTime`.  (C-2)
        //
        // 3. Spawn a child process, register it, and collect.  Verify that its
        //    `startupTime` is within a tolerance of the time at which it was
        //    spawned, and that its `elapsedTime` is within a tolerance of the
        //    current time minus its `startupTime`.  (C-2..3)
        //
        // Testing:
        //   double elapsedTime() const;
        //   const bdlt::Datetime& startupTime() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING `elapsedTime` AND `startupTime`\n"
                             "=======================================\n";

        // On Linux, the start time is derived from the boot time, which is
        // reported in whole seconds.  Note that a `collect` samples the time
        // at some moment within the call, and its remaining work (on Windows,
        // a PDH instance lookup that enumerates every process on the host)
        // can take several seconds on a loaded test host.  So the elapsed
        // time is compared with the times before and after the `collect`,
        // rather than with the time after it.

        const double TOLERANCE = 3.0;

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        {
            Obj mX(&ta);  const Obj& X = mX;
            ASSERT(0 == mX.registerPid(0, "self"));

            const Obj::Statistics& S = *X.find(0);

            if (verbose) cout << "\tTesting this process.\n";

            ASSERTV(S.elapsedTime(), 0.0 == S.elapsedTime());

            consumeCpu(1.1);

            const bdlt::Datetime COLLECT_BEGIN = bdlt::CurrentTime::utc();
            mX.collect();
            const bdlt::Datetime COLLECT_END   = bdlt::CurrentTime::utc();

            const double MIN_EXPECTED = (COLLECT_BEGIN - S.startupTime())
                                                      .totalSecondsAsDouble();
            const double MAX_EXPECTED = (COLLECT_END   - S.startupTime())
                                                      .totalSecondsAsDouble();
            const double ELAPSED      = S.elapsedTime();

            if (veryVerbose) {
                T_ P_(MIN_EXPECTED) P_(ELAPSED) P(MAX_EXPECTED)
            }

            ASSERTV(ELAPSED, 0.0 < ELAPSED);
            ASSERTV(ELAPSED, MIN_EXPECTED,
                    MIN_EXPECTED - TOLERANCE <= ELAPSED);
            ASSERTV(ELAPSED, MAX_EXPECTED,
                    ELAPSED <= MAX_EXPECTED + TOLERANCE);

            if (verbose) cout << "\tTesting a child process.\n";

            const bdlt::Datetime BEFORE = bdlt::CurrentTime::utc();

            processSupport::ProcessHandle child = spawnSleeper(argv[0]);

            const bdlt::Datetime AFTER = bdlt::CurrentTime::utc();

            const int CHILD = processSupport::getId(child);
            ASSERTV(CHILD, -1 != CHILD);
            ASSERTV(CHILD, 0 == mX.registerPid(CHILD, "child"));

            bslmt::ThreadUtil::microSleep(0, 1);

            const bdlt::Datetime CHILD_COLLECT_BEGIN =
                                                     bdlt::CurrentTime::utc();
            mX.collect();
            const bdlt::Datetime CHILD_COLLECT_END   =
                                                     bdlt::CurrentTime::utc();

            {
                ObjIterator it = X.find(CHILD);
                ASSERT(X.end() != it);

                if (X.end() != it) {
                    const bdlt::Datetime START         = it->startupTime();
                    const double         CHILD_ELAPSED = it->elapsedTime();
                    const double         CHILD_MIN_EXPECTED =
                                    (CHILD_COLLECT_BEGIN - START)
                                                      .totalSecondsAsDouble();
                    const double         CHILD_MAX_EXPECTED =
                                    (CHILD_COLLECT_END   - START)
                                                      .totalSecondsAsDouble();

                    if (veryVerbose) {
                        T_ P_(BEFORE) P_(START) P(AFTER)
                        T_ P_(CHILD_MIN_EXPECTED) P_(CHILD_ELAPSED)
                                                       P(CHILD_MAX_EXPECTED)
                    }

                    ASSERTV(BEFORE, START,
                            (START - BEFORE).totalSecondsAsDouble() >=
                                                                  -TOLERANCE);
                    ASSERTV(AFTER, START,
                            (START - AFTER).totalSecondsAsDouble() <=
                                                                   TOLERANCE);
                    ASSERTV(CHILD_ELAPSED, CHILD_MIN_EXPECTED,
                            CHILD_MIN_EXPECTED - TOLERANCE <= CHILD_ELAPSED);
                    ASSERTV(CHILD_ELAPSED, CHILD_MAX_EXPECTED,
                            CHILD_ELAPSED <= CHILD_MAX_EXPECTED + TOLERANCE);
                }
            }

            ASSERT(0 == mX.unregisterPid(CHILD));
            ASSERT(0 == processSupport::terminateProcess(child));
        }
        ASSERTV(ta.numBytesInUse(), 0 == ta.numBytesInUse());
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING `registerPid`, `unregisterPid`, AND ITERATOR INVALIDATION
        //
        // Concerns:
        // 1. `registerPid` fails for a pid that does not exist, and does not
        //    register it.
        //
        // 2. `registerPid` for a pid that is already registered returns 0 and
        //    leaves the registration, including its description, unchanged.
        //
        // 3. `unregisterPid` fails for a pid that is not registered, and
        //    translates a pid of 0 to the current process id.
        //
        // 4. Registering a pid, and unregistering another pid, do not
        //    invalidate an existing iterator.
        //
        // 5. A default-constructed iterator can be copied.
        //
        // Plan:
        // 1. Register a nonexistent pid, and verify that it fails and that
        //    nothing is registered.  (C-1)
        //
        // 2. Register this process twice, with different descriptions, and
        //    verify the results.  Repeat for a child after it exits.  (C-2)
        //
        // 3. Obtain an iterator to this process.  Register and then unregister
        //    a child process, and verify that the iterator still refers to the
        //    statistics of this process.  (C-4)
        //
        // 4. Unregister an unregistered pid, and unregister this process
        //    using a pid of 0, and verify the results.  (C-3)
        //
        // 5. Copy a default-constructed iterator.  (C-5)
        //
        // Testing:
        //   int registerPid(int pid, const bsl::string& description);
        //   int unregisterPid(int pid);
        //   ConstIterator();
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING `registerPid`, `unregisterPid`, AND"
                             " ITERATOR INVALIDATION\n"
                             "==========================================="
                             "======================\n";

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        // No supported platform has a process with this pid.

        const int NONEXISTENT = INT_MAX;
        const int SELF        = bdls::ProcessUtil::getProcessId();

        {
            Obj mX(&ta);  const Obj& X = mX;

            if (verbose) cout << "\tTesting nonexistent pid.\n";

            ASSERT(0 != mX.registerPid(NONEXISTENT, "nonexistent"));
            ASSERT(0 == X.numRegisteredPids());
            ASSERT(X.end() == X.find(NONEXISTENT));

            if (verbose) cout << "\tTesting duplicate registration.\n";

            ASSERT(0 == mX.registerPid(0,    "first"));
            ASSERT(0 == mX.registerPid(SELF, "second"));
            ASSERT(1 == X.numRegisteredPids());
            ASSERT("first" == X.find(0)->description());

            if (verbose) cout << "\tTesting iterator invalidation.\n";

            const ObjIterator      IT   = X.find(0);
            const Obj::Statistics *STATS = &*IT;

            processSupport::ProcessHandle child = spawnSleeper(argv[0]);

            const int CHILD = processSupport::getId(child);
            ASSERTV(CHILD, -1 != CHILD);
            ASSERTV(CHILD, 0 == mX.registerPid(CHILD, "child"));
            ASSERT(2 == X.numRegisteredPids());

            ASSERT(IT == X.find(0));
            ASSERT(SELF  == IT->pid());
            ASSERT(STATS == &*IT);

            mX.collect();

            const Obj::Statistics *childStats = &*X.find(CHILD);
            ASSERT(0 == processSupport::terminateProcess(child));
            ASSERT(0 == mX.registerPid(CHILD, "duplicate"));
            ASSERT(2 == X.numRegisteredPids());
            ASSERT(childStats == &*X.find(CHILD));
            ASSERT("child" == X.find(CHILD)->description());
            ASSERT(1 == numSamples(*X.find(CHILD)));

            ASSERT(0 == mX.unregisterPid(CHILD));
            ASSERT(1 == X.numRegisteredPids());
            ASSERT(X.end() == X.find(CHILD));

            ASSERT(IT == X.find(0));
            ASSERT(SELF    == IT->pid());
            ASSERT("first" == IT->description());
            ASSERT(1       == numSamples(*IT));

            if (verbose) cout << "\tTesting `unregisterPid`.\n";

            ASSERT(0 != mX.unregisterPid(NONEXISTENT));
            ASSERT(1 == X.numRegisteredPids());

            ASSERT(0 == mX.unregisterPid(0));
            ASSERT(0 == X.numRegisteredPids());
            ASSERT(X.end() == X.find(SELF));

            ASSERT(0 != mX.unregisterPid(0));
        }
        ASSERTV(ta.numBytesInUse(), 0 == ta.numBytesInUse());

        if (verbose) cout << "\tTesting default-constructed iterator.\n";
        {
            const ObjIterator A;
            const ObjIterator B(A);
            ASSERT(A == B);
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING `setCollectionInterval`
        //
        // Concerns:
        // 1. A monitor constructed with a scheduler and a positive interval
        //    collects automatically; one constructed with a non-positive
        //    interval does not.
        //
        // 2. `setCollectionInterval` with a positive interval starts automatic
        //    collection.
        //
        // 3. `setCollectionInterval` with a non-positive interval stops
        //    automatic collection, and no collection is in progress when it
        //    returns.
        //
        // 4. `setCollectionInterval` with a non-positive interval when
        //    automatic collection is already stopped leaves it stopped.
        //
        // 5. A monitor can be destroyed while collecting automatically, or
        //    immediately after automatic collection is stopped.
        //
        // 6. A dispatcher callback and another thread can concurrently stop
        //    automatic collection without deadlocking.
        //
        // 7. Changing a positive interval at scheduler capacity preserves
        //    automatic collection.
        //
        // 8. A newer interval request supersedes a blocked request.
        //
        // Plan:
        // 1. Construct a monitor with an interval of 0, register this process,
        //    and verify that no collection occurs.  Call
        //    `setCollectionInterval` with 0 and with -1, and verify that no
        //    collection occurs.  (C-1, C-4)
        //
        // 2. Set a positive interval, and verify that collections occur.
        //    (C-2)
        //
        // 3. Set an interval of 0, and verify that no further collections
        //    occur.  Set it to 0 again, and verify again.  (C-3..4)
        //
        // 4. Construct a monitor with a positive interval, and verify that
        //    collections occur.  (C-1)
        //
        // 5. Repeatedly construct monitors that collect very frequently, and
        //    destroy them, with and without stopping automatic collection
        //    first.  (Run under an address sanitizer to detect use after
        //    free.)  (C-5)
        //
        // 6. Use a test time source to make an earlier callback and the
        //    collection clock due together.  Block the earlier callback,
        //    stop collection from another thread, and then let the callback
        //    also stop collection.  Verify that both calls return.  (C-6)
        //
        // 7. Fill a scheduler to capacity.  Change the collection interval
        //    and verify that collection occurs at the new interval.  (C-7)
        //
        // 8. Repeat P-6 with the other thread requesting a positive interval.
        //    Verify that the callback's later stop request prevails.  (C-8)
        //
        // Testing:
        //   PerformanceMonitor(TimerEventScheduler *, double, Allocator *);
        //   ~PerformanceMonitor();
        //   void setCollectionInterval(double interval);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING `setCollectionInterval`\n"
                             "===============================\n";

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        {
            bdlmt::TimerEventScheduler scheduler(&ta);
            scheduler.start();

            if (verbose) cout << "\tTesting non-positive intervals.\n";
            {
                Obj mX(&scheduler, 0.0, &ta);  const Obj& X = mX;
                ASSERT(0 == mX.registerPid(0, "self"));

                const Obj::Statistics& S = *X.find(0);

                bslmt::ThreadUtil::microSleep(300000, 0);
                ASSERTV(numSamples(S), 0 == numSamples(S));

                mX.setCollectionInterval(0.0);
                bslmt::ThreadUtil::microSleep(300000, 0);
                ASSERTV(numSamples(S), 0 == numSamples(S));

                mX.setCollectionInterval(-1.0);
                bslmt::ThreadUtil::microSleep(300000, 0);
                ASSERTV(numSamples(S), 0 == numSamples(S));

                if (verbose) cout << "\tTesting a positive interval.\n";

                mX.setCollectionInterval(0.05);
                ASSERT(waitForSamples(S, 3, 10.0));

                if (verbose) cout << "\tTesting stopping collection.\n";

                mX.setCollectionInterval(0.0);
                const int N = numSamples(S);

                bslmt::ThreadUtil::microSleep(300000, 0);
                ASSERTV(N, numSamples(S), N == numSamples(S));

                mX.setCollectionInterval(0.0);
                bslmt::ThreadUtil::microSleep(300000, 0);
                ASSERTV(N, numSamples(S), N == numSamples(S));

                if (verbose) cout << "\tTesting changing the interval.\n";

                mX.setCollectionInterval(10.0);
                mX.setCollectionInterval(0.05);
                ASSERT(waitForSamples(S, N + 3, 10.0));

                mX.setCollectionInterval(0.0);
            }

            if (verbose) cout << "\tTesting constructor.\n";
            {
                Obj mX(&scheduler, 0.05, &ta);  const Obj& X = mX;
                ASSERT(0 == mX.registerPid(0, "self"));

                ASSERT(waitForSamples(*X.find(0), 3, 10.0));
            }

            if (verbose) cout << "\tTesting destruction.\n";

            for (int i = 0; i < 20; ++i) {
                {
                    Obj mX(&scheduler, 0.001, &ta);
                    ASSERT(0 == mX.registerPid(0, "self"));
                    bslmt::ThreadUtil::microSleep(5000, 0);
                }
                {
                    Obj mX(&scheduler, 0.001, &ta);
                    ASSERT(0 == mX.registerPid(0, "self"));
                    bslmt::ThreadUtil::microSleep(5000, 0);
                    mX.setCollectionInterval(0.0);
                }
            }

            scheduler.stop();
        }
        ASSERTV(ta.numBytesInUse(), 0 == ta.numBytesInUse());

        if (verbose) cout << "\tTesting interval changes at capacity.\n";
        {
            bdlmt::TimerEventScheduler scheduler(1, 1, &ta);
            bdlmt::TimerEventSchedulerTestTimeSource timeSource(&scheduler);

            Obj mX(&scheduler, 10.0, &ta);
            ASSERT(0 == mX.registerPid(0, "self"));

            while (bdlmt::TimerEventScheduler::e_INVALID_HANDLE !=
                       scheduler.startClock(bsls::TimeInterval(3600, 0),
                                            &ignoreClock)) {
            }
            const int NUM_CLOCKS = scheduler.numClocks();
            ASSERTV(NUM_CLOCKS, 1 < NUM_CLOCKS);

            mX.setCollectionInterval(0.05);
            ASSERTV(NUM_CLOCKS, scheduler.numClocks(),
                    NUM_CLOCKS == scheduler.numClocks());

            timeSource.advanceTime(bsls::TimeInterval(0.05));
            ASSERT(0 == scheduler.start());
            ASSERT(waitForSamples(*mX.find(0), 1, 10.0));

            mX.setCollectionInterval(0);
            scheduler.stop();
        }
        ASSERTV(ta.numBytesInUse(), 0 == ta.numBytesInUse());

        if (verbose) cout << "\tTesting concurrent dispatcher cancellation.\n";
        for (int i = 0; i < 2; ++i)
        {
            bdlmt::TimerEventScheduler scheduler(&ta);
            bdlmt::TimerEventSchedulerTestTimeSource timeSource(&scheduler);

            // Schedule the callback at the test time, and the collection
            // clock 0.05 seconds later.  Then advance the test time past both,
            // so that both are due when the scheduler starts, and the
            // callback runs first.

            const bsls::TimeInterval callbackTime = timeSource.now();
            Obj mX(&scheduler, 0.05, &ta);
            ASSERT(0 == mX.registerPid(0, "self"));

            bslmt::Barrier entered(2);
            bslmt::Barrier release(2);
            const CollectionIntervalSetter callback = { &mX,
                                                          &entered,
                                                          &release,
                                                          0.0 };

            scheduler.scheduleEvent(callbackTime, callback);
            timeSource.advanceTime(bsls::TimeInterval(1, 0));
            ASSERT(0 == scheduler.start());
            entered.wait();

            bslmt::ThreadGroup threads(&ta);
            const CollectionIntervalSetter setter = { &mX,
                                                        0,
                                                        0,
                                                        i == 0 ? 0.0 : 0.05 };
            ASSERT(0 == threads.addThread(setter));

            // Cancellation removes the clock before waiting for the blocked
            // dispatcher.  Once it has done so, let the callback also stop
            // collection; it must be able to acquire the monitor's mutex.

            const bsls::TimeInterval deadline = bdlt::CurrentTime::now()
                                                + bsls::TimeInterval(10, 0);
            while (0 != scheduler.numClocks()
                && bdlt::CurrentTime::now() < deadline) {
                bslmt::ThreadUtil::microSleep(1000, 0);
            }
            ASSERT(0 == scheduler.numClocks());

            release.wait();
            threads.joinAll();
            scheduler.stop();

            ASSERTV(i, scheduler.numClocks(), 0 == scheduler.numClocks());
            ASSERT(0 == numSamples(*mX.find(0)));
        }
        ASSERTV(ta.numBytesInUse(), 0 == ta.numBytesInUse());
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING CPU TIMES AND START TIME
        //
        // Concerns:
        // 1. The system CPU time is reported separately from the user CPU
        //    time, and the total CPU time is their sum.
        //
        // 2. The CPU times and start time reported for a registered process
        //    are those of that process, and not those of the monitoring
        //    process.
        //
        // 3. The CPU times are reported in seconds.
        //
        // Plan:
        // 1. Register this process.  Consume user time, calling `collect`
        //    after each round, until at least 1 second of user CPU time is
        //    reported.  Verify that the total CPU time is the sum of the user
        //    and system CPU times.  Their relative sizes depend on the host;
        //    check each against the native API in the next step.  (C-1)
        //
        // 2. Obtain the user and system CPU times of this process directly
        //    from the operating system immediately before and after a call to
        //    `collect`.  Verify that the reported times lie between them,
        //    within a tolerance for the granularity of the operating system's
        //    accounting.  (C-3)
        //
        // 3. Spawn a child process running test case -3, which sleeps
        //    without consuming CPU time, register it, and call `collect`.
        //    Verify that the total CPU time reported for the child is less
        //    than that of this process, and that the child started after this
        //    process.  (C-2)
        //
        // Testing:
        //   CONCERN: CPU times and start time are those of the process
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING CPU TIMES AND START TIME\n"
                             "================================\n";

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        Obj perfmon(&ta);
        ASSERT(0 == perfmon.registerPid(0, "parent"));

        const Obj::Statistics& stats = *perfmon.find(0);

        if (verbose) cout << "\tTesting user and system CPU time.\n";

        const bsls::TimeInterval deadline = bdlt::CurrentTime::now() +
                                            bsls::TimeInterval(60, 0);

        do {
            consumeUserTime();
            perfmon.collect();
        } while (stats.latestValue(Obj::e_CPU_TIME_USER) < 1.0 &&
                 bdlt::CurrentTime::now() < deadline);

        const double userTime   = stats.latestValue(Obj::e_CPU_TIME_USER);
        const double systemTime = stats.latestValue(Obj::e_CPU_TIME_SYSTEM);
        const double cpuTime    = stats.latestValue(Obj::e_CPU_TIME);

        if (veryVerbose) { T_ P_(userTime) P_(systemTime) P(cpuTime) }

        ASSERTV(userTime, 1.0 <= userTime);
        ASSERTV(systemTime, 0.0 <= systemTime);
        ASSERTV(userTime, systemTime, cpuTime,
                userTime + systemTime == cpuTime);

        if (verbose) cout << "\tTesting units of CPU time.\n";
        {
            // The tolerance covers operating systems that account CPU time
            // in clock ticks as coarse as 10ms.  A unit error is far larger.

            const double TOLERANCE = 0.05;

            double userBefore, systemBefore, userAfter, systemAfter;

            getOwnCpuTimes(&userBefore, &systemBefore);
            perfmon.collect();
            getOwnCpuTimes(&userAfter, &systemAfter);

            const double user   = stats.latestValue(Obj::e_CPU_TIME_USER);
            const double system = stats.latestValue(Obj::e_CPU_TIME_SYSTEM);

            if (veryVerbose) {
                T_ P_(userBefore)   P_(user)   P(userAfter)
                T_ P_(systemBefore) P_(system) P(systemAfter)
            }

            ASSERTV(userBefore, user, userBefore - TOLERANCE <= user);
            ASSERTV(userAfter,  user, user <= userAfter + TOLERANCE);
            ASSERTV(systemBefore, system,
                    systemBefore - TOLERANCE <= system);
            ASSERTV(systemAfter, system, system <= systemAfter + TOLERANCE);
        }

        if (verbose) cout << "\tTesting CPU time and start time of a child.\n";

        // Spawn as a child process a copy of this test driver, running test
        // case -3, which simply sleeps for half a minute and exits.

        bsl::string              command(argv[0]);
        bsl::vector<bsl::string> arguments;

        arguments.push_back(bsl::string("-3"));

        processSupport::ProcessHandle handle =
                                      processSupport::exec(command, arguments);

        const int childPid = processSupport::getId(handle);
        ASSERTV(childPid, -1 != childPid);

        if (-1 != childPid) {
            ASSERTV(childPid, 0 == perfmon.registerPid(childPid, "child"));

            perfmon.collect();

            {
                ObjIterator it = perfmon.find(childPid);
                ASSERTV(childPid, perfmon.end() != it);

                if (perfmon.end() != it) {
                    const double childCpuTime =
                                             it->latestValue(Obj::e_CPU_TIME);

                    if (veryVerbose) {
                        T_ P_(childCpuTime) P(cpuTime)
                        T_ P_(it->startupTime()) P(stats.startupTime())
                    }

                    ASSERTV(childCpuTime, cpuTime, childCpuTime < cpuTime);
                    ASSERTV(it->startupTime(), stats.startupTime(),
                            stats.startupTime() < it->startupTime());
                }
            }

            ASSERTV(childPid, 0 == perfmon.unregisterPid(childPid));
            ASSERTV(childPid, 0 == processSupport::terminateProcess(handle));
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING INITIAL VALUES
        //
        // Concern:
        // 1. If no samples are available for a given measure because `collect`
        //    has not been called enough times, the `latestValue`, `minValue`,
        //    and `maxValue` accessors of the `PerformanceMonitor::Statistics`
        //    class return the values specified by their documentation.  The
        //    necessary number of calls is one for point measures and two for
        //    CPU utilization rates.
        //
        // 2. The zero value returned by the `latestValue` accessor when no
        //    value was yet available is not actually part of the minimum;
        //    therefore, if a positive value is computed in the future, the
        //    minimum will be nonzero.
        //
        // 3. After a call to `resetStatistics`, the above properties hold as
        //    if exactly one call to `collect` has ever occurred.
        //
        // Plan:
        // 1. Create a `balb::PerformanceMonitor` object and register the
        //    current process to it.
        //
        // 2. Before any collection has occurred, call the accessors mentioned
        //    in C-1 and verify that they return the expected values.  (C-1)
        //
        // 3. Call `collect` and verify that for utilization rates, the values
        //    are unchanged, and for point measures that are not cumulative,
        //    the `minValue` accessor returns the same value as `latestValue`
        //    (since only one value contributes to the minimum).  (C-1,2)
        //
        // 4. Call a function that consumes a significant amount of user and
        //    system time, and then call `latestValue` and `minValue` for the
        //    utilization measures again.  Verify that the latest and minimum
        //    values are equal.  (Note that the intent is that these values are
        //    nonzero, but the test doesn't fail if they are zero.)  (C-2)
        //
        // 5. Call `resetStatistics` and then repeat steps 3 and 4 (but without
        //    the initial call to `collect` in P-3).  (C-3)
        //
        // Testing:
        //   CONCERN: Initial values of statistics (DRQS 180602553)
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING INITIAL VALUES\n"
                             "======================\n";

        const double maxDouble = std::numeric_limits<double>::max();

        Obj perfmon;
        perfmon.registerPid(0, "current process");

        const Obj::Statistics& stats = *perfmon.begin();

        {
            if (veryVerbose) cout << "\tcollect() not yet called\n";

            for (int i = 0; i < Obj::e_NUM_MEASURES; ++i) {
                Obj::Measure measure = static_cast<Obj::Measure>(i);
                if (veryVerbose) { T_ T_ P(i) }
                ASSERTV(measure, stats.latestValue(measure),
                        0.0        == stats.latestValue(measure));
                ASSERTV(measure, stats.minValue(measure),
                        maxDouble  == stats.minValue(measure));
                ASSERTV(measure, stats.maxValue(measure),
                        -maxDouble == stats.maxValue(measure));
            }

            perfmon.collect();

            if (veryVerbose) cout << "\tcollect() called once\n";

            for (int i = 0; i < Obj::e_NUM_MEASURES; ++i) {
                Obj::Measure measure     = static_cast<Obj::Measure>(i);
                double       latestValue = stats.latestValue(measure);
                if (veryVerbose) { T_ T_ P_(i) P(latestValue) }
                if (isRate(i)) {
                    ASSERTV(measure, latestValue, 0.0 == latestValue);
                    ASSERTV(measure, stats.minValue(measure),
                            maxDouble   == stats.minValue(measure));
                    ASSERTV(measure, stats.maxValue(measure),
                            -maxDouble  == stats.maxValue(measure));
                } else if (!isCumulative(i)) {
                    ASSERTV(measure, latestValue, stats.minValue(measure),
                            latestValue == stats.minValue(measure));
                }
            }

            consumeUserAndSystemTime();
#if defined(BSLS_PLATFORM_OS_WINDOWS)
            // On Windows, the elapsed time used to compute the rates comes
            // from PDH, which may not advance between two quick calls to
            // `collect`, in which case no rate sample is taken.

            bslmt::ThreadUtil::microSleep(0, 1);
#endif
            perfmon.collect();

            if (veryVerbose) cout << "\tcollect() called twice\n";

            for (int i = 0; i < Obj::e_NUM_MEASURES; ++i) {
                Obj::Measure measure     = static_cast<Obj::Measure>(i);
                double       latestValue = stats.latestValue(measure);
                if (veryVerbose) { T_ T_ P_(i) P(latestValue) }
                if (isRate(i)) {
                    ASSERTV(measure, latestValue, stats.minValue(measure),
                            latestValue == stats.minValue(measure));
                }
            }
        }

        perfmon.resetStatistics();

        {
            if (veryVerbose) cout << "\tresetStatistics() called\n";

            for (int i = 0; i < Obj::e_NUM_MEASURES; ++i) {
                Obj::Measure measure     = static_cast<Obj::Measure>(i);
                double       latestValue = stats.latestValue(measure);
                if (veryVerbose) { T_ T_ P_(i) P(latestValue) }
                if (isRate(i)) {
                    ASSERTV(measure, latestValue, 0.0 == latestValue);
                    ASSERTV(measure, stats.minValue(measure),
                            maxDouble   == stats.minValue(measure));
                    ASSERTV(measure, stats.maxValue(measure),
                            -maxDouble  == stats.maxValue(measure));
                } else if (!isCumulative(i)) {
                    ASSERTV(measure, latestValue, stats.minValue(measure),
                            latestValue == stats.minValue(measure));
                }
            }

            consumeUserAndSystemTime();
#if defined(BSLS_PLATFORM_OS_WINDOWS)
            bslmt::ThreadUtil::microSleep(0, 1);
#endif
            perfmon.collect();

            if (veryVerbose) cout << "\tcollect() called once after reset\n";

            for (int i = 0; i < Obj::e_NUM_MEASURES; ++i) {
                Obj::Measure measure     = static_cast<Obj::Measure>(i);
                double       latestValue = stats.latestValue(measure);
                if (veryVerbose) { T_ T_ P_(i) P(latestValue) }
                if (isRate(i)) {
                    ASSERTV(measure, latestValue, stats.minValue(measure),
                            latestValue == stats.minValue(measure));
                }
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING COMM CONTAINING SPACES
        //
        // The `comm` field normally reflects the filename of the executable,
        // but it can be changed by the client to anything up to 16 bytes long.
        // One way is for a child process to set a thread name in the current
        // thread (this doesn't work for a non-child process).  The `comm`
        // field is not expected to grow beyond 16 bytes, but the test covers
        // longer values.  According to `man proc`, the `comm` field was
        // parsable with `%s` (meaning that it could not contain spaces) but
        // this turned out not to be the case -- a client was configuring the
        // the `comm` field to contain spaces, which was interfering with the
        // the correct functioning of this component.
        //
        // Concern:
        // 1. Spaces occurring in the `comm` field on Linux caused the parsing
        //    of `/proc/<pid>/stat` to fail.  Ensure that our parsing can
        //    cope with this.
        //
        // 2. Linux appends fields to the `/proc/<pid>/stat` file over time.
        //    The parser copes with appended fields.
        //
        // Plan:
        // 1. Call `ProcStat::readProcStatString` to read the
        //    `/proc/<pid>/stat` file into a string.
        //
        // 2. Parse that string  with `ProcStat::parseProcStatString` and
        //    observe that the results are reasonable.
        //
        // 3. Separate out the `front` and `back` seconds of the string before
        //    and after the `comm` field.
        //
        // 4. Initialize an array of various strings.
        //
        // 5. Nest two loops, each iterating through the array of strings.
        //    Have the outer loop dictate the string to be inserted into the
        //    `comm` field, and have the inner loop dictate the string to be
        //    appended to the assembled string.
        //
        // 6. Parse that string  with `ProcStat::parseProcStatString` and
        //    observe that the results are reasonable.
        // --------------------------------------------------------------------

#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_CYGWIN)
        namespace TC = BetterParsingTest;

        if (verbose) cout << "TESTING COMM CONTAINING SPACES\n"
                             "==============================\n";

        const bsl::size_t npos = bsl::string::npos;

        controlledCpuBurn();

        const int pid = bdls::ProcessUtil::getProcessId();
        bsl::string procStat;
        int rc = TC::ProcStats::readProcStatString(&procStat, pid);
        ASSERT(0 == rc);

        if (veryVerbose) P(procStat);

        TC::testParseProcStatStr(procStat, pid);

        ASSERT(1 == bsl::count(procStat.begin(), procStat.end(), '('));
        const bsl::size_t openPos = procStat.find('(');
        ASSERT(npos != openPos);

        const bsl::string FRONT = procStat.substr(0, openPos + 1);

        ASSERT(1 == bsl::count(procStat.begin(), procStat.end(), ')'));
        const bsl::size_t closePos = procStat.find(')');
        ASSERT(npos != closePos);
        ASSERT(openPos < closePos);

        const bsl::string BACK = procStat.substr(closePos);

        if (veryVerbose) { P(FRONT);    P(BACK); }

        const char *strings[] = { "", "woof", "woof meow", "bow() wow()",
                                  "))))", "((((", "!@#$%^&*()",
                                  "()()()()()()()()", "0123456789abcdef",
                                  "((((((((((((((((", "))))))))))))))))",
   "very very very very very very very very very very very very long string" };
        enum { k_NUM_STRINGS = sizeof strings / sizeof *strings };
        enum { k_LONG_STR_IDX = k_NUM_STRINGS - 1 };

        for (int ti = 0; ti < k_NUM_STRINGS; ++ti) {
            const bsl::string COMM     = strings[ti];
            const bsl::string EXP_COMM = '(' + COMM + ')';

            for (int tj = 0; tj < k_NUM_STRINGS; ++tj) {
                const bsl::string TAIL = bsl::string(*strings[tj] ? " " : "") +
                                                                   strings[tj];
                const bsl::string str = FRONT + COMM + BACK + TAIL;

                if (veryVeryVerbose) P(str);

                TC::testParseProcStatStr(str, pid, EXP_COMM.c_str(),
                                                         k_LONG_STR_IDX != ti);
            }
        }

        if (verbose) cout << "\tTesting failures.\n";

        // A pid that does not match the string fails to parse.

        TC::testParseProcStatStr(procStat, pid + 1, 0, false);

        // A process that does not exist has no 'stat' file.

        bsl::string missing;
        ASSERT(0 != TC::ProcStats::readProcStatString(&missing, INT_MAX));
#else
        if (verbose) bsl::cout <<
            "There is no `comm` field parsed on any platform other than\n"
            "Linux, so the bug cannot occur on other platforms.\n"
            "Test skipped.\n";
#endif
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STATISTICS COPY CONSTRUCTOR
        //
        // Concerns:
        // 1. The new object aggregates the same values as the original object.
        //
        // 2. The value of the original object is left unaffected.
        //
        // 3. Subsequent changes in or destruction of the source object have no
        //    effect on the copy-constructed object.
        //
        // 4. The object has its internal memory management system hooked up
        //    properly so that *all* internally allocated memory draws from a
        //    user-supplied allocator whenever one is specified.
        //
        // Plan:
        // 1. Create a `PerformanceMonitor` object, `mX`, and register current
        //    process for statistics collection.
        //
        // 2. Obtain iterator, `mXIt`, pointing the first element in the
        //    underlying map.  Create a const reference, `XIt`, to the
        //    iterator.
        //
        // 3. Store current statistics values using accessors of the
        //    `Statistics` class.
        //
        // 4. Make a copy of origin object and verify its value using accessors
        //    of the `Statistics` class.  (C-1)
        //
        // 5. Compare origin object fields with stored values from P-3.  (C-2)
        //
        // 6. Collect latest statistics to update origin object and verify that
        //    the copy isn't affected.
        //
        // 7. Unregister current process to destroy origin object and verify
        //    that the copy isn't affected.  (C-3)
        //
        // 8. Register current process for statistics collection again.  Obtain
        //    an iterator, `mXIt`, pointing the first element in the underlying
        //    map.  Create a const reference, `XIt`, to the iterator.
        //
        // 9. Create several copies of the current process statistics, passing
        //    different allocators to the copy constructor, and verify that all
        //    memory is allocated by user-supplied allocator.  (C-4)
        //
        // Testing:
        //   Statistics(const Statistics& orig, Allocator *basicAllocator);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING STATISTICS COPY CONSTRUCTOR\n"
                          << "===================================\n";

        bslma::TestAllocator         ma("monitor", veryVeryVeryVerbose);
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocator         sa("supplied", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        Obj        mX(&ma);
        const Obj& X   = mX;
        const int  PID = bdls::ProcessUtil::getProcessId();
        int        rc  = mX.registerPid(PID, LONG_STRING);
        ASSERT(0 == rc);

        if (verbose) cout << "\tTesting copying" << endl;
        {
            ObjIterator        mXIt = X.begin();
            const ObjIterator& XIt  = mXIt;

            // Store values to verify origin object after.

            StatisticsStorage orig;

            orig.d_pid          = XIt->pid();
            orig.d_description  = XIt->description();
            orig.d_startTimeUtc = XIt->startupTime();
            orig.d_elapsedTime  = XIt->elapsedTime();

            for (int i = 0; i < Obj::e_NUM_MEASURES; ++i) {
                Obj::Measure measure = static_cast<Obj::Measure>(i);

                orig.d_lstData[measure] = XIt->latestValue(measure);
                orig.d_minData[measure] = XIt->minValue(measure);
                orig.d_maxData[measure] = XIt->maxValue(measure);
            }

            // Create a copy and verify its value.

            Obj::Statistics        mXS = *XIt;
            const Obj::Statistics& XS  = mXS;

            for (int i = 0; i < Obj::e_NUM_MEASURES; ++i) {
                Obj::Measure measure = static_cast<Obj::Measure>(i);
                ASSERT(XIt->latestValue(measure) == XS.latestValue(measure));
                ASSERT(XIt->minValue(measure)    == XS.minValue(measure)   );
                ASSERT(XIt->maxValue(measure)    == XS.maxValue(measure)   );

                // Verify that origin object is left unaffected.

                ASSERT(orig.d_lstData[measure] == XIt->latestValue(measure));
                ASSERT(orig.d_minData[measure] == XIt->minValue(measure)   );
                ASSERT(orig.d_maxData[measure] == XIt->maxValue(measure)   );
            }

            ASSERT(XIt->pid()         == XS.pid()        );
            ASSERT(XIt->description() == XS.description());
            ASSERT(XIt->startupTime() == XS.startupTime());
            ASSERT(XIt->elapsedTime() == XS.elapsedTime());

            // Verify that origin object is left unaffected.

            ASSERT(orig.d_pid          == XIt->pid()        );
            ASSERT(orig.d_description  == XIt->description());
            ASSERT(orig.d_startTimeUtc == XIt->startupTime());
            ASSERT(orig.d_elapsedTime  == XIt->elapsedTime());

            // Changing origin object.  Sleep for several ticks of the
            // coarsest clock behind `elapsedTime` (15.6 ms on Windows), so
            // that the collection changes it.

            bslmt::ThreadUtil::microSleep(100000, 0);
            mX.collect();

            ASSERT(XIt->pid()         == XS.pid()          );
            ASSERT(XIt->description() == XS.description()  );
            ASSERT(XIt->startupTime() == XS.startupTime()  );

            ASSERT(orig.d_elapsedTime != XIt->elapsedTime());
            ASSERT(orig.d_elapsedTime == XS.elapsedTime()  );

            // Destroying origin object.

            mX.unregisterPid(PID);

            for (int i = 0; i < Obj::e_NUM_MEASURES; ++i) {
                Obj::Measure measure = static_cast<Obj::Measure>(i);

                ASSERT(orig.d_lstData[measure] == XS.latestValue(measure));
                ASSERT(orig.d_minData[measure] == XS.minValue(measure)   );
                ASSERT(orig.d_maxData[measure] == XS.maxValue(measure)   );
            }

            ASSERT(orig.d_pid          == XS.pid()        );
            ASSERT(orig.d_description  == XS.description());
            ASSERT(orig.d_startTimeUtc == XS.startupTime());
            ASSERT(orig.d_elapsedTime  == XS.elapsedTime());
        }

        if (verbose) cout << "\tTesting allocator propagation" << endl;
        {
            rc  = mX.registerPid(PID, LONG_STRING);
            ASSERT(0 == rc);

            ObjIterator        mXIt = X.begin();
            const ObjIterator& XIt  = mXIt;

            const bsls::Types::Int64 NUM_MONITOR_BYTES = ma.numBytesInUse();
            ASSERT(0 != NUM_MONITOR_BYTES);

            ASSERT(NUM_MONITOR_BYTES == ma.numBytesInUse());
            ASSERT(0                 == da.numBytesInUse());
            ASSERT(0                 == sa.numBytesInUse());

            // Default allocator.

            {
                const Obj::Statistics XSD(*XIt);
                (void)XSD;  // quash potential compiler warnings

                ASSERT(NUM_MONITOR_BYTES == ma.numBytesInUse());
                ASSERT(0                 != da.numBytesInUse());
                ASSERT(0                 == sa.numBytesInUse());
            }

            ASSERT(NUM_MONITOR_BYTES == ma.numBytesInUse());
            ASSERT(0                 == da.numBytesInUse());
            ASSERT(0                 == sa.numBytesInUse());

            // Explicit null allocator.

            {
                const Obj::Statistics XSN(*XIt, 0);
                (void)XSN;  // quash potential compiler warnings

                ASSERT(NUM_MONITOR_BYTES == ma.numBytesInUse());
                ASSERT(0                 != da.numBytesInUse());
                ASSERT(0                 == sa.numBytesInUse());
            }

            ASSERT(NUM_MONITOR_BYTES == ma.numBytesInUse());
            ASSERT(0                 == da.numBytesInUse());
            ASSERT(0                 == sa.numBytesInUse());

            // Supplied allocator.

            {
                const Obj::Statistics XSS(*XIt, &sa);
                (void)XSS;  // quash potential compiler warnings

                ASSERT(NUM_MONITOR_BYTES == ma.numBytesInUse());
                ASSERT(0                 == da.numBytesInUse());
                ASSERT(0                 != sa.numBytesInUse());
            }

            ASSERT(NUM_MONITOR_BYTES == ma.numBytesInUse());
            ASSERT(0                 == da.numBytesInUse());
            ASSERT(0                 == sa.numBytesInUse());
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING `find`
        //
        // Concerns:
        // 1. The `find()` returns an iterator, referring to the value with
        //     process id, passed as a parameter, only if this process is
        //     watched by monitor.
        //
        // 2. The `find()` returns an iterator, referring to the address,
        //    following the last value in the underlying  map, if process with
        //    id, passed as a parameter, isn't watched by this monitor.
        //
        // Plan:
        // 1. Spawn several processes, create a `PerformanceMonitor` object,
        //    `mX`, and register these processes for statistics collection.
        //
        // 2. Using the loop-based approach:
        //
        //   1. Use ids of registered process to call the `find` method and
        //    verify results, by comparing them with the results of iteration
        //    from the beginning.  (C-1)
        //
        // 3. Call the `find` method with some dummy value, passed as a
        //    parameter, and verify that it returns an iterator, that
        //    represents the end of the sequence of sets of collected
        //    performance statistics. (C-2)
        //
        // Testing:
        //   ConstIterator end() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING `find`\n"
                          << "==============\n";

        typedef bsl::set<int> Pids;

        const int NUM_PROCESSES = 5;
        Pids      pids;

        const int SELF_PID = bdls::ProcessUtil::getProcessId();
        pids.insert(SELF_PID);

        // Spawn as a child processes several copies of this test driver,
        // running test case -3, which simply sleeps for half a minute and
        // exits.

        bsl::string              command(argv[0]);
        bsl::vector<bsl::string> arguments;

        arguments.push_back(bsl::string("-3"));

        for (int i = 0; i < NUM_PROCESSES; ++i) {

            processSupport::ProcessHandle handle =
                                  processSupport::exec(command, arguments);


            // Store id of the spawned child process.

            int pid = processSupport::getId(handle);
            ASSERTV(pid, -1 != pid);

            pids.insert(pid);
        }

        Obj        mX;
        const Obj& X = mX;

        Pids::iterator pidsIt = pids.begin();
        bsl::string    description;

        while (pidsIt != pids.end()) {
            int PID = *pidsIt;
            description += '0';

            if (veryVerbose) {
                T_ P_(description) P(PID)
            }

            ASSERTV(description, 0 == mX.registerPid(PID, description));

            ++pidsIt;
        }

        // Checking.

        pidsIt    = pids.begin();
        int shift = 0;

        while (pidsIt != pids.end()) {
            int                PID  = *pidsIt;
            ObjIterator        mXIt = advanceIt(X.begin(), shift);
            const ObjIterator& XIt  = mXIt;

            ASSERT(XIt == X.find(PID));

            ++shift;
            ++pidsIt;
        }

        ASSERT(X.end() != X.find(SELF_PID));
        ASSERT(X.end() != X.find(0));
        ASSERT(X.find(0) == X.find(SELF_PID));

        ASSERT(X.end() == X.find(-1));
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING `end`
        //
        // Concerns:
        // 1. The `end()` returns an  iterator, referring to the address,
        //    following the last value in the underlying  map.
        //
        // Plan:
        // 1. Spawn several processes and store their ids to separate map.
        //
        // 2. Create a `PerformanceMonitor` object, `mX`.
        //
        // 3. Using the loop-based approach:
        //
        //   1. Register processes one by one (so the number of elements in the
        //      underlying map should change).
        //
        //   2. Obtain an iterator, pointing to the first element and advance
        //      it to the position, following the last element in underlying
        //      map.
        //
        //   3. Verify return value of the `end` method by comparing it with
        //      the iterator from P-3.2. (C-1)
        //
        // Testing:
        //   ConstIterator end() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING `end`\n"
                          << "=============\n";

        typedef bsl::set<int> Pids;

        const int NUM_PROCESSES = 5;
        Pids      pids;

        // Spawn as a child processes several copies of this test driver,
        // running test case -3, which simply sleeps for half a minute and
        // exits.

        bsl::string              command(argv[0]);
        bsl::vector<bsl::string> arguments;

        arguments.push_back(bsl::string("-3"));

        for (int i = 0; i < NUM_PROCESSES; ++i) {

            processSupport::ProcessHandle handle =
                                  processSupport::exec(command, arguments);


            // Store id of the spawned child process.

            int pid = processSupport::getId(handle);
            ASSERTV(pid, -1 != pid);

            pids.insert(pid);
        }

        Obj        mX;
        const Obj& X = mX;

        ASSERT(X.begin() == X.end());

        Pids::iterator pidsIt = pids.begin();
        bsl::string    description;

        while (pidsIt != pids.end()) {
            int PID = *pidsIt;
            description += '0';

            if (veryVerbose) {
                T_ P_(description) P(PID)
            }

            ASSERTV(description, 0 == mX.registerPid(PID, description));

            ObjIterator        mXIt = advanceIt(X.begin(),
                                                X.numRegisteredPids());
            const ObjIterator& XIt  = mXIt;

            ASSERT(XIt == X.end());

            ++pidsIt;
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // ITERATOR EQUALITY-COMPARISON OPERATORS
        //
        // Concerns:
        // 1. Two objects, `X` and `Y`, compare equal if and only if they point
        //    to the same value in the same map.
        //
        // 2. `true  == (X == X)`  (i.e., identity)
        //
        // 3. `false == (X != X)`  (i.e., identity)
        //
        // 4. `X == Y` if and only if `Y == X`  (i.e., commutativity)
        //
        // 5. `X != Y` if and only if `Y != X`  (i.e., commutativity)
        //
        // 6. `X != Y` if and only if `!(X == Y)`
        //
        // 7. Non-modifiable objects can be compared (i.e., objects or
        //    references providing only non-modifiable access).
        //
        // 8. The equality operator's signature and return type are standard.
        //
        // 9. The inequality operator's signature and return type are standard.
        //
        // Plan:
        // 1. Use the respective addresses of `operator==` and
        //   `operator!=` to initialize function pointers having the
        //    appropriate signatures and return types for those methods.
        //    (C-8..9)
        //
        // 2. Spawn several processes, create a `PerformanceMonitor` object,
        //    `mX`, and register these processes for statistics collection.
        //
        // 3. For each value `(x, y)` in the cross product S x S, where S is a
        //    set of successive indexes from 0 till the number of registered
        //    processes from P-2:
        //
        //  1. Use the `begin` accessor to obtain iterators, `mXIt1` and
        //     `mXIt2`, pointing the first element in the underlying map.
        //
        //  2. Advance them x and y times accordingly, compare and verify
        //     comparison results.  (C-1..7)
        //
        // Testing:
        //   bool operator==(const ConstIterator& rhs) const;
        //   bool operator!=(const ConstIterator& rhs) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "ITERATOR EQUALITY-COMPARISON OPERATORS\n"
                          << "======================================\n";

        // Testing signatures.
        {
            typedef bool (ObjIterator::*operatorPtr)(const ObjIterator&) const;

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = &ObjIterator::operator==;
            operatorPtr operatorNe = &ObjIterator::operator!=;

            (void) operatorEq;  // quash potential compiler warnings
            (void) operatorNe;
        }

        // Testing behaviour.
        {
            typedef bsl::set<int> Pids;

            const int NUM_PROCESSES = 5;
            Pids      pids;

            // Spawn as a child processes several copies of this test driver,
            // running test case -3, which simply sleeps for half a minute and
            // exits.

            bsl::string              command(argv[0]);
            bsl::vector<bsl::string> arguments;

            arguments.push_back(bsl::string("-3"));

            for (int i = 0; i < NUM_PROCESSES; ++i) {

                processSupport::ProcessHandle handle =
                                      processSupport::exec(command, arguments);


                // Store id of the spawned child process.

                int pid = processSupport::getId(handle);
                ASSERTV(pid, -1 != pid);

                pids.insert(pid);
            }

            Obj        mX;
            const Obj& X = mX;

            Pids::iterator pidsIt = pids.begin();
            bsl::string    description;

            while (pidsIt != pids.end()) {
                int PID = *pidsIt;
                description += '0';

                if (veryVerbose) {
                    T_ P_(description) P(PID)
                }

                ASSERTV(description, 0 == mX.registerPid(PID, description));

                ++pidsIt;
            }

            // Checking.

            for (int i = 0; i < NUM_PROCESSES; ++i) {
                for (int j = 0; j < NUM_PROCESSES; ++j) {
                    ObjIterator        mXIt1 = advanceIt(X.begin(), i);
                    ObjIterator        mXIt2 = advanceIt(X.begin(), j);
                    const ObjIterator& XIt1  = mXIt1;
                    const ObjIterator& XIt2  = mXIt2;

                    ASSERTV(i, j, (i == j) == (XIt1 == XIt2));
                    ASSERTV(i, j, (i == j) == (XIt2 == XIt1));
                    ASSERTV(i, j, (i != j) == (XIt1 != XIt2));
                    ASSERTV(i, j, (i != j) == (XIt2 != XIt1));
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING INCRERMENT OPERATORS
        //
        // Concerns:
        // 1. The increment operators change the value of the object to refer
        //    to the next element in the map.
        //
        // 2. The signatures and return types are standard.
        //
        // 3. The value returned by the post-increment operator is the value of
        //    the object prior to the operator call.
        //
        // 4. The reference returned by the pre-increment operator refers to
        //    the object on which the operator was invoked.
        //
        // 5. Advancing iterator, referring to the last element in the map,
        //    sets this iterator equal to `end()`
        //
        // Plan:
        // 1. Use the respective addresses of `operator++()` and
        //    `operator++(int)` to initialize function pointers having the
        //    appropriate signatures and return types for those methods.  (C-2)
        //
        // 2. Spawn several processes, create a `PerformanceMonitor` object,
        //    `mX`, and register these processes for statistics collection.
        //
        // 3. Use the `begin` accessor to obtain iterators, `preXIt` and
        //    `postXIt`, pointing the first element in the underlying map.
        //
        // 4. Iterate through the underlying map using the `operator++()` and
        //    `operator++(int)` manipulators respectively and verify return
        //    values and values of the iterators, using dereference operator
        //    and `pid` accessor of `Statistics` class.  (C-1,3..4)
        //
        // 5. Advance iterators once more and use (as yet unproven) `end`
        //    accessor to verify positions of the iterators.  (C-5)
        //
        // Testing:
        //   ConstIterator& operator++();
        //   ConstIterator operator++(int);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING INCREMENT OPERATORS\n"
                          << "===========================\n";

        // Testing signatures.
        {
            typedef ObjIterator  (ObjIterator::*postOperatorPtr)(int);
            typedef ObjIterator& (ObjIterator::*preOperatorPtr )(   );

            // Verify that the signature and return type are standard.

            postOperatorPtr operatorPostInc = &ObjIterator::operator++;
            preOperatorPtr  operatorPreInc  = &ObjIterator::operator++;

            (void) operatorPostInc;  // quash potential compiler warning
            (void) operatorPreInc;   // quash potential compiler warning
        }

        // Testing behaviour.
        {
            typedef bsl::set<int> Pids;

            const int NUM_PROCESSES = 5;
            Pids      pids;

            // Spawn as a child processes several copies of this test driver,
            // running test case -3, which simply sleeps for half a minute and
            // exits.

            bsl::string              command(argv[0]);
            bsl::vector<bsl::string> arguments;

            arguments.push_back(bsl::string("-3"));

            for (int i = 0; i < NUM_PROCESSES; ++i) {

                processSupport::ProcessHandle handle =
                                      processSupport::exec(command, arguments);


                // Store id of the spawned child process.

                int pid = processSupport::getId(handle);
                ASSERTV(pid, -1 != pid);

                pids.insert(pid);
            }

            Obj        mX;
            const Obj& X = mX;

            Pids::iterator pidsIt = pids.begin();
            bsl::string    description;

            while (pidsIt != pids.end()) {
                int PID = *pidsIt;
                description += '0';

                if (veryVerbose) {
                    T_ P_(description) P(PID)
                }

                ASSERTV(description, 0 == mX.registerPid(PID, description));

                ++pidsIt;
            }

            // Checking pre-increment iterator.

            ObjIterator preXIt = X.begin();
            pidsIt = pids.begin();

            for (int i = 0; i < NUM_PROCESSES - 1; ++i) {
                ASSERTV(i,    *pidsIt  ==    preXIt->pid());
                ASSERTV(i, *(++pidsIt) == (++preXIt)->pid());
                ASSERTV(i,    *pidsIt  ==    preXIt->pid());
            }

            ++preXIt;
            ASSERT(X.end() == preXIt);

            // Checking post-increment iterator.

            ObjIterator postXIt = X.begin();
            pidsIt = pids.begin();

            for (int i = 0; i < NUM_PROCESSES - 1; ++i) {
                ASSERTV(i, *pidsIt     == postXIt->pid());
                ASSERTV(i, *pidsIt     == (postXIt++)->pid());
                ASSERTV(i, *(++pidsIt) == postXIt->pid());
            }

            postXIt++;
            ASSERT(X.end() == postXIt);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CONCERN: Statistics are Reset Correctly
        //
        // Concerns:
        //  1. That CPU statics are reset to zero after calling
        //     `resetStatistics`.
        //
        // Plan:
        //  1. Create a performance monitor, collect statistics, reset the
        //     statics, and verify that the returned statistics after the reset
        //     are plausible (Note that drqs 49280976, reset statistics lead to
        //     incorrectly determining the time since the last reset, and
        //     wildly incorrect statics after a reset).
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

            Obj perfmon(&ta);

            int pid = bdls::ProcessUtil::getProcessId();

            int rc = perfmon.registerPid(pid, "perfmon");
            ASSERT(0 == rc);

            // Because of how CPU utilization is collected, the first collect
            // is always used as a baseline and hence is ignored
            perfmon.collect();

            ObjIterator perfmonIter = perfmon.find(pid);
            ASSERT(perfmonIter != perfmon.end());

            double userCpu = 0, systemCpu = 0, totalCpu = 0;

            // Burn some CPU
            controlledCpuBurn();
            perfmon.collect();

            userCpu   = perfmonIter->avgValue(Obj::e_CPU_UTIL_USER);
            systemCpu = perfmonIter->avgValue(Obj::e_CPU_UTIL_SYSTEM);
            totalCpu  = perfmonIter->avgValue(Obj::e_CPU_UTIL);

            if (verbose) {
                cout << "  userCpu = " << userCpu
                     << ", systemCpu = " << systemCpu
                     << ", totalCpu = " << totalCpu
                     << endl;
            }

            // Reset our statistics.  Note that `resetStatistics` collects a
            // new baseline.

            const bsls::TimeInterval wallBefore = bdlt::CurrentTime::now();
            const CpuSample          outerBefore;
            perfmon.resetStatistics();
            const double             START = perfmonIter->elapsedTime();

            // At this point, the average CPU utilizations should be zero

            // Sleep for 4 seconds (without consuming too much CPU)
            bslmt::ThreadUtil::microSleep(0, 4);
            perfmon.collect();
            const CpuSample          outerAfter;
            const bsls::TimeInterval wallAfter  = bdlt::CurrentTime::now();

            // The interval over which the utilization is computed starts
            // after the reset begins, and ends after the 4 second sleep and
            // before the final `collect` returns.  Note that the monitor
            // computes `elapsedTime` from `bdlt::CurrentTime::now()`, so the
            // same (realtime) clock is used here; a step of the system clock
            // during this test would invalidate these bounds.  `SLACK` allows
            // for clock resolution.

            const double SLACK = 0.05;

            const double INTERVAL     = perfmonIter->elapsedTime() - START;
            const double MAX_INTERVAL = (wallAfter - wallBefore)
                                                      .totalSecondsAsDouble()
                                      + SLACK;
            ASSERTV(INTERVAL, 4.0 - SLACK <= INTERVAL);
            ASSERTV(INTERVAL, MAX_INTERVAL, INTERVAL <= MAX_INTERVAL);

            userCpu   = perfmonIter->avgValue(Obj::e_CPU_UTIL_USER);
            systemCpu = perfmonIter->avgValue(Obj::e_CPU_UTIL_SYSTEM);
            totalCpu  = perfmonIter->avgValue(Obj::e_CPU_UTIL);

            if (verbose) {
                cout << "  userCpu = " << userCpu
                     << ", systemCpu = " << systemCpu
                     << ", totalCpu = " << totalCpu
                     << endl;
            }

            // The only CPU time this process uses after the reset is the
            // work of the `collect` calls themselves.  On Windows, that work
            // (a PDH instance lookup that enumerates every process on the
            // host) uses an amount of system CPU time that depends on the load
            // on the test host, and has been over 35% of the interval.  So
            // rather than compare the utilization with a fixed threshold,
            // bound the CPU time it implies (over the interval verified above)
            // by the native CPU time around the reset and the final
            // `collect`.  A reset that does not discard the CPU time used by
            // `controlledCpuBurn` exceeds this bound.  `SLACK` also allows for
            // clock-tick rounding of the monitor's CPU times (e.g., 10 ms on
            // Linux).

            const double USER_TIME   = userCpu   / 100.0 * INTERVAL;
            const double SYSTEM_TIME = systemCpu / 100.0 * INTERVAL;
            const double CPU_TIME    = totalCpu  / 100.0 * INTERVAL;

            const double MAX_USER   = outerAfter.d_user - outerBefore.d_user
                                    + SLACK;
            const double MAX_SYSTEM = outerAfter.d_system
                                    - outerBefore.d_system
                                    + SLACK;
            const double MAX_CPU    = cpuTime(outerBefore, outerAfter)
                                    + SLACK;

            if (veryVerbose) {
                T_ P_(INTERVAL) P_(USER_TIME) P_(SYSTEM_TIME) P(CPU_TIME)
                T_ P_(MAX_USER) P_(MAX_SYSTEM) P(MAX_CPU)
            }

            ASSERTV(userCpu,   0.0 <= userCpu);
            ASSERTV(systemCpu, 0.0 <= systemCpu);
            ASSERTV(totalCpu,  0.0 <= totalCpu);

            ASSERTV(USER_TIME,   MAX_USER,   USER_TIME   <= MAX_USER);
            ASSERTV(SYSTEM_TIME, MAX_SYSTEM, SYSTEM_TIME <= MAX_SYSTEM);
            ASSERTV(CPU_TIME,    MAX_CPU,    CPU_TIME    <= MAX_CPU);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS
        //
        // Concerns:
        // 1. The `begin()` accessor of `PerformanceMonitor` class returns an
        //    iterator, referring to the first value in the underlying map.
        //
        // 2. The `operator*` of `ConstIterator` class returns the reference to
        //    the value of the element, this object refers to.
        //
        // 3. The `operator->` of `ConstIterator` class returns the address of
        //    of the element, this object refers to.
        //
        // 4. The operators return references to actual `Statistics` object,
        //    belonging to `PerformanceMonitor`, and not to some cached copy.
        //
        // 5. All methods are declared `const`.
        //
        // 6. The signatures and return types of operators are standard.
        //
        // Plan:
        // 1. Use the addresses of `operator*` and `operator->` to initialize
        //    member-function pointers having the appropriate signatures and
        //    return types for the operators defined in this component.  (C-6)
        //
        // 2. Spawn several processes and store their ids to separate map.
        //
        // 3. Create a `PerformanceMonitor` object, `mX` and const reference to
        //    `mX`, named `X`.
        //
        // 4. Using the loop-based approach:
        //
        //   1. Register processes one by one in back order (so the `begin`
        //      accessor should return different values on each iteration).
        //
        //   2. Use the `begin` accessor to obtain iterator, `mXIt` and
        //      create a const reference to `mXIt`, named `XIt`.  (C-5)
        //
        //   3. Using `operator*`, `operator->` and `pid` accessor of the
        //      `Statistics` class verify return values of `begin` accessor.
        //      (C-1..3)
        //
        // 4. Use the `begin` accessor to obtain iterator, `mXIt` and create a
        //    const reference to `mXIt`, named `XIt`.  Get and store some value
        //    from the `Statistics` object, this iterator pointing to.
        //
        // 5. Use the `collect` manipulator to change `Statistics` object,
        //    stored in `PerformanceMonitor`.
        //
        // 6. Verify that value of the object, iterator pointing to, is
        //    changed.  (C-4)
        //
        // Testing:
        //   ConstIterator begin() const;
        //   reference operator*() const;
        //   pointer operator->() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING ACCESSORS\n"
                          << "=================\n";

        if (verbose) cout << "Testing signatures." << endl;
        {
            typedef const Obj::Statistics& (ObjIterator::*opDPtr)() const;
            typedef const Obj::Statistics* (ObjIterator::*opAPtr)() const;


            // Verify that the signature and return type are standard.

            opDPtr operatorDereference = &ObjIterator::operator*;
            opAPtr operatorArrow       = &ObjIterator::operator->;

            (void) operatorDereference;  // quash potential compiler warning
            (void) operatorArrow;        // quash potential compiler warning
        }

        if (verbose) cout << "Testing behavior." << endl;
        {
            typedef bsl::set<int> Pids;

            const int NUM_PROCESSES = 5;
            Pids      pids;

            // Spawn as a child processes several copies of this test driver,
            // running test case -3, which simply sleeps for half a minute and
            // exits.

            bsl::string              command(argv[0]);
            bsl::vector<bsl::string> arguments;

            arguments.push_back(bsl::string("-3"));

            for (int i = 0; i < NUM_PROCESSES; ++i) {

                processSupport::ProcessHandle handle =
                                      processSupport::exec(command, arguments);


                // Store id of the spawned child process.

                int pid = processSupport::getId(handle);
                ASSERTV(pid, -1 != pid);

                pids.insert(pid);
            }

            // Checking.
            Obj        mX;
            const Obj& X = mX;

            ASSERT( X.end() == X.begin());

            Pids::reverse_iterator rPidsIt = pids.rbegin();
            bsl::string            description;

            while (rPidsIt != pids.rend()) {
                int PID = *rPidsIt;
                description += '0';

                if (veryVerbose) {
                    T_ P_(description) P(PID)
                }

                ASSERTV(description, 0 == mX.registerPid(PID, description));

                ObjIterator        mXIt = X.begin();
                const ObjIterator& XIt  = mXIt;
                ASSERTV(description, PID ==   XIt->pid());
                ASSERTV(description, PID == (*XIt).pid());

                ++rPidsIt;
            }

            // Check that accessors return reference to actual `Statistics`
            // object belonging to `PerformanceMonitor`, and not to some cached
            // copy.

            ObjIterator        mXIt    = X.begin();
            const ObjIterator& XIt     = mXIt;
            const double       ELAPSED = XIt->elapsedTime();

            ASSERT(ELAPSED ==   XIt->elapsedTime());
            ASSERT(ELAPSED == (*XIt).elapsedTime());

            bslmt::ThreadUtil::microSleep(0, 1);
            mX.collect();

            ASSERT(ELAPSED !=   XIt->elapsedTime());
            ASSERT(ELAPSED != (*XIt).elapsedTime());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PROCESS START TIME SANITY
        //
        // Concerns:
        //  1. It appears that for some versions of Linux, either different
        //     kernels or different distros, our estimation of process start
        //     time will be wildly inaccurate.  Determine whether this is the
        //     case.
        //
        // Testing:
        //   CONCERN: The Process Start Time is Reasonable
        // --------------------------------------------------------------------

        if (verbose) cout << "PROCESS START TIME SANITY\n"
                             "=========================\n";

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        Obj perfmon(&ta);

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
      case 2: {
        // --------------------------------------------------------------------
        // TESTING `numRegisteredPids`
        //
        // Concerns:
        //  1. `numRegisteredPids` reports the correct number of registered
        //     processes.
        //
        //  2. `numRegisterPids` reports 0 before any processes are registered.
        //
        //  3. `numRegisterPids` increments by 1 every time a process is
        //     registered.
        //
        //  4. `numRegisterPids` decrements by 1 every time a process is
        //     un-registered.
        //
        // Plan:
        //  1. Using the ad-doc approach, monitor this test driver process, and
        //     one or more child processes, spawned as necessary, and check the
        //     value returned by `numRegisteredPids` before and after each
        //     process is registered.  (C-1..3)
        //
        //  2. Un-register processes registered in step 1, and check the value
        //     returned by `numRegisteredPids`.  (C-1,4)
        //
        // Testing:
        //   int numRegisteredPids()
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING `numRegisteredPids`\n"
                          << "===========================\n";

        bdlmt::TimerEventScheduler scheduler;
        Obj                        perfmon(&scheduler, 1.0);

        ASSERT(0 == perfmon.numRegisteredPids());

        // Register this test driver process.

        ASSERT(0 == perfmon.registerPid(0, "mytask"));
        ASSERT(1 == perfmon.numRegisteredPids());

        // Spawn as a child process another copy of this test driver, running
        // test case -3, which simply sleeps for half a minute and exits.

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
        //  1. The class is sufficiently functional to enable comprehensive
        //     testing in subsequent test cases.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                          << "==============\n";

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        {
            {
                Obj perfmon(&ta);
                int rc;

                rc = perfmon.registerPid(0, "mytask");
                ASSERT(0 == rc);

                for (int i = 0; i < 6; ++i) {
                    wasteCpuTime();

                    perfmon.collect();

                    for (ObjIterator it  = perfmon.begin();
                                     it != perfmon.end();
                                   ++it)
                    {
                        const Obj::Statistics &stats = *it;

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

                Obj perfmon(&scheduler, 1.0, &ta);
                int rc;

                rc = perfmon.registerPid(0, "mytask");
                ASSERT(0 == rc);

                for (int i = 0; i < 6; ++i) {
                    bslmt::ThreadUtil::microSleep(250000, 0);
                    for (ObjIterator it  = perfmon.begin();
                                     it != perfmon.end();
                                   ++it)
                    {
                        const Obj::Statistics& stats = *it;

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

        ASSERT(0 == bslma::Default::setDefaultAllocator(&ta));
        bslma::Default::setGlobalAllocator(&ta);

        {
            Obj                perfmon(&ta);

            bsl::size_t        bufferSize          = 0;

            double             virtualSize         = 0;
            double             residentSize        = 0;

            bsls::Types::Int64 currentBytesInUse   = 0;
            bsls::Types::Int64 peakBytesInUse      = 0;

            Obj::Measure       virtualSizeMeasure  = Obj::e_VIRTUAL_SIZE;
            Obj::Measure       residentSizeMeasure = Obj::e_RESIDENT_SIZE;

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
        //   Sleep for 30 seconds.
        //
        //   This test case provides a target for spawning a child process in
        //   other test cases.
        //
        // Concerns:
        //  1. Test runs for at least 30 seconds.
        //
        // Plan:
        //  1. Confirm empirically by observing a test run.  (C-1)
        //
        // Testing:
        //   DUMMY TEST CASE
        // --------------------------------------------------------------------

        if (verbose) cout << "DUMMY TEST CASE\n"
                          << "===============\n";

        bslmt::ThreadUtil::microSleep(0, 30);
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

#endif  // !(defined(BSLS_PLATFORM_OS_SOLARIS)
        //     && defined(BSLS_PLATFORM_CMP_GNU)
        //     && defined(BSLS_PLATFORM_CPU_32_BIT)
        //     && BSLS_PLATFORM_CMP_VERSION < 120000)

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
