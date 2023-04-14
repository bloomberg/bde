// bdlmt_fixedthreadpool.h                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLMT_FIXEDTHREADPOOL
#define INCLUDED_BDLMT_FIXEDTHREADPOOL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide portable implementation for a fixed-size pool of threads.
//
//@CLASSES:
//   bdlmt::FixedThreadPool: portable fixed-size thread pool
//
//@SEE_ALSO: bdlmt_threadpool
//
//@DESCRIPTION: This component defines a portable and efficient implementation
// of a thread pool, 'bdlmt::FixedThreadPool', that can be used to distribute
// various user-defined functions ("jobs") to a separate threads to execute the
// jobs concurrently.  Each thread pool object manages a fixed number of
// processing threads and can hold up to a fixed maximum number of pending
// jobs.
//
// 'bdlmt::FixedThreadPool' implements a queuing mechanism that distributes
// work among the threads.  Jobs are queued for execution as they arrive, and
// each queued job is processed by the next available thread.  If each of the
// concurrent threads is busy processing a job, new jobs will remain enqueued
// until a thread becomes available.  If the queue capacity is reached,
// enqueuing jobs will block until threads consume more jobs from the queue,
// causing its length to drop below its capacity.  Both the queue's capacity
// and number of threads are specified at construction and cannot be changed.
//
// The thread pool provides two interfaces for specifying jobs: the commonly
// used "void function/void pointer" interface and the more versatile functor
// based interface.  The void function/void pointer interface allows callers to
// use a C-style function to be executed as a job.  The application need only
// specify the address of the function, and a single void pointer argument, to
// be passed to the function.  The specified function will be invoked with the
// specified argument by the processing thread.  The functor based interface
// allows for more flexible job execution such as the invocation of member
// functions or the passing of multiple user-defined arguments.  See the 'bdef'
// package-level documentation for more on functors and their usage.
//
// Unlike a 'bdlmt::ThreadPool', an application can not tune a
// 'bdlmt::FixedThreadPool' once it is created with a specified number of
// threads and queue capacity, hence the name "fixed" thread pool.  An
// application can, however, specify the attributes of the threads in the pool
// (e.g., thread priority or stack size), by providing a
// 'bslmt::ThreadAttributes' object with the desired values set.  See
// 'bslmt_threadutil' package documentation for a description of
// 'bslmt::ThreadAttributes'.
//
// Thread pools are ideal for developing multi-threaded server applications.  A
// server need only package client requests to execute as jobs, and
// 'bdlmt::FixedThreadPool' will handle the queue management, thread
// management, and request dispatching.  Thread pools are also well suited for
// parallelizing certain types of application logic.  Without any complex or
// redundant thread management code, an application can easily create a thread
// pool, enqueue a series of jobs to be executed, and wait until all the jobs
// have executed.
//
///Thread Safety
///-------------
// The 'bdlmt::FixedThreadPool' class is both *fully thread-safe* (i.e., all
// non-creator methods can correctly execute concurrently), and is
// *thread-enabled* (i.e., the classes does not function correctly in a
// non-multi-threading environment).  See 'bsldoc_glossary' for complete
// definitions of *fully thread-safe* and *thread-enabled*.
//
///Synchronous Signals on Unix
///---------------------------
// A thread pool ensures that, on unix platforms, all the threads in the pool
// block all asynchronous signals.  Specifically all the signals, except the
// following synchronous signals are blocked:
//..
// SIGBUS
// SIGFPE
// SIGILL
// SIGSEGV
// SIGSYS
// SIGABRT
// SIGTRAP
// SIGIOT
//..
//
///Thread Names for Sub-Threads
///----------------------------
// To facilitate debugging, users can provide a thread name as the 'threadName'
// attribute of the 'bslmt::ThreadAttributes' argument passed to the
// constructor, that will be used for all the sub-threads.  The thread name
// should not be used programmatically, but will appear in debugging tools on
// platforms that support naming threads to help users identify the source and
// purpose of a thread.  If no 'ThreadAttributes' object is passed, or if the
// 'threadName' attribute is not set, the default value "bdl.FixedPool" will be
// used.
//
///Usage
///-----
// This example demonstrates the use of a 'bdlmt::FixedThreadPool' to
// parallelize a segment of program logic.  The example implements a
// multi-threaded file search utility.  The utility searches multiple files for
// a string, similar to the Unix command 'fgrep'; the use of a
// 'bdlmt::FixedThreadPool' allows the utility to search multiple files
// concurrently.
//
// The example program will take as input a string and a list of files to
// search.  The program creates a 'bdlmt::FixedThreadPool', and then enqueues a
// single "job" for each file to be searched.  Each thread in the pool will
// take a job from the queue, open the file, and search for the string.  If a
// match is found, the job adds the filename to an array of matching filenames.
// Because this array of filenames is shared across multiple jobs and across
// multiple threads, access to the array is controlled via a 'bslmt::Mutex'.
//
///Setting FixedThreadPool Attributes
/// - - - - - - - - - - - - - - - - -
// To get started, we declare thread attributes, to be used in constructing the
// thread pool.  In this example, our choices for number of threads and queue
// capacity are arbitrary.
//..
//  #define SEARCH_THREADS         10
//  #define SEARCH_QUEUE_CAPACITY  50
//..
// Below is the structure that will be used to pass arguments to the file
// search function.  Since each job will be searching a separate file, a
// distinct instance of the structure will be used for each job.
//..
//   struct my_FastSearchJobInfo {
//       const bsl::string        *d_word;    // word to search for
//       const bsl::string        *d_path;    // path of the file to search
//       bslmt::Mutex             *d_mutex;   // mutex to control access to the
//                                            // result file list
//       bsl::vector<bsl::string> *d_outList; // list of matching files
//   };
//..
//
///The "void function/void pointer" Interface
/// - - - - - - - - - - - - - - - - - - - - -
// 'myFastSearchJob' is the search function to be executed as a job by threads
// in the thread pool, matching the "void function/void pointer" interface.
// The single 'void *' argument is received and cast to point to a
// 'struct my_FastSearchJobInfo', which then points to the search string and a
// single file to be searched.  Note that different 'my_FastSearchJobInfo'
// structures for the same search request will differ only in the attribute
// 'd_path', which points to a specific filename among the set of files to be
// searched; other fields will be identical across all structures for a given
// Fast Search.
//
// See the following section for an illustration of the functor interface.
//..
//   static void myFastSearchJob(void *arg)
//   {
//       my_FastSearchJobInfo *job =  (my_FastSearchJobInfo*)arg;
//       FILE *file;
//
//       file = fopen(job->d_path->c_str(), "r");
//
//       if (file) {
//           char  buffer[1024];
//           size_t nread;
//           int wordLen = job->d_word->length();
//           const char *word = job->d_word->c_str();
//
//           nread = fread(buffer, 1, sizeof(buffer) - 1, file);
//           while (nread >= wordLen) {
//               buffer[nread] = 0;
//               if (strstr(buffer, word)) {
//..
// If we find a match, we add the file to the result list and return.  Since
// the result list is shared among multiple processing threads, we use a mutex
// lock to regulate access to the list.  We use a 'bslmt::LockGuard' to manage
// access to the mutex lock.  This template object acquires a mutex lock on
// 'job->d_mutex' at construction, releases that lock on destruction.  Thus,
// the mutex will be locked within the scope of the 'if' block, and released
// when the program exits that scope.
//
// See 'bslmt_threadutil' for information about the 'bslmt::Mutex' class, and
// component 'bslmt_lockguard' for information about the 'bslmt::LockGuard'
// template class.
//..
//                bslmt::LockGuard<bslmt::Mutex> lock(job->d_mutex);
//                job->d_outList->push_back(*job->d_path);
//                break;  // bslmt::LockGuard destructor unlocks mutex.
//            }
//            memcpy(buffer, &buffer[nread - wordLen - 1], wordLen - 1);
//            nread = fread(buffer + wordLen - 1, 1, sizeof(buffer) - wordLen,
//                          file);
//        }
//        fclose(file);
//       }
//   }
//..
// Routine 'myFastSearch' is the main driving routine, taking three arguments:
// a single string to search for ('word'), a list of files to search, and an
// output list of files.  When the function completes, the file list will
// contain the names of files where a match was found.
//..
//   void  myFastSearch(const bsl::string&              word,
//                      const bsl::vector<bsl::string>& fileList,
//                      bsl::vector<bsl::string>&       outFileList)
//   {
//       bslmt::Mutex     mutex;
//       bslmt::ThreadAttributes defaultAttributes;
//..
// We initialize the thread pool using default thread attributes.  We then
// start the pool so that the threads can begin while we prepare the jobs.
//..
//       bdlmt::FixedThreadPool pool(defaultAttributes,
//                                 SEARCH_THREADS,
//                                 SEARCH_QUEUE_CAPACITY);
//
//       if (0 != pool.start()) {
//           bsl::cerr << "Thread start() failed.  Thread quota exceeded?"
//                     << bsl::endl;
//           exit(1);
//       }
//..
// For each file to be searched, we create the job info structure that will be
// passed to the search function and add the job to the pool.
//
// As noted above, all jobs will share a single mutex to guard the output file
// list.  Function 'myFastSearchJob' uses a 'bslmt::LockGuard' on this mutex to
// serialize access to the list.
//..
//       int count = fileList.size();
//       my_FastSearchJobInfo *jobInfoArray = new my_FastSearchJobInfo[count];
//
//       for (int i = 0; i < count; ++i) {
//           my_FastSearchJobInfo &job = jobInfoArray[i];
//           job.d_word    = &word;
//           job.d_path    = &fileList[i];
//           job.d_mutex   = &mutex;
//           job.d_outList = &outFileList;
//           pool.enqueueJob(myFastSearchJob, &job);
//       }
//..
// Now we simply wait for all the jobs in the queue to complete.  Any matched
// files should have been added to the output file list.
//..
//       pool.drain();
//       delete[] jobInfoArray;
//   }
//..
//
///The Functor Interface
///- - - - - - - - - - -
// The "void function/void pointer" convention is idiomatic for C programs.
// The 'void' pointer argument provides a generic way of passing in user data,
// without regard to the data type.  Clients who prefer better or more explicit
// type safety may wish to use the Functor Interface instead.  This interface
// uses 'bsl::function' to provide type-safe wrappers that can match argument
// number and type for a C++ free function or member function.
//
// To illustrate the Functor Interface, we will make two small changes to the
// usage example above.  First, we change the signature of the function that
// executes a single job, so that it uses a 'my_FastSearchJobInfo' pointer
// rather than a 'void' pointer.  With this change, we can remove the first
// executable statement, which casts the 'void *' pointer to
// 'my_FastSearchJobInfo *'.
//..
//   static void myFastFunctorSearchJob(my_FastSearchJobInfo *job)
//   {
//       FILE *file;
//
//       file = fopen(job->d_path->c_str(), "r");
//       // the rest of the function is unchanged.
//..
// Next, we make a change to the loop that enqueues the jobs in 'myFastSearch'.
// We create a functor - a C++ object that acts as a function.  The thread pool
// will "execute" this functor (by calling its 'operator()' member function) on
// a thread when one becomes available.
//..
//       for (int i = 0; i < count; ++i) {
//           my_FastSearchJobInfo &job = jobInfoArray[i];
//           job.d_word    = &word;
//           job.d_path    = &fileList[i];
//           job.d_mutex   = &mutex;
//           job.d_outList = &outFileList;
//
//           bsl::function<void()> jobHandle =
//                         bdlf::BindUtil::bind(&myFastFunctorSearchJob, &job);
//           pool.enqueueJob(jobHandle);
//       }
//..
// Use of 'bsl::function' and 'bdlf::BindUtil' is described in the 'bdef'
// package documentation.  For this example, it is important to note that
// 'jobHandle' is a functor object, and that 'bdlf::BindUtil::bind' populates
// that functor object with a function pointer (to the 'void' function
// 'myFastFunctorSearchJob') and user data ('&job').  When the functor is
// executed via 'operator()', it will in turn execute the
// 'myFastFunctorSearchJob' function with the supplied data as its argument.
//
// Note also that the functor is created locally and handed to the thread pool.
// The thread pool copies the functor onto its internal queue, and takes
// responsibility for the copied functor until execution is complete.
//
// The function is completed exactly as it was in the previous example.
//..
//       pool.drain();
//       delete[] jobInfoArray;
//   }
//..

