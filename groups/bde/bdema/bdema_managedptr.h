// bdema_managedptr.h              -*-C++-*-
#ifndef INCLUDED_BDEMA_MANAGEDPTR
#define INCLUDED_BDEMA_MANAGEDPTR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a managed pointer component
//
//@CLASSES:
//                bdema_ManagedPtr: proctor for automatic memory management
//         bdema_ManagedPtrDeleter: used for automatic memory deallocation
//  bdema_ManagedPtrFactoryDeleter: used for automatic memory deallocation of
//                                  factory-allocated pointers
//      bdema_ManagedPtrNilDeleter: used for creating managed pointers to
//                                  stack-allocated objects
//
//@AUTHOR: Ilougino Rocha (irocha), Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides a proctor, similar to 'bsl::auto_ptr',
// that supports user-specified deleters.  The proctor (OED: "a person employed
// to manage the affairs of another") is responsible for the automatic
// destruction of the object referenced by the managed pointer.  (See
// "Aliasing" section below for the detailed description of this behavior.)
//
///Deleters
///--------
// When a managed pointer is destroyed, the managed object is destroyed
// using the user supplied "deleter".  A deleter is simply a function
// that is invoked with a pointer to the object to be deleted, and an optional
// pointer to a 'factory'.  The meaning of 'factory' depends on the deleter.
// The following are examples of deleter functions:
//..
// void nilDeleter(void *, void*)
// {
//    // Do nothing
// }
//
// void MyFactoryDeleter(MyObject *object, MyFactory *factory)
// {
//    factory->deleteObject(object);
// }
//
// void MyReleaseOnlyDeleter(MyObject *object, MyFactory *factory)
// {
//    factory->releaseObject(object);
// }
//..
// 'bdema_ManagedPtr' provides default deleters for factory objects that
// provide a 'deleteObject' operation in the fashion of the second example
// above (e.g. 'bdema_Allocator').  For example, to construct a managed pointer
// to an object constructed from a 'bdema_Pool' memory pool, simply do the
// following:
//..
// bdema_ManagedPtr<MyObject> createObject(bdema_Pool *pool)
// {
//    return bdema_ManagedPtr<MyObject>(new(*pool) MyObject(), pool);
// }
//..
// In the above example, when the returned managed pointer is destroyed,
// 'deleteObject' will be invoked on 'pool' with a pointer to the 'MyObject'
// instance, returning all memory allocated by 'MyObject' to the 'pool'.
//
// Note that the instance pointer that is passed to the deleter is always the
// object that was used to initialize the managed pointer.  That is, if the
// managed pointer is converted to a new pointer value through either casting
// or aliasing, then the original object pointer is remembered, and will be
// passed to the deleter when destroying the managed instance.  This ensures
// that the proper destructor logic is called regardless of how the managed
// pointer is transformed.
//
///Exception safety
///----------------
// The principal usage of a managed pointer is to guarantee that a local object
// will be deallocated properly should an operation throw after its allocation.
// In this, it is very similar to 'bsl::auto_ptr'.  It is required for the
// proper functioning of this component that a deleter does not throw at
// invocation (upon destruction or re-assignment of the managed pointer).
//
///Aliasing
///--------
// In managed pointer, the pointer value (the value returned by the 'ptr'
// method) and the pointer to the managed object need not be the same value.
// the 'loadAlias' method allows a managed pointer to be constructed as an
// "alias" to another managed pointer (possibly of a different type), which
// we'll call the "original" managed pointer.  When 'ptr' is invoked on the
// alias, the aliased pointer value is returned, but when the managed pointer
// is destroyed, the original managed instance will be passed to the deleter.
// (See also the documentation of the 'alias' constructor or of the
// 'loadAlias' method.)
//
// For instance, suppose that we wish to give access to an item in a temporary
// array via a pointer which we'll call the "finger".  The entire array must
// remain valid until the finger is destroyed, but the finger is the only
// reference to the array or any part of the array.  The finger is the only
// pointer to the array or any part of the array, but the entire array must be
// valid until the finger is destroyed, at which time the entire array must be
// deleted.  We handle this situation by first creating a managed pointer to
// the entire array, then creating an alias of that pointer for the
// finger.  The finger takes ownership of the array instance, and when the
// finger is destroyed, it is the array's address, rather than the finger,
// that is passed to the deleter.
//
// What follows is a concrete example illustrating the alias concept.
// Let's say our array stores data acquired from a ticker
// plant accessible by a global 'getQuote()' function:
//..
//  double getQuote() // From ticker plant. Simulated here
//  {
//      static const double QUOTES[] = {
//          7.25, 12.25, 11.40, 12.00, 15.50, 16.25, 18.75, 20.25, 19.25, 21.00
//      };
//      static const int NUM_QUOTES = sizeof(QUOTES) / sizeof(QUOTES[0]);
//      static int index = 0;
//
//      double ret = QUOTES[index];
//      index = (index + 1) % NUM_QUOTES;
//      return ret;
//  }
//..
// We now want to find the first quote larger than a specified threshold, but
// would also like to keep the earlier and later quotes for possible
// examination.  Our 'getFirstQuoteLargerThan' function must allocate memory
// for an array of quotes (the threshold and its neighbors).  It thus returns
// a managed pointer to the desired value:
//..
//  const double END_QUOTE = -1;
//
//  bdema_ManagedPtr<double>
//  getFirstQuoteLargerThan(double threshold, bdema_Allocator *allocator)
//  {
//      assert( END_QUOTE < 0 && 0 <= threshold );
//..
// We allocate our array with extra room to mark the beginning and end with a
// special 'END_QUOTE' value:
//..
//      const int MAX_QUOTES = 100;
//      int numBytes = (MAX_QUOTES + 2) * sizeof(double);
//      double *quotes = (double*) allocator->allocate(numBytes);
//      quotes[0] = quotes[MAX_QUOTES + 1] = END_QUOTE;
//..
// Then we read quotes until the array is full, keeping track of the first
// quote that exceeds the threshold.
//..
//      double *finger = 0;
//
//      for (int i = 1; i <= MAX_QUOTES; ++i) {
//          double quote = getQuote();
//          quotes[i] = quote;
//          if (! finger && quote > threshold) {
//              finger = &quotes[i];
//          }
//      }
//..
// Before we return, we create a managed pointer to the entire array:
//..
//      bdema_ManagedPtr<double> managedQuotes(quotes, allocator);
//..
// Then we use the alias constructor to create a managed pointer that points
// to the desired value (the finger) but manages the entire array:
//..
//      return bdema_ManagedPtr<double>(managedQuotes, finger);
//  }
//..
// Our main program calls 'getFirstQuoteLargerThan' like this:
//..
//  int main()
//  {
//      bdema_TestAllocator ta;
//      bdema_ManagedPtr<double> result = getFirstQuoteLargerThan(16.00, &ta);
//      assert(*result > 16.00);
//      assert(1 == ta.numBlocksInUse());
//      bsl::cout << "Found quote: " << *result << bsl::endl;
//..
// We also print the preceding 5 quotes in last-to-first order:
//..
//      int i;
//      bsl::cout << "Preceded by:";
//      for (i = -1; i >= -5; --i) {
//          double quote = result.ptr()[i];
//          if (END_QUOTE == quote) {
//              break;
//          }
//          assert(quote < *result);
//          bsl::cout << ' ' << quote;
//      }
//      bsl::cout << bsl::endl;
//..
// To move the finger, e.g. to the last position printed, one must be careful
// to retain the ownership of the entire array.  Using the statement
// 'result.load(result.ptr()-i)' would be an error, because it would first
// compute the pointer value 'result.ptr()-i' of the argument, then release the
// entire array before starting to manage what has now become an invalid
// pointer.  Instead, 'result' must retain its ownership to the entire array,
// which can be attained by:
//..
//      result.loadAlias(result, result.ptr()-i);
//..
// If we reset the result pointer, the entire array is deallocated:
//..
//      result.clear();
//      assert(0 == ta.numBlocksInUse());
//      assert(0 == ta.numBytesInUse());
//
//      return 0;
//  }
//..
//
///Type Casting
///------------
// 'bdema_ManagedPtr' objects can be implicitly and explicitly cast to
// different types in the same way as native pointers can.
//
///Implicit Casting
/// - - - - - - - -
// As with native pointers, a pointer of the type 'B' that is derived from the
// type 'A', can be directly assigned a 'bcema_SharedPtr' of 'A'.
// In other words, consider the following code snippets:
//..
//  void implicitCastingExample() {
//..
// If the statements:
//..
//      B *b_p = 0;
//      A *a_p = b_p; // conversion from 'B*' to 'A*'
//..
// are legal expressions, then the statements
//..
//      bdema_ManagedPtr<B> b_mp1;
//      bdema_ManagedPtr<A> a_mp1;
//      a_mp1 = b_mp1; // conversion assignment
//                     // WARNING: READ RELEASE NOTES BELOW
//..
// and
//..
//     bdema_ManagedPtr<B> b_mp2;
//     bdema_ManagedPtr<A> a_mp2(b_mp2); // conversion construction
//..
// should also be valid.  NOTE that in all of the above cases, the proper
// destructor of 'B' will be invoked when the instance is destroyed even if
// 'A' does not provide a polymorphic destructor.
//
// RELEASE NOTES: The assignment with conversion will *not* compile on most
// platforms with BDE 1.3.0.  That is, the following statement will not compile
// (for instance, with the AIX or Gnu compilers):
//..
//     a_mp2 = b_mp2; // WILL NOT COMPILE WITH BDE 1.3.0
//..
// The fixed version is given in the Clearcase dev branch, but because of
// binary incompatibilities, it is not possible to release the fix at this
// time.  This limitation can be overcome, however, by using an explicit cast
// as detailed below:
//..
//     a_mp2  = bdema_ManagedPtr<A>(b_mp2); // WORKAROUND
//  } // implicitCastingExample()
//..
//
///Explicit Casting
/// - - - - - - - -
// Through "aliasing", a managed pointer of any type can be explicitly cast
// to a managed pointer of any other type using any legal cast expression.
// For example, to static-cast a managed pointer of type A to a shared pointer
// of type B, one can simply do the following:
//..
//  void explicitCastingExample() {
//
//     bdema_ManagedPtr<A> a_mp;
//     bdema_ManagedPtr<B> b_mp1(a_mp, static_cast<B*>(a_mp.ptr()));
//..
// or even use the less safe "C"-style casts:
//..
//     // bdema_ManagedPtr<A> a_mp;
//     bdema_ManagedPtr<B> b_mp2(a_mp, (B*)(a_mp.ptr()));
//
//  } // explicitCastingExample()
//..
// Note that when using dynamic cast, if the cast fails, the target managed
// pointer will be reset to an unset state, and the source will not be
// modified.  Consider for example the following snippet of code:
//..
//  void processPolymorphicObject(bdema_ManagedPtr<A> aPtr)
//  {
//      bdema_ManagedPtr<B> bPtr(aPtr, dynamic_cast<B*>(aPtr.ptr()));
//      if (bPtr) {
//          processBObject(bPtr);
//      }
//      else {
//          processAObject(aPtr);
//      }
//  }
//..
// If the value of 'aPtr' can be dynamically cast to 'B*' then ownership is
// transferred to 'bPtr', otherwise 'aPtr' is to be modified.  As previously
// stated, the managed instance will be destroyed correctly regardless of how
// it is cast.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_BDEMA_DEFAULT
#include <bdema_default.h>
#endif

