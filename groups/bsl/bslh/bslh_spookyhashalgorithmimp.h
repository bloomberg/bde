// bslh_spookyhashalgorithmimp.h                                      -*-C++-*-
#ifndef INCLUDED_BSLH_SPOOKYHASHALGORITHMIMP
#define INCLUDED_BSLH_SPOOKYHASHALGORITHMIMP

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide BDE style wrapping around a spooky hash implementation.
//
//@CLASSES:
//  bslh::SpookyHashAlgorithmImp: 3rd party spooky hash algorithm wrapper
//
//@SEE_ALSO: bslh_hash, bslh_spookyhashalgorithm
//
//@DESCRIPTION: 'bslh::SpookyHashAlgorithmImp' provides BDE style wrapping
// around Bob Jenkins spooky hash implementation.  SpookyHash provides a way to
// hash contiguous data all at once, or discontiguous data in pieces.  More
// information is available at: http://burtleburtle.net/bob/hash/spooky.html
//
///Changes
///-------
// The third party code begins with the "SpookyHash" header below, and
// continues until the BloombergLP copyright notice.  Changes made to the
// original code include:
//
//: 1 Adding BloombergLP and bslh namespaces
//:
//: 2 Renamed 'SpookyHash' to 'SpookyHashAlgorithmImp'
//:
//: 3 Removed usage of 'stdint.h' and updated associated typedefs
//:
//: 4 Moved includes
//:
//: 5 Made some methods private
//:
//: 6 Re-wrote comments, indenting, etc in BDE style
//:
//: 7 Moved typedefs within class
//:
//: 8 Changed c style casts to static_casts
//:
//: 9 Reordered methods according to BDE style
//:
//: 10 Added inline to 'Hash32' and 'Hash64'
//
///Third Party Doc
///---------------
//
// SpookyHash: a 128-bit non cryptographic hash function
//
// By Bob Jenkins, public domain
//   Oct 31 2010: alpha, framework + SpookyHash::Mix appears right
//   Oct 31 2011: alpha again, Mix only good to 2^^69 but rest appears right
//   Dec 31 2011: beta, improved Mix, tested it for 2-bit deltas
//   Feb  2 2012: production, same bits as beta
//   Feb  5 2012: adjusted definitions of uint* to be more portable
//   Mar 30 2012: 3 bytes/cycle, not 4. Alpha was 4 but wasn't thorough enough.
//   August 5 2012: SpookyV2 (different results)
//
// Up to 3 bytes/cycle for long messages.  Reasonably fast for short messages.
// All 1 or 2 bit deltas achieve avalanche within 1% bias per output bit.
//
// This was developed for and tested on 64-bit x86-compatible processors.  It
// assumes the processor is little-endian.  There is a macro controlling
// whether unaligned reads are allowed (by default they are).  This should be
// an equally good hash on big-endian machines, but it will compute different
// results on them than on little-endian machines.
//
// Google's CityHash has similar specs to SpookyHash, and CityHash is faster on
// new Intel boxes.  MD4 and MD5 also have similar specs, but they are orders
// of magnitude slower.  CRCs are two or more times slower, but unlike
// SpookyHash, they have nice math for combining the CRCs of pieces to form the
// CRCs of wholes.  There are also cryptographic hashes, but those are even
// slower than MD5.
//

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_STDDEF
#include <stddef.h>    // 'size_t'
#endif

namespace BloombergLP {

namespace bslh {


class SpookyHashAlgorithmImp {
    // This class wraps an implementation of Bob Jenkin's "SpookyHash" in a
    // BDE-style component. For more information, see
    // http://burtleburtle.net/bob/hash/spooky.html .

  public:
    typedef  ::BloombergLP::bsls::Types::Uint64  Uint64;
    typedef  unsigned int                        Uint32;
    typedef  unsigned short                      Uint16;
    typedef  unsigned char                       Uint8;

  private:
    // DATA
    static const size_t sc_numVars = 12;
        // Number of 64-bit integers used in the internal state.

    static const size_t sc_blockSize = sc_numVars*8;
        // Size of the internal state, in bytes.

    static const size_t sc_bufSize = 2*sc_blockSize;
        // Size of buffer of unhashed data, in bytes.

    static const Uint64 sc_const = 0xdeadbeefdeadbeefLL;
        // A non-zero, odd, constant that has an irregular distribution of 1's
        // and 0's to be used in hashing calculations.

