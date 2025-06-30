// bslstl_enablestreamedformatter.t.cpp                               -*-C++-*-
//
#include <bslfmt_enablestreamedformatter.h>

#include <bslfmt_formattertestutil.h>

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>
#include <stdio.h>

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
// Tested is the `bslfmt::EnableStreamedFormatter` nested-compatible boolean
// type trait and its effect of enabling a streaming-based formatter for the
// "flagged" type to make it possible for it to be formatted with `bsl::format`
// and `std::format` if that is available.
//
// Since the trait's effect is implemented in terms of the (tested)
// `bslfmt::StreamedFormatter` we do only minimal verification here.
//-----------------------------------------------------------------------------
// CREATORS
// [ 3] formatter();
// [ 4] ~formatter();
// [ 4] formatter(const formatter &);
//
// MANIPULATORS
// [ 5] operator=(const formatter &);
// [ 6] formatter::parse(PARSE_CONTEXT&);
//
// ACCESSORS
// [ 7] formatter::format(TYPE, FORMAT_CONTEXT&);
//
// TRAIT
// [ 2] bslfmt::EnableStreamedFormatter
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE
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
        const bool  rv =                                                      \
                     bslfmt::FormatterTestUtil<char>::testParseFailure<type>( \
                         &errorMsg,                                           \
                         false,                                               \
                         fmtStr);                                             \
        if (!rv) {                                                            \
            ASSERTV(errorMsg.c_str(), fmtStr, rv);                            \
        }                                                                     \
    } while (false)

#define TEST_PARSE_SUCCESS_F(type, fmtStr)                                    \
    do {                                                                      \
        bsl::string errorMsg;                                                 \
        const bool  rv =                                                      \
            bslfmt::FormatterTestUtil<char>::testParseFormat<type>(&errorMsg, \
                                                                   false,     \
                                                                   fmtStr);   \
        if (!rv) {                                                            \
            ASSERTV(errorMsg.c_str(), fmtStr, rv);                            \
        }                                                                     \
    } while (false)

