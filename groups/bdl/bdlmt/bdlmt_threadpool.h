// bdlmt_threadpool.h                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLMT_THREADPOOL
#define INCLUDED_BDLMT_THREADPOOL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide portable implementation for a dynamic pool of threads.
//
//@CLASSES:
//   bdlmt::ThreadPool: portable dynamic thread pool
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a portable and efficient implementation
// of a thread pool that can be used to distribute various user-defined
// functions ("jobs") to separate threads and execute the jobs concurrently.
// The thread pool manages a dynamic set of processing threads, adding or
// removing threads to manage load, based upon user-defined parameters.
//
// The pool uses a queue mechanism to distribute work among the threads.  Jobs
// are queued for execution as they arrive, and each queued job is processed by
// the next available thread.  If no threads are available, new threads are
// created dynamically (up to the application defined maximum number).  If the
// maximum number of concurrent threads has been reached, new jobs will remain
// enqueued until a thread becomes available.  If the threads become idle for
// longer than a user-defined maximum idle time, they are automatically
// destroyed, releasing unused resources.  A client-defined minimum number of
// threads is always maintained even when there is no work to be done.
//
// The thread pool provides two interfaces for specifying jobs: the commonly
// used "void function/void pointer" interface and the more versatile functor
// based interface.  The void function/void pointer interface allows callers to
// use a C-style function to be executed as a job.  The application need only
// specify the address of the function, and a single void pointer argument, to
// be passed to the function.  The specified function will be invoked with the
// specified argument by the processing thread.  The functor based interface
// allows for flexible job execution such as the invocation of member functions
// or the passing of multiple user-defined arguments.  See the 'bdef' package
// documentation for more on functors and their usage.
//
// An application can tune the thread pool by adjusting the minimum and maximum
// number of threads in the pool, and the maximum amount of time that
// dynamically created threads can idle before being destroyed.  To avoid
// unnecessary and inefficient thread creation/destruction, an application
// should select a value for the minimum number of threads that reflects the
// expected average load.  A higher value for the maximum number of threads can
// be used to handle periodic bursts.  An application can also specify the
// attributes of the threads in the pool (e.g., thread priority or stack size),
// by providing a 'bslmt::ThreadAttributes' object with the desired values set.
// See 'bslmt_threadutil' package documentation for a description of
// 'bslmt::ThreadAttributes'.
//
// Thread pools are ideal for developing multi-threaded server applications.  A
// server need only package client requests to execute as jobs, and
// 'bdlmt::ThreadPool' will handle the queue management, thread management, and
// request dispatching.  Thread pools are also well suited for parallelizing
// certain types of application logic.  Without any complex or redundant thread
// management code, an application can easily create a thread pool, enqueue a
// series of jobs to be executed, and wait until all the jobs have executed.
//
///Thread Safety
///-------------
// The 'bdlmt::ThreadPool' class is both *fully thread-safe* (i.e., all
// non-creator methods can correctly execute concurrently), and is
// *thread-enabled* (i.e., the class does not function correctly in a
// non-multi-threading environment).  See 'bsldoc_glossary' for complete
// definitions of *fully thread-safe* and *thread-enabled*.
//
///Synchronous Signals on Unix
///---------------------------
// A thread pool ensures that, on unix platforms, all the threads in the pool
// block all asynchronous signals.  Specifically all the signals, except the
// following synchronous signals are blocked.
//
// SIGBUS
// SIGFPE
// SIGILL
// SIGSEGV
// SIGSYS
// SIGABRT
// SIGTRAP
// SIGIOT
//
///Usage
///-----
// This example demonstrates the use of a 'bdlmt::ThreadPool' to parallelize a
// segment of program logic.  The example implements a multi-threaded file
// search utility.  The utility searches multiple files for a string, similar
// to the Unix command 'fgrep'; the use of a 'bdlmt::ThreadPool' allows the
// utility to search multiple files concurrently.
//
// The example program will take as input a string and a list of files to
// search.  The program creates a 'bdlmt::ThreadPool', and then enqueues a
// single "job" for each file to be searched.  Each thread in the pool will
// take a job from the queue, open the file, and search for the string.  If a
// match is found, the job adds the filename to an array of matching filenames.
// Because this array of filenames is shared across multiple jobs and across
// multiple threads, access to the array is controlled via a 'bslmt::Mutex'.
//
///Setting ThreadPool Attributes
///- - - - - - - - - - - - - - -
// To get started, we declare thread attributes, to be used in constructing the
// thread pool.  In this example, our choices for minimum search threads and
// maximum idle time are arbitrary; we don't expect the thread pool to become
// idle, so the thread pool should not begin to delete unused threads before
// the program terminates.
//
// However, a maximum number of 50 threads is meaningful, and may affect
// overall performance.  The maximum should cover the expected peak, in this
// case, the maximum number of files to search.  However, if the maximum is too
// large for a given platform, it may cause a bottleneck as the operating
// system spends significant resources switching context among multiple
// threads.  Also we use a very short idle time since new jobs will arrive only
// at startup.
//..
//  const int                MIN_SEARCH_THREADS = 10;
//  const int                MAX_SEARCH_THREADS = 50;
//  const bsls::TimeInterval MAX_SEARCH_THREAD_IDLE(0, 100000000);
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
// The single 'void *' argument is received and cast to point to a 'struct
// my_FastSearchJobInfo', which then points to the search string and a single
// file to be searched.  Note that different 'my_FastSearchJobInfo' structures
// for the same search request will differ only in the attribute 'd_path',
// which points to a specific filename among the set of files to be searched;
// other fields will be identical across all structures for a given Fast
// Search.
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
//           size_t wordLen = job->d_word->length();
//           const char *word = job->d_word->c_str();
//
//           nread = fread(buffer, 1, sizeof(buffer) - 1, file);
//           while(nread >= wordLen) {
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
//       bslmt::Mutex            mutex;
//       bslmt::ThreadAttributes defaultAttributes;
//..
// We initialize the thread pool using default thread attributes.  We then
// start the pool so that the threads can begin while we prepare the jobs.
//..
//       bdlmt::ThreadPool       pool(defaultAttributes,
//                                    MIN_SEARCH_THREADS,
//                                    MAX_SEARCH_THREADS,
//                                    MAX_SEARCH_THREAD_IDLE);
//
//       if (0 != pool.start()) {
//           bsl::cerr << "Failed to start minimum number of threads.\n";
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
// uses the 'bsl::function' component to provide type-safe wrappers that can
// match argument number and type for a C++ free function or member function.
//
// To illustrate the Functor Interface, we will make two small changes to the
// usage example above.  First, we change the signature of the function that
// executes a single job, so that it uses a 'my_FastSearchJobInfo' pointer
// rather than a 'void' pointer.  With this change, we can remove the first
// executable statement, which casts the 'void *' pointer to
// 'my_FastSearchJobInfo *'.
//..
//  static void my_FastFunctorSearchJob(my_FastSearchJobInfo *job)
//  {
//      FILE *file;
//
//      file = fopen(job->d_path->c_str(), "r");
//
//      // The rest of the function is unchanged.
//      if (file) {
//          char  buffer[1024];
//          size_t nread;
//          size_t wordLen = job->d_word->length();
//          const char *word = job->d_word->c_str();
//
//          nread = fread(buffer, 1, sizeof(buffer) - 1, file);
//          while(nread >= wordLen) {
//              buffer[nread] = 0;
//              if (strstr(buffer, word)) {
//                  bslmt::LockGuard<bslmt::Mutex> lock(job->d_mutex);
//                  job->d_outList->push_back(*job->d_path);
//                  break;  // bslmt::LockGuard destructor unlocks mutex.
//              }
//          }
//          bsl::memcpy(buffer, &buffer[nread - wordLen - 1], wordLen - 1);
//          nread = fread(buffer + wordLen - 1, 1, sizeof(buffer) - wordLen,
//                        file);
//      }
//      fclose(file);
//  }
//..
// Next, we make a change to the loop that enqueues the jobs in 'myFastSearch'.
// The function starts exactly as in the previous example:
//..
//  static void myFastFunctorSearch(const string&         word,
//                                  const vector<string>& fileList,
//                                  vector<string>&       outFileList)
//  {
//      bslmt::Mutex            mutex;
//      bslmt::ThreadAttributes defaultAttributes;
//      bdlmt::ThreadPool       pool(defaultAttributes,
//                                   MIN_SEARCH_THREADS,
//                                   MAX_SEARCH_THREADS,
//                                   MAX_SEARCH_THREAD_IDLE);
//
//      if (0 != pool.start()) {
//          bsl::cerr << "Failed to start minimum number of threads.  "
//                    << "Thread quota exceeded?\n";
//          assert(false);
//          return; // things are SNAFU
//      }
//
//      int count = fileList.size();
//      my_FastSearchJobInfo  *jobInfoArray = new my_FastSearchJobInfo[count];
//..
// We create a functor - a C++ object that acts as a function.  The thread pool
// will "execute" this functor (by calling its 'operator()' member function) on
// a thread when one becomes available.
//..
//      for (int i = 0; i < count; ++i) {
//          my_FastSearchJobInfo &job = jobInfoArray[i];
//          job.d_word    = &word;
//          job.d_path    = &fileList[i];
//          job.d_mutex   = &mutex;
//          job.d_outList = &outFileList;
//
//          bsl::function<void()> jobHandle =
//                        bdlf::BindUtil::bind(&my_FastFunctorSearchJob, &job);
//          pool.enqueueJob(jobHandle);
//      }
//..
// Note that the functor is created locally and handed to the thread pool.  The
// thread pool copies the functor onto its internal queue, and takes
// responsibility for the copied functor until execution is complete.
//
// The function is completed exactly as it was in the previous example.
//..
//      pool.drain();
//      delete[] jobInfoArray;
//  }
//..

