// bslalg_dequeprimitives.h                                           -*-C++-*-
#ifndef INCLUDED_BSLALG_DEQUEPRIMITIVES
#define INCLUDED_BSLALG_DEQUEPRIMITIVES

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide primitive algorithms that operate on deques.
//
//@CLASSES:
//  bslalg::DequePrimitives: namespace for deque algorithms
//
//@SEE_ALSO: bslma_constructionutil, bslalg_arrayprimitives
//
//@DESCRIPTION: This component provides utilities to initialize, destroy, move,
// and otherwise perform various primitive manipulations on deques with a
// uniform interface, but selecting a different implementation according to the
// various traits possessed by the underlying type, and selecting a simpler
// implementation when there is only one element per deque block.  See the
// 'bslalg_dequeimputil' and 'bslalg_dequeiterator' components for a definition
// and visual depiction of the internal organization of a deque, and of the
// iterator type used to refer to elements in the deque.
//
// The primitives provided by this component are exceptionally useful for
// implementing generic block-based components such as deques.  A short
// synopsis is provided below describing the observable behavior and mentioning
// the relevant traits.  See the full function-level contract for detailed
// description, including exception-safety guarantees.  In the description
// below, 'Ar' stands for 'bslalg::ArrayPrimitives'.  Note that some algorithms
// are explained in terms of previous algorithms.
//..
//  Algorithm                     Short description of observable behavior
//  ----------------------------  ---------------------------------------------
//  destruct                      Destroy each element in the target range.
//
//  erase                         'destruct' for each element in the target
//                                range, or 'no-op' if bitwise copyable, then
//                                shift the remaining elements from either the
//                                front or the back to fill hole.
//
//  uninitializedFillNBack        'Ar::uninitializedFillN' for each block at
//                                the end of the deque
//
//  uninitializedFillNFront       'Ar::uninitializedFillN' for each block at
//                                the front of the deque.
//
//  valueInititalizeN             'Ar::defaultConstruct' for each block at the
//                                end of the deque.
//
//  insertAndMoveToBack           Copy construct each element in the target
//                                range, or 'std::memmove' if type is bitwise
//                                moveable, to the back of the deque to create
//                                a hole, followed by copy construct of target
//                                value or range to fill the hole.
//
//  insertAndMoveToFront          Copy construct each element in the target
//                                range, or 'std::memmove' if type is bitwise
//                                moveable, to the front of the deque to create
//                                a hole, followed by copy construct or
//                                'std::memmove' of target value or range to
//                                fill the hole.
//
//  moveInsertAndMoveToBack       Move-construct or move-assign each element in
//                                the target range (or 'std::memmove' if type
//                                is bitwise moveable) to the back of the
//                                deque to create a 1-slot hole, followed by
//                                move-assign of the movable source value to
//                                fill the hole.
//
//  moveInsertAndMoveToFront      Move-construct or move-assign each element in
//                                the target range (or 'std::memmove' if type
//                                is bitwise moveable) to the front of the
//                                deque to create a 1-slot hole, followed by
//                                move-assign of the movable source value to
//                                fill the hole.
//
//  emplaceAndMoveToBack          Move-construct or move-assign each element in
//                                the target range (or 'std::memmove' if type
//                                is bitwise moveable) to the back of the
//                                deque to create a 1-slot hole, followed by
//                                in-place construction of the source value to
//                                fill the hole.
//
//  emplaceAndMoveToFront         Move-construct or move-assign each element in
//                                the target range (or 'std::memmove' if type
//                                is bitwise moveable) to the front of the
//                                deque to create a 1-slot hole, followed by
//                                in-place construction of the source value to
//                                fill the hole.
//..
// The traits under consideration directly or indirectly by this component are:
//..
//  Trait                                         English description
//  -----                                         -------------------
//  bsl::is_trivially_copyable                    "TYPE has the bitwise
//                                                copyable trait", or
//                                                "TYPE is bitwise copyable"
//
//  bslmf::IsBitwiseMoveable                      "TYPE has the bitwise
//                                                moveable trait", or
//                                                "TYPE is bitwise moveable"
//..
//
///Aliasing
///--------
// There are some aliasing concerns in this component, due to the presence of
// the reference 'const VALUE_TYPE& value' argument, which may belong to a
// range that will be modified during the course of the operation.  All such
// aliasing concerns are taken care of properly.  Other aliasing concerns due
// to the copying of a range '[first .. last)' are *not* taken care of, since
// their intended use is for range assignments and insertions in standard
// containers, for which the standard explicitly says that 'first' and 'last'
// shall not be iterators into the container.
//
///Usage
///-----
// This component is for use by the 'bslstl' package.  Other clients should use
// the STL deque (in header '<deque>').

#include <bslscm_version.h>

#include <bslalg_arrayprimitives.h>
#include <bslalg_dequeimputil.h>
#include <bslalg_dequeiterator.h>

#include <bslma_allocator.h>
#include <bslma_allocatortraits.h>
#include <bslma_constructionutil.h>

#include <bslmf_functionpointertraits.h>
#include <bslmf_isconvertible.h>
#include <bslmf_isfundamental.h>
#include <bslmf_ispointer.h>
#include <bslmf_movableref.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#include <cstddef>  // std::size_t

#include <cstring>  // memmove

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Thu Oct 21 10:11:37 2021
// Command line: sim_cpp11_features.pl bslalg_dequeprimitives.h
# define COMPILING_BSLALG_DEQUEPRIMITIVES_H
# include <bslalg_dequeprimitives_cpp03.h>
# undef COMPILING_BSLALG_DEQUEPRIMITIVES_H
#else

namespace BloombergLP {

namespace {
        // Workaround for windows.  The windows compiler refuses to recognize
        // enum declarations within a class template.  TBD: verify this is
        // still a concern with recent Windows compilers, as unnamed namespaces
        // in header files are not a great workaround.

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

template <class VALUE_TYPE, int BLOCK_LENGTH, class ALLOCATOR>
class DequePrimitives_DequeElementGuard;

template <class VALUE_TYPE, int BLOCK_LENGTH>
class DequePrimitives_DequeMoveGuard;

template <class VALUE_TYPE, int BLOCK_LENGTH, class ALLOCATOR>
class DequePrimitives_ExternalDequeElementGuard;

template <class VALUE_TYPE, int BLOCK_LENGTH>
class DequePrimitives_DequeEndpointProctor;

                        // ======================
                        // struct DequePrimitives
                        // ======================

template <class VALUE_TYPE, int BLOCK_LENGTH>
struct DequePrimitives {
    // This 'struct' provides a namespace for a suite of utility functions that
    // operate on deques parameterized by the 'VALUE_TYPE' and 'BLOCK_LENGTH'.
    // Depending on the traits of 'VALUE_TYPE', the default and copy
    // constructors, destructor, assignment operators, etcetera may not be
    // invoked, and instead the operation can be optimized using a no-op,
    // bitwise move, or bitwise copy.

    // PUBLIC TYPES
    typedef std::size_t                             size_type;
    typedef DequeIterator<VALUE_TYPE, BLOCK_LENGTH> Iterator;

  private:
    // PRIVATE TYPES
    typedef DequeImpUtil<VALUE_TYPE, BLOCK_LENGTH>                   ImpUtil;
    typedef DequePrimitives_DequeMoveGuard<VALUE_TYPE, BLOCK_LENGTH> MoveGuard;
    typedef bslmf::MovableRefUtil                                    MoveUtil;

  public:
    // CLASS METHODS
    template <class ALLOCATOR>
    static void destruct(Iterator begin, Iterator end, ALLOCATOR allocator);
        // TBD: fix comment
        // Call the destructor on each of the elements of a deque of
        // parameterized 'VALUE_TYPE' in the specified range '[begin .. end)'.
        // The behavior is undefined unless 'begin <= end'.  Note that this
        // does not deallocate any memory (except memory deallocated by the
        // element destructor calls).

    template <class ALLOCATOR>
    static void destruct(Iterator   begin,
                         Iterator   end,
                         ALLOCATOR  allocator,
                         bslmf::MetaInt<NIL_TRAITS> *);
    template <class ALLOCATOR>
    static void destruct(Iterator   begin,
                         Iterator   end,
                         ALLOCATOR  allocator,
                         bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *);
        // Call the destructor on each of the elements of a deque of
        // parameterized 'VALUE_TYPE' in the specified range '[begin .. end)'.
        // The behavior is undefined unless 'begin <= end'.  Note that this
        // does not deallocate any memory (except memory deallocated by the
        // element destructor calls).  Note that the last argument is for
        // removing overload ambiguities and is not used.

    template <class ALLOCATOR>
    static Iterator erase(Iterator  *toBegin,
                          Iterator  *toEnd,
                          Iterator   fromBegin,
                          Iterator   first,
                          Iterator   last,
                          Iterator   fromEnd,
                          ALLOCATOR  allocator);
        // Call the destructor on each of the elements of a deque of
        // parameterized 'VALUE_TYPE' in the specified range '[first .. last)'.
        // Shift the elements to fill up the empty space after the erasure,
        // using the smaller of the range defined by '[fromBegin .. first)' and
        // '[last .. fromEnd)' after the erasure.  Load in the specified
        // 'toBegin' and 'toEnd' the new boundaries of the deque after erasure
        // and return an iterator pointing to the element immediately following
        // the removed elements.  The behavior is undefined unless
        // 'fromBegin <= first <= last <= fromEnd'.

    template <class ALLOCATOR>
    static Iterator erase(Iterator                   *toBegin,
                          Iterator                   *toEnd,
                          Iterator                    fromBegin,
                          Iterator                    first,
                          Iterator                    last,
                          Iterator                    fromEnd,
                          ALLOCATOR                   allocator,
                          bslmf::MetaInt<NIL_TRAITS> *);
    template <class ALLOCATOR>
    static Iterator erase(Iterator                                *toBegin,
                          Iterator                                *toEnd,
                          Iterator                                 fromBegin,
                          Iterator                                 first,
                          Iterator                                 last,
                          Iterator                                 fromEnd,
                          ALLOCATOR                                allocator,
                          bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *);
        // Call the destructor on each of the elements of a deque of
        // parameterized 'VALUE_TYPE' in the specified range '[first .. last)'.
        // Shift the elements from the smaller of the specified range
        // '[fromBegin .. first)' and '[last .. fromEnd)' to fill up the empty
        // spaces after the erasure.  Load in the specified 'toBegin' and
        // 'toEnd' the new boundaries of the deque after erasure and return an
        // iterator pointing to the element immediately following the removed
        // elements.  The behavior is undefined unless
        // 'fromBegin <= first <= last <= fromEnd'.  Note that the last
        // argument is for removing overload ambiguities and is not used.

