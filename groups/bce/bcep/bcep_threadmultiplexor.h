// bcep_threadmultiplexor.h                                           -*-C++-*-
#ifndef INCLUDED_BCEP_THREADMULTIPLEXOR
#define INCLUDED_BCEP_THREADMULTIPLEXOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism for partitioning a collection of threads.
//
//@CLASSES:
// bcep_ThreadMultiplexor: mechanism for partitioning multi-threaded processing
//
//@AUTHOR: David Rubin (drubin6)
//
//@SEE_ALSO: bcep_threadpool, bcep_fixedthreadpool
//
//@DESCRIPTION: This component provides a mechanism for partitioning a
// collection of threads, so that a single collection of threads (e.g., a
// thread pool) can be used to process various types of user-defined functions
// ("jobs") while sharing the thread resources equitably between them.  A
// typical example where this type of partitioning is desired is an application
// that performs both I/O and CPU-intensive processing.  The traditional
// approach is to create two thread pools--one for I/O, and one for
// processing--and pass control (in the form of a callback) from one thread
// pool to the other.  However, there are several problems with this approach.
// Firstly, the process incurs the overhead of context switching between
// threads, which must necessarily occur because there are two different thread
// pools.  Secondly, the process may not be able to adapt well to imbalances
// between one type of processing versus the other if the number of threads in
// each thread pool is bounded.  In this case, a large number of jobs may be
// enqueued while some portion of threads allocated to the process go unused.
// On the other hand, simply sharing a single thread pool without a provision
// for partitioning the use of threads may result in one type of processing
// starving the other.
//
// The 'bcep_ThreadMultiplexor' provides an API, 'processJob', to process
// user-specified jobs.  A multiplexor instance is configured with a maximum
// number of "processors", i.e., the maximum number of threads that may process
// jobs at any particular time.  Additional threads enqueue jobs to a pending
// job queue, which is processed by the next available processing thread.
//
// Typically, a 'bcep_ThreadMultiplexor' instance is used in conjunction with a
// thread pool (e.g., 'bcep_FixedThreadPool'), where each thread pool thread
// calls the multiplexor 'processJob' method to perform some work.  The
// multiplexor guarantees that no more that the configured number of threads
// will process jobs concurrently.  This guarantee allows a single thread pool
// to be used in a variety of situations that require partitioning thread
// resources.
//
///Thread Safety
///-------------
// The 'bcep_ThreadMultiplexor' class is both *fully thread-safe* (i.e., all
// non-creator methods can correctly execute concurrently), and is
// *thread-enabled* (i.e., the class does not function correctly in a
// non-multi-threading environment).  See 'bsldoc_glossary' for complete
// definitions of *fully thread-safe* and *thread-enabled*.
//
///Usage Examples
///--------------
// The following usage example illustrates how the 'bcep_ThreadMultiplexor' can
// be used to share thread resources between three separate work queues.
// Assume that there are three classes of jobs: jobs that are important,
// jobs that are urgent, and jobs that are critical.  We would like to
// execute each class of jobs in a single thread pool, but we want
// ensure that all types of jobs can be executed at any time.
//
// We begin by defining a class that encapsulates the notion of a job queue.
// Our 'JobQueue' class holds a reference to a 'bcep_FixedThreadPool', used to
// instantiate the job queue, and owns an instance of 'bcep_ThreadMultiplexor',
// used to process jobs.
//..
//  class JobQueue {
//      // This class defines a generic processor for user-defined functions
//      // ("jobs").  Jobs specified to the 'processJob' method are executed
//      // in the thread pool specified at construction.
//
//    public:
//      // PUBLIC TYPES
//      typedef bcep_ThreadMultiplexor::Job Job;
//          // A callback of this type my be specified to the 'processJob'
//          // method.
//
//    private:
//      // DATA
//      bcep_FixedThreadPool   *d_threadPool_p;    // (held, not owned)
//      bcep_ThreadMultiplexor  d_multiplexor;     // used to partition threads
//
//    private:
//      // NOT IMPLEMENTED
//      JobQueue(const JobQueue&);
//      JobQueue& operator=(const JobQueue&);
//
//    public:
//      // CREATORS
//      JobQueue(int                   maxProcessors,
//               bcep_FixedThreadPool *threadPool,
//               bslma_Allocator      *basicAllocator = 0);
//        // Create a job queue that executes jobs in the specified
//        // 'threadPool' using no more than the specified 'maxProcessors'.
//        // Optionally specify a 'basicAllocator' used to supply memory.  If
//        // 'basicAllocator is 0, the currently installed default allocator
//        // is used.
//
//      ~JobQueue();
//          // Destroy this object.
//
//      // MANIPULATORS
//      int processJob(const Job& job);
//          // Process the specified 'job' in the thread pool specified at
//          // construction.  Return 0 on success, and a non-zero value
//          // otherwise.
//  };
//..
// The maximum number of processors for the multiplexor instance owned by each
// 'JobQueue' is configured using the following formula, for
// T = number of threads and M = number of multiplexors > 1:
//..
//   maxProc = ceil(T / (M-1))-1
//..
// This allows multiple 'JobQueue' instances to share the same threadpool
// without starving each other when the thread pool has more than one thread.
// For this usage example, we assume M (number of multiplexors) = 3, and
// T (number of threads) = 5, so maxProc = 2.  It is important to note
// that every call to 'processJob' enqueues a job to the thread pool, so
// the length of the thread pool queue determines the maximum number of
// jobs that can be accepted by the JobQueue.  (Multiple JobQueues share
// the same maximum *together*, so not all will be able to reach
// their individual maximum at the same time).
//..
//
//   JobQueue::JobQueue(int                   maxProcessors,
//                      bcep_FixedThreadPool *threadPool,
//                      bslma_Allocator      *basicAllocator)
//   : d_threadPool_p(threadPool)
//   , d_multiplexor (maxProcessors,
//                    threadPool->queueCapacity(),
//                    basicAllocator)
//  {
//  }
//
//  JobQueue::~JobQueue()
//  {
//  }
//..
// The 'processJob' method enqueues a secondary callback into the thread pool
// that executes the user-specified 'job' through the multiplexor.
//..
//  int JobQueue::processJob(const JobQueue::Job& job)
//  {
//      return d_threadPool_p->tryEnqueueJob(
//              bdef_BindUtil::bind(&bcep_ThreadMultiplexor::processJob<Job>,
//                                  &d_multiplexor,
//                                  job));
//  }
//..
// The following program uses three instances of 'JobQueue' to process
// important, urgent, and critical jobs using a single collection of threads.
//..
//  int main(void)
//  {
//      enum {
//          NUM_THREADS   = 5,   // total number of threads
//          NUM_QUEUES    = 3,   // total number of JobQueue objects
//          MAX_QUEUESIZE = 20   // total number of pending jobs
//      };
//
//      int maxProc = bsl::max(1,
//                             ceil(double(MAX_THREADS) / (NUM_QUEUES-1))-1);
//
//      bcep_FixedThreadPool tp(NUM_THREADS, MAX_QUEUESIZE);
//      JobQueue             importantQueue(maxProc, &tp);
//      JobQueue             urgentQueue(maxProc, &tp);
//      JobQueue             criticalQueue(maxProc, &tp);
//
//      if (0 != tp.start()) {
//         ASSERT(!"Could not start thread pool!");
//         return -1;
//      }
//
//      JobQueue::Job ijob =
//         bdef_BindUtil::bind(&bces_AtomicInt::add, &iCheck, 1);
//
//      JobQueue::Job ujob = bdef_BindUtil::bind(
//         bdef_BindUtil::bind(&bces_AtomicInt::add, &uCheck, 1);
//
//      JobQueue::Job cjob = bdef_BindUtil::bind(
//         bdef_BindUtil::bind(&bces_AtomicInt::add, &cCheck, 1);
//
//      importantQueue.processJob(ijob);
//      importantQueue.processJob(ijob);
//      importantQueue.processJob(ijob);
//      importantQueue.processJob(ijob);
//      importantQueue.processJob(ijob);
//      importantQueue.processJob(ijob);
//
//      urgentQueue.processJob(ujob);
//      urgentQueue.processJob(ujob);
//      urgentQueue.processJob(ujob);
//      urgentQueue.processJob(ujob);
//
//      criticalQueue.processJob(cjob);
//      criticalQueue.processJob(cjob);
//
//      tp.stop();
//      ASSERT(6 == iCheck);
//      ASSERT(4 == uCheck);
//      ASSERT(2 == cCheck);
//      return 0;
//  }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEC_FIXEDQUEUE
#include <bcec_fixedqueue.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                        // ============================
                        // class bcep_ThreadMultiplexor
                        // ============================

