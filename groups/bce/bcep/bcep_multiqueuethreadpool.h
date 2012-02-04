// bcep_multiqueuethreadpool.h                                        -*-C++-*-
#ifndef INCLUDED_BCEP_MULTIQUEUETHREADPOOL
#define INCLUDED_BCEP_MULTIQUEUETHREADPOOL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a pool of queues, each processed serially by a thread pool.
//
//@CLASSES:
// bcep_MultiQueueThreadPool: multi-threaded, serial processing of queues
//
//@AUTHOR: David Rubin (drubin6)
//
//@SEE_ALSO: bcep_threadpool
//
//@DESCRIPTION: This component defines a dynamic, configurable pool of queues,
// each of which is processed by a thread in a thread pool, such that elements
// on a given queue are processed serially, regardless of which thread is
// processing the queue at a given time.
//
// A 'bcep_MultiQueueThreadPool' allows clients to create and delete queues,
// and to enqueue "jobs" (represented as client-specified functors) to specific
// queues.  Queue processing is implemented on top of a 'bcep_ThreadPool' by
// enqueuing a per-queue functor to the thread pool.  Each functor dequeues
// the next item from its associated queue, processes it, and re-enqueues
// itself to the thread pool.  Since there is only one representative functor
// per queue, each queue is guaranteed to be processed serially by the thread
// pool.
//
// In addition to the ability to create and delete queues, clients are able to
// tune the underlying thread pool in accordance with the 'bcep_ThreadPool'
// documentation.
//
///Thread Safety
///-------------
// The 'bcep_MultiQueueThreadPool' class is *thread-aware*, but not *fully
// thread-safe*.  The function-level documentation identifies methods (i.e.,
// the 'reset' method) that cannot be safely used by multiple threads.  This
// class is also *thread-enabled* (i.e., the class does not function correctly
// in a non-multi-threading environment).  See 'bsldoc_glossary' for complete
// definitions of *thread-aware*, *fully thread-safe*, and *thread-enabled*.
//
///Usage Examples
///--------------
// The first example illustrates how to use a 'bcep_MultiQueueThreadPool' in
// a word search application.
//
///Example 1
///- - - - -
// This example illustrates the use of a 'bcep_MultiQueueThreadPool' in a
// word search application called 'fastSearch'.  'fastSearch' searches a list
// of files for a list of words, and returns the set of files which contain
// all of the specified words.  'bcep_MultiQueueThreadPool' is used to
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
//      my_SearchProfile(const char      *word,
//                       bslma_Allocator *basicAllocator = 0);
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
//          const char      *word,
//          bslma_Allocator *basicAllocator)
//  : d_word(basicAllocator)
//  , d_fileSet(bsl::less<bsl::string>(), basicAllocator)
//  {
//      ASSERT(word);
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
//
// Next, we define a helper function to perform a search of a word in a
// particular file.  The function is parameterized by a search profile and
// a file name.  If the specified file name matches the profile, it is
// inserted into the profile's file list.
//..
//  void my_SearchCb(my_SearchProfile* profile, const char *file)
//  {
//      // Insert the specified 'file' to the file set of the specified
//      // search 'profile' if 'file' matches the 'profile'.
//
//      ASSERT(profile);
//      ASSERT(file);
//
//      if (profile->isMatch(file)) {
//          profile->insert(file);
//      }
//  }
//..
//
// Lastly, we present the front end to the search application: 'fastSearch'.
// 'fastSearch' is parameterized by a list of words to search for, a list of
// files to search in, and a set which is populated with the search results.
// 'fastSearch' instantiates a 'bcep_MultiQueueThreadPool', and creates a
// queue for each word.  It then associates each queue with a search profile
// based on a word in the word list.  Then, it enqueues a job to each queue
// for each file in the file list that tries to match the file to each
// search profile.  Lastly, 'fastSearch' collects the results, which is the
// set intersection of each file set maintained by the individual search
// profiles.
//..
//  void fastSearch(const bsl::vector<bsl::string>&  wordList,
//                  const bsl::vector<bsl::string>&  fileList,
//                  bsl::set<bsl::string>&           resultSet,
//                  bslma_Allocator                 *basicAllocator = 0)
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
//      bcemt_Attribute           defaultAttrs;
//      bcep_MultiQueueThreadPool pool(defaultAttrs,
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
//          bslma_Allocator *allocator =
//                                    bslma_Default::allocator(basicAllocator);
//
//          const bsl::string& word = *it;
//          int                id = pool.createQueue();
//          LOOP_ASSERT(word, 0 != id);
//          my_SearchProfile *profile = new (*allocator)
//                                               my_SearchProfile(word.c_str(),
//                                                                allocator);
//
//          bslma_RawDeleterProctor<my_SearchProfile, bslma_Allocator>
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
//              bdef_Function<void (*)()> job  =
//                  bdef_BindUtil::bind(&my_SearchCb, rv.second, file.c_str());
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
//          bslma_Default::allocator(basicAllocator)->deleteObjectRaw(profile);
//      }
//  }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEP_THREADPOOL
#include <bcep_threadpool.h>
#endif

