// bslalg_arrayprimitives.h                                           -*-C++-*-
#ifndef INCLUDED_BSLALG_ARRAYPRIMITIVES
#define INCLUDED_BSLALG_ARRAYPRIMITIVES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide primitive algorithms that operate on arrays.
//
//@CLASSES:
//  bslalg::ArrayPrimitives: namespace for array algorithms
//
//@SEE_ALSO: bslalg_scalarprimitives, bslalg_typetraits
//
//@AUTHOR: Pablo Halpern (phalpern), Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides utilies to initialize, move, and
// otherwise perform various primitive manipulations on arrays with a uniform
// interface, but selecting a different implementation according to the various
// traits possessed by the underlying type.  Such primitives are exceptionally
// useful for implementing generic components such as containers.
//
// Several algorithms are provided, with the following short synopsis
// describing the observable behavior and mentioning the relevant traits.  See
// the full function-level contract for detailed description, including
// exception-safety guarantees.  In the description below, 'Sc' stands for
// 'bslalg::ScalarPrimitives' and 'ADP' stands for
// 'bslalg::ArrayDestructionPrimitives' (for brevity).  Note that some
// algorithms (e.g., 'insert') are explained in terms of previous algorithms
// (e.g., 'destructiveMove').
//..
//  Algorithm                     Short description of observable behavior
//  ----------------------------  ---------------------------------------------
//  defaultConstruct              'Sc::defaultConstruct' from value for each
//                                element in the target range, or 'std::memset'
//                                if type has a trivial default constructor
//
//  uninitializedFillN            'Sc::copyConstruct' from value for each
//                                element in the target range, or 'std::memset'
//                                if value is all 0s or 1s bits, and type is
//                                bit-wise copyable
//
//  copyConstruct                 'Sc::copyConstruct' from each element in the
//                                original range to the corresponding element
//                                in the target range, or 'std::memcpy' if
//                                value is null and type is bit-wise copyable
//
//  destructiveMove               'Sc::destructiveMove' from each element in
//                                the original range to the corresponding
//                                element in the target range, or 'std::memcpy'
//                                if type is bit-wise moveable
//
//  destructiveMoveAndInsert      'Sc::destructiveMove' from each element in
//                                the original range to the corresponding
//                                element in the target range, leaving a hole
//                                in the middle, followed by
//                                'uninitializedFillN' or 'copyConstruct' to
//                                fill hole with the appropriate values
//
//  destructiveMoveAndMoveInsert  'destructiveMove' from the original range to
//                                the target range, leaving a hole in the
//                                middle, followed by 'destructiveMove'
//                                from second range to fill hole
//
//  insert                        'destructiveMove' by some positive offset to
//                                create a hole, followed by
//                                'uninitializedFillN' or 'copyConstruct' to
//                                fill hole with the appropriate values
//
//  moveInsert                    'destructiveMove' by some positive offset to
//                                create a hole, followed by 'destructiveMove'
//                                to fill hole with the appropriate values
//
//  erase                         'ADP::destroy' elements in target range until
//                                specified position, followed by
//                                'destructiveMove' by some negative offset
//                                from the end of the range to fill hole with
//                                the remaining values
//
//  rotate                        'Sc::destructiveMove' to move elements into a
//                                shifting hole along parallel cyclic
//                                permutations, or 'std::memmove' for small
//                                rotations if type is bit-wise moveable
//..
// The traits under consideration by this component are:
//..
//  Trait                                         English description
//  --------------------------------------------  -----------------------------
//  bsl::is_trivially_default_constructible       "TYPE has the trivial default
//                                                constructor trait", or
//                                                "TYPE has a trivial default
//                                                constructor"
//
//  bsl::is_trivially_copyable                    "TYPE has the bit-wise
//                                                copyable trait", or
//                                                "TYPE is bit-wise copyable"
//
//  bslmf::IsBitwiseMoveable                      "TYPE has the bit-wise
//                                                moveable trait", or
//                                                "TYPE is bit-wise moveable"
//..
///Aliasing
///--------
// There are some aliasing concerns in this component, due to the presence of
// the reference 'const TARGET_TYPE& value' argument, which may belong to a
// range that will be modified during the course of the operation.  All such
// aliasing concerns are taken care of properly.  Other aliasing concerns due
// to the copying or a range '[first, last)' are *not* taken care of, since
// their intended use is for range assignments and insertions in standard
// containers, for which the standard explicitly says that 'first' and 'last'
// shall not be iterators into the container.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Defining a Vector-Like Type
/// - - - - - - - - - - - - - - - - - - -
// Suppose we want to define a STL-vector-like type.  One requirement is that
// an object of this vector should forward its allocator to its contained
// elements when appropriate.  Another requirement is that the vector should
// take advantage of the optimizations available for certain traits of the
// contained element type.  For example, if the contained element type has the
// 'bslalg::TypeTraitBitwiseMoveable' trait, moving an element in a vector can
// be done using 'memcpy' instead of copy construction.
//
// We can utilize the class methods provided by 'bslalg::ArrayPrimitives' to
// satisfy the above requirements.  Unlike 'bslalg::ScalarPrimitives', which
// operates on a single element, 'bslalg::ArrayPrimitives' operates on arrays,
// which will further help simplify our implementation.
//
// First, we create an elided definition of the class template 'MyVector':
//..
//  template <class TYPE>
//  class MyVector {
//      // This class implements a vector of elements of the (template
//      // parameter) 'TYPE', which must be copy constructable.  Note that for
//      // the brevity of the usage example, this class does not provide any
//      // Exception-Safety guarantee.
//
//      // DATA
//      TYPE             *d_array_p;      // pointer to the allocated array
//
//      int               d_capacity;     // capacity of the allocated array
//
//      int               d_size;         // number of objects
//
//      bslma::Allocator *d_allocator_p;  // allocation pointer (held, not
//                                        // owned)
//
//    public:
//      // ...
//
//      MyVector(const MyVector&   original,
//               bslma::Allocator *basicAllocator = 0);
//          // Create a 'MyVector' object having the same value as the
//          // specified 'original' object.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      // ...
//
//      // MANIPULATORS
//      void reserve(int capacity);
//          // Change the capacity of this vector to the specified 'capacity'
//          // if it is greater than the vector's current capacity.
//
//      void insert(int dstIndex, int numElements, const TYPE& value);
//          // Insert, into this vector, the specified 'numElements' of the
//          // specified 'value' at the specified 'dstIndex'.  The behavior is
//          // undefined unless '0 <= dstIndex <= size()'.
//  };
//..
// Then, we implement the copy constructor of 'MyVector':
//..
//  template <class TYPE>
//  MyVector<TYPE>::MyVector(const MyVector<TYPE>&  original,
//                           bslma::Allocator      *basicAllocator)
//  : d_array_p(0)
//  , d_capacity(0)
//  , d_size(0)
//  , d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//      reserve(d_size);
//..
// Here, we call the 'bslalg::ArrayPrimitives::copyConstruct' class method to
// copy each element from 'original.d_array_p' to 'd_array_p' (When
// appropriate, this class method passes this vector's allocator to the copy
// constructor of 'TYPE' or uses bit-wise copy.):
//..
//      bslalg::ArrayPrimitives::copyConstruct(
//                                        d_array_p,
//                                        original.d_array_p,
//                                        original.d_array_p + original.d_size,
//                                        d_allocator_p);
//      d_size = original.d_size;
//  }
//..
// Now, we implement the 'reserve' method of 'MyVector':
//..
//  template <class TYPE>
//  void MyVector<TYPE>::reserve(int capacity)
//  {
//      if (d_capacity >= capacity) return;
//
//      TYPE *newArrayPtr = static_cast<TYPE*>(d_allocator_p->allocate(
//         BloombergLP::bslma::Allocator::size_type(capacity * sizeof(TYPE))));
//
//      if (d_array_p) {
//..
// Here, we call the 'bslalg::ArrayPrimitives::destructiveMove' class method to
// copy each original element from 'd_array_p' to 'newArrayPtr' and then
// destroy all the original elements (When appropriate, this class method
// passes this vector's allocator to the copy constructor of 'TYPE' or uses
// bit-wise copy.):
//..
//          bslalg::ArrayPrimitives::destructiveMove(newArrayPtr,
//                                                   d_array_p,
//                                                   d_array_p + d_size,
//                                                   d_allocator_p);
//          d_allocator_p->deallocate(d_array_p);
//      }
//
//      d_array_p = newArrayPtr;
//      d_capacity = capacity;
//  }
//..
// Finally, we implement the 'insert' method of 'MyVector':
//..
//  template <class TYPE>
//  void MyVector<TYPE>::insert(int         dstIndex,
//                              int         numElements,
//                              const TYPE& value)
//  {
//      int newSize = d_size + numElements;
//
//      if (newSize > d_capacity) {
//          int newCapacity = d_capacity == 0 ? 2 : d_capacity * 2;
//          reserve(newCapacity);
//      }
//..
// Here, we call the 'bslalg::ArrayPrimitives::insert' class method to first
// move each element after 'dstIndex' by 'numElements' and then copy construct
// 'numElements' of 'value' at 'dstIndex'.  (When appropriate, this class
// method passes this vector's allocator to the copy constructor of 'TYPE' or
// uses bit-wise copy.):
//..
//      bslalg::ArrayPrimitives::insert(d_array_p + dstIndex,
//                                      d_array_p + d_size,
//                                      value,
//                                      numElements,
//                                      d_allocator_p);
//
//      d_size = newSize;
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLALG_ARRAYDESTRUCTIONPRIMITIVES
#include <bslalg_arraydestructionprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_AUTOARRAYDESTRUCTOR
#include <bslalg_autoarraydestructor.h>
#endif

#ifndef INCLUDED_BSLALG_AUTOARRAYMOVEDESTRUCTOR
#include <bslalg_autoarraymovedestructor.h>
#endif

#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#include <bslalg_constructorproxy.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARDESTRUCTIONPRIMITIVES
#include <bslalg_scalardestructionprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMF_ANYTYPE
#include <bslmf_anytype.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS
#include <bslmf_functionpointertraits.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE
#include <bslmf_istriviallydefaultconstructible.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // std::size_t
#define INCLUDED_CSTDDEF
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>  // memset, memcpy, memmove
#define INCLUDED_CSTRING
#endif

#ifndef INCLUDED_CWCHAR
#include <cwchar>  // wmemset
#define INCLUDED_CWCHAR
#endif

namespace BloombergLP {

namespace bslalg {

struct ArrayPrimitives_Imp;

                        // ======================
                        // struct ArrayPrimitives
                        // ======================

struct ArrayPrimitives {
    // This 'struct' provides a namespace for a suite of independent utility
    // functions that operate on arrays of elements of parameterized type
    // 'TARGET_TYPE'.  Depending on the traits of 'TARGET_TYPE', the default
    // and copy constructors, destructor, assignment operators, etc. may not be
    // invoked, optimized away by no-op or bit-wise move or copy.

    // TYPES
    typedef ArrayPrimitives_Imp         Imp;
    typedef std::size_t                 size_type;
    typedef std::ptrdiff_t              difference_type;

    // CLASS METHODS
    template <class TARGET_TYPE, class ALLOCATOR>
    static void defaultConstruct(TARGET_TYPE *begin,
                                 size_type    numElements,
                                 ALLOCATOR   *allocator);
        // Call the default constructor on each of the elements of an array of
        // the specified 'numElements' of the parameterized 'TARGET_TYPE'
        // starting at the 'toBegin' address.  If the parameterized 'ALLOCATOR'
        // is derived from 'bslma::Allocator' and 'TARGET_TYPE' supports
        // 'bslma' allocators, then the specified 'allocator' is passed to each
        // 'TARGET_TYPE' default constructor call.  The behavior is undefined
        // unless 'begin <= end'.  If a 'TARGET_TYPE' constructor throws an
        // exception during the operation, then the destructor is called on any
        // newly-constructed elements, leaving the input array in an
        // uninitialized state.

