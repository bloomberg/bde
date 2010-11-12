// bslmf_ispointertomember.t.cpp            -*-C++-*-

#include <bslmf_ispointertomember.h>

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
// [ 1] bslmf_IsPointerToMember
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

class Poly {
    virtual ~Poly();
};
class DerivedPoly : public Poly {
    ~DerivedPoly();
};

// Note: Sun's CC compiler necessitates these typedef's.

typedef int (Struct::* PMF00)();
typedef int (Struct::* PMF00c)() const;
typedef int (Struct::* PMF00v)() volatile;
typedef int (Struct::* PMF00cv)() const volatile;

typedef int (Struct::* const PMF01)(int);
typedef int (Struct::* const PMF01c)(int) const;
typedef int (Struct::* const PMF01v)(int) volatile;
typedef int (Struct::* const PMF01cv)(int) const volatile;

typedef int (Struct::* volatile PMF02)(short, int&);
typedef int (Struct::* volatile PMF02c)(short, int&) const;
typedef int (Struct::* volatile PMF02v)(short, int&) volatile;
typedef int (Struct::* volatile PMF02cv)(short, int&) const volatile;

typedef int (Struct::* volatile PMF03)(void *[4][4], short, int&);
typedef int (Struct::* volatile PMF03c)(void *[4][4], short, int&) const;
typedef int (Struct::* volatile PMF03v)(void *[4][4], short, int&) volatile;
typedef int (Struct::* volatile PMF03cv)(void *[4][4], short, int&)
                                                                const volatile;

typedef int (Struct::* const volatile PMF04)
                                           (int*, const char *, float, double);
typedef int (Struct::* const volatile PMF04c)
                                     (int*, const char *, float, double) const;
typedef int (Struct::* const volatile PMF04v)
                                  (int*, const char *, float, double) volatile;
typedef int (Struct::* const volatile PMF04cv)
                            (int*, const char *, float, double) const volatile;

typedef char (Base::* PMF05)(int*, const char *, float, double, Union&);
typedef char (Base::* PMF05c)(int*, const char *, float, double, Union&) const;
typedef char (Base::* PMF05v)(int*, const char *, float, double, Union&)
                                                                      volatile;
typedef char (Base::* PMF05cv)(int*, const char *, float, double, Union&)
                                                                const volatile;

typedef char (Base::* const PMF06)
                       (int*, const char *, float, double, Union&, const int&);
typedef char (Base::* const PMF06c)
                 (int*, const char *, float, double, Union&, const int&) const;
typedef char (Base::* const PMF06v)
              (int*, const char *, float, double, Union&, const int&) volatile;
typedef char (Base::* const PMF06cv)
        (int*, const char *, float, double, Union&, const int&) const volatile;

typedef char (Base::* volatile PMF07)
                 (int*, const char *, float, double, Union&, const int&, long);
typedef char (Base::* volatile PMF07c)
           (int*, const char *, float, double, Union&, const int&, long) const;
typedef char (Base::* volatile PMF07v)
        (int*, const char *, float, double, Union&, const int&, long) volatile;
typedef char (Base::* volatile PMF07cv)
  (int*, const char *, float, double, Union&, const int&, long) const volatile;

typedef char (Base::* const volatile PMF08) (int*, const char *, float,
                                 double, Union&, const int&, long, float*[54]);
typedef char (Base::* const volatile PMF08c) (int*, const char *, float,
                           double, Union&, const int&, long, float*[54]) const;
typedef char (Base::* const volatile PMF08v) (int*, const char *, float,
                        double, Union&, const int&, long, float*[54]) volatile;
typedef char (Base::* const volatile PMF08cv) (int*, const char *, float,
                  double, Union&, const int&, long, float*[54]) const volatile;

typedef int (DerivedPoly::* volatile& PMF09)(int*, const char *, float,
                           double, Union&, const int&, long, float*[54], Enum);
typedef int (DerivedPoly::* volatile& PMF09c)(int*, const char *, float,
                     double, Union&, const int&, long, float*[54], Enum) const;
typedef int (DerivedPoly::* volatile& PMF09v)(int*, const char *, float,
                  double, Union&, const int&, long, float*[54], Enum) volatile;
typedef int (DerivedPoly::* volatile& PMF09cv)(int*, const char *, float,
            double, Union&, const int&, long, float*[54], Enum) const volatile;

typedef int (DerivedPoly::* const volatile& PMF10)(int*, const char *, float,
                     double, Union&, const int&, long, float*[54], Enum, char);
typedef int (DerivedPoly::* const volatile& PMF10c)(int*, const char *, float,
               double, Union&, const int&, long, float*[54], Enum, char) const;