#endif

namespace BloombergLP {

template <typename OTHER>
    class bdema_ManagedPtrRef;

template <typename TYPE, typename FACTORY>
    struct bdema_ManagedPtrFactoryDeleter;

template <typename TYPE> class bdema_ManagedPtr;

                        // =============================
                        // class bdema_ManagedPtrDeleter
                        // =============================

class bdema_ManagedPtrDeleter {
    // This struct holds the information necessary for 'bdema_ManagedPtr' to
    // correctly manage its underlying object, namely the addresses of the
    // 'object' and 'factory', and the 'deleter' function, optionally supplied
    // through the constructors or through the 'set' method.  It is stored in a
    // sub-structure to allow the compiler to copy it more efficiently.

    typedef void(*Deleter)(void *, void *);
        // Deleter function prototype used to destroy the managed pointer.

    void             *d_object_p;  // pointer to the actual managed object

    void             *d_factory_p; // optional factory to be specified to
                                   // the deleter

    Deleter           d_deleter;   // deleter used to destroy the managed
                                   // object

    template <typename TYPE> friend class bdema_ManagedPtr;

  public:
    //CREATORS
    bdema_ManagedPtrDeleter();
        // Create an unitialized 'bdema_ManagedPtrDeleter' object that does not
        // refer to any object or factory instance.

