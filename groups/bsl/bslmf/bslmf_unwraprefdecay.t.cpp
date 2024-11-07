// bslmf_unwraprefdecay.t.cpp                                         -*-C++-*-
#include <bslmf_unwraprefdecay.h>

#include <bslmf_issame.h>  // for testing only

#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
#include <type_traits>  // `std::unwrap_reference`, `std::unwrap_reference_t`
#endif

#include <stdio.h>   // `printf`
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function `bsl::unwrap_reference`
// that unwraps a reference wrapper specialization `bsl::reference_wrapper<U>`
// or `std::reference_wrapper<U>` template `TYPE` argument by providing a
// `type` member with the type `U&`.  If the specified `TYPE` is not a
// specialization of either reference wrapper the member `type` shall be
// `TYPE`.
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
// PUBLIC TYPES
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

///Example 1: Unwrap Reference Wrapped Argument Types
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we work in a programming environment where function argument
// types may be presented as is, or wrapped in a `bsl::reference_wrapper` or
// its `std` equivalent and we would like to create a member or local variable
// while obeying the request for reference-wrapping.  So when the argument type
// decays into a reference wrapper we would like to use a reference to the
// wrapped type, and simply use the decayed variations of the unwrapped types.
// This is a use case for `bsl::unwrap_ref_decay`.
//
// First, we create types that may be the bases for reference-wrapped, and
// normal type parameters:
// ```
    typedef bsl::reference_wrapper<int *>  WrappedType;
    typedef int                           *NotWrappedType;
// ```
// Next, we create types that represent possible function argument types:
// ```
    typedef WrappedType                     WrappedTypeArray[5];
    typedef WrappedType                    *WrappedTypePointer;
    typedef const WrappedType               ConstWrappedType;
    typedef volatile WrappedType            VolatileWrappedType;
    typedef const volatile WrappedType      CvWrappedType;
    typedef WrappedType&                    WrappedTypeRef;
    typedef const WrappedType&              ConstWrappedTypeRef;
    typedef volatile WrappedType&           VolatileWrappedTypeRef;
    typedef const volatile WrappedType&     CvWrappedTypeRef;

    typedef NotWrappedType                  NotWrappedTypeArray[5];
    typedef NotWrappedType                 *NotWrappedTypePointer;
    typedef const NotWrappedType            ConstNotWrappedType;
    typedef volatile NotWrappedType         VolatileNotWrappedType;
    typedef const volatile NotWrappedType   CvNotWrappedType;
    typedef NotWrappedType&                 NotWrappedTypeRef;
    typedef const NotWrappedType&           ConstNotWrappedTypeRef;
    typedef volatile NotWrappedType&        VolatileNotWrappedTypeRef;
    typedef const volatile NotWrappedType&  CvNotWrappedTypeRef;
// ```
// Finally we can verify and demonstrate how all these types turn into a
// decayed type, and only those that decay into a reference-wrapper type will
// become themselves references.  Notice that an array of reference wrappers
// decays into a pointer to a wrapper, so it will not turn into a reference.
// ```
    ASSERT((true == bsl::is_same<bsl::unwrap_ref_decay<WrappedTypeArray>::type,
                                 WrappedType *>::value));
    ASSERT((true == bsl::is_same<
                        bsl::unwrap_ref_decay<WrappedTypePointer>::type,
                        WrappedTypePointer>::value));

    ASSERT((true == bsl::is_same<bsl::unwrap_ref_decay<WrappedType>::type,
                                 int *&>::value));
    ASSERT((true == bsl::is_same<
                        bsl::unwrap_ref_decay<ConstWrappedType>::type,
                        int *&>::value));
    ASSERT((true == bsl::is_same<
                        bsl::unwrap_ref_decay<VolatileWrappedType>::type,
                        int *&>::value));
    ASSERT((true == bsl::is_same<bsl::unwrap_ref_decay<CvWrappedType>::type,
                                 int *&>::value));
    ASSERT((true == bsl::is_same<bsl::unwrap_ref_decay<WrappedTypeRef>::type,
                                 int *&>::value));
    ASSERT((true == bsl::is_same<
                        bsl::unwrap_ref_decay<ConstWrappedTypeRef>::type,
                        int *&>::value));
    ASSERT((true == bsl::is_same<
                        bsl::unwrap_ref_decay<VolatileWrappedTypeRef>::type,
                        int *&>::value));
    ASSERT((true == bsl::is_same<bsl::unwrap_ref_decay<CvWrappedTypeRef>::type,
                                 int *&>::value));

    ASSERT((true == bsl::is_same<bsl::unwrap_ref_decay<WrappedTypeArray>::type,
                                 WrappedType *>::value));
    ASSERT((true == bsl::is_same<
                        bsl::unwrap_ref_decay<WrappedTypePointer>::type,
                        WrappedTypePointer>::value));

    // Not wrapped types decay
    ASSERT((true == bsl::is_same<
                        bsl::unwrap_ref_decay<NotWrappedTypeArray>::type,
                        NotWrappedType *>::value));
    ASSERT((true == bsl::is_same<
                        bsl::unwrap_ref_decay<NotWrappedTypePointer>::type,
                        NotWrappedTypePointer>::value));

    ASSERT((true == bsl::is_same<bsl::unwrap_ref_decay<NotWrappedType>::type,
                                 NotWrappedType>::value));
    ASSERT((true == bsl::is_same<
                        bsl::unwrap_ref_decay<ConstNotWrappedType>::type,
                        NotWrappedType>::value));
    ASSERT((true == bsl::is_same<
                        bsl::unwrap_ref_decay<VolatileNotWrappedType>::type,
                        NotWrappedType>::value));
    ASSERT((true == bsl::is_same<bsl::unwrap_ref_decay<CvNotWrappedType>::type,
                                 NotWrappedType>::value));
    ASSERT((true == bsl::is_same<
                        bsl::unwrap_ref_decay<NotWrappedTypeRef>::type,
                        NotWrappedType>::value));
    ASSERT((true == bsl::is_same<
                        bsl::unwrap_ref_decay<ConstNotWrappedTypeRef>::type,
                        NotWrappedType>::value));
    ASSERT((true == bsl::is_same<
                        bsl::unwrap_ref_decay<VolatileNotWrappedTypeRef>::type,
                        NotWrappedType>::value));
    ASSERT((true == bsl::is_same<
                        bsl::unwrap_ref_decay<CvNotWrappedTypeRef>::type,
                        NotWrappedType>::value));
