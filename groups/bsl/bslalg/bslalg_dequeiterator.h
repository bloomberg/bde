// bslalg_dequeiterator.h                                             -*-C++-*-
#ifndef INCLUDED_BSLALG_DEQUEITERATOR
#define INCLUDED_BSLALG_DEQUEITERATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a primitive iterator over deque data structures.
//
//@CLASSES:
//  bslalg::DequeIterator: primitive iterator over a deque data structure
//
//@SEE_ALSO: bslalg_dequeimputil, bslalg_dequeprimitives
//
//@DESCRIPTION: This component provides an in-core value semantic class,
// 'bslalg::DequeIterator', that is a primitive iterator type for enumerating
// elements in a deque (implemented in the form of a dynamic array) knowing
// only its value type and a nominal block size.  Conceptually, a deque is an
// array of block pointers, each block capable of containing a fixed number of
// objects.  An element in the deque is identified by an iterator that consists
// of two pointers:
//: o a pointer to the block pointer array, and
//: o a pointer to a value within the block referred to by the first pointer.
//
// Dereferencing the iterator dereferences the second pointer.  Incrementing or
// decrementing the iterator consists of incrementing the value pointer, unless
// the iterator crosses a block boundary in which case it must increment or
// decrement its pointer to the block pointer.  Computing the distance between
// two iterators involves figuring out how many block are in between and how
// the offsets-in-block differ.
//
// Note that an iterator is valid as long as the element it points to still
// belongs to the deque *and* there is no reallocation of the block pointer
// array.  Inserting elements at either end of the deque usually maintains
// iterator validity, but inserting enough elements at the end of the queue
// might force the creating of sufficiently many blocks to trigger a
// reallocation of the block pointer array and invalidate all iterators into
// the deque; how many depends on the distances between the front and back of
// the deque and the first/last iterator in the block pointer array (19 in the
// picture below).
//
// The picture is as follows:
//..
//                       v--- Iterator to 'I': ptr to this BlockPtr
//  +-----+-----+-----+-----+-----+-----+-----+-----+
//  |  *  |  *  |  *  |  *  |  *  |  *  |  *  |  *  |    BlockPtr array
//  +-----+-----+--|--+--|--+--|--+--|--+-----+-----+
//                 |     |     |     |                  Block
//                 |     |     |     |  +---+---+---+---+---+---+---+---+
//                 |     |     |     `--| V | W | X | Y | Z |   |   |   |
//                 |     |     |        +---+---+---+---+---+---+---+---+
//                 |     |     |                  Block
//                 |     |     |  +---+---+---+---+---+---+---+---+
//                 |     |     `--| N | O | P | Q | R | S | T | U |
//                 |     |        +---+---+---+---+---+---+---+---+
//                 |     |                v---- Iterator to 'I': ptr to value
//                 |     |  +---+---+---+---+---+---+---+---+
//                 |     `--| F | G | H | I | J | K | L | M |
//                 |        +---+---+---+---+---+---+---+---+
//                 |                  Block
//                 |  +---+---+---+---+---+---+---+---+
//                 `--|   |   |   | A | B | C | D | E |
//                    +---+---+---+---+---+---+---+---+
//..
// Depicted above is a deque consisting of eight block pointers, only four
// actually used to point to blocks of eight elements.  In the first block, the
// first three elements are uninitialized, and the twenty six elements follow
// in sequence across the different blocks.  An iterator to the 'I' element
// consists of a pointer to the fourth block pointer and a pointer to the sixth
// element of that block.  The value of the corresponding deque would be
// '[ A, B, C, ... X, Y, Z ]', its logical length 26, and its capacity would be
// 19 (the minimum number of prepend/append to force a reallocation of the
// block pointer array).
//
// This component does not provide the full interface of a C++ standard library
// iterator as we do not want a dependency on 'iterator_traits' in a package
// below 'bslstl'.  'bslalg::DequeIterator' provides the minimal necessary set
// of features to implement such an iterator for a standard conforming 'deque'
// implementation in a higher level component.
//
///Usage
///-----
// This component is for use by the 'bslstl' package.  Other clients should use
// the STL deque (in header '<deque>').

#include <bslscm_version.h>

#include <bslalg_dequeimputil.h>

#include <bslmf_istriviallycopyable.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>

