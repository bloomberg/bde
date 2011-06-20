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

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_ENABLEIF
#include <bslmf_enableif.h>
#endif

#ifndef INCLUDED_BSLMF_IS_CONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

#define NO_SFINAE

namespace BloombergLP {

class bslma_Allocator;

class bdema_ManagedPtr_Members;

template <class BDEMA_TYPE, typename FACTORY> 
struct bdema_ManagedPtrFactoryDeleter;
template <class OTHER>   class  bdema_ManagedPtr_Ref;
template <class BDEMA_TYPE>  class  bdema_ManagedPtr;

                       // =============================
                       // class bdema_ManagedPtrDeleter
                       // =============================

class bdema_ManagedPtrDeleter {
    // This struct holds the information necessary for 'bdema_ManagedPtr' to
    // correctly manage its underlying object, namely the addresses of the
    // 'object' and 'factory', and the 'deleter' function, optionally supplied
    // through the constructors or through the 'set' method.  It is stored in a
    // sub-structure to allow the compiler to copy it more efficiently.

public:
    // PRIVATE TYPES
    typedef void(*Deleter)(void *, void *);
        // Deleter function prototype used to destroy the managed pointer.

private:
    // DATA
    void    *d_object_p;   // pointer to the actual managed object
    void    *d_factory_p;  // optional factory to be specified to the deleter
    Deleter  d_deleter;    // deleter used to destroy the managed object

    // FRIENDS
//    friend class bdema_ManagedPtr_Members;
//    template <class BDEMA_TYPE> friend class bdema_ManagedPtr;

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

    Deleter deleter() const;
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
#if !defined(BDEMA_MANAGEDPTR_TEST_WITHOUT_REF_TRICK)
template <class BDEMA_TYPE>
struct bdema_ManagedPtr_ReferenceType {
    // This class defines some basic traits used by 'bdema_ManagedPtr'.
    // It is primarily used to allow shared pointers of type 'void'
    // to work properly.

    typedef BDEMA_TYPE& Reference;
    BSLMF_ASSERT(true);
};
#endif

template <class BDEMA_TYPE>
struct bdema_ManagedPtr_ReferenceType<BDEMA_TYPE &> {
    // This template partial specialization enforces the rule that (managed)
    // pointers to references are not allowed.

    BSLMF_ASSERT(sizeof BDEMA_TYPE < 0);
};

template <>
struct bdema_ManagedPtr_ReferenceType<void> {
    // This specialization of 'bdema_ManagedPtr_ReferenceType' for type
    // 'void' allows to avoid declaring a reference to 'void'.

    typedef void Reference;
};

template <>
struct bdema_ManagedPtr_ReferenceType<const void> {
    // This specialization of 'bdema_ManagedPtr_ReferenceType' for type
    // 'const void' allows to avoid declaring a reference to 'const void'.

    typedef void Reference;
};

template <>
struct bdema_ManagedPtr_ReferenceType<volatile void> {
    // This specialization of 'bdema_ManagedPtr_ReferenceType' for type
    // 'volatile void' allows to avoid declaring a reference to 'volatile void'.

    typedef void Reference;
};

template <>
struct bdema_ManagedPtr_ReferenceType<const volatile void> {
    // This specialization of 'bdema_ManagedPtr_ReferenceType' for type
    // 'const volatile void' allows to avoid declaring a reference to
    // 'const volatile void'.

    typedef void Reference;
};

            // =====================================================
            // private struct bdema_ManagedPtr_UnspecifiedBoolHelper
            // =====================================================

template<class BDEMA_MANAGED_TYPE>
struct bdema_ManagedPtr_UnspecifiedBoolHelper {
    // This 'struct' provides a member, 'd_member', whose pointer-to-member is
    // used to convert a managed pointer to an "unspecified boolean type".

    int d_member;
        // This data member is used solely for taking its address to return a
        // non-null pointer-to-member.  Note that the *value* of 'd_member' is
        // not used.
};

