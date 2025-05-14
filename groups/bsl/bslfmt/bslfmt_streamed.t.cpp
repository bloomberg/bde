// bslstl_streamed.t.cpp                                              -*-C++-*-
#define BB_DEPRECATE_ENABLE_ALL_DEPRECATIONS_FOR_TESTING                      1

#include <bslfmt_streamed.h>

#include <bslfmt_format.h>

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <stdio.h>

#include <iostream>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
  #include <format>
  #include <string>
#endif

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// Tested are the wrapper class template `bslfmt::Streamed<t_STREAMED>` as well
// as the wrapper-creator function template (mainly for C++03 compatibility)
// `bslfmt::streamed`.
//-----------------------------------------------------------------------------
// [  ] TBD
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE
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
//                               TEST TYPES
//-----------------------------------------------------------------------------

struct CanStream {
    const char *d_content;

    // CREATORS
    explicit CanStream(const char *content)
    : d_content(content)
    {
    }

    // ACCESSORS
    const char *content() const { return d_content; }
};

std::ostream& operator<<(std::ostream& os, const CanStream& obj)
{
    return os << obj.d_content;
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Example 1: Formatting a Streamable Object Using the Function
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<`.  When writing portable code that
// should work on compilers that do not support class template argument
// deduction we would use the wrapper-creator function `bslfmt::streamed` to
// avoid having to know and write the type of the object.
//
// First, for the sake of demonstration we create a type with an obscure and
// long name that we neither want to remember nor ever to write down, and which
// can be streamed out:
//
//```
    class ThisTypeHasLongAndObscureNameButStreamable {
    };

    std::ostream& operator<<(
                         std::ostream&                                     os,
                         const ThisTypeHasLongAndObscureNameButStreamable& )
    {
        return os << "The printout";
    }
//```

///Example 2: Formatting with CTAD support
///- - - - - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<` and we target only modern compilers.
// In such case the wrapper class template can be used directly, without the
// need for the function.
//
// First, for the sake of demonstration we create a type with an obscure and
// long name that we neither want to remember nor ever to write down, and which
// can be streamed out:
//
//```
//  class ThisTypeHasLongAndObscureNameButStreamable {
//  };
//
//  std::ostream& operator<<(
//                       std::ostream&                                     os,
//                       const ThisTypeHasLongAndObscureNameButStreamable& )
//  {
//      return os << "The printout";
//  }
//```

///Example 3: Format String Options
/// - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<` for an environment that requires the
// output to be padded, aligned, and/or truncation of the output.  In this
// example we will introduce the effects of the various possible format string
// specifications.
//
// First, for the sake of demonstration we create a type that prints a series
// of digits to help demonstrate the effects of the various formattings:
//
//```
    class Streamable {
    };

    std::ostream& operator<<(std::ostream& os, const Streamable&)
    {
        return os << "12345678";
    }
//```

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    const int  test        = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose     = argc > 2;
    const bool veryVerbose = argc > 3;
    (void) veryVerbose;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate the functioning of this component.
        //
        // Plan:
        //: 1 Use test contexts to format a single string.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");

        {
///Example 1: Formatting a Streamable Object Using the Function
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<`.  When writing portable code that
// should work on compilers that do not support class template argument
// deduction we would use the wrapper-creator function `bslfmt::streamed` to
// avoid having to know and write the type of the object.
//
// First, for the sake of demonstration we create a type with an obscure and
// long name that we neither want to remember nor ever to write down, and which
// can be streamed out:
//
//```
//  class ThisTypeHasLongAndObscureNameButStreamable {
//  };
//
//  std::ostream& operator<<(
//                       std::ostream&                                     os,
//                       const ThisTypeHasLongAndObscureNameButStreamable& )
//  {
//      return os << "The printout";
//  }
//```
//
// Then, we create an object of said type that we want to print out:
//
//```
    const ThisTypeHasLongAndObscureNameButStreamable obj;
//```
// Next, we format the "value" using `bsl::format` with the wrapper-creator
// function:
//
//```
    bsl::string s = bsl::format("{}", bslfmt::streamed(obj));
//```
// Finally, we verify the output is correct:
//
//```
    ASSERT(s == "The printout");
//```
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
        {
///Example 2: Formatting with CTAD support
///- - - - - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<` and we target only modern compilers.
// In such case the wrapper class template can be used directly, without the
// need for the function.
//
// First, for the sake of demonstration we create a type with an obscure and
// long name that we neither want to remember nor ever to write down, and which
// can be streamed out:
//
//```
//  class ThisTypeHasLongAndObscureNameButStreamable {
//  };
//
//  std::ostream& operator<<(
//                       std::ostream&                                     os,
//                       const ThisTypeHasLongAndObscureNameButStreamable& )
//  {
//      return os << "The printout";
//  }
//```
//
// Then, we create an object of said type that we want to print out:
//
//```
    const ThisTypeHasLongAndObscureNameButStreamable obj;
//```
// Next, we format the "value" using `bsl::format` with the wrapper class
// template, class template argument deduction takes care of the type:
//
//```
    bsl::string s = bsl::format("{}", bslfmt::Streamed(obj));
//```
// Finally, we verify the output is correct:
//
//```
    ASSERT(s == "The printout");
//```
        }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT

        {
///Example 3: Format String Options
/// - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<` for an environment that requires the
// output to be padded, aligned, and/or truncation of the output.  In this
// example we will introduce the effects of the various possible format string
// specifications.
//
// First, for the sake of demonstration we create a type that prints a series
// of digits to help demonstrate the effects of the various formattings:
//
//```
//  class Streamable {
//  };
//
//  std::ostream& operator<<(std::ostream& os, const Streamable&)
//  {
//      return os << "12345678";
//  }
//```
//
// Then, we create an object of said type that we will format:
//
//```
    const Streamable obj;
//```
//
// Next, we format the "value" using many different format strings, starting
// with the default for completeness:
//```
    bsl::string s = bsl::format("{}", bslfmt::streamed(obj));
    ASSERT(s == "12345678");
//```
//
// Then, we format with specifying just a width:
//```
    s = bsl::format("{:10}", bslfmt::streamed(obj));
    ASSERT(s == "12345678  ");
//```
//
// Next, we format with specifying a width, and alignments:
//```
    s = bsl::format("{:<10}", bslfmt::streamed(obj));
    ASSERT(s == "12345678  ");

    s = bsl::format("{:^10}", bslfmt::streamed(obj));
    ASSERT(s == " 12345678 ");

    s = bsl::format("{:>10}", bslfmt::streamed(obj));
    ASSERT(s == "  12345678");
//```
//
// Finally, we demonstrate the truncation using a "precision" value:
//```
    s = bsl::format("{:.6}", bslfmt::streamed(obj));
    ASSERT(s == "123456");

    s = bsl::format("{:8.6}", bslfmt::streamed(obj));
    ASSERT(s == "123456  ");

    s = bsl::format("{:<8.6}", bslfmt::streamed(obj));
    ASSERT(s == "123456  ");

    s = bsl::format("{:^8.6}", bslfmt::streamed(obj));
    ASSERT(s == " 123456 ");

    s = bsl::format("{:>8.6}", bslfmt::streamed(obj));
    ASSERT(s == "  123456");
//```
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 That basic functionality appears to work as advertised before
        //:   before beginning testing in earnest:
        //:   - default and copy constructors
        //:   - assignment operator
        //:   - primary manipulators, basic accessors
        //:   - 'operator==', 'operator!='
        //
        // Plan:
        //: 1 Test all public methods mentioned in concerns.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
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

        TEST_LINE("{}",     "0123456789"  );
        TEST_LINE("{:12}",  "0123456789  ");
        TEST_LINE("{:<12}", "0123456789  ");
        TEST_LINE("{:>12}", "  0123456789");
        TEST_LINE("{:^12}", " 0123456789 ");

        TEST_LINE("{:10.8}",  "01234567  ");
        TEST_LINE("{:<10.8}", "01234567  ");
        TEST_LINE("{:>10.8}", "  01234567");
        TEST_LINE("{:^10.8}", " 01234567 ");

        TEST_LINE("{:10.12}",  "0123456789");
        TEST_LINE("{:<10.12}", "0123456789");
        TEST_LINE("{:<10.12}", "0123456789");
        TEST_LINE("{:^10.12}", "0123456789");

        bsl::string s = bsl::format("{}", bslfmt::streamed(12));
      } break;
      default: {
        printf("WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

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
