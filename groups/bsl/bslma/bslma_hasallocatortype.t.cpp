// bslma_hasallocatortype.t.cpp                                       -*-C++-*-

#include <bslma_hasallocatortype.h>

#include <bslma_isstdallocator.h>

#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addvolatile.h>
#include <bslmf_issame.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <cstdio>   // 'printf'
#include <cstdlib>  // 'atoi'

#ifdef BDE_VERIFY
// Suppress some pedantic bde_verify checks in this test driver
#pragma bde_verify -FD01   // Function declaration requires contract
#pragma bde_verify -FD03   // Parameter not documented in function contract
#pragma bde_verify -FABC01 // Function not in alphabetical order
#pragma bde_verify -TP19   // Missing or malformed standard test driver section
#endif

using std::printf;
using std::fprintf;
using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
//
// ----------------------------------------------------------------------------
// [ 1] FULL TEST
// [ 2] USAGE EXAMPLES
// ----------------------------------------------------------------------------

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

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                                VERBOSITY
// ----------------------------------------------------------------------------

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;
int veryVeryVeryVerbose = 0; // For test allocators

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

template <class TYPE>
struct MinimalAlloc {
    // Minimal C++11 allocator class template.  Member functions are never
    // called and are thus not defined.
    BSLMF_NESTED_TRAIT_DECLARATION(MinimalAlloc, bslma::IsStdAllocator);

    typedef TYPE value_type;

    MinimalAlloc();
    template<class T> MinimalAlloc(const MinimalAlloc<T>& other);

    TYPE* allocate(std::size_t n);
    void deallocate(TYPE* p, std::size_t n);
};

struct NoAllocatorType {
    // Class type having no 'allocator_type' member.
};

struct VoidAllocatorType {
    // Class type having an 'allocator_type' member of 'void' type, which does
    // not meet the C++11 allocator requirements.

    typedef void allocator_type;
};

struct InvalidAllocatorType {
    // Class type having an 'allocator_type' member of 'struct' type that does
    // not meet the C++11 allocator requirements.

    struct allocator_type {
        typedef int value_type;

        // Does not have an 'allocate' method, so does not meet the minimal
        // requirements tested by this component.
    };
};

struct WithAllocatorType {
    // Class type having an 'allocator_type' member that meets the C++11
    // allocator requirements.

    typedef MinimalAlloc<short> allocator_type;
};

// ============================================================================
//                             USAGE EXAMPLES
// ----------------------------------------------------------------------------

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Conditional 'allocator_type'
///- - - - - - - - - - - - - - - - - - - -
// In this example, we create a wrapper class, 'Wrapper', that defines a nested
// 'allocator_type' if and only if the type it wraps has an 'allocator_type'.
// First, we forward-declare a base class template, 'Wrapper_CondAllocType',
// parameterized on whether or the wrapped class is allocator-aware (AA).
//..
    template <class TYPE, bool IS_AA> struct Wrapper_CondAllocType;
//..
// Next, we specialize the base class for the non-AA ('false') case, providing
// no 'allocator_type' nested member:
//..
    template <class TYPE>
    struct Wrapper_CondAllocType<TYPE, false> {
    };
//..
// Then, we specialize it for the AA ('true') case, using the 'allocator_type'
// from the wrapped type:
//..
    template <class TYPE>
    struct Wrapper_CondAllocType<TYPE, true> {
        typedef typename TYPE::allocator_type allocator_type;
    };
//..
// Next, we define our wrapper class to inherit the correct specialization of
// 'Wrapper_CondAllocType' by specifying the result of 'HasAllocatorType' for
// the 'IS_AA' argument:
//..
    template <class TYPE>
    class Wrapper
        : public Wrapper_CondAllocType<TYPE,
                                       bslma::HasAllocatorType<TYPE>::value> {
        // ...
    };
//..
// Now, to test our work, we define a minimal allocator type, 'MyAlloc':
//..
    template <class TYPE>
    struct MyAlloc {
        BSLMF_NESTED_TRAIT_DECLARATION(MyAlloc, bslma::IsStdAllocator);

        typedef TYPE value_type;

        MyAlloc();
        template<class T> MyAlloc(const MyAlloc<T>& other);

        TYPE* allocate(std::size_t n);
        void deallocate(TYPE* p, std::size_t n);
    };
