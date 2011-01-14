// bdea_bitarray.cpp                                                  -*-C++-*-
#include <bdea_bitarray.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bdeu_bitstringutil.h>
#include <bdeu_print.h>

#include <bslmf_assert.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_ostream.h>
#include <bsl_vector.h>

namespace BloombergLP {

BSLMF_ASSERT(4 == sizeof(int));

// Note that we ensure that the capacity of 'd_array' >= 1 at all times.  This
// way we know that 'd_array.begin()' is not 0.

                        // -------------------
                        // class bdea_BitArray
                        // -------------------

// CREATORS
bdea_BitArray::bdea_BitArray(bslma_Allocator *basicAllocator)
: d_array(basicAllocator)
, d_length(0)
{
    d_array.reserve(1);
}

bdea_BitArray::bdea_BitArray(int              initialLength,
                             bslma_Allocator *basicAllocator)
: d_array(arraySize(initialLength), 0, basicAllocator)
, d_length(initialLength)
{
    BSLS_ASSERT(0 <= initialLength);

    if (0 == d_length) {
        d_array.reserve(1);
    }
}

bdea_BitArray::bdea_BitArray(int              initialLength,
                             bool             value,
                             bslma_Allocator *basicAllocator)
: d_array(arraySize(initialLength), value ? ~0 : 0, basicAllocator)
, d_length(initialLength)
{
    BSLS_ASSERT(0 <= initialLength);

    if (0 == d_length) {
        d_array.reserve(1);
    }
    else {
        const int pos = initialLength % BDEA_BITS_PER_INT;
        if (value && pos) {
            bdes_BitUtil::replaceZero(&d_array[d_array.size() - 1],
                                      pos,
                                      BDEA_BITS_PER_INT - pos);
        }
    }
}

bdea_BitArray::bdea_BitArray(const InitialCapacity&  numBits,
                             bslma_Allocator        *basicAllocator)
: d_array(basicAllocator)
, d_length(0)
{
    BSLS_ASSERT(0 <= numBits.d_i);

    const int initialCapacity = numBits.d_i ? arraySize(numBits.d_i) : 1;
    d_array.reserve(initialCapacity);
}

bdea_BitArray::bdea_BitArray(const InitialCapacity&  numBits,
                             int                     initialLength,
                             bslma_Allocator        *basicAllocator)
: d_array(basicAllocator)
, d_length(0)
{
    BSLS_ASSERT(0 <= numBits.d_i);
    BSLS_ASSERT(0 <= initialLength);

    const int initialCapacity = numBits.d_i ? arraySize(numBits.d_i) : 1;
    d_array.reserve(initialCapacity);
    setLength(initialLength);
}

bdea_BitArray::bdea_BitArray(const InitialCapacity&  numBits,
                             int                     initialLength,
                             bool                    value,
                             bslma_Allocator        *basicAllocator)
: d_array(basicAllocator)
, d_length(0)
{
    BSLS_ASSERT(0 <= numBits.d_i);
    BSLS_ASSERT(0 <= initialLength);

    const int initialCapacity = numBits.d_i ? arraySize(numBits.d_i) : 1;
    d_array.reserve(initialCapacity);
    setLength(initialLength, value);
}

bdea_BitArray::bdea_BitArray(const bdea_BitArray&  original,
                             bslma_Allocator      *basicAllocator)
: d_array(original.d_array, basicAllocator)
, d_length(original.d_length)
{
    if (0 == d_length) {
        d_array.reserve(1);
    }
}

bdea_BitArray::~bdea_BitArray()
{
    BSLS_ASSERT(0 <= d_length);
    BSLS_ASSERT(arraySize(d_length) == (int)d_array.size());
}

// MANIPULATORS
bdea_BitArray& bdea_BitArray::operator=(const bdea_BitArray& rhs)
{
    // The allocator used by the temporary copy must be the same as the
    // allocator of this object.

    bdea_BitArray(rhs, allocator()).swap(*this);
    return *this;
}

bdea_BitArray& bdea_BitArray::operator&=(const bdea_BitArray& rhs)
{
    if (this != &rhs) {
        const int lhsLen  = d_length;
        const int rhsLen  = rhs.d_length;
        const int numBits = lhsLen <= rhsLen ? lhsLen : rhsLen;

        bdeu_BitstringUtil::andEqual(d_array.begin(),
                                     0,
                                     rhs.d_array.begin(),
                                     0,
                                     numBits);

        if (lhsLen > rhsLen) {
            set0(rhsLen, lhsLen - rhsLen);
        }
    }

    return *this;
}

bdea_BitArray& bdea_BitArray::operator|=(const bdea_BitArray& rhs)
{
    if (this != &rhs) {
        const int lhsLen  = d_length;
        const int rhsLen  = rhs.d_length;
        const int numBits = lhsLen <= rhsLen ? lhsLen : rhsLen;

        bdeu_BitstringUtil::orEqual(d_array.begin(),
                                    0,
                                    rhs.d_array.begin(),
                                    0,
                                    numBits);
    }

    return *this;
}

bdea_BitArray& bdea_BitArray::operator^=(const bdea_BitArray& rhs)
{
    if (this != &rhs) {
        const int lhsLen = d_length;
        const int rhsLen = rhs.d_length;
        const int numBits = lhsLen <= rhsLen ? lhsLen : rhsLen;

        bdeu_BitstringUtil::xorEqual(d_array.begin(),
                                     0,
                                     rhs.d_array.begin(),
                                     0,
                                     numBits);
    }
    else {
        setAll0();
    }

    return *this;
}

bdea_BitArray& bdea_BitArray::operator-=(const bdea_BitArray& rhs)
{
    if (this != &rhs) {
        const int lhsLen  = d_length;
        const int rhsLen  = rhs.d_length;
        const int numBits = lhsLen <= rhsLen ? lhsLen : rhsLen;

        bdeu_BitstringUtil::minusEqual(d_array.begin(),
                                       0,
                                       rhs.d_array.begin(),
                                       0,
                                       numBits);
    }
    else {
        setAll0();
    }

    return *this;
}

bdea_BitArray& bdea_BitArray::operator>>=(int numBits)
{
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(     numBits <= d_length);

    if (numBits) {
        if (d_length > numBits) {
            const int remBits = d_length - numBits;

            bdeu_BitstringUtil::copy(d_array.begin(),
                                     0,
                                     d_array.begin(),
                                     numBits,
                                     remBits);
            set0(remBits, numBits);
        }
        else {
            setAll0();
        }
    }
    return *this;
}

bdea_BitArray& bdea_BitArray::operator<<=(int numBits)
{
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(     numBits <= d_length);

    if (numBits) {
        if (d_length > numBits) {
            const int remBits = d_length - numBits;

            bdeu_BitstringUtil::copy(d_array.begin(),
                                     numBits,
                                     d_array.begin(),
                                     0,
                                     remBits);
            set0(0, numBits);
        }
        else {
            setAll0();
        }
    }
    return *this;
}

void bdea_BitArray::andEqual(int                  dstIndex,
                             const bdea_BitArray& srcArray,
                             int                  srcIndex,
                             int                  numBits)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(dstIndex + numBits <= d_length);
    BSLS_ASSERT(srcIndex + numBits <= srcArray.d_length);

