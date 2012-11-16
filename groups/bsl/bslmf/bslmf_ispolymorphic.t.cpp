// bslmf_ispolymorphic.t.cpp                                          -*-C++-*-

#include <bslmf_ispolymorphic.h>

#include <bsls_bsltestutil.h>

#include <cstdlib>
#include <cstdio>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under defines two meta-functions, 'bsl::is_polymorphic' and
// 'bslmf::IsPolymorphic', that determine whether a template parameter type is
// a polymorphic type.  Thus, we need to ensure that the values returned by
// these meta-functions are correct for each possible category of types.  Since
// the two meta-functions are functionally equivalent, we will use the same set
// of types for both.
//
//-----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// [ 2] BloombergLP::bslmf::IsPolymorphic
// [ 4] bsl::is_polymorphic
//
// ----------------------------------------------------------------------------
// [ 5] USAGE EXAMPLE

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

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
    ~Poly1();
};

class Poly2 : public virtual Poly {
    ~Poly2();
};

class VirtuallyDerivedFromPoly : public Poly1, public Poly2 {
    ~VirtuallyDerivedFromPoly();
};

class VirtuallyDerivedMixed1 : public Base1, public Poly2 {
    ~VirtuallyDerivedMixed1();
};

class VirtuallyDerivedMixed2 : public Poly1, public Base2 {
    ~VirtuallyDerivedMixed2();
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
// In this section we show intended use of this component.
//
///Example 1: Verify Polymorphic Types
///- - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a polymorphic
// type.
//
// First, we define two types in a non-polymorphic hierarchy, 'MyStruct' and
// 'MyDerivedStruct':
//..
struct MyStruct {
    void nonvirtualMethod();
};
struct MyDerivedStruct : public MyStruct {
};
//..
// Then, we define two types in a polymorphic hierarchy, 'MyClass' and
// 'MyDerivedClass':
//..
class MyClass {
    MyClass();
    virtual ~MyClass();  // makes 'MyClass' polymorphic
};

class MyDerivedClass : public MyClass {
    MyDerivedClass();
    ~MyDerivedClass();
};

// ... continued below

// Finally, note that the following class is detected as polymorphic by this
// component, but should really have a virtual destructor ('gcc' issues a
// warning for such infractions):
//..

class MyIncorrectPolymorphicClass {

    MyIncorrectPolymorphicClass();
    ~MyIncorrectPolymorphicClass();
    virtual void virtualMethod();
};

// ... continued below

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE\n"
                            "\n=============\n");

//..
// Now, assert that the two types in the non-polymorphic hierarchy are not
// polymorphic, and the two types in the polymorphic hierarchy are polymorphic
// using 'bsl::is_polymorphic':
//..
        ASSERT(false == bsl::is_polymorphic<MyStruct          >::value);
        ASSERT(false == bsl::is_polymorphic<MyStruct         *>::value);
        ASSERT(false == bsl::is_polymorphic<MyDerivedStruct&  >::value);
        ASSERT(false == bsl::is_polymorphic<MyDerivedStruct  *>::value);

        ASSERT(true  == bsl::is_polymorphic<      MyClass    >::value);
        ASSERT(false == bsl::is_polymorphic<const MyClass&   >::value);
        ASSERT(false == bsl::is_polymorphic<      MyClass   *>::value);
        ASSERT(false == bsl::is_polymorphic<MyDerivedClass&  >::value);
        ASSERT(false == bsl::is_polymorphic<MyDerivedClass  *>::value);
//..
        ASSERT(0 ==
                  bslmf::IsPolymorphic<MyIncorrectPolymorphicClass&  >::value);
        ASSERT(0 ==
                  bslmf::IsPolymorphic<MyIncorrectPolymorphicClass  *>::value);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'bsl::is_polymorphic' Corner Cases
        //   There are some dark corners of the type system that in principle
        //   we should not care about as their use is unlikely to pass a code
        //   review.  However, it is important to record the limits of this
        //   component in corner cases, so we add tests for known issues as
        //   they arise.
        // Concerns:
        //
        // Plan:
        //   Ensure that types using (possibly a combination of) virtual
        //   inheritance and multiple inheritance is evaluated correctly for
        //   different types of compilers.
        //
        // Tactics:
        //   - Add-Hoc Data Selection Method
        //   - Brute-Force implementation technique
        //
        // Testing:
        //   bsl::is_polymorphic
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bsl::is_polymorphic' Corner Cases\n"
                            "\n==================================\n");

#if defined(BSLMF_ISPOLYMORPHIC_HAS_INTRINSIC) || \
    defined(BSLS_PLATFORM__CMP_IBM)
        static const int EXP = 0;
#else
        static const int EXP = 1;
