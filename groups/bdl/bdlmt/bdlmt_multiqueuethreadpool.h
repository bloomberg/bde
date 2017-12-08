// bdlmt_multiqueuethreadpool.h                                       -*-C++-*-

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
// enqueuing a per-queue functor to the thread pool.  Each functor dequeues the
// next item from its associated queue, processes it, and re-enqueues itself to
// the thread pool.  Since there is at most one representative functor per
// queue, each queue is guaranteed to be processed serially by the thread pool.
//
// In addition to the ability to create, delete, pause, and resume queues,
// clients are able to tune the underlying thread pool in accordance with the
// 'bdlmt::ThreadPool' documentation.
//
///Disabled Queues
///---------------
// 'bdlmt::MultiQueueThreadPool' allows clients to disable and re-enable
// queues.  A disabled queue will allow no further jobs to be enqueued, but
// will continue to process the jobs that were enqueued prior to the call to
// 'disableQueue'.  Note that calling 'disableQueue' will block the calling
// thread until the currently executing job (if any) on that queue completes.
//
///Paused Queues
///-------------
// 'bdlmt::MultiQueueThreadPool' also allows clients to pause and resume
// queues.  Pausing a queue suspends the processing of jobs from a queue --
// i.e., after 'pause' returns no further jobs will be processed on that queue
// until the queue is resumed.  Note that calling 'pauseQueue' will block the
// calling thread until the currently executing job (if any) on that queue
// completes.
//
///Thread Safety
///-------------
// The 'bdlmt::MultiQueueThreadPool' class is *fully thread-safe* (i.e., all
// public methods of a particular instance may safely execute concurrently).
// This class is also *thread-enabled* (i.e., the class does not function
// correctly in a non-multi-threading environment).  See 'bsldoc_glossary' for
// complete definitions of *fully thread-safe* and *thread-enabled*.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Word Search Application
/// - - - - - - - - - - - - - - - - - -
// This example illustrates the use of a 'bdlmt::MultiQueueThreadPool' in a
// word search application called 'fastSearch'.  'fastSearch' searches a list
// of files for a list of words, and returns the set of files which contain all
// of the specified words.  'bdlmt::MultiQueueThreadPool' is used to provide
// concurrent processing of files, and to simplify the collection of results by
// serializing access to result sets which are maintained for each word.
//
// First, we present a class used to manage a word, and the set of files which
// contain that word:
//..
//  class my_SearchProfile {
//      // This class defines a search profile consisting of a word and a set
//      // of files (given by name) that contain the word.  Here, "word" is
//      // defined as any string of characters.
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
//          // Optionally specify a 'basicAllocator' used to supply memory.  If
//          // 'basicAllocator' is 0, the default memory allocator is used.
//
//      ~my_SearchProfile();
//          // Destroy this search profile.
//
//      // MANIPULATORS
//      void insert(const char *file);
//          // Insert the specified 'file' into the file set maintained by this
//          // search profile.
//
//      // ACCESSORS
//      bool isMatch(const char *file) const;
//          // Return 'true' if the specified 'file' matches this search
//          // profile.
//
//      const bsl::set<bsl::string>& fileSet() const;
//          // Return a reference to the non-modifiable file set maintained by
//          // this search profile.
//
//      const bsl::string& word() const;
//          // Return a reference to the non-modifiable word maintained by this
//          // search profile.
//  };
//..
// And the implementation:
//..
//  // CREATORS
//  my_SearchProfile::my_SearchProfile(const char       *word,
//                                     bslma::Allocator *basicAllocator)
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
//      // Insert the specified 'file' to the file set of the specified search
//      // 'profile' if 'file' matches the 'profile'.
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
//                  int                              repetitions = 1,
//                  bslma::Allocator                *basicAllocator = 0)
//  {
//      // Return the set of files, specified by 'fileList', containing every
//      // word in the specified 'wordList', in the specified 'resultSet'.
//      // Optionally specify 'repetitions', the number of repetitions to run
//      // the search jobs (it is used to increase the load for performance
//      // testing).  Optionally specify a 'basicAllocator' used to supply
//      // memory.  If 'basicAllocator' is 0, the default memory allocator is
//      // used.
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
//          k_MIN_THREADS = 4,
//          k_MAX_THREADS = 20,
//          k_MAX_IDLE    = 100  // use a very short idle time since new jobs
//                               // arrive only at startup
//      };
//      bslmt::ThreadAttributes     defaultAttrs;
//      bdlmt::MultiQueueThreadPool pool(defaultAttrs,
//                                       k_MIN_THREADS,
//                                       k_MAX_THREADS,
//                                       k_MAX_IDLE,
//                                       basicAllocator);
//      RegistryType profileRegistry(bsl::less<bsl::string>(), basicAllocator);
//
//      // Create a queue and a search profile associated with each word in
//      // 'wordList'.
//
//      for (ListType::const_iterator it = wordList.begin();
//           it != wordList.end();
//           ++it) {
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
//      // Start the pool, enabling enqueuing and queue processing.
//      pool.start();
//
//      // Enqueue a job which tries to match each file in 'fileList' with each
//      // search profile.
//
//      for (ListType::const_iterator it = fileList.begin();
//           it != fileList.end();
//           ++it) {
//          for (ListType::const_iterator jt = wordList.begin();
//               jt != wordList.end();
//               ++jt) {
//              const bsl::string& file = *it;
//              const bsl::string& word = *jt;
//              RegistryValue&     rv   = profileRegistry[word];
//              Func               job;
//              makeFunc(&job, my_SearchCb, rv.second, file.c_str());
//              for (int i = 0; i < repetitions; ++i) {
//                  int rc = pool.enqueueJob(rv.first, job);
//                  LOOP_ASSERT(word, 0 == rc);
//              }
//          }
//      }
//
//      // Stop the pool, and wait while enqueued jobs are processed.
//      pool.stop();
//
//      // Construct the 'resultSet' as the intersection of file sets collected
//      // in each search profile.
//
//      resultSet.insert(fileList.begin(), fileList.end());
//      for (RegistryType::iterator it = profileRegistry.begin();
//           it != profileRegistry.end();
//           ++it) {
//          my_SearchProfile *profile = it->second.second;
//          const bsl::set<bsl::string>& fileSet = profile->fileSet();
//          bsl::set<bsl::string> tmpSet;
//          bsl::set_intersection(fileSet.begin(),
//                                fileSet.end(),
//                                resultSet.begin(),
//                                resultSet.end(),
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

