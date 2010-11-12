// bdeimp_int64hash.t.cpp               -*-C++-*-

#include <bdeimp_int64hash.h>

#include <bsls_platformutil.h>                  // for testing only

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
// (pure procedure) that computes a hash value for 'int64's.  The general plan
// is that the method is tested against a set of tabulated test vectors.
//-----------------------------------------------------------------------------
// [ 1] static int hash(bsls_PlatformUtil::Int64, int);  CONCERN: value
// [ 2] static int hash(bsls_PlatformUtil::Int64, int);  CONCERN: distribution
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
        //   int hash(bsls_PlatformUtil::Int64, int);  CONCERN: distribution
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing 'hash'" << endl
                 << "==============" << endl;

        if (verbose) cout << "\nTesting 'hash(bsls_PlatformUtil::Int64 value, "
                          << "int size)' distribution." << endl;
        {
            { // test distribution for shifted bits
                const int size = 257;
                int *hit = new int[size];
                int i;
                for (i = 0; i < size; ++i) {
                    hit[i] = 0;
                }
                for (int k = 0; k <= 55; ++k) { // 64 - 9 bits for size of 257
                    for (i = 0; i < size; ++i) {
                        ++hit[bdeimp_Int64Hash::
                                 hash(bsls_PlatformUtil::Int64(i) << k, size)];
                    }
                    for (i = 0; i < size; ++i) {
                        LOOP2_ASSERT(k, i, k + 1 == hit[i]);
                    }
                }
                delete [] hit;
            }
            { // test distribution for varying sizes
                static const int DATA[] = { 1, 5, 23, 257, 65537 };
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
                            ++hit[bdeimp_Int64Hash::
                                      hash(bsls_PlatformUtil::Int64(i), size)];
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
        //   static int hash(bsls_PlatformUtil::Int64, int);  CONCERN: value
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing 'hash'" << endl
                 << "==============" << endl;

        if (verbose) cout << "\nTesting 'hash(int value, int size)' "
                          << "return value." << endl;
        {
            static const struct {
                int d_lineNum;  // source line number
                int d_valueA;   // first half of value to hash
                int d_valueB;   // second half of value to hash
                int d_size;     // size of hash table
                int d_exp;      // expected return value
            } DATA[] = {
                //line  valueA             valueB              size    exp
                //----  -----------------  -----------------  ------  ------
                { L_,                  0,                 0,    257,      0 },
                { L_,                 -1,                -1,    257,      0 },
                { L_,                  0,        2147483647,    257,    128 },
                { L_,                 -1,        0x80000000,    257,    129 },
                { L_,                  0,                 0,  65537,      0 },
                { L_,                 -1,                -1,  65537,      0 },
                { L_,                  0,        2147483647,  65537,  32768 },
                { L_,                 -1,        0x80000000,  65537,  32769 },
                { L_,         2147483647,                 0,  65537,  32768 },
                { L_,         0x12345678,                 0,  65537,  17476 },
                { L_,         0x12345678,        0x90ABCDEF,  65537,  63745 },
                { L_,         0x23453453,        0x33253452,  65537,  62008 },
                { L_,         0x99999999,        0x99999999,  65537,      0 },
                { L_,         0xFFFFFFFF,        0xFFFFFFFF,  65537,      0 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int                      LINE  = DATA[ti].d_lineNum;
                const int                      VA    = DATA[ti].d_valueA;
                const int                      VB    = DATA[ti].d_valueB;
                const int                      SIZE  = DATA[ti].d_size;
                const int                      EXP   = DATA[ti].d_exp;
                const bsls_PlatformUtil::Int64 VALUE =
                                     (bsls_PlatformUtil::Int64(VA) << 32) |
                                     (VB & 0xFFFFFFFF);

                if (veryVerbose) {
                    P_(VALUE);
                    P_(SIZE);
                    P(EXP);
                    P(bdeimp_Int64Hash::hash(VALUE, SIZE));
                    cout << endl;
                }
                LOOP_ASSERT(LINE, EXP == bdeimp_Int64Hash::hash(VALUE, SIZE));
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
