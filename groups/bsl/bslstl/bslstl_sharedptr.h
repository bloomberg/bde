// bslstl_sharedptr.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_SHAREDPTR
#define INCLUDED_BSLSTL_SHAREDPTR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a generic reference-counted shared pointer wrapper.
//
//@CLASSES:
//  bsl::shared_ptr: shared pointer
//  bsl::weak_ptr: "weak" reference to reference-counted shared object
//  bslstl::SharedPtrUtil: shared pointer utility functions
//  bslstl::SharedPtrNilDeleter: no-op deleter
//
//@AUTHOR: Ilougino Rocha (irocha)
//         Herve Bronnimann (hbronnim)
//         Vlad Kliatchko (vkliatch)
//         Raymond Chiu (schiu49)
//         Henry Verschell (hversche)
//         Rohan Bhindwale (rbhindwa)
//
//@SEE_ALSO: bslma_managedptr
//
//@DESCRIPTION: This component implements a thread-safe, generic,
// reference-counted "smart pointer" to support "shared ownership" of objects
// (of parameterized 'ELEMENT_TYPE').  Shared pointers implement a form of the
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
// the representation and the object are destroyed.  'shared_ptr' emulates the
// interface of a native pointer.  The shared object may be accessed directly
// using the '->' operator, or the dereference operator (operator '*') can be
// used to obtain a reference to the shared object.
//
///Thread-Safety
///-------------
// This section qualifies the thread-safety of 'shared_ptr' objects themselves
// rather than the thread-safety of the objects being referenced.
//
// It is safe to access or modify two distinct 'shared_ptr' objects
// simultaneously, each from a separate thread, even if they share ownership of
// a common object.  It is safe to access a single 'shared_ptr' object
// simultaneously from two or more separate threads, provided no other thread
// is simultaneously modifying the object.  It is not safe to access or modify
// a 'shared_ptr' object in one thread while another thread modifies the same
// object.
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
// Creating an in-place shared pointer does not require the parameterized type
// to inherit from a special class (such as 'bsl::enable_shared_from_this'); in
// that case, 'shared_ptr' supports up to fourteen arguments that can be passed
// directly to the object's constructor.  For in-place representations, both
// the object and the representation can be constructed in one allocation as
// opposed to two, effectively creating an "intrusive" reference counter.  Note
// that the size of the allocation is determined at compile-time from the
// combined footprint of the object and of the reference counter.  It is also
// possible to create shared pointers to buffers whose sizes are determined at
// runtime, although such buffers consist of raw (uninitialized) memory.
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
// 'deleterInstance(objectPtr)' is a valid expression, and a "factory" deleter
// is any language entity that can be invoked such that the expression
// 'deleterInstance.deleteObject(objectPtr)' is a valid expression, where
// 'deleterInstance' is an instance of the "deleter" object, and 'objectPtr' is
// a pointer to the shared object.  In summary:
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
// object that implements the 'bcema_Deleter' protocol.  Thus, any of these
// objects can be used as a factory deleter.  The purpose of this design is to
// allow 'bslma' allocators and factories to be used seamlessly as deleters.
//
// The selection of which expression is used by 'shared_ptr' to destroy a
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
///Comparison of Shared Pointers and Conversion to 'bool'
///------------------------------------------------------
// This component does *not* allow comparisons of shared pointers, although it
// does allow a conversion to an "unspecified bool type" (which is *not*
// 'bool') so as to retain the "comparison to 0" idiom:
//..
//  bslma::NewDeleteAllocator nda;
//  shared_ptr<int> sp1(new (nda) int(), &nda);
//  if (sp1) {   // OK
//      // . . .
//  }
//..
// The following attempts to compare two shared pointers:
//..
//  shared_ptr<double> sp2(new (nda) int(), &nda);
//  if (sp1 < sp2) {  // ERROR
//      // . . .
//  }
//..
// will both produce compilation errors, as will:
//..
//  bsl::map<shared_ptr<int>, int> sharedPtrMap;  // ERROR
//..
// (To be precise, the declaration of the map will not trigger an error, but
// the instantiation of any method that uses 'std::less' will.)
//
// In order to index a map by a shared pointer type, use the functor provided
// by the 'shared_ptrLess' class, as in:
//..
//  bsl::map<shared_ptr<int>, int, shared_ptrLess> sharedPtrMap;
//                                                                        // OK
//..
// 'shared_ptrLess' compares the pointer values referred to by the shared
// pointer keys in the map to sort the elements.
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
//  shared_ptr<B> spb(spa, static_cast<B *>(spa.ptr()));
//..
// or even the less safe C-style cast:
//..
//  shared_ptr<A> spa;
//  shared_ptr<B> spb(spa, (B *)(spa.ptr()));
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
///-----------------------------------------
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
///Storing a 'shared_ptr' in an Invokable in a 'bdef_Function' object
///-----------------------------------------------------------------------
// In addition to the guarantees already made in the 'bdef_function' component,
// 'bsl::shared_ptr' also guarantees that storing a shared pointer to an
// invokable object in a 'bdef_Function' object will be "in-place", i.e., it
// will not trigger memory allocation.
//
///C++ Standard Compliance
///-----------------------
// This component provides a (subset of a) standard-compliant implementation of
// 'std::shared_ptr' (sections 2.2.3 of the final "Technical Report on C++
// Library Extensions" and 20.9.12.2 of the current "Final Committee Draft,
// Standard for Programming Language C++").  Support for 'std::weak_ptr' is
// provided in the 'weak_ptr' component.  Also, this component allows
// conversion or assignment from 'std::auto_ptr' and provides most of the
// interface of 'std::shared_ptr'.  In addition, it does not collaborate with
// types that derive from 'std::enable_shared_this': using 'shared_ptr' with
// such types will result in (very destructive) undefined behavior.  The only
// global free function supplied with this component is 'swap'.
//
// As mentioned in the section "Comparison of Shared Pointers" above, using
// comparison operators 'operator<', 'operator<=', 'operator>', and
// 'operator>=', and the corresponding specializations for 'std::less',
// 'std::less_equal', 'bsl::greater', and 'bsl::greater_equal' with shared
// pointers, will cause a compilation error.  And there is a comparison functor
// in 'bslstl::SharedPtrUtil' for use in place of 'std::less' in 'bsl::map' and
// other standard associative containers.
//
// Finally, this component supports allocators following the 'bslma::Allocator'
// protocol and not the C++ Standard Allocators (section 20.1.2).
//
// CAVEAT: This implementation follows the BDE conventions on passing 0 as a
// 'bslma::Allocator *' argument.  A user who wishes to remain compliant with
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
///Example 2 - Nil deleters
///  -  -  -  -  -  -  -  -
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
//  MyUser *MyTransactionManager::systemUser(bslma::Allocator *basicAllocator)
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
///Example 3 - Custom Deleters
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
//      bcemt_Mutex      d_mutex;
//      bcemt_Condition  d_cond;
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
//      bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
//      d_queue.push_back(obj);
//      d_cond.signal();
//  }
//
//  template <class ELEMENT_TYPE>
//  ELEMENT_TYPE my_SafeQueue<ELEMENT_TYPE>::pop()
//  {
//      bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
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
// 'shared_ptr' refers to the parameterized type on which it is instantiated
// "in name only".  This allows for the instantiation of shared pointers to
// incomplete or 'void' types.  This feature is useful for constructing
// interfaces where returning a pointer to a shared object is desirable, but in
// order to control access to the object its interface cannot be exposed.  The
// following examples demonstrate two techniques for achieving this goal using
// a 'shared_ptr'.
//
///Example 4 - Hidden Interfaces
/// -  -  -  -  -  -  -  -  -  -
// Example 4 demonstrates the use of incomplete types to hide the interface of
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
//      bcemt_Mutex       d_mutex;
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
//      typedef bsl::map<int, shared_ptr<void> > HandleMap;
//
//      // DATA
//      bcemt_Mutex       d_mutex;
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
//      bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
//..
// Notice that 'my_Handle', which is a shared pointer to 'void', can be
// transparently assigned to a shared pointer to a 'my_Session' object.  This
// is because the 'shared_ptr' interface allows shared pointers to types that
// can be cast to one another to be assigned directly.
//..
//      my_Handle session(new(*d_allocator_p) my_Session(sessionName,
//                                                       d_nextSessionId++,
//                                                       d_allocator_p));
//      shared_ptr<my_Session> myhandle =
//                      bslstl::SharedPtrUtil::staticCast<my_Session>(session);
//      d_handles[myhandle->handleId()] = session;
//      return session;
//  }
//
//  inline
//  void my_SessionManager::closeSession(my_Handle handle)
//  {
//      bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
//..
// Perform a static cast from 'shared_ptr<void>' to 'shared_ptr<my_Session>'.
//..
//      shared_ptr<my_Session> myhandle =
//                       bslstl::SharedPtrUtil::staticCast<my_Session>(handle);
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
//      shared_ptr<my_Session> myhandle =
//                       bslstl::SharedPtrUtil::staticCast<my_Session>(handle);
//
//      if (!myhandle.ptr()) {
//          return bsl::string();
//      } else {
//          return myhandle->sessionName();
//      }
//  }
//..
//
//   AND NOW WE POST A STRAIGHT COPY OF THE WEAK_PTR DOCUMENTATION
//   THAT NEEDS TO BE PROPERLY INTEGRATED, INCLUDING A RENUMBERING
//   OF THE USAGE EXAMPLES.
//
//
//@DESCRIPTION: This component provides a mechanism, 'bcema_WeakPtr', used to
// create weak references to reference-counted shared ('bcema_SharedPtr')
// objects.  A weak reference provides conditional access to a shared object
// managed by a 'bcema_SharedPtr', but, unlike a shared (or "strong")
// reference, does not affect the shared object's lifetime.  An object having
// even one shared reference to it will not be destroyed, but an object having
// only weak references would have been destroyed when the last shared
// reference was released.
//
// A weak pointer can be constructed from another weak pointer or a
// 'bcema_SharedPtr'.  To access the shared object referenced by a weak pointer
// clients must first obtain a shared pointer to that object using the
// 'acquireSharedPtr' method.  If the shared object has been destroyed (as
// indicated by the 'expired' method), then 'acquireSharedPtr' returns a shared
// pointer in the default constructed (empty) state.
//
///Thread-Safety
///-------------
// This section qualifies the thread-safety of 'bcema_WeakPtr' objects
// themselves rather than the thread-safety of the objects being referenced.
//
// It is safe to access or modify two distinct 'bcema_WeakPtr' objects
// simultaneously, each from a separate thread, even if they reference the same
// shared object.  It is safe to access a single 'bcema_WeakPtr' object
// simultaneously from two or more threads, provided no other thread is
// simultaneously modifying the object.  It is not safe to access or modify a
// 'bcema_WeakPtr' object in one thread while another thread modifies the same
// object.
//
// It is safe to access, modify, copy, or delete a weak pointer in one thread,
// while other threads access or modify other weak pointers that reference the
// same object.  However, there is no guarantee regarding the safety of
// accessing or modifying the object *referenced* by the weak pointer
// simultaneously from multiple threads (that depends on the thread-safety
// guarantees of the shared object itself).
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
// Due to this behavior the memory footprint of shared objects that are
// constructed "in-place" in the shared pointer representation (refer to the
// component-level documentation of 'bcema_sharedptr' for more information on
// shared pointers with "in-place" representations) is not deallocated until
// all weak references to that shared object are released.  Note that a
// shared object is always destroyed when the last shared reference to it is
// released.  Also note that the same behavior is applicable if the shared
// objects were obtained from a class that pools shared pointer representations
// (for example, 'bcec_SharedObjectPool').
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
// will be weak references to it, then it may be advisable to create an
// out-of-place shared pointer representation, which destroys the shared object
// and deallocates its footprint when the last *shared* reference to it is
// released, regardless of whether there are any outstanding weak references to
// the same representation.
//
///Usage
///-----
// The following examples demonstrate various features and uses of weak
// pointers.
//
///Example 1 - Basic Usage
///- - - - - - - - - - - -
// This example illustrates the basic syntax needed to create and use a
// 'bcema_WeakPtr'.  Suppose that we want to construct a weak pointer that
// refers to an 'int' managed by a shared pointer.  Next we define the shared
// pointer and assign a value to the shared 'int':
//..
//  bcema_SharedPtr<int> intPtr;
//  intPtr.createInplace(bslma::Default::allocator());
//  *intPtr = 10;
//  assert(10 == *intPtr);
//..
// Next we construct a weak pointer to the 'int':
//..
//  bcema_WeakPtr<int> intWeakPtr(intPtr);
//  assert(!intWeakPtr.expired());
//..
// 'bcema_WeakPtr' does not provide direct access to the shared object being
// referenced.  To access and manipulate the 'int' from the weak pointer, we
// have to obtain a shared pointer from it:
//..
//  bcema_SharedPtr<int> intPtr2 = intWeakPtr.acquireSharedPtr();
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
//  bcema_WeakPtr<int> intWeakPtr2(intPtr);
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
//  assert(!intWeakPtr2.acquireSharedPtr());
//..
///Example 2 - Breaking Cyclic Dependencies
///- - - - - - - - - - - - - - - - - - - -
// Weak pointers are frequently used to break cyclic dependencies among objects
// that store references to each other via shared pointers.  Consider, for
// example, a simplified broadcasting system that sends news alerts to users
// based on user-selected keywords.  The user information is stored in the
// 'NewsUser' class and the details of the news alert are stored in the
// 'NewsAlert' class.  The class definitions for 'NewsUser' and 'NewsAlert' are
// provided below (with any code not essential to this example elided):
//..
//  class NewsAlert;
//
//  class NewsUser {
//      // This class stores the user information required for listening to
//      // alerts.
//
//      // DATA
//      bsl::vector<bcema_SharedPtr<NewsAlert> > d_alerts; // list of alerts
//                                                         // for which a user
//                                                         // is registered
//
//    public:
//      // ...
//
//      // MANIPULATORS
//      void addAlert(const bcema_SharedPtr<NewsAlert>& alertPtr)
//          // Add the specified 'alertPtr' to the list of alerts being
//          // monitored by this user.
//      {
//          d_alerts.push_back(alertPtr);
//      }
//
//      // ...
//  };
//..
// Now we define the alert class, 'NewsAlert':
//..
//  class NewsAlert {
//      // This class stores the alert information required for sending
//      // alerts.
//
//      // DATA
//      bsl::vector<bcema_SharedPtr<NewsUser> > d_users;  // list of users
//                                                        // who have
//                                                        // registered for
//                                                        // this alert
//
//    public:
//      // ...
//
//      // MANIPULATORS
//      void addUser(const bcema_SharedPtr<NewsUser>& userPtr)
//          // Add the specified 'userPtr' to the list of users monitoring this
//          // alert.
//      {
//          d_users.push_back(userPtr);
//      }
//
//      // ...
//  };
//..
// Note that the 'NewsUser' and 'NewsAlert' classes, as currently defined could
// easily result in a memory leak under typical use due to a cyclic reference:
//..
//  bslma::TestAllocator ta;
//  {
//      bcema_SharedPtr<NewsUser> userPtr;
//      userPtr.createInplace(&ta);
//
//      bcema_SharedPtr<NewsAlert> alertPtr;
//      alertPtr.createInplace(&ta);
//
//      alertPtr->addUser(userPtr);
//      userPtr->addAlert(alertPtr);
//
//      alertPtr.reset();
//      userPtr.reset();
//  }
//
//  // MEMORY LEAK !!
//..
// Even though we have released 'alertPtr' and 'userPtr', there still exists a
// cyclic reference between the two objects, so none of the objects are
// destroyed.
//
// We can break this cyclic dependency by modifying the alert class,
// 'NewsAlert', to store a weak pointer, instead of a shared pointer, to a
// 'NewsUser' object.  Below is the modified definition for the 'NewsAlert'
// class:
//..
//  class NewsAlert {
//      // This class stores the alert information required for sending
//      // alerts.
//
//..
// Note that the user is stored by a weak pointer instead of by a shared
// pointer:
//..
//      // DATA
//      bsl::vector<bcema_WeakPtr<NewsUser> > d_users; // list of users that
//                                                     // have registered for
//                                                     // this alert
//
//    public:
//      // MANIPULATORS
//      void addUser(const bcema_WeakPtr<NewsUser>& userPtr)
//          // Add the specified 'userPtr' to the list of users monitoring this
//          // alert.
//      {
//          d_users.push_back(userPtr);
//      }
//
//      // ...
//  };
//..
// The use of 'NewsUser' and 'NewsAlert' objects no longer causes a memory
// leak:
//..
//  bslma::TestAllocator ta;
//  {
//      bcema_SharedPtr<NewsAlert> alertPtr;
//      alertPtr.createInplace(&ta);
//
//      bcema_SharedPtr<NewsUser> userPtr;
//      userPtr.createInplace(&ta);
//
//      bcema_WeakPtr<NewsUser> userWeakPtr(userPtr);
//
//      alertPtr->addUser(userWeakPtr);
//      userPtr->addAlert(alertPtr);
//
//      alertPtr.reset();
//      userPtr.reset();
//  }
//
//  // No memory leak now.
//..
// Now both the user and alert objects are correctly destroyed, and the memory
// returned to the allocator after 'alertPtr' and 'userPtr' are reset.
//
///Example 3 - Caching
///- - - - - - - - - -
// Suppose we want to implement a peer-to-peer file-sharing system that allows
// users to search for files that match certain keywords.  A peer-to-peer
// file-sharing system connects remotely to similar file-sharing systems
// (peers) and determines which files stored by those peers best match the
// keywords specified by the user.
//
// Typically, such a system would connect to hundreds of peers to increase the
// probability of finding files that match any keyword.  When a user initiates
// a request with certain keywords the system sends the search request to
// connected peers to find out which one of them contains the files that best
// match the requested keywords.  But it is inefficient to send every file
// search request to all connected peers.  So the system may cache a subset of
// the connected peers, and based on their connection bandwidth, relevancy of
// previous search results, etc., send the search requests only to those cached
// peers.
//
// To show the implementation of such a system we will first define a 'Peer'
// class that stores the information about a peer (with code not relevant to
// the usage example elided):
//..
//  class Peer {
//      // This class stores relevant information for a peer.
//
//      // ...
//  };
//..
// We will now define a 'PeerManager' class that maintains the information
// about all connected 'Peer's.  The 'PeerManager' class creates a 'Peer'
// object when a new connection occurs, manages the lifetime of that object,
// and eventually destroys it when the peer disconnects.  In addition, the
// 'PeerManager' class needs to pass 'Peer' objects to other objects that
// need to communicate with the 'Peer'.  Therefore, 'PeerManager' stores
// a 'bcema_SharedPtr' to each 'Peer' object.  The following would be a simple
// definition for the 'PeerManager' classes:
//..
//  class PeerManager {
//      // This class acts as a manager of peers and adds and removes peers
//      // based on peer requests and disconnections.
//
//      // DATA
//      bsl::map<int, bcema_SharedPtr<Peer> > d_peers; // list of all
//                                                     // connected peers
//
//      // ...
//  };
//..
// We now define a 'PeerCache' class that stores a subset of the peers that are
// the primary recipients of search requests.  The 'PeerCache' stores a 'Peer'
// object via a 'bcema_WeakPtr' (as opposed to via a 'bcema_SharedPtr').  This
// allows the cache to store a reference to a 'Peer' object without affecting
// its lifetime and allowing it to be destroyed asynchronously on disconnection
// by that 'Peer'.  For brevity, the population and flushing of this cache is
// not shown:
//..
//  class PeerCache {
//      // This class caches a subset of all peers that match certain criteria.
//
//      // DATA
//      bsl::list<bcema_WeakPtr<Peer> > d_cachedPeers;  // list of cached peers
//
//    public:
//      // TYPES
//      typedef bsl::list<bcema_WeakPtr<Peer> >::const_iterator PeerConstIter;
//
//      // ...
//
//      // ACCESSORS
//      PeerConstIter begin() const
//          // Return an iterator that refers to the first peer in the list of
//          // peers stored by this peer cache.
//      {
//          return d_cachedPeers.begin();
//      }
//
//      PeerConstIter end() const
//          // Return an iterator that indicates the element one past the
//          // last peer in the list of peers stored by this peer cache.
//      {
//          return d_cachedPeers.end();
//      }
//  };
//..
// We now need to store the information representing a search result that is
// returned to the user and allows the user to decide which search result to
// select for downloading.  A search result would contain a 'Peer' object and
// the filename stored by that 'Peer' that best matches the specified keywords.
// For simplicity we store the 'Peer' information and a 'bsl::string' storing
// the filename as a 'bsl::pair'.  Again, note that because we do not want
// search result objects to control the lifetime of 'Peer' objects we use a
// 'bcema_WeakPtr' to 'Peer':
//..
//  typedef bsl::pair<bcema_WeakPtr<Peer>, bsl::string> SearchResult;
//      // 'SearchResult' is an alias for a search result.
//..
// Finally, we provide a search function that retrieves the search results for
// a set of keywords by querying all peers in the 'PeerCache'.  The
// implementation of the search function is provided below:
//..
//  PeerCache cachedPeers;
//
//  void search(bsl::vector<SearchResult>       *results,
//              const bsl::vector<bsl::string>&  keywords)
//  {
//      for (PeerConstIter iter = cachedPeers.begin();
//           iter != cachedPeers.end();
//           ++iter) {
//..
// In the following, we check if the peer is still connected by acquiring a
// shared pointer to the peer.  If the acquire operation succeeds, then we can
// send the peer a request to return the filename best matching the specified
// keywords:
//..
//          bcema_SharedPtr<Peer> peerSharedPtr = iter->acquireSharedPtr();
//          if (peerSharedPtr) {
//
//              // Search the peer for the file best matching the specified
//              // keywords and, if a file is found, append the returned
//              // 'SearchResult' object to 'result'.
//
//              // ...
//          }
//      }
//  }
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_memory.h> instead of <bslstl_sharedptr.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_PAIR
#include <bslstl_pair.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
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

