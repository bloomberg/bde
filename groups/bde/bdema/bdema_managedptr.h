// bdema_managedptr.h              -*-C++-*-
#ifndef INCLUDED_BDEMA_MANAGEDPTR
#define INCLUDED_BDEMA_MANAGEDPTR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a managed pointer class.
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
// above (e.g. 'bslma_Allocator').  For example, to construct a managed pointer
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
// In a managed pointer, the pointer value (the value returned by the 'ptr'
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
//  double getQuote() // From ticker plant.  Simulated here
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
//  getFirstQuoteLargerThan(double threshold, bslma_Allocator *allocator)
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
//      bslma_TestAllocator ta;
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
// type 'A', can be directly assigned a 'bdema_ManagedPtr' of 'A'.
// In other words, consider the following code snippets:
//..
//  void implicitCastingExample()
//  {
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
//..
// and
//..
//      bdema_ManagedPtr<B> b_mp2;
//      bdema_ManagedPtr<A> a_mp2(b_mp2); // conversion construction
//  }
//..
// should also be valid.  Note that in all of the above cases, the proper
// destructor of 'B' will be invoked when the instance is destroyed even if
// 'A' does not provide a polymorphic destructor.
//
///Explicit Casting
/// - - - - - - - -
// Through "aliasing", a managed pointer of any type can be explicitly cast
// to a managed pointer of any other type using any legal cast expression.
// For example, to static-cast a managed pointer of type A to a managed pointer
// of type B, one can simply do the following:
//..
//  void explicitCastingExample()
//  {
//      bdema_ManagedPtr<A> a_mp1;
//      bdema_ManagedPtr<B> b_mp1(a_mp, static_cast<B*>(a_mp1.ptr()));
//..
// or even use the less safe "C"-style casts:
//..
//      bdema_ManagedPtr<A> a_mp2;
//      bdema_ManagedPtr<B> b_mp2(a_mp, (B*)(a_mp2.ptr()));
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

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

class bdema_ManagedPtr_Members;

template <typename TYPE,
          typename FACTORY> struct bdema_ManagedPtrFactoryDeleter;
template <typename OTHER>   class  bdema_ManagedPtr_Ref;
template <typename TYPE>    class  bdema_ManagedPtr;

                       // =============================
                       // class bdema_ManagedPtrDeleter
                       // =============================

class bdema_ManagedPtrDeleter {
    // This struct holds the information necessary for 'bdema_ManagedPtr' to
    // correctly manage its underlying object, namely the addresses of the
    // 'object' and 'factory', and the 'deleter' function, optionally supplied
    // through the constructors or through the 'set' method.  It is stored in a
    // sub-structure to allow the compiler to copy it more efficiently.

    // PRIVATE TYPES
    typedef void(*Deleter)(void *, void *);
        // Deleter function prototype used to destroy the managed pointer.

    // DATA
    void    *d_object_p;   // pointer to the actual managed object
    void    *d_factory_p;  // optional factory to be specified to the deleter
    Deleter  d_deleter;    // deleter used to destroy the managed object

    // FRIENDS
    friend class bdema_ManagedPtr_Members;
    template <typename TYPE> friend class bdema_ManagedPtr;

  public:
    //CREATORS
    bdema_ManagedPtrDeleter();
        // Create an uninitialized 'bdema_ManagedPtrDeleter' object that does
        // not refer to any object or factory instance.

    //! bdema_ManagedPtrDeleter(const bdema_ManagedPtrDeleter& original);
        // Create a 'bdema_ManagedPtrDeleter' object that that refers to the
        // same object or factory as the specified 'original' object.  Note
        // that this trivial constructor's definition is compiler generated.

    bdema_ManagedPtrDeleter(void *object, void *factory, Deleter deleter);
        // Create a 'bdema_ManagedPtrDeleter' struct that refers to the object
        // and factory instance located at the specified 'object' and 'factory'
        // memory locations, and the specified 'deleter'.

    //! ~bdema_ManagedPtrDeleter();
        // Destroy this 'bdema_ManagedPtrDeleter' object.  Note that this
        // trivial destructor's definition is compiler generated.


    //MANIPULATORS
    void deleteManagedObject();
        // Invoke the deleter object.  Behavior is not defined unless this
        // object has a deleter.

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

