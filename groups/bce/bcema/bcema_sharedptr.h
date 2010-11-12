// bcema_sharedptr.h               -*-C++-*-
#ifndef INCLUDED_BCEMA_SHAREDPTR
#define INCLUDED_BCEMA_SHAREDPTR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a polymorphic reference-counted managed pointer wrapper.
//
//@CLASSES:
//                 bcema_SharedPtr: shared pointer
//             bcema_SharedPtrRep : shared pointer representation protocol
//             bcema_SharedPtrUtil: shared pointer utility functions
//             bcema_SharedPtrLess: comparator functor for STL containers
//       bcema_SharedPtrNilDeleter: no-op deleter
//
//@AUTHOR: Ilougino Rocha (irocha)
//         Herve Bronnimann (hbronnim)
//         Vlad Kliatchko (vkliatch)
//
//@SEE_ALSO: bdema_managedptr
//
//@DESCRIPTION: This component implements a thread-safe, generic,
// reference-counted "smart pointer" to support "shared ownership" of
// instances (of the parameterized type 'TYPE') of objects.  Shared pointers
// implement the "envelope/letter" idiom.  For each managed instance, a
// representation is created.  Many shared pointers can simultaneously share
// a reference to the same representation.  Shared pointers also implement the
// "construction is acquisition, destruction is release" idiom.  Upon
// assignment/construction, shared pointer increments the reference count for
// the representation.  Upon re-assignment/destruction, the number of
// references is decremented.  When all references to the representation are
// released, both the representation and the managed instance are destroyed.
// 'bcema_SharedPtr' emulates the interface of a native pointer.  The managed
// object may be accessed directly using the '->' operator, or the dereference
// operator (operator '*') can be used to get a reference to the managed
// object.
//
///Thread Safety
///-------------
// It is safe to access or modify two distinct 'bcema_SharedPtr' objects
// simultaneously, each from a separate thread, even if they share ownership
// of a common object.  It is safe to access a single 'bcema_SharedPtr' object
// simultaneously from two or more separate threads, provided no other thread
// is simultaneously modifying the object.  It is not safe to access or modify
// a 'bcema_SharedPtr' object in one thread while another thread modifies the
// same object.
//
// It is safe to access, modify, copy or delete a shared pointer in one
// thread, while other threads access or modify other shared pointers pointing
// to or managing the same object (the reference count is managed using atomic
// operations).  However, there is no guarantee regarding the safety of
// accessing or modifying the object *pointed* *to* by the shared pointer
// simultaneously from multiple threads.
//
///In-place/Out-of-place Representations
///-------------------------------------
// 'bcema_SharedPtr' provides two types of representations: an out-of-place
// representation, and an in-place representation.  The out-of-place
// representation is used to manage objects that are constructed externally to
// the shared pointer.  Out-of-place objects are provided to a shared pointer
// by pointer along with a deleter that should be used to destroy the object
// when all references to it have been released.  This representation is
// generally less efficient than in-place representations since it usually
// requires at least two allocations (one to construct the object, and one to
// construct the shared pointer representation for the object).  Objects can
// also be constructed directly within a shared pointer representation (see
// 'createInplace'), without its type inheriting from a special class (such as
// 'bsl::enable_shared_from_this'); in that case, 'bcema_SharedPtr' supports up
// to fourteen arguments that can be passed directly to the object constructor.
// This means that both the object and the representation can be constructed in
// one allocation as opposed to two, effectively creating an "intrusive"
// reference counter.  Note that the size of this allocation is determined at
// compile time from the footprint of the object and of the reference counter.
// It is also possible to create shared pointers to buffers whose
// size is determined at runtime only, although such buffers consist of
// uninitialized memory only (arrays of other types are not supported).
//
///Correct Usage of the Allocator Model
///------------------------------------
// Note that once constructed, there is no difference in type, usage, or
// efficiency between the in-place and out-of-place shared pointers, except
// that an in-place shared pointer will exhibit greater locality of reference
// and faster destruction (because there is only one allocated block).
// Also note that a class created with an allocator needs to have this
// allocator specified as its last argument, but this allocator may be
// different from the one passed as the first argument to 'createInplace'.
// For instance, in the following snippet of code:
//..
//  bslma_Allocator *allocator1, *allocator2;
//  // ...
//  bcema_SharedPtr<bsl::string> ptr;
//  ptr.createInplace(allocator1, bsl::string("my string"), allocator2);
//..
// the 'allocator1' is used to allocate the region of memory containing the
// reference counter and 'bsl::string' object representation (essentially a
// length and a pointer to a 'char'), while the memory allocated by the
// 'bsl::string' object is created using the 'allocator2' and the value
// "my string".  In most cases, both allocators should be the same and so
// the allocator should be specified twice.
//
///Deleters
///--------
// When the last reference to a shared pointer is released, the managed object
// is destroyed using the "deleter" provided at the time when the first shared
// pointer representation was instantiated.  This shared pointer supports two
// kinds of "deleter" objects, which vary in how they are invoked.  A
// "function-like" deleter is any language entity that can be invoked such
// that the expression 'deleterInstance(objectPtr)' is a valid expression, and
// a "factory" deleter any language entity that can be invoked such that the
// expression 'deleterInstance.deleteObject(objectPtr)' is a valid expression,
// where 'deleterInstance' is an instance of the "deleter" object, and
// 'objectPtr' is a pointer to the managed object.  In short summary:
//..
//      Deleter                     Expression used to destroy 'objectPtr'
//      - - - -                     - - - - - - - - - - - - - - - - - - -
//      "function-like" deleters    deleterInstance(objectPtr);
//
//      "factory" deleters          deleterInstance.deleteObject(objectPtr);
//..
// The following are examples of function-like deleters:
//..
//  class my_Type {};
//  void deleteObject(my_Type *);
//  void releaseObject(my_Type *);
//
//  class FunctionLikeDeleterObject {
//    public:
//      void operator()(my_Type *);
//  };
//..
// while the following are examples of factory deleters:
//..
//  struct my_Factory {
//     my_Type *createObject();
//     void deleteObject(my_Type*);
//  };
//
//  struct my_Allocator : public bslma_Allocator { /* ... */ };
//..
// Note that 'deleteObject' is provided by all BDE allocators and by any object
// that implements the 'bcema_Deleter' protocol, thus any of these objects can
// be used as factory deleters.  The purpose of this design is to allow
// BDE-style allocators and factories to be used seamlessly as deleters.
//
// The selection of which expression used by 'bcema_SharedPtr' is based on how
// the deleter is passed to the shared pointer instance: deleters that are
// passed by *pointer* are assumed to be factory deleters, and those that are
// passed by *value* are assumed to function-like.  Note that if the wrong
// interface is used for a deleter, i.e., if a function-like deleter is passed
// by pointer, or a factory deleter is passed by value, and the expression used
// to delete the object is invalid, a compiler diagnostic will be emitted
// indicating the error.
//
// The following are examples of constructing shared pointers with the
// addresses of a factory deleters:
//..
//   my_Factory factory;
//   my_Type *myPtr1 = factory.createObject();
//   bcema_SharedPtr<my_Type> mySharedPtr1(myPtr1, &factory, 0);
//
//   bdema_SequentialAllocator alloc;
//   my_Type *myPtr2 = new(alloc) my_Type;
//   bcema_SharedPtr<my_Type> mySharedPtr2(myPtr1, &alloc);
//..
// Note that the deleters are passed *by address* in the above examples.
//
// The following are examples of constructing shared pointers with
// function-like deleters:
//..
//   my_Type *getObject();
//
//   my_Type *myPtr3 = getObject();
//   bcema_SharedPtr<my_Type> mySharedPtr3(myPtr3, &deleteObject, 0);
//
//   my_Type *myPtr4 = getObject();
//   FunctionLikeDeleterObject deleter;
//   bcema_SharedPtr<my_Type> mySharedPtr4(myPtr4, deleter, &alloc);
//..
// Note that 'deleteObject' is also passed by address, but 'deleter' is passed
// by value in the above examples.  Function-like deleter objects (passed by
// value) are stored by value in the representation and therefore *must* be
// copy-constructible.  Note that even though the deleter may be passed by a
// non-modifiable reference, it is its copy (owned by the shared pointer
// representation) that is invoked and thus the 'deleterInstance' is not
// required nor assumed to be non-modifiable.  (For the example above, note
// that 'operator()' is *not* defined 'const'.)
//
// Also note that the third argument (of type 'bslma_Allocator *') to the
// constructor of 'myOtherSharedPtr' is not optional as is otherwise the norm
// in this constructor form; you must always pass it, even if you want to use
// the default allocator (in which case you should pass 0, as above).  If you
// wish to know why, please consult the function-level documentation under the
// constructor with the signature 'bcema_SharedPtr(TYPE *ptr, DELETER deleter,
// bslma_Allocator *allocator)' below.
//
///Aliasing
///--------
// 'bcema_SharedPtr' supports a powerful "aliasing" feature.  That is, a
// shared pointer can be constructed with any arbitrary pointer value while
// referencing an existing shared pointer representation of any arbitrary
// type.  All references are applied to the "aliased" shared pointer
// representation, but the original representation is retained and used for
// reference counting and passed to the deleter upon destruction of the last
// instance of the shared pointer.  Consider the following snippet of code:
//..
//  class Event { /* ... */ };
//  void getEvents(bsl::list<Event> *list);
//
//  void enqueueEvents(bcec_Queue<bcema_SharedPtr<Event> > *queue)
//  {
//      bsl::list<Event> eventList;
//      getEvents(&eventList);
//      for (bsl::list<Event>::iterator it = eventList.begin();
//                                               it != eventList.end(); ++it) {
//          bcema_SharedPtr<Event> e;
//          e.createInplace(0, *it);  // Copy construct the event into a new
//                                    // shared ptr.
//          queue->pushBack(e);
//      }
//  }
//..
// In the above example, 'getEvents' loads into the provided 'bsl::list', a
// list of event objects.  The 'enqueueEvents' function constructs an empty
// list and calls getEvents to fill the list with Event objects.  Once the
// event list is filled, each event item is pushed as a shared pointer
// (presumably because events are "expensive" to construct and may be
// referenced simultaneously from multiple threads) onto the provided queue.
// Since the individual event items are contained by value within the list,
// pointers to them cannot be passed if it cannot be guaranteed that they will
// not live beyond the lifetime of the list itself.  Therefore an expensive
// copy operation is required to create individually managed instances of each
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
//                                   // all of the events
//      getEvents(eventList.ptr());
//
//      for (bsl::list<Event>::iterator it = eventList->begin();
//                                            it != eventList->end(); ++it) {
//          // Push each event onto the queue as an alias of the 'eventList'
//          // shared pointer.  When all the alias references have been
//          // released, the event list will be destroyed deleting all the
//          // events at once.
//
//          queue->pushBack(bcema_SharedPtr<Event>(eventList, &(*it)));
//      }
//  }
//..
// In this implementation, a single shared pointer to the whole list is
// constructed and filled with events.  A pointer to each individual item
// can then be safely pushed onto the queue as an alias of the shared pointer
// to the whole list.  When all aliased instances are released, the list
// will be destroyed.
//
///Comparison of shared pointers and conversion to 'bool'
///------------------------------------------------------
// This component does *not* allow comparisons of shared pointers, although it
// does allow a conversion to some "unspecified bool type" (which is *not*
// 'bool') so as to retain the 'comparison to 0' idiom:
//..
//  bcema_SharedPtr<int> a_sp(new int());
//  if (a_sp) {   // OK
//      // . . .
//  }
//..
// The following attempts:
//..
//  if (a_sp < b_sp) {  // ERROR
//      // . . .
//  }
//  bcema_SharedPtr<double> b_sp(new int());
//  if (a_sp < b_sp) {  // ERROR
//      // . . .
//  }
//..
// will both produce compilation errors, as will:
//..
//  bsl::map<bcema_SharedPtr<int>, int> sharedPtrMap;  // ERROR
//..
// (To be precise, the declaration of a map will not trigger the error, but the
// instantiation of any method that uses 'bsl::less' will.)
//
// In order to index a map by a shared pointer type, use the functor provided
// by the 'bcema_SharedPtrLess' class, as in:
//..
//  bsl::map<bcema_SharedPtr<int>, int, bcema_SharedPtrLess> sharedPtrMap;
//                                                     // OK
//..
// which will index the map by increasing 'ptr()' values of the shared pointer
// keys.
//
///Type Casting
///------------
// There are two ways to cast 'bcema_SharedPtr' of different types among each
// other, implicitly and explicitly.
//
///Implicit Casting
/// - - - - - - - -
// As with native pointers, a pointer of the type 'B' that is derived from the
// type 'A', can be directly assigned a 'bcema_SharedPtr' of 'A'.  In other
// words, if the following statements:
//..
//  class A { virtual void foo(); };  // polymorphic type
//  class B : public A {};
//  B *b_p = 0;
//  A *a_p = b_p;
//..
// are valid, then the following statements:
//..
//  bcema_SharedPtr<B> b_sp;
//  bcema_SharedPtr<A> a_sp;
//  a_sp = b_sp;
//..
// and
//..
//  bcema_SharedPtr<B> b_sp;
//  bcema_SharedPtr<A> a_sp(b_sp);
//..
// are also valid.  Note that in all of the above cases, the proper destructor
// of 'B' will be invoked when the instance is destroyed even if 'A' does not
// provide a virtual destructor.
//
///Explicit Casting
/// - - - - - - - -
// Through "aliasing", a shared pointer of any type can be explicitly cast
// to a shared pointer of any other type using any legal cast expression.  For
// example, to statically cast a shared pointer of type A to a shared pointer
// of type B, one can simply do the following:
//..
//  bcema_SharedPtr<A> a_sp;
//  bcema_SharedPtr<B> b_sp(a_sp, static_cast<B*>(a_sp.ptr()));
//..
// or even the less safe "C" style cast:
//..
//  bcema_SharedPtr<A> a_sp;
//  bcema_SharedPtr<B> b_sp(a_sp, (B*)(a_sp.ptr()));
//..
// For convenience, several utility functions are provided to perform common
// "C++" casts.  Dynamic casts, static casts, and const casts are all
// provided.  Explicit casting is supported through the 'bcema_SharedPtrUtil'
// utility.  The following example demonstrates dynamic casting a shared
// pointer of type 'A' to a shared pointer of type 'B':
//..
//  bcema_SharedPtr<A> a_sp(new A());
//  bcema_SharedPtr<B> b_sp = bcema_SharedPtrUtil::dynamicCast<B>(a_sp);
//  bcema_SharedPtr<B> b2_sp;
//  bcema_SharedPtrUtil::dynamicCast(&b2_sp, a_sp);
//  bcema_SharedPtr<B> b3_sp;
//  b3_sp = bcema_SharedPtrUtil::dynamicCast<B>(a_sp);
//..
// To test if the cast succeeded, simply test if the target shared pointer
// contains a non-null value (assuming the source was not null of course):
//..
//  if (b_sp) {
//     // The cast succeeded.
//  } else {
//     // The cast failed.
//  }
//..
// As previously stated, the managed instance will be destroyed correctly
// regardless of how it is cast.
//
///Converting to and from 'bdema_ManagedPtr'
///-----------------------------------------
// A 'bcema_SharedPtr' pointer can be converted directly to a
// 'bdema_ManagedPtr' pointer while still retaining proper reference counting.
// When a shared pointer is converted to a 'bdema_ManagedPtr', the number of
// references to the shared instance is incremented.  When the managed pointer
// is destroyed (if not transferred to another managed pointer first), the
// number of references will be decremented.  If the number of references
// reaches zero, then the managed instance will be destroyed.  The 'managedPtr'
// function can be used to create a managed pointer from a shared pointer.
//
// A 'bcema_SharedPtr' can be constructed directly from a 'bdema_ManagedPtr'.
// The resulting pointer is reference-counted and will use the deleter from
// the original 'bdema_ManagedPtr' to destroy the managed object.
//
///Storing a 'bcema_SharedPtr' to an invokable in a 'bdef_Function' instance
///-------------------------------------------------------------------------
// In addition to the guarantees already made in the 'bdef_function' component,
// 'bcema_sharedptr' also guarantees that storing a shared pointer to an
// invokable object in a 'bdef_Function' instance will be in-place, i.e., it
// will not trigger memory allocation.
//
///C++ Standard Compliance
///-----------------------
// This component provides a (subset of a) standard-compliant implementation of
// 'std::shared_ptr' (sections 2.2.3 of the final "Technical Report on C++
// Library Extensions" and 20.9.12.2 of the current "Final Committee Draft,
// Standard for Programming Language C++").  Namely, it does not provide or
// collaborate with 'std::weak_ptr', nor does it throw the 'std::bad_weak_ptr'
// exception, but it does accept conversion or assignment from 'bsl::auto_ptr'
// and provides the rest of the interface of 'std::shared_ptr'.  Also, it does
// not collaborate with types that derive from 'std::enable_shared_this': using
// 'bcema_SharedPtr' with such types will result in (very destructive)
// undefined behavior.  Global free functions except 'swap', such as the cast
// functions, 'get_deleter', 'make_shared' and 'allocate_shared' are not
// provided.
//
// As mentioned in the section "Comparison of Shared Pointers" above,
// the comparison operators 'operator<', 'operator<=', 'operator>' and
// 'operator>=', and the corresponding specializations for 'bsl::less',
// 'bsl::less_equal', 'bsl::greater', and 'bsl::greater_equal', will not
// compile, and there is a comparison functor in 'bcema_SharedPtrUtil' for
// use in place of 'bsl::less' in 'bsl::map' and other standard associative
// containers.
//
// Finally, this component only supports allocators that are of type
// 'bslma_Allocator *' and not the C++ Standard Allocators (section 20.1.2),
// for instance in the 'bcema_SharedPtr' class constructors, in the
// 'createInplace' methods of the same class, and in the 'allocate_shared'
// function templates.
//
// CAVEAT:  This implementation follows the BDE conventions on passing 0 as a
// 'bslma_Allocator *' argument.  A user who wishes to remain compliant with
// future C++ Standard specifications should not rely on this default allocator
// behavior.  In the future, not specifying an allocator may result in the use
// of '::operator delete' to destroy the managed object when all references
// have been released, in order to remain compliant with the standard
// 'std::shared_ptr'.
//
///Usage
///-----
// The following examples demonstrate various features and uses of shared
// pointers.
//
///Example 1 - Basic usage
///- - - - - - - - - - - -
// The following example demonstrates basic the instantiation of a shared
// pointer.  First, we need to declare the type that we wish to manage:
//..
//  class MyUser {
//      bsl::string d_name;
//      int         d_id;
//
//    public:
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
//      void setName(const bsl::string& name) { d_name = name; }
//
//      void setId(int id) { d_id = id; }
//
//      const bsl::string& name() const { return d_name; }
//
//      int id() const { return d_id; }
//  };
//..
// The 'createUser' utility function creates an instance of 'MyUser' using the
// provided allocator and returns a shared pointer to it.  Note that the
// shared pointer's internal representation will also be allocated using the
// same allocator.  Also note that if 'allocator' is 0, the
// currently-installed default allocator is used.
//..
//  bcema_SharedPtr<MyUser> createUser(bsl::string name, int id,
//                                     bslma_Allocator *allocator = 0)
//  {
//      allocator = bslma_Default::allocator(allocator);
//      MyUser *user = new (*allocator) MyUser(name, id);
//      return bcema_SharedPtr<MyUser>(user, allocator);
//  }
//..
// Since the 'createUser' function both allocates the object and creates the
// shared pointer, it can benefit from the in-place' facilities to avoid an
// extra allocation.  Again, note that the representation will also be
// allocated using the same allocator (see the section "Correct usage of the
// allocator model" above), and also note that if 'allocator' is 0, the
// currently-installed default allocator is used.
//..
//  bcema_SharedPtr<MyUser> createUser2(bsl::string name, int id,
//                                      bslma_Allocator *allocator = 0)
//  {
//      bcema_SharedPtr<MyUser> user;
//      user.createInplace(allocator, name, id, allocator);
//      return user;
//  }
//..
// Note that the shared pointer allocates both the reference count and the
// 'MyUser' in a single region of memory (which is the memory that will
// eventually be deallocated), but points to the 'MyUser' instance only.
//
///Using custom deleters
///- - - - - - - - - - -
// The following examples demonstrate the use of custom deleters with shared
// pointers.
//
///Example 2 - Nil deleters
///  -  -  -  -  -  -  -  -
// There are cases when an interface calls for an object to be passed as a
// shared pointer, but the object being passed is not owned by the caller
// (e.g., a pointer to a static variable).  In these cases, it is possible to
// create a shared pointer using a deleter that does not delete the object.
// The 'bcema_SharedPtrNilDeleter' is provided for this purpose.  The
// following example demonstrates the use of 'bcema_SharedPtr' using a
// 'bcema_SharedPtrNilDeleter'.  The code uses the 'MyUser' class defined in
// Example 1.  In this example, an asynchronous transaction manager is
// implemented.  Transactions are enqueued into the transaction manager to be
// processed at some later time.  The user associated with the transaction is
// passed as a shared pointer.  Transactions can originate from the "system"
// or from "users".
//
// We first declare the transaction manager and transaction info classes:
//..
//  struct MyTransactionInfo {
//      // Transaction Info...
//  };
//
//  class MyTransactionManager {
//      int enqueueTransaction(bcema_SharedPtr<MyUser>  user,
//                             const MyTransactionInfo& transaction);
//    public:
//      int enqueueSystemTransaction(const MyTransactionInfo& transaction);
//
//      int enqueueUserTransaction(const MyTransactionInfo& transaction,
//                                 bcema_SharedPtr<MyUser>  user);
//
//      static MyUser *systemUser();
//  };
//..
// The 'systemUser' method returns the same instance of 'MyUser' for all
// callers to use, therefore the returned object should not be destroyed:
//..
//  MyUser *MyTransactionManager::systemUser() {
//      static MyUser *systemUserSingleton;
//      if (!systemUserSingleton) {
//       // instantiate singleton in a thread-safe manner...
//      }
//      return systemUserSingleton;
//  }
//..
// For enqueuing user transactions, simply proxy the information to
// 'enqueueTransaction'.
//..
//  inline int MyTransactionManager::enqueueUserTransaction(
//                                  const MyTransactionInfo& transaction,
//                                  bcema_SharedPtr<MyUser>  user)
//  {
//      return enqueueTransaction(user, transaction);
//  }
//..
// For system transactions, we must use the 'MyUser' objected returned from
// the 'systemUser' static method.  Since we do not own the returned object,
// we cannot directly construct a 'bcema_SharedPtr' object for it:  doing so
// would result in the singleton being destroyed when the last reference to
// the shared pointer is released.  To solve this problem, we construct a
// 'bcema_SharedPtr' object for the system user using a nil deleter.  When
// the last reference to the shared pointer is released, the deleter will be
// invoked to destroy the object and will instead do nothing.
//..
// int MyTransactionManager::enqueueSystemTransaction(
//                                   const MyTransactionInfo& transaction)
// {
//     bcema_SharedPtr<MyUser> user(systemUser(),
//                                  bcema_SharedPtrNilDeleter(), 0);
//     return enqueueTransaction(user, transaction);
// }
//..
//
///Example 3 - Specialized deleters
/// -  -  -  -  -  -  -  -  -  -  -
// The role of the "deleter" is to allow users to provide a custom "cleanup"
// for a shared instance.  Although "cleanup" generally involves destroying
// the instance, this need not be the case.  The following example demonstrates
// the user of a custom deleter used to construct "locked" pointers.
// First declare a custom deleter that, when invoked, releases the specified
// mutex and signals the specified condition variable.
//..
//  class MyMutexUnlockAndBroadcastDeleter {
//      bcemt_Mutex     *d_mutex_p;
//      bcemt_Condition *d_cond_p;
//
//      MyMutexUnlockAndBroadcastDeleter(bcemt_Mutex     *mutex,
//                                       bcemt_Condition *cond)
//      : d_mutex_p(mutex)
//      , d_cond_p(cond)
//      {
//          d_mutex_p->lock();
//      }
//
//      MyMutexUnlockAndBroadcastDeleter(
//          MyMutexUnlockAndBroadcastDeleter& orig)
//      : d_mutex_p(orig.d_mutex_p)
//      , d_cond_p(orig.d_cond_p)
//      {
//      }
//..
// Since this deleter does not actually delete anything, 'void' is used,
// allowing it to be used with any type of object.
//..
//      void operator() (void *) {
//         if (d_cond_p) {
//             d_cond_p->broadcast();
//         }
//         d_mutex_p->unlock();
//      }
//  };
//..
// Next declare a thread-safe queue component.  The component uses a
// non-thread-safe 'bsl::deque' to implement the queue.  Thread-safe 'push'
// and 'pop' operations that push and pop individual items are provided.  For
// callers that wish to gain direct access to the queue, the 'queue' method
// returns a shared pointer to the queue using the
// 'MyMutexUnlockAndBroadcastDeleter'.  Callers can safely access the queue
// through the returned shared pointer.  Once the last reference to the pointer
// is released, the mutex will be unlocked and the condition variable will be
// signaled to allow waiting threads to re-evaluate the state of the queue.
//..
//  template<class TYPE>
//  class MySafeQueue {
//      bcemt_Mutex       d_mutex;
//      bcemt_Condition   d_cond;
//      bsl::deque<TYPE>  d_queue;
//
//    public:
//      void push(const TYPE& obj);
//
//      TYPE pop();
//
//      bcema_SharedPtr<bsl::deque<TYPE> > queue();
//  };
//
//  template<class TYPE>
//  void MySafeQueue<TYPE>::push(const TYPE& obj)
//  {
//      bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
//      d_queue.push_back(obj);
//      d_cond.signal();
//  }
//
//  template<class TYPE>
//  TYPE MySafeQueue<TYPE>::pop()
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
//  template<class TYPE>
//  bcema_SharedPtr<bsl::deque<TYPE> > MySafeQueue<TYPE>::queue()
//  {
//      return bcema_SharedPtr<bsl::deque<TYPE> >(
//                 &d_queue,
//                 MyMutexUnlockAndBroadcastDeleter(&d_mutex,
//                                                  &d_cond),
//                 0);
//  }
//..
//
///Implementation Hiding
///- - - - - - - - - - -
// 'bcema_SharedPtr' refers to the parameterized type "in name only".  This
// allows for the instantiation of shared pointers to incomplete or void types.
// This feature is useful for constructing interfaces where returning a pointer
// to a managed object is desirable, but in order to control access to the
// object its interface cannot be exposed.  The following examples demonstrate
// two techniques for achieving this goal using a 'bcema_SharedPtr'.
//
///Example 4 - Hidden interfaces
/// -  -  -  -  -  -  -  -  -  -
// Example 4 demonstrates the use of incomplete types to hide the interface of
// the 'MySession' pointer.  It begins by declaring the 'MySessionManager'
// component which allocates and manages instances of 'MySession' objects.  The
// interface(".h") merely forward declares the 'MySession'.  The actual
// definition of the interface is the implementation(".cpp") code.
//..
//    // Interface
//..
// Forward-declare 'MySession' to be used (in name only) in the definition of
// 'MySessionManager':
//..
//  class MySession;
//..
// Next, define the class 'MySessionManager':
//..
//  class MySessionManager {
//      typedef bsl::map<int, bcema_SharedPtr<MySession> > HandleMap;
//      bcemt_Mutex      d_mutex;
//      HandleMap        d_handles;
//      int              d_nextSessionId;
//      bslma_Allocator *d_allocator_p;
//    public:
//..
// It is useful to have a designated name for the 'bcema_SharedPtr' to
// 'MySession':
//..
//      typedef bcema_SharedPtr<MySession> MyHandle;
//..
// We only need a constructor to initialize the allocator:
//..
//      MySessionManager(bslma_Allocator *allocator = 0);
//..
// The following method constructs a new session object and returns a
// 'bcema_SharedPtr' to it.  Callers can transfer the pointer, but they cannot
// directly access the object since they do not know its interface.
//..
//      MyHandle openSession(const bsl::string& sessionName);
//
//      void closeSession(MyHandle handle);
//
//      bsl::string getSessionName(MyHandle handle) const;
//  };
//..
// Now, in the implementation of the code, we can define and implement
// the class 'MySession':
//..
//    // Implementation
//
//  class MySession {
//      bsl::string d_sessionName;
//      int         d_handleId;
//
//    public:
//      MySession(const bsl::string& sessionName, int handleId);
//
//      int handleId() const;
//
//      bsl::string sessionName() const;
//  };
//
//  MySession::MySession(const bsl::string& sessionName, int handleId)
//  : d_sessionName(sessionName)
//  , d_handleId(handleId)
//  {
//  }
//
//  inline int MySession::handleId() const
//  {
//      return d_handleId;
//  }
//
//  inline bsl::string MySession::sessionName() const
//  {
//      return d_sessionName;
//  }
//..
// The following shows the implementation of 'MySessionManager'.  Note that the
// interface for 'MySession' is now known so it may be accessed without error.
//..
//  MySessionManager::MySessionManager(bslma_Allocator *allocator)
//  : d_nextSessionId(1)
//  , d_allocator_p(bslma_Default::allocator(allocator))
//  {
//  }
//
//  MySessionManager::MyHandle
//  MySessionManager::openSession(const bsl::string& sessionName)
//  {
//      bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
//      MyHandle session(new(*d_allocator_p) MySession(sessionName,
//                                                     d_nextSessionId++));
//      d_handles[session->handleId()] = session;
//      return session;
//  }
//
//  void MySessionManager::closeSession(MyHandle handle)
//  {
//      bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
//      HandleMap::iterator it = d_handles.find(handle->handleId());
//      if (it != d_handles.end()) {
//          d_handles.erase(it);
//      }
//  }
//
//  bsl::string MySessionManager::getSessionName(MyHandle handle) const
//  {
//      return handle->sessionName();
//  }
//..
///Example 5 - Opaque types
///  -  -  -  -  -  -  -  -
// In the above example, users could infer that MyHandle is an actual pointer
// the SessionObject but have no way to directly access it since the
// interface was not exposed.  In the following example, 'MySessionManager'
// is re-implemented to provide an even more opaque session handle.  In this
// implementation, 'MyHandle' is redefined using a void type providing no
// indication of its implementation.  Note that using the void type will
// require casting in the implementation and therefore be a bit more expensive.
//..
//    // Interface
//..
// In the interface, define 'MySessionManager' as follows:
//..
//  class MySessionManager {
//      typedef bsl::map<int, bcema_SharedPtr<void> > HandleMap;
//      bcemt_Mutex      d_mutex;
//      HandleMap        d_handles;
//      int              d_nextSessionId;
//      bslma_Allocator *d_allocator_p;
//..
// It is useful to have a name for the void 'bcema_SharedPtr' handle.
//..
//     public:
//      typedef bcema_SharedPtr<void> MyHandle;
//
//      MySessionManager(bslma_Allocator *allocator = 0);
//
//      MyHandle openSession(const bsl::string& sessionName);
//
//      void closeSession(MyHandle handle);
//
//      bsl::string getSessionName(MyHandle handle) const;
//  };
//..
// In the implementation, we define the methods of 'MySessionManager':
//..
//    // Implementation
//
//  MySessionManager::MySessionManager(bslma_Allocator *allocator)
//  : d_nextSessionId(1)
//  , d_allocator_p(bslma_Default::allocator(allocator))
//  {
//  }
//
//  MySessionManager::MyHandle
//  MySessionManager::openSession(const bsl::string& sessionName)
//  {
//      bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
//..
// Notice that 'MyHandle', which is a 'void' pointer, can be transparently
// assigned a pointer to a 'MySession' object.  This is because the
// 'bcema_SharedPtr' interface allows any pointer that is "naturally"
// castable to be assigned directly.
//..
//      MyHandle session(new(*d_allocator_p) MySession(sessionName,
//                                                     d_nextSessionId++));
//      bcema_SharedPtr<MySession> myhandle =
//                         bcema_SharedPtrUtil::staticCast<MySession>(session);
//      d_handles[myhandle->handleId()] = session;
//      return session;
//  }
//
//  void MySessionManager::closeSession(MyHandle handle)
//  {
//      bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
//..
// Perform a static cast from 'bcema_SharedPtr<void>' to
// 'bcema_SharedPtr<MySession>'.
//..
//      bcema_SharedPtr<MySession> myhandle =
//                          bcema_SharedPtrUtil::staticCast<MySession>(handle);
//..
// Test to make sure that the pointer was non-null before using 'myhandle':
//..
//      if (!myhandle.ptr()) {
//          return;
//      }
//
//      HandleMap::iterator it = d_handles.find(myhandle->handleId());
//      if (it != d_handles.end()) {
//          d_handles.erase(it);
//      }
//  }
//
//  bsl::string MySessionManager::getSessionName(MyHandle handle) const
//  {
//      bcema_SharedPtr<MySession> myhandle =
//                          bcema_SharedPtrUtil::staticCast<MySession>(handle);
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

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BDEMA_MANAGEDPTR
#include <bdema_managedptr.h>
#endif

