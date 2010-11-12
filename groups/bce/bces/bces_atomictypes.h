// bces_atomictypes.h              -*-C++-*-
#ifndef INCLUDED_BCES_ATOMICTYPES
#define INCLUDED_BCES_ATOMICTYPES

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide wrappers for atomic operations.
//
//@CLASSES:
//       bces_AtomicInt: atomic 32-bit integer wrapper
//     bces_AtomicInt64: atomic 64-bit integer wrapper
//   bces_AtomicPointer: parameterized atomic pointer wrapper
//        bces_SpinLock: spin lock wrapper
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@SEE_ALSO: bces_atomicutil
//
//@DESCRIPTION: This component provides convenient wrappers for the types and
// operations provided by the 'bces_atomicutil' component.  Classes
// 'bces_AtomicInt' and 'bces_AtomicInt64' wrap the corresponding atomic
// integers, and provide overloaded operators and functions for common
// arithmetic operations.  The class 'bces_AtomicPointer' provides atomic
// operations to manipulate and dereference a pointer.  The class
// 'bces_SpinLock' provides an object wrapper for a spin lock.
//
// See the 'bces_atomicutil' component for details on "atomic operations",
// including details on "ordering constraints" referenced in the documentation
// of "relaxed" functions in this component.
//
///Usage
///------
// The following examples demonstrate various uses of atomic types.  Compare
// these examples to the corresponding examples in 'bces_atomicutil'.
//
///Example 1: Usage Statistics on a Thread Pool
///- - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates the common use of atomic integer types
// for statistics counters.  The program creates a series of threads to process
// transactions.  As each thread completes a transaction, it atomically
// increments the transaction counters.
//
// For this example we assume the existence of the functions
// 'processNextTransaction', 'createWorkerThread' and 'waitAllThreads'.  The
// function 'createWorkerThread' spawns a new thread that executes the
// 'workerThread' function.  'waitAllThreads' blocks until all the worker
// thread complete.
//
// First, we declare the shared counters.  In contrast to the raw types defined
// in 'bces_atomicutil', these atomic types are zero-initialized at
// construction.
//..
// static bces_AtomicInt64 transactionCount;
// static bces_AtomicInt64 successCount;
// static bces_AtomicInt64 failureCount;
//..
// For each transaction processed, we atomically increment the success/failure
// counters and the total transaction count.
//..
// static void workerThread(int *stop)
// {
//     while (!(*stop)) {
//         if (processNextTransaction()) {
//             ++failureCount;
//         } else {
//             ++successCount;
//         }
//         ++transactionCount;
//     }
// }
//..
// The function 'serverMain' provides the overall control logic for the server;
// it spawns the threads and then waits for all work to be completed.  When all
// threads have finished, the function returns normally.
//..
// void serverMain()
// {
//     const int num_threads = 10;
//     for (int i = 0; i < num_threads; ++i) {
//         createWorkerThread();
//     }
//     waitAllThreads();
// }
//..
//
///Example 2: Thread-safe Counted Handle
///- - - - - - - - - - - - - - - - - - -
// The next example demonstrates the use of atomic integer operations to
// implement a thread safe counted handle similar to 'bcema_SharedPtr'.
// Each counted handle (of type 'my_CountedHandle') maintains a pointer to a
// representation object ('my_CountedHandleRep'); this representation object in
// turn stores a pointer to the managed object and a reference counter.
//
// The representation object can be shared by several counted handle objects.
// When a counted handle object is assigned to a second counted handle object,
// the pointer to the representation is copied to the second counted handle and
// the reference count on the representation is atomically incremented.  When a
// counted handle releases its reference to the representation, it atomically
// decrements the reference count.  If the resulting reference count becomes 0
// (and there are no more references to the object), the handle deletes the
// representation object; the representation object in turn deletes the managed
// object ('INSTANCE').
//
// Both the handle class and the representation class are template classes with
// two template parameters.  The first template parameter, 'INSTANCE', gives
// the type of the "instance", or managed object.  The second template
// parameter provides a "factory" class.  This factory class must support the
// 'deallocate' method that the representation class will use to delete the
// managed object.
//
// Note that the example does not inherently require the use of a "factory"
// class; the factory mechanism allows us to separate out the management of the
// 'INSTANCE' life-cycle from the implementation of the representation class.
// This is useful in supporting situations where deletion of 'INSTANCE' objects
// is non-standard, or where objects are not deleted at all but are instead
// returned to a pool.  See, for instance, 'bcema_deleter' and
// 'bcema_factorydeleter' for further explanation.
//
///Class 'my_CountedHandleRep'
///-  -  -  -  -  -  -  -  -
// We begin by defining class 'my_CountedHandleRep'.  This class manages a
// single 'INSTANCE' object on behalf of multiple "handle" objects; since
// different "handle" objects may be active in different threads, class
// 'my_CountedHandleRep' must be thread-safe and thread-aware.  Specifically,
// methods 'increment' and 'decrement' must work atomically.
//
// The class declaration for 'my_CountedHandleRep' is identical to the same
// class in component 'bces_atomicutil', with a single exception: member
// 'd_count' is of type 'bces_AtomicInt', rather than 'bces_AtomicUtil::Int'.
// Whereas 'bces_AtomicUtil::Int' is merely a 'typedef' for a platform-specific
// data type to be used in atomic integer operations, 'bces_AtomicInt'
// encapsulates those atomic operations as member functions and operator
// overloads.  Class 'my_CountedHandleRep' will benefit from this
// encapsulation: its method implementations will be able to operate
// on 'd_count' as if it were a standard integer.
//
// Note that, as in the example in 'bces_atomicutil, this class is to be used
// only by class 'my_CountedHandle', and thus all methods of class
// 'my_CountedHandleRep' are declared private and 'friend' status is granted to
// class 'my_CountedHandle'.
//..
//                         // =========================
//                         // class my_CountedHandleRep
//                         // =========================
//
// template <class INSTANCE, class FACTORY>
// class my_CountedHandle;
//
//
// template <class INSTANCE, class FACTORY>
// class my_CountedHandleRep {
//
//     // DATA
//     bces_AtomicInt   d_count;        // number of active references
//     INSTANCE        *d_instance_p;   // address of managed instance
//     FACTORY         *d_factory_p;    // held but not owned
//     bslma_Allocator *d_allocator_p;  // held but not owned
//
//     // FRIENDS
//     friend class my_CountedHandle<INSTANCE, FACTORY>;
//
//     // NOT IMPLEMENTED
//     my_CountedHandleRep(const my_CountedHandleRep&);
//     my_CountedHandleRep& operator=(const my_CountedHandleRep&);
//
//   private:
//     // PRIVATE CLASS METHODS
//     static void
//     deleteObject(my_CountedHandleRep<INSTANCE, FACTORY> *object);
//
//     // PRIVATE CREATORS
//     my_CountedHandleRep(INSTANCE        *instance,
//                         FACTORY         *factory,
//                         bslma_Allocator *basicAllocator);
//     ~my_CountedHandleRep();
//
//     // PRIVATE MANIPULATORS
//     void increment();
//     int decrement();
// };
//..
///Class 'my_CountedHandle'
///-  -  -  -  -  -  -  -
// Class 'my_CountedHandle' provides an individual handle to the shared,
// reference-counted object.  Each 'my_CountedHandle' object acts as a smart
// pointer, supplying an overloaded 'operator->' that provides access to the
// underlying 'INSTANCE' object via pointer semantics.
//
// 'my_CountedHandle' can also be copied freely; the copy constructor will use
// the 'increment' method from 'my_CountedHandleRep' to note the extra copy.
// Similarly, the destructor will call 'my_CountedHandleRep::decrement' to note
// that the underlying 'INSTANCE' has one fewer handle, and delete the "rep"
// object when its reference count is reduced to zero.
//
// Similar to 'my_CountedHandleRep', the class declaration for
// 'my_CountedHandle' is identical to that in 'bces_atomicutil'.
//..
//                         // ======================
//                         // class my_CountedHandle
//                         // ======================
//
// template <class INSTANCE, class FACTORY>
// class my_CountedHandle {
//
//     // DATA
//     my_CountedHandleRep<INSTANCE, FACTORY> *d_rep_p;  // shared rep.
//
//   public:
//     // CREATORS
//     my_CountedHandle();
//     my_CountedHandle(INSTANCE        *instance,
//                      FACTORY         *factory,
//                      bslma_Allocator *basicAllocator = 0);
//
//     my_CountedHandle(const my_CountedHandle<INSTANCE, FACTORY>& other);
//
//     ~my_CountedHandle();
//
//     // ACCESSORS
//     INSTANCE *operator->() const;
//     int numReferences() const;
// };
//..
///Function Definitions for 'my_CountedHandleRep'
///-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// 'static' method 'deleteObject' is called by the destructor for
// class 'my_CountedHandle' for the last instance of 'my_CountedHandle' using
// the given "rep" object.  The implementation shows idiomatic object
// destruction using a 'bde_Allocator'.
//..
// template <class INSTANCE, class FACTORY>
// inline
// void my_CountedHandleRep<INSTANCE, FACTORY>::deleteObject(
//                              my_CountedHandleRep<INSTANCE, FACTORY> *object)
// {
//     object->~my_CountedHandleRep();
//     object->d_allocator_p->deallocate(object);
// }
//..
// The following constructor is called by 'my_CountedHandle' for a
// newly-constructed 'INSTANCE' variable.  Member 'd_count' is initialized to
// 1, reflecting the fact that this constructor will be called by a new
// instance of 'my_CountedHandle'; that instance is our first and only handle
// when this constructor is called.  Note that 'd_count' is initialized as if
// it were a simple integer; its object constructor guarantees that the
// initialization is done atomically.
//..
// template <class INSTANCE, class FACTORY>
// inline
// my_CountedHandleRep<INSTANCE, FACTORY>::
//                         my_CountedHandleRep(INSTANCE        *instance,
//                                             FACTORY         *factory,
//                                             bslma_Allocator *basicAllocator)
// : d_instance_p(instance)
// , d_factory_p(factory)
// , d_allocator_p(basicAllocator)
// , d_count(1)
// {
// }
//
// template <class INSTANCE, class FACTORY>
// inline
// my_CountedHandleRep<INSTANCE, FACTORY>::~my_CountedHandleRep()
// {
//     d_factory_p->deallocate(d_instance_p);
// }
//
// // MANIPULATORS
//..
// Method 'increment' is called by 'my_CountedHandle' to add a new
// reference to the current "rep" object.  It simply increments 'd_count',
// using the prefix 'operator++'.  This operation must be done atomically in a
// multi-threaded context; class 'bces_AtomicInt' provides this guarantee for
// all its overloaded operators, and 'my_CountedHandleRep' relies upon this
// guarantee.
//..
// template <class INSTANCE, class FACTORY>
// inline
// void my_CountedHandleRep<INSTANCE, FACTORY>::increment()
// {
//     ++d_count;
// }
//..
// Method 'decrement' is called by 'my_CountedHandle' when a reference
// to the current "rep" object is being deleted.  It atomically decrements the
// number of references to this 'my_CountedHandleRep'; and once again,
// atomicity is guaranteed by the underlying type of 'd_count'.
//..
// template <class INSTANCE, class FACTORY>
// inline
// int my_CountedHandleRep<INSTANCE, FACTORY>::decrement()
// {
//     return --d_count;
// }
//..
///Function Definitions for 'my_CountedHandle'
///-  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// The first constructor for 'my_CountedHandle' is used when creating a handle
// for a new 'INSTANCE'.  The 'INSTANCE' is constructed separately, and a
// pointer to that object is passed as the first argument ('object').
//
// The second argument is a pointer to the 'FACTORY'; as noted above, this
// 'FACTORY' object is used to manage the destruction of the 'INSTANCE'.
//
// Finally, the constructor also shows idiomatic use of the 'bdema_default'
// component to set the allocator for the 'my_CountedHandleRep' object.
//..
//                         // ----------------------
//                         // class my_CountedHandle
//                         // ----------------------
//
// // CREATORS
// template <class INSTANCE, class FACTORY>
// inline
// my_CountedHandle<INSTANCE, FACTORY>::my_CountedHandle(
//                                             INSTANCE        *object,
//                                             FACTORY         *factory,
//                                             bslma_Allocator *basicAllocator)
// {
//     bslma_Allocator *ba = bslma_Default::allocator(basicAllocator);
//     d_rep_p = new(ba->allocate(sizeof *d_rep_p))
//                 my_CountedHandleRep<INSTANCE, FACTORY>(object, factory, ba);
// }
//..
// Next is the copy constructor.  The new object copies the underlying
// 'my_CountedHandleRep', and then increments its counter.
//..
// template <class INSTANCE, class FACTORY>
// inline
// my_CountedHandle<INSTANCE, FACTORY>::my_CountedHandle(
//                            const my_CountedHandle<INSTANCE, FACTORY>& other)
// : d_rep_p(other.d_rep_p)
// {
//     if (d_rep_p) {
//         d_rep_p->increment();
//     }
// }
//..
// The destructor decrements the "rep" object's reference count using the
// 'decrement' method.  The 'decrement' method returns the object's reference
// count after the decrement is completed, and 'my_CountedHandle' uses this
// value to determine whether the "rep" object should be deleted.
//..
// template <class INSTANCE, class FACTORY>
// inline
// my_CountedHandle<INSTANCE, FACTORY>::~my_CountedHandle()
// {
//     if (d_rep_p && 0 == d_rep_p->decrement()) {
//         my_CountedHandleRep<INSTANCE, FACTORY>::deleteObject(d_rep_p);
//     }
// }
//..
// Member 'operator->()' provides basic pointer semantics.  Note that,
// while class 'my_CountedHandleRep' is itself fully thread-aware, it does not
// guaranty thread safety for the 'INSTANCE' object.  In order to provide
// thread safety for the 'INSTANCE' in the general case, the "rep" would need
// to use a more general concurrency mechanism such as a mutex.  For more
// information, see class 'bcemt_Mutex' in component 'bcemt_thread'.
//..
// // ACCESSORS
// template <class INSTANCE, class FACTORY>
// inline
// INSTANCE *my_CountedHandle<INSTANCE, FACTORY>::operator->() const
// {
//     return d_rep_p->d_instance_p;
// }
//
// template <class INSTANCE, class FACTORY>
// inline
// int my_CountedHandle<INSTANCE, FACTORY>::numReferences() const
// {
//     return d_rep_p ? bces_AtomicUtil::getInt(d_count) : 0;
// }
//..
///Usage Example 3: Single-Linked Lists
///- - - - - - - - - - - - - - - - - -
// This example demonstrates the use of atomic pointers to implement fast and
// thread safe single-linked lists.  The example class, class 'my_PtrStack', is
// a templatized pointer stack, supporting 'push' and 'pop' methods.  The class
// is implemented using a single-linked list.  Items on the list a linked
// together using a node structure; instance of this structure are allocated
// using the provided allocator.
//
// When nodes are freed, they are cached on a free list; this free list is also
// implemented as a single-linked list, using atomic pointer operations.
//
// This example parallels the third usage example given for component
// 'bces_atomicutil', presenting a different implementation of
// 'my_PtrStack<T>', with an identical public interface.  Note that, where the
// 'bces_atomicutil' example uses the basic data type
// 'bces_AtomicUtil::Pointer' for members 'd_list_p' and 'd_freeList_p', this
// implementation uses the encapsulated type 'bces_AtomicPointer<T>'.
//..
// template <class TYPE>
// class my_PtrStack {
//
//     // DATA
//     typedef struct Node {
//         TYPE                 *d_item;
//         Node                 *d_next;
//         bces_AtomicUtil::Int  d_dirtyBit; // used to lock this node
//     };
//     bces_AtomicPointer<Node>  d_list_p;
//     bces_AtomicPointer<Node>  d_freeList_p;
//     bslma_Allocator *d_allocator_p;
//
//     // PRIVATE MANIPULATORS
//     Node *allocateNode();
//     void freeNode(Node *node);
//
//   public:
//     // CREATORS
//     my_PtrStack(bslma_Allocator *allocator = 0);
//    ~my_PtrStack();
//
//     // MANIPULATORS
//     void push(TYPE *item);
//     TYPE *pop();
// };
//..
// The constructor merely initializes the stack's allocator with the supplied
// argument.  In the parallel example in 'bces_atomictypes', the constructor
// must also initialize atomic pointers 'd_freeList_p' and 'd_allocator_p'.
// Since this example uses the encapsulated type 'bces_AtomicPointer',
// initialization of these member variables is done in their default
// constructors; no explicit code is required in this class.
//..
// // CREATORS
// template <class TYPE>
// inline my_PtrStack<TYPE>::my_PtrStack(bslma_Allocator *allocator)
// : d_allocator_p(bslma_Default::allocator(d_allocator_p))
// {
// }
//
// template <class TYPE>
// inline my_PtrStack<TYPE>::~my_PtrStack()
// {
// }
//..
// The method 'allocateNode' gets a node from the free list as follows.
//..
// // PRIVATE MANIPULATORS
// template <class TYPE>
// inline
// typename my_PtrStack<TYPE>::Node *my_PtrStack<TYPE>::allocateNode()
// {
//     Node *node;
//     do {
//        node = d_freeList_p; // get the current head
//        if (!node) {
//            break;
//        }
//
//..
// Try locking the node; start over if locking fails.
//..
//        if (bces_AtomicUtil::swapInt(&node->d_dirtyBit, 1)) {
//            continue;
//        }
//..
// Atomically modify the head if it has not changed.  'testAndSwap' compares
// 'd_freeList_p' to 'node', replacing it with 'node->d_next' only if it
// matches.  If 'd_freeList_p' did not match 'node', then the free list has
// been changed on another thread, between its assignment to the 'node' and the
// call to 'testAndSwap'.  If the list head has changed, then try again.
//..
//        if (d_freeList_p.testAndSwap(node, node->d_next) == node) {
//            break;
//        }
//
//        // Unlock the node.
//
//        bces_AtomicUtil::setInt(&node->d_dirtyBit, 0);
//     } while (1);
//..
// If there is no free node then allocate a new one using the provided
// allocator.
//..
//     if (!node) {
//         node = new(*d_allocator_p) Node();
//         bces_AtomicUtil::setInt(&node->d_dirtyBit, 1);
//     }
//     return node;
// }
//..
// Note that the 'node' is returned in the locked state and remained
// locked until it is added to the free list.
//
// The method 'freeNode' adds the given 'node' to the free list.
//..
// template <class TYPE>
// inline void my_PtrStack<TYPE>::freeNode(Node *node)
// {
//     if (!node) {
//         return;
//     }
//
//     do {
//         node->d_next = d_freeList_p;
//         // Atomically test and swap the head of the list with the
//         // new node.  If the list head has been changed (by another
//         // thread), try again.
//         if (d_freeList_p.testAndSwap(node->d_next, node) == node->d_next) {
//             break;
//         }
//     } while (1);
//
//     // unlock the 'node'
//     bces_AtomicUtil::setInt(&node->d_dirtyBit, 0);
// }
//..
// Methods 'push' and 'pop' provide the public "stack" interface for
// 'my_PtrStack'.
//
// The 'push' method is identical to 'freeNode', except that it operates on
// 'd_list_p', which contains active nodes.
//..
// // MANIPULATORS
// template <class TYPE>
// inline void my_PtrStack<TYPE>::push(TYPE *item)
// {
//     Node *node = allocateNode();
//     node->d_item = item;
//     do {
//         node->d_next = d_list_p;
//         if (d_list_p.testAndSwap(node->d_next, node) == node->d_next) {
//             break;
//         }
//     } while (1);
//     bces_AtomicUtil::setInt(&node->d_dirtyBit, 0);
// }
//
// template <class TYPE>
// inline TYPE *my_PtrStack<TYPE>::pop()
// {
//     Node *node;
//     do {
//         node = d_list_p;
//         if (!node) {
//             break;
//         }
//
//         if (bces_AtomicUtil::swapInt(&node->d_dirtyBit, 1)) {
//             continue;
//         }
//
//         if (d_list_p.testAndSwap(node, node->d_next) == node) {
//             break;
//         }
//
//         bces_AtomicUtil::setInt(&node->d_dirtyBit, 0);
//     } while (1);
//
//     TYPE *item = node ? node->d_item : 0;
//     if (node) {
//         freeNode(node);
//     }
//     return item;
// }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCES_ATOMICUTIL
#include <bces_atomicutil.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