    bdema_ManagedPtrDeleter(const bdema_ManagedPtrDeleter& original);
        // Create a 'bdema_ManagedPtrDeleter' struct that refers to the same
        // object or factory as the 'original' object.

    bdema_ManagedPtrDeleter(void *object, void *factory, Deleter deleter);
        // Create a 'bdema_ManagedPtrDeleter' struct that refers to the object
        // and factory instance located at the specified 'object' and 'factory'
        // memory locations, and the specified 'deleter'.

    //MANIPULATORS
    void deleteManagedObject();
        // Invoke the deleter object.

    void set(void *object, void *factory, Deleter deleter);
        // Set this 'bdema_ManagedPtrDeleter' to refer to the object
        // and factory instance located at the specified 'object' and 'factory'
        // memory locations, and to the specified 'deleter'.

    void clear();
        // Reset this 'bdema_ManagedPtrDeleter' to its uninitialized state.

    //ACCESSORS
    void *object() const;
        // Return a pointer to the managed object associated with this deleter.

    void *factory() const;
        // Return a pointer to the factory instance associated with this
        // deleter.

    const Deleter& deleter() const;
        // Return the deleter function associated with this deleter.
};

                        // ======================
                        // class bdema_ManagedPtr
                        // ======================

template <typename TYPE>
class bdema_ManagedPtr {
    // This class provides a proctor for a pointer of type 'TYPE*' which is
    // very similar to 'bsl::auto_ptr'.  The main use of this class is to
    // prevent a resource of type 'TYPE' to be leaked when an exception is
    // thrown inside a block.  When an instance of this class is destroyed or
    // re-assigned, the managed instance (if any) is automatically deleted.
    // This class supports user-supplied deleters, or factories that have a
    // 'deleteObject' method.  If no deleter or factory is specified, the
    // managed instance will be deleted by the default allocator in use during
    // deletion.
    //
    // Note that, like 'bsl::auto_ptr', an instance of 'TYPE' should be managed
    // by at most one managed pointer.  The copy and assignment semantics of
    // this class support this.  Note also that the value of a managed pointer
    // can differ from the pointer to the managed instance (through a mechanism
    // called "aliasing").  See the component-level documentation for an
    // explanation and usage example of aliasing.

    typedef bdema_ManagedPtrDeleter          Deleter;
    typedef bdema_ManagedPtrDeleter::Deleter DeleterFunc;

    TYPE    *d_ptr_p;     // pointer to the managed instance

