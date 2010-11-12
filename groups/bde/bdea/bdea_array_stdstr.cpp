// bdea_array_stdstr.cpp     -*-C++-*-

#include <bdea_array_stdstr.h>

#include <bdes_platformutil.h>                  // for testing only
#include <bdes_platform.h>
#include <bdema_strallocator.h>
#include <bdema_autodeleter.h>
#include <bdema_autodestructor.h>
#include <bdema_autodeallocator.h>
#include <bdema_default.h>
#include <bdeu_print.h>
#include <bdealg_scalarprimitives.h>
#include <bdemf_assert.h>

#include <cassert>
#include <cstring>     // memmove(), memcpy()
#include <ostream>
#include <string>

namespace BloombergLP {

                        // ---------
                        // CONSTANTS
                        // ---------
enum {
    INITIAL_SIZE = 1,     // initial physical capacity (number of elements)
    GROW_FACTOR = 2       // multiplicative factor by which to grow 'd_size'
};

                        // --------------
                        // STATIC METHODS
                        // --------------

static inline void cleanup(std::string *array, int index, int length)
    // Runs the destructor on a segment of an array of elements.  This
    // function is only called when the hint is not used.  All elements have
    // been initialized.
{
    const int finalElement = index + length;
    for (int i = index; i < finalElement; ++i) {
        bdealg_ScalarPrimitives::destruct(&array[i], (bdema_Allocator*) 0);
    }
}

inline
static int nextSize(int size)
    // Return the specified 'size' multiplied by the constant 'GROW_FACTOR'.
{
    return size * GROW_FACTOR;
}

inline
static int calculateSufficientSize(int minLength, int size)
    // Geometrically grow the specified current 'size' value while it is less
    // than the specified 'minLength' value.  Return the new size value.  The
    // behavior is undefined unless 1 <= size and 0 <= minLength.  Note that
    // if minLength <= size then 'size' is returned.
{
    assert(1 <= size);
    assert(0 <= minLength);

    while (size < minLength) {
        size = nextSize(size);
    };

    assert(size >= minLength);
    return size;
}

inline
static void reallocate(std::string    **array,
                       int             *size,
                       int              newSize,
                       int              length,
                       bdema_Allocator *basicAllocator)
    // Reallocate memory in the specified 'array' and update the specified size
    // to the specified 'newSize' using the specified 'basicAllocator'.  The
    // specified 'length' number of leading elements are preserved.  If the
    // 'basicAllocator' should throw an exception, this function has no effect.
    // The behavior is undefined unless 1 <= newSize, 0 <= length, and
    // newSize <= length.
{
    assert(array);
    assert(*array);             // this is not 'allocate'
    assert(size);
    assert(1 <= newSize);
    assert(0 <= length);
    assert(basicAllocator);
    assert(length <= *size);    // sanity check
    assert(length <= newSize);  // ensure class invariant

    std::string *tmp = *array;

    *array = (std::string *)
              basicAllocator->allocate(newSize * sizeof **array);
    // COMMIT
    std::memcpy(*array, tmp, length * sizeof **array);
    *size = newSize;
    basicAllocator->deallocate(tmp);
}


inline
static void initializeWithDefaultValue(std::string     *array,
                                       int              length,
                                       bdema_Allocator *d_strAllocator_p)
    // Initialize the specified 'length' leading elements of the specified
    // array with the default value.  The behavior is undefined unless
    // 0 <= length.
{
    assert(array);
    assert(0 <= length);

    bdema_AutoDestructor<std::string> autoDtor(array, 0);

    for (int i = 0; i < length; ++i) {
         new (array + i) std::string(d_strAllocator_p);
         ++autoDtor;
    }

    autoDtor.release();
}

inline
static void initializeWithValue(std::string        *array,
                                const std::string&  value,
                                int                 numElements,
                                bdema_Allocator    *d_strAllocator_p)
    // The array pointed to by 'array' is initialized to the value 'value'.
    // 'numElements' is the number of values to initialize.  It is possible for
    // the 'std::string' constructor to throw an exception.  On exception, the
    // destructor will be run for all elements already initialized.
{
    assert(array);
    assert(0 <= numElements);

    bdema_AutoDestructor<std::string> autoDtor(array, 0);

    std::string *endPtr = array + numElements;
    for (std::string *d = array; d < endPtr; ++d) {
         new (d) std::string(value, d_strAllocator_p);
         ++autoDtor;
    }

    autoDtor.release();
}

inline
static void elementAssign(std::string *dstArray,
                          int          dstIndex,
                          std::string *srcArray,
                          int          srcIndex,
                          int          numElements)
    // A contiguous set of 'std::string's is assigned using the assignment
    // operator.  It is assumed that the destination strings have been
    // initialized and that the array has enough size.  To cope with the case
    // of overlapping arrays (aliasing), the assign is performed either
    // ascending or descending.  An exception can occur in the 'std::string'
    // assignment operator.  If an exception occurs, the array will be in a
    // consistent state, but the number and values of the strings in the array
    // (though valid) are undefined.
{
    assert(dstArray);
    assert(0 <= dstIndex);
    assert(srcArray);
    assert(0 <= srcIndex);
    assert(0 <= numElements);

    if (&dstArray[dstIndex] <= &srcArray[srcIndex]) {
        if (&dstArray[dstIndex] != &srcArray[srcIndex]) {
            std::string *s = &srcArray[srcIndex];
            std::string *endPtr = &dstArray[dstIndex + numElements];

            for (std::string *d = &dstArray[dstIndex]; d < endPtr; ++s, ++d) {
                *d = *s;
            }
        }
    }
    else {
        const int numElementsMinusOne = numElements - 1;
        std::string *endPtr = &dstArray[dstIndex];
        std::string *s = &srcArray[srcIndex + numElementsMinusOne];

        for (std::string *d = &dstArray[dstIndex + numElementsMinusOne];
             d >= endPtr; --s, --d) {
            *d = *s;
        }
    }
}

inline
static void elementInitialize(std::string     *dstArray,
                              int             *currentLength,
                              std::string     *srcArray,
                              int              srcIndex,
                              int              numElements,
                              bdema_Allocator *d_strAllocator_p)
    // Run the constructor on a contiguous segment of 'std::string' elements
    // using an array of elements to initialize.  'currentLength' is the index
    // of the first element to initialize.  'currentLength' is incremented by
    // 1 for each value initialized.  If an exception occurs in the
    // constructor, 'currentLength' is set appropriately for the number of
    // items initialized.  It is assumed that the size of the array is
    // sufficient to store all the elements.
{
    assert(dstArray);
    assert(currentLength);
    assert(0 <= *currentLength);
    assert(srcArray);
    assert(0 <= srcIndex);
    assert(0 <= numElements);

    std::string *endPtr = srcArray + srcIndex + numElements;
    for (std::string *s = srcArray + srcIndex;
         s < endPtr;
         ++*currentLength, ++s) {
        new (dstArray + *currentLength) std::string(*s, d_strAllocator_p);
    }
}



