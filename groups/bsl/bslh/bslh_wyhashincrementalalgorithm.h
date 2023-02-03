// bslh_wyhashincrementalalgorithm.h                                  -*-C++-*-
#ifndef INCLUDED_BSLH_WYHASHINCREMENTALALGORITHM
#define INCLUDED_BSLH_WYHASHINCREMENTALALGORITHM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an implementation of the WyHash algorithm final v3.
//
//@CLASSES:
//  bslh::WyHashIncrementalAlgorithm: functor implementing the WyHash algorithm
//
//@SEE_ALSO: bslh_hash
//
//@DESCRIPTION: 'bslh::WyHashIncrementalAlgorithm' implements the WyHash
// algorithm by Wang Yi et al (see implementation file for full list of
// authors) with modifications.  This algorithm is known to be very fast yet
// have good avalanche behavior.
//
// The original algorithm was downloaded from
// https://github.com/wangyi-fudan/wyhash/blob/master/wyhash.h which had been
// updated on September 14, 2021, last commit 166f352, and modified to conform
// to BDE coding conventions with no change in the binary results produced.
//
// The modifications are:
//: o A property is added that hashing a segment in one pass will yeild the
//:   same result as hashing it in pieces.
//:
//: o Byte-swapping is eliminated for speed, and therefore the algorithm yields
//:   different results depending on the byte-order of the host.
//
///Security
///--------
// WyHash is *not* a "Cryptographically Secure" hash.  It is "Cryptographically
// Strong", but not "Cryptographically Secure".  In order to be
// cryptographically secure, an algorithm must, among other things, provide
// "Collision Resistance", described in
// https://en.wikipedia.org/wiki/Collision_resistance , meaning that it should
// be difficult to find two different messages 'm1' and 'm2' such that
// 'hash(m1) == hash(m2)'.  Because of the limited sized output (only 2**64
// possibilities) and the fast execution time of the algorithm, it is probable
// to find two such values searching only about 'sqrt(2**64) == 2**32' inputs,
// which wont take long.
//
// WyHash *is*, however, a cryptographically strong PRF (pseudo-random
// function).  This means, assuming a cryptographically secure random seed is
// given, the output of this algorithm will be indistinguishable from a uniform
// random distribution.  This property is enough for the algorithm to be able
// to protect a hash table from malicious Denial of Service (DoS) attacks.
//
///Denial of Service (DoS) Protection
/// - - - - - - - - - - - - - - - - -
// Given a cryptographically secure seed, this algorithm will produce hashes
// with a distribution that is indistinguishable from random.  This
// distribution means that there is no way for an attacker to predict which
// keys will cause collisions, meaning that this algorithm can help mitigate
// Denial of Service (DoS) attacks on a hash table.  DoS attacks occur when an
// attacker deliberately degrades the performance of the hash table by
// inserting data that will collide to the same bucket, causing an average
// constant time lookup to become a linear search.  This protection is only
// effective if the seed provided is a cryptographically secure random number
// that is not available to the attacker.
//
///Hash Distribution
///-----------------
// Output hashes will be well distributed and will avalanche, which means
// changing one bit of the input will change approximately 50% of the output
// bits.  This will prevent similar values from funneling to the same hash or
// bucket.
//
///Alignment-Independence
///----------------------
// The value obtained by hashing a segment of memory is independent of the
// alignment of the segment of memory.
//
///Subdivision-Invariance
///----------------------
// Note that this algorithm is *subdivision-invariant* (see
// {'bslh_hash'|Subdivision-Invariance}).
//
///Speed
///-----
// This algorithm is at least 2X faster than Spooky on all sizes of objects on
// Linux, Windows, and Solaris.  On Aix it is about twice as fast as Spooky on
// small objects and about 50% slower on large objects.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example: Creating and Using a Hash Table
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we have any array of types that define 'operator==', and we want a
// fast way to find out if values are contained in the array.  We can create a
// 'HashTable' data structure that is capable of looking up values in O(1)
// time.
//
// Further suppose that we will be storing futures (the financial instruments)
// in this table.  Since futures have standardized names, we don't have to
// worry about any malicious values causing collisions.  We will want to use a
// general purpose hashing algorithm with a good hash distribution and good
// speed.  This algorithm will need to be in the form of a hash functor -- an
// object that will take objects stored in our array as input, and yield a
// 64-bit int value.  The functor can pass the attributes of the 'TYPE' that
// are salient to hashing into the hashing algorithm, and then return the hash
// that is produced.
//
// We can use the result of the hash function to index into our array of
// 'buckets'.  Each 'bucket' is simply a pointer to a value in our original
// array of 'TYPE' objects.
//
// First, we define our 'HashTable' template class, with the two type
// parameters: 'TYPE' (the type being referenced) and 'HASHER' (a functor that
// produces the hash).
//..
//  template <class TYPE, class HASHER>
//  class HashTable {
//..
// This 'class template' implements a hash table providing fast lookup of an
// external, non-owned, array of values of (template parameter) 'TYPE'.
//
// The (template parameter) 'TYPE' shall have a transitive, symmetric
// 'operator==' function.  There is no requirement that it have any kind of
// creator defined.
//
// The 'HASHER' template parameter type must be a functor with a method having
// the following signature:
//..
//  size_t operator()(TYPE)  const;
//                   -OR-
//  size_t operator()(const TYPE&) const;
//..
// and 'HASHER' shall have a publicly accessible default constructor and
// destructor.
//
// Note that this hash table has numerous simplifications because we know the
// size of the array and never have to resize the table.
//..
//      // DATA
//      const TYPE       *d_values;          // Array of values table is to
//                                           // hold
//      size_t            d_numValues;       // Length of 'd_values'.
//      const TYPE      **d_bucketArray;     // Contains ptrs into 'd_values'
//      size_t            d_bucketArrayMask; // Will always be '2^N - 1'.
//      HASHER            d_hasher;          // User supplied hashing algorithm
//
//    private:
//      // PRIVATE ACCESSORS
//      bool lookup(size_t      *idx,
//                  const TYPE&  value,
//                  size_t       hashValue) const;
//          // Look up the specified 'value', having the specified 'hashValue',
//          // and load its index in 'd_bucketArray' into the specified 'idx'.
//          // If not found, return the vacant entry in 'd_bucketArray' where
//          // it should be inserted.  Return 'true' if 'value' is found and
//          // 'false' otherwise.
//
//    public:
//      // CREATORS
//      HashTable(const TYPE *valuesArray,
//                size_t      numValues);
//          // Create a hash table referring to the specified 'valuesArray'
//          // having length of the specified 'numValues'.  No value in
//          // 'valuesArray' shall have the same value as any of the other
//          // values in 'valuesArray'
//
//      ~HashTable();
//          // Free up memory used by this hash table.
//
//      // ACCESSORS
//      bool contains(const TYPE& value) const;
//          // Return true if the specified 'value' is found in the table and
//          // false otherwise.
//  };
//
//  // PRIVATE ACCESSORS
//  template <class TYPE, class HASHER>
//  bool HashTable<TYPE, HASHER>::lookup(size_t      *idx,
//                                       const TYPE&  value,
//                                       size_t       hashValue) const
//  {
//      const TYPE *ptr;
//      for (*idx = hashValue & d_bucketArrayMask; (ptr = d_bucketArray[*idx]);
//                                     *idx = (*idx + 1) & d_bucketArrayMask) {
//          if (value == *ptr) {
//              return true;                                          // RETURN
//          }
//      }
//
//      // value was not found in table
//
//      return false;
//  }
//
//  // CREATORS
//  template <class TYPE, class HASHER>
//  HashTable<TYPE, HASHER>::HashTable(const TYPE *valuesArray,
//                                     size_t      numValues)
//  : d_values(valuesArray)
//  , d_numValues(numValues)
//  , d_hasher()
//  {
//      size_t bucketArrayLength = 4;
//      while (bucketArrayLength < numValues * 4) {
//          bucketArrayLength *= 2;
//
//      }
//      d_bucketArrayMask = bucketArrayLength - 1;
//      d_bucketArray = new const TYPE *[bucketArrayLength];
//      memset(d_bucketArray,  0, bucketArrayLength * sizeof(TYPE *));
//
//      for (unsigned i = 0; i < numValues; ++i) {
//          const TYPE& value = d_values[i];
//          size_t idx;
//          const bool found = lookup(&idx, value, d_hasher(value));
//          BSLS_ASSERT_OPT(!found);    (void) found;
//          d_bucketArray[idx] = &d_values[i];
//      }
//  }
//
//  template <class TYPE, class HASHER>
//  HashTable<TYPE, HASHER>::~HashTable()
//  {
//      delete [] d_bucketArray;
//  }
//
//  // ACCESSORS
//  template <class TYPE, class HASHER>
//  bool HashTable<TYPE, HASHER>::contains(const TYPE& value) const
//  {
//      size_t idx;
//      return lookup(&idx, value, d_hasher(value));
//  }
//..
// Then, we define a 'Future' class, which holds a c-string 'name', char
// 'callMonth', and short 'callYear'.
//..
//  class Future {
//..
// This 'class' identifies a future contract.  It tracks the name, call month
// and year of the contract it represents, and allows equality comparison.
//..
//      // DATA
//      const char *d_name;    // held, not owned
//      const char  d_callMonth;
//      const short d_callYear;
//
//    public:
//      // CREATORS
//      Future(const char *name, const char callMonth, const short callYear)
//      : d_name(name), d_callMonth(callMonth), d_callYear(callYear)
//          // Create a 'Future' object out of the specified 'name',
//          // 'callMonth', and 'callYear'.
//      {}
//
//      Future() : d_name(""), d_callMonth('\0'), d_callYear(0)
//          // Create a 'Future' with default values.
//      {}
//
//      // ACCESSORS
//      const char * getMonth() const
//          // Return the month that this future expires.
//      {
//          return &d_callMonth;
//      }
//
//      const char * getName() const
//          // Return the name of this future
//      {
//          return d_name;
//      }
//
//      const short * getYear() const
//          // Return the year that this future expires
//      {
//          return &d_callYear;
//      }
//
//      bool operator==(const Future& rhs) const
//          // Compare this to the specified 'other' object and return true if
//          // they are equal
//      {
//          return (!strcmp(d_name, rhs.d_name)) &&
//                                            d_callMonth == rhs.d_callMonth &&
//                                            d_callYear  == rhs.d_callYear;
//      }
//  };
//
//  bool operator!=(const Future& lhs, const Future& rhs)
//      // Compare compare the specified 'lhs' and 'rhs' objects and return
//      // true if they are not equal
//  {
//      return !(lhs == rhs);
//  }
//..
// Next, we need a hash functor for 'Future'.  We are going to use the
// 'SpookyHashAlgorithm' because it is a fast, general purpose hashing
// algorithm that will provide an easy way to combine the attributes of
// 'Future' objects that are salient to hashing into one reasonable hash that
// will distribute the items evenly throughout the hash table.
//..
//  struct HashFuture {
//      // This struct is a functor that will apply the 'SpookyHashAlgorithm'
//      // to objects of type 'Future'.
//
//      bsls::Types::Uint64 d_seed;
//
//      HashFuture()
//      {
//          // Generate random bits in 'd_seed' based on the time of day in
//          // nanoseconds.
//
//          bsls::Types::Int64 nano =
//                    bsls::SystemTime::nowMonotonicClock().totalNanoseconds();
//          const int iterations = static_cast<int>(nano & 7) + 1;
//          for (int ii = 0; ii < iterations; ++ii) {
//              nano *= bsls::SystemTime::nowMonotonicClock().
//                                                          totalNanoseconds();
//              nano += nano >> 32;
//          }
//
//          BSLMF_ASSERT(sizeof(d_seed) <= sizeof(nano));
//
//          memcpy(&d_seed, &nano, sizeof(d_seed));
//      }
//
//      // MANIPULATOR
//      size_t operator()(const Future& future) const
//          // Return the hash of the of the specified 'future'.  Note that
//          // this uses the 'SpookyHashAlgorithm' to quickly combine the
//          // attributes of 'Future' objects that are salient to hashing into
//          // a hash suitable for a hash table.
//      {
//          bslh::WyHashIncrementalAlgorithm hash(d_seed);
//
//          hash(future.getName(),  strlen(future.getName()));
//          hash(future.getMonth(), sizeof(char));
//          hash(future.getYear(),  sizeof(short));
//
//          return static_cast<size_t>(hash.computeHash());
//      }
//  };
//..

