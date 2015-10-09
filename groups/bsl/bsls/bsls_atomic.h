// bsls_atomic.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMIC
#define INCLUDED_BSLS_ATOMIC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide types with atomic operations.
//
//@CLASSES:
//  bsls::AtomicInt: atomic 32-bit integer type
//  bsls::AtomicInt64: atomic 64-bit integer types
//  bsls::AtomicPointer: parameterized atomic pointer type
//
//@SEE_ALSO: bsls_atomicoperations
//
//@DESCRIPTION: This component provides classes with atomic operations for
// 'int', 'Int64', and pointer types.  These classes are based on atomic
// operations supplied by the 'bsls_atomicoperations' component.  The
// 'bsls::AtomicInt' and 'bsls::AtomicInt64' classes represent the
// corresponding atomic integer types, and provide overloaded operators and
// functions for common arithmetic operations.  The 'bsls::AtomicPointer' class
// represents the atomic pointer type, and provides atomic operations to
// manipulate and dereference a pointer.
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
// First, we declare the shared counters (note that, in contrast to the raw
// types defined in 'bsls_atomicoperations', these atomic types are
// zero-initialized at construction):
//..
//  static bsls::AtomicInt64 transactionCount;
//  static bsls::AtomicInt64 successCount;
//  static bsls::AtomicInt64 failureCount;
//..
// Next, for each transaction processed, we atomically increment either the
// success or the failure counter as well as the total transaction count:
//..
//  static void workerThread(int *stop)
//  {
//      while (!(*stop)) {
//          if (processNextTransaction()) {
//              ++failureCount;
//          } else {
//              ++successCount;
//          }
//          ++transactionCount;
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
//      const int num_threads = 10;
//      for (int i = 0; i < num_threads; ++i) {
//          createWorkerThread();
//      }
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
// The class declaration for 'my_CountedHandleRep' is identical to the same
// class in component 'bsls_atomicoperations', with a single exception: member
// 'd_count' is of type 'bsls::AtomicInt', rather than
// 'bsls::AtomicOperations::Int'.  Whereas 'bsls::AtomicOperations::Int' is
// merely a 'typedef' for a platform-specific data type to be used in atomic
// integer operations, 'bsls::AtomicInt' encapsulates those atomic operations
// as member functions and operator overloads.  Class 'my_CountedHandleRep'
// will benefit from this encapsulation: Its method implementations will be
// able to operate on 'd_count' as if it were a standard integer.
//
// Note that, as in the example in component 'bsls_atomicoperations', this rep
// class is intended to be used only by class 'my_CountedHandle', and thus all
// methods of class 'my_CountedHandleRep' are declared private, and 'friend'
// status is granted to class 'my_CountedHandle':
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
//      INSTANCE        *d_instance_p;   // address of managed instance
//      bsls::AtomicInt  d_count;        // number of active references
//
//      // FRIENDS
//      friend class my_CountedHandle<INSTANCE>;
//
//      // NOT IMPLEMENTED
//      my_CountedHandleRep(const my_CountedHandleRep&);
//      my_CountedHandleRep& operator=(const my_CountedHandleRep&);
//
//    private:
//      // PRIVATE CLASS METHODS
//      static void
//      deleteObject(my_CountedHandleRep<INSTANCE> *object);
//
//      // PRIVATE CREATORS
//      my_CountedHandleRep(INSTANCE *instance);
//      ~my_CountedHandleRep();
//
//      // PRIVATE MANIPULATORS
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
// "rep" object when its reference count is reduced to zero.
//
// Similar to 'my_CountedHandleRep', the class declaration for
// 'my_CountedHandle' is identical to that in 'bsls_atomicoperations':
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
//      my_CountedHandle();
//      my_CountedHandle(INSTANCE        *instance);
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
//                                       my_CountedHandleRep<INSTANCE> *object)
//  {
//      delete object;
//  }
//..
// Then, we define the constructor for the 'my_CountedHandleRep<INSTANCE>'
// class.  Member 'd_count' is initialized to 1, reflecting the fact that this
// constructor will be called by a new instance of 'my_CountedHandle', which
// instance is our first and only handle when this constructor is called:
// notice that 'd_count' (of type 'bsls::AtomicInt') is initialized as if it
// were a simple integer; its constructor guarantees that the initialization is
// done atomically.
//..
//  template <class INSTANCE>
//  inline
//  my_CountedHandleRep<INSTANCE>:: my_CountedHandleRep(INSTANCE *instance)
//  : d_instance_p(instance)
//  , d_count(1)
//  {
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
// Next, we define method 'increment', which is called by 'my_CountedHandle'
// to add a new reference to the current "rep" object, which simply increments
// 'd_count', using the prefix 'operator++':
//..
//  // MANIPULATORS
//  template <class INSTANCE>
//  inline
//  void my_CountedHandleRep<INSTANCE>::increment()
//  {
//      ++d_count;
//  }
//..
// The above operation must be done atomically in a multi-threaded context;
// class 'bsls::AtomicInt' provides this guarantee for all its overloaded
// operators, and 'my_CountedHandleRep' relies upon this guarantee.
//
// Then, we implement method 'decrement', which is called by 'my_CountedHandle'
// when a reference to the current "rep" object is being deleted:
//..
//  template <class INSTANCE>
//  inline
//  int my_CountedHandleRep<INSTANCE>::decrement()
//  {
//      return --d_count;
//  }
//..
// This method atomically decrements the number of references to this
// 'my_CountedHandleRep' and, once again, atomicity is guaranteed by the
// underlying type of 'd_count'.
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
//  // CREATORS
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
//  // ACCESSORS
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
//      return d_rep_p ? d_rep_p->d_count : 0;
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
// thread-aware, singly-linked list.  The example class, 'my_PtrStack', is a
// templatized pointer stack, supporting 'push' and 'pop' methods.  The class
// is implemented using a single-linked list.  Nodes in the list are linked
// together using atomic operations.  Instance of this structure are allocated
// using the provided allocator.  When nodes are freed, they are cached on a
// free list.  This free list is also implemented as a single-linked list,
// using atomic pointer operations.
//
// This example parallels the third usage example given for component
// 'bsls_atomicoperations', presenting a different implementation of
// 'my_PtrStack<T>', with an identical public interface.  Note that, where the
// 'bsls_atomicoperations' example uses the basic data type
// 'bsls::AtomicOperations::AtomicTypes::Pointer' for members 'd_list' and
// 'd_freeList', this implementation uses instead the higher-level type
// 'bsls::AtomicPointer<T>'.
//
// First, we create class template, 'my_PtrStack', parameterized by 'TYPE'.
// Instances of this template maintain a list of nodes and a free-node list.
// Each node has a pointer to a data item, 'd_item_p', a link to the next node
// in the list, 'd_next_p' and an atomic flag, 'd_inUseFlag', intended for
// lock-free list manipulation.  The definition of the 'my_PtrStack' class is
// provided below:
//..
//  template <class TYPE>
//  class my_PtrStack {
//      // TYPES
//      struct Node {
//          TYPE                 *d_item_p;
//          Node                 *d_next_p;
//          bsls::AtomicInt       d_inUseFlag; // used to lock this node
//      };
//
//      // DATA
//      bsls::AtomicPointer<Node> d_list;
//      bsls::AtomicPointer<Node> d_freeList;
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
// Then, we write a constructor that default-initializes the stack.  In the
// corresponding example in 'bsls_atomicoperations', the constructor must also
// initialize the atomic pointer 'd_freeList'.  Since this example uses the
// encapsulated type 'bsls::AtomicPointer', initialization of these member
// variables is done in their default constructors.  Hence, no explicit code is
// required in this constructor:
//..
//  // CREATORS
//  template <class TYPE>
//  inline my_PtrStack<TYPE>::my_PtrStack()
//  {
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
//      deleteNodes(d_list);
//      deleteNodes(d_freeList);
//  }
//..
// Then, we define method 'allocateNode' to get a node from the free list in
// the thread-safe manner by leveraging atomic operations to ensure proper
// thread synchronization:
//..
//  // PRIVATE MANIPULATORS
//  template <class TYPE>
//  typename my_PtrStack<TYPE>::Node *my_PtrStack<TYPE>::allocateNode()
//  {
//      Node *node;
//      while (1) {
//          node = d_freeList; // get the current head
//          if (!node) {
//              break;
//          }
//..
// Next, we try locking the node, and start over if locking fails:
//..
//          if (node->d_inUseFlag.swapInt(1)) {
//              continue;
//          }
//..
// Then, we atomically modify the head if it has not changed.  'testAndSwap'
// compares 'd_freeList' to 'node', replacing 'node' with 'node->d_next_p' only
// if it matches 'd_freeList'.  If 'd_freeList' did not match 'node', then the
// free list has been changed on another thread, between its assignment to the
// 'node' and the call to 'testAndSwap'.  If the list head has changed, then
// try again:
//..
//          if (d_freeList.testAndSwap(node, node->d_next_p) == node) {
//              break;
//          }
//
//          // Unlock the node.
//          node->d_inUseFlag = 0;
//      }
//..
// Next, we allocate a new node if there were no nodes in the free node list:
//..
//      if (!node) {
//          node = new Node();  // should allocate with 'd_allocator_p', but
//                              // here we use 'new' directly for simplicity
//          node->d_inUseFlag = 1;
//      }
//
//      return node;
//  }
//..
// Note that the 'node' is returned in the locked state and remained
// locked until it is added to the free list.
//
// Then, we define the 'freeNode' method to add a given 'node' to the free
// list; 'freeNode' also needs to be synchronized using atomic operations:
//..
//  template <class TYPE>
//  inline void my_PtrStack<TYPE>::freeNode(Node *node)
//  {
//      if (!node) {
//          return;
//      }
//
//      while (1) {
//          node->d_next_p = d_freeList;
//          // Atomically test and swap the head of the list with the
//          // new node.  If the list head has been changed (by another
//          // thread), try again.
//          if (d_freeList.testAndSwap(node->d_next_p, node) == node->d_next_p)
//          {
//              break;
//          }
//      }
//
//      // unlock the 'node'
//      node->d_inUseFlag = 0;
//  }
//..
// Now, we begin to define the public "stack-like" interface for 'my_PtrStack'.
// Note that the 'push' method is similar to 'freeNode', except that it assigns
// an item value and operates on 'd_list', which maintains the list of active
// nodes:
//..
//  // MANIPULATORS
//  template <class TYPE>
//  void my_PtrStack<TYPE>::push(TYPE *item)
//  {
//      Node *node = allocateNode();
//      node->d_item_p = item;
//      while (1) {
//          node->d_next_p = d_list;
//          if (d_list.testAndSwap(node->d_next_p, node) == node->d_next_p) {
//              break;
//          }
//      }
//
//      node->d_inUseFlag = 0;
//  }
//..
// Finally, we define the 'pop' method which removes the node from the top
// of active node list, 'd_list', adds it to the free-node list, and returns
// the data item contained in the node to the caller:
//..
//  template <class TYPE>
//  TYPE *my_PtrStack<TYPE>::pop()
//  {
//      Node *node;
//      while (1) {
//          node = d_list;
//          if (!node) {
//              break;
//          }
//
//          if (node->d_inUseFlag.swapInt(1)) {
//              continue;  // node is locked
//          }
//
//          if (d_list.testAndSwap(node, node->d_next_p) == node) {
//              break;  // node list is being modified in another thread
//          }
//
//          node->d_inUseFlag = 0;
//      }
//
//      TYPE *item = node ? node->d_item_p : 0;
//      if (node) {
//          freeNode(node);
//      }
//      return item;
//  }
//..
// Notice that if the stack was empty, a NULL pointer is returned.

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {

namespace bsls {

                               // ===============
                               // class AtomicInt
                               // ===============

class AtomicInt {
    // This class implements an atomic integer, which supports common integer
    // operations in a way that is guaranteed to be atomic.  Operations on
    // objects of this class provide the sequential consistency memory ordering
    // guarantee unless explicitly qualified with a less strict consistency
    // guarantee suffix (i.e., Acquire, Release, AcqRel or Relaxed).

    // DATA
    AtomicOperations::AtomicTypes::Int d_value;

  private:
    // NOT IMPLEMENTED
    AtomicInt(const AtomicInt&);               // = delete
    AtomicInt& operator=(const AtomicInt& );   // = delete
        // Note that the copy constructor and the copy-assignment operator
        // are not implemented because they cannot be done atomically.

  public:
    // CREATORS
    AtomicInt();
        // Create an atomic integer object having the default value 0.

    AtomicInt(int value);
        // Create an atomic integer object having the specified 'value'.

    //! ~AtomicInt() = default;
        // Destroy this atomic integer object.

    // MANIPULATORS
    AtomicInt& operator=(int value);
        // Atomically assign the specified 'value' to this object, and return a
        // modifiable reference to 'this' object.

    int operator+=(int value);
        // Atomically add the specified 'value' to this object, and return the
        // resulting value.

    int operator-=(int value);
        // Atomically subtract the specified 'value' from this object, and
        // return the resulting value.

    int operator++();
        // Atomically increment the value of this object by 1 and return the
        // resulting value.

    int operator++(int);
        // Atomically increment the value of this object by 1 and return the
        // value prior to being incremented.

    int operator--();
        // Atomically decrement the value of this object by 1 and return the
        // resulting value.

    int operator--(int);
        // Atomically decrement the value of this object by 1 and return the
        // value prior to being decremented.

    int add(int value);
        // Atomically add the specified 'value' to this object and return the
        // resulting value.

    int addRelaxed(int value);
        // Atomically add the specified 'value' to this object and return the
        // resulting value, providing the relaxed memory ordering guarantee.

    int addAcqRel(int value);
        // Atomically add the specified 'value' to this object and return the
        // resulting value, providing the acquire/release memory ordering
        // guarantee.

    void storeRelaxed(int value);
        // Atomically assign the specified 'value' to this object, providing
        // the relaxed memory ordering guarantee.

    void storeRelease(int value);
        // Atomically assign the specified 'value' to this object, providing
        // the release memory ordering guarantee.

    int swap(int swapValue);
        // Atomically set the value of this object to the specified 'swapValue'
        // and return its previous value.

    int swapAcqRel(int swapValue);
        // Atomically set the value of this object to the specified 'swapValue'
        // and return its previous value, providing the acquire/release memory
        // ordering guarantee.

    int testAndSwap(int compareValue, int swapValue);
        // Compare the value of this object to the specified 'compareValue'.
        // If they are equal, set the value of this atomic integer to the
        // specified 'swapValue', otherwise leave this value unchanged.  Return
        // the previous value of this atomic integer, whether or not the swap
        // occurred.  Note that the entire test-and-swap operation is performed
        // atomically.

    int testAndSwapAcqRel(int compareValue, int swapValue);
        // Compare the value of this object to the specified 'compareValue'.
        // If they are equal, set the value of this atomic integer to the
        // specified 'swapValue', otherwise leave this value unchanged.  Return
        // the previous value of this atomic integer, whether or not the swap
        // occurred.  Note that the entire test-and-swap operation is performed
        // atomically and it provides the acquire/release memory ordering
        // guarantee.

    // ACCESSORS
    operator int() const;
        // Return the current value of this object.

    int load() const;
        // Return the current value of this object.

    int loadRelaxed() const;
        // Return the current value of this object, providing the relaxed
        // memory ordering guarantee.

    int loadAcquire() const;
        // Return the current value of this object, providing the acquire
        // memory ordering guarantee.
};

                              // =================
                              // class AtomicInt64
                              // =================

class AtomicInt64 {
    // This class is implements an atomic 64-bit integer, which supports common
    // integer operations in a way that is guaranteed to be atomic.  Operations
    // on objects of this class provide the sequential consistency memory
    // ordering guarantee unless explicitly qualified with a less strict
    // consistency guarantee suffix (i.e., Acquire, Release, AcqRel or
    // Relaxed).

    // DATA
    AtomicOperations::AtomicTypes::Int64 d_value;

  private:
    // NOT IMPLEMENTED
    AtomicInt64(const AtomicInt64&);              // = delete
    AtomicInt64& operator=(const AtomicInt64&);   // = delete
        // Note that the copy constructor and the copy-assignment operator
        // are not implemented because they cannot be done atomically.

  public:
    // CREATORS
    AtomicInt64();
        // Create an atomic 64-bit integer object having the default value 0.

    AtomicInt64(Types::Int64 value);
        // Create an atomic 64-bit integer object having the specified 'value'.

    //! ~AtomicInt64() = default;
        // Destroy this atomic 64-bit integer object.

    // MANIPULATORS
    AtomicInt64& operator=(Types::Int64 value);
        // Atomically assign the specified 'value' to this object, and return a
        // modifiable reference to 'this' object.

    Types::Int64 operator+=(Types::Int64 value);
        // Atomically add the specified 'value' to this object, and return the
        // resulting value.

    Types::Int64 operator-=(Types::Int64 value);
        // Atomically subtract the specified 'value' from this object, and
        // return the resulting value.

    Types::Int64 operator++();
        // Atomically increment the value of this object by 1 and return the
        // resulting value.

    Types::Int64 operator++(int);
        // Atomically increment the value of this object by 1 and return the
        // value prior to being incremented.

    Types::Int64 operator--();
        // Atomically decrement the value of this object by 1 and return the
        // resulting value.

    Types::Int64 operator--(int);
        // Atomically decrement the value of this object by 1 and return the
        // value prior to being decremented.

    Types::Int64 add(Types::Int64 value);
        // Atomically add the specified 'value' to this object and return the
        // resulting value.

    Types::Int64 addRelaxed(Types::Int64 value);
        // Atomically add the specified 'value' to this object and return the
        // resulting value, providing the relaxed memory ordering guarantee.

    Types::Int64 addAcqRel(Types::Int64 value);
        // Atomically add the specified 'value' to this object and return the
        // resulting value, providing the acquire/release memory ordering
        // guarantee.

    void storeRelaxed(Types::Int64 value);
        // Atomically assign the specified 'value' to this object, providing
        // the relaxed memory ordering guarantee.

    void storeRelease(Types::Int64 value);
        // Atomically assign the specified 'value' to this object, providing
        // the release memory ordering guarantee.

    Types::Int64 swap(Types::Int64 swapValue);
        // Atomically set the value of this object to the specified 'swapValue'
        // and return its previous value.

    Types::Int64 swapAcqRel(Types::Int64 swapValue);
        // Atomically set the value of this object to the specified 'swapValue'
        // and return its previous value, providing the acquire/release memory
        // ordering guarantee.

    Types::Int64 testAndSwap(Types::Int64 compareValue,
                             Types::Int64 swapValue);
        // Compare the value of this object to the specified 'compareValue'.
        // If they are equal, set the value of this atomic integer to the
        // specified 'swapValue', otherwise leave this value unchanged.  Return
        // the previous value of this atomic integer, whether or not the swap
        // occurred.  Note that the entire test-and-swap operation is performed
        // atomically.

    Types::Int64 testAndSwapAcqRel(Types::Int64 compareValue,
                                   Types::Int64 swapValue);
        // Compare the value of this object to the specified 'compareValue'.
        // If they are equal, set the value of this atomic integer to the
        // specified 'swapValue', otherwise leave this value unchanged.  Return
        // the previous value of this atomic integer, whether or not the swap
        // occurred.  Note that the entire test-and-swap operation is performed
        // atomically and it provides the acquire/release memory ordering
        // guarantee.

    // ACCESSORS
    operator Types::Int64() const;
        // Return the current value of this object.

    Types::Int64 load() const;
        // Return the current value of this object.

    Types::Int64 loadRelaxed() const;
        // Return the current value of this object, providing the relaxed
        // memory ordering guarantee.

    Types::Int64 loadAcquire() const;
        // Return the current value of this object, providing the acquire
        // memory ordering guarantee.
};

                             // ===================
                             // class AtomicPointer
                             // ===================

template <class TYPE>
class AtomicPointer {
    // This class implements an atomic pointer to a parameterized 'TYPE', which
    // supports common pointer operations in a way that is guaranteed to be
    // atomic.  Operations on objects of this class provide the sequential
    // consistency memory ordering guarantee unless explicitly qualified with
    // a less strict consistency guarantee suffix (i.e., Acquire, Release,
    // AcqRel or Relaxed).

    // DATA
    AtomicOperations::AtomicTypes::Pointer d_value;

    typedef char AtomicPointer_PointerSizeCheck[
        sizeof(TYPE *) == sizeof(void *) ? 1 : -1];
        // Static assert that a 'TYPE*' pointer is binary compatible with a
        // 'void*' pointer.  The implementation of 'AtomicPointer' uses
        // 'reinterpret_cast' to convert between 'TYPE*' and 'void*' because
        // function pointers are not implicitly convertible to 'void*', and
        // this assert makes sure that such a cast is safe.  Note that
        // 'bslmf_Assert' can't be used here because of package dependency
        // rules.

    template <class TYPE1>
    struct RemoveConst              { typedef TYPE1 Type; };
    template <class TYPE1>
    struct RemoveConst<TYPE1 const> { typedef TYPE1 Type; };

    typedef typename RemoveConst<TYPE>::Type NcType;

  private:
    // NOT IMPLEMENTED
    AtomicPointer(const AtomicPointer<TYPE>&);                  // = delete
    AtomicPointer<TYPE>& operator=(const AtomicPointer<TYPE>&); // = delete
        // Note that the copy constructor and the copy-assignment operator
        // are not implemented because they cannot be done atomically.

  public:
    // CREATORS
    AtomicPointer();
        // Create an atomic pointer object having the default value NULL.

    AtomicPointer(TYPE *value);
        // Create an atomic pointer object having the specified 'value'.

    //! ~AtomicPointer() = default;
        // Destroy this atomic pointer.

    // MANIPULATORS
    AtomicPointer<TYPE>& operator=(TYPE *value);
        // Atomically assign the specified 'value' to this object, and return a
        // modifiable reference to 'this' object.

    void storeRelaxed(TYPE *value);
        // Atomically assign the specified 'value' to this object, providing
        // the relaxed memory ordering guarantee.

    void storeRelease(TYPE *value);
        // Atomically assign the specified 'value' to this object, providing
        // the release memory ordering guarantee.

    TYPE *swap(TYPE *swapValue);
        // Atomically set the value of this object to the specified 'swapValue'
        // and return its previous value.

    TYPE *swapAcqRel(TYPE *swapValue);
        // Atomically set the value of this object to the specified 'swapValue'
        // and return its previous value, providing the acquire/release memory
        // ordering guarantee.

    TYPE *testAndSwap(const TYPE *compareValue, TYPE *swapValue);
        // Compare the value of this object to the specified 'compareValue'.
        // If they are equal, set the value of this atomic pointer to the
        // specified 'swapValue', otherwise leave this value unchanged.  Return
        // the previous value of this atomic pointer, whether or not the swap
        // occurred.  Note that the entire test-and-swap operation is performed
        // atomically.

    TYPE *testAndSwapAcqRel(const TYPE *compareValue, TYPE *swapValue);
        // Compare the value of this object to the specified 'compareValue'.
        // If they are equal, set the value of this atomic pointer to the
        // specified 'swapValue', otherwise leave this value unchanged.  Return
        // the previous value of this atomic pointer, whether or not the swap
        // occurred.  Note that the entire test-and-swap operation is performed
        // atomically and it provides the acquire/release memory ordering
        // guarantee.

    // ACCESSORS
    TYPE& operator*() const;
        // Return a reference to the value currently pointed to by this object.
        // The behavior is undefined if this pointer has a value of 0.

    TYPE *operator->() const;
        // Return the current value of this object.

    operator TYPE*() const;
        // Return the current value of this object.

    TYPE *load() const;
        // Return the current value of this object.

    TYPE *loadRelaxed() const;
        // Return the current value of this object, providing the relaxed
        // memory ordering guarantee.

    TYPE *loadAcquire() const;
        // Return the current value of this object, providing the acquire
        // memory ordering guarantee.
};

}  // close package namespace

namespace bsls {

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                               // ---------------
                               // class AtomicInt
                               // ---------------

// CREATORS
inline
AtomicInt::AtomicInt()
{
    AtomicOperations_Imp::initInt(&d_value, 0);
}

inline
AtomicInt::AtomicInt(int value)
{
    AtomicOperations_Imp::initInt(&d_value, value);
}

// MANIPULATORS
inline
AtomicInt& AtomicInt::operator=(int value)
{
    AtomicOperations_Imp::setInt(&d_value, value);
    return *this;
}

inline
int AtomicInt::operator+=(int value)
{
    return AtomicOperations_Imp::addIntNv(&d_value, value);
}

inline
int AtomicInt::operator-=(int value)
{
    return AtomicOperations_Imp::addIntNv(&d_value, -value);
}

inline
int AtomicInt::operator++()
{
    return AtomicOperations_Imp::incrementIntNv(&d_value);
}

inline
int AtomicInt::operator++(int)
{
    return AtomicOperations_Imp::incrementIntNv(&d_value) - 1;
}

inline
int AtomicInt::operator--()
{
    return AtomicOperations_Imp::decrementIntNv(&d_value);
}

inline
int AtomicInt::operator--(int)
{
    return AtomicOperations_Imp::decrementIntNv(&d_value) + 1;
}

inline
int AtomicInt::add(int value)
{
    return AtomicOperations_Imp::addIntNv(&d_value, value);
}

inline
int AtomicInt::addRelaxed(int value)
{
    return AtomicOperations_Imp::addIntNvRelaxed(&d_value, value);
}

inline
int AtomicInt::addAcqRel(int value)
{
    return AtomicOperations_Imp::addIntNvAcqRel(&d_value, value);
}

inline
void AtomicInt::storeRelaxed(int value)
{
    AtomicOperations_Imp::setIntRelaxed(&d_value, value);
}

inline
void AtomicInt::storeRelease(int value)
{
    AtomicOperations_Imp::setIntRelease(&d_value, value);
}

inline
int AtomicInt::swap(int swapValue)
{
    return AtomicOperations_Imp::swapInt(&d_value, swapValue);
}

inline
int AtomicInt::swapAcqRel(int swapValue)
{
    return AtomicOperations_Imp::swapIntAcqRel(&d_value, swapValue);
}

inline
int AtomicInt::testAndSwap(int compareValue, int swapValue)
{
    return AtomicOperations_Imp::testAndSwapInt(&d_value,
                                                     compareValue,
                                                     swapValue);
}

inline
int AtomicInt::testAndSwapAcqRel(int compareValue, int swapValue)
{
    return AtomicOperations_Imp::testAndSwapIntAcqRel(&d_value,
                                                      compareValue,
                                                      swapValue);
}

// ACCESSORS

inline
AtomicInt::operator int() const
{
    return AtomicOperations_Imp::getInt(&d_value);
}

inline
int AtomicInt::load() const
{
    return this->operator int();
}

inline
int AtomicInt::loadRelaxed() const
{
    return AtomicOperations_Imp::getIntRelaxed(&d_value);
}

inline
int AtomicInt::loadAcquire() const
{
    return AtomicOperations_Imp::getIntAcquire(&d_value);
}

                              // -----------------
                              // class AtomicInt64
                              // -----------------

// CREATORS
inline
AtomicInt64::AtomicInt64()
{
    AtomicOperations_Imp::initInt64(&d_value, 0);
}

inline
AtomicInt64::AtomicInt64(Types::Int64 value)
{
    AtomicOperations_Imp::initInt64(&d_value, value);
}

// MANIPULATORS
inline
AtomicInt64& AtomicInt64::operator=(Types::Int64 value)
{
    AtomicOperations_Imp::setInt64(&d_value, value);
    return *this;
}

inline
Types::Int64 AtomicInt64::operator+=(Types::Int64 value)
{
    return AtomicOperations_Imp::addInt64Nv(&d_value, value);
}

inline
Types::Int64 AtomicInt64::operator-=(Types::Int64 value)
{
    return AtomicOperations_Imp::addInt64Nv(&d_value, -value);
}

inline
Types::Int64 AtomicInt64::operator++()
{
    return AtomicOperations_Imp::incrementInt64Nv(&d_value);
}

inline
Types::Int64 AtomicInt64::operator++(int)
{
    return AtomicOperations_Imp::incrementInt64Nv(&d_value) - 1;
}

inline
Types::Int64 AtomicInt64::operator--()
{
    return AtomicOperations_Imp::decrementInt64Nv(&d_value);
}

inline
Types::Int64 AtomicInt64::operator--(int)
{
    return AtomicOperations_Imp::decrementInt64Nv(&d_value) + 1;
}

inline
Types::Int64 AtomicInt64::add(Types::Int64 value)
{
    return AtomicOperations_Imp::addInt64Nv(&d_value, value);
}

inline
Types::Int64 AtomicInt64::addRelaxed(Types::Int64 value)
{
    return AtomicOperations_Imp::addInt64NvRelaxed(&d_value, value);
}

inline
Types::Int64 AtomicInt64::addAcqRel(Types::Int64 value)
{
    return AtomicOperations_Imp::addInt64NvAcqRel(&d_value, value);
}

inline
void AtomicInt64::storeRelaxed(Types::Int64 value)
{
    AtomicOperations_Imp::setInt64Relaxed(&d_value, value);
}

inline
void AtomicInt64::storeRelease(Types::Int64 value)
{
    AtomicOperations_Imp::setInt64Release(&d_value, value);
}

inline
Types::Int64 AtomicInt64::swap(Types::Int64 swapValue)
{
    return AtomicOperations_Imp::swapInt64(&d_value, swapValue);
}

inline
Types::Int64 AtomicInt64::swapAcqRel(Types::Int64 swapValue)
{
    return AtomicOperations_Imp::swapInt64AcqRel(&d_value, swapValue);
}

inline
Types::Int64
AtomicInt64::testAndSwap(Types::Int64 compareValue,
                              Types::Int64 swapValue)
{
    return AtomicOperations_Imp::testAndSwapInt64(&d_value,
                                                       compareValue,
                                                       swapValue);
}

inline
Types::Int64
AtomicInt64::testAndSwapAcqRel(Types::Int64 compareValue,
                                    Types::Int64 swapValue)
{
    return AtomicOperations_Imp::testAndSwapInt64AcqRel(&d_value,
                                                        compareValue,
                                                        swapValue);
}

// ACCESSORS
inline
AtomicInt64::operator Types::Int64() const
{
    return AtomicOperations_Imp::getInt64(&d_value);
}

inline
Types::Int64 AtomicInt64::load() const
{
    return this->operator Types::Int64();
}

inline
Types::Int64 AtomicInt64::loadRelaxed() const
{
    return AtomicOperations_Imp::getInt64Relaxed(&d_value);
}

inline
Types::Int64 AtomicInt64::loadAcquire() const
{
    return AtomicOperations_Imp::getInt64Acquire(&d_value);
}

                             // -------------------
                             // class AtomicPointer
                             // -------------------

// CREATORS
template <class TYPE>
inline
AtomicPointer<TYPE>::AtomicPointer()
{
    AtomicOperations_Imp::initPointer(&d_value, 0);
}

template <class TYPE>
inline
AtomicPointer<TYPE>::AtomicPointer(TYPE *value)
{
    AtomicOperations_Imp::initPointer(
            &d_value,
            reinterpret_cast<void *>(const_cast<NcType *>(value)));
}

// MANIPULATORS
template <class TYPE>
inline
AtomicPointer<TYPE>&
AtomicPointer<TYPE>::operator=(TYPE *value)
{
    AtomicOperations_Imp::setPtr(
            &d_value,
            reinterpret_cast<void *>(const_cast<NcType *>(value)));
    return *this;
}

template <class TYPE>
inline
void AtomicPointer<TYPE>::storeRelaxed(TYPE *value)
{
    AtomicOperations_Imp::setPtrRelaxed(
            &d_value,
            reinterpret_cast<void *>(const_cast<NcType *>(value)));
}

template <class TYPE>
inline
void AtomicPointer<TYPE>::storeRelease(TYPE *value)
{
    AtomicOperations_Imp::setPtrRelease(
            &d_value,
            reinterpret_cast<void *>(const_cast<NcType *>(value)));
}

template <class TYPE>
inline
TYPE *AtomicPointer<TYPE>::swap(TYPE *swapValue)
{
    return reinterpret_cast<TYPE *>(
        AtomicOperations_Imp::swapPtr(
            &d_value,
            reinterpret_cast<void *>(const_cast<NcType *>(swapValue))));
}

template <class TYPE>
inline
TYPE *AtomicPointer<TYPE>::swapAcqRel(TYPE *swapValue)
{
    return reinterpret_cast<TYPE *>(
        AtomicOperations_Imp::swapPtrAcqRel(
            &d_value,
            reinterpret_cast<void *>(const_cast<NcType *>(swapValue))));
}

template <class TYPE>
inline
TYPE *AtomicPointer<TYPE>::testAndSwap(const TYPE *compareValue,
                                       TYPE       *swapValue)
{
    return reinterpret_cast<TYPE *>(
        AtomicOperations_Imp::testAndSwapPtr(
            &d_value,
            reinterpret_cast<void *>(const_cast<NcType *>(compareValue)),
            reinterpret_cast<void *>(const_cast<NcType *>(swapValue))));
}

template <class TYPE>
inline
TYPE *AtomicPointer<TYPE>::testAndSwapAcqRel(const TYPE *compareValue,
                                             TYPE       *swapValue)
{
    return reinterpret_cast<TYPE *>(
        AtomicOperations_Imp::testAndSwapPtrAcqRel(
            &d_value,
            reinterpret_cast<void *>(const_cast<NcType *>(compareValue)),
            reinterpret_cast<void *>(const_cast<NcType *>(swapValue))));
}

// ACCESSORS
template <class TYPE>
inline
AtomicPointer<TYPE>::operator TYPE*() const
{
    return (TYPE *) AtomicOperations_Imp::getPtr(&d_value);
}

template <class TYPE>
inline
TYPE *AtomicPointer<TYPE>::load() const
{
    return this->operator TYPE*();
}

template <class TYPE>
inline
TYPE& AtomicPointer<TYPE>::operator*() const
{
    return *((TYPE *) AtomicOperations_Imp::getPtr(&d_value));
}

template <class TYPE>
inline
TYPE *AtomicPointer<TYPE>::operator->() const
{
    return (TYPE *) AtomicOperations_Imp::getPtr(&d_value);
}

template <class TYPE>
inline
TYPE *AtomicPointer<TYPE>::loadRelaxed() const
{
    return (TYPE *) AtomicOperations_Imp::getPtrRelaxed(&d_value);
}

template <class TYPE>
inline
TYPE *AtomicPointer<TYPE>::loadAcquire() const
{
    return (TYPE *) AtomicOperations_Imp::getPtrAcquire(&d_value);
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
