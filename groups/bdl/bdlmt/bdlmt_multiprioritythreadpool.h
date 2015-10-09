// bdlmt_multiprioritythreadpool.h                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLMT_MULTIPRIORITYTHREADPOOL
#define INCLUDED_BDLMT_MULTIPRIORITYTHREADPOOL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism to parallelize a prioritized sequence of jobs.
//
//@CLASSES:
//    bdlmt::MultipriorityThreadPool: mechanism to parallelize prioritized jobs
//
//@SEE_ALSO: bslmt_threadutil
//
//@DESCRIPTION: This component defines an implementation of a thread pool in
// which work items ("jobs") are associated with a limited number of integer
// priorities that determine the sequence in which enqueued jobs are executed.
// (See the package-level documentation for general information on thread
// pools.)
//
// This flavor of our generalized thread pool model associates an integral
// priority with each work item.   For efficiency of implementation, these
// priorities are limited, as indicated at construction, to a relatively small
// number 'N' of contiguous integers '[ 0 .. N - 1 ]', 0 being the most urgent.
// For this implementation, the maximum number of priorities 'N' is 32.  A
// fixed number of worker threads is also specified at construction.  Finally,
// this thread pool takes an optional allocator supplied at construction.
// Once configured, these instance parameters remain unchanged for the lifetime
// of each multi-priority thread pool object.
//
// The associated priority of a job is relevant only while that job is pending;
// once a job has begun executing, it will not be interrupted or suspended to
// make way for a another job regardless of their relative priorities.  While
// processing jobs, worker threads will always choose a more urgent job (lower
// integer value for priority) over a less urgent one.  Given two jobs having
// the same priority value, the one that has been in the thread pool's queue
// the longest is selected (FIFO order).  Note that the number of active worker
// threads does not increase or decrease depending on load.  If no jobs remain
// to be executed, surplus threads will block until work arrives.  If there are
// more jobs than threads, excess jobs wait in the queue until previous jobs
// finish.
//
// 'bdlmt::MultipriorityThreadPool' provides two interfaces for specifying
// jobs: the traditional 'void function'/'void pointer' interface and the more
// versatile functor-based interface.  The 'void function'/'void pointer'
// interface allows callers to use a C-style function to be executed as a job.
// The application need specify only the address of the function, and a single
// 'void *' argument to be passed to the function.  The specified function will
// be invoked with the specified argument by the processing (worker) thread.
// The functor-based interface allows for flexible job execution by copying the
// passed functor and executing its (invokable) 'operator()' method.  Note that
// the functor gets copied twice before it is executed, once when pushed into
// the queue, and once when popped out of it, something to keep in mind if the
// object is going to be expensive to copy.  (See the 'bdef' package-level
// documentation for more information on functors and their use.)
//
// Note that except in the case where 'numThreads() == 1', we cannot guarantee
// the exact order of the execution of the jobs in the queue.
//
// Finally an application can specify the attributes of the worker threads in a
// thread pool (e.g., guard size or stack size), by optionally supplying an
// appropriately configured 'bslmt::ThreadAttributes' object.  (See the
// 'bslmt_threadutil' component-level documentation for a description of the
// 'bslmt::ThreadAttributes' class.)  Note that the field pertaining to whether
// the worker threads should be detached or joinable is ignored.
//
///Thread Safety
///-------------
// The 'bdlmt::MultipriorityThreadPool' class is both *fully thread-safe*
// (i.e., all non-creator methods can correctly execute concurrently), and is
// *thread-enabled* (i.e., the classes does not function correctly in a
// non-multi-threading environment).  See 'bsldoc_glossary' for complete
// definitions of *fully thread-safe* and *thread-enabled*.
//
// Be aware that the behavior is undefined if any of the following methods are
// called on a threadpool from any of the threads belonging to that thread
// pool.
//: o 'stopThreads'
//: o 'suspendProcessing'
//: o 'drainJobs'
// Note that, in these cases, such undefined behavior may include deadlock.
//
///Usage
///-----
// The following two examples illustrate use of the multi-priority thread pool
// provided in this component.
//
///Example 1: The 'void function'/'void pointer' Interface
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// It is possible to enqueue a job to a multi-priority thread pool as a pointer
// to a function that takes a single 'void *' argument.  This first usage
// example will demonstrate that high-priority traffic through a thread pool is
// unimpeded by a much greater quantity of low-priority traffic.
//
// The idea here is we have a large number of jobs submitted in too little time
// for all of them to be completed.  All jobs take the same amount of time to
// complete, but there are two different priorities of jobs.  There are 100
// times more jobs of less urgent priority than of the more urgent priority,
// and there is more than enough time for the jobs of more urgent priority to
// be completed.  We want to verify that all the jobs of more urgent priority
// get completed while most of the jobs of less urgent priority do not.  This
// will demonstrate that we can construct an arrangement where traffic of low
// priority, while massively more numerous, does not impede the progress of
// higher-priority jobs:
//..
//   bsls::AtomicInt     urgentJobsDone;
//   bsls::AtomicInt lessUrgentJobsDone;
//
//   extern "C" void *urgentJob(void *)
//   {
//       bslmt::ThreadUtil::microSleep(10000);          // 10 mSec
//
//       ++urgentJobsDone;
//
//       return 0;
//   }
//
//   extern "C" void *lessUrgentJob(void *)
//   {
//       bslmt::ThreadUtil::microSleep(10000);          // 10 mSec
//
//       ++lessUrgentJobsDone;
//
//       return 0;
//   }
//..
// The main program (below) enqueues 100 times as many low-priority jobs as
// high priority ones.  10,100 jobs are submitted, each taking at least 0.01
// seconds, for a total cpu time of 101 seconds.  We use 20 threads, so that
// is about 5 seconds.  But we shut down the run after only 0.5 seconds, so
// that means at least 90% of the jobs will not complete.  When run, typical
// output of this program is:
//..
//  Jobs done: urgent: 100, less urgent: 507
//..
// meaning *all* of the urgent jobs completed, while approximately 95% of the
// less urgent jobs did not:
//..
//       bdlmt::MultipriorityThreadPool pool(20,  // # of threads
//                                         2);  // # of priorities
//
//       bsls::TimeInterval finishTime = bdlt::CurrentTime::now() + 0.5;
//       pool.startThreads();
//..
// We use 1 as our less urgent priority, leaving 0 as our urgent priority:
//..
//       for (int i = 0; i < 100; ++i) {
//           for (int j = 0; j < 100; ++j) {
//               pool.enqueueJob(&lessUrgentJob, (void *) 0, 1); // less urgent
//           }
//           pool.enqueueJob(&urgentJob, (void *) 0, 0);         // urgent
//       }
//
//       bslmt::ThreadUtil::sleep(finishTime - bdlt::CurrentTime::now());
//       pool.shutdown();
//
//       bsl::cout << "Jobs done: urgent: " << urgentJobsDone <<
//                    ", less urgent: "     << lessUrgentJobsDone << bsl::endl;
//..
//
///Example 2: The Functor-Based Interface
/// - - - - - - - - - - - - - - - - - - -
// In this second example we present a multi-threaded algorithm for calculating
// prime numbers.  This is just to serve as an illustration; although it works,
// it is not really any faster than doing it with a single thread.
//
// For every prime number 'P', we have to mark all multiples of it in two
// ranges, '[ P .. P ** 2 ]' and '[ P ** 2 .. TOP_NUMBER ]', as non-prime,
// where we use 2000 for 'TOP_NUMBER' in this example.  For any 'P ** 2', if we
// can determine that all primes below 'P' have marked all their multiples up
// to 'P ** 2', then we can scan that range and any unmarked values in it will
// be a new prime.  The we can start out with our first prime, 2, and mark all
// primes between it and '2 ** 2 == 4', thus discovering 3 is prime.  Once we
// have marked all multiples of 2 and 3 below '3 * 3 == 9', we can then scan
// that range and discover 5 and 7 are primes, and repeat the process to
// discover bigger and bigger primes until we have covered an entire range (in
// this example all ints below TOP_NUMBER == 2000):
//..
//   enum {
//       TOP_NUMBER     = 2000,
//       NUM_PRIORITIES = 32
//   };
//
//   bool isStillPrime[TOP_NUMBER];
//   bsls::AtomicInt scannedTo[TOP_NUMBER];  // if 'P' is a prime, what is the
//                                           // highest multiple of 'P' that
//                                           // we have marked
//                                           // 'isStillPrime[P] = false'
//
//   bsls::AtomicInt maxPrimeFound;          // maximum prime identified so far
//   int primeNumbers[TOP_NUMBER];           // elements in the range
//                                           // '0 .. numPrimeNumbers - 1' are
//                                           // the prime numbers we have found
//                                           // so far
//   bsls::AtomicInt numPrimeNumbers;
//
//   bdlmt::MultipriorityThreadPool *threadPool;
//
//   bool          doneFlag;                 // set this flag to signal
//                                           // other jobs that we're done
//   bslmt::Barrier doneBarrier(2);           // we wait on this barrier
//                                           // to signal the main thread
//                                           // that we're done
//
//   struct Functor {
//       static bslmt::Mutex s_mutex;
//       int                d_numToScan;
//       int                d_priority;
//       int                d_limit;
//
//       Functor(int numToScan)
//       : d_numToScan(numToScan)
//       , d_priority(0)
//       {
//           d_limit = bsl::min((double) numToScan * numToScan,
//                              (double) TOP_NUMBER);
//       }
//
//       void setNewPrime(int newPrime) {
//           maxPrimeFound = newPrime;
//           primeNumbers[numPrimeNumbers] = newPrime;
//           ++numPrimeNumbers;
//
//           if (2 * newPrime < TOP_NUMBER) {
//               Functor f(newPrime);
//
//               threadPool->enqueueJob(f, 0);
//           }
//       }
//
//       void evaluateCandidatesForPrime() {
//           if (maxPrimeFound > d_limit) {
//               return;
//           }
//
//           int numToScanI;
//           for (numToScanI = numPrimeNumbers - 1; numToScanI > 0;
//                                                           --numToScanI) {
//               if (primeNumbers[numToScanI] == d_numToScan) {
//                   break;
//               }
//           }
//           for (int i = numToScanI - 1; i < 0; --i) {
//               if (TOP_NUMBER < scannedTo[primeNumbers[i]]) {
//                   for (int j = i + 1; j < numPrimeNumbers; ++j) {
//                       if (TOP_NUMBER == scannedTo[primeNumbers[j]]) {
//                           scannedTo[primeNumbers[j]] = TOP_NUMBER + 1;
//                       }
//                       else {
//                           break;
//                       }
//                   }
//                   break;
//               }
//
//               if (scannedTo[primeNumbers[i]] < d_limit) {
//                   // Not all multiples of all prime numbers below
//                   // us have been adequately marked as non-prime.  We
//                   // cannot yet draw any new conclusions about what
//                   // is and what is not prime in this range.
//
//                   // Resubmit ourselves to the back of the priority queue
//                   // so that we'll get re-evaluated when previous prime
//                   // numbers are done scanning.  Note that we could get
//                   // reenqueued several times.
//
//                   // Note that jobs marking the 'isStillPrime' array are
//                   // at priority 0, while later incarnations that can
//                   // only set new primes are at priority 1 and keep
//                   // getting resubmitted at less and less urgent
//                   // priorities until all their prerequisites (which
//                   // are at priority 0) are done.
//
//                   d_priority = bsl::min(NUM_PRIORITIES - 2,
//                                         d_priority + 1);
//                   threadPool->enqueueJob(*this, d_priority);
//
//                   return;
//               }
//           }
//
//           // Everything up to 'd_limit' that has not been marked
//           // non-prime is prime.
//
//           bslmt::LockGuard<bslmt::Mutex> guard(&s_mutex);
//
//           for (int i = maxPrimeFound + 1; d_limit > i; ++i) {
//               if (isStillPrime[i]) {
//                   setNewPrime(i);
//               }
//           }
//
//           if (TOP_NUMBER == d_limit && !doneFlag) {
//               // We have successfully listed all primes below 'TOP_NUMBER'.
//               // Touch the done barrier and our caller will then know that
//               // we are done and shut down the queue.
//
//               doneFlag = true;
//               doneBarrier.wait();
//           }
//       }
//
//       void operator()() {
//           if (0 == d_priority) {
//               bsls::AtomicInt& rScannedTo = scannedTo[d_numToScan];
//
//               for (int i = d_numToScan; i < d_limit; i += d_numToScan) {
//                   isStillPrime[i] = false;
//                   rScannedTo = i;
//               }
//
//               d_priority = 1;
//               threadPool->enqueueJob(*this, d_priority);
//
//               for (int i = d_limit; i < TOP_NUMBER; i += d_numToScan) {
//                   isStillPrime[i] = false;
//                   rScannedTo = i;
//               }
//               rScannedTo = TOP_NUMBER;
//           }
//           else {
//               evaluateCandidatesForPrime();
//           }
//       }
//   };
//   bslmt::Mutex Functor::s_mutex;
//..
// and in the main program:
//..
//         for (int i = 0; TOP_NUMBER > i; ++i) {
//             isStillPrime[i] = true;
//             scannedTo[i] = 0;
//         }
//
//         scannedTo[0] = TOP_NUMBER + 1;
//         scannedTo[1] = TOP_NUMBER + 1;
//
//         maxPrimeFound = 2;
//         primeNumbers[0] = 2;
//         numPrimeNumbers = 1;
//         doneFlag = false;
//
//         bdlmt::MultipriorityThreadPool threadPool(20, NUM_PRIORITIES);
//         threadPool.startThreads();
//
//         Functor f(2);
//         threadPool.enqueueJob(f, 0);
//
//         doneBarrier.wait();
//
//         threadPool.shutdown();
//
//         if (verbose) {
//             printf("%d prime numbers below %d:", (int) numPrimeNumbers,
//                                                  TOP_NUMBER);
//
//             for (int i = 0; numPrimeNumbers > i; ++i) {
//                 printf("%s%4d", 0 == i % 10 ? "\n    " : ", ",
//                                                          primeNumbers[i]);
//             }
//             printf("\n");
//         }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLCC_MULTIPRIORITYQUEUE
#include <bdlcc_multipriorityqueue.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLMT_THREADATTRIBUTES
#include <bslmt_threadattributes.h>
#endif