                        // -------------
                        // CLASS METHODS
                        // -------------

int bdea_Array<std::string>::maxSupportedBdexVersion()
{
    return 1;
}

int bdea_Array<std::string>::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}



                        // ---------------
                        // PRIVATE METHODS
                        // ---------------

void bdea_Array<std::string>::appendImp(const std::string& item)
{
    assert(d_length == d_size);

    // This function implements an append function for the case when there
    // is insufficient memory in the array for an additional item.  Note that
    // this method is called by the inline 'append' function.

    std::string *tmp = d_array_p;
    int newSize = nextSize(d_size);
    d_array_p = (std::string *)
                d_allocator_p->allocate(newSize * sizeof *d_array_p);
    d_size = newSize;
    std::memcpy(d_array_p, tmp, d_length * sizeof *d_array_p);

    // In order to support aliasing, tmp is not deleted until after
    // the item has been appended to the array.  The old array 'tmp' will be
    // released automatically when the function returns or when an exception
    // occurs, thus making this function exception neutral.

    bdema_AutoDeallocator<bdema_Allocator> autoDealloc(tmp, d_allocator_p);

    new (d_array_p + d_length) std::string(item, d_strAllocator_p);
    ++d_length;
}

void bdea_Array<std::string>::reserveCapacityImp(int numElements)
{
    assert(0 <= numElements);

    // This method is called by the inline reserveCapacity() when the current
    // capacity in insufficient.  The size allocated will be >= numElements

    int newSize = calculateSufficientSize(numElements, d_size);
    reallocate(&d_array_p, &d_size, newSize, d_length, d_allocator_p);
}

                        // --------
                        // CREATORS
                        // --------

bdea_Array<std::string>::bdea_Array(bdema_Allocator *basicAllocator)
: d_size(INITIAL_SIZE)
, d_length(0)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    assert(d_allocator_p);

    d_strAllocator_p = d_allocator_p;

    d_array_p = (std::string *)
                           d_allocator_p->allocate(d_size * sizeof *d_array_p);
}

bdea_Array<std::string>::bdea_Array(const Explicit&  initialLength,
                                    bdema_Allocator *basicAllocator)
: d_length(initialLength.d_i)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    assert(0 <= d_length);
    assert(d_allocator_p);

    d_strAllocator_p = d_allocator_p;

    d_size = calculateSufficientSize(d_length, INITIAL_SIZE);
    assert(d_length <= d_size);

    d_array_p = (std::string *)
                           d_allocator_p->allocate(d_size * sizeof *d_array_p);

    // It is possible for initializeWithDefaultValue() to throw an exception;
    // if it does, the array is automatically deallocated.
    // initializeWithValue() takes care of running the destructor on array
    // elements.

    bdema_AutoDeallocator<bdema_Allocator> autoDealloc(d_array_p,
                                                       d_allocator_p);

    initializeWithDefaultValue(d_array_p, d_length, d_strAllocator_p);

    autoDealloc.release();
}

