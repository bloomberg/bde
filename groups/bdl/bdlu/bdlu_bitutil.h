// bdlu_bitutil.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLU_BITUTIL
#define INCLUDED_BDLU_BITUTIL

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

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
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
    // types 'unsigned int' and 'bsls::Type::Uint64', respectively.
    //
    // This utility 'struct' also defines an enum with values
    // 'e_BYTES_PER_INT', 'e_BYTES_PER_INT64', 'e_BITS_PER_INT', and
    // 'e_BITS_PER_INT64'.  'e_BYTES_PER_INT' is equal to the number of bytes
    // used to represent an 'int' on the platform, 'e_BYTES_PER_INT64' is equal
    // to the number of bytes used to represent a 'bsls::Types::Int64' on the
    // platform, 'e_BITS_PER_INT' is equal to the number of bits used to
    // represent an 'int' on the platform, and 'e_BITS_PER_INT64' is equal to
    // the number of bits used to represent a 'bsls::Types::Int64' on the
    // platform.

    // CONSTANTS
    enum {
        e_BYTES_PER_INT = sizeof(int),
        e_BYTES_PER_INT64 = sizeof(bsls::Types::Int64),
        e_BITS_PER_INT  = CHAR_BIT * e_BYTES_PER_INT,
        e_BITS_PER_INT64  = CHAR_BIT * e_BYTES_PER_INT,
    };

  private:
    // PRIVATE CLASS METHODS
    static int privateFindSetBitAtLargestIndex(unsigned int value);
    static int privateFindSetBitAtLargestIndex(bsls::Types::Uint64 value);
        // Return the index (least-significant bit is at 0) of the
        // most-significant 1 bit in the specified 'value', if such a bit
        // exists, and -1 otherwise.

    static int privateFindSetBitAtSmallestIndex(unsigned int value);
    static int privateFindSetBitAtSmallestIndex(bsls::Types::Uint64 value);
        // Return the index (least-significant bit is at 0) of the
        // least-significant 1 bit in the specified 'value', if such a bit
        // exists, and 'sizeInBits(value)' otherwise.

  public:
    // CLASS METHODS
    static int log2(unsigned int value);
    static int log2(bsls::Types::Uint64 value);
        // Return the base-2 logarithm of the specified 'value' rounded up to
        // the nearest integer.  The behavior is undefined unless 'value > 0'.

    static unsigned int clearBit(unsigned int value, int index);
    static bsls::Types::Uint64 clearBit(bsls::Types::Uint64 value, int index);
        // Return the result of replacing the bit at the specified 'index'
        // position in the specified 'value' with 0, transferring all other
        // bits from 'value' unchanged.  The behavior is undefined unless
        // '0 <= index < sizeInBits(value)'.

    static int findSetBitAtLargestIndex(unsigned int value);
    static int findSetBitAtLargestIndex(bsls::Types::Uint64 value);
        // Return the index (least-significant bit is at 0) of the
        // most-significant 1 bit in the specified 'value', if such a bit
        // exists, and -1 otherwise.

    static int findSetBitAtSmallestIndex(unsigned int value);
    static int findSetBitAtSmallestIndex(bsls::Types::Uint64 value);
        // Return the index (least-significant bit is at 0) of the
        // least-significant 1 bit in the specified 'value', if such a bit
        // exists, and 'sizeInBits(value)' otherwise.

    static bool isSetBit(unsigned int value, int index);
    static bool isSetBit(bsls::Types::Uint64 value, int index);
        // Return 'true' if the bit in the specified 'value' at the specified
        // 'index' is set to 1, and 'false' otherwise.  The behavior is
        // undefined unless '0 <= index < sizeInBits(value)'.

    static int numSetBit(unsigned int value);
    static int numSetBit(bsls::Types::Uint64 value);
        // Return the number of 1 bits in the specified 'value'.

    static unsigned int roundUpToBinaryPower(unsigned int value);
    static bsls::Types::Uint64 roundUpToBinaryPower64(
                                                    bsls::Types::Uint64 value);
        // Return the least power of 2 that is greater than or equal to the
        // specified 'value', and 0 if the conversion was not successful.  Note
        // that the conversion will not be successful if and only if
        // '0 == value || value > (1 << (sizeInBits(value) - 1))'.

    static unsigned int setBit(unsigned int value, int index);
    static bsls::Types::Uint64 setBit(bsls::Types::Uint64 value, int index);
        // Return the result of replacing the bit at the specified 'index'
        // position in the specified 'value' with 1, transferring all other
        // bits from 'value' unchanged.  The behavior is undefined unless
        // '0 <= index < sizeInBits(value)'.

    template <typename INTEGER>
    static int sizeInBits(INTEGER value);
        // Return the number of bits in the specified 'value' of the (template
        // parameter) type 'INTEGER'.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                               // ----------------
                               // struct BitUtil
                               // ----------------

