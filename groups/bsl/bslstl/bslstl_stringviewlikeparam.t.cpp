// bslstl_stringviewlikeparam.t.cpp                                  -*-C++-*-
#include <bslstl_stringviewlikeparam.h>

#include <bslstl_stringview.h>

#include <bslma_bslallocator.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>   // `printf`
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test provides macros for SFINAE constraints on
// string-view-like parameters.  We verify that functions using these macros
// are correctly enabled or disabled based on the template argument type.
//
//-----------------------------------------------------------------------------
// MACROS
// [ 1] BSLSTL_STRINGVIEWLIKEPARAM_ENABLE_IF_T(ResultType)
// [ 1] BSLSTL_STRINGVIEWLIKEPARAM_ONLY_ENABLE_IF_T(ResultType)
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

typedef char                      CHAR_TYPE;
typedef std::char_traits<char>    CHAR_TRAITS;
typedef bsl::allocator<char>      ALLOCATOR;

struct MyStringViewWrapper {
    const char *d_data;
    size_t      d_length;

    operator bsl::string_view() const
    {
        return bsl::string_view(d_data, d_length);
    }
};

struct NotConvertible {
    int d_value;
};

// Test class using the BSLSTL_STRINGVIEWLIKEPARAM_ONLY_ENABLE_IF_T macro
class TestClassVoid {
  public:
    int d_which;

    TestClassVoid() : d_which(0) {}

    // Overload for C-string
    explicit TestClassVoid(const char *) : d_which(1) {}

    // Overload for string-view-like (excluding C-string)
    template <class STRING_VIEW_LIKE_TYPE>
    explicit TestClassVoid(
                  const STRING_VIEW_LIKE_TYPE&,
                  BSLSTL_STRINGVIEWLIKEPARAM_ONLY_ENABLE_IF_T(void) * = 0)
    : d_which(2)
    {}
};

// Test class using BSLSTL_STRINGVIEWLIKEPARAM_ONLY_ENABLE_IF_T w/allocator
class TestClassAlloc {
  public:
    int d_which;

    TestClassAlloc() : d_which(0) {}

    // Overload for C-string
    explicit TestClassAlloc(const char *,
                            const ALLOCATOR& = ALLOCATOR())
    : d_which(1)
    {}

    // Overload for string-view-like (excluding C-string)
    template <class STRING_VIEW_LIKE_TYPE>
    explicit TestClassAlloc(
            const STRING_VIEW_LIKE_TYPE&,
            BSLSTL_STRINGVIEWLIKEPARAM_ONLY_ENABLE_IF_T(const ALLOCATOR&)
                                                  basicAllocator = ALLOCATOR())
    : d_which(2)
    {
        (void)basicAllocator;
    }
};

// Test class using BSLSTL_STRINGVIEWLIKEPARAM_ONLY_ENABLE_IF_T(ResultType)
class TestClassOnlyMacro {
  public:
    int d_which;

    TestClassOnlyMacro() : d_which(0) {}

    // Overload for C-string
    explicit TestClassOnlyMacro(const char *,
                                const ALLOCATOR& = ALLOCATOR())
    : d_which(1)
    {}

    // Overload for string-view-like (excluding C-string) using new macro
    template <class STRING_VIEW_LIKE_TYPE>
    explicit TestClassOnlyMacro(
            const STRING_VIEW_LIKE_TYPE&,
            BSLSTL_STRINGVIEWLIKEPARAM_ONLY_ENABLE_IF_T(const ALLOCATOR&)
                                                  basicAllocator = ALLOCATOR())
    : d_which(2)
    {
        (void)basicAllocator;
    }
};

// Test class using BSLSTL_STRINGVIEWLIKEPARAM_ENABLE_IF_T(ResultType) macro
class TestClassMacro {
  public:
    int d_which;

    TestClassMacro() : d_which(0) {}

