// bces_atomicutil.h                                                  -*-C++-*-
#ifndef INCLUDED_BCES_ATOMICUTIL
#define INCLUDED_BCES_ATOMICUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide platform-independent atomic operations.
//
//@CLASSES:
//   bces_AtomicUtil: namespace for platform-independent atomic operations
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This utility provides a set of platform independent primitives
// for atomic operations.  An atomic operation is a sequence of one or more
// instructions guaranteed to execute in serial order, without interruption, in
// a multi-threaded, multi-processor environment.  Note that there is *no*
// guarantee on the order in which atomic operations on separate threads are
// executed relative to each other.
//
// Atomic operations are useful for manipulating certain types of shared data
// without the need for high level synchronization mechanisms (e.g., "mutexes"
// or "critical sections").
//
// The 'bces_atomicutil' component provides three types of atomic operations:
//..
//   >  Atomic Integer Operations
//   >  Atomic Pointer Operations
//   >  Spin Locks
//..
// Atomic integer operations allow for thread-safe manipulation of a single 32
// or 64 bit integer value, without the use of other synchronization
// mechanisms.  Spin Locks are used to efficiently synchronize access to shared
// data or areas of a program.  Even the most basic operations on data that is
// shared among multiple threads must use some form of synchronization to
// ensure proper results.
//
// Consider the following snippet of C code:
//..
//    static int x;
//    ++x;
//..
// Although the increment statement looks very simple, it may result in several
// machine instructions.  Depending on the architecture and compiler
// optimization options, the statement might result in the following pseudo
// machine instructions:
//..
//     LOAD 'x' to register
//     ADD 1 to register
//     STORE register to 'x'
//..
// Consider the situation ff the statement were executed simultaneously by two
// threads.  Thread A could load 'x' to a register, and then be interrupted by
// the operating system.  Thread B could then begin to execute, and complete
// all three instructions, loading, incrementing, and storing the variable 'x'.
// When thread A resumes, it would increment the value that it loaded, and
// store the result.
//
// It is thus possible that both threads load the same value of x to the
// register, add one, and store their individual but equal results, incorrectly
// incrementing 'x' by only 1 instead of the correct value 2.  One could
// correct this problem by using a high level synchronization mechanisms (e.g.,
// "mutex"), but these mechanisms are generally very expensive for such a small
// fragment of code, and could result in a large number of unnecessary context
// switches, for instance, if the increment statement occurs within a loop.
//
// Instead, an atomic operation (in this case, 'bces_AtomicUtil::incrementInt')
// can be used to manipulate the value; use of this operation will ensure that,
// when executed simultaneously by multiple threads, the threads will increment
// the value serially, without interrupting one another.
//
// Special data types are provided to represent these values.  Atomic data
// types are generally the same as their corresponding primitive data types,
// and therefore typically do not incur any additional memory overhead.
// However, on some platforms, it may be necessary to use larger, more complex
// types to represent these values; therefore they must always be accessed or
// manipulated using their accessors or manipulators, respectively.  Not doing
// so may result in incorrect results and/or non-portable code.
//
// Atomic operations should not be used in situations where behavior is
// inherently thread safe; although they are typically much faster than using
// high-level synchronization mechanisms to accomplish the same results, atomic
// operations are typically more expensive (in both speed and code size) than
// their non-atomic equivalents.
//
///Ordering constraints and "Relaxed" Functions
///---------------------------------------------------
// Most operations provided by this component include ordering constraints,
// such as (but not limited to) memory barriers.  These ensure that
// instructions prior to the operation are executed before it, and
// instructions subsequent to the operation are executed after it; they also
// ensure that other CPUs observe those operations happening in the same order
// they were executed.  For example, operations on an atomic integer used as a
// spin-lock require these ordering constraints, because other CPUs must not
// see any operations that follow the lock operation until the value of the
// lock has changed.
//
// In certain cases where the *value* of a variable must be manipulated
// atomically, but it is not important that the value be changed in a
// particular order with respect to other operations, these ordering
// constraints are not necessary.  For example, variables storing performance
// metrics may be updated and loaded from multiple threads simultaneously, but
// it is not necessary to synchronize the order of execution based on the
// values of these variables.  For these cases, this component supplies
// "relaxed" operations.  These methods ensure that values are atomically
// updated, but offer better performance (on some architectures) when
// synchronizing the order of execution
// is not necessary.
//
///Atomic Integer Operations
///-------------------------
// The atomic integer operations provide thread-safe access for 32 or 64 bit
// signed integer numbers without the use of higher level synchronization
// mechanisms.  Atomic integers are most commonly used to manipulate shared
// counters and indices.  Five types of operations are provided; get/set,
// increment/decrement, add, swap, and test and swap.  Two sub-types of
// manipulators are provided for increment/decrement and addition operations.
//
// 'bces_AtomicUtil' manipulators whose names end in "Nv" (e.g., 'addIntNv',
// 'incrementInt64Nv') return the resulting value of the operations; those
// without the suffix do not return a value.  If an application does not
// require the resulting value of an operation, it should not use the "Nv"
// manipulator.  On some platforms, it may be less efficient to determine the
// resulting value of an operation than to simply perform the operation.
//
///Atomic Pointer Operations
///-------------------------
// The atomic pointer operations provide thread-safe access to pointer values
// without the use of higher level synchronization mechanisms.  They are
// commonly used to create fast thread safe single linked lists.
//
///Spin Locks
///----------
// Spin locks provide an efficient synchronization mechanism similar to *mutex*
// locks, with the exception that they do not trigger thread context switches.
// Spin locks typically have small memory footprints (one byte on most
// platforms).  Most implementations of spin locks use a toggle mechanism: a
// thread that wishes to acquire the lock will continuously test the value of
// the lock until it becomes unlocked, then atomically switch it to the locked
// state.
//
// When used inappropriately, spin locks can cause significant performance
// problems.  Since it does not yield processor time while waiting to acquire a
// lock, a spin lock should be used ONLY for fast operations; similarly, it
// should never be used in a situation where the thread can block after it has
// locked the spin lock, and before it has unlocked the lock.
//
// Spin locks are non-recursive; therefore, care must be taken to avoid
// deadlocks when multiple spin locks are employed simultaneously.  Spin locks
// can be acquired using the infinite retry ('spinLock') or finite retry
// ('spinTryLock') methods.
//
///Usage
///-----
// The following examples demonstrate various uses of atomic operations.
// Compare these examples to the corresponding examples in 'bces_atomictypes'.
//
///Example 1: Usage Statistics on a Thread Pool
///- - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates the common use of atomic integer
// operations for statistical counters.  The program creates a series of
// threads to process transactions.  As each thread completes a transaction, it
// atomically increments the transaction counters.
//
// For this example, we assume the existence of the functions
// 'processNextTransaction', 'createWorkerThread' and 'waitAllThreads'.  The
// function 'createWorkerThread' spawns a new thread; this thread then executes
// the 'workerThread' function.  'waitAllThreads' blocks until all the worker
// thread complete.
//
// First, we declare the shared counters:
//..
// static bces_AtomicUtil::Int64 transactionCount;
// static bces_AtomicUtil::Int64 successCount;
// static bces_AtomicUtil::Int64 failureCount;
//..
// For each transaction processed, we atomically increment the success/failure
// counters and the total transaction count.
//..
// static void workerThread(int *stop)
// {
//     while (!(*stop)) {
//         if (processNextTransaction()) {
//             bces_AtomicUtil::incrementInt64(&failureCount);
//         }
//         else {
//             bces_AtomicUtil::incrementInt64(&successCount);
//         }
//         bces_AtomicUtil::incrementInt64(&transactionCount);
//     }
// }
//..
// The function 'serverMain' provides the overall control logic for the server;
// it spawns the threads and then waits for all work to be completed.  When all
// threads have finished, the function returns normally.
//..
// void serverMain()
// {
//     const int numThreads = 10;
//..
// Before any of the counters is used, they must be initialized.  'initInt64'
// is called to initialize each value to 0.
//..
//     bces_AtomicUtil::initInt64(&transactionCount, 0);
//     bces_AtomicUtil::initInt64(&successCount, 0);
//     bces_AtomicUtil::initInt64(&failureCount, 0);
//..
// Spawn the threads to process the transactions concurrently.
//..
//     for (int i = 0; i < numThreads; ++i) {
//         createWorkerThread();
//     }
//..
// Wait for all the threads to complete.
//..
//     waitAllThreads();
// }
//..
// The functions 'createWorkerThread' and 'waitAllThreads' can be implemented
// using any thread support package.  Component 'bcemt_thread' provides the
// required functionality in functions 'bcemt_ThreadUtil::create()' and
// bcemt_ThreadUtil::join()'.
//
///Example 2: Thread-safe Counted Handle
///- - - - - - - - - - - - - - - - - - -
// The next example demonstrates the use of atomic integer operations to
// implement a thread safe counted handle similar to 'bdema_SharedPtr'.
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
// (and there no more references to the object), the handle deletes the
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
// member functions 'increment' and 'decrement' must work atomically.
//
// Note that, since this class is to be used only by class 'my_CountedHandle',
// all methods of class 'my_CountedHandleRep' are declared private and 'friend'
// status is granted to class 'my_CountedHandle'.
//..
//                         // =========================
//                         // class my_CountedHandleRep
//                         // =========================
//
// template <class INSTANCE, class FACTORY>
// class my_CountedHandleRep {
//     bces_AtomicUtil::Int d_count;        // number of active references
//     INSTANCE            *d_instance_p;   // address of managed instance
//     FACTORY             *d_factory_p;    // held but not owned
//     bslma_Allocator     *d_allocator_p;  // held but not owned
//
//     friend class my_CountedHandle<INSTANCE, FACTORY>;
//
//   private: // not implemented
//     my_CountedHandleRep(const my_CountedHandleRep&);
//     my_CountedHandleRep& operator=(const my_CountedHandleRep&);
//
//   private:
//     // CLASS METHODS
//     static void
//     deleteObject(my_CountedHandleRep<INSTANCE, FACTORY> *object);
//
//     // CREATORS
//     my_CountedHandleRep(INSTANCE        *instance,
//                         FACTORY         *factory,
//                         bslma_Allocator *basicAllocator);
//     ~my_CountedHandleRep();
//
//     // MANIPULATORS
//     void increment();
//
//     int decrement();
// };
//..
///Class 'my_CountedHandle'
///-  -  -  -  -  -  -  -  -
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
//..
//                         // ======================
//                         // class my_CountedHandle
//                         // ======================
//
// template <class INSTANCE, class FACTORY>
// class my_CountedHandle {
//     my_CountedHandleRep<INSTANCE, FACTORY> *d_rep_p;  // shared rep.
//
//   public:
//     // CREATORS
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
// 'static' member function 'deleteObject' is called by the destructor for
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
// newly-constructed 'INSTANCE' variable.  We initialize the atomic reference
// counter to one reference using 'bces_AtomicUtil::initInt'; this reflects the
// fact that this constructor will be called by a new instance of
// 'my_CountedHandle'; that instance is our first and only handle when this
// constructor is called.
//..
// template <class INSTANCE, class FACTORY>
// inline
// my_CountedHandleRep<INSTANCE, FACTORY>::
//                         my_CountedHandleRep(INSTANCE        *instance,
//                                             FACTORY         *factory,
//                                             bslma_Allocator *allocator)
// : d_instance_p(instance)
// , d_factory_p(factory)
// , d_allocator_p(allocator)
// {
//     bces_AtomicUtil::initInt(&d_count, 1);
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
// Member function 'increment' atomically increments the number of references
// to this 'my_CountedHandleRep'.  Since our caller is not interested in the
// result (and our return type is thus 'void'), we use 'incrementInt' instead
// of 'incrementIntNv'.
//..
// template <class INSTANCE, class FACTORY>
// inline
// void my_CountedHandleRep<INSTANCE, FACTORY>::increment()
// {
//     bces_AtomicUtil::incrementInt(&d_count);
// }
//
//..
// Member function 'decrement' atomically decrements the reference count; since
// our caller will need to check the resulting value to determine whether the
// 'INSTANCE' should be deleted, we use 'decrementIntNv' rather than
// 'decrementInt', and return the new number of references.
//..
//
// template <class INSTANCE, class FACTORY>
// inline
// int my_CountedHandleRep<INSTANCE, FACTORY>::decrement()
// {
//     return bces_AtomicUtil::decrementIntNv(&d_count);
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
// template <class INSTANCE, class FACTORY>
// inline
// my_CountedHandle<INSTANCE, FACTORY>::my_CountedHandle(
//                                             INSTANCE        *object,
//                                             FACTORY         *factory,
//                                             bslma_Allocator *allocator)
// {
//     bslma_Allocator *ba = bslma_Default::allocator(allocator);
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
// Member function 'operator->()' provides basic pointer semantics.  Note that,
// while class 'my_CountedHandleRep' is itself fully thread-aware, it does not
// guaranty thread safety for the 'INSTANCE' object.  In order to provide
// thread safety for the 'INSTANCE' in the general case, the "rep" would need
// to use a more general concurrency mechanism such as a mutex.  For more
// information, see class 'bcemt_Mutex' in component 'bcemt_thread'.
//..
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
//
///Usage Example 3: Single-Linked Lists
///- - - - - - - - - - - - - - - - - -
// This example demonstrates the use of atomic pointers to implement fast and
// thread safe single-linked lists.  The example class, class 'my_PtrStack', is
// a templatized pointer stack, supporting 'push' and 'pop' methods.  The class
// is implemented using a single-linked list.  Items on the list are linked
// together using a node structure; instances of this structure are allocated
// using the provided allocator.
//
// When nodes are freed, they are cached on a free list; this free list is also
// implemented as a single-linked list, using atomic pointer operations.
//..
//   template <class TYPE>
//   class my_PtrStack {
//       struct Node {
//           TYPE *d_item;
//           Node *d_next;
//       };
//       bces_AtomicUtil::Pointer  d_list_p;
//       bces_AtomicUtil::Pointer  d_freeList_p;
//       bslma_Allocator *d_allocator_p;
//       Node *allocateNode();
//       void freeNode(Node *node);
//     public:
//       my_PtrStack(bslma_Allocator *allocator=0);
//      ~my_PtrStack();
//       void push(TYPE *item);
//       TYPE *pop();
//   };
//..
// The constructor initializes the pointers for the item list and the free
// list.  The atomic operation 'initPointer' guarantees that each operation is
// atomic.
//..
//   template <class TYPE>
//   inline my_PtrStack<TYPE>::my_PtrStack(bslma_Allocator *allocator)
//   : d_allocator_p(allocator)
//   {
//       bces_AtomicUtil::initPointer(&d_freeList_p, 0);
//       bces_AtomicUtil::initPointer(&d_list_p, 0);
//   }
//
//   template <class TYPE>
//   inline my_PtrStack<TYPE>::~my_PtrStack()
//   {
//   }
//..
// Member function 'allocateNode' gets a node from the free list.
//..
//   template <class TYPE>
//   inline my_PtrStack<TYPE>::Node *my_PtrStack<TYPE>::allocateNode()
//   {
//       Node *node;
//..
// To remove an item from this list, get the current list head using 'getPtr'.
// Then, test and swap it with the next node.  'testAndSwapPtr' compares
// 'd_freeList_p' to 'node', replacing it with 'node->d_next' only if it
// matches.  If 'd_freeList_p' did not match 'node', then the free list has
// been changed on another thread, between the calls to 'getPtr' and
// 'testAndSwapPtr'.  If the list head has changed, then try again.
//..
//       do {
//           node = (Node*)bces_AtomicUtil::getPtr(d_freeList_p);
//           if (!node) break;
//       } while (bces_AtomicUtil::testAndSwapPtr(&d_freeList_p,
//                                                node,
//                                                node->d_next) != node);
//..
// If there is no free node then allocate a new one using the provided
// allocator.
//..
//       if (!node) {
//           bslma_Allocator *ba =
//                         bslma_Default::allocator(d_allocator_p);
//           node = new(ba) Node();
//       }
//       return node;
//   }
//..
// 'freeNode' adds the given 'node' to the free list.  To add the node to the
// list, set the next pointer of the new node to the current value of the list
// head.  Atomically test and swap the head of the list with the new node.  If
// the list head has been changed (by another thread), try again.
//..
//   template <class TYPE>
//   inline void my_PtrStack<TYPE>::freeNode(Node *node)
//   {
//       do {
//           node->d_next = (Node*)bces_AtomicUtil::getPtr(d_freeList_p);
//       } while (bces_AtomicUtil::testAndSwapPtr(&d_freeList_p,
//                                                node->d_next,
//                                                node) != node->d_next);
//   }
//..
// Member functions 'push' and 'pop' provide the public "stack" interface for
// 'my_PtrStack'.
//
// The 'push' member function is identical to 'freeNode', except that it
// operates on 'd_list_p', which contains active nodes.
//..
//   template <class TYPE>
//   inline void my_PtrStack<TYPE>::push(TYPE *item)
//   {
//       Node *node = allocateNode();
//       node->d_item = item;
//       do {
//           node->d_next = (Node*)bces_AtomicUtil::getPtr(d_list_p);
//       } while (bces_AtomicUtil::testAndSwapPtr(&d_list_p,
//                                                node->d_next,
//                                                node) != node->d_next);
//   }
//..
// The 'pop' member function is parallel to 'allocateNode', except that it
// operates on 'd_list_p', and returns the freed node to the free list.
//..
//
//   template <class TYPE>
//   inline my_PtrStack<TYPE>::TYPE *my_PtrStack<TYPE>::pop()
//   {
//       Node *node;
//       do {
//           node = (Node*)bces_AtomicUtil::getPtr(d_list_p);
//           if (!node) break;
//       } while (bces_AtomicUtil::testAndSwapPtr(&d_freeList_p,
//                                                node,
//                                                node->d_next) != node);
//       TYPE *item = node ? node->d_item : 0;
//       if (node)
//           freeNode(node);
//       return item;
//   }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCES_ATOMICUTILIMPL_SPARC32
#include <bces_atomicutilimpl_sparc32.h>
#endif

