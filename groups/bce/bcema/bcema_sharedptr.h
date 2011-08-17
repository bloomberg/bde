// bcema_sharedptr.h                                                  -*-C++-*-
#ifndef INCLUDED_BCEMA_SHAREDPTR
#define INCLUDED_BCEMA_SHAREDPTR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a generic reference-counted shared pointer wrapper.
//
//@CLASSES:
//  bcema_SharedPtr: shared pointer
//  bcema_SharedPtrUtil: shared pointer utility functions
//  bcema_SharedPtrLess: comparator functor for STL containers
//  bcema_SharedPtrNilDeleter: no-op deleter
//
//@AUTHOR: Ilougino Rocha (irocha)
//         Herve Bronnimann (hbronnim)
//         Vlad Kliatchko (vkliatch)
//         Raymond Chiu (schiu49)
//
//@SEE_ALSO: bdema_managedptr, bcema_weakptr
//
//@DESCRIPTION: This component implements a thread-safe, generic,
// reference-counted "smart pointer" to support "shared ownership" of objects
// (of parameterized 'TYPE').  Shared pointers implement a form of the
// "envelope/letter" idiom.  For each shared object, a representation that
// manages the number of references to it is created.  Many shared pointers can
// simultaneously refer to the same shared object by storing a reference to the
// same representation.  Shared pointers also implement the "construction is
// acquisition, destruction is release" idiom.  When a shared pointer is
// created it increments the number of shared references to the shared object
// that was specified to its constructor (or was referred to by a shared
// pointer passed to the copy constructor).  When a shared pointer is assigned
// to or destroyed, then the number of shared references to the shared object
// is decremented.  When all references to the shared object are released, both
// the representation and the object are destroyed.  'bcema_SharedPtr' emulates
// the interface of a native pointer.  The shared object may be accessed
// directly using the '->' operator, or the dereference operator (operator '*')
// can be used to obtain a reference to the shared object.
//
///Thread-Safety
///-------------
// This section qualifies the thread-safety of 'bcema_SharedPtr' objects
// themselves rather than the thread-safety of the objects being referenced.
//
// It is safe to access or modify two distinct 'bcema_SharedPtr' objects
// simultaneously, each from a separate thread, even if they share ownership
// of a common object.  It is safe to access a single 'bcema_SharedPtr' object
// simultaneously from two or more separate threads, provided no other thread
// is simultaneously modifying the object.  It is not safe to access or modify
// a 'bcema_SharedPtr' object in one thread while another thread modifies the
// same object.
//
// It is safe to access, modify, copy, or delete a shared pointer in one
// thread, while other threads access or modify other shared pointers pointing
// to or managing the same object (the reference count is managed using atomic
// operations).  However, there is no guarantee regarding the safety of
// accessing or modifying the object *referred* *to* by the shared pointer
// simultaneously from multiple threads.
//
///Shared and Weak References
///- - - - - - - - - - - - - -
// There are two types of references to shared objects:
//
// 1) A shared reference allows users to share the ownership of an object and
// control its lifetime.  A shared object is destroyed only when the last
// shared reference to it is released.  A shared reference to an object can be
// obtained by creating a 'bcema_SharedPtr' referring to it.
//
// 2) A weak reference provides users conditional access to an object without
// sharing its ownership (or affecting its lifetime).  A shared object can be
// destroyed even if there are weak references to it.  A weak reference to an
// object can be obtained by creating a 'bcema_WeakPtr' referring to the object
// from a 'bcema_SharedPtr' referring to that object.
//
///In-place/Out-of-place Representations
///-------------------------------------
// 'bcema_SharedPtr' provides two types of representations: an out-of-place
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
//  bslma_NewDeleteAllocator nda;
//  int *value = new (nda) int(10);
//  bcema_SharedPtr<int> outOfPlaceSharedPtr(value, &nda);
//..
// Next we create an in-place representation of a shared 'int' object that is
// also initialized to 10:
//..
//  bcema_SharedPtr<int> inPlaceSharedPtr;
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
// Creating an in-place shared pointer does not require the parameterized type
// to inherit from a special class (such as 'bsl::enable_shared_from_this'); in
// that case, 'bcema_SharedPtr' supports up to fourteen arguments that can be
// passed directly to the object's constructor.  For in-place representations,
// both the object and the representation can be constructed in one allocation
// as opposed to two, effectively creating an "intrusive" reference counter.
// Note that the size of the allocation is determined at compile-time from the
// combined footprint of the object and of the reference counter.  It is also
// possible to create shared pointers to buffers whose sizes are determined at
// runtime, although such buffers consist of raw (uninitialized) memory.
//
///Weak Pointers using "in-place" or Pooled Shared Pointer Representations
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A weak pointer that is not in the empty state shares a common representation
// (used to refer to the shared object) with the shared (or other weak)
// pointer from which it was constructed, and holds this representation until
// it is either destroyed or reset.  This common representation is not
// destroyed and deallocated (although the shared object itself may have been
// destroyed) until all weak references to that common representation have been
// released.
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
// 'ClassWithLargeFootprint' using the 'createInplace' method of
// 'bcema_SharedPtr'.  The 'sp' shared pointer representation of 'sp' will
// create a 'ClassWithLargeFootprint' object "in-place":
//..
//  bcema_SharedPtr<ClassWithLargeFootprint> sp;
//  sp.createInplace();
//..
// Next we construct a weak pointer from this (in-place) shared pointer:
//..
//  bcema_WeakPtr<ClassWithLargeFootprint> wp(sp);
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
//  bslma_Allocator *allocator1, *allocator2;
//  // ...
//  bcema_SharedPtr<bsl::string> ptr;
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
// When the last shared reference to a shared object is released, the object
// is destroyed using the "deleter" provided when the associated shared
// pointer representation was created.  'bcema_SharedPtr' supports two
// kinds of "deleter" objects, which vary in how they are invoked.  A
// "function-like" deleter is any language entity that can be invoked such
// that the expression 'deleterInstance(objectPtr)' is a valid expression, and
// a "factory" deleter is any language entity that can be invoked such that the
// expression 'deleterInstance.deleteObject(objectPtr)' is a valid expression,
// where 'deleterInstance' is an instance of the "deleter" object, and
// 'objectPtr' is a pointer to the shared object.  In summary:
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
//     my_Type *createObject(bslma_Allocator *basicAllocator = 0);
//         // Create a 'my_Type' object.  Optionally specify a
//         // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//         // 0, the currently installed default allocator is used.
//
//     void deleteObject(my_Type *object);
//         // Delete the specified 'object'.
//  };
//
//  class my_Allocator : public bslma_Allocator { /* ... */ };
//..
// Note that 'deleteObject' is provided by all 'bslma' allocators and by any
// object that implements the 'bcema_Deleter' protocol.  Thus, any of these
// objects can be used as a factory deleter.  The purpose of this design is to
// allow 'bslma' allocators and factories to be used seamlessly as deleters.
//
// The selection of which expression is used by 'bcema_SharedPtr' to destroy a
// shared object is based on how the deleter is passed to the shared pointer
// object: Deleters that are passed by *address* are assumed to be factory
// deleters, while those that are passed by *value* are assumed to be
// function-like.  Note that if the wrong interface is used for a deleter,
// i.e., if a function-like deleter is passed by pointer, or a factory deleter
// is passed by value, and the expression used to delete the object is invalid,
// a compiler diagnostic will be emitted indicating the error.
//
// The following are examples of constructing shared pointers with the
// addresses of factory deleters:
//..
//  my_Factory factory;
//  my_Type *myPtr1 = factory.createObject();
//  bcema_SharedPtr<my_Type> mySharedPtr1(myPtr1, &factory, 0);
//
//  bdema_SequentialAllocator sa;
//  my_Type *myPtr2 = new (sa) my_Type(&sa);
//  bcema_SharedPtr<my_Type> mySharedPtr2(myPtr2, &sa);
//..
// Note that the deleters are passed *by address* in the above examples.
//
// The following are examples of constructing shared pointers with
// function-like deleters:
//..
//  my_Type *getObject(bslma_Allocator *basicAllocator = 0);
//
//  my_Type *myPtr3 = getObject(bslma_Allocator *basicAllocator = 0);
//  bcema_SharedPtr<my_Type> mySharedPtr3(myPtr3, &deleteObject, 0);
//
//  my_Type *myPtr4 = getObject(bslma_Allocator *basicAllocator = 0);
//  FunctionLikeDeleterObject deleter;
//  bcema_SharedPtr<my_Type> mySharedPtr4(myPtr4, deleter, &sa);
//..
// Note that 'deleteObject' is also passed by address, but 'deleter' is passed
// by value in the above examples.  Function-like deleter objects (passed by
// value) are stored by value in the representation and therefore *must* be
// copy-constructible.  Note that even though the deleter may be passed by
// reference, it is a copy (owned by the shared pointer representation) that
// is invoked and thus the 'deleterInstance' is not required, nor assumed, to
// be non-modifiable.  (For the example above, note that 'operator()' is
// intentionally *not* defined 'const'.)
//
// Also note that the third argument (of type 'bslma_Allocator *') to the
// constructor of 'myOtherSharedPtr' is not optional as is otherwise the norm
// in this constructor form; you must always pass it, even if you want to use
// the default allocator (in which case you should pass 0, as above).  This is
// because making the allocator parameter optional results in the
// compiler having to make a decision between
// 'bcema_SharedPtr(TYPE *ptr, DELETER deleter, bslma_Allocator *allocator)'
// and 'bcema_SharedPtr(TYPE *ptr, bslma_Allocator *allocator)' and may select
// the function that the user does not expect.
//
///Aliasing
///--------
// 'bcema_SharedPtr' supports a powerful "aliasing" feature.  That is, a shared
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
//  void enqueueEvents(bcec_Queue<bcema_SharedPtr<Event> > *queue)
//  {
//      bsl::list<Event> eventList;
//      getEvents(&eventList);
//      for (bsl::list<Event>::iterator it = eventList.begin();
//           it != eventList.end();
//           ++it) {
//          bcema_SharedPtr<Event> e;
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
// 'bcema_SharedPtr' aliasing feature:
//..
//  void enqueueEvents(bcec_Queue<bcema_SharedPtr<Event> > *queue)
//  {
//      bcema_SharedPtr<bsl::list<Event> > eventList;
//      eventList.createInplace(0);  // Construct a shared pointer
//                                   // to the event list containing
//                                   // all of the events.
//      getEvents(eventList.ptr());
//
//      for (bsl::list<Event>::iterator it = eventList->begin();
//           it != eventList->end();
//           ++it) {
//          // Push each event onto the queue as an alias of the 'eventList'
//          // shared pointer.  When all the alias references have been
//          // released, the event list will be destroyed deleting all the
//          // events at once.
//
//          queue->pushBack(bcema_SharedPtr<Event>(eventList, &*it));
//      }
//  }
//..
// In the implementation above, we create a single shared pointer to the
// 'Event' list, 'eventList', and use that to create 'Event' shared pointers
// that are aliased to 'eventList'.  The lifetime of each 'Event' object is
// then tied to the 'eventList' and it will not be destroyed until the
// 'eventList' is destroyed.
//
///Comparison of Shared Pointers and Conversion to 'bool'
///------------------------------------------------------
// This component does *not* allow comparisons of shared pointers, although it
// does allow a conversion to an "unspecified bool type" (which is *not*
// 'bool') so as to retain the "comparison to 0" idiom:
//..
//  bslma_NewDeleteAllocator nda;
//  bcema_SharedPtr<int> sp1(new (nda) int(), &nda);
//  if (sp1) {   // OK
//      // . . .
//  }
//..
// The following attempts to compare two shared pointers:
//..
//  bcema_SharedPtr<double> sp2(new (nda) int(), &nda);
//  if (sp1 < sp2) {  // ERROR
//      // . . .
//  }
//..
// will both produce compilation errors, as will:
//..
//  bsl::map<bcema_SharedPtr<int>, int> sharedPtrMap;  // ERROR
//..
// (To be precise, the declaration of the map will not trigger an error, but
// the instantiation of any method that uses 'bsl::less' will.)
//
// In order to index a map by a shared pointer type, use the functor provided
// by the 'bcema_SharedPtrLess' class, as in:
//..
//  bsl::map<bcema_SharedPtr<int>, int, bcema_SharedPtrLess> sharedPtrMap;
//                                                                        // OK
//..
// 'bcema_SharedPtrLess' compares the pointer values referred to by the shared
// pointer keys in the map to sort the elements.
//
///Type Casting
///------------
// A 'bcema_SharedPtr' object of a given type can be implicitly or explicitly
// cast to a 'bcema_SharedPtr' of another type.
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
//  bcema_SharedPtr<B> spb;
//  bcema_SharedPtr<A> spa;
//  spa = spb;
//..
// and:
//..
//  bcema_SharedPtr<B> spb;
//  bcema_SharedPtr<A> spa(spb);
//..
// are also valid.  Note that in all of the above cases, the destructor of 'B'
// will be invoked when the object is destroyed even if 'A' does not provide
// a virtual destructor.
//
///Explicit Casting
/// - - - - - - - -
// Through "aliasing", a shared pointer of any type can be explicitly cast to a
// shared pointer of any other type using any legal cast expression.  For
// example, to statically cast a shared pointer to type 'A'
// ('bcema_SharedPtr<A>') to a shared pointer to type 'B'
// ('bcema_SharedPtr<B>'), one can simply do the following:
//..
//  bcema_SharedPtr<A> spa;
//  bcema_SharedPtr<B> spb(spa, static_cast<B *>(spa.ptr()));
//..
// or even the less safe C-style cast:
//..
//  bcema_SharedPtr<A> spa;
//  bcema_SharedPtr<B> spb(spa, (B *)(spa.ptr()));
//..
// For convenience, several utility functions are provided to perform common
// C++ casts.  Dynamic casts, static casts, and 'const' casts are all provided.
// Explicit casting is supported through the 'bcema_SharedPtrUtil' utility.
// The following example demonstrates the dynamic casting of a shared pointer
// to type 'A' ('bcema_SharedPtr<A>') to a shared pointer to type 'B'
// ('bcema_SharedPtr<B>'):
//..
//  bslma_NewDeleteAllocator nda;
//  bcema_SharedPtr<A> sp1(new (nda) A(), &nda);
//  bcema_SharedPtr<B> sp2 = bcema_SharedPtrUtil::dynamicCast<B>(sp1);
//  bcema_SharedPtr<B> sp3;
//  bcema_SharedPtrUtil::dynamicCast(&sp3, sp1);
//  bcema_SharedPtr<B> sp4;
//  sp4 = bcema_SharedPtrUtil::dynamicCast<B>(sp1);
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
///Converting to and from 'bdema_ManagedPtr'
///-----------------------------------------
// A 'bcema_SharedPtr' can be converted to a 'bdema_ManagedPtr' while still
// retaining proper reference counting.  When a shared pointer is converted to
// a 'bdema_ManagedPtr', the number of references to the shared object is
// incremented.  When the managed pointer is destroyed (if not transferred to
// another managed pointer first), the number of references will be
// decremented.  If the number of references reaches zero, then the shared
// object will be destroyed.  The 'managedPtr' function can be used to create a
// managed pointer from a shared pointer.
//
// A 'bcema_SharedPtr' also can be constructed from a 'bdema_ManagedPtr'.  The
// resulting shared pointer takes over the management of the object and will
// use the deleter from the original 'bdema_ManagedPtr' to destroy the managed
// object when all the references to that shared object are released.
//
///Storing a 'bcema_SharedPtr' in an Invokable in a 'bdef_Function' object
///-----------------------------------------------------------------------
// In addition to the guarantees already made in the 'bdef_function' component,
// 'bcema_sharedptr' also guarantees that storing a shared pointer to an
// invokable object in a 'bdef_Function' object will be "in-place", i.e., it
// will not trigger memory allocation.
//
///C++ Standard Compliance
///-----------------------
// This component provides a (subset of a) standard-compliant implementation of
// 'std::shared_ptr' (sections 2.2.3 of the final "Technical Report on C++
// Library Extensions" and 20.9.12.2 of the current "Final Committee Draft,
// Standard for Programming Language C++").  Support for 'std::weak_ptr' is
// provided in the 'bcema_weakptr' component.  Also, this component allows
// conversion or assignment from 'bsl::auto_ptr' and provides most of the
// interface of 'std::shared_ptr'.  In addition, it does not collaborate with
// types that derive from 'std::enable_shared_this': using 'bcema_SharedPtr'
// with such types will result in (very destructive) undefined behavior.  The
// only global free function supplied with this component is 'swap'.
//
// As mentioned in the section "Comparison of Shared Pointers" above, using
// comparison operators 'operator<', 'operator<=', 'operator>', and
// 'operator>=', and the corresponding specializations for 'bsl::less',
// 'bsl::less_equal', 'bsl::greater', and 'bsl::greater_equal' with shared
// pointers, will cause a compilation error.  And there is a comparison functor
// in 'bcema_SharedPtrUtil' for use in place of 'bsl::less' in 'bsl::map' and
// other standard associative containers.
//
// Finally, this component supports allocators following the 'bslma_Allocator'
// protocol and not the C++ Standard Allocators (section 20.1.2).
//
// CAVEAT: This implementation follows the BDE conventions on passing 0 as a
// 'bslma_Allocator *' argument.  A user who wishes to remain compliant with
// future C++ Standard specifications should not rely on this default allocator
// behavior.  To remain compliant with the standard 'std::shared_ptr', future
// versions of this component may use the '::operator delete' to destroy the
// shared object if an allocator was not specified to the appropriate method.
//
///Usage
///-----
// The following examples demonstrate various features and uses of shared
// pointers.
//
///Example 1 - Basic Usage
///- - - - - - - - - - - -
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
//      MyUser(bslma_Allocator *alloc = 0) : d_name(alloc), d_id(0) {}
//      MyUser(const bsl::string& name, int id, bslma_Allocator *alloc = 0)
//      : d_name(name, alloc)
//      , d_id(id)
//      {
//      }
//      MyUser(const MyUser& original, bslma_Allocator *alloc = 0)
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
//  bcema_SharedPtr<MyUser> createUser(bsl::string      name,
//                                     int              id,
//                                     bslma_Allocator *allocator = 0)
//  {
//      allocator = bslma_Default::allocator(allocator);
//      MyUser *user = new (*allocator) MyUser(name, id, allocator);
//      return bcema_SharedPtr<MyUser>(user, allocator);
//  }
//..
// Since the 'createUser' function both allocates the object and creates the
// shared pointer, it can benefit from the in-place facilities to avoid an
// extra allocation.  Again, note that the representation will also be
// allocated using the same allocator (see the section "Correct Usage of the
// Allocator Model" above).  Also note that if 'allocator' is 0, the
// currently-installed default allocator is used.
//..
//  bcema_SharedPtr<MyUser> createUser2(bsl::string      name,
//                                      int              id,
//                                      bslma_Allocator *allocator = 0)
//  {
//      bcema_SharedPtr<MyUser> user;
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
///Example 2 - Nil deleters
///  -  -  -  -  -  -  -  -
// There are cases when an interface calls for an object to be passed as a
// shared pointer, but the object being passed is not owned by the caller
// (e.g., a pointer to a static variable).  In these cases, it is possible to
// create a shared pointer specifying 'bcema_SharedPtrNilDeleter' as the
// deleter.  The deleter function provided by 'bcema_SharedPtrNilDeleter' is a
// no-op and does not delete the object.  The following example demonstrates
// the use of 'bcema_SharedPtr' using a 'bcema_SharedPtrNilDeleter'.  The code
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
//      int enqueueTransaction(bcema_SharedPtr<MyUser>  user,
//                             const MyTransactionInfo& transaction);
//    public:
//      // CLASS METHODS
//      static MyUser *systemUser(bslma_Allocator *basicAllocator = 0);
//
//      // MANIPULATORS
//      int enqueueSystemTransaction(const MyTransactionInfo& transaction);
//
//      int enqueueUserTransaction(const MyTransactionInfo& transaction,
//                                 bcema_SharedPtr<MyUser>  user);
//
//  };
//..
// The 'systemUser' class method returns the same 'MyUser' object and should
// not be destroyed by its users:
//..
//  MyUser *MyTransactionManager::systemUser(bslma_Allocator *basicAllocator)
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
//                                  bcema_SharedPtr<MyUser>  user)
//  {
//      return enqueueTransaction(user, transaction);
//  }
//..
// For system transactions, we must use the 'MyUser' objected returned from
// the 'systemUser' 'static' method.  Since we do not own the returned object,
// we cannot directly construct a 'bcema_SharedPtr' object for it: doing so
// would result in the singleton being destroyed when the last reference to
// the shared pointer is released.  To solve this problem, we construct a
// 'bcema_SharedPtr' object for the system user using a nil deleter.  When
// the last reference to the shared pointer is released, although the deleter
// will be invoked to destroy the object, it will do nothing.
//..
//  int MyTransactionManager::enqueueSystemTransaction(
//                                        const MyTransactionInfo& transaction)
//  {
//      bcema_SharedPtr<MyUser> user(systemUser(),
//                                   bcema_SharedPtrNilDeleter(),
//                                   0);
//      return enqueueTransaction(user, transaction);
//  }
//..
//
///Example 3 - Custom Deleters
/// -  -  -  -  -  -  -  -  -
// The role of a "deleter" is to allow users to define a custom "cleanup"
// for a shared object.  Although cleanup generally involves destroying the
// object, this need not be the case.  The following example demonstrates the
// use of a custom deleter to construct "locked" pointers.  First we declare a
// custom deleter that, when invoked, releases the specified mutex and signals
// the specified condition variable.
//..
//  class my_MutexUnlockAndBroadcastDeleter {
//
//      // DATA
//      bcemt_Mutex     *d_mutex_p;  // mutex to lock (held, not owned)
//      bcemt_Condition *d_cond_p;   // condition variable used to broadcast
//                                   // (held, not owned)
//
//    public:
//      // CREATORS
//      my_MutexUnlockAndBroadcastDeleter(bcemt_Mutex     *mutex,
//                                        bcemt_Condition *cond)
//          // Create this 'my_MutexUnlockAndBroadcastDeleter' object.  Use
//          // the specified 'cond' to broadcast a signal and the specified
//          // 'mutex' to serialize access to 'cond'.  The behavior is
//          // undefined unless '0 != mutex' and '0 != cond'.
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
//  template <class TYPE>
//  class my_SafeQueue {
//
//      // DATA
//      bcemt_Mutex      d_mutex;
//      bcemt_Condition  d_cond;
//      bsl::deque<TYPE> d_queue;
//
//      // . . .
//
//    public:
//      // MANIPULATORS
//      void push(const TYPE& obj);
//
//      TYPE pop();
//
//      bcema_SharedPtr<bsl::deque<TYPE> > queue();
//  };
//
//  template <class TYPE>
//  void my_SafeQueue<TYPE>::push(const TYPE& obj)
//  {
//      bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
//      d_queue.push_back(obj);
//      d_cond.signal();
//  }
//
//  template <class TYPE>
//  TYPE my_SafeQueue<TYPE>::pop()
//  {
//      bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
//      while (!d_queue.size()) {
//         d_cond.wait(&d_mutex);
//      }
//      TYPE value(d_queue.front());
//      d_queue.pop_front();
//      return value;
//  }
//
//  template <class TYPE>
//  bcema_SharedPtr<bsl::deque<TYPE> > my_SafeQueue<TYPE>::queue()
//  {
//      return bcema_SharedPtr<bsl::deque<TYPE> >(
//                         &d_queue,
//                         MyMutexUnlockAndBroadcastDeleter(&d_mutex, &d_cond),
//                         0);
//  }
//..
//
///Implementation Hiding
///- - - - - - - - - - -
// 'bcema_SharedPtr' refers to the parameterized type on which it is
// instantiated "in name only".  This allows for the instantiation of shared
// pointers to incomplete or 'void' types.  This feature is useful for
// constructing interfaces where returning a pointer to a shared object is
// desirable, but in order to control access to the object its interface cannot
// be exposed.  The following examples demonstrate two techniques for achieving
// this goal using a 'bcema_SharedPtr'.
//
///Example 4 - Hidden Interfaces
/// -  -  -  -  -  -  -  -  -  -
// Example 4 demonstrates the use of incomplete types to hide the interface of
// a 'my_Session' type.  We begin by declaring the 'my_SessionManager'
// 'class', which allocates and manages 'my_Session' objects.  The
// interface ('.h') merely forward declares 'my_Session'.  The actual
// definition of the interface is in the implementation ('.cpp') file.
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
//      typedef bsl::map<int, bcema_SharedPtr<my_Session> > HandleMap;
//
//      // DATA
//      bcemt_Mutex      d_mutex;
//      HandleMap        d_handles;
//      int              d_nextSessionId;
//      bslma_Allocator *d_allocator_p;
//
//..
// It is useful to have a designated name for the 'bcema_SharedPtr' to
// 'my_Session':
//..
//    public:
//      // TYPES
//      typedef bcema_SharedPtr<my_Session> my_Handle;
//..
// We need only a default constructor:
//..
//      // CREATORS
//      my_SessionManager(bslma_Allocator *allocator = 0);
//..
// The 3 methods that follow construct a new session object and return a
// 'bcema_SharedPtr' to it.  Callers can transfer the pointer, but they cannot
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
//                 bslma_Allocator    *basicAllocator = 0);
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
//                         bslma_Allocator    *basicAllocator)
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
//  my_SessionManager::my_SessionManager(bslma_Allocator *allocator)
//  : d_nextSessionId(1)
//  , d_allocator_p(bslma_Default::allocator(allocator))
//  {
//  }
//
//  inline
//  my_SessionManager::my_Handle
//  my_SessionManager::openSession(const bsl::string& sessionName)
//  {
//      bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
//      my_Handle session(new(*d_allocator_p) my_Session(sessionName,
//                                                       d_nextSessionId++,
//                                                       d_allocator_p));
//      d_handles[session->handleId()] = session;
//      return session;
//  }
//
//  inline
//  void my_SessionManager::closeSession(my_Handle handle)
//  {
//      bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
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
///Example 5 - Opaque Types
///  -  -  -  -  -  -  -  -
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
//      typedef bsl::map<int, bcema_SharedPtr<void> > HandleMap;
//
//      // DATA
//      bcemt_Mutex      d_mutex;
//      HandleMap        d_handles;
//      int              d_nextSessionId;
//      bslma_Allocator *d_allocator_p;
//..
// It is useful to have a name for the 'void' 'bcema_SharedPtr' handle.
//..
//     public:
//      // TYPES
//      typedef bcema_SharedPtr<void> my_Handle;
//
//      // CREATORS
//      my_SessionManager(bslma_Allocator *allocator = 0);
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
//  my_SessionManager::my_SessionManager(bslma_Allocator *allocator)
//  : d_nextSessionId(1)
//  , d_allocator_p(bslma_Default::allocator(allocator))
//  {
//  }
//
//  // MANIPULATORS
//  inline
//  my_SessionManager::my_Handle
//  my_SessionManager::openSession(const bsl::string& sessionName)
//  {
//      bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
//..
// Notice that 'my_Handle', which is a shared pointer to 'void', can be
// transparently assigned to a shared pointer to a 'my_Session' object.  This
// is because the 'bcema_SharedPtr' interface allows shared pointers to types
// that can be cast to one another to be assigned directly.
//..
//      my_Handle session(new(*d_allocator_p) my_Session(sessionName,
//                                                       d_nextSessionId++,
//                                                       d_allocator_p));
//      bcema_SharedPtr<my_Session> myhandle =
//                        bcema_SharedPtrUtil::staticCast<my_Session>(session);
//      d_handles[myhandle->handleId()] = session;
//      return session;
//  }
//
//  inline
//  void my_SessionManager::closeSession(my_Handle handle)
//  {
//      bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
//..
// Perform a static cast from 'bcema_SharedPtr<void>' to
// 'bcema_SharedPtr<my_Session>'.
//..
//      bcema_SharedPtr<my_Session> myhandle =
//                         bcema_SharedPtrUtil::staticCast<my_Session>(handle);
//..
// Test to make sure that the pointer is non-null before using 'myhandle':
//..
//      if (!myhandle.ptr()) {
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
//      bcema_SharedPtr<my_Session> myhandle =
//                         bcema_SharedPtrUtil::staticCast<my_Session>(handle);
//
//      if (!myhandle.ptr()) {
//          return bsl::string();
//      } else {
//          return myhandle->sessionName();
//      }
//  }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTRREP
#include <bcema_sharedptrrep.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTRINPLACEREP
#include <bcema_sharedptrinplacerep.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTROUTOFPLACEREP
#include <bcema_sharedptroutofplacerep.h>
#endif

