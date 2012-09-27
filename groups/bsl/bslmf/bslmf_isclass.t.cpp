// bslmf_isclass.t.cpp                                                -*-C++-*-

#include <bslmf_isclass.h>

#include <bsls_platform.h>            // for testing only

#include <cstdlib>    // atoi()
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// TBD
//-----------------------------------------------------------------------------
// [ 1] bslmf::IsClass
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

enum   Enum {};
struct Struct {};
union  Union {};
class  BaseClass {};
class  DerivedClass : public BaseClass {};

typedef int Struct::* PMD;

struct Incomplete;

//=============================================================================
//                 CLASSES, ETC. FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage
///-----
// For example:
//..
     struct MyStruct {};
     enum   MyEnum {};
     class  MyClass {};
     class  MyDerivedClass : public MyClass {};
//
// continued below ...

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating use of 'bslmf::IsClass'.
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

// ... continued from above
     ASSERT(1 == bslmf::IsClass<MyStruct >::value);
     ASSERT(1 == bslmf::IsClass<MyStruct&>::value);
     ASSERT(0 == bslmf::IsClass<MyStruct*>::value);
//
     ASSERT(1 == bslmf::IsClass<const MyClass          >::value);
     ASSERT(1 == bslmf::IsClass<const MyDerivedClass&  >::value);
     ASSERT(0 == bslmf::IsClass<const MyDerivedClass*  >::value);
     ASSERT(0 == bslmf::IsClass<      MyDerivedClass[1]>::value);