    template <class TARGET_TYPE, class ALLOCATOR>
    static void uninitializedFillN(TARGET_TYPE        *begin,
                                   size_type           numElements,
                                   const TARGET_TYPE&  value,
                                   ALLOCATOR          *allocator);
        // Construct copies of the specified 'value' of the parameterized type
        // 'TARGET_TYPE' into the uninitialized array containing the specified
        // 'numElements' starting at the specified 'begin' address.  If the
        // parameterized 'ALLOCATOR' is derived from 'bslma::Allocator' and
        // 'TARGET_TYPE' supports 'bslma' allocators, then the specified
        // 'allocator' is passed to each invocation of the 'TARGET_TYPE' copy
        // constructor.  If a 'TARGET_TYPE' constructor throws an exception
        // during the operation, then the destructor is called on any
        // newly-constructed elements, leaving the input array in an
        // uninitialized state.  Note that the argument order was chosen to
        // maintain compatibility with the existing 'bslalg'.

    template <class TARGET_TYPE, class ALLOCATOR, class FWD_ITER>
    static void copyConstruct(TARGET_TYPE *toBegin,
                              FWD_ITER     fromBegin,
                              FWD_ITER     fromEnd,
                              ALLOCATOR   *allocator);
        // Copy into an uninitialized array of the parameterized 'TARGET_TYPE'
        // beginning at the specified 'toBegin' address, the elements in the
        // array of 'TARGET_TYPE' starting at the specified 'fromBegin' address
        // and ending immediately before the specified 'fromEnd' address.  If
        // the parameterized 'ALLOCATOR' is derived from 'bslma::Allocator' and
        // 'TARGET_TYPE' supports 'bslma' allocators, then the specified
        // 'allocator' is passed to each invocation of the 'TARGET_TYPE' copy
        // constructor.  If a 'TARGET_TYPE' constructor throws an exception
        // during the operation, then the destructor is called on any
        // newly-constructed elements, leaving the input array in an
        // uninitialized state.

    template <class TARGET_TYPE, class ALLOCATOR>
    static void destructiveMove(TARGET_TYPE *toBegin,
                                TARGET_TYPE *fromBegin,
                                TARGET_TYPE *fromEnd,
                                ALLOCATOR   *allocator);
        // Move the elements of the parameterized 'TARGET_TYPE' in the array
        // starting at the specified 'fromBegin' address and ending immediately
        // before the specified 'fromEnd' address into an uninitialized array
        // of 'TARGET_TYPE' beginning at the specified 'toBegin' address.  On
        // return, the elements in the input range are invalid, i.e., their
        // destructors must not be called after this operation returns.  If the
        // parameterized 'ALLOCATOR' type is derived from 'bslma::Allocator'
        // and 'TARGET_TYPE' supports 'bslma' allocators, then the specified
        // 'allocator' is used by the objects in their new location.  If an
        // exception is thrown by a 'TARGET_TYPE' constructor during the
        // operation, then the output array is left in an uninitialized state
        // and the input elements remain in their original state.

    template <class TARGET_TYPE, class ALLOCATOR>
    static void destructiveMoveAndInsert(TARGET_TYPE         *toBegin,
                                         TARGET_TYPE        **fromEndPtr,
                                         TARGET_TYPE         *fromBegin,
                                         TARGET_TYPE         *position,
                                         TARGET_TYPE         *fromEnd,
                                         const TARGET_TYPE&   value,
                                         size_type            numElements,
                                         ALLOCATOR           *allocator);
        // Move the elements of the parameterized 'TARGET_TYPE' in the array
        // starting at the specified 'fromBegin' address and ending immediately
        // before the specified 'fromEnd' address into an uninitialized array
        // of 'TARGET_TYPE' at the specified 'toBegin' address, inserting at
        // the specified 'position' (after translating from 'fromBegin' to
        // 'toBegin') the specified 'numElements' copies of the specified
        // 'value'.  Keep the pointer at the specified 'fromEndptr' address
        // pointing to the first uninitialized element in
        // '[ fromBegin, fromEnd)' as the elements are moved from source to
        // destination.  The behavior is undefined unless
        // 'fromBegin <= position <= fromEnd' and the destination array
        // contains at least '(fromEnd - fromBegin) + numElements'
        // uninitialized elements.  If a copy constructor or assignment
        // operator for 'TARGET_TYPE' throws an exception, then any elements
        // created in the output array are destroyed and the elements in the
        // range '[ fromBegin, *fromEndPtr )' will have unspecified but valid
        // values.

    template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
    static void destructiveMoveAndInsert(TARGET_TYPE  *toBegin,
                                         TARGET_TYPE **fromEndPtr,
                                         TARGET_TYPE  *fromBegin,
                                         TARGET_TYPE  *position,
                                         TARGET_TYPE  *fromEnd,
                                         FWD_ITER      first,
                                         FWD_ITER      last,
                                         size_type     numElements,
                                         ALLOCATOR    *allocator);
        // Move the elements of the parameterized 'TARGET_TYPE' in the array
        // starting at the specified 'fromBegin' address and ending immediately
        // before the specified 'fromEnd' address into an uninitialized array
        // of 'TARGET_TYPE' at the specified 'toBegin' address, inserting at
        // the specified 'position' (after translating from 'fromBegin' to
        // 'toBegin') the specified 'numElements' copies of the non-modifiable
        // elements from the range starting at the specified 'first' iterator
        // of the parameterized 'FWD_ITER' type and ending immediately before
        // the specified 'last' iterator.  Keep the pointer at the specified
        // 'fromEndptr' to point to the first uninitialized element in
        // '[fromBegin, fromEnd)' as the elements are moved from source to
        // destination.  The behavior is undefined unless
        // 'fromBegin <= position <= fromEnd' the destination array contains at
        // least '(fromEnd - fromBegin) + numElements' uninitialized elements
        // after 'toBegin', and 'numElements' is the distance from 'first' to
        // 'last'.  If a copy constructor or assignment operator for
        // 'TARGET_TYPE' throws an exception, then any elements created in the
        // output array are destroyed and the elements in the range
        // '[ fromBegin, *fromEndPtr )' will have unspecified but valid values.

    template <class TARGET_TYPE, class ALLOCATOR>
    static void destructiveMoveAndMoveInsert(TARGET_TYPE  *toBegin,
                                             TARGET_TYPE **fromEndPtr,
                                             TARGET_TYPE **lastPtr,
                                             TARGET_TYPE  *fromBegin,
                                             TARGET_TYPE  *position,
                                             TARGET_TYPE  *fromEnd,
                                             TARGET_TYPE  *first,
                                             TARGET_TYPE  *last,
                                             size_type     numElements,
                                             ALLOCATOR    *allocator);
        // Move the elements of the parameterized 'TARGET_TYPE' in the array
        // starting at the 'fromBegin' address and ending immediately before
        // the 'fromEnd' address into an uninitialized array of 'TARGET_TYPE'
        // at the specified 'toBegin' address, moving into the specified
        // 'position' (after translating from 'fromBegin' to 'toBegin') the
        // specified 'numElements' of the 'TARGET_TYPE' from the array starting
        // at the specified 'first' address and ending immediately before the
        // specified 'last' address.  Keep the pointer at the specified
        // 'fromEndptr' address pointing to the first uninitialized element in
        // '[fromBegin, fromEnd)', and the pointer at the specified 'lastPtr'
        // address pointing to the end of the moved range as the elements from
        // the range '[ first, last)' are moved from source to destination.
        // The behavior is undefined unless 'fromBegin <= position <= fromEnd',
        // the destination array contains at least
        // '(fromEnd - fromBegin) + numElements' uninitialized elements after
        // 'toBegin', and 'numElements' is the distance from 'first' to 'last'.
        // If a copy constructor or assignment operator for 'TARGET_TYPE'
        // throws an exception, then any elements in '[ *lastPtr, last )' as
        // well as in '[ toBegin, ... )' are destroyed, and the elements in the
        // ranges '[ first, *lastPtr )' and '[ fromBegin, *fromEndPtr )' will
        // have unspecified but valid values.

    template <class TARGET_TYPE, class ALLOCATOR>
    static void insert(TARGET_TYPE        *toBegin,
                       TARGET_TYPE        *toEnd,
                       const TARGET_TYPE&  value,
                       size_type           numElements,
                       ALLOCATOR          *allocator);
        // Insert the specified 'numElements' copies of the specified 'value'
        // into the array of the parameterized 'TARGET_TYPE' starting at the
        // 'toBegin' address and ending immediately before the specified
        // 'toEnd' address, shifting the elements in the array by 'numElements'
        // positions towards larger addresses.  The behavior is undefined
        // unless the destination array contains at least 'numElements'
        // uninitialized elements after 'toEnd'.  If a copy constructor or
        // assignment operator for 'TARGET_TYPE' throws an exception, then any
        // elements created after 'toEnd' are destroyed and the elements in the
        // range '[ toBegin, toEnd )' will have unspecified, but valid, values.

    template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
    static void insert(TARGET_TYPE *toBegin,
                       TARGET_TYPE *toEnd,
                       FWD_ITER     fromBegin,
                       FWD_ITER     fromEnd,
                       size_type    numElements,
                       ALLOCATOR   *allocator);
        // Insert the specified 'numElements' from the range starting at the
        // 'fromBegin' and ending immediately before the 'fromEnd' iterators of
        // the parameterized 'FWD_ITER' type, into the array of elements of the
        // parameterized 'TARGET_TYPE' starting at the 'toBegin' address and
        // ending immediately before the 'toEnd' address, shifting the elements
        // in the array by the specified 'numElements' positions towards larger
        // addresses.  The behavior is undefined unless the destination array
        // contains 'numElements' uninitialized elements after 'toEnd',
        // 'numElements' is the distance between 'fromBegin' and 'fromEnd', and
        // the input array and the destination array do not overlap.  If a copy
        // constructor or assignment operator for 'TARGET_TYPE' throws an
        // exception, then any elements created after 'toEnd' are destroyed and
        // the elements in the range '[ toBegin, toEnd )' will have
        // unspecified, but valid, values.

    template <class TARGET_TYPE, class ALLOCATOR>
    static void moveInsert(TARGET_TYPE  *toBegin,
                           TARGET_TYPE  *toEnd,
                           TARGET_TYPE **fromEndPtr,
                           TARGET_TYPE  *fromBegin,
                           TARGET_TYPE  *fromEnd,
                           size_type     numElements,
                           ALLOCATOR    *allocator);
        // Move the elements of the parameterized 'TARGET_TYPE' in the array
        // starting at the 'toBegin' address and ending immediately before the
        // 'toEnd' address by the specified 'numElements' positions towards
        // larger addresses, and fill the specified 'numElements' at the
        // 'toBegin' address by moving the elements from the array starting at
        // the 'fromBegin' and ending immediately before the 'fromEnd' address.
        // Keep the iterator at the specified 'fromEndPtr' address pointing to
        // the end of the range as the elements from '[ fromBegin, fromEnd )'
        // are moved from source to destination.  The behavior is undefined
        // unless the destination array contains 'numElements' uninitialized
        // elements after 'toEnd', 'numElements' is the distance from
        // 'fromBegin' to 'fromEnd', and the input and destination arrays do
        // not overlap.  If a copy constructor or assignment operator for
        // 'TARGET_TYPE' throws an exception, then any elements created after
        // 'toEnd' are destroyed, the elements in the ranges
        // '[ toBegin, toEnd)' and '[ fromBegin, *fromEndPtr )' will have
        // unspecified, but valid, values, and the elements in
        // '[ *fromEndPtr, fromEnd )' will be destroyed.

    template <class TARGET_TYPE, class ALLOCATOR>
    static void erase(TARGET_TYPE *first,
                      TARGET_TYPE *middle,
                      TARGET_TYPE *last,
                      ALLOCATOR   *allocator);
        // Destroy the elements of the parameterized 'TARGET_TYPE' in the array
        // starting at the specified 'first' address and ending immediately
        // before the specified 'middle' address, and move the elements in the
        // array starting at 'middle' and ending at the specified 'last'
        // address down to the 'first' address.  If an assignment throws an
        // exception during this process, all of the elements in the range
        // '[ first, last )' will have unspecified but valid values, and no
        // elements are destroyed.  The behavior is undefined unless
        // 'first <= middle <= last'.