#ifndef INCLUDED_BCEC_OBJECTCATALOG
#include <bcec_objectcatalog.h>
#endif

#ifndef INCLUDED_BCEC_OBJECTPOOL
#include <bcec_objectpool.h>
#endif

#ifndef INCLUDED_BCEMT_RWMUTEX
#include <bcemt_rwmutex.h>
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

#ifndef INCLUDED_BSL_DEQUE
#include <bsl_deque.h>
#endif

namespace BloombergLP {

class bslma_Allocator;
class bcemt_Barrier;

                   // =====================================
                   // class bcep_MultiQueueThreadPool_Queue
                   // =====================================

class bcep_MultiQueueThreadPool_Queue {
    // This private class provides a lightweight queue, plus a spin lock.
    // Thread-safety may be implemented by the client by acquiring
    // and manipulating the queue's lock vis-a-vis the 'mutex' function.

  public:
    // PUBLIC TYPES
    typedef bdef_Function<void(*)()> Job;

  private:
    // TYPES
    enum {
        // Queue states.

        BCEP_ENQUEUEING_ENABLED,     // enqueueing is enabled
        BCEP_ENQUEUEING_DISABLED,    // enqueueing is disabled
        BCEP_ENQUEUEING_BLOCKED      // enqueueing is permanently disabled
    };

  private:
    bsl::deque<Job>       d_list;
    bces_AtomicInt        d_numPendingJobs; // number of unprocessed jobs
    volatile int          d_state;          // maintains enqueue state
    bces_AtomicInt        d_numEnqueued;    // the number of items enqueued
                                            // into this queue since the
                                            // creation or the last time
                                            // it was reset.
    bces_AtomicInt        d_numDequeued;    // the number of items dequeued
                                            // into this queue since the
                                            // creation or the last time
                                            // it was reset.
  private:
    // FRIENDS
    friend class bcep_MultiQueueThreadPool;
    friend class bcep_MultiQueueThreadPool_QueueContext;

    // not implemented
    bcep_MultiQueueThreadPool_Queue(const bcep_MultiQueueThreadPool_Queue&);
    bcep_MultiQueueThreadPool_Queue& operator=(
                                    const bcep_MultiQueueThreadPool_Queue&);

    // CREATORS
    bcep_MultiQueueThreadPool_Queue(bslma_Allocator *basicAllocator = 0);
        // Create a 'bcep_MultiQueueThreadPool_Queue' with an initial capacity
        // of 0.  Optionally specify a 'basicAllocator' used to supply memory
        // If 'basicAllocator' is 0, the default memory allocator is used.

  public:
    // CREATORS
    ~bcep_MultiQueueThreadPool_Queue();
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

               // ============================================
               // class bcep_MultiQueueThreadPool_QueueContext
               // ============================================

class bcep_MultiQueueThreadPool_QueueContext {
    // This private class encapsulates a lightweight job queue and a callback
    // which processes the queue.

  public:
    // PUBLIC TYPES
    typedef bdef_Function<void(*)()> QueueProcessorCb;
        // This type defines a callback functor that is enqueued to the thread
        // pool, and used to process elements of a specific queue.  The queue
        // ID must be bound to the functor at the time of its instantiation.

    // PUBLIC DATA MEMBERS
    bcep_MultiQueueThreadPool_Queue  d_queue;
    mutable bces_SpinLock            d_lock;
    QueueProcessorCb                 d_processingCb;
    bool                             d_destroyFlag;

  private:
    // not implemented
    bcep_MultiQueueThreadPool_QueueContext(
                               const bcep_MultiQueueThreadPool_QueueContext&);
    bcep_MultiQueueThreadPool_QueueContext& operator=(
                               const bcep_MultiQueueThreadPool_QueueContext&);

  public:

