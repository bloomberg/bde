// bdema_alignedallocator.h                                          -*-C++-*-
#ifndef INCLUDED_BDEMA_ALIGNEDALLOCATOR
#define INCLUDED_BDEMA_ALIGNEDALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide a protocol for memory allocators that support alignment.
//
//@CLASSES:
//  bdema_AlignedAllocator: protocol for aligned memory allocators
//
//@SEE_ALSO:
//  bdema_posixmemalignallocator
//
//@AUTHOR: Andrew Paprocki (apaprock), Stefano Pacifico (spacifico1)
//
//@DESCRIPTION: This component extends the base-level protocol (pure abstract
// interface) class, 'bslma_Allocator', providing the ability to allocate
// raw memory with a specified alignment.  The functional capabilities
// documented by this protocol are similar to those afforded by
// POSIX 'posix_memalign': sufficiently aligned memory is guaranteed for any
// object of a given size and alignment.
//..
//   ,----------------------.
//  ( bdema_AlignedAllocator )
//   `----------------------'
//               |       allocateAligned
//               V
//       ,---------------.
//      ( bslma_Allocator )
//       `---------------'
//                       allocate
//                       deallocate
//..
//
///Usage
///-----
// In the following examples we demonstrate how to use a
// 'bdema_AlignedAllocator' to allocate memory aligned according to a specified
// boundary.
//
///Example 1: Implementing 'bdema_AlignedAllocator'
///- - - - - - - - - - - - - - - - - - - - - - - -
// The 'bdema_AlignedAllocator' protocol provided in this component
// defines a bilateral contract between suppliers and consumers of raw
// aligned memory.  In order for the 'bdema_AlignedAllocator' interface to be
// useful, we must supply a concrete allocator that implements it.
//
// In this example, we demonstrate how to adapt 'posix_memalign' to this
// protocol base class:
//
// First, we specify the interface of the concrete implementation of
// 'MyAlignedAllocator:
//..
//  // myposixmemalignallocator.h
//  // ...
//
//  class MyAlignedAllocator: public bdema_AlignedAllocator {
//      // This class is a sample concrete implementation of the
//      // 'bdema_AlignedAllocator' protocol that provides direct access to the
//      // system-supplied 'posix_memalign' and 'free' on UNIX, or 
//      // '_aligned_malloc' and '_aligned_free' on Windows. 
//
//      // NOT IMPLEMENTED
//      MyAlignedAllocator(const MyAlignedAllocator&);
//      MyAlignedAllocator& operator=(const MyAlignedAllocator&);
//
//    public:
//      // CREATORS
//      MyAlignedAllocator();
//          // Create a 'MyAlignedAllocator' object.  Note that all
//          // objects of this class share the same underlying resource.
//
//      virtual ~MyAlignedAllocator();
//          // Destroy this object.  Note that destroying this object has no
//          // effect on any outstanding allocated memory.
//
//      // MANIPULATORS
//      virtual void *allocateAligned(size_type size, size_type alignment);
//          // Return the address of a newly allocated block of memory of at
//          // least the specified positive 'size' (in bytes), sufficiently
//          // aligned such that '0 == (address & (alignement - 1)).  If 'size'
//          // is 0, a null pointer is returned with no other effect.  If this
//          // allocator cannot return the requested number of bytes, then it
//          // throws an 'bsl::bad_alloc' exception, or abort if in a
//          // non-exception build.  The behavior is undefined unless 
//          // '0 <=  size' and 'alignment' is both a multiple of 
//          // 'sizeof(void *)' and a power of two.  Note that the underlying
//          // 'posix_memalign' function is *not* called when 'size' is 0.
//
//      virtual void deallocate(void *address);
//          // Return the memory block at the specified 'address' back to this
//          // allocator.  If 'address' is 0, this function has no effect.  The
//          // behavior is undefined unless 'address' was allocated using this
//          // allocator object and has not already been deallocated.  Note
//          // that the underlying 'posix_memalign' function is *not* called
//          // when 'address' is 0.
//  };
//  // ...
//..
// Now, we define the virtual methods of 'MyAlignedAllocator', non
// inlined, as they would not be inlined when invoked from the base class (the
// typical usage in this case):
//..
//  // MANIPULATORS
//  void *MyAlignedAllocator::allocate(size_type size)
//  void *MyAlignedAllocator::allocateAligned(size_type size,
//                                            size_type alignment)
//  {
//      BSLS_ASSERT_SAFE(0 <= size);
//      BSLS_ASSERT_SAFE(0 <= alignement);
//      BSLS_ASSERT_SAFE(1 == bdes_BitUtil::numSetOne64(alignment));
//      BSLS_ASSERT_SAFE(0 == (alignement % sizeof(void *)));
//
//      void *ret;
//
//  #ifdef BSLS_PLATFORM__OS_WINDOWS
//      errno = 0;
//      *ret = _aligned_malloc(size, alignment);
//      if (0 != errno) {
//          bslma_Allocator::throwBadAlloc();
//      }
//  #else
//      int rc = ::posix_memalign(&ret, alignment, size);
//      if (0 != rc) {
//          bslma_Allocator::throwBadAlloc();
//      }
//  #endif
//
//      return ret;
//  }
//
//  void MyAlignedAllocator::deallocate(void *address)
//  {
//      if (0 == address) {
//          return;                                                   // RETURN
//      }
//  #ifdef BSLS_PLATFORM_WINDOWS
//      _aligned_free(address);
//  #else
//      ::free(address);
//  }
//..
// Finally, we instantiate an object of type 'MyAlignedAllocator':
//..
//  MyAlignedAllocator myAlignedAllocator;
//..
//
///Example 2: Using a 'bdema_AlignedAllocator'.
///- - - - - - - - - - - - - - - - - - - - - -
// In this example we use an object of type 'MyAlignedAllocator', defined
// in the previous example to obtain memory aligned on a page boundary,
// assuming pages of 4096 bytes.
//
// First, we obtain a 'MyAlignedAllocator' reference to 'myP' we constructed in
// the previous example:
//..
//  bdema_AlignedAllocator *alignedAllocator = &myAlignedAllocator;
//..
// Now, we allocate a buffer of 1024 bytes of memory and make sure that it is
// aligned on a 4096 boundary:
//
//..
//  char *address = (char *) alignedAllocator->allocateAligned(1024, 4096);
//..
// Finally, we verify that the obtained address actually is aligned as
// expected:
//..
//  assert(buffer & (bsls_Allocator::size_type)4096 - 1);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ANNOTATION
#include <bsls_annotation.h>
#endif

