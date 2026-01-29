// bslstl_streamed.t.cpp                                              -*-C++-*-
#define BB_DEPRECATE_ENABLE_ALL_DEPRECATIONS_FOR_TESTING                      1

#include <bslfmt_streamed.h>

#include <bslfmt_format.h>
#include <bslfmt_formattertestutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>

#include <bslstl_string.h>

#include <iostream>
#include <string>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
  #include <format>
#endif

#include <limits.h>
#include <stdio.h>   // `printf`
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// Tested are the wrapper class template `bslfmt::Streamed<t_STREAMED>` as well
// as the wrapper-creator function template (mainly for C++03 compatibility)
// `bslfmt::streamed`.  Since this class just delegates to `StreamedFormatter`
// (which is already tested) we just do enough sanity check here to verify that
// the forwarding is there.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] formatter();
// [ 2] ~formatter();
// [ 3] formatter(const formatter &);
// [ 1] Streamed(const T&);
//
// MANIPULATORS
// [ 4] operator=(const formatter &);
// [ 5] formatter<Streamed<T> >::parse(PARSE_CONTEXT&);
//
// ACCESSORS
// [ 6] formatter<Streamed<T> >::format(VALUE, FORMAT_CONTEXT&);
//
// FREE FUNCTIONS
// [ 1] streamed(const T&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
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

#define ASSERT BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                  GLOBAL HELPER MACROS FOR TESTING
//-----------------------------------------------------------------------------

#define TEST_PARSE_FAIL(type, fmtStr)                                         \
    do {                                                                      \
        bsl::string errorMsg;                                                 \
        bool        rv;                                                       \
        rv = bslfmt::FormatterTestUtil<char>::testParseFailure<               \
            bslfmt::Streamed<type> >(&errorMsg, false, fmtStr);               \
        if (!rv) {                                                            \
            ASSERTV(errorMsg.c_str(), fmtStr, rv);                            \
        }                                                                     \
    } while (false)

#define TEST_PARSE_SUCCESS_F(type, fmtStr)                                    \
    do {                                                                      \
        bsl::string errorMsg;                                                 \
        bool        rv;                                                       \
                                                                              \
        rv = bslfmt::FormatterTestUtil<char>::testParseFormat<                \
            bslfmt::Streamed<type> >(&errorMsg, false, fmtStr);               \
        if (!rv) {                                                            \
            ASSERTV(errorMsg.c_str(), fmtStr, rv);                            \
        }                                                                     \
    } while (false)

#define TEST_PARSE_SUCCESS_VF(type, fmtStr)                                   \
    do {                                                                      \
        bsl::string errorMsg;                                                 \
        bool        rv;                                                       \
                                                                              \
        rv = bslfmt::FormatterTestUtil<char>::testParseVFormat<               \
            bslfmt::Streamed<type> >(&errorMsg, false, fmtStr);               \
        if (!rv) {                                                            \
            ASSERTV(errorMsg.c_str(), fmtStr, rv);                            \
        }                                                                     \
    } while (false)

//=============================================================================
//                                  TEST TYPES
//-----------------------------------------------------------------------------

                              // ================
                              // struct CanStream
                              // ================

struct CanStream {
    const char *d_content;

    // CREATORS
    explicit CanStream(const char *content)
    : d_content(content)
    {
    }

    // MANIPULATORS
    void setContent(const char *content) { d_content = content; }

    // ACCESSORS
    const char *content() const { return d_content; }
};

std::ostream& operator<<(std::ostream& os, const CanStream& obj)
{
    return os << obj.d_content;
}

                              // ================
                              // class Streamable
                              // ================

class Streamable {
};

