// bslalg_arrayprimitives.h                                           -*-C++-*-
#ifndef INCLUDED_BSLALG_ARRAYPRIMITIVES
#define INCLUDED_BSLALG_ARRAYPRIMITIVES

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide primitive algorithms that operate on arrays.
//
//@CLASSES:
//  bslalg::ArrayPrimitives: namespace for array algorithms
//
//@SEE_ALSO: bslalg_dequeprimitives, bslma_constructionutil
//
//@DESCRIPTION: This component provides utilities to initialize, move, and
// otherwise perform various primitive manipulations on arrays with a uniform
// interface, but selecting a different implementation according to the various
// traits possessed by the underlying type.  Such primitives are exceptionally
// useful for implementing generic components such as containers.
//
// Several algorithms are provided, with the following short synopsis
// describing the observable behavior and mentioning the relevant traits.  See
// the full function-level contract for detailed description, including
// exception-safety guarantees.  In the description below, 'ADP' stands for
// 'bslalg::ArrayDestructionPrimitives'.  Note that some algorithms (e.g.,
// 'insert') are explained in terms of previous algorithms (e.g.,
// 'destructiveMove').
//..
//  Algorithm                     Short description of observable behavior
//  ----------------------------  ---------------------------------------------
//  defaultConstruct              Construct each element in the target range
//                                by value-initialization, or 'std::memset' if
//                                type has a trivial default constructor.
//                                Note that this function *does* *not* perform
//                                default-initialization, the colloquial
//                                terminology "default construct" is maintained
//                                for backwards compatibility.
//
//  uninitializedFillN            Copy construct from value for each element in
//                                the target range, or 'std::memset' if value
//                                is all 0s or 1s bits, and type is bit-wise
//                                copyable
//
//  copyConstruct                 Copy construct from each element in the
//                                original range to the corresponding element
//                                in the target range, or 'std::memcpy' if
//                                value is null and type is bit-wise copyable
//
//  destructiveMove               Copy from each element in the original range
//                                to the corresponding element in the target
//                                and destroy objects in the original range, or
//                                'std::memcpy' if type is bit-wise moveable
//
//  destructiveMoveAndInsert      'destructiveMove' from the original range to
//                                target range, leaving a hole in the middle,
//                                followed by 'defaultConstruct',
//                                'uninitializedFillN' or 'copyConstruct' to
//                                fill hole with the appropriate values
//
//  destructiveMoveAndMoveInsert  'destructiveMove' from the original range to
//                                the target range, leaving a hole in the
//                                middle, followed by 'destructiveMove'
//                                from second range to fill hole
//
//  insert                        'std::memmove' or 'copyConstruct' by some
//                                positive offset to create a hole, followed by
//                                'uninitializedFillN', 'copyConstruct', or
//                                copy assignment to fill hole with the
//                                appropriate values
//
//  emplace                       'std::memmove' or 'copyConstruct' by some
//                                positive offset to create a hole, followed by
//                                in-place construction, 'copyConstruct', or
//                                copy assignment to fill hole with the
//                                appropriate values
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
//  rotate                        'destructiveMove' to move elements into a
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
//
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
// 'bslmf::IsBitwiseMoveable' trait, moving an element in a vector can be done
// using 'memcpy' instead of copy construction.
//
// We can utilize the class methods provided by 'bslalg::ArrayPrimitives' to
// satisfy the above requirements.  Unlike 'bslma::ConstructionUtil', which
// operates on a single element, 'bslalg::ArrayPrimitives' operates on arrays,
// which will further help simplify our implementation.
//
// First, we create an elided definition of the class template 'MyVector':
//..
//  template <class TYPE, class ALLOC>
//  class MyVector {
//      // This class implements a vector of elements of the (template
//      // parameter) 'TYPE', which must be copy constructible.  Note that for
//      // the brevity of the usage example, this class does not provide any
//      // Exception-Safety guarantee.
//
//      // DATA
//      TYPE             *d_array_p;     // pointer to the allocated array
//      int               d_capacity;    // capacity of the allocated array
//      int               d_size;        // number of objects
//      ALLOC             d_allocator;   // allocator pointer (held, not owned)
//
//    public:
//      // TYPE TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(
//          MyVector,
//          BloombergLP::bslmf::IsBitwiseMoveable);
//
//      // CREATORS
//      explicit MyVector(bslma::Allocator *basicAllocator = 0)
//          // Construct a 'MyVector' object having a size of 0 and and a
//          // capacity of 0.  Optionally specify a 'basicAllocator' used to
//          // supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//      : d_array_p(0)
//      , d_capacity(0)
//      , d_size(0)
//      , d_allocator_p(bslma::Default::allocator(basicAllocator))
//      {
//      }
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
//      void reserve(int minCapacity);
//          // Change the capacity of this vector to at least the specified
//          // 'minCapacity' if it is greater than the vector's current
//          // capacity.
//
//      void insert(int dstIndex, int numElements, const TYPE& value);
//          // Insert, into this vector, the specified 'numElements' of the
//          // specified 'value' at the specified 'dstIndex'.  The behavior is
//          // undefined unless '0 <= dstIndex <= size()'.
//
//      // ACCESSORS
//      const TYPE& operator[](int position) const
//          // Return a reference providing non-modifiable access to the
//          // element at the specified 'position' in this vector.
//      {
//          return d_array_p[position];
//      }
//
//      int size() const
//          // Return the size of this vector.
//      {
//          return d_size;
//      }
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
//      reserve(original.d_size);
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
//
//      d_size = original.d_size;
//  }
//..
// Now, we implement the 'reserve' method of 'MyVector':
//..
//  template <class TYPE>
//  void MyVector<TYPE>::reserve(int minCapacity)
//  {
//      if (d_capacity >= minCapacity) return;                        // RETURN
//
//      TYPE *newArrayPtr = static_cast<TYPE*>(d_allocator_p->allocate(
//      BloombergLP::bslma::Allocator::size_type(minCapacity * sizeof(TYPE))));
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
//      d_capacity = minCapacity;
//  }
//..
// Finally, we implement the 'insert' method of 'MyVector':
//..
//  template <class TYPE>
//  void
//  MyVector<TYPE>::insert(int dstIndex, int numElements, const TYPE& value)
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

#include <bslscm_version.h>

#include <bslalg_arraydestructionprimitives.h>
#include <bslalg_autoarraydestructor.h>
#include <bslalg_autoarraymovedestructor.h>

#include <bslma_allocator.h>
#include <bslma_allocatortraits.h>
#include <bslma_constructionutil.h>
#include <bslma_default.h>
#include <bslma_destructorproctor.h>
#include <bslma_stdallocator.h>

#include <bslmf_assert.h>
#include <bslmf_functionpointertraits.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isconvertible.h>
#include <bslmf_isenum.h>
#include <bslmf_isfundamental.h>
#include <bslmf_ismemberpointer.h>
#include <bslmf_ispointer.h>
#include <bslmf_issame.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_isvoid.h>
#include <bslmf_matchanytype.h>
#include <bslmf_metaint.h>
#include <bslmf_removeconst.h>
#include <bslmf_removecv.h>
#include <bslmf_removepointer.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_objectbuffer.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#include <cstddef>  // 'std::size_t'
#include <cstring>  // 'memset', 'memcpy', 'memmove'
#include <cwchar>   // 'wmemset'

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslalg_constructorproxy.h>
#endif

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Thu Oct 21 10:11:37 2021
// Command line: sim_cpp11_features.pl bslalg_arrayprimitives.h
# define COMPILING_BSLALG_ARRAYPRIMITIVES_H
# include <bslalg_arrayprimitives_cpp03.h>
# undef COMPILING_BSLALG_ARRAYPRIMITIVES_H
#else

#if defined(BSLS_PLATFORM_CMP_IBM)      // IBM needs specific workarounds.
# define BSLALG_ARRAYPRIMITIVES_CANNOT_REMOVE_POINTER_FROM_FUNCTION_POINTER 1
    // xlC has problem removing pointer from function pointer types.

# define BSLALG_ARRAYPRIMITIVES_NON_ZERO_NULL_VALUE_FOR_MEMBER_POINTERS 1
    // xlC representation for a null member pointer is not all zero bits.
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
    // and copy constructors, destructor, assignment operators, etcetera may
    // not be invoked, optimized away by no-op or bit-wise move or copy.

  public:
    // TYPES
    typedef ArrayPrimitives_Imp         Imp;
    typedef std::size_t                 size_type;
    typedef std::ptrdiff_t              difference_type;

    // CLASS METHODS
    template <class ALLOCATOR, class FWD_ITER>
    static void
    copyConstruct(
                 typename bsl::allocator_traits<ALLOCATOR>::pointer toBegin,
                 FWD_ITER                                           fromBegin,
                 FWD_ITER                                           fromEnd,
                 ALLOCATOR                                          allocator);
    template <class ALLOCATOR, class SOURCE_TYPE>
    static void
    copyConstruct(
                typename bsl::allocator_traits<ALLOCATOR>::pointer  toBegin,
                SOURCE_TYPE                                        *fromBegin,
                SOURCE_TYPE                                        *fromEnd,
                ALLOCATOR                                           allocator);
        // Copy the elements of type 'allocator_traits<ALLOCATOR>::value_type'
        // in the range beginning at the specified 'fromBegin' location and
        // ending immediately before the specified 'fromEnd' location into the
        // uninitialized array beginning at the specified 'toBegin' location,
        // using the specified 'allocator' to supply memory (if required).  If
        // a constructor throws an exception during this operation, the output
        // array is left in an uninitialized state.  The behavior is undefined
        // unless 'toBegin' refers to space sufficient to hold
        // 'fromEnd - fromBegin' elements.

    template <class TARGET_TYPE, class FWD_ITER>
    static void copyConstruct(TARGET_TYPE      *toBegin,
                              FWD_ITER          fromBegin,
                              FWD_ITER          fromEnd,
                              bslma::Allocator *allocator);
    template <class TARGET_TYPE, class SOURCE_TYPE>
    static void copyConstruct(TARGET_TYPE      *toBegin,
                              SOURCE_TYPE      *fromBegin,
                              SOURCE_TYPE      *fromEnd,
                              bslma::Allocator *allocator);
        // Copy into an uninitialized array of (the template parameter)
        // 'TARGET_TYPE' beginning at the specified 'toBegin' address, the
        // elements in the array of 'TARGET_TYPE' starting at the specified
        // 'fromBegin' address and ending immediately before the specified
        // 'fromEnd' address.  If the (template parameter) 'ALLOCATOR' type is
        // derived from 'bslma::Allocator' and 'TARGET_TYPE' supports 'bslma'
        // allocators, then the specified 'allocator' is passed to each
        // invocation of the 'TARGET_TYPE' copy constructor.  If a
        // 'TARGET_TYPE' constructor throws an exception during the operation,
        // then the destructor is called on any newly-constructed elements,
        // leaving the output array in an uninitialized state.

    template <class ALLOCATOR>
    static void
    moveConstruct(
                 typename bsl::allocator_traits<ALLOCATOR>::pointer toBegin,
                 typename bsl::allocator_traits<ALLOCATOR>::pointer fromBegin,
                 typename bsl::allocator_traits<ALLOCATOR>::pointer fromEnd,
                 ALLOCATOR                                          allocator);
        // Move the elements of type 'allocator_traits<ALLOCATOR>::value_type'
        // in the range beginning at the specified 'fromBegin' location and
        // ending immediately before the specified 'fromEnd' location into the
        // uninitialized array beginning at the specified 'toBegin' location,
        // using the specified 'allocator' to supply memory (if required).  The
        // elements in the input array are left in a valid but unspecified
        // state.  If a constructor throws an exception during this operation,
        // the output array is left in an uninitialized state.  The behavior is
        // undefined unless 'toBegin' refers to space sufficient to hold
        // 'fromEnd - fromBegin' elements.

    template <class TARGET_TYPE>
    static void moveConstruct(TARGET_TYPE      *toBegin,
                              TARGET_TYPE      *fromBegin,
                              TARGET_TYPE      *fromEnd,
                              bslma::Allocator *allocator);
        // Move the elements of the (template parameter) 'TARGET_TYPE' starting
        // at the specified 'fromBegin' address and ending immediately before
        // the specified 'fromEnd' address into the uninitialized array of
        // 'TARGET_TYPE' beginning at the specified 'toBegin' address, using
        // the specified 'allocator' to supply memory (if required).  The
        // elements in the input array are left in a valid but unspecified
        // state.  If a constructor throws an exception during this operation,
        // the output array is left in an uninitialized state.  The behavior is
        // undefined unless 'toBegin' refers to space sufficient to hold
        // 'fromEnd - fromBegin' elements.

    template <class ALLOCATOR>
    static void defaultConstruct(
               typename bsl::allocator_traits<ALLOCATOR>::pointer  begin,
               size_type                                           numElements,
               ALLOCATOR                                           allocator);
        // Value-inititalize the specified 'numElements' objects of type
        // 'allocator_traits<ALLOCATOR>::value_type' into the uninitialized
        // array beginning at the specified 'begin' location, using the
        // specified 'allocator' to supply memory (if required).  If a
        // constructor throws an exception during this operation, then the
        // destructor is called on any newly constructed elements, leaving the
        // output array in an uninitialized state.  The behavior is undefined
        // unless the 'begin' refers to space sufficient to hold 'numElements'.

    template <class TARGET_TYPE>
    static void defaultConstruct(TARGET_TYPE      *begin,
                                 size_type         numElements,
                                 bslma::Allocator *allocator);
        // Construct each of the elements of an array of the specified
        // 'numElements' of the parameterized 'TARGET_TYPE' starting at the
        // specified 'begin' address by value-initialization.  If the (template
        // parameter) 'ALLOCATOR' type is derived from 'bslma::Allocator' and
        // 'TARGET_TYPE' supports 'bslma' allocators, then the specified
        // 'allocator' is passed to each 'TARGET_TYPE' default constructor
        // call.  The behavior is undefined unless the output array contains at
        // least 'numElements' uninitialized elements after 'begin'.  If a
        // 'TARGET_TYPE' constructor throws an exception during this operation,
        // then the destructor is called on any newly-constructed elements,
        // leaving the output array in an uninitialized state.

    template <class ALLOCATOR>
    static void destructiveMove(
                 typename bsl::allocator_traits<ALLOCATOR>::pointer toBegin,
                 typename bsl::allocator_traits<ALLOCATOR>::pointer fromBegin,
                 typename bsl::allocator_traits<ALLOCATOR>::pointer fromEnd,
                 ALLOCATOR                                          allocator);
        // Move the elements of type 'allocator_traits<ALLOCATOR>::value_type'
        // in the range beginning at the specified 'fromBegin' location and
        // ending immediately before the specified 'fromEnd' location into the
        // uninitialized array beginning at the specified 'toBegin' location,
        // using the specified 'allocator' to supply memory (if required).  On
        // return, the elements in the input range are invalid, i.e., their
        // destructors must not be called after this operation returns.  If a
        // constructor throws an exception during this operation, the output
        // array is left in an uninitialized state.  If a constructor other
        // than the move constructor of a non-copy-constructible type throws
        // an exception during this operation, the input array is unaffected;
        // otherwise, if the move constructor of a non-copy-constructible type
        // throws an exception during this operation, the input array is left
        // in a valid but unspecified state.  The behavior is undefined unless
        // 'toBegin' refers to space sufficient to hold 'fromEnd - fromBegin'
        // elements.

    template <class TARGET_TYPE>
    static void destructiveMove(TARGET_TYPE      *toBegin,
                                TARGET_TYPE      *fromBegin,
                                TARGET_TYPE      *fromEnd,
                                bslma::Allocator *allocator);
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

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=10

    template <class ALLOCATOR, class... ARGS>
    static void destructiveMoveAndEmplace(
                typename bsl::allocator_traits<ALLOCATOR>::pointer  toBegin,
                typename bsl::allocator_traits<ALLOCATOR>::pointer *fromEndPtr,
                typename bsl::allocator_traits<ALLOCATOR>::pointer  fromBegin,
                typename bsl::allocator_traits<ALLOCATOR>::pointer  position,
                typename bsl::allocator_traits<ALLOCATOR>::pointer  fromEnd,
                ALLOCATOR                                           allocator,
                ARGS&&...                                           arguments);
        // Move the elements of type 'allocator_traits<ALLOCATOR>::value_type'
        // in the specified range '[fromBegin .. fromEnd)' into the
        // uninitialized array beginning at the specified 'toBegin' location,
        // using the specified 'allocator' to supply memory (if required),
        // inserting at the specified 'position' (after translating from
        // 'fromBegin' to 'toBegin') a newly created object constructed by
        // forwarding 'allocator' (if required) and the specified (variable
        // number of) 'arguments' to the corresponding constructor of the
        // target type, ensuring that the specified 'fromEndPtr' points to the
        // first uninitialized element in '[fromBegin .. fromEnd)' as the
        // elements are moved from source to destination.  On return, the
        // elements in the input range are invalid, i.e., their destructors
        // must not be called after this operation returns.  If a constructor
        // throws an exception during this operation, the output array is left
        // in an uninitialized state.  If an exception is thrown during the
        // in-place construction of the new object, the input array is
        // unaffected; otherwise, if a (copy or move) constructor throws an
        // exception during this operation, the input elements in the range
        // '[fromBegin .. *fromEndPtr)' are left in a valid but unspecified
        // state and the remaining portion of the input array is left in an
        // uninitialized state.  The behavior is undefined unless
        // 'fromBegin <= position <= fromEnd' and 'toBegin' refers to space
        // sufficient to hold 'fromEnd - fromBegin + 1' elements.

