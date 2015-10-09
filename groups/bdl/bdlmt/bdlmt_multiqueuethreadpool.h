// bdlmt_multiqueuethreadpool.h                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLMT_MULTIQUEUETHREADPOOL
#define INCLUDED_BDLMT_MULTIQUEUETHREADPOOL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a pool of queues, each processed serially by a thread pool.
//
//@CLASSES:
// bdlmt::MultiQueueThreadPool: multi-threaded, serial processing of queues
//
//@SEE_ALSO: bdlmt_threadpool
//
//@DESCRIPTION: This component defines a dynamic, configurable pool of queues,
// each of which is processed by a thread in a thread pool, such that elements
// on a given queue are processed serially, regardless of which thread is
// processing the queue at a given time.
//
// A 'bdlmt::MultiQueueThreadPool' allows clients to create and delete queues,
// and to enqueue "jobs" (represented as client-specified functors) to specific
// queues.  Queue processing is implemented on top of a 'bdlmt::ThreadPool' by
// enqueuing a per-queue functor to the thread pool.  Each functor dequeues
// the next item from its associated queue, processes it, and re-enqueues
// itself to the thread pool.  Since there is only one representative functor
// per queue, each queue is guaranteed to be processed serially by the thread
// pool.
//
// In addition to the ability to create and delete queues, clients are able to
// tune the underlying thread pool in accordance with the 'bdlmt::ThreadPool'
// documentation.
//
///Thread Safety
///-------------
// The 'bdlmt::MultiQueueThreadPool' class is *thread-aware*, but not *fully
// thread-safe*.  The function-level documentation identifies methods (i.e.,
// the 'reset' method) that cannot be safely used by multiple threads.  This
// class is also *thread-enabled* (i.e., the class does not function correctly
// in a non-multi-threading environment).  See 'bsldoc_glossary' for complete
// definitions of *thread-aware*, *fully thread-safe*, and *thread-enabled*.
//
//
//          // ARB: This usage example is entirely broken, because it relies on
//          // absolute paths that no longer exist.  The example should be
//          // rephrased to work on an array of strings instead of a file, or
//          // perhaps re-written entirely.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Word Search Application
/// - - - - - - - - - - - - - - - - - -
// This example illustrates the use of a 'bdlmt::MultiQueueThreadPool' in a
// word search application called 'fastSearch'.  'fastSearch' searches a list
// of files for a list of words, and returns the set of files which contain
// all of the specified words.  'bdlmt::MultiQueueThreadPool' is used to
// provide concurrent processing of files, and to simplify the collection of
// results by serializing access to result sets which are maintained for each
// word.
//
// First, we present a class used to manage a word, and the set of files which
// contain that word:
//..
//  class my_SearchProfile {
//      // This class defines a search profile consisting of a word
//      // and a set of files (given by name) that contain the word.
//      // Here, "word" is defined as any string of characters.
//
//      bsl::string           d_word;     // word to search for
//      bsl::set<bsl::string> d_fileSet;  // set of matching files
//
//    private:
//      // not implemented
//      my_SearchProfile(const my_SearchProfile&);
//      my_SearchProfile& operator=(const my_SearchProfile&);
//
//    public:
//      // CREATORS
//      my_SearchProfile(const char       *word,
//                       bslma::Allocator *basicAllocator = 0);
//          // Create a 'my_SearchProfile' with the specified 'word'.
//          // Optionally specify a 'basicAllocator' used to supply memory.
//          // If 'basicAllocator' is 0, the default memory allocator is used.
//
//      ~my_SearchProfile();
//          // Destroy this search profile.
//
//      // MANIPULATORS
//      void insert(const char *file);
//          // Insert the specified 'file' into the file set maintained
//          // by this search profile.
//
//      // ACCESSORS
//      bool isMatch(const char *file) const;
//          // Return 'true' if the specified 'file' matches this search
//          // profile.
//
//      const bsl::set<bsl::string>& fileSet() const;
//          // Return a reference to the non-modifiable file set maintained
//          // by this search profile.
//
//      const bsl::string& word() const;
//          // Return a reference to the non-modifiable word maintained
//          // by this search profile.
//  };
//..
// And the implementation:
//..
//  // CONSTRUCTORS
//  my_SearchProfile::my_SearchProfile(
//          const char       *word,
//          bslma::Allocator *basicAllocator)
//  : d_word(basicAllocator)
//  , d_fileSet(bsl::less<bsl::string>(), basicAllocator)
//  {
//      assert(word);
//
//      d_word.assign(word);
//  }
//
//  my_SearchProfile::~my_SearchProfile()
//  {
//  }
//
//  // MANIPULATORS
//  inline
//  void my_SearchProfile::insert(const char *file)
//  {
//      assert(file);
//
//      d_fileSet.insert(file);
//  }
//
//  // ACCESSORS
//  bool my_SearchProfile::isMatch(const char *file) const
//  {
//      assert(file);
//
//      bool          found = false;
//      bsl::ifstream ifs(file);
//      bsl::string   line;
//      while (bsl::getline(ifs, line)) {
//          if (bsl::string::npos != line.find(d_word)) {
//              found = true;
//              break;
//          }
//      }
//      ifs.close();
//      return found;
//  }
//
//  inline
//  const bsl::set<bsl::string>& my_SearchProfile::fileSet() const
//  {
//      return d_fileSet;
//  }
//
//  inline
//  const bsl::string& my_SearchProfile::word() const
//  {
//      return d_word;
//  }
//..
// Next, we define a helper function to perform a search of a word in a
// particular file.  The function is parameterized by a search profile and a
// file name.  If the specified file name matches the profile, it is inserted
// into the profile's file list.
//..
//  void my_SearchCb(my_SearchProfile* profile, const char *file)
//  {
//      // Insert the specified 'file' to the file set of the specified
//      // search 'profile' if 'file' matches the 'profile'.
//
//      assert(profile);
//      assert(file);
//
//      if (profile->isMatch(file)) {
//          profile->insert(file);
//      }
//  }
//..
// Lastly, we present the front end to the search application: 'fastSearch'.
// 'fastSearch' is parameterized by a list of words to search for, a list of
// files to search in, and a set which is populated with the search results.
// 'fastSearch' instantiates a 'bdlmt::MultiQueueThreadPool', and creates a
// queue for each word.  It then associates each queue with a search profile
// based on a word in the word list.  Then, it enqueues a job to each queue for
// each file in the file list that tries to match the file to each search
// profile.  Lastly, 'fastSearch' collects the results, which is the set
// intersection of each file set maintained by the individual search profiles.
//..
//  void fastSearch(const bsl::vector<bsl::string>&  wordList,
//                  const bsl::vector<bsl::string>&  fileList,
//                  bsl::set<bsl::string>&           resultSet,
//                  bslma::Allocator                *basicAllocator = 0)
//  {
//      // Return the set of files, specified by 'fileList', containing
//      // every word in the specified 'wordList', in the specified
//      // 'resultSet'.
//
//      typedef bsl::vector<bsl::string> ListType;
//          // This type is defined for notational convenience when iterating
//          // over 'wordList' or 'fileList'.
//
//      typedef bsl::pair<int, my_SearchProfile*> RegistryValue;
//          // This type is defined for notational convenience.  The first
//          // parameter specifies a queue ID.  The second parameter specifies
//          // an associated search profile.
//
//      typedef bsl::map<bsl::string, RegistryValue> RegistryType;
//          // This type is defined for notational convenience.  The first
//          // parameter specifies a word.  The second parameter specifies a
//          // tuple containing a queue ID, and an associated search profile
//          // containing the specified word.
//
//      enum {
//          // thread pool configuration
//          MIN_THREADS = 4,
//          MAX_THREADS = 20,
//          MAX_IDLE    = 100  // use a very short idle time since new jobs
//                             // arrive only at startup
//      };
//      bslmt::ThreadAttributes           defaultAttrs;
//      bdlmt::MultiQueueThreadPool pool(defaultAttrs,
//                                     MIN_THREADS, MAX_THREADS, MAX_IDLE,
//                                     basicAllocator);
//      RegistryType profileRegistry(bsl::less<bsl::string>(), basicAllocator);
//
//      // Start the pool, enabling queue creation and processing.
//      pool.start();
//
//      // Create a queue and a search profile associated with each word in
//      // 'wordList'.
//
//      for (ListType::const_iterator it = wordList.begin();
//           it != wordList.end(); ++it)
//      {
//          bslma::Allocator *allocator =
//                                   bslma::Default::allocator(basicAllocator);
//
//          const bsl::string& word = *it;
//          int                id = pool.createQueue();
//          LOOP_ASSERT(word, 0 != id);
//          my_SearchProfile *profile = new (*allocator)
//                                               my_SearchProfile(word.c_str(),
//                                                                allocator);
//
//          bslma::RawDeleterProctor<my_SearchProfile, bslma::Allocator>
//                                                 deleter(profile, allocator);
//
//          profileRegistry[word] = bsl::make_pair(id, profile);
//          deleter.release();
//      }
//
//      // Enqueue a job which tries to match each file in 'fileList' with each
//      // search profile.
//
//      for (ListType::const_iterator it = fileList.begin();
//           it != fileList.end(); ++it)
//      {
//          for (ListType::const_iterator jt = wordList.begin();
//               jt != wordList.end(); ++jt)
//          {
//              const bsl::string&        file = *it;
//              const bsl::string&        word = *jt;
//              RegistryValue&            rv   = profileRegistry[word];
//              bsl::function<void()> job  =
//                 bdlf::BindUtil::bind(&my_SearchCb, rv.second, file.c_str());
//              int rc = pool.enqueueJob(rv.first, job);
//              LOOP_ASSERT(word, 0 == rc);
//          }
//      }
//
//      // Stop the pool, and wait while enqueued jobs are processed.
//      pool.stop();
//
//      // Construct the 'resultSet' as the intersection of file sets
//      // collected in each search profile.
//
//      resultSet.insert(fileList.begin(), fileList.end());
//      for (RegistryType::iterator it = profileRegistry.begin();
//           it != profileRegistry.end(); ++it)
//      {
//          my_SearchProfile *profile = it->second.second;
//          const bsl::set<bsl::string>& fileSet = profile->fileSet();
//          bsl::set<bsl::string> tmpSet;
//          bsl::set_intersection(fileSet.begin(),   fileSet.end(),
//                                resultSet.begin(), resultSet.end(),
//                                bsl::inserter(tmpSet, tmpSet.begin()));
//          resultSet = tmpSet;
//          bslma::Default::allocator(basicAllocator)->deleteObjectRaw(
//                                                                    profile);
//      }
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMT_THREADPOOL
#include <bdlmt_threadpool.h>
#endif

