// bslstl_sharedptr.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_SHAREDPTR
#define INCLUDED_BSLSTL_SHAREDPTR

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a generic reference-counted shared pointer wrapper.
//
//@CLASSES:
//  bsl::enable_shared_from_this: base class to allow shared ownership of self
//  bsl::shared_ptr: shared pointer
//  bsl::weak_ptr: "weak" reference to reference-counted shared object
//  bslstl::SharedPtrUtil: shared pointer utility functions
//  bslstl::SharedPtrNilDeleter: no-op deleter
//
//@CANONICAL_HEADER: bsl_memory.h
//
//@SEE_ALSO: bslma_managedptr, bslma_sharedptrrep
//
//@DESCRIPTION: This component implements a thread-safe, generic,
// reference-counted "smart pointer" to support "shared ownership" of objects
// of (template parameter) 'ELEMENT_TYPE'.  Shared pointers implement a form of
// the "envelope/letter" idiom.  For each shared object, a representation that
// manages the number of references to it is created.  Many shared pointers can
// simultaneously refer to the same shared object by storing a reference to the
// same representation.  Shared pointers also implement the "construction is
// acquisition, destruction is release" idiom.  When a shared pointer is
// created it increments the number of shared references to the shared object
// that was specified to its constructor (or was referred to by a shared
// pointer passed to the copy constructor).  When a shared pointer is assigned
// to or destroyed, then the number of shared references to the shared object
// is decremented.  When all references to the shared object are released, both
// the representation and the object are destroyed.  'bsl::shared_ptr' emulates
// the interface of a native pointer.  The shared object may be accessed
// directly using the '->' operator, or the dereference operator (operator '*')
// can be used to obtain a reference to the shared object.
//
// This component also provides a mechanism, 'bsl::weak_ptr', used to create
// weak references to reference-counted shared ('bsl::shared_ptr') objects.  A
// weak reference provides conditional access to a shared object managed by a
// 'bsl::shared_ptr', but, unlike a shared (or "strong") reference, does not
// affect the shared object's lifetime.  An object having even one shared
// reference to it will not be destroyed, but an object having only weak
// references would have been destroyed when the last shared reference was
// released.
//
// A weak pointer can be constructed from another weak pointer or a
// 'bsl::shared_ptr'.  To access the shared object referenced by a weak pointer
// clients must first obtain a shared pointer to that object using the 'lock'
// method.  If the shared object has been destroyed (as indicated by the
// 'expired' method), then 'lock' returns a shared pointer in the default
// constructed (empty) state.
//
// This component also provides a mechanism, 'bsl::enable_shared_from_this',
// which can be used to create a type that participates in its own ownership
// through the reference-counting of a 'shared_ptr'.
//
// This component also provides a functor, 'bslstl::SharedPtrNilDeleter', which
// may used to create a shared pointer that takes no action when the last
// shared reference is destroyed.
//
// This component also provides a utility class, 'bslstl::SharedPtrUtil', which
// provides several functions that are frequently used with shared pointers.
//
//
///Thread Safety
///-------------
// This section qualifies the thread safety of 'bsl::shared_ptr' objects and
// 'bsl::weak_ptr' objects themselves rather than the thread safety of the
// objects being referenced.
//
// It is *not* *safe* to access or modify a 'bsl::shared_ptr' (or
// 'bsl::weak_ptr') object in one thread while another thread modifies the same
// object.  However, it is safe to access or modify two distinct 'shared_ptr'
// (or 'bsl::weak_ptr') objects simultaneously, each from a separate thread,
// even if they share ownership of a common object.  It is safe to access a
// single 'bsl::shared_ptr' (or 'bsl::weak_ptr') object simultaneously from two
// or more separate threads, provided no other thread is simultaneously
// modifying the object.
//
// It is safe to access, modify, copy, or delete a shared pointer (or weak
// pointer) in one thread, while other threads access or modify other shared
// pointers and weak pointers pointing to or managing the same object (the
// reference count is managed using atomic operations).  However, there is no
// guarantee regarding the safety of accessing or modifying the object
// *referred* *to* by the shared pointer simultaneously from multiple threads.
//
///Shared and Weak References
///--------------------------
// There are two types of references to shared objects:
//
// 1) A shared reference allows users to share the ownership of an object and
// control its lifetime.  A shared object is destroyed only when the last
// shared reference to it is released.  A shared reference to an object can be
// obtained by creating a 'shared_ptr' referring to it.
//
// 2) A weak reference provides users conditional access to an object without
// sharing its ownership (or affecting its lifetime).  A shared object can be
// destroyed even if there are weak references to it.  A weak reference to an
// object can be obtained by creating a 'weak_ptr' referring to the object from
// a 'shared_ptr' referring to that object.
//
///In-place/Out-of-place Representations
///-------------------------------------
// 'shared_ptr' provides two types of representations: an out-of-place
// representation, and an in-place representation.  Out-of-place
// representations are used to refer to objects that are constructed externally
// to their associated representations.  Out-of-place objects are provided to a
// shared pointer by passing their address along with the deleter that should
// be used to destroy the object when all references to it have been released.
// In-place objects can be constructed directly within a shared pointer
// representation (see 'createInplace').
//
// Below we provide a diagram illustrating the differences between the two
// representations for a shared pointer to an 'int'.  First we create an 'int'
// object on the heap, initialized to 10, and pass its address to a shared
// pointer constructor, resulting in an out-of-place representation for the
// shared object:
//..
//  bslma::NewDeleteAllocator nda;
//  int *value = new (nda) int(10);
//  shared_ptr<int> outOfPlaceSharedPtr(value, &nda);
//..
// Next we create an in-place representation of a shared 'int' object that is
// also initialized to 10:
//..
//  shared_ptr<int> inPlaceSharedPtr;
//  inPlaceSharedPtr.createInplace(&nda, 10);
//..
// The memory layouts of these two representations are shown below (where
// 'd_ptr_p' refers to the shared object and 'd_rep_p' refers to the
// representation):
//..
//  Out-of-Place Representation                 In-Place Representation
//  ----------------------------              ----------------------------
//
//  +------------+                            +------------+
//  |            |                            |            |
//  |  d_ptr_p ------>+-----------+           |  d_ptr_p ---------+
//  |            |    |     10    |           |            |      |
//  |            |    +-----------+           |            |      |
//  |            |                            |            |      |
//  |  d_rep_p ------>+-----------+           |  d_rep_p ------>+-v---------+
//  |            |    | reference |           |            |    |+---------+|
//  |            |    |  counts   |           |            |    ||    10   ||
//  +------------+    +-----------+           +------------+    |+---------+|
//                                                              | reference |
//                                                              |  counts   |
//                                                              +-----------+
//..
// An out-of-place representation is generally less efficient than an in-place
// representation since it usually requires at least two allocations (one to
// construct the object and one to construct the shared pointer representation
// for the object).
//
// Creating an in-place shared pointer does not require the template parameter
// type to inherit from a special class (such as
// 'bsl::enable_shared_from_this'); in that case, 'shared_ptr' supports up to
// fourteen arguments that can be passed directly to the object's constructor.
// For in-place representations, both the object and the representation can be
// constructed in one allocation as opposed to two, effectively creating an
// "intrusive" reference counter.  Note that the size of the allocation is
// determined at compile-time from the combined footprint of the object and of
// the reference counts.  It is also possible to create shared pointers to
// buffers whose sizes are determined at runtime, although such buffers consist
// of raw (uninitialized) memory.
//
///Weak Pointers using "in-place" or Pooled Shared Pointer Representations
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A weak pointer that is not in the empty state shares a common representation
// (used to refer to the shared object) with the shared (or other weak) pointer
// from which it was constructed, and holds this representation until it is
// either destroyed or reset.  This common representation is not destroyed and
// deallocated (although the shared object itself may have been destroyed)
// until all weak references to that common representation have been released.
//
// Due to this behavior the *memory* *footprint* of shared objects that are
// constructed "in-place" in the shared pointer representation (see above) is
// not deallocated until all weak references to that shared object are
// released.  Note that a shared object is always destroyed when the last
// shared reference to it is released.  Also note that the same behavior
// applies if the shared object were obtained from a class that pools shared
// pointer representations (for example, 'bcec_SharedObjectPool').
//
// For example suppose we have a class with a large memory footprint:
//..
//  class ClassWithLargeFootprint {
//      // This class has a large memory footprint.
//
//      // TYPES
//      enum { BUFFER_SIZE = 1024 };
//          // The size of the buffer owned by this 'class'.
//
//      // DATA
//      char d_buffer[BUFFER_SIZE];
//
//      // ...
//  };
//..
// We then create an "in-place" shared pointer to an object of
// 'ClassWithLargeFootprint' using the 'createInplace' method of 'shared_ptr'.
// The 'sp' shared pointer representation of 'sp' will create a
// 'ClassWithLargeFootprint' object "in-place":
//..
//  shared_ptr<ClassWithLargeFootprint> sp;
//  sp.createInplace();
//..
// Next we construct a weak pointer from this (in-place) shared pointer:
//..
//  weak_ptr<ClassWithLargeFootprint> wp(sp);
//..
// Now releasing all shared references to the shared object (using the 'reset'
// function) causes the object's destructor to be called, but the
// representation is not destroyed (and the object's footprint is not
// deallocated) until 'wp' releases its weak reference:
//..
//  sp.reset(); // The object's footprint is not deallocated until all weak
//              // references to it are released.
//
//  wp.reset(); // The release of the *last* weak reference results in the
//              // destruction and deallocation of the representation and the
//              // object's footprint.
//..
// If a shared object has a large footprint, and the client anticipates there
// will be weak references to it, then an out-of-place shared pointer
// representation may be preferred because it destroys the shared object and
// deallocates its footprint when the last *shared* reference is released,
// regardless of whether there are any outstanding weak references to the same
// representation.
//
///Correct Usage of the Allocator Model
///------------------------------------
// Note that once constructed, there is no difference in type, usage, or
// efficiency between in-place and out-of-place shared pointers, except that an
// in-place shared pointer will exhibit greater locality of reference and
// faster destruction (because there is only one allocated block).  Also note
// that an object created with an allocator needs to have this allocator
// specified as its last constructor argument, but this allocator may be
// different from the one passed as the first argument to 'createInplace'.
//
// For example, consider the following snippet of code:
//..
//  bslma::Allocator *allocator1, *allocator2;
//  // ...
//  shared_ptr<bsl::string> ptr;
//  ptr.createInplace(allocator1, bsl::string("my string"), allocator2);
//..
// Here 'allocator1' is used to obtain the shared pointer representation and
// the in-place 'bsl::string' object, and 'allocator2' is used by the
// 'bsl::string' object (having the value "my string") for its memory
// allocations.  Typically, both allocators will be the same, and so the same
// allocator will need to be specified twice.
//
///Deleters
///--------
// When the last shared reference to a shared object is released, the object is
// destroyed using the "deleter" provided when the associated shared pointer
// representation was created.  'shared_ptr' supports two kinds of "deleter"
// objects, which vary in how they are invoked.  A "function-like" deleter is
// any language entity that can be invoked such that the expression
// 'deleterInstance(objectPtr)' is a valid expression.  A "factory" deleter is
// any language entity that can be invoked such that the expression
// 'deleterInstance.deleteObject(objectPtr)' is a valid expression, where
// 'deleterInstance' is an instance of the "deleter" object, and 'objectPtr' is
// a pointer to the shared object.  Factory deleters are a BDE extension to the
// ISO C++ Standard Library specification for 'shared_ptr'.  In summary:
//..
//  Deleter                     Expression used to destroy 'objectPtr'
//  - - - - - - - - - - - -     - - - - - - - - - - - - - - - - - - -
//  "function-like"             deleterInstance(objectPtr);
//  "factory"                   deleterInstance.deleteObject(objectPtr);
//..
// The following are examples of function-like deleters that delete an object
// of 'my_Type':
//..
//  void deleteObject(my_Type *object);
//      // Delete the specified 'object'.
//
//  void releaseObject(my_Type *object);
//      // Release the specified 'object'.
//
//  struct FunctionLikeDeleterObject {
//      // This 'struct' provides an 'operator()' that can be used to delete a
//      // 'my_Type' object.
//
//      void operator()(my_Type *object);
//          // Destroy the specified 'object'.
//  };
//..
// The following, on the other hand is an example of a factory deleter:
//..
//  class my_Factory {
//
//     // . . .
//
//     // MANIPULATORS
//     my_Type *createObject(bslma::Allocator *basicAllocator = 0);
//         // Create a 'my_Type' object.  Optionally specify a
//         // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//         // 0, the currently installed default allocator is used.
//
//     void deleteObject(my_Type *object);
//         // Delete the specified 'object'.
//  };
//
//  class my_Allocator : public bslma::Allocator { /* ... */ };
//..
// Note that 'deleteObject' is provided by all 'bslma' allocators and by any
// object that implements the 'bdlma::Deleter' protocol.  Thus, any of these
// objects can be used as a factory deleter.  The purpose of this design is to
// allow 'bslma' allocators and factories to be used seamlessly as deleters.
//
// The selection of which expression is used by 'shared_ptr' to destroy a
// shared object is based on how the deleter is passed to the shared pointer
// object: Deleters that are passed by *address* are assumed to be factory
// deleters (unless they are function pointers), while those that are passed by
// *value* are assumed to be function-like.  Note that if the wrong interface
// is used for a deleter, i.e., if a function-like deleter is passed by
// pointer, or a factory deleter is passed by value, and the expression used to
// delete the object is invalid, a compiler diagnostic will be emitted
// indicating the error.
//
// In general, deleters should have defined behavior when called with a null
// pointer.  In all cases, throwing an exception out of a copy constructor for
// a deleter will yield undefined behavior.
//
// The following are examples of constructing shared pointers with the
// addresses of factory deleters:
//..
//  my_Factory factory;
//  my_Type *myPtr1 = factory.createObject();
//  shared_ptr<my_Type> mySharedPtr1(myPtr1, &factory, 0);
//
//  bdema_SequentialAllocator sa;
//  my_Type *myPtr2 = new (sa) my_Type(&sa);
//  shared_ptr<my_Type> mySharedPtr2(myPtr2, &sa);
//..
// Note that the deleters are passed *by address* in the above examples.
//
// The following are examples of constructing shared pointers with
// function-like deleters:
//..
//  my_Type *getObject(bslma::Allocator *basicAllocator = 0);
//
//  my_Type *myPtr3 = getObject();
//  shared_ptr<my_Type> mySharedPtr3(myPtr3, &deleteObject);
//
//  my_Type *myPtr4 = getObject();
//  FunctionLikeDeleterObject deleter;
//  shared_ptr<my_Type> mySharedPtr4(myPtr4, deleter, &sa);
//..
// Note that 'deleteObject' is also passed by address, but 'deleter' is passed
// by value in the above examples.  Function-like deleter objects (passed by
// value) are stored by value in the representation and therefore *must* be
// copy-constructible.  Note that even though the deleter may be passed by
// reference, it is a copy (owned by the shared pointer representation) that is
// invoked and thus the 'deleterInstance' is not required, nor assumed, to be
// non-modifiable.  (For the example above, note that 'operator()' is
// intentionally *not* defined 'const'.)
//
///Aliasing
///--------
// 'shared_ptr' supports a powerful "aliasing" feature.  That is, a shared
// pointer can be constructed to refer to a shared object of a certain type
// while the shared pointer representation it holds refers to a shared object
// of any (possibly different) type.  All references are applied to the
// "aliased" shared object referred to by the representation and is used for
// reference counting.  This "aliased" shared object is passed to the deleter
// upon destruction of the last instance of that shared pointer.  Consider the
// following snippet of code:
//..
//  class Event { /* ... */ };
//  void getEvents(bsl::list<Event> *list);
//
//  void enqueueEvents(bcec_Queue<shared_ptr<Event> > *queue)
//  {
//      bsl::list<Event> eventList;
//      getEvents(&eventList);
//      for (bsl::list<Event>::iterator it = eventList.begin();
//           it != eventList.end();
//           ++it) {
//          shared_ptr<Event> e;
//          e.createInplace(0, *it);  // Copy construct the event into a new
//                                    // shared ptr.
//          queue->pushBack(e);
//      }
//  }
//..
// In the above example, 'getEvents' loads into the provided 'bsl::list' a
// sequence of event objects.  The 'enqueueEvents' function constructs an empty
// list and calls 'getEvents' to fill the list with 'Event' objects.  Once the
// event list is filled, each event item is pushed as a shared pointer
// (presumably because events are "expensive" to construct and may be
// referenced simultaneously from multiple threads) onto the provided queue.
// Since the individual event items are contained by value within the list,
// pointers to them cannot be passed if it cannot be guaranteed that they will
// not live beyond the lifetime of the list itself.  Therefore, an expensive
// copy operation is required to create individually-managed instances of each
// of the list items.  The 'createInplace' operation is used to reduce the
// number of required allocations, but this might still be too expensive.  Now
// consider the following alternate implementation of 'enqueueEvents' using the
// 'shared_ptr' aliasing feature:
//..
//  void enqueueEvents(bcec_Queue<shared_ptr<Event> > *queue)
//  {
//      shared_ptr<bsl::list<Event> > eventList;
//      eventList.createInplace(0);  // Construct a shared pointer
//                                   // to the event list containing
//                                   // all of the events.
//      getEvents(eventList.get());
//
//      for (bsl::list<Event>::iterator it = eventList->begin();
//           it != eventList->end();
//           ++it) {
//          // Push each event onto the queue as an alias of the 'eventList'
//          // shared pointer.  When all the alias references have been
//          // released, the event list will be destroyed deleting all the
//          // events at once.
//
//          queue->pushBack(shared_ptr<Event>(eventList, &*it));
//      }
//  }
//..
// In the implementation above, we create a single shared pointer to the
// 'Event' list, 'eventList', and use that to create 'Event' shared pointers
// that are aliased to 'eventList'.  The lifetime of each 'Event' object is
// then tied to the 'eventList' and it will not be destroyed until the
// 'eventList' is destroyed.
//
///Type Casting
///------------
// A 'shared_ptr' object of a given type can be implicitly or explicitly cast
// to a 'shared_ptr' of another type.
//
///Implicit Casting
/// - - - - - - - -
// As with native pointers, a shared pointer to a derived type can be directly
// assigned to a shared pointer to a base type.  In other words, if the
// following statements are valid:
//..
//  class A { virtual void foo(); };  // polymorphic type
//  class B : public A {};
//  B *bp = 0;
//  A *ap = bp;
//..
// then the following statements:
//..
//  shared_ptr<B> spb;
//  shared_ptr<A> spa;
//  spa = spb;
//..
// and:
//..
//  shared_ptr<B> spb;
//  shared_ptr<A> spa(spb);
//..
// are also valid.  Note that in all of the above cases, the destructor of 'B'
// will be invoked when the object is destroyed even if 'A' does not provide a
// virtual destructor.
//
///Explicit Casting
/// - - - - - - - -
// Through "aliasing", a shared pointer of any type can be explicitly cast to a
// shared pointer of any other type using any legal cast expression.  For
// example, to statically cast a shared pointer to type 'A' ('shared_ptr<A>')
// to a shared pointer to type 'B' ('shared_ptr<B>'), one can simply do the
// following:
//..
//  shared_ptr<A> spa;
//  shared_ptr<B> spb(spa, static_cast<B *>(spa.get()));
//..
// or even the less safe C-style cast:
//..
//  shared_ptr<A> spa;
//  shared_ptr<B> spb(spa, (B *)(spa.get()));
//..
// For convenience, several utility functions are provided to perform common
// C++ casts.  Dynamic casts, static casts, and 'const' casts are all provided.
// Explicit casting is supported through the 'bslstl::SharedPtrUtil' utility.
// The following example demonstrates the dynamic casting of a shared pointer
// to type 'A' ('shared_ptr<A>') to a shared pointer to type 'B'
// ('shared_ptr<B>'):
//..
//  bslma::NewDeleteAllocator nda;
//  shared_ptr<A> sp1(new (nda) A(), &nda);
//  shared_ptr<B> sp2 = bslstl::SharedPtrUtil::dynamicCast<B>(sp1);
//  shared_ptr<B> sp3;
//  bslstl::SharedPtrUtil::dynamicCast(&sp3, sp1);
//  shared_ptr<B> sp4;
//  sp4 = bslstl::SharedPtrUtil::dynamicCast<B>(sp1);
//..
// To test if the cast succeeded, simply test if the target shared pointer
// refers to a non-null value (assuming the source was not null, of course):
//..
//  if (sp2) {
//      // The cast succeeded.
//  } else {
//      // The cast failed.
//  }
//..
// As previously stated, the shared object will be destroyed correctly
// regardless of how it is cast.
//
///Converting to and from 'BloombergLP::bslma::ManagedPtr'
///-------------------------------------------------------
// A 'shared_ptr' can be converted to a 'BloombergLP::bslma::ManagedPtr' while
// still retaining proper reference counting.  When a shared pointer is
// converted to a 'BloombergLP::bslma::ManagedPtr', the number of references to
// the shared object is incremented.  When the managed pointer is destroyed (if
// not transferred to another managed pointer first), the number of references
// will be decremented.  If the number of references reaches zero, then the
// shared object will be destroyed.  The 'managedPtr' function can be used to
// create a managed pointer from a shared pointer.
//
// A 'shared_ptr' also can be constructed from a
// 'BloombergLP::bslma::ManagedPtr'.  The resulting shared pointer takes over
// the management of the object and will use the deleter from the original
// 'BloombergLP::bslma::ManagedPtr' to destroy the managed object when all the
// references to that shared object are released.
//
///Weak Pointers using "in-place" or Pooled Shared Pointer Representations
///-----------------------------------------------------------------------
// A weak pointer that is not in the empty state shares a common representation
// (used to refer to the shared object) with the shared (or other weak) pointer
// from which it was constructed, and holds this representation until it is
// either destroyed or reset.  This common representation is not destroyed and
// deallocated (although the shared object itself may have been destroyed)
// until all weak references to that common representation have been released.
//
// Due to this behavior the memory footprint of shared objects that are
// constructed "in-place" in the shared pointer representation (refer to the
// component-level documentation of 'bsl::shared_ptr' for more information on
// shared pointers with "in-place" representations) is not deallocated until
// all weak references to that shared object are released.  Note that a shared
// object is always destroyed when the last shared reference to it is released.
// Also note that the same behavior is applicable if the shared objects were
// obtained from a class that pools shared pointer representations (for
// example, 'bcec_SharedObjectPool').
//
// For example suppose we have a class with a large memory footprint:
//..
//  class ClassWithLargeFootprint {
//      // This class has a large memory footprint.
//
//      // TYPES
//      enum { BUFFER_SIZE = 1024 };
//          // The size of the buffer owned by this 'class'.
//
//      // DATA
//      char d_buffer[BUFFER_SIZE];
//
//      // ...
//  };
//..
// We then create an "in-place" shared pointer to an object of
// 'ClassWithLargeFootprint' using the 'createInplace' method of
// 'bsl::shared_ptr'.  The 'sp' shared pointer representation of 'sp' will
// create a 'ClassWithLargeFootprint' object "in-place":
//..
//  bsl::shared_ptr<ClassWithLargeFootprint> sp;
//  sp.createInplace();
//..
// Next we construct a weak pointer from this (in-place) shared pointer:
//..
//  bsl::weak_ptr<ClassWithLargeFootprint> wp(sp);
//..
// Now releasing all shared references to the shared object (using the 'reset'
// function) causes the object's destructor to be called, but the
// representation is not destroyed (and the object's footprint is not
// deallocated) until 'wp' releases its weak reference:
//..
//  sp.reset(); // The object's footprint is not deallocated until all weak
//              // references to it are released.
//
//  wp.reset(); // The release of the *last* weak reference results in the
//              // destruction and deallocation of the representation and the
//              // object's footprint.
//..
// If a shared object has a large footprint, and the client anticipates there
// will be weak references to it, then it may be advisable to create an
// out-of-place shared pointer representation, which destroys the shared object
// and deallocates its footprint when the last *shared* reference to it is
// released, regardless of whether there are any outstanding weak references to
// the same representation.
//
///C++ Standard Compliance
///-----------------------
// This component provides an (extended) standard-compliant implementation of
// 'std::shared_ptr' and 'std::weak_ptr' (section 20.7.2, [util.smartptr], of
// the ISO C++11 standard)).  However, it does not support the atomic shared
// pointer interface, nor provide the C++17 interface for 'shared_ptr' of an
// array type.  When using a C++03 compiler, its interface is limited to the
// set of operations that can be implemented by an implementation of the C++03
// language, e,g., there are no exception specifications, nor 'constexpr'
// constructors, and move operations are emulated with 'bslmf::MovableRef'.
//
// In addition to the standard interface, this component supports allocators
// following the 'bslma::Allocator' protocol in addition to the C++ Standard
// Allocators (section 17.6.3.5, [allocator.requirements]), supports "factory"
// style deleters in addition to function-like deleters, and interoperation
// with 'bslma::ManagedPtr' smart pointers.
//
///Usage
///-----
// The following examples demonstrate various features and uses of shared
// pointers.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// The following example demonstrates the creation of a shared pointer.  First,
// we declare the type of object that we wish to manage:
//..
//  class MyUser {
//      // DATA
//      bsl::string d_name;
//      int         d_id;
//
//    public:
//      // CREATORS
//      MyUser(bslma::Allocator *alloc = 0) : d_name(alloc), d_id(0) {}
//      MyUser(const bsl::string& name, int id, bslma::Allocator *alloc = 0)
//      : d_name(name, alloc)
//      , d_id(id)
//      {
//      }
//      MyUser(const MyUser& original, bslma::Allocator *alloc = 0)
//      : d_name(original.d_name, alloc)
//      , d_id(original.d_id)
//      {
//      }
//
//      // MANIPULATORS
//      void setName(const bsl::string& name) { d_name = name; }
//      void setId(int id) { d_id = id; }
//
//      // ACCESSORS
//      const bsl::string& name() const { return d_name; }
//      int id() const { return d_id; }
//  };
//..
// The 'createUser' utility function (below) creates a 'MyUser' object using
// the provided allocator and returns a shared pointer to the newly-created
// object.  Note that the shared pointer's internal representation will also be
// allocated using the same allocator.  Also note that if 'allocator' is 0, the
// currently-installed default allocator is used.
//..
//  shared_ptr<MyUser> createUser(bsl::string       name,
//                                     int               id,
//                                     bslma::Allocator *allocator = 0)
//  {
//      allocator = bslma::Default::allocator(allocator);
//      MyUser *user = new (*allocator) MyUser(name, id, allocator);
//      return shared_ptr<MyUser>(user, allocator);
//  }
//..
// Since the 'createUser' function both allocates the object and creates the
// shared pointer, it can benefit from the in-place facilities to avoid an
// extra allocation.  Again, note that the representation will also be
// allocated using the same allocator (see the section "Correct Usage of the
// Allocator Model" above).  Also note that if 'allocator' is 0, the
// currently-installed default allocator is used.
//..
//  shared_ptr<MyUser> createUser2(bsl::string       name,
//                                      int               id,
//                                      bslma::Allocator *allocator = 0)
//  {
//      shared_ptr<MyUser> user;
//      user.createInplace(allocator, name, id, allocator);
//      return user;
//  }
//..
// Note that the shared pointer allocates both the reference count and the
// 'MyUser' object in a single region of memory (which is the memory that will
// eventually be deallocated), but refers to the 'MyUser' object only.
//
///Using Custom Deleters
///- - - - - - - - - - -
// The following examples demonstrate the use of custom deleters with shared
// pointers.
//
///Example 2: Nil Deleters
/// -  -  -  -  -  -  -  -
// There are cases when an interface calls for an object to be passed as a
// shared pointer, but the object being passed is not owned by the caller
// (e.g., a pointer to a static variable).  In these cases, it is possible to
// create a shared pointer specifying 'bslstl::SharedPtrNilDeleter' as the
// deleter.  The deleter function provided by 'bslstl::SharedPtrNilDeleter' is
// a no-op and does not delete the object.  The following example demonstrates
// the use of 'shared_ptr' using a 'bslstl::SharedPtrNilDeleter'.  The code
// uses the 'MyUser' class defined in Example 1.  In this example, an
// asynchronous transaction manager is implemented.  Transactions are enqueued
// into the transaction manager to be processed at some later time.  The user
// associated with the transaction is passed as a shared pointer.  Transactions
// can originate from the "system" or from "users".
//
// We first declare the transaction manager and transaction info classes:
//..
//  class MyTransactionInfo {
//      // Transaction Info...
//  };
//
//  class MyTransactionManager {
//
//      // PRIVATE MANIPULATORS
//      int enqueueTransaction(shared_ptr<MyUser>  user,
//                             const MyTransactionInfo& transaction);
//    public:
//      // CLASS METHODS
//      static MyUser *systemUser(bslma::Allocator *basicAllocator = 0);
//
//      // MANIPULATORS
//      int enqueueSystemTransaction(const MyTransactionInfo& transaction);
//
//      int enqueueUserTransaction(const MyTransactionInfo& transaction,
//                                 shared_ptr<MyUser>  user);
//
//  };
//..
// The 'systemUser' class method returns the same 'MyUser' object and should
// not be destroyed by its users:
//..
//  MyUser *MyTransactionManager::systemUser(
//                                     bslma::Allocator * /* basicAllocator */)
//  {
//      static MyUser *systemUserSingleton;
//      if (!systemUserSingleton) {
//          // instantiate singleton in a thread-safe manner passing
//          // 'basicAllocator'
//
//          // . . .
//      }
//      return systemUserSingleton;
//  }
//..
// For enqueuing user transactions, simply proxy the information to
// 'enqueueTransaction'.
//..
//  inline
//  int MyTransactionManager::enqueueUserTransaction(
//                                  const MyTransactionInfo& transaction,
//                                  shared_ptr<MyUser>  user)
//  {
//      return enqueueTransaction(user, transaction);
//  }
//..
// For system transactions, we must use the 'MyUser' objected returned from the
// 'systemUser' 'static' method.  Since we do not own the returned object, we
// cannot directly construct a 'shared_ptr' object for it: doing so would
// result in the singleton being destroyed when the last reference to the
// shared pointer is released.  To solve this problem, we construct a
// 'shared_ptr' object for the system user using a nil deleter.  When the last
// reference to the shared pointer is released, although the deleter will be
// invoked to destroy the object, it will do nothing.
//..
//  int MyTransactionManager::enqueueSystemTransaction(
//                                        const MyTransactionInfo& transaction)
//  {
//      shared_ptr<MyUser> user(systemUser(),
//                                   bslstl::SharedPtrNilDeleter(),
//                                   0);
//      return enqueueTransaction(user, transaction);
//  }
//..
//
///Example 3: Basic Weak Pointer Usage
///- - - - - - - - - - - - - - - - - -
// This example illustrates the basic syntax needed to create and use a
// 'bsl::weak_ptr'.  Suppose that we want to construct a weak pointer that
// refers to an 'int' managed by a shared pointer.  Next we define the shared
// pointer and assign a value to the shared 'int':
//..
//  bsl::shared_ptr<int> intPtr;
//  intPtr.createInplace(bslma::Default::allocator());
//  *intPtr = 10;
//  assert(10 == *intPtr);
//..
// Next we construct a weak pointer to the 'int':
//..
//  bsl::weak_ptr<int> intWeakPtr(intPtr);
//  assert(!intWeakPtr.expired());
//..
// 'bsl::weak_ptr' does not provide direct access to the shared object being
// referenced.  To access and manipulate the 'int' from the weak pointer, we
// have to obtain a shared pointer from it:
//..
//  bsl::shared_ptr<int> intPtr2 = intWeakPtr.lock();
//  assert(intPtr2);
//  assert(10 == *intPtr2);
//
//  *intPtr2 = 20;
//  assert(20 == *intPtr);
//  assert(20 == *intPtr2);
//..
// We remove the weak reference to the shared 'int' by calling the 'reset'
// method:
//..
//  intWeakPtr.reset();
//  assert(intWeakPtr.expired());
//..
// Note that resetting the weak pointer does not affect the shared pointers
// referencing the 'int' object:
//..
//  assert(20 == *intPtr);
//  assert(20 == *intPtr2);
//..
// Now, we construct another weak pointer referencing the shared 'int':
//..
//  bsl::weak_ptr<int> intWeakPtr2(intPtr);
//  assert(!intWeakPtr2.expired());
//..
// Finally 'reset' all shared references to the 'int', which will cause the
// weak pointer to become "expired"; any subsequent attempt to obtain a shared
// pointer from the weak pointer will return a shared pointer in the default
// constructed (empty) state:
//..
//  intPtr.reset();
//  intPtr2.reset();
//  assert(intWeakPtr2.expired());
//  assert(!intWeakPtr2.lock());
//..
//
///Example 4: Breaking Cyclical Dependencies
///- - - - - - - - - - - - - - - - - - - - -
// Weak pointers are frequently used to break cyclical dependencies between
// objects that store references to each other via a shared pointer.  Consider
// for example a simplified news alert system that sends news alerts to users
// based on keywords that they register for.  The user information is stored in
// the 'User' class and the details of the news alert are stored in the 'Alert'
// class.  The class definitions for 'User' and 'Alert' are provided below
// (with any code not relevant to this example elided):
//..
//  class Alert;
//
//  class User {
//      // This class stores the user information required for listening to
//      // alerts.
//
//      bsl::vector<bsl::shared_ptr<Alert> > d_alerts;  // alerts user is
//                                                      // registered for
//
//      // ...
//
//    public:
//      // MANIPULATORS
//      void addAlert(const bsl::shared_ptr<Alert>& alertPtr)
//      {
//          // Add the specified 'alertPtr' to the list of alerts being
//          // monitored by this user.
//
//          d_alerts.push_back(alertPtr);
//      }
//
//      // ...
//  };
//..
// Now we define an alert class, 'Alert':
//..
//  class Alert {
//      // This class stores the alert information required for sending
//      // alerts.
//
//      bsl::vector<bsl::shared_ptr<User> > d_users;  // users registered
//                                                    // for this alert
//
//    public:
//      // MANIPULATORS
//      void addUser(const bsl::shared_ptr<User>& userPtr)
//      {
//          // Add the specified 'userPtr' to the list of users monitoring this
//          // alert.
//
//          d_users.push_back(userPtr);
//      }
//
//      // ...
//  };
//
//..
// Even though we have released 'alertPtr' and 'userPtr' there still exists a
// cyclic reference between the two objects, so none of the objects are
// destroyed.
//
// We can break this cyclical dependency we define a modified alert class
// 'ModifiedAlert' that stores a weak pointer to a 'ModifiedUser' object.
// Below is the definition for the 'ModifiedUser' class that is identical to
// the 'User' class, the only difference being that it stores shared pointer to
// 'ModifiedAlert's instead of 'Alert's:
//..
//  class ModifiedAlert;
//
//  class ModifiedUser {
//      // This class stores the user information required for listening to
//      // alerts.
//
//      bsl::vector<bsl::shared_ptr<ModifiedAlert> > d_alerts;// alerts user is
//                                                            // registered for
//
//      // ...
//
//    public:
//      // MANIPULATORS
//      void addAlert(const bsl::shared_ptr<ModifiedAlert>& alertPtr)
//      {
//          // Add the specified 'alertPtr' to the list of alerts being
//          // monitored by this user.
//
//          d_alerts.push_back(alertPtr);
//      }
//
//      // ...
//  };
//..
// Now we define the 'ModifiedAlert' class:
//..
//  class ModifiedAlert {
//      // This class stores the alert information required for sending
//      // alerts.
//
//..
// Note that the user is stored by a weak pointer instead of by a shared
// pointer:
//..
//      bsl::vector<bsl::weak_ptr<ModifiedUser> > d_users;  // users registered
//                                                          // for this alert
//
//    public:
//      // MANIPULATORS
//      void addUser(const bsl::weak_ptr<ModifiedUser>& userPtr)
//      {
//          // Add the specified 'userPtr' to the list of users monitoring this
//          // alert.
//
//          d_users.push_back(userPtr);
//      }
//
//      // ...
//  };
//..
//
///Example 5: Caching
/// - - - - - - - - -
// Suppose we want to implement a peer to peer file sharing system that allows
// users to search for files that match specific keywords.  A simplistic
// version of such a system with code not relevant to the usage example elided
// would have the following parts:
//
// a) A peer manager class that maintains a list of all connected peers and
// updates the list based on incoming peer requests and disconnecting peers.
// The following would be a simple interface for the Peer and PeerManager
// classes:
//..
//  class Peer {
//      // This class stores all the relevant information for a peer.
//
//      // ...
//  };
//
//  class PeerManager {
//      // This class acts as a manager of peers and adds and removes peers
//      // based on peer requests and disconnections.
//
//      // DATA
//..
// The peer objects are stored by shared pointer to allow peers to be passed to
// search results and still allow their asynchronous destruction when peers
// disconnect.
//..
//      bsl::map<int, bsl::shared_ptr<Peer> > d_peers;
//
//      // ...
//  };
//..
// b) A peer cache class that stores a subset of the peers that are used for
// sending search requests.  The cache may select peers based on their
// connection bandwidth, relevancy of previous search results, etc.  For
// brevity the population and flushing of this cache is not shown:
//..
//  class PeerCache {
//      // This class caches a subset of all peers that match certain criteria
//      // including connection bandwidth, relevancy of previous search
//      // results, etc.
//
//..
// Note that the cached peers are stored as a weak pointer so as not to
// interfere with the cleanup of Peer objects by the PeerManager if a Peer goes
// down.
//..
//      // DATA
//      bsl::list<bsl::weak_ptr<Peer> > d_cachedPeers;
//
//    public:
//      // TYPES
//      typedef bsl::list<bsl::weak_ptr<Peer> >::const_iterator PeerConstIter;
//
//      // ...
//
//      // ACCESSORS
//      PeerConstIter begin() const { return d_cachedPeers.begin(); }
//      PeerConstIter end() const   { return d_cachedPeers.end(); }
//  };
//..
// c) A search result class that stores a search result and encapsulates a peer
// with the file name stored by the peer that best matches the specified
// keywords:
//..
//  class SearchResult {
//      // This class provides a search result and encapsulates a particular
//      // peer and filename combination that matches a specified set of
//      // keywords.
//
//..
// The peer is stored as a weak pointer because when the user decides to select
// a particular file to download from this peer, the peer might have
// disconnected.
//..
//      // DATA
//      bsl::weak_ptr<Peer> d_peer;
//      bsl::string         d_filename;
//
//    public:
//      // CREATORS
//      SearchResult(const bsl::weak_ptr<Peer>& peer,
//                   const bsl::string&         filename)
//      : d_peer(peer)
//      , d_filename(filename)
//      {
//      }
//
//      // ...
//
//      // ACCESSORS
//      const bsl::weak_ptr<Peer>& peer() const { return d_peer; }
//      const bsl::string& filename() const { return d_filename; }
//  };
//..
// d) A search function that takes a list of keywords and returns available
// results by searching the cached peers:
//..
//  void search(bsl::vector<SearchResult>       * /* results */,
//              const PeerCache&                 peerCache,
//              const bsl::vector<bsl::string>&  /* keywords */)
//  {
//      for (PeerCache::PeerConstIter iter = peerCache.begin();
//           iter != peerCache.end();
//           ++iter) {
//..
// First we check if the peer is still connected by acquiring a shared pointer
// to the peer.  If the acquire operation succeeds, then we can send the peer a
// request to send back the file best matching the specified keywords:
//..
//          bsl::shared_ptr<Peer> peerSharedPtr = iter->lock();
//          if (peerSharedPtr) {
//
//              // Search the peer for file best matching the specified
//              // keywords and if a file is found add the returned
//              // SearchResult object to result.
//
//              // ...
//          }
//      }
//  }
//..
// e) A download function that downloads a file selected by the user:
//..
//  void download(const SearchResult& result)
//  {
//      bsl::shared_ptr<Peer> peerSharedPtr = result.peer().lock();
//      if (peerSharedPtr) {
//          // Download the result.filename() file from peer knowing that
//          // the peer is still connected.
//      }
//  }
//..
//
///Example 6: Custom Deleters
/// -  -  -  -  -  -  -  -  -
// The role of a "deleter" is to allow users to define a custom "cleanup" for a
// shared object.  Although cleanup generally involves destroying the object,
// this need not be the case.  The following example demonstrates the use of a
// custom deleter to construct "locked" pointers.  First we declare a custom
// deleter that, when invoked, releases the specified mutex and signals the
// specified condition variable.
//..
//  class my_MutexUnlockAndBroadcastDeleter {
//
//      // DATA
//      bslmt::Mutex     *d_mutex_p;  // mutex to lock (held, not owned)
//      bslmt::Condition *d_cond_p;   // condition variable used to broadcast
//                                    // (held, not owned)
//
//    public:
//      // CREATORS
//      my_MutexUnlockAndBroadcastDeleter(bslmt::Mutex     *mutex,
//                                        bslmt::Condition *cond)
//          // Create this 'my_MutexUnlockAndBroadcastDeleter' object.  Use
//          // the specified 'cond' to broadcast a signal and the specified
//          // 'mutex' to serialize access to 'cond'.  The behavior is
//          // undefined unless 'mutex' is not 0 and 'cond' is not 0.
//      : d_mutex_p(mutex)
//      , d_cond_p(cond)
//      {
//          BSLS_ASSERT(mutex);
//          BSLS_ASSERT(cond);
//
//          d_mutex_p->lock();
//      }
//
//      my_MutexUnlockAndBroadcastDeleter(
//                                 my_MutexUnlockAndBroadcastDeleter& original)
//      : d_mutex_p(original.d_mutex_p)
//      , d_cond_p(original.d_cond_p)
//      {
//      }
//..
// Since this deleter does not actually delete anything, 'void *' is used in
// the signature of 'operator()', allowing it to be used with any type of
// object.
//..
//      void operator()(void *)
//      {
//          d_cond_p->broadcast();
//          d_mutex_p->unlock();
//      }
//  };
//..
// Next we declare a thread-safe queue 'class'.  The 'class' uses a
// non-thread-safe 'bsl::deque' to implement the queue.  Thread-safe 'push' and
// 'pop' operations that push and pop individual items are provided.  For
// callers that wish to gain direct access to the queue, the 'queue' method
// returns a shared pointer to the queue using the
// 'my_MutexUnlockAndBroadcastDeleter'.  Callers can safely access the queue
// through the returned shared pointer.  Once the last reference to the pointer
// is released, the mutex will be unlocked and the condition variable will be
// signaled to allow waiting threads to re-evaluate the state of the queue.
//..
//  template <class ELEMENT_TYPE>
//  class my_SafeQueue {
//
//      // DATA
//      bslmt::Mutex             d_mutex;
//      bslmt::Condition         d_cond;
//      bsl::deque<ELEMENT_TYPE> d_queue;
//
//      // . . .
//
//    public:
//      // MANIPULATORS
//      void push(const ELEMENT_TYPE& obj);
//
//      ELEMENT_TYPE pop();
//
//      shared_ptr<bsl::deque<ELEMENT_TYPE> > queue();
//  };
//
//  template <class ELEMENT_TYPE>
//  void my_SafeQueue<ELEMENT_TYPE>::push(const ELEMENT_TYPE& obj)
//  {
//      bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
//      d_queue.push_back(obj);
//      d_cond.signal();
//  }
//
//  template <class ELEMENT_TYPE>
//  ELEMENT_TYPE my_SafeQueue<ELEMENT_TYPE>::pop()
//  {
//      bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
//      while (!d_queue.size()) {
//         d_cond.wait(&d_mutex);
//      }
//      ELEMENT_TYPE value(d_queue.front());
//      d_queue.pop_front();
//      return value;
//  }
//
//  template <class ELEMENT_TYPE>
//  shared_ptr<bsl::deque<ELEMENT_TYPE> >
//  my_SafeQueue<ELEMENT_TYPE>::queue()
//  {
//      return shared_ptr<bsl::deque<ELEMENT_TYPE> >(
//                         &d_queue,
//                         MyMutexUnlockAndBroadcastDeleter(&d_mutex, &d_cond),
//                         0);
//  }
//..
//
///Implementation Hiding
///- - - - - - - - - - -
// 'shared_ptr' refers to the template parameter type on which it is
// instantiated "in name only".  This allows for the instantiation of shared
// pointers to incomplete or 'void' types.  This feature is useful for
// constructing interfaces where returning a pointer to a shared object is
// desirable, but in order to control access to the object its interface cannot
// be exposed.  The following examples demonstrate two techniques for achieving
// this goal using a 'shared_ptr'.
//
///Example 7: Hidden Interfaces
/// - - - - - - - - - - - - - -
// Example 7 demonstrates the use of incomplete types to hide the interface of
// a 'my_Session' type.  We begin by declaring the 'my_SessionManager' 'class',
// which allocates and manages 'my_Session' objects.  The interface ('.h')
// merely forward declares 'my_Session'.  The actual definition of the
// interface is in the implementation ('.cpp') file.
//
// We forward-declare 'my_Session' to be used (in name only) in the definition
// of 'my_SessionManager':
//..
//  class my_Session;
//..
// Next, we define the 'my_SessionManager' class:
//..
//  class my_SessionManager {
//
//      // TYPES
//      typedef bsl::map<int, shared_ptr<my_Session> > HandleMap;
//
//      // DATA
//      bslmt::Mutex      d_mutex;
//      HandleMap         d_handles;
//      int               d_nextSessionId;
//      bslma::Allocator *d_allocator_p;
//
//..
// It is useful to have a designated name for the 'shared_ptr' to 'my_Session':
//..
//    public:
//      // TYPES
//      typedef shared_ptr<my_Session> my_Handle;
//..
// We need only a default constructor:
//..
//      // CREATORS
//      my_SessionManager(bslma::Allocator *allocator = 0);
//..
// The 3 methods that follow construct a new session object and return a
// 'shared_ptr' to it.  Callers can transfer the pointer, but they cannot
// directly access the object's methods since they do not have access to its
// interface.
//..
//      // MANIPULATORS
//      my_Handle openSession(const bsl::string& sessionName);
//      void closeSession(my_Handle handle);
//
//      // ACCESSORS
//      bsl::string getSessionName(my_Handle handle) const;
//  };
//..
// Now, in the implementation of the code, we can define and implement the
// 'my_Session' class:
//..
//  class my_Session {
//
//      // DATA
//      bsl::string d_sessionName;
//      int         d_handleId;
//
//    public:
//      // CREATORS
//      my_Session(const bsl::string&  sessionName,
//                 int                 handleId,
//                 bslma::Allocator   *basicAllocator = 0);
//
//      // ACCESSORS
//      int handleId() const;
//      const bsl::string& sessionName() const;
//  };
//
//  // CREATORS
//  inline
//  my_Session::my_Session(const bsl::string&  sessionName,
//                         int                 handleId,
//                         bslma::Allocator   *basicAllocator)
//  : d_sessionName(sessionName, basicAllocator)
//  , d_handleId(handleId)
//  {
//  }
//
//  // ACCESSORS
//  inline
//  int my_Session::handleId() const
//  {
//      return d_handleId;
//  }
//
//  inline
//  const bsl::string& my_Session::sessionName() const
//  {
//      return d_sessionName;
//  }
//..
// The following shows the implementation of 'my_SessionManager'.  Note that
// the interface for 'my_Session' is not known:
//..
//  inline
//  my_SessionManager::my_SessionManager(bslma::Allocator *allocator)
//  : d_nextSessionId(1)
//  , d_allocator_p(bslma::Default::allocator(allocator))
//  {
//  }
//
//  inline
//  my_SessionManager::my_Handle
//  my_SessionManager::openSession(const bsl::string& sessionName)
//  {
//      bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
//      my_Handle session(new (*d_allocator_p) my_Session(sessionName,
//                                                        d_nextSessionId++,
//                                                        d_allocator_p));
//      d_handles[session->handleId()] = session;
//      return session;
//  }
//
//  inline
//  void my_SessionManager::closeSession(my_Handle handle)
//  {
//      bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
//      HandleMap::iterator it = d_handles.find(handle->handleId());
//      if (it != d_handles.end()) {
//          d_handles.erase(it);
//      }
//  }
//
//  inline
//  bsl::string my_SessionManager::getSessionName(my_Handle handle) const
//  {
//      return handle->sessionName();
//  }
//..
//
///Example 8: Opaque Types
/// -  -  -  -  -  -  -  -
// In the above example, users could infer that 'my_Handle' is a pointer to a
// 'my_Session' but have no way to directly access it's methods since the
// interface is not exposed.  In the following example, 'my_SessionManager' is
// re-implemented to provide an even more opaque session handle.  In this
// implementation, 'my_Handle' is redefined using 'void' providing no
// indication of its implementation.  Note that using 'void' will require
// casting in the implementation and, therefore, will be a little more
// expensive.
//
// In the interface, define 'my_SessionManager' as follows:
//..
//  class my_SessionManager {
//
//      // TYPES
//      typedef bsl::map<int, shared_ptr<void> > HandleMap;
//
//      // DATA
//      bslmt::Mutex      d_mutex;
//      HandleMap         d_handles;
//      int               d_nextSessionId;
//      bslma::Allocator *d_allocator_p;
//..
// It is useful to have a name for the 'void' 'shared_ptr' handle.
//..
//     public:
//      // TYPES
//      typedef shared_ptr<void> my_Handle;
//
//      // CREATORS
//      my_SessionManager(bslma::Allocator *allocator = 0);
//
//      // MANIPULATORS
//      my_Handle openSession(const bsl::string& sessionName);
//      void closeSession(my_Handle handle);
//
//      // ACCESSORS
//      bsl::string getSessionName(my_Handle handle) const;
//  };
//..
// Next we define the methods of 'my_SessionManager':
//..
//  // CREATORS
//  inline
//  my_SessionManager::my_SessionManager(bslma::Allocator *allocator)
//  : d_nextSessionId(1)
//  , d_allocator_p(bslma::Default::allocator(allocator))
//  {
//  }
//
//  // MANIPULATORS
//  inline
//  my_SessionManager::my_Handle
//  my_SessionManager::openSession(const bsl::string& sessionName)
//  {
//      bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
//..
// Notice that 'my_Handle', which is a shared pointer to 'void', can be
// transparently assigned to a shared pointer to a 'my_Session' object.  This
// is because the 'shared_ptr' interface allows shared pointers to types that
// can be cast to one another to be assigned directly.
//..
//      my_Handle session(new (*d_allocator_p) my_Session(sessionName,
//                                                        d_nextSessionId++,
//                                                        d_allocator_p));
//      shared_ptr<my_Session> myhandle =
//                      bslstl::SharedPtrUtil::staticCast<my_Session>(session);
//      d_handles[myhandle->handleId()] = session;
//      return session;
//  }
//
//  inline
//  void my_SessionManager::closeSession(my_Handle handle)
//  {
//      bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
//..
// Perform a static cast from 'shared_ptr<void>' to 'shared_ptr<my_Session>'.
//..
//      shared_ptr<my_Session> myhandle =
//                       bslstl::SharedPtrUtil::staticCast<my_Session>(handle);
//..
// Test to make sure that the pointer is non-null before using 'myhandle':
//..
//      if (!myhandle.get()) {
//          return;                                                   // RETURN
//      }
//
//      HandleMap::iterator it = d_handles.find(myhandle->handleId());
//      if (it != d_handles.end()) {
//          d_handles.erase(it);
//      }
//  }
//
//  bsl::string my_SessionManager::getSessionName(my_Handle handle) const
//  {
//      shared_ptr<my_Session> myhandle =
//                       bslstl::SharedPtrUtil::staticCast<my_Session>(handle);
//
//      if (!myhandle.get()) {
//          return bsl::string();
//      } else {
//          return myhandle->sessionName();
//      }
//  }
//..

