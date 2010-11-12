// bdec_voidptrqueue.cpp            -*-C++-*-
#include <bdec_voidptrqueue.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdec_voidptrqueue_cpp,"$Id$ $CSID$")

//
///IMPLEMENTATION NOTES
///--------------------
// This component implements a double-ended queue.  The queue is stored in
// memory as an array with 'd_front' indicating the position before the first
// element and 'd_back' the position after the last element; this implies two
// extra array positions are required beyond what is strictly necessary.  This
// technique was chosen since it avoids all ambiguities involving empty and
// full queues, and simplifies efficient implementation via symmetry.
//
// Since a queue stored in this manner is circular, there are two general
// scenarios to consider during every operation (the numbers in the cells are
// index positions):
//..
//                         +---+---+---+---+---+---+---+---+---+---+
//    d_front < d_back     |   |   | 0 | 1 | 2 | 3 |   |   |   |   |
//                         +---+---+---+---+---+---+---+---+---+---+
//                               ^                   ^
//                               |___ d_front        |___ d_back
//
//                         +---+---+---+---+---+---+---+---+---+---+
//    d_front > d_back     | 3 |   |   |   |   |   |   | 0 | 1 | 2 |
//                         +---+---+---+---+---+---+---+---+---+---+
//                               ^                   ^
//                               |___ d_back         |___ d_front
//..

#include <bdeu_print.h>

#include <bslalg_arrayprimitives.h>

#include <bslma_default.h>

#include <bsls_assert.h>
#include <bsls_platformutil.h>

#include <bsl_ostream.h>

#include <bsl_cstring.h>   // for bsl::memmove, bsl::memset

// CONSTANTS

