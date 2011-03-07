// baecs_mappingmanager.t.cpp -*-C++-*-

// The _CRT_RAND_S macro MUST be defined on Windows *PRIOR* to inclusion of
// stdlib.h in order to defined 'rand_s'.
#include <bsls_platform.h>
#ifdef BSLS_PLATFORM__OS_WINDOWS
// Remembering to define _CRT_RAND_S prior to inclusion statement
#define _CRT_RAND_S
#endif

#include <baecs_mappingmanager.h>
#include <bael_defaultobserver.h>
#include <bael_severity.h>
#include <bael_loggermanager.h>

#include <bcema_testallocator.h>
#include <bcemt_barrier.h>
#include <bcemt_thread.h>
#include <bcemt_threadgroup.h>

#include <bdesu_memoryutil.h>
#include <bdef_bind.h>
#include <bsls_platform.h>
#include <bsl_iostream.h>
#include <bsl_c_stdlib.h>
#include <bsl_cstdio.h>
#include <bsl_c_stdio.h>

#ifdef BSLS_PLATFORM__OS_WINDOWS
#include <windows.h>
#endif

#if defined(BSLS_PLATFORM__OS_AIX) || defined(BSLS_PLATFORM__OS_HPUX)
#define tmpnam_r tmpnam
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

inline
bsl::string tempFileName()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    char tmpPathBuf[MAX_PATH], tmpNameBuf[MAX_PATH];
    GetTempPath(MAX_PATH, tmpPathBuf);
    GetTempFileName(tmpPathBuf, "bde", 0, tmpNameBuf);
    return tmpNameBuf;
#else
    return tempnam(0, 0);
#endif
}

#ifdef BSLS_PLATFORM__OS_WINDOWS
#define rand_r   rand_s
#define tmpnam_r tmpnam_s
#endif

//=============================================================================
//                        STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
static int testStatus = 0;

typedef baecs_MappingManager Obj;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100)  ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\t" \
                    << #M << ": " << M << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

//=============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number.
#define T_()  cout << '\t' << flush;          // Print tab w/o newline.

//=============================================================================
//              GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------
static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

namespace {

class Case2Thread
{
   int            d_id;
   int            d_numThreads;
   int            d_pageSize;
   bcemt_Barrier *d_barrier;
   bcemt_Barrier *d_barrier2;
   Obj           *d_mappingManager;
   Obj::Handle   *d_handles;
   int            d_numPages;
   unsigned int   d_seed;
   int            d_numIterations;
   int            d_numCycles;

   int myRand(int n) {
      return max(0, (int)(n * (rand_r(&d_seed) / (double)(RAND_MAX + 1))));
   }

public:

   Case2Thread(int id, int numThreads, int pageSize, bcemt_Barrier *barrier,
               bcemt_Barrier *barrier2, Obj *mappingManager,
               Obj::Handle *handles, int numPages)
      : d_id(id), d_numThreads(numThreads), d_pageSize(pageSize),
        d_barrier(barrier), d_barrier2(barrier2),
        d_mappingManager(mappingManager),
        d_handles(handles), d_numPages(numPages)
   {
      d_seed = time(0);
      d_numIterations = pageSize / (sizeof(int) * 2 * d_numThreads);
      d_numCycles = (d_numIterations / 10) - 1;
   }

   int numCycles() {return d_numCycles;}
   int numIterations() {return d_numIterations;}

   void run()
   {
      int iterations = 0;
      for (int i = 0; i < d_numCycles; ++i) {
         int iterationsThisCycle = myRand
            (min(5, (d_numIterations - iterations - 5))) + 5;
         for (int j = 0; j < iterationsThisCycle; ++j) {
            for (int k = 0; k < d_numPages; ++k) {
               int *page = (int*)d_mappingManager->usePage(d_handles[k]);
               int pageIndex = iterations * d_numThreads + d_id;
               page[pageIndex] = d_id;
            }
            for (int k = 0; k < d_numPages; ++k) {
               d_mappingManager->releasePage(d_handles[k]);
            }
            ++iterations;
         }
         d_barrier->wait();
         d_barrier2->wait();
      }

      for (int i = iterations; i < d_numIterations; ++i) {
         for (int k = 0; k < d_numPages; ++k) {
            int *page = (int*)d_mappingManager->usePage(d_handles[k]);
            int pageIndex = i * d_numThreads + d_id;
            page[pageIndex] = d_id;
         }
         for (int k = 0; k < d_numPages; ++k) {
            d_mappingManager->releasePage(d_handles[k]);
         }
      }
   }