namespace BloombergLP {

                               // ====================
                               // class bces_AtomicInt
                               // ====================

class bces_AtomicInt {
    // This class is a wrapper for an integer, supporting common integer
    // operations in a way that is guaranteed to be atomic.

    // DATA
    bces_AtomicUtil::Int d_value;

  public:
    // CREATORS
    bces_AtomicInt();
        // Construct an atomic integer object and initialize it to 0.

    bces_AtomicInt(int value);
        // Construct an atomic integer object and initialize it to the
        // specified 'value'.

    bces_AtomicInt(const bces_AtomicInt& original);
        // Construct an atomic integer object and initialize it to the
        // value of the specified 'original' object.

    // ~bces_AtomicInt();
        // Destroy this atomic integer object.  Note that this trivial
        // destructor is generated by the compiler.

    // MANIPULATORS
    bces_AtomicInt& operator=(const bces_AtomicInt& rhs);
        // Atomically assign to this atomic integer object the value of the
        // specified 'rhs' object.

    bces_AtomicInt& operator=(int value);
        // Atomically assign to this atomic integer object the specified
        // 'value'.

    void operator+=(int value);
        // Atomically add to this atomic integer the specified 'value'.

    void operator-=(int value);
        // Atomically subtract from this atomic integer the specified 'value'.

