// bdeimp_charhash.t.cpp                  -*-C++-*-

#include <bdeimp_charhash.h>

#include <bsls_platformutil.h>                  // for testing only
#include <bsls_stopwatch.h>                     // for testing only

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of a static member function
// (pure procedure) that computes a hash value for 'char's.  The general plan
// is that the method is tested against a set of tabulated test vectors.
//-----------------------------------------------------------------------------
// [ 1] static int hash(char value, int size);  CONCERN: value
// [ 2] static int hash(char value, int size);  CONCERN: distribution
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING HASH OPERATOR (DISTRIBUTION) :
        //   Verify the hash function generates a good distribution of return
        //   values over a wide range of 'size' values.  Specifically, verify
        //   that for all x such that 0 <= x < size, x * 2^k for a fixed 'k' is
        //   perfectly distributed (every hash value is returned exactly once).
        //
        // Plan:
        //   Perform a large number of hashes and verifying the distribution.
        //
        // Testing:
        //   static int hash(char value, int size);  CONCERN: distribution
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing 'hash'" << endl
                 << "==============" << endl;

        if (verbose) cout << "\nTesting 'hash(char value, int size)' "
                          << "distribution." << endl;
        {
            { // test distribution for varying sizes
                static const int DATA[] = { 1, 5, 23 };
                static const int NUM_DATA = sizeof DATA / sizeof *DATA;
                int i;
                for (int m = 1; m <= 4; m++) {
                    for (int si = 0; si < NUM_DATA; ++si) {
                        int size = DATA[si];
                        int *hit = new int[size];
                        for (i = 0; i < size; ++i) {
                            hit[i] = 0;
                        }
                        for (i = 0; i < m*size; ++i) {
                            ++hit[bdeimp_CharHash::hash((char)i, size)];
                        }
                        for (i = 0; i < size; ++i) {
                            LOOP4_ASSERT(m, size, i, hit[i], m == hit[i]);
                        }
                        delete [] hit;
                    }
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING HASH OPERATOR (VALUE):
        //   Verify the hash return value is constant across all platforms for
        //    a given input.
        //
        // Plan:
        //   Specifying a set of test vectors and verify the return value.
        //
        // Testing:
        //   static int hash(char value, int size);  CONCERN: value
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing 'hash'" << endl
                 << "==============" << endl;

        if (verbose) cout << "\nTesting 'hash(char value, int size)' "
                          << "return value." << endl;
        {
            static const struct {
                int  d_lineNum;     // source line number
                char d_value;       // value to hash
                int  d_size;        // size of target hash table
                int  d_exp;         // expected return value
            } DATA[] = {
                //line  value          size    exp
                //----  -------------  ------  ------
                { L_,              0,     23,      0 },
                { L_,             10,     23,     10 },
                { L_,      (char)246,     23,     16 },
                { L_,      (char)255,     23,      2 },
                { L_,              0,    257,      0 },
                { L_,             10,    257,     10 },
                { L_,             46,    257,     46 },
                { L_,            127,    257,    127 },
                { L_,      (char)128,    257,    128 },
                { L_,      (char)210,    257,    210 },
                { L_,      (char)246,    257,    246 },
                { L_,      (char)255,    257,    255 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int  LINE  = DATA[ti].d_lineNum;
                const char VALUE = DATA[ti].d_value;
                const int  SIZE  = DATA[ti].d_size;
                const int  EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(VALUE);
                    P_(SIZE);
                    P(EXP);
                    P(bdeimp_CharHash::hash(VALUE, SIZE));
                    cout << endl;
                }
                LOOP_ASSERT(LINE, EXP == bdeimp_CharHash::hash(VALUE, SIZE));
            }
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // TESTING HASH OPERATOR (TIMING) :
        //   Compare the timings to those of the more thorough bdeu_hashutil.
        //
        // Plan:
        //   Perform a large number of hashes and measure the timing by a
        //   'bsls_Stopwatch'.
        //
        // Testing:
        //   static int hash(char value, int size);  CONCERN: timing
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing 'hash'" << endl
                 << "==============" << endl;

        const int LENGTH = 1257;  // not a power of two
        enum { ITERATIONS = 1000000 }; // 1M

        unsigned int value = 0;
            bsls_Stopwatch timer;
            timer.start();
            int i;
            char *c = reinterpret_cast<char*>(&i);
            for (i = 0; i < ITERATIONS; ++i) {
                value += bdeimp_IntHash::hash(*c, LENGTH);
            }
            timer.stop();
            cout << "Hashing 1M int values (in seconds): "
                 << timer.elapsedTime()
                 << endl;

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
