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
// PCG (Permuted Congruential Generator) algorithm.  Operators `==` and `!=`
// provide the operational definition of value.
//
// Primary Manipulators:
//  - `seed`
//  - `generate`
//
// This mechanism class  provides a `seed` function that takes as parameters 2
// values: an `initState` and a `streamSelector`.
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

    /// Create an object used to simulate a single die, using the specified
    /// `initialState` and `streamSelector`.
    Die(bsl::uint64_t initialState, bsl::uint64_t streamSelector);

    // MANIPULATORS

    /// Return the next pseudo-random value in the range `[1 .. 6]`, based
    /// on the sequence of values established by the `initialState` and
    /// `streamSelector` values supplied at construction.
    int roll();
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
// ```
// Now, we can use our `Dice` class to get the random numbers needed to
// simulate a game of craps.  Note that the game of craps requires two dice.
//
// We can instantiate a single `Die` and role it twice,
// ```

    /// Create a single Die instance and print two pseudo-random values in
    /// the range `[1 .. 6]` to standard output.
    void rollOneDieTwice()
    {
        Die a(123, 456);

        int d1 = a.roll();
        int d2 = a.roll();

        cout << "d1 = " << d1 << ", d2 = " << d2 << endl;  // d1 = 3, d2 = 5
    }
// ```
// Alternatively, we could create two instances of `Die`, with separate initial
// states/sequences, and role each one once:
// ```

    /// Create two Die instances and print two pseudo-random values in the
    /// range `[1 .. 6]` to standard output.
    void rollTwoDice()
    {
        Die a(123, 123);
        Die b(456, 456);

        int d1 = a.roll();
        int d2 = b.roll();

        cout << "d1 = " << d1 << ", d2 = " << d2 << endl;  // d1 = 3, d2 = 1
    }
// ```
// Note that the specification of separate seeds is important to produce a
// proper distribution for our game.  If we had shared the seed value each die
// would always produce the same sequence of values as the other.
// ```

    /// Create two Die instances with the identical initial state and stream
    /// selector and print two pseudo-random values in the range `[1 .. 6]`
    /// to standard output.  The produced values should be the same.
    void shareStateAndSequence()
    {
        Die a(123, 456);  // BAD IDEA
        Die b(123, 456);  // BAD IDEA

        int d1 = a.roll();
        int d2 = b.roll();
        ASSERT(d2 == d1);
    }
// ```
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
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
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
        // 1. The implementation conforms to the reference PCG implementation.
        //
        // 2. Default constructor arguments behavior:
        //    `Obj() == Obj(0, 0)`.
        //
        // 3. Objects seeded with different values generate different output.
        //
        // 4. The `==` and `!=` are defined properly to compare the internal
        //    state of the RNG.
        //
        // Plan:
        // 1. Initialize an RNG from this library and a separate reference
        //    implementation and compare output.
        //
        // 2. Create an RNG with no arguments, and two arguments (the default
        //    `initState` and `streamSelector`), and ensure that they are
        //    equal.
        //
        // 3. Create two RNG instances with different seeds and ensure
        //    that they do not generate the same values at the same time.
        //
        // 4. Create two RNG instances with same seeds and different stream
        //    selectors and ensure that they do not generate the same values
        //    at the same time.
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
                2707161783u, 2068313097u, 3122475824u, 2211639955u,
                3215226955u, 3421331566u, 3217466285u, 2167406445u,
                3860803674u, 4181216144u, 853247742u,  499135993u,
                3984091174u, 941769757u,  731976663u,  475758987u,
                2721289578u, 2228905443u, 3470160530u, 2998992390u,
                2441179440u, 1442744599u, 1206460561u, 1214968473u,
                2984805051u, 3261196357u, 446402806u,  2036656260u,
                1597429668u, 518128941u,  2233071061u, 691883599u,
                1838127612u, 3275887881u, 2691487686u, 3828376787u,
                3792673776u, 1075531959u, 2398224190u, 3814187698u,
                2762927671u, 718553706u,  2635185812u, 1922090326u,
                1852782471u, 84684515u,   1339504387u, 3338618763u,
                1260167649u, 374663825u,  3439899378u, 4160699816u,
                2024913114u, 2701156396u, 2508740703u, 3657599091u,
                1723134838u, 241912730u,  1209430164u, 223923616u,
                2406627518u, 4154033139u, 132502308u,  2910379858u,
                355646068u,  508074466u,  1819664228u, 387832886u,
                4074297162u, 619108615u,  1199635762u, 2598322316u,
                2723497167u, 4022773560u, 446839380u,  3315678907u,
                4147768777u, 3282048506u, 1029575953u, 2948913147u,
                1061239646u, 1180748659u, 2298246975u, 3602830748u,
                1672721738u, 3598532062u, 590958475u,  322456388u,
                3716889276u, 4284064286u, 1001483646u, 1845611368u,
                3962861838u, 2993439405u, 3609446448u, 3755020234u,
                2117479073u, 2188374819u, 3415172021u, 2603755493u};

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
            cout << "Testing that `Obj() == Obj(0, 0)`."
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