#ifndef INCLUDED_BDLCC_OBJECTCATALOG
#include <bdlcc_objectcatalog.h>
#endif

#ifndef INCLUDED_BDLCC_OBJECTPOOL
#include <bdlcc_objectpool.h>
#endif

#ifndef INCLUDED_BSLMT_RWMUTEX
#include <bslmt_rwmutex.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BSLS_SPINLOCK
#include <bsls_spinlock.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_DEQUE
#include <bsl_deque.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

namespace BloombergLP {

namespace bslmt { class Barrier; }

namespace bdlmt {
                      // ================================
                      // class MultiQueueThreadPool_Queue
                      // ================================

class MultiQueueThreadPool_Queue {
    // This private class provides a lightweight queue, plus a spin lock.
    // Thread-safety may be implemented by the client by acquiring and
    // manipulating the queue's lock vis-a-vis the 'mutex' function.

  public:
    // PUBLIC TYPES
    typedef bsl::function<void()> Job;

  private:
    // TYPES
    enum {
        // Queue states.

        e_ENQUEUEING_ENABLED,     // enqueueing is enabled
        e_ENQUEUEING_DISABLED,    // enqueueing is disabled
        e_ENQUEUEING_BLOCKED      // enqueueing is permanently disabled
    };

  private:
    bsl::deque<Job> d_list;
    bsls::AtomicInt d_numPendingJobs; // number of unprocessed jobs
    volatile int    d_state;          // maintains enqueue state
    bsls::AtomicInt d_numEnqueued;    // the number of items enqueued into this
                                      // queue since creation or the last time
                                      // it was reset.
    bsls::AtomicInt d_numDequeued;    // the number of items dequeued from this
                                      // queue since creation or the last time
                                      // it was reset.
  private:
    // FRIENDS
    friend class MultiQueueThreadPool;
    friend class MultiQueueThreadPool_QueueContext;