namespace BloombergLP {

enum {
    INITIAL_SIZE   = 2,  // initial physical capacity (number of elements)
    GROW_FACTOR    = 2,  // multiplicative factor by which to grow 'd_size'
    EXTRA_CAPACITY = 2   // additional capacity required by implementation
};
// The queue is full when d_front == d_back.  Hence, 'INITIAL_SIZE' must be at
// least two.

// STATIC METHODS

inline
static int nextSize(int size)
    // Return the specified 'size' multiplied by the constant 'GROW_FACTOR'.
{
    return size * GROW_FACTOR;
}

inline
static int calculateSufficientSize(int minLength, int size)
    // Grow geometrically the specified current 'size' value while it is less
    // than the specified 'minLength' value plus any additional capacity
    // required by the implementation (i.e., 'EXTRA_CAPACITY' elements).
    // Return the new size value.  The behavior is undefined unless
    // 'INITIAL_SIZE <= size' and '0 <= minLength'.  Note that if
    // 'minLength + EXTRA_CAPACITY <= size' then 'size' is returned.
{
    BSLS_ASSERT(INITIAL_SIZE <= size);
    BSLS_ASSERT(0 <= minLength);

    const int len = minLength + EXTRA_CAPACITY;

    while (size < len) {
        size = nextSize(size);
    }

    BSLS_ASSERT(size >= len);
    return size;
}

inline
static int memcpyCircular(void **dstArray,
                          int    dstSize,
                          int    dstIndex,
                          void **srcArray,
                          int    srcSize,
                          int    srcIndex,
                          int    numElements)
    // Copy efficiently the specified 'numElements' data values from the
    // specified 'srcArray' of the specified 'srcSize' starting at the
    // specified 'srcIndex' into the specified 'dstArray' of the specified
    // 'dstSize' starting at the specified 'dstIndex'.  Return the new value
    // for the back of the queue.  The 'srcArray' and the 'dstArray' are
    // assumed to be queues; they are circular which implies copy may have to
    // be broken into multiple parts since the underlying array is linear.  The
    // behavior is undefined unless '0 <= dstSize', '0 <= dstIndex < dstSize',
    // '0 <= srcIndex < srcSize', '0 <= numElements',
    // 'numElements <= dstSize - EXTRA_CAPACITY', and
    // 'numElements <= srcSize - EXTRA_CAPACITY' (the 'EXTRA_CAPACITY' accounts
    // for the locations of 'd_front' and 'd_back').  Note that aliasing is not
    // handled properly.
{
    BSLS_ASSERT(dstArray);
    BSLS_ASSERT(0 <= dstSize);
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(dstIndex < dstSize);
    BSLS_ASSERT(srcArray);
    BSLS_ASSERT(0 <= srcSize);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(srcIndex < srcSize);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(numElements <= dstSize - EXTRA_CAPACITY);
    BSLS_ASSERT(numElements <= srcSize - EXTRA_CAPACITY);

    int dst;  // temporary value to store the correct destination location

    // Break the source queue into one or two linear arrays to copy.

    int srcA = srcIndex;
    if (srcA + numElements <= srcSize) {  // one linear source array
        int lenSrcA = numElements;

        dst = dstIndex;

        // Compute the maximum number of elements that can be copied to the
        // destination array.

        int dstLen = dstSize - dst;

        if (dstLen >= lenSrcA) {  // can copy everything from srcA
            bsl::memcpy(dstArray + dst,
                        srcArray + srcA,
                        lenSrcA * sizeof *dstArray);
            dst += lenSrcA;
        }
        else {  // can copy only part of srcA without changing dst
            bsl::memcpy(dstArray + dst, srcArray + srcA,
                        dstLen * sizeof *dstArray);
            srcA += dstLen;
            lenSrcA -= dstLen;

            // WARNING: There seems to be an AIX compiler issue for the
            // following four lines.  Removing the 'assert' and moving the
            // 'bsl::memcpy' down two lines may cause the program to compile,
            // but not execute properly.

            bsl::memcpy(dstArray, srcArray + srcA, lenSrcA * sizeof *dstArray);
            dstLen = dst;  // max numElements that can be copied to index 0
            dst = lenSrcA;
            BSLS_ASSERT(lenSrcA <= dstLen - EXTRA_CAPACITY);
        }
    }
    else {  // two linear source arrays
        int lenSrcA = srcSize - srcA;
        int lenSrcB = numElements - lenSrcA;

        dst = dstIndex;

        // Compute the maximum number of elements that can be copied to the
        // destination array.

        int dstLen = dstSize - dst;

        if (dstLen >= lenSrcA) {  // can copy everything from srcA
            bsl::memcpy(dstArray + dst,
                        srcArray + srcA,
                        lenSrcA * sizeof *dstArray);
            dst += lenSrcA;
        }
        else {  // can copy only part of srcA without changing dst
            bsl::memcpy(dstArray + dst, srcArray + srcA,
                        dstLen * sizeof *dstArray);
            srcA += dstLen;
            lenSrcA -= dstLen;

            // WARNING: There seems to be an AIX compiler issue for the
            // following four lines.  Removing the 'assert' and moving the
            // 'bsl::memcpy' down two lines may cause the program to compile,
            // but not execute properly.

            bsl::memcpy(dstArray, srcArray + srcA, lenSrcA * sizeof *dstArray);
            dstLen = dst;  // max numElements that can be copied to index 0
            dst = lenSrcA;
            BSLS_ASSERT(lenSrcA + lenSrcB <= dstLen - EXTRA_CAPACITY);
        }
        dstLen -= lenSrcA;

        if (dstLen >= lenSrcB) {  // can copy everything from srcB
            bsl::memcpy(dstArray + dst, srcArray, lenSrcB * sizeof *dstArray);
            dst += lenSrcB;
        }
        else {  // can copy only part of srcB without changing dst
            // NOTE: could not have had insufficient room for srcA
            bsl::memcpy(dstArray + dst, srcArray, dstLen * sizeof *dstArray);
            lenSrcB -= dstLen;
            dst = lenSrcB;
            bsl::memcpy(dstArray, srcArray + dstLen,
                        lenSrcB * sizeof *dstArray);
        }
    }

    return dst % dstSize;
}

inline
static void memShiftLeft(void **array,
                         int    size,
                         int    dstIndex,
                         int    srcIndex,
                         int    numElements)
    // Copy efficiently the specified 'numElements' data values from the
    // specified 'array' of the specified 'size' starting at index position
    // 'srcIndex' to the specified 'dstIndex' assuming the elements are to
    // be moved to the left (towards the front of the queue).  'array' is
    // assumed to be a queue; it is circular.  The behavior is undefined unless
    // '0 <= size', '0 <= dstIndex < size', '0 <= srcIndex < size', and
    // '0 <= numElements <= size - EXTRA_CAPACITY'.  Note that this function is
    // alias safe.
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= size);
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(dstIndex < size);
    BSLS_ASSERT(srcIndex < size);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(numElements <= size - EXTRA_CAPACITY);

    // Move the elements that do not wrap around the array end.

    if (srcIndex > dstIndex) {
        int numMove = size - srcIndex;
        if (numMove >= numElements) {
            bsl::memmove(array + dstIndex,
                         array + srcIndex,
                         numElements * sizeof *array);
            return;
        }
        bsl::memmove(array + dstIndex,
                     array + srcIndex,
                     numMove * sizeof *array);
        numElements -= numMove;
        dstIndex += numMove;
        srcIndex = 0;
    }

    // Move the elements of the source that will just precede the array end.

    int numMove = size - dstIndex;
    if (numMove >= numElements) {
        bsl::memmove(array + dstIndex,
                     array + srcIndex,
                     numElements * sizeof *array);
        return;
    }
    bsl::memmove(array + dstIndex, array + srcIndex, numMove * sizeof *array);
    numElements -= numMove;
    srcIndex += numMove;

    // Move the elements of the source that are around the array end.

    bsl::memmove(array, array + srcIndex, numElements * sizeof *array);
}