    template <class ALLOCATOR>
    static void insertAndMoveToBack(Iterator          *toEnd,
                                    Iterator           fromEnd,
                                    Iterator           position,
                                    size_type          numElements,
                                    const VALUE_TYPE&  value,
                                    ALLOCATOR          allocator);
        // Insert the specified 'numElements' copies of the specified 'value'
        // at the specified 'position', by moving the elements in the range
        // '[position .. fromEnd)' forward by 'numElements' position.  Pass the
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
                          ALLOCATOR                                allocator,
                          bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *);
    template <class ALLOCATOR>
    static void insertAndMoveToBack(
                          Iterator                                *toEnd,
                          Iterator                                 fromEnd,
                          Iterator                                 position,
                          size_type                                numElements,
                          const VALUE_TYPE&                        value,
                          ALLOCATOR                                allocator,
                          bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *);
    template <class ALLOCATOR>
    static void insertAndMoveToBack(Iterator                   *toEnd,
                                    Iterator                    fromEnd,
                                    Iterator                    position,
                                    size_type                   numElements,
                                    const VALUE_TYPE&           value,
                                    ALLOCATOR                   allocator,
                                    bslmf::MetaInt<NIL_TRAITS> *);
        // Insert the specified 'numElements' copies of the specified 'value'
        // at the specified 'position', by moving the elements in the range
        // '[position .. fromEnd)' forward by 'numElements' position.  Pass the
        // specified 'allocator' to the copy constructor if appropriate.  Load
        // into the specified 'toEnd' an iterator to the end of the deque after
        // insertion (i.e., 'fromEnd + numElements').  The behavior is
        // undefined unless 'fromEnd + numElements' is a valid iterator (i.e.,
        // the block pointer array holds enough room after the 'fromEnd'
        // position to insert 'numElements').  Note that the last argument is
        // for removing overload ambiguities and is not used.

    template <class FWD_ITER, class ALLOCATOR>
    static void insertAndMoveToBack(Iterator  *toEnd,
                                    Iterator   fromEnd,
                                    Iterator   position,
                                    FWD_ITER   first,
                                    FWD_ITER   last,
                                    size_type  numElements,
                                    ALLOCATOR  allocator);
        // Insert the specified 'numElements' in the range '[first .. last)' at
        // the specified 'position', by moving the elements in the range
        // '[position .. fromEnd)' forward by 'numElements' position.  Pass the
        // specified 'allocator' to the copy constructor if appropriate.  Load
        // into the specified 'toEnd' an iterator to the end of the data after
        // insertion (i.e., 'fromEnd + numElements').  The behavior is
        // undefined unless 'fromEnd + numElements' is a valid iterator (i.e.,
        // the block pointer array holds enough room after the 'fromEnd'
        // position to insert 'numElements').

    template <class ALLOCATOR>
    static void moveInsertAndMoveToBack(
                                     Iterator                      *toEnd,
                                     Iterator                       fromEnd,
                                     Iterator                       position,
                                     bslmf::MovableRef<VALUE_TYPE>  value,
                                     ALLOCATOR                      allocator);
        // Insert the specified move-insertable 'value' at the specified
        // 'position' by moving the elements in the range
        // '[position .. fromEnd)' forward by 1 position; pass the specified
        // 'allocator' to the move constructor if appropriate.  Load into the
        // specified 'toEnd' an iterator one past the inserted element (i.e.,
        // 'fromEnd + 1').  The behavior is undefined unless 'fromEnd + 1' is a
        // valid iterator (i.e., the block pointer array holds enough room
        // after the 'fromEnd' position to insert 1 element).

    template <class ALLOCATOR>
    static void moveInsertAndMoveToBack(
                            Iterator                                *toEnd,
                            Iterator                                 fromEnd,
                            Iterator                                 position,
                            bslmf::MovableRef<VALUE_TYPE>            value,
                            ALLOCATOR                                allocator,
                            bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *);
    template <class ALLOCATOR>
    static void moveInsertAndMoveToBack(
                            Iterator                                *toEnd,
                            Iterator                                 fromEnd,
                            Iterator                                 position,
                            bslmf::MovableRef<VALUE_TYPE>            value,
                            ALLOCATOR                                allocator,
                            bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *);
    template <class ALLOCATOR>
    static void moveInsertAndMoveToBack(
                                      Iterator                      *toEnd,
                                      Iterator                       fromEnd,
                                      Iterator                       position,
                                      bslmf::MovableRef<VALUE_TYPE>  value,
                                      ALLOCATOR                      allocator,
                                      bslmf::MetaInt<NIL_TRAITS>    *);
        // Insert the specified move-insertable 'value' at the specified
        // 'position' by moving the elements in the range
        // '[position .. fromEnd)' forward by 1 position; pass the specified
        // 'allocator' to the move constructor if appropriate.  Load into the
        // specified 'toEnd' an iterator one past the inserted element (i.e.,
        // 'fromEnd + 1').  The behavior is undefined unless 'fromEnd + 1' is a
        // valid iterator (i.e., the block pointer array holds enough room
        // after the 'fromEnd' position to insert 1 element).  Note that the
        // last argument is for removing overload ambiguities and is not used.

    template <class ALLOCATOR>
    static void insertAndMoveToFront(Iterator          *toBegin,
                                     Iterator           fromBegin,
                                     Iterator           position,
                                     size_type          numElements,
                                     const VALUE_TYPE&  value,
                                     ALLOCATOR          allocator);
        // Insert the specified 'numElements' copies of the specified 'value'
        // at the specified 'position', by moving the elements in the range
        // '[fromBegin .. position)' backward by 'numElements' position.  Pass
        // the specified 'allocator' to the copy constructor if appropriate.
        // Load into the specified 'toBegin' an iterator to the beginning of
        // the data after insertion (i.e., 'fromBegin - numElements').  The
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
                          ALLOCATOR                                allocator,
                          bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *);
    template <class ALLOCATOR>
    static void insertAndMoveToFront(
                          Iterator                                *toBegin,
                          Iterator                                 fromBegin,
                          Iterator                                 position,
                          size_type                                numElements,
                          const VALUE_TYPE&                        value,
                          ALLOCATOR                                allocator,
                          bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *);
    template <class ALLOCATOR>
    static void insertAndMoveToFront(Iterator                   *toBegin,
                                     Iterator                    fromBegin,
                                     Iterator                    position,
                                     size_type                   numElements,
                                     const VALUE_TYPE&           value,
                                     ALLOCATOR                   allocator,
                                     bslmf::MetaInt<NIL_TRAITS> *);
        // Insert the specified 'numElements' copies of the specified 'value'
        // at the specified 'position', by moving the elements in the range
        // '[fromBegin .. position)' backward by 'numElements' position.  Pass
        // the specified 'allocator' to the copy constructor if appropriate.
        // Load into the specified 'toBegin' an iterator to the beginning of
        // the data after insertion (i.e., 'fromBegin - numElements'.  The
        // behavior is undefined unless 'fromBegin - numElements' is a valid
        // iterator (i.e., the block pointer array holds enough room before the
        // 'fromBegin' position to insert 'numElements').  Note that the last
        // argument is for removing overload ambiguities and is not used.

    template <class FWD_ITER, class ALLOCATOR>
    static void insertAndMoveToFront(Iterator  *toBegin,
                                     Iterator   fromBegin,
                                     Iterator   position,
                                     FWD_ITER   first,
                                     FWD_ITER   last,
                                     size_type  numElements,
                                     ALLOCATOR  allocator);
        // Insert the specified 'numElements' in the range '[first .. last)' at
        // the specified 'position', by moving the elements in the range
        // '[fromBegin .. position)' backward by 'numElements' position.  Pass
        // the specified 'allocator' to the copy constructor if appropriate.
        // Load into the specified 'toBegin' an iterator to the end of the data
        // after insertion (i.e., 'fromBegin - numElements').  The behavior is
        // undefined unless 'fromBegin - numElements' is a valid iterator
        // (i.e., the block pointer array holds enough room before the
        // 'fromBefore' position to insert 'numElements').

    template <class ALLOCATOR>
    static void moveInsertAndMoveToFront(
                                     Iterator                      *toBegin,
                                     Iterator                       fromBegin,
                                     Iterator                       position,
                                     bslmf::MovableRef<VALUE_TYPE>  value,
                                     ALLOCATOR                      allocator);
        // Insert the specified move-insertable 'value' at the specified
        // 'position' by moving the elements in the range
        // '[fromBegin .. position)' backward by 1 position; pass the specified
        // 'allocator' to the move constructor if appropriate.  Load into the
        // specified 'toBegin' an iterator to the inserted element (i.e.,
        // 'fromBegin - 1').  The behavior is undefined unless
        // 'fromBegin - 1' is a valid iterator (i.e., the block pointer array
        // holds enough room before the 'fromBegin' position to insert 1
        // element).

    template <class ALLOCATOR>
    static void moveInsertAndMoveToFront(
                            Iterator                                *toBegin,
                            Iterator                                 fromBegin,
                            Iterator                                 position,
                            bslmf::MovableRef<VALUE_TYPE>            value,
                            ALLOCATOR                                allocator,
                            bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *);
    template <class ALLOCATOR>
    static void moveInsertAndMoveToFront(
                            Iterator                                *toBegin,
                            Iterator                                 fromBegin,
                            Iterator                                 position,
                            bslmf::MovableRef<VALUE_TYPE>            value,
                            ALLOCATOR                                allocator,
                            bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *);
    template <class ALLOCATOR>
    static void moveInsertAndMoveToFront(
                                      Iterator                      *toBegin,
                                      Iterator                       fromBegin,
                                      Iterator                       position,
                                      bslmf::MovableRef<VALUE_TYPE>  value,
                                      ALLOCATOR                      allocator,
                                      bslmf::MetaInt<NIL_TRAITS>    *);
        // Insert the specified move-insertable 'value' at the specified
        // 'position' by moving the elements in the range
        // '[fromBegin .. position)' backward by 1 position; pass the specified
        // 'allocator' to the move constructor if appropriate.  Load into the
        // specified 'toBegin' an iterator to the inserted element (i.e.,
        // 'fromBegin - 1').  The behavior is undefined unless
        // 'fromBegin - 1' is a valid iterator (i.e., the block pointer array
        // holds enough room before the 'fromBegin' position to insert 1
        // element).  Note that the last argument is for removing overload
        // ambiguities and is not used.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class ALLOCATOR, class... Args>
    static void emplaceAndMoveToBack(Iterator  *toEnd,
                                     Iterator   fromEnd,
                                     Iterator   position,
                                     ALLOCATOR  allocator,
                                     Args&&...  arguments);
        // Insert at the specified 'position' a newly created 'VALUE_TYPE'
        // object, constructed by forwarding the specified 'allocator' (if
        // required) and the specified (variable number of) 'arguments' to the
        // corresponding constructor of 'VALUE_TYPE', and move the elements in
        // the range '[position .. fromEnd)' forward by 1 position.  Load into
        // the specified 'toEnd' an iterator one past the inserted element
        // (i.e., 'fromEnd + 1').  The behavior is undefined unless
        // 'fromEnd + 1' is a valid iterator (i.e., the block pointer array
        // holds enough room after the 'fromEnd' position to insert 1 element).

