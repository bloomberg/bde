// bdlcc_cache.h                                                      -*-C++-*-
#ifndef INCLUDED_BDLCC_CACHE
#define INCLUDED_BDLCC_CACHE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a in-process cache with configurable eviction policy.
//
//@CLASSES:
//  bdlcc::Cache: in-process key-value cache
//
//@DESCRIPTION: This component defines a single class template, 'bdlcc::Cache',
// implementing a thread-safe in-memory key-value cache with a configurable
// eviction policy.
//
// 'bdlcc::Cache' class uses similar template parameters to
// 'bsl::unordered_map': the key type ('KEY'), the value type ('VALUE'), the
// optional hash function ('HASH'), and the optional equal function ('EQUAL').
// 'bdlcc::Cache' does not support the standard allocator template parameter
// (although 'bslma::Allocator' is supported).
//
// The cache size can be controlled by setting the low watermark and high
// watermark attributes, which is used instead of a single maximum size
// attribute for performance benefits.  Eviction of cached items starts when
// 'size() >= highWatermark' and continues until 'size() < lowWatermark'.  A
// fixed maximum size is obtained by setting the high and low watermarks to the
// same value.
//
// Two eviction policies are supported: LRU (Least Recently Used) and FIFO
// (First In, First Out).  With LRU, the item that has *not* been accessed for
// the longest period of time will be evicted first.  With FIFO, the eviction
// order is based on the order of insertion, with the earliest inserted item
// being evicted first.
//
///Thread Safety
///-------------
// The 'bdlcc::Cache' class template is fully thread-safe (see
// 'bsldoc_glossary') provided that the allocator supplied at construction and
// the default allocator in effect during the lifetime of cached items are both
// fully thread-safe.
//
///Thread Contention
///-----------------
// Threads accessing a 'bdlcc::Cache' may block while waiting for other threads
// to complete their operations upon the cache.  Concurrent reading is
// supported.  Neither readers or writers are starved by the other group.
//
// All of the modifier methods of the cache potentially requires a write lock.
// Of particular note is the 'tryGetValue' method, which requires a writer lock
// only if the eviction queue needs to be modified.  This means 'tryGetValue'
// requires only a read lock if the eviction policy is set to FIFO or the
// argument 'modifyEvictionQueue' is set to 'false'.  For limited cases where
// contention is likely, temporarily setting 'modifyEvictionQueue' to 'false'
// might be of value.
//
// The 'visit' method acquires a read lock and calls the supplied visitor
// function for every item in the cache, or until the visitor function returns
// 'false'.  If the supplied visitor is expensive or the cache is very large,
// calls to modifier methods might be starved until the 'visit' method finishes
// looping through the cache items.  Therefore, the 'visit' method should be
// used judiciously by making the method call relatively cheap or ensuring that
// no time-sensitive write operation is done at the same time as a call to the
// 'visit' method.  A 'visit' method call is inexpensive if the visitor returns
// quickly, or if the visitor returns false after only a subset of the cache
// items were processed.
//
///Post-eviction Callback and Potential Deadlocks
///---------------------------------------------
// When an item is evicted or erased from the cache, the previously set
// post-eviction callback (via the 'setPostEvictionCallback' method) will be
// invoked within the calling thread, supplying a pointer to the item being
// removed.
//
// The cache object itself should not be used in a post-eviction callback;
// otherwise, a deadlock may result.  Since a write lock is held during the
// call to the callback, invoking any operation on the cache that acquires a
// lock inside the callback will lead to a deadlock.
//
///Runtime Complexity
///------------------
//..
// +----------------------------------------------------+--------------------+
// | Operation                                          | Complexity         |
// +====================================================+====================+
// | insert                                             | Average: O[1]      |
// |                                                    | Worst:   O[n]      |
// +----------------------------------------------------+--------------------+
// | tryGetValue                                        | O[1]               |
// +----------------------------------------------------+--------------------+
// | popFront                                           | O[1]               |
// +----------------------------------------------------+--------------------+
// | erase                                              | O[1]               |
// +----------------------------------------------------+--------------------+
// | visit                                              | O[n]               |
// +----------------------------------------------------+--------------------+
//..
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// This examples shows some basic usage of the cache.  First, we define a
// custom post-eviction callback function, 'myPostEvictionCallback' that simply
// prints the evicted item to stdout:
//..
//  void myPostEvictionCallback(bsl::shared_ptr<bsl::string> value)
//  {
//      bsl::cout << "Evicted: " << *value << bsl::endl;
//  }
//..
// Then, we define a 'bdlcc::Cache' object, 'myCache', that maps 'int' to
// 'bsl::string' and uses the LRU eviction policy:
//..
//  bdlcc::Cache<int, bsl::string>
//      myCache(bdlcc::CacheEvictionPolicy::e_LRU, 6, 7, &talloc);
//..
// Next, we insert 3 items into the cache and verify that the size of the cache
// has been updated correctly:
//..
//  myCache.insert(0, "Alex");
//  myCache.insert(1, "John");
//  myCache.insert(2, "Rob");
//  assert(myCache.size() == 3);
//..
// Then, we bulk insert 3 additional items into the cache and verify that the
// size of the cache has been updated correctly:
//..
//  typedef bsl::pair<int, bsl::shared_ptr<bsl::string> > PairType;
//  bsl::vector<PairType> insertData(&talloc);
//  insertData.push_back(PairType(3,
//                        bsl::allocate_shared<bsl::string>(&talloc, "Jim" )));
//  insertData.push_back(PairType(4,
//                        bsl::allocate_shared<bsl::string>(&talloc, "Jeff")));
//  insertData.push_back(PairType(5,
//                        bsl::allocate_shared<bsl::string>(&talloc, "Ian" )));
//  myCache.insertBulk(insertData);
//  ASSERT(myCache.size() == 6);
//..
// Next, we retrieve the second value of the second item stored in the cache
// using the 'tryGetValue' method:
//..
//  bsl::shared_ptr<bsl::string> value;
//  int rc = myCache.tryGetValue(&value, 1);
//  assert(rc == 0);
//  assert(*value == "John");
//..
// Then, we set the cache's post-eviction callback to 'myPostEvictionCallback':
//..
//  myCache.setPostEvictionCallback(myPostEvictionCallback);
//..
// Now, we insert two more items into the cache to trigger the eviction
// behavior:
//..
//  myCache.insert(6, "Steve");
//  assert(myCache.size() == 7);
//  myCache.insert(7, "Tim");
//  assert(myCache.size() == 6);
//..
// Notice that after we insert "Steve", the size of the cache is 7, the high
// watermark.  After the following item, "Tim", is inserted, the size of the
// cache goes back down to 6, the low watermark.
//
// Finally, we observe the following output to stdout:
//..
//  Evicted: Alex
//  Evicted: Rob
//..
// Notice that the item "John" was not evicted even though it was inserted
// before "Rob", because "John" was accessed after "Rob" was inserted.
//
///Example 2: Updating Cache in The Background
///- - - - - - - - - - - - - - - - - - - - - -
// Suppose that a service needs to retrieve some values that are relatively
// expensive to compute.  Clients of the service cannot wait for computing the
// values, so the service should pre-compute and cache them.  In addition, the
// values are only valid for around one hour, so older items must be
// periodically updated in the cache.  This problem can be solved using
// 'bdlcc::Cache' with a background updater thread.
//
// First, we define the types representing the cached values and the cache
// itself:
//..
//  struct MyValue {
//      int            d_data;       // data
//      bdlt::Datetime d_timestamp;  // last update time stamp
//  };
//  typedef bdlcc::Cache<int, MyValue> MyCache;
//..
// Then, suppose that we have access to a function 'retrieveValue' that returns
// a 'MyValue' object given a 'int' key:
//..
//  MyValue retrieveValue(int key)
//  {
//      MyValue ret = {key, bdlt::CurrentTime::utc()};
//      return ret;
//  }
//..
// Next, we define a visitor type to aggregate keys of the out-of-date values
// in the cache:
//..
//  struct MyVisitor {
//      // Visitor to 'MyCache'.
//      bsl::vector<int>  d_oldKeys;  // list of out-of-date keys
//
//      MyVisitor()
//      : d_oldKeys(&talloc)
//      {}
//
//      bool operator() (int key, const MyValue& value)
//        // Check if the specified 'value' is older than 1 hour.  If so,
//        // insert the specified 'key' into 'd_oldKeys'.
//      {
//          if (veryVerbose) {
//              bsl::cout << "Visiting " << key
//                        << ", age: "
//                        << bdlt::CurrentTime::utc() - value.d_timestamp
//                        << bsl::endl;
//          }
//
//          if (bdlt::CurrentTime::utc() - value.d_timestamp <
//              // bdlt::DatetimeInterval(0, 60)) {
//              bdlt::DatetimeInterval(0, 0, 0, 3)) {
//              return false;                                         // RETURN
//          }
//
//          d_oldKeys.push_back(key);
//          return true;
//      }
//  };
//..
// Then, we define the background thread function to find and update the
// out-of-date values:
//..
//  void myWorker(MyCache *cache)
//  {
//      while (true) {
//          if (cache->size() == 0) {
//              break;
//          }
//
//          // Find and update the old values once per minute.
//          // bslmt::ThreadUtil::microSleep(0, 60);
//          bslmt::ThreadUtil::microSleep(0, 5);
//          MyVisitor visitor;
//          cache->visit(visitor);
//          for (bsl::vector<int>::const_iterator itr =
//               visitor.d_oldKeys.begin();
//               itr != visitor.d_oldKeys.end(); ++itr) {
//              if (veryVerbose) bsl::cout << "Updating " << *itr << bsl::endl;
//              cache->insert(*itr, retrieveValue(*itr));
//          }
//      }
//  }
//
//  extern "C" void *myWorkerThread(void *v_cache)
//  {
//      MyCache *cache = (MyCache *) v_cache;
//      myWorker(cache);
//      return 0;
//  }
//..
// Finally, we define the entry point of the application:
//..
//  void example2()
//  {
//      MyCache myCache(bdlcc::CacheEvictionPolicy::e_FIFO, 100, 120, &talloc);
//
//      // Pre-populate the cache.
//
//      myCache.insert(0, retrieveValue(0));
//      myCache.insert(1, retrieveValue(1));
//      myCache.insert(2, retrieveValue(2));
//      ASSERT(myCache.size() == 3);
//
//      bslmt::ThreadUtil::Handle myWorkerHandle;
//
//      int rc = bslmt::ThreadUtil::create(&myWorkerHandle, myWorkerThread,
//                                         &myCache);
//      ASSERT(rc == 0);
//
//      // Do some work.
//
//      bslmt::ThreadUtil::microSleep(0, 7);
//      ASSERT(myCache.size() == 3);
//
//      // Clean up.
//
//      myCache.clear();
//      ASSERT(myCache.size() == 0);
//      bslmt::ThreadUtil::join(myWorkerHandle);
//  }
//..

