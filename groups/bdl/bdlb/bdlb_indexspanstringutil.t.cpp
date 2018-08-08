// bdlb_indexspanstringutil.t.cpp                                     -*-C++-*-
#include <bdlb_indexspanstringutil.h>

#include <bslim_testutil.h>

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
// [2] bslstl::StringRef bind(string, span);
// [3] IndexSpan create(string, substring);
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
    bool veryVeryVerbose = argc > 4; (void)veryVeryVerbose;

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
// Finally, we verify the the resulting 'IndexSpan' objects correctly describe
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
    const bslstl::StringRef fullRef(full);
//..
// Then we demonstrate binding 'IndexSpan' object to that reference:
//..
    ASSERT(bdlb::IndexSpanStringUtil::bind(fullRef, first) == "James");

    ASSERT(bdlb::IndexSpanStringUtil::bind(fullRef, middle) == "Tiberius");

    ASSERT(bdlb::IndexSpanStringUtil::bind(fullRef, last) == "Kirk");
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
        //: 1 It creates the expected 'IndexSpan'.
        //: 2 Bad arguments assert in the proper build mode.
        //: 3 All functions are callable with 'bsl::string' objects.
        //
        // Plan:
        //: 1 Use table based testing with both good and bad calls.
        //: 2 Test all overloads for all rows of the table.
        //: 3 Test both wide and narrow character versions.
        //
        // Testing:
        //   IndexSpan create(string, substring);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING CREATE"
                             "\n==============\n";

        typedef bdlb::IndexSpan::size_type size_type;

        static struct TestData {
            long long         d_line;
            bslstl::StringRef d_str;
            char              d_bad;  // 'X' if precondition violation
            size_type         d_pos;
            size_type         d_len;
        } k_DATA[] = {
            { L_ , "",                 ' ',  0,  0 },
            { L_ , "",                 'X',  1,  0 },
            { L_ , "",                 'X',  0,  1 },
            { L_ , "",                 'X',  1,  1 },
            { L_ , "0123456789abcdef", ' ',  0,  0 },
            { L_ , "0123456789abcdef", ' ',  0,  5 },
            { L_ , "0123456789abcdef", ' ',  5,  5 },
            { L_ , "0123456789abcdef", ' ',  3, 10 },
            { L_ , "0123456789abcdef", ' ', 16,  0 },
            { L_ , "0123456789abcdef", ' ', 15,  1 },
            { L_ , "0123456789abcdef", ' ',  0,  1 },
            { L_ , "0123456789abcdef", 'X', 17,  0 },
            { L_ , "0123456789abcdef", 'X', 16,  1 },
            { L_ , "0123456789abcdef", 'X', 10,  7 },
            { L_ , "0123456789abcdef", ' ', 10,  6 },
        };

        static bsl::size_t NUM_TESTS = sizeof k_DATA / sizeof *k_DATA;

        for (bsl::size_t i = 0; i < NUM_TESTS; ++i) {
            const TestData&          k_TEST     = k_DATA[i];
            const long long          k_LINE     = k_TEST.d_line;
            const bslstl::StringRef& k_STR      = k_TEST.d_str;
            const bool               k_GOOD     = k_TEST.d_bad != 'X';
            const bdlb::IndexSpan    k_EXPECTED(k_TEST.d_pos, k_TEST.d_len);
            const bsl::string&       k_BSLSTR = k_STR;

            if (veryVerbose) {
                P_(k_LINE); P_(k_STR); P_(k_GOOD); P(k_EXPECTED);
            }

            const size_type k_BEGIN_POS = k_EXPECTED.position();
            const size_type k_END_POS   = k_BEGIN_POS + k_EXPECTED.length();

            const size_type k_LENGTH = k_EXPECTED.length();

            const bslstl::StringRef k_SUBSTR(
                                          k_STR.data() + k_EXPECTED.position(),
                                          k_EXPECTED.length());
            const bslstl::StringRef k_BSLSUBSTR(
                                       k_BSLSTR.data() + k_EXPECTED.position(),
                                       k_EXPECTED.length());

            if (k_GOOD) {
                bdlb::IndexSpan created;

                created = Util::createFromPositions(k_STR,
                                                    k_BEGIN_POS,
                                                    k_END_POS);
                ASSERTV(created, k_EXPECTED, k_EXPECTED == created);
                created = Util::createFromPositions(k_BSLSTR,
                                                    k_BEGIN_POS,
                                                    k_END_POS);
                ASSERTV(created, k_EXPECTED, k_EXPECTED == created);

                created = Util::create(k_STR, k_BEGIN_POS, k_LENGTH);
                ASSERTV(created, k_EXPECTED, k_EXPECTED == created);
                created = Util::create(k_BSLSTR, k_BEGIN_POS, k_LENGTH);
                ASSERTV(created, k_EXPECTED, k_EXPECTED == created);

                {
                    typedef bslstl::StringRef::const_iterator Cit;
                    const Cit k_BEGIN = k_STR.begin() + k_EXPECTED.position();
                    const Cit k_END = k_BEGIN + k_EXPECTED.length();

                    created = Util::create(k_STR, k_BEGIN, k_LENGTH);
                    ASSERTV(created, k_EXPECTED, k_EXPECTED == created);

                    created = Util::create(k_STR, k_BEGIN, k_END);
                    ASSERTV(created, k_EXPECTED, k_EXPECTED == created);
                }

                {
                    typedef bsl::string::const_iterator StrCit;
                    const StrCit k_BEGIN = k_BSLSTR.begin()
                                           + k_EXPECTED.position();
                    const StrCit k_END = k_BEGIN + k_EXPECTED.length();

                    created = Util::create(k_BSLSTR, k_BEGIN, k_LENGTH);
                    ASSERTV(created, k_EXPECTED, k_EXPECTED == created);

                    created = Util::create(k_BSLSTR, k_BEGIN, k_END);
                    ASSERTV(created, k_EXPECTED, k_EXPECTED == created);
                }

                created = Util::create(k_STR, k_SUBSTR);
                ASSERTV(created, k_EXPECTED, k_EXPECTED == created);
                created = Util::create(k_BSLSTR, k_BSLSUBSTR);
                ASSERTV(created, k_EXPECTED, k_EXPECTED == created);
            }
            else {
#ifdef BDE_BUILD_TARGET_EXC
                using bsls::AssertFailureHandlerGuard;
                using bsls::AssertTest;
                AssertFailureHandlerGuard g(AssertTest::failTestDriver);
                (void)g;

                ASSERT_FAIL(Util::createFromPositions(k_STR,
                                                      k_BEGIN_POS,
                                                      k_END_POS));
                ASSERT_FAIL(Util::createFromPositions(k_BSLSTR,
                                                      k_BEGIN_POS,
                                                      k_END_POS));

                ASSERT_FAIL(Util::create(k_STR,    k_BEGIN_POS, k_LENGTH));
                ASSERT_FAIL(Util::create(k_BSLSTR, k_BEGIN_POS, k_LENGTH));

                {
                    typedef bslstl::StringRef::const_iterator Cit;
                    const Cit k_BEGIN = k_STR.begin() + k_EXPECTED.position();
                    const Cit k_END = k_BEGIN + k_EXPECTED.length();

                    ASSERT_FAIL(Util::create(k_STR, k_BEGIN, k_LENGTH));
                    ASSERT_FAIL(Util::create(k_STR, k_BEGIN, k_END));
                }

                {
                    typedef bsl::string::const_iterator StrCit;
                    const StrCit k_BEGIN = k_BSLSTR.begin()
                                           + k_EXPECTED.position();
                    const StrCit k_END = k_BEGIN + k_EXPECTED.length();

                    ASSERT_FAIL(Util::create(k_BSLSTR, k_BEGIN, k_LENGTH));
                    ASSERT_FAIL(Util::create(k_BSLSTR, k_BEGIN, k_END));
                }

                ASSERT_FAIL(Util::create(k_STR,    k_SUBSTR));
                ASSERT_FAIL(Util::create(k_BSLSTR, k_BSLSUBSTR));
#endif
            }
        }

        static struct TestDataWide {
            long long             d_line;
            bslstl::StringRefWide d_str;
            char                  d_bad;  // 'X' if precondition violation
            size_type             d_pos;
            size_type             d_len;
        } k_WDATA[] = {
            { L_ , L"",                 ' ',  0,  0 },
            { L_ , L"",                 'X',  1,  0 },
            { L_ , L"",                 'X',  0,  1 },
            { L_ , L"",                 'X',  1,  1 },
            { L_ , L"0123456789abcdef", ' ',  0,  0 },
            { L_ , L"0123456789abcdef", ' ',  0,  5 },
            { L_ , L"0123456789abcdef", ' ',  5,  5 },
            { L_ , L"0123456789abcdef", ' ',  3, 10 },
            { L_ , L"0123456789abcdef", ' ', 16,  0 },
            { L_ , L"0123456789abcdef", ' ', 15,  1 },
            { L_ , L"0123456789abcdef", ' ',  0,  1 },
            { L_ , L"0123456789abcdef", 'X', 17,  0 },
            { L_ , L"0123456789abcdef", 'X', 16,  1 },
            { L_ , L"0123456789abcdef", 'X', 10,  7 },
            { L_ , L"0123456789abcdef", ' ', 10,  6 },
        };

        static bsl::size_t NUM_WTESTS = sizeof k_WDATA / sizeof *k_WDATA;

        for (bsl::size_t i = 0; i < NUM_WTESTS; ++i) {
            const TestDataWide&          k_TEST     = k_WDATA[i];
            const long long              k_LINE     = k_TEST.d_line;
            const bslstl::StringRefWide& k_STR      = k_TEST.d_str;
            const bool                   k_GOOD     = k_TEST.d_bad != 'X';
            const bdlb::IndexSpan        k_EXPECTED(k_TEST.d_pos,
                                                    k_TEST.d_len);
            const bsl::wstring&          k_BSLSTR = k_STR;

            if (veryVerbose) {
                P_(k_LINE); P_(k_GOOD); P(k_EXPECTED);
            }

            const size_type k_BEGIN_POS = k_EXPECTED.position();
            const size_type k_END_POS   = k_BEGIN_POS + k_EXPECTED.length();

            const size_type k_LENGTH = k_EXPECTED.length();

            const bslstl::StringRefWide k_SUBSTR(
                                          k_STR.data() + k_EXPECTED.position(),
                                          k_LENGTH);
            const bslstl::StringRefWide k_BSLSUBSTR(
                                       k_BSLSTR.data() + k_EXPECTED.position(),
                                       k_LENGTH);

            if (k_GOOD) {
                bdlb::IndexSpan created;

                created = Util::createFromPositions(k_STR,
                                                    k_BEGIN_POS,
                                                    k_END_POS);
                ASSERTV(created, k_EXPECTED, k_EXPECTED == created);
                created = Util::createFromPositions(k_BSLSTR,
                                                    k_BEGIN_POS,
                                                    k_END_POS);
                ASSERTV(created, k_EXPECTED, k_EXPECTED == created);

                created = Util::create(k_STR, k_BEGIN_POS, k_LENGTH);
                ASSERTV(created, k_EXPECTED, k_EXPECTED == created);
                created = Util::create(k_BSLSTR, k_BEGIN_POS, k_LENGTH);
                ASSERTV(created, k_EXPECTED, k_EXPECTED == created);

                {
                    typedef bslstl::StringRefWide::const_iterator Cit;
                    const Cit k_BEGIN = k_STR.begin() + k_EXPECTED.position();
                    const Cit k_END = k_BEGIN + k_EXPECTED.length();

                    created = Util::create(k_STR, k_BEGIN, k_LENGTH);
                    ASSERTV(created, k_EXPECTED, k_EXPECTED == created);

                    created = Util::create(k_STR, k_BEGIN, k_END);
                    ASSERTV(created, k_EXPECTED, k_EXPECTED == created);
                }

                {
                    typedef bsl::wstring::const_iterator StrCit;
                    const StrCit k_BEGIN = k_BSLSTR.begin()
                        + k_EXPECTED.position();
                    const StrCit k_END = k_BEGIN + k_EXPECTED.length();

                    created = Util::create(k_BSLSTR, k_BEGIN, k_LENGTH);
                    ASSERTV(created, k_EXPECTED, k_EXPECTED == created);

                    created = Util::create(k_BSLSTR, k_BEGIN, k_END);
                    ASSERTV(created, k_EXPECTED, k_EXPECTED == created);
                }

                created = Util::create(k_STR, k_SUBSTR);
                ASSERTV(created, k_EXPECTED, k_EXPECTED == created);
                created = Util::create(k_BSLSTR, k_BSLSUBSTR);
                ASSERTV(created, k_EXPECTED, k_EXPECTED == created);
            }
            else {
#ifdef BDE_BUILD_TARGET_EXC
                using bsls::AssertFailureHandlerGuard;
                using bsls::AssertTest;
                AssertFailureHandlerGuard g(AssertTest::failTestDriver);
                (void)g;

                ASSERT_FAIL(Util::createFromPositions(k_STR,
                                                      k_BEGIN_POS,
                                                      k_END_POS));
                ASSERT_FAIL(Util::createFromPositions(k_BSLSTR,
                                                      k_BEGIN_POS,
                                                      k_END_POS));

                ASSERT_FAIL(Util::create(k_STR,    k_BEGIN_POS, k_LENGTH));
                ASSERT_FAIL(Util::create(k_BSLSTR, k_BEGIN_POS, k_LENGTH));

                {
                    typedef bslstl::StringRefWide::const_iterator Cit;
                    const Cit k_BEGIN = k_STR.begin() + k_EXPECTED.position();
                    const Cit k_END = k_BEGIN + k_EXPECTED.length();

                    ASSERT_FAIL(Util::create(k_STR, k_BEGIN, k_LENGTH));
                    ASSERT_FAIL(Util::create(k_STR, k_BEGIN, k_END));
                }

                {
                    typedef bsl::wstring::const_iterator StrCit;
                    const StrCit k_BEGIN = k_BSLSTR.begin()
                                           + k_EXPECTED.position();
                    const StrCit k_END = k_BEGIN + k_EXPECTED.length();

                    ASSERT_FAIL(Util::create(k_BSLSTR, k_BEGIN, k_LENGTH));
                    ASSERT_FAIL(Util::create(k_BSLSTR, k_BEGIN, k_END));
                }

                ASSERT_FAIL(Util::create(k_STR,    k_SUBSTR));
                ASSERT_FAIL(Util::create(k_BSLSTR, k_BSLSUBSTR));
#endif
            }
        }
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
        //: 4 'bind' can be called with a 'bsl::string' argument.
        //
        // Plan:
        //: 1 Table based testing with values that test
        //:   1 Valid and invalid substrings of an empty string
        //:   2 Valid substrings of a string
        //:   3 Boundary substrings at the beginning and the end of the string
        //:   4 Spans that does not define a valid substring
        //:   5 The same calls with a 'bsl::string' first argument
        //
        // Testing:
        //   bslstl::StringRef bind(string, span);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING BIND"
                             "\n============\n";

        typedef bdlb::IndexSpan::size_type size_type;

        static struct TestData {
            long long         d_line;
            bslstl::StringRef d_str;
            char              d_bad;  // 'X' if precondition violation
            size_type         d_pos;
            size_type         d_len;
            bslstl::StringRef d_expected;
        } k_DATA[] = {
            { L_ , "",                 ' ',  0,  0, ""           },
            { L_ , "",                 'X',  1,  0, ""           },
            { L_ , "",                 'X',  0,  1, ""           },
            { L_ , "",                 'X',  1,  1, ""           },
            { L_ , "0123456789abcdef", ' ',  0,  0, ""           },
            { L_ , "0123456789abcdef", ' ',  0,  5, "01234"      },
            { L_ , "0123456789abcdef", ' ',  5,  5, "56789"      },
            { L_ , "0123456789abcdef", ' ',  3, 10, "3456789abc" },
            { L_ , "0123456789abcdef", ' ', 16,  0, ""           },
            { L_ , "0123456789abcdef", ' ', 15,  1, "f"          },
            { L_ , "0123456789abcdef", ' ',  0,  1, "0"          },
            { L_ , "0123456789abcdef", 'X', 17,  0, ""           },
            { L_ , "0123456789abcdef", 'X', 16,  1, ""           },
            { L_ , "0123456789abcdef", 'X', 10,  7, ""           },
            { L_ , "0123456789abcdef", ' ', 10,  6, "abcdef"     },
        };

        static bsl::size_t NUM_TESTS = sizeof k_DATA / sizeof *k_DATA;

        for (bsl::size_t i = 0; i < NUM_TESTS; ++i) {
            const TestData&          k_TEST     = k_DATA[i];
            const long long          k_LINE     = k_TEST.d_line;
            const bslstl::StringRef& k_STR      = k_TEST.d_str;
            const bool               k_GOOD     = k_TEST.d_bad != 'X';
            const bdlb::IndexSpan    k_SPAN(k_TEST.d_pos, k_TEST.d_len);
            const bslstl::StringRef& k_EXPECTED = k_TEST.d_expected;
            const bsl::string&       k_BSLSTR   = k_STR;

            if (veryVerbose) {
                P_(k_LINE); P_(k_STR); P_(k_GOOD); P_(k_SPAN); P(k_EXPECTED);
            }

            if (k_GOOD) {
                bslstl::StringRef bound = Util::bind(k_STR, k_SPAN);
                ASSERTV(bound, k_EXPECTED, k_EXPECTED == bound);
                ASSERT(bound.data()   == k_STR.data() + k_SPAN.position());
                ASSERT(bound.length() == k_SPAN.length());

                bound = Util::bind(k_BSLSTR, k_SPAN);
                ASSERTV(bound, k_EXPECTED, k_EXPECTED == bound);
                ASSERT(bound.data()   == k_BSLSTR.data() + k_SPAN.position());
                ASSERT(bound.length() == k_SPAN.length());
            }
            else {
#ifdef BDE_BUILD_TARGET_EXC
                using bsls::AssertFailureHandlerGuard;
                using bsls::AssertTest;
                AssertFailureHandlerGuard g(AssertTest::failTestDriver);
                (void)g;

                ASSERT_FAIL(Util::bind(k_STR,    k_SPAN));
                ASSERT_FAIL(Util::bind(k_BSLSTR, k_SPAN));
#endif
            }
        }

        static struct TestDataWide {
            long long             d_line;
            bslstl::StringRefWide d_str;
            char                  d_bad;  // 'X' if precondition violation
            size_type             d_pos;
            size_type             d_len;
            bslstl::StringRefWide d_expected;
        } k_WDATA[] = {
            { L_ , L"",                 ' ',  0,  0, L""           },
            { L_ , L"",                 'X',  1,  0, L""           },
            { L_ , L"",                 'X',  0,  1, L""           },
            { L_ , L"",                 'X',  1,  1, L""           },
            { L_ , L"0123456789abcdef", ' ',  0,  0, L""           },
            { L_ , L"0123456789abcdef", ' ',  0,  5, L"01234"      },
            { L_ , L"0123456789abcdef", ' ',  5,  5, L"56789"      },
            { L_ , L"0123456789abcdef", ' ',  3, 10, L"3456789abc" },
            { L_ , L"0123456789abcdef", ' ', 16,  0, L""           },
            { L_ , L"0123456789abcdef", ' ', 15,  1, L"f"          },
            { L_ , L"0123456789abcdef", ' ',  0,  1, L"0"          },
            { L_ , L"0123456789abcdef", 'X', 17,  0, L""           },
            { L_ , L"0123456789abcdef", 'X', 16,  1, L""           },
            { L_ , L"0123456789abcdef", 'X', 10,  7, L""           },
            { L_ , L"0123456789abcdef", ' ', 10,  6, L"abcdef"     },
        };

        static bsl::size_t NUM_WTESTS = sizeof k_WDATA / sizeof *k_WDATA;

        for (bsl::size_t i = 0; i < NUM_WTESTS; ++i) {
            const TestDataWide&          k_TEST     = k_WDATA[i];
            const long long              k_LINE     = k_TEST.d_line;
            const bslstl::StringRefWide& k_STR      = k_TEST.d_str;
            const bool                   k_GOOD     = k_TEST.d_bad != 'X';
            const bdlb::IndexSpan        k_SPAN(k_TEST.d_pos, k_TEST.d_len);
            const bslstl::StringRefWide& k_EXPECTED = k_TEST.d_expected;
            const bsl::wstring&          k_BSLSTR   = k_STR;

            if (veryVerbose) {
                P_(k_LINE); P_(k_GOOD); P_(k_SPAN);
            }

            if (k_GOOD) {
                bslstl::StringRefWide bound = Util::bind(k_STR, k_SPAN);
                ASSERT(k_EXPECTED == bound);
                ASSERT(bound.data()   == k_STR.data() + k_SPAN.position());
                ASSERT(bound.length() == k_SPAN.length());

                bound = Util::bind(k_BSLSTR, k_SPAN);
                ASSERT(k_EXPECTED == bound);
                ASSERT(bound.data()   == k_BSLSTR.data() + k_SPAN.position());
                ASSERT(bound.length() == k_SPAN.length());
            }
            else {
#ifdef BDE_BUILD_TARGET_EXC
                using bsls::AssertFailureHandlerGuard;
                using bsls::AssertTest;
                AssertFailureHandlerGuard g(AssertTest::failTestDriver);
                (void)g;

                ASSERT_FAIL(Util::bind(k_STR,    k_SPAN));
                ASSERT_FAIL(Util::bind(k_BSLSTR, k_SPAN));
#endif
            }
        }
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

        bslstl::StringRef str("0123456789abcdef");

        ASSERT(bdlb::IndexSpan(5, 3) == Util::create(str, str.data() + 5, 3));

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
// Copyright 2018 Bloomberg Finance L.P.
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