    template <class TARGET_TYPE>
    static void rotate(TARGET_TYPE *first,
                       TARGET_TYPE *middle,
                       TARGET_TYPE *last);
        // Move the elements of the parameterized 'TARGET_TYPE' in the array
        // starting at the specified 'first' address and ending immediately
        // before the specified 'middle' address to the array of the same
        // length ending at the specified 'last' address (and thus starting at
        // the 'last - (middle - first)' address), and move the elements
        // previously in the array starting at 'middle' and ending at 'last'
        // down to the 'first' address.  If the assignment operator throws an
        // exception during this process, all of the elements in
        // '[ first, last )' will have unspecified, but valid, values.  The
        // behavior is undefined unless 'first <= middle <= last'.
};

                     // ==========================
                     // struct ArrayPrimitives_Imp
                     // ==========================

struct ArrayPrimitives_Imp {
    // This 'struct' provides a namespace for a suite of independent utility
    // functions that operate on arrays of elements of a parameterized
    // 'TARGET_TYPE'.  These utility functions are only for the purpose of
    // implementing those in the 'ArrayPrimitives' utility.  For brevity, we do
    // not repeat the main contracts here, but instead refer to the
    // corresponding contract in the 'ArrayPrimitive' utility.

    // TYPES
    typedef ArrayPrimitives::size_type       size_type;
    typedef ArrayPrimitives::difference_type difference_type;

    enum {
        // These constants are used in the overloads below, when the last
        // argument is of type 'bslmf::MetaInt<N> *', indicating that
        // 'TARGET_TYPE' has the traits for which the enumerator equal to 'N'
        // is named.

        IS_POINTER_TO_POINTER           = 5,
        IS_FUNDAMENTAL_OR_POINTER       = 4,
        HAS_TRIVIAL_DEFAULT_CTOR_TRAITS = 3,
        BITWISE_COPYABLE_TRAITS         = 2,
        BITWISE_MOVEABLE_TRAITS         = 1,
        NIL_TRAITS                      = 0
    };

    enum {
        // Number of bytes for which a stack-allocated buffer can be
        // comfortably obtained to optimize bitwise moves.

        INPLACE_BUFFER_SIZE = 16 * bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT
    };

