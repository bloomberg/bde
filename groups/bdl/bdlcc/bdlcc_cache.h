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
// The cache class template has the same template parameters as
// 'bsl::unordered_map', with exception of not supporting the standard
// allocator template parameter (though 'bslma::Allocator' is supported): the
// key type ('KEYTYPE'), value type ('VALUE'), and the optional hash function
// ('HASH') and the key equal function ('EQUAL').
//
// The maximum cache size can be controlled by setting the low watermark and
// high watermark attributes, which is used instead of a single maximum size
// attribute for the sake of potential performance benefits.  Eviction of
// cached items happens when 'size() >= highWatermark' and stops when
// 'size() == lowWatermark - 1'.  Setting a fixed maximum size can be
// accomplished by setting the high and low watermarks to the same value.
//
// Two kinds of eviction policies are supported because they work efficiently
// with the cache class template's simple internal representation: LRU (Least
// Recently Used), and FIFO (First In, First Out).  With LRU, the item that has
// *not* been accessed for the longest period of time will be evicted first.
// With FIFO, the eviction order is based purely on the order of insertion,
// with the earliest inserted item being evicted first.
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
// The thread safety of 'bdlcc::Cache' is achieved by using
// 'bslmt::ReaderWriterLock', which gives *write* access priority over *read*
// access but allows concurrent *read* access.  Care should be taken to avoid
// excessive thread contention by not acquiring multiple write locks
// concurrently.
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
// function for every item in the cache.  If the supplied visitor is expensive
// or the cache is very large, calls to modifier methods might be starved until
// the 'visit' method ends.  Therefore, the 'visit' method should be used
// carefully by making the method call relatively cheap or ensuring that no
// time-sensitive write operation is done at the same time as a call to the
// 'visit' method.  A 'visit' method call can be made inexpensive either by
// using a supplied visitor that does only simple operations or ending the
// 'visit' method early by returning 'false' from the supplied visitor.
//
///Post-evction Callback and Potential Deadlocks
///---------------------------------------------
// When an item is evicted or erased from the cache, the previously set
// post-eviction callback (via the 'setPostEvictionCallback' method ) will be
// evoked, supplying item being removed.
//
// The cache object itself should not be used in a post-eviction callback;
// otherwise, a deadlock may result.  Since a write lock is held during the
// call to the callback, invoking any operation on the cache that acquires a
// lock inside the callback will lead to a deadlock.
//
///Runtime Complexity
///------------------
// +----------------------------------------------------+--------------------+
// | Operation                                          | Complexity         |
// +====================================================+====================+
// | insert                                             | O[1]               |
// | tryGetValue                                        | O[1]               |
// | popFront                                           | O[1]               |
// | erase                                              | O[1]               |
// +----------------------------------------------------+--------------------+
// | visit                                              | O[n]               |
// +----------------------------------------------------+--------------------+
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
//  bdlcc::Cache<int, bsl::string> myCache(bdlcc::CacheEvictionPolicy::e_LRU,
//                                        3, 4);
//..
// Next, we insert 3 items into the cache and verify that the size of the cache
// has been updated correctly:
//..
//  myCache.insert(0, "Alex");
//  myCache.insert(1, "John");
//  myCache.insert(2, "Rob");
//  assert(myCache.size() == 3);
//..
// Then, we retrieve the second value of the second item stored in the cache
// using the 'tryGetValue' method:
//..
//  bsl::shared_ptr<bsl::string> value;
//  int rc = myCache.tryGetValue(&value, 1);
//  assert(rc == 0);
//  assert(*value == "John");
//..
// Next, we set the cache's post-eviction callback to 'myPostEvictionCallback':
//..
//  myCache.setPostEvictionCallback(myPostEvictionCallback);
//..
// Now, we insert two more items into the cache to trigger the eviction
// behavior:
//..
//  myCache.insert(3, "Steve");
//  assert(myCache.size() == 4);
//  myCache.insert(4, "Tim");
//  assert(myCache.size() == 3);
//..
// Notice that after we insert "Steve", the size of the cache is 4, the high
// watermark.  After the following item, "Tim", is inserted, the size of the
// cache goes back down to 3, the low watermark.
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
//  MyValue retrieveValue(int key);
//..
// Next, we define a visitor type to aggregate keys of the out-of-date values
// in the cache.
//..
//  struct MyVisitor {
//      // Visitor to 'MyCache'.
//
//      std::vector<int>  d_oldKeys;  // list of out-of-date keys
//
//      MyVisitor()
//      : d_oldKeys(&talloc)
//      {}
//
//      bool operator() (int key, const MyValue& value)
//          // Bdlcck whether the specified 'value' is older than 1 hour.  If
//          // so, insert the specified 'key' into 'd_oldKeys'.
//      {
//          if (bdlt::CurrentTime::utc() - value.d_timestamp <
//              bdlt::DatetimeInterval(0, 60)) {
//              return false;
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
//          // Bdlcck and update for old values once per minute.
//
//          bslmt::ThreadUtil::microSleep(0, 60);
//          MyVisitor visitor;
//          cache->visit(visitor);
//
//          for (std::vector<int>::const_iterator itr =
//                                                   visitor.d_oldKeys.begin();
//               itr != visitor.d_oldKeys.end(); ++itr) {
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
//  MyCache myCache(bdlcc::CacheEvictionPolicy::e_FIFO, 100, 120, &talloc);
//
//  // Pre-populate the cache.
//  myCache.insert(0, retrieveValue(0));
//  ...
//
//  bslmt::ThreadUtil::Handle myWorkerHandle;
//  int rc = bslmt::ThreadUtil::create(&myWorkerHandle,
//                                     myWorkerThread,
//                                     &myCache);
//  assert(rc == 0);
//
//  // Do some work.
//  ...
//
//  myCache.clear();
//  assert(myCache.size() == 0);
//  bslmt::ThreadUtil::join(myWorkerHandle);
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