    // Overload for string-view-like using new macro (void* version)
    template <class STRING_VIEW_LIKE_TYPE>
    explicit TestClassMacro(
            const STRING_VIEW_LIKE_TYPE&,
            BSLSTL_STRINGVIEWLIKEPARAM_ENABLE_IF_T(void) * = 0)
    : d_which(1)
    {}
};

}  // close unnamed namespace

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
        // 1. Incorporate usage example from header into test driver.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

        // The usage example is demonstrated by the test classes above.
        // Verify that string-view-like types use overload 2.
        const MyStringViewWrapper svw = {"hello", 5};
        const TestClassVoid       tcv(svw);
        ASSERT(2 == tcv.d_which);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // MACRO FUNCTIONALITY
        //   Ensure the macros correctly enable/disable function templates.
        //
        // Concerns:
        // 1. Functions using `_ONLY_VOID` macros are enabled for types
        //    convertible to string_view but not to C-string.
        //
        // 2. Functions using `_ONLY_VOID` macros are disabled for C-strings,
        //    allowing the C-string overload to be selected.
        //
        // 3. Functions using `_ONLY_ALLOC` macros work similarly with an
        //    allocator parameter.
        //
        // 4. The `BSLSTL_STRINGVIEWLIKEPARAM_ONLY_ENABLE_IF_T(ResultType)`
        //    macro provides the same SFINAE behavior and allows specifying
        //    the result type directly, excluding C-strings.
        //
        // 5. The `BSLSTL_STRINGVIEWLIKEPARAM_ENABLE_IF_T(ResultType)` macro
        //    enables functions for types convertible to string_view (without
        //    the C-string exclusion).
        //
        // Plan:
        // 1. Construct TestClassVoid with various argument types and verify
        //    the correct constructor is selected.  (C-1,2)
        //
        // 2. Construct TestClassAlloc with various argument types and verify
        //    the correct constructor is selected.  (C-3)
        //
        // 3. Construct TestClassOnlyMacro with various argument types and
        //    verify the correct constructor is selected.  (C-4)
        //
        // 4. Construct TestClassMacro with string-view-like types and verify
        //    the constructor is enabled.  (C-5)
        //
        // Testing:
        //   BSLSTL_STRINGVIEWLIKEPARAM_ENABLE_IF_T(ResultType)
        //   BSLSTL_STRINGVIEWLIKEPARAM_ONLY_ENABLE_IF_T(ResultType)
        // --------------------------------------------------------------------

        if (verbose) printf("MACRO FUNCTIONALITY\n"
                            "===================\n");

        // Test with C-string - should select overload 1
        {
            const TestClassVoid tc("hello");
            ASSERT(1 == tc.d_which);
        }

        // Test with string_view - should select overload 2
        {
            const bsl::string_view sv("hello");
            const TestClassVoid    tc(sv);
            ASSERT(2 == tc.d_which);
        }

        // Test with wrapper convertible to string_view - should select
        // overload 2
        {
            const MyStringViewWrapper svw = {"hello", 5};
            const TestClassVoid       tc(svw);
            ASSERT(2 == tc.d_which);
        }

        // Test ALLOC variant with C-string - should select overload 1
        {
            const TestClassAlloc tc("hello");
            ASSERT(1 == tc.d_which);
        }

        // Test ALLOC variant with string_view - should select overload 2
        {
            const bsl::string_view sv("hello");
            const TestClassAlloc   tc(sv);
            ASSERT(2 == tc.d_which);
        }

        // Test BSLSTL_STRINGVIEWLIKEPARAM_ONLY_ENABLE_IF_T(ResultType)
        // macro with C-string - should select overload 1
        {
            const TestClassOnlyMacro tc("hello");
            ASSERT(1 == tc.d_which);
        }

        // Test BSLSTL_STRINGVIEWLIKEPARAM_ONLY_ENABLE_IF_T(ResultType)
        // macro with string_view - should select overload 2
        {
            const bsl::string_view   sv("hello");
            const TestClassOnlyMacro tc(sv);
            ASSERT(2 == tc.d_which);
        }

        // Test BSLSTL_STRINGVIEWLIKEPARAM_ONLY_ENABLE_IF_T(ResultType)
        // macro with wrapper - should select overload 2
        {
            const MyStringViewWrapper svw = {"hello", 5};
            const TestClassOnlyMacro  tc(svw);
            ASSERT(2 == tc.d_which);
        }

        // Test BSLSTL_STRINGVIEWLIKEPARAM_ENABLE_IF_T(ResultType) macro
        // with string_view
        {
            const bsl::string_view sv("hello");
            const TestClassMacro   tc(sv);
            ASSERT(1 == tc.d_which);
        }

        // Test BSLSTL_STRINGVIEWLIKEPARAM_ENABLE_IF_T(ResultType) macro
        // with wrapper
        {
            const MyStringViewWrapper svw = {"hello", 5};
            const TestClassMacro      tc(svw);
            ASSERT(1 == tc.d_which);
        }
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