#ifndef INCLUDED_BSLALG_HASTRAIT
#include <bslalg_hastrait.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS
#include <bslmf_functionpointertraits.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>       // for 'bsl::swap'
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>      // for 'bsl::binary_function'
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>          // for 'bsl::auto_ptr'
#endif

#ifndef INCLUDED_BSL_TYPEINFO
#include <bsl_typeinfo.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>               // for 'bsl::ptrdiff_t', 'bsl::size_t'
#endif

namespace BloombergLP {

template <class TYPE, class DELETER> class bcema_SharedPtr_RepImpl;
template <class TYPE> class bcema_SharedPtr_InplaceRepImpl;
template <class TYPE> struct bcema_SharedPtr_ReferenceType;
template <class TYPE, class DELETER> struct bcema_SharedPtr_InitGuard;

                        // ========================
                        // class bcema_SharedPtrRep
                        // ========================

class bcema_SharedPtrRep {
    // Partially implemented shared pointer representation ("letter") protocol.
    // This class provides a reference counter and a concrete implementation of
    // the 'bcema_Deleter' protocol that decrements the number references and
    // destroys itself if the number of references reaches zero.

    // DATA
    bces_AtomicInt d_count;  // reference counter

  protected:
    // CREATORS
    virtual ~bcema_SharedPtrRep();
        // Destroy this representation object.

