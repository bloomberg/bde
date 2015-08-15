// bdlcxxx_bitarray.cpp                                               -*-C++-*-
#include <bdlcxxx_bitarray.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bdlb_bitstringutil.h>
#include <bdlb_print.h>

#include <bslalg_swaputil.h>
#include <bslmf_assert.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_ostream.h>
#include <bsl_vector.h>

namespace BloombergLP {

BSLMF_ASSERT(4 == sizeof(int));

namespace bdlcxxx {
// Note that we ensure that the capacity of 'd_array' >= 1 at all times.  This
// way we know that 'd_array.begin()' is not 0.

                        // -------------------
                        // class BitArray
                        // -------------------

// CREATORS
BitArray::BitArray(bslma::Allocator *basicAllocator)
: d_array(basicAllocator)
, d_length(0)
{
    d_array.reserve(1);
}

BitArray::BitArray(int               initialLength,
                             bslma::Allocator *basicAllocator)
: d_array(arraySize(initialLength), 0, basicAllocator)
, d_length(initialLength)
{
    BSLS_ASSERT(0 <= initialLength);

    if (0 == d_length) {
        d_array.reserve(1);
    }
}

BitArray::BitArray(int               initialLength,
                             bool              value,
                             bslma::Allocator *basicAllocator)
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
            bdlb::BitUtil::replaceZero(&d_array[d_array.size() - 1],
                                      pos,
                                      BDEA_BITS_PER_INT - pos);
        }
    }
}

BitArray::BitArray(const InitialCapacity&  numBits,
                             bslma::Allocator       *basicAllocator)
: d_array(basicAllocator)
, d_length(0)
{
    BSLS_ASSERT(0 <= numBits.d_i);

    const int initialCapacity = numBits.d_i ? arraySize(numBits.d_i) : 1;
    d_array.reserve(initialCapacity);
}

BitArray::BitArray(const InitialCapacity&  numBits,
                             int                     initialLength,
                             bslma::Allocator       *basicAllocator)
: d_array(basicAllocator)
, d_length(0)
{
    BSLS_ASSERT(0 <= numBits.d_i);
    BSLS_ASSERT(0 <= initialLength);

    const int initialCapacity = numBits.d_i ? arraySize(numBits.d_i) : 1;
    d_array.reserve(initialCapacity);
    setLength(initialLength);
}

BitArray::BitArray(const InitialCapacity&  numBits,
                             int                     initialLength,
                             bool                    value,
                             bslma::Allocator       *basicAllocator)
: d_array(basicAllocator)
, d_length(0)
{
    BSLS_ASSERT(0 <= numBits.d_i);
    BSLS_ASSERT(0 <= initialLength);

    const int initialCapacity = numBits.d_i ? arraySize(numBits.d_i) : 1;
    d_array.reserve(initialCapacity);
    setLength(initialLength, value);
}

BitArray::BitArray(const BitArray&  original,
                             bslma::Allocator     *basicAllocator)
: d_array(original.d_array, basicAllocator)
, d_length(original.d_length)
{
    if (0 == d_length) {
        d_array.reserve(1);
    }
}

BitArray::~BitArray()
{
    BSLS_ASSERT(0 <= d_length);
    BSLS_ASSERT(arraySize(d_length) == (int)d_array.size());
}

// MANIPULATORS
BitArray& BitArray::operator=(const BitArray& rhs)
{
    // The allocator used by the temporary copy must be the same as the
    // allocator of this object.

    BitArray(rhs, allocator()).swap(*this);
    return *this;
}

