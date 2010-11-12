// bdeu_random.t.cpp                  -*-C++-*-

#include <bdeu_random.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
namespace {
int testStatus = 0;

void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
} // closed unnamed namespace

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
                          << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\n";           \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J    \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\t" << #M << ": " << M << "\n";         \
               aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J     \
                         << "\t" << #K << ": " << K << "\t" << #L << ": "  \
                         << L << "\t" << #M << ": " << M << "\t" << #N     \
                         << ": " << N << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl; // Print ID and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;  // Quote ID literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush; // P(X) no nl
#define L_ __LINE__                                // current Line number
#define T_ bsl::cout << "\t" << bsl::flush;        // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------
class Die
{
    int d_seed;  // current state, used to generate next role of this die

  public:
    // CREATORS
    Die(int initialSeed);
      // Create an object used to simulate a single die, using the
      // specified 'initialSeed'.

    // MANIPULATORS
    int roll();
      // Return the next pseudo-random value in the range [1 .. 6], based
      // on the sequence of values established by the initial seed value
      // supplied at construction.
};

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
        result = bdeu_Random::generate15(&d_seed) & 7;
    } while (result > 5);

    return result + 1;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) { case 0:  // Zero is always the leading case.
    case 3: {
      // --------------------------------------------------------------------
      // SEED TEST
      //
      // Concerns:
      //    * Given the same seed, the same random sequence is generated
      //    * Given a different seed, a different random sequence is generated
      //    * Both overloads of generate15 have the same behavior w.r.t. seed
      //
      // Plan: Generate three sequences of some significant number of random
      // values.  Let sequences 1 and 2 be generated starting with the same
      // seed, and sequence 3 be generated with a different seed.  Ensure that
      // sequences 1 and 2 are identical and different from sequence 3.
      // Repeat using the overload of generate15 that takes a seed by
      //
      // Testing:
      // --------------------------------------------------------------------
      if (verbose) bsl::cout << "SEED TEST\n"
                             << "=========" << bsl::endl;

      {
          if (veryVerbose) {
              bsl::cout << "overload 1\n"
                        << "=========" << bsl::endl;
          }

          int sequence1[15], sequence2[15], sequence3[15];

          int seed1 = 321, seed2 = 321, seed3 = 4321;
          for (int i = 0; i < 15; ++i) {
              sequence1[i] = bdeu_Random::generate15(&seed1, seed1);
              sequence2[i] = bdeu_Random::generate15(&seed2, seed2);
              sequence3[i] = bdeu_Random::generate15(&seed3, seed3);
          }
          ASSERT(0 == bsl::memcmp(sequence1, sequence2, sizeof(sequence1)));
          ASSERT(0 != bsl::memcmp(sequence2, sequence3, sizeof(sequence2)));
      }

      {
          if (veryVerbose) {
              bsl::cout << "overload 2\n"
                        << "=========" << bsl::endl;
          }

          int sequence1[15], sequence2[15], sequence3[15];

          int seed1 = 555, seed2 = 555, seed3 = 556;
          for (int i = 0; i < 15; ++i) {
              sequence1[i] = bdeu_Random::generate15(&seed1);
              sequence2[i] = bdeu_Random::generate15(&seed2);
              sequence3[i] = bdeu_Random::generate15(&seed3);
          }
          ASSERT(0 == bsl::memcmp(sequence1, sequence2, sizeof(sequence1)));
          ASSERT(0 != bsl::memcmp(sequence2, sequence3, sizeof(sequence2)));
      }
    } break;

    case 2: {
      // --------------------------------------------------------------------
      // USAGE EXAMPLE TEST
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nUSAGE EXAMPLE TEST"
                             << "\n==================" << bsl::endl;

      enum { NUM_ITERATIONS = 6000 };
      Die die(123);

      int n[6] = {0};

      for (int i = 0; i<NUM_ITERATIONS; ++i) {
          int d = die.roll();
          ASSERT(d <= 6);
          ASSERT(d >= 1);

          ++n[d-1];
      }

      double expected = NUM_ITERATIONS/6;
      ASSERT(n[0] < (expected * 1.1)); ASSERT(n[0] > (expected * 0.9));
      ASSERT(n[1] < (expected * 1.1)); ASSERT(n[1] > (expected * 0.9));
      ASSERT(n[2] < (expected * 1.1)); ASSERT(n[2] > (expected * 0.9));
      ASSERT(n[3] < (expected * 1.1)); ASSERT(n[3] > (expected * 0.9));
      ASSERT(n[4] < (expected * 1.1)); ASSERT(n[4] > (expected * 0.9));
      ASSERT(n[5] < (expected * 1.1)); ASSERT(n[5] > (expected * 0.9));

    } break;
    case 1: {
      // --------------------------------------------------------------------
      // BREATHING TEST
      // --------------------------------------------------------------------

      if (verbose) bsl::cout << "\nBREATHING TEST"
                             << "\n==============" << bsl::endl;

      enum { NUM_ITERATIONS = 2000 };

      int seed = 0;
      int cnt = 0;

      for (int i = 0; i<NUM_ITERATIONS; ++i) {
          int rand = bdeu_Random::generate15(&seed);
          ASSERT(rand < 0x10000);
          ASSERT(rand >= 0);

          for (int b = 0; b < 15; ++b) {
              cnt += rand & 1;
              rand >>= 1;
          }
      }

      double expected = (NUM_ITERATIONS * 15) / 2;
      ASSERT(cnt < (expected * 1.1));
      ASSERT(cnt > (expected * 0.9));

    } break;
    default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
