// bslh_spookyhashalgorithm.h                                         -*-C++-*-
#ifndef INCLUDED_BSLH_SPOOKYHASHALGORITHM
#define INCLUDED_BSLH_SPOOKYHASHALGORITHM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an implementation of the SpookyHash algorithm.
//
//@CLASSES:
//  bslh::SpookyHashAlgorithm: functor implementing the SpookyHash algorithm
//
//@SEE_ALSO: bslh_hash, bslh_spookyhashalgorithmimp
//
//@DESCRIPTION: 'bslh::SpookyHashAlgorithm' implements the SpookyHash algorithm
// by Bob Jenkins.  This algorithm is a general purpose algorithm that is known
// to quickly reach good avalanche performance and execute in time that is
// comparable to or faster than other industry standard algorithms such as
// CityHash.  It is a good default choice for hashing values in unordered
// associative containers.  For more information, see:
// http://burtleburtle.net/bob/hash/spooky.html
//
// This class satisfies the requirements for regular 'bslh' hashing algorithms
// and seeded 'bslh' hashing algorithms, defined in 'bslh_hash.h' and
// 'bslh_seededhash.h' respectively.  More information can be found in the
// package level documentation for 'bslh' (internal users can also find
// information here {TEAM BDE:USING MODULAR HASHING<GO>})
//
///Security
///--------
// In this context "security" refers to the ability of the algorithm to produce
// hashes that are not predictable by an attacker.  Security is a concern when
// an attacker may be able to provide malicious input into a hash table,
// thereby causing hashes to collide to buckets, which degrades performance.
// There are *no* security guarantees made by 'bslh::SpookyHashAlgorithm',
// meaning attackers may be able to engineer keys that will cause a Denial of
// Service (DoS) attack in hash tables using this algorithm.  Note that even if
// an attacker does not know the seed used to initialize this algorithm, they
// may still be able to produce keys that will cause a DoS attack in hash
// tables using this algorithm.  If security is required, an algorithm that
// documents better secure properties should be used, such as
// 'bslh::SipHashAlgorithm'.
//
///Speed
///-----
// This algorithm will compute a hash on the order of O(n) where 'n' is the
// length of the input data.  Note that this algorithm will produce hashes fast
// enough to be used to hash keys in a hash table.  It is quicker than
// specialized algorithms such as SipHash, but not as fast as hashing using the
// identity function.
//
///Hash Distribution
///-----------------
// Output hashes will be well distributed and will avalanche, which means
// changing one bit of the input will change approximately 50% of the output
// bits.  This will prevent similar values from funneling to the same hash or
// bucket.
//
///Hash Consistency
///----------------
// This hash algorithm is endian-specific.  It is designed for little-endian
// machines, however, it will run on big-endian machines.  On big-endian
// machines, the Performance and Security Guarantees still apply, however the
// hashes produced will be different from those produced by the canonical
// implementation.  The creator of this algorithm acknowledges this and says
// that the big-endian hashes are just as good as the little-endian ones.  It
// is not recommended to send hashes from 'bslh::SpookyHashAlgorihtm' over a
// network because of the differences in hashes across architectures.  It is
// also not recommended to write hashes from 'bslh::SpookyHashAlgorihtm' to any
// memory accessible by multiple machines.
//
///Subdivision-Invariance
///----------------------
// Note that this algorithm is *subdivision-invariant* (see
// {'bslh_hash'|Subdivision-Invariance}).
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
//      // This class template implements a hash table providing fast lookup of
//      // an external, non-owned, array of values of (template parameter)
//      // 'TYPE'.
//      //
//      // The (template parameter) 'TYPE' shall have a transitive, symmetric
//      // 'operator==' function.  There is no requirement that it have any
//      // kind of creator defined.
//      //
//      // The 'HASHER' template parameter type must be a functor with a method
//      // having the following signature:
//      //..
//      //  size_t operator()(TYPE)  const;
//      //                   -OR-
//      //  size_t operator()(const TYPE&) const;
//      //..
//      // and 'HASHER' shall have a publicly accessible default constructor
//      // and destructor.
//      //
//      // Note that this hash table has numerous simplifications because we
//      // know the size of the array and never have to resize the table.
//
//      // DATA
//      const TYPE       *d_values;             // Array of values table is to
//                                              // hold
//      size_t            d_numValues;          // Length of 'd_values'.
//      const TYPE      **d_bucketArray;        // Contains ptrs into
//                                              // 'd_values'
//      size_t            d_bucketArrayMask;    // Will always be '2^N - 1'.
//      HASHER            d_hasher;
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
//..
// Then, we define a 'Future' class, which holds a c-string 'name', char
// 'callMonth', and short 'callYear'.
//..
//  class Future {
//      // This class identifies a future contract.  It tracks the name, call
//      // month and year of the contract it represents, and allows equality
//      // comparison.
//
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
//      bool operator==(const Future& other) const
//          // Compare this to the specified 'other' object and return true if
//          // they are equal
//      {
//          return (!strcmp(d_name, other.d_name))  &&
//             d_callMonth == other.d_callMonth &&
//             d_callYear  == other.d_callYear;
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
//      // This struct is a  functor that will apply the SpookyHashAlgorithm to
//      // objects of type 'Future'.
//
//      size_t operator()(const Future& future) const
//          // Return the hash of the of the specified 'future'.  Note that
//          // this uses the 'SpookyHashAlgorithm' to quickly combine the
//          // attributes of 'Future' objects that are salient to hashing into
//          // a hash suitable for a hash table.
//      {
//          SpookyHashAlgorithm hash;
//
//          hash(future.getName(),  strlen(future.getName()));
//          hash(future.getMonth(), sizeof(char));
//          hash(future.getYear(),  sizeof(short));
//
//          return static_cast<size_t>(hash.computeHash());
//      }
//  };
//..
// Then, we want to actually use our hash table on 'Future' objects.  We create
// an array of 'Future's based on data that was originally from some external
// source:
//..
//      Future futures[] = { Future("Swiss Franc", 'F', 2014),
//                           Future("US Dollar", 'G', 2015),
//                           Future("Canadian Dollar", 'Z', 2014),
//                           Future("British Pound", 'M', 2015),
//                           Future("Deutsche Mark", 'X', 2016),
//                           Future("Eurodollar", 'Q', 2017)};
//      enum { NUM_FUTURES = sizeof futures / sizeof *futures };
//..
// Next, we create our HashTable 'hashTable'.  We pass the functor that we
// defined above as the second argument:
//..
//      HashTable<Future, HashFuture> hashTable(futures, NUM_FUTURES);
//..
// Now, we verify that each element in our array registers with count:
//..
//      for ( int i = 0; i < 6; ++i) {
//          assert(hashTable.contains(futures[i]));
//      }
//..
// Finally, we verify that futures not in our original array are correctly
// identified as not being in the set:
//..
//      assert(!hashTable.contains(Future("French Franc", 'N', 2019)));
//      assert(!hashTable.contains(Future("Swiss Franc", 'X', 2014)));
//      assert(!hashTable.contains(Future("US Dollar", 'F', 2014)));
//
//..

