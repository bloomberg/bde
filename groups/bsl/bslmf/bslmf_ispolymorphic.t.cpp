// bslmf_ispolymorphic.t.cpp                                          -*-C++-*-

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
// [ 1] bslmf::IsPolymorphic
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

class Derived : public Base {
};

class DerivedPoly : public Base {
    virtual ~DerivedPoly();
};

class DerivedPolyThrowSpec : public Base {
#ifdef BDE_BUILD_TARGET_EXC
    virtual ~DerivedPolyThrowSpec() throw();
#else
    virtual ~DerivedPolyThrowSpec();
#endif
};

class Poly {
    virtual ~Poly();
};

class DerivedFromPoly : public Poly {
    ~DerivedFromPoly();
};

class DerivedFromPolyThrowSpec : public Poly {
#ifdef BDE_BUILD_TARGET_EXC
    ~DerivedFromPolyThrowSpec() throw();
#else
    ~DerivedFromPolyThrowSpec();
#endif
};

class PolyThrowSpec {
#ifdef BDE_BUILD_TARGET_EXC
    virtual ~PolyThrowSpec() throw();
#else
    virtual ~PolyThrowSpec();
#endif
};

class DerivedFromPolyThrowSpec2 : public PolyThrowSpec {
#ifdef BDE_BUILD_TARGET_EXC
    ~DerivedFromPolyThrowSpec2() throw();
#else
    ~DerivedFromPolyThrowSpec2();
#endif
};

class Base1 : public virtual Base {
};

class Base2 : public virtual Base {
};

class VirtuallyDerived : public Base1, public Base2 {
};

class PolyVirtuallyDerived : public Base1, public Base2 {
#ifdef BDE_BUILD_TARGET_EXC
    virtual ~PolyVirtuallyDerived() throw();
#else
    virtual ~PolyVirtuallyDerived();
#endif
};

class Poly1 : public virtual Poly {
};

class Poly2 : public virtual Poly {
};

class VirtuallyDerivedFromPoly : public Poly1, public Poly2 {
};

class VirtuallyDerivedMixed1 : public Base1, public Poly2 {
};

class VirtuallyDerivedMixed2 : public Poly1, public Base2 {
};


class Multi1 : public Base {
};

class Multi2 : public Base {
};

class MultiplyDerived : public Multi1, public Multi2 {
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
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating use of 'bslmf::IsPolymorphic'.
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
     ASSERT(0 == bslmf::IsPolymorphic<MyStruct          >::VALUE);
     ASSERT(0 == bslmf::IsPolymorphic<MyStruct         *>::VALUE);
     ASSERT(0 == bslmf::IsPolymorphic<MyDerivedStruct&  >::VALUE);
     ASSERT(0 == bslmf::IsPolymorphic<MyDerivedStruct  *>::VALUE);
//
     ASSERT(1 == bslmf::IsPolymorphic<      MyClass    >::VALUE);
     ASSERT(1 == bslmf::IsPolymorphic<const MyClass&   >::VALUE);
     ASSERT(0 == bslmf::IsPolymorphic<      MyClass   *>::VALUE);
     ASSERT(1 == bslmf::IsPolymorphic<MyDerivedClass&  >::VALUE);
     ASSERT(0 == bslmf::IsPolymorphic<MyDerivedClass  *>::VALUE);
//..

// ... continued from above
     ASSERT(1 == bslmf::IsPolymorphic<MyIncorrectPolymorphicClass&  >::VALUE);
     ASSERT(0 == bslmf::IsPolymorphic<MyIncorrectPolymorphicClass  *>::VALUE);
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Corner cases
        //   There are some dark corners of the type system that in principle
        //   we should not care about as their use is unlikely to pass a code
        //   review.  However, it is important to record the limits of this
        //   component in corner cases, so we add tests for known issues as
        //   they arise.
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

