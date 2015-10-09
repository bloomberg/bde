// bdlcc_skiplist.h                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLCC_SKIPLIST
#define INCLUDED_BDLCC_SKIPLIST

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
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
// looked up (either by key or by position).  There is an
// implementation-defined maximum number of references (whether by
// 'bdlcc::SkipListPairHandle' or 'bdlcc::SkipListPair' pointer) to any single
// association element in the list, not less than
// '2^bdlcc::SkipList_Control::k_NUM_REFERENCE_BITS - 1'.  The behavior of this
// component is undefined if more than that number of references are
// simultaneously acquired for a single element.  Note that in addition to
// 'addPairReferenceRaw', member functions of 'bdlcc::SkipList' such as
// 'front', 'back', and 'find' also add a reference to the specified element.
//
///Template Requirements
///---------------------
// The 'bdlcc::SkipList' ordered associative container is parameterized on two
// types, 'KEY' and 'DATA'.  Each type must have a public copy constructor, and
// it is important to declare the "Uses Bdema Allocator" trait if the type
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
//:     key value.  Also a type name used fororeferences* to such objects
//:     ('bdlcc:SkipListPair' objects cannot be constructed directly).
//:
//: PairHandle:
//:     An object (of type 'bdlcc::SkipListPairHandle') with scope and copy
//:     semantics which make it easier to manage and use than a raw
//:     'bdlcc::SkipListPair*'.
//:
//: R:
//:     Stands for "Reverse search" (see '"R" Methods' documentation below).
//:
//: Reference:
//:     An object referring to a pair; either a 'bdlcc::SkipListPair*' which
//:     has not yet been released, or a 'bdlcc::SkipListPairHandle' object.
//
///"R" Methods: Optimized Search From The Back Of The List
///-------------------------------------------------------
// All methods of 'bdlcc::SkipList' that result in a search through the list
// have corresponding "R" versions: for example, there are 'add' and 'addR'
// methods, 'find' and 'findR' methods, etc.  The "R" versions of these methods
// search from the back of the list (i.e., in descending (reverse) order).  Use
// of an "R" method is a hint to the Skip List that the desired key is more
// likely to be near the back than the front.  In no case does the use of one
// version of a method over the other affect the correctness of the result.
// Note that if there are pairs in the list with duplicate keys, the specific
// pair found by 'find' may (or may not) be different from the one found by
// 'findR'.
//
///'bdlcc::SkipListPair' Usage Rules
///---------------------------------
// For safe and correct behavior of this component, it is critical that
// 'bdlcc::SkipListPair' pointers be treated similarly to HANDLEs in the
// Windows API: they should be released (using 'releaseReferenceRaw') when they
// are no longer needed, they must not be used after being released, and they
// must be released only once.  To use a 'bdlcc::SkipListPair' pointer that
// refers to a particular pair in multiple places - e.g., in different
// functions or in different threads - use the 'addPairReferenceRaw' method to
// add additional references to the same pair.  Remember that
// 'releaseReferenceRaw' must be called for *each* such pair reference when it
// is no longer needed.
//
///Thread Safety
///-------------
// 'bdlcc::SkipList' is thread-safe and thread-aware; that is, multiple threads
// may use their own Skip List objects or may concurrently use the same object.
// Note that safe usage of the component depends upon correct usage of
// 'bdlcc::SkipListPair' objects (see above).
//
// 'bdlcc::SkipListPairHandle' is only *const* *thread-safe*.  It is not safe
// for multiple threads to invoke non-const methods on the same PairHandle
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
//
//..
// class SimpleScheduler
// {
//    // DATA
//    typedef bdlcc::SkipList<bdlt::Datetime, bsl::function<void()> > List;
//
//    List                     d_list;
//    bslmt::ThreadUtil::Handle d_dispatcher;
//    bslmt::Condition          d_notEmptyCond;
//    bslmt::Mutex              d_condMutex;
//    volatile bool            d_doneFlag;
//
//    // PRIVATE METHODS
//    void dispatcherThread()
//    {
//        while (!d_doneFlag) {
//            List::PairHandle firstItem;
//            if (0 == d_list.front(&firstItem)) {
//                // The list is not empty.
//
//                bsls::TimeInterval when;
//                bdlt::IntervalConversionUtil::convertToTimeInterval(&when,
//                               firstItem.key() -
//                               bdlt::CurrentTime::utc());
//                if (when.totalSecondsAsDouble() <= 0) {
//                    // Execute now and remove from schedule, then iterate.
//
//                    d_list.remove(firstItem);
//                    firstItem.data()();
//                }
//                else {
//                    // Wait until the first scheduled item is due (no
//                    // problem if we wake up early, since we'll just check
//                    // the list and go back to sleep).
//
//                    d_condMutex.lock();
//                    List::PairHandle newFirst;
//                    if (!d_doneFlag && (0 != d_list.front(&newFirst) ||
//                                        newFirst.key() == firstItem.key())) {
//                        d_notEmptyCond.timedWait(&d_condMutex,
//                                                  bdlt::CurrentTime::now() +
//                                                                      when);
//                    }
//                    d_condMutex.unlock();
//                }
//            }
//            else {
//                // The list is empty; wait on the condition variable.
//
//                d_condMutex.lock();
//                if (d_list.isEmpty() && !d_doneFlag) {
//                   d_notEmptyCond.wait(&d_condMutex);
//                }
//                d_condMutex.unlock();
//            }
//
//            // When firstItem goes out of scope here, it releases the
//            // associated resources in the Skip List.
//        }
//    }
//
//  public:
//    // CREATORS
//    SimpleScheduler(bslma::Allocator *basicAllocator = 0)
//    : d_list(basicAllocator)
//    , d_doneFlag(false)
//    {
//        int rc = bslmt::ThreadUtil::create(
//                    &d_dispatcher,
//                    bdlf::BindUtil::bind(&SimpleScheduler::dispatcherThread,
//                                        this));
//        BSLS_ASSERT_SAFE(0 == rc);
//    }
//
//    ~SimpleScheduler()
//    {
//        stop();
//    }
//
//    // MANIPULATORS
//    void stop()
//    {
//        // NOTE: this method will deadlock if invoked from an event callback
//        bslmt::LockGuard<bslmt::Mutex> guard(&d_condMutex);
//        if (bslmt::ThreadUtil::invalidHandle() != d_dispatcher) {
//            bslmt::ThreadUtil::Handle dispatcher = d_dispatcher;
//            d_doneFlag = true;
//            d_notEmptyCond.signal();
//            {
//                bslmt::UnLockGuard<bslmt::Mutex> g(&d_condMutex);
//                bslmt::ThreadUtil::join(dispatcher);
//            }
//            d_dispatcher = bslmt::ThreadUtil::invalidHandle();
//        }
//    }
//
//    void scheduleEvent(const bsl::function<void()>& event,
//                       const bdlt::Datetime& when)
//    {
//        // Use 'addR' since this event will probably be placed near the end
//        // of the list.
//
//        bool newFrontFlag;
//        d_list.addR(when, event, &newFrontFlag);
//        if (newFrontFlag) {
//            // This event is scheduled before all other events.  Wake up
//            // the dispatcher thread.
//
//            d_condMutex.lock();
//            d_notEmptyCond.signal();
//            d_condMutex.unlock();
//        }
//    }
// };
//..
// We can verify the correct behavior of 'SimpleScheduler'.  First, we need a
// wrapper around vector<int>::push_back, since this function is overloaded and
// cannot be bound directly:
//..
// void pushBackWrapper(bsl::vector<int> *vector, int item)
// {
//     vector->push_back(item);
// }
//..
// Now verify that the scheduler executes events when expected:
//..
// SimpleScheduler scheduler;
//
// bsl::vector<int> values;
//
// bdlt::Datetime now = bdlt::CurrentTime::utc();
// bdlt::Datetime scheduleTime = now;
//
// // Add events out of sequence and ensure they are executed
// // in the proper order:
//
// scheduleTime.addMilliseconds(1500);
// scheduler.scheduleEvent(bdlf::BindUtil::bind(
//                         &vector<int>::push_back, &values, 1),
//                         scheduleTime);
//
// scheduleTime = now;
// scheduleTime.addMilliseconds(750);
// scheduler.scheduleEvent(bdlf::BindUtil::bind(
//                         &vector<int>::push_back, &values, 0),
//                         scheduleTime);
//
// scheduleTime = now;
// scheduleTime.addMilliseconds(2250);
// scheduler.scheduleEvent(bdlf::BindUtil::bind(
//                         &vector<int>::push_back, &values, 2),
//                         scheduleTime);
// assert(values.isEmpty());
// scheduleTime.addMilliseconds(250);
// while (bdlt::CurrentTime::utc() < scheduleTime) {
//     bslmt::ThreadUtil::microSleep(10000);
// }
// scheduler.stop();
// assert(3 == values.size());
// assert(0 == values[0]);
// assert(1 == values[1]);
// assert(2 == values[2]);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMT_LOCKGUARD
#include <bslmt_lockguard.h>
#endif

