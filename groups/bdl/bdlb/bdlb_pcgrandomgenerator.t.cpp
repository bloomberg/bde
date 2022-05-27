// bdlb_pcgrandomgenerator.t.cpp                                      -*-C++-*-

#include <bdlb_pcgrandomgenerator.h>

#include <bslim_testutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstdint.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a random number generator based on the
// PCG (Permuted Congruential Generator) algorithm.  Operators '==' and '!='
// provide the operational definition of value.
//
// Primary Manipulators:
//: o 'seed'
//: o 'generate'
//
// This mechanism class  provides a 'seed' function that takes as parameters 2
// values: an 'initState' and a 'streamSelector'.
//
// ----------------------------------------------------------------------------
//
// CREATORS
// [1] PcgRandomGenerator();
// [1] PcgRandomGenerator(uint64_t init, uint64_t streamSelector);
//
// MANIPULATORS
// [1] seed(bsl::uint64_t initState, bsl::uint64_t streamSelector);
// [1] bsl::uint32_t generate();
//
// FREE OPERATORS
// [1] operator==(const PcgRandomGenerator&, const PcgRandomGenerator&);
// [1] operator!=(const PcgRandomGenerator&, const PcgRandomGenerator&);
// ----------------------------------------------------------------------------
// [2] USAGE EXAMPLE

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

#define ASSERT BSLIM_TESTUTIL_ASSERT
#define ASSERTV BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q BSLIM_TESTUTIL_Q    // Quote identifier literally.
#define P BSLIM_TESTUTIL_P    // Print identifier and value.
#define P_ BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLIM_TESTUTIL_L_  // current Line number

typedef bdlb::PcgRandomGenerator Obj;

namespace {

bool verbose             = false;
bool veryVerbose         = false;
bool veryVeryVerbose     = false;
bool veryVeryVeryVerbose = false;

                    // =========
                    // class Die
                    // =========

class Die {

    // DATA
    bdlb::PcgRandomGenerator d_pcg; // used to generate next role of this die

  public:
    // CREATORS
    Die(bsl::uint64_t initialState, bsl::uint64_t streamSelector);
        // Create an object used to simulate a single die, using the specified
        // 'initialState' and 'streamSelector'.

    // MANIPULATORS
    int roll();
        // Return the next pseudo-random value in the range '[1 .. 6]', based
        // on the sequence of values established by the 'initialState' and
        // 'streamSelector' values supplied at construction.
};