inline
static void memShiftRight(void **array,
                          int    size,
                          int    dstIndex,
                          int    srcIndex,
                          int    numElements)
    // Copy efficiently the specified 'numElements' data values from the
    // specified 'array' of the specified 'size' starting at index position
    // 'srcIndex' to the specified 'dstIndex' assuming the elements are to
    // be moved to the right (towards the back of the queue).  'array' is
    // assumed to be a queue; it is circular.  The behavior is undefined unless
    // '0 <= size', '0 <= dstIndex < size', '0 <= srcIndex < size', and
    // '0 <= numElements <= size - EXTRA_CAPACITY'.  Note that this function is
    // alias safe.
{
    BSLS_ASSERT(array);
    BSLS_ASSERT(0 <= size);
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(dstIndex < size);
    BSLS_ASSERT(srcIndex < size);
    BSLS_ASSERT(numElements <= size - EXTRA_CAPACITY);

    {

        // Move the elements of the source that wrap around the array end.

        int numMove = srcIndex + numElements;
        if (numMove > size) {
            numMove -= size;
            bsl::memmove(array + (dstIndex + numElements - numMove) % size,
                         array,
                         numMove * sizeof *array);
            numElements -= numMove;
        }
    }

    {

        // Move the elements of the source that will wrap around the array end.

        int numMove = dstIndex + numElements;
        if (numMove > size) {
            numMove -= size;
            bsl::memmove(array,
                         array + (srcIndex + numElements - numMove) % size,
                         numMove * sizeof *array);
            numElements -= numMove;
        }
    }

    // Move the elements of the source that do not and will not wrap around
    // the array end.

    bsl::memmove(array + dstIndex, array + srcIndex,
                 numElements * sizeof *array);
}

inline
static void copyData(void **dstArray,
                     int   *dstBack,
                     int    dstSize,
                     int    dstFront,
                     void **srcArray,
                     int    srcSize,
                     int    srcFront,
                     int    srcBack)
    // Copy efficiently the queue indicated by the specified 'srcArray' of the
    // specified 'srcSize' with the specified 'srcFront' and the specified
    // 'srcBack' into the queue indicated by the specified 'dstArray' of the
    // specified 'dstSize', with the specified 'srcFront'.  The specified
    // 'dstBack' is set to make the length of the destination queue the same as
    // the length of the source queue.  The behavior is undefined unless
    // '0 <= dstSize', '0 <= dstFront < dstSize', '0 <= srcSize',
    // '0 <= srcFront < srcSize', and '0 <= srcBack < srcSize'.  Note that
    // aliasing is not handled properly.
{
    BSLS_ASSERT(dstArray);
    BSLS_ASSERT(dstBack);
    BSLS_ASSERT(0 <= dstSize);
    BSLS_ASSERT(0 <= dstFront);
    BSLS_ASSERT(dstFront < dstSize);
    BSLS_ASSERT(srcArray);
    BSLS_ASSERT(0 <= srcSize);
    BSLS_ASSERT(0 <= srcFront);
    BSLS_ASSERT(0 <= srcBack);
    BSLS_ASSERT(srcFront < srcSize);
    BSLS_ASSERT(srcBack < srcSize);

    const int dstIndex = (dstFront + 1) % dstSize;
    const int srcIndex = (srcFront + 1) % srcSize;
    const int numElements = (srcBack + srcSize - srcFront - 1) % srcSize;

    *dstBack = memcpyCircular(dstArray,
                              dstSize,
                              dstIndex,
                              srcArray,
                              srcSize,
                              srcIndex,
                              numElements);
}

inline
static int increaseSizeImp(void            ***addrArray,
                           int               *front,
                           int               *back,
                           int                newSize,
                           int                size,
                           bslma_Allocator   *allocator)
    // Increase the physical capacity of the queue represented by the
    // specified 'addrArray' to specified 'newSize' from the specified 'size'.
    // Return the new size of the queue.  This function copies the data
    // contained within the queue between the specified 'front' and 'back' to
    // the new queue and update the values of both 'front' and 'back'.  Use the
    // specified 'allocator' to supply and retrieve memory.  The behavior is
    // undefined unless 'INITIAL_SIZE <= newSize', 'INITIAL_SIZE <= size',
    // 'size <= newSize', '0 <= *front < size', and '0 <= *back < size'.
{
    BSLS_ASSERT(addrArray);
    BSLS_ASSERT(*addrArray);
    BSLS_ASSERT(INITIAL_SIZE <= newSize);
    BSLS_ASSERT(INITIAL_SIZE <= size);
    BSLS_ASSERT(size <= newSize);
    BSLS_ASSERT(front);
    BSLS_ASSERT(back);
    BSLS_ASSERT(0 <= *front);
    BSLS_ASSERT(0 <= *back);
    BSLS_ASSERT(*front < size);
    BSLS_ASSERT(*back < size);
    BSLS_ASSERT(allocator);

    void **array = (void **)allocator->allocate(newSize * sizeof **addrArray);

    // COMMIT

    const int oldFront = *front;
    *front = newSize - 1;
    copyData(array, back, newSize, *front, *addrArray, size, oldFront, *back);

    allocator->deallocate(*addrArray);
    *addrArray = array;
    return newSize;
}