#ifndef INCLUDED_BSLMT_THREADUTIL
#include <bslmt_threadutil.h>
#endif

#ifndef INCLUDED_BSLMT_THREADGROUP
#include <bslmt_threadgroup.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

#if defined(BSLS_PLATFORM_OS_UNIX)
    #ifndef INCLUDED_BSL_C_SIGNAL
    #include <bsl_c_signal.h>
    #endif
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

namespace BloombergLP {

namespace bdlmt {
                       // =============================
                       // class MultipriorityThreadPool
                       // =============================

class MultipriorityThreadPool {
    // This class implements a thread-enabled, integrally-prioritized
    // thread-pool mechanism used for concurrently executing multiple
    // user-defined "jobs" supplied as either conventional C-style functions
    // taking an optional 'void *' data argument, or as more flexible functor
    // objects.

  public:
    // TYPES
    typedef bsl::function<void()> ThreadFunctor;
        // 'ThreadFunctor' is an alias for a function object that is invokable
        // (with no arguments) and returns 'void'; its use is similar to that
        // of a C-style function pointer and optional 'void *' data pointer
        // passed as arguments when creating a thread.

    enum {
        k_MAX_NUM_PRIORITIES = sizeof(int) * CHAR_BIT  // bits per 'int'
    };

  private:
    // DATA
    bslmt::Mutex            d_mutex;      // mutex for worker threads as they
                                          // analyze state, and for methods
                                          // that manipulate that state

