// bslalg_dequeprimitives.h                                           -*-C++-*-
#ifndef INCLUDED_BSLALG_DEQUEPRIMITIVES
#define INCLUDED_BSLALG_DEQUEPRIMITIVES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide primitive algorithms that operate on deques.
//
//@CLASSES:
//  bslalg::DequePrimitives: namespace for deque algorithms
//
//@SEE_ALSO: bslalg_scalarprimitives, bslalg_arrayprimitives
//
//@DESCRIPTION: This component provides utilities to initialize, destroy, move,
// and otherwise perform various primitive manipulations on deques with a
// uniform interface, but selecting a different implementation according to the
// various traits possessed by the underlying type, and selecting a simpler
// implementation when there is only one element per deque block.  See the
// 'bslalg_dequeimputil' and 'bslalg_dequeiterator' for a definition and visual
// depiction of the inner organization of a deque, and of the iterator type
// used to refer to elements in the deque.
//
// The primitives provided by this component are exceptionally useful for
// implementing generic block-based components such as deques.  A short
// synopsis is provided below describing the observable behavior and mentioning
// the relevant traits.  See the full function-level contract for detailed
// description, including exception-safety guarantees.  In the description
// below, 'Sc' stands for 'bslalg::ScalarPrimitives' and 'Ar' stands for
// 'bslalg::ArrayPrimitives' (for brevity).  Note that some algorithms are
// explained in terms of previous algorithms.
//..
//  Algorithm                     Short description of observable behavior
//  ----------------------------  ---------------------------------------------
//  destruct                      'Sc::destruct' for each element in the target
//                                range.
//
//  erase                         'destruct' for each element in the target
//                                range, or 'no-op' if bit-wise copyable, then
//                                shift the remaining elements from either the
//                                front or the back to fill hole.
//
//  uninitializedFillNBack        'Ar::uninitializedFillN' for each block at
//                                the end of the deque
//
//  uninitializedFillNFront       'Ar::uninitializedFillN' for each block at
//                                the front of the deque.
//
//  insertAndMoveToBack           'Sc::copyConstruct' each element in the
//                                target range, or 'std::memmove' if type is
//                                bit-wise moveable, to the back of the deque
//                                to create a hole, followed by
//                                'Sc::copyConstruct' or 'std::memmove' of
//                                target value or range to fill the hole.
//
//  insertAndMoveToFront          'Sc::copyConstruct' each element in the
//                                target range, or 'std::memmove' if type is
//                                bit-wise moveable, to the front of the deque
//                                to create a hole, followed by
//                                'Sc::copyConstruct' or 'std::memmove' of
//                                target value or range to fill the hole.
//..
// The traits under consideration directly or indirectly by this component are:
//..
//  Trait                                         English description
//  -----                                         -------------------
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
// the reference 'const VALUE_TYPE& value' argument, which may belong to a
// range that will be modified during the course of the operation.  All such
// aliasing concerns are taken care of properly.  Other aliasing concerns due
// to the copying or a range '[first, last)' are *not* taken care of, since
// their intended use is for range assignments and insertions in standard
// containers, for which the standard explicitly says that 'first' and 'last'
// shall not be iterators into the container.
//
///Usage
///-----
// This component is for use by the 'bslstl' package.  Other clients should use
// the STL deque (in header '<deque>').

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_ARRAYPRIMITIVES
#include <bslalg_arrayprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#include <bslalg_constructorproxy.h>
#endif

#ifndef INCLUDED_BSLALG_DEQUEIMPUTIL
#include <bslalg_dequeimputil.h>
#endif

#ifndef INCLUDED_BSLALG_DEQUEITERATOR
#include <bslalg_dequeiterator.h>
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

#ifndef INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS
#include <bslmf_functionpointertraits.h>
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

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // std::size_t
#define INCLUDED_CSTDDEF
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>  // memmove
#define INCLUDED_CSTRING
#endif

namespace BloombergLP {

namespace {
        // Workaround for windows.  The windows compiler refuses to recognize
        // enum declarations within a templated class.

    enum {
        // These constants are used in the overloads below, when the last
        // argument is of type 'bslmf::MetaInt<N> *', indicating that
        // 'VALUE_TYPE' has the traits for which the enumerator equal to 'N' is
        // named.

        NIL_TRAITS              = 0,
        BITWISE_MOVEABLE_TRAITS = 1,
        BITWISE_COPYABLE_TRAITS = 2,
        NON_NIL_TRAITS          = 3
    };

}  // close unnamed namespace

namespace bslalg {

template <class VALUE_TYPE, int BLOCK_LENGTH>
class DequePrimitives_DequeElementGuard;

template <class VALUE_TYPE, int BLOCK_LENGTH>
class DequePrimitives_DequeMoveGuard;

template <class VALUE_TYPE, int BLOCK_LENGTH>
class DequePrimitives_ExternalDequeElementGuard;

                        // ======================
                        // struct DequePrimitives
                        // ======================

template <class VALUE_TYPE, int BLOCK_LENGTH>
struct DequePrimitives {
    // This 'struct' provides a namespace for a suite of utility functions that
    // operate on deques parameterized by the 'VALUE_TYPE' and 'BLOCK_LENGTH'.
    // Depending on the traits of 'VALUE_TYPE', the default and copy
    // constructors, destructor, assignment operators, etc. may not be invoked,
    // and instead the operation can be optimized using a no-op, bit-wise move
    // or copy.

    // PUBLIC TYPES
    typedef std::size_t                                         size_type;

    typedef DequeIterator<VALUE_TYPE, BLOCK_LENGTH>             Iterator;

  private:
    // PRIVATE TYPES
    typedef DequeImpUtil<VALUE_TYPE, BLOCK_LENGTH>              ImpUtil;

    typedef DequePrimitives_DequeElementGuard<VALUE_TYPE,
                                       BLOCK_LENGTH>            Guard;

    typedef DequePrimitives_DequeMoveGuard<VALUE_TYPE,
                                     BLOCK_LENGTH>              MGuard;

    typedef DequePrimitives_ExternalDequeElementGuard<VALUE_TYPE,
                                          BLOCK_LENGTH>         EGuard;

  public:
    // CLASS METHODS
    static void destruct(Iterator begin, Iterator end);
        // Call the destructor on each of the elements of a deque of
        // parameterized 'VALUE_TYPE' in the specified range [begin, end).  The
        // behavior is undefined unless 'begin <= end'.  Note that this does
        // not deallocate any memory (except memory deallocated by the element
        // destructor calls).

    static void destruct(Iterator begin, Iterator end,
                         bslmf::MetaInt<NIL_TRAITS> *);
    static void destruct(Iterator begin, Iterator end,
                         bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *);
        // Call the destructor on each of the elements of a deque of
        // parameterized 'VALUE_TYPE' in the specified range [begin, end).  The
        // behavior is undefined unless 'begin <= end'.  Note that this does
        // not deallocate any memory (except memory deallocated by the element
        // destructor calls).  Note that the last argument is for removing
        // overload ambiguities and is not used.

    template <class ALLOCATOR>
    static Iterator erase(Iterator  *toBegin,
                          Iterator  *toEnd,
                          Iterator   fromBegin,
                          Iterator   first,
                          Iterator   last,
                          Iterator   fromEnd,
                          ALLOCATOR *allocator);
        // Call the destructor on each of the elements of a deque of
        // parameterized 'VALUE_TYPE' in the specified range [first, last).
        // Shift the elements to fill up the empty space after the erasure,
        // using the smaller of the range defined by [fromBegin, first) and
        // [last, fromEnd) after the erasure.  Load in the specified 'toBegin'
        // and 'toEnd' the new boundaries of the deque after erasure and return
        // an iterator pointing to the element immediately following the
        // removed elements.  The behavior is undefined unless
        // 'fromBegin <= first <= last <= fromEnd'.

    template <class ALLOCATOR>
    static Iterator erase(Iterator                   *toBegin,
                          Iterator                   *toEnd,
                          Iterator                    fromBegin,
                          Iterator                    first,
                          Iterator                    last,
                          Iterator                    fromEnd,
                          ALLOCATOR                  *allocator,
                          bslmf::MetaInt<NIL_TRAITS> *);
    template <class ALLOCATOR>
    static Iterator erase(Iterator                                *toBegin,
                          Iterator                                *toEnd,
                          Iterator                                 fromBegin,
                          Iterator                                 first,
                          Iterator                                 last,
                          Iterator                                 fromEnd,
                          ALLOCATOR                               *allocator,
                          bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *);
        // Call the destructor on each of the elements of a deque of
        // parameterized 'VALUE_TYPE' in the specified range [first, last).
        // Shift the elements from the smaller of the specified range
        // [fromBegin, first) and [last, fromEnd) to fill up the empty spaces
        // after the erasure.  Load in the specified 'toBegin' and 'toEnd' the
        // new boundaries of the deque after erasure and return an iterator
        // pointing to the element immediately following the removed elements.
        // The behavior is undefined unless
        // 'fromBegin <= first <= last <= fromEnd'.  Note that the last
        // argument is for removing overload ambiguities and is not used.