#endif

    template <class ALLOCATOR>
    static void destructiveMoveAndInsert(
               typename bsl::allocator_traits<ALLOCATOR>::pointer  toBegin,
               typename bsl::allocator_traits<ALLOCATOR>::pointer *fromEndPtr,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  fromBegin,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  position,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  fromEnd,
               size_type                                           numElements,
               ALLOCATOR                                           allocator);
        // Move the elements of type 'allocator_traits<ALLOCATOR>::value_type'
        // in the range beginning at the specified 'fromBegin' location and
        // ending immediately before the specified 'fromEnd' location into the
        // uninitialized array beginning at the specified 'toBegin' location
        // using the specified 'allocator' to supply memory (if required),
        // inserting at the specified 'position' (after translating from
        // 'fromBegin' to 'toBegin') the specified 'numElements' objects
        // initialized to default values, ensuring that the specified
        // 'fromEndPtr' points to the first uninitialized element in
        // '[fromBegin .. fromEnd)' as the elements are moved from source to
        // destination.  On return, the elements in the input range are
        // invalid, i.e., their destructors must not be called after this
        // operation returns.  If a constructor throws an exception during this
        // operation, the output array is left in an uninitialized state.  If a
        // default constructor throws an exception, the input array is
        // unaffected; otherwise, if a (copy or move) constructor throws an
        // exception during this operation, the input elements in the range
        // '[fromBegin .. *fromEndPtr)' are left in a valid but unspecified
        // state and the remaining portion of the input array is left in an
        // uninitialized state.  The behavior is undefined unless 'fromBegin <=
        // position <= fromEnd' and 'toBegin' refers to space sufficient to
        // hold 'fromEnd - fromBegin + 1' elements.

    template <class TARGET_TYPE>
    static void destructiveMoveAndInsert(TARGET_TYPE         *toBegin,
                                         TARGET_TYPE        **fromEndPtr,
                                         TARGET_TYPE         *fromBegin,
                                         TARGET_TYPE         *position,
                                         TARGET_TYPE         *fromEnd,
                                         size_type            numElements,
                                         bslma::Allocator    *allocator);
        // Move the elements of the (template parameter) 'TARGET_TYPE' in the
        // starting at the specified 'fromBegin' address and ending immediately
        // before the specified 'fromEnd' address into the uninitialized array
        // beginning at the specified 'toBegin' location using the specified
        // 'allocator' to supply memory (if required), inserting at the
        // specified 'position' (after translating from 'fromBegin' to
        // 'toBegin') 'numElements' objects initialized to default values,
        // ensuring that the specified 'fromEndPtr' points to the first
        // uninitialized element in '[fromBegin .. fromEnd)' as the elements
        // are moved from source to destination.  On return, the elements in
        // the input range are invalid, i.e., their destructors must not be
        // called after this operation returns.  If a constructor throws an
        // exception during this operation, the output array is left in an
        // uninitialized state.  If a default constructor throws an exception,
        // the input array is unaffected; otherwise, if a (copy or move)
        // constructor throws an exception during this operation, the input
        // elements in the range '[fromBegin .. *fromEndPtr)' are left in a
        // valid but unspecified state and the remaining portion of the input
        // array is left in an uninitialized state.  The behavior is undefined
        // unless 'fromBegin <= position <= fromEnd' and 'toBegin' refers to
        // space sufficient to hold 'fromEnd - fromBegin + numElements'
        // elements.

    template <class ALLOCATOR>
    static void destructiveMoveAndInsert(
     typename bsl::allocator_traits<ALLOCATOR>::pointer            toBegin,
     typename bsl::allocator_traits<ALLOCATOR>::pointer           *fromEndPtr,
     typename bsl::allocator_traits<ALLOCATOR>::pointer            fromBegin,
     typename bsl::allocator_traits<ALLOCATOR>::pointer            position,
     typename bsl::allocator_traits<ALLOCATOR>::pointer            fromEnd,
     const typename bsl::allocator_traits<ALLOCATOR>::value_type&  value,
     size_type                                                     numElements,
     ALLOCATOR                                                     allocator);
        // Move the elements of type 'allocator_traits<ALLOCATOR>::value_type'
        // in the range beginning at the specified 'fromBegin' location and
        // ending immediately before the specified 'fromEnd' location into the
        // uninitialized array beginning at the specified 'toBegin' location
        // using the specified 'allocator' to supply memory (if required),
        // inserting at the specified 'position' (after translating from
        // 'fromBegin' to 'toBegin') the specified 'numElements' copies of the
        // specified 'value', ensuring that the specified 'fromEndPtr' points
        // to the first uninitialized element in '[fromBegin .. fromEnd)' as
        // the elements are moved from source to destination.  On return, the
        // elements in the input range are invalid, i.e., their destructors
        // must not be called after this operation returns.  If a constructor
        // throws an exception during this operation, the output array is left
        // in an uninitialized state.  If a (copy or move) constructor throws
        // an exception during this operation, the input elements in the range
        // '[fromBegin .. *fromEndPtr)' are left in a valid but unspecified
        // state and the remaining portion of the input array is left in an
        // uninitialized state.  The behavior is undefined unless
        // 'fromBegin <= position <= fromEnd' and 'toBegin' refers to space
        // sufficient to hold 'fromEnd - fromBegin + numElements' elements.

    template <class TARGET_TYPE>
    static void destructiveMoveAndInsert(TARGET_TYPE         *toBegin,
                                         TARGET_TYPE        **fromEndPtr,
                                         TARGET_TYPE         *fromBegin,
                                         TARGET_TYPE         *position,
                                         TARGET_TYPE         *fromEnd,
                                         const TARGET_TYPE&   value,
                                         size_type            numElements,
                                         bslma::Allocator    *allocator);
        // Move the elements of the parameterized 'TARGET_TYPE' in the array
        // starting at the specified 'fromBegin' address and ending immediately
        // before the specified 'fromEnd' address into an uninitialized array
        // of 'TARGET_TYPE' at the specified 'toBegin' address, inserting at
        // the specified 'position' (after translating from 'fromBegin' to
        // 'toBegin') the specified 'numElements' copies of the specified
        // 'value'.  Keep the pointer at the specified 'fromEndPtr' address
        // pointing to the first uninitialized element in '[ fromBegin,
        // fromEnd)' as the elements are moved from source to destination.  The
        // behavior is undefined unless 'fromBegin <= position <= fromEnd' and
        // the destination array contains at least
        // '(fromEnd - fromBegin) + numElements' uninitialized elements.  If a
        // copy constructor or assignment operator for 'TARGET_TYPE' throws an
        // exception, then any elements created in the output array are
        // destroyed and the elements in the range '[ fromBegin, *fromEndPtr )'
        // will have unspecified but valid values.

    template <class ALLOCATOR, class FWD_ITER>
    static void destructiveMoveAndInsert(
               typename bsl::allocator_traits<ALLOCATOR>::pointer  toBegin,
               typename bsl::allocator_traits<ALLOCATOR>::pointer *fromEndPtr,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  fromBegin,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  position,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  fromEnd,
               FWD_ITER                                            first,
               FWD_ITER                                            last,
               size_type                                           numElements,
               ALLOCATOR                                           allocator);
        // Move the elements of type 'allocator_traits<ALLOCATOR>::value_type'
        // in the range beginning at the specified 'fromBegin' location and
        // ending immediately before the specified 'fromEnd' location into the
        // uninitialized array beginning at the specified 'toBegin' location
        // using the specified 'allocator' to supply memory (if required),
        // inserting at the specified 'position' (after translating from
        // 'fromBegin' to 'toBegin') the specified 'numElements' copies of the
        // non-modifiable elements from the range starting at the specified
        // 'first' iterator of (template parameter) type 'FWD_ITER' and ending
        // immediately before the specified 'last' iterator, ensuring that the
        // specified 'fromEndPtr' points to the first uninitialized element in
        // '[fromBegin .. fromEnd)' as the elements are moved from source to
        // destination.  On return, the elements in the input range are
        // invalid, i.e., their destructors must not be called after this
        // operation returns.  If a constructor throws an exception during this
        // operation, the output array is left in an uninitialized state.  If
        // a constructor other than the copy or move constructor throws an
        // exception during this operation, the input array is unaffected;
        // otherwise, if a copy or move constructor throws an exception during
        // this operation, the input elements in the range
        // '[fromBegin .. *fromEndPtr)' are left in a valid but unspecified
        // state and the remaining portion of the input array is left in an
        // uninitialized state.  The behavior is undefined unless
        // 'fromBegin <= position <= fromEnd' and 'toBegin' refers to space
        // sufficient to hold 'fromEnd - fromBegin + numElements' elements.

    template <class TARGET_TYPE, class FWD_ITER>
    static void destructiveMoveAndInsert(TARGET_TYPE       *toBegin,
                                         TARGET_TYPE      **fromEndPtr,
                                         TARGET_TYPE       *fromBegin,
                                         TARGET_TYPE       *position,
                                         TARGET_TYPE       *fromEnd,
                                         FWD_ITER           first,
                                         FWD_ITER           last,
                                         size_type          numElements,
                                         bslma::Allocator  *allocator);
        // Move the elements of the parameterized 'TARGET_TYPE' in the array
        // starting at the specified 'fromBegin' address and ending immediately
        // before the specified 'fromEnd' address into an uninitialized array
        // of 'TARGET_TYPE' at the specified 'toBegin' address, inserting at
        // the specified 'position' (after translating from 'fromBegin' to
        // 'toBegin') the specified 'numElements' copies of the non-modifiable
        // elements from the range starting at the specified 'first' iterator
        // of the parameterized 'FWD_ITER' type and ending immediately before
        // the specified 'last' iterator.  Keep the pointer at the specified
        // 'fromEndPtr' to point to the first uninitialized element in
        // '[fromBegin, fromEnd)' as the elements are moved from source to
        // destination.  The behavior is undefined unless
        // 'fromBegin <= position <= fromEnd', the destination array contains
        // at least '(fromEnd - fromBegin) + numElements' uninitialized
        // elements after 'toBegin', and 'numElements' is the distance from
        // 'first' to 'last'.  If a copy constructor or assignment operator for
        // 'TARGET_TYPE' throws an exception, then any elements created in the
        // output array are destroyed and the elements in the range
        // '[ fromBegin, *fromEndPtr )' will have unspecified but valid values.

    template <class ALLOCATOR>
    static void destructiveMoveAndMoveInsert(
               typename bsl::allocator_traits<ALLOCATOR>::pointer  toBegin,
               typename bsl::allocator_traits<ALLOCATOR>::pointer *fromEndPtr,
               typename bsl::allocator_traits<ALLOCATOR>::pointer *lastPtr,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  fromBegin,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  position,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  fromEnd,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  first,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  last,
               size_type                                           numElements,
               ALLOCATOR                                           allocator);
        // TBD: improve comment
        // Move, into an uninitialized array beginning at the specified
        // 'toBegin' pointer, elements of type given by the 'allocator_traits'
        // class template for (template parameter) 'ALLOCATOR', from elements
        // starting at the specified 'fromBegin' pointer and ending immediately
        // before the specified 'fromEnd' address, moving into the specified
        // 'position' (after translating from 'fromBegin' to 'toBegin') the
        // specified 'numElements' elements starting at the specified 'first'
        // pointer and ending immediately before the specified 'last' pointer.
        // Keep the pointer at the specified 'fromEndPtr' address pointing to
        // the first uninitialized element in '[ fromBegin, fromEnd)' as the
        // elements are moved from source to destination.  The behavior is
        // undefined unless 'fromBegin <= position <= fromEnd' and the
        // destination array contains at least
        // '(fromEnd - fromBegin) + numElements' uninitialized elements.  If a
        // constructor or assignment operator for the target type throws an
        // exception, then any elements created in the output array are
        // destroyed and the elements in the range '[ fromBegin, *fromEndPtr )'
        // will have valid but unspecified values.

    template <class TARGET_TYPE>
    static void destructiveMoveAndMoveInsert(TARGET_TYPE       *toBegin,
                                             TARGET_TYPE      **fromEndPtr,
                                             TARGET_TYPE      **lastPtr,
                                             TARGET_TYPE       *fromBegin,
                                             TARGET_TYPE       *position,
                                             TARGET_TYPE       *fromEnd,
                                             TARGET_TYPE       *first,
                                             TARGET_TYPE       *last,
                                             size_type          numElements,
                                             bslma::Allocator  *allocator);
        // Move the elements of (template parameter) 'TARGET_TYPE' in the array
        // starting at the specified 'fromBegin' address and ending immediately
        // before the specified 'fromEnd' address into an uninitialized array
        // of 'TARGET_TYPE' at the specified 'toBegin' address, moving into the
        // specified 'position' (after translating from 'fromBegin' to
        // 'toBegin') the specified 'numElements' of the 'TARGET_TYPE' from the
        // array starting at the specified 'first' address and ending
        // immediately before the specified 'last' address.  Keep the pointer
        // at the specified 'fromEndPtr' address pointing to the first
        // uninitialized element in '[fromBegin, fromEnd)', and the pointer at
        // the specified 'lastPtr' address pointing to the end of the moved
        // range as the elements from the range '[ first, last)' are moved from
        // source to destination.  The behavior is undefined unless
        // 'fromBegin <= position <= fromEnd', the destination array contains
        // at least '(fromEnd - fromBegin) + numElements' uninitialized
        // elements after 'toBegin', and 'numElements' is the distance from
        // 'first' to 'last'.  If a copy constructor or assignment operator for
        // 'TARGET_TYPE' throws an exception, then any elements in
        // '[ *lastPtr, last )' as well as in '[ toBegin, ... )' are destroyed,
        // and the elements in the ranges '[ first, *lastPtr )' and
        // '[ fromBegin, *fromEndPtr )' will have unspecified but valid values.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=10
    template <class ALLOCATOR, class... ARGS>
    static void emplace(
                typename bsl::allocator_traits<ALLOCATOR>::pointer  toBegin,
                typename bsl::allocator_traits<ALLOCATOR>::pointer  toEnd,
                ALLOCATOR                                           allocator,
                ARGS&&...                                           arguments);
        // Insert a newly created 'allocator_traits<ALLOCATOR>::value_type'
        // object, constructed by forwarding the specified 'allocator' (if
        // required) and the specified (variable number of) 'arguments' to the
        // corresponding constructor of
        // 'allocator_traits<ALLOCATOR>::value_type', into the array at the
        // specified 'toBegin' location, shifting forward the elements from
        // 'toBegin' to the specified 'toEnd' location by one position.  If an
        // exception is thrown during the in-place construction of the new
        // object, the elements in the range '[toBegin .. toEnd)' are
        // unaffected; otherwise, if a (copy or move) constructor or a (copy or
        // move) assignment operator throws an exception, then any elements
        // created after 'toEnd' are destroyed and the elements in the range
        // '[toBegin .. toEnd )' are left in a valid but unspecified state.
        // The behavior is undefined unless 'toBegin' refers to sufficient
        // space to hold at least 'toEnd - toBegin + 1' elements.

    template <class TARGET_TYPE, class... ARGS>
    static void emplace(TARGET_TYPE               *toBegin,
                        TARGET_TYPE               *toEnd,
                        bslma::Allocator          *allocator,
                        ARGS&&...                  args);
        // Insert a newly created object of the (template parameter) type
        // 'TARGET_TYPE', constructed by forwarding the specified 'allocator'
        // (if required) and the specified (variable number of) 'arguments' to
        // the corresponding constructor of 'TARGET_TYPE', into the array at
        // the specified 'toBegin' address, shifting the elements from
        // 'toBegin' to the specified 'toEnd' address up one position towards
        // larger addresses.  If an exception is thrown during the in-place
        // construction of the new object, the elements in the range
        // '[toBegin .. toEnd)' are unaffected; otherwise, if a (copy or move)
        // constructor or a (copy or move) assignment operator throws an
        // exception, then any elements created after 'toEnd' are destroyed and
        // the elements in the range '[toBegin .. toEnd )' are left in a valid
        // but unspecified state.  The behavior is undefined unless 'toBegin'
        // refers to sufficient space to hold at least 'toEnd - toBegin + 1'
        // elements.

#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
    template <class CLASS_TYPE, class MEMBER_TYPE, class... ARGS>
    static void emplace(MEMBER_TYPE CLASS_TYPE::* *toBegin,
                        MEMBER_TYPE CLASS_TYPE::* *toEnd,
                        bslma::Allocator          *allocator)
        // Old Microsoft compilers need help value-initializing elements that
        // are pointer-to-member types.
    {
        emplace(toBegin, toEnd, allocator, nullptr);
    }
