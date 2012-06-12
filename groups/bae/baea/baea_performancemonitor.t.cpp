// baea_performancemonitor.t.cpp                                      -*-C++-*-

#include <baea_performancemonitor.h>

#include <bael_defaultobserver.h>
#include <bael_log.h>
#include <bael_loggermanager.h>
#include <bael_loggermanagerconfiguration.h>
#include <bael_severity.h>

#include <bdetu_systemtime.h>

#include <bcema_testallocator.h>
#include <bcemt_thread.h>

#include <bslma_newdeleteallocator.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_deque.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_stdexcept.h>

#if defined(BSLS_PLATFORM__OS_UNIX)
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
// [ 1]
//-----------------------------------------------------------------------------
// [ 1]
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
    bael_DefaultObserver               observer(&bsl::cout); \
    bael_LoggerManagerConfiguration    configuration; \
    bael_LoggerManager::initSingleton(\
            &observer, \
            configuration, \
            &bslma_NewDeleteAllocator::singleton());\
\
    bael_Severity::Level passthrough = bael_Severity::BAEL_OFF;\
\
    if (verbose) passthrough         = bael_Severity::BAEL_WARN;\
    if (veryVerbose) passthrough     = bael_Severity::BAEL_INFO;\
    if (veryVeryVerbose) passthrough = bael_Severity::BAEL_TRACE;\
\
    bael_LoggerManager::singleton().setDefaultThresholdLevels(\
                                       bael_Severity::BAEL_OFF,\
                                       passthrough,\
                                       bael_Severity::BAEL_OFF,\
                                       bael_Severity::BAEL_OFF)

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

#ifndef BSLS_PLATFORM__OS_WINDOWS
namespace test {

class MmapAllocator : public bslma_Allocator {

    typedef bsl::map<void*, std::size_t> MapType;

    MapType          d_map;
    bslma_Allocator *d_allocator_p;

  public:
    MmapAllocator(bslma_Allocator *basicAllocator = 0)
    : d_map(basicAllocator)
    , d_allocator_p(bslma_Default::allocator(basicAllocator))
    {
    }

    virtual void *allocate(size_type size)
    {
#if defined(BSLS_PLATFORM__OS_SOLARIS)
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

#elif defined(BSLS_PLATFORM__OS_AIX)

        void *result = mmap(0,
                            size,
                            PROT_READ | PROT_WRITE,
                            MAP_ANON | MAP_PRIVATE,
                            -1,
                            0);

        BSLS_ASSERT(reinterpret_cast<void*>(-1) != result);

        d_map.insert(bsl::make_pair(result, size));

        return result;

#elif defined(BSLS_PLATFORM__OS_HPUX)

        void *result = mmap(0,
                            size,
                            PROT_READ | PROT_WRITE,
                            MAP_ANONYMOUS | MAP_PRIVATE,
                            -1,
                            0);

        BSLS_ASSERT(reinterpret_cast<void*>(-1) != result);

        d_map.insert(bsl::make_pair(result, size));

        return result;

#elif defined(BSLS_PLATFORM__OS_LINUX) || defined(BSLS_PLATFORM__OS_DARWIN)
        return d_allocator_p->allocate(size);
#else
#error Not implemented.
#endif
    }

    virtual void deallocate(void *address)
    {
#if defined(BSLS_PLATFORM__OS_LINUX) || defined(BSLS_PLATFORM__OS_DARWIN)
        d_allocator_p->deallocate(address);
#else
        MapType::iterator it = d_map.find(address);
        BSLS_ASSERT(it != d_map.end());

        std::size_t size = it->second;

        int rc = munmap(static_cast<char*>(address), size);
        BSLS_ASSERT(-1 != rc);
#endif
    }
};

}  // close namespace test

void report(int               bufferSize,
            bsls_Types::Int64 currentBytesInUse,
            bsls_Types::Int64 peakBytesInUse,
            double            virtualSize,
            double            residentSize)
{
    std::cout << "BufferSize        = "
              << std::setprecision(8)
              << bufferSize
              << " ("
              << std::setprecision(8)
              << (((double)(bufferSize)) / (1024 * 1024))
              << " MB)"
              << std::endl;

    std::cout << "CurrentBytesInUse = "
              << std::setprecision(8)
              << currentBytesInUse
              << " ("
              << std::setprecision(8)
              << (((double)(currentBytesInUse)) / (1024 * 1024))
              << " MB)"
              << std::endl;

    std::cout << "PeakBytesInUse   = "
              << std::setprecision(8)
              << peakBytesInUse
              << " ("
              << std::setprecision(8)
              << (((double)(peakBytesInUse)) / (1024 * 1024))
              << " MB)"
              << std::endl;

    std::cout << "VirtualSize      = "
              << std::setprecision(8)
              << virtualSize * 1024 * 1024
              << " ("
              << std::setprecision(8)
              << (((double)(virtualSize)))
              << " MB)"
              << std::endl;

    std::cout << "ResidentSize     = "
              << std::setprecision(8)
              << residentSize * 1024 * 1024
              << " ("
              << std::setprecision(8)
              << (((double)(residentSize)))
              << " MB)"
              << std::endl;
}
#endif