// ```
// Note, that (when available) the `bsl::unwrap_ref_decay_t` avoids the
// `::type` suffix and `typename` prefix when we want to use the result of the
// `bsl::unwrap_ref_decay` meta-function in templates.

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // ALIASED TO STANDARD META FUNCTIONS
        //
        // Concerns:
        // 1. The meta functions `bsl::unwrap_ref_decay` and
        //    `bsl::unwrap_ref_decay_t` should be aliased to their standard
        //    library analogs when the latter is available from the native
        //    library.
        //
        // Plan:
        // 1. If `BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY` is defined
        //    1. Use `bsl::is_same` to compare `bsl::remove_ref_decay` to
        //       `std::remove_ref_decay` using a representative type.
        //
        //    2. Use `bsl::is_same` to compare `bsl::remove_reference_t` to
        //       `std::remove_reference_t` using a representative type.
        //
        // Testing:
        //   CONCERN: Aliased to standard types when available.
        // --------------------------------------------------------------------

        if (verbose) puts("\nALIASED TO STANDARD META FUNCTIONS"
                          "\n==================================");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        typedef int T;

        if (veryVerbose) puts("\tTesting `unwrap_ref_decay` `std` alias.");

        ASSERT((bsl::is_same<bsl::unwrap_ref_decay<T>,
                             std::unwrap_ref_decay<T> >::value));

        if (veryVerbose) puts("\tTesting `unwrap_ref_decay_t` `std` alias.");

        ASSERT((bsl::is_same<bsl::unwrap_ref_decay_t<T>,
                             std::unwrap_ref_decay_t<T> >::value));
#else
        if (veryVerbose) puts("\tSkipped: standard types aren't available");
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING `bsl::unwrap_ref_decay::type`
        //   Ensure that the `typedef` `type` of `bsl::unwrap_ref_decay`
        //   instantiations having various (template parameter) `TYPE`s has the
        //   correct value.
        //
        // Concerns:
        // 1. `unwrap_ref_decay::type` is `U&` for any template type argument
        //    that decays (`bsl::decay`) into `bsl::reference_wrapper<U>` or
        //    `std::reference_wrapper<U>`.
        //
        // 2. `unwrap_ref_decay` does not transform `TYPE` when `TYPE` does not
        //    decay into a reference-wrapper type.
        //
        // 3. `unwrap_ref_decay_t` represents the return type of
        //    `unwrap_ref_decay` meta-function.
        //
        // Plan:
        // 1. Instantiate `bsl::unwrap_ref_decay` with various types and
        //    verify that the `type` member is initialized properly.  (C-1,2)
        //
        // 2. Verify that `bsl::unwrap_ref_decay_t` has the same type as the
        //    return type of `bsl::unwrap_ref_decay`. (C-3)
        //
        // Testing:
        //   bsl::unwrap_ref_decay::type
        //   bsl::unwrap_ref_decay_t
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING `bsl::unwrap_ref_decay::type`"
                          "\n=====================================");

#define ASSERT_UNWRAP_REF_(TYPE, RESULT)                                    \
        ASSERT((true == bsl::is_same<bsl::unwrap_ref_decay<TYPE>::type,      \
                                                               RESULT>::value))

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
  #define ASSERT_UNWRAP_REF_T(TYPE, RESULT)                                 \
        ASSERT((true == bsl::is_same<bsl::unwrap_ref_decay_t<TYPE>,          \
                                                               RESULT>::value))

  #define ASSERT_UNWRAP_REF(TYPE, RESULT)                                    \
            ASSERT_UNWRAP_REF_(TYPE, RESULT); ASSERT_UNWRAP_REF_T(TYPE, RESULT)
#else  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
  #define ASSERT_UNWRAP_REF ASSERT_UNWRAP_REF_