#endif

        ASSERT(EXP == bsl::is_polymorphic<Base1               >::value);
        ASSERT(EXP == bsl::is_polymorphic<Base1 const         >::value);
        ASSERT(EXP == bsl::is_polymorphic<Base1 volatile      >::value);
        ASSERT(EXP == bsl::is_polymorphic<Base1 const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<Base1 *              >::value);
        ASSERT(0 == bsl::is_polymorphic<Base1 *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<Base1 *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<Base1 *const volatile>::value);

        ASSERT(EXP == bsl::is_polymorphic<Base2               >::value);
        ASSERT(EXP == bsl::is_polymorphic<Base2 const         >::value);
        ASSERT(EXP == bsl::is_polymorphic<Base2 volatile      >::value);
        ASSERT(EXP == bsl::is_polymorphic<Base2 const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<Base2 *              >::value);
        ASSERT(0 == bsl::is_polymorphic<Base2 *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<Base2 *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<Base2 *const volatile>::value);

        ASSERT(EXP == bsl::is_polymorphic<
                                      VirtuallyDerived               >::value);
        ASSERT(EXP == bsl::is_polymorphic<
                                      VirtuallyDerived const         >::value);
        ASSERT(EXP == bsl::is_polymorphic<
                                      VirtuallyDerived volatile      >::value);
        ASSERT(EXP == bsl::is_polymorphic<
                                      VirtuallyDerived const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<
                                     VirtuallyDerived *              >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                     VirtuallyDerived *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                     VirtuallyDerived *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                     VirtuallyDerived *const volatile>::value);


        ASSERT(0 == bsl::is_polymorphic<Multi1               >::value);
        ASSERT(0 == bsl::is_polymorphic<Multi1 const         >::value);
        ASSERT(0 == bsl::is_polymorphic<Multi1 volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<Multi1 const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<Multi1&               >::value);
        ASSERT(0 == bsl::is_polymorphic<Multi1 const&         >::value);
        ASSERT(0 == bsl::is_polymorphic<Multi1 volatile&      >::value);
        ASSERT(0 == bsl::is_polymorphic<Multi1 const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<Multi1 *              >::value);
        ASSERT(0 == bsl::is_polymorphic<Multi1 *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<Multi1 *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<Multi1 *const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<Multi2               >::value);
        ASSERT(0 == bsl::is_polymorphic<Multi2 const         >::value);
        ASSERT(0 == bsl::is_polymorphic<Multi2 volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<Multi2 const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<Multi2&               >::value);
        ASSERT(0 == bsl::is_polymorphic<Multi2 const&         >::value);
        ASSERT(0 == bsl::is_polymorphic<Multi2 volatile&      >::value);
        ASSERT(0 == bsl::is_polymorphic<Multi2 const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<Multi2 *              >::value);
        ASSERT(0 == bsl::is_polymorphic<Multi2 *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<Multi2 *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<Multi2 *const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<MultiplyDerived         >::value);
        ASSERT(0 == bsl::is_polymorphic<MultiplyDerived const   >::value);
        ASSERT(0 == bsl::is_polymorphic<MultiplyDerived volatile>::value);
        ASSERT(0 ==
                 bsl::is_polymorphic<MultiplyDerived const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<MultiplyDerived&         >::value);
        ASSERT(0 == bsl::is_polymorphic<MultiplyDerived const&   >::value);
        ASSERT(0 == bsl::is_polymorphic<MultiplyDerived volatile&>::value);
        ASSERT(0 ==
                bsl::is_polymorphic<MultiplyDerived const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<MultiplyDerived *        >::value);
        ASSERT(0 == bsl::is_polymorphic<MultiplyDerived *const   >::value);
        ASSERT(0 == bsl::is_polymorphic<MultiplyDerived *volatile>::value);
        ASSERT(0 ==
                bsl::is_polymorphic<MultiplyDerived *const volatile>::value);


        ASSERT(1 == bsl::is_polymorphic<Poly1               >::value);
        ASSERT(1 == bsl::is_polymorphic<Poly1 const         >::value);
        ASSERT(1 == bsl::is_polymorphic<Poly1 volatile      >::value);
        ASSERT(1 == bsl::is_polymorphic<Poly1 const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<Poly1&               >::value);
        ASSERT(0 == bsl::is_polymorphic<Poly1 const&         >::value);
        ASSERT(0 == bsl::is_polymorphic<Poly1 volatile&      >::value);
        ASSERT(0 == bsl::is_polymorphic<Poly1 const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<Poly1 *              >::value);
        ASSERT(0 == bsl::is_polymorphic<Poly1 *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<Poly1 *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<Poly1 *const volatile>::value);

        ASSERT(1 == bsl::is_polymorphic<Poly2               >::value);
        ASSERT(1 == bsl::is_polymorphic<Poly2 const         >::value);
        ASSERT(1 == bsl::is_polymorphic<Poly2 volatile      >::value);
        ASSERT(1 == bsl::is_polymorphic<Poly2 const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<Poly2&               >::value);
        ASSERT(0 == bsl::is_polymorphic<Poly2 const&         >::value);
        ASSERT(0 == bsl::is_polymorphic<Poly2 volatile&      >::value);
        ASSERT(0 == bsl::is_polymorphic<Poly2 const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<Poly2 *              >::value);
        ASSERT(0 == bsl::is_polymorphic<Poly2 *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<Poly2 *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<Poly2 *const volatile>::value);

        ASSERT(1 == bsl::is_polymorphic<
                              VirtuallyDerivedFromPoly               >::value);
        ASSERT(1 == bsl::is_polymorphic<
                              VirtuallyDerivedFromPoly const         >::value);
        ASSERT(1 == bsl::is_polymorphic<
                              VirtuallyDerivedFromPoly volatile      >::value);
        ASSERT(1 == bsl::is_polymorphic<
                              VirtuallyDerivedFromPoly const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<
                             VirtuallyDerivedFromPoly&         >::value);
        ASSERT(0 == bsl::is_polymorphic<
                             VirtuallyDerivedFromPoly const&   >::value);
        ASSERT(0 == bsl::is_polymorphic<
                             VirtuallyDerivedFromPoly volatile&>::value);
        ASSERT(0 == bsl::is_polymorphic<
                             VirtuallyDerivedFromPoly const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<
                             VirtuallyDerivedFromPoly *        >::value);
        ASSERT(0 == bsl::is_polymorphic<
                             VirtuallyDerivedFromPoly *const   >::value);
        ASSERT(0 == bsl::is_polymorphic<
                             VirtuallyDerivedFromPoly *volatile>::value);
        ASSERT(0 == bsl::is_polymorphic<
                             VirtuallyDerivedFromPoly *const volatile>::value);

        ASSERT(1 == bsl::is_polymorphic<
                                VirtuallyDerivedMixed1               >::value);
        ASSERT(1 == bsl::is_polymorphic<
                                VirtuallyDerivedMixed1 const         >::value);
        ASSERT(1 == bsl::is_polymorphic<
                                VirtuallyDerivedMixed1 volatile      >::value);
        ASSERT(1 == bsl::is_polymorphic<
                                VirtuallyDerivedMixed1 const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<
                               VirtuallyDerivedMixed1&               >::value);
        ASSERT(0 == bsl::is_polymorphic<
                               VirtuallyDerivedMixed1 const&         >::value);
        ASSERT(0 == bsl::is_polymorphic<
                               VirtuallyDerivedMixed1 volatile&      >::value);
        ASSERT(0 == bsl::is_polymorphic<
                               VirtuallyDerivedMixed1 const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<
                               VirtuallyDerivedMixed1 *              >::value);
        ASSERT(0 == bsl::is_polymorphic<
                               VirtuallyDerivedMixed1 *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<
                               VirtuallyDerivedMixed1 *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<
                               VirtuallyDerivedMixed1 *const volatile>::value);


        ASSERT(1 == bsl::is_polymorphic<
                                VirtuallyDerivedMixed2               >::value);
        ASSERT(1 == bsl::is_polymorphic<
                                VirtuallyDerivedMixed2 const         >::value);
        ASSERT(1 == bsl::is_polymorphic<
                                VirtuallyDerivedMixed2 volatile      >::value);
        ASSERT(1 == bsl::is_polymorphic<
                                VirtuallyDerivedMixed2 const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<
                               VirtuallyDerivedMixed2&               >::value);
        ASSERT(0 == bsl::is_polymorphic<
                               VirtuallyDerivedMixed2 const&         >::value);
        ASSERT(0 == bsl::is_polymorphic<
                               VirtuallyDerivedMixed2 volatile&      >::value);
        ASSERT(0 == bsl::is_polymorphic<
                               VirtuallyDerivedMixed2 const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<
                               VirtuallyDerivedMixed2 *              >::value);
        ASSERT(0 == bsl::is_polymorphic<
                               VirtuallyDerivedMixed2 *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<
                               VirtuallyDerivedMixed2 *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<
                               VirtuallyDerivedMixed2 *const volatile>::value);


        ASSERT(1 == bsl::is_polymorphic<
                                  PolyVirtuallyDerived               >::value);
        ASSERT(1 == bsl::is_polymorphic<
                                  PolyVirtuallyDerived const         >::value);
        ASSERT(1 == bsl::is_polymorphic<
                                  PolyVirtuallyDerived volatile      >::value);
        ASSERT(1 == bsl::is_polymorphic<
                                  PolyVirtuallyDerived const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<
                                 PolyVirtuallyDerived&               >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                 PolyVirtuallyDerived const&         >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                 PolyVirtuallyDerived volatile&      >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                 PolyVirtuallyDerived const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<
                                 PolyVirtuallyDerived *              >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                 PolyVirtuallyDerived *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                 PolyVirtuallyDerived *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                 PolyVirtuallyDerived *const volatile>::value);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'bsl::is_polymorphic'
        //
        // Test Plan:
        //   Instantiate 'bsl::is_polymorphic' with various types and verify
        //   that their 'value' member is initialized properly.
        //
        // Testing:
        //   bsl::is_polymorphic
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bsl::is_polymorphic'\n"
                            "\n=====================\n");

        ASSERT(0 == bsl::is_polymorphic<void>::value);

        ASSERT(0 == bsl::is_polymorphic<int               >::value);
        ASSERT(0 == bsl::is_polymorphic<int const         >::value);
        ASSERT(0 == bsl::is_polymorphic<int volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<int const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<int&               >::value);
        ASSERT(0 == bsl::is_polymorphic<int const&         >::value);
        ASSERT(0 == bsl::is_polymorphic<int volatile&      >::value);
        ASSERT(0 == bsl::is_polymorphic<int const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<Enum               >::value);
        ASSERT(0 == bsl::is_polymorphic<Enum const         >::value);
        ASSERT(0 == bsl::is_polymorphic<Enum volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<Enum const volatile>::value);

        ASSERT(0 ==
              bsl::is_polymorphic<int *                             >::value);
        ASSERT(0 ==
              bsl::is_polymorphic<int *const                        >::value);
        ASSERT(0 ==
              bsl::is_polymorphic<int *volatile                     >::value);
        ASSERT(0 ==
              bsl::is_polymorphic<int *const volatile               >::value);
        ASSERT(0 ==
              bsl::is_polymorphic<int const *                       >::value);
        ASSERT(0 ==
              bsl::is_polymorphic<int const *const                  >::value);
        ASSERT(0 ==
              bsl::is_polymorphic<int const *volatile               >::value);
        ASSERT(0 ==
              bsl::is_polymorphic<int const *const volatile         >::value);
        ASSERT(0 ==
              bsl::is_polymorphic<int volatile *                    >::value);
        ASSERT(0 ==
              bsl::is_polymorphic<int volatile *const               >::value);
        ASSERT(0 ==
              bsl::is_polymorphic<int volatile *volatile            >::value);
        ASSERT(0 ==
              bsl::is_polymorphic<int volatile *const volatile      >::value);
        ASSERT(0 ==
              bsl::is_polymorphic<int const volatile *              >::value);
        ASSERT(0 ==
              bsl::is_polymorphic<int const volatile *const         >::value);
        ASSERT(0 ==
              bsl::is_polymorphic<int const volatile *volatile      >::value);
        ASSERT(0 ==
              bsl::is_polymorphic<int const volatile *const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<Struct               >::value);
        ASSERT(0 == bsl::is_polymorphic<Struct const         >::value);
        ASSERT(0 == bsl::is_polymorphic<Struct volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<Struct const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<Struct&               >::value);
        ASSERT(0 == bsl::is_polymorphic<Struct const&         >::value);
        ASSERT(0 == bsl::is_polymorphic<Struct volatile&      >::value);
        ASSERT(0 == bsl::is_polymorphic<Struct const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<Struct *              >::value);
        ASSERT(0 == bsl::is_polymorphic<Struct *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<Struct *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<Struct *const volatile>::value);