  private:
    // NOT IMPLEMENTED
    MultiQueueThreadPool_Queue(const MultiQueueThreadPool_Queue&);
    MultiQueueThreadPool_Queue &operator=(const MultiQueueThreadPool_Queue &);

    // CREATORS
    explicit
    MultiQueueThreadPool_Queue(bslma::Allocator *basicAllocator = 0);
        // Create a 'MultiQueueThreadPool_Queue' with an initial capacity of 0.
        // Optionally specify a 'basicAllocator' used to supply memory If
        // 'basicAllocator' is 0, the default memory allocator is used.

  public:
    // CREATORS
    ~MultiQueueThreadPool_Queue();
        // Destroy this queue.

    // MANIPULATORS
    Job popFront();
        // Dequeue and return the element at the front of this queue.  The
        // behavior is undefined if the queue is empty.

    int pushBack(const Job& functor);
        // Enqueue the specified 'functor' at the end of this queue.  Return 0
        // on success, and a non-zero value if enqueuing is disabled.

    int pushFront(const Job& functor);
        // Push the specified 'functor' on the front of this queue.  Return 0
        // on success, and a non-zero value otherwise.  This method always
        // succeeds, regardless of whether or not enqueuing is disabled.

    void block();
        // Permanently disable enqueuing to this queue.