#include <bslscm_version.h>

#include <bslmf_assert.h>
#include <bslmf_istriviallycopyable.h>

#include <bsls_assert.h>
#include <bsls_byteorder.h>
#include <bsls_keyword.h>
#include <bsls_performancehint.h>
#include <bsls_types.h>

#include <algorithm>

#include <stddef.h>  // for 'size_t'
#include <stdint.h>  // for 'uint64_t'
#include <string.h>  // for 'memcpy'

#if defined(_MSC_VER) && defined(_M_X64)
#include <intrin.h>
#pragma intrinsic(_umul128)
#endif

// protections that produce different results:

//: 0 normal valid behavior
//:
//: 1 extra protection against entropy loss (probability=2^-63), aka.  "blind
//:   multiplication"

#undef  BSLH_WYHASHINCREMENTALALGORITHM_WYMUM_XOR
#define BSLH_WYHASHINCREMENTALALGORITHM_WYMUM_XOR 0

//: 0 normal, real version of 64x64 -> 128 multiply, slow on 32 bit systems
//:
//: 1 not real multiply, faster on 32 bit systems but produces different
//:   results

#undef  BSLH_WYHASHINCREMENTALALGORITHM_WYMUM_PSEUDO_MULTIPLY
#define BSLH_WYHASHINCREMENTALALGORITHM_WYMUM_PSEUDO_MULTIPLY 0