  public:
    // CLASS METHODS
    static void managedPtrDeleter(void *, bcema_SharedPtrRep *rep);
        // This function is used as a deleter when converting from a
        // 'bcema_SharedPtr' to a 'bdema_ManagedPtr'.  It decrements the
        // reference count of the specified 'rep' and destroys it if the number
        // of references reaches zero.  Note that the first argument (the
        // current pointer value of this shared pointer representation) is
        // ignored.

    // CREATORS
    bcema_SharedPtrRep();
        // Construct a 'bcema_SharedPtrRep' object with an initial reference
        // count of one.

    // MANIPULATORS
    void incrementRefs(int incrementAmount = 1);
        // Increment the number of references to this representation by the
        // optionally specified 'incrementAmount'.  If 'incrementAmount' is not
        // specified, the number of references is incremented by 1.  The
        // behavior is undefined unless 'incrementAmount > 0'.

    int decrementRefs(int decrementAmount = 1);
        // Decrement the number of references to this representation by the
        // optionally specified 'decrementAmount' and return the updated number
        // of references.  If 'decrementAmount' is not specified, the number of
        // references is decremented by 1.  The behavior is undefined unless
        // 'decrementAmount > 0'.

    virtual void release() = 0;
        // Release this representation object.  This method is invoked when the
        // number of references reaches zero (i.e., when a call to
        // 'decrementRefs' returns zero) to dispose of this representation
        // object.  This virtual 'release' method effectively serves as the
        // destructor and is expected to be overridden in all the (directly or
        // indirectly) derived classes.  Derived classes should override this
        // method to perform actions such as deleting this representation or
        // returning it to an object pool.  Note that this class's destructor
        // is protected and although virtual, is not called by derived classes
        // in a virtual context, hence inlining is appropriate in this case.

    // ACCESSORS
    virtual void *originalPtr() const = 0;
        // Return the (untyped) address of the object managed by this
        // representation.  Must be overridden by the derived implementation.

    int numReferences() const;
        // Return a "snapshot" of the current number of references to this
        // representation object.
};

                        // ====================================
                        // struct bcema_SharedPtr_ReferenceType
                        // ====================================

template <class TYPE>
struct bcema_SharedPtr_ReferenceType {
    // This class defines some basic traits used by bcema_SharedPtr.
    // It is primarily used to allow shared pointers of type 'void'
    // to work properly.

    typedef TYPE& Reference;
};

template <>
struct bcema_SharedPtr_ReferenceType<void> {
    // This specialization of 'bcema_SharedPtr_ReferenceType' for type 'void'
    // allows to avoid declaring a reference to 'void'.

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
    // having a conversion to 'bool' or being less-than comparable (either of
    // which would also enable undesirable implicit comparisons of shared
    // pointers to 'int' and less-than comparisons).

                        // =====================
                        // class bcema_SharedPtr
                        // =====================

template <class TYPE>
class bcema_SharedPtr {
    // This class provides a thread-safe reference-counted "smart pointer" to
    // support "shared ownership" of objects: a shared pointer ensures that the
    // object that it manages is destroyed only when there are no longer any
    // references to it, and using the appropriate deletion method.  The object
    // (of the parameterized 'TYPE') pointed to by a shared pointer instance
    // may be accessed directly using the '->' operator, or the dereference
    // operator (operator '*') can be used to get a reference to that object.
    // See the component-level documentation for a thorough description and
    // examples of its many usages.
    //
    // Note that the object managed by a shared pointer instance is usually the
    // same as the object pointed to by that instance (of the same 'TYPE'), but
    // this need not always be true in the presence of conversions or
    // "aliasing": the object pointed-to, of the parameterized 'TYPE', may
    // differ from the managed object of type 'BCE_OTHER_TYPE' (see the section
    // "Aliasing" in the component-level documentation).  Nevertheless, both
    // shall exist or else the shared pointer is *empty* (i.e., manages no
    // object, has no deleter, and points to 0).
    //
    // More generally, this class supports a complete set of *in*-*core*
    // pointer semantic operations. . .  TBD (Exception guarantees, aliasing
    // guarantees - not to be confused with pointer aliasing, "In no event is
    // memory leaked", etc.)

    // DATA
    TYPE               *d_ptr_p;  // pointer to the shared instance

    bcema_SharedPtrRep *d_rep_p;  // pointer to the representation object
                                  // that manages the shared instance

    // PRIVATE TYPES
    typedef bcema_SharedPtr<TYPE>                                    SelfType;

    // PRIVATE MANIPULATORS
    template <class BCE_OTHER_TYPE, class DELETER>
    void makeRep(BCE_OTHER_TYPE  *ptr,
                 const DELETER&   deleter,
                 bslma_Allocator *allocator);
        // Create a representation managing the object of the parameterized
        // 'BCE_OTHER_TYPE' at the specified 'ptr' address, using a copy of the
        // specified 'deleter' to destroy the specified 'ptr' when the
        // representation is destroyed, and reset this shared pointer to use
        // this representation and point to the object of the parameterized
        // 'TYPE' at the 'ptr' address (after converting 'ptr' to the type
        // 'TYPE *').  Use the specified 'allocator' to supply memory for the
        // representation object.

    // FRIENDS
    template <class BCE_OTHER_TYPE> friend class bcema_SharedPtr;

  public:
    // TYPES
    typedef TYPE                 ElementType;
        // 'ElementType' is an alias to the parameterized 'TYPE' passed as
        // first and only template parameter to the 'bcema_SharedPtr' class
        // template.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(bcema_SharedPtr,
                                  bslalg_TypeTraitHasPointerSemantics,
                                  bslalg_TypeTraitBitwiseMoveable);

    // CREATORS
    bcema_SharedPtr();
        // Create an empty shared pointer, i.e., a shared pointer with no
        // representation, that does not manage any object and has no deleter,
        // and that has a null pointer value.

    explicit bcema_SharedPtr(TYPE *ptr, bslma_Allocator *allocator = 0);
        // Create a shared pointer that points to and manages the object at the
        // specified 'ptr' address.  Optionally specify an 'allocator' to
        // allocate and deallocate the internal representation of the shared
        // pointer and to destroy the managed object when all references have
        // been released.  If 'allocator' is 0, the currently installed default
        // allocator is used (but see the "CAVEAT" in the section "C++ Standard
        // Compliance" of the component-level documentation).  Note that if
        // 'ptr' is 0, this shared pointer is initialized to an empty state and
        // 'allocator' is ignored.