                           // ======================
                           // class bdema_ManagedPtr
                           // ======================

template <class BDEMA_TYPE>
class bdema_ManagedPtr {
    // This class provides a "smart pointer" to an object of the parameterized
    // 'BDEMA_TYPE', which supports sole ownership of objects: a managed
    // pointer ensures that the object it manages is destroyed when the managed
    // pointer is destroyed, using the appropriate deletion method.  The object
    // (of the parameterized 'BDEMA_TYPE') pointed to by a shared pointer
    // instance may be accessed directly using the '->' operator, or the
    // dereference operator (operator '*') can be used to get a reference to
    // that object.  See the component-level documentation for a thorough
    // description and examples of its many usages.
    //
    // Note that the object managed by a managed pointer instance is usually
    // the same as the object pointed to by that instance (of the same
    // 'BDEMA_TYPE'), but this need not always be true in the presence of
    // conversions or "aliasing": the object pointed-to, of the parameterized
    // 'BDEMA_TYPE', may differ from the managed object of type 'OTHERTYPE'
    // (see the section "Aliasing" in the component-level documentation).
    // Nevertheless, both shall exist or else the managed pointer is *unset*
    // (i.e., manages no object, has no deleter, and points to 0).

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

#ifndef BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
    struct MyNullptrProxy { int dummy; };
    typedef int MyNullptrProxy::* nullptr_t;
#else
    typedef native_std::nullptr_t nullptr_t;
#endif

    struct unspecified {}; // private type to guarantee bslma_EnableIf disambiguates

    //DATA
    bdema_ManagedPtr_Members d_members;

    // PRIVATE MANIPULATORS
    void setPtr(BDEMA_TYPE *ptr);
        // Set 'd_members.d_obj_p' to 'ptr', using appropriate const-casts,
        // etc.  To avoid accidental type-safety errors, the
        // 'd_members.d_obj_p' variable should never be set directly except by
        // this function.

    void init(BDEMA_TYPE *ptr, const Deleter& rep);
        // Initialize this managed pointer to the specified 'ptr' pointer
        // value and the specified deleter.  Note that this function does
        // not destroy the current managed instance.

    void init(BDEMA_TYPE *ptr, void *object, void *factory, DeleterFunc deleter);
        // Initialize this managed pointer to the specified 'ptr' pointer
        // value, the specified 'object' pointer value, and the specified
        // 'deleter' function, using the specified 'factory'.  The specified
        // 'ptr' points to the object that will be referred to by the '*' and
        // '->' operators, while the specified 'object' points to the object
        // that will be destroyed and deallocated when this ManagedPtr is
        // destroyed.

    void reset(BDEMA_TYPE *ptr, Deleter *deleter);
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to the specified 'ptr' pointer value and the
        // specified 'deleter'.

    void reset(BDEMA_TYPE *ptr, void *object, void *factory, DeleterFunc deleter);
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
    template <class OTHER> friend class bdema_ManagedPtr;

    template<class BDEMA_OTHER>
    void operator==(const BDEMA_OTHER&) const;
    template<class BDEMA_OTHER>
    void operator!=(const BDEMA_OTHER&) const;
        // The two operator overloads are declared as 'private' but never
        // defined in order to eliminate accidental equality comparisons that
        // would occur through the implicit conversion to an unspecified
        // boolean type.  Note that the return type of 'void' is chosen as it
        // will often produce a clearer error message than relying on the
        // 'private' control failure.

  public:
    // TYPES
    typedef BDEMA_TYPE ElementType;
        // 'ElementType' is an alias to the parameterized 'BDEMA_TYPE' passed as
        // first and only template parameter to the 'bdema_ManagedPtr' class
        // template.

    // CREATORS
    bdema_ManagedPtr();
        // Construct a managed pointer that is in an unset state.

    explicit bdema_ManagedPtr(nullptr_t);
        // Construct a managed pointer that is in an unset state.  Note that
        // this constructor is necessary to match a single null literal
        // argument, as the only other viable constructor is a template that
        // cannot deduce the pointed-to type from a null pointer.

