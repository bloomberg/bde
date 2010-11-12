// bdea_cstrarray.cpp        -*-C++-*-

#include <bdea_cstrarray.h>

#include <bdema_default.h>
#include <bdema_multipoolallocator.h>
#include <bdema_sequentialallocator.h>
#include <bdema_allocator.h>
#include <bdema_autodeallocator.h>
#include <bdema_autodeleter.h>

#include <bdema_testallocator.h>               // for testing only
#include <bdema_testallocatorexception.h>      // for testing only
#include <bdex_testoutstream.h>                // for testing only
#include <bdex_testinstream.h>                 // for testing only
#include <bdex_testinstreamexception.h>        // for testing only

#include <cassert>
#include <cstdio>      // sprintf()
#include <cstring>     // strlen(), memset(), memcpy(), memcmp()

#include <ostream>
#include <strstream>                           // for testing only

namespace BloombergLP {

                        // ---------
                        // CONSTANTS
                        // ---------

enum {
    INITIAL_SIZE = 1,            // initial size of an empty array
    GROW_FACTOR  = 2,            // factor by which to increase size
    INITIAL_SMALL_CAPACITY = 4,  // initial capacity of smallest pool
    MULTIPOOLALLOCATOR_BINS = 12 // number of bins a 'bdema_MultipoolAllocator'
                                 // will be created with.
};  
    

                        // ----------------
                        // STATIC FUNCTIONS
                        // ----------------

static
bdema_Allocator *makeInternalAllocator(bdea_CstrArray::Hint  usagePattern,
                                       bdema_Allocator      *originalAllocator)
{
    bdema_Allocator *internalAllocator;

    switch (usagePattern) {
      case bdea_CstrArray::WRITE_MANY: {
        bdema_MultipoolAllocator
                     *multipoolAllocator = new (*originalAllocator)
                              bdema_MultipoolAllocator(MULTIPOOLALLOCATOR_BINS,
                                                       originalAllocator);
        
        bdema_AutoDeleter<bdema_MultipoolAllocator, bdema_Allocator>
                     allocatorGuard(&multipoolAllocator, 1, originalAllocator);

        multipoolAllocator->reserveCapacity(1, INITIAL_SMALL_CAPACITY);

        allocatorGuard.release();
        internalAllocator = multipoolAllocator;
      } break;
      case bdea_CstrArray::WRITE_ONCE: {
        internalAllocator = new (*originalAllocator)
                                  bdema_SequentialAllocator(originalAllocator);
      } break;
      default: {
        internalAllocator = originalAllocator;
      } break;
    }
    return internalAllocator;
}

                        // ================================
                        // class bdea_CstrArray_StringGuard
                        // ================================

    
class bdea_CstrArray_StringGuard {
    // This class implements a proctor that, unless its 'release' method is
    // invoked, automatically *deallocates* each managed 'const' 'char*'
    // ("C-string") element in its proctoreded array of "pointer-to-C-string"
    // when the proctor is destroyed.
    //
    // This class is a customized adaptation of 'bdema_AutoDeleter'.  In
    // particular, this proctor deallocates the managed memory but does not
    // need to call a destructor.  Also, managed elements (pointers) are
    // permitted to be 0, which is only recently true of 'bdema_AutoDeleter'.

  private:
    char            **d_origin_p;// reference location for the managed sequence
    int               d_length;  // num. managed ptrs (sign encodes direction)
    bdema_Allocator  *d_allocator_p; // held allocator (not owned)

  public:
    // CREATORS
    bdea_CstrArray_StringGuard(char            **origin,
                               int               length,
                               bdema_Allocator  *stringAllocator);
        // Create a proctor C-string to manage an out-of-place sequence of
        // C-strings whose range is defined by the specified 'origin' and
        // 'length'.  Each C-string in the managed sequence must either be 0 or
        // else have been allocated from the specified 'stringAllocator'.  The
        // sequence may extend in either direction from 'origin'.  A positive
        // 'length' represents the sequence of C-strings starting at 'origin'
        // and extending "up" to 'length' (*not* including the C-string at the
        // index position 'origin' + 'length').  A negative 'length' represents
        // the sequence of C-strings starting at one index position below
        // 'origin' and extending "down" to the absolute value of 'length'
        // (including the C-string at index position 'origin' + 'length'). 
        // Note that when 'length' is non-positive, the C-string at 'origin' is
        // *not* managed by this proctor.

    ~bdea_CstrArray_StringGuard();
        // Deallocate all managed C-strings.  Note that the order of
        // deallocation is undefined.

    // MANIPULATORS
    bdea_CstrArray_StringGuard& operator++();
        // Increase by one the (signed) length of the sequence of C-strings
        // managed by this proctor.  Note that if the length of this proctor is
        // currently negative, the number of managed C-strings will decrease by
        // one, whereas if the length is non-negative, the number of managed
        // C-strings will increase by one.

    bdea_CstrArray_StringGuard& operator--();
        // Decrease by one the (signed) length of the sequence of C-strings
        // managed by this proctor.  Note that if the length of this proctor is
        // currently positive, the number of managed C-strings will decrease by
        // one, whereas if the length is non-positive, the number of managed
        // C-strings will increase by one.
    