                     // =====================================
                     // private class bdema_ManagedPtrMembers
                     // =====================================

class bdema_ManagedPtr_Members {
    // Non-type-specific managed pointer member variables.  This type exists
    // so that a 'bdema_ManagedPtr_Ref' can point to the representation of a
    // 'bdema_ManagedPtr' even if the ref object is instantiated on a different
    // type than the managed pointer type (e.g. in the case of conversions).

    // Private copy operations (not implemented).  Although these operations
    // would be easy to provide, it is best if copying were prevented because
    // the use of a 'void*' can cause a type-safety hole in the case where a
    // managed pointer is copied to another managed pointer of a different
    // type.

    // NOT IMPLEMENTED
    bdema_ManagedPtr_Members(const bdema_ManagedPtr_Members&);
    bdema_ManagedPtr_Members operator=(const bdema_ManagedPtr_Members&);

  public:
    typedef bdema_ManagedPtrDeleter Deleter;
    typedef bdema_ManagedPtrDeleter::Deleter DeleterFunc;

    void    *d_obj_p;     // pointer to the managed instance.  A void pointer
                          // is used so that a bdem_ManagedPtrRef of a
                          // different type can point to this pointer.
    Deleter  d_deleter;   // deleter-related information

    bdema_ManagedPtr_Members();
        // Default constructor.  Sets 'd_obj_p' to 0.

    //! bdema_ManagedPtr_Members(const bdema_ManagedPtr_Members& original);
        // Create a 'bdema_ManagedPtr_Members' object having the same 'd_obj_p'
        // and 'd_deleter' values as the specified 'original'.  Note that this
        // trivial constructor's definition is compiler generated.

    //! ~bdema_ManagedPtr_Members();
        // Destroy this 'bdema_ManagedPtr_Members' object.  Note that this
        // trivial destructor's definition is compiler generated.

    void rawClear();
        // Reset this managed pointer to an unset state.  If this managed
        // pointer currently has a value, then the managed instance will not
        // be destroyed.
};

                  // =============================================
                  // private struct bdema_ManagedPtr_ReferenceType
                  // =============================================

template <class TYPE>
struct bdema_ManagedPtr_ReferenceType {
    // This class defines some basic traits used by 'bdema_ManagedPtr'.
    // It is primarily used to allow shared pointers of type 'void'
    // to work properly.

    typedef TYPE& Reference;
};

template <>
struct bdema_ManagedPtr_ReferenceType<void> {
    // This specialization of 'bdema_ManagedPtr_ReferenceType' for type 'void'
    // allows to avoid declaring a reference to 'void'.

    typedef void Reference;
};

            // =====================================================
            // private struct bdema_ManagedPtr_UnspecifiedBoolHelper
            // =====================================================

template<class BDEMA_MANAGED_TYPE>
struct bdema_ManagedPtr_UnspecifiedBoolHelper {
    // This 'struct' provides a member, 'd_member', whose pointer-to-member is
    // used to convert a managed pointer to an "unspecified boolean type".  The
    // specified 'BDEMA_MANAGED_TYPE' type parameter ensures that different
    // types can be used by distint classes using this dummy type to support
    // boolean conversion, otherwise two distinct types (or
    // template instantiations) would compare equal through 'operator=='
    // seeing the implicit conversion to this type.

    int d_member;
        // This data member is used solely for taking its address to return a
        // non-null pointer-to-member.  Note that the *value* of 'd_member' is
        // not used.
};

                           // ======================
                           // class bdema_ManagedPtr
                           // ======================

template <typename TYPE>
class bdema_ManagedPtr {
    // This class provides a "smart pointer" to an object of the parameterized
    // 'TYPE', which supports sole ownership of objects: a managed pointer
    // ensures that the object it manages is destroyed when the managed pointer
    // is destroyed, using the appropriate deletion method.  The object (of the
    // parameterized 'TYPE') pointed to by a shared pointer instance may be
    // accessed directly using the '->' operator, or the dereference operator
    // (operator '*') can be used to get a reference to that object.  See the
    // component-level documentation for a thorough description and examples of
    // its many usages.
    //
    // Note that the object managed by a managed pointer instance is usually
    // the same as the object pointed to by that instance (of the same 'TYPE'),
    // but this need not always be true in the presence of conversions or
    // "aliasing": the object pointed-to, of the parameterized 'TYPE', may
    // differ from the managed object of type 'OTHERTYPE' (see the section
    // "Aliasing" in the component-level documentation).  Nevertheless, both
    // shall exist or else the managed pointer is *unset* (i.e., manages no
    // object, has no deleter, and points to 0).