    template<class BDEMA_TARGET_TYPE>
#if !defined (NO_SFINAE)
    explicit bdema_ManagedPtr(BDEMA_TARGET_TYPE *ptr, typename 
                       bslma_EnableIf<bslmf_IsConvertible<BDEMA_TARGET_TYPE *, 
                                                          BDEMA_TYPE *>::VALUE,
                                      unspecified>::type * = 0);
#else
    explicit bdema_ManagedPtr(BDEMA_TARGET_TYPE *ptr);
#endif
        // Construct a managed pointer that manages the specified 'ptr' using
        // the current default allocator to destroy 'ptr' when this managed
        // pointer is destroyed or re-assigned, unless it is released before
        // then.  Note that the object will be initialized to an unset state
        // if 'ptr' == 0.  The behavior is undefined if 'ptr' is already
        // managed by another managed pointer.

#if !defined(BDEMA_MANAGEDPTR_TEST_WITHOUT_REF_TRICK)
    bdema_ManagedPtr(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref);
        // Construct a 'bdema_ManagedPtr' and transfer the value and ownership
        // from the managed pointer referred-to by the specified 'ref' object
        // to this managed pointer.  Note that the managed pointer referred-to
        // by 'ref' will be re-initialized to an unset state.  This constructor
        // is used to construct from a managed pointer rvalue.
#endif

    bdema_ManagedPtr(bdema_ManagedPtr& original);
        // Construct a 'bdema_ManagedPtr' and transfer the value and ownership
        // from the specified 'original' managed pointer to this managed
        // pointer.  Note that 'original' will be re-initialized to an unset
        // state.

    template <class OTHER>
    bdema_ManagedPtr(bdema_ManagedPtr<OTHER>& original);
        // Construct a 'bdema_ManagedPtr' and transfer the value and ownership
        // from the specified 'original' managed pointer this managed pointer.
        // Note that 'original' will be re-initialized to an unset state.
        // Compilation will fail unless 'OTHER*' is convertible to 'BDEMA_TYPE*'.

    template <class OTHER>
    bdema_ManagedPtr(bdema_ManagedPtr<OTHER>& alias, BDEMA_TYPE *ptr);
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
        // Compilation will fail unless 'OTHER*' is convertible to 'BDEMA_TYPE*'.
        // The behavior is undefined if 'ptr' is already managed by a managed
        // pointer other than 'alias'.

    template <class FACTORY>
    bdema_ManagedPtr(BDEMA_TYPE *ptr, FACTORY *factory);
        // Construct a managed pointer to manage the specified 'ptr' using the
        // specified 'factory' to destroy 'ptr' when this managed pointer is
        // destroyed or re-assigned, unless it is released before then.  The
        // 'FACTORY' class can be any class that has a 'deleteObject' method.
        // 'bslma_Allocator' or any class derived from 'bslma_Allocator' meets
        // the requirements for 'FACTORY'.  The behavior is undefined if 'ptr'
        // is already managed by another managed pointer.

    bdema_ManagedPtr(BDEMA_TYPE *ptr,
                     void       *factory,
                     void      (*deleter)(BDEMA_TYPE *, void *));
    template <class FACTORY>
    bdema_ManagedPtr(BDEMA_TYPE *ptr,
                     FACTORY    *factory,
                     void      (*deleter)(BDEMA_TYPE *, FACTORY *));
        // Construct a managed pointer to manage the specified 'ptr' using the
        // specified 'deleter' and associated 'factory' to destroy 'ptr' when
        // this managed pointer is destroyed or re-assigned (unless it is
        // released before then).  Note that if 0 == 'ptr', then this object
        // will be initialized to an unset state.  The behavior is undefined
        // if 'ptr' is already managed by another managed pointer.

    ~bdema_ManagedPtr();
        // Destroy this managed pointer object and any managed instance by
        // invoking the user-supplied deleter.
        // AJM : note that this destructor does nothing if this object is unset
        //       i.e. it is managing a null pointer.
        // AJM : Note that even aliased null pointers do not execute a deleter.

    // MANIPULATORS
#if !defined(BDEMA_MANAGEDPTR_TEST_WITHOUT_REF_TRICK)
    operator bdema_ManagedPtr_Ref<BDEMA_TYPE>();
    template <class OTHER>
    operator bdema_ManagedPtr_Ref<OTHER>();
        // Implicit conversion to a managed pointer reference.  This
        // conversion operator is used to allow the construction of managed
        // pointer rvalues because temporaries cannot be passed by modifiable
        // reference.
        // TBD AJM notes that the test driver does not exercise case where
        // conversion is needed.
#endif