#ifndef INCLUDED_BDEMA_MANAGEDPTR
#include <bdema_managedptr.h>
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

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>       // 'bsl::swap'
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>      // 'bsl::binary_function'
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>          // 'bsl::auto_ptr'
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_TYPEINFO
#include <bsl_typeinfo.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>         // 'bsl::ptrdiff_t', 'bsl::size_t'
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>         // 'bsl::pair'
#endif

namespace BloombergLP {

template <class TYPE> struct bcema_SharedPtr_ReferenceType;

                        // ====================================
                        // struct bcema_SharedPtr_ReferenceType
                        // ====================================

template <class TYPE>
struct bcema_SharedPtr_ReferenceType {
    // This 'struct' defines some basic traits used by 'bcema_SharedPtr'.  It
    // is primarily used to allow shared pointers of type 'void' to work
    // properly.

    typedef TYPE& Reference;
};

template <>
struct bcema_SharedPtr_ReferenceType<void> {
    // This 'struct' provides a specialization of
    // 'bcema_SharedPtr_ReferenceType' for type 'void' and allows us to avoid
    // declaring a reference to 'void' (which is invalid).

    typedef void Reference;
};

                // ============================================
                // struct bcema_SharedPtr_UnspecifiedBoolHelper
                // ============================================

struct bcema_SharedPtr_UnspecifiedBoolHelper {
    // This 'struct' provides a member, 'd_member', whose pointer-to-member is
    // used to convert a shared pointer to an "unspecified boolean type".

