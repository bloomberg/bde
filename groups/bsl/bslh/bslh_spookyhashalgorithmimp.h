// bslh_spookyhashalgorithmimp.h                                      -*-C++-*-
#ifndef INCLUDED_BSLH_SPOOKYHASHALGORITHMIMP
#define INCLUDED_BSLH_SPOOKYHASHALGORITHMIMP

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide BDE style encapsulation of 3rd party SpookyHash code.
//
//@CLASSES:
//  bslh::SpookyHashAlgorithmImp: encapsulation of 3rd party SpookyHash code
//
//@SEE_ALSO: bslh_hash, bslh_spookyhashalgorithm
//
//@DESCRIPTION: 'bslh::SpookyHashAlgorithmImp' provides BDE style encapsulation
// around Bob Jenkins' canonical SpookyHash implementation.  SpookyHash
// provides a way to hash contiguous data all at once, or discontiguous data in
// pieces.  More information is available at:
// http://burtleburtle.net/bob/hash/spooky.html
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example: Creating 128-bit checksums
///- - - - - - - - - - - - - - - - - -
// Suppose we have a library of 4 billion pieces of data and we want to store
// checksums for this data.  For a 64-bit hash, there is a 35% chance of two of
// these checksums colliding (according to the approximation found here:
// http://en.wikipedia.org/wiki/Birthday_problem).  We want to avoid checksum
// collision, so we will use the 128-bit hashing functionality provided by
// 'SpookyHashAlgorithmImp'.
//
// First, we will declare a class 'CheckedData' which will store some data as
// well as the checksum associated with it.
//..
//
//  class CheckedData {
//      // This class holds a pointer to data and provides a way of verifying
//      // that the data has not changed.
//
//      // TYPES
//      typedef bsls::Types::Uint64 Uint64;
//
//      // DATA
//      size_t      d_length;
//      const char *d_data;
//      Uint64      d_checksum1;
//      Uint64      d_checksum2;
//
//    public:
//      CheckedData(const char *data, size_t length);
//          // Creates an instance of this class having the specified 'length'
//          // bytes of 'data'.  The behavior is undefined unless 'data' is
//          // initialized with at least 'length' bytes, and remains valid for
//          // the lifetime of this object.  Note that only a pointer to the
//          // data will be maintained, it will not be copied.
//
//      const char *getData();
//          // Return a pointer to the data being tracked by this class.
//
//      bool isDataValid();
//          // Return 'true' if the data stored in this class matches the
//          // stored checksum, and 'false' otherwise.
//  };
//
//..
// Then, we define the 'CheckedData' constructor.  Here we will use
// 'SpookyHashImp' to calculate a 128-bit checksum.
//..
//
//  CheckedData::CheckedData(const char *data, size_t length)
//  : d_length(length)
//  , d_data(data)
//  , d_checksum1(0)
//  , d_checksum2(0)
//  {
//      BSLS_ASSERT(data);
//
//      SpookyHashAlgorithmImp hashAlg(1, 2);
//
//      hashAlg.hash128(d_data, d_length, &d_checksum1, &d_checksum2);
//  }
//
//  const char *CheckedData::getData() {
//      return d_data;
//  }
//
//..
// Next, we define 'isDataValid'.  We will generate a checksum from the
// contained data and then compare it to the checksum we generated when the
// class was created.  If the two hashes match, then we can be reasonably
// certain that the data is still in a valid state (the chance of an accidental
// collision is very low).  If the checksums do not match, we know that the
// data has been corrupted.  We will not be able to restore it, but we will
// know not to trust it.
//..
//
//  bool CheckedData::isDataValid() {
//      SpookyHashAlgorithmImp hashAlg(1, 2);
//      Uint64 checksum1 = 0;
//      Uint64 checksum2 = 0;
//
//      hashAlg.hash128(d_data, d_length, &checksum1, &checksum2);
//
//      return (d_checksum1 == checksum1) && (d_checksum2 == checksum2);
//  }
//..
// Then, we store some data in our 'CheckedData' class for safekeeping.
//..
//
//      char data[] = "To be, or not to be--that is the question:"
//                    "Whether 'tis nobler in the mind to suffer"
//                    "The slings and arrows of outrageous fortune"
//                    "Or to take arms against a sea of troubles"
//                    "And by opposing end them.  To die, to sleep--"
//                    "No more--and by a sleep to say we end"
//                    "The heartache, and the thousand natural shocks"
//                    "That flesh is heir to.  'Tis a consummation"
//                    "Devoutly to be wished.  To die, to sleep--"
//                    "To sleep--perchance to dream: ay, there's the rub,"
//                    "For in that sleep of death what dreams may come"
//                    "When we have shuffled off this mortal coil,"
//                    "Must give us pause.  There's the respect"
//                    "That makes calamity of so long life."
//                    "For who would bear the whips and scorns of time,"
//                    "Th' oppressor's wrong, the proud man's contumely"
//                    "The pangs of despised love, the law's delay,"
//                    "The insolence of office, and the spurns"
//                    "That patient merit of th' unworthy takes,"
//                    "When he himself might his quietus make"
//                    "With a bare bodkin? Who would fardels bear,"
//                    "To grunt and sweat under a weary life,"
//                    "But that the dread of something after death,"
//                    "The undiscovered country, from whose bourn"
//                    "No traveller returns, puzzles the will,"
//                    "And makes us rather bear those ills we have"
//                    "Than fly to others that we know not of?"
//                    "Thus conscience does make cowards of us all,"
//                    "And thus the native hue of resolution"
//                    "Is sicklied o'er with the pale cast of thought,"
//                    "And enterprise of great pitch and moment"
//                    "With this regard their currents turn awry"
//                    "And lose the name of action. -- Soft you now,"
//                    "The fair Ophelia! -- Nymph, in thy orisons"
//                    "Be all my sins remembered.";
//      CheckedData checkedData(data, strlen(data));
//
//..
// Now, we check that the 'CheckedData' recognizes that it is still valid.
//..
//
//      ASSERT(checkedData.isDataValid());
//
//..
// Finally, we tamper with the data and check that our 'CheckedData' class can
// detect this.
//..
//      data[34] = 'z';
//      ASSERT(!checkedData.isDataValid());
//..
//
///Changes
///-------
// The third party code begins with the "SpookyHash" header below, and
// continues until the BloombergLP copyright notice.  Changes made to the
// original code include:
//
//: 1 Added 'BloombergLP' and 'bslh' namespaces
//:
//: 2 Renamed 'SpookyHash' to 'SpookyHashAlgorithmImp'
//:
//: 3 Removed usage of 'stdint.h' (which might not be available on all
//:   platforms) and updated associated 'typedef's
//:
//: 4 Added 'include' guards
//:
//: 5 Made some methods private
//:
//: 6 Reformatted comments and added comments
//:
//: 7 Updated indenting to BDE style
//:
//: 8 Moved 'typedef's within class
//:
//: 9 Changed C-style casts to 'static_cast's
//:
//: 10 Reordered methods according to BDE style
//:
//: 11 Added inline to 'Hash32' and 'Hash64'
//:
//: 12 Changed static constants to 'enum's to avoid storage overhead
//:
//: 13 Added constructor in place of 'init'
//:
//: 14 Made function names lower case (had to change 'Final' to 'finalize' and
//:    'Short' to 'shortHash' to avoid using a keyword)
//
///Third-Party Documentation
///-------------------------
//..
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
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>    // 'size_t'
#define INCLUDED_STDDEF_H
#endif