    Deleter  d_deleter;   // deleter-related information

    template <typename OTHER> friend class bdema_ManagedPtr;

    void init(TYPE *ptr, Deleter *deleter);
        // Initialize this managed pointer to the specified 'ptr' pointer
        // value and the specified 'deleter'.  Note that this function does
        // not destroy the current managed instance.

    void init(TYPE *ptr, void *object, void *factory, DeleterFunc deleter);
        // Initialize this managed pointer to the specified 'ptr' pointer value
        // and the specified 'deleter', using the specified 'factory'.

    void reset(TYPE *ptr, Deleter *deleter);
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to the specified 'ptr' pointer value and the
        // specified 'deleter'.

    void reset(TYPE *ptr, void *object, void *factory, DeleterFunc deleter);
        // Destroy the current managed object(if any) and re-initialize this
        // managed pointer to the specified 'ptr' pointer value and the
        // specified 'deleter', using the specified 'factory'.

    void rawClear();
        // Reset this managed pointer to an unset state.  If this managed
        // pointer currently has a value, then the managed instance will not
        // be destroyed.

  public:
    // CREATORS
    bdema_ManagedPtr();
        // Construct a managed pointer that is in an unset state.

    bdema_ManagedPtr(TYPE *ptr);
        // Construct a managed pointer that manages the specified 'ptr' using
        // the current default allocator to destroy 'ptr' when this managed
        // pointer is destroyed or re-assigned, unless it is released before
        // then.  Note that the object will be initialized to an unset state
        // if 'ptr' == 0.  The behavior is undefined if 'ptr' is already
        // managed by another managed pointer.

    bdema_ManagedPtr(bdema_ManagedPtrRef<TYPE> ref);
        // Construct a 'bdema_ManagedPtr' and transfer the value and ownership
        // from the managed pointer referred-to by the specified 'ref' object
        // to this managed pointer.  Note that the managed pointer referred-to
        // by 'ref' will be re-initialized to an unset state.  This constructor
        // is used to construct from a managed pointer rvalue.

    bdema_ManagedPtr(bdema_ManagedPtr<TYPE>& original);
        // Construct a 'bdema_ManagedPtr' and transfer the value and ownership
        // from the specified 'original' managed pointer to this managed
        // pointer.  Note that 'original' will be re-initialized to an unset
        // state.

    template <typename OTHER>
    bdema_ManagedPtr(bdema_ManagedPtrRef<OTHER> ref);
        // Construct a 'bdema_ManagedPtr' and transfer the value and ownership
        // from the managed pointer referred-to by the specified 'ref' object
        // to this managed pointer.  Note that managed pointer referred-to by
        // 'ref' will be re-initialized to an unset state.  This constructor
        // is used to construct from a managed pointer rvalue with conversion.
        // Compilation will fail unless 'OTHER*' is convertible to 'TYPE*'.

    template <typename OTHER>
    bdema_ManagedPtr(bdema_ManagedPtr<OTHER>& original);
        // Construct a 'bdema_ManagedPtr' and transfer the value and ownership
        // from the specified 'original' managed pointer this managed pointer.
        // Note that 'original' will be re-initialized to an unset state.
        // Compilation will fail unless 'OTHER*' is convertible to 'TYPE*'.

    template <typename OTHER>
    bdema_ManagedPtr(bdema_ManagedPtr<OTHER>& alias, TYPE *ptr);
        // Construct a managed pointer that takes over management of the value
        // of the specified 'alias' managed pointer, but which has the
        // specified 'ptr' as an externally-visible value.  The value of 'ptr'
        // is treated as an alias to all or part of 'alias.ptr()'.  Unless it
        // is released first, destroying or re-assigning a managed pointer
        // created with this constructor will delete the object originally
        // managed by 'alias' -- the destructor for '*ptr' is not called
        // directly.  Note that 'alias' will be re-initialized to an unset
        // state, and that the object previously managed by 'alias' will not be
        // deleted until this managed pointer is destroyed or re-assigned.
        // Compilation will fail unless 'OTHER*' is convertible to 'TYPE*'.
        // The behavior is undefined if 'ptr' is already managed by a managed
        // pointer other than 'alias'.

    template <typename FACTORY>
    bdema_ManagedPtr(TYPE *ptr, FACTORY *factory);
        // Construct a managed pointer to manage the specified 'ptr' using the
        // specified 'factory' to destroy 'ptr' when this managed pointer is
        // destroyed or re-assigned, unless it is released before then.  The
        // 'FACTORY' class can be any class that has a 'deleteObject' method.
        // 'bdema_Allocator' or any class derived from 'bdema_Allocator' meets
        // the requirements for 'FACTORY'.  The behavior is undefined if 'ptr'
        // is already managed by another managed pointer.

    bdema_ManagedPtr(TYPE  *ptr,
                     void  *factory,
                     void (*deleter)(TYPE*, void*));
    template <typename FACTORY>
    bdema_ManagedPtr(TYPE     *ptr,
                     FACTORY  *factory,
                     void    (*deleter)(TYPE *, FACTORY*) );
        // Construct a managed pointer to manage the specified 'ptr' using the
        // specified 'deleter' and associated 'factory' to destroy 'ptr' when
        // this managed pointer is destroyed or re-assigned (unless it is
        // released before then).  Note that if 0 == 'ptr', then this object
        // will be initialized to an unset state.  The behavior is undefined
        // if 'ptr' is already managed by another managed pointer.

