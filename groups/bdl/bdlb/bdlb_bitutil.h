// bdlb_bitutil.h                                                     -*-C++-*-
#ifndef INCLUDED_BDLB_BITUTIL
#define INCLUDED_BDLB_BITUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide efficient bit-manipulation of 'uint32_t'/'uint64_t' values.
//
//@CLASSES:
//  bdlb::BitUtil: namespace for 'uint32_t' and 'uint64_t' bit-level operations
//
//@DESCRIPTION: This component provides a utility 'struct', 'bdlb::BitUtil',
// that serves as a namespace for a collection of efficient, bit-level
// procedures on 'uint32_t' and 'uint64_t'.  In particular, 'BitUtil' supplies
// single bit manipulation, bit counting, and mathematical functions that can
// be optimized with bitwise operations.
//
// Some of the methods provided in 'BitUtil' have other common names.  Below is
// a list of mappings from the name used in 'BitUtil' to these related function
// names:
//
//: * numLeadingUnsetBits: cntlz, clz, ffs, ffo, nlz, ctlz
//:
//: * numTrailingUnsetBits: cnttz, ctz, ntz, cttz
//:
//: * numBitsSet: popcnt, popcount
//
///Usage
///-----
// The following usage examples illustrate how some of the methods provided by
// this component are used.  Note that, in all of these examples, the low-order
// bit is considered bit 0 and resides on the right edge of the bit string.
//
// First, we use 'withBitSet' to demonstrate the ordering of bits:
//..
//  assert(static_cast<uint32_t>(0x00000001)
//                 == bdlb::BitUtil::withBitSet(static_cast<uint32_t>(0),  0));
//  assert(static_cast<uint32_t>(0x00000008)
//                 == bdlb::BitUtil::withBitSet(static_cast<uint32_t>(0),  3));
//  assert(static_cast<uint32_t>(0x00800000)
//                 == bdlb::BitUtil::withBitSet(static_cast<uint32_t>(0), 23));
//  assert(static_cast<uint32_t>(0x66676666)
//        == bdlb::BitUtil::withBitSet(static_cast<uint32_t>(0x66666666), 16));
//
//  /*------------------------------------------------------------------------+
//  | 'bdlb::BitUtil::withBitSet(0x66666666, 16)' in binary:                  |
//  |                                                                         |
//  | input in binary:                       01100110011001100110011001100110 |
//  | set bit 16:                                           1                 |
//  | result:                                01100110011001110110011001100110 |
//  +------------------------------------------------------------------------*/
//..
// Then, we count the number of set bits in a value with 'numBitsSet':
//..
//  assert(0 == bdlb::BitUtil::numBitsSet(static_cast<uint32_t>(0x00000000)));
//  assert(2 == bdlb::BitUtil::numBitsSet(static_cast<uint32_t>(0x00101000)));
//  assert(8 == bdlb::BitUtil::numBitsSet(static_cast<uint32_t>(0x30071101)));
//
//  /*------------------------------------------------------------------------+
//  | 'bdlb::BitUtil::numBitsSet(0x30071101)' in binary:                      |
//  |                                                                         |
//  | input in binary:                       00110000000001110001000100000001 |
//  | that has 8 bits set.  result: 8                                         |
//  +------------------------------------------------------------------------*/
//..
// Finally, we use 'numLeadingUnsetBits' to determine the number of unset bits
// with a higher index than the first set bit:
//..
//  assert(32 ==
//      bdlb::BitUtil::numLeadingUnsetBits(static_cast<uint32_t>(0x00000000)));
//  assert(31 ==
//      bdlb::BitUtil::numLeadingUnsetBits(static_cast<uint32_t>(0x00000001)));
//  assert(7 ==
//      bdlb::BitUtil::numLeadingUnsetBits(static_cast<uint32_t>(0x01000000)));
//  assert(7 ==
//      bdlb::BitUtil::numLeadingUnsetBits(static_cast<uint32_t>(0x01620030)));
//
//  /*------------------------------------------------------------------------+
//  | 'bdlb::BitUtil::numLeadingUnsetBits(0x01620030)' in binary:             |
//  |                                                                         |
//  | input in binary:                       00000001011000100000000000110000 |
//  | highest set bit:                              1                         |
//  | number of unset bits leading this set bit == 7                          |
//  +------------------------------------------------------------------------*/
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
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