    int d_member;
        // This data member is used solely for taking its address to return a
        // non-null pointer-to-member.  Note that the *value* of 'd_member' is
        // not used.
};

typedef int bcema_SharedPtr_UnspecifiedBoolHelper::*
                                               bcema_SharedPtr_UnspecifiedBool;
    // 'bcema_SharedPtr_UnspecifiedBool' is an alias for a pointer-to-member of
    // the 'bcema_SharedPtr_UnspecifiedBoolHelper' class.  This (opaque) type
    // can be used as an "unspecified boolean type" for converting a shared
    // pointer to 'bool' in contexts such as 'if (sp) { ... }' without actually
    // having a conversion to 'bool', or being less-than comparable (either of
    // which would enable undesirable implicit comparisons of shared
    // pointers to 'int' and less-than comparisons).

                        // =====================
                        // class bcema_SharedPtr
                        // =====================

template <class TYPE>
class bcema_SharedPtr {
    // This class provides a thread-safe reference-counted "smart pointer" to
    // support "shared ownership" of objects: a shared pointer ensures that the
    // shared object is destroyed, using the appropriate deletion method, only
    // when there are no shared references to it.  The object
    // (of parameterized 'TYPE') referred to by a shared pointer may be
    // accessed directly using the '->' operator, or the dereference operator
    // (operator '*') can be used to obtain a reference to that object.
    //
    // Note that the object referred to by a shared pointer representation is
    // usually the same as the object referred to by that shared pointer (of
    // the same 'TYPE'), but this need not always be true in the presence of
    // conversions or "aliasing": the object referred to (of parameterized
    // 'TYPE') by the shared pointer may differ from the object of type
    // 'BCE_OTHER_TYPE' (see the "Aliasing" section in the component-level
    // documentation) referred to by the shared pointer representation.
    //
    // More generally, this class supports a complete set of *in*-*core*
    // pointer semantic operations.

    // DATA
    TYPE               *d_ptr_p;  // pointer to the shared object

    bcema_SharedPtrRep *d_rep_p;  // pointer to the representation object
                                  // that manages the shared object

    // PRIVATE TYPES
    typedef bcema_SharedPtr<TYPE> SelfType;
        // 'SelfType' is an alias to this 'class'.

    // FRIENDS
    template <class BCE_OTHER_TYPE> friend class bcema_SharedPtr;

  public:
    // TYPES
    typedef TYPE ElementType;
        // 'ElementType' is an alias to the 'TYPE' template parameter.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(bcema_SharedPtr,
                                  bslalg_TypeTraitHasPointerSemantics,
                                  bslalg_TypeTraitBitwiseMoveable);

    // CREATORS
    bcema_SharedPtr();
        // Create an empty shared pointer, i.e., a shared pointer with no
        // representation, that does not refer to any object and has no
        // deleter.

    explicit bcema_SharedPtr(TYPE *ptr, bslma_Allocator *basicAllocator = 0);
        // Create a shared pointer that refers to and manages the modifiable
        // object of parameterized 'TYPE' at the specified 'ptr' address.
        // Optionally specify an 'basicAllocator' to allocate and deallocate
        // the internal representation of the shared pointer and to destroy the
        // shared object when all references have been released.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Note that if 'ptr' is 0, an empty shared pointer is created
        // and 'basicAllocator' is ignored.  Also note that as mentioned in the
        // "CAVEAT" in the "C++ Standard Compliance" section of the
        // component-level documentation, to comply with C++ standard
        // specifications, future implementations of 'bcema_SharedPtr' may
        // destroy the shared object using '::operator delete' if an allocator
        // is not specified.

