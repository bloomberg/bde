// bdlb_bitstringimputil.h                                            -*-C++-*-
#ifndef INCLUDED_BDLB_BITSTRINGIMPUTIL
#define INCLUDED_BDLB_BITSTRINGIMPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functional bit-manipulation of 'uint64_t' values.
//
//@CLASSES:
//  bdlb::BitStringImpUtil: namespace for 'uint64_t' utilities
//
//@SEE_ALSO: bdlb_bitstringutil
//
//@DESCRIPTION: This component provides a utility 'struct',
// 'bdlb::BitStringImpUtil', that serves as a namespace for a collection of
// functions that provide bit-level operations on 'uint64_t' values.  Some of
// these functions consist of a single bitwise logical operation.  The point of
// implementing them as functions is to facilitate providing these functions as
// arguments to templates in 'bdlb_bitstringutil'.
//
// Note that no functions supporting 'uint32_t' are provided here.  This
// component exists solely to support 'bdlb::BitStringUtil', which deals
// entirely in 'uint64_t' values.
//
// Note that the 'find*' functions defined here only find set bits -- there is
// never a context in 'bdlb_bitstringutil' where a 'find*' that found clear
// bits is needed.
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
// Note that, in all of these examples, the low-order bit is considered bit 0
// and resides on the right edge of the bit string.
//
///Example 1: Manipulators
///- - - - - - - - - - - -
// This example demonstrates the "manipulator" static functions defined in this
// component, which can change the state of a 'uint64_t'.
//
// The '*EqBits' functions ('andEqBits', 'minusEqBits', 'orEqBits', and
// 'xorEqBits'), have the following signature:
//..
//    void function(uint64_t *dstValue,
//                  int       dstIndex,
//                  uint64_t  srcValue,
//                  int       numBits);
//..
// First, we demonstrate the 'andEqBits' function:
//..
// +--------------------------------------------------------------------------+
// | 'bdlb::BitStringImpUtil::andEqBits(&dstValue, 8, 0, 8)' in binary:       |
// |                                                                          |
// | 'dstValue' before in binary:       0..00000000000000000011001100110011   |
// | 'srcValue == 0' in binary:         0..00000000000000000000000000000000   |
// | 'srcValue', 0x00, at index 8:                         00000000           |
// | 'dstValue' after in binary:        0..00000000000000000000000000110011   |
// +--------------------------------------------------------------------------+
//
//  uint64_t dstValue;
//
//  dstValue = 0x3333;
//  bdlb::BitStringImpUtil::andEqBits(&dstValue, 8, 0, 8);
//  assert(static_cast<uint64_t>(0x33) == dstValue);
//..
// Then, we apply 'andEqBits' with all bits set in the relevant part of
// 'srcValue, which has no effect:
//..
// +--------------------------------------------------------------------------+
// | 'bdlb::BitStringImpUtil::andEqBits(&dstValue, 8, 0, 8)' in binary:       |
// |                                                                          |
// | 'dstValue' before in binary:       0..00000000000000000011001100110011   |
// | 'srcValue == 0xffff' in binary:    0..00000000000000001111111111111111   |
// | 'srcValue', 0xff, at index 8:                         11111111           |
// | 'dstValue' after in binary:        0..00000000000000000011001100110011   |
// +--------------------------------------------------------------------------+
//
//  dstValue = 0x3333;
//  bdlb::BitStringImpUtil::andEqBits(&dstValue, 8, 0xffff, 8);
//  assert(static_cast<uint64_t>(0x3333) == dstValue);
//..
// Next, we demonstrate 'orEqBits', which takes low-order bits of a 'srcValue'
// and bitwise ORs them with 'dstValue':
//..
// +--------------------------------------------------------------------------+
// | 'bdlb::BitStringImpUtil::orEqBits(&dstValue, 16, 0xffff, 8)' in binary:  |
// |                                                                          |
// | 'dstValue' before in binary:       0..00110011001100110011001100110011   |
// | 'srcValue == 0xffff' in binary:    0..00000000000000001111111111111111   |
// | 'srcValue', 0xff, at index 16:                11111111                   |
// | 'dstValue' after in binary:        0..00110011111111110011001100110011   |
// +--------------------------------------------------------------------------+
//
//  dstValue = 0x33333333;
//  bdlb::BitStringImpUtil::orEqBits(&dstValue, 16, 0xffff, 8);
//  assert(static_cast<uint64_t>(0x33ff3333) == dstValue);
//..
// Then, we demonstrate applying the same operation where '*dstValue' is
// initially 0:
//..
// +--------------------------------------------------------------------------+
// | 'bdlb::BitStringImpUtil::orEqBits(&dstValue, 16, 0xffff, 8)' in binary:  |
// |                                                                          |
// | 'dstValue' before in binary:       0..00000000000000000000000000000000   |
// | 'srcValue == 0xffff' in binary:    0..00000000000000001111111111111111   |
// | 'srcValue', 0xff, at index 16:                11111111                   |
// | 'dstValue' after in binary:        0..00000000111111110000000000000000   |
// +--------------------------------------------------------------------------+
//
//  dstValue = 0;
//  bdlb::BitStringImpUtil::orEqBits(&dstValue, 16, 0xffff, 8);
//  assert(static_cast<uint64_t>(0x00ff0000) == dstValue);
//..
// Now, we apply another function, 'xorEqBits', that takes the low-order bits
// of 'srcValue' and bitwise XORs them with 'dstValue':
//..
// +--------------------------------------------------------------------------+
// | 'bdlb::BitStringImpUtil::xorEqBits(&dstValue, 16, 0xffff, 8)' in binary: |
// |                                                                          |
// | 'dstValue' before in binary:       0..01110111011101110111011101110111   |
// | 'srcValue', 0xff, at index 16:                11111111                   |
// | 'dstValue' after in binary:        0..01110111100010000111011101110111   |
// ----------------------------------------------------------------------------
//
//  dstValue = 0x77777777;
//  bdlb::BitStringImpUtil::xorEqBits(&dstValue, 16, 0xffff, 8);
//  assert(static_cast<uint64_t>(0x77887777) == dstValue);
//..
// Finally, we apply the same function with a different value of 'srcValue'
// and observe the result:
//..
// +--------------------------------------------------------------------------+
// | 'bdlb::BitStringImpUtil::xorEqBits(&dstValue, 16, 0x5555, 8)' in binary: |
// |                                                                          |
// | 'dstValue' before in binary:       0..01110111011101110111011101110111   |
// | 'srcValue', 0x55, at index 16:                01010101                   |
// | 'dstValue' after in binary:        0..01110111001000100111011101110111   |
// +--------------------------------------------------------------------------+
//
//  dstValue = 0x77777777;
//  bdlb::BitStringImpUtil::xorEqBits(&dstValue, 16, 0x5555, 8);
//  assert(static_cast<uint64_t>(0x77227777) == dstValue);
//..
//
///Accessors
///- - - - -
// This example demonstrates the "accessor" static functions, which read, but
// do not modify, the state of a 'uint64_t'.
//
// The 'find1At(Max,Min)IndexRaw' routines are used for finding the
// highest-order (or lowest-order) set bit in a 'uint64_t'.  These functions
// are "raw" because the behavior is undefined if they are passed 0.
//
// First, we apply 'find1AtMaxIndexRaw':
//..
// +--------------------------------------------------------------------------+
// | 'bdlb::BitStringImpUtil::find1AtMaxIndexRaw(0x10a)' in binary:           |
// |                                                                          |
// | input:                             0..000000000000000000000000100001010  |
// | bit 8, highest bit set:                                       1          |
// +--------------------------------------------------------------------------+
//
//  assert(8 == bdlb::BitStringImpUtil::find1AtMaxIndexRaw(0x10a));
//..
// Finally, we apply 'find1AtMinIndexRaw':
//..
// +--------------------------------------------------------------------------+
// | 'bdlb::BitStringImpUtil::find1AtMinIndexRaw(0xffff0180)' in binary:      |
// |                                                                          |
// | input:                             0..011111111111111110000000110000000  |
// | bit 7, lowest bit set:                                         1         |
// +--------------------------------------------------------------------------+
//
//  assert(7 == bdlb::BitStringImpUtil::find1AtMinIndexRaw(0xffff0180));
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLB_BITMASKUTIL
#include <bdlb_bitmaskutil.h>
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

                           // =======================
                           // struct BitStringImpUtil
                           // =======================

