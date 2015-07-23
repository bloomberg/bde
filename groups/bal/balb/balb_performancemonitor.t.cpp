// balb_performancemonitor.t.cpp                                      -*-C++-*-

#include <balb_performancemonitor.h>

#include <ball_defaultobserver.h>
#include <ball_log.h>
#include <ball_loggermanager.h>
#include <ball_loggermanagerconfiguration.h>
#include <ball_severity.h>

#include <bdlsu_processutil.h>

#include <bdlt_currenttime.h>

#include <bslma_testallocator.h>
#include <bdlmtt_xxxthread.h>

#include <bslma_newdeleteallocator.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_deque.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_stdexcept.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <sys/mman.h>
#include <sys/times.h>
#endif

using namespace BloombergLP;

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] int numRegisteredPids() const
//----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE
// [ 4] CONCERN: The Process Start Time is Reasonable
// [ 5] CONCERN: Statistics are Reset Correctly (DRQS 49280976)
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"
#define T_() bsl::cout << '\t' << bsl::flush; // Print tab w/o newline.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

const char LOG_CATEGORY[] = "BAEA.PERFORMANCEMONITOR.TEST";

//=============================================================================
//                        HELPER FUNCTIONS AND CLASSES
//-----------------------------------------------------------------------------

#define INITIALIZE_LOGGER() \
    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;\
\
    ball::DefaultObserver               observer(&bsl::cout); \
    ball::LoggerManagerConfiguration    configuration; \
    ball::LoggerManager::initSingleton(\
            &observer, \
            configuration, \
            &bslma::NewDeleteAllocator::singleton());\
\
    ball::Severity::Level passthrough = ball::Severity::BAEL_OFF;\
\
    if (verbose) passthrough         = ball::Severity::BAEL_WARN;\
    if (veryVerbose) passthrough     = ball::Severity::BAEL_INFO;\
    if (veryVeryVerbose) passthrough = ball::Severity::BAEL_TRACE;\
\
    ball::LoggerManager::singleton().setDefaultThresholdLevels(\
                                       ball::Severity::BAEL_OFF,\
                                       passthrough,\
                                       ball::Severity::BAEL_OFF,\
                                       ball::Severity::BAEL_OFF)

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

#ifndef BSLS_PLATFORM_OS_WINDOWS
namespace test {

class MmapAllocator : public bslma::Allocator {

    typedef bsl::map<void*, bsl::size_t> MapType;

    MapType           d_map;
    bslma::Allocator *d_allocator_p;

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
    bdlmtt::ThreadUtil::microSleep(0, 1);
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
        //: 1 That CPU statics are reset to zero after calling
        //:   'resetStatistics'.
        //
        // Plan:
        //: 1 Create a performance monitor, collect statistics, reset the
        //:   statics, and verify that the returned statiscs after the reset
        //:   are plausible (Note that drqs 49280976, reset statistics lead to
        //:   incorrectly determining the time since the last reset, and
        //:   wildly incorrect statics after a reset).
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING RESET STATISTICS\n"
                          << "========================\n";

        if (verbose) cout << "\tPerform a reset and verify metrics"
                          << " are set to 0\n";
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);

            balb::PerformanceMonitor perfmon(&ta);

            int pid = bdlsu::ProcessUtil::getProcessId();

            int rc = perfmon.registerPid(pid, "perfmon");
            ASSERT(0 == rc);

            // Because of how CPU utlization is collected, the first collect is
            // always used as a baseline and hence is ignored
            perfmon.collect();

            balb::PerformanceMonitor::ConstIterator perfmonIter =
                                                             perfmon.find(pid);
            ASSERT(perfmonIter != perfmon.end());

            double userCpu = 0, systemCpu = 0, totalCpu = 0;

            // Burn some CPU
            controlledCpuBurn();
            perfmon.collect();

            userCpu = perfmonIter->avgValue(
                                  balb::PerformanceMonitor::BAEA_CPU_UTIL_USER);
            systemCpu = perfmonIter->avgValue(
                                balb::PerformanceMonitor::BAEA_CPU_UTIL_SYSTEM);
            totalCpu = perfmonIter->avgValue(
                                       balb::PerformanceMonitor::BAEA_CPU_UTIL);

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
            bdlmtt::ThreadUtil::microSleep(0, 1);
            perfmon.collect();