namespace BloombergLP {
namespace bslh {

                    // ======================================
                    // class bslh::WyHashIncrementalAlgorithm
                    // ======================================

class WyHashIncrementalAlgorithm {
    // This class wraps an implementation of the "WyHash" hash algorithm in an
    // interface that is usable in the modular hashing system in 'bslh'.

  private:
    // PRIVATE TYPES
    enum { k_PREPAD_LENGTH = 16,                       // See implementation
           k_PREPAD_LENGTH_RAW = k_PREPAD_LENGTH - 1,  // notes in the imp file
           k_REPEAT_LENGTH = 48 };

  public:
    // TYPES
    typedef bsls::Types::Uint64 result_type;
        // Typedef indicating the value type returned by this algorithm.

    enum { k_SEED_LENGTH = sizeof(uint64_t) };

  private:
    // DATA
    uint64_t d_initialSeed, d_seed, d_see1, d_see2; // seeds, state of the hash
                                                    // computation

    bool     d_last16AtEnd;                         // indicates that the last
                                                    // 16 bytes at the end of
                                                    // the repeat buf are valid

    uint8_t  d_buffer[k_PREPAD_LENGTH_RAW + k_REPEAT_LENGTH];
                                                    // prePad + repeat buffer,
                                                    // not including the first
                                                    // (never used) byte.  See
                                                    // the implementation notes
                                                    // in the imp file

