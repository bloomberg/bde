// bdemf_issame.t.cpp              -*-C++-*-

#include <bdemf_issame.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//-----------------------------------------------------------------------------
// [ 1] bdemf_IsSame
// [ 2] USAGE EXAMPLE
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }
//=============================================================================
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;           // Print tab w/o linefeed.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum Enum1 {};
enum Enum2 {};
enum Enum3 { e = 0 };

struct Struct1 {};
struct Struct2 {};
struct Struct3 { int x; };

class Base {};
class Derived : public Base {};

typedef int INT_TYPE;
namespace NS {
    typedef int INT_TYPE;
}
class Class {
  public:
    typedef int INT_TYPE;
};

typedef void F();
typedef void Fv(void);
typedef void Fi(int);
typedef void Fie(int, ...);
typedef void Fe(...);
typedef void (*PFi)(int);

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    (void) verbose;          // eliminate unused variable warning
    (void) veryVerbose;      // eliminate unused variable warning

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating use of 'bdemf_IsSame'.
        //
        // Concerns:
        //
        // Plan:
        //
        // Tactics:
        //   - Add-Hoc Data Selection Method
        //   - Brute-Force implementation technique
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// For example:
//..
       typedef int    INT;
       typedef double DOUBLE;
//
       const int I = bdemf_IsSame<INT, INT>::VALUE;            ASSERT(1 == I);
       const int J = bdemf_IsSame<INT, DOUBLE>::VALUE;         ASSERT(0 == J);
//..
// Note that a 'const'-qualified type is considered distinct from the
// non-'const' (but otherwise identical) type:
//..
       typedef       short       SHORT;
       typedef const short CONST_SHORT;
       const int K = bdemf_IsSame<SHORT, CONST_SHORT>::VALUE;  ASSERT(0 == K);
//..
// Similarly, a 'TYPE' and a reference to 'TYPE' ('TYPE&') are distinct:
//..
       typedef int  INT;
       typedef int& INT_REF;
       const int L = bdemf_IsSame<INT, INT_REF>::VALUE;        ASSERT(0 == L);
