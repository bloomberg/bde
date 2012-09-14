// bslmf_issame.t.cpp                                                 -*-C++-*-

#include <bslmf_issame.h>

#include <cstdlib>     // atoi()
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//-----------------------------------------------------------------------------
// [ 1] bslmf::IsSame
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
        //   Simple example illustrating use of 'bslmf::IsSame'.
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
       const int I = bslmf::IsSame<INT, INT>::value;            ASSERT(1 == I);
       const int J = bslmf::IsSame<INT, DOUBLE>::value;         ASSERT(0 == J);
//..
// Note that a 'const'-qualified type is considered distinct from the
// non-'const' (but otherwise identical) type:
//..
       typedef       short       SHORT;
       typedef const short CONST_SHORT;
       const int K = bslmf::IsSame<SHORT, CONST_SHORT>::value;  ASSERT(0 == K);
//..
// Similarly, a 'TYPE' and a reference to 'TYPE' ('TYPE&') are distinct:
//..
       typedef int  INT;
       typedef int& INT_REF;
       const int L = bslmf::IsSame<INT, INT_REF>::value;        ASSERT(0 == L);
//..

        } break;
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf::IsSame' with various combinations of types and
        //   verify that the 'VALUE' member is initialized properly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "bslmf::IsSame" << endl
                                  << "=============" << endl;

        ASSERT(1 == (bslmf::IsSame<int, int>::value));
        ASSERT(1 == (bslmf::IsSame<short, short>::value));
        ASSERT(0 == (bslmf::IsSame<int, short>::value));
        ASSERT(0 == (bslmf::IsSame<short, int>::value));
        ASSERT(0 == (bslmf::IsSame<int, unsigned>::value));
        ASSERT(0 == (bslmf::IsSame<int&, int>::value));
        ASSERT(0 == (bslmf::IsSame<float, double>::value));
        ASSERT(0 == (bslmf::IsSame<float, float&>::value));

        ASSERT(1 == (bslmf::IsSame<int const, int const>::value));
        ASSERT(1 == (bslmf::IsSame<const int, int const>::value));
        ASSERT(0 == (bslmf::IsSame<int, int const>::value));
        ASSERT(0 == (bslmf::IsSame<int volatile, int>::value));
        ASSERT(0 == (bslmf::IsSame<int const volatile, int const>::value));

        ASSERT(1 == (bslmf::IsSame<Enum1, Enum1>::value));
        ASSERT(1 == (bslmf::IsSame<Enum3, Enum3>::value));
        ASSERT(0 == (bslmf::IsSame<Enum1, Enum2>::value));
        ASSERT(0 == (bslmf::IsSame<Enum3, Enum1>::value));
        ASSERT(0 == (bslmf::IsSame<int, Enum1>::value));
        ASSERT(0 == (bslmf::IsSame<Enum1, unsigned>::value));
        ASSERT(0 == (bslmf::IsSame<long, Enum1>::value));
        ASSERT(0 == (bslmf::IsSame<Enum1, unsigned long>::value));

        ASSERT(1 == (bslmf::IsSame<char *, char *>::value));
        ASSERT(1 == (bslmf::IsSame<void *, void *>::value));
        ASSERT(1 == (bslmf::IsSame<const char *, const char *>::value));
        ASSERT(0 == (bslmf::IsSame<const char *, char *>::value));
        ASSERT(0 == (bslmf::IsSame<char *, char *const>::value));
        ASSERT(0 == (bslmf::IsSame<char *, void *>::value));
        ASSERT(0 == (bslmf::IsSame<int *, char *>::value));

        ASSERT(1 == (bslmf::IsSame<Struct1, Struct1>::value));
        ASSERT(1 == (bslmf::IsSame<Struct3, Struct3>::value));
        ASSERT(0 == (bslmf::IsSame<Struct1, Struct2>::value));
        ASSERT(0 == (bslmf::IsSame<Struct3, Struct1>::value));

        ASSERT(1 == (bslmf::IsSame<Base, Base>::value));
        ASSERT(1 == (bslmf::IsSame<const Base *, const Base *>::value));
        ASSERT(1 == (bslmf::IsSame<Derived&, Derived&>::value));
        ASSERT(0 == (bslmf::IsSame<Base&, Base>::value));
        ASSERT(0 == (bslmf::IsSame<Base&, Derived&>::value));
        ASSERT(0 == (bslmf::IsSame<Derived *, Base *>::value));
        ASSERT(0 == (bslmf::IsSame<void *, Base *>::value));

        ASSERT(1 == (bslmf::IsSame<int Base::*, int Base::*>::value));
        ASSERT(1 == (bslmf::IsSame<int Struct3::*, int Struct3::*>::value));
        ASSERT(0 == (bslmf::IsSame<int Base::*, int Class::*>::value));
        ASSERT(0 == (bslmf::IsSame<int Base::*, int Derived::*>::value));
        ASSERT(0 == (bslmf::IsSame<int Derived::*, int Base::*>::value));

        ASSERT(1 == (bslmf::IsSame<INT_TYPE, INT_TYPE>::value));
        ASSERT(1 == (bslmf::IsSame<INT_TYPE, Class::INT_TYPE>::value));
        ASSERT(1 == (bslmf::IsSame<NS::INT_TYPE, Class::INT_TYPE>::value));
        ASSERT(1 == (bslmf::IsSame<INT_TYPE, NS::INT_TYPE>::value));

        ASSERT(1 == (bslmf::IsSame<char [1], char [1]>::value));
        ASSERT(1 == (bslmf::IsSame<const int [5], const int [5]>::value));
        ASSERT(0 == (bslmf::IsSame<char, char [1]>::value));
        ASSERT(0 == (bslmf::IsSame<int [5], char [5]>::value));
        ASSERT(0 == (bslmf::IsSame<int [2][4], int [4][2]>::value));

        ASSERT(1 == (bslmf::IsSame<F, F>::value));
        ASSERT(1 == (bslmf::IsSame<Fv, F>::value));
        ASSERT(1 == (bslmf::IsSame<Fi, Fi>::value));
        ASSERT(1 == (bslmf::IsSame<PFi, PFi>::value));
        ASSERT(0 == (bslmf::IsSame<Fe, Fi>::value));
        ASSERT(0 == (bslmf::IsSame<Fe, Fie>::value));
        ASSERT(0 == (bslmf::IsSame<Fie, Fi>::value));
        ASSERT(0 == (bslmf::IsSame<PFi, Fi>::value));
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