#ifndef INCLUDED_BSLIM_PRINTER
#include <bslim_printer.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ALLOCATORARGT
#include <bslmf_allocatorargt.h>
#endif

#ifndef INCLUDED_BSLMT_READERWRITERMUTEX
#include <bslmt_readerwritermutex.h>
#endif

#ifndef INCLUDED_BSLMT_READLOCKGUARD
#include <bslmt_readlockguard.h>
#endif

#ifndef INCLUDED_BSLMT_WRITELOCKGUARD
#include <bslmt_writelockguard.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_UNORDERED_MAP
#include <bsl_unordered_map.h>
#endif

#ifndef INCLUDED_BSL_LIST
#include <bsl_list.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_IOSTREAM
#include <bsl_iostream.h>
#endif

#ifndef INCLUDED_BSL_LIMITS
#include <bsl_limits.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>            // 'bsl::size_t'
#endif

namespace BloombergLP {
namespace bdlcc {

struct CacheEvictionPolicy {

    // TYPES
    enum Enum {
        // Enumeration of supported cache eviction policies.

        e_LRU,  // Least Recently Used
        e_FIFO  // First In, First Out
    };
};

template <class KEY>
class Cache_QueueProctor {
    // This class implements a proctor that, on destruction, removes the last
    // element of a 'QueueType' object.  This proctor is intended to work with
    // 'bdlcc::Cache' to provide basic exception safety guarantee.