BitArray& BitArray::operator&=(const BitArray& rhs)
{
    if (this != &rhs) {
        const int lhsLen  = d_length;
        const int rhsLen  = rhs.d_length;
        const int numBits = lhsLen <= rhsLen ? lhsLen : rhsLen;

        bdlb::BitstringUtil::andEqual(d_array.begin(),
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

BitArray& BitArray::operator|=(const BitArray& rhs)
{
    if (this != &rhs) {
        const int lhsLen  = d_length;
        const int rhsLen  = rhs.d_length;
        const int numBits = lhsLen <= rhsLen ? lhsLen : rhsLen;

        bdlb::BitstringUtil::orEqual(d_array.begin(),
                                    0,
                                    rhs.d_array.begin(),
                                    0,
                                    numBits);
    }

    return *this;
}

BitArray& BitArray::operator^=(const BitArray& rhs)
{
    if (this != &rhs) {
        const int lhsLen = d_length;
        const int rhsLen = rhs.d_length;
        const int numBits = lhsLen <= rhsLen ? lhsLen : rhsLen;

        bdlb::BitstringUtil::xorEqual(d_array.begin(),
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

BitArray& BitArray::operator-=(const BitArray& rhs)
{
    if (this != &rhs) {
        const int lhsLen  = d_length;
        const int rhsLen  = rhs.d_length;
        const int numBits = lhsLen <= rhsLen ? lhsLen : rhsLen;

        bdlb::BitstringUtil::minusEqual(d_array.begin(),
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
}  // close package namespace

bdlcxxx::BitArray& bdlcxxx::BitArray::operator>>=(int numBits)
{
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(     numBits <= d_length);

    if (numBits) {
        if (d_length > numBits) {
            const int remBits = d_length - numBits;

            bdlb::BitstringUtil::copy(d_array.begin(),
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

bdlcxxx::BitArray& bdlcxxx::BitArray::operator<<=(int numBits)
{
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(     numBits <= d_length);

    if (numBits) {
        if (d_length > numBits) {
            const int remBits = d_length - numBits;

            bdlb::BitstringUtil::copy(d_array.begin(),
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

namespace bdlcxxx {
void BitArray::andEqual(int                  dstIndex,
                             const BitArray& srcArray,
                             int                  srcIndex,
                             int                  numBits)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(dstIndex + numBits <= d_length);
    BSLS_ASSERT(srcIndex + numBits <= srcArray.d_length);

    bdlb::BitstringUtil::andEqual(d_array.begin(),
                                 dstIndex,
                                 srcArray.d_array.begin(),
                                 srcIndex,
                                 numBits);
}

void BitArray::minusEqual(int                  dstIndex,
                               const BitArray& srcArray,
                               int                  srcIndex,
                               int                  numBits)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(dstIndex + numBits <= d_length);
    BSLS_ASSERT(srcIndex + numBits <= srcArray.d_length);

    bdlb::BitstringUtil::minusEqual(d_array.begin(),
                                   dstIndex,
                                   srcArray.d_array.begin(),
                                   srcIndex,
                                   numBits);
}

void BitArray::orEqual(int                  dstIndex,
                            const BitArray& srcArray,
                            int                  srcIndex,
                            int                  numBits)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(dstIndex + numBits <= d_length);
    BSLS_ASSERT(srcIndex + numBits <= srcArray.d_length);

    bdlb::BitstringUtil::orEqual(d_array.begin(),
                                dstIndex,
                                srcArray.d_array.begin(),
                                srcIndex,
                                numBits);
}

void BitArray::swap(BitArray& other)
{
    // 'swap' is undefined for objects with non-equal allocators.
    BSLS_ASSERT(allocator() == other.allocator());

    bslalg::SwapUtil::swap(&d_array, &other.d_array);
    bslalg::SwapUtil::swap(&d_length, &other.d_length);
}

void BitArray::swap(int index1, int index2)
{
    BSLS_ASSERT(0 <= index1);
    BSLS_ASSERT(     index1 < d_length);
    BSLS_ASSERT(0 <= index2);
    BSLS_ASSERT(     index2 < d_length);

    if (index1 != index2) {
        // 'swapRaw' is not alias-safe, in general, but the following call is
        // okay since it is only for one bit.

        bdlb::BitstringUtil::swapRaw(d_array.begin(),
                                    index1,
                                    d_array.begin(),
                                    index2,
                                    1);
    }
}

void BitArray::xorEqual(int                  dstIndex,
                             const BitArray& srcArray,
                             int                  srcIndex,
                             int                  numBits)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(dstIndex + numBits <= d_length);
    BSLS_ASSERT(srcIndex + numBits <= srcArray.d_length);

    bdlb::BitstringUtil::xorEqual(d_array.begin(),
                                 dstIndex,
                                 srcArray.d_array.begin(),
                                 srcIndex,
                                 numBits);
}

void BitArray::rotateLeft(int numBits)
{
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(     numBits <= d_length);

    numBits %= d_length;
    const int length = d_length;
    setLength(length + numBits);
    bdlb::BitstringUtil::copy(d_array.begin(),
                             numBits,
                             d_array.begin(),
                             0,
                             length);
    bdlb::BitstringUtil::copy(d_array.begin(),
                             0,
                             d_array.begin(),
                             length,
                             numBits);
    setLength(length);
}

void BitArray::rotateRight(int numBits)
{
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(     numBits <= d_length);

    numBits %= d_length;
    const int length = d_length;
    setLength(length + numBits);
    bdlb::BitstringUtil::copy(d_array.begin(),
                             length,
                             d_array.begin(),
                             0,
                             numBits);
    bdlb::BitstringUtil::copy(d_array.begin(),
                             0,
                             d_array.begin(),
                             numBits,
                             length);
    setLength(length);
}

void BitArray::setAll1()
{
    bdlb::BitstringUtil::set(d_array.begin(), 0, true, d_length);
}

void BitArray::set1(int index, int numBits)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(index + numBits <= d_length);

    bdlb::BitstringUtil::set(d_array.begin(), index, true, numBits);
}

void BitArray::setAll0()
{
    bdlb::BitstringUtil::set(d_array.begin(), 0, false, d_length);
}

void BitArray::set0(int index, int numBits)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(index + numBits <= d_length);

    bdlb::BitstringUtil::set(d_array.begin(), index, false, numBits);
}

void BitArray::toggle(int index, int numBits)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(index + numBits <= d_length);

    bdlb::BitstringUtil::toggle(d_array.begin(), index, numBits);
}

void BitArray::toggleAll()
{
    toggle(0, d_length);
}

void BitArray::append(const BitArray& srcArray)
{
    insert(d_length, srcArray, 0, srcArray.d_length);
}

void BitArray::append(const BitArray& srcArray,
                           int                  srcIndex,
                           int                  numBits)
{
    insert(d_length, srcArray, srcIndex, numBits);
}

void BitArray::insert(int dstIndex, bool value)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= d_length);

    setLength(d_length + 1);
    bdlb::BitstringUtil::insert(d_array.begin(),
                               d_length - 1,
                               dstIndex,
                               value,
                               1);
}

void BitArray::insert(int dstIndex, bool value, int numBits)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= d_length);
    BSLS_ASSERT(0 <= numBits);

    setLength(d_length + numBits);
    bdlb::BitstringUtil::insert(d_array.begin(),
                               d_length - numBits,
                               dstIndex,
                               value,
                               numBits);
}

void BitArray::insert(int dstIndex, const BitArray& srcArray)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= d_length);

    insert(dstIndex, srcArray, 0, srcArray.d_length);
}

void BitArray::insert(int                  dstIndex,
                           const BitArray& srcArray,
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

    bdlb::BitstringUtil::copy(d_array.begin(),
                             dstIndex + numBits,
                             d_array.begin(),
                             dstIndex,
                             oldLength - dstIndex);

    if (this != &srcArray || dstIndex >= srcIndex + numBits) {
        bdlb::BitstringUtil::copyRaw(d_array.begin(),
                                    dstIndex,
                                    srcArray.d_array.begin(),
                                    srcIndex,
                                    numBits);
    }
    else if (dstIndex < srcIndex) {
        bdlb::BitstringUtil::copyRaw(d_array.begin(),
                                    dstIndex,
                                    srcArray.d_array.begin(),
                                    srcIndex + numBits,
                                    numBits);
    }
    else {
        const int leftLen  = dstIndex - srcIndex;
        const int rightLen = numBits - leftLen;
        bdlb::BitstringUtil::copyRaw(d_array.begin(),
                                    dstIndex,
                                    srcArray.d_array.begin(),
                                    srcIndex,
                                    leftLen);
        bdlb::BitstringUtil::copyRaw(d_array.begin(),
                                    dstIndex + leftLen,
                                    srcArray.d_array.begin(),
                                    srcIndex + leftLen + numBits,
                                    rightLen);
    }
}

void BitArray::remove(int index)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    remove(index, 1);
}

void BitArray::remove(int index, int numBits)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(index + numBits <= d_length);

    bdlb::BitstringUtil::removeAndFill0(d_array.begin(),
                                       d_length,
                                       index,
                                       numBits);
    setLength(d_length - numBits);
}

void BitArray::replace(int                  dstIndex,
                            const BitArray& srcArray,
                            int                  srcIndex,
                            int                  numBits)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= d_length);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numBits);
    BSLS_ASSERT(dstIndex + numBits <= d_length);
    BSLS_ASSERT(srcIndex + numBits <= srcArray.d_length);

    bdlb::BitstringUtil::copy(d_array.begin(),
                             dstIndex,
                             srcArray.d_array.begin(),
                             srcIndex,
                             numBits);
}

