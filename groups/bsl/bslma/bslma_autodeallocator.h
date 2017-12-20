// bslma_autodeallocator.h                                            -*-C++-*-
#ifndef INCLUDED_BSLMA_AUTODEALLOCATOR
#define INCLUDED_BSLMA_AUTODEALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a range proctor to managed a block of memory.
//
//@CLASSES:
//  bslma::AutoDeallocator: range proctor to manage a block of memory
//
//@SEE_ALSO: bslma_deallocatorguard, bslma_deallocatorproctor
//
//@DESCRIPTION: This component provides a range proctor class template,
// 'bslma::AutoDeallocator', to manage a sequence of blocks of
// (otherwise-unmanaged) memory of parameterized 'TYPE' supplied at
// construction.  If not explicitly released, the sequence of managed memory
// blocks are deallocated automatically when the range proctor goes out of
// scope by freeing the memory using the parameterized 'ALLOCATOR' (allocator
// or pool) supplied at construction.  Note that after a range proctor releases
// its managed sequence of memory, the same proctor can be reused to
// conditionally manage another sequence of memory (allocated from the same
// allocator or pool that was supplied at construction) by invoking the 'reset'
// method.
//
///Requirement
///-----------
// The parameterized 'ALLOCATOR' type of the 'bslma::AutoRawDeleter' class
// template must provide a (possibly 'virtual') method:
//..
//  void deallocate(void *address);
//..
// to deallocate memory at the specified 'address' (originally supplied by the
// 'ALLOCATOR' object).
//
///Usage
///-----
// The 'bslma::AutoDeallocator' proctor object can be used to achieve
// *exception* *safety* in an *exception* *neutral* way during manipulation of
// "out-of-place" arrays of raw resources or memory.  Since there are no
// destructor calls, this component is more efficient compared to the
// 'bslma::AutoRawDeleter'.  The following illustrates the insertion operation
// for an "out-of-place" array of raw character sequences.  Assume that an
// array initially contains 5 character sequences as its elements:
//..
//     0     1     2     3     4
//   _____ _____ _____ _____ _____
//  |  o  |  o  |  o  |  o  |  o  |
//  `==|==^==|==^==|==^==|==^==|=='
//     |    _V___  |   __V___  |
//     |   |"Bye"| |  |"berg"| |
//     |   `=====' |  `======' |
//    _V_____     _V_____     _V__
//   |"Hello"|   |"Bloom"|   |"LP"|
//   `======='   `======='   `===='
//..
// To insert two more character sequences at index position 2, the array is
// first reallocated if it is not big enough, and then the existing elements at
// index positions 2, 3, and 4 are shifted:
//..
//     0     1     2     3     4     5     6
//   _____ _____ _____ _____ _____ _____ _____
//  |  o  |  o  |xxxxx|xxxxx|  o  |  o  |  o  |
//  `==|==^==|==^=====^=====^==|==^==|==^==|=='
//     |    _V___              |   __V___  |
//     |   |"Bye"|             |  |"berg"| |
//     |   `====='             |  `======' |
//    _V_____                 _V_____     _V__
//   |"Hello"|               |"Bloom"|   |"LP"|
//   `======='               `======='   `===='
//
//  Note: "xxxxx" denotes undefined value.
//..
// Next, two new memory blocks must be allocated to position 2 and 3.  If, one
// of the two allocations fails and an exception is thrown, the array will be
// left in an invalid state because the addresses contained at index positions
// 2 and 3 may be duplicates of those at index positions 4 and 5, or, if a
// resize occurred, invalid altogether.  We can restore exception neutrality by
// setting the array's length to 2 before attempting to create the string
// objects, but there is still a problem: the character sequences "Bloom",
// "berg", and "LP" (at index positions 4, 5, and 6) are "orphaned" and will
// never be deallocated -- a memory leak.  To prevent this potential memory
// leak, we can additionally create a 'bslma::AutoDeallocator' object to manage
// (temporarily) the memory at index positions 4, 5, and 6 prior to allocating
// the new memory:
//..
//      0     1     2     3     4     5     6
//    _____ _____ _____ _____ _____ _____ _____
//   |  o  |  o  |xxxxx|xxxxx|  o  |  o  |  o  |
//   `==|==^==|==^=====^=====^==|==^==|==^==|=='
//      |    _V___              |   __V___  |
//      |   |"Bye"|             |  |"berg"| |
//      |   `====='             |  `======' |
//     _V_____                 _V_____     _V__
//    |"Hello"|               |"Bloom"|   |"LP"|
//    `======='               `======='   `===='
//   my_StrArray              ^---------------bslma::AutoDeallocator
//   (length = 2)                          (origin = 4, length = 3)
//
//  Note: Configuration after initializing the proctor.
//..
// If an exception occurs, the array (now of length 2) is in a perfectly valid
// state, while the proctor is responsible for deallocating the orphaned memory
// at index positions 4, 5, and 6.  If no exception is thrown, the length is
// set to 7 and the proctor's 'release' method is called, releasing its control
// over the (temporarily) managed memory.
//
// The following example illustrates the use of 'bslma::AutoDeallocator' to
// manage temporarily an "out-of-place" array of character sequences during the
// array's insertion operation.
//
// First we define a 'my_StrArray' class that stores an array of character
// sequences.
//..
//  // my_strarray.h
//  // ...
//
//  template <class ALLOCATOR>
//  class my_StrArray {
//      // This class is a container that stores an array of character
//      // sequences.  Memory will be supplied by the parameterized 'ALLOCATOR'
//      // type provided at construction (which must remain valid throughout
//      // the lifetime of this guard object).  Note that memory is managed by
//      // a parameterized 'ALLOCATOR' type, instead of a 'bslma::Allocator',
//      // to enable clients to pass in a pool (such as a sequential pool)
//      // optimized for allocations of character sequences.
//
//      // DATA
//      char      **d_array_p;      // dynamically allocated array of character
//                                  // sequence
//
//      int         d_length;       // logical length of this array
//
//      int         d_size;         // physical capacity of this array
//
//      ALLOCATOR  *d_allocator_p;  // allocator or pool (held, not owned)
//
//    public:
//      // CREATORS
//      my_StrArray(ALLOCATOR *basicAllocator);
//          // Create a 'my_StrArray' object using the specified
//          // 'basicAllocator' used to supply memory.
//
//      ~my_StrArray();
//          // Destroy this 'my_StrArray' object and all elements currently
//          // stored.
//
//      // MANIPULATORS
//      void append(const char *src);
//          // Append to this array the string 'src'.
//
//      void insert(int dstIndex, const my_StrArray& srcArray);
//          // Insert into this array at the specified 'dstIndex', the
//          // character sequences in the specified 'srcArray'.  All values
//          // with initial indices at or above 'dstIndex' are shifted up by
//          // unless '0 <= dstIndex' and 'dstIndex <= length()'.  Note that
//          // this method is functionally the same as 'insert2', but has a
//          // different implementation to facilitate the usage example.
//
//      void insert2(int dstIndex, const my_StrArray& srcArray);
//          // Insert into this array at the specified 'dstIndex', the
//          // character sequences in the specified 'srcArray'.  All values
//          // with initial indices at or above 'dstIndex' are shifted up by
//          // unless '0 <= dstIndex' and 'dstIndex <= length()'.  Note that
//          // this method is functionally the same as 'insert', but has a
//          // different implementation to facilitate the usage example.
//
//      // ...
//
//      // ACCESSORS
//      int length() const;
//          // Return the logical length of this array.
//
//      const char *operator[](int index) const;
//          // Return a pointer to the 'index'th string in the array.  Note
//          // the pointer is returned by value, it is not a reference to a
//          // pointer.
//
//      // ...
//  };
//..
// Next, we define the 'insert' method of 'my_StrArray':
//..
//  template <class ALLOCATOR>
//  void my_StrArray<ALLOCATOR>::insert(int                           dstIndex,
//                                      const my_StrArray<ALLOCATOR>& srcArray)
//  {
//      int srcLength  = srcArray.d_length;
//      int newLength  = d_length + srcLength;
//      int numShifted = d_length - dstIndex;
//
//      if (newLength > d_size) {
//          while (d_size < newLength) {
//              d_size = ! d_size ? 1 : 2 * d_size;
//          }
//
//          char ** newArray =
//                  (char **) d_allocator_p->allocate(d_size * sizeof(char *));
//          memcpy(newArray, d_array_p, d_length * sizeof(char *));
//          if (d_array_p) {
//              d_allocator_p->deallocate(d_array_p);
//          }
//          d_array_p = newArray;
//      }
//
//      char **tmpSrc = srcArray.d_array_p;
//      if (this == &srcArray) {
//          // self-alias
//          size_t size = srcLength * sizeof(char *);
//          tmpSrc = (char **) d_allocator_p->allocate(size);
//          memcpy(tmpSrc, d_array_p, size);
//      }
//      bslma::DeallocatorProctor<ALLOCATOR> proctor(
//                                              this == &srcArray ? tmpSrc : 0,
//                                              d_allocator_p);
//
//      // First shift the elements to the back of the array.
//      memmove(d_array_p + dstIndex + srcLength,
//              d_array_p + dstIndex,
//              numShifted * sizeof *d_array_p);
//
//      // Shorten 'd_length' and use 'bslma::AutoDeallocator' to proctor the
//      // memory shifted.
//      d_length = dstIndex;
//
//      //*******************************************************
//      // Note use of auto deallocator on tail memory (below). *
//      //*******************************************************
//
//      bslma::AutoDeallocator<ALLOCATOR> tailDeallocator(
//                                  (void **) d_array_p + dstIndex + srcLength,
//                                  d_allocator_p,
//                                  numShifted);
//..
// Now, if any allocation for the inserted character sequences throws, the
// memory used for the character sequences that had been moved to the end of
// array will be deallocated automatically by the 'bslma::AutoDeallocator'.
//..
//      // Copy the character sequences from the 'srcArray'.
//      for (int i = 0; i < srcLength; ++i, ++d_length) {
//          std::size_t size = std::strlen(tmpSrc[i]) + 1;
//          d_array_p[dstIndex + i] = (char *) d_allocator_p->allocate(size);
//          memcpy(d_array_p[dstIndex + i], tmpSrc[i], size);
//      }
//
//      //*********************************************
//      // Note that the proctor is released (below). *
//      //*********************************************
//
//      tailDeallocator.release();
//      d_length = newLength;
//  }
//..
// The above method copies the source elements (visually) from left to right.
// Another (functionally equivalent) implementation copies the source elements
// from right to left, and makes use of the 'operator--()' of the
// 'bslma::AutoDeallocator' interface:
//..
//  template <class ALLOCATOR>
//  void my_StrArray<ALLOCATOR>::insert2(int                          dstIndex,
//                                      const my_StrArray<ALLOCATOR>& srcArray)
//  {
//      int srcLength  = srcArray.d_length;
//      int newLength  = d_length + srcLength;
//      int numShifted = d_length - dstIndex;
//
//      if (newLength > d_size) {
//          while (d_size < newLength) {
//              d_size = ! d_size ? 1 : 2 * d_size;
//          }
//
//          char ** newArray =
//                  (char **) d_allocator_p->allocate(d_size * sizeof(char *));
//          memcpy(newArray, d_array_p, d_length * sizeof(char *));
//          if (d_array_p) {
//              d_allocator_p->deallocate(d_array_p);
//          }
//          d_array_p = newArray;
//      }
//
//      char **tmpSrc = srcArray.d_array_p;
//      if (this == &srcArray) {
//          // self-alias
//          size_t size = srcLength * sizeof(char *);
//          tmpSrc = (char **) d_allocator_p->allocate(size);
//          memcpy(tmpSrc, d_array_p, size);
//      }
//      bslma::DeallocatorProctor<ALLOCATOR> proctor(
//                                              this == &srcArray ? tmpSrc : 0,
//                                              d_allocator_p);
//
//      // First shift the elements to the back of the array.
//      memmove(d_array_p + dstIndex + srcLength,
//              d_array_p + dstIndex,
//              numShifted * sizeof *d_array_p);
//
//      // Shorten 'd_length' and use 'bslma::AutoDeallocator' to proctor the
//      // memory shifted.
//      d_length = dstIndex;
//
//      //********************************************
//      //* Note the use of auto deallocator on tail *
//      //* memory with negative length (below).     *
//      //********************************************
//
//      bslma::AutoDeallocator<ALLOCATOR> tailDeallocator(
//                     (void **) d_array_p + d_length + srcLength + numShifted,
//                     d_allocator_p,
//                     -numShifted);
//..
// Since we have decided to copy the source elements from right to left, we set
// the origin of the 'bslma::AutoDeallocator' to the end of the array, and
// decrement the (signed) length on each copy to extend the proctor range by 1.
//..
//      // Copy the character sequences from the 'srcArray'.  Note that the
//      // 'tailDeallocator' has to be decremented to cover the newly
//      // created object.
//
//      for (int i = srcLength - 1; i >= 0; --i, --tailDeallocator) {
//          std::size_t size = std::strlen(tmpSrc[i]) + 1;
//          d_array_p[dstIndex + i] = (char *)d_allocator_p->allocate(size);
//          memcpy(d_array_p[dstIndex + i], tmpSrc[i], size);
//      }
//
//      //*********************************************
//      // Note that the proctor is released (below). *
//      //*********************************************
//
//      tailDeallocator.release();
//      d_length = newLength;
//  }
//..
// Note that though the two implementations are functionally equivalent, they
// are logically different.  First of all, the second implementation will be
// slightly slower because it is accessing memory backwards when compared to
// the normal forward sequential access.  Secondly, in case of an exception,
// the first implementation will retain all the elements copied prior to the
// exception, whereas the second implementation will remove them.

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

                        // =====================
                        // class AutoDeallocator
                        // =====================