        if (verbose) cout << endl
                          << "Testing corner cases" << endl
                          << "====================" << endl;

#if defined(BSLMF_ISPOLYMORPHIC_HAS_INTRINSIC) || \
    defined(BSLS_PLATFORM__CMP_IBM)
        static const int EXP = 0;
#else
        static const int EXP = 1;
#endif

        ASSERT(EXP == bslmf::IsPolymorphic<Base1               >::VALUE);
        ASSERT(EXP == bslmf::IsPolymorphic<Base1 const         >::VALUE);
        ASSERT(EXP == bslmf::IsPolymorphic<Base1 volatile      >::VALUE);
        ASSERT(EXP == bslmf::IsPolymorphic<Base1 const volatile>::VALUE);

        ASSERT(EXP == bslmf::IsPolymorphic<Base1&               >::VALUE);
        ASSERT(EXP == bslmf::IsPolymorphic<Base1 const&         >::VALUE);
        ASSERT(EXP == bslmf::IsPolymorphic<Base1 volatile&      >::VALUE);
        ASSERT(EXP == bslmf::IsPolymorphic<Base1 const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Base1 *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Base1 *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Base1 *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Base1 *const volatile>::VALUE);

        ASSERT(EXP == bslmf::IsPolymorphic<Base2               >::VALUE);
        ASSERT(EXP == bslmf::IsPolymorphic<Base2 const         >::VALUE);
        ASSERT(EXP == bslmf::IsPolymorphic<Base2 volatile      >::VALUE);
        ASSERT(EXP == bslmf::IsPolymorphic<Base2 const volatile>::VALUE);

        ASSERT(EXP == bslmf::IsPolymorphic<Base2&               >::VALUE);
        ASSERT(EXP == bslmf::IsPolymorphic<Base2 const&         >::VALUE);
        ASSERT(EXP == bslmf::IsPolymorphic<Base2 volatile&      >::VALUE);
        ASSERT(EXP == bslmf::IsPolymorphic<Base2 const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Base2 *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Base2 *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Base2 *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Base2 *const volatile>::VALUE);

        ASSERT(EXP == bslmf::IsPolymorphic<
                                      VirtuallyDerived               >::VALUE);
        ASSERT(EXP == bslmf::IsPolymorphic<
                                      VirtuallyDerived const         >::VALUE);
        ASSERT(EXP == bslmf::IsPolymorphic<
                                      VirtuallyDerived volatile      >::VALUE);
        ASSERT(EXP == bslmf::IsPolymorphic<
                                      VirtuallyDerived const volatile>::VALUE);

        ASSERT(EXP == bslmf::IsPolymorphic<
                                      VirtuallyDerived&              >::VALUE);
        ASSERT(EXP == bslmf::IsPolymorphic<
                                     VirtuallyDerived const&         >::VALUE);
        ASSERT(EXP == bslmf::IsPolymorphic<
                                     VirtuallyDerived volatile&      >::VALUE);
        ASSERT(EXP == bslmf::IsPolymorphic<
                                     VirtuallyDerived const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<
                                     VirtuallyDerived *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                                     VirtuallyDerived *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                                     VirtuallyDerived *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                                     VirtuallyDerived *const volatile>::VALUE);


        ASSERT(0 == bslmf::IsPolymorphic<Multi1               >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Multi1 const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Multi1 volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Multi1 const volatile>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Multi1&               >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Multi1 const&         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Multi1 volatile&      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Multi1 const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Multi1 *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Multi1 *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Multi1 *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Multi1 *const volatile>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Multi2               >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Multi2 const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Multi2 volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Multi2 const volatile>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Multi2&               >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Multi2 const&         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Multi2 volatile&      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Multi2 const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Multi2 *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Multi2 *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Multi2 *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Multi2 *const volatile>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<MultiplyDerived         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<MultiplyDerived const   >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<MultiplyDerived volatile>::VALUE);
        ASSERT(0 ==
                 bslmf::IsPolymorphic<MultiplyDerived const volatile>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<MultiplyDerived&         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<MultiplyDerived const&   >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<MultiplyDerived volatile&>::VALUE);
        ASSERT(0 ==
                bslmf::IsPolymorphic<MultiplyDerived const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<MultiplyDerived *        >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<MultiplyDerived *const   >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<MultiplyDerived *volatile>::VALUE);
        ASSERT(0 ==
                bslmf::IsPolymorphic<MultiplyDerived *const volatile>::VALUE);