    Uint64 m_data[2*sc_numVars]; // Unhashed data, for partial messages
    Uint64 m_state[sc_numVars];  // Internal state of the hash
    size_t m_length;             // Total length of the input so far
    Uint8  m_remainder;          // Length of unhashed data stashed in m_data

    // PRIVATE CLASS METHODS
    static inline void End(
        const Uint64 *data,
        Uint64 &h0, Uint64 &h1, Uint64 &h2, Uint64 &h3,
        Uint64 &h4, Uint64 &h5, Uint64 &h6, Uint64 &h7,
        Uint64 &h8, Uint64 &h9, Uint64 &h10,Uint64 &h11);
        // Incorporate the first 12 bytes of the specified 'data' into 'h0',
        // 'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'h7', 'h8', 'h9', 'h10', and
        // 'h11', and then mix the inputs together so that 'h0' and 'h1' are a
        // hash of all the inputs.

    static inline void EndPartial(
        Uint64 &h0, Uint64 &h1, Uint64 &h2, Uint64 &h3,
        Uint64 &h4, Uint64 &h5, Uint64 &h6, Uint64 &h7,
        Uint64 &h8, Uint64 &h9, Uint64 &h10,Uint64 &h11);
        // Combine the specified 'h0', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6',
        // 'h7', 'h8', 'h9', 'h10', and 'h11' together so that 'h0' and 'h1'
        // will be a hash of all the inputs.

    static inline void Mix(const Uint64 *data, Uint64 &s0, Uint64 &s1,
                           Uint64 &s2,  Uint64 &s3, Uint64 &s4, Uint64 &s5,
                           Uint64 &s6,  Uint64 &s7, Uint64 &s8, Uint64 &s9,
                           Uint64 &s10, Uint64 &s11);
        // Thoroughly mix the first 12 bytes of the specified 'data' into 's0',
        // 's1', 's2', 's3', 's4', 's5', 's6', 's7', 's8', 's9', 's10', and
        // 's1'. This method should be used when the input is 96 bytes or
        // longer to prevent the loss of entropy, because the internal state of
        // 'SpookyHashAlgorithmImp' is overwritten every 96 bytes.

    static inline Uint64 Rot64(Uint64 x, int k);
        // Return the specified 'x' left rotated by 'k' bits.

    static void Short(const void *message,
                      size_t length,
                      Uint64 *hash1,
                      Uint64 *hash2);
        // Hash the specified 'length' bytes of 'message' using 'hash1' and
        // 'hash2' as seeds. Load the higher order bits of the resulting
        // 128-bit into 'hash1' and the lower order bits in 'hash2'. This
        // method is meant to be used for messages less than 192 bytes in
        // length because of it's lower startup cost.

    static inline void ShortEnd(Uint64 &h0,
                                Uint64 &h1,
                                Uint64 &h2,
                                Uint64 &h3);
        // Combine the specified 'h0', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6',
        // 'h7', 'h8', 'h9', 'h10', and 'h11' together so that 'h0' and 'h1'
        // will be a hash of all the inputs.

    static inline void ShortMix(Uint64 &h0,
                                Uint64 &h1,
                                Uint64 &h2,
                                Uint64 &h3);
        // Thoroughly mix the specified 'h0', 'h1', 'h2', and 'h3' so that each
        // bit of input contributes entropy to every bit of the final states of
        // 'h0', 'h1', 'h2', and 'h3'.

  public:
    // PUBLIC CLASS METHODS
    static inline Uint32 Hash32(const void *message,
                                size_t length,
                                Uint32 seed);
        // Hash the specified 'length' bytes of 'message' using 'seed' as a
        // seed. Return the resulting 32-bit hash.

    static inline Uint64 Hash64(const void *message,
                                size_t length,
                                Uint64 seed);
        // Hash the specified 'length' bytes of 'message' using 'seed' as a
        // seed. Return the resulting 64-bit hash.

    static void Hash128(const void *message,
                        size_t length,
                        Uint64 *hash1,
                        Uint64 *hash2);
        // Hash the specified 'length' bytes of 'message' using 'hash1' and
        // 'hash2' as seeds. Load the higher order bits of the resulting
        // 128-bit into 'hash1' and the lower order bits in 'hash2'.

    // CREATORS
    //! SpookyHashAlgorithmImp() = default;
        // Create a 'SpookyHashAlgorithmImp', leaving it uninitialized.

