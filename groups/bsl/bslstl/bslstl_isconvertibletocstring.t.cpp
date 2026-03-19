// bslstl_isconvertibletocstring.t.cpp                                -*-C++-*-
#include <bslstl_isconvertibletocstring.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>   // `printf`
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function,
// `bslstl::IsConvertibleToCString`, that determines whether a template
// parameter type can be converted to a C-string (`const CHAR_TYPE *`).  Thus,
// we need to ensure that the value returned by this meta-function is correct
// for various types.
//
//-----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bslstl::IsConvertibleToCString::value
//
// ----------------------------------------------------------------------------
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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

struct MyCStringWrapper {
    const char *d_str;
    operator const char *() const { return d_str; }
};

struct MyWCStringWrapper {
    const wchar_t *d_str;
    operator const wchar_t *() const { return d_str; }
};

struct NotConvertible {
    int d_value;
};

struct ConvertibleToVoidPtr {
    operator void *() const { return 0; }
};

}  // close unnamed namespace

//=============================================================================
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

namespace usage_example_1 {

///Usage
///-----
// In this section we show the intended use of this component.
//
///Example 1: Determine If a Type Is Convertible to a C-String
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to determine whether various types are convertible to
// C-strings (null-terminated character arrays).
//
// First, we create a type that is convertible to `const char *`:
// ```
struct TypeConvertibleToCString {
    const char *d_str;
    operator const char *() const { return d_str; }
};
// ```
// Now, we instantiate the `bslstl::IsConvertibleToCString` template for
// various types and verify the results:
// ```
void run()
{
    ASSERT((bslstl::IsConvertibleToCString<char, const char *>::value));
    ASSERT((bslstl::IsConvertibleToCString<char,
                                           TypeConvertibleToCString>::value));
    ASSERT(!(bslstl::IsConvertibleToCString<char, int>::value));
    ASSERT(!(bslstl::IsConvertibleToCString<char, void *>::value));
}
// ```

}  // close namespace usage_example_1

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

        usage_example_1::run();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // `bslstl::IsConvertibleToCString::value`
        //   Ensure that the static data member `value` of
        //   `bslstl::IsConvertibleToCString` instantiation has the correct
        //   value for various types.
        //
        // Concerns:
        // 1. `bslstl::IsConvertibleToCString<CHAR, TYPE>::value` is `true`
        //    when `TYPE` is `const CHAR *`.
        //
        // 2. `bslstl::IsConvertibleToCString<CHAR, TYPE>::value` is `true`
        //    when `TYPE` has a conversion operator to `const CHAR *`.
        //
        // 3. `bslstl::IsConvertibleToCString<CHAR, TYPE>::value` is `true`
        //    when `TYPE` is a character array (e.g., `const char (&)[N]`).
        //
        // 4. `bslstl::IsConvertibleToCString<CHAR, TYPE>::value` is `false`
        //    when `TYPE` is not convertible to `const CHAR *`.
        //
        // 5. Works correctly for different character types (char, wchar_t).
        //
        // Plan:
        //   Verify that `bslstl::IsConvertibleToCString::value` has the
        //   correct value for each (template parameter) `TYPE` in the
        //   concerns.  (C-1..5)
        //
        // Testing:
        //   bslstl::IsConvertibleToCString::value
        // --------------------------------------------------------------------

        if (verbose) printf("bslstl::IsConvertibleToCString::value\n"
                            "=====================================\n");

        // C-1: Direct pointer types
        ASSERT((bslstl::IsConvertibleToCString<char, const char *>::value));
        ASSERT((bslstl::IsConvertibleToCString<char, char *>::value));
        ASSERT((bslstl::IsConvertibleToCString<wchar_t,
                                              const wchar_t *>::value));

        // C-2: Types with conversion operators
        ASSERT((bslstl::IsConvertibleToCString<char,
                                              MyCStringWrapper>::value));
        ASSERT((bslstl::IsConvertibleToCString<wchar_t,
                                              MyWCStringWrapper>::value));

        // C-3: Character arrays
        ASSERT((bslstl::IsConvertibleToCString<char,
                                              const char (&)[10]>::value));
        ASSERT((bslstl::IsConvertibleToCString<char, char (&)[10]>::value));

        // C-4: Non-convertible types
        ASSERT(!(bslstl::IsConvertibleToCString<char, int>::value));
        ASSERT(!(bslstl::IsConvertibleToCString<char, NotConvertible>::value));
        ASSERT(!(bslstl::IsConvertibleToCString<char,
                                                ConvertibleToVoidPtr>::value));
        ASSERT(!(bslstl::IsConvertibleToCString<char, void *>::value));

        // C-5: Different character types don't cross-convert
        ASSERT(!(bslstl::IsConvertibleToCString<wchar_t,
                                                const char *>::value));
        ASSERT(!(bslstl::IsConvertibleToCString<char,
                                                const wchar_t *>::value));
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