// The following cases fail to compile on platforms that do not support an
// instrinsic operation to check this trait.  It may be possible to detect
// 'union' types in C++11 with extended SFINAE, but all compilers known to
// implement extended SFINAE also provide an appropriate traits intrinsic.
#if defined(BSLMF_ISPOLYMORPHIC_HAS_INTRINSIC)
        ASSERT(0 == bsl::is_polymorphic<Union               >::value);
        ASSERT(0 == bsl::is_polymorphic<Union const         >::value);
        ASSERT(0 == bsl::is_polymorphic<Union volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<Union const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<Union&               >::value);
        ASSERT(0 == bsl::is_polymorphic<Union const&         >::value);
        ASSERT(0 == bsl::is_polymorphic<Union volatile&      >::value);
        ASSERT(0 == bsl::is_polymorphic<Union const volatile&>::value);
#endif

        ASSERT(0 == bsl::is_polymorphic<Union *              >::value);
        ASSERT(0 == bsl::is_polymorphic<Union *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<Union *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<Union *const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<Base               >::value);
        ASSERT(0 == bsl::is_polymorphic<Base const         >::value);
        ASSERT(0 == bsl::is_polymorphic<Base volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<Base const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<Base&               >::value);
        ASSERT(0 == bsl::is_polymorphic<Base const&         >::value);
        ASSERT(0 == bsl::is_polymorphic<Base volatile&      >::value);
        ASSERT(0 == bsl::is_polymorphic<Base const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<Base *              >::value);
        ASSERT(0 == bsl::is_polymorphic<Base *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<Base *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<Base *const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<Derived               >::value);
        ASSERT(0 == bsl::is_polymorphic<Derived const         >::value);
        ASSERT(0 == bsl::is_polymorphic<Derived volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<Derived const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<Derived&               >::value);
        ASSERT(0 == bsl::is_polymorphic<Derived const&         >::value);
        ASSERT(0 == bsl::is_polymorphic<Derived volatile&      >::value);
        ASSERT(0 == bsl::is_polymorphic<Derived const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<Derived *              >::value);
        ASSERT(0 == bsl::is_polymorphic<Derived *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<Derived *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<Derived *const volatile>::value);

        ASSERT(1 == bsl::is_polymorphic<DerivedPoly               >::value);
        ASSERT(1 == bsl::is_polymorphic<DerivedPoly const         >::value);
        ASSERT(1 == bsl::is_polymorphic<DerivedPoly volatile      >::value);
        ASSERT(1 == bsl::is_polymorphic<DerivedPoly const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<DerivedPoly&               >::value);
        ASSERT(0 == bsl::is_polymorphic<DerivedPoly const&         >::value);
        ASSERT(0 == bsl::is_polymorphic<DerivedPoly volatile&      >::value);
        ASSERT(0 == bsl::is_polymorphic<DerivedPoly const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<DerivedPoly *              >::value);
        ASSERT(0 == bsl::is_polymorphic<DerivedPoly *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<DerivedPoly *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<DerivedPoly *const volatile>::value);

        ASSERT(1 == bsl::is_polymorphic<
                                 DerivedPolyThrowSpec               >::value);
        ASSERT(1 == bsl::is_polymorphic<
                                 DerivedPolyThrowSpec const         >::value);
        ASSERT(1 == bsl::is_polymorphic<
                                 DerivedPolyThrowSpec volatile      >::value);
        ASSERT(1 == bsl::is_polymorphic<
                                 DerivedPolyThrowSpec const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<
                                 DerivedPolyThrowSpec&               >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                 DerivedPolyThrowSpec const&         >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                 DerivedPolyThrowSpec volatile&      >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                 DerivedPolyThrowSpec const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<
                                 DerivedPolyThrowSpec *              >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                 DerivedPolyThrowSpec *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                 DerivedPolyThrowSpec *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                 DerivedPolyThrowSpec *const volatile>::value);

        ASSERT(1 == bsl::is_polymorphic<Poly               >::value);
        ASSERT(1 == bsl::is_polymorphic<Poly const         >::value);
        ASSERT(1 == bsl::is_polymorphic<Poly volatile      >::value);
        ASSERT(1 == bsl::is_polymorphic<Poly const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<Poly&               >::value);
        ASSERT(0 == bsl::is_polymorphic<Poly const&         >::value);
        ASSERT(0 == bsl::is_polymorphic<Poly volatile&      >::value);
        ASSERT(0 == bsl::is_polymorphic<Poly const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<Poly *              >::value);
        ASSERT(0 == bsl::is_polymorphic<Poly *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<Poly *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<Poly *const volatile>::value);

        ASSERT(1 == bsl::is_polymorphic<
                                     DerivedFromPoly               >::value);
        ASSERT(1 == bsl::is_polymorphic<
                                     DerivedFromPoly const         >::value);
        ASSERT(1 == bsl::is_polymorphic<
                                     DerivedFromPoly volatile      >::value);
        ASSERT(1 == bsl::is_polymorphic<
                                     DerivedFromPoly const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<
                                     DerivedFromPoly&               >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                     DerivedFromPoly const&         >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                     DerivedFromPoly volatile&      >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                     DerivedFromPoly const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<
                                     DerivedFromPoly *              >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                     DerivedFromPoly *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                     DerivedFromPoly *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                     DerivedFromPoly *const volatile>::value);

        ASSERT(1 == bsl::is_polymorphic<
                             DerivedFromPolyThrowSpec               >::value);
        ASSERT(1 == bsl::is_polymorphic<
                             DerivedFromPolyThrowSpec const         >::value);
        ASSERT(1 == bsl::is_polymorphic<
                             DerivedFromPolyThrowSpec volatile      >::value);
        ASSERT(1 == bsl::is_polymorphic<
                             DerivedFromPolyThrowSpec const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<
                            DerivedFromPolyThrowSpec&               >::value);
        ASSERT(0 == bsl::is_polymorphic<
                            DerivedFromPolyThrowSpec const&         >::value);
        ASSERT(0 == bsl::is_polymorphic<
                            DerivedFromPolyThrowSpec volatile&      >::value);
        ASSERT(0 == bsl::is_polymorphic<
                            DerivedFromPolyThrowSpec const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<
                            DerivedFromPolyThrowSpec *              >::value);
        ASSERT(0 == bsl::is_polymorphic<
                            DerivedFromPolyThrowSpec *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<
                            DerivedFromPolyThrowSpec *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<
                            DerivedFromPolyThrowSpec *const volatile>::value);

        ASSERT(1 == bsl::is_polymorphic<PolyThrowSpec               >::value);
        ASSERT(1 == bsl::is_polymorphic<PolyThrowSpec const         >::value);
        ASSERT(1 == bsl::is_polymorphic<PolyThrowSpec volatile      >::value);
        ASSERT(1 == bsl::is_polymorphic<PolyThrowSpec const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<
                                        PolyThrowSpec&               >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                        PolyThrowSpec const&         >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                        PolyThrowSpec volatile&      >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                        PolyThrowSpec const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<
                                        PolyThrowSpec *              >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                        PolyThrowSpec *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                        PolyThrowSpec *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<
                                        PolyThrowSpec *const volatile>::value);

        ASSERT(1 == bsl::is_polymorphic<
                           DerivedFromPolyThrowSpec2               >::value);
        ASSERT(1 == bsl::is_polymorphic<
                           DerivedFromPolyThrowSpec2 const         >::value);
        ASSERT(1 == bsl::is_polymorphic<
                           DerivedFromPolyThrowSpec2 volatile      >::value);
        ASSERT(1 == bsl::is_polymorphic<
                           DerivedFromPolyThrowSpec2 const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<
                           DerivedFromPolyThrowSpec2&               >::value);
        ASSERT(0 == bsl::is_polymorphic<
                           DerivedFromPolyThrowSpec2 const&         >::value);
        ASSERT(0 == bsl::is_polymorphic<
                           DerivedFromPolyThrowSpec2 volatile&      >::value);
        ASSERT(0 == bsl::is_polymorphic<
                           DerivedFromPolyThrowSpec2 const volatile&>::value);

        ASSERT(0 == bsl::is_polymorphic<
                           DerivedFromPolyThrowSpec2 *              >::value);
        ASSERT(0 == bsl::is_polymorphic<
                           DerivedFromPolyThrowSpec2 *const         >::value);
        ASSERT(0 == bsl::is_polymorphic<
                           DerivedFromPolyThrowSpec2 *volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<
                           DerivedFromPolyThrowSpec2 *const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<int Struct::*               >::value);
        ASSERT(0 == bsl::is_polymorphic<int Struct::* const         >::value);
        ASSERT(0 == bsl::is_polymorphic<int Struct::* volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<int Struct::* const volatile>::value);

        ASSERT(0 == bsl::is_polymorphic<int Poly::*               >::value);
        ASSERT(0 == bsl::is_polymorphic<int Poly::* const         >::value);
        ASSERT(0 == bsl::is_polymorphic<int Poly::* volatile      >::value);
        ASSERT(0 == bsl::is_polymorphic<int Poly::* const volatile>::value);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'bslmf::IsPolymorphic' Corner Cases
        //   There are some dark corners of the type system that in principle
        //   we should not care about as their use is unlikely to pass a code
        //   review.  However, it is important to record the limits of this
        //   component in corner cases, so we add tests for known issues as
        //   they arise.
        // Concerns:
        //
        // Plan:
        //   Ensure that types using (possibly a combination of) virtual
        //   inheritance and multiple inheritance is evaluated correctly for
        //   different types of compilers.
        //
        // Tactics:
        //   - Add-Hoc Data Selection Method
        //   - Brute-Force implementation technique
        //
        // Testing:
        //   bslmf::IsPolymorphic
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bslmf::IsPolymorphic' Corner Cases\n"
                            "\n===================================\n");