double wasteCpuTime()
    // Just take up a measurable amount of cpu time.  Try 100 clock ticks (1.0
    // seconds or less).
{
#ifdef BSLS_PLATFORM__OS_UNIX
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
    bcemt_ThreadUtil::microSleep(0, 1);
    return 1.0;
#endif
}

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    bdet_TimeInterval startTime = bdetu_SystemTime::now();

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // TESTING PROCESS START TIME SANITY
        //
        // Concerns:
        //   it appears that for some versions of Linux, either different
        //   kernels or different distros, our estimation of process start
        //   time will be wildly inaccurate.  Determine whether this is the
        //   case.
        // --------------------------------------------------------------------

        if (verbose) cout << "PROCESS START TIME SANITY\n"
                             "=========================\n";

        bslma_TestAllocator ta(veryVeryVeryVerbose);

        baea_PerformanceMonitor perfmon(&ta);

        int rc = perfmon.registerPid(0, "perfmon");
        ASSERT(0 == rc);

        bdet_Datetime st = perfmon.begin()->startupTime();// process start time

        bdet_Datetime nowDt(1970, 1, 1);
        nowDt.addSeconds(bdetu_SystemTime::now().totalSecondsAsDouble());

        bdet_DatetimeInterval diff = nowDt - st;

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

        bcema_TestAllocator ta(veryVeryVeryVerbose);
        {
            // Instantiate a scheduler used by the performance monitor to
            // schedule collection events.
            bcep_TimerEventScheduler scheduler;
            scheduler.start();

            // Create the performance monitor, monitoring the current process
            // and auto-collecting statistics every second.
            baea_PerformanceMonitor perfmon(&scheduler, 1.0, &ta);

            if (0 != perfmon.registerPid(0, "mytask")) {
                ASSERT(!"Failed to register process with performance monitor");
                return testStatus;
            }

            // Print a formatted report of the performance statistics every 10
            // seconds for one minute.  Note that the report interval may vary
            // from the collection interval.
            for (int i = 0; i < 6; ++i) {
                bcemt_ThreadUtil::microSleep(0, 1);
                for (baea_PerformanceMonitor::ConstIterator
                                                         it  = perfmon.begin();
                                                         it != perfmon.end();
                                                       ++it)
                {
                    const baea_PerformanceMonitor::Statistics& stats = *it;

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
        bcep_TimerEventScheduler scheduler;
        baea_PerformanceMonitor perfmon(&scheduler, 1.0);

        ASSERT(0 == perfmon.numRegisteredPids());

        ASSERT(0 == perfmon.registerPid(0, "mytask"));
        ASSERT(1 == perfmon.numRegisteredPids());

        ASSERT(0 == perfmon.registerPid(1, "mytask2"));
        ASSERT(2 == perfmon.numRegisteredPids());

        ASSERT(0 == perfmon.unregisterPid(1));
        ASSERT(1 == perfmon.numRegisteredPids());

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

        bcema_TestAllocator ta(veryVeryVeryVerbose);
        {
            {
                baea_PerformanceMonitor perfmon(&ta);
                int                     rc;

                rc = perfmon.registerPid(0, "mytask");
                ASSERT(0 == rc);

                for (int i = 0; i < 6; ++i) {
                    wasteCpuTime();

                    perfmon.collect();

                    for (baea_PerformanceMonitor::ConstIterator
                                                        it  = perfmon.begin();
                                                        it != perfmon.end();
                                                      ++it)
                    {
                        const baea_PerformanceMonitor::Statistics& stats = *it;

                        if (veryVerbose) {
                            bsl::cout << "Pid = " << stats.pid() << ":\n";
                            stats.print(bsl::cout);
                        }
                    }
                }
            }

            {
                bcep_TimerEventScheduler scheduler;
                scheduler.start();

                baea_PerformanceMonitor perfmon(&scheduler, 1.0, &ta);
                int                     rc;

                rc = perfmon.registerPid(0, "mytask");
                ASSERT(0 == rc);

                for (int i = 0; i < 6; ++i) {
                    bcemt_ThreadUtil::microSleep(0, 1);
                    for (baea_PerformanceMonitor::ConstIterator
                                                        it  = perfmon.begin();
                                                        it != perfmon.end();
                                                      ++it)
                    {
                        const baea_PerformanceMonitor::Statistics& stats = *it;

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
#ifndef BSLS_PLATFORM__OS_WINDOWS
      case -1:
      case -2: {
        // --------------------------------------------------------------------
        // TESTING VIRTUAL SIZE AND RESIDENT SIZE
        //
        // Tests:
        //   numRegisteredPids()
        // --------------------------------------------------------------------

        bslma_NewDeleteAllocator newDeleteAllocator;
        test::MmapAllocator      mmapAllocator;

        bslma_Allocator *allocator;
        switch (test) {
        case -1: allocator = &newDeleteAllocator; break;
        case -2: allocator = &mmapAllocator;      break;
        }

        bcema_TestAllocator ta(veryVeryVeryVerbose, allocator);

        bslma_Default::setDefaultAllocator(&ta);
        bslma_Default::setGlobalAllocator(&ta);

        {
            INITIALIZE_LOGGER();

            baea_PerformanceMonitor perfmon(&ta);

            int                     bufferSize        = 0;

            double                  virtualSize       = 0;
            double                  residentSize      = 0;

            bsls_Types::Int64       currentBytesInUse = 0;
            bsls_Types::Int64       peakBytesInUse    = 0;

            baea_PerformanceMonitor::Measure virtualSizeMeasure =
                                  baea_PerformanceMonitor::BAEA_VIRTUAL_SIZE;

            baea_PerformanceMonitor::Measure residentSizeMeasure =
                                  baea_PerformanceMonitor::BAEA_RESIDENT_SIZE;

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

                    std::cout << "--"
                              << std::endl;

                    report(bufferSize,
                           currentBytesInUse,
                           peakBytesInUse,
                           virtualSize,
                           residentSize);
                }

                std::size_t halfBufferSize = bufferSize / 2;

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

                    std::cout << "**"
                              << std::endl;

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

            std::cout << "##"
                      << std::endl;

            report(0,
                   currentBytesInUse,
                   peakBytesInUse,
                   virtualSize,
                   residentSize);
        }
        ASSERT(0  < ta.numAllocation());
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