#ifndef INCLUDED_BSLMT_READERWRITERLOCK
#include <bslmt_readerwriterlock.h>
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

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_IOSTREAM
#include <bsl_iostream.h>
#endif

#ifndef INCLUDED_BSL_LIMITS
#include <bsl_limits.h>
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

template <class KEYTYPE>
class Cache_QueueProctor {
    // This class implements a proctor that, on destruction, removes the last
    // element of a 'QueueType' object.  This proctor is intended to work with
    // 'bdlcc::Cache' to provide the basic exception safety guarantee.

    // DATA
    bsl::list<KEYTYPE> *d_queue_p;  // queue (held, not owned)

  public:
    // CREATORS
    explicit Cache_QueueProctor(bsl::list<KEYTYPE> *queue);
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

template <class KEYTYPE,
          class VALUE,
          class HASH  = bsl::hash<KEYTYPE>,
          class EQUAL = bsl::equal_to<KEYTYPE> >
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

  private:
    // PRIVATE TYPES
    typedef bsl::list<KEYTYPE>                                    QueueType;
        // Eviction queue type.

    typedef bsl::pair<ValuePtrType, typename QueueType::iterator> MapVALUE;
        // Value type of the hash map.

    typedef bsl::unordered_map<KEYTYPE, MapVALUE, HASH, EQUAL>
                                                                  MapType;
        // Hash map type.

    // typedef bslmt::ReaderWriterMutex                              LockType;
    typedef bslmt::ReaderWriterLock                              LockType;

    // DATA
    bslma::Allocator                *d_allocator_p;  // memory allocator (held,
                                                     // not owned)

    mutable LockType                 d_rwlock;
    // mutable bslmt::ReaderWriterLock  d_rwlock;  // reader-writer lock

    MapType                          d_map;     // hash table storing key-value
                                                // pairs

    QueueType                        d_queue;   // queue storing eviction order
                                                // of keys, the key of the
                                                // first item to be evicted is
                                                // at the front of the queue

    CacheEvictionPolicy::Enum        d_evictionPolicy;  // eviction policy

    bsl::size_t                      d_lowWatermark;  // the size of this cache
                                                      // when eviction stops

    bsl::size_t                      d_highWatermark;  // the size of this
                                                       // cache when eviction
                                                       // starts after an
                                                       // insert

    PostEvictionCallback             d_postEvictionCallback;  // the function
                                                              // to call after
                                                              // a value has
                                                              // been evicted
                                                              // from the
                                                              // cache

    // PRIVATE MANIPULATORS
    void enforceHighWatermark();
        // Evict items from this cache if 'size() >= highWatermark()' until
        // 'size() == lowWatermark() - 1' beginning from the front of the
        // eviction queue.  Invoke the post-eviction callback for each item
        // evicted.

    void evictItem(const typename MapType::iterator& mapIt);
        // Evict the item at the specified 'mapIt' and invoke the post-eviction
        // callback for that item.