    template <class BCE_OTHER_TYPE>
    explicit bcema_SharedPtr(BCE_OTHER_TYPE  *ptr,
                             bslma_Allocator *allocator = 0);
        // Create a shared pointer that manages the object of the parameterized
        // 'BCE_OTHER_TYPE' at the specified 'ptr' address and points to
        // '(TYPE *)ptr'.  Optionally specify an 'allocator' used to allocate
        // and deallocate the internal representation of this shared pointer
        // and to destroy the managed object when all references have been
        // released.  If 'allocator' is 0, the currently installed default
        // allocator is used (but see the "CAVEAT" in the section "C++ Standard
        // Compliance" of the component-level documentation).  If
        // 'BCE_OTHER_TYPE *' is not implicitly convertible to 'TYPE *' then a
        // compiler diagnostic will be emitted indicating the error.  Note
        // that if 'ptr' is 0, this shared pointer is initialized to an empty
        // state and 'allocator' is ignored.

    template <class BCE_OTHER_TYPE>
    bcema_SharedPtr(bdema_ManagedPtr<BCE_OTHER_TYPE>  managedPtr,
                    bslma_Allocator                  *allocator = 0);
        // Create a shared pointer that takes over the management of the object
        // (if any) previously managed by the specified 'managedPtr' to the
        // parameterized 'BCE_OTHER_TYPE', and that points to the object at the
        // address '(TYPE *)managedPtr.ptr()'.  The deleter used in the
        // 'managedPtr' will be used to destroy the managed object when all
        // references have been released.  Optionally specify an 'allocator'
        // used to allocate and deallocate the internal representation of the
        // shared pointer.  If 'allocator' is 0, the currently installed
        // default allocator is used.  If 'BCE_OTHER_TYPE *' is not implicitly
        // convertible to 'TYPE *' then a compiler diagnostic will be emitted
        // indicating the error.  Note that if 'managedPtr' is empty, then this
        // shared pointer is initialized to the empty state.

    template <class BCE_OTHER_TYPE>
    explicit bcema_SharedPtr(bsl::auto_ptr<BCE_OTHER_TYPE>&  autoPtr,
                             bslma_Allocator                *allocator = 0);
        // Create a shared pointer that takes over the management of the
        // object previously managed by the specified 'autoPtr' to the
        // parameterized 'BCE_OTHER_TYPE', and that points to the object at the
        // address '(TYPE *)autoPtr.get()'.  The global '::operator delete'
        // will be used to destroy the managed object when all references have
        // been released.  Optionally specify an 'allocator' used to allocate
        // and deallocate the internal representation of the shared pointer.
        // If 'allocator' is 0, the currently installed default allocator is
        // used.  If 'BCE_OTHER_TYPE *' is not implicitly convertible to
        // 'TYPE *' then a compiler diagnostic will be emitted indicating the
        // error.

    template <class BCE_OTHER_TYPE, class DELETER>
    bcema_SharedPtr(BCE_OTHER_TYPE  *ptr,
                    const DELETER&   deleter,
                    bslma_Allocator *allocator);
        // Create a shared pointer that manages the object of the parameterized
        // 'BCE_OTHER_TYPE' at the specified 'ptr' address and that points to
        // '(TYPE *)ptr', using the specified 'deleter' to delete the managed
        // object when all references have been released and the specified
        // 'allocator' to allocate and deallocate the internal representation
        // of the shared pointer.  If 'allocator' is 0, the currently installed
        // default allocator is used.  If 'DELETER' is a reference type, then
        // 'deleter' is assumed to be a function-like deleter that may be
        // invoked to destroy the object pointed-to by a single argument of
        // type 'BCE_OTHER_TYPE *' (i.e., 'deleter(ptr)' will be called to
        // destroy the managed object).  If 'DELETER' is a pointer type, then
        // 'deleter' is assumed to be a pointer to a factory object which
        // exposes a 'deleteObject(BCE_OTHER_TYPE *)' interface that will be
        // invoked to destroy the object at the 'ptr' address (i.e.,
        // 'deleter->deleteObject(ptr)' will be called to delete the managed
        // object).  (See the section "Deleters" in the component-level
        // documentation.)  If 'BCE_OTHER_TYPE *' is not implicitly convertible
        // to 'TYPE *' then a compiler diagnostic will be emitted indicating
        // the error.  Note that, for factory deleters, the specified 'deleter'
        // must remain valid until all references to 'ptr' have been released.
        // Also note that 'allocator' is not optional in this constructor form.
        // The reason is to avoid hiding the two-argument constructor:
        //..
        //  bcema_SharedPtr(BCE_OTHER_TYPE  *ptr,
        //                  bslma_Allocator *allocator = 0)
        //..
        // defined above.  Allowing the third argument to this constructor to
        // be optional causes it to be the best match for a constructor
        // invocation intended to match the preceding one.  Therefore, one must
        // always specify the allocator argument.  (Of course, one can
        // explicitly pass the 0 pointer to use the default allocator).  Note
        // that if 'ptr' is 0, this shared pointer is initialized to an empty
        // state and both 'deleter' and 'allocator' are ignored.

    template <class BCE_OTHER_TYPE>
    bcema_SharedPtr(bcema_SharedPtr<BCE_OTHER_TYPE> const&  source,
                    TYPE                                   *object);
        // Create a shared pointer that manages the same object (if any) as the
        // specified 'source' shared pointer to the parameterized
        // 'BCE_OTHER_TYPE', and that points to the object at the specified
        // 'object' address.  The resulting shared pointer is known as an
        // "alias" of 'source'.  Typically, the objects pointed to by 'source'
        // and 'object' have identical lifetimes (e.g., one might be a part of
        // the other) so that the deleter for 'source' will destroy them both,
        // but do not necessarily have the same type.  Note that if either
        // 'source' is empty or 'object' is null, then this shared pointer is
        // initialized to the empty state.

    explicit
    bcema_SharedPtr(bcema_SharedPtrRep *rep);
        // Construct a shared pointer taking ownership of the specified 'rep'
        // and pointing to the object stored in 'rep'.  Note that this method
        // *DOES* *NOT* increment the number of references to 'rep'.

    bcema_SharedPtr(TYPE *ptr, bcema_SharedPtrRep *rep);
        // Construct a shared pointer taking ownership of the specified 'rep'
        // and pointing to the object at the specified 'ptr' address.  Note
        // that this method *DOES* *NOT* increment the number of references to
        // 'rep'.

    bcema_SharedPtr(bcema_SharedPtr<TYPE> const& original);
        // Create a shared pointer that manages the same object (if any) as the
        // specified 'original' shared pointer, using the same deleter as the
        // 'original' to destroy the managed object, and that points to the
        // same object as 'original'.  Note that if 'original' is empty, then
        // this shared pointer will be initialized to the empty state.

    template <class BCE_OTHER_TYPE>
    bcema_SharedPtr(bcema_SharedPtr<BCE_OTHER_TYPE> const& original);
        // Create a shared pointer that manages the same object (if any) as the
        // specified 'original' shared pointer to the parameterized
        // 'BCE_OTHER_TYPE', using the same deleter as the 'original' to
        // destroy the managed object, and that points to the object at the
        // address '(TYPE *)original.ptr()'.  If 'BCE_OTHER_TYPE *' is not
        // implicitly convertible to 'TYPE *' then a compiler diagnostic will
        // be emitted indicating the error.  Note that if 'original' is empty,
        // then this shared pointer is initialized to the empty state.

    ~bcema_SharedPtr();
        // Destroy this shared pointer object.  If this shared pointer manages
        // a (possibly shared) object, then release the reference to the
        // managed object, calling the deleter to destroy the managed object if
        // this shared pointer is the last reference.

    // MANIPULATORS
    bcema_SharedPtr<TYPE>& operator=(const bcema_SharedPtr<TYPE>& rhs);
        // Make this shared pointer manage the same object as the specified
        // 'rhs' shared pointer, using the same deleter as 'rhs', pointing to
        // the same object as 'rhs', and return a reference to '*this'.  If
        // this shared pointer is already managing a (possibly shared) object,
        // then release the reference to the managed object, calling the
        // deleter to destroy the managed object if this pointer is the last
        // reference.  Note that if 'rhs' is empty, then this shared pointer
        // will also be empty after the assignment.  Also note that if '*this'
        // is the same object as 'rhs', then assignment has no effect.

    template <class BCE_OTHER_TYPE>
    bcema_SharedPtr<TYPE>&
    operator=(const bcema_SharedPtr<BCE_OTHER_TYPE>& rhs);
        // Make this shared pointer manage the same object as the specified
        // 'rhs' shared pointer to the parameterized 'BCE_OTHER_TYPE', using
        // the same deleter as 'rhs', pointing to the address
        // '(TYPE *)rhs.ptr()', and return a reference to '*this'.  If this
        // shared pointer is already managing a (possibly shared) object, then
        // release the reference to the managed object, calling the deleter to
        // destroy the managed object if this pointer is the last reference.
        // Note that if 'rhs' is empty, then this shared pointer will also be
        // empty after the assignment.

    template <class BCE_OTHER_TYPE>
    bcema_SharedPtr<TYPE>& operator=(bsl::auto_ptr<BCE_OTHER_TYPE>& rhs);
        // Transfer ownership to this shared pointer of the object managed by
        // the 'rhs' auto pointer to the parameterized 'BCE_OTHER_TYPE', using
        // '::operator delete' to destroy the managed object when the last
        // reference is released, and make this shared pointer point to the
        // address '(TYPE *)rhs.get()'.  If this shared pointer is already
        // managing a (possibly shared) object, then release the reference to
        // the managed object, calling the deleter to destroy the managed
        // object if this pointer is the last reference.  Note that if 'rhs' is
        // null, then this shared pointer will also be empty after the
        // assignment.

    void clear();
        // Reset this shared pointer to an empty state.  If this shared pointer
        // is already managing a (possibly shared) object, then release the
        // reference to the managed object, calling the deleter to destroy the
        // managed object if this pointer is the last reference.

    template <class BCE_OTHER_TYPE>
    void load(BCE_OTHER_TYPE *ptr, bslma_Allocator *allocator = 0);
        // Modify this shared pointer to manage the object of the parameterized
        // 'BCE_OTHER_TYPE' at the specified 'ptr' address and point to
        // '(TYPE *)ptr'.  If this shared pointer is already managing a
        // (possibly shared) object, then release the reference to the managed
        // object, calling the deleter to destroy the managed object if this
        // pointer is the last reference.  Optionally specify an 'allocator'
        // used to allocate and deallocate the internal representation of this
        // shared pointer and to destroy the managed object when all references
        // have been released.  If 'allocator' is 0, the currently installed
        // default allocator is used (but see the "CAVEAT" in the section "C++
        // Standard Compliance" of the component-level documentation).  If
        // 'BCE_OTHER_TYPE *' is not implicitly convertible to 'TYPE *' then a
        // compiler diagnostic will be emitted indicating the error.  Note that
        // if 'ptr' is 0, this shared pointer is initialized to the empty state
        // and 'allocator' is ignored.

    template <class BCE_OTHER_TYPE, class DELETER>
    void load(BCE_OTHER_TYPE  *ptr,
              const DELETER&   deleter,
              bslma_Allocator *allocator);
        // Modify this shared pointer to manage the object of the parameterized
        // 'BCE_OTHER_TYPE' at the specified 'ptr' address and to point to
        // '(TYPE *)ptr', using the specified 'deleter' to delete the managed
        // object when all references have been released and the specified
        // 'allocator' to allocate and deallocate the internal representation
        // of the shared pointer.  If 'allocator' is 0, the currently installed
        // default allocator is used.  If this shared pointer is already
        // managing a (possibly shared) object, then release the reference to
        // the managed object, calling the deleter to destroy the managed
        // object if this pointer is the last reference.  If 'DELETER' is a
        // reference type, then 'deleter' is assumed to be a function-like
        // deleter that may be invoked to destroy the object pointed-to by a
        // single argument of type 'BCE_OTHER_TYPE *' (i.e., 'deleter(ptr)'
        // will be called to destroy the managed object).  If 'DELETER' is a
        // pointer type, then 'deleter' is assumed to be a pointer to a factory
        // object which exposes a 'deleteObject(BCE_OTHER_TYPE *)' interface
        // that will be invoked to destroy the object at the 'ptr' address
        // (i.e., 'deleter->deleteObject(ptr)' will be called to delete the
        // managed object).  (See the section "Deleters" in the component-level
        // documentation.)  If 'BCE_OTHER_TYPE *' is not implicitly convertible
        // to 'TYPE *' then a compiler diagnostic will be emitted indicating
        // the error.  Note that, for factory deleters, the specified 'deleter'
        // must remain valid until all references to 'ptr' have been released.
        // Note that if 'ptr' is 0, this shared pointer is initialized to the
        // empty state and both 'deleter' and 'allocator' are ignored.
        // Finally, note that this function is logically equivalent to:
        //..
        //  *this = bcema_SharedPtr<TYPE>(ptr, deleter, allocator);
        //..
        // and that, for the same reasons as explained in that constructor, the
        // third 'allocator' argument is not optional.

    template <class BCE_OTHER_TYPE>
    void loadAlias(bcema_SharedPtr<BCE_OTHER_TYPE> const&  source,
                   TYPE                                   *object);
        // Modify this shared pointer to manage the same object (if any) as the
        // specified 'source' shared pointer to the parameterized
        // 'BCE_OTHER_TYPE', and point to the object at the specified 'object'
        // address (i.e., make this shared pointer an "alias" of 'source').  If
        // this shared pointer is already managing a (possibly shared) object,
        // then release the reference to the managed object, calling the
        // deleter to destroy the managed object if this pointer is the last
        // reference.  Typically, the objects pointed to by 'source' and
        // 'object' have identical lifetimes (e.g., one might be a part of the
        // other) so that the deleter for 'source' will destroy them both, but
        // do not necessarily have the same type.  Note that if either 'source'
        // is unset or 'object' is null, then this shared pointer will be
        // initialized to the empty state.  Also note that this function is
        // logically equivalent to:
        //..
        //  *this = bcema_SharedPtr<TYPE>(source, object)
        //..