    size_t   d_totalLen;                            // total length of input so
                                                    // far

    // CLASS DATA

    // These values for 's_secret*' were copied directly from the original
    // github source.

    static const uint64_t s_secret0 = 0xa0761d6478bd642full;
    static const uint64_t s_secret1 = 0xe7037ed1a0b428dbull;
    static const uint64_t s_secret2 = 0x8ebc6af09c88c6e3ull;
    static const uint64_t s_secret3 = 0x589965cc75374cc3ull;

  private:
    // NOT IMPLEMENTED
    WyHashIncrementalAlgorithm(const WyHashIncrementalAlgorithm&)
                                                          BSLS_KEYWORD_DELETED;
    WyHashIncrementalAlgorithm& operator=(const WyHashIncrementalAlgorithm&)
                                                          BSLS_KEYWORD_DELETED;

  private:
    // PRIVATE CLASS METHODS
#if BSLH_WYHASHINCREMENTALALGORITHM_WYMUM_PSEUDO_MULTIPLY
    static uint64_t _wyrot(uint64_t x);
        // Return the specified 'x' with the high- and low-order 32 bits
        // swapped.
#endif

    static void _wymum(uint64_t *a_p, uint64_t *b_p);
        // Multiply the specified '*a_p' and '*b_p', yielding a 128 bit result,
        // when '*b_p' will contain the high 64 bits and '*a_p' will contain
        // the low 64 bits of the result.  This may be configured through
        // conditional switches to perform a faster function other than
        // multiply.