    bslmt::Mutex            d_metaMutex;  // mutex for '[start|stop]Threads',
                                          // '[suspend|resume]Processing',
                                          // 'drainJobs'; this mutex gets
                                          // locked much less frequently than
                                          // 'd_mutex', but for longer periods
                                          // of time

    bdlcc::MultipriorityQueue<ThreadFunctor>
                            d_queue;      // pending job queue

    bslmt::ThreadAttributes d_threadAttributes;
                                          // user-supplied attributes of all
                                          // the threads this pool spawns

    bslmt::ThreadGroup      d_threadGroup;
                                          // thread group managing our threads

    const int               d_numThreads;
                                          // user-supplied number of threads
                                          // to be used to process jobs

    bsls::AtomicInt         d_threadStartState;
                                          // enum of type 'StartState' (local
                                          // to .cpp) regarding whether threads
                                          // are started

    bsls::AtomicInt         d_threadSuspendState;
                                          // enum of type 'ResumeState' (local
                                          // to .cpp) regarding whether threads
                                          // are suspended

    bsls::AtomicInt         d_numStartedThreads;
                                          // number of threads currently
                                          // started

    bsls::AtomicInt         d_numSuspendedThreads;
                                          // number of threads currently
                                          // suspended

    bsls::AtomicInt         d_numActiveThreads;
                                          // number of threads currently
                                          // processing jobs

