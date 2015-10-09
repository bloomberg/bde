// bsls_atomicoperations.h                                            -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#define INCLUDED_BSLS_ATOMICOPERATIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide platform-independent atomic operations.
//
//@CLASSES:
//  bsls::AtomicOperations: namespace for atomic operations
//
//@DESCRIPTION: This utility provides a set of platform-independent atomic
// operations for fundamental data types, such as 32-bit and 64-bit integer and
// pointer.  The examples of provided atomic operations include loading,
// storing, exchanging, incrementing and decrementing the data of fundamental
// types.  Atomic operations are useful for manipulating certain types of
// shared data without the need for high level synchronization mechanisms
// (e.g., "mutexes" or "critical sections").
//
// Integer atomic operations allow for thread-safe manipulation of a single 32
// or 64-bit integer value, without the use of other synchronization
// mechanisms.  Even the most basic operations on data that is shared among
// multiple threads must use some form of synchronization to ensure proper
// results.
//
// Consider the prefix increment from the following snippet of C code:
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
// Consider the situation when the above statements were executed
// simultaneously by two threads.  Thread A could load 'x' to a register, and
// then be interrupted by the operating system.  Thread B could then begin to
// execute, and complete all three instructions, loading, incrementing, and
// storing the variable 'x'.  When thread A resumes, it would increment the
// value that it loaded, and store the result.
//
// Thus, it is possible that both threads load the same value of x to the
// register, add one, and store their individual but equal results, incorrectly
// incrementing 'x' by only 1 instead of the correct value 2.  One could
// correct this problem by using a high level synchronization mechanisms (e.g.,
// "mutex"), but these mechanisms are generally very expensive for such a small
// fragment of code, and could result in a large number of unnecessary context
// switches, for instance, if the increment statement occurs within a loop.
//
// Instead, an atomic operation (in this case,
// 'bsls::AtomicOperations::incrementInt') can be used to manipulate the value;
// use of this operation will ensure that, when executed simultaneously by
// multiple threads, the threads will increment the value serially, without
// interrupting one another.
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
// inherently thread-safe and no synchronization is required; although they are
// typically much faster than using high-level synchronization mechanisms to
// accomplish the same results, atomic operations are typically more expensive
// (in both speed and code size) than their non-atomic equivalents.
//
///Memory Order and Consistency Guarantees of Atomic Operations
///------------------------------------------------------------
// Atomic operations provided by this component ensure various memory ordering
// consistency guarantees.  Memory ordering guarantees of atomic operations are
// designed to ensure visibility and synchronization order of memory reads and
// writes between threads that perform atomic operations.  The operations on
// objects of the provided classes ensure the most strict consistency
// guarantee, sequential consistency (described below), unless explicitly
// qualified with a less strict consistency guarantee (i.e., Acquire, Release,
// Acquire/Release or Relaxed).
//
// This component implements memory order and consistency guarantees as defined
// in the C++ 2011 Standard (sections: [intro.multithreaded], [atomics.order]).
//
// The following memory ordering guarantees are supported:
//:
//: o relaxed - the operation does not provide any memory consistency guarantee
//:
//: o release - the operation performs a release operation on the affected
//:   memory location, thus making preceding regular memory writes of the
//:   calling thread visible to other threads through the atomic variable to
//:   which it is applied (generally available for operations that write to a
//:   memory location).
//:
//: o acquire - the operation performs an acquire operation on the affected
//:   memory location, thus making regular memory writes in other threads
//:   released through the atomic variable to which it is applied visible to
//:   the current thread (generally available for operations that read from a
//:   memory location).
//:
//: o acquire/release - the operation has both acquire and release semantics
//:   (generally available for operations that both read and write a memory
//:   location).
//:
//: o sequential consistency - the operation has both acquire and release
//:   guarantees, and further guarantees that all sequentially consistent
//:   operations performed by the process will be observed to occur in a
//:   single global total order (regardless of the thread from which they are
//:   observed).
//
///Acquire and Release Memory Consistency Guarantees
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Operations providing acquire and release guarantees are essential to
// synchronizing the memory state between multiple threads.  For example,
// consider two threads, A and B, that perform store and load operations to
// shared memory locations.  Without any synchronization, store operations in
// thread A can be freely reordered with load operations in thread B, i.e,
// thread A can perform two store operations to two memory locations in a
// certain order and thread B can see those operations done in a different
// order due to such effects as: compiler or processor optimizations of store
// and load operations, and cache synchronization between processors and cores.
//
// However, stores in thread A can be ordered with loads in thread B using a
// combination of store-release and load-acquire operations.  A store-release
// operation in thread A followed by a load-acquire operation in thread B
// to the *same* *memory* *location* guarantees that thread B sees all other
// stores done in thread A prior to the store-release operation.  The
// store-release in thread A effectively synchronizes the memory state with the
// load-acquire in thread B.
//
// An acquire-release operation is a load-modify-store operation that, if
// performed in both threads A and B on the same memory location, synchronizes
// stores and loads between threads A and B in both directions.
//
///Sequential Consistency Memory Consistency Guarantee
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// Finally, load and store operations with sequential consistency are
// guaranteed to performed in a global total order among all threads in the
// process.  To illustrate the total order, let's consider the so-called
// "independent reads of independent writes" example:
//..
//  bsls::AtomicInt x(0);
//  bsls::AtomicInt y(0);
//  int r1, r2, r3, r4;
//
//  void thread1() {
//      x = 1;  // sequential consistency store
//  }
//
//  void thread2() {
//      y = 1;  // sequential consistency store
//  }
//
//  void thread3() {
//      r1 = x;  // sequential consistency load
//      r2 = y;  // sequential consistency load
//  }
//
//  void thread4() {
//      r3 = y;  // sequential consistency load
//      r4 = x;  // sequential consistency load
//  }
//..
// Where 'threadN' functions are executed concurrently by different threads
// (note that values 'x' and 'y' are written by independent threads).
// Sequential consistency guarantees that if 'thread3' observes values 'x' and
// 'y' as 'r1 == 1 && r2 == 0', then 'thread4' can't observe values 'x' and
// 'y' in a different order, i.e., 'r3 == 1 && r4 == 0'.
//
///Atomic Integer Operations
///-------------------------
// The atomic integer operations provide thread-safe access for 32- or 64-bit
// signed integer numbers without the use of higher level synchronization
// mechanisms.  Atomic integers are most commonly used to manipulate shared
// counters and indices.  Five types of operations are provided; get/set,
// increment/decrement, add, swap, and test and swap.  Two sub-types of
// manipulators are provided for increment/decrement and addition operations.
//
// 'bsls::AtomicOperations' functions whose names end in "Nv" (stands for "new
// value"; e.g., 'addIntNv', 'incrementInt64Nv') return the resulting value of
// the operations; those without the suffix do not return a value.  If an
// application does not require the resulting value of an operation, it should
// not use the "Nv" manipulator.  On some platforms, it may be less efficient
// to determine the resulting value of an operation than to simply perform the
// operation.
//
///Atomic Pointer Operations
///-------------------------
// The atomic pointer operations provide thread-safe access to pointer values
// without the use of higher level synchronization mechanisms.  They are
// commonly used to create fast thread-safe singly-linked lists.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Usage Statistics on a Thread Pool
/// - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates a common use of atomic integer types for
// statistics counters.  The program creates a series of threads to process
// transactions.  As each thread completes a transaction, it atomically
// increments the transaction counters.
//
// For this example, we assume the existence of the functions
// 'processNextTransaction', 'createWorkerThread', and 'waitAllThreads'.  The
// function 'createWorkerThread' spawns a new thread, which executes the
// 'workerThread' function.  'waitAllThreads' blocks until all the worker
// thread complete.
//
// First, we declare the shared counters:
//..
// static bsls::AtomicOperations::AtomicTypes::Int64 transactionCount;
// static bsls::AtomicOperations::AtomicTypes::Int64 successCount;
// static bsls::AtomicOperations::AtomicTypes::Int64 failureCount;
//..
// Next, for each transaction processed, we atomically increment either the
// success or the failure counter as well as the total transaction count:
//..
//  static void workerThread(int *stop)
//  {
//      while (!(*stop)) {
//          if (processNextTransaction()) {
//              bsls::AtomicOperations::incrementInt64(&failureCount);
//          }
//          else {
//              bsls::AtomicOperations::incrementInt64(&successCount);
//          }
//          bsls::AtomicOperations::incrementInt64(&transactionCount);
//      }
//  }
//..
// Finally, we write function, 'serverMain', that provides the overall control
// logic for the server.  This function spawns the threads and then waits for
// all work to be completed; when all of the threads have finished, this
// function returns normally:
//..
//  void serverMain()
//  {
//      const int numThreads = 10;
//..
// Before any of the counters is used, they must be initialized.  'initInt64'
// is called to initialize each value to 0:
//..
//      bsls::AtomicOperations::initInt64(&transactionCount, 0);
//      bsls::AtomicOperations::initInt64(&successCount, 0);
//      bsls::AtomicOperations::initInt64(&failureCount, 0);
//..
// Spawn the threads to process the transactions concurrently:
//..
//      for (int i = 0; i < numThreads; ++i) {
//          createWorkerThread();
//      }
//..
// Wait for all the threads to complete:
//..
//      waitAllThreads();
//  }
//..
// Note that functions 'createWorkerThread' and 'waitAllThreads' can be
// implemented using any thread-support package.
//
///Example 2: Thread-Safe Counted Handle
///- - - - - - - - - - - - - - - - - - -
// The following example demonstrates the use of atomic integer operations to
// implement a thread-safe ref-counted handle similar to a shared pointer.
// Each handle (of type 'my_CountedHandle') maintains a pointer to a
// representation object, 'my_CountedHandleRep', which in turn, stores both a
// pointer to the managed object and a reference counter.
//
// Both the handle class and the representation class are template classes with
// two template parameters.  The template parameter, 'INSTANCE', represents the
// type of the "instance", or managed object.
//
// A representation object can be shared by several handle objects.  When a
// handle object is assigned to a second handle object, the address of the
// representation is copied to the second handle, and the reference count on
// the representation is atomically incremented.  When a handle releases its
// reference to the representation, it atomically decrements the reference
// count.  If the resulting reference count becomes 0 (and there are no more
// references to the object), the handle deletes the representation object and
// the representation object, in turn, deletes the managed object ('INSTANCE').
//
///Class 'my_CountedHandleRep'
///- - - - - - - - - - - - - -
// First, we define class 'my_CountedHandleRep'.  This class manages a single
// 'INSTANCE' object on behalf of multiple "handle" objects; since different
// "handle" objects may be active in different threads, class
// 'my_CountedHandleRep' must be (fully) thread-safe.  Specifically, methods
// 'increment' and 'decrement' must work atomically.
//
// Note that, this rep class is intended to be used only by class
// 'my_CountedHandle', and thus all methods of class 'my_CountedHandleRep' are
// declared private, and 'friend' status is granted to class
// 'my_CountedHandle':
//..
//                          // =========================
//                          // class my_CountedHandleRep
//                          // =========================
//
//  template <class INSTANCE>
//  class my_CountedHandle;
//
//  template <class INSTANCE>
//  class my_CountedHandleRep {
//
//      // DATA
//      bsls::AtomicOperations::AtomicTypes::Int
//                           d_count;        // number of active references
//      INSTANCE            *d_instance_p;   // address of managed instance
//
//      // FRIENDS
//      friend class my_CountedHandle<INSTANCE>;
//
//    private: // not implemented
//      my_CountedHandleRep(const my_CountedHandleRep&);
//      my_CountedHandleRep& operator=(const my_CountedHandleRep&);
//
//    public:
//      // CLASS METHODS
//      static void
//      deleteObject(my_CountedHandleRep<INSTANCE> *object);
//
//      // CREATORS
//      my_CountedHandleRep(INSTANCE *instance);
//      ~my_CountedHandleRep();
//
//      // MANIPULATORS
//      void increment();
//      int decrement();
//  };
//..
//
///Class 'my_CountedHandle'
///-  -  -  -  -  -  -  - -
// Then, we create class 'my_CountedHandle' that provides an individual handle
// to the shared, reference-counted object.  Each 'my_CountedHandle' object
// acts as a smart pointer, supplying an overloaded 'operator->' that provides
// access to the underlying 'INSTANCE' object via pointer semantics.
//
// 'my_CountedHandle' can also be copied freely; the copy constructor will use
// the 'increment' method from 'my_CountedHandleRep' to note the extra copy.
// Similarly, the destructor will call 'my_CountedHandleRep::decrement' to note
// that there is one fewer handle the underlying 'INSTANCE' has, and delete the
// "rep" object when its reference count is reduced to zero:
//..
//                          // ======================
//                          // class my_CountedHandle
//                          // ======================
//
//  template <class INSTANCE>
//  class my_CountedHandle {
//
//      // DATA
//      my_CountedHandleRep<INSTANCE> *d_rep_p;  // shared rep.
//
//    public:
//      // CREATORS
//      my_CountedHandle(INSTANCE *instance);
//
//      my_CountedHandle(const my_CountedHandle<INSTANCE>& other);
//
//      ~my_CountedHandle();
//
//      // ACCESSORS
//      INSTANCE *operator->() const;
//      int numReferences() const;
//  };
//..
//
///Function Definitions for 'my_CountedHandleRep'
///-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// Next, we provide a definition for the 'static' 'deleteObject' method, which
// is called by the destructor for class 'my_CountedHandle' for the last
// instance of 'my_CountedHandle' using the given "rep" object:
//..
//  template <class INSTANCE>
//  inline
//  void my_CountedHandleRep<INSTANCE>::deleteObject(
//                               my_CountedHandleRep<INSTANCE> *object)
//  {
//      delete object;
//  }
//..
// Then, we write the constructor for the 'my_CountedHandleRep<INSTANCE>'
// class.  We initialize the atomic reference counter to one reference using
// 'bsls::AtomicOperations::initInt'.  This reflects the fact that this
// constructor will be called by a new instance of 'my_CountedHandle'.  That
// instance is our first and only handle when this constructor is called:
//..
//  template <class INSTANCE>
//  inline
//  my_CountedHandleRep<INSTANCE>::
//                          my_CountedHandleRep(INSTANCE        *instance)
//  : d_instance_p(instance)
//  {
//      bsls::AtomicOperations::initInt(&d_count, 1);
//  }
//..
// Then, we define the destructor, which just deletes 'my_CountedHandle'
// 'd_instance_p':
//..
//  template <class INSTANCE>
//  inline
//  my_CountedHandleRep<INSTANCE>::~my_CountedHandleRep()
//  {
//      delete d_instance_p;
//  }
//..
// Next, we define method 'increment', which atomically increments the number
// of references to this 'my_CountedHandleRep'.  Since our caller is not
// interested in the result (and our return type is thus 'void'), we use
// 'incrementInt' instead of 'incrementIntNv'.
//..
//  // MANIPULATORS
//  template <class INSTANCE>
//  inline
//  void my_CountedHandleRep<INSTANCE>::increment()
//  {
//      bsls::AtomicOperations::incrementInt(&d_count);
//  }
//..
// Then, we implement method 'decrement', which atomically decrements the
// reference count; since our caller will need to check the resulting value to
// determine whether the 'INSTANCE' should be deleted, we use 'decrementIntNv'
// rather than 'decrementInt', and return the new number of references:
//..
//  template <class INSTANCE>
//  inline
//  int my_CountedHandleRep<INSTANCE>::decrement()
//  {
//      return bsls::AtomicOperations::decrementIntNv(&d_count);
//  }
//..
//
///Function Definitions for 'my_CountedHandle'
///-  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// Next, we define the first constructor for 'my_CountedHandle', which is used
// when creating a handle for a new 'INSTANCE'; note that the 'INSTANCE' is
// constructed separately, and a pointer to that object is passed as the first
// argument ('object'):
//..
//                          // ----------------------
//                          // class my_CountedHandle
//                          // ----------------------
//
//  template <class INSTANCE>
//  inline
//  my_CountedHandle<INSTANCE>::my_CountedHandle(INSTANCE *instance)
//  {
//      d_rep_p = new my_CountedHandleRep<INSTANCE>(instance);
//  }
//..
// Then, we define the copy constructor; the new object copies the underlying
// 'my_CountedHandleRep' and then increments its counter:
//..
//  template <class INSTANCE>
//  inline
//  my_CountedHandle<INSTANCE>::my_CountedHandle(
//                                     const my_CountedHandle<INSTANCE>& other)
//  : d_rep_p(other.d_rep_p)
//  {
//      if (d_rep_p) {
//          d_rep_p->increment();
//      }
//  }
//..
// Next, we define the destructor which decrements the "rep" object's reference
// count using the 'decrement' method.  The 'decrement' method returns the
// object's reference count after the decrement is completed, and
// 'my_CountedHandle' uses this value to determine whether the "rep" object
// should be deleted:
//..
//  template <class INSTANCE>
//  inline
//  my_CountedHandle<INSTANCE>::~my_CountedHandle()
//  {
//      if (d_rep_p && 0 == d_rep_p->decrement()) {
//          my_CountedHandleRep<INSTANCE>::deleteObject(d_rep_p);
//      }
//  }
//..
// Now, we define member 'operator->()', which provides basic pointer semantics
// for 'my_CountedHandle':
//..
//  template <class INSTANCE>
//  inline
//  INSTANCE *my_CountedHandle<INSTANCE>::operator->() const
//  {
//      return d_rep_p->d_instance_p;
//  }
//..
// Finally, we define method 'numReferences', which returns the value of the
// reference counter:
//..
//  template <class INSTANCE>
//  inline
//  int my_CountedHandle<INSTANCE>::numReferences() const
//  {
//      return d_rep_p ? bsls::AtomicOperations::getInt(d_rep_p->d_count) : 0;
//  }
//..
// Note that, while class 'my_CountedHandleRep' is itself fully thread-safe, it
// does not guarantee thread safety for the 'INSTANCE' object.  In order to
// provide thread safety for the 'INSTANCE' in the general case, the "rep"
// would need to use a more general concurrency mechanism such as a mutex.
//
///Example 3: Thread-Safe Lock-Free Singly-Linked List
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the use of atomic pointers to implement a fast and
// thread-aware, yet fast single-linked list.  The example class,
// 'my_PtrStack', is a templatized pointer stack, supporting 'push' and 'pop'
// methods.  The class is implemented using a single-linked list.  Nodes in the
// list are linked together using atomic operations.  Instance of this
// structure are allocated using the provided allocator.  When nodes are freed,
// they are cached on a free list.  This free list is also implemented as a
// single-linked list, using atomic pointer operations.
//
// First, we create class template, 'my_PtrStack', parameterized by 'TYPE'.
// Instances of this template maintain a list of nodes and a free-node list.
// Each node has a pointer to a data item, 'd_item_p', a link to the next node
// in the list, 'd_next_p'.  The definition of the 'my_PtrStack' class is
// provided below:
//..
//  template <class TYPE>
//  class my_PtrStack {
//      // TYPES
//      struct Node {
//          TYPE *d_item_p;
//          Node *d_next_p;
//      };
//
//      // DATA
//      bsls::AtomicOperations::AtomicTypes::Pointer  d_list_p;
//      bsls::AtomicOperations::AtomicTypes::Pointer  d_freeList_p;
//
//      // PRIVATE MANIPULATORS
//      Node *allocateNode();
//      void freeNode(Node *node);
//      void deleteNodes(Node *node);
//
//    public:
//      // CREATORS
//      my_PtrStack();
//     ~my_PtrStack();
//
//      // MANIPULATORS
//      void push(TYPE *item);
//      TYPE *pop();
//  };
//..
// Then, we write the constructor that initializes the pointers for the node
// list and the free list:
//..
//  // CREATORS
//  template <class TYPE>
//  inline my_PtrStack<TYPE>::my_PtrStack()
//  {
//      bsls::AtomicOperations::initPointer(&d_freeList_p, 0);
//      bsls::AtomicOperations::initPointer(&d_list_p, 0);
//  }
//..
// Next, we define the 'deleteNodes' and the destructor function to delete
// nodes that the 'my_PtrStack' object owns.  Note that we don't need to worry
// about the concurrent access to node lists in the destructor, as destructor
// can be executed in only a single thread:
//..
//  template <class TYPE>
//  inline void my_PtrStack<TYPE>::deleteNodes(Node *node)
//  {
//      while (node) {
//          Node *next = node->d_next_p;
//          delete node;
//          node = next;
//      }
//  }
//
//  template <class TYPE>
//  inline my_PtrStack<TYPE>::~my_PtrStack()
//  {
//      deleteNodes(
//         (Node *) bsls::AtomicOperations::getPtrRelaxed(&d_list_p));
//      deleteNodes(
//         (Node *) bsls::AtomicOperations::getPtrRelaxed(&d_freeList_p));
//  }
//..
// Then, we define method 'allocateNode' to get a node from the free list in
// the thread-safe manner by leveraging atomic operations to ensure proper
// thread synchronization:
//..
//  // PRIVATE MANIPULATORS
//  template <class TYPE>
//  inline typename my_PtrStack<TYPE>::Node *my_PtrStack<TYPE>::allocateNode()
//  {
//      Node *node;
//..
// To remove an item from this list, get the current list head using 'getPtr'.
// Then, test and swap it with the next node.  'testAndSwapPtr' compares
// 'd_freeList_p' to 'node', replacing it with 'node->d_next_p' only if it
// matches.  If 'd_freeList_p' did not match 'node', then the free list has
// been changed on another thread, between the calls to 'getPtr' and
// 'testAndSwapPtr'.  If the list head has changed, then try again:
//..
//      do {
//          node = (Node*) bsls::AtomicOperations::getPtr(&d_freeList_p);
//          if (!node) break;
//      } while (bsls::AtomicOperations::testAndSwapPtr(
//                                                    &d_freeList_p,
//                                                    node,
//                                                    node->d_next_p) != node);
//..
// Next, we allocate a new node if there are no nodes in the free node list:
//..
//      if (!node) {
//          node = new Node();
//      }
//      return node;
//  }
//..
// Then, we provide the 'freeNode' method to add a given 'node' to the free
// list.  To add the node to the list, we set the next pointer of the new node
// to the current value of the list head, and atomically test and swap the head
// of the list with the new node.  If the list head has been changed (by
// another thread), we try again:
//..
//  template <class TYPE>
//  inline void my_PtrStack<TYPE>::freeNode(Node *node)
//  {
//      do {
//        node->d_next_p = (Node*) bsls::AtomicOperations::getPtr(
//                                                              &d_freeList_p);
//      } while (bsls::AtomicOperations::testAndSwapPtr(
//                                                    &d_freeList_p,
//                                                    node->d_next_p,
//                                                    node) != node->d_next_p);
//  }
//..
// Now, we begin to define the public "stack-like" interface for 'my_PtrStack'.
// Note that the 'push' method is similar to 'freeNode', except that it assigns
// an item value and operates on 'd_list_p', which maintains the list of active
// nodes:
//..
//  template <class TYPE>
//  inline void my_PtrStack<TYPE>::push(TYPE *item)
//  {
//      Node *node = allocateNode();
//      node->d_item_p = item;
//      do {
//          node->d_next_p = (Node*) bsls::AtomicOperations::getPtr(&d_list_p);
//      } while (bsls::AtomicOperations::testAndSwapPtr(
//                                                    &d_list_p,
//                                                    node->d_next_p,
//                                                    node)!= node->d_next_p);
//  }
//..
// Finally, we define the 'pop' method which removes the node from the top
// of active node list, 'd_list_p', adds it to the free-node list, and returns
// the data item contained in the node to the caller:
//..
//  template <class TYPE>
//  inline TYPE *my_PtrStack<TYPE>::pop()
//  {
//      Node *node;
//      do {
//          node = (Node*) bsls::AtomicOperations::getPtr(&d_list_p);
//          if (!node) break;
//      } while (bsls::AtomicOperations::testAndSwapPtr(
//                                                     &d_freeList_p,
//                                                     node,
//                                                     node->d_next_p)!= node);
//      TYPE *item = node ? node->d_item_p : 0;
//      if (node)
//          freeNode(node);
//      return item;
//  }
//..
// Notice that if the stack was empty, a NULL pointer is returned.

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#if defined(BSLS_PLATFORM_CMP_CLANG)
#if __has_extension(c_atomic) || __has_extension(cxx_atomic)  // clang 3.1+
#define BSLS_ATOMICOPERATIONS_CLANG_ATOMICS
#endif
#endif

