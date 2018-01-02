// bslmf_isarray.t.cpp                                                -*-C++-*-

#include <bslmf_isarray.h>

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines two meta-functions, 'bsl::is_array' and
// 'bslmf::IsArray', that determine whether a template parameter type is an
// array type.  Thus, we need to ensure that the values returned by these
// meta-functions are correct for each possible category of types.  Since the
// two meta-functions are functionally equivalent, we will use the same set of
// types for both.
//
//-----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 2] BloombergLP::bslmf::IsArray::VALUE
// [ 1] bsl::is_array::value
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
//                  COMPONENT SPECIFIC MACROS FOR TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1700
# define BSLMF_ISARRAY_NO_REFERENCES_TO_ARRAY_OF_UNKNOWN_BOUND 1
// Old microsoft compilers compilers do not support references to arrays of
// unknown bound.
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum Enum {};

struct Struct {
    int a[4];
};

union Union {
    int  i;
    char a[sizeof(int)];
};

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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Array Types
///- - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is an array type.
//
// First, we create two 'typedef's -- an array type and a non-array type:
//..
    typedef int MyType;
    typedef int MyArrayType[];
//..
// Now, we instantiate the 'bsl::is_array' template for each of the 'typedef's
// and assert the 'value' static data member of each instantiation:
//..
    ASSERT(false == bsl::is_array<MyType>::value);
    ASSERT(true  == bsl::is_array<MyArrayType>::value);
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'bslmf::IsArray::VALUE'
        //   Ensure that 'bslmf::IsArray' returns the correct values for a
        //   variety of template parameter types.
        //
        // Concerns:
        //: 1 'IsArray' returns 'true' for a bounded array type.
        //:
        //: 2 'IsArray' returns 'true' for an unbounded array type.
        //:
        //: 3 'IsArray' returns 'false' for a reference to an array type.
        //:
        //: 4 'IsArray' returns 'false' for non-array types.
        //
        // Plan:
        //: 1 Verify that 'bslmf::IsArray' returns the correct value for each
        //:   concern.
        //
        // Testing:
        //   BloombergLP::bslmf::IsArray::VALUE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'bslmf::IsArray::VALUE'"
                            "\n===============================\n");

        ASSERT(1 == bslmf::IsArray<char          [1]>::VALUE);
        ASSERT(1 == bslmf::IsArray<char const    [1]>::VALUE);
        ASSERT(0 == bslmf::IsArray<char       (&)[1]>::VALUE);
        ASSERT(0 == bslmf::IsArray<char             >::VALUE);
        ASSERT(0 == bslmf::IsArray<char const       >::VALUE);
        ASSERT(0 == bslmf::IsArray<char        *    >::VALUE);

        ASSERT(1 == bslmf::IsArray<void       *   [2]>::VALUE);
        ASSERT(1 == bslmf::IsArray<void const *   [2]>::VALUE);
        ASSERT(0 == bslmf::IsArray<void       *(&)[2]>::VALUE);
        ASSERT(0 == bslmf::IsArray<void       *      >::VALUE);
        ASSERT(0 == bslmf::IsArray<void              >::VALUE);

        ASSERT(1 == bslmf::IsArray<int                [3]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const          [3]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int volatile       [3]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const volatile [3]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int                   >::VALUE);
        ASSERT(0 == bslmf::IsArray<int const             >::VALUE);
        ASSERT(0 == bslmf::IsArray<int volatile          >::VALUE);
        ASSERT(0 == bslmf::IsArray<int const volatile    >::VALUE);

        ASSERT(0 == bslmf::IsArray<int                (&)[4]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int const          (&)[4]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int volatile       (&)[4]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int const volatile (&)[4]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int                 &    >::VALUE);
        ASSERT(0 == bslmf::IsArray<int const           &    >::VALUE);
        ASSERT(0 == bslmf::IsArray<int volatile        &    >::VALUE);
        ASSERT(0 == bslmf::IsArray<int const volatile  &    >::VALUE);

        ASSERT(1 == bslmf::IsArray<int                 * [5]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const           * [5]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int volatile        * [5]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const volatile  * [5]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int                (*)[5]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int const          (*)[5]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int volatile       (*)[5]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int const volatile (*)[5]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int                 *    >::VALUE);
        ASSERT(0 == bslmf::IsArray<int const           *    >::VALUE);
        ASSERT(0 == bslmf::IsArray<int volatile        *    >::VALUE);
        ASSERT(0 == bslmf::IsArray<int const volatile  *    >::VALUE);

        ASSERT(1 == bslmf::IsArray<int         [6][6]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const   [6][6]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int      (&)[6][6]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int       * [6][6]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int      (*)[6][6]>::VALUE);

        ASSERT(1 == bslmf::IsArray<int *const    [6][6]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int *const (&)[6][6]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int *const (*)[6][6]>::VALUE);

        ASSERT(1 == bslmf::IsArray<void *[]>::VALUE);

        ASSERT(1 == bslmf::IsArray<int                []>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const          []>::VALUE);
        ASSERT(1 == bslmf::IsArray<int volatile       []>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const volatile []>::VALUE);

        ASSERT(1 == bslmf::IsArray<int                 * []>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const           * []>::VALUE);
        ASSERT(1 == bslmf::IsArray<int volatile        * []>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const volatile  * []>::VALUE);
        ASSERT(0 == bslmf::IsArray<int                (*)[]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int const          (*)[]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int volatile       (*)[]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int const volatile (*)[]>::VALUE);

        ASSERT(1 == bslmf::IsArray<int          [][7]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const    [][7]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int        * [][7]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int       (*)[][7]>::VALUE);

        ASSERT(1 == bslmf::IsArray<int *const    [][7]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int *const (*)[][7]>::VALUE);

