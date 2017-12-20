// bslma_mallocfreeallocator.h                                        -*-C++-*-
#ifndef INCLUDED_BSLMA_MALLOCFREEALLOCATOR
#define INCLUDED_BSLMA_MALLOCFREEALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide malloc/free adaptor to 'bslma::Allocator' protocol.
//
//@CLASSES:
//  bslma::MallocFreeAllocator: support malloc/free style allocate/deallocate
//
//@SEE_ALSO: bslma_newdeleteallocator, bslma_allocator
//
//@DESCRIPTION: This component provides an allocator,
// 'bslma::MallocFreeAllocator', that implements the 'bslma::Allocator'
// protocol and supplies memory using the system-supplied (native)
// 'std::malloc' and 'std::free' operators.
//..
//   ,--------------------------.
//  ( bslma::MallocFreeAllocator )
//   `--------------------------'
//                |         ctor/dtor
//                |         singleton
//                V
//        ,----------------.
//       ( bslma::Allocator )
//        `----------------'
//                        allocate
//                        deallocate
//..
// The key purpose of this component is to facilitate the use of 'malloc' and
// 'free' when the default 'bslma::NewDeleteAllocator' is not desirable (such
// as the case of 'bslma::TestAllocator').  To accomplish this goal, 'malloc'
// and 'free' are wrapped in a singleton object whose lifetime is guaranteed to
// exceed any possibility of its use.
//
///Thread Safety
///-------------
// A single object of 'bslma::MallocFreeAllocator' is safe for concurrent
// access by multiple threads.  The underlying implementation of 'malloc' and
// 'free' will ensure that heap corruption does not occur.  Note that this
// allocator therefore has a stronger thread safety guarantee than is required
// by the base-class contract or than is provided by other allocators.
//
///Usage
///-----
// This component is intended to be used when the use of 'new' and 'delete' are
// not desirable, such as the case of 'bslma::TestAllocator'.  Instead of using
// 'bslma::Default' which uses the 'bslma::NewDeleteAllocator', this component
// can be used to bypass the use of 'new' and 'delete'.
//
// The following example demonstrates the use of this component for a user
// defined allocator instead of using the default allocator:
//..
//  // my_allocator.h
//  // ...
//
//  class my_Allocator : public bslma::Allocator {
//      // This class provides a mechanism for allocation and deallocation.
//
//      // DATA
//      bslma::Allocator *d_allocator_p;  // allocator (held, not owned)
//
//    public:
//      // CREATORS
//      my_Allocator(bslma::Allocator *basicAllocator = 0);
//          // Create a 'my_Allcoator'.  Optionally specify 'basicAllocator' to
//          // supply memory.  If 'basicAllocator' is 0, the
//          // 'bslma::MallocFreeAllocator' will be used.
//
//      ~my_Allocator();
//          // Destroy this allocator.  Note that the behavior of destroying an
//          // allocator while memory is allocated from it is not specified.
//          // (Unless you *know* that it is valid to do so, don't!)
//
//      // MANIPULATORS
//      void *allocate(size_type size);
//          // Return a newly allocated block of memory of (at least) the
//          // specified positive 'size' (bytes).  If 'size' is 0, a null
//          // pointer is returned with no effect.  Note that the alignment of
//          // the address returned is the maximum alignment for any
//          // fundamental type defined for this platform.
//
//      void deallocate(void *address);
//          // Return the memory at the specified 'address' back to this
//          // allocator.  If 'address' is 0, this function has no effect.  The
//          // behavior is undefined if 'address' was not allocated using this
//          // allocator, or has already been deallocated.
//  };
//..
// The constructor is implemented using 'bslma::MallocFreeAllocator'.
//..
//  // my_allocator.cpp
//  // ...
//
//  // CREATORS
//  my_Allocator::my_Allocator(bslma::Allocator *basicAllocator)
//  : d_allocator_p(basicAllocator
//                  ? basicAllocator
//                  : &bslma::MallocFreeAllocator::singleton())
//  {
//  }
//
//  // ...
//..
// When the 'basicAllocator' is not specified, the 'bslma::MallocFreeAllocator'
// will be used.  That allocator then then calls 'std::malloc' and 'std::free'
// for allocating and deallocating memory.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_CSTDLIB
#include <cstdlib>  // 'std::malloc', 'std::free'
#define INCLUDED_CSTDLIB
#endif

namespace BloombergLP {

namespace bslma {

                        // =========================
                        // class MallocFreeAllocator
                        // =========================

class MallocFreeAllocator : public Allocator {
    // This class provides direct access to the system-supplied (native) global
    // 'std::malloc' and 'std::free'.  A 'static' method is provided for
    // obtaining a unique, process wide object of this class, which is valid
    // from the time the method is called until after the program (not just
    // 'main') exits.

  private:
    // NOT IMPLEMENTED
    MallocFreeAllocator(const MallocFreeAllocator&);
    MallocFreeAllocator& operator=(const MallocFreeAllocator&);

  public:
    // CLASS METHODS
    static MallocFreeAllocator& singleton();
        // Return a reference to a valid object of this class.  Note that this
        // object is guaranteed to be valid from the time of this call onward
        // (i.e., not just until exiting 'main').

    // CREATORS
    MallocFreeAllocator();
        // Create an allocator that uses 'std::malloc' and 'std::free' to
        // supply memory.  Note that all objects of this class share the same
        // underlying resource.

    virtual ~MallocFreeAllocator();
        // Destroy this allocator.  Note that the behavior of destroying an
        // allocator while memory is allocated from it is not specified.
        // (Unless you *know* that it is valid to do so, don't!)
        //
        // For this concrete implementation, destroying this allocator object
        // has no effect on allocated memory.

    // MANIPULATORS
    virtual void *allocate(size_type size);
        // Return a newly allocated block of memory of (at least) the specified
        // positive 'size' (in bytes).  If 'size' is 0, a null pointer is
        // returned with no other effect.  If this allocator cannot return the
        // requested number of bytes, then it will return a null pointer.  The
        // behavior is undefined unless '0 <= size'.  Note that the alignment
        // of the address returned is the maximum alignment for any type
        // defined on this platform.  Also note that 'std::malloc' is *not*
        // called when 'size' is 0 (in order to avoid having to acquire a lock,
        // and potential contention in multi-treaded programs).

    virtual void deallocate(void *address);
        // Return the memory block at the specified 'address' back to this
        // allocator.  If 'address' is 0, this function has no effect.  The
        // behavior is undefined unless 'address' was allocated using this
        // allocator object and has not already been deallocated.  Note that
        // 'std::free' is *not* called when 'address' is 0 (in order to avoid
        // having to acquire a lock, and potential contention in multi-treaded
        // programs).
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                        // -------------------------
                        // class MallocFreeAllocator
                        // -------------------------

// CREATORS
inline
MallocFreeAllocator::MallocFreeAllocator()
{
}

inline
MallocFreeAllocator::~MallocFreeAllocator()
{
}

// MANIPULATORS
inline
void MallocFreeAllocator::deallocate(void *address)
{
    // While the C and C++ standard guarantees that calling free(0) is safe
    // (3.7.3.2 paragraph 3), some libc implementations take out a lock to deal
    // with the free(0) case, so this check can improve efficiency of threaded
    // programs.

    if (address) {
        std::free(address);
    }
}

}  // close package namespace


}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