    void enable();
        // Enable enqueuing to this queue.

    void disable();
        // Disable enqueuing to this queue.

    void numProcessedReset(int *numDequeued, int *numEnqueued);
        // Load into the specified 'numDequeued' and 'numEnqueued' the number
        // of items dequeued / enqueued (respectively) since the last time
        // these values were reset and set these values to 0.

    void reset();
        // Reset this queue to its initial state.  After this method returns,
        // the object is ready for use as though it were a new object.  Note
        // that this method is not thread-safe.

    // ACCESSORS
    int length() const;
        // Return an instantaneous snapshot of the length of this queue.

    void numProcessed(int *numDequeued, int *numEnqueued) const;
        // Load into the specified 'numDequeued' and 'numEnqueued' the number
        // of items dequeued / enqueued (respectively) since the last time
        // these values were reset.
};

                  // =======================================
                  // class MultiQueueThreadPool_QueueContext
                  // =======================================

class MultiQueueThreadPool_QueueContext {
    // This private class encapsulates a lightweight job queue and a callback
    // which processes the queue.

  public:
    // PUBLIC TYPES
    typedef bsl::function<void()> QueueProcessorCb;
        // This type defines a callback functor that is enqueued to the thread
        // pool, and used to process elements of a specific queue.  The queue
        // ID must be bound to the functor at the time of its instantiation.

    // PUBLIC DATA MEMBERS
    MultiQueueThreadPool_Queue d_queue;
    mutable bsls::SpinLock     d_lock;
    QueueProcessorCb           d_processingCb;
    bool                       d_destroyFlag;

