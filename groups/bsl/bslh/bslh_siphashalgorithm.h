// bslh_siphashalgorithm.h                                            -*-C++-*-
#ifndef INCLUDED_BSLH_SIPHASHALGORITHM
#define INCLUDED_BSLH_SIPHASHALGORITHM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an implementation of the SipHash algorithm.
//
//@CLASSES:
//  bslh::SipHashAlgorithm: functor implementing the SipHash algorithm
//
//@SEE_ALSO: bslh_hash
//
//@DESCRIPTION: 'bslh::SipHashAlgorithm' implements the SipHash algorithm.
// SipHash is an algorithm designed for speed and security.  A primary use case
// for this algorithm is to provide an extra line of defense in hash tables
// (such as the hash table that is used to implement 'unordered_map') against
// malicious input that could cause Denial of Service (DoS) attacks.  It is
// based on one of the finalists for the SHA-3 cryptographic hash standard.
// Full details of the hash function can be found here:
// https://131002.net/siphash/siphash.pdf This particular implementation has
// been derived from Howard Hinnant's work here:
// https://github.com/HowardHinnant/hash_append/blob/master/siphash.h and as
// much of the original code as possible, including comment headers, has been
// preserved.
//
// This class satisfies the requirements for seeded 'bslh' hashing algorithms,
// defined in 'bslh_seededhash.h'.  More information can be found in the
// package level documentation for 'bslh' (internal users can also find
// information here {TEAM BDE:USING MODULAR HASHING<GO>})
//
///Security
///--------
// SipHash is *not* a cryptographically secure hash.  In the paper linked
// above, the creators of this hash function describe it as "Cryptographically
// Strong", but explicitly avoid calling it cryptographically secure.  In order
// to be cryptographically secure, and algorithm must, among other things,
// provide "collision resistance".  "Collision resistance" means that it should
// be difficult to find two different messages m1 and m2 such that
// 'hash(m1) == hash(m2)'.  Because of the limited sized output (only 2^64
// possibilities) and the fast execution time of the algorithm, it is feasible
// to find collisions by brute force, making the algorithm not
// cryptographically secure.
//
// SipHash *is*, however, a cryptographically strong PRF (pseudo-random
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
///Speed
///-----
// This algorithm is designed to be fast in comparison to other algorithms
// making similar guarantees.  It is still slower than other commonly accepted
// and used hashes such as SpookyHash.  This algorithm should only be used when
// protection from malicious input is required.  Otherwise, an algorithm that
// documents better performance properties should be used, such as
// 'bslh::SpookyHashAlgorithm'.
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
// This hash algorithm is endian-independent.  The hashes produced for a given
// 16-byte key sequence and given data will be the same on big-endian and
// little-endian platforms.  (In the literature the key is sometimes presented
// as a large integer or sequence of integers, such as
// '0xDEADBEEF, 0xCAFEBABE, 0x8BADF00D, 0x1BADB002', in which case care must be
// taken to supply the individual key bytes in the same order on both platforms
// if the same hash results are desired.)  However, if the "given data" is not
// just a character string but has internal structure, such as being integral
// or floating-point, it is likely ordered in different ways depending on the
// platform, and thus will not hash to the same value.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example: Creating and Using a Hash Table containing User Input
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have any array of types that define 'operator==', and we want a
// fast way to find out if values are contained in the array.  We can create a
// 'HashTable' data structure that is capable of looking up values in O(1)
// time.
//
// Further suppose that we will be storing arbitrary user input in our table.
// It is possible that an attacker with knowledge of the hashing algorithm we
// are using could specially craft input that will cause collisions in our hash
// table, degrading performance to O(n).  To avoid this we will need to use a
// secure hash algorithm with a random seed.  This algorithm will need to be in
// the form of a hash functor -- an object that will take objects stored in our
// array as input, and yield an integer value which is hard enough for an
// outside observer to predict that it appear random.  The functor can pass the
// attributes of the 'TYPE' that are salient to hashing into the hashing
// algorithm, and then return the hash that is produced.
//
// We can use the result of the hash function to index into our array of
// 'buckets'.  Each 'bucket' is simply a pointer to a value in our original
// array of 'TYPE' objects.
//
// First, we define our 'HashTable' template class, with the two type
// parameters: 'TYPE' (the type being referenced) and 'HASHER' (a functor that
// produces the hash).
//..
//
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
//      unsigned          d_bucketArrayMask;    // Will always be '2^N - 1'.
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
//
//..
// Then, we define a 'Future' class, which holds a cstring 'name', char
// 'callMonth', and short 'callYear'.  This class can be used to store custom
// futures that the users have uploaded.
//..
//
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
//
//..
// Next, we need a hash functor for 'Future'.  We are going to use the
// 'SipHashAlgorithm' because, it is a secure hash algorithm that will provide
// a way to securely combine the attributes of 'Future' objects that are
// salient to hashing into one reasonable hash that an malicious user will not
// be able to predict.
//..
//
//  struct HashFuture {
//      // This struct is a functor that will apply the SipHashAlgorithm to
//      // objects of type 'Future'.
//
//      size_t operator()(const Future& future) const
//          // Return the hash of the of the specified 'future'.  Note that
//          // this uses the 'SipHashAlgorithm' to safely combine the
//          // attributes of 'Future' objects that are salient to hashing into
//          // a hash that is not predictable by an attacker.
//      {
//          char seed[SipHashAlgorithm::k_SEED_LENGTH];
//          SeedGenerator<CryptoSecureRNG> seedGenerator;
//          seedGenerator.generateSeed(seed, SipHashAlgorithm::k_SEED_LENGTH);
//
//          SipHashAlgorithm hash(seed);
//
//          hash(future.getName(),  strlen(future.getName()));
//          hash(future.getMonth(), sizeof(char));
//          hash(future.getYear(),  sizeof(short));
//
//          return static_cast<size_t>(hash.computeHash());
//      }
//  };
//
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
//
// Next, we create our HashTable 'hashTable'.  We pass the functor that we
// defined above as the second argument:
//
//      HashTable<Future, HashFuture> hashTable(futures, NUM_FUTURES);
//..
// Now, we verify that each element in our array registers with count:
//..
//      for ( int i = 0; i < 6; ++i) {
//          ASSERT(hashTable.contains(futures[i]));
//      }
//..
// Finally, we verify that futures not in our original array are correctly
// identified as not being in the set:
//..
//      ASSERT(!hashTable.contains(Future("French Franc", 'N', 2019)));
//      ASSERT(!hashTable.contains(Future("Swiss Franc", 'X', 2014)));
//      ASSERT(!hashTable.contains(Future("US Dollar", 'F', 2014)));
//..
//
///Changes
///-------
// The third party code begins with the 'siphash.h' header below, and continues
// until the TYPE TRAITS banner below.  Changes made to the original code
// include:
//
//: 1 Adding 'BloombergLP' and 'bslh' namespaces
//:
//: 2 Renaming 'siphash' to 'SipHashAlgorithm'
//:
//: 3 Whitespace changes for formatting
//:
//: 4 Added comments
//:
//: 5 Removed C++11 features including class member initializer, 'noexcept',
//:   'std::Uint64_t', explicit conversion operator, and an '= default'
//:   constructor.
//:
//: 6 Added 'typedef' to replace removed 'std::Uint64_t'
//:
//: 7 Added 'computeHash()' to replace the removed explicit conversion
//:
//: 8 Added 'k_SEED_LENGTH' and changed the constructor to accept a
//:   'const char *'
//:
//: 9 Included headers and added 'include' guards
//:
//: 10 Changed variables to use 'size_t' rather than 'unsigned int'
//
///Third-Party Documentation
///-------------------------
//------------------------------- siphash.h -----------------------------------
//
// This software is in the public domain.  The only restriction on its use is
// that no one can remove it from the public domain by claiming ownership of
// it, including the original authors.
//
// There is no warranty of correctness on the software contained herein.  Use
// at your own risk.
//
// Derived from:
//
// SipHash reference C implementation
//
// Written in 2012 by Jean-Philippe Aumasson <jeanphilippe.aumasson@gmail.com>
// Daniel J.  Bernstein <djb@cr.yp.to>
//
// To the extent possible under law, the author(s) have dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide.  This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along
// with this software.  If not, see
// <http://creativecommons.org/publicdomain/zero/1.0/>.
//
//-----------------------------------------------------------------------------

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>  // for 'size_t'
#define INCLUDED_STDDEF_H
#endif