#if defined(BSLS_ATOMICOPERATIONS_CLANG_ATOMICS)
    //  clang 3.1+
#   include <bsls_atomicoperations_all_all_clangintrinsics.h>

#elif defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION >= 40700
    //  GCC 4.7+
#   include <bsls_atomicoperations_all_all_gccintrinsics.h>

#elif defined(BSLS_PLATFORM_CPU_X86)

#   if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
#       include <bsls_atomicoperations_x86_all_gcc.h>
#   elif defined(BSLS_PLATFORM_CMP_MSVC)
#       include <bsls_atomicoperations_x86_win_msvc.h>
#   else
#       define BSLS_ATOMICOPERATIONS_ERROR
#   endif

#elif defined(BSLS_PLATFORM_CPU_X86_64)

#   if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
#       include <bsls_atomicoperations_x64_all_gcc.h>
#   elif defined(BSLS_PLATFORM_CMP_MSVC)
#       include <bsls_atomicoperations_x64_win_msvc.h>
#   else
#       define BSLS_ATOMICOPERATIONS_ERROR
#   endif

#elif defined(BSLS_PLATFORM_CPU_POWERPC)

#   if defined(BSLS_PLATFORM_CMP_IBM)
#       if defined(BSLS_PLATFORM_CPU_64_BIT)
#           include <bsls_atomicoperations_powerpc64_aix_xlc.h>
#       else
#           include <bsls_atomicoperations_powerpc32_aix_xlc.h>
#       endif
#   elif defined(BSLS_PLATFORM_CMP_GNU)
#       include <bsls_atomicoperations_powerpc_all_gcc.h>
#   else
#       define BSLS_ATOMICOPERATIONS_ERROR
#   endif