    // DATA
    bsl::list<KEY> *d_queue_p;  // queue (held, not owned)

  public:
    // CREATORS
    explicit Cache_QueueProctor(bsl::list<KEY> *queue);
        // Create a 'Cache_QueueProctor' object to monitor the specified
        // 'queue'.

    ~Cache_QueueProctor();
        // Destroy this proctor object.  Remove the last element of the 'queue'
        // being monitored.

    // MANIPULATORS
    void release();
        // Release the queue specified on construction, so that it will not be
        // modified on the destruction of this proctor.
};

template <class KEY,
          class VALUE,
          class HASH  = bsl::hash<KEY>,
          class EQUAL = bsl::equal_to<KEY> >
class Cache_TestUtil;

template <class KEY,
          class VALUE,
          class HASH  = bsl::hash<KEY>,
          class EQUAL = bsl::equal_to<KEY> >
class Cache {
    // This class represents a simple in-process key-value store supporting a
    // variety of eviction policies.

  public:
    // PUBLIC TYPES
    typedef bsl::shared_ptr<VALUE>                            ValuePtrType;
        // Shared pointer type pointing to value type.

    typedef bsl::function<void(const ValuePtrType&)> PostEvictionCallback;
        // Type of function to call after an item has been evicted from the
        // cache.