    static uint64_t _wymix(uint64_t a, uint64_t b);
        // Do a 64x64 -> 128 bit multiply of the specified 'a' and 'b', then
        // then return the bitwise-xor of the high and low 64-bits.

    static uint64_t _wyr8(const uint8_t *p);
        // Read 8 bytes, native-endian.  Note that 'p' might not be aligned.

    static uint64_t _wyr4(const uint8_t *p);
        // Read 4 bytes, native-endian,  Note that 'p' might not be aligned.

    static uint64_t _wyr3(const uint8_t *p, size_t k);
        // Read a mix of the specified 'k' bytes beginning at the specified
        // 'p', where 'k' is in the range '[ 1 .. 3 ]'.

    // PRIVATE MANIPULATORS
    uint8_t *prePadAt(ptrdiff_t offset);
        // Return a ptr to the address at the specified 'offset' after the
        // beginning of the 'prepad' area of the buffer.  The behavior is
        // undefined unless '1 <= offset'.

    void process48ByteSection(const uint8_t *buffer);
        // Process the specified 'k_REPEAT_LENGTH'-byte 'buffer'.  Note that
        // this function is called only when there is additional input beyond
        // the buffer.

    uint8_t *repeatBufferBegin();
        // Return a pointer to the beginning of the repeated buffer area.

    uint8_t *repeatBufferEnd();
        // Return a pointer past the end of the buffer.

  public:
    // CREATORS
    WyHashIncrementalAlgorithm();
        // Create a 'WyHashIncrementalAlgorithm' using a default initial seed.

    explicit WyHashIncrementalAlgorithm(uint64_t seed);
        // Create a 'bslh::WyHashIncrementalAlgorithm', seeded with the
        // specified 'seed'.

    explicit WyHashIncrementalAlgorithm(const char *seed);
        // Create a 'bslh::WyHashIncrementalAlgorithm', seeded with
        // 'k_SEED_LENGTH' bytes of data starting at the specified 'seed'.

    //! ~WyHashIncrementalAlgorithm() = default;
        // Destroy this object.

    // MANIPULATORS
    void operator()(const void *data, size_t numBytes);
        // Incorporate the specified 'data', of at least the specified
        // 'numBytes', into the internal state of the hashing algorithm.  Every
        // bit of data incorporated into the internal state of the algorithm
        // will contribute to the final hash produced by 'computeHash()'.  The
        // same hash value will be produced regardless of whether a sequence of
        // bytes is passed in all at once or through multiple calls to this
        // member function.  Input where 'numBytes' is 0 will have no effect on
        // the internal state of the algorithm.  The behaviour is undefined
        // unless 'data' points to a valid memory location with at least
        // 'numBytes' bytes of initialized memory or 'numBytes' is zero.

