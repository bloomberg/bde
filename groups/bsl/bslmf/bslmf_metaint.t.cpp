// bslmf_metaint.t.cpp                                                -*-C++-*-

#include <bslmf_metaint.h>

#include <limits.h>
#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'
#include <string.h>  // 'strcmp'

using namespace BloombergLP;
using namespace bslmf;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//-----------------------------------------------------------------------------
// [ 1] VALUE
// [ 2] operator int() const;
// [ 2] operator bool() const;
// [ 3] implicit upcast to bsl::integral_constant<int, INT_VALUE>
// [ 3] operator bsl::false_type() const
// [ 3] operator bsl::true_type() const
// [ 4] conversion constructors
//-----------------------------------------------------------------------------
// [ 5] USAGE EXAMPLE
//=============================================================================

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static bool         verbose = false;
static bool     veryVerbose = false;
static bool veryVeryVerbose = false;

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

namespace {

struct AnyType
{
    // Type convertible from any other type.
    template <class TYPE>
    AnyType(const TYPE&) { }                                        // IMPLICIT
};

template <int VALUE>
bool matchIntConstant(bsl::integral_constant<int, VALUE>)
    // Return true when called with an 'integral_constant' of the specified
    // 'VALUE'.  Does not participate in overload resolution for
    // 'integral_constant's with a 'value' other than 'VALUE'.
{
    return true;
}

template <bool VALUE>
bool matchIntConstant(bsl::integral_constant<bool, VALUE>)
    // Return true when called with an 'integral_constant' of the specified
    // 'VALUE'.  Does not participate in overload resolution for
    // 'integral_constant's with a 'value' other than 'VALUE'.
{
    return true;
}

template <int VALUE>
bool matchIntConstant(AnyType)
    // Return false.  Overload resolution will select this function only when
    // the argument is other than 'integral_constant<int, VALUE>', e.g., when
    // called with an argument of type integral_constant<int, OTHER_VALUE>',
    // where 'Other_VALUE' is different from the specified 'VALUE' template
    // parameter.
{
    return false;
}

int dispatchOnIntConstant(int, const char*, bslmf::MetaInt<0>)
{
    return 0;
}

int dispatchOnIntConstant(int, const char*, bslmf::MetaInt<1>)
{
    return 1;
}

int dispatchOnIntConstant(int, const char*, bslmf::MetaInt<999>)
{
    return 999;
}

int dispatchOnBoolConstant(float, bsl::false_type)
{
    return 1;
}

int dispatchOnBoolConstant(float, bsl::true_type)
{
    return 2;
}

}  // close unnamed namespace

//=============================================================================
//                  CODE FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Example 1: Compile-Time Function Dispatching
/// - - - - - - - - - - - - - - - - - - - - - -
// The most common use of this structure is to perform static function
// dispatching based on a compile-time calculation.  Often the calculation is
// nothing more than a simple predicate, allowing us to select one of two
// functions.  The following function, 'doSomething', uses a fast
// implementation (e.g., 'memcpy') if the parameterized type allows for such
// operations, otherwise it will use a more generic and slower implementation
// (e.g., copy constructor).
//..
    template <class T>
    void doSomethingImp(T *t, bslmf::MetaInt<0>)
    {
        // slow generic implementation
        (void) t;
        // ...
    }

    template <class T>
    void doSomethingImp(T *t, bslmf::MetaInt<1>)
    {
        // fast implementation (works only for some T's)
        (void) t;
        // ...
    }

    template <class T, bool IsFast>
    void doSomething(T *t)
    {
        doSomethingImp(t, bslmf::MetaInt<IsFast>());
    }
//..
// The power of this approach is that the compiler will compile only the
// implementation selected by the 'MetaInt' argument.  For some parameter
// types, the fast version of 'doSomethingImp' would be ill-formed.  This kind
// of compile-time dispatch prevents the ill-formed version from ever being
// instantiated.
//..
    int usageExample1()
    {
        int i;
        doSomething<int, true>(&i); // fast version selected for int

        double m;
        doSomething<double, false>(&m); // slow version selected for double

        return 0;
    }
