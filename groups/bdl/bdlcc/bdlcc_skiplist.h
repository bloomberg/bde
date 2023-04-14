// bdlcc_skiplist.h                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLCC_SKIPLIST
#define INCLUDED_BDLCC_SKIPLIST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a generic thread-safe Skip List.
//
//@CLASSES:
//  bdlcc::SkipList:           generic thread-aware ordered map
//  bdlcc::SkipListPair:       type for opaque pointers
//  bdlcc::SkipListPairHandle: scope mechanism for safe item references
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a thread-safe value-semantic
// associative Skip List container.  A Skip List stores objects of a
// parameterized 'DATA' type, ordered by values of a parameterized 'KEY' type.
// 'DATA' objects can be added, looked up, and removed quickly on the basis of
// their 'KEY' value.  In addition, 'bdlcc::SkipList' provides methods to
// change the 'KEY' value associated with an object in the list such that it is
// efficiently moved to an appropriate location within the list for the new
// 'KEY' value.
//
// Associations (pairings of data objects with key values) in the list are
// identified by 'bdlcc::SkipListPairHandle' objects or 'bdlcc::SkipListPair'
// pointers.  'bdlcc::SkipListPair' pointers must be used with caution: See the
// "'bdlcc::SkipListPair' Usage Rules" below.  'bdlcc::SkipListPair' and
// 'bdlcc::SkipListPairHandle' objects are optionally populated when new
// associations are added, and are also populated whenever associations are
// looked up (either by key or by position).  Note that in addition to
// 'addPairReferenceRaw', member functions of 'bdlcc::SkipList' such as
// 'front', 'back', and 'find' also add a reference to the specified element.
//
///Template Requirements
///---------------------
// The 'bdlcc::SkipList' ordered associative container is parameterized on two
// types, 'KEY' and 'DATA'.  Each type must have a public copy constructor, and
// it is important to declare the "Uses bslma Allocator" trait if the type
// accepts a 'bslma::Allocator' in its constructor (see 'bslalg_typetraits').
// In addition, operators '=', '<', and '==' must be defined for the type
// 'KEY'; for correct behavior, operator '<' must define a Strict Weak Ordering
// on 'KEY' values.
//
///Glossary
///--------
// Some terms used frequently in this documentation:
//
//: Back:
//:     The last element in the list.  The key value at the back is greater
//:     than or equal to every other key value in the list.
//:
//: Front:
//:     The beginning of the list.  The key value at the front is less than or
//:     equal to every other key value in the list.
//:
//: Pair:
//:     An element of the list; a pairing (association) of a data object with a
//:     key value.  Also a type name used for *references* to such objects
//:     ('bdlcc::SkipListPair' objects cannot be constructed directly).
//:
//: PairHandle:
//:     An object (of type 'bdlcc::SkipListPairHandle') with scope and copy
//:     semantics that makes it easier to manage and use than a raw
//:     'bdlcc::SkipListPair *'.
//:
//: R:
//:     Stands for "Reverse search" (see '"R" Methods' documentation below).
//:
//: Reference:
//:     An object referring to a pair; either a 'bdlcc::SkipListPair *' which
//:     has not yet been released, or a 'bdlcc::SkipListPairHandle' object.
//
///"R" Methods: Optimized Search From The Back Of The List
///-------------------------------------------------------
// The regular methods (no R suffix) of 'bdlcc::SkipList' that result in a
// search through the list, search from the front of the list (i.e., in
// ascending order).
//
// All methods of 'bdlcc::SkipList' that result in a search through the list
// have corresponding "R" versions: for example, there are 'add' and 'addR'
// methods, 'find' and 'findR' methods, etc.  The "R" versions of these methods
// search from the back of the list (i.e., in descending (reverse) order).  Use
// of an "R" method is a hint to the Skip List that the desired key is more
// likely to be near the back than the front.  In the event of duplicate keys,
// 'find' will find the first matching key, and 'findR' will find the last
// matching key.  Note that if there are pairs in the list with duplicate keys,
// the specific pair found by 'find' may (or may not) be different from the one
// found by 'findR'.
//
///Referring to Elements in the Container
///--------------------------------------
// 'bdlcc::SkipList' has two 'handle' types for referring to elements in the
// container:
//
//: o 'bdlcc::SkipList::Pair *' -- raw pointer, no destructor,
//:   'bdlcc::SkipList::releaseReferenceRaw' must be called on these pointers
//:   before the container is destroyed.
//:
//: o 'bdlcc::SkipList::PairHandle' -- 'class', has a destructor which will
//:   release the pair handle when it goes out of scope via RAII.  If the pair
//:   handle will not be destroyed before the container is, it is necessary to
//:   call 'bdlcc::SkipList::PairHandle::release' before the container is
//:   destroyed.
//
// The 'PairHandle' type has an implicit conversion to 'Pair *'.  In most cases
// 'bdlcc::SkipList' provides dual functions supporting 'Pair *' and
// 'PairHandle'.  Some functions, however, only support 'Pair *' parameters;
// for these functions, either a 'Pair *' or a 'PairHandle' may be passed.
//
// Unless the client has some reason to prefer the 'Pair *' interface, the
// 'PairHandle' interface is recommended since it provides RAII, making it
// harder to leak nodes.
//
// Note that in some build modes, 'SkipList' will attempt to detect leaked
// nodes, i.e., those that were referred to by 'Pair *'s for which
// 'releaseReferenceRaw' hasn't been called, and nodes referred to by
// 'PairHandle's that haven't been destroyed or 'release'd at the time of the
// skip list's destruction.
//
///Thread Safety
///-------------
// 'bdlcc::SkipList' is thread-safe and thread-aware; that is, multiple threads
// may use their own Skip List objects or may concurrently use the same object.
//
// Note that safe usage of the component depends upon correct usage of
// 'bdlcc::SkipListPair' objects (see above).
//
// 'bdlcc::SkipListPairHandle' is only *const* *thread-safe*.  It is not safe
// for multiple threads to invoke non-'const' methods on the same 'PairHandle'
// object concurrently.
//
// 'bdlcc::SkipListPair' is a name used for opaque pointers; the concept of
// thread safety does not apply to it.
//
///Exception Safety
///----------------
// 'bdlcc::SkipList' is exception-neutral: no method invokes 'throw' or
// 'catch'.  Insertion methods ('add', 'addR', etc) invoke the copy
// constructors of the contained 'KEY' and 'DATA' types; if those constructors
// throw an exception, the list provides a full rollback guarantee (it will
// have the same state it had prior to the call to 'add').  The assignment
// operator may also indirectly cause 'bad_alloc' to be thrown if the system is
// out of memory, but in that case there is *no* guarantee of rollback on the
// left-hand list.
//
// No method of 'bdlcc::SkipListPairHandle' can throw.
//
// 'bdlcc::SkipListPair' is only a name used for opaque pointers; the concept
// of exception safety does not apply to it.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a Scheduler
///- - - - - - - - - - - - - - - -
// The "R" methods of 'bdlcc::SkipList' make it ideal for use in a scheduler,
// in which events are likely to be scheduled after existing events.  In such
// an implementation, events are stored in the list with their scheduled
// execution times as 'KEY' objects: Searching near the end of the list for the
// right location for new events, and removing events from the front of the
// list for execution, are very efficient operations.  Being thread- enabled
// also makes 'bdlcc::SkipList' well-suited to use in a scheduler - a
// "dispatcher" thread can safety use the list at the same time that events are
// being scheduled from other threads.  The following is an implementation of a
// simple scheduler class using 'bdlcc::SkipList'.  Note that the mutex in the
// scheduler is used only in connection with the scheduler's condition variable
// - thread-safe access to the 'bdlcc::SkipList' object does *not* require any
// synchronization.
//..
//  class SimpleScheduler
//  {
//      // TYPES
//      typedef bdlcc::SkipList<bdlt::Datetime, bsl::function<void()> > List;
//
//      // DATA
//      List                       d_list;
//      bslmt::ThreadUtil::Handle  d_dispatcher;
//      bslmt::Condition           d_notEmptyCond;
//      bslmt::Condition           d_emptyCond;
//      bslmt::Barrier             d_startBarrier;
//      bslmt::Mutex               d_condMutex;
//      bsls::AtomicInt            d_doneFlag;
//
//    private:
//      // NOT IMPLEMENTED
//      SimpleScheduler(const SimpleScheduler&);
//
//    private:
//      // PRIVATE MANIPULATORS
//      void dispatcherThread()
//          // Run a thread that executes functions off 'd_list'.
//      {
//          d_startBarrier.wait();
//
//          while (!d_doneFlag) {
//              List::PairHandle firstItem;
//              if (0 == d_list.front(&firstItem)) {
//                  // The list is not empty.
//
//                  bsls::TimeInterval when =
//                      bdlt::IntervalConversionUtil::convertToTimeInterval(
//                                 firstItem.key() - bdlt::CurrentTime::utc());
//                  if (when.totalSecondsAsDouble() <= 0) {
//                      // Execute now and remove from schedule, then iterate.
//
//                      d_list.remove(firstItem);
//                      firstItem.data()();
//
//                      List::PairHandle tmpItem;
//
//                      bslmt::LockGuard<bslmt::Mutex> guard(&d_condMutex);
//
//                      if (0 == d_list.length()) {
//                          d_emptyCond.broadcast();
//                      }
//                  }
//                  else {
//                      // Wait until the first scheduled item is due.
//
//                      bslmt::LockGuard<bslmt::Mutex> guard(&d_condMutex);
//                      List::PairHandle newFirst;
//                      if (!d_doneFlag && (0 != d_list.front(&newFirst) ||
//                                        newFirst.key() == firstItem.key())) {
//                          d_notEmptyCond.timedWait(&d_condMutex,
//                                            bdlt::CurrentTime::now() + when);
//                      }
//                  }
//              }
//              else {
//                  // The list is empty; wait on the condition variable.
//
//                  bslmt::LockGuard<bslmt::Mutex> guard(&d_condMutex);
//                  if (d_list.isEmpty() && !d_doneFlag) {
//                      d_notEmptyCond.wait(&d_condMutex);
//                  }
//              }
//          }
//      }
//
//    public:
//      // CREATORS
//      explicit
//      SimpleScheduler(bslma::Allocator *basicAllocator = 0)
//      : d_list(basicAllocator)
//      , d_startBarrier(2)
//      , d_doneFlag(false)
//          // Creator.
//      {
//          int rc = bslmt::ThreadUtil::create(
//                  &d_dispatcher,
//                  bdlf::BindUtil::bind(&SimpleScheduler::dispatcherThread,
//                                          this));
//          BSLS_ASSERT(0 == rc);  (void)rc;
//          d_startBarrier.wait();
//      }
//
//      ~SimpleScheduler()
//          // d'tor
//      {
//          stop();
//      }
//
//      // MANIPULATORS
//      void drain()
//          // Block until the scheduler has no jobs.
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_condMutex);
//
//          while (!d_doneFlag && 0 != d_list.length()) {
//              d_emptyCond.wait(&d_condMutex);
//          }
//      }
//
//      void scheduleEvent(const bsl::function<void()>& event,
//                         const bdlt::Datetime&        when)
//          // Schedule the specified 'event' to occur at the specified 'when'.
//      {
//          // Use 'addR' since this event will probably be placed near the end
//          // of the list.
//
//          bool newFrontFlag;
//          d_list.addR(when, event, &newFrontFlag);
//          if (newFrontFlag) {
//              // This event is scheduled before all other events.  Wake up
//              // the dispatcher thread.
//
//              d_notEmptyCond.signal();
//          }
//      }
//
//      void stop()
//          // Stop the scheduler.
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_condMutex);
//
//          d_list.removeAll();
//
//          d_doneFlag = true;
//          d_notEmptyCond.signal();
//          d_emptyCond.broadcast();
//
//          if (bslmt::ThreadUtil::invalidHandle() != d_dispatcher) {
//              bslmt::ThreadUtil::Handle dispatcher = d_dispatcher;
//              {
//                  bslmt::LockGuardUnlock<bslmt::Mutex> g(&d_condMutex);
//                  bslmt::ThreadUtil::join(dispatcher);
//              }
//              d_dispatcher = bslmt::ThreadUtil::invalidHandle();
//          }
//      }
//  };
//..
// We can verify the correct behavior of 'SimpleScheduler'.  First, we need a
// wrapper around vector<int>::push_back, since this function is overloaded and
// cannot be bound directly:
//..
//  void pushBackWrapper(bsl::vector<int> *vector, int item)
//      // Push the specified 'item' onto the specified 'vector'.
//  {
//      vector->push_back(item);
//  }
//..
// Now, in 'main', verify that the scheduler executes events when expected:
//..
//  SimpleScheduler      scheduler;
//
//  bsl::vector<int>     values;
//
//  const bdlt::Datetime start = bdlt::CurrentTime::utc();
//  bdlt::Datetime       scheduleTime;
//..
// Add events out of sequence and ensure they are executed in the proper order.
//..
//  scheduleTime = start;
//  scheduleTime.addMilliseconds(2250);
//  scheduler.scheduleEvent(bdlf::BindUtil::bind(&pushBackWrapper, &values, 2),
//                          scheduleTime);
//
//  scheduleTime = start;
//  scheduleTime.addMilliseconds(750);
//  scheduler.scheduleEvent(bdlf::BindUtil::bind(&pushBackWrapper, &values, 0),
//                          scheduleTime);
//
//  scheduleTime = start;
//  scheduleTime.addMilliseconds(1500);
//  scheduler.scheduleEvent(bdlf::BindUtil::bind(&pushBackWrapper, &values, 1),
//                          scheduleTime);
//
//  assert(values.empty());
//
//  scheduler.drain();
//
//  bdlt::Datetime finish = bdlt::CurrentTime::utc();
//
//  assert(3 == values.size());
//  assert(0 == values[0]);
//  assert(1 == values[1]);
//  assert(2 == values[2]);
//
//  const double elapsed = bdlt::IntervalConversionUtil::convertToTimeInterval(
//                                      finish - start).totalSecondsAsDouble();
//
//  assert(2.25 <= elapsed);
//  assert(elapsed < 2.75);
//..
// Note that the destructor of 'scheduler' will call 'stop()'.

#include <bdlscm_version.h>

#include <bdlb_print.h>
#include <bdlb_printmethods.h>

#include <bslmt_mutexassert.h>
#include <bslmt_lockguard.h>
#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>

#include <bslalg_scalarprimitives.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_conditional.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_review.h>
#include <bsls_types.h>
#include <bsls_util.h>

#include <bsl_algorithm.h>
#include <bsl_functional.h>
#include <bsl_ostream.h>
#include <bsl_vector.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslalg_typetraits.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
# include <memory_resource>
#endif

#include <vector>

namespace BloombergLP {
namespace bdlcc {

template <class KEY, class DATA>
class SkipList;


                         // =========================
                         // local class SkipList_Node
                         // =========================

template<class KEY, class DATA>
struct SkipList_Node {
    // This component-private structure is a node in the SkipList.