#endif

    template <class ALLOCATOR>
    static void
    erase(typename bsl::allocator_traits<ALLOCATOR>::pointer first,
          typename bsl::allocator_traits<ALLOCATOR>::pointer middle,
          typename bsl::allocator_traits<ALLOCATOR>::pointer last,
          ALLOCATOR                                          allocator);
        // TBD: improve comment
        // Destroy the elements of type given by the 'allocator_traits' class
        // template for (template parameter) 'ALLOCATOR' starting at the
        // specified 'first' 'first' pointer and ending immediately before the
        // specified 'middle' pointer, and move the elements in the array
        // starting at 'middle' and ending at the specified 'last' pointer down
        // to the 'first' pointer.  If an assignment throws an exception during
        // this process, all of the elements in the range '[ first, last )'
        // will have unspecified but valid values, and no elements are
        // destroyed.  The behavior is undefined unless
        // 'first <= middle <= last'.

    template <class TARGET_TYPE>
    static void erase(TARGET_TYPE      *first,
                      TARGET_TYPE      *middle,
                      TARGET_TYPE      *last,
                      bslma::Allocator *allocator = 0);
        // Destroy the elements of the parameterized 'TARGET_TYPE' in the array
        // starting at the specified 'first' address and ending immediately
        // before the specified 'middle' address, and move the elements in the
        // array starting at 'middle' and ending at the specified 'last'
        // address down to the 'first' address.  If an assignment throws an
        // exception during this process, all of the elements in the range
        // '[ first, last )' will have unspecified but valid values, and no
        // elements are destroyed.  The behavior is undefined unless
        // 'first <= middle <= last'.

    template <class ALLOCATOR>
    static void
    insert(typename bsl::allocator_traits<ALLOCATOR>::pointer     toBegin,
           typename bsl::allocator_traits<ALLOCATOR>::pointer     toEnd,
           bslmf::MovableRef<
           typename bsl::allocator_traits<ALLOCATOR>::value_type> value,
           ALLOCATOR                                              allocator);
        // Insert the specified 'value' into the array of
        // 'allocator_traits<ALLOCATOR>::value_type' objects at the specified
        // 'toBegin' location, shifting forward the elements from 'toBegin' to
        // the specified 'toEnd' location by one position.  'value' is left in
        // a valid but unspecified state.  If a (copy or move) constructor or a
        // (copy or move) assignment operator throws an exception, then any
        // elements created after 'toEnd' are destroyed and the elements in the
        // range '[toBegin .. toEnd )' are left in a valid but unspecified
        // state.  The behavior is undefined unless 'toBegin' refers to
        // sufficient space to hold at least 'toEnd - toBegin + 1' elements.

    template <class TARGET_TYPE>
    static void insert(TARGET_TYPE                                *toBegin,
                       TARGET_TYPE                                *toEnd,
                       bslmf::MovableRef<TARGET_TYPE>              value,
                       bslma::Allocator                           *allocator);
        // Insert the specified 'value' into the array of the (template
        // parameter) type 'TARGET_TYPE' at the specified 'toBegin' address,
        // shifting the elements from 'toBegin' to the specified 'toEnd'
        // address by one position towards larger addresses.  'value' is left
        // in a valid but unspecified state.  If a (copy or move) constructor
        // or a (copy or move) assignment operator throws an exception, then
        // any elements created after 'toEnd' are destroyed and the elements in
        // the range '[toBegin .. toEnd )' are left in a valid but unspecified
        // state.  The behavior is undefined unless 'toBegin' refers to
        // sufficient space to hold at least 'toEnd - toBegin + 1' elements.

    template <class ALLOCATOR>
    static void
    insert(
      typename bsl::allocator_traits<ALLOCATOR>::pointer           toBegin,
      typename bsl::allocator_traits<ALLOCATOR>::pointer           toEnd,
      const typename bsl::allocator_traits<ALLOCATOR>::value_type& value,
      size_type                                                    numElements,
      ALLOCATOR                                                    allocator);
        // Insert the specified 'numElements' copies of the specified 'value'
        // into the array of type 'allocator_traits<ALLOCATOR>::value_type'
        // starting at the specified 'toBegin' location, shifting forward the
        // elements from 'toBegin' to the specified 'toEnd' location by
        // 'numElements' positions.  If a (copy or move) constructor or a (copy
        // or move) assignment operator throws an exception, any elements
        // created after 'toEnd' are destroyed and the elements in the range
        // '[toBegin .. toEnd)' are left in a valid but unspecified state.  The
        // behavior is undefined unless 'toBegin' refers to space sufficient to
        // hold at least 'toEnd - toBegin + numElements' elements.

    template <class TARGET_TYPE>
    static void insert(TARGET_TYPE        *toBegin,
                       TARGET_TYPE        *toEnd,
                       const TARGET_TYPE&  value,
                       size_type           numElements,
                       bslma::Allocator   *allocator);
        // Insert the specified 'numElements' copies of the specified 'value'
        // into the array of (template parameter) 'TARGET_TYPE' starting at the
        // specified 'toBegin' address and ending immediately before the
        // specified 'toEnd' address, shifting the elements in the array by
        // 'numElements' positions towards larger addresses.  The behavior is
        // undefined unless the destination array contains at least
        // 'numElements' uninitialized elements after 'toEnd'.  If a copy
        // constructor or assignment operator for 'TARGET_TYPE' throws an
        // exception, then any elements created after 'toEnd' are destroyed and
        // the elements in the range '[ toBegin, toEnd )' will have
        // unspecified, but valid, values.

    template <class ALLOCATOR, class FWD_ITER>
    static void
    insert(typename bsl::allocator_traits<ALLOCATOR>::pointer  toBegin,
           typename bsl::allocator_traits<ALLOCATOR>::pointer  toEnd,
           FWD_ITER                                            fromBegin,
           FWD_ITER                                            fromEnd,
           size_type                                           numElements,
           ALLOCATOR                                           allocator);
    template <class ALLOCATOR, class SOURCE_TYPE>
    static void
    insert(typename bsl::allocator_traits<ALLOCATOR>::pointer  toBegin,
           typename bsl::allocator_traits<ALLOCATOR>::pointer  toEnd,
           SOURCE_TYPE                                        *fromBegin,
           SOURCE_TYPE                                        *fromEnd,
           size_type                                           numElements,
           ALLOCATOR                                           allocator);
        // TBD: improve comment
        // Insert the specified 'numElements' from the range starting at the
        // specified 'fromBegin' and ending immediately before the specified
        // 'fromEnd' iterators of (template parameter) 'FWD_ITER' type (or
        // template parameter 'SOURCE_TYPE *'), into the array of elements of
        // type given by the 'allocator_traits' class template for (template
        // parameter) 'ALLOCATOR', starting at the specified 'toBegin' address,
        // shifting forward the elements in the array by 'numElements'
        // positions.  The behavior is undefined unless the destination array
        // contains 'numElements' uninitialized elements after 'toEnd',
        // 'numElements' is the distance between 'fromBegin' and 'fromEnd',
        // and the input array and the destination array do not overlap.  If a
        // copy constructor or assignment operator throws an exception, then
        // any elements created after 'toEnd' are destroyed and the elements in
        // the range '[ toBegin, toEnd )' will have valid but unspecified
        // values.

    template <class TARGET_TYPE, class FWD_ITER>
    static void insert(TARGET_TYPE      *toBegin,
                       TARGET_TYPE      *toEnd,
                       FWD_ITER          fromBegin,
                       FWD_ITER          fromEnd,
                       size_type         numElements,
                       bslma::Allocator *allocator);
    template <class TARGET_TYPE, class SOURCE_TYPE>
    static void insert(TARGET_TYPE      *toBegin,
                       TARGET_TYPE      *toEnd,
                       SOURCE_TYPE      *fromBegin,
                       SOURCE_TYPE      *fromEnd,
                       size_type         numElements,
                       bslma::Allocator *allocator);
        // Insert, into the array at the specified 'toBegin' location, the
        // specified 'numElements' from the range starting at the specified
        // 'fromBegin' and ending immediately before the specified 'fromEnd'
        // iterators of the (template parameter) 'FWD_ITER' type (or the
        // (template parameter) 'SOURCE_TYPE *'), into the array of elements of
        // the parameterized 'TARGET_TYPE' starting at the specified 'toBegin'
        // address and ending immediately before the specified 'toEnd' address,
        // shifting the elements in the array by 'numElements' positions
        // towards larger addresses.  The behavior is undefined unless the
        // destination array contains 'numElements' uninitialized elements
        // after 'toEnd', 'numElements' is the distance between 'fromBegin' and
        // 'fromEnd', and the input array and the destination array do not
        // overlap.  If a copy constructor or assignment operator for
        // 'TARGET_TYPE' throws an exception, then any elements created after
        // 'toEnd' are destroyed and the elements in the range
        // '[ toBegin, toEnd )' will have unspecified, but valid, values.

    template <class ALLOCATOR>
    static void moveInsert(
               typename bsl::allocator_traits<ALLOCATOR>::pointer  toBegin,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  toEnd,
               typename bsl::allocator_traits<ALLOCATOR>::pointer *fromEndPtr,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  fromBegin,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  fromEnd,
               size_type                                           numElements,
               ALLOCATOR                                           allocator);
        // TBD: improve comment
        // Move the elements of type given by the 'allocator_traits' class
        // template for (template parameter) 'ALLOCATOR' in the array starting
        // at the specified 'toBegin' location and ending immediately before
        // the specified 'toEnd' location by the specified 'numElements'
        // positions towards larger addresses, and fill the 'numElements' at
        // the 'toBegin' location by moving the elements from the array
        // starting at the specified 'fromBegin' and ending immediately before
        // the specified 'fromEnd' location.  Keep the iterator at the
        // specified 'fromEndPtr' address pointing to the end of the range as
        // the elements from '[ fromBegin, fromEnd )' are moved from source to
        // destination.  The behavior is undefined unless the destination array
        // contains 'numElements' uninitialized elements after 'toEnd',
        // 'numElements' is the distance from 'fromBegin' to 'fromEnd', and the
        // input and destination arrays do not overlap.  If a copy constructor
        // or assignment operator for 'TARGET_TYPE' throws an exception, then
        // any elements created after 'toEnd' are destroyed, the elements in
        // the ranges '[ toBegin, toEnd)' and '[ fromBegin, *fromEndPtr )' will
        // have unspecified, but valid, values, and the elements in
        // '[ *fromEndPtr, fromEnd )' will be destroyed.

    template <class TARGET_TYPE>
    static void moveInsert(TARGET_TYPE       *toBegin,
                           TARGET_TYPE       *toEnd,
                           TARGET_TYPE      **fromEndPtr,
                           TARGET_TYPE       *fromBegin,
                           TARGET_TYPE       *fromEnd,
                           size_type          numElements,
                           bslma::Allocator  *allocator);
        // Move the elements of the (template parameter) 'TARGET_TYPE' in the
        // array starting at the specified 'toBegin' address and ending
        // immediately before the specified 'toEnd' address by the specified
        // 'numElements' positions towards larger addresses, and fill the
        // 'numElements' at the 'toBegin' address by moving the elements from
        // the array starting at the specified 'fromBegin' and ending
        // immediately before the specified 'fromEnd' address.  Keep the
        // iterator at the specified 'fromEndPtr' address pointing to the end
        // of the range as the elements from '[ fromBegin, fromEnd )' are moved
        // from source to destination.  The behavior is undefined unless the
        // destination array contains 'numElements' uninitialized elements
        // after 'toEnd', 'numElements' is the distance from 'fromBegin' to
        // 'fromEnd', and the input and destination arrays do not overlap.  If
        // a copy constructor or assignment operator for 'TARGET_TYPE' throws
        // an exception, then any elements created after 'toEnd' are destroyed,
        // the elements in the ranges '[ toBegin, toEnd)' and
        // '[ fromBegin, *fromEndPtr )' will have unspecified, but valid,
        // values, and the elements in '[ *fromEndPtr, fromEnd )' will be
        // destroyed.

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

    template <class ALLOCATOR>
    static void uninitializedFillN(
      typename bsl::allocator_traits<ALLOCATOR>::pointer           begin,
      size_type                                                    numElements,
      const typename bsl::allocator_traits<ALLOCATOR>::value_type& value,
      ALLOCATOR                                                    allocator);
        // TBD: improve comment
        // Construct copies of the specified 'value' of type given by the
        // 'allocator_traits' class template for (template parameter)
        // 'ALLOCATOR' into the uninitialized array containing the specified
        // 'numElements' starting at the specified 'begin' location.  The
        // behavior is undefined unless the output array contains at least
        // 'numElements' uninitialized elements after 'begin'.  If a
        // constructor throws an exception during the operation, then the
        // destructor is called on any newly-constructed elements, leaving the
        // output array in an uninitialized state.

    template <class TARGET_TYPE>
    static void uninitializedFillN(TARGET_TYPE        *begin,
                                   size_type           numElements,
                                   const TARGET_TYPE&  value,
                                   bslma::Allocator   *allocator);
        // Construct copies of the specified 'value' of the parameterized type
        // 'TARGET_TYPE' into the uninitialized array containing the specified
        // 'numElements' starting at the specified 'begin' address.  If the
        // (template parameter) 'ALLOCATOR' type is derived from
        // 'bslma::Allocator' and 'TARGET_TYPE' supports 'bslma' allocators,
        // then the specified 'allocator' is passed to each invocation of the
        // 'TARGET_TYPE' copy constructor.  The behavior is undefined unless
        // the output array contains at least 'numElements' uninitialized
        // elements after 'begin'.  If a 'TARGET_TYPE' constructor throws an
        // exception during the operation, then the destructor is called on any
        // newly-constructed elements, leaving the output array in an
        // uninitialized state.  Note that the argument order was chosen to
        // maintain compatibility with the existing 'bslalg'.
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
    // corresponding contract in the 'ArrayPrimitives' utility.

  private:
    // PRIVATE METHODS
    template <class TARGET_TYPE>
    static void assign(TARGET_TYPE *srcStart,
                       TARGET_TYPE *srcEnd,
                       TARGET_TYPE& value);
        // Copy-assign the specified 'value' to the range starting at the
        // specified 'srcStart' and ending immediately before the specified
        // 'srcEnd'.  Note that the (template parameter) 'TARGET_TYPE' must be
        // copy-assignable.  Also note that 'value' should not be an element in
        // the range '[srcStart, srcEnd)'.

    template <class TARGET_TYPE>
    static void reverseAssign(TARGET_TYPE *dest,
                              TARGET_TYPE *srcStart,
                              TARGET_TYPE *srcEnd);
        // Copy-assign the elements in reverse order from the range starting at
        // the specified 'srcStart' and ending immediately before the specified
        // 'srcEnd' to the range starting at the specified 'dest' and ending
        // immediately before 'dest + (srcEnd - srcStart)'.  The behavior is
        // undefined unless each element is both range '[srcStart, srcEnd)' and
        // range '[dest, dest + (srcEnd - srcStart))' is valid.  Note that the
        // (template parameter) 'TARGET_TYPE' must be copy-assignable.  Also
        // note that this method is intended to support range assignment when
        // the two ranges may be overlapped, and 'srcStart <= dest'.

  public:
    // TYPES
    typedef ArrayPrimitives::size_type       size_type;
    typedef ArrayPrimitives::difference_type difference_type;

    enum {
        // These constants are used in the overloads below, when the last
        // argument is of type 'bslmf::MetaInt<N> *', indicating that
        // 'TARGET_TYPE' has the traits for which the enumerator equal to 'N'
        // is named.

        e_IS_ITERATOR_TO_FUNCTION_POINTER  = 6,
        e_IS_POINTER_TO_POINTER            = 5,
        e_IS_FUNDAMENTAL_OR_POINTER        = 4,
        e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS  = 3,
        e_BITWISE_COPYABLE_TRAITS          = 2,
        e_BITWISE_MOVEABLE_TRAITS          = 1,
        e_NIL_TRAITS                       = 0
    };

    enum {
        // Number of bytes for which a stack-allocated buffer can be
        // comfortably obtained to optimize bitwise moves.

        k_INPLACE_BUFFER_SIZE = 16 * bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT
    };

    // CLASS METHODS
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
                      bool                                        *begin,
                      bool                                         value,
                      size_type                                    numElements,
                      void                                        * = 0,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                      char                                        *begin,
                      char                                         value,
                      size_type                                    numElements,
                      void                                        * = 0,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                      unsigned char                               *begin,
                      unsigned char                                value,
                      size_type                                    numElements,
                      void                                        * = 0,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                      signed char                                 *begin,
                      signed char                                  value,
                      size_type                                    numElements,
                      void                                        * = 0,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                      wchar_t                                     *begin,
                      wchar_t                                      value,
                      size_type                                    numElements,
                      void                                        * = 0,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                      short                                       *begin,
                      short                                        value,
                      size_type                                    numElements,
                      void                                        * = 0,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                      unsigned short                              *begin,
                      unsigned short                               value,
                      size_type                                    numElements,
                      void                                        * = 0,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                      int                                         *begin,
                      int                                          value,
                      size_type                                    numElements,
                      void                                        * = 0,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                      unsigned int                                *begin,
                      unsigned int                                 value,
                      size_type                                    numElements,
                      void                                        * = 0,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                      long                                        *begin,
                      long                                         value,
                      size_type                                    numElements,
                      void                                        * = 0,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                      unsigned long                               *begin,
                      unsigned long                                value,
                      size_type                                    numElements,
                      void                                        * = 0,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                      bsls::Types::Int64                          *begin,
                      bsls::Types::Int64                           value,
                      size_type                                    numElements,
                      void                                        * = 0,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                      bsls::Types::Uint64                         *begin,
                      bsls::Types::Uint64                          value,
                      size_type                                    numElements,
                      void                                        * = 0,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                      float                                       *begin,
                      float                                        value,
                      size_type                                    numElements,
                      void                                        * = 0,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                      double                                      *begin,
                      double                                       value,
                      size_type                                    numElements,
                      void                                        * = 0,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                      long double                                 *begin,
                      long double                                  value,
                      size_type                                    numElements,
                      void                                        * = 0,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> * = 0);
    static void uninitializedFillN(
                     void                                        **begin,
                     void                                         *value,
                     size_type                                     numElements,
                     void                                         * = 0,
                     bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER>  * = 0);
    static void uninitializedFillN(
                     const void                                  **begin,
                     const void                                   *value,
                     size_type                                     numElements,
                     void                                         * = 0,
                     bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER>  * = 0);
    static void uninitializedFillN(
                     volatile void                               **begin,
                     volatile void                                *value,
                     size_type                                     numElements,
                     void                                         * = 0,
                     bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER>  * = 0);
    static void uninitializedFillN(
                     const volatile void                         **begin,
                     const volatile void                          *value,
                     size_type                                     numElements,
                     void                                         * = 0,
                     bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER>  * = 0);
    template <class TARGET_TYPE>
    static void uninitializedFillN(
                     TARGET_TYPE                                 **begin,
                     TARGET_TYPE                                  *value,
                     size_type                                     numElements,
                     void                                         * = 0,
                     bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER>  * = 0);
    template <class TARGET_TYPE>
    static void uninitializedFillN(
                     const TARGET_TYPE                           **begin,
                     const TARGET_TYPE                            *value,
                     size_type                                     numElements,
                     void                                         * = 0,
                     bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER>  * = 0);
    template <class TARGET_TYPE>
    static void uninitializedFillN(
                     volatile  TARGET_TYPE                       **begin,
                     volatile TARGET_TYPE                         *value,
                     size_type                                     numElements,
                     void                                         * = 0,
                     bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER>  * = 0);
    template <class TARGET_TYPE>
    static void uninitializedFillN(
                     const volatile TARGET_TYPE                  **begin,
                     const volatile TARGET_TYPE                   *value,
                     size_type                                     numElements,
                     void                                         * = 0,
                     bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER>  * = 0);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void uninitializedFillN(
                        TARGET_TYPE                               *begin,
                        const TARGET_TYPE&                         value,
                        size_type                                  numElements,
                        ALLOCATOR                                 *allocator,
                        bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS> *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void uninitializedFillN(TARGET_TYPE                  *begin,
                                   const TARGET_TYPE&            value,
                                   size_type                     numElements,
                                   ALLOCATOR                    *allocator,
                                   bslmf::MetaInt<e_NIL_TRAITS> *);
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
                              ALLOCATOR                   allocator,
                              bslmf::MetaInt<e_IS_POINTER_TO_POINTER> *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void copyConstruct(
                          TARGET_TYPE                               *toBegin,
                          const TARGET_TYPE                         *fromBegin,
                          const TARGET_TYPE                         *fromEnd,
                          ALLOCATOR                                  allocator,
                          bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS> *);
    template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
    static void copyConstruct(TARGET_TYPE                           *toBegin,
                              FWD_ITER                               fromBegin,
                              FWD_ITER                               fromEnd,
                              ALLOCATOR                              allocator,
                          bslmf::MetaInt<e_IS_ITERATOR_TO_FUNCTION_POINTER> *);
    template <class FWD_ITER, class ALLOCATOR>
    static void copyConstruct(void                                 **toBegin,
                              FWD_ITER                               fromBegin,
                              FWD_ITER                               fromEnd,
                              ALLOCATOR                              allocator,
                          bslmf::MetaInt<e_IS_ITERATOR_TO_FUNCTION_POINTER> *);
    template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
    static void copyConstruct(TARGET_TYPE                  *toBegin,
                              FWD_ITER                      fromBegin,
                              FWD_ITER                      fromEnd,
                              ALLOCATOR                     allocator,
                              bslmf::MetaInt<e_NIL_TRAITS> *);
        // These functions follow the 'copyConstruct' contract.  If the
        // (template parameter) 'ALLOCATOR' type is based on 'bslma::Allocator'
        // and the 'TARGET_TYPE' constructors take an allocator argument, then
        // pass the specified 'allocator' to the copy constructor.  The
        // behavior is undefined unless the output array has length at least
        // the distance from the specified 'fromBegin' to the specified
        // 'fromEnd'.  Note that if 'FWD_ITER' is the 'TARGET_TYPE *' pointer
        // type and 'TARGET_TYPE' is bit-wise copyable, then this operation is
        // simply 'memcpy'.  The last argument is for removing overload
        // ambiguities and is not used.

    template <class TARGET_TYPE, class ALLOCATOR>
    static void moveConstruct(
                          TARGET_TYPE                               *toBegin,
                          TARGET_TYPE                               *fromBegin,
                          TARGET_TYPE                               *fromEnd,
                          ALLOCATOR                                  allocator,
                          bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS> *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void moveConstruct(
                          TARGET_TYPE                               *toBegin,
                          TARGET_TYPE                               *fromBegin,
                          TARGET_TYPE                               *fromEnd,
                          ALLOCATOR                                  allocator,
                          bslmf::MetaInt<e_NIL_TRAITS> *);
        // TBD: improve comment
        // Move-insert into an uninitialized array beginning at the specified
        // 'toBegin' pointer, elements of type given by the 'allocator_traits'
        // class template for (template parameter) 'ALLOCATOR' from elements
        // starting at the specified 'fromBegin' pointer and ending immediately
        // before the specified 'fromEnd' pointer.  The elements in the range
        // '[fromBegin...fromEnd)' are left in a valid but unspecified state.
        // If a constructor throws an exception during the operation, then the
        // destructor is called on any newly-constructed elements, leaving the
        // output array in an uninitialized state.  The behavior is undefined
        // unless 'toBegin' refers to space sufficient to hold
        // 'fromEnd - fromBegin' elements.

    template <class TARGET_TYPE, class ALLOCATOR>
    static void moveIfNoexcept(
                          TARGET_TYPE                               *toBegin,
                          TARGET_TYPE                               *fromBegin,
                          TARGET_TYPE                               *fromEnd,
                          ALLOCATOR                                  allocator,
                          bslmf::MetaInt<e_NIL_TRAITS> *);
        // TBD: improve comment
        // Either move- or copy-insert into an uninitialized array beginning at
        // the specified 'toBegin' pointer, elements of type given by the
        // 'allocator_traits' class template for (template parameter)
        // 'ALLOCATOR' from elements starting at the specified 'fromBegin'
        // pointer and ending immediately before the specified 'fromEnd'
        // pointer.  The elements in the range '[fromBegin...fromEnd)' are left
        // in a valid but unspecified state.  Use the move constructor if it is
        // guaranteed to not throw or if the target type does not define a copy
        // constructor; otherwise use the copy constructor.  If a constructor
        // throws an exception during the operation, then the destructor is
        // called on any newly-constructed elements, leaving the output array
        // in an uninitialized state.  The behavior is undefined unless
        // 'toBegin' refers to space sufficient to hold 'fromEnd - fromBegin'
        // elements.

    template <class TARGET_TYPE, class ALLOCATOR>
    static void defaultConstruct(
                TARGET_TYPE                                       *begin,
                size_type                                          numElements,
                ALLOCATOR                                          allocator,
                bslmf::MetaInt<e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS> *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void defaultConstruct(
                        TARGET_TYPE                               *begin,
                        size_type                                  numElements,
                        ALLOCATOR                                  allocator,
                        bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS> *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void defaultConstruct(TARGET_TYPE                  *begin,
                                 size_type                     numElements,
                                 ALLOCATOR                     allocator,
                                 bslmf::MetaInt<e_NIL_TRAITS> *);
        // Use the default constructor of the (template parameter)
        // 'TARGET_TYPE' (or 'memset' to 0 if 'TARGET_TYPE' has a trivial
        // default constructor) on each element of the array starting at the
        // specified 'begin' address and ending immediately before the 'end'
        // address.  Pass the specified 'allocator' to the default constructor
        // if appropriate.  The last argument is for traits overloading
        // resolution only and its value is ignored.

    template <class TARGET_TYPE, class ALLOCATOR>
    static void destructiveMove(
                          TARGET_TYPE                               *toBegin,
                          TARGET_TYPE                               *fromBegin,
                          TARGET_TYPE                               *fromEnd,
                          ALLOCATOR                                  allocator,
                          bslmf::MetaInt<e_BITWISE_MOVEABLE_TRAITS> *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void destructiveMove(TARGET_TYPE                  *toBegin,
                                TARGET_TYPE                  *fromBegin,
                                TARGET_TYPE                  *fromEnd,
                                ALLOCATOR                     allocator,
                                bslmf::MetaInt<e_NIL_TRAITS> *);
        // These functions follow the 'destructiveMove' contract.  Note that
        // both arrays cannot overlap (one contains only initialized elements
        // and the other only uninitialized elements), and that if
        // 'TARGET_TYPE' is bit-wise moveable, then this operation is simply
        // 'memcpy'.  The last argument is for removing overload ambiguities
        // and is not used.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class TARGET_TYPE, class ALLOCATOR, class... ARGS>
    static void emplace(TARGET_TYPE                               *toBegin,
                        TARGET_TYPE                               *toEnd,
                        ALLOCATOR                                  allocator,
                        bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS> *,
                        ARGS&&...                                  args);
    template <class TARGET_TYPE, class ALLOCATOR, class... ARGS>
    static void emplace(TARGET_TYPE                               *toBegin,
                        TARGET_TYPE                               *toEnd,
                        ALLOCATOR                                  allocator,
                        bslmf::MetaInt<e_BITWISE_MOVEABLE_TRAITS> *,
                        ARGS&&...                                  args);
    template <class TARGET_TYPE, class ALLOCATOR, class... ARGS>
    static void emplace(TARGET_TYPE                  *toBegin,
                        TARGET_TYPE                  *toEnd,
                        ALLOCATOR                     allocator,
                        bslmf::MetaInt<e_NIL_TRAITS> *,
                        ARGS&&...                     args);
        // TBD: document this
#endif

    template <class TARGET_TYPE, class ALLOCATOR>
    static void erase(TARGET_TYPE                               *first,
                      TARGET_TYPE                               *middle,
                      TARGET_TYPE                               *last,
                      ALLOCATOR                                  allocator,
                      bslmf::MetaInt<e_BITWISE_MOVEABLE_TRAITS> *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void erase(TARGET_TYPE                *first,
                      TARGET_TYPE                *middle,
                      TARGET_TYPE                *last,
                      ALLOCATOR                   allocator,
                      bslmf::MetaInt<e_NIL_TRAITS> *);
        // These functions follow the 'erase' contract.  Note that if (template
        // parameter) 'TARGET_TYPE' is bit-wise moveable, then this operation
        // can be implemented by first bit-wise moving the elements in
        // '[middle, last)' towards first, and destroying
        // '[ last - (middle - first), last)'; note that this cannot throw
        // exceptions.

    template <class TARGET_TYPE, class ALLOCATOR>
    static void insert(TARGET_TYPE                               *toBegin,
                       TARGET_TYPE                               *toEnd,
                       const TARGET_TYPE&                         value,
                       size_type                                  numElements,
                       ALLOCATOR                                  allocator,
                       bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS> *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void insert(TARGET_TYPE                               *toBegin,
                       TARGET_TYPE                               *toEnd,
                       const TARGET_TYPE&                         value,
                       size_type                                  numElements,
                       ALLOCATOR                                  allocator,
                       bslmf::MetaInt<e_BITWISE_MOVEABLE_TRAITS> *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void insert(TARGET_TYPE                  *toBegin,
                       TARGET_TYPE                  *toEnd,
                       const TARGET_TYPE&            value,
                       size_type                     numElements,
                       ALLOCATOR                     allocator,
                       bslmf::MetaInt<e_NIL_TRAITS> *);
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
                       ALLOCATOR                                allocator,
                       bslmf::MetaInt<e_IS_POINTER_TO_POINTER> *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void insert(TARGET_TYPE                               *toBegin,
                       TARGET_TYPE                               *toEnd,
                       const TARGET_TYPE                         *fromBegin,
                       const TARGET_TYPE                         *fromEnd,
                       size_type                                  numElements,
                       ALLOCATOR                                  allocator,
                       bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS> *);
    template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
    static void insert(TARGET_TYPE                               *toBegin,
                       TARGET_TYPE                               *toEnd,
                       FWD_ITER                                   fromBegin,
                       FWD_ITER                                   fromEnd,
                       size_type                                  numElements,
                       ALLOCATOR                                  allocator,
                       bslmf::MetaInt<e_BITWISE_MOVEABLE_TRAITS> *);
    template <class FWD_ITER, class ALLOCATOR>
    static void insert(void                                   **toBegin,
                       void                                   **toEnd,
                       FWD_ITER                                 fromBegin,
                       FWD_ITER                                 fromEnd,
                       size_type                                numElements,
                       ALLOCATOR                                allocator,
                       bslmf::MetaInt<e_IS_ITERATOR_TO_FUNCTION_POINTER> *);
    template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
    static void insert(TARGET_TYPE                  *toBegin,
                       TARGET_TYPE                  *toEnd,
                       FWD_ITER                      fromBegin,
                       FWD_ITER                      fromEnd,
                       size_type                     numElements,
                       ALLOCATOR                     allocator,
                       bslmf::MetaInt<e_NIL_TRAITS> *);
        // These functions follow the 'insert' contract.  Note that if
        // 'TARGET_TYPE' is bit-wise copyable and 'FWD_ITER' is convertible to
        // 'const TARGET_TYPE *', then this operation is simply 'memmove'
        // followed by 'memcpy'.  If 'TARGET_TYPE' is bit-wise moveable and
        // 'FWD_ITER' is convertible to 'const TARGET_TYPE *', then this
        // operation can still be optimized using 'memmove' followed by
        // repeated copies.  The last argument is for removing overload
        // ambiguities and is not used.

    template <class TARGET_TYPE, class ALLOCATOR>
    static void moveInsert(
                       TARGET_TYPE                                *toBegin,
                       TARGET_TYPE                                *toEnd,
                       TARGET_TYPE                               **lastPtr,
                       TARGET_TYPE                                *first,
                       TARGET_TYPE                                *last,
                       size_type                                   numElements,
                       ALLOCATOR                                   allocator,
                       bslmf::MetaInt<e_BITWISE_MOVEABLE_TRAITS>  *);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void moveInsert(TARGET_TYPE                   *toBegin,
                           TARGET_TYPE                   *toEnd,
                           TARGET_TYPE                  **lastPtr,
                           TARGET_TYPE                   *first,
                           TARGET_TYPE                   *last,
                           size_type                      numElements,
                           ALLOCATOR                      allocator,
                           bslmf::MetaInt<e_NIL_TRAITS>  *);
        // These functions follow the 'moveInsert' contract.  Note that if
        // 'TARGET_TYPE' is at least bit-wise moveable, then this operation is
        // simply 'memmove' followed by 'memcpy'.

    template <class TARGET_TYPE>
    static void rotate(TARGET_TYPE                               *begin,
                       TARGET_TYPE                               *middle,
                       TARGET_TYPE                               *end,
                       bslmf::MetaInt<e_BITWISE_MOVEABLE_TRAITS> *);
    template <class TARGET_TYPE>
    static void rotate(TARGET_TYPE                  *begin,
                       TARGET_TYPE                  *middle,
                       TARGET_TYPE                  *end,
                       bslmf::MetaInt<e_NIL_TRAITS> *);
        // These functions follow the 'rotate' contract, but the first overload
        // is optimized when the parameterized 'TARGET_TYPE' is bit-wise
        // moveable.  The last argument is for removing overload ambiguities
        // and is not used.  Note that if 'TARGET_TYPE' is bit-wise moveable,
        // the 'rotate(char*, char*, char*)' can be used, enabling to take the
        // whole implementation out-of-line.

    template <class ALLOCATOR>
    static void shiftAndInsert(
             typename bsl::allocator_traits<ALLOCATOR>::pointer      begin,
             typename bsl::allocator_traits<ALLOCATOR>::pointer      end,
             bslmf::MovableRef<
             typename bsl::allocator_traits<ALLOCATOR>::value_type>  value,
             ALLOCATOR                                               allocator,
             bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS>              *);
    template <class ALLOCATOR>
    static void shiftAndInsert(
             typename bsl::allocator_traits<ALLOCATOR>::pointer      begin,
             typename bsl::allocator_traits<ALLOCATOR>::pointer      end,
             bslmf::MovableRef<
             typename bsl::allocator_traits<ALLOCATOR>::value_type>  value,
             ALLOCATOR                                               allocator,
             bslmf::MetaInt<e_BITWISE_MOVEABLE_TRAITS>              *);
    template <class ALLOCATOR>
    static void shiftAndInsert(
             typename bsl::allocator_traits<ALLOCATOR>::pointer      begin,
             typename bsl::allocator_traits<ALLOCATOR>::pointer      end,
             bslmf::MovableRef<
             typename bsl::allocator_traits<ALLOCATOR>::value_type>  value,
             ALLOCATOR                                               allocator,
             bslmf::MetaInt<e_NIL_TRAITS>                           *);
        // Shift the specified '[begin, end)' sequence one position right, then
        // insert the specified 'value' at the position pointed by 'begin'.
        // The specified 'allocator' is used for the element construction.  The
        // behavior is undefined unless the specified '[begin, end)' sequence
        // contains at least one element.

    // 'bitwise' METHODS
    static void bitwiseRotate(char *begin, char *middle, char *end);
        // This function follows the 'rotate' contract, but by using bit-wise
        // moves on the underlying 'char' array.

    static void bitwiseRotateBackward(char *begin, char *middle, char *end);
        // Move the characters in the array starting at the specified 'first'
        // address and ending immediately before the specified 'middle' address
        // to the array of the same length ending at the specified 'last'
        // address (and thus starting at the 'last - (middle - first)'
        // address), and move the elements previously in the array starting at
        // 'middle' and ending at 'last' down to the 'first' address.  The
        // behavior is undefined unless
        // 'middle - begin <= k_INPLACE_BUFFER_SIZE'.

    static void bitwiseRotateForward(char *begin, char *middle, char *end);
        // Move the characters in the array starting at the specified 'first'
        // address and ending immediately before the specified 'middle' address
        // to the array of the same length ending at the specified 'last'
        // address (and thus starting at the 'last - (middle - first)'
        // address), and move the elements previously in the array starting at
        // 'middle' and ending at 'last' down to the 'first' address.  The
        // behavior is undefined unless
        // 'end - middle <= k_INPLACE_BUFFER_SIZE'.

    static void bitwiseSwapRanges(char *begin, char *middle, char *end);
        // Swap the characters in the array starting at the specified 'first'
        // address and ending immediately before the specified 'middle' address
        // with the array of the same length starting at the 'middle' address
        // and ending at the specified 'last' address.  The behavior is
        // undefined unless 'middle - begin == end - middle'.

    template <class FORWARD_ITERATOR>
    static bool isInvalidRange(FORWARD_ITERATOR begin, FORWARD_ITERATOR end);
    template <class TARGET_TYPE>
    static bool isInvalidRange(TARGET_TYPE *begin, TARGET_TYPE *end);
        // Return 'true' if the specified 'begin' and the specified 'end'
        // provably do not form a valid semi-open range, '[begin, end)', and
        // 'false' otherwise.  Note that 'begin == null == end' produces a
        // valid range, and any other use of the null pointer value will return
        // 'true'.  Also note that this function is intended to support
        // testing, primarily through assertions, so will return 'false' unless
        // it can *prove* that the passed range is invalid.  Currently, this
        // function can prove invalid ranges only for pointers, although should
        // also encompass generic random access iterators in a future update,
        // where iterator tag types are levelized below 'bslalg'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================
// IMPLEMENTATION NOTES: Specializations of 'uninitializedFillN' for most
// fundamental types are not templates nor inline, and thus can be found in the
// '.cpp' file.

                   // =====================================
                   // struct ArrayPrimitives_CanBitwiseCopy
                   // =====================================

template <class FROM_TYPE, class TO_TYPE>
struct ArrayPrimitives_CanBitwiseCopy
    : bsl::integral_constant<bool,
                             bsl::is_same<
                                   typename bsl::remove_const<FROM_TYPE>::type,
                                   typename bsl::remove_const<TO_TYPE  >::type>
                                                                        ::value
                          && bsl::is_trivially_copyable<
                                   typename bsl::remove_const<TO_TYPE  >::type>
                                                                        ::value
                            > {
};

                       // ----------------------
                       // struct ArrayPrimitives
                       // ----------------------

template <class ALLOCATOR>
inline
void ArrayPrimitives::uninitializedFillN(
      typename bsl::allocator_traits<ALLOCATOR>::pointer           begin,
      size_type                                                    numElements,
      const typename bsl::allocator_traits<ALLOCATOR>::value_type& value,
      ALLOCATOR                                                    allocator)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type TargetType;

    enum {
        // We provide specialized overloads of 'uninitializedFillN' for
        // fundamental and pointer types.  However, function pointers can have
        // extern "C" linkage and SunPro doesn't match them properly with the
        // pointer template function overload in 'Imp', so we resort to the
        // general case for those.

        k_IS_FUNCTION_POINTER = bslmf::IsFunctionPointer<TargetType>::value,
        k_IS_FUNDAMENTAL      = bsl::is_fundamental<TargetType>::value,
        k_IS_POINTER          = bsl::is_pointer<TargetType>::value,

        k_IS_FUNDAMENTAL_OR_POINTER = k_IS_FUNDAMENTAL ||
                                     (k_IS_POINTER && !k_IS_FUNCTION_POINTER),

        k_IS_BITWISECOPYABLE  = bsl::is_trivially_copyable<TargetType>::value,

        k_VALUE =
                 k_IS_FUNDAMENTAL_OR_POINTER ? Imp::e_IS_FUNDAMENTAL_OR_POINTER
               : k_IS_BITWISECOPYABLE        ? Imp::e_BITWISE_COPYABLE_TRAITS
               :                               Imp::e_NIL_TRAITS
    };
    ArrayPrimitives_Imp::uninitializedFillN(begin,
                                            value,
                                            numElements,
                                            &allocator,
                                            (bslmf::MetaInt<k_VALUE>*)0);
}

template <class TARGET_TYPE>
inline
void ArrayPrimitives::uninitializedFillN(TARGET_TYPE        *begin,
                                         size_type           numElements,
                                         const TARGET_TYPE&  value,
                                         bslma::Allocator   *basicAllocator)
{
    uninitializedFillN<bsl::allocator<TARGET_TYPE> >(begin,
                                                     numElements,
                                                     value,
                                                     basicAllocator);
}

template <class ALLOCATOR, class FWD_ITER>
void ArrayPrimitives::copyConstruct(
                  typename bsl::allocator_traits<ALLOCATOR>::pointer toBegin,
                  FWD_ITER                                           fromBegin,
                  FWD_ITER                                           fromEnd,
                  ALLOCATOR                                          allocator)
{
    BSLS_ASSERT_SAFE(toBegin || fromBegin == fromEnd);

    BSLMF_ASSERT(!bsl::is_pointer<FWD_ITER>::value);
    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type TargetType;
    typedef typename FWD_ITER::value_type FwdTarget;
        // Overload resolution will handle the case where 'FWD_ITER' is a raw
        // pointer, so we need handle only user-defined iterators.  As 'bslalg'
        // is levelized below 'bslstl' we cannot use 'iterator_traits', but
        // rely on the same property as 'iterator_traits' that this typedef
        // must be defined for any standard-conforming iterator, unless the
        // iterator explicitly specialized the 'std::iterator_traits' template.
        // In practice, iterators always prefer to provide the member typedef
        // than specialize the traits as it is a much simpler implementation,
        // so this assumption is good enough.
        //
        // Also note that as we know that 'FWD_ITER' is not a pointer, then we
        // cannot take advantage of bitwise copying as we do not have pointers
        // to pass to the 'memcpy' describing the whole range.  It is not worth
        // the effort to try to bitwise copy one element at a time.

    typedef typename bsl::remove_pointer<TargetType>::type RemovePtrTarget;
        // We want to detect the special case of copying function pointers to
        // 'void *' or 'const void *' pointers.

    enum {
        k_ITER_TO_FUNC_PTRS   = bslmf::IsFunctionPointer<FwdTarget>::value,
        k_TARGET_IS_VOID_PTR  = bsl::is_pointer<TargetType>::value &&
                                bsl::is_void<RemovePtrTarget>::value,

        k_VALUE = k_ITER_TO_FUNC_PTRS && k_TARGET_IS_VOID_PTR
                ? Imp::e_IS_ITERATOR_TO_FUNCTION_POINTER
                : Imp::e_NIL_TRAITS
    };

    ArrayPrimitives_Imp::copyConstruct(toBegin,
                                       fromBegin,
                                       fromEnd,
                                       allocator,
                                       (bslmf::MetaInt<k_VALUE>*)0);
}

template <class TARGET_TYPE, class FWD_ITER>
inline
void ArrayPrimitives::copyConstruct(TARGET_TYPE      *toBegin,
                                    FWD_ITER          fromBegin,
                                    FWD_ITER          fromEnd,
                                    bslma::Allocator *basicAllocator)
{
    copyConstruct<bsl::allocator<TARGET_TYPE> >(toBegin,
                                                fromBegin,
                                                fromEnd,
                                                basicAllocator);
}

template <class ALLOCATOR, class SOURCE_TYPE>
inline
void ArrayPrimitives::copyConstruct(
                 typename bsl::allocator_traits<ALLOCATOR>::pointer  toBegin,
                 SOURCE_TYPE                                        *fromBegin,
                 SOURCE_TYPE                                        *fromEnd,
                 ALLOCATOR                                           allocator)
{
    BSLS_ASSERT_SAFE(toBegin || fromBegin == fromEnd);

    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type TargetType;

    enum {
        k_ARE_PTRS_TO_PTRS = bsl::is_pointer<TargetType>::value &&
                             bsl::is_pointer<SOURCE_TYPE>::value,
        k_IS_BITWISECOPYABLE  =
               ArrayPrimitives_CanBitwiseCopy<SOURCE_TYPE, TargetType>::value,
        k_VALUE = k_ARE_PTRS_TO_PTRS   ? Imp::e_IS_POINTER_TO_POINTER
                : k_IS_BITWISECOPYABLE ? Imp::e_BITWISE_COPYABLE_TRAITS
                :                        Imp::e_NIL_TRAITS
    };

    ArrayPrimitives_Imp::copyConstruct(toBegin,
                                       fromBegin,
                                       fromEnd,
                                       allocator,
                                       (bslmf::MetaInt<k_VALUE>*)0);
}

template <class TARGET_TYPE, class SOURCE_TYPE>
inline
void ArrayPrimitives::copyConstruct(TARGET_TYPE      *toBegin,
                                    SOURCE_TYPE      *fromBegin,
                                    SOURCE_TYPE      *fromEnd,
                                    bslma::Allocator *basicAllocator)
{
    copyConstruct<bsl::allocator<TARGET_TYPE> >(toBegin,
                                                fromBegin,
                                                fromEnd,
                                                basicAllocator);
}

template <class ALLOCATOR>
inline
void ArrayPrimitives::defaultConstruct(
                typename bsl::allocator_traits<ALLOCATOR>::pointer begin,
                size_type                                          numElements,
                ALLOCATOR                                          allocator)
{
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));
    BSLS_ASSERT_SAFE(begin || 0 == numElements);

    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type TargetType;

    enum {
        k_VALUE = bsl::is_fundamental<TargetType>::value
               || bsl::is_enum<TargetType>::value
               || bsl::is_pointer<TargetType>::value
#if !defined(BSLALG_ARRAYPRIMITIVES_NON_ZERO_NULL_VALUE_FOR_MEMBER_POINTERS)
               || bsl::is_member_pointer<TargetType>::value
#endif
              ? Imp::e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS
              : bsl::is_trivially_copyable<TargetType>::value &&
                bsl::is_trivially_default_constructible<TargetType>::value
                  ? Imp::e_BITWISE_COPYABLE_TRAITS
                  : Imp::e_NIL_TRAITS
    };
    ArrayPrimitives_Imp::defaultConstruct(begin,
                                          numElements,
                                          allocator,
                                          (bslmf::MetaInt<k_VALUE>*)0);
}

template <class TARGET_TYPE>
inline
void ArrayPrimitives::defaultConstruct(TARGET_TYPE      *begin,
                                       size_type         numElements,
                                       bslma::Allocator *basicAllocator)
{
    defaultConstruct<bsl::allocator<TARGET_TYPE> >(begin,
                                                   numElements,
                                                   basicAllocator);
}

template <class ALLOCATOR>
inline
void ArrayPrimitives::destructiveMove(
                  typename bsl::allocator_traits<ALLOCATOR>::pointer toBegin,
                  typename bsl::allocator_traits<ALLOCATOR>::pointer fromBegin,
                  typename bsl::allocator_traits<ALLOCATOR>::pointer fromEnd,
                  ALLOCATOR                                          allocator)
{
    BSLS_ASSERT_SAFE(toBegin || fromBegin == fromEnd);
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(fromBegin, fromEnd));

    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type TargetType;

    enum {
        k_VALUE = bslmf::IsBitwiseMoveable<TargetType>::value
              ? Imp::e_BITWISE_MOVEABLE_TRAITS
              : Imp::e_NIL_TRAITS
    };
    ArrayPrimitives_Imp::destructiveMove(toBegin,
                                         fromBegin,
                                         fromEnd,
                                         allocator,
                                         (bslmf::MetaInt<k_VALUE>*)0);
}

template <class TARGET_TYPE>
inline
void ArrayPrimitives::destructiveMove(TARGET_TYPE      *toBegin,
                                      TARGET_TYPE      *fromBegin,
                                      TARGET_TYPE      *fromEnd,
                                      bslma::Allocator *basicAllocator)
{
    destructiveMove<bsl::allocator<TARGET_TYPE> >(toBegin,
                                                  fromBegin,
                                                  fromEnd,
                                                  basicAllocator);
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=10
template <class ALLOCATOR, class... ARGS>
void ArrayPrimitives::destructiveMoveAndEmplace(
                typename bsl::allocator_traits<ALLOCATOR>::pointer  toBegin,
                typename bsl::allocator_traits<ALLOCATOR>::pointer *fromEndPtr,
                typename bsl::allocator_traits<ALLOCATOR>::pointer  fromBegin,
                typename bsl::allocator_traits<ALLOCATOR>::pointer  position,
                typename bsl::allocator_traits<ALLOCATOR>::pointer  fromEnd,
                ALLOCATOR                                           allocator,
                ARGS&&...                                           arguments)
{
    // Key to the transformation diagrams:
    //..
    //  A...H   original contents of '[fromBegin, fromEnd)'  ("source")
    //  v...v   default-constructed values                   ("input")
    //  ; ...   contents of '[toBegin, toEnd)'               ("destination")
    //  ..:..   position of 'fromEndPtr' in the input
    //  _____   uninitialized array elements
    //  [...]   part of array protected by an exception guard object
    //..

    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type TargetType;

    *fromEndPtr = fromEnd;

    // Note: Construct new element.
    //..
    //  Transformation: ABCDEFGH: ; ___________[]
    //               => ABCDEFGH: ; ____[v]____
    //..

    TargetType *toPositionBegin = toBegin + (position - fromBegin);

    bsl::allocator_traits<ALLOCATOR>::construct(
                             allocator,
                             toPositionBegin,
                             BSLS_COMPILERFEATURES_FORWARD(ARGS,arguments)...);

    TargetType *toPositionEnd = toPositionBegin + 1;

    AutoArrayDestructor<TargetType, ALLOCATOR> guard(toPositionBegin,
                                                     toPositionEnd,
                                                     allocator);

    //..
    //  Transformation: ABCDEFGH: ; ____[v]____
    //               => ABCD:____ ; ____[vEFGH]
    //..

    destructiveMove(toPositionEnd,
                    position,
                    fromEnd,
                    allocator);

    *fromEndPtr = position;  // shorten input range after partial destruction
    guard.moveEnd(fromEnd - position);  // toEnd

    //..
    //  Transformation: ABCD:____ ; ____[vEFGH]
    //               => :________ ; ABCDvEFGH[]
    //..

    destructiveMove(toBegin,
                    fromBegin,
                    position,
                    allocator);

    *fromEndPtr = fromBegin;  // empty input range after final destruction
    guard.release();
}
#endif

template <class ALLOCATOR>
void ArrayPrimitives::destructiveMoveAndInsert(
               typename bsl::allocator_traits<ALLOCATOR>::pointer  toBegin,
               typename bsl::allocator_traits<ALLOCATOR>::pointer *fromEndPtr,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  fromBegin,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  position,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  fromEnd,
               size_type                                           numElements,
               ALLOCATOR                                           allocator)
{
    // Key to the transformation diagrams:
    //..
    //  A...H   original contents of '[fromBegin, fromEnd)'  ("source")
    //  v...v   default-constructed values                   ("input")
    //  ; ...   contents of '[toBegin, toEnd)'               ("destination")
    //  ..:..   position of 'fromEndPtr' in the input
    //  _____   uninitialized array elements
    //  [...]   part of array protected by an exception guard object
    //..

    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type TargetType;

    *fromEndPtr = fromEnd;

    // Note: Construct default values.
    //..
    //  Transformation: ABCDEFGH: ; _____________[]
    //               => ABCDEFGH: ; ____[vvvvv]____
    //..

    TargetType *toPositionBegin = toBegin + (position - fromBegin);
    defaultConstruct(toPositionBegin, numElements, allocator);

    TargetType *toPositionEnd = toPositionBegin + numElements;

    AutoArrayDestructor<TargetType, ALLOCATOR> guard(toPositionBegin,
                                                     toPositionEnd,
                                                     allocator);

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

template <class TARGET_TYPE>
inline
void ArrayPrimitives::destructiveMoveAndInsert(
                                             TARGET_TYPE       *toBegin,
                                             TARGET_TYPE      **fromEndPtr,
                                             TARGET_TYPE       *fromBegin,
                                             TARGET_TYPE       *position,
                                             TARGET_TYPE       *fromEnd,
                                             size_type          numElements,
                                             bslma::Allocator  *basicAllocator)
{
    destructiveMoveAndInsert<bsl::allocator<TARGET_TYPE> >(toBegin,
                                                           fromEndPtr,
                                                           fromBegin,
                                                           position,
                                                           fromEnd,
                                                           numElements,
                                                           basicAllocator);
}

template <class ALLOCATOR>
void ArrayPrimitives::destructiveMoveAndInsert(
     typename bsl::allocator_traits<ALLOCATOR>::pointer            toBegin,
     typename bsl::allocator_traits<ALLOCATOR>::pointer           *fromEndPtr,
     typename bsl::allocator_traits<ALLOCATOR>::pointer            fromBegin,
     typename bsl::allocator_traits<ALLOCATOR>::pointer            position,
     typename bsl::allocator_traits<ALLOCATOR>::pointer            fromEnd,
     const typename bsl::allocator_traits<ALLOCATOR>::value_type&  value,
     size_type                                                     numElements,
     ALLOCATOR                                                     allocator)
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

    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type TargetType;

    *fromEndPtr = fromEnd;

    // Note: Construct copies of 'value' first in case 'value' is a reference
    // in the input range, which would be invalidated by any of the following
    // moves.
    //
    //..
    //  Transformation: ABCDEFGH: ; _____________[]
    //               => ABCDEFGH: ; ____[vvvvv]____
    //..

    TargetType *toPositionBegin = toBegin + (position - fromBegin);
    uninitializedFillN(toPositionBegin, numElements, value, allocator);

    TargetType *toPositionEnd = toPositionBegin + numElements;

    AutoArrayDestructor<TargetType, ALLOCATOR> guard(toPositionBegin,
                                                     toPositionEnd,
                                                     allocator);

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

template <class TARGET_TYPE>
void ArrayPrimitives::destructiveMoveAndInsert(
                                           TARGET_TYPE         *toBegin,
                                           TARGET_TYPE        **fromEndPtr,
                                           TARGET_TYPE         *fromBegin,
                                           TARGET_TYPE         *position,
                                           TARGET_TYPE         *fromEnd,
                                           const TARGET_TYPE&   value,
                                           size_type            numElements,
                                           bslma::Allocator    *basicAllocator)
{
    destructiveMoveAndInsert<bsl::allocator<TARGET_TYPE> >(toBegin,
                                                           fromEndPtr,
                                                           fromBegin,
                                                           position,
                                                           fromEnd,
                                                           value,
                                                           numElements,
                                                           basicAllocator);
}

template <class ALLOCATOR, class FWD_ITER>
void ArrayPrimitives::destructiveMoveAndInsert(
               typename bsl::allocator_traits<ALLOCATOR>::pointer  toBegin,
               typename bsl::allocator_traits<ALLOCATOR>::pointer *fromEndPtr,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  fromBegin,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  position,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  fromEnd,
               FWD_ITER                                            first,
               FWD_ITER                                            last,
               size_type                                           numElements,
               ALLOCATOR                                           allocator)
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

    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type TargetType;

    *fromEndPtr = fromEnd;

    // Note: Construct copies of 'value' first in case 'value' is a reference
    // in the input range, which would be invalidated by any of the following
    // moves:
    //..
    //  Transformation: ABCDEFGH: ; _________[]____
    //               => ABCDEFGH: ; ____[tuvxy]____
    //..

    TargetType *toPositionBegin = toBegin + (position - fromBegin);
    copyConstruct(toPositionBegin, first, last, allocator);

    TargetType *toPositionEnd   = toPositionBegin + numElements;

    AutoArrayDestructor<TargetType, ALLOCATOR> guard(toPositionBegin,
                                                     toPositionEnd,
                                                     allocator);

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

template <class TARGET_TYPE, class FWD_ITER>
inline
void ArrayPrimitives::destructiveMoveAndInsert(
                                             TARGET_TYPE       *toBegin,
                                             TARGET_TYPE      **fromEndPtr,
                                             TARGET_TYPE       *fromBegin,
                                             TARGET_TYPE       *position,
                                             TARGET_TYPE       *fromEnd,
                                             FWD_ITER           first,
                                             FWD_ITER           last,
                                             size_type          numElements,
                                             bslma::Allocator  *basicAllocator)
{
    destructiveMoveAndInsert<bsl::allocator<TARGET_TYPE> >(toBegin,
                                                           fromEndPtr,
                                                           fromBegin,
                                                           position,
                                                           fromEnd,
                                                           first,
                                                           last,
                                                           numElements,
                                                           basicAllocator);
}

template <class ALLOCATOR>
void ArrayPrimitives::destructiveMoveAndMoveInsert(
               typename bsl::allocator_traits<ALLOCATOR>::pointer  toBegin,
               typename bsl::allocator_traits<ALLOCATOR>::pointer *fromEndPtr,
               typename bsl::allocator_traits<ALLOCATOR>::pointer *lastPtr,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  fromBegin,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  position,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  fromEnd,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  first,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  last,
               size_type                                           numElements,
               ALLOCATOR                                           allocator)
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

    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type TargetType;

    *lastPtr = last;
    *fromEndPtr = fromEnd;

    //..
    //  Transformation: ABCDEFGH: ; _____________[]
    //               => ABCD:____ ; _________[EFGH]
    //..

    TargetType *toPositionBegin = toBegin + (position - fromBegin);
    TargetType *toPositionEnd   = toPositionBegin + numElements;

    destructiveMove(toPositionEnd,
                    position,
                    fromEnd,
                    allocator);

    *fromEndPtr = position;  // shorten input range after partial destruction

    AutoArrayDestructor<TargetType, ALLOCATOR>
                                   guard(toPositionEnd,
                                         toPositionEnd + (fromEnd - position),
                                         allocator);

    //..
    //  Transformation: ABCD:____ ; _________[EFGH]
    //               => ABCD:____ ; _____[tuvwEFGH]
    //..

    destructiveMove(toPositionBegin,
                    first,
                    last,
                    allocator);

    *lastPtr = first;
    guard.moveBegin(-static_cast<difference_type>(numElements));

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

template <class TARGET_TYPE>
inline
void ArrayPrimitives::destructiveMoveAndMoveInsert(
                                             TARGET_TYPE       *toBegin,
                                             TARGET_TYPE      **fromEndPtr,
                                             TARGET_TYPE      **lastPtr,
                                             TARGET_TYPE       *fromBegin,
                                             TARGET_TYPE       *position,
                                             TARGET_TYPE       *fromEnd,
                                             TARGET_TYPE       *first,
                                             TARGET_TYPE       *last,
                                             size_type          numElements,
                                             bslma::Allocator  *basicAllocator)
{
    destructiveMoveAndMoveInsert<bsl::allocator<TARGET_TYPE> >(toBegin,
                                                               fromEndPtr,
                                                               lastPtr,
                                                               fromBegin,
                                                               position,
                                                               fromEnd,
                                                               first,
                                                               last,
                                                               numElements,
                                                               basicAllocator);
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class ALLOCATOR, class... ARGS>
inline
void ArrayPrimitives::emplace(
                 typename bsl::allocator_traits<ALLOCATOR>::pointer  toBegin,
                 typename bsl::allocator_traits<ALLOCATOR>::pointer  toEnd,
                 ALLOCATOR                                           allocator,
                 ARGS&&...                                           args)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin,
                                                          toEnd));
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type TargetType;

    enum {
        k_VALUE = bsl::is_trivially_copyable<TargetType>::value
              ? Imp::e_BITWISE_COPYABLE_TRAITS
              : bslmf::IsBitwiseMoveable<TargetType>::value
                  ? Imp::e_BITWISE_MOVEABLE_TRAITS
                  : Imp::e_NIL_TRAITS
    };

    ArrayPrimitives_Imp::emplace(
        toBegin,
        toEnd,
        allocator,
        (bslmf::MetaInt<k_VALUE> *)0,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}

template <class TARGET_TYPE, class... ARGS>
inline
void ArrayPrimitives::emplace(TARGET_TYPE      *toBegin,
                              TARGET_TYPE      *toEnd,
                              bslma::Allocator *basicAllocator,
                              ARGS&&...         args)
{
    emplace<bsl::allocator<TARGET_TYPE> >(
                                 toBegin,
                                 toEnd,
                                 basicAllocator,
                                 BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}

#endif

template <class ALLOCATOR>
inline
void ArrayPrimitives::erase(
                  typename bsl::allocator_traits<ALLOCATOR>::pointer first,
                  typename bsl::allocator_traits<ALLOCATOR>::pointer middle,
                  typename bsl::allocator_traits<ALLOCATOR>::pointer last,
                  ALLOCATOR                                          allocator)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(first, middle));
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(middle, last));

    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type TargetType;

    if (first == middle) { // erasing empty range O(1) versus O(N): Do not
                           // remove!
        return;                                                       // RETURN
    }

    enum {
        k_VALUE = bslmf::IsBitwiseMoveable<TargetType>::value
              ? Imp::e_BITWISE_MOVEABLE_TRAITS
              : Imp::e_NIL_TRAITS
    };
    ArrayPrimitives_Imp::erase(first,
                               middle,
                               last,
                               allocator,
                               (bslmf::MetaInt<k_VALUE>*)0);
}

template <class TARGET_TYPE>
inline
void ArrayPrimitives::erase(TARGET_TYPE      *first,
                            TARGET_TYPE      *middle,
                            TARGET_TYPE      *last,
                            bslma::Allocator *basicAllocator)
{
    erase<bsl::allocator<TARGET_TYPE> >(first,
                                        middle,
                                        last,
                                        basicAllocator);
}

template <class ALLOCATOR>
inline
void ArrayPrimitives::insert(
              typename bsl::allocator_traits<ALLOCATOR>::pointer     toBegin,
              typename bsl::allocator_traits<ALLOCATOR>::pointer     toEnd,
              bslmf::MovableRef<
              typename bsl::allocator_traits<ALLOCATOR>::value_type> value,
              ALLOCATOR                                              allocator)
{
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin, toEnd));

    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type TargetType;

    if (toBegin != toEnd) {
        // Insert in the middle.  Note that there is no strong exception
        // guarantee if copy constructor, move constructor, or assignment
        // operator throw.

        enum {
            k_VALUE = bsl::is_trivially_copyable<TargetType>::value
                ? Imp::e_BITWISE_COPYABLE_TRAITS
                : bslmf::IsBitwiseMoveable<TargetType>::value
                    ? Imp::e_BITWISE_MOVEABLE_TRAITS
                    : Imp::e_NIL_TRAITS
        };

        ArrayPrimitives_Imp::shiftAndInsert(toBegin,
                                            toEnd,
                                            bslmf::MovableRefUtil::move(value),
                                            allocator,
                                            (bslmf::MetaInt<k_VALUE>*)0);
    }
    else { // toBegin == toEnd
        bsl::allocator_traits<ALLOCATOR>::construct(
            allocator, toBegin, bslmf::MovableRefUtil::move(value));
    }
}

template <class TARGET_TYPE>
inline
void ArrayPrimitives::insert(TARGET_TYPE                    *toBegin,
                             TARGET_TYPE                    *toEnd,
                             bslmf::MovableRef<TARGET_TYPE>  value,
                             bslma::Allocator               *basicAllocator)
{
    insert<bsl::allocator<TARGET_TYPE> >(toBegin,
                                         toEnd,
                                         bslmf::MovableRefUtil::move(value),
                                         basicAllocator);
}

template <class ALLOCATOR>
void ArrayPrimitives::insert(
      typename bsl::allocator_traits<ALLOCATOR>::pointer           toBegin,
      typename bsl::allocator_traits<ALLOCATOR>::pointer           toEnd,
      const typename bsl::allocator_traits<ALLOCATOR>::value_type& value,
      size_type                                                    numElements,
      ALLOCATOR                                                    allocator)
{
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin, toEnd));

    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type TargetType;

    if (0 == numElements) {
        return;                                                       // RETURN
    }

    enum {
        k_VALUE = bsl::is_trivially_copyable<TargetType>::value
              ? Imp::e_BITWISE_COPYABLE_TRAITS
              : bslmf::IsBitwiseMoveable<TargetType>::value
                  ? Imp::e_BITWISE_MOVEABLE_TRAITS
                  : Imp::e_NIL_TRAITS
    };
    ArrayPrimitives_Imp::insert(toBegin,
                                toEnd,
                                value,
                                numElements,
                                allocator,
                                (bslmf::MetaInt<k_VALUE>*)0);
}