#include <cstddef>  // std::size_t, std::ptrdiff_t

namespace BloombergLP {

namespace bslalg {

template <class VALUE_TYPE, int BLOCK_LENGTH>
class DequeIterator;

template <class VALUE_TYPE>
class DequeIterator<VALUE_TYPE, 1>;

                       // ===================
                       // class DequeIterator
                       // ===================

template <class VALUE_TYPE, int BLOCK_LENGTH>
class DequeIterator {
    // Implementation of a deque iterator, parameterized by the 'VALUE_TYPE',
    // for a deque with the parameterized 'BLOCK_LENGTH', and suitable for use
    // by the 'bslstl::RandomAccessIterator' adapter.  Note that 'BLOCK_LENGTH'
    // is the number of items of 'VALUE_TYPE' within a block, not the size of a
    // block in bytes.

    // PRIVATE TYPES
    typedef bslalg::DequeImpUtil<VALUE_TYPE, BLOCK_LENGTH>  DequeImpUtil;
    typedef typename DequeImpUtil::BlockPtr                 BlockPtr;
    typedef DequeIterator<VALUE_TYPE, BLOCK_LENGTH>         IteratorType;

    // DATA
    BlockPtr   *d_blockPtr_p;
    VALUE_TYPE *d_value_p;

    // FRIENDS
    friend bool operator==(const DequeIterator& lhs, const DequeIterator& rhs)
        // Return 'true' if the specified 'lhs' iterator points to the same
        // element in the same block as the specified 'rhs' iterator, and
        // 'false' otherwise.  The behavior is undefined unless 'lhs' and 'rhs'
        // are iterators over the same deque.  Note that this friend is a
        // regular functon, not a function template, so there is no way to
        // declare it outside the class in order to provide the definition.
    {
        return lhs.d_value_p == rhs.d_value_p;
    }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
    friend auto operator<=>(const DequeIterator& lhs, const DequeIterator& rhs)
        // Perform a three-way comparison between the specified 'lhs' and 'rhs'
        // iterators.  The behavior is undefined unless 'lhs' and 'rhs' are
        // iterators over the same deque.
    {
        auto result = lhs.d_blockPtr_p <=> rhs.d_blockPtr_p;
        return result == 0 ? lhs.d_value_p <=> rhs.d_value_p
                           : result;
    }
#else
    friend bool operator!=(const DequeIterator& lhs, const DequeIterator& rhs)
        // Return 'true' if the specified 'lhs' iterator points to a different
        // element in the same block as the specified 'rhs' iterator, or points
        // to an element in a different block to the 'rhs' iterator, and
        // 'false' otherwise.  The behavior is undefined unless 'lhs' and 'rhs'
        // are iterators over the same deque.  Note that this friend is a
        // regular functon, not a function template, so there is no way to
        // declare it outside the class in order to provide the definition.
    {
        return lhs.d_value_p != rhs.d_value_p;
    }

    friend bool operator<(const DequeIterator& lhs, const DequeIterator& rhs)
        // Return 'true' if the specified 'lhs' iterator points to an element
        // in a previous block or in a previous position in the same block as
        // the specified 'rhs' iterator, and 'false' otherwise.  The behavior
        // is undefined unless 'lhs' and 'rhs' are iterators over the same
        // deque.  Note that this friend is a regular functon, not a function
        // template, so there is no way to declare it outside the class in
        // order to provide the definition.
    {
        if (lhs.d_blockPtr_p == rhs.d_blockPtr_p) {
            return lhs.d_value_p < rhs.d_value_p;                     // RETURN
        }
        else {
            return lhs.d_blockPtr_p < rhs.d_blockPtr_p;               // RETURN
        }
    }
#endif

  public:
    // CREATORS
    DequeIterator();
        // Create a singular iterator (i.e., having internal null pointers).

    explicit
    DequeIterator(BlockPtr *blockPtrPtr);
        // Create an iterator pointing to the first element in the block
        // pointed to by the specified 'blockPtrPtr'.

    DequeIterator(BlockPtr *blockPtrPtr, VALUE_TYPE *valuePtr);
        // Create an iterator pointing to the element at the specified
        // 'valuePtr' address in the block pointed to by the specified
        // 'blockPtrPtr'.  The behavior is undefined unless 'valuePtr' points
        // into the block '*blockPtrPtr'.

