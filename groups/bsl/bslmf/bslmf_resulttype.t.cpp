// bslmf_resulttype.t.cpp                                             -*-C++-*-
#include <bslmf_resulttype.h>

#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>
#include <bsls_exceptionutil.h>

#include <cstring>   // 'strcmp'

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// This component defines a metafunction.  Testing involves defining types that
// model the concepts that this metafunction is trying to detect and verifying
// that it detects them correctly.  A small amount of infrastructure is needed
// to detect whether the metafunction produced any result at all.
//-----------------------------------------------------------------------------
// [1] TEST APPARATUS
// [2] FULL TEST
// [3] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

struct NoResultType {
    // A 'struct' that defines neither 'result_type' nor 'ResultType''
};

struct STDResultType {
    // A 'struct' that defines a standard-style 'result_type' type.

    typedef short result_type;
};

struct BDEResultType {
    // A 'struct' that defines a BDE-style 'ResultType' type.

    typedef int ResultType;
};

struct BothResultTypes {
    // A 'struct' that defines both 'result_type' and 'ResultType'.  This
    // component should prefer 'result_type' to 'ResultType'.

    typedef unsigned short result_type;
    typedef unsigned int   ResultType;
};

template <class TYPE, class = void>
struct HasType {
    // Metafunction with value 'true' if 'TYPE::type' names a valid type.

    enum { VALUE = false };
};

template <class TYPE>
struct HasType<TYPE, BSLMF_VOIDTYPE(typename TYPE::type)> {
    // Specialization for when 'TYPE::type' names a valid type.

    enum { VALUE = true };
};

struct WithType {
    // A 'struct' that defines 'type', for testing 'HasType'.

    typedef char *type;
};

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage Example 1
///- - - - - - - -
// In this example, we write a C++03-compatible function template,
// 'wrapInvoke<FUNC>(t_A1 a1, t_A2 a2)', that constructs an instance of functor
// 'FUNC', invokes it with arguments 'a1' and 'a2', and translates any thrown
// exception to a generic exception type.
//
// First, we declare the generic exception type:
//..
    struct InvocationException { };
//..
// Now, we declare 'wrapInvoke'.  The return type of 'wrapInvoke' should be the
// same as the return type of invoking an object of type 'FUNC'.  There is no
// non-intrusive way to deduce the return type of 'FUNC' in C++03.  We
// therefore require that 'FUNC' provide either a 'result_type' nested type
// (the idiom used in standard library functors) or a 'ResultType' nested type
// (the idiom used in BDE library functors).  We use 'bslmf::ResultType' to
// automatically select the correct idiom:
//..
    template <class FUNC, class t_A1, class t_A2>
    typename bslmf::ResultType<FUNC>::type
    wrapInvoke(t_A1 a1, t_A2 a2)
    {
        FUNC f;
        BSLS_TRY {
            return f(a1, a2);                                         // RETURN
        }
        BSLS_CATCH (...) {
            BSLS_THROW(InvocationException());
        }
    }
//..
// Next, we declare a functor class that compares its arguments, returns the
// string "less" if 'a1 < a2', returns "greater" if 'a2 > a1', and throws an
// exception if neither is true.  The return type of this functor is declared
// using the BDE-style 'ResultType' nested type:
//..
    struct LessGreater {
        typedef const char *ResultType;
        struct BadArgs { };  // exception class

        const char *operator()(long a1, long a2);
            // For the specified long integers 'a1' and 'a2', return the
            // null-terminated string "less" if 'a1 < a2' and "greater" if
            // 'a2 < a1'; otherwise, throw 'BadArgs()'.
    };

    const char *LessGreater::operator()(long a1, long a2)
    {
        if (a1 < a2) {
            return "less";                                            // RETURN
        }
        else if (a2 < a1) {
            return "greater";                                         // RETURN
        }
        else {
            BSLS_THROW(BadArgs());
        }
    }
//..
// For comparison, let's also define a 'plus' functor that conforms to the
// C++11 standard definition of 'std::plus':
//..
    template <class TYPE>
    struct plus {
        typedef TYPE first_argument_type;
        typedef TYPE second_argument_type;
        typedef TYPE result_type;
        TYPE operator()(const TYPE& x, const TYPE& y) const { return x + y; }
    };
//..
// Now, we can use 'wrapInvoke' with our 'LessGreater' functor:
//..
    int usageExample1()
    {
        const char *s = wrapInvoke<LessGreater>(5, -2);
        ASSERT(0 == std::strcmp(s, "greater"));
//..
// Finally we confirm that we can also use 'wrapInvoke' with the functor
// 'plus<int>':
//..
        int sum = wrapInvoke<plus<int> >(5, -2);
        ASSERT(3 == sum);

        return 0;
    }
//..
///Usage Example 2
///- - - - - - - -
// This example extends the previous one by considering a functor that does not
// declare either 'result_type' or 'ResultType'.  The 'PostIncrement' functor
// performs the operation '*a += b' and returns the old value of '*a':
//..
    struct PostIncrement {
        int operator()(int *a, int b)
        {
            unsigned tmp = *a;
            *a += b;
            return tmp;
        }
    };
//..
// Unfortunately, the following innocent-looking code is ill-formed; even
// though the return value is being discarded, the return type of 'wrapInvoke'
// is undefined because 'bslmf::ResultType<PostIncrement>::type' is undefined:
//..
    // int x = 10;
    // wrapInvoke<PostIncrement>(x, 2);  // ill-formed
