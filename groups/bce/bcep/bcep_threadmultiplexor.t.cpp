// bcep_threadmultiplexor.t.cpp   -*-C++-*-

#include <bcep_threadmultiplexor.h>

#include <bdef_bind.h>
#include <bslmf_if.h>
#include <bcemt_semaphore.h>
#include <bcemt_thread.h>
#include <bcemt_threadgroup.h>
#include <bcep_fixedthreadpool.h>
#include <bces_atomictypes.h>

#include <bdema_managedptr.h>
#include <bcema_testallocator.h>
#include <bsl_iostream.h>
#include <bsl_streambuf.h>

#include <bsl_c_math.h>

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
// TBD: Overview
//-----------------------------------------------------------------------------
// CREATORS
// [ 1] bcep_ThreadMultiplexor(int, int, bslma_Allocator*);
// [ 1] ~bcep_ThreadMultiplexor();
//
// MANIPULATORS
// [ 2] int processJob(const JOBTYPE&);
//
// ACCESSORS
// [ 1] int maxProcessors();
// [ 1] int numProcessors();

//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE
// [ 3] MORE PARTITIONS
// [ 4] SMALL QUEUE
// [ 5] STRESS TEST
// [ 6] CONCERN: Single-processor multiplexor
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

//=============================================================================
//                GLOBAL SUPPORT FUNCTIONS AND CLASSES FOR TESTING
//-----------------------------------------------------------------------------

                              // ===============
                              // class TestQueue
                              // ===============

class TestQueue {
    // This class appears in the usage example for the ThreadMultiplexor
    // component.  It associates a multiplexor with a thread pool, so that
    // threads in the thread pool execute all their jobs through the
    // multiplexor.

    // DATA
    bcep_FixedThreadPool   *d_threadPool_p;    // (held, not owned)
    bcep_ThreadMultiplexor  d_multiplexor;     // used to partition threads

    // NOT IMPLEMENTED
    TestQueue(const TestQueue&);
    TestQueue& operator=(const TestQueue&);

  public:
    // TYPES
    typedef bcep_ThreadMultiplexor::Job Job;

    // CREATORS
    TestQueue(int                   maxProcessors,
              int                   queueCapacity,
              bcep_FixedThreadPool *threadPool,
              bslma_Allocator      *basicAllocator = 0);

    ~TestQueue();

    // MANIPULATORS
    int processJob(const Job& job);
      // Submit a job to the thread pool.  If there are processors available
      // according to the multiplexor, the job will be executed immediately;
      // otherwise it will be enqueued.

    bcep_ThreadMultiplexor *multiplexor();
      // Return a pointer to the modifiable "multiplexor" attribute.
};

// CREATORS
TestQueue::TestQueue(int                   maxProcessors,
                     int                   queueCapacity,
                     bcep_FixedThreadPool *threadPool,
                     bslma_Allocator      *basicAllocator)
: d_threadPool_p(threadPool)
, d_multiplexor(maxProcessors,
                queueCapacity,
                basicAllocator)
{
}

TestQueue::~TestQueue()
{
}

// MANIPULATORS
int TestQueue::processJob(const TestQueue::Job& job)
{
    return d_threadPool_p->tryEnqueueJob(
            bdef_BindUtil::bind(&bcep_ThreadMultiplexor::processJob<Job>,
                                &d_multiplexor,
                                job));
}

bcep_ThreadMultiplexor *TestQueue::multiplexor() {
   return &d_multiplexor;
}

                           // ======================
                           // class UsageTestChecker
                           // ======================

class UsageTestChecker {
    // This class provides a functor to be passed to the ThreadMultiplexor
    // for testing.  The job the functor does is to increment a counter
    // controlled by a mutex.  The start of the job can be coordinated
    // with a semaphore, allowing for forced contention.
    //

    // DATA
    int                     d_timesCalled;
    int                     d_maxProcessors;
    bcemt_Mutex             d_mutex;
    bcemt_Semaphore        *d_semaphore;      // (held, not owned)
    bcep_ThreadMultiplexor *d_multiplexor;    // (held, not owned)

public:
    // CREATORS
   UsageTestChecker(bcep_ThreadMultiplexor *multiplexor);

   ~UsageTestChecker();

   // MANIPULATORS
   operator TestQueue::Job ();
     // Return an invokable functor which calls the 'eval' method of this
     // object.

   void eval();
     // Perform the function of this object.

   void reset();
     // Reset the counter variables of this object to 0.

   void setSemaphore(bcemt_Semaphore* semaphore);
     // Specify the semaphore which will control the start of the job.

