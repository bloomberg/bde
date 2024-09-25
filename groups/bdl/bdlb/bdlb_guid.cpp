// bdlb_guid.cpp                                                      -*-C++-*-
#include <bdlb_guid.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_guid_cpp,"$Id$ $CSID$")

///Implementation Notes
///--------------------
// The member 'Guid::d_alignedBuffer' is aligned to 8 bytes in order to avoid
// slower unaligned loads with the SIMD instructions used to implement
// 'format' (and may similarly optimize loads and stores when copying 'Guid's).

#include <bsl_algorithm.h>
#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>
#include <bsl_ostream.h>

#include <bslim_printer.h>

#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>

#ifdef BSLS_PLATFORM_CPU_SSE4_1
#include <immintrin.h>
#endif

namespace BloombergLP {

namespace {

#ifdef BSLS_PLATFORM_CPU_SSE4_1

/// Convert 8 bytes of data starting at the specified `src` into its
/// equivalent hex representation, writing the resulting 16 characters to
/// the specified `dst` buffer, which must be aligned to a 16-byte boundary.
/// Note that `src` need not be aligned.
void hex16sse(void *dst, const unsigned char *src)
{
    // Load 8 bytes (unaligned)
    __m128i x = _mm_loadu_si64(src);
    // Isolate the high and low nibbles into individual bytes (mask & shift)
    __m128 nibbleMask = (__m128)_mm_set1_epi8(0x0f);
    __m128 lo = _mm_and_ps((__m128)x, nibbleMask);
    __m128 hi = _mm_and_ps((__m128)_mm_srli_epi32(x, 4), nibbleMask);
    // Pack the 8 high and 8 low nibble-bytes into one 16-byte value
    __m128i unpacked = _mm_unpacklo_epi8((__m128i)hi, (__m128i)lo);
    // Load the hex digit ASCII values (descending in little-endian order)
    __m128i digits = _mm_set_epi64((__m64)0x6665646362613938ull,
                                   (__m64)0x3736353433323130ull);
    // For each nibble-byte, index into the hex digits to convert to hex
    __m128i hex = _mm_shuffle_epi8(digits, unpacked);
    // Store 16 hex chars (aligned)
    _mm_store_si128((__m128i*)dst, hex);
}

#else

char hex1(unsigned x)
    // Return the equivalent hex ASCII character for the nibble specified in
    // 'x'.
{
    return "0123456789abcdef"[x];
}

void hex8(char *dst, const unsigned char *src)
    // Convert 4 bytes of data starting at the specified 'src' into its
    // equivalent hex representation, writing the resulting 8 characters to the
    // specified 'dst' buffer.
{
    dst[0] = hex1(src[0] >> 4);
    dst[1] = hex1(src[0] & 0xf);
    dst[2] = hex1(src[1] >> 4);
    dst[3] = hex1(src[1] & 0xf);
    dst[4] = hex1(src[2] >> 4);
    dst[5] = hex1(src[2] & 0xf);
    dst[6] = hex1(src[3] >> 4);
    dst[7] = hex1(src[3] & 0xf);
}

void hex16(char *dst, const unsigned char *src)
    // Convert 8 bytes of data starting at the specified 'src' into its
    // equivalent hex representation, writing the resulting 16 characters to
    // the specified 'dst' buffer.
{
    hex8(dst + 0, src + 0);
    hex8(dst + 8, src + 4);
}

#endif

}  // close unnamed namespace

namespace bdlb {

void Guid::format(bsl::span<char, k_GUID_NUM_CHARS> buffer) const
{
#ifdef BSLS_PLATFORM_CPU_SSE4_1
    bsls::AlignedBuffer<32, 16> temp;
    hex16sse(temp.buffer() +  0, data() +  0);
    hex16sse(temp.buffer() + 16, data() +  8);
#else
    bsls::AlignedBuffer<32, 8> temp;
    hex16(temp.buffer() +  0, data() +  0);
    hex16(temp.buffer() + 16, data() +  8);
#endif

    // Format: 00000000-0000-0000-0000-000000000000
    memcpy(buffer.data() + 0, temp.buffer() + 0, 8);
    buffer[8] = '-';
    memcpy(buffer.data() + 9, temp.buffer() + 8, 4);
    buffer[13] = '-';
    memcpy(buffer.data() + 14, temp.buffer() + 12, 4);
    buffer[18] = '-';
    memcpy(buffer.data() + 19, temp.buffer() + 16, 4);
    buffer[23] = '-';
    memcpy(buffer.data() + 24, temp.buffer() + 20, 12);
}

bsl::ostream& Guid::print(bsl::ostream& stream,
                          int           level,
                          int           spacesPerLevel) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(stream.bad())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return stream;                                                // RETURN
    }

    char buffer[k_GUID_NUM_CHARS];
    format(buffer);

    bslim::Printer printer(&stream, level, spacesPerLevel);

    printer.start(true);
    stream.write(buffer, sizeof(buffer));
    printer.end(true);

    return stream;
}

}  // close package namespace

// FREE OPERATORS
bool bdlb::operator<(const bdlb::Guid& lhs, const bdlb::Guid& rhs)
{
    return bsl::memcmp(&lhs[0], &rhs[0], bdlb::Guid::k_GUID_NUM_BYTES) < 0;
}

bool bdlb::operator>(const bdlb::Guid& lhs, const bdlb::Guid& rhs)
{
    return bsl::memcmp(&lhs[0], &rhs[0], bdlb::Guid::k_GUID_NUM_BYTES) > 0;
}

bool bdlb::operator<=(const bdlb::Guid& lhs, const bdlb::Guid& rhs)
{
    return bsl::memcmp(&lhs[0], &rhs[0], bdlb::Guid::k_GUID_NUM_BYTES) <= 0;
}

bool bdlb::operator>=(const bdlb::Guid& lhs, const bdlb::Guid& rhs)
{
    return bsl::memcmp(&lhs[0], &rhs[0], bdlb::Guid::k_GUID_NUM_BYTES) >= 0;
}

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