    void insertImp(const KEYTYPE&   key,
                   const VALUE&     value,
                   bsl::true_type);
    void insertImp(const KEYTYPE&   key,
                   const VALUE&     value,
                   bsl::false_type);
        // Insert the specified 'key' and its associated 'value' into this
        // cache.  If 'key' already exists, then its value will be replaced
        // with 'value'.  The last parameter should be 'true_type' if
        // 'VALUE' uses a 'bslma::Allocator'.

    // NOT IMPLEMENTED
    Cache(const Cache<KEYTYPE, VALUE, HASH, EQUAL>&);
    Cache<KEYTYPE, VALUE, HASH, EQUAL>& operator=(
                                           const Cache<KEYTYPE, VALUE, HASH>&);

  public:
    // CREATORS
    explicit Cache(bslma::Allocator *basicAllocator = 0);
        // Create an empty LRU cache having effectively no size limit.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

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
        // Create an empty cache using the specified 'evictionPolicy' and the
        // specified 'lowWatermark', 'highWatermark'.  The specified
        // 'hashFunction' used to generate the hash values for a given key, and
        // the specified 'equalFunction' is used to determine whether two keys
        // have the same value.  Optionally specify the 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // 'lowWatermark <= highWatermark', '1 <= lowWatermark', and
        // '1 <= highWatermark'.

    // ~Cache() = default;
        // Destroy this object.

    // MANIPULATORS
    void clear();
        // Remove all items from this cache.  Do *not* invoke the post-eviction
        // callback.

    int erase(const KEYTYPE& key);
        // Remove the item having the specified 'key' from this cache.  Invoke
        // the post-eviction callback for the removed item.  Return 0 on
        // success and 1 if 'key' does not exist.

    void insert(const KEYTYPE& key, const VALUE& value);
        // Insert the specified 'key' and its associated 'value' into this
        // cache.  If 'key' already exists, then its value will be replaced
        // with 'value'.

    void insert(const KEYTYPE& key, const ValuePtrType& valuePtr);
        // Insert the specified 'key' and its associated 'valuePtr' into this
        // cache.  If 'key' already exists, then its value will be replaced
        // with 'value'.

    int popFront();
        // Remove the item at the front of the eviction queue.  Invoke the
        // post-eviction callback for the removed item.  Return 0 on success
        // and 1 if this cache is empty.

    void setPostEvictionCallback(PostEvictionCallback postEvictionCallback);
        // Set the post-eviction callback to the specified
        // 'postEvictionCallback'.  The post-eviction callback is invoked for
        // each item evicted or removed from this cache.

    int tryGetValue(bsl::shared_ptr<VALUE> *value,
                    const KEYTYPE&          key,
                    bool                    modifyEvictionQueue = true);
        // Load, into the specified *'value', the value of the specified 'key'
        // in this cache.  If the optionally specified 'modifyEvictionQueue' is
        // 'true' and the eviction policy is LRU, then move the cached item to
        // the back of the eviction queue.  Return 0 on success and 1 if 'key'
        // does not exist.  Note that a write lock is acquired only if eviction
        // queue needs to be modified.

    // ACCESSORS
    EQUAL equalFunction() const;
        // Return (a copy of) binary the key-equality functor used by this
        // Cache that returns 'true' if two 'KEYTYPE' objects have the same
        // value, and 'false' otherwise.

    CacheEvictionPolicy::Enum evictionPolicy() const;
        // Return the eviction policy used by this cache.

    HASH hashFunction() const;
        // Return (a copy of) the unary hash functor used by this cache to
        // generate a hash value (of type 'std::size_t') for a 'KEYTYPE'
        // object.

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
        // the same way as the function 'bool (const KEYTYPE&, const VALUE&)'

};

// FREE OPERATORS

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ------------------------
                        // class Cache_QueueProctor
                        // ------------------------

// CREATORS
template <class KEYTYPE>
Cache_QueueProctor<KEYTYPE>::Cache_QueueProctor(
                                  bsl::list<KEYTYPE> *queue) : d_queue_p(queue)
{
}

template <class KEYTYPE>
Cache_QueueProctor<KEYTYPE>::~Cache_QueueProctor()
{
    if (d_queue_p) {
        d_queue_p->pop_back();
    }
}

// MANIPULATORS
template <class KEYTYPE>
void Cache_QueueProctor<KEYTYPE>::release()
{
    d_queue_p = 0;
}

                        // -----------
                        // class Cache
                        // -----------

