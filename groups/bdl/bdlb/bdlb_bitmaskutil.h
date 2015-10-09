// bdlb_bitmaskutil.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLB_BITMASKUTIL
#define INCLUDED_BDLB_BITMASKUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide simple mask values of 'uint32_t' and 'uint64_t' types.
//
//@CLASSES:
//  bdlb::BitMaskUtil: namespace for bit-level mask operations
//
//@DESCRIPTION: This component provides a utility 'struct',
// 'bdlb::BitMaskUtil', that serves as a namespace for a collection of
// functions that provide simple binary masks.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creation of Simple Bit Masks
///- - - - - - - - - - - - - - - - - - - -
// The following usage examples illustrate how some of the methods provided by
// this component are used.  Note that, in all of these examples, the low-order
// bit is considered bit 0 and resides on the right edge of the bit string.
//
// First, the 'ge' function takes a single argument, 'index', and returns a bit
// mask with all bits below the specified 'index' cleared and all bits at or
// above the 'index' set:
//..
//  +-------------------------------------------------------------------------+
//  | 'bdlb::BitMaskUtil::ge(16)' in binary:                                  |
//  |                                                                         |
//  | 'index': bit 16:                                      *                 |
//  | All bits at and above bit 16 are set:  11111111111111110000000000000000 |
//  +-------------------------------------------------------------------------+
//
//  const uint32_t expGe = 0xffff0000;
//  assert(expGe == bdlb::BitMaskUtil::ge(16));
//..
// Next, the 'lt' function returns a bit mask with all bits at or above the
// specified 'index' cleared, and all bits below 'index' set.  'lt' and 'ge'
// return the complement of each other if passed the same 'index':
//..
//  +-------------------------------------------------------------------------+
//  | 'bdlb::BitMaskUtil::lt(16)' in binary:                                  |
//  |                                                                         |
//  | 'index': bit 16:                                      *                 |
//  | All bits below bit 16 are set:         00000000000000001111111111111111 |
//  +-------------------------------------------------------------------------+
//
//  const uint32_t expLt = 0x0000ffff;
//  assert(expLt == bdlb::BitMaskUtil::lt(16));
//
//  assert(expGe == ~expLt);
//..
// Then, the 'eq' function returns a bit mask with only the bit at the
// specified 'index' set:
//..
//  +-------------------------------------------------------------------------+
//  | 'bdlb::BitMaskUtil::eq(23)' in binary:                                  |
//  |                                                                         |
//  | 'index': bit 23:                               *                        |
//  | Only bit 23 is set:                    00000000100000000000000000000000 |
//  +-------------------------------------------------------------------------+
//
//  const uint32_t expEq = 0x00800000;
//  assert(expEq == bdlb::BitMaskUtil::eq(23));
//..
// Now, the 'ne' function returns a bit mask with only the bit at the specified
// 'index' cleared.  'ne' and 'eq' return the complement of each other for a
// given 'index':
//..
//  +-------------------------------------------------------------------------+
//  | 'bdlb::BitMaskUtil::ne(23)' in binary:                                  |
//  |                                                                         |
//  | 'index': bit 23:                               *                        |
//  | All bits other than bit 16 are set:    11111111011111111111111111111111 |
//  +-------------------------------------------------------------------------+
//
//  const uint32_t expNe = 0xff7fffff;
//  assert(expNe == bdlb::BitMaskUtil::ne(23));
//
//  assert(expEq == ~expNe);
//..
// Finally, 'one' and 'zero' return a bit mask with all bits within a specified
// range starting from a specified 'index' either set or cleared, respectively.
// For the same arguments, 'one' and 'zero' return the complement of each
// other:
//..
//  +-------------------------------------------------------------------------+
//  | 'bdlb::BitMaskUtil::one(16, 4)' in binary:                              |
//  |                                                                         |
//  | bit 16:                                               *                 |
//  | 4 bits starting at bit 16:                         ****                 |
//  | Result: only those bits set:           00000000000011110000000000000000 |
//  +-------------------------------------------------------------------------+
//
//  const uint32_t expOne = 0x000f0000;
//  assert(expOne == bdlb::BitMaskUtil::one(16, 4));
//
//  +-------------------------------------------------------------------------+
//  | 'bdlb::BitMaskUtil::zero(16, 4)' in binary:                             |
//  |                                                                         |
//  | bit 16:                                               *                 |
//  | 4 bits starting at bit 16:                         ****                 |
//  | Result: only those bits cleared:       11111111111100001111111111111111 |
//  +-------------------------------------------------------------------------+
//
//  const uint32_t expZero = 0xfff0ffff;
//  assert(expZero == bdlb::BitMaskUtil::zero(16, 4));
//
//  assert(expZero == ~expOne);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLB_BITUTIL
#include <bdlb_bitutil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSL_CSTDINT
#include <bsl_cstdint.h>
#endif