bdea_Array<std::string>::bdea_Array(const Explicit&     initialLength,
                                    const std::string&  initialValue,
                                    bdema_Allocator    *basicAllocator)
: d_length(initialLength.d_i)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    assert(0 <= d_length);
    assert(d_allocator_p);

    d_strAllocator_p = d_allocator_p;

    d_size = calculateSufficientSize(d_length, INITIAL_SIZE);
    assert(d_length <= d_size);

    d_array_p = (std::string *)
                d_allocator_p->allocate(d_size * sizeof *d_array_p);

    // It is possible for initializeWithValue() to throw an exception;
    // if it does, the array is automatically deallocated.
    // initializeWithValue() takes care of running the destructor on array
    // elements.

    bdema_AutoDeallocator<bdema_Allocator> autoDealloc(d_array_p,
                                                       d_allocator_p);

    initializeWithValue(d_array_p, initialValue, d_length, d_strAllocator_p);

    autoDealloc.release();
}

bdea_Array<std::string>::bdea_Array(const InitialCapacity&  numElements,
                                    bdema_Allocator        *basicAllocator)
: d_size(numElements.d_i <= 0 ? INITIAL_SIZE : numElements.d_i)
, d_length(0)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    assert(1 <= d_size);
    assert(d_allocator_p);

    d_strAllocator_p = d_allocator_p;

    d_array_p = (std::string *)
                           d_allocator_p->allocate(d_size * sizeof *d_array_p);
}

bdea_Array<std::string>::bdea_Array(const char      **srcCArray,
                                    int               numElements,
                                    bdema_Allocator  *basicAllocator)
: d_length(numElements)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    assert(srcCArray);
    assert(0 <= numElements);
    assert(d_allocator_p);

    d_strAllocator_p = d_allocator_p;

    d_size = calculateSufficientSize(d_length, INITIAL_SIZE);
    d_array_p = (std::string *)
                d_allocator_p->allocate(d_size * sizeof *d_array_p);

    // The 'std::string' constructor can throw an exception; if it does, we
    // must run the destructor on elements that have already been initialized
    // followed by a deallocate on the array itself.  Note the order:

    bdema_AutoDeallocator<bdema_Allocator> autoDealloc(d_array_p,
                                                       d_allocator_p);

    bdema_AutoDestructor<std::string> autoDtor(d_array_p, 0); // destroyed 1st

    for (int i = 0; i < d_length; ++i) {
         new (d_array_p + i) std::string(srcCArray[i], d_strAllocator_p);
         ++autoDtor;
    }

    autoDtor.release();
    autoDealloc.release();
}

bdea_Array<std::string>::
                     bdea_Array(const bdea_Array<std::string>&  original,
                                bdema_Allocator                *basicAllocator)
: d_length(original.d_length)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    assert(d_allocator_p);

    d_strAllocator_p = d_allocator_p;

    d_size = calculateSufficientSize(d_length, INITIAL_SIZE);

    d_array_p = (std::string *)
                           d_allocator_p->allocate(d_size * sizeof *d_array_p);

    // The 'std::string' constructor can throw an exception; if it does, we
    // must run the destructor on elements that have already been initialized
    // followed by a deallocate on the array itself.

    bdema_AutoDeallocator<bdema_Allocator> autoDealloc(d_array_p,
                                                       d_allocator_p);

    bdema_AutoDestructor<std::string> autoDtor(d_array_p, 0);
    std::string *origPtr = original.d_array_p;

    for (int i = 0; i < d_length; ++i) {
         new (d_array_p + i) std::string(origPtr[i], d_strAllocator_p);
         ++autoDtor;
    }

    autoDtor.release();
    autoDealloc.release();
}

bdea_Array<std::string>::bdea_Array(Hint             infrequentDeleteHint,
                                    bdema_Allocator *basicAllocator)
: d_size(INITIAL_SIZE)
, d_length(0)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    assert(d_allocator_p);

    // The optional string allocator is automatically destroyed and deallocated
    // in the case of an exception.

    bdema_Allocator *deallocatorPtr = 0;
    bdema_AutoDeleter<bdema_Allocator, bdema_Allocator>
                                autoDeleter(&deallocatorPtr, 0, d_allocator_p);

    if (INFREQUENT_DELETE_HINT == infrequentDeleteHint) {
        deallocatorPtr = d_strAllocator_p =
                        new (*d_allocator_p) bdema_StrAllocator(d_allocator_p);
        ++autoDeleter;
    }
    else {
        d_strAllocator_p = d_allocator_p;
    }

    d_array_p = (std::string *)
                d_allocator_p->allocate(d_size * sizeof *d_array_p);

    autoDeleter.release();
}

