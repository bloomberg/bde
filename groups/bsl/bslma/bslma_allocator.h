// bslma_allocator.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLMA_ALLOCATOR
#define INCLUDED_BSLMA_ALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a pure abstract interface for memory-allocation mechanisms.
//
//@CLASSES:
//  bslma_Allocator: protocol class for memory allocation and deallocation
//
//@SEE_ALSO: bslma_newdeleteallocator, bslma_testallocator
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component provides the base-level protocol (pure abstract
// interface) class, 'bslma_allocator', that serves as a ubiquitous vocabulary
// type for various memory allocation mechanisms.  The functional capabilities
// documented by this protocol are similar to those afforded by global
// operators 'new' and 'delete': sufficiently (but not necessarily maximally)
// aligned memory is guaranteed for any object of a given size.  Clients of
// this abstract base class will typically accept a supplied allocator (often
// at construction) and use its 'allocate' and 'deallocate' methods instead of
// 'new' and 'delete' directly.
//
// The use of (abstract) allocators provide at least three distinct advantages
// over direct (hard-coded) calls to global 'new' and 'delete' (see
// 'bslma_newdeleteallocator'):
//..
//  (1) The particular choice of allocator can be selected to improve
//      performance on a per-object basis.  Without allocators, the best
//      we can do in C++ is to overload the class-specific new and delete.
//      Class-specific allocators tend to hoard memory even when most
//      objects of the class have been deallocated, and often mask
//      memory leaks that would otherwise have been detected.
//      See Lakos-96, Section 10.3.4.2, pp 705-711.
//
//  (2) By providing extra capabilities (beyond 'new' and 'delete') in the
//      derived class (see 'bslma_managedallocator'), we can bypass the
//      individual destructors in a dynamically allocated type and remove
//      all memory for one or more associated object almost instantly.
//
//  (3) The 'bslma_Allocator' protocol, like any other protocol, isolates
//      clients from direct coupling with platform level facilities that
//      are not fully under their control.  By installing a test allocator
//      (see 'bslma_testallocator'), we are able to orchestrate the white-box
//      testing of internal calls to global operators 'new' and 'delete' in a
//      platform-neutral manner.
//..
///Thread Safety
///-------------
// Unless otherwise documented, a single allocator object is not safe for
// concurrent access by multiple threads.  Classes derived from
// 'bslma_Allocator' that are specifically designed for concurrent access must
// be documented as such.  Unless specifically documented otherwise, separate
// objects of classes derived from 'bslma_Allocator' may safely be used in
// separate threads.
//
///Allocators Versus Pools
///-----------------------
// An allocator and a pool are quite different.  For starters,
// 'bslma_Allocator' is an abstract class used to obtain "raw" memory of
// arbitrary size.  A pool is a concrete data structure used to organize and
// supply memory according to specific needs (e.g., a consistent size).
// Concrete allocators may use pools in their implementations, and pools will
// aways take a base 'bslma_Allocator' protocol in their interface.  You can
// think of an allocator as a stream of memory that flows into a pool of
// memory.  Memory is allocated from the pool until it is dry; only then does
// new memory flow into the pool from the allocator.
//
///Overloaded Global Operators 'new' and 'delete'
///----------------------------------------------
// This component overloads the global operator 'new' to allow convenient
// syntax for the construction of objects using the 'bslma_Allocator' protocol.
// The overloaded 'new' operator defined in this component has a second
// parameter, 'bslma_Allocator&', that identifies the concrete (derived)
// allocator that will be used to supply memory.
//
// Consider the following use of standard placement syntax (supplied by
// '#include <new>') along with a 'bslma_Allocator', used to allocate an
// arbitrary 'TYPE'.
//..
//  void someFunction(bslma_Allocator *basicAllocator)
//  {
//      TYPE *obj = new (basicAllocator->allocate(sizeof(TYPE))) TYPE(...);
//
//      // ...
//..
// This style of usage is inconvenient and error prone; it is also *not*
// exception safe: If the constructor of 'TYPE' throws an exception, the
// 'basicAllocator->deallocate' method is never called.
//
// Providing an overloaded global operator 'new', taking a reference to a
// modifiable 'bslma_Allocator' as an explicit argument allows for cleaner
// usage and guarantees that the 'basicAllocator->deallocate' method is
// called in case of an exception:
//..
//  void someFunction(bslma_Allocator *basicAllocator)
//  {
//      TYPE *obj = new (*basicAllocator) TYPE(...);
//
//      // ...
//..
// Finally, the analogous version of operator 'delete' should not be called
// directly: The overloaded operator 'delete' supplied in this component is
// solely for the compiler to invoke in the event an exception is thrown
// during a failed construction.  Instead, the 'bslma_Allocator' protocol
// provides 'deleteObject' (a template member function parameterized by the
// type of the object being deleted), which is implemented *conceptually*
// as follows:
//..
//  template <class TYPE>
//  void bslma_Allocator::deleteObject(TYPE *address)
//  {
//      address->~TYPE();
//      this->deallocate(address);
//  }
//..
// Note that there is also a 'deleteObjectRaw' which is more efficient when
// it is known that the 'address' does *not* refer to a secondary base class
// of the object being deleted.
//
///Usage
///-----
// The 'bslma_Allocator' protocol provided in this component defines a
// bilateral contract between suppliers and consumers of raw memory.
// The following subsections illustrate (1) use, and (2) implementation of
// the abstract 'bslma_Allocator' base class:
//
///Usage 1: Container Objects
///- - - - - - - - - - - - -
// Allocators are often supplied to objects requiring dynamically-allocated
// memory at construction.  For example, consider the following
// 'my_DoubleStack' class, parameterized by a 'bslma_Allocator':
//..
//  // my_doublestack.h
//  // ...
//
//  class bslma_Allocator; // forward declaration of allocator type
//
//  class my_DoubleStack {
//      // DATA
//      double          *d_stack_p;     // dynamically allocated array (d_size
//                                      // elements)
//
//      int              d_size;        // physical capacity of this stack (in
//                                      // elements)
//
//      int              d_length;      // logical index of next available
//                                      // stack element
//
//      bslma_Allocator *d_allocator_p; // holds (but does not own) object
//
//      // FRIENDS
//      friend class my_DoubleStackIter;
//
//    private:
//      // PRIVATE MANIPULATORS
//      void increaseSize(); // Increase the capacity by at least one element.
//
//    public:
//      // CREATORS
//      my_DoubleStack(bslma_Allocator *basicAllocator = 0);
//      my_DoubleStack(const my_DoubleStack&  other,
//                     bslma_Allocator       *basicAllocator = 0);
//      ~my_DoubleStack();
//
//      // MANIPULATORS
//      my_DoubleStack& operator=(const my_DoubleStack& rhs);
//      void push(double value);
//      void pop();
//
//      // ACCESSORS
//      const double& top() const;
//      bool isEmpty() const;
//  };
//
//  // ...
//
//  inline
//  void my_DoubleStack::push(double value)
//  {
//      if (d_length >= d_size) {
//          increaseSize();
//      }
//      d_stack_p[d_length++] = item;
//  }
//
//  // ...
//..
// The stack interface takes an optional 'basicAllocator' supplied only at
// construction.  (We avoid use of the name 'allocator' so as not to conflict
// with the STL use of the word, which differs slightly.)  If non-zero, the
// stack holds a pointer to this allocator, but does not own it.  If no
// allocator is supplied, the implementation itself must either conditionally
// invoke global 'new' and 'delete' explicitly whenever memory dynamic memory
// must be managed (BAD IDEA) or (GOOD IDEA) install a default allocator that
// adapts use of these global operators to the 'bslma_allocator' interface (see
// 'bslma_newdeleteallocator').
//..
//  // my_doublestack.cpp
//  #include <my_doublestack.h>
//  #include <bslma_allocator.h>
//  #include <bslma_newdeleteallocator.h>  // adapter for 'new' and 'delete'
//  // ...
//
//  enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };
//
//  // ...
//
//  // CREATORS
//  my_DoubleStack::my_DoubleStack(bslma_Allocator *basicAllocator)
//  : d_size(INITIAL_SIZE)
//  , d_length(0)
//  , d_allocator_p(bslma_NewDeleteAllocator::allocator(basicAllocator))
//      // The above initialization expression is equivalent to 'basicAllocator
//      // ? basicAllocator : &bslma_NewDeleteAllocator::singleton()'.
//  {
//      assert(d_allocator_p);
//      d_stack_p = (double *)
//                  d_allocator_p->allocate(d_size * sizeof *d_stack_p);
//  }
//
//  my_DoubleStack::~my_DoubleStack()
//  {
//      // CLASS INVARIANTS
//      assert(d_allocator_p);
//      assert(d_stack_p);
//      assert(0 <= d_length);
//      assert(0 <= d_size);
//      assert(d_length <= d_size);
//
//      d_allocator_p->deallocate(d_stack_p);
//  }
//..
// Even in this simplified implementation, all use of the allocator protocol is
// relegated to the '.cpp' file.  Subsequent use of the allocator is
// demonstrated by the following file-scope static reallocation function:
//..
//  static inline
//  void reallocate(double          **array,
//                  int               newSize,
//                  int               length,
//                  bslma_Allocator  *basicAllocator)
//      // Reallocate memory in the specified 'array' to the specified
//      // 'newSize' using the specified 'basicAllocator'.  The specified
//      // 'length' number of leading elements are preserved.  Since the
//      // class invariant requires that the physical capacity of the
//      // container may grow but never shrink, the behavior is undefined
//      // unless 'length <= newSize'.
//  {
//      assert(array);
//      assert(1 <= newSize);
//      assert(0 <= length);
//      assert(basicAllocator);
//      assert(length <= newSize);  // enforce class invariant
//
//      double *tmp = *array;       // support exception neutrality
//      *array = (double *) basicAllocator->allocate(newSize * sizeof **array);
//
//      // COMMIT POINT
//
//      std::memcpy(*array, tmp, length * sizeof **array);
//      basicAllocator->deallocate(tmp);
//  }
//
//  // PRIVATE MANIPULATORS
//  void my_DoubleStack::increaseSize()
//  {
//      int proposedNewSize = d_size * GROW_FACTOR;  // reallocate can throw
//      assert(proposedNewSize > d_length);
//      reallocate(&d_stack_p, proposedNewSize, d_length, d_allocator_p);
//      d_size = proposedNewSize;                    // we're committed
//  }
//..
///Usage 2: Derived Concrete Allocators
///- - - - - - - - - - - - - - - - - -
// In order for the 'bslma_Allocator' interface to be useful, we must supply a
// concrete allocator that implements it.  In this example we demonstrate
// how to adapt 'operator new' and 'operator delete' to this protocol base
// class.
//..
//  // my_newdeleteallocator.h
//  // ...
//
//  class my_NewDeleteAllocator : public bslma_Allocator {
//      // This class is a sample concrete implementation of the
//      // 'bslma_Allocator' protocol that provides direct access to the
//      // system-supplied (native) global operators 'new' and 'delete'.
//
//      // NOT IMPLEMENTED
//      my_NewDeleteAllocator(const bslma_NewDeleteAllocator&);
//      my_NewDeleteAllocator& operator=(const bslma_NewDeleteAllocator&);
//
//    public:
//      // CREATORS
//      my_NewDeleteAllocator();
//          // Create an allocator that wraps the global (native) operators
//          // 'new' and 'delete' to supply memory.  Note that all objects of
//          // this class share the same underlying resource.
//
//      virtual ~my_NewDeleteAllocator();
//          // Destroy this allocator object.  Note that destroying this
//          // allocator has no effect on any outstanding allocated memory.
//
//      // MANIPULATORS
//      virtual void *allocate(size_type size);
//          // Return a newly allocated block of memory of (at least) the
//          // specified positive 'size' (in bytes).  If 'size' is 0, a null
//          // pointer is returned with no other effect.  If this allocator
//          // cannot return the requested number of bytes, then it will throw
//          // a 'std::bad_alloc' exception in an exception-enabled build, or
//          // else will abort the program in a non-exception build.  The
//          // behavior is undefined unless '0 <= size'.  Note that the
//          // alignment of the address returned is the maximum alignment for
//          // any type defined on this platform.  Also note that global
//          // 'operator new' is *not* called when 'size' is 0 (in order to
//          // avoid having to acquire a lock, and potential contention in
//          // multi-threaded programs).
//
//      virtual void deallocate(void *address);
//          // Return the memory block at the specified 'address' back to this
//          // allocator.  If 'address' is 0, this function has no effect.  The
//          // behavior is undefined unless 'address' was allocated using this
//          // allocator object and has not already been deallocated.  Note
//          // that global 'operator delete' is *not* called when 'address' is
//          // 0 (in order to avoid having to acquire a lock, and potential
//          // contention in multi-treaded programs).
//  };
//
//  // CREATORS
//  inline
//  my_NewDeleteAllocator::my_NewDeleteAllocator()
//  {
//  }
//  // ...
//..
// The virtual methods of 'my_NewDeleteAllocator' are defined in the component
// '.cpp' file as they would not be inlined when invoked from the base class,
// which would be the typical usage in this case:
//..
//  // my_newdeleteallocator.cpp
//  #include <my_newdeleteallocator.h>
//
//  // CREATORS
//  my_NewDeleteAllocator::~my_NewDeleteAllocator()
//  {
//  }
//
//  // MANIPULATORS
//  void *my_NewDeleteAllocator::allocate(size_type size)
//  {
//      BSLS_ASSERT_SAFE(0 <= size);
//
//      return 0 == size ? 0 : ::operator new(size);
//  }
//
//  void my_NewDeleteAllocator::deallocate(void *address)
//  {
//      // While the C++ standard guarantees that calling delete(0) is safe
//      // (3.7.3.2 paragraph 3), some libc implementations take out a lock to
//      // deal with the free(0) case, so this check can improve efficiency of
//      // threaded programs.
//
//      if (address) {
//          ::operator delete(address);
//      }
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_DELETERHELPER
#include <bslma_deleterhelper.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>       // for 'std::size_t', 'std::ptrdiff_t'
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {

                        // =====================
                        // class bslma_Allocator
                        // =====================

class bslma_Allocator {
    // This protocol class provides a pure abstract interface and contract for
    // clients and suppliers of raw memory.  If the requested memory cannot
    // be returned; the contract requires that an 'std::bad_alloc' exception
    // be thrown.  Note that memory is guaranteed to be sufficiently aligned
    // for any object of the requested size on the current platform, which
    // may be less than the maximal alignment guarantee afforded by global
    // 'operator new'.

  public:
    // PUBLIC TYPES  // TBD
    typedef bsls_Types::size_type size_type;  // TBD
        // Alias for an unsigned integer type capable of representing the
        // number of bytes in this platform's virtual address space.

    // CLASS METHODS  // TBD
    static void throwBadAlloc();  // TBD
        // Throw 'std::bad_alloc' if exceptions are enabled or abort the
        // program otherwise.  Derived classes and helper functions will
        // typically call this function when they are unable to satisfy an
        // allocation request.  This function never returns.

    // CREATORS
    virtual ~bslma_Allocator();
        // Destroy this allocator.  Note that the behavior of destroying an
        // allocator while memory is allocated from it is not specified.
        // (Unless you *know* that it is valid to do so, don't!)

    // MANIPULATORS
    virtual void *allocate(size_type size) = 0;
        // Return a newly allocated block of memory of (at least) the specified
        // positive 'size' (in bytes).  If 'size' is 0, a null pointer is
        // returned with no other effect.  If this allocator cannot return the
        // requested number of bytes, then it will throw a 'std::bad_alloc'
        // exception in an exception-enabled build, or else will abort the
        // program in a non-exception build.  The behavior is undefined unless
        // '0 <= size'.  Note that the alignment of the address returned
        // conforms to the platform requirement for any object of the
        // specified 'size'.

    virtual void deallocate(void *address) = 0;
        // Return the memory block at the specified 'address' back to this
        // allocator.  If 'address' is 0, this function has no effect.  The
        // behavior is undefined unless 'address' was allocated using this
        // allocator object and has not already been deallocated.

    template <class TYPE>
    void deleteObject(const TYPE *object);
        // Destroy the specified 'object' based on its dynamic type and then
        // use this allocator to deallocate its memory footprint.  Do nothing
        // if 'object' is a null pointer.  The behavior is undefined unless
        // 'object', when cast appropriately to 'void *', was allocated using
        // this allocator and has not already been deallocated.  Note that
        // 'dynamic_cast<void *>(object)' is applied if 'TYPE' is polymorphic,
        // and 'static_cast<void *>(object)' is applied otherwise.

    template <class TYPE>
    void deleteObjectRaw(const TYPE *object);
        // Destroy the specified 'object' and then use this allocator to
        // deallocate its memory footprint.  Do nothing if 'object' is a null
        // pointer.  The behavior is undefined unless 'object' was allocated
        // using this allocator, is !not! a secondary base class pointer --
        // i.e., the address is (numerically) the same as when it was
        // originally dispensed by this allocator, and has not already been
        // deallocated.
};

// ============================================================================
//                      INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------
                        // class bslma_Allocator
                        // ---------------------

// MANIPULATORS
template <class TYPE>
inline
void bslma_Allocator::deleteObject(const TYPE *object)
{
    bslma_DeleterHelper::deleteObject(object, this);
}

template <class TYPE>
inline
void bslma_Allocator::deleteObjectRaw(const TYPE *object)
{
    bslma_DeleterHelper::deleteObjectRaw(object, this);
}

}  // close namespace BloombergLP

// FREE OPERATORS

// Note that the operators 'new' and 'delete' are declared outside the
// 'BloombergLP' namespace so that they do not hide the standard placement
// 'new' and 'delete' operators (i.e.,
// 'void *operator new(std::size_t, void *)' and
// 'void operator delete(void *)').
//
// Note also that only the scalar versions of operators 'new' and 'delete' are
// provided, because overloading 'new' (and 'delete') with their array versions
// would cause dangerous ambiguity.  Consider what would have happened had we
// overloaded the array version of operator 'new':
//..
//  void *operator new[](std::size_t size,
//                       BloombergLP::bslma_Allocator& basicAllocator)
//..
// The user of the allocator class may expect to be able to use array
// 'operator new' as follows:
//..
//  new (*basicAllocator) my_Type[...];
//..
// The problem is that this expression returns an array that cannot be safely
// deallocated.  On the one hand, there is no syntax in C++ to invoke an
// overloaded 'operator delete' that, other than deallocating memory, would
// invoke the destructor.  On the other hand, the pointer returned by
// 'operator new' cannot be passed to the 'deallocate' method directly because
// the pointer is different from the one returned by the 'allocate' method.
// The compiler offsets the value of this pointer by a header, which is used
// to maintain the number of objects in the array (so that the non-overloaded
// 'operator delete' can destroy the right number of objects).

inline
void *operator new(std::size_t                   size,
                   BloombergLP::bslma_Allocator& basicAllocator);
    // Return the memory allocated from the specified 'basicAllocator' of at
    // least the specified 'size' bytes, or a null pointer if 'size' is 0.  The
    // behavior is undefined unless '0 <= size'.  Note that an object may
    // allocate additional memory internally, requiring the allocator to be
    // passed in as a constructor argument:
    //..
    //  my_Type *createMyType(bslma_Allocator *basicAllocator)
    //  {
    //      return new (*basicAllocator) my_Type(..., basicAllocator);
    //  }
    //..
    // Note also that the analogous version of operator 'delete' should *not*
    // be called directly.  Instead, this component provides a template member
    // function 'deleteObject' parameterized by 'TYPE' that effectively
    // performs the following operations:
    //..
    //  void deleteMyType(bslma_Allocator *basicAllocator, my_Type *address)
    //  {
    //      address->~my_Type();
    //      basicAllocator->deallocate(object);
    //  }
    //..
    // See also 'deleteObjectRaw' for better performance when 'address' is
    // known not be a secondary base type of the object being deleted.

inline
void operator delete(void                          *address,
                     BloombergLP::bslma_Allocator&  basicAllocator);
    // Use the specified 'basicAllocator' to deallocate the memory at the
    // specified 'address'.  The behavior is undefined unless 'address' was
    // allocated using 'basicAllocator' and has not already been deallocated.
    // This operator is supplied solely to allow the compiler to arrange for it
    // to be called in case of an exception.

// NOTE: The following two operators are declared but never defined to force a
// link-time error should anyone inadvertently use them.

void *operator new(std::size_t                   size,
                   BloombergLP::bslma_Allocator *basicAllocator);
    // Note that this operator is intentionally declared, but not defined.

void operator delete(void                         *address,
                     BloombergLP::bslma_Allocator *basicAllocator);
    // Note that this operator is intentionally declared, but not defined.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

inline
void *operator new(std::size_t                   size,
                   BloombergLP::bslma_Allocator& basicAllocator)
{
    return basicAllocator.allocate(size);
}

inline
void operator delete(void                          *address,
                     BloombergLP::bslma_Allocator&  basicAllocator)
{
    basicAllocator.deallocate(address);
}

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