std::ostream& operator<<(std::ostream& os, const Streamable&)
{
    return os << "12345678";
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Formatting a Streamable Object
/// - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<`.
//
// First, we define a type with a streaming operator but without a formatter
// specialization:
//```
    class NonFormattableType {};

    std::ostream& operator<<(std::ostream& os, const NonFormattableType&)
    {
        return os << "The printout";
    }
//```
// Then we create an instance of this type and use bsl::streamed to allow us to
// format it:
//```
//  const NonFormattableType obj;
//```
// Next, we format the "value" using `bsl::format` with the wrapper-creator
// function:
//```
// bsl::string s = bsl::format("{}", bslfmt::streamed(obj));
//```
// Finally, we verify the output is correct:
//```
// assert(s == "The printout");
//```

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;  (void)veryVeryVerbose;
    const bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: No global memory is allocated after `main` starts.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    // Confirm no static initialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

    switch (test) {  case 0:
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Use the Usage Examples from the header by replacing `assert` with
        //    `ASSERT` and moving definitions of types and functions outside of
        //    `main`.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");

///Formatting a Streamable Object
/// - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<`.
//
// First, we define a type with a streaming operator but without a formatter
// specialization:
//```
//  class NonFormattableType {};
//
//  std::ostream& operator<<(std::ostream& os, const NonFormattableType&)
//  {
//      return os << "The printout";
//  }
//```
// Then we create an instance of this type and use bsl::streamed to allow us to
// format it:
//```
    const NonFormattableType obj;
//```
// Next, we format the "value" using `bsl::format` with the wrapper-creator
// function:
//```
   bsl::string s = bsl::format("{}", bslfmt::streamed(obj));
//```
// Finally, we verify the output is correct:
//```
   ASSERT(s == "The printout");
//```
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING format(VALUE, FORMAT_CONTEXT&)
        //
        // Concerns:
        // 1. `format` is properly forwarded to the implementation.
        //
        // Plan:
        // 1. Spot check that formatting works.  (C-1)
        //
        // Testing:
        //   formatter<Streamed<T> >::format(VALUE, FORMAT_CONTEXT&);
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING format(VALUE, FORMAT_CONTEXT&)"
                          "\n======================================");
        if (veryVerbose)
            puts("\nThis has been tested exhaustively in the breathing test.");

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING parse(PARSE_CONTEXT&)
        //
        // Concerns:
        // 1. Invalid format specs will generate a parse error
        // 2. Valid format specs will not generate a parse error
        //
        // Plan:
        // 1. Construct format specs corresponding to each of the known error
        //    conditions and verify that they result in a parse error.  (C-1)
        //
        // 2. Construct format specs containing different combinations of
        //    valid specification components and verify that they correctly
        //    parse.  (C-2)
        //
        // Testing:
        //   formatter<Streamed<T> >::parse(PARSE_CONTEXT&);
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING parse(PARSE_CONTEXT&)"
                          "\n=============================");

        // Bad fill character
        // Note can only test '{' as '}' closes the parse string.
        TEST_PARSE_FAIL(Streamable, "{:{<5.5s}");

        // A selection of valid format strings (non-Unicode)
        TEST_PARSE_SUCCESS_F(Streamable, "{0:*^{1}.{1}}"         );

#if defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE <= 13
  #define TPS TEST_PARSE_SUCCESS_VF
#else
  #define TPS TEST_PARSE_SUCCESS_F
