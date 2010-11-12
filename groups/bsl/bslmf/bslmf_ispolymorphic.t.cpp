// bslmf_ispolymorphic.t.cpp            -*-C++-*-

#include <bslmf_ispolymorphic.h>

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
// [ 1] bslmf_IsPolymorphic
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

class Base {};
class Derived : public Base {};
class DerivedPoly : public Base {
    virtual ~DerivedPoly();
};

class Poly {
    virtual ~Poly();
};
class DerivedOtherPoly : public Poly {
    ~DerivedOtherPoly();
};

//=============================================================================
//                 CLASSES, ETC. FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage
///-----
// For example:
//..
     struct MyStruct {
         void nonvirtualMethod();
     };
     struct MyDerivedStruct : public MyStruct {};
//..
// defines a non-polymorphic hierarchy, while:
//..
     class MyClass {
         MyClass();
         virtual ~MyClass();  // makes 'MyClass' polymorphic
     };

     class MyDerivedClass : public MyClass {
         MyDerivedClass();
         ~MyDerivedClass();
     };
//..
// defines a polymorphic hierarchy.  With these definitions:
//..

// ... continued below

// The following class is detected as polymorphic by this component, but should
// really have a virtual destructor.  Note that gcc issues a warning for such
// infractions.
//..
     class MyIncorrectPolymorphicClass {

         MyIncorrectPolymorphicClass();
         ~MyIncorrectPolymorphicClass();
         virtual void virtualMethod();
     };
//..

// ... continued below

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
        //   Simple example illustrating use of 'bslmf_IsPolymorphic'.
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
     ASSERT(0 == bslmf_IsPolymorphic<MyStruct          >::VALUE);
     ASSERT(0 == bslmf_IsPolymorphic<MyStruct         *>::VALUE);
     ASSERT(0 == bslmf_IsPolymorphic<MyDerivedStruct&  >::VALUE);
     ASSERT(0 == bslmf_IsPolymorphic<MyDerivedStruct  *>::VALUE);
//
     ASSERT(1 == bslmf_IsPolymorphic<      MyClass    >::VALUE);
     ASSERT(1 == bslmf_IsPolymorphic<const MyClass&   >::VALUE);
     ASSERT(0 == bslmf_IsPolymorphic<      MyClass   *>::VALUE);
     ASSERT(1 == bslmf_IsPolymorphic<MyDerivedClass&  >::VALUE);
     ASSERT(0 == bslmf_IsPolymorphic<MyDerivedClass  *>::VALUE);
//..

// ... continued from above
     ASSERT(1 == bslmf_IsPolymorphic<MyIncorrectPolymorphicClass&  >::VALUE);
     ASSERT(0 == bslmf_IsPolymorphic<MyIncorrectPolymorphicClass  *>::VALUE);
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf_IsPolymorphic' with various types and verify
        //   that their 'VALUE' member is initialized properly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bslmf_IsPolymorphic" << endl
                          << "===================" << endl;

        ASSERT(0 == bslmf_IsPolymorphic<void>::VALUE);

        ASSERT(0 == bslmf_IsPolymorphic<int               >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<int const         >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<int volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<int const volatile>::VALUE);

        ASSERT(0 == bslmf_IsPolymorphic<int&               >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<int const&         >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<int volatile&      >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<int const volatile&>::VALUE);

        ASSERT(0 == bslmf_IsPolymorphic<Enum               >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Enum const         >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Enum volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Enum const volatile>::VALUE);

        ASSERT(0 ==
               bslmf_IsPolymorphic<int *                             >::VALUE);
        ASSERT(0 ==
               bslmf_IsPolymorphic<int *const                        >::VALUE);
        ASSERT(0 ==
               bslmf_IsPolymorphic<int *volatile                     >::VALUE);
        ASSERT(0 ==
               bslmf_IsPolymorphic<int *const volatile               >::VALUE);
        ASSERT(0 ==
               bslmf_IsPolymorphic<int const *                       >::VALUE);
        ASSERT(0 ==
               bslmf_IsPolymorphic<int const *const                  >::VALUE);
        ASSERT(0 ==
               bslmf_IsPolymorphic<int const *volatile               >::VALUE);
        ASSERT(0 ==
               bslmf_IsPolymorphic<int const *const volatile         >::VALUE);
        ASSERT(0 ==
               bslmf_IsPolymorphic<int volatile *                    >::VALUE);
        ASSERT(0 ==
               bslmf_IsPolymorphic<int volatile *const               >::VALUE);
        ASSERT(0 ==
               bslmf_IsPolymorphic<int volatile *volatile            >::VALUE);
        ASSERT(0 ==
               bslmf_IsPolymorphic<int volatile *const volatile      >::VALUE);
        ASSERT(0 ==
               bslmf_IsPolymorphic<int const volatile *              >::VALUE);
        ASSERT(0 ==
               bslmf_IsPolymorphic<int const volatile *const         >::VALUE);
        ASSERT(0 ==
               bslmf_IsPolymorphic<int const volatile *volatile      >::VALUE);
        ASSERT(0 ==
               bslmf_IsPolymorphic<int const volatile *const volatile>::VALUE);

        ASSERT(0 == bslmf_IsPolymorphic<Struct               >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Struct const         >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Struct volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Struct const volatile>::VALUE);

        ASSERT(0 == bslmf_IsPolymorphic<Struct&               >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Struct const&         >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Struct volatile&      >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Struct const volatile&>::VALUE);

        ASSERT(0 == bslmf_IsPolymorphic<Struct *              >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Struct *const         >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Struct *volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Struct *const volatile>::VALUE);

