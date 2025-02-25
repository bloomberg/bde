// bdlb_bitutil.h                                                     -*-C++-*-
#ifndef INCLUDED_BDLB_BITUTIL
#define INCLUDED_BDLB_BITUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide efficient bit-manipulation of `uint32_t`/`uint64_t` values.
//
//@CLASSES:
//  bdlb::BitUtil: namespace for `uint32_t` and `uint64_t` bit-level operations
//
//@DESCRIPTION: This component provides a utility `struct`, `bdlb::BitUtil`,
// that serves as a namespace for a collection of efficient, bit-level
// procedures on 32- and 64-bit unsigned integer types.  In particular,
// `BitUtil` supplies single bit manipulation, bit counting, and mathematical
// functions that can be optimized with bitwise operations.
//
// This component is meant to interoperate cleanly both with fundamental types,
// as well as common sized integer type aliases like `bsl::uin64_t` and
// `bslsl::Types::UInt64`.  An overload set consisting of the fundamental
// integer types `unsigned int`, `unsigned long`, and `unsigned long long` is
// used to minimize warnings and avoid ambiguity that may arise when dealing
// with explicitly sized types that may alias to different fundamental types on
// different platforms.
//
// Some of the methods provided in `BitUtil` have other common names.  Below is
// a list of mappings from the name used in `BitUtil` to these related function
// names:
//
// * numLeadingUnsetBits: cntlz, clz, ffs, ffo, nlz, ctlz
// * numTrailingUnsetBits: cnttz, ctz, ntz, cttz
// * numBitsSet: popcnt, popcount
//
///Usage
///-----
// The following usage examples illustrate how some of the methods provided by
// this component are used.  Note that, in all of these examples, the low-order
// bit is considered bit 0 and resides on the right edge of the bit string.
//
// First, we use `withBitSet` to demonstrate the ordering of bits:
// ```
// assert(static_cast<uint32_t>(0x00000001)
//                == bdlb::BitUtil::withBitSet(static_cast<uint32_t>(0),  0));
// assert(static_cast<uint32_t>(0x00000008)
//                == bdlb::BitUtil::withBitSet(static_cast<uint32_t>(0),  3));
// assert(static_cast<uint32_t>(0x00800000)
//                == bdlb::BitUtil::withBitSet(static_cast<uint32_t>(0), 23));
// assert(static_cast<uint32_t>(0x66676666)
//       == bdlb::BitUtil::withBitSet(static_cast<uint32_t>(0x66666666), 16));
//
// /*------------------------------------------------------------------------+
// | 'bdlb::BitUtil::withBitSet(0x66666666, 16)' in binary:                  |
// |                                                                         |
// | input in binary:                       01100110011001100110011001100110 |
// | set bit 16:                                           1                 |
// | result:                                01100110011001110110011001100110 |
// +------------------------------------------------------------------------*/
// ```
// Then, we count the number of set bits in a value with `numBitsSet`:
// ```
// assert(0 == bdlb::BitUtil::numBitsSet(static_cast<uint32_t>(0x00000000)));
// assert(2 == bdlb::BitUtil::numBitsSet(static_cast<uint32_t>(0x00101000)));
// assert(8 == bdlb::BitUtil::numBitsSet(static_cast<uint32_t>(0x30071101)));
//
// /*------------------------------------------------------------------------+
// | 'bdlb::BitUtil::numBitsSet(0x30071101)' in binary:                      |
// |                                                                         |
// | input in binary:                       00110000000001110001000100000001 |
// | that has 8 bits set.  result: 8                                         |
// +------------------------------------------------------------------------*/
// ```
// Finally, we use `numLeadingUnsetBits` to determine the number of unset bits
// with a higher index than the first set bit:
// ```
// assert(32 ==
//     bdlb::BitUtil::numLeadingUnsetBits(static_cast<uint32_t>(0x00000000)));
// assert(31 ==
//     bdlb::BitUtil::numLeadingUnsetBits(static_cast<uint32_t>(0x00000001)));
// assert(7 ==
//     bdlb::BitUtil::numLeadingUnsetBits(static_cast<uint32_t>(0x01000000)));
// assert(7 ==
//     bdlb::BitUtil::numLeadingUnsetBits(static_cast<uint32_t>(0x01620030)));
//
// /*------------------------------------------------------------------------+
// | 'bdlb::BitUtil::numLeadingUnsetBits(0x01620030)' in binary:             |
// |                                                                         |
// | input in binary:                       00000001011000100000000000110000 |
// | highest set bit:                              1                         |
// | number of unset bits leading this set bit == 7                          |
// +------------------------------------------------------------------------*/
// ```