// CLASS METHODS
inline
int BitUtil::log2(unsigned value)
{
    BSLS_ASSERT_SAFE(value > 0);

    return findSetBitAtLargestIndex(value - 1) + 1;
}

inline
int BitUtil::log2(bsls::Types::Uint64 value)
{
    BSLS_ASSERT_SAFE(value > 0LL);

    return findSetBitAtLargestIndex(value - 1LL) + 1;
}

inline
unsigned int BitUtil::clearBit(unsigned int value, int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  e_BITS_PER_INT);

    return value & ~(1 << index);
}

inline
bsls::Types::Uint64 BitUtil::clearBit(bsls::Types::Uint64 value, int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  e_BITS_PER_INT64);

    return value & ~(1LL << index);
}

inline
int BitUtil::findSetBitAtLargestIndex(unsigned int value)
{
#if defined(BSLS_PLATFORM_CMP_IBM)
    return 31 - __cntlz4(value);
#elif defined(BSLS_PLATFORM_CMP_GNU)
    return (31 - __builtin_clz(value)) | -static_cast<int>(!value);
#else
    return privateFindSetBitAtLargestIndex(value);
#endif
}

inline
int BitUtil::findSetBitAtLargestIndex(bsls::Types::Uint64 value)
{
#if defined(BSLS_PLATFORM_CMP_IBM)
    return 63 - __cntlz8(value);
#elif defined(BSLS_PLATFORM_CMP_GNU)
    return (63 - __builtin_clzll(value)) |
                                      -static_cast<bsls::Types::Int64>(!value);
#else
    return privateFindSetBitAtLargestIndex(value);
#endif
}

inline
int BitUtil::findSetBitAtSmallestIndex(unsigned int value)
{
#if defined(BSLS_PLATFORM_CMP_IBM)
    return __cnttz4(value);
#elif defined(BSLS_PLATFORM_CMP_GNU)
    enum {
        e_INT_MASK = e_BITS_PER_INT - 1
    };
    const unsigned int a = __builtin_ffs(value) - 1;
    return (a & e_INT_MASK) + (a >> e_INT_MASK);

    // Other possibility:
    //..
    //  return (__builtin_ffs(value) - 1)
    //       ^ ((-!value) & ~e_BITS_PER_INT);
    //..
#else
    return privateFindSetBitAtSmallestIndex(value);
#endif
}

inline
int BitUtil::findSetBitAtSmallestIndex(bsls::Types::Uint64 value)
{
#if defined(BSLS_PLATFORM_CMP_IBM)
    return __cnttz8(value);
#elif defined(BSLS_PLATFORM_CMP_GNU)
    enum {
        e_INT64_MASK = e_BITS_PER_INT64 - 1,
        e_INT_MASK = e_BITS_PER_INT - 1
    };
    const unsigned int a = __builtin_ffsll(value) - 1;
    return (a & e_INT64_MASK) + (a >> e_INT_MASK);

    // Other possibility:
    //..
    //  return (__builtin_ffsll(value) - 1)
    //       ^ ((-!value) & ~e_BITS_PER_INT64);
    //..
#else
    return privateFindSetBitAtSmallestIndex(value);
#endif
}

inline
bool BitUtil::isSetBit(unsigned int value, int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  e_BITS_PER_INT);

    return (1 << index) & value;
}

inline
bool BitUtil::isSetBit(bsls::Types::Uint64 value, int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  e_BITS_PER_INT64);

    return (1LL << index) & value;
}

inline
int BitUtil::numSetBit(unsigned int value)
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
int BitUtil::numSetBit(bsls::Types::Uint64 value)
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
        const Int64 mask = 0x3333333333333333LL;
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
unsigned int BitUtil::roundUpToBinaryPower(unsigned int value)
{
    int      index = findSetBitAtLargestIndex(value);
    unsigned ret   = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(0 <= index)
                     ? (1 << index)
                     : 0;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(ret == value)) {
        return ret;                                                   // RETURN
    }
    return ret << 1;
}

inline
bsls::Types::Uint64 BitUtil::roundUpToBinaryPower64(
                                                bsls::Types::Uint64 value)
{
    int                 index = findSetBitAtLargestIndex(value);
    bsls::Types::Uint64 ret   = BSLS_PERFORMANCEHINT_PREDICT_LIKELY(0 <= index)
                                ? (1LL << index)
                                : 0;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(ret == value)) {
        return ret;                                                   // RETURN
    }
    return ret << 1;
}

inline
unsigned int BitUtil::setBit(unsigned int value, int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  e_BITS_PER_INT);

    return value | (1 << index);
}

inline
bsls::Types::Uint64 BitUtil::setBit(bsls::Types::Uint64 value, int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  e_BITS_PER_INT64);

    return value | (1LL << index);
}

template <typename TYPE>
inline
int BitUtil::sizeInBits(TYPE)
{
    return CHAR_BIT * sizeof(TYPE);
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