namespace BloombergLP {

namespace bslh {


class SpookyHashAlgorithmImp {
    // This class wraps an implementation of Bob Jenkin's "SpookyHash" in a
    // BDE-style component.  For more information, see
    // http://burtleburtle.net/bob/hash/spooky.html .

  public:
    typedef  bsls::Types::Uint64  Uint64;
    typedef  unsigned int         Uint32;
    typedef  unsigned short       Uint16;
    typedef  unsigned char        Uint8;

  private:
    // DATA
    enum { k_NUM_VARS = 12 };
        // Number of 64-bit integers used in the internal state.

    enum { k_BLOCK_SIZE = k_NUM_VARS * 8 };
        // Size of the internal state, in bytes.

    enum { k_BUFFER_SIZE = k_BLOCK_SIZE * 2 };
        // Size of buffer of unhashed data, in bytes.

    static const Uint64 sc_const = 0xdeadbeefdeadbeefULL;
        // A non-zero, odd, constant that has an irregular distribution of 1's
        // and 0's to be used in hashing calculations.

    Uint64 m_data[2 * k_NUM_VARS]; // unhashed data, for partial messages
    Uint64 m_state[k_NUM_VARS];    // internal state of the hash
    size_t m_length;               // total length of the input so far
    Uint8  m_remainder;            // length of unhashed data stashed in m_data

