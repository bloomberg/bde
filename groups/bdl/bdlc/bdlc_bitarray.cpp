// bdlc_bitarray.cpp                                                  -*-C++-*-
#include <bdlc_bitarray.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlc_bitarray_cpp,"$Id$ $CSID$")

#include <bdlb_bitstringutil.h>

#include <bslmf_assert.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_ostream.h>
#include <bsl_vector.h>

#include <bsl_c_limits.h>    // 'CHAR_BIT'

using bsl::size_t;
using bsl::uint64_t;

// Note that we ensure that the capacity of 'd_array' is at least 1 at all
// times.  This way we know that '&d_array.front()' is always valid.

namespace BloombergLP {
namespace bdlc {

static inline
uint64_t rawLt64(int numBits)
    // Return a value with only the specified low-order 'numBits' set.  The
    // behavior is undefined unless '0 <= numBits < k_BITS_PER_UINT64'.  Note
    // that this is a faster version of 'bdlb::BitMaskUtil::lt64' with a
    // narrower contract.
{
    BSLS_ASSERT_SAFE(0 <= numBits);
    BSLS_ASSERT_SAFE(     numBits < BitArray::k_BITS_PER_UINT64);

    return (static_cast<uint64_t>(1) << numBits) - 1;
}

BSLMF_ASSERT(sizeof(uint64_t) * CHAR_BIT == BitArray::k_BITS_PER_UINT64);

                                // --------------
                                // class BitArray
                                // --------------

// CREATORS
BitArray::BitArray(bslma::Allocator *basicAllocator)
: d_array(basicAllocator)
, d_length(0)
{
    d_array.resize(1);
}

BitArray::BitArray(size_t            initialLength,
                   bslma::Allocator *basicAllocator)
: d_array(arraySize(initialLength), 0, basicAllocator)
, d_length(initialLength)
{
}

BitArray::BitArray(size_t            initialLength,
                   bool              value,
                   bslma::Allocator *basicAllocator)
: d_array(arraySize(initialLength), value ? s_minusOne : 0, basicAllocator)
, d_length(initialLength)
{
    const int pos = initialLength % k_BITS_PER_UINT64;
    if (value && (pos || !initialLength)) {
        d_array.back() &= rawLt64(pos);
    }
}

BitArray::BitArray(const BitArray&   original,
                   bslma::Allocator *basicAllocator)
: d_array(original.d_array, basicAllocator)
, d_length(original.d_length)
{
    BSLS_ASSERT_SAFE(!d_array.empty());
}

BitArray::~BitArray()
{
    const size_t estimated = arraySize(d_length);
    const size_t actual    = d_array.size();
    BSLS_ASSERT(estimated == actual);
    BSLS_ASSERT(0 <          actual);
}

// MANIPULATORS
void BitArray::insert(size_t          dstIndex,
                      const BitArray& srcArray,
                      size_t          srcIndex,
                      size_t          numBits)
{
    BSLS_ASSERT(dstIndex <= d_length);
    BSLS_ASSERT(srcIndex + numBits <= srcArray.d_length);

    const size_t oldLength = d_length;
    setLength(d_length + numBits);

    bdlb::BitStringUtil::insertRaw(data(),
                                   oldLength,
                                   dstIndex,
                                   numBits);

    if (this != &srcArray || dstIndex >= srcIndex + numBits) {
        bdlb::BitStringUtil::copyRaw(data(),
                                     dstIndex,
                                     srcArray.data(),
                                     srcIndex,
                                     numBits);
    }
    else if (dstIndex < srcIndex) {
        bdlb::BitStringUtil::copyRaw(data(),
                                     dstIndex,
                                     data(),
                                     srcIndex + numBits,
                                     numBits);
    }
    else {
        const size_t leftLen  = dstIndex - srcIndex;
        const size_t rightLen = numBits - leftLen;
        bdlb::BitStringUtil::copyRaw(data(),
                                     dstIndex,
                                     data(),
                                     srcIndex,
                                     leftLen);
        bdlb::BitStringUtil::copyRaw(data(),
                                     dstIndex + leftLen,
                                     data(),
                                     srcIndex + leftLen + numBits,
                                     rightLen);
    }
}

void BitArray::rotateLeft(size_t numBits)
{
    BSLS_ASSERT(numBits <= d_length);

    if (numBits == d_length || 0 == numBits) {
        return;                                                       // RETURN
    }

    if (numBits > d_length / 2) {
        rotateRight(d_length - numBits);

        return;                                                       // RETURN
    }

    const size_t oldLen = d_length;
    setLength(oldLen + numBits);

    bdlb::BitStringUtil::copy(   data(), numBits, data(),      0, oldLen);
    bdlb::BitStringUtil::copyRaw(data(), 0,       data(), oldLen, numBits);

    setLength(oldLen);
}

void BitArray::rotateRight(size_t numBits)
{
    BSLS_ASSERT(numBits <= d_length);

    if (numBits == d_length || 0 == numBits) {
        return;                                                       // RETURN
    }

    if (numBits > d_length / 2) {
        rotateLeft(d_length - numBits);

        return;                                                       // RETURN
    }

    const size_t oldLen = d_length;
    setLength(oldLen + numBits);

    bdlb::BitStringUtil::copyRaw(data(), oldLen, data(),       0, numBits);
    bdlb::BitStringUtil::copyRaw(data(), 0,      data(), numBits, oldLen);

    setLength(oldLen);
}

void BitArray::setLength(size_t newLength, bool value)
{
    const size_t oldLength = d_length;
    const int    oldPos    = static_cast<unsigned>(oldLength) %
                                                             k_BITS_PER_UINT64;
    BSLS_ASSERT_SAFE((!oldPos && oldLength) ||
                          !(d_array.back() & bdlb::BitMaskUtil::ge64(oldPos)));

    if (oldLength < newLength) {
        if (value && (oldPos || !oldLength)) {
            const int numBits = static_cast<int>(
                                 bsl::min<size_t>(newLength - oldLength,
                                                  k_BITS_PER_UINT64 - oldPos));
            d_array.back() |= bdlb::BitMaskUtil::one64(oldPos, numBits);
        }
    }
    else if (oldLength == newLength) {
        return;                                                       // RETURN
    }

    // The following 'resize' might be a no-op.  Note that the 'resize' might
    // throw, which could cause this object to be destroyed and the assert in
    // the d'tor verifying consistency between 'd_array.size()' and 'd_length'
    // has to pass, so update 'd_length' immediately after the 'resize' and not
    // before.

    d_array.resize(arraySize(newLength), value ? s_minusOne : 0);
    d_length = newLength;

    // Trim possible residual set bits at the end.

    const int newPos = static_cast<unsigned>(newLength) % k_BITS_PER_UINT64;
    if ((newPos || !newLength) && (value || newLength < oldLength)) {
        // It is possible for the code to reach this point under circumstances
        // where this mask is not necessary, but it will never be harmful.

        d_array.back() &= rawLt64(newPos);
    }
}

// ACCESSORS

                                // Aspects

bsl::ostream& BitArray::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    return stream ? bdlb::BitStringUtil::print(stream,
                                               data(),
                                               d_length,
                                               level,
                                               spacesPerLevel)
                  : stream;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