void BitArray::setLengthRaw(int newLength)
{
    BSLS_ASSERT(0 <= newLength);

    d_array.resize(arraySize(newLength));
    d_length = newLength;
}

void BitArray::setLength(int newLength)
{
    setLengthRaw(newLength);
}

void BitArray::setLength(int newLength, bool value)
{
    BSLS_ASSERT(0 <= newLength);

    const int length = d_length;
    setLengthRaw(newLength);
    if (value && newLength > length) {
        set1(length, newLength - length);
    }
}

// ACCESSORS
int BitArray::find0AtLargestIndex() const
{
    return bdlb::BitstringUtil::find0AtLargestIndex(d_array.begin(), d_length);
}

int BitArray::find0AtLargestIndexGE(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdlb::BitstringUtil::find0AtLargestIndexGE(d_array.begin(),
                                                     d_length,
                                                     index);
}

int BitArray::find0AtLargestIndexGT(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdlb::BitstringUtil::find0AtLargestIndexGT(d_array.begin(),
                                                     d_length,
                                                     index);
}

int BitArray::find0AtLargestIndexLE(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdlb::BitstringUtil::find0AtLargestIndexLE(d_array.begin(), index);
}

int BitArray::find0AtLargestIndexLT(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdlb::BitstringUtil::find0AtLargestIndexLT(d_array.begin(), index);
}

