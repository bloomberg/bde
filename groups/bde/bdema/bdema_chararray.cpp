// bdema_chararray.cpp    -*-C++-*-
#include <bdema_chararray.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_chararray_cpp,"$Id$ $CSID$")


#include <bslma_bufferallocator.h>           // for testing only
#include <bslma_default.h>
#include <bslma_testallocator.h>             // for testing only
#include <bslma_testallocatorexception.h>    // for testing only

#include <bsls_assert.h>

#include <cstring>     // 'memcmp()', 'memcpy()', 'memmove()'

namespace BloombergLP {

// CONSTANTS
enum {
    INITIAL_SIZE = 1,  // initial physical capacity (number of elements)
    GROW_FACTOR  = 2   // multiplicative factor by which to grow 'd_size'
};

// STATIC METHODS
inline
static int nextSize(int size)
    // Return the specified 'size' multiplied by the constant 'GROW_FACTOR'.
{
    return size * GROW_FACTOR;
}

static int calculateSufficientSize(int minLength, int size)
    // Geometrically grow the specified current 'size' value while it is less
    // than the specified 'minLength' value.  Return the new size value.  The
    // behavior is undefined unless '1 <= size' and '0 <= minLength'.  Note
    // that if 'minLength <= size' then 'size' is returned.
{
    BSLS_ASSERT(1 <= size);
    BSLS_ASSERT(0 <= minLength);

    while (size < minLength) {
        size = nextSize(size);
    }

    BSLS_ASSERT(size >= minLength);

    return size;
}

static void reallocate(char            **array,
                       int              *size,
                       int               newSize,
                       int               length,
                       bslma_Allocator  *basicAllocator)
    // Reallocate memory in the specified 'array' and update the specified
    // size to the specified 'newSize' using the specified 'basicAllocator'.
    // The specified 'length' number of leading elements are preserved.  If
    // the 'basicAllocator' should throw an exception, this function has no
    // effect.  The behavior is undefined unless '1 <= newSize', '0 <= length',
    // and 'newSize <= length'.
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(*array);             // this is not 'allocate'
    BSLS_ASSERT(size);
    BSLS_ASSERT(1 <= newSize);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(basicAllocator);
    BSLS_ASSERT(length <= *size);    // sanity check
    BSLS_ASSERT(length <= newSize);  // ensure class invariant

    char *tmp = *array;

    *array = (char *) basicAllocator->allocate(newSize * sizeof **array);
    // COMMIT
    std::memcpy(*array, tmp, length * sizeof **array);
    *size = newSize;
    basicAllocator->deallocate(tmp);
}

inline
static void initializeWithDefaultValue(char *array, int length)
    // Initialize the specified 'length' leading elements of the specified
    // array with the default value.  The behavior is undefined unless
    // '0 <= length'.
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    std::memset(array, 0, length * sizeof *array);
}

inline
static void initializeWithValue(char *array, char value, int length)
    // Initialize the specified 'length' leading elements of the specified
    // array with the specified 'value'.  The behavior is undefined unless
    // '0 <= length'.
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= length);

    std::memset(array, value, length * sizeof *array);
}

                        // ---------------------
                        // class bdema_CharArray
                        // ---------------------

// PRIVATE MANIPULATORS
void bdema_CharArray::increaseSize()
{
    reallocate(&d_array_p, &d_size, nextSize(d_size), d_length, d_allocator_p);
}

void bdema_CharArray::reserveCapacityImp(int numElements)
    // This method is called by the inline 'reserveCapacity()' when the current
    // capacity in insufficient.
{
    BSLS_ASSERT(0 <= numElements);

    // The size allocated will be >= numElements

    int newSize = calculateSufficientSize(numElements, d_size);
    reallocate(&d_array_p, &d_size, newSize, d_length, d_allocator_p);
}

// CREATORS
bdema_CharArray::bdema_CharArray(bslma_Allocator *basicAllocator)
: d_size(INITIAL_SIZE)
, d_length(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(d_allocator_p);

    d_array_p = (char *) d_allocator_p->allocate(d_size * sizeof *d_array_p);
}

bdema_CharArray::bdema_CharArray(int              initialLength,
                                 bslma_Allocator *basicAllocator)