#include <bdlscm_version.h>

#include <bdlcc_boundedqueue.h>

#include <bslmf_movableref.h>

#include <bslmt_barrier.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>
#include <bslmt_threadgroup.h>

#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_platform.h>

#include <bdlf_bind.h>

#include <bslma_allocator.h>

#include <bsl_cstdlib.h>
#include <bsl_functional.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#include <bdlcc_fixedqueue.h>

#include <bslmt_condition.h>
#include <bslmt_semaphore.h>

#include <bsl_algorithm.h>

#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

extern "C" typedef void (*bcep_FixedThreadPoolJobFunc)(void *);
        // This type declares the prototype for functions that are suitable to
        // be specified 'bdlmt::FixedThreadPool::enqueueJob'.

#endif // BDE_OMIT_INTERNAL_DEPRECATED

namespace bdlmt {

extern "C" typedef void (*FixedThreadPoolJobFunc)(void *);
    // This type declares the prototype for functions that are suitable to be
    // specified 'bdlmt::FixedThreadPool::enqueueJob'.

                          // =====================
                          // class FixedThreadPool
                          // =====================

class FixedThreadPool {
    // This class implements a thread pool used for concurrently executing
    // multiple user-defined functions ("jobs").

  public:
    // TYPES
    typedef bsl::function<void()>    Job;
    typedef bdlcc::BoundedQueue<Job> Queue;