    void createInplace(bslma_Allocator *allocator = 0);
        // Create "in-place" in a large enough contiguous memory region both an
        // internal representation for this shared pointer and a
        // default-constructed instance of 'TYPE', and make this shared pointer
        // point to the newly created 'TYPE' instance.  Optionally specify an
        // 'allocator' used to supply memory.  If 'allocator' is 0, the
        // currently installed default allocator is used.  If an exception is
        // thrown during allocation or construction of 'TYPE', this shared
        // pointer will be unchanged.  Otherwise, if this shared pointer is
        // already managing a (possibly shared) object, then release the
        // reference to the managed object, calling the deleter to destroy the
        // managed object if this pointer is the last reference.  Note that the
        // allocator argument is *not* passed to the constructor for 'TYPE'.
        // To construct an in-place 'TYPE' with an allocator, use one of the
        // other variants of 'createInplace', below.

    template <class A1>
    void createInplace(bslma_Allocator *allocator, const A1& a1);
    template <class A1, class A2>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2);
    template <class A1, class A2, class A3>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3);
    template <class A1, class A2, class A3, class A4>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4);
    template <class A1, class A2, class A3, class A4, class A5>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5);
    template <class A1, class A2, class A3, class A4, class A5, class A6>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8, const A9& a9);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8, const A9& a9, const A10& a10);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8, const A9& a9, const A10& a10,
                       const A11& a11);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8, const A9& a9, const A10& a10,
                       const A11& a11, const A12& a12);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12,
              class A13>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8, const A9& a9, const A10& a10,
                       const A11& a11, const A12& a12, const A13& a13);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12,
              class A13, class A14>
    void createInplace(bslma_Allocator *allocator, const A1& a1,
                       const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7,
                       const A8& a8, const A9& a9, const A10& a10,
                       const A11& a11, const A12& a12, const A13& a13,
                       const A14& a14);
        // Create "in-place" in a large enough contiguous memory region both an
        // internal representation for this shared pointer and an instance of
        // 'TYPE' using the 'TYPE' constructor that takes the specified
        // arguments 'a1' up to 'aN' where 'N' (currently at most 14) is the
        // number of arguments passed to this method, and make this shared
        // pointer point to the newly created 'TYPE' instance.  Optionally
        // specify an 'allocator' used to supply memory.  If 'allocator' is 0,
        // the currently installed default allocator is used.  If an exception
        // is thrown during the construction of 'TYPE', this shared pointer
        // will be unchanged.  Otherwise, if this shared pointer is already
        // managing a (possibly shared) object, then release the reference to
        // the managed object, calling the deleter to destroy the managed
        // object if this pointer is the last reference.  Note that the
        // allocator argument is *not* implicitly passed to the constructor for
        // 'TYPE'.  To construct an instance of 'TYPE' with an allocator, pass
        // the allocator as one of the arguments (typically the last argument).

    bsl::pair<TYPE*, bcema_SharedPtrRep*> release();
        // Return the pair of addresses of the 'TYPE' instance pointed to and
        // the representation shared by this shared pointer, and reset this
        // shared pointer to an empty value with no effect on the
        // representation.  Note that in particular, the reference counter is
        // not modified nor is the managed object deleted; if the reference
        // count of this representation is greater than one, then it is not
        // safe to release the representation (thereby destroying the
        // managed object), but it is always safe to create another shared
        // pointer with this representation using the constructor with the
        // signature 'bcema_SharedPtr(TYPE *ptr, bcema_SharedPtrRep *rep)'.
        // Also note that this function returns a pair of null pointers if this
        // shared pointer is empty.

    void swap(bcema_SharedPtr<TYPE>& src);
        // Efficiently exchange the states of this shared pointer and the
        // specified 'src' shared pointer such that each will point to the
        // object formerly pointed to by the other and each will manage the
        // object formerly managed by the other.

    // ACCESSORS
    operator bcema_SharedPtr_UnspecifiedBool() const;
        // Return a value of the "unspecified bool" that evaluates to 'false'
        // if this shared pointer is in an unset state, and 'true' otherwise.
        // Note that this conversion operator allows a shared pointer to be
        // used within a conditional context (e.g., within an 'if' or 'while'
        // statement), but does *not* allow shared pointers to be compared
        // (e.g., via '<' or '>').

    typename bcema_SharedPtr_ReferenceType<TYPE>::Reference
    operator[](bsl::ptrdiff_t index) const;
        // Return a reference to the instance at the specified 'index' offset
        // of the object of the parameterized 'TYPE' pointed to by this shared
        // pointer.  The behavior is undefined if this shared pointer is empty,
        // or if 'TYPE' is 'void' (a compiler error will be generated if this
        // operator is instantiated within the 'bcema_SharedPtr<void>' class).
        // Note that this is logically equivalent to '*(ptr() + index)'.

    typename bcema_SharedPtr_ReferenceType<TYPE>::Reference
    operator*() const;
        // Return a reference to the object pointed to by this shared pointer.
        // The behavior is undefined if this shared pointer is empty, or if
        // 'TYPE' is 'void' (a compiler error will be generated if this
        // operator is instantiated within the 'bcema_SharedPtr<void>' class).

    TYPE *operator->() const;
        // Return the address of the object pointed to by this shared pointer,
        // or 0 if this shared pointer is empty.  Note that applying this
        // operator conventionally (e.g., to invoke a method) to an empty
        // shared pointer will result in undefined behavior.

    TYPE *ptr() const;
        // Return the address of the 'TYPE' instance pointed to by this shared
        // pointer, or 0 if this shared pointer is empty.

    bcema_SharedPtrRep *rep() const;
        // Return the address of the modifiable 'bcema_SharedPtrRep' instance
        // used by this shared pointer, or 0 if the shared pointer is empty.

    int numReferences() const;
        // Return a "snapshot" of the number of shared pointers (including this
        // one) that share ownership of the object managed by this shared
        // pointer.

    bdema_ManagedPtr<TYPE> managedPtr() const;
        // Return a managed pointer that points to the same object as this
        // shared pointer and which has a deleter that decrements the
        // reference count for the managed object.

                        // *** std::tr1 COMPATIBILITY ***
    // TYPES
    typedef TYPE  element_type;
        // Equivalent to 'ElementType'.  See 'ElementType'.

    // MANIPULATORS
    void reset();
        // Equivalent to 'clear'.  See 'clear'.

    template <class BCE_OTHER_TYPE>
    void reset(BCE_OTHER_TYPE *ptr);
        // Equivalent to 'load(ptr)'.  See 'load'.

    template <class BCE_OTHER_TYPE, class DELETER>
    void reset(BCE_OTHER_TYPE *ptr, const DELETER& deleter);
        // Equivalent to 'load(ptr, deleter, (bslma_Allocator*)0)'.  See
        // 'load'.

    template <class BCE_OTHER_TYPE>
    void reset(bcema_SharedPtr<BCE_OTHER_TYPE> const& source, TYPE *ptr);
        // Equivalent to 'loadAlias(source, ptr)'.  See 'loadAlias'.

    // ACCESSORS
    TYPE *get() const;
        // Return the address of the 'TYPE' instance pointed to by this shared
        // pointer, or 0 if this shared pointer is empty.  See 'ptr'.

    bool unique() const;
        // Return 'true' if this shared pointer does not share ownership of its
        // managed object with any other shared pointer, and 'false' otherwise.
        // See 'numReferences'.

    int  use_count() const;
        // Return a "snapshot" of the number of shared pointers (including this
        // one) that share ownership of the object managed by this shared
        // pointer.  See 'numReferences'.
};