    template <class BCE_OTHER_TYPE>
    explicit bcema_SharedPtr(BCE_OTHER_TYPE  *ptr,
                             bslma_Allocator *basicAllocator = 0);
        // Create a shared pointer that refers to and manages the modifiable
        // object of parameterized 'BCE_OTHER_TYPE' at the specified 'ptr'
        // address and refers to '(TYPE *)ptr'.  Optionally specify an
        // 'basicAllocator' used to allocate and deallocate the internal
        // representation of the shared pointer and to destroy the shared
        // object when all references have been released.  If 'basicAllocator'
        // is 0, the currently installed default allocator is used.  If
        // 'BCE_OTHER_TYPE *' is not implicitly convertible to 'TYPE *' then a
        // compiler diagnostic will be emitted indicating the error.  Note that
        // if 'ptr' is 0, an empty shared pointer is created and
        // 'basicAllocator' is ignored.  Also note that as mentioned in the
        // "CAVEAT" in the "C++ Standard Compliance" section of the
        // component-level documentation, to comply with C++ standard
        // specifications, future implementations of 'bcema_SharedPtr' may
        // destroy the shared object using '::operator delete' if an allocator
        // is not specified.

    template <class BCE_OTHER_TYPE>
    bcema_SharedPtr(bdema_ManagedPtr<BCE_OTHER_TYPE>  managedPtr,
                    bslma_Allocator                  *basicAllocator = 0);
        // Create a shared pointer that takes over the management of the
        // modifiable object (if any) previously managed by the specified
        // 'managedPtr' to the parameterized 'BCE_OTHER_TYPE', and that refers
        // to '(TYPE *)managedPtr.ptr()'.  The deleter used in the 'managedPtr'
        // will be used to destroy the shared object when all references have
        // been released.  Optionally specify an 'basicAllocator' used to
        // allocate and deallocate the internal representation of the shared
        // pointer.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  If 'BCE_OTHER_TYPE *' is not implicitly
        // convertible to 'TYPE *' then a compiler diagnostic will be emitted
        // indicating the error.  Note that if 'managedPtr' is empty, then an
        // empty shared pointer is created and 'basicAllocator' is ignored.

    template <class BCE_OTHER_TYPE>
    explicit bcema_SharedPtr(
                           bsl::auto_ptr<BCE_OTHER_TYPE>&  autoPtr,
                           bslma_Allocator                *basicAllocator = 0);
        // Create a shared pointer that takes over the management of the
        // modifiable object previously managed by the specified 'autoPtr' to
        // the parameterized 'BCE_OTHER_TYPE', and that refers to
        // '(TYPE *)autoPtr.get()'.  The global '::operator delete' will be
        // used to destroy the shared object when all references have been
        // released.  Optionally specify an 'basicAllocator' used to allocate
        // and deallocate the internal representation of the shared pointer.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.  If 'BCE_OTHER_TYPE *' is not implicitly convertible to
        // 'TYPE *' then a compiler diagnostic will be emitted indicating the
        // error.

    template <class BCE_OTHER_TYPE, class DELETER>
    bcema_SharedPtr(BCE_OTHER_TYPE  *ptr,
                    const DELETER&   deleter,
                    bslma_Allocator *basicAllocator);
        // Create a shared pointer that manages a modifiable object of
        // parameterized 'BCE_OTHER_TYPE' and refers to '(TYPE *)ptr', using
        // the specified 'deleter' to delete the shared object when all
        // references have been released and the specified 'basicAllocator' to
        // allocate and deallocate the internal representation of the shared
        // pointer.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  If 'DELETER' is a reference type, then 'deleter'
        // is assumed to be a function-like deleter that may be invoked to
        // destroy the object referred to by a single argument of type
        // 'BCE_OTHER_TYPE *' (i.e., 'deleter(ptr)' will be called to destroy
        // the shared object).  If 'DELETER' is a pointer type, then 'deleter'
        // is assumed to be a pointer to a factory object that exposes a
        // 'deleteObject(BCE_OTHER_TYPE *)' interface that will be invoked to
        // destroy the object at the 'ptr' address (i.e.,
        // 'deleter->deleteObject(ptr)' will be called to delete the shared
        // object).  (See the "Deleters" section in the component-level
        // documentation.)  If 'BCE_OTHER_TYPE *' is not implicitly convertible
        // to 'TYPE *' then a compiler diagnostic will be emitted indicating
        // the error.  Note that, for factory deleters, the specified 'deleter'
        // must remain valid until all references to 'ptr' have been released.
        // Also note that 'basicAllocator' is not optional in this constructor
        // form.  The reason is to avoid hiding the two-argument constructor:
        //..
        //  bcema_SharedPtr(BCE_OTHER_TYPE  *ptr,
        //                  bslma_Allocator *basicAllocator = 0)
        //..
        // defined above.  Allowing the third argument to this constructor to
        // be optional would cause it to be the best match for a constructor
        // invocation intended to match the preceding one.  Therefore, one must
        // always specify the allocator argument.  (Of course, one can
        // explicitly pass the 0 pointer to use the default allocator).  Also
        // note that if 'ptr' is 0, an empty shared pointer is created and
        // 'deleter' and 'basicAllocator' are ignored.

    template <class BCE_OTHER_TYPE>
    bcema_SharedPtr(const bcema_SharedPtr<BCE_OTHER_TYPE>&  source,
                    TYPE                                   *object);
        // Create a shared pointer that manages the same modifiable object (if
        // any) as the specified 'source' shared pointer to the parameterized
        // 'BCE_OTHER_TYPE', and that refers to the modifiable object at the
        // specified 'object' address.  The resulting shared pointer is known
        // as an "alias" of 'source'.  Note that typically the objects referred
        // to by 'source' and 'object' have identical lifetimes (e.g., one
        // might be a part of the other) so that the deleter for 'source' will
        // destroy them both, but they do not necessarily have the same type.
        // Also note that if either 'source' is empty or 'object' is null, then
        // an empty shared pointer is created.

    explicit
    bcema_SharedPtr(bcema_SharedPtrRep *rep);
        // Construct a shared pointer taking ownership of the specified 'rep'
        // and referring to the object stored in 'rep'.  Note that this method
        // *DOES* *NOT* increment the number of references to 'rep'.

    bcema_SharedPtr(TYPE *ptr, bcema_SharedPtrRep *rep);
        // Construct a shared pointer taking ownership of the specified 'rep'
        // and referring to the modifiable object at the specified 'ptr'
        // address.  Note that this method *DOES* *NOT* increment the number of
        // references to 'rep'.

    template <class BCE_OTHER_TYPE>
    bcema_SharedPtr(const bcema_SharedPtr<BCE_OTHER_TYPE>& other);
        // Create a shared pointer that manages the same modifiable object (if
        // any) as the specified 'other' shared pointer to the parameterized
        // 'BCE_OTHER_TYPE', using the same deleter as 'other' to destroy the
        // shared object, and that refers to '(TYPE *)other.ptr()'.  If
        // 'BCE_OTHER_TYPE *' is not implicitly convertible to 'TYPE *' then a
        // compiler diagnostic will be emitted indicating the error.  Note that
        // if 'other' is empty, then an empty shared pointer is created.

    bcema_SharedPtr(const bcema_SharedPtr<TYPE>& original);
        // Create a shared pointer that refers to and manages the same object
        // (if any) as the specified 'original' shared pointer, using the same
        // deleter as 'original' to destroy the shared object.  Note that if
        // 'original' is empty, then an empty shared pointer is created.

    ~bcema_SharedPtr();
        // Destroy this shared pointer.  If this shared pointer refers to a
        // (possibly shared) object, then release the reference to that object,
        // and destroy the shared object using its associated deleter if this
        // shared pointer is the last reference to that object.

    // MANIPULATORS
    bcema_SharedPtr<TYPE>& operator=(const bcema_SharedPtr<TYPE>& rhs);
        // Make this shared pointer refer to and manage the same modifiable
        // object as the specified 'rhs' shared pointer and using the same
        // deleter as 'rhs', and return a reference to this modifiable shared
        // pointer.  If this shared pointer is already managing a (possibly
        // shared) object, then release the shared reference to that object,
        // and destroy it using its associated deleter if this shared pointer
        // held the last shared reference to that object.  Note that if 'rhs'
        // is empty, then this shared pointer will also be empty after the
        // assignment.  Also note that if '*this' is the same object as 'rhs',
        // then this method has no effect.

    template <class BCE_OTHER_TYPE>
    bcema_SharedPtr<TYPE>&
    operator=(const bcema_SharedPtr<BCE_OTHER_TYPE>& rhs);
        // Make this shared pointer refer to and manage the same modifiable
        // object as the specified 'rhs' shared pointer to the parameterized
        // 'BCE_OTHER_TYPE', using the same deleter as 'rhs' and referring to
        // '(TYPE *)rhs.ptr()', and return a reference to this modifiable
        // shared pointer.  If this shared pointer is already managing a
        // (possibly shared) object, then release the shared reference to that
        // object, and destroy it using its associated deleter if this shared
        // pointer held the last shared reference to that object.  Note that if
        // 'rhs' is empty, then this shared pointer will also be empty after
        // the assignment.

    template <class BCE_OTHER_TYPE>
    bcema_SharedPtr<TYPE>& operator=(bsl::auto_ptr<BCE_OTHER_TYPE>& rhs);
        // Transfer ownership to this shared pointer of the modifiable object
        // managed by the 'rhs' auto pointer to the parameterized
        // 'BCE_OTHER_TYPE', using '::operator delete' to destroy the shared
        // object when the last reference is released, and make this shared
        // pointer refer to '(TYPE *)rhs.get()'.  If this shared pointer is
        // already managing a (possibly shared) object, then release the
        // reference to that shared object, and destroy it using it associated
        // deleter if this shared pointer held the last shared reference to
        // that object.  Note that if 'rhs' is null, then this shared pointer
        // will also be empty after the assignment.

    void clear();
        // Reset this shared pointer to the empty state.  If this shared
        // pointer is managing a (possibly shared) object, then release the
        // reference to the shared object, calling the deleter to destroy the
        // shared object if this pointer is the last reference.
        //
        // DEPRECATED: Use 'reset' instead.

    template <class BCE_OTHER_TYPE>
    void load(BCE_OTHER_TYPE *ptr, bslma_Allocator *basicAllocator = 0);
        // Modify this shared pointer to manage the modifiable object of the
        // parameterized 'BCE_OTHER_TYPE' at the specified 'ptr' address and
        // refer to '(TYPE *)ptr'.  If this shared pointer is already managing
        // a (possibly shared) object, then release the shared reference to
        // that shared object, and destroy it using its associated deleter if
        // this shared pointer held the last shared reference to that object.
        // Optionally specify an 'basicAllocator' used to allocate and
        // deallocate the internal representation of this shared pointer and to
        // destroy the shared object when all references have been released.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.  If 'BCE_OTHER_TYPE *' is not implicitly convertible to
        // 'TYPE *' then a compiler diagnostic will be emitted indicating the
        // error.  Note that if 'ptr' is 0, then this shared pointer will be
        // reset to the empty state and 'basicAllocator' will be ignored.  Also
        // note that as mentioned in the "CAVEAT" in the "C++ Standard
        // Compliance" section of the component-level documentation, to comply
        // with C++ standard specifications, future implementations of
        // 'bcema_SharedPtr' may destroy the shared object using '::operator
        // delete' if an allocator is not specified.