namespace BloombergLP {
namespace bdlb {

                               // ==================
                               // struct BitMaskUtil
                               // ==================

struct BitMaskUtil {
    // This utility 'struct' provides a namespace for a set of bit-level,
    // stateless functions that take one or two 'int' arguments and return
    // masks of the built-in 32- and 64-bit integer types 'uint32_t' and
    // 'uint64_t', respectively.

    // PUBLIC TYPES
    enum {
        k_BITS_PER_UINT32 = 32,  // number of bits in type 'uint32_t'

        k_BITS_PER_UINT64 = 64   // number of bits in type 'uint64_t'
    };

    // CLASS METHODS
    static bsl::uint32_t eq(int   index);
    static bsl::uint64_t eq64(int index);
        // Return the unsigned integral value having the bit at the specified
        // 'index' position set to 1, and all other bits set to 0.  The
        // behavior is undefined unless '0 <= index <= # of bits in result'.

    static bsl::uint32_t ge(int   index);
    static bsl::uint64_t ge64(int index);
        // Return the unsigned integral value having all bits at positions
        // greater than or equal to the specified 'index' set to 1, and all
        // other bits set to 0.  The behavior is undefined unless
        // '0 <= index <= # of bits in result'.

    static bsl::uint32_t gt(int   index);
    static bsl::uint64_t gt64(int index);
        // Return the unsigned integral value having all bits at positions
        // greater than the specified 'index' set to 1, and all other bits set
        // to 0.  The behavior is undefined unless
        // '0 <= index <= # of bits in result'.

    static bsl::uint32_t le(int   index);
    static bsl::uint64_t le64(int index);
        // Return the unsigned integral value having all bits at positions less
        // than or equal to the specified 'index' set to 1, and all other bits
        // set to 0.  The behavior is undefined unless
        // '0 <= index <= # of bits in result'.

    static bsl::uint32_t lt(int   index);
    static bsl::uint64_t lt64(int index);
        // Return the unsigned integral value having all bits at positions less
        // than the specified 'index' set to 1, and all other bits set to 0.
        // The behavior is undefined unless
        // '0 <= index <= # of bits in result'.

    static bsl::uint32_t ne(int   index);
    static bsl::uint64_t ne64(int index);
        // Return the unsigned integral value having the bit at the specified
        // 'index' position set to 0, and all other bits set to 1.  The
        // behavior is undefined unless '0 <= index <= # of bits in result'.

    static bsl::uint32_t one(int   index, int numBits);
    static bsl::uint64_t one64(int index, int numBits);
        // Return the unsigned integral value having the specified 'numBits'
        // starting at the specified 'index' set to 1, and all other bits set
        // to 0.  The behavior is undefined unless '0 <= index',
        // '0 <= numBits', and 'index + numBits <= # of bits in result'.

    static bsl::uint32_t zero(int   index, int numBits);
    static bsl::uint64_t zero64(int index, int numBits);
        // Return the unsigned integral value having the specified 'numBits'
        // starting at the specified 'index' set to 0, and all other bits set
        // to 1.  The behavior is undefined unless '0 <= index',
        // '0 <= numBits', and 'index + numBits <= # of bits in result'.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                               // ------------------
                               // struct BitMaskUtil
                               // ------------------

// CLASS METHODS
inline
bsl::uint32_t BitMaskUtil::eq(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index <= static_cast<int>(k_BITS_PER_UINT32));

    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                   index < static_cast<int>(k_BITS_PER_UINT32))
           ? 1 << index
           : 0;
}

inline
bsl::uint64_t BitMaskUtil::eq64(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index <= static_cast<int>(k_BITS_PER_UINT64));

    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                   index < static_cast<int>(k_BITS_PER_UINT64))
           ? 1LL << index
           : 0;
}

