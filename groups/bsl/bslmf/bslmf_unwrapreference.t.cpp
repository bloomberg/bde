// bslmf_unwrapreference.t.cpp                                        -*-C++-*-
#include <bslmf_unwrapreference.h>

#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
#include <type_traits>  // `std::unwrap_reference`, `std::unwrap_reference_t`
#endif

#include <stdio.h>   // `printf`
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;

#if defined(BSLS_PLATFORM_CMP_SUN)
  #pragma error_messages(off, reftoref)
#endif

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function `bsl::unwrap_reference`
// that unwraps a reference wrapper specialization `bsl::reference_wrapper<U>`
// (which is an alias to `std::reference_wrapper<U>` when that exists) template
// `t_TYPE` argument by providing a `type` member with the type `U&`.  If the
// specified `t_TYPE` is not a specialization of either reference wrapper the
// member `type` will be `t_TYPE`.
//
// The component also defines an alias to the result type of the
// `bsl::unwrap_reference` meta-function.  Thus, we need to ensure that the
// value returned by the meta-function is correct for each possible category of
// types.
//
// `bsl::unwrap_reference` and `bsl::unwrap_reference_t` should be aliased to
// `std::unwrap_reference` and `std::unwrap_reference_t`, respectively when the
// standard meta functions are available from the native library.
//
// ----------------------------------------------------------------------------
// [ 1] bsl::unwrap_reference::type
// [ 1] bsl::unwrap_reference_t
//
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 2] CONCERN: Aliased to standard types when available.

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
//                            TYPES FOR TESTING
//-----------------------------------------------------------------------------

struct Struct {};
class  Class  {};
enum   Enum   {};
union  Union  {};

template <template <class> class> class X;

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

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 3: {
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

        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");

///Example 1: Unwrap Reference Wrapped Types
///- - - - - - - - - - - - - - - - - - - - -
// Suppose that we work in a programming environment where types may be
// presented wrapped in a `bsl::reference_wrapper` or its `std` equivalent.  We
// would like to use a reference to the wrapped type, but use unwrapped types
// as they are.  This is the exact use case for `bsl::unwrap_reference`.
//
// First, we create types that represent both reference-wrapped, and normal
// type parameters:
// ```
    typedef bsl::reference_wrapper<int *>  WrappedType;
    typedef int                           *NotWrappedType;
// ```
// Next, we create types that are references if they were wrapped:
// ```
    typedef bsl::unwrap_reference<WrappedType>::type    UnwrappedWrapped;
    typedef bsl::unwrap_reference<NotWrappedType>::type UnwrappedNotWrapped;
// ```
// Finally we can verify that the wrapped type became a reference, while the
// other type is unchanged:
// ```
    ASSERT((true == bsl::is_same<UnwrappedWrapped,    int *&>::value));
    ASSERT((true == bsl::is_same<UnwrappedNotWrapped, int *>::value));
// ```
// Note, that (when available) the `bsl::unwrap_reference_t` avoids the
// `::type` suffix and `typename` prefix when we want to use the result of the
// `bsl::unwrap_reference` meta-function in templates.
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // ALIASED TO STANDARD META FUNCTIONS
        //
        // Concerns:
        // 1. The meta functions `bsl::unwrap_reference` and
        //    `bsl::unwrap_reference_t` should be aliased to their standard
        //    library analogs when the latter are available from the native
        //    library.
        //
        // Plan:
        // 1. If `BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY` is defined
        //    1. Use `bsl::is_same` to compare `bsl::unwrap_reference` to
        //       `std::unwrap_reference` using a representative type.  (C-1)
        //
        //    2. Use `bsl::is_same` to compare `bsl::unwrap_reference_t` to
        //       `std::unwrap_reference_t` using a representative type.  (C-1)
        //
        // Testing:
        //   CONCERN: Aliased to standard types when available.
        // --------------------------------------------------------------------

        if (verbose) puts("\nALIASED TO STANDARD META FUNCTIONS"
                          "\n==================================");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        typedef int T;

        if (veryVerbose) puts("\tTesting `unwrap_reference` `std` alias.");

        ASSERT((bsl::is_same<bsl::unwrap_reference<T>,
                             std::unwrap_reference<T> >::value));

        if (veryVerbose) puts("\tTesting `unwrap_reference_t` `std` alias.");

        ASSERT((bsl::is_same<X<bsl::unwrap_reference_t>,
                             X<std::unwrap_reference_t> >::value));
#else
        if (veryVerbose) puts("\tSkipped: standard types aren't available");
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING `bsl::unwrap_reference::type`
        //   Ensure that the `typedef` `type` of `bsl::unwrap_reference`
        //   instantiations having various (template parameter) `TYPE`s has the
        //   correct value.
        //
        // Concerns:
        // 1. `unwrap_reference::type` is `U&` for any
        //    `bsl::reference_wrapper<U>` or `std::reference_wrapper<U>`.
        //
        // 2. `unwrap_reference` does not transform `TYPE` when `TYPE` is not a
        //    reference-wrapper type.
        //
        // 3. `unwrap_reference_t` represents the return type of
        //    `unwrap_reference` meta-function.
        //
        // Plan:
        // 1. Instantiate `bsl::unwrap_reference` with various types and
        //    verify that the `type` member is initialized properly.  (C-1,2)
        //
        // 2. Verify that `bsl::unwrap_reference_t` has the same type as the
        //    return type of `bsl::unwrap_reference`. (C-3)
        //
        // Testing:
        //   bsl::unwrap_reference::type
        //   bsl::unwrap_reference_t
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING `bsl::unwrap_reference::type`"
                          "\n=====================================");