// FREE OPERATORS
template <typename LHS_TYPE, typename RHS_TYPE>
bool operator==(const bcema_SharedPtr<LHS_TYPE>& lhs,
                const bcema_SharedPtr<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' has the same value as the specified
    // 'rhs', and 'false' otherwise.  Two shared pointers have the same value
    // if they point to the same address.  Note that if a (raw) pointer of the
    // parameterized type 'LHS_TYPE' cannot be compared to a (raw) pointer of
    // the parameterized type 'RHS_TYPE', then a compiler diagnostic will be
    // emitted indicating the error.  Note that two equal shared pointers do
    // not necessarily manage the same object.

template <typename LHS_TYPE, typename RHS_TYPE>
bool operator!=(const bcema_SharedPtr<LHS_TYPE>& lhs,
                const bcema_SharedPtr<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' does not have the same value as the
    // specified 'rhs', and 'false' otherwise.  Two shared pointers do not have
    // the same value if they point to different addresses.  Note that if a
    // (raw) pointer of the parameterized type 'LHS_TYPE' cannot be compared
    // to a (raw) pointer of the parameterized type 'RHS_TYPE', then a
    // compiler diagnostic will be emitted indicating the error.  Note that two
    // equal shared pointers do not necessarily manage the same object.

template <typename TYPE>
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bcema_SharedPtr<TYPE>& rhs);
    // Print the pointer value of the specified 'rhs' shared pointer to the
    // specified 'stream' and return a reference to the modifiable 'stream'.

                        // *** std::tr1 COMPATIBILITY ***

template <typename TYPE>
void swap(bcema_SharedPtr<TYPE>& lhs, bcema_SharedPtr<TYPE>& rhs);
    // Efficiently exchange the states of the specified 'lhs' shared pointer
    // and the specified 'rhs' shared pointer such that each will point to the
    // object formerly pointed to by the other and each will manage the object
    // formerly managed by the other.

                        // ==========================
                        // struct bcema_SharedPtrLess
                        // ==========================

struct bcema_SharedPtrLess {
    // This class template provides a functor comparing two shared pointers to
    // the parameterized 'TYPE' suitable for use in the standard associative
    // containers such as 'bsl::map'.  Note that this class is *not* a
    // template, and will compare two shared pointers of arbitrary types (as
    // long as one is convertible into the other).

    template <typename TYPE>
    bool operator()(bcema_SharedPtr<TYPE> const& lhs,
                    bcema_SharedPtr<TYPE> const& rhs) const;
        // Return 'true' if the specified 'lhs' shared pointer to the
        // parameterized 'TYPE' is less than the specified 'rhs' shared pointer
        // to the same 'TYPE', and 'false' otherwise.  A shared pointer 'lhs'
        // is less than another 'rhs' shared pointer if 'lhs.ptr()' is less
        // than 'rhs.ptr()' as compared by the 'bsl::less<TYPE *>' functor.
        // Note that the expression 'lhs < rhs' will *not* compile.
};

                        // ==========================
                        // struct bcema_SharedPtrUtil
                        // ==========================

struct bcema_SharedPtrUtil {
    // Namespace for operations on shared pointers.

    // TYPES
    template <typename TYPE>
    struct PtrLess : bsl::binary_function<bcema_SharedPtr<TYPE>,
                                          bcema_SharedPtr<TYPE>,
                                          bool>
    {
        // This class template provides a functor comparing two shared
        // pointers to the parameterized 'TYPE' suitable for use in the
        // standard associative containers such as 'bsl::map'.

        bool operator()(bcema_SharedPtr<TYPE> const& lhs,
                        bcema_SharedPtr<TYPE> const& rhs) const;
            // Return 'true' if the specified 'lhs' shared pointer to the
            // parameterized 'TYPE' is less than the specified 'rhs' shared
            // pointer to the same 'TYPE', and 'false' otherwise.  A shared
            // pointer 'lhs' is less than another 'rhs' shared pointer if
            // 'lhs.ptr()' is less than 'rhs.ptr()' as compared by the
            // 'bsl::less<TYPE *>' functor.  Note that the expression
            // 'lhs < rhs' will *not* compile.
    };

    // CLASS METHODS
    template <class TARGET, class SOURCE>
    static
    bcema_SharedPtr<TARGET> dynamicCast(bcema_SharedPtr<SOURCE> const& source);
        // Return a 'bcema_SharedPtr<TARGET>' object sharing ownership of the
        // same object as the specified 'source' shared pointer to the
        // parameterized 'SOURCE', and pointing to
        // 'dynamic_cast<TARGET *>(source.ptr())'.  If 'source' cannot be
        // dynamically cast to 'TARGET', then return an empty
        // 'bcema_SharedPtr<TARGET>' object.

    template <class TARGET, class SOURCE>
    static
    bcema_SharedPtr<TARGET> staticCast(bcema_SharedPtr<SOURCE> const& source);
        // Return a 'bcema_SharedPtr<TARGET>' object sharing ownership of the
        // same object as the specified 'source' shared pointer to the
        // parameterized 'SOURCE', and pointing to
        // 'static_cast<TARGET *>(source.ptr())'.  Note that if 'source' cannot
        // be statically cast to 'TARGET', then a compiler diagnostic will be
        // emitted indicating the error.

    template <class TARGET, class SOURCE>
    static
    bcema_SharedPtr<TARGET> constCast(bcema_SharedPtr<SOURCE> const& source);
        // Return a 'bcema_SharedPtr<TARGET>' object sharing ownership of the
        // same object as the specified 'source' shared pointer to the
        // parameterized 'SOURCE', and pointing to
        // 'const_cast<TARGET *>(source.ptr())'.  Note that if 'source' cannot
        // be 'const'-cast to 'TARGET', then a compiler diagnostic will be
        // emitted indicating the error.

    template <class TARGET, class SOURCE>
    static
    void dynamicCast(bcema_SharedPtr<TARGET>        *target,
                     bcema_SharedPtr<SOURCE> const&  source);
        // Load into the specified 'target' shared pointer an alias of the
        // specified 'source' shared pointer pointing to
        // 'dynamic_cast<TARGET *>(source.ptr())'.  The previous 'target'
        // shared pointer is destroyed (destroying the managed object if
        // 'target' holds the last reference to this object).  If 'source'
        // cannot be dynamically cast to 'TARGET', 'target' will be initialized
        // to an empty value.

    template <class TARGET, class SOURCE>
    static
    void staticCast(bcema_SharedPtr<TARGET>        *target,
                    bcema_SharedPtr<SOURCE> const&  source);
        // Load into the specified 'target' shared pointer an alias of the
        // specified 'source' shared pointer pointing to
        // 'static_cast<TARGET *>(source.ptr())'.  The previous 'target' shared
        // pointer is destroyed (destroying the managed object if 'target'
        // holds the last reference to this object).  Note that if 'source'
        // cannot be statically cast to 'TARGET', then a compiler diagnostic
        // will be emitted indicating the error.

    template <class TARGET, class SOURCE>
    static
    void constCast(bcema_SharedPtr<TARGET>        *target,
                   bcema_SharedPtr<SOURCE> const&  source);
        // Load into the specified 'target' shared pointer an alias of the
        // specified 'source' shared pointer pointing to
        // 'static_cast<TARGET *>(source.ptr())'.  The previous 'target' shared
        // pointer is destroyed (destroying the managed object if 'target'
        // holds the last reference to this object).  Note that if 'source'
        // cannot be statically cast to 'TARGET', then a compiler diagnostic
        // will be emitted indicating the error.

    static
    bcema_SharedPtr<char>
    createInplaceUninitializedBuffer(bsl::size_t      bufferSize,
                                     bslma_Allocator *allocator = 0);
        // Return a shared pointer with an in-place representation to a newly
        // created uninitialized buffer of the specified 'bufferSize'.
        // Optionally specify the 'allocator' used to supply memory.  If
        // 'allocator' is 0, the currently installed default allocator is used.
        // Note that the return buffer is only *naturally* aligned to
        // 'bufferSize', even if 'allocator' always return maximally aligned
        // memory addresses.
};

                        // ================================
                        // struct bcema_SharedPtrNilDeleter
                        // ================================

struct bcema_SharedPtrNilDeleter {
    // This 'struct' provides a function-like shared pointer deleter that does
    // nothing when invoked.

    template <class TYPE>
    void operator() (TYPE *)
        // No-Op.
    {
    }
};

                        // ======================================
                        // struct bcema_SharedPtr_DeleterTypeEnum
                        // ======================================

struct bcema_SharedPtr_DeleterTypeEnum {
    // This 'struct' enumerates four kinds of deleters, two of them are factory
    // deleters, and the last two are function-like deleters.

    enum {
        BCEMA_ALLOCATOR_PTR,
        BCEMA_FACTORY_PTR,
        BCEMA_FUNCTOR_WITH_ALLOC,
        BCEMA_FUNCTOR_WO_ALLOC
    };
};

                        // =================================
                        // class bcema_SharedPtr_DeleterType
                        // =================================

template<class DELETER>
class bcema_SharedPtr_DeleterType {
    // This 'class' provides two meta-functions for determining the enumerated
    // type and the C++ type of a deleter based on whether it is a pointer to a
    // function, to a factory deleter, or an instance of a function-like
    // deleter.

    enum {
        BCEMA_USES_ALLOC =
           bslalg_HasTrait<DELETER, bslalg_TypeTraitUsesBslmaAllocator>::VALUE,

        BCEMA_IS_PTR = bslmf_IsPointer<DELETER>::VALUE
                   && !bslmf_IsFunctionPointer<DELETER>::VALUE,

        BCEMA_IS_ALLOC_PTR = bslmf_IsConvertible<DELETER,
                                                 bslma_Allocator *>::VALUE
    };

    typedef bcema_SharedPtr_DeleterTypeEnum DeleterTypeEnum;

  public:
    enum {
        VALUE = BCEMA_USES_ALLOC   ?DeleterTypeEnum::BCEMA_FUNCTOR_WITH_ALLOC :
               !BCEMA_IS_PTR       ?DeleterTypeEnum::BCEMA_FUNCTOR_WO_ALLOC :
                BCEMA_IS_ALLOC_PTR ?DeleterTypeEnum::BCEMA_ALLOCATOR_PTR :
                                    DeleterTypeEnum::BCEMA_FACTORY_PTR
    };

    typedef
    typename bslmf_If<BCEMA_IS_ALLOC_PTR,
                      bslma_Allocator *,
                      DELETER>::Type                 Type;
};

                        // ====================================
                        // struct bcema_SharedPtr_DeleterHelper
                        // ====================================

struct bcema_SharedPtr_DeleterHelper {
    // This 'struct' provides a utility to apply a deleter in a way that is
    // appropriate given its type.

    typedef bcema_SharedPtr_DeleterTypeEnum DeleterTypeEnum;

    template <class TYPE, class DELETER>
    static void deleteObject(
        TYPE *ptr, DELETER& deleter,
        bslmf_MetaInt<DeleterTypeEnum::BCEMA_ALLOCATOR_PTR>);

    template <class TYPE, class DELETER>
    static void deleteObject(
        TYPE *ptr, DELETER& deleter,
        bslmf_MetaInt<DeleterTypeEnum::BCEMA_FACTORY_PTR>);

    template <class TYPE, class DELETER>
    static void deleteObject(
        TYPE *ptr, DELETER& deleter,
        bslmf_MetaInt<DeleterTypeEnum::BCEMA_FUNCTOR_WITH_ALLOC>);

    template <class TYPE, class DELETER>
    static void deleteObject(
        TYPE *ptr, DELETER& deleter,
        bslmf_MetaInt<DeleterTypeEnum::BCEMA_FUNCTOR_WO_ALLOC>);
};

                        // ============================
                        // class bcema_SharedPtrRepImpl
                        // ============================

template <class TYPE, class DELETER>
class bcema_SharedPtr_RepImpl: public bcema_SharedPtrRep {
    // This class provides a concrete implementation of the
    // 'bcema_SharedPtr_Rep' protocol for out-of-place instances of the
    // parameterized 'TYPE'.  Upon destruction of this object, the deleter of
    // the parameterized type 'DELETER' is invoked with a pointer to
    // the managed object.

    typedef bcema_SharedPtr_DeleterTypeEnum DeleterTypeEnum;
    typedef typename bcema_SharedPtr_DeleterType<DELETER>::Type Deleter;

    // DATA
    Deleter           d_deleter;     // deleter for this out-of-place instance
    TYPE             *d_ptr_p;       // pointer to out-of-place instance
    bslma_Allocator  *d_allocator_p; // basic allocator

    // NOT IMPLEMENTED
  private:
    bcema_SharedPtr_RepImpl(const bcema_SharedPtr_RepImpl&);
    bcema_SharedPtr_RepImpl& operator=(const bcema_SharedPtr_RepImpl&);

  public:
    // CREATORS
    bcema_SharedPtr_RepImpl(
        TYPE *ptr, const DELETER& deleter, bslma_Allocator *allocator,
        bslmf_MetaInt<DeleterTypeEnum::BCEMA_ALLOCATOR_PTR>);
    bcema_SharedPtr_RepImpl(
        TYPE *ptr, const DELETER& deleter, bslma_Allocator *allocator,
        bslmf_MetaInt<DeleterTypeEnum::BCEMA_FACTORY_PTR>);
    bcema_SharedPtr_RepImpl(
        TYPE *ptr, const DELETER& deleter, bslma_Allocator *allocator,
        bslmf_MetaInt<DeleterTypeEnum::BCEMA_FUNCTOR_WITH_ALLOC>);
    bcema_SharedPtr_RepImpl(
        TYPE *ptr, const DELETER& deleter, bslma_Allocator *allocator,
        bslmf_MetaInt<DeleterTypeEnum::BCEMA_FUNCTOR_WO_ALLOC>);
        // Construct a 'bcema_SharedPtr_RepImpl' to represent the object of
        // parameterized 'TYPE' specified by 'ptr' and instantiate a copy the
        // specified 'deleter' that will be used to destroy the specified 'ptr'
        // at destruction of this object.  Note that the specified 'allocator'
        // is the allocator that should be used to destroy this representation
        // object, but not necessarily to destroy 'ptr'.

    ~bcema_SharedPtr_RepImpl();
        // Destroy this representation object and delete the managed pointer
        // using the associated deleter.

    // MANIPULATORS
    virtual void release();
        // Delete this representation object using the allocator supplied
        // at construction.

    // ACCESSORS
    virtual void *originalPtr() const;
        // Return the (untyped) pointer value supplied to this representation
        // object at construction.

    TYPE *ptr() const;
        // Return the address of the object managed by this representation
        // object, supplied to this representation object at construction.
};

                      // ====================================
                      // class bcema_SharedPtr_InplaceRepImpl
                      // ====================================

template <class TYPE>
class bcema_SharedPtr_InplaceRepImpl: public bcema_SharedPtrRep {
    // This class provides a concrete implementation of the
    // 'bcema_SharedPtrRep' protocol for in-place instances of 'TYPE'.

    // DATA
    bslma_Allocator  *d_allocator_p; // basic allocator

    TYPE              d_instance;    // beginning of in-place buffer
                                     // (* must be last in this layout, as the
                                     // in-place uninitialized buffers created
                                     // by 'createInplaceUninitializedBuffer'
                                     // may access memory beyond the
                                     // 'd_instance' footprint *)

    // NOT IMPLEMENTED
  private:
    bcema_SharedPtr_InplaceRepImpl(const bcema_SharedPtr_InplaceRepImpl&);
    bcema_SharedPtr_InplaceRepImpl& operator=(
                                   const bcema_SharedPtr_InplaceRepImpl&);

  public:
    // CREATORS
    explicit bcema_SharedPtr_InplaceRepImpl(bslma_Allocator *allocator);
        // Create "in-place" in this representation object a
        // default-constructed instance of the parameterized type 'TYPE'.

    template <class A1>
    bcema_SharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1);
    template <class A1, class A2>
    bcema_SharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                    const A2& a2);
    template <class A1, class A2, class A3>
    bcema_SharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                   const A2& a2, const A3& a3);
    template <class A1, class A2, class A3, class A4>
    bcema_SharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                   const A2& a2, const A3& a3, const A4& a4);
    template <class A1, class A2, class A3, class A4, class A5>
    bcema_SharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                   const A2& a2, const A3& a3, const A4& a4,
                                   const A5& a5);
    template <class A1, class A2, class A3, class A4, class A5, class A6>
    bcema_SharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                   const A2& a2, const A3& a3, const A4& a4,
                                   const A5& a5, const A6& a6);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7>
    bcema_SharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                    const A2& a2, const A3& a3, const A4& a4,
                                    const A5& a5, const A6& a6, const A7& a7);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8>
    bcema_SharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                    const A2& a2, const A3& a3, const A4& a4,
                                    const A5& a5, const A6& a6, const A7& a7,
                                    const A8& a8);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9>
    bcema_SharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                    const A2& a2, const A3& a3, const A4& a4,
                                    const A5& a5, const A6& a6, const A7& a7,
                                    const A8& a8, const A9& a9);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10>
    bcema_SharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                    const A2& a2, const A3& a3, const A4& a4,
                                    const A5& a5, const A6& a6, const A7& a7,
                                    const A8& a8, const A9& a9,
                                    const A10& a10);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11>
    bcema_SharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                    const A2& a2, const A3& a3, const A4& a4,
                                    const A5& a5, const A6& a6, const A7& a7,
                                    const A8& a8, const A9& a9, const A10& a10,
                                    const A11& a11);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12>
    bcema_SharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                    const A2& a2, const A3& a3, const A4& a4,
                                    const A5& a5, const A6& a6, const A7& a7,
                                    const A8& a8, const A9& a9, const A10& a10,
                                    const A11& a11, const A12& a12);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12,
              class A13>
    bcema_SharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                    const A2& a2, const A3& a3, const A4& a4,
                                    const A5& a5, const A6& a6, const A7& a7,
                                    const A8& a8, const A9& a9, const A10& a10,
                                    const A11& a11, const A12& a12,
                                    const A13& a13);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12,
              class A13, class A14>
    bcema_SharedPtr_InplaceRepImpl(bslma_Allocator *allocator, const A1& a1,
                                    const A2& a2, const A3& a3, const A4& a4,
                                    const A5& a5, const A6& a6, const A7& a7,
                                    const A8& a8, const A9& a9, const A10& a10,
                                    const A11& a11, const A12& a12,
                                    const A13& a13, const A14& a14);
        // Create "in-place" in this representation object an instance of the
        // parameterized type 'TYPE' using the 'TYPE' constructor that takes
        // the specified arguments 'a1' up to 'aN' where 'N' (currently at most
        // 14) is the number of arguments passed to this method.

    ~bcema_SharedPtr_InplaceRepImpl();
        // Destroy this representation object and the embedded instance of
        // 'TYPE'.

    // MANIPULATORS
    TYPE *ptr();
        // Return the address of the modifiable (in-place) object managed by
        // this representation object.

    virtual void release();
        // Delete this representation object using the allocator supplied
        // at construction.

    // ACCESSORS
    virtual void *originalPtr() const;
        // Return the (untyped) address of the (in-place) object managed by
        // this representation object.
};

                        // ================================
                        // struct bcema_SharedPtr_InitGuard
                        // ================================

template <class TYPE, class DELETER>
struct bcema_SharedPtr_InitGuard {
    // This guard is used for out-of-place shared pointer instantiations.
    // Generally it is instantiated prior to attempting to construct the rep
    // and release after the rep has been successfully constructed.  In the
    // event that an exception is thrown during construction of the rep, the
    // guard will delete the provided pointer using the provided deleter.  Note
    // that the provided deleter is held by reference and must remain valid for
    // the lifetime of this guard.  If this guard is not release before it's
    // destruction, a copy of the deleter is instantiated to delete the
    // pointer(in case operator() is non-const).  Also note that if the deleter
    // throws during copy-construction, the provided pointer will not be
    // destroyed.

    // INSTANCE DATA
    TYPE          *d_ptr_p;
    const DELETER& d_deleter;