template <class ALLOCATOR>
class AutoDeallocator {
    // This class implements a range proctor that, unless its 'release' method
    // has previously been invoked, automatically deallocates the contiguous
    // sequence of managed memory blocks upon its own destruction by invoking
    // the 'deallocate' method of an allocator (or pool) of parameterized
    // 'ALLOCATOR' type supplied to it at construction.  Each of the managed
    // memory blocks must have been supplied by this allocator (or pool), which
    // must remain valid throughout the lifetime of the range proctor.  Note
    // that when the length of this object is non-zero, it must refer to a
    // non-null array of memory blocks.

    // DATA
    void      **d_origin_p;     // reference location for the sequence of
                                // managed memory

    int         d_length;       // number of memory blocks managed (sign
                                // encodes direction)

    ALLOCATOR  *d_allocator_p;  // allocator or pool (held, not owned)

    // NOT IMPLEMENTED
    AutoDeallocator(const AutoDeallocator&);
    AutoDeallocator& operator=(const AutoDeallocator&);

  private:
    // PRIVATE MANIPULATORS
    void deallocate();
        // Deallocate the contiguous sequence of memory blocks managed by this
        // auto deallocator (if any) by invoking the 'deallocate' method of the
        // allocator (or pool) supplied at construction on each memory block.
        // Note that the order in which the managed memory blocks are
        // deallocated is undefined.  Also note that this method factors out
        // the deallocation logic, which allows the destructor to be declared
        // 'inline' for the common case (the range proctor is released before
        // being destroyed).