            // We called collect almost a second after restting statistics, so
            // our CPU utilization should be really low

            userCpu = perfmonIter->avgValue(
                                  balb::PerformanceMonitor::BAEA_CPU_UTIL_USER);
            systemCpu = perfmonIter->avgValue(
                                balb::PerformanceMonitor::BAEA_CPU_UTIL_SYSTEM);
            totalCpu = perfmonIter->avgValue(
                                       balb::PerformanceMonitor::BAEA_CPU_UTIL);

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
        // CONCERN: The Process Start Time is Reasonable
        //
        // Concerns:
        //   it appears that for some versions of Linux, either different
        //   kernels or different distros, our estimation of process start
        //   time will be wildly inaccurate.  Determine whether this is the
        //   case.
        // --------------------------------------------------------------------

        if (verbose) cout << "PROCESS START TIME SANITY\n"
                             "=========================\n";

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        balb::PerformanceMonitor perfmon(&ta);

        int rc = perfmon.registerPid(0, "perfmon");
        ASSERT(0 == rc);

        bdlt::Datetime st = perfmon.begin()->startupTime();// process start time

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
        //   The usage example shown in the component-level documentation
        //   compiles and executes as expected.
        //
        // Plan:
        //   Implement the test exactly as shown in the example.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Usage Example"
                 << endl
                 << "====================="
                 << endl;
        }

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
                return testStatus;
            }

            // Print a formatted report of the performance statistics every 10
            // seconds for one minute.  Note that the report interval may vary
            // from the collection interval.
            for (int i = 0; i < 6; ++i) {
                bdlmtt::ThreadUtil::microSleep(0, 1);
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
        // Tests:
        //   numRegisteredPids()
        // --------------------------------------------------------------------
        bdlmt::TimerEventScheduler scheduler;
        balb::PerformanceMonitor perfmon(&scheduler, 1.0);

        ASSERT(0 == perfmon.numRegisteredPids());

        ASSERT(0 == perfmon.registerPid(0, "mytask"));
        ASSERT(1 == perfmon.numRegisteredPids());

#if defined(BSLS_PLATFORM_OS_UNIX) \
    && !defined(BSLS_PLATFORM_OS_CYGWIN) \
    && !defined(BSLS_PLATFORM_OS_DARWIN)
        // 1 is not a valid pid on Windows or Cygwin.  On Darwin it is, but
        // registerPid only works for processes running under the same user as
        // this test is run as, meaning this would only work if the test was
        // run as 'root'.
        //
        // A better test might be to search for a valid pid (and for Darwin,
        // one which is running under the same user as this test).

        ASSERT(0 == perfmon.registerPid(1, "mytask2"));
        ASSERT(2 == perfmon.numRegisteredPids());

        ASSERT(0 == perfmon.unregisterPid(1));
        ASSERT(1 == perfmon.numRegisteredPids());
#endif

        ASSERT(0 == perfmon.unregisterPid(0));
        ASSERT(0 == perfmon.numRegisteredPids());
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Breathing Test"
                 << endl
                 << "==============="
                 << endl;
        }

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
                        const balb::PerformanceMonitor::Statistics& stats = *it;

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
                    bdlmtt::ThreadUtil::microSleep(0, 1);
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
        // Tests:
        //   numRegisteredPids()
        // --------------------------------------------------------------------

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
            INITIALIZE_LOGGER();

            balb::PerformanceMonitor perfmon(&ta);

            int                     bufferSize        = 0;

            double                  virtualSize       = 0;
            double                  residentSize      = 0;

            bsls::Types::Int64      currentBytesInUse = 0;
            bsls::Types::Int64      peakBytesInUse    = 0;

            balb::PerformanceMonitor::Measure virtualSizeMeasure =
                                  balb::PerformanceMonitor::BAEA_VIRTUAL_SIZE;

            balb::PerformanceMonitor::Measure residentSizeMeasure =
                                  balb::PerformanceMonitor::BAEA_RESIDENT_SIZE;

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
