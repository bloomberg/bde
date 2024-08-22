// bslstl_formatterstring.t.cpp                                       -*-C++-*-
#include <bslfmt_formatterstring.h>

#include <bsls_bsltestutil.h>

#include <bslstl_algorithm.h>
#include <bslstl_string.h>

#include <stdio.h>
#include <string.h>

#include <bslfmt_formatimp.h> // Testing only
#include <bslfmt_formattertestutil.h> // Testing only

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'bsl::formatter<t_TYPE, t_CHAR>' where t_TYPE is a string type is a
// partial specialization of 'bsl::formatter' to enable formatting of
// strings. It is hard to test standalone as the interface is designed to
// be called by the 'bslfmt::format' suite of functions, and such testing
// requires the creation of "mock" contexts. The 'format' and 'parse'
// functions are designed to be called in order, and it is not possible to
// test a successful parse individually in isolation. As a result test 10
// ('parse') will focus on parsing failures while parsing successes will be
// tested in test 11 ('format').
// 
// It should meet the requirements specified in [format.string.std].
//
//-----------------------------------------------------------------------------
// CLASS 'bsl::formtter'
//
// CREATORS
// [ 3] formatter();
// [ 3] ~formatter();
// [ 7] formatter(const formatter &);
//
// MANIPULATORS
// [ 9] operator=(const formatter &);
// [11] parse(PARSE_CONTEXT&);
//
// ACCESSORS
// [12] format(TYPE, FORMAT_CONTEXT&);
//
// FREE FUNCTIONS
// [ 8] swap(formatter &, formatter&);
//
// OUTPUT
// [ 5] parsing failures
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TESTING PRIMARY MANIPULATORS: Not Applicable
// [ 4] TESTING BASIC ACCESSORS:      Not Applicable
// [ 5] TESTING OUTPUT:               Not Applicable
// [ 6] TESTING EQUALITY OPERATOR:    Not Applicable
// [10] STREAMING FUNCTIONALITY:      Not Applicable
// [13] USAGE EXAMPLE
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


#define TEST_PARSE_FAIL(type, fmtStr, useOracle)                               \
    {                                                                          \
        bsl::string errorMsg;                                                  \
        bool rv;                                                               \
        rv = bslfmt::Formatter_TestUtil<type>::                                \
                 testParseFailure<const type *>(&errorMsg, fmtStr, useOracle); \
        if (!rv) {                                                             \
            bsl::basic_string<type> formatStr(fmtStr);                         \
            ASSERTV(errorMsg.c_str(), formatStr.c_str(), rv);                  \
        }                                                                      \
    }


// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------