    template <class ALLOCATOR, class... Args>
    static void emplaceAndMoveToBackDispatch(
                           Iterator                                *toEnd,
                           Iterator                                 fromEnd,
                           Iterator                                 position,
                           ALLOCATOR                                allocator,
                           bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *,
                           Args&&...                                arguments);
    template <class ALLOCATOR, class... Args>
    static void emplaceAndMoveToBackDispatch(
                           Iterator                                *toEnd,
                           Iterator                                 fromEnd,
                           Iterator                                 position,
                           ALLOCATOR                                allocator,
                           bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *,
                           Args&&...                                arguments);
    template <class ALLOCATOR, class... Args>
    static void emplaceAndMoveToBackDispatch(
                                     Iterator                   *toEnd,
                                     Iterator                    fromEnd,
                                     Iterator                    position,
                                     ALLOCATOR                   allocator,
                                     bslmf::MetaInt<NIL_TRAITS> *,
                                     Args&&...                   arguments);
        // Insert at the specified 'position' a newly created 'VALUE_TYPE'
        // object, constructed by forwarding the specified 'allocator' (if
        // required) and the specified (variable number of) 'arguments' to the
        // corresponding constructor of 'VALUE_TYPE', and move the elements in
        // the range '[position .. fromEnd)' forward by 1 position.  Load into
        // the specified 'toEnd' an iterator one past the inserted element
        // (i.e., 'fromEnd + 1').  The behavior is undefined unless
        // 'fromEnd + 1' is a valid iterator (i.e., the block pointer array
        // holds enough room after the 'fromEnd' position to insert 1 element).
        // Note that the next to last argument is for removing overload
        // ambiguities and is not used.

    template <class ALLOCATOR, class... Args>
    static void emplaceAndMoveToFront(Iterator  *toBegin,
                                      Iterator   fromBegin,
                                      Iterator   position,
                                      ALLOCATOR  allocator,
                                      Args&&...  arguments);
        // Insert at the specified 'position' a newly created 'VALUE_TYPE'
        // object, constructed by forwarding the specified 'allocator' (if
        // required) and the specified (variable number of) 'arguments' to the
        // corresponding constructor of 'VALUE_TYPE', and move the elements in
        // the range '[fromBegin .. position)' backward by 1 position.  Load
        // into the specified 'toBegin' an iterator to the inserted element
        // (i.e., 'fromBegin - 1').  The behavior is undefined unless
        // 'fromBegin - 1' is a valid iterator (i.e., the block pointer array
        // holds enough room before the 'fromBegin' position to insert 1
        // element).

    template <class ALLOCATOR, class... Args>
    static void emplaceAndMoveToFrontDispatch(
                           Iterator                                *toBegin,
                           Iterator                                 fromBegin,
                           Iterator                                 position,
                           ALLOCATOR                                allocator,
                           bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *,
                           Args&&...                                arguments);
    template <class ALLOCATOR, class... Args>
    static void emplaceAndMoveToFrontDispatch(
                           Iterator                                *toBegin,
                           Iterator                                 fromBegin,
                           Iterator                                 position,
                           ALLOCATOR                                allocator,
                           bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *,
                           Args&&...                                arguments);
    template <class ALLOCATOR, class... Args>
    static void emplaceAndMoveToFrontDispatch(
                                      Iterator                   *toBegin,
                                      Iterator                    fromBegin,
                                      Iterator                    position,
                                      ALLOCATOR                   allocator,
                                      bslmf::MetaInt<NIL_TRAITS> *,
                                      Args&&...                   arguments);
        // Insert at the specified 'position' a newly created 'VALUE_TYPE'
        // object, constructed by forwarding the specified 'allocator' (if
        // required) and the specified (variable number of) 'arguments' to the
        // corresponding constructor of 'VALUE_TYPE', and move the elements in
        // the range '[fromBegin .. position)' backward by 1 position.  Load
        // into the specified 'toBegin' an iterator to the inserted element
        // (i.e., 'fromBegin - 1').  The behavior is undefined unless
        // 'fromBegin - 1' is a valid iterator (i.e., the block pointer array
        // holds enough room before the 'fromBegin' position to insert 1
        // element).  Note that the next to last argument is for removing
        // overload ambiguities and is not used.
#endif

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
                                       ALLOCATOR          allocator);
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
                                        ALLOCATOR          allocator);
        // Prepend the specified 'numElements' copies of the specified 'value'
        // to the deque starting at the specified 'fromBegin' iterator, passing
        // the specified 'allocator' through to the new elements, and load into
        // the specified 'toBegin' an iterator pointing to the end of the data
        // after prepending, i.e., 'fromBegin - numElements'.  The behavior is
        // undefined unless 'fromBegin - numElements' is a valid iterator
        // (i.e., the block pointer array holds enough room before the
        // 'fromBegin' position to insert 'numElements').

    template <class ALLOCATOR>
    static void valueInititalizeN(Iterator  *toEnd,
                                  Iterator   fromEnd,
                                  size_type  numElements,
                                  ALLOCATOR  allocator);
        // Append the specified 'numElements' value-initialized objects to the
        // deque ending at the specified 'fromEnd' iterator, passing the
        // specified 'allocator' through to the new elements, and load into the
        // specified 'toEnd' an iterator pointing to the end of the data after
        // appending (i.e., 'fromEnd + numElements').  The behavior is
        // undefined unless 'fromEnd + numElements' is a valid iterator (i.e.,
        // the block pointer array holds enough room after the 'fromEnd'
        // position to insert 'numElements').
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

    // CLASS METHODS
    template <class ALLOCATOR>
    static void destruct(Iterator begin, Iterator end, ALLOCATOR allocator);

    template <class ALLOCATOR>
    static Iterator erase(Iterator  *toBegin,
                          Iterator  *toEnd,
                          Iterator   fromBegin,
                          Iterator   first,
                          Iterator   last,
                          Iterator   fromEnd,
                          ALLOCATOR  allocator);

    template <class ALLOCATOR>
    static void insertAndMoveToBack(Iterator          *toEnd,
                                    Iterator           fromEnd,
                                    Iterator           position,
                                    size_type          numElements,
                                    const VALUE_TYPE&  value,
                                    ALLOCATOR          allocator);

    template <class FWD_ITER, class ALLOCATOR>
    static void insertAndMoveToBack(Iterator  *toEnd,
                                    Iterator   fromEnd,
                                    Iterator   position,
                                    FWD_ITER   first,
                                    FWD_ITER   last,
                                    size_type  numElements,
                                    ALLOCATOR  allocator);

    template <class ALLOCATOR>
    static void insertAndMoveToFront(Iterator          *toBegin,
                                     Iterator           fromBegin,
                                     Iterator           position,
                                     size_type          numElements,
                                     const VALUE_TYPE&  value,
                                     ALLOCATOR          allocator);

    template <class FWD_ITER, class ALLOCATOR>
    static void insertAndMoveToFront(Iterator  *toBegin,
                                     Iterator   fromBegin,
                                     Iterator   position,
                                     FWD_ITER   first,
                                     FWD_ITER   last,
                                     size_type  numElements,
                                     ALLOCATOR  allocator);

    template <class ALLOCATOR>
    static void uninitializedFillNBack(Iterator          *toEnd,
                                       Iterator           fromEnd,
                                       size_type          numElements,
                                       const VALUE_TYPE&  value,
                                       ALLOCATOR          allocator);
    template <class ALLOCATOR>
    static void uninitializedFillNBack(Iterator                   *toEnd,
                                       Iterator                    fromEnd,
                                       size_type                   numElements,
                                       const VALUE_TYPE&           value,
                                       ALLOCATOR                   allocator,
                                       bslmf::MetaInt<NIL_TRAITS> *);
    template <class ALLOCATOR>
    static void uninitializedFillNBack(
                                   Iterator                       *toEnd,
                                   Iterator                        fromEnd,
                                   size_type                       numElements,
                                   const VALUE_TYPE&               value,
                                   ALLOCATOR                       allocator,
                                   bslmf::MetaInt<NON_NIL_TRAITS> *);

    template <class ALLOCATOR>
    static void uninitializedFillNFront(Iterator          *toBegin,
                                        Iterator           fromBegin,
                                        size_type          numElements,
                                        const VALUE_TYPE&  value,
                                        ALLOCATOR          allocator);
    template <class ALLOCATOR>
    static void uninitializedFillNFront(
                                       Iterator                   *toBegin,
                                       Iterator                    fromBegin,
                                       size_type                   numElements,
                                       const VALUE_TYPE&           value,
                                       ALLOCATOR                   allocator,
                                       bslmf::MetaInt<NIL_TRAITS> *);
    template <class ALLOCATOR>
    static void uninitializedFillNFront(
                                   Iterator                       *toBegin,
                                   Iterator                        fromBegin,
                                   size_type                       numElements,
                                   const VALUE_TYPE&               value,
                                   ALLOCATOR                       allocator,
                                   bslmf::MetaInt<NON_NIL_TRAITS> *);

    template <class ALLOCATOR>
    static void valueInititalizeN(Iterator  *toEnd,
                                  Iterator   fromEnd,
                                  size_type  numElements,
                                  ALLOCATOR  allocator);

    template <class ALLOCATOR>
    static void valueInititalizeN(Iterator  *toEnd,
                                  Iterator   fromEnd,
                                  size_type  numElements,
                                  ALLOCATOR  allocator,
                                  bslmf::MetaInt<NIL_TRAITS> *);

    template <class ALLOCATOR>
    static void valueInititalizeN(Iterator  *toEnd,
                                  Iterator   fromEnd,
                                  size_type  numElements,
                                  ALLOCATOR  allocator,
                                  bslmf::MetaInt<NON_NIL_TRAITS> *);
};

                    // =======================================
                    // class DequePrimitives_DequeElementGuard
                    // =======================================

template <class VALUE_TYPE, int BLOCK_LENGTH, class ALLOCATOR>
class DequePrimitives_DequeElementGuard {
    // This 'class' provides a specialized proctor object that, upon
    // destruction and unless the 'release' method has been called, destroys
    // the elements in a segment of a deque of parameterized 'VALUE_TYPE'.  The
    // elements destroyed are delimited by the "guarded" range
    // '[d_begin .. d_end)'.

  public:
    // PUBLIC TYPES
    typedef DequeIterator<VALUE_TYPE, BLOCK_LENGTH>  Iterator;

  private:
    // DATA
    Iterator  d_begin;      // iterator of first element in guarded range
    Iterator  d_end;        // iterator beyond last element in guarded range
    ALLOCATOR d_allocator;