    ~bdema_ManagedPtr();
        // Destroy this managed pointer object and any managed instance by
        // invoking the user-supplied deleter.

    // MANIPULATORS
    operator bdema_ManagedPtrRef<TYPE>();
        // Implicit conversion to a managed pointer reference.  This
        // conversion operator is used to allow the construction of managed
        // pointer rvalues because temporaries cannot be passed by modifiable
        // reference.

    void load(TYPE *ptr=0);
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to manage the specified 'ptr' using the default
        // allocator to destroy 'ptr' when this managed pointer is destroyed
        // or re-assigned, unless it is released before then.  Note that if 0
        // == 'ptr', then this object will be initialized to an unset state.
        // The behavior is undefined if 'ptr' is already managed by another
        // managed pointer.

    template <typename FACTORY>
    void load(TYPE *ptr, FACTORY *factory);
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to manage the specified 'ptr' using the specified
        // 'factory' deleter to destroy 'ptr' when this managed pointer is
        // destroyed or re-assigned, unless it is released before then.  Note
        // that if 0 == 'ptr', then this object will be initialized to an
        // unset state.  The behavior is undefined if 'ptr' is already managed
        // by another managed pointer.

    void load(TYPE *ptr, void *factory, void (*deleter)(TYPE *, void*));
    template <typename FACTORY>
    void load(TYPE *ptr, FACTORY *factory, void (*deleter)(TYPE *, FACTORY*));
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to manage the specified 'ptr' using the specified
        // 'deleter' with arguments 'ptr' and the specified 'factory' to
        // destroy 'ptr' when this managed pointer is destroyed or
        // re-assigned, unless it is released before then.  Note that if 0 ==
        // 'ptr', then this object will be initialized to an unset state.  The
        // behavior is undefined if 'ptr' is already managed by another
        // managed pointer.

    template <typename OTHER>
    void loadAlias(bdema_ManagedPtr<OTHER>& alias, TYPE *ptr);
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to take over management of the value of the
        // specified 'alias' managed pointer, but with the specified 'ptr' as
        // an externally-visible value.  The value of 'ptr' is treated as an
        // alias to all or part of 'alias.ptr()'.  Unless it is released first,
        // destroying or re-assigning this managed pointer will delete the
        // object originally managed by 'alias' -- the destructor for '*ptr' is
        // not called directly.  Note that 'alias' will be re-initialized to an
        // unset state, and that the object previously managed by 'alias' will
        // not be deleted until this managed pointer is destroyed or
        // re-assigned.  The behavior is undefined if 'ptr' is already managed
        // by a managed pointer other than 'alias'.

    void swap(bdema_ManagedPtr<TYPE>& rhs);
        // Exchange the value and ownership of this managed pointer with the
        // specified 'rhs' managed pointer.

    bdema_ManagedPtr<TYPE>& operator=(bdema_ManagedPtr<TYPE>& rhs);
        // Assign to this managed pointer the value and ownership of the
        // specified 'rhs' managed pointer.  Note that 'rhs' will be
        // re-initialized to an unset state and that the previous contents of
        // this pointer (if any) are destroyed unless this pointer and 'rhs'
        // point to the same object.

    bdema_ManagedPtr<TYPE>& operator=(bdema_ManagedPtrRef<TYPE> ref);
        // Assign to this managed pointer the value and ownership of the
        // managed pointer referred-to by the specified 'ref'.  Note that the
        // managed pointer referred to by 'ref' will be re-initialized to an
        // unset state and that the previous contents of this pointer (if any)
        // are destroyed unless this pointer and 'rhs' point to the same
        // object.  This assignment operator is used to assign a managed
        // pointer rvalue.

    template <typename OTHER>
    bdema_ManagedPtr<TYPE>& operator=(bdema_ManagedPtr<OTHER>& rhs);
        // Assign to this managed pointer the value and ownership of the
        // specified 'rhs' managed pointer.  Note that 'rhs' will be
        // re-initialized to an unset state and that the previous contents of
        // this pointer (if any) are destroyed unless this pointer and 'rhs'
        // point to the same object.

    void clear();
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to an unset state.

    bsl::pair<TYPE*,bdema_ManagedPtrDeleter> release();
        // Return a raw pointer to the current managed object (if any) and its
        // current deleter and factory through the 'bdema_ManagedPtrDeleter'
        // member of the return value, and re-initialize this managed pointer
        // to an unset state.

    // ACCESSORS
    operator bool() const;
        // Return 'true' if this managed pointer currently has a value, or
        // 'false' otherwise.

    TYPE& operator*() const;
        // Return a reference to the managed instance.  The behavior is
        // undefined if this managed pointer is in an unset state.

    TYPE *operator->() const;
        // Return the address of the managed (modifiable) instance, or 0 if
        // this managed pointer is in an unset state.

    TYPE *ptr() const;
        // Return the address of the managed (modifiable) instance, or 0 if
        // this managed pointer is in an unset state.