    int operator++();
        // Atomically increment the value of this atomic integer by 1 and
        // return the resulting value.

    int operator++(int);
        // Atomically increment the value of this atomic integer by 1 and
        // return the value prior to being incremented.

    int operator--();
        // Atomically decrement the value of this atomic integer by 1 and
        // return the resulting value.

    int operator--(int);
        // Atomically decrement the value of this atomic integer by 1 and
        // return the value prior to being decremented.

    int add(int value);
        // Atomically add to this atomic integer the specified 'value' and
        // return the resulting value.

    int relaxedAdd(int value);
        // Atomically add to this atomic integer the specified 'value' and
        // return the resulting value, without additional ordering
        // constraints.

    void relaxedStore(int value);
        // Atomically assign to this atomic integer object the specified
        // 'value', without additional ordering constraints.

    int swap(int swapValue);
        // Atomically set the value of this atomic integer to the specified
        // 'swapValue' and return its previous value.

    int testAndSwap(int compareValue, int swapValue);
        // Compare the value of this atomic integer to the specified
        // 'compareValue'.  If they are equal, set the value of this atomic
        // integer to the specified 'swapValue', otherwise leave this value
        // unchanged.  Return the previous value of this atomic integer,
        // whether or not the swap occurred.  Note that the entire
        // test-and-swap operation is performed atomically.

