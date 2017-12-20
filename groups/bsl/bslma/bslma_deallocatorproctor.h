// bslma_deallocatorproctor.h                                         -*-C++-*-
#ifndef INCLUDED_BSLMA_DEALLOCATORPROCTOR
#define INCLUDED_BSLMA_DEALLOCATORPROCTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a proctor to conditionally manage a block memory.
//
//@CLASSES:
//  bslma::DeallocatorProctor: proctor to conditionally manage a memory
//
//@SEE_ALSO: bslma_deallocatorguard, bslma_autodeallocator
//
//@DESCRIPTION: This component provides a proctor class template,
// 'bslma::DeallocatorProctor', to conditionally manage a block of
// (otherwise-unmanaged) memory.  If not explicitly released, the managed
// memory is deallocated automatically when the proctor object goes out of
// scope by freeing the memory using the parameterized 'ALLOCATOR' (allocator
// or pool) supplied at construction.  Note that after a proctor object
// releases its managed memory, the same proctor can be reused to conditionally
// manage another block of memory (allocated from the same allocator or pool
// that was supplied at construction) by invoking the 'reset' method.
//
///Requirement
///-----------
// The parameterized 'ALLOCATOR' type of the 'bslma::DeallocatorProctor' class
// must provide a (possibly 'virtual') method:
//..
//  void deallocate(void *address);
//..
// to deallocate memory at the specified 'address' (originally supplied by the
// 'ALLOCATOR' object).
//
///Usage
///-----
// The 'bslma::DeallocatorProctor' is normally used to achieve *exception*
// *safety* in an *exception* *neutral* way by managing memory in a sequence of
// continuous memory allocations.  Since each memory allocation may
// potentially throw an exception, an object of this proctor class can be used
// to (temporarily) manage newly allocated memory while attempting to allocate
// additional memory.  Should an exception occur in subsequent memory
// allocation, the proctor's destructor deallocates its managed memory,
// preventing a memory leak.
//
// This example illustrate a typical use of 'bslma::DeallocatorProctor'.
// Suppose we have an array class that stores an "in-place" representation of
// objects of parameterized 'TYPE':
//..
//  // my_array.h
//  // ...
//
//  template <class TYPE>
//  class my_Array {
//      // This class implements an "in-place" array of objects of
//      // parameterized 'TYPE' stored contiguously in memory.
//
//      // DATA
//      TYPE             *d_array_p;      // dynamically allocated array
//      int               d_length;       // logical length of this array
//      int               d_size;         // physical capacity of this array
//      bslma::Allocator *d_allocator_p;  // allocator (held, not owned)
//
//    public:
//      // CREATORS
//      my_Array(bslma::Allocator *basicAllocator = 0);
//          // Create a 'my_Array' object.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      // ...
//
//      ~my_Array();
//          // Destroy this 'my_Array' object and all elements currently
//          // stored.
//
//      // MANIPULATORS
//      // ...
//
//      void append(const TYPE& object);
//          // Append (a copy of) the specified 'object' of parameterized
//          // 'TYPE' to (the end of) this array.
//
//      // ...
//  };
//..
// Note that the rest of the 'my_Array' interface (above) and implementation
// (below) is elided as the portion shown is sufficient to demonstrate the use
// of 'bslma::DeallocatorProctor'.
//..
//  // CREATORS
//  template <class TYPE>
//  inline
//  my_Array<TYPE>::my_Array(bslma::Allocator *basicAllocator)
//  : d_length(0)
//  , d_size(1)
//  , d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//      d_array_p = (TYPE *)d_allocator_p->allocate(sizeof(TYPE));
//  }
//
//  template <class TYPE>
//  my_Array<TYPE>::~my_Array()
//  {
//      for (int i = 0; i < d_length; ++i) {
//          d_array_p[i].~TYPE();
//      }
//      d_allocator_p->deallocate(d_array_p);
//  }
//..
// In order to implement the 'append' function, we first have to introduce an
// 'my_AutoDestructor' 'class', which automatically destroy a sequence of
// managed objects upon destruction.  See 'bslma::AutoDestructor' for a similar
// component with full documentation:
//..
//  // my_autodestructor.h
//  // ...
//
//  template <class TYPE>
//  class my_AutoDestructor {
//      // This class implements a range proctor that, unless its 'release'
//      // method has previously been invoked, automatically invokes the
//      // destructor of each of sequence of objects it manages.
//
//      // DATA
//      TYPE * d_origin_p;
//      int    d_length;
//
//    public:
//      // CREATORS
//      my_AutoDestructor(TYPE *origin, int length)
//          // Create an 'my_AutoDestructor' to manage a contiguous sequence of
//          // objects.
//      : d_origin_p(origin)
//      , d_length(length)
//      {
//      }
//
//      ~my_AutoDestructor()
//          // Destroy this 'my_AutoDestructor' and, unless its 'release'
//          // method has previously been invoked, destroy the sequence of
//          // objects it manages by invoking the destructor of each of the
//          // (managed) objects.
//      {
//          if (d_length) {
//              for (; d_length > 0; --d_length, ++d_origin_p) {
//                  d_origin_p->~TYPE();
//              }
//          }
//      }
//
//      // MANIPULATORS
//      my_AutoDestructor<TYPE>& operator++()
//          // Increase by one the length of the sequence of objects managed by
//          // this range proctor.
//      {
//          ++d_length;
//          return *this;
//      }
//
//      void release()
//          // Release from management the sequence of objects currently
//          // managed by this range proctor.
//      {
//          d_length = 0;
//      }
//  };
//..
// We can now continue with our implementation of the 'my_Array' class:
//..
//  // my_array.h
//  // ...
//
//  // MANIPULATORS
//  template <class TYPE>
//  void my_Array<TYPE>::append(const TYPE &object)
//  {
//      if (d_length == d_size) {
//          TYPE *newArray = (TYPE *)d_allocator_p->allocate(
//                               d_size * 2 * sizeof(TYPE));  // possibly throw
//
//          //*****************************************************************
//          // Note the use of the deallocator proctor on 'newArray' (below). *
//          //*****************************************************************
//
//          bslma::DeallocatorProctor<bslma::Allocator> proctor(newArray,
//                                                              d_allocator_p);
//
//          // Note use of 'my_AutoDestructor' here to protect the copy
//          // construction of 'TYPE' objects.
//          my_AutoDestructor<TYPE> destructor(newArray, 0);
//
//          for (int i = 0; i < d_length; ++i) {
//              new(&newArray[i]) TYPE(d_array_p[i], d_allocator_p);
//                                                           // possibly throw
//              d_array_p[i].~TYPE();
//              ++destructor;
//          }
//
//          destructor.release();
//
//          //*********************************************************
//          // Note that the deallocator proctor is released (below). *
//          //*********************************************************
//
//          proctor.release();
//
//          d_allocator_p->deallocate(d_array_p);
//          d_array_p = newArray;
//          d_size   *= 2;
//      }
//
//      new(&d_array_p[d_length]) TYPE(object, d_allocator_p);
//      ++d_length;
//  }
//..
// Both the use of 'bslma::DeallocatorProctor' and 'my_AutoDestructor' are
// necessary to implement exception safety.
//
// The 'append' method defined above potentially throws in two places.  If the
// memory allocator held in 'd_allocator_p' where to throw while attempting to
// allocate the new array of parameterized 'TYPE', no memory would be leaked.
// But without subsequent use of the 'bslma::DeallocatorProctor', if the
// allocator subsequently throws while copy constructing the objects from the
// old array to the new array, the newly allocated memory block would be
// leaked.  Using the 'bslma::DeallocatorProctor' prevents the leak by
// deallocating the proctored memory automatically should the proctor go out
// of scope before the 'release' method of the proctor is called (such as when
// the function exits prematurely due to an exception).
//
// Similarly, any resources acquired as a result of copy constructing the
// objects from the old array to the new array would be leaked if the
// constructor of 'TYPE' throws.  Using the 'my_AutoDestructor' prevents the
// leak by invoking the destructor of the proctored (and newly created) objects
// in the new array should the 'my_AutoDestructor' goes out of scope before the
// 'release' method of the proctor is called.
//
// Note that the 'append' method assumes the copy constructor of 'TYPE' takes
// an allocator as a second argument.  In production code, a constructor proxy
// that checks the traits of 'TYPE' (to see whether 'TYPE' uses
// 'bslma::Allocator') should be used (see 'bslalg::ConstructorProxy').

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