#elif defined(BSLS_PLATFORM_CPU_SPARC_32) \
      && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_SUN))
#   include <bsls_atomicoperations_sparc32_sun_cc.h>
#elif defined(BSLS_PLATFORM_CPU_SPARC_V9) \
      && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_SUN))
#   include <bsls_atomicoperations_sparc64_sun_cc.h>
#elif defined(BSLS_PLATFORM_CPU_IA64) && defined(BSLS_PLATFORM_OS_HPUX)
#   include <bsls_atomicoperations_ia64_hp_acc.h>

#elif defined(BSLS_PLATFORM_CPU_ARM)
#   if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
#       include <bsls_atomicoperations_arm_all_gcc.h>
#   else
#       define BSLS_ATOMICOPERATIONS_ERROR
#   endif

#else
#   define BSLS_ATOMICOPERATIONS_ERROR
#endif

#if defined(BSLS_ATOMICOPERATIONS_ERROR)
#   error "no implementation of atomics found for this platform"
#endif

namespace BloombergLP {

namespace bsls {

                           // =======================
                           // struct AtomicOperations
                           // =======================

struct AtomicOperations {
    // 'AtomicOperations' provides a namespace for a suite of atomic
    // operations on the following types as defined by the 'AtomicTypes'
    // typedef: integer - 'AtomicTypes::Int', 64bit integer -
    // 'AtomicTypes::Int64', pointer - 'AtomicTypes::Pointer'.

