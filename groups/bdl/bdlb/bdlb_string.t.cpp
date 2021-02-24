// bdlb_string.t.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlb_string.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bsls_platform.h>
#include <bsls_review.h>

#include <bsl_algorithm.h>   // 'bsl::transform'
#include <bsl_clocale.h>     // 'bsl::setlocale'
#include <bsl_cstddef.h>
#include <bsl_cstdio.h>      // 'bsl::sprintf'
#include <bsl_cstdlib.h>     // 'bsl::atoi'
#include <bsl_cstring.h>     // 'bsl::strcmp', 'bsl::memset'
#include <bsl_iostream.h>

#include <ctype.h>           // 'tolower', 'toupper'

#if defined(BSLS_PLATFORM_CMP_MSVC)
#include <bsl_c_string.h>    // 'bsl::_stricmp', 'bsl::_strnicmp'
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#else
#include <strings.h>         // 'bsl::strcasecmp', 'bsl::strncasecmp'
#endif

using namespace BloombergLP;
using bsl::cout; using bsl::flush; using bsl::endl; using bsl::cerr;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The component under test is a utility component.  The functions will be
// tested individually.
//-----------------------------------------------------------------------------
// [ 3] areEqualCaseless(cchar *lhs, cchar *rhs);
// [ 3] areEqualCaseless(cchar *lhs, cchar *rhs, int rhsL);
// [ 3] areEqualCaseless(cchar *lhs, cBslStr& rhs);
// [ 3] areEqualCaseless(cchar *lhs, int lshL, cchar *rhs);
// [ 3] areEqualCaseless(cchar *lhs, int lshL, cchar *rhs, int rhsL);
// [ 3] areEqualCaseless(cchar *lhs, int lshL, cBslStr& rhs);
// [ 3] areEqualCaseless(cBslStr& lhs, cchar *rhs);
// [ 3] areEqualCaseless(cBslStr& lhs, cchar *rhs, int rhsL);
// [ 3] areEqualCaseless(cBslStr& lhs, cBslStr& rhs);
// [11] copy(cchar *string, bslma::Allocator *basicAllocator);
// [11] copy(cchar *string, int length, bslma::Allocator *basicAllocator);
// [11] copy(cBslStr& string, bslma::Allocator *basicAllocator);
// [ 4] lowerCaseCmp(cchar *lhsStr, cchar *rhsStr);
// [ 4] lowerCaseCmp(cchar *lhsStr, cchar *rhsStr, int rhsL);
// [ 4] lowerCaseCmp(cchar *lhsStr, cBslStr& rhsStr);
// [ 4] lowerCaseCmp(cchar *lhsStr, int lshL, cchar *rhsStr);
// [ 4] lowerCaseCmp(cchar *lhsStr, int lshL, cchar *rhsStr, int rhsL);
// [ 4] lowerCaseCmp(cchar *lhsStr, int lshL, cBslStr& rhsStr);
// [ 4] lowerCaseCmp(cBslStr& lhsStr, cchar *rhsStr);
// [ 4] lowerCaseCmp(cBslStr& lhsStr, cchar *rhsStr, int rhsL);
// [ 4] lowerCaseCmp(cBslStr& lhsStr, cBslStr& rhsStr);
// [ 5] ltrim(char *str);
// [ 5] ltrim(char *str, int *L);
// [ 5] ltrim(bsl::string *str);
// [ 5] ltrim(std::string *str);
// [ 5] ltrim(std::pmr::string *str);
// [ 6] pad(bsl::string *str, int numChars, char padChar = ' ');
// [ 6] pad(std::string *str, int numChars, char padChar = ' ');
// [ 6] pad(std::pmr::string *str, size_type numChars, char padChar = ' ');
// [ 5] rtrim(char *str);
// [ 5] rtrim(bsl::string *str);
// [ 5] rtrim(std::string *str);
// [ 5] rtrim(std::pmr::string *str);
// [ 5] rtrim(cchar *str, int *L);
// [ 9] strstr(cchar *str, int strL, cchar *subStr, int subStrL);
// [ 9] strstrCaseless(cchar *str, int strL, cchar *subStr, int subStrL);
// [ 9] strrstr(cchar *str, int strL, cchar *subStr, int subStrL);
// [ 9] strrstrCaseless(cchar *str, int strL, cchar *subStr, int subStrL);
// [ 7] strnlen(cchar *str, int maximumLength);
// [ 8] toFixedLength(char *d, int dL, cchar *s, int sL, char pad= ' ');
// [ 2] toLower(char *string);
// [ 2] toLower(char *string, int length);
// [ 2] toLower(bsl::string *string);
// [ 2] toLower(std::string *string);
// [ 2] toLower(std::pmr::string *string);
// [ 2] toUpper(char *string);
// [ 2] toUpper(char *string, int length);
// [ 2] toUpper(bsl::string *string);
// [ 2] toUpper(std::string *string);
// [ 2] toUpper(std::pmr::string *string);
// [ 5] trim(char *string);
// [ 5] trim(char *string, int *length);
// [ 5] trim(bsl::string *string);
// [ 5] trim(std::string *string);
// [ 5] trim(std::pmr::string *string);
// [10] skipLeadingTrailing(cchar **begin, cchar **end);
// [ 4] upperCaseCmp(cchar *lhs, cchar *rhs);
// [ 4] upperCaseCmp(cchar *lhs, cchar *rhs, int rhsL);
// [ 4] upperCaseCmp(cchar *lhs, cBslStr& rhs);
// [ 4] upperCaseCmp(cchar *lhs, int lhsL, cchar *rhs);
// [ 4] upperCaseCmp(cchar *lhs, int lhsL, cchar *rhs, int rhsL);
// [ 4] upperCaseCmp(cchar *lhs, int lhsL, cBslStr& rhs);
// [ 4] upperCaseCmp(cBslStr& lhs, cchar *rhs);
// [ 4] upperCaseCmp(cBslStr& lhs, cchar *rhs, int rhsL);
// [ 4] upperCaseCmp(cBslStr& lhs, cBslStr& rhs);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST

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
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlb::String Util;

// The following two functions enable us to test that functions can take a null
// input string pointer in the cases where the specified string length is 0.
// All tests that iterate over a range of input lengths were extended to also
// include a length of '-1', and 'ns' will return a null pointer in that case,
// and everywhere the string length is used in the test case is passed through
// 'nneg', which will return 0 in the case of '-1 == len'.

static inline
const char *ns(const char *string, int len)
    // Return the specified 'string' if 'len >= 0', otherwise return null.
{
    return len < 0 ? 0 : string;
}

