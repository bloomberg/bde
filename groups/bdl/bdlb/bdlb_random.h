// bdlb_random.h                                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLB_RANDOM
#define INCLUDED_BDLB_RANDOM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a suite of procedures for random-number generation.
//
//@CLASSES:
//  bdlb::Random: namespace for a suite of random-number generation procedures
//
//@DESCRIPTION: This component provides a utility 'struct', 'bdlb::Random',
// that is a namespace for a suite of function used to efficiently generate
// random numbers over a specific range of values.  The seed (or current state)
// is maintained externally.  Two variants of each random number generator are
// provided: one has a single [in/out] seed parameter, which is first used then
// updated; the other takes the current seed as an [input] parameter, and
// stores a new seed in an [output] parameter.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Simulating a Pair of Dice
/// - - - - - - - - - - - - - - - - - -
// This example shows how one might use 'bdlb::Random' to create and use a
// class to simulate the roll of a single die in a game, such as craps, that
// uses dice.
//
// First, we define the 'Die' class itself:
//..
//                      // =========
//                      // class Die
//                      // =========
//
//  class Die {
//
//      // DATA
//      int d_seed;  // current state, used to generate next role of this die
//
//    public:
//      // CREATORS
//      Die(int initialSeed);
//          // Create an object used to simulate a single die, using the
//          // specified 'initialSeed'.
//
//      // MANIPULATORS
//      int roll();
//          // Return the next pseudo-random value in the range '[1 .. 6]',
//          // based on the sequence of values established by the initial seed
//          // value supplied at construction.
//  };
//
//                      // ---------
//                      // class Die
//                      // ---------
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
//          result = bdlb::Random::generate15(&d_seed) & 7;
//      } while (result > 5);
//
//      return result + 1;
//  }
//..
// Now, we can use our 'Dice' class to get the random numbers needed to
// simulate a game of craps.  Note that the game of craps requires two dice.
//
// We can instantiate a single 'Die' and role it twice,
//..
//  void rollOneDieTwice()
//  {
//      Die a(123);
//
//      int d1 = a.roll();
//      int d2 = a.roll();
//
//      cout << "d1 = " << d1 << ", d2 = " << d2 << endl;  // d1 = 3, d2 = 5
//  }
//..
// Alternatively, we could create two instances of 'Die', with separate initial
// seeds, and role each one once:
//..
//  void rollTwoDice()
//  {
//      Die a(123);
//      Die b(456);
//
//      int d1 = a.roll();
//      int d2 = b.roll();
//
//      cout << "d1 = " << d1 << ", d2 = " << d2 << endl;  // d1 = 3, d2 = 1
//  }
//..
// Note that the specification of separate seeds is important to produce a
// proper distribution for our game.  If we had shared the seed value each die
// would always produce the same sequence of values as the other.
//..
//  void shareSeed()
//  {
//      Die a(123);  // BAD IDEA
//      Die b(123);  // BAD IDEA
//
//      int d1 = a.roll();
//      int d2 = b.roll();
//      assert(d2 == d1);
//
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace bdlb {
                               // =============
                               // struct Random
                               // =============

struct Random {
    // This 'struct' provides a namespace for a suite of functions used for
    // random-number generation.

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

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                               // -------------
                               // struct Random
                               // -------------

// CLASS METHODS
inline
int Random::generate15(int *nextSeed, int seed)
{
    BSLS_ASSERT_SAFE(nextSeed);

    unsigned int next = seed;

    next *= 1103515245;
    next += 12345;

    *nextSeed = next;

    return (next >> 16) & 0x7FFF;
}

inline
int Random::generate15(int *seed)
{
    BSLS_ASSERT_SAFE(seed);

    return generate15(seed, *seed);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
// ----------------------------- END-OF-FILE ----------------------------------