    // TYPES
    typedef AtomicOperations_Imp   Imp;
    typedef Atomic_TypeTraits<Imp> AtomicTypes;

        // *** atomic functions for int ***

    // CLASS METHODS
    static void initInt(AtomicTypes::Int *atomicInt, int initialValue = 0);
        // Initialize the specified 'atomicInt' and set its value to the
        // specified 'initialValue'.

    static int getInt(AtomicTypes::Int const *atomicInt);
        // Atomically retrieve the value of the specified 'atomicInt',
        // providing the sequential consistency memory ordering guarantee.

    static int getIntRelaxed(AtomicTypes::Int const *atomicInt);
        // Atomically retrieve the value of the specified 'atomicInt', without
        // providing any memory ordering guarantees.

    static int getIntAcquire(AtomicTypes::Int const *atomicInt);
        // Atomically retrieve the value of the specified 'atomicInt',
        // providing the acquire memory ordering guarantee.

    static void setInt(AtomicTypes::Int *atomicInt, int value);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value', providing the sequential consistency memory
        // ordering guarantee.

    static void setIntRelaxed(AtomicTypes::Int *atomicInt, int value);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value', without providing any memory ordering
        // guarantees.

    static void setIntRelease(AtomicTypes::Int *atomicInt, int value);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value', providing the release memory ordering guarantee.

