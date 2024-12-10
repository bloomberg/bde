// bslstl_referencewrapper.t.cpp                                      -*-C++-*-
#include <bslstl_referencewrapper.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a legacy, backwards compatibility component that
// simply `#include`s `<bslmf_referencewrapper.h>`.  In this test driver we
// simply verify that the names in `bsl` exist, and that the
// `BSLSTL_REFRENCEWRAPPER_IS_ALIASED` macro is defined when expected.
//
// ----------------------------------------------------------------------------
// [ 1] reference_wrapper
// [ 1] reference_wrapper<T> cref(const T&);
// [ 1] reference_wrapper<T> cref(reference_wrapper<T>);
// [ 1] reference_wrapper<T> ref(T&);
// [ 1] reference_wrapper<T> ref(reference_wrapper<T>);
// [ 1] BSLSTL_REFRENCEWRAPPER_IS_ALIASED

// ============================================================================
//                  STANDARD BSL ASSERT TEST FUNCTION
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
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;  (void)veryVerbose;
    bool     veryVeryVerbose = argc > 4;  (void)veryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // VERIFY EXISTENCE OF NAMES
        //
        // Concerns:
        // 1. `<bslmf_referencewrapper.h>` was included
        //
        // Plan:
        // 1. Define a dummy `typedef` using `bsl::reference_wrapper`.
        //
        // 2. Get the address of the free functions.
        //
        // 3. Use the wrappers' explicit and implicit accessors.  (C-1)
        //
        // 4. Verify that `BSLSTL_REFRENCE_WRAPPER_IS_ALIASED` is defined when
        //    `BSLLMF_REFRENCE_WRAPPER_IS_ALIASED` is defined, and otherwise
        //    it is not defined.
        //
        // Testing:
        //   reference_wrapper;
        //   reference_wrapper<T> cref(const T&);
        //   reference_wrapper<T> cref(reference_wrapper<T>);
        //   reference_wrapper<T> ref(T&);
        //   reference_wrapper<T> ref(reference_wrapper<T>);
        //   BSLSTL_REFRENCEWRAPPER_IS_ALIASED
        // --------------------------------------------------------------------

        if (verbose) puts("\nVERIFY EXISTENCE OF NAMES"
                          "\n=========================");

        int i = 42;
        bsl::reference_wrapper<int> irw(i);
        ASSERT(&irw.operator int& () == &i);

        bsl::reference_wrapper<int> irw2 = bsl::ref(i);
        ASSERT(&irw.operator int& () == &irw2.operator int& ());
        bsl::reference_wrapper<int> irw3 = bsl::ref(irw2);
        ASSERT(&irw2.operator int& () == &irw3.operator int& ());

        bsl::reference_wrapper<const int> cirw = bsl::cref(i);
        ASSERT(&irw.operator int& () == &cirw.operator const int& ());
        bsl::reference_wrapper<const int> cirw2 = bsl::cref(cirw);
        ASSERT(&cirw.operator const int& () == &cirw2.operator const int& ());

        const bool BSLSTL_REFRENCEWRAPPER_IS_ALIASED_is_defined =
#ifdef BSLSTL_REFRENCEWRAPPER_IS_ALIASED
            true;
#else
            false;
#endif

        const bool BSLMF_REFERENCEWRAPPER_IS_ALIASED_is_defined =
#ifdef BSLMF_REFERENCEWRAPPER_IS_ALIASED
            true;
#else
            false;
#endif

        ASSERT(BSLMF_REFERENCEWRAPPER_IS_ALIASED_is_defined ==
                                 BSLSTL_REFRENCEWRAPPER_IS_ALIASED_is_defined);

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
