// bdema_bufferallocator.h                                            -*-C++-*-
#ifndef INCLUDED_BDEMA_BUFFERALLOCATOR
#define INCLUDED_BDEMA_BUFFERALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Support efficient memory allocation from a user-supplied buffer.
//
//@DEPRECATED: Use 'bdema_bufferedsequentialallocator' instead.
//
//@CLASSES:
//   bdema_BufferAllocator: memory allocator from user-supplied buffer
//
//@AUTHOR: John Lakos (jlakos)
//
//@SEE_ALSO: bsls_alignment
//
//@DESCRIPTION: This component implements a concrete allocator derived from the
// 'bdema_Allocator' interface.  'bdema_BufferAllocator' sequentially allocates
// memory blocks from a fixed-size buffer that is supplied by the user at
// construction.  If an allocation request exceeds the remaining space in the
// buffer, the return value is the result of invoking an optional callback
// function that was supplied at construction, or zero if no callback was
// specified.  This component also provides static utility functions for
// allocating memory directly from a user-specified buffer:
//..
//                     ( bdema_BufferAllocator )
//                                 |         ctor
//                                 V
//                        ( bdema_Allocator )
//                                           dtor
//                                           allocate
//                                           deallocate
//..
///ALIGNMENT STRATEGY
///------------------
// The 'bdema_BufferAllocator' allocates memory using one of the two alignment
// strategies: 1) MAXIMUM ALIGNMENT or 2) NATURAL ALIGNMENT.
//..
//     MAXIMUM ALIGNMENT: This strategy always allocates memory aligned with
//     the most restrictive alignment on the host platform.  The value is
//     defined in 'bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT'.
//
//     NATURAL ALIGNMENT: This strategy allocates memory whose alignment
//     depends on the requested number of bytes.  An instance of a fundamental
//     type ('int', etc.) is *naturally* *aligned* when it's size evenly
//     divides its address.  An instance of an aggregate type has natural
//     alignment if the alignment of the most-restrictively aligned sub-object
//     evenly divides the address of the aggregate.  Natural alignment is
//     always at least as restrictive as the compiler's required alignment.
//     When only the size of an aggregate is known, and not its composition,
//     we compute the alignment by finding the largest integral power of 2 (up
//     to and including 'bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT') that divides
//     the requested (non-zero) number of bytes.  This computed alignment is
//     guaranteed to be at least as restrictive as any sub-object within the
//     aggregate.
//..
///Usage
///-----
// The 'bdema_BufferAllocator' class defined in this component is commonly used
// to allocate memory from a static buffer that does not require "deletion".
// The following snippet of code creates an array that obtains its memory from
// a fixed-sized buffer, but through the 'bdema_Allocator' protocol:
//..
//  // my_shortarray.h
//
//  // ...
//
//  class bdema_Allocator;
//
//  class my_ShortArray {
//      short           *d_array_p;      // dynamically-allocated array of
//                                       // 'short' integers
//
//      int              d_size;         // physical size of 'd_array_p'
//
//      int              d_length;       // logical length of 'd_array_p'
//
//      bdema_Allocator *d_allocator_p;  // memory allocator (held, not owned)
//
//    private:
//      // PRIVATE MANIPULATORS
//      void increaseSize();  // Increase the capacity by at least one element.
//
//    public:
//      // CREATORS
//      my_ShortArray(bdema_Allocator *basicAllocator);
//          // Create an empty array using the specified 'basicAllocator' to
//          // supply memory.
//      // ...
//
//      ~my_ShortArray();
//      void append(int value);
//      const short& operator[](int index) const { return d_array_p[index]; }
//      int length() const { return d_length; }
//  };
//..
// The implementation of 'my_ShortArray' is as follows:
//..
//  // my_shortarray.cpp
//  #include <my_shortarray.h>
//
//  enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };
//  // ...
//
//  my_ShortArray::my_ShortArray(bdema_Allocator *basicAllocator)
//  : d_size(INITIAL_SIZE)
//  , d_length(0)
//  , d_allocator_p(basicAllocator)
//  {
//      assert(d_allocator_p);
//      d_array_p =
//          (short *) d_allocator_p->allocate(d_size * sizeof *d_array_p);
//  }
//
//  my_ShortArray::~my_ShortArray()
//  {
//      // CLASS INVARIANTS
//      assert(d_array_p);
//      assert(0 <= d_size);
//      assert(0 <= d_length); assert(d_length <= d_size);
//      assert(d_allocator_p);
//
//      d_allocator_p->deallocate(d_array_p);
//  }
//
//  inline
//  void my_ShortArray::append(int value)
//  {
//      if (d_length >= d_size) {
//          increaseSize();
//      }
//      d_array_p[d_length++] = value;
//  }
//
//  static
//  void reallocate(short **array, int newSize, int length,
//                  bdema_Allocator *basicAllocator)
//      // Reallocate memory in the specified 'array' to the specified
//      // 'newSize' using the specified 'basicAllocator' or global new
//      // operator.  The specified 'length' number of leading elements are
//      // preserved.  Since the class invariant requires that the physical
//      // capacity of the container may grow but never shrink, the behavior
//      // is undefined unless length <= newSize.
//  {
//      assert(array);
//      assert(1 <= newSize);
//      assert(0 <= length);
//      assert(length <= newSize);        // enforce class invariant
//      assert(basicAllocator);
//
//      short *tmp = *array;              // support exception neutrality
//      *array = (short *) basicAllocator->allocate(newSize * sizeof **array);
//
//      // COMMIT POINT
//
//      memcpy(*array, tmp, length * sizeof **array);
//      basicAllocator->deallocate(tmp);
//  }
//
//  void my_ShortArray::increaseSize()
//  {
//       int proposedNewSize = d_size * GROW_FACTOR;    // reallocate can throw
//       assert(proposedNewSize > d_length);
//       reallocate(&d_array_p, proposedNewSize, d_length, d_allocator_p);
//       d_size = proposedNewSize;                      // we're committed
//  }
//  // ...
//..
// Use buffer allocator to allocate memory for 'my_ShortArray' as follows:
//..
//  void *callbackFunc(int size)
//  {
//      bsl::cerr << "Buffer allocator out of memory." << bsl::endl;
//      exit(-1);
//      return (void *) 0;
//  }
//  // ...
//
//  int main(int argc, char *argv[])
//  {
//  // ...
//      static char memory[1024];  // No alignment issue; see below.
//      bdema_BufferAllocator allocator(memory, sizeof memory, callbackFunc);
//
//      my_ShortArray mA(allocator);
//      const my_ShortArray& A = mA;
//      mA.append(123);
//      mA.append(456);
//  // ...
//  }
//..
// Note that in the 'main' function above, 'memory' does not need to be aligned
// because 'bdema_BufferAllocator::allocate' internally performs alignment for
// each requested memory block based on the allocator's alignment strategy.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMA_BUFFERALLOCATOR
#include <bslma_bufferallocator.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

typedef bslma_BufferAllocator bdema_BufferAllocator;
    // This 'class' defines a concrete, fixed-size buffer allocator
    // implementing the 'bdema_Allocator' interface.  The allocator supports
    // both maximum (default) and natural alignment strategies.

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