#ifndef INCLUDED_BCES_ATOMICUTILIMPL_SPARCV9
#include <bces_atomicutilimpl_sparcv9.h>
#endif

#ifndef INCLUDED_BCES_ATOMICUTILIMPL_AMD64
#include <bces_atomicutilimpl_amd64.h>
#endif

#ifndef INCLUDED_BCES_ATOMICUTILIMPL_IA64
#include <bces_atomicutilimpl_ia64.h>
#endif

#ifndef INCLUDED_BCES_ATOMICUTILIMPL_INTEL_PENTIUM
#include <bces_atomicutilimpl_intel_pentium.h>
#endif

#ifndef INCLUDED_BCES_ATOMICUTILIMPL_POWERPC
#include <bces_atomicutilimpl_powerpc.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

namespace BloombergLP {

template <typename PLATFORM> struct bces_AtomicUtilImpl;

                        // ======================
                        // struct bces_AtomicUtil
                        // ======================

struct bces_AtomicUtil {
    // This 'struct' provides a namespace for a suite of atomic operations.
    // Each operation is guaranteed to execute atomically on the current
    // architecture (as defined by bsls_Platform::Cpu).

    // TYPES
    typedef bces_AtomicUtilImpl<bsls_Platform::Cpu>            Impl;
    typedef bces_AtomicUtilImpl<bsls_Platform::Cpu>::Int       Int;
    typedef bces_AtomicUtilImpl<bsls_Platform::Cpu>::Int64     Int64;
    typedef bces_AtomicUtilImpl<bsls_Platform::Cpu>::Pointer   Pointer;
    typedef bces_AtomicUtilImpl<bsls_Platform::Cpu>::SpinLock  SpinLock;