template <class TARGET_TYPE>
inline
void ArrayPrimitives::insert(TARGET_TYPE        *toBegin,
                             TARGET_TYPE        *toEnd,
                             const TARGET_TYPE&  value,
                             size_type           numElements,
                             bslma::Allocator   *basicAllocator)
{
    insert<bsl::allocator<TARGET_TYPE> >(toBegin,
                                         toEnd,
                                         value,
                                         numElements,
                                         basicAllocator);
}

template <class ALLOCATOR, class FWD_ITER>
void ArrayPrimitives::insert(
               typename bsl::allocator_traits<ALLOCATOR>::pointer  toBegin,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  toEnd,
               FWD_ITER                                            fromBegin,
               FWD_ITER                                            fromEnd,
               size_type                                           numElements,
               ALLOCATOR                                           allocator)
{
    if (0 == numElements) {
        return;                                                       // RETURN
    }

    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type TargetType;

    BSLMF_ASSERT(!bsl::is_pointer<FWD_ITER>::value);

    typedef typename FWD_ITER::value_type FwdTarget;
        // Overload resolution will handle the case where 'FWD_ITER' is a raw
        // pointer, so we need handle only user-defined iterators.  As 'bslalg'
        // is levelized below 'bslstl' we cannot use 'iterator_traits', but
        // rely on the same property as 'iterator_traits' that this typedef
        // must be defined for any standard-conforming iterator, unless the
        // iterator explicitly specialized the 'std::iterator_traits' template.
        // In practice, iterators always prefer to provide the member typedef
        // than specialize the traits as it is a much simpler implementation,
        // so this assumption is good enough.
        //
        // Also note that as we know that 'FWD_ITER' is not a pointer, then we
        // cannot take advantage of bitwise copying as we do not have pointers
        // to pass to the 'memcpy' describing the whole range.  It is not worth
        // the effort to try to bitwise copy one element at a time.

    typedef typename bsl::remove_pointer<TargetType>::type RemovePtrTarget;
        // We want to detect the special case of copying function pointers to
        // 'void *' or 'const void *' pointers.

    enum {
        k_ITER_TO_FUNC_PTRS  = bslmf::IsFunctionPointer<FwdTarget>::value,
        k_TARGET_IS_VOID_PTR = bsl::is_pointer<TargetType>::value &&
                               bsl::is_void<RemovePtrTarget>::value,

        k_VALUE = k_ITER_TO_FUNC_PTRS && k_TARGET_IS_VOID_PTR
                ? Imp::e_IS_ITERATOR_TO_FUNCTION_POINTER
                : Imp::e_NIL_TRAITS
    };
    ArrayPrimitives_Imp::insert(toBegin,
                                toEnd,
                                fromBegin,
                                fromEnd,
                                numElements,
                                allocator,
                                (bslmf::MetaInt<k_VALUE>*)0);
}