#endif

        // A selection of valid format strings (Unicode)
        TPS(Streamable, "{0:\xF0\x9F\x98\x80^{1}.{1}}" );
        #undef TPS
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //
        // Concerns:
        // 1. We can assign 'bsl::formatter' types for `Streamed` wrapper
        //    specializations.
        //
        // 2. We can assign 'std::formatter' types for `Streamed` wrapper
        //    specializations.
        //
        // Plan:
        // 1. Construct two 'bsl::formatter's for `Streamed` wrapper
        //    specializations, and assign one to the other. (C-1)
        //
        // 2. Construct two 'std::formatter's for `Streamed` wrapper
        //    specializations, and assign one to the other. (C-2)
        //
        // Testing:
        //   operator=(const formatter &);
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING ASSIGNMENT OPERATOR"
                          "\n===========================");

        {
            const bsl::formatter<bslfmt::Streamed<Streamable>, char> dummy_c;
            bsl::formatter<bslfmt::Streamed<Streamable>, char>       dummy2_c;
            dummy2_c = dummy_c;
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        {
            const std::formatter<bslfmt::Streamed<Streamable>, char> dummy_c;
            std::formatter<bslfmt::Streamed<Streamable>, char>       dummy2_c;
            dummy2_c = dummy_c;
        }
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //
        // Concerns:
        // 1. We can copy construct 'bsl::formatter' types of `Streamed`
        //    wrapper instances.
        //
        // 2. We can copy construct 'std::formatter' types of `Streamed`
        //    wrapper instances.
        //
        // Plan:
        // 1. Construct a 'bsl::formatter' for a wrapper instance and copy it.
        //    (C-1)
        //
        // 2. Construct a 'std::formatter' for a wrapper instance and copy it.
        //    (C-2)
        //
        // Testing:
        //   formatter(const formatter &);
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING COPY CONSTRUCTOR"
                          "\n========================");

        {
            const bsl::formatter<bslfmt::Streamed<Streamable>, char> dummy_c;
            bsl::formatter<bslfmt::Streamed<Streamable>, char> copy_c(dummy_c);
            (void)copy_c;
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        {
            const std::formatter<bslfmt::Streamed<Streamable>, char> dummy_c;
            std::formatter<bslfmt::Streamed<Streamable>, char> copy_c(dummy_c);
            (void)copy_c;
        }
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING (PRIMITIVE) GENERATORS
        //   The only generator for 'formatter' is the default constructor.
        //
        // Concerns:
        // 1. `bsl::formatter` types for `Streamed` types are default
        //    constructible.
        //
        // 2. `std::formatter` types for `Streamed` types are default
        //    constructible.
        //
        // Plan:
        // 1. Default construct a 'bsl::formatter' for a `Streamed` wrapper type
        //    instance.  (C-1)
        //
        // 2. Default construct a 'std::formatter' for a `Streamed` wrapper type
        //    instance.  (C-1)
        //
        // Testing:
        //   formatter();
        //   ~formatter();
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING (PRIMITIVE) GENERATORS"
                          "\n==============================");

        {
            bsl::formatter<bslfmt::Streamed<Streamable>, char> dummy_c;
            (void)dummy_c;
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        {
            std::formatter<bslfmt::Streamed<Streamable>, char> dummy_c;
            (void)dummy_c;
        }
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        // 1. That basic functionality appears to work as advertised before
        //    before beginning testing in earnest.
        //
        // 2. That warning is generated for wrapped-types that already have a
        //    formatter for `char` (and the type itself) enabled on compilers
        //    that support concepts.
        //
        // 3. Demonstrate the use of the wrapper class template with class
        //    template argument deduction (CTAD) when it is available.
        //
        // Plan:
        // 1. Test formatting wrapped objects.  (C-1)
        //
        // 2. Attempt to format an `int` (if warning demonstration is
        //    requested) and observe if a warning is generated for that line of
        //    the test driver.  (C-2)
        //
        // 3. If CTAD is available use the `Streamed` class without specifying
        //    the wrapped type.
        //
        // Testing:
        //   BREATHING TEST
        //   Streamed(const T&);
        //   streamed(const T&);
        // --------------------------------------------------------------------

        if (verbose) puts("\nBREATHING TEST"
                          "\n==============");

        CanStream        mX("0123456789");
        const CanStream& X = mX;

#define TEST_LINE_BSL(fmt, res)                                               \
    result = bsl::format(fmt, bslfmt::Streamed<CanStream>(X));                \
    ASSERTV(result.c_str(), result == res);                                   \
                                                                              \
    result = bsl::format(fmt, bslfmt::streamed(X));                           \
    ASSERTV(result.c_str(), result == res)

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
  #define TEST_LINE_STD(fmt, res)                                             \
    stdResult = std::format(fmt, bslfmt::Streamed<CanStream>(X));             \
    ASSERTV(stdResult.c_str(), stdResult == res);                             \
                                                                              \
    stdResult = std::format(fmt, bslfmt::streamed(X));                        \
    ASSERTV(stdResult.c_str(), stdResult == res)
#else
  #define TEST_LINE_STD(fmt, res)
#endif

#define TEST_LINE(fmt, res)                                                   \
    TEST_LINE_BSL(fmt, res);                                                  \
    TEST_LINE_STD(fmt, res)

        bsl::string result;
        std::string stdResult;

        TEST_LINE("{:12}",     "0123456789  ");
        TEST_LINE("{:>10.8}",  "  01234567"  );
        TEST_LINE("{:<10.12}", "0123456789"  );
        TEST_LINE("{:*^12}",   "*0123456789*");
        TEST_LINE("{:*^10.7}", "*0123456**"  );

#ifdef BSLFMT_STREAMED_DEMONSTRATE_FORMATTER_EXISTS_WARNING
        {
            // Note that warnings for these two lines will appear only on
            // compilers that support concepts.
            bsl::string s = bsl::format("{}", bslfmt::streamed(12));
            s             = bsl::format("{}", bslfmt::Streamed<int>(12));
        }
#endif  // BSLFMT_STREAMED_DEMONSTRATE_FORMATTER_EXISTS_WARNING

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        {
            // Note that warnings for these two lines will appear only on
            // compilers that support concepts.
            const bsl::string s = bsl::format("{:.10}", bslfmt::Streamed(X));
            ASSERTV(s.c_str(), "0123456789" == s);
        }
#endif  // BSLS_COMPILERFEATURES_SUPPORT_CTAD
      } break;
      default: {
        printf("WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        printf("Error, non-zero test status = %d .\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
