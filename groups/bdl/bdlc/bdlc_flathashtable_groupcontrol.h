// bdlc_flathashtable_groupcontrol.h                                  -*-C++-*-
#ifndef INCLUDED_BDLC_FLATHASHTABLE_GROUPCONTROL
#define INCLUDED_BDLC_FLATHASHTABLE_GROUPCONTROL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide inquiries to a flat hash table group of control values.
//
//@CLASSES:
//  bdlc::FlatHashTable_GroupControl: flat hash table group control inquiries
//
//@DESCRIPTION: This component implements the class,
// 'bdlc::FlatHashTable_GroupControl', that provides query methods to a group
// of flat hash table control values.  Note that the number of entries in a
// group control and the inquiry performance is platform dependant.
//
// The flat hash map/set/table data structures are inspired by Google's
// flat_hash_map CppCon presentations (available on youtube).  The
// implementations draw from Google's open source 'raw_hash_set.h' file at:
// https://github.com/abseil/abseil-cpp/blob/master/absl/container/internal.
//
///Usage
///-----
// There is no usage example for this component since it is not meant for
// direct client use.

#include <bdlscm_version.h>

#include <bsls_assert.h>
#include <bsls_byteorder.h>
#include <bsls_platform.h>

#include <bsl_cstddef.h>
#include <bsl_cstdint.h>
#include <bsl_cstring.h>

#if defined(BSLS_PLATFORM_CPU_SSE2)
#include <immintrin.h>
#include <emmintrin.h>
#endif

namespace BloombergLP {
namespace bdlc {

                     // ================================
                     // class FlatHashTable_GroupControl
                     // ================================

class FlatHashTable_GroupControl
    // This class provides methods for making inquires to the data of a group
    // control loading during construction.
{
  public:
    // TYPES
#if defined(BSLS_PLATFORM_CPU_SSE2)
    typedef __m128i       Storage;
#else
    typedef bsl::uint64_t Storage;
#endif

  private:
    // CLASS DATA
    static const bsl::uint64_t k_MULT          = 0x0101010101010101ull;
    static const bsl::uint64_t k_DEFLATE       = 0x0002040810204081ull;
    static const int           k_DEFLATE_SHIFT = 56;
    static const bsl::uint64_t k_MSB_MASK      = 0x8080808080808080ull;

    // DATA
    Storage d_value;  // efficiently cached value for inquiries

    // PRIVATE ACCESSORS
    bsl::uint32_t matchRaw(bsl::uint8_t value) const;
        // Return a bit mask of the 'k_SIZE' entries that have the specified
        // 'value'.  The bit at index 'i' corresponds to the result for
        // 'data[i]'.

    // NOT IMPLEMENTED
    FlatHashTable_GroupControl();
    FlatHashTable_GroupControl(const FlatHashTable_GroupControl&);
    FlatHashTable_GroupControl& operator=(const FlatHashTable_GroupControl&);

  public:
    // PUBLIC CLASS DATA
    static const bsl::uint8_t k_EMPTY  = 0x80;  // = 0b10000000
    static const bsl::uint8_t k_ERASED = 0xC0;  // = 0b11000000
    static const bsl::size_t  k_SIZE   = sizeof(Storage);

    // CREATORS
    explicit FlatHashTable_GroupControl(const bsl::uint8_t *data);
        // Create a group control query object using the specified 'data'.  The
        // bytes of 'data' have no alignment requirement.  The behavior is
        // undefined unless 'data' has at least 'k_SIZE' bytes available.

    //! ~FlatHashTable_GroupControl() = default;
        // Destroy this object.

    // ACCESSORS
    bsl::uint32_t available() const;
        // Return a bit mask of the 'k_SIZE' entries that are empty or erased.
        // The bit at index 'i' corresponds to the result for 'data[i]'.

    bsl::uint32_t inUse() const;
        // Return a bit mask of the 'k_SIZE' entries that are in use (i.e., not
        // empty or erased).  The bit at index 'i' corresponds to the result
        // for 'data[i]'.

    bsl::uint32_t match(bsl::uint8_t value) const;
        // Return a bit mask of the 'k_SIZE' entries that have the specified
        // 'value'.  The bit at index 'i' corresponds to the result for
        // 'data[i]'.  The behavior is undefined unless '0 == (0x80 & value)'.

    bool neverFull() const;
        // Return 'true' if this group control was never full (i.e., has a
        // value that is empty, but not erased).
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                     // --------------------------------
                     // class FlatHashTable_GroupControl
                     // --------------------------------

// PRIVATE ACCESSORS
inline
bsl::uint32_t FlatHashTable_GroupControl::matchRaw(bsl::uint8_t value) const
{
#if defined(BSLS_PLATFORM_CPU_SSE2)
    return _mm_movemask_epi8(_mm_cmpeq_epi8(
                                       _mm_set1_epi8(static_cast<char>(value)),
                                       d_value));
#else
    Storage t = d_value ^ (k_MULT * value);

    t = t | (t << 4);
    t = t | (t << 2);
    t = t | (t << 1);

    return static_cast<bsl::uint32_t>(
                         (((~t) & k_MSB_MASK) * k_DEFLATE) >> k_DEFLATE_SHIFT);
#endif
}

// CREATORS
inline
FlatHashTable_GroupControl::FlatHashTable_GroupControl(
                                                      const bsl::uint8_t *data)
{
#if defined(BSLS_PLATFORM_CPU_SSE2)
    d_value = _mm_loadu_si128(static_cast<const Storage *>(
                                             static_cast<const void *>(data)));
#else
    bsl::memcpy(&d_value, data, k_SIZE);
    d_value = BSLS_BYTEORDER_HOST_U64_TO_LE(d_value);
#endif
}

// ACCESSORS
inline
bsl::uint32_t FlatHashTable_GroupControl::available() const
{
#if defined(BSLS_PLATFORM_CPU_SSE2)
    return _mm_movemask_epi8(d_value);
#else
    return static_cast<bsl::uint32_t>(
                      ((d_value & k_MSB_MASK) * k_DEFLATE) >> k_DEFLATE_SHIFT);
#endif
}

inline
bsl::uint32_t FlatHashTable_GroupControl::inUse() const
{
#if defined(BSLS_PLATFORM_CPU_SSE2)
    return (~available()) & 0xFFFF;
#else
    return (~available()) & 0xFF;
#endif
}

inline
bsl::uint32_t FlatHashTable_GroupControl::match(bsl::uint8_t value) const
{
    BSLS_ASSERT_SAFE(0 == (value & 0x80));

    return matchRaw(value);
}

inline
bool FlatHashTable_GroupControl::neverFull() const
{
    return 0 != matchRaw(k_EMPTY);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