    // CLASS METHODS
    static void initInt(bces_AtomicUtil::Int *atomicInt, int initalValue = 0);
        // Initialize the specified 'atomicInt' and set its value to the
        // specified 'initialValue'.  Note that a 'bces_AtomicUtil::Int' may
        // also be initialized through aggregate initialization.
        //
        // DEPRECATED: use '= { <value> };' instead.

    static void initInt64(bces_AtomicUtil::Int64   *atomicInt,
                          bsls_PlatformUtil::Int64  initialValue = 0LL );
        // Initialize the specified 'atomicInt' and set its value to the
        // specified 'initialValue'.  Note that a 'bces_AtomicUtil::Int' may
        // also be initialized through aggregate initialization.
        //
        // DEPRECATED: use '= { <value> };' instead.

    static void initPointer(bces_AtomicUtil::Pointer *atomicPtr,
                            const volatile void      *initalValue = 0);
        // Initialize the specified 'atomicPtr' and set its value to the
        // specified 'initialValue'.  Note that a 'bces_AtomicUtil::Int' may
        // also be initialized through aggregate initialization.
        //
        // DEPRECATED: use '= { <value> };' instead.

    static void initSpinLock(bces_AtomicUtil::SpinLock *spinlock);
        // Initialize the specified 'spinlock' and set it to an unlocked state.
        // Note that this method must be called before any other operation on
        // 'spinlock'.

