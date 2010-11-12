// bslmf_anytype.t.cpp                  -*-C++-*-

#include <bslmf_anytype.h>

#include <cstdio>
#include <cstdlib>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

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
        // It's inconvenient to actually create an instance of this type
        // because the constructor takes so many arguments.  It's also
        // impossible because the c'tor is undefined.
};
struct HasHorridCtorY : public Y {
    HasHorridCtorY(int, double, X, char, char *, void *) {}
        // It's inconvenient to actually create an instance of this type
        // because the construnctor takes so many arguments.  It's also
        // impossible because the c'tor is undefined.
};

template <int i>
struct MetaInt { char d_array[i + 1]; };

#define METAINT_TO_UINT(metaint)   (sizeof(metaint) - 1)

MetaInt<1> isX(const X&);
MetaInt<0> isX(const bslmf_AnyType&);

}  // namespace USAGE_TYPEREP_NAMESPACE

namespace USAGE_ANYTYPE_NAMESPACE {

struct X { };
struct Y { };
struct Z : public Y { };

inline bool isY(const bslmf_AnyType&) { return false; }
inline bool isY(const Y&)             { return true;  }

}  // namespace USAGE_ANYTYPE_NAMESPACE

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

inline bool my_isBtraits(my_Btraits) { return true; }
inline bool my_isBtraits(bslmf_AnyType) { return false; }

template <class T>
inline bool my_isB(const T&) { return my_isBtraits(my_Traits<T>()); }
template <class T>
inline bool my_isB(const volatile T&) { return my_isBtraits(my_Traits<T>()); }

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // TypeRep Usage Example
        //
        // Concerns:
        //   Demonstrate a use for bslmf_TypeRep
        //
        // Plan:
        //   Create a situation where we have a type with a difficult to use
        //   c'tor, and we just want an instance of it within an expression
        //   that won't run.  We create the types 'HasHorridCtor*' which
        //   have inconvenient constructors and use bslmf_TypeRep to avoid
        //   using them.
        // --------------------------------------------------------------------

        using namespace USAGE_TYPEREP_NAMESPACE;

        ASSERT(1 == METAINT_TO_UINT(isX(X())));
        ASSERT(0 == METAINT_TO_UINT(isX(Y())));
        ASSERT(1 ==
            METAINT_TO_UINT(isX(bslmf_TypeRep<HasHorridCtorX>::rep())));
        ASSERT(0 ==
            METAINT_TO_UINT(isX(bslmf_TypeRep<HasHorridCtorY>::rep())));
        ASSERT(0 == METAINT_TO_UINT(isX(3)));
        ASSERT(0 == METAINT_TO_UINT(isX(3.0)));
        ASSERT(0 == METAINT_TO_UINT(isX("The king is a fink!")));
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Anytype Usage Example
        //
        // Concerns:
        //   Demonstrate a use for bslmf_AnyType.
        //
        // Plan:
        //   Overload a function such that one version of it take an argument
        //   of type 'Y' and the other takes an argument of type
        //   'bslmf_AnyType', and the combination produces a function that
        //   can evaluate whether a type is 'Y' or not.
        // --------------------------------------------------------------------

        using namespace USAGE_ANYTYPE_NAMESPACE;

        ASSERT(! isY(X()));
        ASSERT(  isY(Y()));
        ASSERT(  isY(Z()));
        ASSERT(! isY(int()));
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

        my_A  a;
        my_A2 a2;
        my_B  b;
        my_C  c;
        int   i;
        volatile       int vi;
        const          int ci = 4;
        const volatile int &cvi = i;

        ASSERT(! my_isB(a));
        ASSERT(! my_isB(a2));
        ASSERT(  my_isB(b));
        ASSERT(  my_isB(c));

        ASSERT(! my_isBtraits(i));
        ASSERT(! my_isBtraits(vi));
        ASSERT(! my_isBtraits(ci));
        ASSERT(! my_isBtraits(cvi));
        ASSERT(! my_isBtraits(&i));
        ASSERT(! my_isBtraits(&vi));
        ASSERT(! my_isBtraits(&ci));
        ASSERT(! my_isBtraits(&cvi));

        const my_Traits<my_A> ta = my_Traits<my_A>();
        my_Traits<my_B> tb;
        ASSERT(! my_isBtraits(ta));
        ASSERT(  my_isBtraits(tb));

        // int or void* matches bslmf_AnyType
        ASSERT(! my_isBtraits(0));
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