    bslmt::Condition        d_allThreadsStartedCondition;
                                          // broadcast when number of started
                                          // threads reaches 'd_numThreads',
                                          // watched during starting

    bslmt::Condition        d_allThreadsSuspendedCondition;
                                          // broadcast when number of started
                                          // threads reaches 'd_numThreads',
                                          // watched during starting

    bslmt::Condition        d_resumeCondition;
                                          // broadcast when suspended threads
                                          // are to resume

    // NOT IMPLEMENTED
    MultipriorityThreadPool(const MultipriorityThreadPool&);
    MultipriorityThreadPool& operator=(const MultipriorityThreadPool&);

    // PRIVATE MANIPULATORS
    void worker();
        // This method runs in each thread of this multi-priority thread pool,
        // processing jobs until commanded to stop, in which case the thread
        // exits to be joined.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(MultipriorityThreadPool,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    MultipriorityThreadPool(int               numThreads,
                            int               numPriorities,
                            bslma::Allocator *basicAllocator = 0);
    MultipriorityThreadPool(
                           int                             numThreads,
                           int                             numPriorities,
                           const bslmt::ThreadAttributes&  threadAttributes,
                           bslma::Allocator               *basicAllocator = 0);
        // Create a multi-priority thread pool capable of concurrently
        // executing the specified 'numThreads' "jobs" with associated integer
        // priorities in the specified range '[0 .. numPriorities - 1]', 0
        // being the most urgent.  Optionally specify 'threadAttributes' used
        // to customize each worker thread created by this thread pool, in
        // which case the attribute pertaining to whether the worker threads
        // are created in the detached state is ignored.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The newly
        // created thread pool will initially be enabled for enqueuing jobs,
        // but with no worker threads created.  The behavior is undefined
        // unless '1 <= numThreads' and
        // '1 <= numPriorities <= k_MAX_NUM_PRIORITIES'.

