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
//@DESCRIPTION: This component provides a proctor object,
// 'bdlma::AutoReleaser', to manage memory allocated from a managed allocator
// or pool.  The proctor's destructor invokes the 'release' method of its
// managed allocator or pool unless the proctor's own 'release' method has been
// called.  Note that after a proctor releases management of its managed
// allocator or pool, the proctor can be reused by invoking its 'reset' method
// with another allocator or pool object (of the same (template parameter) type
// 'ALLOCATOR').
//
///Requirements
///------------
// The object of the (template parameter) type 'ALLOCATOR' must provide a
// method having the following signature:
//..
//  void release();
//..
//
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
//      // Not implemented:
//      my_FastCstrArray(const my_FastCstrArray&);
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
//  // FREE OPERATORS
//  ostream& operator<<(ostream& stream, const my_FastCstrArray& array);
//
//..
// Then, we implement the methods:
//..
//  enum {
//      k_MY_INITIAL_SIZE = 1, // initial physical capacity
//      k_MY_GROW_FACTOR  = 2  // factor by which to grow 'd_capacity'
//  };
//
//  static inline
//  int nextSize(int size)
//      // Return the specified 'size' multiplied by 'k_MY_GROW_FACTOR'.
//  {
//      return size * k_MY_GROW_FACTOR;
//  }
//
//  static inline
//  void reallocate(char             ***array,
//                  int                *size,
//                  int                 newSize,
//                  int                 length,
//                  bslma::Allocator   *allocator)
//      // Reallocate memory in the specified 'array' and update the specified
//      // 'size' to the specified 'newSize', using the specified 'allocator'
//      // to supply memory.  The specified 'length' number of leading elements
//      // are preserved.  If 'allocate' should throw an exception, this
//      // function has no effect.  The behavior is undefined unless
//      // '1 <= newSize', '0 <= length', and 'length <= newSize'.
//  {
//      ASSERT(array);
//      ASSERT(*array);
//      ASSERT(size);
//      ASSERT(1 <= newSize);
//      ASSERT(0 <= length);
//      ASSERT(length <= *size);    // sanity check
//      ASSERT(length <= newSize);  // ensure class invariant
//
//      char **tmp = *array;
//
//      *array = (char **)allocator->allocate(newSize * sizeof **array);
//
//      // commit
//      bsl::memcpy(*array, tmp, length * sizeof **array);
//      *size = newSize;
//      allocator->deallocate(tmp);
//  }
//
//  void my_FastCstrArray::increaseSize()
//  {
//      reallocate(&d_array_p,
//                 &d_capacity,
//                 nextSize(d_capacity),
//                 d_length,
//                 d_allocator_p);
//  }
//
//  // CREATORS
//  my_FastCstrArray::my_FastCstrArray(bslma::Allocator *basicAllocator)
//  : d_capacity(k_MY_INITIAL_SIZE)
//  , d_length(0)
//  , d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//      d_array_p = (char **)d_allocator_p->allocate(
//                                             d_capacity * sizeof *d_array_p);
//  }
//
//  my_FastCstrArray::~my_FastCstrArray()
//  {
//      ASSERT(1        <= d_capacity);
//      ASSERT(0        <= d_length);
//      ASSERT(d_length <= d_capacity);
//
//      d_allocator_p->deallocate(d_array_p);
//  }
//..
// Now, we implement 'my_FastCstrArray::operator=' using a
// 'bdlma::AutoReleaser' proctor to preserve exception neutrality:
//..
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
//              const int size =
//                         static_cast<int>(bsl::strlen(rhs.d_array_p[i])) + 1;
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
//..
// Note that a 'bdlma::AutoReleaser' proctor is used to manage the array's C
// string memory pool while allocating memory for the individual elements.  If
// an exception is thrown during the 'for' loop, the proctor's destructor
// releases memory for all elements allocated through the pool, thus ensuring
// that no memory is leaked.
//
// Finally, we complete the implementation:
//..
//  void my_FastCstrArray::append(const char *item)
//  {
//      if (d_length >= d_capacity) {
//          this->increaseSize();
//      }
//      const int sSize = static_cast<int>(bsl::strlen(item)) + 1;
//      char *elem = (char *)d_strPool.allocate(sSize);
//      bsl::memcpy(elem, item, sSize * sizeof *item);
//      d_array_p[d_length] = elem;
//      ++d_length;
//  }
//
//  // FREE OPERATORS
//  ostream& operator<<(ostream& stream, const my_FastCstrArray& array)
//  {
//      stream << "[ ";
//      for (int i = 0; i < array.length(); ++i) {
//          stream << '"' << array[i] << "\" ";
//      }
//      return stream << ']' << flush;
//  }
//..

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
//                             INLINE DEFINITIONS
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