    bdeu_BitstringUtil::andEqual(d_array.begin(),
                                 dstIndex,
                                 srcArray.d_array.begin(),
                                 srcIndex,
                                 numBits);
}

void bdea_BitArray::minusEqual(int                  dstIndex,
                               const bdea_BitArray& srcArray,
                               int                  srcIndex,
                               int                  numBits)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(dstIndex + numBits <= d_length);
    BSLS_ASSERT(srcIndex + numBits <= srcArray.d_length);

    bdeu_BitstringUtil::minusEqual(d_array.begin(),
                                   dstIndex,
                                   srcArray.d_array.begin(),
                                   srcIndex,
                                   numBits);
}

void bdea_BitArray::orEqual(int                  dstIndex,
                            const bdea_BitArray& srcArray,
                            int                  srcIndex,
                            int                  numBits)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(dstIndex + numBits <= d_length);
    BSLS_ASSERT(srcIndex + numBits <= srcArray.d_length);

    bdeu_BitstringUtil::orEqual(d_array.begin(),
                                dstIndex,
                                srcArray.d_array.begin(),
                                srcIndex,
                                numBits);
}

void bdea_BitArray::swap(bdea_BitArray& other)
{
    // 'swap' is undefined for objects with non-equal allocators.

    BSLS_ASSERT(allocator() == other.allocator());

    using bsl::swap;

    swap(d_array, other.d_array);

    // We cannot use the 'swap' found by ADL to swap lengths because there is
    // a member 'swap' with two parameters of the same type as that found by
    // ADL.

    bsl::swap(d_length, other.d_length);
}