: d_length(initialLength)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 <= d_length);
    BSLS_ASSERT(d_allocator_p);

    d_size = calculateSufficientSize(d_length, INITIAL_SIZE);

    BSLS_ASSERT(d_length <= d_size);

    d_array_p = (char *) d_allocator_p->allocate(d_size * sizeof *d_array_p);
    initializeWithDefaultValue(d_array_p, d_length);
}

bdema_CharArray::bdema_CharArray(int              initialLength,
                                 char             initialValue,
                                 bslma_Allocator *basicAllocator)
: d_length(initialLength)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 <= d_length);
    BSLS_ASSERT(d_allocator_p);

    d_size = calculateSufficientSize(d_length, INITIAL_SIZE);

    BSLS_ASSERT(d_length <= d_size);

    d_array_p = (char *) d_allocator_p->allocate(d_size * sizeof *d_array_p);
    initializeWithValue(d_array_p, initialValue, d_length);
}

bdema_CharArray::bdema_CharArray(const InitialCapacity& numElements,
                                 bslma_Allocator       *basicAllocator)
: d_size(numElements.d_i <= 0 ? INITIAL_SIZE : numElements.d_i)
, d_length(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(1 <= d_size);
    BSLS_ASSERT(d_length <= d_size);
    BSLS_ASSERT(0 <= numElements.d_i);
    BSLS_ASSERT(d_allocator_p);

    d_array_p = (char *) d_allocator_p->allocate(d_size * sizeof *d_array_p);
}

bdema_CharArray::bdema_CharArray(const InitialCapacity& numElements,
                                 Hint                   sizingHint,
                                 bslma_Allocator       *basicAllocator)
: d_length(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 <= numElements.d_i);
    BSLS_ASSERT(d_allocator_p);

    if (numElements.d_i <= 0) {
        d_size = INITIAL_SIZE;
    } else if (sizingHint == POWER_OF_2_HINT) {
        d_size = calculateSufficientSize(numElements.d_i, INITIAL_SIZE);
    } else {              // EXACT_SIZE_HINT
        d_size = numElements.d_i;
    }

    BSLS_ASSERT(1 <= d_size);
    BSLS_ASSERT(d_length <= d_size);

    d_array_p = (char *) d_allocator_p->allocate(d_size * sizeof *d_array_p);
}

bdema_CharArray::bdema_CharArray(const char      *srcArray,
                                 int              numElements,
                                 bslma_Allocator *basicAllocator)
: d_length(numElements)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(srcArray);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(0 <= d_length);
    BSLS_ASSERT(d_allocator_p);

    d_size = calculateSufficientSize(d_length, INITIAL_SIZE);
    d_array_p = (char *) d_allocator_p->allocate(d_size * sizeof *d_array_p);
    std::memcpy(d_array_p, srcArray, d_length * sizeof *d_array_p);
}

bdema_CharArray::bdema_CharArray(const bdema_CharArray&  original,
                                 bslma_Allocator        *basicAllocator)
: d_length(original.d_length)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(d_allocator_p);

    d_size = calculateSufficientSize(d_length, INITIAL_SIZE);
    d_array_p = (char *) d_allocator_p->allocate(d_size * sizeof *d_array_p);
    std::memcpy(d_array_p, original.d_array_p, d_length * sizeof *d_array_p);
}

bdema_CharArray::~bdema_CharArray()
{
    BSLS_ASSERT(d_array_p);
    BSLS_ASSERT(1 <= d_size);
    BSLS_ASSERT(0 <= d_length);
    BSLS_ASSERT(d_allocator_p);
    BSLS_ASSERT(d_length <= d_size);

    d_allocator_p->deallocate(d_array_p);
}

// MANIPULATORS
bdema_CharArray& bdema_CharArray::operator=(const bdema_CharArray& rhs)
{
    if (this != &rhs) {
        if (d_size < rhs.d_length) {
            char *tmp = d_array_p;
            int newSize = calculateSufficientSize(rhs.d_length, d_size);
            d_array_p = (char *)
                        d_allocator_p->allocate(newSize * sizeof *d_array_p);
            // COMMIT
            d_size = newSize;
            d_allocator_p->deallocate(tmp);
        }

        BSLS_ASSERT(d_size >= rhs.d_length);

        d_length = rhs.d_length;
        std::memcpy(d_array_p, rhs.d_array_p, d_length * sizeof *d_array_p);
    }
    return *this;
}