    // PUBLIC CONSTANTS
    enum {
        e_SUCCESS  = Queue::e_SUCCESS,
        e_FULL     = Queue::e_FULL,
        e_DISABLED = Queue::e_DISABLED,
        e_FAILED   = Queue::e_FAILED
    };

    enum {
        e_STOP
      , e_RUN
      , e_SUSPEND
      , e_DRAIN
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BCEP_STOP    = e_STOP
      , BCEP_RUN     = e_RUN
      , BCEP_SUSPEND = e_SUSPEND
      , BCEP_DRAIN   = e_DRAIN
      , TP_STOP    = e_STOP
      , TP_RUN     = e_RUN
      , TP_SUSPEND = e_SUSPEND
      , TP_DRAIN   = e_DRAIN
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

  private:
    // PRIVATE CLASS DATA
    static const char       s_defaultThreadName[16];  // Thread name to use
                                                      // when none is
                                                      // specified.

    // PRIVATE DATA
    Queue                   d_queue;              // underlying queue

    bsls::AtomicInt         d_numActiveThreads;   // number of threads
                                                  // processing jobs

    bsls::AtomicBool        d_drainFlag;          // set when draining

    bslmt::Barrier          d_barrier;            // barrier to sync threads
                                                  // during 'start' and 'drain'