template <class TARGET_TYPE, class FWD_ITER>
inline
void ArrayPrimitives::insert(TARGET_TYPE      *toBegin,
                             TARGET_TYPE      *toEnd,
                             FWD_ITER          fromBegin,
                             FWD_ITER          fromEnd,
                             size_type         numElements,
                             bslma::Allocator *basicAllocator)
{
    insert<bsl::allocator<TARGET_TYPE> >(toBegin,
                                         toEnd,
                                         fromBegin,
                                         fromEnd,
                                         numElements,
                                         basicAllocator);
}

template <class ALLOCATOR, class SOURCE_TYPE>
void ArrayPrimitives::insert(
               typename bsl::allocator_traits<ALLOCATOR>::pointer  toBegin,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  toEnd,
               SOURCE_TYPE                                        *fromBegin,
               SOURCE_TYPE                                        *fromEnd,
               size_type                                           numElements,
               ALLOCATOR                                           allocator)
{
    if (0 == numElements) {
        return;                                                       // RETURN
    }

    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type TargetType;

    enum {
        k_ARE_PTRS_TO_PTRS   = bsl::is_pointer<TargetType>::value &&
                               bsl::is_pointer<SOURCE_TYPE>::value,
        k_IS_BITWISEMOVEABLE = bslmf::IsBitwiseMoveable<TargetType>::value,
        k_IS_BITWISECOPYABLE =
               ArrayPrimitives_CanBitwiseCopy<SOURCE_TYPE, TargetType>::value,
        k_VALUE = k_ARE_PTRS_TO_PTRS   ? Imp::e_IS_POINTER_TO_POINTER
                : k_IS_BITWISECOPYABLE ? Imp::e_BITWISE_COPYABLE_TRAITS
                : k_IS_BITWISEMOVEABLE ? Imp::e_BITWISE_MOVEABLE_TRAITS
                : Imp::e_NIL_TRAITS
    };
    ArrayPrimitives_Imp::insert(toBegin,
                                toEnd,
                                fromBegin,
                                fromEnd,
                                numElements,
                                allocator,
                                (bslmf::MetaInt<k_VALUE>*)0);
}