bdea_Array<std::string>::bdea_Array(const Explicit&  initialLength,
                                    Hint             infrequentDeleteHint,
                                    bdema_Allocator *basicAllocator)
: d_length(initialLength.d_i)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    assert(0 <= d_length);
    assert(d_allocator_p);

    // The optional string allocator is automatically destroyed and deallocated
    // in the case of an exception.

    bdema_Allocator *deallocatorPtr = 0;
    bdema_AutoDeleter<bdema_Allocator, bdema_Allocator>
                                autoDeleter(&deallocatorPtr, 0, d_allocator_p);

    if (INFREQUENT_DELETE_HINT == infrequentDeleteHint) {
        deallocatorPtr = d_strAllocator_p =
                        new (*d_allocator_p) bdema_StrAllocator(d_allocator_p);
        ++autoDeleter;
    }
    else {
        d_strAllocator_p = d_allocator_p;
    }

    d_size = calculateSufficientSize(d_length, INITIAL_SIZE);
    assert(d_length <= d_size);

    d_array_p = (std::string *)
                d_allocator_p->allocate(d_size * sizeof *d_array_p);

    bdema_AutoDeallocator<bdema_Allocator>
                                    autoDeallocArray(d_array_p, d_allocator_p);

    initializeWithDefaultValue(d_array_p, d_length, d_strAllocator_p);

    autoDeallocArray.release();
    autoDeleter.release();
}

bdea_Array<std::string>::bdea_Array(const Explicit&     initialLength,
                                    const std::string&  initialValue,
                                    Hint                infrequentDeleteHint,
                                    bdema_Allocator    *basicAllocator)
: d_length(initialLength.d_i)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    assert(0 <= d_length);
    assert(d_allocator_p);


    // The optional string allocator is automatically destroyed and deallocated
    // in the case of an exception.

    bdema_Allocator *deallocatorPtr = 0;
    bdema_AutoDeleter<bdema_Allocator, bdema_Allocator>
                                autoDeleter(&deallocatorPtr, 0, d_allocator_p);

    if (INFREQUENT_DELETE_HINT == infrequentDeleteHint) {
        deallocatorPtr = d_strAllocator_p =
                        new (*d_allocator_p) bdema_StrAllocator(d_allocator_p);
        ++autoDeleter;
    }
    else {
        d_strAllocator_p = d_allocator_p;
    }

    d_size = calculateSufficientSize(d_length, INITIAL_SIZE);
    assert(d_length <= d_size);

    d_array_p = (std::string *)
                           d_allocator_p->allocate(d_size * sizeof *d_array_p);

    bdema_AutoDeallocator<bdema_Allocator>
                                    autoDeallocArray(d_array_p, d_allocator_p);

    initializeWithValue(d_array_p, initialValue, d_length, d_strAllocator_p);

    autoDeallocArray.release();
    autoDeleter.release();
}

bdea_Array<std::string>::
                       bdea_Array(const InitialCapacity&  numElements,
                                  Hint                    infrequentDeleteHint,
                                  bdema_Allocator        *basicAllocator)
: d_size(numElements.d_i <= 0 ? INITIAL_SIZE : numElements.d_i)
, d_length(0)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    assert(1 <= d_size);
    assert(d_length <= d_size);
    assert(d_allocator_p);

    // The optional string allocator is automatically destroyed and deallocated
    // in the case of an exception.

    bdema_Allocator *deallocatorPtr = 0;
    bdema_AutoDeleter<bdema_Allocator, bdema_Allocator>
                                autoDeleter(&deallocatorPtr, 0, d_allocator_p);

    if (INFREQUENT_DELETE_HINT == infrequentDeleteHint) {
        deallocatorPtr = d_strAllocator_p =
                        new (*d_allocator_p) bdema_StrAllocator(d_allocator_p);
        ++autoDeleter;
    }
    else {
        d_strAllocator_p = d_allocator_p;
    }

    d_array_p = (std::string *)
                d_allocator_p->allocate(d_size * sizeof *d_array_p);

    autoDeleter.release();
}

bdea_Array<std::string>::bdea_Array(const char       **srcCArray,
                                    int                numElements,
                                    Hint               infrequentDeleteHint,
                                    bdema_Allocator   *basicAllocator)
