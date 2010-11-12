// bsltst_complex.t.cpp                                               -*-C++-*-

#include <bsltst_complex.h>

#include <bsl_complex.h>
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>

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
      case 2: {
        // --------------------------------------------------------------------
        // TEST ABS, ETC.
        //
        // Concerns:
        //   That the parameterized math functions specified in ISO C++ to
        //   accept 'bsl::complex<T>' arguments are callable.
        //
        // Plan:
        //   Call each of them.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTest 'abs', 'exp', etc."
                               << "\n=======================" << bsl::endl;


        {
            typedef float           T;
            typedef bsl::complex<T> Obj;

            Obj x(37.2, 554.23);  const Obj& X = x;
            Obj y;

            y = bsl::real(X);
            y = bsl::imag(X);
            y = bsl::abs(X);
            y = bsl::arg(X);
            y = bsl::norm(X);
            y = bsl::conj(X);
            y = bsl::cos(X);
            y = bsl::cosh(X);
            y = bsl::exp(X);
            y = bsl::log(X);
            y = bsl::log10(X);
            y = bsl::pow(X, 2);
            y = bsl::sin(X);
            y = bsl::sinh(X);
            y = bsl::sqrt(X);
            y = bsl::tan(X);
            y = bsl::tanh(X);

            T t = 1.2f;
            y = bsl::polar(t, (T) 0);
        }

        {
            typedef double          T;
            typedef bsl::complex<T> Obj;

            Obj x(37.2, 554.23);  const Obj& X = x;
            Obj y;

            y = bsl::real(X);
            y = bsl::imag(X);
            y = bsl::abs(X);
            y = bsl::arg(X);
            y = bsl::norm(X);
            y = bsl::conj(X);
            y = bsl::cos(X);
            y = bsl::cosh(X);
            y = bsl::exp(X);
            y = bsl::log(X);
            y = bsl::log10(X);
            y = bsl::pow(X, 2);
            y = bsl::sin(X);
            y = bsl::sinh(X);
            y = bsl::sqrt(X);
            y = bsl::tan(X);
            y = bsl::tanh(X);

            T t = 1.2;
            y = bsl::polar(t, (T)0);
        }

        {
            typedef long double     T;
            typedef bsl::complex<T> Obj;

            Obj x(37.2, 554.23);  const Obj& X = x;
            Obj y;

            y = bsl::real(X);
            y = bsl::imag(X);
            y = bsl::abs(X);
            y = bsl::arg(X);
            y = bsl::norm(X);
            y = bsl::conj(X);
            y = bsl::cos(X);
            y = bsl::cosh(X);
            y = bsl::exp(X);
            y = bsl::log(X);
            y = bsl::log10(X);
            y = bsl::pow(X, 2);
            y = bsl::sin(X);
            y = bsl::sinh(X);
            y = bsl::sqrt(X);
            y = bsl::tan(X);
            y = bsl::tanh(X);

            T t = 1.2;
            y = bsl::polar(t, (T)0);
        }
      } break;
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

        if (verbose) bsl::cout << "\nBreathing Test"
                               << "\n==============" << bsl::endl;

        typedef bsl::complex<int> Obj;

        Obj x1;        const Obj& X1 = x1;  ASSERT(0 == X1.real());
                                            ASSERT(0 == X1.imag());

        Obj x2(3);     const Obj& X2 = x2;  ASSERT(3 == X2.real());
                                            ASSERT(0 == X2.imag());

        Obj x3(2, 4);  const Obj& X3 = x3;  ASSERT(2 == X3.real());
                                            ASSERT(4 == X3.imag());

        Obj x4(X3);    const Obj& X4 = x4;  ASSERT(2 == X4.real());
                                            ASSERT(4 == X4.imag());

        x1 = X4;                            ASSERT(2 == X1.real());
                                            ASSERT(4 == X1.imag());
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