    bslmt::Mutex            d_metaMutex;          // mutex to ensure that there
                                                  // is only one controlling
                                                  // thread at any time

    bslmt::ThreadGroup      d_threadGroup;        // threads used by this pool

    bslmt::ThreadAttributes d_threadAttributes;   // thread attributes to be
                                                  // used when constructing
                                                  // processing threads

    const int               d_numThreads;         // number of configured
                                                  // processing threads.

#if defined(BSLS_PLATFORM_OS_UNIX)
    sigset_t                d_blockSet;           // set of signals to be
                                                  // blocked in managed threads
#endif

    // PRIVATE MANIPULATORS
    void workerThread();
        // The main function executed by each worker thread.

    int startNewThread();
        // Internal method to spawn a new processing thread and increment the
        // current count.  Note that this method must be called with
        // 'd_metaMutex' locked.

    // NOT IMPLEMENTED
    FixedThreadPool(const FixedThreadPool&);
    FixedThreadPool& operator=(const FixedThreadPool&);

  public:
    // CREATORS
    FixedThreadPool(int               numThreads,
                    int               maxNumPendingJobs,
                    bslma::Allocator *basicAllocator = 0);
        // Construct a thread pool with the specified 'numThreads' number of
        // threads and a job queue of capacity sufficient to enqueue the
        // specified 'maxNumPendingJobs' without blocking.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '1 <= numThreads'.

    FixedThreadPool(const bslmt::ThreadAttributes&  threadAttributes,
                    int                             numThreads,
                    int                             maxNumPendingJobs,
                    bslma::Allocator               *basicAllocator = 0);
        // Construct a thread pool with the specified 'threadAttributes',
        // 'numThreads' number of threads, and a job queue with capacity
        // sufficient to enqueue the specified 'maxNumPendingJobs' without
        // blocking.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // '1 <= numThreads'.

    ~FixedThreadPool();
        // Remove all pending jobs from the queue without executing them, block
        // until all currently running jobs complete, and then destroy this
        // thread pool.

