// bdlqq_threadgroup.t.cpp                                            -*-C++-*-

#include <bdlqq_threadgroup.h>
#include <bdlqq_semaphore.h>
#include <bdlqq_lockguard.h>

#include <bslma_testallocator.h>
#include <bsls_assert.h>

#include <bsl_iostream.h>
#include <bsl_streambuf.h>

#include <bsl_cstdlib.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
//
//-----------------------------------------------------------------------------
// CREATORS
// [ 1] bdlqq::ThreadGroup(bslma::Allocator *basicAllocator = 0);
// [ 5] ~bdlqq::ThreadGroup();
//
// MANIPULATORS
// [ 2] int addThread(functor);
// [ 2] int addThread(functor, attributes);
// [ 2] int addThreads(functor, numThreads);
// [ 2] int addThreads(functor, numThreads, attributes);
// [ 1] void joinAll();
//
// ACCESSORS
// [ 1] int numThreads() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] CONCERN: MULTI-THREADING
// [ 3] CONCERN: FUNCTOR LIFETIME
// [ 4] CONCERN: DESTRUCTOR DOES NOT BLOCK
// [ 5] USAGE EXAMPLE
//-----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
static int testStatus = 0;

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

// ============================================================================
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number.
#define T_()  cout << '\t' << flush;          // Print tab w/o newline.

// ============================================================================
//            GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
// ----------------------------------------------------------------------------
static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

namespace {

class ThreadChecker {

    // DATA
    int           d_numCalls;
    bdlqq::Mutex  d_mutex;
    bdlqq::Mutex *d_startBarrier;

  public:
    // TYPES
    class ThreadCheckerFunctor {
        ThreadChecker *d_checker_p;

    public:
        ThreadCheckerFunctor(ThreadChecker *checker)
        : d_checker_p(checker)
        {}

        // MANIPULATORS
        void operator()() {
            d_checker_p->eval();
        }
    };

    // CREATORS
    explicit ThreadChecker(bdlqq::Mutex* startBarrier)
       : d_numCalls(0),
         d_startBarrier(startBarrier)
    {}

    // MANIPULATORS
    void eval() {
       d_startBarrier->lock();
       d_startBarrier->unlock();
       d_mutex.lock();
       ++d_numCalls;
       d_mutex.unlock();
    }

    ThreadCheckerFunctor getFunc() {
        return ThreadCheckerFunctor(this);
    }

    // ACCESSORS
    int numCalls() const
    {
        return d_numCalls;
    }
};

class MutexTestJob {
   int           d_numIterations;
   int          *d_value_p;
   bdlqq::Mutex *d_mutex_p;

 public:
   MutexTestJob(int numIterations, int *value, bdlqq::Mutex *mutex)
   : d_numIterations(numIterations)
   , d_value_p(value)
   , d_mutex_p(mutex)
   {}

   void operator()() {
      for (int i = 0; i < d_numIterations; ++i) {
         bdlqq::LockGuard<bdlqq::Mutex> guard(d_mutex_p);
         ++*d_value_p;
      }
   }
};

class MutexTestSyncJob : private MutexTestJob {
    bdlqq::Semaphore *d_startSemaphore_p;

public:
    MutexTestSyncJob(int              *value,
                     bdlqq::Mutex     *mutex,
                     int               numIterations,
                     bdlqq::Semaphore *startSemaphore)
    : MutexTestJob(numIterations, value, mutex)
    , d_startSemaphore_p(startSemaphore)
    {}

    void operator()() {
        d_startSemaphore_p->wait();
        MutexTestJob::operator()();
    }
};

class MutexTestDoubleSyncJob : private MutexTestSyncJob {
    bdlqq::Semaphore *d_doneSync_p;

public:
    MutexTestDoubleSyncJob(int              *value,
                           bdlqq::Mutex     *mutex,
                           int               numIterations,
                           bdlqq::Semaphore *startSemaphore,
                           bdlqq::Semaphore *doneSync)
    : MutexTestSyncJob(value, mutex, numIterations, startSemaphore) 
    , d_doneSync_p(doneSync)
    {}

    void operator()() {
        MutexTestSyncJob::operator()();
        d_doneSync_p->post();
    }
};

class SynchronizedAddJob {
    bdlqq::ThreadGroup *d_tg_p;
    MutexTestJob        d_job;
    bdlqq::Mutex       *d_start_p;
    int                 d_numThreadsToAdd;

public:
    SynchronizedAddJob(bdlqq::ThreadGroup* tg,
                       const MutexTestJob& job,
                       bdlqq::Mutex* start,
                       int numThreadsToAdd)
    : d_tg_p(tg)
    , d_job(job)
    , d_start_p(start)
    , d_numThreadsToAdd(numThreadsToAdd) {
        BSLS_ASSERT(3 >= numThreadsToAdd);
    }