inline
static bsl::ostream&
                    indent(bsl::ostream& stream, int level, int spacesPerLevel)
    // Emit the product of the specified 'level' and 'spacesPerLevel' to the
    // specified output 'stream' and return a reference to the modifiable
    // 'stream'.  If the product is negative, this function has no effect.
    // The behavior is undefined unless '0 <= spacesPerLevel'.

{
    BSLS_ASSERT(0 <= spacesPerLevel);
    {
        int n = level * spacesPerLevel;
        while (n-- > 0) {
            stream << ' ';
        }
    }
    return stream;
}

// PRIVATE METHODS

void bdec_VoidPtrQueue::increaseSize()
{
    d_size = increaseSizeImp(&d_array_p,
                             &d_front,
                             &d_back,
                             nextSize(d_size),
                             d_size,
                             d_allocator_p);
}

// CREATORS

bdec_VoidPtrQueue::bdec_VoidPtrQueue(bslma_Allocator *basicAllocator)
: d_size(INITIAL_SIZE)
, d_front(INITIAL_SIZE - 1)
, d_back(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(d_allocator_p);
    d_array_p = (void **)d_allocator_p->allocate(d_size * sizeof *d_array_p);
}

bdec_VoidPtrQueue::bdec_VoidPtrQueue(unsigned int     initialLength,
                                     bslma_Allocator *basicAllocator)
: d_back(initialLength)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(d_allocator_p);

    d_size = calculateSufficientSize(initialLength, INITIAL_SIZE);
    BSLS_ASSERT(static_cast<int>(initialLength) <= d_size);

    d_array_p = (void **)d_allocator_p->allocate(d_size * sizeof *d_array_p);

    d_front = d_size - 1;

    // initialize the array values
    bsl::memset(d_array_p, 0, d_back * sizeof(void *));
}

bdec_VoidPtrQueue::bdec_VoidPtrQueue(unsigned int     initialLength,
                                     void            *initialValue,
                                     bslma_Allocator *basicAllocator)
: d_back(initialLength)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(d_allocator_p);

    d_size = calculateSufficientSize(initialLength, INITIAL_SIZE);
    BSLS_ASSERT(static_cast<int>(initialLength) <= d_size);

    d_array_p = (void **)d_allocator_p->allocate(d_size * sizeof *d_array_p);

    d_front = d_size - 1;

    bslalg_ArrayPrimitives::uninitializedFillN(d_array_p,
                                               d_back,
                                               initialValue,
                                               d_allocator_p);
}

bdec_VoidPtrQueue::bdec_VoidPtrQueue(const InitialCapacity&  numElements,
                                     bslma_Allocator        *basicAllocator)
: d_size(numElements.d_i + EXTRA_CAPACITY) // to hold the empty positions
, d_front(numElements.d_i + 1)
, d_back(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(EXTRA_CAPACITY <= d_size);
    BSLS_ASSERT(d_allocator_p);

    d_array_p = (void **)d_allocator_p->allocate(d_size * sizeof *d_array_p);
}

bdec_VoidPtrQueue::bdec_VoidPtrQueue(void * const    *srcArray,
                                     int              numElements,
                                     bslma_Allocator *basicAllocator)
: d_back(numElements)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(srcArray);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(d_allocator_p);

    d_size = calculateSufficientSize(numElements, INITIAL_SIZE);
    d_front = d_size - 1;
    d_array_p = (void **)d_allocator_p->allocate(d_size * sizeof *d_array_p);
    bsl::memcpy(d_array_p, srcArray, numElements * sizeof *d_array_p);
}

bdec_VoidPtrQueue::bdec_VoidPtrQueue(const bdec_VoidPtrQueue&  original,
                                     bslma_Allocator          *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(d_allocator_p);

    d_size = calculateSufficientSize(original.length(), INITIAL_SIZE);
    d_array_p = (void **)d_allocator_p->allocate(d_size * sizeof *d_array_p);
    d_front = d_size - 1;
    copyData(d_array_p,
             &d_back,
             d_size,
             d_front,
             original.d_array_p,
             original.d_size,
             original.d_front,
             original.d_back);
}

bdec_VoidPtrQueue::~bdec_VoidPtrQueue()
{
    // Verify object invariants.

    BSLS_ASSERT(d_array_p);
    BSLS_ASSERT(INITIAL_SIZE <= d_size);
    BSLS_ASSERT(0 <= d_front);
    BSLS_ASSERT(0 <= d_back);
    BSLS_ASSERT(d_front != d_back);
    BSLS_ASSERT(d_front < d_size);
    BSLS_ASSERT(d_back < d_size);
    BSLS_ASSERT(d_allocator_p);

    d_allocator_p->deallocate(d_array_p);
}

// MANIPULATORS

bdec_VoidPtrQueue& bdec_VoidPtrQueue::operator=(const bdec_VoidPtrQueue& rhs)
{
    if (this != &rhs) {
        const int newSize = calculateSufficientSize(rhs.length(),
                                                    INITIAL_SIZE);
        if (newSize > d_size) {
            void **array =
                  (void **)d_allocator_p->allocate(newSize *
                                                    sizeof *d_array_p);
            d_allocator_p->deallocate(d_array_p);
            d_array_p = array;
            d_size = newSize;
        }
        copyData(d_array_p,
                 &d_back,
                 d_size,
                 d_front,
                 rhs.d_array_p,
                 rhs.d_size,
                 rhs.d_front,
                 rhs.d_back);
    }
    return *this;
}