    // TYPES
    typedef SkipList_Node<KEY, DATA> Node;

    struct Ptrs {
        // PUBLIC DATA
        Node *d_next_p;
        Node *d_prev_p;
    };

    // PUBLIC DATA
    bsls::AtomicInt  d_refCount;
    int              d_level;      // values in range '[ 0 .. 31 ]'
    DATA             d_data;
    KEY              d_key;
    Ptrs             d_ptrs[1];    // Must be last; each node has space for
                                   // extra 'Ptrs' allocated based on its
                                   // level.
};

                   // ====================================
                   // local class SkipList_DoubleLockGuard
                   // ====================================

class SkipList_DoubleLockGuard {
    // DATA
    bslmt::LockGuard<bslmt::Mutex> d_firstGuard, d_lastGuard;

  public:
    // CREATOR
    SkipList_DoubleLockGuard(bslmt::Mutex *lock1, bslmt::Mutex *lock2);
        // Lock both 'lock1' and 'lock2', the one in the lower memory location
        // first.
};

                 // =========================================
                 // local class SkipList_RandomLevelGenerator
                 // =========================================

class SkipList_RandomLevelGenerator {
    // This component-private class handles randomizing the levelization of
    // list nodes.

    // PRIVATE TYPES
    enum {
        k_MAX_LEVEL      = 31,         // Also defined in SkipList and
                                       // PoolManager
        k_SEED           = 0x12b9b0a1  // arbitrary

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BCEC_MAX_LEVEL = k_MAX_LEVEL
      , BCEC_SEED = k_SEED
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    // DATA
    bsls::AtomicInt d_seed;        // current random seed

    bsls::AtomicInt d_randomBits;  // 14 random bits and a sentinel bit at the
                                   // 15th position

  public:
    // CREATORS
    SkipList_RandomLevelGenerator();
        // Construct a thread-aware random-level generator.

    // MANIPULATORS
    int randomLevel();
        // Return a random integer between 0 and k_MAX_LEVEL.
};

                    // ====================================
                    // local class bdlcc::SkipList_PoolUtil
                    // ====================================

class SkipList_PoolManager;

struct SkipList_PoolUtil {
    // This component-private utility handles the lock-free pool of list nodes.

    // TYPES
    typedef SkipList_PoolManager PoolManager;

    // CLASS METHODS
    static void *allocate(PoolManager *poolManager, int level);
        // Reserve sufficient space for a node at the specified 'level' from
        // the specified 'poolManager', and return the address of the reserved
        // memory.

    static PoolManager *createPoolManager(int              *objectSizes,
                                          int               numLevels,
                                          bslma::Allocator *basicAllocator);
        // Create a new pooled node allocator that manages nodes up to the
        // specified 'numLevels' as described by the specified 'objectSizes'.
        // For 'i' in '[0, numLevels)', a node at level 'i' will have size
        // 'objectSizes[i]' bytes.  Use the specified 'basicAllocator' to
        // supply memory.  Return the address of the new allocator.  Note that
        // the behavior is undefined if 'basicAllocator' is 0.

    static void deallocate(PoolManager *poolManager, void *address);
        // Return the node at the specified 'address' to the specified
        // 'poolManager'.  The behavior is undefined if 'address' was not
        // allocated from 'poolManager'.

    static void deletePoolManager(bslma::Allocator *basicAllocator,
                                  PoolManager      *poolManager);
        // Destroy the specified 'poolManager' which was allocated from the
        // specified 'basicAllocator'.  The behavior is undefined if
        // 'poolManager' was not allocated from 'basicAllocator'.
};

                  // =======================================
                  // local class SkipList_NodeCreationHelper
                  // =======================================

template<class KEY, class DATA>
class SkipList_NodeCreationHelper {
    // This component-private structure is a scoped guard that initializes new
    // nodes and releases them in case of exception.

    // PRIVATE TYPES
    typedef SkipList_PoolManager     PoolManager;
    typedef SkipList_PoolUtil        PoolUtil;

    typedef SkipList_Node<KEY, DATA> Node;

    // DATA
    Node             *d_node_p;        // the node, or 0 if no managed node
    PoolManager      *d_poolManager_p; // pool from which node was allocated
    bool              d_keyFlag;       // 'true' if the key was constructed
    bslma::Allocator *d_allocator_p;   // held

    BSLMF_NESTED_TRAIT_DECLARATION(SkipList_NodeCreationHelper,
                                                    bslma::UsesBslmaAllocator);

  public:
    // CREATORS
    SkipList_NodeCreationHelper(PoolManager      *poolManager,
                                Node             *node,
                                bslma::Allocator *basicAllocator = 0);
        // Create a new scoped guard object to assist in exception-safe
        // initialization of the specified 'node', which was allocated from the
        // specified 'poolManager'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~SkipList_NodeCreationHelper();
        // Destroy this scoped guard.  If the guard currently manages a node,
        // destroy its data as necessary and return it to the pool.

    // MANIPULATORS
    void construct(const KEY& key, const DATA& data);
        // Attempt to copy-construct the specified 'key' and 'data' into the
        // node specified at construction; then release the node from
        // management.  Note that if an exception is thrown during the
        // invocation of either constructor, the node will remain under
        // management and thus the destructor of this object will do the
        // appropriate cleanup.  The behavior is undefined if 'construct' has
        // already been invoked on this scoped guard object.
};

                             // ==================
                             // class SkipListPair
                             // ==================

template <class KEY, class DATA>
class SkipListPair {
    // Pointers to objects of this class are used in the "raw" API of
    // 'SkipList'; however, objects of the class are never constructed as the
    // class serves only to provide type-safe pointers.
    //
    // In addition, this class defines 'key' and 'data' member functions that
    // pass 'this' to static methods of 'SkipList'.

    // Note these data elements are never accessed.  A pointer to this type
    // will be cast to a pointer to 'SkipList_Node' so make sure we are
    // adequately aligned to avoid compiler warnings.

    // DATA
    SkipList_Node<KEY, DATA> d_node;    // never directly accessed

  private:
    // NOT IMPLEMENTED
    SkipListPair();
    SkipListPair(const SkipListPair&);
    SkipListPair& operator=(const SkipListPair&);

  public:
    // ACCESSORS
    DATA& data() const;
        // Return a reference to the modifiable "data" of this pair.

    const KEY& key() const;
        // Return a reference to the non-modifiable "key" value of this pair.
};

                          // ========================
                          // class SkipListPairHandle
                          // ========================

template <class KEY, class DATA>
class SkipListPairHandle {
    // Objects of this class refer to an association (pair) in a 'SkipList'.  A
    // 'bdlcc::SkipListPairHandle' is implicitly convertible to a 'const Pair*'
    // and thus may be used anywhere in the 'SkipList' API that a 'const Pair*'
    // is expected.

    // PRIVATE TYPES
    typedef SkipListPair<KEY, DATA> Pair;

    // DATA
    SkipList<KEY, DATA> *d_list_p;
    Pair                *d_node_p;

    // FRIENDS
    friend class SkipList<KEY, DATA>;

  private:
    // PRIVATE CREATORS
    SkipListPairHandle(SkipList<KEY, DATA> *list, Pair *reference);
        // Construct a new pair handle for the specified 'list' that manages
        // the specified 'reference'.  Note that it is assumed that the
        // creating (calling) scope already owns the 'reference'.

    // PRIVATE MANIPULATORS
    void reset(const SkipList<KEY, DATA> *list, Pair *reference);
        // Change this 'SkipListPairHandle' to refer to manage the specified
        // 'reference' in the specified 'list'.  If this 'SkipListPairHandle'
        // refers to a pair, release the reference.  Note that it is assumed
        // that the calling scope already owns the 'reference'.

  public:
    // CREATORS
    SkipListPairHandle();
        // Construct a new PairHandle that does not refer to a pair.

    SkipListPairHandle(const SkipListPairHandle& original);
        // Construct a new pair reference for the same list and pair as the
        // specified 'original'.

    ~SkipListPairHandle();
        // Destroy this 'SkipListPairHandle'.  If this 'SkipListPairHandle'
        // refers to a pair in the list, release the reference.

    // MANIPULATORS
    SkipListPairHandle& operator=(const SkipListPairHandle& rhs);
        // Change this 'SkipListPairHandle' to refer to the same list and pair
        // as the specified 'rhs'.  If this 'SkipListPairHandle' initially
        // refers to a pair, release the reference.  Return '*this'.

    void release();
        // Release the reference (if any) managed by this 'SkipListPairHandle'.

    void releaseReferenceRaw(SkipList<KEY, DATA> **list, Pair **reference);
        // Invoke 'release' and populate the specified 'list' and 'reference'
        // pointers with the list and reference values of this
        // 'SkipListPairHandle'.

    // ACCESSORS
    operator const Pair*() const;
        // Return the address of the pair referred to by this
        // 'SkipListPairHandle', or 0 if this handle does not manage a
        // reference.

    DATA& data() const;
        // Return a reference to the "data" value of the pair referred to by
        // this object.  The behavior is undefined unless 'isValid' returns
        //  'true'.

    const KEY& key() const;
        // Return a reference to the non-modifiable "key" value of the pair
        // referred to by this object.  The behavior is undefined unless
        // 'isValid' returns 'true'.

    bool isValid() const;
        // Return 'true' if this PairHandle currently refers to a pair, and
        // 'false' otherwise.
};

                               // ==============
                               // class SkipList
                               // ==============

template<class KEY, class DATA>
class SkipList {
    // This class provides a generic thread-safe Skip List (an ordered
    // associative container).  It supports an almost complete set of *value*
    // *semantic* operations, including copy construction, assignment, equality
    // comparison, and 'ostream' printing (but not BDEX serialization).

  public:
    // CONSTANTS
    enum {
        e_SUCCESS   = 0,
        e_NOT_FOUND = 1,
        e_DUPLICATE = 2,
        e_INVALID   = 3

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BCEC_SUCCESS   = e_SUCCESS
      , BCEC_NOT_FOUND = e_NOT_FOUND
      , BCEC_DUPLICATE = e_DUPLICATE
      , BCEC_INVALID   = e_INVALID
      , RET_SUCCESS    = e_SUCCESS
      , RET_NOT_FOUND  = e_NOT_FOUND
      , RET_DUPLICATE  = e_DUPLICATE
      , RET_INVALID    = e_INVALID
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    // TYPES
    typedef SkipListPair<KEY, DATA>           Pair;
    typedef SkipListPairHandle<KEY, DATA>     PairHandle;

  private:
    // PRIVATE TYPES
    typedef SkipList_PoolManager              PoolManager;
    typedef SkipList_PoolUtil                 PoolUtil;

    typedef SkipList_Node<KEY, DATA>          Node;
    typedef SkipList_NodeCreationHelper<KEY, DATA>
                                              NodeGuard;

    typedef bslmt::Mutex                      Lock;
    typedef bslmt::LockGuard<bslmt::Mutex>    LockGuard;

    typedef SkipList_DoubleLockGuard          DoubleLockGuard;

    template <class VECTOR, class VALUE_TYPE>
    class IsVector;
    class PairFactory;
    class PairHandleFactory;

    // PRIVATE CONSTANTS
    enum {
        k_MAX_NUM_LEVELS = 32,       // Also defined in RandomLevelGenerator
                                     // and PoolManager

        k_MAX_LEVEL      = 31
    };

    // DATA
    SkipList_RandomLevelGenerator              d_rand;

    bsls::AtomicInt                            d_listLevel;
    Node                                      *d_head_p;
    Node                                      *d_tail_p;

    mutable Lock                               d_lock;

    int                                        d_length;

    PoolManager                               *d_poolManager_p; // owned

    bslma::Allocator                          *d_allocator_p; // held

    // FRIENDS
    friend class SkipListPair<KEY, DATA>;
    friend class SkipListPairHandle<KEY, DATA>;
    template <class KEY2, class DATA2>
    friend bool operator==(const SkipList<KEY2, DATA2>&,
                           const SkipList<KEY2, DATA2>&);
    template <class KEY2, class DATA2>
    friend bool operator!=(const SkipList<KEY2, DATA2>&,
                           const SkipList<KEY2, DATA2>&);

    // PRIVATE CLASS METHODS
    static DATA& data(const Pair *reference);
        // Return a non-'const' reference to the "data" value of the pair
        // identified by the specified 'reference'.

    static const KEY& key(const Pair *reference);
        // Return a 'const' reference to the "key" value of the pair identified
        // by the specified 'reference'.

    static inline BSLS_KEYWORD_CONSTEXPR bsls::Types::IntPtr offsetOfPtrs();
        // Return the offset in bytes of 'd_ptrs' from the start of the
        // 'SkipList_Node' struct.  (similar to
        // 'offsetof(SkipList_Node, d_ptrs)' but with no requirement that
        // 'DATA' or 'KEY' be PODs)

    static Node *pairToNode(Pair *reference);
        // Cast the specified 'reference' to a 'Node *'.

    static Node *pairToNode(const Pair *reference);
        // Const-cast the specified 'reference' to a 'Node *'.

    // PRIVATE MANIPULATORS
    void addNode(bool *newFrontFlag, Node *newNode);
        // Acquire the lock, add the specified 'newNode' to the list, and
        // release the lock.  If the specified 'newFrontFlag' is not 0, load
        // into it a 'true' value if the node is at the front of the list, and
        // a 'false' value otherwise.

    void addNodeImpR(bool *newFrontFlag, Node *newNode, bool lock);
        // Acquire the lock if the specified 'lock' is 'true', add the
        // specified 'newNode' to the list, and release the lock (if acquired).
        // Search for the correct position for 'newNode' from the back of the
        // list (in descending order by key value).  If the specified
        // 'newFrontFlag' is not 0, load into it a 'true' value if the node is
        // at the front of the list, and a 'false' value otherwise.

    void addNodeR(bool *newFrontFlag, Node *newNode);
        // Invoke 'addNodeImpR' with lock='true'.  IMPLEMENTATION NOTE: this
        // *particular* flavor of "addNode" is factored into an
        // optionally-non-locking version to facilitate writing the assignment
        // operator.  If the specified 'newFrontFlag' is not 0, load into it a
        // 'true' value if the node is at the front of the list, and a 'false'
        // value otherwise.
        //
        // Acquire the lock, add the specified 'newNode' to the list, and
        // release the lock (if acquired).  Search for the correct position for
        // 'newNode' from the back of the list (in descending order by key
        // value).  If 'newFrontFlag' is not 0, load into it a 'true' value if
        // the node is at the front of the list, and a 'false' value otherwise.

    int addNodeUnique(bool *newFrontFlag, Node *newNode);
        // Acquire the lock, add the specified 'newNode' to the list, and
        // release the lock.  If the specified 'newFrontFlag' is not 0, load
        // into it a 'true' value if the node is at the front of the list, and
        // a 'false' value otherwise.  Return 0 on success, and a nonzero value
        // (with no effect on the list) if a node with the same "key" value as
        // 'newNode' is in the list.