    bdema_ManagedPtrDeleter const& deleter() const;
        // Return a non-modifiable reference to the deleter information
        // associated with this managed pointer.
};

                        // ===========================
                        // class bdema_ManagedPtrRef
                        // ===========================

template <typename TYPE>
class bdema_ManagedPtrRef {
    // This struct holds a managed pointer reference, returned by the implicit
    // conversion operator in the class 'bdema_ManagedPtr'.  This struct
    // is used to allow the construction of temporary managed pointer
    // objects (since temporaries cannot be passed by modifiable reference).

    bdema_ManagedPtr<TYPE> *d_ptr_p;

public:
    bdema_ManagedPtrRef(bdema_ManagedPtr<TYPE> *ptr);

    bdema_ManagedPtr<TYPE>* ptr() const;

    bdema_ManagedPtr<TYPE>* operator ->() const;
};


                        // ====================================
                        // class bdema_ManagedPtrFactoryDeleter
                        // ====================================

template <typename TYPE, typename FACTORY>
struct bdema_ManagedPtrFactoryDeleter {
    // This utility provides a general deleter for objects that provide
    // a 'deleteObject' operation (e.g., 'bdema_Allocator', 'bdema_Pool').

    static void deleter(TYPE *object, FACTORY *factory);
        // Deleter function that deletes the specified 'object' by invoking
        // 'deleteObject' on the specified 'factory'.  Note that the behavior
        // is undefined if 0 == 'object' or if 'factory' does not point to an
        // an object of type 'FACTORY'.
};

                      // ================================
                      // class bdema_ManagedPtrNilDeleter
                      // ================================

template <typename TYPE>
struct bdema_ManagedPtrNilDeleter {
    // This utility provides a general no-op deleter, which is useful when
    // creating managed pointers to stack-allocated objects.