#include <bdlscm_version.h>

#include <bslmf_conditional.h>

#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_review.h>

#include <bsl_climits.h>
#include <bsl_cstdint.h>

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
/// Use optimal intrinsics that know about CPU instruction sets on compilers
/// the recognize the Gnu intrinsic spellings.
# define BDLB_BITUTIL_USE_GNU_INTRINSICS 1
#endif

#ifdef BSLS_PLATFORM_CMP_MSVC
#include <intrin.h>
# define BDLB_BITUTIL_USE_MSVC_INTRINSICS 1
    // Use the intrinsics that map directly to CPU instructions on MSVC

# if defined(BSLS_PLATFORM_CPU_ARM)
#  define BDLB_BITUTIL_USE_MSVC_COUNT_ONE_BITS 1
    // Use _CountOneBits instead of __popcnt intrinsics on MSVC
# endif

#endif

namespace BloombergLP {
namespace bdlb {

                               // ==============
                               // struct BitUtil
                               // ==============

/// This utility `struct` provides a namespace for a set of bit-level,
/// stateless functions that operate on the built-in 32- and 64-bit unsigned
/// integer types.
struct BitUtil {

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
    // PRIVATE TYPES
    typedef bsl::conditional<sizeof(unsigned long) == sizeof(unsigned int),
                             unsigned int,
                             unsigned long long>::type ULongLikeType;

    // PRIVATE CLASS METHODS

    /// Convert the specified `value` from `unsigned long` to another
    /// unsigned type of the same size - `unsigned int` or
    /// `unsigned long long`.
    static ULongLikeType normalize(unsigned long value);

    /// Return the number of 1 bits in the specified `value`.
    static int privateNumBitsSet(unsigned int value);
    static int privateNumBitsSet(unsigned long value);
    static int privateNumBitsSet(unsigned long long value);

    /// Return the number of consecutive 0 bits starting from the
    /// most-significant bit in the specified `value`.
    static int privateNumLeadingUnsetBits(unsigned int value);
    static int privateNumLeadingUnsetBits(unsigned long value);
    static int privateNumLeadingUnsetBits(unsigned long long value);

    /// Return the number of consecutive 0 bits starting from the
    /// least-significant bit in the specified `value`.
    static int privateNumTrailingUnsetBits(unsigned int value);
    static int privateNumTrailingUnsetBits(unsigned long value);
    static int privateNumTrailingUnsetBits(unsigned long long value);

  public:
    // CLASS METHODS

    /// Return `true` if the bit in the specified `value` at the specified
    /// `index` is set to 1, and `false` otherwise.  The behavior is
    /// undefined unless `0 <= index < sizeInBits(value)`.
    static bool isBitSet(unsigned int value, int index);
    static bool isBitSet(unsigned long value, int index);
    static bool isBitSet(unsigned long long value, int index);

    /// Return the base-2 logarithm of the specified `value` rounded up to
    /// the nearest integer.  The behavior is undefined unless `0 < value`.
    static int log2(unsigned int value);
    static int log2(unsigned long value);
    static int log2(unsigned long long value);

    /// Return the number of 1 bits in the specified `value`.
    static int numBitsSet(unsigned int value);
    static int numBitsSet(unsigned long value);
    static int numBitsSet(unsigned long long value);

    /// Return the number of consecutive 0 bits starting from the
    /// most-significant bit in the specified `value`.
    static int numLeadingUnsetBits(unsigned int value);
    static int numLeadingUnsetBits(unsigned long value);
    static int numLeadingUnsetBits(unsigned long long value);

