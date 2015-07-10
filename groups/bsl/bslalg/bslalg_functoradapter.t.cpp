// bslalg_functoradapter.t.cpp                                        -*-C++-*-
#include <bslalg_functoradapter.h>

#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>
#include <bsls_nativestd.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

#include <functional>

using namespace BloombergLP;
using namespace bslalg;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// [ 1] Type
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

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                                 TYPE TRAITS
// ----------------------------------------------------------------------------

// ============================================================================
//                     GLOBAL CONSTANTS USED FOR TESTING
// ----------------------------------------------------------------------------
static int numTestFunctionCalls;
// ============================================================================
//                               TEST APPARATUS
// ----------------------------------------------------------------------------

namespace {

                       // =================
                       // class TestFunctor
                       // =================

class TestFunctor {

  public:
    void operator() () {
    }
};

void testFunction()
{
    ++numTestFunctionCalls;
}



}  // close unnamed namespace

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Using function pointer base for an empty-base optimized class
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we wanted to define a binder that binds a parameter of the
// binary predicate to a value passed on construction, and we want to use the
// empty-base optimization to avoid paying storage cost when the predicate type
// is a functor type with no data members.  Unfortunately, the binder cannot
// directly inherit from the parameterized binary predicate type, because it
// may be a function pointer type; instead, binder can inherit from
// 'FunctorAdapter::Type', which adapts a function pointer type to a functor
// type that is a suitable base class.
//
// First, we define the class 'Bind2ndInteger', which inherits from
// 'FunctorAdapter::Type' to take advantage of the empty-base optimization:
//..

template <class BINARY_PREDICATE>
#ifdef BSLS_PLATFORM_CMP_MSVC
// Visual studio compiler fails to resolve the conversion operator in
// 'bslalg::FunctorAdapter_FunctionPointer' when using private inheritance.
// Below is a workaround until a more suitable way the resolve this issue can
// be found.
class Bind2ndInteger : public FunctorAdapter<BINARY_PREDICATE>::Type {
#else
class Bind2ndInteger : private FunctorAdapter<BINARY_PREDICATE>::Type {
#endif
    // This class provides a functor that delegate its function-call operator
    // to the parameterized 'BINARY_PREDICATE', passing the user supplied
    // parameter as the first argument and the integer value passed on
    // construction as the second argument.

    // DATA
    int d_bondValue;  // the bound value

    // NOT IMPLEMENTED
    Bind2ndInteger(const Bind2ndInteger&);
    Bind2ndInteger& operator=(const Bind2ndInteger&);

  public:
    // CREATORS
    Bind2ndInteger(int value, const BINARY_PREDICATE& predicate);
        // Create a 'Bind2ndInteger' object that will bind the second parameter
        // of the specified 'predicate' with the specified integer 'value'.

    // ~Bind2ndInteger() = default;
        // Destroy this object.

    // ACCESSORS
    bool operator() (const int value) const;
        // Return the result of calling the parameterized 'BINARY_PREDICATE'
        // passing the specified 'value' as the first argument and the integer
        // value passed on construction as the second argument.
};
//..
//  Then, we implement the methods of the 'Bind2ndInteger' class:
//..
template <class BINARY_PREDICATE>
Bind2ndInteger<BINARY_PREDICATE>::Bind2ndInteger(int value,
                                             const BINARY_PREDICATE& predicate)
: FunctorAdapter<BINARY_PREDICATE>::Type(predicate), d_bondValue(value)
{
}
//..
// Here, we implement the 'operator()' member function that simply delegates to
// 'BINARY_PREDICATE'
//..
template <class BINARY_PREDICATE>
bool Bind2ndInteger<BINARY_PREDICATE>::operator() (const int value) const
{
    const BINARY_PREDICATE& predicate = *this;
    return predicate(value, d_bondValue);
}
//..
// Next, we define a function, 'intCompareFunction', that compares two
// integers:
//..
bool intCompareFunction(const int lhs, const int rhs)
{
    return lhs < rhs;
}
//..

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

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: This test driver is reusable w/other, similar components.

    // CONCERN: In no case does memory come from the global allocator.

    switch (test) { case 0:
      case 2: {
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
// Now, we define a 'Bind2ndInteger' object 'functorLessThan10' using the
// 'std::less<int>' functor as the parameterized 'BINARY_PREDICATE' and verify
// that the function call operator is working:
//..
        Bind2ndInteger<native_std::less<int> > functorLessThan10(10,
                                                      native_std::less<int>());

        ASSERT(functorLessThan10(1));
        ASSERT(!functorLessThan10(12));
//..
// Finally, we define a 'Bind2ndInteger' object 'functionLessThan10' passing
// the address of 'intCompareFunction' on construction and verify that the
// function call operator is working:
//..

        Bind2ndInteger<bool (*)(const int, const int)>
            functionLessThan10(10, &intCompareFunction);

        ASSERT(functionLessThan10(1));
        ASSERT(!functionLessThan10(12));
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // ALIAS
        // Concerns:
        //: 1 If 'CALLABLE_OBJECT' is a functor type, 'Type' is an alias to
        //:   'CALLABLE_OBJECT'
        //: 2 If 'CALLABLE_OBJECT' is a function pointer type, 'Type' is a
        //:   functor type that delegates its operations to a function of that
        //:   type.
        //
        // Plan:
        //  1 Using a functor type as the parameterized 'CALLABLE_OBJECT',
        //    verify that 'FunctorAdapter::Type' is an alias to that functor
        //    type.
        //  2 Using a function pointer type as the parameterized
        //    'CALLABLE_OBJECT', construct an object of 'FunctorAdapter::Type'
        //    and pass the address of a suitable function on
        //    construction.  Verify the object's function call operator
        //    delegates to the function pointer by calling the operator
        //    checking the global counter 'numTestFunctionCalls'.
        //
        // Testing:
        //   Type
        // --------------------------------------------------------------------

        typedef TestFunctor FunctorType;
        typedef void (*FunctionType) ();

        if (verbose)
            printf("\n\tTesting alias 'Type' for functors");
        {
            ASSERT(1 == (bsl::is_same<FunctorType,
                       FunctorAdapter<FunctorType>::Type>::value));
        }

        if (verbose)
            printf("\n\tTesting alias 'Type' for function pointers");
        {
          FunctorAdapter<FunctionType>::Type functor(&testFunction);
          numTestFunctionCalls = 0;


          FunctionType obj = functor;
          obj();

          ASSERT(1 == numTestFunctionCalls);
        }
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
