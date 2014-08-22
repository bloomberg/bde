// bslh_seedgenerator.h                                               -*-C++-*-
#ifndef INCLUDED_BSLH_SEEDGENERATOR
#define INCLUDED_BSLH_SEEDGENERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class to generate arbitrary length seeds for algorithms.
//
//@CLASSES:
//  bslh::SeedGenerator: Generator for arbitrary length seeds
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a class, 'bslh::SeedGenerator', which
// utilizes a user-supplied random number generator (RNG) to generate arbitrary
// length seeds.  The quality of the seeds will only be as good as the quality
// of the supplied RNG.  A cryptographically secure RNG must be supplied in
// order for 'SeedGenerator' to produce seeds suitable for a cryptographically
// secure algorithm.
//
// This class satisfies the requirements for a seed generator, defined in
// bslh_seededhash.h.  More information can be found in the package level
// documentation for 'bslh' (internal users can also find information here
// {TEAM BDE:USING MODULAR HASHING<GO>})
//
///Requirements on RNG
///-------------------
// The (template parameter) type 'RNG' shall be a class exposing 'operator()'
// which returns a result of type 'result_type', which will also be and
// publicly accessible in 'RNG'.  The value returned by 'operator()' shall be
// random bits, the quality of which can be defined by 'RNG'. 'RNG' shall also
// be default and copy constructible.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_STRING_H
#include <string.h>  // for 'memcpy'
#define INCLUDED_STRING_H
#endif

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>  // for 'size_t'
#define INCLUDED_STDDEF_H
#endif

namespace BloombergLP {

namespace bslh {

                        // =========================
                        // class bslh::SeedGenerator
                        // =========================
template<class RNG>
class SeedGenerator : private RNG
{
    // This class template implements a seed generator which takes a user
    // supplied random number generator and uses it to generate an arbitrary
    // length seed.

  private:
    // PRIVATE TYPES
    typedef typename RNG::result_type result_type;
        // 'result_type' is an alias for the value returned by a call to
        // 'operator()' on the (template parameter) type 'RNG'.

    // DATA
    enum { k_RNGOUTPUTSIZE =  sizeof(typename RNG::result_type)};
        // Size in bytes of the rng's output.

  public:
    // CREATORS
    SeedGenerator();
        // Create a 'bslh::SeedGenerator' that will default construct the
        // parameterized 'RNG' and use it to generate its seeds.

    explicit SeedGenerator(const RNG &randomNumberGenerator);
        // Create a 'bslh::SeedGenerator' that will use the specified
        // 'randomNumberGenerator' to generate its seeds.

    //! SeedGenerator(const SeedGenerator& original) = default;
        // Create a 'SeedGenerator' object with a copy of the random number
        // generator used by the specified 'original'.

    //! ~SeedGenerator() = default;
        // Destroy this object.

    // MANIPULATORS
    //! SeedGenerator& operator=(const SeedGenerator& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void generateSeed(char *seedLocation, size_t seedLength);
        // Generate a seed of the specified 'seedLength' bytes and store it at
        // the specified 'seedLocation'.  The seed will be generated with bytes
        // from the random number generator supplied at construction.  All of
        // the returned bytes will come from the RNG, meaning if the requested
        // seed is larger than the return type of the RNG, the RNG will be
        // called multiple times.  The behaviour is undefined unless the memory
        // at 'seedLocation' can store 'seedLength' bytes.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// CREATORS
template<class RNG>
inline
SeedGenerator<RNG>::SeedGenerator()
: RNG()
{
}

template<class RNG>
inline
SeedGenerator<RNG>::SeedGenerator(const RNG &randomNumberGenerator)
: RNG(randomNumberGenerator)
{
}

// MANIPULATORS
template<class RNG>
inline
void SeedGenerator<RNG>::generateSeed(char *seedLocation, size_t seedLength)
{
    BSLS_ASSERT(seedLocation || !seedLength);

    size_t numChunks = seedLength / k_RNGOUTPUTSIZE;
    size_t remainder = seedLength % k_RNGOUTPUTSIZE;

#if defined(BSLS_PLATFORM_OS_SOLARIS)
    for (size_t i = 0; i != numChunks; ++i) {
        result_type rand = RNG::operator()();
        memcpy(seedLocation + i * sizeof(rand), &rand, sizeof(rand));
    }

    if (remainder) {
        result_type rand = RNG::operator()();
        memcpy(seedLocation + numChunks * sizeof(rand), &rand, remainder);
    }
#else
    char  *chunkEnd  = seedLocation + numChunks * k_RNGOUTPUTSIZE;

    for (; seedLocation != chunkEnd; seedLocation += k_RNGOUTPUTSIZE) {
        *(reinterpret_cast<result_type *>(seedLocation)) = RNG::operator()();
    }

    if (remainder) {
        result_type randomBytes = RNG::operator()();
        memcpy(seedLocation, &randomBytes, remainder);
    }
#endif
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