#include <bdlf_bind.h>

#include <bdlscm_version.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_functionpointertraits.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_threadattributes.h>
#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>

#include <bsls_atomic.h>
#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>  // BSLS_PLATFORM_OS_UNIX
#include <bsls_timeinterval.h>

#include <bsl_deque.h>
#if defined(BSLS_PLATFORM_OS_UNIX)
    #include <bsl_csignal.h>              // sigfillset
#endif
#include <bsl_functional.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslalg_typetraits.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

extern "C" typedef void (*bcep_ThreadPoolJobFunc)(void *);
        // This type declares the prototype for functions that are suitable to
        // be specified 'bdlmt::FixedThreadPool::enqueueJob'.

#endif // BDE_OMIT_INTERNAL_DEPRECATED

namespace bdlmt {

struct ThreadPoolWaitNode;

extern "C" void *ThreadPoolEntry(void *);
    // Entry point for processing threads.

extern "C" typedef void (*ThreadPoolJobFunc)(void *);
    // This type declares the prototype for functions that are suitable to be
    // specified 'bdlmt::FixedThreadPool::enqueueJob'.

                              // ================
                              // class ThreadPool
                              // ================

class ThreadPool {
    // This class implements a thread pool used for concurrently executing
    // multiple user-defined functions ("jobs").

