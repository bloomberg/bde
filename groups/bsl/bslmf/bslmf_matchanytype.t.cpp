// bslmf_matchanytype.t.cpp                                           -*-C++-*-

#include <bslmf_matchanytype.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                  DEFECT MACROS FOR TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION != 0x514
# define SUN_HAS_PROBLEM_WITH_LITERALS
    // The 5.14 compiler has an odd problem where literals, passed as rvalues
    // to functions, match 'const volatile' references during argument type
    // deduction, but cannot actually match that signature so fail to compile
    // when called.  Even more oddly, this does not affect all types, so tests
    // may be selectively disabled waiting for a compiler patch.
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                     CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

namespace USAGE_TYPEREP_NAMESPACE {

struct X {};
struct Y {};

struct HasHorridCtorX : public X {
    HasHorridCtorX(int, double, X, char, char *, void *) {}
        // It is inconvenient to actually create an instance of this type
        // because the constructor takes so many arguments.  It is also
        // impossible because the constructor is undefined.
};
struct HasHorridCtorY : public Y {
    HasHorridCtorY(int, double, X, char, char *, void *) {}
        // It is inconvenient to actually create an instance of this type
        // because the constructor takes so many arguments.  It is also
        // impossible because the constructor is undefined.
};

template <int i>
struct MetaInt { char d_array[i + 1]; };

#define METAINT_TO_UINT(metaint)   (sizeof(metaint) - 1)

MetaInt<1> isX(const X&);
MetaInt<0> isX(const bslmf::MatchAnyType&);

}  // close namespace USAGE_TYPEREP_NAMESPACE

namespace USAGE_MATCHANYTYPE_NAMESPACE {

struct X { };
struct Y { };
struct Z : public Y { };

inline bool isY(const bslmf::MatchAnyType&) { return false; }
inline bool isY(const Y&)                   { return true;  }

}  // close namespace USAGE_MATCHANYTYPE_NAMESPACE

//=============================================================================
//                         CLASSES FOR BREATHING TEST
//-----------------------------------------------------------------------------

struct my_A { };
struct my_A2 : my_A { };
struct my_B : my_A { };
struct my_C : my_B { };

struct my_Atraits { };
struct my_Btraits { };

template <class T> struct my_Traits : my_Atraits { };
template <> struct my_Traits<my_B> : my_Atraits, my_Btraits { };
template <> struct my_Traits<my_C> : my_Atraits, my_Btraits { };

inline bool my_isBtraits(my_Btraits)          { return true; }
inline bool my_isBtraits(bslmf::MatchAnyType) { return false; }

template <class T>
inline bool my_isB(const T&) { return my_isBtraits(my_Traits<T>()); }
template <class T>
inline bool my_isB(const volatile T&) { return my_isBtraits(my_Traits<T>()); }

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;          // eliminate unused variable warning
    (void) veryVeryVerbose;      // eliminate unused variable warning
    (void) veryVeryVeryVerbose;  // eliminate unused variable warning

    setbuf(stdout, NULL);        // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // 'TypeRep' USAGE EXAMPLE
        //
        // Concerns:
        //   Demonstrate a use for bslmf::TypeRep
        //
        // Plan:
        //   Create a situation where we have a type with a difficult to use
        //   c'tor, and we just want an instance of it within an expression
        //   that won't run.  We create the types 'HasHorridCtor*' which
        //   have inconvenient constructors and use bslmf::TypeRep to avoid
        //   using them.
        // --------------------------------------------------------------------

        if (verbose) printf("\n'TypeRep' USAGE EXAMPLE"
                            "\n=======================\n");

        using namespace USAGE_TYPEREP_NAMESPACE;

        ASSERT(1 == METAINT_TO_UINT(isX(X())));
        ASSERT(0 == METAINT_TO_UINT(isX(Y())));
        ASSERT(1 ==
                  METAINT_TO_UINT(isX(bslmf::TypeRep<HasHorridCtorX>::rep())));
        ASSERT(0 ==
                  METAINT_TO_UINT(isX(bslmf::TypeRep<HasHorridCtorY>::rep())));
        ASSERT(0 == METAINT_TO_UINT(isX(3)));
        ASSERT(0 == METAINT_TO_UINT(isX(3.0)));
        ASSERT(0 == METAINT_TO_UINT(isX("The king is a fink!")));
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'MathAnyType' USAGE EXAMPLE
        //
        // Concerns:
        //   Demonstrate a use for 'bslmf::MatchAnyType'.
        //
        // Plan:
        //   Overload a function such that one version of it take an argument
        //   of type 'Y' and the other takes an argument of type
        //   'bslmf::MatchAnyType', and the combination produces a function
        //   that can evaluate whether a type is 'Y' or not.
        // --------------------------------------------------------------------

        if (verbose) printf("\n'MathAnyType' USAGE EXAMPLE"
                            "\n===========================\n");

        using namespace USAGE_MATCHANYTYPE_NAMESPACE;

        ASSERT(! isY(X()));
        ASSERT(  isY(Y()));
        ASSERT(  isY(Z()));
#if !defined(SUN_HAS_PROBLEM_WITH_LITERALS)
        ASSERT(! isY(int()));
#endif
        ASSERT(! isY(4));
        ASSERT(! isY(4.0));
        ASSERT(! isY("The king is a fink!"));

        X x;
        Y y;
        Z z;
        ASSERT(! isY(x));
        ASSERT(  isY(y));
        ASSERT(  isY(z));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
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

        my_A  a;
        my_A2 a2;
        my_B  b;
        my_C  c;
        int   i;

        volatile       int  vi;
        const          int  ci  = 4;
        const volatile int& cvi = i;

        ASSERT(! my_isB(a));
        ASSERT(! my_isB(a2));
        ASSERT(  my_isB(b));
        ASSERT(  my_isB(c));

        ASSERT(! my_isBtraits(i));
        ASSERT(! my_isBtraits(vi));
        ASSERT(! my_isBtraits(ci));
        ASSERT(! my_isBtraits(cvi));
        ASSERT(! my_isBtraits(&i));
        ASSERT(! my_isBtraits(&ci));
#if !defined(SUN_HAS_PROBLEM_WITH_LITERALS)
        ASSERT(! my_isBtraits(&vi));
        ASSERT(! my_isBtraits(&cvi));
#endif

        const my_Traits<my_A> ta = my_Traits<my_A>();
        my_Traits<my_B>       tb;
        ASSERT(! my_isBtraits(ta));
        ASSERT(  my_isBtraits(tb));

        // int or void* matches bslmf::MatchAnyType
#if !defined(SUN_HAS_PROBLEM_WITH_LITERALS)
        ASSERT(! my_isBtraits(0));
#endif
        ASSERT(! my_isBtraits((void*) 0));

      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
