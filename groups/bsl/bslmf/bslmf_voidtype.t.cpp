// bslmf_voidtype.t.cpp                                               -*-C++-*-
#include <bslmf_voidtype.h>

#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// The metafunction defined in this component does no actual calculation; it
// simply provides a 'void' type member.  Testing it is trivial: simply
// instantiate it with 0 to 14 parameters and verify that it compiles.  The
// usefulness of this component is demonstrated in the usage examples.
//-----------------------------------------------------------------------------
// [1] FULL TEST
// [2] USAGE EXAMPLE
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

struct MyStruct {
    int d_x;
};

enum MyEnum { E0, E1 };

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage Example 1
///- - - - - - - -
// In this example, we demonstrate the use of 'VoidType' to determine whether a
// given type 'T' has a member type 'T::iterator'.  Our goal is to create a
// metafunction, 'HasIteratorType', such that 'HasIteratorType<T>::VALUE' is
// 'true' if 'T::iterator' is a valid type and 'false' otherwise.  This example
// is adapted from the paper proposing 'std::void_t' for the C++ Standard,
// N3911.
//
// First, we define the base-case metafunction that returns 'false':
//..
    template <class TYPE, class = void>
    struct HasIteratorType {
        enum { VALUE = false };
    };
//..
// Now we create a partial specialization that uses 'VoidType' to probe for
// 'T::iterator'.
//..
    template <class TYPE>
    struct HasIteratorType<
                     TYPE,
                     typename bslmf::VoidType<typename TYPE::iterator>::type> {
        enum { VALUE = true };
    };
//..
// To see how this works, we define a class that has a 'iterator' member and
// apply 'HasIteratorType' to it:
//..
    struct WithIterator {
        typedef short *iterator;
    };

    int usageExample1()
    {
        ASSERT(true == HasIteratorType<WithIterator>::VALUE);
//..
// Since 'WithIterator::iterator' is a valid type,
// 'VoidType<WithIterator::iterator>::type' will be 'void' and the second
// 'HasIteratorType' template will be more specialized than the primary
// template and will thus get instantiated, yielding a 'VALUE' of 'true'.
//
// Conversely, if we try to instantiate 'HasIteratorType<int>', any use of
// 'VoidType<int::iterator>::type' will result in a substitution failure.
// Fortunately, the Substitution Failure Is Not An Error (SFINAE) rule applies,
// so the code will compile, but the specialization is eliminated from
// consideration, resulting in the primary template being instantiated and
// yielding a 'VALUE' of 'false':
//..
        ASSERT(false == HasIteratorType<int>::VALUE);

        return 0;
    }
//..
//
///Usage Example 2
///- - - - - - - -
// This example demonstrates the use of 'VoidType' to probe for more than one
// type at once.  As in the previous example, we are defining a metafunction.
// We'll define 'IsTraversable<T>::VALUE' to be 'true' if 'T::iterator' and
// 'T::value_type' both exist.  As before, we start with a primary template
// that always yields 'false':
//..
    template <class TYPE, class = void>
    struct IsTraversable {
        enum { VALUE = false };
    };
//..
// This time, we create a partial specialization that uses 'VoidType' with two
// parameters:
//..
    template <class TYPE>
    struct IsTraversable<TYPE,
                         typename bslmf::VoidType<typename TYPE::iterator,
                                                  typename TYPE::value_type
                                                 >::type> {
        enum { VALUE = true };
    };
//..
// Next, we define a type that meets the requirement for being traversable:
//..
    struct MyTraversable {
        typedef int  value_type;
        typedef int *iterator;
    };
//..
// The 'IsTraversable' metafunction yields 'true' for 'Traversable' but not for
// either 'WithIterator', which lacks 'value_type', or 'int', which lacks both
// 'iterator' and 'value_type':
//..
    int usageExample2()
    {
        ASSERT(true  == IsTraversable<MyTraversable>::VALUE);
        ASSERT(false == IsTraversable<WithIterator>::VALUE);
        ASSERT(false == IsTraversable<int>::VALUE);

        return 0;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usageExample1();
        usageExample2();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // FULL TEST
        //
        // Concerns:
        //: 1 'bslmf::VoidType<T1, T2... TN>::type' is 'void' for N from 0 to
        //:   14.
        //:
        //: 2 The types used to instantiate 'bslmf::VoidType' can be any mix of
        //:   fundamental types, pointers, enumerations, classes, references,
        //:   or 'void'.
        //
        // Plan:
        //: 1 For concern 1, instantiate 'bslmf::VoidType<T1, T2... TN>::type'
        //:   with 0 to 14 parameters and verify that the result is type
        //:   'void'.  (C-1)
        //:
        //: 2 For concern 2, ensure that P-1 has a healthy mix of parameter
        //:   types.  (C-2)
        //
        // Testing:
        //   FULL TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nFULL TEST"
                            "\n=========\n");

#define TEST(...) \
        ASSERT((bsl::is_same<void, bslmf::VoidType<__VA_ARGS__>::type>::value))

        TEST();
        TEST(int);
        TEST(int,short*);
        TEST(int,short*,MyEnum);
        TEST(int,short*,MyEnum,MyStruct);
        TEST(int,short*,MyEnum,MyStruct,MyEnum&);
        TEST(int,short*,MyEnum,MyStruct,MyEnum&,MyStruct const&);
        TEST(int,short*,MyEnum,MyStruct,MyEnum&,MyStruct const&,void);
        TEST(int,short*,MyEnum,MyStruct,MyEnum&,MyStruct const&,void,
             const void);
        TEST(int,short*,MyEnum,MyStruct,MyEnum&,MyStruct const&,void,
             const void,volatile int);
        TEST(int,short*,MyEnum,MyStruct,MyEnum&,MyStruct const&,void,
             const void,volatile int,volatile char *const);
        TEST(int,short*,MyEnum,MyStruct,MyEnum&,MyStruct const&,void,
             const void,volatile int,volatile char *const,double);
        TEST(int,short*,MyEnum,MyStruct,MyEnum&,MyStruct const&,void,
             const void,volatile int,volatile char *const,double,
             unsigned);
        TEST(int,short*,MyEnum,MyStruct,MyEnum&,MyStruct const&,void,
             const void,volatile int,volatile char *const,double,
             unsigned,MyEnum *const*);
        TEST(int,short*,MyEnum,MyStruct,MyEnum&,MyStruct const&,void,
             const void,volatile int,volatile char *const,double,
             unsigned(&)(),MyEnum *const*,int MyStruct::*);

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
