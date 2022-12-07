// bdlmt_threadmultiplexor.h                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLMT_THREADMULTIPLEXOR
#define INCLUDED_BDLMT_THREADMULTIPLEXOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism for partitioning a collection of threads.
//
//@CLASSES:
// bdlmt::ThreadMultiplexor: mechanism to partition multi-threaded processing
//
//@SEE_ALSO: bdlmt_threadpool, bdlmt_fixedthreadpool
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
// The 'bdlmt::ThreadMultiplexor' provides an API, 'processJob', to process
// user-specified jobs.  A multiplexor instance is configured with a maximum
// number of "processors", i.e., the maximum number of threads that may process
// jobs at any particular time.  Additional threads enqueue jobs to a pending
// job queue, which is processed by the next available processing thread.
//
// Typically, a 'bdlmt::ThreadMultiplexor' instance is used in conjunction with
// a thread pool (e.g., 'bdlmt::FixedThreadPool'), where each thread pool
// thread calls the multiplexor 'processJob' method to perform some work.  The
// multiplexor guarantees that no more that the configured number of threads
// will process jobs concurrently.  This guarantee allows a single thread pool
// to be used in a variety of situations that require partitioning thread
// resources.
//
///Thread Safety
///-------------
// The 'bdlmt::ThreadMultiplexor' class is both *fully thread-safe* (i.e., all
// non-creator methods can correctly execute concurrently), and is
// *thread-enabled* (i.e., the class does not function correctly in a
// non-multi-threading environment).  See 'bsldoc_glossary' for complete
// definitions of *fully thread-safe* and *thread-enabled*.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Multiple Work Queues
///- - - - - - - - - - - - - - - -
// The following usage example illustrates how the 'bdlmt::ThreadMultiplexor'
// can be used to share thread resources between three separate work queues.
// Assume that there are three classes of jobs: jobs that are important, jobs
// that are urgent, and jobs that are critical.  We would like to execute each
// class of jobs in a single thread pool, but we want ensure that all types of
// jobs can be executed at any time.
//
// We begin by defining a class that encapsulates the notion of a job queue.
// Our 'JobQueue' class holds a reference to a 'bdlmt::FixedThreadPool', used
// to instantiate the job queue, and owns an instance of
// 'bdlmt::ThreadMultiplexor', used to process jobs.
//..
//  class JobQueue {
//      // This class defines a generic processor for user-defined functions
//      // ("jobs").  Jobs specified to the 'processJob' method are executed
//      // in the thread pool specified at construction.
//
//    public:
//      // PUBLIC TYPES
//      typedef bdlmt::ThreadMultiplexor::Job Job;
//          // A callback of this type my be specified to the 'processJob'
//          // method.
//
//    private:
//      // DATA
//      bdlmt::FixedThreadPool   *d_threadPool_p;  // (held, not owned)
//      bdlmt::ThreadMultiplexor  d_multiplexor;   // used to partition threads
//
//    private:
//      // NOT IMPLEMENTED
//      JobQueue(const JobQueue&);
//      JobQueue& operator=(const JobQueue&);
//
//    public:
//      // CREATORS
//      JobQueue(int                     maxProcessors,
//               bdlmt::FixedThreadPool *threadPool,
//               bslma::Allocator       *basicAllocator = 0);
//        // Create a job queue that executes jobs in the specified
//        // 'threadPool' using no more than the specified 'maxProcessors'.
//        // Optionally specify a 'basicAllocator' used to supply memory.  If
//        // 'basicAllocator' is 0, the currently installed default allocator
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
// For this usage example, we assume M (number of multiplexors) = 3, and T
// (number of threads) = 5, so maxProc = 2.  It is important to note that every
// call to 'processJob' enqueues a job to the thread pool, so the length of the
// thread pool queue determines the maximum number of jobs that can be accepted
// by the JobQueue.  (Multiple JobQueues share the same maximum *together*, so
// not all will be able to reach their individual maximum at the same time).
//..
//
//   JobQueue::JobQueue(int                     maxProcessors,
//                      bdlmt::FixedThreadPool *threadPool,
//                      bslma::Allocator       *basicAllocator)
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
//      return d_threadPool_p->tryEnqueueJob(bdlf::BindUtil::bind(
//                                  &bdlmt::ThreadMultiplexor::processJob<Job>,
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
//                             ceil(double(NUM_THREADS) / (NUM_QUEUES-1))-1);
//
//      bdlmt::FixedThreadPool tp(NUM_THREADS, MAX_QUEUESIZE);
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
//         bdlf::BindUtil::bind(&bsls::AtomicInt::add, &iCheck, 1);
//
//      JobQueue::Job ujob = bdlf::BindUtil::bind(
//         bdlf::BindUtil::bind(&bsls::AtomicInt::add, &uCheck, 1);
//
//      JobQueue::Job cjob = bdlf::BindUtil::bind(
//         bdlf::BindUtil::bind(&bsls::AtomicInt::add, &cCheck, 1);
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