  public:
    // CREATORS
    template <class TYPE>
    AutoDeallocator(TYPE      **origin,
                    ALLOCATOR  *allocator,
                    int         length = 0);
    AutoDeallocator(void      **origin,
                    ALLOCATOR  *allocator,
                    int         length = 0);
        // Create an auto deallocator to manage a sequence of memory blocks at
        // the specified 'origin', and that uses the specified 'allocator' to
        // deallocate the sequence of memory blocks managed by this range
        // proctor (if not released -- see 'release') upon destruction.
        // Optionally specify 'length' to define its range, which by default is
        // empty (i.e., 'length = 0').  The sequence of memory blocks may
        // extend in either direction from 'origin'.  A positive 'length'
        // represents the sequence of memory blocks starting at 'origin' and
        // extending "up" to 'length' (*not* including the memory block at the
        // index position 'origin + length').  A negative 'length' represents
        // the sequence of memory blocks starting at one position below
        // 'origin' and extending "down" to the absolute value of 'length'
        // (including the memory block at index position 'origin + length').
        // If 'length' is 0, then this range proctor manages no memory blocks.
        // If 'origin' is non-zero, all memory blocks within the proctored
        // range (if any) must be supplied by 'allocator'.  The behavior is
        // undefined unless 'allocator' is non-zero, and, if 'origin' is 0,
        // 'length' is also 0.  Note that when 'length' is non-positive, the
        // memory block at the origin is *not* managed by this range proctor.
        // For example, if 'origin' is at the index position 2, a 'length' of 2
        // signifies that the memory blocks at positions 2 and 3 are managed,
        // whereas a 'length' of -2 signifies that the memory blocks at
        // positions 0 and 1 are managed:
        //..
        //     length = -2                            length = 2
        //     |<----->|                              |<----->|
        //      ___ ___ ___ ___ ___            ___ ___ ___ ___ ___
        //     | 0 | 1 | 2 | 3 | 4 |          | 0 | 1 | 2 | 3 | 4 |
        //     `===^===^===^===^==='          `===^===^===^===^==='
        //             ^------------ origin           ^------------ origin
        //..