#if defined(BSLMF_ISPOLYMORPHIC_HAS_INTRINSIC) || \
    defined(BSLS_PLATFORM_CMP_IBM)
        static const int EXP = 0;
#else
        static const int EXP = 1;
#endif

        ASSERT(EXP == bslmf::IsPolymorphic<Base1               >::value);
        ASSERT(EXP == bslmf::IsPolymorphic<Base1 const         >::value);
        ASSERT(EXP == bslmf::IsPolymorphic<Base1 volatile      >::value);
        ASSERT(EXP == bslmf::IsPolymorphic<Base1 const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Base1 *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Base1 *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Base1 *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Base1 *const volatile>::value);

        ASSERT(EXP == bslmf::IsPolymorphic<Base2               >::value);
        ASSERT(EXP == bslmf::IsPolymorphic<Base2 const         >::value);
        ASSERT(EXP == bslmf::IsPolymorphic<Base2 volatile      >::value);
        ASSERT(EXP == bslmf::IsPolymorphic<Base2 const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Base2 *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Base2 *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Base2 *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Base2 *const volatile>::value);

        ASSERT(EXP == bslmf::IsPolymorphic<
                                      VirtuallyDerived               >::value);
        ASSERT(EXP == bslmf::IsPolymorphic<
                                      VirtuallyDerived const         >::value);
        ASSERT(EXP == bslmf::IsPolymorphic<
                                      VirtuallyDerived volatile      >::value);
        ASSERT(EXP == bslmf::IsPolymorphic<
                                      VirtuallyDerived const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<
                                     VirtuallyDerived *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                     VirtuallyDerived *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                     VirtuallyDerived *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                     VirtuallyDerived *const volatile>::value);


        ASSERT(0 == bslmf::IsPolymorphic<Multi1               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Multi1 const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Multi1 volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Multi1 const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Multi1&               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Multi1 const&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Multi1 volatile&      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Multi1 const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Multi1 *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Multi1 *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Multi1 *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Multi1 *const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Multi2               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Multi2 const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Multi2 volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Multi2 const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Multi2&               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Multi2 const&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Multi2 volatile&      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Multi2 const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Multi2 *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Multi2 *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Multi2 *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Multi2 *const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<MultiplyDerived         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<MultiplyDerived const   >::value);
        ASSERT(0 == bslmf::IsPolymorphic<MultiplyDerived volatile>::value);
        ASSERT(0 ==
                 bslmf::IsPolymorphic<MultiplyDerived const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<MultiplyDerived&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<MultiplyDerived const&   >::value);
        ASSERT(0 == bslmf::IsPolymorphic<MultiplyDerived volatile&>::value);
        ASSERT(0 ==
                bslmf::IsPolymorphic<MultiplyDerived const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<MultiplyDerived *        >::value);
        ASSERT(0 == bslmf::IsPolymorphic<MultiplyDerived *const   >::value);
        ASSERT(0 == bslmf::IsPolymorphic<MultiplyDerived *volatile>::value);
        ASSERT(0 ==
                bslmf::IsPolymorphic<MultiplyDerived *const volatile>::value);


        ASSERT(1 == bslmf::IsPolymorphic<Poly1               >::value);
        ASSERT(1 == bslmf::IsPolymorphic<Poly1 const         >::value);
        ASSERT(1 == bslmf::IsPolymorphic<Poly1 volatile      >::value);
        ASSERT(1 == bslmf::IsPolymorphic<Poly1 const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Poly1&               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Poly1 const&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Poly1 volatile&      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Poly1 const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Poly1 *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Poly1 *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Poly1 *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Poly1 *const volatile>::value);

        ASSERT(1 == bslmf::IsPolymorphic<Poly2               >::value);
        ASSERT(1 == bslmf::IsPolymorphic<Poly2 const         >::value);
        ASSERT(1 == bslmf::IsPolymorphic<Poly2 volatile      >::value);
        ASSERT(1 == bslmf::IsPolymorphic<Poly2 const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Poly2&               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Poly2 const&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Poly2 volatile&      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Poly2 const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Poly2 *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Poly2 *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Poly2 *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Poly2 *const volatile>::value);

        ASSERT(1 == bslmf::IsPolymorphic<
                              VirtuallyDerivedFromPoly               >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                              VirtuallyDerivedFromPoly const         >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                              VirtuallyDerivedFromPoly volatile      >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                              VirtuallyDerivedFromPoly const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<
                             VirtuallyDerivedFromPoly&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                             VirtuallyDerivedFromPoly const&   >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                             VirtuallyDerivedFromPoly volatile&>::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                             VirtuallyDerivedFromPoly const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<
                             VirtuallyDerivedFromPoly *        >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                             VirtuallyDerivedFromPoly *const   >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                             VirtuallyDerivedFromPoly *volatile>::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                             VirtuallyDerivedFromPoly *const volatile>::value);

        ASSERT(1 == bslmf::IsPolymorphic<
                                VirtuallyDerivedMixed1               >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                                VirtuallyDerivedMixed1 const         >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                                VirtuallyDerivedMixed1 volatile      >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                                VirtuallyDerivedMixed1 const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed1&               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed1 const&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed1 volatile&      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed1 const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed1 *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed1 *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed1 *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed1 *const volatile>::value);


        ASSERT(1 == bslmf::IsPolymorphic<
                                VirtuallyDerivedMixed2               >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                                VirtuallyDerivedMixed2 const         >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                                VirtuallyDerivedMixed2 volatile      >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                                VirtuallyDerivedMixed2 const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed2&               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed2 const&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed2 volatile&      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed2 const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed2 *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed2 *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed2 *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                               VirtuallyDerivedMixed2 *const volatile>::value);


        ASSERT(1 == bslmf::IsPolymorphic<
                                  PolyVirtuallyDerived               >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                                  PolyVirtuallyDerived const         >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                                  PolyVirtuallyDerived volatile      >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                                  PolyVirtuallyDerived const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<
                                 PolyVirtuallyDerived&               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                 PolyVirtuallyDerived const&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                 PolyVirtuallyDerived volatile&      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                 PolyVirtuallyDerived const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<
                                 PolyVirtuallyDerived *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                 PolyVirtuallyDerived *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                 PolyVirtuallyDerived *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                 PolyVirtuallyDerived *const volatile>::value);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bslmf::IsPolymorphic'
        //
        // Test Plan:
        //   Instantiate 'bslmf::IsPolymorphic' with various types and verify
        //   that their 'VALUE' member is initialized properly.
        //
        // Testing:
        //   bslmf::IsPolymorphic
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bslmf::IsPolymorphic'\n"
                            "\n======================\n");

        ASSERT(0 == bslmf::IsPolymorphic<void>::value);

        ASSERT(0 == bslmf::IsPolymorphic<int               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<int const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<int volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<int const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<int&               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<int const&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<int volatile&      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<int const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Enum               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Enum const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Enum volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Enum const volatile>::value);

        ASSERT(0 ==
              bslmf::IsPolymorphic<int *                             >::value);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int *const                        >::value);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int *volatile                     >::value);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int *const volatile               >::value);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int const *                       >::value);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int const *const                  >::value);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int const *volatile               >::value);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int const *const volatile         >::value);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int volatile *                    >::value);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int volatile *const               >::value);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int volatile *volatile            >::value);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int volatile *const volatile      >::value);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int const volatile *              >::value);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int const volatile *const         >::value);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int const volatile *volatile      >::value);
        ASSERT(0 ==
              bslmf::IsPolymorphic<int const volatile *const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Struct               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Struct const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Struct volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Struct const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Struct&               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Struct const&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Struct volatile&      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Struct const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Struct *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Struct *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Struct *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Struct *const volatile>::value);