    int addNodeUniqueR(bool *newFrontFlag, Node *newNode);
        // Acquire the lock, add the specified 'newNode' to the list, and
        // release the lock.  Search for the correct position for 'newNode'
        // from the back of the list (in descending order by key value).  If
        // the specified 'newFrontFlag' is not 0, load into it a 'true' value
        // if the node is at the front of the list, and a 'false' value
        // otherwise.  Return 0 on success, and a nonzero value (with no effect
        // on the list) if a node with the same "key" value as 'newNode' is in
        // the list.

    Node *allocateNode(int level, const KEY& key, const DATA& data);
        // Allocate a node from the node pool of this list, and set its key
        // value to the specified 'key' and data value to the specified 'data'.
        // Set the node's level to the specified 'level' if 'level' is less
        // than or equal to the highest level of any node previously in the
        // list, or to one greater than that value otherwise.  Return the
        // allocated node.  Note that this method neither acquires nor requires
        // the lock.

    void initialize();
        // Populate the members of a new Skip List.  This private manipulator
        // must be called only once, by the constructor.

    void insertImp(bool *newFrontFlag, Node *location[], Node *node);
        // Insert the specified 'node' into this list immediately before the
        // specified 'location' (which is populated by either
        // 'lookupImpLowerBound' or 'lookupImpLowerBoundR').  Load into the
        // specified 'newFrontFlag' a 'true' value if the node is inserted at
        // the front of the list, and 'false' otherwise.  This method must be
        // called under the lock.

    void moveImp(bool *newFrontFlag, Node *location[], Node *node);
        // Insert the specified 'node' into this list immediately before the
        // specified 'location' (which is populated by either
        // 'lookupImpLowerBound' or 'lookupImpLowerBoundR',
        // 'lookupImpUpperBound', or lookupImpUpperBoundR').  Load into the
        // specified 'newFrontFlag' a 'true' value if the node is inserted at
        // the front of the list, and 'false' otherwise.  This method must be
        // called under the lock.
        //
        // Like 'insertImp', but 'node' must already be present in the list.
        // This internal method must be called under the lock.

    Node *popFrontImp();
        // Acquire the lock, remove the front of the list, and release the
        // lock.  Return the node that was at the front of the list, or 0 if
        // the list was empty.

    void releaseNode(Node *node);
        // Decrement the reference count of the specified 'node', and if it
        // reaches 0, destroy 'node' and return it to the pool.  Note that this
        // method neither acquires nor requires the lock.

    template <class VECTOR>
    int removeAllMaybeUnlock(VECTOR *removed, bool unlock);
        // Remove all items from this list, and append to the specified
        // 'removed' vector objects referring to the removed nodes.  Note that
        // the objects appended to 'removed' will be in ascending order by key
        // value.  Note that 'removed' may be 0, in which case removed nodes
        // are released.  Return the number of items that were removed from the
        // list.  The behavior is undefined unless the mutex is already locked
        // before it is called.

    template <class VECTOR>
    int removeAllImp(VECTOR *removed);
        // Remove all items from this list, and append to the specified
        // 'removed' vector objects referring to the removed nodes.  Note that
        // the objects appended to 'removed' will be in ascending order by key
        // value.  Note that 'removed' may be 0, in which case removed nodes
        // are released.  Return the number of items that were removed from the
        // list.  The behavior is undefined unless the mutex is already locked
        // before it is called.

    int removeNode(Node *node);
        // Acquire the lock, remove the specified 'node' from the list, and
        // release the lock.  Return 0 on success, and 'e_NOT_FOUND' if the
        // 'node' is no longer in the list.

    int updateNode(bool       *newFrontFlag,
                   Node       *node,
                   const KEY&  newKey,
                   bool        allowDuplicates);
        // Acquire the lock, move the specified 'node' to the correct position
        // for the specified 'newKey', and release the lock.  Update the key
        // value of 'node' to the 'newKey' value.  If the specified
        // 'newFrontFlag' is not 0, load into it a 'true' value if the new
        // location of the node is the front of the list, and a 'false' value
        // otherwise.  If there will be multiple instances of 'newKey' in the
        // list after the update, the updated node will be the *first* node
        // with the key 'newKey'.  Return 0 on success, 'e_NOT_FOUND' if the
        // node is no longer in the list, or 'e_DUPLICATE' if the specified
        // 'allowDuplicates' is 'false' and 'newKey' already appears in the
        // list.

    int updateNodeR(bool       *newFrontFlag,
                    Node       *node,
                    const KEY&  newKey,
                    bool        allowDuplicates);
        // Acquire the lock, move the specified 'node' to the correct position
        // for the specified 'newKey', and release the lock.  The search for
        // the correct location for 'newKey' proceeds from the back of the list
        // in descending order by by key value.  Update the key value of 'node'
        // to the 'newKey' value.  If the specified 'newFrontFlag' is not 0,
        // load into it a 'true' value if the new location of the node is the
        // front of the list, and a 'false' value otherwise.  If there will be
        // multiple instances of 'newKey' in the list after the update, the
        // updated node will be the *last* node with the key 'newKey'.  Return
        // 0 on success, 'e_NOT_FOUND' if the node is no longer in the list, or
        // 'e_DUPLICATE' if the specified 'allowDuplicates' is 'false' and
        // 'newKey' already appears in the list.

    // PRIVATE ACCESSORS
    Node *backNode() const;
        // Return the node at the back of the list, or 0 if the list is empty.
        // This method acquires and releases the lock.

    void checkInvariants() const;
        // This function is normally never called -- it is useful in debugging.
        // If this function is called from anywhere other than the destructor,
        // it is important that the mutex be locked.

    Node *findNode(const KEY& key) const;
        // Return the node with the specified 'key', or 0 if no node could be
        // found.  This method acquires and releases the lock.

    Node *findNodeR(const KEY& key) const;
        // Return the node with the specified 'key', or 0 if no node could be
        // found.  This method acquires and releases the lock.

    Node *findNodeLowerBound(const KEY& key) const;
        // Return the first node in this list whose key is not less than the
        // specified 'key', found by searching the list from the front (in
        // ascending order of key value), and 0 if no such node exists.  This
        // method acquires and releases the lock.

    Node *findNodeLowerBoundR(const KEY& key) const;
        // Return the first node in this list whose key is not less than the
        // specified 'key', found by searching the list from the back (in
        // descending order of key value), and 0 if no such node exists.  This
        // method acquires and releases the lock.

    Node *findNodeUpperBound(const KEY& key) const;
        // Return the first node in this list whose key is greater than the
        // specified 'key', found by searching the list from the front (in
        // ascending order of key value), and 0 if no such node exists.  This
        // method acquires and releases the lock.

    Node *findNodeUpperBoundR(const KEY& key) const;
        // Return the first node in this list whose key is greater than the
        // specified 'key', found by searching the list from the back (in
        // descending order of key value), and 0 if no such node exists.  This
        // method acquires and releases the lock.

    Node *frontNode() const;
        // Return the node at the front of the list, or 0 if the list is empty.
        // This method acquires and releases the lock.

    void lookupImpLowerBound(Node *location[], const KEY& key) const;
        // Populate the specified 'location' array with the first node whose
        // key is not less than the specified 'key' at each level in the list,
        // found by searching the list from the front (in ascending order of
        // key value); if no such node exists at a given level, the
        // tail-of-list sentinel is populated for that level.  This method must
        // be called under the lock.

    void lookupImpLowerBoundR(Node *location[], const KEY& key) const;
        // Populate the specified 'location' array with the first node whose
        // key is not less than the specified 'key' at each level in the list,
        // found by searching the list from the back (in descending order of
        // key value); if no such node exists at a given level, the
        // tail-of-list sentinel is populated for that level.  This method must
        // be called under the lock.

    void lookupImpUpperBound(Node *location[], const KEY& key) const;
        // Populate the specified 'location' array with the first node whose
        // key is greater than the specified 'key' at each level in the list,
        // found by searching the list from the front (in ascending order of
        // key value); if no such node exists at a given level, the
        // tail-of-list sentinel is populated for that level.  This method must
        // be called under the lock.

    void lookupImpUpperBoundR(Node *location[], const KEY& key) const;
        // Populate the specified 'location' array with the first node whose
        // key is greater than the specified 'key' at each level in the list,
        // found by searching the list from the back (in descending order of
        // key value); if no such node exists at a given level, the
        // tail-of-list sentinel is populated for that level.  This method must
        // be called under the lock.

    Node *nextNode(Node *node) const;
        // Return the node after to the specified 'node', or 0 if 'node' is at
        // the back of the list.  This method acquires and releases the lock.

    Node *prevNode(Node *node) const;
        // Return the node prior to the specified 'node', or 0 if 'node' is at
        // the front of the list.  This method acquires and releases the lock.

    int skipBackward(Node **node) const;
        // If the item identified by the specified 'node' is not at the front
        // of the list, load a reference to the previous item in the list into
        // 'node'; otherwise load 0 into 'node'.  Return 0 on success, and
        // 'e_NOT_FOUND' (with no effect on the value of 'node') if 'node' is
        // no longer in the list.  This method acquires and releases the lock.

    int skipForward(Node **node) const;
        // If the item identified by the specified 'node' is not at the back of
        // the list, load a reference to the next item in the list into 'node';
        // otherwise load 0 into 'node'.  Return 0 on success, and
        // 'e_NOT_FOUND' (with no effect on the value of 'node') if 'node' is
        // no longer in the list.  This method acquires and releases the lock.

  private:
    // NOT IMPLEMENTED
    void addPairReferenceRaw(const PairHandle&);
    void releaseReferenceRaw(const PairHandle&);
        // These methods are declared 'private' and not implemented to prevent
        // the accidental casting of a 'SkipListPairHandle' to a
        // 'SkipListPair *'.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(SkipList, bslma::UsesBslmaAllocator);

    // CLASS METHODS
    static int level(const Pair *reference);
        // Return the level of the pair identified by the specified
        // 'reference'.  This method is provided for testing.

    // CREATORS
    explicit SkipList(bslma::Allocator *basicAllocator = 0);
        // Create a new Skip List.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    SkipList(const SkipList& original, bslma::Allocator *basicAllocator = 0);
        // Create a new Skip List initialized to the value of the specified
        // 'original' list.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~SkipList();
        // Destroy this Skip List.  The behavior is undefined if references are
        // outstanding to any pairs in the list.

    // MANIPULATORS
    SkipList& operator=(const SkipList& rhs);
        // Assign to this Skip List the value of the specified 'rhs' list and
        // return a reference to the modifiable list.

    void releaseReferenceRaw(const Pair *reference);
        // Release the specified 'reference'.  After calling this method, the
        // value of 'reference' must not be used or released again.

                         // Insertion Methods