    BSLALG_DECLARE_NESTED_TRAITS(bcep_MultiQueueThreadPool_QueueContext,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    bcep_MultiQueueThreadPool_QueueContext(
                                          bslma_Allocator *basicAllocator = 0);
        // Construct a queue context object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the default memory allocator is used.

    ~bcep_MultiQueueThreadPool_QueueContext();
        // Destroy this queue context.

    void reset();
        // Reset this queue context to its initial state.  The behavior is
        // undefined unless this context's lock is in an unlocked state.  After
        // this method returns, the object is ready for use as though it were a
        // new object.  Note that this method is not thread-safe.

    // ACCESSORS
    bces_SpinLock& mutex() const;
        // Return the lock that is used by this context.

};

                      // ===============================
                      // class bcep_MultiQueueThreadPool
                      // ===============================

class bcep_MultiQueueThreadPool {
    // This class implements a dynamic, configurable pool of queues, each of
    // which is processed serially by a thread pool.

  public:
    // PUBLIC TYPES
    typedef bdef_Function<void(*)()> Job;
    typedef bdef_Function<void(*)()> CleanupFunctor;

  private:
    //TYPES
    enum {
        BCEP_ENQUEUE_FRONT,    // enqueue new job at front of queue
        BCEP_ENQUEUE_BACK      // enqueue new job at back of queue
    };

  private:
    bslma_Allocator *d_allocator_p;        // memory allocator (held)
    bcep_ThreadPool *d_threadPool_p;       // threads for queue processing
    bool             d_threadPoolIsOwned;  // 'true' if thread pool is owned

    bcec_ObjectPool<bcep_MultiQueueThreadPool_QueueContext,
                    bcec_ObjectPoolFunctors::DefaultCreator,
                    bcec_ObjectPoolFunctors::Reset<
                                     bcep_MultiQueueThreadPool_QueueContext> >
                     d_queuePool;          // pool of queue contexts

    bcec_ObjectCatalog<bcep_MultiQueueThreadPool_QueueContext*>
                     d_queueRegistry;      // registry of queue contexts

    mutable bcemt_RWMutex
                     d_registryLock;       // synchronizes registry access
    bces_AtomicInt   d_numActiveQueues;    // number of non-empty queues

    volatile int     d_state;              // maintains internal state
    bces_SpinLock    d_stateLock;          // synchronizes internal state

    bces_AtomicInt   d_numDequeued;        // the total number of request
                                           // processed by this pool
                                           // since the last time this value
                                           // was reset
    bces_AtomicInt   d_numEnqueued;        // the total number of request
                                           // enqueued into this pool
                                           // since the last time this value
                                           // was reset
  private:
    // NOT IMPLEMENTED
    bcep_MultiQueueThreadPool(const bcep_MultiQueueThreadPool&);
    bcep_MultiQueueThreadPool& operator=(const bcep_MultiQueueThreadPool&);

    // PRIVATE MANIPULATORS
    void createQueueContextCb(void *memory);
        // Instantiate a 'QueueContext' object in the specified 'memory'.

    void deleteQueueCb(int                    id,
                       const CleanupFunctor&  cleanupFunctor,
                       bcemt_Barrier         *barrier);
        // Remove the queue associated with the specified 'id' from the queue
        // registry, execute the specified 'cleanupFunctor', wait on the
        // specified 'barrier', and then delete the referenced queue.

    void processQueueCb(bcep_MultiQueueThreadPool_QueueContext *context);
        // If the queue contained in the specified 'context' is not empty,
        // dequeue the next job, and process it.

    int enqueueJobImpl(int id, const Job& functor, int where);
        // Enqueue the specified 'functor' to the queue specified by 'id'
        // at either the front or the back of the queue, as specified by
        // 'where'.  Return 0 if enqueued successfully, and a non-zero value if
        // queuing is otherwise.  The behavior is undefined unless 'functor'
        // is bound.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcep_MultiQueueThreadPool,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    bcep_MultiQueueThreadPool(const bcemt_Attribute& threadAttributes,
                              int                    minThreads,
                              int                    maxThreads,
                              int                    maxIdleTime,
                              bslma_Allocator       *basicAllocator = 0);
        // Construct a 'bcep_MultiQueueThreadPool' with the specified
        // 'threadAttributes', 'minThread' minimum number of threads,
        // 'maxThreads' maximum number of threads, 'maxIdleTime' maximum
        // idle time (in milliseconds).  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the default memory
        // allocator is used.  Note that the 'bcep_MultiQueueThreadPool' is
        // created without any queues.  Although queues may be created, 'start'
        // must be called before enqueuing jobs.