    ~AutoDeallocator();
        // Destroy this range proctor and deallocate the contiguous sequence of
        // memory blocks it manages (if any) by invoking the 'deallocate'
        // method of the allocator (or pool) supplied at construction on each
        // memory block.  Note that the order in which the managed memory
        // blocks are deallocated is undefined.

    // MANIPULATORS
    void operator++();
        // Increase by one the (signed) length of the sequence of memory blocks
        // managed by this range proctor.  The behavior is undefined unless the
        // origin of the sequence of memory blocks managed by this proctor is
        // non-zero.  The behavior is undefined unless the origin or this range
        // proctor is non-zero.  Note that if the length of this proctor is
        // currently negative, the number of managed memory blocks will
        // decrease by one, whereas if the length is non-negative, the number
        // of managed memory blocks will increase by one.

    void operator--();
        // Decrease by one the (signed) length of the sequence of memory blocks
        // managed by this range proctor.  The behavior is undefined unless the
        // origin of the sequence of memory blocks managed by this proctor is
        // non-zero.  The behavior is undefined unless the origin or this range
        // proctor is non-zero.  Note that if the length of this proctor is
        // currently positive, the number of managed memory blocks will
        // decrease by one, whereas if the length is non-positive, the number
        // of managed memory blocks will increase by one.

