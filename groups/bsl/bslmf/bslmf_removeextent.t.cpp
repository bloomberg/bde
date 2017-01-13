// bslmf_removeextent.t.cpp                                           -*-C++-*-

#include "bslmf_removeextent.h"
#include <bslmf_issame.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
// The component under test is a simple metafunction with a well-define set of
// input and output types.  This test driver consists of applying a sequence
// of simple test input types and verifying the correct output types.
//
//-----------------------------------------------------------------------------
// [ 1] bsl::remove_extent<TYPE>::type
//-----------------------------------------------------------------------------
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
//                  COMPONENT SPECIFIC MACROS FOR TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1700
# define BSLMF_REMOVEEXTENT_NO_REFERENCES_TO_ARRAY_OF_UNKNOWN_BOUND 1
// Old microsoft compilers do not support references to arrays of unknown
// bound.
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

struct MyClass
{
    int d_value;
};

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage
///-----
// The class template 'Traverser' is used to traverse an array and perform some
// operation.  In order to do its job in the case of two-dimensional arrays,
// 'Traverser' must hold on to an entire row of the array at a time in order to
// process it correctly.  The row type is determined from the array type using
// 'remove_extent':
//..
    template <class ARRAY_TYPE>
    class Traverser {
    public:
        typedef typename bsl::remove_extent<ARRAY_TYPE>::type RowType;

    private:
        RowType d_row;  // Might be scalar
        // ...
    };
//..
// Now we can see that the row type is the type of the array after having
// striped off the high-order dimension:
//..
    int usageExample()
    {
        ASSERT((bsl::is_same<int, Traverser<int>::RowType>::value));
        ASSERT((bsl::is_same<int, Traverser<int[]>::RowType>::value));
        ASSERT((bsl::is_same<int, Traverser<int[5]>::RowType>::value));
        typedef const int MyRow[6];
        ASSERT((bsl::is_same<MyRow, Traverser<MyRow[]>::RowType>::value));
        ASSERT((bsl::is_same<int[6], Traverser<int[7][6]>::RowType>::value));

        return 0;
    }
//..

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

    (void) veryVerbose;          // eliminate unused variable warning
    (void) veryVeryVerbose;      // eliminate unused variable warning
    (void) veryVeryVeryVerbose;  // eliminate unused variable warning

    setbuf(stdout, NULL);       // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 The usage example from the component documentation compiles and
        //:   runs correctly.
        //
        // Plan:
        //: 1 For concern 1, copy the usage example from the component
        //:   documentation, replacing 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usageExample();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // COMPLETE TEST
        //
        // Concerns:
        //: 1 If 'TYPE' is a scalar, function, pointer, pointer-to-function,
        //:   pointer-to-member, or void type,
        //:   'bsl::remove_extent<TYPE>::type' is exactly 'TYPE'.
        //: 2 If 'TYPE' is a one-dimensional array of unknown bound, 'U[]',
        //:   then 'bsl::remove_extent<TYPE>::type' is 'U'
        //: 3 If 'TYPE' is a one-dimensional array of known bound, 'U[N]',
        //:   then 'bsl::remove_extent<TYPE>::type' is 'U'
        //: 4 If 'TYPE' is a two-dimensional array of unknown bound,
        //:   'U[][M]', then 'bsl::remove_extent<TYPE>::type' is 'U[M]'.
        //:   Similarly for a three-dimensional array of unknown high-order
        //:   bound.
        //: 5 If 'TYPE' is a two-dimensional array of known bound,
        //:   'U[N][M]', then 'bsl::remove_extent<TYPE>::type' is 'U[M]'.
        //:   Similarly for a three-dimensional array of known high-order
        //:   bound.
        //: 6 Cv-qualification on scalars or array elements is preserved.
        //: 7 If 'TYPE' is a lvalue reference type 'U&', then
        //:   'bsl::remove_extent<TYPE>::type' is exactly 'U&', even if 'U' is
        //:   an array type.
        //: 8 If 'TYPE' is a rvalue reference type 'U&&', then
        //:   'bsl::remove_extent<TYPE>::type' is exactly 'U&&', even if 'U' is
        //:   an array type.
        //
        // Plan:
        //: 1 For each of the above concerns, instantiate
        //:   'bsl::remove_extent<TYPE>' with an appropriate 'TYPE'.
        //: 2  Use 'bsl::is_same' to verify that
        //:    'bsl::remove_extent<TYPE>::type' is as expected.
        //
        // Testing:
        //   bsl::remove_extent<TYPE>::type
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOMPLETE TEST"
                            "\n=============\n");

#define TEST(a,b) ASSERT((bsl::is_same<bsl::remove_extent<a>::type, b>::value))

        //   TYPE                     remove_extent<TYPE>::type Concern #
        //   =======================  ========================= =========
        TEST(int                    , int                  );   // 1
        TEST(const int              , const int            );
        TEST(MyClass                , MyClass              );
        TEST(void (double)          , void (double)        );
        TEST(char *                 , char *               );
        TEST(void (*)(int)          , void (*)(int)        );
        TEST(int MyClass::*         , int MyClass::*       );
        TEST(short[]                , short                );   // 2
        TEST(int*[]                 , int*                 );
        TEST(double[10]             , double               );   // 3
        TEST(short*[10]             , short*               );
        TEST(char[][20]             , char[20]             );   // 4
        TEST(int*[][20]             , int*[20]             );
        TEST(char[][20][30]         , char[20][30]         );
        TEST(int*[][20][30]         , int*[20][30]         );
        TEST(long[10][40]           , long[40]             );   // 5
        TEST(const char*[10][40]    , const char*[40]      );
        TEST(long[10][40][50]       , long[40][50]         );
        TEST(char*[10][40][50]      , char*[40][50]        );
        TEST(volatile short[]       , volatile short       );   // 6
        TEST(const double[10]       , const double         );
        TEST(double *const[10]      , double *const        );
        TEST(volatile char[][20]    , volatile char[20]    );
        TEST(volatile char[][20][30], volatile char[20][30]);
        TEST(const long[10][40]     , const long[40]       );
        TEST(const long[10][40][50] , const long[40][50]   );
        TEST(int&                   , int&                 );   // 7
        TEST(const int&             , const int&           );
        TEST(double(&)[10]          , double(&)[10]        );
        TEST(long(&)[10][30]        , long(&)[10][30]      );
#if !defined(BSLMF_REMOVEEXTENT_NO_REFERENCES_TO_ARRAY_OF_UNKNOWN_BOUND)
        TEST(short(&)[]             , short(&)[]           );
        TEST(const char(&)[][20]    , const char(&)[][20]  );
#endif // BSLMF_REMOVEEXTENT_NO_REFERENCES_TO_ARRAY_OF_UNKNOWN_BOUND
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        TEST(int&&                  , int&&                );   // 8
        TEST(const int&&            , const int&&          );
        TEST(short(&&)[]            , short(&&)[]          );
        TEST(double(&&)[10]         , double(&&)[10]       );
        TEST(const char(&&)[][20]   , const char(&&)[][20] );
        TEST(long(&&)[10][30]       , long(&&)[10][30]     );
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

#undef TEST

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
// Copyright 2016 Bloomberg Finance L.P.
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