namespace BloombergLP {

namespace bslh {

                          // ============================
                          // class bslh::SipHashAlgorithm
                          // ============================

class SipHashAlgorithm {
    // This class wraps an implementation of the "SipHash" algorithm in an
    // interface that is usable in the modular hashing system in 'bslh'.

  private:
    // PRIVATE TYPES
    typedef bsls::Types::Uint64 Uint64;
        // Typedef for a 64-bit integer type used in the hashing algorithm.

    // DATA
    Uint64 d_v0;
    Uint64 d_v1;
    Uint64 d_v2;
    Uint64 d_v3;
        // Stores the intermediate state of the algorithm as values are
        // accumulated

    union {
        Uint64 d_alignment;
            // Provides alignment
        unsigned char d_buf [8];
            // Used to buffer data until we have enough to do a full round of
            // computation as specified by the algorithm.
    };

    size_t d_bufSize;
        // The length of the data currently stored in the buffer.

    size_t d_totalLength;
        // The total length of all data that has been passed into the
        // algorithm.

    // NOT IMPLEMENTED
    SipHashAlgorithm(const SipHashAlgorithm& original); // = delete;
        // Do not allow copy construction.

    SipHashAlgorithm& operator=(const SipHashAlgorithm& rhs); // = delete;
        // Do not allow assignment.