    typedef bsl::pair<KEY, ValuePtrType>                          KVType;
        // Value type of a bulk insert entry.

  private:
    // PRIVATE TYPES
    typedef bsl::list<KEY>                                        QueueType;
        // Eviction queue type.

    typedef bsl::pair<ValuePtrType, typename QueueType::iterator> MapValue;
        // Value type of the hash map.

    typedef bsl::unordered_map<KEY, MapValue, HASH, EQUAL>        MapType;
        // Hash map type.

    typedef bslmt::ReaderWriterMutex                              LockType;

    // DATA
    bslma::Allocator          *d_allocator_p;          // memory allocator
                                                       // (held, not owned)

    mutable LockType           d_rwlock;               // reader-writer lock

    MapType                    d_map;                  // hash table storing
                                                       // key-value pairs

    QueueType                  d_queue;                // queue storing
                                                       // eviction order of
                                                       // keys, the key of the
                                                       // first item to be
                                                       // evicted is at the
                                                       // front of the queue

    CacheEvictionPolicy::Enum  d_evictionPolicy;       // eviction policy

    bsl::size_t                d_lowWatermark;         // the size of this
                                                       // cache when eviction
                                                       // stops

    bsl::size_t                d_highWatermark;        // the size of this
                                                       // cache when eviction
                                                       // starts after an
                                                       // insert

    PostEvictionCallback       d_postEvictionCallback; // the function to call
                                                       // after a value has
                                                       // been evicted from the
                                                       // cache

    // FRIENDS
    friend class Cache_TestUtil<KEY, VALUE, HASH, EQUAL>;

    // PRIVATE MANIPULATORS
    void enforceHighWatermark();
        // Evict items from this cache if 'size() >= highWatermark()' until
        // 'size() < lowWatermark()' beginning from the front of the eviction
        // queue.  Invoke the post-eviction callback for each item evicted.

    void evictItem(const typename MapType::iterator& mapIt);
        // Evict the item at the specified 'mapIt' and invoke the post-eviction
        // callback for that item.

    void insertImp(const KEY& key, const ValuePtrType& valuePtr);
        // Insert the specified 'key' and its associated 'valuePtr' into this
        // cache.  If 'key' already exists, then its value will be replaced
        // with 'value'.

    void insertImpWrapper(const KEY&   key,
                          const VALUE& value,
                          bsl::true_type);
    void insertImpWrapper(const KEY&   key,
                          const VALUE& value,
                          bsl::false_type);
        // Insert the specified 'key' and its associated 'value' into this
        // cache.  If 'key' already exists, then its value will be replaced
        // with 'value'.  The last parameter is 'true_type' if 'VALUE' uses a
        // 'bslma::Allocator', and 'false_type' otherwise.

    // NOT IMPLEMENTED
    Cache(const Cache<KEY, VALUE, HASH, EQUAL>&);
    // BDE_VERIFY pragma: -FD01
    Cache<KEY, VALUE, HASH, EQUAL>& operator=(const Cache<KEY, VALUE, HASH>&);

    // BDE_VERIFY pragma: +FD01

  public:
    // CREATORS
    explicit Cache(bslma::Allocator *basicAllocator = 0);
        // Create an empty LRU cache having no size limit.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    Cache(CacheEvictionPolicy::Enum  evictionPolicy,
          bsl::size_t                lowWatermark,
          bsl::size_t                highWatermark,
          bslma::Allocator          *basicAllocator = 0);
        // Create an empty cache using the specified 'evictionPolicy' and the
        // specified 'lowWatermark' and 'highWatermark'.  Optionally specify
        // the 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.  The behavior
        // is undefined unless 'lowWatermark <= highWatermark',
        // '1 <= lowWatermark', and '1 <= highWatermark'.

