// bsla_nouniqueaddress.t.cpp                                         -*-C++-*-
#include <bsla_nouniqueaddress.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>  // `atoi`

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The effect of `BSLA_NO_UNIQUE_ADDRESS` is observable through the size and
// layout of the classes in which it is used, which we verify at run time.
// ----------------------------------------------------------------------------
// [ 2] BSLA_NO_UNIQUE_ADDRESS
// [ 2] BSLA_NO_UNIQUE_ADDRESS_IS_ACTIVE
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 1] BREATHING TEST

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

#define STRINGIFY2(...) "" #__VA_ARGS__
#define STRINGIFY(a) STRINGIFY2(a)

// ============================================================================
//                                   USAGE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Annotating a Data Member of an Empty Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we define an empty type, `DefaultPolicy`, and a class, `Widget`,
// having a data member of type `DefaultPolicy` annotated with
// `BSLA_NO_UNIQUE_ADDRESS`:
// ```
struct DefaultPolicy {
};

struct Widget {
    BSLA_NO_UNIQUE_ADDRESS DefaultPolicy d_policy;
    int                                  d_value;
};
// ```

// ============================================================================
//                  DEFINITION OF ANNOTATED TYPES
// ----------------------------------------------------------------------------

namespace {

struct Empty {
};

struct Plain {
    Empty d_empty;
    int   d_value;
};

struct Annotated {
    BSLA_NO_UNIQUE_ADDRESS Empty d_empty;
    int                          d_value;
};

struct TwoSame {
    BSLA_NO_UNIQUE_ADDRESS Empty d_first;
    BSLA_NO_UNIQUE_ADDRESS Empty d_second;
    int                          d_value;
};

}  // close unnamed namespace

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;

    printf("TEST %s CASE %d\n", __FILE__, test);

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
        // 1. Incorporate usage example from header into test driver, replace
        //    leading comment characters with spaces, and replace `assert` with
        //    `ASSERT`.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

// Then, we observe that, if the annotation is active, `d_policy` occupies no
// storage:
// ```
#ifdef BSLA_NO_UNIQUE_ADDRESS_IS_ACTIVE
    ASSERT(sizeof(Widget) == sizeof(int));
#endif
// ```
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING `BSLA_NO_UNIQUE_ADDRESS`
        //
        // Concerns:
        // 1. If active, an annotated member of empty type occupies no storage.
        //
        // 2. If not active, the annotation has no effect on layout.
        //
        // 3. Two annotated members of the same empty type have distinct
        //    addresses.
        //
        // 4. The link-coercion symbol matching this build is defined.
        //
        // Plan:
        // 1. Compare `sizeof` of annotated and un-annotated classes.  (C-1..2)
        //
        // 2. Compare the addresses of two annotated members of the same empty
        //    type.  (C-3)
        //
        // 3. Check the value of `bsla::NoUniqueAddress_Coerce::s_value`.
        //    (C-4)
        //
        // Testing:
        //   BSLA_NO_UNIQUE_ADDRESS
        //   BSLA_NO_UNIQUE_ADDRESS_IS_ACTIVE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `BSLA_NO_UNIQUE_ADDRESS`"
                            "\n================================\n");

        if (veryVerbose) {
            P(sizeof(Plain));
            P(sizeof(Annotated));
        }

#ifdef BSLA_NO_UNIQUE_ADDRESS_IS_ACTIVE
        ASSERT(sizeof(Annotated) == sizeof(int));
        ASSERT(sizeof(Plain)     >  sizeof(int));
#else
        ASSERT(sizeof(Annotated) == sizeof(Plain));
#endif

        TwoSame mX;
        ASSERT(static_cast<void *>(&mX.d_first) !=
                                           static_cast<void *>(&mX.d_second));

        // The link-coercion symbol matching this build is defined.

#ifdef BSLA_NO_UNIQUE_ADDRESS_IS_ACTIVE
        ASSERT(1 == BloombergLP::bsla::NoUniqueAddress_Coerce::s_value);
#else
        ASSERT(0 == BloombergLP::bsla::NoUniqueAddress_Coerce::s_value);
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        // 1. The macros can be printed.
        //
        // Plan:
        // 1. In verbose mode, print the macro expansions.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) {
            printf("BSLA_NO_UNIQUE_ADDRESS: %s\n",
                   STRINGIFY(BSLA_NO_UNIQUE_ADDRESS));
#ifdef BSLA_NO_UNIQUE_ADDRESS_IS_ACTIVE
            printf("BSLA_NO_UNIQUE_ADDRESS_IS_ACTIVE: %d\n",
                   BSLA_NO_UNIQUE_ADDRESS_IS_ACTIVE);
#else
            printf("BSLA_NO_UNIQUE_ADDRESS_IS_ACTIVE: UNDEFINED\n");
#endif
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d` NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