#ifndef INCLUDED_BDLCC_OBJECTPOOL
#include <bdlcc_objectpool.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEXASSERT
#include <bslmt_mutexassert.h>
#endif

#ifndef INCLUDED_BSLMT_READERWRITERMUTEX
#include <bslmt_readerwritermutex.h>
#endif

#ifndef INCLUDED_BSLMT_READLOCKGUARD
#include <bslmt_readlockguard.h>
#endif

#ifndef INCLUDED_BSLMT_SEMAPHORE
#include <bslmt_semaphore.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BSL_DEQUE
#include <bsl_deque.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

namespace BloombergLP {
namespace bslmt { class Latch; }
namespace bdlmt {

class MultiQueueThreadPool;

                     // ================================
                     // class MultiQueueThreadPool_Queue
                     // ================================

class MultiQueueThreadPool_Queue {
    // This private class provides a thread-safe, lightweight job queue.

  public:
    // PUBLIC TYPES
    typedef bsl::function<void()> Job;

  private:
    // PRIVATE TYPES
    enum EnqueueState {
        // enqueue states
        e_ENQUEUING_ENABLED,   // enqueuing is enabled
        e_ENQUEUING_DISABLED,  // enqueuing is disabled
        e_DELETING             // deleting
    };

    enum RunState {
        e_NOT_SCHEDULED,       // running but not scheduled
        e_SCHEDULED,           // running and scheduled
        e_PAUSING,             // pause requested but not completed yet
        e_PAUSED               // paused
    };

    // DATA
    MultiQueueThreadPool      *d_multiQueueThreadPool_p;
                                                 // the 'MultiQueueThreadPool'
                                                 // that owns this object

    bsl::deque<Job>            d_list;           // queue of jobs to be
                                                 // executed

    EnqueueState               d_enqueueState;   // maintains enqueue state

    RunState                   d_runState;       // maintains run state

    mutable bslmt::Mutex       d_lock;           // protect queue and
                                                 // informational members

    bslmt::Semaphore           d_pauseBlock;     // use to notify thread
                                                 // awaiting pause state

    int                        d_pauseCount;     // number of threads waiting
                                                 // for the pause to complete

    Job                        d_processingCb;   // bound processing callback
                                                 // for pool

