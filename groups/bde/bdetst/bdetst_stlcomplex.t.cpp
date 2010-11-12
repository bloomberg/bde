// bdetst_stlcomplex.t.cpp                  -*-C++-*-

#include <bdetst_stlcomplex.h>

#include <cstdlib>     // atoi()
#include <iostream>

using namespace BloombergLP;

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
        std::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << std::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) std::cout << #X " = " << (X) << std::endl;
                                                 // Print identifier and value.
#define Q(X) std::cout << "<| " #X " |>" << std::endl;
                                                 // Quote identifier literally.
#define P_(X) std::cout << #X " = " << (X) << ", "<< std::flush;
                                                 // P(X) without '\n'
#define L_ __LINE__                              // current Line number
#define PS(X) std::cout << #X " = \n" << (X) << std::endl;
                                                 // Print identifier and value.
#define T_()  std::cout << "\t" << std::flush;   // Print a tab (w/o newline)

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
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    std::cout << "TEST " << __FILE__ << " CASE " << test << std::endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TEST ABS, ETC.
        //
        // Concerns:
        //   That the parameterized math functions specified in ISO C++ to
        //   accept 'std::complex<T>' arguments are callable.
        //
        // Plan:
        //   Call each of them.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) std::cout << "\nTest 'abs', 'exp', etc."
                               << "\n=======================" << std::endl;

// TBD
#if 0
        {
            typedef int               T;
            typedef std::complex<int> Obj;

            Obj x(7, 42);  const Obj& X = x;
            Obj y;

            y = std::real(X);
            y = std::imag(X);
            y = std::abs(X);
            y = std::arg(X);
            y = std::norm(X);
            y = std::conj(X);
            y = std::cos(X);
            y = std::cosh(X);
            y = std::exp(X);
            y = std::log(X);
            y = std::log10(X);
            y = std::pow(X, 2);
            y = std::sin(X);
            y = std::sinh(X);
            y = std::sqrt(X);
            y = std::tan(X);
            y = std::tanh(X);

            T t = 6;
            y = std::polar(t);
        }
#endif

        {
            typedef float           T;
            typedef std::complex<T> Obj;

            Obj x(37.2, 554.23);  const Obj& X = x;
            Obj y;

            y = std::real(X);
            y = std::imag(X);
            y = std::abs(X);
            y = std::arg(X);
            y = std::norm(X);
            y = std::conj(X);
            y = std::cos(X);
            y = std::cosh(X);
            y = std::exp(X);
            y = std::log(X);
            y = std::log10(X);
            y = std::pow(X, 2);
            y = std::sin(X);
            y = std::sinh(X);
            y = std::sqrt(X);
            y = std::tan(X);
            y = std::tanh(X);

            T t = 1.2f;
            y = std::polar(t);
        }

        {
            typedef double          T;
            typedef std::complex<T> Obj;

            Obj x(37.2, 554.23);  const Obj& X = x;
            Obj y;

            y = std::real(X);
            y = std::imag(X);
            y = std::abs(X);
            y = std::arg(X);
            y = std::norm(X);
            y = std::conj(X);
            y = std::cos(X);
            y = std::cosh(X);
            y = std::exp(X);
            y = std::log(X);
            y = std::log10(X);
            y = std::pow(X, 2);
            y = std::sin(X);
            y = std::sinh(X);
            y = std::sqrt(X);
            y = std::tan(X);
            y = std::tanh(X);

            T t = 1.2;
            y = std::polar(t);
        }

        {
            typedef long double     T;
            typedef std::complex<T> Obj;

            Obj x(37.2, 554.23);  const Obj& X = x;
            Obj y;

            y = std::real(X);
            y = std::imag(X);
            y = std::abs(X);
            y = std::arg(X);
            y = std::norm(X);
            y = std::conj(X);
            y = std::cos(X);
            y = std::cosh(X);
            y = std::exp(X);
            y = std::log(X);
            y = std::log10(X);
            y = std::pow(X, 2);
            y = std::sin(X);
            y = std::sinh(X);
            y = std::sqrt(X);
            y = std::tan(X);
            y = std::tanh(X);

            T t = 1.2;
            y = std::polar(t);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   This breathing test exercises basic functionality as a preliminary
        //   to the thorough testing done in subsequent cases.
        // --------------------------------------------------------------------

        if (verbose) std::cout << "\nBreathing Test"
                               << "\n==============" << std::endl;

        typedef std::complex<int> Obj;

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
          std::cerr << "WARNING: CASE `" << test << "' NOT FOUND."
                    << std::endl;
          testStatus = -1;
      }
    }

    if (testStatus > 0) {
        std::cerr << "Error: non-zero test status = " << testStatus << "."
                  << std::endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