    // CLASS METHODS
    template <class TARGET_TYPE, class ALLOCATOR>
    static void defaultConstruct(
                  TARGET_TYPE                                     *begin,
                  size_type                                        numElements,
                  ALLOCATOR                                       *allocator,
                  bslmf::MetaInt<HAS_TRIVIAL_DEFAULT_CTOR_TRAITS> *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void defaultConstruct(
                          TARGET_TYPE                             *begin,
                          size_type                                numElements,
                          ALLOCATOR                               *allocator,
                          bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void defaultConstruct(TARGET_TYPE                *begin,
                                 size_type                   numElements,
                                 ALLOCATOR                  *allocator,
                                 bslmf::MetaInt<NIL_TRAITS> *);
        // Use the default constructor of the parameterized 'TARGET_TYPE' (or
        // 'memset' to 0 if 'TARGET_TYPE' has a trivial default constructor) on
        // each element of the array starting at the 'begin' address and ending
        // immediately before the 'end' address.  Pass the specified
        // 'allocator' to the default constructor if appropriate.  The last
        // argument is for traits overloading resolution only and its value is
        // ignored.

    static void bitwiseFillN(char      *begin,
                             size_type  numBytesInitialized,
                             size_type  numBytes);
        // Fill the specified 'numBytes' in the array starting at the specified
        // 'begin' address, as if by bit-wise copying the specified
        // 'numBytesInitialized' at every offset that is a multiple of
        // 'numBytesInitialized' within the output array.  The behavior is
        // undefined unless 'numBytesInitialized <= numBytes'.  Note that
        // 'numBytes' usually is, but does not have to be, a multiple of
        // 'numBytesInitialized'.

    static void uninitializedFillN(
                        bool                                      *begin,
                        bool                                       value,
                        size_type                                  numElements,
                        void                                      * = 0,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                        char                                      *begin,
                        char                                       value,
                        size_type                                  numElements,
                        void                                      * = 0,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                        unsigned char                             *begin,
                        unsigned char                              value,
                        size_type                                  numElements,
                        void                                      * = 0,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                        signed char                               *begin,
                        signed char                                value,
                        size_type                                  numElements,
                        void                                      * = 0,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                        wchar_t                                   *begin,
                        wchar_t                                    value,
                        size_type                                  numElements,
                        void                                      * = 0,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                        short                                     *begin,
                        short                                      value,
                        size_type                                  numElements,
                        void                                      * = 0,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                        unsigned short                            *begin,
                        unsigned short                             value,
                        size_type                                  numElements,
                        void                                      * = 0,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                        int                                       *begin,
                        int                                        value,
                        size_type                                  numElements,
                        void                                      * = 0,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                        unsigned int                              *begin,
                        unsigned int                               value,
                        size_type                                  numElements,
                        void                                      * = 0,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                        long                                      *begin,
                        long                                       value,
                        size_type                                  numElements,
                        void                                      * = 0,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                        unsigned long                             *begin,
                        unsigned long                              value,
                        size_type                                  numElements,
                        void                                      * = 0,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                        bsls::Types::Int64                        *begin,
                        bsls::Types::Int64                         value,
                        size_type                                  numElements,
                        void                                      * = 0,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                        bsls::Types::Uint64                       *begin,
                        bsls::Types::Uint64                        value,
                        size_type                                  numElements,
                        void                                      * = 0,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                        float                                     *begin,
                        float                                      value,
                        size_type                                  numElements,
                        void                                      * = 0,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                        double                                    *begin,
                        double                                     value,
                        size_type                                  numElements,
                        void                                      * = 0,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                        long double                               *begin,
                        long double                                value,
                        size_type                                  numElements,
                        void                                      * = 0,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                       void                                      **begin,
                       void                                       *value,
                       size_type                                   numElements,
                       void                                       * = 0,
                       bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER>  * = 0);
    static void uninitializedFillN(
                       const void                                **begin,
                       const void                                 *value,
                       size_type                                   numElements,
                       void                                       * = 0,
                       bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER>  * = 0);
    template <class TARGET_TYPE>
    static void uninitializedFillN(
                       TARGET_TYPE                               **begin,
                       TARGET_TYPE                                *value,
                       size_type                                   numElements,
                       void                                       * = 0,
                       bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER>  * = 0);
    template <class TARGET_TYPE>
    static void uninitializedFillN(
                       const TARGET_TYPE                         **begin,
                       const TARGET_TYPE                          *value,
                       size_type                                   numElements,
                       void                                       * = 0,
                       bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER>  * = 0);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void uninitializedFillN(
                          TARGET_TYPE                             *begin,
                          const TARGET_TYPE&                       value,
                          size_type                                numElements,
                          ALLOCATOR                               *allocator,
                          bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void uninitializedFillN(TARGET_TYPE                *begin,
                                   const TARGET_TYPE&          value,
                                   size_type                   numElements,
                                   ALLOCATOR                  *allocator,
                                   bslmf::MetaInt<NIL_TRAITS> *);
        // Copy the specified 'value' of the parameterized 'TARGET_TYPE' into
        // every of the specified 'numElements' in the array starting at the
        // specified 'begin' address.  Pass the specified 'allocator' to the
        // copy constructor if appropriate.  Note that if 'TARGET_TYPE' is
        // bit-wise copyable or is not based on 'bslma::Allocator', 'allocator'
        // is ignored.  The last argument is for removing overload ambiguities
        // and is not used.

    template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
    static void copyConstruct(TARGET_TYPE                *toBegin,
                              FWD_ITER                    fromBegin,
                              FWD_ITER                    fromEnd,
                              ALLOCATOR                  *allocator,
                              bslmf::MetaInt<IS_POINTER_TO_POINTER> *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void copyConstruct(
                            TARGET_TYPE                             *toBegin,
                            const TARGET_TYPE                       *fromBegin,
                            const TARGET_TYPE                       *fromEnd,
                            ALLOCATOR                               *allocator,
                            bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *);
    template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
    static void copyConstruct(TARGET_TYPE                *toBegin,
                              FWD_ITER                    fromBegin,
                              FWD_ITER                    fromEnd,
                              ALLOCATOR                  *allocator,
                              bslmf::MetaInt<NIL_TRAITS> *);
        // These functions follow the 'copyConstruct' contract.  If the
        // parameterized 'ALLOCATOR' is based on 'bslma::Allocator' and the
        // 'TARGET_TYPE' constructors take an allocator argument, then pass the
        // specified 'allocator' to the copy constructor.  The behavior is
        // undefined unless the output array has length at least the distance
        // from 'fromBegin' to 'fromEnd'.  Note that if 'FWD_ITER' is the
        // 'TARGET_TYPE *' pointer type and 'TARGET_TYPE' is bit-wise copyable,
        // then this operation is simply 'memcpy'.  The last argument is for
        // removing overload ambiguities and is not used.

    template <class TARGET_TYPE, class ALLOCATOR>
    static void destructiveMove(
                            TARGET_TYPE                             *toBegin,
                            TARGET_TYPE                             *fromBegin,
                            TARGET_TYPE                             *fromEnd,
                            ALLOCATOR                               *allocator,
                            bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void destructiveMove(TARGET_TYPE                *toBegin,
                                TARGET_TYPE                *fromBegin,
                                TARGET_TYPE                *fromEnd,
                                ALLOCATOR                  *allocator,
                                bslmf::MetaInt<NIL_TRAITS> *);
        // These functions follow the 'destructiveMove' contract.  Note that
        // both arrays cannot overlap (one contains only initialized elements
        // and the other only uninitialized elements), and that if
        // 'TARGET_TYPE' is bit-wise moveable, then this operation is simply
        // 'memcpy'.  The last argument is for removing overload ambiguities
        // and is not used.

    template <class TARGET_TYPE, class ALLOCATOR>
    static void insert(TARGET_TYPE                             *toBegin,
                       TARGET_TYPE                             *toEnd,
                       const TARGET_TYPE&                       value,
                       size_type                                numElements,
                       ALLOCATOR                               *allocator,
                       bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void insert(TARGET_TYPE                             *toBegin,
                       TARGET_TYPE                             *toEnd,
                       const TARGET_TYPE&                       value,
                       size_type                                numElements,
                       ALLOCATOR                               *allocator,
                       bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void insert(TARGET_TYPE                *toBegin,
                       TARGET_TYPE                *toEnd,
                       const TARGET_TYPE&          value,
                       size_type                   numElements,
                       ALLOCATOR                  *allocator,
                       bslmf::MetaInt<NIL_TRAITS> *);
        // These functions follow the 'insert' contract.  Note that if
        // 'TARGET_TYPE' is bit-wise copyable, then this operation is simply
        // 'memmove' followed by 'bitwiseFillN'.  If 'TARGET_TYPE' is bit-wise
        // moveable, then this operation can still be optimized using 'memmove'
        // followed by repeated assignments, but a guard needs to be set up.
        // The last argument is for removing overload ambiguities and is not
        // used.

    template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
    static void insert(TARGET_TYPE                             *toBegin,
                       TARGET_TYPE                             *toEnd,
                       FWD_ITER                                 fromBegin,
                       FWD_ITER                                 fromEnd,
                       size_type                                numElements,
                       ALLOCATOR                               *allocator,
                       bslmf::MetaInt<IS_POINTER_TO_POINTER>   *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void insert(TARGET_TYPE                             *toBegin,
                       TARGET_TYPE                             *toEnd,
                       const TARGET_TYPE                       *fromBegin,
                       const TARGET_TYPE                       *fromEnd,
                       size_type                                numElements,
                       ALLOCATOR                               *allocator,
                       bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *);
    template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
    static void insert(TARGET_TYPE                             *toBegin,
                       TARGET_TYPE                             *toEnd,
                       FWD_ITER                                 fromBegin,
                       FWD_ITER                                 fromEnd,
                       size_type                                numElements,
                       ALLOCATOR                               *allocator,
                       bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *);
    template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
    static void insert(TARGET_TYPE                *toBegin,
                       TARGET_TYPE                *toEnd,
                       FWD_ITER                    fromBegin,
                       FWD_ITER                    fromEnd,
                       size_type                   numElements,
                       ALLOCATOR                  *allocator,
                       bslmf::MetaInt<NIL_TRAITS> *);
        // These functions follow the 'insert' contract.  Note that if
        // 'TARGET_TYPE' is bit-wise copyable and 'FWD_ITER' is convertible to
        // 'const TARGET_TYPE *', then this operation is simply 'memmove'
        // followed by 'memcopy'.  If 'TARGET_TYPE' is bit-wise moveable and
        // 'FWD_ITER' is convertible to 'const TARGET_TYPE *', then this
        // operation can still be optimized using 'memmove' followed by
        // repeated copies.  The last argument is for removing overload
        // ambiguities and is not used.

    template <class TARGET_TYPE, class ALLOCATOR>
    static void moveInsert(
                         TARGET_TYPE                              *toBegin,
                         TARGET_TYPE                              *toEnd,
                         TARGET_TYPE                             **lastPtr,
                         TARGET_TYPE                              *first,
                         TARGET_TYPE                              *last,
                         size_type                                 numElements,
                         ALLOCATOR                                *allocator,
                         bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS>  *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void moveInsert(TARGET_TYPE                 *toBegin,
                           TARGET_TYPE                 *toEnd,
                           TARGET_TYPE                **lastPtr,
                           TARGET_TYPE                 *first,
                           TARGET_TYPE                 *last,
                           size_type                    numElements,
                           ALLOCATOR                   *allocator,
                           bslmf::MetaInt<NIL_TRAITS>  *);
        // These functions follow the 'moveInsert' contract.  Note that if
        // 'TARGET_TYPE' is at least bit-wise moveable, then this operation is
        // simply 'memmove' followed by 'memcpy'.

    template <class TARGET_TYPE, class ALLOCATOR>
    static void erase(TARGET_TYPE                             *first,
                      TARGET_TYPE                             *middle,
                      TARGET_TYPE                             *last,
                      ALLOCATOR                               *allocator,
                      bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void erase(TARGET_TYPE                *first,
                      TARGET_TYPE                *middle,
                      TARGET_TYPE                *last,
                      ALLOCATOR                  *allocator,
                      bslmf::MetaInt<NIL_TRAITS> *);
        // These functions follow the 'erase' contract.  Note that if
        // 'TARGET_TYPE' is bit-wise moveable, then this operation can be
        // implemented by first bit-wise moving the elements in
        // '[ middle, last)' towards first, and destroying
        // '[ last - (middle - first), last)'; note that this cannot throw
        // exceptions.

    static void bitwiseSwapRanges(char *begin, char *middle, char *end);
        // Swap the characters in the array starting at the specified 'first'
        // address and ending immediately before the specified 'middle' address
        // with the array of the same length starting at the 'middle' address
        // and ending at the specified 'last' address.  The behavior is
        // undefined unless 'middle - begin == end - middle'.

    static void bitwiseRotateBackward(char *begin, char *middle, char *end);
        // Move the characters in the array starting at the specified 'first'
        // address and ending immediately before the specified 'middle' address
        // to the array of the same length ending at the specified 'last'
        // address (and thus starting at the 'last - (middle - first)'
        // address), and move the elements previously in the array starting at
        // 'middle' and ending at 'last' down to the 'first' address.  The
        // behavior is undefined unless
        // 'middle - begin <= INPLACE_BUFFER_SIZE'.

    static void bitwiseRotateForward(char *begin, char *middle, char *end);
        // Move the characters in the array starting at the specified 'first'
        // address and ending immediately before the specified 'middle' address
        // to the array of the same length ending at the specified 'last'
        // address (and thus starting at the 'last - (middle - first)'
        // address), and move the elements previously in the array starting at
        // 'middle' and ending at 'last' down to the 'first' address.  The
        // behavior is undefined unless 'end - middle <= INPLACE_BUFFER_SIZE'.

    static void bitwiseRotate(char *begin, char *middle, char *end);
        // This function follows the 'rotate' contract, but by using bit-wise
        // moves on the underlying 'char' array.

    template <class TARGET_TYPE>
    static void rotate(TARGET_TYPE                             *begin,
                       TARGET_TYPE                             *middle,
                       TARGET_TYPE                             *end,
                       bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *);
    template <class TARGET_TYPE>
    static void rotate(TARGET_TYPE                *begin,
                       TARGET_TYPE                *middle,
                       TARGET_TYPE                *end,
                       bslmf::MetaInt<NIL_TRAITS> *);
        // These functions follow the 'rotate' contract, but the first overload
        // is optimized when the parameterized 'TARGET_TYPE' is bit-wise
        // moveable.  The last argument is for removing overload ambiguities
        // and is not used.  Note that if 'TARGET_TYPE' is bit-wise moveable,
        // the 'rotate(char*, char*, char*)' can be used, enabling to take the
        // whole implementation out-of-line.

    template <class FORWARD_ITERATOR>
    static bool isInvalidRange(FORWARD_ITERATOR begin, FORWARD_ITERATOR end);
    template <class TARGET_TYPE>
    static bool isInvalidRange(TARGET_TYPE *begin, TARGET_TYPE *end);
        // Return 'true' if '[begin, end)' provably do not form a valid range,
        // and 'false' otherwise.  Note that 'begin == null == end' produces a
        // valid range, and any other use of the null pointer value will return
        // 'true'.  Also not that this function is intended to support testing,
        // primarily through assertions, so will return 'true' unless it can
        // *prove* that the passed range is invalid.
};

                        // ================================
                        // bslalg_ArrayPrimitives_RemovePtr
                        // ================================

//TBD #ifndef BDE_OMIT_DEPRECATED
template <typename NON_PTR_TYPE>
struct ArrayPrimitives_RemovePtr {
    // Given a template parameter 'T*', yield 'Type == T'.  Given a template
    // paramter 'T' that is not a pointer, yield 'T'.
    //
    // DEPRECATED: In a future release, the class will be phased out and
    // replaced by a new component in bslmf.

    typedef NON_PTR_TYPE Type;
};

template <typename TARGET_TYPE>
struct ArrayPrimitives_RemovePtr<TARGET_TYPE *> {

    typedef TARGET_TYPE Type;
};

template <typename TARGET_TYPE>
struct ArrayPrimitives_RemovePtr<const TARGET_TYPE *> {

    typedef TARGET_TYPE Type;
};

template <typename TARGET_TYPE>
struct ArrayPrimitives_RemovePtr<volatile TARGET_TYPE *> {

    typedef TARGET_TYPE Type;
};

template <typename TARGET_TYPE>
struct ArrayPrimitives_RemovePtr<const volatile TARGET_TYPE *> {

    typedef TARGET_TYPE Type;
};
//TBD #endif  // BDE_OMIT_DEPRECATED

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================
// IMPLEMENTATION NOTES: Specializations of 'uninitializedFillN' for most
// fundamental types are not templates nor inline, and thus can be found in the
// '.cpp' file.

                       // ----------------------
                       // struct ArrayPrimitives
                       // ----------------------

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives::defaultConstruct(TARGET_TYPE *begin,
                                       size_type    numElements,
                                       ALLOCATOR   *allocator)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    enum {
        VALUE = bsl::is_trivially_default_constructible<TARGET_TYPE>::value
              ? Imp::HAS_TRIVIAL_DEFAULT_CTOR_TRAITS
              : bsl::is_trivially_copyable<TARGET_TYPE>::value
                  ? Imp::BITWISE_COPYABLE_TRAITS
                  : Imp::NIL_TRAITS
    };
    ArrayPrimitives_Imp::defaultConstruct(begin,
                                          numElements,
                                          allocator,
                                          (bslmf::MetaInt<VALUE>*)0);
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives::uninitializedFillN(TARGET_TYPE        *begin,
                                         size_type           numElements,
                                         const TARGET_TYPE&  value,
                                         ALLOCATOR          *allocator)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    enum {
         // We provide specialized overloads of 'uninitializedFillN' for
         // fundamental and pointer types.  However, function pointers can have
         // extern "C" linkage and SunPro doesn't match them properly with the
         // pointer template function overload in 'Imp', so we resort to the
         // general case for those.

         IS_FUNCTION_POINTER = bslmf::IsFunctionPointer<TARGET_TYPE>::value,
         IS_FUNDAMENTAL      = bslmf::IsFundamental<TARGET_TYPE>::value,
         IS_POINTER          = bslmf::IsPointer<TARGET_TYPE>::value,

         IS_FUNDAMENTAL_OR_POINTER = IS_FUNDAMENTAL ||
                                     (IS_POINTER && !IS_FUNCTION_POINTER),

         IS_BITWISECOPYABLE  = bsl::is_trivially_copyable<TARGET_TYPE>::value,

         VALUE = IS_FUNDAMENTAL_OR_POINTER ? Imp::IS_FUNDAMENTAL_OR_POINTER
               : IS_BITWISECOPYABLE ?  Imp::BITWISE_COPYABLE_TRAITS
               : Imp::NIL_TRAITS
    };
    ArrayPrimitives_Imp::uninitializedFillN(begin,
                                            value,
                                            numElements,
                                            allocator,
                                            (bslmf::MetaInt<VALUE>*)0);
}

template <class TARGET_TYPE, class ALLOCATOR, class FWD_ITER>
inline
void ArrayPrimitives::copyConstruct(TARGET_TYPE *toBegin,
                                    FWD_ITER     fromBegin,
                                    FWD_ITER     fromEnd,
                                    ALLOCATOR   *allocator)
{
    BSLS_ASSERT_SAFE(toBegin || fromBegin == fromEnd);

    typedef typename ArrayPrimitives_RemovePtr<FWD_ITER>::Type FwdTarget;
    enum {
        ARE_PTRS_TO_PTRS = bslmf::IsPointer<TARGET_TYPE>::value &&
                           bslmf::IsPointer<FWD_ITER   >::value &&
                           bslmf::IsPointer<FwdTarget  >::value,
        IS_BITWISECOPYABLE = bsl::is_trivially_copyable<TARGET_TYPE>::value &&
                             bslmf::IsConvertible<FWD_ITER,
                                                   const TARGET_TYPE *>::value,
        VALUE = ARE_PTRS_TO_PTRS   ? Imp::IS_POINTER_TO_POINTER
              : IS_BITWISECOPYABLE ? Imp::BITWISE_COPYABLE_TRAITS
              : Imp::NIL_TRAITS
    };

    ArrayPrimitives_Imp::copyConstruct(toBegin,
                                       fromBegin,
                                       fromEnd,
                                       allocator,
                                       (bslmf::MetaInt<VALUE>*)0);
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives::destructiveMove(TARGET_TYPE *toBegin,
                                      TARGET_TYPE *fromBegin,
                                      TARGET_TYPE *fromEnd,
                                      ALLOCATOR   *allocator)
{
    BSLS_ASSERT_SAFE(toBegin || fromBegin == fromEnd);
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(fromBegin,
                                                          fromEnd));

    enum {
        VALUE = bslmf::IsBitwiseMoveable<TARGET_TYPE>::value
              ? Imp::BITWISE_MOVEABLE_TRAITS
              : Imp::NIL_TRAITS
    };
    ArrayPrimitives_Imp::destructiveMove(toBegin,
                                         fromBegin,
                                         fromEnd,
                                         allocator,
                                         (bslmf::MetaInt<VALUE>*)0);
}

template <class TARGET_TYPE, class ALLOCATOR>
void ArrayPrimitives::destructiveMoveAndInsert(
                                              TARGET_TYPE         *toBegin,
                                              TARGET_TYPE        **fromEndPtr,
                                              TARGET_TYPE         *fromBegin,
                                              TARGET_TYPE         *position,
                                              TARGET_TYPE         *fromEnd,
                                              const TARGET_TYPE&   value,
                                              size_type            numElements,
                                              ALLOCATOR           *allocator)
{
    // Key to the transformation diagrams:
    //..
    //  A...H   original contents of '[fromBegin, fromEnd)'  ("source")
    //  v...v   copies of 'value'                            ("input")
    //  ; ...   contents of '[toBegin, toEnd)'               ("destination")
    //  ..:..   position of 'fromEndPtr' in the input
    //  _____   uninitialized array elements
    //  [...]   part of array protected by an exception guard object
    //..

    *fromEndPtr = fromEnd;

    // Note: Construct copies of 'value' first in case 'value' is a reference
    // in the input range, which would be invalidated by any of the following
    // moves.
    //
    //..
    //  Transformation: ABCDEFGH: ; _____________[]
    //               => ABCDEFGH: ; ____[vvvvv]____
    //..

    TARGET_TYPE *toPositionBegin = toBegin + (position - fromBegin);
    uninitializedFillN(toPositionBegin, numElements, value, allocator);

    TARGET_TYPE *toPositionEnd   = toPositionBegin + numElements;
    AutoArrayDestructor<TARGET_TYPE> guard(toPositionBegin,
                                           toPositionEnd);

    //..
    //  Transformation: ABCDEFGH: ; ____[vvvvv]____
    //               => ABCD:____ ; ____[vvvvvEFGH]
    //..

    destructiveMove(toPositionEnd,
                    position,
                    fromEnd,
                    allocator);

    *fromEndPtr = position;  // shorten input range after partial destruction
    guard.moveEnd(fromEnd - position);  // toEnd

    //..
    //  Transformation: ABCD:____ ; ____[vvvvvEFGH]
    //               => :________ ; ABCDvvvvvEFGH[]
    //..

    destructiveMove(toBegin,
                    fromBegin,
                    position,
                    allocator);

    *fromEndPtr = fromBegin;  // empty input range after final destruction
    guard.release();
}

template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
void ArrayPrimitives::destructiveMoveAndInsert(TARGET_TYPE  *toBegin,
                                               TARGET_TYPE **fromEndPtr,
                                               TARGET_TYPE  *fromBegin,
                                               TARGET_TYPE  *position,
                                               TARGET_TYPE  *fromEnd,
                                               FWD_ITER      first,
                                               FWD_ITER      last,
                                               size_type     numElements,
                                               ALLOCATOR    *allocator)
{
    // Key to the transformation diagrams:
    //..
    //  A...H   original contents of '[fromBegin, fromEnd)'  ("source")
    //  t...z   original contents of '[first, last)'         ("input")
    //  ; ...   contents of '[toBegin, toEnd)'               ("destination")
    //  ..:..   position of 'fromEndPtr' in the input
    //  _____   uninitialized array elements
    //  [...]   part of array protected by an exception guard object
    //..

    *fromEndPtr = fromEnd;

    // Note: Construct copies of 'value' first in case 'value' is a reference
    // in the input range, which would be invalidated by any of the following
    // moves:
    //..
    //  Transformation: ABCDEFGH: ; _________[]____
    //               => ABCDEFGH: ; ____[tuvxy]____
    //..

    TARGET_TYPE *toPositionBegin = toBegin + (position - fromBegin);
    copyConstruct(toPositionBegin, first, last, allocator);

    TARGET_TYPE *toPositionEnd   = toPositionBegin + numElements;
    AutoArrayDestructor<TARGET_TYPE> guard(toPositionBegin,
                                           toPositionEnd);

    //..
    //  Transformation: ABCDEFGH: ; ____[tuvxy]____
    //               => ABCD:____ ; ____[tuvxyEFGH]
    //..

    destructiveMove(toPositionEnd,
                    position,
                    fromEnd,
                    allocator);

    *fromEndPtr = position;  // shorten input range after partial destruction
    guard.moveEnd(fromEnd - position);  // toEnd

    //..
    //  Transformation: ABCD:____ ; ____[tuvxyEFGH]
    //               => :________ ; ABCDtuvxyEFGH[]
    //..

    destructiveMove(toBegin,
                    fromBegin,
                    position,
                    allocator);

    *fromEndPtr = fromBegin;  // empty input range after final destruction
    guard.release();
}

template <class TARGET_TYPE, class ALLOCATOR>
void ArrayPrimitives::destructiveMoveAndMoveInsert(TARGET_TYPE  *toBegin,
                                                   TARGET_TYPE **fromEndPtr,
                                                   TARGET_TYPE **lastPtr,
                                                   TARGET_TYPE  *fromBegin,
                                                   TARGET_TYPE  *position,
                                                   TARGET_TYPE  *fromEnd,
                                                   TARGET_TYPE  *first,
                                                   TARGET_TYPE  *last,
                                                   size_type     numElements,
                                                   ALLOCATOR    *allocator)
{
    // Key to the transformation diagrams:
    //..
    //  A...H   original contents of '[fromBegin, fromEnd)'  ("source")
    //  t...z   original contents of '[first, last)'         ("input")
    //  ; ...   contents of '[toBegin, toEnd)'               ("destination")
    //  ..:..   position of 'fromEndPtr' in the input
    //  _____   uninitialized array elements
    //  [...]   part of array protected by an exception guard object
    //..

    *lastPtr = last;
    *fromEndPtr = fromEnd;

    //..
    //  Transformation: ABCDEFGH: ; _____________[]
    //               => ABCD:____ ; _________[EFGH]
    //..

    TARGET_TYPE *toPositionBegin = toBegin + (position - fromBegin);
    TARGET_TYPE *toPositionEnd   = toPositionBegin + numElements;
    destructiveMove(toPositionEnd,
                    position,
                    fromEnd,
                    allocator);

    *fromEndPtr = position;  // shorten input range after partial destruction

    AutoArrayDestructor<TARGET_TYPE>
                                   guard(toPositionEnd,
                                         toPositionEnd + (fromEnd - position));

    //..
    //  Transformation: ABCD:____ ; _________[EFGH]
    //               => ABCD:____ ; _____[tuvwEFGH]
    //..

    destructiveMove(toPositionBegin, first, last, allocator);

    *lastPtr = first;
    guard.moveBegin(-(difference_type)numElements);

    //..
    //  Transformation: ABCD:____ ; ____[tuvwEFGH]
    //               => :________ ; ABCDtuvwEFGH[]
    //..

    destructiveMove(toBegin,
                    fromBegin,
                    position,
                    allocator);

    *fromEndPtr = fromBegin;  // empty input range after final destruction
    guard.release();
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives::insert(TARGET_TYPE        *toBegin,
                             TARGET_TYPE        *toEnd,
                             const TARGET_TYPE&  value,
                             size_type           numElements,
                             ALLOCATOR          *allocator)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin,
                                                          toEnd));
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    if (0 == numElements) {
        return;                                                       // RETURN
    }

    enum {
        VALUE = bsl::is_trivially_copyable<TARGET_TYPE>::value
              ? Imp::BITWISE_COPYABLE_TRAITS
              : bslmf::IsBitwiseMoveable<TARGET_TYPE>::value
                  ? Imp::BITWISE_MOVEABLE_TRAITS
                  : Imp::NIL_TRAITS
    };
    ArrayPrimitives_Imp::insert(toBegin,
                                toEnd,
                                value,
                                numElements,
                                allocator,
                                (bslmf::MetaInt<VALUE>*)0);
}

template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
inline
void ArrayPrimitives::insert(TARGET_TYPE *toBegin,
                             TARGET_TYPE *toEnd,
                             FWD_ITER     fromBegin,
                             FWD_ITER     fromEnd,
                             size_type    numElements,
                             ALLOCATOR   *allocator)
{
    if (0 == numElements) {
        return;                                                       // RETURN
    }

    typedef typename ArrayPrimitives_RemovePtr<FWD_ITER>::Type FwdTarget;
    enum {
        ARE_PTRS_TO_PTRS = bslmf::IsPointer<TARGET_TYPE>::value &&
                           bslmf::IsPointer<FWD_ITER   >::value &&
                           bslmf::IsPointer<FwdTarget  >::value,
        IS_BITWISEMOVEABLE  = bslmf::IsBitwiseMoveable<TARGET_TYPE>::value,
        IS_BITWISECOPYABLE  = bslmf::IsConvertible<FWD_ITER,
                                                   const TARGET_TYPE *>::value
                            && bsl::is_trivially_copyable<TARGET_TYPE>::value,
        VALUE = ARE_PTRS_TO_PTRS   ? Imp::IS_POINTER_TO_POINTER
              : IS_BITWISECOPYABLE ? Imp::BITWISE_COPYABLE_TRAITS
              : IS_BITWISEMOVEABLE ? Imp::BITWISE_MOVEABLE_TRAITS
              : Imp::NIL_TRAITS
    };
    ArrayPrimitives_Imp::insert(toBegin,
                                toEnd,
                                fromBegin,
                                fromEnd,
                                numElements,
                                allocator,
                                (bslmf::MetaInt<VALUE>*)0);
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives::moveInsert(TARGET_TYPE  *toBegin,
                                 TARGET_TYPE  *toEnd,
                                 TARGET_TYPE **fromEndPtr,
                                 TARGET_TYPE  *fromBegin,
                                 TARGET_TYPE  *fromEnd,
                                 size_type     numElements,
                                 ALLOCATOR    *allocator)
{
    enum {
        VALUE = bslmf::IsBitwiseMoveable<TARGET_TYPE>::value
              ? Imp::BITWISE_MOVEABLE_TRAITS
              : Imp::NIL_TRAITS
    };
    ArrayPrimitives_Imp::moveInsert(toBegin,
                                    toEnd,
                                    fromEndPtr,
                                    fromBegin,
                                    fromEnd,
                                    numElements,
                                    allocator,
                                    (bslmf::MetaInt<VALUE>*)0);
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives::erase(TARGET_TYPE *first,
                            TARGET_TYPE *middle,
                            TARGET_TYPE *last,
                            ALLOCATOR   *allocator)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(first,
                                                          middle));
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(middle,
                                                          last));

    if (first == middle) { // erasing empty range O(1) versus O(N): Do not
                           // remove!
        return;
    }

    enum {
        VALUE = bslmf::IsBitwiseMoveable<TARGET_TYPE>::value
              ? Imp::BITWISE_MOVEABLE_TRAITS
              : Imp::NIL_TRAITS
    };
    ArrayPrimitives_Imp::erase(first,
                               middle,
                               last,
                               allocator,
                               (bslmf::MetaInt<VALUE>*)0);
}

template <class TARGET_TYPE>
inline
void ArrayPrimitives::rotate(TARGET_TYPE *first,
                             TARGET_TYPE *middle,
                             TARGET_TYPE *last)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(first,
                                                          middle));
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(middle,
                                                          last));

    enum {
        VALUE = bslmf::IsBitwiseMoveable<TARGET_TYPE>::value
              ? Imp::BITWISE_MOVEABLE_TRAITS
              : Imp::NIL_TRAITS
    };
    ArrayPrimitives_Imp::rotate(first,
                                middle,
                                last,
                                (bslmf::MetaInt<VALUE>*)0);
}

                     // --------------------------
                     // struct ArrayPrimitives_Imp
                     // --------------------------

// CLASS METHODS

              // *** 'defaultConstruct' overloads: ***

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::defaultConstruct(
                  TARGET_TYPE                                     *begin,
                  size_type                                        numElements,
                  ALLOCATOR                                       *,
                  bslmf::MetaInt<HAS_TRIVIAL_DEFAULT_CTOR_TRAITS> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    std::memset((void *)begin, 0, sizeof(TARGET_TYPE) * numElements);
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::defaultConstruct(
                          TARGET_TYPE                             *begin,
                          size_type                                numElements,
                          ALLOCATOR                               *allocator,
                          bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    ScalarPrimitives::defaultConstruct(begin, allocator);
    bitwiseFillN((char *)begin,
                 sizeof(TARGET_TYPE),
                 numElements * sizeof(TARGET_TYPE));
}

template <class TARGET_TYPE, class ALLOCATOR>
void ArrayPrimitives_Imp::defaultConstruct(
                                       TARGET_TYPE                *begin,
                                       size_type                   numElements,
                                       ALLOCATOR                  *allocator,
                                       bslmf::MetaInt<NIL_TRAITS> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    AutoArrayDestructor<TARGET_TYPE> guard(begin, begin);

    const TARGET_TYPE *end = begin + numElements;
    while (begin != end) {
        ScalarPrimitives::defaultConstruct(begin, allocator);
        begin = guard.moveEnd(1);
    }
    guard.release();
}

                   // *** 'uninitializedFillN' overloads: ***

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                        bool                                      *begin,
                        bool                                       value,
                        size_type                                  numElements,
                        void                                      *,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    std::memset((char *)begin, (char)value, numElements);
}

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                        char                                      *begin,
                        char                                       value,
                        size_type                                  numElements,
                        void                                      *,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    std::memset(begin, value, numElements);
}

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                        unsigned char                             *begin,
                        unsigned char                              value,
                        size_type                                  numElements,
                        void                                      *,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    std::memset(begin, value, numElements);
}

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                        signed char                               *begin,
                        signed char                                value,
                        size_type                                  numElements,
                        void                                      *,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    std::memset(begin, value, numElements);
}

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                        wchar_t                                   *begin,
                        wchar_t                                    value,
                        size_type                                  numElements,
                        void                                      *,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    std::wmemset(begin, value, numElements);
}

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                        unsigned short                            *begin,
                        unsigned short                             value,
                        size_type                                  numElements,
                        void                                      *,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    uninitializedFillN(
                      (short *)begin, (short)value, numElements,
                      (void*)0, (bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER>*)0);
}

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                        unsigned int                              *begin,
                        unsigned int                               value,
                        size_type                                  numElements,
                        void                                      *,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    uninitializedFillN(
                      (int *)begin, (int)value, numElements,
                      (void*)0, (bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER>*)0);
}

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                        long                                      *begin,
                        long                                       value,
                        size_type                                  numElements,
                        void                                      *,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