    ~MultipriorityThreadPool();
        // Remove (cancel) all pending jobs and destroy this multi-priority
        // thread pool.  The behavior is undefined unless this thread pool is
        // stopped.

    // MANIPULATORS
    int enqueueJob(const ThreadFunctor& job, int priority);
        // Add the specified 'job' to the queue of this multi-priority thread
        // pool, assigning it the specified 'priority'.  Return 0 if the job
        // was enqueued successfully, and a non-zero value otherwise (implying
        // that the queue was in the disabled state).  The behavior is
        // undefined unless '0 <= priority < numPriorities()'.

    int enqueueJob(bslmt_ThreadFunction  jobFunction,
                   void                 *jobData,
                   int                   priority);
        // Add a job to the queue of this multi-priority thread pool indicated
        // by the specified 'jobFunction' and associated 'jobData', assigning
        // it the specified 'priority'.  Return 0 if the job was enqueued
        // successfully, and a non-zero value otherwise (implying that the
        // queue was in the disabled state).  When invoked, 'jobFunction' is
        // passed the 'void *' address 'jobData' as its only argument.  The
        // behavior is undefined unless 'jobFunction' is non-null and
        // '0 <= priority < numPriorities()'.  Note that 'jobData' may be 0 as
        // long as 'jobFunction' supports that value.

    void enableQueue();
        // Enable the enqueuing of jobs to this multi-priority thread pool.
        // When this thread pool is enabled, the status returned when calling
        // either overloaded 'enqueueJob' method will be 0, indicating that the
        // job was successfully enqueued.  Note that calling this method when
        // the queue is already in the enabled state has no effect.

    void disableQueue();
        // Disable the enqueuing of jobs to this multi-priority thread pool.
        // When this thread pool is disabled, the status returned when calling
        // either overloaded 'enqueueJob' method will be non-zero, indicating
        // that the job was *not* enqueued.  Note that calling this method when
        // the queue is already in the disabled state has no effect.