#if !defined(BSLMF_ISARRAY_NO_REFERENCES_TO_ARRAY_OF_UNKNOWN_BOUND)
        ASSERT(0 == bslmf::IsArray<int                (&)[]>::value);
        ASSERT(0 == bslmf::IsArray<int const          (&)[]>::value);
        ASSERT(0 == bslmf::IsArray<int volatile       (&)[]>::value);
        ASSERT(0 == bslmf::IsArray<int const volatile (&)[]>::value);
        ASSERT(0 == bslmf::IsArray<int                (&)[][7]>::value);
        ASSERT(0 == bslmf::IsArray<int *const         (&)[][7]>::value);

        ASSERT(0 == bslmf::IsArray<int                (*&)[]>::value);
        ASSERT(0 == bslmf::IsArray<int const          (*&)[]>::value);
        ASSERT(0 == bslmf::IsArray<int volatile       (*&)[]>::value);
        ASSERT(0 == bslmf::IsArray<int const volatile (*&)[]>::value);
        ASSERT(0 == bslmf::IsArray<int                (*&)[][7]>::value);
        ASSERT(0 == bslmf::IsArray<int *const         (*&)[][7]>::value);
#endif // BSLMF_DECAY_NO_REFERENCES_TO_ARRAY_OF_UNKNOWN_BOUND

        ASSERT(1 == bslmf::IsArray<Enum          [8]>::VALUE);
        ASSERT(0 == bslmf::IsArray<Enum       (&)[8]>::VALUE);
        ASSERT(0 == bslmf::IsArray<Enum const (&)[8]>::VALUE);
        ASSERT(0 == bslmf::IsArray<Enum             >::VALUE);

        ASSERT(1 == bslmf::IsArray<Struct    [8]>::VALUE);
        ASSERT(0 == bslmf::IsArray<Struct (&)[8]>::VALUE);
        ASSERT(0 == bslmf::IsArray<Struct       >::VALUE);

        ASSERT(1 == bslmf::IsArray<Union    [8]>::VALUE);
        ASSERT(0 == bslmf::IsArray<Union (&)[8]>::VALUE);
        ASSERT(0 == bslmf::IsArray<Union       >::VALUE);

        ASSERT(0 == bslmf::IsArray<int  Struct::*    >::VALUE);
        ASSERT(0 == bslmf::IsArray<int (Struct::*)[9]>::VALUE);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'bsl::is_array::value'
        //   Ensure that 'bsl::is_array' returns the correct values for a
        //   variety of template parameter types.
        //
        // Concerns:
        //: 1 'is_array' returns 'true' for a bounded array type.
        //:
        //: 2 'is_array' returns 'true' for an unbounded array type.
        //:
        //: 3 'is_array' returns 'false' for a reference to an array type.
        //:
        //: 4 'is_array' returns 'false' for non-array types.
        //
        // Plan:
        //: 1 Verify that 'bsl::is_array' returns the correct value for each
        //:   concern.
        //
        // Testing:
        //   bsl::is_array::value
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'bsl::is_array::value'"
                            "\n==============================\n");

        ASSERT(1 == bsl::is_array<char          [1]>::value);
        ASSERT(1 == bsl::is_array<char const    [1]>::value);
        ASSERT(0 == bsl::is_array<char       (&)[1]>::value);
        ASSERT(0 == bsl::is_array<char             >::value);
        ASSERT(0 == bsl::is_array<char const       >::value);
        ASSERT(0 == bsl::is_array<char        *    >::value);

        ASSERT(1 == bsl::is_array<void       *   [2]>::value);
        ASSERT(1 == bsl::is_array<void const *   [2]>::value);
        ASSERT(0 == bsl::is_array<void       *(&)[2]>::value);
        ASSERT(0 == bsl::is_array<void       *      >::value);
        ASSERT(0 == bsl::is_array<void              >::value);

        ASSERT(1 == bsl::is_array<int                [3]>::value);
        ASSERT(1 == bsl::is_array<int const          [3]>::value);
        ASSERT(1 == bsl::is_array<int volatile       [3]>::value);
        ASSERT(1 == bsl::is_array<int const volatile [3]>::value);
        ASSERT(0 == bsl::is_array<int                   >::value);
        ASSERT(0 == bsl::is_array<int const             >::value);
        ASSERT(0 == bsl::is_array<int volatile          >::value);
        ASSERT(0 == bsl::is_array<int const volatile    >::value);

        ASSERT(0 == bsl::is_array<int                (&)[4]>::value);
        ASSERT(0 == bsl::is_array<int const          (&)[4]>::value);
        ASSERT(0 == bsl::is_array<int volatile       (&)[4]>::value);
        ASSERT(0 == bsl::is_array<int const volatile (&)[4]>::value);
        ASSERT(0 == bsl::is_array<int                 &    >::value);
        ASSERT(0 == bsl::is_array<int const           &    >::value);
        ASSERT(0 == bsl::is_array<int volatile        &    >::value);
        ASSERT(0 == bsl::is_array<int const volatile  &    >::value);

        ASSERT(1 == bsl::is_array<int                 * [5]>::value);
        ASSERT(1 == bsl::is_array<int const           * [5]>::value);
        ASSERT(1 == bsl::is_array<int volatile        * [5]>::value);
        ASSERT(1 == bsl::is_array<int const volatile  * [5]>::value);
        ASSERT(0 == bsl::is_array<int                (*)[5]>::value);
        ASSERT(0 == bsl::is_array<int const          (*)[5]>::value);
        ASSERT(0 == bsl::is_array<int volatile       (*)[5]>::value);
        ASSERT(0 == bsl::is_array<int const volatile (*)[5]>::value);
        ASSERT(0 == bsl::is_array<int                 *    >::value);
        ASSERT(0 == bsl::is_array<int const           *    >::value);
        ASSERT(0 == bsl::is_array<int volatile        *    >::value);
        ASSERT(0 == bsl::is_array<int const volatile  *    >::value);

        ASSERT(1 == bsl::is_array<int         [6][6]>::value);
        ASSERT(1 == bsl::is_array<int const   [6][6]>::value);
        ASSERT(0 == bsl::is_array<int      (&)[6][6]>::value);
        ASSERT(1 == bsl::is_array<int       * [6][6]>::value);
        ASSERT(0 == bsl::is_array<int      (*)[6][6]>::value);

        ASSERT(1 == bsl::is_array<int *const    [6][6]>::value);
        ASSERT(0 == bsl::is_array<int *const (&)[6][6]>::value);
        ASSERT(0 == bsl::is_array<int *const (*)[6][6]>::value);

        ASSERT(1 == bsl::is_array<void *[]>::value);

        ASSERT(1 == bsl::is_array<int                []>::value);
        ASSERT(1 == bsl::is_array<int const          []>::value);
        ASSERT(1 == bsl::is_array<int volatile       []>::value);
        ASSERT(1 == bsl::is_array<int const volatile []>::value);

        ASSERT(1 == bsl::is_array<int                 * []>::value);
        ASSERT(1 == bsl::is_array<int const           * []>::value);
        ASSERT(1 == bsl::is_array<int volatile        * []>::value);
        ASSERT(1 == bsl::is_array<int const volatile  * []>::value);
        ASSERT(0 == bsl::is_array<int                (*)[]>::value);
        ASSERT(0 == bsl::is_array<int const          (*)[]>::value);
        ASSERT(0 == bsl::is_array<int volatile       (*)[]>::value);
        ASSERT(0 == bsl::is_array<int const volatile (*)[]>::value);

        ASSERT(1 == bsl::is_array<int          [][7]>::value);
        ASSERT(1 == bsl::is_array<int const    [][7]>::value);
        ASSERT(1 == bsl::is_array<int        * [][7]>::value);
        ASSERT(0 == bsl::is_array<int       (*)[][7]>::value);

        ASSERT(1 == bsl::is_array<int *const    [][7]>::value);
        ASSERT(0 == bsl::is_array<int *const (*)[][7]>::value);

