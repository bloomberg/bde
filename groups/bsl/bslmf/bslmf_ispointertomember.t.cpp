// bslmf_ispointertomember.t.cpp                                      -*-C++-*-

#include <bslmf_ispointertomember.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>      // print, fprintf
#include <stdlib.h>     // atoi

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//                                  TBD
//-----------------------------------------------------------------------------
// [ 1] bslmf::IsPointerToMember
// [ 2] USAGE EXAMPLE

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

typedef char (Base::* const volatile PMF08) (
                                 int*, const char *, float,
                                 double, Union&, const int&, long, float*[54]);
typedef char (Base::* const volatile PMF08c) (
                           int*, const char *, float,
                           double, Union&, const int&, long, float*[54]) const;
typedef char (Base::* const volatile PMF08v) (int*, const char *, float,
                        double, Union&, const int&, long, float*[54]) volatile;
typedef char (Base::* const volatile PMF08cv) (int*, const char *, float,
                  double, Union&, const int&, long, float*[54]) const volatile;

typedef int (DerivedPoly::* volatile& PMF09)(
                           int*, const char *, float,
                           double, Union&, const int&, long, float*[54], Enum);
typedef int (DerivedPoly::* volatile& PMF09c)(
                     int*, const char *, float,
                     double, Union&, const int&, long, float*[54], Enum) const;
typedef int (DerivedPoly::* volatile& PMF09v)(int*, const char *, float,
                  double, Union&, const int&, long, float*[54], Enum) volatile;
typedef int (DerivedPoly::* volatile& PMF09cv)(int*, const char *, float,
            double, Union&, const int&, long, float*[54], Enum) const volatile;

typedef int (DerivedPoly::* const volatile& PMF10)(
                     int*, const char *, float,
                     double, Union&, const int&, long, float*[54], Enum, char);
typedef int (DerivedPoly::* const volatile& PMF10c)(
               int*, const char *, float,
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
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);       // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating use of 'bslmf::IsPointerToMember'.
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

        if (verbose) printf("\nUSAGE EXAMPLE\n"
                            "\n=============\n");