#define ASSERT_UNWRAP_REF_(TYPE, RESULT)                                    \
        ASSERT((true == bsl::is_same<bsl::unwrap_reference<TYPE>::type,      \
                                                               RESULT>::value))

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
  #define ASSERT_UNWRAP_REF_T(TYPE, RESULT)                                 \
        ASSERT((true == bsl::is_same<bsl::unwrap_reference_t<TYPE>,          \
                                                               RESULT>::value))

  #define ASSERT_UNWRAP_REF(TYPE, RESULT)                                    \
            ASSERT_UNWRAP_REF_(TYPE, RESULT); ASSERT_UNWRAP_REF_T(TYPE, RESULT)
#else  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
  #define ASSERT_UNWRAP_REF ASSERT_UNWRAP_REF_
#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

        ASSERT_UNWRAP_REF(void*,  void*);
        ASSERT_UNWRAP_REF(int&,   int&);
        ASSERT_UNWRAP_REF(Enum,   Enum);
        ASSERT_UNWRAP_REF(Struct, Struct);
        ASSERT_UNWRAP_REF(Union,  Union);
        ASSERT_UNWRAP_REF(Class,  Class);

        ASSERT_UNWRAP_REF(int Class::*, int Class::*);

        ASSERT_UNWRAP_REF(void(int), void(int));
        ASSERT_UNWRAP_REF(void(*)(int), void(*)(int));

        ASSERT_UNWRAP_REF(void, void);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
  #define ASSERT_UNWRAP_REF_WRAPPED(TYPE, RESULT)                  \
          ASSERT_UNWRAP_REF(bsl::reference_wrapper<TYPE >, RESULT); \
          ASSERT_UNWRAP_REF(std::reference_wrapper<TYPE >, RESULT)
#else
  #define ASSERT_UNWRAP_REF_WRAPPED(TYPE, RESULT)               \
          ASSERT_UNWRAP_REF(bsl::reference_wrapper<TYPE >, RESULT)
#endif

        ASSERT_UNWRAP_REF_WRAPPED(void*,  void*&);
        ASSERT_UNWRAP_REF_WRAPPED(int,    int&);
        ASSERT_UNWRAP_REF_WRAPPED(int&,   int&);
        ASSERT_UNWRAP_REF_WRAPPED(Enum,   Enum&);
        ASSERT_UNWRAP_REF_WRAPPED(Struct, Struct&);
        ASSERT_UNWRAP_REF_WRAPPED(Union,  Union&);
        ASSERT_UNWRAP_REF_WRAPPED(Class,  Class&);

        ASSERT_UNWRAP_REF_WRAPPED(int Class::*, int Class::*&);

        ASSERT_UNWRAP_REF_WRAPPED(void(int), void(&)(int));

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
// Copyright 2024 Bloomberg Finance L.P.
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
