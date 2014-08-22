// bslh_seededhash.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLH_SEEDEDHASH
#define INCLUDED_BSLH_SEEDEDHASH

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a struct to run seeded 'bslh' hash algorithms on types.
//
//@CLASSES:
//  bslh::SeededHash: Functor that runs seeded 'bslh' hash algorithms on types
//
//@SEE_ALSO: bslh_hash, bslh_seedgenerator
//
//@DESCRIPTION: This component provides a templated 'struct',
// 'bslh::SeededHash', that defines a hash-functor that can be used with
// standard containers (a drop in replacement for 'bsl::hash'), and which
// applies the supplied (template parameter) 'HASH_ALGORITHM' to the attributes
// of the (template parameter) 'TYPE' which have been identified as salient to
// hashing.  The 'bslh::SeededHash' template parameter 'HASH_ALGORITHM' must be
// a hashing algorithm that conforms the the requirements outlined below (see
// {'Requirements for Seeded 'bslh' Hashing Algorithms'}).  Note that there are
// several hashing algorithms defined in 'bslh', some of which do not accept
// seeds, meaning they cannot be used with 'bslh::SeededHash'.
//
// 'bslh::SeededHash' will use the (template parameter) 'SEED_GENERATOR' to
// generate the seed used to instantiate the 'HASH_ALGORITHM'.  The
// 'bslh::SeededHash' template parameter 'SEED_GENERATOR' must be a seed
// generator that conforms the the requirements outlined below (see
// {'Requirements on (template parameter) Type 'SEED_GENERATOR''}).  The seed
// will be generated once upon construction of 'bslh::SeededHash' and then held
// until destruction.
//
// A call to 'bslh::Hash::operator()' for a (template parameter) 'TYPE' will
// call the 'hashAppend' free function for 'TYPE' and provide 'hashAppend' an
// instance of the 'HASH_ALGORITHM' which has been constructed using the stored
// seed.  Clients are expected to define a free-function 'hashAppend' for each
// of the types they wish to be hashable (see bslh_hash.h for details on
// 'hashAppend').   More information can be found in the package level
// documentation for 'bslh' (internal users can also find information here
// {TEAM BDE:USING MODULAR HASHING<GO>})
//
///Relationship to 'bslh::Hash'
/// - - - - - - - - - - - - - -
// 'bslh::SeededHash' is substantially similar to 'bslh::Hash'.
// 'bslh::SeededHash' presents a similar interface to that of 'bslh::Hash',
// however, it adds a constructor that accepts a seed generator.  Because of
// the use of seeds, 'bslh::SeededHash' stores data and therefor does not allow
// the empty base optimization like 'bslh::Hash' does.
//
///Requirements for Seeded 'bslh' Hashing Algorithms
///-------------------------------------------------
// Users of this modular hashing system are free write their own hashing
// algorithms.  In order to plug into 'bslh::SeededHash', the user-implemented
// algorithms must meet the requirements for regular 'bslh' hashing algorithms
// defined in bslh_hash.h, with the exception that a default constructor is not
// required.  The user-implemented algorithm must also implement the interface
// shown here:
//..
// class SomeHashAlgorithm
// {
//   public:
// //     // CONSTANTS
//     enum { k_SEED_LENGTH = XXX };
//
//     // CREATORS
//     explicit SomeHashAlgorithm(const char *seed);
// };
//..
// The 'k_SEED_LENGTH' enum must be in the public interface, and 'XXX' must be
// replaced with an integer literal indicating the number of bytes of seed the
// algorithm requires.  The parameterized constructor must accept a
// 'const char *'.  This pointer will point to a seed of 'XXX' bytes in size.
//
///Requirements on (template parameter) Type 'SEED_GENERATOR'
///----------------------------------------------------------
// Users are free to write their own seed generator, which can be supplied to
// bslh::SeededHash.  The seed generator must conform to the interface shown
// here:
//..
// class SomeSeedGenerator
// {
//     // ACCESSORS
//     void generateSeed(char *seedLocation, size_t seedLength);
// };
//..
// The only mandatory piece of the seed generator interface is the generateSeed
// method, which accepts a char pointer to memory to be written and a size_t
// length in bytes.  The generateSeed method must fill the size_t bytes of the
// memory pointed to by the char pointer with a seed.  The seed generator must
// be meet one of the two requirements:
//
//: A The seed generator is default constructible.
//:
//: B The seed generator is constructable through any method (default or via
//:   parameterized constructor) and ALSO copy constructible.
//
// Option A is prefered because it allows 'bslh::SeededHash' to be defualt
// constructible. Option B is allowed, but means that 'bslh::SeededHash' must
// be passed an already-instantiated 'SEED_GENERATOR' at construction.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLH_DEFAULTSEEDEDHASHALGORITHM
#include <bslh_defaultseededhashalgorithm.h>
#endif

