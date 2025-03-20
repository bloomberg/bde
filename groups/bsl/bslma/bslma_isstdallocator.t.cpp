// bslma_isstdallocator.t.cpp                                         -*-C++-*-

#include <bslma_isstdallocator.h>

#include <bsls_bsltestutil.h>

#include <cassert>     // `assert`
#include <cstdio>      // `std::printf` and `std::fprintf`
#include <cstdlib>     // `atoi`
#include <memory>      // `std::allocator`

// It would be tempting to use some higher-level components in testing.  These
// checks prevent inadvertantly modifying this test driver so as to create a
// dependency inversion or (worse) a cyclic dependancy.
#ifdef INCLUDED_BSLMA_BSLALLOCATOR
# error This component should not depend on 'bslma_bslallocator'
#endif

#ifdef INCLUDED_BSLMA_ALLOCATORTRAITS
# error This component should not depend on 'bslma_allocatortraits'
#endif

using namespace BloombergLP;
using std::printf;
using std::fprintf;
using std::fflush;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function, `bslma::IsStdAllocator`
// and a template variable `bslma::IsStdAllocator_v`, that determine whether a
// template parameter type meets the requirements for an allocator, as
// specified (loosely) in [container.requirements.general].  Thus, we need to
// ensure that the values returned by the meta-function are correct for each
// possible category of types.

//
//-----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bslma::IsStdAllocator<TYPE>::value
// [ 1] bslma::IsStdAllocator_v<TYPE>
// ----------------------------------------------------------------------------
// [-1] NEGATIVE COMPILATION TESTS
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
        fflush(stdout);

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
//                      WARNING SUPPRESSION
//-----------------------------------------------------------------------------

// This test driver intentionally creates types with unusual uses of
// cv-qualifiers in order to confirm that there are no strange corners of the
// type system that are not addressed by this traits component.  Consequently,
// we disable certain warnings from common compilers.

#if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
# pragma GCC diagnostic ignored "-Wignored-qualifiers"
#elif defined(BSLS_PLATFORM_CMP_MSVC)
# pragma warning(disable : 4180)
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

enum EnumTestType {
    // This user-defined `enum` type is intended to be used for testing as the
    // template parameter `TYPE` of `bslma::IsStdAllocator`.
};

/// This user-defined `struct` type is intended to be used for testing as
/// the template parameter `TYPE` of `bslma::IsStdAllocator`.
struct StructTestType {
};

/// This user-defined `union` type is intended to be used for testing as the
/// template parameter `TYPE` of `bslma::IsStdAllocator`.
union UnionTestType {
};

/// This user-defined base class type is intended to be used for testing as
/// the template parameter `TYPE` of `bslma::IsStdAllocator`.
class BaseClassTestType {
};

/// This user-defined derived class type is intended to be used for testing
/// as the template parameter `TYPE` of `bslma::IsStdAllocator`.
class DerivedClassTestType : public BaseClassTestType {
};

/// This pointer to non-static member function type is intended to be used
/// for testing as the template parameter `TYPE` of `bslma::IsStdAllocator`.
typedef int (StructTestType::*MethodPtrTestType) ();

/// This function pointer type is intended to be used for testing as the
/// template parameter `TYPE` of `bslma::IsStdAllocator`.
typedef void (*FunctionPtrTestType) ();

/// This pointer to member object type is intended to be used for testing as
/// the template parameter `TYPE` of `bslma::IsStdAllocator`.
typedef int StructTestType::*PMD;

struct Incomplete;
    // This incomplete `struct` type is intended to be used for testing as the
    // template parameter `TYPE` of `bslma::IsStdAllocator`.

/// A class that meets the allocator requirements.  The `allocate`
/// method is callable with one parameters of type `size_t`.
struct AllocatorOneParam {
    BSLMF_NESTED_TRAIT_DECLARATION(AllocatorOneParam, bslma::IsStdAllocator);
    typedef int value_type;

    /// a function signature that matches the allocator requirements.
    int *allocate(size_t);
};

/// A class that meets the allocator requirements.  The `allocate`
/// method is callable with one or two parameters.
struct AllocatorTwoParams {
    BSLMF_NESTED_TRAIT_DECLARATION(AllocatorTwoParams, bslma::IsStdAllocator);
    typedef int value_type;

    /// a function signature that matches the allocator requirements.
    int *allocate(size_t, const void * = 0);
};

/// A class that meets the allocator requirements.  The `allocate`
/// method is callable with one or two parameters.
struct AllocatorOverloadedAlloc {
    BSLMF_NESTED_TRAIT_DECLARATION(AllocatorOverloadedAlloc,
                                   bslma::IsStdAllocator);
    typedef int value_type;