    void release();
        // Set this proctor's length to 0, thereby releasing from management
        // any currently-managed C-strings.
};

bdea_CstrArray_StringGuard::bdea_CstrArray_StringGuard(
                                           char            **origin,
                                           int               length,
                                           bdema_Allocator  *stringAllocator)
: d_origin_p(origin)
, d_length(length)
, d_allocator_p(stringAllocator)
{
}

bdea_CstrArray_StringGuard::~bdea_CstrArray_StringGuard()
{
    // Imp Note:  Any or all managed pointer in the sequence may be null.  The
    //            imp tests for null before calling 'deallocate'.
    
    if (d_length) { // expected case is 0 == d_length
        if (0 < d_length) { // positive length
            for (int i = 0; i < d_length; ++i) {
                if (char *str = d_origin_p[i]) {
                    d_allocator_p->deallocate(str);
                }
            }
        }
        else { // negative length
            for (int i = -1; i >= d_length; --i) {
                if (char *str = d_origin_p[i]) {
                    d_allocator_p->deallocate(str);
                }
            }
        }
    }
}

void bdea_CstrArray_StringGuard::release()
{
    d_length = 0;
}

bdea_CstrArray_StringGuard& bdea_CstrArray_StringGuard::operator++()
{
    ++d_length;
    return *this;
}

bdea_CstrArray_StringGuard& bdea_CstrArray_StringGuard::operator--()
{
    --d_length;
    return *this;
}


                        // ====================
                        // class bdea_CstrArray
                        // ====================

    
                        // ---------------------
                        // PRIVATE CLASS METHODS
                        // ---------------------

int bdea_CstrArray::calculateCapacity(int numElements, int originalCapacity)
{
    // Imp Note:  This method uses a geometric growth algorithm.
    
    assert(1 <= originalCapacity);
    assert(0 <= numElements);

    while (originalCapacity < numElements) {
        originalCapacity *= GROW_FACTOR;
    };

    assert(originalCapacity >= numElements);
    return originalCapacity;
}
    