  private:
    // NOT IMPLEMENTED
    DequePrimitives_DequeElementGuard(
                                     const DequePrimitives_DequeElementGuard&);
    DequePrimitives_DequeElementGuard& operator=(
                                     const DequePrimitives_DequeElementGuard&);

  public:
    // CREATORS
    DequePrimitives_DequeElementGuard(const Iterator& begin,
                                      const Iterator& end,
                                      ALLOCATOR       allocator);
        // Create a deque exception guard object for the sequence of elements
        // of the parameterized 'VALUE_TYPE' delimited by the specified range
        // '[begin .. end)'.  The behavior is undefined unless 'begin <= end'
        // and unless each element in the range '[begin .. end)' has been
        // initialized.

    ~DequePrimitives_DequeElementGuard();
        // Call the destructor on each of the elements of the parameterized
        // 'VALUE_TYPE' delimited by the range '[begin .. end)' and destroy
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

template <class VALUE_TYPE, int BLOCK_LENGTH, class ALLOCATOR>
class DequePrimitives_ExternalDequeElementGuard {
    // This 'class' provides a specialized proctor object that, upon
    // destruction and unless the 'release' method has been called, destroys
    // the elements in a segment of a 'bsl::deque' of parameterized type
    // 'VALUE_TYPE'.  The elements destroyed are delimited by the "guarded"
    // range '[*d_begin .. *d_end)'.  Note that the range guarded by this
    // 'class' is dynamic and can be changed outside of this 'class'.

  public:
    // PUBLIC TYPES
    typedef DequeIterator<VALUE_TYPE, BLOCK_LENGTH>  Iterator;

  private:
    // DATA
    Iterator  *d_begin_p;    // pointer to iterator of first element in guarded
                             // range

    Iterator  *d_end_p;      // pointer to iterator beyond last element in
                             // guarded range
    ALLOCATOR  d_allocator;

  private:
    // NOT IMPLEMENTED
    DequePrimitives_ExternalDequeElementGuard(
                             const DequePrimitives_ExternalDequeElementGuard&);
    DequePrimitives_ExternalDequeElementGuard& operator=(
                             const DequePrimitives_ExternalDequeElementGuard&);

  public:
    // CREATORS
    DequePrimitives_ExternalDequeElementGuard(Iterator  *begin,
                                              Iterator  *end,
                                              ALLOCATOR  allocator);
        // Create a deque exception guard object for the sequence of elements
        // of the parameterized 'VALUE_TYPE' delimited by the specified range
        // '[*begin .. *end)'.  The behavior is undefined unless '*begin' <=
        // '*end' and unless each element in the range '[*begin .. *end)' has
        // been initialized.

    ~DequePrimitives_ExternalDequeElementGuard();
        // Call the destructor on each of the elements of the parameterized
        // 'VALUE_TYPE' delimited by the range '[*d_begin_p .. *d_end_p)' and
        // destroy this array exception guard.

    // MANIPULATORS
    void release();
        // Set the range of elements guarded by this object to be empty.  Note
        // that 'd_begin_p == d_end_p == 0' following this operation.
};

                    // ====================================
                    // class DequePrimitives_DequeMoveGuard
                    // ====================================

template <class VALUE_TYPE, int BLOCK_LENGTH>
class DequePrimitives_DequeMoveGuard {
    // This 'class' provides a guard object that, upon destruction and unless
    // the 'release' method has been called, uses 'moveBack' or 'moveFront' to
    // move the "guarded" range '[d_source_p .. d_source_p + d_size - 1]'
    // back to '[d_destination_p .. d_destination_p + d_size -1]'.

  public:
    // PUBLIC TYPES
    typedef DequeIterator<VALUE_TYPE, BLOCK_LENGTH>           Iterator;
    typedef bslalg::DequePrimitives<VALUE_TYPE, BLOCK_LENGTH> DequePrimitives;

  private:
    // DATA
    Iterator    d_destination_p;  // destination of the move
    Iterator    d_source_p;       // source of the move
    std::size_t d_size;           // size of the range being guarded
    bool        d_front;          // whether to use 'moveFront'

  private:
    // NOT IMPLEMENTED
    DequePrimitives_DequeMoveGuard(const DequePrimitives_DequeMoveGuard&);
    DequePrimitives_DequeMoveGuard& operator=(
                                   const DequePrimitives_DequeMoveGuard&);

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

                    // ==========================================
                    // class DequePrimitives_DequeEndpointProctor
                    // ==========================================

template <class VALUE_TYPE, int BLOCK_LENGTH>
class DequePrimitives_DequeEndpointProctor {
    // This class implements a proctor that, upon destruction and unless its
    // 'release' method has previously been invoked, sets a deque endpoint
    // (i.e., "start" or "finish" iterator) to a position within the deque.
    // Both the endpoint and position are supplied at construction.  See
    // 'emplaceAndMoveToBack' and 'emplaceAndMoveToFront' for use cases.

  public:
    // PUBLIC TYPES
    typedef DequeIterator<VALUE_TYPE, BLOCK_LENGTH> Iterator;

  private:
    // DATA
    Iterator *d_endpoint_p;  // proctored endpoint of a deque
    Iterator  d_position;    // set endpoint to this upon destruction

  private:
    // NOT IMPLEMENTED
    DequePrimitives_DequeEndpointProctor(
                                  const DequePrimitives_DequeEndpointProctor&);
    DequePrimitives_DequeEndpointProctor& operator=(
                                  const DequePrimitives_DequeEndpointProctor&);

  public:
    // CREATORS
    DequePrimitives_DequeEndpointProctor(Iterator *endpoint,
                                         Iterator  position);
        // Create a deque endpoint proctor that conditionally manages the
        // specified 'endpoint' (if non-zero) by setting '*endpoint' to the
        // specified 'position' (if not released -- see 'release') upon
        // destruction.

    ~DequePrimitives_DequeEndpointProctor();
        // Destroy this endpoint proctor, and set the deque endpoint it manages
        // (if any) to the position supplied at construction.  If no endpoint
        // is currently being managed, this method has no effect.