                    // ---------
                    // class Die
                    // ---------

// CREATORS
inline
Die::Die(bsl::uint64_t initialState, bsl::uint64_t streamSelector)
: d_pcg(initialState, streamSelector)
{
}

// MANIPULATORS
int Die::roll()
{
    int result;

    do {
        result = d_pcg.generate() & 7;
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
        // Create a single Die instance and print two pseudo-random values in
        // the range '[1 .. 6]' to standard output.
    {
        Die a(123, 456);

        int d1 = a.roll();
        int d2 = a.roll();

        cout << "d1 = " << d1 << ", d2 = " << d2 << endl;  // d1 = 3, d2 = 5
    }
//..
// Alternatively, we could create two instances of 'Die', with separate initial
// states/sequences, and role each one once:
//..
    void rollTwoDice()
        // Create two Die instances and print two pseudo-random values in the
        // range '[1 .. 6]' to standard output.
    {
        Die a(123, 123);
        Die b(456, 456);

        int d1 = a.roll();
        int d2 = b.roll();

        cout << "d1 = " << d1 << ", d2 = " << d2 << endl;  // d1 = 3, d2 = 1
    }
//..
// Note that the specification of separate seeds is important to produce a
// proper distribution for our game.  If we had shared the seed value each die
// would always produce the same sequence of values as the other.
//..
    void shareStateAndSequence()
        // Create two Die instances with the identical initial state and stream
        // selector and print two pseudo-random values in the range '[1 .. 6]'
        // to standard output.  The produced values should be the same.
    {
        Die a(123, 456);  // BAD IDEA
        Die b(123, 456);  // BAD IDEA

        int d1 = a.roll();
        int d2 = b.roll();
        ASSERT(d2 == d1);
    }
//..
}  // close unnamed namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
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
        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;
        rollOneDieTwice();
        rollTwoDice();
        shareStateAndSequence();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // SEED, GENERATE, AND EQUALITY-COMPARISON OPERATORS
        //
        // Concerns:
        //: 1 The implementation conforms to the reference PCG implementation.
        //:
        //: 2 Default constructor arguments behavior:
        //:   'Obj() == Obj(0, 0)'.
        //:
        //: 3 Objects seeded with different values generate different output.
        //:
        //: 4 The '==' and '!=' are defined properly to compare the internal
        //:   state of the RNG.
        //
        // Plan:
        //: 1 Initialize an RNG from this library and a separate reference
        //:   implementation and compare output.
        //:
        //: 2 Create an RNG with no arguments, and two arguments (the default
        //:   'initState' and 'streamSelector'), and ensure that they are
        //:   equal.
        //:
        //: 3 Create two RNG instances with different seeds and ensure
        //:   that they do not generate the same values at the same time.
        //:
        //: 4 Create two RNG instances with same seeds and different stream
        //:   selectors and ensure that they do not generate the same values
        //:   at the same time.
        //
        // Testing:
        //   PcgRandomGenerator();
        //   PcgRandomGenerator(uint64_t init, uint64_t streamSelector);
        //   seed(bsl::uint64_t initState, bsl::uint64_t streamSelector);
        //   bsl::uint32_t generate();
        //   operator==(const PcgRandomGenerator&, const PcgRandomGenerator&);
        //   operator!=(const PcgRandomGenerator&, const PcgRandomGenerator&);
        //
        // --------------------------------------------------------------------

        if (verbose)
            cout << "SEED, GENERATE, AND EQUALITY-COMPARISON OPERATORS" << endl
                 << "================================================="
                 << endl;
        {
            // we have previously generated values from the implementation
            // provided by the PCG website
            const uint64_t SAMPLE_SEED = 42u;
            const uint64_t SAMPLE_SEQ  = 54u;

            const uint32_t referenceValues[] = {
                2707161783, 2068313097, 3122475824, 2211639955, 3215226955,
                3421331566, 3217466285, 2167406445, 3860803674, 4181216144,
                853247742,  499135993,  3984091174, 941769757,  731976663,
                475758987,  2721289578, 2228905443, 3470160530, 2998992390,
                2441179440, 1442744599, 1206460561, 1214968473, 2984805051,
                3261196357, 446402806,  2036656260, 1597429668, 518128941,
                2233071061, 691883599,  1838127612, 3275887881, 2691487686,
                3828376787, 3792673776, 1075531959, 2398224190, 3814187698,
                2762927671, 718553706,  2635185812, 1922090326, 1852782471,
                84684515,   1339504387, 3338618763, 1260167649, 374663825,
                3439899378, 4160699816, 2024913114, 2701156396, 2508740703,
                3657599091, 1723134838, 241912730,  1209430164, 223923616,
                2406627518, 4154033139, 132502308,  2910379858, 355646068,
                508074466,  1819664228, 387832886,  4074297162, 619108615,
                1199635762, 2598322316, 2723497167, 4022773560, 446839380,
                3315678907, 4147768777, 3282048506, 1029575953, 2948913147,
                1061239646, 1180748659, 2298246975, 3602830748, 1672721738,
                3598532062, 590958475,  322456388,  3716889276, 4284064286,
                1001483646, 1845611368, 3962861838, 2993439405, 3609446448,
                3755020234, 2117479073, 2188374819, 3415172021, 2603755493};

            const int NUM_SAMPLES =
                sizeof referenceValues / sizeof referenceValues[0];

            bdlb::PcgRandomGenerator gen;
            gen.seed(SAMPLE_SEED, SAMPLE_SEQ);

            for (int i = 0; i < NUM_SAMPLES; ++i) {
                bsl::uint32_t randomInt = gen.generate();
                if (veryVerbose) {
                    T_ P_(i) P_(randomInt) P_(referenceValues[i])
                }

                ASSERT(randomInt == referenceValues[i]);
            }
        }
        if (verbose)
            cout << "Testing that 'Obj() == Obj(0, 0)'."
                 << endl;
        {
            ASSERT(Obj() == Obj(0, 0));
        }
        if (verbose)
            cout << "Testing that different seeds produce different sequences."
                 << endl;
        {
            Obj      g1, g2, g3;
            uint64_t initState      = 10;
            uint64_t streamSelector = 5;

            g1.seed(initState, streamSelector);
            g2.seed(initState + 1, streamSelector);
            g3.seed(initState, streamSelector);  // same as g1

            ASSERT(g1 != g2);
            ASSERT(g1 == g1);
            ASSERT(g1 == g3);

            const int NUM_CHECKS = 100;
            if (verbose)
                cout << "Checking uniqueness of " << NUM_CHECKS << " iteration"
                     << endl;

            for (int i = 0; i < NUM_CHECKS; ++i) {
                const uint32_t randomValue1 = g1.generate();
                const uint32_t randomValue2 = g2.generate();
                const uint32_t randomValue3 = g3.generate();

                if (veryVerbose) {
                    T_ P_(i) P_(randomValue1) P_(randomValue2) P_(randomValue3)
                }

                ASSERT(randomValue1 != randomValue2);
                ASSERT(randomValue1 == randomValue3);
                ASSERT(g1 != g2);
                ASSERT(g1 == g1);
                ASSERT(g1 == g3);
            }
        }
        if (verbose)
            cout << "Testing that same seeds with different stream selectors "
                    "produce different sequences."
                 << endl;
        {
            Obj      g1, g2;
            uint64_t initState      = 10;
            uint64_t streamSelector = 5;
            g1.seed(initState, streamSelector);
            g2.seed(initState, streamSelector + 1);

            ASSERT(g1 != g2);
            ASSERT(g1 == g1);

            const int NUM_CHECKS = 100;
            if (verbose)
                cout << "Checking uniqueness of " << NUM_CHECKS << " iteration"
                     << endl;

            for (int i = 0; i < NUM_CHECKS; ++i) {
                const uint32_t randomValue1 = g1.generate();
                const uint32_t randomValue2 = g2.generate();

                if (veryVerbose) {
                    T_ P_(i) P_(randomValue1) P_(randomValue2)
                }
                ASSERT(randomValue1 != randomValue2);
                ASSERT(g1 != g2);
                ASSERT(g1 == g1);
            }
        }
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
