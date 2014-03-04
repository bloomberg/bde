// bdlu_bitutil.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLU_BITUTIL
#define INCLUDED_BDLU_BITUTIL

#ifndef INCLUDED_STDINT
#include <stdint.h>
#define INCLUDED_STDINT
#endif

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>  // required by 'bdlu'
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

#ifdef BSLS_PLATFORM_CMP_IBM
#ifndef INCLUDED_BUILTINS
#include <builtins.h>
#define INCLUDED_BUILTINS
#endif
#endif

namespace BloombergLP {
namespace bdlu {

                               // ==============
                               // struct BitUtil
                               // ==============

struct BitUtil {
    // This utility 'struct' provides a namespace for a set of bit-level,
    // stateless functions that operate on the built-in 32- and 64-bit integer
    // types 'uint32_t' and 'uint64_t', respectively.

  private:
    // PRIVATE CONSTANTS
    enum {
        k_BITS_PER_INT32  = 32,  // bits used to represent an 'int32_t'
        k_BITS_PER_INT64  = 64,  // bits used to represent an 'int64_t'
    };

    // PRIVATE CLASS METHODS
    static int privateNumLeadingUnsetBits(uint32_t value);
    static int privateNumLeadingUnsetBits(uint64_t value);
        // Return the number of consecutive 0 bits starting from the
        // most-significant bit in the specified 'value'.

    static int privateNumTrailingUnsetBits(uint32_t value);
    static int privateNumTrailingUnsetBits(uint64_t value);
        // Return the number of consecutive 0 bits starting from the
        // least-significant bit in the specified 'value'.

  public:
    // CLASS METHODS
    static bool isBitSet(uint32_t value, int index);
    static bool isBitSet(uint64_t value, int index);
        // Return 'true' if the bit in the specified 'value' at the specified
        // 'index' is set to 1, and 'false' otherwise.  The behavior is
        // undefined unless '0 <= index < sizeInBits(value)'.

    static int log2(uint32_t value);
    static int log2(uint64_t value);
        // Return the base-2 logarithm of the specified 'value' rounded up to
        // the nearest integer.  The behavior is undefined unless 'value > 0'.

    static int numBitsSet(uint32_t value);
    static int numBitsSet(uint64_t value);
        // Return the number of 1 bits in the specified 'value'.

    static int numLeadingUnsetBits(uint32_t value);
    static int numLeadingUnsetBits(uint64_t value);
        // Return the number of consecutive 0 bits starting from the
        // most-significant bit in the specified 'value'.

    static int numTrailingUnsetBits(uint32_t value);
    static int numTrailingUnsetBits(uint64_t value);
        // Return the number of consecutive 0 bits starting from the
        // least-significant bit in the specified 'value'.

    static uint32_t roundUpToBinaryPower(uint32_t value);
    static uint64_t roundUpToBinaryPower(uint64_t value);
        // Return the least power of 2 that is greater than or equal to the
        // specified 'value', and 0 if the conversion was not successful.  Note
        // that the conversion will not be successful if and only if
        // '0 == value || value > (1 << (sizeInBits(value) - 1))'.

    template <typename INTEGER>
    static int sizeInBits(INTEGER value);
        // Return the number of bits in the specified 'value' of the (template
        // parameter) type 'INTEGER'.

    static uint32_t withBitCleared(uint32_t value, int index);
    static uint64_t withBitCleared(uint64_t value, int index);
        // Return the result of replacing the bit at the specified 'index'
        // position in the specified 'value' with 0, transferring all other
        // bits from 'value' unchanged.  The behavior is undefined unless
        // '0 <= index < sizeInBits(value)'.

    static uint32_t withBitSet(uint32_t value, int index);
    static uint64_t withBitSet(uint64_t value, int index);
        // Return the result of replacing the bit at the specified 'index'
        // position in the specified 'value' with 1, transferring all other
        // bits from 'value' unchanged.  The behavior is undefined unless
        // '0 <= index < sizeInBits(value)'.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                               // ----------------
                               // struct BitUtil
                               // ----------------

// CLASS METHODS
inline
bool BitUtil::isBitSet(uint32_t value, int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  k_BITS_PER_INT32);

    return (1 << index) & value;
}

inline
bool BitUtil::isBitSet(uint64_t value, int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  k_BITS_PER_INT64);

