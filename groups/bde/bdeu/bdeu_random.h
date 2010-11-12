// bdeu_random.h                                                      -*-C++-*-
#ifndef INCLUDED_BDEU_RANDOM
#define INCLUDED_BDEU_RANDOM

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a suite of procedures for random-number generation.
//
//@CLASSES:
// bdeu_Random: namespace for a suite of random-number generation procedures
//
//@SEE_ALSO:
//
//@AUTHOR: Vlad Kliatchko (vkliatch)
//
//@DESCRIPTION: This component provides a namespace for a suite of pure
// procedures used to generate random numbers efficiently over a specific
// range of values.  The seed (or current state) is maintained externally.
// Two variants of each random number generator are provided: one has
// a single [in/out] seed parameter, which is first used then updated; the
// other takes the current seed as an [input] parameter, and stores a new
// seed in an [output] parameter.
//
///Usage
///-----
// This example shows how one might use 'bdeu_Random' to create and use a class
// to simulate the roll of a single die in a game, such as craps, that uses
// dice.  First we create the 'Die' class itself:
//..
//  // die.h
//  // ...
//
//  class Die {
//      // DATA
//      int d_seed;  // current state, used to generate next role of this die
//
//    public:
//      // CREATORS
//      Die(int initialSeed);
//        // Create an object used to simulate a single die, using the
//        // specified 'initialSeed'.
//
//      // MANIPULATORS
//      int roll();
//        // Return the next pseudo-random value in the range '[1 .. 6]', based
//        // on the sequence of values established by the initial seed value
//        // supplied at construction.
//  };
//
//  // CREATORS
//  inline
//  Die::Die(int initialSeed)
//  : d_seed(initialSeed)
//  {
//  }
//
//  // MANIPULATORS
//  int Die::roll()
//  {
//      int result;
//
//      do {
//          result = bdeu_Random::generate15(&d_seed) & 7;
//      } while (result > 5);
//
//      return result + 1;
//  }
//..
// For a game such as craps, that requires two dice, we can either instantiate
// a single 'Die' and role it twice:
//..
//  Die a(123);
//
//  int d1 = a.roll();
//  int d2 = a.roll();
//
//  cout << "d1 = " << d1 << ", d2 = " << d2 << endl;  // d1 = 3, d2 = 5
//..
// Or we could instantiate two instances of 'Die', with separate initial
// seeds, and role each one once:
//..
//  Die a(123);
//  Die b(456);
//
//  int d1 = a.roll();
//  int d2 = b.roll();
//
//  cout << "d1 = " << d1 << ", d2 = " << d2 << endl;  // d1 = 3, d2 = 1
//..
// Had we specified the same seed for both instances, however, the two dice
// would always produce the same sequence of values:
//..
//  Die a(123);
//  Die b(123);
//
//  int d1 = a.roll();
//  int d2 = b.roll();
//
//  cout << "d1 = " << d1 << ", d2 = " << d2 << endl;  // d1 = 3, d2 = 3
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

                        // ==================
                        // struct bdeu_Random
                        // ==================

struct bdeu_Random {
    // This 'struct' provides a namespace for a suite of pure procedures used
    // for random-number generation.

    // CLASS METHODS
    static int generate15(int *nextSeed, int seed);
        // Return a 15-bit random number in the range '[ 0 .. 32,767 ]'
        // generated from the specified 'seed', and load into the specified
        // 'nextSeed' a value suitable for generating the next random number.

    static int generate15(int *seed);
        // Return a 15-bit random number in the range '[ 0 .. 32,767 ]'
        // generated from the specified 'seed', and load into 'seed' a value
        // suitable for generating the next random number.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ------------------
                        // struct bdeu_Random
                        // ------------------

// CLASS METHODS
inline
int bdeu_Random::generate15(int *nextSeed, int seed)
{
    BSLS_ASSERT_SAFE(nextSeed);

    unsigned int next = seed;

    next *= 1103515245;
    next += 12345;

    *nextSeed = next;

    return (next >> 16) & 0x7FFF;
}

inline
int bdeu_Random::generate15(int *seed)
{
    BSLS_ASSERT_SAFE(seed);

    return generate15(seed, *seed);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