void bdema_CharArray::append(const bdema_CharArray& srcArray)
{
    int newLength = d_length + srcArray.d_length;
    if (d_size < newLength) {
        int newSize = calculateSufficientSize(newLength, d_size);
        reallocate(&d_array_p, &d_size, newSize, d_length, d_allocator_p);
    }

    BSLS_ASSERT(d_size >= newLength);

    std::memcpy(d_array_p + d_length,
                srcArray.d_array_p,
                srcArray.d_length * sizeof *d_array_p);
    d_length = newLength;
}

void bdema_CharArray::append(const bdema_CharArray& srcArray,
                             int                    srcIndex,
                             int                    numElements)
{
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(srcIndex + numElements <= srcArray.d_length);

    int newLength = d_length + numElements;
    if (d_size < newLength) {
        int newSize = calculateSufficientSize(newLength, d_size);
        reallocate(&d_array_p, &d_size, newSize, d_length, d_allocator_p);
    }

    BSLS_ASSERT(d_size >= newLength);

    std::memcpy(d_array_p + d_length,
                srcArray.d_array_p + srcIndex,
                numElements * sizeof *d_array_p);
    d_length = newLength;
}

void bdema_CharArray::append(const char *srcArray,
                             int         srcIndex,
                             int         numElements)
{
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numElements);

    int newLength = d_length + numElements;
    if (newLength > d_size) {  // Need to resize and guard against alias.
        int newSize = calculateSufficientSize(newLength, d_size);
        char *tmp = d_array_p;
        d_array_p =
            (char *) d_allocator_p->allocate(newSize * sizeof *d_array_p);
        // COMMIT
        d_size = newSize;
        std::memcpy(d_array_p, tmp, d_length * sizeof *d_array_p);
        std::memcpy(d_array_p + d_length,
                    srcArray + srcIndex,
                    numElements * sizeof *d_array_p);

        d_length = newLength;
        d_allocator_p->deallocate(tmp);
        return;                                                       // RETURN
    }

    BSLS_ASSERT(d_size >= newLength);

    std::memcpy(d_array_p + d_length,
                srcArray + srcIndex,
                numElements * sizeof *d_array_p);
    d_length = newLength;
}

void bdema_CharArray::insert(int dstIndex, char item)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(dstIndex <= d_length);

    if (d_length >= d_size) {  // Need to resize.
        int newSize = nextSize(d_size);
        char *tmp = d_array_p;
        d_array_p =
            (char *) d_allocator_p->allocate(newSize * sizeof *d_array_p);
        // COMMIT
        d_size = newSize;
        char *here = d_array_p + dstIndex;
        char *there = tmp + dstIndex;
        std::memcpy(d_array_p, tmp, dstIndex * sizeof *d_array_p);
        std::memcpy(here + 1,
                    there,
                    (d_length - dstIndex) * sizeof *d_array_p);
        *here = item;
        ++d_length;
        d_allocator_p->deallocate(tmp);
        return;                                                       // RETURN
    }

    BSLS_ASSERT(d_length < d_size);  // Enough capacity exists.

    // There are no aliasing issues when an item (of fundamental type) is
    // passed by value.

    char *here = d_array_p + dstIndex;
    std::memmove(here + 1, here, (d_length - dstIndex) * sizeof *d_array_p);
    *here = item;
    ++d_length;
}