#include <bslscm_version.h>

#include <bslh_spookyhashalgorithmimp.h>

#include <bslmf_isbitwisemoveable.h>

#include <bsls_assert.h>
#include <bsls_byteorder.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <stddef.h>  // for 'size_t'
#include <string.h>  // for 'memcpy'

namespace BloombergLP {
namespace bslh {


                        // ===============================
                        // class bslh::SpookyHashAlgorithm
                        // ===============================

class SpookyHashAlgorithm {
    // This class wraps an implementation of the "SpookyHash" hash algorithm in
    // an interface that is usable in the modular hashing system in 'bslh'.

  private:
    // PRIVATE TYPES
    typedef bsls::Types::Uint64 Uint64;
        // Typedef for a 64-bit integer type used in the hashing algorithm.

    // DATA
    SpookyHashAlgorithmImp d_state;
        // Object that contains the actual implementation of the SpookHash
        // algorithm.

    // NOT IMPLEMENTED
    SpookyHashAlgorithm(const SpookyHashAlgorithm& original); // = delete;
        // Do not allow copy construction.

    SpookyHashAlgorithm& operator=(const SpookyHashAlgorithm& rhs);// = delete;
        // Do not allow assignment.

  public:
    // TYPES
    typedef bsls::Types::Uint64 result_type;
        // Typedef indicating the value type returned by this algorithm.


    // CONSTANTS
    enum { k_SEED_LENGTH = 16 }; // Seed length in bytes.

  private:
    // PRIVATE CLASS METHODS
    static
    Uint64 getSeed(const char *seed);
        // The specified 'seed' points to a possibly non-aligned 'Uint64',
        // return the value of that 'Uint64' without violating the host
        // machine's alignment requirements.

  public:
    // CREATORS
    SpookyHashAlgorithm();
        // Create a 'SpookyHashAlgorithm' using a default initial seed.

    explicit SpookyHashAlgorithm(const char *seed);
        // Create a 'bslh::SpookyHashAlgorithm', seeded with a 128-bit
        // ('k_SEED_LENGTH' bytes) seed pointed to by the specified 'seed'.
        // Each bit of the supplied seed will contribute to the final hash
        // produced by 'computeHash()'.  The behaviour is undefined unless
        // 'seed' points to at least 16 bytes of initialized memory.

    //! ~SpookyHashAlgorithm() = default;
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

// PRIVATE CLASS METHODS
inline
SpookyHashAlgorithm::Uint64 SpookyHashAlgorithm::getSeed(const char *seed)
{
    Uint64 ret;
    ::memcpy(&ret, seed, sizeof(ret));
    return ret;
}

// CREATORS
inline
SpookyHashAlgorithm::SpookyHashAlgorithm()
: d_state(1, 2)
{}

inline
SpookyHashAlgorithm::SpookyHashAlgorithm(const char *seed)
: d_state(getSeed(seed), getSeed(seed + sizeof(Uint64)))
{}

// MANIPULATORS
inline
void SpookyHashAlgorithm::operator()(const void *data, size_t numBytes)
{
    BSLS_ASSERT(0 != data || 0 == numBytes);

    d_state.update(data, numBytes);
}

inline
SpookyHashAlgorithm::result_type SpookyHashAlgorithm::computeHash()
{
    bsls::Types::Uint64 h1, h2;
    d_state.finalize(&h1, &h2);
    return h1;
}

}  // close package namespace

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace bslmf {
template <>
struct IsBitwiseMoveable<bslh::SpookyHashAlgorithm>
    : bsl::true_type {};
}  // close namespace bslmf

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