#if defined(BSLS_PLATFORM_CPU_64_BIT) && !defined(BSLS_PLATFORM_OS_WINDOWS)
    uninitializedFillN((bsls::Types::Int64 *)begin,
                       (bsls::Types::Int64)value,
                       numElements);
#else
    uninitializedFillN((int *)begin,
                       (int)value,
                       numElements,
                       (void*)0,
                       (bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER>*)0);
#endif
}

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                        unsigned long                             *begin,
                        unsigned long                              value,
                        size_type                                  numElements,
                        void                                      *,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

#if defined(BSLS_PLATFORM_CPU_64_BIT) && !defined(BSLS_PLATFORM_OS_WINDOWS)
    uninitializedFillN(
                      (bsls::Types::Int64 *)begin,
                      (bsls::Types::Int64)value,
                      numElements,
                      (void*)0, (bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER>*)0);
#else
    uninitializedFillN(
                      (int *)begin, (int)value, numElements,
                      (void*)0, (bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER>*)0);
#endif
}

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                        bsls::Types::Uint64                       *begin,
                        bsls::Types::Uint64                        value,
                        size_type                                  numElements,
                        void                                      *,
                        bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    uninitializedFillN((bsls::Types::Int64 *)begin,
                       (bsls::Types::Uint64)value,
                       numElements,
                       (void*)0,
                       (bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER>*)0);
}