    static void addInt(bces_AtomicUtil::Int *atomicInt, int value);
        // Atomically add to the specified 'atomicInt' the specified 'value'.
        // The behavior is undefined if 'atomicInt' is 0.

    static void incrementInt(bces_AtomicUtil::Int *atomicInt);
        // Atomically increment the value of the specified 'atomicInt' by 1.
        // The behavior is undefined if 'atomicInt' is 0.

    static void decrementInt(bces_AtomicUtil::Int *atomicInt);
        // Atomically decrement the value of the specified 'atomicInt' by 1.
        // The behavior is undefined if 'atomicInt' is 0.

    static int swapInt(bces_AtomicUtil::Int *atomicInt, int swapValue);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value', and return its previous value.  The behavior is
        // undefined if 'atomicInt' is 0.

    static int testAndSwapInt(bces_AtomicUtil::Int *atomicInt,
                              int                   compareValue,
                              int                   swapValue);
        // Conditionally set the value of the specified 'atomicInt' to the
        // specified 'swapValue' if and only if the value of 'atomicInt' equals
        // the value of the specified 'compareValue', and return the initial
        // value of 'atomicInt'.  The whole operation is performed atomically.
        // The behavior is undefined if 'atomicInt' is 0.

    static void setInt(bces_AtomicUtil::Int *atomicInt, int value);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value'.  The behavior is undefined if 'atomicInt' is 0.