// TBD
// The following cases fail to compile on all platforms.  To date, it is not
// possible to detect 'union' types in C++.
#if 0
        ASSERT(0 == bslmf_IsPolymorphic<Union               >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Union const         >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Union volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Union const volatile>::VALUE);

        ASSERT(0 == bslmf_IsPolymorphic<Union&               >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Union const&         >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Union volatile&      >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Union const volatile&>::VALUE);
#endif

        ASSERT(0 == bslmf_IsPolymorphic<Union *              >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Union *const         >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Union *volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Union *const volatile>::VALUE);

        ASSERT(0 == bslmf_IsPolymorphic<Base               >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Base const         >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Base volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Base const volatile>::VALUE);

        ASSERT(0 == bslmf_IsPolymorphic<Base&               >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Base const&         >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Base volatile&      >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Base const volatile&>::VALUE);

        ASSERT(0 == bslmf_IsPolymorphic<Base *              >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Base *const         >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Base *volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Base *const volatile>::VALUE);

        ASSERT(0 == bslmf_IsPolymorphic<Derived               >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Derived const         >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Derived volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Derived const volatile>::VALUE);

        ASSERT(0 == bslmf_IsPolymorphic<Derived&               >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Derived const&         >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Derived volatile&      >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Derived const volatile&>::VALUE);

        ASSERT(0 == bslmf_IsPolymorphic<Derived *              >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Derived *const         >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Derived *volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Derived *const volatile>::VALUE);

        ASSERT(1 == bslmf_IsPolymorphic<DerivedPoly               >::VALUE);
        ASSERT(1 == bslmf_IsPolymorphic<DerivedPoly const         >::VALUE);
        ASSERT(1 == bslmf_IsPolymorphic<DerivedPoly volatile      >::VALUE);
        ASSERT(1 == bslmf_IsPolymorphic<DerivedPoly const volatile>::VALUE);

        ASSERT(1 == bslmf_IsPolymorphic<DerivedPoly&               >::VALUE);
        ASSERT(1 == bslmf_IsPolymorphic<DerivedPoly const&         >::VALUE);
        ASSERT(1 == bslmf_IsPolymorphic<DerivedPoly volatile&      >::VALUE);
        ASSERT(1 == bslmf_IsPolymorphic<DerivedPoly const volatile&>::VALUE);

        ASSERT(0 == bslmf_IsPolymorphic<DerivedPoly *              >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<DerivedPoly *const         >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<DerivedPoly *volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<DerivedPoly *const volatile>::VALUE);

        ASSERT(1 == bslmf_IsPolymorphic<Poly               >::VALUE);
        ASSERT(1 == bslmf_IsPolymorphic<Poly const         >::VALUE);
        ASSERT(1 == bslmf_IsPolymorphic<Poly volatile      >::VALUE);
        ASSERT(1 == bslmf_IsPolymorphic<Poly const volatile>::VALUE);

        ASSERT(1 == bslmf_IsPolymorphic<Poly&               >::VALUE);
        ASSERT(1 == bslmf_IsPolymorphic<Poly const&         >::VALUE);
        ASSERT(1 == bslmf_IsPolymorphic<Poly volatile&      >::VALUE);
        ASSERT(1 == bslmf_IsPolymorphic<Poly const volatile&>::VALUE);

        ASSERT(0 == bslmf_IsPolymorphic<Poly *              >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Poly *const         >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Poly *volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<Poly *const volatile>::VALUE);

        ASSERT(1 ==
                  bslmf_IsPolymorphic<DerivedOtherPoly               >::VALUE);
        ASSERT(1 ==
                  bslmf_IsPolymorphic<DerivedOtherPoly const         >::VALUE);
        ASSERT(1 ==
                  bslmf_IsPolymorphic<DerivedOtherPoly volatile      >::VALUE);
        ASSERT(1 ==
                  bslmf_IsPolymorphic<DerivedOtherPoly const volatile>::VALUE);

        ASSERT(1 ==
                 bslmf_IsPolymorphic<DerivedOtherPoly&               >::VALUE);
        ASSERT(1 ==
                 bslmf_IsPolymorphic<DerivedOtherPoly const&         >::VALUE);
        ASSERT(1 ==
                 bslmf_IsPolymorphic<DerivedOtherPoly volatile&      >::VALUE);
        ASSERT(1 ==
                 bslmf_IsPolymorphic<DerivedOtherPoly const volatile&>::VALUE);

        ASSERT(0 ==
                 bslmf_IsPolymorphic<DerivedOtherPoly *              >::VALUE);
        ASSERT(0 ==
                 bslmf_IsPolymorphic<DerivedOtherPoly *const         >::VALUE);
        ASSERT(0 ==
                 bslmf_IsPolymorphic<DerivedOtherPoly *volatile      >::VALUE);
        ASSERT(0 ==
                 bslmf_IsPolymorphic<DerivedOtherPoly *const volatile>::VALUE);

        ASSERT(0 == bslmf_IsPolymorphic<int Struct::*               >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<int Struct::* const         >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<int Struct::* volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<int Struct::* const volatile>::VALUE);

        ASSERT(0 == bslmf_IsPolymorphic<int Poly::*               >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<int Poly::* const         >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<int Poly::* volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPolymorphic<int Poly::* const volatile>::VALUE);

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