#ifndef INCLUDED_BSLMT_CONDITION
#include <bslmt_condition.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLMT_THREADUTIL
#include <bslmt_threadutil.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BDLB_PRINT
#include <bdlb_print.h>
#endif

#ifndef INCLUDED_BDLB_PRINTMETHODS
#include <bdlb_printmethods.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTFROMTYPE
#include <bsls_alignmentfromtype.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace bdlcc {template <class KEY, class DATA> class SkipList;

template <class KEY, class DATA>
bool operator==(const SkipList<KEY, DATA>& lhs,
                const SkipList<KEY, DATA>& rhs);

template <class KEY, class DATA>
bool operator!=(const SkipList<KEY, DATA>& lhs,
                const SkipList<KEY, DATA>& rhs);

                        // ============================
                        // local class SkipList_Control
                        // ============================

struct SkipList_Control {
    // This component-private structure stores the "control word" and level of
    // a list node.

    // TYPES
    enum {
        k_NUM_REFERENCE_BITS = 20  // minimum; implementation may have more

    };

    // DATA
    bsls::AtomicInt d_cw;   // control word; reference count, release flag, and
                           // acquire count for a node in the list

    unsigned char  d_level;

    // MANIPULATORS
    void init(int level);
        // Set the value of this control word to the initial state for a node
        // at the specified 'level'.

    int incrementRefCount();
        // Add 1 to the reference count portion of this control word.  Return
        // the new reference count.  The behavior is undefined if the reference
        // count is at the implementation-defined maximum.

    int decrementRefCount();
        // Subtract 1 from the reference count portion of this control word.
        // Return the new reference count.  The behavior is undefined if the
        // reference count is 0.

    // ACCESSORS
    int level() const;
        // Return the level stored in this control word.
};

                         // =========================
                         // local class SkipList_Node
                         // =========================

template<class KEY, class DATA>
struct SkipList_Node {
    // This component-private structure is a node in the SkipList.

    // TYPES
    typedef SkipList_Control         Control;
    typedef SkipList_Node<KEY, DATA> Node;

    struct Ptrs {
        Node *d_next_p;
        Node *d_prev_p;
    };

    // DATA
    Control        d_control;    // must be first!

    DATA           d_data;

    KEY            d_key;

    Ptrs           d_ptrs[1];    // Must be last; each node has space for extra
                                 // 'Ptrs' allocated based on its level.

    // MANIPULATORS
    void initControlWord(int level);

    int incrementRefCount();
    int decrementRefCount();

    // ACCESSORS
    int level() const;
};

                 // =========================================
                 // local class SkipList_RandomLevelGenerator
                 // =========================================

class SkipList_RandomLevelGenerator {
    // This component-private class handles randomizing the levelization of
    // list nodes.

    // PRIVATE TYPES
    enum {
        k_MAX_LEVEL = 31,         // Also defined in SkipList and PoolManager

        k_SEED      = 0x12b9b0a1  // arbitrary

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

}  // close package namespace

                    // ====================================
                    // local class bdlcc::SkipList_PoolUtil
                    // ====================================

namespace bdlcc {class SkipList_PoolManager;

struct SkipList_PoolUtil {
    // This component-private utility handles the lock-free pool of list nodes.

    // TYPES
    typedef SkipList_PoolManager PoolManager;

    // CLASS METHODS
    static void *allocate(PoolManager *poolManager, int level);
        // Reserve sufficient space for a node at the specified 'level' from
        // the specified 'poolManager', and return the address of the reserved
        // memory.

    static void deallocate(PoolManager *poolManager, void *address);
        // Return the node at the specified 'address' to the specified
        // 'poolManager'.  The behavior is undefined if 'address' was not
        // allocated from 'poolManager'.

    static PoolManager *createPoolManager(int              *objectSizes,
                                          int               numLevels,
                                          bslma::Allocator *basicAllocator);
        // Create a new pooled node allocator which manages nodes up to the
        // specified 'numLevels' as described by the specified 'objectSizes'.
        // For 'i' in '[0, numLevels)', a node at level 'i' will have size
        // 'objectSizes[i]' bytes.  Use the specified 'basicAllocator' to
        // supply memory.  Return the address of the new allocator.  Note that
        // the behavior is undefined if 'basicAllocator' is 0.

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

