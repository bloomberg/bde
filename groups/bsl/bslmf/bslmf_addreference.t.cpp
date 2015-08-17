// bslmf_addreference.t.cpp                                           -*-C++-*-

#include <bslmf_addreference.h>

#include <bslmf_addrvaluereference.h>
#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>    // atoi()
#include <stdlib.h>    // atoi()

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//                                  TBD
//-----------------------------------------------------------------------------
// [ 1] bslmf::AddReference
// [ 2] USAGE EXAMPLE
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i) {
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
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define ASSERT_SAME(X, Y) ASSERT((bsl::is_same<X, Y>::value))

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum   Enum   {};
struct Struct {};
union  Union  {};
class  Class  {};

typedef int INT;

typedef void      F ();
typedef void ( & RF)();
typedef void (*  PF)();
typedef void (*&RPF)();

typedef void    Fi  (int);
typedef void (&RFi) (int);
typedef void    FRi (int&);
typedef void (&RFRi)(int&);

typedef char    A [5];
typedef char (&RA)[5];

//=============================================================================
//                     GLOBAL TYPES FOR USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// For example:
//..
     struct MyType {};
     typedef MyType& MyTypeRef;

namespace USAGE_EXAMPLE_1 {

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: A Simple Wrapper Class
///- - - - - - - - - - - - - - - - -
// First, let us write a simple class that can wrap any other type:
//..
    template <class TYPE>
    class Wrapper {
      private:
        // DATA
        TYPE d_data;

      public:
        // TYPES
        typedef typename bslmf::AddReference<TYPE>::Type WrappedType;

        // CREATORS
        Wrapper(TYPE value) : d_data(value) {}                      // IMPLICIT
            // Create a 'Wrapper' object having the specified 'value'.

        //! ~Wrapper() = default;
            // Destroy this object.
//..
// Then, we would like to expose access to the wrapped element through a
// method that returns a reference to the data member 'd_data'.  However,
// there would be a problem if the user supplied a parameterized type 'TYPE'
// that is a reference type, as references-to-references were not permitted by
// the language (prior the C++11 standard).  We can resolve such problems
// using the meta-function 'bslmf::AddReference'.
//..
        // MANIPULATORS
        typename bslmf::AddReference<TYPE>::Type value()
        {
            return d_data;
        }
//..
// Next, we supply an accessor function, 'value', that similarly wraps the
// parameterized type 'TYPE' with the 'bslmf::AddReference' meta-function.
// In this case we must remember to const-quality 'TYPE' before passing it
// on to the meta-function.
//..
        // ACCESSORS
        typename bslmf::AddReference<const TYPE>::Type value() const
        {
            return d_data;
        }
    };
//..
// Now, we write a test function, 'runTest', to verify our simple wrapper
// type.  We start by wrapping a simple 'int' value:
//..
    void runTests()
    {
        int i = 42;

        Wrapper<int> ti(i);  const Wrapper<int>& TI = ti;
        ASSERT(42 == i);
        ASSERT(42 == TI.value());

        ti.value() = 13;
        ASSERT(42 == i);
        ASSERT(13 == TI.value());
//..
// Finally, we test 'Wrapper' with a reference type:
//..
        Wrapper<int&> tr(i);  const Wrapper<int&>& TR = tr;
        ASSERT(42 == i);
        ASSERT(42 == TR.value());

        tr.value() = 13;
        ASSERT(13 == i);
        ASSERT(13 == TR.value());

        i = 42;
        ASSERT(42 == i);
        ASSERT(42 == TR.value());
    }
//..

}  // close namespace USAGE_EXAMPLE_1

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    (void) veryVerbose;      // eliminate unused variable warning

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating use of 'bslmf::AddReference'.
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");
//..
        ASSERT_SAME(bslmf::AddReference<int >::Type, int&);
        ASSERT_SAME(bslmf::AddReference<int&>::Type, int&);
        ASSERT_SAME(bslmf::AddReference<int volatile >::Type, volatile int&);
        ASSERT_SAME(bslmf::AddReference<int volatile&>::Type, volatile int&);

        ASSERT_SAME(bslmf::AddReference<MyType >::Type, MyType&);
        ASSERT_SAME(bslmf::AddReference<MyType&>::Type, MyType&);
        ASSERT_SAME(bslmf::AddReference<MyTypeRef>::Type, MyType&);
        ASSERT_SAME(bslmf::AddReference<MyType const >::Type, const MyType&);
        ASSERT_SAME(bslmf::AddReference<MyType const&>::Type, const MyType&);
        ASSERT_SAME(bslmf::AddReference<const MyTypeRef>::Type, MyType&);
//      ASSERT_SAME(bslmf::AddReference<const MyTypeRef&>::Type, MyType&);C++11

        ASSERT_SAME(bslmf::AddReference<void  >::Type, void);
        ASSERT_SAME(bslmf::AddReference<void *>::Type, void *&);
//..

      } break;
       case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating use of 'bslmf::AddReference'.
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        USAGE_EXAMPLE_1::runTests();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf::AddReference' with various types and verify
        //   that its 'Type' typedef is set to a non-reference type.
        // --------------------------------------------------------------------

        if (verbose) printf("\nbslmf::AddReference"
                            "\n==================\n");

        ASSERT_SAME(bslmf::AddReference<int       >::Type, int&);
        ASSERT_SAME(bslmf::AddReference<int&      >::Type, int&);
        ASSERT_SAME(bslmf::AddReference<int const >::Type, const int&);
        ASSERT_SAME(bslmf::AddReference<int const&>::Type, const int&);

        ASSERT_SAME(bslmf::AddReference<void *          >::Type, void *&);
        ASSERT_SAME(bslmf::AddReference<void *&         >::Type, void *&);
        ASSERT_SAME(bslmf::AddReference<void volatile *&>::Type,
                    volatile void *&);
        ASSERT_SAME(bslmf::AddReference<char const *const&>::Type,
                    const char *const&);

        ASSERT_SAME(bslmf::AddReference<Enum        >::Type, Enum&);
        ASSERT_SAME(bslmf::AddReference<Enum&       >::Type, Enum&);
        ASSERT_SAME(bslmf::AddReference<Struct      >::Type, Struct&);
        ASSERT_SAME(bslmf::AddReference<Struct&     >::Type, Struct&);
        ASSERT_SAME(bslmf::AddReference<Union       >::Type, Union&);
        ASSERT_SAME(bslmf::AddReference<Union&      >::Type, Union&);
        ASSERT_SAME(bslmf::AddReference<Class       >::Type, Class&);
        ASSERT_SAME(bslmf::AddReference<const Class&>::Type, const Class&);

        ASSERT_SAME(bslmf::AddReference<INT >::Type, int&);
        ASSERT_SAME(bslmf::AddReference<INT&>::Type, int&);

        ASSERT_SAME(bslmf::AddReference<int Class::* >::Type, int Class::*&);
        ASSERT_SAME(bslmf::AddReference<int Class::*&>::Type, int Class::*&);

        ASSERT_SAME(bslmf::AddReference<  F>::Type,  F&);
        ASSERT_SAME(bslmf::AddReference< RF>::Type,  F&);
        ASSERT_SAME(bslmf::AddReference< PF>::Type, PF&);
        ASSERT_SAME(bslmf::AddReference<RPF>::Type, PF&);

        ASSERT_SAME(bslmf::AddReference< Fi >::Type, Fi&);
        ASSERT_SAME(bslmf::AddReference<RFi >::Type, Fi&);
        ASSERT_SAME(bslmf::AddReference< FRi>::Type, FRi&);
        ASSERT_SAME(bslmf::AddReference<RFRi>::Type, FRi&);

        ASSERT_SAME(bslmf::AddReference< A>::Type, A&);
        ASSERT_SAME(bslmf::AddReference<RA>::Type, A&);

        ASSERT_SAME(bslmf::AddReference<void         >::Type, void);
        ASSERT_SAME(bslmf::AddReference<void const   >::Type, const void);
        ASSERT_SAME(bslmf::AddReference<void volatile>::Type, volatile void);
        ASSERT_SAME(bslmf::AddReference<void const volatile>::Type,
                    const volatile void);

        if (verbose) printf("\nbsl::add_lvalue_reference"
                            "\n=========================\n");

        ASSERT_SAME(bsl::add_lvalue_reference<int       >::type, int&);
        ASSERT_SAME(bsl::add_lvalue_reference<int&      >::type, int&);
        ASSERT_SAME(bsl::add_lvalue_reference<int const >::type, const int&);
        ASSERT_SAME(bsl::add_lvalue_reference<int const&>::type, const int&);

        ASSERT_SAME(bsl::add_lvalue_reference<void *          >::type,
                    void *&);
        ASSERT_SAME(bsl::add_lvalue_reference<void *&         >::type,
                    void *&);
        ASSERT_SAME(bsl::add_lvalue_reference<void volatile *&>::type,
                    volatile void *&);
        ASSERT_SAME(bsl::add_lvalue_reference<char const *const&>::type,
                    const char *const&);

        ASSERT_SAME(bsl::add_lvalue_reference<Enum        >::type, Enum&);
        ASSERT_SAME(bsl::add_lvalue_reference<Enum&       >::type, Enum&);
        ASSERT_SAME(bsl::add_lvalue_reference<Struct      >::type, Struct&);
        ASSERT_SAME(bsl::add_lvalue_reference<Struct&     >::type, Struct&);
        ASSERT_SAME(bsl::add_lvalue_reference<Union       >::type, Union&);
        ASSERT_SAME(bsl::add_lvalue_reference<Union&      >::type, Union&);
        ASSERT_SAME(bsl::add_lvalue_reference<Class       >::type, Class&);
        ASSERT_SAME(bsl::add_lvalue_reference<const Class&>::type,
                    const Class&);

        ASSERT_SAME(bsl::add_lvalue_reference<INT >::type, int&);
        ASSERT_SAME(bsl::add_lvalue_reference<INT&>::type, int&);

        ASSERT_SAME(bsl::add_lvalue_reference<int Class::* >::type,
                    int Class::*&);
        ASSERT_SAME(bsl::add_lvalue_reference<int Class::*&>::type,
                    int Class::*&);

        ASSERT_SAME(bsl::add_lvalue_reference<  F>::type,  F&);
        ASSERT_SAME(bsl::add_lvalue_reference< RF>::type,  F&);
        ASSERT_SAME(bsl::add_lvalue_reference< PF>::type, PF&);
        ASSERT_SAME(bsl::add_lvalue_reference<RPF>::type, PF&);

        ASSERT_SAME(bsl::add_lvalue_reference< Fi >::type, Fi&);
        ASSERT_SAME(bsl::add_lvalue_reference<RFi >::type, Fi&);
        ASSERT_SAME(bsl::add_lvalue_reference< FRi>::type, FRi&);
        ASSERT_SAME(bsl::add_lvalue_reference<RFRi>::type, FRi&);

        ASSERT_SAME(bsl::add_lvalue_reference< A>::type, A&);
        ASSERT_SAME(bsl::add_lvalue_reference<RA>::type, A&);

        ASSERT_SAME(bsl::add_lvalue_reference<void         >::type,
                    void);
        ASSERT_SAME(bsl::add_lvalue_reference<void const   >::type,
                    const void);
        ASSERT_SAME(bsl::add_lvalue_reference<void volatile>::type,
                    volatile void);
        ASSERT_SAME(bsl::add_lvalue_reference<void const volatile>::type,
                    const volatile void);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        if (verbose) printf("\nbsl::add_rvalue_reference"
                            "\n=========================\n");

        ASSERT_SAME(bsl::add_rvalue_reference<int       >::type, int&&);
        ASSERT_SAME(bsl::add_rvalue_reference<int&      >::type, int&);
        ASSERT_SAME(bsl::add_rvalue_reference<int const >::type, const int&&);
        ASSERT_SAME(bsl::add_rvalue_reference<int const&>::type, const int&);

        ASSERT_SAME(bsl::add_rvalue_reference<void *          >::type,
                    void *&&);
        ASSERT_SAME(bsl::add_rvalue_reference<void *&         >::type,
                    void *&);
        ASSERT_SAME(bsl::add_rvalue_reference<void volatile *&>::type,
                    volatile void *&);
        ASSERT_SAME(bsl::add_rvalue_reference<char const *const&>::type,
                    const char *const&);

        ASSERT_SAME(bsl::add_rvalue_reference<Enum        >::type, Enum&&);
        ASSERT_SAME(bsl::add_rvalue_reference<Enum&       >::type, Enum&);
        ASSERT_SAME(bsl::add_rvalue_reference<Struct      >::type, Struct&&);
        ASSERT_SAME(bsl::add_rvalue_reference<Struct&     >::type, Struct&);
        ASSERT_SAME(bsl::add_rvalue_reference<Union       >::type, Union&&);
        ASSERT_SAME(bsl::add_rvalue_reference<Union&      >::type, Union&);
        ASSERT_SAME(bsl::add_rvalue_reference<Class       >::type, Class&&);
        ASSERT_SAME(bsl::add_rvalue_reference<const Class&>::type,
                    const Class&);

        ASSERT_SAME(bsl::add_rvalue_reference<INT >::type, int&&);
        ASSERT_SAME(bsl::add_rvalue_reference<INT&>::type, int&);

        ASSERT_SAME(bsl::add_rvalue_reference<int Class::* >::type,
                    int Class::*&&);
        ASSERT_SAME(bsl::add_rvalue_reference<int Class::*&>::type,
                    int Class::*&);

        ASSERT_SAME(bsl::add_rvalue_reference<  F>::type,  F&&);
        ASSERT_SAME(bsl::add_rvalue_reference< RF>::type,  F&);
        ASSERT_SAME(bsl::add_rvalue_reference< PF>::type, PF&&);
        ASSERT_SAME(bsl::add_rvalue_reference<RPF>::type, PF&);

        ASSERT_SAME(bsl::add_rvalue_reference< Fi >::type, Fi&&);
        ASSERT_SAME(bsl::add_rvalue_reference<RFi >::type, Fi&);
        ASSERT_SAME(bsl::add_rvalue_reference< FRi>::type, FRi&&);
        ASSERT_SAME(bsl::add_rvalue_reference<RFRi>::type, FRi&);

        ASSERT_SAME(bsl::add_rvalue_reference< A>::type, A&&);
        ASSERT_SAME(bsl::add_rvalue_reference<RA>::type, A&);

        ASSERT_SAME(bsl::add_rvalue_reference<void         >::type,
                    void);
        ASSERT_SAME(bsl::add_rvalue_reference<void const   >::type,
                    const void);
        ASSERT_SAME(bsl::add_rvalue_reference<void volatile>::type,
                    volatile void);
        ASSERT_SAME(bsl::add_rvalue_reference<void const volatile>::type,
                    const volatile void);
#endif
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