    /// Return the number of consecutive 0 bits starting from the
    /// least-significant bit in the specified `value`.
    static int numTrailingUnsetBits(unsigned int value);
    static int numTrailingUnsetBits(unsigned long value);
    static int numTrailingUnsetBits(unsigned long long value);

    /// Return the least multiple of the specified `boundary` that is
    /// greater than or equal to the specified `value`, and 0 if
    /// `0 == value` or the conversion was not successful.  The behavior is
    /// undefined unless `1 == numBitsSet(boundary)`.  Note that the
    /// conversion will succeed if and only if `0 == value % boundary` or
    /// `(1 << sizeInBits(value)) > (value / boundary + 1) * boundary`.
    static unsigned int roundUp(unsigned int value, unsigned int boundary);
    static unsigned long roundUp(unsigned long value, unsigned long boundary);
    static unsigned long long roundUp(unsigned long long value,
                                      unsigned long long boundary);

    /// Return the least power of 2 that is greater than or equal to the
    /// specified `value`, and 0 if the conversion was not successful.  Note
    /// that the conversion will succeed if and only if
    /// `0 < value <= (1 << (sizeInBits(value) - 1))`
    static unsigned int roundUpToBinaryPower(unsigned int value);
    static unsigned long roundUpToBinaryPower(unsigned long value);
    static unsigned long long roundUpToBinaryPower(unsigned long long value);

    /// Return the number of bits in the specified `value` of the (template
    /// parameter) type `INTEGER`.
    template <class INTEGER>
    static int sizeInBits(INTEGER value = 0);

    /// Return the result of replacing the bit at the specified `index` in
    /// the specified `value` with 0, transferring all other bits from
    /// `value` unchanged.  The behavior is undefined unless
    /// `0 <= index < sizeInBits(value)`.
    static unsigned int withBitCleared(unsigned int value, int index);
    static unsigned long withBitCleared(unsigned long value, int index);
    static unsigned long long withBitCleared(unsigned long long value,
                                             int                index);

    /// Return the result of replacing the bit at the specified `index` in
    /// the specified `value` with 1, transferring all other bits from
    /// `value` unchanged.  The behavior is undefined unless
    /// `0 <= index < sizeInBits(value)`.
    static unsigned int withBitSet(unsigned int value, int index);
    static unsigned long withBitSet(unsigned long value, int index);
    static unsigned long long withBitSet(unsigned long long value, int index);
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                               // --------------
                               // struct BitUtil
                               // --------------

// CLASS METHODS
inline
BitUtil::ULongLikeType BitUtil::normalize(unsigned long value)
{
    return static_cast<ULongLikeType>(value);
}

inline
bool BitUtil::isBitSet(unsigned int value, int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  k_BITS_PER_INT32);