  public:
    // CREATORS
    SkipList_NodeCreationHelper(PoolManager      *poolManager,
                                Node             *node,
                                bslma::Allocator *basicAllocator = 0);
        // Create a new scoped guard object to assist in exception-safe
        // initialization of the specified 'node', which was allocated from the
        // specified 'poolManager'.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

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
    // In addition, this class defines 'key' and 'data' member functions which
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
    const KEY& key() const;
        // Return a reference to the non-modifiable "key" value of this pair.

    DATA& data() const;
        // Return a reference to the modifiable "data" of this pair.
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
    Pair                     *d_node_p;

    // FRIENDS
    friend class SkipList<KEY, DATA>;

  private:
    // PRIVATE MANIPULATORS
    SkipListPairHandle(SkipList<KEY, DATA> *list, Pair *reference);
        // Construct a new pair handle for the specified 'list' that manages
        // the specified 'reference'.  Note that it is assumed that the
        // creating (calling) scope already owns the 'reference'.

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
    bool isValid() const;
        // Return 'true' if this PairHandle currently refers to a pair, and
        // 'false' otherwise.

    const KEY& key() const;
        // Return a reference to the non-modifiable "key" value of the pair
        // referred to by this object.  The behavior is undefined unless
        // 'isValid' returns 'true'.

    DATA& data() const;
        // Return a reference to the "data" value of the pair referred to by
        // this object.  The behavior is undefined unless 'isValid' returns
        //  'true'.

    operator const Pair*() const;
        // Return the address of the pair referred to by this
        // 'SkipListPairHandle', or 0 if this handle does not manage a
        // reference.
};

                               // ==============
                               // class SkipList
                               // ==============

template<class KEY, class DATA>
class SkipList {
    // This class provides a generic thread-safe Skip List (an ordered
    // associative container).  It supports an almost complete set of *value*
    // *semantic* operations, including copy construction, assignment, equality
    // comparison, and 'ostream' printing (but not 'bdex' serialization).

  public:
    // CONSTANTS
    enum {
        e_SUCCESS   = 0,
        e_NOT_FOUND = 1,
        e_DUPLICATE = 2,
        e_INVALID   = 3

    };

    // TYPES
    typedef SkipListPair<KEY, DATA>       Pair;
    typedef SkipListPairHandle<KEY, DATA> PairHandle;

  private:
    // PRIVATE CONSTANTS
    enum {
        k_MAX_NUM_LEVELS = 32,       // Also defined in RandomLevelGenerator
                                     // and PoolManager

        k_MAX_LEVEL      = 31
    };

    // PRIVATE TYPES
    typedef SkipList_PoolManager          PoolManager;
    typedef SkipList_PoolUtil             PoolUtil;

    typedef SkipList_Node<KEY, DATA>      Node;
    typedef SkipList_NodeCreationHelper<KEY, DATA>
                                               NodeGuard;

    typedef bslmt::Mutex                        Lock;
    typedef bslmt::LockGuard<bslmt::Mutex>       LockGuard;

    // DATA
    SkipList_RandomLevelGenerator         d_rand;

    bsls::AtomicInt                             d_listLevel;
    Node                                      *d_head_p;
    Node                                      *d_tail_p;

    mutable Lock                               d_lock;

    int                                        d_length;

    PoolManager                               *d_poolManager_p; // owned

    bslma::Allocator                          *d_allocator_p; // held

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
        // operator.

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
        // Add the specified 'node' into the list at the specified 'location'
        // (populated by 'lookupImp' or 'lookupImpR').  Load into the specified
        // 'newFrontFlag' a 'true' value if the node is at the front.  This
        // internal method must be called under the lock.

    void moveImp(bool *newFrontFlag, Node *location[], Node *node);
        // Like 'insert', but the specified 'node' must already be present in
        // the list.  This internal method must be called under the lock.

    Node *popFrontImp();
        // Acquire the lock, remove the front of the list, and release the
        // lock.  Return the node that was at the front of the list, or 0 if
        // the list was empty.

    void releaseNode(Node *node);
        // Decrement the reference count of the specified 'node', and if it
        // reaches 0, destroy 'node' and return it to the pool.  Note that this
        // method neither acquires nor requires the lock.

    int removeAllImp(bsl::vector<Pair *> *removed, bool unlock);
        // Remove all items from this list, and then unlock the mutex if the
        // specified 'unlock' flag is 'true'.  Load into the 'removed' vector
        // pointers which can be used to refer to the removed items.  *Each*
        // such pointer must be released (using 'releaseReferenceRaw') when it
        // is no longer needed.  Note that the pairs in 'removed' will be in
        // ascending order by key value.  Return the number of items that were
        // removed from the list.  This internal method must be called under
        // the lock.

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
        // otherwise.  Return 0 on success, 'e_NOT_FOUND' if the node is no
        // longer in the list, or 'e_DUPLICATE' if the specified
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
        // front of the list, and a 'false' value otherwise.  Return 0 on
        // success, 'e_NOT_FOUND' if the node is no longer in the list, or
        // 'e_DUPLICATE' if the specified 'allowDuplicates' is 'false' and
        // 'newKey' already appears in the list.

    // PRIVATE ACCESSORS
    Node *backNode() const;
        // Return the node at the back of the list, or 0 if the list is empty.
        // Note that this method acquires and releases the lock.

    Node *findNode(const KEY& key) const;
    Node *findNodeR(const KEY& key) const;
        // Return the node with the specified 'key', or 0 if no node could be
        // found.  Note that this method acquires and releases the lock.

    Node *frontNode() const;
        // Return the node at the front of the list, or 0 if the list is empty.
        // Note that this method acquires and releases the lock.

    void lookupImp(Node *update[], const KEY& key) const;
        // Populate the specified 'update' with the first node less than or
        // equal to the specified 'key' at each level in the list.  Note that
        // if 'key' does not appear in the list, either 'd_head_p' (if the list
        // is empty) or a node not equal to 'key' will be loaded into
        // update[0].  This internal method must be called under the lock.

    void lookupImpR(Node *update[], const KEY& key) const;
        // Searching from the back, populate the specified 'update' with the
        // first node less than or equal to the specified 'key' at each level
        // in the list.  Note that if 'key' does not appear in the list, either
        // d_head_p (if the list is empty) or a node not equal to 'key' will be
        // loaded into update[0].  This internal method must be called under
        // the lock.

    Node *nextNode(Node *node) const;
        // Return the node after to the specified 'node', or 0 if 'node' is at
        // the back of the list.  Note that this method acquires and releases
        // the lock.