    // PRIVATE TYPES
    typedef bdema_ManagedPtrDeleter          Deleter;
    typedef bdema_ManagedPtrDeleter::Deleter DeleterFunc;

    typedef int bdema_ManagedPtr_UnspecifiedBoolHelper
                                             <bdema_ManagedPtr<BDEMA_TYPE> >::*
                                              bdema_ManagedPtr_UnspecifiedBool;
    // 'bdema_ManagedPtr_UnspecifiedBool' is an alias for a pointer-to-member
    // of the 'bdema_ManagedPtr_UnspecifiedBoolHelper' class.  This (opaque)
    // type can be used as an "unspecified boolean type" for converting a
    // managed pointer to 'bool' in contexts such as 'if (mp) { ... }' without
    // actually having a conversion to 'bool' or being less-than comparable
    // (either of which would also enable undesirable implicit comparisons of
    // managed pointers to 'int' and less-than comparisons).

    //DATA
    bdema_ManagedPtr_Members d_members;

    // PRIVATE MANIPULATORS
    void setPtr(TYPE *ptr);
        // Set 'd_members.d_obj_p' to 'ptr', using appropriate const-casts,
        // etc.  To avoid accidental type-safety errors, the
        // 'd_members.d_obj_p' variable should never be set directly except by
        // this function.

    void init(TYPE *ptr, const Deleter& rep);
        // Initialize this managed pointer to the specified 'ptr' pointer
        // value and the specified deleter.  Note that this function does
        // not destroy the current managed instance.

    void init(TYPE *ptr, void *object, void *factory, DeleterFunc deleter);
        // Initialize this managed pointer to the specified 'ptr' pointer
        // value, the specified 'object' pointer value, and the specified
        // 'deleter' function, using the specified 'factory'.  The specified
        // 'ptr' points to the object that will be referred to by the '*' and
        // '->' operators, while the specified 'object' points to the object
        // that will be destroyed and deallocated when this ManagedPtr is
        // destroyed.

    void reset(TYPE *ptr, Deleter *deleter);
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to the specified 'ptr' pointer value and the
        // specified 'deleter'.

    void reset(TYPE *ptr, void *object, void *factory, DeleterFunc deleter);
        // Destroy the current managed object(if any) and re-initialize this
        // managed pointer to the specified 'ptr' pointer value, the specified
        // 'object' pointer value, and the specified 'deleter' function, using
        // the specified 'factory'.  The specified 'ptr' points to the object
        // that will be referred to by the '*' and '->' operators, while the
        // specified 'object' points to the object that will be destroyed and
        // deallocated when this ManagedPtr is destroyed.

    void rawClear();
        // Reset this managed pointer to an unset state.  If this managed
        // pointer currently has a value, then the managed instance will not
        // be destroyed.

    // FRIENDS
    template <typename OTHER> friend class bdema_ManagedPtr;

  public:
    // TYPES
    typedef TYPE                 ElementType;
        // 'ElementType' is an alias to the parameterized 'TYPE' passed as
        // first and only template parameter to the 'bdema_ManagedPtr' class
        // template.

    // CREATORS
    bdema_ManagedPtr();
        // Construct a managed pointer that is in an unset state.

    explicit bdema_ManagedPtr(TYPE *ptr);
        // Construct a managed pointer that manages the specified 'ptr' using
        // the current default allocator to destroy 'ptr' when this managed
        // pointer is destroyed or re-assigned, unless it is released before
        // then.  Note that the object will be initialized to an unset state
        // if 'ptr' == 0.  The behavior is undefined if 'ptr' is already
        // managed by another managed pointer.

    bdema_ManagedPtr(bdema_ManagedPtr_Ref<TYPE> ref);
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
        // 'bslma_Allocator' or any class derived from 'bslma_Allocator' meets
        // the requirements for 'FACTORY'.  The behavior is undefined if 'ptr'
        // is already managed by another managed pointer.