                        // --------
                        // CREATORS
                        // --------

bdea_CstrArray::bdea_CstrArray(bdema_Allocator *basicAllocator)
: d_size(INITIAL_SIZE)
, d_length(0)
, d_originalAllocator_p(bdema_Default::allocator(basicAllocator))
, d_stringAllocator_p(bdema_Default::allocator(basicAllocator))
, d_arrayAllocator_p(bdema_Default::allocator(basicAllocator))
{ 
    d_array_p = (char **)
                     d_arrayAllocator_p->allocate(sizeof(*d_array_p) * d_size);
}

bdea_CstrArray::bdea_CstrArray(bdea_CstrArray::Hint  usagePattern,
                               bdema_Allocator      *basicAllocator)
: d_size(INITIAL_SIZE)
, d_length(0)
, d_originalAllocator_p(bdema_Default::allocator(basicAllocator))
, d_arrayAllocator_p(bdema_Default::allocator(basicAllocator))
{
    bdema_AutoDeleter<bdema_Allocator, bdema_Allocator>
                allocatorGuard(&d_stringAllocator_p, 0, d_originalAllocator_p);
    d_stringAllocator_p = makeInternalAllocator(usagePattern,
                                                d_originalAllocator_p);

    if (d_originalAllocator_p != d_stringAllocator_p) {
        ++allocatorGuard;
    }

    d_array_p = (char **)
                     d_arrayAllocator_p->allocate(sizeof(*d_array_p) * d_size);

    allocatorGuard.release();
}

bdea_CstrArray::bdea_CstrArray(int              initialLength,
                               bdema_Allocator *basicAllocator)
: d_size(calculateCapacity(initialLength, INITIAL_SIZE))
, d_length(initialLength)
, d_originalAllocator_p(bdema_Default::allocator(basicAllocator))
, d_stringAllocator_p(bdema_Default::allocator(basicAllocator))
, d_arrayAllocator_p(bdema_Default::allocator(basicAllocator))
{
    d_array_p = (char **)
                    d_arrayAllocator_p->allocate(sizeof(*d_array_p) * d_size);

    std::memset(d_array_p, 0, sizeof(*d_array_p) * d_length);
}

bdea_CstrArray::bdea_CstrArray(int                   initialLength,
                               bdea_CstrArray::Hint  usagePattern,
                               bdema_Allocator      *basicAllocator)
: d_size(calculateCapacity(initialLength, INITIAL_SIZE))
, d_length(initialLength)
, d_originalAllocator_p(bdema_Default::allocator(basicAllocator))
, d_arrayAllocator_p(bdema_Default::allocator(basicAllocator))
{
    bdema_AutoDeleter<bdema_Allocator, bdema_Allocator>
                allocatorGuard(&d_stringAllocator_p, 0, d_originalAllocator_p);
    d_stringAllocator_p = makeInternalAllocator(usagePattern,
                                                d_originalAllocator_p);

    if (d_originalAllocator_p != d_stringAllocator_p) {
        ++allocatorGuard;
    }

    d_array_p = (char **)
                     d_arrayAllocator_p->allocate(sizeof(*d_array_p) * d_size);

    allocatorGuard.release();

    std::memset(d_array_p, 0, sizeof(*d_array_p) * d_length);
}

bdea_CstrArray::bdea_CstrArray(const bdea_CstrArray&  original,
                               bdema_Allocator       *basicAllocator)
: d_size(calculateCapacity(original.d_length, INITIAL_SIZE))
, d_length(original.d_length)
, d_originalAllocator_p(bdema_Default::allocator(basicAllocator))
, d_stringAllocator_p(bdema_Default::allocator(basicAllocator))
, d_arrayAllocator_p(bdema_Default::allocator(basicAllocator))
{
    d_array_p = (char **)
                     d_arrayAllocator_p->allocate(sizeof(*d_array_p) * d_size);
    bdema_AutoDeallocator<bdema_Allocator> arrayGuard(d_array_p,
                                                      d_arrayAllocator_p);
    bdea_CstrArray_StringGuard stringGuard(d_array_p, 0, d_stringAllocator_p);
    for (int i = 0; i < d_length; ++i) {
        if (char *originalString = original.d_array_p[i]) {
            int size = std::strlen(originalString) + 1;
            d_array_p[i] = (char *) d_stringAllocator_p->allocate(size);
            std::strcpy(d_array_p[i], originalString);
        }
        else {
            d_array_p[i] = 0;
        }
        ++stringGuard;
    }
    stringGuard.release();
    arrayGuard.release();
}

bdea_CstrArray::bdea_CstrArray(const bdea_CstrArray&  original,
                               bdea_CstrArray::Hint   usagePattern,
                               bdema_Allocator       *basicAllocator)
: d_size(calculateCapacity(original.d_length, INITIAL_SIZE))
, d_length(original.d_length)
, d_originalAllocator_p(bdema_Default::allocator(basicAllocator))
, d_arrayAllocator_p(bdema_Default::allocator(basicAllocator))
{
    bdema_AutoDeleter<bdema_Allocator, bdema_Allocator>
              allocatorGuard(&d_stringAllocator_p, 0, d_originalAllocator_p);
    d_stringAllocator_p = makeInternalAllocator(usagePattern,
                                                d_originalAllocator_p);

    if (d_originalAllocator_p != d_stringAllocator_p) {
        ++allocatorGuard;
    }

    d_array_p = (char **)
                     d_arrayAllocator_p->allocate(sizeof(*d_array_p) * d_size);
    bdema_AutoDeallocator<bdema_Allocator> arrayGuard(d_array_p,
                                                      d_arrayAllocator_p);

    if (bdea_CstrArray::PASS_THROUGH == usagePattern) { // must proctor
        bdea_CstrArray_StringGuard stringGuard(d_array_p, 0,
                                                          d_stringAllocator_p);
        for (int i = 0; i < d_length; ++i) {
            if (char *originalString = original.d_array_p[i]) {
                int size = std::strlen(originalString) + 1;
                d_array_p[i] = (char *) d_stringAllocator_p->allocate(size);
                std::strcpy(d_array_p[i], originalString);
            }
            else {
                d_array_p[i] = 0;
            }
            ++stringGuard;
        }
        stringGuard.release();
    }
    else {  // need not proctor elements; allocator will release on destruction
        for (int i = 0; i < d_length; ++i) {
            if (char *originalString = original.d_array_p[i]) {
                int size = std::strlen(originalString) + 1;
                d_array_p[i] = (char *) d_stringAllocator_p->allocate(size);
                std::strcpy(d_array_p[i], originalString);
            }
            else {
                d_array_p[i] = 0;
            }
        }
    }
    arrayGuard.release();
    allocatorGuard.release();
}

bdea_CstrArray::~bdea_CstrArray()
{
    assert(d_array_p);
    assert(0 < d_size);
    assert(d_length <= d_size);
    assert(d_originalAllocator_p);
    assert(d_stringAllocator_p);
    assert(d_arrayAllocator_p);

    if (d_stringAllocator_p != d_originalAllocator_p) {
        d_stringAllocator_p->~bdema_Allocator();
        d_originalAllocator_p->deallocate(d_stringAllocator_p);
    }
    else {
        for (int i = 0; i < d_length; ++i) {
            if (char *element = d_array_p[i]) {
                d_stringAllocator_p->deallocate(element);
            }
        }
    }
    
    d_arrayAllocator_p->deallocate(d_array_p);
}

    
                        // ------------
                        // MANIPULATORS
                        // ------------

bdea_CstrArray& bdea_CstrArray::operator=(const bdea_CstrArray& rhs)
{
    if (&rhs == this) {  // cheap test for self-assignment
        return *this;
    }

    if (d_stringAllocator_p != d_originalAllocator_p) {
        // 'd_stringAllocator_p' is a managed allocator, so can 'release'.
        static_cast<bdema_ManagedAllocator *>(d_stringAllocator_p)->release();
    }
    else { // must 'deallocate' one at a time
        for (int i = 0; i < d_length; ++i) {
            if (char *element = d_array_p[i]) {
                d_stringAllocator_p->deallocate(element);
            }
        }
    }
    d_length = 0;

    if (d_size <= rhs.d_length) { // Capacity is not sufficient
        // Allocate new 'temp' array.  If this throws, we're still good.
        int newSize = calculateCapacity(rhs.d_length, INITIAL_SIZE);
        char **temp = (char **)
                    d_arrayAllocator_p->allocate(sizeof(*d_array_p) * newSize);

        d_arrayAllocator_p->deallocate(d_array_p);
        d_array_p = temp;
        d_size    = newSize;
    }

    // Copy non-0 rhs strings, or assign 0 if the rhs string is 0.
    for (int i = 0; i < rhs.d_length; ++i) {
        if (char *rhsString = rhs.d_array_p[i]) {
            int size = std::strlen(rhsString) + 1;
            // Ok if allocation throws since didn't increment 'd_length'
            d_array_p[i] = (char *) d_stringAllocator_p->allocate(size);
            std::strcpy(d_array_p[i], rhsString);
        }
        else {
            d_array_p[i] = 0;
        }
        ++d_length; // COMMIT to ith element
    }
    
    return *this;
}

void bdea_CstrArray::append(const char *cstring)
{
    if (d_length >= d_size) {
        // First allocate new 'temp' array.  If this throws, we're still good.
        int newSize = calculateCapacity(d_length + 1, d_size);
        char **temp = (char **)
                    d_arrayAllocator_p->allocate(sizeof(*d_array_p) * newSize);
        std::memcpy(temp, d_array_p, sizeof(*d_array_p) * d_length);
        d_arrayAllocator_p->deallocate(d_array_p);
        d_array_p = temp;
        d_size    = newSize;
    }
    
    if (cstring) {
        int size = std::strlen(cstring) + 1;
        // Ok if allocation throws since didn't increment 'd_length'
        d_array_p[d_length] = (char *) d_stringAllocator_p->allocate(size);
        std::strcpy(d_array_p[d_length], cstring);
    }
    else {
        d_array_p[d_length] = 0;
    }
    ++d_length; // COMMIT to appended element
}

void bdea_CstrArray::append(const bdea_CstrArray& srcArray)
{
    int srcLength = srcArray.d_length;
    int newLength = d_length + srcLength;
    if (newLength >= d_size) {
        // First allocate new 'temp' array.  If this throws, we're still good.
        int newSize = calculateCapacity(newLength, d_size);
        char **temp = (char **)
                    d_arrayAllocator_p->allocate(sizeof(*d_array_p) * newSize);
        std::memcpy(temp, d_array_p, sizeof(*d_array_p) * d_length);
        d_arrayAllocator_p->deallocate(d_array_p);
        d_array_p = temp;
        d_size    = newSize;
    }

    for (int i = 0; i < srcLength; ++i) {
        if (const char *srcString = srcArray.d_array_p[i]) {
            int size = std::strlen(srcString) + 1;
            // Ok if allocation throws since didn't increment 'd_length'
            d_array_p[d_length] = (char *) d_stringAllocator_p->allocate(size);
            std::strcpy(d_array_p[d_length], srcString);
        }
        else {
            d_array_p[d_length] = 0;
        }
        ++d_length; // COMMIT to ith element
    }
}

void bdea_CstrArray::append(const bdea_CstrArray& srcArray,
                            int                   srcIndex,
                            int                   numElements)
{
    assert(0 <= srcIndex);
    assert(0 <= numElements);
    assert(srcIndex + numElements <= srcArray.length());
    
    int newLength = d_length + numElements;
    if (newLength >= d_size) {
        // First allocate new 'temp' array.  If this throws, we're still good.
        int newSize = calculateCapacity(newLength, d_size);
        char **temp = (char **)
                    d_arrayAllocator_p->allocate(sizeof(*d_array_p) * newSize);
        std::memcpy(temp, d_array_p, sizeof(*d_array_p) * d_length);
        d_arrayAllocator_p->deallocate(d_array_p);
        d_array_p = temp;
        d_size    = newSize;
    }

    const int LAST_INDEX = srcIndex + numElements;
    for (int i = srcIndex; i < LAST_INDEX; ++i) {
        if (const char *srcString = srcArray.d_array_p[i]) {
            int size = std::strlen(srcString) + 1;
            // Ok if allocation throws since didn't increment 'd_length'
            d_array_p[d_length] = (char *) d_stringAllocator_p->allocate(size);
            std::strcpy(d_array_p[d_length], srcString);
        }
        else {
            d_array_p[d_length] = 0;
        }
        ++d_length; // COMMIT to ith element
    }
}

void bdea_CstrArray::insert(int index, const char *cstring)
{
    assert(0 <= index);  assert(index <= d_length);

    if (d_length >= d_size) {
        // First allocate new 'temp' array.  If this throws, we're still good.
        int newSize = calculateCapacity(d_length + 1, d_size);
        char **temp = (char **)
                    d_arrayAllocator_p->allocate(sizeof(*d_array_p) * newSize);

        char **dst = temp + index;
        char **src = d_array_p + index;
        
        std::memcpy(temp, d_array_p, sizeof(*d_array_p) * index);
        std::memcpy(dst + 1, src, sizeof(*d_array_p) * (d_length - index));
        *dst = 0; // not necessarily right, but a legal value if alloc. throws
        
        d_arrayAllocator_p->deallocate(d_array_p);
        d_array_p = temp;
        d_size    = newSize;
    }
    else {
        char **dst = d_array_p + index;
        std::memmove(dst + 1, dst, sizeof(*d_array_p) * (d_length - index));
        *dst = 0; // not necessarily right, but a legal value if alloc. throws
    }
    ++d_length;
    
    if (cstring) {
        int size = std::strlen(cstring) + 1;
        // Ok if allocation throws since "inserted" 0, which is legal
        d_array_p[index] = (char *) d_stringAllocator_p->allocate(size);
        std::strcpy(d_array_p[index], cstring);
    }
}

void bdea_CstrArray::insert(int dstIndex, const bdea_CstrArray& srcArray)
{
    assert(0 <= dstIndex);  assert(dstIndex <= d_length);

    const int srcLength  = srcArray.d_length;
    const int newLength  = d_length + srcLength;
    const int tailLength = d_length - dstIndex;
    
    if (newLength >= d_size) {  // must resize in this case -> alias-safe!
        // First allocate new 'temp' array.  If this throws, we're still good.
        int newSize = calculateCapacity(newLength, d_size);
        char **temp = (char **)
                    d_arrayAllocator_p->allocate(sizeof(*d_array_p) * newSize);

        char **dst = temp + dstIndex;
        char **src = d_array_p + dstIndex;
        
        std::memcpy(temp, d_array_p, sizeof(*d_array_p) * dstIndex);
        std::memcpy(dst + srcLength, src, sizeof(*d_array_p) * tailLength);
        
        bdea_CstrArray_StringGuard tempGuard(dst, 0, d_stringAllocator_p);
        
        for (int i = 0; i < srcLength; ++i) {
            const char  *srcString   = srcArray[i];  // alias-safe
            char       **dstString_p = dst + i;
            if (srcString) {
                int size = std::strlen(srcString) + 1;
                // Ok if allocation throws since 'temp' is guarded
                *dstString_p = (char *) d_stringAllocator_p->allocate(size);
                std::strcpy(*dstString_p, srcString);
                ++tempGuard;
            }
            else {
                *dstString_p = 0; // can set one at a time in this case
            }
        }

        // success: cleanup; swap in 'temp', size, length; return
        tempGuard.release();
        d_arrayAllocator_p->deallocate(d_array_p);
        d_array_p = temp;
        d_length  = newLength;
        d_size    = newSize;

        return;                                                       // RETURN
    }

    char **dst = d_array_p + dstIndex;
    std::memmove(dst + srcLength, dst, sizeof(*d_array_p) * tailLength);
    std::memset(dst, 0, sizeof(*d_array_p) * srcLength);
                           // 0 may not be right, but is legal if alloc. throws
    d_length = newLength;  // destructor now "proctors" all C-strings

    if (&srcArray != this) {
        for (int i = 0; i < srcLength; ++i) {
            const char *srcString = srcArray[i];  // No alias here!
            if (srcString) {
                int size = std::strlen(srcString) + 1;
                char **dstString_p = dst + i;
                // Ok if allocation throws since "inserted" 0, which is valid
                *dstString_p = (char *) d_stringAllocator_p->allocate(size);
                std::strcpy(*dstString_p, srcString);
            }
        }
    }
    else { // alias case, so loop over pointers explicitly, since same array
        for (char **s = d_array_p; s < dst; ++s) { // loop over source addrs.
            const char *srcString = *s;  // alias, before the insertion point
            if (srcString) {
                int size = std::strlen(srcString) + 1;
                char **dstString_p = s + dstIndex;
                // Ok if allocation throws since "inserted" 0, which is valid
                *dstString_p = (char *) d_stringAllocator_p->allocate(size);
                std::strcpy(*dstString_p, srcString);
            }
        }
        char **onePastEnd = d_array_p + d_length;
        for (char **s = dst + srcLength; s < onePastEnd; ++s) {
            const char *srcString = *s;  // alias, after the insertion point
            if (srcString) {
                int size = std::strlen(srcString) + 1;
                char **dstString_p = s - tailLength;
                // Ok if allocation throws since "inserted" 0, which is valid
                *dstString_p = (char *) d_stringAllocator_p->allocate(size);
                std::strcpy(*dstString_p, srcString);
            }
        }        
    }
}

void bdea_CstrArray::insert(int                   dstIndex, 
                            const bdea_CstrArray& srcArray, 
                            int                   srcIndex, 
                            int                   numElements)
{
    assert(0 <= dstIndex);  assert(dstIndex <= d_length);
    assert(0 <= srcIndex);
    assert(0 <= numElements);
    assert(srcIndex + numElements <= srcArray.d_length);

    const int srcLength  = srcArray.d_length;
    const int newLength  = d_length + numElements;
    const int tailLength = d_length - dstIndex;
    
    if (newLength >= d_size) {  // must resize in this case -> alias-safe!
        // First allocate new 'temp' array.  If this throws, we're still good.
        int newSize = calculateCapacity(newLength, d_size);
        char **temp = (char **)
                    d_arrayAllocator_p->allocate(sizeof(*d_array_p) * newSize);

        char **dst = temp + dstIndex;
        char **src = d_array_p + dstIndex;
        
        std::memcpy(temp, d_array_p, sizeof(*d_array_p) * dstIndex);
        std::memcpy(dst + numElements, src, sizeof(*d_array_p) * tailLength);
        
        bdea_CstrArray_StringGuard tempGuard(dst, 0, d_stringAllocator_p);
        
        for (int i = 0; i < numElements; ++i) {
            const char  *srcString   = srcArray[srcIndex + i];  // alias-safe
            char       **dstString_p = dst + i;
            if (srcString) {
                int size = std::strlen(srcString) + 1;
                // Ok if allocation throws since 'temp' is guarded
                *dstString_p = (char *) d_stringAllocator_p->allocate(size);
                std::strcpy(*dstString_p, srcString);
            }
            else {
                *dstString_p = 0; // not 'memset': no except. neutrality issue
            }
            ++tempGuard;
        }

        // success: cleanup; swap in 'temp', size, length; return
        tempGuard.release();
        d_arrayAllocator_p->deallocate(d_array_p);
        d_array_p = temp;
        d_length  = newLength;
        d_size    = newSize;

        return;                                                       // RETURN
    }

    char **dst = d_array_p + dstIndex;
    std::memmove(dst + numElements, dst, sizeof(*d_array_p) * tailLength);
    std::memset(dst, 0, sizeof(*d_array_p) * numElements);
                           // 0 may not be right, but is legal if alloc. throws
    d_length = newLength;  // destructor now "proctors" all C-strings

    if (&srcArray != this) {
        for (int i = 0; i < numElements; ++i) {
            const char *srcString = srcArray[srcIndex + i];  // No alias here!
            if (srcString) {
                int size = std::strlen(srcString) + 1;
                char **dstString_p = dst + i;
                // Ok if allocation throws since "inserted" 0, which is valid
                *dstString_p = (char *) d_stringAllocator_p->allocate(size);
                std::strcpy(*dstString_p, srcString);
            }
        }
    }
    else { // alias case
        int numSrcBeforeDst = dstIndex > srcIndex ? dstIndex - srcIndex : 0;
        if (numSrcBeforeDst > numElements || 0 == numSrcBeforeDst) {
            int    offset = numSrcBeforeDst > numElements ? 0 : numElements;
            char **src    = d_array_p + srcIndex + offset;  // alias
            for (int i = 0; i < numElements; ++i) {
                const char *srcString = *(src + i);  // alias, no overlap
                if (srcString) {
                    int size = std::strlen(srcString) + 1;
                    char **dstString_p = dst + i;
                    // Ok if allocation throws since "inserted" (valid) 0's
                    *dstString_p = (char *)d_stringAllocator_p->allocate(size);
                    std::strcpy(*dstString_p, srcString);
                }
            }
        }
        else {
            char **src = d_array_p + srcIndex;  // alias, start before gap
            for (int i = 0; i < numSrcBeforeDst; ++i) {
                const char *srcString = *(src + i);  // alias, before gap
                if (srcString) {
                    int size = std::strlen(srcString) + 1;
                    char **dstString_p = dst + i;
                    // Ok if allocation throws since "inserted" (valid) 0's
                    *dstString_p = (char *)d_stringAllocator_p->allocate(size);
                    std::strcpy(*dstString_p, srcString);
                }
            }
            src += numElements;  // bump source ptr past gap
            for (int i = numSrcBeforeDst; i < numElements; ++i) {
                const char *srcString = *(src + i);  // alias, after gap
                if (srcString) {
                    int size = std::strlen(srcString) + 1;
                    char **dstString_p = dst + i;
                    // Ok if allocation throws since "inserted" (valid) 0's
                    *dstString_p = (char *)d_stringAllocator_p->allocate(size);
                    std::strcpy(*dstString_p, srcString);
                }
            }
        }
    }
}

void bdea_CstrArray::remove(int index)
{
    assert(0 <= index);  assert(index < d_length);

    char **dst = d_array_p + index;
    if (*dst) {
        d_stringAllocator_p->deallocate(*dst);
    }
    --d_length; // doing this now saves a subtraction on the 'memmove'
    std::memmove(dst, dst + 1, sizeof(*d_array_p) * (d_length - index));
}
    
void bdea_CstrArray::remove(int index, int numElements)
{
    assert(0 <= index);
    assert(0 <= numElements);
    assert(index + numElements < d_length);

    char **dst         = d_array_p + index;
    char **onePastLast = dst + numElements;
    if (d_stringAllocator_p == d_originalAllocator_p) {
        for (char **p = dst; p < onePastLast; ++p) {
            if (*p) {
                d_stringAllocator_p->deallocate(*p);
            }
        }
    }
    d_length -= numElements; // doing now saves a subtraction on the 'memmove'
    std::memmove(dst, onePastLast, sizeof(*d_array_p) * (d_length - index));
}
    
void bdea_CstrArray::removeAll()
{
    if (d_stringAllocator_p != d_originalAllocator_p) {
        // 'd_stringAllocator_p' is a managed allocator, so can 'release'.
        static_cast<bdema_ManagedAllocator *>(d_stringAllocator_p)->release();
    }
    else { // must 'deallocate' one at a time
        for (int i = 0; i < d_length; ++i) {
            if (char *element = d_array_p[i]) {
                d_stringAllocator_p->deallocate(element);
            }
        }
    }
    d_length = 0;
}
    
void bdea_CstrArray::replace(int index, const char *cstring)
{
    assert(0 <= index);  assert(index < d_length);

    char **dst = d_array_p + index;
    if (*dst) {
        d_stringAllocator_p->deallocate(*dst);
        *dst = 0; // for exception neutrality; also saves 'else' below
    }

    if (cstring) {
        int size = std::strlen(cstring) + 1;
        // Ok if allocation throws since we have set *dst = 0
        *dst = (char *)d_stringAllocator_p->allocate(size);
        std::strcpy(*dst, cstring);
    }
}

void bdea_CstrArray::replace(int                   dstIndex, 
                             const bdea_CstrArray& srcArray, 
                             int                   srcIndex, 
                             int                   numElements)
{
    assert(0 <= dstIndex);  assert(dstIndex <= d_length);
    assert(0 <= srcIndex);
    assert(0 <= numElements);
    assert(dstIndex + numElements <= d_length);
    assert(srcIndex + numElements <= srcArray.d_length);

    if (&srcArray == this) {
        // Imp Note: We *must* treat the alias case separately for correctness.
        // We *also* optimize for efficiency (at the cost of some complexity)
        // by using 'memcpy' when there is an appropriate overlap of source and
        // destination.  In particular, the *minimum* of the number of
        // overlapping elements and the number of non-overlapping elements can
        // be 'memcpy'd without re-allocaton.  The destination of the 'memcpy'
        // must first be deallocated; then, an equal number of elements must be
        // copied (with allocation) but *without* deallocation, before going on
        // to the "non-alias-like" case of deallocate-and-copy-with-allocation.
        
        int dIndex = dstIndex - srcIndex;
        if (0 == dIndex) {
            return; // unlikely, but cheap
        }
        if (0 > dIndex) { // srcIndex > dstIndex, therefore copy forwards
            char **dst     = d_array_p + dstIndex;
            char **src     = d_array_p + srcIndex;
            char **end     = dst + numElements;     // copy forwards: end > dst
            int    overlap = numElements + dIndex;  // 'dIndex' is negative
            if (0 < overlap) {
                int numMemcpy = -dIndex < overlap ? -dIndex : overlap;
                for (int i = 0; i < numMemcpy; ++i) {
                    if (char *str = dst[i]) {
                        d_stringAllocator_p->deallocate(str);
                    }
                }
                std::memcpy(dst, src, sizeof(*dst) * numMemcpy);
                dst += numMemcpy;
                src += numMemcpy;
                
                // copy, don't dealloc for 'numMemcpy' elements
                char **firstDealloc = dst + numMemcpy;
                for (; dst < firstDealloc; ++dst, ++src) {
                    *dst = 0; // for exception neutrality
                    if (*src) {
                        int size = std::strlen(*src) + 1;
                        // Ok if allocation throws since we have set *dst = 0
                        *dst = (char *)d_stringAllocator_p->allocate(size);
                        std::strcpy(*dst, *src);
                    }
                }
            }
            
            // "normal" deallocate-and-copy as in non-alias case
            for (; dst < end; ++dst, ++src) {
                if (*dst) {
                    d_stringAllocator_p->deallocate(*dst);
                    *dst = 0; // for exception neutrality; also saves 'else'
                }
                if (*src) {
                    int size = std::strlen(*src) + 1;
                    // Ok if allocation throws since we have set *dst = 0
                    *dst = (char *)d_stringAllocator_p->allocate(size);
                    std::strcpy(*dst, *src);
                }
            }
        }
        else { // srcIndex < dstIndex, therefore copy backwards
            char **dst     = d_array_p + dstIndex + numElements - 1;
            char **src     = d_array_p + srcIndex + numElements - 1;
            char **end     = dst - numElements;    // copy backwards: end < dst
            int    overlap = numElements - dIndex; // 'dIndex' is positive
            if (0 < overlap) {
                int numMemcpy = dIndex < overlap ? dIndex : overlap;
                dst -= numMemcpy; // points to last non-memcpy cstring
                src -= numMemcpy;
                for (int i = 1; i <= numMemcpy; ++i) {
                    if (char *str = dst[i]) {
                        d_stringAllocator_p->deallocate(str);
                    }
                }
                std::memcpy(dst + 1, src + 1, sizeof(*dst) * numMemcpy);

                // copy, don't dealloc for 'numMemcpy' elements
                char **firstDealloc = dst - numMemcpy;
                for (; dst > firstDealloc; --dst, --src) {
                    *dst = 0; // for exception neutrality
                    if (*src) {
                        int size = std::strlen(*src) + 1;
                        // Ok if allocation throws since we have set *dst = 0
                        *dst = (char *)d_stringAllocator_p->allocate(size);
                        std::strcpy(*dst, *src);
                    }
                }
            }
            
            // "normal" deallocate-and-copy as in non-alias case
            for (; dst > end; --dst, --src) {
                if (*dst) {
                    d_stringAllocator_p->deallocate(*dst);
                    *dst = 0; // for exception neutrality; also saves 'else'
                }
                if (*src) {
                    int size = std::strlen(*src) + 1;
                    // Ok if allocation throws since we have set *dst = 0
                    *dst = (char *)d_stringAllocator_p->allocate(size);
                    std::strcpy(*dst, *src);
                }
            }
        }
        return; // alias cases taken care of; done                    // RETURN
    }

    // non-alias case
    char **dst = d_array_p + dstIndex;
    char **end = dst + numElements;
    char **src = (char **)(srcArray.d_array_p + srcIndex);
    for (; dst < end; ++dst, ++src) {
        if (*dst) {
            d_stringAllocator_p->deallocate(*dst);
            *dst = 0; // for exception neutrality; also saves 'else' below
        }
        if (*src) {
            int size = std::strlen(*src) + 1;
            // Ok if allocation throws since we have set *dst = 0
            *dst = (char *)d_stringAllocator_p->allocate(size);
            std::strcpy(*dst, *src);
        }
    }
}

void bdea_CstrArray::reserveCapacity(int numElements)
{
    assert(0 <= numElements);

    if (numElements > d_size) {
        // First allocate new 'temp' array.  If this throws, we're still good.
        int newSize = calculateCapacity(numElements, d_size);
        char **temp = (char **)
                    d_arrayAllocator_p->allocate(sizeof(*d_array_p) * newSize);
        std::memcpy(temp, d_array_p, sizeof(*d_array_p) * d_length);
        d_arrayAllocator_p->deallocate(d_array_p);
        d_array_p = temp;
        d_size    = newSize;
    }        
}

void bdea_CstrArray::setLength(int newLength)
{
    if (newLength > d_size) {
        // First allocate new 'temp' array.  If this throws, we're still good.
        int newSize = calculateCapacity(newLength, d_size);
        char **temp = (char **)
                    d_arrayAllocator_p->allocate(sizeof(*d_array_p) * newSize);
        std::memcpy(temp, d_array_p, sizeof(*temp) * d_length);
        std::memset(temp + d_length,
                    0,
                    sizeof(*temp) * (newLength - d_length));

        d_arrayAllocator_p->deallocate(d_array_p);
        d_array_p = temp;
        d_length  = newLength;
        d_size    = newSize;

        return;  // saved 'if' by doing memset in this block; done    // RETURN
    }

    if (newLength > d_length) {
        std::memset(d_array_p + d_length, 0,
                                  sizeof(*d_array_p) * (newLength - d_length));
    }
    else {
        for (int i = newLength; i < d_length; ++i) {
            if (d_array_p[i]) {
                d_stringAllocator_p->deallocate(d_array_p[i]);
            }
        }
    }
    d_length = newLength;
}

void bdea_CstrArray::swap(int index1, int index2)
{
    assert(0 <= index1);  assert(index1 < d_length);
    assert(0 <= index2);  assert(index2 < d_length);

    char *temp        = d_array_p[index1];
    d_array_p[index1] = d_array_p[index2];
    d_array_p[index2] = temp;
}

                        // --------------
                        // FREE OPERATORS
                        // --------------

bool operator==(const bdea_CstrArray& lhs, const bdea_CstrArray& rhs)
{
    if (&lhs == &rhs) { // trivial, cheap test for equality to self
        return true;
    }

    if (lhs.length() != rhs.length()) {
        return false;
    }
    
    int length = lhs.length();
    for (int i = 0; i < length; ++i) {
        const char *s1 = lhs.d_array_p[i];
        const char *s2 = rhs.d_array_p[i];
        if (s1 && s2) {
            if (std::strcmp(s1, s2)) {
                return false;
            }
        }
        else if (!(!s1 && !s2)) {
            return false;
        }
    }
    return true;
}

std::ostream& operator<<(std::ostream &stream, const bdea_CstrArray &rhs)
{
    stream << '[';
    for (int i = 0; i < rhs.length(); i++) {
        stream << ' ';
        if (!rhs[i]) {
            stream << 0;
        }
        else {
            stream << '"' << rhs[i] << '"';
        }

        if (i < rhs.length() - 1) stream << ',';
    }
    stream << " ]";

    return stream;
}

} // end namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