int BitArray::find0AtSmallestIndex() const
{
    return bdlb::BitstringUtil::find0AtSmallestIndex(d_array.begin(), d_length);
}

int BitArray::find0AtSmallestIndexGE(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdlb::BitstringUtil::find0AtSmallestIndexGE(d_array.begin(),
                                                      d_length,
                                                      index);
}

int BitArray::find0AtSmallestIndexGT(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdlb::BitstringUtil::find0AtSmallestIndexGT(d_array.begin(),
                                                      d_length,
                                                      index);
}

int BitArray::find0AtSmallestIndexLE(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdlb::BitstringUtil::find0AtSmallestIndexLE(d_array.begin(), index);
}

int BitArray::find0AtSmallestIndexLT(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdlb::BitstringUtil::find0AtSmallestIndexLT(d_array.begin(), index);
}

int BitArray::find1AtLargestIndex() const
{
    return bdlb::BitstringUtil::find1AtLargestIndex(d_array.begin(), d_length);
}

int BitArray::find1AtLargestIndexGE(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdlb::BitstringUtil::find1AtLargestIndexGE(d_array.begin(),
                                                     d_length,
                                                     index);
}

int BitArray::find1AtLargestIndexGT(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdlb::BitstringUtil::find1AtLargestIndexGT(d_array.begin(),
                                                     d_length,
                                                     index);
}

