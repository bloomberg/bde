// bslalg_dequeiterator.h                                             -*-C++-*-
#ifndef INCLUDED_BSLALG_DEQUEITERATOR
#define INCLUDED_BSLALG_DEQUEITERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide primitive data structures for implementing deques.
//
//@CLASSES:
//  bslalg::DequeImp: namespace for deque primitive data structures
//
//@SEE_ALSO: bslalg_dequeiterator, bslalg_dequeprimitives
//
//@DESCRIPTION: This component provides an iterator type for enumerating
// elements in a deque (implemented in the form of dynamic-array) knowing only
// its value type and a nominal block size.  Conceptually, a deque is an array
// of blocks pointers, each block capable of containing a fixed number of
// objects.  An element in the deque is identified by an iterator which
// consists of two pointers:
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
///Usage
///-----
// This component is for use by the 'bslstl' package.  Other clients should use
// the STL deque (in header '<deque>').

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_DEQUEIMPUTIL
#include <bslalg_dequeimputil.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // std::size_t, std::ptrdiff_t
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {

namespace bslalg {

template <class VALUE_TYPE, int BLOCK_LENGTH>
class DequeIterator;

template <class VALUE_TYPE>
class DequeIterator<VALUE_TYPE, 1>;

// WARNING: These free operators are declared here as a workaround for windows,
// because windows gives an ambiguity error if the operator declaration is
// declared after the friend declaration.

// FREE OPERATORS
template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
bool operator==(const DequeIterator<VALUE_TYPE, BLOCK_LENGTH>& lhs,
                const DequeIterator<VALUE_TYPE, BLOCK_LENGTH>& rhs);
    // Return 'true' if the specified 'rhs' iterator points to the same element
    // in the same block as this iterator, and 'false' otherwise.

template <class VALUE_TYPE>
inline
bool operator==(const DequeIterator<VALUE_TYPE, 1>& lhs,
                const DequeIterator<VALUE_TYPE, 1>& rhs);
    // Specialization for deques having a block length of 1.

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
bool operator!=(const DequeIterator<VALUE_TYPE, BLOCK_LENGTH>& lhs,
                const DequeIterator<VALUE_TYPE, BLOCK_LENGTH>& rhs);
    // Return 'true' if the specified 'rhs' iterator points to a different
    // element as this iterator, and 'false' otherwise.

template <class VALUE_TYPE>
inline
bool operator!=(const DequeIterator<VALUE_TYPE, 1>& lhs,
                const DequeIterator<VALUE_TYPE, 1>& rhs);
    // Specialization for deques having a block length of 1.

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
bool operator<(const DequeIterator<VALUE_TYPE, BLOCK_LENGTH>& lhs,
               const DequeIterator<VALUE_TYPE, BLOCK_LENGTH>& rhs);
    // Return 'true' if the specified 'rhs' iterator points to an element in a
    // previous block or in a previous position in the same block as this
    // iterator, and 'false' otherwise.

template <class VALUE_TYPE>
inline
bool operator<(const DequeIterator<VALUE_TYPE, 1>& lhs,
               const DequeIterator<VALUE_TYPE, 1>& rhs);
    // Specialization for deques having a block length of 1.

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
    friend bool operator==<VALUE_TYPE, BLOCK_LENGTH>(
                               const DequeIterator<VALUE_TYPE, BLOCK_LENGTH>&,
                               const DequeIterator<VALUE_TYPE, BLOCK_LENGTH>&);

    friend bool operator!=<VALUE_TYPE, BLOCK_LENGTH>(
                               const DequeIterator<VALUE_TYPE, BLOCK_LENGTH>&,
                               const DequeIterator<VALUE_TYPE, BLOCK_LENGTH>&);

    friend bool operator< <VALUE_TYPE, BLOCK_LENGTH>(
                               const DequeIterator<VALUE_TYPE, BLOCK_LENGTH>&,
                               const DequeIterator<VALUE_TYPE, BLOCK_LENGTH>&);

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

    void valuePtrIncrement();
        // Increment this iterator to point to the next element in the block of
        // the corresponding deque.  The behavior is undefined unless this
        // iterator is pointed to a valid position of the deque.  Note that
        // this method is used only for optimization purposes in
        // 'bslstl_Deque', and clients of this package should not use this
        // directly.

    void valuePtrDecrement();
        // Decrement this iterator to point to the next element in the block of
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
    // implementation.

    // PRIVATE TYPES
    typedef bslalg::DequeImpUtil<VALUE_TYPE, 1> DequeImpUtil;
    typedef typename DequeImpUtil::BlockPtr     BlockPtr;
    typedef DequeIterator<VALUE_TYPE, 1>        IteratorType;

    // DATA
    BlockPtr   *d_blockPtr_p; // pointer to BlockPtr within BlockPtr array
    VALUE_TYPE *d_value_p;    // pointer to element referenced by iterator

#ifdef BSLS_PLATFORM_CMP_SUN
    // WARNING: Note that SUN's compiler complains about function "friend
    // declaration is incompatible with function template" when xlC, gcc and
    // windows all accept the form wrapped in the '#else' statement.  Therefore
    // it is declared differently.

    template <class VALUE_TYPE>
    friend bool operator==(const DequeIterator<VALUE_TYPE, 1>&,
                           const DequeIterator<VALUE_TYPE, 1>&);

    template <class VALUE_TYPE>
    friend bool operator!=(const DequeIterator<VALUE_TYPE, 1>&,
                           const DequeIterator<VALUE_TYPE, 1>&);

    template <class VALUE_TYPE>
    friend bool operator< (const DequeIterator<VALUE_TYPE, 1>&,
                           const DequeIterator<VALUE_TYPE, 1>&);
#else
    friend bool operator==<VALUE_TYPE>
                              (const DequeIterator<VALUE_TYPE, 1>&,
                               const DequeIterator<VALUE_TYPE, 1>&);
    friend bool operator!=<VALUE_TYPE>
                              (const DequeIterator<VALUE_TYPE, 1>&,
                               const DequeIterator<VALUE_TYPE, 1>&);
    friend bool operator< <VALUE_TYPE>
                              (const DequeIterator<VALUE_TYPE, 1>&,
                               const DequeIterator<VALUE_TYPE, 1>&);
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
    void valuePtrIncrement();
    void valuePtrDecrement();

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
    BSLS_ASSERT_SAFE(blockPtrPtr[0]->d_data <= valuePtr);
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
void DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::valuePtrIncrement()
{
    BSLS_ASSERT_SAFE(d_blockPtr_p[0]->d_data <= d_value_p);
    BSLS_ASSERT_SAFE(d_value_p < d_blockPtr_p[0]->d_data + BLOCK_LENGTH);

    ++d_value_p;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
void DequeIterator<VALUE_TYPE, BLOCK_LENGTH>::valuePtrDecrement()
{
    BSLS_ASSERT_SAFE(d_blockPtr_p[0]->d_data <= d_value_p);
    BSLS_ASSERT_SAFE(d_value_p < d_blockPtr_p[0]->d_data + BLOCK_LENGTH);

    --d_value_p;
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
void DequeIterator<VALUE_TYPE, 1>::valuePtrIncrement()
{
    // This should never be called for 'BLOCK_LENGTH' of 1
    BSLS_ASSERT_SAFE(0);
}

template <class VALUE_TYPE>
inline
void DequeIterator<VALUE_TYPE, 1>::valuePtrDecrement()
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

// FREE OPERATORS
template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
bool bslalg::operator==(const DequeIterator<VALUE_TYPE, BLOCK_LENGTH>& lhs,
                        const DequeIterator<VALUE_TYPE, BLOCK_LENGTH>& rhs)
{
    return lhs.d_value_p == rhs.d_value_p;
}

template <class VALUE_TYPE>
inline
bool bslalg::operator==(const DequeIterator<VALUE_TYPE, 1>& lhs,
                        const DequeIterator<VALUE_TYPE, 1>& rhs)
{
    return lhs.d_blockPtr_p == rhs.d_blockPtr_p;
}

template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
bool bslalg::operator!=(const DequeIterator<VALUE_TYPE, BLOCK_LENGTH>& lhs,
                        const DequeIterator<VALUE_TYPE, BLOCK_LENGTH>& rhs)
{
    return !(lhs == rhs);
}

template <class VALUE_TYPE>
inline
bool bslalg::operator!=(const DequeIterator<VALUE_TYPE, 1>& lhs,
                        const DequeIterator<VALUE_TYPE, 1>& rhs)
{
    return !(lhs == rhs);
}


template <class VALUE_TYPE, int BLOCK_LENGTH>
inline
bool bslalg::operator<(const DequeIterator<VALUE_TYPE, BLOCK_LENGTH>& lhs,
                       const DequeIterator<VALUE_TYPE, BLOCK_LENGTH>& rhs)
{
    if (lhs.d_blockPtr_p == rhs.d_blockPtr_p) {
        return lhs.d_value_p < rhs.d_value_p;                         // RETURN
    }
    else {
        return lhs.d_blockPtr_p < rhs.d_blockPtr_p;                   // RETURN
    }
}

template <class VALUE_TYPE>
inline
bool bslalg::operator<(const DequeIterator<VALUE_TYPE, 1>& lhs,
                       const DequeIterator<VALUE_TYPE, 1>& rhs)
{
    return lhs.d_blockPtr_p < rhs.d_blockPtr_p;
}


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