    void release();
        // Release from management the sequence of memory blocks currently
        // managed by this range proctor by setting the length of the managed
        // sequence to 0.  All memory blocks currently under management will
        // become unmanaged (i.e., when the proctor goes out of scope and it
        // was not assigned another sequence of memory blocks to manage by
        // invoking 'reset', no memory blocks will be deallocated).  If no
        // memory blocks are currently being managed, this method has no
        // effect.  Note that the origin is not affected.

    template <class TYPE>
    void reset(TYPE **origin);
    void reset(void **origin);
        // Set the specified 'origin' as the origin of the sequence of memory
        // blocks to be managed by this range proctor.  The behavior is
        // undefined unless 'origin' is non-zero.  Note that the length of the
        // sequence of memory blocks managed by this proctor is not affected,
        // and 'setLength' should be invoked if the managed range is different
        // from the previously managed sequence of memory blocks.  Also note
        // that this method releases any previously-managed memory blocks from
        // management (without deallocating them), and so may be called with or
        // without having called 'release' when reusing this object.

    void setLength(int length);
        // Set the (signed) length of the sequence of memory blocks managed by
        // this range proctor to the specified 'length'.  The behavior is
        // undefined unless the origin of this range proctor is non-zero.

    // ACCESSORS
    int length() const;
        // Return the (signed) length of the sequence of memory blocks managed
        // by this proctor.
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                        // ---------------------
                        // class AutoDeallocator
                        // ---------------------

// PRIVATE MANIPULATORS
template <class ALLOCATOR>
void AutoDeallocator<ALLOCATOR>::deallocate()
{
    if (d_length > 0) {
        for (; d_length > 0; --d_length, ++d_origin_p) {
            d_allocator_p->deallocate(*d_origin_p);
        }
    }
    else {
        --d_origin_p;
        for (; d_length < 0; ++d_length, --d_origin_p) {
            d_allocator_p->deallocate(*d_origin_p);
        }
    }
}

// CREATORS
template <class ALLOCATOR>
template <class TYPE>
inline
AutoDeallocator<ALLOCATOR>
::AutoDeallocator(TYPE      **origin,
                  ALLOCATOR  *allocator,
                  int         length)
: d_origin_p((void **)origin)
, d_length(length)
, d_allocator_p(allocator)
{
    BSLS_ASSERT_SAFE(allocator);
    BSLS_ASSERT_SAFE(origin || !length);
}

template <class ALLOCATOR>
inline
AutoDeallocator<ALLOCATOR>
::AutoDeallocator(void      **origin,
                  ALLOCATOR  *allocator,
                  int         length)
: d_origin_p(origin)
, d_length(length)
, d_allocator_p(allocator)
{
    BSLS_ASSERT_SAFE(allocator);
    BSLS_ASSERT_SAFE(origin || !length);
}

template <class ALLOCATOR>
inline
AutoDeallocator<ALLOCATOR>::~AutoDeallocator()
{
    BSLS_ASSERT_SAFE(d_origin_p || !d_length);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_length)) {
        deallocate();
    }
}

