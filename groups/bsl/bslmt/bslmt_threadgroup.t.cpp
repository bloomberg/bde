// bslmt_threadgroup.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_threadgroup.h>

#include <bslmt_semaphore.h>
#include <bslmt_lockguard.h>

#include <bslim_testutil.h>

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
// [ 1] bslmt::ThreadGroup(bslma::Allocator *basicAllocator = 0);
// [ 5] ~bslmt::ThreadGroup();
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
    bslmt::Mutex  d_mutex;
    bslmt::Mutex *d_startBarrier;

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
    explicit ThreadChecker(bslmt::Mutex* startBarrier)
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

///Usage
///-----
// The following usage example illustrates how 'bslmt::ThreadGroup' might be
// used in a typical test driver to simplify the execution of a common function
// in multiple threads.  Suppose that we are interested in creating a
// stress-test for the 'bslmt::Mutex' class.  The test is controlled by two
// parameters: the number of executions (defined by subsequent calls to 'lock'
// and 'unlock', and the amount of contention, defined by the number of threads
// accessing the mutex.  The test can be expressed as two functions.  The first
// is executed in each thread via a functor object:
//..
    class MutexTestJob {
        int           d_numIterations;
        int          *d_value_p;
        bslmt::Mutex *d_mutex_p;

      public:
        MutexTestJob(int numIterations, int *value, bslmt::Mutex *mutex)
        : d_numIterations(numIterations)
        , d_value_p(value)
        , d_mutex_p(mutex)
        {}

        void operator()() {
            for (int i = 0; i < d_numIterations; ++i) {
                bslmt::LockGuard<bslmt::Mutex> guard(d_mutex_p);
                ++*d_value_p;
            }
        }
    };
//..

class MutexTestSyncJob : private MutexTestJob {
    bslmt::Semaphore *d_startSemaphore_p;

public:
    MutexTestSyncJob(int              *value,
                     bslmt::Mutex     *mutex,
                     int               numIterations,
                     bslmt::Semaphore *startSemaphore)
    : MutexTestJob(numIterations, value, mutex)
    , d_startSemaphore_p(startSemaphore)
    {}

    void operator()() {
        d_startSemaphore_p->wait();
        MutexTestJob::operator()();
    }
};

class MutexTestDoubleSyncJob : private MutexTestSyncJob {
    bslmt::Semaphore *d_doneSync_p;

public:
    MutexTestDoubleSyncJob(int              *value,
                           bslmt::Mutex     *mutex,
                           int               numIterations,
                           bslmt::Semaphore *startSemaphore,
                           bslmt::Semaphore *doneSync)
    : MutexTestSyncJob(value, mutex, numIterations, startSemaphore)
    , d_doneSync_p(doneSync)
    {}

    void operator()() {
        MutexTestSyncJob::operator()();
        d_doneSync_p->post();
    }
};

class SynchronizedAddJob {
    bslmt::ThreadGroup *d_tg_p;
    MutexTestJob        d_job;
    bslmt::Mutex       *d_start_p;
    int                 d_numThreadsToAdd;

public:
    SynchronizedAddJob(bslmt::ThreadGroup* threadGroup,
                       const MutexTestJob& job,
                       bslmt::Mutex*       start,
                       int                 numThreadsToAdd)
    : d_tg_p(threadGroup)
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

// The second executes the main body of the test:
//..
    bslma::TestAllocator ta;
    {
        const int NUM_ITERATIONS = 10000;
        const int NUM_THREADS    = 8;

        bslmt::Mutex   mutex;                     // object under test
        int            value = 0;

        MutexTestJob testJob(NUM_ITERATIONS, &value, &mutex);

        bslmt::ThreadGroup threadGroup(&ta);
        for (int i = 0; i < NUM_THREADS; ++i) {
            ASSERT(0 == threadGroup.addThread(testJob));
        }
        threadGroup.joinAll();
        ASSERT(NUM_ITERATIONS * NUM_THREADS == value);
    }
    ASSERT(0 <  ta.numAllocations());
    ASSERT(0 == ta.numBytesInUse());
//..
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

            bslmt::Mutex     mutex;
            bslmt::Semaphore startSemaphore;
            bslmt::Semaphore doneSemaphore;

            int value = 0;

            MutexTestDoubleSyncJob testFunc(&value, &mutex, NUM_ITERATIONS,
                                            &startSemaphore, &doneSemaphore);

            {
                bslmt::ThreadGroup threadGroup(&ta);

                // Threads in the first batch are added individually
                for (int i = 0; i < NUM_THREADS; ++i) {
                   ASSERT(0 == threadGroup.addThread(testFunc));
                }

                // Threads in each subsequent batch are added as a group.
                for (int i = 0; i < NUM_BATCHES - 1; ++i) {
                    ASSERT(NUM_THREADS == threadGroup.addThreads(testFunc,
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

            bslmt::Mutex mutex;
            bslmt::Semaphore startSemaphore;

            bslmt::ThreadGroup mX(&ta);

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
            bslmt::Mutex   mutex;
            bslmt::Mutex   startMutex;
            startMutex.lock();

            MutexTestJob testFunc(NUM_ITERATIONS, &value, &mutex);

            bslmt::ThreadGroup tg(&ta);
            bslmt::ThreadGroup addingGroup(&ta);
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
        //   Exercise the basic functionality of the 'bslmt::ThreadGroup'
        //   class.  We want to ensure that thread groups can be instantiated
        //   and destroyed.  We also want to exercise the primary manipulators
        //   and accessors.
        //
        // Plan:
        //   Create a 'bslmt::ThreadGroup', 'mX', and a
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
           bslmt::ThreadGroup        mX(&ta);
           const bslmt::ThreadGroup&  X = mX;

           bslmt::Mutex startBarrier;

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