    return ((uint64_t)1 << index) & value;
}

inline
int BitUtil::log2(unsigned value)
{
    BSLS_ASSERT_SAFE(value > 0);

    return 32 - numLeadingUnsetBits(value - 1);
}

inline
int BitUtil::log2(uint64_t value)
{
    BSLS_ASSERT_SAFE(value > 0LL);

    return 64 - numLeadingUnsetBits(value - 1);
}

inline
int BitUtil::numBitsSet(uint32_t value)
{
#if defined(BSLS_PLATFORM_CMP_IBM)
    return __popcnt4(value);
#elif defined(BSLS_PLATFORM_CMP_GNU)
    return __builtin_popcount(value);
#else

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
#endif
}

inline
int BitUtil::numBitsSet(uint64_t value)
{
#if defined(BSLS_PLATFORM_CMP_IBM)
    return __popcnt8(value);
#elif defined(BSLS_PLATFORM_CMP_GNU)
    return __builtin_popcountll(value);
#else

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
#endif
}

inline
int BitUtil::numLeadingUnsetBits(uint32_t value)
{
#if defined(BSLS_PLATFORM_CMP_IBM)
    return __cntlz4(value);
#elif defined(BSLS_PLATFORM_CMP_GNU)
    // __builtin_clz(0) is undefined
    return __builtin_clz(value | 1) + static_cast<int>(!value);
#else
    return privateNumLeadingUnsetBits(value);
#endif
}

inline
int BitUtil::numLeadingUnsetBits(uint64_t value)
{
#if defined(BSLS_PLATFORM_CMP_IBM)
    return __cntlz8(value);
#elif defined(BSLS_PLATFORM_CMP_GNU)
    // __builtin_clzll(0) is undefined
    return __builtin_clzll(value | 1) + static_cast<int>(!value);
#else
    return privateNumLeadingUnsetBits(value);
#endif
}

inline
int BitUtil::numTrailingUnsetBits(uint32_t value)
{
#if defined(BSLS_PLATFORM_CMP_IBM)
    return __cnttz4(value);
#elif defined(BSLS_PLATFORM_CMP_GNU)
    enum {
        k_INT32_MASK = k_BITS_PER_INT32 - 1
    };
    const uint32_t a = __builtin_ffs(value) - 1;
    return (a & k_INT32_MASK) + (a >> k_INT32_MASK);

    // Other possibility:
    //..
    //  return (__builtin_ffs(value) - 1)
    //       ^ ((-!value) & ~k_BITS_PER_INT32);
    //..
#else
    return privateNumTrailingUnsetBits(value);
#endif
}

inline
int BitUtil::numTrailingUnsetBits(uint64_t value)
{
#if defined(BSLS_PLATFORM_CMP_IBM)
    return __cnttz8(value);
#elif defined(BSLS_PLATFORM_CMP_GNU)
    enum {
        k_INT64_MASK = k_BITS_PER_INT64 - 1,
        k_INT32_MASK = k_BITS_PER_INT32 - 1
    };
    const uint32_t a = __builtin_ffsll(value) - 1;
    return (a & k_INT64_MASK) + (a >> k_INT32_MASK);

    // Other possibility:
    //..
    //  return (__builtin_ffsll(value) - 1)
    //       ^ ((-!value) & ~k_BITS_PER_INT64);
    //..
#else
    return privateNumTrailingUnsetBits(value);
#endif
}

inline
uint32_t BitUtil::roundUpToBinaryPower(uint32_t value)
{
    return (uint32_t)1 << (32 - numLeadingUnsetBits(value - 1));
}

inline
uint64_t BitUtil::roundUpToBinaryPower(uint64_t value)
{
    return (uint64_t)1 << (64 - numLeadingUnsetBits(value - 1));
}

template <typename TYPE>
inline
int BitUtil::sizeInBits(TYPE)
{
    return CHAR_BIT * sizeof(TYPE);
}

inline
uint32_t BitUtil::withBitCleared(uint32_t value, int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  k_BITS_PER_INT32);

    return value & ~(1 << index);
}

inline
uint64_t BitUtil::withBitCleared(uint64_t value, int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  k_BITS_PER_INT64);

    return value & ~((uint64_t)1 << index);
}

inline
uint32_t BitUtil::withBitSet(uint32_t value, int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  k_BITS_PER_INT32);

    return value | (1 << index);
}

inline
uint64_t BitUtil::withBitSet(uint64_t value, int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  k_BITS_PER_INT64);

    return value | ((uint64_t)1 << index);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