// CREATORS
template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
Cache<KEYTYPE, VALUE, HASH, EQUAL>::Cache(bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_map(d_allocator_p)
, d_queue(d_allocator_p)
, d_evictionPolicy(CacheEvictionPolicy::e_LRU)
, d_lowWatermark(bsl::numeric_limits<bsl::size_t>::max())
, d_highWatermark(bsl::numeric_limits<bsl::size_t>::max())
{
}

template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
Cache<KEYTYPE, VALUE, HASH, EQUAL>::Cache(
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
{
    BSLS_ASSERT_SAFE(lowWatermark <= highWatermark);
    BSLS_ASSERT_SAFE(1 <= lowWatermark);
    BSLS_ASSERT_SAFE(1 <= highWatermark);
}

template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
Cache<KEYTYPE, VALUE, HASH, EQUAL>::Cache(
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
{
    BSLS_ASSERT_SAFE(lowWatermark <= highWatermark);
    BSLS_ASSERT_SAFE(1 <= lowWatermark);
    BSLS_ASSERT_SAFE(1 <= highWatermark);
}

// PRIVATE MANIPULATORS
template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
void Cache<KEYTYPE, VALUE, HASH, EQUAL>::enforceHighWatermark()
{
    if (d_map.size() < d_highWatermark) {
        return;                                                       // RETURN
    }

    while (size() >= d_lowWatermark && size() > 0) {
        const typename MapType::iterator mapIt = d_map.find(d_queue.front());
        BSLS_ASSERT(mapIt != d_map.end());
        evictItem(mapIt);
    }
}

template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
int Cache<KEYTYPE, VALUE, HASH, EQUAL>::erase(const KEYTYPE& key)
{
    bslmt::WriteLockGuard<LockType> guard(&d_rwlock);

    const typename MapType::iterator mapIt = d_map.find(key);
    if (mapIt == d_map.end()) {
        return 1;                                                     // RETURN
    }

    evictItem(mapIt);
    return 0;
}

template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
void Cache<KEYTYPE, VALUE, HASH, EQUAL>::evictItem(
                                       const typename MapType::iterator& mapIt)
{
    ValuePtrType value = mapIt->second.first;

    d_queue.erase(mapIt->second.second);
    d_map.erase(mapIt);

    if (d_postEvictionCallback) {
        d_postEvictionCallback(value);
    }
}

template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
void Cache<KEYTYPE, VALUE, HASH, EQUAL>::insertImp(const KEYTYPE&  key,
                                                   const VALUE&    value,
                                                   bsl::false_type)
{
    ValuePtrType valuePtr;
    valuePtr.createInplace(d_allocator_p, value);
    insert(key, valuePtr);
}

template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
void Cache<KEYTYPE, VALUE, HASH, EQUAL>::insertImp(const KEYTYPE& key,
                                                   const VALUE&   value,
                                                   bsl::true_type)
{
    ValuePtrType valuePtr;
    valuePtr.createInplace(d_allocator_p, value, d_allocator_p);
    insert(key, valuePtr);
}

// MANIPULATORS
template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
void Cache<KEYTYPE, VALUE, HASH, EQUAL>::clear()
{
    bslmt::WriteLockGuard<LockType> guard(&d_rwlock);
    d_map.clear();
    d_queue.clear();
}

template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
void Cache<KEYTYPE, VALUE, HASH, EQUAL>::insert(const KEYTYPE& key,
                                                const VALUE&   value)
{
    insertImp(key, value, bslma::UsesBslmaAllocator<VALUE>());
}

template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
void Cache<KEYTYPE, VALUE, HASH, EQUAL>::insert(
                                                  const KEYTYPE&      key,
                                                  const ValuePtrType& valuePtr)
{
    bslmt::WriteLockGuard<LockType> guard(&d_rwlock);

    enforceHighWatermark();

    typename MapType::iterator mapIt = d_map.find(key);
    if (mapIt != d_map.end()) {
        mapIt->second.first = valuePtr;
        typename QueueType::iterator queueItr = mapIt->second.second;

        d_queue.splice(d_queue.end(), d_queue, queueItr);
    }
    else {
        d_queue.push_back(key);
        Cache_QueueProctor<KEYTYPE>  proctor(&d_queue);
        typename QueueType::iterator queueItr = d_queue.end();
        --queueItr;

        d_map.emplace(key, MapVALUE(valuePtr, queueItr, d_allocator_p));
        proctor.release();
    }
}

template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
int Cache<KEYTYPE, VALUE, HASH, EQUAL>::popFront()
{
    bslmt::WriteLockGuard<LockType> guard(&d_rwlock);

    if (size() > 0) {
        const typename MapType::iterator mapIt = d_map.find(d_queue.front());
        BSLS_ASSERT(mapIt != d_map.end());
        evictItem(mapIt);
        return 0;                                                     // RETURN
    }

    return 1;
}

template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
void Cache<KEYTYPE, VALUE, HASH, EQUAL>::setPostEvictionCallback(
                                     PostEvictionCallback postEvictionCallback)
{
    bslmt::WriteLockGuard<LockType> guard(&d_rwlock);
    d_postEvictionCallback = postEvictionCallback;
}

template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
int Cache<KEYTYPE, VALUE, HASH, EQUAL>::tryGetValue(
                                   bsl::shared_ptr<VALUE> *value,
                                   const KEYTYPE&          key,
                                   bool                    modifyEvictionQueue)
{
    //..
    // int preLocked = 1;
    // if (d_evictionPolicy == CacheEvictionPolicy::e_FIFO ||
    //     !modifyEvictionQueue) {
    //     preLocked = 0;
    // }
    // else {  //(d_evictionPolicy == CacheEvictionPolicy::e_LRU) {
    //     d_rwlock.lockReadReserveWrite();
    // }
    //..

    // bslmt::ReadLockGuard<LockType> guard(&d_rwlock);
    bslmt::WriteLockGuard<LockType> guard(&d_rwlock);

    typename MapType::iterator mapIt = d_map.find(key);
    if (mapIt == d_map.end()) {
        return 1;                                                     // RETURN
    }

    *value = mapIt->second.first;

    // if (d_evictionPolicy == CacheEvictionPolicy::e_LRU &&
    //     modifyEvictionQueue) {
    //     typename QueueType::iterator queueItr = mapIt->second.second;
    //     typename QueueType::iterator last = d_queue.end();
    //     --last;
    //     if (last != queueItr) {
    //         d_rwlock.upgradeToWriteLock();
    //         d_queue.splice(d_queue.end(), d_queue, queueItr);
    //     }
    // }

    return 0;
}

// ACCESSORS
template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
EQUAL Cache<KEYTYPE, VALUE, HASH, EQUAL>::equalFunction() const
{
    return d_map.key_eq();
}

template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
CacheEvictionPolicy::Enum
Cache<KEYTYPE, VALUE, HASH, EQUAL>::evictionPolicy() const
{
    return d_evictionPolicy;
}

template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
HASH Cache<KEYTYPE, VALUE, HASH, EQUAL>::hashFunction() const
{
    return d_map.hash_function();
}

template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
bsl::size_t Cache<KEYTYPE, VALUE, HASH, EQUAL>::highWatermark() const
{
    return d_highWatermark;
}

template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
bsl::size_t Cache<KEYTYPE, VALUE, HASH, EQUAL>::lowWatermark() const
{
    return d_lowWatermark;
}

template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
bsl::size_t Cache<KEYTYPE, VALUE, HASH, EQUAL>::size() const
{
    return d_map.size();
}

template <class KEYTYPE, class VALUE, class HASH, class EQUAL>
template <class VISITOR>
void Cache<KEYTYPE, VALUE, HASH, EQUAL>::visit(VISITOR& visitor) const
{
    // bslmt::ReadLockGuard<bslmt::ReaderWriterLock> guard(&d_rwlock);
    bslmt::ReadLockGuard<LockType> guard(&d_rwlock);

    for (typename QueueType::const_iterator queueItr = d_queue.begin();
         queueItr != d_queue.end(); ++queueItr) {

        const KEYTYPE&                         key = *queueItr;
        const typename MapType::const_iterator mapIt = d_map.find(key);
        BSLS_ASSERT(mapIt != d_map.end());
        const ValuePtrType& valuePtr = mapIt->second.first;

        if (!visitor(key, *valuePtr)) {
            break;
        }
    }
}

}  // close package namespace


namespace bslma {

template <class KEYTYPE,  class VALUE,  class HASH,  class EQUAL>
struct UsesBslmaAllocator<bdlcc::Cache<KEYTYPE, VALUE, HASH, EQUAL> >
    : bsl::true_type
{
};

}  // close namespace bslma

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