    /// overloaded function signatures that match the allocator
    /// requirements.
    int *allocate(size_t);
    int *allocate(size_t, void *);
};

/// A class that meets the allocator requirements through inheritance.
struct AllocatorInherited : AllocatorOneParam {
    BSLMF_NESTED_TRAIT_DECLARATION(AllocatorInherited, bslma::IsStdAllocator);
};

/// A class that does not meet the allocator requirements.  No `allocate`.
struct NoAllocateMember {
    typedef int value_type;
  private:
    int *allocate(std::size_t);
};

/// A class that does not meet the allocator requirements.  No `value_type`.
struct NoValueType {
    int *allocate(size_t);
};

/// A class that does not meet the allocator requirements.  The `allocate`
/// method is not callable with a single `size_t` parameter.
struct NonAllocatorTwoParams {
    typedef int value_type;

    /// a function signature that does not match the allocator requirements.
    int *allocate(size_t, const void *);
};

/// A class that does not meet the allocator requirements.  The `allocate`
/// method is not callable with a single `size_t` parameter.
struct NonAllocatorBadParam {
    typedef int value_type;

    /// a function signature that does not match the allocator requirements.
    int *allocate(const StructTestType &);
};

/// FOR NEGATIVE COMPILATION TESTS.  A class that meets the allocator
/// requirements but which does not explicitly declare the `IsStdAllocator`
/// trait.
struct NegativeNotDeclared : AllocatorOneParam {
};

/// FOR NEGATIVE COMPILATION TESTS.  A class that does not meet the
/// allocator requirements (missing `value_type`) but nevertheless declares
/// the `IsStdAllocator` nested trait.
struct NegativeMissingValueType {

    BSLMF_NESTED_TRAIT_DECLARATION(NegativeMissingValueType,
                                   bslma::IsStdAllocator);
    int *allocate(std::size_t);
};

/// FOR NEGATIVE COMPILATION TESTS.  A class that does not meet the
/// allocator requirements (missing `allocate(size_type)` but nevertheless
/// declares the `IsStdAllocator` nested trait.
struct NegativeMissingAllocate {

    BSLMF_NESTED_TRAIT_DECLARATION(NegativeMissingAllocate,
                                   bslma::IsStdAllocator);

    typedef int value_type;

    /// Function signature that does not match the allocator requirements.
    int *allocate(const StructTestType &);
};

/// FOR NEGATIVE COMPILATION TESTS.  A class that does not meet the
/// allocator requirements (missing `value_type` and `allocate(size_type)`
/// but nevertheless declares the `IsStdAllocator` nested trait.
struct NegativeMissingBoth {

    BSLMF_NESTED_TRAIT_DECLARATION(NegativeMissingBoth, bslma::IsStdAllocator);
};

}  // close unnamed namespace

namespace BloombergLP {
namespace bslma {

/// Specialize trait under test when auto-detection would fail.
template <> struct IsStdAllocator<Incomplete> : bsl::false_type { };

}  // close package namespace
}  // close enterprise namespace

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
/// `ASSERT` that `IsStdAllocator_v` has the same value as
/// `IsStdAllocator::value`.
#   define TYPE_ASSERT_V_SAME(type)                                           \
       ASSERT(bslma::IsStdAllocator<type>::value ==                           \
              bslma::IsStdAllocator_v<type>)
#else
#   define TYPE_ASSERT_V_SAME(type)
#endif

/// Apply the specified `META_FUNC` to the specified `TYPE` and confirm that
/// the result value and the specified `result` are the same.  Also confirm
/// that the result value of the `META_FUNC` and the value of the
/// `META_FUNC_v` variable are the same.
#define TYPE_ASSERT(META_FUNC, TYPE, result)                                  \
    ASSERT(result == META_FUNC<TYPE>::value);                                 \
    TYPE_ASSERT_V_SAME(TYPE);                                                 \

//=============================================================================
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

// This section shows the intended use of this component.
//
///Example 1: Verify if a class meets the requirements for an allocator.
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types meet the requirements
// for allocators.
//
// First, we create a struct type `MyAllocator`:
// ```
    struct MyAllocator
    {
        BSLMF_NESTED_TRAIT_DECLARATION(MyAllocator, bslma::IsStdAllocator);
        typedef int value_type;

        /// Allocate some memory for use by the caller.
        int *allocate(size_t);
    };