// continued from above ...
//..
     ASSERT(0 == bslmf::IsPointerToMember<int             *>::value);
     ASSERT(0 == bslmf::IsPointerToMember<    MyStruct    *>::value);
     ASSERT(1 == bslmf::IsPointerToMember<int MyStruct::*  >::value);
     ASSERT(0 == bslmf::IsPointerToMember<int MyStruct::*& >::value);
     ASSERT(0 == bslmf::IsPointerToMember<int MyStruct::* *>::value);

     ASSERT(1 == bslmf::IsPointerToMemberData<int MyStruct::*>::value);
     ASSERT(0 == bslmf::IsPointerToMemberData<PMFdRi         >::value);

     ASSERT(1 == bslmf::IsPointerToMember<PMFdRi >::value);
     ASSERT(0 == bslmf::IsPointerToMember<PMFdRi&>::value);
     ASSERT(1 == bslmf::IsPointerToMember<PMFCe  >::value);
     ASSERT(0 == bslmf::IsPointerToMember<PMFCe& >::value);

     ASSERT(1 == bslmf::IsPointerToMemberFunction<PMFdRi        >::value);
     ASSERT(0 == bslmf::IsPointerToMemberFunction<int MyClass::*>::value);
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf::IsPointerToMember' with various types and
        //   verify that their 'VALUE' member is initialized properly.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'bslmf::IsPointerToMember'\n"
                            "\n==================================\n");

        ASSERT(0 == bslmf::IsPointerToMember<int               >::value);
        ASSERT(0 == bslmf::IsPointerToMember<int const         >::value);
        ASSERT(0 == bslmf::IsPointerToMember<int volatile      >::value);
        ASSERT(0 == bslmf::IsPointerToMember<int const volatile>::value);

        ASSERT(0 == bslmf::IsPointerToMember<int&               >::value);
        ASSERT(0 == bslmf::IsPointerToMember<int const&         >::value);
        ASSERT(0 == bslmf::IsPointerToMember<int volatile&      >::value);
        ASSERT(0 == bslmf::IsPointerToMember<int const volatile&>::value);

        ASSERT(0 == bslmf::IsPointerToMember<Enum               >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Enum const         >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Enum volatile      >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Enum const volatile>::value);

        ASSERT(0 ==
          bslmf::IsPointerToMember<int *                             >::value);
        ASSERT(0 ==
          bslmf::IsPointerToMember<int *const                        >::value);
        ASSERT(0 ==
          bslmf::IsPointerToMember<int *volatile                     >::value);
        ASSERT(0 ==
          bslmf::IsPointerToMember<int *const volatile               >::value);
        ASSERT(0 ==
          bslmf::IsPointerToMember<int const *                       >::value);
        ASSERT(0 ==
          bslmf::IsPointerToMember<int const *const                  >::value);
        ASSERT(0 ==
          bslmf::IsPointerToMember<int const *volatile               >::value);
        ASSERT(0 ==
          bslmf::IsPointerToMember<int const *const volatile         >::value);
        ASSERT(0 ==
          bslmf::IsPointerToMember<int volatile *                    >::value);
        ASSERT(0 ==
          bslmf::IsPointerToMember<int volatile *const               >::value);
        ASSERT(0 ==
          bslmf::IsPointerToMember<int volatile *volatile            >::value);
        ASSERT(0 ==
          bslmf::IsPointerToMember<int volatile *const volatile      >::value);
        ASSERT(0 ==
          bslmf::IsPointerToMember<int const volatile *              >::value);
        ASSERT(0 ==
          bslmf::IsPointerToMember<int const volatile *const         >::value);
        ASSERT(0 ==
          bslmf::IsPointerToMember<int const volatile *volatile      >::value);
        ASSERT(0 ==
          bslmf::IsPointerToMember<int const volatile *const volatile>::value);

        ASSERT(0 == bslmf::IsPointerToMember<Struct *              >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Struct *const         >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Struct *volatile      >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Struct *const volatile>::value);

        ASSERT(0 == bslmf::IsPointerToMember<Struct               >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Struct const         >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Struct volatile      >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Struct const volatile>::value);

        ASSERT(0 == bslmf::IsPointerToMember<Struct&               >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Struct const&         >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Struct volatile&      >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Struct const volatile&>::value);

        ASSERT(0 == bslmf::IsPointerToMember<Union               >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Union const         >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Union volatile      >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Union const volatile>::value);

        ASSERT(0 == bslmf::IsPointerToMember<Union&               >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Union const&         >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Union volatile&      >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Union const volatile&>::value);

        ASSERT(0 == bslmf::IsPointerToMember<Base               >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Base const         >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Base volatile      >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Base const volatile>::value);

        ASSERT(0 == bslmf::IsPointerToMember<Base&               >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Base const&         >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Base volatile&      >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Base const volatile&>::value);

        ASSERT(0 == bslmf::IsPointerToMember<Derived               >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Derived const         >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Derived volatile      >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Derived const volatile>::value);

        ASSERT(0 == bslmf::IsPointerToMember<Derived&               >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Derived const&         >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Derived volatile&      >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Derived const volatile&>::value);

        ASSERT(0 == bslmf::IsPointerToMember<Poly               >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Poly const         >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Poly volatile      >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Poly const volatile>::value);

        ASSERT(0 == bslmf::IsPointerToMember<Poly&               >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Poly const&         >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Poly volatile&      >::value);
        ASSERT(0 == bslmf::IsPointerToMember<Poly const volatile&>::value);

        ASSERT(0 ==
                  bslmf::IsPointerToMember<DerivedPoly               >::value);
        ASSERT(0 ==
                  bslmf::IsPointerToMember<DerivedPoly const         >::value);
        ASSERT(0 ==
                  bslmf::IsPointerToMember<DerivedPoly volatile      >::value);
        ASSERT(0 ==
                  bslmf::IsPointerToMember<DerivedPoly const volatile>::value);

        ASSERT(0 ==
                 bslmf::IsPointerToMember<DerivedPoly&               >::value);
        ASSERT(0 ==
                 bslmf::IsPointerToMember<DerivedPoly const&         >::value);
        ASSERT(0 ==
                 bslmf::IsPointerToMember<DerivedPoly volatile&      >::value);
        ASSERT(0 ==
                 bslmf::IsPointerToMember<DerivedPoly const volatile&>::value);

        ASSERT(1 ==
                bslmf::IsPointerToMember<int Struct::*               >::value);
        ASSERT(1 ==
                bslmf::IsPointerToMember<int Struct::* const         >::value);
        ASSERT(1 ==
                bslmf::IsPointerToMember<int Struct::* volatile      >::value);
        ASSERT(1 ==
                bslmf::IsPointerToMember<int Struct::* const volatile>::value);

        ASSERT(0 ==
              bslmf::IsPointerToMember<int *Struct::*&               >::value);
        ASSERT(0 ==
              bslmf::IsPointerToMember<int *Struct::* const&         >::value);
        ASSERT(0 ==
              bslmf::IsPointerToMember<int *Struct::* volatile&      >::value);
        ASSERT(0 ==
              bslmf::IsPointerToMember<int *Struct::* const volatile&>::value);

        ASSERT(1 ==
              bslmf::IsPointerToMember<Struct Union::*               >::value);
        ASSERT(1 ==
              bslmf::IsPointerToMember<Struct Union::* const         >::value);
        ASSERT(1 ==
              bslmf::IsPointerToMember<Struct Union::* volatile      >::value);
        ASSERT(1 ==
              bslmf::IsPointerToMember<Struct Union::* const volatile>::value);

        ASSERT(0 ==
            bslmf::IsPointerToMember<Struct *Union::*&               >::value);
        ASSERT(0 ==
            bslmf::IsPointerToMember<Struct *Union::* const&         >::value);
        ASSERT(0 ==
            bslmf::IsPointerToMember<Struct *Union::* volatile&      >::value);
        ASSERT(0 ==
            bslmf::IsPointerToMember<Struct *Union::* const volatile&>::value);

        ASSERT(1 ==
                 bslmf::IsPointerToMember<char Base::*               >::value);
        ASSERT(1 ==
                 bslmf::IsPointerToMember<char Base::* const         >::value);
        ASSERT(1 ==
                 bslmf::IsPointerToMember<char Base::* volatile      >::value);
        ASSERT(1 ==
                 bslmf::IsPointerToMember<char Base::* const volatile>::value);

        ASSERT(0 ==
                bslmf::IsPointerToMember<Enum Base::*&               >::value);
        ASSERT(0 ==
                bslmf::IsPointerToMember<Enum Base::* const&         >::value);
        ASSERT(0 ==
                bslmf::IsPointerToMember<Enum Base::* volatile&      >::value);
        ASSERT(0 ==
                bslmf::IsPointerToMember<Enum Base::* const volatile&>::value);

        ASSERT(1 ==
             bslmf::IsPointerToMember<Enum *Derived::*               >::value);
        ASSERT(1 ==
             bslmf::IsPointerToMember<Enum *Derived::* const         >::value);
        ASSERT(1 ==
             bslmf::IsPointerToMember<Enum *Derived::* volatile      >::value);
        ASSERT(1 ==
             bslmf::IsPointerToMember<Enum *Derived::* const volatile>::value);

        ASSERT(0 ==
              bslmf::IsPointerToMember<int Derived::*&               >::value);
        ASSERT(0 ==
              bslmf::IsPointerToMember<int Derived::* const&         >::value);
        ASSERT(0 ==
              bslmf::IsPointerToMember<int Derived::* volatile&      >::value);
        ASSERT(0 ==
              bslmf::IsPointerToMember<int Derived::* const volatile&>::value);

        ASSERT(1 ==
                  bslmf::IsPointerToMember<int Poly::*               >::value);
        ASSERT(1 ==
                  bslmf::IsPointerToMember<int Poly::* const         >::value);
        ASSERT(1 ==
                  bslmf::IsPointerToMember<int Poly::* volatile      >::value);
        ASSERT(1 ==
                  bslmf::IsPointerToMember<int Poly::* const volatile>::value);

        ASSERT(0 ==
                 bslmf::IsPointerToMember<int Poly::*&               >::value);
        ASSERT(0 ==
                 bslmf::IsPointerToMember<int Poly::* const&         >::value);
        ASSERT(0 ==
                 bslmf::IsPointerToMember<int Poly::* volatile&      >::value);
        ASSERT(0 ==
                 bslmf::IsPointerToMember<int Poly::* const volatile&>::value);

        ASSERT(1 ==
           bslmf::IsPointerToMember<int DerivedPoly::*               >::value);
        ASSERT(1 ==
           bslmf::IsPointerToMember<int DerivedPoly::* const         >::value);
        ASSERT(1 ==
           bslmf::IsPointerToMember<int DerivedPoly::* volatile      >::value);
        ASSERT(1 ==
           bslmf::IsPointerToMember<int DerivedPoly::* const volatile>::value);

        ASSERT(0 ==
          bslmf::IsPointerToMember<int DerivedPoly::*&               >::value);
        ASSERT(0 ==
          bslmf::IsPointerToMember<int DerivedPoly::* const&         >::value);
        ASSERT(0 ==
          bslmf::IsPointerToMember<int DerivedPoly::* volatile&      >::value);
        ASSERT(0 ==
          bslmf::IsPointerToMember<int DerivedPoly::* const volatile&>::value);

        ASSERT(1 == bslmf::IsPointerToMember<PMF00  >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF00c >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF00v >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF00cv>::value);

        ASSERT(1 == bslmf::IsPointerToMember<PMF01  >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF01c >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF01v >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF01cv>::value);

        ASSERT(1 == bslmf::IsPointerToMember<PMF02  >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF02c >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF02v >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF02cv>::value);

        ASSERT(1 == bslmf::IsPointerToMember<PMF03  >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF03c >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF03v >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF03cv>::value);

        ASSERT(1 == bslmf::IsPointerToMember<PMF04  >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF04c >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF04v >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF04cv>::value);

        ASSERT(1 == bslmf::IsPointerToMember<PMF05  >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF05c >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF05v >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF05cv>::value);

        ASSERT(1 == bslmf::IsPointerToMember<PMF06  >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF06c >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF06v >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF06cv>::value);

        ASSERT(1 == bslmf::IsPointerToMember<PMF07  >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF07c >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF07v >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF07cv>::value);

        ASSERT(1 == bslmf::IsPointerToMember<PMF08  >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF08c >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF08v >::value);
        ASSERT(1 == bslmf::IsPointerToMember<PMF08cv>::value);

        ASSERT(0 == bslmf::IsPointerToMember<PMF09  >::value);
        ASSERT(0 == bslmf::IsPointerToMember<PMF09c >::value);
        ASSERT(0 == bslmf::IsPointerToMember<PMF09v >::value);
        ASSERT(0 == bslmf::IsPointerToMember<PMF09cv>::value);

        ASSERT(0 == bslmf::IsPointerToMember<PMF10  >::value);
        ASSERT(0 == bslmf::IsPointerToMember<PMF10c >::value);
        ASSERT(0 == bslmf::IsPointerToMember<PMF10v >::value);
        ASSERT(0 == bslmf::IsPointerToMember<PMF10cv>::value);
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