void bdea_BitArray::swap(int index1, int index2)
{
    BSLS_ASSERT(0 <= index1);
    BSLS_ASSERT(     index1 < d_length);
    BSLS_ASSERT(0 <= index2);
    BSLS_ASSERT(     index2 < d_length);

    if (index1 != index2) {
        // 'swapRaw' is not alias-safe, in general, but the following call is
        // okay since it is only for one bit.

        bdeu_BitstringUtil::swapRaw(d_array.begin(),
                                    index1,
                                    d_array.begin(),
                                    index2,
                                    1);
    }
}

void bdea_BitArray::xorEqual(int                  dstIndex,
                             const bdea_BitArray& srcArray,
                             int                  srcIndex,
                             int                  numBits)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(dstIndex + numBits <= d_length);
    BSLS_ASSERT(srcIndex + numBits <= srcArray.d_length);

    bdeu_BitstringUtil::xorEqual(d_array.begin(),
                                 dstIndex,
                                 srcArray.d_array.begin(),
                                 srcIndex,
                                 numBits);
}

void bdea_BitArray::rotateLeft(int numBits)
{
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(     numBits <= d_length);

    numBits %= d_length;
    const int length = d_length;
    setLength(length + numBits);
    bdeu_BitstringUtil::copy(d_array.begin(),
                             numBits,
                             d_array.begin(),
                             0,
                             length);
    bdeu_BitstringUtil::copy(d_array.begin(),
                             0,
                             d_array.begin(),
                             length,
                             numBits);
    setLength(length);
}

void bdea_BitArray::rotateRight(int numBits)
{
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(     numBits <= d_length);

    numBits %= d_length;
    const int length = d_length;
    setLength(length + numBits);
    bdeu_BitstringUtil::copy(d_array.begin(),
                             length,
                             d_array.begin(),
                             0,
                             numBits);
    bdeu_BitstringUtil::copy(d_array.begin(),
                             0,
                             d_array.begin(),
                             numBits,
                             length);
    setLength(length);
}

void bdea_BitArray::setAll1()
{
    bdeu_BitstringUtil::set(d_array.begin(), 0, true, d_length);
}

void bdea_BitArray::set1(int index, int numBits)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(index + numBits <= d_length);

    bdeu_BitstringUtil::set(d_array.begin(), index, true, numBits);
}

void bdea_BitArray::setAll0()
{
    bdeu_BitstringUtil::set(d_array.begin(), 0, false, d_length);
}

void bdea_BitArray::set0(int index, int numBits)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(index + numBits <= d_length);

    bdeu_BitstringUtil::set(d_array.begin(), index, false, numBits);
}

void bdea_BitArray::toggle(int index, int numBits)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(index + numBits <= d_length);

    bdeu_BitstringUtil::toggle(d_array.begin(), index, numBits);
}

void bdea_BitArray::toggleAll()
{
    toggle(0, d_length);
}

void bdea_BitArray::append(const bdea_BitArray& srcArray)
{
    insert(d_length, srcArray, 0, srcArray.d_length);
}

void bdea_BitArray::append(const bdea_BitArray& srcArray,
                           int                  srcIndex,
                           int                  numBits)
{
    insert(d_length, srcArray, srcIndex, numBits);
}

void bdea_BitArray::insert(int dstIndex, bool value)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= d_length);

    setLength(d_length + 1);
    bdeu_BitstringUtil::insert(d_array.begin(),
                               d_length - 1,
                               dstIndex,
                               value,
                               1);
}

void bdea_BitArray::insert(int dstIndex, bool value, int numBits)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= d_length);
    BSLS_ASSERT(0 <= numBits);

    setLength(d_length + numBits);
    bdeu_BitstringUtil::insert(d_array.begin(),
                               d_length - numBits,
                               dstIndex,
                               value,
                               numBits);
}

void bdea_BitArray::insert(int dstIndex, const bdea_BitArray& srcArray)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= d_length);

    insert(dstIndex, srcArray, 0, srcArray.d_length);
}