template <class TARGET_TYPE, class SOURCE_TYPE>
inline
void ArrayPrimitives::insert(TARGET_TYPE      *toBegin,
                             TARGET_TYPE      *toEnd,
                             SOURCE_TYPE      *fromBegin,
                             SOURCE_TYPE      *fromEnd,
                             size_type         numElements,
                             bslma::Allocator *basicAllocator)
{
    insert<bsl::allocator<TARGET_TYPE> >(toBegin,
                                         toEnd,
                                         fromBegin,
                                         fromEnd,
                                         numElements,
                                         basicAllocator);
}

template <class ALLOCATOR>
inline
void ArrayPrimitives::moveConstruct(
                  typename bsl::allocator_traits<ALLOCATOR>::pointer toBegin,
                  typename bsl::allocator_traits<ALLOCATOR>::pointer fromBegin,
                  typename bsl::allocator_traits<ALLOCATOR>::pointer fromEnd,
                  ALLOCATOR                                          allocator)
{
    BSLS_ASSERT_SAFE(toBegin || fromBegin == fromEnd);

    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type TargetType;

    enum {
        k_VALUE = bsl::is_trivially_copyable<TargetType>::value
                  ? Imp::e_BITWISE_COPYABLE_TRAITS
                  : Imp::e_NIL_TRAITS
    };

    ArrayPrimitives_Imp::moveConstruct(toBegin,
                                       fromBegin,
                                       fromEnd,
                                       allocator,
                                       (bslmf::MetaInt<k_VALUE>*)0);
}

template <class TARGET_TYPE>
inline
void ArrayPrimitives::moveConstruct(TARGET_TYPE      *toBegin,
                                    TARGET_TYPE      *fromBegin,
                                    TARGET_TYPE      *fromEnd,
                                    bslma::Allocator *basicAllocator)
{
    moveConstruct<bsl::allocator<TARGET_TYPE> >(toBegin,
                                                fromBegin,
                                                fromEnd,
                                                basicAllocator);
}

template <class ALLOCATOR>
inline
void ArrayPrimitives::moveInsert(
               typename bsl::allocator_traits<ALLOCATOR>::pointer  toBegin,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  toEnd,
               typename bsl::allocator_traits<ALLOCATOR>::pointer *fromEndPtr,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  fromBegin,
               typename bsl::allocator_traits<ALLOCATOR>::pointer  fromEnd,
               size_type                                           numElements,
               ALLOCATOR                                           allocator)
{
    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type TargetType;

    enum {
        k_VALUE = bslmf::IsBitwiseMoveable<TargetType>::value
              ? Imp::e_BITWISE_MOVEABLE_TRAITS
              : Imp::e_NIL_TRAITS
    };
    ArrayPrimitives_Imp::moveInsert(toBegin,
                                    toEnd,
                                    fromEndPtr,
                                    fromBegin,
                                    fromEnd,
                                    numElements,
                                    allocator,
                                    (bslmf::MetaInt<k_VALUE>*)0);
}

template <class TARGET_TYPE>
inline
void ArrayPrimitives::moveInsert(TARGET_TYPE       *toBegin,
                                 TARGET_TYPE       *toEnd,
                                 TARGET_TYPE      **fromEndPtr,
                                 TARGET_TYPE       *fromBegin,
                                 TARGET_TYPE       *fromEnd,
                                 size_type          numElements,
                                 bslma::Allocator  *basicAllocator)
{
    moveInsert<bsl::allocator<TARGET_TYPE> >(toBegin,
                                             toEnd,
                                             fromEndPtr,
                                             fromBegin,
                                             fromEnd,
                                             numElements,
                                             basicAllocator);
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
        k_VALUE = bslmf::IsBitwiseMoveable<TARGET_TYPE>::value
              ? Imp::e_BITWISE_MOVEABLE_TRAITS
              : Imp::e_NIL_TRAITS
    };
    ArrayPrimitives_Imp::rotate(first,
                                middle,
                                last,
                                (bslmf::MetaInt<k_VALUE>*)0);
}

                     // --------------------------
                     // struct ArrayPrimitives_Imp
                     // --------------------------

// CLASS METHODS
template <class TARGET_TYPE>
inline
void ArrayPrimitives_Imp::assign(TARGET_TYPE *srcStart,
                                 TARGET_TYPE *srcEnd,
                                 TARGET_TYPE& value)
{
    for ( ; srcStart != srcEnd; ++srcStart) {
        *srcStart = value;
    }
}

template <class FORWARD_ITERATOR>
inline
bool ArrayPrimitives_Imp::isInvalidRange(FORWARD_ITERATOR,
                                         FORWARD_ITERATOR)
{
    // Ideally would dispatch on random_access_iterator_tag to support
    // generalized random access iterators, but we are constrained by 'bsl'
    // levelization to not depend on 'bsl_iterator.h'.  As the intent is to
    // detect invalid ranges in assertions, the conservative choice is to
    // return 'false' always.  Note that this differs from the pointers case
    // below, which also disallows empty ranges.

    return false;
}

template <class TARGET_TYPE>
inline
bool ArrayPrimitives_Imp::isInvalidRange(TARGET_TYPE *begin,
                                         TARGET_TYPE *end)
{
    return !begin != !end || begin > end;
}

template <class TARGET_TYPE>
inline
void ArrayPrimitives_Imp::reverseAssign(TARGET_TYPE *dest,
                                        TARGET_TYPE *srcStart,
                                        TARGET_TYPE *srcEnd)
{
    TARGET_TYPE *destEnd = srcEnd - srcStart + dest;
    while (srcStart != srcEnd) {
        *--destEnd = *--srcEnd;
    }
}


                   // *** 'uninitializedFillN' overloads: ***

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                      bool                                        *begin,
                      bool                                         value,
                      size_type                                    numElements,
                      void                                        *,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));
    BSLMF_ASSERT(sizeof(bool) == 1);

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numElements != 0)) {
        std::memset(reinterpret_cast<char *>(begin),  // odd, why not 'void *'?
                    static_cast<char>(value),
                    numElements);
    }
}

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                      char                                        *begin,
                      char                                         value,
                      size_type                                    numElements,
                      void                                        *,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numElements != 0)) {
        std::memset(begin, value, numElements);
    }
}

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                      unsigned char                               *begin,
                      unsigned char                                value,
                      size_type                                    numElements,
                      void                                        *,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numElements != 0)) {
        std::memset(begin, value, numElements);
    }
}

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                      signed char                                 *begin,
                      signed char                                  value,
                      size_type                                    numElements,
                      void                                        *,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numElements != 0)) {
        std::memset(begin, value, numElements);
    }
}

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                      wchar_t                                     *begin,
                      wchar_t                                      value,
                      size_type                                    numElements,
                      void                                        *,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numElements != 0)) {
        std::wmemset(begin, value, numElements);
    }
}

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                      unsigned short                              *begin,
                      unsigned short                               value,
                      size_type                                    numElements,
                      void                                        *,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    uninitializedFillN(reinterpret_cast<short *>(begin),
                       static_cast<short>(value),
                       numElements,
                       (void *)0,
                       (bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> *)0);
}

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                      unsigned int                                *begin,
                      unsigned int                                 value,
                      size_type                                    numElements,
                      void                                        *,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    uninitializedFillN(reinterpret_cast<int *>(begin),
                       static_cast<int>(value),
                       numElements,
                       (void *)0,
                       (bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> *)0);
}

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                      long                                        *begin,
                      long                                         value,
                      size_type                                    numElements,
                      void                                        *,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

#if defined(BSLS_PLATFORM_CPU_64_BIT) && !defined(BSLS_PLATFORM_OS_WINDOWS)
    uninitializedFillN(reinterpret_cast<bsls::Types::Int64 *>(begin),
                       static_cast<bsls::Types::Int64>(value),
                       numElements);
#else
    uninitializedFillN(reinterpret_cast<int *>(begin),
                       static_cast<int>(value),
                       numElements,
                       (void *)0,
                       (bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> *)0);
#endif
}

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                      unsigned long                               *begin,
                      unsigned long                                value,
                      size_type                                    numElements,
                      void                                        *,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

#if defined(BSLS_PLATFORM_CPU_64_BIT) && !defined(BSLS_PLATFORM_OS_WINDOWS)
    uninitializedFillN(reinterpret_cast<bsls::Types::Int64 *>(begin),
                       static_cast<bsls::Types::Int64>(value),
                       numElements,
                       (void *)0,
                       (bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER>*)0);
#else
    uninitializedFillN(reinterpret_cast<int *>(begin),
                       static_cast<int>(value),
                       numElements,
                       (void *)0,
                       (bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> *)0);
#endif
}

inline
void ArrayPrimitives_Imp::uninitializedFillN(
                      bsls::Types::Uint64                         *begin,
                      bsls::Types::Uint64                          value,
                      size_type                                    numElements,
                      void                                        *,
                      bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    uninitializedFillN(reinterpret_cast<bsls::Types::Int64 *>(begin),
                       value,
                       numElements,
                       (void *)0,
                       (bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> *)0);
}

template <class TARGET_TYPE>
inline
void ArrayPrimitives_Imp::uninitializedFillN(
                     TARGET_TYPE                                 **begin,
                     TARGET_TYPE                                  *value,
                     size_type                                     numElements,
                     void                                         *,
                     bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER>  *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    // Note: 'const'-correctness is respected because the next overload picks
    // up the 'const TARGET_TYPE' and will be a better match.  Note that we
    // cannot cast to 'const void **' (one would have to add 'const' at every
    // level, not just the innermost; i.e., 'const void *const *' would be
    // correct, 'const void **' is not [C++ Standard, 4.4 Qualification
    // conversions]).

    uninitializedFillN(reinterpret_cast<void **>(begin),
                       static_cast<void *>(value),
                       numElements,
                       (void *)0,
                       (bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> *)0);
}

template <class TARGET_TYPE>
inline
void ArrayPrimitives_Imp::uninitializedFillN(
                     const TARGET_TYPE                           **begin,
                     const TARGET_TYPE                            *value,
                     size_type                                     numElements,
                     void                                         *,
                     bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER>  *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    // While it seems that this overload is subsumed by the previous template,
    // SunPro does not detect it.

    uninitializedFillN(reinterpret_cast<const void **>(begin),
                       static_cast<const void *>(value),
                       numElements,
                       (void *)0,
                       (bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> *)0);
}

template <class TARGET_TYPE>
inline
void ArrayPrimitives_Imp::uninitializedFillN(
                     volatile TARGET_TYPE                        **begin,
                     volatile TARGET_TYPE                         *value,
                     size_type                                     numElements,
                     void                                         *,
                     bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER>  *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    // While it seems that this overload is subsumed by the previous template,
    // SunPro does not detect it.

    uninitializedFillN(reinterpret_cast<volatile void **>(begin),
                       static_cast<volatile void *>(value),
                       numElements,
                       (void *)0,
                       (bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> *)0);
}

template <class TARGET_TYPE>
inline
void ArrayPrimitives_Imp::uninitializedFillN(
                     const volatile TARGET_TYPE                  **begin,
                     const volatile TARGET_TYPE                   *value,
                     size_type                                     numElements,
                     void                                         *,
                     bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER>  *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    // While it seems that this overload is subsumed by the previous template,
    // SunPro does not detect it.

    uninitializedFillN(reinterpret_cast<const volatile void **>(begin),
                       static_cast<const volatile void *>(value),
                       numElements,
                       (void *)0,
                       (bslmf::MetaInt<e_IS_FUNDAMENTAL_OR_POINTER> *)0);
}

template <class TARGET_TYPE, class ALLOCATOR>
void ArrayPrimitives_Imp::uninitializedFillN(
                        TARGET_TYPE                               *begin,
                        const TARGET_TYPE&                         value,
                        size_type                                  numElements,
                        ALLOCATOR                                 *,
                        bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    if (0 == numElements) {
        return;                                                       // RETURN
    }

    const char *valueBuffer =
                    reinterpret_cast<const char *>(BSLS_UTIL_ADDRESSOF(value));

    std::memcpy((void *)begin, valueBuffer, sizeof(TARGET_TYPE));
    bitwiseFillN(reinterpret_cast<char *>(begin),
                 sizeof(TARGET_TYPE),
                 sizeof(TARGET_TYPE) * numElements);
}

template <class TARGET_TYPE, class ALLOCATOR>
void ArrayPrimitives_Imp::uninitializedFillN(
                                     TARGET_TYPE                  *begin,
                                     const TARGET_TYPE&            value,
                                     size_type                     numElements,
                                     ALLOCATOR                    *allocator,
                                     bslmf::MetaInt<e_NIL_TRAITS> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));
    BSLS_ASSERT_SAFE(allocator);

    if (0 == numElements) {
        return;                                                       // RETURN
    }
    AutoArrayDestructor<TARGET_TYPE, ALLOCATOR> guard(begin,
                                                      begin,
                                                      *allocator);

    TARGET_TYPE *end = begin + numElements;
    do {
        bsl::allocator_traits<ALLOCATOR>::construct(*allocator, begin, value);
        begin = guard.moveEnd(1);
    } while (begin != end);
    guard.release();
}

                    // *** 'copyConstruct' overloads: ***

template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::copyConstruct(
                            TARGET_TYPE                             *toBegin,
                            FWD_ITER                                 fromBegin,
                            FWD_ITER                                 fromEnd,
                            ALLOCATOR                                allocator,
                            bslmf::MetaInt<e_IS_POINTER_TO_POINTER> *)
{
    // We may be casting a function pointer to a 'void *' here, so this won't
    // work if we port to an architecture where the two are of different sizes.

    BSLMF_ASSERT(sizeof(void *) == sizeof(void (*)()));

    typedef typename bsl::remove_cv<
            typename bsl::remove_pointer<TARGET_TYPE>::type>::type NcPtrType;

    typedef typename bsl::remove_cv<
            typename bsl::remove_pointer<
            typename bsl::remove_pointer<FWD_ITER>::type>::type>::type NcIter;

#if defined(BSLALG_ARRAYPRIMITIVES_CANNOT_REMOVE_POINTER_FROM_FUNCTION_POINTER)
    // fall back on traditional C-style casts.
    copyConstruct((void *       *)toBegin,
                  (void * const *)fromBegin,
                  (void * const *)fromEnd,
                  allocator,
                  (bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS> *) 0);
#else
    copyConstruct(
     reinterpret_cast<void *       *>(const_cast<NcPtrType **>(toBegin)),
     reinterpret_cast<void * const *>(const_cast<NcIter * const *>(fromBegin)),
     reinterpret_cast<void * const *>(const_cast<NcIter * const *>(fromEnd)),
     allocator,
     (bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS> *)0);
#endif
}