void bdec_VoidPtrQueue::append(const bdec_VoidPtrQueue& srcQueue)
{
    const int numElements = srcQueue.length();
    const int newLength = length() + numElements;
    const int minSize = calculateSufficientSize(newLength, d_size);
    if (d_size < minSize) {
        d_size = increaseSizeImp(&d_array_p,
                                 &d_front,
                                 &d_back,
                                 minSize,
                                 d_size,
                                 d_allocator_p);
    }
    d_back = memcpyCircular(d_array_p,
                            d_size,
                            d_back,
                            srcQueue.d_array_p,
                            srcQueue.d_size,
                            (srcQueue.d_front + 1) % srcQueue.d_size,
                            numElements);
}

void bdec_VoidPtrQueue::append(const bdec_VoidPtrQueue& srcQueue,
                              int srcIndex,
                              int numElements)
{
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(srcIndex + numElements <= srcQueue.length());

    const int newLength = length() + numElements;
    const int minSize = calculateSufficientSize(newLength, d_size);
    if (d_size < minSize) {
        d_size = increaseSizeImp(&d_array_p,
                                 &d_front,
                                 &d_back,
                                 minSize,
                                 d_size,
                                 d_allocator_p);
    }
    d_back = memcpyCircular(d_array_p,
                            d_size,
                            d_back,
                            srcQueue.d_array_p,
                            srcQueue.d_size,
                            (srcQueue.d_front + 1 + srcIndex) %
                                srcQueue.d_size,
                            numElements);
}

void bdec_VoidPtrQueue::insert(int dstIndex, void *item)
{
    BSLS_ASSERT(0 <= dstIndex);

    // The capacity must always be greater than or equal to
    // length + EXTRA_CAPACITY.

    const int originalLength = length();
    const int newLength = originalLength + 1;
    const int newSize = calculateSufficientSize(newLength, d_size);

    if (d_size < newSize) {
        // resize, makes move easy
        void **array =
                (void **)d_allocator_p->allocate(newSize * sizeof *d_array_p);

        // COMMIT

        const int start = d_front + 1;

        // NOTE: newSize >= size + 1 so '% newSize' is not needed in next line.
        memcpyCircular(array,
                       newSize,
                       start,           // no '% newSize'
                       d_array_p,
                       d_size,
                       start % d_size,
                       dstIndex);
        memcpyCircular(array,
                       newSize,
                       (start + dstIndex + 1) % newSize,
                       d_array_p,
                       d_size,
                       (start + dstIndex) % d_size,
                       originalLength - dstIndex);

        d_allocator_p->deallocate(d_array_p);
        d_array_p = array;

        d_size = newSize;
        d_back = (start + newLength) % d_size;
        d_array_p[(start + dstIndex) % d_size] = item;
    }
    else {  // sufficient capacity

        // No resize is required.  Copy as few elements as possible.

        // Compute number of elements that are past the insertion point: the
        // back length.

        const int backLen = originalLength - dstIndex;

        if (dstIndex < backLen) {

            // We will choose to shift 'dstIndex' elements to the left.

            const int src = (d_front + 1) % d_size;
            const int dst = d_front;

            memShiftLeft(d_array_p, d_size, dst, src, dstIndex);
            d_array_p[(d_front + dstIndex) % d_size] = item;
            d_front = (d_front - 1 + d_size) % d_size;
        }
        else {

            // We will choose to shift 'backLen' elements to the right.

            const int src = (d_front + 1 + dstIndex) % d_size;
            const int dst = (src + 1) % d_size;

            memShiftRight(d_array_p,
                          d_size,
                          dst,
                          src,
                          backLen);
            d_array_p[(d_front + 1 + dstIndex) % d_size] = item;
            d_back = (d_back + 1) % d_size;
        }
    }
}