//..
// To make 'wrapInvoke' usable in these situations, we define a new version,
// 'wrapInvoke2', that will fall back to a 'void' return type if neither
// 'FUNC::result_type' nor 'FUNC::ResultType' is defined:
//..
    template <class FUNC, class t_A1, class t_A2>
    typename bslmf::ResultType<FUNC, void>::type
    wrapInvoke2(t_A1 a1, t_A2 a2)
    {
        typedef typename bslmf::ResultType<FUNC, void>::type RetType;
        FUNC f;
        BSLS_TRY {
            // C-style cast needed for some compilers
            return (RetType)f(a1, a2);                                // RETURN
        }
        BSLS_CATCH (...) {
            BSLS_THROW(InvocationException());
        }
    }
//..
// This use of the fallback parameter allows us to use 'bslmf::ResultType' in a
// context where the return type of a function might be ignored:
//..
    int usageExample2()
    {
        int x = 10;
        wrapInvoke2<PostIncrement>(&x, 2);
        ASSERT(12 == x);
        return 0;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test    = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    // bool veryVerbose = argc > 3;
    // bool veryVeryVerbose = argc > 4;
    // bool veryVeryVeryVerbose = argc > 5;

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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usageExample1();
        usageExample2();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // FULL TEST
        //
        // Concerns:
        //: 1 'ResultType<T>::type' names 'T::result_type' when
        //:   'T::result_type' exists.
        //:
        //: 2 'ResultType<T>::type' names 'T::ResultType' when 'T::ResultType'
        //:   exists and 'T::result_type' does not exist.
        //:
        //: 3 'ResultType<T>::type' names 'T::result_type' when
        //:   'T::result_type' and 'T::ResultType' both exist.
        //:
        //: 4 'ResultType<T>::type' does not name a type when neither
        //:   'T::result_type' nor 'T::ResultType' exist.
        //:
        //: 5 'ResultType<T,F>::type' names 'F' when neither 'T::result_type'
        //:   nor 'T::ResultType' exist.  'F' is ignored if either or both of
        //:   'T::result_type' and 'T::ResultType' exist.
        //:
        //: 6 'ResultType<T,F>::type' is insensitive to cv-qualifications on
        //:   'T'.
        //
        // Plan:
        //: 1 For concerns 1-3, instantiate 'ResultType<T>' for representative
        //:   types that define one or both of 'T::result_type' and
        //:   'T::ResultType'.  Verify the expected 'type'.  (C-1..3)
        //:
        //: 2 For concern 4, instantiate 'ResultType<T>' for a type that
        //:   defines neither 'T::result_type' nor 'T::ResultType'.  Using
        //:   'HasType', verify that 'type' does not exist.  (C-4)
        //:
        //: 3 For concern 5, instantiate 'ResultType<T, float>' for the same
        //:   types as in P-1.  Verify that the result is the same as in P-1.
        //:   Instantiate 'ResultType<T, float>' on the same type as in P-2.
        //:   Verify that 'type' is 'float'.  (C-5)
        //:
        //: 4 For concern 6, repeat the above steps using cv-qualified versions
        //:   of 'T'.  Verify that the results are unchanged.  (C-6)
        //
        // Testing:
        //   FULL TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nFULL TEST"
                            "\n=========\n");

        using BloombergLP::bslmf::ResultType;

#define ASSERT_SAME(...) \
        ASSERT((bsl::is_same<__VA_ARGS__>::value))

        ASSERT(  HasType<ResultType<STDResultType  > >::VALUE);
        ASSERT(  HasType<ResultType<BDEResultType  > >::VALUE);
        ASSERT(  HasType<ResultType<BothResultTypes> >::VALUE);
        ASSERT(! HasType<ResultType<NoResultType   > >::VALUE);

        ASSERT_SAME(ResultType<STDResultType  >::type, short);
        ASSERT_SAME(ResultType<BDEResultType  >::type, int  );
        ASSERT_SAME(ResultType<BothResultTypes>::type, unsigned short);

        ASSERT((  HasType<ResultType<STDResultType  , float> >::VALUE));
        ASSERT((  HasType<ResultType<BDEResultType  , float> >::VALUE));
        ASSERT((  HasType<ResultType<BothResultTypes, float> >::VALUE));
        ASSERT((  HasType<ResultType<NoResultType   , float> >::VALUE));

        ASSERT_SAME(ResultType<STDResultType  , float>::type , short);
        ASSERT_SAME(ResultType<BDEResultType  , float>::type , int  );
        ASSERT_SAME(ResultType<BothResultTypes, float>::type , unsigned short);
        ASSERT_SAME(ResultType<NoResultType   , float>::type , float);

        // cv-qualified types
        ASSERT_SAME(ResultType<const volatile STDResultType>::type, short);
        ASSERT_SAME(ResultType<volatile BDEResultType      >::type, int  );
        ASSERT_SAME(ResultType<const BothResultTypes>::type, unsigned short);
        ASSERT(! HasType<ResultType<const NoResultType   > >::VALUE);

        ASSERT_SAME(ResultType<const STDResultType   , float>::type , short);
        ASSERT_SAME(ResultType<volatile BDEResultType, float>::type , int  );
        ASSERT_SAME(ResultType<const volatile BothResultTypes, float>::type,
                    unsigned short);
        ASSERT_SAME(ResultType<volatile NoResultType, float>::type , float);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //
        // Concerns:
        //: 1 'HasType<T>::VALUE' is 'true' if and only if 'T::type' is a valid
        //:   type.
        //
        // Plan:
        //: 1 Declare 'struct's with and without a nested 'type' 'typedef'.
        //:   Instantiate 'HasType<T>' with each of them and verify that
        //:   'VALUE' is true if and only if the nested 'typedef' exists.
        //
        // Testing:
        //   TEST APPARATUS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST APPARATUS"
                            "\n==============\n");

        ASSERT(false == HasType<NoResultType>::VALUE);
        ASSERT(true  == HasType<WithType>::VALUE);

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
// Copyright 2019 Bloomberg Finance L.P.
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
