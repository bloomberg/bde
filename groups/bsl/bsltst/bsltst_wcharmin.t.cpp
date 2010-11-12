// bsltst_wcharmin.t.cpp                                              -*-C++-*-

#include <bsltst_wcharmin.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cwchar.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>

#ifdef WCHAR_MIN
#    define WCHAR_MIN_PREDEFINED 1
#endif

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//-----------------------------------------------------------------------------
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_                                    // Print tab.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                          HELPER CLASS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1 : {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
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
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