    static void setIntRelaxed(bces_AtomicUtil::Int *atomicInt, int value);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value', without additional ordering constraints.  The
        // behavior is undefined if 'atomicInt' is 0.

    static int getInt(const bces_AtomicUtil::Int& atomicInt);
        // Atomically retrieve the value of the specified 'atomicInt'.

    static int getIntRelaxed(const bces_AtomicUtil::Int& atomicInt);
        // Atomically retrieve the value of the specified 'atomicInt', without
        // additional ordering constraints.

    static int addIntNv(bces_AtomicUtil::Int *atomicInt, int val);
        // Atomically add to the specified 'atomicInt' the specified 'value'
        // and return the resulting value.  The behavior is undefined if
        // 'atomicInt' is 0.

    static int addIntNvRelaxed(bces_AtomicUtil::Int *atomicInt, int val);
        // Atomically add to the specified 'atomicInt' the specified 'value'
        // and return the resulting value, without additional ordering
        // constraints.  The behavior is undefined if 'atomicInt' is 0.

    static int incrementIntNv(bces_AtomicUtil::Int *atomicInt);
        // Atomically increment the specified 'atomicInt' by 1 and return the
        // resulting value.  The behavior is undefined if 'atomicInt' is 0.

    static int decrementIntNv(bces_AtomicUtil::Int *atomicInt);
        // Atomically decrement the specified 'atomicInt' by 1 and return the
        // resulting value.  The behavior is undefined if 'atomicInt' is 0.