void bdea_BitArray::insert(int                  dstIndex,
                           const bdea_BitArray& srcArray,
                           int                  srcIndex,
                           int                  numBits)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= d_length);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(srcIndex + numBits <= srcArray.d_length);

    const int oldLength = d_length;
    setLength(d_length + numBits);

    bdeu_BitstringUtil::copy(d_array.begin(),
                             dstIndex + numBits,
                             d_array.begin(),
                             dstIndex,
                             oldLength - dstIndex);

    if (this != &srcArray || dstIndex >= srcIndex + numBits) {
        bdeu_BitstringUtil::copyRaw(d_array.begin(),
                                    dstIndex,
                                    srcArray.d_array.begin(),
                                    srcIndex,
                                    numBits);
    }
    else if (dstIndex < srcIndex) {
        bdeu_BitstringUtil::copyRaw(d_array.begin(),
                                    dstIndex,
                                    srcArray.d_array.begin(),
                                    srcIndex + numBits,
                                    numBits);
    }
    else {
        const int leftLen  = dstIndex - srcIndex;
        const int rightLen = numBits - leftLen;
        bdeu_BitstringUtil::copyRaw(d_array.begin(),
                                    dstIndex,
                                    srcArray.d_array.begin(),
                                    srcIndex,
                                    leftLen);
        bdeu_BitstringUtil::copyRaw(d_array.begin(),
                                    dstIndex + leftLen,
                                    srcArray.d_array.begin(),
                                    srcIndex + leftLen + numBits,
                                    rightLen);
    }
}

void bdea_BitArray::remove(int index)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    remove(index, 1);
}

void bdea_BitArray::remove(int index, int numBits)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(index + numBits <= d_length);

    bdeu_BitstringUtil::removeAndFill0(d_array.begin(),
                                       d_length,
                                       index,
                                       numBits);
    setLength(d_length - numBits);
}

void bdea_BitArray::replace(int                  dstIndex,
                            const bdea_BitArray& srcArray,
                            int                  srcIndex,
                            int                  numBits)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= d_length);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(dstIndex + numBits <= d_length);
    BSLS_ASSERT(srcIndex + numBits <= srcArray.d_length);

    bdeu_BitstringUtil::copy(d_array.begin(),
                             dstIndex,
                             srcArray.d_array.begin(),
                             srcIndex,
                             numBits);
}

void bdea_BitArray::setLengthRaw(int newLength)
{
    BSLS_ASSERT(0 <= newLength);

    d_array.resize(arraySize(newLength));
    d_length = newLength;
}

void bdea_BitArray::setLength(int newLength)
{
    setLengthRaw(newLength);
}

void bdea_BitArray::setLength(int newLength, bool value)
{
    BSLS_ASSERT(0 <= newLength);

    const int length = d_length;
    setLengthRaw(newLength);
    if (value && newLength > length) {
        set1(length, newLength - length);
    }
}

// ACCESSORS
int bdea_BitArray::find0AtLargestIndex() const
{
    return bdeu_BitstringUtil::find0AtLargestIndex(d_array.begin(), d_length);
}

int bdea_BitArray::find0AtLargestIndexGE(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdeu_BitstringUtil::find0AtLargestIndexGE(d_array.begin(),
                                                     d_length,
                                                     index);
}

int bdea_BitArray::find0AtLargestIndexGT(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdeu_BitstringUtil::find0AtLargestIndexGT(d_array.begin(),
                                                     d_length,
                                                     index);
}

int bdea_BitArray::find0AtLargestIndexLE(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdeu_BitstringUtil::find0AtLargestIndexLE(d_array.begin(), index);
}

int bdea_BitArray::find0AtLargestIndexLT(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdeu_BitstringUtil::find0AtLargestIndexLT(d_array.begin(), index);
}

int bdea_BitArray::find0AtSmallestIndex() const
{
    return bdeu_BitstringUtil::find0AtSmallestIndex(d_array.begin(), d_length);
}

int bdea_BitArray::find0AtSmallestIndexGE(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdeu_BitstringUtil::find0AtSmallestIndexGE(d_array.begin(),
                                                      d_length,
                                                      index);
}

int bdea_BitArray::find0AtSmallestIndexGT(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdeu_BitstringUtil::find0AtSmallestIndexGT(d_array.begin(),
                                                      d_length,
                                                      index);
}

int bdea_BitArray::find0AtSmallestIndexLE(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdeu_BitstringUtil::find0AtSmallestIndexLE(d_array.begin(), index);
}

int bdea_BitArray::find0AtSmallestIndexLT(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdeu_BitstringUtil::find0AtSmallestIndexLT(d_array.begin(), index);
}

int bdea_BitArray::find1AtLargestIndex() const
{
    return bdeu_BitstringUtil::find1AtLargestIndex(d_array.begin(), d_length);
}

int bdea_BitArray::find1AtLargestIndexGE(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdeu_BitstringUtil::find1AtLargestIndexGE(d_array.begin(),
                                                     d_length,
                                                     index);
}

int bdea_BitArray::find1AtLargestIndexGT(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdeu_BitstringUtil::find1AtLargestIndexGT(d_array.begin(),
                                                     d_length,
                                                     index);
}