    // ACCESSORS
    operator int() const;
        // Return the current value of this atomic integer.

    int relaxedLoad() const;
        // Return the current value of this atomic integer without a memory
        // barrier.
};

                       // ======================
                       // class bces_AtomicInt64
                       // ======================

class bces_AtomicInt64 {
    // This class is a wrapper for a 64-bit integer, supporting common integer
    // operations in a way that is guaranteed to be atomic.

    // DATA
    bces_AtomicUtil::Int64 d_value;

  public:
    // CREATORS
    bces_AtomicInt64();
        // Construct an atomic 64-bit integer object and initialize it to 0.

    bces_AtomicInt64(bsls_PlatformUtil::Int64 value);
        // Construct an atomic 64-bit integer object and initialize it to the
        // specified 'value'.

    bces_AtomicInt64(const bces_AtomicInt64& original);
        // Construct an atomic 64-bit integer object and initialize it to the
        // value of the specified 'original' object.

    // ~bces_AtomicInt64();
        // Destroy this atomic 64-bit integer object.  Note that this trivial
        // destructor is generated by the compiler.

    // MANIPULATORS
    bces_AtomicInt64& operator=(const bces_AtomicInt64& rhs);
        // Atomically assign to this atomic 64-bit integer object the value of
        // the specified 'rhs' object.

