// bdema_allocator.h            -*-C++-*-
#ifndef INCLUDED_BDEMA_ALLOCATOR
#define INCLUDED_BDEMA_ALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for memory allocation and deallocation.
//
//@DEPRECATED: Use 'bslma_allocator' instead.
//
//@CLASSES:
//   bdema_Allocator: protocol class for memory allocation and deallocation
//
//@SEE_ALSO: bdema_NewDeleteAllocator, bdema_ManagedAllocator,
// bdema_TestAllocator
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component defines the base-level protocol for memory
// allocation; concrete types derived from this protocol supply memory.  The
// functional capabilities provided by this protocol are similar to those
// afforded by global operators 'new' and 'delete'.  Typical clients of this
// class will accept a supplied allocator (typically at construction) and used
// its 'allocate' and 'deallocate' methods instead of 'new' and 'delete'
// directly.
//
// The use of allocators provide at least three distinct advantages over direct
// (hard-coded) calls to global 'new' and 'delete' (see
// 'bdema_newdeleteallocator').
//..
//   (1) The particular choice of allocator can be selected to improve
//       performance on a per-object basis.  Without allocators, the best
//       we can do in C++ is to overload the class-specific new and delete.
//       Class-specific allocators tend to hoard memory even when most
//       instances of the class have been deallocated, and often mask
//       memory leaks that would otherwise have been detected.
//       See Lakos-96, Section 10.3.4.2, pp 705-711.
//
//   (2) By providing extra capabilities (beyond 'new' and 'delete') in the
//       derived class (see 'bdema_managedallocator'), we can bypass the
//       individual destructors in a dynamically allocated type and remove
//       all memory for one or more associated object almost instantly.
//
//   (3) The 'bdema_Allocator' protocol, like any other protocol, isolates
//       clients from direct coupling with platform level facilities that
//       are not fully under their control.  By installing a test allocator
//       (see 'bdema_testallocator'), we are able to orchestrate the white-box
//       testing of internal calls to global operators 'new' and 'delete' in a
//       platform-neutral manner.
//..
///Thread Safety
///-------------
// Unless otherwise documented, a single allocator object is not safe for
// concurrent access by multiple threads.  Classes derived from
// 'bdema_Allocator' that are specifically designed for concurrent access
// must be documented as such.  Unless specifically documented otherwise,
// separate instances of classes derived from 'bdema_Allocator' may safely
// be used in separate threads.
//
///Allocators Versus Pools
///-----------------------
// An allocator and a pool are quite different.  For starters,
// 'bdema_Allocator' is an abstract class used to obtain "raw" memory of
// arbitrary size.  A pool is a concrete data structure used to organize and
// supply memory according to specific needs (e.g., a consistent size).
// Concrete allocators may use pools in their implementations, and pools will
// aways take a base 'bdema_Allocator' protocol in their interface.  You can
// think of an allocator as a stream of memory that flows into a pool of
// memory.  Memory is allocated from the pool until it is dry; only then does
// new memory flow into the pool from the allocator.
//
///Overloaded Global Operator 'new'
///--------------------------------
// This component overloads the global operator 'new' to allow convenient
// syntax for the construction of objects using the 'bdema_Allocator' protocol.
// The 'new' operator supplied in this component takes a 'bdema_Allocator'
// argument indicating the source of the memory.  Consider the following use of
// standard placement syntax (supplied in 'new.h') along with a
// 'bdema_Allocator' to allocate an arbitrary type.
//..
//    void f(bdema_Allocator *basicAllocator) {
//        T *t = new (basicAllocator->allocate(sizeof(T))) T(...);
//        // ...
//..
// This usage style is error-prone since sizeof(T) must be used explicitly and
// passed to
//..
//      void *::operator new(std::size_t size).
//..
// Also, this usage style is not exception safe.  If the constructor of T
// throws an exception, basicAllocator->deallocate is never called.
//
// Supplying an overloaded global operator 'new'
//..
//      ::operator new(std::size_t size, bdema_Allocator& basicAllocator)
//..
// allows the following cleaner usage, which does not require the size
// calculation and guarantees that basicAllocator->deallocate is called in case
// of an exception:
//..
//    void f(bdema_Allocator *basicAllocator) {
//        T *t = new(*basicAllocator) T(...);
//        // ...
//..
// Note also that the analogous version of operator 'delete' should not be
// called directly.  Instead, this component provides a template member
// function 'deleteObject' parameterized by 'TYPE' that performs the following:
//..
//        t->~TYPE();
//        this->deallocate(t);
//..
// The operator 'delete' is supplied solely to allow the compiler to arrange
// for it to be called in case of an exception.
//
///Usage 1: Container Objects
///--------------------------
// Allocators are often supplied to objects requiring dynamically-allocated
// memory at construction.  For example, consider the following
// 'my_DoubleStack' class, parameterized by a 'bdema_Allocator'.
//..
//  // my_doublestack.h
//  // ...
//
//  class bdema_Allocator; // forward declaration of allocator type
//
//  class my_DoubleStack {
//      double *d_stack_p; // dynamically allocated array (d_size elements)
//      int d_size;        // physical capacity of this stack (in elements)
//      int d_length;      // logical index of next available stack element
//      bdema_Allocator *d_allocator_p; // holds (but doesn't own) object
//
//      friend class my_DoubleStackIter;
//
//    private:
//      void increaseSize(); // Increase the capacity by at least one element.
//
//    public:
//      // CREATORS
//      my_DoubleStack(bdema_Allocator *basicAllocator = 0);
//      my_DoubleStack(const my_DoubleStack&  other,
//                     bdema_Allocator       *basicAllocator = 0);
//      ~my_DoubleStack();
//
//      // MANIPULATORS
//      my_DoubleStack& operator=(const my_DoubleStack& rhs);
//      void push(double value);
//      void pop();
//
//      // ACCESSORS
//      const double& top() const;
//      int isEmpty() const;
//  };
//
//  // ...
//
//  inline void my_DoubleStack::push(double value)
//  {
//      if (d_length >= d_size) {
//          increaseSize();
//      }
//      d_stack_p[d_length++] = item;
//  }
//
//  // ..
//..
// The stack interface takes an optional 'basicAllocator' supplied only at
// construction.  (We avoid use of the name 'allocator' so as not to conflict
// with the STL use of the word, which differs slightly.) If non-zero, the
// stack holds a pointer to this allocator, but does not own it.  If no
// allocator is supplied, the implementation itself must either conditionally
// invoke global 'new' and 'delete' explicitly whenever memory dynamic memory
// must be managed (BAD IDEA) or (GOOD IDEA) install a default allocator that
// adapts use of these global operators to the 'bdema_allocator' interface (see
// 'bdema_newdeleteallocator').
//..
//  // my_doublestack.cpp
//  // ...
//  #include <my_doublestack.cpp>
//  #include <bdema_allocator.h>
//  #include <bdema_newdeleteallocator.h>  // adapter for 'new' and 'delete'
//
//  enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };
//
//  // ...
//
//  my_DoubleStack::my_DoubleStack(bdema_Allocator *basicAllocator)
//  : d_size(INITIAL_SIZE)
//  , d_length(0)
//  , d_allocator_p(bdema_NewDeleteAllocator::allocator(basicAllocator))
//      // The above initialization expression is equivalent to 'basicAllocator
//      // ? basicAllocator : &bdema_NewDeleteAllocator::singleton()'.
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
//  inline static
//  void reallocate(double          **array,
//                  int               newSize,
//                  int               length,
//                  bdema_Allocator  *basicAllocator)
//      // Reallocate memory in the specified 'array' to the specified
//      // 'newSize' using the specified 'basicAllocator'.  The specified
//      // 'length' number of leading elements are preserved.  Since the
//      // class invariant requires that the physical capacity of the
//      // container may grow but never shrink, the behavior is undefined
//      // unless length <= newSize.
//{
//    assert(array);
//    assert(1 <= newSize);
//    assert(0 <= length);
//    assert(basicAllocator);
//    assert(length <= newSize);        // enforce class invariant
//
//    double *tmp = *array;             // support exception neutrality
//    *array = (double *) basicAllocator->allocate(newSize * sizeof **array);
//
//    // COMMIT POINT
//
//    memcpy(*array, tmp, length * sizeof **array);
//    basicAllocator->deallocate(tmp);
//}
//
// void my_DoubleStack::increaseSize()
// {
//     int proposedNewSize = d_size * GROW_FACTOR;      // reallocate can throw
//     assert(proposedNewSize > d_length);
//     reallocate(&d_stack_p, proposedNewSize, d_length, d_allocator_p);
//     d_size = proposedNewSize;                        // we're committed
// }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef bdema_Allocator
#define bdema_Allocator bslma_Allocator
    // Provide a protocol (or pure interface) for memory allocation and
    // deallocation.
#endif

namespace BloombergLP {

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