    bslmt::ThreadUtil::Handle  d_processor;      // current worker thread, or
                                                 // ThreadUtil::invalidHandle()

    // NOT IMPLEMENTED
    MultiQueueThreadPool_Queue();
    MultiQueueThreadPool_Queue(const MultiQueueThreadPool_Queue&);
    MultiQueueThreadPool_Queue &operator=(const MultiQueueThreadPool_Queue &);

    // PRIVATE MANIPULATORS
    void setPaused();
        // Mark this queue as paused, notify any threads blocked on
        // 'd_pauseBlock', and schedule the deletion job if this queue is to be
        // deleted.  The behavior is undefined unless this queue's lock is in a
        // locked state and 'e_PAUSING == d_runState'.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MultiQueueThreadPool_Queue,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    MultiQueueThreadPool_Queue(MultiQueueThreadPool *multiQueueThreadPool,
                               bslma::Allocator     *basicAllocator = 0);
        // Create a 'MultiQueueThreadPool_Queue' with an initial capacity of 0
        // and initialized to use the specified 'multiQueueThreadPool' to track
        // aggregate values (e.g., the number of active queues) and to obtain
        // the thread pool used to execute jobs that are appended to this
        // queue.  Optionally specify a 'basicAllocator' used to supply memory.
        // If 'basicAllocator' is 0, the default memory allocator is used.

    ~MultiQueueThreadPool_Queue();
        // Destroy this queue.

    // MANIPULATORS
    int enable();
        // Enable enqueuing to this queue.  Return 0 on success, and a non-zero
        // value otherwise.  This method will fail (with an error) if
        // 'prepareForDeletion' has already been called on this object.

    int disable();
        // Disable enqueuing to this queue.  Return 0 on success, and a
        // non-zero value otherwise.  This method will fail (with an error) if
        // 'prepareForDeletion' has already been called on this object.

    int pause();
        // Wait until any currently-executing job on the queue completes, then
        // prevent any more jobs from being executed on that queue.  Return 0
        // on success, and a non-zero value if the queue is already paused or
        // is being paused or deleted by another thread.  Note that this method
        // may be invoked from a job executing on the given queue, in which
        // case this method does not wait.  Note also that this method differs
        // from 'disable' in that (1) 'pause' stops processing for a queue, and
        // (2) does *not* prevent additional jobs from being enqueued.

    void executeFront();
        // Execute the 'Job' at the front of this queue, dequeue the 'Job', and
        // if the queue is not paused schedule a callback from the associated
        // thread pool.  The behavior is undefined if this queue is empty.

    bool prepareForDeletion(const Job& functor, const Job& functorIfProcessor);
        // Permanently disable enqueuing to this queue.  If the calling thread
        // is not the current thread processing a job, return 'false' and
        // schedule the specified 'functor' to be executed by a thread from the
        // associated thread pool.  Otherwise, return 'true' and schedule the
        // specified 'functorIfProcessor' to be executed by a thread from the
        // associated thread pool

    int pushBack(const Job& functor);
        // Enqueue the specified 'functor' at the end of this queue.  Return 0
        // on success, and a non-zero value if enqueuing is disabled.

    int pushFront(const Job& functor);
        // Add the specified 'functor' at the front of this queue.  Return 0 on
        // success, and a non-zero value if enqueuing is disabled.

    void reset();
        // Reset this queue to its initial state.  The behavior is undefined
        // unless this queue's lock is in an unlocked state.  After this method
        // returns, the object is ready for use as though it were a new object.
        // Note that this method is not thread-safe and is used by the object
        // pool contained within '*d_multiQueueThreadPool_p'.

    int resume();
        // Allow jobs on the queue to begin executing.  Return 0 on success,
        // and a non-zero value if the queue is not paused or '!d_list.empty()'
        // and the associated thread pool fails to enqueue a job.

    // ACCESSORS
    bool isDrained() const;
        // Report whether all jobs in this queue are finished.

    bool isEnabled() const;
        // Report whether enqueuing to this object is enabled.  This object is
        // constructed with enqueuing enabled.

    bool isPaused() const;
        // Report whether this object is paused.

    int length() const;
        // Return an instantaneous snapshot of the length of this queue.
};

                        // ==========================
                        // class MultiQueueThreadPool
                        // ==========================

class MultiQueueThreadPool {
    // This class implements a dynamic, configurable pool of queues, each of
    // which is processed serially by a thread pool.

