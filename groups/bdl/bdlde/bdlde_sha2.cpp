// bdlde_sha2.cpp                                                     -*-C++-*-
#include "bdlde_sha2.h"

#include <bsl_algorithm.h>

namespace BloombergLP {
namespace bdlde {
namespace {

template<class INTEGER>
INTEGER rotateRight(INTEGER value, unsigned shift)
    // Rotate the specified 'value' by 'shift' bits to the right, filling in
    // bits open up from left with those that "fell off" from the right.  The
    // behavior is undefined unless 'shift <= sizeof(value) * CHAR_BIT'.
{
    return (value >> shift) | (value << ((sizeof(value) * CHAR_BIT) - shift));
}

template<class INTEGER>
INTEGER bitwiseConditional(INTEGER condition, INTEGER x, INTEGER y)
    // Returns a value that for each bit set in the specified 'condition' uses
    // the corresponding bit from the specified 'x', otherwise uses the
    // corresponding bit from the specified 'y'. This function is named 'Ch' in
    // FIPS 180-4.
{
    return (x & (y ^ condition)) ^ condition;
}

template<class INTEGER>
INTEGER bitwiseMajority(INTEGER x, INTEGER y, INTEGER z)
    // Returns a value that has each bit set if and only if the corresponding
    // bit is set in at least two out of three of the specified 'x', the
    // specified 'y', and the specified 'z'. This function is named 'Maj' in
    // FIPS 180-4.
{
    return (x & y) | ((x | y) & z);
}

bsl::uint32_t f1(bsl::uint32_t value)
    // First mixing function used by SHA-224 and SHA-256.  Mixes together the
    // bits of the specified 'value'.
{
    return rotateRight(value,  2)
         ^ rotateRight(value, 13)
         ^ rotateRight(value, 22);
}

bsl::uint64_t f1(bsl::uint64_t value)
    // First mixing function used by SHA-384 and SHA-512.  Mixes together the
    // bits of the specified 'value'.
{
    return rotateRight(value, 28)
         ^ rotateRight(value, 34)
         ^ rotateRight(value, 39);
}

bsl::uint32_t f2(bsl::uint32_t value)
    // Second mixing function used by SHA-224 and SHA-256.  Mixes together the
    // bits of the specified 'value'.
{
    return rotateRight(value,  6)
         ^ rotateRight(value, 11)
         ^ rotateRight(value, 25);
}

bsl::uint64_t f2(bsl::uint64_t value)
    // Second mixing function used by SHA-384 and SHA-512.  Mixes together the
    // bits of the specified 'value'.
{
    return rotateRight(value, 14)
         ^ rotateRight(value, 18)
         ^ rotateRight(value, 41);
}

bsl::uint32_t f3(bsl::uint32_t value)
    // Third mixing function used by SHA-224 and SHA-256.  Mixes together the
    // bits of the specified 'value'.
{
    return rotateRight(value,  7) ^ rotateRight(value, 18) ^ (value >>  3);
}

bsl::uint64_t f3(bsl::uint64_t value)
    // Third mixing function used by SHA-384 and SHA-512.  Mixes together the
    // bits of the specified 'value'.
{
    return rotateRight(value,  1) ^ rotateRight(value,  8) ^ (value >>  7);
}

bsl::uint32_t f4(bsl::uint32_t value)
    // Fourth mixing function used by SHA-224 and SHA-256.  Mixes together the
    // bits of the specified 'value'.
{
    return rotateRight(value, 17) ^ rotateRight(value, 19) ^ (value >> 10);
}

bsl::uint64_t f4(bsl::uint64_t value)
    // Fourth mixing function used by SHA-384 and SHA-512.  Mixes together the
    // bits of the specified 'value'.
{
    return rotateRight(value, 19) ^ rotateRight(value, 61) ^ (value >>  6);
}

template<class INTEGER>
INTEGER pack(const unsigned char *bytes)
    // Returns an integer that has the value that would be read from the
    // address indicated by the specified 'bytes' interpreted as a big-endian
    // integer of type 'INTEGER'.  The behavior is undefined unless
    // '[bytes, bytes + sizeof(INTEGER))' is a valid range.
{
    bsl::size_t shift = sizeof(INTEGER) * CHAR_BIT;
    INTEGER     x     = 0;
    for (const unsigned char *ptr = bytes;
         ptr != bytes + sizeof(INTEGER);
         ++ptr) {
        shift -= CHAR_BIT;
        x |= (static_cast<INTEGER>(*ptr) << shift);
    }
    return x;
}

template<class INTEGER>
void unpack(INTEGER value, unsigned char *bytes)
    // Stores the integer representation of the specified 'value' into the
    // address indicated by the specified 'bytes', interpreting 'value' as a
    // big-endian integer.  The behavior is undefined unless
    // '[bytes, bytes + sizeof(INTEGER))' is a valid range.
{
    bsl::size_t shift = sizeof(INTEGER) * 8;
    for (bsl::size_t index = 0; index != sizeof(INTEGER); ++index) {
        shift -= 8;
        bytes[index] = static_cast<bsl::uint8_t>(value >> shift);
    }
}

// First 32 bits of fractional part of the cube roots of the first 64 primes.
const bsl::uint32_t sha256Constants[64] =
            {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
             0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
             0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
             0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
             0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
             0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
             0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
             0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
             0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
             0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
             0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
             0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
             0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
             0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
             0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
             0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

// First 64 bits of fractional part of the cube roots of the first 80 primes.
const bsl::uint64_t sha512Constants[80] =
            {0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL,
             0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
             0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
             0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
             0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
             0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
             0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL,
             0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
             0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
             0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
             0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL,
             0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
             0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL,
             0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
             0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
             0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
             0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL,
             0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
             0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL,
             0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
             0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
             0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
             0xd192e819d6ef5218ULL, 0xd69906245565a910ULL,
             0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
             0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
             0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
             0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
             0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
             0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL,
             0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
             0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL,
             0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
             0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
             0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
             0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
             0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
             0x28db77f523047d84ULL, 0x32caab7b40c72493ULL,
             0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
             0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
             0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL};

template<class INTEGER, bsl::size_t ARRAY_SIZE>
void transform(INTEGER             *state,
               const unsigned char *message,
               bsl::uint64_t        numberOfBuffers,
               bsl::uint64_t        bufferSize,
               const INTEGER      (&constants)[ARRAY_SIZE])
    // Update the specified 'state' with the hashed contents of the specified
    // 'message' having a length equal to the specified 'bufferSize' times the
    // specified 'numberOfBuffers', mixing it with the values in the specified
    // 'constants'.
{
    const unsigned char *messageEnd = message + bufferSize * numberOfBuffers;
    for (; message != messageEnd; message += bufferSize)
    {
        INTEGER w[ARRAY_SIZE];
        for (int index = 0; index != 16; ++index) {
            w[index] = pack<INTEGER>(message + index * sizeof(INTEGER));
        }
        for (int index = 16; index != ARRAY_SIZE; ++index) {
            w[index] = f4(w[index -  2])
                     +    w[index -  7]
                     + f3(w[index - 15])
                     +    w[index - 16];
        }

        INTEGER wv[8];
        bsl::copy(state, state + 8, wv);
        for (int index = 0; index != ARRAY_SIZE; ++index) {
            const INTEGER t1 = wv[7]
                             + f2(wv[4])
                             + bitwiseConditional(wv[6], wv[4], wv[5])
                             + constants[index]
                             + w[index];
            const INTEGER t2 = f1(wv[0])
                             + bitwiseMajority(wv[0], wv[1], wv[2]);
            wv[7] = wv[6];
            wv[6] = wv[5];
            wv[5] = wv[4];
            wv[4] = wv[3] + t1;
            wv[3] = wv[2];
            wv[2] = wv[1];
            wv[1] = wv[0];
            wv[0] = t1 + t2;
        }

        for (int index = 0; index < 8; ++index) {
            state[index] += wv[index];
        }
    }
}

template<bsl::size_t BUFFER_CAPACITY, class INTEGER, bsl::size_t ARRAY_SIZE>
void updateImpl(INTEGER             *state,
                bsl::uint64_t       *totalSize,
                bsl::uint64_t       *bufferSize,
                unsigned char      (&buffer)[BUFFER_CAPACITY],
                const unsigned char *message,
                bsl::size_t          messageSize,
                const INTEGER      (&constants)[ARRAY_SIZE])
    // Update the specified 'state' with the contents of the specified 'buffer'
    // followed by the contents of the specified 'message' having the specified
    // 'messageSize', mixed with the data in the specified 'constants'.  Update
    // the specified 'totalSize' to have the size, in bytes, of all messages
    // passed in so far.  Populate 'buffer' with all bytes leftover that did
    // not fit into a multiple of 'BUFFER_CAPACITY', and store into the
    // specified 'bufferSize' the count of the bytes in 'buffer' that are
    // currently in use.
{
    const bsl::uint64_t prologueSize = bsl::min(
                                       static_cast<bsl::uint64_t>(messageSize),
                                                BUFFER_CAPACITY - *bufferSize);
    bsl::copy(message,
              message + prologueSize,
              buffer + *bufferSize);
    *bufferSize += prologueSize;

    *totalSize += messageSize;
    if (*bufferSize != BUFFER_CAPACITY) {
        return;                                                       // RETURN
    }

    transform(state, buffer, 1, BUFFER_CAPACITY, constants);

    const unsigned char *remaining        = message + prologueSize;
    const bsl::uint64_t  remainingSize    = messageSize - prologueSize;
    const bsl::uint64_t  remainingBuffers = remainingSize / BUFFER_CAPACITY;
    transform(state, remaining, remainingBuffers, BUFFER_CAPACITY, constants);

    *bufferSize = remainingSize % BUFFER_CAPACITY;
    const unsigned char *epilogue = remaining
                                  + remainingBuffers
                                  * BUFFER_CAPACITY;
    bsl::copy(epilogue, epilogue + *bufferSize, buffer);
}

template<bsl::size_t BUFFER_CAPACITY, class INTEGER, bsl::size_t ARRAY_SIZE>
void finalize(unsigned char        *result,
              bsl::size_t           digestSize,
              INTEGER              *state,
              bsl::uint64_t         totalSize,
              bsl::uint64_t         bufferSize,
              const unsigned char (&buffer)[BUFFER_CAPACITY],
              const INTEGER       (&constants)[ARRAY_SIZE])
    // Mix the remaining contents of the specified 'buffer' as indicated by the
    // specified 'bufferSize' after appending the SHA-2 metadata, which
    // uses the specified 'totalSize', with the data in the specified
    // 'constants', and mix the result into the specified 'state'.  Store into
    // the specified 'result' having the specified 'digestSize' the contents of
    // 'state'.
{
    const bsl::uint64_t totalSizeInBits  = totalSize * 8;
    const bsl::uint64_t unpaddedSize     = bufferSize
                                         + 1
                                         + sizeof(INTEGER) * 2;
    const bsl::uint64_t remainingBuffers =
                                     (unpaddedSize <= BUFFER_CAPACITY) ? 1 : 2;
    // At the end of the message, we write a special marker byte, followed by
    // the total size of the message.  Insert '0' bytes between those to pad
    // the message to a multiple of BUFFER_CAPACITY.
    unsigned char       finalBuffers[BUFFER_CAPACITY * 2] = {};
    bsl::copy(buffer, buffer + bufferSize, finalBuffers);
    finalBuffers[bufferSize] = 1 << 7;
    unsigned char *end = finalBuffers + remainingBuffers * BUFFER_CAPACITY;
    unpack(totalSizeInBits, end - sizeof(totalSizeInBits));
    transform(state,
              finalBuffers,
              remainingBuffers,
              BUFFER_CAPACITY,
              constants);

    for (unsigned index = 0 ; index < digestSize / sizeof(INTEGER); ++index) {
        unpack(state[index], &result[index * sizeof(INTEGER)]);
    }
}

template<bsl::size_t SIZE>
void toHex(char *output, const unsigned char (&input)[SIZE])
    // Store into the specified 'output' the hex representation of the bytes in
    // the specified 'input'.
{
    const char *hexTable = "0123456789abcdef";
    for (bsl::size_t index = 0; index != SIZE; ++index) {
        const unsigned char byte = input[index];
        output[index * 2]     = hexTable[byte / 16];
        output[index * 2 + 1] = hexTable[byte % 16];
    }
}

} // close unnamed namespace

Sha224::Sha224()
{
    reset();
}

Sha224::Sha224(const void *data, bsl::size_t length)
{
    reset();
    update(data, length);
}

Sha256::Sha256()
{
    reset();
}

Sha256::Sha256(const void *data, bsl::size_t length)
{
    reset();
    update(data, length);
}

Sha384::Sha384()
{
    reset();
}

Sha384::Sha384(const void *data, bsl::size_t length)
{
    reset();
    update(data, length);
}

Sha512::Sha512()
{
    reset();
}

Sha512::Sha512(const void *data, bsl::size_t length)
{
    reset();
    update(data, length);
}

void Sha224::reset()
{
    d_totalSize = 0;
    d_bufferSize = 0;
    // Second 32 bits of the fractional parts of the square root of the 9th
    // through 16th primes.
    d_state[0] = 0xc1059ed8;
    d_state[1] = 0x367cd507;
    d_state[2] = 0x3070dd17;
    d_state[3] = 0xf70e5939;
    d_state[4] = 0xffc00b31;
    d_state[5] = 0x68581511;
    d_state[6] = 0x64f98fa7;
    d_state[7] = 0xbefa4fa4;
}

void Sha256::reset()
{
    d_totalSize = 0;
    d_bufferSize = 0;
    // First 32 bits of the fractional part of the square root of the first 8
    // primes.
    d_state[0] = 0x6a09e667;
    d_state[1] = 0xbb67ae85;
    d_state[2] = 0x3c6ef372;
    d_state[3] = 0xa54ff53a;
    d_state[4] = 0x510e527f;
    d_state[5] = 0x9b05688c;
    d_state[6] = 0x1f83d9ab;
    d_state[7] = 0x5be0cd19;
}

void Sha384::reset()
{
    d_totalSize = 0;
    d_bufferSize = 0;
    // First 64 bits of the fractional parts of the square root of the 9th
    // through 16th primes.
    d_state[0] = 0xcbbb9d5dc1059ed8ULL;
    d_state[1] = 0x629a292a367cd507ULL;
    d_state[2] = 0x9159015a3070dd17ULL;
    d_state[3] = 0x152fecd8f70e5939ULL;
    d_state[4] = 0x67332667ffc00b31ULL;
    d_state[5] = 0x8eb44a8768581511ULL;
    d_state[6] = 0xdb0c2e0d64f98fa7ULL;
    d_state[7] = 0x47b5481dbefa4fa4ULL;
}

void Sha512::reset()
{
    d_totalSize = 0;
    d_bufferSize = 0;
    // First 64 bits of the fractional part of the square root of the first 8
    // primes.
    d_state[0] = 0x6a09e667f3bcc908ULL;
    d_state[1] = 0xbb67ae8584caa73bULL;
    d_state[2] = 0x3c6ef372fe94f82bULL;
    d_state[3] = 0xa54ff53a5f1d36f1ULL;
    d_state[4] = 0x510e527fade682d1ULL;
    d_state[5] = 0x9b05688c2b3e6c1fULL;
    d_state[6] = 0x1f83d9abfb41bd6bULL;
    d_state[7] = 0x5be0cd19137e2179ULL;
}

void Sha224::update(const void *message, bsl::size_t length)
{
    updateImpl( d_state,
               &d_totalSize,
               &d_bufferSize,
                d_buffer,
                static_cast<const unsigned char *>(message),
                length,
                sha256Constants);
}

void Sha256::update(const void *message, bsl::size_t length)
{
    updateImpl( d_state,
               &d_totalSize,
               &d_bufferSize,
                d_buffer,
                static_cast<const unsigned char *>(message),
                length,
                sha256Constants);
}

void Sha384::update(const void *message, bsl::size_t length)
{
    updateImpl( d_state,
               &d_totalSize,
               &d_bufferSize,
                d_buffer,
                static_cast<const unsigned char *>(message),
                length,
                sha512Constants);
}

void Sha512::update(const void *message, bsl::size_t length)
{
    updateImpl( d_state,
               &d_totalSize,
               &d_bufferSize,
                d_buffer,
                static_cast<const unsigned char *>(message),
                length,
                sha512Constants);
}

void Sha224::loadDigest(unsigned char *result) const
{
    bsl::uint32_t outputState[8];
    bsl::copy(d_state, d_state + 8, outputState);
    finalize(result,
             k_DIGEST_SIZE,
             outputState,
             d_totalSize,
             d_bufferSize,
             d_buffer,
             sha256Constants);
}

void Sha256::loadDigest(unsigned char *result) const
{
    bsl::uint32_t outputState[8];
    bsl::copy(d_state, d_state + 8, outputState);
    finalize(result,
             k_DIGEST_SIZE,
             outputState,
             d_totalSize,
             d_bufferSize,
             d_buffer,
             sha256Constants);
}

void Sha384::loadDigest(unsigned char *result) const
{
    bsl::uint64_t outputState[8];
    bsl::copy(d_state, d_state + 8, outputState);
    finalize(result,
             k_DIGEST_SIZE,
             outputState,
             d_totalSize,
             d_bufferSize,
             d_buffer,
             sha512Constants);
}

void Sha512::loadDigest(unsigned char *result) const
{
    bsl::uint64_t outputState[8];
    bsl::copy(d_state, d_state + 8, outputState);
    finalize(result,
             k_DIGEST_SIZE,
             outputState,
             d_totalSize,
             d_bufferSize,
             d_buffer,
             sha512Constants);
}

void Sha224::loadDigestAndReset(unsigned char *result)
{
    finalize(result,
             k_DIGEST_SIZE,
             d_state,
             d_totalSize,
             d_bufferSize,
             d_buffer,
             sha256Constants);
    reset();
}

void Sha256::loadDigestAndReset(unsigned char *result)
{
    finalize(result,
             k_DIGEST_SIZE,
             d_state,
             d_totalSize,
             d_bufferSize,
             d_buffer,
             sha256Constants);
    reset();
}

void Sha384::loadDigestAndReset(unsigned char *result)
{
    finalize(result,
             k_DIGEST_SIZE,
             d_state,
             d_totalSize,
             d_bufferSize,
             d_buffer,
             sha512Constants);
    reset();
}

void Sha512::loadDigestAndReset(unsigned char *result)
{
    finalize(result,
             k_DIGEST_SIZE,
             d_state,
             d_totalSize,
             d_bufferSize,
             d_buffer,
             sha512Constants);
    reset();
}

bsl::ostream& Sha224::print(bsl::ostream& stream) const
{
    unsigned char result[k_DIGEST_SIZE];
    loadDigest(result);
    char output[2 * k_DIGEST_SIZE + 1];
    toHex(output, result);
    output[2 * k_DIGEST_SIZE] = '\0';

    stream << output;

    return stream;
}

bsl::ostream& Sha256::print(bsl::ostream& stream) const
{
    unsigned char result[k_DIGEST_SIZE];
    loadDigest(result);
    char output[2 * k_DIGEST_SIZE + 1];
    toHex(output, result);
    output[2 * k_DIGEST_SIZE] = '\0';

    stream << output;

    return stream;
}

bsl::ostream& Sha384::print(bsl::ostream& stream) const
{
    unsigned char result[k_DIGEST_SIZE];
    loadDigest(result);
    char output[2 * k_DIGEST_SIZE + 1];
    toHex(output, result);
    output[2 * k_DIGEST_SIZE] = '\0';

    stream << output;

    return stream;
}

bsl::ostream& Sha512::print(bsl::ostream& stream) const
{
    unsigned char result[k_DIGEST_SIZE];
    loadDigest(result);
    char output[2 * k_DIGEST_SIZE + 1];
    toHex(output, result);
    output[2 * k_DIGEST_SIZE] = '\0';

    stream << output;

    return stream;
}

// FREE OPERATORS
bool operator==(const Sha224& lhs, const Sha224& rhs)
{
    return lhs.d_totalSize  == rhs.d_totalSize
        && lhs.d_bufferSize == rhs.d_bufferSize
        && bsl::equal(lhs.d_buffer,
                      lhs.d_buffer + lhs.d_bufferSize,
                      rhs.d_buffer)
        && bsl::equal(lhs.d_state, lhs.d_state + 8, rhs.d_state);
}

bool operator==(const Sha256& lhs, const Sha256& rhs)
{
    return lhs.d_totalSize  == rhs.d_totalSize
        && lhs.d_bufferSize == rhs.d_bufferSize
        && bsl::equal(lhs.d_buffer,
                      lhs.d_buffer + lhs.d_bufferSize,
                      rhs.d_buffer)
        && bsl::equal(lhs.d_state, lhs.d_state + 8, rhs.d_state);
}

bool operator==(const Sha384& lhs, const Sha384& rhs)
{
    return lhs.d_totalSize  == rhs.d_totalSize
        && lhs.d_bufferSize == rhs.d_bufferSize
        && bsl::equal(lhs.d_buffer,
                      lhs.d_buffer + lhs.d_bufferSize,
                      rhs.d_buffer)
        && bsl::equal(lhs.d_state, lhs.d_state + 8, rhs.d_state);
}

bool operator==(const Sha512& lhs, const Sha512& rhs)
{
    return lhs.d_totalSize  == rhs.d_totalSize
        && lhs.d_bufferSize == rhs.d_bufferSize
        && bsl::equal(lhs.d_buffer,
                      lhs.d_buffer + lhs.d_bufferSize,
                      rhs.d_buffer)
        && bsl::equal(lhs.d_state, lhs.d_state + 8, rhs.d_state);
}

} // close package namespace
} // close enterprise namespace