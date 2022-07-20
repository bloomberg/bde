// bdlde_sha1.cpp                                                     -*-C++-*-
#include <bdlde_sha1.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlde_sha1_cpp,"$Id$ $CSID$")

#include <bsl_algorithm.h>
#include <bsl_climits.h>
#include <bsl_iterator.h>
#include <bsl_ostream.h>
#include <bsl_type_traits.h>
#include <bslmf_assert.h>

namespace BloombergLP {
namespace bdlde {
namespace {

typedef bsl::uint32_t Sha1Word;
    // Alias for the word type of the SHA-1 algorithm.  Note that this must
    // match 'Sha1::Word'.

typedef Sha1Word      Sha1State[5];
    // Alias for the internal state type of the SHA-1 algorithm.  Note that
    // this must match 'Sha1::State'.

const bsl::size_t k_SHA1_BLOCK_SIZE = 512 / 8;
    // Size (in bytes) of the blocks into which messages are divided before
    // being ingested into the SHA-1 state.  Note that this must match
    // 'Sha1::k_BLOCK_SIZE'.

const Sha1Word k_SHA1_CONSTANTS[80] = {
    // The SHA-1 constants defined in section 4.2.1 of FIPS 180-4.
    0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999,
    0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999,
    0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999,
    0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999,
    0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1,
    0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1,
    0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1,
    0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1,
    0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc,
    0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc,
    0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc,
    0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc,
    0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6,
    0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6,
    0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6,
    0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6,
};

static Sha1Word rotateLeft(Sha1Word value, int shift)
    // Return the result of a bitwise rotation on the specified 'value' by the
    // specified 'shift' number of bits.  The behavior is undefined unless
    // 'shift' is positive and strictly less than 32.
{
    return (value << shift) | (value >> ((sizeof(value) * CHAR_BIT) - shift));
}

static Sha1Word bitwiseConditional(Sha1Word condition, Sha1Word x, Sha1Word y)
    // Return a value that for each bit set in the specified 'condition' uses
    // the corresponding bit from the specified 'x', otherwise uses the
    // corresponding bit from the specified 'y'.  This function is named 'Ch'
    // in FIPS 180-4.
{
    // The following implementation, taken from bdlde_sha2.cpp, only uses 3
    // operations, compared with the more straightforward one, which uses 4:
    // ..
    //   return (condition & x) ^ (~condition & y);
    // ..
    return (condition & (x ^ y)) ^ y;
}

static Sha1Word bitwiseMajority(Sha1Word x, Sha1Word y, Sha1Word z)
    // Return a value that has each bit set if and only if the corresponding
    // bit is set in at least two out of three of the specified 'x', 'y', and
    // 'z'.  This function is named 'Maj' in FIPS 180-4.
{
    return (x & y) | ((x | y) & z);
}

static Sha1Word f(Sha1Word x, Sha1Word y, Sha1Word z, int index)
    // The SHA-1 mixing function with the specified 'index' having the
    // specified 'x', 'y', and 'z' as arguments, as defined in section 4.1.1 of
    // FIPS 180-4.  The behavior is undefined unless 'index' is nonnegative and
    // less than or equal to 79.
{
    if (index <= 19) {
        return bitwiseConditional(x, y, z);                           // RETURN
    } else if (index >= 40 && index <= 59) {
        return bitwiseMajority(x, y, z);                              // RETURN
    } else {
        return x ^ y ^ z;                                             // RETURN
    }
}

static Sha1Word pack(const unsigned char *bytes)
    // Return an integer that has the value that would be read from the address
    // indicated by the specified 'bytes' interpreted as a big-endian integer
    // of type 'Word'.  The behavior is undefined unless
    // '[bytes, bytes + sizeof(INTEGER))' is a valid range.
{
    bsl::size_t shift = sizeof(Sha1Word) * CHAR_BIT;
    Sha1Word    x     = 0;
    for (const unsigned char *ptr = bytes;
         ptr != bytes + sizeof(Sha1Word);
         ++ptr) {
        shift -= CHAR_BIT;
        x |= (static_cast<Sha1Word>(*ptr) << shift);
    }
    return x;
}

template <class INTEGER>
static void unpack(unsigned char *bytes, INTEGER value)
    // Stores the integer representation of the specified 'value' into the
    // address indicated by the specified 'bytes', interpreting 'value' as a
    // big-endian integer.  The behavior is undefined unless
    // '[bytes, bytes + sizeof(INTEGER))' is a valid range.
{
    bsl::size_t shift = sizeof(INTEGER) * CHAR_BIT;
    for (bsl::size_t index = 0; index != sizeof(INTEGER); ++index) {
        shift -= CHAR_BIT;
        bytes[index] = static_cast<bsl::uint8_t>(value >> shift);
    }
}

void unpackArray(unsigned char *result, const Sha1State& state)
    // Store into the specified 'result' the 20 bytes whose values are the
    // big-endian representation of the specified 'state'.
{
    for (bsl::size_t index = 0; index < bsl::size(state); ++index)
    {
        unpack(result + (index * sizeof(Sha1Word)), state[index]);
    }
}

void toHex(char *output, const unsigned char (&input)[Sha1::k_DIGEST_SIZE])
    // Store into the specified 'output' the hex representation of the bytes in
    // the specified 'input'.
{
    const char *hexTable = "0123456789abcdef";
    for (bsl::size_t index = 0; index != Sha1::k_DIGEST_SIZE; ++index) {
        const unsigned char byte = input[index];
        output[index * 2]     = hexTable[byte / 16];
        output[index * 2 + 1] = hexTable[byte % 16];
    }
}

void transform(Sha1State           *state,
               const unsigned char *message,
               bsl::uint64_t        numMessageBlocks)
    // Update the specified 'state' with the hashed contents of the specified
    // 'message' having a length equal to the specified 'numMessageBlocks'
    // times 'k_SHA1_BLOCK_SIZE'.
{
    const unsigned char *messageEnd =
        message + k_SHA1_BLOCK_SIZE * numMessageBlocks;
    for (; message != messageEnd; message += k_SHA1_BLOCK_SIZE)
    {
        // Prepare the message schedule
        Sha1Word w[80];
        for (int index = 0; index != 16; ++index) {
            w[index] = pack(message + index * sizeof(Sha1Word));
        }
        for (int index = 16; index != 80; ++index) {
            const Sha1Word x =
                w[index - 3] ^ w[index - 8] ^ w[index - 14] ^ w[index - 16];
            w[index] = rotateLeft(x, 1);
        }

        // Initialize the working variables
        Sha1Word wv[5];
        bsl::copy(bsl::begin(*state), bsl::end(*state), bsl::begin(wv));

        // Mix the message data into the working variables
        for (int index = 0; index != 80; ++index) {
            const Sha1Word t = rotateLeft(wv[0], 5) +
                               f(wv[1], wv[2], wv[3], index) + wv[4] +
                               k_SHA1_CONSTANTS[index] + w[index];
            wv[4] = wv[3];
            wv[3] = wv[2];
            wv[2] = rotateLeft(wv[1], 30);
            wv[1] = wv[0];
            wv[0] = t;
        }

        // Update the state
        for (bsl::size_t index = 0; index < bsl::size(*state); ++index) {
            (*state)[index] += wv[index];
        }
    }
}

void updateImpl(Sha1State           *state,
                bsl::uint64_t       *totalSize,
                bsl::uint64_t       *bufferSize,
                unsigned char      (*buffer)[k_SHA1_BLOCK_SIZE],
                const unsigned char *message,
                bsl::size_t          messageSize)
    // Update the specified 'state' with the contents of the specified 'buffer'
    // followed by the contents of the specified 'message' having the specified
    // 'messageSize' in bytes.  Update the specified 'totalSize' to have the
    // size, in bytes, of all messages passed in so far.  Populate 'buffer
    // with all bytes left over that did not fit into a multiple of
    // 'k_BLOCK_SIZE', and store into the specified 'bufferSize' the count of
    // the bytes in 'buffer' that are currently in use.
{
    const bsl::uint64_t prologueSize =
        bsl::min(static_cast<bsl::uint64_t>(messageSize),
                 k_SHA1_BLOCK_SIZE - *bufferSize);
    bsl::copy(message,
              message + prologueSize,
              *buffer + *bufferSize);
    *bufferSize += prologueSize;

    *totalSize += messageSize;
    if (*bufferSize != k_SHA1_BLOCK_SIZE) {
        return;                                                       // RETURN
    }

    transform(state, *buffer, 1);

    const unsigned char *remaining       = message + prologueSize;
    const bsl::uint64_t  remainingSize   = messageSize - prologueSize;
    const bsl::uint64_t  remainingBlocks = remainingSize / k_SHA1_BLOCK_SIZE;
    transform(state, remaining, remainingBlocks);

    *bufferSize = remainingSize % k_SHA1_BLOCK_SIZE;
    const unsigned char *epilogue = remaining
                                  + remainingBlocks
                                  * k_SHA1_BLOCK_SIZE;
    bsl::copy(epilogue, epilogue + *bufferSize, *buffer);
}

void finalize(Sha1State            *state,
              bsl::uint64_t         totalSize,
              bsl::uint64_t         bufferSize,
              const unsigned char (&buffer)[k_SHA1_BLOCK_SIZE])
    // Mix into the specified 'state' the remaining contents of the specified
    // 'buffer' as indicated by the specified 'bufferSize' followed by a
    // sequence of padding bytes computed from the specified 'totalSize' using
    // the formula specified by section 5.1.1 of FIPS 180-4.
{
    const bsl::uint64_t totalSizeInBits = totalSize * 8;
    const bsl::uint64_t unpaddedSize = bufferSize + 1 + sizeof(bsl::uint64_t);
    const bsl::uint64_t remainingBlocks =
        (unpaddedSize <= k_SHA1_BLOCK_SIZE) ? 1 : 2;

    // At the end of the message, we write a special marker byte, followed by
    // the total size of the message.  Insert '0' bytes between those to pad
    // the message to a multiple of k_SHA1_BLOCK_SIZE.
    unsigned char finalBlocks[k_SHA1_BLOCK_SIZE * 2] = {};
    bsl::copy(buffer, buffer + bufferSize, finalBlocks);
    finalBlocks[bufferSize] = 1 << 7;
    unsigned char *end = finalBlocks + remainingBlocks * k_SHA1_BLOCK_SIZE;
    unpack(end - sizeof(totalSizeInBits), totalSizeInBits);
    transform(state, finalBlocks, remainingBlocks);
}

}  // close unnamed namespace

