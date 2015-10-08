// bdlde_md5.cpp                                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlde_md5.h>

///IMPLEMENTATION NOTES
///--------------------
// This implementation is based upon the MD5 implementation found at the end
// of RFC 1321 (http://www.ietf.org/rfc/rfc1321.txt).  Several implementation
// details are important:
//..
// 1. The constants, S11 to S44, are the same '#define' constants as specified
//    in the RFC document.  The functions, F, G, H, I, FF, GG, HH, II are the
//    same functions as specified in the RFC document as macros.  They are
//    converted from macros ('#define') to 'inline' functions and constants
//    within the unnamed 'namespace'.  However, to preserve consistency and
//    readability between this implementation and the one from the RFC, the
//    function and macro names have been retained even though they don't adhere
//    to BDE naming conventions.
//..
// 2. The length stored in the 'bdlde::Md5' object is the byte length of the
//    message.  However, when it is used in the MD5 digest, the bit length is
//    used.
//..
// 3. Whenever temporary variables are used to store sensitive information,
//    they are zero-ed out using 'memset', for example, the destructor.
//..
// 4. The function F and G are optimized.  '(x & y) | ((~x) & z)' and
//    'z ^ (x & (y ^ z))' are logically equivalent but the latter is faster.
//..
// 5. This works on both little- and big-endian machines.  The difference of
//    implementation lies in how we append the length bytes to the MD5 digest.
//    In a little-endian machine, we just copy byte by byte.  But in a big
//    endian machine, we reverse the byte order when appending to the MD5
//    digest because MD5 requires little-endian ordering for the length of the
//    message.
//..
// 6. Both the 'update' and 'loadDigest' functions will perform MD5
//    calculations.  The 'loadDigest' function requires calculation because of
//    the need to support the update function.  When we try to generate an MD5
//    digest, the length of the message is appended to the message for
//    calculations.  This action is very difficult to reverse due to
//    modification of buffers and internal states.  Yet, when appending new
//    data to the digest, we cannot keep the length of the original message in
//    the buffer, else an incorrect digest will be generated.  Hence, the
//    update function cannot pre-append the length to the message and process
//    it (as new data might come and the length might change).  Therefore, the
//    'loadDigest' function is responsible for part of the processing.
//..
// Other than changing from a C-style comment to C++-style comments, the
// following was taken verbatim from http://www.ietf.org/rfc/rfc1321.txt:
//..
// Copyright (C) 1991-2, RSA Data Security, Inc.  Created 1991.  All rights
// reserved.
//
// License to copy and use this software is granted provided that it is
// identified as the "RSA Data Security, Inc.  MD5 Message-Digest Algorithm" in
// all material mentioning or referencing this software or this function.
//
// License is also granted to make and use derivative works provided that such
// works are identified as "derived from the RSA Data Security, Inc.  MD5
// Message-Digest Algorithm" in all material mentioning or referencing the
// derived work.
//
// RSA Data Security, Inc. makes no representations concerning either the
// merchantability of this software or the suitability of this software for any
// particular purpose.  It is provided "as is" without express or implied
// warranty of any kind.
//
// These notices must be retained in any copies of any part of this
// documentation and/or software.
//..

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

#include <bsl_cstring.h>