    void load(nullptr_t =0);
    template<class BDEMA_TARGET_TYPE>
#if !defined (NO_SFINAE)
    void load(BDEMA_TARGET_TYPE *ptr,
              typename bslma_EnableIf<
                        bslmf_IsConvertible<BDEMA_TARGET_TYPE *,
                                            BDEMA_TYPE *>::VALUE,
                        unspecified>::type * = 0);
#else
    void load(BDEMA_TARGET_TYPE *ptr);
#endif
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to manage the specified 'ptr' using the default
        // allocator to destroy 'ptr' when this managed pointer is destroyed
        // or re-assigned, unless it is released before then.  Note that if 0
        // == 'ptr', then this object will be initialized to an unset state.
        // The behavior is undefined if 'ptr' is already managed by another
        // managed pointer.

    template <class FACTORY>
    void load(BDEMA_TYPE *ptr, FACTORY *factory);
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to manage the specified 'ptr' using the specified
        // 'factory' deleter to destroy 'ptr' when this managed pointer is
        // destroyed or re-assigned, unless it is released before then.  Note
        // that if '0 == ptr', then this object will be initialized to an
        // unset state.  The behavior is undefined if 'ptr' is already managed
        // by another managed pointer.

    void load(BDEMA_TYPE *ptr,
              void       *factory,
              void      (*deleter)(BDEMA_TYPE *, void*));
    template <class FACTORY>
    void load(BDEMA_TYPE *ptr,
              FACTORY    *factory,
              void      (*deleter)(BDEMA_TYPE *, FACTORY*));
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to manage the specified 'ptr' using the specified
        // 'deleter' with arguments 'ptr' and the specified 'factory' to
        // destroy 'ptr' when this managed pointer is destroyed or
        // re-assigned, unless it is released before then.  Note that if
        // '0 == ptr', then this object will be initialized to an unset state.
        // The behavior is undefined if 'ptr' is already managed by another
        // managed pointer.

    template <class OTHER>
    void loadAlias(bdema_ManagedPtr<OTHER>& alias, BDEMA_TYPE *ptr);
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

    void swap(bdema_ManagedPtr& rhs);
        // Exchange the value and ownership of this managed pointer with the
        // specified 'rhs' managed pointer.

    bdema_ManagedPtr& operator=(bdema_ManagedPtr& rhs);
        // Assign to this managed pointer the value and ownership of the
        // specified 'rhs' managed pointer.  Note that 'rhs' will be
        // re-initialized to an unset state and that the previous contents of
        // this pointer (if any) are destroyed unless this pointer and 'rhs'
        // point to the same object.
        // TBD document 'return' and note no-op when self assigning.

    template <class OTHER>
    bdema_ManagedPtr& operator=(bdema_ManagedPtr<OTHER>& rhs);
        // Assign to this managed pointer the value and ownership of the
        // specified 'rhs' managed pointer.  Note that 'rhs' will be
        // re-initialized to an unset state and that the previous contents of
        // this pointer (if any) are destroyed unless this pointer and 'rhs'
        // point to the same object.  Note the deleter from rhs will
        // be copied, so at destruction, that, rather than ~BDEMA_TYPE(), will
        // be called.

#if !defined(BDEMA_MANAGEDPTR_TEST_WITHOUT_REF_TRICK)
    bdema_ManagedPtr& operator=(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref);
        // Transfer the value and ownership from the specified 'ref' managed
        // pointer to this managed pointer.  Note that 'ref' will be
        // re-initialized to an unset state and that the previous contents of
        // this pointer (if any) are destroyed unless this pointer and 'ref'
        // point to the same object.
#endif

    void clear();
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to an unset state.

    bsl::pair<BDEMA_TYPE*,bdema_ManagedPtrDeleter> release();
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

    typename bdema_ManagedPtr_ReferenceType<BDEMA_TYPE>::Reference
    operator*() const;
        // Return a reference to the managed instance.  The behavior is
        // undefined if this managed pointer is in an unset state.
        // TBD document 'void' as a special case