                                 // ----------
                                 // class Sha1
                                 // ----------

// CLASS DATA
const bsl::size_t Sha1::k_BLOCK_SIZE;
const bsl::size_t Sha1::k_DIGEST_SIZE;

// CREATORS
Sha1::Sha1()
{
    reset();
}

Sha1::Sha1(const void *data, bsl::size_t length)
{
    reset();
    update(data, length);
}

// MANIPULATORS
void Sha1::loadDigestAndReset(unsigned char *result)
{
    // Ensure that the local typedefs and constants are the same as the
    // corresponding members of 'Sha1'; otherwise 'finalize' (which uses the
    // former) will not function correctly.
    BSLMF_ASSERT((bsl::is_same<Word, Sha1Word>::value));
    BSLMF_ASSERT((bsl::is_same<State, Sha1State>::value));
    BSLMF_ASSERT(k_BLOCK_SIZE == k_SHA1_BLOCK_SIZE);

    finalize(&d_state,
              d_totalSize,
              d_bufferSize,
              d_buffer);
    unpackArray(result, d_state);
    reset();
}

void Sha1::reset()
{
    d_totalSize = 0;
    d_bufferSize = 0;
    d_state[0] = 0x67452301;
    d_state[1] = 0xefcdab89;
    d_state[2] = 0x98badcfe;
    d_state[3] = 0x10325476;
    d_state[4] = 0xc3d2e1f0;
}

void Sha1::update(const void *data, bsl::size_t length)
{
    // Ensure that the local typedefs and constants are the same as the
    // corresponding members of 'Sha1'; otherwise 'updateImpl' (which uses the
    // former) will not function correctly.
    BSLMF_ASSERT((bsl::is_same<Word, Sha1Word>::value));
    BSLMF_ASSERT((bsl::is_same<State, Sha1State>::value));
    BSLMF_ASSERT(k_BLOCK_SIZE == k_SHA1_BLOCK_SIZE);

    updateImpl(&d_state,
               &d_totalSize,
               &d_bufferSize,
               &d_buffer,
                static_cast<const unsigned char *>(data),
                length);
}

// ACCESSORS
void Sha1::loadDigest(unsigned char *result) const
{
    // Ensure that the local typedefs and constants are the same as the
    // corresponding members of 'Sha1'; otherwise 'finalize' (which uses the
    // former) will not function correctly.
    BSLMF_ASSERT((bsl::is_same<Word, Sha1Word>::value));
    BSLMF_ASSERT((bsl::is_same<State, Sha1State>::value));
    BSLMF_ASSERT(k_BLOCK_SIZE == k_SHA1_BLOCK_SIZE);

    Word outputState[5];
    bsl::copy(bsl::begin(d_state), bsl::end(d_state), outputState);
    finalize(&outputState,
              d_totalSize,
              d_bufferSize,
              d_buffer);
    unpackArray(result, outputState);
}

bsl::ostream& Sha1::print(bsl::ostream& stream) const
{
    unsigned char result[k_DIGEST_SIZE];
    loadDigest(result);
    char output[2 * k_DIGEST_SIZE + 1];
    toHex(output, result);
    output[2 * k_DIGEST_SIZE] = '\0';

    stream << output;

    return stream;
}

}  // close package namespace

// FREE OPERATORS
bool bdlde::operator==(const Sha1& lhs, const Sha1& rhs)
{
    return lhs.d_totalSize == rhs.d_totalSize &&
           lhs.d_bufferSize == rhs.d_bufferSize &&
           bsl::equal(lhs.d_buffer,
                      lhs.d_buffer + lhs.d_bufferSize,
                      rhs.d_buffer) &&
           bsl::equal(bsl::begin(lhs.d_state),
                      bsl::end(lhs.d_state),
                      bsl::begin(rhs.d_state));
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