//..
// Next, we define a class type, 'AAType', that uses 'MyAlloc':
//..
    struct AAType {
        typedef MyAlloc<int> allocator_type;
        //..
    };
//..
// Finally, we can verify that any instantiation of 'Wrapper' on 'AAType' is
// itself AA, whereas instantiations on, e.g., 'int' or pointer types are not:
//..
    void usageExample1()
    {
        ASSERT(bslma::HasAllocatorType<        AAType  >::value);
        ASSERT(bslma::HasAllocatorType<Wrapper<AAType> >::value);
        ASSERT((bsl::is_same<Wrapper<AAType>::allocator_type,
                             MyAlloc<int> >::value));

        ASSERT(! bslma::HasAllocatorType<        int     >::value);
        ASSERT(! bslma::HasAllocatorType<Wrapper<int   > >::value);

        ASSERT(! bslma::HasAllocatorType<        char *  >::value);
        ASSERT(! bslma::HasAllocatorType<Wrapper<char *> >::value);
    }
//..

}  // close unnamed namespace

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concerns:
        //: 1 That the usage examples shown in the component-level
        //:   documentation compile and run as described.
        //
        // Plan:
        //: 1 Copy the usage examples from the component header, changing
        //    'assert' to 'ASSERT' and execute them.
        //
        // Testing:
        //     USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLES"
                            "\n==============\n");

        usageExample1();

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // FULL TEST
        //   This case tests the entire functionality of this component.  In
        //   the description below, "the metafunction" refers to an
        //   instantiation of "bslma::HasAllocatorType<TYPE>", for some
        //   'TYPE'.  The value "returned" by the metafunction refers to the
        //   'value' member constant of the instantiation.
        //
        // Concerns:
        //: 1 The metafunction returns 'true' if 'TYPE' is a class having a
        //:   public member type, 'allocator_type', where
        //:   'TYPE::allocator_type' meets the requirements of a C++11
        //:   allocator class.
        //: 2 The metafunction returns 'false' if TYPE is a class that does not
        //:   have an 'allocator_type' member or where 'TYPE::allocator_type'
        //:   does not have a 'value_type' member.
        //: 3 The metafunction returns 'false' if TYPE is a scalar, pointer,
        //:   reference, or 'void' type.
        //: 4 The cv-qualifiers on 'TYPE' have no affect on the result.
        //
        // Plan:
        //: 1 Verify the results of invoking the member function for a
        //:   representative sample of the types described in concerns 1-3.
        //:   (C-1, C-2, C-3)
        //: 2 As an orthogonal perturbation on step one, try each type with
        //:   'const', 'volatile', and 'const volatile' qualifiers.  (C-4)
        //
        // Testing:
        //      FULL TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nFULL TEST"
                            "\n=========\n");

#define TEST(TP, EXP) do {                                                  \
        typedef TP                          Tp;                             \
        typedef bsl::add_const<Tp>::type    ConstTp;                        \
        typedef bsl::add_volatile<Tp>::type VolatileTp;                     \
        typedef bsl::add_cv<Tp>::type       ConstVolatileTp;                \
        ASSERT((EXP) == bslma::HasAllocatorType<             Tp>::value);   \
        ASSERT((EXP) == bslma::HasAllocatorType<        ConstTp>::value);   \
        ASSERT((EXP) == bslma::HasAllocatorType<     VolatileTp>::value);   \
        ASSERT((EXP) == bslma::HasAllocatorType<ConstVolatileTp>::value);   \
    } while (false)

        //   Type                   Exp
        //   ---------------------  -----
        TEST(int                  , false);
        TEST(void *               , false);
        TEST(               char *, false);
        TEST(const          char *, false);
        TEST(      volatile char *, false);
        TEST(const volatile char *, false);
        TEST(               char& , false);
        TEST(const          char& , false);
        TEST(      volatile char& , false);
        TEST(const volatile char& , false);
        TEST(NoAllocatorType      , false);
        TEST(VoidAllocatorType    , false);
        TEST(InvalidAllocatorType , false );
        TEST(WithAllocatorType    , true );

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
// Copyright 2022 Bloomberg Finance L.P.
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