    static void addInt64(bces_AtomicUtil::Int64   *atomicInt,
                         bsls_PlatformUtil::Int64  value);
        // Atomically add to the specified 'atomicInt' the specified 'value'.
        // The behavior is undefined if 'atomicInt' is 0.

    static void incrementInt64(bces_AtomicUtil::Int64 *atomicInt);
        // Atomically increment the value of the specified 'atomicInt' by 1.
        // The behavior is undefined if 'atomicInt' is 0.

    static void decrementInt64(bces_AtomicUtil::Int64 *atomicInt);
        // Atomically decrement the value of the specified 'atomicInt' by 1.
        // The behavior is undefined if 'atomicInt' is 0.

    static bsls_PlatformUtil::Int64
                                swapInt64(bces_AtomicUtil::Int64   *atomicInt,
                                          bsls_PlatformUtil::Int64  swapValue);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value' and return its previous value.  The behavior is
        // undefined if 'atomicInt' is 0.

    static bsls_PlatformUtil::Int64 testAndSwapInt64(
                                        bces_AtomicUtil::Int64   *atomicInt,
                                        bsls_PlatformUtil::Int64  compareValue,
                                        bsls_PlatformUtil::Int64  swapValue);
        // Conditionally set the value of the specified 'atomicInt' to the
        // specified 'swapValue' if and only if the value of 'atomicInt' equals
        // the value of the specified 'compareValue', and return the initial
        // value of 'atomicInt'.  The whole operation is performed atomically.
        // The behavior is undefined if 'atomicInt' is 0.

    static bsls_PlatformUtil::Int64 addInt64Nv(
                                            bces_AtomicUtil::Int64  *atomicInt,
                                            bsls_PlatformUtil::Int64 value);
        // Atomically add to the specified 'atomicInt' the specified 'value'
        // and return the resulting value.  The behavior is undefined if
        // 'atomicInt' is 0.

    static bsls_PlatformUtil::Int64 addInt64NvRelaxed(
                                            bces_AtomicUtil::Int64  *atomicInt,
                                            bsls_PlatformUtil::Int64 value);
        // Atomically add to the specified 'atomicInt' the specified 'value'
        // and return the resulting value, without additional ordering
        // constraints.  The behavior is undefined if 'atomicInt' is 0.

    static bsls_PlatformUtil::Int64 incrementInt64Nv(
                                            bces_AtomicUtil::Int64 *atomicInt);
        // Atomically increment the specified 'atomicInt' by 1 and return the
        // resulting value.  The behavior is undefined if 'atomicInt' is 0.

    static bsls_PlatformUtil::Int64 decrementInt64Nv(
                                            bces_AtomicUtil::Int64 *atomicInt);
        // Atomically decrement the specified 'atomicInt' by 1 and return the
        // resulting value.  The behavior is undefined if 'atomicInt' is 0.

    static void setInt64(bces_AtomicUtil::Int64   *atomicInt,
                         bsls_PlatformUtil::Int64  val);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value'.  The behavior is undefined if 'atomicInt' is 0.

    static bsls_PlatformUtil::Int64 getInt64(
                                      const bces_AtomicUtil::Int64& atomicInt);
        // Atomically retrieve the value of the specified 'atomicInt'.

    static void setInt64Relaxed(bces_AtomicUtil::Int64   *atomicInt,
                                bsls_PlatformUtil::Int64  val);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value' without additional ordering constraints.  The
        // behavior is undefined if 'atomicInt' is 0.

    static bsls_PlatformUtil::Int64 getInt64Relaxed(
                                      const bces_AtomicUtil::Int64& atomicInt);
        // Atomically retrieve the value of the specified 'atomicInt' without
        // additional ordering constraints.

    static void *getPtr(const bces_AtomicUtil::Pointer& atomicPtr);
        // Atomically retrieve the value of the specified 'atomicPtr'.

    static void *getPtrRelaxed(const bces_AtomicUtil::Pointer& atomicPtr);
        // Atomically retrieve the value of the specified 'atomicPtr', without
        // additional ordering constraints.

    static void setPtr(bces_AtomicUtil::Pointer *atomicPtr,
                       const volatile void      *value);
        // Atomically set the value of the specified 'atomicPtr' to the
        // specified 'value'.  The behavior is undefined if 'atomicPtr' is 0.

    static void setPtrRelaxed(bces_AtomicUtil::Pointer *atomicPtr,
                              const volatile void      *value);
        // Atomically set the value of the specified 'atomicPtr' to the
        // specified 'value', without additional ordering constraints.

    static void *swapPtr(bces_AtomicUtil::Pointer *atomicPtr,
                         const volatile void      *swapValue);
        // Atomically set the value of the specified 'atomicPtr' to the
        // specified 'value', and return its previous value.  The behavior is
        // undefined if 'atomicPtr' is 0.