    Cache(CacheEvictionPolicy::Enum  evictionPolicy,
          bsl::size_t                lowWatermark,
          bsl::size_t                highWatermark,
          const HASH&                hashFunction,
          const EQUAL&               equalFunction,
          bslma::Allocator          *basicAllocator = 0);
        // Create an empty cache using the specified 'evictionPolicy',
        // 'lowWatermark', and 'highWatermark'.  The specified 'hashFunction'
        // is used to generate the hash values for a given key, and the
        // specified 'equalFunction' is used to determine whether two keys have
        // the same value.  Optionally specify the 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // 'lowWatermark <= highWatermark', '1 <= lowWatermark', and
        // '1 <= highWatermark'.

    // ~Cache() = default;
        // Destroy this object.

    // MANIPULATORS
    void clear();
        // Remove all items from this cache.  Do *not* invoke the post-eviction
        // callback.

    int erase(const KEY& key);
        // Remove the item having the specified 'key' from this cache.  Invoke
        // the post-eviction callback for the removed item.  Return 0 on
        // success and 1 if 'key' does not exist.

    int eraseBulk(const bsl::vector<KEY>& keys);
        // Remove the items having the specified 'keys' from this cache.
        // Invoke the post-eviction callback for each removed item.  Return
        // the number of items successfully removed.

    void insert(const KEY& key, const VALUE& value);
        // Insert the specified 'key' and its associated 'value' into this
        // cache.  If 'key' already exists, then its value will be replaced
        // with 'value'.

    void insert(const KEY& key, const ValuePtrType& valuePtr);
        // Insert the specified 'key' and its associated 'valuePtr' into this
        // cache.  If 'key' already exists, then its value will be replaced
        // with 'value'.

    int insertBulk(const bsl::vector<KVType>& data);
        // Insert the specified 'data' (composed of Key-Value pairs) into this
        // cache.  If a key already exists, then its value will be replaced
        // with the value.  Return the number of items successfully inserted.

    int popFront();
        // Remove the item at the front of the eviction queue.  Invoke the
        // post-eviction callback for the removed item.  Return 0 on success,
        // and 1 if this cache is empty.

    void setPostEvictionCallback(
                             const PostEvictionCallback& postEvictionCallback);
        // Set the post-eviction callback to the specified
        // 'postEvictionCallback'.  The post-eviction callback is invoked for
        // each item evicted or removed from this cache.

    int tryGetValue(bsl::shared_ptr<VALUE> *value,
                    const KEY&              key,
                    bool                    modifyEvictionQueue = true);
        // Load, into the specified 'value', the value associated with the
        // specified 'key' in this cache.  If the optionally specified
        // 'modifyEvictionQueue' is 'true' and the eviction policy is LRU, then
        // move the cached item to the back of the eviction queue.  Return 0 on
        // success, and 1 if 'key' does not exist in this cache.  Note that a
        // write lock is acquired only if this queue is modified.

    // ACCESSORS
    EQUAL equalFunction() const;
        // Return (a copy of) the key-equality functor used by this cache that
        // returns 'true' if two 'KEY' objects have the same value, and 'false'
        // otherwise.

    CacheEvictionPolicy::Enum evictionPolicy() const;
        // Return the eviction policy used by this cache.

    HASH hashFunction() const;
        // Return (a copy of) the unary hash functor used by this cache to
        // generate a hash value (of type 'std::size_t') for a 'KEY' object.

    bsl::size_t highWatermark() const;
        // Return the high watermark of this cache, which is the size at which
        // eviction of existing items begins.

    bsl::size_t lowWatermark() const;
        // Return the low watermark of this cache, which is the size at which
        // eviction of existing items ends.

    bsl::size_t size() const;
        // Return the current size of this cache.

    template <class VISITOR>
    void visit(VISITOR& visitor) const;
        // Call the specified 'visitor' for every item stored in this cache in
        // the order of the eviction queue until 'visitor' returns 'false'.
        // The 'VISITOR' type must be a callable object that can be invoked in
        // the same way as the function 'bool (const KEY&, const VALUE&)'

};

template <class KEY,
          class VALUE,
          class HASH,
          class EQUAL>
class Cache_TestUtil {
    // This class implements a test utility that gives the test driver access
    // to the lock / unlock method of the RW mutex.  Its purpose is to allow
    // testing that the locking actually happens as planned.