: d_length(numElements)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    assert(srcCArray);
    assert(0 <= d_length);
    assert(d_allocator_p);

    // The optional string allocator is automatically destroyed and deallocated
    // in the case of an exception.

    bdema_Allocator *deallocatorPtr = 0;
    bdema_AutoDeleter<bdema_Allocator, bdema_Allocator>
                                autoDeleter(&deallocatorPtr, 0, d_allocator_p);

    if (INFREQUENT_DELETE_HINT == infrequentDeleteHint) {
        deallocatorPtr = d_strAllocator_p =
                        new (*d_allocator_p) bdema_StrAllocator(d_allocator_p);
        ++autoDeleter;
    }
    else {
        d_strAllocator_p = d_allocator_p;
    }

    d_size = calculateSufficientSize(d_length, INITIAL_SIZE);
    d_array_p = (std::string *)
                           d_allocator_p->allocate(d_size * sizeof *d_array_p);

    // The following order of construction (destruction) is significant.

    bdema_AutoDeallocator<bdema_Allocator> autoDeallocArray(d_array_p,
                                                            d_allocator_p);
    bdema_AutoDestructor<std::string> autoDtor(d_array_p, 0);

    for (int i = 0; i < d_length; ++i) {
        new (d_array_p + i) std::string(srcCArray[i], d_strAllocator_p);
        ++autoDtor;
    }

    autoDtor.release();
    autoDeallocArray.release();
    autoDeleter.release();
}

bdea_Array<std::string>::
               bdea_Array(const bdea_Array<std::string>&  original,
                          Hint                            infrequentDeleteHint,
                          bdema_Allocator                *basicAllocator)
: d_length(original.d_length)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    assert(d_allocator_p);

    // The optional string allocator is automatically destroyed and deallocated
    // in the case of an exception.

    bdema_Allocator *deallocatorPtr = 0;
    bdema_AutoDeleter<bdema_Allocator, bdema_Allocator>
                                autoDeleter(&deallocatorPtr, 0, d_allocator_p);

    if (INFREQUENT_DELETE_HINT == infrequentDeleteHint) {
        deallocatorPtr = d_strAllocator_p =
                        new (*d_allocator_p) bdema_StrAllocator(d_allocator_p);
        ++autoDeleter;
    }
    else {
        d_strAllocator_p = d_allocator_p;
    }

    d_size = calculateSufficientSize(d_length, INITIAL_SIZE);
    d_array_p = (std::string *)
                d_allocator_p->allocate(d_size * sizeof *d_array_p);

    // The following order of construction (destruction) is significant.

    bdema_AutoDeallocator<bdema_Allocator> autoDeallocArray(d_array_p,
                                                            d_allocator_p);
    bdema_AutoDestructor<std::string> autoDtor(d_array_p, 0);

    std::string *origPtr = original.d_array_p;
    for (int i = 0; i < d_length; ++i) {
         new (d_array_p + i)
             std::string(origPtr[i], d_strAllocator_p);
         ++autoDtor;
    }

    autoDtor.release();
    autoDeallocArray.release();
    autoDeleter.release();
}

bdea_Array<std::string>::~bdea_Array()
{
    assert(d_array_p);
    assert(1 <= d_size);
    assert(0 <= d_length);
    assert(d_allocator_p);
    assert(d_strAllocator_p);
    assert(d_length <= d_size);

    // If the allocator for string elements is different from the basic
    // allocator, it is a specialized string allocator, and memory for all
    // string elements can be deallocated just by destroying the string
    // allocator.

    if (d_strAllocator_p != d_allocator_p) {
        d_strAllocator_p->~bdema_Allocator();   // release all string data
        d_allocator_p->deallocate(d_strAllocator_p);
    }
    else {
         cleanup(d_array_p, 0, d_length);
    }

    d_allocator_p->deallocate(d_array_p);
}

                        // ------------
                        // MANIPULATORS
                        // ------------

bdea_Array<std::string>& bdea_Array<std::string>::
                                  operator=(const bdea_Array<std::string>& rhs)
{
    if (this != &rhs) {
        removeAll();
        int newLength = rhs.d_length;
        if (d_size < newLength) {
            // insufficient memory so reallocate the array.  Don't release
            // the old memory until after allocate succeeds in case of
            // exceptions.

            std::string *tmp = d_array_p;
            int newSize = calculateSufficientSize(newLength, d_size);
            d_array_p = (std::string *)
                        d_allocator_p->allocate(newSize * sizeof *d_array_p);
            // COMMIT
            d_size = newSize;
            d_allocator_p->deallocate(tmp);
        }
        assert(d_size >= newLength);
        std::string *s = rhs.d_array_p;
        std::string *endPtr = d_array_p + newLength;
        for (std::string *d = d_array_p; d < endPtr; ++d_length, ++s, ++d) {
             new (d) std::string(*s, d_strAllocator_p);
        }
    }
    return *this;
}

void bdea_Array<std::string>::append(const bdea_Array<std::string>& srcArray)
{
    int newLength = d_length + srcArray.d_length;
    if (d_size < newLength) {
        int newSize = calculateSufficientSize(newLength, d_size);
        reallocate(&d_array_p, &d_size, newSize, d_length, d_allocator_p);
    }

    std::string *s = srcArray.d_array_p;
    for (; d_length < newLength; ++d_length, ++s) {
         new (d_array_p + d_length) std::string(*s, d_strAllocator_p);
    }
}