#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

        ASSERT_UNWRAP_REF(void*,               void*);

        ASSERT_UNWRAP_REF(int&,                int);
        ASSERT_UNWRAP_REF(const int&,          int);
        ASSERT_UNWRAP_REF(const volatile int&, int);

        typedef int IntArray[5];
        ASSERT_UNWRAP_REF(IntArray,            int*);
        ASSERT_UNWRAP_REF(IntArray&,           int*);

        typedef void FuncType();
        ASSERT_UNWRAP_REF(FuncType,            FuncType*);
        ASSERT_UNWRAP_REF(FuncType&,           FuncType*);

        // Arrays decay into pointers so the wrapper should not be unwrapped
        typedef bsl::reference_wrapper<int> WrappedType;
        ASSERT_UNWRAP_REF(WrappedType[],      WrappedType*);

        ASSERT_UNWRAP_REF(FuncType,            FuncType*);
        ASSERT_UNWRAP_REF(Enum,                Enum);
        ASSERT_UNWRAP_REF(Struct,              Struct);
        ASSERT_UNWRAP_REF(Union,               Union);
        ASSERT_UNWRAP_REF(Class,               Class);

        ASSERT_UNWRAP_REF(int Class::*, int Class::*);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
  #define ASSERT_UNWRAP_REF_WRAPPED(TYPE, RESULT)                             \
          ASSERT_UNWRAP_REF(bsl::reference_wrapper<TYPE >,           RESULT); \
          ASSERT_UNWRAP_REF(std::reference_wrapper<TYPE >,           RESULT); \
          ASSERT_UNWRAP_REF(bsl::reference_wrapper<TYPE >&,          RESULT); \
          ASSERT_UNWRAP_REF(std::reference_wrapper<TYPE >&,          RESULT); \
          ASSERT_UNWRAP_REF(const bsl::reference_wrapper<TYPE >,     RESULT); \
          ASSERT_UNWRAP_REF(const std::reference_wrapper<TYPE >,     RESULT); \
          ASSERT_UNWRAP_REF(const bsl::reference_wrapper<TYPE >&,    RESULT); \
          ASSERT_UNWRAP_REF(const std::reference_wrapper<TYPE >&,    RESULT); \
          ASSERT_UNWRAP_REF(volatile bsl::reference_wrapper<TYPE >,  RESULT); \
          ASSERT_UNWRAP_REF(volatile std::reference_wrapper<TYPE >,  RESULT); \
          ASSERT_UNWRAP_REF(volatile bsl::reference_wrapper<TYPE >&, RESULT); \
          ASSERT_UNWRAP_REF(volatile std::reference_wrapper<TYPE >&, RESULT); \
          ASSERT_UNWRAP_REF(const volatile bsl::reference_wrapper<TYPE >,     \
                                                                    RESULT);  \
          ASSERT_UNWRAP_REF(const volatile std::reference_wrapper<TYPE >,     \
                                                                    RESULT);  \
          ASSERT_UNWRAP_REF(const volatile bsl::reference_wrapper<TYPE >&,    \
                                                                     RESULT); \
          ASSERT_UNWRAP_REF(const volatile std::reference_wrapper<TYPE >&,    \
                                                                        RESULT)
#else
  #define ASSERT_UNWRAP_REF_WRAPPED(TYPE, RESULT)                             \
          ASSERT_UNWRAP_REF(bsl::reference_wrapper<TYPE >,           RESULT); \
          ASSERT_UNWRAP_REF(bsl::reference_wrapper<TYPE >&,          RESULT); \
          ASSERT_UNWRAP_REF(const bsl::reference_wrapper<TYPE >,     RESULT); \
          ASSERT_UNWRAP_REF(const bsl::reference_wrapper<TYPE >&,    RESULT); \
          ASSERT_UNWRAP_REF(volatile bsl::reference_wrapper<TYPE >,  RESULT); \
          ASSERT_UNWRAP_REF(volatile bsl::reference_wrapper<TYPE >&, RESULT); \
          ASSERT_UNWRAP_REF(const volatile bsl::reference_wrapper<TYPE >,     \
                                                                     RESULT); \
          ASSERT_UNWRAP_REF(const volatile bsl::reference_wrapper<TYPE >&,    \
                                                                        RESULT)
#endif

        ASSERT_UNWRAP_REF_WRAPPED(void*,              void*&);
        ASSERT_UNWRAP_REF_WRAPPED(int,                int&);
        ASSERT_UNWRAP_REF_WRAPPED(int&,               int&);
        ASSERT_UNWRAP_REF_WRAPPED(const int&,         const int&);
        ASSERT_UNWRAP_REF_WRAPPED(const volatile int, const volatile int&);
        ASSERT_UNWRAP_REF_WRAPPED(Enum,               Enum&);
        ASSERT_UNWRAP_REF_WRAPPED(Struct,             Struct&);
        ASSERT_UNWRAP_REF_WRAPPED(Union,              Union&);
        ASSERT_UNWRAP_REF_WRAPPED(Class,              Class&);

        ASSERT_UNWRAP_REF_WRAPPED(int Class::*, int Class::*&);

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
