// bdeci_hashtableimputil.t.cpp       -*-C++-*-

#include <bdeci_hashtableimputil.h>

#include <bsls_platformutil.h>

#include <bsl_c_stdlib.h>
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of a static member function (pure
// procedures) that provides precomputed hashtable sizes.  The helper function
// 'isPrime' is used to verify the contract of this static member function.
//-----------------------------------------------------------------------------
// [ 3] static int lookup(int index);
//-----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
//
// [ 2] static int isPrime(int number);
// [ 1] generation of hashtable size data

//=============================================================================
//                        STANDARD BDE ASSERT TEST MACRO
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
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

int isPrime(int number)
    // Return 1 if the specified 'number' is prime, and 0 otherwise.  The
    // runtime complexity is O[sqrt(n)].  The behavior is undefined unless
    // 0 <= number < 2^31.
{
    ASSERT(0 <= number);
    ASSERT(number <= 0x7fffffff);

    // The approach taken is first to consult a short lookup-table of values
    // and then to exhaustively search up to the sqrt(number) for values that
    // divide 'number' and hence demonstrate that 'number' is not prime.  As an
    // optimization, even numbers are not used in this search except for 2
    // (if an even value were to divide 'number' so must 2).  Searching up to
    // the square root of 'number' is sufficient since, if 'x' is a value
    // greater than sqrt(number) that divides 'number', there must by a 'y'
    // such that x * y == number and 'y' is less than sqrt(number).

    switch (number) {
      case 0: return 0;
      case 1: return 0;
      case 2: return 1;
      case 3: return 1;
      case 4: return 0;
      case 5: return 1;
      case 6: return 0;
      case 7: return 1;
      case 8: return 0;
      case 9: return 0;
    }

    // Check if 2 divides 'number'.

    if (0 == number % 2) {
        return 0;
    }

    // Check all remaining numbers that might divide 'number'.  Iterate up to
    // and including the square root of number.  Also check to avoid integer
    // overflow (i * i > INT_MAX) by requiring 'i' to be less than 46340.

    for (int i = 3; i < 46340 && i * i <= number; i += 2) {
        if (0 == number % i) {
            return 0;
        }
    }

    // 'number' must be prime.

    return 1;
}

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
      case 4: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;

        if (verbose) {
            for (int i = 0; i < bdeci_HashtableImpUtil::BDECI_NUM_SIZES; ++i) {
                cout << bdeci_HashtableImpUtil::lookup(i) << endl;
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING LOOKUP METHOD:
        //   The concern is that the method returns one for index 0, 0x7fffffff
        //   for index BDECI_NUM_SIZES - 1, and an increasing sequence of prime
        //   numbers for increasing index values.
        //
        // Plan:
        //   Verify the requirements upon the method are met.
        //
        // Testing:
        //   static int lookup(int index);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing 'lookup'" << endl
                 << "================" << endl;

        if (verbose) cout << "\nTesting 'lookup(int index)'." << endl;
        {
            ASSERT(1 == bdeci_HashtableImpUtil::lookup(0));
            ASSERT(0x7fffffff == bdeci_HashtableImpUtil::
                          lookup(bdeci_HashtableImpUtil::BDECI_NUM_SIZES - 1));
            for (int i = 1; i < bdeci_HashtableImpUtil::BDECI_NUM_SIZES; ++i) {
                if (veryVerbose) {
                    P_(i);  P(bdeci_HashtableImpUtil::lookup(i));
                }
                LOOP_ASSERT(i,
                            1 == isPrime(bdeci_HashtableImpUtil::lookup(i)));
                LOOP_ASSERT(i, bdeci_HashtableImpUtil::lookup(i - 1) <
                               bdeci_HashtableImpUtil::lookup(i));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING ISPRIME METHOD:
        //   The concern is that the method returns the correct value.
        //
        // Plan:
        //   Verify the results of the method against a set of test vectors
        //
        // Testing:
        //   static int isPrime(int number);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing 'isPrime'" << endl
                 << "=================" << endl;

        if (verbose) cout << "\nTesting 'isPrime(int number)'." << endl;
        {
            static const struct {
                int d_lineNum;     // source line number
                int d_number;      // function argument
                int d_exp;         // expected return value
            } DATA[] = {
                //line         number         rv
                //----  -------------  --
                { L_,              0,   0 },
                { L_,              1,   0 },
                { L_,              2,   1 },
                { L_,              3,   1 },
                { L_,              4,   0 },
                { L_,              5,   1 },
                { L_,              6,   0 },
                { L_,              7,   1 },
                { L_,              8,   0 },
                { L_,              9,   0 },
                { L_,             10,   0 },
                { L_,             11,   1 },
                { L_,             12,   0 },
                { L_,             13,   1 },
                { L_,             14,   0 },
                { L_,             15,   0 },
                { L_,             16,   0 },
                { L_,             17,   1 },
                { L_,             23,   1 },
                { L_,             25,   0 },
                { L_,             49,   0 },
                { L_,            253,   0 },
                { L_,            254,   0 },
                { L_,            255,   0 },
                { L_,            256,   0 },
                { L_,            257,   1 },
                { L_,     2147483645,   0 },
                { L_,     2147483646,   0 },
                { L_,     0x7fffffff,   1 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE   = DATA[ti].d_lineNum;
                const int NUMBER = DATA[ti].d_number;
                const int EXP    = DATA[ti].d_exp;

                if (veryVerbose) {
                    P_(NUMBER);
                    P_(EXP);
                    P(isPrime(NUMBER));
                }
                LOOP_ASSERT(LINE, EXP == isPrime(NUMBER));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // THIS IS NOT A TEST CASE
        // This section of code was used to generate the hashtable size data.
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Generating hashtable size data" << endl
                 << "==============================" << endl;

        if (verbose) {
            int i = 0;
            int n = 1;

            cout << i << "\t" << n << endl;
            for (i = 1; i <= 15; ++i) {
                n *= 4;
                while (!isPrime(n)) {
                    ++n;
                }
                cout << i << "\t" << n << endl;
            }
            n = 0x7fffffff;
            ASSERT(isPrime(n));
            cout << i << "\t" << n << endl;
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