    BDEMA_TYPE *operator->() const;
        // Return the address of the managed (modifiable) instance, or 0 if
        // this managed pointer is in an unset state.

    BDEMA_TYPE *ptr() const;
        // Return the address of the managed (modifiable) instance, or 0 if
        // this managed pointer is in an unset state.

    const bdema_ManagedPtrDeleter& deleter() const;
        // Return a non-modifiable reference to the deleter information
        // associated with this managed pointer.
};

#if !defined(BDEMA_MANAGEDPTR_TEST_WITHOUT_REF_TRICK)
                     // ==================================
                     // private class bdema_ManagedPtr_Ref
                     // ==================================

template <class BDEMA_TYPE>
class bdema_ManagedPtr_Ref {
    // This struct holds a managed pointer reference, returned by the implicit
    // conversion operator in the class 'bdema_ManagedPtr'.  This struct
    // is used to allow the construction of temporary managed pointer
    // objects (since temporaries cannot be passed by reference to a
    // modifiable).
    // TBD document class invariant, '0 != d_base_p'

    bdema_ManagedPtr_Members *d_base_p;
    BDEMA_TYPE               *d_obj_p;

public:
    bdema_ManagedPtr_Ref(bdema_ManagedPtr_Members *base, BDEMA_TYPE *ptr);
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

    BDEMA_TYPE *ptr() const;

    bdema_ManagedPtr_Members *operator ->() const;

    bdema_ManagedPtr_Members *base() const;
};
#endif
                    // =====================================
                    // struct bdema_ManagedPtrFactoryDeleter
                    // =====================================

template <class BDEMA_TYPE, typename FACTORY>
struct bdema_ManagedPtrFactoryDeleter {
    // This utility provides a general deleter for objects that provide
    // a 'deleteObject' operation (e.g., 'bslma_Allocator', 'bdema_Pool').

    static void deleter(BDEMA_TYPE *object, FACTORY *factory);
        // Deleter function that deletes the specified 'object' by invoking
        // 'deleteObject' on the specified 'factory'.  Note that the behavior
        // is undefined if 0 == 'object' or if 'factory' does not point to an
        // an object of type 'FACTORY'.
};

                      // =================================
                      // struct bdema_ManagedPtrNilDeleter
                      // =================================

template <class BDEMA_TYPE>
struct bdema_ManagedPtrNilDeleter {
    // This utility provides a general no-op deleter, which is useful when
    // creating managed pointers to stack-allocated objects.