#ifndef INCLUDED_BSLH_HASH
#include <bslh_hash.h>
#endif

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>  // for 'size_t'
#define INCLUDED_STDDEF_H
#endif

namespace BloombergLP {

namespace bslh {

                        // ======================
                        // class bslh::SeededHash
                        // ======================

template <class SEED_GENERATOR, class HASH_ALGORITHM
                                            = bslh::DefaultSeededHashAlgorithm>
struct SeededHash {
    // This class wraps the (template parameter) 'HASH_ALGORITHM', which
    // requires a seed, in an interface that satisfies the 'hash' requirements
    // of the C++11 standard. The (template parameter) type 'SEED_GENERATOR'
    // will be used to generate the seed required by 'HASH_ALGORITHM'.

  private:
    // DATA
    char seed[HASH_ALGORITHM::k_SEED_LENGTH];
        // Stores the seed generated by the (template parameter) type
        // 'SEED_GENERATOR'.

  public:
    // TYPES
    typedef size_t result_type;
        // The type of the hash value that will be returned by the
        // function-call operator.

    // CREATORS
    SeededHash();
        // Create a 'bslh::SeededHash' which will default construct the
        // (template parameter) type 'SEED_GENERATOR' to initialize the seed
        // that will be passed to the (template parameter) type
        // 'HASH_ALGORITHM' when it is used.  'SEED_GENERATOR' must be default
        // constructible to use this constructor.

    explicit SeededHash(SEED_GENERATOR& seedGenerator);
        // Create a 'bslh::SeededHash' which will use the specified
        // 'seedGenerator' to initialize the seed that will be passed to the
        // (template parameter) type 'HASH_ALGORITHM' when it is used.
        // 'SEED_GENERATOR' must be copy-constructible to use this constructor.

    //! SeededHash(const SeededHash& original) = default;
        // Create a 'SeededHash' object having the same internal state as the
        // specified 'original'.

    //! ~SeededHash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! SeededHash& operator=(const SeededHash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    // ACCESSORS
    template <class TYPE>
    result_type operator()(const TYPE& type) const;
        // Returns a hash generated by the (template parameter) type
        // 'HASH_ALGORITHM' for the specified 'type'.  The value returned by
        // the 'HASH_ALGORITHM' is cast to 'size_t' before returning.

};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// CREATORS
template <class SEED_GENERATOR, class HASH_ALGORITHM>
inline
SeededHash<SEED_GENERATOR, HASH_ALGORITHM>::SeededHash()
{
    SEED_GENERATOR seedGenerator = SEED_GENERATOR();
    seedGenerator.generateSeed(seed, HASH_ALGORITHM::k_SEED_LENGTH);
}

template <class SEED_GENERATOR, class HASH_ALGORITHM>
inline
SeededHash<SEED_GENERATOR, HASH_ALGORITHM>::SeededHash(
                                                 SEED_GENERATOR& seedGenerator)
{
    seedGenerator.generateSeed(seed, HASH_ALGORITHM::k_SEED_LENGTH);
}

// ACCESSORS
template <class SEED_GENERATOR, class HASH_ALGORITHM>
template <class TYPE>
inline
typename SeededHash<SEED_GENERATOR, HASH_ALGORITHM>::result_type
SeededHash<SEED_GENERATOR, HASH_ALGORITHM>::operator()(TYPE const& key) const
{
    HASH_ALGORITHM hashAlg(seed);
    hashAppend(hashAlg, key);
    return static_cast<result_type>(hashAlg.computeHash());
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