    //! SpookyHashAlgorithmImp(const SpookyHashAlgorithmImp& original) =
    //!                                                                default;
        // Create a 'SpookyHashAlgorithmImp' object having the same internal
        // state as the specified 'original'.

    //! ~SpookyHashAlgorithmImp() = default;
        // Destroy this object.

    // MANIPULATORS
    //! SpookyHashAlgorithmImp& operator=(const SpookyHashAlgorithmImp& rhs) =
    //!                                                                default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void Init(Uint64 seed1, Uint64 seed2);
        // Initialize the internal state of the object using the specified
        // 'seed1' and 'seed2' as seeds for the algorithm. This method must be
        // called before calling 'Update' or 'Final' Note that this does not
        // need to be called to use the static 'HashXX' methods.

    void Update(const void *message, size_t length);
        // Accumulate the specified 'length' bytes of 'message' into the
        // internal state of the algorithm. Accumulating bytes through 'Update'
        // will produce the same result as hashing them all at once through the
        // 'HashXX' static methods.

    //ACCESSORS
    void Final(Uint64 *hash1, Uint64 *hash2);
        // Load the finalized hash into the specified 'hash1' and 'hash2'.
        // 'hash1' will contain the higher order bits of the hash and 'hash2'
        // will contain the lower order bits. The internal state of the
        // algorithm will not be modified, meaning 'Final' can be called
        // multiple times. The returned hash will be the same as if 'Hash128'
        // had been called will all of the accumulated data in one block.
};

// PUBLIC CLASS METHODS
inline
SpookyHashAlgorithmImp::Uint32 SpookyHashAlgorithmImp::Hash32(
                                                           const void *message,
                                                           size_t      length,
                                                           Uint32      seed)
{
    Uint64 hash1 = seed, hash2 = seed;
    Hash128(message, length, &hash1, &hash2);
    return static_cast<Uint32>(hash1);
}

inline
SpookyHashAlgorithmImp::Uint64 SpookyHashAlgorithmImp::Hash64(
                                                           const void *message,
                                                           size_t      length,
                                                           Uint64      seed)
{
    Uint64 hash1 = seed;
    Hash128(message, length, &hash1, &seed);
    return hash1;
}

// PRIVATE CLASS METHODS
inline
void SpookyHashAlgorithmImp::End(
    const Uint64 *data,
    Uint64 &h0, Uint64 &h1, Uint64 &h2, Uint64 &h3,
    Uint64 &h4, Uint64 &h5, Uint64 &h6, Uint64 &h7,
    Uint64 &h8, Uint64 &h9, Uint64 &h10,Uint64 &h11)
{
    h0 += data[0];   h1 += data[1];   h2 += data[2];   h3 += data[3];
    h4 += data[4];   h5 += data[5];   h6 += data[6];   h7 += data[7];
    h8 += data[8];   h9 += data[9];   h10 += data[10]; h11 += data[11];
    EndPartial(h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
    EndPartial(h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
    EndPartial(h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
}

inline
void SpookyHashAlgorithmImp::EndPartial(
    Uint64 &h0, Uint64 &h1, Uint64 &h2, Uint64 &h3,
    Uint64 &h4, Uint64 &h5, Uint64 &h6, Uint64 &h7,
    Uint64 &h8, Uint64 &h9, Uint64 &h10,Uint64 &h11)
{
    h11+= h1;    h2 ^= h11;   h1 = Rot64(h1,44);
    h0 += h2;    h3 ^= h0;    h2 = Rot64(h2,15);
    h1 += h3;    h4 ^= h1;    h3 = Rot64(h3,34);
    h2 += h4;    h5 ^= h2;    h4 = Rot64(h4,21);
    h3 += h5;    h6 ^= h3;    h5 = Rot64(h5,38);
    h4 += h6;    h7 ^= h4;    h6 = Rot64(h6,33);
    h5 += h7;    h8 ^= h5;    h7 = Rot64(h7,10);
    h6 += h8;    h9 ^= h6;    h8 = Rot64(h8,13);
    h7 += h9;    h10^= h7;    h9 = Rot64(h9,38);
    h8 += h10;   h11^= h8;    h10= Rot64(h10,53);
    h9 += h11;   h0 ^= h9;    h11= Rot64(h11,42);
    h10+= h0;    h1 ^= h10;   h0 = Rot64(h0,54);
}

inline
void SpookyHashAlgorithmImp::Mix(
    const Uint64 *data,
    Uint64 &s0, Uint64 &s1, Uint64 &s2, Uint64 &s3,
    Uint64 &s4, Uint64 &s5, Uint64 &s6, Uint64 &s7,
    Uint64 &s8, Uint64 &s9, Uint64 &s10,Uint64 &s11)
{
    s0 += data[0];   s2  ^= s10; s11 ^= s0;  s0  = Rot64(s0,11);  s11 += s1;
    s1 += data[1];   s3  ^= s11; s0  ^= s1;  s1  = Rot64(s1,32);  s0  += s2;
    s2 += data[2];   s4  ^= s0;  s1  ^= s2;  s2  = Rot64(s2,43);  s1  += s3;
    s3 += data[3];   s5  ^= s1;  s2  ^= s3;  s3  = Rot64(s3,31);  s2  += s4;
    s4 += data[4];   s6  ^= s2;  s3  ^= s4;  s4  = Rot64(s4,17);  s3  += s5;
    s5 += data[5];   s7  ^= s3;  s4  ^= s5;  s5  = Rot64(s5,28);  s4  += s6;
    s6 += data[6];   s8  ^= s4;  s5  ^= s6;  s6  = Rot64(s6,39);  s5  += s7;
    s7 += data[7];   s9  ^= s5;  s6  ^= s7;  s7  = Rot64(s7,57);  s6  += s8;
    s8 += data[8];   s10 ^= s6;  s7  ^= s8;  s8  = Rot64(s8,55);  s7  += s9;
    s9 += data[9];   s11 ^= s7;  s8  ^= s9;  s9  = Rot64(s9,54);  s8  += s10;
    s10 += data[10]; s0  ^= s8;  s9  ^= s10; s10 = Rot64(s10,22); s9  += s11;
    s11 += data[11]; s1  ^= s9;  s10 ^= s11; s11 = Rot64(s11,46); s10 += s0;
}

inline
SpookyHashAlgorithmImp::Uint64 SpookyHashAlgorithmImp::Rot64(Uint64 x, int k)
{
    return (x << k) | (x >> (64 - k));
}

inline
void SpookyHashAlgorithmImp::ShortEnd(Uint64 &h0,
                                      Uint64 &h1,
                                      Uint64 &h2,
                                      Uint64 &h3)
{
    h3 ^= h2;  h2 = Rot64(h2,15);  h3 += h2;
    h0 ^= h3;  h3 = Rot64(h3,52);  h0 += h3;
    h1 ^= h0;  h0 = Rot64(h0,26);  h1 += h0;
    h2 ^= h1;  h1 = Rot64(h1,51);  h2 += h1;
    h3 ^= h2;  h2 = Rot64(h2,28);  h3 += h2;
    h0 ^= h3;  h3 = Rot64(h3,9);   h0 += h3;
    h1 ^= h0;  h0 = Rot64(h0,47);  h1 += h0;
    h2 ^= h1;  h1 = Rot64(h1,54);  h2 += h1;
    h3 ^= h2;  h2 = Rot64(h2,32);  h3 += h2;
    h0 ^= h3;  h3 = Rot64(h3,25);  h0 += h3;
    h1 ^= h0;  h0 = Rot64(h0,63);  h1 += h0;
}

inline
void SpookyHashAlgorithmImp::ShortMix(Uint64 &h0,
                                      Uint64 &h1,
                                      Uint64 &h2,
                                      Uint64 &h3)
{
    h2 = Rot64(h2,50);  h2 += h3;  h0 ^= h2;
    h3 = Rot64(h3,52);  h3 += h0;  h1 ^= h3;
    h0 = Rot64(h0,30);  h0 += h1;  h2 ^= h0;
    h1 = Rot64(h1,41);  h1 += h2;  h3 ^= h1;
    h2 = Rot64(h2,54);  h2 += h3;  h0 ^= h2;
    h3 = Rot64(h3,48);  h3 += h0;  h1 ^= h3;
    h0 = Rot64(h0,38);  h0 += h1;  h2 ^= h0;
    h1 = Rot64(h1,37);  h1 += h2;  h3 ^= h1;
    h2 = Rot64(h2,62);  h2 += h3;  h0 ^= h2;
    h3 = Rot64(h3,34);  h3 += h0;  h1 ^= h3;
    h0 = Rot64(h0,5);   h0 += h1;  h2 ^= h0;
    h1 = Rot64(h1,36);  h1 += h2;  h3 ^= h1;
}

}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg Finance L.P.
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