int bdea_BitArray::find1AtLargestIndexLE(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdeu_BitstringUtil::find1AtLargestIndexLE(d_array.begin(), index);
}

int bdea_BitArray::find1AtLargestIndexLT(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdeu_BitstringUtil::find1AtLargestIndexLT(d_array.begin(), index);
}

int bdea_BitArray::find1AtSmallestIndex() const
{
    return bdeu_BitstringUtil::find1AtSmallestIndex(d_array.begin(), d_length);
}

int bdea_BitArray::find1AtSmallestIndexGE(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdeu_BitstringUtil::find1AtSmallestIndexGE(d_array.begin(),
                                                      d_length,
                                                      index);
}

int bdea_BitArray::find1AtSmallestIndexGT(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdeu_BitstringUtil::find1AtSmallestIndexGT(d_array.begin(),
                                                      d_length,
                                                      index);
}

int bdea_BitArray::find1AtSmallestIndexLE(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdeu_BitstringUtil::find1AtSmallestIndexLE(d_array.begin(), index);
}

int bdea_BitArray::find1AtSmallestIndexLT(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdeu_BitstringUtil::find1AtSmallestIndexLT(d_array.begin(), index);
}

bool bdea_BitArray::isAnySet0() const
{
    return bdeu_BitstringUtil::isAny0(d_array.begin(), 0, d_length);
}

bool bdea_BitArray::isAnySet1() const
{
    return bdeu_BitstringUtil::isAny1(d_array.begin(), 0, d_length);
}

int bdea_BitArray::numSet0() const
{
    return bdeu_BitstringUtil::num0(d_array.begin(), 0, d_length);
}

int bdea_BitArray::numSet1() const
{
    return bdeu_BitstringUtil::num1(d_array.begin(), 0, d_length);
}

bsl::ostream& bdea_BitArray::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    if (stream) {
        const char SPACE = spacesPerLevel < 0 ? ' ' : '\n';
        if (spacesPerLevel > 0) {
            bdeu_Print::indent(stream, level, spacesPerLevel);
        }
        stream << "[";
        if (spacesPerLevel < 0 && d_length > 0) {
            stream << ' ';
        }

        if (level < 0) {
            level = -level;
        }

        int levelPlus1 = level + 1;

        for (int i = 0; i < d_length; ++i) {
            if (spacesPerLevel >= 0) {
                bdeu_Print::newlineAndIndent(stream,
                                             levelPlus1,
                                             spacesPerLevel);
            }
            stream << (*this)[i];
        }

        bdeu_Print::newlineAndIndent(stream, level, spacesPerLevel);
        stream << "]";
        if (spacesPerLevel >= 0) {
            stream << '\n';
        }
    }
    return stream;
}

// FREE OPERATORS
bool operator==(const bdea_BitArray& lhs, const bdea_BitArray& rhs)
{
    if (lhs.d_length != rhs.d_length) {
        return 0;                                                     // RETURN
    }

    return bdeu_BitstringUtil::areEqual(lhs.d_array.begin(),
                                        0,
                                        rhs.d_array.begin(),
                                        0,
                                        lhs.d_length);
}

bdea_BitArray operator~(const bdea_BitArray& array)
{
    bdea_BitArray tmp(array);
    tmp.toggleAll();
    return tmp;
}

bdea_BitArray operator<<(const bdea_BitArray& array, int numBits)
{
    BSLS_ASSERT(0 <= numBits);

    bdea_BitArray tmp(array);
    tmp <<= numBits;
    return tmp;
}

bdea_BitArray operator>>(const bdea_BitArray& array, int numBits)
{
    BSLS_ASSERT(0 <= numBits);

    bdea_BitArray tmp(array);
    tmp >>= numBits;
    return tmp;
}

bdea_BitArray operator&(const bdea_BitArray& lhs, const bdea_BitArray& rhs)
{
    bdea_BitArray tmp(lhs);
    tmp &= rhs;
    return tmp;
}

bdea_BitArray operator|(const bdea_BitArray& lhs, const bdea_BitArray& rhs)
{
    bdea_BitArray tmp(lhs);
    tmp |= rhs;
    return tmp;
}

bdea_BitArray operator^(const bdea_BitArray& lhs, const bdea_BitArray& rhs)
{
    bdea_BitArray tmp(lhs);
    tmp ^= rhs;
    return tmp;
}

bdea_BitArray operator-(const bdea_BitArray& lhs, const bdea_BitArray& rhs)
{
    bdea_BitArray tmp(lhs);
    tmp -= rhs;
    return tmp;
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