   // ACCESSORS
   int timesCalled() const;
     // Return the number of times the functor was invoked.

   int maxProcessors() const;
     // Return the maximal value of the "processors" attribute of the
     // multiplexor when executing this functor.
};

// CREATORS
UsageTestChecker::UsageTestChecker(bcep_ThreadMultiplexor *multiplexor)
: d_timesCalled(0)
, d_maxProcessors(0)
, d_semaphore(0)
, d_multiplexor(multiplexor)
{}

UsageTestChecker::~UsageTestChecker()
{
}

// MANIPULATORS
UsageTestChecker::operator TestQueue::Job()
{
    return bdef_BindUtil::bind(&UsageTestChecker::eval, this);
}

void UsageTestChecker::eval() {
    if (d_semaphore) {
        d_semaphore->wait();
    }

    d_mutex.lock();
    ++d_timesCalled;
    d_maxProcessors = bsl::max(d_maxProcessors,
            d_multiplexor->numProcessors());
    d_mutex.unlock();
}

void UsageTestChecker::reset() {
    d_timesCalled = d_maxProcessors = 0;
}

void UsageTestChecker::setSemaphore(bcemt_Semaphore* semaphore) {
    d_semaphore = semaphore;
}

// ACCESSORS
int UsageTestChecker::timesCalled() const {
    return d_timesCalled;
}

int UsageTestChecker::maxProcessors() const {
    return d_maxProcessors;
}

//=============================================================================
//                     TEST CASE 6: SUPPORT FUNCTIONS
//-----------------------------------------------------------------------------
namespace TEST_CASE_6 {

void testCase6(bcemt_Semaphore                 *startSemaphore,
               bcep_ThreadMultiplexor          *mX,
               int                              numJobs,
               const bdef_Function<void(*)()>&  job)
{
   startSemaphore->wait();
   for (; 0 < numJobs; --numJobs) {
      mX->processJob(job);
   }
}

}  //close namespace TEST_CASE_6
//=============================================================================
//                           COMPONENT USAGE EXAMPLE
//-----------------------------------------------------------------------------
namespace TEST_CASE_USAGE_EXAMPLE {

class JobQueue {
   // This class defines a generic processor for user-defined functions
   // ("jobs").  Jobs specified to the 'processJob' method are executed
   // in the thread pool specified at construction.

public:
   // PUBLIC TYPES
   typedef bcep_ThreadMultiplexor::Job Job;
     // A callback of this type my be specified to the 'processJob'
     // method.

private:
   // DATA
   bcep_FixedThreadPool   *d_threadPool_p;    // (held, not owned)
   bcep_ThreadMultiplexor  d_multiplexor;     // used to partition threads

private:
   // NOT IMPLEMENTED
   JobQueue(const JobQueue&);
   JobQueue& operator=(const JobQueue&);

public:
   // CREATORS
   JobQueue(int                   maxProcessors,
            bcep_FixedThreadPool *threadPool,
            bslma_Allocator      *basicAllocator = 0);
     // Create a job queue that executes jobs in the specified
     // 'threadPool' using no more than the specified 'maxProcessors'.
     // Optionally specify a 'basicAllocator' used to supply memory.  If
     // 'basicAllocator is 0, the currently installed default allocator
     // is used.

   ~JobQueue();
     // Destroy this object.