typedef int (DerivedPoly::* const volatile& PMF10v)(int*, const char *, float,
            double, Union&, const int&, long, float*[54], Enum, char) volatile;
typedef int (DerivedPoly::* const volatile& PMF10cv)(int*, const char *, float,
      double, Union&, const int&, long, float*[54], Enum, char) const volatile;

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
//
     typedef int (MyClass::* PMFdRi)(double, int&);
     typedef int (MyClass::* PMFCe)(MyEnum) const;
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
        //   Simple example illustrating use of 'bslmf_IsPointerToMember'.
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

// continued from above ...
//..
     ASSERT(0 == bslmf_IsPointerToMember<int             *>::VALUE);
     ASSERT(0 == bslmf_IsPointerToMember<    MyStruct    *>::VALUE);
     ASSERT(1 == bslmf_IsPointerToMember<int MyStruct::*  >::VALUE);
     ASSERT(0 == bslmf_IsPointerToMember<int MyStruct::*& >::VALUE);
     ASSERT(0 == bslmf_IsPointerToMember<int MyStruct::* *>::VALUE);

     ASSERT(1 == bslmf_IsPointerToMemberData<int MyStruct::*>::VALUE);
     ASSERT(0 == bslmf_IsPointerToMemberData<PMFdRi         >::VALUE);

     ASSERT(1 == bslmf_IsPointerToMember<PMFdRi >::VALUE);
     ASSERT(0 == bslmf_IsPointerToMember<PMFdRi&>::VALUE);
     ASSERT(1 == bslmf_IsPointerToMember<PMFCe  >::VALUE);
     ASSERT(0 == bslmf_IsPointerToMember<PMFCe& >::VALUE);

     ASSERT(1 == bslmf_IsPointerToMemberFunction<PMFdRi        >::VALUE);
     ASSERT(0 == bslmf_IsPointerToMemberFunction<int MyClass::*>::VALUE);
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf_IsPointerToMember' with various types and
        //   verify that their 'VALUE' member is initialized properly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bslmf_IsPointerToMember" << endl
                          << "=======================" << endl;

        ASSERT(0 == bslmf_IsPointerToMember<int               >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<int const         >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<int volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<int const volatile>::VALUE);

        ASSERT(0 == bslmf_IsPointerToMember<int&               >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<int const&         >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<int volatile&      >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<int const volatile&>::VALUE);

        ASSERT(0 == bslmf_IsPointerToMember<Enum               >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Enum const         >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Enum volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Enum const volatile>::VALUE);

        ASSERT(0 ==
           bslmf_IsPointerToMember<int *                             >::VALUE);
        ASSERT(0 ==
           bslmf_IsPointerToMember<int *const                        >::VALUE);
        ASSERT(0 ==
           bslmf_IsPointerToMember<int *volatile                     >::VALUE);
        ASSERT(0 ==
           bslmf_IsPointerToMember<int *const volatile               >::VALUE);
        ASSERT(0 ==
           bslmf_IsPointerToMember<int const *                       >::VALUE);
        ASSERT(0 ==
           bslmf_IsPointerToMember<int const *const                  >::VALUE);
        ASSERT(0 ==
           bslmf_IsPointerToMember<int const *volatile               >::VALUE);
        ASSERT(0 ==
           bslmf_IsPointerToMember<int const *const volatile         >::VALUE);
        ASSERT(0 ==
           bslmf_IsPointerToMember<int volatile *                    >::VALUE);
        ASSERT(0 ==
           bslmf_IsPointerToMember<int volatile *const               >::VALUE);
        ASSERT(0 ==
           bslmf_IsPointerToMember<int volatile *volatile            >::VALUE);
        ASSERT(0 ==
           bslmf_IsPointerToMember<int volatile *const volatile      >::VALUE);
        ASSERT(0 ==
           bslmf_IsPointerToMember<int const volatile *              >::VALUE);
        ASSERT(0 ==
           bslmf_IsPointerToMember<int const volatile *const         >::VALUE);
        ASSERT(0 ==
           bslmf_IsPointerToMember<int const volatile *volatile      >::VALUE);
        ASSERT(0 ==
           bslmf_IsPointerToMember<int const volatile *const volatile>::VALUE);

        ASSERT(0 == bslmf_IsPointerToMember<Struct *              >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Struct *const         >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Struct *volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Struct *const volatile>::VALUE);

        ASSERT(0 == bslmf_IsPointerToMember<Struct               >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Struct const         >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Struct volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Struct const volatile>::VALUE);

        ASSERT(0 == bslmf_IsPointerToMember<Struct&               >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Struct const&         >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Struct volatile&      >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Struct const volatile&>::VALUE);

        ASSERT(0 == bslmf_IsPointerToMember<Union               >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Union const         >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Union volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Union const volatile>::VALUE);

        ASSERT(0 == bslmf_IsPointerToMember<Union&               >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Union const&         >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Union volatile&      >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Union const volatile&>::VALUE);

        ASSERT(0 == bslmf_IsPointerToMember<Base               >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Base const         >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Base volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Base const volatile>::VALUE);

        ASSERT(0 == bslmf_IsPointerToMember<Base&               >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Base const&         >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Base volatile&      >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Base const volatile&>::VALUE);

        ASSERT(0 == bslmf_IsPointerToMember<Derived               >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Derived const         >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Derived volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Derived const volatile>::VALUE);

        ASSERT(0 == bslmf_IsPointerToMember<Derived&               >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Derived const&         >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Derived volatile&      >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Derived const volatile&>::VALUE);

        ASSERT(0 == bslmf_IsPointerToMember<Poly               >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Poly const         >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Poly volatile      >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Poly const volatile>::VALUE);

        ASSERT(0 == bslmf_IsPointerToMember<Poly&               >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Poly const&         >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Poly volatile&      >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<Poly const volatile&>::VALUE);

        ASSERT(0 ==
                   bslmf_IsPointerToMember<DerivedPoly               >::VALUE);
        ASSERT(0 ==
                   bslmf_IsPointerToMember<DerivedPoly const         >::VALUE);
        ASSERT(0 ==
                   bslmf_IsPointerToMember<DerivedPoly volatile      >::VALUE);
        ASSERT(0 ==
                   bslmf_IsPointerToMember<DerivedPoly const volatile>::VALUE);

        ASSERT(0 ==
                  bslmf_IsPointerToMember<DerivedPoly&               >::VALUE);
        ASSERT(0 ==
                  bslmf_IsPointerToMember<DerivedPoly const&         >::VALUE);
        ASSERT(0 ==
                  bslmf_IsPointerToMember<DerivedPoly volatile&      >::VALUE);
        ASSERT(0 ==
                  bslmf_IsPointerToMember<DerivedPoly const volatile&>::VALUE);

        ASSERT(1 ==
                 bslmf_IsPointerToMember<int Struct::*               >::VALUE);
        ASSERT(1 ==
                 bslmf_IsPointerToMember<int Struct::* const         >::VALUE);
        ASSERT(1 ==
                 bslmf_IsPointerToMember<int Struct::* volatile      >::VALUE);
        ASSERT(1 ==
                 bslmf_IsPointerToMember<int Struct::* const volatile>::VALUE);

        ASSERT(0 ==
               bslmf_IsPointerToMember<int *Struct::*&               >::VALUE);
        ASSERT(0 ==
               bslmf_IsPointerToMember<int *Struct::* const&         >::VALUE);
        ASSERT(0 ==
               bslmf_IsPointerToMember<int *Struct::* volatile&      >::VALUE);
        ASSERT(0 ==
               bslmf_IsPointerToMember<int *Struct::* const volatile&>::VALUE);

        ASSERT(1 ==
               bslmf_IsPointerToMember<Struct Union::*               >::VALUE);
        ASSERT(1 ==
               bslmf_IsPointerToMember<Struct Union::* const         >::VALUE);
        ASSERT(1 ==
               bslmf_IsPointerToMember<Struct Union::* volatile      >::VALUE);
        ASSERT(1 ==
               bslmf_IsPointerToMember<Struct Union::* const volatile>::VALUE);

        ASSERT(0 ==
             bslmf_IsPointerToMember<Struct *Union::*&               >::VALUE);
        ASSERT(0 ==
             bslmf_IsPointerToMember<Struct *Union::* const&         >::VALUE);
        ASSERT(0 ==
             bslmf_IsPointerToMember<Struct *Union::* volatile&      >::VALUE);
        ASSERT(0 ==
             bslmf_IsPointerToMember<Struct *Union::* const volatile&>::VALUE);

        ASSERT(1 ==
                  bslmf_IsPointerToMember<char Base::*               >::VALUE);
        ASSERT(1 ==
                  bslmf_IsPointerToMember<char Base::* const         >::VALUE);
        ASSERT(1 ==
                  bslmf_IsPointerToMember<char Base::* volatile      >::VALUE);
        ASSERT(1 ==
                  bslmf_IsPointerToMember<char Base::* const volatile>::VALUE);

        ASSERT(0 ==
                 bslmf_IsPointerToMember<Enum Base::*&               >::VALUE);
        ASSERT(0 ==
                 bslmf_IsPointerToMember<Enum Base::* const&         >::VALUE);
        ASSERT(0 ==
                 bslmf_IsPointerToMember<Enum Base::* volatile&      >::VALUE);
        ASSERT(0 ==
                 bslmf_IsPointerToMember<Enum Base::* const volatile&>::VALUE);

        ASSERT(1 ==
              bslmf_IsPointerToMember<Enum *Derived::*               >::VALUE);
        ASSERT(1 ==
              bslmf_IsPointerToMember<Enum *Derived::* const         >::VALUE);
        ASSERT(1 ==
              bslmf_IsPointerToMember<Enum *Derived::* volatile      >::VALUE);
        ASSERT(1 ==
              bslmf_IsPointerToMember<Enum *Derived::* const volatile>::VALUE);

        ASSERT(0 ==
               bslmf_IsPointerToMember<int Derived::*&               >::VALUE);
        ASSERT(0 ==
               bslmf_IsPointerToMember<int Derived::* const&         >::VALUE);
        ASSERT(0 ==
               bslmf_IsPointerToMember<int Derived::* volatile&      >::VALUE);
        ASSERT(0 ==
               bslmf_IsPointerToMember<int Derived::* const volatile&>::VALUE);

        ASSERT(1 ==
                   bslmf_IsPointerToMember<int Poly::*               >::VALUE);
        ASSERT(1 ==
                   bslmf_IsPointerToMember<int Poly::* const         >::VALUE);
        ASSERT(1 ==
                   bslmf_IsPointerToMember<int Poly::* volatile      >::VALUE);
        ASSERT(1 ==
                   bslmf_IsPointerToMember<int Poly::* const volatile>::VALUE);

        ASSERT(0 ==
                  bslmf_IsPointerToMember<int Poly::*&               >::VALUE);
        ASSERT(0 ==
                  bslmf_IsPointerToMember<int Poly::* const&         >::VALUE);
        ASSERT(0 ==
                  bslmf_IsPointerToMember<int Poly::* volatile&      >::VALUE);
        ASSERT(0 ==
                  bslmf_IsPointerToMember<int Poly::* const volatile&>::VALUE);

        ASSERT(1 ==
            bslmf_IsPointerToMember<int DerivedPoly::*               >::VALUE);
        ASSERT(1 ==
            bslmf_IsPointerToMember<int DerivedPoly::* const         >::VALUE);
        ASSERT(1 ==
            bslmf_IsPointerToMember<int DerivedPoly::* volatile      >::VALUE);
        ASSERT(1 ==
            bslmf_IsPointerToMember<int DerivedPoly::* const volatile>::VALUE);

        ASSERT(0 ==
           bslmf_IsPointerToMember<int DerivedPoly::*&               >::VALUE);
        ASSERT(0 ==
           bslmf_IsPointerToMember<int DerivedPoly::* const&         >::VALUE);
        ASSERT(0 ==
           bslmf_IsPointerToMember<int DerivedPoly::* volatile&      >::VALUE);
        ASSERT(0 ==
           bslmf_IsPointerToMember<int DerivedPoly::* const volatile&>::VALUE);

        ASSERT(1 == bslmf_IsPointerToMember<PMF00  >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF00c >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF00v >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF00cv>::VALUE);

        ASSERT(1 == bslmf_IsPointerToMember<PMF01  >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF01c >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF01v >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF01cv>::VALUE);

        ASSERT(1 == bslmf_IsPointerToMember<PMF02  >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF02c >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF02v >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF02cv>::VALUE);

        ASSERT(1 == bslmf_IsPointerToMember<PMF03  >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF03c >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF03v >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF03cv>::VALUE);

        ASSERT(1 == bslmf_IsPointerToMember<PMF04  >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF04c >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF04v >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF04cv>::VALUE);

        ASSERT(1 == bslmf_IsPointerToMember<PMF05  >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF05c >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF05v >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF05cv>::VALUE);

        ASSERT(1 == bslmf_IsPointerToMember<PMF06  >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF06c >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF06v >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF06cv>::VALUE);

        ASSERT(1 == bslmf_IsPointerToMember<PMF07  >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF07c >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF07v >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF07cv>::VALUE);

        ASSERT(1 == bslmf_IsPointerToMember<PMF08  >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF08c >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF08v >::VALUE);
        ASSERT(1 == bslmf_IsPointerToMember<PMF08cv>::VALUE);

        ASSERT(0 == bslmf_IsPointerToMember<PMF09  >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<PMF09c >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<PMF09v >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<PMF09cv>::VALUE);

        ASSERT(0 == bslmf_IsPointerToMember<PMF10  >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<PMF10c >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<PMF10v >::VALUE);
        ASSERT(0 == bslmf_IsPointerToMember<PMF10cv>::VALUE);
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