#if !defined(BSLMF_ISARRAY_NO_REFERENCES_TO_ARRAY_OF_UNKNOWN_BOUND)
        ASSERT(0 == bsl::is_array<int                (&)[]>::value);
        ASSERT(0 == bsl::is_array<int const          (&)[]>::value);
        ASSERT(0 == bsl::is_array<int volatile       (&)[]>::value);
        ASSERT(0 == bsl::is_array<int const volatile (&)[]>::value);
        ASSERT(0 == bsl::is_array<int                (&)[][7]>::value);
        ASSERT(0 == bsl::is_array<int *const         (&)[][7]>::value);

        ASSERT(0 == bsl::is_array<int                (*&)[]>::value);
        ASSERT(0 == bsl::is_array<int const          (*&)[]>::value);
        ASSERT(0 == bsl::is_array<int volatile       (*&)[]>::value);
        ASSERT(0 == bsl::is_array<int const volatile (*&)[]>::value);
        ASSERT(0 == bsl::is_array<int                (*&)[][7]>::value);
        ASSERT(0 == bsl::is_array<int *const         (*&)[][7]>::value);
#endif // BSLMF_DECAY_NO_REFERENCES_TO_ARRAY_OF_UNKNOWN_BOUND

        ASSERT(1 == bsl::is_array<Enum          [8]>::value);
        ASSERT(0 == bsl::is_array<Enum       (&)[8]>::value);
        ASSERT(0 == bsl::is_array<Enum const (&)[8]>::value);
        ASSERT(0 == bsl::is_array<Enum             >::value);

        ASSERT(1 == bsl::is_array<Struct    [8]>::value);
        ASSERT(0 == bsl::is_array<Struct (&)[8]>::value);
        ASSERT(0 == bsl::is_array<Struct       >::value);

        ASSERT(1 == bsl::is_array<Union    [8]>::value);
        ASSERT(0 == bsl::is_array<Union (&)[8]>::value);
        ASSERT(0 == bsl::is_array<Union       >::value);

        ASSERT(0 == bsl::is_array<int  Struct::*    >::value);
        ASSERT(0 == bsl::is_array<int (Struct::*)[9]>::value);
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