  public:
    // TYPES
    typedef bsl::function<void()> Job;

  private:
    // PRIVATE DATA
    bsl::deque<Job>      d_queue;          // queue of pending jobs

    mutable bslmt::Mutex d_mutex;          // mutex used to control access to
                                           // this thread pool

    bslmt::Condition     d_drainCond;      // condition variable used to signal
                                           // that the queue is fully drained
                                           // and that all active jobs have
                                           // completed

    bslmt::ThreadAttributes
                         d_threadAttributes;
                                           // thread attributes to be used when
                                           // constructing processing threads

    const int            d_maxThreads;     // maximum number of processing
                                           // threads that can be started at
                                           // any given time by this thread
                                           // pool

    const int            d_minThreads;     // minimum number of processing
                                           // threads that must running at any
                                           // given time

    int                  d_threadCount;    // current number of processing
                                           // threads started by this thread
                                           // pool

    bsls::AtomicInt      d_createFailures; // number of thread create failures


    bsls::TimeInterval   d_maxIdleTime;    // time that threads (in excess of
                                           // the minimum number of threads)
                                           // remain idle before being shut
                                           // down

    int                  d_numActiveThreads;
                                           // current number of threads that
                                           // are actively processing a job

    bsls::AtomicInt      d_enabled;        // indicates the enabled state of
                                           // queue; queuing is disabled when
                                           // 0, enabled otherwise