    int startThreads();
        // Create and start 'numThreads()' worker threads in this
        // multi-priority thread pool.  Return 0 on success, and a non-zero
        // value with no worker threads and no jobs processed otherwise.  This
        // method has no impact on the enabled/disabled or suspended/resumed
        // states of this thread pool.  Note that calling this method when this
        // thread pool is already in the started state has no effect.  Also
        // note that until this method is called, the thread pool will not
        // process any jobs.

    void stopThreads();
        // Destroy all worker threads of this multi-priority thread pool after
        // waiting for any active (i.e., already-running) jobs to complete; no
        // new jobs will be allowed to become active.  This method has no
        // impact on the enabled/disabled or suspended/resumed states of this
        // thread pool.  Note that calling this function when this thread pool
        // is not in the started state has no effect.  Also not that calling
        // this method from one of the threads belonging to this thread pool
        // will cause a deadlock.

    void suspendProcessing();
        // Put this multi-priority thread pool into the suspended state.  This
        // method does not suspend any jobs that have begun to execute; such
        // jobs are allowed to complete.  No pending jobs, however, will be
        // allowed to begin execution until 'resumeProcessing()' is called.
        // This is orthogonal to start/stop, and enable/disable; no threads are
        // stopped.  If this thread pool is started, this call blocks until all
        // threads have finished any jobs they were processing and have gone
        // into suspension.  This method has no effect if this thread pool was
        // already in the suspended state.  Note that calling this method from
        // one of the threads belonging to this thread pool will cause a
        // deadlock.

    void resumeProcessing();
        // If this multi-priority thread pool is suspended, resume processing
        // of jobs.  This is orthogonal to start/stop, and enable/disable; no
        // threads are started.  Note that this method has no effect if this
        // thread pool is not in the suspended state.

    void drainJobs();
        // Block until all executing jobs and pending jobs enqueued to this
        // multi-priority thread pool complete.  This method does not affect
        // the enabled/disabled state of this thread pool.  If this thread pool
        // is enabled and jobs are enqueued during draining, this method may
        // return before all enqueued jobs are executed.  The behavior is
        // undefined if:
        //
        //: o this method is called while this thread pool is stopped or
        //:   suspended, or
        //:
        //: o this method is called concurrently with a call to the
        //:   'removeJobs' method, or
        //:
        //: o this method is called by one of the threads belonging to this
        //:   thread pool.
        //
        // Note that, in these cases, such undefined behavior may include
        // deadlock.

    void removeJobs();
        // Remove all pending (i.e., not yet active) jobs from the queue of
        // this multi-priority thread pool.  This method does not affect the
        // enabled status of the queue, nor does it affect the started status
        // or any active jobs in this thread pool.  The behavior is undefined
        // if this method is called concurrently with the 'drainJobs' method.
        // Note that, in this case, such undefined behavior may include
        // deadlock.

    void shutdown();
        // Disable the enqueuing of new jobs to this multi-priority thread
        // pool, cancel all pending jobs, and stop all worker threads.

    // ACCESSORS
    bool isEnabled() const;
        // Return 'true' if the enqueuing of new jobs is enabled for this
        // multi-priority thread pool, and 'false' otherwise.

    bool isStarted() const;
        // Return 'true' if all 'numThreads()' worker threads (specified at
        // construction) have been created for this multi-priority thread pool,
        // and 'false' otherwise.

    bool isSuspended() const;
        // Return 'true' if the threads of this multi-priority thread pool are
        // currently suspended from processing jobs, and 'false' otherwise.

    int numActiveThreads() const;
        // Return a snapshot of the number of threads that are actively
        // processing jobs for this multi-priority thread pool.  Note that
        // '0 <= numActiveThreads() <= numThreads()' is an invariant of this
        // class.

    int numPendingJobs() const;
        // Return a snapshot of the number of jobs currently enqueued to be
        // processed by this multi-priority thread pool, but are not yet
        // running.

    int numPriorities() const;
        // Return the fixed number of priorities, specified at construction,
        // that this multi-priority thread pool supports.

    int numStartedThreads() const;
        // Return the number of threads that have been started for this
        // multi-priority thread pool.  Note that they may be currently
        // suspended.

    int numThreads() const;
        // Returns the fixed number of threads, specified at construction, that
        // are started by this multi-priority thread pool.
};

}  // close package namespace
}  // close enterprise namespace

#endif

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