    bces_AtomicInt64& operator=(bsls_PlatformUtil::Int64 value);
        // Atomically assign to this atomic 64-bit integer object the
        // specified 'value'.

    void operator+=(bsls_PlatformUtil::Int64 value);
        // Atomically add to this atomic 64-bit integer the specified 'value'.

    void operator-=(bsls_PlatformUtil::Int64 value);
        // Atomically subtract from this atomic 64-bit integer the specified
        // 'value'.

    bsls_PlatformUtil::Int64 operator++();
        // Atomically increment the value of this atomic 64-bit integer by 1
        // and return the resulting value.

    bsls_PlatformUtil::Int64 operator++(int);
        // Atomically increment the value of this atomic 64-bit integer by 1
        // and return the value prior to being incremented.

    bsls_PlatformUtil::Int64 operator--();
        // Atomically decrement the value of this atomic 64-bit integer by 1
        // and return the resulting value.

    bsls_PlatformUtil::Int64 operator--(int);
        // Atomically decrement the value of this atomic 64-bit integer by 1
        // and return the value prior to being decremented.

    bsls_PlatformUtil::Int64 add(bsls_PlatformUtil::Int64 value);
        // Atomically add to this atomic 64-bit integer the specified 'value'
        // and return the result value.

    bsls_PlatformUtil::Int64 relaxedAdd(bsls_PlatformUtil::Int64 value);
        // Atomically add to this atomic 64-bit integer the specified 'value'
        // and return the result value, without additional ordering
        // constraints.

