// bdlma_countingallocator.h                                          -*-C++-*-
#ifndef INCLUDED_BDLMA_COUNTINGALLOCATOR
#define INCLUDED_BDLMA_COUNTINGALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a memory allocator that counts allocated bytes.
//
//@CLASSES:
//  bdlma::CountingAllocator: concrete allocator that counts allocated bytes
//
//@SEE_ALSO: bslma_allocator, bslma_testallocator
//
//@DESCRIPTION: This component provides a special-purpose counting allocator,
// 'bdlma::CountingAllocator', that implements the 'bslma::Allocator' protocol
// and provides instrumentation to track: (1) the number of bytes currently in
// use ('numBytesInUse'), and (2) the cumulative number of bytes that have ever
// been allocated ('numBytesTotal').  The accumulated statistics are based
// solely on the number of bytes requested in calls to the 'allocate' method.
// A 'print' method is provided to output the current state of the allocator's
// byte counts to a specified 'bsl::ostream':
//..
//   ,------------------------.
//  ( bdlma::CountingAllocator )
//   `------------------------'
//                |           ctor/dtor
//                |           numBytesInUse
//                |           numBytesTotal
//                |           name
//                |           print
//                V
//       ,----------------.
//      ( bslma::Allocator )
//       `----------------'
//                            allocate
//                            deallocate
//..
// Like many other allocators, 'bdlma::CountingAllocator' relies on the
// currently installed default allocator (see 'bslma_default') at construction.
// Clients may, however, override this allocator by supplying (at construction)
// any other allocator implementing the 'bslma::Allocator' protocol provided
// that it is fully thread-safe.
//
// Note that a 'bdlma::CountingAllocator' necessarily incurs some overhead in
// order to provide its byte-counting functionality.  However, this overhead is
// *substantially* less than that incurred by the 'bslma::TestAllocator' (see
// 'bslma_testallocator'), which keeps track of the same two statistics that
// are maintained by a 'bdlma::CountingAllocator'.  Consequently, use of a
// 'bdlma::CountingAllocator' may be appropriate in cases where the overhead of
// 'bslma::TestAllocator' is too onerous.  In particular, a counting allocator
// may be suitable even for production use in certain situations, whereas the
// test allocator is not intended for production use under any circumstance.
//
///Byte Counts
///-----------
// The two byte counts maintained by 'bdlma::CountingAllocator' are initialized
// to 0 at construction and increased with each call to 'allocate' by 'size',
// i.e., by the actual number of bytes requested.  Each call to 'deallocate'
// decreases the 'numBytesInUse' count by the same amount by which the byte
// count was increased in the original 'allocate' call.  The number of bytes
// currently in use is returned by 'numBytesInUse' and the total number of
// bytes ever allocated is returned by 'numBytesTotal'.
//
///Thread Safety
///-------------
// The 'bdlma::CountingAllocator' class is fully thread-safe (see
// 'bsldoc_glossary') provided that the underlying allocator (established at
// construction) is fully thread-safe.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Tracking a Container's Dynamic Memory Use
///----------------------------------------------------
// In this example, we demonstrate how a counting allocator may be used to
// track the amount of dynamic memory used by a container.  The container used
// for illustration is 'DoubleStack', a stack of out-of-place 'double' values.
//
// First, we show the interface of the 'DoubleStack' class:
//..
//  // doublestack.h
//
//  class DoubleStack {
//      // This class implements a stack of out-of-place 'double' values.
//
//      // DATA
//      double           **d_stack_p;      // dynamically allocated array of
//                                         // 'd_capacity' elements
//
//      int                d_capacity;     // physical capacity of the stack
//                                         // (in elements)
//
//      int                d_length;       // logical index of next available
//                                         // stack element
//
//      bslma::Allocator  *d_allocator_p;  // memory allocator (held, not
//                                         // owned)
//
//      // NOT IMPLEMENTED
//      DoubleStack(const DoubleStack&);
//      DoubleStack& operator=(const DoubleStack&);
//
//    private:
//      // PRIVATE MANIPULATORS
//      void increaseCapacity();
//          // Increase the capacity of this stack by at least one element.
//
//    public:
//      // CREATORS
//      explicit
//      DoubleStack(bslma::Allocator *basicAllocator = 0);
//          // Create a stack for 'double' values having an initial capacity to
//          // hold one element.  Optionally specify a 'basicAllocator' used to
//          // supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      ~DoubleStack();
//          // Delete this object.
//
//      // MANIPULATORS
//      void push(double value);
//          // Add the specified 'value' to the top of this stack.
//
//      void pop();
//          // Remove the element at the top of this stack.  The behavior is
//          // undefined unless this stack is non-empty.
//
//      // ACCESSORS
//      // ...
//  };
//..
// Next, we show the (elided) implementation of 'DoubleStack'.
//
// The default constructor creates a stack having the capacity for one element
// (the implementation of the destructor is not shown):
//..
//  // doublestack.cpp
//  // ...
//
//  // TYPES
//  enum { k_INITIAL_CAPACITY = 1, k_GROWTH_FACTOR = 2 };
//
//  // CREATORS
//  DoubleStack::DoubleStack(bslma::Allocator *basicAllocator)
//  : d_stack_p(0)
//  , d_capacity(k_INITIAL_CAPACITY)
//  , d_length(0)
//  , d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//      d_stack_p = (double **)
//                     d_allocator_p->allocate(d_capacity * sizeof *d_stack_p);
//  }
//..
// The 'push' method first ensures that the array has sufficient capacity to
// accommodate an additional value, then allocates a block in which to store
// that value:
//..
//  // MANIPULATORS
//  void DoubleStack::push(double value)
//  {
//      if (d_length >= d_capacity) {
//          increaseCapacity();
//      }
//      double *stackValue = (double *)d_allocator_p->allocate(sizeof(double));
//      *stackValue = value;
//      d_stack_p[d_length] = stackValue;
//      ++d_length;
//  }
//..
// The 'pop' method asserts that the stack is not empty before deallocating the
// block used to store the element at the top of the stack:
//..
//  void DoubleStack::pop()
//  {
//      BSLS_ASSERT(0 < d_length);
//
//      d_allocator_p->deallocate(d_stack_p[d_length - 1]);
//      --d_length;
//  }
//..
// The 'push' method (above) made use of the private 'increaseCapacity' method,
// which, in turn, makes use of the 'reallocate' helper function ('static' to
// the '.cpp' file).  Note that 'increaseCapacity' (below) increases the
// capacity of the 'double *' array by a factor of 2 each time that it is
// called:
//..
//  // HELPER FUNCTIONS
//  static
//  void reallocate(double           ***array,
//                  int                 newCapacity,
//                  int                 length,
//                  bslma::Allocator   *allocator)
//      // Reallocate memory in the specified 'array' to accommodate the
//      // specified 'newCapacity' elements using the specified 'allocator'.
//      // The specified 'length' number of leading elements are preserved.
//      // The behavior is undefined unless 'newCapacity > length'.
//  {
//      BSLS_ASSERT(newCapacity > length);
//
//      double **tmp = *array;
//      *array = (double **)allocator->allocate(newCapacity * sizeof **array);
//      bsl::memcpy(*array, tmp, length * sizeof **array);  // commit
//      allocator->deallocate(tmp);
//  }
//
//  // PRIVATE MANIPULATORS
//  void DoubleStack::increaseCapacity()
//  {
//      const int newCapacity = d_capacity * k_GROWTH_FACTOR;
//                                                      // reallocate can throw
//      reallocate(&d_stack_p, newCapacity, d_length, d_allocator_p);
//      d_capacity = newCapacity;                       // commit
//  }
//..
// Now, we are ready to employ a 'CountingAllocator' to illustrate the dynamic
// memory use of 'DoubleStack'.  We first define two constants that facilitate
// portability of this example across 32- and 64-bit platforms:
//..
//  const int DBLSZ = sizeof(double);
//  const int PTRSZ = sizeof(double *);
//..
// First, we define a 'CountingAllocator', 'ca'.  At construction, a counting
// allocator can be configured with an optional name and an optional allocator.
// In this case, we give 'ca' a name to distinguish it from other counting
// allocators, but settle for using the default allocator:
//..
//  bdlma::CountingAllocator ca("'DoubleStack' Allocator");
//..
// Next, we create a 'DoubleStack', supplying it with 'ca', and assert the
// expected memory use incurred by the default constructor:
//..
//  DoubleStack stack(&ca);
//                         assert(1 * PTRSZ             == ca.numBytesInUse());
//                         assert(1 * PTRSZ             == ca.numBytesTotal());
//..
// Next, we push an element onto the stack.  The first push incurs an
// additional allocation to store (out-of-place) the value being inserted:
//..
//  stack.push(1.54);      assert(1 * PTRSZ + 1 * DBLSZ == ca.numBytesInUse());
//                         assert(1 * PTRSZ + 1 * DBLSZ == ca.numBytesTotal());
//..
// Next, we push a second element onto the stack.  In this case, two
// allocations result, one due to the resizing of the internal array and one
// required to store the new value out-of-place:
//..
//  stack.push(0.99);      assert(2 * PTRSZ + 2 * DBLSZ == ca.numBytesInUse());
//                         assert(3 * PTRSZ + 2 * DBLSZ == ca.numBytesTotal());
//..
// Next, we pop the top-most element from the stack.  The number of bytes in
// use decreases by the amount used to store the popped element out-of-place:
//..
//  stack.pop();           assert(2 * PTRSZ + 1 * DBLSZ == ca.numBytesInUse());
//                         assert(3 * PTRSZ + 2 * DBLSZ == ca.numBytesTotal());
//..
// Finally, we print the state of 'ca' to standard output:
//..
//  ca.print(bsl::cout);
//..
// which displays the following on a 32-bit platform:
//..
//  ----------------------------------------
//                          Counting Allocator State
//  ----------------------------------------
//  Allocator name: 'DoubleStack' Allocator
//  Bytes in use:   16
//  Bytes in total: 28
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace bdlma {

                         // =======================
                         // class CountingAllocator
                         // =======================

class CountingAllocator : public bslma::Allocator {
    // This class defines a concrete "counting" allocator mechanism that
    // implements the 'bslma::Allocator' protocol, and provides instrumentation
    // to track: (1) the number of bytes currently in use, and (2) the
    // cumulative number of bytes that have ever been allocated.  The
    // accumulated statistics are based solely on the number of bytes requested
    // (see 'allocate').
    //
    // Note that, like many other allocators, this allocator relies on the
    // currently installed default allocator (see 'bslma_default').  Clients
    // may, however, override this allocator by supplying (at construction) any
    // other allocator implementing the 'bslma::Allocator' protocol provided
    // that it is fully thread-safe.

    // DATA
    const char        *d_name_p;         // optionally specified name of this
                                         // allocator object (or 0)

    bsls::AtomicInt64  d_numBytesInUse;  // number of bytes currently allocated
                                         // from this object

    bsls::AtomicInt64  d_numBytesTotal;  // cumulative number of bytes ever
                                         // allocated from this object

    bslma::Allocator  *d_allocator_p;    // memory allocator (held, not owned)

  private:
    // NOT IMPLEMENTED
    CountingAllocator(const CountingAllocator&);
    CountingAllocator& operator=(const CountingAllocator&);

  public:
    // CREATORS
    explicit
    CountingAllocator(bslma::Allocator *basicAllocator = 0);
    explicit
    CountingAllocator(const char *name, bslma::Allocator *basicAllocator = 0);
        // Create a counting allocator.  Optionally specify a 'name'
        // (associated with this object) to be included in messages output by
        // the 'print' method, thereby distinguishing this counting allocator
        // from others that might be used in the same program.  If 'name' is 0
        // (or not specified), no distinguishing name is incorporated in
        // 'print' output.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    virtual ~CountingAllocator();
        // Destroy this allocator object.  Note that destroying this allocator
        // has no effect on any outstanding allocated memory.

    // MANIPULATORS
    virtual void *allocate(size_type size);
        // Return a newly-allocated block of memory of the specified 'size' (in
        // bytes).  If 'size' is 0, a null pointer is returned with no other
        // effect (e.g., on allocation statistics).  Otherwise, invoke the
        // 'allocate' method of the allocator supplied at construction, and
        // increment the number of currently (and cumulatively) allocated bytes
        // by 'size'.

    virtual void deallocate(void *address);
        // Return the memory block at the specified 'address' back to this
        // allocator.  If 'address' is 0, this function has no effect (e.g., on
        // allocation statistics).  Otherwise, decrease the number of currently
        // allocated bytes by the size originally requested for the block.  The
        // behavior is undefined unless 'address' was allocated using this
        // allocator object and has not already been deallocated.

    // ACCESSORS
    const char *name() const;
        // Return the name of this counting allocator, or 0 if no name was
        // specified at construction.

    bsls::Types::Int64 numBytesInUse() const;
        // Return the number of bytes currently allocated from this object.
        // Note that 'numBytesInUse() <= numBytesTotal()'.

    bsls::Types::Int64 numBytesTotal() const;
        // Return the cumulative number of bytes ever allocated from this
        // object.  Note that 'numBytesInUse() <= numBytesTotal()'.

    bsl::ostream& print(bsl::ostream& stream) const;
        // Write the accumulated state information held in this allocator to
        // the specified 'stream' in some reasonable (multi-line) format, and
        // return a reference to 'stream'.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                         // -----------------------
                         // class CountingAllocator
                         // -----------------------

// ACCESSORS
inline
const char *CountingAllocator::name() const
{
    return d_name_p;
}

inline
bsls::Types::Int64 CountingAllocator::numBytesInUse() const
{
    return d_numBytesInUse.loadRelaxed();
}

inline
bsls::Types::Int64 CountingAllocator::numBytesTotal() const
{
    return d_numBytesTotal.loadRelaxed();
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