//..
///Example 2: Reading the 'VALUE' member
/// - - - - - - - - - - - - - - - - - -
// In addition to forming new types, the value of the integral paramameter to
// 'MetaInt' is "saved" in the enum member 'VALUE', and is accessible for use
// in compile-time or run-time operations.
//..
    template <int V>
    unsigned g()
    {
        bslmf::MetaInt<V> i;
        ASSERT(V == i.VALUE);
        ASSERT(V == bslmf::MetaInt<V>::VALUE);
        return bslmf::MetaInt<V>::VALUE;
    }

    int usageExample2()
    {
        int v = g<1>();
        ASSERT(1 == v);
        return 0;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting USAGE EXAMPLE"
                            "\n=====================\n");

        usageExample1();
        usageExample2();

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CONVERSION CONSTRUCTORS
        //
        // Concerns:
        //: 1 'MetaInt<V>' is constructible from 'integral_constant<int, V>'.
        //:
        //: 2 Given several function overloads that take an argument of type
        //:   'MetaInt<V>' for different values of 'V' and
        //:   otherwise identical parameters, passing an argument of type
        //:   'integral_constant<int, X>' will dispatch to overload that takes
        //:   'MetaInt<X>'.
        //:
        //: 3 Given two function overloads with identical parameters such
        //:   that the first takes an argument of type 'bsl::false_type' and
        //:   the second takes an argument of type 'bsl::true_type', passing
        //:   an argument of type 'MetaInt<0>' will dispatch to the first and
        //:   passing an argument of type 'MetaInt<1>' will dispatch t the
        //:   second.
        //
        // TBD: Incomplete specification and implementation of this test case
        // Plan:
        //: 1 Create a function template, 'matchIntConstant<V>' having two
        //:   overloads: one that takes an argument of type
        //:   'integral_constant<int, V>' and returns 'true', and another which
        //:   takes an argument of a type convertible from *any*
        //:   'integral_constant' and returns 'false'.  For various values 'V',
        //:   construct rvalues of type 'MetaInt<V>' and call
        //:   'matchIntConstant<V>', verifying that it returns
        //:   'true'. (C-1)
        //:
        //: 2 For various values 'V' and 'X' such that 'V != X', construct
        //:   rvalues of type 'MetaInt<V>' and call 'matchIntConstant<X>',
        //:   verifying that it returns 'false'. (C-2)
        //:
        //: 3 Create a set of overloaded functions,
        //:   'dispatchOnIntConstant' taking identical arguments except
        //:   that the last parameter is of type 'integral_constant<int, V>'
        //:   for several values of 'V'.  The return value of
        //:   'dispatchOnIntConstant' is an 'int' with value 'V'.  Call
        //:   'dispatchOnIntConstant' several times, each time passing a
        //:   different instantiation of 'integral_constant<int, V>' and
        //:   verifying that the return value is as expected (i.e., that the
        //:   call dispatched to the correct overload).  (C-3)
        //:
        //: 4 Create a pair of overloaded functions, 'dispatchOnBoolConstant'
        //:   taking identical arguments except that the last parameter of the
        //:   first overload is 'false_type' and the last parameter of the
        //:   second overload is 'true_type'.  The return value of
        //:   'dispatchOnBoolConstant' is an 'int' with value 1 for the first
        //:   overload and 2 for the second overload.  Call
        //:   'dispatchOnBoolConstant', passing it 'MetaInt<0>' as the last
        //:   argument and verify that it returns 1.  Call
        //:   'dispatchOnBoolConstant', passing it 'MetaInt<1>' as the last
        //:   argument and verify that it returns 2. (C-4)
        //
        // Testing:
        //      implicit upcast to bsl::integral_constant<int, INT_VALUE>
        //      inheritance from bsl::integral_constant<int, INT_VALUE>
        //      operator bsl::false_type() const;  // MetaInt<0> only
        //      operator bsl::true_type() const;   // MetaInt<1> only
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONVERSION TO integral_constant"
                            "\n======================================\n");

        if (veryVerbose) printf("Testing good conversions\n");
        ASSERT(  (matchIntConstant<0>(MetaInt<0>())));
        ASSERT(  (matchIntConstant<1>(MetaInt<1>())));
        ASSERT(  (matchIntConstant<1000>(MetaInt<1000>())));
        ASSERT(  (matchIntConstant<INT_MAX>(MetaInt<INT_MAX>())));

        if (veryVerbose) printf("Testing bad conversions\n");
        ASSERT(! (matchIntConstant<0>(MetaInt<1>())));
        ASSERT(! (matchIntConstant<1>(MetaInt<0>())));
        ASSERT(! (matchIntConstant<-99>(MetaInt<99>())));
        ASSERT(! (matchIntConstant<1000>(MetaInt<6>())));

        if (veryVerbose) printf("Testing int dispatch\n");
        ASSERT(0 == dispatchOnIntConstant(9, "hello", MetaInt<0>()));
        ASSERT(1 == dispatchOnIntConstant(8, "world", MetaInt<1>()));
        ASSERT(999 == dispatchOnIntConstant(8, "nine", MetaInt<999>()));

        if (veryVerbose) printf("Testing bool dispatch\n");
        ASSERT(1 == dispatchOnBoolConstant(0.3f, MetaInt<0>()));
        ASSERT(2 == dispatchOnBoolConstant(5.2f, MetaInt<1>()));

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONVERSION TO integral_constant
        //
        // Concerns:
        //: 1 'MetaInt<V>' is convertible to 'integral_constant<int, V>'.
        //:
        //: 2 'MetaInt<V>' is NOT convertible to 'integral_constant<int, X>',
        //:   'X != V'.
        //:
        //: 3 Given several function overloads that take an argument of type
        //:   'integral_constant<int, V>' for different values of 'V' and
        //:   otherwise identical parameters, passing an argument of type
        //:   'MetaInt<X>' will dispatch to overload that takes
        //:   'integral_constant<int, X>'.
        //:
        //: 4 Given two function overloads with identical parameters such
        //:   that the first takes an argument of type 'bsl::false_type' and
        //:   the second takes an argument of type 'bsl::true_type', passing
        //:   an argument of type 'MetaInt<0>' will dispatch to the first and
        //:   passing an argument of type 'MetaInt<1>' will dispatch t the
        //:   second.
        //
        // Plan:
        //: 1 Create a function template, 'matchIntConstant<V>' having two
        //:   overloads: one that takes an argument of type
        //:   'integral_constant<int, V>' and returns 'true', and another which
        //:   takes an argument of a type convertible from *any*
        //:   'integral_constant' and returns 'false'.  For various values 'V',
        //:   construct rvalues of type 'MetaInt<V>' and call
        //:   'matchIntConstant<V>', verifying that it returns
        //:   'true'. (C-1)
        //:
        //: 2 For various values 'V' and 'X' such that 'V != X', construct
        //:   rvalues of type 'MetaInt<V>' and call 'matchIntConstant<X>',
        //:   verifying that it returns 'false'. (C-2)
        //:
        //: 3 Create a set of overloaded functions,
        //:   'dispatchOnIntConstant' taking identical arguments except
        //:   that the last parameter is of type 'integral_constant<int, V>'
        //:   for several values of 'V'.  The return value of
        //:   'dispatchOnIntConstant' is an 'int' with value 'V'.  Call
        //:   'dispatchOnIntConstant' several times, each time passing a
        //:   different instantiation of 'integral_constant<int, V>' and
        //:   verifying that the return value is as expected (i.e., that the
        //:   call dispatched to the correct overload).  (C-3)
        //:
        //: 4 Create a pair of overloaded functions, 'dispatchOnBoolConstant'
        //:   taking identical arguments except that the last parameter of the
        //:   first overload is 'false_type' and the last parameter of the
        //:   second overload is 'true_type'.  The return value of
        //:   'dispatchOnBoolConstant' is an 'int' with value 1 for the first
        //:   overload and 2 for the second overload.  Call
        //:   'dispatchOnBoolConstant', passing it 'MetaInt<0>' as the last
        //:   argument and verify that it returns 1.  Call
        //:   'dispatchOnBoolConstant', passing it 'MetaInt<1>' as the last
        //:   argument and verify that it returns 2. (C-4)
        //
        // Testing:
        //      implicit upcast to bsl::integral_constant<int, INT_VALUE>
        //      inheritance from bsl::integral_constant<int, INT_VALUE>
        //      operator bsl::false_type() const;  // MetaInt<0> only
        //      operator bsl::true_type() const;   // MetaInt<1> only
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONVERSION TO integral_constant"
                            "\n======================================\n");

        if (veryVerbose) printf("Testing good conversions\n");
        ASSERT(  (matchIntConstant<0>(MetaInt<0>())));
        ASSERT(  (matchIntConstant<1>(MetaInt<1>())));
        ASSERT(  (matchIntConstant<1000>(MetaInt<1000>())));
        ASSERT(  (matchIntConstant<INT_MAX>(MetaInt<INT_MAX>())));

        if (veryVerbose) printf("Testing bad conversions\n");
        ASSERT(! (matchIntConstant<0>(MetaInt<1>())));
        ASSERT(! (matchIntConstant<1>(MetaInt<0>())));
        ASSERT(! (matchIntConstant<-99>(MetaInt<99>())));
        ASSERT(! (matchIntConstant<1000>(MetaInt<6>())));

        if (veryVerbose) printf("Testing int dispatch\n");
        ASSERT(0 == dispatchOnIntConstant(9, "hello", MetaInt<0>()));
        ASSERT(1 == dispatchOnIntConstant(8, "world", MetaInt<1>()));
        ASSERT(999 == dispatchOnIntConstant(8, "nine", MetaInt<999>()));

        if (veryVerbose) printf("Testing bool dispatch\n");
        ASSERT(1 == dispatchOnBoolConstant(0.3f, MetaInt<0>()));
        ASSERT(2 == dispatchOnBoolConstant(5.2f, MetaInt<1>()));

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CONVERSION TO int
        //
        // Concerns:
        //: 1 'MetaInt<V>' is convertible to 'int' with a resulting value of
        //:   'V'.
        //:
        //: 2 'MetaInt<0>' is convertible to 'bool' with a resulting value of
        //:   'false'.
        //:
        //: 3 'MetaInt<0>' is convertible to 'bool' with a resulting value of
        //:   'true'.
        //
        // Plan:
        //: 1 Define several 'int' variables, initializing each one with a
        //:   different 'MetaInt<V>' type.  Verify that the value of each
        //:   'int' is the corresponding 'V'. (C1)
        //:
        //: 2 Define a 'bool' variable, initializing it with a 'MetaInt<0>'
        //:   object. Verify that the resulting value is false. (C2)
        //:
        //: 3 Define a 'bool' variable, initializing it with a 'MetaInt<1>'
        //:   object. Verify that the resulting value is true. (C3)
        //
        // Testing:
        //      operator int() const;
        //      operator bool() const; // MetaInt<0> and MetaInt<1> only
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONVERSION TO 'int'"
                            "\n===========================\n");

        int a = MetaInt<0>();
        ASSERT(0 == a);

        int b = MetaInt<1>();
        ASSERT(1 == b);

        int d = MetaInt<INT_MAX>();
        ASSERT(INT_MAX == d);

        bool f = MetaInt<0>();
        ASSERT(!f);

        bool g = MetaInt<1>();
        ASSERT(g);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING VALUE
        //
        // Test Plan:
        //   Instantiate 'MetaInt' with various constant integral
        //   values and verify that their 'VALUE' member is initialized
        //   properly.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING VALUE"
                            "\n=============\n");

        // verify that the 'VALUE' member is evaluated at compile-time
        enum {
            C1 = bslmf::MetaInt<1>::VALUE,
            C2 = bslmf::MetaInt<2>::VALUE,
            C0 = bslmf::MetaInt<0>::VALUE
        };

        ASSERT(0 == C0);
        ASSERT(1 == C1);
        ASSERT(2 == C2);

        bslmf::MetaInt<0> i0;
        bslmf::MetaInt<1> i1;
        bslmf::MetaInt<2> i2;

        ASSERT(0 == i0.VALUE);
        ASSERT(1 == i1.VALUE);
        ASSERT(2 == i2.VALUE);

        // Check inherited value
        ASSERT(0 == i0.value);
        ASSERT(1 == i1.value);
        ASSERT(2 == i2.value);

        // 'MetaInt' supports all non-negative integer values:
        ASSERT(0 == bslmf::MetaInt<0>::VALUE);
        ASSERT(1 == bslmf::MetaInt<1>::VALUE);
        ASSERT(INT_MAX == bslmf::MetaInt<INT_MAX>::VALUE);

#if ! defined(BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT)
        // The following tests will not compile if the current compiler
        // supports static asserts.

        // 'VALUE' is signed:
        ASSERT(bslmf::MetaInt<(unsigned) 5>::VALUE !=
               bslmf::MetaInt<(unsigned)-5>::VALUE);

        ASSERT(bslmf::MetaInt<           5>::VALUE !=
               bslmf::MetaInt<          -5>::VALUE);
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