#include <bslscm_version.h>

#include <bslstl_hash.h>
#include <bslstl_pair.h>
#include <bslstl_referencewrapper.h>
#include <bslstl_sharedptrallocateinplacerep.h>
#include <bslstl_sharedptrallocateoutofplacerep.h>

#include <bslma_allocator.h>
#include <bslma_allocatortraits.h>
#include <bslma_default.h>
#include <bslma_managedptr.h>
#include <bslma_sharedptrinplacerep.h>
#include <bslma_sharedptroutofplacerep.h>
#include <bslma_sharedptrrep.h>
#include <bslma_stdallocator.h>

#include <bslmf_addlvaluereference.h>
#include <bslmf_addpointer.h>
#include <bslmf_conditional.h>
#include <bslmf_enableif.h>
#include <bslmf_haspointersemantics.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isarray.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isconvertible.h>
#include <bslmf_isfunction.h>
#include <bslmf_ispointer.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_util.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_deprecatefeature.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nullptr.h>
#include <bsls_platform.h>
#include <bsls_unspecifiedbool.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#include <functional>           // use 'std::less' to order pointers

#include <memory>               // 'std::auto_ptr', 'std::unique_ptr'

#include <ostream>              // 'std::basic_ostream'

#include <stddef.h>             // 'size_t', 'ptrdiff_t'

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Wed Oct 26 08:50:25 2022
// Command line: sim_cpp11_features.pl bslstl_sharedptr.h
# define COMPILING_BSLSTL_SHAREDPTR_H
# include <bslstl_sharedptr_cpp03.h>
# undef COMPILING_BSLSTL_SHAREDPTR_H
#else

#if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
        // Here and throughout the file wherever 'auto_ptr' is used, suspend
        // GCC reporting of deprecated declarations since the use of 'auto_ptr'
        // in this standard interface is required.
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS) \
 && (!defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VERSION >= 1900)
# define BSLSTL_SHAREDPTR_SUPPORTS_SFINAE_CHECKS 1

#if BSLS_PLATFORM_CMP_VERSION >= 1910 &&                                      \
    BSLS_PLATFORM_CMP_VERSION <  1920 &&                                      \
    BSLS_COMPILERFEATURES_CPLUSPLUS >= 201703L
// Visual Studio 2017 in C++17 mode crashes with an internal compiler error on
// the shared pointer SFINAE code.  See {DRQS 148281696}.
# undef BSLSTL_SHAREDPTR_SUPPORTS_SFINAE_CHECKS
#endif

// If the macro 'BSLSTL_SHAREDPTR_SUPPORTS_SFINAE_CHECKS' is defined, then a
// conforming C++11 compiler will define the constructors in this component in
// such a way that they will not be selected during overload resolution unless
// they would instantiate correctly.  This means that code depending on the
// result of 'is_constructible' and similar traits will have the expected
// behavior.  There is no attempt to support this feature in C++03.
//
// Support for SFINAE-queries on the constructability of a 'shared_ptr' depend
// on a variety of C++11 language features, including "expression-SFINAE".
// However, the main language feature that enables SFINAE elimination of a
// constructor is the ability to use default template arguments in a function
// template.  It is significantly preferred to use the template parameter list,
// rather than add additional default arguments to the constructor signatures,
// as there are so many constructor overloads in this component that there is a
// real risk of introducing ambiguities that would need to be worked around.
// Therefore, the 'BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS' macro
// serves as our proxy for whether SFINAE-constructors are enabled in this
// component.  Note that the MSVC 2015 compiler almost supported
// "expression-SFINAE", to the extent that it works for this component, unlike
// earlier versions of that compiler.  We therefore make a special version-test
// on Microsoft in addition to the feature testing.
#endif

# if defined(BSLS_PLATFORM_CMP_GNU)
# define BSLSTL_SHAREDPTR_NO_PARTIAL_ORDER_ON_ALLOCATOR_POINTER 1
// If the macro 'BSLSTL_SHAREDPTR_NO_PARTIAL_ORDER_ON_ALLOCATOR_POINTER' is
// defined, we recognize that some compilers need an extra hint to disambiguate
// overload resolution when passed a 'bslma::Allocator *' pointer, that might
// also deduce (incorrectly) as a C++11-style allocator.  Gcc is known to have
// this problem, and was tested as recently as gcc 9. This compiler has a
// problem partially ordering function templates that differ only by the first
// argument deducing as any object type ('T'), or deducing as the a pointer to
// something ('T*').  The rules for partial ordering should make the second
// overload a stronger match when passed a pointer; however, this compiler
// complains about ambiguities when additional parameters are involved.  This
// appears to be fixed in gcc 10.
#endif

#if defined(BSLSTL_SHAREDPTR_SUPPORTS_SFINAE_CHECKS)
// Note the intentional comma in the first line of the definition of each
// macro, which allows these macros to be applied incrementally, even when the
// alternate definition is empty.  This avoids the problem of introducing new
// template parameters along with the macros in the non-SFINAE-supporting case
// below.
# define BSLSTL_SHAREDPTR_DECLARE_IF_CONVERTIBLE ,                            \
    typename enable_if<                                                       \
        BloombergLP::bslstl::SharedPtr_IsPointerConvertible<                  \
                            CONVERTIBLE_TYPE,                                 \
                            ELEMENT_TYPE>::value>::type *                     \
                                                                = nullptr

# define BSLSTL_SHAREDPTR_DEFINE_IF_CONVERTIBLE ,                             \
    typename enable_if<                                                       \
        BloombergLP::bslstl::SharedPtr_IsPointerConvertible<                  \
                            CONVERTIBLE_TYPE,                                 \
                            ELEMENT_TYPE>::value>::type *


# define BSLSTL_SHAREDPTR_DECLARE_IF_COMPATIBLE ,                             \
    typename enable_if<                                                       \
        BloombergLP::bslstl::SharedPtr_IsPointerCompatible<                   \
                            COMPATIBLE_TYPE,                                  \
                            ELEMENT_TYPE>::value>::type *                     \
                                                                = nullptr

# define BSLSTL_SHAREDPTR_DEFINE_IF_COMPATIBLE ,                              \
    typename enable_if<                                                       \
        BloombergLP::bslstl::SharedPtr_IsPointerCompatible<                   \
                            COMPATIBLE_TYPE,                                  \
                            ELEMENT_TYPE>::value>::type *


# define BSLSTL_SHAREDPTR_DECLARE_IF_DELETER(FUNCTOR, ARGUMENT) ,             \
    typename enable_if<                                                       \
      BloombergLP::bslstl::SharedPtr_IsCallable  <FUNCTOR,                    \
                                                  ARGUMENT *>::k_VALUE ||     \
      BloombergLP::bslstl::SharedPtr_IsFactoryFor<FUNCTOR,                    \
                                                  ARGUMENT>::k_VALUE>::type * \
                                                                      = nullptr

# define BSLSTL_SHAREDPTR_DEFINE_IF_DELETER(FUNCTOR, ARGUMENT) ,              \
    typename enable_if<                                                       \
      BloombergLP::bslstl::SharedPtr_IsCallable  <FUNCTOR,                    \
                                                  ARGUMENT *>::k_VALUE ||     \
      BloombergLP::bslstl::SharedPtr_IsFactoryFor<FUNCTOR,                    \
                                                  ARGUMENT  >::k_VALUE>::type *


# define BSLSTL_SHAREDPTR_DECLARE_IF_NULLPTR_DELETER(FUNCTOR) ,               \
    typename enable_if<                                                       \
      BloombergLP::bslstl::SharedPtr_IsCallable  <FUNCTOR,                    \
                                                  nullptr_t>::k_VALUE ||      \
      BloombergLP::bslstl::SharedPtr_IsNullableFactory<                       \
                                           FUNCTOR>::k_VALUE>::type * = nullptr

# define BSLSTL_SHAREDPTR_DEFINE_IF_NULLPTR_DELETER(FUNCTOR) ,                \
    typename enable_if<                                                       \
      BloombergLP::bslstl::SharedPtr_IsCallable  <FUNCTOR,                    \
                                                  nullptr_t>::k_VALUE ||      \
      BloombergLP::bslstl::SharedPtr_IsNullableFactory<                       \
                                                     FUNCTOR>::k_VALUE>::type *
#else
// Do not attempt to support SFINAE in constructors in a C++03 compiler
# define BSLSTL_SHAREDPTR_DECLARE_IF_CONVERTIBLE
# define BSLSTL_SHAREDPTR_DEFINE_IF_CONVERTIBLE

# define BSLSTL_SHAREDPTR_DECLARE_IF_COMPATIBLE
# define BSLSTL_SHAREDPTR_DEFINE_IF_COMPATIBLE