void bdec_VoidPtrQueue::insert(int                    dstIndex,
                             const bdec_VoidPtrQueue& srcQueue,
                             int                    srcIndex,
                             int                    numElements)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(srcIndex + numElements <= srcQueue.length());

    // The capacity must always be greater than or equal to
    // length + EXTRA_CAPACITY.

    const int originalLength = length();
    const int newLength = originalLength + numElements;
    const int newSize = calculateSufficientSize(newLength, d_size);

    if (d_size < newSize) {
        // resize, makes move easy
        void **array =
                (void **)d_allocator_p->allocate(newSize * sizeof *d_array_p);

        // COMMIT

        const int start = d_front + 1;
        const int startIndex = start + dstIndex;

        // NOTE: newSize >= size + 1 so '% newSize' is not needed in next line.
        memcpyCircular(array,
                       newSize,
                       start,           // no '% newSize'
                       d_array_p,
                       d_size,
                       start % d_size,
                       dstIndex);
        memcpyCircular(array,
                       newSize,
                       (startIndex + numElements) % newSize,
                       d_array_p,
                       d_size,
                       (startIndex) % d_size,
                       originalLength - dstIndex);
        memcpyCircular(array,
                       newSize,
                       startIndex % newSize,
                       srcQueue.d_array_p,
                       srcQueue.d_size,
                       (srcQueue.d_front + 1 + srcIndex) % srcQueue.d_size,
                       numElements);
        d_allocator_p->deallocate(d_array_p);
        d_array_p = array;
        d_size = newSize;
        d_back = (start + newLength) % d_size;
    }
    else { // sufficient capacity

        // No resize is required.  Copy as few elements as possible.

        // Compute number of elements that are past the insertion point: the
        // back length.

        const int backLen = originalLength - dstIndex;
        if (dstIndex < backLen) {

            // We will shift 'dstIndex' elements to the left.

            const int d = (d_front + 1 - numElements + d_size) % d_size;
            memShiftLeft(d_array_p,
                         d_size,
                         d,
                         (d_front + 1) % d_size,
                         dstIndex);

            if (this != &srcQueue || srcIndex >= dstIndex) {  // not aliased
                memcpyCircular(d_array_p,
                               d_size,
                               (d + dstIndex) % d_size,
                               srcQueue.d_array_p,
                               srcQueue.d_size,
                               (srcQueue.d_front + 1 + srcIndex) %
                                                               srcQueue.d_size,
                               numElements);
            }
            else { // aliased
                const int distance = dstIndex - srcIndex;
                if (distance >= numElements) {
                    memcpyCircular(d_array_p,
                                   d_size,
                                   (d + dstIndex) % d_size,
                                   d_array_p,
                                   d_size,
                                   (d + srcIndex) % d_size,
                                   numElements);
                }
                else {
                    memcpyCircular(d_array_p,
                                   d_size,
                                   (d + dstIndex) % d_size,
                                   d_array_p,
                                   d_size,
                                   (d + srcIndex) % d_size,
                                   distance);
                    memcpyCircular(d_array_p,
                                   d_size,
                                   (d + dstIndex + distance) % d_size,
                                   d_array_p,
                                   d_size,
                                   (d_front + 1 + dstIndex) % d_size,
                                   numElements - distance);
                }
            }
            d_front = (d_front - numElements + d_size) % d_size;
        }
        else {

            // We will shift 'backLen' elements to the right.

            // Destination index is as close or closer to the back as to the
            // front.

            const int s = (d_front + 1 + dstIndex) % d_size;
            memShiftRight(d_array_p,
                          d_size,
                          (s + numElements) % d_size,
                          s,
                          backLen);

            if (this != &srcQueue ||
                           srcIndex + numElements <= dstIndex) { // not aliased
                memcpyCircular(d_array_p,
                               d_size,
                               s,
                               srcQueue.d_array_p,
                               srcQueue.d_size,
                               (srcQueue.d_front + 1 + srcIndex) %
                                                               srcQueue.d_size,
                               numElements);
            }
            else { // aliased
                if (dstIndex <= srcIndex) {
                    memcpyCircular(d_array_p,
                                   d_size,
                                   s,
                                   d_array_p,
                                   d_size,
                                   (d_front + 1 + srcIndex + numElements) %
                                                                        d_size,
                                   numElements);
                }
                else {
                    const int distance = dstIndex - srcIndex;
                    memcpyCircular(d_array_p,
                                   d_size,
                                   s,
                                   d_array_p,
                                   d_size,
                                   (d_front + 1 + srcIndex) % d_size,
                                   distance);
                    memcpyCircular(d_array_p,
                                   d_size,
                                   (s + distance) % d_size,
                                   d_array_p,
                                   d_size,
                                   (d_front + 1 + srcIndex + distance +
                                                         numElements) % d_size,
                                   numElements - distance);
                }
            }
            d_back = (d_back + numElements) % d_size;
        }
    }
}

void bdec_VoidPtrQueue::remove(int index)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(index < length());

    // Compute number of elements that are past the insertion point: the
    // back length.

    const int backLen =
                 (d_back - d_front - EXTRA_CAPACITY - index + d_size) % d_size;
    if (index < backLen) {
        d_front = (d_front + 1) % d_size;
        memShiftRight(d_array_p,
                      d_size,
                      (d_front + 1) % d_size,
                      d_front,
                      index);
    }
    else {
        const int d = (d_front + 1 + index) % d_size;
        memShiftLeft(d_array_p,
                     d_size,
                     d,
                     (d + 1) % d_size,
                     (d_back + d_size - d_front - 1) % d_size - 1 - index);
        d_back = (d_back - 1 + d_size) % d_size;
    }
}