  public:
    // CREATORS
    bcema_SharedPtr_InitGuard(TYPE *ptr, const DELETER& deleter);

    ~bcema_SharedPtr_InitGuard();
        // Destroy this guard.

    // MANIPULATORS
    void release();
};

// ============================================================================
//                      INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

///IMPLEMENTATION NOTES
///--------------------
// The following expression is a class invariant of 'bcema_SharedPtr' and shall
// always return 'true':
//..
//  d_rep_p == 0 || d_ptr_p != 0
//..

                        // ------------------------
                        // class bcema_SharedPtrRep
                        // ------------------------

// CREATORS
inline
bcema_SharedPtrRep::bcema_SharedPtrRep()
: d_count(1)
{
}

inline
bcema_SharedPtrRep::~bcema_SharedPtrRep()
{
}

// MANIPULATORS
inline
void bcema_SharedPtrRep::incrementRefs(int incrementAmount)
{
    this->d_count.relaxedAdd(incrementAmount);
}

inline
int bcema_SharedPtrRep::decrementRefs(int decrementAmount)
{
    return this->d_count.add(-decrementAmount);
}

// ACCESSORS
inline
int bcema_SharedPtrRep::numReferences() const
{
    return this->d_count;
}

                            // ---------------------
                            // class bcema_SharedPtr
                            // ---------------------

// PRIVATE MANIPULATORS
template <class TYPE>
template <class BCE_OTHER_TYPE, class DELETER>
void bcema_SharedPtr<TYPE>::makeRep(BCE_OTHER_TYPE  *ptr,
                                    const DELETER&   deleter,
                                    bslma_Allocator *allocator)
{
    enum { BCEMA_DELETER_TYPE = bcema_SharedPtr_DeleterType<DELETER>::VALUE };

    this->d_ptr_p = ptr;
    if (this->d_ptr_p) {
        allocator = bslma_Default::allocator(allocator);
        this->d_rep_p = new(*allocator)
            bcema_SharedPtr_RepImpl<BCE_OTHER_TYPE, DELETER>(
                ptr, deleter, allocator, bslmf_MetaInt<BCEMA_DELETER_TYPE>());
    } else {
        this->d_rep_p = 0;
    }
}

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
                                       bslma_Allocator *allocator)
{
    bcema_SharedPtr_InitGuard<TYPE, bslma_Allocator*> guard(ptr, allocator);
    this->makeRep(ptr, allocator, allocator);
    guard.release();
}

template <class TYPE>
template <class BCE_OTHER_TYPE>
bcema_SharedPtr<TYPE>::bcema_SharedPtr(BCE_OTHER_TYPE  *ptr,
                                       bslma_Allocator *allocator)
{
    bcema_SharedPtr_InitGuard<BCE_OTHER_TYPE, bslma_Allocator *>
                                                         guard(ptr, allocator);
    this->makeRep(ptr, allocator, allocator);
    guard.release();
}

template <class TYPE>
template <class BCE_OTHER_TYPE>
bcema_SharedPtr<TYPE>::bcema_SharedPtr(
                                  bdema_ManagedPtr<BCE_OTHER_TYPE>  managedPtr,
                                  bslma_Allocator                  *allocator)
: d_ptr_p(managedPtr.ptr())
, d_rep_p(0)
{
    typedef bcema_SharedPtr_InplaceRepImpl<bdema_ManagedPtr<TYPE> > Rep;

    if (d_ptr_p) {
        typedef void(*ManagedPtrDeleter)(void *, bcema_SharedPtrRep *);

        if (&bcema_SharedPtrRep::managedPtrDeleter ==
            (ManagedPtrDeleter)managedPtr.deleter().deleter()) {
            this->d_rep_p = (bcema_SharedPtrRep*)
                managedPtr.release().second.factory();
        }
        else {
            allocator = bslma_Default::allocator(allocator);
            Rep *rep = new(*allocator) Rep(allocator);
            (*rep->ptr())  = managedPtr;
            this->d_rep_p = rep;
        }
    }
}

template <class TYPE>
template <class BCE_OTHER_TYPE>
bcema_SharedPtr<TYPE>::bcema_SharedPtr(
                                     bsl::auto_ptr<BCE_OTHER_TYPE>&  autoPtr,
                                     bslma_Allocator                *allocator)
: d_ptr_p(autoPtr.get())
, d_rep_p(0)
{
    typedef bcema_SharedPtr_InplaceRepImpl<bsl::auto_ptr<BCE_OTHER_TYPE> > Rep;

    if (d_ptr_p) {
        allocator = bslma_Default::allocator(allocator);
        Rep *rep = new(*allocator) Rep(allocator);
        (*rep->ptr())  = autoPtr;
        this->d_rep_p = rep;
    }
}

template <class TYPE>
template <class BCE_OTHER_TYPE, class DELETER>
bcema_SharedPtr<TYPE>::bcema_SharedPtr(BCE_OTHER_TYPE  *ptr,
                                       const DELETER&   deleter,
                                       bslma_Allocator *allocator)
: d_ptr_p(ptr)
, d_rep_p(0)
{
    bcema_SharedPtr_InitGuard<BCE_OTHER_TYPE, DELETER> guard(ptr, deleter);
    this->makeRep(ptr, deleter, allocator);
    guard.release();
}

template <class TYPE>
template <class BCE_OTHER_TYPE>
bcema_SharedPtr<TYPE>::bcema_SharedPtr(
                                bcema_SharedPtr<BCE_OTHER_TYPE> const&  source,
                                TYPE                                   *object)
: d_ptr_p(object)
, d_rep_p(source.d_rep_p)
{
    if (this->d_ptr_p && this->d_rep_p) {
        this->d_rep_p->incrementRefs();
    } else {
        this->d_ptr_p = 0;
        this->d_rep_p = 0;
    }
}

template <class TYPE>
template <class BCE_OTHER_TYPE>
bcema_SharedPtr<TYPE>::bcema_SharedPtr(
                               bcema_SharedPtr<BCE_OTHER_TYPE> const& original)
: d_ptr_p(original.d_ptr_p)
, d_rep_p(original.d_rep_p)
{
    if (this->d_ptr_p) {
        this->d_rep_p->incrementRefs();
    } else {
        this->d_rep_p = 0;
    }
}

template <class TYPE>
inline
bcema_SharedPtr<TYPE>::bcema_SharedPtr(TYPE *ptr, bcema_SharedPtrRep *rep)
: d_ptr_p(ptr)
, d_rep_p(rep)
{
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
bcema_SharedPtr<TYPE>::bcema_SharedPtr(bcema_SharedPtr<TYPE> const& original)
: d_ptr_p(original.d_ptr_p)
, d_rep_p(original.d_rep_p)
{
    if (this->d_ptr_p) {
        this->d_rep_p->incrementRefs();
    } else {
        this->d_rep_p = 0;
    }
}

template <class TYPE>
bcema_SharedPtr<TYPE>::~bcema_SharedPtr()
{
    BSLS_ASSERT_SAFE(d_rep_p == 0 || d_ptr_p != 0);
    if (this->d_rep_p && !this->d_rep_p->decrementRefs()) {
        this->d_rep_p->release();
    }
}

// MANIPULATORS
template <class TYPE>
bcema_SharedPtr<TYPE>& bcema_SharedPtr<TYPE>::
                                    operator=(bcema_SharedPtr<TYPE> const& rhs)
{
    // Instead of testing '&rhs == this', which happens infrequently, optimize
    // for when reps are the same.
    if (rhs.d_rep_p == this->d_rep_p) {
        this->d_ptr_p = rhs.d_ptr_p;
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
    if (rhs.d_rep_p == this->d_rep_p) {
        this->d_ptr_p = rhs.d_ptr_p;
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
    if (this->d_rep_p && !this->d_rep_p->decrementRefs()) {
        this->d_rep_p->release();
    }
    this->d_ptr_p = 0;
    this->d_rep_p = 0;
}

template <class TYPE>
template <class BCE_OTHER_TYPE>
void bcema_SharedPtr<TYPE>::load(BCE_OTHER_TYPE  *ptr,
                                 bslma_Allocator *allocator)
{
    SelfType(ptr, allocator).swap(*this);
}

template <class TYPE>
template <class BCE_OTHER_TYPE, class DELETER>
void bcema_SharedPtr<TYPE>::load(BCE_OTHER_TYPE  *ptr,
                                 const DELETER&   deleter,
                                 bslma_Allocator *allocator)
{
    SelfType(ptr, deleter, allocator).swap(*this);
}

template <class TYPE>
template <class BCE_OTHER_TYPE>
void bcema_SharedPtr<TYPE>::loadAlias(
        bcema_SharedPtr<BCE_OTHER_TYPE> const&  source,
        TYPE                                   *object)
{
    if (source.d_rep_p == this->d_rep_p && object) {
        this->d_ptr_p = this->d_rep_p ? object : 0;
    }
    else {
        SelfType(source, object).swap(*this);
    }
}

template <class TYPE>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *allocator)
{
    typedef bcema_SharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
       const A1& a1)
{
    typedef bcema_SharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2)
{
    typedef bcema_SharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3)
{
    typedef bcema_SharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4)
{
    typedef bcema_SharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5)
{
    typedef bcema_SharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5, const A6& a6)
{
    typedef bcema_SharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5, a6);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5, const A6& a6, const A7& a7)
{
    typedef bcema_SharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5, a6, a7);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5, const A6& a6, const A7& a7, const A8& a8)
{
    typedef bcema_SharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5, a6, a7, a8);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
        const A9& a9)
{
    typedef bcema_SharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5, a6, a7,
                                   a8, a9);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
        const A9& a9, const A10& a10)
{
    typedef bcema_SharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5, a6, a7,
                                   a8, a9, a10);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
        const A9& a9, const A10& a10, const A11& a11)
{
    typedef bcema_SharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5, a6, a7,
                                   a8, a9, a10, a11);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
        const A9& a9, const A10& a10, const A11& a11, const A12& a12)
{
    typedef bcema_SharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5, a6, a7,
                                   a8, a9, a10, a11, a12);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
        const A9& a9, const A10& a10, const A11& a11, const A12& a12,
        const A13& a13)
{
    typedef bcema_SharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5, a6, a7,
                                   a8, a9, a10, a11, a12, a13);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
void bcema_SharedPtr<TYPE>::createInplace(bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
        const A9& a9, const A10& a10, const A11& a11, const A12& a12,
        const A13& a13, const A14& a14)
{
    typedef bcema_SharedPtr_InplaceRepImpl<TYPE> Rep;
    allocator = bslma_Default::allocator(allocator);
    Rep *rep = new(*allocator) Rep(allocator, a1, a2, a3, a4, a5, a6, a7,
                                   a8, a9, a10, a11, a12, a13, a14);
    SelfType(rep->ptr(), rep).swap(*this);
}

template <class TYPE>
bsl::pair<TYPE*, bcema_SharedPtrRep*> bcema_SharedPtr<TYPE>::release()
{
    bsl::pair<TYPE*, bcema_SharedPtrRep*> ret(this->d_ptr_p, this->d_rep_p);
    this->d_ptr_p = 0;
    this->d_rep_p = 0;
    return ret;
}

template <class TYPE>
void bcema_SharedPtr<TYPE>::swap(bcema_SharedPtr<TYPE>& src)
{
    bsl::swap(this->d_ptr_p, src.d_ptr_p);
    bsl::swap(this->d_rep_p, src.d_rep_p);
}

// ACCESSORS
template <class TYPE>
inline
bcema_SharedPtr<TYPE>::operator bcema_SharedPtr_UnspecifiedBool() const
{
    return this->d_ptr_p
         ? &bcema_SharedPtr_UnspecifiedBoolHelper::d_member
         : 0;
}

template <class TYPE>
inline
typename bcema_SharedPtr_ReferenceType<TYPE>::Reference
bcema_SharedPtr<TYPE>::operator[](bsl::ptrdiff_t index) const
{
    return *(this->d_ptr_p + index);
}

template <class TYPE>
inline
typename bcema_SharedPtr_ReferenceType<TYPE>::Reference
bcema_SharedPtr<TYPE>::operator*() const
{
    return *this->d_ptr_p;
}

template <class TYPE>
inline
TYPE *bcema_SharedPtr<TYPE>::operator->() const
{
    return this->d_ptr_p;
}

template <class TYPE>
inline
TYPE *bcema_SharedPtr<TYPE>::ptr() const
{
    return this->d_ptr_p;
}

template <class TYPE>
inline
bcema_SharedPtrRep *bcema_SharedPtr<TYPE>::rep() const
{
    return this->d_rep_p;
}

template <class TYPE>
inline
int bcema_SharedPtr<TYPE>::numReferences() const
{
    return this->d_rep_p ? d_rep_p->numReferences() : 0;
}

template <class TYPE>
bdema_ManagedPtr<TYPE> bcema_SharedPtr<TYPE>::managedPtr() const
{
    typedef void(*Deleter)(TYPE*, bcema_SharedPtrRep*);
    if (this->d_rep_p) {
        this->d_rep_p->incrementRefs();
    }
    bdema_ManagedPtr<TYPE> ptr(this->d_ptr_p, this->d_rep_p,
                             (Deleter)&bcema_SharedPtrRep::managedPtrDeleter);
    return ptr;
}

                        // *** std::tr1 COMPATIBILITY ***

// MANIPULATORS
template <class TYPE>
inline
void bcema_SharedPtr<TYPE>::reset()
{
    clear();
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
                                bcema_SharedPtr<BCE_OTHER_TYPE> const&  source,
                                TYPE                                   *ptr)
{
    SelfType(source, ptr).swap(*this);
}

// ACCESSORS
template <class TYPE>
inline
TYPE *bcema_SharedPtr<TYPE>::get() const
{
    return this->ptr();
}