//..

        } break;
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bdemf_IsSame' with various combinations of types and
        //   verify that the 'VALUE' member is initialized properly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "bdemf_IsSame" << endl
                                  << "============" << endl;

        ASSERT(1 == (bdemf_IsSame<int, int>::VALUE));
        ASSERT(1 == (bdemf_IsSame<short, short>::VALUE));
        ASSERT(0 == (bdemf_IsSame<int, short>::VALUE));
        ASSERT(0 == (bdemf_IsSame<short, int>::VALUE));
        ASSERT(0 == (bdemf_IsSame<int, unsigned>::VALUE));
        ASSERT(0 == (bdemf_IsSame<int&, int>::VALUE));
        ASSERT(0 == (bdemf_IsSame<float, double>::VALUE));
        ASSERT(0 == (bdemf_IsSame<float, float&>::VALUE));

        ASSERT(1 == (bdemf_IsSame<int const, int const>::VALUE));
        ASSERT(1 == (bdemf_IsSame<const int, int const>::VALUE));
        ASSERT(0 == (bdemf_IsSame<int, int const>::VALUE));
        ASSERT(0 == (bdemf_IsSame<int volatile, int>::VALUE));
        ASSERT(0 == (bdemf_IsSame<int const volatile, int const>::VALUE));

        ASSERT(1 == (bdemf_IsSame<Enum1, Enum1>::VALUE));
        ASSERT(1 == (bdemf_IsSame<Enum3, Enum3>::VALUE));
        ASSERT(0 == (bdemf_IsSame<Enum1, Enum2>::VALUE));
        ASSERT(0 == (bdemf_IsSame<Enum3, Enum1>::VALUE));
        ASSERT(0 == (bdemf_IsSame<int, Enum1>::VALUE));
        ASSERT(0 == (bdemf_IsSame<Enum1, unsigned>::VALUE));
        ASSERT(0 == (bdemf_IsSame<long, Enum1>::VALUE));
        ASSERT(0 == (bdemf_IsSame<Enum1, unsigned long>::VALUE));

        ASSERT(1 == (bdemf_IsSame<char *, char *>::VALUE));
        ASSERT(1 == (bdemf_IsSame<void *, void *>::VALUE));
        ASSERT(1 == (bdemf_IsSame<const char *, const char *>::VALUE));
        ASSERT(0 == (bdemf_IsSame<const char *, char *>::VALUE));
        ASSERT(0 == (bdemf_IsSame<char *, char *const>::VALUE));
        ASSERT(0 == (bdemf_IsSame<char *, void *>::VALUE));
        ASSERT(0 == (bdemf_IsSame<int *, char *>::VALUE));

        ASSERT(1 == (bdemf_IsSame<Struct1, Struct1>::VALUE));
        ASSERT(1 == (bdemf_IsSame<Struct3, Struct3>::VALUE));
        ASSERT(0 == (bdemf_IsSame<Struct1, Struct2>::VALUE));
        ASSERT(0 == (bdemf_IsSame<Struct3, Struct1>::VALUE));

        ASSERT(1 == (bdemf_IsSame<Base, Base>::VALUE));
        ASSERT(1 == (bdemf_IsSame<const Base *, const Base *>::VALUE));
        ASSERT(1 == (bdemf_IsSame<Derived&, Derived&>::VALUE));
        ASSERT(0 == (bdemf_IsSame<Base&, Base>::VALUE));
        ASSERT(0 == (bdemf_IsSame<Base&, Derived&>::VALUE));
        ASSERT(0 == (bdemf_IsSame<Derived *, Base *>::VALUE));
        ASSERT(0 == (bdemf_IsSame<void *, Base *>::VALUE));

        ASSERT(1 == (bdemf_IsSame<int Base::*, int Base::*>::VALUE));
        ASSERT(1 == (bdemf_IsSame<int Struct3::*, int Struct3::*>::VALUE));
        ASSERT(0 == (bdemf_IsSame<int Base::*, int Class::*>::VALUE));
        ASSERT(0 == (bdemf_IsSame<int Base::*, int Derived::*>::VALUE));
        ASSERT(0 == (bdemf_IsSame<int Derived::*, int Base::*>::VALUE));

        ASSERT(1 == (bdemf_IsSame<INT_TYPE, INT_TYPE>::VALUE));
        ASSERT(1 == (bdemf_IsSame<INT_TYPE, Class::INT_TYPE>::VALUE));
        ASSERT(1 == (bdemf_IsSame<NS::INT_TYPE, Class::INT_TYPE>::VALUE));
        ASSERT(1 == (bdemf_IsSame<INT_TYPE, NS::INT_TYPE>::VALUE));

        ASSERT(1 == (bdemf_IsSame<char [1], char [1]>::VALUE));
        ASSERT(1 == (bdemf_IsSame<const int [5], const int [5]>::VALUE));
        ASSERT(0 == (bdemf_IsSame<char, char [1]>::VALUE));
        ASSERT(0 == (bdemf_IsSame<int [5], char [5]>::VALUE));
        ASSERT(0 == (bdemf_IsSame<int [2][4], int [4][2]>::VALUE));

        ASSERT(1 == (bdemf_IsSame<F, F>::VALUE));
        ASSERT(1 == (bdemf_IsSame<Fv, F>::VALUE));
        ASSERT(1 == (bdemf_IsSame<Fi, Fi>::VALUE));
        ASSERT(1 == (bdemf_IsSame<PFi, PFi>::VALUE));
        ASSERT(0 == (bdemf_IsSame<Fe, Fi>::VALUE));
        ASSERT(0 == (bdemf_IsSame<Fe, Fie>::VALUE));
        ASSERT(0 == (bdemf_IsSame<Fie, Fi>::VALUE));
        ASSERT(0 == (bdemf_IsSame<PFi, Fi>::VALUE));
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
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