    template <class ALLOCATOR>
    static void insertAndMoveToBack(Iterator          *toEnd,
                                    Iterator           fromEnd,
                                    Iterator           position,
                                    size_type          numElements,
                                    const VALUE_TYPE&  value,
                                    ALLOCATOR         *allocator);
        // Insert the specified 'numElements' copies of the specified 'value'
        // at the specified 'position', by moving the elements in the range
        // [position, fromEnd) forward by 'numElements' position.  Pass the
        // specified 'allocator' to the copy constructor if appropriate.  Load
        // into the specified 'toEnd' an iterator to the end of the deque after
        // insertion (i.e., 'fromEnd + numElements').  The behavior is
        // undefined unless 'fromEnd + numElements' is a valid iterator (i.e.,
        // the block pointer array holds enough room after the 'fromEnd'
        // position to insert 'numElements').

    template <class ALLOCATOR>
    static void insertAndMoveToBack(
                          Iterator                                *toEnd,
                          Iterator                                 fromEnd,
                          Iterator                                 position,
                          size_type                                numElements,
                          const VALUE_TYPE&                        value,
                          ALLOCATOR                               *allocator,
                          bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *);
    template <class ALLOCATOR>
    static void insertAndMoveToBack(
                          Iterator                                *toEnd,
                          Iterator                                 fromEnd,
                          Iterator                                 position,
                          size_type                                numElements,
                          const VALUE_TYPE&                        value,
                          ALLOCATOR                               *allocator,
                          bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *);
    template <class ALLOCATOR>
    static void insertAndMoveToBack(Iterator                   *toEnd,
                                    Iterator                    fromEnd,
                                    Iterator                    position,
                                    size_type                   numElements,
                                    const VALUE_TYPE&           value,
                                    ALLOCATOR                  *allocator,
                                    bslmf::MetaInt<NIL_TRAITS> *);
        // Insert the specified 'numElements' copies of the specified 'value'
        // at the specified 'position', by moving the elements in the range
        // [position, fromEnd) forward by 'numElements' position.  Pass the
        // specified 'allocator' to the copy constructor if appropriate.  Load
        // into the specified 'toEnd' an iterator to the end of the deque after
        // insertion (i.e., 'fromEnd + numElements').  The behavior is
        // undefined unless 'fromEnd + numElements' is a valid iterator (i.e.,
        // the block pointer array holds enough room after the 'fromEnd'
        // position to insert 'numElements').  Note that the last argument is
        // for removing overload ambiguities and is not used.

    template <class ALLOCATOR>
    static void insertAndMoveToFront(Iterator          *toBegin,
                                     Iterator           fromBegin,
                                     Iterator           position,
                                     size_type          numElements,
                                     const VALUE_TYPE&  value,
                                     ALLOCATOR         *allocator);
        // Insert the specified 'numElements' copies of the specified 'value'
        // at the specified 'position', by moving the elements in the range
        // [fromBegin, position) backward by 'numElements' position.  Pass the
        // specified 'allocator' to the copy constructor if appropriate.  Load
        // into the specified 'toBegin' an iterator to the beginning of the
        // data after insertion (i.e., 'fromBegin - numElements').  The
        // behavior is undefined unless 'fromBegin - numElements' is a valid
        // iterator (i.e., the block pointer array holds enough room before the
        // 'fromBegin' position to insert 'numElements').

    template <class ALLOCATOR>
    static void insertAndMoveToFront(
                          Iterator                                *toBegin,
                          Iterator                                 fromBegin,
                          Iterator                                 position,
                          size_type                                numElements,
                          const VALUE_TYPE&                        value,
                          ALLOCATOR                               *allocator,
                          bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *);
    template <class ALLOCATOR>
    static void insertAndMoveToFront(
                          Iterator                                *toBegin,
                          Iterator                                 fromBegin,
                          Iterator                                 position,
                          size_type                                numElements,
                          const VALUE_TYPE&                        value,
                          ALLOCATOR                               *allocator,
                          bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *);
    template <class ALLOCATOR>
    static void insertAndMoveToFront(Iterator                   *toBegin,
                                     Iterator                    fromBegin,
                                     Iterator                    position,
                                     size_type                   numElements,
                                     const VALUE_TYPE&           value,
                                     ALLOCATOR                  *allocator,
                                     bslmf::MetaInt<NIL_TRAITS> *);
        // Insert the specified 'numElements' copies of the specified 'value'
        // at the specified 'position', by moving the elements in the range
        // [fromBegin, position) backward by 'numElements' position.  Pass the
        // specified 'allocator' to the copy constructor if appropriate.  Load
        // into the specified 'toBegin' an iterator to the beginning of the
        // data after insertion (i.e., 'fromBegin - numElements'.  The behavior
        // is undefined unless 'fromBegin - numElements' is a valid iterator
        // (i.e., the block pointer array holds enough room before the
        // 'fromBegin' position to insert 'numElements').  Note that the last
        // argument is for removing overload ambiguities and is not used.

    template <class FWD_ITER, class ALLOCATOR>
    static void insertAndMoveToBack(Iterator  *toEnd,
                                    Iterator   fromEnd,
                                    Iterator   position,
                                    FWD_ITER   first,
                                    FWD_ITER   last,
                                    size_type  numElements,
                                    ALLOCATOR *allocator);
        // Insert the specified 'numElements' in the range [first, last) at the
        // specified 'position', by moving the elements in the range [position,
        // fromEnd) forward by 'numElements' position.  Pass the specified
        // 'allocator' to the copy constructor if appropriate.  Load into the
        // specified 'toEnd' an iterator to the end of the data after insertion
        // (i.e., 'fromEnd + numElements').  The behavior is undefined unless
        // 'fromEnd + numElements' is a valid iterator (i.e., the block pointer
        // array holds enough room after the 'fromEnd' position to insert
        // 'numElements').

    template <class FWD_ITER, class ALLOCATOR>
    static void insertAndMoveToFront(Iterator  *toBegin,
                                     Iterator   fromBegin,
                                     Iterator   position,
                                     FWD_ITER   first,
                                     FWD_ITER   last,
                                     size_type  numElements,
                                     ALLOCATOR *allocator);
        // Insert the specified 'numElements' in the range [first, last) at the
        // specified 'position', by moving the elements in the range
        // [fromBegin, position) backward by 'numElements' position.  Pass the
        // specified 'allocator' to the copy constructor if appropriate.  Load
        // into the specified 'toBegin' an iterator to the end of the data
        // after insertion (i.e., 'fromBegin - numElements').  The behavior is
        // undefined unless 'fromBegin - numElements' is a valid iterator
        // (i.e., the block pointer array holds enough room before the
        // 'fromBefore' position to insert 'numElements').

    static void moveBack(Iterator  *destination,
                         Iterator  *source,
                         size_type  numElements);
        // Move the specified 'numElements' from the specified 'source' to the
        // specified 'destination' using 'std::memmove'.  Also load into
        // 'destination' the value 'destination - numElements' and 'source' the
        // value 'source - numElements'.  The behavior is undefined unless
        // 'destination >= source'.

    static void moveFront(Iterator  *destination,
                          Iterator  *source,
                          size_type  numElements);
        // Move the specified 'numElements' from the specified 'source' to the
        // specified 'destination' using 'std::memmove'.  Also load into
        // 'destination' the value 'destination + numElements' and 'source' the
        // the value 'source + numElements'.  The behavior is undefined unless
        // 'destination <= source'.

    template <class ALLOCATOR>
    static void uninitializedFillNBack(Iterator          *toEnd,
                                       Iterator           fromEnd,
                                       size_type          numElements,
                                       const VALUE_TYPE&  value,
                                       ALLOCATOR         *allocator);
        // Append the specified 'numElements' copies of the specified 'value'
        // to the deque ending at the specified 'fromEnd' iterator, passing the
        // specified 'allocator' through to the new elements, and load into the
        // specified 'toEnd' an iterator pointing to the end of the data after
        // appending (i.e., 'fromEnd + numElements').  The behavior is
        // undefined unless 'fromEnd + numElements' is a valid iterator (i.e.,
        // the block pointer array holds enough room after the 'fromEnd'
        // position to insert 'numElements').

    template <class ALLOCATOR>
    static void uninitializedFillNFront(Iterator          *toBegin,
                                        Iterator           fromBegin,
                                        size_type          numElements,
                                        const VALUE_TYPE&  value,
                                        ALLOCATOR         *allocator);
        // Prepend the specified 'numElements' copies of the specified 'value'
        // to the deque starting at the specified 'fromBegin' iterator, passing
        // the specified 'allocator' through to the new elements, and load into
        // the specified 'toBegin' an iterator pointing to the end of the data
        // after prepending, i.e., 'fromBegin - numElements'.  The behavior is
        // undefined unless 'fromBegin - numElements' is a valid iterator
        // (i.e., the block pointer array holds enough room before the
        // 'fromBegin' position to insert 'numElements').
};

// PARTIAL SPECIALIZATION
template <class VALUE_TYPE>
struct DequePrimitives<VALUE_TYPE, 1> {
    // This is a partial specialization of 'DequePrimitives' for the case when
    // there is a single element per block.