void bdec_VoidPtrQueue::remove(int index, int numElements)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(index + numElements <= length());

    // Compute number of elements that are past the insertion point: the
    // back length.

    const int backLen = (d_back - d_front - 1
                                      - index - numElements + d_size) % d_size;
    if (index < backLen) {
        const int dst = (d_front + 1 + numElements) % d_size;
        const int src = (d_front + 1) % d_size;

        memShiftRight(d_array_p, d_size, dst, src, index);
        d_front = (d_front + numElements) % d_size;
    }
    else {
        const int dst = (d_front + 1 + index) % d_size;
        const int src = (dst + numElements) % d_size;

        memShiftLeft(d_array_p,
                     d_size,
                     dst,
                     src,
                     (d_back + d_size - d_front - 1) % d_size -
                                                          numElements - index);
        d_back = (d_back - numElements + d_size) % d_size;
    }
}

void bdec_VoidPtrQueue::replace(int                    dstIndex,
                              const bdec_VoidPtrQueue& srcQueue,
                              int                    srcIndex,
                              int                    numElements)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(dstIndex + numElements <= length());
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(srcIndex + numElements <= srcQueue.length());

    if (this != &srcQueue || srcIndex + numElements <= dstIndex ||
                          dstIndex + numElements <= srcIndex) {  // not aliased
        memcpyCircular(d_array_p,
                       d_size,
                       (d_front + 1 + dstIndex) % d_size,
                       srcQueue.d_array_p,
                       srcQueue.d_size,
                       (srcQueue.d_front + 1 + srcIndex) % srcQueue.d_size,
                       numElements);
    }
    else {  // aliased; do nothing if srcIndex == dstIndex
        if (srcIndex < dstIndex) {
            memShiftRight(d_array_p,
                          d_size,
                          (d_front + 1 + dstIndex) % d_size,
                          (d_front + 1 + srcIndex) % d_size,
                          numElements);
        }
        else if (srcIndex > dstIndex) {
            memShiftLeft(d_array_p,
                         d_size,
                         (d_front + 1 + dstIndex) % d_size,
                         (d_front + 1 + srcIndex) % d_size,
                         numElements);
        }
    }
}

void bdec_VoidPtrQueue::reserveCapacity(int numElements)
{
    BSLS_ASSERT(0 <= numElements);

    const int newSize = calculateSufficientSize(numElements, d_size);
    if (d_size < newSize) {
        d_size = increaseSizeImp(&d_array_p,
                                 &d_front,
                                 &d_back,
                                 newSize,
                                 d_size,
                                 d_allocator_p);

        // To improve testability, all empty queues have canonical front and
        // back values.

        if (0 == length()) {
            d_front = d_size - 1;
            d_back = 0;
        }
    }
}

void bdec_VoidPtrQueue::reserveCapacityRaw(int numElements)
{
    BSLS_ASSERT(0 <= numElements);

    const int newSize = numElements + EXTRA_CAPACITY; // to hold the front/back
                                                      // positions
    if (d_size < newSize) {
        d_size = increaseSizeImp(&d_array_p,
                                 &d_front,
                                 &d_back,
                                 newSize,
                                 d_size,
                                 d_allocator_p);
    }
}

void bdec_VoidPtrQueue::setLength(int newLength)
{
    BSLS_ASSERT(0 <= newLength);

    const int newSize = newLength + EXTRA_CAPACITY; // to hold the front/back
                                                    // positions
    if (d_size < newSize) {
        d_size = increaseSizeImp(&d_array_p,
                                 &d_front,
                                 &d_back,
                                 newSize,
                                 d_size,
                                 d_allocator_p);
    }
    const int oldBack = d_back;
    const int oldLength = length();
    BSLS_ASSERT(oldBack == (d_front + 1 + oldLength) % d_size);
    d_back = (d_front + 1 + newLength) % d_size;
    if (newLength > oldLength) {
        if (oldBack < d_back) {
            bsl::memset(d_array_p + oldBack,
                        0,
                        (d_back - oldBack) * sizeof(void *));
        }
        else {
            bsl::memset(d_array_p + oldBack,
                        0,
                        (d_size - oldBack) * sizeof(void *));
            bsl::memset(d_array_p, 0, d_back * sizeof(void *));
        }
    }
}

void bdec_VoidPtrQueue::setLength(int newLength, void *initialValue)
{
    BSLS_ASSERT(0 <= newLength);

    const int newSize = newLength + EXTRA_CAPACITY; // to hold the empty
                                                    // positions
    if (d_size < newSize) {
        d_size = increaseSizeImp(&d_array_p,
                                 &d_front,
                                 &d_back,
                                 newSize,
                                 d_size,
                                 d_allocator_p);
    }
    const int oldBack = d_back;
    const int oldLength = length();
    d_back = (d_front + 1 + newLength) % d_size;
    if (newLength > oldLength) {
        if (oldBack < d_back) {
            bslalg_ArrayPrimitives::uninitializedFillN(d_array_p + oldBack,
                                                       d_back - oldBack,
                                                       initialValue,
                                                       d_allocator_p);
        }
        else {
            bslalg_ArrayPrimitives::uninitializedFillN(d_array_p + oldBack,
                                                       d_size - oldBack,
                                                       initialValue,
                                                       d_allocator_p);
            bslalg_ArrayPrimitives::uninitializedFillN(d_array_p,
                                                       d_back,
                                                       initialValue,
                                                       d_allocator_p);
        }
    }
}