    // MANIPULATORS
    void disable();
        // Disable enqueueing into this pool.  All subsequent invocations of
        // 'enqueueJob' or 'tryEnqueueJob' will fail immediately.  All blocked
        // invocations of 'enqueueJob' will fail immediately.  If the pool is
        // already enqueue disabled, this method has no effect.  Note that this
        // method has no effect on jobs currently in the pool.

    void enable();
        // Enable queuing into this pool.  If the queue is not enqueue
        // disabled, this call has no effect.

    int enqueueJob(const Job& functor);
    int enqueueJob(bslmf::MovableRef<Job> functor);
        // Enqueue the specified 'functor' to be executed by the next available
        // thread.  Return 0 on success, and a non-zero value otherwise.
        // Specifically, return 'e_SUCCESS' on success, 'e_DISABLED' if
        // '!isEnabled()', and 'e_FAILED' if an error occurs.  This operation
        // will block if there is not sufficient capacity in the underlying
        // queue until there is free capacity to successfully enqueue this job.
        // Threads blocked (on enqueue methods) due to the underlying queue
        // being full will unblock and return 'e_DISABLED' if 'disable' is
        // invoked (on another thread).  The behavior is undefined unless
        // 'functor' is not null.

    int enqueueJob(FixedThreadPoolJobFunc function, void *userData);
        // Enqueue the specified 'function' to be executed by the next
        // available thread.  The specified 'userData' pointer will be passed
        // to the function by the processing thread.  Return 0 on success, and
        // a non-zero value otherwise.  Specifically, return 'e_SUCCESS' on
        // success, 'e_DISABLED' if '!isEnabled()', and 'e_FAILED' if an error
        // occurs.  This operation will block if there is not sufficient
        // capacity in the underlying queue until there is free capacity to
        // successfully enqueue this job.  Threads blocked (on enqueue methods)
        // due to the underlying queue being full will unblock and return
        // 'e_DISABLED' if 'disable' is invoked (on another thread).  The
        // behavior is undefined unless 'function' is not null.

    int tryEnqueueJob(const Job& functor);
    int tryEnqueueJob(bslmf::MovableRef<Job> functor);
        // Enqueue the specified 'functor' to be executed by the next available
        // thread.  Return 0 on success, and a non-zero value otherwise.
        // Specifically, return 'e_SUCCESS' on success, 'e_DISABLED' if
        // '!isEnabled()', 'e_FULL' if 'isEnabled()' and the underlying queue
        // was full, and 'e_FAILED' if an error occurs.  The behavior is
        // undefined unless 'functor' is not null.

    int tryEnqueueJob(FixedThreadPoolJobFunc function, void *userData);
        // Enqueue the specified 'function' to be executed by the next
        // available thread.  The specified 'userData' pointer will be passed
        // to the function by the processing thread.  Return 0 on success, and
        // a non-zero value otherwise.  Specifically, return 'e_SUCCESS' on
        // success, 'e_DISABLED' if '!isEnabled()', 'e_FULL' if 'isEnabled()'
        // and the underlying queue was full, and 'e_FAILED' if an error
        // occurs.  The behavior is undefined unless 'function' is not null.

    void drain();
        // Wait until the underlying queue is empty without disabling this pool
        // (and may thus wait indefinitely), and then wait until all executing
        // jobs complete.  If the thread pool was not already started
        // ('isStarted()' is 'false'), this method has no effect.  Note that if
        // any jobs are submitted concurrently with this method, this method
        // may or may not wait until they have also completed.

    void shutdown();
        // Disable enqueuing jobs on this thread pool, cancel all pending jobs,
        // wait until all active jobs complete, and join all processing
        // threads.  If the thread pool was not already started ('isStarted()'
        // is 'false'), this method has no effect.  At the completion of this
        // method, 'false == isStarted()'.

    int start();
        // Spawn threads until there are 'numThreads()' processing threads.  On
        // success, enable enqueuing and return 0.  Otherwise, join all threads
        // (ensuring 'false == isStarted()') and return -1.  If the thread pool
        // was already started ('isStarted()' is 'true'), this method has no
        // effect.

    void stop();
        // Disable enqueuing jobs on this thread pool, wait until all active
        // and pending jobs complete, and join all processing threads.  If the
        // thread pool was not already started ('isStarted()' is 'false'), this
        // method has no effect.  At the completion of this method,
        // 'false == isStarted()'.