#ifndef INCLUDED_BSLMA_MANAGEDPTR
#include <bslma_managedptr.h>
#endif

#ifndef INCLUDED_BSLMA_SHAREDPTRINPLACEREP
#include <bslma_sharedptrinplacerep.h>
#endif

#ifndef INCLUDED_BSLMA_SHAREDPTROUTOFPLACEREP
#include <bslma_sharedptroutofplacerep.h>
#endif

#ifndef INCLUDED_BSLMA_SHAREDPTRREP
#include <bslma_sharedptrrep.h>
#endif

#ifndef INCLUDED_BSLMF_ADDLVALUEREFERENCE
#include <bslmf_addlvaluereference.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_NULLPTR
#include <bsls_nullptr.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_UNSPECIFIEDBOOL
#include <bsls_unspecifiedbool.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>               // 'std::size_t', 'std::ptrdiff_t'
#define INCLUDED_CSTDDEF
#endif

#ifndef INCLUDED_FUNCTIONAL
#include <functional>            // 'std::less'
#define INCLUDED_FUNCTIONAL
#endif

#ifndef INCLUDED_MEMORY
#include <memory>                // 'std::auto_ptr'
#define INCLUDED_MEMORY
#endif

#ifndef INCLUDED_OSTREAM
#include <ostream>               // 'std::basic_ostream'
#define INCLUDED_OSTREAM
#endif

namespace bsl {

typedef native_std::size_t size_t;
typedef native_std::ptrdiff_t ptrdiff_t;

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

    // DATA
    ELEMENT_TYPE                     *d_ptr_p; // pointer to the shared object

    BloombergLP::bslma::SharedPtrRep *d_rep_p; // pointer to the representation
                                               // object that manages the
                                               // shared object

    // PRIVATE TYPES
    typedef shared_ptr<ELEMENT_TYPE> SelfType;
        // 'SelfType' is an alias to this 'class'.

    typedef typename BloombergLP::bsls::UnspecifiedBool<shared_ptr>::BoolType
                                                                      BoolType;

    // FRIENDS
    template <class COMPATIBLE_TYPE> friend class shared_ptr;

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
        // Return an out of place representation for a shared pointer managing
        // the specified 'ptr' and using the specified 'allocator' to provide
        // storage.

    template <class COMPATIBLE_TYPE, class DELETER>
    static BloombergLP::bslma::SharedPtrRep *makeInternalRep(
                                                   COMPATIBLE_TYPE *ptr,
                                                   DELETER         *deleter,
                                                   ...);
        // Return an out of place representation for a shared pointer managing
        // the specified 'ptr, that will destroy the object pointed to by the
        // specied 'ptr' using the specified 'deleter', using the currently
        // installed default allocator to provide storage.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    explicit shared_ptr(BloombergLP::bslma::SharedPtrRep *rep);
        // Create a shared pointer taking ownership of the specified 'rep' and
        // pointing to the object stored in the specified 'rep'.  The behavior
        // is undefined unless 'rep->originalPtr()' points to an object of type
        // 'ELEMENT_TYPE'.  Note that this method *DOES* *NOT* increment the
        // number of references to 'rep'.
        //
        // DEPRECATED This constructor will be made inaccessible in the next
        // BDE release, as the undefined behavior is too easily triggered and
        // offers no simple way to guard against misuse.  Instead, call the
        // constructor taking an additional 'TYPE *' initial argument:
        //..
        //  shared_ptr(TYPE *ptr, BloombergLP::bslma::SharedPtrRep *rep);
        //..
#endif