    bsls::AtomicPointer<ThreadPoolWaitNode>
                         d_waitHead;       // pointer to the 'WaitNode' control
                                           // structure of the first thread
                                           // that is waiting for a request

    bsls::AtomicInt64    d_lastResetTime;  // last reset time of percent-busy
                                           // metric in nanoseconds from some
                                           // arbitrary but fixed point in time

    bsls::AtomicInt64    d_callbackTime;   // the total time spent running jobs
                                           // (callbacks) across all threads,
                                           // in nanoseconds

#if defined(BSLS_PLATFORM_OS_UNIX)
    sigset_t             d_blockSet;       // set of signals to be blocked in
                                           // managed threads
#endif

    // CLASS DATA
    static const char    s_defaultThreadName[16];   // default name of threads
                                                    // if supported and
                                                    // attributes doesn't
                                                    // specify another name

    // FRIENDS
    friend void* ThreadPoolEntry(void *);

    // PRIVATE MANIPULATORS
    void doEnqueueJob(const Job& job);
    void doEnqueueJob(bslmf::MovableRef<Job> job);
        // Internal method used to push the specified 'job' onto 'd_queue' and
        // signal the next waiting thread if any.  Note that this method must
        // be called with 'd_mutex' locked.

    void wakeThreadIfNeeded();
        // Signal this thread and pop the current thread from the wait list.

    int startThreadIfNeeded();
        // Start a new thread if needed and the maximum number of threads are
        // not yet running.  This method must be called with 'd_mutex' locked.
        // Return 0 if at least one thread is running, and a non-zero value
        // otherwise.

#if defined(BSLS_PLATFORM_OS_UNIX)
    void initBlockSet();
        // Initialize the set of signals to be blocked in the managed threads.
#endif

    int startNewThread();
        // Internal method to spawn a new processing thread and increment the
        // current count.  This method must be called with 'd_mutex' locked.

    void workerThread();
        // Processing thread function.

  private:
    // NOT IMPLEMENTED
    ThreadPool(const ThreadPool&);
    ThreadPool& operator=(const ThreadPool&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ThreadPool, bslma::UsesBslmaAllocator);

    // CREATORS
    ThreadPool(const bslmt::ThreadAttributes&  threadAttributes,
               int                             minThreads,
               int                             maxThreads,
               int                             maxIdleTime,
               bslma::Allocator               *basicAllocator = 0);
        // Construct a thread pool with the specified 'threadAttributes', the
        // specified 'minThreads' minimum number of threads, the specified
        // 'maxThreads' maximum number of threads, and the specified
        // 'maxIdleTime' idle time (in milliseconds) after which a thread may
        // be considered for destruction.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 <= minThreads', 'minThreads <= maxThreads', and
        // '0 <= maxIdleTime'.

    ThreadPool(const bslmt::ThreadAttributes&  threadAttributes,
               int                             minThreads,
               int                             maxThreads,
               bsls::TimeInterval              maxIdleTime,
               bslma::Allocator               *basicAllocator = 0);
        // Construct a thread pool with the specified 'threadAttributes', the
        // specified 'minThreads' minimum number of threads, the specified
        // 'maxThreads' maximum number of threads, and the specified
        // 'maxIdleTime' idle time after which a thread may be considered for
        // destruction.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // '0 <= minThreads', 'minThreads <= maxThreads', '0 <= maxIdleTime',
        // and the 'maxIdleTime' has a value less than or equal to 'INT_MAX'
        // milliseconds.

    ~ThreadPool();
        // Call 'shutdown()' and destroy this thread pool.

    // MANIPULATORS
    void drain();
        // Disable queuing on this thread pool and wait until all pending jobs
        // complete.  Use 'start' to re-enable queuing.