    // FRIENDS
    friend class MultiQueueThreadPool_Queue;

    // PRIVATE TYPES
    enum State {
        // Internal running states.
        e_STATE_RUNNING,
        e_STATE_STOPPING,
        e_STATE_STOPPED
    };

  public:
    // PUBLIC TYPES
    typedef bsl::function<void()>                       Job;
    typedef bsl::function<void()>                       CleanupFunctor;
    typedef bsl::map<int, MultiQueueThreadPool_Queue *> QueueRegistry;

  private:
    // DATA
    bslma::Allocator *d_allocator_p;        // memory allocator (held)

    ThreadPool       *d_threadPool_p;       // threads for queue processing

    bool              d_threadPoolIsOwned;  // 'true' if thread pool is owned

    bdlcc::ObjectPool<
          MultiQueueThreadPool_Queue,
          bdlcc::ObjectPoolFunctors::DefaultCreator,
          bdlcc::ObjectPoolFunctors::Reset<MultiQueueThreadPool_Queue>
    >                 d_queuePool;          // pool of queues

    QueueRegistry     d_queueRegistry;      // registry of queues

    int               d_nextId;             // next id to provide from
                                            // 'createQueue'

    State             d_state;              // maintains internal state

    mutable bslmt::ReaderWriterMutex
                      d_lock;               // locked for write when deleting
                                            // queues or changing pool state

    bsls::AtomicInt   d_numActiveQueues;    // number of non-empty queues

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
    void deleteQueueCb(MultiQueueThreadPool_Queue *queue,
                       const CleanupFunctor&       cleanupFunctor,
                       bslmt::Latch               *latch);
        // If the specified 'latch' is non-null, execute 'latch->arrive()'.
        // Otherwise, execute the specified 'cleanupFunctor' if it is valid.
        // Then, delete the specified 'queue'.  The behavior is undefined
        // unless '0 != queue'.  Note that this callback provides a mechanism
        // for proper lifetime management of the 'queue' by scheduling the
        // deletion with the associated thread pool since the
        // 'MultiQueueThreadPool' does not know *when* to delete the queue and
        // a 'MultiQueueThreadPool_Queue' cannot delete itself at the
        // appropriate time.

    int findIfUsable(int id, MultiQueueThreadPool_Queue **queue);
       // Load into the specified '*queue' a pointer to the queue referenced by
       // the specified 'id' if this 'MultiQueueThreadPool' is in a state where
       // the 'queue' can be used.  Return 0 on success, and a non-zero value
       // if the 'id' is not contained in 'd_queueRegistry', this
       // 'MultiQueueThreadPool' is not in the running state, or
       // '0 == d_threadPool_p->enabled()'.  The behavior is undefined unless
       // the invoking thread has a lock, read or write, on 'd_lock'.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MultiQueueThreadPool,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    MultiQueueThreadPool(const bslmt::ThreadAttributes&  threadAttributes,
                         int                             minThreads,
                         int                             maxThreads,
                         int                             maxIdleTime,
                         bslma::Allocator               *basicAllocator = 0);
        // Construct a 'MultiQueueThreadPool' with the specified
        // 'threadAttributes', the specified 'minThreads' minimum number of
        // threads, the specified 'maxThreads' maximum number of threads, and
        // the specified 'maxIdleTime' maximum idle time (in milliseconds).
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless '0 <= minThreads',
        // 'minThreads <= maxThreads', and '0 <= maxIdleTime'.  Note that the
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
    int addJobAtFront(int id, const Job& functor);
        // Add the specified 'functor' at the front of the queue specified by
        // 'id'.  Return 0 if added successfully, and a non-zero value if
        // queuing is disabled.  The behavior is undefined unless 'functor' is
        // bound.  Note that the position of 'functor' relative to any
        // currently queued jobs is unspecified unless the queue is currently
        // paused.

    int createQueue();
        // Create a queue with unlimited capacity and a default number of
        // initial elements.  Return a non-zero queue ID.  The queue ID can be
        // used to enqueue jobs to the queue, or to control or delete the
        // queue.