    bdema_ManagedPtr(TYPE *ptr, void *factory, void (*deleter)(TYPE*, void*));
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
    operator bdema_ManagedPtr_Ref<TYPE>();
    template <typename OTHER>
    operator bdema_ManagedPtr_Ref<OTHER>();
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

    template <typename OTHER>
    bdema_ManagedPtr<TYPE>& operator=(bdema_ManagedPtr<OTHER>& rhs);
        // Assign to this managed pointer the value and ownership of the
        // specified 'rhs' managed pointer.  Note that 'rhs' will be
        // re-initialized to an unset state and that the previous contents of
        // this pointer (if any) are destroyed unless this pointer and 'rhs'
        // point to the same object.  Note the deleter from rhs will
        // be copied, so at destruction, that, rather than ~TYPE(), will
        // be called.

    bdema_ManagedPtr<TYPE>& operator=(bdema_ManagedPtr_Ref<TYPE> ref);
        // Transfer the value and ownership from the specified 'ref' managed
        // pointer to this managed pointer.  Note that 'ref' will be
        // re-initialized to an unset state and that the previous contents of
        // this pointer (if any) are destroyed unless this pointer and 'ref'
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
    operator bdema_ManagedPtr_UnspecifiedBool() const;
        // Return a value of the "unspecified bool" that evaluates to 'false'
        // if this managed pointer is in an unset state, and 'true' otherwise.
        // Note that this conversion operator allows a managed pointer to be
        // used within a conditional context (e.g., within an 'if' or 'while'
        // statement), but does *not* allow managed pointers to be compared
        // (e.g., via '<' or '>').

    typename bdema_ManagedPtr_ReferenceType<TYPE>::Reference
    operator*() const;
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

                     // =================================
                     // private class bdema_ManagedPtr_Ref
                     // =================================

template <typename TYPE>
class bdema_ManagedPtr_Ref {
    // This struct holds a managed pointer reference, returned by the implicit
    // conversion operator in the class 'bdema_ManagedPtr'.  This struct
    // is used to allow the construction of temporary managed pointer
    // objects (since temporaries cannot be passed by reference to a
    // modifiable).
    // TBD document class invariant, '0 != d_base_p'

    bdema_ManagedPtr_Members *d_base_p;
    TYPE                     *d_obj_p;

public:
    bdema_ManagedPtr_Ref(bdema_ManagedPtr_Members *base, TYPE *ptr);
        // Create a 'bdema_ManagedPtr_Ref' object having the specified 'base'
        // value for its 'base' attribute, and the specified 'ptr' value for
        // its 'ptr' attribute.  Behavior is undefined unless '0 != base'.


    //! bdema_ManagedPtr_Ref(const bdema_ManagedPtr_Ref& original);
        // Create a 'bdema_ManagedPtr_Ref' object having the same 'd_base_p'
        // and 'd_obj_p' values as the specified 'original'.  Note that this
        // trivial constructor's definition is compiler generated.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    // The following destructor is generated by the compiler, except in "SAFE"
    // build modes (e.g., to enable the checking of class invariants).

    ~bdema_ManagedPtr_Ref();
        // Destroy this object.
#endif

    TYPE *ptr() const;

    bdema_ManagedPtr_Members *operator ->() const;

    bdema_ManagedPtr_Members *base() const;
};

                    // =====================================
                    // struct bdema_ManagedPtrFactoryDeleter
                    // =====================================

template <typename TYPE, typename FACTORY>
struct bdema_ManagedPtrFactoryDeleter {
    // This utility provides a general deleter for objects that provide
    // a 'deleteObject' operation (e.g., 'bslma_Allocator', 'bdema_Pool').

    static void deleter(TYPE *object, FACTORY *factory);
        // Deleter function that deletes the specified 'object' by invoking
        // 'deleteObject' on the specified 'factory'.  Note that the behavior
        // is undefined if 0 == 'object' or if 'factory' does not point to an
        // an object of type 'FACTORY'.
};

                      // =================================
                      // struct bdema_ManagedPtrNilDeleter
                      // =================================

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