void bdema_CharArray::insert(int dstIndex, const bdema_CharArray& srcArray)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(dstIndex <= d_length);

    int srcLength = srcArray.d_length;
    int newLength = d_length + srcLength;

    if (newLength > d_size) {  // Need to resize.
        int newSize = calculateSufficientSize(newLength, d_size);
        char *tmp = d_array_p;
        char *src = srcArray.d_array_p;  // necessary for aliasing
        d_array_p =
                 (char *) d_allocator_p->allocate(newSize * sizeof *d_array_p);
        // COMMIT
        d_size = newSize;
        char *here = d_array_p + dstIndex;
        char *there = tmp + dstIndex;
        std::memcpy(d_array_p, tmp, dstIndex * sizeof *d_array_p);
        std::memcpy(here, src, srcLength * sizeof *d_array_p);
        std::memcpy(here + srcLength,
                    there,
                    (d_length - dstIndex) * sizeof *d_array_p);
        d_length = newLength;
        d_allocator_p->deallocate(tmp);
        return;                                                       // RETURN
    }

    BSLS_ASSERT(newLength <= d_size);  // Enough capacity exists.

    // There is no aliasing issue so long as we (1) shift as in 'std::memmove'
    // (rather than via copy ctor/dtor) and (2) do not affect data below the
    // logical end of this array (which we won't since the shift is by the
    // entire 'srcLength').

    char *here = d_array_p + dstIndex;
    std::memmove(here + srcArray.d_length,
                 here,
                 (d_length - dstIndex) * sizeof *d_array_p);
    std::memmove(here, srcArray.d_array_p, srcLength * sizeof *d_array_p);
    d_length = newLength;
}

void bdema_CharArray::insert(int                    dstIndex,
                             const bdema_CharArray& srcArray,
                             int                    srcIndex,
                             int                    numElements)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(dstIndex <= d_length);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(srcIndex + numElements <= srcArray.d_length);

    int newLength = d_length + numElements;

    if (newLength > d_size) {  // Need to resize.
        int newSize = calculateSufficientSize(newLength, d_size);
        char *tmp = d_array_p;
        char *src = srcArray.d_array_p;  // necessary for aliasing
        d_array_p =
                 (char *) d_allocator_p->allocate(newSize * sizeof *d_array_p);
        // COMMIT
        d_size = newSize;
        char *here = d_array_p + dstIndex;
        char *there = tmp + dstIndex;
        std::memcpy(d_array_p, tmp, dstIndex * sizeof *d_array_p);
        std::memcpy(here, src + srcIndex, numElements * sizeof *d_array_p);
        std::memcpy(here + numElements,
                    there,
                    (d_length - dstIndex) * sizeof *d_array_p);
        d_length = newLength;
        d_allocator_p->deallocate(tmp);
        return;                                                       // RETURN
    }

    char *d = d_array_p + dstIndex;
    char *s = srcArray.d_array_p + srcIndex;
    std::memmove(d + numElements, d, (d_length - dstIndex) * sizeof *d);

    if (&srcArray == this && srcIndex + numElements > dstIndex) {
        // We have an aliasing problem and shifting will affect the source:
        // If srcIndex < dstIndex then the source is split; otherwise, the
        // entire source has been shifted up by exactly numElement positions.

        if (srcIndex < dstIndex) {  // Alias problem: the source is split.
            int ne1 = d - s;
            char *d2 = d + ne1;
            char *s2 = s + numElements + ne1;
            int ne2 = numElements - ne1;

            BSLS_ASSERT(ne1 > 0);
            BSLS_ASSERT(ne2 > 0);

            std::memcpy(d, s, ne1 * sizeof *d);
            std::memcpy(d2, s2, ne2 * sizeof *d);
        }
        else {  // Alias problem: source shifted as single unit by numElements.
            std::memcpy(d, s + numElements, numElements * sizeof *d);
        }
    }
    else {  // There are no aliasing issues.
        std::memcpy(d, s, numElements * sizeof *d);
    }

    d_length = newLength;
}