    // DATA
    Cache<KEY, VALUE, HASH, EQUAL>& d_cache;

  public:
    // CREATORS
    explicit Cache_TestUtil(Cache<KEY, VALUE, HASH, EQUAL>& cache);
        // Create a 'Cache_TestUtil' object to test locking in the specified
        // 'cache'.

    // ~Cache_TestUtil() = default;
        // Destroy this object.

    // MANIPULATORS
    void lockRead();
        // Call the 'lockRead' method of 'bdlcc::Cache' 'd_rwlock' lock.

    void lockWrite();
        // Call the 'lockWrite' method of 'bdlcc::Cache' 'd_rwlock' lock.

    void unlock();
        // Call the 'unlock' method of 'bdlcc::Cache' 'd_rwlock' lock.

};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ------------------------
                        // class Cache_QueueProctor
                        // ------------------------

// CREATORS
template <class KEY>
inline
Cache_QueueProctor<KEY>::Cache_QueueProctor(
                                      bsl::list<KEY> *queue) : d_queue_p(queue)
{
}

template <class KEY>
inline
Cache_QueueProctor<KEY>::~Cache_QueueProctor()
{
    if (d_queue_p) {
        d_queue_p->pop_back();
    }
}

// MANIPULATORS
template <class KEY>
inline
void Cache_QueueProctor<KEY>::release()
{
    d_queue_p = 0;
}