    static int swapInt(AtomicTypes::Int *atomicInt, int swapValue);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value', and return its previous value, providing the
        // sequential consistency memory ordering guarantee.

    static int swapIntAcqRel(AtomicTypes::Int *atomicInt, int swapValue);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value', and return its previous value, providing the
        // acquire/release memory ordering guarantee.

    static int testAndSwapInt(AtomicTypes::Int *atomicInt,
                              int               compareValue,
                              int               swapValue);
        // Conditionally set the value of the specified 'atomicInt' to the
        // specified 'swapValue' if and only if the value of 'atomicInt' equals
        // the value of the specified 'compareValue', and return the initial
        // value of 'atomicInt', providing the sequential consistency memory
        // ordering guarantee.  The whole operation is performed atomically.

    static int testAndSwapIntAcqRel(AtomicTypes::Int *atomicInt,
                                    int               compareValue,
                                    int               swapValue);
        // Conditionally set the value of the specified 'atomicInt' to the
        // specified 'swapValue' if and only if the value of 'atomicInt' equals
        // the value of the specified 'compareValue', and return the initial
        // value of 'atomicInt', providing the acquire/release memory ordering
        // guarantee.  The whole operation is performed atomically.

    static int addIntNv(AtomicTypes::Int *atomicInt, int value);
        // Atomically add to the specified 'atomicInt' the specified 'value'
        // and return the resulting value, providing the sequential consistency
        // memory ordering guarantee.

    static int addIntNvRelaxed(AtomicTypes::Int *atomicInt, int value);
        // Atomically add to the specified 'atomicInt' the specified 'value'
        // and return the resulting value, without providing any memory
        // ordering guarantees.

    static int addIntNvAcqRel(AtomicTypes::Int *atomicInt, int value);
        // Atomically add to the specified 'atomicInt' the specified 'value'
        // and return the resulting value, providing the acquire/release memory
        // ordering guarantee.

    static void addInt(AtomicTypes::Int *atomicInt, int value);
        // Atomically add to the specified 'atomicInt' the specified 'value',
        // providing the sequential consistency memory ordering guarantee.

    static void addIntRelaxed(AtomicTypes::Int *atomicInt, int value);
        // Atomically add to the specified 'atomicInt' the specified 'value',
        // without providing any memory ordering guarantees.

    static void addIntAcqRel(AtomicTypes::Int *atomicInt, int value);
        // Atomically add to the specified 'atomicInt' the specified 'value',
        // providing the acquire/release memory ordering guarantee.

    static int incrementIntNv(AtomicTypes::Int *atomicInt);
        // Atomically increment the specified 'atomicInt' by 1 and return the
        // resulting value, providing the sequential consistency memory
        // ordering guarantee.

    static int incrementIntNvAcqRel(AtomicTypes::Int *atomicInt);
        // Atomically increment the specified 'atomicInt' by 1 and return the
        // resulting value, providing the acquire/release memory ordering
        // guarantee.

    static void incrementInt(AtomicTypes::Int *atomicInt);
        // Atomically increment the value of the specified 'atomicInt' by 1,
        // providing the sequential consistency memory ordering guarantee.

    static void incrementIntAcqRel(AtomicTypes::Int *atomicInt);
        // Atomically increment the value of the specified 'atomicInt' by 1,
        // providing the acquire/release memory ordering guarantee.

    static int decrementIntNv(AtomicTypes::Int *atomicInt);
        // Atomically decrement the specified 'atomicInt' by 1 and return the
        // resulting value, providing the sequential consistency memory
        // ordering guarantee.

    static int decrementIntNvAcqRel(AtomicTypes::Int *atomicInt);
        // Atomically decrement the specified 'atomicInt' by 1 and return the
        // resulting value, providing the acquire/release memory ordering
        // guarantee.

    static void decrementInt(AtomicTypes::Int *atomicInt);
        // Atomically decrement the value of the specified 'atomicInt' by 1,
        // providing the sequential consistency memory ordering guarantee.

    static void decrementIntAcqRel(AtomicTypes::Int *atomicInt);
        // Atomically decrement the value of the specified 'atomicInt' by 1,
        // providing the acquire/release memory ordering guarantee.

        // *** atomic functions for Int64 ***

    static void initInt64(AtomicTypes::Int64 *atomicInt,
                          Types::Int64        initialValue = 0);
        // Initialize the specified 'atomicInt' and set its value to the
        // specified 'initialValue'.

    static Types::Int64 getInt64(AtomicTypes::Int64 const *atomicInt);
        // Atomically retrieve the value of the specified 'atomicInt',
        // providing the sequential consistency memory ordering guarantee.