// MANIPULATORS
template <class ALLOCATOR>
inline
void AutoDeallocator<ALLOCATOR>::operator++()
{
    BSLS_ASSERT_SAFE(d_origin_p);

    ++d_length;
}

template <class ALLOCATOR>
inline
void AutoDeallocator<ALLOCATOR>::operator--()
{
    BSLS_ASSERT_SAFE(d_origin_p);

    --d_length;
}

template <class ALLOCATOR>
inline
void AutoDeallocator<ALLOCATOR>::release()
{
    d_length = 0;
}

template <class ALLOCATOR>
template <class TYPE>
inline
void AutoDeallocator<ALLOCATOR>::reset(TYPE **origin)
{
    BSLS_ASSERT_SAFE(origin);

    d_origin_p = static_cast<void **>(origin);
}

template <class ALLOCATOR>
inline
void AutoDeallocator<ALLOCATOR>::reset(void **origin)
{
    BSLS_ASSERT_SAFE(origin);

    d_origin_p = origin;
}

template <class ALLOCATOR>
inline
void
AutoDeallocator<ALLOCATOR>::setLength(int length)
{
    BSLS_ASSERT_SAFE(d_origin_p);

    d_length = length;
}

// ACCESSORS
template <class ALLOCATOR>
inline
int AutoDeallocator<ALLOCATOR>::length() const
{
    return d_length;
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