    void relaxedStore(bsls_PlatformUtil::Int64 value);
        // Atomically assign to this atomic 64-bit integer object the
        // specified 'value', without additional ordering constraints.

    bsls_PlatformUtil::Int64 swap(bsls_PlatformUtil::Int64 swapValue);
        // Atomically set the value of this atomic 64-bit integer to the
        // specified 'swapValue' and return its previous value.

    bsls_PlatformUtil::Int64 testAndSwap(bsls_PlatformUtil::Int64 compareValue,
                                         bsls_PlatformUtil::Int64 swapValue);
        // Compare the value of this atomic 64-bit integer to the specified
        // 'compareValue'.  If they are equal, set the value of this atomic
        // 64-bit integer to the specified 'swapValue', otherwise leave this
        // value unchanged.  Return the previous value of this atomic 64-bit
        // integer, whether or not the swap occurred.  Note that the entire
        // test-and-swap operation is performed atomically.

    // ACCESSORS
    operator bsls_PlatformUtil::Int64() const;
        // Return the value of this atomic 64-bit integer.

    bsls_PlatformUtil::Int64 relaxedLoad() const;
        // Return the current value of this atomic 64-bit integer without
        // additional ordering constraints.
};

                     // ========================
                     // class bces_AtomicPointer
                     // ========================

template <class TYPE>
class bces_AtomicPointer {
    // This class provides a wrapper for pointers to a parameterized 'TYPE',
    // supporting pointer operations in a way that is guaranteed to be atomic.

    // DATA
    bces_AtomicUtil::Pointer d_value;

  public:
    // CREATORS
    bces_AtomicPointer();
        // Construct an atomic pointer to an object of the parameterized
        // 'TYPE' and initialize it to null.

    bces_AtomicPointer(TYPE *value);
        // Construct an atomic pointer to an object of the parameterized 'TYPE'
        // and initialize it to the specified pointer 'value'.

    bces_AtomicPointer(const bces_AtomicPointer<TYPE>& original);
        // Construct an atomic pointer and initialize it to point to the
        // same 'TYPE' instance as the specified 'original' object.

    // ~bces_AtomicPointer();
        // Destroy this atomic pointer.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    bces_AtomicPointer<TYPE>& operator=(const bces_AtomicPointer<TYPE>& rhs);
        // Atomically assign to this atomic pointer the value of the specified
        // 'rhs' object.

    bces_AtomicPointer<TYPE>& operator=(TYPE *value);
        // Atomically assign to this atomic pointer the specified pointer
        // 'value'.

    void relaxedStore(TYPE *value);
        // Atomically assign to this atomic pointer the specified 'value',
        // without additional ordering constraints.

    TYPE *swap(TYPE *swapValue);
        // Atomically set the value of this atomic pointer to the specified
        // 'swapValue' and return the previous value of this atomic pointer.

    TYPE *testAndSwap(const volatile TYPE *compareValue, TYPE *swapValue);
        // Compare the value of this atomic pointer to the specified
        // 'compareValue'.  If they are equal, set the value of this atomic
        // pointer to the specified 'swapValue', otherwise leave this value
        // unchanged.  Return the previous value of this atomic pointer,
        // whether or not the swap occurred.  Note that the entire
        // test-and-swap operation is performed atomically.

    // ACCESSORS
    TYPE& operator*() const;
        // Return a reference to the object currently pointed to by this atomic
        // pointer.  The behavior is undefined if this pointer has a value of
        // 0.

    TYPE *operator->() const;
        // Return the value of this atomic pointer.

    operator TYPE*() const;
        // Return the value of this atomic pointer.  Note that this method can
        // be used to determine if this pointer is null.

    TYPE *relaxedLoad() const;
        // Return the value of this atomic pointer without additional
        // ordering constraints.

};

                     // =========================
                     // class bces_AtomicSpinLock
                     // =========================

class bces_SpinLock {
    // This class is a wrapper to an atomic spin lock, supporting the 'lock',
    // 'tryLock' and 'unlock' methods.

    // DATA
    bces_AtomicUtil::SpinLock d_spin;

    // NOT IMPLEMENTED
    bces_SpinLock(const bces_SpinLock&);
    bces_SpinLock& operator=(const bces_SpinLock&);

  public:
    // CREATORS
    bces_SpinLock();
        // Construct a spin lock and initialize it to the unlock state.

    // ~bces_SpinLock();
        // Destroy this spin lock.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    void lock();
        // Lock this spinlock.

    int tryLock(int numRetries = 100);
        // Try to lock this spin lock.  Optionally specify 'numRetries'
        // indicating the maximum number of subsequent attempts to lock this
        // spin lock if the first attempt fails.  If 'numRetries' is not
        // specified, attempt to lock this spin-lock 100 times after the
        // initial attempt.  Return 0 on success, and a non-zero value if the
        // lock could not be acquired.

