// bdlma_alignedallocator.h                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLMA_ALIGNEDALLOCATOR
#define INCLUDED_BDLMA_ALIGNEDALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide a protocol for memory allocators that support alignment.
//
//@CLASSES:
//  bdlma::AlignedAllocator: protocol for aligned memory allocators
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides an implementation,
// 'bdlma::AlignedAllocator', of the base-level protocol (pure abstract
// interface) class, 'bslma::Allocator', providing the ability to allocate raw
// memory with a specified alignment.  The following inheritance diagram shows
// the classes involved and their methods:
//..
//   ,----------------------.
//  ( bdlma::AlignedAllocator )
//   `----------------------'
//               |       allocateAligned
//               V
//       ,----------------.
//      ( bslma::Allocator )
//       `----------------'
//                       allocate
//                       deallocate
//..
// The 'allocateAligned' method supplies the address of a contiguous block of
// allocated memory of at least the indicated size, that is aligned to a given
// boundary.  Note that this behavior is similar to the behavior of the POSIX
// function 'posix_memalign'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing 'bdlma::AlignedAllocator'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The 'bdlma::AlignedAllocator' protocol provided in this component defines a
// bilateral contract between suppliers and consumers of raw aligned memory.
// In order for the 'bdlma::AlignedAllocator' interface to be useful, we must
// supply a concrete allocator that implements it.
//
// In this example, we demonstrate how to adapt 'posix_memalign' on Linux and
// AIX, 'memalign' on SunOS and '_aligned_malloc' on Windows, to this protocol
// base class:
//
// First, we specify the interface of the concrete implementation of
// 'MyAlignedAllocator:
//..
//  // myposixmemalignallocator.h
//  // ...
//
//  class MyAlignedAllocator: public bdlma::AlignedAllocator {
//      // This class is a sample concrete implementation of the
//      // 'bdlma::AlignedAllocator' protocol that provides direct access to
//      // the system-supplied 'posix_memalign' and 'free' on Linux and AIX
//      // platforms, 'memalign' and 'free' on SunOS, or '_aligned_malloc' and
//      // '_aligned_free' on Windows.
//
//    private:
//      // NOT IMPLEMENTED
//      MyAlignedAllocator(const MyAlignedAllocator&);
//      MyAlignedAllocator& operator=(const MyAlignedAllocator&);
//
//    public:
//      // CREATORS
//      MyAlignedAllocator();
//          // Create a 'MyAlignedAllocator' object.  Note that all objects of
//          // this class share the same underlying resource.
//
//      virtual ~MyAlignedAllocator();
//          // Destroy this object.  Note that destroying this object has no
//          // effect on any outstanding allocated memory.
//
//      // MANIPULATORS
//      virtual void *allocate(size_type size);
//          // Return a newly allocated block of memory of (at least) the
//          // specified positive 'size' (in bytes).  If 'size' is 0, a null
//          // pointer is returned with no other effect.  If this allocator
//          // cannot return the requested number of bytes, then it will throw
//          // an 'std::bad_alloc' exception in an exception-enabled build, or
//          // else it will abort the program in a non-exception build.  The
//          // behavior is undefined unless '0 <= size'.   Note that the
//          // alignment of the address returned conforms to the platform
//          // requirement for any object of the 'size'.  Also note that global
//          // 'operator new' is *not* called when 'size' is 0 (in order to
//          // avoid having to acquire a lock, and potential contention in
//          // multi-threaded programs).
//
//      virtual void *allocateAligned(bsl::size_t size, size_type alignment);
//          // Return the address of a newly allocated block of memory of at
//          // least the specified positive 'size' (in bytes), sufficiently
//          // aligned such that the returned 'address' satisfies, for the
//          // specified 'alignment', '0 == (address & (alignment - 1))'.  If
//          // 'size' is 0, a null pointer is returned with no other effect.
//          // If the requested number of appropriately aligned bytes cannot be
//          // returned, then a 'bsl::bad_alloc' exception is thrown, or in a
//          // non-exception build the program is terminated.  The behavior is
//          // undefined unless 'alignment' is both a multiple of
//          // 'sizeof(void *)' and an integral non-negative power of two.
//
//      virtual void deallocate(void *address);
//          // Return the memory block at the specified 'address' back to this
//          // allocator.  If 'address' is 0, this function has no effect.  The
//          // behavior is undefined unless 'address' was allocated using this
//          // allocator object and has not already been deallocated.
//  };
//  // ...
//..
// Then, we implement the creators, trivially, as this class contains no
// instance data members.
//..
//  // CREATORS
//  MyAlignedAllocator::MyAlignedAllocator()
//  {
//  }
//
//  MyAlignedAllocator::~MyAlignedAllocator()
//  {
//  }
//..
// Now, we define the virtual methods of 'MyAlignedAllocator'.  Note that these
// definitions are not 'inline', as they would not be inlined when invoked from
// the base class (the typical usage in this case):
//..
//  // MANIPULATORS
//  void *MyAlignedAllocator::allocate(size_type size)
//  {
//      if (0 == size) {
//          return 0;                                                 // RETURN
//      }
//
//      int alignment = bsls::AlignmentUtil::calculateAlignmentFromSize(size);
//      return allocateAligned(size, alignment);
//  }
//
//  void *MyAlignedAllocator::allocateAligned(bsl::size_t size,
//                                            size_type   alignment)
//  {
//      BSLS_ASSERT_SAFE(0 == (alignment & (alignment - 1)));
//      BSLS_ASSERT_SAFE(0 == (alignment % sizeof(void *)));
//
//      if (0 == size) {
//          return 0;                                                 // RETURN
//      }
//
//      void *ret = 0;
//
//  #ifdef BSLS_PLATFORM_OS_WINDOWS
//      errno = 0;
//      ret = _aligned_malloc(size, alignment);
//      if (0 != errno) {
//          bsls::BslExceptionUtil::throwBadAlloc();
//      }
//  #elif defined(BSLS_PLATFORM_OS_SOLARIS) || defined(BSLS_PLATFORM_OS_HPUX)
//      ret = memalign(alignment, size);
//      if (0 == ret) {
//          bsls::BslExceptionUtil::throwBadAlloc();
//      }
//  #else
//      int rc = ::posix_memalign(&ret, alignment, size);
//      if (0 != rc) {
//          bsls::BslExceptionUtil::throwBadAlloc();
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
//  #endif
//  }
//..
// Finally, we define a function 'f' that instantiates an object of type
// 'MyAlignedAllocator':
//..
//  void f() {
//      MyAlignedAllocator a;
//  }
//..
// Note that the memory is not released when the allocator goes out of scope.
//
///Example 2: Using the 'bdlma::AlignedAllocator' Protocol
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we illustrate how to use the 'bdlma::AlignedAllocator'
// protocol to allocate memory that is aligned to the beginning of a memory
// page.  Third party libraries, for example device drivers that perform DMA
// access of device drivers, or some extreme optimizations to reduce the number
// of page faults, might require page aligned allocations.
//
// First, we create an aligned allocator 'myAlignedAllocator' using the class
// 'MyAlignedAllocator' defined in the previous example, and obtain a
// 'bdlma::AlignedAllocator' pointer to it:
//..
//  MyAlignedAllocator myAlignedAllocator;
//  bdlma::AlignedAllocator *alignedAllocator = &myAlignedAllocator;
//..
// Now, assuming a page size of 4K, we allocate a buffer of 1024 bytes of
// memory and indicate that it should be aligned on a 4096 boundary:
//..
//  char *address = (char *) alignedAllocator->allocateAligned(1024, 4096);
//..
// Finally, we verify that the obtained address actually is aligned as
// expected:
//..
//  assert(0 == ((bsl::size_t)address & (4096 - 1)));
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

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

namespace BloombergLP {
namespace bdlma {

                          // ======================
                          // class AlignedAllocator
                          // ======================

class AlignedAllocator : public bslma::Allocator {
    // This protocol provides a pure abstract interface and contract for
    // clients and suppliers of raw aligned memory.  If the requested memory
    // cannot be returned, the contract requires that an 'std::bad_alloc'
    // exception be thrown.

  public:
    // MANIPULATORS
    virtual void *allocateAligned(bsl::size_t size, size_type alignment) = 0;
        // Return the address of a newly allocated block of memory of at least
        // the specified positive 'size' (in bytes), sufficiently aligned such
        // that the returned 'address' satisfies, for the specified
        // 'alignment', '0 == (address & (alignment - 1))'.  If 'size' is 0, a
        // null pointer is returned with no other effect.  If the requested
        // number of appropriately aligned bytes cannot be returned, then a
        // 'bsl::bad_alloc' exception is thrown, or in a non-exception build
        // the program is terminated.  The behavior is undefined unless
        // 'alignment' is both a multiple of 'sizeof(void *)' and an integral
        // non-negative power of two.
};

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