// ```
// Now, we instantiate the `bslma::IsStdAllocator` template for both a type
// that does not meet the allocator requirements and the defined type
// `MyClass`, that does, asserting the `value` static data member of each
// instantiation.
// ```
    void usageExample()
    {
        ASSERT(false == bslma::IsStdAllocator<int>::value);
        ASSERT(true  == bslma::IsStdAllocator<MyAllocator>::value);
// ```
// Note that if the current compiler supports the variable the templates C++14
// feature then we can re-write the snippet of code above using the
// `bslma::IsStdAllocator_v` variable as follows:
// ```
    #ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
        ASSERT(false == bslma::IsStdAllocator_v<int>);
        ASSERT(true  == bslma::IsStdAllocator_v<MyAllocator>);
    #endif
    }
// ```

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? std::atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    std::setbuf(stdout, NULL);       // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

        usageExample();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // `bslma::IsStdAllocator::value`
        //   Ensure that the static data member `value` of
        //   `bslma::IsStdAllocator` instantiations having various
        //   (template parameter) `TYPE`s has the correct value.
        //
        // Concerns:
        // 1. `IsStdAllocator::value` is `false` when `TYPE` is a primitive
        //    type.
        //
        // 2. `IsStdAllocator::value` is `false` when `TYPE` is an `enum` type.
        //
        // 3. `IsStdAllocator::value` is `false` when `TYPE` is a `class`,
        //    `struct`, or `union` type.
        //
        // 4. `IsStdAllocator::value` is `false` when `TYPE` is a pointer or
        //    pointer-to-member type.
        //
        // 5. `IsStdAllocator::value` is `false` when `TYPE` is a function
        //    type.
        //
        // 6. `IsStdAllocator::value` is `true` when `TYPE` is a  type that
        //    meets the requirements of an allocator.
        //
        // 7. `IsStdAllocator::value` is `false` when `TYPE` is a type that
        //    does not meet the requirements of an allocator.
        //
        // 8. `IsStdAllocator::value` is `false` when `TYPE` is a reference
        //    type, even if the type it refers to meets the requirements of an
        //    allocator.
        //
        // 9. That `IsStdAllocator_v` equals to `IsStdAllocator::value` for
        //    a variety of template parameter types.
        //
        // Plan:
        // 1. Verify that `bslma::IsStdAllocator::value` has the correct value
        //    for each (template parameter) `TYPE` in the concerns.
        //
        // Testing:
        //   bslma::IsStdAllocator<TYPE>::value
        //   bslma::IsStdAllocator_v<TYPE>
        // --------------------------------------------------------------------

        if (verbose) printf("`bslma::IsStdAllocator::value`\n"
                            "==============================\n");

        // C-1,9
        TYPE_ASSERT(bslma::IsStdAllocator, void,  false);
        TYPE_ASSERT(bslma::IsStdAllocator, int,   false);
        TYPE_ASSERT(bslma::IsStdAllocator, int,   false);

        // C-2,9
        TYPE_ASSERT(bslma::IsStdAllocator, EnumTestType,  false);

        // C-3,8,9
        TYPE_ASSERT(bslma::IsStdAllocator, StructTestType,        false);
        TYPE_ASSERT(bslma::IsStdAllocator, StructTestType&,       false);
        TYPE_ASSERT(bslma::IsStdAllocator, UnionTestType,         false);
        TYPE_ASSERT(bslma::IsStdAllocator, UnionTestType&,        false);
        TYPE_ASSERT(bslma::IsStdAllocator, Incomplete,            false);
        TYPE_ASSERT(bslma::IsStdAllocator, Incomplete&,           false);
        TYPE_ASSERT(bslma::IsStdAllocator, BaseClassTestType,     false);
        TYPE_ASSERT(bslma::IsStdAllocator, BaseClassTestType&,    false);
        TYPE_ASSERT(bslma::IsStdAllocator, DerivedClassTestType,  false);
        TYPE_ASSERT(bslma::IsStdAllocator, DerivedClassTestType&, false);

        // C-4,9
        TYPE_ASSERT(bslma::IsStdAllocator, int*,                       false);
        TYPE_ASSERT(bslma::IsStdAllocator, StructTestType*,            false);
        TYPE_ASSERT(bslma::IsStdAllocator, int StructTestType::*,      false);
        TYPE_ASSERT(bslma::IsStdAllocator, int StructTestType::* *,    false);
        TYPE_ASSERT(bslma::IsStdAllocator, UnionTestType*,             false);
        TYPE_ASSERT(bslma::IsStdAllocator, PMD BaseClassTestType::*,   false);
        TYPE_ASSERT(bslma::IsStdAllocator, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT(bslma::IsStdAllocator, BaseClassTestType*,         false);
        TYPE_ASSERT(bslma::IsStdAllocator, DerivedClassTestType*,      false);
        TYPE_ASSERT(bslma::IsStdAllocator, Incomplete*,                false);
        TYPE_ASSERT(bslma::IsStdAllocator, MethodPtrTestType,          false);
        TYPE_ASSERT(bslma::IsStdAllocator, FunctionPtrTestType,        false);

        // C-8
        TYPE_ASSERT(bslma::IsStdAllocator, int*&,                       false);
        TYPE_ASSERT(bslma::IsStdAllocator, StructTestType*&,            false);
        TYPE_ASSERT(bslma::IsStdAllocator, int StructTestType::*&,      false);
        TYPE_ASSERT(bslma::IsStdAllocator, int StructTestType::* *&,    false);
        TYPE_ASSERT(bslma::IsStdAllocator, UnionTestType*&,             false);
        TYPE_ASSERT(bslma::IsStdAllocator, PMD BaseClassTestType::*&,   false);
        TYPE_ASSERT(bslma::IsStdAllocator, PMD BaseClassTestType::* *&, false);
        TYPE_ASSERT(bslma::IsStdAllocator, BaseClassTestType*&,         false);
        TYPE_ASSERT(bslma::IsStdAllocator, DerivedClassTestType*&,      false);
        TYPE_ASSERT(bslma::IsStdAllocator, Incomplete*&,                false);
        TYPE_ASSERT(bslma::IsStdAllocator, MethodPtrTestType&,          false);
        TYPE_ASSERT(bslma::IsStdAllocator, FunctionPtrTestType&,        false);

        // C-5,9
        TYPE_ASSERT(bslma::IsStdAllocator, int  (int),   false);
        TYPE_ASSERT(bslma::IsStdAllocator, void (void),  false);
        TYPE_ASSERT(bslma::IsStdAllocator, int  (void),  false);
        TYPE_ASSERT(bslma::IsStdAllocator, void (int),   false);

        // C-6,9
        TYPE_ASSERT(bslma::IsStdAllocator, MyAllocator,                 true);
        TYPE_ASSERT(bslma::IsStdAllocator, AllocatorOneParam,           true);
        TYPE_ASSERT(bslma::IsStdAllocator, AllocatorTwoParams,          true);
        TYPE_ASSERT(bslma::IsStdAllocator, AllocatorOverloadedAlloc,    true);
        TYPE_ASSERT(bslma::IsStdAllocator, AllocatorInherited,          true);
        TYPE_ASSERT(bslma::IsStdAllocator, std::allocator<int>,         true);
        TYPE_ASSERT(bslma::IsStdAllocator, std::allocator<Incomplete>,  true);

        // C-7,9
        TYPE_ASSERT(bslma::IsStdAllocator, NoAllocateMember,            false);
        TYPE_ASSERT(bslma::IsStdAllocator, NoValueType,                 false);
        TYPE_ASSERT(bslma::IsStdAllocator, NonAllocatorTwoParams,       false);
        TYPE_ASSERT(bslma::IsStdAllocator, NonAllocatorBadParam,        false);

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // NEGATIVE TESTS
        //   These tests (disabled by default) should result in compilation
        //   errors when enabled.
        //
        // Concerns:
        // 1. For type declaring the `bslma::IsStdAllocator` nested trait,
        //    `bslma::IsStdAllocator` will result in a compile-time error if
        //    the type is missing `value_type`, `allocate(size_type)`, or both.
        //
        // 2. In C++11 and later, for type that appears to be an allocator
        //    (i.e., it has `value_type` and `allocate(size_type)`),
        //    `bslma::IsStdAllocator` will result in a compile-time error if
        //    the type does not declare the `bslma::IsStdAllocator` nested
        //    trait.
        //
        // Plan:
        // 1. Instantiate `bslma::IsStdAllocator` with classes that declare the
        //    `bslma::IsStdAllocator` nested trait but are missing
        //    `value_type`, `allocate(size_type)`, or both.  Verify that a
        //    compilation error results.  (C-1)
        //
        // 2. Instantiate `bslma::IsStdAllocator` with a class that meets the
        //    C++11 allocator requirements but does not delcare the
        //    `bslma::IsStdAllocator` nested trait.  Verify that a compilation
        //    error results.  (C-2)
        //
        // Testing
        //    NEGATIVE COMPILATION TESTS
        // --------------------------------------------------------------------

        if (verbose) printf("NEGATIVE COMPILATION TESTS"
                            "==========================\n");

#if 0
        // C-1
        bslma::IsStdAllocator<NegativeMissingValueType>::type v1;
        (void) v1;

        bslma::IsStdAllocator<NegativeMissingAllocate>::type  v2;
        (void) v2;

        bslma::IsStdAllocator<NegativeMissingBoth>::type      v3;
        (void) v3;
#endif

#if 0
# ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
        // C-2
        bslma::IsStdAllocator<NegativeNotDeclared>::type      v4;

        (void) v4;
# endif
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
// Copyright 2021 Bloomberg Finance L.P.
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