    void unlock();
        // Unlock this spinlock which was previously locked by a successful
        // call to either 'lock' or 'tryLock'.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

              // ---------------------------------------------
              // inlined methods used by other inlined methods
              // ---------------------------------------------

inline
bces_AtomicInt::operator int() const
{
    return bces_AtomicUtil::getInt(d_value);
}

inline
bces_AtomicInt64::operator bsls_PlatformUtil::Int64() const
{
    return bces_AtomicUtil::getInt64(d_value);
}

template <class TYPE>
inline
bces_AtomicPointer<TYPE>::operator TYPE*() const
{
    return (TYPE*)bces_AtomicUtil::getPtr(d_value);
}

                               // --------------------
                               // class bces_AtomicInt
                               // --------------------

// CREATORS
inline
bces_AtomicInt::bces_AtomicInt()
{
    bces_AtomicUtil::initInt(&d_value, 0);
}

inline
bces_AtomicInt::bces_AtomicInt(int value)
{
    bces_AtomicUtil::initInt(&d_value, value);
}

inline
bces_AtomicInt::bces_AtomicInt(const bces_AtomicInt& original)
{
    bces_AtomicUtil::initInt(&d_value,
                             bces_AtomicUtil::getInt(original.d_value));
}

// MANIPULATORS
inline
bces_AtomicInt& bces_AtomicInt::operator=(const bces_AtomicInt& rhs)
{
    bces_AtomicUtil::setInt(&d_value, rhs);
    return *this;
}

inline
bces_AtomicInt& bces_AtomicInt::operator=(int value)
{
    bces_AtomicUtil::setInt(&d_value, value);
    return *this;
}

inline
void bces_AtomicInt::operator+=(int value)
{
    bces_AtomicUtil::addInt(&d_value, value);
}

inline
void bces_AtomicInt::operator-=(int value)
{
    bces_AtomicUtil::addInt(&d_value, -value);
}

inline
int bces_AtomicInt::operator++()
{
    return bces_AtomicUtil::incrementIntNv(&d_value);
}

inline
int bces_AtomicInt::operator++(int)
{
    return bces_AtomicUtil::incrementIntNv(&d_value) - 1;
}

inline
int bces_AtomicInt::operator--()
{
    return bces_AtomicUtil::decrementIntNv(&d_value);
}

inline
int bces_AtomicInt::operator--(int)
{
    return bces_AtomicUtil::decrementIntNv(&d_value) + 1;
}

inline
int bces_AtomicInt::add(int value)
{
    return bces_AtomicUtil::addIntNv(&d_value, value);
}

inline
int bces_AtomicInt::relaxedAdd(int value)
{
    return bces_AtomicUtil::addIntNvRelaxed(&d_value, value);
}

inline
void bces_AtomicInt::relaxedStore(int value)
{
    bces_AtomicUtil::setIntRelaxed(&d_value, value);
}

inline
int bces_AtomicInt::swap(int value)
{
    return bces_AtomicUtil::swapInt(&d_value, value);
}

inline
int bces_AtomicInt::testAndSwap(int compareValue, int swapValue)
{
    return bces_AtomicUtil::testAndSwapInt(&d_value, compareValue, swapValue);
}

// ACCESSORS
inline
int bces_AtomicInt::relaxedLoad() const
{
    return bces_AtomicUtil::getIntRelaxed(d_value);
}

