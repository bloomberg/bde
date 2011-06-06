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
//@AUTHOR: Andrew Paprocki (apaprock)
//
//@DESCRIPTION: This component extends the base-level protocol (pure abstract
// interface) class, 'bslma_Allocator', providing the ability to allocate
// raw memory with a specified alignment.  The functional capabilities
// documented by this protocol are similar to those afforded by
// POSIX 'posix_memalign': sufficiently aligned memory is guaranteed for any
// object of a given size and alignment.
//..
///Usage
///-----
// The 'bdema_AlignedAllocator' protocol provided in this component
// defines a bilateral contract between suppliers and consumers of raw
// aligned memory.  In order for the 'bdema_AlignedAllocator' interface to be
// useful, we must supply a concrete allocator that implements it.
// 
// In this example, we demonstrate how to adapt 'posix_memalign' to this
// protocol base class:
//
// First, we specify the interface of the concrete implementation of
// 'MyPosixMemAlignAllocator: 
//..
//  // myposixmemalignallocator.h
//  // ...
//
//  class MyPosixMemAlignAllocator: public bdema_AlignedAllocator {
//      // This class is a sample concrete implementation of the
//      // 'bdema_AlignedAllocator' protocol that provides direct access
//      // to the system-supplied 'posix_memalign' and 'free' functions.
//
//      // NOT IMPLEMENTED
//      bdema_PosixMemAlignAllocator(const bdema_PosixMemAlignAllocator&);
//      bdema_PosixMemAlignAllocator& operator=
//                                  (const bdema_PosixMemAlignAllocator&);
//
//    public:
//      // CREATORS
//      MyPosixMemAlignAllocator();
//          // Create a 'MyPosixMemAlignAllocator' object.  Note that all
//          // objects of this class share the same underlying resource.
//
//      virtual ~bdema_PosixMemAlignAllocator();
//          // Destroy this object.  Note that destroying this object has no
//          // effect on any outstanding allocated memory.
//
//      // MANIPULATORS
//      virtual void *allocateAligned(bsl::size_type size, int alignment);
//          // Return the address of a newly allocated block of memory of at
//          // least the specified positive 'size' (in bytes), sufficiently
//          // aligned such that '0 == (address & (alignemnet -1)).  If 'size'
//          // is 0, a null pointer is returned with no other effect.  If this
//          // allocator cannot return the requested number of bytes, then it
//          // throws an 'std::bad_alloc' exception, or abort if in a
//          // non-exception build.  The behavior is undefined unless '0 <=
//          // size' and 'alignment' is both a multiple of 'sizeof(void *)' and
//          // a power of two.  Note that the underlying 'posix_memalign'
//          // function is *not* called when 'size' is 0.
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
// Now, we define the virtual methods of 'bdema_PosixMemAlignAllocator', non
// inlined, as they would not be inlined when invoked from the base class (the
// typical usage in this case):
//..
//  // MANIPULATORS
//  void *bdema_PosixMemAlignAllocator::allocateAligned(size_type size,
//                                                      int       alignment)
//  {
//      BSLS_ASSERT_SAFE(0 <= size);
//      BSLS_ASSERT_SAFE(0 <= alignement);
//      BSLS_ASSERT_SAFE(0 == (alignement % 2));
//      BSLS_ASSERT_SAFE(0 == (alignement % sizeof(void *)));
//
//      if (0 < size) {
//          void *ret;
//          int rc = ::posix_memalign(&ret, alignment, size);
//          if (0 != rc) {
//              bslma_Allocator::throwBadAlloc();
//          }
//          return ret;
//      }
//
//      return 0;
//  }
//
//  void bdema_PosixMemAlignAllocator::deallocate(void *address)
//  {
//      ::free(address);
//  }
//..
// Finally, we instantiate 

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
    virtual void *allocateAligned(size_type size, int alignment)
                                        BSLS_ANNOTATION_WARN_UNUSED_RESULT = 0;
        // Return the address of a newly allocated block of memory of at
        // least the specified positive 'size' (in bytes), sufficiently
        // aligned such that '0 == (address & (alignemnet -1)).  If 'size'
        // is 0, a null pointer is returned with no other effect.  If this
        // allocator cannot return the requested number of bytes, then it
        // throws an 'std::bad_alloc' exception, or abort if in a
        // non-exception build.  The behavior is undefined unless
        // '0 <= size', '0 <= alignment' and 'alignment' is both a multiple of
        // 'sizeof(void *)' and a power of two.  Note that the underlying
        // 'posix_memalign' function is *not* called when 'size' is 0.
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