    void operator()() {
        d_start_p->lock();
        d_start_p->unlock();
        d_tg_p->addThread(d_job);
        d_tg_p->addThreads(d_job, d_numThreadsToAdd - 1);
    }
};

}  // close unnamed namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = (argc > 1) ? bsl::atoi(argv[1]) : 1;

    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and execute as shown.
        //
        // Plan:
        //   Incorporate the usage example from the header file into the test
        //   driver.  Make use of existing test apparatus by instantiating
        //   objects with a 'bslma::TestAllocator' object where applicable.
        //   Additionally, replace all calls to 'assert' in the usage example
        //   with calls to 'ASSERT'.  This now becomes the source, which is
        //   then "copied" back to the header file by reversing the above
        //   process.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Usage Example" << endl
                 << "=====================" << endl;
        }

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        {
            const int NUM_ITERATIONS = 10000;
            const int NUM_THREADS    = 8;

            bdlqq::Mutex   mutex;                     // object under test
            int            value = 0;
            
            MutexTestJob testJob(NUM_ITERATIONS, &value, &mutex);
            
            bdlqq::ThreadGroup tg(&ta);
            for (int i = 0; i < NUM_THREADS; ++i) {
                ASSERT(0 == tg.addThread(testJob));
            }
            tg.joinAll();
            ASSERT(NUM_ITERATIONS * NUM_THREADS == value);
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 4: {
        // --------------------------------------------------------------------
        // CONCERN: DESTRUCTOR DOES NOT BLOCK
        //
        // Concerns:
        //  That the destructor correctly abandons the threads.
        //
        // Plan:
        //  Create several threads in a group, synchronized so they do
        //  not start executing.  Allow the group to go out of scope, then
        //  release the threads.  Verify that they all execute.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Concern: Destructor Does Not Block" << endl
                 << "==================================" << endl;
        }

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        {
            const int NUM_ITERATIONS = 1000;
            const int NUM_THREADS    = 8;
            const int NUM_BATCHES    = 3;   // assert(NUM_BATCHES >= 3)

            bdlqq::Mutex     mutex;
            bdlqq::Semaphore startSemaphore;
            bdlqq::Semaphore doneSemaphore;

            int value = 0;

            MutexTestDoubleSyncJob testFunc(&value, &mutex, NUM_ITERATIONS,
                                            &startSemaphore, &doneSemaphore);
            
            {
                bdlqq::ThreadGroup tg(&ta);

                // Threads in the first batch are added individually
                for (int i = 0; i < NUM_THREADS; ++i) {
                   ASSERT(0 == tg.addThread(testFunc));
                }

                // Threads in each subsequent batch are added as a group.
                for (int i = 0; i < NUM_BATCHES - 1; ++i) {
                    ASSERT(NUM_THREADS == tg.addThreads(testFunc,
                                                        NUM_THREADS));
                }
            }
            ASSERT(0 == value);
            startSemaphore.post(NUM_THREADS * NUM_BATCHES);

            for (int i = 0; i < NUM_THREADS * NUM_BATCHES; ++i) {
                doneSemaphore.wait();
            }

            ASSERT(NUM_ITERATIONS * NUM_THREADS * NUM_BATCHES == value);
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // CONCERN: FUNCTOR LIFETIME
        //
        // Concerns:
        //  That the functor passed to both the 'addThread' and 'addThreads'
        //  methods may safely go out of scope before the thread begins
        //  executing.
        //
        // Plan:
        //  Create a thread by passing a functor object to the 'addThread'
        //  method.  Ensure that the functor goes out of scope before the
        //  thread begins executing.  Check that the thread executes properly.
        //  Repeat, substituting the 'addThreads' method as the means of adding
        //  a thread to the thread group.
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Concern: Functor Lifetime" << endl
                 << "=========================" << endl;
        }

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        {
            const int NUM_ITERATIONS = 100;
            const int NUM_THREADS    = 3;

            bdlqq::Mutex mutex;
            bdlqq::Semaphore startSemaphore;

            bdlqq::ThreadGroup mX(&ta);

            int value = 0;

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Testing 'addThread'
            {
                MutexTestSyncJob testFunc(&value, &mutex, NUM_ITERATIONS,
                                          &startSemaphore);
                ASSERT(0 == mX.addThread(testFunc));
            }

            ASSERT(0 == value);
            startSemaphore.post();
            mX.joinAll();
            ASSERT(NUM_ITERATIONS == value);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Testing 'addThreads'

            value = 0;

            {
                MutexTestSyncJob testFunc(&value, &mutex, NUM_ITERATIONS,
                                          &startSemaphore);
                ASSERT(NUM_THREADS == mX.addThreads(testFunc, NUM_THREADS));
            }

            ASSERT(0 == value);
            startSemaphore.post(NUM_THREADS);
            mX.joinAll();
            ASSERT(NUM_ITERATIONS * NUM_THREADS == value);
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // CONCERN: MULTI-THREADING
        //
        // Concerns:
        //   That multiple threads may simultaneously add threads to the group.
        //
        // Plan:
        //   Establish a group of threads which add threads to another group.
        //   Synchronize the adding group so the threads start together.
        //   Ensure that the processing group is correctly set up and executes.
        //
        // Testing:
        //   int addThread(functor);
        //   int addThreads(functor, numThreads);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Concern: Multithreading" << endl
                 << "========================" << endl;
        }

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        {
            const int NUM_ITERATIONS               = 10000;
#ifdef BSLS_PLATFORM_OS_LINUX
            const int NUM_THREADS_ADDED_PER_THREAD = 2;
#else
            const int NUM_THREADS_ADDED_PER_THREAD = 3;
#endif
            const int NUM_ADDING_THREADS           = 16;

            int value = 0;
            bdlqq::Mutex   mutex;
            bdlqq::Mutex   startMutex;
            startMutex.lock();

            MutexTestJob testFunc(NUM_ITERATIONS, &value, &mutex);

            bdlqq::ThreadGroup tg(&ta);
            bdlqq::ThreadGroup addingGroup(&ta);
            for (int i = 0; i < NUM_ADDING_THREADS; ++i) {
                LOOP_ASSERT(i, 0 == addingGroup.addThread(SynchronizedAddJob(
                                            &tg,
                                            testFunc,
                                            &startMutex,
                                            NUM_THREADS_ADDED_PER_THREAD)));
            }
            startMutex.unlock();
            addingGroup.joinAll();
            tg.joinAll();
            ASSERT(NUM_ITERATIONS
                 * NUM_THREADS_ADDED_PER_THREAD
                 * NUM_ADDING_THREADS
                == value);
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Exercise the basic functionality of the 'bdlqq::ThreadGroup'
        //   class.  We want to ensure that thread groups can be instantiated
        //   and destroyed.  We also want to exercise the primary manipulators
        //   and accessors.
        //
        // Plan:
        //   Create a 'bdlqq::ThreadGroup', 'mX', and a
        //   non-modifiable reference to 'mX' named 'X'.  Add multiple threads
        //   by calling 'addThread' on 'mX', and verify the number of threads
        //   by calling 'numThreads' on 'X'.  Call 'joinAll' on 'mX', and
        //   verify the number of remaining threads on 'X'.
        //
        // Testing:
        //   Exercise basic functionality.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Breathing Test" << endl
                 << "==============" << endl;
        }

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        {
           bdlqq::ThreadGroup        mX(&ta);
           const bdlqq::ThreadGroup&  X = mX;

           bdlqq::Mutex startBarrier;

           // Perform two iterations to ensure the object behaves correctly
           // after 'joinAll' is called.
           for (int i = 0; i < 2; ++i) {
               ThreadChecker checker(&startBarrier);
               startBarrier.lock();

               ASSERT(0 == mX.numThreads());
               ASSERT(0 == mX.addThread(checker.getFunc()));
               ASSERT(0 == checker.numCalls());
               ASSERT(1 == mX.numThreads());
               ASSERT(0 == mX.addThread(checker.getFunc()));
               ASSERT(0 == mX.addThread(checker.getFunc()));
               ASSERT(0 == checker.numCalls());
               ASSERT(3 == mX.numThreads());
               ASSERT(3 == mX.addThreads(checker.getFunc(), 3));
               ASSERT(0 == checker.numCalls());
               ASSERT(6 == mX.numThreads());

               startBarrier.unlock();
               mX.joinAll();

               ASSERT(6 == checker.numCalls());
               ASSERT(0 == mX.numThreads());
           }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
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