// ============================================================================
//                     GLOBAL HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const int  test        = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose     = argc > 2;
    const bool veryVerbose = argc > 3;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 11: {
        // -----------------------------------------------
        // TESTING parse(PARSE_CONTEXT&);
        //
        // Testing:
        //   parse(PARSE_CONTEXT&);
        // -----------------------------------------------
        if (verbose)
            printf("\nTESTING parse(PARSE_CONTEXT&);"
                   "\n==============================\n");

        typedef char C;
        typedef wchar_t W;

        // Bad fill character
        // Note can only test '{' as '}' closes the parse string.
        TEST_PARSE_FAIL(char,     "{:{<5.5s}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{:{<5.5s}",    true);

        // Missing fill specifier
        TEST_PARSE_FAIL(char,     "{:*5.5s}",     true);
        TEST_PARSE_FAIL(wchar_t, L"{:*5.5s}",     true);

        // Sign
        TEST_PARSE_FAIL(char,     "{:*< 5.5s}",   true);
        TEST_PARSE_FAIL(char,     "{:*<+5.5s}",   true);
        TEST_PARSE_FAIL(char,     "{:*<-5.5s}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*< 5.5s}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<+5.5s}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<-5.5s}",   true);

        // Alternative option
        TEST_PARSE_FAIL(char,     "{:*<#5.5s}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<#5.5s}",   true);

        // Zero pad option
        TEST_PARSE_FAIL(char,     "{:*<05.5s}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<05.5s}",   true);

        // Locale option
        TEST_PARSE_FAIL(char,     "{:*<5.5Ls}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<5.5Ls}",   true);

        // Escaped string type
        // Not supported in bslfmt at all or in std before C++23
        TEST_PARSE_FAIL(char,     "{:*<5.5?}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<5.5?}",    true);

        // Non-string type
        TEST_PARSE_FAIL(char,     "{:*<5.5d}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<5.5d}",    true);

        // Non-numeric width or precision
        TEST_PARSE_FAIL(char,     "{:*< X.5s}",   true);
        TEST_PARSE_FAIL(char,     "{:*<+5.Xs}",   true);
        TEST_PARSE_FAIL(char,     "{:*<-X.Xs}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*< X.5s}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<+5.Xs}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<-X.Xs}",   true);

        // Missing precision marker
        TEST_PARSE_FAIL(char,     "{:*<{}{}s}",      false);
        TEST_PARSE_FAIL(char,     "{0:*<{3}{2}s}",   false);
        TEST_PARSE_FAIL(wchar_t, L"{:*<{}{}s}",      false);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{3}{2}s}",   false);

        // Mixed numeric and non-numeric nested args
        TEST_PARSE_FAIL(char,     "{:*<{}.{2}s}",     true);
        TEST_PARSE_FAIL(char,     "{:*<{1}.{}s}",     true);
        TEST_PARSE_FAIL(char,     "{:*<{1}.{2}s}",    true);
        TEST_PARSE_FAIL(char,     "{0:*<{}.{}s}",     true);
        TEST_PARSE_FAIL(char,     "{0:*<{}.{2}s}",    true);
        TEST_PARSE_FAIL(char,     "{0:*<{1}.{}s}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<{}.{2}s}",     true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<{1}.{}s}",     true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<{1}.{2}s}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{}.{}s}",     true);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{}.{2}s}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{1}.{}s}",    true);

        // Nested args out of range
        // Not checked in std parsing
        TEST_PARSE_FAIL(char,     "{0:*<{1}.{3}s}",   false);
        TEST_PARSE_FAIL(char,     "{0:*<{3}.{2}s}",   false);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{1}.{3}s}",   false);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{3}.{2}s}",   false);
      } break;
      case 10: {
        // -----------------------------------------------
        // TESTING STREAMING FUNCTIONALITY: Not Applicable
        //
        // Testing:
        //   STREAMING FUNCTIONALITY: Not Applicable
        // -----------------------------------------------
        if (verbose)
            printf("\nSTREAMING FUNCTIONALITY: Not Applicable"
                   "\n=======================================\n");
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //
        // Concerns:
        //: 1 We can swap two 'bsl::formatter' types for all the string
        //:   specializations.
        //:
        //: 2 We can swap two 'std::formatter' types for those partial
        //:   specializations we expect to be aliased into the 'std' namespace.
        //
        // Plan:
        //: 1 Construct two 'bsl::formatter's for each of the partial
        //:   specializations, and assign one to the other. (C-1)
        //:
        //: 2 Construct two 'std::formatter' for each of the partial
        //:   specializations that we promote to 'std' and assign one to the
        //:   other. (C-2)
        //
        // Testing:
        //   operator=(const formatter &);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING SWAP"
                   "\n============\n");

        typedef char    C;
        typedef wchar_t W;

        if (verbose)
            printf("\nValidating bslfmt swap\n");

        {
            bsl::formatter<const C *, C> dummy_c;
            bsl::formatter<const W *, W> dummy_w;
            bsl::formatter<const C *, C> dummy2_c;
            bsl::formatter<const W *, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<C *, C> dummy_c;
            bsl::formatter<W *, W> dummy_w;
            bsl::formatter<C *, C> dummy2_c;
            bsl::formatter<W *, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<C[10], C> dummy_c;
            bsl::formatter<W[10], W> dummy_w;
            bsl::formatter<C[10], C> dummy2_c;
            bsl::formatter<W[10], W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<bsl::basic_string<C>, C> dummy_c;
            bsl::formatter<bsl::basic_string<W>, W> dummy_w;
            bsl::formatter<bsl::basic_string<C>, C> dummy2_c;
            bsl::formatter<bsl::basic_string<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<std::basic_string<C>, C> dummy_c;
            bsl::formatter<std::basic_string<W>, W> dummy_w;
            bsl::formatter<std::basic_string<C>, C> dummy2_c;
            bsl::formatter<std::basic_string<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<bsl::basic_string_view<C>, C> dummy_c;
            bsl::formatter<bsl::basic_string_view<W>, W> dummy_w;
            bsl::formatter<bsl::basic_string_view<C>, C> dummy2_c;
            bsl::formatter<bsl::basic_string_view<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<std::basic_string_view<C>, C> dummy_c;
            bsl::formatter<std::basic_string_view<W>, W> dummy_w;
            bsl::formatter<std::basic_string_view<C>, C> dummy2_c;
            bsl::formatter<std::basic_string_view<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        if (verbose)
            printf("\nValidating std swap\n");

        {
            std::formatter<const C *, C> dummy_c;
            std::formatter<const W *, W> dummy_w;
            std::formatter<const C *, C> dummy2_c;
            std::formatter<const W *, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<C *, C> dummy_c;
            std::formatter<W *, W> dummy_w;
            std::formatter<C *, C> dummy2_c;
            std::formatter<W *, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<C[10], C> dummy_c;
            std::formatter<W[10], W> dummy_w;
            std::formatter<C[10], C> dummy2_c;
            std::formatter<W[10], W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<bsl::basic_string<C>, C> dummy_c;
            std::formatter<bsl::basic_string<W>, W> dummy_w;
            std::formatter<bsl::basic_string<C>, C> dummy2_c;
            std::formatter<bsl::basic_string<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<std::basic_string<C>, C> dummy_c;
            std::formatter<std::basic_string<W>, W> dummy_w;
            std::formatter<std::basic_string<C>, C> dummy2_c;
            std::formatter<std::basic_string<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<bsl::basic_string_view<C>, C> dummy_c;
            std::formatter<bsl::basic_string_view<W>, W> dummy_w;
            std::formatter<bsl::basic_string_view<C>, C> dummy2_c;
            std::formatter<bsl::basic_string_view<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<std::basic_string_view<C>, C> dummy_c;
            std::formatter<std::basic_string_view<W>, W> dummy_w;
            std::formatter<std::basic_string_view<C>, C> dummy2_c;
            std::formatter<std::basic_string_view<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
#endif
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING SWAP
        //
        // Concerns:
        //: 1 We can swap two 'bsl::formatter' types for all the string
        //:   specializations.
        //:
        //: 2 We can swap two 'std::formatter' types for those partial
        //:   specializations we expect to be aliased into the 'std' namespace.
        //
        // Plan:
        //: 1 Construct two 'bsl::formatter's for each of the partial
        //:   specializations, and swap them. (C-1)
        //:
        //: 2 Construct two 'std::formatter' for each of the partial
        //:   specializations that we promote to 'std' and swap them. (C-2)
        //
        // Testing:
        //   formatter(const formatter &);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING SWAP"
                   "\n============\n");

        typedef char    C;
        typedef wchar_t W;

        if (verbose)
            printf("\nValidating bslfmt swap\n");

        {
            bsl::formatter<const C *, C> dummy_c;
            bsl::formatter<const W *, W> dummy_w;
            bsl::formatter<const C *, C> dummy2_c;
            bsl::formatter<const W *, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<C *, C> dummy_c;
            bsl::formatter<W *, W> dummy_w;
            bsl::formatter<C *, C> dummy2_c;
            bsl::formatter<W *, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<C[10], C> dummy_c;
            bsl::formatter<W[10], W> dummy_w;
            bsl::formatter<C[10], C> dummy2_c;
            bsl::formatter<W[10], W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<bsl::basic_string<C>, C> dummy_c;
            bsl::formatter<bsl::basic_string<W>, W> dummy_w;
            bsl::formatter<bsl::basic_string<C>, C> dummy2_c;
            bsl::formatter<bsl::basic_string<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<std::basic_string<C>, C> dummy_c;
            bsl::formatter<std::basic_string<W>, W> dummy_w;
            bsl::formatter<std::basic_string<C>, C> dummy2_c;
            bsl::formatter<std::basic_string<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<bsl::basic_string_view<C>, C> dummy_c;
            bsl::formatter<bsl::basic_string_view<W>, W> dummy_w;
            bsl::formatter<bsl::basic_string_view<C>, C> dummy2_c;
            bsl::formatter<bsl::basic_string_view<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<std::basic_string_view<C>, C> dummy_c;
            bsl::formatter<std::basic_string_view<W>, W> dummy_w;
            bsl::formatter<std::basic_string_view<C>, C> dummy2_c;
            bsl::formatter<std::basic_string_view<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        if (verbose)
            printf("\nValidating std swap\n");

        {
            std::formatter<const C *, C> dummy_c;
            std::formatter<const W *, W> dummy_w;
            std::formatter<const C *, C> dummy2_c;
            std::formatter<const W *, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<C *, C> dummy_c;
            std::formatter<W *, W> dummy_w;
            std::formatter<C *, C> dummy2_c;
            std::formatter<W *, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<C[10], C> dummy_c;
            std::formatter<W[10], W> dummy_w;
            std::formatter<C[10], C> dummy2_c;
            std::formatter<W[10], W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<bsl::basic_string<C>, C> dummy_c;
            std::formatter<bsl::basic_string<W>, W> dummy_w;
            std::formatter<bsl::basic_string<C>, C> dummy2_c;
            std::formatter<bsl::basic_string<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<std::basic_string<C>, C> dummy_c;
            std::formatter<std::basic_string<W>, W> dummy_w;
            std::formatter<std::basic_string<C>, C> dummy2_c;
            std::formatter<std::basic_string<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<bsl::basic_string_view<C>, C> dummy_c;
            std::formatter<bsl::basic_string_view<W>, W> dummy_w;
            std::formatter<bsl::basic_string_view<C>, C> dummy2_c;
            std::formatter<bsl::basic_string_view<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<std::basic_string_view<C>, C> dummy_c;
            std::formatter<std::basic_string_view<W>, W> dummy_w;
            std::formatter<std::basic_string_view<C>, C> dummy2_c;
            std::formatter<std::basic_string_view<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //
        // Concerns:
        //: 1 We can copy construct 'bsl::formatter' types for all the string
        //:   specializations.
        //:
        //: 2 We can copy construct 'std::formatter' types for those partial
        //:   specializations we expect to be aliased into the 'std' namespace.
        //
        // Plan:
        //: 1 Construct a 'bsl::formatter' for each of the partial
        //:   specializations, and copy it. (C-1)
        //:
        //: 2 Construct a 'std::formatter' for each of the partial
        //:   specializations that we promote to 'std' and copy it. (C-2)
        //
        // Testing:
        //   formatter(const formatter &);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING COPY CONSTRUCTOR"
                   "\n========================\n");

        typedef char    C;
        typedef wchar_t W;

        if (verbose)
            printf("\nValidating bslfmt copy construction\n");

        {
            bsl::formatter<const C *, C> dummy_c;
            bsl::formatter<const W *, W> dummy_w;
            bsl::formatter<const C *, C> copy_c(dummy_c);
            bsl::formatter<const W *, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            bsl::formatter<C *, C> dummy_c;
            bsl::formatter<W *, W> dummy_w;
            bsl::formatter<C *, C> copy_c(dummy_c);
            bsl::formatter<W *, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            bsl::formatter<C[10], C> dummy_c;
            bsl::formatter<W[10], W> dummy_w;
            bsl::formatter<C[10], C> copy_c(dummy_c);
            bsl::formatter<W[10], W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            bsl::formatter<bsl::basic_string<C>, C> dummy_c;
            bsl::formatter<bsl::basic_string<W>, W> dummy_w;
            bsl::formatter<bsl::basic_string<C>, C> copy_c(dummy_c);
            bsl::formatter<bsl::basic_string<W>, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            bsl::formatter<std::basic_string<C>, C> dummy_c;
            bsl::formatter<std::basic_string<W>, W> dummy_w;
            bsl::formatter<std::basic_string<C>, C> copy_c(dummy_c);
            bsl::formatter<std::basic_string<W>, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            bsl::formatter<bsl::basic_string_view<C>, C> dummy_c;
            bsl::formatter<bsl::basic_string_view<W>, W> dummy_w;
            bsl::formatter<bsl::basic_string_view<C>, C> copy_c(dummy_c);
            bsl::formatter<bsl::basic_string_view<W>, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            bsl::formatter<std::basic_string_view<C>, C> dummy_c;
            bsl::formatter<std::basic_string_view<W>, W> dummy_w;
            bsl::formatter<std::basic_string_view<C>, C> copy_c(dummy_c);
            bsl::formatter<std::basic_string_view<W>, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        if (verbose)
            printf("\nValidating std copy construction\n");

        {
            std::formatter<const C *, C> dummy_c;
            std::formatter<const W *, W> dummy_w;
            std::formatter<const C *, C> copy_c(dummy_c);
            std::formatter<const W *, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            std::formatter<C *, C> dummy_c;
            std::formatter<W *, W> dummy_w;
            std::formatter<C *, C> copy_c(dummy_c);
            std::formatter<W *, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            std::formatter<C[10], C> dummy_c;
            std::formatter<W[10], W> dummy_w;
            std::formatter<C[10], C> copy_c(dummy_c);
            std::formatter<W[10], W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            std::formatter<bsl::basic_string<C>, C> dummy_c;
            std::formatter<bsl::basic_string<W>, W> dummy_w;
            std::formatter<bsl::basic_string<C>, C> copy_c(dummy_c);
            std::formatter<bsl::basic_string<W>, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            std::formatter<std::basic_string<C>, C> dummy_c;
            std::formatter<std::basic_string<W>, W> dummy_w;
            std::formatter<std::basic_string<C>, C> copy_c(dummy_c);
            std::formatter<std::basic_string<W>, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            std::formatter<bsl::basic_string_view<C>, C> dummy_c;
            std::formatter<bsl::basic_string_view<W>, W> dummy_w;
            std::formatter<bsl::basic_string_view<C>, C> copy_c(dummy_c);
            std::formatter<bsl::basic_string_view<W>, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            std::formatter<std::basic_string_view<C>, C> dummy_c;
            std::formatter<std::basic_string_view<W>, W> dummy_w;
            std::formatter<std::basic_string_view<C>, C> copy_c(dummy_c);
            std::formatter<std::basic_string_view<W>, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
#endif
      } break;
      case 6: {
        // --------------------------------------------
        // TESTING EQUALITY OPERATOR: Not Applicable
        //
        // Testing:
        //   EQUALITY OPERATOR: Not Applicable
        // --------------------------------------------
        if (verbose)
            printf("\nEQUALITY OPERATOR: Not Applicable"
                   "\n= ===============================\n");
      } break;
      case 5: {
        // --------------------------------------------
        // TESTING OUTPUT: Not Applicable
        //
        // Testing:
        //   OUTPUT: Not Applicable
        // --------------------------------------------
        if (verbose)
            printf("\nOUTPUT: Not Applicable"
                   "\n======================\n");
      } break;
      case 4: {
        // --------------------------------------------
        // TESTING BASIC ACCESSORS: Not Applicable
        //
        // Testing:
        //   BASIC ACCESSORS: Not Applicable
        // --------------------------------------------
        if (verbose)
            printf("\nBASIC ACCESSORS: Not Applicable"
                   "\n===============================\n");
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING (PRIMITIVE) GENERATORS
        //   The only generators for 'formatter' is the default constructor.
        //
        // Concerns:
        //: 1 We can construct 'bsl::formatter' types for all the string
        //:   specializations.
        //:
        //: 2 We can construct 'std::formatter' types for those partial
        //:   specializations we expect to be aliased into the 'std' namespace.
        //
        // Plan:
        //: 1 Construct a 'bsl::formatter' for each of the partial
        //:   specializations. (C-1)
        //:
        //: 2 Construct a 'std::formatter' for each of the partial
        //:   specializations that we promote to 'std'. (C-2)
        //
        // Testing:
        //   formatter();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING (PRIMITIVE) GENERATORS"
                            "\n==============================\n");

        if (verbose)
            printf("\nValidating bslfmt construction\n");

        {
            bsl::formatter<const char *,    char>    dummy_c;
            bsl::formatter<const wchar_t *, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            bsl::formatter<char *,    char>    dummy_c;
            bsl::formatter<wchar_t *, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            bsl::formatter<char[10],    char>    dummy_c;
            bsl::formatter<wchar_t[10], wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            bsl::formatter<bsl::basic_string<char>,    char>    dummy_c;
            bsl::formatter<bsl::basic_string<wchar_t>, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            bsl::formatter<std::basic_string<char>, char>       dummy_c;
            bsl::formatter<std::basic_string<wchar_t>, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            bsl::formatter<bsl::basic_string_view<char>, char>       dummy_c;
            bsl::formatter<bsl::basic_string_view<wchar_t>, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            bsl::formatter<std::basic_string_view<char>, char>       dummy_c;
            bsl::formatter<std::basic_string_view<wchar_t>, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        if (verbose)
            printf("\nValidating std construction\n");

        {
            std::formatter<const char *, char>       dummy_c;
            std::formatter<const wchar_t *, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            std::formatter<char *, char>       dummy_c;
            std::formatter<wchar_t *, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            std::formatter<char[10], char>       dummy_c;
            std::formatter<wchar_t[10], wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            std::formatter<bsl::basic_string<char>, char>       dummy_c;
            std::formatter<bsl::basic_string<wchar_t>, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            std::formatter<std::basic_string<char>, char>       dummy_c;
            std::formatter<std::basic_string<wchar_t>, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            std::formatter<bsl::basic_string_view<char>, char>       dummy_c;
            std::formatter<bsl::basic_string_view<wchar_t>, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            std::formatter<std::basic_string_view<char>, char>       dummy_c;
            std::formatter<std::basic_string_view<wchar_t>, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
#endif

      } break;
      case 2: {
        // --------------------------------------------
        // TESTING PRIMARY MANIPULATORS: Not Applicable
        // 
        // Testing:
        //   PRIMARY MANIPULATORS: Not Applicable
        // --------------------------------------------
        if (verbose)
            printf("\nPRIMARY MANIPULATORS: Not Applicable"
                   "\n====================================\n");
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
        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        bsl::formatter<const char *, char>                  dummy1;
        bsl::formatter<bsl::string_view, char>     dummy2;
        bsl::formatter<bsl::wstring_view, wchar_t> dummy3;
        (void)dummy1;
        (void)dummy2;
        (void)dummy3;
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        std::formatter<bsl::string, char>          dummy4;
        std::formatter<bsl::wstring, wchar_t>      dummy5;
        (void)dummy4;
        (void)dummy5;
#endif
        ASSERT(true);  // placeholder

        ASSERT((bslfmt::format("String={:*^10.5}.",
                               "abcdefg") == "String=**abcde***."));
        ASSERT((bslfmt::format("String={:*^10.5}.", std::string("abcdefg")) ==
                "String=**abcde***."));
        ASSERT((bslfmt::format("String={:*^10.5}.", bsl::string("abcdefg")) ==
                "String=**abcde***."));

        ASSERT((bslfmt::format("String={2:*^{0}.{1}}.",
                               10, 5, "abcdefg") == "String=**abcde***."));

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        std::string val = std::format("String={:*^10.5}.",
                                      bsl::string("abcdefg"));
#endif

        bsl::string message;

        const wchar_t *winput = L"abcdefg";

        bool rv;

        rv = bslfmt::Formatter_TestUtil<char>::testEvaluate(&message,
                                                            "**abcde***",
                                                            "{0:*^{1}.{2}}",
                                                            "abcdefghi",
                                                            10,
                                                            5);

        ASSERTV(message.c_str(), rv);

        rv = bslfmt::Formatter_TestUtil<wchar_t>::testEvaluate(
                                                              &message,
                                                              L"**abcde***",
                                                              L"{0:*^{1}.{2}}",
                                                              winput,
                                                              10,
                                                              5);

        ASSERTV(message.c_str(), rv);

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
// Copyright 2023 Bloomberg Finance L.P.
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