struct BitStringImpUtil {
    // This 'struct' provides a namespace for static functions to be used
    // solely in the implementation of 'BitStringUtil'.  The "Manipulators"
    // are intended to be provided as arguments to templates in
    // 'bdlb_bitstringutil', whereas the "Accessors" are to be called directly
    // within that component.

    // PUBLIC TYPES
    enum { k_BITS_PER_UINT64 = 64 };  // number of bits in 'uint64_t'

    // CLASS METHODS

                                // Manipulators

    static void andEqBits(bsl::uint64_t *dstValue,
                          int            dstIndex,
                          bsl::uint64_t  srcValue,
                          int            numBits);
        // Bitwise AND the specified least-significant 'numBits' in the
        // specified 'srcValue' to those in the specified 'dstValue' starting
        // at the specified 'dstIndex'.  The behavior is undefined unless
        // '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= k_BITS_PER_UINT64'.

    static void andEqWord(bsl::uint64_t *dstValue, bsl::uint64_t srcValue);
        // Assign to the specified '*dstValue' the value of '*dstValue' bitwise
        // AND-ed with the specified 'srcValue'.

    static void minusEqBits(bsl::uint64_t *dstValue,
                            int            dstIndex,
                            bsl::uint64_t  srcValue,
                            int            numBits);
        // Bitwise MINUS the specified least-significant 'numBits' in the
        // specified 'srcValue' from those in the specified 'dstValue' starting
        // at the specified 'dstIndex'.  The behavior is undefined unless
        // '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= k_BITS_PER_UINT64'.  Note that the bitwise
        // difference, 'a - b', is defined in C++ code as 'a & ~b'.