    return ((1 << index) & value) != 0;
}

inline
bool BitUtil::isBitSet(unsigned long long value, int index)
{
    BSLS_ASSERT_SAFE(    0 <= index);
    BSLS_ASSERT_SAFE(index <  k_BITS_PER_INT64);

    return ((1ULL << index) & value) != 0;
}

inline
bool BitUtil::isBitSet(unsigned long value, int index)
{
    return isBitSet(normalize(value), index);
}

inline
int BitUtil::log2(unsigned int value)
{
    BSLS_ASSERT(0 < value);

    return k_BITS_PER_INT32 - numLeadingUnsetBits(value - 1);
}

inline
int BitUtil::log2(unsigned long long value)
{
    BSLS_ASSERT(0ULL < value);

    return k_BITS_PER_INT64 - numLeadingUnsetBits(value - 1);
}

inline
int BitUtil::log2(unsigned long value)
{
    return log2(normalize(value));
}

inline
int BitUtil::numBitsSet(unsigned int value)
{
#if defined(BDLB_BITUTIL_USE_GNU_INTRINSICS)
    return __builtin_popcount(value);
#elif defined(BDLB_BITUTIL_USE_MSVC_INTRINSICS)
# if !defined(BDLB_BITUTIL_USE_MSVC_COUNT_ONE_BITS)
    return __popcnt(value);
# else
    return _CountOneBits(value);
# endif
#else
    return privateNumBitsSet(value);
#endif
}

inline
int BitUtil::numBitsSet(unsigned long long value)
{
#if defined(BDLB_BITUTIL_USE_GNU_INTRINSICS)
    return __builtin_popcountll(value);
#elif defined(BDLB_BITUTIL_USE_MSVC_INTRINSICS)
    #if !defined(BDLB_BITUTIL_USE_MSVC_COUNT_ONE_BITS)
        #if defined(BSLS_PLATFORM_CPU_64_BIT)
            return static_cast<int>(__popcnt64(value));
        #else
            // '__popcnt64' available only in 64bit target
            return __popcnt(static_cast<unsigned int>(value)) +
                   __popcnt(static_cast<unsigned int>(value >>
                                                      k_BITS_PER_INT32));
        #endif
    #else
        return _CountOneBits64(value);
    #endif
#else
    return privateNumBitsSet(value);
#endif
}

inline
int BitUtil::numBitsSet(unsigned long value)
{
    return numBitsSet(normalize(value));
}

inline
int BitUtil::numLeadingUnsetBits(unsigned int value)
{
#if defined(BDLB_BITUTIL_USE_GNU_INTRINSICS)
    // '__builtin_clz(0)' is undefined
    return __builtin_clz(value | 1) + static_cast<int>(!value);
#elif defined(BDLB_BITUTIL_USE_MSVC_INTRINSICS)
    // '_BitScanReverse(&index, 0)' sets 'index' to an unspecified value
    unsigned long index;
    return _BitScanReverse(&index, value)
         ? k_BITS_PER_INT32 - 1 - index
         : k_BITS_PER_INT32;
#else
    return privateNumLeadingUnsetBits(value);
#endif
}

inline
int BitUtil::numLeadingUnsetBits(unsigned long long value)
{
#if defined(BDLB_BITUTIL_USE_GNU_INTRINSICS)
    // '__builtin_clzll(0)' is undefined
    return __builtin_clzll(value | 1) + static_cast<int>(!value);
#elif defined(BDLB_BITUTIL_USE_MSVC_INTRINSICS)
    #if defined(BSLS_PLATFORM_CPU_64_BIT)
        // '_BitScanReverse64(&index, 0)' sets 'index' to an unspecified value
        unsigned long index;
        return _BitScanReverse64(&index, value)
             ? k_BITS_PER_INT64 - 1 - index
             : k_BITS_PER_INT64;
    #else
        // '_BitScanReverse64' available only in 64bit target
        return value > 0xffffffff
             ? numLeadingUnsetBits(static_cast<unsigned int>(
                                                    value >> k_BITS_PER_INT32))
             : numLeadingUnsetBits(static_cast<unsigned int>(value))
                                                            + k_BITS_PER_INT32;
    #endif
#else
    return privateNumLeadingUnsetBits(value);
#endif
}

inline
int BitUtil::numLeadingUnsetBits(unsigned long value)
{
    return numLeadingUnsetBits(normalize(value));
}

inline
int BitUtil::numTrailingUnsetBits(unsigned int value)
{
#if defined(BDLB_BITUTIL_USE_GNU_INTRINSICS)
    enum {
        k_INT32_MASK = k_BITS_PER_INT32 - 1
    };
    const unsigned int a = __builtin_ffs(value) - 1;
    return (a & k_INT32_MASK) + (a >> k_INT32_MASK);

    // Other possibility:
    //..
    //  return (__builtin_ffs(value) - 1) ^ ((-!value) & ~k_BITS_PER_INT32);
    //..
#elif defined(BDLB_BITUTIL_USE_MSVC_INTRINSICS)
    // '_BitScanForward(&index, 0)' sets 'index' to an unspecified value
    unsigned long index;
    return _BitScanForward(&index, value) ? index : k_BITS_PER_INT32;
#else
    return privateNumTrailingUnsetBits(value);
#endif
}

inline
int BitUtil::numTrailingUnsetBits(unsigned long long value)
{
#if defined(BDLB_BITUTIL_USE_GNU_INTRINSICS)
    enum {
        k_INT64_MASK = k_BITS_PER_INT64 - 1,
        k_INT32_MASK = k_BITS_PER_INT32 - 1
    };
    const unsigned int a = __builtin_ffsll(value) - 1;
    return (a & k_INT64_MASK) + (a >> k_INT32_MASK);

    // Other possibility:
    //..
    //  return (__builtin_ffsll(value) - 1) ^ ((-!value) & ~k_BITS_PER_INT64);
    //..
#elif defined(BDLB_BITUTIL_USE_MSVC_INTRINSICS)
    #if defined(BSLS_PLATFORM_CPU_64_BIT)
        // '_BitScanForward64(&index, 0)' sets 'index' to an unspecified value
        unsigned long index;
        return _BitScanForward64(&index, value) ? index : k_BITS_PER_INT64;
    #else
        // '_BitScanForward64' available only in 64bit target
        return 0 != (value & 0xffffffff)
            ? numTrailingUnsetBits(static_cast<unsigned int>(value))
            : numTrailingUnsetBits(static_cast<unsigned int>(
                                value >> k_BITS_PER_INT32)) + k_BITS_PER_INT32;
    #endif
#else
    return privateNumTrailingUnsetBits(value);
#endif
}

inline
int BitUtil::numTrailingUnsetBits(unsigned long value)
{
    return numTrailingUnsetBits(normalize(value));
}

inline
unsigned int BitUtil::roundUp(unsigned int value, unsigned int boundary)
{
    BSLS_ASSERT(1 == numBitsSet(boundary));

    return ((value - 1) | (boundary - 1)) + 1;
}

inline
unsigned long long BitUtil::roundUp(unsigned long long value,
                                    unsigned long long boundary)
{
    BSLS_ASSERT(1 == numBitsSet(boundary));

    return ((value - 1) | (boundary - 1)) + 1;
}

inline
unsigned long BitUtil::roundUp(unsigned long value, unsigned long boundary)
{
    return roundUp(normalize(value), normalize(boundary));
}

inline
unsigned int BitUtil::roundUpToBinaryPower(unsigned int value)
{
    const int index = numLeadingUnsetBits(value - 1);
    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(0 < index)
           ? 1U << (k_BITS_PER_INT32 - index)
           : 0;
}

inline
unsigned long long BitUtil::roundUpToBinaryPower(unsigned long long value)
{
    const int index = numLeadingUnsetBits(value - 1);
    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(0 < index)
           ? 1ULL << (k_BITS_PER_INT64 - index)
           : 0;
}

inline
unsigned long BitUtil::roundUpToBinaryPower(unsigned long value)
{
    return roundUpToBinaryPower(normalize(value));
}

template <class TYPE>
inline
int BitUtil::sizeInBits(TYPE)
{
    return static_cast<int>(CHAR_BIT * sizeof(TYPE));
}

inline
unsigned int BitUtil::withBitCleared(unsigned int value, int index)
{
    BSLS_ASSERT(    0 <= index);
    BSLS_ASSERT(index <  k_BITS_PER_INT32);

    return value & ~(1 << index);
}

inline
unsigned long long BitUtil::withBitCleared(unsigned long long value, int index)
{
    BSLS_ASSERT(    0 <= index);
    BSLS_ASSERT(index <  k_BITS_PER_INT64);

    return value & ~(1ULL << index);
}

inline
unsigned long BitUtil::withBitCleared(unsigned long value, int index)
{
    return withBitCleared(normalize(value), index);
}

inline
unsigned int BitUtil::withBitSet(unsigned int value, int index)
{
    BSLS_ASSERT(    0 <= index);
    BSLS_ASSERT(index <  k_BITS_PER_INT32);

    return value | (1 << index);
}

inline
unsigned long long BitUtil::withBitSet(unsigned long long value, int index)
{
    BSLS_ASSERT(    0 <= index);
    BSLS_ASSERT(index <  k_BITS_PER_INT64);

    return value | (1ULL << index);
}

inline
unsigned long BitUtil::withBitSet(unsigned long value, int index)
{
    return withBitSet(normalize(value), index);
}

}  // close package namespace
}  // close enterprise namespace

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