    template <class BCE_OTHER_TYPE, class DELETER>
    void load(BCE_OTHER_TYPE  *ptr,
              const DELETER&   deleter,
              bslma_Allocator *basicAllocator);
        // Modify this shared pointer to manage the modifiable object of the
        // parameterized 'BCE_OTHER_TYPE' at the specified 'ptr' address and to
        // refer to '(TYPE *)ptr', using the specified 'deleter' to delete the
        // shared object when all references have been released and the
        // specified 'basicAllocator' to allocate and deallocate the internal
        // representation of the shared pointer.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.  If this shared
        // pointer is already managing a (possibly shared) object, then release
        // the shared reference to that shared object, and destroy it using its
        // associated deleter if this shared pointer held the last shared
        // reference to that object.  If 'DELETER' is a reference type, then
        // 'deleter' is assumed to be a function-like deleter that may be
        // invoked to destroy the object referred to by a single argument of
        // type 'BCE_OTHER_TYPE *' (i.e., 'deleter(ptr)' will be called to
        // destroy the shared object).  If 'DELETER' is a pointer type, then
        // 'deleter' is assumed to be a pointer to a factory object that
        // exposes a 'deleteObject(BCE_OTHER_TYPE *)' interface that will be
        // invoked to destroy the object at the 'ptr' address (i.e.,
        // 'deleter->deleteObject(ptr)' will be called to delete the shared
        // object).  (See the "Deleters" section in the component-level
        // documentation.)  If 'BCE_OTHER_TYPE *' is not implicitly convertible
        // to 'TYPE *' then a compiler diagnostic will be emitted indicating
        // the error.  Note that, for factory deleters, the specified 'deleter'
        // must remain valid until all references to 'ptr' have been released.
        // Also note that if 'ptr' is 0, then this shared pointer is reset to
        // the empty state and both 'deleter' and 'basicAllocator' are ignored.
        // Also note that this function is logically equivalent to:
        //..
        //  *this = bcema_SharedPtr<TYPE>(ptr, deleter, allocator);
        //..
        // and that, for the same reasons as explained in the constructor, the
        // third 'basicAllocator' argument is not optional.

    template <class BCE_OTHER_TYPE>
    void loadAlias(const bcema_SharedPtr<BCE_OTHER_TYPE>&  source,
                   TYPE                                   *object);
        // Modify this shared pointer to manage the same modifiable object (if
        // any) as the specified 'source' shared pointer to the parameterized
        // 'BCE_OTHER_TYPE', and refer to the modifiable object at the
        // specified 'object' address (i.e., make this shared pointer an
        // "alias" of 'source').  If this shared pointer is already managing a
        // (possibly shared) object, then release the shared reference to that
        // shared object, and destroy it using its associated deleter if this
        // shared pointer held the last shared reference to that object.  Note
        // that typically the objects referred to by 'source' and 'object' have
        // identical lifetimes (e.g., one might be a part of the other) so that
        // the deleter for 'source' will destroy them both, but they do not
        // necessarily have the same type.  Note that if either 'source' is
        // unset or 'object' is null, then this shared pointer will be reset to
        // the empty state.  Also note that this function is logically
        // equivalent to:
        //..
        //  *this = bcema_SharedPtr<TYPE>(source, object);
        //..

    void createInplace(bslma_Allocator *basicAllocator = 0);
        // Create "in-place" in a large enough contiguous memory region both an
        // internal representation for this shared pointer and a
        // default-constructed instance of 'TYPE', and make this shared pointer
        // refer to the newly-created 'TYPE' object.  Optionally specify an
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  If an exception
        // is thrown during allocation or construction of the 'TYPE' object,
        // this shared pointer will be unchanged.  Otherwise, if this shared
        // pointer is already managing a (possibly shared) object, then release
        // the shared reference to that shared object, and destroy it using its
        // associated deleter if this shared pointer held the last shared
        // reference to that object.  Note that the allocator argument is *not*
        // passed to the constructor for 'TYPE'.  To construct an in-place
        // 'TYPE' with an allocator, use one of the other variants of
        // 'createInplace' below.