    // PUBLIC TYPES
    typedef std::size_t                                         size_type;
    typedef DequeImpUtil<VALUE_TYPE, 1>                  ImpUtil;
    typedef DequeIterator<VALUE_TYPE, 1>                 Iterator;
    typedef DequePrimitives_DequeElementGuard<VALUE_TYPE, 1>
                                                                Guard;

    // CLASS METHODS
    static void destruct(Iterator begin, Iterator end);

    template <class ALLOCATOR>
    static Iterator erase(Iterator  *toBegin,
                          Iterator  *toEnd,
                          Iterator   fromBegin,
                          Iterator   first,
                          Iterator   last,
                          Iterator   fromEnd,
                          ALLOCATOR *allocator);

    template <class ALLOCATOR>
    static void insertAndMoveToBack(Iterator          *toEnd,
                                    Iterator           fromEnd,
                                    Iterator           position,
                                    size_type          numElements,
                                    const VALUE_TYPE&  value,
                                    ALLOCATOR         *allocator);

    template <class FWD_ITER, class ALLOCATOR>
    static void insertAndMoveToBack(Iterator  *toEnd,
                                    Iterator   fromEnd,
                                    Iterator   position,
                                    FWD_ITER   first,
                                    FWD_ITER   last,
                                    size_type  numElements,
                                    ALLOCATOR *allocator);

    template <class ALLOCATOR>
    static void insertAndMoveToFront(Iterator          *toBegin,
                                     Iterator           fromBegin,
                                     Iterator           position,
                                     size_type          numElements,
                                     const VALUE_TYPE&  value,
                                     ALLOCATOR         *allocator);

    template <class FWD_ITER, class ALLOCATOR>
    static void insertAndMoveToFront(Iterator  *toBegin,
                                     Iterator   fromBegin,
                                     Iterator   position,
                                     FWD_ITER   first,
                                     FWD_ITER   last,
                                     size_type  numElements,
                                     ALLOCATOR *allocator);

    template <class ALLOCATOR>
    static void uninitializedFillNBack(Iterator          *toEnd,
                                       Iterator           fromEnd,
                                       size_type          numElements,
                                       const VALUE_TYPE&  value,
                                       ALLOCATOR         *allocator);
    template <class ALLOCATOR>
    static void uninitializedFillNBack(Iterator                   *toEnd,
                                       Iterator                    fromEnd,
                                       size_type                   numElements,
                                       const VALUE_TYPE&           value,
                                       ALLOCATOR                  *allocator,
                                       bslmf::MetaInt<NIL_TRAITS> *);
    template <class ALLOCATOR>
    static void uninitializedFillNBack(
                                   Iterator                       *toEnd,
                                   Iterator                        fromEnd,
                                   size_type                       numElements,
                                   const VALUE_TYPE&               value,
                                   ALLOCATOR                      *allocator,
                                   bslmf::MetaInt<NON_NIL_TRAITS> *);

    template <class ALLOCATOR>
    static void uninitializedFillNFront(Iterator          *toBegin,
                                        Iterator           fromBegin,
                                        size_type          numElements,
                                        const VALUE_TYPE&  value,
                                        ALLOCATOR         *allocator);
    template <class ALLOCATOR>
    static void uninitializedFillNFront(
                                       Iterator                   *toBegin,
                                       Iterator                    fromBegin,
                                       size_type                   numElements,
                                       const VALUE_TYPE&           value,
                                       ALLOCATOR                  *allocator,
                                       bslmf::MetaInt<NIL_TRAITS> *);
    template <class ALLOCATOR>
    static void uninitializedFillNFront(
                                   Iterator                       *toBegin,
                                   Iterator                        fromBegin,
                                   size_type                       numElements,
                                   const VALUE_TYPE&               value,
                                   ALLOCATOR                      *allocator,
                                   bslmf::MetaInt<NON_NIL_TRAITS> *);
};

                    // =======================================
                    // class DequePrimitives_DequeElementGuard
                    // =======================================

template <class VALUE_TYPE, int BLOCK_LENGTH>
class DequePrimitives_DequeElementGuard {
    // This 'class' provides a specialized proctor object that, upon
    // destruction and unless the 'release' method has been called, destroys
    // the elements in a segment of a deque of parameterized 'VALUE_TYPE'.  The
    // elements destroyed are delimited by the "guarded" range
    // '[d_begin, d_end )'.

  public:
    // PUBLIC TYPES
    typedef DequeIterator<VALUE_TYPE, BLOCK_LENGTH>  Iterator;

  private:
    // DATA
    Iterator d_begin;  // iterator of first element in guarded range
    Iterator d_end;    // iterator beyond last element in guarded range

  private:
    // NOT IMPLEMENTED
    DequePrimitives_DequeElementGuard(
                                     const DequePrimitives_DequeElementGuard&);
    DequePrimitives_DequeElementGuard& operator=(
                                     const DequePrimitives_DequeElementGuard&);

  public:
    // CREATORS
    DequePrimitives_DequeElementGuard(const Iterator& begin,
                                      const Iterator& end);
        // Create a deque exception guard object for the sequence of elements
        // of the parameterized 'VALUE_TYPE' delimited by the specified range
        // '[ begin, end )'.  The behavior is undefined unless 'begin' <= 'end'
        // and unless each element in the range '[ begin, end )' has been
        // initialized.

    ~DequePrimitives_DequeElementGuard();
        // Call the destructor on each of the elements of the parameterized
        // 'VALUE_TYPE' delimited by the range '[ begin, end )' and destroy
        // this array exception guard.

    // MANIPULATORS
    Iterator& moveBegin(std::ptrdiff_t offset = -1);
        // Move the begin iterator by the specified 'offset', and return the
        // new begin iterator.

    Iterator& moveEnd(std::ptrdiff_t offset = 1);
        // Move the end pointer by the specified 'offset', and return the new
        // end pointer.

    void release();
        // Set the range of elements guarded by this object to be empty.  Note
        // that 'd_begin == d_end' following this operation, but the specific
        // value is unspecified.
};

                // ===============================================
                // class DequePrimitives_ExternalDequeElementGuard
                // ===============================================

template <class VALUE_TYPE, int BLOCK_LENGTH>
class DequePrimitives_ExternalDequeElementGuard {
    // This 'class' provides a specialized proctor object that, upon
    // destruction and unless the 'release' method has been called, destroys
    // the elements in a segment of a 'bslstl_deque' of parameterized type
    // 'VALUE_TYPE'.  The elements destroyed are delimited by the "guarded"
    // range '[ *d_begin, *d_end )'.  Note that the range guarded by this
    // 'class' is dynamic and can be changed outside of this 'class'.

  public:
    // PUBLIC TYPES
    typedef DequeIterator<VALUE_TYPE, BLOCK_LENGTH>  Iterator;

  private:
    // DATA
    Iterator *d_begin_p;  // pointer to iterator of first element in guarded
                          // range

    Iterator *d_end_p;    // pointer to iterator beyond last element in guarded
                          // range

  private:
    // NOT IMPLEMENTED
    DequePrimitives_ExternalDequeElementGuard(
                             const DequePrimitives_ExternalDequeElementGuard&);
    DequePrimitives_ExternalDequeElementGuard&
            operator=(const DequePrimitives_ExternalDequeElementGuard&);

  public:
    // CREATORS
    DequePrimitives_ExternalDequeElementGuard(Iterator *begin,
                                              Iterator *end);
        // Create a deque exception guard object for the sequence of elements
        // of the parameterized 'VALUE_TYPE' delimited by the specified range
        // '[ *begin, *end )'.  The behavior is undefined unless '*begin' <=
        // '*end' and unless each element in the range '[ *begin, *end )' has
        // been initialized.

    ~DequePrimitives_ExternalDequeElementGuard();
        // Call the destructor on each of the elements of the parameterized
        // 'VALUE_TYPE' delimited by the range '[ *d_begin_p, *d_end_p )' and
        // destroy this array exception guard.

    // MANIPULATORS
    void release();
        // Set the range of elements guarded by this object to be empty.  Note
        // that 'd_begin_p = d_end_p = 0' following this operation.
};

                    // ====================================
                    // class DequePrimitives_DequeMoveGuard
                    // ====================================

template <class VALUE_TYPE, int BLOCK_LENGTH>
class DequePrimitives_DequeMoveGuard {
    // This 'class' provides a guard object that, upon destruction and unless
    // the 'release' method has been called, uses 'moveBack' or 'moveFront' to
    // move the "guarded" range '[ d_source_p .. d_source_p + d_size - 1 ]'
    // back to '[ d_destination_p .. d_destination_p + d_size -1 ]'.

  public:
    // PUBLIC TYPES
    typedef DequeIterator<VALUE_TYPE,
                                 BLOCK_LENGTH>         Iterator;

    typedef bslalg::DequePrimitives<VALUE_TYPE,
                                   BLOCK_LENGTH>       DequePrimitives;

