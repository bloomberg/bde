// bdlb_bitutil.cpp                                                   -*-C++-*-
#include <bdlb_bitutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_bitutil_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>

namespace BloombergLP {
namespace bdlb {

BSLMF_ASSERT(4 == sizeof(BitUtil::uint32_t));
BSLMF_ASSERT(8 == sizeof(BitUtil::uint64_t));

                        // --------------
                        // struct BitUtil
                        // --------------

// PRIVATE CLASS METHODS

int BitUtil::privateNumBitsSet(uint32_t value)
{
    // First we use a tricky way of getting every 2-bit half-nibble to
    // represent the number of bits that were set in those two bits.

    value -= (value >> 1) & 0x55555555;

    // Henceforth, we just accumulate the sum down into lower and lower bits.

    {
        const int mask = 0x33333333;
        value = ((value >> 2) & mask) + (value & mask);
    }

    // Any 4-bit nibble is now guaranteed to be '<= 4', so we don't have to
    // mask both sides of the addition.  We must mask after the addition so
    // 8-bit bytes are the sum of bits in those 8 bits.

    value = ((value >> 4) + value) & 0x0f0f0f0f;

    // It is no longer necessary to mask the additions, because it is
    // impossible for any bit groups to add up to more than 256 and carry, thus
    // interfering with adjacent groups.  Each 8-bit byte is independent from
    // now on.

    value = (value >>  8) + value;
    value = (value >> 16) + value;

    return value & 0x000000ff;
}

int BitUtil::privateNumBitsSet(uint64_t value)
{
    // First we use a tricky way of getting every 2-bit half-nibble to
    // represent the number of bits that were set in those two bits.

    value -= (value >> 1) & 0x5555555555555555LL;

    // Henceforth, we just accumulate the sum down into lower and lower bits.

    {
        const uint64_t mask = 0x3333333333333333LL;
        value = ((value >> 2) & mask) + (value & mask);
    }

    // Any 4-bit nibble is now guaranteed to be '<= 4', so we don't have to
    // mask both sides of the addition.  We must mask after the addition so
    // 8-bit bytes are the sum of bits in those 8 bits.

    value = ((value >> 4) + value) & 0x0f0f0f0f0f0f0f0fLL;

    // It is no longer necessary to mask the additions, because it is
    // impossible for any bit groups to add up to more than 256 and carry, thus
    // interfering with adjacent groups.  Each 8-bit byte is independent from
    // now on.

    value = (value >>  8) + value;
    value = (value >> 16) + value;
    value = (value >> 32) + value;

    return static_cast<int>(value & 0xff);
}

int BitUtil::privateNumLeadingUnsetBits(uint32_t value)
{
    // Note that it doesn't matter whether the right shifts sign extend or not.

    value |= value >> 16;
    value |= value >>  8;
    value |= value >>  4;
    value |= value >>  2;
    value |= value >>  1;
    return 32 - numBitsSet(value);
}

int BitUtil::privateNumLeadingUnsetBits(uint64_t value)
{
    // Note that it doesn't matter whether the right shifts sign extend or not.

    value |= value >> 32;
    value |= value >> 16;
    value |= value >>  8;
    value |= value >>  4;
    value |= value >>  2;
    value |= value >>  1;
    return 64 - numBitsSet(value);
}

int BitUtil::privateNumTrailingUnsetBits(uint32_t value)
{
    value |= value << 16;
    value |= value <<  8;
    value |= value <<  4;
    value |= value <<  2;
    value |= value <<  1;
    return numBitsSet(~value);
}

int BitUtil::privateNumTrailingUnsetBits(uint64_t value)
{
    value |= value << 32;
    value |= value << 16;
    value |= value <<  8;
    value |= value <<  4;
    value |= value <<  2;
    value |= value <<  1;
    return numBitsSet(~value);
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