    // PRIVATE CLASS METHODS
    static void end(const Uint64 *data,
                    Uint64 &h0, Uint64 &h1, Uint64 &h2, Uint64 &h3,
                    Uint64 &h4, Uint64 &h5, Uint64 &h6, Uint64 &h7,
                    Uint64 &h8, Uint64 &h9, Uint64 &h10,Uint64 &h11);
        // Incorporate the first 12 bytes of the specified 'data' into 'h0',
        // 'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'h7', 'h8', 'h9', 'h10', and
        // 'h11', and then mix the inputs together so that 'h0' and 'h1' are a
        // hash of all the inputs.  Note that non-BDE-standard passing by
        // non-const reference is used here to remain consistent with the
        // cannonical implementation.  The behavior is undefined unles 'data'
        // points at least 8 bytes of initialized memory.

    static void endPartial(Uint64 &h0, Uint64 &h1, Uint64 &h2, Uint64 &h3,
                           Uint64 &h4, Uint64 &h5, Uint64 &h6, Uint64 &h7,
                           Uint64 &h8, Uint64 &h9, Uint64 &h10,Uint64 &h11);
        // Combine the specified 'h0', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6',
        // 'h7', 'h8', 'h9', 'h10', and 'h11' together so that 'h0' and 'h1'
        // will be a hash of all the inputs.  Note that non-BDE-standard
        // passing by non-const reference is used here to remain consistent
        // with the cannonical implementation.  The behavior is undefined unles
        // 'data' points at least 8 bytes of initialized memory.

    static void mix(const Uint64 *data,
                    Uint64 &s0, Uint64 &s1, Uint64 &s2,  Uint64 &s3,
                    Uint64 &s4, Uint64 &s5, Uint64 &s6,  Uint64 &s7,
                    Uint64 &s8, Uint64 &s9, Uint64 &s10, Uint64 &s11);
        // Thoroughly mix the first 12 bytes of the specified 'data' into 's0',
        // 's1', 's2', 's3', 's4', 's5', 's6', 's7', 's8', 's9', 's10', and
        // 's11'.  This method should be used when the input is 96 bytes or
        // longer to prevent the loss of entropy, because the internal state of
        // 'SpookyHashAlgorithmImp' is overwritten every 96 bytes.  Note that
        // non-BDE-standard passing by non-const reference is used here to
        // remain consistent with the cannonical implementation.  The behavior
        // is undefined unles 'data' points at least 8 bytes of initialized
        // memory.

    static Uint64 rot64(Uint64 x, int k);
        // Return the specified 'x' left rotated by 'k' bits.