    // ACCESSORS
    bool isEnabled() const;
        // Return 'true' if enqueuing jobs is enabled on this thread pool, and
        // 'false' otherwise.

    bool isStarted() const;
        // Return 'true' if 'numThreads()' are started on this threadpool and
        // 'false' otherwise (indicating that 0 threads are started on this
        // thread pool.)

    int numActiveThreads() const;
        // Return a snapshot of the number of threads that are currently
        // processing a job for this threadpool.

    int numPendingJobs() const;
        // Return a snapshot of the number of jobs currently enqueued to be
        // processed by thread pool.

    int numThreads() const;
        // Return the number of threads passed to this thread pool at
        // construction.

    int numThreadsStarted() const;
        // Return a snapshot of the number of threads currently started by this
        // thread pool.

    int queueCapacity() const;
        // Return the capacity of the queue used to enqueue jobs by this thread
        // pool.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                          // ---------------------
                          // class FixedThreadPool
                          // ---------------------

// MANIPULATORS
inline
void FixedThreadPool::disable()
{
    d_queue.disablePushBack();
}

inline
void FixedThreadPool::enable()
{
    d_queue.enablePushBack();
}

inline
int FixedThreadPool::enqueueJob(const Job& functor)
{
    BSLS_ASSERT(functor);

    return d_queue.pushBack(functor);
}

inline
int FixedThreadPool::enqueueJob(bslmf::MovableRef<Job> functor)
{
    BSLS_ASSERT(bslmf::MovableRefUtil::access(functor));

    return d_queue.pushBack(bslmf::MovableRefUtil::move(functor));
}

inline
int FixedThreadPool::enqueueJob(FixedThreadPoolJobFunc  function,
                                void                   *userData)
{
    BSLS_ASSERT(0 != function);

    return enqueueJob(bdlf::BindUtil::bindR<void>(function, userData));
}

inline
int FixedThreadPool::tryEnqueueJob(const Job& functor)
{
    BSLS_ASSERT(functor);

    return d_queue.tryPushBack(functor);
}

inline
int FixedThreadPool::tryEnqueueJob(bslmf::MovableRef<Job> functor)
{
    BSLS_ASSERT(bslmf::MovableRefUtil::access(functor));

    return d_queue.tryPushBack(bslmf::MovableRefUtil::move(functor));
}

inline
int FixedThreadPool::tryEnqueueJob(FixedThreadPoolJobFunc  function,
                                   void                   *userData)
{
    BSLS_ASSERT(0 != function);

    return tryEnqueueJob(bdlf::BindUtil::bindR<void>(function, userData));
}

inline
void FixedThreadPool::drain()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_metaMutex);

    if (isStarted()) {
        d_queue.waitUntilEmpty();

        d_drainFlag = true;
        d_queue.disablePopFront();
        d_barrier.wait();

        d_drainFlag = false;
        d_queue.enablePopFront();
        d_barrier.wait();
    }
}

inline
void FixedThreadPool::shutdown()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_metaMutex);

    if (isStarted()) {
        d_queue.disablePushBack();
        d_queue.disablePopFront();
        d_threadGroup.joinAll();
        d_queue.removeAll();
    }
}

inline
void FixedThreadPool::stop()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_metaMutex);

    if (isStarted()) {
        d_queue.disablePushBack();
        d_queue.waitUntilEmpty();
        d_queue.disablePopFront();
        d_threadGroup.joinAll();
    }
}

// ACCESSORS
inline
bool FixedThreadPool::isEnabled() const
{
    return !d_queue.isPushBackDisabled();
}

inline
bool FixedThreadPool::isStarted() const
{
    return d_numThreads == d_threadGroup.numThreads();
}

inline
int FixedThreadPool::numActiveThreads() const
{
    return d_numActiveThreads.loadAcquire();
}

inline
int FixedThreadPool::numPendingJobs() const
{
    return static_cast<int>(d_queue.numElements());
}

inline
int FixedThreadPool::numThreads() const
{
    return d_numThreads;
}

inline
int FixedThreadPool::numThreadsStarted() const
{
    return d_threadGroup.numThreads();
}

inline
int FixedThreadPool::queueCapacity() const
{
    return static_cast<int>(d_queue.capacity());
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