    static void deleter(BDEMA_TYPE *object, void *);
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
template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr()
{
    rawClear();
}

template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(nullptr_t)
{
    rawClear();
}

#if !defined(NO_SFINAE)
template<class BDEMA_TYPE>
template<class BDEMA_TARGET_TYPE>
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(BDEMA_TARGET_TYPE * ptr,
        typename bslma_EnableIf<bslmf_IsConvertible<BDEMA_TARGET_TYPE *,
                                                    BDEMA_TYPE *>::VALUE,
                                unspecified>::type *)
{
#else
template <class BDEMA_TYPE>
template <class BDEMA_TARGET_TYPE> 
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(BDEMA_TARGET_TYPE *ptr)
{
    BSLMF_ASSERT((bslmf_IsConvertible<BDEMA_TARGET_TYPE *,
                                      BDEMA_TYPE *>::VALUE));
#endif
    setPtr(ptr);

    if (! ptr) {
        return;
    }

    typedef bdema_ManagedPtrFactoryDeleter<BDEMA_TARGET_TYPE,bslma_Allocator>
                                                                DeleterFactory;
    init(ptr, ptr, bslma_Default::allocator(),
         reinterpret_cast<DeleterFunc>(&DeleterFactory::deleter));
}

#if !defined(BDEMA_MANAGEDPTR_TEST_WITHOUT_REF_TRICK)
template <class BDEMA_TYPE>
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(
                                          bdema_ManagedPtr_Ref<BDEMA_TYPE> ref)
{
    init(ref.ptr(), ref.base()->d_deleter);
    if (d_members.d_obj_p) {
        ref->rawClear();
    }
}
#endif

template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(
                                        bdema_ManagedPtr<BDEMA_TYPE>& original)
{
    init(original.ptr(), original.d_members.d_deleter);
    if (d_members.d_obj_p) {
        original.rawClear();
    }
}

template <class BDEMA_TYPE>
template <class OTHER>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(bdema_ManagedPtr<OTHER>&
                                                                      original)
{
    init(original.ptr(), original.d_members.d_deleter);
    if (d_members.d_obj_p) {
        original.release();
    }
}

template <class BDEMA_TYPE>
template <class OTHER>
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(bdema_ManagedPtr<OTHER>& alias,
                                         BDEMA_TYPE                    *ptr)
{
    if (ptr && alias.d_members.d_obj_p) {
        init(ptr, alias.d_members.d_deleter);
        alias.rawClear();
    }
    else {
        rawClear();
    }
}

template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(BDEMA_TYPE  *ptr,
                                         void  *factory,
                                         void (*deleter)(BDEMA_TYPE*,void*))
{
    if (ptr) {
        init(ptr, ptr, factory, reinterpret_cast<DeleterFunc>(deleter));
    }
    else {
        rawClear();
    }
}

template <class BDEMA_TYPE>
template <class FACTORY>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(BDEMA_TYPE *ptr, FACTORY *factory)
{
    init(ptr, ptr, factory, reinterpret_cast<DeleterFunc>(
               &bdema_ManagedPtrFactoryDeleter<BDEMA_TYPE, FACTORY>::deleter));
}

template <class BDEMA_TYPE>
template <class FACTORY>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(
                                 BDEMA_TYPE *ptr,
                                 FACTORY    *factory,
                                 void      (*deleter)(BDEMA_TYPE *, FACTORY*) )
{
    init(ptr, ptr, factory, reinterpret_cast<DeleterFunc>(deleter));
}

template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::~bdema_ManagedPtr()
{
    if (d_members.d_obj_p) {
        d_members.d_deleter.deleteManagedObject();
    }
}

// MANIPULATORS
template <class BDEMA_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::setPtr(BDEMA_TYPE *ptr)
{
    d_members.d_obj_p = const_cast<void*>(static_cast<const void*>(ptr));
}

template <class BDEMA_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::init(BDEMA_TYPE *ptr, const Deleter& rep)
{
    setPtr(ptr);
    if (ptr) {
        d_members.d_deleter = rep;
    }
    else {
        d_members.d_deleter.clear();
    }
}

template <class BDEMA_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::init(BDEMA_TYPE  *ptr,
                                        void        *object,
                                        void        *factory,
                                        DeleterFunc  deleter)
{
    BSLS_ASSERT_SAFE(0 != factory || 0 == ptr);
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    setPtr(ptr);
    if (ptr) {
        d_members.d_deleter.set(object, factory, deleter);
    }
    else {
        d_members.d_deleter.clear();
    }
}

template <class BDEMA_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::reset(BDEMA_TYPE *ptr, Deleter *rep)
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


template <class BDEMA_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::reset(BDEMA_TYPE  *ptr,
                                         void        *object,
                                         void        *factory,
                                         DeleterFunc  deleter)
{
    BSLS_ASSERT_SAFE(0 != factory || 0 == ptr);
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

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

template <class BDEMA_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::rawClear()
{
    d_members.rawClear();
}

template <class BDEMA_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::load(nullptr_t)
{
    reset(0,0);
}

template <class BDEMA_TYPE>
template <class BDEMA_TARGET_TYPE>
inline
#if !defined (NO_SFINAE)
void bdema_ManagedPtr<BDEMA_TYPE>::load(BDEMA_TARGET_TYPE *ptr,
          typename bslma_EnableIf<bslmf_IsConvertible<BDEMA_TARGET_TYPE *,
                                                      BDEMA_TYPE *>::VALUE,
                                  unspecified>::type *)
{
#else
void bdema_ManagedPtr<BDEMA_TYPE>::load(BDEMA_TARGET_TYPE *ptr)
{
    BSLMF_ASSERT((bslmf_IsConvertible<BDEMA_TARGET_TYPE *, BDEMA_TYPE *>::VALUE));
#endif
    reset(ptr, ptr, bslma_Default::allocator(),
          reinterpret_cast<DeleterFunc>(
             &bdema_ManagedPtrFactoryDeleter<BDEMA_TARGET_TYPE,bslma_Allocator>
                                                                   ::deleter));
}

template <class BDEMA_TYPE>
template <class FACTORY>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::load(BDEMA_TYPE *ptr, FACTORY *factory)
{
    BSLS_ASSERT_SAFE(0 != factory || 0 == ptr);

    reset(ptr, ptr, factory, reinterpret_cast<DeleterFunc>(
               &bdema_ManagedPtrFactoryDeleter<BDEMA_TYPE, FACTORY>::deleter));
}

template <class BDEMA_TYPE>
template <class FACTORY>
inline
void
bdema_ManagedPtr<BDEMA_TYPE>::load(BDEMA_TYPE *ptr,
                                   FACTORY *factory,
                                   void (*deleter)(BDEMA_TYPE *, FACTORY *))
{
    BSLS_ASSERT_SAFE(0 != factory || 0 == ptr);
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    reset(ptr, ptr, factory, reinterpret_cast<DeleterFunc>(deleter));
}

template <class BDEMA_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::load(BDEMA_TYPE *ptr,
                                  void *factory,
                                  void(*deleter)(BDEMA_TYPE*, void*))
{
    BSLS_ASSERT_SAFE(0 != factory || 0 == ptr);
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    reset(ptr, ptr, factory, reinterpret_cast<DeleterFunc>(deleter));
}

template <class BDEMA_TYPE>
template <class OTHER>
void
bdema_ManagedPtr<BDEMA_TYPE>::loadAlias(bdema_ManagedPtr<OTHER>& alias,
                                        BDEMA_TYPE *ptr)
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

template <class BDEMA_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::clear()
{
    reset(0,0);
}

template<typename BDEMA_TYPE>
bsl::pair<BDEMA_TYPE*,bdema_ManagedPtrDeleter> 
bdema_ManagedPtr<BDEMA_TYPE>::release()
{
    BDEMA_TYPE *p = ptr();
    bsl::pair<BDEMA_TYPE*,bdema_ManagedPtrDeleter> ret(p, d_members.d_deleter);
    rawClear();
    return ret;
}

template <class BDEMA_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::swap(bdema_ManagedPtr<BDEMA_TYPE>& rhs)
{
    void *tmp_p             = d_members.d_obj_p;
    d_members.d_obj_p       = rhs.d_members.d_obj_p;
    rhs.d_members.d_obj_p   = tmp_p;

    Deleter tmp             = d_members.d_deleter;
    d_members.d_deleter     = rhs.d_members.d_deleter;
    rhs.d_members.d_deleter = tmp;
}

template <class BDEMA_TYPE>
bdema_ManagedPtr<BDEMA_TYPE>&
    bdema_ManagedPtr<BDEMA_TYPE>::operator=(bdema_ManagedPtr<BDEMA_TYPE>& rhs)
{
    if (this == &rhs) return *this;

    reset(rhs.ptr(), &rhs.d_members.d_deleter);
    rhs.rawClear();
    return *this;
}

template <class BDEMA_TYPE>
template <class OTHER>
bdema_ManagedPtr<BDEMA_TYPE>&
bdema_ManagedPtr<BDEMA_TYPE>::operator=(bdema_ManagedPtr<OTHER>& rhs)
{
    if ((bdema_ManagedPtr_Members *)this == (bdema_ManagedPtr_Members *)&rhs) {
        return *this;
    }

    reset(rhs.ptr(), &rhs.d_members.d_deleter);
    rhs.rawClear();
    return *this;
}

#if !defined(BDEMA_MANAGEDPTR_TEST_WITHOUT_REF_TRICK)
template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>&
bdema_ManagedPtr<BDEMA_TYPE>::operator=(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref)
{
    if (&d_members == ref.base()) {
        return *this;
    }

    reset(ref.ptr(), &ref.base()->d_deleter);
    ref->rawClear();
    return *this;
}

template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::operator bdema_ManagedPtr_Ref<BDEMA_TYPE>()
{
    return bdema_ManagedPtr_Ref<BDEMA_TYPE>(&d_members, this->ptr());
}

template <class BDEMA_TYPE>
template <class OTHER>
inline
bdema_ManagedPtr<BDEMA_TYPE>::operator bdema_ManagedPtr_Ref<OTHER>()
{
    return bdema_ManagedPtr_Ref<OTHER>(&d_members, this->ptr());
}
#endif

// ACCESSORS
template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::operator bdema_ManagedPtr_UnspecifiedBool() const
{
    return d_members.d_obj_p
         ? &bdema_ManagedPtr_UnspecifiedBoolHelper<bdema_ManagedPtr<BDEMA_TYPE>
                                                                    >::d_member
         : 0;
}

template <class BDEMA_TYPE>
inline
typename bdema_ManagedPtr_ReferenceType<BDEMA_TYPE>::Reference
bdema_ManagedPtr<BDEMA_TYPE>::operator*() const
{
    BSLS_ASSERT_SAFE(d_members.d_obj_p);
    return *static_cast<BDEMA_TYPE*>(d_members.d_obj_p);
}

template <class BDEMA_TYPE>
inline
BDEMA_TYPE *bdema_ManagedPtr<BDEMA_TYPE>::operator->() const
{
    return static_cast<BDEMA_TYPE*>(d_members.d_obj_p);
}

template <class BDEMA_TYPE>
inline
const bdema_ManagedPtrDeleter& bdema_ManagedPtr<BDEMA_TYPE>::deleter() const
{
    return d_members.d_deleter;
}

template <class BDEMA_TYPE>
inline
BDEMA_TYPE *bdema_ManagedPtr<BDEMA_TYPE>::ptr() const
{
    return static_cast<BDEMA_TYPE*>(d_members.d_obj_p);
}

                      // ----------------------------------
                      // private class bdema_ManagedPtr_Ref
                      // ----------------------------------

#if !defined(BDEMA_MANAGEDPTR_TEST_WITHOUT_REF_TRICK)
// CREATOR
template <class BDEMA_TYPE>
inline
bdema_ManagedPtr_Ref<BDEMA_TYPE>::bdema_ManagedPtr_Ref(
                                                bdema_ManagedPtr_Members *base,
                                                BDEMA_TYPE               *ptr)
: d_base_p(base), d_obj_p(ptr)
{
    BSLS_ASSERT_SAFE(0 != base);
}

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
template <class BDEMA_TYPE>
inline
bdema_ManagedPtr_Ref<BDEMA_TYPE>::~bdema_ManagedPtr_Ref()
{
    BSLS_ASSERT_SAFE(0 != d_base_p);
}
#endif

// MANIPULATORS
template <class BDEMA_TYPE>
inline
BDEMA_TYPE *bdema_ManagedPtr_Ref<BDEMA_TYPE>::ptr() const
{
    return d_obj_p;
}

template <class BDEMA_TYPE>
inline
bdema_ManagedPtr_Members *bdema_ManagedPtr_Ref<BDEMA_TYPE>::operator->() const
{
    return d_base_p;
}

template <class BDEMA_TYPE>
inline
bdema_ManagedPtr_Members *bdema_ManagedPtr_Ref<BDEMA_TYPE>::base() const
{
    return d_base_p;
}
#endif

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
bdema_ManagedPtrDeleter::Deleter
bdema_ManagedPtrDeleter::deleter() const
{
    return d_deleter;
}

                    // -------------------------------------
                    // struct bdema_ManagedPtrFactoryDeleter
                    // -------------------------------------

template <class BDEMA_TYPE, typename FACTORY>
inline
void
bdema_ManagedPtrFactoryDeleter<BDEMA_TYPE,FACTORY>::deleter(BDEMA_TYPE *object,
                                                            FACTORY    *factory)
{
    BSLS_ASSERT_SAFE(0 != object);
    BSLS_ASSERT_SAFE(0 != factory);
    factory->deleteObject(object);
}

                    // ---------------------------------
                    // struct bdema_ManagedPtrNilDeleter
                    // ---------------------------------

template <class BDEMA_TYPE>
inline
void
bdema_ManagedPtrNilDeleter<BDEMA_TYPE>::deleter(BDEMA_TYPE *object, void *)
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