    Node *prevNode(Node *node) const;
        // Return the node prior to the specified 'node', or 0 if 'node' is at
        // the front of the list.  Note that this method acquires and releases
        // the lock.

    int skipBackward(Node **node) const;
        // If the item identified by the specified 'node' is not at the front
        // of the list, load a reference to the previous item in the list into
        // 'node'; otherwise load 0 into 'node'.  Return 0 on success, and
        // 'e_NOT_FOUND' (with no effect on the value of 'node') if 'node' is
        // no longer in the list.  Note that this method acquires and releases
        // the lock.

    int skipForward(Node **node) const;
        // If the item identified by the specified 'node' is not at the back of
        // the list, load a reference to the next item in the list into 'node';
        // otherwise load 0 into 'node'.  Return 0 on success, and
        // 'e_NOT_FOUND' (with no effect on the value of 'node') if 'node' is
        // no longer in the list.  Note that this method acquires and releases
        // the lock.

    // NOT IMPLEMENTED
    void addPairReferenceRaw(const PairHandle&);
    void releaseReferenceRaw(const PairHandle&);
        // These methods are declared 'private' and not implemented to prevent
        // the accidental casting of a 'SkipListPairHandle' to a
        // 'SkipListPair *'.

    // FRIENDS
    friend class SkipListPair<KEY, DATA>;
    friend class SkipListPairHandle<KEY, DATA>;
    friend bool operator==<> (const SkipList<KEY, DATA>& lhs,
                              const SkipList<KEY, DATA>& rhs);
    friend bool operator!=<> (const SkipList<KEY, DATA>& lhs,
                              const SkipList<KEY, DATA>& rhs);

    // PRIVATE CLASS METHODS
    static const KEY& key(const Pair *reference);
        // Return a non-modifiable reference to the "key" value of the pair
        // identified by the specified 'reference'.

    static DATA& data(const Pair *reference);
        // Return a reference to the modifiable "data" value of the pair
        // identified by the specified 'reference'.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(SkipList,
                                 bslalg::TypeTraitUsesBslmaAllocator);

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
        // into the the optionally specified 'newFrontFlag' a 'true' value if
        // the pair is at the front of the list, and a 'false' value otherwise.

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
        // when it is no longer needed.  Load into the the optionally specified
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
        // into the the optionally specified 'newFrontFlag' a 'true' value if
        // the pair is at the front of the list, and a 'false' value otherwise.
        // Return 0 on success, and a non-zero value (with no effect on the
        // list) if 'key' is already in the list.