void bdea_Array<std::string>::
                             append(const bdea_Array<std::string>& srcArray,
                                    int                            srcIndex,
                                    int                            numElements)
{
    assert(0 <= srcIndex);
    assert(0 <= numElements);
    assert(srcIndex + numElements <= srcArray.d_length);

    int newLength = d_length + numElements;
    if (d_size < newLength) {
        int newSize = calculateSufficientSize(newLength, d_size);
        reallocate(&d_array_p, &d_size, newSize, d_length, d_allocator_p);
    }
    assert(d_size >= newLength);
    elementInitialize(d_array_p, &d_length,
                      srcArray.d_array_p,
                      srcIndex, numElements,
                      d_strAllocator_p);
}

void bdea_Array<std::string>::insert(int dstIndex, const std::string& item)
{
    assert(0 <= dstIndex);  assert(dstIndex <= d_length);

    if (d_length >= d_size) {  // Need to resize.
        int newSize = nextSize(d_size);
        std::string *tmp = d_array_p;
        d_array_p = (std::string *)
                 d_allocator_p->allocate(newSize * sizeof *d_array_p);
        // COMMIT

        // tmp is automatically deallocated on normal return or exception.

        bdema_AutoDeallocator<bdema_Allocator> autoDeallocArray(tmp,
                                                                d_allocator_p);
        d_size = newSize;
        std::string *here = d_array_p + dstIndex;
        std::string *there = tmp + dstIndex;

        // If an exception occurs, the destructor is run on the end of the
        // array leaving the array truncated at the insertion point.

        bdema_AutoDestructor<std::string> autoDtor(here + 1,
                                                   d_length - dstIndex);
        int currentLength = d_length;
        d_length = dstIndex;

        std::memcpy(d_array_p, tmp, dstIndex * sizeof *d_array_p);
        std::memcpy(here + 1, there,
               (currentLength - dstIndex) * sizeof *d_array_p);
        new (here) std::string(item, d_strAllocator_p);

        autoDtor.release();
        d_length = currentLength + 1;
        return;                                                 // RETURN
    }

    assert(d_length < d_size);  // Enough capacity exists.

    std::string *here = d_array_p + dstIndex;
    std::memmove(here + 1, here, (d_length - dstIndex) * sizeof *d_array_p);

    // If an exception occurs the destructor is run on the end of the array
    // leaving the array truncated at the insertion point.

    bdema_AutoDestructor<std::string> autoDtor(here + 1,
                                               d_length - dstIndex);
    int currentLength = d_length;
    d_length = dstIndex;

    if (&item >= here && &item < d_array_p + currentLength)
    {
        // aliasing: item has moved one location
        new (here) std::string(*(&item + 1), d_strAllocator_p);
    }
    else {
        new (here) std::string(item, d_strAllocator_p);
    }

    autoDtor.release();
    d_length = currentLength + 1;
}

void bdea_Array<std::string>::insert(int                            dstIndex,
                                     const bdea_Array<std::string>& srcArray)
{
    assert(0 <= dstIndex);  assert(dstIndex <= d_length);

    int srcLength = srcArray.d_length;
    int newLength = d_length + srcLength;

    if (newLength > d_size) {  // Need to resize.
        int newSize = calculateSufficientSize(newLength, d_size);
        std::string *tmp = d_array_p;
        std::string *src = srcArray.d_array_p;
        d_array_p = (std::string *)
                 d_allocator_p->allocate(newSize * sizeof *d_array_p);
        // COMMIT
        bdema_AutoDeallocator<bdema_Allocator> autoDeallocArray(tmp,
                                                                d_allocator_p);
        d_size = newSize;
        std::string *here = d_array_p + dstIndex;
        std::string *there = tmp + dstIndex;
        std::memcpy(d_array_p, tmp, dstIndex * sizeof *d_array_p);
        int origLength = d_length;
        d_length = dstIndex;
        bdema_AutoDestructor<std::string> autoDtor(here + srcLength,
                                                   origLength - dstIndex);

        std::memcpy(here + srcLength, there,
               (origLength - dstIndex) * sizeof *d_array_p);

        elementInitialize(d_array_p, &d_length, src, 0,
                          srcLength, d_strAllocator_p);

        autoDtor.release();
        d_length = newLength;
        return;                                                 // RETURN
    }

    assert(newLength <= d_size);  // Enough capacity exists.

    std::string *s = srcArray.d_array_p;
    std::string *d = d_array_p + dstIndex;
    int numElemsMoved = d_length - dstIndex;
    int numElements = srcArray.d_length;
    std::memmove(d + numElements, d, numElemsMoved * sizeof *d);

    // An exception could occur while initializing the new elements.
    // In order to be exception neutral, the array must be left in a consistent
    // state.  The autodestructor is run on those elements moved if an
    // exception is thrown.  d_length has the current number of contiguous
    // initialized values.

    bdema_AutoDestructor<std::string> autoDtor(d + numElements, numElemsMoved);
    d_length = dstIndex;

    if (&srcArray == this) {
        // We have an aliasing problem and shifting will affect the source:
        // Unless 'dstIndex' happens to be either 0 or length, then the source
        // is split.  If 'dstIndex' is 0, the entire source has been shifted
        // up by exactly 'numElement' positions.

        int ne2 = numElements - dstIndex;

        assert(ne2 >= 0);

        elementInitialize(d_array_p, &d_length,
                          d_array_p, 0, dstIndex, d_strAllocator_p);

        elementInitialize(d_array_p, &d_length,
                          d_array_p, numElements + dstIndex,
                          ne2, d_strAllocator_p);
    }
    else {  // There are no aliasing issues.
        elementInitialize(d_array_p, &d_length, s, 0,
                          numElements, d_strAllocator_p);
    }

    autoDtor.release();
    d_length = newLength;
}