namespace BloombergLP {

                        // ============================
                        // class bdema_AlignedAllocator
                        // ============================

class bdema_AlignedAllocator : public bslma_Allocator {
    // This protocol provides a pure abstract interface and contract for
    // clients and suppliers of raw aligned memory.  If the requested memory
    // cannot be returned, the contract requires that an 'std::bad_alloc'
    // exception be thrown.  Note that memory is guaranteed to be sufficiently
    // aligned for any pointer, which may be less than the alignment needed
    // for all objects.

  public:
    // MANIPULATORS
    virtual void *allocateAligned(size_type size, size_type alignment)
                                        BSLS_ANNOTATION_WARN_UNUSED_RESULT = 0;
        // Return the address of a newly allocated block of memory of at
        // least the specified positive 'size' (in bytes), sufficiently
        // aligned such that '0 == (address & (alignemnet -1)).  If 'size'
        // is 0, a null pointer is returned with no other effect.  If this
        // allocator cannot return the requested number of bytes, then it
        // throws an 'bsl::bad_alloc' exception, or abort if in a
        // non-exception build.  The behavior is undefined unless
        // '0 <= size', '0 <= alignment' and 'alignment' is both a multiple of
        // 'sizeof(void *)' and a power of two.  
    
    virtual void deallocate(void *address);
        // Return the memory block at the specified 'address' back to this
        // allocator.  If 'address' is 0, this function has no effect.  The
        // behavior is undefined unless 'address' was allocated using this
        // allocator object and has not already been deallocated.
};

}   // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