#ifndef INCLUDED_BSL_CSTDINT
#include <bsl_cstdint.h>
#endif

#ifdef BSLS_PLATFORM_CMP_IBM
#ifndef INCLUDED_BUILTINS
#include <builtins.h>
#define INCLUDED_BUILTINS
#endif
#endif

namespace BloombergLP {
namespace bdlb {

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
        k_BITS_PER_INT32 = 32,  // bits used to represent an 'int32_t'
        k_BITS_PER_INT64 = 64   // bits used to represent an 'int64_t'
    };

  public:
    // PUBLIC TYPE ALIASES (to support old toolchains)
    typedef bsl::uint32_t uint32_t;
    typedef bsl::uint64_t uint64_t;

  private:
    // PRIVATE CLASS METHODS
    static int privateNumBitsSet(uint32_t value);
    static int privateNumBitsSet(uint64_t value);
        // Return the number of 1 bits in the specified 'value'.

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
        // the nearest integer.  The behavior is undefined unless '0 < value'.

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

    static uint32_t roundUp(uint32_t value, uint32_t boundary);
    static uint64_t roundUp(uint64_t value, uint64_t boundary);
        // Return the least multiple of the specified 'boundary' that is
        // greater than or equal to the specified 'value', and 0 if
        // '0 == value' or the conversion was not successful.  The behavior is
        // undefined unless '1 == numBitsSet(boundary)'.  Note that the
        // conversion will succeed if and only if '0 == value % boundary' or
        // '(1 << sizeInBits(value)) > (value / boundary + 1) * boundary'.

    static uint32_t roundUpToBinaryPower(uint32_t value);
    static uint64_t roundUpToBinaryPower(uint64_t value);
        // Return the least power of 2 that is greater than or equal to the
        // specified 'value', and 0 if the conversion was not successful.  Note
        // that the conversion will succeed if and only if
        // '0 < value <= (1 << (sizeInBits(value) - 1))'

    template <class INTEGER>
    static int sizeInBits(INTEGER value);
        // Return the number of bits in the specified 'value' of the (template
        // parameter) type 'INTEGER'.

    static uint32_t withBitCleared(uint32_t value, int index);
    static uint64_t withBitCleared(uint64_t value, int index);
        // Return the result of replacing the bit at the specified 'index' in
        // the specified 'value' with 0, transferring all other bits from
        // 'value' unchanged.  The behavior is undefined unless
        // '0 <= index < sizeInBits(value)'.

    static uint32_t withBitSet(uint32_t value, int index);
    static uint64_t withBitSet(uint64_t value, int index);
        // Return the result of replacing the bit at the specified 'index' in
        // the specified 'value' with 1, transferring all other bits from
        // 'value' unchanged.  The behavior is undefined unless
        // '0 <= index < sizeInBits(value)'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                               // --------------
                               // struct BitUtil
                               // --------------

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

    return (static_cast<uint64_t>(1) << index) & value;
}

inline
int BitUtil::log2(uint32_t value)
{
    BSLS_ASSERT_SAFE(0 < value);

    return k_BITS_PER_INT32 - numLeadingUnsetBits(value - 1);
}

inline
int BitUtil::log2(uint64_t value)
{
    BSLS_ASSERT_SAFE(0ULL < value);

    return k_BITS_PER_INT64 - numLeadingUnsetBits(value - 1);
}