    template <class A1>
    void createInplace(bslma_Allocator *basicAllocator, const A1& a1);
    template <class A1, class A2>
    void createInplace(bslma_Allocator *basicAllocator, const A1& a1,
                       const A2& a2);
    template <class A1, class A2, class A3>
    void createInplace(bslma_Allocator *basicAllocator, const A1& a1,
                       const A2& a2, const A3& a3);
    template <class A1, class A2, class A3, class A4>
    void createInplace(bslma_Allocator *basicAllocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4);
    template <class A1, class A2, class A3, class A4, class A5>
    void createInplace(bslma_Allocator *basicAllocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5);
    template <class A1, class A2, class A3, class A4, class A5, class A6>
    void createInplace(bslma_Allocator *basicAllocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7>
    void createInplace(bslma_Allocator *basicAllocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8>
    void createInplace(bslma_Allocator *basicAllocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9>
    void createInplace(bslma_Allocator *basicAllocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8, const A9& a9);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10>
    void createInplace(bslma_Allocator *basicAllocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8, const A9& a9, const A10& a10);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11>
    void createInplace(bslma_Allocator *basicAllocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8, const A9& a9, const A10& a10,
                       const A11& a11);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12>
    void createInplace(bslma_Allocator *basicAllocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8, const A9& a9, const A10& a10,
                       const A11& a11, const A12& a12);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12,
              class A13>
    void createInplace(bslma_Allocator *basicAllocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8, const A9& a9, const A10& a10,
                       const A11& a11, const A12& a12, const A13& a13);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12,
              class A13, class A14>
    void createInplace(bslma_Allocator *basicAllocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8, const A9& a9, const A10& a10,
                       const A11& a11, const A12& a12, const A13& a13,
                       const A14& a14);
        // Create "in-place" in a large enough contiguous memory region, using
        // the specified 'basicAllocator' to supply memory, both an internal
        // representation for this shared pointer and an object of 'TYPE' using
        // the 'TYPE' constructor that takes the specified arguments 'a1' up to
        // 'aN' where 'N' (at most 14) is the number of arguments passed to
        // this method, and make this shared pointer refer to the newly-created
        // 'TYPE' object.  If an exception is thrown during the construction of
        // the 'TYPE' object, this shared pointer will be unchanged.
        // Otherwise, if this shared pointer is already managing a (possibly
        // shared) object, then release the shared reference to that shared
        // object, and destroy it using its associated deleter if this shared
        // pointer held the last shared reference to that object.  Note that
        // the allocator argument is *not* implicitly passed to the constructor
        // for 'TYPE'.  To construct an instance of 'TYPE' with an allocator,
        // pass the allocator as one of the arguments (typically the last
        // argument).

    bsl::pair<TYPE *, bcema_SharedPtrRep *> release();
        // Return the pair consisting of the addresses of the modifiable 'TYPE'
        // object referred to, and the representation shared by, this shared
        // pointer, and reset this shared pointer to the empty state with no
        // effect on the representation.  The reference counter is not modified
        // nor is the shared object deleted; if the reference count of this
        // representation is greater than one, then it is not safe to release
        // the representation (thereby destroying the shared object), but it
        // is always safe to create another shared pointer with this
        // representation using the constructor with the signature
        // 'bcema_SharedPtr(TYPE *ptr, bcema_SharedPtrRep *rep)'.  Also note
        // that this function returns a pair of null pointers if this shared
        // pointer is empty.

    void swap(bcema_SharedPtr<TYPE>& source);
        // Efficiently exchange the states of this shared pointer and the
        // specified 'source' shared pointer such that each will refer to the
        // object formerly referred to by the other and each will manage the
        // object formerly referred to by the other.

    // ACCESSORS
    operator bcema_SharedPtr_UnspecifiedBool() const;
        // Return a value of an "unspecified bool" type that evaluates to
        // 'false' if this shared pointer is in the empty state, and 'true'
        // otherwise.  Note that this conversion operator allows a shared
        // pointer to be used within a conditional context (e.g., within an
        // 'if' or 'while' statement), but does *not* allow shared pointers to
        // be compared (e.g., via '<' or '>').

    typename bcema_SharedPtr_ReferenceType<TYPE>::Reference
    operator[](bsl::ptrdiff_t index) const;
        // Return a reference to the modifiable object at the specified 'index'
        // offset in object referred to by this shared pointer.  The behavior
        // is undefined unless this shared pointer is not empty, 'TYPE' is not
        // 'void' (a compiler error will be generated if this operator is
        // instantiated within the 'bcema_SharedPtr<void>' class), and this
        // shared pointer refers to an array of 'TYPE' objects.  Note that this
        // is logically equivalent to '*(ptr() + index)'.

    typename bcema_SharedPtr_ReferenceType<TYPE>::Reference
    operator*() const;
        // Return a reference to the modifiable object referred to by this
        // shared pointer.  The behavior is undefined unless this shared
        // pointer is not empty, and 'TYPE' is not 'void' (a compiler error
        // will be generated if this operator is instantiated within the
        // 'bcema_SharedPtr<void>' class).

    TYPE *operator->() const;
        // Return the address of the modifiable object referred to by this
        // shared pointer, or 0 if this shared pointer is empty.  Note that
        // applying this operator conventionally (e.g., to invoke a method) to
        // an empty shared pointer will result in undefined behavior.

    TYPE *ptr() const;
        // Return the address of the modifiable object referred to by this
        // shared pointer, or 0 if this shared pointer is empty.

    bcema_SharedPtrRep *rep() const;
        // Return the address of the modifiable 'bcema_SharedPtrRep' instance
        // used by this shared pointer, or 0 if this shared pointer is empty.

    int numReferences() const;
        // Return a "snapshot" of the number of shared pointers (including this
        // one) that share ownership of the object referred to by this shared
        // pointer.

    bdema_ManagedPtr<TYPE> managedPtr() const;
        // Return a managed pointer that refers to the same object as this
        // shared pointer and which has a deleter that decrements the
        // reference count for the shared object.

                        // *** std::tr1 COMPATIBILITY ***

    // TYPES
    typedef TYPE element_type;
        // 'element_type' is an alias to the 'TYPE' template parameter.  Note
        // that 'element_type' refers to the same type as 'ElementType'.

    // MANIPULATORS
    void reset();
        // Reset this shared pointer to the empty state.  If this shared
        // pointer is managing a (possibly shared) object, then release the
        // reference to the shared object, calling the deleter to destroy the
        // shared object if this pointer is the last reference.

    template <class BCE_OTHER_TYPE>
    void reset(BCE_OTHER_TYPE *ptr);
        // Modify this shared pointer to manage the modifiable object of the
        // parameterized 'BCE_OTHER_TYPE' at the specified 'ptr' address and to
        // refer to '(TYPE *)ptr'.  If this shared pointer is already managing
        // a (possibly shared) object, then release the reference to the
        // shared object, calling the deleter to destroy the shared object if
        // this pointer is the last reference.  The currently installed default
        // allocator is used to allocate the internal representation of this
        // shared pointer and to destroy the shared object when all references
        // have been released.  If 'BCE_OTHER_TYPE *' is not implicitly
        // convertible to 'TYPE *' then a compiler diagnostic will be emitted
        // indicating the error.  Note that if 'ptr' is 0, then this shared
        // pointer will be reset to the empty state.  Note that the behavior of
        // this method is the same as 'load(ptr)'.

    template <class BCE_OTHER_TYPE, class DELETER>
    void reset(BCE_OTHER_TYPE *ptr, const DELETER& deleter);
        // Modify this shared pointer to manage the modifiable object of the
        // parameterized 'BCE_OTHER_TYPE' at the specified 'ptr' address and
        // refer to '(TYPE *)ptr', using the specified 'deleter' to delete the
        // shared object when all references have been released.  If this
        // shared pointer is already managing a (possibly shared) object, then
        // release the reference to the shared object, calling the deleter to
        // destroy the shared object if this pointer is the last reference.
        // If 'DELETER' is a reference type, then 'deleter' is assumed to be a
        // function-like deleter that may be invoked to destroy the object
        // referred to by a single argument of type 'BCE_OTHER_TYPE *' (i.e.,
        // 'deleter(ptr)' will be called to destroy the shared object).  If
        // 'DELETER' is a pointer type, then 'deleter' is assumed to be a
        // pointer to a factory object that exposes a
        // 'deleteObject(BCE_OTHER_TYPE *)' interface that will be invoked to
        // destroy the object at the 'ptr' address (i.e.,
        // 'deleter->deleteObject(ptr)' will be called to delete the shared
        // object).  (See the "Deleters" section in the component-level
        // documentation.)  The currently installed default allocator is used
        // to allocate the internal representation of this shared pointer and
        // to destroy the shared object when all references have been
        // released.  If 'BCE_OTHER_TYPE *' is not implicitly convertible to
        // 'TYPE *' then a compiler diagnostic will be emitted indicating the
        // error.  Note that, for factory deleters, 'deleter' must remain valid
        // until all references to 'ptr' have been released.  Also note that if
        // 'ptr' is 0, then this shared pointer will be reset to the empty
        // state and 'deleter' is ignored.  The behavior of this method is the
        // same as 'load(ptr, deleter, (bslma_Allocator *)0)'.

    template <class BCE_OTHER_TYPE>
    void reset(const bcema_SharedPtr<BCE_OTHER_TYPE>& source, TYPE *ptr);
        // Modify this shared pointer to manage the same modifiable object (if
        // any) as the specified 'source' shared pointer to the parameterized
        // 'BCE_OTHER_TYPE', and refer to the modifiable object at the
        // specified 'ptr' address (i.e., make this shared pointer an "alias"
        // of 'source').  If this shared pointer is already managing a
        // (possibly shared) object, then release the reference to the shared
        // object, calling the deleter to destroy the shared object if this
        // pointer is the last reference.  Note that typically the objects
        // referred to by 'source' and 'ptr' have identical lifetimes (e.g.,
        // one might be a part of the other) so that the deleter for 'source'
        // will destroy them both, but do not necessarily have the same type.
        // Also note that if either 'source' is unset or 'ptr' is null, then
        // this shared pointer will be reset to the empty state.  Also note
        // that the behavior of this method is the same as
        // 'loadAlias(source, ptr)'.

    // ACCESSORS
    TYPE *get() const;
        // Return the address of the modifiable object referred to by this
        // shared pointer, or 0 if this shared pointer is empty.  Note that the
        // behavior of this function is the same as 'ptr'.

    bool unique() const;
        // Return 'true' if this shared pointer does not share ownership of the
        // object it refers to with any other shared pointer, and 'false'
        // otherwise.

    int  use_count() const;
        // Return a "snapshot" of the number of shared pointers (including this
        // one) that share ownership of the object referred to by this shared
        // pointer.  Note that the behavior of this function is the same as
        // 'numReferences'.
};

// FREE OPERATORS
template <typename LHS_TYPE, typename RHS_TYPE>
bool operator==(const bcema_SharedPtr<LHS_TYPE>& lhs,
                const bcema_SharedPtr<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' shared pointer refers to the same
    // object (if any) as that being referred to by the specified 'rhs' shared
    // pointer (if any), and 'false' otherwise.  Note that if a (raw) pointer
    // to 'LHS_TYPE' cannot be compared to a (raw) pointer to 'RHS_TYPE', then
    // a compiler diagnostic will be emitted indicating the error.  Also note
    // that two equal shared pointers do not necessarily manage the same object
    // due to aliasing.

template <typename LHS_TYPE, typename RHS_TYPE>
bool operator!=(const bcema_SharedPtr<LHS_TYPE>& lhs,
                const bcema_SharedPtr<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' shared pointer does not refer to
    // the same object (if any) as that being referred to by the specified
    // 'rhs' shared pointer (if any), and 'false' otherwise.  Note that if a
    // (raw) pointer to 'LHS_TYPE' cannot be compared to a (raw) pointer to
    // 'RHS_TYPE', then a compiler diagnostic will be emitted indicating the
    // error.  Also note that two equal shared pointers do not necessarily
    // manage the same object due to aliasing.

template <typename TYPE>
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bcema_SharedPtr<TYPE>& object);
    // Print to the specified 'stream' the address of the shared object
    // referred to by the specified 'object' shared pointer and return a
    // reference to the modifiable 'stream'.

                        // *** std::tr1 COMPATIBILITY ***

template <typename TYPE>
void swap(bcema_SharedPtr<TYPE>& a, bcema_SharedPtr<TYPE>& b);
    // Efficiently exchange the states of the specified 'a' and 'b' shared
    // pointers such that each will refer to the object formerly referred to by
    // the other, and each will manage the object formerly referred to by the
    // other.

                        // ==========================
                        // struct bcema_SharedPtrLess
                        // ==========================

struct bcema_SharedPtrLess {
    // This class provides a functor for comparing two shared pointers to the
    // parameterized 'TYPE' suitable for use in standard associative containers
    // such as 'bsl::map'.  Note that this class can be used to compare two
    // shared pointers of arbitrary types (as long as one is convertible to the
    // other).

    // ACCESSORS
    template <typename TYPE>
    bool operator()(const bcema_SharedPtr<TYPE>& a,
                    const bcema_SharedPtr<TYPE>& b) const;
        // Return 'true' if the specified 'a' shared pointer to the
        // parameterized 'TYPE' is less than the specified 'b' shared pointer
        // to the same 'TYPE', and 'false' otherwise.  A shared pointer ('a')
        // compares less than another shared pointer ('b') if the address of
        // the shared object referred to by 'a' is less than the address of the
        // shared object referred to by 'b' as compared by the
        // 'bsl::less<TYPE *>' functor.  Note that the expression 'a < b' will
        // *not* compile.
};

                        // ==========================
                        // struct bcema_SharedPtrUtil
                        // ==========================

struct bcema_SharedPtrUtil {
    // This 'struct' provides a namespace for operations on shared pointers.

    // TYPES
    template <typename TYPE>
    struct PtrLess : bsl::binary_function<bcema_SharedPtr<TYPE>,
                                          bcema_SharedPtr<TYPE>,
                                          bool>
    {
        // This class template provides a functor for comparing two shared
        // pointers to the parameterized 'TYPE' suitable for use in standard
        // associative containers such as 'bsl::map'.

        // ACCESSORS
        bool operator()(const bcema_SharedPtr<TYPE>& a,
                        const bcema_SharedPtr<TYPE>& b) const;
            // Return 'true' if the specified 'a' shared pointer to the
            // parameterized 'TYPE' is less than the specified 'b' shared
            // pointer to the same 'TYPE', and 'false' otherwise.  A shared
            // pointer ('a') compares less than another shared pointer ('b') if
            // the address of the shared object referred to by 'a' is less than
            // the address of the shared object referred to by 'b' as compared
            // by the 'bsl::less<TYPE *>' functor.  Note that the expression
            // 'a < b' will *not* compile.
    };

    // CLASS METHODS
    template <class TARGET, class SOURCE>
    static
    bcema_SharedPtr<TARGET> dynamicCast(const bcema_SharedPtr<SOURCE>& source);
        // Return a 'bcema_SharedPtr<TARGET>' object sharing ownership of the
        // same object as the specified 'source' shared pointer to the
        // parameterized 'SOURCE' type, and referring to
        // 'dynamic_cast<TARGET *>(source.ptr())'.  If 'source' cannot be
        // dynamically cast to 'TARGET *', then an empty
        // 'bcema_SharedPtr<TARGET>' object is returned.

    template <class TARGET, class SOURCE>
    static
    bcema_SharedPtr<TARGET> staticCast(const bcema_SharedPtr<SOURCE>& source);
        // Return a 'bcema_SharedPtr<TARGET>' object sharing ownership of the
        // same object as the specified 'source' shared pointer to the
        // parameterized 'SOURCE' type, and referring to
        // 'static_cast<TARGET *>(source.ptr())'.  Note that if 'source' cannot
        // be statically cast to 'TARGET *', then a compiler diagnostic will be
        // emitted indicating the error.

    template <class TARGET, class SOURCE>
    static
    bcema_SharedPtr<TARGET> constCast(const bcema_SharedPtr<SOURCE>& source);
        // Return a 'bcema_SharedPtr<TARGET>' object sharing ownership of the
        // same object as the specified 'source' shared pointer to the
        // parameterized 'SOURCE' type, and referring to
        // 'const_cast<TARGET *>(source.ptr())'.  Note that if 'source' cannot
        // be 'const'-cast to 'TARGET *', then a compiler diagnostic will be
        // emitted indicating the error.

    template <class TARGET, class SOURCE>
    static
    void dynamicCast(bcema_SharedPtr<TARGET>        *target,
                     const bcema_SharedPtr<SOURCE>&  source);
        // Load into the specified 'target' shared pointer an alias of the
        // specified 'source' shared pointer referring to
        // 'dynamic_cast<TARGET *>(source.ptr())'.  The previous 'target'
        // shared pointer is destroyed (destroying the shared object if
        // 'target' holds the last reference to this object).  If 'source'
        // cannot be dynamically cast to 'TARGET *', 'target' will be reset to
        // the empty state.

    template <class TARGET, class SOURCE>
    static
    void staticCast(bcema_SharedPtr<TARGET>        *target,
                    const bcema_SharedPtr<SOURCE>&  source);
        // Load into the specified 'target' shared pointer an alias of the
        // specified 'source' shared pointer referring to
        // 'static_cast<TARGET *>(source.ptr())'.  The previous 'target' shared
        // pointer is destroyed (destroying the shared object if 'target'
        // holds the last reference to this object).  Note that if 'source'
        // cannot be statically cast to 'TARGET *', then a compiler diagnostic
        // will be emitted indicating the error.

    template <class TARGET, class SOURCE>
    static
    void constCast(bcema_SharedPtr<TARGET>        *target,
                   const bcema_SharedPtr<SOURCE>&  source);
        // Load into the specified 'target' shared pointer an alias of the
        // specified 'source' shared pointer referring to
        // 'const_cast<TARGET *>(source.ptr())'.  The previous 'target' shared
        // pointer is destroyed (destroying the shared object if 'target'
        // holds the last reference to this object).  Note that if 'source'
        // cannot be statically cast to 'TARGET *', then a compiler diagnostic
        // will be emitted indicating the error.

    static
    bcema_SharedPtr<char>
    createInplaceUninitializedBuffer(bsl::size_t      bufferSize,
                                     bslma_Allocator *basicAllocator = 0);
        // Return a shared pointer with an in-place representation to a
        // newly-created uninitialized buffer of the specified 'bufferSize' (in
        // bytes).  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.
};

                        // ================================
                        // struct bcema_SharedPtrNilDeleter
                        // ================================

struct bcema_SharedPtrNilDeleter {
    // This 'struct' provides a function-like shared pointer deleter that does
    // nothing when invoked.

    // MANIPULATORS
    template <class TYPE>
    void operator()(TYPE *)
        // No-Op.
    {
    }
};

// ============================================================================
//                      INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                            // ---------------------
                            // class bcema_SharedPtr
                            // ---------------------

// CREATORS
template <class TYPE>
inline
bcema_SharedPtr<TYPE>::bcema_SharedPtr()
: d_ptr_p(0)
, d_rep_p(0)
{
}

template <class TYPE>
inline
bcema_SharedPtr<TYPE>::bcema_SharedPtr(TYPE            *ptr,
                                       bslma_Allocator *basicAllocator)
{
    d_ptr_p = ptr;
    d_rep_p = bcema_SharedPtrOutofplaceRep<TYPE, bslma_Allocator*>::
                        makeOutofplaceRep(ptr, basicAllocator, basicAllocator);
}

template <class TYPE>
template <class BCE_OTHER_TYPE>
bcema_SharedPtr<TYPE>::bcema_SharedPtr(BCE_OTHER_TYPE  *ptr,
                                       bslma_Allocator *basicAllocator)
{
    d_ptr_p = ptr;
    d_rep_p = bcema_SharedPtrOutofplaceRep<BCE_OTHER_TYPE, bslma_Allocator*>::
                        makeOutofplaceRep(ptr, basicAllocator, basicAllocator);
}

template <class TYPE>
template <class BCE_OTHER_TYPE>
bcema_SharedPtr<TYPE>::bcema_SharedPtr(
                              bdema_ManagedPtr<BCE_OTHER_TYPE>  managedPtr,
                              bslma_Allocator                  *basicAllocator)
: d_ptr_p(managedPtr.ptr())
, d_rep_p(0)
{
    typedef bcema_SharedPtrInplaceRep<bdema_ManagedPtr<TYPE> > Rep;

    if (d_ptr_p) {
        typedef void (*ManagedPtrDeleter)(void *, bcema_SharedPtrRep *);

        if (&bcema_SharedPtrRep::managedPtrDeleter ==
                           (ManagedPtrDeleter)managedPtr.deleter().deleter()) {
            d_rep_p = (bcema_SharedPtrRep *)
                                         managedPtr.release().second.factory();
        }
        else {
            basicAllocator = bslma_Default::allocator(basicAllocator);
            Rep *rep = new(*basicAllocator) Rep(basicAllocator);
            (*rep->ptr()) = managedPtr;
            d_rep_p = rep;
        }
    }
}

template <class TYPE>
template <class BCE_OTHER_TYPE>
bcema_SharedPtr<TYPE>::bcema_SharedPtr(
                                bsl::auto_ptr<BCE_OTHER_TYPE>&  autoPtr,
                                bslma_Allocator                *basicAllocator)
: d_ptr_p(autoPtr.get())
, d_rep_p(0)
{
    typedef bcema_SharedPtrInplaceRep<bsl::auto_ptr<BCE_OTHER_TYPE> > Rep;

    if (d_ptr_p) {
        basicAllocator = bslma_Default::allocator(basicAllocator);
        Rep *rep = new (*basicAllocator) Rep(basicAllocator);
        (*rep->ptr()) = autoPtr;
        d_rep_p = rep;
    }
}

template <class TYPE>
template <class BCE_OTHER_TYPE, class DELETER>
bcema_SharedPtr<TYPE>::bcema_SharedPtr(BCE_OTHER_TYPE  *ptr,
                                       const DELETER&   deleter,
                                       bslma_Allocator *basicAllocator)
: d_ptr_p(ptr)
, d_rep_p(0)
{
    d_rep_p = bcema_SharedPtrOutofplaceRep<BCE_OTHER_TYPE, DELETER>::
                               makeOutofplaceRep(ptr, deleter, basicAllocator);
}

template <class TYPE>
template <class BCE_OTHER_TYPE>
bcema_SharedPtr<TYPE>::bcema_SharedPtr(
                                const bcema_SharedPtr<BCE_OTHER_TYPE>&  source,
                                TYPE                                   *object)
: d_ptr_p(object)
, d_rep_p(source.d_rep_p)
{
    if (d_ptr_p && d_rep_p) {
        d_rep_p->acquireRef();
    } else {
        d_ptr_p = 0;
        d_rep_p = 0;
    }
}

template <class TYPE>
inline
bcema_SharedPtr<TYPE>::bcema_SharedPtr(bcema_SharedPtrRep *rep)
: d_ptr_p(rep ? reinterpret_cast<TYPE *>(rep->originalPtr()) : 0)
, d_rep_p(rep)
{
}

template <class TYPE>
inline
bcema_SharedPtr<TYPE>::bcema_SharedPtr(TYPE *ptr, bcema_SharedPtrRep *rep)
: d_ptr_p(ptr)
, d_rep_p(rep)
{
}

template <class TYPE>
template <class BCE_OTHER_TYPE>
bcema_SharedPtr<TYPE>::bcema_SharedPtr(
                                  const bcema_SharedPtr<BCE_OTHER_TYPE>& other)
: d_ptr_p(other.d_ptr_p)
, d_rep_p(other.d_rep_p)
{
    if (d_ptr_p) {
        d_rep_p->acquireRef();
    } else {
        d_rep_p = 0;
    }
}

template <class TYPE>
bcema_SharedPtr<TYPE>::bcema_SharedPtr(const bcema_SharedPtr<TYPE>& original)
: d_ptr_p(original.d_ptr_p)
, d_rep_p(original.d_rep_p)
{
    if (d_ptr_p) {
        d_rep_p->acquireRef();
    } else {
        d_rep_p = 0;
    }
}

template <class TYPE>
bcema_SharedPtr<TYPE>::~bcema_SharedPtr()
{
    BSLS_ASSERT_SAFE(!d_rep_p || d_ptr_p);

    if (d_rep_p) {
        d_rep_p->releaseRef();
    }
}

// MANIPULATORS
template <class TYPE>
bcema_SharedPtr<TYPE>& bcema_SharedPtr<TYPE>::
                                    operator=(const bcema_SharedPtr<TYPE>& rhs)
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

template <class TYPE>
template <class BCE_OTHER_TYPE>
bcema_SharedPtr<TYPE>& bcema_SharedPtr<TYPE>::operator=(
                                    const bcema_SharedPtr<BCE_OTHER_TYPE>& rhs)
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

template <class TYPE>
template <class BCE_OTHER_TYPE>
bcema_SharedPtr<TYPE>& bcema_SharedPtr<TYPE>::operator=(
                                            bsl::auto_ptr<BCE_OTHER_TYPE>& rhs)
{
    SelfType(rhs).swap(*this);
    return *this;
}

template <class TYPE>
void bcema_SharedPtr<TYPE>::clear()
{
    reset();
}

template <class TYPE>
template <class BCE_OTHER_TYPE>
void bcema_SharedPtr<TYPE>::load(BCE_OTHER_TYPE  *ptr,
                                 bslma_Allocator *basicAllocator)
{
    SelfType(ptr, basicAllocator).swap(*this);
}

template <class TYPE>
template <class BCE_OTHER_TYPE, class DELETER>
void bcema_SharedPtr<TYPE>::load(BCE_OTHER_TYPE  *ptr,
                                 const DELETER&   deleter,
                                 bslma_Allocator *basicAllocator)
{
    SelfType(ptr, deleter, basicAllocator).swap(*this);
}

template <class TYPE>
template <class BCE_OTHER_TYPE>
void bcema_SharedPtr<TYPE>::loadAlias(
                                const bcema_SharedPtr<BCE_OTHER_TYPE>&  source,
                                TYPE                                   *object)
{
    if (source.d_rep_p == d_rep_p && object) {
        d_ptr_p = d_rep_p ? object : 0;
    }
    else {
        SelfType(source, object).swap(*this);
    }
}

template <class TYPE>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *basicAllocator)
{
    typedef bcema_SharedPtrInplaceRep<TYPE> Rep;
    basicAllocator = bslma_Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *basicAllocator,
                                          const A1&        a1)
{
    typedef bcema_SharedPtrInplaceRep<TYPE> Rep;
    basicAllocator = bslma_Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator, a1);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *basicAllocator,
                                          const A1&        a1,
                                          const A2&        a2)
{
    typedef bcema_SharedPtrInplaceRep<TYPE> Rep;
    basicAllocator = bslma_Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator, a1, a2);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *basicAllocator,
                                          const A1&        a1,
                                          const A2&        a2,
                                          const A3&        a3)
{
    typedef bcema_SharedPtrInplaceRep<TYPE> Rep;
    basicAllocator = bslma_Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator, a1, a2, a3);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *basicAllocator,
                                          const A1&        a1,
                                          const A2&        a2,
                                          const A3&        a3,
                                          const A4&        a4)
{
    typedef bcema_SharedPtrInplaceRep<TYPE> Rep;
    basicAllocator = bslma_Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator, a1, a2, a3, a4);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *basicAllocator,
                                          const A1&        a1,
                                          const A2&        a2,
                                          const A3&        a3,
                                          const A4&        a4,
                                          const A5&        a5)
{
    typedef bcema_SharedPtrInplaceRep<TYPE> Rep;
    basicAllocator = bslma_Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator, a1, a2, a3, a4, a5);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *basicAllocator,
                                          const A1&        a1,
                                          const A2&        a2,
                                          const A3&        a3,
                                          const A4&        a4,
                                          const A5&        a5,
                                          const A6&        a6)
{
    typedef bcema_SharedPtrInplaceRep<TYPE> Rep;
    basicAllocator = bslma_Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator,
                                         a1,
                                         a2,
                                         a3,
                                         a4,
                                         a5,
                                         a6);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *basicAllocator,
                                          const A1&        a1,
                                          const A2&        a2,
                                          const A3&        a3,
                                          const A4&        a4,
                                          const A5&        a5,
                                          const A6&        a6,
                                          const A7&        a7)
{
    typedef bcema_SharedPtrInplaceRep<TYPE> Rep;
    basicAllocator = bslma_Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator,
                                         a1,
                                         a2,
                                         a3,
                                         a4,
                                         a5,
                                         a6,
                                         a7);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *basicAllocator,
                                          const A1&        a1,
                                          const A2&        a2,
                                          const A3&        a3,
                                          const A4&        a4,
                                          const A5&        a5,
                                          const A6&        a6,
                                          const A7&        a7,
                                          const A8&        a8)
{
    typedef bcema_SharedPtrInplaceRep<TYPE> Rep;
    basicAllocator = bslma_Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator,
                                         a1,
                                         a2,
                                         a3,
                                         a4,
                                         a5,
                                         a6,
                                         a7,
                                         a8);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *basicAllocator,
                                          const A1&        a1,
                                          const A2&        a2,
                                          const A3&        a3,
                                          const A4&        a4,
                                          const A5&        a5,
                                          const A6&        a6,
                                          const A7&        a7,
                                          const A8&        a8,
                                          const A9&        a9)
{
    typedef bcema_SharedPtrInplaceRep<TYPE> Rep;
    basicAllocator = bslma_Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator,
                                         a1,
                                         a2,
                                         a3,
                                         a4,
                                         a5,
                                         a6,
                                         a7,
                                         a8,
                                         a9);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *basicAllocator,
                                          const A1&        a1,
                                          const A2&        a2,
                                          const A3&        a3,
                                          const A4&        a4,
                                          const A5&        a5,
                                          const A6&        a6,
                                          const A7&        a7,
                                          const A8&        a8,
                                          const A9&        a9,
                                          const A10&       a10)
{
    typedef bcema_SharedPtrInplaceRep<TYPE> Rep;
    basicAllocator = bslma_Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator,
                                         a1,
                                         a2,
                                         a3,
                                         a4,
                                         a5,
                                         a6,
                                         a7,
                                         a8,
                                         a9,
                                         a10);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *basicAllocator,
                                          const A1&        a1,
                                          const A2&        a2,
                                          const A3&        a3,
                                          const A4&        a4,
                                          const A5&        a5,
                                          const A6&        a6,
                                          const A7&        a7,
                                          const A8&        a8,
                                          const A9&        a9,
                                          const A10&       a10,
                                          const A11&       a11)
{
    typedef bcema_SharedPtrInplaceRep<TYPE> Rep;
    basicAllocator = bslma_Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator,
                                         a1,
                                         a2,
                                         a3,
                                         a4,
                                         a5,
                                         a6,
                                         a7,
                                         a8,
                                         a9,
                                         a10,
                                         a11);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *basicAllocator,
                                          const A1&        a1,
                                          const A2&        a2,
                                          const A3&        a3,
                                          const A4&        a4,
                                          const A5&        a5,
                                          const A6&        a6,
                                          const A7&        a7,
                                          const A8&        a8,
                                          const A9&        a9,
                                          const A10&       a10,
                                          const A11&       a11,
                                          const A12&       a12)
{
    typedef bcema_SharedPtrInplaceRep<TYPE> Rep;
    basicAllocator = bslma_Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator,
                                         a1,
                                         a2,
                                         a3,
                                         a4,
                                         a5,
                                         a6,
                                         a7,
                                         a8,
                                         a9,
                                         a10,
                                         a11,
                                         a12);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *basicAllocator,
                                          const A1&        a1,
                                          const A2&        a2,
                                          const A3&        a3,
                                          const A4&        a4,
                                          const A5&        a5,
                                          const A6&        a6,
                                          const A7&        a7,
                                          const A8&        a8,
                                          const A9&        a9,
                                          const A10&       a10,
                                          const A11&       a11,
                                          const A12&       a12,
                                          const A13&       a13)
{
    typedef bcema_SharedPtrInplaceRep<TYPE> Rep;
    basicAllocator = bslma_Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator,
                                         a1,
                                         a2,
                                         a3,
                                         a4,
                                         a5,
                                         a6,
                                         a7,
                                         a8,
                                         a9,
                                         a10,
                                         a11,
                                         a12,
                                         a13);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *basicAllocator,
                                          const A1&        a1,
                                          const A2&        a2,
                                          const A3&        a3,
                                          const A4&        a4,
                                          const A5&        a5,
                                          const A6&        a6,
                                          const A7&        a7,
                                          const A8&        a8,
                                          const A9&        a9,
                                          const A10&       a10,
                                          const A11&       a11,
                                          const A12&       a12,
                                          const A13&       a13,
                                          const A14&       a14)
{
    typedef bcema_SharedPtrInplaceRep<TYPE> Rep;
    basicAllocator = bslma_Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator,
                                         a1,
                                         a2,
                                         a3,
                                         a4,
                                         a5,
                                         a6,
                                         a7,
                                         a8,
                                         a9,
                                         a10,
                                         a11,
                                         a12,
                                         a13,
                                         a14);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