void bdea_Array<std::string>::
                             insert(int                            dstIndex,
                                    const bdea_Array<std::string>& srcArray,
                                    int                            srcIndex,
                                    int                            numElements)
{
    assert(0 <= dstIndex);  assert(dstIndex <= d_length);
    assert(0 <= srcIndex);
    assert(0 <= numElements);
    assert(srcIndex + numElements <= srcArray.d_length);

    int newLength = d_length + numElements;

    if (newLength > d_size) {  // Need to resize.
        int newSize = calculateSufficientSize(newLength, d_size);
        std::string *tmp = d_array_p;
        std::string *src = srcArray.d_array_p;
        d_array_p =
            (std::string *)
                d_allocator_p->allocate(newSize * sizeof *d_array_p);
        // COMMIT
        d_size = newSize;
        std::string *here = d_array_p + dstIndex;
        std::string *there = tmp + dstIndex;
        std::memcpy(d_array_p, tmp, dstIndex * sizeof *d_array_p);
        int origLength = d_length;

        // On exception, the destructors for AutoDeallocator and
        // AutoDestructor are called in reverse order.  The order of
        // these statements is significant.

        bdema_AutoDeallocator<bdema_Allocator> autoDealloc(tmp,
                                                           d_allocator_p);

        bdema_AutoDestructor<std::string> autoDtor(here + numElements,
                                                   origLength - dstIndex);
        d_length = dstIndex;

        std::memcpy(here + numElements, there,
                    (origLength - dstIndex) * sizeof *d_array_p);

        elementInitialize(d_array_p, &d_length,
                          src, srcIndex,
                          numElements, d_strAllocator_p);
        autoDtor.release();
        d_length = newLength;
        return;                                                 // RETURN
    }

    std::string *d = d_array_p + dstIndex;
    std::string *s = srcArray.d_array_p + srcIndex;
    std::memmove(d + numElements, d, (d_length - dstIndex) * sizeof *d);

    // An exception could occur while initializing the new elements.
    // In order to be exception neutral, the array must be left in a consistent
    // state.  The autodestructor is run on those elements moved.
    // 'd_length' has the current number of contiguous initialized values.

    bdema_AutoDestructor<std::string> autoDtor(d + numElements,
                                               d_length - dstIndex);
    d_length = dstIndex;

    if (&srcArray == this && srcIndex + numElements > dstIndex) {
        // We have an aliasing problem and shifting will affect the source:
        // If the srcIndex < dstIndex then the source is split; otherwise the
        // entire source has been shifted up by exactly numElement positions.

        if (srcIndex < dstIndex) {  // Alias problem: source split.
            int ne1 = dstIndex - srcIndex;
            std::string *s2 = s + numElements + ne1;
            int ne2 = numElements - ne1;
            assert(ne1 > 0);
            assert(ne2 > 0);
            elementInitialize(d_array_p, &d_length, s, 0,
                              ne1, d_strAllocator_p);
            elementInitialize(d_array_p, &d_length, s2, 0,
                              ne2, d_strAllocator_p);
        }
        else {  // Alias problem: source shifted as single unit by numElements.
            elementInitialize(d_array_p, &d_length, s, numElements,
                              numElements, d_strAllocator_p);
        }
    }
    else {  // There are no aliasing issues.
        elementInitialize(d_array_p, &d_length, s, 0,
                          numElements, d_strAllocator_p);
    }

    autoDtor.release();
    d_length = newLength;
}

void bdea_Array<std::string>::remove(int index)
{
    assert(0 <= index);  assert(index < d_length);

    if (d_strAllocator_p == d_allocator_p) {
        // Run the destructor only when no infrequent delete hint.
        cleanup(d_array_p, index, 1);
    }
    --d_length;
    std::string *here = d_array_p + index;
    std::memmove(here, here + 1, (d_length - index) * sizeof *d_array_p);
}