    static void minusEqWord(bsl::uint64_t *dstValue, bsl::uint64_t srcValue);
        // Assign to the specified '*dstValue' the value of '*dstValue' bitwise
        // AND-ed with the complement of the specified 'srcValue'.

    static void orEqBits(bsl::uint64_t *dstValue,
                         int            dstIndex,
                         bsl::uint64_t  srcValue,
                         int            numBits);
        // Bitwise OR the specified least-significant 'numBits' in the
        // specified 'srcValue' to those in the specified 'dstValue' starting
        // at the specified 'dstIndex'.  The behavior is undefined unless
        // '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= k_BITS_PER_UINT64'.

    static void orEqWord(bsl::uint64_t *dstValue, bsl::uint64_t srcValue);
        // Assign to the specified '*dstValue' the value of '*dstValue' bitwise
        // OR-ed with the specified 'srcValue'.

    static void setEqBits(bsl::uint64_t *dstValue,
                          int            dstIndex,
                          bsl::uint64_t  srcValue,
                          int            numBits);
        // Replace the specified 'numBits' in the specified 'dstValue' starting
        // at the specified 'dstIndex' with the least-significant 'numBits' of
        // the specified 'srcValue'.  The behavior is undefined unless
        // '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= k_BITS_PER_UINT64'.

    static void setEqWord(bsl::uint64_t *dstValue, bsl::uint64_t srcValue);
        // Assign to the specified '*dstValue' the value of the specified
        // 'srcValue'.

    static void xorEqBits(bsl::uint64_t *dstValue,
                          int            dstIndex,
                          bsl::uint64_t  srcValue,
                          int            numBits);
        // Bitwise XOR the specified least-significant 'numBits' in the
        // specified 'srcValue' to those in the specified 'dstValue' starting
        // at the specified 'dstIndex'.  The behavior is undefined unless
        // '0 <= dstIndex', '0 <= numBits', and
        // 'dstIndex + numBits <= k_BITS_PER_UINT64'.

    static void xorEqWord(bsl::uint64_t *dstValue, bsl::uint64_t srcValue);
        // Assign to the specified '*dstValue' the value of '*dstValue' bitwise
        // XOR-ed with the specified 'srcValue'.

                                // Accessors

    static int find1AtMaxIndexRaw(bsl::uint64_t value);
        // Return the index of the highest-order set bit in the specified
        // non-zero 'value'.  The behavior is undefined unless '0 != value'.
        // Note that this method is "raw" due to the requirement that at least
        // one bit in 'value' must be set.

    static int find1AtMinIndexRaw(bsl::uint64_t value);
        // Return the index of the lowest-order set bit in the specified
        // non-zero 'value'.  The behavior is undefined unless '0 != value'.
        // Note that this method is "raw" due to the requirement that at least
        // one bit in 'value' must be set.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                           // -----------------------
                           // struct BitStringImpUtil
                           // -----------------------