bsl::pair<TYPE *, bcema_SharedPtrRep *> bcema_SharedPtr<TYPE>::release()
{
    bsl::pair<TYPE *, bcema_SharedPtrRep *> ret(d_ptr_p, d_rep_p);
    d_ptr_p = 0;
    d_rep_p = 0;
    return ret;
}

template <class TYPE>
void bcema_SharedPtr<TYPE>::swap(bcema_SharedPtr<TYPE>& source)
{
    bsl::swap(d_ptr_p, source.d_ptr_p);
    bsl::swap(d_rep_p, source.d_rep_p);
}

// ACCESSORS
template <class TYPE>
inline
bcema_SharedPtr<TYPE>::operator bcema_SharedPtr_UnspecifiedBool() const
{
    return d_ptr_p
           ? &bcema_SharedPtr_UnspecifiedBoolHelper::d_member
           : 0;
}

template <class TYPE>
inline
typename bcema_SharedPtr_ReferenceType<TYPE>::Reference
bcema_SharedPtr<TYPE>::operator[](bsl::ptrdiff_t index) const
{
    return *(d_ptr_p + index);
}

template <class TYPE>
inline
typename bcema_SharedPtr_ReferenceType<TYPE>::Reference
bcema_SharedPtr<TYPE>::operator*() const
{
    return *d_ptr_p;
}