template <class TYPE>
inline
bool bcema_SharedPtr<TYPE>::unique() const
{
    return 1 == this->numReferences();
}

template <class TYPE>
inline
int bcema_SharedPtr<TYPE>::use_count() const
{
    return this->numReferences();
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
void swap(bcema_SharedPtr<TYPE>& lhs, bcema_SharedPtr<TYPE>& rhs)
{
    lhs.swap(rhs);
}

                    // --------------------------
                    // struct bcema_SharedPtrLess
                    // --------------------------

template <typename TYPE>
bool bcema_SharedPtrLess::operator()(const bcema_SharedPtr<TYPE>& lhs,
                                     const bcema_SharedPtr<TYPE>& rhs) const
{
    return bsl::less<TYPE *>()(lhs.ptr(), rhs.ptr());
}

                    // -----------------------------------
                    // struct bcema_SharedPtrUtil::PtrLess
                    // -----------------------------------

template <typename TYPE>
bool bcema_SharedPtrUtil::PtrLess<TYPE>::operator()(
                                        const bcema_SharedPtr<TYPE>& lhs,
                                        const bcema_SharedPtr<TYPE>& rhs) const
{
    return bsl::less<TYPE *>()(lhs.ptr(), rhs.ptr());
}

                        // --------------------------
                        // struct bcema_SharedPtrUtil
                        // --------------------------

// CLASS METHODS
template <class TARGET, class SOURCE>
bcema_SharedPtr<TARGET>
bcema_SharedPtrUtil::dynamicCast(bcema_SharedPtr<SOURCE> const& source)
{
    return bcema_SharedPtr<TARGET>(source,
                                   dynamic_cast<TARGET*>(source.ptr()));
}

template <class TARGET, class SOURCE>
bcema_SharedPtr<TARGET>
bcema_SharedPtrUtil::staticCast(bcema_SharedPtr<SOURCE> const& source)
{
    return bcema_SharedPtr<TARGET>(source,
                                   static_cast<TARGET*>(source.ptr()));
}

template <class TARGET, class SOURCE>
bcema_SharedPtr<TARGET>
bcema_SharedPtrUtil::constCast(bcema_SharedPtr<SOURCE> const& source)
{
    return bcema_SharedPtr<TARGET>(source,
                                   const_cast<TARGET*>(source.ptr()));
}

template <class TARGET, class SOURCE>
void bcema_SharedPtrUtil::dynamicCast(bcema_SharedPtr<TARGET>        *target,
                                      bcema_SharedPtr<SOURCE> const&  source)
{
    target->loadAlias(source, dynamic_cast<TARGET*>(source.ptr()));
}

template <class TARGET, class SOURCE>
void bcema_SharedPtrUtil::staticCast(bcema_SharedPtr<TARGET>        *target,
                                     bcema_SharedPtr<SOURCE> const&  source)
{
    target->loadAlias(source, static_cast<TARGET*>(source.ptr()));
}

template <class TARGET, class SOURCE>
void bcema_SharedPtrUtil::constCast(bcema_SharedPtr<TARGET>        *target,
                                    bcema_SharedPtr<SOURCE> const&  source)
{
    target->loadAlias(source, const_cast<TARGET*>(source.ptr()));
}

                        // -------------------------------------
                        // struct bcema_SharedPtr_DeleterHelper
                        // ------------------------------------

template <class TYPE, class DELETER>
inline
void bcema_SharedPtr_DeleterHelper::deleteObject(
    TYPE *ptr, DELETER& deleter,
    bslmf_MetaInt<DeleterTypeEnum::BCEMA_ALLOCATOR_PTR>)
{
    deleter->deleteObject(ptr);
}

template <class TYPE, class DELETER>
inline
void bcema_SharedPtr_DeleterHelper::deleteObject(
    TYPE *ptr, DELETER& deleter,
    bslmf_MetaInt<DeleterTypeEnum::BCEMA_FACTORY_PTR>)
{
    deleter->deleteObject(ptr);
}

template <class TYPE, class DELETER>
inline
void bcema_SharedPtr_DeleterHelper::deleteObject(
    TYPE *ptr, DELETER& deleter,
    bslmf_MetaInt<DeleterTypeEnum::BCEMA_FUNCTOR_WITH_ALLOC>)
{
    deleter(ptr);
}

template <class TYPE, class DELETER>
inline
void bcema_SharedPtr_DeleterHelper::deleteObject(
    TYPE *ptr, DELETER& deleter,
    bslmf_MetaInt<DeleterTypeEnum::BCEMA_FUNCTOR_WO_ALLOC>)
{
    deleter(ptr);
}

                        // ------------------------------
                        // class bcema_SharedPtr_RepImpl
                        // ------------------------------

// CREATORS
template <class TYPE, class DELETER>
bcema_SharedPtr_RepImpl<TYPE, DELETER>::bcema_SharedPtr_RepImpl(
    TYPE *ptr, const DELETER& deleter, bslma_Allocator *allocator,
    bslmf_MetaInt<DeleterTypeEnum::BCEMA_ALLOCATOR_PTR>)
: d_deleter(bslma_Default::allocator(deleter))
, d_ptr_p(ptr)
, d_allocator_p(allocator)
{
}

template <class TYPE, class DELETER>
bcema_SharedPtr_RepImpl<TYPE, DELETER>::bcema_SharedPtr_RepImpl(
    TYPE *ptr, const DELETER& deleter, bslma_Allocator *allocator,
    bslmf_MetaInt<DeleterTypeEnum::BCEMA_FACTORY_PTR>)
: d_deleter(deleter)
, d_ptr_p(ptr)
, d_allocator_p(allocator)
{
}

template <class TYPE, class DELETER>
bcema_SharedPtr_RepImpl<TYPE, DELETER>::bcema_SharedPtr_RepImpl(
    TYPE *ptr, const DELETER& deleter, bslma_Allocator *allocator,
    bslmf_MetaInt<DeleterTypeEnum::BCEMA_FUNCTOR_WITH_ALLOC>)
: d_deleter(deleter, allocator)
, d_ptr_p(ptr)
, d_allocator_p(allocator)
{
}

template <class TYPE, class DELETER>
bcema_SharedPtr_RepImpl<TYPE, DELETER>::bcema_SharedPtr_RepImpl(
    TYPE *ptr, const DELETER& deleter, bslma_Allocator *allocator,
    bslmf_MetaInt<DeleterTypeEnum::BCEMA_FUNCTOR_WO_ALLOC>)
: d_deleter(deleter)
, d_ptr_p(ptr)
, d_allocator_p(allocator)
{
}

template <class TYPE, class DELETER>
bcema_SharedPtr_RepImpl<TYPE, DELETER>::~bcema_SharedPtr_RepImpl()
{
    enum { BCEMA_DELETER_TYPE = bcema_SharedPtr_DeleterType<DELETER>::VALUE };
    bcema_SharedPtr_DeleterHelper::deleteObject(d_ptr_p, d_deleter,
                                          bslmf_MetaInt<BCEMA_DELETER_TYPE>());
}

// MANIPULATORS
template <class TYPE, class DELETER>
inline
void bcema_SharedPtr_RepImpl<TYPE, DELETER>::release()
{
    // Note: by using explicit destructor call instead of deleteObject, we
    // allow the dtor to be called non-virtually ([class.virtual] 13: "Explicit
    // qualification with the scope operator (5.1) suppresses the virtual call
    // mechanism.", page 224 of Working Draft 2007-10).  Moreover,
    // 'deleteObject' is forced to perform a dynamic_cast to get address of the
    // most derived object to deallocate it, but it is not needed here.

    this->bcema_SharedPtr_RepImpl<TYPE, DELETER>::~bcema_SharedPtr_RepImpl();
    d_allocator_p->deallocate(this);
}

// ACCESSORS
template <class TYPE, class DELETER>
inline
void *bcema_SharedPtr_RepImpl<TYPE, DELETER>::originalPtr() const
{
    return const_cast<void*>(static_cast<const void*>(d_ptr_p));
}

template <class TYPE, class DELETER>
inline
TYPE *bcema_SharedPtr_RepImpl<TYPE, DELETER>::ptr() const
{
    return this->d_ptr_p;
}

                        // -------------------------------------
                        // class bcema_SharedPtr_InplaceRepImpl
                        // -------------------------------------

// CREATORS
template <class TYPE>
inline
bcema_SharedPtr_InplaceRepImpl<TYPE>::bcema_SharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator)
: d_allocator_p(allocator)
{
}

template <class TYPE>
template <class A1>
bcema_SharedPtr_InplaceRepImpl<TYPE>::bcema_SharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator, const A1& a1)
: d_allocator_p(allocator)
, d_instance(a1)
{
}

template <class TYPE>
template <class A1, class A2>
bcema_SharedPtr_InplaceRepImpl<TYPE>::bcema_SharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator, const A1& a1, const A2& a2)
: d_allocator_p(allocator)
, d_instance(a1, a2)
{
}

template <class TYPE>
template <class A1, class A2, class A3>
bcema_SharedPtr_InplaceRepImpl<TYPE>::bcema_SharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator, const A1& a1, const A2& a2, const A3& a3)
: d_allocator_p(allocator)
, d_instance(a1, a2, a3)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4>
bcema_SharedPtr_InplaceRepImpl<TYPE>::bcema_SharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4)
: d_allocator_p(allocator)
, d_instance(a1, a2, a3, a4)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5>
bcema_SharedPtr_InplaceRepImpl<TYPE>::bcema_SharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
: d_allocator_p(allocator)
, d_instance(a1, a2, a3, a4, a5)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6>
bcema_SharedPtr_InplaceRepImpl<TYPE>::bcema_SharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5,
        const A6& a6)
: d_allocator_p(allocator)
, d_instance(a1, a2, a3, a4, a5, a6)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
bcema_SharedPtr_InplaceRepImpl<TYPE>::bcema_SharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5,
        const A6& a6, const A7& a7)
: d_allocator_p(allocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8>
bcema_SharedPtr_InplaceRepImpl<TYPE>::bcema_SharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5,
        const A6& a6, const A7& a7, const A8& a8)
: d_allocator_p(allocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
bcema_SharedPtr_InplaceRepImpl<TYPE>::bcema_SharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5,
        const A6& a6, const A7& a7, const A8& a8, const A9& a9)
: d_allocator_p(allocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10>
bcema_SharedPtr_InplaceRepImpl<TYPE>::bcema_SharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5,
        const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)
: d_allocator_p(allocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
bcema_SharedPtr_InplaceRepImpl<TYPE>::bcema_SharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5,
        const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10,
        const A11& a11)
: d_allocator_p(allocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
bcema_SharedPtr_InplaceRepImpl<TYPE>::bcema_SharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5,
        const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10,
        const A11& a11, const A12& a12)
: d_allocator_p(allocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
bcema_SharedPtr_InplaceRepImpl<TYPE>::bcema_SharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5,
        const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10,
        const A11& a11, const A12& a12, const A13& a13)
: d_allocator_p(allocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
bcema_SharedPtr_InplaceRepImpl<TYPE>::bcema_SharedPtr_InplaceRepImpl(
        bslma_Allocator *allocator,
        const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5,
        const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10,
        const A11& a11, const A12& a12, const A13& a13, const A14& a14)
: d_allocator_p(allocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)
{
}

template <class TYPE>
inline
bcema_SharedPtr_InplaceRepImpl<TYPE>::~bcema_SharedPtr_InplaceRepImpl()
{
}

// MANIPULATORS
template <class TYPE>
inline
void bcema_SharedPtr_InplaceRepImpl<TYPE>::release()
{
    // Note: by using explicit destructor call instead of deleteObject, we
    // allow the dtor to be called non-virtually ([class.virtual] 13: "Explicit
    // qualification with the scope operator (5.1) suppresses the virtual call
    // mechanism.", page 224 of Working Draft 2007-10).  Moreover,
    // 'deleteObject' is forced to perform a dynamic_cast to get address of the
    // most derived object to deallocate it, but it is not needed here.

    this->bcema_SharedPtr_InplaceRepImpl<TYPE>::
                                             ~bcema_SharedPtr_InplaceRepImpl();
    d_allocator_p->deallocate(this);
}

// ACCESSORS
template <class TYPE>
inline
void *bcema_SharedPtr_InplaceRepImpl<TYPE>::originalPtr() const
{
    return const_cast<void*>(static_cast<const void*>(&(this->d_instance)));
}

template <class TYPE>
inline
TYPE *bcema_SharedPtr_InplaceRepImpl<TYPE>::ptr()
{
    return &(this->d_instance);
}

                        // --------------------------------
                        // struct bcema_SharedPtr_InitGuard
                        // --------------------------------

//CREATORS
template <class TYPE, class DELETER>
inline
bcema_SharedPtr_InitGuard<TYPE, DELETER>::bcema_SharedPtr_InitGuard(
                                                       TYPE           *ptr,
                                                       const DELETER&  deleter)
: d_ptr_p(ptr)
, d_deleter(deleter)
{
}

template <class TYPE, class DELETER>
inline
bcema_SharedPtr_InitGuard<TYPE, DELETER>::~bcema_SharedPtr_InitGuard()
{
    enum { BCEMA_DELETER_TYPE = bcema_SharedPtr_DeleterType<DELETER>::VALUE };

    if (d_ptr_p) {
        DELETER tempDeleter(d_deleter);
        bcema_SharedPtr_DeleterHelper::deleteObject(
            d_ptr_p, tempDeleter, bslmf_MetaInt<BCEMA_DELETER_TYPE>());
    }
}

// MANIPULATORS
template <class TYPE, class DELETER>
inline
void bcema_SharedPtr_InitGuard<TYPE, DELETER>::release()
{
    this->d_ptr_p = 0;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
