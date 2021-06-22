// bdlb_indexspanstringutil.t.cpp                                     -*-C++-*-
#include <bdlb_indexspanstringutil.h>

#include <bdlb_stringrefutil.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_buildtarget.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component is a utility operating on 'bldb::IndexSpan' and string
// objects.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [2] bsl::[w]string_view bind([w]string, span);
// [3] IndexSpan createFromPositions([w]string, pos, end);
// [3] IndexSpan create([w]string, pos, len);
// [3] IndexSpan create([w]string, substring);
// [3] IndexSpan create([w]string, begin, len);
// [3] IndexSpan create([w]string, begin, end);
// ----------------------------------------------------------------------------
// [1] BREATHING TEST
// [4] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                              TYPE ALIASES
//-----------------------------------------------------------------------------

typedef bdlb::IndexSpanStringUtil Util;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test            = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool verbose         = argc > 2;
    bool veryVerbose     = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test)  { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE"
                             "\n=============\n";

///Example 1: Creating 'IndexSpan' Objects Safely
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we are creating a parser, and we want the results of the
// parsing to be stored in 'IndexSpan' objects.  The parser will have either a
// pointer (or "begin" iterator) into the original string input and then
// another pointer (or iterator) or a length to identify the end of the input.
// Turning the beginning and ending identifiers into an 'IndexSpan' is a simple
// calculation, but one that is verbose and potentially error prone.  Instead
// of implementing the calculation ourselves we use the convenience function
// 'create' from 'IndexSpanStringUtil'.
//
// First, we define a string that we want to parse:
//..
    const bsl::string full("James Tiberius Kirk");
//..
// Then, we implement the parsing of the first name:
//..
    typedef bsl::string::const_iterator Cit;
    Cit it = bsl::find(full.begin(), full.end(), ' ');
    // Error checking omitted since we know the string
    bdlb::IndexSpan first = bdlb::IndexSpanStringUtil::create(full,
                                                              full.begin(),
                                                              it);
//..
// Next, we implement the parsing of the middle name, this time using length,
// rather than an end iterator (demonstrating an alternative 'create' overload
// provided by 'IndexSpanStringUtil'):
//..
    ++it;  // Skip the space
    Cit it2 = bsl::find(it, full.end(), ' ');
    bdlb::IndexSpan middle;
    if (full.end() != it2) {
        middle = bdlb::IndexSpanStringUtil::create(full, it, it2 - it);
        it = it2 + 1;
    }
//..
// Then, we create the 'IndexSpan' for the last name, using two positions:
//..
    bdlb::IndexSpan last = bdlb::IndexSpanStringUtil::createFromPositions(
                                                             full,
                                                             it - full.begin(),
                                                             full.length());
//..
// Finally, we verify that the resulting 'IndexSpan' objects correctly describe
// the parsed names of the 'full' name:
//..
    ASSERT(full.substr(first.position(), first.length()) == "James");

    ASSERT(full.substr(middle.position(), middle.length()) == "Tiberius");

    ASSERT(full.substr(last.position(), last.length()) == "Kirk");
//..
//
///Example 2: Creating String References
///- - - - - - - - - - - - - - - - - - -
// Suppose that we have 'IndexSpan' objects that define the 'first', 'middle',
// and 'last' part of a string that has a full name in it and we want to get
// actual string references that correspond to those parts of the string.  The
// 'bind' functions of 'IndexSpanStringUtil' provide that functionality.  The
// 'bind' functions return a 'StringRef' into the original string (so the
// characters of the string are not copied).  Note that this example builds on
// Example 1.
//
// First, we define a string reference of the parsed string to show that 'bind'
// works both on strings and string references:
//..
    const bsl::string_view full_view(full);
//..
// Then we demonstrate binding 'IndexSpan' object to that reference:
//..
    ASSERT(bdlb::IndexSpanStringUtil::bind(full_view, first) == "James");

    ASSERT(bdlb::IndexSpanStringUtil::bind(full_view, middle) == "Tiberius");

    ASSERT(bdlb::IndexSpanStringUtil::bind(full_view, last) == "Kirk");
//..
// Finally we demonstrate binding 'IndexSpan' object to a string:
//..
    ASSERT(bdlb::IndexSpanStringUtil::bind(full, first) == "James");

    ASSERT(bdlb::IndexSpanStringUtil::bind(full, middle) == "Tiberius");

    ASSERT(bdlb::IndexSpanStringUtil::bind(full, last) == "Kirk");
//..
      } break;

      case 3: {
        // --------------------------------------------------------------------
        // TESTING CREATE
        //
        // Concerns:
        //: 1 Calls create the expected 'IndexSpan' values.
        //:
        //: 2 Bad arguments assert in the proper build mode.
        //:
        //: 3 All functions are callable with 'const' references to
        //:   'bsl::string_view', 'bslstl::StringRef', 'bsl::string',
        //:   'std::string', and 'std::pmr::string' objects as first argument,
        //:   as well as their wide equivalents.
        //
        // Plan:
        //: 1 Use table based testing for both good and bad (invalid
        //:   precondition) calls.
        //:
        //: 2 Test all overloads of 'create', as well as 'createFromPositions'
        //:   for all rows of the tables.
        //:
        //: 3 Test both wide and narrow character versions.
        //:
        //: 4 Separately verify that too low 'begin' for 'create' with
        //:   'bsl::string_view' and 'bslstl::StringRef' inputs asserts.  Note
        //:   that too low begin position or iterator cannot be tested with a
        //:   'bsl::string', 'std::string', or 'std::pmr::string' input as we
        //:   cannot create a valid iterator', or even a 'StringRef', that
        //:   would point into the same allocated memory area (as required by
        //:   the C++ standard) and has a lower begin value than 'begin()'.
        //
        // Testing:
        //   IndexSpan createFromPositions([w]string, pos, end);
        //   IndexSpan create([w]string, pos, len);
        //   IndexSpan create([w]string, substring);
        //   IndexSpan create([w]string, begin, len);
        //   IndexSpan create([w]string, begin, end);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING CREATE"
                             "\n==============\n";

        typedef bdlb::IndexSpan::size_type size_type;

        if (veryVerbose) cout << "Positive narrow string overload tests.\n";

        static const struct TestData {
            long long         d_line;
            bslstl::StringRef d_str;
            size_type         d_pos;
            size_type         d_len;
        } k_DATA[] = {
            { L_ , "",                  0,  0 },
            { L_ , "0123456789abcdef",  0,  0 },
            { L_ , "0123456789abcdef",  0,  5 },
            { L_ , "0123456789abcdef",  5,  5 },
            { L_ , "0123456789abcdef",  3, 10 },
            { L_ , "0123456789abcdef", 16,  0 },
            { L_ , "0123456789abcdef", 15,  1 },
            { L_ , "0123456789abcdef",  0,  1 },
            { L_ , "0123456789abcdef", 10,  6 },
        };

        static bsl::size_t NUM_TESTS = sizeof k_DATA / sizeof *k_DATA;

        for (bsl::size_t i = 0; i < NUM_TESTS; ++i) {
            bslma::TestAllocator     sa("bslstring", veryVeryVerbose);
            const TestData&          k_TEST    = k_DATA[i];
            const long long          k_LINE    = k_TEST.d_line;
            const bsl::string_view&  k_STRVIEW = k_TEST.d_str;
            const size_type          k_POS     = k_TEST.d_pos;
            const size_type          k_LEN     = k_TEST.d_len;
            const bslstl::StringRef& k_STRREF  = k_STRVIEW;
            const bsl::string        k_BSLSTR(k_STRVIEW, &sa);
#ifdef BSLSTL_STRING_VIEW_IS_ALIASED
            const std::string        k_STDSTR(k_STRVIEW);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            const std::pmr::string   k_PMRSTR(k_STRVIEW);
#endif
#else
            const std::string       k_STDSTR(k_STRREF.data(), k_STRREF.size());
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            const std::pmr::string  k_PMRSTR(k_STRREF.data(), k_STRREF.size());
#endif
#endif
            const bdlb::IndexSpan    k_EXPECTED(k_POS, k_LEN);

            if (veryVerbose) {
                P_(k_LINE); P_(k_STRVIEW); P(k_EXPECTED);
            }

            const size_type k_END_POS = k_POS + k_LEN;

            bdlb::IndexSpan created;

#define VERIFY_CREATE ASSERTV(created, k_EXPECTED, k_EXPECTED == created)

#define TEST_CREATE_FROM_POSITIONS(input)                                     \
    created = Util::createFromPositions(input, k_POS, k_END_POS);             \
    VERIFY_CREATE

            TEST_CREATE_FROM_POSITIONS(k_STRVIEW);
            TEST_CREATE_FROM_POSITIONS(k_STRREF);
            TEST_CREATE_FROM_POSITIONS(k_BSLSTR);
            TEST_CREATE_FROM_POSITIONS(k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_CREATE_FROM_POSITIONS(k_PMRSTR);
#endif
#undef TEST_CREATE_FROM_POSITIONS

#define TEST_CREATE_FROM_POS_LEN(input)                                       \
    created = Util::create(input, k_POS, k_LEN);                              \
    VERIFY_CREATE

            TEST_CREATE_FROM_POS_LEN(k_STRVIEW);
            TEST_CREATE_FROM_POS_LEN(k_STRREF);
            TEST_CREATE_FROM_POS_LEN(k_BSLSTR);
            TEST_CREATE_FROM_POS_LEN(k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_CREATE_FROM_POS_LEN(k_PMRSTR);
#endif
#undef TEST_CREATE_FROM_POS_LEN

#define TEST_CREATE_FROM_ITER(inType, input)                                  \
    {                                                                         \
        typedef inType::const_iterator Cit;                                   \
        const Cit k_BEGIN = input.begin() + k_POS;                            \
        const Cit k_END = k_BEGIN + k_LEN;                                    \
                                                                              \
        created = Util::create(input, k_BEGIN, k_LEN);                        \
        VERIFY_CREATE;                                                        \
                                                                              \
        created = Util::create(input, k_BEGIN, k_END);                        \
        VERIFY_CREATE;                                                        \
    }

            TEST_CREATE_FROM_ITER(bsl::string_view,  k_STRVIEW);
            TEST_CREATE_FROM_ITER(bslstl::StringRef, k_STRREF);
            TEST_CREATE_FROM_ITER(bsl::string,       k_BSLSTR);
            TEST_CREATE_FROM_ITER(std::string,       k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_CREATE_FROM_ITER(std::pmr::string, k_PMRSTR);
#endif
#undef TEST_CREATE_FROM_ITER

#define TEST_CREATE_FROM_SUBSTR(input)                                        \
    {                                                                         \
        const bsl::string_view k_SUBVIEW(input.data() + k_POS, k_LEN);        \
        created = Util::create(input, k_SUBVIEW);                             \
        VERIFY_CREATE;                                                        \
    }                                                                         \
    {                                                                         \
        const bslstl::StringRef k_SUBREF(input.data() + k_POS,  k_LEN);       \
        created = Util::create(input, k_SUBREF);                              \
        VERIFY_CREATE;                                                        \
    }

            TEST_CREATE_FROM_SUBSTR(k_STRVIEW);
            TEST_CREATE_FROM_SUBSTR(k_STRREF);
            TEST_CREATE_FROM_SUBSTR(k_BSLSTR);
            TEST_CREATE_FROM_SUBSTR(k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_CREATE_FROM_SUBSTR(k_PMRSTR);
#endif
#undef TEST_CREATE_FROM_SUBSTR
#undef VERIFY_CREATE
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (veryVerbose) cout << "Negative narrow string overload tests.\n";

        static const struct NegativeTestData {
            long long        d_line;
            bsl::string_view d_str;
            size_type        d_pos;
            size_type        d_len;
        } k_NDATA[] = {
            { L_ , "",                  1,  0 },
            { L_ , "",                  0,  1 },
            { L_ , "",                  1,  1 },
            { L_ , "0123456789abcdef", 17,  0 },
            { L_ , "0123456789abcdef", 16,  1 },
            { L_ , "0123456789abcdef", 10,  7 },
        };

        static bsl::size_t NUM_NTESTS = sizeof k_NDATA / sizeof *k_NDATA;

        for (bsl::size_t i = 0; i < NUM_NTESTS; ++i) {
            bslma::TestAllocator     sa("bslstring", veryVeryVerbose);
            const NegativeTestData&  k_TEST    = k_NDATA[i];
            const long long          k_LINE    = k_TEST.d_line;
            const size_type          k_POS     = k_TEST.d_pos;
            const size_type          k_LEN     = k_TEST.d_len;
            const bsl::string_view&  k_STRVIEW = k_TEST.d_str;
            const bslstl::StringRef& k_STRREF  = k_STRVIEW;
            const bsl::string        k_BSLSTR(k_STRVIEW, &sa);
#ifdef BSLSTL_STRING_VIEW_IS_ALIASED
            const std::string        k_STDSTR(k_STRVIEW);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            const std::pmr::string   k_PMRSTR(k_STRVIEW);
#endif
#else
            const std::string       k_STDSTR(k_STRREF.data(), k_STRREF.size());
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            const std::pmr::string  k_PMRSTR(k_STRREF.data(), k_STRREF.size());
#endif
#endif

            if (veryVerbose) {
                P_(k_LINE); P_(k_STRVIEW); P_(k_POS);  P(k_LEN);
            }

            const size_type k_ENDP = k_POS + k_LEN;

            bsls::AssertTestHandlerGuard g; (void)g;

            ASSERT_FAIL(Util::createFromPositions(k_STRVIEW, k_POS, k_ENDP));
            ASSERT_FAIL(Util::createFromPositions(k_STRREF,  k_POS, k_ENDP));
            ASSERT_FAIL(Util::createFromPositions(k_BSLSTR,  k_POS, k_ENDP));
            ASSERT_FAIL(Util::createFromPositions(k_STDSTR,  k_POS, k_ENDP));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            ASSERT_FAIL(Util::createFromPositions(k_PMRSTR,  k_POS, k_ENDP));
#endif

            ASSERT_FAIL(Util::create(k_STRVIEW, k_POS, k_LEN));
            ASSERT_FAIL(Util::create(k_STRREF,  k_POS, k_LEN));
            ASSERT_FAIL(Util::create(k_BSLSTR,  k_POS, k_LEN));
            ASSERT_FAIL(Util::create(k_STDSTR,  k_POS, k_LEN));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            ASSERT_FAIL(Util::create(k_PMRSTR,  k_POS, k_LEN));
#endif

#define TEST_CREATE_FROM_ITER(inType, input)                                  \
    {                                                                         \
        typedef inType::const_iterator Cit;                                   \
        const Cit k_BEGIN = input.begin() + k_POS;                            \
        const Cit k_END = k_BEGIN + k_LEN;                                    \
                                                                              \
        ASSERT_FAIL(Util::create(input, k_BEGIN, k_LEN));                     \
        ASSERT_FAIL(Util::create(input, k_BEGIN, k_END));                     \
    }
            TEST_CREATE_FROM_ITER(bsl::string_view,  k_STRVIEW);
            TEST_CREATE_FROM_ITER(bslstl::StringRef, k_STRREF);
            TEST_CREATE_FROM_ITER(bsl::string,       k_BSLSTR);
            TEST_CREATE_FROM_ITER(std::string,       k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_CREATE_FROM_ITER(std::pmr::string,  k_PMRSTR);
#endif
#undef TEST_CREATE_FROM_ITER

#define TEST_CREATE_FROM_SUBSTR(input)                                        \
    {                                                                         \
        const bsl::string_view k_SUBVIEW(input.data() + k_POS, k_LEN);        \
        ASSERT_FAIL(Util::create(input, k_SUBVIEW));                          \
    }                                                                         \
    {                                                                         \
        const bslstl::StringRef k_SUBREF(input.data() + k_POS, k_LEN);        \
        ASSERT_FAIL(Util::create(input, k_SUBREF));                           \
    }

            TEST_CREATE_FROM_SUBSTR(k_STRVIEW);
            TEST_CREATE_FROM_SUBSTR(k_STRREF);
            TEST_CREATE_FROM_SUBSTR(k_BSLSTR);
            TEST_CREATE_FROM_SUBSTR(k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_CREATE_FROM_SUBSTR(k_PMRSTR);
#endif
#undef TEST_CREATE_FROM_SUBSTR
        }
#endif

        if (veryVerbose) cout << "Positive wide string overload tests.\n";

        static const struct TestDataWide {
            long long         d_line;
            bsl::wstring_view d_str;
            size_type         d_pos;
            size_type         d_len;
        } k_WDATA[] = {
            { L_ , L"",                  0,  0 },
            { L_ , L"0123456789abcdef",  0,  0 },
            { L_ , L"0123456789abcdef",  0,  5 },
            { L_ , L"0123456789abcdef",  5,  5 },
            { L_ , L"0123456789abcdef",  3, 10 },
            { L_ , L"0123456789abcdef", 16,  0 },
            { L_ , L"0123456789abcdef", 15,  1 },
            { L_ , L"0123456789abcdef",  0,  1 },
            { L_ , L"0123456789abcdef", 10,  6 },
        };

        static bsl::size_t NUM_WTESTS = sizeof k_WDATA / sizeof *k_WDATA;

        for (bsl::size_t i = 0; i < NUM_WTESTS; ++i) {
            bslma::TestAllocator         sa("bslstring", veryVeryVerbose);
            const TestDataWide&          k_TEST    = k_WDATA[i];
            const long long              k_LINE    = k_TEST.d_line;
            const bsl::wstring_view&     k_STRVIEW = k_TEST.d_str;
            const size_type              k_POS     = k_TEST.d_pos;
            const size_type              k_LEN     = k_TEST.d_len;
            const bslstl::StringRefWide& k_STRREF  = k_STRVIEW;
            const bsl::wstring           k_BSLSTR(k_STRVIEW, &sa);
#ifdef BSLSTL_STRING_VIEW_IS_ALIASED
            const std::wstring            k_STDSTR(k_STRVIEW);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            const std::pmr::wstring       k_PMRSTR(k_STRVIEW);
#endif
#else
            const std::wstring      k_STDSTR(k_STRREF.data(), k_STRREF.size());
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            const std::pmr::wstring k_PMRSTR(k_STRREF.data(), k_STRREF.size());
#endif
#endif
            const bdlb::IndexSpan   k_EXPECTED(k_POS, k_LEN);

            if (veryVerbose) {
                P_(k_LINE); P(k_EXPECTED);
            }

            const size_type k_END_POS = k_POS + k_LEN;

            bdlb::IndexSpan created;

#define VERIFY_CREATE ASSERTV(created, k_EXPECTED, k_EXPECTED == created)

#define TEST_CREATE_FROM_POSITIONS(input)                                     \
    created = Util::createFromPositions(input, k_POS, k_END_POS);             \
    VERIFY_CREATE

            TEST_CREATE_FROM_POSITIONS(k_STRVIEW);
            TEST_CREATE_FROM_POSITIONS(k_STRREF);
            TEST_CREATE_FROM_POSITIONS(k_BSLSTR);
            TEST_CREATE_FROM_POSITIONS(k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_CREATE_FROM_POSITIONS(k_PMRSTR);
#endif
#undef TEST_CREATE_FROM_POSITIONS

#define TEST_CREATE_FROM_POS_LEN(input)                                       \
    created = Util::create(input, k_POS, k_LEN);                              \
    VERIFY_CREATE

            TEST_CREATE_FROM_POS_LEN(k_STRVIEW);
            TEST_CREATE_FROM_POS_LEN(k_STRREF);
            TEST_CREATE_FROM_POS_LEN(k_BSLSTR);
            TEST_CREATE_FROM_POS_LEN(k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_CREATE_FROM_POS_LEN(k_PMRSTR);
#endif
#undef TEST_CREATE_FROM_POS_LEN

#define TEST_CREATE_FROM_ITER(inType, input)                                  \
    {                                                                         \
        typedef inType::const_iterator Cit;                                   \
        const Cit k_BEGIN = input.begin() + k_POS;                            \
        const Cit k_END = k_BEGIN + k_LEN;                                    \
                                                                              \
        created = Util::create(input, k_BEGIN, k_LEN);                        \
        VERIFY_CREATE;                                                        \
                                                                              \
        created = Util::create(input, k_BEGIN, k_END);                        \
        VERIFY_CREATE;                                                        \
    }

            TEST_CREATE_FROM_ITER(bsl::wstring_view,     k_STRVIEW);
            TEST_CREATE_FROM_ITER(bslstl::StringRefWide, k_STRREF);
            TEST_CREATE_FROM_ITER(bsl::wstring,          k_BSLSTR);
            TEST_CREATE_FROM_ITER(std::wstring,          k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_CREATE_FROM_ITER(std::pmr::wstring,     k_PMRSTR);
#endif
#undef TEST_CREATE_FROM_ITER

#define TEST_CREATE_FROM_SUBSTR(input)                                        \
    {                                                                         \
        const bsl::wstring_view k_SUBVIEW(input.data() + k_POS, k_LEN);       \
        created = Util::create(input, k_SUBVIEW);                             \
        VERIFY_CREATE;                                                        \
    }                                                                         \
    {                                                                         \
        const bslstl::StringRefWide k_SUBREF(input.data() + k_POS, k_LEN);    \
        created = Util::create(input, k_SUBREF);                              \
        VERIFY_CREATE;                                                        \
    }

            TEST_CREATE_FROM_SUBSTR(k_STRVIEW);
            TEST_CREATE_FROM_SUBSTR(k_STRREF);
            TEST_CREATE_FROM_SUBSTR(k_BSLSTR);
            TEST_CREATE_FROM_SUBSTR(k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_CREATE_FROM_SUBSTR(k_PMRSTR);
#endif
#undef TEST_CREATE_FROM_SUBSTR
#undef VERIFY_CREATE
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (veryVerbose) cout << "Negative wide string overload tests.\n";

        static const struct NegativeTestDataWide {
            long long         d_line;
            bsl::wstring_view d_str;
            size_type         d_pos;
            size_type         d_len;
        } k_NWDATA[] = {
            { L_ , L"",                  1,  0 },
            { L_ , L"",                  0,  1 },
            { L_ , L"",                  1,  1 },
            { L_ , L"0123456789abcdef", 17,  0 },
            { L_ , L"0123456789abcdef", 16,  1 },
            { L_ , L"0123456789abcdef", 10,  7 },
        };

        static bsl::size_t NUM_NWTESTS = sizeof k_NWDATA / sizeof *k_NWDATA;

        for (bsl::size_t i = 0; i < NUM_NWTESTS; ++i) {
            bslma::TestAllocator         sa("bslstring", veryVeryVerbose);
            const NegativeTestDataWide&  k_TEST    = k_NWDATA[i];
            const long long              k_LINE    = k_TEST.d_line;
            const size_type              k_POS     = k_TEST.d_pos;
            const size_type              k_LEN     = k_TEST.d_len;
            const bsl::wstring_view&     k_STRVIEW = k_TEST.d_str;
            const bslstl::StringRefWide& k_STRREF  = k_STRVIEW;
            const bsl::wstring           k_BSLSTR(k_STRVIEW, &sa);
#ifdef BSLSTL_STRING_VIEW_IS_ALIASED
            const std::wstring           k_STDSTR(k_STRVIEW);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            const std::pmr::wstring      k_PMRSTR(k_STRVIEW);
#endif
#else
            const std::wstring      k_STDSTR(k_STRREF.data(), k_STRREF.size());
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            const std::pmr::wstring k_PMRSTR(k_STRREF.data(), k_STRREF.size());
#endif
#endif

            if (veryVerbose) {
                P_(k_LINE); P_(k_POS);  P(k_LEN);
            }

            const size_type k_ENDP = k_POS + k_LEN;

            bsls::AssertTestHandlerGuard g; (void)g;

            ASSERT_FAIL(Util::createFromPositions(k_STRVIEW, k_POS, k_ENDP));
            ASSERT_FAIL(Util::createFromPositions(k_STRREF, k_POS,  k_ENDP));
            ASSERT_FAIL(Util::createFromPositions(k_BSLSTR, k_POS,  k_ENDP));
            ASSERT_FAIL(Util::createFromPositions(k_STDSTR, k_POS,  k_ENDP));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            ASSERT_FAIL(Util::createFromPositions(k_PMRSTR, k_POS,  k_ENDP));
#endif

            ASSERT_FAIL(Util::create(k_STRVIEW, k_POS, k_LEN));
            ASSERT_FAIL(Util::create(k_STRREF,  k_POS, k_LEN));
            ASSERT_FAIL(Util::create(k_BSLSTR,  k_POS, k_LEN));
            ASSERT_FAIL(Util::create(k_STDSTR,  k_POS, k_LEN));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            ASSERT_FAIL(Util::create(k_PMRSTR,  k_POS, k_LEN));
#endif

#define TEST_CREATE_FROM_ITER(inType, input)                                  \
    {                                                                         \
        typedef inType::const_iterator Cit;                                   \
        const Cit k_BEGIN = input.begin() + k_POS;                            \
        const Cit k_END = k_BEGIN + k_LEN;                                    \
                                                                              \
        ASSERT_FAIL(Util::create(input, k_BEGIN, k_LEN));                     \
        ASSERT_FAIL(Util::create(input, k_BEGIN, k_END));                     \
    }
            TEST_CREATE_FROM_ITER(bsl::wstring_view,     k_STRVIEW);
            TEST_CREATE_FROM_ITER(bslstl::StringRefWide, k_STRREF);
            TEST_CREATE_FROM_ITER(bsl::wstring,          k_BSLSTR);
            TEST_CREATE_FROM_ITER(std::wstring,          k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_CREATE_FROM_ITER(std::pmr::wstring,     k_PMRSTR);
#endif
#undef TEST_CREATE_FROM_ITER

#define TEST_CREATE_FROM_SUBSTR(input)                                        \
    {                                                                         \
        const bsl::wstring_view k_SUBVIEW(input.data() + k_POS, k_LEN);       \
        ASSERT_FAIL(Util::create(input, k_SUBVIEW));                          \
    }                                                                         \
    {                                                                         \
        const bslstl::StringRefWide k_SUBREF(input.data() + k_POS, k_LEN);    \
        ASSERT_FAIL(Util::create(input, k_SUBREF));                           \
    }

            TEST_CREATE_FROM_SUBSTR(k_STRVIEW);
            TEST_CREATE_FROM_SUBSTR(k_STRREF);
            TEST_CREATE_FROM_SUBSTR(k_BSLSTR);
            TEST_CREATE_FROM_SUBSTR(k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_CREATE_FROM_SUBSTR(k_PMRSTR);
#endif
#undef TEST_CREATE_FROM_SUBSTR
        }

        if (verbose)  cout << "Testing assertions with too low 'begin'.\n";
        {
            bslstl::StringRef string("0123456789");
            bslstl::StringRef input = bdlb::StringRefUtil::substr(string, 1);

            bsls::AssertTestHandlerGuard g; (void)g;

            ASSERT_FAIL(Util::create(input, string.begin(), (bsl::size_t)0));
            ASSERT_PASS(Util::create(input, input.begin(), (bsl::size_t)0));

            ASSERT_FAIL(Util::create(input, string.begin(), input.end()));
            ASSERT_PASS(Util::create(input, input.begin(), input.end()));

            ASSERT_FAIL(Util::create(input, string));
            ASSERT_PASS(Util::create(input, input));
        }
        {
            bsl::string_view string("0123456789");
            bsl::string_view input = string.substr(1);

            bsls::AssertTestHandlerGuard g; (void)g;

            ASSERT_FAIL(Util::create(input, string.begin(), (bsl::size_t)0));
            ASSERT_PASS(Util::create(input, input.begin(), (bsl::size_t)0));

            ASSERT_FAIL(Util::create(input, string.begin(), input.end()));
            ASSERT_PASS(Util::create(input, input.begin(), input.end()));

            ASSERT_FAIL(Util::create(input, string));
            ASSERT_PASS(Util::create(input, input));
        }

        {
            bslstl::StringRefWide string(L"0123456789");
            bslstl::StringRefWide input(string.data() + 1, string.size() - 1);

            bsls::AssertTestHandlerGuard g; (void)g;

            ASSERT_FAIL(Util::create(input, string.begin(), (bsl::size_t)0));
            ASSERT_PASS(Util::create(input, input.begin(), (bsl::size_t)0));

            ASSERT_FAIL(Util::create(input, string.begin(), input.end()));
            ASSERT_PASS(Util::create(input, input.begin(), input.end()));

            ASSERT_FAIL(Util::create(input, string));
            ASSERT_PASS(Util::create(input, input));
        }
        {
            bsl::wstring_view string(L"0123456789");
            bsl::wstring_view input(string.data() + 1, string.size() - 1);

            bsls::AssertTestHandlerGuard g; (void)g;

            ASSERT_FAIL(Util::create(input, string.begin(), (bsl::size_t)0));
            ASSERT_PASS(Util::create(input, input.begin(), (bsl::size_t)0));

            ASSERT_FAIL(Util::create(input, string.begin(), input.end()));
            ASSERT_PASS(Util::create(input, input.begin(), input.end()));

            ASSERT_FAIL(Util::create(input, string));
            ASSERT_PASS(Util::create(input, input));
        }
#endif
      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING BIND
        //
        // Concerns:
        //: 1 Return value references the proper substring.
        //:
        //: 2 Return value references a substring of the passed in string.
        //:
        //: 3 If the specified span does not fit into the specified string an
        //:   assertion is raised (if the build mode calls for it).
        //:
        //: 4 'bind' can be called with 'bsl::string', 'bsl::string_view',
        //:    'bslstl::StringRef', 'std::string', and 'std::pmr::string'
        //:    first argument.
        //:
        //: 5 'bind' result can be assigned to 'bslstl::StringRef', or
        //:   'const bslstl::StringRef&', 'bsl::string' variable.
        //:
        //: 6 'bind' result can initialize a 'bslstl::StringRef', or
        //:   'const bslstl::StringRef&', 'bsl::string' variable.
        //:
        //: 7 When 'bsl::string_view' is an alias to the native
        //:   'std::string_view' the result of 'bind' can direct-initialize a
        //:   'std::string', or 'std::pmr::string' variable.
        //
        // Plan:
        //: 1 Table based testing with values that test
        //:
        //:   1 Valid and invalid substrings of an empty string
        //:
        //:   2 Valid substrings of a string
        //:
        //:   3 Boundary substrings at the beginning and the end of the string
        //:
        //:   4 Spans that does not define a valid substring
        //:
        //:   5 The same calls with all possible combinations of first argument
        //:     and results types both with initialization and assignment
        //:
        //:   6 Test all with assigning result to 'StringRef' and 'const' ref
        //:
        //:   7 Verify 'StringRef' results are identical
        //:
        //
        // Testing:
        //   bsl::[w]string_view bind([w]string, span);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING BIND"
                             "\n============\n";

        typedef bdlb::IndexSpan::size_type size_type;

        if (veryVeryVerbose) cout << "Positive testing of narrow strings\n";

        static const struct TestData {
            long long        d_line;
            bsl::string_view d_str;
            size_type        d_pos;
            size_type        d_len;
            bsl::string_view d_expected;
        } k_DATA[] = {
            { L_ , "",                  0,  0, ""           },
            { L_ , "0123456789abcdef",  0,  0, ""           },
            { L_ , "0123456789abcdef",  0,  5, "01234"      },
            { L_ , "0123456789abcdef",  5,  5, "56789"      },
            { L_ , "0123456789abcdef",  3, 10, "3456789abc" },
            { L_ , "0123456789abcdef", 16,  0, ""           },
            { L_ , "0123456789abcdef", 15,  1, "f"          },
            { L_ , "0123456789abcdef",  0,  1, "0"          },
            { L_ , "0123456789abcdef", 10,  6, "abcdef"     },
        };

        static bsl::size_t NUM_TESTS = sizeof k_DATA / sizeof *k_DATA;

        for (bsl::size_t i = 0; i < NUM_TESTS; ++i) {
            bslma::TestAllocator     sa("bslstring", veryVeryVerbose);
            const TestData&          k_TEST    = k_DATA[i];
            const long long          k_LINE    = k_TEST.d_line;
            const bsl::string_view&  k_STRVIEW = k_TEST.d_str;
            const bslstl::StringRef& k_STRREF  = k_STRVIEW;
            const bsl::string        k_BSLSTR(k_STRVIEW, &sa);
#ifdef BSLSTL_STRING_VIEW_IS_ALIASED
            const std::string        k_STDSTR(k_STRVIEW);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            const std::pmr::string   k_PMRSTR(k_STRVIEW);
#endif
#else
            const std::string       k_STDSTR(k_STRREF.data(), k_STRREF.size());
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            const std::pmr::string  k_PMRSTR(k_STRREF.data(), k_STRREF.size());
#endif
#endif
            const bdlb::IndexSpan    k_SPAN(k_TEST.d_pos, k_TEST.d_len);
            const bsl::string_view&  k_EXPECTED = k_TEST.d_expected;

            if (veryVerbose) {
                P_(k_LINE); P_(k_STRVIEW); P_(k_SPAN); P(k_EXPECTED);
            }

#define TEST_BIND_VERIFY(ResultType, InputName)                               \
    ASSERTV(bound, k_EXPECTED, k_EXPECTED == bound);                          \
    ASSERT(bound.data() == InputName.data() + k_SPAN.position());             \
    ASSERT(bound.length() == k_SPAN.length())

#define TEST_BIND_IMPLICIT(ResultType, InputName)                             \
    {                                                                         \
        const ResultType bound = Util::bind(InputName, k_SPAN);               \
        TEST_BIND_VERIFY(ResultType, InputName);                              \
    }

#define TEST_BIND_EXPLICIT(ResultType, InputName)                             \
    {                                                                         \
        const ResultType bound(Util::bind(InputName, k_SPAN));                \
        TEST_BIND_VERIFY(ResultType, InputName);                              \
    }

#define TEST_BIND_ASSIGN(ResultType, InputName)                               \
    {                                                                         \
        ResultType bound;                                                     \
        bound = Util::bind(InputName, k_SPAN);                                \
        TEST_BIND_VERIFY(ResultType, InputName);                              \
    }

            TEST_BIND_IMPLICIT(bsl::string_view, k_STRVIEW);
            TEST_BIND_IMPLICIT(bsl::string_view, k_STRREF);
            TEST_BIND_IMPLICIT(bsl::string_view, k_BSLSTR);
            TEST_BIND_IMPLICIT(bsl::string_view, k_STDSTR);
            TEST_BIND_ASSIGN(bsl::string_view,   k_STRVIEW);
            TEST_BIND_ASSIGN(bsl::string_view,   k_STRREF);
            TEST_BIND_ASSIGN(bsl::string_view,   k_BSLSTR);
            TEST_BIND_ASSIGN(bsl::string_view,   k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_BIND_IMPLICIT(bsl::string_view, k_PMRSTR);
            TEST_BIND_ASSIGN(bsl::string_view,   k_PMRSTR);
#endif

            TEST_BIND_IMPLICIT(bslstl::StringRef, k_STRVIEW);
            TEST_BIND_IMPLICIT(bslstl::StringRef, k_STRREF);
            TEST_BIND_IMPLICIT(bslstl::StringRef, k_BSLSTR);
            TEST_BIND_IMPLICIT(bslstl::StringRef, k_STDSTR);
            TEST_BIND_ASSIGN(bslstl::StringRef,   k_STRVIEW);
            TEST_BIND_ASSIGN(bslstl::StringRef,   k_STRREF);
            TEST_BIND_ASSIGN(bslstl::StringRef,   k_BSLSTR);
            TEST_BIND_ASSIGN(bslstl::StringRef,   k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_BIND_IMPLICIT(bslstl::StringRef, k_PMRSTR);
            TEST_BIND_ASSIGN(bslstl::StringRef,   k_PMRSTR);
#endif

            TEST_BIND_IMPLICIT(bslstl::StringRef&, k_STRVIEW);
            TEST_BIND_IMPLICIT(bslstl::StringRef&, k_STRREF);
            TEST_BIND_IMPLICIT(bslstl::StringRef&, k_BSLSTR);
            TEST_BIND_IMPLICIT(bslstl::StringRef&, k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_BIND_IMPLICIT(bslstl::StringRef&, k_PMRSTR);
#endif

#undef TEST_BIND_ASSIGN
#undef TEST_BIND_EXPLICIT
#undef TEST_BIND_IMPLICIT
#undef TEST_BIND_VERIFY

#define TEST_BIND_VERIFY(ResultType, InputName)                               \
    ASSERTV(bound, k_EXPECTED, k_EXPECTED == bound);                          \
    ASSERT(bound.length() == k_SPAN.length());

#define TEST_BIND_EXPLICIT(ResultType, InputName)                             \
    {                                                                         \
        const ResultType bound(Util::bind(InputName, k_SPAN));                \
        TEST_BIND_VERIFY(ResultType, InputName);                              \
    }

#define TEST_BIND_ASSIGN(ResultType, InputName)                               \
    {                                                                         \
        ResultType bound;                                                     \
        bound = Util::bind(InputName, k_SPAN);                                \
        TEST_BIND_VERIFY(ResultType, InputName);                              \
    }

            TEST_BIND_EXPLICIT(bsl::string, k_STRVIEW);
            TEST_BIND_EXPLICIT(bsl::string, k_STRREF);
            TEST_BIND_EXPLICIT(bsl::string, k_BSLSTR);
            TEST_BIND_EXPLICIT(bsl::string, k_STDSTR);
            TEST_BIND_ASSIGN(bsl::string,   k_STRVIEW);
            TEST_BIND_ASSIGN(bsl::string,   k_STRREF);
            TEST_BIND_ASSIGN(bsl::string,   k_BSLSTR);
            TEST_BIND_ASSIGN(bsl::string,   k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_BIND_EXPLICIT(bsl::string, k_PMRSTR);
            TEST_BIND_ASSIGN(bsl::string,   k_PMRSTR);
#endif

#ifdef BSLSTL_STRING_VIEW_IS_ALIASED
            TEST_BIND_EXPLICIT(std::string, k_STRVIEW);
            TEST_BIND_EXPLICIT(std::string, k_STRREF);
            TEST_BIND_EXPLICIT(std::string, k_BSLSTR);
            TEST_BIND_EXPLICIT(std::string, k_STDSTR);
            TEST_BIND_ASSIGN(std::string,   k_STRVIEW);
            TEST_BIND_ASSIGN(std::string,   k_STRREF);
            TEST_BIND_ASSIGN(std::string,   k_BSLSTR);
            TEST_BIND_ASSIGN(std::string,   k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_BIND_EXPLICIT(std::string, k_PMRSTR);
            TEST_BIND_ASSIGN(std::string,   k_PMRSTR);
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_BIND_EXPLICIT(std::pmr::string, k_STRVIEW);
            TEST_BIND_EXPLICIT(std::pmr::string, k_STRREF);
            TEST_BIND_EXPLICIT(std::pmr::string, k_BSLSTR);
            TEST_BIND_EXPLICIT(std::pmr::string, k_STDSTR);
            TEST_BIND_EXPLICIT(std::pmr::string, k_PMRSTR);
            TEST_BIND_ASSIGN(std::pmr::string,   k_STRVIEW);
            TEST_BIND_ASSIGN(std::pmr::string,   k_STRREF);
            TEST_BIND_ASSIGN(std::pmr::string,   k_BSLSTR);
            TEST_BIND_ASSIGN(std::pmr::string,   k_STDSTR);
            TEST_BIND_ASSIGN(std::pmr::string,   k_PMRSTR);
#endif
#endif

#undef TEST_BIND_ASSIGN
#undef TEST_BIND_EXPLICIT
#undef TEST_BIND_VERIFY
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (veryVeryVerbose) cout << "Negative testing of narrow strings\n";

        static const struct NegativeTestData {
            long long        d_line;
            bsl::string_view d_str;
            size_type        d_pos;
            size_type        d_len;
        } k_NDATA[] = {
            { L_ , "",                  1,  0 },
            { L_ , "",                  0,  1 },
            { L_ , "",                  1,  1 },
            { L_ , "0123456789abcdef", 17,  0 },
            { L_ , "0123456789abcdef", 16,  1 },
            { L_ , "0123456789abcdef", 10,  7 },
        };

        static bsl::size_t NUM_NTESTS = sizeof k_NDATA / sizeof *k_NDATA;

        for (bsl::size_t i = 0; i < NUM_NTESTS; ++i) {
            bslma::TestAllocator     sa("bslstring", veryVeryVerbose);
            const NegativeTestData&  k_TEST     = k_NDATA[i];
            const long long          k_LINE     = k_TEST.d_line;
            const bsl::string_view&  k_STRVIEW  = k_TEST.d_str;
            const bslstl::StringRef& k_STRREF   = k_STRVIEW;
            const bsl::string        k_BSLSTR(k_STRVIEW, &sa);
#ifdef BSLSTL_STRING_VIEW_IS_ALIASED
            const std::string        k_STDSTR(k_STRVIEW);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            const std::pmr::string   k_PMRSTR(k_STRVIEW);
#endif
#else
            const std::string       k_STDSTR(k_STRREF.data(), k_STRREF.size());
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            const std::pmr::string  k_PMRSTR(k_STRREF.data(), k_STRREF.size());
#endif
#endif
            const bdlb::IndexSpan    k_SPAN(k_TEST.d_pos, k_TEST.d_len);

            if (veryVerbose) {
                P_(k_LINE); P_(k_STRVIEW); P_(k_SPAN);
            }

            bsls::AssertTestHandlerGuard g; (void)g;

            ASSERT_FAIL(Util::bind(k_STRVIEW, k_SPAN));
            ASSERT_FAIL(Util::bind(k_BSLSTR,  k_SPAN));
            ASSERT_FAIL(Util::bind(k_STRREF,  k_SPAN));
            ASSERT_FAIL(Util::bind(k_STDSTR,  k_SPAN));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            ASSERT_FAIL(Util::bind(k_PMRSTR,  k_SPAN));
#endif
        }
#endif

        if (veryVeryVerbose) cout << "Positive testing of wide strings\n";

        static const struct TestDataWide {
            long long         d_line;
            bsl::wstring_view d_str;
            size_type         d_pos;
            size_type         d_len;
            bsl::wstring_view d_expected;
        } k_WDATA[] = {
            { L_ , L"",                  0,  0, L""           },
            { L_ , L"0123456789abcdef",  0,  0, L""           },
            { L_ , L"0123456789abcdef",  0,  5, L"01234"      },
            { L_ , L"0123456789abcdef",  5,  5, L"56789"      },
            { L_ , L"0123456789abcdef",  3, 10, L"3456789abc" },
            { L_ , L"0123456789abcdef", 16,  0, L""           },
            { L_ , L"0123456789abcdef", 15,  1, L"f"          },
            { L_ , L"0123456789abcdef",  0,  1, L"0"          },
            { L_ , L"0123456789abcdef", 10,  6, L"abcdef"     },
        };

        static bsl::size_t NUM_WTESTS = sizeof k_WDATA / sizeof *k_WDATA;

        for (bsl::size_t i = 0; i < NUM_WTESTS; ++i) {
            bslma::TestAllocator         sa("bslstring", veryVeryVerbose);
            const TestDataWide&          k_TEST     = k_WDATA[i];
            const long long              k_LINE    = k_TEST.d_line;
            const bsl::wstring_view&     k_STRVIEW = k_TEST.d_str;
            const bslstl::StringRefWide& k_STRREF  = k_STRVIEW;
            const bsl::wstring           k_BSLSTR(k_STRVIEW, &sa);
#ifdef BSLSTL_STRING_VIEW_IS_ALIASED
            const std::wstring           k_STDSTR(k_STRVIEW);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            const std::pmr::wstring      k_PMRSTR(k_STRVIEW);
#endif
#else
            const std::wstring      k_STDSTR(k_STRREF.data(), k_STRREF.size());
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            const std::pmr::wstring k_PMRSTR(k_STRREF.data(), k_STRREF.size());
#endif
#endif
            const bdlb::IndexSpan        k_SPAN(k_TEST.d_pos, k_TEST.d_len);
            const bsl::wstring_view&     k_EXPECTED = k_TEST.d_expected;

            if (veryVerbose) {
                P_(k_LINE); P_(k_SPAN);
            }


#define TEST_BIND_VERIFY(ResultType, InputName)                               \
    ASSERT(k_EXPECTED == bound);                                              \
    ASSERT(bound.data() == InputName.data() + k_SPAN.position());             \
    ASSERT(bound.length() == k_SPAN.length())

#define TEST_BIND_IMPLICIT(ResultType, InputName)                             \
    {                                                                         \
        const ResultType bound = Util::bind(InputName, k_SPAN);               \
        TEST_BIND_VERIFY(ResultType, InputName);                              \
    }

#define TEST_BIND_EXPLICIT(ResultType, InputName)                             \
    {                                                                         \
        const ResultType bound(Util::bind(InputName, k_SPAN));                \
        TEST_BIND_VERIFY(ResultType, InputName);                              \
    }

#define TEST_BIND_ASSIGN(ResultType, InputName)                               \
    {                                                                         \
        ResultType bound;                                                     \
        bound = Util::bind(InputName, k_SPAN);                                \
        TEST_BIND_VERIFY(ResultType, InputName);                              \
    }

            TEST_BIND_IMPLICIT(bsl::wstring_view, k_STRVIEW);
            TEST_BIND_IMPLICIT(bsl::wstring_view, k_STRREF);
            TEST_BIND_IMPLICIT(bsl::wstring_view, k_BSLSTR);
            TEST_BIND_IMPLICIT(bsl::wstring_view, k_STDSTR);
            TEST_BIND_ASSIGN(bsl::wstring_view,   k_STRVIEW);
            TEST_BIND_ASSIGN(bsl::wstring_view,   k_STRREF);
            TEST_BIND_ASSIGN(bsl::wstring_view,   k_BSLSTR);
            TEST_BIND_ASSIGN(bsl::wstring_view,   k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_BIND_IMPLICIT(bsl::wstring_view, k_PMRSTR);
            TEST_BIND_ASSIGN(bsl::wstring_view,   k_PMRSTR);
#endif

            TEST_BIND_IMPLICIT(bslstl::StringRefWide, k_STRVIEW);
            TEST_BIND_IMPLICIT(bslstl::StringRefWide, k_STRREF);
            TEST_BIND_IMPLICIT(bslstl::StringRefWide, k_BSLSTR);
            TEST_BIND_IMPLICIT(bslstl::StringRefWide, k_STDSTR);
            TEST_BIND_ASSIGN(bslstl::StringRefWide,   k_STRVIEW);
            TEST_BIND_ASSIGN(bslstl::StringRefWide,   k_STRREF);
            TEST_BIND_ASSIGN(bslstl::StringRefWide,   k_BSLSTR);
            TEST_BIND_ASSIGN(bslstl::StringRefWide,   k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_BIND_IMPLICIT(bslstl::StringRefWide, k_PMRSTR);
            TEST_BIND_ASSIGN(bslstl::StringRefWide,   k_PMRSTR);
#endif

            TEST_BIND_IMPLICIT(bslstl::StringRefWide&, k_STRVIEW);
            TEST_BIND_IMPLICIT(bslstl::StringRefWide&, k_STRREF);
            TEST_BIND_IMPLICIT(bslstl::StringRefWide&, k_BSLSTR);
            TEST_BIND_IMPLICIT(bslstl::StringRefWide&, k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_BIND_IMPLICIT(bslstl::StringRefWide&, k_PMRSTR);
#endif

#undef TEST_BIND_ASSIGN
#undef TEST_BIND_EXPLICIT
#undef TEST_BIND_IMPLICIT
#undef TEST_BIND_VERIFY

#define TEST_BIND_VERIFY(ResultType, InputName)                               \
    ASSERT(k_EXPECTED == bound);                                              \
    ASSERT(bound.length() == k_SPAN.length());

#define TEST_BIND_EXPLICIT(ResultType, InputName)                             \
    {                                                                         \
        const ResultType bound(Util::bind(InputName, k_SPAN));                \
        TEST_BIND_VERIFY(ResultType, InputName);                              \
    }

#define TEST_BIND_ASSIGN(ResultType, InputName)                               \
    {                                                                         \
        ResultType bound;                                                     \
        bound = Util::bind(InputName, k_SPAN);                                \
        TEST_BIND_VERIFY(ResultType, InputName);                              \
    }

            TEST_BIND_EXPLICIT(bsl::wstring, k_STRVIEW);
            TEST_BIND_EXPLICIT(bsl::wstring, k_STRREF);
            TEST_BIND_EXPLICIT(bsl::wstring, k_BSLSTR);
            TEST_BIND_EXPLICIT(bsl::wstring, k_STDSTR);
            TEST_BIND_ASSIGN(bsl::wstring,   k_STRVIEW);
            TEST_BIND_ASSIGN(bsl::wstring,   k_STRREF);
            TEST_BIND_ASSIGN(bsl::wstring,   k_BSLSTR);
            TEST_BIND_ASSIGN(bsl::wstring,   k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_BIND_EXPLICIT(bsl::wstring, k_PMRSTR);
            TEST_BIND_ASSIGN(bsl::wstring,   k_PMRSTR);
#endif

#ifdef BSLSTL_STRING_VIEW_IS_ALIASED
            TEST_BIND_EXPLICIT(std::wstring, k_STRVIEW);
            TEST_BIND_EXPLICIT(std::wstring, k_STRREF);
            TEST_BIND_EXPLICIT(std::wstring, k_BSLSTR);
            TEST_BIND_EXPLICIT(std::wstring, k_STDSTR);
            TEST_BIND_ASSIGN(std::wstring,   k_STRVIEW);
            TEST_BIND_ASSIGN(std::wstring,   k_STRREF);
            TEST_BIND_ASSIGN(std::wstring,   k_BSLSTR);
            TEST_BIND_ASSIGN(std::wstring,   k_STDSTR);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_BIND_EXPLICIT(std::wstring, k_PMRSTR);
            TEST_BIND_ASSIGN(std::wstring,   k_PMRSTR);
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            TEST_BIND_EXPLICIT(std::pmr::wstring, k_STRVIEW);
            TEST_BIND_EXPLICIT(std::pmr::wstring, k_STRREF);
            TEST_BIND_EXPLICIT(std::pmr::wstring, k_BSLSTR);
            TEST_BIND_EXPLICIT(std::pmr::wstring, k_STDSTR);
            TEST_BIND_EXPLICIT(std::pmr::wstring, k_PMRSTR);
            TEST_BIND_ASSIGN(std::pmr::wstring,   k_STRVIEW);
            TEST_BIND_ASSIGN(std::pmr::wstring,   k_STRREF);
            TEST_BIND_ASSIGN(std::pmr::wstring,   k_BSLSTR);
            TEST_BIND_ASSIGN(std::pmr::wstring,   k_STDSTR);
            TEST_BIND_ASSIGN(std::pmr::wstring,   k_PMRSTR);
#endif
#endif

#undef TEST_BIND_ASSIGN
#undef TEST_BIND_EXPLICIT
#undef TEST_BIND_VERIFY
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (veryVeryVerbose) cout << "Negative testing of wide strings\n";

        static const struct NegativeTestDataWide {
            long long         d_line;
            bsl::wstring_view d_str;
            size_type         d_pos;
            size_type         d_len;
        } k_NWDATA[] = {
            { L_ , L"",                  1,  0 },
            { L_ , L"",                  0,  1 },
            { L_ , L"",                  1,  1 },
            { L_ , L"0123456789abcdef", 17,  0 },
            { L_ , L"0123456789abcdef", 16,  1 },
            { L_ , L"0123456789abcdef", 10,  7 },
        };

        static bsl::size_t NUM_NWTESTS = sizeof k_NWDATA / sizeof *k_NWDATA;

        for (bsl::size_t i = 0; i < NUM_NWTESTS; ++i) {
            bslma::TestAllocator         sa("bslstring", veryVeryVerbose);
            const NegativeTestDataWide&  k_TEST     = k_NWDATA[i];
            const long long              k_LINE     = k_TEST.d_line;
            const bsl::wstring_view&     k_STRVIEW  = k_TEST.d_str;
            const bslstl::StringRefWide& k_STRREF   = k_STRVIEW;
            const bsl::wstring           k_BSLSTR(k_STRVIEW, &sa);
#ifdef BSLSTL_STRING_VIEW_IS_ALIASED
            const std::wstring           k_STDSTR(k_STRVIEW);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            const std::pmr::wstring      k_PMRSTR(k_STRVIEW);
#endif
#else
            const std::wstring      k_STDSTR(k_STRREF.data(), k_STRREF.size());
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            const std::pmr::wstring k_PMRSTR(k_STRREF.data(), k_STRREF.size());
#endif
#endif
            const bdlb::IndexSpan   k_SPAN(k_TEST.d_pos, k_TEST.d_len);

            if (veryVerbose) {
                P_(k_LINE); P_(k_SPAN);
            }

            bsls::AssertTestHandlerGuard g; (void)g;

            ASSERT_FAIL(Util::bind(k_STRVIEW, k_SPAN));
            ASSERT_FAIL(Util::bind(k_BSLSTR,  k_SPAN));
            ASSERT_FAIL(Util::bind(k_STRREF,  k_SPAN));
            ASSERT_FAIL(Util::bind(k_STDSTR,  k_SPAN));
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            ASSERT_FAIL(Util::bind(k_PMRSTR,  k_SPAN));
#endif
        }
#endif
      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Call the utility functions to verify their existence and basics.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============\n";

        const bsl::string_view str("0123456789abcdef");

        ASSERT(bdlb::IndexSpan(5, 3) == Util::create(str, str.begin() + 5, 3));

        ASSERT("89abc" == Util::bind(str, bdlb::IndexSpan(8, 5)));

      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
    }
    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
