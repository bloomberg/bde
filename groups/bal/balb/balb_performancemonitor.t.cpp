// balb_performancemonitor.t.cpp                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balb_performancemonitor.h>

#include <bdls_processutil.h>

#include <bdlt_currenttime.h>
#include <bdlt_datetimeinterval.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_deque.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_set.h>
#include <bsl_sstream.h>
#include <bsl_stdexcept.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bslmt_barrier.h>
#include <bslmt_threadutil.h>

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
// Thread-safety concerns are not tested because of limitations of test
// apparatus and component's design.
// ----------------------------------------------------------------------------
//                            // ----------------
//                            // class Statistics
//                            // ----------------
// CREATORS
// [10] Statistics(const Statistics& orig, Allocator *basicAllocator);
//
//                           // -------------------
//                           // class ConstIterator
//                           // -------------------
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
// CLASS METHODS
// [ 4] ConstIterator begin() const;
// [ 8] ConstIterator end() const;
// [ 9] ConstIterator find(int pid) const;
// [ 2] int numRegisteredPids() const
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] USAGE EXAMPLE
// [ 3] CONCERN: The Process Start Time is Reasonable
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

typedef balb::PerformanceMonitor                Obj;
typedef balb::PerformanceMonitor::ConstIterator ObjIterator;

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

// Define 'bsl::string' value long enough to ensure dynamic memory allocation.

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

struct StatisticsStorage
    // The structure to store values accessible via public interfaces of
    // 'balb::PerformanceMonitor::Statistics' class.
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
    commandLine.append(app);
    for (Args::iterator i = arguments.begin(); i != arguments.end(); ++i) {
        commandLine.append(" ");
        commandLine.append(*i);
    }

    // N.B. The 'lpCommandLine' argument of 'CreateProcess' must be mutable,
    // per requirements of 'CreateProcessEx' (cited in documentation for
    // 'CreateProcess').
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

double wasteCpuTime()
    // Just take up a measurable amount of cpu time.  Try 100 clock ticks (1.0
    // seconds or less).
{
#ifdef BSLS_PLATFORM_OS_UNIX
    struct tms tmsBuffer;
    clock_t    rc = times(&tmsBuffer);
    BSLS_ASSERT(-1 != rc);
    clock_t startTime = tmsBuffer.tms_utime + tmsBuffer.tms_stime;

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

ObjIterator advanceIt(const ObjIterator& begin, int n)
    // Return a copy of the specified 'begin', incremented by the specified 'n'
    // elements.
{
    ObjIterator ret = begin;
    for (int i = 0; i < n; ++i) {
        ++ret;
    }

    return ret;
}

}  // close unnamed namespace

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

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters and replace 'assert' with 'ASSERT'.
        //:   (C-1)
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
///Example 1: Basic Use of 'balb::PerformanceMonitor'
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example shows how to monitor the currently executing process
// and produce a formatted report of the collected measures after a certain
// interval.
//
// First, we instantiate a scheduler used by the performance monitor to
// schedule collection events.
//..
    bdlmt::TimerEventScheduler scheduler;
    scheduler.start();
//..
// Then, we create the performance monitor, monitoring the current process and
// auto-collecting statistics every second.
//..
    balb::PerformanceMonitor perfmon(&scheduler, 1.0);
    int                      rc  = perfmon.registerPid(0, "perfmon");
    const int                pid = bdls::ProcessUtil::getProcessId();

    ASSERT(0 == rc);
    ASSERT(1 == perfmon.numRegisteredPids());
//..
// Next, we print a formatted report of the performance statistics collected
// for each pid every 5 seconds for half a minute.  Note, that 'Statistics'
// object can be simultaneously modified by scheduler callback and accessed via
// a 'ConstIterator'.  To ensure that the call to 'Statistics::print' outputs
// consistent data from a single update of the statistics for this process, we
// create a local copy (copy construction is guaranteed to be thread-safe).
//..
    for (int i = 0; i < 6; ++i) {
        bslmt::ThreadUtil::microSleep(0, 5);

        balb::PerformanceMonitor::ConstIterator    it    = perfmon.begin();
        const balb::PerformanceMonitor::Statistics stats = *it;

        ASSERT(pid == stats.pid());

        bsl::cout << "PID = " << stats.pid() << ":\n";
        stats.print(bsl::cout);
    }
//..
// Finally, we unregister the process and stop the scheduler to cease
// collecting statistics for this process.  It is safe to call 'unregisterPid'
// here, because we don't have any 'ConstIterators' objects or references to
// 'Statistics' objects.
//..
    rc  = perfmon.unregisterPid(pid);

    ASSERT(0 == rc);
    ASSERT(0 == perfmon.numRegisteredPids());

    scheduler.stop();