template <class TARGET_TYPE>
inline
void ArrayPrimitives_Imp::uninitializedFillN(
                       TARGET_TYPE                               **begin,
                       TARGET_TYPE                                *value,
                       size_type                                   numElements,
                       void                                       *,
                       bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER>  *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    // Note: 'const'-correctness is respected because the next overload picks
    // up the 'const TARGET_TYPE' and will be a better match.  Note that we
    // cannot cast to 'const void **' (one would have to add 'const' at every
    // level, not just the innermost; i.e., 'const void *const *' would be
    // correct, 'const void **' is not [C++ Standard, 4.4 Qualification
    // conversions]).

    uninitializedFillN((void **)begin,
                       (void *)value,
                       numElements,
                       (void*)0,
                       (bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER>*)0);
}

template <class TARGET_TYPE>
inline
void ArrayPrimitives_Imp::uninitializedFillN(
                       const TARGET_TYPE                         **begin,
                       const TARGET_TYPE                          *value,
                       size_type                                   numElements,
                       void                                       *,
                       bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER>  *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    // While it seems that this overload is subsumed by the previous template,
    // SunPro does not detect it.

    uninitializedFillN((const void **)begin,
                       (const void *)value,
                       numElements,
                       (void*)0,
                       (bslmf::MetaInt<IS_FUNDAMENTAL_OR_POINTER>*)0);
}

template <class TARGET_TYPE, class ALLOCATOR>
void ArrayPrimitives_Imp::uninitializedFillN(
                          TARGET_TYPE                             *begin,
                          const TARGET_TYPE&                       value,
                          size_type                                numElements,
                          ALLOCATOR                               *,
                          bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    if (0 == numElements) {
        return;
    }

    // Important sub-case: When value is identically (bit-wise) 0, such as
    // happens for default-constructed values of a type that has a trivial
    // default constructor, or even the same byte pattern repeated over
    // 'sizeof value' times, we can use 'memset.

    size_type index = 0;
    const char *valueBuffer = (const char *) BSLS_UTIL_ADDRESSOF(value);
    while (++index < sizeof(TARGET_TYPE)) {
        if (valueBuffer[index] != valueBuffer[0]) {
            break;
        }
    }

    if (index == sizeof value) {
        std::memset(begin, valueBuffer[0], sizeof(TARGET_TYPE) * numElements);
    } else {
        std::memcpy(begin, valueBuffer, sizeof(TARGET_TYPE));
        bitwiseFillN((char *)begin,
                     sizeof(TARGET_TYPE),
                     sizeof(TARGET_TYPE) * numElements);
    }
}

template <class TARGET_TYPE, class ALLOCATOR>
void ArrayPrimitives_Imp::uninitializedFillN(
                                       TARGET_TYPE                *begin,
                                       const TARGET_TYPE&          value,
                                       size_type                   numElements,
                                       ALLOCATOR                  *allocator,
                                       bslmf::MetaInt<NIL_TRAITS> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    if (0 == numElements) {
        return;                                                       // RETURN
    }
    AutoArrayDestructor<TARGET_TYPE> guard(begin, begin);

    TARGET_TYPE *end = begin + numElements;
    do {
        ScalarPrimitives::copyConstruct(begin, value, allocator);
        begin = guard.moveEnd(1);
    } while (begin != end);
    guard.release();
}

                    // *** 'copyConstruct' overloads: ***

template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::copyConstruct(
                              TARGET_TYPE                           *toBegin,
                              FWD_ITER                               fromBegin,
                              FWD_ITER                               fromEnd,
                              ALLOCATOR                             *allocator,
                              bslmf::MetaInt<IS_POINTER_TO_POINTER> *)
{
    // We may be casting a func ptr to a 'void *' here, so this won't work if
    // we port to an architecture where the two are of different sizes.

    BSLMF_ASSERT(sizeof(void *) == sizeof(void (*)()));

    typedef typename bslmf::RemovePtrCvq<TARGET_TYPE>::Type NoConstTargetType;
    typedef typename bslmf::RemovePtrCvq<FWD_ITER>::ValueType
                                                           NoConstFwdIterValue;
    typedef typename bslmf::RemovePtrCvq<NoConstFwdIterValue>::ValueType
                                                      NoConstFwdIterValueValue;

    copyConstruct(
           (void *       *) const_cast<NoConstTargetType *>(toBegin),
           (void * const *) const_cast<NoConstFwdIterValueValue **>(fromBegin),
           (void * const *) const_cast<NoConstFwdIterValueValue **>(fromEnd),
           allocator,
           (bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *) 0);
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::copyConstruct(
                            TARGET_TYPE                             *toBegin,
                            const TARGET_TYPE                       *fromBegin,
                            const TARGET_TYPE                       *fromEnd,
                            ALLOCATOR                               *,
                            bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *)
{
    BSLS_ASSERT_SAFE(toBegin);
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(fromBegin,
                                                          fromEnd));

    const size_type numBytes = (const char*)fromEnd - (const char*)fromBegin;
    std::memcpy(toBegin, fromBegin, numBytes);
}

template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
void ArrayPrimitives_Imp::copyConstruct(TARGET_TYPE                *toBegin,
                                        FWD_ITER                    fromBegin,
                                        FWD_ITER                    fromEnd,
                                        ALLOCATOR                  *allocator,
                                        bslmf::MetaInt<NIL_TRAITS> *)
{
    BSLS_ASSERT_SAFE(toBegin || fromBegin == fromEnd);
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(fromBegin,
                                                          fromEnd));

    AutoArrayDestructor<TARGET_TYPE> guard(toBegin, toBegin);

    while (fromBegin != fromEnd) {
        // Note: We are not sure the value type of 'FWD_ITER' is convertible to
        // 'TARGET_TYPE'.  Use 'construct' instead.

        ScalarPrimitives::construct(toBegin, *fromBegin, allocator);
        ++fromBegin;
        toBegin = guard.moveEnd(1);
    }
    guard.release();
}

                     // *** 'destructiveMove' overloads: ***

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::destructiveMove(
                            TARGET_TYPE                             *toBegin,
                            TARGET_TYPE                             *fromBegin,
                            TARGET_TYPE                             *fromEnd,
                            ALLOCATOR                               *,
                            bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *)
{
    BSLS_ASSERT_SAFE(toBegin || fromBegin == fromEnd);
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(fromBegin,
                                                          fromEnd));

    const size_type numBytes = (const char*)fromEnd - (const char*)fromBegin;
    std::memcpy(toBegin, fromBegin, numBytes);
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::destructiveMove(
                                         TARGET_TYPE                *toBegin,
                                         TARGET_TYPE                *fromBegin,
                                         TARGET_TYPE                *fromEnd,
                                         ALLOCATOR                  *allocator,
                                         bslmf::MetaInt<NIL_TRAITS> *)
{
    BSLS_ASSERT_SAFE(toBegin || fromBegin == fromEnd);
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(fromBegin,
                                                          fromEnd));

    // 'TARGET_TYPE' certainly cannot be bit-wise copyable, so we can save the
    // compiler some work.

    copyConstruct(toBegin, fromBegin, fromEnd, allocator,
                  (bslmf::MetaInt<NIL_TRAITS>*)0);
    ArrayDestructionPrimitives::destroy(fromBegin, fromEnd);
}

                   // *** 'insert' with 'value' overloads: ***

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::insert(
                          TARGET_TYPE                             *toBegin,
                          TARGET_TYPE                             *toEnd,
                          const TARGET_TYPE&                       value,
                          size_type                                numElements,
                          ALLOCATOR                               *allocator,
                          bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin,
                                                          toEnd));
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    // Key to the transformation diagrams:
    //..
    //  A...G   original contents of '[toBegin, toEnd)'  ("tail")
    //  v...v   contents of '[fromBegin, fromEnd)'       ("input")
    //  _____   uninitialized array element
    //..

    // ALIASING: If 'value' is a reference into the array 'toBegin..toEnd',
    // then moving the array first might introduce a change in 'value'.  Since
    // type is bitwise copyable, then no memory changes outside the array, so
    // the test below is sufficient to discover all the possible aliasing.
    // Note that we never make a copy.

    const TARGET_TYPE *tempValuePtr = BSLS_UTIL_ADDRESSOF(value);
    if (toBegin <= tempValuePtr && tempValuePtr < toEnd ) {
        // Adjust pointer for shifting after the move.

        tempValuePtr += numElements;
    }

    //..
    //  Transformation: ABCDE___ => ___ABCDE  (might overlap).
    //..

    const size_type numBytes = (const char*)toEnd - (const char*)toBegin;
    std::memmove(toBegin + numElements, toBegin, numBytes);

    //..
    //  Transformation: ___ABCDE => v__ABCDE (no overlap).
    //..

    // Use 'copyConstruct' instead of 'memcpy' because the former optimizes for
    // fundamental types using 'operator=' instead, which avoid the 'memcpy'
    // function call.

    ScalarPrimitives::copyConstruct(toBegin,
                                    *tempValuePtr,
                                    allocator);

    //..
    //  Transformation: v__ABCDE => vvvABCDE.
    //..

    bitwiseFillN((char*)toBegin, sizeof value, numElements * sizeof value);
}

