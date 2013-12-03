// bdlma_autoreleaser.h                                               -*-C++-*-
#ifndef INCLUDED_BDLMA_AUTORELEASER
#define INCLUDED_BDLMA_AUTORELEASER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Release memory to a managed allocator or pool at destruction.
//
//@CLASSES:
//  bdlma::AutoReleaser: proctor to release memory to a managed allocator/pool
//
//@SEE_ALSO: bslma_deallocatorproctor, bdlma_managedallocator
//
//@DESCRIPTION: This component provides a proctor object to manage memory
// allocated from a managed allocator or pool.  The proctor's destructor
// invokes the 'release' method of its managed allocator or pool unless the
// proctor's own 'release' method has been called.  Note that after a proctor
// releases management of its managed allocator or pool, the proctor can be
// reused by invoking its 'reset' method with another allocator or pool object
// (of the same (template parameter) type 'ALLOCATOR').
//
///REQUIREMENTS
///------------
// The object of the (template parameter) type 'ALLOCATOR' must provide a
// method having the following signature:
//..
//  void release();
//..
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using a 'bdlma::AutoReleaser' to Preserve Exception Neutrality
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A 'bdlma::AutoReleaser' proctor is often used to preserve exception
// neutrality for containers that allocate their elements using a managed
// allocator or pool.  For operations that may potentially throw an exception,
// a proctor can be used to (temporarily) manage the container's allocator or
// pool and its associated memory.  If an exception is thrown, the proctor's
// destructor invokes the 'release' method of its held allocator or pool,
// deallocating memory for all of the container's elements, thereby preventing
// a memory leak and restoring the container to the empty state.
//
// In this example, we illustrate use of a 'bdlma::AutoReleaser' proctor within
// the 'operator=' method of 'my_FastStrArray', a class that implements an
// array of C string elements.  Note that a 'my_FastStrArray' object allocates
// memory for its C string elements using a string pool, 'my_StrPool', the
// definition of which is elided.
//
// First, we define the interface of our 'my_FastStrArray' class:
//..
//  // my_fastcstrarray.h
//
//  class my_FastCstrArray {
//      // This class implements an array of C string elements.  Each C string
//      // is allocated using the 'my_StrPool' member for fast memory
//      // allocation and deallocation.
//
//      // DATA
//      char             **d_array_p;      // dynamically allocated array
//      int                d_capacity;     // physical capacity of this array
//      int                d_length;       // logical length of this array
//      my_StrPool         d_strPool;      // memory manager to supply memory
//      bslma::Allocator  *d_allocator_p;  // held, not owned
//
//    private:
//      // PRIVATE MANIPULATORS
//      void increaseSize();
//
//    public:
//      // CREATORS
//      my_FastCstrArray(bslma::Allocator *basicAllocator = 0);
//      ~my_FastCstrArray();
//
//      // MANIPULATORS
//      my_FastCstrArray& operator=(const my_FastCstrArray& rhs);
//      void append(const char *item);
//
//      // ACCESSORS
//      const char *operator[](int index) const { return d_array_p[index]; }
//      int length() const { return d_length; }
//  };
//
//  // ...
//..
// Finally, we implement 'my_FastCstrArray::operator=' using a
// 'bdlma::AutoReleaser' proctor to preserve exception neutrality:
//..
//  // my_fastcstrarray.cpp
//  #include <my_fastcstrarray.h>
//
//  // ...
//
//  // MANIPULATORS
//  my_FastCstrArray&
//  my_FastCstrArray::operator=(const my_FastCstrArray& rhs)
//  {
//      if (&rhs != this) {
//          d_strPool.release();
//          d_length = 0;
//
//          if (rhs.d_length > d_capacity) {
//              char **tmp = d_array_p;
//              d_array_p = (char **)d_allocator_p->allocate(
//                                           rhs.d_length * sizeof *d_array_p);
//              d_capacity = rhs.d_length;
//              d_allocator_p->deallocate(tmp);
//          }
//
//          bdlma::AutoReleaser<my_StrPool> autoReleaser(&d_strPool);
//
//          for (int i = 0; i < rhs.d_length; ++i) {
//              const int size = bsl::strlen(rhs.d_array_p[i]) + 1;
//              d_array_p[i] = (char *)d_strPool.allocate(size);
//              bsl::memcpy(d_array_p[i], rhs.d_array_p[i], size);
//          }
//
//          d_length = rhs.d_length;
//          autoReleaser.release();
//      }
//
//      return *this;
//  }
//
//  // ...
//..
// Note that a 'bdlma::AutoReleaser' proctor is used to manage the array's C
// string memory pool while allocating memory for the individual elements.  If
// an exception is thrown during the 'for' loop, the proctor's destructor
// releases memory for all elements allocated through the pool, thus ensuring
// that no memory is leaked.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

namespace BloombergLP {
namespace bdlma {

                        // ==================
                        // class AutoReleaser
                        // ==================

template <class ALLOCATOR>
class AutoReleaser {
    // This class implements a proctor that invokes the 'release' method of its
    // managed allocator or pool at destruction unless the proctor's 'release'
    // method is invoked.

    // DATA
    ALLOCATOR *d_allocator_p;  // allocator or pool (held, not owned)

  private:
    // NOT IMPLEMENTED
    AutoReleaser(const AutoReleaser&);
    AutoReleaser& operator=(const AutoReleaser&);

  public:
    // CREATORS
    AutoReleaser(ALLOCATOR *originalAllocator);
        // Create a proctor object to manage the specified 'originalAllocator'.
        // Unless the 'release' method of this proctor is invoked, the
        // 'release' method of 'originalAllocator' is automatically invoked
        // upon destruction of this proctor.

    ~AutoReleaser();
        // Destroy this proctor object and, unless the 'release' method has
        // been invoked on this object with no subsequent call to 'reset',
        // invoke the 'release' method of the held allocator or pool.

    // MANIPULATORS
    void release();
        // Release from management the allocator or pool currently managed by
        // this proctor.  If no allocator or pool is currently being managed,
        // this method has no effect.

    void reset(ALLOCATOR *newAllocator);
        // Set the specified 'newAllocator' as the allocator or pool to be
        // managed by this proctor.  Note that this method releases from
        // management any previously held allocator or pool, and so may be
        // invoked with or without having called 'release' when reusing this
        // object.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ------------------
                        // class AutoReleaser
                        // ------------------

// CREATORS
template <class ALLOCATOR>
inline
AutoReleaser<ALLOCATOR>::AutoReleaser(ALLOCATOR *originalAllocator)
: d_allocator_p(originalAllocator)
{
}

template <class ALLOCATOR>
inline
AutoReleaser<ALLOCATOR>::~AutoReleaser()
{
    if (d_allocator_p) {
        d_allocator_p->release();
    }
}

// MANIPULATORS
template <class ALLOCATOR>
inline
void AutoReleaser<ALLOCATOR>::release()
{
    d_allocator_p = 0;
}

template <class ALLOCATOR>
inline
void AutoReleaser<ALLOCATOR>::reset(ALLOCATOR *newAllocator)
{
    d_allocator_p = newAllocator;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