    result_type computeHash();
        // Return the finalized version of the hash that has been accumulated.
        // Note that this changes the internal state of the object, so calling
        // 'computeHash()' multiple times in a row will return different
        // results, and only the first result returned will match the expected
        // result of the algorithm.  Also note that a value will be returned,
        // even if data has not been passed into 'operator()'
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                        // --------------------------
                        // WyHashIncrementalAlgorithm
                        // --------------------------

// PRIVATE CLASS METHODS
#if BSLH_WYHASHINCREMENTALALGORITHM_WYMUM_PSEUDO_MULTIPLY
inline
uint64_t WyHashIncrementalAlgorithm::_wyrot(uint64_t x)
{
    return (x >> 32) | (x << 32);
}
#endif

inline
void WyHashIncrementalAlgorithm::_wymum(uint64_t *a_p, uint64_t *b_p)
{
#if BSLH_WYHASHINCREMENTALALGORITHM_WYMUM_PSEUDO_MULTIPLY
    const uint64_t hh = (*a_p >> 32) * (*b_p >> 32);
    const uint64_t hl = (*a_p >> 32) * static_cast<uint32_t>(*b_p);
    const uint64_t lh = static_cast<uint32_t>(*a_p) * (*b_p >> 32);
    const uint64_t ll = static_cast<uint64_t>(static_cast<uint32_t>(*a_p)) *
                                                   static_cast<uint32_t>(*b_p);

#if BSLH_WYHASHINCREMENTALALGORITHM_WYMUM_XOR
    // pseudo munge (not real multiply) -> xor

    *a_p ^= _wyrot(hl) ^ hh;
    *b_p ^= _wyrot(lh) ^ ll;
#else
    // pseudo munge (not real multiply)

    *a_p = _wyrot(hl) ^ hh;
    *b_p = _wyrot(lh) ^ ll;
#endif
#elif defined(__SIZEOF_INT128__)
    __uint128_t r = *a_p;
    r *= *b_p;

#if BSLH_WYHASHINCREMENTALALGORITHM_WYMUM_XOR
    // multiply -> xor

    *a_p ^= static_cast<uint64_t>(r);
    *b_p ^= static_cast<uint64_t>(r >> 64);
#else
    // multiply

    *a_p = static_cast<uint64_t>(r);
    *b_p = static_cast<uint64_t>(r >> 64);
#endif
#elif defined(_MSC_VER) && defined(_M_X64)
#if BSLH_WYHASHINCREMENTALALGORITHM_WYMUM_XOR
    // multiply -> xor

    uint64_t a, b;
    a = _umul128(*a_p, *b_p, &b);
    *a_p ^= a;
    *b_p ^= b;
#else
    // multiply

    *a_p = _umul128(*a_p, *b_p, b_p);
#endif
#else
    const uint64_t ha = *a_p >> 32, hb = *b_p >> 32;
    const uint64_t la = static_cast<uint32_t>(*a_p);
    const uint64_t lb = static_cast<uint32_t>(*b_p);

    const uint64_t rh = ha * hb, rl = la * lb;
    const uint64_t rm0 = ha * lb, rm1 = hb * la;
    const uint64_t t = rl + (rm0 << 32);

    const uint64_t lo = t + (rm1 << 32);
    const uint64_t hi = rh + (rm0 >> 32) + (rm1 >> 32) + (t < rl) + (lo < t);

#if BSLH_WYHASHINCREMENTALALGORITHM_WYMUM_XOR
    // multiply -> xor

    *a_p ^= lo;
    *b_p ^= hi;
#else
    // multiply

    *a_p = lo;
    *b_p = hi;
#endif
#endif
}

//multiply and xor mix function, aka MUM
inline
uint64_t WyHashIncrementalAlgorithm::_wymix(uint64_t a, uint64_t b)
{
    _wymum(&a, &b);
    return a ^ b;
}

//read functions
inline
uint64_t WyHashIncrementalAlgorithm::_wyr8(const uint8_t *p)
{
    uint64_t v;
    memcpy(&v, p, 8);
    return v;
}

inline
uint64_t WyHashIncrementalAlgorithm::_wyr4(const uint8_t *p)
{
    uint32_t v;
    memcpy(&v, p, 4);
    return v;
}

inline
uint64_t WyHashIncrementalAlgorithm::_wyr3(const uint8_t *p, size_t k)
    // Read a mix of the 'k' bytes beginning at 'p', where 'k' is in the range
    // '[ 1 .. 3 ]'.
{
    BSLS_ASSERT_SAFE(1 <= k && k <= 3);

    return (static_cast<uint64_t>(p[0]) << 16) |
           (static_cast<uint64_t>(p[k >> 1]) << 8) |
           p[k - 1];
}

// PRIVATE MANIPULATORS
inline
uint8_t *WyHashIncrementalAlgorithm::prePadAt(ptrdiff_t offset)
{
    BSLMF_ASSERT(sizeof(d_last16AtEnd) == 1);    // see implementation doc
    BSLS_ASSERT_SAFE(1 <= offset);

    return d_buffer + offset - 1;
}

inline
void WyHashIncrementalAlgorithm::process48ByteSection(const uint8_t *buffer)
{
    d_seed = _wymix(_wyr8(buffer)      ^ s_secret1,
                    _wyr8(buffer +  8) ^ d_seed);
    d_see1 = _wymix(_wyr8(buffer + 16) ^ s_secret2,
                    _wyr8(buffer + 24) ^ d_see1);
    d_see2 = _wymix(_wyr8(buffer + 32) ^ s_secret3,
                    _wyr8(buffer + 40) ^ d_see2);
}

inline
uint8_t *WyHashIncrementalAlgorithm::repeatBufferBegin()
{
    return d_buffer + k_PREPAD_LENGTH_RAW;
}

inline
uint8_t *WyHashIncrementalAlgorithm::repeatBufferEnd()
{
    return d_buffer + sizeof(d_buffer);
}

// CREATORS
inline
WyHashIncrementalAlgorithm::WyHashIncrementalAlgorithm()
: d_initialSeed(0x50defacedfacade5ULL)
, d_last16AtEnd(false)
, d_totalLen(0)
{
    BSLMF_ASSERT(sizeof(*this) == 13 * sizeof(uint64_t) ||
                 sizeof(*this) == 12 * sizeof(uint64_t) + sizeof(size_t));

    d_seed = d_initialSeed ^ s_secret0;
}

inline
WyHashIncrementalAlgorithm::WyHashIncrementalAlgorithm(uint64_t seed)
: d_initialSeed(seed)
, d_last16AtEnd(false)
, d_totalLen(0)
{
    BSLMF_ASSERT(sizeof(d_initialSeed) == sizeof(seed));
    d_seed = d_initialSeed ^ s_secret0;
}

inline
WyHashIncrementalAlgorithm::WyHashIncrementalAlgorithm(const char *seed)
: d_last16AtEnd(false)
, d_totalLen(0)
{
    BSLMF_ASSERT(sizeof(d_initialSeed) == k_SEED_LENGTH);

    memcpy(&d_initialSeed, seed, sizeof(d_initialSeed));

    d_seed = d_initialSeed ^ s_secret0;
}

// MANIPULATORS
inline
void WyHashIncrementalAlgorithm::operator()(const void *data, size_t numBytes)
{
    BSLS_ASSERT_SAFE(0 != data || 0 == numBytes);

    const uint8_t *p   = static_cast<const uint8_t *>(data);
    const uint8_t *end = p + numBytes;

    const size_t   origLen = d_totalLen;
    size_t         repeatBufNumBytes = origLen % k_REPEAT_LENGTH;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!repeatBufNumBytes && origLen)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        repeatBufNumBytes = k_REPEAT_LENGTH;
    }