namespace BloombergLP {

namespace bslma {

                        // ========================
                        // class DeallocatorProctor
                        // ========================

template <class ALLOCATOR>
class DeallocatorProctor {
    // This class implements a proctor that, unless its 'release' method has
    // previously been invoked, automatically deallocates a block of managed
    // memory upon destruction by invoking the 'deallocate' method of an
    // allocator (or pool) of parameterized 'ALLOCATOR' type supplied to it at
    // construction.  The managed memory must have been provided by this
    // allocator (or pool), which must remain valid throughout the lifetime of
    // the proctor object.

    // DATA
    void      *d_memory_p;     // address of managed memory
    ALLOCATOR *d_allocator_p;  // allocator or pool (held, not owned)

    // NOT IMPLEMENTED
    DeallocatorProctor(const DeallocatorProctor&);
    DeallocatorProctor& operator=(const DeallocatorProctor&);

  public:
    // CREATORS
    DeallocatorProctor(void *memory, ALLOCATOR *allocator);
        // Create a deallocator proctor that conditionally manages the
        // specified 'memory' (if non-zero), and that uses the specified
        // 'allocator' to deallocate the block of memory managed by this
        // proctor (if not released -- see 'release') upon destruction.  The
        // behavior is undefined unless 'allocator' is non-zero and supplied
        // 'memory'.  Note that 'allocator' must remain valid throughout the
        // lifetime of this proctor.