                    // --------------------------------------
                    // private class bdema_ManagedPtr_Members
                    // --------------------------------------

// CREATOR
inline
bdema_ManagedPtr_Members::bdema_ManagedPtr_Members()
: d_obj_p(0)
{
}

// MANIPULATOR
inline
void bdema_ManagedPtr_Members::rawClear()
{
    d_obj_p = 0;
    d_deleter.clear();
}

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
{
    setPtr(ptr);

    if (! ptr) {
        return;
    }

    init(ptr, ptr,  bslma_Default::allocator(),
         reinterpret_cast<DeleterFunc>(&bdema_ManagedPtrFactoryDeleter<TYPE,
                                       bslma_Allocator>::deleter));
}

template <typename TYPE>
bdema_ManagedPtr<TYPE>::bdema_ManagedPtr(bdema_ManagedPtr_Ref<TYPE> ref)
{
    init(ref.ptr(), ref.base()->d_deleter);
    if (d_members.d_obj_p) {
        ref->rawClear();
    }
}

template <typename TYPE>
inline
bdema_ManagedPtr<TYPE>::bdema_ManagedPtr(bdema_ManagedPtr<TYPE>& original)
{
    init(original.ptr(), original.d_members.d_deleter);
    if (d_members.d_obj_p) {
        original.rawClear();
    }
}

template <typename TYPE>
template <typename OTHER>
inline
bdema_ManagedPtr<TYPE>::bdema_ManagedPtr(bdema_ManagedPtr<OTHER>& original)
{
    init(original.ptr(), original.d_members.d_deleter);
    if (d_members.d_obj_p) {
        original.release();
    }
}

template <typename TYPE>
template <typename OTHER>
bdema_ManagedPtr<TYPE>::bdema_ManagedPtr(bdema_ManagedPtr<OTHER>& alias,
                                         TYPE                    *ptr)
{
    if (ptr && alias.d_members.d_obj_p) {
        init(ptr, alias.d_members.d_deleter);
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
inline
bdema_ManagedPtr<TYPE>::bdema_ManagedPtr(TYPE *ptr, FACTORY *factory)
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
    if (d_members.d_obj_p) {
        d_members.d_deleter.deleteManagedObject();
    }
}

// MANIPULATORS
template <typename TYPE>
inline
void bdema_ManagedPtr<TYPE>::setPtr(TYPE *ptr)
{
    d_members.d_obj_p = const_cast<void*>(static_cast<const void*>(ptr));
}

template <typename TYPE>
inline
void bdema_ManagedPtr<TYPE>::init(TYPE *ptr, const Deleter& rep)
{
    setPtr(ptr);
    if (ptr) {
        d_members.d_deleter = rep;
    }
    else {
        d_members.d_deleter.clear();
    }
}

template <typename TYPE>
inline
void bdema_ManagedPtr<TYPE>::init(TYPE        *ptr,
                                  void        *object,
                                  void        *factory,
                                  DeleterFunc deleter)
{
    setPtr(ptr);
    if (ptr) {
        d_members.d_deleter.set(object, factory, deleter);
    }
    else {
        d_members.d_deleter.clear();
    }
}

template <typename TYPE>
inline
void bdema_ManagedPtr<TYPE>::reset(TYPE *ptr, Deleter *rep)
{
    BSLS_ASSERT_SAFE(0 != rep || 0 == ptr);

    if (d_members.d_obj_p) {
        d_members.d_deleter.deleteManagedObject();
    }

    setPtr(ptr);
    if (ptr) {
        d_members.d_deleter = *rep;
    }
    else {
        d_members.d_deleter.clear();
    }
}


template <typename TYPE>
inline
void bdema_ManagedPtr<TYPE>::reset(TYPE        *ptr,
                                   void        *object,
                                   void        *factory,
                                   DeleterFunc  deleter)
{
    if (d_members.d_obj_p) {
        d_members.d_deleter.deleteManagedObject();
    }

    setPtr(ptr);
    if (ptr) {
        d_members.d_deleter.set(object, factory, deleter);
    }
    else {
        d_members.d_deleter.clear();
    }
}

template <typename TYPE>
inline
void bdema_ManagedPtr<TYPE>::rawClear()
{
    d_members.rawClear();
}

template <typename TYPE>
inline
void bdema_ManagedPtr<TYPE>::load(TYPE *ptr)
{
    reset(ptr, ptr, bslma_Default::allocator(),
          reinterpret_cast<DeleterFunc>(
                                    &bdema_ManagedPtrFactoryDeleter<TYPE,
                                    bslma_Allocator>::deleter));
}

template <typename TYPE>
inline
void bdema_ManagedPtr<TYPE>::load(TYPE *ptr,
                                  void *factory,
                                  void(*deleter)(TYPE*, void*))
{
    reset(ptr, ptr, factory, reinterpret_cast<DeleterFunc>(deleter));
}

template <typename TYPE>
inline
void bdema_ManagedPtr<TYPE>::clear()
{
    reset(0,0);
}

template<typename TYPE>
bsl::pair<TYPE*,bdema_ManagedPtrDeleter> bdema_ManagedPtr<TYPE>::release()
{
    TYPE *p = ptr();
    bsl::pair<TYPE*,bdema_ManagedPtrDeleter> ret(p, d_members.d_deleter);
    rawClear();
    return ret;
}

template <typename TYPE>
template <typename FACTORY>
inline
void bdema_ManagedPtr<TYPE>::load(TYPE *ptr, FACTORY *factory)
{
    reset(ptr, ptr, factory,
            reinterpret_cast<DeleterFunc>(
              &bdema_ManagedPtrFactoryDeleter<TYPE, FACTORY>::deleter));
}

template <typename TYPE>
template <typename FACTORY>
inline
void bdema_ManagedPtr<TYPE>::load(TYPE    *ptr,
                                  FACTORY *factory,
                                  void   (*deleter)(TYPE *, FACTORY*))
{
    reset(ptr, ptr, factory, reinterpret_cast<DeleterFunc>(deleter));
}

template <typename TYPE>
template <typename OTHER>
void
bdema_ManagedPtr<TYPE>::loadAlias(bdema_ManagedPtr<OTHER>& alias, TYPE *ptr)
{
    if (ptr && alias.d_members.d_obj_p) {
        if (&d_members != &alias.d_members) {
            reset(ptr, &alias.d_members.d_deleter);
            alias.rawClear();
        }
        else {
            // create alias to subpart, parent, or peer of self.
            setPtr(ptr);
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
    void *tmp_p             = d_members.d_obj_p;
    d_members.d_obj_p       = rhs.d_members.d_obj_p;
    rhs.d_members.d_obj_p   = tmp_p;

    Deleter tmp             = d_members.d_deleter;
    d_members.d_deleter     = rhs.d_members.d_deleter;
    rhs.d_members.d_deleter = tmp;
}

template <typename TYPE>
bdema_ManagedPtr<TYPE>&
    bdema_ManagedPtr<TYPE>::operator=(bdema_ManagedPtr<TYPE>& rhs)
{
    if (this == &rhs) return *this;

    reset(rhs.ptr(), &rhs.d_members.d_deleter);
    rhs.rawClear();
    return *this;
}

template <typename TYPE>
template <typename OTHER>
bdema_ManagedPtr<TYPE>&
bdema_ManagedPtr<TYPE>::operator=(bdema_ManagedPtr<OTHER>& rhs)
{
    if ((bdema_ManagedPtr_Members *)this == (bdema_ManagedPtr_Members *)&rhs) {
        return *this;
    }

    reset(rhs.ptr(), &rhs.d_members.d_deleter);
    rhs.rawClear();
    return *this;
}

template <typename TYPE>
inline
bdema_ManagedPtr<TYPE>&
bdema_ManagedPtr<TYPE>::operator=(bdema_ManagedPtr_Ref<TYPE> ref)
{
    if (&d_members == ref.base()) {
        return *this;
    }

    reset(ref.ptr(), &ref.base()->d_deleter);
    ref->rawClear();
    return *this;
}

template <typename TYPE>
inline
bdema_ManagedPtr<TYPE>::operator bdema_ManagedPtr_Ref<TYPE>()
{
    return bdema_ManagedPtr_Ref<TYPE>(&d_members, this->ptr());
}

template <typename TYPE>
template <typename OTHER>
inline
bdema_ManagedPtr<TYPE>::operator bdema_ManagedPtr_Ref<OTHER>()
{
    return bdema_ManagedPtr_Ref<OTHER>(&d_members, this->ptr());
}

// ACCESSORS
template <class TYPE>
inline
bdema_ManagedPtr<TYPE>::operator bdema_ManagedPtr_UnspecifiedBool() const
{
    return d_members.d_obj_p
         ? &bdema_ManagedPtr_UnspecifiedBoolHelper::d_member
         : 0;
}

template <typename TYPE>
inline
typename bdema_ManagedPtr_ReferenceType<TYPE>::Reference
bdema_ManagedPtr<TYPE>::operator*() const
{
    BSLS_ASSERT_SAFE(d_members.d_obj_p);
    return *static_cast<TYPE*>(d_members.d_obj_p);
}

template <typename TYPE>
inline
TYPE *bdema_ManagedPtr<TYPE>::operator->() const
{
    return static_cast<TYPE*>(d_members.d_obj_p);
}

template <typename TYPE>
inline
const bdema_ManagedPtrDeleter& bdema_ManagedPtr<TYPE>::deleter() const
{
    return d_members.d_deleter;
}

template <typename TYPE>
inline
TYPE *bdema_ManagedPtr<TYPE>::ptr() const
{
    return static_cast<TYPE*>(d_members.d_obj_p);
}

                      // ---------------------------------
                      // private class bdema_ManagedPtr_Ref
                      // ---------------------------------

// CREATOR
template <typename TYPE>
inline
bdema_ManagedPtr_Ref<TYPE>::bdema_ManagedPtr_Ref(
                                                bdema_ManagedPtr_Members *base,
                                                TYPE                     *ptr)
: d_base_p(base), d_obj_p(ptr)
{
    BSLS_ASSERT_SAFE(0 != base);
}

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
template <typename TYPE>
inline
bdema_ManagedPtr_Ref<TYPE>::~bdema_ManagedPtr_Ref()
{
    BSLS_ASSERT_SAFE(0 != d_base_p);
}
#endif

// MANIPULATORS
template <typename TYPE>
inline
TYPE *bdema_ManagedPtr_Ref<TYPE>::ptr() const
{
    return d_obj_p;
}

template <typename TYPE>
inline
bdema_ManagedPtr_Members *bdema_ManagedPtr_Ref<TYPE>::operator->() const
{
    return d_base_p;
}

template <typename TYPE>
inline
bdema_ManagedPtr_Members *bdema_ManagedPtr_Ref<TYPE>::base() const
{
    return d_base_p;
}

                       // -----------------------------
                       // class bdema_ManagedPtrDeleter
                       // -----------------------------

// CREATORS
inline
bdema_ManagedPtrDeleter::bdema_ManagedPtrDeleter()
: d_object_p(0)
, d_factory_p(0)
, d_deleter(0)
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

// MANIPULATORS
inline
void bdema_ManagedPtrDeleter::set(void *object, void *factory, Deleter deleter)
{
    d_object_p  = object;
    d_factory_p = factory;
    d_deleter   = deleter;
}

inline
void bdema_ManagedPtrDeleter::deleteManagedObject()
{
    BSLS_ASSERT_SAFE(0 != d_deleter);
    d_deleter(d_object_p, d_factory_p);
}

inline
void bdema_ManagedPtrDeleter::clear()
{
    d_object_p  = 0;
    d_factory_p = 0;
    d_deleter   = 0;
}

// ACCESSORS
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

                    // -------------------------------------
                    // struct bdema_ManagedPtrFactoryDeleter
                    // -------------------------------------

template <typename TYPE, typename FACTORY>
inline
void
bdema_ManagedPtrFactoryDeleter<TYPE,FACTORY>::deleter(TYPE    *object,
                                                      FACTORY *factory)
{
    BSLS_ASSERT_SAFE(0 != object);
    BSLS_ASSERT_SAFE(0 != factory);
    factory->deleteObject(object);
}

                    // ---------------------------------
                    // struct bdema_ManagedPtrNilDeleter
                    // ---------------------------------

template <typename TYPE>
inline
void
bdema_ManagedPtrNilDeleter<TYPE>::deleter(TYPE *object, void *)
{
    (void) object;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