    // MANIPULATORS
    void operator++();
        // Increment this iterator to point to the next element in the
        // corresponding deque (i.e., the element following the current one in
        // the same block or, if the current element is the last one in the
        // block, the first element in the next block).

    void operator--();
        // Decrement this iterator to point to the previous element in the
        // corresponding deque (i.e., the element preceding the current one in
        // the same block or, if the current element is the first one in the
        // block, the last element in the previous block).

    void operator+=(std::ptrdiff_t offset);
        // Advance this iterator by the specified 'offset'.

    void operator-=(std::ptrdiff_t offset);
        // Move this iterator backward by the specified 'offset'.

    void nextBlock();
        // Set this iterator to point to the first element of the next block.

    void previousBlock();
        // Set this iterator to point to the first (not the last) element of
        // the previous block.

    void setBlock(BlockPtr *blockPtrPtr);
        // Set this iterator to point to the first element of the block pointed
        // to by the specified 'blockPtrPtr'.

    void valuePtrDecrement();
        // Decrement this iterator to point to the next element in the block of
        // the corresponding deque.  The behavior is undefined unless this
        // iterator is pointed to a valid position of the deque.  Note that
        // this method is used only for optimization purposes in
        // 'bslstl_Deque', and clients of this package should not use this
        // directly.

    void valuePtrIncrement();
        // Increment this iterator to point to the next element in the block of
        // the corresponding deque.  The behavior is undefined unless this
        // iterator is pointed to a valid position of the deque.  Note that
        // this method is used only for optimization purposes in
        // 'bslstl_Deque', and clients of this package should not use this
        // directly.

    // ACCESSORS
    VALUE_TYPE& operator*() const;
        // Return a reference to the parameterized 'VALUE_TYPE' object pointed
        // to by this iterator.  Note that this value is modifiable if
        // 'VALUE_TYPE' is modifiable, and non-modifiable if it is not.

    DequeIterator operator+(std::ptrdiff_t offset) const;
        // Return an iterator pointing the element at the specified 'offset'
        // after this iterator.

    DequeIterator operator-(std::ptrdiff_t offset) const;
        // Return an iterator pointing the element at the specified 'offset'
        // before this iterator.

    std::ptrdiff_t operator-(const DequeIterator& rhs) const;
        // Return the distance between this iterator and the specified 'rhs'
        // iterator.

    VALUE_TYPE *blockBegin() const;
        // Return the address of the first element in the block pointed to by
        // this iterator.

    VALUE_TYPE *blockEnd() const;
        // Return the address of (one-past) the last element in the block
        // pointed to by this iterator.

    BlockPtr *blockPtr() const;
        // Return the address of the block pointer pointed to by this iterator.

    std::size_t offsetInBlock() const;
        // Return the offset of the element pointed to by this iterator, from
        // the beginning of the block containing it.

    std::size_t remainingInBlock() const;
        // Return the number of elements in the block pointed to by this
        // iterator, until the end of this block, starting at (and including)
        // the element pointed to by this iterator.

    VALUE_TYPE *valuePtr() const;
        // Return the address of the parameterized 'VALUE_TYPE' object pointed
        // to by this iterator.
};

// PARTIAL SPECIALIZATION
template <class VALUE_TYPE>
class DequeIterator<VALUE_TYPE, 1> {
    // This partial specialization of 'DequeIterator' for the case when there
    // is a single element per block uses simpler storage and a simpler
    // implementation.  The contract for all functions is the same, and so not
    // repeated.

    // PRIVATE TYPES
    typedef bslalg::DequeImpUtil<VALUE_TYPE, 1> DequeImpUtil;
    typedef typename DequeImpUtil::BlockPtr     BlockPtr;
    typedef DequeIterator<VALUE_TYPE, 1>        IteratorType;

    // DATA
    BlockPtr   *d_blockPtr_p; // pointer to BlockPtr within BlockPtr array
    VALUE_TYPE *d_value_p;    // pointer to element referenced by iterator

    // FRIENDS
    friend bool operator==(const DequeIterator& lhs, const DequeIterator& rhs)
    {
        return lhs.d_blockPtr_p == rhs.d_blockPtr_p;
    }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
    friend auto operator<=>(const DequeIterator& lhs, const DequeIterator& rhs)
    {
        return lhs.d_blockPtr_p <=> rhs.d_blockPtr_p;
    }
#else
    friend bool operator!=(const DequeIterator& lhs, const DequeIterator& rhs)
    {
        return lhs.d_blockPtr_p != rhs.d_blockPtr_p;
    }