# define BSLSTL_SHAREDPTR_DECLARE_IF_DELETER(FUNCTOR, ARGUMENT)
# define BSLSTL_SHAREDPTR_DEFINE_IF_DELETER(FUNCTOR, ARGUMENT)

# define BSLSTL_SHAREDPTR_DECLARE_IF_NULLPTR_DELETER(FUNCTOR)
# define BSLSTL_SHAREDPTR_DEFINE_IF_NULLPTR_DELETER(FUNCTOR)
#endif  // BSLSTL_SHAREDPTR_SUPPORTS_SFINAE_CHECKS

// Some SFINAE checks, when enabled, make use of discarded-value expressions
// (as the left-hand side of a comma operator). Clang compilers based on
// versions of LLVM earlier than 12 contain a bug in which substitution
// failures are not caught in discarded-value expressions when used in SFINAE
// contexts (this includes Clang 11 and earlier, and Apple Clang 13 and
// earlier).  In order to ensure that these compilers catch substitution
// failures in such expressions, this component does not discard them.  Note
// that this is dangerous, because not discarding the expression allows the
// possibility that the comma operator will be overloaded on the type of the
// expression, and so it is preferable to discard the expression where
// possible.
#if defined(BSLSTL_SHAREDPTR_SUPPORTS_SFINAE_CHECKS)
# if !defined(BSLS_PLATFORM_CMP_CLANG)                             \
  || !defined(__APPLE_CC__) && BSLS_PLATFORM_CMP_VERSION >= 120000 \
  ||  defined(__APPLE_CC__) && BSLS_PLATFORM_CMP_VERSION >  130000
#  define BSLSTL_SHAREDPTR_SFINAE_DISCARD(EXPRESSION) \
     static_cast<void>(EXPRESSION)
# else
#  define BSLSTL_SHAREDPTR_SFINAE_DISCARD(EXPRESSION) \
     (EXPRESSION)
# endif
#endif

namespace BloombergLP {
namespace bslstl {

struct SharedPtr_RepFromExistingSharedPtr {
    // This 'struct' is for internal use only, providing a tag for 'shared_ptr'
    // constructors to recognize that a passed 'SharedPtrRep' was obtained from
    // an existing 'shared_ptr' object.
};

struct SharedPtr_ImpUtil;
    // Forward declaration of 'SharedPtr_ImpUtil'. This is needed because this
    // struct is a friend of 'enable_shared_from_this' in the 'bsl' namespace.

#if defined(BSLSTL_SHAREDPTR_SUPPORTS_SFINAE_CHECKS)
template <class FUNCTOR, class ARG>
struct SharedPtr_IsCallable;
    // Forward declaration of component-private type trait to indicate whether
    // an object of (template parameter) type 'FUNCTOR' can be called as a
    // function with an argument of (template parameter) type 'ARG'

template <class FACTORY, class ARG>
struct SharedPtr_IsFactoryFor;
    // Forward declaration of component-private type trait to indicate whether
    // a pointer to a 'FACTORY' has a 'deleteObject' member that can be called
    // as 'factory->deleteObject((ARG *)p)'.

template <class FACTORY>
struct SharedPtr_IsNullableFactory;
    // Forward declaration of component-private type trait to indicate whether
    // a pointer to a 'FACTORY' has a 'deleteObject' member that can be called
    // as 'factory->deleteObject((ARG *)p)'.

template <class SOURCE_TYPE, class DEST_TYPE>
struct SharedPtr_IsPointerConvertible;
    // Forward declaration of component-private type trait to indicate whether
    // a pointer to a 'SOURCE_TYPE' can be converted to a pointer to a
    // 'DEST_TYPE'.  [util.smartptr.shared.const]/8 says "either DEST_TYPE is
    // U[N] and SOURCE_TYPE(*)[N] is convertible to DEST_TYPE*, or DEST_TYPE is
    // U[] and SOURCE_TYPE(*)[] is convertible to DEST_TYPE*".

template <class SOURCE_TYPE, class DEST_TYPE>
struct SharedPtr_IsPointerCompatible;
    // Forward declaration of component-private type trait to indicate whether
    // a pointer to a 'SOURCE_TYPE' is compatible with a pointer to
    // 'DEST_TYPE'.  [util.smartptr.shared]/5 says: "for the purposes of ...,
    // a pointer type SOURCE_TYPE* is said to be compatible with a pointer type
    // DEST_TYPE* when either SOURCE_TYPE* is convertible to DEST_TYPE* or
    // SOURCE_TYPE is U[N] and DEST_TYPE is cv U[]."

#endif

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

template<class ELEMENT_TYPE>
class enable_shared_from_this;

template <class ELEMENT_TYPE>
class shared_ptr;

template <class ELEMENT_TYPE>
class weak_ptr;

                        // ================
                        // class shared_ptr
                        // ================

template <class ELEMENT_TYPE>
class shared_ptr {
    // This class provides a thread-safe reference-counted "smart pointer" to
    // support "shared ownership" of objects: a shared pointer ensures that the
    // shared object is destroyed, using the appropriate deletion method, only
    // when there are no shared references to it.  The object (of template
    // parameter type 'ELEMENT_TYPE') referred to by a shared pointer may be
    // accessed directly using the '->' operator, or the dereference operator
    // (operator '*') can be used to obtain a reference to that object.
    //
    // Note that the object referred to by a shared pointer representation is
    // usually the same as the object referred to by that shared pointer (of
    // the same 'ELEMENT_TYPE'), but this need not always be true in the
    // presence of conversions or "aliasing": the object referred to (of
    // template parameter type 'ELEMENT_TYPE') by the shared pointer may differ
    // from the object of type 'COMPATIBLE_TYPE' (see the "Aliasing" section in
    // the component-level documentation) referred to by the shared pointer
    // representation.
    //
    // More generally, this class supports a complete set of *in*-*core*
    // pointer semantic operations.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(shared_ptr<ELEMENT_TYPE>,
                                   bsl::is_nothrow_move_constructible);

    // TYPES
    typedef typename bsl::remove_extent<ELEMENT_TYPE>::type element_type;
        // For shared pointers to non-array types, 'element_type' is an alias
        // to the 'ELEMENT_TYPE' template parameter.  Otherwise, it is an alias
        // to the type contained in the array.

    typedef weak_ptr<ELEMENT_TYPE> weak_type;
        // 'weak_type' is an alias to a weak pointer with the same element type
        // as this 'shared_ptr'.

  private:
    // DATA
    element_type                     *d_ptr_p; // pointer to the shared object

    BloombergLP::bslma::SharedPtrRep *d_rep_p; // pointer to the representation
                                               // object that manages the
                                               // shared object

    // PRIVATE TYPES
    typedef shared_ptr<ELEMENT_TYPE> SelfType;
        // 'SelfType' is an alias to this 'class', for compilers that do not
        // recognize plain 'shared_ptr'.

    typedef typename BloombergLP::bsls::UnspecifiedBool<shared_ptr>::BoolType
                                                                      BoolType;

    // FRIENDS
    template <class COMPATIBLE_TYPE>
    friend class shared_ptr;

    friend struct BloombergLP::bslstl::SharedPtr_ImpUtil;

  private:
    // PRIVATE CLASS METHODS
    template <class INPLACE_REP>
    static BloombergLP::bslma::SharedPtrRep *makeInternalRep(
                                        ELEMENT_TYPE                     *,
                                        INPLACE_REP                      *,
                                        BloombergLP::bslma::SharedPtrRep *rep);
        // Return the specified 'rep'.

    template <class COMPATIBLE_TYPE, class ALLOCATOR>
    static BloombergLP::bslma::SharedPtrRep *makeInternalRep(
                                     COMPATIBLE_TYPE               *ptr,
                                     ALLOCATOR                     *,
                                     BloombergLP::bslma::Allocator *allocator);
        // Return the address of a new out-of-place representation for a shared
        // pointer that manages the specified 'ptr' and uses the specified
        // 'allocator' to destroy the object pointed to by 'ptr'.  Use
        // 'allocator' to supply memory.

    template <class COMPATIBLE_TYPE, class DELETER>
    static BloombergLP::bslma::SharedPtrRep *makeInternalRep(
                                                      COMPATIBLE_TYPE *ptr,
                                                      DELETER         *deleter,
                                                      ...);
        // Return the address of a new out-of-place representation for a shared
        // pointer that manages the specified 'ptr' and uses the specified
        // 'deleter' to destroy the object pointed to by 'ptr'.  Use the
        // currently installed default allocator to supply memory.

  public:
    // CREATORS
    BSLS_KEYWORD_CONSTEXPR
    shared_ptr() BSLS_KEYWORD_NOEXCEPT;
        // Create an empty shared pointer, i.e., a shared pointer with no
        // representation that does not refer to any object and has no
        // deleter.

    BSLS_KEYWORD_CONSTEXPR
    shared_ptr(bsl::nullptr_t) BSLS_KEYWORD_NOEXCEPT;               // IMPLICIT
        // Create an empty shared pointer, i.e., a shared pointer with no
        // representation that does not refer to any object and has no
        // deleter.

    template <class CONVERTIBLE_TYPE
              BSLSTL_SHAREDPTR_DECLARE_IF_CONVERTIBLE>
    explicit shared_ptr(CONVERTIBLE_TYPE *ptr);
        // Create a shared pointer that manages a modifiable object of
        // (template parameter) type 'CONVERTIBLE_TYPE' and refers to the
        // specified '(ELEMENT_TYPE *)ptr'.  The currently installed default
        // allocator is used to allocate and deallocate the internal
        // representation of the shared pointer.  When all references have been
        // released, the object pointed to by the managed pointer will be
        // destroyed by a call to 'delete ptr'.  If 'CONVERTIBLE_TYPE *' is not
        // implicitly convertible to 'ELEMENT_TYPE *', then a compiler
        // diagnostic will be emitted indicating the error.  If 'ptr' is 0,
        // then this shared pointer will still allocate an internal
        // representation to share ownership of that empty state, which will be
        // reclaimed when the last reference is destroyed.  If an exception is
        // thrown allocating storage for the representation, then 'delete ptr'
        // will be called.  Note that if 'ptr' is a null-pointer constant, the
        // compiler will actually select the 'shared_ptr(bsl::nullptr_t)'
        // constructor, resulting in an empty shared pointer.

    template <class CONVERTIBLE_TYPE
              BSLSTL_SHAREDPTR_DECLARE_IF_CONVERTIBLE>
    shared_ptr(CONVERTIBLE_TYPE              *ptr,
               BloombergLP::bslma::Allocator *basicAllocator);
        // Create a shared pointer that manages a modifiable object of
        // (template parameter) type 'CONVERTIBLE_TYPE' and refers to the
        // specified 'ptr' cast to a pointer to the (template parameter) type
        // 'ELEMENT_TYPE'.  If the specified 'basicAllocator' is not 0, then
        // 'basicAllocator' is used to allocate and deallocate the internal
        // representation of the shared pointer and to destroy the shared
        // object when all references have been released; otherwise, the
        // currently installed default allocator is used.  If
        // 'CONVERTIBLE_TYPE *' is not implicitly convertible to
        // 'ELEMENT_TYPE *', then a compiler diagnostic will be emitted
        // indicating the error.  If 'ptr' is 0, then this shared pointer will
        // still allocate an internal representation to share ownership of that
        // empty state, which will be reclaimed when the last reference is
        // destroyed.  Note that if 'ptr' is a null-pointer constant, the
        // compiler will actually select the
        // 'shared_ptr(bsl::nullptr_t, BloombergLP::bslma::Allocator *)'
        // constructor, resulting in an empty shared pointer.  Note that if
        // 'basicAllocator' is a pointer to a class derived from
        // 'bslma::Allocator', the compiler will actually select the following
        // (more general) constructor that has the same behavior:
        //..
        //  template <class CONVERTIBLE_TYPE, class DELETER>
        //  shared_ptr(CONVERTIBLE_TYPE *ptr, DELETER * deleter);
        //..

    shared_ptr(ELEMENT_TYPE *ptr, BloombergLP::bslma::SharedPtrRep *rep);
        // Create a shared pointer that takes ownership of the specified 'rep'
        // and refers to the modifiable object at the specified 'ptr' address.
        // The number of references to 'rep' is *NOT* incremented.  Note that
        // if 'rep' is a pointer to a class derived from
        // 'BloombergLP::bslma::SharedPtrRep', the compiler will actually
        // select the following (more general) constructor that has the same
        // behavior:
        //..
        //  template <class COMPATIBLE_TYPE, class DELETER>
        //  shared_ptr(COMPATIBLE_TYPE *ptr, DELETER * deleter);
        //..

    shared_ptr(ELEMENT_TYPE                     *ptr,
               BloombergLP::bslma::SharedPtrRep *rep,
               BloombergLP::bslstl::SharedPtr_RepFromExistingSharedPtr);
        // Create a shared pointer that takes ownership of the specified 'rep'
        // and refers to the modifiable object at the specified 'ptr' address.
        // The number of references to 'rep' is *NOT* incremented.  The
        // behavior is undefined unless 'rep' was previously obtained from an
        // existing 'shared_ptr', 'rep->disposeObject' has not been called, and
        // 'rep->numReferences() > 0'.  Note that this constructor is intended
        // for use by 'weak_ptr::lock', and it would be surprising to find
        // another client.  This solves an obscure problem that arises from
        // unusual use of classes derived from 'enable_shared_from_this'.
        // Further note that the caller is responsible for incrementing the
        // 'numReferences' count prior to calling this constructor, in order to
        // maintain a consistent reference count when this 'shared_ptr' object
        // releases the shared object from its management.

    template <class CONVERTIBLE_TYPE,
              class DELETER
              BSLSTL_SHAREDPTR_DECLARE_IF_CONVERTIBLE
              BSLSTL_SHAREDPTR_DECLARE_IF_DELETER(DELETER *, CONVERTIBLE_TYPE)>
    shared_ptr(CONVERTIBLE_TYPE *ptr, DELETER *deleter);
        // Create a shared pointer that manages a modifiable object of
        // (template parameter) type 'CONVERTIBLE_TYPE', refers to the
        // specified 'ptr' cast to a pointer to the (template parameter) type
        // 'ELEMENT_TYPE', and uses the specified 'deleter' to delete the
        // shared object when all references have been released.  Use the
        // currently installed default allocator to allocate and deallocate the
        // internal representation of the shared pointer, unless 'DELETER' is a
        // class derived from either 'bslma::Allocator' or
        // 'bslma::SharedPtrRep'; if 'DELETER' is a class derived from
        // 'bslma::allocator', create a shared pointer as if calling the
        // constructor:
        //..
        //  template <class CONVERTIBLE_TYPE>
        //  shared_ptr(CONVERTIBLE_TYPE               *ptr,
        //             BloombergLP::bslma::Allocator *basicAllocator);
        //..
        // If 'DELETER' is a class derived from 'bslma::SharedPtrRep', create a
        // shared pointer as if calling the constructor:
        //..
        //  shared_ptr(ELEMENT_TYPE                     *ptr,
        //             BloombergLP::bslma::SharedPtrRep *rep);
        //..
        // If 'DELETER' does not derive from either 'bslma::Allocator' or
        // 'BloombergLP::bslma::SharedPtrRep', then 'deleter' shall be a
        // pointer to a factory object that exposes a member function that can
        // be invoked as 'deleteObject(ptr)' that will be called to destroy the
        // object at the 'ptr' address (i.e., 'deleter->deleteObject(ptr)' will
        // be called to delete the shared object).  (See the "Deleters" section
        // in the component-level documentation.)  If 'CONVERTIBLE_TYPE *' is
        // not implicitly convertible to 'ELEMENT_TYPE *', then a compiler
        // diagnostic will be emitted indicating the error.  If 'ptr' is 0,
        // then the null pointer will be reference counted, and the deleter
        // will be called when the last reference is destroyed.  If an
        // exception is thrown when allocating storage for the internal
        // representation, then 'deleter(ptr)' will be called.  Note that this
        // method is a BDE extension and not part of the C++ standard
        // interface.

    template <class CONVERTIBLE_TYPE,
              class DELETER
              BSLSTL_SHAREDPTR_DECLARE_IF_CONVERTIBLE
              BSLSTL_SHAREDPTR_DECLARE_IF_DELETER(DELETER, CONVERTIBLE_TYPE)>
    shared_ptr(CONVERTIBLE_TYPE              *ptr,
               DELETER                        deleter,
               BloombergLP::bslma::Allocator *basicAllocator = 0);
        // Create a shared pointer that manages a modifiable object of
        // (template parameter) type 'CONVERTIBLE_TYPE', refers to the
        // specified '(ELEMENT_TYPE *)ptr', and uses the specified 'deleter' to
        // delete the shared object when all references have been released.
        // Optionally specify a 'basicAllocator' to allocate and deallocate the
        // internal representation of the shared pointer (including a copy of
        // 'deleter').  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'DELETER' shall be either a function
        // pointer or a "factory" deleter that may be invoked to destroy the
        // object referred to by a single argument of type 'CONVERTIBLE_TYPE *'
        // (i.e., 'deleter(ptr)' or 'deleter->deleteObject(ptr)' will be called
        // to destroy the shared object).  (See the "Deleters" section in the
        // component-level documentation.)  If 'CONVERTIBLE_TYPE *' is not
        // implicitly convertible to 'ELEMENT_TYPE *', then this constructor
        // will not be selected by overload resolution.  If 'ptr' is 0, then
        // the null pointer will be reference counted, and 'deleter(ptr)' will
        // be called when the last reference is destroyed.  If an exception is
        // thrown when allocating storage for the internal representation, then
        // 'deleter(ptr)' will be called.  The behavior is undefined unless the
        // constructor making a copy of 'deleter' does not throw an exception.

    template <class CONVERTIBLE_TYPE,
              class DELETER,
              class ALLOCATOR
              BSLSTL_SHAREDPTR_DECLARE_IF_CONVERTIBLE
              BSLSTL_SHAREDPTR_DECLARE_IF_DELETER(DELETER, CONVERTIBLE_TYPE)>
    shared_ptr(CONVERTIBLE_TYPE               *ptr,
               DELETER                         deleter,
               ALLOCATOR                       basicAllocator,
               typename ALLOCATOR::value_type * = 0);
        // Create a shared pointer that manages a modifiable object of
        // (template parameter) type 'CONVERTIBLE_TYPE', refers to the
        // specified 'ptr' cast to a pointer to the (template parameter) type
        // 'ELEMENT_TYPE', and uses the specified 'deleter' to delete the
        // shared object when all references have been released.  Use the
        // specified 'basicAllocator' to allocate and deallocate the internal
        // representation of the shared pointer (including a copy of the
        // 'deleter').  The (template parameter) type 'DELETER' shall be either
        // a function pointer or a function-like deleter that may be invoked to
        // destroy the object referred to by a single argument of type
        // 'CONVERTIBLE_TYPE *' (i.e., 'deleter(ptr)' will be called to destroy
        // the shared object).  (See the "Deleters" section in the component-
        // level documentation.)  The (template parameter) type 'ALLOCATOR'
        // shall satisfy the Allocator requirements of the C++ standard (C++11
        // 17.6.3.5, [allocator.requirements]).  If 'CONVERTIBLE_TYPE *' is not
        // implicitly convertible to 'ELEMENT_TYPE *', then a compiler
        // diagnostic will be emitted indicating the error.  If 'ptr' is 0,
        // then the null pointer will be reference counted, and 'deleter(ptr)'
        // will be called when the last reference is destroyed.  If an
        // exception is thrown when allocating storage for the internal
        // representation, then 'deleter(ptr)' will be called.  The behavior is
        // undefined unless the constructor making a copy of 'deleter' does not
        // throw an exception.  Note that the final dummy parameter is a simple
        // SFINAE check that the (template parameter) 'ALLOCATOR' type probably
        // satisfies the standard allocator requirements; in particular, it
        // will not match pointer types, so any pointers to 'bslma::Allocator'
        // derived classes will dispatch to the constructor above this, and not
        // be greedily matched to a generic type parameter.

    shared_ptr(nullptr_t                      nullPointerLiteral,
               BloombergLP::bslma::Allocator *basicAllocator);
        // Create an empty shared pointer.  The specified 'nullPointerLiteral'
        // and 'basicAllocator' are not used.  Note that use of this
        // constructor is equivalent to calling the default constructor.

    template <class DELETER
              BSLSTL_SHAREDPTR_DECLARE_IF_NULLPTR_DELETER(DELETER)>
    shared_ptr(nullptr_t                      nullPointerLiteral,
               DELETER                        deleter,
               BloombergLP::bslma::Allocator *basicAllocator = 0);
        // Create a shared pointer that reference-counts the null pointer, and
        // calls the specified 'deleter' with a null pointer (i.e., invokes
        // 'deleter((ELEMENT_TYPE *)0)') when the last shared reference is
        // destroyed.  The specified 'nullPointerLiteral' is not used.
        // Optionally specify a 'basicAllocator' to allocate and deallocate the
        // internal representation of the shared pointer (including a copy of
        // 'deleter').  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  If an exception is thrown when
        // allocating storage for the internal representation, then
        // 'deleter((ELEMENT_TYPE *)0)' will be called.  The behavior is
        // undefined unless 'deleter' can be called with a null pointer, and
        // unless the constructor making a copy of 'deleter' does not throw an
        // exception.

    template <class DELETER, class ALLOCATOR
              BSLSTL_SHAREDPTR_DECLARE_IF_NULLPTR_DELETER(DELETER)>
    shared_ptr(nullptr_t                       nullPointerLiteral,
               DELETER                         deleter,
               ALLOCATOR                       basicAllocator,
               typename ALLOCATOR::value_type * = 0);
        // Create a shared pointer that reference-counts the null pointer,
        // calls the specified 'deleter' with a null pointer (i.e., invokes
        // 'deleter((ELEMENT_TYPE *)0)') when the last shared reference is
        // destroyed, and uses the specified 'basicAllocator' to allocate and
        // deallocate the internal representation of the shared pointer
        // (including a copy of the 'deleter').  The (template parameter) type
        // 'DELETER' shall be either a function pointer or a function-like
        // deleter (See the "Deleters" section in the component- level
        // documentation).  The (template parameter) type 'ALLOCATOR' shall
        // satisfy the Allocator requirements of the C++ standard (C++11
        // 17.6.3.5, [allocator.requirements]).  The specified
        // 'nullPointerLiteral' is not used.  If an exception is thrown when
        // allocating storage for the internal representation, then
        // 'deleter((ELEMENT_TYPE *)0)' will be called.  The behavior is
        // undefined unless 'deleter' can be called with a null pointer, and
        // unless the constructor making a copy of 'deleter' does not throw an
        // exception.  Note that the final dummy parameter is a simple SFINAE
        // check that the 'ALLOCATOR' type probably satisfies the standard
        // allocator requirements; in particular, it will not match pointer
        // types, so any pointers to 'bslma::Allocator' derived classes will
        // dispatch to the constructor above this, and not be greedily matched
        // to a generic type parameter.

    template <class CONVERTIBLE_TYPE
              BSLSTL_SHAREDPTR_DECLARE_IF_CONVERTIBLE>
    shared_ptr(
         BloombergLP::bslma::ManagedPtr<CONVERTIBLE_TYPE>  managedPtr,
         BloombergLP::bslma::Allocator                    *basicAllocator = 0);
                                                                    // IMPLICIT
        // Create a shared pointer that takes over the management of the
        // modifiable object (if any) previously managed by the specified
        // 'managedPtr' to the (template parameter) type 'CONVERTIBLE_TYPE',
        // and that refers to '(ELEMENT_TYPE *)managedPtr.ptr()'.  The deleter
        // used in the 'managedPtr' will be used to destroy the shared object
        // when all references have been released.  Optionally specify a
        // 'basicAllocator' used to allocate and deallocate the internal
        // representation of the shared pointer.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.  If
        // 'CONVERTIBLE_TYPE *' is not implicitly convertible to
        // 'ELEMENT_TYPE *', then a compiler diagnostic will be emitted
        // indicating the error.  Note that if 'managedPtr' is empty, then an
        // empty shared pointer is created and 'basicAllocator' is ignored.
        // Also note that if 'managedPtr' owns a reference to another shared
        // object (due to a previous call to 'shared_ptr<T>::managedPtr') then
        // no memory will be allocated, and this 'shared_ptr' will adopt the
        // 'ManagedPtr's ownership of that shared object.

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
    template <class CONVERTIBLE_TYPE
              BSLSTL_SHAREDPTR_DECLARE_IF_CONVERTIBLE>
    explicit shared_ptr(std::auto_ptr<CONVERTIBLE_TYPE>&  autoPtr,
                        BloombergLP::bslma::Allocator    *basicAllocator = 0);
        // Create a shared pointer that takes over the management of the
        // modifiable object previously managed by the specified 'autoPtr' to
        // the (template parameter) type 'CONVERTIBLE_TYPE', and that refers to
        // '(ELEMENT_TYPE *)autoPtr.get()'.  'delete(autoPtr.release())' will
        // be called to destroy the shared object when all references have been
        // released.  Optionally specify a 'basicAllocator' used to allocate
        // and deallocate the internal representation of the shared pointer.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.  If 'CONVERTIBLE_TYPE *' is not implicitly convertible to
        // 'ELEMENT_TYPE *', then a compiler diagnostic will be emitted
        // indicating the error.

    explicit shared_ptr(std::auto_ptr_ref<ELEMENT_TYPE>  autoRef,
                        BloombergLP::bslma::Allocator   *basicAllocator = 0);
        // Create a shared pointer that takes over the management of the
        // modifiable object of (template parameter) type 'COMPATIBLE_TYPE'
        // previously managed by the auto pointer object that the specified
        // 'autoRef' refers to; this shared pointer refers to the same object
        // that it manages, and 'delete(get())' will be called to destroy the
        // shared object when all references have been released.  Optionally
        // specify a 'basicAllocator' used to allocate and deallocate the
        // internal representation of the shared pointer.  If 'basicAllocator'
        // is 0, the currently installed default allocator is used.  This
        // function does not exist unless 'COMPATIBLE_TYPE *' is convertible to
        // 'ELEMENT_TYPE *'.
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
# if defined(BSLSTL_SHAREDPTR_SUPPORTS_SFINAE_CHECKS)
    template <class COMPATIBLE_TYPE,
              class UNIQUE_DELETER,
              typename enable_if<is_convertible<
                      typename std::unique_ptr<COMPATIBLE_TYPE,
                                               UNIQUE_DELETER>::pointer,
                      ELEMENT_TYPE *>::value>::type * = nullptr>
     shared_ptr(std::unique_ptr<COMPATIBLE_TYPE,
                                UNIQUE_DELETER>&&  adoptee,
                BloombergLP::bslma::Allocator     *basicAllocator = 0);
                                                                    // IMPLICIT
        // Create a shared pointer that takes over the management of the
        // modifiable object previously managed by the specified 'adoptee' to
        // the (template parameter) type 'COMPATIBLE_TYPE', and that refers to
        // '(ELEMENT_TYPE *)autoPtr.get()'.  'delete(autoPtr.release())' will
        // be called to destroy the shared object when all references have been
        // released.  Optionally specify a 'basicAllocator' used to allocate
        // and deallocate the internal representation of the shared pointer.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.  This function does not exist unless
        // 'unique_ptr<COMPATIBLE_TYPE, DELETER>::pointer' is convertible to
        // 'ELEMENT_TYPE *'.  Note that this function creates a 'shared_ptr'
        // with an unspecified deleter type that has satisfies this contract,
        // which might not be the deleter of 'rhs', which is specified by the
        // C++ standard.
# else
    template <class COMPATIBLE_TYPE, class UNIQUE_DELETER>
    shared_ptr(std::unique_ptr<COMPATIBLE_TYPE,
                                UNIQUE_DELETER>&&  adoptee,
               BloombergLP::bslma::Allocator      *basicAllocator = 0,
               typename enable_if<is_convertible<
                      typename std::unique_ptr<COMPATIBLE_TYPE,
                                               UNIQUE_DELETER>::pointer,
                      ELEMENT_TYPE *>::value,
                      BloombergLP::bslstl::SharedPtr_ImpUtil>::type =
                                      BloombergLP::bslstl::SharedPtr_ImpUtil())
                                                                    // IMPLICIT
        // Create a shared pointer that takes over the management of the
        // modifiable object previously managed by the specified 'adoptee' to
        // the (template parameter) type 'COMPATIBLE_TYPE', and that refers to
        // '(ELEMENT_TYPE *)autoPtr.get()'.  'delete(autoPtr.release())' will
        // be called to destroy the shared object when all references have been
        // released.  Optionally specify a 'basicAllocator' used to allocate
        // and deallocate the internal representation of the shared pointer.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.  This function does not exist unless
        // 'unique_ptr<COMPATIBLE_TYPE, DELETER>::pointer' is convertible to
        // 'ELEMENT_TYPE *'.  Note that this function creates a 'shared_ptr'
        // with an unspecified deleter type that has satisfies this contract,
        // which might not be the deleter of 'rhs', which is specified by the
        // C++ standard.
    : d_ptr_p(adoptee.get())
    , d_rep_p(0)
    {
        // This constructor template must be defined inline inside the class
        // definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.

        typedef BloombergLP::bslma::SharedPtrInplaceRep<
                        std::unique_ptr<COMPATIBLE_TYPE, UNIQUE_DELETER> > Rep;

        if (d_ptr_p) {
            basicAllocator =
                        BloombergLP::bslma::Default::allocator(basicAllocator);
            Rep *rep = new (*basicAllocator) Rep(basicAllocator,
                            BloombergLP::bslmf::MovableRefUtil::move(adoptee));
            d_rep_p = rep;
            BloombergLP::bslstl::SharedPtr_ImpUtil::loadEnableSharedFromThis(
                                                                       d_ptr_p,
                                                                       this);
        }
    }
# endif // BSLSTL_SHAREDPTR_SUPPORTS_SFINAE_CHECKS
#endif // BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR

    template <class ANY_TYPE>
    shared_ptr(const shared_ptr<ANY_TYPE>&  source,
               ELEMENT_TYPE                *object) BSLS_KEYWORD_NOEXCEPT;
        // Create a shared pointer that manages the same modifiable object (if
        // any) as the specified 'source' shared pointer to the (template
        // parameter) type 'ANY_TYPE', and that refers to the modifiable object
        // at the specified 'object' address.  The resulting shared pointer is
        // known as an "alias" of 'source'.  Note that typically the objects
        // referred to by 'source' and 'object' have identical lifetimes (e.g.,
        // one might be a part of the other), so that the deleter for 'source'
        // will destroy them both, but they do not necessarily have the same
        // type.  Also note that if 'source' is empty, then an empty shared
        // pointer is created, even if 'object' is not null (in which case this
        // empty shared pointer will refer to the same object as 'object').
        // Also note that if 'object' is null and 'source' is not empty, then a
        // reference-counted null pointer alias will be created.

    template <class COMPATIBLE_TYPE
              BSLSTL_SHAREDPTR_DECLARE_IF_COMPATIBLE>
    shared_ptr(const shared_ptr<COMPATIBLE_TYPE>& other) BSLS_KEYWORD_NOEXCEPT;
        // Create a shared pointer that manages the same modifiable object (if
        // any) as the specified 'other' shared pointer to the (template
        // parameter) type 'COMPATIBLE_TYPE', uses the same deleter as 'other'
        // to destroy the shared object, and refers to
        // '(ELEMENT_TYPE*)other.get()'.  If 'COMPATIBLE_TYPE *' is not
        // implicitly convertible to 'ELEMENT_TYPE *', then a compiler
        // diagnostic will be emitted indicating the error.  Note that if
        // 'other' is empty, then an empty shared pointer is created, which may
        // still point to an un-managed object if 'other' were constructed
        // through an aliasing constructor.

    shared_ptr(const shared_ptr& original) BSLS_KEYWORD_NOEXCEPT;
        // Create a shared pointer that refers to and manages the same object
        // (if any) as the specified 'original' shared pointer, and uses the
        // same deleter as 'original' to destroy the shared object.  Note that
        // if 'original' is empty, then an empty shared pointer is created,
        // which may still point to an un-managed object if 'original' were
        // constructed through an aliasing constructor.

    shared_ptr(BloombergLP::bslmf::MovableRef<shared_ptr> original)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a shared pointer that refers to and assumes management of the
        // same object (if any) as the specified 'original' shared pointer,
        // using the same deleter as 'original' to destroy the shared object,
        // and reset 'original' to an empty state, not pointing to any object.
        // Note that if 'original' is empty, then an empty shared pointer is
        // created, which may still point to an un-managed object if 'original'
        // were constructed through an aliasing constructor.

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    template <class COMPATIBLE_TYPE
              BSLSTL_SHAREDPTR_DECLARE_IF_COMPATIBLE>
    shared_ptr(shared_ptr<COMPATIBLE_TYPE>&& other) BSLS_KEYWORD_NOEXCEPT;
        // Create a shared pointer that refers to and assumes management of the
        // same object (if any) as the specified 'other' shared pointer to the
        // (template parameter) type 'COMPATIBLE_TYPE', using the same deleter
        // as 'other' to destroy the shared object, and refers to
        // '(ELEMENT_TYPE*)other.get()'.  If 'COMPATIBLE_TYPE *' is not
        // implicitly convertible to 'ELEMENT_TYPE *', then a compiler
        // diagnostic will be emitted indicating the error.  Note that if
        // 'other' is empty, then an empty shared pointer is created, which may
        // still point to an un-managed object if 'other' were constructed
        // through an aliasing constructor.
#else
    template <class COMPATIBLE_TYPE
              BSLSTL_SHAREDPTR_DECLARE_IF_COMPATIBLE>
    shared_ptr(
           BloombergLP::bslmf::MovableRef<shared_ptr<COMPATIBLE_TYPE> > other)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a shared pointer that refers to and assumes management of the
        // same object (if any) as the specified 'other' shared pointer to the
        // (template parameter) type 'COMPATIBLE_TYPE', using the same deleter
        // as 'other' to destroy the shared object, and refers to
        // '(ELEMENT_TYPE*)other.get()'.  If 'COMPATIBLE_TYPE *' is not
        // implicitly convertible to 'ELEMENT_TYPE *', then a compiler
        // diagnostic will be emitted indicating the error.  Note that if
        // 'other' is empty, then an empty shared pointer is created, which may
        // still point to an un-managed object if 'other' were constructed
        // through an aliasing constructor.
#endif

    template<class COMPATIBLE_TYPE
              BSLSTL_SHAREDPTR_DECLARE_IF_COMPATIBLE>
    explicit shared_ptr(const weak_ptr<COMPATIBLE_TYPE>& ptr);
        // Create a shared pointer that refers to and manages the same object
        // as the specified 'ptr' if 'ptr.expired()' is 'false'; otherwise,
        // create a shared pointer in the empty state.  Note that the
        // referenced and managed objects may be different if 'ptr' was created
        // from a 'shared_ptr' in an aliasing state.

#if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    template<class COMPATIBLE_TYPE
              BSLSTL_SHAREDPTR_DECLARE_IF_COMPATIBLE>
    explicit shared_ptr(
              BloombergLP::bslmf::MovableRef<weak_ptr<COMPATIBLE_TYPE> > ptr);
        // Create a shared pointer that refers to and manages the same object
        // as the specified 'ptr' if  'ptr.expired()' is 'false'; otherwise,
        // create a shared pointer in the empty state.  Note that the
        // referenced and managed objects may be different if 'ptr' was created
        // from a 'shared_ptr' in an aliasing state.  Also note that this
        // overloaded constructor is necessary only for C++03 compilers that
        // rely on the BDE move-emulation type, 'bslmf::MovableRef'; a C++11
        // compiler will pass rvalues directly to the constructor taking a
        // 'const weak_ptr&', rendering this constructor redundant.
#endif

    ~shared_ptr();
        // Destroy this shared pointer.  If this shared pointer refers to a
        // (possibly shared) object, then release the reference to that object,
        // and destroy the shared object using its associated deleter if this
        // shared pointer is the last reference to that object.

    // MANIPULATORS
    shared_ptr& operator=(const shared_ptr& rhs) BSLS_KEYWORD_NOEXCEPT;
        // Make this shared pointer manage the same modifiable object as the
        // specified 'rhs' shared pointer to the (template parameter) type
        // 'COMPATIBLE_TYPE', use the same deleter as 'rhs', and refer to
        // '(ELEMENT_TYPE *)rhs.get()'; return a reference providing modifiable
        // access to this shared pointer.  Note that if 'rhs' is empty, then
        // this shared pointer will also be empty after the assignment.  Also
        // note that if '*this' is the same object as 'rhs', then this method
        // has no effect.