    static void *testAndSwapPtr(bces_AtomicUtil::Pointer *atomicPtr,
                                const volatile void      *compareValue,
                                const volatile void      *swapValue);
        // Conditionally set the value of the specified 'atomicPtr' to the
        // specified 'swapValue' if and only if the value of 'atomicPtr' equals
        // the value of the specified 'compareValue', and return the initial
        // value of 'atomicPtr'.  The whole operation is performed atomically.
        // The behavior is undefined if 'atomicPtr' is 0.

    static void spinLock(bces_AtomicUtil::SpinLock *spinlock);
        // Lock the specified 'spinlock'.  If the specified 'spinlock' has
        // already been locked, test repeatedly until the 'spinlock' becomes
        // unlocked.  The behavior is undefined if 'spinlock' is 0.

    static int spinTryLock(bces_AtomicUtil::SpinLock *spinlock, int retries);
        // Attempt to lock the specified 'spinlock' and, if the attempt is
        // unsucessful, retry up to the specified 'numRetries' times.  Return
        // 0 on success, and a non-zero value if the lock was not successfuly
        // aquired.  The behavior is undefined unless 'spinLock' is a valid
        // address to a 'SpinLock' and '0 <= retries'.

    static void spinUnlock(bces_AtomicUtil::SpinLock *spinlock);
        // Unlock the specified 'spinlock', which was previously locked by a
        // successful call to 'spinLock' or 'spinTryLock'.  The behavior is
        // undefined if 'spinlock' is not locked, if 'spinLock' was locked by a
        // different thread, or if 'spinLock' is 0.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

inline
void bces_AtomicUtil::initInt(bces_AtomicUtil::Int *atomicInt,
                              int                   initialValue)
{
    Impl::initInt(atomicInt, initialValue);
}

inline
void bces_AtomicUtil::initInt64(bces_AtomicUtil::Int64   *atomicInt,
                                bsls_PlatformUtil::Int64  initialValue)
{
    Impl::initInt64(atomicInt,initialValue);
}

inline
void bces_AtomicUtil::initPointer(bces_AtomicUtil::Pointer *atomicPtr,
                                  const volatile void      *initialValue)
{
    Impl::initPointer(atomicPtr, const_cast<const void *>(initialValue));
}

inline
void bces_AtomicUtil::initSpinLock(bces_AtomicUtil::SpinLock *spinlock)
{
    Impl::initSpinLock(spinlock);
}

inline
void bces_AtomicUtil::addInt(bces_AtomicUtil::Int *atomicInt, int value)
{
    Impl::addInt(atomicInt, value);
}

inline
void bces_AtomicUtil::incrementInt(bces_AtomicUtil::Int *atomicInt)
{
    Impl::incrementInt(atomicInt);
}

inline
void bces_AtomicUtil::decrementInt(bces_AtomicUtil::Int *atomicInt)
{
    Impl::decrementInt(atomicInt);
}

inline
int bces_AtomicUtil::swapInt(bces_AtomicUtil::Int *atomicInt, int value)
{
    return Impl::swapInt(atomicInt, value);
}

inline
int bces_AtomicUtil::testAndSwapInt(bces_AtomicUtil::Int *atomicInt,
                                    int                   compareValue,
                                    int                   swapValue)
{
    return Impl::testAndSwapInt(atomicInt, compareValue, swapValue);
}

inline
void bces_AtomicUtil::setInt(bces_AtomicUtil::Int *atomicInt, int value)
{
    Impl::setInt(atomicInt, value);
}

inline
void bces_AtomicUtil::setIntRelaxed(bces_AtomicUtil::Int *atomicInt, int value)
{
    Impl::setIntRelaxed(atomicInt, value);
}

inline
int bces_AtomicUtil::getInt(const bces_AtomicUtil::Int& atomicInt)
{
    return Impl::getInt(atomicInt);
}

inline
int bces_AtomicUtil::getIntRelaxed(const bces_AtomicUtil::Int& atomicInt)
{
    return Impl::getIntRelaxed(atomicInt);
}

inline
int bces_AtomicUtil::addIntNv(bces_AtomicUtil::Int *atomicInt, int value)
{
    return Impl::addIntNv(atomicInt, value);
}

inline
int bces_AtomicUtil::addIntNvRelaxed(bces_AtomicUtil::Int *atomicInt,
                                     int value)
{
    return Impl::addIntNvRelaxed(atomicInt, value);
}

inline
int bces_AtomicUtil::incrementIntNv(bces_AtomicUtil::Int *atomicInt)
{
    return Impl::incrementIntNv(atomicInt);
}

inline
int bces_AtomicUtil::decrementIntNv(bces_AtomicUtil::Int *atomicInt)
{
    return Impl::decrementIntNv(atomicInt);
}

inline
void bces_AtomicUtil::addInt64(bces_AtomicUtil::Int64   *atomicInt,
                               bsls_PlatformUtil::Int64  value)
{
    Impl::addInt64(atomicInt, value);
}

inline
void bces_AtomicUtil::incrementInt64(bces_AtomicUtil::Int64 *atomicInt)
{
    Impl::incrementInt64(atomicInt);
}

inline
void bces_AtomicUtil::decrementInt64(bces_AtomicUtil::Int64 *atomicInt)
{
    Impl::decrementInt64(atomicInt);
}

inline
bsls_PlatformUtil::Int64 bces_AtomicUtil::swapInt64(
                                           bces_AtomicUtil::Int64   *atomicInt,
                                           bsls_PlatformUtil::Int64  value)
{
    return Impl::swapInt64(atomicInt, value);
}

inline
bsls_PlatformUtil::Int64 bces_AtomicUtil::testAndSwapInt64(
                                        bces_AtomicUtil::Int64   *atomicInt,
                                        bsls_PlatformUtil::Int64  compareValue,
                                        bsls_PlatformUtil::Int64  swapValue)
{
    return Impl::testAndSwapInt64(atomicInt, compareValue, swapValue);
}

inline
bsls_PlatformUtil::Int64 bces_AtomicUtil::addInt64Nv(
                                           bces_AtomicUtil::Int64   *atomicInt,
                                           bsls_PlatformUtil::Int64  value)
{
    return Impl::addInt64Nv(atomicInt,value);
}

inline
bsls_PlatformUtil::Int64 bces_AtomicUtil::addInt64NvRelaxed(
                                           bces_AtomicUtil::Int64   *atomicInt,
                                           bsls_PlatformUtil::Int64  value)
{
    return Impl::addInt64NvRelaxed(atomicInt,value);
}

inline
bsls_PlatformUtil::Int64 bces_AtomicUtil::incrementInt64Nv(
                                             bces_AtomicUtil::Int64 *atomicInt)
{
    return Impl::incrementInt64Nv(atomicInt);
}

inline
bsls_PlatformUtil::Int64 bces_AtomicUtil::decrementInt64Nv(
                                             bces_AtomicUtil::Int64 *atomicInt)
{
    return Impl::decrementInt64Nv(atomicInt);
}

inline
void bces_AtomicUtil::setInt64(bces_AtomicUtil::Int64 *atomicInt,
                               bsls_PlatformUtil::Int64 value)
{
    Impl::setInt64(atomicInt, value);
}

inline
void bces_AtomicUtil::setInt64Relaxed(bces_AtomicUtil::Int64 *atomicInt,
                                      bsls_PlatformUtil::Int64 value)
{
    Impl::setInt64Relaxed(atomicInt, value);
}

inline
bsls_PlatformUtil::Int64 bces_AtomicUtil::getInt64(
                                       const bces_AtomicUtil::Int64& atomicInt)
{
    return Impl::getInt64(atomicInt);
}

inline
bsls_PlatformUtil::Int64 bces_AtomicUtil::getInt64Relaxed(
                                       const bces_AtomicUtil::Int64& atomicInt)
{
    return Impl::getInt64Relaxed(atomicInt);
}

inline
void *bces_AtomicUtil::getPtr(const bces_AtomicUtil::Pointer& atomicPtr)
{
    return Impl::getPtr(atomicPtr);
}

inline
void *bces_AtomicUtil::getPtrRelaxed(const bces_AtomicUtil::Pointer& atomicPtr)
{
    return Impl::getPtrRelaxed(atomicPtr);
}

inline
void bces_AtomicUtil::setPtr(bces_AtomicUtil::Pointer *atomicPtr,
                             const volatile void      *value)
{
    Impl::setPtr(atomicPtr, const_cast<void*>(value));
}

inline
void bces_AtomicUtil::setPtrRelaxed(bces_AtomicUtil::Pointer *atomicPtr,
                                    const volatile void      *value)
{
    Impl::setPtrRelaxed(atomicPtr, const_cast<void*>(value));
}

inline
void *bces_AtomicUtil::swapPtr(bces_AtomicUtil::Pointer *atomicPtr,
                               const volatile void      *swapValue)
{
    return Impl::swapPtr(atomicPtr, const_cast<void*>(swapValue));
}

inline
void *bces_AtomicUtil::testAndSwapPtr(bces_AtomicUtil::Pointer *atomicPtr,
                                      const volatile void      *compareValue,
                                      const volatile void      *swapValue)
{
    return Impl::testAndSwapPtr(atomicPtr,
                                const_cast<void*>(compareValue),
                                const_cast<void*>(swapValue));
}

inline
void bces_AtomicUtil::spinLock(bces_AtomicUtil::SpinLock *spinlock)
{
    Impl::spinLock(spinlock);
}

inline
int bces_AtomicUtil::spinTryLock(bces_AtomicUtil::SpinLock *spinlock,
                                 int                         retries)
{
    return Impl::spinTryLock(spinlock, retries);
}

inline
void bces_AtomicUtil::spinUnlock(bces_AtomicUtil::SpinLock *spinlock)
{
    Impl::spinUnlock(spinlock);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