    static void shortHash(const void *message,
                          size_t      length,
                          Uint64     *hash1,
                          Uint64     *hash2);
        // Hash the specified 'length' bytes of 'message' using 'hash1' and
        // 'hash2' as seeds.  Load the higher order bits of the resulting
        // 128-bit hash value into 'hash1' and the lower order bits in 'hash2'.
        // This method is meant to be used for messages less than 192 bytes in
        // length because of its lower startup cost.  The behavior is undefined
        // unless 'message' points at least 'length' bytes of initialized
        // memory and both 'hash1' and 'hash2' point to at least 8 bytes of
        // initialized, modifiable, memory.

    static void shortEnd(Uint64 &h0, Uint64 &h1, Uint64 &h2, Uint64 &h3);
        // Combine the specified 'h0', 'h1', 'h2', and 'h3' together so that
        // 'h0' and 'h1' will be a hash of all the inputs.  Note that
        // non-BDE-standard passing by non-const reference is used here to
        // remain consistent with the cannonical implementation.

    static void shortMix(Uint64 &h0, Uint64 &h1, Uint64 &h2, Uint64 &h3);
        // Thoroughly mix the specified 'h0', 'h1', 'h2', and 'h3' so that each
        // bit of input contributes entropy to every bit of the final states of
        // 'h0', 'h1', 'h2', and 'h3'.  Note that non-BDE-standard passing by
        // non-const reference is used here to remain consistent with the
        // cannonical implementation.

    // NOT IMPLEMENTED
    SpookyHashAlgorithmImp(const SpookyHashAlgorithmImp& original);// = delete;
        // Do not allow copy construction

    SpookyHashAlgorithmImp& operator=(const SpookyHashAlgorithmImp& rhs);
                                                                   // = delete;
        // Do not allow assignment

  public:
    // PUBLIC CLASS METHODS
    static Uint32 hash32(const void *message,
                         size_t      length,
                         Uint32      seed);
        // Hash the specified 'length' bytes of 'message' using 'seed' as a
        // seed.  Return the resulting 32-bit hash.  The behavior is undefined
        // unless 'message' points at least 'length' bytes of initialized
        // memory.

    static Uint64 hash64(const void *message,
                         size_t      length,
                         Uint64      seed);
        // Hash the specified 'length' bytes of 'message' using 'seed' as a
        // seed.  Return the resulting 64-bit hash.  The behavior is undefined
        // unless 'message' points at least 'length' bytes of initialized
        // memory.

    static void hash128(const void *message,
                        size_t      length,
                        Uint64     *hash1,
                        Uint64     *hash2);
        // Hash the specified 'length' bytes of 'message' using 'hash1' and
        // 'hash2' as seeds.  Load the higher order bits of the resulting
        // 128-bit hash value into 'hash1' and the lower order bits in 'hash2'.
        // The behavior is undefined unless 'message' points at least 'length'
        // bytes of initialized memory and both 'hash1' and 'hash2' point to at
        // least 8 bytes of initialized, modifiable, memory.

    // CREATORS
    SpookyHashAlgorithmImp(Uint64 seed1, Uint64 seed2);
        // Create a 'bslh::SpookyHashAlgorithmImp', initializing the internal
        // state of the object using the specified 'seed1' and 'seed2' as seeds
        // for the algorithm.

    //! ~SpookyHashAlgorithmImp() = default;
        // Destroy this object.

    // MANIPULATORS
    void update(const void *message, size_t length);
        // Accumulate the specified 'length' bytes of 'message' into the
        // internal state of the algorithm.  Accumulating bytes through
        // 'Update' will produce the same result as hashing them all at once
        // through the 'HashXX' static methods.  The behavior is undefined
        // unles 'message' points at least 'length' bytes of initialized
        // memory.