template <class TARGET_TYPE, class ALLOCATOR>
void ArrayPrimitives_Imp::insert(
                          TARGET_TYPE                             *toBegin,
                          TARGET_TYPE                             *toEnd,
                          const TARGET_TYPE&                       value,
                          size_type                                numElements,
                          ALLOCATOR                               *allocator,
                          bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin,
                                                          toEnd));
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    // Key to the transformation diagrams:
    //..
    //  A...G   original contents of '[toBegin, toEnd)'  ("tail")
    //  v...v   contents of '[fromBegin, fromEnd)'       ("input")
    //  _____   uninitialized array element
    //  [...]   part of an array guarded by an exception guard object
    //  |.(.,.) part of array guarded by move guard
    //          (middle indicated by ',' and dest by '|')
    //..

    const TARGET_TYPE *tempValuePtr = BSLS_UTIL_ADDRESSOF(value);
    if (toBegin <= tempValuePtr && tempValuePtr < toEnd + numElements) {
        // Adjust pointer for shifting after the move.

        tempValuePtr += numElements;
    }

    size_type tailLen    = toEnd - toBegin;
    size_type numGuarded = tailLen < numElements ? tailLen : numElements;

    //..
    //  Transformation: ABCDE_______ => _______ABCDE (might overlap)
    //..

    TARGET_TYPE *destBegin = toBegin + numElements;
    std::memmove(destBegin, toBegin, tailLen * sizeof(TARGET_TYPE));

    //..
    //  Transformation: |_______(,ABCDE) => vvvvv|__(ABCDE,)
    //..

    TARGET_TYPE *destEnd = toEnd + numElements;
    AutoArrayMoveDestructor<TARGET_TYPE> guard(toBegin,
                                               destEnd - numGuarded,
                                               destEnd - numGuarded,
                                               destEnd);

    while (guard.middle() != guard.end()) {
        ScalarPrimitives::copyConstruct(guard.destination(),
                                        *tempValuePtr,
                                        allocator);
        guard.advance();
    }

    // The bitwise 'guard' is now inactive, since 'middle() == end()' and
    // 'guard.destination()' is the smaller of 'destBegin' or 'toEnd'.

    if (tailLen < numElements) {
        // There still is a gap of 'numElements - tailLen' to fill in between
        // 'toEnd' and 'destBegin'.  The elements that have been 'memmove'-ed
        // need to be guarded, we fill the gap backward from there to keep
        // guarded portion in one piece.

        AutoArrayDestructor<TARGET_TYPE> endGuard(destBegin, destEnd);

        //..
        //  Transformation: vvvvv__[ABCDE] => vvvvv[vvABCDE]
        //..

        while (toEnd != destBegin) {
            ScalarPrimitives::copyConstruct(--destBegin,
                                            *tempValuePtr,
                                            allocator);
            endGuard.moveBegin(-1);
        }
        endGuard.release();
    }
}

template <class TARGET_TYPE, class ALLOCATOR>
void ArrayPrimitives_Imp::insert(TARGET_TYPE                *toBegin,
                                 TARGET_TYPE                *toEnd,
                                 const TARGET_TYPE&          value,
                                 size_type                   numElements,
                                 ALLOCATOR                  *allocator,
                                 bslmf::MetaInt<NIL_TRAITS> *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin,
                                                          toEnd));
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    // Key to the transformation diagrams:
    //..
    //  A...G   original contents of '[toBegin, toEnd)'  ("tail")
    //  v...v   copies of 'value'                        ("input")
    //  _____   uninitialized array elements
    //  [...]   part of array protected by an exception guard object
    //..

    const size_type tailLen = toEnd - toBegin;
    if (tailLen >= numElements) {
        // Tail is not shorter than input.

        //..
        //  Transformation: ABCDEFG___[] => ABCDEFG[EFG].
        //..

        copyConstruct(                                           // destination
                      toEnd,
                      toEnd - numElements,  // source
                      toEnd,                // end source
                      allocator,
                      (bslmf::MetaInt<NIL_TRAITS>*)0);

        AutoArrayDestructor<TARGET_TYPE> guard(toEnd,
                                               toEnd + numElements);

        // Aliasing: Make a temp copy of 'value' (always).  The reason is that
        // 'value' could be a reference inside the input range, or even outside
        // but with lifetime controlled by one of these values, and so the next
        // transformation could invalidate 'value'.  Note: One cannot rely on
        // 'TARGET_TYPE' to have a single-argument copy ctor (i.e., default
        // allocator argument to 0) if it takes an allocator; hence the
        // constructor proxy.

        ConstructorProxy<TARGET_TYPE>
                                 tempValue(value, bslma::Default::allocator());

        //..
        //  Transformation: ABCDEFG[EFG] => ABCABCD[EFG].
        //..

        TARGET_TYPE *src  = toEnd - numElements;
        TARGET_TYPE *dest = toEnd;
        while (toBegin != src) {
            *--dest = *--src;
        }

        //..
        //  Transformation: ABCABCD[EFG] => vvvABCD[EFG].
        //..

        for ( ; toBegin != dest; ++toBegin) {
            *toBegin = tempValue.object();
        }

        guard.release();
    }
    else {
        // Tail is shorter than input.  We can avoid the temp copy of value
        // since there will be space to make a first copy after the tail, and
        // use that to make the subsequent copies.

        difference_type remElements = numElements - tailLen;

        //..
        //  Transformation: ABC_______[] => ABC____[ABC].
        //..

        copyConstruct(                                           // destination
                      toBegin + numElements,
                      toBegin,                // source
                      toEnd,                  // end source
                      allocator,
                      (bslmf::MetaInt<NIL_TRAITS>*)0);

        AutoArrayDestructor<TARGET_TYPE> guard(toEnd + remElements,
                                               toEnd + numElements);

        //..
        //  Transformation: ABC____[ABC] => ABC[vvvvABC].
        //..

        uninitializedFillN(toEnd,
                           value,
                           remElements,
                           allocator,
                           (bslmf::MetaInt<NIL_TRAITS>*)0);
        guard.moveBegin(-remElements);

        //..
        //  Transformation: ABC[vvvvABC] => vvv[vvvvABC].
        //..

        for ( ; toBegin != toEnd; ++toBegin) {
            *toBegin = *toEnd;
        }

        guard.release();
    }
}

                  // *** 'insert' with 'FWD_ITER' overloads: ***


template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::insert(
                          TARGET_TYPE                             *toBegin,
                          TARGET_TYPE                             *toEnd,
                          FWD_ITER                                 fromBegin,
                          FWD_ITER                                 fromEnd,
                          size_type                                numElements,
                          ALLOCATOR                               *allocator,
                          bslmf::MetaInt<IS_POINTER_TO_POINTER>   *)
{
    // We may be casting a func ptr to a 'void *' here, so this won't work if
    // we port to an architecture where the two are of different sizes.

    BSLMF_ASSERT(sizeof(void *) == sizeof(void (*)()));

    typedef typename bslmf::RemovePtrCvq<TARGET_TYPE>::Type NoConstTargetType;
    typedef typename bslmf::RemovePtrCvq<FWD_ITER>::ValueType
                                                           NoConstFwdIterValue;
    typedef typename bslmf::RemovePtrCvq<NoConstFwdIterValue>::ValueType
                                                      NoConstFwdIterValueValue;

    insert((void *       *) const_cast<NoConstTargetType *>(toBegin),
           (void *       *) const_cast<NoConstTargetType *>(toEnd),
           (void * const *) const_cast<NoConstFwdIterValueValue **>(fromBegin),
           (void * const *) const_cast<NoConstFwdIterValueValue **>(fromEnd),
           numElements,
           allocator,
           (bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *) 0);
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::insert(
                          TARGET_TYPE                             *toBegin,
                          TARGET_TYPE                             *toEnd,
                          const TARGET_TYPE                       *fromBegin,
                          const TARGET_TYPE                       *fromEnd,
                          size_type                                numElements,
                          ALLOCATOR                               *,
                          bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *)
{
    // 'FWD_ITER' has been converted to a 'const TARGET_TYPE *' and
    // 'TARGET_TYPE' is bit-wise copyable.
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin,
                                                          toEnd));
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(fromBegin,
                                                          fromEnd));
    BSLS_ASSERT_SAFE(fromBegin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    BSLS_ASSERT_SAFE(fromBegin + numElements == fromEnd);
    BSLS_ASSERT_SAFE(fromEnd <= toBegin || toEnd + numElements <= fromBegin);
    (void)fromEnd;  // quell warning when 'BSLS_ASSERT_SAFE' is compiled out

    // Key to the transformation diagrams:
    //..
    //  A...G   original contents of '[toBegin, toEnd)'  ("tail")
    //  t...z   contents of '[fromBegin, fromEnd)'       ("input")
    //  _____   uninitialized array element
    //..

    //..
    //  Transformation: ABCDE_______ => _______ABCDE (might overlap).
    //..

    const size_type numBytes = (const char*)toEnd - (const char*)toBegin;
    std::memmove(toBegin + numElements, toBegin, numBytes);

    //..
    //  Transformation: _______ABCDE => tuvwxyzABCDE (no overlap).
    //..

    std::memcpy(toBegin, fromBegin, numElements * sizeof(TARGET_TYPE));
}

template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
void ArrayPrimitives_Imp::insert(
                          TARGET_TYPE                             *toBegin,
                          TARGET_TYPE                             *toEnd,
                          FWD_ITER                                 fromBegin,
                          FWD_ITER,
                          size_type                                numElements,
                          ALLOCATOR                               *allocator,
                          bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *)
{
    // 'TARGET_TYPE' is bit-wise moveable.
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin,
                                                          toEnd));
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    if (0 == numElements) {
        return;                                                       // RETURN
    }

    // The following assertions only make sense if 'FWD_ITER' is a pointer to a
    // possibly cv-qualified 'TARGET_TYPE', and are tested in that overload
    // (see above).
    //..
    //  BSLS_ASSERT(fromBegin + numElements == fromEnd);
    //  BSLS_ASSERT(fromEnd <= toBegin || toEnd + numElements <= fromBegin);
    //..

    // Key to the transformation diagrams:
    //..
    //  A...G   original contents of '[toBegin, toEnd)'  ("tail")
    //  t...z   contents of '[fromBegin, fromEnd)'       ("input")
    //  _____   uninitialized array element
    //  [...]   part of array guarded by exception guard
    //  |.(.,.) part of array guarded by move guard
    //          (middle indicated by ',' and dest by '|')
    //..

    const size_type tailLen  = toEnd - toBegin;
    const size_type numGuarded = tailLen < numElements ? tailLen : numElements;

    //..
    //  Transformation: ABCDE____ => ____ABCDE (might overlap).
    //..

    TARGET_TYPE *destBegin = toBegin + numElements;
    std::memmove(destBegin, toBegin, tailLen * sizeof(TARGET_TYPE));

    //..
    //  Transformation: |_______(,ABCDE) => tuvwx|__(ABCDE,).
    //..

    TARGET_TYPE *destEnd = toEnd + numElements;
    AutoArrayMoveDestructor<TARGET_TYPE> guard(toBegin,
                                               destEnd - numGuarded,
                                               destEnd - numGuarded,
                                               destEnd);

    for (; guard.middle() != guard.end(); ++fromBegin) {
        ScalarPrimitives::construct(guard.destination(),
                                    *fromBegin,
                                    allocator);
        guard.advance();
    }

    // The bitwise 'guard' is now inactive, since 'middle() == end()', and
    // 'guard.destination()' is the smaller of 'destBegin' or 'toEnd'.

    if (tailLen < numElements) {
        // There still is a gap of 'numElements - tailLen' to fill in between
        // 'toEnd' and 'destBegin'.  The elements that have been 'memmove'-ed
        // need to be guarded, and we need to continue to fill the hole at the
        // same guarding the copied elements as well.

        AutoArrayDestructor<TARGET_TYPE> endGuard1(toEnd, toEnd);
        AutoArrayDestructor<TARGET_TYPE> endGuard2(destBegin, destEnd);

        //..
        //  Transformation: tuvwx__[ABCDE] => tuvwx[yzABCDE].
        //..

        for (; toEnd != destBegin; ++fromBegin) {
            ScalarPrimitives::construct(toEnd,
                                        *fromBegin,
                                        allocator);
            toEnd = endGuard1.moveEnd(1);
        }
        endGuard1.release();
        endGuard2.release();
    }
}

