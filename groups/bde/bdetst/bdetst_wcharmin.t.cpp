// bdetst_wcharmin.t.cpp                  -*-C++-*-

// Test WCHAR_MIN and WCHAR_MAX macros.

#include <cwchar>
#ifdef WCHAR_MIN
#    define WCHAR_MIN_PREDEFINED 1
#endif

#include <bdetst_wcharmin.h>

#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//

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
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

#define stringize2(s) #s
#define stringize(s) stringize2(s)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) {
#ifdef WCHAR_MIN_PREDEFINED
            cerr << "WCHAR_MIN IS predefined\n";
#else
            cerr << "WCHAR_MIN IS NOT predefined\n";
            cerr << "__WCHAR_IS_SIGNED = " << boolalpha << __WCHAR_IS_SIGNED
                 << endl;
            cerr << "__WCHAR_HIGHBIT   = " << __WCHAR_HIGHBIT << endl;
#endif
            cerr << "sizeof(wchar_t) = " << sizeof(wchar_t) << endl;
            cerr << "limits min = " << numeric_limits<wchar_t>::min() << endl;
            cerr << "WCHAR_MIN  = " << WCHAR_MIN << endl;
            cerr << "limits max = " << numeric_limits<wchar_t>::max() << endl;
            cerr << "WCHAR_MAX  = " << WCHAR_MAX << endl;
        }

        // Ensure that WCHAR_MIN is the smallest wchar_t value and
        // WCHAR_MAX is the largest wchar_t value.
        wchar_t w1 = WCHAR_MIN;
        const wchar_t WMIN = WCHAR_MIN;
        ASSERT(WMIN == WCHAR_MIN);
        ASSERT(WMIN == numeric_limits<wchar_t>::min());

        wchar_t w2 = WCHAR_MAX;
        const wchar_t WMAX = WCHAR_MAX;
        ASSERT(WMAX == WCHAR_MAX);
        ASSERT(WMAX == numeric_limits<wchar_t>::max());

        ASSERT(WMIN < WMAX);

        --w1; // Underflow to UCHAR_MAX
        ASSERT(WMIN < w1);
        ASSERT(WMAX == w1);
        ASSERT(WCHAR_MAX == w1);

        ++w2; // Overflow to UCHAR_MIN
        ASSERT(WMAX > w2);
        ASSERT(WMIN == w2);
        ASSERT(WCHAR_MIN == w2);

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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