    void finalize(Uint64 *hash1, Uint64 *hash2);
        // Load the finalized hash into the specified 'hash1' and 'hash2'.
        // 'hash1' will contain the higher order bits of the hash and 'hash2'
        // will contain the lower order bits.  The internal state of the
        // algorithm will be modified, meaning that calling final multiple
        // times will result in different hash values being returned.  The
        // returned hash will be the same as if 'Hash128' had been called will
        // all of the accumulated data in one block.  The behavior is
        // undefined unless both 'hash1' and 'hash2' point to 8 bytes of
        // modifiable memory.  Note that a value will be returned even if
        // 'update' has not been called.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// PUBLIC CLASS METHODS
inline
SpookyHashAlgorithmImp::Uint32 SpookyHashAlgorithmImp::hash32(
                                                           const void *message,
                                                           size_t      length,
                                                           Uint32      seed)
{
    BSLS_ASSERT(message);
    Uint64 hash1 = seed, hash2 = seed;
    hash128(message, length, &hash1, &hash2);
    return static_cast<Uint32>(hash1);
}

inline
SpookyHashAlgorithmImp::Uint64 SpookyHashAlgorithmImp::hash64(
                                                           const void *message,
                                                           size_t      length,
                                                           Uint64      seed)
{
    BSLS_ASSERT(message);
    Uint64 hash1 = seed;
    hash128(message, length, &hash1, &seed);
    return hash1;
}

// CREATORS
inline
SpookyHashAlgorithmImp::SpookyHashAlgorithmImp(Uint64 seed1, Uint64 seed2)
: m_length(0)
, m_remainder(0)
{
    m_state[0] = seed1;
    m_state[1] = seed2;
}

// PRIVATE CLASS METHODS
inline
void SpookyHashAlgorithmImp::end(
    const Uint64 *data,
    Uint64 &h0, Uint64 &h1, Uint64 &h2, Uint64 &h3,
    Uint64 &h4, Uint64 &h5, Uint64 &h6, Uint64 &h7,
    Uint64 &h8, Uint64 &h9, Uint64 &h10,Uint64 &h11)
{
    BSLS_ASSERT(data);
    h0 += data[0];   h1 += data[1];   h2 += data[2];   h3 += data[3];
    h4 += data[4];   h5 += data[5];   h6 += data[6];   h7 += data[7];
    h8 += data[8];   h9 += data[9];   h10 += data[10]; h11 += data[11];
    endPartial(h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
    endPartial(h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
    endPartial(h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
}

inline
void SpookyHashAlgorithmImp::endPartial(
    Uint64 &h0, Uint64 &h1, Uint64 &h2, Uint64 &h3,
    Uint64 &h4, Uint64 &h5, Uint64 &h6, Uint64 &h7,
    Uint64 &h8, Uint64 &h9, Uint64 &h10,Uint64 &h11)
{
    h11+= h1;    h2 ^= h11;   h1 = rot64(h1,44);
    h0 += h2;    h3 ^= h0;    h2 = rot64(h2,15);
    h1 += h3;    h4 ^= h1;    h3 = rot64(h3,34);
    h2 += h4;    h5 ^= h2;    h4 = rot64(h4,21);
    h3 += h5;    h6 ^= h3;    h5 = rot64(h5,38);
    h4 += h6;    h7 ^= h4;    h6 = rot64(h6,33);
    h5 += h7;    h8 ^= h5;    h7 = rot64(h7,10);
    h6 += h8;    h9 ^= h6;    h8 = rot64(h8,13);
    h7 += h9;    h10^= h7;    h9 = rot64(h9,38);
    h8 += h10;   h11^= h8;    h10= rot64(h10,53);
    h9 += h11;   h0 ^= h9;    h11= rot64(h11,42);
    h10+= h0;    h1 ^= h10;   h0 = rot64(h0,54);
}

inline
void SpookyHashAlgorithmImp::mix(
    const Uint64 *data,
    Uint64 &s0, Uint64 &s1, Uint64 &s2, Uint64 &s3,
    Uint64 &s4, Uint64 &s5, Uint64 &s6, Uint64 &s7,
    Uint64 &s8, Uint64 &s9, Uint64 &s10,Uint64 &s11)
{
    BSLS_ASSERT(data);
    s0 += data[0];   s2  ^= s10; s11 ^= s0;  s0  = rot64(s0,11);  s11 += s1;
    s1 += data[1];   s3  ^= s11; s0  ^= s1;  s1  = rot64(s1,32);  s0  += s2;
    s2 += data[2];   s4  ^= s0;  s1  ^= s2;  s2  = rot64(s2,43);  s1  += s3;
    s3 += data[3];   s5  ^= s1;  s2  ^= s3;  s3  = rot64(s3,31);  s2  += s4;
    s4 += data[4];   s6  ^= s2;  s3  ^= s4;  s4  = rot64(s4,17);  s3  += s5;
    s5 += data[5];   s7  ^= s3;  s4  ^= s5;  s5  = rot64(s5,28);  s4  += s6;
    s6 += data[6];   s8  ^= s4;  s5  ^= s6;  s6  = rot64(s6,39);  s5  += s7;
    s7 += data[7];   s9  ^= s5;  s6  ^= s7;  s7  = rot64(s7,57);  s6  += s8;
    s8 += data[8];   s10 ^= s6;  s7  ^= s8;  s8  = rot64(s8,55);  s7  += s9;
    s9 += data[9];   s11 ^= s7;  s8  ^= s9;  s9  = rot64(s9,54);  s8  += s10;
    s10 += data[10]; s0  ^= s8;  s9  ^= s10; s10 = rot64(s10,22); s9  += s11;
    s11 += data[11]; s1  ^= s9;  s10 ^= s11; s11 = rot64(s11,46); s10 += s0;
}

inline
SpookyHashAlgorithmImp::Uint64 SpookyHashAlgorithmImp::rot64(Uint64 x, int k)
{
    return (x << k) | (x >> (64 - k));
}

inline
void SpookyHashAlgorithmImp::shortEnd(Uint64 &h0,
                                      Uint64 &h1,
                                      Uint64 &h2,
                                      Uint64 &h3)
{
    h3 ^= h2;  h2 = rot64(h2,15);  h3 += h2;
    h0 ^= h3;  h3 = rot64(h3,52);  h0 += h3;
    h1 ^= h0;  h0 = rot64(h0,26);  h1 += h0;
    h2 ^= h1;  h1 = rot64(h1,51);  h2 += h1;
    h3 ^= h2;  h2 = rot64(h2,28);  h3 += h2;
    h0 ^= h3;  h3 = rot64(h3,9);   h0 += h3;
    h1 ^= h0;  h0 = rot64(h0,47);  h1 += h0;
    h2 ^= h1;  h1 = rot64(h1,54);  h2 += h1;
    h3 ^= h2;  h2 = rot64(h2,32);  h3 += h2;
    h0 ^= h3;  h3 = rot64(h3,25);  h0 += h3;
    h1 ^= h0;  h0 = rot64(h0,63);  h1 += h0;
}

inline
void SpookyHashAlgorithmImp::shortMix(Uint64 &h0,
                                      Uint64 &h1,
                                      Uint64 &h2,
                                      Uint64 &h3)
{
    h2 = rot64(h2,50);  h2 += h3;  h0 ^= h2;
    h3 = rot64(h3,52);  h3 += h0;  h1 ^= h3;
    h0 = rot64(h0,30);  h0 += h1;  h2 ^= h0;
    h1 = rot64(h1,41);  h1 += h2;  h3 ^= h1;
    h2 = rot64(h2,54);  h2 += h3;  h0 ^= h2;
    h3 = rot64(h3,48);  h3 += h0;  h1 ^= h3;
    h0 = rot64(h0,38);  h0 += h1;  h2 ^= h0;
    h1 = rot64(h1,37);  h1 += h2;  h3 ^= h1;
    h2 = rot64(h2,62);  h2 += h3;  h0 ^= h2;
    h3 = rot64(h3,34);  h3 += h0;  h1 ^= h3;
    h0 = rot64(h0,5);   h0 += h1;  h2 ^= h0;
    h1 = rot64(h1,36);  h1 += h2;  h3 ^= h1;
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
