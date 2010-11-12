// bslmf_isclass.t.cpp            -*-C++-*-

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
// [ 1] bslmf_IsClass
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
        //   Simple example illustrating use of 'bslmf_IsClass'.
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
     ASSERT(1 == bslmf_IsClass<MyStruct >::VALUE);
     ASSERT(1 == bslmf_IsClass<MyStruct&>::VALUE);
     ASSERT(0 == bslmf_IsClass<MyStruct*>::VALUE);
//
     ASSERT(1 == bslmf_IsClass<const MyClass          >::VALUE);
     ASSERT(1 == bslmf_IsClass<const MyDerivedClass&  >::VALUE);
     ASSERT(0 == bslmf_IsClass<const MyDerivedClass*  >::VALUE);
     ASSERT(0 == bslmf_IsClass<      MyDerivedClass[1]>::VALUE);
//
     ASSERT(0 == bslmf_IsClass<int   >::VALUE);
     ASSERT(0 == bslmf_IsClass<int * >::VALUE);
     ASSERT(0 == bslmf_IsClass<MyEnum>::VALUE);
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf_IsClass' with various types and verify
        //   that their 'VALUE' member is initialized properly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bslmf_IsClass" << endl
                          << "=============" << endl;

        ASSERT(0 == bslmf_IsClass<void>::VALUE);

        ASSERT(0 == bslmf_IsClass<int               >::VALUE);
        ASSERT(0 == bslmf_IsClass<int const         >::VALUE);
        ASSERT(0 == bslmf_IsClass<int volatile      >::VALUE);
        ASSERT(0 == bslmf_IsClass<int const volatile>::VALUE);

        ASSERT(0 == bslmf_IsClass<int&               >::VALUE);
        ASSERT(0 == bslmf_IsClass<int const&         >::VALUE);
        ASSERT(0 == bslmf_IsClass<int volatile&      >::VALUE);
        ASSERT(0 == bslmf_IsClass<int const volatile&>::VALUE);

        ASSERT(0 == bslmf_IsClass<Enum               >::VALUE);
        ASSERT(0 == bslmf_IsClass<Enum const         >::VALUE);
        ASSERT(0 == bslmf_IsClass<Enum volatile      >::VALUE);
        ASSERT(0 == bslmf_IsClass<Enum const volatile>::VALUE);

        ASSERT(0 == bslmf_IsClass<int *                             >::VALUE);
        ASSERT(0 == bslmf_IsClass<int *const                        >::VALUE);
        ASSERT(0 == bslmf_IsClass<int *volatile                     >::VALUE);
        ASSERT(0 == bslmf_IsClass<int *const volatile               >::VALUE);
        ASSERT(0 == bslmf_IsClass<int const *                       >::VALUE);
        ASSERT(0 == bslmf_IsClass<int const *const                  >::VALUE);
        ASSERT(0 == bslmf_IsClass<int const *volatile               >::VALUE);
        ASSERT(0 == bslmf_IsClass<int const *const volatile         >::VALUE);
        ASSERT(0 == bslmf_IsClass<int volatile *                    >::VALUE);
        ASSERT(0 == bslmf_IsClass<int volatile *const               >::VALUE);
        ASSERT(0 == bslmf_IsClass<int volatile *volatile            >::VALUE);
        ASSERT(0 == bslmf_IsClass<int volatile *const volatile      >::VALUE);
        ASSERT(0 == bslmf_IsClass<int const volatile *              >::VALUE);
        ASSERT(0 == bslmf_IsClass<int const volatile *const         >::VALUE);
        ASSERT(0 == bslmf_IsClass<int const volatile *volatile      >::VALUE);
        ASSERT(0 == bslmf_IsClass<int const volatile *const volatile>::VALUE);

        ASSERT(1 == bslmf_IsClass<Struct               >::VALUE);
        ASSERT(1 == bslmf_IsClass<Struct const         >::VALUE);
        ASSERT(1 == bslmf_IsClass<Struct volatile      >::VALUE);
        ASSERT(1 == bslmf_IsClass<Struct const volatile>::VALUE);

        ASSERT(1 == bslmf_IsClass<Struct&               >::VALUE);
        ASSERT(1 == bslmf_IsClass<Struct const&         >::VALUE);
        ASSERT(1 == bslmf_IsClass<Struct volatile&      >::VALUE);
        ASSERT(1 == bslmf_IsClass<Struct const volatile&>::VALUE);

        ASSERT(0 == bslmf_IsClass<Struct *              >::VALUE);
        ASSERT(0 == bslmf_IsClass<Struct *const         >::VALUE);
        ASSERT(0 == bslmf_IsClass<Struct *volatile      >::VALUE);
        ASSERT(0 == bslmf_IsClass<Struct *const volatile>::VALUE);

        ASSERT(1 == bslmf_IsClass<Union               >::VALUE);
        ASSERT(1 == bslmf_IsClass<Union const         >::VALUE);
        ASSERT(1 == bslmf_IsClass<Union volatile      >::VALUE);
        ASSERT(1 == bslmf_IsClass<Union const volatile>::VALUE);

        ASSERT(1 == bslmf_IsClass<Union&               >::VALUE);
        ASSERT(1 == bslmf_IsClass<Union const&         >::VALUE);
        ASSERT(1 == bslmf_IsClass<Union volatile&      >::VALUE);
        ASSERT(1 == bslmf_IsClass<Union const volatile&>::VALUE);

        ASSERT(0 == bslmf_IsClass<Union *              >::VALUE);
        ASSERT(0 == bslmf_IsClass<Union *const         >::VALUE);
        ASSERT(0 == bslmf_IsClass<Union *volatile      >::VALUE);
        ASSERT(0 == bslmf_IsClass<Union *const volatile>::VALUE);

        ASSERT(1 == bslmf_IsClass<BaseClass               >::VALUE);
        ASSERT(1 == bslmf_IsClass<BaseClass const         >::VALUE);
        ASSERT(1 == bslmf_IsClass<BaseClass volatile      >::VALUE);
        ASSERT(1 == bslmf_IsClass<BaseClass const volatile>::VALUE);

        ASSERT(1 == bslmf_IsClass<BaseClass&               >::VALUE);
        ASSERT(1 == bslmf_IsClass<BaseClass const&         >::VALUE);
        ASSERT(1 == bslmf_IsClass<BaseClass volatile&      >::VALUE);
        ASSERT(1 == bslmf_IsClass<BaseClass const volatile&>::VALUE);

        ASSERT(0 == bslmf_IsClass<BaseClass *              >::VALUE);
        ASSERT(0 == bslmf_IsClass<BaseClass *const         >::VALUE);
        ASSERT(0 == bslmf_IsClass<BaseClass *volatile      >::VALUE);
        ASSERT(0 == bslmf_IsClass<BaseClass *const volatile>::VALUE);

        ASSERT(1 == bslmf_IsClass<DerivedClass               >::VALUE);
        ASSERT(1 == bslmf_IsClass<DerivedClass const         >::VALUE);
        ASSERT(1 == bslmf_IsClass<DerivedClass volatile      >::VALUE);
        ASSERT(1 == bslmf_IsClass<DerivedClass const volatile>::VALUE);

        ASSERT(1 == bslmf_IsClass<DerivedClass&               >::VALUE);
        ASSERT(1 == bslmf_IsClass<DerivedClass const&         >::VALUE);
        ASSERT(1 == bslmf_IsClass<DerivedClass volatile&      >::VALUE);
        ASSERT(1 == bslmf_IsClass<DerivedClass const volatile&>::VALUE);

        ASSERT(0 == bslmf_IsClass<DerivedClass *              >::VALUE);
        ASSERT(0 == bslmf_IsClass<DerivedClass *const         >::VALUE);
        ASSERT(0 == bslmf_IsClass<DerivedClass *volatile      >::VALUE);
        ASSERT(0 == bslmf_IsClass<DerivedClass *const volatile>::VALUE);

        ASSERT(1 == bslmf_IsClass<Incomplete  >::VALUE);
        ASSERT(1 == bslmf_IsClass<Incomplete& >::VALUE);
        ASSERT(0 == bslmf_IsClass<Incomplete *>::VALUE);

        ASSERT(0 == bslmf_IsClass<int Struct::*  >::VALUE);
        ASSERT(0 == bslmf_IsClass<int Struct::*& >::VALUE);
        ASSERT(0 == bslmf_IsClass<int Struct::* *>::VALUE);

        ASSERT(0 == bslmf_IsClass<PMD BaseClass::*  >::VALUE);
        ASSERT(0 == bslmf_IsClass<PMD BaseClass::*& >::VALUE);
        ASSERT(0 == bslmf_IsClass<PMD BaseClass::* *>::VALUE);

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