   bdef_Function<void(*)()> getFunctor()
   {
      return bdef_BindUtil::bind(&Case2Thread::run, this);
   }
};

}

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = (argc > 1) ? atoi(argv[1]) : 1;

    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    bael_DefaultObserver observer(bsl::cout);
    bael_LoggerManagerConfiguration configuration;
    if (veryVeryVerbose) {
       configuration.setDefaultThresholdLevelsIfValid(
                                                    bael_Severity::BAEL_TRACE,
                                                    bael_Severity::BAEL_TRACE,
                                                    bael_Severity::BAEL_TRACE,
                                                    bael_Severity::BAEL_TRACE);
    }

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
       case 2: {
          // ----------------------------
          // MULTITHREADED usePage() TEST
          //
          // Concern: That usePage() is safe under heavy contention
          //
          // Given a small number of pages, many threads will cycle through
          // all the pages, using and then releasing them.  They will
          // periodically pause so that the main thread can verify that all
          // pages are released.  During this test, the mapping limit must
          // be smaller than the total size of all pages so that the automatic
          // unmapping logic will be exercised.
          //
          // When the test is concluded, the backing file will be closed and
          // reopened.  The contents of the file will be checked to make sure
          // all the changes from all the threads are still present.
          //
          // ----------------------------
          if (verbose) {
             cout << "Multithreaded usePage() Test" << endl
                  << "============================" << endl;
          }

          enum { NUM_PAGES = 30, NUM_THREADS = 18  } ;

          bsl::string filename = tempFileName();
          const int PAGESIZE_BYTES = bdesu_MemoryUtil::pageSize();

          if (verbose) {
             P(NUM_PAGES); P(NUM_THREADS); P(PAGESIZE_BYTES); P(filename);
          }

          bdesu_FileUtil::FileDescriptor fd
              = bdesu_FileUtil::open(filename, 1, 0);
          ASSERT(bdesu_FileUtil::INVALID_FD != fd);

          // TEST INVARIANTS: grow the file beyond the necessary size
          ASSERT(-1 != bdesu_FileUtil::seek(fd, PAGESIZE_BYTES*NUM_PAGES*3,
                                            SEEK_SET));
          ASSERT(1 == bdesu_FileUtil::write(fd, (char*)"", 1));

          enum { MAPPING_LIMIT = (1 << 19), // 500 K
                 NUM_PRIORITY_LEVELS = 2 };
          int numIterations = 0;
          Case2Thread *threadData[NUM_THREADS];
          bcema_TestAllocator ta(veryVeryVeryVerbose);
          {
             Obj mappingManager (MAPPING_LIMIT, NUM_PRIORITY_LEVELS, &ta);
             baecs_MappingManager::Handle pageHandles[NUM_PAGES];

             if (verbose) {
                cout << "Adding and initializing pages..." << endl;
             }

             for (int i = 0; i < NUM_PAGES; ++i)
             {
                 pageHandles[i] = mappingManager.addPage(
                                                        fd,
                                                        PAGESIZE_BYTES*i,
                                                        PAGESIZE_BYTES,
                                                        true,
                                                        NUM_PRIORITY_LEVELS - 1
                                                        );
                char* page = mappingManager.usePage(pageHandles[i]);
                memset(page, 255, PAGESIZE_BYTES);
                mappingManager.releasePage(pageHandles[i]);
             }

             bcemt_ThreadGroup group;

             if (verbose) {
                cout << "Starting threads..." << endl;
             }

             bcemt_Barrier barrier1(NUM_THREADS+1), barrier2(NUM_THREADS+1);
             int numCycles = 0;
             for (int i = 0; i < NUM_THREADS; ++i) {
                threadData[i] = new Case2Thread(i, NUM_THREADS,
                                                PAGESIZE_BYTES, &barrier1,
                                                &barrier2, &mappingManager,
                                                pageHandles, NUM_PAGES);
                group.addThread(threadData[i]->getFunctor());
                if (0 == numCycles) {
                   numCycles = threadData[i]->numCycles();
                   numIterations = threadData[i]->numIterations();
                }
                else {
                   // These are *TEST INVARIANTS*...
                   ASSERT(numCycles == threadData[i]->numCycles());
                   ASSERT(numIterations == threadData[i]->numIterations());
                }
             }

             if (verbose) {
                cout << "Verifying thread safety..." << endl;
             }

             for (int i = 0; i < numCycles; ++i) {
                barrier1.wait();
                //Now all threads should be blocked, and all pages should
                //have been released.
                for (int j = 0; j < NUM_PAGES; ++j) {
                   ASSERT(0 == mappingManager.getPageUseCount(pageHandles[j]));
                }
                barrier2.wait();
             }
             if (verbose) {
                cout << "Waiting for threads to finish..." << endl;
             }
             group.joinAll();

             for (int i = 0; i < NUM_PAGES; ++i)
             {
                ASSERT(0 == mappingManager.getPageUseCount(pageHandles[i]));
                mappingManager.removePage(pageHandles[i]);
             }

          }
          ASSERT(0 < ta.numAllocations());
          ASSERT(0 == ta.numBytesInUse());
          ASSERT(0 == bdesu_FileUtil::close(fd));

          if (verbose) {
             cout << "Verifying data on disk..." << endl;
          }

          char* pageBuffer = new char[PAGESIZE_BYTES];
          fd = bdesu_FileUtil::open(filename, 0, 1);
          ASSERT(bdesu_FileUtil::INVALID_FD != fd);

          for (int i = 0; i < NUM_PAGES; ++i) {
             ASSERT(PAGESIZE_BYTES == bdesu_FileUtil::read(fd,
                                                           (void*)pageBuffer,
                                                           PAGESIZE_BYTES));
             // Each page should consist of numIterations copies of the
             // thread IDs.  e.g., if there were 3 threads, there should
             // be numIterations copies of 0, 1, 2, 0, 1, 2, etc...
             int* idBuffer = (int*)pageBuffer;
             int bufferIndex = 0;
             for (int j = 0; j < numIterations; ++j) {
                for (int k = 0; k < NUM_THREADS; ++k) {
                   LOOP3_ASSERT(i, j, k, k == idBuffer[bufferIndex]);

                   if (testStatus >= 100 ) {
                      goto verify_done; //to break out of 3 loops
                   }
                   if (veryVerbose && k != idBuffer[bufferIndex]) {
                      P(idBuffer[bufferIndex]);
                   }
                   bufferIndex++;

                }
             }
          }

          verify_done:
          if (verbose) {
             cout << "Cleaning up..." << endl;
          }

          delete [] pageBuffer;
          for (int i = 0; i < NUM_THREADS; ++i) {
             delete threadData[i];
          }

          ASSERT(0 == bdesu_FileUtil::close(fd));
          ASSERT(0 == bdesu_FileUtil::remove(filename));
          break;
       }
       case 1: {
          // -------------------------
          // USAGE EXAMPLE TEST
          // -------------------------
          if (verbose) {
             cout << "Usage Example Test" << endl
                  << "==================" << endl;
          }

          // Usage example precondition: a file descriptor "fd"
          bsl::string tmpname = tempFileName();
          bdesu_FileUtil::FileDescriptor fd =
              bdesu_FileUtil::open(tmpname, 1, 0);
          ASSERT(bdesu_FileUtil::INVALID_FD != fd);

          enum {
              MAPPING_LIMIT = (1 << 20) * 100 , // 100 Mb
              NUM_PRIORITY_LEVELS = 1
          };

          enum { NUM_PAGES = 100 };
          const int PAGESIZE_BYTES = bdesu_MemoryUtil::pageSize();

          if (verbose)
          {
              P(tmpname);
              P(PAGESIZE_BYTES);
          }
          bcema_TestAllocator ta(veryVeryVeryVerbose);
          {

              Obj mappingManager (MAPPING_LIMIT, NUM_PRIORITY_LEVELS, &ta);
              baecs_MappingManager::Handle pageHandles[NUM_PAGES];

              for (int i = 0; i < NUM_PAGES; ++i)
              {
                  pageHandles[i] = mappingManager.addPage(
                                                      fd,
                                                      PAGESIZE_BYTES*i,
                                                      PAGESIZE_BYTES,
                                                      true,
                                                      NUM_PRIORITY_LEVELS - 1);
              }

              ASSERT(0 == mappingManager.mapCountReset());

              for (int i = 0; i < NUM_PAGES; ++i)
              {
                  mappingManager.usePage(pageHandles[i]);
              }

              ASSERT(NUM_PAGES == mappingManager.mapCountReset());

              for (int i = 0; i < NUM_PAGES; ++i)
              {
                  mappingManager.releasePage(pageHandles[i]);
                  mappingManager.removePage(pageHandles[i]);
              }

          }

          ASSERT(0 < ta.numAllocations());
          ASSERT(0 == ta.numBytesInUse());

          // IMPORTANT: do not close the file descriptor until the object
          // has been destroyed
          bdesu_FileUtil::close(fd);
          bdesu_FileUtil::remove(tmpname);

       } break;
       default: {
           cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
           testStatus = -1;
       }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