  private:
    // NOT IMPLEMENTED
    MultiQueueThreadPool_QueueContext(
                                     const MultiQueueThreadPool_QueueContext&);
    MultiQueueThreadPool_QueueContext& operator=(
                                     const MultiQueueThreadPool_QueueContext&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(MultiQueueThreadPool_QueueContext,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    MultiQueueThreadPool_QueueContext(bslma::Allocator *basicAllocator = 0);
        // Construct a queue context object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the default memory allocator is used.

    ~MultiQueueThreadPool_QueueContext();
        // Destroy this queue context.

    // MANIPULATORS
    void reset();
        // Reset this queue context to its initial state.  The behavior is
        // undefined unless this context's lock is in an unlocked state.  After
        // this method returns, the object is ready for use as though it were a
        // new object.  Note that this method is not thread-safe.

    // ACCESSORS
    bsls::SpinLock& mutex() const;
        // Return the lock that is used by this context.

};

                         // ==========================
                         // class MultiQueueThreadPool
                         // ==========================

class MultiQueueThreadPool {
    // This class implements a dynamic, configurable pool of queues, each of
    // which is processed serially by a thread pool.

  public:
    // PUBLIC TYPES
    typedef bsl::function<void()> Job;
    typedef bsl::function<void()> CleanupFunctor;

  private:
    // TYPES
    enum {
        e_ENQUEUE_FRONT,    // enqueue new job at front of queue
        e_ENQUEUE_BACK      // enqueue new job at back of queue
    };

  private:
    // DATA
    bslma::Allocator *d_allocator_p;        // memory allocator (held)
    ThreadPool       *d_threadPool_p;       // threads for queue processing
    bool              d_threadPoolIsOwned;  // 'true' if thread pool is owned

    bdlcc::ObjectPool<
          MultiQueueThreadPool_QueueContext,
          bdlcc::ObjectPoolFunctors::DefaultCreator,
          bdlcc::ObjectPoolFunctors::Reset<MultiQueueThreadPool_QueueContext>
    >                 d_queuePool;          // pool of queue contexts

    bdlcc::ObjectCatalog<MultiQueueThreadPool_QueueContext*>
                      d_queueRegistry;      // registry of queue contexts

    mutable bslmt::RWMutex
                      d_registryLock;       // synchronizes registry access
    bsls::AtomicInt   d_numActiveQueues;    // number of non-empty queues

    volatile int      d_state;              // maintains internal state
    bsls::SpinLock    d_stateLock;          // synchronizes internal state

    bsls::AtomicInt   d_numDequeued;        // the total number of requests
                                            // processed by this pool since the
                                            // last time this value was reset
    bsls::AtomicInt   d_numEnqueued;        // the total number of requests
                                            // enqueued into this pool since
                                            // the last time this value was
                                            // reset
  private:
    // NOT IMPLEMENTED
    MultiQueueThreadPool(const MultiQueueThreadPool&);
    MultiQueueThreadPool& operator=(const MultiQueueThreadPool&);

    // PRIVATE MANIPULATORS
    void createQueueContextCb(void *memory);
        // Instantiate a 'QueueContext' object in the specified 'memory'.

    void deleteQueueCb(int                    id,
                       const CleanupFunctor&  cleanupFunctor,
                       bslmt::Barrier        *barrier);
        // Remove the queue associated with the specified 'id' from the queue
        // registry, execute the specified 'cleanupFunctor', wait on the
        // specified 'barrier', and then delete the referenced queue.

    void processQueueCb(MultiQueueThreadPool_QueueContext *context);
        // If the queue contained in the specified 'context' is not empty,
        // dequeue the next job, and process it.

    int enqueueJobImpl(int id, const Job& functor, int where);
        // Enqueue the specified 'functor' to the queue specified by 'id' at
        // either the front or the back of the queue, as specified by 'where'.
        // Return 0 if enqueued successfully, and a non-zero value if queuing
        // is otherwise.  The behavior is undefined unless 'functor' is bound.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(MultiQueueThreadPool,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    MultiQueueThreadPool(const bslmt::ThreadAttributes&  threadAttributes,
                         int                             minThreads,
                         int                             maxThreads,
                         int                             maxIdleTime,
                         bslma::Allocator               *basicAllocator = 0);
        // Construct a 'MultiQueueThreadPool' with the specified
        // 'threadAttributes', 'minThread' and 'maxThreads' minimum and maximum
        // number of threads respectively, and the specified 'maxIdleTime'
        // maximum idle time (in milliseconds).  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the default memory allocator is used.  Note that the
        // 'MultiQueueThreadPool' is created without any queues.  Although
        // queues may be created, 'start' must be called before enqueuing jobs.

    explicit
    MultiQueueThreadPool(ThreadPool       *threadPool,
                         bslma::Allocator *basicAllocator = 0);
        // Construct a 'MultiQueueThreadPool' with the specified 'threadPool'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the default memory allocator is used.  The
        // behavior is undefined if 'threadPool' is 0.  Note that the
        // 'MultiQueueThreadPool' is created without any queues.  Although
        // queues may be created, 'start' must be called before enqueuing jobs.

    ~MultiQueueThreadPool();
        // Destroy this multi-queue thread pool.  Disable queuing on all
        // queues, and wait until all queues are empty.  Then, delete all
        // queues, and shut down the thread pool if the thread pool is owned by
        // this object.  This method will block if any thread is executing
        // 'start' or 'stop' at the time of the call.

    // MANIPULATORS
    int createQueue();
        // Create a queue with unlimited capacity and a default number of
        // initial elements.  Return a non-zero queue ID on success, and 0
        // otherwise.  The queue ID can be used to enqueue jobs to the queue,
        // or to delete the queue.

    int deleteQueue(int id, const CleanupFunctor& cleanupFunctor);
        // Disable enqueuing to the queue associated with the specified 'id',
        // and enqueue the specified 'cleanupFunctor' to the *front* of the
        // queue.  The 'cleanupFunctor' is guaranteed to be the last queue
        // element processed, after which the queue is destroyed and removed
        // from all internal registries.  The caller will NOT be blocked until
        // 'cleanupFunctor' executes to completion.  Return 0 on success, and a
        // non-zero value otherwise.  The behavior is undefined if this
        // function is called simultaneously with 'disableQueue', 'drainQueue',
        // 'start', 'drain', 'stop', or 'shutdown'.  Note that passing an
        // unbound 'cleanupFunctor' is equivalent to passing a 'cleanupFunctor'
        // bound with 'makeNull'.  Also note that this function will fail if
        // called on a stopped multi queue thread pool.

    int deleteQueue(int id);
        // Disable enqueuing to the queue associated with the specified 'id',
        // and block the calling thread until a currently-active callback, if
        // any, is completed.  Return 0 on success, and a non-zero value
        // otherwise.

    int disableQueue(int id);
        // Disable enqueuing to the queue associated with the specified 'id'.
        // Return 0 on success, and a non-zero value otherwise.

    int drainQueue(int id);
        // Wait until all jobs in the queue indicated by the specified 'id' are
        // finished.  This method simply blocks until that queue is empty,
        // without disabling the queue.  The queue may be enabled or disabled
        // when this is called.  Return 0 on success, and a non-zero value
        // otherwise.  The behavior is undefined if another thread deletes the
        // queue before this method finishes.

    int enqueueJob(int id, const Job& functor);
        // Enqueue the specified 'functor' to the queue specified by 'id'.
        // Return 0 if enqueued successfully, and a non-zero value if queuing
        // is otherwise.  The behavior is undefined unless 'functor' is bound.

    int enableQueue(int id);
        // Enable enqueuing to the queue associated with the specified 'id'.
        // Return 0 on success, and a non-zero value otherwise.  It is an error
        // to call 'enableQueue' if a previous call to 'stop' is being
        // executed.

    void numProcessedReset(int *numDequeued, int *numEnqueued);
        // Load into the specified 'numDequeued' and 'numEnqueued' the number
        // of items dequeued / enqueued (respectively) since the last time
        // these values were reset and reset these values.

    int start();
        // Enable queuing on all queues, start the thread pool if the thread
        // pool is owned by this object, and ensure that at least the minimum
        // number of processing threads are started.  Return 0 on success, and
        // a non-zero value otherwise.  This method will block if any thread is
        // executing 'drain', 'stop', or 'shutdown' at the time of the call.
        // This method has no effect if this thread pool has already been
        // started.

    void drain();
        // Block until all queues are empty.  This method waits until all
        // queues are empty without disabling the queues.  The queues and/or
        // the thread pool may be either enabled or disabled when this method
        // is called.  This method may be called on a stopped or started thread
        // pool.  This method will block if any thread is executing 'start',
        // 'stop', or 'shutdown' at the time of the call.  Note that 'drain'
        // does not attempt to delete queues directly.  However as a
        // side-effect of emptying all queues, any queue for which
        // 'deleteQueue' was called previously will be deleted before 'drain'
        // unblocks.

    void stop();
        // Disable queuing on all queues, and wait until all queues are empty.
        // Then, stop the thread pool if the thread pool is owned by this
        // object.  This method will block if any thread is executing 'start'
        // or 'drain' or 'shutdown' at the time of the call.  Note that 'stop'
        // does not attempt to delete queues directly.  However as a
        // side-effect of emptying all queues, any queue for which
        // 'deleteQueue' was called previously will be deleted before 'stop'
        // unblocks.

    void shutdown();
        // Disable queuing on all queues, and wait until all queues are empty.
        // Then, delete all queues, and shut down the thread pool if the thread
        // pool is owned by this object.  This method will block if any thread
        // is executing 'start' or 'drain' or 'stop' at the time of the call.

    // ACCESSORS
    int numQueues() const;
        // Return an instantaneous snapshot of the number of queues managed by
        // this object.

    int numElements(int id) const;
        // Return an instantaneous snapshot of the number of elements enqueued
        // in the queue associated with the specified 'id' as a non-negative
        // integer, or -1 if 'id' does not specify a valid queue.

    void numProcessed(int *numDequeued, int *numEnqueued) const;
        // Load into the specified 'numDequeued' and 'numEnqueued' the number
        // of items dequeued / enqueued (respectively) since the last time
        // these values were reset.

    const ThreadPool& threadPool() const;
        // Return a reference to the non-modifiable thread pool owned by this
        // object.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                         // --------------------------
                         // class MultiQueueThreadPool
                         // --------------------------

// MANIPULATORS
inline
int MultiQueueThreadPool::enqueueJob(int id, const Job& functor)
{
    ++d_numEnqueued;
    return enqueueJobImpl(id, functor, e_ENQUEUE_BACK);
}

inline
void MultiQueueThreadPool::numProcessedReset(int *numDequeued,
                                             int *numEnqueued)
{
    // Implementation note: This is not entirely thread-consistent, though
    // thread safe.  If in between the two 'swap' operations the number
    // enqueued changes, we can get a slightly inconsistent picture.

    *numDequeued = d_numDequeued.swap(0);
    *numEnqueued = d_numEnqueued.swap(0);
}

// ACCESSORS
inline
void MultiQueueThreadPool::numProcessed(int *numDequeued,
                                        int *numEnqueued) const
{
    *numDequeued = d_numDequeued;
    *numEnqueued = d_numEnqueued;
}

inline
int MultiQueueThreadPool::numQueues() const
{
    return d_queueRegistry.length();
}

inline
const ThreadPool& MultiQueueThreadPool::threadPool() const
{
    return *d_threadPool_p;
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