        ASSERT(1 == bslmf::IsPolymorphic<Poly1               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<Poly1 const         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<Poly1 volatile      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<Poly1 const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<Poly1&               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<Poly1 const&         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<Poly1 volatile&      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<Poly1 const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Poly1 *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Poly1 *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Poly1 *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Poly1 *const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<Poly2               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<Poly2 const         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<Poly2 volatile      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<Poly2 const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<Poly2&               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<Poly2 const&         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<Poly2 volatile&      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<Poly2 const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Poly2 *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Poly2 *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Poly2 *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Poly2 *const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<
                              VirtuallyDerivedFromPoly               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                              VirtuallyDerivedFromPoly const         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                              VirtuallyDerivedFromPoly volatile      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                              VirtuallyDerivedFromPoly const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<
                             VirtuallyDerivedFromPoly&         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                             VirtuallyDerivedFromPoly const&   >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                             VirtuallyDerivedFromPoly volatile&>::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                             VirtuallyDerivedFromPoly const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<
                             VirtuallyDerivedFromPoly *        >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                             VirtuallyDerivedFromPoly *const   >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                             VirtuallyDerivedFromPoly *volatile>::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                             VirtuallyDerivedFromPoly *const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<
                                VirtuallyDerivedMixed1               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                VirtuallyDerivedMixed1 const         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                VirtuallyDerivedMixed1 volatile      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                VirtuallyDerivedMixed1 const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed1&               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed1 const&         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed1 volatile&      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed1 const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed1 *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed1 *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed1 *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed1 *const volatile>::VALUE);


        ASSERT(1 == bslmf::IsPolymorphic<
                                VirtuallyDerivedMixed2               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                VirtuallyDerivedMixed2 const         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                VirtuallyDerivedMixed2 volatile      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                VirtuallyDerivedMixed2 const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed2&               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed2 const&         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed2 volatile&      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed2 const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed2 *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed2 *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed2 *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed2 *const volatile>::VALUE);


        ASSERT(1 == bslmf::IsPolymorphic<
                                  PolyVirtuallyDerived               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                  PolyVirtuallyDerived const         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                  PolyVirtuallyDerived volatile      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                  PolyVirtuallyDerived const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<
                                 PolyVirtuallyDerived&               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                 PolyVirtuallyDerived const&         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                 PolyVirtuallyDerived volatile&      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                 PolyVirtuallyDerived const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<
                                 PolyVirtuallyDerived *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                                 PolyVirtuallyDerived *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                                 PolyVirtuallyDerived *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                                 PolyVirtuallyDerived *const volatile>::VALUE);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf::IsPolymorphic' with various types and verify
        //   that their 'VALUE' member is initialized properly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bslmf::IsPolymorphic" << endl
                          << "====================" << endl;

        ASSERT(0 == bslmf::IsPolymorphic<void>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<int               >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<int const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<int volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<int const volatile>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<int&               >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<int const&         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<int volatile&      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<int const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Enum               >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Enum const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Enum volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Enum const volatile>::VALUE);

        ASSERT(0 ==
              bslmf::IsPolymorphic<int *                             >::VALUE);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int *const                        >::VALUE);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int *volatile                     >::VALUE);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int *const volatile               >::VALUE);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int const *                       >::VALUE);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int const *const                  >::VALUE);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int const *volatile               >::VALUE);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int const *const volatile         >::VALUE);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int volatile *                    >::VALUE);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int volatile *const               >::VALUE);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int volatile *volatile            >::VALUE);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int volatile *const volatile      >::VALUE);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int const volatile *              >::VALUE);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int const volatile *const         >::VALUE);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int const volatile *volatile      >::VALUE);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int const volatile *const volatile>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Struct               >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Struct const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Struct volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Struct const volatile>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Struct&               >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Struct const&         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Struct volatile&      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Struct const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Struct *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Struct *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Struct *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Struct *const volatile>::VALUE);