  public:
    // TYPES
    typedef Uint64 result_type;
        // Typedef indicating the value type returned by this algorithm.

    // CONSTANTS
    enum { k_SEED_LENGTH = 16 }; // Seed length in bytes.

    // CREATORS
    explicit SipHashAlgorithm(const char *seed);
        // Create a 'bslh::SipHashAlgorithm', seeded with a 128-bit
        // ('k_SEED_LENGTH' bytes) seed pointed to by the specified 'seed'.
        // Each bit of the supplied seed will contribute to the final hash
        // produced by 'computeHash()'.  The behaviour is undefined unless
        // 'seed' points to at least 16 bytes of initialized memory.  Note that
        // if data in 'seed' is not random, all guarantees of security and
        // Denial of Service (DoS) protection are void.

    //! ~SipHashAlgorithm() = default;
        // Destroy this object.

    // MANIPULATORS
    void operator()(const void *data, size_t numBytes);
        // Incorporate the specified 'data', of at least the specified
        // 'numBytes', into the internal state of the hashing algorithm.  Every
        // bit of data incorporated into the internal state of the algorithm
        // will contribute to the final hash produced by 'computeHash()'.  The
        // same hash will be produced regardless of whether a sequence of bytes
        // is passed in all at once or through multiple calls to this member
        // function.  Input where 'numBytes' is 0 will have no effect on the
        // internal state of the algorithm.  The behaviour is undefined unless
        // 'data' points to a valid memory location with at least 'numBytes'
        // bytes of initialized memory.

    result_type computeHash();
        // Return the finalized version of the hash that has been accumulated.
        // Note that this changes the internal state of the object, so calling
        // 'computeHash()' multiple times in a row will return different
        // results, and only the first result returned will match the expected
        // result of the algorithm.  Also note that a value will be returned,
        // even if data has not been passed into 'operator()'
};

}  // close package namespace

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace bslmf {
template <>
struct IsBitwiseMoveable<bslh::SipHashAlgorithm>
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