    void add(const KEY& key, const DATA& data, bool *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list.  Load into the
        // the optionally specified 'newFrontFlag' a 'true' value if the pair
        // is at the front of the list, and a 'false' value otherwise.

    void add(PairHandle  *result,
             const KEY&   key,
             const DATA&  data,
             bool        *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list, and load into
        // the specified 'result' a reference to the pair in the list.  Load
        // into the optionally specified 'newFrontFlag' a 'true' value if the
        // pair is at the front of the list, and a 'false' value otherwise.

    void addAtLevelRaw(Pair        **result,
                       int           level,
                       const KEY&    key,
                       const DATA&   data,
                       bool         *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list at the specified
        // 'level', and load into the specified 'result' a reference to the
        // pair in the list.  The 'result' reference must be released (using
        // 'releaseReferenceRaw') when it is no longer needed.  Load into the
        // the optionally specified 'newFrontFlag' a 'true' value if the pair
        // is at the front of the list, and a 'false' value otherwise.  The
        // behavior is undefined if 'level' is greater than the
        // implementation-defined maximum level of this class, or if 'level' is
        // negative.  Note that this method is provided for testing purposes.

    int addAtLevelUniqueRaw(Pair        **result,
                            int           level,
                            const KEY&    key,
                            const DATA&   data,
                            bool         *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list at the specified
        // 'level', and load into the specified 'result' a reference to the
        // pair in the list.  The 'result' reference must be released (using
        // 'releaseReferenceRaw') when it is no longer needed.  Load into the
        // the optionally specified 'newFrontFlag' a 'true' value if the pair
        // is at the front of the list, and a 'false' value otherwise.  The
        // behavior is undefined if 'level' is greater than the
        // implementation-defined maximum level of this class, or if 'level' is
        // negative.  Return 0 on success, and a non-zero value (with no effect
        // on the list) if 'key' is already in the list.  Note that this method
        // is provided for testing purposes.

    void addRaw(Pair        **result,
                const KEY&    key,
                const DATA&   data,
                bool         *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list, and load into
        // the specified 'result' a reference to the pair in the list.  The
        // 'result' reference must be released (using 'releaseReferenceRaw')
        // when it is no longer needed.  Load into the optionally specified
        // 'newFrontFlag' a 'true' value if the pair is at the front of the
        // list, and a 'false' value otherwise.

    int addUnique(const KEY& key, const DATA& data, bool *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list.  Load into the
        // the optionally specified 'newFrontFlag' a 'true' value if the pair
        // is at the front of the list, and a 'false' value otherwise.  Return
        // 0 on success, and a non-zero value (with no effect on the list) if
        // 'key' is already in the list.

    int addUnique(PairHandle  *result,
                  const KEY&   key,
                  const DATA&  data,
                  bool        *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list, and load into
        // the specified 'result' a reference to the pair in the list.  Load
        // into the optionally specified 'newFrontFlag' a 'true' value if the
        // pair is at the front of the list, and a 'false' value otherwise.
        // Return 0 on success, and a non-zero value (with no effect on the
        // list) if 'key' is already in the list.

    int addUniqueRaw(Pair        **result,
                     const KEY&    key,
                     const DATA&   data,
                     bool         *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list, and load into
        // the specified 'result' a reference to the pair in the list.  The
        // 'result' reference must be released (using 'releaseReferenceRaw')
        // when it is no longer needed.  Load into the optionally specified
        // 'newFrontFlag' a 'true' value if the pair is at the front of the
        // list, and a 'false' value otherwise.  Return 0 on success, and a
        // non-zero value (with no effect on the list) if 'key' is already in
        // the list.

                         // Insertion Methods (Reverse Search)

    void addAtLevelRawR(Pair        **result,
                        int           level,
                        const KEY&    key,
                        const DATA&   data,
                        bool         *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list at the specified
        // 'level', and load into the specified 'result' a reference to the
        // pair in the list.  Search for the correct position for 'key' from
        // the back of the list (in descending order by key value).  The
        // 'result' reference must be released (using 'releaseReferenceRaw')
        // when it is no longer needed.  Load into the optionally specified
        // 'newFrontFlag' a 'true' value if the pair is at the front of the
        // list, and a 'false' value otherwise.  The behavior is undefined if
        // 'level' is greater than the implementation-defined maximum level of
        // this class, or if 'level' is negative.  Note that this method is
        // provided for testing purposes.

    int addAtLevelUniqueRawR(Pair        **result,
                             int           level,
                             const KEY&    key,
                             const DATA&   data,
                             bool         *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list at the specified
        // 'level', and load into the specified 'result' a reference to the
        // pair in the list.  Search for the correct position for 'key' from
        // the back of the list (in descending order by key value).  The
        // 'result' reference must be released (using 'releaseReferenceRaw')
        // when it is no longer needed.  Load into the optionally specified
        // 'newFrontFlag' a 'true' value if the pair is at the front of the
        // list, and a 'false' value otherwise.  The behavior is undefined if
        // 'level' is greater than the implementation-defined maximum level of
        // this class, or if 'level' is negative.  Return 0 on success, and a
        // non-zero value (with no effect on the list) if 'key' is already in
        // the list.  Note that this method is provided for testing purposes.

    void addR(const KEY& key, const DATA& data, bool *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list.  Search for the
        // correct position for 'key' from the back of the list (in descending
        // order by key value).  Load into the optionally specified
        // 'newFrontFlag' a 'true' value if the pair is at the front of the
        // list, and a 'false' value otherwise.

    void addR(PairHandle  *result,
              const KEY&   key,
              const DATA&  data,
              bool        *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list, and load into
        // the specified 'result' a reference to the pair in the list.  Search
        // for the correct position for 'key' from the back of the list (in
        // descending order by key value).  Load into the optionally specified
        // 'newFrontFlag' a 'true' value if the pair is at the front of the
        // list, and a 'false' value otherwise.

    void addRawR(Pair        **result,
                 const KEY&    key,
                 const DATA&   data,
                 bool         *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list, and load into
        // the specified 'result' a reference to the pair in the list.  Search
        // for the correct position for 'key' from the back of the list (in
        // descending order by key value).  The 'result' reference must be
        // released (using 'releaseReferenceRaw') when it is no longer needed.
        // Load into the optionally specified 'newFrontFlag' a 'true' value if
        // the pair is at the front of the list, and a 'false' value otherwise.

    int addUniqueR(const KEY& key, const DATA& data, bool *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list.  Search for the
        // correct position for 'key' from the back of the list (in descending
        // order by key value).  Load into the optionally specified
        // 'newFrontFlag' a 'true' value if the pair is at the front of the
        // list, and a 'false' value otherwise.  Return 0 on success, and a
        // non-zero value (with no effect on the list) if 'key' is already in
        // the list.

    int addUniqueR(PairHandle  *result,
                   const KEY&   key,
                   const DATA&  data,
                   bool        *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list, and load into
        // the specified 'result' a reference to the pair in the list.  Search
        // for the correct position for 'key' from the back of the list (in
        // descending order by key value).  Load into the optionally specified
        // 'newFrontFlag' a 'true' value if the pair is at the front of the
        // list, and a 'false' value otherwise.  Return 0 on success, and a
        // non-zero value (with no effect on the list) if 'key' is already in
        // the list.

    int addUniqueRawR(Pair        **result,
                      const KEY&    key,
                      const DATA&   data,
                      bool         *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list, and load into
        // the specified 'result' a reference to the pair in the list.  Search
        // for the correct position for 'key' from the back of the list (in
        // descending order by key value).  The 'result' reference must be
        // released (using 'releaseReferenceRaw') when it is no longer needed.
        // Load into the optionally specified 'newFrontFlag' a 'true' value if
        // the pair is at the front of the list, and a 'false' value otherwise.
        // Return 0 on success, and a non-zero value (with no effect on the
        // list) if 'key' is already in the list.

                         // Removal Methods

    int popFront(PairHandle *item = 0);
        // Remove the first item from the list and load a reference to it into
        // the optionally specified 'item'.  Return 0 on success, and a
        // non-zero value if the list is empty.

    int popFrontRaw(Pair **item);
        // Remove the first item from the list and load a reference to it into
        // the specified 'item'.  This reference must be released (using
        // 'releaseReferenceRaw') when it is no longer needed.  Return 0 on
        // success, and a non-zero value if the list is empty.

    int remove(const Pair *reference);
        // Remove the item identified by the specified 'reference' from the
        // list.  Return 0 on success, and a non-zero value if the pair has
        // already been removed from the list.

    int removeAll();
    int removeAll(bsl::vector<PairHandle>      *removed);
    int removeAll(std::vector<PairHandle>      *removed);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    int removeAll(std::pmr::vector<PairHandle> *removed);
#endif
        // Remove all items from this list.  Optionally specify 'removed', a
        // vector to which to append handles to the removed nodes.  The items
        // appended to 'removed' will be in ascending order by key value.
        // Return the number of items that were removed from this list.  Note
        // that all references in 'removed' must be released (i.e., destroyed)
        // before this skip list is destroyed.  Note that if 'removed' is not
        // specified, all removed elements will be released by this method.

    int removeAllRaw(bsl::vector<Pair *>      *removed);
    int removeAllRaw(std::vector<Pair *>      *removed);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    int removeAllRaw(std::pmr::vector<Pair *> *removed);
#endif
        // Remove all items from this list.  Append to the specified
        // 'removed' vector pointers that can be used to refer to the removed
        // items.  *Each* such pointer must be released (using
        // 'releaseReferenceRaw') when it is no longer needed.  The pairs
        // appended to 'removed' will be in ascending order by key value.
        // Return the number of items that were removed from this list.

                         // Update Methods

    int update(const Pair *reference,
               const KEY&  newKey,
               bool       *newFrontFlag = 0,
               bool        allowDuplicates = true);
        // Assign the specified 'newKey' value to the pair identified by the
        // specified 'reference', moving the pair within the list as necessary.
        // Load into the optionally specified 'newFrontFlag' a 'true' value if
        // the new location of the pair is the front of the list.  Return 0 on
        // success, 'e_NOT_FOUND' if the pair referred to by 'reference' is no
        // longer in the list, or 'e_DUPLICATE' if the optionally specified
        // 'allowDuplicates' is 'false' and 'newKey' already appears in the
        // list.

    int updateR(const Pair *reference,
                const KEY&  newKey,
                bool       *newFrontFlag = 0,
                bool        allowDuplicates = true);
        // Assign the specified 'newKey' value to the pair identified by the
        // specified 'reference', moving the pair within the list as necessary.
        // Search for the new position from the back of the list (in descending
        // order by key value).  Load into the optionally specified
        // 'newFrontFlag' a 'true' value if the new location of the pair is the
        // front of the list.  Return 0 on success, 'e_NOT_FOUND' if the pair
        // referred to by 'reference' is no longer in the list, or
        // 'e_DUPLICATE' if the optionally specified 'allowDuplicates' is
        // 'false' and 'newKey' already appears in the list.

    // ACCESSORS
    Pair *addPairReferenceRaw(const Pair *reference) const;
        // Increment the reference count for the list element referred to by
        // the specified 'reference'.  There must be a corresponding call to
        // 'releaseReferenceRaw' when the reference is no longer needed.  The
        // behavior is undefined 'item' has already been released.  Return
        // 'reference'.

    int back(PairHandle *back) const;
        // Load into the specified 'back' a reference to the last item in the
        // list.  Return 0 on success, and a non-zero value (with no effect on
        // 'back') if the list is empty.

    int backRaw(Pair **back) const;
        // Load into the specified 'back' a reference to the last item in the
        // list.  The 'back' reference must be released (using
        // 'releaseReferenceRaw') when it is no longer needed.  Return 0 on
        // success, and a non-zero value if the list is empty.  Note that if
        // the list is empty, the value of '*back' is undefined.

    bool exists(const KEY& key) const;
        // Return 'true' if there is a pair in the list with the specified
        // 'key', and 'false' otherwise.

    int front(PairHandle *front) const;
        // Load into the specified 'front' a reference to the first item in the
        // list.  Return 0 on success, and a non-zero value (with no effect on
        // 'front') if the list is empty.

    int frontRaw(Pair **front) const;
        // Load into the specified 'front' a reference to the first item in the
        // list.  The 'front' reference must be released (using
        // 'releaseReferenceRaw') when it is no longer needed.  Return 0 on
        // success, and a non-zero value if the list is empty.

    bool isEmpty() const;
        // Return 'true' if this list is empty, and 'false' otherwise.

    int length() const;
        // Return the number of items in this list.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this list object to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level' and
        // return a reference to 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, suppress all indentation AND format the entire output on
        // one line.  If 'stream' is not valid on entry, this operation has no
        // effect.

                            // simple forward finds

    int find(PairHandle *item, const KEY& key) const;
        // Load into the specified 'item' a reference to the element in this
        // list with the specified 'key' found by searching the list from the
        // front (in ascending order of key value).  If multiple elements
        // having 'key' are in the container, load 'item' with the *first*
        // matching element.  Return 0 on success, and a non-zero value (with
        // no effect on 'item') if no such element exists.  If there are
        // multiple elements in the list with the 'key', it is undefined which
        // one is returned.

    int findRaw(Pair **item, const KEY& key) const;
        // Load into the specified 'item' a reference to the element in this
        // list with the specified 'key' found by searching the list from the
        // front (in ascending order of key value).  Return 0 on success, and a
        // non-zero value (with no effect on 'item') if no such element exists.
        // If there are multiple elements in the list with the 'key', it is
        // undefined which one is returned.  The 'item' reference must be
        // released (using 'releaseReferenceRaw') when it is no longer needed.

                            // simple reverse finds

    int findR(PairHandle *item, const KEY& key) const;
        // Load into the specified 'item' a reference to the element in this
        // list with the specified 'key' found by searching the list from the
        // back (in descending order of key value).  If multiple elements
        // having 'key' are in the container, load 'item' with the *last*
        // matching element.  'key' are present, find the last one.  Return 0
        // on success, and a non-zero value (with no effect on 'item') if no
        // such element exists.  If there are multiple elements in the list
        // with the 'key', it is undefined which one is returned.

    int findRRaw(Pair **item, const KEY& key) const;
        // Load into the specified 'item' a reference to the element in this
        // list with the specified 'key' found by searching the list from the
        // back (in descending order of key value).  Return 0 on success, and a
        // non-zero value (with no effect on 'item') if no such element exists.
        // If there are multiple elements in the list with the 'key', it is
        // undefined which one is returned.  The 'item' reference must be
        // released (using 'releaseReferenceRaw') when it is no longer needed.

                        // find lower bound

    int findLowerBound(PairHandle *item, const KEY& key) const;
        // Load into the specified 'item' a reference to the first element in
        // this list whose key value is not less than the specified 'key' found
        // by searching the list from the front (in ascending order of key
        // value).  Return 0 on success, and a non-zero value (with no effect
        // on 'item') if no such element exists.

    int findLowerBoundRaw(Pair **item, const KEY& key) const;
        // Load into the specified 'item' a reference to the first element in
        // this list whose key value is not less than the specified 'key' found
        // by searching the list from the front (in ascending order of key
        // value).  Return 0 on success, and a non-zero value (with no effect
        // on 'item') if no such element exists.  The 'item' reference must be
        // released (using 'releaseReferenceRaw') when it is no longer needed.

                        // find lower bound reverse

    int findLowerBoundR(PairHandle *item, const KEY& key) const;
        // Load into the specified 'item' a reference to the first element in
        // this list whose key value is not less than the specified 'key' found
        // by searching the list from the back (in descending order of key
        // value).  Return 0 on success, and a non-zero value (with no effect
        // on 'item') if no such element exists.

    int findLowerBoundRRaw(Pair **item, const KEY& key) const;
        // Load into the specified 'item' a reference to the first element in
        // this list whose key value is not less than the specified 'key' found
        // by searching the list from the back (in descending order of key
        // value).  Return 0 on success, and a non-zero value (with no effect
        // on 'item') if no such element exists.  The 'item' reference must be
        // released (using 'releaseReferenceRaw') when it is no longer needed.

                        // find upper bound

    int findUpperBound(PairHandle *item, const KEY& key) const;
        // Load into the specified 'item' a reference to the first element in
        // this list whose key value is greater than the specified 'key' found
        // by searching the list from the front (in ascending order of key
        // value).  Return 0 on success, and a non-zero value (with no effect
        // on 'item') if no such element exists.

    int findUpperBoundRaw(Pair **item, const KEY& key) const;
        // Load into the specified 'item' a reference to the first element in
        // this list whose key value is greater than the specified 'key' found
        // by searching the list from the front (in ascending order of key
        // value).  Return 0 on success, and a non-zero value (with no effect
        // on 'item') if no such element exists.  The 'item' reference must be
        // released (using 'releaseReferenceRaw') when it is no longer needed.

                        // find upper bound reverse

    int findUpperBoundR(PairHandle *item, const KEY& key) const;
        // Load into the specified 'item' a reference to the first element in
        // this list whose key value is greater than the specified 'key' found
        // by searching the list from the back (in descending order of key
        // value).  Return 0 on success, and a non-zero value (with no effect
        // on 'item') if no such element exists.

    int findUpperBoundRRaw(Pair **item, const KEY& key) const;
        // Load into the specified 'item' a reference to the first element in
        // this list whose key value is greater than the specified 'key' found
        // by searching the list from the back (in descending order of key
        // value).  Return 0 on success, and a non-zero value (with no effect
        // on 'item') if no such element exists.  The 'item' reference must be
        // released (using 'releaseReferenceRaw') when it is no longer needed.

                            // next, previous, & skip*

    int next(PairHandle *next, const Pair *reference) const;
        // Load into the specified 'next' a reference to the item that appears
        // in the list after the item identified by the specified 'reference'.
        // Return 0 on success, or a non-zero value if 'reference' refers to
        // the back of the list.

    int nextRaw(Pair **next, const Pair *reference) const;
        // Load into the specified 'next' a reference to the item that appears
        // in the list after the item identified by the specified 'reference'.
        // The 'next' reference must be released (using 'releaseReferenceRaw')
        // when it is no longer needed.  Return 0 on success, or a non-zero
        // value if 'reference' refers to the back of the list.

    int previous(PairHandle *prevPair, const Pair *reference) const;
        // Load into the specified 'prevPair' a reference to the pair that
        // appears in the list before the pair identified by the specified
        // 'reference'.  Return 0 on success, or a non-zero value if
        // 'reference' refers to the front of the list.

    int previousRaw(Pair **prevPair, const Pair *reference) const;
        // Load into the specified 'prevPair' a reference to the pair that
        // appears in the list before the pair identified by the specified
        // 'reference'.  The 'prevPair' reference must be released (using
        // 'releaseReferenceRaw') when it is no longer needed.  Return 0 on
        // success, or a non-zero value if 'reference' refers to the front of
        // the list.

    int skipBackward(PairHandle *item) const;
        // If the item identified by the specified 'item' is not at the front
        // of the list, load a reference to the previous item in the list into
        // 'item'; otherwise reset the value of 'item'.  Return 0 on success,
        // and 'e_NOT_FOUND' (with no effect on the value of 'item') if 'item'
        // is no longer in the list.

    int skipBackwardRaw(Pair **item) const;
        // If the item identified by the specified 'item' is not at the front
        // of the list, load a reference to the previous item in the list into
        // 'item'; otherwise reset the value of 'item'.  Return 0 on success,
        // and 'e_NOT_FOUND' (with no effect on the value of 'item') if 'item'
        // is no longer in the list.

    int skipForward(PairHandle *item) const;
        // If the item identified by the specified 'item' is not at the end of
        // the list, load a reference to the next item in the list into 'item';
        // otherwise reset the value of 'item'.  Return 0 on success, and
        // 'e_NOT_FOUND' (with no effect on the value of 'item') if 'item' is
        // no longer in the list.

    int skipForwardRaw(Pair **item) const;
        // If the item identified by the specified 'item' is not at the end of
        // the list, load a reference to the next item in the list into 'item';
        // otherwise reset the value of 'item'.  Return 0 on success, and
        // 'e_NOT_FOUND' (with no effect on the value of 'item') if 'item' is
        // no longer in the list.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

// FREE OPERATORS
template <class KEY, class DATA>
bool operator==(const SkipList<KEY, DATA>& lhs,
                const SkipList<KEY, DATA>& rhs);
    // Return 'true' if the specified 'lhs' list has the same value as the
    // specified 'rhs' list, and 'false' otherwise.  Two lists A and B have the
    // same value if they have the same number of elements, and if for all i in
    // the range [0, numberOfElements), the i'th pair from the front of A has
    // the same key and data values as the i'th pair from the front of B.  Note
    // that if there are duplicate key values in a list, the order of iteration
    // over those pairs may be different than for another list that was
    // constructed from the same sequence of values (and thus the lists may not
    // compare equal).

template <class KEY, class DATA>
bool operator!=(const SkipList<KEY, DATA>& lhs,
                const SkipList<KEY, DATA>& rhs);
    // Return 'true' if the specified 'lhs' list list has a different value
    // from the specified 'rhs' list, and 'false' otherwise.  Two lists A and B
    // have different values if they have a different of elements, or if there
    // exists an i in the range [0, numberOfElements) such that the i'th pair
    // from the front of A differs in key or data values from i'th pair from
    // the front of B.

template<class KEY, class DATA>
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const SkipList<KEY, DATA>& list);
    // Write the specified 'list' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

                          // ========================
                          // class SkipList::IsVector
                          // ========================

template <class KEY, class DATA>
template <class VECTOR, class VALUE_TYPE>
class SkipList<KEY, DATA>::IsVector {
    // This 'struct' has a 'value' that evaluates to 'true' if the specified
    // 'VECTOR' is a 'bsl', 'std', or 'std::pmr' 'vector<VALUE_TYPE>'.

  public:
    // PUBLIC CLASS DATA
    static const bool value =
                      bsl::is_same<bsl::vector<VALUE_TYPE>, VECTOR>::value
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                   || bsl::is_same<std::pmr::vector<VALUE_TYPE>, VECTOR>::value
#endif
                   || bsl::is_same<std::vector<VALUE_TYPE>, VECTOR>::value;
};

                        // ===========================
                        // class SkipList::PairFactory
                        // ===========================

template <class KEY, class DATA>
class SkipList<KEY, DATA>::PairFactory {
  public:
    // CREATORS
    explicit PairFactory(SkipList *);
        // Create a 'PairFactory'.

    // ACCESSOR
    Pair *operator()(Node *node) const;
        // Convert the specified 'node' to a 'Pair *'.
};

                    // =================================
                    // class SkipList::PairHandleFactory
                    // =================================

template <class KEY, class DATA>
class SkipList<KEY, DATA>::PairHandleFactory {
    // DATA
    SkipList *d_list_p;

  public:
    // CREATORS
    explicit PairHandleFactory(SkipList *list);
        // Create a 'PairHandleFactory' bound to the specified 'list'.

    // ACCESSOR
    PairHandle operator()(Node *node) const;
        // Return a 'PairHandle' bound to the list this object is bound to, and
        // referring to the specified 'node'.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                          // ------------------------
                          // SkipList_DoubleLockGuard
                          // ------------------------

inline
SkipList_DoubleLockGuard::SkipList_DoubleLockGuard(bslmt::Mutex *lock1,
                                                   bslmt::Mutex *lock2)
: d_firstGuard(bsl::min(lock1, lock2, bsl::less<bslmt::Mutex *>()))
, d_lastGuard( bsl::max(lock1, lock2, bsl::less<bslmt::Mutex *>()))
{}

                             // ------------------
                             // class SkipListPair
                             // ------------------

// ACCESSORS
template <class KEY, class DATA>
inline
DATA& SkipListPair<KEY, DATA>::data() const
{
    return SkipList<KEY, DATA>::data(this);
}

template <class KEY, class DATA>
inline
const KEY& SkipListPair<KEY, DATA>::key() const
{
    return SkipList<KEY, DATA>::key(this);
}

                          // ------------------------
                          // class SkipListPairHandle
                          // ------------------------

// CREATORS
template <class KEY, class DATA>
inline
SkipListPairHandle<KEY, DATA>::SkipListPairHandle()
: d_list_p(0)
, d_node_p(0)
{
}

template <class KEY, class DATA>
inline
SkipListPairHandle<KEY, DATA>::SkipListPairHandle(
                                                SkipList<KEY, DATA> *list,
                                                Pair                *reference)
: d_list_p(list)
, d_node_p(reference)
{
}

template <class KEY, class DATA>
inline
SkipListPairHandle<KEY, DATA>::SkipListPairHandle(
                                            const SkipListPairHandle& original)
: d_list_p(original.d_list_p)
, d_node_p(original.d_node_p
           ? d_list_p->addPairReferenceRaw(original.d_node_p)
           : 0)
{
}

template <class KEY, class DATA>
inline
SkipListPairHandle<KEY, DATA>::~SkipListPairHandle()
{
    release();
}

// MANIPULATORS
template <class KEY, class DATA>
inline
SkipListPairHandle<KEY, DATA>&
SkipListPairHandle<KEY, DATA>::operator=(const SkipListPairHandle& rhs)
{
    reset(rhs.d_list_p, 0);
    d_node_p = rhs.d_node_p ? d_list_p->addPairReferenceRaw(rhs.d_node_p) : 0;
    return *this;
}

template <class KEY, class DATA>
inline
void SkipListPairHandle<KEY, DATA>::release()
{
    if (d_node_p) {
        BSLS_ASSERT(0 != d_list_p);

        d_list_p->releaseReferenceRaw(d_node_p);
        d_node_p = 0;
    }
}

template <class KEY, class DATA>
inline
void SkipListPairHandle<KEY, DATA>::releaseReferenceRaw(
                                               SkipList<KEY, DATA> **list,
                                               Pair                **reference)
{
    BSLS_ASSERT(list);
    BSLS_ASSERT(reference);

    *list      = d_list_p;
    *reference = d_node_p;
    release();
}

template <class KEY, class DATA>
inline
void SkipListPairHandle<KEY, DATA>::reset(const SkipList<KEY, DATA> *list,
                                          Pair                      *reference)
{
    release();
    d_list_p = const_cast<SkipList<KEY, DATA> *>(list);
    d_node_p = reference;
}

// ACCESSORS
template <class KEY, class DATA>
inline
DATA& SkipListPairHandle<KEY, DATA>::data() const
{
    BSLS_ASSERT_SAFE(isValid());

    return SkipList<KEY, DATA>::data(d_node_p);
}

template <class KEY, class DATA>
inline
bool SkipListPairHandle<KEY, DATA>::isValid() const
{
    return d_node_p != 0 && d_list_p != 0;
}

template <class KEY, class DATA>
inline
const KEY& SkipListPairHandle<KEY, DATA>::key() const
{
    BSLS_ASSERT_SAFE(isValid());

    return SkipList<KEY, DATA>::key(d_node_p);
}

}  // close package namespace

// The scoping of "Pair" below should not be necessary, but xlC (versions 8 and
// 9) requires it.

template <class KEY, class DATA>
inline
bdlcc::SkipListPairHandle<KEY, DATA>::
                         operator const bdlcc::SkipListPair<KEY, DATA>*() const
{
    return d_node_p;
}

namespace bdlcc {

                     // ---------------------------------
                     // class SkipList_NodeCreationHelper
                     // ---------------------------------

template<class KEY, class DATA>
inline
SkipList_NodeCreationHelper<KEY, DATA>::SkipList_NodeCreationHelper(
                                              PoolManager      *poolManager,
                                              Node             *node,
                                              bslma::Allocator *basicAllocator)
: d_node_p(node)
, d_poolManager_p(poolManager)
, d_keyFlag(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template<class KEY, class DATA>
inline
SkipList_NodeCreationHelper<KEY, DATA>::~SkipList_NodeCreationHelper()
{
    if (d_node_p) {
        if (d_keyFlag) {
            d_node_p->d_key.~KEY();
        }
        PoolUtil::deallocate(d_poolManager_p, d_node_p);
    }
}

template<class KEY, class DATA>
inline
void SkipList_NodeCreationHelper<KEY, DATA>::construct(const KEY&  key,
                                                       const DATA& data)
{
    BSLS_ASSERT(d_node_p);

    bslalg::ScalarPrimitives::copyConstruct(
                                         BSLS_UTIL_ADDRESSOF(d_node_p->d_key),
                                         key,
                                         d_allocator_p);
    d_keyFlag = true;

    bslalg::ScalarPrimitives::copyConstruct(
                                         BSLS_UTIL_ADDRESSOF(d_node_p->d_data),
                                         data,
                                         d_allocator_p);

    d_node_p = 0;
}

                        // ---------------------------
                        // class SkipList::PairFactory
                        // ---------------------------

// CREATORS
template<class KEY, class DATA>
inline
SkipList<KEY, DATA>::PairFactory::PairFactory(SkipList *)
{}

// ACCESSOR
template<class KEY, class DATA>
inline
typename SkipList<KEY, DATA>::Pair
                *SkipList<KEY, DATA>::PairFactory::operator()(Node *node) const
{
    return reinterpret_cast<Pair *>(node);
}

                    // ---------------------------------
                    // class SkipList::PairHandleFactory
                    // ---------------------------------

// CREATORS
template<class KEY, class DATA>
inline
SkipList<KEY, DATA>::PairHandleFactory::PairHandleFactory(SkipList *list)
: d_list_p(list)
{}

// ACCESSOR
template<class KEY, class DATA>
inline
typename SkipList<KEY, DATA>::PairHandle
           SkipList<KEY, DATA>::PairHandleFactory::operator()(Node *node) const
{
    return PairHandle(d_list_p, reinterpret_cast<Pair *>(node));
}

                               // --------------
                               // class SkipList
                               // --------------

// PRIVATE CLASS METHODS
template<class KEY, class DATA>
inline
DATA& SkipList<KEY, DATA>::data(const Pair *reference)
{
    BSLS_ASSERT(reference);

    Node *node = static_cast<Node *>(static_cast<void *>(
                                               const_cast<Pair *>(reference)));
    return node->d_data;
}

template<class KEY, class DATA>
inline
const KEY& SkipList<KEY, DATA>::key(const Pair *reference)
{
    BSLS_ASSERT(reference);

    const Node *node = static_cast<const Node *>(
                                         static_cast<const void *>(reference));
    return node->d_key;
}


template<class KEY, class DATA>
inline
BSLS_KEYWORD_CONSTEXPR
bsls::Types::IntPtr SkipList<KEY, DATA>::offsetOfPtrs()
{
    typedef bsls::Types::IntPtr IntPtr;

    // The null pointer dereference is just used for taking offsets and sizes
    // in the 'Node' struct.  Note that we don't want to just create a default
    // constructed 'Node', because if 'KEY' or 'DATA' lack default constructors
    // then 'Node' has no default constructor.

    return reinterpret_cast<IntPtr>(&reinterpret_cast<Node*>(0)->d_ptrs);
}

template<class KEY, class DATA>
inline
typename SkipList<KEY, DATA>::Node
                              *SkipList<KEY, DATA>::pairToNode(Pair *reference)
{
    return static_cast<Node *>(static_cast<void *>(reference));
}

template<class KEY, class DATA>
inline
typename SkipList<KEY, DATA>::Node
                        *SkipList<KEY, DATA>::pairToNode(const Pair *reference)
{
    return static_cast<Node *>(static_cast<void *>(
                                               const_cast<Pair *>(reference)));
}


// PRIVATE MANIPULATORS
template<class KEY, class DATA>
void SkipList<KEY, DATA>::addNode(bool *newFrontFlag, Node *newNode)
{
    LockGuard guard(&d_lock);

    BSLS_ASSERT(newNode);
    BSLS_ASSERT(0 == newNode->d_ptrs[0].d_next_p);

    Node *location[k_MAX_NUM_LEVELS];
    lookupImpLowerBound(location, newNode->d_key);

    insertImp(newFrontFlag, location, newNode);
}

template<class KEY, class DATA>
void SkipList<KEY, DATA>::addNodeImpR(bool *newFrontFlag,
                                      Node *newNode,
                                      bool  lock)
{
    LockGuard lockGuard(&d_lock, !lock);
    if (!lock) {
        lockGuard.release();
    }

    BSLS_ASSERT(newNode);
    BSLS_ASSERT(0 == newNode->d_ptrs[0].d_next_p);

    Node *location[k_MAX_NUM_LEVELS];
    lookupImpUpperBoundR(location, newNode->d_key);

    insertImp(newFrontFlag, location, newNode);
}

template<class KEY, class DATA>
inline
void SkipList<KEY, DATA>::addNodeR(bool *newFrontFlag, Node *newNode)
{
    addNodeImpR(newFrontFlag, newNode, true);  // true -> lock
}

template<class KEY, class DATA>
int SkipList<KEY, DATA>::addNodeUnique(bool *newFrontFlag, Node *newNode)
{
    LockGuard guard(&d_lock);

    BSLS_ASSERT(newNode);
    BSLS_ASSERT(0 == newNode->d_ptrs[0].d_next_p);

    Node *location[k_MAX_NUM_LEVELS];
    lookupImpLowerBound(location, newNode->d_key);

    Node *q = location[0];
    if (q != d_tail_p && q->d_key == newNode->d_key) {
        return e_DUPLICATE;                                           // RETURN
    }

    insertImp(newFrontFlag, location, newNode);

    return 0;
}

template<class KEY, class DATA>
int SkipList<KEY, DATA>::addNodeUniqueR(bool *newFrontFlag, Node *newNode)
{
    LockGuard guard(&d_lock);

    BSLS_ASSERT(newNode);
    BSLS_ASSERT(0 == newNode->d_ptrs[0].d_next_p);

    Node *location[k_MAX_NUM_LEVELS];
    lookupImpLowerBoundR(location, newNode->d_key);

    Node *q = location[0];
    if (q != d_tail_p && q->d_key == newNode->d_key) {
        return e_DUPLICATE;                                           // RETURN
    }

    insertImp(newFrontFlag, location, newNode);

    return 0;
}

template<class KEY, class DATA>
SkipList_Node<KEY, DATA> *
SkipList<KEY, DATA>::allocateNode(int level, const KEY& key, const DATA& data)
{
    int listLevel = d_listLevel;
    if (level > listLevel) {
        level = listLevel + 1;
    }

    Node *node = reinterpret_cast<Node *>(PoolUtil::allocate(d_poolManager_p,
                                                             level));

    NodeGuard nodeGuard(d_poolManager_p, node, d_allocator_p);

    nodeGuard.construct(key, data);

    ++node->d_refCount;
    node->d_ptrs[0].d_next_p = 0;

    return node;
}

template<class KEY, class DATA>
void SkipList<KEY, DATA>::initialize()
{
    typedef bsls::Types::IntPtr IntPtr;

    static const int alignMask = bsls::AlignmentFromType<Node>::VALUE - 1;

    // Assert that this method has not been invoked.

    BSLS_ASSERT(0 == d_poolManager_p);

    int nodeSizes[k_MAX_NUM_LEVELS];

    const IntPtr offset = offsetOfPtrs();
    for (int i = 0; i < k_MAX_NUM_LEVELS; ++i) {
        IntPtr nodeSize = offset + (i + 1) * sizeof(typename Node::Ptrs);
        nodeSize = (nodeSize + alignMask) & ~alignMask;
        nodeSizes[i] = static_cast<int>(nodeSize);
    }

    d_poolManager_p = PoolUtil::createPoolManager(nodeSizes,
                                                  k_MAX_NUM_LEVELS,
                                                  d_allocator_p);

    d_head_p = reinterpret_cast<Node *>(PoolUtil::allocate(d_poolManager_p,
                                                           k_MAX_LEVEL));
    d_tail_p = reinterpret_cast<Node *>(PoolUtil::allocate(d_poolManager_p,
                                                           k_MAX_LEVEL));

    for (int i = 0; i < k_MAX_NUM_LEVELS; ++i) {
        d_head_p->d_ptrs[i].d_prev_p = 0;
        d_head_p->d_ptrs[i].d_next_p = d_tail_p;

        d_tail_p->d_ptrs[i].d_prev_p = d_head_p;
        d_tail_p->d_ptrs[i].d_next_p = 0;
    }
}

template<class KEY, class DATA>
void SkipList<KEY, DATA>::insertImp(bool *newFrontFlag,
                                    Node *location[],
                                    Node *node)
{
    BSLS_ASSERT(location);
    BSLS_ASSERT(node);

    int level = node->d_level;
    if (level > d_listLevel) {
        BSLS_ASSERT(level == d_listLevel + 1);

        d_listLevel = level;

        node->d_ptrs[level].d_prev_p = d_head_p;
        node->d_ptrs[level].d_next_p = d_tail_p;

        d_head_p->d_ptrs[level].d_next_p = node;
        d_tail_p->d_ptrs[level].d_prev_p = node;

        level--;
    }

    for (int k = level; k >= 0; --k) {
        Node *p = location[k]->d_ptrs[k].d_prev_p;
        Node *q = location[k];

        node->d_ptrs[k].d_prev_p = p;
        node->d_ptrs[k].d_next_p = q;

        p->d_ptrs[k].d_next_p = node;
        q->d_ptrs[k].d_prev_p = node;
    }

    if (newFrontFlag) {
        *newFrontFlag = (node->d_ptrs[0].d_prev_p == d_head_p);
    }

    ++d_length;
}

template<class KEY, class DATA>
void SkipList<KEY, DATA>::moveImp(bool *newFrontFlag,
                                  Node *location[],
                                  Node *node)
{
    BSLS_ASSERT(location);
    BSLS_ASSERT(node);

    int level = node->d_level;
    BSLS_ASSERT(level <= d_listLevel);

    for (int k = 0; k <= level; ++k) {
        Node *newP = location[k]->d_ptrs[k].d_prev_p;
        Node *newQ = location[k];

        if (newP == node || newQ == node) {
            // The node's already in the right place.  Since we started at
            // level 0, there's no more work to do.

            break;
        }

        Node *oldP = node->d_ptrs[k].d_prev_p;
        Node *oldQ = node->d_ptrs[k].d_next_p;

        oldQ->d_ptrs[k].d_prev_p = oldP;
        oldP->d_ptrs[k].d_next_p = oldQ;

        node->d_ptrs[k].d_prev_p = newP;
        node->d_ptrs[k].d_next_p = newQ;

        newP->d_ptrs[k].d_next_p = node;
        newQ->d_ptrs[k].d_prev_p = node;
    }

    if (newFrontFlag) {
        *newFrontFlag = (node->d_ptrs[0].d_prev_p == d_head_p);
    }
}

template<class KEY, class DATA>
SkipList_Node<KEY, DATA> *SkipList<KEY, DATA>::popFrontImp()
{
    LockGuard guard(&d_lock);

    Node *node = d_head_p->d_ptrs[0].d_next_p;
    if (node == d_tail_p) {
        return 0;                                                     // RETURN
    }

    int level = node->d_level;

    for (int k = level; k >= 0; --k) {
        Node *q = node->d_ptrs[k].d_next_p;
        q->d_ptrs[k].d_prev_p = d_head_p;
        d_head_p->d_ptrs[k].d_next_p = q;
    }

    node->d_ptrs[0].d_next_p = 0;
    --d_length;

    return node;
}

template<class KEY, class DATA>
inline
void SkipList<KEY, DATA>::releaseNode(Node *node)
{
    BSLS_ASSERT(node);

    const int refCnt = --node->d_refCount;
    if (!refCnt) {
        node->d_key.~KEY();
        node->d_data.~DATA();

        BSLS_ASSERT(0 == node->d_ptrs[0].d_next_p);

        PoolUtil::deallocate(d_poolManager_p, node);
    }
    else {
        BSLS_ASSERT_SAFE(0 < refCnt);
    }
}

template<class KEY, class DATA>
template<class VECTOR>
int SkipList<KEY, DATA>::removeAllMaybeUnlock(VECTOR *removed, bool unlock)
{
    typedef typename VECTOR::value_type ValueType;

    BSLMF_ASSERT((IsVector<VECTOR, ValueType>::value));

    BSLMF_ASSERT((bsl::is_same<ValueType, Pair *>::value
               || bsl::is_same<ValueType, PairHandle>::value));

    typedef typename bsl::conditional<bsl::is_same<ValueType, Pair *>::value,
                                      PairFactory,
                                      PairHandleFactory>::type FactoryType;

    BSLMT_MUTEXASSERT_IS_LOCKED(&d_lock);

    Node * const begin      = d_head_p;
    Node * const end        = d_tail_p->d_ptrs[0].d_prev_p;
    int          numRemoved = d_length;

    for (int ii = 0; ii <= d_listLevel; ++ii) {
        d_head_p->d_ptrs[ii].d_next_p = d_tail_p;
        d_tail_p->d_ptrs[ii].d_prev_p = d_head_p;
    }
    d_length = 0;

    for (Node *q = end; begin != q; q = q->d_ptrs[0].d_prev_p) {
        q->d_ptrs[0].d_next_p = 0;    // Marks node as removed from list,
                                      // must be done before mutex unlock.
    }

    if (unlock) {
        d_lock.unlock();
    }

    if (removed) {
        const FactoryType factory(this);

        // 'oldSize' must be a signed type to be compared to the subtraction
        // in the assertion after the loop.

        const bsls::Types::IntPtr oldSize = removed->size();
        removed->resize(oldSize + numRemoved);
        typename VECTOR::reverse_iterator removedIt = removed->rbegin();
        for (Node *q = end; begin != q; q = q->d_ptrs[0].d_prev_p) {
            *removedIt++ = factory(q);
        }
        BSLS_ASSERT(oldSize == removed->rend() - removedIt);
    }
    else {
        for (Node *q = end; begin != q; ) {
            Node *condemned = q;
            q = q->d_ptrs[0].d_prev_p;

            releaseNode(condemned);
        }
    }

    return numRemoved;
}

template<class KEY, class DATA>
template<class VECTOR>
int SkipList<KEY, DATA>::removeAllImp(VECTOR *removed)
{
    d_lock.lock();
    return removeAllMaybeUnlock(removed, true);
}

template<class KEY, class DATA>
int SkipList<KEY, DATA>::removeNode(Node *node)
{
    BSLS_ASSERT(node);

    LockGuard guard(&d_lock);

    if (0 == node->d_ptrs[0].d_next_p) {
        return e_NOT_FOUND;                                           // RETURN
    }

    int level = node->d_level;

    for (int k = level; k >= 0; --k) {
        Node *p = node->d_ptrs[k].d_prev_p;
        Node *q = node->d_ptrs[k].d_next_p;

        q->d_ptrs[k].d_prev_p = p;
        p->d_ptrs[k].d_next_p = q;
    }

    node->d_ptrs[0].d_next_p = 0;
    --d_length;
    return 0;
}

template<class KEY, class DATA>
int SkipList<KEY, DATA>::updateNode(bool       *newFrontFlag,
                                    Node       *node,
                                    const KEY&  newKey,
                                    bool        allowDuplicates)
{
    BSLS_ASSERT(node);

    LockGuard guard(&d_lock);

    if (0 == node->d_ptrs[0].d_next_p) {
        return e_NOT_FOUND;                                           // RETURN
    }

    Node *location[k_MAX_NUM_LEVELS];
    lookupImpLowerBound(location, newKey);

    if (!allowDuplicates) {
        Node *q = location[0];
        if (q != d_tail_p && q != node && q->d_key == newKey) {
            return e_DUPLICATE;                                       // RETURN
        }
    }

    node->d_key = newKey;  // may throw

    // now we are committed: change the list!

    moveImp(newFrontFlag, location, node);

    return 0;
}

template<class KEY, class DATA>
int SkipList<KEY, DATA>::updateNodeR(bool       *newFrontFlag,
                                     Node       *node,
                                     const KEY&  newKey,
                                     bool        allowDuplicates)
{
    BSLS_ASSERT(node);

    LockGuard guard(&d_lock);

    if (0 == node->d_ptrs[0].d_next_p) {
        return e_NOT_FOUND;                                           // RETURN
    }

    Node *location[k_MAX_NUM_LEVELS];

    if (!allowDuplicates) {
        lookupImpLowerBoundR(location, newKey);
        Node *p = location[0];
        if (p != d_tail_p && p != node && p->d_key == newKey) {
            return e_DUPLICATE;                                       // RETURN
        }
    }
    else {
        lookupImpUpperBoundR(location, newKey);
    }

    node->d_key = newKey;  // may throw

    // now we are committed: change the list!

    moveImp(newFrontFlag, location, node);

    return 0;
}

// PRIVATE ACCESSORS
template<class KEY, class DATA>
SkipList_Node<KEY, DATA> *
SkipList<KEY, DATA>::backNode() const
{
    LockGuard guard(&d_lock);

    Node *node = d_tail_p->d_ptrs[0].d_prev_p;
    if (node == d_head_p) {
        return 0;                                                     // RETURN
    }

    ++node->d_refCount;
    return node;
}

template<class KEY, class DATA>
void SkipList<KEY, DATA>::checkInvariants() const
{
    for (int ii = 0; ii <= d_listLevel; ++ii) {
        int numNodes = 0;
        Node *prev = d_head_p;
        for (Node *q = d_head_p->d_ptrs[ii].d_next_p; d_tail_p != q;
                                        prev = q, q = q->d_ptrs[ii].d_next_p) {
            ++numNodes;

            BSLS_ASSERT(q->d_ptrs[ii].d_prev_p == prev);

            BSLS_ASSERT(0 < q->d_refCount);
            BSLS_ASSERT(q->d_level >= ii);
            BSLS_ASSERT(q->d_level <= d_listLevel);

            for (int jj = ii - 1; 0 <= jj; --jj) {
                BSLS_ASSERT(q->d_ptrs[jj].d_next_p);
                BSLS_ASSERT(q->d_ptrs[jj].d_prev_p);
            }
        }

        BSLS_ASSERT(numNodes <= d_length);  (void)numNodes;
        BSLS_ASSERT(0 != ii || numNodes == d_length);

        BSLS_ASSERT(0 == d_head_p->d_ptrs[ii].d_prev_p);
        BSLS_ASSERT(0 == d_tail_p->d_ptrs[ii].d_next_p);
    }
}

template<class KEY, class DATA>
SkipList_Node<KEY, DATA> *SkipList<KEY, DATA>::findNode(const KEY& key) const
{
    Node *locator[k_MAX_NUM_LEVELS];

    LockGuard guard(&d_lock);
    lookupImpLowerBound(locator, key);

    Node *q = locator[0];
    if (q != d_tail_p && q->d_key == key) {
        ++q->d_refCount;
        return q;                                                     // RETURN
    }

    return 0;
}

template<class KEY, class DATA>
SkipList_Node<KEY, DATA> *SkipList<KEY, DATA>::findNodeR(const KEY& key) const
{
    Node *locator[k_MAX_NUM_LEVELS];

    LockGuard guard(&d_lock);
    lookupImpUpperBoundR(locator, key);

    Node *p = locator[0];
    if (d_head_p != p) {
        p = p->d_ptrs[0].d_prev_p;
        if (d_head_p != p && key == p->d_key) {
            ++p->d_refCount;
            return p;                                                 // RETURN
        }
    }

    return 0;
}

template<class KEY, class DATA>
SkipList_Node<KEY, DATA> *SkipList<KEY, DATA>::findNodeLowerBound(
                                                          const KEY& key) const
{
    Node *locator[k_MAX_NUM_LEVELS];

    LockGuard guard(&d_lock);
    lookupImpLowerBound(locator, key);

    Node *q = locator[0];
    if (q != d_tail_p) {
        ++q->d_refCount;
        return q;                                                     // RETURN
    }

    return 0;
}

template<class KEY, class DATA>
SkipList_Node<KEY, DATA> *SkipList<KEY, DATA>::findNodeUpperBound(
                                                          const KEY& key) const
{
    Node *locator[k_MAX_NUM_LEVELS];

    LockGuard guard(&d_lock);
    lookupImpUpperBound(locator, key);

    Node *q = locator[0];
    if (q != d_tail_p) {
        ++q->d_refCount;
        return q;                                                     // RETURN
    }

    return 0;
}

template<class KEY, class DATA>
SkipList_Node<KEY, DATA> *SkipList<KEY, DATA>::findNodeLowerBoundR(
                                                          const KEY& key) const
{
    Node *locator[k_MAX_NUM_LEVELS];

    LockGuard guard(&d_lock);
    lookupImpLowerBoundR(locator, key);

    Node *q = locator[0];
    if (q != d_tail_p) {
        ++q->d_refCount;
        return q;                                                     // RETURN
    }

    return 0;
}

template<class KEY, class DATA>
SkipList_Node<KEY, DATA> *SkipList<KEY, DATA>::findNodeUpperBoundR(
                                                          const KEY& key) const
{
    Node *locator[k_MAX_NUM_LEVELS];

    LockGuard guard(&d_lock);
    lookupImpUpperBoundR(locator, key);

    Node *q = locator[0];
    if (q != d_tail_p) {
        ++q->d_refCount;
        return q;                                                     // RETURN
    }

    return 0;
}

template<class KEY, class DATA>
SkipList_Node<KEY, DATA> *SkipList<KEY, DATA>::frontNode() const
{
    LockGuard guard(&d_lock);

    Node *node = d_head_p->d_ptrs[0].d_next_p;
    if (node == d_tail_p) {
        return 0;                                                     // RETURN
    }

    ++node->d_refCount;
    return node;
}

template<class KEY, class DATA>
void SkipList<KEY, DATA>::lookupImpLowerBound(Node       *location[],
                                              const KEY&  key) const
{
    Node *p = d_head_p;
    for (int k = d_listLevel; k >= 0; --k) {
        Node *q = p->d_ptrs[k].d_next_p;
        while (q != d_tail_p && q->d_key < key) {
            p = q;
            q = p->d_ptrs[k].d_next_p;
        }
        location[k] = q;
    }

    BSLS_ASSERT_SAFE(d_head_p != location[0]);
}

template<class KEY, class DATA>
void SkipList<KEY, DATA>::lookupImpLowerBoundR(Node       *location[],
                                               const KEY&  key) const
{
    Node *q = d_tail_p;
    for (int k = d_listLevel; k >= 0; --k) {
        Node *p = q->d_ptrs[k].d_prev_p;
        while (p != d_head_p && !(p->d_key < key)) {
            q = p;
            p = p->d_ptrs[k].d_prev_p;
        }
        location[k] = q;
    }

    BSLS_ASSERT_SAFE(d_head_p != location[0]);
}

template<class KEY, class DATA>
void SkipList<KEY, DATA>::lookupImpUpperBound(Node       *location[],
                                              const KEY&  key) const
{
    Node *p = d_head_p;
    for (int k = d_listLevel; k >= 0; --k) {
        Node *q = p->d_ptrs[k].d_next_p;
        while (q != d_tail_p && !(key < q->d_key)) {
            p = q;
            q = p->d_ptrs[k].d_next_p;
        }

        location[k] = q;
    }

    BSLS_ASSERT_SAFE(d_head_p != location[0]);
}

template<class KEY, class DATA>
void SkipList<KEY, DATA>::lookupImpUpperBoundR(Node       *location[],
                                               const KEY&  key) const
{
    Node *q = d_tail_p;
    for (int k = d_listLevel; k >= 0; --k) {
        Node *p = q->d_ptrs[k].d_prev_p;
        while (p != d_head_p && key < p->d_key) {
            q = p;
            p = q->d_ptrs[k].d_prev_p;
        }
        location[k] = q;
    }

    BSLS_ASSERT_SAFE(d_head_p != location[0]);
}

template<class KEY, class DATA>
SkipList_Node<KEY, DATA> *
SkipList<KEY, DATA>::nextNode(Node *node) const
{
    BSLS_ASSERT(node != d_head_p);
    BSLS_ASSERT(node != d_tail_p);

    LockGuard guard(&d_lock);

    Node *next = node->d_ptrs[0].d_next_p;
    if (0 == next || d_tail_p == next) {
        return 0;                                                     // RETURN
    }

    ++next->d_refCount;
    return next;
}

template<class KEY, class DATA>
SkipList_Node<KEY, DATA> *
SkipList<KEY, DATA>::prevNode(Node *node) const
{
    BSLS_ASSERT(node != d_head_p);
    BSLS_ASSERT(node != d_tail_p);

    LockGuard guard(&d_lock);
    if (0 == node->d_ptrs[0].d_next_p) {
        return 0;                                                     // RETURN
    }

    Node *prev = node->d_ptrs[0].d_prev_p;
    if (d_head_p == prev) {
        return 0;                                                     // RETURN
    }

    ++prev->d_refCount;
    return prev;
}

template<class KEY, class DATA>
int SkipList<KEY, DATA>::skipBackward(Node **node) const
{
    BSLS_ASSERT(node);

    Node *current = *node;
    BSLS_ASSERT(current);
    BSLS_ASSERT(current != d_head_p);
    BSLS_ASSERT(current != d_tail_p);

    LockGuard guard(&d_lock);

    if (0 == current->d_ptrs[0].d_next_p) {
        // The node is no longer on the list.

        return e_NOT_FOUND;                                           // RETURN
    }

    const int count = --current->d_refCount;
    BSLS_ASSERT(0 < count);
    (void) count;    // suppress 'unused variable' warnings

    Node *prev = current->d_ptrs[0].d_prev_p;
    if (d_head_p == prev) {
        *node = 0;
        return 0;                                                     // RETURN
    }

    ++prev->d_refCount;
    *node = prev;
    return 0;
}

template<class KEY, class DATA>
int SkipList<KEY, DATA>::skipForward(Node **node) const
{
    BSLS_ASSERT(node);

    Node *current = *node;
    BSLS_ASSERT(current);
    BSLS_ASSERT(current != d_head_p);
    BSLS_ASSERT(current != d_tail_p);

    LockGuard guard(&d_lock);

    if (0 == current->d_ptrs[0].d_next_p) {
        // The node is no longer on the list.

        return e_NOT_FOUND;                                           // RETURN
    }

    const int count = --current->d_refCount;
    BSLS_ASSERT(0 < count);
    (void) count;    // suppress 'unused variable' warnings

    Node *next = current->d_ptrs[0].d_next_p;
    if (d_tail_p == next) {
        *node = 0;
        return 0;                                                     // RETURN
    }

    ++next->d_refCount;
    *node = next;
    return 0;
}

// CLASS METHODS
template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::level(const Pair *reference)
{
    BSLS_ASSERT(reference);

    Node *node = pairToNode(reference);
    return node->d_level;
}

// CREATORS
template<class KEY, class DATA>
SkipList<KEY, DATA>::SkipList(bslma::Allocator *basicAllocator)
: d_listLevel(0)
, d_length(0)
, d_poolManager_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    initialize();
}

template<class KEY, class DATA>
SkipList<KEY, DATA>::SkipList(const SkipList&   original,
                              bslma::Allocator *basicAllocator)
: d_listLevel(0)
, d_length(0)
, d_poolManager_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    initialize();

    *this = original;
}

template<class KEY, class DATA>
SkipList<KEY, DATA>::~SkipList()
{
#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    checkInvariants();
#endif

    for (Node *p = d_tail_p->d_ptrs[0].d_prev_p; d_head_p != p; ) {
        BSLS_ASSERT(1 == p->d_refCount);

        Node *condemned = p;
        p = p->d_ptrs[0].d_prev_p;
        condemned->d_ptrs[0].d_next_p = 0;

        releaseNode(condemned);
    }

    PoolUtil::deallocate(d_poolManager_p, d_head_p);
    PoolUtil::deallocate(d_poolManager_p, d_tail_p);

    PoolUtil::deletePoolManager(d_allocator_p, d_poolManager_p);
}

// MANIPULATORS
template<class KEY, class DATA>
SkipList<KEY, DATA>&
SkipList<KEY, DATA>::operator=(const SkipList& rhs)
{
    if (&rhs == this) {
        return *this;                                                 // RETURN
    }

    DoubleLockGuard guard(&d_lock, &rhs.d_lock);

    // first empty this list

    removeAllMaybeUnlock(static_cast<bsl::vector<Pair *> *>(0), false);

    // Now get handles to all of 'rhs's elements.  Since rhs.d_lock is locked,
    // we need to do all operations manually because the important functions of
    // 'rhs' (like frontNode and nextNode) will lock the mutex.

    bsl::vector<PairHandle> rhsElements;
    rhsElements.reserve(rhs.d_length);
    for (Node *node = rhs.d_head_p->d_ptrs[0].d_next_p;
         node && node != rhs.d_tail_p;
         node = node->d_ptrs[0].d_next_p)
    {
        ++node->d_refCount;
        rhsElements.insert(rhsElements.end(),
                           PairHandle())->reset(
                                               &rhs,
                                               reinterpret_cast<Pair *>(node));
    }

    // Note that unlocking 'rhs.d_lock' here causes a data race if another
    // thread calls 'update' or 'updateR' on a node in 'rhs'.

    for (typename bsl::vector<PairHandle>::iterator it = rhsElements.begin();
         it != rhsElements.end(); ++it) {
        Node *node = allocateNode(d_rand.randomLevel(),
                                  it->key(), it->data());
        addNodeImpR(0, node, false);  // false -> do not lock (already locked)
    }

    return *this;
}

template<class KEY, class DATA>
inline
void SkipList<KEY, DATA>::releaseReferenceRaw(const Pair *reference)
{
    Node *node = pairToNode(reference);
    releaseNode(node);
}

                         // Insertion Methods

template<class KEY, class DATA>
inline
void SkipList<KEY, DATA>::add(PairHandle  *result,
                              const KEY&   key,
                              const DATA&  data,
                              bool        *newFrontFlag)
{
    Pair *handle;
    addRaw(&handle, key, data, newFrontFlag);
    result->reset(this, handle);
}

template<class KEY, class DATA>
inline
void SkipList<KEY, DATA>::add(const KEY&   key,
                              const DATA&  data,
                              bool        *newFrontFlag)
{
    Pair **zeroPair = 0;
    addRaw(zeroPair, key, data, newFrontFlag);
}

template<class KEY, class DATA>
inline
void SkipList<KEY, DATA>::addAtLevelRaw(Pair        **result,
                                        int           level,
                                        const KEY&    key,
                                        const DATA&   data,
                                        bool         *newFrontFlag)
{
    Node *node = allocateNode(level, key, data);
    if (result) {
        ++node->d_refCount;
        *result = reinterpret_cast<Pair *>(node);
    }

    addNode(newFrontFlag, node);
}

template<class KEY, class DATA>
int SkipList<KEY, DATA>::addAtLevelUniqueRaw(Pair        **result,
                                             int           level,
                                             const KEY&    key,
                                             const DATA&   data,
                                             bool         *newFrontFlag)
{
    Node *node = allocateNode(level, key, data);
    if (result) {
        ++node->d_refCount;
        *result = reinterpret_cast<Pair *>(node);
    }

    int ret = addNodeUnique(newFrontFlag, node);
    if (ret) {
        if (result) {
            --node->d_refCount;
            *result = 0;
        }
        releaseNode(node);
        return ret;                                                   // RETURN
    }

    return 0;
}

template<class KEY, class DATA>
inline
void SkipList<KEY, DATA>::addRaw(Pair        **result,
                                 const KEY&    key,
                                 const DATA&   data,
                                 bool         *newFrontFlag)
{
    addAtLevelRaw(result, d_rand.randomLevel(), key, data, newFrontFlag);
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::addUnique(PairHandle  *result,
                                   const KEY&   key,
                                   const DATA&  data,
                                   bool        *newFrontFlag)
{
    Pair *handle;
    int   rc = addUniqueRaw(&handle, key, data, newFrontFlag);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }
    result->reset(this, handle);
    return 0;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::addUnique(const KEY&   key,
                                   const DATA&  data,
                                   bool        *newFrontFlag)
{

    Pair **zeroPair = 0;
    return addUniqueRaw(zeroPair, key, data, newFrontFlag);
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::addUniqueRaw(Pair        **result,
                                      const KEY&    key,
                                      const DATA&   data,
                                      bool         *newFrontFlag)
{
    return addAtLevelUniqueRaw(result, d_rand.randomLevel(), key, data,
                               newFrontFlag);
}

                         // Insertion Methods (Reverse Search)

template<class KEY, class DATA>
inline
void SkipList<KEY, DATA>::addAtLevelRawR(Pair        **result,
                                         int           level,
                                         const KEY&    key,
                                         const DATA&   data,
                                         bool         *newFrontFlag)
{
    Node *node = allocateNode(level, key, data);
    if (result) {
        ++node->d_refCount;
        *result = reinterpret_cast<Pair *>(node);
    }

    addNodeR(newFrontFlag, node);
}

template<class KEY, class DATA>
int SkipList<KEY, DATA>::addAtLevelUniqueRawR(Pair        **result,
                                              int           level,
                                              const KEY&    key,
                                              const DATA&   data,
                                              bool         *newFrontFlag)
{
    Node *node = allocateNode(level, key, data);
    if (result) {
        ++node->d_refCount;
        *result = reinterpret_cast<Pair *>(node);
    }

    int ret = addNodeUniqueR(newFrontFlag, node);
    if (ret) {
        if (result) {
            --node->d_refCount;
            *result = 0;
        }
        releaseNode(node);
        return ret;                                                   // RETURN
    }

    return 0;
}

template<class KEY, class DATA>
inline
void SkipList<KEY, DATA>::addR(PairHandle  *result,
                               const KEY&   key,
                               const DATA&  data,
                               bool        *newFrontFlag)
{
    Pair *handle;
    addRawR(&handle, key, data, newFrontFlag);
    result->reset(this, handle);
}

template<class KEY, class DATA>
inline
void SkipList<KEY, DATA>::addR(const KEY&   key,
                               const DATA&  data,
                               bool        *newFrontFlag)
{
    Pair **zeroPair = 0;
    addRawR(zeroPair, key, data, newFrontFlag);
}

template<class KEY, class DATA>
inline
void SkipList<KEY, DATA>::addRawR(Pair        **result,
                                  const KEY&    key,
                                  const DATA&   data,
                                  bool         *newFrontFlag)
{
    addAtLevelRawR(result, d_rand.randomLevel(), key, data, newFrontFlag);
}

template<class KEY, class DATA>
int SkipList<KEY, DATA>::addUniqueR(PairHandle  *result,
                                    const KEY&   key,
                                    const DATA&  data,
                                    bool        *newFrontFlag)
{
    Pair *handle;
    int   rc = addUniqueRawR(&handle, key, data, newFrontFlag);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }
    result->reset(this, handle);

    return 0;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::addUniqueR(const KEY&   key,
                                    const DATA&  data,
                                    bool        *newFrontFlag)
{
    Pair **zeroPair = 0;
    return addUniqueRawR(zeroPair, key, data, newFrontFlag);
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::addUniqueRawR(Pair        **result,
                                       const KEY&    key,
                                       const DATA&   data,
                                       bool         *newFrontFlag)
{
    return addAtLevelUniqueRawR(result, d_rand.randomLevel(), key, data,
                                newFrontFlag);
}

                             // Removal Methods

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::popFront(PairHandle *item)
{
    Node *node = popFrontImp();
    if (!node) {
        return e_NOT_FOUND;                                           // RETURN
    }

    if (item) {
        item->reset(this, reinterpret_cast<Pair *>(node));
    }
    else {
        releaseNode(node);
    }

    return 0;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::popFrontRaw(Pair **item)
{
    Node *node = popFrontImp();
    if (!node) {
        return e_NOT_FOUND;                                           // RETURN
    }

    if (item) {
        *item = reinterpret_cast<Pair *>(node);
    }
    else {
        releaseNode(node);
    }

    return 0;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::remove(const Pair *reference)
{
    if (0 == reference) {
        return e_INVALID;                                             // RETURN
    }

    Node *node = pairToNode(reference);

    int ret = removeNode(node);
    if (ret) {
        return ret;                                                   // RETURN
    }

    releaseNode(node);
    return 0;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::removeAll()
{
    return removeAllImp(static_cast<bsl::vector<PairHandle> *>(0));
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::removeAll(bsl::vector<PairHandle> *removed)
{
    return removeAllImp(removed);
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::removeAll(std::vector<PairHandle> *removed)
{
    return removeAllImp(removed);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::removeAll(std::pmr::vector<PairHandle> *removed)
{
    return removeAllImp(removed);
}
#endif

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::removeAllRaw(bsl::vector<Pair *> *removed)
{
    return removeAllImp(removed);
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::removeAllRaw(std::vector<Pair *> *removed)
{
    return removeAllImp(removed);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::removeAllRaw(std::pmr::vector<Pair *> *removed)
{
    return removeAllImp(removed);
}
#endif

                         // Update Methods

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::update(const Pair *reference,
                                const KEY&  newKey,
                                bool       *newFrontFlag,
                                bool        allowDuplicates)
{
    if (0 == reference) {
        return e_INVALID;                                             // RETURN
    }

    Node *node = pairToNode(reference);
    return updateNode(newFrontFlag, node, newKey, allowDuplicates);
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::updateR(const Pair *reference,
                                 const KEY&  newKey,
                                 bool       *newFrontFlag,
                                 bool        allowDuplicates)
{
    if (0 == reference) {
        return e_INVALID;                                             // RETURN
    }

    Node *node = pairToNode(reference);
    return updateNodeR(newFrontFlag, node, newKey, allowDuplicates);
}

// ACCESSORS
template<class KEY, class DATA>
inline
SkipListPair<KEY, DATA> *
SkipList<KEY, DATA>::addPairReferenceRaw(const Pair *reference) const
{
    Node *node = pairToNode(reference);
    ++node->d_refCount;
    return const_cast<Pair *>(reference);
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::back(PairHandle *back) const
{
    Pair *backPtr = reinterpret_cast<Pair *>(backNode());
    if (backPtr) {
        back->reset(this, backPtr);
        return 0;                                                     // RETURN
    }
    return -1;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::backRaw(Pair **back) const
{
    *back = reinterpret_cast<Pair *>(backNode());
    return *back ? 0 : -1;
}

template<class KEY, class DATA>
bool SkipList<KEY, DATA>::exists(const KEY& key) const
{
    Node *locator[k_MAX_NUM_LEVELS];

    LockGuard guard(&d_lock);
    lookupImpLowerBound(locator, key);

    Node *q = locator[0];
    if (q != d_tail_p && q->d_key == key) {
        return true;                                                  // RETURN
    }

    return false;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::front(PairHandle *front) const
{
    BSLS_ASSERT(front);

    Pair *frontPtr = reinterpret_cast<Pair *>(frontNode());
    if (frontPtr) {
        front->reset(this, frontPtr);
        return 0;                                                     // RETURN
    }
    return -1;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::frontRaw(Pair **front) const
{
    BSLS_ASSERT(front);

    *front = reinterpret_cast<Pair *>(frontNode());
    return *front ? 0 : -1;
}

template<class KEY, class DATA>
inline
bool SkipList<KEY, DATA>::isEmpty() const
{
    LockGuard guard(&d_lock);

    return d_tail_p == d_head_p->d_ptrs[0].d_next_p;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::length() const
{
    LockGuard guard(&d_lock);

    return d_length;
}

template<class KEY, class DATA>
bsl::ostream&
SkipList<KEY, DATA>::print(bsl::ostream& stream,
                           int           level,
                           int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bdlb::Print::indent(stream, level, spacesPerLevel);

    LockGuard guard(&d_lock);
    // Now we must do all operations manually, since all important functions
    // like frontNode() and nextNode will lock the mutex

    if (0 <= spacesPerLevel) {
        // Multi-line output.

        if (level < 0) {
            level = -level;
        }

        stream << "[\n";

        const int levelPlus1 = level + 1;

        for (Node *node = d_head_p->d_ptrs[0].d_next_p;
             node && node != d_tail_p;
             node = node->d_ptrs[0].d_next_p) {
            bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
            stream << "[\n";

            const int levelPlus2 = level + 2;
            bdlb::Print::indent(stream, levelPlus2, spacesPerLevel);
            stream << "level = " << node->d_level << "\n";

            bdlb::PrintMethods::print(stream,
                                      node->d_key,
                                      levelPlus2,
                                      spacesPerLevel);

            bdlb::Print::indent(stream, levelPlus2, spacesPerLevel);
            stream << "=>\n";

            bdlb::PrintMethods::print(stream,
                                      node->d_data,
                                      levelPlus2,
                                      spacesPerLevel);
            bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
            stream << "]\n";
        }

        bdlb::Print::indent(stream, level, spacesPerLevel);

        stream << "]\n";
    }
    else {
        // Output on a single line and suppress any further indentation.

        stream << "[";

        for (Node *node = d_head_p->d_ptrs[0].d_next_p;
             node && node != d_tail_p;
             node = node->d_ptrs[0].d_next_p) {
            stream << "[ (level = " << node->d_level << ") ";

            bdlb::PrintMethods::print(stream, node->d_key, 0, -1);
            stream << " => ";
            bdlb::PrintMethods::print(stream, node->d_data, 0, -1);

            stream << " ]";

        }

        stream << "]";
    }

    return stream << bsl::flush;
}

                            // simple forward finds

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::find(PairHandle *item, const KEY& key) const
{
    BSLS_ASSERT(item);

    Pair *itemPtr = reinterpret_cast<Pair *>(findNode(key));
    if (itemPtr) {
        item->reset(this, itemPtr);
        return 0;                                                     // RETURN
    }
    return -1;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::findRaw(Pair **item, const KEY& key) const
{
    BSLS_ASSERT(item);

    *item = reinterpret_cast<Pair *>(findNode(key));
    return *item ? 0 : -1;
}

                            // simple reverse finds

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::findR(PairHandle *item, const KEY& key) const
{
    BSLS_ASSERT(item);

    Pair *itemPtr = reinterpret_cast<Pair *>(findNodeR(key));
    if (itemPtr) {
        item->reset(this, itemPtr);
        return 0;                                                     // RETURN
    }
    return -1;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::findRRaw(Pair **item, const KEY& key) const
{
    BSLS_ASSERT(item);

    *item = reinterpret_cast<Pair *>(findNodeR(key));
    return *item ? 0 : -1;
}

                        // find lower bound

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::findLowerBound(PairHandle *item, const KEY& key) const
{
    BSLS_ASSERT(item);

    Pair *itemPtr = reinterpret_cast<Pair *>(findNodeLowerBound(key));
    if (itemPtr) {
        item->reset(this, itemPtr);
        return 0;                                                     // RETURN
    }
    return -1;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::findLowerBoundRaw(Pair **item, const KEY& key) const
{
    BSLS_ASSERT(item);

    *item = reinterpret_cast<Pair *>(findNodeLowerBound(key));
    return *item ? 0 : -1;
}

                        // find lower bound reverse

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::findLowerBoundR(PairHandle *item,
                                         const KEY&  key) const
{
    BSLS_ASSERT(item);

    Pair *itemPtr = reinterpret_cast<Pair *>(findNodeLowerBoundR(key));
    if (itemPtr) {
        item->reset(this, itemPtr);
        return 0;                                                     // RETURN
    }
    return -1;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::findLowerBoundRRaw(Pair **item, const KEY& key) const
{
    BSLS_ASSERT(item);

    *item = reinterpret_cast<Pair *>(findNodeLowerBoundR(key));
    return *item ? 0 : -1;
}

                        // find upper bound

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::findUpperBound(PairHandle *item,
                                        const KEY&  key) const
{
    BSLS_ASSERT(item);

    Pair *itemPtr = reinterpret_cast<Pair *>(findNodeUpperBound(key));
    if (itemPtr) {
        item->reset(this, itemPtr);
        return 0;                                                     // RETURN
    }
    return -1;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::findUpperBoundRaw(Pair **item, const KEY& key) const
{
    BSLS_ASSERT(item);

    *item = reinterpret_cast<Pair *>(findNodeUpperBound(key));
    return *item ? 0 : -1;
}

                        // find upper bound reverse

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::findUpperBoundR(PairHandle *item,
                                         const KEY&  key) const
{
    BSLS_ASSERT(item);

    Pair *itemPtr = reinterpret_cast<Pair *>(findNodeUpperBoundR(key));
    if (itemPtr) {
        item->reset(this, itemPtr);
        return 0;                                                     // RETURN
    }
    return -1;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::findUpperBoundRRaw(Pair **item, const KEY& key) const
{
    BSLS_ASSERT(item);

    *item = reinterpret_cast<Pair *>(findNodeUpperBoundR(key));
    return *item ? 0 : -1;
}

                            // next, previous, & skip*
template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::next(PairHandle *next, const Pair *reference) const
{
    if (0 == reference) {
        return e_INVALID;                                             // RETURN
    }

    Node *node  = pairToNode(reference);
    Node *nNode = nextNode(node);
    if (nNode) {
        next->reset(this, reinterpret_cast<Pair *>(nNode));
        return 0;                                                     // RETURN
    }
    return -1;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::nextRaw(Pair **next, const Pair *reference) const
{
    BSLS_ASSERT(next);
    BSLS_ASSERT(reference);

    Node *node = pairToNode(reference);
    *next = reinterpret_cast<Pair *>(nextNode(node));

    return *next ? 0 : -1;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::previous(PairHandle *prevPair,
                                  const Pair *reference) const
{
    if (0 == reference) {
        return e_INVALID;                                             // RETURN
    }

    Node *node  = pairToNode(reference);
    Node *pNode = prevNode(node);
    if (pNode) {
        prevPair->reset(this, reinterpret_cast<Pair *>(pNode));
        return 0;                                                     // RETURN
    }
    return -1;
}

template<class KEY, class DATA>
inline
int
SkipList<KEY, DATA>::previousRaw(Pair **prevPair, const Pair *reference) const
{
    BSLS_ASSERT(prevPair);
    BSLS_ASSERT(reference);

    Node *node = pairToNode(reference);
    *prevPair = reinterpret_cast<Pair *>(prevNode(node));
    return *prevPair ? 0 : -1;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::skipBackward(PairHandle *item) const
{
    BSLS_ASSERT(item->isValid());

    Node **node_p = reinterpret_cast<Node **>(&item->d_node_p);
    return skipBackward(node_p);
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::skipBackwardRaw(Pair **item) const
{
    BSLS_ASSERT(item);

    Node **node_p = reinterpret_cast<Node **>(item);
    return skipBackward(node_p);
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::skipForward(PairHandle *item) const
{
    BSLS_ASSERT(item->isValid());

    Node **node_p = reinterpret_cast<Node **>(&item->d_node_p);
    return skipForward(node_p);
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::skipForwardRaw(Pair **item) const
{
    BSLS_ASSERT(item);

    Node **node_p = reinterpret_cast<Node **>(item);
    return skipForward(node_p);
}

                                  // Aspects

template<class KEY, class DATA>
inline
bslma::Allocator *SkipList<KEY, DATA>::allocator() const
{
    return d_allocator_p;
}

}  // close package namespace

// FREE OPERATORS
template<class KEY, class DATA>
bool bdlcc::operator==(const SkipList<KEY, DATA>& lhs,
                       const SkipList<KEY, DATA>& rhs)
{
    if (&lhs == &rhs) {
        return true;                                                  // RETURN
    }

    bdlcc::SkipList_DoubleLockGuard guard(&lhs.d_lock, &rhs.d_lock);

    // Once we have locked the lists, we need to do all operations manually
    // because the important functions of the lists (like frontNode and
    // nextNode) will lock the mutex.

    for (SkipList_Node<KEY, DATA>
              *lhsNode = lhs.d_head_p->d_ptrs[0].d_next_p,
              *rhsNode = rhs.d_head_p->d_ptrs[0].d_next_p;
         ;
         lhsNode = lhsNode->d_ptrs[0].d_next_p,
         rhsNode = rhsNode->d_ptrs[0].d_next_p)
    {
        if ((!lhsNode && !rhsNode)
         || (lhsNode == lhs.d_tail_p && rhsNode == rhs.d_tail_p)) {
            // we reached the end of both lists at the same time

            return true;                                              // RETURN
        }
        if (!lhsNode || !rhsNode
         || lhsNode == lhs.d_tail_p || rhsNode == rhs.d_tail_p) {
            // We reached the end of one list before the other

            return false;                                             // RETURN
        }

        if (!(lhsNode->d_key  == rhsNode->d_key
           && lhsNode->d_data == rhsNode->d_data)) {
            return false;                                             // RETURN
        }
    }

    BSLS_ASSERT(!"unreachable");

    return false;
}

template<class KEY, class DATA>
bool bdlcc::operator!=(const SkipList<KEY, DATA>& lhs,
                       const SkipList<KEY, DATA>& rhs)
{
    if (&lhs == &rhs) {
        return false;                                                 // RETURN
    }

    bdlcc::SkipList_DoubleLockGuard guard(&lhs.d_lock, &rhs.d_lock);

    // Once we have locked the lists, we need to do all operations manually
    // because the important functions of the lists (like frontNode and
    // nextNode) will lock the mutex.

    for (SkipList_Node<KEY, DATA>
              *lhsNode = lhs.d_head_p->d_ptrs[0].d_next_p,
              *rhsNode = rhs.d_head_p->d_ptrs[0].d_next_p;
         ;
         lhsNode = lhsNode->d_ptrs[0].d_next_p,
         rhsNode = rhsNode->d_ptrs[0].d_next_p)
    {
        if ((!lhsNode && !rhsNode)
         || (lhsNode == lhs.d_tail_p && rhsNode == rhs.d_tail_p)) {
            // we reached the end of both lists at the same time

            return false;                                             // RETURN
        }
        if (!lhsNode || !rhsNode
         || lhsNode == lhs.d_tail_p || rhsNode == rhs.d_tail_p) {
            // We reached the end of one list before the other

            return true;                                              // RETURN
        }

        if (!(lhsNode->d_key  == rhsNode->d_key)
         || !(lhsNode->d_data == rhsNode->d_data)) {
            return true;                                              // RETURN
        }
    }

    BSLS_ASSERT(!"unreachable");

    return false;
}

template<class KEY, class DATA>
inline
bsl::ostream& bdlcc::operator<<(bsl::ostream&              stream,
                                const SkipList<KEY, DATA>& list)
{
    return list.print(stream, 0, -1);
}

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