template <class FWD_ITER, class ALLOCATOR>
void ArrayPrimitives_Imp::copyConstruct(
                           void                                    **toBegin,
                           FWD_ITER                                  fromBegin,
                           FWD_ITER                                  fromEnd,
                           ALLOCATOR                                 ,
                           bslmf::MetaInt<e_IS_ITERATOR_TO_FUNCTION_POINTER> *)
{
    BSLMF_ASSERT(sizeof(void *) == sizeof(void (*)()));
        // We will be casting a function pointer to a 'void *', so this won't
        // work if we port to an architecture where the two are of different
        // sizes.

    BSLS_ASSERT_SAFE(toBegin || fromBegin == fromEnd);
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(fromBegin, fromEnd));

    while (fromBegin != fromEnd) {
        // 'fromBegin' iterates over pointers to functions, which must be
        // 'reinterpret_cast' to 'void *'.

        *toBegin = reinterpret_cast<void *>(*fromBegin);
        ++fromBegin;
        ++toBegin;
    }
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::copyConstruct(
                          TARGET_TYPE                               *toBegin,
                          const TARGET_TYPE                         *fromBegin,
                          const TARGET_TYPE                         *fromEnd,
                          ALLOCATOR                                  ,
                          bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS> *)
{
    BSLS_ASSERT_SAFE(toBegin);
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(fromBegin,
                                                          fromEnd));

    const size_type numBytes = reinterpret_cast<const char*>(fromEnd)
                             - reinterpret_cast<const char*>(fromBegin);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numBytes != 0)) {
        std::memcpy((void *)toBegin, fromBegin, numBytes);
    }
}

template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
void ArrayPrimitives_Imp::copyConstruct(
                                       TARGET_TYPE                  *toBegin,
                                       FWD_ITER                      fromBegin,
                                       FWD_ITER                      fromEnd,
                                       ALLOCATOR                     allocator,
                                       bslmf::MetaInt<e_NIL_TRAITS> *)
{
    BSLS_ASSERT_SAFE(toBegin || fromBegin == fromEnd);
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(fromBegin,
                                                          fromEnd));

    AutoArrayDestructor<TARGET_TYPE, ALLOCATOR> guard(toBegin, toBegin,
                                                      allocator);

    while (fromBegin != fromEnd) {
        // Note: We are not sure the value type of 'FWD_ITER' is convertible to
        // 'TARGET_TYPE'.  Use 'construct' instead.

        bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                    toBegin,
                                                    *fromBegin);
        ++fromBegin;
        toBegin = guard.moveEnd(1);
    }
    guard.release();
}

              // *** 'moveConstruct' overloads: ***
template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::moveConstruct(
                          TARGET_TYPE                               *toBegin,
                          TARGET_TYPE                               *fromBegin,
                          TARGET_TYPE                               *fromEnd,
                          ALLOCATOR                                  ,
                          bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS> *)
{
    BSLS_ASSERT_SAFE(toBegin);
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(fromBegin,
                                                          fromEnd));

    const size_type numBytes = reinterpret_cast<const char*>(fromEnd)
                             - reinterpret_cast<const char*>(fromBegin);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numBytes != 0)) {
        std::memcpy((void *)toBegin, fromBegin, numBytes);
    }
}

template <class TARGET_TYPE, class ALLOCATOR>
void ArrayPrimitives_Imp::moveConstruct(
                                       TARGET_TYPE                  *toBegin,
                                       TARGET_TYPE                  *fromBegin,
                                       TARGET_TYPE                  *fromEnd,
                                       ALLOCATOR                     allocator,
                                       bslmf::MetaInt<e_NIL_TRAITS> *)
{
    BSLS_ASSERT_SAFE(toBegin || fromBegin == fromEnd);
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(fromBegin,
                                                          fromEnd));

    AutoArrayDestructor<TARGET_TYPE, ALLOCATOR> guard(toBegin, toBegin,
                                                      allocator);

    while (fromBegin != fromEnd) {
        bsl::allocator_traits<ALLOCATOR>::construct(
            allocator,
            toBegin,
            bslmf::MovableRefUtil::move(*fromBegin));
        ++fromBegin;
        toBegin = guard.moveEnd(1);
    }
    guard.release();
}

template <class TARGET_TYPE, class ALLOCATOR>
void ArrayPrimitives_Imp::moveIfNoexcept(
                                       TARGET_TYPE                  *toBegin,
                                       TARGET_TYPE                  *fromBegin,
                                       TARGET_TYPE                  *fromEnd,
                                       ALLOCATOR                     allocator,
                                       bslmf::MetaInt<e_NIL_TRAITS> *)
{
    BSLS_ASSERT_SAFE(toBegin || fromBegin == fromEnd);
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(fromBegin,
                                                          fromEnd));

    AutoArrayDestructor<TARGET_TYPE, ALLOCATOR> guard(toBegin, toBegin,
                                                      allocator);

    while (fromBegin != fromEnd) {
        bsl::allocator_traits<ALLOCATOR>::construct(
            allocator,
            toBegin,
            bslmf::MovableRefUtil::move_if_noexcept(*fromBegin));
        ++fromBegin;
        toBegin = guard.moveEnd(1);
    }
    guard.release();
}


              // *** 'defaultConstruct' overloads: ***

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::defaultConstruct(
                TARGET_TYPE                                       *begin,
                size_type                                          numElements,
                ALLOCATOR                                          ,
                bslmf::MetaInt<e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numElements != 0)) {
        std::memset(static_cast<void *>(begin),
                    0,
                    sizeof(TARGET_TYPE) * numElements);
    }
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::defaultConstruct(
                        TARGET_TYPE                               *begin,
                        size_type                                  numElements,
                        ALLOCATOR                                  allocator,
                        bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    if (0 < numElements) {
        bsl::allocator_traits<ALLOCATOR>::construct(allocator, begin);
        bitwiseFillN(reinterpret_cast<char *>(begin),
                     sizeof(TARGET_TYPE),
                     numElements * sizeof(TARGET_TYPE));
    }
}

template <class TARGET_TYPE, class ALLOCATOR>
void ArrayPrimitives_Imp::defaultConstruct(
                                     TARGET_TYPE                  *begin,
                                     size_type                     numElements,
                                     ALLOCATOR                     allocator,
                                     bslmf::MetaInt<e_NIL_TRAITS> *)
{
    BSLS_ASSERT_SAFE(begin || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    AutoArrayDestructor<TARGET_TYPE, ALLOCATOR> guard(begin, begin, allocator);

    const TARGET_TYPE *end = begin + numElements;
    while (begin != end) {
        bsl::allocator_traits<ALLOCATOR>::construct(allocator, begin);
        begin = guard.moveEnd(1);
    }
    guard.release();
}

                     // *** 'destructiveMove' overloads: ***

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::destructiveMove(
                          TARGET_TYPE                               *toBegin,
                          TARGET_TYPE                               *fromBegin,
                          TARGET_TYPE                               *fromEnd,
                          ALLOCATOR                                  ,
                          bslmf::MetaInt<e_BITWISE_MOVEABLE_TRAITS> *)
{
    BSLS_ASSERT_SAFE(toBegin || fromBegin == fromEnd);
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(fromBegin,
                                                          fromEnd));

    const size_type numBytes = reinterpret_cast<const char*>(fromEnd)
                             - reinterpret_cast<const char*>(fromBegin);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numBytes != 0)) {
        std::memcpy((void *)toBegin, fromBegin, numBytes);
    }
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::destructiveMove(
                                       TARGET_TYPE                  *toBegin,
                                       TARGET_TYPE                  *fromBegin,
                                       TARGET_TYPE                  *fromEnd,
                                       ALLOCATOR                     allocator,
                                       bslmf::MetaInt<e_NIL_TRAITS> *)
{
    BSLS_ASSERT_SAFE(toBegin || fromBegin == fromEnd);
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(fromBegin,
                                                          fromEnd));

    // 'TARGET_TYPE' certainly cannot be bit-wise copyable, so we can save the
    // compiler some work.

    moveIfNoexcept(toBegin, fromBegin, fromEnd, allocator,
                   (bslmf::MetaInt<e_NIL_TRAITS>*)0);
    ArrayDestructionPrimitives::destroy(fromBegin, fromEnd, allocator);
}

                  // *** 'emplace' with 'args' overloads: ***

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TARGET_TYPE, class ALLOCATOR, class... ARGS>
inline
void ArrayPrimitives_Imp::emplace(
                          TARGET_TYPE                               *toBegin,
                          TARGET_TYPE                               *toEnd,
                          ALLOCATOR                                  allocator,
                          bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS> *,
                          ARGS&&...                                  args)
{
    // TBD: The implementation is exactly the same as 'BITWISE_MOVEABLE_TRAITS'
    // unless 'AutoArrayMoveDestructor' has a 'release' method so the guard can
    // be called off after one in-place construction.  Then an optimization
    // using 'bitwiseFillN' is possible.

    ArrayPrimitives_Imp::emplace(toBegin,
                                 toEnd,
                                 allocator,
                                 (bslmf::MetaInt<e_BITWISE_MOVEABLE_TRAITS>*)0,
                                 BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}

template <class TARGET_TYPE, class ALLOCATOR, class... ARGS>
void ArrayPrimitives_Imp::emplace(
                          TARGET_TYPE                               *toBegin,
                          TARGET_TYPE                               *toEnd,
                          ALLOCATOR                                  allocator,
                          bslmf::MetaInt<e_BITWISE_MOVEABLE_TRAITS> *,
                          ARGS&&...                                  args)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin,
                                                          toEnd));
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    // TBD: fix to reflect that its only a single item....
    size_type numElements = 1;

    // Key to the transformation diagrams:
    //..
    //  A...G   original contents of '[toBegin, toEnd)'  ("tail")
    //  v...v   contents of '[fromBegin, fromEnd)'       ("input")
    //  _____   uninitialized array element
    //  [...]   part of an array guarded by an exception guard object
    //  |.(.,.) part of array guarded by move guard
    //          (middle indicated by ',' and dest by '|')
    //..

    size_type tailLen    = toEnd - toBegin;
    size_type numGuarded = tailLen < numElements ? tailLen : numElements;

    //..
    //  Transformation: ABCDE_______ => _______ABCDE (might overlap)
    //..

    TARGET_TYPE *destBegin = toBegin + numElements;
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(tailLen != 0)) {
        std::memmove((void *)destBegin,
                     toBegin,
                     tailLen * sizeof(TARGET_TYPE));
    }

    //..
    //  Transformation: |_______(,ABCDE) => vvvvv|__(ABCDE,)
    //..

    TARGET_TYPE *destEnd = toEnd + numElements;

    AutoArrayMoveDestructor<TARGET_TYPE, ALLOCATOR> guard(toBegin,
                                                          destEnd - numGuarded,
                                                          destEnd - numGuarded,
                                                          destEnd,
                                                          allocator);

    while (guard.middle() != guard.end()) {
        bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                    guard.destination(),
                                 BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
        guard.advance();
    }

    // The bitwise 'guard' is now inactive, since 'middle() == end()' and
    // 'guard.destination()' is the smaller of 'destBegin' or 'toEnd'.

    if (tailLen < numElements) {
        // There still is a gap of 'numElements - tailLen' to fill in between
        // 'toEnd' and 'destBegin'.  The elements that have been 'memmove'-ed
        // need to be guarded, we fill the gap backward from there to keep
        // guarded portion in one piece.

        AutoArrayDestructor<TARGET_TYPE, ALLOCATOR> endGuard(destBegin,
                                                             destEnd,
                                                             allocator);

        //..
        //  Transformation: vvvvv__[ABCDE] => vvvvv[vvABCDE]
        //..

        while (toEnd != destBegin) {
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        --destBegin,
                                  BSLS_COMPILERFEATURES_FORWARD(ARGS,args)...);

            endGuard.moveBegin(-1);
        }
        endGuard.release();
    }
}

template <class TARGET_TYPE, class ALLOCATOR, class... ARGS>
void ArrayPrimitives_Imp::emplace(TARGET_TYPE                  *toBegin,
                                  TARGET_TYPE                  *toEnd,
                                  ALLOCATOR                     allocator,
                                  bslmf::MetaInt<e_NIL_TRAITS> *,
                                  ARGS&&...                     args)
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

    if (toEnd > toBegin) {
        // Insert in the middle.  First, construct a temporary object from the
        // parameter pack of the strong exception guarantee if the construction
        // throws.  A welcome consequence is that the parameter pack may refer
        // (directly or indirectly) into a container element.

        bsls::ObjectBuffer<TARGET_TYPE> space;
        bsl::allocator_traits<ALLOCATOR>::construct(
            allocator,
            BSLS_UTIL_ADDRESSOF(space.object()),
            BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
        bslma::DestructorProctor<TARGET_TYPE> temp(
                                          BSLS_UTIL_ADDRESSOF(space.object()));

        //..
        //  Transformation: ABCDEFG_[] => ABCDEFG[G].
        //..

        bsl::allocator_traits<ALLOCATOR>::construct(
            allocator,
            toEnd,
            bslmf::MovableRefUtil::move_if_noexcept(*(toEnd - 1)));

        AutoArrayDestructor<TARGET_TYPE, ALLOCATOR> guard(toEnd,
                                                          toEnd + 1,
                                                          allocator);

        //..
        //  Transformation: ABCDEFG[G] => AABCDEF[G].
        //..

        TARGET_TYPE *destEnd = toEnd;
        TARGET_TYPE *srcEnd  = toEnd - 1;
        while (toBegin != srcEnd) {
            *--destEnd = bslmf::MovableRefUtil::move_if_noexcept(*--srcEnd);
        }

        //..
        //  Transformation: AABCDEFG[G] => vABCDEF[G].
        //..

        *toBegin = bslmf::MovableRefUtil::move_if_noexcept(space.object());

        guard.release();
    }
    else {
        //..
        //  Transformation: _ => v.
        //..

        bsl::allocator_traits<ALLOCATOR>::construct(
            allocator, toEnd, BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    }
}
#endif

                          // *** 'erase' overloads: ***

template <class TARGET_TYPE, class ALLOCATOR>
void ArrayPrimitives_Imp::erase(
                          TARGET_TYPE                               *first,
                          TARGET_TYPE                               *middle,
                          TARGET_TYPE                               *last,
                          ALLOCATOR                                  allocator,
                          bslmf::MetaInt<e_BITWISE_MOVEABLE_TRAITS> *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(first, middle));
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(middle, last));

    // Key to the transformation diagrams:
    //..
    //  t...z   Original contents of '[first, middle)'
    //  A...G   Original contents of '[middle, last)'
    //  _       Destroyed array element
    //..

    //..
    //  Transformation: tuvABCDEFG => ___ABCDEFG (no throw)
    //..
    ArrayDestructionPrimitives::destroy(first, middle, allocator);

    //..
    //  Transformation: ___ABCDEFG => ABCDEFG___  (might overlap, but no throw)
    //..
    size_type numBytes = reinterpret_cast<const char *>(last)
                       - reinterpret_cast<const char *>(middle);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numBytes != 0)) {
        std::memmove((void *)first, middle, numBytes);
    }
}

template <class TARGET_TYPE, class ALLOCATOR>
void ArrayPrimitives_Imp::erase(TARGET_TYPE                  *first,
                                TARGET_TYPE                  *middle,
                                TARGET_TYPE                  *last,
                                ALLOCATOR                     allocator,
                                bslmf::MetaInt<e_NIL_TRAITS> *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(first, middle));
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(middle, last));

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
        *first++ = bslmf::MovableRefUtil::move_if_noexcept(*middle++);
    }

    //..
    //  Transformation: ABCDEFGEFG => ABCDEFG___.
    //..

    ArrayDestructionPrimitives::destroy(first, middle, allocator);
}

                   // *** 'insert' with 'value' overloads: ***

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::insert(
                          TARGET_TYPE                             *toBegin,
                          TARGET_TYPE                             *toEnd,
                          const TARGET_TYPE&                       value,
                          size_type                                numElements,
                          ALLOCATOR                                allocator,
                          bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS> *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin, toEnd));
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

    const size_type numBytes = reinterpret_cast<const char*>(toEnd)
                             - reinterpret_cast<const char*>(toBegin);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numBytes != 0)) {
        std::memmove((void *)(toBegin + numElements), toBegin, numBytes);
    }

    //..
    //  Transformation: ___ABCDE => v__ABCDE (no overlap).
    //..

    // Use 'copyConstruct' instead of 'memcpy' because the former optimizes for
    // fundamental types using 'operator=' instead, which avoid the 'memcpy'
    // function call.

    bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                toBegin,
                                                *tempValuePtr);
    //..
    //  Transformation: v__ABCDE => vvvABCDE.
    //..

    bitwiseFillN(reinterpret_cast<char *>(toBegin),
                 sizeof value,
                 numElements * sizeof value);
}