void bdec_VoidPtrQueue::setLengthRaw(int newLength)
{
    BSLS_ASSERT(0 <= newLength);

    const int newSize = newLength + EXTRA_CAPACITY; // to hold the empty
                                                    // positions
    if (d_size < newSize) {
        d_size = increaseSizeImp(&d_array_p,
                                 &d_front,
                                 &d_back,
                                 newSize,
                                 d_size,
                                 d_allocator_p);
    }
    d_back = (d_front + 1 + newLength) % d_size;
}

// ACCESSORS

bsl::ostream& bdec_VoidPtrQueue::print(bsl::ostream& stream,
                                int      level,
                                int      spacesPerLevel) const
{
    BSLS_ASSERT(0 <= spacesPerLevel);

    indent(stream, level, spacesPerLevel);
    stream << "[\n";

    if (level < 0) {
        level = -level;
    }

    int levelPlus1 = level + 1;

    const int len = length();
    for (int i = 0; i < len; ++i) {
        indent(stream, levelPlus1, spacesPerLevel);
        bdeu_Print::printPtr(stream,
                                 d_array_p[(i + d_front + 1) % d_size]);
        stream << '\n';
    }

    indent(stream, level, spacesPerLevel);
    return stream << "]\n";
}

// FREE OPERATORS

bool operator==(const bdec_VoidPtrQueue& lhs, const bdec_VoidPtrQueue& rhs)
{
    const int len = lhs.length();
    if (rhs.length() != len) {
        return 0;
    }

    // Lengths are equal.

    void **lhsStart = lhs.d_array_p + lhs.d_front + 1;
    void **rhsStart = rhs.d_array_p + rhs.d_front + 1;
    if (lhs.d_back > lhs.d_front) {
        if (rhs.d_back > rhs.d_front) {

            // lhs is stored linearly, rhs is stored linearly.

            return !bsl::memcmp(lhsStart,
                                rhsStart,
                                len * sizeof *lhs.d_array_p);
        }
        else {

            // lhs is stored linearly, rhs is wrapped.

            const int rhsStartLen = rhs.d_size - rhs.d_front - 1;
            if (bsl::memcmp(lhsStart,
                            rhsStart,
                            rhsStartLen * sizeof *lhs.d_array_p)) {
                return 0;
            }
            return !bsl::memcmp(lhsStart + rhsStartLen,
                                rhs.d_array_p,
                                rhs.d_back * sizeof *lhs.d_array_p);
        }
    }
    else { // lhs.d_back <= lhs.d_front
        if (rhs.d_back > rhs.d_front) {

            // lhs is wrapped, rhs is stored linearly.

            const int lhsStartLen = lhs.d_size - lhs.d_front - 1;
            if (bsl::memcmp(lhsStart,
                            rhsStart,
                            lhsStartLen * sizeof *lhs.d_array_p)) {
                return 0;
            }
            return !bsl::memcmp(lhs.d_array_p,
                                rhsStart + lhsStartLen,
                                lhs.d_back * sizeof *lhs.d_array_p);
        }
        else {

            // lhs is wrapped, rhs is wrapped.

            const int lhsStartLen = lhs.d_size - lhs.d_front - 1;
            const int rhsStartLen = rhs.d_size - rhs.d_front - 1;
            if (lhsStartLen > rhsStartLen) {

                // lhs first piece longer.

                const int startLengthDiff = lhsStartLen - rhsStartLen;
                if (bsl::memcmp(lhsStart,
                                rhsStart,
                                rhsStartLen * sizeof *lhs.d_array_p)) {
                    return 0;
                }
                if (bsl::memcmp(lhsStart + rhsStartLen,
                                rhs.d_array_p,
                                startLengthDiff * sizeof *lhs.d_array_p)) {
                    return 0;
                }
                return !bsl::memcmp(lhs.d_array_p,
                                    rhs.d_array_p + startLengthDiff,
                                    lhs.d_back * sizeof *lhs.d_array_p);
            }
            else {

                // rhs first piece longer.

              const int startLengthDiff = rhsStartLen - lhsStartLen;
              if (bsl::memcmp(lhsStart,
                              rhsStart,
                              lhsStartLen * sizeof *lhs.d_array_p)) {
                  return 0;
              }
              if (bsl::memcmp(lhs.d_array_p,
                              rhsStart + lhsStartLen,
                              startLengthDiff * sizeof *lhs.d_array_p)) {
                  return 0;
              }
              return !bsl::memcmp(lhs.d_array_p + startLengthDiff,
                                  rhs.d_array_p,
                                  rhs.d_back * sizeof *lhs.d_array_p);
            }
        }
    }
}

bsl::ostream& operator<<(bsl::ostream& stream, const bdec_VoidPtrQueue& rhs)
{
    stream << '[';
    for (int i = 0; i < rhs.length(); ++i) {
        stream << ' ';
        bdeu_Print::printPtr(stream, rhs[i]);
    }
    return stream << " ]" << bsl::flush;
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