    int addUniqueRaw(Pair        **result,
                     const KEY&    key,
                     const DATA&   data,
                     bool         *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list, and load into
        // the specified 'result' a reference to the pair in the list.  The
        // 'result' reference must be released (using 'releaseReferenceRaw')
        // when it is no longer needed.  Load into the the optionally specified
        // 'newFrontFlag' a 'true' value if the pair is at the front of the
        // list, and a 'false' value otherwise.  Return 0 on success, and a
        // non-zero value (with no effect on the list) if 'key' is already in
        // the list.

                         // Insertion Methods (Reverse Search)

    void addR(const KEY& key, const DATA& data, bool *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list.  Search for the
        // correct position for 'key' from the back of the list (in descending
        // order by key value).  Load into the the optionally specified
        // 'newFrontFlag' a 'true' value if the pair is at the front of the
        // list, and a 'false' value otherwise.

    void addR(PairHandle  *result,
              const KEY&   key,
              const DATA&  data,
              bool        *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list, and load into
        // the specified 'result' a reference to the pair in the list.  Search
        // for the correct position for 'key' from the back of the list (in
        // descending order by key value).  Load into the the optionally
        // specified 'newFrontFlag' a 'true' value if the pair is at the front
        // of the list, and a 'false' value otherwise.

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
        // when it is no longer needed.  Load into the the optionally specified
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
        // when it is no longer needed.  Load into the the optionally specified
        // 'newFrontFlag' a 'true' value if the pair is at the front of the
        // list, and a 'false' value otherwise.  The behavior is undefined if
        // 'level' is greater than the implementation-defined maximum level of
        // this class, or if 'level' is negative.  Return 0 on success, and a
        // non-zero value (with no effect on the list) if 'key' is already in
        // the list.  Note that this method is provided for testing purposes.

    void addRawR(Pair        **result,
                 const KEY&    key,
                 const DATA&   data,
                 bool         *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list, and load into
        // the specified 'result' a reference to the pair in the list.  Search
        // for the correct position for 'key' from the back of the list (in
        // descending order by key value).  The 'result' reference must be
        // released (using 'releaseReferenceRaw') when it is no longer needed.
        // Load into the the optionally specified 'newFrontFlag' a 'true' value
        // if the pair is at the front of the list, and a 'false' value
        // otherwise.

    int addUniqueR(const KEY& key, const DATA& data, bool *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list.  Search for the
        // correct position for 'key' from the back of the list (in descending
        // order by key value).  Load into the the optionally specified
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
        // descending order by key value).  Load into the the optionally
        // specified 'newFrontFlag' a 'true' value if the pair is at the front
        // of the list, and a 'false' value otherwise.  Return 0 on success,
        // and a non-zero value (with no effect on the list) if 'key' is
        // already in the list.

    int addUniqueRawR(Pair        **result,
                      const KEY&    key,
                      const DATA&   data,
                      bool         *newFrontFlag = 0);
        // Add the specified 'key' / 'data' pair to this list, and load into
        // the specified 'result' a reference to the pair in the list.  Search
        // for the correct position for 'key' from the back of the list (in
        // descending order by key value).  The 'result' reference must be
        // released (using 'releaseReferenceRaw') when it is no longer needed.
        // Load into the the optionally specified 'newFrontFlag' a 'true' value
        // if the pair is at the front of the list, and a 'false' value
        // otherwise.  Return 0 on success, and a non-zero value (with no
        // effect on the list) if 'key' is already in the list.

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

    int removeAll(bsl::vector<PairHandle> *removed = 0);
        // Remove all items from this list.  Load into the optionally specified
        // 'removed' vector handles which can be used to refer to the removed
        // items.  Note that the items in 'removed' will be in ascending order
        // by key value.  Note also that all references in 'removed' must be
        // released (i.e., destroyed) before this skip list is destroyed.
        // Return the number of items that were removed from this list.

    int removeAllRaw(bsl::vector<Pair *> *removed);
        // Remove all items from this list.  Load into the specified 'removed'
        // vector pointers which can be used to refer to the removed items.
        // *Each* such pointer must be released (using 'releaseReferenceRaw')
        // when it is no longer needed.  Note that the pairs in 'removed' will
        // be in ascending order by key value.  Note also that all references
        // must be released before this skip list is destroyed.  Return the
        // number of items that were removed from this list.

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

    int find(PairHandle *item, const KEY& key) const;
        // Load into the specified 'item' a reference to the element in the
        // list with the specified 'key'.  If there are multiple elements with
        // the 'key', it is not defined which one will be returned.  Return 0
        // on success, and a non-zero value if no such item could be found.

    int findRaw(Pair **item, const KEY& key) const;
        // Load into the specified 'item' a reference to the element in the
        // list with the specified 'key'.  If there are multiple elements with
        // the 'key', it is not defined which one will be returned.  The 'item'
        // reference must be released (using 'releaseReferenceRaw') when it is
        // no longer needed.  Return 0 on success, and a non-zero value if no
        // such item could be found.

    int findR(PairHandle *item, const KEY& key) const;
        // Load into the specified 'item' a reference to the element in the
        // list with the specified 'key' found by searching the list in
        // descending order.  If there are multiple elements with the 'key', it
        // is not defined which one will be returned.  Return 0 on success, and
        // a non-zero value if no such item could be found.

    int findRRaw(Pair **item, const KEY& key) const;
        // Load into the specified 'item' a reference to the element in the
        // list with the specified 'key' found by searching the list in
        // descending order.  If there are multiple elements with the 'key', it
        // is not defined which one will be returned.  The 'item' reference
        // must be released (using 'releaseReferenceRaw') when it is no longer
        // needed.  Return 0 on success, and a non-zero value if no such item
        // could be found.

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

    int skipBackward(PairHandle *item) const;
    int skipBackwardRaw(Pair **item) const;
        // If the item identified by the specified 'item' is not at the front
        // of the list, load a reference to the previous item in the list into
        // 'item'; otherwise reset the value of 'item'.  Return 0 on success,
        // and 'e_NOT_FOUND' (with no effect on the value of 'item') if 'item'
        // is no longer in the list.

    int skipForward(PairHandle *item) const;
    int skipForwardRaw(Pair **item) const;
        // If the item identified by the specified 'item' is not at the end of
        // the list, load a reference to the next item in the list into 'item';
        // otherwise reset the value of 'item'.  Return 0 on success, and
        // 'e_NOT_FOUND' (with no effect on the value of 'item') if 'item' is
        // no longer in the list.
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
    // over those pairs may be different than for another list which was
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

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                             // ------------------
                             // class SkipListPair
                             // ------------------

// ACCESSORS
template <class KEY, class DATA>
inline
const KEY& SkipListPair<KEY, DATA>::key() const
{
    return SkipList<KEY, DATA>::key(this);
}

template <class KEY, class DATA>
inline
DATA& SkipListPair<KEY, DATA>::data() const
{
    return SkipList<KEY, DATA>::data(this);
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
void SkipListPairHandle<KEY, DATA>::release()
{
    if (d_node_p) {
        BSLS_ASSERT_SAFE(0 != d_list_p);

        d_list_p->releaseReferenceRaw(d_node_p);
        d_node_p = 0;
    }
}

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
void SkipListPairHandle<KEY, DATA>::releaseReferenceRaw(
                                               SkipList<KEY, DATA> **list,
                                               Pair                **reference)
{
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

template <class KEY, class DATA>
inline
DATA& SkipListPairHandle<KEY, DATA>::data() const
{
    BSLS_ASSERT_SAFE(isValid());

    return SkipList<KEY, DATA>::data(d_node_p);
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

                            // -------------------
                            // class SkipList_Node
                            // -------------------

template<class KEY, class DATA>
inline
void SkipList_Node<KEY, DATA>::initControlWord(int level)
{
    d_control.init(level);
}

template<class KEY, class DATA>
inline
int SkipList_Node<KEY, DATA>::level() const
{
    return d_control.level();
}

template<class KEY, class DATA>
inline
int SkipList_Node<KEY, DATA>::incrementRefCount()
{
    return d_control.incrementRefCount();
}

template<class KEY, class DATA>
inline
int SkipList_Node<KEY, DATA>::decrementRefCount()
{
    return d_control.decrementRefCount();
}

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
SkipList_NodeCreationHelper<KEY, DATA>::
                                            ~SkipList_NodeCreationHelper()
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
    BSLS_ASSERT_SAFE(d_node_p);

    bslalg::ScalarPrimitives::copyConstruct(&d_node_p->d_key,
                                            key,
                                            d_allocator_p);
    d_keyFlag = true;

    bslalg::ScalarPrimitives::copyConstruct(&d_node_p->d_data,
                                            data,
                                            d_allocator_p);

    d_node_p = 0;
}

                               // --------------
                               // class SkipList
                               // --------------

// PRIVATE MANIPULATORS
template<class KEY, class DATA>
void SkipList<KEY, DATA>::addNode(bool *newFrontFlag, Node *newNode)
{
    LockGuard guard(&d_lock);

    BSLS_ASSERT(0 == newNode->d_ptrs[0].d_next_p);

    Node *update[k_MAX_NUM_LEVELS];
    lookupImp(update, newNode->d_key);

    insertImp(newFrontFlag, update, newNode);
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

    BSLS_ASSERT(0 == newNode->d_ptrs[0].d_next_p);

    Node *update[k_MAX_NUM_LEVELS];
    lookupImpR(update, newNode->d_key);

    insertImp(newFrontFlag, update, newNode);
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

    BSLS_ASSERT(0 == newNode->d_ptrs[0].d_next_p);

    Node *update[k_MAX_NUM_LEVELS];
    lookupImp(update, newNode->d_key);

    Node *q = update[0]->d_ptrs[0].d_next_p;
    if (q != d_tail_p && q->d_key == newNode->d_key) {
        return e_DUPLICATE;                                           // RETURN
    }

    insertImp(newFrontFlag, update, newNode);

    return 0;
}

template<class KEY, class DATA>
int SkipList<KEY, DATA>::addNodeUniqueR(bool *newFrontFlag, Node *newNode)
{
    LockGuard guard(&d_lock);

    BSLS_ASSERT(0 == newNode->d_ptrs[0].d_next_p);

    Node *update[k_MAX_NUM_LEVELS];
    lookupImpR(update, newNode->d_key);

    Node *q = update[0]->d_ptrs[0].d_next_p;
    if (q != d_tail_p && q->d_key == newNode->d_key) {
        return e_DUPLICATE;                                           // RETURN
    }

    insertImp(newFrontFlag, update, newNode);

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

    node->incrementRefCount();
    node->d_ptrs[0].d_next_p = 0;

    return node;
}

template<class KEY, class DATA>
void SkipList<KEY, DATA>::initialize()
{
    // Assert that this method has not been invoked.
    BSLS_ASSERT(0 == d_poolManager_p);

    int nodeSizes[k_MAX_NUM_LEVELS];

    // We can't use address 0, because it generates a warning with gcc.

    const int offsetofPtrs = static_cast<int>(
                                   (char *)&(((Node *)1)->d_ptrs) - (char *)1);
    for (int i = 0; i < k_MAX_NUM_LEVELS; ++i) {
        int nodeSize = static_cast<int>(
                           offsetofPtrs + (i + 1)*sizeof(typename Node::Ptrs));
        nodeSize = (nodeSize + bsls::AlignmentFromType<Node>::VALUE - 1) &
                                   ~(bsls::AlignmentFromType<Node>::VALUE - 1);
        nodeSizes[i] = nodeSize;
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
    int level = node->level();
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
        Node *p = location[k];
        Node *q = p->d_ptrs[k].d_next_p;

        node->d_ptrs[k].d_prev_p = p;
        node->d_ptrs[k].d_next_p = q;

        p->d_ptrs[k].d_next_p = node;
        q->d_ptrs[k].d_prev_p = node;
    }

    if (newFrontFlag) {
        *newFrontFlag = (location[0] == d_head_p);
    }

    ++d_length;
}

template<class KEY, class DATA>
void SkipList<KEY, DATA>::moveImp(bool *newFrontFlag,
                                  Node *location[],
                                  Node *node)
{
    int level = node->level();
    BSLS_ASSERT(level <= d_listLevel);

    for (int k = 0; k <= level; ++k) {
        Node *newP = location[k];
        Node *newQ = newP->d_ptrs[k].d_next_p;

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
        *newFrontFlag = (location[0] == d_head_p);
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

    int level = node->level();

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
    int refCnt = node->decrementRefCount();

    if (!refCnt) {
        node->d_key.~KEY();
        node->d_data.~DATA();
        PoolUtil::deallocate(d_poolManager_p, node);
    }
}

template<class KEY, class DATA>
int SkipList<KEY, DATA>::removeAllImp(bsl::vector<Pair *> *removed,
                                      bool                 unlock)
{
    Node *p = d_head_p;
    Node *q = p->d_ptrs[0].d_next_p;

    int numRemoved = 0;
    while (q != d_tail_p) {
        p = q;
        q = p->d_ptrs[0].d_next_p;

        p->d_ptrs[0].d_next_p = 0;
        numRemoved++;
    }
    d_length -= numRemoved;

    for (int i = 0; i <= d_listLevel; ++i) {
        d_head_p->d_ptrs[i].d_next_p = d_tail_p;
        d_tail_p->d_ptrs[i].d_prev_p = d_head_p;
    }

    if (unlock) {
        d_lock.unlock();
    }

    if (removed) {
        removed->resize(numRemoved);
        int i = numRemoved - 1;
        while (p != d_head_p) {
            q = p;
            p = q->d_ptrs[0].d_prev_p;
            (*removed)[i--] = reinterpret_cast<Pair *>(q);
        }
    }
    else {
        while (p != d_head_p) {
            q = p;
            p = q->d_ptrs[0].d_prev_p;

            releaseNode(q);
        }
    }
    return numRemoved;
}

template<class KEY, class DATA>
int SkipList<KEY, DATA>::removeNode(Node *node)
{
    LockGuard guard(&d_lock);

    if (0 == node->d_ptrs[0].d_next_p) {
        return e_NOT_FOUND;                                           // RETURN
    }

    int level = node->level();

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
    LockGuard guard(&d_lock);

    if (0 == node->d_ptrs[0].d_next_p) {
        return e_NOT_FOUND;                                           // RETURN
    }

    Node *update[k_MAX_NUM_LEVELS];
    lookupImp(update, newKey);

    if (!allowDuplicates) {
        Node *q = update[0]->d_ptrs[0].d_next_p;
        if (q != d_tail_p && q != node && q->d_key == newKey) {
            return e_DUPLICATE;                                       // RETURN
        }
    }

    node->d_key = newKey;  // may throw

    // now we are committed: change the list!
    moveImp(newFrontFlag, update, node);

    return 0;
}

template<class KEY, class DATA>
int SkipList<KEY, DATA>::updateNodeR(bool       *newFrontFlag,
                                     Node       *node,
                                     const KEY&  newKey,
                                     bool        allowDuplicates)
{
    LockGuard guard(&d_lock);

    if (0 == node->d_ptrs[0].d_next_p) {
        return e_NOT_FOUND;                                           // RETURN
    }

    Node *update[k_MAX_NUM_LEVELS];
    lookupImpR(update, newKey);

    if (!allowDuplicates) {
        Node *p = update[0];
        if (p != d_head_p && p != node && p->d_key == newKey) {
            return e_DUPLICATE;                                       // RETURN
        }
    }

    node->d_key = newKey;  // may throw

    // now we are committed: change the list!
    moveImp(newFrontFlag, update, node);

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

    node->incrementRefCount();
    return node;
}

template<class KEY, class DATA>
SkipList_Node<KEY, DATA> *SkipList<KEY, DATA>::findNode(const KEY& key) const
{
    Node *locator[k_MAX_NUM_LEVELS];

    LockGuard guard(&d_lock);
    lookupImp(locator, key);

    Node *q = locator[0]->d_ptrs[0].d_next_p;
    if (q != d_tail_p && q->d_key == key) {
        q->incrementRefCount();
        return q;                                                     // RETURN
    }

    return 0;
}

template<class KEY, class DATA>
SkipList_Node<KEY, DATA> *SkipList<KEY, DATA>::findNodeR(const KEY& key) const
{
    Node *locator[k_MAX_NUM_LEVELS];

    LockGuard guard(&d_lock);
    lookupImpR(locator, key);

    Node *p = locator[0];
    if (p != d_head_p && p->d_key == key) {
        p->incrementRefCount();
        return p;                                                     // RETURN
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

    node->incrementRefCount();
    return node;
}

template<class KEY, class DATA>
void SkipList<KEY, DATA>::lookupImp(Node *update[], const KEY& key) const
{
    Node *p = d_head_p;
    for (int k = d_listLevel; k >= 0; --k) {
        Node *q = p->d_ptrs[k].d_next_p;
        while (q != d_tail_p && q->d_key < key) {
            p = q;
            q = p->d_ptrs[k].d_next_p;
        }
        update[k] = p;
    }
}

template<class KEY, class DATA>
void SkipList<KEY, DATA>::lookupImpR(Node *update[], const KEY& key) const
{
    Node *q = d_tail_p;
    for (int k = d_listLevel; k >= 0; --k) {
        Node *p = q->d_ptrs[k].d_prev_p;
        while (p != d_head_p && p->d_key > key) {
            q = p;
            p = q->d_ptrs[k].d_prev_p;
        }
        update[k] = p;
    }
}

template<class KEY, class DATA>
SkipList_Node<KEY, DATA> *
SkipList<KEY, DATA>::nextNode(Node *node) const
{
    BSLS_ASSERT(node != d_head_p && node != d_tail_p);

    LockGuard guard(&d_lock);

    Node *next = node->d_ptrs[0].d_next_p;
    if (0 == next || d_tail_p == next) {
        return 0;                                                     // RETURN
    }

    next->incrementRefCount();
    return next;
}

template<class KEY, class DATA>
SkipList_Node<KEY, DATA> *
SkipList<KEY, DATA>::prevNode(Node *node) const
{
    BSLS_ASSERT(node != d_head_p && node != d_tail_p);

    LockGuard guard(&d_lock);
    if (0 == node->d_ptrs[0].d_next_p) {
        return 0;                                                     // RETURN
    }

    Node *prev = node->d_ptrs[0].d_prev_p;
    if (d_head_p == prev) {
        return 0;                                                     // RETURN
    }

    prev->incrementRefCount();
    return prev;
}

template<class KEY, class DATA>
int SkipList<KEY, DATA>::skipBackward(Node **node) const
{
    Node *current = *node;
    BSLS_ASSERT(current);
    BSLS_ASSERT(current != d_head_p && current != d_tail_p);

    LockGuard guard(&d_lock);

    if (0 == current->d_ptrs[0].d_next_p) {
        // We set this pointer to 0 only when removing from the list.
        return e_NOT_FOUND;                                           // RETURN
    }

    const int count = current->decrementRefCount();
    BSLS_ASSERT(count);
    (void) count;    // suppress 'unused variable' warnings

    Node *prev = current->d_ptrs[0].d_prev_p;
    if (d_head_p == prev) {
        *node = 0;
        return 0;                                                     // RETURN
    }

    prev->incrementRefCount();
    *node = prev;
    return 0;
}

template<class KEY, class DATA>
int SkipList<KEY, DATA>::skipForward(Node **node) const
{
    Node *current = *node;
    BSLS_ASSERT(current);
    BSLS_ASSERT(current != d_head_p && current != d_tail_p);

    LockGuard guard(&d_lock);

    if (0 == current->d_ptrs[0].d_next_p) {
        // We set this pointer to 0 only when removing from the list.
        return e_NOT_FOUND;                                           // RETURN
    }

    const int count = current->decrementRefCount();
    BSLS_ASSERT(count);
    (void) count;    // suppress 'unused variable' warnings

    Node *next = current->d_ptrs[0].d_next_p;
    if (d_tail_p == next) {
        *node = 0;
        return 0;                                                     // RETURN
    }

    next->incrementRefCount();
    *node = next;
    return 0;
}

// PRIVATE CLASS METHODS
template<class KEY, class DATA>
inline
const KEY& SkipList<KEY, DATA>::key(const Pair *reference)
{
    const Node *node = (const Node *)(const void *)(reference);
    return node->d_key;
}

template<class KEY, class DATA>
inline
DATA& SkipList<KEY, DATA>::data(const Pair *reference)
{
    Node *node = (Node *)(void *)const_cast<Pair *>(reference);
    return node->d_data;
}

// CLASS METHODS
template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::level(const Pair *reference)
{
    Node *node = (Node *)(void *)const_cast<Pair *>(reference);
    return node->level();
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
    Node *p = d_head_p->d_ptrs[0].d_next_p;
    while (p != d_tail_p) {
        const int count = p->decrementRefCount();
        BSLS_ASSERT(0 == count);
        (void) count;    // suppress 'unused variable' warnings

        p->d_key.~KEY();
        p->d_data.~DATA();
        p = p->d_ptrs[0].d_next_p;
    }

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

    // first empty this list
    LockGuard guard(&d_lock);
    removeAllImp(0, false);

    // Now lock the other list and get handles to all its elements.  Once we
    // have locked it, we need to do all operations manually because the
    // important functions of 'rhs' (like frontNode and nextNode) will lock the
    // mutex.

    LockGuard rhsGuard(&rhs.d_lock);

    bsl::vector<PairHandle> rhsElements;
    for (Node *node = rhs.d_head_p->d_ptrs[0].d_next_p;
         node && node != rhs.d_tail_p;
         node = node->d_ptrs[0].d_next_p)
    {
        node->incrementRefCount();
        rhsElements.insert(rhsElements.end(),
                           PairHandle())->reset(
                                               &rhs,
                                               reinterpret_cast<Pair *>(node));
    }

    // Now we can unlock the other list, since our handles will remain valid
    // even if the referenced elements are removed.

    rhsGuard.release()->unlock();

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
void SkipList<KEY, DATA>::releaseReferenceRaw(const Pair *reference)
{
    Node *node = (Node *)(void *)const_cast<Pair *>(reference);
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
    addRaw((Pair **)0, key, data, newFrontFlag);
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
        node->incrementRefCount();
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
        node->incrementRefCount();
        *result = reinterpret_cast<Pair *>(node);
    }

    int ret = addNodeUnique(newFrontFlag, node);
    if (ret) {
        if (result) {
            node->decrementRefCount();
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
    int rc = addUniqueRaw(&handle, key, data, newFrontFlag);
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
    return addUniqueRaw((Pair **)0, key, data, newFrontFlag);
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
    addRawR((Pair **)0, key, data, newFrontFlag);
}

template<class KEY, class DATA>
int SkipList<KEY, DATA>::addUniqueR(PairHandle  *result,
                                    const KEY&   key,
                                    const DATA&  data,
                                    bool        *newFrontFlag)
{
    Pair *handle;
    int rc = addUniqueRawR(&handle, key, data, newFrontFlag);
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
    return addUniqueRawR((Pair **)0, key, data, newFrontFlag);
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
inline
int SkipList<KEY, DATA>::addUniqueRawR(Pair        **result,
                                       const KEY&    key,
                                       const DATA&   data,
                                       bool         *newFrontFlag)
{
    return addAtLevelUniqueRawR(result, d_rand.randomLevel(), key, data,
                                newFrontFlag);
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
        node->incrementRefCount();
        *result = reinterpret_cast<Pair *>(node);
    }

    int ret = addNodeUniqueR(newFrontFlag, node);
    if (ret) {
        if (result) {
            node->decrementRefCount();
            *result = 0;
        }
        releaseNode(node);
        return ret;                                                   // RETURN
    }

    return 0;
}

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
        node->incrementRefCount();
        *result = reinterpret_cast<Pair *>(node);
    }

    addNodeR(newFrontFlag, node);
}

                         // Removal Methods

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::remove(const Pair *reference)
{
    if (0 == reference) {
        return e_INVALID;                                             // RETURN
    }

    Node *node = (Node *)(void *)const_cast<Pair *>(reference);

    int ret = removeNode(node);
    if (ret) {
        return ret;                                                   // RETURN
    }

    releaseNode(node);
    return 0;
}

template<class KEY, class DATA>
int SkipList<KEY, DATA>::removeAll(bsl::vector<PairHandle> *removed)
{
    bsl::vector<Pair *> removedRaw;

    int rc = removeAllRaw(removed ? &removedRaw : 0);
    if (0 == removed) {
        return rc;                                                    // RETURN
    }
    else {
        for (typename bsl::vector<Pair *>::iterator it = removedRaw.begin();
             it != removedRaw.end();
             ++it)
        {
            PairHandle item(this, *it);
            removed->push_back(item);
        }
    }
    return rc;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::removeAllRaw(bsl::vector<Pair *> *removed)
{
    d_lock.lock();

    return removeAllImp(removed, true); // true = unlock after removal
}

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

    Node *node = (Node *)(void *)const_cast<Pair *>(reference);
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

    Node *node = (Node *)(void *)const_cast<Pair *>(reference);
    return updateNodeR(newFrontFlag, node, newKey, allowDuplicates);
}

// ACCESSORS
template<class KEY, class DATA>
inline
SkipListPair<KEY, DATA> *
SkipList<KEY, DATA>::addPairReferenceRaw(const Pair *reference) const
{
    Node *node = (Node *)(void *)const_cast<Pair *>(reference);
    node->incrementRefCount();
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
    lookupImp(locator, key);

    Node *q = locator[0]->d_ptrs[0].d_next_p;
    if (q != d_tail_p && q->d_key == key) {
        return true;                                                  // RETURN
    }

    return false;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::findRaw(Pair **item, const KEY& key) const
{
    *item = reinterpret_cast<Pair *>(findNode(key));
    return *item ? 0 : -1;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::findRRaw(Pair **item, const KEY& key) const
{
    *item = reinterpret_cast<Pair *>(findNodeR(key));
    return *item ? 0 : -1;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::find(PairHandle *item, const KEY& key) const
{
    Pair *itemPtr = reinterpret_cast<Pair *>(findNode(key));
    if (itemPtr) {
        item->reset(this, itemPtr);
        return 0;                                                     // RETURN
    }
    return -1;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::findR(PairHandle *item, const KEY& key) const
{
    Pair *itemPtr = reinterpret_cast<Pair *>(findNodeR(key));
    if (itemPtr) {
        item->reset(this, itemPtr);
        return 0;                                                     // RETURN
    }
    return -1;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::frontRaw(Pair **front) const
{
    *front = reinterpret_cast<Pair *>(frontNode());
    return *front ? 0 : -1;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::front(PairHandle *front) const
{
    Pair *frontPtr = reinterpret_cast<Pair *>(frontNode());
    if (frontPtr) {
        front->reset(this, frontPtr);
        return 0;                                                     // RETURN
    }
    return -1;
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
inline
int SkipList<KEY, DATA>::nextRaw(Pair **next, const Pair *reference) const
{
    Node *node = (Node *)(void *)const_cast<Pair *>(reference);
    *next = reinterpret_cast<Pair *>(nextNode(node));

    return *next ? 0 : -1;
}

template<class KEY, class DATA>
inline
int
SkipList<KEY, DATA>::next(PairHandle *next, const Pair *reference) const
{
    if (0 == reference) {
        return e_INVALID;                                             // RETURN
    }

    Node *node  = (Node *)(void *)const_cast<Pair *>(reference);
    Node *nNode = nextNode(node);
    if (nNode) {
        next->reset(this, reinterpret_cast<Pair *>(nNode));
        return 0;                                                     // RETURN
    }
    return -1;
}

template<class KEY, class DATA>
inline
int
SkipList<KEY, DATA>::previousRaw(Pair **prevPair, const Pair *reference) const
{
    Node *node = (Node *)(void *)const_cast<Pair *>(reference);
    *prevPair = prevNode(node);
    return *prevPair ? 0 : -1;
}

template<class KEY, class DATA>
inline
int
SkipList<KEY, DATA>::previous(PairHandle *prevPair,
                              const Pair *reference) const
{
    if (0 == reference) {
        return e_INVALID;                                             // RETURN
    }

    Node *node  = (Node *)(void *)const_cast<Pair *>(reference);
    Node *pNode = prevNode(node);
    if (pNode) {
        prevPair->reset(this, reinterpret_cast<Pair *>(pNode));
        return 0;                                                     // RETURN
    }
    return -1;
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
            stream << "level = " << node->level() << "\n";

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
            stream << "[ (level = " << node->level() << ") ";

            bdlb::PrintMethods::print(stream, node->d_key, 0, -1);
            stream << " => ";
            bdlb::PrintMethods::print(stream, node->d_data, 0, -1);

            stream << " ]";

        }

        stream << "]";
    }

    return stream << bsl::flush;
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::skipBackward(PairHandle *item) const
{
    BSLS_ASSERT_SAFE(item->isValid());

    Node **node_p = reinterpret_cast<Node **>(&item->d_node_p);
    return skipBackward(node_p);
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::skipForward(PairHandle *item) const
{
    BSLS_ASSERT_SAFE(item->isValid());

    Node **node_p = reinterpret_cast<Node **>(&item->d_node_p);
    return skipForward(node_p);
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::skipForwardRaw(Pair **item) const
{
    BSLS_ASSERT_SAFE(item);

    Node **node_p = reinterpret_cast<Node **>(item);
    return skipForward(node_p);
}

template<class KEY, class DATA>
inline
int SkipList<KEY, DATA>::skipBackwardRaw(Pair **item) const
{
    BSLS_ASSERT_SAFE(item);

    Node **node_p = reinterpret_cast<Node **>(item);
    return skipBackward(node_p);
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
    bslmt::LockGuard<bslmt::Mutex> lhsGuard(&lhs.d_lock);
    bslmt::LockGuard<bslmt::Mutex> rhsGuard(&rhs.d_lock);

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
    bslmt::LockGuard<bslmt::Mutex> lhsGuard(&lhs.d_lock);
    bslmt::LockGuard<bslmt::Mutex> rhsGuard(&rhs.d_lock);

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

        if (lhsNode->d_key  != rhsNode->d_key
         || lhsNode->d_data != rhsNode->d_data) {
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