#define TEST_PARSE_SUCCESS_VF(type, fmtStr)                                   \
    do {                                                                      \
        bsl::string errorMsg;                                                 \
        const bool  rv =                                                      \
                     bslfmt::FormatterTestUtil<char>::testParseVFormat<type>( \
                         &errorMsg,                                           \
                         false,                                               \
                         fmtStr);                                             \
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

                     // =================================
                     // struct CanStreamFormattingEnabled
                     // =================================

struct CanStreamFormattingEnabled : CanStream {
    // CREATORS
    explicit CanStreamFormattingEnabled(const char *content)
    : CanStream(content)
    {
    }

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(CanStreamFormattingEnabled,
                                   bslfmt::EnableStreamedFormatter);
};

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Enabling Formatting of a Streamable Type
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we own a simple type that supports `ostream` insert `operator<<` and
// want to quickly add `bsl::format`ing capability to it, based on streaming.
//
// First, we introduce a type with a streaming operator but without a formatter
// that enables `bsl::format` use:
//```
    class NonFormattableType {};

    std::ostream& operator<<(std::ostream& os, const NonFormattableType&)
    {
        return os << "The printout";
    }

    // The following would not compile:
    //
    // const NonFormattableType noFormatObj;
    // bsl::string s = bsl::format("{}", noFormatObj);
//```
// Then, we enable formatting using the trait (notice the type name changed):
//```
    class NowFormattableType {
      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(NowFormattableType,
                                       bslfmt::EnableStreamedFormatter);
    };

    std::ostream& operator<<(std::ostream& os, const NowFormattableType&)
    {
        return os << "The printout";
    }
//```
// Next, we create an instance of this type and use `bsl::format` to format it:
//```
//  const NowFormattableType obj;
//  bsl::string s = bsl::format("{}", obj);
//```
// Finally, we verify the output is correct:
//```
//  assert(s == "The printout");
//```

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    int  test        = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose     = argc > 2;
    bool veryVerbose = argc > 3;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 8: {
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

///Enabling Formatting of a Streamable Type
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we own a simple type that supports `ostream` insert `operator<<` and
// want to quickly add `bsl::format`ing capability to it, based on streaming.
//
// First, we introduce a type with a streaming operator but without a formatter
// that enables `bsl::format` use:
//```
//  class NonFormattableType {};
//
//  std::ostream& operator<<(std::ostream& os, const NonFormattableType&)
//  {
//      return os << "The printout";
//  }
//
//  // The following would not compile:
//  //
//  // const NonFormattableType noFormatObj;
//  // bsl::string s = bsl::format("{}", noFormatObj);
//```
// Then, we enable formatting using the trait (notice the type name changed):
//```
//  class NowFormattableType {
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(NowFormattableType,
//                                     bslfmt::EnableStreamedFormatter);
//  };
//
//  std::ostream& operator<<(std::ostream& os, const NowFormattableType&)
//  {
//      return os << "The printout";
//  }
//```
// Next, we create an instance of this type and use `bsl::format` to format it:
//```
    const NowFormattableType obj;
    bsl::string s = bsl::format("{}", obj);
//```
// Finally, we verify the output is correct:
//```
    ASSERT(s == "The printout");
//```
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING format(VALUE, FORMAT_CONTEXT&)
        //
        // Concerns:
        // 1. Trait enables `StreamedFormatter`-based formatters.
        //
        // Plan:
        // 1. This test is done in the breathing test.
        //
        // Testing:
        //   formatter::format(VALUE, FORMAT_CONTEXT&);
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING format(VALUE, FORMAT_CONTEXT&)"
                          "\n======================================");

        if (veryVerbose)
            puts("This verification is covered by the breathing test.\n");
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING parse(PARSE_CONTEXT&)
        //
        // Concerns:
        // 1. Invalid format specs will generate a parse error
        //
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
        //   formatter::parse(PARSE_CONTEXT&);
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING parse(PARSE_CONTEXT&)"
                          "\n=============================");

        // Bad fill character
        // Note can only test '{' as '}' closes the parse string.
        TEST_PARSE_FAIL(CanStreamFormattingEnabled, "{:{<5.5s}");

        // A selection of valid format strings (non-Unicode)
        TEST_PARSE_SUCCESS_F(CanStreamFormattingEnabled, "{0:*^{1}.{1}}");

#if defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE <= 13
  #define TPS TEST_PARSE_SUCCESS_VF
#else
  #define TPS TEST_PARSE_SUCCESS_F
#endif

        // A selection of valid format strings (Unicode)
        TPS(CanStreamFormattingEnabled, "{0:\xF0\x9F\x98\x80^{1}.{1}}");
#undef TPS
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //
        // Concerns:
        // 1. We can copy assign 'bsl::formatter' types for trait-enabled
        //    formatters.
        //
        // 2. We can copy assign 'std::formatter' types for trait-enabled
        //    formatters.
        //
        // Plan:
        // 1. Construct two 'bsl::formatter's for `CanStreamFormattingEnabled`
        //    specializations, and assign one to the other.  (C-1)
        //
        // 2. Construct two 'std::formatter's for `CanStreamFormattingEnabled`
        //    specializations, and assign one to the other.  (C-2)
        //
        // Testing:
        //   operator=(const formatter &);
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING ASSIGNMENT OPERATOR"
                          "\n===========================");

        {
            const bsl::formatter<CanStreamFormattingEnabled, char> dummy_c;
            bsl::formatter<CanStreamFormattingEnabled, char>       dummy2_c;
            dummy2_c = dummy_c;
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        {
            const std::formatter<CanStreamFormattingEnabled, char> dummy_c;
            std::formatter<CanStreamFormattingEnabled, char>       dummy2_c;
            dummy2_c = dummy_c;
        }
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //
        // Concerns:
        // 1. We can copy construct 'bsl::formatter' types of trait-enabled
        //    instances.
        //
        // 2. We can copy construct 'std::formatter' types of trait-enabled
        //    instances.
        //
        // Plan:
        // 1. Construct a 'bsl::formatter' for `CanStreamFormattingEnabled`
        //    and copy it.  (C-1)
        //
        // 2. Construct a 'std::formatter' for `CanStreamFormattingEnabled`
        //    and copy it.  (C-2)
        //
        // Testing:
        //   formatter(const formatter &);
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING COPY CONSTRUCTOR"
                          "\n========================");

        {
            const bsl::formatter<CanStreamFormattingEnabled, char> dummy_c;
            bsl::formatter<CanStreamFormattingEnabled, char> copy_c(dummy_c);
            (void)copy_c;
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        {
            const std::formatter<CanStreamFormattingEnabled, char> dummy_c;
            std::formatter<CanStreamFormattingEnabled, char> copy_c(dummy_c);
            (void)copy_c;
        }
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING (PRIMITIVE) GENERATORS
        //   The only generator for 'formatter' is the default constructor.
        //
        // Concerns:
        // 1. `bsl::formatter` types for trait-enabled types are default
        //    constructible.
        //
        // 2. `std::formatter` types for trait-enabled types are default
        //    constructible.
        //
        // Plan:
        // 1. Default construct a 'bsl::formatter' for
        //    `CanStreamFormattingEnabled` instance.  (C-1)
        //
        // 2. Default construct a 'std::formatter' for
        //    `CanStreamFormattingEnabled` instance.  (C-2)
        //
        // Testing:
        //   formatter();
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING (PRIMITIVE) GENERATORS"
                          "\n==============================");

        {
            bsl::formatter<CanStreamFormattingEnabled, char> dummy_c;
            (void)dummy_c;
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        {
            std::formatter<CanStreamFormattingEnabled, char> dummy_c;
            (void)dummy_c;
        }
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TRAITS INDUCED FORMATTING
        //
        // Concerns:
        // 1. `bslfmt::StreamedEnableFormatterTrait::value` is `false` for
        //     types for which the trait is not set, while it is `true` for
        //     those types that have the trait explicitly set.
        //
        // 2. Types with the trait set can be formatted.
        //
        // Plan:
        // 1. Verify that `CanStream` has `false` trait value and
        //    `CanStreamFormattingEnabled` has `true` trait value.  (C-1)
        //
        // 2. Verify that `CanStreamFormattingEnabled` can be formatted.  (C-2)
        //
        // Testing:
        //   bslfmt::EnableStreamedFormatter
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING TRAITS INDUCED FORMATTING"
                          "\n=================================");

        ASSERT(!bslfmt::EnableStreamedFormatter<CanStream>::value);
        ASSERT(
            bslfmt::EnableStreamedFormatter<CanStreamFormattingEnabled>::value
        );

        const bsl::string s = bsl::format("{}",
                                          CanStreamFormattingEnabled("works"));
        ASSERTV(s.c_str(), "works" == s);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        // 1. That basic functionality appears to work as advertised before
        //    before beginning testing in earnest.
        //
        // Plan:
        // 1. Test formatting wrapped objects.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) puts("\nBREATHING TEST"
                          "\n==============");

        CanStreamFormattingEnabled        mX("0123456789");
        const CanStreamFormattingEnabled& X = mX;

#define TEST_LINE_BSL(fmt, res)                                               \
    result = bsl::format(fmt, X);                                             \
    ASSERTV(result.c_str(), result == res)

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
  #define TEST_LINE_STD(fmt, res)                                             \
    stdResult = std::format(fmt, X);                                          \
    ASSERTV(stdResult.c_str(), stdResult == res)
#else
  #define TEST_LINE_STD(fmt, res)
#endif

#define TEST_LINE(fmt, res)                                                   \
    TEST_LINE_BSL(fmt, res);                                                  \
    TEST_LINE_STD(fmt, res)

        bsl::string result;
        std::string stdResult;

        TEST_LINE("{:^12}",    " 0123456789 ");
        TEST_LINE("{:<10.8}",  "01234567  "  );
        TEST_LINE("{:10.12}",  "0123456789"  );
        TEST_LINE("{:*>12}",   "**0123456789");
        TEST_LINE("{:*>10.7}", "***0123456"  );
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