template <class TYPE>
inline
TYPE *bcema_SharedPtr<TYPE>::operator->() const
{
    return d_ptr_p;
}

template <class TYPE>
inline
TYPE *bcema_SharedPtr<TYPE>::ptr() const
{
    return d_ptr_p;
}

template <class TYPE>
inline
bcema_SharedPtrRep *bcema_SharedPtr<TYPE>::rep() const
{
    return d_rep_p;
}

template <class TYPE>
inline
int bcema_SharedPtr<TYPE>::numReferences() const
{
    return d_rep_p ? d_rep_p->numReferences() : 0;
}

template <class TYPE>
bdema_ManagedPtr<TYPE> bcema_SharedPtr<TYPE>::managedPtr() const
{
    typedef void (*Deleter)(TYPE *, bcema_SharedPtrRep *);
    if (d_rep_p) {
        d_rep_p->acquireRef();
    }
    bdema_ManagedPtr<TYPE> ptr(d_ptr_p, d_rep_p,
                             (Deleter)&bcema_SharedPtrRep::managedPtrDeleter);
    return ptr;
}

                        // *** std::tr1 COMPATIBILITY ***

// MANIPULATORS
template <class TYPE>
inline
void bcema_SharedPtr<TYPE>::reset()
{
    bcema_SharedPtrRep *rep = d_rep_p;

    // Clear 'd_rep_p' first so that a self-referencing shared pointer's
    // destructor does not try to call 'releaseRef' again.

    d_rep_p = 0;
    d_ptr_p = 0;

    if (rep) {
        rep->releaseRef();
    }
}

template <class TYPE>
template <class BCE_OTHER_TYPE>
void bcema_SharedPtr<TYPE>::reset(BCE_OTHER_TYPE *ptr)
{
    // Wrap 'ptr' in 'auto_ptr' to ensure standard delete behavior.
    bsl::auto_ptr<BCE_OTHER_TYPE> ap(ptr);
    SelfType(ap).swap(*this);
}

template <class TYPE>
template <class BCE_OTHER_TYPE, class DELETER>
void bcema_SharedPtr<TYPE>::reset(BCE_OTHER_TYPE *ptr, const DELETER& deleter)
{
    SelfType(ptr, deleter, 0).swap(*this);
}

template <class TYPE>
template <class BCE_OTHER_TYPE>
void bcema_SharedPtr<TYPE>::reset(
                                const bcema_SharedPtr<BCE_OTHER_TYPE>&  source,
                                TYPE                                   *ptr)
{
    SelfType(source, ptr).swap(*this);
}

// ACCESSORS
template <class TYPE>
inline
TYPE *bcema_SharedPtr<TYPE>::get() const
{
    return ptr();
}

template <class TYPE>
inline
bool bcema_SharedPtr<TYPE>::unique() const
{
    return 1 == numReferences();
}

template <class TYPE>
inline
int bcema_SharedPtr<TYPE>::use_count() const
{
    return numReferences();
}

// FREE OPERATORS
template <typename LHS_TYPE, typename RHS_TYPE>
inline
bool operator==(const bcema_SharedPtr<LHS_TYPE>& lhs,
                const bcema_SharedPtr<RHS_TYPE>& rhs)
{
    return rhs.ptr() == lhs.ptr();
}

template <typename LHS_TYPE, typename RHS_TYPE>
inline
bool operator!=(const bcema_SharedPtr<LHS_TYPE>& lhs,
                const bcema_SharedPtr<RHS_TYPE>& rhs)
{
    return !(lhs == rhs);
}

template <typename TYPE>
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bcema_SharedPtr<TYPE>& rhs)
{
    stream << rhs.ptr();
    return stream;
}

                        // *** std::tr1 COMPATIBILITY ***

template <class TYPE>
void swap(bcema_SharedPtr<TYPE>& a, bcema_SharedPtr<TYPE>& b)
{
    a.swap(b);
}

                    // --------------------------
                    // struct bcema_SharedPtrLess
                    // --------------------------

// ACCESSORS
template <typename TYPE>
bool bcema_SharedPtrLess::operator()(const bcema_SharedPtr<TYPE>& a,
                                     const bcema_SharedPtr<TYPE>& b) const
{
    return bsl::less<TYPE *>()(a.ptr(), b.ptr());
}

                    // -----------------------------------
                    // struct bcema_SharedPtrUtil::PtrLess
                    // -----------------------------------

// ACCESSORS
template <typename TYPE>
bool bcema_SharedPtrUtil::PtrLess<TYPE>::operator()(
                                        const bcema_SharedPtr<TYPE>& a,
                                        const bcema_SharedPtr<TYPE>& b) const
{
    return bsl::less<TYPE *>()(a.ptr(), b.ptr());
}

                        // --------------------------
                        // struct bcema_SharedPtrUtil
                        // --------------------------

// CLASS METHODS
template <class TARGET, class SOURCE>
bcema_SharedPtr<TARGET>
bcema_SharedPtrUtil::dynamicCast(const bcema_SharedPtr<SOURCE>& source)
{
    return bcema_SharedPtr<TARGET>(source,
                                   dynamic_cast<TARGET *>(source.ptr()));
}

template <class TARGET, class SOURCE>
bcema_SharedPtr<TARGET>
bcema_SharedPtrUtil::staticCast(const bcema_SharedPtr<SOURCE>& source)
{
    return bcema_SharedPtr<TARGET>(source,
                                   static_cast<TARGET *>(source.ptr()));
}

template <class TARGET, class SOURCE>
bcema_SharedPtr<TARGET>
bcema_SharedPtrUtil::constCast(const bcema_SharedPtr<SOURCE>& source)
{
    return bcema_SharedPtr<TARGET>(source,
                                   const_cast<TARGET *>(source.ptr()));
}

template <class TARGET, class SOURCE>
void bcema_SharedPtrUtil::dynamicCast(bcema_SharedPtr<TARGET>        *target,
                                      const bcema_SharedPtr<SOURCE>&  source)
{
    target->loadAlias(source, dynamic_cast<TARGET *>(source.ptr()));
}

template <class TARGET, class SOURCE>
void bcema_SharedPtrUtil::staticCast(bcema_SharedPtr<TARGET>        *target,
                                     const bcema_SharedPtr<SOURCE>&  source)
{
    target->loadAlias(source, static_cast<TARGET *>(source.ptr()));
}

template <class TARGET, class SOURCE>
void bcema_SharedPtrUtil::constCast(bcema_SharedPtr<TARGET>        *target,
                                    const bcema_SharedPtr<SOURCE>&  source)
{
    target->loadAlias(source, const_cast<TARGET *>(source.ptr()));
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