  private:
    // DATA
    Iterator         d_destination_p;  // destination of the move
    Iterator         d_source_p;       // source of the move
    std::size_t      d_size;           // size of the range being guarded
    bool             d_front;          // whether to use 'moveFront'

  public:
    // CREATORS
    DequePrimitives_DequeMoveGuard(Iterator    dest,
                                   Iterator    src,
                                   std::size_t size,
                                   bool        isFront);
        // Create a guard object that will call 'moveBack' or 'moveFront',
        // depending on the specified 'isFront', on the specified 'size'
        // elements from 'src' to 'dest' upon destruction unless 'release' has
        // been called.

    ~DequePrimitives_DequeMoveGuard();
        // Call either 'moveBack' or 'moveFront' depending on 'd_front' upon
        // destruction unless 'release' has been called before this.

    // MANIPULATORS
    void release();
        // Set the size of the range guarded by this object to be zero.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------
                        // class DequePrimitives
                        // ---------------------

// CLASS METHODS
template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
void DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
                                       ::destruct(Iterator begin, Iterator end)
{
    enum {
        IS_BITWISECOPYABLE  = bsl::is_trivially_copyable<VALUE_TYPE>::value,

        VALUE = IS_BITWISECOPYABLE
              ? BITWISE_COPYABLE_TRAITS
              : NIL_TRAITS
    };

    return destruct(begin, end, (bslmf::MetaInt<VALUE>*) 0);
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
void DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
                      ::destruct(Iterator                                 ,
                                 Iterator                                 ,
                                 bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *)
{
    // No-op.
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
void DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
                                   ::destruct(
                                             Iterator                    begin,
                                             Iterator                    end,
                                             bslmf::MetaInt<NIL_TRAITS> *)
{
    for (; !(begin == end); ++begin) {
        ScalarDestructionPrimitives::destroy(begin.valuePtr());
    }
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
inline
typename
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::Iterator
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
                                            ::erase(Iterator  *toBegin,
                                                    Iterator  *toEnd,
                                                    Iterator   fromBegin,
                                                    Iterator   first,
                                                    Iterator   last,
                                                    Iterator   fromEnd,
                                                    ALLOCATOR *allocator)
{
    enum {
        IS_BITWISECOPYABLE  = bsl::is_trivially_copyable<VALUE_TYPE>::value,

        VALUE = IS_BITWISECOPYABLE
              ? BITWISE_COPYABLE_TRAITS
              : NIL_TRAITS
    };

    return erase(toBegin, toEnd, fromBegin, first, last, fromEnd, allocator,
                 (bslmf::MetaInt<VALUE>*) 0);
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
typename
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::Iterator
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
                                  ::erase(
                                         Iterator                   *toBegin,
                                         Iterator                   *toEnd,
                                         Iterator                    fromBegin,
                                         Iterator                    first,
                                         Iterator                    last,
                                         Iterator                    fromEnd,
                                         ALLOCATOR                  *,
                                         bslmf::MetaInt<NIL_TRAITS> *)
{
    size_type frontSize = first - fromBegin;
    size_type backSize  = fromEnd - last;
    Iterator  ret;

    if (frontSize < backSize) {
        ret = last;
        for (; 0 < frontSize; --frontSize) {
            --last;
            --first;
            *last = *first;
        }
        *toBegin = last;
        *toEnd   = fromEnd;
    }
    else {
        ret = first;
        for (; 0 < backSize; --backSize, ++first, ++last) {
            *first = *last;
        }
        *toBegin = fromBegin;
        *toEnd   = first;
    }
    destruct(first, last);
    return ret;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
typename
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::Iterator
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
                 ::erase(Iterator                                *toBegin,
                         Iterator                                *toEnd,
                         Iterator                                 fromBegin,
                         Iterator                                 first,
                         Iterator                                 last,
                         Iterator                                 fromEnd,
                         ALLOCATOR                               *,
                         bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *)
{
    size_type frontSize = first - fromBegin;
    size_type backSize  = fromEnd - last;
    Iterator  ret;

    if (frontSize < backSize) {
        ret = last;
        moveBack(&last, &first, frontSize);
        *toBegin = last;
        *toEnd   = fromEnd;
    }
    else {
        ret = first;
        moveFront(&first, &last, backSize);
        *toBegin = fromBegin;
        *toEnd   = first;
    }
    // 'destruct' is no-op for types with 'BITWISE_COPYABLE_TRAITS'.
    return ret;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
inline
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
                         ::insertAndMoveToBack(Iterator          *toEnd,
                                               Iterator           fromEnd,
                                               Iterator           position,
                                               size_type          numElements,
                                               const VALUE_TYPE&  value,
                                               ALLOCATOR         *allocator)
{
    enum {
        IS_BITWISECOPYABLE  = bsl::is_trivially_copyable<VALUE_TYPE>::value,
        IS_BITWISEMOVEABLE  = bslmf::IsBitwiseMoveable<VALUE_TYPE>::value,

        VALUE = IS_BITWISECOPYABLE
              ? BITWISE_COPYABLE_TRAITS : IS_BITWISEMOVEABLE
              ? BITWISE_MOVEABLE_TRAITS : NIL_TRAITS
    };

    insertAndMoveToBack(toEnd, fromEnd, position, numElements, value,
                        allocator, (bslmf::MetaInt<VALUE>*)0);
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
   ::insertAndMoveToBack(Iterator                                *toEnd,
                         Iterator                                 fromEnd,
                         Iterator                                 position,
                         size_type                                numElements,
                         const VALUE_TYPE&                        value,
                         ALLOCATOR                               *allocator,
                         bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *)
{
    size_type backSize = fromEnd - position;
    Iterator  end      = fromEnd;
    Iterator  dest     = end + numElements;

    ConstructorProxy<VALUE_TYPE>
                                 tempValue(value, bslma::Default::allocator());

    // No guard needed since all the operations won't throw due to
    // bitwisecopyable trait
    moveBack(&dest, &end, backSize);
    uninitializedFillNFront(&dest, dest, numElements, tempValue.object(),
                            allocator);

    *toEnd = fromEnd + numElements;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
   ::insertAndMoveToBack(Iterator                                *toEnd,
                         Iterator                                 fromEnd,
                         Iterator                                 position,
                         size_type                                numElements,
                         const VALUE_TYPE&                        value,
                         ALLOCATOR                               *allocator,
                         bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *)
{
    size_type backSize = fromEnd - position;
    Iterator  end      = fromEnd;
    Iterator  dest     = end + numElements;

    // In case of aliasing, make a copy of the value.
    ConstructorProxy<VALUE_TYPE>
                                 tempValue(value, bslma::Default::allocator());

    // Setup a reverse guard that will reverse the moveBack operation in case
    // of an exception.
    MGuard guard(end, dest, backSize, false);

    moveBack(&dest, &end, backSize);

    // Create a guard for 'uninitializedFillNBack' because it can throw under
    // 'bitwisemoveable' trait.  Need to use this special guard because
    // uninitializedFillNFront is not exception safe.
    Iterator dest2(dest);
    EGuard eguard(&dest, &dest2);

    uninitializedFillNFront(&dest, dest, numElements, tempValue.object(),
                            allocator);
    eguard.release();
    guard.release();
    *toEnd = fromEnd + numElements;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
                  ::insertAndMoveToBack(
                                       Iterator                   *toEnd,
                                       Iterator                    fromEnd,
                                       Iterator                    position,
                                       size_type                   numElements,
                                       const VALUE_TYPE&           value,
                                       ALLOCATOR                  *allocator,
                                       bslmf::MetaInt<NIL_TRAITS> *)
{
    size_type backSize = fromEnd - position;
    Iterator  end      = fromEnd;
    Iterator  dest     = end + numElements;
    size_type numDest;

    // In case of aliasing, make a copy of the value.
    ConstructorProxy<VALUE_TYPE> tempValue(value, bslma::Default::allocator());

    Guard guard(dest, dest);
    if (backSize >= numElements) {
        for (numDest = numElements; 0 < numDest; --numDest) {
            --dest;
            --end;
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            *end,
                                            allocator);
            guard.moveBegin(-1);
        }
        for (backSize -= numElements; 0 < backSize; --backSize) {
            --dest;
            --end;
            *dest = *end;
        }
        for (numDest = numElements; 0 < numDest; --numDest, ++position) {
            *position = tempValue.object();
        }
    } else {
        for (numDest = backSize; 0 < numDest; --numDest) {
            --dest;
            --end;
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            *end,
                                            allocator);
            guard.moveBegin(-1);
        }
        for (numDest = numElements; backSize < numDest; --numDest) {
            --dest;
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            tempValue.object(),
                                            allocator);
            guard.moveBegin(-1);
        }
        for (; 0 < numDest; --numDest, ++position) {
            *position = tempValue.object();
        }
    }
    guard.release();

    *toEnd = fromEnd + numElements;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class FWD_ITER, class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
                                  ::insertAndMoveToBack(Iterator  *toEnd,
                                                        Iterator   fromEnd,
                                                        Iterator   position,
                                                        FWD_ITER   first,
                                                        FWD_ITER   /*last*/,
                                                        size_type  numElements,
                                                        ALLOCATOR *allocator)
{
    size_type backSize = fromEnd - position;
    Iterator  end      = fromEnd;
    Iterator  dest     = end + numElements;
    size_type numDest;

    Guard guard(dest, dest);
    if (backSize >= numElements) {
        for (numDest = numElements; 0 < numDest; --numDest) {
            --dest;
            --end;
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            *end,
                                            allocator);
            guard.moveBegin(-1);
        }
        for (numDest = backSize; numElements < numDest; --numDest) {
            --dest;
            --end;
            *dest = *end;
        }
        for (; 0 < numDest; ++first, ++position, --numDest) {
            *position = *first;
        }
    } else {
        for (numDest = backSize; 0 < numDest; --numDest) {
            --dest;
            --end;
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            *end,
                                            allocator);
            guard.moveBegin(-1);
        }
        for (numDest = backSize; 0 < numDest; --numDest, ++position, ++first) {
            *position = *first;
        }
        // Second guard needed because we're guarding from a different range.
        Guard guard2(position, position);
        for (numDest = numElements; backSize < numDest; ++first, ++position,
                                                                   --numDest) {
            ScalarPrimitives::copyConstruct(position.valuePtr(),
                                            *first,
                                            allocator);
            guard2.moveEnd(1);
        }
        guard2.release();
    }
    guard.release();
    *toEnd = fromEnd + numElements;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
inline
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
             ::insertAndMoveToFront(Iterator          *toBegin,
                                    Iterator           fromBegin,
                                    Iterator           position,
                                    size_type          numElements,
                                    const VALUE_TYPE&  value,
                                    ALLOCATOR         *allocator)
{
    enum {
        IS_BITWISECOPYABLE  = bsl::is_trivially_copyable<VALUE_TYPE>::value,
        IS_BITWISEMOVEABLE  = bslmf::IsBitwiseMoveable<VALUE_TYPE>::value,

        VALUE = IS_BITWISECOPYABLE
              ? BITWISE_COPYABLE_TRAITS : IS_BITWISEMOVEABLE
              ? BITWISE_MOVEABLE_TRAITS : NIL_TRAITS
    };

    insertAndMoveToFront(toBegin, fromBegin, position, numElements, value,
                         allocator, (bslmf::MetaInt<VALUE>*)0);
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
  ::insertAndMoveToFront(Iterator                                *toBegin,
                         Iterator                                 fromBegin,
                         Iterator                                 position,
                         size_type                                numElements,
                         const VALUE_TYPE&                        value,
                         ALLOCATOR                               *allocator,
                         bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *)
{
    size_type frontSize = position - fromBegin;
    Iterator  begin     = fromBegin;
    Iterator  dest      = begin - numElements;

    // In case of aliasing, make a copy of the value.
    ConstructorProxy<VALUE_TYPE>
                                 tempValue(value, bslma::Default::allocator());

    // No guard needed since all the operations won't throw due to
    // bitwisecopyable trait
    moveFront(&dest, &begin, frontSize);
    uninitializedFillNBack(&dest, dest, numElements, tempValue.object(),
                           allocator);

    *toBegin = fromBegin - numElements;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
    ::insertAndMoveToFront(
                          Iterator                                *toBegin,
                          Iterator                                 fromBegin,
                          Iterator                                 position,
                          size_type                                numElements,
                          const VALUE_TYPE&                        value,
                          ALLOCATOR                               *allocator,
                          bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *)
{
    size_type frontSize = position - fromBegin;
    Iterator  begin     = fromBegin;
    Iterator  dest      = begin - numElements;
    // In case of aliasing, make a copy of the value.
    ConstructorProxy<VALUE_TYPE>
                                 tempValue(value, bslma::Default::allocator());

    // Create a reverse guard that will reverse the moveFront operation in case
    // of an exception.
    MGuard guard(begin, dest, frontSize, true);

    moveFront(&dest, &begin, frontSize);

    // Create a guard for 'uninitializedFillNBack' because it can throw under
    // 'bitwisemoveable' trait.  Need to use this special guard because
    // uninitializedFillNBack is not exception safe.
    Iterator dest2(dest);
    EGuard eguard(&dest2, &dest);

    uninitializedFillNBack(&dest, dest, numElements, tempValue.object(),
                           allocator);

    eguard.release();
    guard.release();
    *toBegin = fromBegin - numElements;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
                 ::insertAndMoveToFront(
                                       Iterator                   *toBegin,
                                       Iterator                    fromBegin,
                                       Iterator                    position,
                                       size_type                   numElements,
                                       const VALUE_TYPE&           value,
                                       ALLOCATOR                  *allocator,
                                       bslmf::MetaInt<NIL_TRAITS> *)
{
    size_type frontSize = position - fromBegin;
    Iterator  begin     = fromBegin;
    Iterator  dest      = begin - numElements;
    size_type numDest;

    // In case of aliasing, make a copy of the value.
    ConstructorProxy<VALUE_TYPE>
                                 tempValue(value, bslma::Default::allocator());

    Guard guard(dest, dest);
    if (frontSize >= numElements) {
        for (numDest = numElements; 0 < numDest; --numDest, ++dest, ++begin) {
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            *begin,
                                            allocator);
            guard.moveEnd(1);
        }
        for (frontSize -= numElements; 0 < frontSize;
                                                --frontSize, ++dest, ++begin) {
            *dest = *begin;
        }
        for (numDest = numElements; 0 < numDest; --numDest, ++dest) {
            *dest = tempValue.object();
        }
    } else {
        for (numDest = frontSize; 0 < numDest; --numDest, ++dest, ++begin) {
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            *begin,
                                            allocator);
            guard.moveEnd(1);
        }
        for (numDest = numElements; frontSize < numDest; --numDest, ++dest) {
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            tempValue.object(),
                                            allocator);
            guard.moveEnd(1);
        }
        for (; 0 < numDest; --numDest, ++dest) {
            *dest = tempValue.object();
        }
    }
    guard.release();
    *toBegin = fromBegin - numElements;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class FWD_ITER, class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
                           ::insertAndMoveToFront(Iterator  *toBegin,
                                                  Iterator   fromBegin,
                                                  Iterator   position,
                                                  FWD_ITER   first,
                                                  FWD_ITER   /*last*/,
                                                  size_type  numElements,
                                                  ALLOCATOR *allocator)
{
    size_type frontSize = position - fromBegin;
    Iterator  begin     = fromBegin;
    Iterator  dest      = begin - numElements;
    size_type numDest;

    Guard guard(dest, dest);
    if (frontSize >= numElements) {
        for (numDest = numElements; 0 < numDest; --numDest, ++dest, ++begin) {
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            *begin,
                                            allocator);
            guard.moveEnd(1);
        }
        for (frontSize -= numElements; 0 < frontSize;
                                                --frontSize, ++dest, ++begin) {
            *dest = *begin;
        }
        for (numDest = numElements; 0 < numDest; --numDest, ++dest, ++first) {
            *dest = *first;
        }
    } else {
        for (numDest = frontSize; 0 < numDest; --numDest, ++dest, ++begin) {
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            *begin,
                                            allocator);
            guard.moveEnd(1);
        }
        for (numDest = numElements; frontSize < numDest;
                                                  --numDest, ++dest, ++first) {
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            *first,
                                            allocator);
            guard.moveEnd(1);
        }
        for (; 0 < numDest; --numDest, ++dest, ++first) {
            *dest = *first;
        }
    }
    guard.release();
    *toBegin = fromBegin - numElements;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
void DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
                                       ::moveBack(Iterator  *destination,
                                                  Iterator  *source,
                                                  size_type  numElements)
{
    if (destination->offsetInBlock() > numElements
        && source->offsetInBlock() > numElements) {
            // There is enough room to move everything at once.

        *destination -= numElements;
        *source      -= numElements;
        std::memmove(destination->valuePtr(),
                     source->valuePtr(),
                     numElements * sizeof(VALUE_TYPE));

        return;                                                       // RETURN
    }

    // Moving the blocks involving segments of 3 different lengths, as
    // illustrated below.  We need to distinguish whether destination or the
    // source have more space remaining, and adjust the algorithm accordingly.
    //..
    //  [--- n ---]         - segment 'n'
    //  [--- n'---]         - segment with equal length as 'n'.
    //  [--- n ---I-- m --] - two segments, with length 'n' and 'm'.
    //   - - - - - - - -
    //  | | | | | | | | |  - a block in the deque pointed by a 'blockPtr'
    //   - - - - - - - -
    //..
    //
    // Scenario 1: Source has less element than destination has space
    // ==============================================================
    // Under this scenario, we have to move all elements from the source block
    // to the destination block first (segment 1), then fill the destination
    // block with the remaining elements (segment 2).  After, we alternate
    // between segments 3 and 2.
    //..
    //  [--- 3 ---I- 2'-]   [- 1 -]             [- 2 -I- 1'-]
    //   - - - - - - - -     - - - - - - - -     - - - - - - - -
    //  |d|e|f|g|h|i|j|k|   |a|b|c|/| | | | |   | | | | | | | | |
    //   - - - - - - - -     - - - - - - - -     - - - - - - - -
    //                             ^                         ^
    //                  source ____|        destination _____|
    //..
    //
    // Scenario 2: Source has more elements than destination has space
    // ================================================================
    // Under this scenario, we can only move some elements (the number of
    // elements that can fit in the destination block) from the source block to
    // the destination block first (segment 1), then move the remaining
    // elements (segment 2) from the source block.  After, we alternate between
    // segments 3 and 2.
    //..
    //                      [---- 3 ----]
    //      [---- 3' ---]   [-2-I- 1 -] [2' ]   [- 1 -]
    //   - - - - - - - -     - - - - - - - -     - - - - - - - -
    //  |d|e|f|g|h|i|j|k|   |a|b|c|d|e|/| | |   | | | | | | | | |
    //   - - - - - - - -     - - - - - - - -     - - - - - - - -
    //                                 ^               ^
    //                     source _____|  destination _|
    //
    //  1) Copy segment 1
    //  2) Copy segment 2
    //  3) Copy segment 3
    //  4) Repeat 2, 3 until there's less than 1 'BLOCK_LENGTH' left
    //  5) Copy the remaining items over
    //..

    size_type firstSegment, secondSegment;
    if (source->offsetInBlock() > destination->offsetInBlock()) {
        firstSegment  = destination->offsetInBlock();
        secondSegment = source->offsetInBlock() - firstSegment;
    }
    else {
        firstSegment  = source->offsetInBlock();
        secondSegment = destination->offsetInBlock() - firstSegment;
    }

    size_type thirdSegment = BLOCK_LENGTH - secondSegment;

    *destination -= firstSegment;
    *source      -= firstSegment;
    numElements  -= firstSegment;

    std::memmove(destination->valuePtr(),
                 source->valuePtr(),
                 firstSegment * sizeof(VALUE_TYPE));

    for (; numElements >= BLOCK_LENGTH; numElements -= BLOCK_LENGTH) {

        *destination -= secondSegment;
        *source      -= secondSegment;

        std::memmove(destination->valuePtr(),
                     source->valuePtr(),
                     secondSegment * sizeof(VALUE_TYPE));

        *destination -= thirdSegment;
        *source      -= thirdSegment;

        std::memmove(destination->valuePtr(),
                     source->valuePtr(),
                     thirdSegment * sizeof(VALUE_TYPE));
    }

    size_type remaining = numElements > secondSegment
                        ? secondSegment
                        : numElements;

    *destination -= remaining;
    *source      -= remaining;
    numElements  -= remaining;

    std::memmove(destination->valuePtr(),
                 source->valuePtr(),
                 remaining * sizeof(VALUE_TYPE));

    *destination -= numElements;
    *source      -= numElements;

    std::memmove(destination->valuePtr(),
                 source->valuePtr(),
                 numElements * sizeof(VALUE_TYPE));
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
void DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
                                      ::moveFront(Iterator  *destination,
                                                  Iterator  *source,
                                                  size_type  numElements)
{
    if (destination->remainingInBlock() > numElements
        && source->remainingInBlock() > numElements) {
            // There is enough room to move everything at once

        std::memmove(destination->valuePtr(),
                     source->valuePtr(),
                     numElements * sizeof(VALUE_TYPE));
        *destination += numElements;
        *source      += numElements;

        return;                                                       // RETURN
    }

    // Moving the blocks involving segments of 3 different lengths, as
    // illustrated below.  We need to distinguish whether destination or the
    // source have more space remaining, and adjust the algorithm accordingly.
    //..
    //  [--- n ---]         - segment 'n'
    //  [--- n'---]         - segment with equal length as 'n'
    //  [--- n ---I-- m --] - two segments, with length 'n' and 'm'.
    //   - - - - - - - -
    //  | | | | | | | | |   - a block in the deque pointed by a 'blockPtr'
    //   - - - - - - - -
    //..
    //
    // Scenario 1: Source has more elements than destination has space
    // ===============================================================
    // Under this scenario, we can only move some elements (the number of
    // elements that can fit in the destination block) from the source block to
    // the destination block first (segment 1), then move the remaining
    // elements (segment 2) from the source block.  After, we alternate between
    // segments 3 and 2.
    //..
    //                          [---- 3 ----]
    //            [- 1 -]   [ 2'] [- 1'-I-2-]   [---- 3'----]
    //   - - - - - - - -     - - - - - - - -     - - - - - - - -
    //  | | | | | | | | |   | | | |a|a|b|c|d|   |e|f|g|h|i|j|k|l|
    //   - - - - - - - -     - - - - - - - -     - - - - - - - -
    //             ^               ^
    //             |_ destination  |____ source
    //..
    //
    // Scenario 2: Source has less elements than destination has space
    // ===============================================================
    // Under this scenario, we have to move all elements from the source block
    // to the destination block first (segment 1), then fill the destination
    // block with the remaining elements (segment 2).  After, we alternate
    // between segments 3 and 2.
    //..
    //      [- 1'-I- 2 -]   [--- 3'---I- 1 -]   [- 2'-I--- 3 ---]
    //   - - - - - - - -     - - - - - - - -     - - - - - - - -
    //  | | | | | | | | |   | | | | | |a|b|c|   |d|e|f|g|h|i|j|k|
    //   - - - - - - - -     - - - - - - - -     - - - - - - - -
    //       ^                         ^
    //       |_ destination            |____ source
    //
    //  1) Copy segment 1
    //  2) Copy segment 2
    //  3) Copy segment 3
    //  4) Repeat 2, 3 until there's less than 1 'BLOCK_LENGTH' left
    //  5) Copy the remaining items over
    //..

    size_type firstSegment, secondSegment;
    if (source->remainingInBlock() > destination->remainingInBlock()) {
        firstSegment  = destination->remainingInBlock();
        secondSegment = source->remainingInBlock() - firstSegment;
    }
    else {
        firstSegment  = source->remainingInBlock();
        secondSegment = destination->remainingInBlock() - firstSegment;
    }

    size_type thirdSegment = BLOCK_LENGTH - secondSegment;

    std::memmove(destination->valuePtr(),
                 source->valuePtr(),
                 firstSegment * sizeof(VALUE_TYPE));

    *destination += firstSegment;
    *source      += firstSegment;
    numElements  -= firstSegment;

    for (; numElements >= BLOCK_LENGTH; numElements -= BLOCK_LENGTH) {

        std::memmove(destination->valuePtr(),
                     source->valuePtr(),
                     secondSegment * sizeof(VALUE_TYPE));

        *destination += secondSegment;
        *source      += secondSegment;

        std::memmove(destination->valuePtr(),
                     source->valuePtr(),
                     thirdSegment * sizeof(VALUE_TYPE));

        *destination += thirdSegment;
        *source      += thirdSegment;
    }

    size_type remaining = numElements > secondSegment
                        ? secondSegment
                        : numElements;

    std::memmove(destination->valuePtr(),
                 source->valuePtr(),
                 remaining * sizeof(VALUE_TYPE));

    *destination += remaining;
    *source      += remaining;
    numElements  -= remaining;

    std::memmove(destination->valuePtr(),
                 source->valuePtr(),
                 numElements * sizeof(VALUE_TYPE));

    *destination += numElements;
    *source      += numElements;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
           ::uninitializedFillNBack(Iterator          *toEnd,
                                    Iterator           fromEnd,
                                    size_type          numElements,
                                    const VALUE_TYPE&  value,
                                    ALLOCATOR         *allocator)
{
    if (fromEnd.remainingInBlock() > numElements) {
        ArrayPrimitives::uninitializedFillN(fromEnd.valuePtr(),
                                            numElements,
                                            value,
                                            allocator);
        fromEnd += numElements;
        *toEnd   = fromEnd;
        return;                                                       // RETURN
    }

    size_type firstRemaining = fromEnd.remainingInBlock();

    ArrayPrimitives::uninitializedFillN(fromEnd.valuePtr(),
                                        firstRemaining,
                                        value,
                                        allocator);

    numElements -= firstRemaining;
    fromEnd     += firstRemaining;
    *toEnd       = fromEnd;

    for ( ; numElements >= BLOCK_LENGTH; numElements -= BLOCK_LENGTH) {
        ArrayPrimitives::uninitializedFillN(fromEnd.valuePtr(),
                                            BLOCK_LENGTH,
                                            value,
                                            allocator);
        fromEnd.nextBlock();
        toEnd->nextBlock();
    }

    ArrayPrimitives::uninitializedFillN(fromEnd.valuePtr(),
                                        numElements,
                                        value,
                                        allocator);

    fromEnd += numElements;
    *toEnd   = fromEnd;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
          ::uninitializedFillNFront(Iterator          *toBegin,
                                    Iterator           fromBegin,
                                    size_type          numElements,
                                    const VALUE_TYPE&  value,
                                    ALLOCATOR         *allocator)
{
    if (fromBegin.offsetInBlock() > numElements) {
        fromBegin -= numElements;

        ArrayPrimitives::uninitializedFillN(fromBegin.valuePtr(),
                                            numElements,
                                            value,
                                            allocator);
        *toBegin = fromBegin;
        return;                                                       // RETURN
    }

    size_type firstRemaining = fromBegin.offsetInBlock();

    fromBegin   -= firstRemaining;
    numElements -= firstRemaining;

    ArrayPrimitives::uninitializedFillN(fromBegin.valuePtr(),
                                        firstRemaining,
                                        value,
                                        allocator);

    *toBegin = fromBegin;  // in case of exception

    for ( ; numElements >= BLOCK_LENGTH; numElements -= BLOCK_LENGTH) {
        fromBegin.previousBlock();
        ArrayPrimitives::uninitializedFillN(fromBegin.valuePtr(),
                                            BLOCK_LENGTH,
                                            value,
                                            allocator);
        *toBegin = fromBegin;
    }

    fromBegin -= numElements;
    ArrayPrimitives::uninitializedFillN(fromBegin.valuePtr(),
                                        numElements,
                                        value,
                                        allocator);

    *toBegin = fromBegin;
}

                 // ------------------------------------
                 // class DequePrimitives<VALUE_TYPE, 1>
                 // ------------------------------------

// CLASS METHODS
template <class VALUE_TYPE>
template <class ALLOCATOR>
typename DequePrimitives<VALUE_TYPE, 1>::Iterator
DequePrimitives<VALUE_TYPE, 1>::erase(Iterator  *toBegin,
                                      Iterator  *toEnd,
                                      Iterator   fromBegin,
                                      Iterator   first,
                                      Iterator   last,
                                      Iterator   fromEnd,
                                      ALLOCATOR * /*allocator*/)
{
    size_type frontSize = first - fromBegin;
    size_type backSize  = fromEnd - last;
    Iterator  ret;

    if (frontSize < backSize) {
        ret = last;
        for (; 0 < frontSize; --frontSize) {
            --last;
            --first;
            *last = *first;
        }
        *toBegin = last;
        *toEnd   = fromEnd;
    }
    else {
        ret = first;
        for (; 0 < backSize; --backSize, ++first, ++last) {
            *first = *last;
        }
        *toBegin = fromBegin;
        *toEnd   = first;
    }
    destruct(first, last);
    return ret;
}

template <class VALUE_TYPE>
void DequePrimitives<VALUE_TYPE, 1>::destruct(Iterator begin,
                                              Iterator end)
{
    for (; !(begin == end); ++begin) {
        ScalarDestructionPrimitives::destroy(begin.valuePtr());
    }
}

template <class VALUE_TYPE>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, 1>
                        ::insertAndMoveToFront(Iterator          *toBegin,
                                               Iterator           fromBegin,
                                               Iterator           position,
                                               size_type          numElements,
                                               const VALUE_TYPE&  value,
                                               ALLOCATOR         *allocator)
{
    size_type frontSize = position - fromBegin;
    Iterator  begin     = fromBegin;
    Iterator  dest      = begin - numElements;
    size_type numDest;

    ConstructorProxy<VALUE_TYPE>
                                 tempValue(value, bslma::Default::allocator());

    Guard guard(dest, dest);
    if (frontSize >= numElements) {
        for (numDest = numElements; 0 < numDest; --numDest, ++dest, ++begin) {
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            *begin,
                                            allocator);
            guard.moveEnd(1);
        }
        for (frontSize -= numElements; 0 < frontSize;
                                                --frontSize, ++dest, ++begin) {
            *dest = *begin;
        }
        for (numDest = numElements; 0 < numDest; --numDest, ++dest) {
            *dest = tempValue.object();
        }
    } else {
        for (numDest = frontSize; 0 < numDest; --numDest, ++dest, ++begin) {
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            *begin,
                                            allocator);
            guard.moveEnd(1);
        }
        for (numDest = numElements; frontSize < numDest; --numDest, ++dest) {
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            tempValue.object(),
                                            allocator);
            guard.moveEnd(1);
        }
        for (; 0 < numDest; --numDest, ++dest) {
            *dest = tempValue.object();
        }
    }
    guard.release();
    *toBegin = fromBegin - numElements;
}

template <class VALUE_TYPE>
template <class FWD_ITER, class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, 1>
                           ::insertAndMoveToFront(Iterator  *toBegin,
                                                  Iterator   fromBegin,
                                                  Iterator   position,
                                                  FWD_ITER   first,
                                                  FWD_ITER   /*last*/,
                                                  size_type  numElements,
                                                  ALLOCATOR *allocator)
{
    size_type frontSize = position - fromBegin;
    Iterator  begin     = fromBegin;
    Iterator  dest      = begin - numElements;
    size_type numDest;

    Guard guard(dest, dest);
    if (frontSize >= numElements) {
        for (numDest = numElements; 0 < numDest; --numDest, ++dest, ++begin) {
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            *begin,
                                            allocator);
            guard.moveEnd(1);
        }
        for (frontSize -= numElements; 0 < frontSize;
                                                --frontSize, ++dest, ++begin) {
            *dest = *begin;
        }
        for (numDest = numElements; 0 < numDest; --numDest, ++dest, ++first) {
            *dest = *first;
        }
    } else {
        for (numDest = frontSize; 0 < numDest; --numDest, ++dest, ++begin) {
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            *begin,
                                            allocator);
            guard.moveEnd(1);
        }
        for (numDest = numElements; frontSize < numDest;
                                                  --numDest, ++dest, ++first) {
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            *first,
                                            allocator);
            guard.moveEnd(1);
        }
        for (; 0 < numDest; --numDest, ++dest, ++first) {
            *dest = *first;
        }
    }
    guard.release();
    *toBegin = fromBegin - numElements;
}

template <class VALUE_TYPE>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, 1>::insertAndMoveToBack(
                                                Iterator          *toEnd,
                                                Iterator           fromEnd,
                                                Iterator           position,
                                                size_type          numElements,
                                                const VALUE_TYPE&  value,
                                                ALLOCATOR         *allocator)
{
    size_type backSize = fromEnd - position;
    Iterator  end      = fromEnd;
    Iterator  dest     = end + numElements;
    size_type numDest;

    ConstructorProxy<VALUE_TYPE>
                                 tempValue(value, bslma::Default::allocator());

    Guard guard(dest, dest);
    if (backSize >= numElements) {
        for (numDest = numElements; 0 < numDest; --numDest) {
            --dest;
            --end;
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            *end,
                                            allocator);
            guard.moveBegin(-1);
        }
        for (backSize -= numElements; 0 < backSize; --backSize) {
            --dest;
            --end;
            *dest = *end;
        }
        for (numDest = numElements; 0 < numDest; --numDest, ++position) {
            *position = tempValue.object();
        }
    } else {
        for (numDest = backSize; 0 < numDest; --numDest) {
            --dest;
            --end;
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            *end,
                                            allocator);
            guard.moveBegin(-1);
        }
        for (numDest = numElements; backSize < numDest; --numDest) {
            --dest;
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            tempValue.object(),
                                            allocator);
            guard.moveBegin(-1);
        }
        for (; 0 < numDest; --numDest, ++position) {
            *position = tempValue.object();
        }
    }
    guard.release();
    *toEnd = fromEnd + numElements;
}

template <class VALUE_TYPE>
template <class FWD_ITER, class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, 1>::insertAndMoveToBack(Iterator  *toEnd,
                                                    Iterator   fromEnd,
                                                    Iterator   position,
                                                    FWD_ITER   first,
                                                    FWD_ITER   /*last*/,
                                                    size_type  numElements,
                                                    ALLOCATOR *allocator)
{
    size_type backSize = fromEnd - position;
    Iterator  end      = fromEnd;
    Iterator  dest     = end + numElements;
    size_type numDest;

    Guard guard(dest, dest);
    if (backSize >= numElements) {
        for (numDest = numElements; 0 < numDest; --numDest) {
            --dest;
            --end;
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            *end,
                                            allocator);
            guard.moveBegin(-1);
        }
        for (numDest = backSize; numElements < numDest; --numDest) {
            --dest;
            --end;
            *dest = *end;
        }
        for (; 0 < numDest; ++first, ++position, --numDest) {
            *position = *first;
        }
    } else {
        for (numDest = backSize; 0 < numDest; --numDest) {
            --dest;
            --end;
            ScalarPrimitives::copyConstruct(dest.valuePtr(),
                                            *end,
                                            allocator);
            guard.moveBegin(-1);
        }
        for (numDest = backSize; 0 < numDest; --numDest, ++position, ++first) {
            *position = *first;
        }
        Guard guard2(position, position);
        for (numDest = numElements; backSize < numDest; ++first, ++position,
                                                                   --numDest) {
            ScalarPrimitives::copyConstruct(position.valuePtr(),
                                            *first,
                                            allocator);
            guard2.moveEnd(1);
        }
        guard2.release();
    }
    guard.release();
    *toEnd = fromEnd + numElements;
}

template <class VALUE_TYPE>
template <class ALLOCATOR>
inline
void
DequePrimitives<VALUE_TYPE, 1>::uninitializedFillNFront(
                                                Iterator          *toBegin,
                                                Iterator           fromBegin,
                                                size_type          numElements,
                                                const VALUE_TYPE&  value,
                                                ALLOCATOR         *allocator)
{
    enum {
        IS_FUNCTION_POINTER = bslmf::IsFunctionPointer<VALUE_TYPE>::value,
        IS_FUNDAMENTAL      = bslmf::IsFundamental<VALUE_TYPE>::value,
        IS_POINTER          = bslmf::IsPointer<VALUE_TYPE>::value,

        IS_FUNDAMENTAL_OR_POINTER = IS_FUNDAMENTAL ||
                                    (IS_POINTER && !IS_FUNCTION_POINTER),

        IS_BITWISECOPYABLE  = bsl::is_trivially_copyable<VALUE_TYPE>::value,

        VALUE = IS_FUNDAMENTAL_OR_POINTER || IS_BITWISECOPYABLE ?
                NON_NIL_TRAITS
              : NIL_TRAITS
    };

    uninitializedFillNFront(toBegin, fromBegin, numElements, value, allocator,
                            (bslmf::MetaInt<VALUE>*)0);
}

template <class VALUE_TYPE>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, 1>::uninitializedFillNFront(
                                       Iterator                   *toBegin,
                                       Iterator                    fromBegin,
                                       size_type                   numElements,
                                       const VALUE_TYPE&           value,
                                       ALLOCATOR                  *allocator,
                                       bslmf::MetaInt<NIL_TRAITS> *)
{
    Guard guard(fromBegin, fromBegin);
    for (; 0 < numElements; --numElements) {
        --fromBegin;
        ScalarPrimitives::copyConstruct(fromBegin.valuePtr(),
                                        value,
                                        allocator);
        guard.moveBegin(-1);
    }
    guard.release();
    *toBegin = fromBegin;
}

template <class VALUE_TYPE>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, 1>::uninitializedFillNFront(
                                   Iterator                       *toBegin,
                                   Iterator                        fromBegin,
                                   size_type                       numElements,
                                   const VALUE_TYPE&               value,
                                   ALLOCATOR                      *allocator,
                                   bslmf::MetaInt<NON_NIL_TRAITS> *)
{
    *toBegin = fromBegin;  // necessary in case 'numElements = 0'
    for ( ; 0 < numElements; --numElements) {
        --fromBegin;
        ArrayPrimitives::uninitializedFillN(fromBegin.valuePtr(),
                                            1,
                                            value,
                                            allocator);
        *toBegin = fromBegin;  // in case of exception
    }
}

template <class VALUE_TYPE>
template <class ALLOCATOR>
inline
void
DequePrimitives<VALUE_TYPE, 1>::uninitializedFillNBack(
                                                Iterator          *toEnd,
                                                Iterator           fromEnd,
                                                size_type          numElements,
                                                const VALUE_TYPE&  value,
                                                ALLOCATOR         *allocator)
{
    enum {
        IS_FUNCTION_POINTER = bslmf::IsFunctionPointer<VALUE_TYPE>::value,
        IS_FUNDAMENTAL      = bslmf::IsFundamental<VALUE_TYPE>::value,
        IS_POINTER          = bslmf::IsPointer<VALUE_TYPE>::value,

        IS_FUNDAMENTAL_OR_POINTER = IS_FUNDAMENTAL ||
                                    (IS_POINTER && !IS_FUNCTION_POINTER),

        IS_BITWISECOPYABLE  = bsl::is_trivially_copyable<VALUE_TYPE>::value,

        VALUE = IS_FUNDAMENTAL_OR_POINTER || IS_BITWISECOPYABLE ?
                NON_NIL_TRAITS
              : NIL_TRAITS
    };

    uninitializedFillNBack(toEnd, fromEnd, numElements, value, allocator,
                           (bslmf::MetaInt<VALUE>*)0);
}

template <class VALUE_TYPE>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, 1>::uninitializedFillNBack(
                                       Iterator                   *toEnd,
                                       Iterator                    fromEnd,
                                       size_type                   numElements,
                                       const VALUE_TYPE&           value,
                                       ALLOCATOR                  *allocator,
                                       bslmf::MetaInt<NIL_TRAITS> *)
{
    Guard guard(fromEnd, fromEnd);
    for (; 0 < numElements; --numElements) {
        ScalarPrimitives::copyConstruct(fromEnd.valuePtr(),
                                        value,
                                        allocator);
        ++fromEnd;
        guard.moveEnd(1);
    }
    guard.release();
    *toEnd = fromEnd;
}

template <class VALUE_TYPE>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, 1>::uninitializedFillNBack(
                                   Iterator                       *toEnd,
                                   Iterator                        fromEnd,
                                   size_type                       numElements,
                                   const VALUE_TYPE&               value,
                                   ALLOCATOR                      *allocator,
                                   bslmf::MetaInt<NON_NIL_TRAITS> *)
{
    *toEnd = fromEnd;  // necessary in case 'numElements = 0'
    for ( ; 0 < numElements; --numElements) {
        ArrayPrimitives::uninitializedFillN(fromEnd.valuePtr(),
                                            1,
                                            value,
                                            allocator);
        ++fromEnd;
        *toEnd = fromEnd;
    }
}