template <class TARGET_TYPE, class ALLOCATOR>
void ArrayPrimitives_Imp::insert(
                          TARGET_TYPE                             *toBegin,
                          TARGET_TYPE                             *toEnd,
                          const TARGET_TYPE&                       value,
                          size_type                                numElements,
                          ALLOCATOR                                allocator,
                          bslmf::MetaInt<e_BITWISE_MOVEABLE_TRAITS> *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin, toEnd));
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
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(tailLen != 0)) {
        std::memmove((void *)destBegin,
                     toBegin,
                     tailLen * sizeof(TARGET_TYPE));
    }

    //..
    //  Transformation: |_______(,ABCDE) => vvvvv|__(ABCDE,)
    //..

    TARGET_TYPE *destEnd = toEnd + numElements;

    AutoArrayMoveDestructor<TARGET_TYPE, ALLOCATOR> guard(toBegin,
                                                          destEnd - numGuarded,
                                                          destEnd - numGuarded,
                                                          destEnd,
                                                          allocator);

    while (guard.middle() != guard.end()) {
        bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                    guard.destination(),
                                                    *tempValuePtr);
        guard.advance();
    }

    // The bitwise 'guard' is now inactive, since 'middle() == end()' and
    // 'guard.destination()' is the smaller of 'destBegin' or 'toEnd'.

    if (tailLen < numElements) {
        // There still is a gap of 'numElements - tailLen' to fill in between
        // 'toEnd' and 'destBegin'.  The elements that have been 'memmove'-ed
        // need to be guarded, we fill the gap backward from there to keep
        // guarded portion in one piece.

        AutoArrayDestructor<TARGET_TYPE, ALLOCATOR> endGuard(destBegin,
                                                             destEnd,
                                                             allocator);

        //..
        //  Transformation: vvvvv__[ABCDE] => vvvvv[vvABCDE]
        //..

        while (toEnd != destBegin) {
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        --destBegin,
                                                        *tempValuePtr);

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
                                 ALLOCATOR                   allocator,
                                 bslmf::MetaInt<e_NIL_TRAITS> *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin, toEnd));
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    // Aliasing: Make a temp copy of 'value' (always).  The reason is that
    // 'value' could be a reference inside the input range, or even outside
    // but with lifetime controlled by one of these values, and so the next
    // transformation could invalidate 'value'.  Note: One cannot rely on
    // 'TARGET_TYPE' to have a single-argument copy constructor (i.e.,
    // default allocator argument to 0) if it takes an allocator; hence the
    // constructor proxy.

    bsls::ObjectBuffer<TARGET_TYPE> space;
    bslma::ConstructionUtil::construct(BSLS_UTIL_ADDRESSOF(space.object()),
                                       bslma::Default::allocator(),
                                       value);
    bslma::DestructorProctor<TARGET_TYPE> temp(
                                          BSLS_UTIL_ADDRESSOF(space.object()));

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

        moveIfNoexcept(toEnd,                // destination
                       toEnd - numElements,  // source
                       toEnd,                // end source
                       allocator,
                       (bslmf::MetaInt<e_NIL_TRAITS> *)0);

        AutoArrayDestructor<TARGET_TYPE, ALLOCATOR> guard(toEnd,
                                                          toEnd + numElements,
                                                          allocator);

        // TBD: this does the same thing as the old code - don't like that we
        // circumvent the whole allocator thing, but for now, let's keep it
        // consistent.
        //     ConstructorProxy<TARGET_TYPE>
        //                       tempValue(value, bslma::Default::allocator());

        //..
        //  Transformation: ABCDEFG[EFG] => ABCABCD[EFG].
        //..

        TARGET_TYPE *src  = toEnd - numElements;
        TARGET_TYPE *dest = toEnd;
        while (toBegin != src) {
            *--dest = bslmf::MovableRefUtil::move_if_noexcept(*--src);
        }

        //..
        //  Transformation: ABCABCD[EFG] => vvvABCD[EFG].
        //..

        for ( ; toBegin != dest; ++toBegin) {
            *toBegin = space.object();
        }
        // TBD: this can't be good
        guard.release();
    }
    else {
        // Tail is shorter than input.  We can avoid the temp copy of value
        // since there will be space to make a first copy after the tail, and
        // use that to make the subsequent copies.
        //
        // TBD: Update comment now that the assumption is no longer true, and
        // we make a copy at the top of the call, regardless.  We could
        // restore this optimization if we use metaprogramming to check if
        // 'moveIfNoexcept' will move or copy, but not convinced it is worth
        // the complexity.

        difference_type remElements = numElements - tailLen;

        //..
        //  Transformation: ABC_______[] => ABC____[ABC].
        //..

        moveIfNoexcept(toBegin + numElements,  // destination
                       toBegin,                // source
                       toEnd,                  // end source
                       allocator,
                       (bslmf::MetaInt<e_NIL_TRAITS>*)0);

        AutoArrayDestructor<TARGET_TYPE, ALLOCATOR> guard(toEnd + remElements,
                                                          toEnd + numElements,
                                                          allocator);

        //..
        //  Transformation: ABC____[ABC] => ABC[vvvvABC].
        //..

        uninitializedFillN(toEnd,
                           space.object(),
                           remElements,
                           &allocator,
                           (bslmf::MetaInt<e_NIL_TRAITS>*)0);
        guard.moveBegin(-remElements);

        //..
        //  Transformation: ABC[vvvvABC] => vvv[vvvvABC].
        //..

        for ( ; toBegin != toEnd; ++toBegin) {
            *toBegin = space.object();
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
                          ALLOCATOR                                allocator,
                          bslmf::MetaInt<e_IS_POINTER_TO_POINTER>   *)
{
    // We may be casting a function pointer to a 'void *' here, so this won't
    // work if we port to an architecture where the two are of different sizes.

    BSLMF_ASSERT(sizeof(void *) == sizeof(void (*)()));

#if defined(BSLALG_ARRAYPRIMITIVES_CANNOT_REMOVE_POINTER_FROM_FUNCTION_POINTER)
    // fall back on traditional C-style casts.
    insert((void *       *)toBegin,
           (void *       *)toEnd,
           (void * const *)fromBegin,
           (void * const *)fromEnd,
           numElements,
           allocator,
           (bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS> *) 0);
#else
    typedef typename bsl::remove_cv<
            typename bsl::remove_pointer<TARGET_TYPE>::type>::type NcPtrType;

    typedef typename bsl::remove_cv<
            typename bsl::remove_pointer<
            typename bsl::remove_pointer<FWD_ITER>::type>::type>::type NcIter;

    insert(
     reinterpret_cast<void *       *>(const_cast<NcPtrType **>(toBegin)),
     reinterpret_cast<void *       *>(const_cast<NcPtrType **>(toEnd)),
     reinterpret_cast<void * const *>(const_cast<NcIter * const *>(fromBegin)),
     reinterpret_cast<void * const *>(const_cast<NcIter * const *>(fromEnd)),
     numElements,
     allocator,
     (bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS> *) 0);
#endif
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayPrimitives_Imp::insert(
                          TARGET_TYPE                             *toBegin,
                          TARGET_TYPE                             *toEnd,
                          const TARGET_TYPE                       *fromBegin,
                          const TARGET_TYPE                       *fromEnd,
                          size_type                                numElements,
                          ALLOCATOR                                ,
                          bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS> *)
{
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    // 'FWD_ITER' has been converted to a 'const TARGET_TYPE *' and
    // 'TARGET_TYPE' is bit-wise copyable.
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin, toEnd));
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(fromBegin, fromEnd));
    BSLS_ASSERT_SAFE(fromBegin || 0 == numElements);

    BSLS_ASSERT_SAFE(fromBegin + numElements == fromEnd);
    BSLS_ASSERT_SAFE(fromEnd <= toBegin || toEnd + numElements <= fromBegin);

    (void) fromEnd;  // quell warning when 'BSLS_ASSERT_SAFE' is compiled out

    // Key to the transformation diagrams:
    //..
    //  A...G   original contents of '[toBegin, toEnd)'  ("tail")
    //  t...z   contents of '[fromBegin, fromEnd)'       ("input")
    //  _____   uninitialized array element
    //..

    //..
    //  Transformation: ABCDE_______ => _______ABCDE (might overlap).
    //..

    const size_type numBytes = reinterpret_cast<const char*>(toEnd)
                             - reinterpret_cast<const char*>(toBegin);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numBytes != 0)) {
        std::memmove((void *)(toBegin + numElements), toBegin, numBytes);
    }

    //..
    //  Transformation: _______ABCDE => tuvwxyzABCDE (no overlap).
    //..

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(numElements != 0)) {
        std::memcpy((void *)toBegin,
                    fromBegin,
                    numElements * sizeof(TARGET_TYPE));
    }
}

template <class TARGET_TYPE, class FWD_ITER, class ALLOCATOR>
void ArrayPrimitives_Imp::insert(
                          TARGET_TYPE                             *toBegin,
                          TARGET_TYPE                             *toEnd,
                          FWD_ITER                                 fromBegin,
                          FWD_ITER,
                          size_type                                numElements,
                          ALLOCATOR                                allocator,
                          bslmf::MetaInt<e_BITWISE_MOVEABLE_TRAITS> *)
{
    // 'TARGET_TYPE' is bit-wise moveable.
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin, toEnd));
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    if (0 == numElements) {
        return;                                                       // RETURN
    }

    // The following assertions make sense only if 'FWD_ITER' is a pointer to a
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
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(tailLen != 0)) {
        std::memmove((void *)destBegin,
                     toBegin,
                     tailLen * sizeof(TARGET_TYPE));
    }

    //..
    //  Transformation: |_______(,ABCDE) => tuvwx|__(ABCDE,).
    //..

    TARGET_TYPE *destEnd = toEnd + numElements;

    AutoArrayMoveDestructor<TARGET_TYPE, ALLOCATOR> guard(toBegin,
                                                          destEnd - numGuarded,
                                                          destEnd - numGuarded,
                                                          destEnd,
                                                          allocator);

    for (; guard.middle() != guard.end(); ++fromBegin) {
        bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                    guard.destination(),
                                                    *fromBegin);
        guard.advance();
    }

    // The bitwise 'guard' is now inactive, since 'middle() == end()', and
    // 'guard.destination()' is the smaller of 'destBegin' or 'toEnd'.

    if (tailLen < numElements) {
        // There still is a gap of 'numElements - tailLen' to fill in between
        // 'toEnd' and 'destBegin'.  The elements that have been 'memmove'-ed
        // need to be guarded, and we need to continue to fill the hole at the
        // same guarding the copied elements as well.

        AutoArrayDestructor<TARGET_TYPE, ALLOCATOR> endGuard1(toEnd,
                                                              toEnd,
                                                              allocator);
        AutoArrayDestructor<TARGET_TYPE, ALLOCATOR> endGuard2(destBegin,
                                                              destEnd,
                                                              allocator);

        //..
        //  Transformation: tuvwx[]__[ABCDE] => tuvwx[yz][ABCDE].
        //..

        for (; toEnd != destBegin; ++fromBegin) {
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        toEnd,
                                                        *fromBegin);
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
                                 ALLOCATOR                   allocator,
                                 bslmf::MetaInt<e_NIL_TRAITS> *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin, toEnd));
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

        moveIfNoexcept(toEnd,                // destination
                       toEnd - numElements,  // source
                       toEnd,                // end source
                       allocator,
                       (bslmf::MetaInt<e_NIL_TRAITS>*)0);

        AutoArrayDestructor<TARGET_TYPE, ALLOCATOR> guard(toEnd,
                                                          toEnd + numElements,
                                                          allocator);

        //..
        //  Transformation: ABCDEFG[EFG] => ABCABCD[EFG].
        //..

        TARGET_TYPE *src  = toEnd - numElements;
        TARGET_TYPE *dest = toEnd;
        while (toBegin != src) {
            *--dest = bslmf::MovableRefUtil::move_if_noexcept(*--src);
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

        moveIfNoexcept(toBegin + numElements,  // destination
                       toBegin,                // source
                       toEnd,                  // end source
                       allocator,
                       (bslmf::MetaInt<e_NIL_TRAITS>*)0);

        AutoArrayDestructor<TARGET_TYPE, ALLOCATOR> guard(toEnd + remElements,
                                                          toEnd + numElements,
                                                          allocator);

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
                      (bslmf::MetaInt<e_NIL_TRAITS>*)0);

        guard.release();
    }
}

template <class FWD_ITER, class ALLOCATOR>
void ArrayPrimitives_Imp::insert(
                           void                                  **toBegin,
                           void                                  **toEnd,
                           FWD_ITER                                fromBegin,
                           FWD_ITER,
                           size_type                               numElements,
                           ALLOCATOR                               ,
                           bslmf::MetaInt<e_IS_ITERATOR_TO_FUNCTION_POINTER> *)
{
    // This very specific overload is required for the case that 'FWD_ITER' is
    // an iterator that is not a pointer, iterating over a sequence of function
    // pointers.  The implementation relies on the conditionally-supported
    // behavior that any function pointer can be 'reinterpret_cast' to
    // 'void *'.

    // 'TARGET_TYPE' is bit-wise moveable.
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin, toEnd));
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));

    if (0 == numElements) {
        return;                                                       // RETURN
    }

    // The following assertions make sense only if 'FWD_ITER' is a pointer to a
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

    //..
    //  Transformation: ABCDE____ => ____ABCDE (might overlap).
    //..

    void **destBegin = toBegin + numElements;

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(tailLen != 0)) {
        std::memmove(destBegin, toBegin, tailLen * sizeof(void **));
    }

    for (size_type i = 0; i < numElements; ++i) {
        *toBegin = reinterpret_cast<void *>(*fromBegin);

        ++fromBegin;
        ++toBegin;
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
                         ALLOCATOR                                 allocator,
                         bslmf::MetaInt<e_BITWISE_MOVEABLE_TRAITS>  *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin, toEnd));
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(first, last));
    BSLS_ASSERT_SAFE(first || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));
    BSLS_ASSERT_SAFE(lastPtr);

    // Functionally indistinguishable from this:

    *lastPtr = last;
    insert(toBegin, toEnd, first, last, numElements, allocator,
           (bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS>*)0);
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
                                     ALLOCATOR                    allocator,
                                     bslmf::MetaInt<e_NIL_TRAITS>  *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(toBegin, toEnd));
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(first, last));
    BSLS_ASSERT_SAFE(first || 0 == numElements);
    BSLMF_ASSERT((bsl::is_same<size_type, std::size_t>::value));
    BSLS_ASSERT_SAFE(lastPtr);

    // There isn't any advantage at destroying [first,last) one by one as we're
    // moving it, except perhaps for slightly better memory usage.

    *lastPtr = last;
    insert(toBegin, toEnd, first, last, numElements, allocator,
           (bslmf::MetaInt<e_NIL_TRAITS>*)0);
    ArrayDestructionPrimitives::destroy(first, last, allocator);
    *lastPtr = first;
}

                         // *** 'rotate' overloads: ***

template <class TARGET_TYPE>
inline
void ArrayPrimitives_Imp::rotate(
                               TARGET_TYPE                             *begin,
                               TARGET_TYPE                             *middle,
                               TARGET_TYPE                             *end,
                               bslmf::MetaInt<e_BITWISE_MOVEABLE_TRAITS> *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(begin, middle));
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(middle, end));

    bitwiseRotate(reinterpret_cast<char *>(begin),
                  reinterpret_cast<char *>(middle),
                  reinterpret_cast<char *>(end));
}

template <class TARGET_TYPE>
void ArrayPrimitives_Imp::rotate(TARGET_TYPE                *begin,
                                 TARGET_TYPE                *middle,
                                 TARGET_TYPE                *end,
                                 bslmf::MetaInt<e_NIL_TRAITS> *)
{
    BSLS_ASSERT_SAFE(!ArrayPrimitives_Imp::isInvalidRange(begin, middle));
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
            //  (length - numElements) / numCycles = remElements / numCycles
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
            //                                      numElements / numCycles - 1
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

                         // *** 'shiftAndInsert' overloads: ***

template <class ALLOCATOR>
inline
void ArrayPrimitives_Imp::shiftAndInsert(
             typename bsl::allocator_traits<ALLOCATOR>::pointer      begin,
             typename bsl::allocator_traits<ALLOCATOR>::pointer      end,
             bslmf::MovableRef<
             typename bsl::allocator_traits<ALLOCATOR>::value_type>  value,
             ALLOCATOR                                               allocator,
             bslmf::MetaInt<e_BITWISE_COPYABLE_TRAITS>              *)
{
    BSLS_ASSERT_SAFE(begin != end); // the range is non-empty

    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type ValueType;

    // ALIASING: If 'value' is a reference into the array '[begin, end)',
    // then moving the array first might introduce a change in 'value'.
    // Fortunately we can easily predict its new position after the shift.

    ValueType *valuePtr =
                     BSLS_UTIL_ADDRESSOF(bslmf::MovableRefUtil::access(value));
    if (begin <= valuePtr && valuePtr < end) {
        valuePtr += 1; // new address after the shift
    }

#if defined(BSLS_PLATFORM_PRAGMA_GCC_DIAGNOSTIC_GCC)
// clang does not support this pragma
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif

    // shift
    std::memmove(begin + 1, begin, (end - begin) * sizeof(ValueType));

    // insert
    bsl::allocator_traits<ALLOCATOR>::construct(
                           allocator,
                           begin,
                           bslmf::MovableRefUtil::move_if_noexcept(*valuePtr));

#if defined(BSLS_PLATFORM_PRAGMA_GCC_DIAGNOSTIC_GCC)
#pragma GCC diagnostic pop
#endif
}

template <class ALLOCATOR>
inline
void ArrayPrimitives_Imp::shiftAndInsert(
             typename bsl::allocator_traits<ALLOCATOR>::pointer      begin,
             typename bsl::allocator_traits<ALLOCATOR>::pointer      end,
             bslmf::MovableRef<
             typename bsl::allocator_traits<ALLOCATOR>::value_type>  value,
             ALLOCATOR                                               allocator,
             bslmf::MetaInt<e_BITWISE_MOVEABLE_TRAITS>              *)
{
    BSLS_ASSERT_SAFE(begin != end); // the range is non-empty

    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type ValueType;

    // ALIASING: If 'value' is a reference into the array '[begin, end)',
    // then moving the array first might introduce a change in 'value'.
    // Fortunately we can easily predict its new position after the shift.

    ValueType *valuePtr =
                     BSLS_UTIL_ADDRESSOF(bslmf::MovableRefUtil::access(value));
    if (begin <= valuePtr && valuePtr < end) {
        valuePtr += 1; // new address after the shift
    }

    // shift
    size_t bytesNum = (end - begin) * sizeof(ValueType);


#if defined(BSLS_PLATFORM_PRAGMA_GCC_DIAGNOSTIC_GCC)
// clang does not support this pragma
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
    std::memmove(begin + 1, begin, bytesNum);


    class ElementsProctor {
        // Moves the elements back if 'construct' throws.

        // DATA
        ValueType *d_begin;
        size_t     d_bytesNum;
      public:
        // CREATORS
        ElementsProctor(ValueType *p, size_t n) : d_begin(p), d_bytesNum(n) {}
        ~ElementsProctor()
        {
            if(d_bytesNum) std::memmove(d_begin, d_begin + 1, d_bytesNum);
        }
        // MANIPULATORS
        void release() { d_bytesNum = 0; }
    } proctor(begin, bytesNum);


    // insert
    bsl::allocator_traits<ALLOCATOR>::construct(
                           allocator,
                           begin,
                           bslmf::MovableRefUtil::move_if_noexcept(*valuePtr));
    proctor.release();
#if defined(BSLS_PLATFORM_PRAGMA_GCC_DIAGNOSTIC_GCC)
#pragma GCC diagnostic pop
#endif
}

template <class ALLOCATOR>
inline
void ArrayPrimitives_Imp::shiftAndInsert(
             typename bsl::allocator_traits<ALLOCATOR>::pointer      begin,
             typename bsl::allocator_traits<ALLOCATOR>::pointer      end,
             bslmf::MovableRef<
             typename bsl::allocator_traits<ALLOCATOR>::value_type>  value,
             ALLOCATOR                                               allocator,
             bslmf::MetaInt<e_NIL_TRAITS>                           *)
{
    BSLS_ASSERT_SAFE(begin != end); // the range is non-empty

    typedef typename bsl::allocator_traits<ALLOCATOR>::value_type ValueType;

    // ALIASING: If 'value' is a reference into the array '[begin, end)',
    // then moving the array first might introduce a change in 'value'.
    // Fortunately we can easily predict its new position after the shift.

    ValueType *valuePtr =
                     BSLS_UTIL_ADDRESSOF(bslmf::MovableRefUtil::access(value));
    if (begin <= valuePtr && valuePtr < end) {
        valuePtr += 1; // new address after the shift
    }

    // Key to the transformation diagrams:
    //..
    //  A...G   original contents of '[toBegin, toEnd)'  ("tail")
    //  a...g   moved-from or copied values
    //  v       moved 'value'                            ("input")
    //  _____   uninitialized array elements
    //  [...]   part of array protected by an exception guard object
    //..

    //..
    //  Transformation: ABCDEFG_[] => ABCDEFg[G].
    //..

    bsl::allocator_traits<ALLOCATOR>::construct(
                          allocator,
                          end,
                          bslmf::MovableRefUtil::move_if_noexcept(*(end - 1)));

    bslalg::AutoArrayDestructor<ValueType, ALLOCATOR> guard(
                                                      end, end + 1, allocator);

    //..
    //  Transformation: ABCDEFg[G] => aABCDEF[G].
    //..

    ValueType *dst = end;
    ValueType *src = end - 1;
    while (src != begin) {
        *--dst = bslmf::MovableRefUtil::move_if_noexcept(*--src);
    }

    //..
    //  Transformation: aABCDEFG[G] => vABCDEF[G].
    //..

    *begin = bslmf::MovableRefUtil::move_if_noexcept(*valuePtr);

    guard.release();
}

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

typedef bslalg::ArrayPrimitives bslalg_ArrayPrimitives;
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#if defined(BSLALG_ARRAYPRIMITIVES_CANNOT_REMOVE_POINTER_FROM_FUNCTION_POINTER)
# undef BSLALG_ARRAYPRIMITIVES_CANNOT_REMOVE_POINTER_FROM_FUNCTION_POINTER
#endif

#endif // End C++11 code

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