#include <bdlscm_version.h>

#include <bdlcc_fixedqueue.h>

#include <bsls_atomic.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsl_functional.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslalg_typetraits.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {

namespace bdlmt {
                          // =======================
                          // class ThreadMultiplexor
                          // =======================

class ThreadMultiplexor {
    // This class provides a mechanism for facilitating the use of multiple
    // threads to perform various user-defined functions ("jobs") when some
    // degree of collaboration between threads is required.  The thread
    // multiplexor is configured with a total number of "processors",
    // representing the number of threads that may process jobs at any
    // particular time.  Additional threads enqueue jobs to a pending job
    // queue, which is processed by the next available processing thread.

  public:
    // PUBLIC TYPES
    typedef bsl::function<void()> Job;
        // A callback of this type may be passed to the 'processJob' method.

  private:
    // DATA
    bslma::Allocator     *d_allocator_p;      // memory allocator (held)
    bdlcc::FixedQueue<Job> *d_jobQueue_p;       // pending job queue (owned)
    bsls::AtomicInt        d_numProcessors;    // current number of processors
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
    ThreadMultiplexor(const ThreadMultiplexor&);
    ThreadMultiplexor& operator=(const ThreadMultiplexor&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ThreadMultiplexor,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    ThreadMultiplexor(int               maxProcessors,
                      int               maxQueueSize,
                      bslma::Allocator *basicAllocator = 0);
        // Create a thread multiplexor which uses, at most, the specified
        // 'maxProcessors' number of threads to process user-specified jobs,
        // identified as callbacks of type 'Job'.  Jobs that cannot be
        // processed immediately are placed on a queue having the specified
        // 'maxQueueSize' to be processed by the next free thread.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless '0 < maxProcessors' and
        // '0 < maxQueueSize'.

    ~ThreadMultiplexor();
        // Destroy this thread multiplexor object.

    // MANIPULATORS
    template <class t_JOBTYPE>
    int processJob(const t_JOBTYPE& job);
        // Process the specified 'job' functor in the calling thread if the
        // current number of processors is less than the maximum number of
        // processors.  Otherwise, enqueue 'job' to the pending job queue.
        // Return 0 on success, and a non-zero value otherwise.  Note that the
        // only requirements on 't_JOBTYPE' are that it defines 'operator()',
        // having a 'void' return type, and that it defines a copy constructor.

    // ACCESSORS
    int maxProcessors() const;
        // Return the maximum number of active processors.

    int numProcessors() const;
        // Return the current number of active processors.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// MANIPULATORS
template <class t_JOBTYPE>
inline
int ThreadMultiplexor::processJob(const t_JOBTYPE& job)
{
    // Execute 'job' in the calling thread if the current number of processors
    // is less than the maximum number of processors.  Otherwise, enqueue 'job'
    // to the pending job queue.  In all cases, check the pending job queue at
    // the end of the loop, as the number of processors may have changed,
    // allowing the execution of a job in the current thread.

    int previousNumProcessors = d_numProcessors;
    if (previousNumProcessors < d_maxProcessors &&
        previousNumProcessors ==
                      d_numProcessors.testAndSwap(previousNumProcessors,
                                                  previousNumProcessors + 1)) {
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
int ThreadMultiplexor::maxProcessors() const
{
    return d_maxProcessors;
}

inline
int ThreadMultiplexor::numProcessors() const
{
    return d_numProcessors;
}

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

