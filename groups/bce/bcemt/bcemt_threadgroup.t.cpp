// bcemt_threadgroup.t.cpp   -*-C++-*-

#include <bcemt_threadgroup.h>
#include <bcemt_semaphore.h>

#include <bdef_bind.h>

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
// [ 1] bcemt_ThreadGroup(bslma_Allocator *basicAllocator = 0);
// [ 5] ~bcemt_ThreadGroup();
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

//=============================================================================
//                        STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
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

class ThreadChecker {

    // DATA
    int          d_numCalls;
    bcemt_Mutex  d_mutex;
    bcemt_Mutex *d_startBarrier;

  public:
    // CREATORS
    explicit ThreadChecker(bcemt_Mutex* startBarrier)
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

    bdef_Function<void(*)()> getFunc() {
       return bdef_BindUtil::bind(&ThreadChecker::eval, this);
    }

    // ACCESSORS
    int numCalls() const
    {
        return d_numCalls;
    }
};

void testMutex(int         *value,
               bcemt_Mutex *mutex,
               int          numIterations)
{
    while (numIterations--) {
        mutex->lock();
        ++*value;
        mutex->unlock();
    }
}

void testMutexSync(int             *value,
                   bcemt_Mutex     *mutex,
                   int              numIterations,
                   bcemt_Semaphore *startSemaphore)
{
    startSemaphore->wait();
    testMutex(value, mutex, numIterations);
}

void testMutexDoubleSync(int             *value,
                         bcemt_Mutex     *mutex,
                         int              numIterations,
                         bcemt_Semaphore *startSemaphore,
                         bcemt_Semaphore *doneSync)
{
    startSemaphore->wait();
    testMutex(value, mutex, numIterations);
    doneSync->post();
}

void synchronizedAdd(bcemt_ThreadGroup* tg,
                     const bdef_Function<void (*)()>& function,
                     bcemt_Mutex* start,
                     int numThreadsToAdd)
{
    BSLS_ASSERT(3 >= numThreadsToAdd);

    start->lock();
    start->unlock();
    tg->addThread(function);
    tg->addThreads(function, numThreadsToAdd - 1);
}

}  // close unnamed namespace

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

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
        //   objects with a 'bcema_TestAllocator' object where applicable.
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

        bslma_TestAllocator ta(veryVeryVeryVerbose);
        {
            const int NUM_ITERATIONS = 10000;
            const int NUM_THREADS    = 8;

            bcemt_Mutex   mutex;
            int value = 0;

            bdef_Function<void(*)()> testFunc =
                bdef_BindUtil::bind(&testMutex,
                                    &value,
                                    &mutex,
                                    NUM_ITERATIONS);

            bcemt_ThreadGroup tg(&ta);
            for (int i = 0; i < NUM_THREADS; ++i) {
                ASSERT(0 == tg.addThread(testFunc));
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

        bslma_TestAllocator ta(veryVeryVeryVerbose);
        {
            const int NUM_ITERATIONS = 1000;
            const int NUM_THREADS    = 8;
            const int NUM_BATCHES    = 3;   // assert(NUM_BATCHES >= 3)

            bcemt_Mutex     mutex;
            bcemt_Semaphore startSemaphore;
            bcemt_Semaphore doneSemaphore;

            int value = 0;

            bdef_Function<void(*)()> testFunc =
                bdef_BindUtil::bind(&testMutexDoubleSync, &value, &mutex,
                                    NUM_ITERATIONS,
                                    &startSemaphore,
                                    &doneSemaphore);

            {
                bcemt_ThreadGroup tg(&ta);

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

        bslma_TestAllocator ta(veryVeryVeryVerbose);
        {
            const int NUM_ITERATIONS = 100;
            const int NUM_THREADS    = 3;

            bcemt_Mutex mutex;
            bcemt_Semaphore startSemaphore;

            bcemt_ThreadGroup mX(&ta);

            int value = 0;

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Testing 'addThread'
            {
                bdef_Function<void(*)()> testFunc =
                    bdef_BindUtil::bind(&testMutexSync, &value, &mutex,
                                        NUM_ITERATIONS, &startSemaphore);
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
                bdef_Function<void(*)()> testFunc =
                    bdef_BindUtil::bind(&testMutexSync, &value, &mutex,
                                        NUM_ITERATIONS, &startSemaphore);
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

        bslma_TestAllocator ta(veryVeryVeryVerbose);
        {
            const int NUM_ITERATIONS               = 10000;
            const int NUM_THREADS_ADDED_PER_THREAD = 3;
            const int NUM_ADDING_THREADS           = 16;

            int value = 0;
            bcemt_Mutex   mutex;
            bcemt_Mutex   startMutex;
            startMutex.lock();

            bdef_Function<void(*)()> testFunc =
                bdef_BindUtil::bind(&testMutex,
                                    &value,
                                    &mutex,
                                    NUM_ITERATIONS);

            bcemt_ThreadGroup tg(&ta);
            bcemt_ThreadGroup addingGroup(&ta);
            for (int i = 0; i < NUM_ADDING_THREADS; ++i) {
                LOOP_ASSERT(i, 0 == addingGroup.addThread(
                                        bdef_BindUtil::bind(
                                            synchronizedAdd,
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
        //   Exercise the basic functionality of the 'bcemt_ThreadGroup' class.
        //   We want to ensure that thread groups can be instantiated and
        //   destroyed.  We also want to exercise the primary manipulators and
        //   accessors.
        //
        // Plan:
        //   Create a 'bcemt_ThreadGroup', 'mX', and a
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

        bslma_TestAllocator ta(veryVeryVeryVerbose);
        {
           bcemt_ThreadGroup        mX(&ta);
           const bcemt_ThreadGroup&  X = mX;

           bcemt_Mutex startBarrier;

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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