    int deleteQueue(int id, const CleanupFunctor& cleanupFunctor);
        // Disable enqueuing to the queue associated with the specified 'id',
        // and enqueue the specified 'cleanupFunctor' to the *front* of the
        // queue.  The 'cleanupFunctor' is guaranteed to be the last queue
        // element processed, after which the queue is destroyed.  This
        // function does not wait for the 'cleanupFunctor' to be executed
        // (instead the caller is notified asynchronously through the execution
        // of the supplied 'cleanupFunctor').  Return 0 on success, and a
        // non-zero value otherwise.  Note that this function will fail if this
        // pool is stopped.

    int deleteQueue(int id);
        // Disable enqueuing to the queue associated with the specified 'id',
        // and block the calling thread until a currently-active callback, if
        // any, is completed; then destroy the queue.  Return 0 on success, and
        // a non-zero value otherwise.  Note that this function will fail if
        // this pool is stopped.

    int disableQueue(int id);
        // Disable enqueuing to the queue associated with the specified 'id'.
        // Return 0 on success, and a non-zero value otherwise.  Note that this
        // method differs from 'pauseQueue' in that (1) 'disableQueue' does
        // *not* stop processing for a queue, and (2) prevents additional jobs
        // from being enqueued.

    int drainQueue(int id);
        // Wait until all jobs in the queue indicated by the specified 'id' are
        // finished.  This method simply waits until that queue is empty,
        // without disabling the queue; it may thus wait indefinitely if more
        // jobs are being added.  The queue may be enabled or disabled when
        // this method is called.  Return 0 on success, and a non-zero value if
        // the specified queue does not exist or is deleted while this method
        // is waiting.  Note that this method waits by repeatedly yielding.

    int enqueueJob(int id, const Job& functor);
        // Enqueue the specified 'functor' to the queue specified by 'id'.
        // Return 0 if enqueued successfully, and a non-zero value if queuing
        // is disabled.  The behavior is undefined unless 'functor' is bound.

    int enableQueue(int id);
        // Enable enqueuing to the queue associated with the specified 'id'.
        // Return 0 on success, and a non-zero value otherwise.  It is an error
        // to call 'enableQueue' if a previous call to 'stop' is being
        // executed.

    void numProcessedReset(int *numDequeued, int *numEnqueued);
        // Load into the specified 'numDequeued' and 'numEnqueued' the number
        // of items dequeued / enqueued (respectively) since the last time
        // these values were reset and reset these values.

    int pauseQueue(int id);
        // Wait until any currently-executing job on the queue with the
        // specified 'id' completes, then prevent any more jobs from being
        // executed on that queue.  Return 0 on success, and a non-zero value
        // if the queue is already paused or is being paused or deleted by
        // another thread.  Note that this method may be invoked from a job
        // executing on the given queue, in which case this method does not
        // wait.  Note also that this method differs from 'disableQueue' in
        // that (1) 'pauseQueue' stops processing for a queue, and (2) does
        // *not* prevent additional jobs from being enqueued.

    int resumeQueue(int id);
        // Allow jobs on the queue with the specified 'id' to begin executing.
        // Return 0 on success, and a non-zero value if the queue does not
        // exist or is not paused.

    int start();
        // Enable queuing on all queues, start the thread pool if the thread
        // pool is owned by this object, and ensure that at least the minimum
        // number of processing threads are started.  Return 0 on success, and
        // a non-zero value otherwise.  This method will block if any thread is
        // executing 'stop' or 'shutdown' at the time of the call.  This method
        // has no effect if this thread pool has already been started.  Note
        // that any paused queues remain paused.

    void drain();
        // Wait until all queues are empty.  This method waits until all
        // non-paused queues are empty without disabling the queues (and may
        // thus wait indefinitely).  The queues and/or the thread pool may be
        // either enabled or disabled when this method is called.  This method
        // may be called on a stopped or started thread pool.  Note that
        // 'drain' does not attempt to delete queues directly.  However, as a
        // side-effect of emptying all queues, any queue for which
        // 'deleteQueue' was called previously will be deleted before 'drain'
        // returns.  Note also that this method waits by repeatedly yielding.

    void stop();
        // Disable queuing on all queues and wait until all non-paused queues
        // are empty.  Then, stop the thread pool if the thread pool is owned
        // by this object.  Note that 'stop' does not attempt to delete queues
        // directly.  However, as a side-effect of emptying all queues, any
        // queue for which 'deleteQueue' was called previously will be deleted
        // before 'stop' unblocks.

    void shutdown();
        // Disable queuing on all queues, and wait until all non-paused queues
        // are empty.  Then, delete all queues, and shut down the thread pool
        // if the thread pool is owned by this object.