class bcep_ThreadMultiplexor {
    // This class provides a mechanism for facilitating the use of multiple
    // threads to perform various user-defined functions ("jobs") when some
    // degree of collaboration between threads is required.  The thread
    // multiplexor is configured with a total number of "processors",
    // representing the number of threads that may process jobs at any
    // particular time.  Additional threads enqueue jobs to a pending job
    // queue, which is processed by the next available processing thread.

  public:
    // PUBLIC TYPES
    typedef bdef_Function<void(*)()> Job;
        // A callback of this type may be passed to the 'processJob' method.

  private:
    // DATA
    bslma_Allocator      *d_allocator_p;      // memory allocator (held)
    bcec_FixedQueue<Job> *d_jobQueue_p;       // pending job queue (owned)
    bces_AtomicInt        d_numProcessors;    // current number of processors
    int                   d_maxProcessors;    // maximum number of processors

  private:
    // PRIVATE MANIPULATORS
    int processJobQueue();
        // Process the pending job queue.  Execute each functor obtained from
        // the queue in the calling thread if the current number of processors
        // is less than the maximum number of processors.  Otherwise, enqueue
        // the job back to the pending queue.  Return 0 on success, and a
        // non-zero value otherwise.

    // NOT IMPLEMENTED
    bcep_ThreadMultiplexor(const bcep_ThreadMultiplexor&);
    bcep_ThreadMultiplexor& operator=(const bcep_ThreadMultiplexor&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcep_ThreadMultiplexor,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    bcep_ThreadMultiplexor(int              maxProcessors,
                           int              maxQueueSize,
                           bslma_Allocator *basicAllocator = 0);
        // Create a thread multiplexor which uses, at most, the specified
        // 'maxProcessors' number of threads to process user-specified jobs,
        // identified as callbacks of type 'Job'.  Jobs that cannot be
        // processed immediately are placed on a queue having the specified
        // 'maxQueueSize' to be processed by the next free thread.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless '0 < maxProcessors' and
        // '0 < maxQueueSize'.

    ~bcep_ThreadMultiplexor();
        // Destroy this thread multiplexor object.

    // MANIPULATORS
    template <typename JOBTYPE>
    int processJob(const JOBTYPE& job);
        // Process the specified 'job' functor in the calling thread if the
        // current number of processors is less than the maximum number of
        // processors.  Otherwise, enqueue 'job' to the pending job queue.
        // Return 0 on success, and a non-zero value otherwise.  Note that the
        // only requirements on 'JOBTYPE' are that it defines 'operator()',
        // having a 'void' return type, and that it defines a copy constructor.

    // ACCESSORS
    int maxProcessors() const;
        // Return the maximum number of active processors.

    int numProcessors() const;
        // Return the current number of active processors.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

// MANIPULATORS
template <typename JOBTYPE>
inline
int bcep_ThreadMultiplexor::processJob(const JOBTYPE& job)
{
    // Execute 'job' in the calling thread if the current number of processors
    // is less than the maximum number of processors.  Otherwise, enqueue 'job'
    // to the pending job queue.  In all cases, check the pending job queue at
    // the end of the loop, as the number of processors may have changed,
    // allowing the execution of a job in the current thread.

    int previousNumProcessors = d_numProcessors;
    if (previousNumProcessors < d_maxProcessors
     && previousNumProcessors == d_numProcessors.testAndSwap(
                                            previousNumProcessors,
                                            previousNumProcessors + 1))
    {
        // Process the job
        job();
        --d_numProcessors;
    }
    else {
        int rc = d_jobQueue_p->tryPushBack(job);
        if (0 != rc) {
            return rc;                                                // RETURN
        }
    }

    return processJobQueue();
}

// ACCESSORS
inline
int bcep_ThreadMultiplexor::maxProcessors() const
{
    return d_maxProcessors;
}

inline
int bcep_ThreadMultiplexor::numProcessors() const
{
    return d_numProcessors;
}

}  // close namespace BloombergLP
#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