    d_totalLen = origLen + numBytes;

    if (0 != repeatBufNumBytes) {
        const ptrdiff_t remainingSpaceInBuf = k_REPEAT_LENGTH -
                                                             repeatBufNumBytes;
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(end - p <=
                                                        remainingSpaceInBuf)) {
            memcpy(repeatBufferBegin() + repeatBufNumBytes, p, end - p);

            // Leave 'd_last16AtEnd' alone.

            return;                                                   // RETURN
        }
        else {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            if (origLen <= k_REPEAT_LENGTH) {
                d_see1 = d_see2 = d_seed;
            }

            memcpy(repeatBufferBegin() + repeatBufNumBytes,
                   p,
                   remainingSpaceInBuf);

            p += remainingSpaceInBuf;
            process48ByteSection(repeatBufferBegin());

            d_last16AtEnd = true;
        }
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(k_REPEAT_LENGTH < end - p)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        if (0 == origLen) {
            d_see1 = d_see2 = d_seed;
        }

        do {
            process48ByteSection(p);
            p += k_REPEAT_LENGTH;
        } while (k_REPEAT_LENGTH < end - p);

        d_last16AtEnd = false;

        const ptrdiff_t remOffset = end - p;
        if (remOffset < 16) {
            BSLS_ASSERT_SAFE(0 < remOffset);

            // We say 'p + remOffset - 16' rather than 'end - 16' below because
            // the latter caused an inaccurate warning which could not be
            // silenced via pragmas.

            memcpy(prePadAt(remOffset), p + remOffset - 16, 16);

            return;                                                   // RETURN
        }
    }