    static void deleter(TYPE *object, void *);
        // Deleter function that does nothing.
};


// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------------
                        // class bdema_ManagedPtr
                        // ----------------------

// CREATORS
template <typename TYPE>
inline
bdema_ManagedPtr<TYPE>::bdema_ManagedPtr()
{
    rawClear();
}

template <typename TYPE>
inline
bdema_ManagedPtr<TYPE>::bdema_ManagedPtr(TYPE *ptr)
: d_ptr_p(ptr)
{
    if (!ptr) {
        return;
    }

    init(ptr, ptr,  bslma_Default::allocator(),
         reinterpret_cast<DeleterFunc>(&bdema_ManagedPtrFactoryDeleter<TYPE,
                                       bdema_Allocator>::deleter));
}

template <typename TYPE>
inline
bdema_ManagedPtr<TYPE>::bdema_ManagedPtr(bdema_ManagedPtrRef<TYPE> ref)
: d_ptr_p(ref->ptr())
, d_deleter(ref->d_deleter)
{
    if (this->d_ptr_p) {
        ref->rawClear();
    }
}

template <typename TYPE>
template <typename OTHER>
inline
bdema_ManagedPtr<TYPE>::bdema_ManagedPtr(bdema_ManagedPtrRef<OTHER> ref)
: d_ptr_p(ref->ptr())
, d_deleter(ref->d_deleter)
{
    if(this->d_ptr_p) {
        ref->rawClear();
    }
}

template <typename TYPE>
inline
bdema_ManagedPtr<TYPE>::bdema_ManagedPtr(bdema_ManagedPtr<TYPE>& original)
: d_ptr_p(original.ptr())
, d_deleter(original.d_deleter)
{
    if (this->d_ptr_p) {
        original.rawClear();
    }
}

template <typename TYPE>
template <typename OTHER>
inline
bdema_ManagedPtr<TYPE>::bdema_ManagedPtr(bdema_ManagedPtr<OTHER>& original)
: d_ptr_p(original.ptr())
, d_deleter(original.d_deleter)
{
    if (d_ptr_p) {
        original.release();
    }
}


template <typename TYPE>
template <typename OTHER>
inline
bdema_ManagedPtr<TYPE>::bdema_ManagedPtr(bdema_ManagedPtr<OTHER>& alias,
                                         TYPE                    *ptr)
{
    if (ptr && alias.d_ptr_p) {
        init(ptr, &alias.d_deleter);
        alias.rawClear();
    }
    else {
        rawClear();
    }
}

template <typename TYPE>
inline
bdema_ManagedPtr<TYPE>::bdema_ManagedPtr(TYPE  *ptr,
                                         void  *factory,
                                         void (*deleter)(TYPE*,void*))
{
    if (ptr) {
        init(ptr, ptr, factory, reinterpret_cast<DeleterFunc>(deleter));
    }
    else {
        rawClear();
    }
}

template <typename TYPE>
template <typename FACTORY>
bdema_ManagedPtr<TYPE>::bdema_ManagedPtr(TYPE    *ptr,
                                         FACTORY *factory)
{
    init(ptr, ptr, factory,
            reinterpret_cast<DeleterFunc>(
                &bdema_ManagedPtrFactoryDeleter<TYPE,
                FACTORY>::deleter));
}

template <typename TYPE>
template <typename FACTORY>
inline
bdema_ManagedPtr<TYPE>::bdema_ManagedPtr(TYPE     *ptr,
                                         FACTORY  *factory,
                                         void    (*deleter)(TYPE *, FACTORY*) )
{
    init(ptr, ptr, factory, reinterpret_cast<DeleterFunc>(deleter));
}

template <typename TYPE>
inline
bdema_ManagedPtr<TYPE>::~bdema_ManagedPtr()
{
    if (this->d_ptr_p) {
        this->d_deleter.deleteManagedObject();
    }
}

// PRIVATE MANIPULATORS
template <typename TYPE>
inline
void bdema_ManagedPtr<TYPE>::init(TYPE *ptr, Deleter *rep)
{
    this->d_ptr_p = ptr;

    if (ptr) {
        this->d_deleter = *rep;
    }
    else {
        this->d_deleter.clear();
    }
}

template <typename TYPE>
inline
void bdema_ManagedPtr<TYPE>::init(TYPE *ptr, void *object,
                                         void *factory, DeleterFunc deleter)
{
    this->d_ptr_p = ptr;

    if (ptr) {
        this->d_deleter.set(object, factory, deleter);
    }
    else {
        this->d_deleter.clear();
    }
}

template <typename TYPE>
inline
void bdema_ManagedPtr<TYPE>::reset(TYPE *ptr, Deleter *rep)
{
    if (this->d_ptr_p) {
        this->d_deleter.deleteManagedObject();
    }

    this->d_ptr_p = ptr;
    if (ptr) {
        this->d_deleter = *rep;
    }
    else {
        this->d_deleter.clear();
    }

}


template <typename TYPE>
inline
void bdema_ManagedPtr<TYPE>::reset(TYPE        *ptr,
                                   void        *object,
                                   void        *factory,
                                   DeleterFunc  deleter)
{
    if (this->d_ptr_p) {
        this->d_deleter.deleteManagedObject();
    }

    this->d_ptr_p = ptr;
    if (ptr) {
        this->d_deleter.set(object, factory, deleter);
    }
    else {
        this->d_deleter.clear();
    }
}

template <typename TYPE>
inline
void bdema_ManagedPtr<TYPE>::rawClear()
{
    this->d_ptr_p = 0;
    this->d_deleter.clear();
}

// MANIPULATORS
template <typename TYPE>
inline
void bdema_ManagedPtr<TYPE>::load(TYPE *ptr)
{
    reset(ptr, ptr, bslma_Default::allocator(),
          reinterpret_cast<DeleterFunc>(
                                    &bdema_ManagedPtrFactoryDeleter<TYPE,
                                    bdema_Allocator>::deleter));
}

template <typename TYPE>
template <typename FACTORY>
inline
void bdema_ManagedPtr<TYPE>::load(TYPE *ptr, FACTORY *factory)
{
    reset(ptr, ptr, factory,
            reinterpret_cast<DeleterFunc>(
                &bdema_ManagedPtrFactoryDeleter<TYPE,
                FACTORY>::deleter));
}

template <typename TYPE>
inline
void bdema_ManagedPtr<TYPE>::load(TYPE  *ptr,
                                  void  *factory,
                                  void (*deleter)(TYPE*, void*))
{
    reset(ptr, ptr, factory, reinterpret_cast<DeleterFunc>(deleter));
}

template <typename TYPE>
template <typename FACTORY>
inline
void bdema_ManagedPtr<TYPE>::load(TYPE     *ptr,
                                  FACTORY  *factory,
                                  void    (*deleter)(TYPE*, FACTORY*))
{
    reset(ptr, ptr, factory, reinterpret_cast<DeleterFunc>(deleter));
}

template <typename TYPE>
template <typename OTHER>
inline
void bdema_ManagedPtr<TYPE>::loadAlias(bdema_ManagedPtr<OTHER>& alias,
                                       TYPE                    *ptr)
{
    if (ptr && alias.d_ptr_p) {
        if ((void*)this != (void*)&alias) {
            reset(ptr, &alias.d_deleter);
            alias.rawClear();
        }
        else {
            // create alias to subpart, parent, or peer of self
            d_ptr_p = ptr;
        }
    }
    else {
        reset(0, 0);
    }
}


template <typename TYPE>
inline
void bdema_ManagedPtr<TYPE>::swap(bdema_ManagedPtr<TYPE>& rhs)
{
    bsl::swap(this->d_ptr_p, rhs.d_ptr_p);
    bsl::swap(this->d_deleter, rhs.d_deleter);
}

template <typename TYPE>
inline
void bdema_ManagedPtr<TYPE>::clear()
{
    reset(0,0);
}

template<class TYPE>
inline
bsl::pair<TYPE*,bdema_ManagedPtrDeleter> bdema_ManagedPtr<TYPE>::release()
{
    bsl::pair<TYPE*,bdema_ManagedPtrDeleter> ret(d_ptr_p, d_deleter);
    return rawClear(), ret;
}

template <typename TYPE>
inline
bdema_ManagedPtr<TYPE>&
    bdema_ManagedPtr<TYPE>::operator=(bdema_ManagedPtr<TYPE>& rhs)
{
    if (this == &rhs) {
        return *this;
    }

    reset(rhs.d_ptr_p, &rhs.d_deleter);
    rhs.rawClear();
    return *this;
}

template <typename TYPE>
inline
bdema_ManagedPtr<TYPE>&
    bdema_ManagedPtr<TYPE>::operator=(bdema_ManagedPtrRef<TYPE> ref)
{
    if (this == ref.ptr()) {
        return *this;
    }

    reset(ref->d_ptr_p, &ref->d_deleter);
    ref->rawClear();
    return *this;
}

template <typename TYPE>
template <typename OTHER>
inline
bdema_ManagedPtr<TYPE>&
bdema_ManagedPtr<TYPE>::operator=(bdema_ManagedPtr<OTHER>& rhs)
{
    if ((void *)this == (void *)&rhs) {
        // This should also works, but some of us are more comfortable
        // with the above void casts:
        //if (this == (bdema_ManagedPtr<TYPE> *)&rhs) return *this;

        return *this;
    }

    reset(rhs.d_ptr_p, &rhs.d_deleter);
    rhs.rawClear();
    return *this;
}

template <typename TYPE>
inline
bdema_ManagedPtr<TYPE>::operator bdema_ManagedPtrRef<TYPE>()
{
    return this;
}

// ACCESSORS

template <typename TYPE>
inline
bdema_ManagedPtr<TYPE>::operator bool() const
{
    return this->d_ptr_p;
}

template <typename TYPE>
inline
TYPE& bdema_ManagedPtr<TYPE>::operator*() const
{
    return *this->d_ptr_p;
}

template <typename TYPE>
inline
TYPE* bdema_ManagedPtr<TYPE>::operator->() const
{
    return this->d_ptr_p;
}

template <typename TYPE>
inline
const bdema_ManagedPtrDeleter& bdema_ManagedPtr<TYPE>::deleter() const
{
    return d_deleter;
}

template <typename TYPE>
inline
TYPE* bdema_ManagedPtr<TYPE>::ptr() const
{
    return this->d_ptr_p;
}