inline
bsl::uint32_t BitMaskUtil::ge(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index <= static_cast<int>(k_BITS_PER_UINT32));

    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                   index < static_cast<int>(k_BITS_PER_UINT32))
           ? (~0 << index)
           : 0;
}

inline
bsl::uint64_t BitMaskUtil::ge64(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index <= static_cast<int>(k_BITS_PER_UINT64));

    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                   index < static_cast<int>(k_BITS_PER_UINT64))
           ? (~0ULL << index)
           : 0;
}

inline
bsl::uint32_t BitMaskUtil::gt(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index <= static_cast<int>(k_BITS_PER_UINT32));

    ++index;
    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                   index < static_cast<int>(k_BITS_PER_UINT32))
           ? ~((1 << index) - 1)
           : 0;
}

inline
bsl::uint64_t BitMaskUtil::gt64(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index <= static_cast<int>(k_BITS_PER_UINT64));

    ++index;
    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                   index < static_cast<int>(k_BITS_PER_UINT64))
           ? ~((1LL << index) - 1)
           : 0;
}

inline
bsl::uint32_t BitMaskUtil::le(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index <= static_cast<int>(k_BITS_PER_UINT32));

    ++index;
    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                   index < static_cast<int>(k_BITS_PER_UINT32))
           ? (1 << index) - 1
           : -1;
}

inline
bsl::uint64_t BitMaskUtil::le64(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index <= static_cast<int>(k_BITS_PER_UINT64));

    ++index;
    return BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                   index < static_cast<int>(k_BITS_PER_UINT64))
           ? (1LL << index) - 1
           : -1LL;
}

inline
bsl::uint32_t BitMaskUtil::lt(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index <= static_cast<int>(k_BITS_PER_UINT32));

    return BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                  index >= static_cast<int>(k_BITS_PER_UINT32))
           ? -1
           : (1 << index) - 1;
}

inline
bsl::uint64_t BitMaskUtil::lt64(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index <= static_cast<int>(k_BITS_PER_UINT64));

    return BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                  index >= static_cast<int>(k_BITS_PER_UINT64))
           ? -1LL
           : (1LL << index) - 1;
}

inline
bsl::uint32_t BitMaskUtil::ne(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index <= static_cast<int>(k_BITS_PER_UINT32));

    return BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                  index >= static_cast<int>(k_BITS_PER_UINT32))
           ? -1
           : ~(1 << index);
}

inline
bsl::uint64_t BitMaskUtil::ne64(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index <= static_cast<int>(k_BITS_PER_UINT64));

    return BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                  index >= static_cast<int>(k_BITS_PER_UINT64))
           ? -1LL
           : ~(1LL << index);
}

inline
bsl::uint32_t BitMaskUtil::one(int index, int numBits)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(0 <= numBits);
    BSLS_ASSERT_SAFE(index + numBits <= static_cast<int>(k_BITS_PER_UINT32));

    return lt(index + numBits) & ge(index);
}

inline
bsl::uint64_t BitMaskUtil::one64(int index, int numBits)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(0 <= numBits);
    BSLS_ASSERT_SAFE(index + numBits <= static_cast<int>(k_BITS_PER_UINT64));

    return lt64(index + numBits) & ge64(index);
}

inline
bsl::uint32_t BitMaskUtil::zero(int index, int numBits)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(0 <= numBits);
    BSLS_ASSERT_SAFE(index + numBits <= static_cast<int>(k_BITS_PER_UINT32));

    return lt(index) | ge(index + numBits);
}

inline
bsl::uint64_t BitMaskUtil::zero64(int index, int numBits)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(0 <= numBits);
    BSLS_ASSERT_SAFE(index + numBits <= static_cast<int>(k_BITS_PER_UINT64));

    return lt64(index) | ge64(index + numBits);
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