    memcpy(repeatBufferBegin(), p, end - p);
}

inline
WyHashIncrementalAlgorithm::result_type
WyHashIncrementalAlgorithm::computeHash()
{
    BSLMF_ASSERT(sizeof(result_type) == sizeof(d_seed));

    uint8_t *p = repeatBufferBegin();

    uint64_t a, b, seed = d_seed;

    const size_t len = d_totalLen;
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(len <= 16)) {
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(len >= 4)) {
            const size_t subLen = (len >> 3) << 2;

            a = (_wyr4(p) << 32) | _wyr4(p + subLen);
            b = (_wyr4(p + len - 4) << 32) | _wyr4(p + len - 4 - subLen);
        }
        else if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(len > 0)) {
            a = _wyr3(p, len);
            b = 0;
        }
        else {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            a = b = 0;
        }
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        size_t totalLenRemainder = len % k_REPEAT_LENGTH;
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == totalLenRemainder)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            totalLenRemainder = k_REPEAT_LENGTH;
        }
        uint8_t *end = p + totalLenRemainder;

        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(k_REPEAT_LENGTH < len)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            seed ^= d_see1 ^ d_see2;
        }

        while (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(end - p > 16)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            seed = _wymix(_wyr8(p) ^ s_secret1, _wyr8(p + 8) ^ seed);
            p += 16;
        }

        const ptrdiff_t offset = end - repeatBufferBegin() - k_PREPAD_LENGTH;
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(offset < 0 &&
                                                              d_last16AtEnd)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            BSLS_ASSERT_SAFE(k_REPEAT_LENGTH < len);
            memcpy(repeatBufferBegin() + offset,
                   repeatBufferEnd() + offset,
                   -offset);
        }

        a = _wyr8(end - 16);
        b = _wyr8(end - 8);
    }

    return d_initialSeed ^ _wymix(s_secret1 ^ len,
                                              _wymix(a ^ s_secret1, b ^ seed));
}

}  // close package namespace
}  // close enterprise namespace

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace bsl {
template <>
struct is_trivially_copyable<
                     BloombergLP::bslh::WyHashIncrementalAlgorithm> : true_type
{};
}  // close namespace bsl

#undef BSLH_WYHASHINCREMENTALALGORITHM_WYMUM_PSEUDO_MULTIPLY
#undef BSLH_WYHASHINCREMENTALALGORITHM_WYMUM_XOR

#endif

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