template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
void ArrayPrimitives_Imp::insert(TARGET_TYPE                *toBegin,
                                 TARGET_TYPE                *toEnd,
                                 FWD_ITER                    fromBegin,
                                 FWD_ITER                    fromEnd,
                                 size_type                   numElements,
                                 ALLOCATOR                  *allocator,
                                 bslmf::MetaInt<NIL_TRAITS> *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin,
                                                          toEnd));
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    if (0 == numElements) {
        return;                                                       // RETURN
    }

    // Key to the transformation diagrams:
    //..
    //  A...G   original contents of '[toBegin, toEnd)'  ("tail")
    //  t...z   contents of '[fromBegin, fromEnd)'       ("input")
    //  _____   uninitialized array elements
    //  [...]   part of array protected by a guard object
    //..

    const size_type tailLen = toEnd - toBegin;
    if (tailLen > numElements) {
        // Tail is longer than input.

        //..
        //  Transformation: ABCDEFG___[] => ABCDEFG[EFG].
        //..

        copyConstruct(                                           // destination
                      toEnd,
                      toEnd - numElements,  // source
                      toEnd,                // end source
                      allocator,
                      (bslmf::MetaInt<NIL_TRAITS>*)0);

        AutoArrayDestructor<TARGET_TYPE> guard(toEnd,
                                               toEnd + numElements);

        //..
        //  Transformation: ABCDEFG[EFG] => ABCABCD[EFG].
        //..

        TARGET_TYPE *src  = toEnd - numElements;
        TARGET_TYPE *dest = toEnd;
        while (toBegin != src) {
            *--dest = *--src;
        }

        //..
        //  Transformation: ABCABCD[EFG] => tuvABCD[EFG].
        //..

        for (; toBegin != dest; ++toBegin, ++fromBegin) {
            *toBegin = *fromBegin;
        }

        guard.release();
    }
    else {
        // Tail is not longer than input (numElements).

        difference_type remElements = numElements - tailLen;

        //..
        //  Transformation: ABC_______[] => ABC____[ABC]
        //..

        copyConstruct(                                           // destination
                      toBegin + numElements,
                      toBegin,                // source
                      toEnd,                  // end source
                      allocator,
                      (bslmf::MetaInt<NIL_TRAITS>*)0);

        AutoArrayDestructor<TARGET_TYPE> guard(toEnd + remElements,
                                               toEnd + numElements);

        //..
        //  Transformation: ABC____[ABC] => tuv____[ABC].
        //..

        for (; toBegin != toEnd; ++fromBegin, ++toBegin) {
            *toBegin = *fromBegin;
        }

        //..
        //  Transformation: tuv____[ABC] => tuvwxyzABC[].
        //..
        copyConstruct(toBegin,
                      fromBegin,
                      fromEnd,
                      allocator,
                      (bslmf::MetaInt<NIL_TRAITS>*)0);

        guard.release();
    }
}

                       // *** 'moveInsert' overloads: ***

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::moveInsert(
                         TARGET_TYPE                              *toBegin,
                         TARGET_TYPE                              *toEnd,
                         TARGET_TYPE                             **lastPtr,
                         TARGET_TYPE                              *first,
                         TARGET_TYPE                              *last,
                         size_type                                 numElements,
                         ALLOCATOR                                *allocator,
                         bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS>  *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin,
                                                          toEnd));
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(first, last));
    BSLS_ASSERT_SAFE(first || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));
    BSLS_ASSERT_SAFE(lastPtr);

    // Functionally indistinguishable from this:

    *lastPtr = last;
    insert(toBegin, toEnd, first, last, numElements, allocator,
           (bslmf::MetaInt<BITWISE_COPYABLE_TRAITS>*)0);
    *lastPtr = first;
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::moveInsert(TARGET_TYPE                 *toBegin,
                                     TARGET_TYPE                 *toEnd,
                                     TARGET_TYPE                **lastPtr,
                                     TARGET_TYPE                 *first,
                                     TARGET_TYPE                 *last,
                                     size_type                    numElements,
                                     ALLOCATOR                   *allocator,
                                     bslmf::MetaInt<NIL_TRAITS>  *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin,
                                                          toEnd));
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(first, last));
    BSLS_ASSERT_SAFE(first || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));
    BSLS_ASSERT_SAFE(lastPtr);

    // There isn't any advantage at destroying [first,last) one by one as we're
    // moving it, except perhaps for slightly better memory usage.

    *lastPtr = last;
    insert(toBegin, toEnd, first, last, numElements, allocator,
           (bslmf::MetaInt<NIL_TRAITS>*)0);
    ArrayDestructionPrimitives::destroy(first, last);
    *lastPtr = first;
}

                          // *** 'erase' overloads: ***

template <class TARGET_TYPE, class ALLOCATOR>
void ArrayPrimitives_Imp::erase(
                               TARGET_TYPE                             *first,
                               TARGET_TYPE                             *middle,
                               TARGET_TYPE                             *last,
                               ALLOCATOR                               *,
                               bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(first,
                                                          middle));
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(middle,
                                                          last));

    // Key to the transformation diagrams:
    //..
    //  t...z   Original contents of '[first, middle)'
    //  A...G   Original contents of '[middle, last)'
    //  _       Destroyed array element
    //..

    //..
    //  Transformation: tuvABCDEFG => ___ABCDEFG (no throw)
    //..
    ArrayDestructionPrimitives::destroy(first, middle);

    //..
    //  Transformation: ___ABCDEFG => ABCDEFG___  (might overlap, but no throw)
    //..
    size_type numBytes = (const char *)last - (const char *)middle;
    std::memmove(first, middle, numBytes);
}

template <class TARGET_TYPE, class ALLOCATOR>
void ArrayPrimitives_Imp::erase(TARGET_TYPE                *first,
                                TARGET_TYPE                *middle,
                                TARGET_TYPE                *last,
                                ALLOCATOR                  *,
                                bslmf::MetaInt<NIL_TRAITS> *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(first,
                                                          middle));
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(middle,
                                                          last));

    // Key to the transformation diagrams:
    //..
    //  t...z   Original contents of '[first, middle)'
    //  A...G   Original contents of '[middle, last)'
    //  _       Destructed array element
    //..

    //..
    //  Transformation: tuvABCDEFG => ABCDEFGEFG.
    //..

    while (middle != last) {
        *first++ = *middle++;
    }

    //..
    //  Transformation: ABCDEFGEFG => ABCDEFG___.
    //..

    ArrayDestructionPrimitives::destroy(first, middle);
}

                         // *** 'rotate' overloads: ***

template <class TARGET_TYPE>
inline
void ArrayPrimitives_Imp::rotate(
                               TARGET_TYPE                             *begin,
                               TARGET_TYPE                             *middle,
                               TARGET_TYPE                             *end,
                               bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(begin,
                                                          middle));
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(middle, end));

    bitwiseRotate((char *)begin, (char *)middle, (char *)end);
}

template <class TARGET_TYPE>
void ArrayPrimitives_Imp::rotate(TARGET_TYPE                *begin,
                                 TARGET_TYPE                *middle,
                                 TARGET_TYPE                *end,
                                 bslmf::MetaInt<NIL_TRAITS> *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(begin,
                                                          middle));
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(middle, end));

    if (begin == middle || middle == end) {
        // This test changes into O(1) what would otherwise be O(N): Do not
        // remove!

        return;                                                       // RETURN
    }

    // This case is simple enough, it should be taken care of on its own.

    const std::size_t numElements = middle - begin;
    const std::size_t remElements = end - middle;

    if (numElements == remElements) {
        for (; middle != end; ++begin, ++middle) {
            TARGET_TYPE tmp(*middle);
            *middle = *begin;
            *begin = tmp;
        }
        return;                                                       // RETURN
    }

    // This algorithm proceeds by decomposing the rotation into cycles, which
    // can then be rotated using a single element buffer.  First we compute the
    // 'gcd(end - begin, numElements)' which is the number of cycles in the
    // rotation.

    std::size_t numCycles = end - begin;
    std::size_t remainder = numElements;
    while (remainder != 0) {
        std::size_t t = numCycles % remainder;
        numCycles = remainder;
        remainder = t;
    }

    // This algorithm is directly taken from stlport's implementation for
    // RandomAccessIterator.

    // Key to the transformation diagrams:
    //..
    //  A...D   Contents of the current cycle
    //  W...Z   Contents of another cycle
    //  _       Elements not in the current cycle
    //..

    for (std::size_t i = 0; i < numCycles; ++i) {
        // Let the current cycle be initially 'A__B__C__D__', (note that its
        // stride is 'length / numCycles') and let (*) denote the current
        // position of 'ptr'.

        TARGET_TYPE *ptr = begin; // seed for current cycle:  A(*)__B__C__D__
        TARGET_TYPE  tmp = *ptr;  // value held at the seed:  tmp == A

        if (numElements < remElements) {
            // Rotate the cycle forward by numElements positions (or backward
            // by -(length-numElements)=-remElements positions if crossing the
            // boundary forward).  The transformation is:
            //..
            //  A(*)__B__C__D__ => B__B(*)__C__D__
            //                  => B__C__C(*)__D__
            //                  => B__C__D__D(*)__
            //..
            // The length of the cycle is always 'length / numCycles', but it
            // crosses the range boundaries 'numElements / numCycles' times,
            // each triggering an extra assignment in the 'if' clause below, so
            // the loop must only be executed:
            //..
            //  (length - numElements) / numCycles = remELements / numCycles
            //..
            // times.

            std::size_t cycleSize = remElements / numCycles;

            for (std::size_t j = 0; j < cycleSize; ++j) {
                if (ptr > begin + remElements) {
                    // Wrap around the range boundaries.  (Note that
                    // '-remElements == numElements - (end - begin)'.)

                    *ptr = *(ptr - remElements);
                    ptr -= remElements;
                }

                *ptr = *(ptr + numElements);
                ptr += numElements;
            }
        }
        else {
            // Rotate the cycle backward by '-remElements' positions (or
            // forward by 'numElements' positions if crossing the boundary
            // backward).  The transformation is:
            //..
            //  A(*)__B__C__D__ => D__B__C__D(*)__
            //                  => D__B__C(*)__C__
            //                  => D__B(*)__B__C__
            //..
            // The length of the cycle is always 'length/numCycles', but going
            // backward (which adds an initial extra crossing) crosses the
            // range boundaries 'remElements/numCycles+1' times each of which
            // trigger an extra assignment in the 'if' clause below, so the
            // loop must only be executed:
            //..
            //  (length - remElements) / numCycles - 1 =
            //                                      numELements / numCycles - 1
            //..
            // times.

            std::size_t cycleSize = numElements / numCycles - 1;

            for (std::size_t j = 0; j < cycleSize; ++j) {
                if (ptr < end - numElements) {
                    *ptr = *(ptr + numElements);
                    ptr += numElements;
                }

                *ptr = *(ptr - remElements);
                ptr -= remElements;
            }
        }

        *ptr = tmp; // Close the cycle, e.g.:
                    //..
                    //  (first case):  B__C__D__D(*)__ => B__C__D__A__
                    //  (second case): D__D(*)__B__C__ => D__A__B__C__
                    //..
        ++begin;    // and move on to the next cycle:
                    //..
                    //                                 => _W__X__Y__Z_
                    //..
    }
}

template <class FORWARD_ITERATOR>
bool ArrayPrimitives_Imp::isInvalidRange(FORWARD_ITERATOR,
                                         FORWARD_ITERATOR)
{
    // Ideally would dispatch on random_access_iterator_tag to support
    // generalized random access iterators, but we are constrained by 'bsl'
    // levelization to not depend on 'bsl_iterator.h'.  As the intent is to
    // detect invalid ranges in assertions, the conservative choice is to
    // return 'false' always.

    return false;
}

template <class TARGET_TYPE>
bool ArrayPrimitives_Imp::isInvalidRange(TARGET_TYPE *begin,
                                         TARGET_TYPE *end)
{
    return !begin != !end || begin > end;
}

}  // close package namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

typedef bslalg::ArrayPrimitives bslalg_ArrayPrimitives;
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