               // ---------------------------------------
               // class DequePrimitives_DequeElementGuard
               // ---------------------------------------

// CREATORS
template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
DequePrimitives_DequeElementGuard<VALUE_TYPE, BLOCK_LENGTH>
              ::DequePrimitives_DequeElementGuard(const Iterator& begin,
                                                  const Iterator& end)
: d_begin(begin),
  d_end(end)
{
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
DequePrimitives_DequeElementGuard<VALUE_TYPE, BLOCK_LENGTH>
                                  ::~DequePrimitives_DequeElementGuard()
{
    DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::
                                                      destruct(d_begin, d_end);
}

// MANIPULATORS
template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
typename DequePrimitives_DequeElementGuard
<VALUE_TYPE, BLOCK_LENGTH>::Iterator&
DequePrimitives_DequeElementGuard<VALUE_TYPE, BLOCK_LENGTH>
                                             ::moveBegin(std::ptrdiff_t offset)
{
    d_begin += offset;
    return d_begin;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
typename DequePrimitives_DequeElementGuard
<VALUE_TYPE, BLOCK_LENGTH>::Iterator&
DequePrimitives_DequeElementGuard<VALUE_TYPE, BLOCK_LENGTH>
                                               ::moveEnd(std::ptrdiff_t offset)
{
    d_end += offset;
    return d_end;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
void DequePrimitives_DequeElementGuard<VALUE_TYPE, BLOCK_LENGTH>::release()
{
    d_begin = d_end;
}

            // -----------------------------------------------
            // class DequePrimitives_ExternalDequeElementGuard
            // -----------------------------------------------

// CREATORS
template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
DequePrimitives_ExternalDequeElementGuard<VALUE_TYPE, BLOCK_LENGTH>
            ::DequePrimitives_ExternalDequeElementGuard(Iterator *begin,
                                                        Iterator *end)
: d_begin_p(begin),
  d_end_p(end)
{
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
DequePrimitives_ExternalDequeElementGuard<VALUE_TYPE, BLOCK_LENGTH>
                          ::~DequePrimitives_ExternalDequeElementGuard()
{
    if (d_begin_p != d_end_p) {
        DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::
                                                destruct(*d_begin_p, *d_end_p);
    }
}

// MANIPULATORS
template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
void
DequePrimitives_ExternalDequeElementGuard<VALUE_TYPE, BLOCK_LENGTH>::release()
{
    d_begin_p = d_end_p = 0;
}

                // ------------------------------------
                // class DequePrimitives_DequeMoveGuard
                // ------------------------------------

// CREATORS
template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
DequePrimitives_DequeMoveGuard<VALUE_TYPE, BLOCK_LENGTH>
            ::DequePrimitives_DequeMoveGuard(Iterator    dest,
                                             Iterator    src,
                                             std::size_t size,
                                             bool        isFront)
: d_destination_p(dest)
, d_source_p(src)
, d_size(size)
, d_front(isFront)
{
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
DequePrimitives_DequeMoveGuard<VALUE_TYPE, BLOCK_LENGTH>
                                     ::~DequePrimitives_DequeMoveGuard()
{
    if (d_size != 0) {
        // We need to reverse 'moveFront'.
        if (d_front) {
            d_destination_p += d_size;
            d_source_p      += d_size;
            DequePrimitives::moveBack(&d_destination_p,
                                      &d_source_p,
                                      d_size);
        }
        // We need to reverse 'moveBack'.
        else {
            d_destination_p -= d_size;
            d_source_p      -= d_size;
            DequePrimitives::moveFront(&d_destination_p,
                                       &d_source_p,
                                       d_size);
        }
    }
}

// MANIPULATORS
template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
void DequePrimitives_DequeMoveGuard<VALUE_TYPE, BLOCK_LENGTH>::release()
{
    d_size = 0;
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