    // ACCESSORS
    bool isPaused(int id) const;
        // Return 'true' if the queue associated with the specified 'id' is
        // currently paused, or 'false' otherwise (including if 'id' is not a
        // valid queue id).

    bool isEnabled(int id) const;
        // Return 'true' if the queue associated with the specified 'id' is
        // currently enabled, or 'false' otherwise (including if 'id' is not a
        // valid queue id).

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
//                             INLINE DEFINITIONS
// ============================================================================

                     // --------------------------------
                     // class MultiQueueThreadPool_Queue
                     // --------------------------------

// PRIVATE MANIPULATORS
inline
void MultiQueueThreadPool_Queue::setPaused()
{
    BSLS_ASSERT(e_PAUSING == d_runState);

    BSLMT_MUTEXASSERT_IS_LOCKED(&d_lock);

    d_runState = e_PAUSED;

    if (d_pauseCount) {
        d_pauseBlock.post(d_pauseCount);
        d_pauseCount = 0;
    }

    if (e_DELETING == d_enqueueState) {
        int status = d_multiQueueThreadPool_p->d_threadPool_p->
                                                    enqueueJob(d_list.front());

        BSLS_ASSERT(0 == status);  (void)status;
    }

    // Note that decreasing the number of active queues must be done last to
    // ensure the 'enqueueJob' above will always succeed.

    --d_multiQueueThreadPool_p->d_numActiveQueues;
}

// ACCESSORS
inline
bool MultiQueueThreadPool_Queue::isDrained() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

    return 0 == d_list.size() && (   e_NOT_SCHEDULED == d_runState
                                  || e_PAUSED        == d_runState);
}

inline
bool MultiQueueThreadPool_Queue::isEnabled() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

    return e_ENQUEUING_ENABLED == d_enqueueState;
}

inline
bool MultiQueueThreadPool_Queue::isPaused() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

    return e_PAUSED == d_runState;
}

inline
int MultiQueueThreadPool_Queue::length() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);

    return static_cast<int>(d_list.size());
}

                        // --------------------------
                        // class MultiQueueThreadPool
                        // --------------------------

// PRIVATE MANIPULATORS
inline
int MultiQueueThreadPool::findIfUsable(int                          id,
                                       MultiQueueThreadPool_Queue **queue)
{
    if (   e_STATE_RUNNING != d_state
        ||               0 == d_threadPool_p->enabled()) {
        return 1;                                                     // RETURN
    }

    QueueRegistry::iterator iter = d_queueRegistry.find(id);

    if (d_queueRegistry.end() == iter) {
        return 1;
    }

    *queue = iter->second;

    return 0;
}

// MANIPULATORS
inline
int MultiQueueThreadPool::addJobAtFront(int id, const Job& functor)
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    MultiQueueThreadPool_Queue *queue;

    if (findIfUsable(id, &queue)) {
        return 1;                                                     // RETURN
    }

    if (0 == queue->pushFront(functor)) {
        ++d_numEnqueued;
        return 0;                                                     // RETURN
    }

    return 1;
}

inline
int MultiQueueThreadPool::enqueueJob(int id, const Job& functor)
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    MultiQueueThreadPool_Queue *queue;

    if (findIfUsable(id, &queue)) {
        return 1;                                                     // RETURN
    }

    if (0 == queue->pushBack(functor)) {
        ++d_numEnqueued;
        return 0;                                                     // RETURN
    }

    return 1;
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
bool MultiQueueThreadPool::isEnabled(int id) const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    QueueRegistry::const_iterator iter = d_queueRegistry.find(id);

    if (d_queueRegistry.end() != iter) {
        return iter->second->isEnabled();                             // RETURN
    }

    return false;
}

inline
bool MultiQueueThreadPool::isPaused(int id) const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    QueueRegistry::const_iterator iter = d_queueRegistry.find(id);

    if (d_queueRegistry.end() != iter) {
        return iter->second->isPaused();                              // RETURN
    }

    return false;
}

inline
int MultiQueueThreadPool::numElements(int id) const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    QueueRegistry::const_iterator iter = d_queueRegistry.find(id);

    if (d_queueRegistry.end() != iter) {
        return iter->second->length();                                // RETURN
    }

    return -1;
}

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
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_lock);

    return static_cast<int>(d_queueRegistry.size());
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
// Copyright 2017 Bloomberg Finance L.P.
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