    static Types::Int64 getInt64Relaxed(AtomicTypes::Int64 const *atomicInt);
        // Atomically retrieve the value of the specified 'atomicInt',
        // without providing any memory ordering guarantees.

    static Types::Int64 getInt64Acquire(AtomicTypes::Int64 const *atomicInt);
        // Atomically retrieve the value of the specified 'atomicInt',
        // providing the acquire memory ordering guarantee.

    static void setInt64(AtomicTypes::Int64 *atomicInt,
                         Types::Int64 value);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value', providing the sequential consistency memory
        // ordering guarantee.

    static void setInt64Relaxed(AtomicTypes::Int64 *atomicInt,
                                Types::Int64        value);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value', without providing any memory ordering guarantees.

    static void setInt64Release(AtomicTypes::Int64 *atomicInt,
                                Types::Int64        value);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value', providing the release memory ordering guarantee.

    static Types::Int64 swapInt64(AtomicTypes::Int64 *atomicInt,
                                  Types::Int64        swapValue);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value' and return its previous value, providing the
        // sequential consistency memory ordering guarantee.

    static Types::Int64 swapInt64AcqRel(AtomicTypes::Int64 *atomicInt,
                                        Types::Int64        swapValue);
        // Atomically set the value of the specified 'atomicInt' to the
        // specified 'value' and return its previous value, providing the
        // acquire/release memory ordering guarantee.

    static Types::Int64 testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                                         Types::Int64        compareValue,
                                         Types::Int64        swapValue);
        // Conditionally set the value of the specified 'atomicInt' to the
        // specified 'swapValue' if and only if the value of 'atomicInt' equals
        // the value of the specified 'compareValue', and return the initial
        // value of 'atomicInt', providing the sequential consistency memory
        // ordering guarantee.  The whole operation is performed atomically.

    static Types::Int64 testAndSwapInt64AcqRel(
                                              AtomicTypes::Int64 *atomicInt,
                                              Types::Int64        compareValue,
                                              Types::Int64        swapValue);
        // Conditionally set the value of the specified 'atomicInt' to the
        // specified 'swapValue' if and only if the value of 'atomicInt' equals
        // the value of the specified 'compareValue', and return the initial
        // value of 'atomicInt', providing the acquire/release memory ordering
        // guarantee.  The whole operation is performed atomically.

    static Types::Int64 addInt64Nv(AtomicTypes::Int64 *atomicInt,
                                   Types::Int64        value);
        // Atomically add to the specified 'atomicInt' the specified 'value'
        // and return the resulting value, providing the sequential consistency
        // memory ordering guarantee.

    static Types::Int64 addInt64NvRelaxed(AtomicTypes::Int64 *atomicInt,
                                         Types::Int64         value);
        // Atomically add to the specified 'atomicInt' the specified 'value'
        // and return the resulting value, without providing any memory
        // ordering guarantees.

    static Types::Int64 addInt64NvAcqRel(AtomicTypes::Int64 *atomicInt,
                                         Types::Int64        value);
        // Atomically add to the specified 'atomicInt' the specified 'value'
        // and return the resulting value, providing the acquire/release memory
        // ordering guarantee.

    static void addInt64(AtomicTypes::Int64 *atomicInt,
                         Types::Int64        value);
        // Atomically add to the specified 'atomicInt' the specified 'value',
        // providing the sequential consistency memory ordering guarantee.

    static void addInt64Relaxed(AtomicTypes::Int64 *atomicInt,
                                Types::Int64        value);
        // Atomically add to the specified 'atomicInt' the specified 'value',
        // without providing any memory ordering guarantees.

    static void addInt64AcqRel(AtomicTypes::Int64 *atomicInt,
                               Types::Int64        value);
        // Atomically add to the specified 'atomicInt' the specified 'value',
        // providing the acquire/release memory ordering guarantee.

    static void incrementInt64(AtomicTypes::Int64 *atomicInt);
        // Atomically increment the value of the specified 'atomicInt' by 1,
        // providing the sequential consistency memory ordering guarantee.

    static void incrementInt64AcqRel(AtomicTypes::Int64 *atomicInt);
        // Atomically increment the value of the specified 'atomicInt' by 1,
        // providing the acquire/release memory ordering guarantee.

    static Types::Int64 incrementInt64Nv(AtomicTypes::Int64 *atomicInt);
        // Atomically increment the specified 'atomicInt' by 1 and return the
        // resulting value, providing the sequential consistency memory
        // ordering guarantee.

    static Types::Int64 incrementInt64NvAcqRel(AtomicTypes::Int64 *atomicInt);
        // Atomically increment the specified 'atomicInt' by 1 and return the
        // resulting value, providing the acquire/release memory ordering
        // guarantee.

    static void decrementInt64(AtomicTypes::Int64 *atomicInt);
        // Atomically decrement the specified 'atomicInt' by 1 and return the
        // resulting value, providing the sequential consistency memory
        // ordering guarantee.

    static void decrementInt64AcqRel(AtomicTypes::Int64 *atomicInt);
        // Atomically decrement the specified 'atomicInt' by 1 and return the
        // resulting value, providing the acquire/release memory ordering
        // guarantee.

    static Types::Int64 decrementInt64Nv(AtomicTypes::Int64 *atomicInt);
        // Atomically decrement the specified 'atomicInt' by 1 and return the
        // resulting value, providing the sequential consistency memory
        // ordering guarantee.

    static Types::Int64 decrementInt64NvAcqRel(AtomicTypes::Int64 *atomicInt);
        // Atomically decrement the specified 'atomicInt' by 1 and return the
        // resulting value, providing the acquire/release memory ordering
        // guarantee.

        // *** atomic functions for pointer ***

    static void initPointer(AtomicTypes::Pointer *atomicPtr,
                            void                 *initialValue = 0);
        // Initialize the specified 'atomicPtr' and set its value to the
        // specified 'initialValue'.

    static void *getPtr(AtomicTypes::Pointer const *atomicPtr);
        // Atomically retrieve the value of the specified 'atomicPtr',
        // providing the sequential consistency memory ordering guarantee.

    static void *getPtrRelaxed(AtomicTypes::Pointer const *atomicPtr);
        // Atomically retrieve the value of the specified 'atomicPtr',
        // without providing any memory ordering guarantees.

    static void *getPtrAcquire(AtomicTypes::Pointer const *atomicPtr);
        // Atomically retrieve the value of the specified 'atomicPtr',
        // providing the acquire memory ordering guarantee.

    static void setPtr(AtomicTypes::Pointer *atomicPtr,
                       void                 *value);
        // Atomically set the value of the specified 'atomicPtr' to the
        // specified 'value', providing the sequential consistency memory
        // ordering guarantee.

    static void setPtrRelaxed(AtomicTypes::Pointer *atomicPtr,
                              void                 *value);
        // Atomically set the value of the specified 'atomicPtr' to the
        // specified 'value', without providing any memory ordering guarantees.