    bcep_MultiQueueThreadPool(bcep_ThreadPool *threadPool,
                              bslma_Allocator *basicAllocator = 0);
        // Construct a 'bcep_MultiQueueThreadPool' with the specified
        // 'threadPool'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the default memory allocator is
        // used.  The behavior is undefined if 'threadPool' is 0.  Note that
        // the 'bcep_MultiQueueThreadPool' is created without any queues.
        // Although queues may be created, 'start' must be called before
        // enqueuing jobs.

    ~bcep_MultiQueueThreadPool();
        // Destroy this multi-queue thread pool.  Disable queuing on all
        // queues, and wait until all queues are empty.  Then, delete all
        // queues, and shut down the thread pool if the thread pool is owned
        // by this object.  This method will block if any thread is executing
        // 'start' or 'stop' at the time of the call.  Note that no queue will
        // be deleted unless 'deleteQueue' was called previously.

    // MANIPULATORS
    int createQueue();
        // Create a queue with unlimited capacity and a default number of
        // initial elements.  Return a non-zero queue ID on success, and 0
        // otherwise.  The queue ID can be used to enqueue jobs to the queue,
        // or to delete the queue.

    int deleteQueue(int id, const CleanupFunctor& cleanupFunctor);
        // Disable enqueuing to the queue associated with the specified 'id',
        // and enqueue 'cleanupFunctor' to the *front* of the queue.
        // The specified 'cleanupFunctor' is guaranteed to be the last queue
        // element processed, after which the queue is destroyed and removed
        // from all internal registries.  The caller will NOT be blocked until
        // 'cleanupFunctor' executes to completion.  Return 0 on success, and
        // a non-zero value otherwise.  Note that passing an unbound
        // 'cleanupFunctor' is equivalent to passing a 'cleanupFunctor' bound
        // with 'makeNull'.

    int deleteQueue(int id);
        // Disable enqueuing to the queue associated with the specified 'id',
        // and block the calling thread until a currently-active callback,
        // if any, is completed.  Return 0 on success, and a non-zero value
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
        // Return 0 if enqueued successfully, and a non-zero value if
        // queuing is otherwise.  The behavior is undefined unless 'functor'
        // is bound.

    int enableQueue(int id);
        // Enable enqueuing to the queue associated with the specified 'id'.
        // Return 0 on success, and a non-zero value otherwise.  It is an
        // error to call 'enableQueue' if a previous call to 'stop' is being
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
        // 'stop', or 'shutdown' at the time of the call.

    void stop();
        // Disable queuing on all queues, and wait until all queues are
        // empty.  Then, stop the thread pool if the thread pool is owned by
        // this object.  This method will block if any thread is executing
        // 'start' or 'drain' or 'shutdown' at the time of the call.  Note
        // that no queue will be deleted unless 'deleteQueue' was called
        // previously.

    void shutdown();
        // Disable queuing on all queues, and wait until all queues are
        // empty.  Then, delete all queues, and shut down the thread pool if
        // the thread pool is owned by this object.  This method will block
        // if any thread is executing 'start' or 'drain' or 'stop' at the time
        // of the call.

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

    const bcep_ThreadPool& threadPool() const;
        // Return a reference to the non-modifiable thread pool owned by this
        // object.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // -------------------------------
                      // class bcep_MultiQueueThreadPool
                      // -------------------------------

// MANIPULATORS
inline
int bcep_MultiQueueThreadPool::enqueueJob(int id, const Job& functor)
{
    ++d_numEnqueued;
    return enqueueJobImpl(id, functor, BCEP_ENQUEUE_BACK);
}

inline
void bcep_MultiQueueThreadPool::numProcessedReset(int *numDequeued,
                                                  int *numEnqueued)
{
    // Implementation note:
    // This is not entirely thread-consistent, though thread safe.
    // If in between the two 'swap' operations the number enqueued changes,
    // we can get a slightly inconsistent picture.
    *numDequeued = d_numDequeued.swap(0);
    *numEnqueued = d_numEnqueued.swap(0);
}

// ACCESSORS
inline
void bcep_MultiQueueThreadPool::numProcessed(int *numDequeued,
                                             int *numEnqueued) const
{
    *numDequeued = d_numDequeued;
    *numEnqueued = d_numEnqueued;
}

inline
int bcep_MultiQueueThreadPool::numQueues() const
{
    return d_queueRegistry.length();
}

inline
const bcep_ThreadPool& bcep_MultiQueueThreadPool::threadPool() const
{
    return *d_threadPool_p;
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