                        // ---------------------------
                        // class bdema_ManagedPtrRef
                        // ---------------------------

template <typename TYPE>
inline
bdema_ManagedPtrRef<TYPE>::bdema_ManagedPtrRef(bdema_ManagedPtr<TYPE> *ptr)
: d_ptr_p(ptr)
{
}

template <typename TYPE>
inline
bdema_ManagedPtr<TYPE>* bdema_ManagedPtrRef<TYPE>::ptr() const
{
    return this->d_ptr_p;
}


template <typename TYPE>
inline
bdema_ManagedPtr<TYPE>* bdema_ManagedPtrRef<TYPE>::operator->() const
{
    return this->d_ptr_p;
}

                        // ------------------------------------
                        // class bdema_ManagedPtrFactoryDeleter
                        // ------------------------------------

template <typename TYPE, class FACTORY>
void bdema_ManagedPtrFactoryDeleter<TYPE,FACTORY>
    ::deleter(TYPE *object, FACTORY *factory)
{
    factory->deleteObject(object);
}

                        // -----------------------------
                        // class bdema_ManagedPtrDeleter
                        // -----------------------------

inline
bdema_ManagedPtrDeleter::bdema_ManagedPtrDeleter()
: d_object_p(0)
, d_factory_p(0)
, d_deleter(0)
{
}

inline
bdema_ManagedPtrDeleter::bdema_ManagedPtrDeleter(
                                        bdema_ManagedPtrDeleter const&original)
: d_object_p(original.d_object_p)
, d_factory_p(original.d_factory_p)
, d_deleter(original.d_deleter)
{
}

inline
bdema_ManagedPtrDeleter::bdema_ManagedPtrDeleter(void    *object,
                                                 void    *factory,
                                                 Deleter  deleter)
: d_object_p(object)
, d_factory_p(factory)
, d_deleter(deleter)
{
}

inline
void bdema_ManagedPtrDeleter::set(void    *object,
                                  void    *factory,
                                  Deleter  deleter)
{
    d_object_p  = object;
    d_factory_p = factory;
    d_deleter   = deleter;
}

inline
void bdema_ManagedPtrDeleter::deleteManagedObject()
{
    d_deleter(d_object_p, d_factory_p);
}

inline
void bdema_ManagedPtrDeleter::clear()
{
    d_object_p  = 0;
    d_factory_p = 0;
    d_deleter   = 0;
}

inline
void *bdema_ManagedPtrDeleter::object() const
{
    return d_object_p;
}

inline
void *bdema_ManagedPtrDeleter::factory() const
{
    return d_factory_p;
}

inline
const bdema_ManagedPtrDeleter::Deleter&
bdema_ManagedPtrDeleter::deleter() const
{
    return d_deleter;
}

                    // --------------------------------
                    // class bdema_ManagedPtrNilDeleter
                    // --------------------------------

template <typename TYPE>
inline
void
bdema_ManagedPtrNilDeleter<TYPE>::deleter(TYPE *object, void *)
{
    // DO NOTHING
}

} // namespace BloombergLP {



#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