// The following cases fail to compile on platforms that do not support an
// instrinsic operation to check this trait.  It may be possible to detect
// 'union' types in C++11 with extended SFINAE, but all compilers known to
// implement extended SFINAE also provide an appropriate traits intrinsic.
#if defined(BSLMF_ISPOLYMORPHIC_HAS_INTRINSIC)
        ASSERT(0 == bslmf::IsPolymorphic<Union               >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Union const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Union volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Union const volatile>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Union&               >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Union const&         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Union volatile&      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Union const volatile&>::VALUE);
#endif

        ASSERT(0 == bslmf::IsPolymorphic<Union *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Union *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Union *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Union *const volatile>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Base               >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Base const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Base volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Base const volatile>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Base&               >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Base const&         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Base volatile&      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Base const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Base *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Base *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Base *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Base *const volatile>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Derived               >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Derived const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Derived volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Derived const volatile>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Derived&               >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Derived const&         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Derived volatile&      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Derived const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Derived *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Derived *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Derived *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Derived *const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<DerivedPoly               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<DerivedPoly const         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<DerivedPoly volatile      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<DerivedPoly const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<DerivedPoly&               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<DerivedPoly const&         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<DerivedPoly volatile&      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<DerivedPoly const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<DerivedPoly *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<DerivedPoly *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<DerivedPoly *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<DerivedPoly *const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec const         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec volatile      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec&               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec const&         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec volatile&      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec *const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<Poly               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<Poly const         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<Poly volatile      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<Poly const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<Poly&               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<Poly const&         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<Poly volatile&      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<Poly const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<Poly *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Poly *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Poly *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<Poly *const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<
                                     DerivedFromPoly               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                     DerivedFromPoly const         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                     DerivedFromPoly volatile      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                     DerivedFromPoly const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<
                                     DerivedFromPoly&               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                     DerivedFromPoly const&         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                     DerivedFromPoly volatile&      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                     DerivedFromPoly const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<
                                     DerivedFromPoly *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                                     DerivedFromPoly *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                                     DerivedFromPoly *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                                     DerivedFromPoly *const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<
                             DerivedFromPolyThrowSpec               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                             DerivedFromPolyThrowSpec const         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                             DerivedFromPolyThrowSpec volatile      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                             DerivedFromPolyThrowSpec const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<
                            DerivedFromPolyThrowSpec&               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                            DerivedFromPolyThrowSpec const&         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                            DerivedFromPolyThrowSpec volatile&      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                            DerivedFromPolyThrowSpec const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<
                            DerivedFromPolyThrowSpec *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                            DerivedFromPolyThrowSpec *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                            DerivedFromPolyThrowSpec *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                            DerivedFromPolyThrowSpec *const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<PolyThrowSpec               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<PolyThrowSpec const         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<PolyThrowSpec volatile      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<PolyThrowSpec const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<
                                        PolyThrowSpec&               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                        PolyThrowSpec const&         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                        PolyThrowSpec volatile&      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                                        PolyThrowSpec const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<
                                        PolyThrowSpec *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                                        PolyThrowSpec *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                                        PolyThrowSpec *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                                        PolyThrowSpec *const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 const         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 volatile      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2&               >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 const&         >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 volatile&      >::VALUE);
        ASSERT(1 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 const volatile&>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 *              >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 *const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 *volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 *const volatile>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<int Struct::*               >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<int Struct::* const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<int Struct::* volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<int Struct::* const volatile>::VALUE);

        ASSERT(0 == bslmf::IsPolymorphic<int Poly::*               >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<int Poly::* const         >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<int Poly::* volatile      >::VALUE);
        ASSERT(0 == bslmf::IsPolymorphic<int Poly::* const volatile>::VALUE);
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