    int enqueueJob(const Job& functor);
    int enqueueJob(bslmf::MovableRef<Job> functor);
        // Enqueue the specified 'functor' to be executed by the next available
        // thread.  Return 0 if enqueued successfully, and a non-zero value if
        // queuing is currently disabled.  The behavior is undefined unless
        // 'functor' is not "unset".  See 'bsl::function' for more information
        // on functors.

    int enqueueJob(ThreadPoolJobFunc function, void *userData);
        // Enqueue the specified 'function' to be executed by the next
        // available thread.  The specified 'userData' pointer will be passed
        // to the function by the processing thread.  Return 0 if enqueued
        // successfully, and a non-zero value if queuing is currently disabled.

    double resetPercentBusy();
        // Atomically report the percentage of wall time spent by each thread
        // of this thread pool executing jobs since the last reset time, and
        // set the reset time to now.  The creation of the thread pool is
        // considered a first reset time.  This value is calculated as
        //..
        //           sum(jobExecutionTime)       100%
        //  P_busy = --------------------   x ----------
        //            timeSinceLastReset      maxThreads
        //..
        // Note that this percentage reflects the wall time spent per thread,
        // and not CPU time per thread, or not even CPU time per processor.
        // Also note that there is no guarantee that all threads are processed
        // concurrently (e.g., the number of threads could be larger than the
        // number of processors).

    void shutdown();
        // Disable queuing on this thread pool, cancel all queued jobs, and
        // shut down all processing threads (after all active jobs complete).

    int start();
        // Enable queuing on this thread pool and spawn 'minThreads()'
        // processing threads.  Return 0 on success, and a non-zero value
        // otherwise.  If 'minThreads()' threads were not successfully started,
        // all threads are stopped.

    void stop();
        // Disable queuing on this thread pool and wait until all pending jobs
        // complete, then shut down all processing threads.

    // ACCESSORS
    int enabled() const;
        // Return the state (enabled or not) of the thread pool.

    int maxThreads() const;
        // Return the maximum number of threads that are allowed to be running
        // at given time.

    int maxIdleTime() const;
        // Return the amount of time (in milliseconds) a thread remains idle
        // before being shut down when there are more than min threads started.

    bsls::TimeInterval maxIdleTimeInterval() const;
        // Return the amount of time a thread remains idle before being shut
        // down when there are more than min threads started.

    int minThreads() const;
        // Return the minimum number of threads that must be started at any
        // given time.

    int numActiveThreads() const;
        // Return the number of threads that are currently processing a job.

    int numPendingJobs() const;
        // Return the number of jobs that are currently queued, but not yet
        // being processed.

    int numWaitingThreads() const;
        // Return the number of threads that are currently waiting for a job.

    double percentBusy() const;
        // Return the percentage of wall time spent by each thread of this
        // thread pool executing jobs since the last reset time.  The creation
        // of the thread pool is considered a first reset time.  This value is
        // calculated as
        //..
        //           sum(jobExecutionTime)       100%
        //  P_busy = --------------------   x ----------
        //            timeSinceLastReset      maxThreads
        //..
        // Note that this percentage reflects the wall time spent per thread,
        // and not CPU time per thread, or not even CPU time per processor.
        // Also note that there is no guarantee that all threads are processed
        // concurrently (e.g., the number of threads could be larger than the
        // number of processors).

    int threadFailures() const;
        // Return the number of times that thread creation failed.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// MANIPULATORS

inline
int ThreadPool::enqueueJob(ThreadPoolJobFunc function, void *userData)
{
    return enqueueJob(bdlf::BindUtil::bindR<void>(function, userData));
}

// ACCESSORS

inline
int ThreadPool::enabled() const
{
    return d_enabled;
}

inline
int ThreadPool::minThreads() const
{
    return d_minThreads;
}

inline
int ThreadPool::maxThreads() const
{
    return d_maxThreads;
}

inline
int ThreadPool::threadFailures() const
{
    return d_createFailures;
}

inline
int ThreadPool::maxIdleTime() const
{
    return static_cast<int>(d_maxIdleTime.totalMilliseconds());
}

inline
bsls::TimeInterval ThreadPool::maxIdleTimeInterval() const
{
    return d_maxIdleTime;
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