    shared_ptr& operator=(BloombergLP::bslmf::MovableRef<shared_ptr> rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Make this shared pointer manage the same modifiable object as the
        // specified 'rhs' shared pointer to the (template parameter) type
        // 'COMPATIBLE_TYPE', use the same deleter as 'rhs', and refer to
        // 'rhs.get()'; return a reference providing modifiable access to this
        // shared pointer.  Reset 'rhs' to an empty state, not pointing to any
        // object, unless '*this' is the same object as 'rhs'.  Note that if
        // 'rhs' is empty, then this shared pointer will also be empty after
        // the assignment.

    template <class COMPATIBLE_TYPE>
    typename enable_if<
        is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value,
        shared_ptr&>::type
    operator=(const shared_ptr<COMPATIBLE_TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT;
        // Make this shared pointer refer to and manage the same modifiable
        // object as the specified 'rhs' shared pointer to the (template
        // parameter) type 'COMPATIBLE_TYPE', using the same deleter as 'rhs'
        // and referring to '(ELEMENT_TYPE *)rhs.get()', and return a reference
        // to this modifiable shared pointer.  If this shared pointer is
        // already managing a (possibly shared) object, then release the shared
        // reference to that object, and destroy it using its associated
        // deleter if this shared pointer held the last shared reference to
        // that object.  Note that if 'rhs' is empty, then this shared pointer
        // will also be empty after the assignment.

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    template <class COMPATIBLE_TYPE>
    typename
            enable_if<is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value,
                      shared_ptr&>::type
    operator=(shared_ptr<COMPATIBLE_TYPE>&& rhs) BSLS_KEYWORD_NOEXCEPT;
        // Make this shared pointer refer to and manage the same modifiable
        // object as the specified 'rhs' shared pointer to the (template
        // parameter) type 'COMPATIBLE_TYPE', using the same deleter as 'rhs'
        // and referring to '(ELEMENT_TYPE *)rhs.get()', and return a reference
        // to this modifiable shared pointer.  If this shared pointer is
        // already managing a (possibly shared) object, then release the shared
        // reference to that object, and destroy it using its associated
        // deleter if this shared pointer held the last shared reference to
        // that object.  Reset 'rhs' to an empty state, not pointing to any
        // object, unless '*this' is the same object as 'rhs'.  This function
        // does not exist unless a pointer to (template parameter)
        // 'COMPATIBLE_TYPE' is convertible to a pointer to the (template
        // parameter) 'ELEMENT_TYPE' of this 'shared_ptr'.  Note that if 'rhs'
        // is empty, then this shared pointer will also be empty after the
        // assignment.
#else
    template <class COMPATIBLE_TYPE>
    typename
            enable_if<is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value,
                      shared_ptr&>::type
    operator=(BloombergLP::bslmf::MovableRef<shared_ptr<COMPATIBLE_TYPE> > rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Make this shared pointer refer to and manage the same modifiable
        // object as the specified 'rhs' shared pointer to the (template
        // parameter) type 'COMPATIBLE_TYPE', using the same deleter as 'rhs'
        // and referring to '(ELEMENT_TYPE *)rhs.get()', and return a reference
        // to this modifiable shared pointer.  If this shared pointer is
        // already managing a (possibly shared) object, then release the shared
        // reference to that object, and destroy it using its associated
        // deleter if this shared pointer held the last shared reference to
        // that object.  Reset 'rhs' to an empty state, not pointing to any
        // object, unless '*this' is the same object as 'rhs'.  This function
        // does not exist unless a pointer to (template parameter)
        // 'COMPATIBLE_TYPE' is convertible to a pointer to the (template
        // parameter) 'ELEMENT_TYPE' of this 'shared_ptr'.  Note that if 'rhs'
        // is empty, then this shared pointer will also be empty after the
        // assignment.
#endif

    template <class COMPATIBLE_TYPE>
    typename enable_if<
        is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value,
        shared_ptr&>::type
    operator=(BloombergLP::bslma::ManagedPtr<COMPATIBLE_TYPE> rhs);
        // Transfer, to this shared pointer, ownership of the modifiable object
        // managed by the specified 'rhs' managed pointer to the (template
        // parameter) type 'COMPATIBLE_TYPE', and make this shared pointer
        // refer to '(ELEMENT_TYPE *)rhs.ptr()'.  The deleter used in the 'rhs'
        // will be used to destroy the shared object when all references have
        // been released.  The *default* *allocator* is used to allocate a
        // 'SharedPtrRep', if needed (users must use the copy-constructor and
        // swap instead of using this operator to supply an alternative
        // allocator).  If this shared pointer is already managing a (possibly
        // shared) object, then release the reference to that shared object,
        // and destroy it using its associated deleter if this shared pointer
        // held the last shared reference to that object.  Note that if 'rhs'
        // is empty, then this shared pointer will be empty after the
        // assignment.  Also note that if 'rhs' owns a reference to another
        // shared object (due to a previous call to
        // 'shared_ptr<T>::managedPtr') then this 'shared_ptr' will adopt the
        // 'ManagedPtr's ownership of that shared object.

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
    template <class COMPATIBLE_TYPE>
    typename enable_if<
        is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value,
        shared_ptr&>::type
    operator=(std::auto_ptr<COMPATIBLE_TYPE> rhs);
        // Transfer, to this shared pointer, ownership of the modifiable object
        // managed by the specified 'rhs' auto pointer to the (template
        // parameter) type 'COMPATIBLE_TYPE', and make this shared pointer
        // refer to '(ELEMENT_TYPE *)rhs.get()'.  'delete(autoPtr.release())'
        // will be called to destroy the shared object when all references have
        // been released.  If this shared pointer is already managing a
        // (possibly shared) object, then release the reference to that shared
        // object, and destroy it using its associated deleter if this shared
        // pointer held the last shared reference to that object.  Note that if
        // 'rhs' is empty, then this shared pointer will be empty after the
        // assignment.
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
    template <class COMPATIBLE_TYPE, class UNIQUE_DELETER>
    typename enable_if<
        is_convertible<
            typename std::unique_ptr<COMPATIBLE_TYPE, UNIQUE_DELETER>::pointer,
                                     ELEMENT_TYPE *>::value,
            shared_ptr&>::type
    operator=(std::unique_ptr<COMPATIBLE_TYPE, UNIQUE_DELETER>&& rhs);
        // Transfer, to this shared pointer, ownership of the object managed by
        // the specified 'rhs' unique pointer to the (template parameter) type
        // 'COMPATIBLE_TYPE', and make this shared pointer refer to
        // '(ELEMENT_TYPE *)rhs.get()'.  The deleter of 'rhs' will be called to
        // destroy the shared object when all references have been released.
        // If this shared pointer is already managing a (possibly shared)
        // object, then release the reference to that shared object, and
        // destroy it using its associated deleter if this shared pointer held
        // the last shared reference to that object.  This function does not
        // exist unless 'unique_ptr<COMPATIBLE_TYPE, DELETER>::pointer' is
        // convertible to 'ELEMENT_TYPE *'.  Note that if 'rhs' is empty, then
        // this shared pointer will be empty after the assignment.  Also note
        // that this function creates a 'shared_ptr' with an unspecified
        // deleter type that satisfies this contract; the C++11 standard
        // specifies the exact deleter that should be in use after assignment,
        // so this implementation may be non-conforming.
#endif

    void reset() BSLS_KEYWORD_NOEXCEPT;
        // Reset this shared pointer to the empty state.  If this shared
        // pointer is managing a (possibly shared) object, then release the
        // reference to the shared object, calling the associated deleter to
        // destroy the shared object if this shared pointer is the last shared
        // reference.

    template <class COMPATIBLE_TYPE>
    typename
      enable_if<is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value>::type
    reset(COMPATIBLE_TYPE *ptr);
        // Modify this shared pointer to manage the modifiable object of the
        // (template parameter) type 'COMPATIBLE_TYPE' at the specified 'ptr'
        // address and to refer to '(ELEMENT_TYPE *)ptr'.  If this shared
        // pointer is already managing a (possibly shared) object, then, unless
        // an exception is thrown allocating memory to manage 'ptr', release
        // the reference to the shared object, calling the associated deleter
        // to destroy the shared object if this shared pointer is the last
        // reference.  The currently installed default allocator is used to
        // allocate the internal representation of this shared pointer, and the
        // shared object will be destroyed by a call to 'delete ptr' when all
        // references have been released.  If an exception is thrown allocating
        // the internal representation, then 'delete ptr' is called and this
        // shared pointer retains ownership of its original object.  If
        // 'COMPATIBLE_TYPE*' is not implicitly convertible to 'ELEMENT_TYPE*',
        // then a compiler diagnostic will be emitted indicating the error.
        // Note that if 'ptr' is 0, then this shared pointer will still
        // allocate an internal representation to share ownership of that empty
        // state, which will be reclaimed when the last reference is destroyed.

    template <class COMPATIBLE_TYPE, class DELETER>
    typename
      enable_if<is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value>::type
    reset(COMPATIBLE_TYPE *ptr, DELETER deleter);
        // Modify this shared pointer to manage the modifiable object of the
        // (template parameter) type 'COMPATIBLE_TYPE' at the specified 'ptr'
        // address, refer to '(ELEMENT_TYPE *)ptr', and use the specified
        // 'deleter' to delete the shared object when all references have been
        // released.  If this shared pointer is already managing a (possibly
        // shared) object, then unless an exception is thrown allocating memory
        // to manage 'ptr', release the reference to the shared object, calling
        // the associated deleter to destroy the shared object if this shared
        // pointer is the last reference.  If 'DELETER' is an object type, then
        // 'deleter' is assumed to be a function-like deleter that may be
        // invoked to destroy the object referred to by a single argument of
        // type 'COMPATIBLE_TYPE *' (i.e., 'deleter(ptr)' will be called to
        // destroy the shared object).  If 'DELETER' is a pointer type that is
        // not a function pointer, then 'deleter' shall be a pointer to a
        // factory object that exposes a member function that can be invoked as
        // 'deleteObject(ptr)' that will be called to destroy the object at the
        // 'ptr' address (i.e., 'deleter->deleteObject(ptr)' will be called to
        // delete the shared object).  (See the "Deleters" section in the
        // component-level documentation.)  If 'DELETER' is also a pointer to
        // 'bslma::Allocator' or to a class derived from 'bslma::Allocator',
        // then that allocator will also be used to allocate and destroy the
        // internal representation of this shared pointer when all references
        // have been released; otherwise, the currently installed default
        // allocator is used to allocate and destroy the internal
        // representation of this shared pointer when all references have been
        // released.  If an exception is thrown allocating the internal
        // representation, then 'deleter(ptr)' is called (or
        // 'deleter->deleteObject(ptr)' for factory-type deleters) and this
        // shared pointer retains ownership of its original object.  If
        // 'COMPATIBLE_TYPE*' is not implicitly convertible to 'ELEMENT_TYPE*',
        // then a compiler diagnostic will be emitted indicating the error.
        // Note that, for factory deleters, 'deleter' must remain valid until
        // all references to 'ptr' have been released.  If 'ptr' is 0, then an
        // internal representation will still be allocated, and this shared
        // pointer will share ownership of a copy of 'deleter'.  Further note
        // that this function is logically equivalent to:
        //..
        //  *this = shared_ptr<ELEMENT_TYPE>(ptr, deleter);
        //..


    template <class COMPATIBLE_TYPE, class DELETER, class ALLOCATOR>
    typename
      enable_if<is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value>::type
    reset(COMPATIBLE_TYPE *ptr,
          DELETER          deleter,
          ALLOCATOR        basicAllocator);
        // Modify this shared pointer to manage the modifiable object of the
        // (template parameter) type 'COMPATIBLE_TYPE' at the specified 'ptr'
        // address, refer to '(ELEMENT_TYPE *)ptr' and use the specified
        // 'deleter' to delete the shared object when all references have been
        // released.  Use the specified 'basicAllocator' to allocate and
        // deallocate the internal representation of the shared pointer.  If
        // this shared pointer is already managing a (possibly shared) object,
        // then, unless an exception is thrown allocating memory to manage
        // 'ptr', release the shared reference to that shared object, and
        // destroy it using its associated deleter if this shared pointer held
        // the last shared reference to that object.  If 'DELETER' is a
        // reference type, then 'deleter' is assumed to be a function-like
        // deleter that may be invoked to destroy the object referred to by a
        // single argument of type 'COMPATIBLE_TYPE *' (i.e., 'deleter(ptr)'
        // will be called to destroy the shared object).  If 'DELETER' is a
        // pointer type, then 'deleter' is assumed to be a pointer to a factory
        // object that exposes a member function that can be invoked as
        // 'deleteObject(ptr)' that will be called to destroy the object at the
        // 'ptr' address (i.e., 'deleter->deleteObject(ptr)' will be called to
        // delete the shared object).  (See the "Deleters" section in the
        // component-level documentation.)  If an exception is thrown
        // allocating the internal representation, then 'deleter(ptr)' is
        // called (or 'deleter->deleteObject(ptr)' for factory-type deleters)
        // and this shared pointer retains ownership of its original object.
        // The behavior is undefined unless 'deleter(ptr)' is a well-defined
        // expression (or 'deleter->deleteObject(ptr)' for factory-type
        // deleters), and unless the copy constructor for 'deleter' does not
        // throw an exception.  If 'COMPATIBLE_TYPE *' is not implicitly
        // convertible to 'ELEMENT_TYPE *', then a compiler diagnostic will be
        // emitted indicating the error.  Note that, for factory deleters, the
        // 'deleter' must remain valid until all references to 'ptr' have been
        // released.  Also note that if 'ptr' is 0, then an internal
        // representation will still be allocated, and this shared pointer will
        // share ownership of a copy of 'deleter'.  Further note that this
        // function is logically equivalent to:
        //..
        //  *this = shared_ptr<ELEMENT_TYPE>(ptr, deleter, basicAllocator);
        //..

    template <class ANY_TYPE>
    void reset(const shared_ptr<ANY_TYPE>& source, ELEMENT_TYPE *ptr);
        // Modify this shared pointer to manage the same modifiable object (if
        // any) as the specified 'source' shared pointer to the (template
        // parameter) type 'ANY_TYPE', and refer to the modifiable object at
        // the specified 'ptr' address (i.e., make this shared pointer an
        // "alias" of 'source').  If this shared pointer is already managing a
        // (possibly shared) object, then release the reference to the shared
        // object, calling the associated deleter to destroy the shared object
        // if this shared pointer is the last reference.  Note that typically
        // the objects referred to by 'source' and 'ptr' have identical
        // lifetimes (e.g., one might be a part of the other), so that the
        // deleter for 'source' will destroy them both, but do not necessarily
        // have the same type.  Also note that if 'source' is empty, then this
        // shared pointer will be reset to an empty state, even if 'ptr' is not
        // null (in which case this empty shared pointer will refer to the same
        // object as 'ptr').  Also note that if 'ptr' is null and 'source' is
        // not empty, then this shared pointer will be reset to a
        // (reference-counted) null pointer alias.  Further note that the
        // behavior of this method is the same as 'loadAlias(source, ptr)'.
        // Finally note that this is a non-standard BDE extension to the C++
        // Standard 'shared_ptr' interface, which does not provide an alias
        // overload for the 'reset' function.

    void swap(shared_ptr& other) BSLS_KEYWORD_NOEXCEPT;
        // Efficiently exchange the states of this shared pointer and the
        // specified 'other' shared pointer such that each will refer to the
        // object formerly referred to by the other and each will manage the
        // object formerly managed by the other.

    // ADDITIONAL BSL MANIPULATORS
    void createInplace();
        // Create "in-place" in a large enough contiguous memory region both an
        // internal representation for this shared pointer and a
        // default-constructed object of 'ELEMENT_TYPE', and make this shared
        // pointer refer to the newly-created 'ELEMENT_TYPE' object.  The
        // currently installed default allocator is used to supply memory.  If
        // an exception is thrown during allocation or construction of the
        // 'ELEMENT_TYPE' object, this shared pointer will be unchanged.
        // Otherwise, if this shared pointer is already managing a (possibly
        // shared) object, then release the shared reference to that shared
        // object, and destroy it using its associated deleter if this shared
        // pointer held the last shared reference to that object.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
    template <class... ARGS>
    void createInplace(BloombergLP::bslma::Allocator *basicAllocator,
                       ARGS&&...                      args);
        // Create "in-place" in a large enough contiguous memory region, using
        // the specified 'basicAllocator' to supply memory, both an internal
        // representation for this shared pointer and an object of
        // 'ELEMENT_TYPE' using the 'ELEMENT_TYPE' constructor that takes the
        // specified 'args...' arguments, and make this shared pointer refer to
        // the newly-created 'ELEMENT_TYPE' object.  If an exception is thrown
        // during the construction of the 'ELEMENT_TYPE' object, this shared
        // pointer will be unchanged.  Otherwise, if this shared pointer is
        // already managing a (possibly shared) object, then release the shared
        // reference to that shared object, and destroy it using its associated
        // deleter if this shared pointer held the last shared reference to
        // that object.  Note that the allocator argument is *not* implicitly
        // passed to the constructor for 'ELEMENT_TYPE'; to construct an object
        // of 'ELEMENT_TYPE' with an allocator, pass the allocator as one of
        // the arguments (typically the last argument), or assign with a
        // 'shared_ptr' created using the standard 'allocate_shared' function.
#endif

    template <class ANY_TYPE>
    void loadAlias(const shared_ptr<ANY_TYPE>&  source,
                   ELEMENT_TYPE                *object);
        // [!DEPRECATED!] Use 'reset' instead.
        //
        // Modify this shared pointer to manage the same modifiable object (if
        // any) as the specified 'source' shared pointer to the (template
        // parameter) type 'ANY_TYPE', and refer to the modifiable object at
        // the specified 'object' address (i.e., make this shared pointer an
        // "alias" of 'source').  If this shared pointer is already managing a
        // (possibly shared) object, then release the shared reference to that
        // shared object, and destroy it using its associated deleter if this
        // shared pointer held the last shared reference to that object.  Note
        // that typically the objects referred to by 'source' and 'object' have
        // identical lifetimes (e.g., one might be a part of the other), so
        // that the deleter for 'source' will destroy them both, but they do
        // not necessarily have the same type.  Also note that if 'source' is
        // empty, then this shared pointer will be reset to an empty state,
        // even if 'object' is not null (in which case this empty shared
        // pointer will refer to the same object as 'object').  Also note that
        // if 'object' is null and 'source' is not empty, then this shared
        // pointer will be reset to a (reference-counted) null pointer alias.
        // Also note that this function is logically equivalent to:
        //..
        //  *this = shared_ptr<ELEMENT_TYPE>(source, object);
        //..
        // Further note that the behavior of this method is the same as
        // 'reset(source, object)'.

    pair<ELEMENT_TYPE *, BloombergLP::bslma::SharedPtrRep *> release()
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Return the pair consisting of the addresses of the modifiable
        // 'ELEMENT_TYPE' object referred to, and the representation shared by,
        // this shared pointer, and reset this shared pointer to the empty
        // state, referring to no object, with no effect on the representation.
        // The reference counter is not modified nor is the shared object
        // deleted; if the reference count of the representation is greater
        // than one, then it is not safe to release the representation (thereby
        // destroying the shared object), but it is always safe to create
        // another shared pointer with the representation using the constructor
        // with the signature
        // 'shared_ptr(ELEMENT_TYPE                     *ptr,
        //             BloombergLP::bslma::SharedPtrRep *rep)'.
        // Note that this function returns a pair of null pointers if this
        // shared pointer is empty.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // DEPRECATED BDE LEGACY MANIPULATORS
    void clear() BSLS_KEYWORD_NOEXCEPT;
        // [!DEPRECATED!] Use 'reset' instead.
        //
        // Reset this shared pointer to the empty state.  If this shared
        // pointer is managing a (possibly shared) object, then release the
        // reference to the shared object, calling the associated deleter to
        // destroy the shared object if this shared pointer is the last
        // reference.  Note that the behavior of this method is the same as
        // 'reset()'.

    template <class COMPATIBLE_TYPE>
    void load(COMPATIBLE_TYPE *ptr);
        // [!DEPRECATED!] Use 'reset' instead.
        //
        // Modify this shared pointer to manage the modifiable object of the
        // (template parameter) type 'COMPATIBLE_TYPE' at the specified 'ptr'
        // address and to refer to '(ELEMENT_TYPE *)ptr'.  If this shared
        // pointer is already managing a (possibly shared) object, then, unless
        // an exception is thrown allocating memory to manage 'ptr', release
        // the reference to the shared object, calling the associated deleter
        // to destroy the shared object if this shared pointer is the last
        // reference.  The currently installed default allocator is used to
        // allocate the internal representation of this shared pointer, and the
        // shared object will be destroyed by a call to 'delete ptr' when all
        // references have been released.  If an exception is thrown allocating
        // the internal representation, then 'delete ptr' is called and this
        // shared pointer retains ownership of its original object.  If
        // 'COMPATIBLE_TYPE*' is not implicitly convertible to 'ELEMENT_TYPE*',
        // then a compiler diagnostic will be emitted indicating the error.
        // Note that if 'ptr' is 0, then this shared pointer will still
        // allocate an internal representation to share ownership of that empty
        // state, which will be reclaimed when the last reference is destroyed.
        // Also note also that the behavior of this method is the same as
        // 'reset(ptr)'.

    template <class COMPATIBLE_TYPE>
    void load(COMPATIBLE_TYPE               *ptr,
              BloombergLP::bslma::Allocator *basicAllocator);
        // [!DEPRECATED!] Use 'reset' instead.
        //
        // Modify this shared pointer to manage the modifiable object of the
        // (template parameter) type 'COMPATIBLE_TYPE' at the specified 'ptr'
        // address and to refer to '(ELEMENT_TYPE *)ptr'.  If this shared
        // pointer is already managing a (possibly shared) object, then, unless
        // an exception is thrown allocating memory to manage 'ptr', release
        // the reference to the shared object, calling the associated deleter
        // to destroy the shared object if this shared pointer is the last
        // reference.  Use the specified 'basicAllocator' to allocate the
        // internal representation of this shared pointer and to destroy the
        // shared object when all references have been released; if
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  If an exception is thrown allocating the internal
        // representation, then destroy '*ptr' with a call to
        // 'alloc->deleteObject(ptr)' where 'alloc' is the chosen allocator,
        // and this shared pointer retains ownership of its original object.
        // If 'COMPATIBLE_TYPE *' is not implicitly convertible to
        // 'ELEMENT_TYPE *', then a compiler diagnostic will be emitted
        // indicating the error.  Note that if 'ptr' is 0, then this shared
        // pointer will still allocate an internal representation to share
        // ownership of that empty state, which will be reclaimed when the last
        // reference is destroyed.  Also note that this function is logically
        // equivalent to:
        //..
        //  *this = shared_ptr<ELEMENT_TYPE>(ptr, basicAllocator);
        //..

    template <class COMPATIBLE_TYPE, class DELETER>
    void load(COMPATIBLE_TYPE               *ptr,
              const DELETER&                 deleter,
              BloombergLP::bslma::Allocator *basicAllocator);
        // [!DEPRECATED!] Use 'reset' instead.
        //
        // Modify this shared pointer to manage the modifiable object of the
        // (template parameter) type 'COMPATIBLE_TYPE' at the specified 'ptr'
        // address, refer to '(ELEMENT_TYPE *)ptr' and use the specified
        // 'deleter' to delete the shared object when all references have been
        // released.  Use the specified 'basicAllocator' to allocate and
        // deallocate the internal representation of the shared pointer.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  If this shared pointer is already managing a (possibly
        // shared) object, then, unless an exception is thrown creating storage
        // to manage 'ptr', release the shared reference to that shared object,
        // and destroy it using its associated deleter if this shared pointer
        // held the last shared reference to that object.  If 'DELETER' is a
        // reference type, then 'deleter' is assumed to be a function-like
        // deleter that may be invoked to destroy the object referred to by a
        // single argument of type 'COMPATIBLE_TYPE *' (i.e., 'deleter(ptr)'
        // will be called to destroy the shared object).  If 'DELETER' is a
        // pointer type, then 'deleter' is assumed to be a pointer to a factory
        // object that exposes a member function that can be invoked as
        // 'deleteObject(ptr)' that will be called to destroy the object at the
        // 'ptr' address (i.e., 'deleter->deleteObject(ptr)' will be called to
        // delete the shared object).  (See the "Deleters" section in the
        // component-level documentation.)  If an exception is thrown
        // allocating the internal representation, then 'deleter(ptr)' is
        // called (or 'deleter->deleteObject(ptr)' for factory-type deleters)
        // and this shared pointer retains ownership of its original object.
        // The behavior is undefined unless 'deleter(ptr)' is a well-defined
        // expression (or 'deleter->deleteObject(ptr)' for factory-type
        // deleters), and unless the copy constructor for 'deleter' does not
        // throw an exception.  If 'COMPATIBLE_TYPE *' is not implicitly
        // convertible to 'ELEMENT_TYPE *', then a compiler diagnostic will be
        // emitted indicating the error.  Note that, for factory deleters, the
        // 'deleter' must remain valid until all references to 'ptr' have been
        // released.  Also note that if 'ptr' is 0, then an internal
        // representation will still be allocated, and this shared pointer will
        // share ownership of a copy of 'deleter'.  Further note that this
        // function is logically equivalent to:
        //..
        //  *this = shared_ptr<ELEMENT_TYPE>(ptr, deleter, basicAllocator);
        //..

#endif // BDE_OMIT_INTERNAL_DEPRECATED

    // ACCESSORS
    operator BoolType() const BSLS_KEYWORD_NOEXCEPT;
        // Return a value of an "unspecified bool" type that evaluates to
        // 'false' if this shared pointer does not refer to an object, and
        // 'true' otherwise.  Note that this conversion operator allows a
        // shared pointer to be used within a conditional context (e.g., within
        // an 'if' or 'while' statement), but does *not* allow shared pointers
        // to unrelated types to be compared (e.g., via '<' or '>').

    typename add_lvalue_reference<ELEMENT_TYPE>::type
    operator*() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reference providing modifiable access to the object
        // referred to by this shared pointer.  The behavior is undefined
        // unless this shared pointer refers to an object, and 'ELEMENT_TYPE'
        // is not (potentially 'const' or 'volatile' qualified) 'void'.

    ELEMENT_TYPE *operator->() const BSLS_KEYWORD_NOEXCEPT;
        // Return the address providing modifiable access to the object
        // referred to by this shared pointer, or 0 if this shared pointer does
        // not refer to an object.  Note that applying this operator
        // conventionally (e.g., to invoke a method) to an shared pointer that
        // does not refer to an object will result in undefined behavior.

    element_type *get() const BSLS_KEYWORD_NOEXCEPT;
        // Return the address providing modifiable access to the object
        // referred to by this shared pointer, or 0 if this shared pointer does
        // not refer to an object.

    typename add_lvalue_reference<element_type>::type
    operator[](ptrdiff_t index) const;
        // Return a reference providing modifiable access to the object at the
        // specified 'index' offset in the object referred to by this shared
        // pointer.  The behavior is undefined unless this shared pointer is
        // not empty, 'ELEMENT_TYPE' is not 'void' (a compiler error will be
        // generated if this operator is instantiated within the
        // 'shared_ptr<void>' class), and this shared pointer refers to an
        // array of 'ELEMENT_TYPE' objects.  Instead of 'element_type &', we
        // use 'add_lvalue_reference<element_type>::type' for the return type
        // because that allows people to instantiate 'shared_ptr<cv_void>', as
        // long as they don't use this method.  Note that this method is
        // logically equivalent to '*(get() + index)'.

    template<class ANY_TYPE>
    bool owner_before(const shared_ptr<ANY_TYPE>& other) const
                                                         BSLS_KEYWORD_NOEXCEPT;
    template<class ANY_TYPE>
    bool owner_before(const weak_ptr<ANY_TYPE>& other) const
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if the address of the
        // 'BloombergLP::bslma::SharedPtrRep' object used by this shared
        // pointer is ordered before the address of the
        // 'BloombergLP::bslma::SharedPtrRep' object used by the specified
        // 'other' shared pointer under the total ordering defined by
        // 'std::less<BloombergLP::bslma::SharedPtrRep *>', and 'false'
        // otherwise.

    BSLS_DEPRECATE_FEATURE("bsl",
                           "deprecated_cpp17_standard_library_features",
                           "do not use")
    bool unique() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if this shared pointer is not empty and does not share
        // ownership of the object it managed with any other shared pointer,
        // and 'false' otherwise.  Note that a shared pointer with a custom
        // deleter can refer to a null pointer without being empty, and so may
        // be 'unique'.  Also note that the result of this function may not be
        // reliable in a multi-threaded program, where a weak pointer may be
        // locked on another thread.
        //
        // DEPRECATED: This function is deprecated in C++17 because its
        // correctness is not guaranteed since the value returned by the used
        // 'use_count' function is approximate.

    long use_count() const BSLS_KEYWORD_NOEXCEPT;
        // Return a "snapshot" of the number of shared pointers (including this
        // one) that share ownership of the object managed by this shared
        // pointer.  Note that 0 is returned if this shared pointer is empty.
        // Also note that any result other than 0 may be unreliable in a
        // multi-threaded program, where another pointer sharing ownership in a
        // different thread may be copied or destroyed, or a weak pointer may
        // be locked in the case that 1 is returned (that would otherwise
        // indicate unique ownership).

    // ADDITIONAL BSL ACCESSORS
    BloombergLP::bslma::ManagedPtr<ELEMENT_TYPE> managedPtr() const;
        // Return a managed pointer that refers to the same object as this
        // shared pointer.  If this shared pointer is not empty, and is not
        // null, then increment the shared count on the shared object, and give
        // the managed pointer a deleter that decrements the reference count
        // for the shared object.  Note that if this 'shared_ptr' is reference-
        // counting a null pointer, the empty 'bslma::ManagedPtr' returned will
        // not participate in that shared ownership.

    BloombergLP::bslma::SharedPtrRep *rep() const BSLS_KEYWORD_NOEXCEPT;
        // Return the address providing modifiable access to the
        // 'BloombergLP::bslma::SharedPtrRep' object used by this shared
        // pointer, or 0 if this shared pointer is empty.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // DEPRECATED BDE LEGACY ACCESSORS
    int numReferences() const BSLS_KEYWORD_NOEXCEPT;
        // [!DEPRECATED!] Use 'use_count' instead.
        //
        // Return a "snapshot" of the number of shared pointers (including this
        // one) that share ownership of the object managed by this shared
        // pointer.  Note that the behavior of this function is the same as
        // 'use_count', and the result may be unreliable in multi-threaded code
        // for the same reasons.

    ELEMENT_TYPE *ptr() const BSLS_KEYWORD_NOEXCEPT;
        // [!DEPRECATED!] Use 'get' instead.
        //
        // Return the address providing modifiable access to the object
        // referred to by this shared pointer, or 0 if this shared pointer does
        // not refer to an object.  Note that the behavior of this function is
        // the same as 'get'.
#endif // BDE_OMIT_INTERNAL_DEPRECATED
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
// CLASS TEMPLATE DEDUCTION GUIDES

//  The obvious deduction guide:
//  template <class T>
//  shared_ptr(T*) -> shared_ptr<T>;
//  is not provided because there's no way to distinguish from T* and T[].

template<class ELEMENT_TYPE>
shared_ptr(weak_ptr<ELEMENT_TYPE>) -> shared_ptr<ELEMENT_TYPE>;
    // Deduce the specified type 'ELEMENT_TYPE' corresponding template
    // parameter of the 'bsl::weak_ptr' supplied to the constructor of
    // 'shared_ptr'.

template<class ELEMENT_TYPE, class DELETER>
shared_ptr(std::unique_ptr<ELEMENT_TYPE, DELETER>)
-> shared_ptr<ELEMENT_TYPE>;
    // Deduce the specified type 'ELEMENT_TYPE' corresponding template
    // parameter of the 'std::unique_ptr' supplied to the constructor of
    // 'shared_ptr'.

template<class ELEMENT_TYPE,
         class DELETER,
         class ALLOC,
         class = typename bsl::enable_if_t<
               bsl::is_convertible_v<ALLOC *, BloombergLP::bslma::Allocator *>>
         >
shared_ptr(std::unique_ptr<ELEMENT_TYPE, DELETER>, ALLOC *)
-> shared_ptr<ELEMENT_TYPE>;
    // Deduce the specified type 'ELEMENT_TYPE' corresponding template
    // parameter of the 'std::unique_ptr' supplied to the constructor of
    // 'shared_ptr'.  This guide does not participate in deduction unless the
    // specified 'ALLOC' inherits from 'bslma::Allocator'.

// Deduction guides for 'auto_ptr' and 'auto_ptr_ref' are deliberately not
// provided, since auto_ptr has been removed from C++17.

template<class ELEMENT_TYPE>
shared_ptr(BloombergLP::bslma::ManagedPtr<ELEMENT_TYPE>)
-> shared_ptr<ELEMENT_TYPE>;
    // Deduce the specified type 'ELEMENT_TYPE' corresponding template
    // parameter of the 'bslma::ManagedPtr' supplied to the constructor of
    // 'shared_ptr'.

template<class ELEMENT_TYPE,
         class ALLOC,
         class = typename bsl::enable_if_t<
               bsl::is_convertible_v<ALLOC *, BloombergLP::bslma::Allocator *>>
         >
shared_ptr(BloombergLP::bslma::ManagedPtr<ELEMENT_TYPE>, ALLOC *)
-> shared_ptr<ELEMENT_TYPE>;
    // Deduce the specified type 'ELEMENT_TYPE' corresponding template
    // parameter of the 'bslma::ManagedPtr' supplied to the constructor of
    // 'shared_ptr'.  This guide does not participate in deduction unless the
    // specified 'ALLOC' inherits from 'bslma::Allocator'.
#endif

// FREE OPERATORS
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const shared_ptr<LHS_TYPE>& lhs,
                const shared_ptr<RHS_TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'lhs' shared pointer refers to the same
    // object (if any) as that referred to by the specified 'rhs' shared
    // pointer (if any), and 'false' otherwise; a compiler diagnostic will be
    // emitted indicating the error unless a (raw) pointer to 'LHS_TYPE' can
    // be compared to a (raw) pointer to 'RHS_TYPE'.  Note that two shared
    // pointers that compare equal do not necessarily manage the same object
    // due to aliasing.

template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const shared_ptr<LHS_TYPE>& lhs,
                const shared_ptr<RHS_TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'lhs' shared pointer does not refer to
    // the same object (if any) as that referred to by the specified 'rhs'
    // shared pointer (if any), and 'false' otherwise; a compiler diagnostic
    // will be emitted indicating the error unless a (raw) pointer to
    // 'LHS_TYPE' can be compared to a (raw) pointer to 'RHS_TYPE'.  Note that
    // two shared pointers that do not compare equal may manage the same object
    // due to aliasing.

template<class LHS_TYPE, class RHS_TYPE>
bool operator<(const shared_ptr<LHS_TYPE>& lhs,
               const shared_ptr<RHS_TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the address of the object that the specified 'lhs'
    // shared pointer refers to is ordered before the address of the object
    // that the specified 'rhs' shared pointer refers to under the total
    // ordering supplied by 'std::less<T *>', where 'T *' is the composite
    // pointer type of 'LHS_TYPE *' and 'RHS_TYPE *', and 'false' otherwise.

template<class LHS_TYPE, class RHS_TYPE>
bool operator>(const shared_ptr<LHS_TYPE>& lhs,
               const shared_ptr<RHS_TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the address of the object that the specified 'lhs'
    // shared pointer refers to is ordered after the address of the object
    // that the specified 'rhs' shared pointer refers to under the total
    // ordering supplied by 'std::less<T *>', where 'T *' is the composite
    // pointer type of 'LHS_TYPE *' and 'RHS_TYPE *', and 'false' otherwise.

template<class LHS_TYPE, class RHS_TYPE>
bool operator<=(const shared_ptr<LHS_TYPE>& lhs,
                const shared_ptr<RHS_TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'lhs' shared pointer refers to the same
    // object as the specified 'rhs' shared pointer, or if the address of the
    // object referred to by 'lhs' (if any) is ordered before the address of
    // the object referred to by 'rhs' (if any) under the total ordering
    // supplied by 'std::less<T *>', where 'T *' is the composite pointer type
    // of 'LHS_TYPE *' and 'RHS_TYPE *', and 'false' otherwise.

template<class LHS_TYPE, class RHS_TYPE>
bool operator>=(const shared_ptr<LHS_TYPE>& lhs,
                const shared_ptr<RHS_TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'lhs' shared pointer refers to the same
    // object as the specified 'rhs' shared pointer, or if the address of the
    // object referred to by 'lhs' (if any) is ordered after the address of the
    // object referred to by 'rhs' (if any) under the total ordering supplied
    // by 'std::less<T *>', where 'T *' is the composite pointer type of
    // 'LHS_TYPE *' and 'RHS_TYPE *', and 'false' otherwise.

template <class LHS_TYPE>
bool operator==(const shared_ptr<LHS_TYPE>& lhs,
                nullptr_t) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'lhs' shared pointer does not refer to an
    // object, and 'false' otherwise.

template <class RHS_TYPE>
bool operator==(nullptr_t,
                const shared_ptr<RHS_TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'rhs' shared pointer does not refer to an
    // object, and 'false' otherwise.

template <class LHS_TYPE>
bool operator!=(const shared_ptr<LHS_TYPE>& lhs,
                nullptr_t) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'lhs' shared pointer refers to an object,
    // and 'false' otherwise.

template <class RHS_TYPE>
bool operator!=(nullptr_t,
                const shared_ptr<RHS_TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'rhs' shared pointer refers to an object,
    // and 'false' otherwise.

template <class LHS_TYPE>
bool operator<(const shared_ptr<LHS_TYPE>& lhs, nullptr_t)
                                                         BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the address of the object referred to by the specified
    // 'lhs' shared pointer is ordered before the null-pointer value under the
    // total ordering supplied by 'std::less<LHS_TYPE *>', and 'false'
    // otherwise.

template <class RHS_TYPE>
bool operator<(nullptr_t, const shared_ptr<RHS_TYPE>& rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the address of the object referred to by the specified
    // 'rhs' shared pointer is ordered after the null-pointer value under the
    // total ordering supplied by 'std::less<RHS_TYPE *>', and 'false'
    // otherwise.

template <class LHS_TYPE>
bool operator<=(const shared_ptr<LHS_TYPE>& lhs,
                nullptr_t) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'lhs' shared pointer does not refer to an
    // object, or if the address of the object referred to by 'lhs' is ordered
    // before the null-pointer value under the total ordering supplied by
    // 'std::less<LHS_TYPE *>', and 'false' otherwise.

template <class RHS_TYPE>
bool operator<=(nullptr_t,
                const shared_ptr<RHS_TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'rhs' shared pointer does not refer to an
    // object, or if the address of the object referred to by 'rhs' is ordered
    // after the null-pointer value under the total ordering supplied by
    // 'std::less<RHS_TYPE *>', and 'false' otherwise.

template <class LHS_TYPE>
bool operator>(const shared_ptr<LHS_TYPE>& lhs, nullptr_t)
                                                         BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the address of the object referred to by the specified
    // 'lhs' shared pointer is ordered after the null-pointer value under the
    // total ordering supplied by 'std::less<LHS_TYPE *>', and 'false'
    // otherwise.

template <class RHS_TYPE>
bool operator>(nullptr_t, const shared_ptr<RHS_TYPE>& rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the address of the object referred to by the specified
    // 'rhs' shared pointer is ordered before the null-pointer value under the
    // total ordering supplied by 'std::less<RHS_TYPE *>', and 'false'
    // otherwise.

template <class LHS_TYPE>
bool operator>=(const shared_ptr<LHS_TYPE>& lhs,
                nullptr_t) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'lhs' shared pointer does not refer to an
    // object, or if the address of the object referred to by 'lhs' is ordered
    // after the null-pointer value under the total ordering supplied by
    // 'std::less<LHS_TYPE *>', and 'false' otherwise.

template <class RHS_TYPE>
bool operator>=(nullptr_t,
                const shared_ptr<RHS_TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'rhs' shared pointer does not refer to an
    // object, or if the address of the object referred to by 'rhs' is ordered
    // before the null-pointer value under the total ordering supplied by
    // 'std::less<RHS_TYPE *>', and 'false' otherwise.

template<class CHAR_TYPE, class CHAR_TRAITS, class ELEMENT_TYPE>
std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>&
operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>& stream,
           const shared_ptr<ELEMENT_TYPE>&             rhs);
    // Print to the specified 'stream' the address of the shared object
    // referred to by the specified 'rhs' shared pointer and return a reference
    // to the modifiable 'stream'.

// ASPECTS
template <class HASHALG, class ELEMENT_TYPE>
void hashAppend(HASHALG& hashAlg, const shared_ptr<ELEMENT_TYPE>& input);
    // Pass the address of the object referred to by the specified 'input'
    // shared pointer to the specified 'hashAlg' hashing algorithm of (template
    // parameter) type 'HASHALG'.

template <class ELEMENT_TYPE>
void swap(shared_ptr<ELEMENT_TYPE>& a, shared_ptr<ELEMENT_TYPE>& b)
                                                         BSLS_KEYWORD_NOEXCEPT;
    // Efficiently exchange the states of the specified 'a' and 'b' shared
    // pointers such that each will refer to the object formerly referred to by
    // the other, and each will manage the object formerly managed by the
    // other.

// STANDARD FREE FUNCTIONS
template<class DELETER, class ELEMENT_TYPE>
DELETER *get_deleter(const shared_ptr<ELEMENT_TYPE>& p) BSLS_KEYWORD_NOEXCEPT;
    // Return the address of deleter used by the specified 'p' shared pointer
    // if the (template parameter) type 'DELETER' is the type of the deleter
    // installed in 'p', and a null pointer value otherwise.

// STANDARD CAST FUNCTIONS
template<class TO_TYPE, class FROM_TYPE>
shared_ptr<TO_TYPE> const_pointer_cast(const shared_ptr<FROM_TYPE>& source)
                                                         BSLS_KEYWORD_NOEXCEPT;
    // Return a 'shared_ptr<TO_TYPE>' object sharing ownership of the same
    // object as the specified 'source' shared pointer to the (template
    // parameter) 'FROM_TYPE', and referring to
    // 'const_cast<TO_TYPE *>(source.get())'.  Note that if 'source' cannot be
    // 'const'-cast to 'TO_TYPE *', then a compiler diagnostic will be emitted
    // indicating the error.

template<class TO_TYPE, class FROM_TYPE>
shared_ptr<TO_TYPE> dynamic_pointer_cast(const shared_ptr<FROM_TYPE>& source)
                                                         BSLS_KEYWORD_NOEXCEPT;
    // Return a 'shared_ptr<TO_TYPE>' object sharing ownership of the same
    // object as the specified 'source' shared pointer to the (template
    // parameter) 'FROM_TYPE', and referring to
    // 'dynamic_cast<TO_TYPE*>(source.get())'.  If 'source' cannot be
    // dynamically cast to 'TO_TYPE *', then an empty 'shared_ptr<TO_TYPE>'
    // object is returned.

template<class TO_TYPE, class FROM_TYPE>
shared_ptr<TO_TYPE> static_pointer_cast(const shared_ptr<FROM_TYPE>& source)
                                                         BSLS_KEYWORD_NOEXCEPT;
    // Return a 'shared_ptr<TO_TYPE>' object sharing ownership of the same
    // object as the specified 'source' shared pointer to the (template
    // parameter) 'FROM_TYPE', and referring to
    // 'static_cast<TO_TYPE *>(source.get())'.  Note that if 'source' cannot be
    // statically cast to 'TO_TYPE *', then a compiler diagnostic will be
    // emitted indicating the error.

template<class TO_TYPE, class FROM_TYPE>
shared_ptr<TO_TYPE> reinterpret_pointer_cast(
                    const shared_ptr<FROM_TYPE>& source) BSLS_KEYWORD_NOEXCEPT;
    // Return a 'shared_ptr<TO_TYPE>' object sharing ownership of the same
    // object as the specified 'source' shared pointer to the (template
    // parameter) 'FROM_TYPE', and referring to
    // 'reinterpret_cast<TO_TYPE *>(source.get())'.  Note that if 'source'
    // cannot be reinterpret_cast-ed to 'TO_TYPE *', then a compiler diagnostic
    // will be emitted indicating the error.


// STANDARD FACTORY FUNCTIONS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
template<class ELEMENT_TYPE, class ALLOC, class... ARGS>
# if defined(BSLSTL_SHAREDPTR_NO_PARTIAL_ORDER_ON_ALLOCATOR_POINTER)
    // work-around for gcc variadic template bug - confirm not fixed by gcc 9
typename enable_if<!is_pointer<ALLOC>::value && !is_array<ELEMENT_TYPE>::value,
                   shared_ptr<ELEMENT_TYPE> >::type
# else
typename enable_if<!is_array<ELEMENT_TYPE>::value,
                   shared_ptr<ELEMENT_TYPE> >::type
// shared_ptr<ELEMENT_TYPE>
# endif
allocate_shared(ALLOC basicAllocator, ARGS&&... args);
    // Return a 'shared_ptr' object referring to and managing a new
    // 'ELEMENT_TYPE' object.  The specified 'basicAllocator' will be used to
    // supply a single contiguous region of memory holding the returned shared
    // pointer's internal representation and the new 'ELEMENT_TYPE' object,
    // which is initialized by calling 'allocator_traits<ALLOC>::construct'
    // passing 'basicAllocator', an 'ELEMENT_TYPE *' pointer to space for the
    // new shared object, and the specified arguments
    // 'std::forward<ARGS>(args)...'.

template<class ELEMENT_TYPE, class ALLOC, class... ARGS>
typename enable_if<!is_array<ELEMENT_TYPE>::value,
                   shared_ptr<ELEMENT_TYPE> >::type
allocate_shared(ALLOC *basicAllocator, ARGS&&... args);
    // Return a 'shared_ptr' object referring to and managing a new
    // 'ELEMENT_TYPE' object.  The specified 'basicAllocator' will be used to
    // supply a single contiguous region of memory holding the returned shared
    // pointer's internal representation and the new 'ELEMENT_TYPE' object,
    // which is initialized using the 'ELEMENT_TYPE' constructor that takes the
    // specified arguments 'std::forward<ARGS>(args)...'.  If 'ELEMENT_TYPE'
    // uses 'bslma' allocators, then 'basicAllocator' is passed as an extra
    // argument in the final position.  If 'basicAllocator' is 0, then the
    // default allocator will be used instead, and passed as the allocator,
    // when appropriate, to the 'ELEMENT_TYPE' constructor.

template<class ELEMENT_TYPE, class... ARGS>
typename enable_if<!is_array<ELEMENT_TYPE>::value,
                   shared_ptr<ELEMENT_TYPE> >::type
make_shared(ARGS&&... args);
    // Return a 'shared_ptr' object referring to and managing a new
    // 'ELEMENT_TYPE' object.  The default allocator will be used to supply a
    // single contiguous region of memory holding the returned shared pointer's
    // internal representation and the new 'ELEMENT_TYPE' object, which is
    // initialized using the 'ELEMENT_TYPE' constructor that takes the
    // specified arguments 'std::forward<ARGS>(args)...'.  If 'ELEMENT_TYPE'
    // uses 'bslma' allocators, then the default allocator is passed as an
    // extra argument in the final position.
#endif

                        // ==============
                        // class weak_ptr
                        // ==============

template <class ELEMENT_TYPE>
class weak_ptr {
    // This 'class' provides a mechanism to create weak references to
    // reference-counted shared ('shared_ptr') objects.  A weak reference
    // provides conditional access to a shared object managed by a
    // 'shared_ptr', but, unlike a shared (or "strong") reference, does not
    // affect the shared object's lifetime.

    // DATA
    ELEMENT_TYPE                     *d_ptr_p; // pointer to the referenced
                                               // object

    BloombergLP::bslma::SharedPtrRep *d_rep_p; // pointer to the representation
                                               // object that manages the
                                               // shared object (held, not
                                               // owned)

    // PRIVATE MANIPULATORS
    void privateAssign(BloombergLP::bslma::SharedPtrRep *rep,
                       ELEMENT_TYPE                     *target);
        // Release weak ownership of the currently managed shared pointer rep
        // and assign to this weak pointer weak ownership of the specified
        // shared pointer 'rep', aliasing the specified 'target' pointer.

    // FRIENDS
    template <class COMPATIBLE_TYPE>
    friend class weak_ptr;
        // This 'friend' declaration provides access to the internal data
        // members while constructing a weak pointer from a weak pointer of a
        // different type.

    friend struct BloombergLP::bslstl::SharedPtr_ImpUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(weak_ptr<ELEMENT_TYPE>,
                                   bsl::is_nothrow_move_constructible);

    // TYPES
    typedef typename bsl::remove_extent<ELEMENT_TYPE>::type element_type;
        // For weak pointers to non-array types, 'element_type' is an alias to
        // the 'ELEMENT_TYPE' template parameter.  Otherwise, it is an alias to
        // the type contained in the array.

    // CREATORS
    BSLS_KEYWORD_CONSTEXPR
    weak_ptr() BSLS_KEYWORD_NOEXCEPT;
        // Create a weak pointer in the empty state and referring to no object,
        // i.e., a weak pointer having no representation.

    weak_ptr(BloombergLP::bslmf::MovableRef<weak_ptr> original)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a weak pointer that refers to the same object (if any) as the
        // specified 'original' weak pointer, and reset 'original' to an empty
        // state.

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    template <class COMPATIBLE_TYPE
              BSLSTL_SHAREDPTR_DECLARE_IF_COMPATIBLE>
    weak_ptr(weak_ptr<COMPATIBLE_TYPE>&& other) BSLS_KEYWORD_NOEXCEPT;
#else
    template <class COMPATIBLE_TYPE
              BSLSTL_SHAREDPTR_DECLARE_IF_COMPATIBLE>
    weak_ptr(BloombergLP::bslmf::MovableRef<weak_ptr<COMPATIBLE_TYPE> > other)
                                                         BSLS_KEYWORD_NOEXCEPT;
#endif
        // Create a weak pointer that refers to the same object (if any) as the
        // specified 'other' weak pointer, and reset 'original' to an empty
        // state.  Note that this operation does not involve any change to
        // reference counts.

    weak_ptr(const weak_ptr& original) BSLS_KEYWORD_NOEXCEPT;
        // Create a weak pointer that refers to the same object (if any) as the
        // specified 'original' weak pointer, and increment the number of weak
        // references to the object managed by 'original' (if any).  Note that
        // if 'original' is in the empty state, this weak pointer will be
        // initialized to the empty state.

    template <class COMPATIBLE_TYPE
              BSLSTL_SHAREDPTR_DECLARE_IF_COMPATIBLE>
    weak_ptr(const shared_ptr<COMPATIBLE_TYPE>& other) BSLS_KEYWORD_NOEXCEPT;
                                                                    // IMPLICIT
    template <class COMPATIBLE_TYPE
              BSLSTL_SHAREDPTR_DECLARE_IF_COMPATIBLE>
    weak_ptr(const weak_ptr<COMPATIBLE_TYPE>& other) BSLS_KEYWORD_NOEXCEPT;
                                                                    // IMPLICIT
        // Create a weak pointer that refers to the same object (if any) as the
        // specified 'other' (shared or weak) pointer of the (template
        // parameter) 'COMPATIBLE_TYPE', and increment the number of weak
        // references to the object managed by 'other' (if any).  If
        // 'COMPATIBLE_TYPE *' is not implicitly convertible to
        // 'ELEMENT_TYPE *', then a compiler diagnostic will be emitted.  Note
        // that if 'other' is in the empty state, this weak pointer will be
        // initialized to the empty state.

    ~weak_ptr();
        // Destroy this weak pointer object.  If this weak pointer manages a
        // (possibly shared) object, release the weak reference to that object.

    // MANIPULATORS
    weak_ptr& operator=(BloombergLP::bslmf::MovableRef<weak_ptr> rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Make this weak pointer refer to the same object (if any) as the
        // specified 'rhs' weak pointer.  If 'rhs' is not a reference to this
        // weak pointer, decrement the number of weak references to the object
        // this weak pointer managed (if any), and reset 'rhs' to an empty
        // state.  Return a reference providing modifiable access to this weak
        // pointer.  Note that if 'rhs' is in an empty state, this weak pointer
        // will be set to an empty state.

    weak_ptr& operator=(const weak_ptr& rhs) BSLS_KEYWORD_NOEXCEPT;
        // Make this weak pointer refer to the same object (if any) as the
        // specified 'rhs' weak pointer.  Decrement the number of weak
        // references to the object this weak pointer manages (if any), and
        // increment the number of weak references to the object managed by
        // 'rhs' (if any).  Return a reference providing modifiable access to
        // this weak pointer.  Note that if 'rhs' is in an empty state, this
        // weak pointer will be set to an empty state.

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    template <class COMPATIBLE_TYPE>
    typename enable_if<
     is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value, weak_ptr&>::type
    operator=(weak_ptr<COMPATIBLE_TYPE>&& rhs) BSLS_KEYWORD_NOEXCEPT;
#else
    template <class COMPATIBLE_TYPE>
    typename enable_if<
     is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value, weak_ptr&>::type
    operator=(BloombergLP::bslmf::MovableRef<weak_ptr<COMPATIBLE_TYPE> > rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;
#endif
        // Make this weak pointer refer to the same object (if any) as the
        // specified 'rhs' weak pointer.  Decrement the number of weak
        // references to the object this weak pointer managed (if any), and
        // reset 'rhs' to an empty state.  Return a reference providing
        // modifiable access to this weak pointer.  This function does not
        // exist unless a pointer to (the template parameter) 'COMPATIBLE_TYPE'
        // is convertible to a pointer to (the template parameter)
        // 'ELEMENT_TYPE'.  Note that if 'rhs' is in an empty state, this weak
        // pointer will be set to an empty state.

    template <class COMPATIBLE_TYPE>
    typename enable_if<
     is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value, weak_ptr&>::type
    operator=(const shared_ptr<COMPATIBLE_TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT;
    template <class COMPATIBLE_TYPE>
    typename enable_if<
     is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value, weak_ptr&>::type
    operator=(const weak_ptr<COMPATIBLE_TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT;
        // Make this weak pointer refer to the same object (if any) as the
        // specified 'rhs' (shared or weak) pointer to the (template parameter)
        // 'COMPATIBLE_TYPE'.  Decrement the number of weak references to the
        // object to which this weak pointer currently manages (if any), and
        // increment the number of weak references to the object managed by
        // 'rhs' (if any).  Return a reference providing modifiable access to
        // this weak pointer.  If 'COMPATIBLE_TYPE *' is not implicitly
        // convertible to 'TYPE *', then a compiler diagnostic will be emitted.
        // Note that if 'rhs' is in the empty state, this weak pointer will be
        // set to the empty state.

    void reset() BSLS_KEYWORD_NOEXCEPT;
        // Reset this weak pointer to the empty state.  If this weak pointer
        // manages a (possibly shared) object, then decrement the number of
        // weak references to that object.

    void swap(weak_ptr& other) BSLS_KEYWORD_NOEXCEPT;
        // Efficiently exchange the states of this weak pointer and the
        // specified 'other' weak pointer such that each will refer to the
        // object (if any) and representation (if any) formerly referred to and
        // managed by the other.

    // ACCESSORS
    bool expired() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if this weak pointer is in the empty state or the
        // object that it originally referenced has been destroyed, and 'false'
        // otherwise.

    shared_ptr<ELEMENT_TYPE> lock() const BSLS_KEYWORD_NOEXCEPT;
        // Return a shared pointer to the object referred to by this weak
        // pointer if 'false == expired()', and a shared pointer in the empty
        // state otherwise.

    template <class ANY_TYPE>
    bool owner_before(const shared_ptr<ANY_TYPE>& other) const
                                                         BSLS_KEYWORD_NOEXCEPT;
    template <class ANY_TYPE>
    bool owner_before(const weak_ptr<ANY_TYPE>& other) const
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if the address of the
        // 'BloombergLP::bslma::SharedPtrRep' object used by this weak pointer
        // is ordered before the address of the
        // 'BloombergLP::bslma::SharedPtrRep' object used by the specified
        // 'other' shared pointer under the total ordering defined by
        // 'std::less<BloombergLP::bslma::SharedPtrRep *>', and 'false'
        // otherwise.

    BloombergLP::bslma::SharedPtrRep *rep() const BSLS_KEYWORD_NOEXCEPT;
        // Return the address providing modifiable access to the
        // 'BloombergLP::bslma::SharedPtrRep' object held by this weak pointer,
        // or 0 if this weak pointer is in the empty state.

    long use_count() const BSLS_KEYWORD_NOEXCEPT;
        // Return a "snapshot" of the current number of shared pointers that
        // share ownership of the object referred to by this weak pointer, or 0
        // if this weak pointer is in the empty state.  Note that any result
        // other than 0 may be unreliable in a multi-threaded program, where
        // another pointer sharing ownership in a different thread may be
        // copied or destroyed, or another weak pointer may be locked in the
        // case that 1 is returned (that would otherwise indicate unique
        // ownership).

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // DEPRECATED BDE LEGACY ACCESSORS
    shared_ptr<ELEMENT_TYPE> acquireSharedPtr() const BSLS_KEYWORD_NOEXCEPT;
        // Return a shared pointer to the object referred to by this weak
        // pointer and managing the same object as that managed by this weak
        // pointer (if any) if 'false == expired()', and a shared pointer in
        // the empty state otherwise.  Note that the behavior of this method is
        // the same as that of 'lock'.

    int numReferences() const BSLS_KEYWORD_NOEXCEPT;
        // [!DEPRECATED!] Use 'use_count' instead.
        //
        // Return a "snapshot" of the current number of shared pointers that
        // share ownership of the object referred to by this weak pointer, or 0
        // if this weak pointer is in the empty state.  Note that the behavior
        // of this method is the same as that of 'use_count', and the result
        // may be unreliable in multi-threaded code for the same reasons.
#endif // BDE_OMIT_INTERNAL_DEPRECATED
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
// CLASS TEMPLATE DEDUCTION GUIDES

template<class ELEMENT_TYPE>
weak_ptr(shared_ptr<ELEMENT_TYPE>) -> weak_ptr<ELEMENT_TYPE>;
    // Deduce the specified type 'ELEMENT_TYPE' corresponding template
    // parameter of the 'bsl::shared_ptr' supplied to the constructor of
    // 'weak_ptr'.
#endif

                    //==============================
                    // class enable_shared_from_this
                    //==============================

template<class ELEMENT_TYPE>
class enable_shared_from_this {
    // This class allows an object that is currently managed by a 'shared_ptr'
    // to safely generate a copy of the managing 'shared_ptr' object.
    // Inheriting from 'enable_shared_from_this<ELEMENT_TYPE>' provides the
    // (template parameter) 'ELEMENT_TYPE' type with a member function
    // 'shared_from_this'.  If an object of type 'ELEMENT_TYPE' is managed by a
    // 'shared_ptr' then calling 'shared_from_this' will return a
    // 'shared_ptr<ELEMENT_TYPE>' that shares ownership of that object.  It is
    // undefined behavior to call 'shared_from_this' on an object unless that
    // object is managed by a 'shared_ptr'.
    //
    // The intended use of 'enable_shared_from_this' is that the (template
    // parameter) type 'ELEMENT_TYPE' inherits directly from the
    // 'enable_shared_from_this' class template.  In the case of multiple
    // inheritance, only one of the base classes should inherit from the
    // 'enable_shared_from_this' class template.  If multiple base classes
    // inherit from 'enable_shared_from_this', then there will be ambiguous
    // calls to the 'shared_from_this' function.

    // FRIENDS
    friend struct BloombergLP::bslstl::SharedPtr_ImpUtil;
        // Allows 'shared_ptr' to initialize 'd_weakThis' when it detects an
        // 'enable_shared_from_this' base class.

  private:
    // DATA
    mutable bsl::weak_ptr<ELEMENT_TYPE> d_weakThis;

  protected:
    // PROTECTED CREATORS
    enable_shared_from_this() BSLS_KEYWORD_NOEXCEPT;
        // Create an 'enable_shared_from_this' object that is not owned by any
        // 'shared_ptr' object.

    enable_shared_from_this(const enable_shared_from_this& unused)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create an 'enable_shared_from_this' object that is not owned by any
        // 'shared_ptr' object.  Note that the specified 'unused' argument is
        // not used by this constructor.

    ~enable_shared_from_this();
        // Destroy this 'enable_shared_form_this'.

    // PROTECTED MANIPULATORS
    enable_shared_from_this& operator=(const enable_shared_from_this& rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Return '*this'. This object is unchanged.  Note that the specified
        // 'rhs' is not used.

  public:
    // MANIPULATORS
    bsl::shared_ptr<ELEMENT_TYPE> shared_from_this();
        // Return a 'shared_ptr<ELEMENT_TYPE>' that shares ownership with an
        // existing 'shared_ptr' object that managed this object, and throw a
        // 'std::bad_weak_ptr' exception if there is no 'shared_ptr' currently
        // managing this object.  If multiple groups of 'shared_ptr's are
        // managing this object, the returned 'shared_ptr' will share ownership
        // with the group that first managed this object.

    bsl::weak_ptr<ELEMENT_TYPE> weak_from_this() BSLS_KEYWORD_NOEXCEPT;
        // Return a 'weak_ptr' holding a weak reference to this managed object
        // if this object is currently managed by 'shared_ptr', and return an
        // expired 'weak_ptr' otherwise.  If multiple groups of 'shared_ptr's
        // are managing this object, the returned 'weak_ptr' will hold a weak
        // reference to the group that first managed this object.

    // ACCESSORS
    bsl::shared_ptr<const ELEMENT_TYPE> shared_from_this() const;
        // Return a 'shared_ptr<const ELEMENT_TYPE>' that shares ownership with
        // an existing 'shared_ptr' object that managed this object, and throw
        // a 'std::bad_weak_ptr' exception if there is no 'shared_ptr'
        // currently managing this object.  If multiple groups of 'shared_ptr's
        // are managing this object, the returned 'shared_ptr' will share
        // ownership with the group that first managed this object.


    bsl::weak_ptr<const ELEMENT_TYPE> weak_from_this() const
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Return a 'weak_ptr' holding a weak reference (with only 'const'
        // access) to this managed object if this object is currently managed
        // by 'shared_ptr', and return an expired 'weak_ptr' otherwise.  If
        // multiple groups of 'shared_ptr's are managing this object, the
        // returned 'weak_ptr' will hold a weak reference to the group that
        // first managed this object.
};

// ASPECTS
template <class ELEMENT_TYPE>
void swap(weak_ptr<ELEMENT_TYPE>& a, weak_ptr<ELEMENT_TYPE>& b)
                                                         BSLS_KEYWORD_NOEXCEPT;
    // Efficiently exchange the states of the specified 'a' and 'b' weak
    // pointers such that each will refer to the object (if any) and
    // representation formerly referred to by the other.

                        // =========================
                        // class hash specialization
                        // =========================

// A partial specialization of 'bsl::hash' is no longer necessary, as the
// primary template has the correct behavior once 'hashAppend' is defined.

}  // close namespace bsl

namespace BloombergLP {
namespace bslstl {

                            // ====================
                            // struct SharedPtrUtil
                            // ====================

struct SharedPtrUtil {
    // This 'struct' provides a namespace for operations on shared pointers.

    // CLASS METHODS
    static
    bsl::shared_ptr<char>
    createInplaceUninitializedBuffer(size_t            bufferSize,
                                     bslma::Allocator *basicAllocator = 0);
        // Return a shared pointer with an in-place representation holding a
        // newly-created uninitialized buffer of the specified 'bufferSize' (in
        // bytes).  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // '0 < bufferSize'.

    // CASTING FUNCTIONS
    template <class TARGET, class SOURCE>
    static
    void constCast(bsl::shared_ptr<TARGET>        *target,
                   const bsl::shared_ptr<SOURCE>&  source);
        // Load into the specified 'target' an aliased shared pointer sharing
        // ownership of the object managed by the specified 'source' shared
        // pointer and referring to 'const_cast<TARGET *>(source.get())'.  If
        // '*target' is already managing a (possibly shared) object, then
        // release the shared reference to that object, and destroy it using
        // its associated deleter if that shared pointer held the last shared
        // reference to that object.  Note that a compiler diagnostic will be
        // emitted indicating an error unless
        // 'const_cast<TARGET *>(source.get())' is a valid expression.

    template <class TARGET, class SOURCE>
    static
    bsl::shared_ptr<TARGET> constCast(const bsl::shared_ptr<SOURCE>& source)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Return a 'bsl::shared_ptr<TARGET>' object sharing ownership of the
        // same object as the specified 'source' shared pointer to the
        // (template parameter) 'SOURCE' type, and referring to
        // 'const_cast<TARGET *>(source.get())'.  Note that a compiler
        // diagnostic will be emitted indicating an error unless
        // 'const_cast<TARGET *>(source.get())' is a valid expression.

    template <class TARGET, class SOURCE>
    static
    void dynamicCast(bsl::shared_ptr<TARGET>        *target,
                     const bsl::shared_ptr<SOURCE>&  source);
        // Load into the specified 'target' an aliased shared pointer sharing
        // ownership of the object managed by the specified 'source' shared
        // pointer and referring to 'dynamic_cast<TARGET *>(source.get())'.  If
        // '*target' is already managing a (possibly shared) object, then
        // release the shared reference to that object, and destroy it using
        // its associated deleter if that shared pointer held the last shared
        // reference to that object.  If
        // '0 == dynamic_cast<TARGET*>(source.get())', then '*target' shall be
        // reset to an empty state that does not refer to an object.  Note that
        // a compiler diagnostic will be emitted indicating an error unless
        // 'dynamic_cast<TARGET *>(source.get())' is a valid expression.

    template <class TARGET, class SOURCE>
    static
    bsl::shared_ptr<TARGET> dynamicCast(const bsl::shared_ptr<SOURCE>& source)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Return a 'bsl::shared_ptr<TARGET>' object sharing ownership of the
        // same object as the specified 'source' shared pointer to the
        // (template parameter) 'SOURCE' type, and referring to
        // 'dynamic_cast<TARGET *>(source.get())'.  If that would return a
        // shared pointer referring to nothing ('0 == get()'), then instead
        // return an (empty) default constructed shared pointer.  Note that a
        // compiler diagnostic will be emitted indicating an error unless
        // 'dynamic_cast<TARGET *>(source.get())' is a valid expression..

    template <class TARGET, class SOURCE>
    static
    void staticCast(bsl::shared_ptr<TARGET>        *target,
                    const bsl::shared_ptr<SOURCE>&  source);
        // Load into the specified 'target' an aliased shared pointer sharing
        // ownership of the object managed by the specified 'source' shared
        // pointer and referring to 'static_cast<TARGET *>(source.get())'.  If
        // '*target' is already managing a (possibly shared) object, then
        // release the shared reference to that object, and destroy it using
        // its associated deleter if that shared pointer held the last shared
        // reference to that object.  Note that a compiler diagnostic will be
        // emitted indicating an error unless
        // 'static_cast<TARGET *>(source.get())' is a valid expression.

    template <class TARGET, class SOURCE>
    static
    bsl::shared_ptr<TARGET> staticCast(const bsl::shared_ptr<SOURCE>& source)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Return a 'bsl::shared_ptr<TARGET>' object sharing ownership of the
        // same object as the specified 'source' shared pointer to the
        // (template parameter) 'SOURCE' type, and referring to
        // 'static_cast<TARGET *>(source.get())'.  Note that a compiler
        // diagnostic will be emitted indicating an error unless
        // 'static_cast<TARGET *>(source.get())' is a valid expression.
};

                        // ==========================
                        // struct SharedPtrNilDeleter
                        // ==========================

struct SharedPtrNilDeleter {
    // This 'struct' provides a function-like shared pointer deleter that does
    // nothing when invoked.

    // ACCESSORS
    void operator()(const volatile void *) const BSLS_KEYWORD_NOEXCEPT;
        // No-Op.
};

                        // ===============================
                        // struct SharedPtr_DefaultDeleter
                        // ===============================

template <bool>
struct SharedPtr_DefaultDeleter {
    // This 'struct' provides a function-like shared pointer deleter that
    // invokes 'delete' with the passed pointer.  If the template parameter is
    // 'true', then the pointer is deleted using 'operator delete []'.
    // Otherwise, it is deleted using 'operator delete'.

    // ACCESSORS
    template <class ANY_TYPE>
    void operator()(ANY_TYPE *ptr) const BSLS_KEYWORD_NOEXCEPT;
        // Call 'delete' with the specified 'ptr'.
};

                        //=========================
                        // struct SharedPtr_ImpUtil
                        //=========================

struct SharedPtr_ImpUtil {
    // This 'struct' should be used by only 'shared_ptr' constructors. Its
    // purpose is to enable 'shared_ptr' constructors to determine if the
    // (template parameter) types 'COMPATIBLE_TYPE' or 'ELEMENT_TYPE' have a
    // specialization of 'enable_shared_from_this' as an unambiguous, publicly
    // accessible, base class.

    // CLASS METHODS
    template<class SHARED_TYPE, class ENABLE_TYPE>
    static void loadEnableSharedFromThis(
                   const bsl::enable_shared_from_this<ENABLE_TYPE> *result,
                   bsl::shared_ptr<SHARED_TYPE>                    *sharedPtr);
        // Load the specified 'result' with the control block (i.e.,
        // 'SharedPtrRep') from the specified 'sharedPtr' if (and only if)
        // 'result' is not 0 and 'result' does not already refer to a
        // non-expired shared-pointer control block.  If 'result' is 0, or if
        // 'result->d_weakThis' has not expired, this operation has no effect.
        // This operation is used to initialize data members from a type that
        // inherits from 'enable_shared_from_this' when constructing an
        // out-of-place shared pointer representation.  This function shall be
        // called only by 'shared_ptr' constructors creating shared pointers
        // for classes that derive publicly and unambiguously from a
        // specialization of 'enabled_shared_from_this'.  Note that overload
        // resolution will select the overload below if a supplied type does
        // not derive from a specialization of 'enable_shared_from_this'.

    static void loadEnableSharedFromThis(const volatile void *, const void *)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Do nothing.  This overload is selected, rather than the immediately
        // preceding template, when the 'SHARED_TYPE' template type parameter
        // of 'shared_ptr<SHARED_TYPE>' does not derive from a specialization
        // of 'enable_shared_from_this'.

    static void throwBadWeakPtr();
        // Throw a 'bsl::bad_weak_ptr' exception.

    template <class TYPE>
    static void *voidify(TYPE *address) BSLS_KEYWORD_NOEXCEPT;
        // Return the specified 'address' cast as a pointer to 'void', even if
        // (the template parameter) 'TYPE' is cv-qualified.

    template <class TYPE>
    static TYPE *unqualify(const volatile TYPE *address) BSLS_KEYWORD_NOEXCEPT;
        // Return the specified 'address' of a potentially 'cv'-qualified
        // object of the given (template parameter) 'TYPE', cast as a pointer
        // to a modifiable non-volatile object of the given 'TYPE'.
};

                        // ==========================
                        // class SharedPtr_RepProctor
                        // ==========================

class SharedPtr_RepProctor {
    // This 'class' implements a proctor that, unless its 'release' method has
    // previously been invoked, automatically releases a reference held by the
    // 'bslma::SharedPtrRep' object that is supplied at construction.

  private:
    // DATA
    bslma::SharedPtrRep *d_rep_p;    // Address of representation being managed

  private:
    // NOT IMPLEMENTED
    SharedPtr_RepProctor(const SharedPtr_RepProctor&);
    SharedPtr_RepProctor& operator=(const SharedPtr_RepProctor&);

  public:
    // CREATORS
    explicit SharedPtr_RepProctor(bslma::SharedPtrRep *rep)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a 'SharedPtr_RepProctor' that conditionally manages the
        // specified 'rep' (if non-zero).

    ~SharedPtr_RepProctor();
        // Destroy this 'SharedPtr_RepProctor', and dispose of (deallocate) the
        // 'bslma::SharedPtrRep' it manages (if any).  If no such object is
        // currently being managed, this method has no effect.  Note that the
        // destructor of the 'bslma::SharedPtrRep' will not be called as the
        // reference count will not be decremented.

    // MANIPULATORS
    void release() BSLS_KEYWORD_NOEXCEPT;
        // Release from management the object currently managed by this
        // proctor.  If no object is currently being managed, this method has
        // no effect.
};

}  // close package namespace
}  // close enterprise namespace

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

#if defined(BSLSTL_SHAREDPTR_SUPPORTS_SFINAE_CHECKS)
namespace BloombergLP {
namespace bslstl {

template <class FUNCTOR>
struct SharedPtr_TestIsCallable {
  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslmf::Util Util;

    struct TrueType {
        char d_padding;
    };
    struct FalseType {
        char d_padding[17];
    };

    // The two structs 'TrueType' and 'FalseType' are guaranteed to have
    // distinct sizes, so that a 'sizeof(expression)' query, where 'expression'
    // returns one of these two types, will give different answers depending on
    // which type is returned.

  public:
    // CLASS METHODS
    template <class ARG>
    static FalseType test(...);
    template <class ARG>
    static TrueType
    test(typename bsl::enable_if<static_cast<bool>(
                    sizeof(BSLSTL_SHAREDPTR_SFINAE_DISCARD(
                               Util::declval<FUNCTOR>()(Util::declval<ARG>())),
                           0))>::type *);
        // This function is never defined.  It provides a property-checker that
        // an entity of (template parameter) type 'FACTORY' can be called like
        // a function with a single argument, which is a pointer to an object
        // of (template parameter) type 'ARG'.  The 'sizeof' expression
        // provides an unevaluated context to check the validity of the
        // enclosed expression, and the ', 0' ensures that the 'sizeof' check
        // remains valid, even if the expression returns 'void'.  Similarly,
        // the cast to 'void' ensures that there are no surprises with types
        // that overload the comma operator.  Note that the cast to 'void' is
        // elided for Clang compilers using versions of LLVM prior to 12, which
        // fail to evaluate the trait properly.
};

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1920
// Microsoft needs a workaround to correctly handle calling through function
// pointers with incompatible types in Visual Studio 2017.  In Visual Studio
// 2019 the workaround isn't needed and crashes the compiler if enabled!
// (Visual Studio versions prior to 2017 appear to not need the workaround,
// based on further testing, but it's being left in place so as not to alter
// this code for people using older compiler versions.)

template <class RESULT, class PARAM>
struct SharedPtr_TestIsCallable<RESULT(PARAM)> {
  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslmf::Util Util;

    struct TrueType  { char d_padding; };
    struct FalseType { char d_padding[17]; };

    // PRIVATE CLASS METHODS
    static RESULT callMe(PARAM);

  public:
    // CLASS METHODS
    template <class ARG>
    static FalseType test(...);
    template <class ARG>
    static TrueType test(typename bsl::enable_if<(bool)sizeof(
                                        ((void)callMe(Util::declval<ARG>())), 0
                                                       )>::type *);
        // This function is never defined.  It provides a property-checker that
        // an entity of (template parameter) type 'FACTORY' can be called like
        // a function with a single argument, which is a pointer to an object
        // of (template parameter) type 'ARG'.  The 'sizeof' expression
        // provides an unevaluated context to check the validity of the
        // enclosed expression, and the ', 0' ensures that the 'sizeof' check
        // remains valid, even if the expression returns 'void'.  Similarly,
        // the cast to 'void' ensures that there are no surprises with types
        // that overload the comma operator.
};

template <class RESULT, class PARAM>
struct SharedPtr_TestIsCallable<RESULT(*)(PARAM)>
    :  SharedPtr_TestIsCallable<RESULT(PARAM)> {
};

template <class RESULT, class PARAM>
struct SharedPtr_TestIsCallable<RESULT(&)(PARAM)>
    :  SharedPtr_TestIsCallable<RESULT(PARAM)> {
};

#if BSLS_PLATFORM_CMP_VERSION >= 1910
// MSVC 2017 expression-SFINAE has a regression that is failing in two
// additional cases:
//  1) for pointers to object types
//  2) where '0' is used for a null pointer literal, deducing as 'int'.
// We resolve those issues with a couple more specializations below.

template <class TYPE>
struct SharedPtr_TestIsCallable<TYPE *> {
    struct TrueType  { char d_padding; };
    struct FalseType { char d_padding[17]; };

    template <class ARG>
    static FalseType test(...);
};

template <>
struct SharedPtr_TestIsCallable<int> {
    struct TrueType  { char d_padding; };
    struct FalseType { char d_padding[17]; };

    template <class ARG>
    static FalseType test(...);
};
#endif  // MSVC 2017

#endif  // BSLS_PLATFORM_CMP_MSVC

template <class FUNCTOR, class ARG>
struct SharedPtr_IsCallable {
    enum { k_VALUE =
          sizeof(SharedPtr_TestIsCallable<FUNCTOR>::template test<ARG>(0)) == 1
    };
};


struct SharedPtr_IsFactoryFor_Impl {
  private:
    // PRIVATE TYPES
    struct TrueType {
        char d_padding;
    };
    struct FalseType {
        char d_padding[17];
    };

  public:
    // CLASS METHODS
    template <class FACTORY, class ARG>
    static FalseType test(...);
    template <class FACTORY, class ARG>
    static TrueType test(typename bsl::enable_if<static_cast<bool>(sizeof(
                                  BSLSTL_SHAREDPTR_SFINAE_DISCARD(
                                      (*(FACTORY *)0)->deleteObject((ARG *)0)),
                                  0))>::type *);
        // This function is never defined.  It provides a property-checker that
        // an object of (template parameter) type 'FACTORY' has a
        // member-function called 'deleteObject' that can be called with a
        // single argument, which is a pointer to an object of (template
        // parameter) type 'ARG'.  The 'sizeof' expression provides an
        // unevaluated context to check the validity of the enclosed
        // expression, and the ', 0' ensures that the 'sizeof' check remains
        // valid, even if the expression returns 'void'.  Similarly, the cast
        // to 'void' ensures that there are no surprises with types that
        // overload the comma operator.  Note that the cast to 'void' is elided
        // for Clang compilers using versions of LLVM prior to 12, which fail
        // to evaluate the trait properly.
};

template <class FACTORY, class ARG>
struct SharedPtr_IsFactoryFor {
    enum { k_VALUE =
                sizeof(SharedPtr_IsFactoryFor_Impl::test<FACTORY, ARG>(0)) == 1
    };
};


struct SharedPtr_IsNullableFactory_Impl {
  private:
    // PRIVATE TYPES
    struct TrueType {
        char d_padding;
    };
    struct FalseType {
        char d_padding[17];
    };

  public:
    // CLASS METHODS
    template <class FACTORY>
    static FalseType test(...);
    template <class FACTORY>
    static TrueType test(typename bsl::enable_if<static_cast<bool>(sizeof(
                                   BSLSTL_SHAREDPTR_SFINAE_DISCARD(
                                       (*(FACTORY *)0)->deleteObject(nullptr)),
                                   0))>::type *);
        // This function is never defined.  It provides a property-checker that
        // an object of (template parameter) type 'FACTORY' has a
        // member-function called 'deleteObject' that can be called with a
        // single argument, which is a pointer to an object of (template
        // parameter) type 'ARG'.  The 'sizeof' expression provides an
        // unevaluated context to check the validity of the enclosed
        // expression, and the ', 0' ensures that the 'sizeof' check remains
        // valid, even if the expression returns 'void'.  Similarly, the cast
        // to 'void' ensures that there are no surprises with types that
        // overload the comma operator.  Note that the cast to 'void' is elided
        // for Clang compilers using versions of LLVM prior to 12, which fail
        // to evaluate the trait properly.
};

template <class FACTORY>
struct SharedPtr_IsNullableFactory {
    enum { k_VALUE =
                sizeof(SharedPtr_IsNullableFactory_Impl::test<FACTORY>(0)) == 1
    };
};


template <class SOURCE_TYPE, class DEST_TYPE>
struct SharedPtr_IsPointerConvertible_Impl
: bsl::is_convertible<SOURCE_TYPE *, DEST_TYPE *>::type {};

template <class SOURCE_TYPE, class DEST_TYPE>
struct SharedPtr_IsPointerConvertible_Impl<SOURCE_TYPE, DEST_TYPE[]>
: bsl::is_convertible<SOURCE_TYPE (*)[], DEST_TYPE (*)[]>::type {};

template <class SOURCE_TYPE, class DEST_TYPE, size_t DEST_SIZE>
struct SharedPtr_IsPointerConvertible_Impl<SOURCE_TYPE, DEST_TYPE[DEST_SIZE]>
: bsl::is_convertible<SOURCE_TYPE (*)[DEST_SIZE],
                      DEST_TYPE (*)[DEST_SIZE]>::type {};


template <class SOURCE_TYPE, class DEST_TYPE>
struct SharedPtr_IsPointerConvertible
: SharedPtr_IsPointerConvertible_Impl<SOURCE_TYPE, DEST_TYPE>::type {};


template <class SOURCE_TYPE, class DEST_TYPE>
struct SharedPtr_IsPointerCompatible_Impl
    : bsl::is_convertible<SOURCE_TYPE *, DEST_TYPE *>::type {};

template <class TYPE, size_t SIZE>
struct SharedPtr_IsPointerCompatible_Impl<TYPE[SIZE], TYPE[]>
     : bsl::true_type {};

template <class TYPE, size_t SIZE>
struct SharedPtr_IsPointerCompatible_Impl<TYPE[SIZE], const TYPE[]>
     : bsl::true_type {};

template <class TYPE, size_t SIZE>
struct SharedPtr_IsPointerCompatible_Impl<TYPE[SIZE], volatile TYPE[]>
     : bsl::true_type {};

template <class TYPE, size_t SIZE>
struct SharedPtr_IsPointerCompatible_Impl<TYPE[SIZE], const volatile TYPE[]>
     : bsl::true_type {};


template <class SOURCE_TYPE, class DEST_TYPE>
struct SharedPtr_IsPointerCompatible
: SharedPtr_IsPointerCompatible_Impl<SOURCE_TYPE, DEST_TYPE>::type {};

}  // close package namespace
}  // close enterprise namespace
#endif  // BSLSTL_SHAREDPTR_SUPPORTS_SFINAE_CHECKS


namespace bsl {
                    //------------------------------
                    // class enable_shared_from_this
                    //------------------------------
// CREATORS
template<class ELEMENT_TYPE>
inline // constexpr
enable_shared_from_this<ELEMENT_TYPE>::enable_shared_from_this()
                                                          BSLS_KEYWORD_NOEXCEPT
: d_weakThis()
{
}

template<class ELEMENT_TYPE>
inline
enable_shared_from_this<ELEMENT_TYPE>::enable_shared_from_this(
                                                const enable_shared_from_this&)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_weakThis()
{
}

template<class ELEMENT_TYPE>
inline
enable_shared_from_this<ELEMENT_TYPE>::~enable_shared_from_this()
{
}

// MANIPULATORS
template<class ELEMENT_TYPE>
inline
enable_shared_from_this<ELEMENT_TYPE>&
enable_shared_from_this<ELEMENT_TYPE>::operator=(
                                                const enable_shared_from_this&)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return *this;
}

template<class ELEMENT_TYPE>
inline
shared_ptr<ELEMENT_TYPE>
enable_shared_from_this<ELEMENT_TYPE>::shared_from_this()
{
    return shared_ptr<ELEMENT_TYPE>(d_weakThis);
}

template<class ELEMENT_TYPE>
inline
shared_ptr<const ELEMENT_TYPE>
enable_shared_from_this<ELEMENT_TYPE>::shared_from_this() const
{
    return shared_ptr<const ELEMENT_TYPE>(d_weakThis);
}

template<class ELEMENT_TYPE>
inline
weak_ptr<ELEMENT_TYPE>
enable_shared_from_this<ELEMENT_TYPE>::weak_from_this() BSLS_KEYWORD_NOEXCEPT
{
    return d_weakThis;
}

template<class ELEMENT_TYPE>
inline
weak_ptr<const ELEMENT_TYPE>
enable_shared_from_this<ELEMENT_TYPE>::weak_from_this() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_weakThis;
}

                            // ----------------
                            // class shared_ptr
                            // ----------------

// PRIVATE CLASS METHODS
template <class ELEMENT_TYPE>
template <class INPLACE_REP>
inline
BloombergLP::bslma::SharedPtrRep *
shared_ptr<ELEMENT_TYPE>::makeInternalRep(
                                         ELEMENT_TYPE                     *,
                                         INPLACE_REP                      *,
                                         BloombergLP::bslma::SharedPtrRep *rep)
{
    return rep;
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE, class ALLOCATOR>
inline
BloombergLP::bslma::SharedPtrRep *
shared_ptr<ELEMENT_TYPE>::makeInternalRep(
                                      COMPATIBLE_TYPE               *ptr,
                                      ALLOCATOR                     *,
                                      BloombergLP::bslma::Allocator *allocator)
{
    typedef BloombergLP::bslma::SharedPtrOutofplaceRep<
                                               COMPATIBLE_TYPE,
                                               BloombergLP::bslma::Allocator *>
                                                                      RepMaker;

    return RepMaker::makeOutofplaceRep(ptr, allocator, allocator);
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE, class DELETER>
inline
BloombergLP::bslma::SharedPtrRep *
shared_ptr<ELEMENT_TYPE>::makeInternalRep(COMPATIBLE_TYPE *ptr,
                                          DELETER         *deleter,
                                          ...)
{
    typedef BloombergLP::bslma::SharedPtrOutofplaceRep<COMPATIBLE_TYPE,
                                                       DELETER *>     RepMaker;

    return RepMaker::makeOutofplaceRep(ptr, deleter, 0);
}

// CREATORS
template <class ELEMENT_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR
shared_ptr<ELEMENT_TYPE>::shared_ptr() BSLS_KEYWORD_NOEXCEPT
: d_ptr_p(0)
, d_rep_p(0)
{
}

template <class ELEMENT_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR
shared_ptr<ELEMENT_TYPE>::shared_ptr(bsl::nullptr_t) BSLS_KEYWORD_NOEXCEPT
: d_ptr_p(0)
, d_rep_p(0)
{
}

template <class ELEMENT_TYPE>
template <class CONVERTIBLE_TYPE
          BSLSTL_SHAREDPTR_DEFINE_IF_CONVERTIBLE>
inline
shared_ptr<ELEMENT_TYPE>::shared_ptr(CONVERTIBLE_TYPE *ptr)
: d_ptr_p(ptr)
{
    typedef BloombergLP::bslstl::SharedPtr_DefaultDeleter<
                                   bsl::is_array<ELEMENT_TYPE>::value> Deleter;
    typedef BloombergLP::bslma::SharedPtrOutofplaceRep<CONVERTIBLE_TYPE,
                                                       Deleter>       RepMaker;

    d_rep_p = RepMaker::makeOutofplaceRep(ptr, Deleter(), 0);
    if (!bsl::is_array<ELEMENT_TYPE>::value) {
        BloombergLP::bslstl::SharedPtr_ImpUtil::loadEnableSharedFromThis(ptr,
                                                                         this);
    }
}

template <class ELEMENT_TYPE>
template <class CONVERTIBLE_TYPE
          BSLSTL_SHAREDPTR_DEFINE_IF_CONVERTIBLE>
inline
shared_ptr<ELEMENT_TYPE>::shared_ptr(
                                 CONVERTIBLE_TYPE              *ptr,
                                 BloombergLP::bslma::Allocator *basicAllocator)
: d_ptr_p(ptr)
{
    typedef BloombergLP::bslma::SharedPtrOutofplaceRep<
                                               CONVERTIBLE_TYPE,
                                               BloombergLP::bslma::Allocator *>
                                                                      RepMaker;

    d_rep_p = RepMaker::makeOutofplaceRep(ptr, basicAllocator, basicAllocator);
    if (!bsl::is_array<ELEMENT_TYPE>::value) {
        BloombergLP::bslstl::SharedPtr_ImpUtil::loadEnableSharedFromThis(ptr,
                                                                         this);
    }
}

template <class ELEMENT_TYPE>
inline
shared_ptr<ELEMENT_TYPE>::shared_ptr(ELEMENT_TYPE                     *ptr,
                                     BloombergLP::bslma::SharedPtrRep *rep)
: d_ptr_p(ptr)
, d_rep_p(rep)
{
    BloombergLP::bslstl::SharedPtr_ImpUtil::loadEnableSharedFromThis(ptr,
                                                                     this);
}

template <class ELEMENT_TYPE>
inline
shared_ptr<ELEMENT_TYPE>::shared_ptr(
                       ELEMENT_TYPE                     *ptr,
                       BloombergLP::bslma::SharedPtrRep *rep,
                       BloombergLP::bslstl::SharedPtr_RepFromExistingSharedPtr)
: d_ptr_p(ptr)
, d_rep_p(rep)
{
}

template <class ELEMENT_TYPE>
template <class CONVERTIBLE_TYPE,
          class DISPATCH
          BSLSTL_SHAREDPTR_DEFINE_IF_CONVERTIBLE
          BSLSTL_SHAREDPTR_DEFINE_IF_DELETER(DISPATCH *, CONVERTIBLE_TYPE)>
inline
shared_ptr<ELEMENT_TYPE>::shared_ptr(CONVERTIBLE_TYPE *ptr,
                                     DISPATCH         *dispatch)
: d_ptr_p(ptr)
, d_rep_p(makeInternalRep(ptr, dispatch, dispatch))
{
    if (!bsl::is_array<ELEMENT_TYPE>::value) {
        BloombergLP::bslstl::SharedPtr_ImpUtil::loadEnableSharedFromThis(ptr,
                                                                         this);
    }
}

template <class ELEMENT_TYPE>
template <class CONVERTIBLE_TYPE,
          class DELETER
          BSLSTL_SHAREDPTR_DEFINE_IF_CONVERTIBLE
          BSLSTL_SHAREDPTR_DEFINE_IF_DELETER(DELETER, CONVERTIBLE_TYPE)>
inline
shared_ptr<ELEMENT_TYPE>::shared_ptr(
                                 CONVERTIBLE_TYPE              *ptr,
                                 DELETER                        deleter,
                                 BloombergLP::bslma::Allocator *basicAllocator)
: d_ptr_p(ptr)
{
    typedef BloombergLP::bslma::SharedPtrOutofplaceRep<CONVERTIBLE_TYPE,
                                                       DELETER> RepMaker;

    d_rep_p = RepMaker::makeOutofplaceRep(ptr, deleter, basicAllocator);
    if (!bsl::is_array<ELEMENT_TYPE>::value) {
        BloombergLP::bslstl::SharedPtr_ImpUtil::loadEnableSharedFromThis(ptr,
                                                                         this);
    }
}

template <class ELEMENT_TYPE>
template <class CONVERTIBLE_TYPE,
          class DELETER,
          class ALLOCATOR
          BSLSTL_SHAREDPTR_DEFINE_IF_CONVERTIBLE
          BSLSTL_SHAREDPTR_DEFINE_IF_DELETER(DELETER, CONVERTIBLE_TYPE)>
inline
shared_ptr<ELEMENT_TYPE>::shared_ptr(CONVERTIBLE_TYPE *ptr,
                                     DELETER           deleter,
                                     ALLOCATOR         basicAllocator,
                                     typename ALLOCATOR::value_type *)
: d_ptr_p(ptr)
{
#ifdef BSLS_PLATFORM_CMP_MSVC
    // This is not quite C++11 'decay' as we do not need to worry about array
    // types, and do not want to remove reference or cv-qualification from
    // DELETER otherwise.  This works around a Microsoft bug turning function
    // pointers into function references.

    typedef typename bsl::conditional<bsl::is_function<DELETER>::value,
                                      typename bsl::add_pointer<DELETER>::type,
                                      DELETER>::type DeleterType;
#else
    typedef DELETER DeleterType;
#endif

    typedef
    BloombergLP::bslstl::SharedPtrAllocateOutofplaceRep<CONVERTIBLE_TYPE,
                                                        DeleterType,
                                                        ALLOCATOR> RepMaker;

    d_rep_p = RepMaker::makeOutofplaceRep(ptr, deleter, basicAllocator);
    if (!bsl::is_array<ELEMENT_TYPE>::value) {
        BloombergLP::bslstl::SharedPtr_ImpUtil::loadEnableSharedFromThis(ptr,
                                                                         this);
    }
}

template <class ELEMENT_TYPE>
inline
shared_ptr<ELEMENT_TYPE>::shared_ptr(nullptr_t,
                                     BloombergLP::bslma::Allocator *)
: d_ptr_p(0)
, d_rep_p(0)
{
}

template <class ELEMENT_TYPE>
template <class DELETER
          BSLSTL_SHAREDPTR_DEFINE_IF_NULLPTR_DELETER(DELETER)>
inline
shared_ptr<ELEMENT_TYPE>::shared_ptr(
                                 nullptr_t,
                                 DELETER                        deleter,
                                 BloombergLP::bslma::Allocator *basicAllocator)
: d_ptr_p(0)
{
    typedef BloombergLP::bslma::SharedPtrOutofplaceRep<ELEMENT_TYPE,
                                                       DELETER> RepMaker;

    if (bsl::is_convertible<DELETER, BloombergLP::bslma::Allocator *>::value &&
        bsl::is_pointer<DELETER>::value) {
        d_rep_p = 0;
    }
    else {
        d_rep_p = RepMaker::makeOutofplaceRep((ELEMENT_TYPE *)0,
                                              deleter,
                                              basicAllocator);
    }
}

template <class ELEMENT_TYPE>
template <class DELETER, class ALLOCATOR
          BSLSTL_SHAREDPTR_DEFINE_IF_NULLPTR_DELETER(DELETER)>
inline
shared_ptr<ELEMENT_TYPE>::shared_ptr(
                                nullptr_t,
                                DELETER                         deleter,
                                ALLOCATOR                       basicAllocator,
                                typename ALLOCATOR::value_type *)
: d_ptr_p(0)
{
#ifdef BSLS_PLATFORM_CMP_MSVC
    // This is not quite C++11 'decay' as we do not need to worry about array
    // types, and do not want to remove reference or cv-qualification from
    // DELETER otherwise.  This works around a Microsoft bug turning function
    // pointers into function references.

    typedef typename bsl::conditional<bsl::is_function<DELETER>::value,
                                      typename bsl::add_pointer<DELETER>::type,
                                      DELETER>::type DeleterType;
#else
    typedef DELETER DeleterType;
#endif

    typedef
    BloombergLP::bslstl::SharedPtrAllocateOutofplaceRep<ELEMENT_TYPE,
                                                        DeleterType,
                                                        ALLOCATOR> RepMaker;

    d_rep_p = RepMaker::makeOutofplaceRep((ELEMENT_TYPE *)0,
                                          deleter,
                                          basicAllocator);
}

template <class ELEMENT_TYPE>
template <class CONVERTIBLE_TYPE
          BSLSTL_SHAREDPTR_DEFINE_IF_CONVERTIBLE>
shared_ptr<ELEMENT_TYPE>::shared_ptr(
              BloombergLP::bslma::ManagedPtr<CONVERTIBLE_TYPE>  managedPtr,
              BloombergLP::bslma::Allocator                    *basicAllocator)
: d_ptr_p(managedPtr.ptr())
, d_rep_p(0)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<
                            BloombergLP::bslma::ManagedPtr<ELEMENT_TYPE> > Rep;

    if (d_ptr_p) {
        ELEMENT_TYPE *pPotentiallyShared = static_cast<ELEMENT_TYPE *>(
                                                managedPtr.deleter().object());

        if (&BloombergLP::bslma::SharedPtrRep::managedPtrDeleter ==
                                              managedPtr.deleter().deleter()) {
            d_rep_p = static_cast<BloombergLP::bslma::SharedPtrRep *>
                                       (managedPtr.release().second.factory());
        }
        else if (&BloombergLP::bslma::SharedPtrRep::managedPtrEmptyDeleter ==
                                              managedPtr.deleter().deleter()) {
            d_rep_p = 0;
            managedPtr.release();
        }
        else {
            basicAllocator =
                        BloombergLP::bslma::Default::allocator(basicAllocator);
            Rep *rep = new (*basicAllocator) Rep(basicAllocator);
            (*rep->ptr()) = managedPtr;
            d_rep_p = rep;
        }

        BloombergLP::bslstl::SharedPtr_ImpUtil::loadEnableSharedFromThis(
                                                            pPotentiallyShared,
                                                            this);
    }
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
template <class ELEMENT_TYPE>
template <class CONVERTIBLE_TYPE
          BSLSTL_SHAREDPTR_DEFINE_IF_CONVERTIBLE>
shared_ptr<ELEMENT_TYPE>::shared_ptr(
                              std::auto_ptr<CONVERTIBLE_TYPE>&  autoPtr,
                              BloombergLP::bslma::Allocator    *basicAllocator)
: d_ptr_p(autoPtr.get())
, d_rep_p(0)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<
                                         std::auto_ptr<CONVERTIBLE_TYPE> > Rep;

    if (d_ptr_p) {
        basicAllocator =
                        BloombergLP::bslma::Default::allocator(basicAllocator);
        Rep *rep = new (*basicAllocator) Rep(basicAllocator);
        (*rep->ptr()) = autoPtr;
        d_rep_p = rep;
        BloombergLP::bslstl::SharedPtr_ImpUtil::loadEnableSharedFromThis(
                                                                       d_ptr_p,
                                                                       this);
    }
}

template <class ELEMENT_TYPE>
shared_ptr<ELEMENT_TYPE>::shared_ptr(
                               std::auto_ptr_ref<ELEMENT_TYPE>  autoRef,
                               BloombergLP::bslma::Allocator   *basicAllocator)
: d_ptr_p(0)
, d_rep_p(0)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<
                                             std::auto_ptr<ELEMENT_TYPE> > Rep;

    std::auto_ptr<ELEMENT_TYPE> autoPtr(autoRef);
    if (autoPtr.get()) {
        basicAllocator =
                        BloombergLP::bslma::Default::allocator(basicAllocator);
        Rep *rep = new (*basicAllocator) Rep(basicAllocator);
        d_ptr_p = autoPtr.get();
        (*rep->ptr()) = autoPtr;
        d_rep_p = rep;
    }
}
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
# if defined(BSLSTL_SHAREDPTR_SUPPORTS_SFINAE_CHECKS)
template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE,
          class UNIQUE_DELETER,
          typename enable_if<is_convertible<
                      typename std::unique_ptr<COMPATIBLE_TYPE,
                                               UNIQUE_DELETER>::pointer,
                      ELEMENT_TYPE *>::value>::type *>
shared_ptr<ELEMENT_TYPE>::shared_ptr(
            std::unique_ptr<COMPATIBLE_TYPE, UNIQUE_DELETER>&&  adoptee,
            BloombergLP::bslma::Allocator                      *basicAllocator)
: d_ptr_p(adoptee.get())
, d_rep_p(0)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<
                        std::unique_ptr<COMPATIBLE_TYPE, UNIQUE_DELETER> > Rep;

    if (d_ptr_p) {
        basicAllocator =
                        BloombergLP::bslma::Default::allocator(basicAllocator);
        Rep *rep = new (*basicAllocator) Rep(basicAllocator,
                            BloombergLP::bslmf::MovableRefUtil::move(adoptee));
        d_rep_p = rep;
        BloombergLP::bslstl::SharedPtr_ImpUtil::loadEnableSharedFromThis(
                                                                       d_ptr_p,
                                                                       this);
    }
}
# endif
#endif

template <class ELEMENT_TYPE>
template <class ANY_TYPE>
shared_ptr<ELEMENT_TYPE>::shared_ptr(const shared_ptr<ANY_TYPE>&  source,
                                     ELEMENT_TYPE                *object)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_ptr_p(object)
, d_rep_p(source.d_rep_p)
{
    if (d_rep_p) {
        d_rep_p->acquireRef();
    }
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE
          BSLSTL_SHAREDPTR_DEFINE_IF_COMPATIBLE>
shared_ptr<ELEMENT_TYPE>::
shared_ptr(const shared_ptr<COMPATIBLE_TYPE>& other) BSLS_KEYWORD_NOEXCEPT
: d_ptr_p(other.d_ptr_p)
, d_rep_p(other.d_rep_p)
{
    if (d_rep_p) {
        d_rep_p->acquireRef();
    }
}

template <class ELEMENT_TYPE>
shared_ptr<ELEMENT_TYPE>::shared_ptr(const shared_ptr& original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_ptr_p(original.d_ptr_p)
, d_rep_p(original.d_rep_p)
{
    if (d_rep_p) {
        d_rep_p->acquireRef();
    }
}

template <class ELEMENT_TYPE>
shared_ptr<ELEMENT_TYPE>::shared_ptr
                          (BloombergLP::bslmf::MovableRef<shared_ptr> original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_ptr_p(BloombergLP::bslmf::MovableRefUtil::access(original).d_ptr_p)
, d_rep_p(BloombergLP::bslmf::MovableRefUtil::access(original).d_rep_p)
{
    BloombergLP::bslmf::MovableRefUtil::access(original).d_ptr_p = 0;
    BloombergLP::bslmf::MovableRefUtil::access(original).d_rep_p = 0;
}

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE
          BSLSTL_SHAREDPTR_DEFINE_IF_COMPATIBLE>
shared_ptr<ELEMENT_TYPE>::shared_ptr(shared_ptr<COMPATIBLE_TYPE>&& other)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_ptr_p(other.d_ptr_p)
, d_rep_p(other.d_rep_p)
{
    other.d_ptr_p = 0;
    other.d_rep_p = 0;
}
#else
template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE
          BSLSTL_SHAREDPTR_DEFINE_IF_COMPATIBLE>
shared_ptr<ELEMENT_TYPE>::
shared_ptr(BloombergLP::bslmf::MovableRef<shared_ptr<COMPATIBLE_TYPE> > other)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_ptr_p(BloombergLP::bslmf::MovableRefUtil::access(other).d_ptr_p)
, d_rep_p(BloombergLP::bslmf::MovableRefUtil::access(other).d_rep_p)
{
    BloombergLP::bslmf::MovableRefUtil::access(other).d_ptr_p = 0;
    BloombergLP::bslmf::MovableRefUtil::access(other).d_rep_p = 0;
}
#endif

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE
          BSLSTL_SHAREDPTR_DEFINE_IF_COMPATIBLE>
shared_ptr<ELEMENT_TYPE>::shared_ptr(const weak_ptr<COMPATIBLE_TYPE>& other)
: d_ptr_p(0)
, d_rep_p(0)
{
    // This implementation handles two awkward cases:
    //
    // i) a ref-counted null pointer, means we cannot simply test 'if (!value)'
    // ii) a null pointer aliasing a non-null pointer is still expired, and so
    //     should throw.

    SelfType value = other.lock();
    if (other.expired()) {
        // Test after lock to avoid a race between testing 'expired' and
        // claiming the lock.

        BloombergLP::bslstl::SharedPtr_ImpUtil::throwBadWeakPtr();
    }

    swap(value);
}

#if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE
          BSLSTL_SHAREDPTR_DEFINE_IF_COMPATIBLE>
shared_ptr<ELEMENT_TYPE>::shared_ptr(
                BloombergLP::bslmf::MovableRef<weak_ptr<COMPATIBLE_TYPE> > ptr)
: d_ptr_p(0)
, d_rep_p(0)
{
    // This implementation handles two awkward cases:
    //
    // i) a ref-counted null pointer, means we cannot simply test 'if (!value)'
    // ii) a null pointer aliasing a non-null pointer is still expired, and so
    //     should throw.

    weak_ptr<COMPATIBLE_TYPE>& other = ptr;

    SelfType value = other.lock();
    if (other.expired()) {
        // Test after lock to avoid a race between testing 'expired' and
        // claiming the lock.

        BloombergLP::bslstl::SharedPtr_ImpUtil::throwBadWeakPtr();
    }

    swap(value);
}
#endif

template <class ELEMENT_TYPE>
shared_ptr<ELEMENT_TYPE>::~shared_ptr()
{
    if (d_rep_p) {
        d_rep_p->releaseRef();
    }
}

// MANIPULATORS
template <class ELEMENT_TYPE>
shared_ptr<ELEMENT_TYPE>&
shared_ptr<ELEMENT_TYPE>::operator=(const shared_ptr& rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    // Instead of testing '&rhs == this', which happens infrequently, optimize
    // for when reps are the same.

    if (rhs.d_rep_p == d_rep_p) {
        d_ptr_p = rhs.d_ptr_p;
    }
    else {
        SelfType(rhs).swap(*this);
    }

    return *this;
}

template <class ELEMENT_TYPE>
shared_ptr<ELEMENT_TYPE>&
shared_ptr<ELEMENT_TYPE>::operator=(
                                BloombergLP::bslmf::MovableRef<shared_ptr> rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    // No self-assignment to optimize, postcondition demands 'rhs' is left
    // empty, unless it is the exact same object, not just the same 'rep'.

    shared_ptr(BloombergLP::bslmf::MovableRefUtil::move(rhs)).swap(*this);

    return *this;
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
typename enable_if<
    is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value,
    shared_ptr<ELEMENT_TYPE>&>::type
shared_ptr<ELEMENT_TYPE>::operator=(const shared_ptr<COMPATIBLE_TYPE>& rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    // Instead of testing '&rhs == this', which happens infrequently, optimize
    // for when reps are the same.

    if (rhs.d_rep_p == d_rep_p) {
        d_ptr_p = rhs.d_ptr_p;
    }
    else {
        SelfType(rhs).swap(*this);
    }

    return *this;
}

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
typename enable_if<is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value,
                   shared_ptr<ELEMENT_TYPE>&>::type
shared_ptr<ELEMENT_TYPE>::operator=(shared_ptr<COMPATIBLE_TYPE>&& rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
#else
template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
typename enable_if<is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value,
                   shared_ptr<ELEMENT_TYPE>&>::type
shared_ptr<ELEMENT_TYPE>::operator=(
              BloombergLP::bslmf::MovableRef<shared_ptr<COMPATIBLE_TYPE> > rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
#endif
{
    // No self-assignment to optimize, postcondition demands 'rhs' is left
    // empty, unless it is the exact same object, not just the same 'rep'.

    shared_ptr(BloombergLP::bslmf::MovableRefUtil::move(rhs)).swap(*this);

    return *this;
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
inline
typename enable_if<
    is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value,
    shared_ptr<ELEMENT_TYPE>&>::type
shared_ptr<ELEMENT_TYPE>::operator=(
                           BloombergLP::bslma::ManagedPtr<COMPATIBLE_TYPE> rhs)
{
    SelfType(rhs).swap(*this);
    return *this;
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
inline
typename enable_if<
    is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value,
    shared_ptr<ELEMENT_TYPE>&>::type
shared_ptr<ELEMENT_TYPE>::operator=(std::auto_ptr<COMPATIBLE_TYPE> rhs)
{
    SelfType(rhs).swap(*this);
    return *this;
}
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE, class UNIQUE_DELETER>
inline
typename enable_if<
    is_convertible<
        typename std::unique_ptr<COMPATIBLE_TYPE, UNIQUE_DELETER>::pointer,
        ELEMENT_TYPE *>::value,
    shared_ptr<ELEMENT_TYPE>&>::type
shared_ptr<ELEMENT_TYPE>::operator=(
                        std::unique_ptr<COMPATIBLE_TYPE, UNIQUE_DELETER>&& rhs)
{
    SelfType(BloombergLP::bslmf::MovableRefUtil::move(rhs)).swap(*this);
    return *this;
}
#endif

template <class ELEMENT_TYPE>
inline
void shared_ptr<ELEMENT_TYPE>::reset() BSLS_KEYWORD_NOEXCEPT
{
    BloombergLP::bslma::SharedPtrRep *rep = d_rep_p;

    // Clear 'd_rep_p' first so that a self-referencing shared pointer's
    // destructor does not try to call 'releaseRef' again.

    d_rep_p = 0;
    d_ptr_p = 0;

    if (rep) {
        rep->releaseRef();
    }
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
inline
typename
      enable_if<is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value>::type
shared_ptr<ELEMENT_TYPE>::reset(COMPATIBLE_TYPE *ptr)
{
    SelfType(ptr).swap(*this);
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE, class DELETER>
inline
typename
      enable_if<is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value>::type
shared_ptr<ELEMENT_TYPE>::reset(COMPATIBLE_TYPE *ptr,
                                DELETER          deleter)
{
    SelfType(ptr, deleter).swap(*this);
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE, class DELETER, class ALLOCATOR>
inline
typename
      enable_if<is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value>::type
shared_ptr<ELEMENT_TYPE>::reset(COMPATIBLE_TYPE *ptr,
                                DELETER          deleter,
                                ALLOCATOR        basicAllocator)
{
    SelfType(ptr, deleter, basicAllocator).swap(*this);
}

template <class ELEMENT_TYPE>
template <class ANY_TYPE>
inline
void shared_ptr<ELEMENT_TYPE>::reset(const shared_ptr<ANY_TYPE>&  source,
                                     ELEMENT_TYPE                *ptr)
{
    // Optimize for the (expected) common case where aliases are managing the
    // same data structure.

    if (source.d_rep_p == d_rep_p && ptr) {
        d_ptr_p = ptr;
    }
    else {
        SelfType(source, ptr).swap(*this);
    }
}

template <class ELEMENT_TYPE>
inline
void shared_ptr<ELEMENT_TYPE>::swap(shared_ptr& other) BSLS_KEYWORD_NOEXCEPT
{
    // We directly implement swapping of two pointers, rather than simply
    // calling 'bsl::swap' or using 'bslalg::SwapUtil', to avoid (indirectly)
    // including the platform <algorithm> header, which may transitively
    // include other standard headers.  This reduces the risk of
    // platform-specific cycles, which have been observed to cause problems.

    // Also, as 'shared_ptr' is bitwise-moveable, we could simplify this to
    // 'memcpy'-ing through an (aligned?) array of sufficient 'char'.

    ELEMENT_TYPE *tempPtr_p = d_ptr_p;
    d_ptr_p       = other.d_ptr_p;
    other.d_ptr_p = tempPtr_p;

    BloombergLP::bslma::SharedPtrRep *tempRep_p = d_rep_p;
    d_rep_p       = other.d_rep_p;
    other.d_rep_p = tempRep_p;
}

// ADDITIONAL BSL MANIPULATORS
template<class ELEMENT_TYPE>
void
shared_ptr<ELEMENT_TYPE>::createInplace()
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<ELEMENT_TYPE> Rep;

    BloombergLP::bslma::Allocator *basicAllocator =
                                      BloombergLP::bslma::Default::allocator();

    Rep *rep = new (*basicAllocator) Rep(basicAllocator);
    SelfType(rep->ptr(), rep).swap(*this);
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class ELEMENT_TYPE>
template <class... ARGS>
void
shared_ptr<ELEMENT_TYPE>::createInplace(
                                 BloombergLP::bslma::Allocator *basicAllocator,
                                 ARGS&&...                      args)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<ELEMENT_TYPE> Rep;

    basicAllocator = BloombergLP::bslma::Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator,
                                  BSLS_COMPILERFEATURES_FORWARD(ARGS,args)...);
    SelfType(rep->ptr(), rep).swap(*this);
}
#endif

template <class ELEMENT_TYPE>
template <class ANY_TYPE>
void
shared_ptr<ELEMENT_TYPE>::loadAlias(const shared_ptr<ANY_TYPE>&  source,
                                    ELEMENT_TYPE                *object)
{
    if (source.d_rep_p == d_rep_p && object) {
        d_ptr_p = object;
    }
    else {
        SelfType(source, object).swap(*this);
    }
}

template <class ELEMENT_TYPE>
pair<ELEMENT_TYPE *, BloombergLP::bslma::SharedPtrRep *>
shared_ptr<ELEMENT_TYPE>::release() BSLS_KEYWORD_NOEXCEPT
{
    pair<ELEMENT_TYPE *, BloombergLP::bslma::SharedPtrRep *> ret(d_ptr_p,
                                                                 d_rep_p);
    d_ptr_p = 0;
    d_rep_p = 0;
    return ret;
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// DEPRECATED BDE LEGACY MANIPULATORS
template <class ELEMENT_TYPE>
inline
void shared_ptr<ELEMENT_TYPE>::clear() BSLS_KEYWORD_NOEXCEPT
{
    reset();
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
inline
void shared_ptr<ELEMENT_TYPE>::load(COMPATIBLE_TYPE *ptr)
{
    SelfType(ptr).swap(*this);
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
inline
void
shared_ptr<ELEMENT_TYPE>::load(COMPATIBLE_TYPE               *ptr,
                               BloombergLP::bslma::Allocator *basicAllocator)
{
    SelfType(ptr, basicAllocator).swap(*this);
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE, class DELETER>
inline
void
shared_ptr<ELEMENT_TYPE>::load(COMPATIBLE_TYPE               *ptr,
                               const DELETER&                 deleter,
                               BloombergLP::bslma::Allocator *basicAllocator)
{
    SelfType(ptr, deleter, basicAllocator).swap(*this);
}
#endif // BDE_OMIT_INTERNAL_DEPRECATED

// ACCESSORS
template <class ELEMENT_TYPE>
inline
# if defined(BSLS_PLATFORM_CMP_IBM)     // Last tested with xlC 12.1
shared_ptr<ELEMENT_TYPE>::operator typename shared_ptr::BoolType() const
                                                          BSLS_KEYWORD_NOEXCEPT
# else
shared_ptr<ELEMENT_TYPE>::operator BoolType() const BSLS_KEYWORD_NOEXCEPT
# endif
{
    return BloombergLP::bsls::UnspecifiedBool<shared_ptr>::makeValue(d_ptr_p);
}

template <class ELEMENT_TYPE>
inline
typename add_lvalue_reference<ELEMENT_TYPE>::type
shared_ptr<ELEMENT_TYPE>::operator*() const BSLS_KEYWORD_NOEXCEPT
{
    BSLS_ASSERT_SAFE(d_ptr_p);

    return *d_ptr_p;
}

template <class ELEMENT_TYPE>
inline
ELEMENT_TYPE *shared_ptr<ELEMENT_TYPE>::operator->() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_ptr_p;
}

template <class ELEMENT_TYPE>
inline
typename shared_ptr<ELEMENT_TYPE>::element_type *
shared_ptr<ELEMENT_TYPE>::get() const BSLS_KEYWORD_NOEXCEPT
{
    return d_ptr_p;
}

template <class ELEMENT_TYPE>
inline typename
add_lvalue_reference<typename shared_ptr<ELEMENT_TYPE>::element_type>::type
shared_ptr<ELEMENT_TYPE>::operator[](ptrdiff_t index) const
{
    BSLS_ASSERT_SAFE(d_ptr_p);

    return *(d_ptr_p + index);
}

template <class ELEMENT_TYPE>
template<class ANY_TYPE>
inline
bool shared_ptr<ELEMENT_TYPE>::owner_before(const shared_ptr<ANY_TYPE>& other)
                                                    const BSLS_KEYWORD_NOEXCEPT
{
    return std::less<BloombergLP::bslma::SharedPtrRep *>()(rep(), other.rep());
}

template <class ELEMENT_TYPE>
template<class ANY_TYPE>
inline
bool
shared_ptr<ELEMENT_TYPE>::owner_before(const weak_ptr<ANY_TYPE>& other) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return std::less<BloombergLP::bslma::SharedPtrRep *>()(rep(), other.rep());
}

template <class ELEMENT_TYPE>
inline
bool shared_ptr<ELEMENT_TYPE>::unique() const BSLS_KEYWORD_NOEXCEPT
{
    return 1 == use_count();
}

template <class ELEMENT_TYPE>
inline
long shared_ptr<ELEMENT_TYPE>::use_count() const BSLS_KEYWORD_NOEXCEPT
{
    return d_rep_p ? d_rep_p->numReferences() : 0;
}

// ADDITIONAL BSL ACCESSORS
template <class ELEMENT_TYPE>
BloombergLP::bslma::ManagedPtr<ELEMENT_TYPE>
shared_ptr<ELEMENT_TYPE>::managedPtr() const
{
    if (d_rep_p && d_ptr_p) {
        d_rep_p->acquireRef();
        return BloombergLP::bslma::ManagedPtr<ELEMENT_TYPE>(d_ptr_p,
                                                            d_rep_p,
                         &BloombergLP::bslma::SharedPtrRep::managedPtrDeleter);
                                                                      // RETURN
    }

    return BloombergLP::bslma::ManagedPtr<ELEMENT_TYPE>(
                     d_ptr_p,
                    (BloombergLP::bslma::SharedPtrRep *)0,
                    &BloombergLP::bslma::SharedPtrRep::managedPtrEmptyDeleter);
}

template <class ELEMENT_TYPE>
inline
BloombergLP::bslma::SharedPtrRep *shared_ptr<ELEMENT_TYPE>::rep() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_rep_p;
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// DEPRECATED BDE LEGACY ACCESSORS
template <class ELEMENT_TYPE>
inline
int shared_ptr<ELEMENT_TYPE>::numReferences() const BSLS_KEYWORD_NOEXCEPT
{
    return d_rep_p ? d_rep_p->numReferences() : 0;
}

template <class ELEMENT_TYPE>
inline
ELEMENT_TYPE *shared_ptr<ELEMENT_TYPE>::ptr() const BSLS_KEYWORD_NOEXCEPT
{
    return d_ptr_p;
}
#endif // BDE_OMIT_INTERNAL_DEPRECATED

                        // --------------
                        // class weak_ptr
                        // --------------

// CREATORS
template <class ELEMENT_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR
weak_ptr<ELEMENT_TYPE>::weak_ptr() BSLS_KEYWORD_NOEXCEPT
: d_ptr_p(0)
, d_rep_p(0)
{
}

template <class ELEMENT_TYPE>
weak_ptr<ELEMENT_TYPE>::weak_ptr(
         BloombergLP::bslmf::MovableRef<weak_ptr> original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_ptr_p(BloombergLP::bslmf::MovableRefUtil::access(original).d_ptr_p)
, d_rep_p(BloombergLP::bslmf::MovableRefUtil::access(original).d_rep_p)
{
    BloombergLP::bslmf::MovableRefUtil::access(original).d_rep_p = 0;
//    original.d_ptr_p = 0; // this seems overkill for a /weak/ pointer
}

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE
          BSLSTL_SHAREDPTR_DEFINE_IF_COMPATIBLE>
weak_ptr<ELEMENT_TYPE>::weak_ptr(weak_ptr<COMPATIBLE_TYPE>&& original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_ptr_p(original.d_ptr_p)
, d_rep_p(original.d_rep_p)
{
    original.d_rep_p = 0;
//    original.d_ptr_p = 0; // this seems overkill for a /weak/ pointer
}
#else
template <class ELEMENT_TYPE>
template <class CONVERTIBLE_TYPE
          BSLSTL_SHAREDPTR_DEFINE_IF_CONVERTIBLE>
weak_ptr<ELEMENT_TYPE>::weak_ptr(
          BloombergLP::bslmf::MovableRef<weak_ptr<CONVERTIBLE_TYPE> > original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_ptr_p(original.d_ptr_p)
, d_rep_p(original.d_rep_p)
{
    original.d_rep_p = 0;
//    original.d_ptr_p = 0; // this seems overkill for a /weak/ pointer
}
#endif

template <class ELEMENT_TYPE>
weak_ptr<ELEMENT_TYPE>::weak_ptr(const weak_ptr<ELEMENT_TYPE>& original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_ptr_p(original.d_ptr_p)
, d_rep_p(original.d_rep_p)
{
    if (d_rep_p) {
        d_rep_p->acquireWeakRef();
    }
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE
          BSLSTL_SHAREDPTR_DEFINE_IF_COMPATIBLE>
weak_ptr<ELEMENT_TYPE>::weak_ptr(const shared_ptr<COMPATIBLE_TYPE>& other)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_ptr_p(other.get())
, d_rep_p(other.rep())
{
    if (d_rep_p) {
        d_rep_p->acquireWeakRef();
    }
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE
          BSLSTL_SHAREDPTR_DEFINE_IF_COMPATIBLE>
weak_ptr<ELEMENT_TYPE>::weak_ptr(const weak_ptr<COMPATIBLE_TYPE>& other)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_ptr_p(other.d_ptr_p)
, d_rep_p(other.d_rep_p)
{
    if (d_rep_p) {
        d_rep_p->acquireWeakRef();
    }
}

template <class ELEMENT_TYPE>
inline
weak_ptr<ELEMENT_TYPE>::~weak_ptr()
{
    if (d_rep_p) {
        d_rep_p->releaseWeakRef();
    }
}

// PRIVATE MANIPULATORS
template <class ELEMENT_TYPE>
inline
void weak_ptr<ELEMENT_TYPE>::privateAssign(
                                      BloombergLP::bslma::SharedPtrRep *rep,
                                      ELEMENT_TYPE                     *target)
{
    if (d_rep_p) {
        d_rep_p->releaseWeakRef();
    }

    d_ptr_p = target;
    d_rep_p = rep;
    d_rep_p->acquireWeakRef();
}

// MANIPULATORS
template <class ELEMENT_TYPE>
weak_ptr<ELEMENT_TYPE>& weak_ptr<ELEMENT_TYPE>::operator=(
                                  BloombergLP::bslmf::MovableRef<weak_ptr> rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    weak_ptr tmp(BloombergLP::bslmf::MovableRefUtil::move(rhs));
    tmp.swap(*this);
    return *this;
}

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
typename enable_if<
    is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value,
    weak_ptr<ELEMENT_TYPE>&>::type
weak_ptr<ELEMENT_TYPE>::operator=(weak_ptr<COMPATIBLE_TYPE>&& rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    weak_ptr tmp(BloombergLP::bslmf::MovableRefUtil::move(rhs));
    tmp.swap(*this);
    return *this;
}
#else
template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
typename enable_if<
    is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value,
    weak_ptr<ELEMENT_TYPE>&>::type
weak_ptr<ELEMENT_TYPE>::operator=(
                BloombergLP::bslmf::MovableRef<weak_ptr<COMPATIBLE_TYPE> > rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    weak_ptr tmp(BloombergLP::bslmf::MovableRefUtil::move(rhs));
    tmp.swap(*this);
    return *this;
}
#endif

template <class ELEMENT_TYPE>
weak_ptr<ELEMENT_TYPE>& weak_ptr<ELEMENT_TYPE>::operator=(
                                             const weak_ptr<ELEMENT_TYPE>& rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
#if 1
    weak_ptr tmp(rhs);
    tmp.swap(*this);
#else
    // needs friendship, or use the cheating util class.
    privateAssign(rhs.d_rep_p, rhs.d_ptr_p);
#endif
    return *this;
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
typename enable_if<
    is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value,
    weak_ptr<ELEMENT_TYPE>&>::type
weak_ptr<ELEMENT_TYPE>::operator=(const shared_ptr<COMPATIBLE_TYPE>& rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
#if 1
    weak_ptr tmp(rhs);
    tmp.swap(*this);
#else
    // needs friendship, or use the cheating util class.
    privateAssign(rhs.d_rep_p, rhs.d_ptr_p);
#endif
    return *this;
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
typename enable_if<
    is_convertible<COMPATIBLE_TYPE *, ELEMENT_TYPE *>::value,
    weak_ptr<ELEMENT_TYPE>&>::type
weak_ptr<ELEMENT_TYPE>::operator=(const weak_ptr<COMPATIBLE_TYPE>& rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
#if 1
    weak_ptr tmp(rhs);
    tmp.swap(*this);
#else
    // needs friendship, or use the cheating util class.
    privateAssign(rhs.d_rep_p, rhs.d_ptr_p);
#endif
    return *this;
}

template <class ELEMENT_TYPE>
inline
void weak_ptr<ELEMENT_TYPE>::reset() BSLS_KEYWORD_NOEXCEPT
{
    if (d_rep_p) {
        d_rep_p->releaseWeakRef();
    }

    d_ptr_p = 0;
    d_rep_p = 0;
}

template <class ELEMENT_TYPE>
inline
void weak_ptr<ELEMENT_TYPE>::swap(weak_ptr<ELEMENT_TYPE>& other)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    // We directly implement swapping of two pointers, rather than simply
    // calling 'bsl::swap' or using 'bslalg::SwapUtil', to avoid (indirectly)
    // including the platform <algorithm> header, which may transitively
    // include other standard headers.  This reduces the risk of
    // platform-specific cycles, which have been observed to cause problems.

    ELEMENT_TYPE *tempPtr_p = d_ptr_p;
    d_ptr_p       = other.d_ptr_p;
    other.d_ptr_p = tempPtr_p;

    BloombergLP::bslma::SharedPtrRep *tempRep_p = d_rep_p;
    d_rep_p       = other.d_rep_p;
    other.d_rep_p = tempRep_p;
}

// ACCESSORS
template <class ELEMENT_TYPE>
inline
bool weak_ptr<ELEMENT_TYPE>::expired() const BSLS_KEYWORD_NOEXCEPT
{
    return !(d_rep_p && d_rep_p->numReferences());
}

template <class ELEMENT_TYPE>
shared_ptr<ELEMENT_TYPE> weak_ptr<ELEMENT_TYPE>::lock() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    if (d_rep_p && d_rep_p->tryAcquireRef()) {
        return shared_ptr<ELEMENT_TYPE>(
                    d_ptr_p,
                    d_rep_p,
                    BloombergLP::bslstl::SharedPtr_RepFromExistingSharedPtr());
                                                                      // RETURN
    }
    return shared_ptr<ELEMENT_TYPE>();
}

template <class ELEMENT_TYPE>
template <class ANY_TYPE>
inline
bool
weak_ptr<ELEMENT_TYPE>::owner_before(const shared_ptr<ANY_TYPE>& other) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return std::less<BloombergLP::bslma::SharedPtrRep *>()(d_rep_p,
                                                           other.rep());
}

template <class ELEMENT_TYPE>
template <class ANY_TYPE>
inline
bool
weak_ptr<ELEMENT_TYPE>::owner_before(const weak_ptr<ANY_TYPE>& other) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return std::less<BloombergLP::bslma::SharedPtrRep *>()(d_rep_p,
                                                           other.d_rep_p);
}

template <class ELEMENT_TYPE>
inline
BloombergLP::bslma::SharedPtrRep *weak_ptr<ELEMENT_TYPE>::rep() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_rep_p;
}

template <class ELEMENT_TYPE>
inline
long weak_ptr<ELEMENT_TYPE>::use_count() const BSLS_KEYWORD_NOEXCEPT
{
    return d_rep_p ? d_rep_p->numReferences() : 0;
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// DEPRECATED BDE LEGACY ACCESSORS
template <class ELEMENT_TYPE>
inline
shared_ptr<ELEMENT_TYPE> weak_ptr<ELEMENT_TYPE>::acquireSharedPtr() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return lock();
}

template <class ELEMENT_TYPE>
inline
int weak_ptr<ELEMENT_TYPE>::numReferences() const BSLS_KEYWORD_NOEXCEPT
{
    return d_rep_p ? d_rep_p->numReferences() : 0;
}
#endif // BDE_OMIT_INTERNAL_DEPRECATED

}  // close namespace bsl

namespace BloombergLP {
namespace bslstl {

                            // -----------------
                            // SharedPtr_ImpUtil
                            // -----------------

template <class SHARED_TYPE, class ENABLE_TYPE>
inline
void SharedPtr_ImpUtil::loadEnableSharedFromThis(
                    const bsl::enable_shared_from_this<ENABLE_TYPE> *result,
                    bsl::shared_ptr<SHARED_TYPE>                    *sharedPtr)
{
    BSLS_ASSERT(0 != sharedPtr);

    if (0 != result && result->d_weakThis.expired()) {
        result->d_weakThis.privateAssign(
                         sharedPtr->d_rep_p,
                         const_cast <ENABLE_TYPE      *>(
                         static_cast<ENABLE_TYPE const*>(sharedPtr->d_ptr_p)));
    }
}

inline
void bslstl::SharedPtr_ImpUtil::loadEnableSharedFromThis(const volatile void *,
                                                         const void *)
                                                          BSLS_KEYWORD_NOEXCEPT
{
}

template <class TYPE>
inline
TYPE *SharedPtr_ImpUtil::unqualify(const volatile TYPE *address)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return const_cast<TYPE *>(address);
}

template <class TYPE>
inline
void *SharedPtr_ImpUtil::voidify(TYPE *address) BSLS_KEYWORD_NOEXCEPT
{
    return static_cast<void *>(
            const_cast<typename bsl::remove_cv<TYPE>::type *>(address));
}

                            // --------------------
                            // struct SharedPtrUtil
                            // --------------------

// CLASS METHODS
template <class TARGET, class SOURCE>
inline
void SharedPtrUtil::constCast(bsl::shared_ptr<TARGET>        *target,
                              const bsl::shared_ptr<SOURCE>&  source)
{
    BSLS_ASSERT(0 != target);

    target->reset(source, const_cast<TARGET *>(source.get()));
}

template <class TARGET, class SOURCE>
inline
bsl::shared_ptr<TARGET>
SharedPtrUtil::constCast(const bsl::shared_ptr<SOURCE>& source)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return bsl::shared_ptr<TARGET>(source,
                                   const_cast<TARGET *>(source.get()));
}

template <class TARGET, class SOURCE>
inline
void SharedPtrUtil::dynamicCast(bsl::shared_ptr<TARGET>        *target,
                                const bsl::shared_ptr<SOURCE>&  source)
{
    BSLS_ASSERT(0 != target);

    if (TARGET *castPtr = dynamic_cast<TARGET *>(source.get())) {
        target->reset(source, castPtr);
    }
    else {
        target->reset();
    }
}

template <class TARGET, class SOURCE>
inline
bsl::shared_ptr<TARGET>
SharedPtrUtil::dynamicCast(const bsl::shared_ptr<SOURCE>& source)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    if (TARGET *castPtr = dynamic_cast<TARGET *>(source.get())) {
        return bsl::shared_ptr<TARGET>(source, castPtr);              // RETURN
    }

    return bsl::shared_ptr<TARGET>();
}

template <class TARGET, class SOURCE>
inline
void SharedPtrUtil::staticCast(bsl::shared_ptr<TARGET>        *target,
                               const bsl::shared_ptr<SOURCE>&  source)
{
    BSLS_ASSERT(0 != target);

    target->reset(source, static_cast<TARGET *>(source.get()));
}

template <class TARGET, class SOURCE>
inline
bsl::shared_ptr<TARGET>
SharedPtrUtil::staticCast(const bsl::shared_ptr<SOURCE>& source)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return bsl::shared_ptr<TARGET>(source,
                                   static_cast<TARGET *>(source.get()));
}

                        // --------------------------
                        // struct SharedPtrNilDeleter
                        // --------------------------

// ACCESSORS
inline
void SharedPtrNilDeleter::operator()(const volatile void *) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
}

                        // -------------------------------
                        // struct SharedPtr_DefaultDeleter
                        // -------------------------------

// ACCESSORS
template <>
template <class ANY_TYPE>
inline
void SharedPtr_DefaultDeleter<true>::operator()(ANY_TYPE *ptr) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    delete [] ptr;
}

template <>
template <class ANY_TYPE>
inline
void SharedPtr_DefaultDeleter<false>::operator()(ANY_TYPE *ptr) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    delete ptr;
}

                        // --------------------------
                        // class SharedPtr_RepProctor
                        // --------------------------

// CREATORS
inline
SharedPtr_RepProctor::SharedPtr_RepProctor(bslma::SharedPtrRep *rep)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_rep_p(rep)
{
}

inline
SharedPtr_RepProctor::~SharedPtr_RepProctor()
{
    if (d_rep_p) {
        d_rep_p->disposeRep();
    }
}

// MANIPULATORS
inline
void SharedPtr_RepProctor::release() BSLS_KEYWORD_NOEXCEPT
{
    d_rep_p = 0;
}

}  // close package namespace
}  // close enterprise namespace

// FREE OPERATORS
template <class LHS_TYPE, class RHS_TYPE>
inline
bool bsl::operator==(const shared_ptr<LHS_TYPE>& lhs,
                     const shared_ptr<RHS_TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return lhs.get() == rhs.get();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bsl::operator!=(const shared_ptr<LHS_TYPE>& lhs,
                     const shared_ptr<RHS_TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return !(lhs == rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bsl::operator<(const shared_ptr<LHS_TYPE>& lhs,
                    const shared_ptr<RHS_TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return std::less<const void *>()(lhs.get(), rhs.get());
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bsl::operator>(const shared_ptr<LHS_TYPE>& lhs,
                    const shared_ptr<RHS_TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return rhs < lhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bsl::operator<=(const shared_ptr<LHS_TYPE>& lhs,
                     const shared_ptr<RHS_TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return !(rhs < lhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bsl::operator>=(const shared_ptr<LHS_TYPE>& lhs,
                     const shared_ptr<RHS_TYPE>& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return !(lhs < rhs);
}

template <class LHS_TYPE>
inline
bool bsl::operator==(const shared_ptr<LHS_TYPE>& lhs, bsl::nullptr_t)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return !lhs;
}

template <class RHS_TYPE>
inline
bool bsl::operator==(bsl::nullptr_t, const shared_ptr<RHS_TYPE>& rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return !rhs;
}

template <class LHS_TYPE>
inline
bool bsl::operator!=(const shared_ptr<LHS_TYPE>& lhs, bsl::nullptr_t)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return static_cast<bool>(lhs);
}

template <class RHS_TYPE>
inline
bool bsl::operator!=(bsl::nullptr_t, const shared_ptr<RHS_TYPE>& rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return static_cast<bool>(rhs);
}

template <class LHS_TYPE>
inline
bool bsl::operator<(const shared_ptr<LHS_TYPE>& lhs, bsl::nullptr_t)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return std::less<LHS_TYPE *>()(lhs.get(), 0);
}

template <class RHS_TYPE>
inline
bool bsl::operator<(bsl::nullptr_t, const shared_ptr<RHS_TYPE>& rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return std::less<RHS_TYPE *>()(0, rhs.get());
}

template <class LHS_TYPE>
inline
bool bsl::operator<=(const shared_ptr<LHS_TYPE>& lhs, bsl::nullptr_t)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return !std::less<LHS_TYPE *>()(0, lhs.get());
}

template <class RHS_TYPE>
inline
bool bsl::operator<=(bsl::nullptr_t, const shared_ptr<RHS_TYPE>& rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return !std::less<RHS_TYPE *>()(rhs.get(), 0);
}

template <class LHS_TYPE>
inline
bool bsl::operator>(const shared_ptr<LHS_TYPE>& lhs, bsl::nullptr_t)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return std::less<LHS_TYPE *>()(0, lhs.get());
}

template <class RHS_TYPE>
inline
bool bsl::operator>(bsl::nullptr_t, const shared_ptr<RHS_TYPE>& rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return std::less<RHS_TYPE *>()(rhs.get(), 0);
}

template <class LHS_TYPE>
inline
bool bsl::operator>=(const shared_ptr<LHS_TYPE>& lhs, bsl::nullptr_t)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return !std::less<LHS_TYPE *>()(lhs.get(), 0);
}

template <class RHS_TYPE>
inline
bool bsl::operator>=(bsl::nullptr_t, const shared_ptr<RHS_TYPE>& rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return !std::less<RHS_TYPE *>()(0, rhs.get());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ELEMENT_TYPE>
inline
std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>&
bsl::operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>& stream,
                const shared_ptr<ELEMENT_TYPE>&             rhs)
{
    return stream << rhs.get();
}

// ASPECTS
template <class HASHALG, class ELEMENT_TYPE>
inline
void bsl::hashAppend(HASHALG& hashAlg, const shared_ptr<ELEMENT_TYPE>& input)
{
    hashAppend(hashAlg, input.get());
}

template <class ELEMENT_TYPE>
inline
void bsl::swap(shared_ptr<ELEMENT_TYPE>& a, shared_ptr<ELEMENT_TYPE>& b)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    a.swap(b);
}

template <class ELEMENT_TYPE>
inline
void bsl::swap(weak_ptr<ELEMENT_TYPE>& a, weak_ptr<ELEMENT_TYPE>& b)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    a.swap(b);
}

// STANDARD FREE FUNCTIONS
template<class DELETER, class ELEMENT_TYPE>
inline
DELETER *bsl::get_deleter(const shared_ptr<ELEMENT_TYPE>& p)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    BloombergLP::bslma::SharedPtrRep *rep = p.rep();
    return rep ? static_cast<DELETER *>(rep->getDeleter(typeid(DELETER))) : 0;
}

// STANDARD CAST FUNCTIONS
template<class TO_TYPE, class FROM_TYPE>
inline
bsl::shared_ptr<TO_TYPE>
bsl::const_pointer_cast(const shared_ptr<FROM_TYPE>& source)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return shared_ptr<TO_TYPE>(source, const_cast<TO_TYPE *>(source.get()));
}

template<class TO_TYPE, class FROM_TYPE>
inline
bsl::shared_ptr<TO_TYPE>
bsl::dynamic_pointer_cast(const shared_ptr<FROM_TYPE>& source)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    if (TO_TYPE *castPtr = dynamic_cast<TO_TYPE *>(source.get())) {
        return shared_ptr<TO_TYPE>(source, castPtr);                  // RETURN
    }

    return shared_ptr<TO_TYPE>();
}

template<class TO_TYPE, class FROM_TYPE>
inline
bsl::shared_ptr<TO_TYPE>
bsl::static_pointer_cast(const shared_ptr<FROM_TYPE>& source)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return shared_ptr<TO_TYPE>(source, static_cast<TO_TYPE *>(source.get()));
}

template<class TO_TYPE, class FROM_TYPE>
inline
bsl::shared_ptr<TO_TYPE>
bsl::reinterpret_pointer_cast(const shared_ptr<FROM_TYPE>& source)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return shared_ptr<TO_TYPE>(source,
                               reinterpret_cast<TO_TYPE *>(source.get()));
}

// STANDARD FACTORY FUNCTIONS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14

// C++11, PERFECT FORWARDING THROUGH A VARIADIC TEMPLATE

template<class ELEMENT_TYPE, class ALLOC, class... ARGS>
# if defined(BSLSTL_SHAREDPTR_NO_PARTIAL_ORDER_ON_ALLOCATOR_POINTER)
typename bsl::enable_if<!bsl::is_pointer<ALLOC>::value &&
                                           !bsl::is_array<ELEMENT_TYPE>::value,
                         bsl::shared_ptr<ELEMENT_TYPE> >::type
# else
typename bsl::enable_if<!bsl::is_array<ELEMENT_TYPE>::value,
                        bsl::shared_ptr<ELEMENT_TYPE> >::type
# endif
bsl::allocate_shared(ALLOC basicAllocator, ARGS&&... args)
{
    typedef BloombergLP::bslstl::SharedPtr_ImpUtil ImpUtil;

    typedef BloombergLP::bslstl::SharedPtrAllocateInplaceRep<ELEMENT_TYPE,
                                                             ALLOC> Rep;
    Rep *rep_p = Rep::makeRep(basicAllocator);

    BloombergLP::bslstl::SharedPtr_RepProctor proctor(rep_p);
    bsl::allocator_traits<ALLOC>::construct(
                                  basicAllocator,
                                  ImpUtil::unqualify(rep_p->ptr()),
                                  BSLS_COMPILERFEATURES_FORWARD(ARGS,args)...);
    proctor.release();
    return shared_ptr<ELEMENT_TYPE>(rep_p->ptr(), rep_p);
}

template<class ELEMENT_TYPE, class ALLOC, class... ARGS>
inline
typename bsl::enable_if<!bsl::is_array<ELEMENT_TYPE>::value,
                        bsl::shared_ptr<ELEMENT_TYPE> >::type
bsl::allocate_shared(ALLOC *basicAllocator, ARGS&&...  args)
{
    typedef BloombergLP::bslstl::SharedPtr_ImpUtil ImpUtil;
    typedef bsl::allocator<char>                   AllocatorType;
    typedef bsl::allocator_traits<AllocatorType>   AllocatorTraits;

    typedef BloombergLP::bslstl::SharedPtrAllocateInplaceRep<
                                                            ELEMENT_TYPE,
                                                            AllocatorType> Rep;
    AllocatorType  alloc(basicAllocator);
    Rep           *rep_p = Rep::makeRep(alloc);

    BloombergLP::bslstl::SharedPtr_RepProctor proctor(rep_p);
    AllocatorTraits::construct(alloc,
                               ImpUtil::unqualify(rep_p->ptr()),
                               BSLS_COMPILERFEATURES_FORWARD(ARGS,args)...);
    proctor.release();
    return shared_ptr<ELEMENT_TYPE>(rep_p->ptr(), rep_p);
}

// 'make_shared' using the default allocator

template<class ELEMENT_TYPE, class... ARGS>
inline
typename bsl::enable_if<!bsl::is_array<ELEMENT_TYPE>::value,
                        bsl::shared_ptr<ELEMENT_TYPE> >::type
bsl::make_shared(ARGS&&... args)
{
    typedef BloombergLP::bslstl::SharedPtr_ImpUtil ImpUtil;
    typedef bsl::allocator<char>                   AllocatorType;

    typedef BloombergLP::bslstl::SharedPtrAllocateInplaceRep<
                                                            ELEMENT_TYPE,
                                                            AllocatorType> Rep;

    AllocatorType  basicAllocator;
    Rep           *rep_p = Rep::makeRep(basicAllocator);

    BloombergLP::bslstl::SharedPtr_RepProctor proctor(rep_p);
    ::new (ImpUtil::voidify(rep_p->ptr())) ELEMENT_TYPE(
                                 BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    proctor.release();

    return shared_ptr<ELEMENT_TYPE>(rep_p->ptr(), rep_p);
}
#endif

// ============================================================================
//                              TYPE TRAITS
// ============================================================================

// Type traits for smart pointers:
//: o 'shared_ptr' has pointer semantics, but 'weak_ptr' does not.
//:
//: o Although 'shared_ptr' constructs with an allocator, it does not 'use' an
//:   allocator in the manner of the 'UsesBslmaAllocator' trait, and should be
//:   explicitly specialized as a clear sign to code inspection tools.
//:
//: o Smart pointers are bitwise-movable as long as there is no opportunity for
//:   holding a pointer to internal state in the immediate object itself.  As
//:   'd_ptr_p' is never exposed by reference, it is not possible to create an
//:   internal pointer, so the trait should be 'true'.

namespace BloombergLP {

namespace bslma {

template <class ELEMENT_TYPE>
struct UsesBslmaAllocator< ::bsl::shared_ptr<ELEMENT_TYPE> >
    : bsl::false_type
{};

}  // close namespace bslma

namespace bslmf {

template <class ELEMENT_TYPE>
struct HasPointerSemantics< ::bsl::shared_ptr<ELEMENT_TYPE> >
    : bsl::true_type
{};

template <class ELEMENT_TYPE>
struct IsBitwiseMoveable< ::bsl::shared_ptr<ELEMENT_TYPE> >
    : bsl::true_type
{};

template <class ELEMENT_TYPE>
struct IsBitwiseMoveable< ::bsl::weak_ptr<ELEMENT_TYPE> >
    : bsl::true_type
{};

}  // close namespace bslmf

}  // close enterprise namespace

#if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
# pragma GCC diagnostic pop
#endif

#undef BSLSTL_SHAREDPTR_DECLARE_IF_CONVERTIBLE
#undef BSLSTL_SHAREDPTR_DEFINE_IF_CONVERTIBLE

#undef BSLSTL_SHAREDPTR_DECLARE_IF_COMPATIBLE
#undef BSLSTL_SHAREDPTR_DEFINE_IF_COMPATIBLE

#undef BSLSTL_SHAREDPTR_DECLARE_IF_DELETER
#undef BSLSTL_SHAREDPTR_DEFINE_IF_DELETER

#undef BSLSTL_SHAREDPTR_DECLARE_IF_NULLPTR_DELETER
#undef BSLSTL_SHAREDPTR_DEFINE_IF_NULLPTR_DELETER

#undef BSLSTL_SHAREDPTR_SFINAE_DISCARD

#endif // End C++11 code

#endif

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