                        // -----------
                        // class Cache
                        // -----------

// CREATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
Cache<KEY, VALUE, HASH, EQUAL>::Cache(bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_map(d_allocator_p)
, d_queue(d_allocator_p)
, d_evictionPolicy(CacheEvictionPolicy::e_LRU)
, d_lowWatermark(bsl::numeric_limits<bsl::size_t>::max())
, d_highWatermark(bsl::numeric_limits<bsl::size_t>::max())
, d_postEvictionCallback(bsl::allocator_arg, d_allocator_p)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL>
Cache<KEY, VALUE, HASH, EQUAL>::Cache(
                                     CacheEvictionPolicy::Enum  evictionPolicy,
                                     bsl::size_t                lowWatermark,
                                     bsl::size_t                highWatermark,
                                     bslma::Allocator          *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_map(d_allocator_p)
, d_queue(d_allocator_p)
, d_evictionPolicy(evictionPolicy)
, d_lowWatermark(lowWatermark)
, d_highWatermark(highWatermark)
, d_postEvictionCallback(bsl::allocator_arg, d_allocator_p)
{
    BSLS_ASSERT_SAFE(lowWatermark <= highWatermark);
    BSLS_ASSERT_SAFE(1 <= lowWatermark);
    BSLS_ASSERT_SAFE(1 <= highWatermark);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
Cache<KEY, VALUE, HASH, EQUAL>::Cache(
                                     CacheEvictionPolicy::Enum  evictionPolicy,
                                     bsl::size_t                lowWatermark,
                                     bsl::size_t                highWatermark,
                                     const HASH&                hashFunction,
                                     const EQUAL&               equalFunction,
                                     bslma::Allocator          *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_map(0, hashFunction, equalFunction, d_allocator_p)
, d_queue(d_allocator_p)
, d_evictionPolicy(evictionPolicy)
, d_lowWatermark(lowWatermark)
, d_highWatermark(highWatermark)
, d_postEvictionCallback(bsl::allocator_arg, d_allocator_p)
{
    BSLS_ASSERT_SAFE(lowWatermark <= highWatermark);
    BSLS_ASSERT_SAFE(1 <= lowWatermark);
    BSLS_ASSERT_SAFE(1 <= highWatermark);
}

// PRIVATE MANIPULATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
void Cache<KEY, VALUE, HASH, EQUAL>::enforceHighWatermark()
{
    if (d_map.size() < d_highWatermark) {
        return;                                                       // RETURN
    }

    while (d_map.size() >= d_lowWatermark && d_map.size() > 0) {
        const typename MapType::iterator mapIt = d_map.find(d_queue.front());
        BSLS_ASSERT(mapIt != d_map.end());
        evictItem(mapIt);
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void Cache<KEY, VALUE, HASH, EQUAL>::evictItem(
                                       const typename MapType::iterator& mapIt)
{
    ValuePtrType value = mapIt->second.first;

    d_queue.erase(mapIt->second.second);
    d_map.erase(mapIt);

    if (d_postEvictionCallback) {
        d_postEvictionCallback(value);
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void Cache<KEY, VALUE, HASH, EQUAL>::insertImp(const KEY&          key,
                                               const ValuePtrType& valuePtr)
{
    bslmt::WriteLockGuard<LockType> guard(&d_rwlock);

    enforceHighWatermark();

    typename MapType::iterator mapIt = d_map.find(key);
    if (mapIt != d_map.end()) {
        mapIt->second.first = valuePtr;
        typename QueueType::iterator queueIt = mapIt->second.second;

        d_queue.splice(d_queue.end(), d_queue, queueIt);
    }
    else {
        d_queue.push_back(key);
        Cache_QueueProctor<KEY>      proctor(&d_queue);
        typename QueueType::iterator queueIt = d_queue.end();
        --queueIt;

        d_map.emplace(key, MapValue(valuePtr, queueIt, d_allocator_p));
        proctor.release();
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void Cache<KEY, VALUE, HASH, EQUAL>::insertImpWrapper(
                                                         const KEY&      key,
                                                         const VALUE&    value,
                                                         bsl::false_type)
{
    ValuePtrType valuePtr;
    valuePtr.createInplace(d_allocator_p, value);
    insertImp(key, valuePtr);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void Cache<KEY, VALUE, HASH, EQUAL>::insertImpWrapper(
                                                          const KEY&     key,
                                                          const VALUE&   value,
                                                          bsl::true_type)
{
    ValuePtrType valuePtr;
    valuePtr.createInplace(d_allocator_p, value, d_allocator_p);
    insertImp(key, valuePtr);
}

// MANIPULATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
void Cache<KEY, VALUE, HASH, EQUAL>::clear()
{
    bslmt::WriteLockGuard<LockType> guard(&d_rwlock);
    d_map.clear();
    d_queue.clear();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
int Cache<KEY, VALUE, HASH, EQUAL>::erase(const KEY& key)
{
    bslmt::WriteLockGuard<LockType> guard(&d_rwlock);

    const typename MapType::iterator mapIt = d_map.find(key);
    if (mapIt == d_map.end()) {
        return 1;                                                     // RETURN
    }

    evictItem(mapIt);
    return 0;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
int
Cache<KEY, VALUE, HASH, EQUAL>::eraseBulk(const bsl::vector<KEY>& keys)
{
    bslmt::WriteLockGuard<LockType> guard(&d_rwlock);

    int count = 0;
    for(bsl::size_t i = 0; i < keys.size(); ++i) {
        const typename MapType::iterator mapIt = d_map.find(keys[i]);
        if (mapIt == d_map.end()) {
            continue;
        }
        ++count;
        evictItem(mapIt);
    }
    return count;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void Cache<KEY, VALUE, HASH, EQUAL>::insert(const KEY& key,const VALUE& value)
{
    insertImpWrapper(key, value, bslma::UsesBslmaAllocator<VALUE>());
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void Cache<KEY, VALUE, HASH, EQUAL>::insert(const KEY&          key,
                                            const ValuePtrType& valuePtr)
{
    insertImp(key, valuePtr);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
int
Cache<KEY, VALUE, HASH, EQUAL>::insertBulk(const bsl::vector<KVType>& data)
{
    int                             count = 0;
    bslmt::WriteLockGuard<LockType> guard(&d_rwlock);

    for(bsl::size_t i = 0; i < data.size(); ++i) {
        enforceHighWatermark();

        typename MapType::iterator mapIt = d_map.find(data[i].first);
        if (mapIt != d_map.end()) {
            mapIt->second.first = data[i].second;
            typename QueueType::iterator queueIt = mapIt->second.second;

            d_queue.splice(d_queue.end(), d_queue, queueIt);
        }
        else {
            d_queue.push_back(data[i].first);
            Cache_QueueProctor<KEY>      proctor(&d_queue);
            typename QueueType::iterator queueIt = d_queue.end();
            --queueIt;

            d_map.emplace(data[i].first, MapValue(data[i].second, queueIt,
                                                               d_allocator_p));
            proctor.release();
            ++count;
        }
    }
    return count;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
int Cache<KEY, VALUE, HASH, EQUAL>::popFront()
{
    bslmt::WriteLockGuard<LockType> guard(&d_rwlock);

    if (d_map.size() > 0) {
        const typename MapType::iterator mapIt = d_map.find(d_queue.front());
        BSLS_ASSERT(mapIt != d_map.end());
        evictItem(mapIt);
        return 0;                                                     // RETURN
    }

    return 1;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void Cache<KEY, VALUE, HASH, EQUAL>::setPostEvictionCallback(
                              const PostEvictionCallback& postEvictionCallback)
{
    bslmt::WriteLockGuard<LockType> guard(&d_rwlock);
    d_postEvictionCallback = postEvictionCallback;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
int Cache<KEY, VALUE, HASH, EQUAL>::tryGetValue(
                                   bsl::shared_ptr<VALUE> *value,
                                   const KEY&              key,
                                   bool                    modifyEvictionQueue)
{
    int writeLock = d_evictionPolicy == CacheEvictionPolicy::e_LRU &&
         modifyEvictionQueue ? 1 : 0;
    if (writeLock) {
        d_rwlock.lockWrite();
    }
    else {
        d_rwlock.lockRead();
    }

    // Since the guard is constructed with a locked synchronization object, the
    // guard's call to 'unlock' correctly handles both read and write
    // scenarios.
    bslmt::ReadLockGuard<LockType> guard(&d_rwlock, true);

    typename MapType::iterator mapIt = d_map.find(key);
    if (mapIt == d_map.end()) {
        return 1;                                                     // RETURN
    }

    *value = mapIt->second.first;

    if (writeLock) {
        typename QueueType::iterator queueIt = mapIt->second.second;
        typename QueueType::iterator last = d_queue.end();
        --last;
        if (last != queueIt) {
            d_queue.splice(d_queue.end(), d_queue, queueIt);
        }
    }

    return 0;
}

// ACCESSORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
EQUAL Cache<KEY, VALUE, HASH, EQUAL>::equalFunction() const
{
    return d_map.key_eq();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
CacheEvictionPolicy::Enum
Cache<KEY, VALUE, HASH, EQUAL>::evictionPolicy() const
{
    return d_evictionPolicy;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
HASH Cache<KEY, VALUE, HASH, EQUAL>::hashFunction() const
{
    return d_map.hash_function();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t Cache<KEY, VALUE, HASH, EQUAL>::highWatermark() const
{
    return d_highWatermark;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t Cache<KEY, VALUE, HASH, EQUAL>::lowWatermark() const
{
    return d_lowWatermark;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t Cache<KEY, VALUE, HASH, EQUAL>::size() const
{
    bslmt::ReadLockGuard<LockType> guard(&d_rwlock);
    return d_map.size();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
template <class VISITOR>
void Cache<KEY, VALUE, HASH, EQUAL>::visit(VISITOR& visitor) const
{
    bslmt::ReadLockGuard<LockType> guard(&d_rwlock);

    for (typename QueueType::const_iterator queueIt = d_queue.begin();
         queueIt != d_queue.end(); ++queueIt) {

        const KEY&                             key = *queueIt;
        const typename MapType::const_iterator mapIt = d_map.find(key);
        BSLS_ASSERT(mapIt != d_map.end());
        const ValuePtrType& valuePtr = mapIt->second.first;

        if (!visitor(key, *valuePtr)) {
            break;
        }
    }
}

                            // --------------------
                            // class Cache_TestUtil
                            // --------------------

// CREATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
Cache_TestUtil<KEY, VALUE, HASH, EQUAL>::Cache_TestUtil(
                                         Cache<KEY, VALUE, HASH, EQUAL>& cache)
: d_cache(cache)
{
}

// MANIPULATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void Cache_TestUtil<KEY, VALUE, HASH, EQUAL>::lockRead()
{
    d_cache.d_rwlock.lockRead();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void Cache_TestUtil<KEY, VALUE, HASH, EQUAL>::lockWrite()
{
    d_cache.d_rwlock.lockWrite();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void Cache_TestUtil<KEY, VALUE, HASH, EQUAL>::unlock()
{
    d_cache.d_rwlock.unlock();
}

}  // close package namespace


namespace bslma {

template <class KEY,  class VALUE,  class HASH,  class EQUAL>
struct UsesBslmaAllocator<bdlcc::Cache<KEY, VALUE, HASH, EQUAL> >
    : bsl::true_type
{
};

}  // close namespace bslma

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