int BitArray::find1AtLargestIndexLE(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdlb::BitstringUtil::find1AtLargestIndexLE(d_array.begin(), index);
}

int BitArray::find1AtLargestIndexLT(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdlb::BitstringUtil::find1AtLargestIndexLT(d_array.begin(), index);
}

int BitArray::find1AtSmallestIndex() const
{
    return bdlb::BitstringUtil::find1AtSmallestIndex(d_array.begin(), d_length);
}

int BitArray::find1AtSmallestIndexGE(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdlb::BitstringUtil::find1AtSmallestIndexGE(d_array.begin(),
                                                      d_length,
                                                      index);
}

int BitArray::find1AtSmallestIndexGT(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdlb::BitstringUtil::find1AtSmallestIndexGT(d_array.begin(),
                                                      d_length,
                                                      index);
}

int BitArray::find1AtSmallestIndexLE(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdlb::BitstringUtil::find1AtSmallestIndexLE(d_array.begin(), index);
}

int BitArray::find1AtSmallestIndexLT(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < d_length);

    return bdlb::BitstringUtil::find1AtSmallestIndexLT(d_array.begin(), index);
}

bool BitArray::isAnySet0() const
{
    return bdlb::BitstringUtil::isAny0(d_array.begin(), 0, d_length);
}

bool BitArray::isAnySet1() const
{
    return bdlb::BitstringUtil::isAny1(d_array.begin(), 0, d_length);
}

int BitArray::numSet0() const
{
    return bdlb::BitstringUtil::num0(d_array.begin(), 0, d_length);
}

int BitArray::numSet1() const
{
    return bdlb::BitstringUtil::num1(d_array.begin(), 0, d_length);
}

bsl::ostream& BitArray::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    if (stream) {
        if (spacesPerLevel > 0) {
            bdlb::Print::indent(stream, level, spacesPerLevel);
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
                bdlb::Print::newlineAndIndent(stream,
                                             levelPlus1,
                                             spacesPerLevel);
            }
            stream << (*this)[i];
        }

        bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
        stream << "]";
        if (spacesPerLevel >= 0) {
            stream << '\n';
        }
    }
    return stream;
}
}  // close package namespace

// FREE OPERATORS
bool bdlcxxx::operator==(const BitArray& lhs, const BitArray& rhs)
{
    if (lhs.d_length != rhs.d_length) {
        return 0;                                                     // RETURN
    }

    return bdlb::BitstringUtil::areEqual(lhs.d_array.begin(),
                                        0,
                                        rhs.d_array.begin(),
                                        0,
                                        lhs.d_length);
}

bdlcxxx::BitArray bdlcxxx::operator~(const BitArray& array)
{
    BitArray tmp(array);
    tmp.toggleAll();
    return tmp;
}

bdlcxxx::BitArray bdlcxxx::operator<<(const BitArray& array, int numBits)
{
    BSLS_ASSERT(0 <= numBits);

    BitArray tmp(array);
    tmp <<= numBits;
    return tmp;
}

bdlcxxx::BitArray bdlcxxx::operator>>(const BitArray& array, int numBits)
{
    BSLS_ASSERT(0 <= numBits);

    BitArray tmp(array);
    tmp >>= numBits;
    return tmp;
}

bdlcxxx::BitArray bdlcxxx::operator&(const BitArray& lhs, const BitArray& rhs)
{
    BitArray tmp(lhs);
    tmp &= rhs;
    return tmp;
}

bdlcxxx::BitArray bdlcxxx::operator|(const BitArray& lhs, const BitArray& rhs)
{
    BitArray tmp(lhs);
    tmp |= rhs;
    return tmp;
}

bdlcxxx::BitArray bdlcxxx::operator^(const BitArray& lhs, const BitArray& rhs)
{
    BitArray tmp(lhs);
    tmp ^= rhs;
    return tmp;
}

bdlcxxx::BitArray bdlcxxx::operator-(const BitArray& lhs, const BitArray& rhs)
{
    BitArray tmp(lhs);
    tmp -= rhs;
    return tmp;
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