static inline
int nneg(int len)
    // Return the specified 'len' if it is positive and 0 otherwise.
{
    return len < 0 ? 0 : len;
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'copy'
        //
        // Concerns:
        //: 1 That the three 'copy' methods work as specified in the doc.
        //
        // Plan:
        //: 1 Table-based tests are used.
        //
        // Testing:
        //  copy(cchar *string, bslma::Allocator *basicAllocator);
        //  copy(cchar *string, int length, bslma::Allocator *basicAllocator);
        //  copy(cBslStr& string, bslma::Allocator *basicAllocator);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "TESTING 'copy'" "\n"
                                  "==============" "\n";

        bslma::TestAllocator ta;

        {
            static const struct {
                const char *string;
                int         clipLen;
                const char *clippedResult;
            } DATA[] = {
                { "",        -1, "" },
                { "",         0, "" },
                { "arf arf",  7, "arf arf" },
                { "arf arf",  0, "" },
                { "arf arf",  5, "arf a" },
                { "arf\0arf", 7, "arf\0arf" },
            };
            enum { DATA_LEN = sizeof(DATA) / sizeof(*DATA) };

            for (int i = 0; i < DATA_LEN; ++i) {
                const char *STRING         = DATA[i].string;
                const int   CLIP_LEN       = DATA[i].clipLen;
                const char *CLIPPED_RESULT = DATA[i].clippedResult;

                char *result = bdlb::String::copy(STRING, &ta);

                ASSERT(result != STRING);
                ASSERT(!bsl::strcmp(result, STRING));

                ta.deallocate(result);

                result = bdlb::String::copy(ns(STRING, CLIP_LEN),
                                            nneg(CLIP_LEN),
                                            &ta);

                ASSERT(result != STRING);
                ASSERT(!bsl::strcmp(result, CLIPPED_RESULT));
                int minLength = bsl::min(static_cast<int>(nneg(CLIP_LEN)),
                                         static_cast<int>(
                                                         bsl::strlen(STRING)));
                ASSERT(minLength == static_cast<int>(bsl::strlen(result)));
                ASSERT(0 == bsl::memcmp(result, STRING, nneg(CLIP_LEN)));
                ASSERT(0 == result[nneg(CLIP_LEN)]);

                ta.deallocate(result);
            }
        }

        {
            static const struct {
                const char *string;
                int         len;
            } DATA[] = {
                { "", 0 },
                { "arf arf", 7 },
                { "arf\0arf", 7 },
            };
            enum { DATA_LEN = sizeof(DATA) / sizeof(*DATA) };

            for (int i = 0; i < DATA_LEN; ++i) {
                const char *STRING = DATA[i].string;
                const int   LEN    = DATA[i].len;

                bsl::string s;
                s.resize(LEN);
                for (int j = 0; j < LEN; ++j) {
                    s[j] = STRING[j];
                }
                const bsl::string& S = s;

                char *result = bdlb::String::copy(S, &ta);

                ASSERT(0 == bsl::memcmp(result, STRING, LEN));
                ASSERT(!bsl::strcmp(result, STRING));
                ASSERT(0 == result[LEN]);

                ta.deallocate(result);
            }
        }
      }  break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'skipLeadingTrailing'
        //
        // Concerns:
        //: 1 Correctly handles leading and/or trailing spaces.
        //:
        //: 2 Correctly adjust only the 'end' pointer when the entire string is
        //:   whitespace.
        //:
        //: 3 '*begin' is adjusted at most to the '*end - 1'th position.
        //:
        //: 4 Nothing happens when '*begin == *end'.
        //
        // Plan:
        //: 1 Pass the end points of a variety of strings covering all 4
        //:   concerns.  In each case, observe where the end points wind up and
        //:   confirm they are as expected.
        //
        // Testing:
        //  skipLeadingTrailing(cchar **begin, cchar **end);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "TESTING 'skipLeadingTrailing'" "\n"
                                  "=============================" "\n";

        static const struct {
            const int   d_lineNumber;  // line number
            const char *d_string;      // test string
            const int   d_length;      // string length
            const int   d_expBegin;    // expected begin
            const int   d_expEnd;      // expected end
        } DATA[] = {
            // LINE  STRING    LENGTH  EXPBEGIN  EXPEND
            // ----  ------    ------  --------  ------

            // empty string
            { L_,   "",          0,         0,      0},

            // no white spaces
            { L_,   "a",         1,         0,      1},
            { L_,   "ab",        2,         0,      2},
            { L_,   "abc",       3,         0,      3},

            // white space in between
            { L_,   "a b",       3,         0,      3},
            { L_,   "a  b",      4,         0,      4},
            { L_,   "a   b",     5,         0,      5},

            // leading white space only
            { L_,   " a",        2,         1,      2},
            { L_,   "  a",       3,         2,      3},
            { L_,   "   a",      4,         3,      4},
            { L_,   " ab",       3,         1,      3},
            { L_,   "  ab",      4,         2,      4},
            { L_,   "   ab",     5,         3,      5},

            // trailing white space only
            { L_,   "a ",        2,         0,      1},
            { L_,   "a  ",       3,         0,      1},
            { L_,   "a   ",      4,         0,      1},
            { L_,   "ab ",       3,         0,      2},
            { L_,   "ab  ",      4,         0,      2},
            { L_,   "ab   ",     5,         0,      2},

            // leading and trailing white spaces
            { L_,   " a ",       3,         1,      2},
            { L_,   "  a ",      4,         2,      3},
            { L_,   "   a ",     5,         3,      4},
            { L_,   " ab ",      4,         1,      3},
            { L_,   "  ab ",     5,         2,      4},
            { L_,   "   ab ",    6,         3,      5},
            { L_,   " a  ",      4,         1,      2},
            { L_,   "  a  ",     5,         2,      3},
            { L_,   "   a  ",    6,         3,      4},
            { L_,   " ab  ",     5,         1,      3},
            { L_,   "  ab  ",    6,         2,      4},
            { L_,   "   ab  ",   7,         3,      5},
            { L_,   " a   ",     5,         1,      2},
            { L_,   "  a   ",    6,         2,      3},
            { L_,   "   a   ",   7,         3,      4},
            { L_,   " ab   ",    6,         1,      3},
            { L_,   "  ab   ",   7,         2,      4},
            { L_,   "   ab   ",  8,         3,      5},

            // all white spaces
            { L_,   " ",         1,         0,      0},
            { L_,   "  ",        2,         0,      0},
            { L_,   "   ",       3,         0,      0},

            // discontinued white spaces
            { L_,   "a  b  e",   7,         0,      7},
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE     = DATA[ti].d_lineNumber;
            const char *STRING   = DATA[ti].d_string;
            const int   LENGTH   = DATA[ti].d_length;
            const int   EXPBEGIN = DATA[ti].d_expBegin;
            const int   EXPEND   = DATA[ti].d_expEnd;

            if (veryVerbose) {
                P_(LINE) P_(STRING) P_(LENGTH) P_(EXPBEGIN) P(EXPEND)
            }

            const char *begin = STRING;
            const char *end   = STRING + LENGTH;

            bdlb::String::skipLeadingTrailing(&begin, &end);

            const bsl::ptrdiff_t RESBEGIN = begin - STRING;
            const bsl::ptrdiff_t RESEND   = end   - STRING;

            ASSERTV(RESBEGIN, EXPBEGIN, STRING + EXPBEGIN == begin);
            ASSERTV(RESEND,   EXPEND,   STRING + EXPEND   == end);
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'strstr', 'strstrCaseless', 'strrstr', 'strrstrCaseless'
        //
        // Concerns:
        //: 1 Forward-search correctly returns the position of the first found
        //:   substring or '0' if not found.
        //:
        //: 2 Forward-search correctly returns the starting position of the
        //:   string when an empty substring is passed in.
        //:
        //: 3 Reverse-search correctly returns the position of the last found
        //:   substring or '0' if not found.
        //:
        //: 4 Reverse-search correctly returns the starting position of the
        //:   string when an empty substring is passed in.
        //
        // Plan:
        //: 1 This case use the Ad-Hoc Data Selection Method for contents of
        //:   the original string and Depth-First Enumeration substring values.
        //:
        //: 2 To address concerns 1, 2, 3, and 4 we enumerate through different
        //:   lengths of substrings (0 to 2) and match them against different
        //:   lengths of original strings (0 to substring length + 2).  The
        //:   case of both the substrings and the original strings are also
        //:   permuted.  The resulting address returned from 'strstr',
        //:   'strstrCaseless', 'strrstr', and 'strrstrCaseless' is compared
        //:   against the expected offset from the original string to make sure
        //:   concerns 1, 2, 3, and 4 are addressed.
        //
        // Testing:
        //  strstr(cchar *str, int strL, cchar *subStr, int subStrL);
        //  strstrCaseless(cchar *str, int strL, cchar *subStr, int subStrL);
        //  strrstr(cchar *str, int strL, cchar *subStr, int subStrL);
        //  strrstrCaseless(cchar *str, int strL, cchar *subStr, int subStrL);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n"
       //-------------------------^
       "TESTING 'strstr', 'strstrCaseless', 'strrstr', 'strrstrCaseless'" "\n"
       "================================================================" "\n";
       //-------------------------v

        static const struct {
            const int   d_lineNumber;
            const char *d_string;
            const int   d_stringLen;
            const char *d_subString;
            const int   d_subStringLen;
            const int   d_result;         // Result will be the position of the
                                          // substring relative to the starting
                                          // position of the string.  Returns
                                          // -1 if not found.
            const int   d_resultCaseless; // Result will be the position of the
                                          // substring relative to the starting
                                          // position of the string.  Returns
                                          // -1 if not found.
            const int   d_resultReverse;  // Result will be the position of the
                                          // substring relative to the starting
                                          // position of the string.  Returns
                                          // -1 if not found.
            const int   d_resultReverseCaseless; // Result will be the position
                                                 // of the substring relative
                                                 // to the starting position of
                                                 // the string.  Returns -1 if
                                                 // not found.

        } DATA[] = {
            //In the table below, R1-4 correspond to the results from the
            //'strstr', 'strstrCaseless', 'strrstr', and 'strrstrCaseless'.
            //
            //LINE, STR,     STRLEN,     SUBSTR,  SUBSTRLEN,  R1, R2, R3, R4

            //Substring length 0, caseless
            { L_,   "",     -1,          0,       0,          0,  0,  0,  0},
            { L_,   "",      0,          0,       0,          0,  0,  0,  0},
            { L_,   "a",     1,          0,       0,          0,  0,  1,  1},
            { L_,   "ab",    2,          0,       0,          0,  0,  2,  2},
            { L_,   "abc",   3,          0,       0,          0,  0,  3,  3},

            //Substring length 0, case sensitive
            { L_,   "A",     1,         "",      -1,          0,  0,  1,  1},
            { L_,   "Ab",    2,         "",      -1,          0,  0,  2,  2},
            { L_,   "aB",    2,         "",      -1,          0,  0,  2,  2},
            { L_,   "AB",    2,         "",      -1,          0,  0,  2,  2},
            { L_,   "Abc",   3,         "",      -1,          0,  0,  3,  3},
            { L_,   "aBc",   3,         "",      -1,          0,  0,  3,  3},
            { L_,   "abC",   3,         "",      -1,          0,  0,  3,  3},
            { L_,   "ABc",   3,         "",      -1,          0,  0,  3,  3},
            { L_,   "aBC",   3,         "",      -1,          0,  0,  3,  3},
            { L_,   "AbC",   3,         "",      -1,          0,  0,  3,  3},
            { L_,   "ABC",   3,         "",      -1,          0,  0,  3,  3},

            { L_,   "A",     1,         "",       0,          0,  0,  1,  1},
            { L_,   "Ab",    2,         "",       0,          0,  0,  2,  2},
            { L_,   "aB",    2,         "",       0,          0,  0,  2,  2},
            { L_,   "AB",    2,         "",       0,          0,  0,  2,  2},
            { L_,   "Abc",   3,         "",       0,          0,  0,  3,  3},
            { L_,   "aBc",   3,         "",       0,          0,  0,  3,  3},
            { L_,   "abC",   3,         "",       0,          0,  0,  3,  3},
            { L_,   "ABc",   3,         "",       0,          0,  0,  3,  3},
            { L_,   "aBC",   3,         "",       0,          0,  0,  3,  3},
            { L_,   "AbC",   3,         "",       0,          0,  0,  3,  3},
            { L_,   "ABC",   3,         "",       0,          0,  0,  3,  3},

            //Substring length 1, caseless
            { L_,   "",      0,         "a",      1,         -1, -1, -1, -1},
            { L_,   " ",     1,         "a",      1,         -1, -1, -1, -1},
            { L_,   "a",     1,         "a",      1,          0,  0,  0,  0},
            { L_,   "b",     1,         "a",      1,         -1, -1, -1, -1},
            { L_,   "  ",    2,         "a",      1,         -1, -1, -1, -1},
            { L_,   "aa",    2,         "a",      1,          0,  0,  1,  1},
            { L_,   "ab",    2,         "a",      1,          0,  0,  0,  0},
            { L_,   "ba",    2,         "a",      1,          1,  1,  1,  1},
            { L_,   "bb",    2,         "a",      1,         -1, -1, -1, -1},
            { L_,   "   ",   3,         "a",      1,         -1, -1, -1, -1},
            { L_,   "aaa",   3,         "a",      1,          0,  0,  2,  2},
            { L_,   "aab",   3,         "a",      1,          0,  0,  1,  1},
            { L_,   "abb",   3,         "a",      1,          0,  0,  0,  0},
            { L_,   "aba",   3,         "a",      1,          0,  0,  2,  2},
            { L_,   "baa",   3,         "a",      1,          1,  1,  2,  2},
            { L_,   "bba",   3,         "a",      1,          2,  2,  2,  2},
            { L_,   "bbb",   3,         "a",      1,         -1, -1, -1, -1},

            //String embedded nulls
            { L_,   "\0  ",  3,         "a",      1,         -1, -1, -1, -1},
            { L_,   "\0aa",  3,         "a",      1,          1,  1,  2,  2},
            { L_,   "\0ab",  3,         "a",      1,          1,  1,  1,  1},
            { L_,   "\0ba",  3,         "a",      1,          2,  2,  2,  2},
            { L_,   "\0bb",  3,         "a",      1,         -1, -1, -1, -1},
            { L_,   "  \0",  3,         "a",      1,         -1, -1, -1, -1},
            { L_,   "aa\0",  3,         "a",      1,          0,  0,  1,  1},
            { L_,   "ab\0",  3,         "a",      1,          0,  0,  0,  0},
            { L_,   "ba\0",  3,         "a",      1,          1,  1,  1,  1},
            { L_,   "bb\0",  3,         "a",      1,         -1, -1, -1, -1},

            //Substring length 1, case sensitive
            { L_,   "A",     1,         "a",      1,         -1,  0, -1,  0},
            { L_,   "Aa",    2,         "a",      1,          1,  0,  1,  1},
            { L_,   "aA",    2,         "a",      1,          0,  0,  0,  1},
            { L_,   "AA",    2,         "a",      1,         -1,  0, -1,  1},
            { L_,   "bA",    2,         "a",      1,         -1,  1, -1,  1},
            { L_,   "Aaa",   3,         "a",      1,          1,  0,  2,  2},
            { L_,   "aAa",   3,         "a",      1,          0,  0,  2,  2},
            { L_,   "aaA",   3,         "a",      1,          0,  0,  1,  2},
            { L_,   "AAa",   3,         "a",      1,          2,  0,  2,  2},
            { L_,   "aAA",   3,         "a",      1,          0,  0,  0,  2},
            { L_,   "AaA",   3,         "a",      1,          1,  0,  1,  2},
            { L_,   "AAA",   3,         "a",      1,         -1,  0, -1,  2},
            { L_,   "Aab",   3,         "a",      1,          1,  0,  1,  1},
            { L_,   "aAb",   3,         "a",      1,          0,  0,  0,  1},
            { L_,   "AAb",   3,         "a",      1,         -1,  0, -1,  1},
            { L_,   "Aba",   3,         "a",      1,          2,  0,  2,  2},
            { L_,   "abA",   3,         "a",      1,          0,  0,  0,  2},
            { L_,   "AbA",   3,         "a",      1,         -1,  0, -1,  2},
            { L_,   "bAa",   3,         "a",      1,          2,  1,  2,  2},
            { L_,   "baA",   3,         "a",      1,          1,  1,  1,  2},
            { L_,   "bAA",   3,         "a",      1,         -1,  1, -1,  2},
            { L_,   "bbA",   3,         "a",      1,         -1,  2, -1,  2},
            { L_,   "A",     1,         "A",      1,          0,  0,  0,  0},
            { L_,   "Aa",    2,         "A",      1,          0,  0,  0,  1},
            { L_,   "aA",    2,         "A",      1,          1,  0,  1,  1},
            { L_,   "AA",    2,         "A",      1,          0,  0,  1,  1},
            { L_,   "bA",    2,         "A",      1,          1,  1,  1,  1},
            { L_,   "Aaa",   3,         "A",      1,          0,  0,  0,  2},
            { L_,   "aAa",   3,         "A",      1,          1,  0,  1,  2},
            { L_,   "aaA",   3,         "A",      1,          2,  0,  2,  2},
            { L_,   "AAa",   3,         "A",      1,          0,  0,  1,  2},
            { L_,   "aAA",   3,         "A",      1,          1,  0,  2,  2},
            { L_,   "AaA",   3,         "A",      1,          0,  0,  2,  2},
            { L_,   "AAA",   3,         "A",      1,          0,  0,  2,  2},
            { L_,   "Aab",   3,         "A",      1,          0,  0,  0,  1},
            { L_,   "aAb",   3,         "A",      1,          1,  0,  1,  1},
            { L_,   "AAb",   3,         "A",      1,          0,  0,  1,  1},
            { L_,   "Aba",   3,         "A",      1,          0,  0,  0,  2},
            { L_,   "abA",   3,         "A",      1,          2,  0,  2,  2},
            { L_,   "AbA",   3,         "A",      1,          0,  0,  2,  2},
            { L_,   "bAa",   3,         "A",      1,          1,  1,  1,  2},
            { L_,   "baA",   3,         "A",      1,          2,  1,  2,  2},
            { L_,   "bAA",   3,         "A",      1,          1,  1,  2,  2},
            { L_,   "bbA",   3,         "A",      1,          2,  2,  2,  2},

            //Substring length 2, caseless
            { L_,   "",      0,         "ab",     2,         -1, -1, -1, -1},
            { L_,   " ",     1,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "a",     1,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "b",     1,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "  ",    2,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "aa",    2,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "ab",    2,         "ab",     2,          0,  0,  0,  0},
            { L_,   "ba",    2,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "bb",    2,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "   ",   3,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "aaa",   3,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "aab",   3,         "ab",     2,          1,  1,  1,  1},
            { L_,   "aba",   3,         "ab",     2,          0,  0,  0,  0},
            { L_,   "baa",   3,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "bba",   3,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "bbb",   3,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "    ",  4,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "aaab",  4,         "ab",     2,          2,  2,  2,  2},
            { L_,   "aaba",  4,         "ab",     2,          1,  1,  1,  1},
            { L_,   "abaa",  4,         "ab",     2,          0,  0,  0,  0},
            { L_,   "abab",  4,         "ab",     2,          0,  0,  2,  2},

            //Substring length 2, case sensitive
            { L_,   "A",     1,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "Aa",    2,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "aA",    2,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "AA",    2,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "bA",    2,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "Aaa",   3,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "aAa",   3,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "aaA",   3,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "AAa",   3,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "aAA",   3,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "AaA",   3,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "AAA",   3,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "Aab",   3,         "ab",     2,          1,  1,  1,  1},
            { L_,   "aAb",   3,         "ab",     2,         -1,  1, -1,  1},
            { L_,   "AAb",   3,         "ab",     2,         -1,  1, -1,  1},
            { L_,   "Aba",   3,         "ab",     2,         -1,  0, -1,  0},
            { L_,   "abA",   3,         "ab",     2,          0,  0,  0,  0},
            { L_,   "AbA",   3,         "ab",     2,         -1,  0, -1,  0},
            { L_,   "bAa",   3,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "baA",   3,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "bAA",   3,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "bbA",   3,         "ab",     2,         -1, -1, -1, -1},
            { L_,   "Aaab",  4,         "ab",     2,          2,  2,  2,  2},
            { L_,   "aAab",  4,         "ab",     2,          2,  2,  2,  2},
            { L_,   "aaAb",  4,         "ab",     2,         -1,  2, -1,  2},
            { L_,   "Aaba",  4,         "ab",     2,          1,  1,  1,  1},
            { L_,   "aAba",  4,         "ab",     2,         -1,  1, -1,  1},
            { L_,   "aabA",  4,         "ab",     2,          1,  1,  1,  1},
            { L_,   "Abab",  4,         "ab",     2,          2,  0,  2,  2},
            { L_,   "abAb",  4,         "ab",     2,          0,  0,  0,  2},
            { L_,   "AbAb",  4,         "ab",     2,         -1,  0, -1,  2},
            { L_,   "A",     1,         "Ab",     2,         -1, -1, -1, -1},
            { L_,   "Aa",    2,         "Ab",     2,         -1, -1, -1, -1},
            { L_,   "aA",    2,         "Ab",     2,         -1, -1, -1, -1},
            { L_,   "AA",    2,         "Ab",     2,         -1, -1, -1, -1},
            { L_,   "bA",    2,         "Ab",     2,         -1, -1, -1, -1},
            { L_,   "Ab",    2,         "Ab",     2,          0,  0,  0,  0},
            { L_,   "Aaa",   3,         "Ab",     2,         -1, -1, -1, -1},
            { L_,   "aAa",   3,         "Ab",     2,         -1, -1, -1, -1},
            { L_,   "aaA",   3,         "Ab",     2,         -1, -1, -1, -1},
            { L_,   "AAa",   3,         "Ab",     2,         -1, -1, -1, -1},
            { L_,   "aAA",   3,         "Ab",     2,         -1, -1, -1, -1},
            { L_,   "AaA",   3,         "Ab",     2,         -1, -1, -1, -1},
            { L_,   "AAA",   3,         "Ab",     2,         -1, -1, -1, -1},
            { L_,   "Aab",   3,         "Ab",     2,         -1,  1, -1,  1},
            { L_,   "aAb",   3,         "Ab",     2,          1,  1,  1,  1},
            { L_,   "AAb",   3,         "Ab",     2,          1,  1,  1,  1},
            { L_,   "Aba",   3,         "Ab",     2,          0,  0,  0,  0},
            { L_,   "abA",   3,         "Ab",     2,         -1,  0, -1,  0},
            { L_,   "AbA",   3,         "Ab",     2,          0,  0,  0,  0},
            { L_,   "bAa",   3,         "Ab",     2,         -1, -1, -1, -1},
            { L_,   "baA",   3,         "Ab",     2,         -1, -1, -1, -1},
            { L_,   "bAA",   3,         "Ab",     2,         -1, -1, -1, -1},
            { L_,   "bbA",   3,         "Ab",     2,         -1, -1, -1, -1},
            { L_,   "Aaab",  4,         "Ab",     2,         -1,  2, -1,  2},
            { L_,   "aAab",  4,         "Ab",     2,         -1,  2, -1,  2},
            { L_,   "aaAb",  4,         "Ab",     2,          2,  2,  2,  2},
            { L_,   "Aaba",  4,         "Ab",     2,         -1,  1, -1,  1},
            { L_,   "aAba",  4,         "Ab",     2,          1,  1,  1,  1},
            { L_,   "aabA",  4,         "Ab",     2,         -1,  1, -1,  1},
            { L_,   "Abab",  4,         "Ab",     2,          0,  0,  0,  2},
            { L_,   "abAb",  4,         "Ab",     2,          2,  0,  2,  2},
            { L_,   "AbAb",  4,         "Ab",     2,          0,  0,  2,  2},
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            if (veryVerbose) { T_ P(i) }
            const char *STRING         = DATA[i].d_string;
            const int   STRINGLEN      = DATA[i].d_stringLen;
            const char *SUBSTRING      = DATA[i].d_subString;
            const int   SUBSTRINGLEN   = DATA[i].d_subStringLen;
            const int   RESULT         = DATA[i].d_result;
            const int   RESULTCASELESS = DATA[i].d_resultCaseless;
            const int   RESULTREVERSE  = DATA[i].d_resultReverse;
            const int   RESULTREVERSECASELESS
                                       = DATA[i].d_resultReverseCaseless;

            if (veryVeryVerbose) {
                if (STRING) {
                    T_ T_ P(STRING)
                }
                T_ T_ P(STRINGLEN)
                if (SUBSTRING) {
                    T_ T_ P(SUBSTRING)
                }
                T_ T_ P(SUBSTRINGLEN)
                T_ T_ P(RESULT)
                T_ T_ P(RESULTCASELESS)
                T_ T_ P(RESULTREVERSE)
                T_ T_ P(RESULTREVERSECASELESS)
            }

            // Run 'strstr', 'strstrCaseless', 'strrstr', and
            // 'strrstrCaseless'.
            const char* strstrResult = Util::strstr(ns(STRING, STRINGLEN),
                                                    nneg(STRINGLEN),
                                                    ns(SUBSTRING,
                                                       SUBSTRINGLEN),
                                                    nneg(SUBSTRINGLEN));

            const char* strstrCaselessResult = Util::strstrCaseless(
                                                   ns(STRING, STRINGLEN),
                                                   nneg(STRINGLEN),
                                                   ns(SUBSTRING, SUBSTRINGLEN),
                                                   nneg(SUBSTRINGLEN));

            const char* strrstrResult = Util::strrstr(
                                                   ns(STRING, STRINGLEN),
                                                   nneg(STRINGLEN),
                                                   ns(SUBSTRING, SUBSTRINGLEN),
                                                   nneg(SUBSTRINGLEN));

            const char* strrstrCaselessResult = Util::strrstrCaseless(
                                                   ns(STRING, STRINGLEN),
                                                   nneg(STRINGLEN),
                                                   ns(SUBSTRING, SUBSTRINGLEN),
                                                   nneg(SUBSTRINGLEN));

            if (-1 == RESULT) {
                ASSERT(0 == strstrResult);
            }
            else {
                if (veryVeryVerbose) {
                    T_ T_ P(RESULT)
                    T_ T_ bsl::cout << "STRING + RESULT: "
                                    << bsl::hex
                                    << static_cast<const void *>(
                                                               STRING + RESULT)
                                    << bsl::dec << bsl::endl;
                    T_ T_ bsl::cout << "STRSTRRESULT: "
                                    << bsl::hex
                                    << static_cast<const void *>(strstrResult)
                                    << bsl::dec << bsl::endl;
                }
                ASSERT(strstrResult == (-1 == STRINGLEN ? 0
                                                        : STRING + RESULT));
            }

            if (-1 == RESULTCASELESS) {
                ASSERT (0 == strstrCaselessResult);
            }
            else {
                if (veryVeryVerbose) {
                    T_ T_ P(RESULTCASELESS)
                    T_ T_ bsl::cout << "STRING + RESULTCASELESS: "
                                    << bsl::hex
                                    << static_cast<const void *>(
                                                       STRING + RESULTCASELESS)
                                    << bsl::dec << bsl::endl;
                    T_ T_ bsl::cout << "STRSTRCASELESSRESULT: "
                                    << bsl::hex
                                    << static_cast<const void *>(
                                                          strstrCaselessResult)
                                    << bsl::dec << bsl::endl;
                }
                ASSERT(strstrCaselessResult == (-1 != STRINGLEN
                                               ? STRING + RESULTCASELESS
                                               : 0));
            }

            if (-1 == RESULTREVERSE) {
                ASSERT (0 == strrstrResult);
            }
            else {
                if (veryVeryVerbose) {
                    T_ T_ P(RESULT)
                    T_ T_ bsl::cout << "STRING + RESULTREVERSE: "
                                    << bsl::hex
                                    << static_cast<const void *>(
                                                        STRING + RESULTREVERSE)
                                    << bsl::dec << bsl::endl;
                    T_ T_ bsl::cout << "STRRSTRRESULT: "
                                    << bsl::hex
                                    << static_cast<const void *>(
                                                                 strrstrResult)
                                    << bsl::dec << bsl::endl;
                }
                ASSERT(strrstrResult == (-1 != STRINGLEN
                                        ? STRING + RESULTREVERSE
                                        : 0));
            }

            if (-1 == RESULTREVERSECASELESS) {
                ASSERT (0 == strrstrCaselessResult);
            }
            else {
                if (veryVeryVerbose) {
                    T_ T_ P(RESULTREVERSECASELESS)
                    T_ T_ bsl::cout << "STRING + RESULTREVERSECASELESS: "
                                    << bsl::hex
                                    << static_cast<const void *>(
                                                STRING + RESULTREVERSECASELESS)
                                    << bsl::dec << bsl::endl;
                    T_ T_ bsl::cout << "STRRSTRCASELESSRESULT: "
                                    << bsl::hex
                                    << static_cast<const void *>(
                                                         strrstrCaselessResult)
                                    << bsl::dec << bsl::endl;
                }
                ASSERT (strrstrCaselessResult == (-1 != STRINGLEN
                                                 ? STRING +
                                                        RESULTREVERSECASELESS
                                                 : 0));
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'toFixedLength'
        //
        // Concerns:
        //: 1 The correct content from the source string is copied into the
        //:   destination string.  The number of characters copied is at most
        //:   'dstLength' (i.e., the length of the destination string).
        //:
        //: 2 If the length of the source string ('srcLength') is less than
        //:   'dstLength', the destination string is padded with the specified
        //:   character to the length 'dstLength'.
        //
        // Plan:
        //: 1 This case uses the Ad-Hoc Data Selection Method and the
        //:   Array-Based Implementation Technique.
        //:
        //: 2 To address concerns 1 and 2, we arbitrarily select a set of
        //:   strings as source strings.  We generate a string S for each of
        //:   these source strings for verification purpose.  The length of S
        //:   is always 'dstLength'.  If 'srcLength < dstLength', S contains
        //:   the entire source string plus the correct padding at the end.
        //:   Otherwise, S contains the first 'dstLength' characters of the
        //:   source string.  Then we compare S with the destination string
        //:   after 'toFixedLength' is invoked and make sure they are equal.
        //
        // Testing:
        //  toFixedLength(char *d, int dL, cchar *s, int sL, char pad= ' ');
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "TESTING 'toFixedLength'" "\n"
                                  "=======================" "\n";

        static const char *STRINGS[] = {
            // strings of length 0 or 1
            "", " ", "\n", "\t", "*", "?", "a", "0",

            // strings of length 2
            "  ", " \t", "\n ",
            "**", "*a", "a*",
            "00", "a0", "0a",
            "a ", " a", "aa",

            // strings of length 3
            "   ", "\t\n\t",
            "***", "a**", "*a*", "**a", "aa*", "a*a", "*aa",
            "000", "a00", "0a0", "00a", "aa0", "a0a", "0aa",
            "aaa", "a  ", " a ", "  a", "aa ", "a a", " aa",

            // strings of length 4 and above
            "    ", "abcd", " Test string ",
            "1234567890123456789012345678901234567890123456789",
        0};

        static const char CHARS[] = {
            ' ', '\0', '\t', '\n', '.', '0', 'Z', 'x'  // *must* contain 'x'
        };
        static const int  NUM_CHARS = sizeof CHARS / sizeof *CHARS;

        const int BUFSIZE = 32;  // size of destination buffer

        char atBuf[BUFSIZE];  // filled with '@'
        bsl::memset(atBuf, '@', BUFSIZE);

        char dstBuf[BUFSIZE];

        for (int strIdx = 0; STRINGS[strIdx]; ++strIdx) {
            const char *SRC = STRINGS[strIdx];
            const int   LEN = static_cast<int>(bsl::strlen(SRC));

            for (int charIdx = 0; charIdx < NUM_CHARS; ++charIdx) {
                const char PADCHAR = CHARS[charIdx];

                // 'x' is a sentinel in 'CHARS' indicating that we want to
                // verify the default value for the 'padChar' parameter.

                const char CHKCHAR = 'x' != PADCHAR ? PADCHAR : ' ';

                if (veryVerbose) { T_ P_(charIdx) P(PADCHAR) }

                // Call 'toFixedLength' with various lengths for both source
                // and destination string.

                for (int dstLen = 0; dstLen < BUFSIZE; ++dstLen) {

                    if (veryVerbose) { T_ T_ P(dstLen) }

                    for (int srcLen = -1; srcLen < LEN; ++srcLen) {

                       if (veryVerbose) { T_ T_ T_ P(srcLen) }

                        bsl::memset(dstBuf, '@', BUFSIZE);

                        if ('x' != PADCHAR) {
                            Util::toFixedLength(dstBuf,
                                                dstLen,
                                                ns(SRC, srcLen),
                                                nneg(srcLen),
                                                PADCHAR);
                        }
                        else {
                            Util::toFixedLength(dstBuf,
                                                dstLen,
                                                ns(SRC, srcLen),
                                                nneg(srcLen));
                                                           // default 'padChar'
                        }

                        // Generate the expected result string.

                        bsl::string result(SRC,
                                           0,
                                           bsl::min(nneg(srcLen), dstLen));

                        if (nneg(srcLen) < dstLen) {
                            result.append(dstLen - nneg(srcLen), CHKCHAR);
                        }

                        // Verify the result from 'toFixedLength'.

                        ASSERTV(strIdx, charIdx, dstLen, srcLen,
                             0 == bsl::memcmp(result.c_str(), dstBuf, dstLen));

                        // Verify no overwrite of 'dstBuf'.

                        if (dstLen < BUFSIZE) {
                             ASSERTV(strIdx, charIdx, dstLen, srcLen,
                                           0 == bsl::memcmp(&dstBuf[dstLen],
                                                            &atBuf[dstLen],
                                                            BUFSIZE - dstLen));
                        }
                    }
                }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'strnlen'
        //
        // Concerns:
        //: 1 We want to verify that 'strnlen' returns the minimum of the
        //:   length of the specified string and the specified maximum length.
        //
        // Plan:
        //: 1 This case uses the Ad-Hoc Data Selection Method and the
        //:   Array-Based Implementation Technique.
        //:
        //: 2 We first select an arbitrary set of strings.  For each of these
        //:   strings, we vary the maximum length from 0 to two times the
        //:   actually string length.  We then use the string and maximum
        //:   length as inputs to 'strnlen' and verify its return value equals
        //:   the minimum of result of 'strlen' and the maximum length.
        //
        // Testing:
        //  strnlen(cchar *str, int maximumLength);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "TESTING 'strnlen'" "\n"
                                  "=================" "\n";

        static const char *STRINGS[] = {
            // strings of length 0 and 1
            "", " ", "\t", "\n", "!", "0", "a",

            // strings of length 2
            "  ", "\t\n", "!!", "10", "a ", " b", "ab",

            // strings of length 3
            "   ", "\n\t\t", "!!!", "210",
            "a  ", " b ", "  c", "ab ", "a c", " bc", "abc",

            // strings of length 4 and above
            "    ", "3210", "Bloomberg LP.",
            "1234567890abcdefghijklmnopqrstuvwxyz~!@#$%^&*()_+-=`{}|[]\\\t\n ",
        0};

        for (int i = 0; STRINGS[i]; ++i) {
            const int LEN = static_cast<int>(bsl::strlen(STRINGS[i]));

            if (veryVerbose) { T_ P_(i) P_(STRINGS[i]) P(LEN) }

            for (int l = 0; l < 2 * LEN; ++l) {

                if (veryVerbose) { T_ T_ P(l) }

                ASSERTV(i, l,
                        bsl::min(l, LEN) == Util::strnlen(STRINGS[i], l));
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'pad'
        //
        // Concerns:
        //: 1 The 'pad' method properly pads the input string with the
        //:   character provided to the specified number if the number is
        //:   greater than the original length of the string.
        //:
        //: 2 The 'pad' method will no alter the input string if the length
        //:   specified is less than or equal to the length of the input
        //:   string.
        //
        // Plan:
        //: 1 This case uses the Ad-Hoc Data Selection Method and the
        //:   Table-Driven Implementation Technique.
        //:
        //: 2 To address concern 1 and 2, we call 'pad' with an arbitrarily
        //:   selected set of data.  Then we verify that if the length
        //:   specified does not exceed the string's original length, the
        //:   string is unmodified.  Otherwise, the string is padded with the
        //:   supplied character up to the specified length.
        //
        // Testing:
        //  pad(bsl::string *str, int numChars, char padChar = ' ');
        //  pad(std::string *str, int numChars, char padChar = ' ');
        //  pad(std::pmr::string *str, size_type numChars, char padChar = ' ');
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "TESTING 'pad'" "\n"
                                  "=============" "\n";

        static const char *STRINGS[] = {
            // strings of length 0 or 1
            "", " ", "\t", "p", "?", "1",

            // strings of length 2
            "pp", "  ", "??", "11", " p", "p ",

            // strings of length 3
            "ppp", "   ", "???", "111", "  p", " p ", "p  ",

            // strings of length 4 and above
            "Test string", " Bloomberg LP. ",
        0};

        static const char CHARS[] = {
            ' ', '\0', '\t', '\n', '.', '0', 'Z', 'x'  // *must* contain 'x'
        };
        static const int  NUM_CHARS = sizeof CHARS / sizeof *CHARS;

        for (int strIdx = 0; STRINGS[strIdx]; ++strIdx) {
            const char *STRING = STRINGS[strIdx];
            const int   LEN    = static_cast<int>(bsl::strlen(STRING));

            if (veryVerbose) { T_ P_(strIdx) P_(STRING) P(LEN) }

            for (int charIdx = 0; charIdx < NUM_CHARS; ++charIdx) {
                const char PADCHAR = CHARS[charIdx];

                // 'x' is a sentinel in 'CHARS' indicating that we want to
                // verify the default value for the 'padChar' parameter.

                const char CHKCHAR = 'x' != PADCHAR ? PADCHAR : ' ';

                // Pad the string to various lengths and verify the result.

                for (int length = 0; length < 2 * LEN; ++length) {

                 if (veryVerbose) { T_ T_ P(length) }
                    bsl::string bslString(STRING);
                    if ('x' != PADCHAR) {
                        Util::pad(&bslString, length, PADCHAR);
                    }
                    else {
                        Util::pad(&bslString, length);  // default 'padChar'
                    }

                    std::string stdString(STRING);
                    if ('x' != PADCHAR) {
                        Util::pad(&stdString, length, PADCHAR);
                    }
                    else {
                        Util::pad(&stdString, length);  // default 'padChar'
                    }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
                    std::pmr::string pmrString(STRING);
                    if ('x' != PADCHAR) {
                        Util::pad(&pmrString, length, PADCHAR);
                    }
                    else {
                        Util::pad(&pmrString, length);  // default 'padChar'
                    }
#endif

                    if (LEN >= length) {
                        // If the specified 'length' is not greater than the
                        // original string length, then the string should be
                        // unmodified.

                        ASSERTV(strIdx, charIdx, bslString == STRING);
                        ASSERTV(strIdx, charIdx, stdString == STRING);
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
                        ASSERTV(strIdx, charIdx, pmrString == STRING);
#endif
                    }
                    else {
                        // Verify that the strings have been properly padded.

                        ASSERTV(strIdx, charIdx, bslString ==
                                  STRING + bsl::string(length - LEN, CHKCHAR));
                        ASSERTV(strIdx, charIdx, stdString ==
                                  STRING + std::string(length - LEN, CHKCHAR));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
                        ASSERTV(strIdx, charIdx, pmrString ==
                             STRING + std::pmr::string(length - LEN, CHKCHAR));
#endif
                    }
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'ltrim', 'rtrim', 'trim'
        //
        // Concerns:
        //: 1 All of the 'ltrim', 'rtrim', and 'trim' methods properly trim the
        //:   input strings of various types ('bsl::string', null-terminated
        //:   C-style, and a non-null-terminated 'const char      *' with an
        //:   'int' length specifier).
        //:
        //: 2 Specifically the methods that take a non-null-terminated
        //:   'const char *' strings as input will not read pass the end of
        //:   these strings.
        //
        // Plan:
        //: 1 This case uses the Ad-Hoc Data Selection Method and the
        //:   Table-Driven Implementation Technique.
        //:
        //: 2 To address concern 1, we arbitrarily select a set S of strings
        //:   with no whitespace characters as their beginning or ending
        //:   characters.  Then we append whitespace characters before and
        //:   after them and apply 'ltrim', 'rtrim', and 'trim' methods to
        //:   them.  We verify that the result from the 'trim' methods equal
        //:   the original string; that the result from the 'ltrim' methods
        //:   equal the original string with the whitespace appended at the
        //:   end; and that the result from the 'rtrim' methods equal the
        //:   original string with the whitespace prepended at the beginning.
        //:   For each string s in the set S, we generate all 3 types of
        //:   strings for it, then pass them to the corresponding methods and
        //:   verify the results.
        //:
        //: 3 To address concern 2, we create a non-null-terminated string for
        //:   each string s by copying s into a buffer pre-filled with non-zero
        //:   content.  This will cause incorrect compare results if any of
        //:   these 'ltrim', 'rtrim' or 'trim' methods read pass the end of
        //:   these strings.
        //
        // Testing:
        //  ltrim(char *str);
        //  ltrim(char *str, int *L);
        //  ltrim(bsl::string *str);
        //  ltrim(std::string *str);
        //  ltrim(std::pmr::string *str);
        //  rtrim(char *str);
        //  rtrim(bsl::string *str);
        //  rtrim(std::string *str);
        //  rtrim(std::pmr::string *str);
        //  rtrim(cchar *str, int *L);
        //  trim(char *string);
        //  trim(char *string, int *length);
        //  trim(bsl::string *string);
        //  trim(std::string *string);
        //  trim(std::pmr::string *string);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "TESTING 'ltrim', 'rtrim', 'trim'" "\n"
                                  "================================" "\n";

        static const char *STRINGS[]    = {
            "", "t", "test", "test  test",
        0};

        //                        HT    NL    VT      FF      CR    SP
        const char white[]   = { '\t', '\n', '\013', '\014', '\r', ' ' };
        const int  NUM_WHITE = sizeof white / sizeof *white;

        if (verbose) cout << "\tTesting 'char *', 'bsl::string', 'std::string'"
                          << endl;

        for (int i = 0; STRINGS[i]; ++i) {

            if (veryVerbose) { T_ P(i) }

            char cstring[64];
            char nonNullString[sizeof(cstring)];

            for (int padBefore = 0; padBefore < 5; ++padBefore) {

                if (veryVerbose) { T_ T_ P(padBefore) }

                for (int padAfter = 0; padAfter < 5; ++padAfter) {

                    if (veryVerbose) { T_ T_ T_ P(padAfter) }

                    // Generate the whitespace padding before the string.

                    bsl::string bslRTrim;
                    for (int trimIdx = 0; trimIdx < padBefore; ++trimIdx) {
                        // Insert random whitespace characters in the padding.

                        bslRTrim.push_back(white[bsl::rand() % NUM_WHITE]);
                    }

                    std::string stdRTrim;
                    for (int trimIdx = 0; trimIdx < padBefore; ++trimIdx) {
                        // Insert random whitespace characters in the padding.

                        stdRTrim.push_back(white[bsl::rand() % NUM_WHITE]);
                    }

                    // Generate the whitespace padding after the string.

                    bsl::string bslLTrim;
                    for (int trimIdx = 0; trimIdx < padAfter; ++trimIdx) {
                        // Insert random whitespace characters in the padding.

                        bslLTrim.push_back(white[bsl::rand() % NUM_WHITE]);
                    }

                    std::string stdLTrim;
                    for (int trimIdx = 0; trimIdx < padAfter; ++trimIdx) {
                        // Insert random whitespace characters in the padding.

                        stdLTrim.push_back(white[bsl::rand() % NUM_WHITE]);
                    }

                    // Generate the string to be trimmed.

                    bsl::string bslString(bslRTrim);
                    bslString.append(STRINGS[i]);
                    bslString.append(bslLTrim);

                    std::string stdString(stdRTrim);
                    stdString.append(STRINGS[i]);
                    stdString.append(stdLTrim);

                    bsl::string bslStringBeforeTrim = bslString;
                    std::string stdStringBeforeTrim = stdString;

                    bsl::strcpy(cstring, bslString.c_str());

                    ASSERTV(bslString.size(), bslString.size() <= INT_MAX);
                    int length = static_cast<int>(bslString.size());
                    ASSERTV(i, length < static_cast<int>(sizeof cstring));

                    bsl::memset(nonNullString, 'Z', sizeof(nonNullString));
                    bsl::memcpy(nonNullString, cstring, length);

                    // Generate the expected results for 'ltrim' and 'rtrim'.

                    if (0 == bsl::strlen(STRINGS[i])) {
                        // Special case: If the original string was "", all
                        // (before and after) padding will be removed by the
                        // 'ltrim' and 'rtrim' methods.  Therefore, clear the
                        // '*LTrim' and '*RTrim' strings.

                        bslLTrim.clear();
                        bslRTrim.clear();

                        stdLTrim.clear();
                        stdRTrim.clear();
                    }
                    else {
                        bslLTrim.insert(0, STRINGS[i]);  // prepend
                        bslRTrim += STRINGS[i];          // append

                        stdLTrim.insert(0, STRINGS[i]);  // prepend
                        stdRTrim += STRINGS[i];          // append
                    }

                    // Testing 'ltrim'.

                    Util::ltrim(cstring);
                    ASSERTV(i, bslLTrim == cstring);
                    Util::ltrim(&bslString);
                    ASSERTV(i, bslLTrim == bslString);
                    Util::ltrim(&stdString);
                    ASSERTV(i, stdLTrim == stdString);
                    Util::ltrim(nonNullString, &length);
                    ASSERTV(bslLTrim.size(), bslLTrim.size() <= INT_MAX);
                    ASSERTV(i, static_cast<int>(bslLTrim.size()) == length);
                    ASSERTV(i, 0 == bsl::strncmp(bslLTrim.c_str(),
                                                 nonNullString,
                                                 length));

                    // Testing 'rtrim'.

                    bslString = bslStringBeforeTrim;
                    stdString = stdStringBeforeTrim;
                    bsl::strcpy(cstring, bslStringBeforeTrim.c_str());
                    ASSERTV(bslString.size(), bslString.size() <= INT_MAX);
                    length = static_cast<int>(bslString.size());
                    bsl::memset(nonNullString, 'Z', sizeof(nonNullString));
                    bsl::memcpy(nonNullString, cstring, length);

                    Util::rtrim(cstring);
                    ASSERTV(i, bslRTrim == cstring);
                    Util::rtrim(&bslString);
                    ASSERTV(i, bslRTrim == bslString);
                    Util::rtrim(&stdString);
                    ASSERTV(i, stdRTrim == stdString);
                    Util::rtrim(nonNullString, &length);
                    ASSERTV(bslRTrim.size(), bslRTrim.size() <= INT_MAX);
                    ASSERTV(i, static_cast<int>(bslRTrim.size()) == length);
                    ASSERTV(i, 0 == bsl::strncmp(bslRTrim.c_str(),
                                                 nonNullString,
                                                 length));

                    // Testing 'trim'.

                    bslString = bslStringBeforeTrim;
                    stdString = stdStringBeforeTrim;
                    bsl::strcpy(cstring, bslStringBeforeTrim.c_str());
                    ASSERTV(bslString.size(), bslString.size() <= INT_MAX);
                    length = static_cast<int>(bslString.size());
                    bsl::memset(nonNullString, 'Z', sizeof(nonNullString));
                    bsl::memcpy(nonNullString, cstring, length);

                    Util::trim(cstring);
                    ASSERTV(i, 0 == bsl::strcmp(STRINGS[i], cstring));
                    Util::trim(&bslString);
                    ASSERTV(i, STRINGS[i] == bslString);
                    Util::trim(&stdString);
                    ASSERTV(i, STRINGS[i] == stdString);
                    Util::trim(nonNullString, &length);
                    ASSERTV(bsl::strlen(STRINGS[i]),
                            bsl::strlen(STRINGS[i]) <= INT_MAX);
                    ASSERTV(i,
                          static_cast<int>(bsl::strlen(STRINGS[i])) == length);
                    ASSERTV(i,
                         0 == bsl::strncmp(STRINGS[i], nonNullString, length));
                }
            }
        }

        if (verbose) cout << "\tTesting 'std::pmr::string'" << endl;

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
        for (int i = 0; STRINGS[i]; ++i) {

            if (veryVerbose) { T_ P(i) }

            for (int padBefore = 0; padBefore < 5; ++padBefore) {

                if (veryVerbose) { T_ T_ P(padBefore) }

                for (int padAfter = 0; padAfter < 5; ++padAfter) {

                    if (veryVerbose) { T_ T_ T_ P(padAfter) }

                    // Generate the whitespace padding before the string.

                    std::pmr::string pmrRTrim;
                    for (int trimIdx = 0; trimIdx < padBefore; ++trimIdx) {
                        // Insert random whitespace characters in the padding.

                        pmrRTrim.push_back(white[bsl::rand() % NUM_WHITE]);
                    }

                    // Generate the whitespace padding after the string.

                    std::pmr::string pmrLTrim;
                    for (int trimIdx = 0; trimIdx < padAfter; ++trimIdx) {
                        // Insert random whitespace characters in the padding.

                        pmrLTrim.push_back(white[bsl::rand() % NUM_WHITE]);
                    }

                    // Generate the string to be trimmed.

                    std::pmr::string pmrString(pmrRTrim);
                    pmrString.append(STRINGS[i]);
                    pmrString.append(pmrLTrim);

                    std::pmr::string pmrStringBeforeTrim = pmrString;

                    // Generate the expected results for 'ltrim' and 'rtrim'.

                    if (0 == bsl::strlen(STRINGS[i])) {
                        // Special case: If the original string was "", all
                        // (before and after) padding will be removed by the
                        // 'ltrim' and 'rtrim' methods.  Therefore, clear the
                        // '*LTrim' and '*RTrim' strings.

                        pmrLTrim.clear();
                        pmrRTrim.clear();
                    }
                    else {
                        pmrLTrim.insert(0, STRINGS[i]);  // prepend
                        pmrRTrim += STRINGS[i];          // append
                    }

                    // Testing 'ltrim'.

                    Util::ltrim(&pmrString);
                    ASSERTV(i, pmrLTrim == pmrString);

                    // Testing 'rtrim'.

                    pmrString = pmrStringBeforeTrim;

                    Util::rtrim(&pmrString);
                    ASSERTV(i, pmrRTrim == pmrString);

                    // Testing 'trim'.

                    pmrString = pmrStringBeforeTrim;

                    Util::trim(&pmrString);
                    ASSERTV(i, STRINGS[i] == pmrString);
                }
            }
        }
#endif

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'lowerCaseCmp' AND 'upperCaseCmp'
        //
        // Concerns:
        //: 1 All of the 'lowerCaseCmp' and 'upperCaseCmp' methods return the
        //:   correct results when comparing two strings of various types
        //:   ('bsl::string', null-terminated C-style, and a
        //:   non-null-terminated 'const char *' with an 'int' length
        //:   specifier).
        //:
        //: 2 Specifically the methods that take a non-null-terminated
        //:   'const char *' strings as input will not read pass the end of
        //:   these strings.
        //:
        //: 3 The algorithm performs coherently with extended ASCII characters
        //:   (128-255).
        //
        // Plan:
        //: 1 This case uses the Ad-Hoc Data Selection Method and the
        //:   Table-Driven Implementation Technique.
        //:
        //: 2 To address concern 1, we arbitrarily select a set S of string
        //:   pairs and exercise all the 'lowerCaseCmp' and 'upperCaseCmp'
        //:   methods and then verify the results by converting these strings
        //:   to upper/lower cases.  These strings contain letters, digits,
        //:   symbols, and special characters such as '\t' or '\n'.  For each
        //:   pair of strings (s1, s2) in the set S, we generate all 3 types of
        //:   strings for both s1 and s2, then pass them to the corresponding
        //:   methods in the order of both (s1, s2) and (s2, s1) and verify the
        //:   results.  Specifically for the non-null-terminated 'const char *'
        //:   strings, we will vary the length of these strings while comparing
        //:   against other strings using one of the above methods.
        //:
        //: 3 To address concern 2, we create a pair of non-null-terminated
        //:   strings for each string pair (s1, s2) by copying s1 and s2 into
        //:   two buffers pre-filled with non-zero content.  This will cause
        //:   incorrect compare results if any of these 'lowerCaseCmp' or
        //:   'upperCaseCmp' methods read pass the end of these strings.
        //
        // Testing:
        //  lowerCaseCmp(cchar *lhsStr, cchar *rhsStr);
        //  lowerCaseCmp(cchar *lhsStr, cchar *rhsStr, int rhsL);
        //  lowerCaseCmp(cchar *lhsStr, cBslStr& rhsStr);
        //  lowerCaseCmp(cchar *lhsStr, int lshL, cchar *rhsStr);
        //  lowerCaseCmp(cchar *lhsStr, int lshL, cchar *rhsStr, int rhsL);
        //  lowerCaseCmp(cchar *lhsStr, int lshL, cBslStr& rhsStr);
        //  lowerCaseCmp(cBslStr& lhsStr, cchar *rhsStr);
        //  lowerCaseCmp(cBslStr& lhsStr, cchar *rhsStr, int rhsL);
        //  lowerCaseCmp(cBslStr& lhsStr, cBslStr& rhsStr);
        //  upperCaseCmp(cchar *lhs, cchar *rhs);
        //  upperCaseCmp(cchar *lhs, cchar *rhs, int rhsL);
        //  upperCaseCmp(cchar *lhs, cBslStr& rhs);
        //  upperCaseCmp(cchar *lhs, int lhsL, cchar *rhs);
        //  upperCaseCmp(cchar *lhs, int lhsL, cchar *rhs, int rhsL);
        //  upperCaseCmp(cchar *lhs, int lhsL, cBslStr& rhs);
        //  upperCaseCmp(cBslStr& lhs, cchar *rhs);
        //  upperCaseCmp(cBslStr& lhs, cchar *rhs, int rhsL);
        //  upperCaseCmp(cBslStr& lhs, cBslStr& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout <<
                         "\n" "TESTING 'lowerCaseCmp' AND 'upperCaseCmp'" "\n"
                              "=========================================" "\n";

        static const struct {
            const int   d_lineNumber;
            const char *d_string1;
            const char *d_string2;
        } DATA[] = {
            //LINE, STRING1,             STRING2

            // empty string tests
            { L_,   "",                  ""         },
            { L_,   "",                  " "        },
            { L_,   "",                  "a"        },
            { L_,   "",                  "A"        },

            // whitespace characters (lengths 1 vs 1)
            { L_,   " ",                 " "        },
            { L_,   "\t",                "\n"       },
            { L_,   "\n",                "a"        },
            // symbols between lowercase and uppercase (lengths 1 vs 1)
            { L_,   "`",                 "`"        },
            { L_,   "`",                 "["        },
            { L_,   "`",                 "a"        },
            { L_,   "`",                 "A"        },
            // letters, digits, and symbols (lengths 1 vs 1)
            { L_,   "a",                 "a"        },
            { L_,   "a",                 "A"        },
            { L_,   "a",                 "b"        },
            { L_,   "a",                 "1"        },
            { L_,   "a",                 "*"        },

            // lengths 1 vs 2
            { L_,   " ",                 "  "       },
            { L_,   " ",                 " a"       },
            { L_,   "a",                 "  "       },
            { L_,   "a",                 " a"       },
            { L_,   "a",                 "a "       },
            { L_,   "a",                 "A "       },
            { L_,   "a",                 "` "       },
            { L_,   "A",                 "` "       },
            { L_,   "b",                 "a "       },

            // whitespace characters (lengths 2 vs 2)
            { L_,   "  ",                "  "       },
            { L_,   "\t ",               "\n "      },
            { L_,   "a ",                "  "       },
            { L_,   "a ",                "a "       },
            { L_,   "a ",                " a"       },
            { L_,   " b",                "  "       },
            { L_,   "ab",                "a "       },
            { L_,   "ab",                " b"       },
            // symbols between lowercase and uppercase (lengths 2 vs 2)
            { L_,   "ab",                "`b"       },
            { L_,   "ab",                "a`"       },
            { L_,   "AB",                "`B"       },
            { L_,   "AB",                "A`"       },
            // letters, digits, and symbols (lengths 2 vs 2)
            { L_,   "ab",                "ab"       },
            { L_,   "ab",                "aB"       },
            { L_,   "ab",                "Ab"       },
            { L_,   "ab",                "AB"       },
            { L_,   "ab",                "ac"       },
            { L_,   "ab",                "bb"       },
            { L_,   "ab",                "1b"       },
            { L_,   "ab",                "a1"       },
            { L_,   "ab",                "a*"       },
            { L_,   "ab",                "*a"       },

            // lengths 2 vs 3
            { L_,   "  ",                "   "      },
            { L_,   "\t ",               "   "      },
            { L_,   "ab",                "   "      },
            { L_,   "ab",                "a  "      },
            { L_,   "ab",                " b "      },
            { L_,   "ab",                "ab "      },
            { L_,   "ab",                "AB "      },
            { L_,   "ab",                "`b "      },
            { L_,   "AB",                "`B "      },

            // whitespace characters (lengths 3 vs 3)
            { L_,   "   ",               "   "      },
            { L_,   "\t  ",              "\n  "     },
            { L_,   "ab ",               "   "      },
            { L_,   "ab ",               "ab "      },
            { L_,   "ab ",               "AB "      },
            { L_,   "ab ",               "bb "      },
            { L_,   " bc",               "   "      },
            { L_,   " bc",               " bc"      },
            { L_,   " bc",               " BC"      },
            { L_,   " bc",               " cc"      },
            { L_,   "a c",               "   "      },
            { L_,   "a c",               "a c"      },
            { L_,   "a c",               "A C"      },
            { L_,   "a c",               "b c"      },
            // symbols between lowercase and uppercase (lengths 3 vs 3)
            { L_,   "`bc",               "abc"      },
            { L_,   "a`c",               "abc"      },
            { L_,   "ab`",               "abc"      },
            { L_,   "`BC",               "ABC"      },
            { L_,   "A`C",               "ABC"      },
            { L_,   "AB`",               "ABC"      },
            // letters, digits, and symbols (lengths 3 vs 3)
            { L_,   "abc",               "abc"      },
            { L_,   "abc",               "Abc"      },
            { L_,   "abc",               "aBc"      },
            { L_,   "abc",               "abC"      },
            { L_,   "abc",               "ABC"      },
            { L_,   "abc",               "abd"      },
            { L_,   "abc",               "1bc"      },
            { L_,   "abc",               "a1c"      },
            { L_,   "abc",               "ab1"      },
            { L_,   "abc",               "*bc"      },
            { L_,   "abc",               "a*c"      },
            { L_,   "abc",               "ab*"      },

            // lengths 3 vs 4 and beyond
            { L_,   "abc",               "abcc"     },
            { L_,   "abc",               "ABCC",    },
            { L_,   "abc",               "bbcd",    },
            { L_,   "abcD",              "ABcD",    },
            { L_,   "abcd",              "accd",    },
            { L_,   "abc123de",          "Abc123DeF"},

            // test with negative signed characters
            { L_,   "\xc8\xc9\xca",    "\xc7\xc9\xca" },
            { L_,   "\xc8\xc9\xcb",    "\xc7\xc9\xca" },
            { L_,   "\xc8\xc9\xca",    "\xc7\xc9\xcb" },
            { L_,   "\xc8\xc9\xca",    "\xc9\xc9\xca" },
            { L_,   "\xc8\xc9\xca",    ""             },
            { L_,   "",                "\xc8\xc9\xca" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int nb = 0; nb < NUM_DATA; ++nb) {

            if (veryVerbose) { T_ P(nb) }

            const char *STRING1 = DATA[nb].d_string1;
            const char *STRING2 = DATA[nb].d_string2;

            if (veryVeryVerbose) {
                T_ T_ P(STRING1)
                T_ T_ P(STRING2)
            }

            // Compare string 1 to string 2, then string 2 to string 1.

            for (int i = 0; i < 2; ++i) {

                if (veryVerbose) { T_ T_ P(i) }

                const char *cstring1, *cstring2;

                if (0 == i) {
                    cstring1 = STRING1;
                    cstring2 = STRING2;
                }
                else {
                    cstring1 = STRING2;
                    cstring2 = STRING1;
                }

                bsl::string string1(cstring1);
                bsl::string string2(cstring2);

                // Testing 'lowerCaseCmp'.

                bsl::string lowerStr1(string1);
                bsl::string lowerStr2(string2);
                Util::toLower(&lowerStr1);
                Util::toLower(&lowerStr2);

                int result = lowerStr1.compare(lowerStr2);
                result = result > 0 ? 1 : (result < 0 ? -1 : 0);
                ASSERTV(nb, result == Util::lowerCaseCmp(cstring1, cstring2));
                ASSERTV(nb, result == Util::lowerCaseCmp(cstring1, string2));
                ASSERTV(nb, result == Util::lowerCaseCmp(string1,  cstring2));
                ASSERTV(nb, result == Util::lowerCaseCmp(string1,  string2));

                char nonNullString1[64], nonNullString2[64];
                int  length1 = static_cast<int>(string1.size());
                ASSERTV(nb, i,
                        length1 < static_cast<int>(sizeof nonNullString1));
                int  length2 = static_cast<int>(string2.size());
                ASSERTV(nb, i,
                        length2 < static_cast<int>(sizeof nonNullString2));

                bsl::memset(nonNullString1, 'Z', sizeof(nonNullString1));
                bsl::memset(nonNullString2, 'Y', sizeof(nonNullString2));
                bsl::memcpy(nonNullString1, cstring1, length1);
                bsl::memcpy(nonNullString2, cstring2, length2);

                // Testing methods that take a non-null-terminated string as
                // their first argument.

                for (int len = -1; len <= length1; ++len) {

                    if (veryVerbose) { T_ T_ T_ P(len) }

                    result = lowerStr1.compare(0, nneg(len), lowerStr2);
                    result = result > 0 ? 1 : (result < 0 ? -1 : 0);
                    ASSERTV(nb, result ==
                                    Util::lowerCaseCmp(ns(nonNullString1, len),
                                                       nneg(len),
                                                       cstring2));
                    ASSERTV(nb, result ==
                                    Util::lowerCaseCmp(ns(nonNullString1, len),
                                                       nneg(len),
                                                       string2));
                }

                // Testing methods that take a non-null-terminated string as
                // their second argument.

                for (int len = -1; len <= length2; ++len) {

                    if (veryVerbose) { T_ T_ T_ P(len) }

                    result = lowerStr2.compare(0, nneg(len), lowerStr1);
                    result = result > 0 ? -1 : (result < 0 ? 1 : 0);
                    ASSERTV(nb, result ==
                                    Util::lowerCaseCmp(cstring1,
                                                       ns(nonNullString2, len),
                                                       nneg(len)));
                    ASSERTV(nb, result ==
                                    Util::lowerCaseCmp(string1,
                                                       ns(nonNullString2, len),
                                                       nneg(len)));
                }

                // Testing methods that take non-null-terminated strings as
                // both of their arguments.

                for (int len1 = -1; len1 <= length1; ++len1) {

                    if (veryVerbose) { T_ T_ T_ P(len1) }

                    for (int len2 = -1; len2 <= length2; ++len2) {

                        if (veryVerbose) { T_ T_ T_ T_ P(len2) }

                        result = lowerStr1.compare(0, nneg(len1), lowerStr2, 0,
                                                                   nneg(len2));
                        result = result > 0 ? 1 : (result < 0 ? -1 : 0);
                        ASSERTV(nb, result ==
                                   Util::lowerCaseCmp(ns(nonNullString1, len1),
                                                      nneg(len1),
                                                      ns(nonNullString2, len2),
                                                      nneg(len2)));
                    }
                }

                // Testing 'upperCaseCmp'.

                bsl::string upperStr1(cstring1);
                bsl::string upperStr2(cstring2);
                Util::toUpper(&upperStr1);
                Util::toUpper(&upperStr2);

                result = upperStr1.compare(upperStr2);
                result = result > 0 ? 1 : (result < 0 ? -1 : 0);
                ASSERTV(nb, result == Util::upperCaseCmp(cstring1, cstring2));
                ASSERTV(nb, result == Util::upperCaseCmp(cstring1, string2));
                ASSERTV(nb, result == Util::upperCaseCmp(string1,  cstring2));
                ASSERTV(nb, result == Util::upperCaseCmp(string1,  string2));

                // Testing methods that take a non-null-terminated string as
                // their first argument.

                for (int len = -1; len <= length1; ++len) {

                    if (veryVerbose) { T_ T_ T_ P(len) }

                    result = upperStr1.compare(0, nneg(len), upperStr2);
                    result = result > 0 ? 1 : (result < 0 ? -1 : 0);
                    ASSERTV(nb, result ==
                                    Util::upperCaseCmp(ns(nonNullString1, len),
                                                       nneg(len),
                                                       cstring2));
                    ASSERTV(nb, result ==
                                    Util::upperCaseCmp(ns(nonNullString1, len),
                                                       nneg(len),
                                                       string2));
                }

                // Testing methods that take a non-null-terminated string as
                // their second argument.

                for (int len = -1; len <= length2; ++len) {

                    if (veryVerbose) { T_ T_ T_ P(len) }

                    result = upperStr2.compare(0, nneg(len), upperStr1);
                    result = result > 0 ? -1 : (result < 0 ? 1 : 0);
                    ASSERTV(nb, result ==
                                    Util::upperCaseCmp(cstring1,
                                                       ns(nonNullString2, len),
                                                       nneg(len)));
                    ASSERTV(nb, result ==
                                    Util::upperCaseCmp(string1,
                                                       ns(nonNullString2, len),
                                                       nneg(len)));
                }

                // Testing methods that take non-null-terminated strings as
                // both of their arguments.

                for (int len1 = -1; len1 <= length1; ++len1) {

                    if (veryVerbose) { T_ T_ T_ P(len1) }

                    for (int len2 = -1; len2 <= length2; ++len2) {

                        if (veryVerbose) { T_ T_ T_ T_ P(len2) }

                        result = upperStr1.compare(0, nneg(len1), upperStr2, 0,
                                                                   nneg(len2));
                        result = result > 0 ? 1 : (result < 0 ? -1 : 0);
                        ASSERTV(nb, result ==
                                   Util::upperCaseCmp(ns(nonNullString1, len1),
                                                      nneg(len1),
                                                      ns(nonNullString2, len2),
                                                      nneg(len2)));
                    }
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'areEqualCaseless'
        //
        // Concerns:
        //: 1 All of the 'areEqualCaseless' methods return the correct results
        //:   when comparing two strings of various types ('bsl::string',
        //:   null-terminated C-style, and a non-null-terminated 'const char *'
        //:   with an 'int' length specifier).
        //:
        //: 2 Specifically the methods that take non-null-terminated
        //:   'const char *' strings as input will not read pass the end of
        //:   these strings.
        //
        // Plan:
        //: 1 This case uses the Ad-Hoc Data Selection Method and the
        //:   Array-Based Implementation Technique.
        //:
        //: 2 To address concern 1, we arbitrarily select a set of strings and
        //:   use the 'areEqualCaseless' methods to compare these strings
        //:   against each other and verify the results against the ones
        //:   returned by 'strcasecmp' or 'strncasecmp'.  These strings contain
        //:   letters, digits, symbols, and special characters such as '\t' or
        //:   '\n'.  Specifically for the non-null-terminated 'const char *'
        //:   strings, we will vary the length of these strings while comparing
        //:   against other strings using one of the above methods.
        //:
        //: 3 To address concern 2, we create the non-null-terminated strings
        //:   by copying the original string without the null character into a
        //:   buffer pre-filled with non-zero content.  This will cause
        //:   incorrect compare results if any of these 'areEqualCaseless'
        //:   methods read pass the end of these strings.
        //
        // Testing:
        //  areEqualCaseless(cchar *lhs, cchar *rhs);
        //  areEqualCaseless(cchar *lhs, cchar *rhs, int rhsL);
        //  areEqualCaseless(cchar *lhs, cBslStr& rhs);
        //  areEqualCaseless(cchar *lhs, int lshL, cchar *rhs);
        //  areEqualCaseless(cchar *lhs, int lshL, cchar *rhs, int rhsL);
        //  areEqualCaseless(cchar *lhs, int lshL, cBslStr& rhs);
        //  areEqualCaseless(cBslStr& lhs, cchar *rhs);
        //  areEqualCaseless(cBslStr& lhs, cchar *rhs, int rhsL);
        //  areEqualCaseless(cBslStr& lhs, cBslStr& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout <<
                    "\n" "TESTING 'areEqualCaseless' AND 'isNoCaseEqual'" "\n"
                         "==============================================" "\n";

        static const char *STRINGS[] = {
            // strings of length 0 or 1
            "", " ", "@", "!", "\t", "\n", "1", "0", "Z", "z",

            // strings of length 2 with whitespace characters
            "  ", " \t", " z", "Z ", "@\t", "\t@", "1\n", "\n1",
            // strings of length 2 consisting of letters
            "yz", "YZ", "Yz", "yZ",
            // strings of length 2 consisting of letters and/or digits
            "y1", "1y", "Z0", "0Z", "01",
            // strings of length 2 with symbols
            "x@", "@y", "@1", "1@", "@!",

            // strings of length 3 with whitespace
            "x  ", " y ", "  z", "X\n\n", "\nY\n", "\n\nZ",
            "xy ", "x z", " yz", "X\tZ", "\tYZ", "XY\t",
            // strings of length 3 consisting of letters
            "xyz", "xyZ", "xYz", "xYZ", "Xyz", "XyZ", "XYz", "XYZ",
            // strings of length 3 consisting of letters and/or digits
            "1YZ", "X1Z", "XY1", "X10", "2Y0", "21Z", "210",
            // strings of length 3 with symbols
            "@yZ", "X@z", "xY@", "@!z", "@y!", "X@!", "#@!",

            // strings with length of 4 and above
            "wXyZ", "WxYz",
            "abcdefghijklmnopqrstuvwxyz", "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        0};

        for (int i = 0; STRINGS[i]; ++i) {

            if (veryVerbose) { T_ P(i) }

            for (int j = 0; STRINGS[j]; ++j) {

                if (veryVerbose) { T_ T_ P_(i) P(j) }

                const char *cstring1, *cstring2;

                cstring1 = STRINGS[i];
                cstring2 = STRINGS[j];

                bsl::string string1(cstring1);
                bsl::string string2(cstring2);

                // Concern 1

                bool result = !strcasecmp(cstring1, cstring2);
                ASSERTV(i, j, result ==
                                   Util::areEqualCaseless(cstring1, cstring2));
                ASSERTV(i, j, result ==
                                   Util::areEqualCaseless(cstring1, string2));
                ASSERTV(i, j, result ==
                                   Util::areEqualCaseless(string1,  string2));
                ASSERTV(i, j, result ==
                                   Util::areEqualCaseless(string1,  cstring2));

                // Concern 2

                char nonNullString1[64], nonNullString2[64];
                int  length1 = static_cast<int>(string1.size());
                ASSERTV(i, j,
                        length1 < static_cast<int>(sizeof nonNullString1));
                int  length2 = static_cast<int>(string2.size());
                ASSERTV(i, j,
                        length2 < static_cast<int>(sizeof nonNullString2));

                bsl::memset(nonNullString1, 'Z', sizeof(nonNullString1));
                bsl::memset(nonNullString2, 'Y', sizeof(nonNullString2));
                bsl::memcpy(nonNullString1, cstring1, length1);
                bsl::memcpy(nonNullString2, cstring2, length2);

                // Testing methods that take a non-null-terminated string as
                // their first argument.

                for (int len = -1; len <= length1; ++len) {

                    if (veryVerbose) { T_ P(len) }

                    // We need to compare the lengths because 'strncasecmp'
                    // compares only the first 'len' characters and return 0 if
                    // they match, even though the string lengths might be
                    // different.  Note that 'strncasecmp' returns 0 on
                    // success.

                    result = (nneg(len) == length2)
                          && !strncasecmp(nonNullString1, cstring2, nneg(len));

                    ASSERTV(i, j, result ==
                                Util::areEqualCaseless(ns(nonNullString1, len),
                                                       nneg(len),
                                                       cstring2));

                    ASSERTV(i, j, result ==
                                Util::areEqualCaseless(ns(nonNullString1, len),
                                                       nneg(len),
                                                       string2));
                }

                // Testing methods that take a non-null-terminated string as
                // their second argument.

                for (int len = -1; len <= length2; ++len) {

                    if (veryVerbose) { T_ P(len) }

                    // We need to compare the lengths because 'strncasecmp'
                    // compares only the first 'len' characters and return 0 if
                    // they match, even though the string lengths might be
                    // different.  Note that 'strncasecmp' returns 0 on
                    // success.

                    result = (nneg(len) == length1)
                          && !strncasecmp(cstring1, nonNullString2, nneg(len));

                    ASSERTV(i, j, result ==
                                Util::areEqualCaseless(string1,
                                                       ns(nonNullString2, len),
                                                       nneg(len)));

                    ASSERTV(i, j, result ==
                                Util::areEqualCaseless(cstring1,
                                                       ns(nonNullString2, len),
                                                       nneg(len)));
                }

                // Testing methods that take non-null-terminated strings as
                // both of their arguments.

                for (int len1 = -1; len1 <= length1; ++len1) {

                    if (veryVerbose) { T_ P(len1) }

                    for (int len2 = -1; len2 <= length2; ++len2) {

                        if (veryVerbose) { T_ T_ P_(len1) P(len2) }

                        // We need to compare the lengths because 'strncasecmp'
                        // compares only the first 'len' characters and return
                        // 0 if they match, even though the string lengths
                        // might be different.  Note that 'strncasecmp' returns
                        // 0 on success.

                        result = (nneg(len1) == nneg(len2))
                              && !strncasecmp(nonNullString1, nonNullString2,
                                                                   nneg(len1));
                        ASSERTV(i, j, result ==
                               Util::areEqualCaseless(ns(nonNullString1, len1),
                                                      nneg(len1),
                                                      ns(nonNullString2, len2),
                                                      nneg(len2)));
                    }
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'toLower' AND 'toUpper'
        //
        // Concerns:
        //: 1 These methods properly convert input strings to lower/upper
        //:   cases.
        //:
        //: 2 If the string supplied is a non-null-terminated 'const char *'
        //:   with an 'int' length specifier, the corresponding methods won't
        //:   go beyond the end of the string.
        //
        // Plan:
        //: 1 This case uses the Ad-Hoc Data Selection Method and the
        //:   Array-Based Implementation Technique.
        //:
        //: 2 To address concern 1, we use 'tolower' or 'toupper' to
        //:   convert the supplied test strings to lower/upper cases.  Then we
        //:   compare them with the results of 'toLower' and 'toUpper' to make
        //:   sure they match.
        //:
        //: 3 To address concern 2, we create the non-null-terminated
        //:   'const char  *' string in a buffer pre-filled with non-null data.
        //:   Then we verify that after the corresponding methods are invoked,
        //:   the content in the buffer beyond the valid range of the string is
        //:   unmodified.
        //
        // Testing:
        //  toLower(char *string);
        //  toLower(char *string, int length);
        //  toLower(bsl::string *string);
        //  toLower(std::string *string);
        //  toLower(std::pmr::string *string);
        //  toUpper(char *string);
        //  toUpper(char *string, int length);
        //  toUpper(bsl::string *string);
        //  toUpper(std::string *string);
        //  toUpper(std::pmr::string *string);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "TESTING 'toLower' AND 'toUpper'" "\n"
                                  "===============================" "\n";

        // 'bdlb::String' functions assume 7-bit ASCII, so we 'setlocale' for
        // the calls to '::tolower' and '::toupper' in this test case.

        bsl::setlocale(LC_CTYPE, "C");

        static const char *STRINGS[] = {
            // strings of length 0 or 1
            "", " ", "@", "!", "\t", "\n", "1", "0", "Z", "z",

            // strings of length 2 with whitespace characters
            "  ", " \t", " z", "Z ", "@\t", "\t@", "1\n", "\n1",
            // strings of length 2 consisting of letters
            "yz", "YZ", "Yz", "yZ",
            // strings of length 2 consisting of letters and/or digits
            "y1", "1y", "Z0", "0Z", "01",
            // strings of length 2 with symbols
            "x@", "@y", "@1", "1@", "@!",

            // strings of length 3 with whitespace
            "x  ", " y ", "  z", "X\n\n", "\nY\n", "\n\nZ",
            "xy ", "x z", " yz", "X\tZ", "\tYZ", "XY\t",
            // strings of length 3 consisting of letters
            "xyz", "xyZ", "xYz", "xYZ", "Xyz", "XyZ", "XYz", "XYZ",
            // strings of length 3 consisting of letters and/or digits
            "1YZ", "X1Z", "XY1", "X10", "2Y0", "21Z", "210",
            // strings of length 3 with symbols
            "@yZ", "X@z", "xY@", "@!z", "@y!", "X@!", "#@!",

            // strings with length of 4 and above
            "wXyZ", "WxYz",
            "abcdefghijklmnopqrstuvwxyz", "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        0};

        for (int i = 0; STRINGS[i]; ++i) {

            if (veryVerbose) { T_ P(i) }

            {
                // Testing 'toLower'.

                char cstring[64];
                bsl::strcpy(cstring, STRINGS[i]);
                const int LENGTH = static_cast<int>(bsl::strlen(cstring));
                ASSERTV(i, LENGTH < static_cast<int>(sizeof cstring));
                bsl::string bslString(cstring);
                std::string stdString(cstring);
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
                std::pmr::string pmrString(cstring);
#endif
                char        nonNullString[sizeof(cstring)];
                bsl::memset(nonNullString, 'Z', sizeof(nonNullString));
                bsl::memcpy(nonNullString, cstring, LENGTH);

                Util::toLower(cstring);
                Util::toLower(&bslString);
                Util::toLower(&stdString);
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
                Util::toLower(&pmrString);
#endif
                Util::toLower(nonNullString, LENGTH);

                bsl::string lowerStr(STRINGS[i]);
                bsl::transform(lowerStr.begin(),
                               lowerStr.end(),
                               lowerStr.begin(),
                               ::tolower);
                ASSERTV(i, lowerStr == bslString);
                ASSERTV(i, lowerStr == stdString);
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
                ASSERTV(i, lowerStr == pmrString);
#endif
                ASSERTV(i, lowerStr == cstring);
                ASSERTV(i, 0 == bsl::strncmp(lowerStr.c_str(),
                                             nonNullString,
                                             LENGTH));
                ASSERTV(i, 'Z' == nonNullString[LENGTH]);
            }
            {
                // Testing 'toUpper'.

                char cstring[64];
                bsl::strcpy(cstring, STRINGS[i]);
                const int LENGTH = static_cast<int>(bsl::strlen(cstring));
                ASSERTV(i, LENGTH < static_cast<int>(sizeof cstring));
                bsl::string bslString(cstring);
                std::string stdString(cstring);
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
                std::pmr::string pmrString(cstring);
#endif
                char        nonNullString[sizeof(cstring)];
                bsl::memset(nonNullString, 'z', sizeof(nonNullString));
                bsl::memcpy(nonNullString, cstring, LENGTH);

                Util::toUpper(cstring);
                Util::toUpper(&bslString);
                Util::toUpper(&stdString);
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
                Util::toUpper(&pmrString);
#endif
                Util::toUpper(nonNullString, LENGTH);

                bsl::string upperStr(STRINGS[i]);
                bsl::transform(upperStr.begin(),
                               upperStr.end(),
                               upperStr.begin(),
                               ::toupper);
                ASSERTV(i, upperStr == bslString);
                ASSERTV(i, upperStr == stdString);
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
                ASSERTV(i, upperStr == pmrString);
#endif
                ASSERTV(i, upperStr == cstring);
                ASSERTV(i, 0 == bsl::strncmp(upperStr.c_str(),
                                             nonNullString,
                                             LENGTH));
                ASSERTV(i, 'z' == nonNullString[LENGTH]);
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
        //: 1 Using the Ad-Hoc Data Selection Method and the Brute-Force
        //:   Implementation Technique, create several strings and use them to
        //:   test various methods.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "BREATHING TEST" "\n"
                                  "==============" "\n";

        bsl::string  s1  = "hello";
        bsl::string  s2  = "Hello";
        bsl::string  s3  = "good-bye";

        const char  *cs1 = s1.c_str();
        const char  *cs2 = s2.c_str();
        const char  *cs3 = s3.c_str();

        int          l1  = static_cast<int>(s1.size());
        int          l2  = static_cast<int>(s2.size());
        int          l3  = static_cast<int>(s3.size());

        if (verbose) cout << "\tTesting 'areEqualCaseless'" << endl;

        ASSERT(Util::areEqualCaseless(s1, s2));
        ASSERT(Util::areEqualCaseless(s1, cs2));
        ASSERT(Util::areEqualCaseless(s1, cs2, l2));

        ASSERT(Util::areEqualCaseless(cs1, s2));
        ASSERT(Util::areEqualCaseless(cs1, cs2));
        ASSERT(Util::areEqualCaseless(cs1, cs2, l2));

        ASSERT(Util::areEqualCaseless(cs1, l1, s2));
        ASSERT(Util::areEqualCaseless(cs1, l1, cs2));
        ASSERT(Util::areEqualCaseless(cs1, l1, cs2, l2));

        ASSERT(!Util::areEqualCaseless(s1, s3));
        ASSERT(!Util::areEqualCaseless(s1, cs3));
        ASSERT(!Util::areEqualCaseless(s1, cs3, l3));

        ASSERT(!Util::areEqualCaseless(cs1, s3));
        ASSERT(!Util::areEqualCaseless(cs1, cs3));
        ASSERT(!Util::areEqualCaseless(cs1, cs3, l3));

        ASSERT(!Util::areEqualCaseless(cs1, l1, s3));
        ASSERT(!Util::areEqualCaseless(cs1, l1, cs3));
        ASSERT(!Util::areEqualCaseless(cs1, l1, cs3, l3));

        // --------------------------------------------------------------------

        if (verbose) cout << "\tTesting 'lowerCaseCmp'" << endl;

        ASSERT(0 == Util::lowerCaseCmp(s1, s2));
        ASSERT(0 == Util::lowerCaseCmp(s1, cs2));
        ASSERT(0 == Util::lowerCaseCmp(s1, cs2, l2));

        ASSERT(0 == Util::lowerCaseCmp(cs1, s2));
        ASSERT(0 == Util::lowerCaseCmp(cs1, cs2));
        ASSERT(0 == Util::lowerCaseCmp(cs1, cs2, l2));

        ASSERT(0 == Util::lowerCaseCmp(cs1, l1, s2));
        ASSERT(0 == Util::lowerCaseCmp(cs1, l1, cs2));
        ASSERT(0 == Util::lowerCaseCmp(cs1, l1, cs2, l2));

        ASSERT(1 == Util::lowerCaseCmp(s1, s3));
        ASSERT(1 == Util::lowerCaseCmp(s1, cs3));
        ASSERT(1 == Util::lowerCaseCmp(s1, cs3, l3));

        ASSERT(1 == Util::lowerCaseCmp(cs1, s3));
        ASSERT(1 == Util::lowerCaseCmp(cs1, cs3));
        ASSERT(1 == Util::lowerCaseCmp(cs1, cs3, l3));

        ASSERT(1 == Util::lowerCaseCmp(cs1, l1, s3));
        ASSERT(1 == Util::lowerCaseCmp(cs1, l1, cs3));
        ASSERT(1 == Util::lowerCaseCmp(cs1, l1, cs3, l3));

        // --------------------------------------------------------------------

        if (verbose) cout << "\tTesting 'upperCaseCmp'" << endl;

        ASSERT(0 == Util::upperCaseCmp(s1, s2));
        ASSERT(0 == Util::upperCaseCmp(s1, cs2));
        ASSERT(0 == Util::upperCaseCmp(s1, cs2, l2));

        ASSERT(0 == Util::upperCaseCmp(cs1, s2));
        ASSERT(0 == Util::upperCaseCmp(cs1, cs2));
        ASSERT(0 == Util::upperCaseCmp(cs1, cs2, l2));

        ASSERT(0 == Util::upperCaseCmp(cs1, l1, s2));
        ASSERT(0 == Util::upperCaseCmp(cs1, l1, cs2));
        ASSERT(0 == Util::upperCaseCmp(cs1, l1, cs2, l2));

        ASSERT(1 == Util::upperCaseCmp(s1, s3));
        ASSERT(1 == Util::upperCaseCmp(s1, cs3));
        ASSERT(1 == Util::upperCaseCmp(s1, cs3, l3));

        ASSERT(1 == Util::upperCaseCmp(cs1, s3));
        ASSERT(1 == Util::upperCaseCmp(cs1, cs3));
        ASSERT(1 == Util::upperCaseCmp(cs1, cs3, l3));

        ASSERT(1 == Util::upperCaseCmp(cs1, l1, s3));
        ASSERT(1 == Util::upperCaseCmp(cs1, l1, cs3));
        ASSERT(1 == Util::upperCaseCmp(cs1, l1, cs3, l3));

        // --------------------------------------------------------------------

        {
            if (verbose) cout << "\tTesting 'ltrim'" << endl;

            char        cs[] = "   hello";
            bsl::string str(cs);
            int         len  = static_cast<int>(str.size());

            Util::ltrim(cs);
            ASSERT(0 == bsl::strcmp(cs, "hello"));

            bsl::strcpy(cs, str.c_str());
            Util::ltrim(cs, &len);
            ASSERT(0 == bsl::strncmp(cs, "hello", len));

            Util::ltrim(&str);
            ASSERT(str == "hello");
        }

        // --------------------------------------------------------------------

        {
            if (verbose) cout << "\tTesting 'rtrim'" << endl;

            char        cs[] = "hello    ";
            bsl::string str(cs);
            int         len  = static_cast<int>(str.size());

            Util::rtrim(cs);
            ASSERT(0 == bsl::strcmp(cs, "hello"));

            bsl::strcpy(cs, str.c_str());
            Util::rtrim(cs, &len);
            ASSERT(0 == bsl::strncmp(cs, "hello", len));

            Util::rtrim(&str);
            ASSERT(str == "hello");
        }

        // --------------------------------------------------------------------

        {
            if (verbose) cout << "\tTesting 'trim'" << endl;

            char        cs[] = "   hello   ";
            bsl::string str(cs);
            int         len  = static_cast<int>(str.size());

            Util::trim(cs);
            ASSERT(0 == bsl::strcmp(cs, "hello"));

            bsl::strcpy(cs, str.c_str());
            Util::trim(cs, &len);
            ASSERT(0 == bsl::strncmp(cs, "hello", len));

            Util::trim(&str);
            ASSERT(str == "hello");
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\tTesting 'pad'" << endl;

        bsl::string strPad = "hello";
        Util::pad(&strPad, 8, '!');
        ASSERT(strPad == "hello!!!");
        Util::pad(&strPad, 8, '?');
        ASSERT(strPad == "hello!!!");

        // --------------------------------------------------------------------

        if (verbose) cout << "\tTesting 'strnlen'" << endl;

        ASSERT(static_cast<int>(bsl::strlen(cs1)) == Util::strnlen(cs1, 9));
        ASSERT(                                 3 == Util::strnlen(cs1, 3));

        // --------------------------------------------------------------------

        if (verbose) cout << "\tTesting 'toFixedLength'" << endl;

        char buf[32];
        Util::toFixedLength(buf, 2, "hello", 5, '*');
        ASSERT(0 == bsl::strncmp(buf, "he", 2));
        Util::toFixedLength(buf, 8, "hello", 5, '!');
        ASSERT(0 == bsl::strncmp(buf, "hello!!!", 8));

        // --------------------------------------------------------------------

        if (verbose) cout << "\tTesting 'toLower' and 'toUpper'" << endl;

        char        csLower[] = "Hello123";
        char        csUpper[] = "Hello123";
        bsl::string strLower(csLower);
        bsl::string strUpper(csUpper);
        int         lenLower = static_cast<int>(strLower.size());
        int         lenUpper = static_cast<int>(strUpper.size());

        Util::toLower(csLower);
        ASSERT(0 == bsl::strcmp(csLower, "hello123"));
        bsl::strcpy(csLower, strLower.c_str());

        Util::toLower(&strLower);
        ASSERT(strLower == "hello123");

        Util::toLower(csLower, lenLower);
        ASSERT(0 == bsl::strncmp(csLower, "hello123", 8));

        Util::toUpper(csUpper);
        ASSERT(0 == bsl::strcmp(csUpper, "HELLO123"));
        bsl::strcpy(csUpper, strUpper.c_str());

        Util::toUpper(&strUpper);
        ASSERT(strUpper == "HELLO123");

        Util::toUpper(csUpper, lenUpper);
        ASSERT(0 == bsl::strncmp(csUpper, "HELLO123", 8));

      } break;
        default: {
          cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
          testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