    friend bool operator<(const DequeIterator& lhs, const DequeIterator& rhs)
    {
        return lhs.d_blockPtr_p < rhs.d_blockPtr_p;
    }
#endif

  public:
    // CREATORS
    DequeIterator();
    DequeIterator(BlockPtr *blockPtrPtr);
    DequeIterator(BlockPtr *blockPtrPtr, VALUE_TYPE *valuePtr);

    // MANIPULATORS
    void operator++();
    void operator--();
    void operator+=(std::ptrdiff_t offset);
    void operator-=(std::ptrdiff_t offset);

    void nextBlock();
    void previousBlock();
    void setBlock(BlockPtr *blockPtrPtr);
    void valuePtrDecrement();
    void valuePtrIncrement();

    // ACCESSORS
    VALUE_TYPE& operator*() const;
    DequeIterator operator+(std::ptrdiff_t offset) const;
    DequeIterator operator-(std::ptrdiff_t offset) const;
    std::ptrdiff_t operator-(const DequeIterator& rhs) const;
    VALUE_TYPE *blockBegin() const;
    VALUE_TYPE *blockEnd() const;
    BlockPtr   *blockPtr() const;
    std::size_t offsetInBlock() const;
    std::size_t remainingInBlock() const;
    VALUE_TYPE *valuePtr() const;
};

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                         // -------------------
                         // class DequeIterator
                         // -------------------

// CREATORS
template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::DequeIterator()
: d_blockPtr_p(0)
, d_value_p(0)
{
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::DequeIterator(BlockPtr *blockPtrPtr)
: d_blockPtr_p(blockPtrPtr)
, d_value_p(reinterpret_cast<VALUE_TYPE*>(*blockPtrPtr))
{
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::DequeIterator(BlockPtr   *blockPtrPtr,
                                                       VALUE_TYPE *valuePtr)
: d_blockPtr_p(blockPtrPtr)
, d_value_p(valuePtr)
{
    // Trivially true, or undefined behavior, without the cast.
    BSLS_ASSERT_SAFE(
          reinterpret_cast<bsls::Types::UintPtr>((void*)blockPtrPtr[0]->d_data)
       <= reinterpret_cast<bsls::Types::UintPtr>((void*)valuePtr));
    BSLS_ASSERT_SAFE(valuePtr - blockPtrPtr[0]->d_data < BLOCK_LENGTH);
}

// MANIPULATORS
template <class VALUE_TYPE, int BLOCK_LENGTH>
void DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::operator++()
{
    if (1 == this->remainingInBlock()) {
        ++d_blockPtr_p;
        d_value_p = this->blockBegin();
    }
    else {
        ++d_value_p;
    }
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
void DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::operator--()
{
    if (0 == this->offsetInBlock()) {
        --d_blockPtr_p;
        d_value_p = this->blockEnd();
    }
    --d_value_p;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
void
DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::operator+=(std::ptrdiff_t offset)
{
    offset += offsetInBlock();
    if (offset >= 0) {
        d_blockPtr_p += offset / BLOCK_LENGTH;
        d_value_p = blockBegin() + (offset % BLOCK_LENGTH);
    }
    else {
        d_blockPtr_p -= (-offset - 1) / BLOCK_LENGTH + 1;
        d_value_p = blockEnd() - ((-offset - 1) % BLOCK_LENGTH + 1);
    }
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
void
DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::operator-=(std::ptrdiff_t offset)
{
    this->operator+=(-offset);
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
void DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::nextBlock()
{
    ++d_blockPtr_p;
    d_value_p = this->blockBegin();
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
void DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::previousBlock()
{
    --d_blockPtr_p;
    d_value_p = this->blockBegin();
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
void DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::setBlock(BlockPtr *blockPtrPtr)
{
    d_blockPtr_p = blockPtrPtr;
    d_value_p    = this->blockBegin();
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
void DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::valuePtrDecrement()
{
    BSLS_ASSERT_SAFE(d_blockPtr_p[0]->d_data <= d_value_p);
    BSLS_ASSERT_SAFE(d_value_p < d_blockPtr_p[0]->d_data + BLOCK_LENGTH);

    --d_value_p;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
void DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::valuePtrIncrement()
{
    BSLS_ASSERT_SAFE(d_blockPtr_p[0]->d_data <= d_value_p);
    BSLS_ASSERT_SAFE(d_value_p < d_blockPtr_p[0]->d_data + BLOCK_LENGTH);

    ++d_value_p;
}

// ACCESSORS
template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
VALUE_TYPE&
DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::operator*() const
{
    return *d_value_p;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
DequeIterator<VALUE_TYPE, BLOCK_LENGTH>
DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::operator+(std::ptrdiff_t offset) const
{
    DequeIterator ret(*this);
    ret += offset;
    return ret;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
DequeIterator<VALUE_TYPE, BLOCK_LENGTH>
DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::operator-(std::ptrdiff_t offset) const
{
    DequeIterator ret(*this);
    ret += -offset;
    return ret;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
std::ptrdiff_t
DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::operator-(
                      const DequeIterator<VALUE_TYPE, BLOCK_LENGTH>& rhs) const
{
    if (d_blockPtr_p == rhs.d_blockPtr_p) {
        return d_value_p - rhs.d_value_p;                             // RETURN
    }
    else {
        const int numFullBlocks = static_cast<int>(
                                    this->d_blockPtr_p - rhs.d_blockPtr_p - 1);
        return (numFullBlocks * BLOCK_LENGTH +
                rhs.remainingInBlock() + this->offsetInBlock());      // RETURN
    }
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
VALUE_TYPE *
DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::blockBegin() const
{
    return reinterpret_cast<VALUE_TYPE*>(*d_blockPtr_p);
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
VALUE_TYPE *
DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::blockEnd() const
{
    return reinterpret_cast<VALUE_TYPE*>(*d_blockPtr_p) + BLOCK_LENGTH;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
typename
DequeImpUtil<VALUE_TYPE, BLOCK_LENGTH>::BlockPtr *
DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::blockPtr() const
{
    return d_blockPtr_p;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
std::size_t
DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::offsetInBlock() const
{
    return d_value_p - blockBegin();
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
std::size_t
DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::remainingInBlock() const
{
    return blockBegin() + BLOCK_LENGTH - d_value_p;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
VALUE_TYPE *
DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::valuePtr() const
{
    return d_value_p;
}

                  // ---------------------------------
                  // class DequeIterator<VALUE_TYPE,1>
                  // ---------------------------------

// CREATORS
template <class VALUE_TYPE>
inline
DequeIterator<VALUE_TYPE, 1>::DequeIterator()
: d_blockPtr_p(0)
, d_value_p(0)
{
}

template <class VALUE_TYPE>
inline
DequeIterator<VALUE_TYPE, 1>::DequeIterator(BlockPtr *blockPtrPtr)
: d_blockPtr_p(blockPtrPtr)
, d_value_p(reinterpret_cast<VALUE_TYPE*>(*blockPtrPtr))
{
}

template <class VALUE_TYPE>
inline
DequeIterator<VALUE_TYPE, 1>::DequeIterator(BlockPtr   *blockPtrPtr,
                                            VALUE_TYPE *valuePtr)
: d_blockPtr_p(blockPtrPtr)
, d_value_p(valuePtr)
{
    BSLS_ASSERT_SAFE((*blockPtrPtr)->d_data == valuePtr);
}

// MANIPULATORS
template <class VALUE_TYPE>
inline
void DequeIterator<VALUE_TYPE, 1>::operator++()
{
    ++d_blockPtr_p;
    d_value_p = reinterpret_cast<VALUE_TYPE*>(*d_blockPtr_p);
}

template <class VALUE_TYPE>
inline
void DequeIterator<VALUE_TYPE, 1>::operator--()
{
    --d_blockPtr_p;
    d_value_p = reinterpret_cast<VALUE_TYPE*>(*d_blockPtr_p);
}

template <class VALUE_TYPE>
inline
void DequeIterator<VALUE_TYPE, 1>::operator+=(std::ptrdiff_t offset)
{
    d_blockPtr_p += offset;
    d_value_p     = reinterpret_cast<VALUE_TYPE*>(*d_blockPtr_p);
}

template <class VALUE_TYPE>
inline
void DequeIterator<VALUE_TYPE, 1>::operator-=(std::ptrdiff_t offset)
{
    this->operator+=(-offset);
}

template <class VALUE_TYPE>
inline
void DequeIterator<VALUE_TYPE, 1>::nextBlock()
{
    ++d_blockPtr_p;
    d_value_p = reinterpret_cast<VALUE_TYPE*>(*d_blockPtr_p);
}

template <class VALUE_TYPE>
inline
void DequeIterator<VALUE_TYPE, 1>::previousBlock()
{
    --d_blockPtr_p;
    d_value_p = reinterpret_cast<VALUE_TYPE*>(*d_blockPtr_p);
}

template <class VALUE_TYPE>
inline
void DequeIterator<VALUE_TYPE, 1>::setBlock(BlockPtr *blockPtrPtr)
{
    d_blockPtr_p = blockPtrPtr;
    d_value_p    = reinterpret_cast<VALUE_TYPE*>(*d_blockPtr_p);
}

template <class VALUE_TYPE>
inline
void DequeIterator<VALUE_TYPE, 1>::valuePtrDecrement()
{
    // This should never be called for 'BLOCK_LENGTH' of 1
    BSLS_ASSERT_SAFE(0);
}

template <class VALUE_TYPE>
inline
void DequeIterator<VALUE_TYPE, 1>::valuePtrIncrement()
{
    // This should never be called for 'BLOCK_LENGTH' of 1
    BSLS_ASSERT_SAFE(0);
}

// ACCESSORS
template <class VALUE_TYPE>
inline
VALUE_TYPE& DequeIterator<VALUE_TYPE, 1>::operator*() const
{
    return *d_value_p;
}

template <class VALUE_TYPE>
inline
DequeIterator<VALUE_TYPE, 1>
DequeIterator<VALUE_TYPE, 1>::operator+(std::ptrdiff_t offset) const
{
    return DequeIterator<VALUE_TYPE, 1>(d_blockPtr_p + offset);
}

template <class VALUE_TYPE>
inline
DequeIterator<VALUE_TYPE, 1>
DequeIterator<VALUE_TYPE, 1>::operator-(std::ptrdiff_t offset) const
{
    return DequeIterator<VALUE_TYPE, 1>(d_blockPtr_p - offset);
}

template <class VALUE_TYPE>
inline
std::ptrdiff_t
DequeIterator<VALUE_TYPE, 1>::operator-(
                                 const DequeIterator<VALUE_TYPE, 1>& rhs) const
{
    return d_blockPtr_p - rhs.d_blockPtr_p;
}

template <class VALUE_TYPE>
inline
VALUE_TYPE *
DequeIterator<VALUE_TYPE, 1>::blockBegin() const
{
    return reinterpret_cast<VALUE_TYPE*>(*d_blockPtr_p);
}

template <class VALUE_TYPE>
inline
VALUE_TYPE *
DequeIterator<VALUE_TYPE, 1>::blockEnd() const
{
    return reinterpret_cast<VALUE_TYPE*>(*d_blockPtr_p) + 1;
}

template <class VALUE_TYPE>
inline
typename
DequeImpUtil<VALUE_TYPE, 1>::BlockPtr *
DequeIterator<VALUE_TYPE, 1>::blockPtr() const
{
    return d_blockPtr_p;
}

template <class VALUE_TYPE>
inline
std::size_t
DequeIterator<VALUE_TYPE, 1>::offsetInBlock() const
{
    return 0;
}

template <class VALUE_TYPE>
inline
std::size_t
DequeIterator<VALUE_TYPE, 1>::remainingInBlock() const
{
    return 1;
}

template <class VALUE_TYPE>
inline
VALUE_TYPE *
DequeIterator<VALUE_TYPE, 1>::valuePtr() const
{
    return d_value_p;
}

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslalg_DequeIterator
#undef bslalg_DequeIterator
#endif
#define bslalg_DequeIterator bslalg::DequeIterator
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace bsl {

template <class VALUE_TYPE, int BLOCK_LENGTH>
struct is_trivially_copyable<BloombergLP::bslalg::DequeIterator<VALUE_TYPE,
                                                                BLOCK_LENGTH> >
    : true_type
{};

}  // close namespace bsl

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