   // MANIPULATORS
   int processJob(const Job& job);
     // Process the specified 'job' in the thread pool specified at
     // construction.  Return 0 on success, and a non-zero value
     // otherwise.

};

// CREATORS
JobQueue::JobQueue(int                   maxProcessors,
                   bcep_FixedThreadPool *threadPool,
                   bslma_Allocator      *basicAllocator)
: d_threadPool_p(threadPool)
, d_multiplexor (maxProcessors,
                 threadPool->queueCapacity(),
                 basicAllocator)
{
}

JobQueue::~JobQueue()
{
}

// MANIPULATORS
int JobQueue::processJob(const JobQueue::Job& job)
{
    return d_threadPool_p->tryEnqueueJob(
            bdef_BindUtil::bind(&bcep_ThreadMultiplexor::processJob<Job>,
                                &d_multiplexor,
                                job));
}

int usageExample(bslma_Allocator *allocator)
{
    enum {
        NUM_THREADS   = 5,   // total number of threads
        NUM_QUEUES    = 3,   // total number of JobQueue objects
        MAX_QUEUESIZE = 20  // total number of pending jobs
    };

    int maxProc = bsl::max(1.0,
            ceil(double(NUM_THREADS)/(NUM_QUEUES-1))-1);

    bcep_FixedThreadPool tp(NUM_THREADS, MAX_QUEUESIZE, allocator);
    JobQueue             importantQueue(maxProc, &tp);
    JobQueue             urgentQueue(maxProc, &tp);
    JobQueue             criticalQueue(maxProc, &tp);

    if (0 != tp.start()) {
       ASSERT(!"Could not start thread pool! Threads cannot be created!");
       return -1;
    }

    bces_AtomicInt iCheck=0, uCheck=0, cCheck=0;

    JobQueue::Job ijob =
        bdef_BindUtil::bind(&bces_AtomicInt::add, &iCheck, 1);

    JobQueue::Job ujob =
        bdef_BindUtil::bind(&bces_AtomicInt::add, &uCheck, 1);

    JobQueue::Job cjob =
        bdef_BindUtil::bind(&bces_AtomicInt::add, &cCheck, 1);

    importantQueue.processJob(ijob);
    importantQueue.processJob(ijob);
    importantQueue.processJob(ijob);
    importantQueue.processJob(ijob);
    importantQueue.processJob(ijob);
    importantQueue.processJob(ijob);

    urgentQueue.processJob(ujob);
    urgentQueue.processJob(ujob);
    urgentQueue.processJob(ujob);
    urgentQueue.processJob(ujob);

    criticalQueue.processJob(cjob);
    criticalQueue.processJob(cjob);

    tp.stop();

    ASSERT(6 == iCheck);
    ASSERT(4 == uCheck);
    ASSERT(2 == cCheck);
    return 0;
}

}  // close namespace TEST_CASE_USAGE_EXAMPLE
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
      case 6: {
        // --------------------------------------------------------------------
        // CONCERN: Single-processor multiplexor
        //
        // Concern:
        //  That a multiplexor shared between several threads and having
        //  a processor limit of 1 will process jobs correctly.
        //
        // Plan:
        //  Set up multiple threads sharing a single multiplexor.  Set up the
        //  multiplexor with a limit of 1 processor.  Ensure that a counter
        //  variable is hit the proper number of times.
        //
        // Testing:
        //   processJob();
        // --------------------------------------------------------------------

        if (verbose) cout << "Single-Processor Test" << endl
                          << "=====================" << endl;

        bcema_TestAllocator ta(veryVeryVeryVerbose);
        {
            using namespace TEST_CASE_6;
            enum {
                NUM_THREADS   = 7,
                MAX_QUEUESIZE = 1000,
                NUM_JOBS      = 100
            };

            bces_AtomicInt timesCalled = 0;
            bcemt_Semaphore startSemaphore;
            bcep_ThreadMultiplexor mX(1, MAX_QUEUESIZE, &ta);
            bcemt_ThreadGroup threads;

            bdef_Function<void(*)()> addFunc = bdef_BindUtil::bind(
                                                          &bces_AtomicInt::add,
                                                          &timesCalled,
                                                          1);

            for (int i = 0; i < NUM_THREADS; ++i) {
               LOOP_ASSERT(i, 0 == threads.addThread(
                                            bdef_BindUtil::bind(&testCase6,
                                                               &startSemaphore,
                                                               &mX,
                                                               (int)NUM_JOBS,
                                                               addFunc)));
            }

            startSemaphore.post(NUM_THREADS);
            threads.joinAll();
            ASSERT(NUM_THREADS * NUM_JOBS == timesCalled);
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // STRESS TEST
        //
        // Concerns:
        //  Test that the system can handle a large number of submitted
        //  jobs.  Also test that in such a situation, the multiplexor will
        //  at some point use the maximum number of "processors" and no more.
        //
        // Plan:
        //  Create multiplexors with large queues and then submit jobs until
        //  the queues are full.  Verify that all the jobs eventually execute,
        //  and verify that the maximum number of processors used is exactly
        //  what's specified to the multiplexor.
        //
        // Testing:
        //   processJob();
        // --------------------------------------------------------------------

        if (verbose) cout << "Large Queue Test" << endl
                          << "================" << endl;

        bcema_TestAllocator ta(veryVeryVeryVerbose);
        {
            enum {
                NUM_THREADS   = 7,       // total number of threads
                MAX_QUEUESIZE = 0x20000,
                NUM_QUEUES = 3,
                IQUEUE_MAX_PROC=2,
                UQUEUE_MAX_PROC=4,
                OQUEUE_MAX_PROC=1
            };

            bcep_FixedThreadPool tp(NUM_THREADS, MAX_QUEUESIZE, &ta);
            TestQueue             importantQueue
               (IQUEUE_MAX_PROC, MAX_QUEUESIZE, &tp, &ta);
            TestQueue             urgentQueue
               (UQUEUE_MAX_PROC, MAX_QUEUESIZE, &tp, &ta);
            TestQueue             otherQueue
               (OQUEUE_MAX_PROC, MAX_QUEUESIZE, &tp, &ta);

            if (0 != tp.start()) {
               ASSERT(!"Could not start thread pool!  "
                       "Threads cannot be created!");
               break;
            }
            if (veryVerbose) cout << "Thread-pool Started" << endl;

            UsageTestChecker iChecker(importantQueue.multiplexor());
            UsageTestChecker uChecker(urgentQueue.multiplexor());
            UsageTestChecker oChecker(otherQueue.multiplexor());
            int iJobs = 0, uJobs = 0, oJobs = 0;

            bcemt_Semaphore startSemaphore;
            iChecker.setSemaphore(&startSemaphore);
            uChecker.setSemaphore(&startSemaphore);
            oChecker.setSemaphore(&startSemaphore);

            if (veryVerbose) cout << " Adding jobs..." << endl;

            for (int rc = 0; 0 == rc; ) {
               rc = urgentQueue.processJob(uChecker);
               if (0 == rc) {
                  ++uJobs;
                  rc = importantQueue.processJob(iChecker);
                  if (0 == rc) {
                     ++iJobs;

                     rc = otherQueue.processJob(oChecker);
                     if (0 == rc) {
                        ++oJobs;
                     }
                  }
               }
            }

            if (veryVerbose) cout << " ...Done adding" << endl;

            startSemaphore.post(iJobs+uJobs+oJobs);

            tp.stop();
            if (veryVerbose) cout << "Thread-pool Stopped" << endl;

            ASSERT(iJobs == iChecker.timesCalled());
            ASSERT(uJobs == uChecker.timesCalled());
            ASSERT(oJobs == oChecker.timesCalled());
            if (verbose) {
                P_(iJobs); P(iChecker.timesCalled());
                P_(uJobs); P(uChecker.timesCalled());
                P_(oJobs); P(oChecker.timesCalled());
            }

            LOOP_ASSERT(iChecker.maxProcessors(),
                        IQUEUE_MAX_PROC == iChecker.maxProcessors());
            // With very low system resources it may be impossible to run
            // the dispatching thread frequently enough to get all 4 threads
            // in the UNIQUE group executing simultaneously.  This check
            // usually works but tends to fail during important release
            // builds.  Much more trouble than it's worth.
            //
            //LOOP_ASSERT(uChecker.maxProcessors(),
            //            UQUEUE_MAX_PROC == uChecker.maxProcessors());
            if (verbose && UQUEUE_MAX_PROC != uChecker.maxProcessors()) {
               cout << "MILD WARNING: uChecker.maxProcessors() == "
                    << uChecker.maxProcessors() << ", expected "
                    << UQUEUE_MAX_PROC << endl;
            }

            LOOP_ASSERT(oChecker.maxProcessors(),
                        OQUEUE_MAX_PROC == oChecker.maxProcessors());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CONCERN: Small number of threads test
        //
        // Concerns:
        //  Test the behavior of the component when the number of threads is
        //  smaller than the number of processors.
        //
        // Plan:
        //  Create a thread pool with 3 threads and a large queue.  Using
        //  a multiplexor configured for 10 processors, submit many jobs
        //  and confirm they all run.
        //
        // Testing:
        //   processJob();
        // --------------------------------------------------------------------

        if (verbose) cout << "FEW THREADS TEST" << endl
                          << "================" << endl;

        bcema_TestAllocator ta(veryVeryVeryVerbose);
        {
            enum {
                NUM_THREADS   = 3,   // total number of threads
                MAX_QUEUESIZE = 6000,// total number of pending jobs
                NUM_JOBS      = 2000,// number of jobs to submit
                MAX_PROC      = 10    // test condition
            };

            bcep_FixedThreadPool tp(NUM_THREADS, MAX_QUEUESIZE, &ta);

            if (0 != tp.start()) {
               ASSERT(!"Could not start thread pool!  "
                       "Threads cannot be created!");
               break;
            }
            if (veryVerbose) cout << "Thread-pool Started" << endl;

            TestQueue theQueue(10, MAX_QUEUESIZE, &tp, &ta);

            UsageTestChecker checker(theQueue.multiplexor());
            bcemt_Semaphore startSemaphore;
            checker.setSemaphore(&startSemaphore);

            for (int i = 0; i < NUM_JOBS; ++i) {
               LOOP_ASSERT(i, 0 == theQueue.processJob(checker));
            }

            startSemaphore.post(NUM_JOBS);
            tp.stop();
            if (veryVerbose) cout << "Thread-pool Stopped" << endl;

            ASSERT(NUM_JOBS == checker.timesCalled());
            ASSERT(MAX_PROC >= checker.maxProcessors());
            if (verbose) {
                P(checker.timesCalled());
                P(checker.maxProcessors());
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // 1-length Queue Test
        //
        // Concerns:
        //  Test the behavior of the component when the queue has a capacity
        //  of only 1.
        //
        // Plan:
        //  Test the component with 7 threads and a queue of 1.
        //
        // Testing:
        //   processJob();
        // --------------------------------------------------------------------

        if (verbose) cout << "SMALL QUEUE TEST" << endl
                          << "================" << endl;

        bcema_TestAllocator ta(veryVeryVeryVerbose);
        {
            enum {
                NUM_THREADS    = 7, // total number of threads
                MAX_QUEUESIZE  = 1,
                NUM_QUEUES     = 3,
                IMPORTANT_JOBS = 6,
                URGENT_JOBS    = 4,
                OTHER_JOBS     = 2
            };

            int maxProc = bsl::max
               (1.0, ceil(double(NUM_THREADS) / (NUM_QUEUES-1))-1);

            bcep_FixedThreadPool tp(NUM_THREADS, MAX_QUEUESIZE, &ta);
            TestQueue importantQueue(maxProc, MAX_QUEUESIZE, &tp, &ta);
            TestQueue urgentQueue(maxProc, MAX_QUEUESIZE, &tp, &ta);
            TestQueue otherQueue(maxProc, MAX_QUEUESIZE, &tp, &ta);

            if (0 != tp.start()) {
               ASSERT(!"Could not start thread pool!  "
                       "Threads cannot be created!");
               break;
            }
            if (veryVerbose) cout << "Thread-pool Started" << endl;

            UsageTestChecker iChecker(importantQueue.multiplexor());
            UsageTestChecker uChecker(urgentQueue.multiplexor());
            UsageTestChecker oChecker(otherQueue.multiplexor());

            for (int i = 0; i < IMPORTANT_JOBS; ++i) {
               while(0 != importantQueue.processJob(iChecker));
            }

            for (int i = 0; i < URGENT_JOBS; ++i) {
               while(0 != urgentQueue.processJob(uChecker));
            }

            for (int i = 0; i < OTHER_JOBS; ++i) {
               while(0 != otherQueue.processJob(oChecker));
            }

            tp.stop();
            if (veryVerbose) cout << "Thread-pool Stopped" << endl;

            ASSERT(IMPORTANT_JOBS == iChecker.timesCalled());
            ASSERT(URGENT_JOBS == uChecker.timesCalled());
            ASSERT(OTHER_JOBS == oChecker.timesCalled());
            if (veryVerbose) {
               P_(IMPORTANT_JOBS); P(iChecker.timesCalled());
               P_(URGENT_JOBS); P(uChecker.timesCalled());
               P_(OTHER_JOBS); P(oChecker.timesCalled());
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
        // Concerns:
        //  The usage example provided in the component documentation compiles,
        //  runs, and behaves as advertised.
        //
        // Plan:
        //  Use the 'JobQueue' usage example as the basis for the test.
        //
        // Testing:
        //  USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "Usage Example Test" << endl
                          << "==================" << endl;

        bcema_TestAllocator ta(veryVeryVeryVerbose);

        using namespace TEST_CASE_USAGE_EXAMPLE;
        usageExample(&ta);

        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //  A bcep_ThreadMultiplexor object can be constructed and destroyed.
        //
        // Plan:
        //  Construct a bcep_ThreadMultiplexor object using a test allocator
        //  and verify that memory is reserved and then returned after
        //  destruction.
        //
        // Testing:
        //   CREATORS
        // --------------------------------------------------------------------

        if (verbose) cout << "Breathing Test" << endl
                          << "==============" << endl;

        bcema_TestAllocator ta(veryVeryVeryVerbose);
        {
            bcep_ThreadMultiplexor mX(2, 3, &ta);

            const bcep_ThreadMultiplexor& mX_ref = mX;

            ASSERT(2 == mX_ref.maxProcessors());
            ASSERT(0 == mX_ref.numProcessors());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

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
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
