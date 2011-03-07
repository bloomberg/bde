// baea_performancemonitor.t.cpp -*-C++-*-

#include <baea_performancemonitor.h>

#include <bael_defaultobserver.h>
#include <bael_log.h>
#include <bael_loggermanager.h>
#include <bael_loggermanagerconfiguration.h>
#include <bael_severity.h>

#include <bslma_newdeleteallocator.h>

#include <bcema_testallocator.h>
#include <bcemt_thread.h>

#include <bsl_cstdlib.h>
#include <bsl_deque.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_stdexcept.h>

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

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);
    int verbosity = 1 + verbose + veryVerbose
                  + veryVeryVerbose + veryVeryVeryVerbose;

    switch (test) {
      case 0: // Zero is always the leading case.
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
                    bcemt_ThreadUtil::microSleep(0, 1);

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
      case -1: {
        // --------------------------------------------------------------------
        // TESTING RESIDENT SIZE
        //
        // Tests:
        //   numRegisteredPids()
        // --------------------------------------------------------------------

        bcema_TestAllocator ta(veryVeryVeryVerbose);

        bslma_Default::setDefaultAllocator(&ta);
        bslma_Default::setGlobalAllocator(&ta);

        {
            INITIALIZE_LOGGER();
            
            baea_PerformanceMonitor  perfmon(&ta);
            int                      rc;
            double                   residentSize;
            bsls_PlatformUtil::Int64 numBytesInUse;
            bsl::deque<char>         buffer(&ta);
            int                      bufferSize;

            baea_PerformanceMonitor::Measure residentSizeMeasure =
                                  baea_PerformanceMonitor::BAEA_RESIDENT_SIZE;

            perfmon.registerPid(0, "test");

            bufferSize = 1024;

            for (int i = 0; i < 21; ++i) {
                buffer.resize(bufferSize);

                perfmon.collect();

                residentSize = 
                    perfmon.begin()->latestValue(residentSizeMeasure);

                std::cout << "--" 
                          << std::endl;

                std::cout << "BufferSize    = "
                          << std::setprecision(8)
                          << bufferSize
                          << " (" 
                          << std::setprecision(8)
                          << (((double)(bufferSize)) / (1024 * 1024)) 
                          << " MB)"
                          << std::endl; 

                std::cout << "NumBytesInUse = " 
                          << std::setprecision(8)
                          << ta.numBytesInUse() 
                          << " (" 
                          << std::setprecision(8)
                          << (((double)(ta.numBytesInUse())) / (1024 * 1024)) 
                          << " MB)"
                          << std::endl;

                std::cout << "ResidentSize  = " 
                          << std::setprecision(8) 
                          << residentSize * 1024 * 1024
                          << " (" 
                          << std::setprecision(8)
                          << (((double)(residentSize))) 
                          << " MB)"
                          << std::endl;

                bufferSize *= 2;
            }
        }
        ASSERT(0  < ta.numAllocation());
        ASSERT(0 == ta.numBytesInUse());

      }  break;
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