//
     ASSERT(0 == bslmf::IsClass<int   >::value);
     ASSERT(0 == bslmf::IsClass<int * >::value);
     ASSERT(0 == bslmf::IsClass<MyEnum>::value);
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf::IsClass' with various types and verify
        //   that their 'value' member is initialized properly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bslmf::IsClass" << endl
                          << "==============" << endl;

        ASSERT(0 == bslmf::IsClass<void>::value);

        ASSERT(0 == bslmf::IsClass<int               >::value);
        ASSERT(0 == bslmf::IsClass<int const         >::value);
        ASSERT(0 == bslmf::IsClass<int volatile      >::value);
        ASSERT(0 == bslmf::IsClass<int const volatile>::value);

        ASSERT(0 == bslmf::IsClass<int&               >::value);
        ASSERT(0 == bslmf::IsClass<int const&         >::value);
        ASSERT(0 == bslmf::IsClass<int volatile&      >::value);
        ASSERT(0 == bslmf::IsClass<int const volatile&>::value);

        ASSERT(0 == bslmf::IsClass<Enum               >::value);
        ASSERT(0 == bslmf::IsClass<Enum const         >::value);
        ASSERT(0 == bslmf::IsClass<Enum volatile      >::value);
        ASSERT(0 == bslmf::IsClass<Enum const volatile>::value);

        ASSERT(0 == bslmf::IsClass<int *                             >::value);
        ASSERT(0 == bslmf::IsClass<int *const                        >::value);
        ASSERT(0 == bslmf::IsClass<int *volatile                     >::value);
        ASSERT(0 == bslmf::IsClass<int *const volatile               >::value);
        ASSERT(0 == bslmf::IsClass<int const *                       >::value);
        ASSERT(0 == bslmf::IsClass<int const *const                  >::value);
        ASSERT(0 == bslmf::IsClass<int const *volatile               >::value);
        ASSERT(0 == bslmf::IsClass<int const *const volatile         >::value);
        ASSERT(0 == bslmf::IsClass<int volatile *                    >::value);
        ASSERT(0 == bslmf::IsClass<int volatile *const               >::value);
        ASSERT(0 == bslmf::IsClass<int volatile *volatile            >::value);
        ASSERT(0 == bslmf::IsClass<int volatile *const volatile      >::value);
        ASSERT(0 == bslmf::IsClass<int const volatile *              >::value);
        ASSERT(0 == bslmf::IsClass<int const volatile *const         >::value);
        ASSERT(0 == bslmf::IsClass<int const volatile *volatile      >::value);
        ASSERT(0 == bslmf::IsClass<int const volatile *const volatile>::value);

        ASSERT(1 == bslmf::IsClass<Struct               >::value);
        ASSERT(1 == bslmf::IsClass<Struct const         >::value);
        ASSERT(1 == bslmf::IsClass<Struct volatile      >::value);
        ASSERT(1 == bslmf::IsClass<Struct const volatile>::value);

        ASSERT(1 == bslmf::IsClass<Struct&               >::value);
        ASSERT(1 == bslmf::IsClass<Struct const&         >::value);
        ASSERT(1 == bslmf::IsClass<Struct volatile&      >::value);
        ASSERT(1 == bslmf::IsClass<Struct const volatile&>::value);

        ASSERT(0 == bslmf::IsClass<Struct *              >::value);
        ASSERT(0 == bslmf::IsClass<Struct *const         >::value);
        ASSERT(0 == bslmf::IsClass<Struct *volatile      >::value);
        ASSERT(0 == bslmf::IsClass<Struct *const volatile>::value);

        ASSERT(1 == bslmf::IsClass<Union               >::value);
        ASSERT(1 == bslmf::IsClass<Union const         >::value);
        ASSERT(1 == bslmf::IsClass<Union volatile      >::value);
        ASSERT(1 == bslmf::IsClass<Union const volatile>::value);

        ASSERT(1 == bslmf::IsClass<Union&               >::value);
        ASSERT(1 == bslmf::IsClass<Union const&         >::value);
        ASSERT(1 == bslmf::IsClass<Union volatile&      >::value);
        ASSERT(1 == bslmf::IsClass<Union const volatile&>::value);

        ASSERT(0 == bslmf::IsClass<Union *              >::value);
        ASSERT(0 == bslmf::IsClass<Union *const         >::value);
        ASSERT(0 == bslmf::IsClass<Union *volatile      >::value);
        ASSERT(0 == bslmf::IsClass<Union *const volatile>::value);

        ASSERT(1 == bslmf::IsClass<BaseClass               >::value);
        ASSERT(1 == bslmf::IsClass<BaseClass const         >::value);
        ASSERT(1 == bslmf::IsClass<BaseClass volatile      >::value);
        ASSERT(1 == bslmf::IsClass<BaseClass const volatile>::value);

        ASSERT(1 == bslmf::IsClass<BaseClass&               >::value);
        ASSERT(1 == bslmf::IsClass<BaseClass const&         >::value);
        ASSERT(1 == bslmf::IsClass<BaseClass volatile&      >::value);
        ASSERT(1 == bslmf::IsClass<BaseClass const volatile&>::value);

        ASSERT(0 == bslmf::IsClass<BaseClass *              >::value);
        ASSERT(0 == bslmf::IsClass<BaseClass *const         >::value);
        ASSERT(0 == bslmf::IsClass<BaseClass *volatile      >::value);
        ASSERT(0 == bslmf::IsClass<BaseClass *const volatile>::value);

        ASSERT(1 == bslmf::IsClass<DerivedClass               >::value);
        ASSERT(1 == bslmf::IsClass<DerivedClass const         >::value);
        ASSERT(1 == bslmf::IsClass<DerivedClass volatile      >::value);
        ASSERT(1 == bslmf::IsClass<DerivedClass const volatile>::value);

        ASSERT(1 == bslmf::IsClass<DerivedClass&               >::value);
        ASSERT(1 == bslmf::IsClass<DerivedClass const&         >::value);
        ASSERT(1 == bslmf::IsClass<DerivedClass volatile&      >::value);
        ASSERT(1 == bslmf::IsClass<DerivedClass const volatile&>::value);

        ASSERT(0 == bslmf::IsClass<DerivedClass *              >::value);
        ASSERT(0 == bslmf::IsClass<DerivedClass *const         >::value);
        ASSERT(0 == bslmf::IsClass<DerivedClass *volatile      >::value);
        ASSERT(0 == bslmf::IsClass<DerivedClass *const volatile>::value);

        ASSERT(1 == bslmf::IsClass<Incomplete  >::value);
        ASSERT(1 == bslmf::IsClass<Incomplete& >::value);
        ASSERT(0 == bslmf::IsClass<Incomplete *>::value);

        ASSERT(0 == bslmf::IsClass<int Struct::*  >::value);
        ASSERT(0 == bslmf::IsClass<int Struct::*& >::value);
        ASSERT(0 == bslmf::IsClass<int Struct::* *>::value);

        ASSERT(0 == bslmf::IsClass<PMD BaseClass::*  >::value);
        ASSERT(0 == bslmf::IsClass<PMD BaseClass::*& >::value);
        ASSERT(0 == bslmf::IsClass<PMD BaseClass::* *>::value);

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
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