void bdea_Array<std::string>::removeAll()
{
    if (d_allocator_p != d_strAllocator_p) {
        // A seperate strallocator in use - release the memory!
        ((bdema_StrAllocator *) d_strAllocator_p)->release();
    }
    else {
        // Run the destructor only when no infrequent delete hint.
        cleanup(d_array_p, 0, d_length);
    }

    d_length = 0;
}

void bdea_Array<std::string>::remove(int index, int numElements)
{
    assert(0 <= index);
    assert(0 <= numElements);
    assert(index + numElements <= d_length);

    if (d_strAllocator_p == d_allocator_p) {
        // Run the destructor only when no infrequent delete hint.
        cleanup(d_array_p, index, numElements);
    }
    d_length -= numElements;
    std::string *here = d_array_p + index;
    std::memmove(here, here + numElements,
                 (d_length - index) * sizeof *d_array_p);
}

void bdea_Array<std::string>::
                            replace(int                            dstIndex,
                                    const bdea_Array<std::string>& srcArray,
                                    int                            srcIndex,
                                    int                            numElements)
{
    assert(0 <= dstIndex);
    assert(0 <= srcIndex);
    assert(0 <= numElements);
    assert(dstIndex + numElements <= d_length);
    assert(srcIndex + numElements <= srcArray.d_length);

    elementAssign(d_array_p, dstIndex, srcArray.d_array_p,
                  srcIndex, numElements);
}

void bdea_Array<std::string>::reserveCapacityRaw(int numElements)
{
    assert(0 <= numElements);

    if (d_size < numElements) {
        reallocate(&d_array_p, &d_size, numElements, d_length, d_allocator_p);
    }
}

void bdea_Array<std::string>::setLength(int newLength)
{
    // Exceptions can be thrown during allocation of the new array memory.
    // However, no exceptions can be thrown during the initialization of
    // array elements.  If an exception is thrown, the array state is left
    // unchanged.

    assert(0 <= newLength);

    if (newLength > d_length) {
        if (newLength > d_size) {
            int newSize = calculateSufficientSize(newLength, d_size);
            reallocate(&d_array_p, &d_size, newSize, d_length, d_allocator_p);
        }
        initializeWithDefaultValue(d_array_p + d_length,
                                   newLength - d_length,
                                   d_strAllocator_p);
    }
    else {
        if (newLength < d_length && d_strAllocator_p == d_allocator_p) {
             // Run the destructor only when no infrequent delete hint.
             cleanup(d_array_p, newLength, d_length - newLength);
        }
    }
    d_length = newLength;
}

void bdea_Array<std::string>::setLength(int                newLength,
                                        const std::string& initialValue)
{
    // Exceptions can be thrown during allocation of the new array memory.
    // Exceptions can also be thrown during the initialization of
    // array elements.  If an exception is thrown, the array state is left
    // unchanged (except that the array size may have been increased).

    assert(0 <= newLength);

    if (newLength > d_length) {
        if (newLength > d_size) {
            int newSize = calculateSufficientSize(newLength, d_size);
            reallocate(&d_array_p, &d_size, newSize, d_length, d_allocator_p);
        }

        // An exception could occur within initializeWithValue.  However,
        // the function will run the destructor on all array elements
        // already initialized by the function before the exception occurred.

        initializeWithValue(d_array_p + d_length,
                            initialValue,
                            newLength - d_length,
                            d_strAllocator_p);
    }
    else {
        if (newLength < d_length && d_strAllocator_p == d_allocator_p) {
            // The array will be shorter so run the destructor on excessive
            // elements, only when no infrequent delete hint.
            cleanup(d_array_p, newLength, d_length - newLength);
        }
    }
    d_length = newLength;
}

                        // ---------
                        // ACCESSORS
                        // ---------

std::ostream& bdea_Array<std::string>::
                                      print(std::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    assert(0 <= spacesPerLevel);

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "[\n";

    if (level < 0) {
        level = -level;
    }

    int levelPlus1 = level + 1;

    for (int i = 0; i < d_length; ++i) {
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << d_array_p[i] << '\n';
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);
    return stream << "]\n";
}

std::ostream& bdea_Array<std::string>::streamOut(std::ostream& stream) const
{
    stream << '[';
    for (int i = 0; i < d_length; ++i) {
        stream << ' ' << d_array_p[i];
    }
    return stream << " ]";
}

                        // --------------
                        // FREE OPERATORS
                        // --------------

int operator==(const bdea_Array<std::string>& lhs,
               const bdea_Array<std::string>& rhs)
{
    const int len = lhs.length();
    if (len != rhs.length()) {
        return 0;
    }

    for (int i = 0; i < len; ++i) {
        if (lhs[i] != rhs[i]) return 0;
    }
    return 1;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