                       // ----------------------
                       // class bces_AtomicInt64
                       // ----------------------

// CREATORS
inline
bces_AtomicInt64::bces_AtomicInt64()
{
    bces_AtomicUtil::initInt64(&d_value, 0);
}

inline
bces_AtomicInt64::bces_AtomicInt64(bsls_PlatformUtil::Int64 value)
{
    bces_AtomicUtil::initInt64(&d_value, value);
}

inline
bces_AtomicInt64::bces_AtomicInt64(const bces_AtomicInt64& original)
{
    bces_AtomicUtil::initInt64(&d_value,
                               bces_AtomicUtil::getInt64(original.d_value));
}

// MANIPULATORS
inline
bces_AtomicInt64& bces_AtomicInt64::operator=(const bces_AtomicInt64& rhs)
{
    bces_AtomicUtil::setInt64(&d_value, rhs);
    return *this;
}

inline
bces_AtomicInt64& bces_AtomicInt64::operator=(bsls_PlatformUtil::Int64 value)
{
    bces_AtomicUtil::setInt64(&d_value, value);
    return *this;
}

inline
void bces_AtomicInt64::operator+=(bsls_PlatformUtil::Int64 value)
{
    bces_AtomicUtil::addInt64(&d_value, value);
}

inline
void bces_AtomicInt64::operator-=(bsls_PlatformUtil::Int64 value)
{
    bces_AtomicUtil::addInt64(&d_value, -value);
}

inline
bsls_PlatformUtil::Int64 bces_AtomicInt64::operator++()
{
    return bces_AtomicUtil::incrementInt64Nv(&d_value);
}

inline
bsls_PlatformUtil::Int64 bces_AtomicInt64::operator++(int)
{
    return bces_AtomicUtil::incrementInt64Nv(&d_value) - 1;
}

inline
bsls_PlatformUtil::Int64 bces_AtomicInt64::operator--()
{
    return bces_AtomicUtil::decrementInt64Nv(&d_value);
}

inline
bsls_PlatformUtil::Int64 bces_AtomicInt64::operator--(int)
{
    return bces_AtomicUtil::decrementInt64Nv(&d_value) + 1;
}

inline
bsls_PlatformUtil::Int64 bces_AtomicInt64::add(bsls_PlatformUtil::Int64 value)
{
    return bces_AtomicUtil::addInt64Nv(&d_value, value);
}

inline
bsls_PlatformUtil::Int64 bces_AtomicInt64::relaxedAdd(
                                                bsls_PlatformUtil::Int64 value)
{
    return bces_AtomicUtil::addInt64NvRelaxed(&d_value, value);
}

inline
void bces_AtomicInt64::relaxedStore(bsls_PlatformUtil::Int64 value)
{
    bces_AtomicUtil::setInt64Relaxed(&d_value, value);
}

inline
bsls_PlatformUtil::Int64 bces_AtomicInt64::swap(bsls_PlatformUtil::Int64 value)
{
    return bces_AtomicUtil::swapInt64(&d_value, value);
}

inline
bsls_PlatformUtil::Int64
 bces_AtomicInt64::testAndSwap(bsls_PlatformUtil::Int64 compareValue,
                               bsls_PlatformUtil::Int64 swapValue)
{
    return bces_AtomicUtil::testAndSwapInt64(&d_value,
                                             compareValue,
                                             swapValue);
}

// ACCESSORS
inline
bsls_PlatformUtil::Int64 bces_AtomicInt64::relaxedLoad() const
{
    return bces_AtomicUtil::getInt64Relaxed(d_value);
}

                     // ------------------------
                     // class bces_AtomicPointer
                     // ------------------------

// CREATORS
template <class TYPE>
inline
bces_AtomicPointer<TYPE>::bces_AtomicPointer()
{
    bces_AtomicUtil::initPointer(&d_value, 0);
}

template <class TYPE>
inline
bces_AtomicPointer<TYPE>::bces_AtomicPointer(TYPE *value)
{
    bces_AtomicUtil::initPointer(&d_value, value);
}

template <class TYPE>
inline
bces_AtomicPointer<TYPE>::bces_AtomicPointer(
                                      const bces_AtomicPointer<TYPE>& original)
{
    bces_AtomicUtil::initPointer(&d_value, (TYPE*)original);
}

// MANIPULATORS
template <class TYPE>
inline
bces_AtomicPointer<TYPE>&
bces_AtomicPointer<TYPE>::operator=(const bces_AtomicPointer<TYPE>& rhs)
{
    bces_AtomicUtil::setPtr(&d_value, rhs);
    return *this;
}

template <class TYPE>
inline
bces_AtomicPointer<TYPE>&
bces_AtomicPointer<TYPE>::operator=(TYPE *value)
{
    bces_AtomicUtil::setPtr(&d_value, value);
    return *this;
}

template <class TYPE>
inline
TYPE *bces_AtomicPointer<TYPE>::swap(TYPE *swapValue)
{
    return (TYPE*)bces_AtomicUtil::swapPtr(&d_value, swapValue);
}

template <class TYPE>
inline
TYPE *bces_AtomicPointer<TYPE>::testAndSwap(const volatile TYPE *compareValue,
                                            TYPE                *swapValue)
{
    return (TYPE*)bces_AtomicUtil::testAndSwapPtr(&d_value,
                                                  compareValue,
                                                  swapValue);
}

template <class TYPE>
inline
void bces_AtomicPointer<TYPE>::relaxedStore(TYPE *value)
{
    bces_AtomicUtil::setPtrRelaxed(&d_value, value);
}

// ACCESSORS
template <class TYPE>
inline
TYPE& bces_AtomicPointer<TYPE>::operator*() const
{
    return *((TYPE*)bces_AtomicUtil::getPtr(d_value));
}

template <class TYPE>
inline
TYPE *bces_AtomicPointer<TYPE>::operator->() const
{
    return (TYPE*)bces_AtomicUtil::getPtr(d_value);
}

template <class TYPE>
inline
TYPE *bces_AtomicPointer<TYPE>::relaxedLoad() const
{
    return (TYPE*)bces_AtomicUtil::getPtrRelaxed(d_value);
}

                     // -------------------------
                     // class bces_AtomicSpinLock
                     // -------------------------

// CREATORS
inline
bces_SpinLock::bces_SpinLock()
{
    bces_AtomicUtil::initSpinLock(&d_spin);
}

// MANIPULATORS
inline
void bces_SpinLock::lock()
{
    bces_AtomicUtil::spinLock(&d_spin);
}

inline
int bces_SpinLock::tryLock(int numRetries)
{
    return bces_AtomicUtil::spinTryLock(&d_spin, numRetries);
}

inline
void bces_SpinLock::unlock()
{
    bces_AtomicUtil::spinUnlock(&d_spin);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002, 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