    ~DeallocatorProctor();
        // Destroy this deallocator proctor, and deallocate the block of memory
        // it manages (if any) by invoking the 'deallocate' method of the
        // allocator (or pool) that was supplied at construction of this
        // proctor.  If no memory is currently being managed, this method has
        // no effect.

    // MANIPULATORS
    void release();
        // Release from management the block of memory currently managed by
        // this proctor.  If no memory is currently being managed, this method
        // has no effect.

    void reset(void *memory);
        // Set the specified 'memory' as the block of memory to be managed by
        // this proctor.  The behavior is undefined unless 'memory' is non-zero
        // and was allocated by the allocator (or pool) supplied at
        // construction.  Note that this method releases any previously-managed
        // memory from management (without deallocating it), and so may be
        // invoked with or without having called 'release' when reusing this
        // object.
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                        // ------------------------
                        // class DeallocatorProctor
                        // ------------------------

// CREATORS
template <class ALLOCATOR>
inline
DeallocatorProctor<ALLOCATOR>:: DeallocatorProctor(void *memory,
                                                   ALLOCATOR *allocator)
: d_memory_p(memory)
, d_allocator_p(allocator)
{
    BSLS_ASSERT_SAFE(allocator);
}

template <class ALLOCATOR>
inline
DeallocatorProctor<ALLOCATOR>::~DeallocatorProctor()
{
    BSLS_ASSERT_SAFE(d_allocator_p);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 != d_memory_p)) {
        d_allocator_p->deallocate(d_memory_p);
    }
}

// MANIPULATORS
template <class ALLOCATOR>
inline
void DeallocatorProctor<ALLOCATOR>::release()
{
    d_memory_p = 0;
}

template <class ALLOCATOR>
inline
void DeallocatorProctor<ALLOCATOR>::reset(void *memory)
{
    BSLS_ASSERT_SAFE(memory);

    d_memory_p = memory;
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