//..
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STATISTICS COPY CONSTRUCTOR
        //
        // Concerns:
        //: 1 The new object aggregates the same values as the original object.
        //:
        //: 2 The value of the original object is left unaffected.
        //:
        //: 3 Subsequent changes in or destruction of the source object have no
        //:   effect on the copy-constructed object.
        //:
        //: 4 The object has its internal memory management system hooked up
        //:   properly so that *all* internally allocated memory draws from a
        //:   user-supplied allocator whenever one is specified.
        //
        // Plan:
        //: 1 Create a 'PerformanceMonitor' object, 'mX', and register current
        //:   process for statistics collection.
        //:
        //: 2 Obtain iterator, 'mXIt', pointing the first element in the
        //:   underlying map.  Create a const reference, 'XIt', to the
        //:   iterator.
        //:
        //: 3 Store current statistics values using accessors of the
        //:   'Statistics' class.
        //:
        //: 4 Make a copy of origin object and verify its value using accessors
        //:   of the 'Statistics' class.  (C-1)
        //:
        //: 5 Compare origin object fields with stored values from P-3.  (C-2)
        //:
        //: 6 Collect latest statistics to update origin object and verify that
        //:   the copy isn't affected.
        //:
        //: 7 Unregister current process to destroy origin object and verify
        //:   that the copy isn't affected.  (C-3)
        //:
        //: 8 Register current process for statistics collection again.  Obtain
        //:   an iterator, 'mXIt', pointing the first element in the underlying
        //:   map.  Create a const reference, 'XIt', to the iterator.
        //:
        //: 9 Create several copies of the current process statistics, passing
        //:   different allocators to the copy constructor, and verify that all
        //:   memory is allocated by user-supplied allocator.  (C-4)
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

            // Changing origin object.

            bslmt::ThreadUtil::microSleep(10000, 0);
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
        // TESTING 'find'
        //
        // Concerns:
        //: 1 The 'find()' returns an iterator, referring to the value with
        //:    process id, passed as a parameter, only if this process is
        //:    watched by monitor.
        //:
        //: 2 The 'find()' returns an iterator, referring to the address,
        //:   following the last value in the underlying  map, if process with
        //:   id, passed as a parameter, isn't watched by this monitor.
        //
        // Plan:
        //: 1 Spawn several processes, create a 'PerformanceMonitor' object,
        //:   'mX', and register these processes for statistics collection.
        //:
        //: 2 Using the loop-based approach:
        //:
        //:   1 Use ids of registered process to call the 'find' method and
        //:   verify results, by comparing them with the results of iteration
        //:   from the beginning.  (C-1)
        //:
        //: 3 Call the 'find' method with some dummy value, passed as a
        //:   parameter, and verify that it returns an iterator, that
        //:   represents the end of the sequence of sets of collected
        //:   performance statistics. (C-2)
        //
        // Testing:
        //   ConstIterator end() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'find'\n"
                          << "==============\n";

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

        ASSERT(X.end() == X.find(-1));
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'end'
        //
        // Concerns:
        //: 1 The 'end()' returns an  iterator, referring to the address,
        //:   following the last value in the underlying  map.
        //
        // Plan:
        //: 1 Spawn several processes and store their ids to separate map.
        //:
        //: 2 Create a 'PerformanceMonitor' object, 'mX'.
        //:
        //: 3 Using the loop-based approach:
        //:
        //:   1 Register processes one by one (so the number of elements in the
        //:     underlying map should change).
        //:
        //:   2 Obtain an iterator, pointing to the first element and advance
        //:     it to the position, following the last element in underlying
        //:     map.
        //:
        //:   3 Verify return value of the 'end' method by comparing it with
        //:     the iterator from P-3.2. (C-1)
        //
        // Testing:
        //   ConstIterator end() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'end'\n"
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
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if they point
        //:   to the same value in the same map.
        //:
        //: 2 'true  == (X == X)'  (i.e., identity)
        //:
        //: 3 'false == (X != X)'  (i.e., identity)
        //:
        //: 4 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 5 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 6 'X != Y' if and only if '!(X == Y)'
        //:
        //: 7 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 8 The equality operator's signature and return type are standard.
        //:
        //: 9 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and
        //:  'operator!=' to initialize function pointers having the
        //:   appropriate signatures and return types for those methods.
        //:   (C-8..9)
        //:
        //: 2 Spawn several processes, create a 'PerformanceMonitor' object,
        //:   'mX', and register these processes for statistics collection.
        //:
        //: 3 For each value '(x, y)' in the cross product S x S, where S is a
        //:   set of successive indexes from 0 till the number of registered
        //:   processes from P-2:
        //:
        //:  1 Use the 'begin' accessor to obtain iterators, 'mXIt1' and
        //:    'mXIt2', pointing the first element in the underlying map.
        //:
        //:  2 Advance them x and y times accordingly, compare and verify
        //:    comparison results.  (C-1..7)
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
        //: 1 The increment operators change the value of the object to refer
        //:   to the next element in the map.
        //:
        //: 2 The signatures and return types are standard.
        //:
        //: 3 The value returned by the post-increment operator is the value of
        //:   the object prior to the operator call.
        //:
        //: 4 The reference returned by the pre-increment operator refers to
        //:   the object on which the operator was invoked.
        //:
        //: 5 Advancing iterator, referring to the last element in the map,
        //:   sets this iterator equal to 'end()'
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator++()' and
        //:   'operator++(int)' to initialize function pointers having the
        //:   appropriate signatures and return types for those methods.  (C-2)
        //:
        //: 2 Spawn several processes, create a 'PerformanceMonitor' object,
        //:   'mX', and register these processes for statistics collection.
        //:
        //: 3 Use the 'begin' accessor to obtain iterators, 'preXIt' and
        //:   'postXIt', pointing the first element in the underlying map.
        //:
        //: 4 Iterate through the underlying map using the 'operator++()' and
        //:   'operator++(int)' manipulators respectively and verify return
        //:   values and values of the iterators, using dereference operator
        //:   and 'pid' accessor of 'Statistics' class.  (C-1,3..4)
        //:
        //: 5 Advance iterators once more and use (as yet unproven) 'end'
        //:   accessor to verify positions of the iterators.  (C-5)
        //
        // Testing:
        //   ConstIterator& operator++();
        //   ConstIterator operator++(int);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING INCRERMENT OPERATORS\n"
                          << "============================\n";

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

            // Reset our statistics
            perfmon.resetStatistics();

            // At this point, the average CPU utilizations should be zero

            // Sleep for 1 second (without consuming too much CPU)
            bslmt::ThreadUtil::microSleep(0, 1);
            perfmon.collect();

            // We called collect almost a second after restting statistics, so
            // our CPU utilization should be really low

            userCpu   = perfmonIter->avgValue(Obj::e_CPU_UTIL_USER);
            systemCpu = perfmonIter->avgValue(Obj::e_CPU_UTIL_SYSTEM);
            totalCpu  = perfmonIter->avgValue(Obj::e_CPU_UTIL);

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
        // TESTING ACCESSORS
        //
        // Concerns:
        //: 1 The 'begin()' accessor of 'PerformanceMonitor' class returns an
        //:   iterator, referring to the first value in the underlying map.
        //:
        //: 2 The 'operator*' of 'ConstIterator' class returns the reference to
        //:   the value of the element, this object refers to.
        //:
        //: 3 The 'operator->' of 'ConstIterator' class returns the address of
        //:   of the element, this object refers to.
        //:
        //: 4 The operators return references to actual 'Statistics' object,
        //:   belonging to 'PerformanceMonitor', and not to some cached copy.
        //:
        //: 5 All methods are declared 'const'.
        //:
        //: 6 The signatures and return types of operators are standard.
        //
        // Plan:
        //: 1 Use the addresses of 'operator*' and 'operator->' to initialize
        //:   member-function pointers having the appropriate signatures and
        //:   return types for the operators defined in this component.  (C-6)
        //:
        //: 2 Spawn several processes and store their ids to separate map.
        //:
        //: 3 Create a 'PerformanceMonitor' object, 'mX' and const reference to
        //:   'mX', named 'X'.
        //:
        //: 4 Using the loop-based approach:
        //:
        //:   1 Register processes one by one in back order (so the 'begin'
        //:     accessor should return different values on each iteration).
        //:
        //:   2 Use the 'begin' accessor to obtain iterator, 'mXIt' and
        //:     create a const reference to 'mXIt', named 'XIt'.  (C-5)
        //:
        //:   3 Using 'operator*', 'operator->' and 'pid' accessor of the
        //:     'Statistics' class verify return values of 'begin' accessor.
        //:     (C-1..3)
        //:
        //: 4 Use the 'begin' accessor to obtain iterator, 'mXIt' and create a
        //:   const reference to 'mXIt', named 'XIt'.  Get and store some value
        //:   from the 'Statistics' object, this iterator pointing to.
        //:
        //: 5 Use the 'collect' manipulator to change 'Statistics' object,
        //:   stored in 'PerformanceMonitor'.
        //:
        //: 6 Verify that value of the object, iterator pointing to, is
        //:   changed.  (C-4)
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

            // Check that accessors return reference to actual 'Statistics'
            // object belonging to 'PerformanceMonitor', and not to some cached
            // copy.

            ObjIterator        mXIt    = X.begin();
            const ObjIterator& XIt     = mXIt;
            const double       ELAPSED = XIt->elapsedTime();

            ASSERT(ELAPSED ==   XIt->elapsedTime());
            ASSERT(ELAPSED == (*XIt).elapsedTime());

            bslmt::ThreadUtil::microSleep(10000, 0);
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
                    bslmt::ThreadUtil::microSleep(0, 1);
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

        bslma::Default::setDefaultAllocator(&ta);
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
        //   test case 2.
        //
        // Concerns:
        //:  1 Test runs for at least 30 seconds.
        //
        // Plan:
        //:  1 Confirm empirically by observing a test run.  (C-1)
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