void bdema_CharArray::insert(int         dstIndex,
                             const char *srcArray,
                             int         srcIndex,
                             int         numElements)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(dstIndex <= d_length);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numElements);

    int newLength = d_length + numElements;

    if (newLength > d_size) {  // Need to resize.
        int newSize = calculateSufficientSize(newLength, d_size);
        char *tmp = d_array_p;
        const char *src = srcArray;  // necessary for aliasing
        d_array_p =
            (char *) d_allocator_p->allocate(newSize * sizeof *d_array_p);
        // COMMIT
        d_size = newSize;
        char *here = d_array_p + dstIndex;
        char *there = tmp + dstIndex;
        std::memcpy(d_array_p, tmp, dstIndex * sizeof *d_array_p);
        std::memcpy(here, src + srcIndex, numElements * sizeof *d_array_p);
        std::memcpy(here + numElements,
                    there,
                    (d_length - dstIndex) * sizeof *d_array_p);
        d_length = newLength;
        d_allocator_p->deallocate(tmp);
        return;                                                       // RETURN
    }

    char *d = d_array_p + dstIndex;
    const char *s = srcArray + srcIndex;
    std::memmove(d + numElements, d, (d_length - dstIndex) * sizeof *d);

    int isAlias = srcArray >= d_array_p && srcArray < d_array_p + d_length;
    int delta = srcArray - d_array_p;

    if (isAlias && delta + srcIndex + numElements > dstIndex) {
        // We have an aliasing problem and shifting will affect the source:
        // If delta + srcIndex < dstIndex then the source is split;
        // otherwise, the entire source has been shifted up by exactly
        // numElement positions.

        if (delta + srcIndex < dstIndex) {  // Alias problem: source split.
            int ne1 = d - s;
            char *d2 = d + ne1;
            const char *s2 = s + numElements + ne1;
            int ne2 = numElements - ne1;

            BSLS_ASSERT(ne1 > 0);
            BSLS_ASSERT(ne2 > 0);

            std::memcpy(d, s, ne1 * sizeof *d);
            std::memcpy(d2, s2, ne2 * sizeof *d);
        }
        else {  // Alias problem: source shifted as single unit by numElements.
            std::memcpy(d, s + numElements, numElements * sizeof *d);
        }
    }
    else {  // There are no aliasing issues.
        std::memcpy(d, s, numElements * sizeof *d);
    }

    d_length = newLength;
}

void bdema_CharArray::remove(int index)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(index < d_length);

    --d_length;
    char *here = d_array_p + index;
    std::memmove(here, here + 1, (d_length - index) * sizeof *d_array_p);
}

void bdema_CharArray::remove(int index, int numElements)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(index + numElements <= d_length);

    d_length -= numElements;
    char *here = d_array_p + index;
    std::memmove(here,
                 here + numElements,
                 (d_length - index) * sizeof *d_array_p);
}

void bdema_CharArray::replace(int                    dstIndex,
                              const bdema_CharArray& srcArray,
                              int                    srcIndex,
                              int                    numElements)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(dstIndex + numElements <= d_length);
    BSLS_ASSERT(srcIndex + numElements <= srcArray.d_length);

    std::memmove(d_array_p + dstIndex,
                 srcArray.d_array_p + srcIndex,
                 numElements * sizeof *d_array_p);
}

void bdema_CharArray::reserveCapacityRaw(int numElements)
{
    BSLS_ASSERT(0 <= numElements);

    if (d_size < numElements) {
        reallocate(&d_array_p, &d_size, numElements, d_length, d_allocator_p);
    }
}

void bdema_CharArray::setLength(int newLength)
{
    BSLS_ASSERT(0 <= newLength);

    if (newLength > d_length) {
        if (newLength > d_size) {
            int newSize = calculateSufficientSize(newLength, d_size);
            reallocate(&d_array_p, &d_size, newSize, d_length, d_allocator_p);
        }
        initializeWithDefaultValue(d_array_p + d_length, newLength - d_length);
    }
    d_length = newLength;
}

void bdema_CharArray::setLength(int newLength, char initialValue)
{
    BSLS_ASSERT(0 <= newLength);

    if (newLength > d_length) {
        if (newLength > d_size) {
            int newSize = calculateSufficientSize(newLength, d_size);
            reallocate(&d_array_p, &d_size, newSize, d_length, d_allocator_p);
        }
        initializeWithValue(d_array_p + d_length, initialValue,
                            newLength - d_length);
    }
    d_length = newLength;
}

// FREE OPERATORS
bool operator==(const bdema_CharArray& lhs, const bdema_CharArray& rhs)
{
    const int len = lhs.length();
    return len == rhs.length()
        && 0 == std::memcmp(lhs.data(), rhs.data(), len * sizeof *lhs.data());
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
