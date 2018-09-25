// bslmf_issame.t.cpp                                                 -*-C++-*-
#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines two meta-functions, 'bsl::is_same' and
// 'bslmf::IsSame', that determine whether two template parameter types are
// same.  Thus, we need to ensure that the values returned by these
// meta-functions are correct for each possible pair of types.  Since the two
// meta-functions are functionally equivalent, we will use the same set of
// types for both.
//
//-----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 2] BloombergLP::bslmf::IsSame::VALUE
// [ 1] bsl::is_same::value
//
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE

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
}  // close namespace NS
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
      case 3: {
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

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Determine Same Types
///- - - - - - - - - - - - - - - -
// Suppose that we have several pairs of types and want to assert whether the
// types in each pair are the same.
//
// First, we define several 'typedef's:
//..
    typedef       int    INT;
    typedef       double DOUBLE;
    typedef       short  SHORT;
    typedef const short  CONST_SHORT;
    typedef       int    INT;
    typedef       int&   INT_REF;
//..
// Now, we instantiate the 'bsl::is_same' template for certain pairs of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
    ASSERT(true  == (bsl::is_same<INT, INT>::value));
    ASSERT(false == (bsl::is_same<INT, DOUBLE>::value));
//..
// Note that a 'const'-qualified type is considered distinct from the
// non-'const' (but otherwise identical) type:
//..
    ASSERT(false == (bsl::is_same<SHORT, CONST_SHORT>::value));
//..
// Similarly, a 'TYPE' and a reference to 'TYPE' ('TYPE&') are distinct:
//..
    ASSERT(false == (bsl::is_same<INT, INT_REF>::value));
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'bslmf::IsSame::VALUE'
        //   Ensure that the static data member 'VALUE' of 'bslmf::IsSame'
        //   instantiations having various (template parameter) 'TYPE1' and
        //   'TYPE2' has the correct value.
        //
        // Concerns:
        //: 1 'IsSame::VALUE' is 1 when the two template parameter types are
        //:   the same.
        //:
        //: 2 'IsSame::VALUE' is 0 when the two template parameter types are
        //:   not the same.
        //
        // Plan:
        //   Instantiate 'bslmf::IsSame' with various combinations of types and
        //   verify that the 'VALUE' member is initialized properly.
        //
        // Testing:
        //   bslmf::IsSame::VALUE
        // --------------------------------------------------------------------

        if (verbose) printf("bslmf::IsSame::VALUE\n"
                            "====================\n");

        ASSERT(1 == (bslmf::IsSame<int, int>::VALUE));
        ASSERT(1 == (bslmf::IsSame<short, short>::VALUE));
        ASSERT(0 == (bslmf::IsSame<int, short>::VALUE));
        ASSERT(0 == (bslmf::IsSame<short, int>::VALUE));
        ASSERT(0 == (bslmf::IsSame<int, unsigned>::VALUE));
        ASSERT(0 == (bslmf::IsSame<int&, int>::VALUE));
        ASSERT(0 == (bslmf::IsSame<float, double>::VALUE));
        ASSERT(0 == (bslmf::IsSame<float, float&>::VALUE));

        ASSERT(1 == (bslmf::IsSame<int const, int const>::VALUE));
        ASSERT(1 == (bslmf::IsSame<const int, int const>::VALUE));
        ASSERT(0 == (bslmf::IsSame<int, int const>::VALUE));
        ASSERT(0 == (bslmf::IsSame<int volatile, int>::VALUE));
        ASSERT(0 == (bslmf::IsSame<int const volatile, int const>::VALUE));

        ASSERT(1 == (bslmf::IsSame<Enum1, Enum1>::VALUE));
        ASSERT(1 == (bslmf::IsSame<Enum3, Enum3>::VALUE));
        ASSERT(0 == (bslmf::IsSame<Enum1, Enum2>::VALUE));
        ASSERT(0 == (bslmf::IsSame<Enum3, Enum1>::VALUE));
        ASSERT(0 == (bslmf::IsSame<int, Enum1>::VALUE));
        ASSERT(0 == (bslmf::IsSame<Enum1, unsigned>::VALUE));
        ASSERT(0 == (bslmf::IsSame<long, Enum1>::VALUE));
        ASSERT(0 == (bslmf::IsSame<Enum1, unsigned long>::VALUE));

        ASSERT(1 == (bslmf::IsSame<char *, char *>::VALUE));
        ASSERT(1 == (bslmf::IsSame<void *, void *>::VALUE));
        ASSERT(1 == (bslmf::IsSame<const char *, const char *>::VALUE));
        ASSERT(0 == (bslmf::IsSame<const char *, char *>::VALUE));
        ASSERT(0 == (bslmf::IsSame<char *, char *const>::VALUE));
        ASSERT(0 == (bslmf::IsSame<char *, void *>::VALUE));
        ASSERT(0 == (bslmf::IsSame<int *, char *>::VALUE));

        ASSERT(1 == (bslmf::IsSame<Struct1, Struct1>::VALUE));
        ASSERT(1 == (bslmf::IsSame<Struct3, Struct3>::VALUE));
        ASSERT(0 == (bslmf::IsSame<Struct1, Struct2>::VALUE));
        ASSERT(0 == (bslmf::IsSame<Struct3, Struct1>::VALUE));

        ASSERT(1 == (bslmf::IsSame<Base, Base>::VALUE));
        ASSERT(1 == (bslmf::IsSame<const Base *, const Base *>::VALUE));
        ASSERT(1 == (bslmf::IsSame<Derived&, Derived&>::VALUE));
        ASSERT(0 == (bslmf::IsSame<Base&, Base>::VALUE));
        ASSERT(0 == (bslmf::IsSame<Base&, Derived&>::VALUE));
        ASSERT(0 == (bslmf::IsSame<Derived *, Base *>::VALUE));
        ASSERT(0 == (bslmf::IsSame<void *, Base *>::VALUE));

        ASSERT(1 == (bslmf::IsSame<int Base::*, int Base::*>::VALUE));
        ASSERT(1 == (bslmf::IsSame<int Struct3::*, int Struct3::*>::VALUE));
        ASSERT(0 == (bslmf::IsSame<int Base::*, int Class::*>::VALUE));
        ASSERT(0 == (bslmf::IsSame<int Base::*, int Derived::*>::VALUE));
        ASSERT(0 == (bslmf::IsSame<int Derived::*, int Base::*>::VALUE));

        ASSERT(1 == (bslmf::IsSame<INT_TYPE, INT_TYPE>::VALUE));
        ASSERT(1 == (bslmf::IsSame<INT_TYPE, Class::INT_TYPE>::VALUE));
        ASSERT(1 == (bslmf::IsSame<NS::INT_TYPE, Class::INT_TYPE>::VALUE));
        ASSERT(1 == (bslmf::IsSame<INT_TYPE, NS::INT_TYPE>::VALUE));

        ASSERT(1 == (bslmf::IsSame<char [1], char [1]>::VALUE));
        ASSERT(1 == (bslmf::IsSame<const int [5], const int [5]>::VALUE));
        ASSERT(0 == (bslmf::IsSame<char, char [1]>::VALUE));
        ASSERT(0 == (bslmf::IsSame<int [5], char [5]>::VALUE));
        ASSERT(0 == (bslmf::IsSame<int [2][4], int [4][2]>::VALUE));

        ASSERT(1 == (bslmf::IsSame<F, F>::VALUE));
        ASSERT(1 == (bslmf::IsSame<Fv, F>::VALUE));
        ASSERT(1 == (bslmf::IsSame<Fi, Fi>::VALUE));
        ASSERT(1 == (bslmf::IsSame<PFi, PFi>::VALUE));
        ASSERT(0 == (bslmf::IsSame<Fe, Fi>::VALUE));
        ASSERT(0 == (bslmf::IsSame<Fe, Fie>::VALUE));
        ASSERT(0 == (bslmf::IsSame<Fie, Fi>::VALUE));
        ASSERT(0 == (bslmf::IsSame<PFi, Fi>::VALUE));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_same::value'
        //   Ensure that the static data member 'value' of 'bsl::is_same'
        //   instantiations having various (template parameter) 'TYPE1' and
        //   'TYPE2' has the correct value.
        //
        // Concerns:
        //: 1 'is_same::value' is 'true' when the two template parameter types
        //:   are the same.
        //:
        //: 2 'is_same::value' is 'false' when the two template parameter types
        //:   are not the same.
        //
        // Plan:
        //   Instantiate 'bsl::is_same' with various combinations of types and
        //   verify that the 'value' member is initialized properly.  (C-1,2)
        //
        // Testing:
        //   bsl::is_same::value
        // --------------------------------------------------------------------

        if (verbose) printf("bsl::is_same::value\n"
                            "===================\n");

        ASSERT(true  == (bsl::is_same<int, int>::value));
        ASSERT(true  == (bsl::is_same<short, short>::value));
        ASSERT(false == (bsl::is_same<int, short>::value));
        ASSERT(false == (bsl::is_same<short, int>::value));
        ASSERT(false == (bsl::is_same<int, unsigned>::value));
        ASSERT(false == (bsl::is_same<int&, int>::value));
        ASSERT(false == (bsl::is_same<float, double>::value));
        ASSERT(false == (bsl::is_same<float, float&>::value));

        ASSERT(true  == (bsl::is_same<int const, int const>::value));
        ASSERT(true  == (bsl::is_same<const int, int const>::value));
        ASSERT(false == (bsl::is_same<int, int const>::value));
        ASSERT(false == (bsl::is_same<int volatile, int>::value));
        ASSERT(false == (bsl::is_same<int const volatile, int const>::value));

        ASSERT(true  == (bsl::is_same<Enum1, Enum1>::value));
        ASSERT(true  == (bsl::is_same<Enum3, Enum3>::value));
        ASSERT(false == (bsl::is_same<Enum1, Enum2>::value));
        ASSERT(false == (bsl::is_same<Enum3, Enum1>::value));
        ASSERT(false == (bsl::is_same<int, Enum1>::value));
        ASSERT(false == (bsl::is_same<Enum1, unsigned>::value));
        ASSERT(false == (bsl::is_same<long, Enum1>::value));
        ASSERT(false == (bsl::is_same<Enum1, unsigned long>::value));

        ASSERT(true  == (bsl::is_same<char *, char *>::value));
        ASSERT(true  == (bsl::is_same<void *, void *>::value));
        ASSERT(true  == (bsl::is_same<const char *, const char *>::value));
        ASSERT(false == (bsl::is_same<const char *, char *>::value));
        ASSERT(false == (bsl::is_same<char *, char *const>::value));
        ASSERT(false == (bsl::is_same<char *, void *>::value));
        ASSERT(false == (bsl::is_same<int *, char *>::value));

        ASSERT(true  == (bsl::is_same<Struct1, Struct1>::value));
        ASSERT(true  == (bsl::is_same<Struct3, Struct3>::value));
        ASSERT(false == (bsl::is_same<Struct1, Struct2>::value));
        ASSERT(false == (bsl::is_same<Struct3, Struct1>::value));

        ASSERT(true  == (bsl::is_same<Base, Base>::value));
        ASSERT(true  == (bsl::is_same<const Base *, const Base *>::value));
        ASSERT(true  == (bsl::is_same<Derived&, Derived&>::value));
        ASSERT(false == (bsl::is_same<Base&, Base>::value));
        ASSERT(false == (bsl::is_same<Base&, Derived&>::value));
        ASSERT(false == (bsl::is_same<Derived *, Base *>::value));
        ASSERT(false == (bsl::is_same<void *, Base *>::value));

        ASSERT(true  == (bsl::is_same<int Base::*, int Base::*>::value));
        ASSERT(true  == (bsl::is_same<int Struct3::*, int Struct3::*>::value));
        ASSERT(false == (bsl::is_same<int Base::*, int Class::*>::value));
        ASSERT(false == (bsl::is_same<int Base::*, int Derived::*>::value));
        ASSERT(false == (bsl::is_same<int Derived::*, int Base::*>::value));

        ASSERT(true  == (bsl::is_same<INT_TYPE, INT_TYPE>::value));
        ASSERT(true  == (bsl::is_same<INT_TYPE, Class::INT_TYPE>::value));
        ASSERT(true  == (bsl::is_same<NS::INT_TYPE, Class::INT_TYPE>::value));
        ASSERT(true  == (bsl::is_same<INT_TYPE, NS::INT_TYPE>::value));

        ASSERT(true  == (bsl::is_same<char [1], char [1]>::value));
        ASSERT(true  == (bsl::is_same<const int [5], const int [5]>::value));
        ASSERT(false == (bsl::is_same<char, char [1]>::value));
        ASSERT(false == (bsl::is_same<int [5], char [5]>::value));
        ASSERT(false == (bsl::is_same<int [2][4], int [4][2]>::value));

        ASSERT(true  == (bsl::is_same<F, F>::value));
        ASSERT(true  == (bsl::is_same<Fv, F>::value));
        ASSERT(true  == (bsl::is_same<Fi, Fi>::value));
        ASSERT(true  == (bsl::is_same<PFi, PFi>::value));
        ASSERT(false == (bsl::is_same<Fe, Fi>::value));
        ASSERT(false == (bsl::is_same<Fe, Fie>::value));
        ASSERT(false == (bsl::is_same<Fie, Fi>::value));
        ASSERT(false == (bsl::is_same<PFi, Fi>::value));
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