                                // Manipulators

inline
void BitStringImpUtil::andEqBits(bsl::uint64_t *dstValue,
                                 int            dstIndex,
                                 bsl::uint64_t  srcValue,
                                 int            numBits)
{
    BSLS_ASSERT_SAFE(dstValue);
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= k_BITS_PER_UINT64);

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(dstIndex < k_BITS_PER_UINT64)) {
        *dstValue &= BitMaskUtil::zero64(dstIndex, numBits) |
                                                       (srcValue << dstIndex);
    }
}

inline
void BitStringImpUtil::andEqWord(bsl::uint64_t *dstValue,
                                 bsl::uint64_t  srcValue)
{
    BSLS_ASSERT_SAFE(dstValue);

    *dstValue &= srcValue;
}

inline
void BitStringImpUtil::minusEqBits(bsl::uint64_t *dstValue,
                                   int            dstIndex,
                                   bsl::uint64_t  srcValue,
                                   int            numBits)
{
    BSLS_ASSERT_SAFE(dstValue);
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= k_BITS_PER_UINT64);

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(dstIndex < k_BITS_PER_UINT64)) {
        *dstValue &= BitMaskUtil::zero64(dstIndex, numBits) |
                                                      (~srcValue << dstIndex);
    }
}

inline
void BitStringImpUtil::minusEqWord(bsl::uint64_t *dstValue,
                                   bsl::uint64_t  srcValue)
{
    BSLS_ASSERT_SAFE(dstValue);

    *dstValue &= ~srcValue;
}

inline
void BitStringImpUtil::orEqBits(bsl::uint64_t *dstValue,
                                int            dstIndex,
                                bsl::uint64_t  srcValue,
                                int            numBits)
{
    BSLS_ASSERT_SAFE(dstValue);
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= k_BITS_PER_UINT64);

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(dstIndex < k_BITS_PER_UINT64)) {
        *dstValue |= (srcValue & BitMaskUtil::lt64(numBits)) << dstIndex;
    }
}

inline
void BitStringImpUtil::orEqWord(bsl::uint64_t *dstValue,
                                bsl::uint64_t  srcValue)
{
    BSLS_ASSERT_SAFE(dstValue);

    *dstValue |= srcValue;
}

inline
void BitStringImpUtil::setEqBits(bsl::uint64_t *dstValue,
                                 int            dstIndex,
                                 bsl::uint64_t  srcValue,
                                 int            numBits)
{
    BSLS_ASSERT_SAFE(dstValue);
    BSLS_ASSERT_SAFE(0                  <= dstIndex);
    BSLS_ASSERT_SAFE(0                  <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= k_BITS_PER_UINT64);

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(dstIndex < k_BITS_PER_UINT64)) {
        const bsl::uint64_t mask = BitMaskUtil::lt64(numBits);

        *dstValue &= ~(mask << dstIndex);
        *dstValue |= (srcValue & mask) << dstIndex;
    }
}

inline
void BitStringImpUtil::setEqWord(bsl::uint64_t *dstValue,
                                 bsl::uint64_t  srcValue)
{
    BSLS_ASSERT_SAFE(dstValue);

    *dstValue = srcValue;
}

inline
void BitStringImpUtil::xorEqBits(bsl::uint64_t *dstValue,
                                 int            dstIndex,
                                 bsl::uint64_t  srcValue,
                                 int            numBits)
{
    BSLS_ASSERT_SAFE(dstValue);
    BSLS_ASSERT_SAFE(                 0 <= dstIndex);
    BSLS_ASSERT_SAFE(                 0 <= numBits);
    BSLS_ASSERT_SAFE(dstIndex + numBits <= k_BITS_PER_UINT64);

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(dstIndex < k_BITS_PER_UINT64)) {
        *dstValue ^= (srcValue & BitMaskUtil::lt64(numBits)) << dstIndex;
    }
}

inline
void BitStringImpUtil::xorEqWord(bsl::uint64_t *dstValue,
                                 bsl::uint64_t  srcValue)
{
    BSLS_ASSERT_SAFE(dstValue);

    *dstValue ^= srcValue;
}

                                // Accessors

inline
int BitStringImpUtil::find1AtMaxIndexRaw(bsl::uint64_t value)
{
    BSLS_ASSERT_SAFE(0 != value);

    return k_BITS_PER_UINT64 - 1 - BitUtil::numLeadingUnsetBits(value);
}

inline
int BitStringImpUtil::find1AtMinIndexRaw(bsl::uint64_t value)
{
    BSLS_ASSERT_SAFE(0 != value);

    return BitUtil::numTrailingUnsetBits(value);
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