    static void setPtrRelease(AtomicTypes::Pointer *atomicPtr,
                              void                 *value);
        // Atomically set the value of the specified 'atomicPtr' to the
        // specified 'value', providing the release memory ordering guarantee.

    static void *swapPtr(AtomicTypes::Pointer *atomicPtr,
                         void                 *swapValue);
        // Atomically set the value of the specified 'atomicPtr' to the
        // specified 'value', and return its previous value, providing the
        // sequential consistency memory ordering guarantee.

    static void *swapPtrAcqRel(AtomicTypes::Pointer *atomicPtr,
                               void                 *swapValue);
        // Atomically set the value of the specified 'atomicPtr' to the
        // specified 'value', and return its previous value, providing the
        // acquire/release memory ordering guarantee.

    static void *testAndSwapPtr(AtomicTypes::Pointer *atomicPtr,
                                void                 *compareValue,
                                void                 *swapValue);
        // Conditionally set the value of the specified 'atomicPtr' to the
        // specified 'swapValue' if and only if the value of 'atomicPtr' equals
        // the value of the specified 'compareValue', and return the initial
        // value of 'atomicPtr', providing the sequential consistency memory
        // ordering guarantee.  The whole operation is performed atomically.

    static void *testAndSwapPtrAcqRel(AtomicTypes::Pointer *atomicPtr,
                                      void                 *compareValue,
                                      void                 *swapValue);
        // Conditionally set the value of the specified 'atomicPtr' to the
        // specified 'swapValue' if and only if the value of 'atomicPtr' equals
        // the value of the specified 'compareValue', and return the initial
        // value of 'atomicPtr', providing the acquire/release memory ordering
        // guarantee.  The whole operation is performed atomically.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                           // -----------------------
                           // struct AtomicOperations
                           // -----------------------

inline
void AtomicOperations::initInt(AtomicTypes::Int *atomicInt, int initialValue)
{
    Imp::initInt(atomicInt, initialValue);
}

inline
int AtomicOperations::getInt(AtomicTypes::Int const *atomicInt)
{
    return Imp::getInt(atomicInt);
}

inline
int AtomicOperations::getIntRelaxed(AtomicTypes::Int const *atomicInt)
{
    return Imp::getIntRelaxed(atomicInt);
}

inline
int AtomicOperations::getIntAcquire(AtomicTypes::Int const *atomicInt)
{
    return Imp::getIntAcquire(atomicInt);
}

inline
void AtomicOperations::setInt(AtomicTypes::Int *atomicInt, int value)
{
    Imp::setInt(atomicInt, value);
}

inline
void AtomicOperations::setIntRelaxed(AtomicTypes::Int *atomicInt, int value)
{
    Imp::setIntRelaxed(atomicInt, value);
}

inline
void AtomicOperations::setIntRelease(AtomicTypes::Int *atomicInt, int value)
{
    Imp::setIntRelease(atomicInt, value);
}

inline
int AtomicOperations::swapInt(AtomicTypes::Int *atomicInt, int swapValue)
{
    return Imp::swapInt(atomicInt, swapValue);
}

inline
int AtomicOperations::swapIntAcqRel(AtomicTypes::Int *atomicInt, int swapValue)
{
    return Imp::swapIntAcqRel(atomicInt, swapValue);
}

inline
int AtomicOperations::testAndSwapInt(AtomicTypes::Int *atomicInt,
                                     int               compareValue,
                                     int               swapValue)
{
    return Imp::testAndSwapInt(atomicInt, compareValue, swapValue);
}

inline
int AtomicOperations::testAndSwapIntAcqRel(AtomicTypes::Int *atomicInt,
                                           int               compareValue,
                                           int               swapValue)
{
    return Imp::testAndSwapIntAcqRel(atomicInt, compareValue, swapValue);
}

inline
int AtomicOperations::addIntNv(AtomicTypes::Int *atomicInt, int value)
{
    return Imp::addIntNv(atomicInt, value);
}

inline
int AtomicOperations::addIntNvRelaxed(AtomicTypes::Int *atomicInt, int value)
{
    return Imp::addIntNvRelaxed(atomicInt, value);
}

inline
int AtomicOperations::addIntNvAcqRel(AtomicTypes::Int *atomicInt, int value)
{
    return Imp::addIntNvAcqRel(atomicInt, value);
}

inline
void AtomicOperations::addInt(AtomicTypes::Int *atomicInt, int value)
{
    Imp::addInt(atomicInt, value);
}

inline
void AtomicOperations::addIntRelaxed(AtomicTypes::Int *atomicInt, int value)
{
    Imp::addIntRelaxed(atomicInt, value);
}

inline
void AtomicOperations::addIntAcqRel(AtomicTypes::Int *atomicInt, int value)
{
    Imp::addIntAcqRel(atomicInt, value);
}

inline
int AtomicOperations::incrementIntNv(AtomicTypes::Int *atomicInt)
{
    return Imp::incrementIntNv(atomicInt);
}

inline
int AtomicOperations::incrementIntNvAcqRel(AtomicTypes::Int *atomicInt)
{
    return Imp::incrementIntNvAcqRel(atomicInt);
}

inline
void AtomicOperations::incrementInt(AtomicTypes::Int *atomicInt)
{
    Imp::incrementInt(atomicInt);
}

inline
void AtomicOperations::incrementIntAcqRel(AtomicTypes::Int *atomicInt)
{
    Imp::incrementIntAcqRel(atomicInt);
}

inline
int AtomicOperations::decrementIntNv(AtomicTypes::Int *atomicInt)
{
    return Imp::decrementIntNv(atomicInt);
}

inline
int AtomicOperations::decrementIntNvAcqRel(AtomicTypes::Int *atomicInt)
{
    return Imp::decrementIntNvAcqRel(atomicInt);
}

inline
void AtomicOperations::decrementInt(AtomicTypes::Int *atomicInt)
{
    Imp::decrementInt(atomicInt);
}

inline
void AtomicOperations::decrementIntAcqRel(AtomicTypes::Int *atomicInt)
{
    Imp::decrementIntAcqRel(atomicInt);
}

inline
void AtomicOperations::initInt64(AtomicTypes::Int64 *atomicInt,
                                 Types::Int64        initialValue)
{
    Imp::initInt64(atomicInt, initialValue);
}

inline
Types::Int64
    AtomicOperations::getInt64(AtomicTypes::Int64 const *atomicInt)
{
    return Imp::getInt64(atomicInt);
}

inline
Types::Int64
    AtomicOperations::getInt64Relaxed(AtomicTypes::Int64 const *atomicInt)
{
    return Imp::getInt64Relaxed(atomicInt);
}

inline
Types::Int64
    AtomicOperations::getInt64Acquire(AtomicTypes::Int64 const *atomicInt)
{
    return Imp::getInt64Acquire(atomicInt);
}

inline
void AtomicOperations::setInt64(AtomicTypes::Int64 *atomicInt,
                                Types::Int64        value)
{
    Imp::setInt64(atomicInt, value);
}

inline
void AtomicOperations::setInt64Relaxed(AtomicTypes::Int64 *atomicInt,
                                       Types::Int64        value)
{
    Imp::setInt64Relaxed(atomicInt, value);
}

inline
void AtomicOperations::setInt64Release(AtomicTypes::Int64 *atomicInt,
                                       Types::Int64        value)
{
    Imp::setInt64Release(atomicInt, value);
}

inline
Types::Int64 AtomicOperations::swapInt64(AtomicTypes::Int64 *atomicInt,
                                         Types::Int64        swapValue)
{
    return Imp::swapInt64(atomicInt, swapValue);
}

inline
Types::Int64 AtomicOperations::swapInt64AcqRel(AtomicTypes::Int64 *atomicInt,
                                               Types::Int64        swapValue)
{
    return Imp::swapInt64AcqRel(atomicInt, swapValue);
}

inline
Types::Int64 AtomicOperations::testAndSwapInt64(
                                              AtomicTypes::Int64 *atomicInt,
                                              Types::Int64        compareValue,
                                              Types::Int64        swapValue)
{
    return Imp::testAndSwapInt64(atomicInt, compareValue, swapValue);
}

inline
Types::Int64 AtomicOperations::testAndSwapInt64AcqRel(
                                              AtomicTypes::Int64 *atomicInt,
                                              Types::Int64        compareValue,
                                              Types::Int64        swapValue)
{
    return Imp::testAndSwapInt64AcqRel(atomicInt, compareValue, swapValue);
}

inline
Types::Int64 AtomicOperations::addInt64Nv(AtomicTypes::Int64 *atomicInt,
                                          Types::Int64        value)
{
    return Imp::addInt64Nv(atomicInt, value);
}

inline
Types::Int64 AtomicOperations::addInt64NvRelaxed(AtomicTypes::Int64 *atomicInt,
                                                 Types::Int64        value)
{
    return Imp::addInt64NvRelaxed(atomicInt, value);
}

inline
Types::Int64 AtomicOperations::addInt64NvAcqRel(AtomicTypes::Int64 *atomicInt,
                                                Types::Int64        value)
{
    return Imp::addInt64NvAcqRel(atomicInt, value);
}

inline
void AtomicOperations::addInt64(AtomicTypes::Int64 *atomicInt,
                                Types::Int64        value)
{
    Imp::addInt64(atomicInt, value);
}

inline
void AtomicOperations::addInt64Relaxed(AtomicTypes::Int64 *atomicInt,
                                       Types::Int64        value)
{
    Imp::addInt64Relaxed(atomicInt, value);
}

inline
void AtomicOperations::addInt64AcqRel(AtomicTypes::Int64 *atomicInt,
                                      Types::Int64        value)
{
    Imp::addInt64AcqRel(atomicInt, value);
}

inline
void AtomicOperations::incrementInt64(AtomicTypes::Int64 *atomicInt)
{
    Imp::incrementInt64(atomicInt);
}

inline
void AtomicOperations::incrementInt64AcqRel(AtomicTypes::Int64 *atomicInt)
{
    Imp::incrementInt64AcqRel(atomicInt);
}

inline
Types::Int64
    AtomicOperations::incrementInt64Nv(AtomicTypes::Int64 *atomicInt)
{
    return Imp::incrementInt64Nv(atomicInt);
}

inline
Types::Int64
    AtomicOperations::incrementInt64NvAcqRel(AtomicTypes::Int64 *atomicInt)
{
    return Imp::incrementInt64NvAcqRel(atomicInt);
}

inline
void AtomicOperations::decrementInt64(AtomicTypes::Int64 *atomicInt)
{
    Imp::decrementInt64(atomicInt);
}

inline
void AtomicOperations::decrementInt64AcqRel(AtomicTypes::Int64 *atomicInt)
{
    Imp::decrementInt64AcqRel(atomicInt);
}

inline
Types::Int64
    AtomicOperations::decrementInt64Nv(AtomicTypes::Int64 *atomicInt)
{
    return Imp::decrementInt64Nv(atomicInt);
}

inline
Types::Int64
    AtomicOperations::decrementInt64NvAcqRel(AtomicTypes::Int64 *atomicInt)
{
    return Imp::decrementInt64NvAcqRel(atomicInt);
}

inline
void AtomicOperations::initPointer(AtomicTypes::Pointer *atomicPtr,
                                   void                 *initialValue)
{
    Imp::initPointer(atomicPtr, initialValue);
}

inline
void *AtomicOperations::getPtr(AtomicTypes::Pointer const *atomicPtr)
{
    return Imp::getPtr(atomicPtr);
}

inline
void * AtomicOperations::getPtrRelaxed(AtomicTypes::Pointer const *atomicPtr)
{
    return Imp::getPtrRelaxed(atomicPtr);
}

inline
void * AtomicOperations::getPtrAcquire(AtomicTypes::Pointer const *atomicPtr)
{
    return Imp::getPtrAcquire(atomicPtr);
}

inline
void AtomicOperations::setPtr(AtomicTypes::Pointer *atomicPtr,
                              void                 *value)
{
    Imp::setPtr(atomicPtr, value);
}

inline
void AtomicOperations::setPtrRelaxed(AtomicTypes::Pointer *atomicPtr,
                                     void                 *value)
{
    Imp::setPtrRelaxed(atomicPtr, value);
}

inline
void AtomicOperations::setPtrRelease(AtomicTypes::Pointer *atomicPtr,
                                     void                 *value)
{
    Imp::setPtrRelease(atomicPtr, value);
}

inline
void *AtomicOperations::swapPtr(AtomicTypes::Pointer *atomicPtr,
                                void                 *swapValue)
{
    return Imp::swapPtr(atomicPtr, swapValue);
}

inline
void *AtomicOperations::swapPtrAcqRel(AtomicTypes::Pointer *atomicPtr,
                                      void                 *swapValue)
{
    return Imp::swapPtrAcqRel(atomicPtr, swapValue);
}

inline
void *AtomicOperations::testAndSwapPtr(AtomicTypes::Pointer *atomicPtr,
                                       void                 *compareValue,
                                       void                 *swapValue)
{
    return Imp::testAndSwapPtr(atomicPtr, compareValue, swapValue);
}

inline
void *AtomicOperations::testAndSwapPtrAcqRel(
                                            AtomicTypes::Pointer *atomicPtr,
                                            void                 *compareValue,
                                            void                 *swapValue)
{
    return Imp::testAndSwapPtrAcqRel(atomicPtr, compareValue, swapValue);
}

}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
