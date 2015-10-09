// bdlb_random.t.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlb_random.h>

#include <bslim_testutil.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test is a utility that defines two overloads of a
// function.  One overload is more general and is used to implement the other.
// The more general overload is tested first, albeit in within the same test
// case as the other.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] static int generate15(int *nextSeed, int seed);
// [ 2] static int generate15(int *seed);
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 1] BREATHING TEST

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

static bool         verbose = false;
static bool     veryVerbose = false;
static bool veryVeryVerbose = false;

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------
///Usage
///-----
//
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
                        // =========
                        // class Die
                        // =========

    class Die {

        // DATA
        int d_seed;  // current state, used to generate next role of this die

      public:
        // CREATORS
        Die(int initialSeed);
            // Create an object used to simulate a single die, using the
            // specified 'initialSeed'.

        // MANIPULATORS
        int roll();
            // Return the next pseudo-random value in the range '[1 .. 6]',
            // based on the sequence of values established by the initial seed
            // value supplied at construction.
    };

                        // ---------
                        // class Die
                        // ---------

    // CREATORS
    inline
    Die::Die(int initialSeed)
    : d_seed(initialSeed)
    {
    }

    // MANIPULATORS
    int Die::roll()
    {
        int result;

        do {
            result = bdlb::Random::generate15(&d_seed) & 7;
        } while (result > 5);

        return result + 1;
    }
//..
// Now, we can use our 'Dice' class to get the random numbers needed to
// simulate a game of craps.  Note that the game of craps requires two dice.
//
// We can instantiate a single 'Die' and role it twice,
//..
    void rollOneDieTwice()
    {
        Die a(123);

        int d1 = a.roll();
        int d2 = a.roll();

        cout << "d1 = " << d1 << ", d2 = " << d2 << endl;  // d1 = 3, d2 = 5
    }
//..
// Alternatively, we could create two instances of 'Die', with separate initial
// seeds, and role each one once:
//..
    void rollTwoDice()
    {
        Die a(123);
        Die b(456);

        int d1 = a.roll();
        int d2 = b.roll();

        cout << "d1 = " << d1 << ", d2 = " << d2 << endl;  // d1 = 3, d2 = 1
    }
//..
// Note that the specification of separate seeds is important to produce a
// proper distribution for our game.  If we had shared the seed value each die
// would always produce the same sequence of values as the other.
//..
    void shareSeed()
    {
        Die a(123);  // BAD IDEA
        Die b(123);  // BAD IDEA

        int d1 = a.roll();
        int d2 = b.roll();
        ASSERT(d2 == d1);

    }
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? bsl::atoi(argv[1]) : 0;
            verbose = argc > 2;
        veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "USAGE EXAMPLE" "\n"
                                  "=============" "\n";

        rollOneDieTwice();
        rollTwoDice();
        shareSeed();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // SEED TEST
        //
        // Concerns:
        //: 1 Given the same seed, the same random sequence is generated.
        //: 2 Given a different seed, a different random sequence is generated.
        //: 3 Both overloads of 'generate15' have the same behavior with
        //:   respect to seed.
        //
        // Plan:
        //: 1 Generate three sequences of some significant number of random
        //:   values.
        //: 2 Let sequences 1 and 2 be generated starting with the same seed,
        //:   and sequence 3 be generated with a different seed.
        //: 3 Ensure that sequences 1 and 2 are identical and different from
        //:   sequence 3.
        //: 4 Repeat using the second overload of 'generate15'.
        //
        // Testing:
        //   static int generate15(int *nextSeed, int seed);
        //   static int generate15(int *seed);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "SEED TEST" "\n"
                                  "=========" "\n";

        if (veryVerbose) { cout << "\noverload 1\n" << bsl::endl; }
        {
            int sequence1[15], sequence2[15], sequence3[15];

            int seed1 = 321, seed2 = 321, seed3 = 4321;

            for (int i = 0; i < 15; ++i) {

                if (veryVerbose) { T_ P_(seed1) P_(seed2) P(seed3) }

                sequence1[i] = bdlb::Random::generate15(&seed1, seed1);
                sequence2[i] = bdlb::Random::generate15(&seed2, seed2);
                sequence3[i] = bdlb::Random::generate15(&seed3, seed3);
            }
            ASSERT(0 == bsl::memcmp(sequence1, sequence2, sizeof(sequence1)));
            ASSERT(0 != bsl::memcmp(sequence2, sequence3, sizeof(sequence2)));
        }

        if (veryVerbose) { cout << "\noverload 2" << bsl::endl; }
        {
            int sequence1[15], sequence2[15], sequence3[15];

            int seed1 = 555, seed2 = 555, seed3 = 556;
            for (int i = 0; i < 15; ++i) {

                if (veryVerbose) { T_ P_(seed1) P_(seed2) P(seed3) }

                sequence1[i] = bdlb::Random::generate15(&seed1);
                sequence2[i] = bdlb::Random::generate15(&seed2);
                sequence3[i] = bdlb::Random::generate15(&seed3);
            }
            ASSERT(0 == bsl::memcmp(sequence1, sequence2, sizeof(sequence1)));
            ASSERT(0 != bsl::memcmp(sequence2, sequence3, sizeof(sequence2)));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Confirm that the distribution of 0- and 1- bits in a sequence
        //:   generated by 'generate15' is roughly equal.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "BREATHING TEST" "\n"
                                  "==============" "\n";

        enum { NUM_ITERATIONS = 2000 };

        int seed = 0;
        int cnt  = 0;

        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            int rand = bdlb::Random::generate15(&seed);
            ASSERT(rand < 0x10000);
            ASSERT(rand >= 0);

            if (veryVerbose) {
                if (0 == i % 100) {
                    T_ P_(i) P_(seed) P(rand);
                }
            }

            for (int b = 0; b < 15; ++b) {
                cnt   += rand & 1;
                rand >>= 1;
            }
        }

        double expected = (NUM_ITERATIONS * 15) / 2;
        ASSERT(cnt < (expected * 1.1));
        ASSERT(cnt > (expected * 0.9));

      } break;
      default: {
          cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
          testStatus = -1;
        }
      }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