inline
int BitUtil::numBitsSet(uint32_t value)
{
#if defined(BSLS_PLATFORM_CMP_IBM)
    return __popcnt4(value);
#elif defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    return __builtin_popcount(value);
#else
    return privateNumBitsSet(value);
#endif
}

inline
int BitUtil::numBitsSet(uint64_t value)
{
#if defined(BSLS_PLATFORM_CMP_IBM)
    return __popcnt8(value);
#elif defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    return __builtin_popcountll(value);
#else
    return privateNumBitsSet(value);
#endif
}

inline
int BitUtil::numLeadingUnsetBits(uint32_t value)
{
#if defined(BSLS_PLATFORM_CMP_IBM)
    return __cntlz4(value);
#elif defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    // '__builtin_clz(0)' is undefined
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
#elif defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    // '__builtin_clzll(0)' is undefined
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
#elif defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    enum {
        k_INT32_MASK = k_BITS_PER_INT32 - 1
    };
    const uint32_t a = __builtin_ffs(value) - 1;
    return (a & k_INT32_MASK) + (a >> k_INT32_MASK);

    // Other possibility:
    //..
    //  return (__builtin_ffs(value) - 1) ^ ((-!value) & ~k_BITS_PER_INT32);
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
#elif defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    enum {
        k_INT64_MASK = k_BITS_PER_INT64 - 1,
        k_INT32_MASK = k_BITS_PER_INT32 - 1
    };
    const uint32_t a = __builtin_ffsll(value) - 1;
    return (a & k_INT64_MASK) + (a >> k_INT32_MASK);

    // Other possibility:
    //..
    //  return (__builtin_ffsll(value) - 1) ^ ((-!value) & ~k_BITS_PER_INT64);
    //..
#else
    return privateNumTrailingUnsetBits(value);
#endif
}

inline
BitUtil::uint32_t BitUtil::roundUp(uint32_t value, uint32_t boundary)
{
    BSLS_ASSERT_SAFE(1 == numBitsSet(boundary));

    return ((value - 1) | (boundary - 1)) + 1;
}

inline
BitUtil::uint64_t BitUtil::roundUp(uint64_t value, uint64_t boundary)
{
    BSLS_ASSERT_SAFE(1 == numBitsSet(boundary));

    return ((value - 1) | (boundary - 1)) + 1;
}

inline
BitUtil::uint32_t BitUtil::roundUpToBinaryPower(uint32_t value)
{
    const int index = numLeadingUnsetBits(value - 1);
    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(0 < index)
           ? static_cast<uint32_t>(1) << (k_BITS_PER_INT32 - index)
           : 0;
}

inline
BitUtil::uint64_t BitUtil::roundUpToBinaryPower(uint64_t value)
{
    const int index = numLeadingUnsetBits(value - 1);
    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(0 < index)
           ? static_cast<uint64_t>(1) << (k_BITS_PER_INT64 - index)
           : 0;
}

template <class TYPE>
inline
int BitUtil::sizeInBits(TYPE)
{
    return static_cast<int>(CHAR_BIT * sizeof(TYPE));
}

inline
BitUtil::uint32_t BitUtil::withBitCleared(uint32_t value, int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  k_BITS_PER_INT32);

    return value & ~(1 << index);
}

inline
BitUtil::uint64_t BitUtil::withBitCleared(uint64_t value, int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  k_BITS_PER_INT64);

    return value & ~(static_cast<uint64_t>(1) << index);
}

inline
BitUtil::uint32_t BitUtil::withBitSet(uint32_t value, int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  k_BITS_PER_INT32);

    return value | (1 << index);
}

inline
BitUtil::uint64_t BitUtil::withBitSet(uint64_t value, int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  k_BITS_PER_INT64);

    return value | (static_cast<uint64_t>(1) << index);
}

}  // close package namespace
}  // close enterprise namespace

#if defined(BDLB_BITUTIL_NO_STDINT)
# undef BDLB_BITUTIL_NO_STDINT
#endif

#endif

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