namespace BloombergLP {

BSLMF_ASSERT(sizeof(unsigned int) == 4);

// The original implementation from RFC 1321 uses C macros.  They have been
// commented out (below) and replaced with 'inline' 'static' functions and
// constants.

//#define S11 7
//#define S12 12
//#define S13 17
//#define S14 22
//#define S21 5
//#define S22 9
//#define S23 14
//#define S24 20
//#define S31 4
//#define S32 11
//#define S33 16
//#define S34 23
//#define S41 6
//#define S42 10
//#define S43 15
//#define S44 21
//
//#define F(x, y, z) (z ^ (x & (y ^ z)))
//#define G(x, y, z) F(z, x, y)
//#define H(x, y, z) ((x) ^ (y) ^ (z))
//#define I(x, y, z) ((y) ^ ((x) | (~z)))
//
//#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))
//
//#define FF(a, b, c, d, x, s, ac) \@
//((a) += F ((b), (c), (d)) + (x) + (ac), (a) = ROTATE_LEFT ((a), (s)) + b )
//
//#define GG(a, b, c, d, x, s, ac) \@
//((a) += G ((b), (c), (d)) + (x) + (ac), (a) = ROTATE_LEFT ((a), (s)) + b )
//
//#define HH(a, b, c, d, x, s, ac) \@
//((a) += H ((b), (c), (d)) + (x) + (ac), (a) = ROTATE_LEFT ((a), (s)) + b )
//
//#define II(a, b, c, d, x, s, ac) \@
//((a) += I ((b), (c), (d)) + (x) + (ac), (a) = ROTATE_LEFT ((a), (s)) + b)
//
// Note that '@' is appended to each line in the macro that ends with '\' to
// quell a diagnostic from gcc ("warning: multi-line comment").

static const unsigned char u_md5Pad[] = {
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

// Constants used in MD5 algorithm specified by the RFC 1321 document.

static const int S11 = 7;
static const int S12 = 12;
static const int S13 = 17;
static const int S14 = 22;
static const int S21 = 5;
static const int S22 = 9;
static const int S23 = 14;
static const int S24 = 20;
static const int S31 = 4;
static const int S32 = 11;
static const int S33 = 16;
static const int S34 = 23;
static const int S41 = 6;
static const int S42 = 10;
static const int S43 = 15;
static const int S44 = 21;

static const int BITBLOCKSIZE = 512;                    // number of bits per
                                                        // block
static const int BYTEBLOCKSIZE = BITBLOCKSIZE / 8;      // number of bytes per
                                                        // block
static const int BYTELENGTHPADSIZE = 8;                 // number of bytes for
                                                        // padding length

static const char HEX[32] = "0123456789abcdef";  // array used for hex
                                                 // conversion

inline
static unsigned int F(unsigned int x, unsigned int y, unsigned int z)
{
    // A more optimized version of '(x & y) | ((~x) & z);', which was
    // specified in the RFC 1321 document.

    return z ^ (x & (y ^ z));
}

inline
static unsigned int G(unsigned int x, unsigned int y, unsigned int z)
{
    // A more optimized version of '(x & z) | (y & (~z));', which was
    // specified in the RFC 1321 document.

    return y ^ (z & (x ^ y));
}

inline
static unsigned int H(unsigned int x, unsigned int y, unsigned int z)
{
    return x ^ y ^ z;
}

inline
static unsigned int I(unsigned int x, unsigned int y, unsigned int z)
{
    return y ^ (x | (~z));
}

inline
static unsigned int rotateLeft(unsigned int x, unsigned int n)
{
    return (x << n) | (x >> (32 - n));
}

inline
static unsigned int FF(unsigned int a,
                       unsigned int b,
                       unsigned int c,
                       unsigned int d,
                       unsigned int x,
                       unsigned int s,
                       unsigned int ac)
{
    a += F(b, c, d);
    a += x;
    a += ac;
    return rotateLeft(a, s) + b;
}

inline
static unsigned int GG(unsigned int a,
                       unsigned int b,
                       unsigned int c,
                       unsigned int d,
                       unsigned int x,
                       unsigned int s,
                       unsigned int ac)
{
    a += G(b, c, d);
    a += x;
    a += ac;
    return rotateLeft(a, s) + b;
}

inline
static unsigned int HH(unsigned int a,
                       unsigned int b,
                       unsigned int c,
                       unsigned int d,
                       unsigned int x,
                       unsigned int s,
                       unsigned int ac)
{
    a += H(b, c, d);
    a += x;
    a += ac;
    return rotateLeft(a, s) + b;
}

inline
static unsigned int II(unsigned int a,
                       unsigned int b,
                       unsigned int c,
                       unsigned int d,
                       unsigned int x,
                       unsigned int s,
                       unsigned int ac)
{
    a += I(b, c, d);
    a += x;
    a += ac;
    return rotateLeft(a, s) + b;
}

inline
static unsigned int Decode(const unsigned char *input)
{
    return ((unsigned int)input[0])
        | (((unsigned int)input[1]) <<  8)
        | (((unsigned int)input[2]) << 16)
        | (((unsigned int)input[3]) << 24);
}

static void append(unsigned int *state, const unsigned char *data)
{
    register unsigned int a = state[0];
    register unsigned int b = state[1];
    register unsigned int c = state[2];
    register unsigned int d = state[3];

    unsigned int xArray[16];
    register unsigned int *const x = xArray;

    // Round 1
    //
    // The last parameter is the integer part of '2 ^ 32 * abs(sin(i))', where
    // 'i' starts from 1 in radians.  Furthermore, each 4-byte block is decoded
    // separately for efficiency.

    x[0]  = Decode(data);
    data += 4;
    a = FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
    x[1]  = Decode(data);
    data += 4;
    d = FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
    x[2]  = Decode(data);
    data += 4;
    c = FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
    x[3]  = Decode(data);
    data += 4;
    b = FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
    x[4]  = Decode(data);
    data += 4;
    a = FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
    x[5]  = Decode(data);
    data += 4;
    d = FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
    x[6]  = Decode(data);
    data += 4;
    c = FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
    x[7]  = Decode(data);
    data += 4;
    b = FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
    x[8] = Decode(data);
    data += 4;
    a = FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
    x[9]  = Decode(data);
    data += 4;
    d = FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
    x[10] = Decode(data);
    data += 4;
    c = FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
    x[11] = Decode(data);
    data += 4;
    b = FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
    x[12] = Decode(data);
    data += 4;
    a = FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
    x[13] = Decode(data);
    data += 4;
    d = FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
    x[14] = Decode(data);
    data += 4;
    c = FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
    x[15] = Decode(data);
    b = FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

    // Round 2

    a = GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
    d = GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
    c = GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
    b = GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
    a = GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
    d = GG (d, a, b, c, x[10], S22, 0x02441453); /* 22 */
    c = GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
    b = GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
    a = GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
    d = GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
    c = GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
    b = GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
    a = GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
    d = GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
    c = GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
    b = GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

    // Round 3

    a = HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
    d = HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
    c = HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
    b = HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
    a = HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
    d = HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
    c = HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
    b = HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
    a = HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
    d = HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
    c = HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
    b = HH (b, c, d, a, x[ 6], S34, 0x04881d05); /* 44 */
    a = HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
    d = HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
    c = HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
    b = HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

    // Round 4

    a = II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
    d = II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
    c = II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
    b = II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
    a = II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
    d = II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
    c = II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
    b = II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
    a = II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
    d = II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
    c = II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
    b = II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
    a = II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
    d = II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
    c = II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
    b = II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

    // Zeroing it out just in case the information is sensitive.

    bsl::memset(xArray, 0, sizeof(xArray));
}

static void padLengthToBuffer(unsigned int       *state,
                              unsigned char      *buffer,
                              bsls::Types::Int64  length)
{
    // The same as 'int inUse = length % BYTEBLOCKSIZE;'

    int inUse = length & 0x3f;
    int start = 0;
    const int PADLENGTHINDEX = BYTEBLOCKSIZE - BYTELENGTHPADSIZE;

    // The last 8 bytes are used for the length of the message.  If we don't
    // have enough space to fit the length of the message in, we should pad it
    // with 'u_md5Pad' first.

    if (PADLENGTHINDEX <= inUse) {
        int fill = BYTEBLOCKSIZE - inUse;
        bsl::memcpy(buffer + inUse, u_md5Pad, fill);
        append(state, buffer);
        start += fill;
        inUse = 0;
    }

    // Pad the remaining bytes.

    int copy = PADLENGTHINDEX - inUse;
    bsl::memcpy(buffer + inUse, u_md5Pad + start, copy);

    // Need bit length, so multiply by 8.

    length *= 8;

    {
        char *tmp = (char *)&length;

#if BSLS_PLATFORM_IS_BIG_ENDIAN
        // We want it in little-endian format, so reverse byte ordering.

        for (int i = 0; i < 8; ++i) {
            buffer[PADLENGTHINDEX + i] = tmp[7 - i];
        }
#else
        // If it is little-endian already, just copy through.

        for (int i = 0; i < 8; ++i) {
            buffer[PADLENGTHINDEX + i] = tmp[i];
        }
#endif
    }

}

static void populateResultBuffer(bdlde::Md5::Md5Digest *result,
                                 unsigned int         *state)
{
    char *resultBytes = result->buffer();

    // Again, endianness does not matter here because we're treating
    // 'd_state[i]' as a number instead of reading it byte by byte in memory.
    //  If we cast '&(d_state[i])' as a 'char*' and walk through it, then we
    // have to take care of whether it is big- or little-endian.

    for (int i = 0; i < 4; ++i) {
        resultBytes[    i * 4] = static_cast<char>( state[i]        & 0xff);
        resultBytes[1 + i * 4] = static_cast<char>((state[i] >>  8) & 0xff);
        resultBytes[2 + i * 4] = static_cast<char>((state[i] >> 16) & 0xff);
        resultBytes[3 + i * 4] = static_cast<char>((state[i] >> 24) & 0xff);
    }
}

static void initStates(unsigned int (*state)[4])
{
    // Initial states specified by the RFC 1321 document.

    const unsigned int INITSTATE0 = 0x67452301;
    const unsigned int INITSTATE1 = 0xefcdab89;
    const unsigned int INITSTATE2 = 0x98badcfe;
    const unsigned int INITSTATE3 = 0x10325476;

    (*state)[0] = INITSTATE0;
    (*state)[1] = INITSTATE1;
    (*state)[2] = INITSTATE2;
    (*state)[3] = INITSTATE3;
}

namespace bdlde {

                                 // ---------
                                 // class Md5
                                 // ---------

// CREATORS
Md5::Md5()
: d_length(0)
{
    initStates(&d_state);
}

Md5::Md5(const void *data, int length)
: d_length(0)
{
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(data || !length);

    initStates(&d_state);
    update(data, length);
}

Md5::Md5(const Md5& original)
: d_length(original.d_length)
{
    d_state[0] = original.d_state[0];
    d_state[1] = original.d_state[1];
    d_state[2] = original.d_state[2];
    d_state[3] = original.d_state[3];
    bsl::memcpy(d_buffer, original.d_buffer, sizeof(d_buffer));
}

Md5::~Md5()
{
    // Zeroing out the buffer just in case the information in the memory is
    // sensitive.

    bsl::memset(d_buffer, 0, sizeof(d_buffer));
}

// MANIPULATORS
void Md5::update(const void *data, int length)
{
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(data || !length);

    const unsigned char *input = (const unsigned char *)data;

    // The same as 'int inUse = d_length % BYTEBLOCKSIZE;'

    int inUse = d_length & 0x3f;
    int start = 0;

    if (inUse) {
        int space = BYTEBLOCKSIZE - inUse;
        if (space <= length) {
            bsl::memcpy(d_buffer + inUse, input, space);
            start    += space;
            length   -= space;
            d_length += space;
            append(d_state, d_buffer);
        }
        else {
            bsl::memcpy(d_buffer + inUse, input, length);
            d_length += length;
            return;                                                   // RETURN
        }
    }
    while (BYTEBLOCKSIZE <= length) {
        append(d_state, input + start);
        start    += BYTEBLOCKSIZE;
        length   -= BYTEBLOCKSIZE;
        d_length += BYTEBLOCKSIZE;
    }
    bsl::memcpy(d_buffer, input + start, length);
    d_length += length;
}

void Md5::loadDigestAndReset(Md5::Md5Digest *result)
{
    BSLS_ASSERT(result);

    // Before loading, complete padding by adding the length.

    padLengthToBuffer(d_state, d_buffer, d_length);

    // Append that to the states.

    append(d_state, d_buffer);

    // Populate the result.

    populateResultBuffer(result, d_state);

    reset();
}

void Md5::reset()
{
    initStates(&d_state);
    d_length = 0;
}

// ACCESSORS
void Md5::loadDigest(Md5::Md5Digest *result) const
{
    BSLS_ASSERT(result);

    // Save the states before proceeding by making a copy.  Cannot just save
    // the 'd_state[]' because this is a 'const' function.

    Md5 copy(*this);

    // Before loading, complete padding by adding the length.

    padLengthToBuffer(copy.d_state, copy.d_buffer, copy.d_length);

    // Append that to the states.

    append(copy.d_state, copy.d_buffer);

    // Populate the result.

    populateResultBuffer(result, copy.d_state);
}

bsl::ostream& Md5::print(bsl::ostream& stream) const
{
    Md5::Md5Digest result;
    loadDigest(&result);

    const char *resultBytes = result.buffer();

    char array[2 * sizeof result + 1];

    for (int i = 0; i < static_cast<int>(sizeof result); ++i) {
        array[i * 2]     = HEX[(resultBytes[i] >> 4) & 0xf];
        array[i * 2 + 1] = HEX[ resultBytes[i]       & 0xf];
    }
    array[2 * sizeof result] = '\0';

    stream << array;

    return stream;
}

// FREE OPERATORS
bool operator==(const Md5& lhs, const Md5& rhs)
{
    if (lhs.d_length != rhs.d_length
     || 0 != bsl::memcmp(lhs.d_state, rhs.d_state, sizeof lhs.d_state)) {
        return false;                                                 // RETURN
    }

    // Same as 'int inUse = lhs.d_length % BYTEBLOCKSIZE;'.

    int inUse = lhs.d_length & 0x3f;
    return 0 == bsl::memcmp(lhs.d_buffer,
                            rhs.d_buffer,
                            (sizeof *lhs.d_buffer) * inUse);
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