// The following cases fail to compile on platforms that do not support an
// instrinsic operation to check this trait.  It may be possible to detect
// 'union' types in C++11 with extended SFINAE, but all compilers known to
// implement extended SFINAE also provide an appropriate traits intrinsic.
#if defined(BSLMF_ISPOLYMORPHIC_HAS_INTRINSIC)
        ASSERT(0 == bslmf::IsPolymorphic<Union               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Union const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Union volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Union const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Union&               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Union const&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Union volatile&      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Union const volatile&>::value);
#endif

        ASSERT(0 == bslmf::IsPolymorphic<Union *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Union *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Union *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Union *const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Base               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Base const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Base volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Base const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Base&               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Base const&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Base volatile&      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Base const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Base *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Base *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Base *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Base *const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Derived               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Derived const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Derived volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Derived const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Derived&               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Derived const&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Derived volatile&      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Derived const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Derived *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Derived *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Derived *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Derived *const volatile>::value);

        ASSERT(1 == bslmf::IsPolymorphic<DerivedPoly               >::value);
        ASSERT(1 == bslmf::IsPolymorphic<DerivedPoly const         >::value);
        ASSERT(1 == bslmf::IsPolymorphic<DerivedPoly volatile      >::value);
        ASSERT(1 == bslmf::IsPolymorphic<DerivedPoly const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<DerivedPoly&               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<DerivedPoly const&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<DerivedPoly volatile&      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<DerivedPoly const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<DerivedPoly *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<DerivedPoly *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<DerivedPoly *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<DerivedPoly *const volatile>::value);

        ASSERT(1 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec               >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec const         >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec volatile      >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec&               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec const&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec volatile&      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                 DerivedPolyThrowSpec *const volatile>::value);

        ASSERT(1 == bslmf::IsPolymorphic<Poly               >::value);
        ASSERT(1 == bslmf::IsPolymorphic<Poly const         >::value);
        ASSERT(1 == bslmf::IsPolymorphic<Poly volatile      >::value);
        ASSERT(1 == bslmf::IsPolymorphic<Poly const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Poly&               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Poly const&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Poly volatile&      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Poly const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<Poly *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Poly *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Poly *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<Poly *const volatile>::value);

        ASSERT(1 == bslmf::IsPolymorphic<
                                     DerivedFromPoly               >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                                     DerivedFromPoly const         >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                                     DerivedFromPoly volatile      >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                                     DerivedFromPoly const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<
                                     DerivedFromPoly&               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                     DerivedFromPoly const&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                     DerivedFromPoly volatile&      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                     DerivedFromPoly const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<
                                     DerivedFromPoly *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                     DerivedFromPoly *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                     DerivedFromPoly *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                     DerivedFromPoly *const volatile>::value);

        ASSERT(1 == bslmf::IsPolymorphic<
                             DerivedFromPolyThrowSpec               >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                             DerivedFromPolyThrowSpec const         >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                             DerivedFromPolyThrowSpec volatile      >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                             DerivedFromPolyThrowSpec const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<
                            DerivedFromPolyThrowSpec&               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                            DerivedFromPolyThrowSpec const&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                            DerivedFromPolyThrowSpec volatile&      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                            DerivedFromPolyThrowSpec const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<
                            DerivedFromPolyThrowSpec *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                            DerivedFromPolyThrowSpec *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                            DerivedFromPolyThrowSpec *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                            DerivedFromPolyThrowSpec *const volatile>::value);

        ASSERT(1 == bslmf::IsPolymorphic<PolyThrowSpec               >::value);
        ASSERT(1 == bslmf::IsPolymorphic<PolyThrowSpec const         >::value);
        ASSERT(1 == bslmf::IsPolymorphic<PolyThrowSpec volatile      >::value);
        ASSERT(1 == bslmf::IsPolymorphic<PolyThrowSpec const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<
                                        PolyThrowSpec&               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                        PolyThrowSpec const&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                        PolyThrowSpec volatile&      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                        PolyThrowSpec const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<
                                        PolyThrowSpec *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                        PolyThrowSpec *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                        PolyThrowSpec *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                                        PolyThrowSpec *const volatile>::value);

        ASSERT(1 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2               >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 const         >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 volatile      >::value);
        ASSERT(1 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2&               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 const&         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 volatile&      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 const volatile&>::value);

        ASSERT(0 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 *              >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 *const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 *volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<
                           DerivedFromPolyThrowSpec2 *const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<int Struct::*               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<int Struct::* const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<int Struct::* volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<int Struct::* const volatile>::value);

        ASSERT(0 == bslmf::IsPolymorphic<int Poly::*               >::value);
        ASSERT(0 == bslmf::IsPolymorphic<int Poly::* const         >::value);
        ASSERT(0 == bslmf::IsPolymorphic<int Poly::* volatile      >::value);
        ASSERT(0 == bslmf::IsPolymorphic<int Poly::* const volatile>::value);
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
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