    // MANIPULATORS
    void release();
        // Release from management the deque endpoint currently managed by this
        // proctor.  If no endpoint is currently being managed, this method has
        // no effect.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------
                        // class DequePrimitives
                        // ---------------------

// CLASS METHODS
template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
inline
void DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::destruct(Iterator   begin,
                                                         Iterator   end,
                                                         ALLOCATOR  allocator)
{
    enum {
        IS_BITWISECOPYABLE  = bsl::is_trivially_copyable<VALUE_TYPE>::value,

        VALUE = IS_BITWISECOPYABLE
              ? BITWISE_COPYABLE_TRAITS
              : NIL_TRAITS
    };

    return destruct(begin, end, allocator, (bslmf::MetaInt<VALUE>*) 0);
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
void DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
                      ::destruct(Iterator                                 ,
                                 Iterator                                 ,
                                 ALLOCATOR                                ,
                                 bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *)
{
    // No-op.
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
void DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>
                                   ::destruct(
                                         Iterator                    begin,
                                         Iterator                    end,
                                         ALLOCATOR                   allocator,
                                         bslmf::MetaInt<NIL_TRAITS> *)
{
    for (; !(begin == end); ++begin) {
        bsl::allocator_traits<ALLOCATOR>::destroy(allocator,
                                                  begin.valuePtr());
    }
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
inline
typename
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::Iterator
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::erase(Iterator  *toBegin,
                                                 Iterator  *toEnd,
                                                 Iterator   fromBegin,
                                                 Iterator   first,
                                                 Iterator   last,
                                                 Iterator   fromEnd,
                                                 ALLOCATOR  allocator)
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
                                         ALLOCATOR                   allocator,
                                         bslmf::MetaInt<NIL_TRAITS> *)
{
    if (first == last) {  // Nothing to delete, bail out fast
        *toBegin = fromBegin;
        *toEnd   = fromEnd;
        return first;                                                 // RETURN
    }

    size_type frontSize = first - fromBegin;
    size_type backSize  = fromEnd - last;
    Iterator  ret;

    if (frontSize < backSize) {
        ret = last;
        for (; 0 < frontSize; --frontSize) {
            --last;
            --first;
            *last = MoveUtil::move(*first);
        }
        *toBegin = last;
        *toEnd   = fromEnd;
    }
    else {
        ret = first;
        for (; 0 < backSize; --backSize, ++first, ++last) {
            *first = MoveUtil::move(*last);
        }
        *toBegin = fromBegin;
        *toEnd   = first;
    }
    destruct(first, last, allocator);
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
                         ALLOCATOR                                ,
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
                                               ALLOCATOR          allocator)
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
                         ALLOCATOR                                allocator,
                         bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *)
{
    size_type backSize = fromEnd - position;
    Iterator  end      = fromEnd;
    Iterator  dest     = end + numElements;

    bsls::ObjectBuffer<VALUE_TYPE> space;
    bslma::ConstructionUtil::construct(
                              BSLS_UTIL_ADDRESSOF(space.object()),
                              bslma::Default::allocator(),
                              value);
    bslma::DestructorProctor<VALUE_TYPE>
                                     temp(BSLS_UTIL_ADDRESSOF(space.object()));

    // TBD: this does the same thing as the old code - don't like that we
    // circumvent the whole allocator thing, but for now, let's keep it
    // consistent.
    // ConstructorProxy<VALUE_TYPE>
    //                           tempValue(value, bslma::Default::allocator());

    // No guard needed since all the operations won't throw due to the
    // bitwise-copyable trait
    moveBack(&dest, &end, backSize);
    uninitializedFillNFront(&dest, dest, numElements, space.object(),
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
                         ALLOCATOR                                allocator,
                         bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *)
{
    typedef DequePrimitives_ExternalDequeElementGuard<VALUE_TYPE,
                                                      BLOCK_LENGTH,
                                                      ALLOCATOR>    ExtGuard;


    size_type backSize = fromEnd - position;
    Iterator  end      = fromEnd;
    Iterator  dest     = end + numElements;

    // In case of aliasing, make a copy of the value.
    bsls::ObjectBuffer<VALUE_TYPE> space;
    bslma::ConstructionUtil::construct(
                              BSLS_UTIL_ADDRESSOF(space.object()),
                              bslma::Default::allocator(),
                              value);
    bslma::DestructorProctor<VALUE_TYPE>
                                     temp(BSLS_UTIL_ADDRESSOF(space.object()));

    // TBD: this does the same thing as the old code - don't like that we
    // circumvent the whole allocator thing, but for now, let's keep it
    // consistent.
    // ConstructorProxy<VALUE_TYPE>
    //                           tempValue(value, bslma::Default::allocator());

    // Setup a reverse guard that will reverse the moveBack operation in case
    // of an exception.
    MoveGuard guard(end, dest, backSize, false);

    moveBack(&dest, &end, backSize);

    // Create a guard for 'uninitializedFillNBack' because it can throw under
    // 'bitwisemoveable' trait.  Need to use this special guard because
    // uninitializedFillNFront is not exception safe.
    Iterator dest2(dest);
    ExtGuard eguard(&dest, &dest2, allocator);

    uninitializedFillNFront(&dest, dest, numElements, space.object(),
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
                                       ALLOCATOR                   allocator,
                                       bslmf::MetaInt<NIL_TRAITS> *)
{
    typedef DequePrimitives_DequeElementGuard<VALUE_TYPE,
                                              BLOCK_LENGTH,
                                              ALLOCATOR>    ElementGuard;

    size_type backSize = fromEnd - position;
    Iterator  end      = fromEnd;
    Iterator  dest     = end + numElements;
    size_type numDest;

    // In case of aliasing, make a copy of the value.

    bsls::ObjectBuffer<VALUE_TYPE> space;
    bslma::ConstructionUtil::construct(BSLS_UTIL_ADDRESSOF(space.object()),
                                       bslma::Default::allocator(),
                                       value);
    bslma::DestructorProctor<VALUE_TYPE>
                                     temp(BSLS_UTIL_ADDRESSOF(space.object()));

    // TBD: this does the same thing as the old code - don't like that we
    // circumvent the whole allocator thing, but for now, let's keep it
    // consistent.
    // ConstructorProxy<VALUE_TYPE> tempValue(value,
    //                                        bslma::Default::allocator());

    ElementGuard guard(dest, dest, allocator);
    if (backSize >= numElements) {
        for (numDest = numElements; 0 < numDest; --numDest) {
            --dest;
            --end;
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        *end);
            guard.moveBegin(-1);
        }
        for (backSize -= numElements; 0 < backSize; --backSize) {
            --dest;
            --end;
            *dest = *end;
        }
        for (numDest = numElements; 0 < numDest; --numDest, ++position) {
            *position = space.object();
        }
    } else {
        for (numDest = backSize; 0 < numDest; --numDest) {
            --dest;
            --end;
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        *end);
            guard.moveBegin(-1);
        }
        for (numDest = numElements; backSize < numDest; --numDest) {
            --dest;
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        space.object());

            guard.moveBegin(-1);
        }
        for (; 0 < numDest; --numDest, ++position) {
            *position = space.object();
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
                                                        ALLOCATOR  allocator)
{
    typedef DequePrimitives_DequeElementGuard<VALUE_TYPE,
                                              BLOCK_LENGTH,
                                              ALLOCATOR>    ElementGuard;

    size_type backSize = fromEnd - position;
    Iterator  end      = fromEnd;
    Iterator  dest     = end + numElements;
    size_type numDest;

    ElementGuard guard(dest, dest, allocator);
    if (backSize >= numElements) {
        for (numDest = numElements; 0 < numDest; --numDest) {
            --dest;
            --end;
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        *end);
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
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        *end);
            guard.moveBegin(-1);
        }
        for (numDest = backSize; 0 < numDest; --numDest, ++position, ++first) {
            *position = *first;
        }
        // Second guard needed because we're guarding from a different range.
        ElementGuard guard2(position, position, allocator);
        for (numDest = numElements; backSize < numDest; ++first, ++position,
                                                                   --numDest) {
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        position.valuePtr(),
                                                        *first);
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
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::moveInsertAndMoveToBack(
                                      Iterator                      *toEnd,
                                      Iterator                       fromEnd,
                                      Iterator                       position,
                                      bslmf::MovableRef<VALUE_TYPE>  value,
                                      ALLOCATOR                      allocator)
{
    enum {
        IS_BITWISECOPYABLE = bsl::is_trivially_copyable<VALUE_TYPE>::value,
        IS_BITWISEMOVEABLE = bslmf::IsBitwiseMoveable<VALUE_TYPE>::value,

        VALUE = IS_BITWISECOPYABLE
                ? BITWISE_COPYABLE_TRAITS : IS_BITWISEMOVEABLE
                ? BITWISE_MOVEABLE_TRAITS : NIL_TRAITS
    };

    VALUE_TYPE& lvalue = value;

    moveInsertAndMoveToBack(toEnd,
                            fromEnd,
                            position,
                            MoveUtil::move(lvalue),
                            allocator,
                            (bslmf::MetaInt<VALUE> *)0);
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::moveInsertAndMoveToBack(
                            Iterator                                *toEnd,
                            Iterator                                 fromEnd,
                            Iterator                                 position,
                            bslmf::MovableRef<VALUE_TYPE>            value,
                            ALLOCATOR                                allocator,
                            bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *)
{
    const size_type backSize = fromEnd - position;
    Iterator        end      = fromEnd;
    Iterator        dest     = end + 1;

    // No guard needed since no operations will throw with
    // 'BITWISE_COPYABLE_TRAITS'.

    moveBack(&dest, &end, backSize);
    --dest;

    VALUE_TYPE& lvalue = value;
    bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                dest.valuePtr(),
                                                MoveUtil::move(lvalue));

    *toEnd = fromEnd + 1;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::moveInsertAndMoveToBack(
                            Iterator                                *toEnd,
                            Iterator                                 fromEnd,
                            Iterator                                 position,
                            bslmf::MovableRef<VALUE_TYPE>            value,
                            ALLOCATOR                                allocator,
                            bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *)
{
    const size_type backSize = fromEnd - position;
    Iterator        end      = fromEnd;
    Iterator        dest     = end + 1;

    // Create a reverse guard that will undo the 'moveBack' operation in case
    // of an exception.

    MoveGuard guard(end, dest, backSize, false);

    moveBack(&dest, &end, backSize);
    --dest;

    VALUE_TYPE& lvalue = value;
    bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                dest.valuePtr(),
                                                MoveUtil::move(lvalue));

    guard.release();

    *toEnd = fromEnd + 1;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::moveInsertAndMoveToBack(
                                      Iterator                      *toEnd,
                                      Iterator                       fromEnd,
                                      Iterator                       position,
                                      bslmf::MovableRef<VALUE_TYPE>  value,
                                      ALLOCATOR                      allocator,
                                      bslmf::MetaInt<NIL_TRAITS>    *)
{
    typedef DequePrimitives_DequeElementGuard<VALUE_TYPE,
                                              BLOCK_LENGTH,
                                              ALLOCATOR>    ElementGuard;

    size_type backSize = fromEnd - position;
    Iterator  end      = fromEnd;
    Iterator  dest     = end + 1;

    BSLS_ASSERT_SAFE(backSize >= 1);

    ElementGuard guard(dest, dest, allocator);

    // 1. move-construct back-most element being bumped 1 slot

    --dest;
    --end;
    bsl::allocator_traits<ALLOCATOR>::construct(
                                             allocator,
                                             dest.valuePtr(),
                                             MoveUtil::move_if_noexcept(*end));
    guard.moveBegin(-1);

    // 2. move-assign other existing elements being bumped back 1 slot

    for (backSize -= 1; 0 < backSize; --backSize) {
        --dest;
        --end;
        *dest = MoveUtil::move_if_noexcept(*end);
    }

    // 3. move-assign new element

    VALUE_TYPE& lvalue = value;
    *position = MoveUtil::move(lvalue);

    guard.release();

    *toEnd = fromEnd + 1;
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
                                    ALLOCATOR          allocator)
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
                         ALLOCATOR                                allocator,
                         bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *)
{
    size_type frontSize = position - fromBegin;
    Iterator  begin     = fromBegin;
    Iterator  dest      = begin - numElements;

    // In case of aliasing, make a copy of the value.
    bsls::ObjectBuffer<VALUE_TYPE> space;
    bslma::ConstructionUtil::construct(BSLS_UTIL_ADDRESSOF(space.object()),
                                       bslma::Default::allocator(),
                                       value);
    bslma::DestructorProctor<VALUE_TYPE>
                                     temp(BSLS_UTIL_ADDRESSOF(space.object()));

    // TBD: this does the same thing as the old code - don't like that we
    // circumvent the whole allocator thing, but for now, let's keep it
    // consistent.
    // ConstructorProxy<VALUE_TYPE>
    //                           tempValue(value, bslma::Default::allocator());

    // No guard needed since all the operations won't throw due to the
    // bitwise-copyable trait
    moveFront(&dest, &begin, frontSize);
    uninitializedFillNBack(&dest, dest, numElements, space.object(),
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
                          ALLOCATOR                                allocator,
                          bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *)
{
    typedef DequePrimitives_ExternalDequeElementGuard<VALUE_TYPE,
                                                      BLOCK_LENGTH,
                                                      ALLOCATOR>    ExtGuard;
    size_type frontSize = position - fromBegin;
    Iterator  begin     = fromBegin;
    Iterator  dest      = begin - numElements;

    // In case of aliasing, make a copy of the value.
    bsls::ObjectBuffer<VALUE_TYPE> space;
    bslma::ConstructionUtil::construct(BSLS_UTIL_ADDRESSOF(space.object()),
                                       bslma::Default::allocator(),
                                       value);
    bslma::DestructorProctor<VALUE_TYPE>
                                     temp(BSLS_UTIL_ADDRESSOF(space.object()));

    // TBD: this does the same thing as the old code - don't like that we
    // circumvent the whole allocator thing, but for now, let's keep it
    // consistent.
    // ConstructorProxy<VALUE_TYPE>
    //                           tempValue(value, bslma::Default::allocator());

    // Create a reverse guard that will reverse the moveFront operation in case
    // of an exception.
    MoveGuard guard(begin, dest, frontSize, true);

    moveFront(&dest, &begin, frontSize);

    // Create a guard for 'uninitializedFillNBack' because it can throw under
    // 'bitwisemoveable' trait.  Need to use this special guard because
    // uninitializedFillNBack is not exception safe.
    Iterator dest2(dest);
    ExtGuard eguard(&dest2, &dest, allocator);

    uninitializedFillNBack(&dest, dest, numElements, space.object(),
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
                                       ALLOCATOR                   allocator,
                                       bslmf::MetaInt<NIL_TRAITS> *)
{
    typedef DequePrimitives_DequeElementGuard<VALUE_TYPE,
                                              BLOCK_LENGTH,
                                              ALLOCATOR>    ElementGuard;

    size_type frontSize = position - fromBegin;
    Iterator  begin     = fromBegin;
    Iterator  dest      = begin - numElements;
    size_type numDest;

    // In case of aliasing, make a copy of the value.
    bsls::ObjectBuffer<VALUE_TYPE> space;
    bslma::ConstructionUtil::construct(BSLS_UTIL_ADDRESSOF(space.object()),
                                       bslma::Default::allocator(),
                                       value);
    bslma::DestructorProctor<VALUE_TYPE>
                                     temp(BSLS_UTIL_ADDRESSOF(space.object()));

    // TBD: this does the same thing as the old code - don't like that we
    // circumvent the whole allocator thing, but for now, let's keep it
    // consistent.
    // ConstructorProxy<VALUE_TYPE>
    //                           tempValue(value, bslma::Default::allocator());

    ElementGuard guard(dest, dest, allocator);
    if (frontSize >= numElements) {
        for (numDest = numElements; 0 < numDest; --numDest, ++dest, ++begin) {
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        *begin);
            guard.moveEnd(1);
        }
        for (frontSize -= numElements; 0 < frontSize;
                                                --frontSize, ++dest, ++begin) {
            *dest = *begin;
        }
        for (numDest = numElements; 0 < numDest; --numDest, ++dest) {
            *dest = space.object();
        }
    } else {
        for (numDest = frontSize; 0 < numDest; --numDest, ++dest, ++begin) {
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        *begin);
            guard.moveEnd(1);
        }
        for (numDest = numElements; frontSize < numDest; --numDest, ++dest) {
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        space.object());
            guard.moveEnd(1);
        }
        for (; 0 < numDest; --numDest, ++dest) {
            *dest = space.object();
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
                                                  ALLOCATOR  allocator)
{
    typedef DequePrimitives_DequeElementGuard<VALUE_TYPE,
                                              BLOCK_LENGTH,
                                              ALLOCATOR>    ElementGuard;

    size_type frontSize = position - fromBegin;
    Iterator  begin     = fromBegin;
    Iterator  dest      = begin - numElements;
    size_type numDest;

    ElementGuard guard(dest, dest, allocator);
    if (frontSize >= numElements) {
        for (numDest = numElements; 0 < numDest; --numDest, ++dest, ++begin) {
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        *begin);
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
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        *begin);
            guard.moveEnd(1);
        }
        for (numDest = numElements; frontSize < numDest;
                                                  --numDest, ++dest, ++first) {
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        *first);
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
template <class ALLOCATOR>
inline
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::moveInsertAndMoveToFront(
                                      Iterator                      *toBegin,
                                      Iterator                       fromBegin,
                                      Iterator                       position,
                                      bslmf::MovableRef<VALUE_TYPE>  value,
                                      ALLOCATOR                      allocator)
{
    enum {
        IS_BITWISECOPYABLE = bsl::is_trivially_copyable<VALUE_TYPE>::value,
        IS_BITWISEMOVEABLE = bslmf::IsBitwiseMoveable<VALUE_TYPE>::value,

        VALUE = IS_BITWISECOPYABLE
                ? BITWISE_COPYABLE_TRAITS : IS_BITWISEMOVEABLE
                ? BITWISE_MOVEABLE_TRAITS : NIL_TRAITS
    };

    VALUE_TYPE& lvalue = value;

    moveInsertAndMoveToFront(toBegin,
                             fromBegin,
                             position,
                             MoveUtil::move(lvalue),
                             allocator,
                             (bslmf::MetaInt<VALUE> *)0);
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::moveInsertAndMoveToFront(
                            Iterator                                *toBegin,
                            Iterator                                 fromBegin,
                            Iterator                                 position,
                            bslmf::MovableRef<VALUE_TYPE>            value,
                            ALLOCATOR                                allocator,
                            bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *)
{
    const size_type frontSize = position - fromBegin;
    Iterator        begin     = fromBegin;
    Iterator        dest      = begin - 1;

    // No guard needed since no operations will throw with
    // 'BITWISE_COPYABLE_TRAITS'.

    moveFront(&dest, &begin, frontSize);

    VALUE_TYPE& lvalue = value;
    bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                dest.valuePtr(),
                                                MoveUtil::move(lvalue));

    *toBegin = fromBegin - 1;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::moveInsertAndMoveToFront(
                            Iterator                                *toBegin,
                            Iterator                                 fromBegin,
                            Iterator                                 position,
                            bslmf::MovableRef<VALUE_TYPE>            value,
                            ALLOCATOR                                allocator,
                            bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *)
{
    const size_type frontSize = position - fromBegin;
    Iterator        begin     = fromBegin;
    Iterator        dest      = begin - 1;

    // Create a reverse guard that will undo the 'moveFront' operation in case
    // of an exception.

    MoveGuard guard(begin, dest, frontSize, true);

    moveFront(&dest, &begin, frontSize);

    VALUE_TYPE& lvalue = value;
    bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                dest.valuePtr(),
                                                MoveUtil::move(lvalue));

    guard.release();

    *toBegin = fromBegin - 1;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::moveInsertAndMoveToFront(
                                      Iterator                      *toBegin,
                                      Iterator                       fromBegin,
                                      Iterator                       position,
                                      bslmf::MovableRef<VALUE_TYPE>  value,
                                      ALLOCATOR                      allocator,
                                      bslmf::MetaInt<NIL_TRAITS>    *)
{
    typedef DequePrimitives_DequeElementGuard<VALUE_TYPE,
                                              BLOCK_LENGTH,
                                              ALLOCATOR>    ElementGuard;

    size_type frontSize = position - fromBegin;
    Iterator  begin     = fromBegin;
    Iterator  dest      = begin - 1;

    BSLS_ASSERT_SAFE(frontSize >= 1);

    ElementGuard guard(dest, dest, allocator);

    // 1. move-construct front-most element being bumped 1 slot

    bsl::allocator_traits<ALLOCATOR>::construct(
                                           allocator,
                                           dest.valuePtr(),
                                           MoveUtil::move_if_noexcept(*begin));
    guard.moveEnd(1);
    ++dest;
    ++begin;

    // 2. move-assign other existing elements being bumped forward 1 slot

    for (frontSize -= 1; 0 < frontSize; --frontSize, ++dest, ++begin) {
        *dest = MoveUtil::move_if_noexcept(*begin);
    }

    // 3. move-assign the new element

    VALUE_TYPE& lvalue = value;
    *dest = MoveUtil::move(lvalue);

    guard.release();

    *toBegin = fromBegin - 1;
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR, class... Args>
inline
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::emplaceAndMoveToBack(
                                                          Iterator  *toEnd,
                                                          Iterator   fromEnd,
                                                          Iterator   position,
                                                          ALLOCATOR  allocator,
                                                          Args&&...  arguments)
{
    enum {
        IS_BITWISECOPYABLE = bsl::is_trivially_copyable<VALUE_TYPE>::value,
        IS_BITWISEMOVEABLE = bslmf::IsBitwiseMoveable<VALUE_TYPE>::value,

        VALUE = IS_BITWISECOPYABLE
                ? BITWISE_COPYABLE_TRAITS : IS_BITWISEMOVEABLE
                ? BITWISE_MOVEABLE_TRAITS : NIL_TRAITS
    };

    emplaceAndMoveToBackDispatch(
                            toEnd,
                            fromEnd,
                            position,
                            allocator,
                            (bslmf::MetaInt<VALUE> *)0,
                            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR, class... Args>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::emplaceAndMoveToBackDispatch(
                            Iterator                                *toEnd,
                            Iterator                                 fromEnd,
                            Iterator                                 position,
                            ALLOCATOR                                allocator,
                            bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *,
                            Args&&...                                arguments)
{
    const size_type backSize = fromEnd - position;
    Iterator        end      = fromEnd;
    Iterator        dest     = end + 1;

    // No guard needed since no operations will throw with
    // 'BITWISE_COPYABLE_TRAITS'.

    moveBack(&dest, &end, backSize);
    --dest;

    bsl::allocator_traits<ALLOCATOR>::construct(
                            allocator,
                            dest.valuePtr(),
                            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);

    *toEnd = fromEnd + 1;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR, class... Args>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::emplaceAndMoveToBackDispatch(
                            Iterator                                *toEnd,
                            Iterator                                 fromEnd,
                            Iterator                                 position,
                            ALLOCATOR                                allocator,
                            bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *,
                            Args&&...                                arguments)
{
    const size_type backSize = fromEnd - position;
    Iterator        end      = fromEnd;
    Iterator        dest     = end + 1;

    // Create a reverse guard that will undo the 'moveBack' operation in case
    // of an exception.

    MoveGuard guard(end, dest, backSize, false);

    moveBack(&dest, &end, backSize);
    --dest;

    bsl::allocator_traits<ALLOCATOR>::construct(
                            allocator,
                            dest.valuePtr(),
                            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);

    guard.release();

    *toEnd = fromEnd + 1;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR, class... Args>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::emplaceAndMoveToBackDispatch(
                                         Iterator                   *toEnd,
                                         Iterator                    fromEnd,
                                         Iterator                    position,
                                         ALLOCATOR                   allocator,
                                         bslmf::MetaInt<NIL_TRAITS> *,
                                         Args&&...                   arguments)
{
    typedef DequePrimitives_DequeElementGuard<VALUE_TYPE,
                                              BLOCK_LENGTH,
                                              ALLOCATOR>       ElementGuard;

    typedef DequePrimitives_DequeEndpointProctor<VALUE_TYPE,
                                                 BLOCK_LENGTH> EndpointProctor;

    size_type backSize = fromEnd - position;
    Iterator  end      = fromEnd;
    Iterator  dest     = end + 1;

    BSLS_ASSERT_SAFE(backSize >= 1);

    ElementGuard guard(dest, dest, allocator);

    // 1. move-construct back-most element being bumped 1 slot

    --dest;
    --end;
    bsl::allocator_traits<ALLOCATOR>::construct(
                                             allocator,
                                             dest.valuePtr(),
                                             MoveUtil::move_if_noexcept(*end));
    guard.moveBegin(-1);

    // 2. move-assign other existing elements being bumped back 1 slot

    size_type n = 0;  // additional elements to guard following 'for' loop
    for (backSize -= 1; 0 < backSize; --backSize) {
        --dest;
        --end;
        *dest = MoveUtil::move_if_noexcept(*end);
        ++n;
    }
    guard.moveBegin(-n);  // in case the emplacement (below) throws

    // 3. destroy element in emplacement slot

    bsl::allocator_traits<ALLOCATOR>::destroy(allocator, position.valuePtr());

    // 4. emplace the new element

    // If emplacement throws, lop off the '[position .. *toEnd)' portion of the
    // deque because a hole was just created that cannot be repopulated.

    EndpointProctor endpointProctor(toEnd, position);

    bsl::allocator_traits<ALLOCATOR>::construct(
                            allocator,
                            position.valuePtr(),
                            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);

    endpointProctor.release();
    guard.release();

    *toEnd = fromEnd + 1;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR, class... Args>
inline
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::emplaceAndMoveToFront(
                                                          Iterator  *toBegin,
                                                          Iterator   fromBegin,
                                                          Iterator   position,
                                                          ALLOCATOR  allocator,
                                                          Args&&...  arguments)
{
    enum {
        IS_BITWISECOPYABLE = bsl::is_trivially_copyable<VALUE_TYPE>::value,
        IS_BITWISEMOVEABLE = bslmf::IsBitwiseMoveable<VALUE_TYPE>::value,

        VALUE = IS_BITWISECOPYABLE
                ? BITWISE_COPYABLE_TRAITS : IS_BITWISEMOVEABLE
                ? BITWISE_MOVEABLE_TRAITS : NIL_TRAITS
    };

    emplaceAndMoveToFrontDispatch(
                            toBegin,
                            fromBegin,
                            position,
                            allocator,
                            (bslmf::MetaInt<VALUE> *)0,
                            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR, class... Args>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::emplaceAndMoveToFrontDispatch(
                            Iterator                                *toBegin,
                            Iterator                                 fromBegin,
                            Iterator                                 position,
                            ALLOCATOR                                allocator,
                            bslmf::MetaInt<BITWISE_COPYABLE_TRAITS> *,
                            Args&&...                                arguments)
{
    const size_type frontSize = position - fromBegin;
    Iterator        begin     = fromBegin;
    Iterator        dest      = begin - 1;

    // No guard needed since no operations will throw with
    // 'BITWISE_COPYABLE_TRAITS'.

    moveFront(&dest, &begin, frontSize);

    bsl::allocator_traits<ALLOCATOR>::construct(
                            allocator,
                            dest.valuePtr(),
                            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);

    *toBegin = fromBegin - 1;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR, class... Args>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::emplaceAndMoveToFrontDispatch(
                            Iterator                                *toBegin,
                            Iterator                                 fromBegin,
                            Iterator                                 position,
                            ALLOCATOR                                allocator,
                            bslmf::MetaInt<BITWISE_MOVEABLE_TRAITS> *,
                            Args&&...                                arguments)
{
    const size_type frontSize = position - fromBegin;
    Iterator        begin     = fromBegin;
    Iterator        dest      = begin - 1;

    // Create a reverse guard that will undo the 'moveFront' operation in case
    // of an exception.

    MoveGuard guard(begin, dest, frontSize, true);

    moveFront(&dest, &begin, frontSize);

    bsl::allocator_traits<ALLOCATOR>::construct(
                            allocator,
                            dest.valuePtr(),
                            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);

    guard.release();

    *toBegin = fromBegin - 1;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
template <class ALLOCATOR, class... Args>
void
DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::emplaceAndMoveToFrontDispatch(
                                         Iterator                   *toBegin,
                                         Iterator                    fromBegin,
                                         Iterator                    position,
                                         ALLOCATOR                   allocator,
                                         bslmf::MetaInt<NIL_TRAITS> *,
                                         Args&&...                   arguments)
{
    typedef DequePrimitives_DequeElementGuard<VALUE_TYPE,
                                              BLOCK_LENGTH,
                                              ALLOCATOR>       ElementGuard;

    typedef DequePrimitives_DequeEndpointProctor<VALUE_TYPE,
                                                 BLOCK_LENGTH> EndpointProctor;

    size_type frontSize = position - fromBegin;
    Iterator  begin     = fromBegin;
    Iterator  dest      = begin - 1;

    BSLS_ASSERT_SAFE(frontSize >= 1);

    ElementGuard guard(dest, dest, allocator);

    // 1. move-construct front-most element being bumped 1 slot

    bsl::allocator_traits<ALLOCATOR>::construct(
                                           allocator,
                                           dest.valuePtr(),
                                           MoveUtil::move_if_noexcept(*begin));
    guard.moveEnd(1);
    ++dest;
    ++begin;

    // 2. move-assign other existing elements being bumped forward 1 slot

    size_type n = 0;  // additional elements to guard following 'for' loop
    for (frontSize -= 1; 0 < frontSize; --frontSize, ++dest, ++begin) {
        *dest = MoveUtil::move_if_noexcept(*begin);
        ++n;
    }
    guard.moveEnd(n);  // in case the emplacement (below) throws

    // 3. destroy element in emplacement slot

    bsl::allocator_traits<ALLOCATOR>::destroy(allocator, dest.valuePtr());

    // 4. emplace the new element

    // If emplacement throws, lop off the '[*toBegin .. position)' portion the
    // the deque because a hole was just created that cannot be repopulated.

    EndpointProctor endpointProctor(toBegin, position);

    bsl::allocator_traits<ALLOCATOR>::construct(
                            allocator,
                            dest.valuePtr(),
                            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);

    endpointProctor.release();
    guard.release();

    *toBegin = fromBegin - 1;
}
#endif

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
        std::memmove((void *)destination->valuePtr(),
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

    std::memmove((void *)destination->valuePtr(),
                 source->valuePtr(),
                 firstSegment * sizeof(VALUE_TYPE));

    for (; numElements >= BLOCK_LENGTH; numElements -= BLOCK_LENGTH) {

        *destination -= secondSegment;
        *source      -= secondSegment;

        std::memmove((void *)destination->valuePtr(),
                     source->valuePtr(),
                     secondSegment * sizeof(VALUE_TYPE));

        *destination -= thirdSegment;
        *source      -= thirdSegment;

        std::memmove((void *)destination->valuePtr(),
                     source->valuePtr(),
                     thirdSegment * sizeof(VALUE_TYPE));
    }

    size_type remaining = numElements > secondSegment
                        ? secondSegment
                        : numElements;

    *destination -= remaining;
    *source      -= remaining;
    numElements  -= remaining;

    std::memmove((void *)destination->valuePtr(),
                 source->valuePtr(),
                 remaining * sizeof(VALUE_TYPE));

    *destination -= numElements;
    *source      -= numElements;

    std::memmove((void *)destination->valuePtr(),
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

        std::memmove((void *)destination->valuePtr(),
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

    std::memmove((void *)destination->valuePtr(),
                 source->valuePtr(),
                 firstSegment * sizeof(VALUE_TYPE));

    *destination += firstSegment;
    *source      += firstSegment;
    numElements  -= firstSegment;

    for (; numElements >= BLOCK_LENGTH; numElements -= BLOCK_LENGTH) {

        std::memmove((void *)destination->valuePtr(),
                     source->valuePtr(),
                     secondSegment * sizeof(VALUE_TYPE));

        *destination += secondSegment;
        *source      += secondSegment;

        std::memmove((void *)destination->valuePtr(),
                     source->valuePtr(),
                     thirdSegment * sizeof(VALUE_TYPE));

        *destination += thirdSegment;
        *source      += thirdSegment;
    }

    size_type remaining = numElements > secondSegment
                        ? secondSegment
                        : numElements;

    std::memmove((void *)destination->valuePtr(),
                 source->valuePtr(),
                 remaining * sizeof(VALUE_TYPE));

    *destination += remaining;
    *source      += remaining;
    numElements  -= remaining;

    std::memmove((void *)destination->valuePtr(),
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
                                    ALLOCATOR          allocator)
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
    ::valueInititalizeN(Iterator  *toEnd,
                        Iterator   fromEnd,
                        size_type  numElements,
                        ALLOCATOR  allocator)
{
    if (fromEnd.remainingInBlock() > numElements) {
        ArrayPrimitives::defaultConstruct(fromEnd.valuePtr(),
                                          numElements,
                                          allocator);
        fromEnd += numElements;
        *toEnd   = fromEnd;
        return;                                                       // RETURN
    }

    size_type firstRemaining = fromEnd.remainingInBlock();

    ArrayPrimitives::defaultConstruct(fromEnd.valuePtr(),
                                      firstRemaining,
                                      allocator);

    numElements -= firstRemaining;
    fromEnd     += firstRemaining;
    *toEnd       = fromEnd;

    for ( ; numElements >= BLOCK_LENGTH; numElements -= BLOCK_LENGTH) {
        ArrayPrimitives::defaultConstruct(fromEnd.valuePtr(),
                                          BLOCK_LENGTH,
                                          allocator);
        fromEnd.nextBlock();
        toEnd->nextBlock();
    }

    ArrayPrimitives::defaultConstruct(fromEnd.valuePtr(),
                                      numElements,
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
                                    ALLOCATOR          allocator)
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
                                      ALLOCATOR  allocator)
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
    destruct(first, last, allocator);
    return ret;
}

template <class VALUE_TYPE>
template <class ALLOCATOR>
void DequePrimitives<VALUE_TYPE, 1>::destruct(Iterator   begin,
                                              Iterator   end,
                                              ALLOCATOR  allocator)
{
    for (; !(begin == end); ++begin) {
        bsl::allocator_traits<ALLOCATOR>::destroy(allocator,
                                                  begin.valuePtr());
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
                                               ALLOCATOR          allocator)
{
    typedef DequePrimitives_DequeElementGuard<VALUE_TYPE,
                                              1,
                                              ALLOCATOR>  ElementGuard;

    size_type frontSize = position - fromBegin;
    Iterator  begin     = fromBegin;
    Iterator  dest      = begin - numElements;
    size_type numDest;

    bsls::ObjectBuffer<VALUE_TYPE> space;
    bslma::ConstructionUtil::construct(BSLS_UTIL_ADDRESSOF(space.object()),
                                       bslma::Default::allocator(),
                                       value);
    bslma::DestructorProctor<VALUE_TYPE>
                                     temp(BSLS_UTIL_ADDRESSOF(space.object()));

    // TBD: this does the same thing as the old code - don't like that we
    // circumvent the whole allocator thing, but for now, let's keep it
    // consistent.
    // ConstructorProxy<VALUE_TYPE>
    //                           tempValue(value, bslma::Default::allocator());

    ElementGuard guard(dest, dest, allocator);
    if (frontSize >= numElements) {
        for (numDest = numElements; 0 < numDest; --numDest, ++dest, ++begin) {
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        *begin);
            guard.moveEnd(1);
        }
        for (frontSize -= numElements; 0 < frontSize;
                                                --frontSize, ++dest, ++begin) {
            *dest = *begin;
        }
        for (numDest = numElements; 0 < numDest; --numDest, ++dest) {
            *dest = space.object();
        }
    } else {
        for (numDest = frontSize; 0 < numDest; --numDest, ++dest, ++begin) {
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        *begin);
            guard.moveEnd(1);
        }
        for (numDest = numElements; frontSize < numDest; --numDest, ++dest) {
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        space.object());
            guard.moveEnd(1);
        }
        for (; 0 < numDest; --numDest, ++dest) {
            *dest = space.object();
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
                                                  ALLOCATOR  allocator)
{
    typedef DequePrimitives_DequeElementGuard<VALUE_TYPE,
                                              1,
                                              ALLOCATOR>  ElementGuard;

    size_type frontSize = position - fromBegin;
    Iterator  begin     = fromBegin;
    Iterator  dest      = begin - numElements;
    size_type numDest;

    ElementGuard guard(dest, dest, allocator);
    if (frontSize >= numElements) {
        for (numDest = numElements; 0 < numDest; --numDest, ++dest, ++begin) {
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        *begin);
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
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        *begin);
            guard.moveEnd(1);
        }
        for (numDest = numElements; frontSize < numDest;
                                                  --numDest, ++dest, ++first) {
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        *first);
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
                                                ALLOCATOR          allocator)
{
    typedef DequePrimitives_DequeElementGuard<VALUE_TYPE,
                                              1,
                                              ALLOCATOR>  ElementGuard;

    size_type backSize = fromEnd - position;
    Iterator  end      = fromEnd;
    Iterator  dest     = end + numElements;
    size_type numDest;

    bsls::ObjectBuffer<VALUE_TYPE> space;
    bslma::ConstructionUtil::construct(BSLS_UTIL_ADDRESSOF(space.object()),
                                       bslma::Default::allocator(),
                                       value);
    bslma::DestructorProctor<VALUE_TYPE>
                                     temp(BSLS_UTIL_ADDRESSOF(space.object()));

    // TBD: this does the same thing as the old code - don't like that we
    // circumvent the whole allocator thing, but for now, let's keep it
    // consistent.
    // ConstructorProxy<VALUE_TYPE>
    //                           tempValue(value, bslma::Default::allocator());

    ElementGuard guard(dest, dest, allocator);
    if (backSize >= numElements) {
        for (numDest = numElements; 0 < numDest; --numDest) {
            --dest;
            --end;
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        *end);
            guard.moveBegin(-1);
        }
        for (backSize -= numElements; 0 < backSize; --backSize) {
            --dest;
            --end;
            *dest = *end;
        }
        for (numDest = numElements; 0 < numDest; --numDest, ++position) {
            *position = space.object();
        }
    } else {
        for (numDest = backSize; 0 < numDest; --numDest) {
            --dest;
            --end;
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        *end);
            guard.moveBegin(-1);
        }
        for (numDest = numElements; backSize < numDest; --numDest) {
            --dest;
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        space.object());
            guard.moveBegin(-1);
        }
        for (; 0 < numDest; --numDest, ++position) {
            *position = space.object();
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
                                                    ALLOCATOR  allocator)
{
    typedef DequePrimitives_DequeElementGuard<VALUE_TYPE,
                                              1,
                                              ALLOCATOR>  ElementGuard;

    size_type backSize = fromEnd - position;
    Iterator  end      = fromEnd;
    Iterator  dest     = end + numElements;
    size_type numDest;

    ElementGuard guard(dest, dest, allocator);
    if (backSize >= numElements) {
        for (numDest = numElements; 0 < numDest; --numDest) {
            --dest;
            --end;
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        *end);
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
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        dest.valuePtr(),
                                                        *end);
            guard.moveBegin(-1);
        }
        for (numDest = backSize; 0 < numDest; --numDest, ++position, ++first) {
            *position = *first;
        }
        ElementGuard guard2(position, position, allocator);
        for (numDest = numElements; backSize < numDest; ++first, ++position,
                                                                   --numDest) {
            bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                        position.valuePtr(),
                                                        *first);
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
                                                ALLOCATOR          allocator)
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
                                       ALLOCATOR                   allocator,
                                       bslmf::MetaInt<NIL_TRAITS> *)
{
    typedef DequePrimitives_DequeElementGuard<VALUE_TYPE,
                                              1,
                                              ALLOCATOR>  ElementGuard;

    ElementGuard guard(fromBegin, fromBegin, allocator);
    for (; 0 < numElements; --numElements) {
        --fromBegin;
        bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                    fromBegin.valuePtr(),
                                                    value);
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
                                   ALLOCATOR                       allocator,
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
                                                ALLOCATOR          allocator)
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
                                       ALLOCATOR                   allocator,
                                       bslmf::MetaInt<NIL_TRAITS> *)
{
    typedef DequePrimitives_DequeElementGuard<VALUE_TYPE,
                                              1,
                                              ALLOCATOR>  ElementGuard;

    ElementGuard guard(fromEnd, fromEnd, allocator);
    for (; 0 < numElements; --numElements) {
        bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                    fromEnd.valuePtr(),
                                                    value);
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
                                   ALLOCATOR                       allocator,
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

template <class VALUE_TYPE>
template <class ALLOCATOR>
inline
void
DequePrimitives<VALUE_TYPE, 1>::valueInititalizeN(Iterator  *toEnd,
                                                  Iterator   fromEnd,
                                                  size_type  numElements,
                                                  ALLOCATOR  allocator)
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

    valueInititalizeN(toEnd,
                      fromEnd,
                      numElements,
                      allocator,
                      (bslmf::MetaInt<VALUE>*)0);
}

template <class VALUE_TYPE>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, 1>::valueInititalizeN(Iterator  *toEnd,
                                                  Iterator   fromEnd,
                                                  size_type  numElements,
                                                  ALLOCATOR  allocator,
                                                  bslmf::MetaInt<NIL_TRAITS> *)
{
    typedef DequePrimitives_DequeElementGuard<VALUE_TYPE,
                                              1,
                                              ALLOCATOR>  ElementGuard;

    ElementGuard guard(fromEnd, fromEnd, allocator);
    for (; 0 < numElements; --numElements) {
        bsl::allocator_traits<ALLOCATOR>::construct(allocator,
                                                    fromEnd.valuePtr());
        ++fromEnd;
        guard.moveEnd(1);
    }
    guard.release();
    *toEnd = fromEnd;
}

template <class VALUE_TYPE>
template <class ALLOCATOR>
void
DequePrimitives<VALUE_TYPE, 1>::valueInititalizeN(
                                              Iterator  *toEnd,
                                              Iterator   fromEnd,
                                              size_type  numElements,
                                              ALLOCATOR  allocator,
                                              bslmf::MetaInt<NON_NIL_TRAITS> *)
{
    *toEnd = fromEnd;  // necessary in case 'numElements = 0'
    for ( ; 0 < numElements; --numElements) {
        ArrayPrimitives::defaultConstruct(fromEnd.valuePtr(),
                                          1,
                                          allocator);
        ++fromEnd;
        *toEnd = fromEnd;
    }
}

               // ---------------------------------------
               // class DequePrimitives_DequeElementGuard
               // ---------------------------------------

// CREATORS
template <class VALUE_TYPE, int BLOCK_LENGTH, class ALLOCATOR>
inline
DequePrimitives_DequeElementGuard<VALUE_TYPE, BLOCK_LENGTH, ALLOCATOR>
              ::DequePrimitives_DequeElementGuard(const Iterator& begin,
                                                  const Iterator& end,
                                                  ALLOCATOR       allocator)
: d_begin(begin)
, d_end(end)
, d_allocator(allocator)
{
}

template <class VALUE_TYPE, int BLOCK_LENGTH, class ALLOCATOR>
inline
DequePrimitives_DequeElementGuard<VALUE_TYPE, BLOCK_LENGTH, ALLOCATOR>
                                  ::~DequePrimitives_DequeElementGuard()
{
    DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::destruct(d_begin,
                                                        d_end,
                                                        d_allocator);
}

// MANIPULATORS
template <class VALUE_TYPE, int BLOCK_LENGTH, class ALLOCATOR>
inline
typename DequePrimitives_DequeElementGuard
<VALUE_TYPE, BLOCK_LENGTH, ALLOCATOR>::Iterator&
DequePrimitives_DequeElementGuard<VALUE_TYPE, BLOCK_LENGTH, ALLOCATOR>
                                             ::moveBegin(std::ptrdiff_t offset)
{
    d_begin += offset;
    return d_begin;
}

template <class VALUE_TYPE, int BLOCK_LENGTH, class ALLOCATOR>
inline
typename DequePrimitives_DequeElementGuard
<VALUE_TYPE, BLOCK_LENGTH, ALLOCATOR>::Iterator&
DequePrimitives_DequeElementGuard<VALUE_TYPE, BLOCK_LENGTH, ALLOCATOR>
                                               ::moveEnd(std::ptrdiff_t offset)
{
    d_end += offset;
    return d_end;
}

template <class VALUE_TYPE, int BLOCK_LENGTH, class ALLOCATOR>
inline
void DequePrimitives_DequeElementGuard<VALUE_TYPE, BLOCK_LENGTH, ALLOCATOR>::
                                                                      release()
{
    d_begin = d_end;
}

            // -----------------------------------------------
            // class DequePrimitives_ExternalDequeElementGuard
            // -----------------------------------------------

// CREATORS
template <class VALUE_TYPE, int BLOCK_LENGTH, class ALLOCATOR>
inline
DequePrimitives_ExternalDequeElementGuard<VALUE_TYPE, BLOCK_LENGTH, ALLOCATOR>
            ::DequePrimitives_ExternalDequeElementGuard(Iterator  *begin,
                                                        Iterator  *end,
                                                        ALLOCATOR  allocator)
: d_begin_p(begin)
, d_end_p(end)
, d_allocator(allocator)
{
}

template <class VALUE_TYPE, int BLOCK_LENGTH, class ALLOCATOR>
inline
DequePrimitives_ExternalDequeElementGuard<VALUE_TYPE, BLOCK_LENGTH, ALLOCATOR>
                          ::~DequePrimitives_ExternalDequeElementGuard()
{
    if (d_begin_p != d_end_p) {
        DequePrimitives<VALUE_TYPE, BLOCK_LENGTH>::destruct(*d_begin_p,
                                                            *d_end_p,
                                                            d_allocator);
    }
}

// MANIPULATORS
template <class VALUE_TYPE, int BLOCK_LENGTH, class ALLOCATOR>
inline
void
DequePrimitives_ExternalDequeElementGuard<VALUE_TYPE, BLOCK_LENGTH, ALLOCATOR>
                                                                    ::release()
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
            DequePrimitives::moveBack(&d_destination_p, &d_source_p, d_size);
        }
        // We need to reverse 'moveBack'.
        else {
            d_destination_p -= d_size;
            d_source_p      -= d_size;
            DequePrimitives::moveFront(&d_destination_p, &d_source_p, d_size);
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

                // ------------------------------------------
                // class DequePrimitives_DequeEndpointProctor
                // ------------------------------------------

// CREATORS
template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
DequePrimitives_DequeEndpointProctor<VALUE_TYPE, BLOCK_LENGTH>
                     ::DequePrimitives_DequeEndpointProctor(Iterator *endpoint,
                                                            Iterator  position)
: d_endpoint_p(endpoint)
, d_position(position)
{
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
DequePrimitives_DequeEndpointProctor<VALUE_TYPE, BLOCK_LENGTH>
                                      ::~DequePrimitives_DequeEndpointProctor()
{
    if (0 != d_endpoint_p) {
        *d_endpoint_p = d_position;
    }
}

// MANIPULATORS
template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
void DequePrimitives_DequeEndpointProctor<VALUE_TYPE, BLOCK_LENGTH>::release()
{
    d_endpoint_p = 0;
}

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslalg_DequePrimitives
#undef bslalg_DequePrimitives
#endif
#define bslalg_DequePrimitives bslalg::DequePrimitives
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

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