  public:
    // TYPES
    typedef ELEMENT_TYPE ElementType;
        // 'ElementType' is an alias to the 'ELEMENT_TYPE' template parameter.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(
                            shared_ptr,
                            BloombergLP::bslalg::TypeTraitHasPointerSemantics,
                            BloombergLP::bslalg::TypeTraitBitwiseMoveable);

    // CREATORS
    shared_ptr();
        // Create an empty shared pointer, i.e., a shared pointer with no
        // representation, that does not refer to any object and has no
        // deleter.

    template <class COMPATIBLE_TYPE>
    explicit shared_ptr(COMPATIBLE_TYPE *ptr);
        // Create a shared pointer that manages a modifiable object of
        // (template parameter) type 'COMPATIBLE_TYPE' and refers to
        // '(ELEMENT_TYPE *)ptr'.  The currently installed default allocator is
        // used to allocate and deallocate the internal representation of the
        // shared pointer and to destroy the shared object when all references
        // have been released.  If 'COMPATIBLE_TYPE *' is not implicitly
        // convertible to 'ELEMENT_TYPE *' then a compiler diagnostic will be
        // emitted indicating the error.  If 'ptr' is 0, an empty shared
        // pointer is created and no memory is allocated.  Note that as
        // mentioned in the "CAVEAT" in the "C++ Standard Compliance" section
        // of the component-level documentation, to comply with C++ standard
        // specifications, future implementations of 'shared_ptr' may destroy
        // the shared object using '::operator delete'.

    template <class COMPATIBLE_TYPE>
    shared_ptr(COMPATIBLE_TYPE               *ptr,
               BloombergLP::bslma::Allocator *basicAllocator);
        // Create a shared pointer that manages a modifiable object of
        // (template parameter) type 'COMPATIBLE_TYPE' and refers to
        // '(ELEMENT_TYPE *)ptr'.  If 'basicAllocator' is not 0 the specified
        // 'basicAllocator' used to allocate and deallocate the internal
        // representation of the shared pointer and to destroy the shared
        // object when all references have been released, otherwise the
        // currently installed default allocator is used.  If 'COMPATIBLE_TYPE
        // *' is not implicitly convertible to 'ELEMENT_TYPE *' then a compiler
        // diagnostic will be emitted indicating the error.  If 'ptr' is 0, an
        // empty shared pointer is created and 'basicAllocator' is ignored.
        // Note that as mentioned in the "CAVEAT" in the "C++ Standard
        // Compliance" section of the component-level documentation, to comply
        // with C++ standard specifications, future implementations of
        // 'shared_ptr' may destroy the shared object using '::operator delete'
        // if an allocator is not specified.  Also note that if
        // 'basicAllocator' is a pointer to a class derived from
        // 'bslma::Allocator', the compiler will actually select the following
        // (more general) constructor that has the same behavior:
        //..
        //  template <class COMPATIBLE_TYPE, class DELETER>
        //  shared_ptr(COMPATIBLE_TYPE *ptr, DELETER *const&  dispatch);
        //..

    shared_ptr(ELEMENT_TYPE *ptr, BloombergLP::bslma::SharedPtrRep *rep);
        // Create a shared pointer taking ownership of the specified 'rep' and
        // referring to the modifiable object at the specified 'ptr' address.
        // If 'ptr' is 0, an empty shared pointer is created.  Note that this
        // method *DOES* *NOT* increment the number of references to 'rep'.
        // Note that if 'rep' is a pointer to a class derived from
        // 'BloombergLP::bslma::SharedPtrRep', the compiler will actually
        // select the following (more general) constructor that has the same
        // behavior:
        //..
        //  template <class COMPATIBLE_TYPE, class DELETER>
        //  shared_ptr(COMPATIBLE_TYPE *ptr, DELETER *const&  dispatch);
        //..

    template <class COMPATIBLE_TYPE, class DELETER>
    shared_ptr(COMPATIBLE_TYPE *ptr, DELETER *const& deleter);
        // Create a shared pointer that manages a modifiable object the
        // (template parameter) type 'COMPATIBLE_TYPE' and refers to
        // '(ELEMENT_TYPE *)ptr', using the specified 'deleter' to delete the
        // shared object when all references have been released and the
        // currently installed default allocator to allocate and deallocate the
        // internal representation of the shared pointer, unless 'DELETER' is a
        // class derived from either 'bslma::allocator' or
        // 'BloombergLP::bslma::SharedPtrRep'; if 'DELETER' is a class derived
        // from 'bslma::allocator' create a shared pointer as if calling the
        // constructor:
        //..
        //  template <class COMPATIBLE_TYPE>
        //  shared_ptr(COMPATIBLE_TYPE               *ptr,
        //             BloombergLP::bslma::Allocator *basicAllocator);
        //..
        // If 'DELETER' is a class derived from
        // 'BloombergLP::bslma::SharedPtrRep' create a shared pointer as if
        // calling the constructor:
        //..
        //  shared_ptr(ELEMENT_TYPE                     *ptr,
        //             BloombergLP::bslma::SharedPtrRep *rep);
        //..
        // If 'DELETER' does not derive from either 'bslma::allocator' or
        // 'BloombergLP::bslma::SharedPtrRep', then 'deleter' is assumed to be
        // a pointer to a factory object that exposes a member function that
        // can be invoked as 'deleteObject(ptr)' that will be called to destroy
        // the object at the 'ptr' address (i.e., 'deleter->deleteObject(ptr)'
        // will be called to delete the shared object).  (See the "Deleters"
        // section in the component-level documentation.)  If 'COMPATIBLE_TYPE
        // *' is not implicitly convertible to 'ELEMENT_TYPE *' then a compiler
        // diagnostic will be emitted indicating the error.  Also note that if
        // 'ptr' is 0, an empty shared pointer is created and 'deleter' is
        // ignored.

    template <class COMPATIBLE_TYPE, class DELETER>
    shared_ptr(COMPATIBLE_TYPE               *ptr,
               const DELETER&                 deleter,
               BloombergLP::bslma::Allocator *basicAllocator = 0);
        // Create a shared pointer that manages a modifiable object of
        // (template parameter) type 'COMPATIBLE_TYPE' and refers to
        // '(ELEMENT_TYPE *)ptr', using the specified 'deleter' to delete the
        // shared object when all references have been released and the
        // specified 'basicAllocator' to allocate and deallocate the internal
        // representation of the shared pointer.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.  If 'DELETER' is a
        // reference type, then 'deleter' is assumed to be a function-like
        // deleter that may be invoked to destroy the object referred to by a
        // single argument of type 'COMPATIBLE_TYPE *' (i.e., 'deleter(ptr)'
        // will be called to destroy the shared object).  (See the "Deleters"
        // section in the component- level documentation.)  If 'COMPATIBLE_TYPE
        // *' is not implicitly convertible to 'ELEMENT_TYPE *' then a compiler
        // diagnostic will be emitted indicating the error.  Also note that if
        // 'ptr' is 0, an empty shared pointer is created and 'deleter' and
        // 'basicAllocator' are ignored.

    template <class DELETER>
    shared_ptr(nullptr_t                      nullPointerLiteral,
               const DELETER&                 deleter,
               BloombergLP::bslma::Allocator *basicAllocator = 0);
        // Create an empty shared pointer.  Note that for conformance with the
        // C++ Standard specification for 'shared_ptr', a future version of
        // this component may reference count the deleter, and uses the
        // specified 'basicAllocator' to create the storage area for the
        // reference counts and a copy of the specified 'deleter'.

    shared_ptr(nullptr_t                      nullPointerLiteral,
               BloombergLP::bslma::Allocator *basicAllocator);
        // Create an empty shared pointer, i.e., a shared pointer with no
        // representation, that does not refer to any object and has no
        // deleter.

    template <class COMPATIBLE_TYPE>
    shared_ptr(
          BloombergLP::bslma::ManagedPtr<COMPATIBLE_TYPE>  managedPtr,
          BloombergLP::bslma::Allocator                   *basicAllocator = 0);
                                                                    // IMPLICIT
        // Create a shared pointer that takes over the management of the
        // modifiable object (if any) previously managed by the specified
        // 'managedPtr' to the (template parameter) type 'COMPATIBLE_TYPE', and
        // that refers to '(ELEMENT_TYPE *)managedPtr.ptr()'.  The deleter used
        // in the 'managedPtr' will be used to destroy the shared object when
        // all references have been released.  Optionally specify a
        // 'basicAllocator' used to allocate and deallocate the internal
        // representation of the shared pointer.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.  If 'COMPATIBLE_TYPE
        // *' is not implicitly convertible to 'ELEMENT_TYPE *' then a compiler
        // diagnostic will be emitted indicating the error.  Note that if
        // 'managedPtr' is empty, then an empty shared pointer is created and
        // 'basicAllocator' is ignored.

    template <class COMPATIBLE_TYPE>
    explicit shared_ptr(std::auto_ptr<COMPATIBLE_TYPE>  autoPtr,
                        BloombergLP::bslma::Allocator  *basicAllocator = 0);
        // Create a shared pointer that takes over the management of the
        // modifiable object previously managed by the specified 'autoPtr' to
        // the (template parameter) type 'COMPATIBLE_TYPE', and that refers to
        // '(ELEMENT_TYPE *)autoPtr.get()'.  The global '::operator delete'
        // will be used to destroy the shared object when all references have
        // been released.  Optionally specify an 'basicAllocator' used to
        // allocate and deallocate the internal representation of the shared
        // pointer.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  If 'COMPATIBLE_TYPE *' is not implicitly
        // convertible to 'ELEMENT_TYPE *' then a compiler diagnostic will be
        // emitted indicating the error.

    template <class ANY_TYPE>
    shared_ptr(const shared_ptr<ANY_TYPE>&  source,
               ELEMENT_TYPE                *object);
        // Create a shared pointer that manages the same modifiable object (if
        // any) as the specified 'source' shared pointer to the (template
        // parameter) type 'ANY_TYPE', and that refers to the modifiable object
        // at the specified 'object' address.  The resulting shared pointer is
        // known as an "alias" of 'source'.  Note that typically the objects
        // referred to by 'source' and 'object' have identical lifetimes (e.g.,
        // one might be a part of the other) so that the deleter for 'source'
        // will destroy them both, but they do not necessarily have the same
        // type.  Also note that if either 'source' is empty or 'object' is
        // null, then an empty shared pointer is created.

    template <class COMPATIBLE_TYPE>
    shared_ptr(const shared_ptr<COMPATIBLE_TYPE>& other);
        // Create a shared pointer that manages the same modifiable object (if
        // any) as the specified 'other' shared pointer to the (template
        // parameter) type 'COMPATIBLE_TYPE', using the same deleter as 'other'
        // to destroy the shared object, and that refers to
        // '(ELEMENT_TYPE*)other.ptr()'.  If 'COMPATIBLE_TYPE *' is not
        // implicitly convertible to 'ELEMENT_TYPE *' then a compiler
        // diagnostic will be emitted indicating the error.  Note that if
        // 'other' is empty, then an empty shared pointer is created.

    shared_ptr(const shared_ptr& original);
        // Create a shared pointer that refers to and manages the same object
        // (if any) as the specified 'original' shared pointer, using the same
        // deleter as 'original' to destroy the shared object.  Note that if
        // 'original' is empty, then an empty shared pointer is created.

    ~shared_ptr();
        // Destroy this shared pointer.  If this shared pointer refers to a
        // (possibly shared) object, then release the reference to that object,
        // and destroy the shared object using its associated deleter if this
        // shared pointer is the last reference to that object.

    // MANIPULATORS
    shared_ptr& operator=(const shared_ptr& rhs);
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

    template <class COMPATIBLE_TYPE>
    shared_ptr& operator=(const shared_ptr<COMPATIBLE_TYPE>& rhs);
        // Make this shared pointer refer to and manage the same modifiable
        // object as the specified 'rhs' shared pointer to the (template
        // parameter) type 'COMPATIBLE_TYPE', using the same deleter as 'rhs'
        // and referring to '(ELEMENT_TYPE *)rhs.ptr()', and return a reference
        // to this modifiable shared pointer.  If this shared pointer is
        // already managing a (possibly shared) object, then release the shared
        // reference to that object, and destroy it using its associated
        // deleter if this shared pointer held the last shared reference to
        // that object.  Note that if 'rhs' is empty, then this shared pointer
        // will also be empty after the assignment.

    template <class COMPATIBLE_TYPE>
    shared_ptr& operator=(std::auto_ptr<COMPATIBLE_TYPE> rhs);
        // Transfer ownership to this shared pointer of the modifiable object
        // managed by the 'rhs' auto pointer to the (template parameter) type
        // 'COMPATIBLE_TYPE', using the 'delete' operator to destroy the shared
        // object when the last reference is released, and make this shared
        // pointer refer to '(ELEMENT_TYPE *)rhs.get()'.  If this shared
        // pointer is already managing a (possibly shared) object, then release
        // the reference to that shared object, and destroy it using it
        // associated deleter if this shared pointer held the last shared
        // reference to that object.  Note that if 'rhs' is null, then this
        // shared pointer will also be empty after the assignment.

    void clear();
        // Reset this shared pointer to the empty state.  If this shared
        // pointer is managing a (possibly shared) object, then release the
        // reference to the shared object, calling the deleter to destroy the
        // shared object if this pointer is the last reference.
        //
        // DEPRECATED: Use 'reset' instead.

    template <class COMPATIBLE_TYPE>
    void load(COMPATIBLE_TYPE               *ptr,
              BloombergLP::bslma::Allocator *basicAllocator = 0);
        // Modify this shared pointer to manage the modifiable object of the
        // (template parameter) type 'COMPATIBLE_TYPE' at the specified 'ptr'
        // address and refer to '(ELEMENT_TYPE *)ptr'.  If this shared pointer
        // is already managing a (possibly shared) object, then release the
        // shared reference to that shared object, and destroy it using its
        // associated deleter if this shared pointer held the last shared
        // reference to that object.  Optionally specify an 'basicAllocator'
        // used to allocate and deallocate the internal representation of this
        // shared pointer and to destroy the shared object when all references
        // have been released.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  If 'COMPATIBLE_TYPE *' is not
        // implicitly convertible to 'ELEMENT_TYPE *' then a compiler
        // diagnostic will be emitted indicating the error.  Note that if 'ptr'
        // is 0, then this shared pointer will be reset to the empty state and
        // 'basicAllocator' will be ignored.  Also note that as mentioned in
        // the "CAVEAT" in the "C++ Standard Compliance" section of the
        // component-level documentation, to comply with C++ standard
        // specifications, future implementations of 'shared_ptr' may destroy
        // the shared object using '::operator delete' if an allocator is not
        // specified.

    template <class COMPATIBLE_TYPE, class DELETER>
    void load(COMPATIBLE_TYPE  *ptr,
              const DELETER&    deleter,
              BloombergLP::bslma::Allocator *basicAllocator);
        // Modify this shared pointer to manage the modifiable object of the
        // (template parameter) type 'COMPATIBLE_TYPE' at the specified 'ptr'
        // address and to refer to '(ELEMENT_TYPE *)ptr', using the specified
        // 'deleter' to delete the shared object when all references have been
        // released and the specified 'basicAllocator' to allocate and
        // deallocate the internal representation of the shared pointer.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  If this shared pointer is already managing a (possibly
        // shared) object, then release the shared reference to that shared
        // object, and destroy it using its associated deleter if this shared
        // pointer held the last shared reference to that object.  If 'DELETER'
        // is a reference type, then 'deleter' is assumed to be a function-like
        // deleter that may be invoked to destroy the object referred to by a
        // single argument of type 'COMPATIBLE_TYPE *' (i.e., 'deleter(ptr)'
        // will be called to destroy the shared object).  If 'DELETER' is a
        // pointer type, then 'deleter' is assumed to be a pointer to a factory
        // object that exposes a member function that can be invoked as
        // 'deleteObject(ptr)' that will be called to destroy the object at the
        // 'ptr' address (i.e., 'deleter->deleteObject(ptr)' will be called to
        // delete the shared object).  (See the "Deleters" section in the
        // component-level documentation.)  If 'COMPATIBLE_TYPE *' is not
        // implicitly convertible to 'ELEMENT_TYPE *' then a compiler
        // diagnostic will be emitted indicating the error.  Note that, for
        // factory deleters, the specified 'deleter' must remain valid until
        // all references to 'ptr' have been released.  Also note that if 'ptr'
        // is 0, then this shared pointer is reset to the empty state and both
        // 'deleter' and 'basicAllocator' are ignored.  Also note that this
        // function is logically equivalent to:
        //..
        //  *this = shared_ptr<ELEMENT_TYPE>(ptr, deleter, allocator);
        //..
        // and that, for the same reasons as explained in the constructor, the
        // third 'basicAllocator' argument is not optional.

    template <class ANY_TYPE>
    void loadAlias(const shared_ptr<ANY_TYPE>&  source,
                   ELEMENT_TYPE                     *object);
        // Modify this shared pointer to manage the same modifiable object (if
        // any) as the specified 'source' shared pointer to the (template
        // parameter) type 'ANY_TYPE', and refer to the modifiable object at
        // the specified 'object' address (i.e., make this shared pointer an
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
        //  *this = shared_ptr<ELEMENT_TYPE>(source, object);
        //..

    void createInplace(BloombergLP::bslma::Allocator *basicAllocator = 0);
        // Create "in-place" in a large enough contiguous memory region both an
        // internal representation for this shared pointer and a
        // default-constructed object of 'ELEMENT_TYPE', and make this shared
        // pointer refer to the newly-created 'ELEMENT_TYPE' object.
        // Optionally specify an 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  If an exception is thrown during allocation or construction
        // of the 'ELEMENT_TYPE' object, this shared pointer will be unchanged.
        // Otherwise, if this shared pointer is already managing a (possibly
        // shared) object, then release the shared reference to that shared
        // object, and destroy it using its associated deleter if this shared
        // pointer held the last shared reference to that object.  Note that
        // the allocator argument is *not* passed to the constructor for
        // 'ELEMENT_TYPE'.  To construct an in-place 'ELEMENT_TYPE' with an
        // allocator, use one of the other variants of 'createInplace' below.

    template <class A1>
    void createInplace(BloombergLP::bslma::Allocator *basicAllocator,
                       const A1& a1);
    template <class A1, class A2>
    void createInplace(BloombergLP::bslma::Allocator *basicAllocator,
                       const A1& a1, const A2& a2);
    template <class A1, class A2, class A3>
    void createInplace(BloombergLP::bslma::Allocator *basicAllocator,
                       const A1& a1, const A2& a2, const A3& a3);
    template <class A1, class A2, class A3, class A4>
    void createInplace(BloombergLP::bslma::Allocator *basicAllocator,
                       const A1& a1, const A2& a2, const A3& a3, const A4& a4);
    template <class A1, class A2, class A3, class A4, class A5>
    void createInplace(BloombergLP::bslma::Allocator *basicAllocator,
                       const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5);
    template <class A1, class A2, class A3, class A4, class A5, class A6>
    void createInplace(BloombergLP::bslma::Allocator *basicAllocator,
                       const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7>
    void createInplace(BloombergLP::bslma::Allocator *basicAllocator,
                       const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8>
    void createInplace(BloombergLP::bslma::Allocator *basicAllocator,
                       const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7, const A8& a8);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9>
    void createInplace(BloombergLP::bslma::Allocator *basicAllocator,
                       const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                       const A9& a9);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10>
    void createInplace(BloombergLP::bslma::Allocator *basicAllocator,
                       const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                       const A9& a9, const A10& a10);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11>
    void createInplace(BloombergLP::bslma::Allocator *basicAllocator,
                       const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                       const A9& a9, const A10& a10, const A11& a11);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12>
    void createInplace(BloombergLP::bslma::Allocator *basicAllocator,
                       const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                       const A9& a9, const A10& a10, const A11& a11,
                       const A12& a12);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12,
              class A13>
    void createInplace(BloombergLP::bslma::Allocator *basicAllocator,
                       const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                       const A9& a9, const A10& a10, const A11& a11,
                       const A12& a12, const A13& a13);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12,
              class A13, class A14>
    void createInplace(BloombergLP::bslma::Allocator *basicAllocator,
                       const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                       const A9& a9, const A10& a10, const A11& a11,
                       const A12& a12, const A13& a13, const A14& a14);
        // Create "in-place" in a large enough contiguous memory region, using
        // the specified 'basicAllocator' to supply memory, both an internal
        // representation for this shared pointer and an object of
        // 'ELEMENT_TYPE' using the 'ELEMENT_TYPE' constructor that takes the
        // specified arguments 'a1' up to 'aN' where 'N' (at most 14) is the
        // number of arguments passed to this method, and make this shared
        // pointer refer to the newly-created 'ELEMENT_TYPE' object.  If an
        // exception is thrown during the construction of the 'ELEMENT_TYPE'
        // object, this shared pointer will be unchanged.  Otherwise, if this
        // shared pointer is already managing a (possibly shared) object, then
        // release the shared reference to that shared object, and destroy it
        // using its associated deleter if this shared pointer held the last
        // shared reference to that object.  Note that the allocator argument
        // is *not* implicitly passed to the constructor for 'ELEMENT_TYPE'.
        // To construct an object of 'ELEMENT_TYPE' with an allocator, pass the
        // allocator as one of the arguments (typically the last argument).

    pair<ELEMENT_TYPE *, BloombergLP::bslma::SharedPtrRep *> release();
        // Return the pair consisting of the addresses of the modifiable
        // 'ELEMENT_TYPE' object referred to, and the representation shared by,
        // this shared pointer, and reset this shared pointer to the empty
        // state with no effect on the representation.  The reference counter
        // is not modified nor is the shared object deleted; if the reference
        // count of this representation is greater than one, then it is not
        // safe to release the representation (thereby destroying the shared
        // object), but it is always safe to create another shared pointer with
        // this representation using the constructor with the signature
        // 'shared_ptr(ELEMENT_TYPE                     *ptr,
        //             BloombergLP::bslma::SharedPtrRep *rep)'.
        // Also note that this function returns a pair of null pointers if this
        // shared pointer is empty.

    void swap(shared_ptr<ELEMENT_TYPE>& other);
        // Efficiently exchange the states of this shared pointer and the
        // specified 'other' shared pointer such that each will refer to the
        // object formerly referred to by the other and each will manage the
        // object formerly referred to by the other.

    // ACCESSORS
    operator BoolType() const;
        // Return a value of an "unspecified bool" type that evaluates to
        // 'false' if this shared pointer is in the empty state, and 'true'
        // otherwise.  Note that this conversion operator allows a shared
        // pointer to be used within a conditional context (e.g., within an
        // 'if' or 'while' statement), but does *not* allow shared pointers to
        // be compared (e.g., via '<' or '>').

    typename add_lvalue_reference<ELEMENT_TYPE>::type
    operator[](ptrdiff_t index) const;
        // Return a reference to the modifiable object at the specified 'index'
        // offset in object referred to by this shared pointer.  The behavior
        // is undefined unless this shared pointer is not empty, 'ELEMENT_TYPE'
        // is not 'void' (a compiler error will be generated if this operator
        // is instantiated within the 'shared_ptr<void>' class), and this
        // shared pointer refers to an array of 'ELEMENT_TYPE' objects.  Note
        // that this is logically equivalent to '*(ptr() + index)'.

    typename add_lvalue_reference<ELEMENT_TYPE>::type
    operator*() const;
        // Return a reference to the modifiable object referred to by this
        // shared pointer.  The behavior is undefined unless this shared
        // pointer is not empty, and 'ELEMENT_TYPE' is not 'void' (a compiler
        // error will be generated if this operator is instantiated within the
        // 'shared_ptr<void>' class).

    ELEMENT_TYPE *operator->() const;
        // Return the address of the modifiable object referred to by this
        // shared pointer, or 0 if this shared pointer is empty.  Note that
        // applying this operator conventionally (e.g., to invoke a method) to
        // an empty shared pointer will result in undefined behavior.

    ELEMENT_TYPE *ptr() const;
        // Return the address of the modifiable object referred to by this
        // shared pointer, or 0 if this shared pointer is empty.

    BloombergLP::bslma::SharedPtrRep *rep() const;
        // Return the address of the modifiable
        // 'BloombergLP::bslma::SharedPtrRep' object used by this shared
        // pointer, or 0 if this shared pointer is empty.

    int numReferences() const;
        // Return a "snapshot" of the number of shared pointers (including this
        // one) that share ownership of the object referred to by this shared
        // pointer.

    BloombergLP::bslma::ManagedPtr<ELEMENT_TYPE> managedPtr() const;
        // Return a managed pointer that refers to the same object as this
        // shared pointer and which has a deleter that decrements the reference
        // count for the shared object.

                        // *** std::tr1 COMPATIBILITY ***

    // TYPES
    typedef ELEMENT_TYPE element_type;
        // 'element_type' is an alias to the 'ELEMENT_TYPE' template parameter.
        // Note that 'element_type' refers to the same type as 'ElementType'.

    // MANIPULATORS
    void reset();
        // Reset this shared pointer to the empty state.  If this shared
        // pointer is managing a (possibly shared) object, then release the
        // reference to the shared object, calling the deleter to destroy the
        // shared object if this pointer is the last reference.

    template <class COMPATIBLE_TYPE>
    void reset(COMPATIBLE_TYPE *ptr);
        // Modify this shared pointer to manage the modifiable object of the
        // (template parameter) type 'COMPATIBLE_TYPE' at the specified 'ptr'
        // address and to refer to '(ELEMENT_TYPE *)ptr'.  If this shared
        // pointer is already managing a (possibly shared) object, then release
        // the reference to the shared object, calling the deleter to destroy
        // the shared object if this pointer is the last reference.  The
        // currently installed default allocator is used to allocate the
        // internal representation of this shared pointer and to destroy the
        // shared object when all references have been released.  If
        // 'COMPATIBLE_TYPE *' is not implicitly convertible to
        // 'ELEMENT_TYPE *' then a compiler diagnostic will be emitted
        // indicating the error.  Note that if 'ptr' is 0, then this shared
        // pointer will be reset to the empty state.  Note that the behavior of
        // this method is the same as 'load(ptr)'.

    template <class COMPATIBLE_TYPE, class DELETER>
    void reset(COMPATIBLE_TYPE *ptr, const DELETER& deleter);
        // Modify this shared pointer to manage the modifiable object of the
        // (template parameter) type 'COMPATIBLE_TYPE' at the specified 'ptr'
        // address and refer to '(ELEMENT_TYPE *)ptr', using the specified
        // 'deleter' to delete the shared object when all references have been
        // released.  If this shared pointer is already managing a (possibly
        // shared) object, then release the reference to the shared object,
        // calling the deleter to destroy the shared object if this pointer is
        // the last reference.  If 'DELETER' is a reference type, then
        // 'deleter' is assumed to be a function-like deleter that may be
        // invoked to destroy the object referred to by a single argument of
        // type 'COMPATIBLE_TYPE *' (i.e., 'deleter(ptr)' will be called to
        // destroy the shared object).  If 'DELETER' is a pointer type, then
        // 'deleter' is assumed to be a pointer to a factory object that
        // exposes a member function that can be invoked as 'deleteObject(ptr)'
        // that will be called to destroy the object at the 'ptr' address
        // (i.e., 'deleter->deleteObject(ptr)' will be called to delete the
        // shared object).  (See the "Deleters" section in the component-level
        // documentation.)  The currently installed default allocator is used
        // to allocate the internal representation of this shared pointer and
        // to destroy the shared object when all references have been released.
        // If 'COMPATIBLE_TYPE *' is not implicitly convertible to
        // 'ELEMENT_TYPE *' then a compiler diagnostic will be emitted
        // indicating the error.  Note that, for factory deleters, 'deleter'
        // must remain valid until all references to 'ptr' have been released.
        // Also note that if 'ptr' is 0, then this shared pointer will be reset
        // to the empty state and 'deleter' is ignored.  The behavior of this
        // method is the same as 'load(ptr, deleter, (bslma::Allocator *)0)'.

    template <class ANY_TYPE>
    void reset(const shared_ptr<ANY_TYPE>& source, ELEMENT_TYPE *ptr);
        // Modify this shared pointer to manage the same modifiable object (if
        // any) as the specified 'source' shared pointer to the (template
        // parameter) type 'ANY_TYPE', and refer to the modifiable object at
        // the specified 'ptr' address (i.e., make this shared pointer an
        // "alias" of 'source').  If this shared pointer is already managing a
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
    ELEMENT_TYPE *get() const;
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

    template<class ANY_TYPE>
    bool owner_before(const shared_ptr<ANY_TYPE>& other) const;
        // Return 'true' if the address of the
        // 'BloombergLP::bslma::SharedPtrRep' object used by this shared
        // pointer is ordered before the address of the
        // 'BloombergLP::bslma::SharedPtrRep' object used by the specified
        // 'other' shared pointer under the total ordering supplied by
        // 'std::less<BloombergLP::bslma::SharedPtrRep *>', and 'false'
        // otherwise.
};

// FREE OPERATORS
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const shared_ptr<LHS_TYPE>& lhs,
                const shared_ptr<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' shared pointer refers to the same
    // object (if any) as that being referred to by the specified 'rhs' shared
    // pointer (if any), and 'false' otherwise.  Note that if a (raw) pointer
    // to 'LHS_TYPE' cannot be compared to a (raw) pointer to 'RHS_TYPE', then
    // a compiler diagnostic will be emitted indicating the error.  Also note
    // that two equal shared pointers do not necessarily manage the same object
    // due to aliasing.

template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const shared_ptr<LHS_TYPE>& lhs,
                const shared_ptr<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' shared pointer does not refer to
    // the same object (if any) as that being referred to by the specified
    // 'rhs' shared pointer (if any), and 'false' otherwise.  Note that if a
    // (raw) pointer to 'LHS_TYPE' cannot be compared to a (raw) pointer to
    // 'RHS_TYPE', then a compiler diagnostic will be emitted indicating the
    // error.  Also note that two equal shared pointers do not necessarily
    // manage the same object due to aliasing.

template<class LHS_TYPE, class RHS_TYPE>
bool operator<(const shared_ptr<LHS_TYPE>& lhs,
               const shared_ptr<RHS_TYPE>& rhs);
    // Return 'true' if the address of the object referenced by the specified
    // 'lhs' is ordered before the address of the object referenced by the
    // specified 'rhs' under the total ordering supplied by 'std::less<T *>',
    // where 'T *' is the composite pointer type of 'LHS_TYPE *' and
    // 'RHS_TYPE *', and 'false' otherwise.

template<class LHS_TYPE, class RHS_TYPE>
bool operator>(const shared_ptr<LHS_TYPE>& lhs,
               const shared_ptr<RHS_TYPE>& rhs);
    // Return 'true' if the address of the object referenced by the specified
    // 'lhs' is ordered after the address of the object referenced by the
    // specified 'rhs' under the total ordering supplied by 'std::less<T *>',
    // where 'T *' is the composite pointer type of 'LHS_TYPE *' and
    // 'RHS_TYPE *', and 'false' otherwise.

template<class LHS_TYPE, class RHS_TYPE>
bool operator<=(const shared_ptr<LHS_TYPE>& lhs,
                const shared_ptr<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' refers to the same object as the
    // specified 'rhs', or if the address of the object referenced by 'lhs' is
    // ordered before the address of the object referenced by 'rhs' under the
    // total ordering supplied by 'std::less<T *>', where 'T *' is the
    // composite pointer type of 'LHS_TYPE *' and 'RHS_TYPE *', and 'false'
    // otherwise.

template<class LHS_TYPE, class RHS_TYPE>
bool operator>=(const shared_ptr<LHS_TYPE>& lhs,
                const shared_ptr<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' refers to the same object as the
    // specified 'rhs', or if the address of the object referenced by 'lhs' is
    // ordered after the address of the object referenced by 'rhs' under the
    // total ordering supplied by 'std::less<T *>', where 'T *' is the
    // composite pointer type of 'LHS_TYPE *' and 'RHS_TYPE *', and 'false'
    // otherwise.

template <class LHS_TYPE>
bool operator==(const shared_ptr<LHS_TYPE>& lhs, nullptr_t);
    // Return 'true' if the specified 'lhs' shared pointer is empty, and
    // 'false' otherwise.

template <class RHS_TYPE>
bool operator==(nullptr_t, const shared_ptr<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'rhs' shared pointer is empty, and
    // 'false' otherwise.

template <class LHS_TYPE>
bool operator!=(const shared_ptr<LHS_TYPE>& lhs, nullptr_t);
    // Return 'true' if the specified 'lhs' shared pointer is not empty, and
    // 'false' otherwise.

template <class RHS_TYPE>
bool operator!=(nullptr_t, const shared_ptr<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'rhs' shared pointer is not empty, and
    // 'false' otherwise.

template <class LHS_TYPE>
bool operator<(const shared_ptr<LHS_TYPE>& lhs, nullptr_t);
    // Return 'true' if the address of the object referenced by the specified
    // 'lhs' is ordered before the null-pointer value under the total ordering
    // supplied by 'std::less<LHS_TYPE *>', and 'false' otherwise.

template <class RHS_TYPE>
bool operator<(nullptr_t, const shared_ptr<RHS_TYPE>& rhs);
    // Return 'true' if the address of the object referenced by the specified
    // 'rhs' is ordered after the null-pointer value under the total ordering
    // supplied by 'std::less<RHS_TYPE *>', and 'false' otherwise.

template <class LHS_TYPE>
bool operator<=(const shared_ptr<LHS_TYPE>& lhs, nullptr_t);
    // Return 'true' if the specified 'lhs' is empty, or if the address of the
    // object referenced by 'lhs' is ordered before the null-pointer value
    // under the total ordering supplied by 'std::less<LHS_TYPE *>', and
    // 'false' otherwise.

template <class RHS_TYPE>
bool operator<=(nullptr_t, const shared_ptr<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'rhs' is empty, or if the address of the
    // object referenced by 'rhs' is ordered after the null-pointer value under
    // the total ordering supplied by 'std::less<RHS_TYPE *>', and 'false'
    // otherwise.

template <class LHS_TYPE>
bool operator>(const shared_ptr<LHS_TYPE>& lhs, nullptr_t);
    // Return 'true' if the address of the object referenced by the specified
    // 'lhs' is ordered after the null-pointer value under the total ordering
    // supplied by 'std::less<LHS_TYPE *>', and 'false' otherwise.

template <class RHS_TYPE>
bool operator>(nullptr_t, const shared_ptr<RHS_TYPE>& rhs);
    // Return 'true' if the address of the object referenced by the specified
    // 'rhs' is ordered before the null-pointer value under the total ordering
    // supplied by 'std::less<RHS_TYPE *>', and 'false' otherwise.

template <class LHS_TYPE>
bool operator>=(const shared_ptr<LHS_TYPE>& lhs, nullptr_t);
    // Return 'true' if the specified 'lhs' is empty, or if the address of the
    // object referenced by 'lhs' is ordered after the null-pointer value under
    // the total ordering supplied by 'std::less<LHS_TYPE *>', and 'false'
    // otherwise.

template <class RHS_TYPE>
bool operator>=(nullptr_t, const shared_ptr<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'rhs' is empty, or if the address of the
    // object referenced by 'rhs' is ordered before the null-pointer value
    // under the total ordering supplied by 'std::less<RHS_TYPE *>', and
    // 'false' otherwise.

template<class CHAR_TYPE, class CHAR_TRAITS, class ELEMENT_TYPE>
std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>&
operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>& stream,
           const shared_ptr<ELEMENT_TYPE>&             rhs);
    // Print to the specified 'stream' the address of the shared object
    // referred to by the specified 'rhs' shared pointer and return a reference
    // to the modifiable 'stream'.

                        // *** std::tr1 COMPATIBILITY ***

template <class ELEMENT_TYPE>
void swap(shared_ptr<ELEMENT_TYPE>& a, shared_ptr<ELEMENT_TYPE>& b);
    // Efficiently exchange the states of the specified 'a' and 'b' shared
    // pointers such that each will refer to the object formerly referred to by
    // the other, and each will manage the object formerly referred to by the
    // other.

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
    ELEMENT_TYPE                     *d_ptr_p; // pointer to the shared object
                                               // (held, not owned)

    BloombergLP::bslma::SharedPtrRep *d_rep_p; // pointer to the representation
                                               // object that manages the
                                               // shared object (held, not
                                               // owned)

    // FRIENDS
    template <class COMPATIBLE_TYPE> friend class weak_ptr;
        // This 'friend' declaration provides access to the internal data
        // members while constructing a weak pointer from a weak pointer of a
        // different type.

  public:
    // TYPES
    typedef ELEMENT_TYPE ElementType;
        // 'ElementType' is an alias for the 'TYPE' parameter of this class
        // template.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                                weak_ptr,
                                BloombergLP::bslalg::TypeTraitBitwiseMoveable);

    // CREATORS
    weak_ptr();
        // Create a weak pointer in the empty state, i.e., a weak pointer
        // having no representation that does not refer to any object.

    weak_ptr(const weak_ptr<ELEMENT_TYPE>& original);
        // Create a weak pointer that refers to the same object (if any) as the
        // specified 'original' weak pointer, and increment the number of weak
        // references to that object (if any).  Note that if 'original' is in
        // the empty state, this weak pointer will be initialized to the empty
        // state.

    template <class COMPATIBLE_TYPE>
    weak_ptr(const shared_ptr<COMPATIBLE_TYPE>& other);             // IMPLICIT
    template <class COMPATIBLE_TYPE>
    weak_ptr(const weak_ptr<COMPATIBLE_TYPE>& other);               // IMPLICIT
        // Create a weak pointer that refers to the same object (if any) as the
        // specified 'other' (shared or weak) pointer of the parameterized
        // 'COMPATIBLE_TYPE', and increment the number of weak references to
        // that object (if any).  If 'COMPATIBLE_TYPE *' is not implicitly
        // convertible to 'TYPE *', then a compiler diagnostic will be emitted.
        // Note that if 'other' is in the empty state, this weak pointer will
        // be initialized to the empty state.

    ~weak_ptr();
        // Destroy this weak pointer object.  If this weak pointer refers to a
        // (possibly shared) object, release the weak reference to that object.

    // MANIPULATORS
    weak_ptr<ELEMENT_TYPE>& operator=(const weak_ptr<ELEMENT_TYPE>& rhs);
        // Make this weak pointer refer to the same object (if any) as the
        // specified 'rhs' weak pointer.  Decrement the number of weak
        // references to the object to which this weak pointer currently refers
        // (if any), and increment the number of weak references to the object
        // referred to by 'rhs' (if any).  Return a reference to this
        // modifiable weak pointer.  Note that if 'rhs' is in the empty state,
        // this weak pointer will be set to the empty state.

    template <class COMPATIBLE_TYPE>
    weak_ptr<ELEMENT_TYPE>& operator=(const shared_ptr<COMPATIBLE_TYPE>& rhs);
    template <class COMPATIBLE_TYPE>
    weak_ptr<ELEMENT_TYPE>& operator=(const weak_ptr<COMPATIBLE_TYPE>& rhs);
        // Make this weak pointer refer to the same object (if any) as the
        // specified 'rhs' (shared or weak) pointer to the parameterized
        // 'COMPATIBLE_TYPE'.  Decrement the number of weak references to the
        // object to which this weak pointer currently refers (if any), and
        // increment the number of weak references to the object referred to by
        // 'rhs' (if any).  Return a reference to this modifiable weak pointer.
        // If 'COMPATIBLE_TYPE *' is not implicitly convertible to 'TYPE *',
        // then a compiler diagnostic will be emitted.  Note that if 'rhs' is
        // in the empty state, this weak pointer will be set to the empty
        // state.

    void swap(weak_ptr<ELEMENT_TYPE>& other);
        // Efficiently exchange the states of this weak pointer and the
        // specified 'other' weak pointer such that each will refer to the
        // object (if any) and representation formerly referred to by the
        // other.

    // ACCESSORS
    shared_ptr<ELEMENT_TYPE> acquireSharedPtr() const;
        // Return a shared pointer to the object referred to by this weak
        // pointer if 'false == expired()', and a shared pointer in the empty
        // state otherwise.

    int numReferences() const;
        // Return a "snapshot" of the current number of shared pointers that
        // share ownership of the object referred to by this weak pointer, or 0
        // if this weak pointer is in the empty state.

    BloombergLP::bslma::SharedPtrRep *rep() const;
        // Return the address of the modifiable
        // 'BloombergLP::bslma::SharedPtrRep' object held by this weak pointer,
        // or 0 if this weak pointer is in the empty state.

                        // C++0x Compatibility

    // TYPES
    typedef ELEMENT_TYPE element_type;
        // 'element_type' is an alias for the 'TYPE' parameter of this class
        // template, and is equivalent to 'ElementType'.

    // MANIPULATORS
    void reset();
        // Reset this weak pointer to the empty state.  If this weak pointer
        // refers to a (possibly shared) object, then decrement the number of
        // weak references to that object.

    // ACCESSORS
    shared_ptr<ELEMENT_TYPE> lock() const;
        // Return a shared pointer to the object referred to by this weak
        // pointer if 'false == expired()', and a shared pointer in the empty
        // state otherwise.  Note that the behavior of this method is the same
        // as that of 'acquireSharedPtr'.

    int use_count() const;
        // Return a "snapshot" of the current number of shared pointers that
        // share ownership of the object referred to by this weak pointer, or 0
        // if this weak pointer is in the empty state.  Note that the behavior
        // of this method is the same as that of 'numReferences'.

    bool expired() const;
        // Return 'true' if this weak pointer is in the empty state or the
        // object that it originally referenced has been destroyed, and 'false'
        // otherwise.

    template <class COMPATIBLE_TYPE>
    bool owner_before(const shared_ptr<COMPATIBLE_TYPE>& other) const;
    template <class COMPATIBLE_TYPE>
    bool owner_before(const weak_ptr<COMPATIBLE_TYPE>& other) const;
        // Return 'true' if this weak pointer can be ordered before the
        // specified 'other' (shared or weak) pointer, and 'false' otherwise.
};

                        // C++0x Compatibility

// FREE METHODS
template <class ELEMENT_TYPE>
void swap(weak_ptr<ELEMENT_TYPE>& a, weak_ptr<ELEMENT_TYPE>& b);
    // Efficiently exchange the states of the specified 'a' and 'b' weak
    // pointers such that each will refer to the object (if any) and
    // representation formerly referred to by the other.

}  // close namespace bsl

namespace BloombergLP {
namespace bslstl {
                        // ============================
                        // struct bslstl::SharedPtrUtil
                        // ============================

struct SharedPtrUtil {
    // This 'struct' provides a namespace for operations on shared pointers.

    // CLASS METHODS
    template <class TARGET, class SOURCE>
    static
    bsl::shared_ptr<TARGET> dynamicCast(const bsl::shared_ptr<SOURCE>& source);
        // Return a 'bsl::shared_ptr<TARGET>' object sharing ownership of the
        // same object as the specified 'source' shared pointer to the
        // parameterized 'SOURCE' type, and referring to
        // 'dynamic_cast<TARGET *>(source.ptr())'.  If 'source' cannot be
        // dynamically cast to 'TARGET *', then an empty
        // 'bsl::shared_ptr<TARGET>' object is returned.

    template <class TARGET, class SOURCE>
    static
    bsl::shared_ptr<TARGET> staticCast(const bsl::shared_ptr<SOURCE>& source);
        // Return a 'bsl::shared_ptr<TARGET>' object sharing ownership of the
        // same object as the specified 'source' shared pointer to the
        // parameterized 'SOURCE' type, and referring to
        // 'static_cast<TARGET *>(source.ptr())'.  Note that if 'source' cannot
        // be statically cast to 'TARGET *', then a compiler diagnostic will be
        // emitted indicating the error.

    template <class TARGET, class SOURCE>
    static
    bsl::shared_ptr<TARGET> constCast(const bsl::shared_ptr<SOURCE>& source);
        // Return a 'bsl::shared_ptr<TARGET>' object sharing ownership of the
        // same object as the specified 'source' shared pointer to the
        // parameterized 'SOURCE' type, and referring to
        // 'const_cast<TARGET *>(source.ptr())'.  Note that if 'source' cannot
        // be 'const'-cast to 'TARGET *', then a compiler diagnostic will be
        // emitted indicating the error.

    template <class TARGET, class SOURCE>
    static
    void dynamicCast(bsl::shared_ptr<TARGET>        *target,
                     const bsl::shared_ptr<SOURCE>&  source);
        // Load into the specified 'target' shared pointer an alias of the
        // specified 'source' shared pointer referring to
        // 'dynamic_cast<TARGET *>(source.ptr())'.  The previous 'target'
        // shared pointer is destroyed (destroying the shared object if
        // 'target' holds the last reference to this object).  If 'source'
        // cannot be dynamically cast to 'TARGET *', 'target' will be reset to
        // the empty state.

    template <class TARGET, class SOURCE>
    static
    void staticCast(bsl::shared_ptr<TARGET>        *target,
                    const bsl::shared_ptr<SOURCE>&  source);
        // Load into the specified 'target' shared pointer an alias of the
        // specified 'source' shared pointer referring to
        // 'static_cast<TARGET *>(source.ptr())'.  The previous 'target' shared
        // pointer is destroyed (destroying the shared object if 'target' holds
        // the last reference to this object).  Note that if 'source' cannot be
        // statically cast to 'TARGET *', then a compiler diagnostic will be
        // emitted indicating the error.

    template <class TARGET, class SOURCE>
    static
    void constCast(bsl::shared_ptr<TARGET>        *target,
                   const bsl::shared_ptr<SOURCE>&  source);
        // Load into the specified 'target' shared pointer an alias of the
        // specified 'source' shared pointer referring to
        // 'const_cast<TARGET *>(source.ptr())'.  The previous 'target' shared
        // pointer is destroyed (destroying the shared object if 'target' holds
        // the last reference to this object).  Note that if 'source' cannot be
        // statically cast to 'TARGET *', then a compiler diagnostic will be
        // emitted indicating the error.

    static
    bsl::shared_ptr<char>
    createInplaceUninitializedBuffer(bsl::size_t       bufferSize,
                                     bslma::Allocator *basicAllocator = 0);
        // Return a shared pointer with an in-place representation to a
        // newly-created uninitialized buffer of the specified 'bufferSize' (in
        // bytes).  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.
};

                        // ==================================
                        // struct bslstl::SharedPtrNilDeleter
                        // ==================================

struct SharedPtrNilDeleter {
    // This 'struct' provides a function-like shared pointer deleter that does
    // nothing when invoked.

    // ACCESSORS
    void operator()(const void *) const;
        // No-Op.
};

}  // close namespace bslstl
}  // close namespace BloombergLP

// ============================================================================
//                      INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

namespace bsl {

                            // ----------------
                            // class shared_ptr
                            // ----------------

// PRIVATE CLASS METHODS
template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE, class ALLOCATOR>
inline
BloombergLP::bslma::SharedPtrRep *
shared_ptr<ELEMENT_TYPE>::makeInternalRep(
                                      COMPATIBLE_TYPE  *ptr,
                                      ALLOCATOR        *,
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

    BloombergLP::bslma::Allocator *defaultAllocator =
                               BloombergLP::bslma::Default::defaultAllocator();
    return RepMaker::makeOutofplaceRep(ptr, deleter, defaultAllocator);
}

template <class ELEMENT_TYPE>
template <class INPLACE_REP>
inline
BloombergLP::bslma::SharedPtrRep *
shared_ptr<ELEMENT_TYPE>::makeInternalRep(
                                         ELEMENT_TYPE       *,
                                         INPLACE_REP        *,
                                         BloombergLP::bslma::SharedPtrRep *rep)
{
    return rep;
}


// CREATORS
template <class ELEMENT_TYPE>
inline
shared_ptr<ELEMENT_TYPE>::shared_ptr()
: d_ptr_p(0)
, d_rep_p(0)
{
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
inline
shared_ptr<ELEMENT_TYPE>::shared_ptr(COMPATIBLE_TYPE *ptr)
: d_ptr_p(ptr)
{
    typedef BloombergLP::bslma::SharedPtrOutofplaceRep<
                                              COMPATIBLE_TYPE,
                                              BloombergLP::bslma::Allocator *>
                                                                      RepMaker;

    BloombergLP::bslma::Allocator *defaultAllocator =
                               BloombergLP::bslma::Default::defaultAllocator();
    d_rep_p = RepMaker::makeOutofplaceRep(ptr,
                                          defaultAllocator,
                                          defaultAllocator);
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
inline
shared_ptr<ELEMENT_TYPE>::shared_ptr(
                                 COMPATIBLE_TYPE  *ptr,
                                 BloombergLP::bslma::Allocator *basicAllocator)
: d_ptr_p(ptr)
{
    typedef BloombergLP::bslma::SharedPtrOutofplaceRep<
                                               COMPATIBLE_TYPE,
                                               BloombergLP::bslma::Allocator *>
                                                                      RepMaker;

    d_rep_p = RepMaker::makeOutofplaceRep(ptr, basicAllocator, basicAllocator);
}

template <class ELEMENT_TYPE>
inline
shared_ptr<ELEMENT_TYPE>::shared_ptr(
                                 ELEMENT_TYPE                     *ptr,
                                 BloombergLP::bslma::SharedPtrRep *rep)
: d_ptr_p(ptr)
, d_rep_p(rep)
{
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE, class DISPATCH>
inline
shared_ptr<ELEMENT_TYPE>::shared_ptr(COMPATIBLE_TYPE  *ptr,
                                     DISPATCH *const&  dispatch)
: d_ptr_p(ptr)
, d_rep_p(makeInternalRep(ptr, dispatch, dispatch))
{
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE, class DELETER>
inline
shared_ptr<ELEMENT_TYPE>::shared_ptr(
                                 COMPATIBLE_TYPE               *ptr,
                                 const DELETER&                 deleter,
                                 BloombergLP::bslma::Allocator *basicAllocator)
: d_ptr_p(ptr)
{
    typedef BloombergLP::bslma::SharedPtrOutofplaceRep<COMPATIBLE_TYPE,
                                                       DELETER> RepMaker;

    d_rep_p = RepMaker::makeOutofplaceRep(ptr, deleter, basicAllocator);
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
template <class DELETER>
inline
shared_ptr<ELEMENT_TYPE>::shared_ptr(nullptr_t,
                                     const DELETER&,
                                     BloombergLP::bslma::Allocator *)
: d_ptr_p(0)
, d_rep_p(0)
{
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
shared_ptr<ELEMENT_TYPE>::shared_ptr(
       BloombergLP::bslma::ManagedPtr<COMPATIBLE_TYPE>  managedPtr,
       BloombergLP::bslma::Allocator                   *basicAllocator)
: d_ptr_p(managedPtr.ptr())
, d_rep_p(0)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<
                     BloombergLP::bslma::ManagedPtr<ELEMENT_TYPE> > Rep;

    if (d_ptr_p) {
        if (&BloombergLP::bslma::SharedPtrRep::managedPtrDeleter ==
                                              managedPtr.deleter().deleter()) {
            d_rep_p = static_cast<BloombergLP::bslma::SharedPtrRep *>
                                       (managedPtr.release().second.factory());
        }
        else {
            basicAllocator =
                        BloombergLP::bslma::Default::allocator(basicAllocator);
            Rep *rep = new(*basicAllocator) Rep(basicAllocator);
            (*rep->ptr()) = managedPtr;
            d_rep_p = rep;
        }
    }
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
shared_ptr<ELEMENT_TYPE>::shared_ptr(
                                std::auto_ptr<COMPATIBLE_TYPE>  autoPtr,
                                BloombergLP::bslma::Allocator  *basicAllocator)
: d_ptr_p(autoPtr.get())
, d_rep_p(0)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<
                                          std::auto_ptr<COMPATIBLE_TYPE> > Rep;

    if (d_ptr_p) {
        basicAllocator =
                        BloombergLP::bslma::Default::allocator(basicAllocator);
        Rep *rep = new (*basicAllocator) Rep(basicAllocator);
        (*rep->ptr()) = autoPtr;
        d_rep_p = rep;
    }
}

template <class ELEMENT_TYPE>
template <class ANY_TYPE>
shared_ptr<ELEMENT_TYPE>::shared_ptr(const shared_ptr<ANY_TYPE>&  source,
                                     ELEMENT_TYPE                *object)
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

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
shared_ptr<ELEMENT_TYPE>::shared_ptr(
                                 const shared_ptr<COMPATIBLE_TYPE>& other)
: d_ptr_p(other.d_ptr_p)
, d_rep_p(other.d_rep_p)
{
    if (d_ptr_p) {
        d_rep_p->acquireRef();
    } else {
        d_rep_p = 0;
    }
}

template <class ELEMENT_TYPE>
shared_ptr<ELEMENT_TYPE>::shared_ptr(const shared_ptr& original)
: d_ptr_p(original.d_ptr_p)
, d_rep_p(original.d_rep_p)
{
    if (d_ptr_p) {
        d_rep_p->acquireRef();
    } else {
        d_rep_p = 0;
    }
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
template <class ELEMENT_TYPE>
inline
shared_ptr<ELEMENT_TYPE>::shared_ptr(BloombergLP::bslma::SharedPtrRep *rep)
: d_ptr_p(rep ? reinterpret_cast<ELEMENT_TYPE *>(rep->originalPtr()) : 0)
, d_rep_p(rep)
{
}
#endif

template <class ELEMENT_TYPE>
shared_ptr<ELEMENT_TYPE>::~shared_ptr()
{
    BSLS_ASSERT_SAFE(!d_rep_p || d_ptr_p);

    if (d_rep_p) {
        d_rep_p->releaseRef();
    }
}

// MANIPULATORS
template <class ELEMENT_TYPE>
shared_ptr<ELEMENT_TYPE>&
shared_ptr<ELEMENT_TYPE>::operator=(const shared_ptr& rhs)
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
template <class COMPATIBLE_TYPE>
shared_ptr<ELEMENT_TYPE>&
shared_ptr<ELEMENT_TYPE>::operator=(
                                   const shared_ptr<COMPATIBLE_TYPE>& rhs)
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
template <class COMPATIBLE_TYPE>
inline
shared_ptr<ELEMENT_TYPE>&
shared_ptr<ELEMENT_TYPE>::operator=(std::auto_ptr<COMPATIBLE_TYPE> rhs)
{
    SelfType(rhs).swap(*this);
    return *this;
}

template <class ELEMENT_TYPE>
inline
void shared_ptr<ELEMENT_TYPE>::clear()
{
    reset();
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
inline
void
shared_ptr<ELEMENT_TYPE>::load(COMPATIBLE_TYPE  *ptr,
                               BloombergLP::bslma::Allocator *basicAllocator)
{
    SelfType(ptr, basicAllocator).swap(*this);
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE, class DELETER>
inline
void
shared_ptr<ELEMENT_TYPE>::load(COMPATIBLE_TYPE  *ptr,
                               const DELETER&    deleter,
                               BloombergLP::bslma::Allocator *basicAllocator)
{
    SelfType(ptr, deleter, basicAllocator).swap(*this);
}

template <class ELEMENT_TYPE>
template <class ANY_TYPE>
void
shared_ptr<ELEMENT_TYPE>::loadAlias(const shared_ptr<ANY_TYPE>&  source,
                                    ELEMENT_TYPE                *object)
{
    if (source.d_rep_p == d_rep_p && object) {
        d_ptr_p = d_rep_p ? object : 0;
    }
    else {
        SelfType(source, object).swap(*this);
    }
}

template <class ELEMENT_TYPE>
void
shared_ptr<ELEMENT_TYPE>::createInplace(
                                 BloombergLP::bslma::Allocator *basicAllocator)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<ELEMENT_TYPE> Rep;
    basicAllocator = BloombergLP::bslma::Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class ELEMENT_TYPE>
template <class A1>
void
shared_ptr<ELEMENT_TYPE>::createInplace(
                                 BloombergLP::bslma::Allocator *basicAllocator,
                                 const A1&                      a1)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<ELEMENT_TYPE> Rep;
    basicAllocator = BloombergLP::bslma::Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator, a1);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class ELEMENT_TYPE>
template <class A1, class A2>
void
shared_ptr<ELEMENT_TYPE>::createInplace(
                                 BloombergLP::bslma::Allocator *basicAllocator,
                                 const A1&                      a1,
                                 const A2&                      a2)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<ELEMENT_TYPE> Rep;
    basicAllocator = BloombergLP::bslma::Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator, a1, a2);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class ELEMENT_TYPE>
template <class A1, class A2, class A3>
void
shared_ptr<ELEMENT_TYPE>::createInplace(
                                 BloombergLP::bslma::Allocator *basicAllocator,
                                 const A1&                      a1,
                                 const A2&                      a2,
                                 const A3&                      a3)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<ELEMENT_TYPE> Rep;
    basicAllocator = BloombergLP::bslma::Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator, a1, a2, a3);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class ELEMENT_TYPE>
template <class A1, class A2, class A3, class A4>
void
shared_ptr<ELEMENT_TYPE>::createInplace(
                                 BloombergLP::bslma::Allocator *basicAllocator,
                                 const A1&                      a1,
                                 const A2&                      a2,
                                 const A3&                      a3,
                                 const A4&                      a4)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<ELEMENT_TYPE> Rep;
    basicAllocator = BloombergLP::bslma::Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator, a1, a2, a3, a4);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class ELEMENT_TYPE>
template <class A1, class A2, class A3, class A4, class A5>
void
shared_ptr<ELEMENT_TYPE>::createInplace(
                                 BloombergLP::bslma::Allocator *basicAllocator,
                                 const A1&                      a1,
                                 const A2&                      a2,
                                 const A3&                      a3,
                                 const A4&                      a4,
                                 const A5&                      a5)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<ELEMENT_TYPE> Rep;
    basicAllocator = BloombergLP::bslma::Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator, a1, a2, a3, a4, a5);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class ELEMENT_TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6>
void
shared_ptr<ELEMENT_TYPE>::createInplace(
                                 BloombergLP::bslma::Allocator *basicAllocator,
                                 const A1&                      a1,
                                 const A2&                      a2,
                                 const A3&                      a3,
                                 const A4&                      a4,
                                 const A5&                      a5,
                                 const A6&                      a6)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<ELEMENT_TYPE> Rep;
    basicAllocator = BloombergLP::bslma::Default::allocator(basicAllocator);
    Rep *rep = new (*basicAllocator) Rep(basicAllocator,
                                         a1,
                                         a2,
                                         a3,
                                         a4,
                                         a5,
                                         a6);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class ELEMENT_TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
void
shared_ptr<ELEMENT_TYPE>::createInplace(
                                 BloombergLP::bslma::Allocator *basicAllocator,
                                 const A1&                      a1,
                                 const A2&                      a2,
                                 const A3&                      a3,
                                 const A4&                      a4,
                                 const A5&                      a5,
                                 const A6&                      a6,
                                 const A7&                      a7)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<ELEMENT_TYPE> Rep;
    basicAllocator = BloombergLP::bslma::Default::allocator(basicAllocator);
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

template <class ELEMENT_TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8>
void
shared_ptr<ELEMENT_TYPE>::createInplace(
                                 BloombergLP::bslma::Allocator *basicAllocator,
                                 const A1&                      a1,
                                 const A2&                      a2,
                                 const A3&                      a3,
                                 const A4&                      a4,
                                 const A5&                      a5,
                                 const A6&                      a6,
                                 const A7&                      a7,
                                 const A8&                      a8)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<ELEMENT_TYPE> Rep;
    basicAllocator = BloombergLP::bslma::Default::allocator(basicAllocator);
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

template <class ELEMENT_TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
void
shared_ptr<ELEMENT_TYPE>::createInplace(
                                 BloombergLP::bslma::Allocator *basicAllocator,
                                 const A1&                      a1,
                                 const A2&                      a2,
                                 const A3&                      a3,
                                 const A4&                      a4,
                                 const A5&                      a5,
                                 const A6&                      a6,
                                 const A7&                      a7,
                                 const A8&                      a8,
                                 const A9&                      a9)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<ELEMENT_TYPE> Rep;
    basicAllocator = BloombergLP::bslma::Default::allocator(basicAllocator);
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

template <class ELEMENT_TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10>
void
shared_ptr<ELEMENT_TYPE>::createInplace(
                                 BloombergLP::bslma::Allocator *basicAllocator,
                                 const A1&                      a1,
                                 const A2&                      a2,
                                 const A3&                      a3,
                                 const A4&                      a4,
                                 const A5&                      a5,
                                 const A6&                      a6,
                                 const A7&                      a7,
                                 const A8&                      a8,
                                 const A9&                      a9,
                                 const A10&                     a10)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<ELEMENT_TYPE> Rep;
    basicAllocator = BloombergLP::bslma::Default::allocator(basicAllocator);
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

template <class ELEMENT_TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
void
shared_ptr<ELEMENT_TYPE>::createInplace(
                                 BloombergLP::bslma::Allocator *basicAllocator,
                                 const A1&                      a1,
                                 const A2&                      a2,
                                 const A3&                      a3,
                                 const A4&                      a4,
                                 const A5&                      a5,
                                 const A6&                      a6,
                                 const A7&                      a7,
                                 const A8&                      a8,
                                 const A9&                      a9,
                                 const A10&                     a10,
                                 const A11&                     a11)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<ELEMENT_TYPE> Rep;
    basicAllocator = BloombergLP::bslma::Default::allocator(basicAllocator);
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

template <class ELEMENT_TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
void
shared_ptr<ELEMENT_TYPE>::createInplace(
                                 BloombergLP::bslma::Allocator *basicAllocator,
                                 const A1&                      a1,
                                 const A2&                      a2,
                                 const A3&                      a3,
                                 const A4&                      a4,
                                 const A5&                      a5,
                                 const A6&                      a6,
                                 const A7&                      a7,
                                 const A8&                      a8,
                                 const A9&                      a9,
                                 const A10&                     a10,
                                 const A11&                     a11,
                                 const A12&                     a12)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<ELEMENT_TYPE> Rep;
    basicAllocator = BloombergLP::bslma::Default::allocator(basicAllocator);
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

template <class ELEMENT_TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
void
shared_ptr<ELEMENT_TYPE>::createInplace(
                                 BloombergLP::bslma::Allocator *basicAllocator,
                                 const A1&                      a1,
                                 const A2&                      a2,
                                 const A3&                      a3,
                                 const A4&                      a4,
                                 const A5&                      a5,
                                 const A6&                      a6,
                                 const A7&                      a7,
                                 const A8&                      a8,
                                 const A9&                      a9,
                                 const A10&                     a10,
                                 const A11&                     a11,
                                 const A12&                     a12,
                                 const A13&                     a13)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<ELEMENT_TYPE> Rep;
    basicAllocator = BloombergLP::bslma::Default::allocator(basicAllocator);
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

template <class ELEMENT_TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
void
shared_ptr<ELEMENT_TYPE>::createInplace(
                                 BloombergLP::bslma::Allocator *basicAllocator,
                                 const A1&                      a1,
                                 const A2&                      a2,
                                 const A3&                      a3,
                                 const A4&                      a4,
                                 const A5&                      a5,
                                 const A6&                      a6,
                                 const A7&                      a7,
                                 const A8&                      a8,
                                 const A9&                      a9,
                                 const A10&                     a10,
                                 const A11&                     a11,
                                 const A12&                     a12,
                                 const A13&                     a13,
                                 const A14&                     a14)
{
    typedef BloombergLP::bslma::SharedPtrInplaceRep<ELEMENT_TYPE> Rep;
    basicAllocator = BloombergLP::bslma::Default::allocator(basicAllocator);
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

template <class ELEMENT_TYPE>
pair<ELEMENT_TYPE *, BloombergLP::bslma::SharedPtrRep *>
shared_ptr<ELEMENT_TYPE>::release()
{
    pair<ELEMENT_TYPE *, BloombergLP::bslma::SharedPtrRep *> ret(d_ptr_p,
                                                                 d_rep_p);
    d_ptr_p = 0;
    d_rep_p = 0;
    return ret;
}

template <class ELEMENT_TYPE>
inline
void shared_ptr<ELEMENT_TYPE>::swap(shared_ptr<ELEMENT_TYPE>& other)
{
    BloombergLP::bslalg::SwapUtil::swap(&d_ptr_p, &other.d_ptr_p);
    BloombergLP::bslalg::SwapUtil::swap(&d_rep_p, &other.d_rep_p);
}

// ACCESSORS
template <class ELEMENT_TYPE>
inline
#if defined(BSLS_PLATFORM_CMP_IBM)
shared_ptr<ELEMENT_TYPE>::operator typename shared_ptr::BoolType() const
#else
shared_ptr<ELEMENT_TYPE>::operator BoolType() const
#endif
{
    return BloombergLP::bsls::UnspecifiedBool<shared_ptr>::makeValue(d_ptr_p);
}

template <class ELEMENT_TYPE>
inline
typename add_lvalue_reference<ELEMENT_TYPE>::type
shared_ptr<ELEMENT_TYPE>::operator[](ptrdiff_t index) const
{
    return *(d_ptr_p + index);
}

template <class ELEMENT_TYPE>
inline
typename add_lvalue_reference<ELEMENT_TYPE>::type
shared_ptr<ELEMENT_TYPE>::operator*() const
{
    return *d_ptr_p;
}

template <class ELEMENT_TYPE>
inline
ELEMENT_TYPE *shared_ptr<ELEMENT_TYPE>::operator->() const
{
    return d_ptr_p;
}

template <class ELEMENT_TYPE>
inline
ELEMENT_TYPE *shared_ptr<ELEMENT_TYPE>::ptr() const
{
    return d_ptr_p;
}

template <class ELEMENT_TYPE>
inline
BloombergLP::bslma::SharedPtrRep *shared_ptr<ELEMENT_TYPE>::rep() const
{
    return d_rep_p;
}

template <class ELEMENT_TYPE>
inline
int shared_ptr<ELEMENT_TYPE>::numReferences() const
{
    return d_rep_p ? d_rep_p->numReferences() : 0;
}

template <class ELEMENT_TYPE>
BloombergLP::bslma::ManagedPtr<ELEMENT_TYPE>
shared_ptr<ELEMENT_TYPE>::managedPtr() const
{
    if (d_rep_p) {
        d_rep_p->acquireRef();
    }
    BloombergLP::bslma::ManagedPtr<ELEMENT_TYPE> ptr(d_ptr_p,
                                                     d_rep_p,
                         &BloombergLP::bslma::SharedPtrRep::managedPtrDeleter);
    return ptr;
}

                        // *** std::tr1 COMPATIBILITY ***

// MANIPULATORS
template <class ELEMENT_TYPE>
inline
void shared_ptr<ELEMENT_TYPE>::reset()
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
void shared_ptr<ELEMENT_TYPE>::reset(COMPATIBLE_TYPE *ptr)
{
    // Wrap 'ptr' in 'auto_ptr' to ensure standard delete behavior.

    std::auto_ptr<COMPATIBLE_TYPE> ap(ptr);
    SelfType(ap).swap(*this);
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE, class DELETER>
inline
void
shared_ptr<ELEMENT_TYPE>::reset(COMPATIBLE_TYPE *ptr,
                                     const DELETER&   deleter)
{
    SelfType(ptr, deleter, 0).swap(*this);
}

template <class ELEMENT_TYPE>
template <class ANY_TYPE>
inline
void shared_ptr<ELEMENT_TYPE>::reset(
                                      const shared_ptr<ANY_TYPE>&  source,
                                      ELEMENT_TYPE                     *ptr)
{
    SelfType(source, ptr).swap(*this);
}

// ACCESSORS
template <class ELEMENT_TYPE>
inline
ELEMENT_TYPE *shared_ptr<ELEMENT_TYPE>::get() const
{
    return ptr();
}

template <class ELEMENT_TYPE>
inline
bool shared_ptr<ELEMENT_TYPE>::unique() const
{
    return 1 == numReferences();
}

template <class ELEMENT_TYPE>
inline
int shared_ptr<ELEMENT_TYPE>::use_count() const
{
    return numReferences();
}

template <class ELEMENT_TYPE>
template<class ANY_TYPE>
inline
bool shared_ptr<ELEMENT_TYPE>::owner_before(
                                  const shared_ptr<ANY_TYPE>& other) const
{
    return std::less<BloombergLP::bslma::SharedPtrRep *>()(rep(), other.rep());
}

                        // --------------
                        // class weak_ptr
                        // --------------

// CREATORS
template <class ELEMENT_TYPE>
inline
weak_ptr<ELEMENT_TYPE>::weak_ptr()
: d_ptr_p(0)
, d_rep_p(0)
{
}

template <class ELEMENT_TYPE>
weak_ptr<ELEMENT_TYPE>::weak_ptr(const weak_ptr<ELEMENT_TYPE>& original)
: d_ptr_p(original.d_ptr_p)
, d_rep_p(original.d_rep_p)
{
    if (d_rep_p) {
        d_rep_p->acquireWeakRef();
    }
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
weak_ptr<ELEMENT_TYPE>::weak_ptr(const shared_ptr<COMPATIBLE_TYPE>& other)
: d_ptr_p(other.ptr())
, d_rep_p(other.rep())
{
    if (d_rep_p) {
        d_rep_p->acquireWeakRef();
    }
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
weak_ptr<ELEMENT_TYPE>::weak_ptr(const weak_ptr<COMPATIBLE_TYPE>& other)
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

// MANIPULATORS
template <class ELEMENT_TYPE>
weak_ptr<ELEMENT_TYPE>& weak_ptr<ELEMENT_TYPE>::operator=(
                                             const weak_ptr<ELEMENT_TYPE>& rhs)
{
    weak_ptr<ELEMENT_TYPE> tmp(rhs);
    tmp.swap(*this);
    return *this;
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
weak_ptr<ELEMENT_TYPE>& weak_ptr<ELEMENT_TYPE>::operator=(
                                    const shared_ptr<COMPATIBLE_TYPE>& rhs)
{
    weak_ptr<ELEMENT_TYPE> tmp(rhs);
    tmp.swap(*this);
    return *this;
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
weak_ptr<ELEMENT_TYPE>& weak_ptr<ELEMENT_TYPE>::operator=(
                                      const weak_ptr<COMPATIBLE_TYPE>& rhs)
{
    weak_ptr<ELEMENT_TYPE> tmp(rhs);
    tmp.swap(*this);
    return *this;
}

template <class ELEMENT_TYPE>
void weak_ptr<ELEMENT_TYPE>::swap(weak_ptr<ELEMENT_TYPE>& other)
{
    BloombergLP::bslalg::SwapUtil::swap(&d_ptr_p, &other.d_ptr_p);
    BloombergLP::bslalg::SwapUtil::swap(&d_rep_p, &other.d_rep_p);
}

// ACCESSORS
template <class ELEMENT_TYPE>
shared_ptr<ELEMENT_TYPE> weak_ptr<ELEMENT_TYPE>::acquireSharedPtr() const
{
    if (d_rep_p && d_rep_p->tryAcquireRef()) {
        return shared_ptr<ELEMENT_TYPE>(d_ptr_p, d_rep_p);            // RETURN
    }
    return shared_ptr<ELEMENT_TYPE>();
}

template <class ELEMENT_TYPE>
inline
int weak_ptr<ELEMENT_TYPE>::numReferences() const
{
    return d_rep_p ? d_rep_p->numReferences() : 0;
}

template <class ELEMENT_TYPE>
inline
BloombergLP::bslma::SharedPtrRep *weak_ptr<ELEMENT_TYPE>::rep() const
{
    return d_rep_p;
}

                        // C++0x Compatibility

// MANIPULATORS
template <class ELEMENT_TYPE>
inline
void weak_ptr<ELEMENT_TYPE>::reset()
{
    if (d_rep_p) {
        d_rep_p->releaseWeakRef();
    }

    d_ptr_p = 0;
    d_rep_p = 0;
}

// ACCESSORS
template <class ELEMENT_TYPE>
inline
shared_ptr<ELEMENT_TYPE> weak_ptr<ELEMENT_TYPE>::lock() const
{
    return acquireSharedPtr();
}

template <class ELEMENT_TYPE>
inline
int weak_ptr<ELEMENT_TYPE>::use_count() const
{
    return numReferences();
}

template <class ELEMENT_TYPE>
inline
bool weak_ptr<ELEMENT_TYPE>::expired() const
{
    return !(d_rep_p && d_rep_p->numReferences());
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
inline
bool weak_ptr<ELEMENT_TYPE>::owner_before(
                             const shared_ptr<COMPATIBLE_TYPE>& other) const
{
    return std::less<BloombergLP::bslma::SharedPtrRep *>()(d_rep_p,
                                                           other.rep());
}

template <class ELEMENT_TYPE>
template <class COMPATIBLE_TYPE>
inline
bool weak_ptr<ELEMENT_TYPE>::owner_before(
                                  const weak_ptr<COMPATIBLE_TYPE>& other) const
{
    return std::less<BloombergLP::bslma::SharedPtrRep *>()(d_rep_p,
                                                           other.d_rep_p);
}

}  // close namespace bsl

namespace BloombergLP {
namespace bslstl {

                        // ----------------------------
                        // struct bslstl::SharedPtrUtil
                        // ----------------------------

// CLASS METHODS
template <class TARGET, class SOURCE>
inline
bsl::shared_ptr<TARGET>
SharedPtrUtil::dynamicCast(const bsl::shared_ptr<SOURCE>& source)
{
    return bsl::shared_ptr<TARGET>(source,
                                   dynamic_cast<TARGET *>(source.ptr()));
}

template <class TARGET, class SOURCE>
inline
bsl::shared_ptr<TARGET>
SharedPtrUtil::staticCast(const bsl::shared_ptr<SOURCE>& source)
{
    return bsl::shared_ptr<TARGET>(source,
                                   static_cast<TARGET *>(source.ptr()));
}

template <class TARGET, class SOURCE>
inline
bsl::shared_ptr<TARGET>
SharedPtrUtil::constCast(const bsl::shared_ptr<SOURCE>& source)
{
    return bsl::shared_ptr<TARGET>(source,
                                   const_cast<TARGET *>(source.ptr()));
}

template <class TARGET, class SOURCE>
void SharedPtrUtil::dynamicCast(bsl::shared_ptr<TARGET>        *target,
                                const bsl::shared_ptr<SOURCE>&  source)
{
    target->loadAlias(source, dynamic_cast<TARGET *>(source.ptr()));
}

template <class TARGET, class SOURCE>
inline
void SharedPtrUtil::staticCast(bsl::shared_ptr<TARGET>        *target,
                               const bsl::shared_ptr<SOURCE>&  source)
{
    target->loadAlias(source, static_cast<TARGET *>(source.ptr()));
}

template <class TARGET, class SOURCE>
inline
void SharedPtrUtil::constCast(bsl::shared_ptr<TARGET>        *target,
                              const bsl::shared_ptr<SOURCE>&  source)
{
    target->loadAlias(source, const_cast<TARGET *>(source.ptr()));
}

                      // --------------------------------
                      // struct bslstl::SharedPtrNilDeleter
                      // --------------------------------

// ACCESSORS
inline
void bslstl::SharedPtrNilDeleter::operator()(const void *) const
{
}

}  // close namespace bslstl
}  // close namespace BloombergLP

// FREE OPERATORS
template <class LHS_TYPE, class RHS_TYPE>
inline
bool bsl::operator==(const shared_ptr<LHS_TYPE>& lhs,
                     const shared_ptr<RHS_TYPE>& rhs)
{
    return rhs.ptr() == lhs.ptr();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bsl::operator!=(const shared_ptr<LHS_TYPE>& lhs,
                     const shared_ptr<RHS_TYPE>& rhs)
{
    return !(lhs == rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bsl::operator<(const shared_ptr<LHS_TYPE>& lhs,
                    const shared_ptr<RHS_TYPE>& rhs)
{
    return std::less<const void *>()(lhs.ptr(), rhs.ptr());
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bsl::operator>(const shared_ptr<LHS_TYPE>& lhs,
                    const shared_ptr<RHS_TYPE>& rhs)
{
    return rhs < lhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bsl::operator<=(const shared_ptr<LHS_TYPE>& lhs,
                     const shared_ptr<RHS_TYPE>& rhs)
{
    return !(rhs < lhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool bsl::operator>=(const shared_ptr<LHS_TYPE>& lhs,
                     const shared_ptr<RHS_TYPE>& rhs)
{
    return !(lhs < rhs);
}

template <class LHS_TYPE>
inline
bool bsl::operator==(const shared_ptr<LHS_TYPE>& lhs, bsl::nullptr_t)
{
    return !lhs;
}

template <class RHS_TYPE>
inline
bool bsl::operator==(bsl::nullptr_t, const shared_ptr<RHS_TYPE>& rhs)
{
    return !rhs;
}

template <class LHS_TYPE>
inline
bool bsl::operator!=(const shared_ptr<LHS_TYPE>& lhs, bsl::nullptr_t)
{
    return static_cast<bool>(lhs);
}

template <class RHS_TYPE>
inline
bool bsl::operator!=(bsl::nullptr_t, const shared_ptr<RHS_TYPE>& rhs)
{
    return static_cast<bool>(rhs);
}

template <class LHS_TYPE>
inline
bool bsl::operator<(const shared_ptr<LHS_TYPE>& lhs, bsl::nullptr_t)
{
    return std::less<LHS_TYPE *>()(lhs.ptr(), 0);
}

template <class RHS_TYPE>
inline
bool bsl::operator<(bsl::nullptr_t, const shared_ptr<RHS_TYPE>& rhs)
{
    return std::less<RHS_TYPE *>()(0, rhs.ptr());
}

template <class LHS_TYPE>
inline
bool bsl::operator<=(const shared_ptr<LHS_TYPE>& lhs, bsl::nullptr_t)
{
    return !std::less<LHS_TYPE *>()(0, lhs.ptr());
}

template <class RHS_TYPE>
inline
bool bsl::operator<=(bsl::nullptr_t, const shared_ptr<RHS_TYPE>& rhs)
{
    return !std::less<RHS_TYPE *>()(rhs.ptr(), 0);
}

template <class LHS_TYPE>
inline
bool bsl::operator>(const shared_ptr<LHS_TYPE>& lhs, bsl::nullptr_t)
{
    return std::less<LHS_TYPE *>()(0, lhs.ptr());
}

template <class RHS_TYPE>
inline
bool bsl::operator>(bsl::nullptr_t, const shared_ptr<RHS_TYPE>& rhs)
{
    return std::less<RHS_TYPE *>()(rhs.ptr(), 0);
}

template <class LHS_TYPE>
inline
bool bsl::operator>=(const shared_ptr<LHS_TYPE>& lhs, bsl::nullptr_t)
{
    return !std::less<LHS_TYPE *>()(lhs.ptr(), 0);
}

template <class RHS_TYPE>
inline
bool bsl::operator>=(bsl::nullptr_t, const shared_ptr<RHS_TYPE>& rhs)
{
    return !std::less<RHS_TYPE *>()(0, rhs.ptr());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ELEMENT_TYPE>
inline
std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>&
bsl::operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>& stream,
                const shared_ptr<ELEMENT_TYPE>&             rhs)
{
    return stream << rhs.ptr();
}

                        // *** std::tr1 COMPATIBILITY ***

template <class ELEMENT_TYPE>
inline
void bsl::swap(shared_ptr<ELEMENT_TYPE>& a, shared_ptr<ELEMENT_TYPE>& b)
{
    a.swap(b);
}

// FREE METHODS
template <class ELEMENT_TYPE>
inline
void bsl::swap(weak_ptr<ELEMENT_TYPE>& a, weak_ptr<ELEMENT_TYPE>& b)
{
    a.swap(b);
}

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
