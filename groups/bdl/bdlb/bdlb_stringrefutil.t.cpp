// bdlb_stringrefutil.t.cpp                                           -*-C++-*-

#include <bdlb_stringrefutil.h>

#include <bslim_testutil.h>
#include <bsls_asserttest.h>

#include <bslstl_stringref.h>

#include <bsl_algorithm.h> // 'bsl::transform'
#include <bsl_cctype.h>    // 'bsl::toupper', 'bsl::tolower'
#include <bsl_cstdlib.h>   // 'bsl::atoi',    'bsl::strspn'
#include <bsl_cstring.h>   // 'bsl::strlen'
#include <bsl_iostream.h>
#include <bsl_string.h>    // 'bsl::memcpy',  'bsl::memcmp'
#include <bsl_vector.h>

using namespace BloombergLP;
using bsl::cerr;
using bsl::cout;
using bsl::dec;
using bsl::endl;
using bsl::flush;
using bsl::hex;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The component under test is a utility component in which some functions are
// used in the implementation of other functions:
//
//: o The 'lowerCaseCmp' function (test case 2) is used in the implementations
//:   of 'strstrCaseless' and 'strrstrCaseless' (test case 4).
//:
//: o The 'trim' function is implemented in terms of 'ltrim' and 'rtim', All
//:   will be tested in case 3; each test for 'trim' will parallel but follow
//:   the corresponding tests for 'ltrim' and 'rtrim'.
//
// The Global Concerns listed below apply to each function of this utility and
// are not repeated in the description of the individual test cases.
// Similarly, the approach for addressing each global concern is stated once
// under Global Plans.
//
// The test case concerns and descriptions will focus on issues particular to
// the specific functions being tested.
//
///Global Concerns:
///---------------
//: 1 The functions handle string in each of these distinguished categories:
//:
//:   o '2 < length': distinct first and last elements, and one or more
//:     undistinguished (neither first nor last) middle elements.
//:
//:   o '2 == length': adjacent first and last elements (i.e., no middle).
//:
//:   o '1 == length': coincident first and last elements.
//:
//:   o '0 == length': no elements (empty)
//:
//:   o 'bslstl::StringRef()': a special case for 'bslstl::StringRef' objects
//:     having a zero address and a zero length.
//:
//: 2 The functions handle all possible byte values in arbitrary order such as:
//:   o ASCII values with embedded '\0' values.
//;   o ASCII values corresponding to upper and lower case letters.
//:   o Extended ASCII values (which have the 8th bit [msb] set).
//:
//: 3 The functions operate on the intended sequence of bytes and no other.
//:
//: 4 The functions accept constant references to 'bslstl::StringRef' objects.
//
///Global Plans:
///-------------
//: 1 Define test input in each length category.  When there are two inputs,
//:   the cross product of the categories is defined.
//:
//: 2 Define test input having embedded nulls and values with the 8th bit set.
//:
//: 3 Where possible, surround test input sequences with sentinel characters
//:   having values that, if included in the calculation, would change the
//:   result.
//:
//: 4 Invoke all function calls with 'const' qualified objects (or
//:   'const'-qualified references to modifiable objects).
//
// ----------------------------------------------------------------------------
// [ 2] areEqualCaseless(const bslstl::StringRef& lhs, rhs);
// [ 2] lowerCaseCmp    (const bslstl::StringRef& lhs, rhs);
// [ 2] upperCaseCmp    (const bslstl::StringRef& lhs, rhs);
//
// [ 3] ltrim(const bslstl::StringRef& string);
// [ 3] rtrim(const bslstl::StringRef& string);
// [ 3]  trim(const bslstl::StringRef& string);
//
// [ 4] strstr         (const bslstl::StringRef& string, subString);
// [ 4] strstrCaseless (const bslstl::StringRef& string, subString);
// [ 4] strrstr        (const bslstl::StringRef& string, subString);
// [ 4] strrstrCaseless(const bslstl::StringRef& string, subString);
//
// [ 5] substr(string, position = 0, numChars = k_NPOS);
// ----------------------------------------------------------------------------
// [ 6] USAGE EXAMPLE
// [ 1] HELPER FUNCTION: 'whitespaceLabel'
// [ 1] HELPER FUNCTION: 'isEqual'
// [ 1] HELPER FUNCTION: 'Local::toLower'
// [ 1] HELPER FUNCTION: 'Local::toUpper'

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

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlb::StringRefUtil Util;
typedef bslstl::StringRef   SR;

typedef int(*Disambiguate)(int);

// ============================================================================
//                              HELPER FUNCTIONS
// ----------------------------------------------------------------------------

const char *whitespaceLabel(int ch)
    // Return the address of a static string containing a two letter sequence
    // representing the specified ASCII whitespace character 'ch'.  If 'ch' is
    // not an ASCII whitespace character, the address of a "XX" string literal
    // is returned.  The mapping of character to string is:
    //..
    //  CHARACTER  LABEL
    //  ---------  -----
    //  ' '        "SP"  // blank space
    //  '\t'       "HT"  // horizontal tab
    //  '\n'       "NL"  // new line
    //  '\v'       "VT"  // vertical tab
    //  '\f'       "FF"  // form feed
    //  '\r'       "CR"  // carriage return
    //  other      "XX"  // NOT A WHITESPACE
    //..
{
    if (' ' == ch) {
        return "SP";                                                  // RETURN
    }

    if (ch < '\t' || '\r' < ch ) {
        return "XX";                                                  // RETURN
    }

    static const char *labels[] = { "HT" // '\t'  9 decimal
                                  , "NL" // '\n' 10
                                  , "VT" // '\v' 11
                                  , "FF" // '\f' 12
                                  , "CR" // '\r' 13
                                  };
    return labels[ch - '\t'];
}

static void testWhitespaceLabel()
    // Execute the validation test for the 'whitespacelabel' helper function.
{
    for (int ch = 0; ch <= 255; ++ch) {
        switch(ch) {
       //-^
       case ' ' : ASSERT(0 == bsl::strcmp("SP", whitespaceLabel( ' '))); break;
       case '\t': ASSERT(0 == bsl::strcmp("HT", whitespaceLabel('\t'))); break;
       case '\n': ASSERT(0 == bsl::strcmp("NL", whitespaceLabel('\n'))); break;
       case '\v': ASSERT(0 == bsl::strcmp("VT", whitespaceLabel('\v'))); break;
       case '\f': ASSERT(0 == bsl::strcmp("FF", whitespaceLabel('\f'))); break;
       case '\r': ASSERT(0 == bsl::strcmp("CR", whitespaceLabel('\r'))); break;
       default  : ASSERT(0 == bsl::strcmp("XX", whitespaceLabel(  ch))); break;
       //-V
        }
    }
}

static bool u_veryVerbose;

static bool isEqual(const SR& a, const SR&b)
    // Return 'true' if the specified 'a' and 'b' have the same 'data' and
    // 'length' attributes and 'false' otherwise.
{
    return a.begin() == b.begin()
        && a.end()   == b.end();
}

static void testIsEqual()
    // Execute the validation test for the 'isEqual' helper function.
{
    struct Local {
        static const char *dataLabel(const char *dataPtr)
            // Return "null pointer" if the specified 'dataPtr' is 0, and
            // "non-null pointer" otherwise.
        {
            return 0 == dataPtr
                   ? "null pointer"
                   : "non-null pointer";
        }
    };

    static const struct {
        int          d_line;
        const char  *d_data_p;
        bsl::size_t  d_length;
    } DATA[] = {
        //LINE DATA                               LENGTH
        //---  ---------------------------------  ------
        { L_,  reinterpret_cast<const char *>(0),      0 }
      , { L_,  reinterpret_cast<const char *>(1),      0 }
   // , { L_,  reinterpret_cast<const char *>(0),      1 } // disallowed pair
      , { L_,  reinterpret_cast<const char *>(1),      1 }
    };
    const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int          LINE1   = DATA[ti].d_line;
        const char        *DATA1   = DATA[ti].d_data_p;
        const bsl::size_t  LENGTH1 = DATA[ti].d_length;

        if (u_veryVerbose) {
            T_
            P_(LINE1)
            P_(Local::dataLabel(DATA1))
            P(LENGTH1)
        }

        const SR mX1(DATA1, DATA1 + LENGTH1); const SR& X1 = mX1;

        LOOP2_ASSERT(ti, LINE1, isEqual(X1, X1))

        for (bsl::size_t tj = 0; tj < NUM_DATA; ++tj) {
            const int          LINE2   = DATA[tj].d_line;
            const char        *DATA2   = DATA[tj].d_data_p;
            const bsl::size_t  LENGTH2 = DATA[tj].d_length;

            if (u_veryVerbose) {
                T_
                P_(LINE2)
                P_(Local::dataLabel(DATA2))
                P(LENGTH2)
            }

            const SR mX2(DATA2, DATA2 + LENGTH2); const SR& X2 = mX2;

            bool expect = ti == tj
                          ? true
                          : false;

            LOOP2_ASSERT(tj, LINE2, expect == isEqual(X1, X2))
        }
    }
}

struct Local {
    // This 'struct' provides a namespace for a helper functions that avoid the
    // disambiguation issues of using 'bsl::tolower' or 'bsl::toupper' as third
    // arguments to 'bsl::transform'.

    static int toLower(const int& ch);
        // Return the result of calling 'bsl::tolower' using the specified
        // 'ch'.

    static int toUpper(const int& ch);
        // Return the result of calling 'bsl::toupper' using the specified
        // 'ch'.
};

int Local::toLower(const int& ch)
{
    return bsl::tolower(ch);
}

int Local::toUpper(const int& ch)
{
    return bsl::toupper(ch);
}

static void testLocalFunctions()
    // Execute the validation test for the 'Local' helper function.
{
    for (int ch = 0; ch <= 255; ++ch) {
        ASSERT(bsl::tolower(ch) == Local::toLower(ch));
        ASSERT(bsl::toupper(ch) == Local::toUpper(ch));
    }
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool             verbose = argc > 2; (void)             verbose;
    bool         veryVerbose = argc > 3; (void)         veryVerbose;
    bool     veryVeryVerbose = argc > 4; (void)     veryVeryVerbose;
    bool veryVeryVeryVerbose = argc > 4; (void) veryVeryVeryVerbose;

    u_veryVerbose = veryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" << "USAGE EXAMPLE" << "\n"
                                  << "=============" << "\n";
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Trimming Whitespace
//- - - - - - - - - - - - - - - -
// Many applications must normalize user input by removing leading and trailing
// whitespace characters to obtain the essential text that is the intended
// input.  Naturally, one would prefer to do this as efficiently as possible.
//
// Suppose the response entered by a user is captured in 'rawInput' below:
//..
    const char * const rawInput    = "    \t\r\n  Hello, world!    \r\n";
                                    //1234 5 6 789             1234 5 6
                                    //            123456789ABCD
                                    // Note lengths of whitespace and
                                    // non-whitespace substrings for later.
//..
// First, for this pedagogical example, we copy the contents at 'rawInput' for
// later reference:
//..
    const bsl::string copyRawInput(rawInput);
//..
// Then, we create a 'bslstl::StringRef' object referring to the raw data.
// Given a single argument of 'const char *', the constructor assumes the data
// is a null-terminated string and implicitly calculates the length for the
// reference:
//..
    bslstl::StringRef text(rawInput);

    ASSERT(rawInput   == text.data());
    ASSERT(9 + 13 + 6 == text.length());
//..
// Now, we invoke the 'bdlb::StringRefUtil::trim' method to find the "Hello,
// world!" sequence in 'rawInput'.
//..
    bslstl::StringRef textOfInterest = bdlb::StringRefUtil::trim(text);
//..
// Finally, we observe the results:
//..
    ASSERT("Hello, world!"       == textOfInterest); // content comparison
    ASSERT(13                    == textOfInterest.length());

    ASSERT(text.data()   + 9     == textOfInterest.data());
    ASSERT(text.length() - 9 - 6 == textOfInterest.length());

    ASSERT(rawInput              == copyRawInput);   // content comparison
//..
// Notice that, as expected, the 'textOfInterest' object refers to the "Hello,
// world!" sub-sequence within the 'rawInput' byte array while the data at
// 'rawInput' remains *unchanged*.

      } break;
    case 5: {
        // --------------------------------------------------------------------
        // SUBSTRING
        //
        // Concerns:
        //: 1 Bad 'position' asserts.
        //: 2 Negative 'numChars' asserts.
        //: 3 Larger-than-length 'numChars' is adjusted.
        //: 4 The proper substring is returned.
        //: 5 Default arguments are provided.
        //: 6 The input string argument is 'const'.
        //: 7 'position' at the end is valid.
        //
        // Plan:
        //: 1 If compiling with exceptions enabled, verify that a 'position'
        //    larger than string length asserts.
        //:
        //: 2 Verify that if 'position + numChars' are larger than the length
        //:   of the string 'numChars' is adjusted.
        //:
        //: 3 Verify that 'position == string.length()' is valid and returns an
        //:   empty string.
        //:
        //: 3 Verify that the proper substring is returned.
        //:
        //: 4 Verify the default arguments.
        //:
        //: 5 Use a 'const bslstl::StringRef' as input.
        //
        // Testing:
        //  substr(string, position = 0, numChars = k_NPOS);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "SUBSTRING\n"
                                     "=========" << endl;

        static const bslstl::StringRef string = "0123456789012345678901234567";

        typedef bdlb::StringRefUtil Util;

#ifdef BDE_BUILD_TARGET_EXC
        {
            using bsls::AssertFailureHandlerGuard;
            using bsls::AssertTest;
            AssertFailureHandlerGuard g(AssertTest::failTestDriver); (void)g;

            ASSERT_SAFE_FAIL(Util::substr(string, string.length() + 1));
        }
#endif

        ASSERT(Util::substr(string, 10, 50) == "012345678901234567");

        ASSERT(Util::substr(string, string.length()) == "");

        for (Util::size_type pos = 0; pos <= string.length(); ++pos) {
            if (veryVerbose) P(pos);
            for (Util::size_type n = 0; n <= string.length(); ++n) {
                bslstl::StringRef result = Util::substr(string, pos, n);
                bslstl::StringRef oracle(string, pos, n);
                if (veryVerbose) {
                    P(n);
                    P(oracle);
                    P(result);
                }
                LOOP2_ASSERT(pos, n, result == oracle);
            }
        }

        ASSERT(Util::substr(string, 20) == "01234567");
        ASSERT(Util::substr(string, 20) ==
                                       Util::substr(string, 20, Util::k_NPOS));

        ASSERT(Util::substr(string) == string);
        ASSERT(Util::substr(string) == Util::substr(string, 0, Util::k_NPOS));

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // FIND SUBSTRING
        //   See the Overview for global concerns that apply to these functions
        //   and the plans for addressing those concerns.
        //
        // Concerns:
        //: 1 The specified sub-string is correctly located in the given string
        //:   if and only if the sub-string is entirely contained in string.
        //:
        //: 2 In the "caseless" methods, characters from 'string' and 'substr'
        //:   are *both* converted to a common case before comparison.
        //:
        //: 3 When a string contains multiple instances of a specified
        //:   substring, the correct occurrence (first/last) of the substring
        //:   is reported.
        //
        // Plan:
        //: 1 Construct a series of input string/sub-string pairs in which the
        //:   sub-string "slides" through the string.  The substring should be
        //:   found only when it entirely overlaps the string.  (C-1)
        //:
        //: 2 Repeat step P-1 for four configurations: (C-2)
        //:   1 Both 'string' and 'substr' are lowercase (as initialized in the
        //:     'DATA' array).
        //:   2 'string' is lowercase and 'substr' is uppercase.
        //:   3 'string' is uppercase and 'substr' is lowercase.
        //:   4 Both 'string' and 'substr' are uppercase.
        //:
        //: 3 Create input data having mixed characters cases in
        //:   both the string and the substring.  Compare the result
        //:   from each of the four search methods with the calculated
        //:   expected value.  (C-2)
        //
        //: 4 Construct a series of input string/sub-string pairs in which
        //:   there are multiple occurrences of the substring.  (C-3)
        //
        // Testing:
        //  strstr         (const bslstl::StringRef& string, subString);
        //  strstrCaseless (const bslstl::StringRef& string, subString);
        //  strrstr        (const bslstl::StringRef& string, subString);
        //  strrstrCaseless(const bslstl::StringRef& string, subString);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "FIND SUBSTRING" "\n"
                                  "==============" "\n";

        const static struct {
            int         d_line;
            const char *d_string_p;
            const char *d_substr_p;
            int         d_position;
            int         d_overlap;
        } DATA[] = {
           //LINE STRING  SUBSTR  POSITION OVERLAP
           //---- ------  ------- -------- -------

           // STRING LENGTH 0

           { L_,  ""    , ""    ,        0,      0 }

         , { L_,  ""    , "a"   ,       -1,      0 }
         , { L_,  ""    , "a"   ,        1,      0 }

           // STRING LENGTH 1

         , { L_,  "-"   , ""    ,        0,      0 }

         , { L_,  "-"   , "a"   ,       -1,      0 }
         , { L_,  "a"   , "a"   ,        0,      1 }
         , { L_,  "-"   , "a"   ,        1,      0 }

         , { L_,  "-"   , "ab"  ,       -2,      0 }
         , { L_,  "b"   , "ab"  ,       -1,      1 }
         , { L_,  "a"   , "ab"  ,        0,      1 }
         , { L_,  "-"   , "ab"  ,        1,      0 }

           // STRING LENGTH 2

         , { L_,  "--"  , ""    ,        0,      0 }

         , { L_,  "--"  , "a"   ,       -1,      0 }
         , { L_,  "a-"  , "a"   ,        0,      1 }
         , { L_,  "-a"  , "a"   ,        1,      1 }
         , { L_,  "--"  , "a"   ,        2,      0 }

         , { L_,  "--"  , "ab"  ,       -2,      0 }
         , { L_,  "b-"  , "ab"  ,       -1,      1 }
         , { L_,  "ab"  , "ab"  ,        0,      2 }
         , { L_,  "-a"  , "ab"  ,        1,      1 }
         , { L_,  "--"  , "ab"  ,        2,      0 }

         , { L_,  "--"  , "abc" ,       -3,      0 }
         , { L_,  "c-"  , "abc" ,       -2,      1 }
         , { L_,  "bc"  , "abc" ,       -1,      2 }
         , { L_,  "ab"  , "abc" ,        0,      2 }
         , { L_,  "-a"  , "abc" ,        1,      1 }
         , { L_,  "--"  , "abc" ,        2,      0 }

           // STRING LENGTH 3

         , { L_,  "---" , ""    ,        0,      0 }

         , { L_,  "---" , "a"   ,       -1,      0 }
         , { L_,  "a--" , "a"   ,        0,      1 }
         , { L_,  "-a-" , "a"   ,        1,      1 }
         , { L_,  "--a" , "a"   ,        2,      1 }
         , { L_,  "---" , "a"   ,        3,      0 }

         , { L_,  "---" , "ab"  ,       -2,      0 }
         , { L_,  "b--" , "ab"  ,       -1,      1 }
         , { L_,  "ab-" , "ab"  ,        0,      2 }
         , { L_,  "-ab" , "ab"  ,        1,      2 }
         , { L_,  "--a" , "ab"  ,        2,      1 }
         , { L_,  "---" , "ab"  ,        3,      0 }

         , { L_,  "---" , "abc" ,       -3,      0 }
         , { L_,  "c--" , "abc" ,       -2,      1 }
         , { L_,  "bc-" , "abc" ,       -1,      2 }
         , { L_,  "abc" , "abc" ,        0,      3 }
         , { L_,  "-ab" , "abc" ,        1,      2 }
         , { L_,  "--a" , "abc" ,        2,      1 }
         , { L_,  "---" , "abc" ,        3,      0 }

         , { L_,  "---" , "abcd",       -4,      0 }
         , { L_,  "d--" , "abcd",       -3,      1 }
         , { L_,  "cd-" , "abcd",       -2,      2 }
         , { L_,  "bcd" , "abcd",       -1,      3 }
         , { L_,  "abc" , "abcd",        0,      3 }
         , { L_,  "-ab" , "abcd",        1,      3 }
         , { L_,  "--a" , "abcd",        2,      1 }
         , { L_,  "---" , "abcd",        3,      0 }

            // STRING LENGTH 4

         , { L_,  "----", ""    ,        0,      0 }

         , { L_,  "----", "a"   ,       -1,      0 }
         , { L_,  "a---", "a"   ,        0,      1 }
         , { L_,  "-a--", "a"   ,        1,      1 }
         , { L_,  "--a-", "a"   ,        2,      1 }
         , { L_,  "---a", "a"   ,        3,      1 }
         , { L_,  "----", "a"   ,        4,      0 }

         , { L_,  "----", "ab"  ,       -2,      0 }
         , { L_,  "b---", "ab"  ,       -1,      1 }
         , { L_,  "ab--", "ab"  ,        0,      2 }
         , { L_,  "-ab-", "ab"  ,        1,      2 }
         , { L_,  "--ab", "ab"  ,        2,      2 }
         , { L_,  "---b", "ab"  ,        3,      1 }
         , { L_,  "----", "ab"  ,        4,      0 }

         , { L_,  "----", "abc"  ,      -3,      0 }
         , { L_,  "c---", "abc"  ,      -2,      1 }
         , { L_,  "ab--", "abc"  ,      -1,      2 }
         , { L_,  "abc-", "abc"  ,       0,      3 }
         , { L_,  "-abc", "abc"  ,       1,      3 }
         , { L_,  "--ab", "abc"  ,       2,      2 }
         , { L_,  "---a", "abc"  ,       3,      1 }
         , { L_,  "----", "abc"  ,       4,      0 }

         , { L_,  "----", "abcd" ,      -4,      0 }
         , { L_,  "d---", "abcd" ,      -3,      1 }
         , { L_,  "cd--", "abcd" ,      -2,      2 }
         , { L_,  "bcd-", "abcd" ,       1,      3 }
         , { L_,  "abcd", "abcd" ,       0,      4 }
         , { L_,  "-abc", "abcd" ,       1,      3 }
         , { L_,  "--ab", "abcd" ,       2,      2 }
         , { L_,  "---a", "abcd" ,       3,      1 }
         , { L_,  "----", "abcd" ,       4,      0 }

         , { L_,  "----", "abcde",      -5,      0 }
         , { L_,  "e---", "abcde",      -4,      1 }
         , { L_,  "de--", "abcde",      -3,      2 }
         , { L_,  "cde-", "abcde",      -2,      3 }
         , { L_,  "bcde", "abcde",      -1,      4 }
         , { L_,  "abcd", "abcde",       0,      4 }
         , { L_,  "-abc", "abcde",       1,      3 }
         , { L_,  "--ab", "abcde",       2,      2 }
         , { L_,  "---a", "abcde",       3,      1 }
         , { L_,  "----", "abcde",       4,      0 }
        };
        const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        if (veryVerbose) cout <<
                       "\n" "Test ASCII strings (including \"caseless\")" "\n";

        for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE     = DATA[ti].d_line;
            const char * const CSTRING  = DATA[ti].d_string_p;
            const char * const CSUBSTR  = DATA[ti].d_substr_p;
            const int          POSITION = DATA[ti].d_position;
            const bsl::size_t  OVERLAP  = DATA[ti].d_overlap;

            if (veryVerbose) {
                T_
                P_(LINE)
                P_(CSTRING)
                P_(CSUBSTR)
                P_(POSITION)
                P(OVERLAP)
            }

            const char * const lettersInData = "abcde";

            bool stringHasLetters  =    bsl::strcspn(CSTRING, lettersInData)
                                    == bsl::strlen (CSTRING)
                                    ?  false
                                    :  true;
            bool substrHasLetters  =    bsl::strcspn(CSUBSTR, lettersInData)
                                    == bsl::strlen (CSUBSTR)
                                    ?  false
                                    :  true;
            bool   bothHaveLetters = stringHasLetters && substrHasLetters;

            if (veryVeryVerbose) {
                P_(CSTRING)
                P_(stringHasLetters)
                P_(CSUBSTR)
                P_(substrHasLetters)
                P(bothHaveLetters)
            }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {

                bsl::string string(CSTRING);
                bsl::string substr(CSUBSTR);
                bsl::size_t lenCSUBSTR = substr.length();

                bool areSameCaseStrSubstr;

                switch (cfg) {
                  case 'a': {
                    areSameCaseStrSubstr = true;
                  } break;
                  case 'b': {
                    bsl::transform(substr.begin(),
                                   substr.end(),
                                   substr.begin(), // output
                                   Local::toUpper);

                    areSameCaseStrSubstr = false;
                  } break;
                  case 'c': {
                    bsl::transform(string.begin(),
                                   string.end(),
                                   string.begin(), // output
                                   Local::toUpper);

                    areSameCaseStrSubstr = false;
                  } break;
                  case 'd': {
                    bsl::transform(string.begin(),
                                   string.end(),
                                   string.begin(), // output
                                   Local::toUpper);

                    bsl::transform(substr.begin(),
                                   substr.end(),
                                   substr.begin(), // output
                                   Local::toUpper);

                    areSameCaseStrSubstr = true;
                  } break;
                  default: {
                      LOOP_ASSERT(cfg, !"Unknown 'cfg'");
                  } break;
                };

                if (veryVeryVerbose) {
                    P_(cfg)
                    P_(string)
                    P_(substr)
                    P(areSameCaseStrSubstr)
                }

     //---------^
     const SR expected          = 0 <= POSITION  && OVERLAP == lenCSUBSTR
                                  ? SR(string.data() + POSITION,
                                       string.data() + POSITION + lenCSUBSTR)
                                  : SR();
     const SR expectedR         = 0 == lenCSUBSTR
                                  ? SR(string.end(), 0)
                                  : 0 <= POSITION  && OVERLAP == lenCSUBSTR
                                  ? SR(string.data() + POSITION,
                                       string.data() + POSITION + lenCSUBSTR)
                                  : SR();
     const SR expectedCasefull  = bothHaveLetters && ! areSameCaseStrSubstr
                                  ? SR()
                                  : expected;
     const SR expectedCaseless  = expected;
     const SR expectedCasefullR = bothHaveLetters && ! areSameCaseStrSubstr
                                  ? SR()
                                  : expectedR;
     const SR expectedCaselessR = expectedR;

     const SR resultCasefull   = Util::strstr         (SR(string), SR(substr));
     const SR resultCasefullR  = Util::strrstr        (SR(string), SR(substr));
     const SR resultCaseless   = Util::strstrCaseless (SR(string), SR(substr));
     const SR resultCaselessR  = Util::strrstrCaseless(SR(string), SR(substr));

     LOOP2_ASSERT(LINE, cfg, isEqual(expectedCasefull,  resultCasefull));
     LOOP2_ASSERT(LINE, cfg, isEqual(expectedCaseless,  resultCaseless));
     LOOP2_ASSERT(LINE, cfg, isEqual(expectedCasefullR, resultCasefullR));
     LOOP2_ASSERT(LINE, cfg, isEqual(expectedCaselessR, resultCaselessR));
     //---------V
            }
        }

        if (veryVerbose) cout << "\n" "Test non-ASCII strings" "\n";
        for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE     = DATA[ti].d_line;
            const char * const CSTRING  = DATA[ti].d_string_p;
            const char * const CSUBSTR  = DATA[ti].d_substr_p;
            const int          POSITION = DATA[ti].d_position;
            const bsl::size_t  OVERLAP  = DATA[ti].d_overlap;

            if (veryVerbose) {
                T_
                P_(LINE)
                P_(CSTRING)
                P_(CSUBSTR)
                P_(POSITION)
                P(OVERLAP)
            }

            bsl::string string(CSTRING);
            bsl::string substr(CSUBSTR);
            bsl::size_t lenCSUBSTR = substr.length();

            struct Local {
                static unsigned char setMsb(const char& ch)
                    // Return the value of the specified 'ch' with the most
                    // significant bit (msb) set.  Note that if the msb is set
                    // on entry, the given value is returned unchanged.
                {
                    return static_cast<unsigned char>(ch) | 0x80;
                }
            };

            bsl::transform(string.begin(),
                           string.end(),
                           string.begin(),   // output
                           Local::setMsb);

            bsl::transform(substr.begin(),
                           substr.end(),
                           substr.begin(),   // output
                           Local::setMsb);

            const SR expected  = 0 <= POSITION && OVERLAP == lenCSUBSTR
                                 ? SR(string.data() + POSITION,
                                      string.data() + POSITION + lenCSUBSTR)
                                 : SR();

            const SR expectedR = 0 == lenCSUBSTR
                                 ? SR(string.end(), 0)
                                 : 0 <= POSITION && OVERLAP == lenCSUBSTR
                                 ? SR(string.data() + POSITION,
                                      string.data() + POSITION + lenCSUBSTR)
                                 : SR();

            const SR SR_STRING(string);
            const SR SR_SUBSTR(substr);

            const SR resultCasefull  = Util::strstr         (SR_STRING,
                                                             SR_SUBSTR);
            const SR resultCaseless  = Util::strstrCaseless (SR_STRING,
                                                             SR_SUBSTR);
            const SR resultCasefullR = Util::strrstr        (SR_STRING,
                                                             SR_SUBSTR);
            const SR resultCaselessR = Util::strrstrCaseless(SR_STRING,
                                                             SR_SUBSTR);

            LOOP_ASSERT(LINE, isEqual(expected,  resultCasefull ));
            LOOP_ASSERT(LINE, isEqual(expected,  resultCaseless ));
            LOOP_ASSERT(LINE, isEqual(expectedR, resultCasefullR));
            LOOP_ASSERT(LINE, isEqual(expectedR, resultCaselessR));
        }

        if (verbose) cout << "\n" "Check strings containing mixed cases" "\n";
        {
            static const struct {
                const int   d_lineNumber;
                const char *d_string_p;
                const int   d_stringLen;
                const char *d_subString_p;
                const int   d_subStringLen;

                const int   d_result;
                const int   d_resultCaseless;
                const int   d_resultReverse;
                const int   d_resultReverseCaseless;
                    // Index of substring in string or -1 if not found.

            } DATA_MIXEDCASE[] = {
                // In the table below, R1-4 correspond to the results from the
                // the four methods under test.  The mapping is:
                //: 1 Results from 'strstr',
                //: 2 Results from 'strstrCaseless',
                //: 3 Results from 'strrstr',
                //: 4 Results from 'strrstrCaseless'.

                //LI  STRING       SUBSTRING    R1  R2  R3  R4
                //--  ----------   ---------    --  --  --  --

                //Substring length 0
                { L_, "A"   ,  1,  ""   ,  0,    0,  0,  1,  1 }
              , { L_, "Ab"  ,  2,  ""   ,  0,    0,  0,  2,  2 }
              , { L_, "aB"  ,  2,  ""   ,  0,    0,  0,  2,  2 }
              , { L_, "AB"  ,  2,  ""   ,  0,    0,  0,  2,  2 }
              , { L_, "Abc" ,  3,  ""   ,  0,    0,  0,  3,  3 }
              , { L_, "aBc" ,  3,  ""   ,  0,    0,  0,  3,  3 }
              , { L_, "abC" ,  3,  ""   ,  0,    0,  0,  3,  3 }
              , { L_, "ABc" ,  3,  ""   ,  0,    0,  0,  3,  3 }
              , { L_, "aBC" ,  3,  ""   ,  0,    0,  0,  3,  3 }
              , { L_, "AbC" ,  3,  ""   ,  0,    0,  0,  3,  3 }
              , { L_, "ABC" ,  3,  ""   ,  0,    0,  0,  3,  3 }

                //Substring length 1
              , { L_, "A"   ,  1,  "a"  ,  1,   -1,  0, -1,  0 }
              , { L_, "Aa"  ,  2,  "a"  ,  1,    1,  0,  1,  1 }
              , { L_, "aA"  ,  2,  "a"  ,  1,    0,  0,  0,  1 }
              , { L_, "AA"  ,  2,  "a"  ,  1,   -1,  0, -1,  1 }
              , { L_, "bA"  ,  2,  "a"  ,  1,   -1,  1, -1,  1 }
              , { L_, "Aaa" ,  3,  "a"  ,  1,    1,  0,  2,  2 }
              , { L_, "aAa" ,  3,  "a"  ,  1,    0,  0,  2,  2 }
              , { L_, "aaA" ,  3,  "a"  ,  1,    0,  0,  1,  2 }
              , { L_, "AAa" ,  3,  "a"  ,  1,    2,  0,  2,  2 }
              , { L_, "aAA" ,  3,  "a"  ,  1,    0,  0,  0,  2 }
              , { L_, "AaA" ,  3,  "a"  ,  1,    1,  0,  1,  2 }
              , { L_, "AAA" ,  3,  "a"  ,  1,   -1,  0, -1,  2 }
              , { L_, "Aab" ,  3,  "a"  ,  1,    1,  0,  1,  1 }
              , { L_, "aAb" ,  3,  "a"  ,  1,    0,  0,  0,  1 }
              , { L_, "AAb" ,  3,  "a"  ,  1,   -1,  0, -1,  1 }
              , { L_, "Aba" ,  3,  "a"  ,  1,    2,  0,  2,  2 }
              , { L_, "abA" ,  3,  "a"  ,  1,    0,  0,  0,  2 }
              , { L_, "AbA" ,  3,  "a"  ,  1,   -1,  0, -1,  2 }
              , { L_, "bAa" ,  3,  "a"  ,  1,    2,  1,  2,  2 }
              , { L_, "baA" ,  3,  "a"  ,  1,    1,  1,  1,  2 }
              , { L_, "bAA" ,  3,  "a"  ,  1,   -1,  1, -1,  2 }
              , { L_, "bbA" ,  3,  "a"  ,  1,   -1,  2, -1,  2 }
              , { L_, "A"   ,  1,  "A"  ,  1,    0,  0,  0,  0 }
              , { L_, "Aa"  ,  2,  "A"  ,  1,    0,  0,  0,  1 }
              , { L_, "aA"  ,  2,  "A"  ,  1,    1,  0,  1,  1 }
              , { L_, "AA"  ,  2,  "A"  ,  1,    0,  0,  1,  1 }
              , { L_, "bA"  ,  2,  "A"  ,  1,    1,  1,  1,  1 }
              , { L_, "Aaa" ,  3,  "A"  ,  1,    0,  0,  0,  2 }
              , { L_, "aAa" ,  3,  "A"  ,  1,    1,  0,  1,  2 }
              , { L_, "aaA" ,  3,  "A"  ,  1,    2,  0,  2,  2 }
              , { L_, "AAa" ,  3,  "A"  ,  1,    0,  0,  1,  2 }
              , { L_, "aAA" ,  3,  "A"  ,  1,    1,  0,  2,  2 }
              , { L_, "AaA" ,  3,  "A"  ,  1,    0,  0,  2,  2 }
              , { L_, "AAA" ,  3,  "A"  ,  1,    0,  0,  2,  2 }
              , { L_, "Aab" ,  3,  "A"  ,  1,    0,  0,  0,  1 }
              , { L_, "aAb" ,  3,  "A"  ,  1,    1,  0,  1,  1 }
              , { L_, "AAb" ,  3,  "A"  ,  1,    0,  0,  1,  1 }
              , { L_, "Aba" ,  3,  "A"  ,  1,    0,  0,  0,  2 }
              , { L_, "abA" ,  3,  "A"  ,  1,    2,  0,  2,  2 }
              , { L_, "AbA" ,  3,  "A"  ,  1,    0,  0,  2,  2 }
              , { L_, "bAa" ,  3,  "A"  ,  1,    1,  1,  1,  2 }
              , { L_, "baA" ,  3,  "A"  ,  1,    2,  1,  2,  2 }
              , { L_, "bAA" ,  3,  "A"  ,  1,    1,  1,  2,  2 }
              , { L_, "bbA" ,  3,  "A"  ,  1,    2,  2,  2,  2 }

                //Substring length 2
              , { L_, "A"   ,  1,  "ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "Aa"  ,  2,  "ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "aA"  ,  2,  "ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "AA"  ,  2,  "ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "bA"  ,  2,  "ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "Aaa" ,  3,  "ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "aAa" ,  3,  "ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "aaA" ,  3,  "ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "AAa" ,  3,  "ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "aAA" ,  3,  "ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "AaA" ,  3,  "ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "AAA" ,  3,  "ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "Aab" ,  3,  "ab" ,  2,    1,  1,  1,  1 }
              , { L_, "aAb" ,  3,  "ab" ,  2,   -1,  1, -1,  1 }
              , { L_, "AAb" ,  3,  "ab" ,  2,   -1,  1, -1,  1 }
              , { L_, "Aba" ,  3,  "ab" ,  2,   -1,  0, -1,  0 }
              , { L_, "abA" ,  3,  "ab" ,  2,    0,  0,  0,  0 }
              , { L_, "AbA" ,  3,  "ab" ,  2,   -1,  0, -1,  0 }
              , { L_, "bAa" ,  3,  "ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "baA" ,  3,  "ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "bAA" ,  3,  "ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "bbA" ,  3,  "ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "Aaab",  4,  "ab" ,  2,    2,  2,  2,  2 }
              , { L_, "aAab",  4,  "ab" ,  2,    2,  2,  2,  2 }
              , { L_, "aaAb",  4,  "ab" ,  2,   -1,  2, -1,  2 }
              , { L_, "Aaba",  4,  "ab" ,  2,    1,  1,  1,  1 }
              , { L_, "aAba",  4,  "ab" ,  2,   -1,  1, -1,  1 }
              , { L_, "aabA",  4,  "ab" ,  2,    1,  1,  1,  1 }
              , { L_, "abAb",  4,  "ab" ,  2,    0,  0,  0,  2 }
              , { L_, "AbAb",  4,  "ab" ,  2,   -1,  0, -1,  2 }
              , { L_, "A"   ,  1,  "Ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "Aa"  ,  2,  "Ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "aA"  ,  2,  "Ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "AA"  ,  2,  "Ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "bA"  ,  2,  "Ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "Ab"  ,  2,  "Ab" ,  2,    0,  0,  0,  0 }
              , { L_, "Aaa" ,  3,  "Ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "aAa" ,  3,  "Ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "aaA" ,  3,  "Ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "AAa" ,  3,  "Ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "AAA" ,  3,  "Ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "Aab" ,  3,  "Ab" ,  2,   -1,  1, -1,  1 }
              , { L_, "aAb" ,  3,  "Ab" ,  2,    1,  1,  1,  1 }
              , { L_, "AAb" ,  3,  "Ab" ,  2,    1,  1,  1,  1 }
              , { L_, "Aba" ,  3,  "Ab" ,  2,    0,  0,  0,  0 }
              , { L_, "abA" ,  3,  "Ab" ,  2,   -1,  0, -1,  0 }
              , { L_, "AbA" ,  3,  "Ab" ,  2,    0,  0,  0,  0 }
              , { L_, "bAa" ,  3,  "Ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "baA" ,  3,  "Ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "bAA" ,  3,  "Ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "bbA" ,  3,  "Ab" ,  2,   -1, -1, -1, -1 }
              , { L_, "Aaab",  4,  "Ab" ,  2,   -1,  2, -1,  2 }
              , { L_, "aAab",  4,  "Ab" ,  2,   -1,  2, -1,  2 }
              , { L_, "aaAb",  4,  "Ab" ,  2,    2,  2,  2,  2 }
              , { L_, "Aaba",  4,  "Ab" ,  2,   -1,  1, -1,  1 }
              , { L_, "aAba",  4,  "Ab" ,  2,    1,  1,  1,  1 }
              , { L_, "aabA",  4,  "Ab" ,  2,   -1,  1, -1,  1 }
              , { L_, "Abab",  4,  "Ab" ,  2,    0,  0,  0,  2 }
              , { L_, "abAb",  4,  "Ab" ,  2,    2,  0,  2,  2 }
              , { L_, "AbAb",  4,  "Ab" ,  2,    0,  0,  2,  2 }

                //String embedded nulls
              , { L_, "\0  ",  3,  "a"  ,  1,   -1, -1, -1, -1 }
              , { L_, "\0Aa",  3,  "a"  ,  1,    2,  1,  2,  2 }
              , { L_, "\0Ab",  3,  "a"  ,  1,   -1,  1, -1,  1 }
              , { L_, "\0Ba",  3,  "a"  ,  1,    2,  2,  2,  2 }
              , { L_, "\0bb",  3,  "a"  ,  1,   -1, -1, -1, -1 }
              , { L_, "  \0",  3,  "a"  ,  1,   -1, -1, -1, -1 }
              , { L_, "aa\0",  3,  "a"  ,  1,    0,  0,  1,  1 }
              , { L_, "Ab\0",  3,  "a"  ,  1,   -1,  0, -1,  0 }
              , { L_, "bA\0",  3,  "a"  ,  1,   -1,  1, -1,  1 }
              , { L_, "bB\0",  3,  "a"  ,  1,   -1, -1, -1, -1 }
              , { L_, "A\0a",  3,  "a"  ,  1,    2,  0,  2,  2 }
              , { L_, "a\0b",  3,  "a"  ,  1,    0,  0,  0,  0 }
              , { L_, "B\0a",  3,  "a"  ,  1,    2,  2,  2,  2 }
              , { L_, "B\0b",  3,  "a"  ,  1,   -1, -1, -1, -1 }
            };
            const bsl::size_t NUM_DATA_MIXEDCASE = sizeof  DATA_MIXEDCASE
                                                 / sizeof *DATA_MIXEDCASE;

            struct Local {
                static const char *print(const char *string)
                    // Return "null pointer" if the specified 'string' is 0,
                    // and 'string' otherwise.
                {
                    return 0 == string
                           ? "null pointer"
                           : string;
                }
            };

            for (bsl::size_t ti = 0; ti < NUM_DATA_MIXEDCASE; ++ti) {

                if (veryVerbose) { T_ P(ti) }

                const int   LINE    = DATA_MIXEDCASE[ti].d_lineNumber;
                const char *CSTR    = DATA_MIXEDCASE[ti].d_string_p;
                const int   CSTRLEN = DATA_MIXEDCASE[ti].d_stringLen;
                const char *CSUB    = DATA_MIXEDCASE[ti].d_subString_p;
                const int   CSUBLEN = DATA_MIXEDCASE[ti].d_subStringLen;

                const int   RES          = DATA_MIXEDCASE[ti].d_result;
                const int   RES_CASELESS = DATA_MIXEDCASE[ti].d_resultCaseless;
                const int   RES_REVERSE  = DATA_MIXEDCASE[ti].d_resultReverse;
                const int   RES_REVERSECASELESS
                                         = DATA_MIXEDCASE[ti].
                                                       d_resultReverseCaseless;

                if (veryVeryVerbose) {
                    T_ T_ P(Local::print(CSTR))
                    T_ T_ P(CSTRLEN)
                    T_ T_ P(Local::print(CSUB))
                    T_ T_ P(CSUBLEN)
                    T_ T_ P(RES)
                    T_ T_ P(RES_CASELESS)
                    T_ T_ P(RES_REVERSE)
                    T_ T_ P(RES_REVERSECASELESS)
                }

    //----------^
    const SR expectedCasefull   = -1 == RES
                                  ? SR()
                                  : SR(CSTR + RES,                CSUBLEN);

    const SR expectedCaseless  = -1 == RES_CASELESS
                                 ? SR()
                                 : SR(CSTR + RES_CASELESS,        CSUBLEN);

    const SR expectedCasefullR = -1 == RES_REVERSE
                                 ? SR()
                                 : SR(CSTR + RES_REVERSE,         CSUBLEN);

    const SR expectedCaselessR = -1 == RES_REVERSECASELESS
                                 ? SR()
                                 : SR(CSTR + RES_REVERSECASELESS, CSUBLEN);

    const SR SR_STR(CSTR, CSTRLEN);
    const SR SR_SUB(CSUB, CSUBLEN);

    const SR resultCasefull   = Util::strstr         (SR_STR, SR_SUB);
    const SR resultCasefullR  = Util::strrstr        (SR_STR, SR_SUB);
    const SR resultCaseless   = Util::strstrCaseless (SR_STR, SR_SUB);
    const SR resultCaselessR  = Util::strrstrCaseless(SR_STR, SR_SUB);

    LOOP_ASSERT(LINE, isEqual(expectedCasefull, resultCasefull));
    LOOP_ASSERT(LINE, isEqual(expectedCaseless, resultCaseless));
    LOOP_ASSERT(LINE, isEqual(expectedCasefullR, resultCasefullR));
    LOOP_ASSERT(LINE, isEqual(expectedCaselessR, resultCaselessR));
    //----------v
            }
        }

        if (verbose) cout << "\n" "Check default contructed input"  << "\n";
        {
           const char    *empty = "";   const SR     EMPTY(   empty);
           const char *nonEmpty = "a";  const SR NON_EMPTY(nonEmpty);

           ASSERT(1 == bsl::strlen(nonEmpty));

           const SR DEFAULT;

 //--------^
 ASSERT(isEqual(SR(   empty, 0), Util::strstr         (    EMPTY,     EMPTY)));
 ASSERT(isEqual(SR(   empty, 0), Util::strstr         (    EMPTY,   DEFAULT)));
 ASSERT(isEqual(SR(       0, 0), Util::strstr         (  DEFAULT,     EMPTY)));
 ASSERT(isEqual(SR(       0, 0), Util::strstr         (  DEFAULT,   DEFAULT)));

 ASSERT(isEqual(SR(nonEmpty, 1), Util::strstr         (NON_EMPTY, NON_EMPTY)));
 ASSERT(isEqual(SR(nonEmpty, 0), Util::strstr         (NON_EMPTY,   DEFAULT)));
 ASSERT(isEqual(SR(       0, 0), Util::strstr         (  DEFAULT, NON_EMPTY)));
 ASSERT(isEqual(SR(       0, 0), Util::strstr         (  DEFAULT,   DEFAULT)));

 ASSERT(isEqual(SR(   empty, 0), Util::strrstr        (    EMPTY,     EMPTY)));
 ASSERT(isEqual(SR(   empty, 0), Util::strrstr        (    EMPTY,   DEFAULT)));
 ASSERT(isEqual(SR(       0, 0), Util::strrstr        (  DEFAULT,     EMPTY)));
 ASSERT(isEqual(SR(       0, 0), Util::strrstr        (  DEFAULT,   DEFAULT)));

 ASSERT(isEqual(SR(nonEmpty, 1), Util::strrstr        (NON_EMPTY, NON_EMPTY)));
 ASSERT(isEqual(SR(nonEmpty + 1,
                   0),           Util::strrstr        (NON_EMPTY,   DEFAULT)));
 ASSERT(isEqual(SR(       0, 0), Util::strrstr        (  DEFAULT, NON_EMPTY)));
 ASSERT(isEqual(SR(       0, 0), Util::strrstr        (  DEFAULT,   DEFAULT)));

 ASSERT(isEqual(SR(   empty, 0), Util::strstrCaseless (    EMPTY,     EMPTY)));
 ASSERT(isEqual(SR(   empty, 0), Util::strstrCaseless (    EMPTY,   DEFAULT)));
 ASSERT(isEqual(SR(       0, 0), Util::strstrCaseless (  DEFAULT,     EMPTY)));
 ASSERT(isEqual(SR(       0, 0), Util::strstrCaseless (  DEFAULT,   DEFAULT)));

 ASSERT(isEqual(SR(nonEmpty, 1), Util::strstrCaseless (NON_EMPTY, NON_EMPTY)));
 ASSERT(isEqual(SR(nonEmpty, 0), Util::strstrCaseless (NON_EMPTY,   DEFAULT)));
 ASSERT(isEqual(SR(       0, 0), Util::strstrCaseless (  DEFAULT, NON_EMPTY)));
 ASSERT(isEqual(SR(       0, 0), Util::strstrCaseless (  DEFAULT,   DEFAULT)));

 ASSERT(isEqual(SR(   empty, 0), Util::strrstrCaseless(    EMPTY,     EMPTY)));
 ASSERT(isEqual(SR(   empty, 0), Util::strrstrCaseless(    EMPTY,   DEFAULT)));
 ASSERT(isEqual(SR(       0, 0), Util::strrstrCaseless(  DEFAULT,     EMPTY)));
 ASSERT(isEqual(SR(       0, 0), Util::strrstrCaseless(  DEFAULT,   DEFAULT)));

 ASSERT(isEqual(SR(nonEmpty, 1), Util::strrstrCaseless(NON_EMPTY, NON_EMPTY)));
 ASSERT(isEqual(SR(nonEmpty + 1,
                   0)          , Util::strrstrCaseless(NON_EMPTY,   DEFAULT)));
 ASSERT(isEqual(SR(       0, 0), Util::strrstrCaseless(  DEFAULT, NON_EMPTY)));
 ASSERT(isEqual(SR(       0, 0), Util::strrstrCaseless(  DEFAULT,   DEFAULT)));
 //--------V
        }

        if (verbose) cout << "\n" "Multiple matching substrings"  << "\n";
        {
            const struct {
                int         d_line;
                const char *d_string_p;
                const char *d_substr_p;
                int         d_pos;
                int         d_posr;
            } DATA[] = {
                //LINE STRING        SUBSTR POS POSR
                //---- ------        ------ --- ----
                { L_,  "a"          , "a" ,   0,   0 }
              , { L_,  "-a"         , "a" ,   1,   1 }
              , { L_,  "a-"         , "a" ,   0,   0 }
              , { L_,  "-a-"        , "a" ,   1,   1 }

              , { L_,  "aa"         , "a" ,   0,   1 }
              , { L_,  "-aa"        , "a" ,   1,   2 }
              , { L_,  "a-a"        , "a" ,   0,   2 }
              , { L_,  "aa-"        , "a" ,   0,   1 }

              , { L_,  "abab"       , "ab",   0,   2 }
              , { L_,  "-abab"      , "ab",   1,   3 }
              , { L_,  "a-bab"      , "ab",   3,   3 }
              , { L_,  "ab-ab"      , "ab",   0,   3 }
              , { L_,  "aba-b"      , "ab",   0,   0 }
              , { L_,  "abab-"      , "ab",   0,   2 }

              , { L_,  "abab"       , "ab",   0,   2 }
              , { L_,  "ab--ab"     , "ab",   0,   4 }
              , { L_,  "--abab--"   , "ab",   2,   4 }

              , { L_,  "ababab"     , "ab",   0,   4 }
              , { L_,  "-ab-ab-ab-" , "ab",   1,   7 }
            };
            const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE    = DATA[ti].d_line;
                const char *CSTRING = DATA[ti].d_string_p;
                const char *CSUBSTR = DATA[ti].d_substr_p;
                const int   POS     = DATA[ti].d_pos;
                const int   POSR    = DATA[ti].d_posr;

                if (veryVerbose) {
                    P_(LINE)
                    P_(CSTRING)
                    P_(CSUBSTR)
                    P_(POS)
                     P(POSR)
                }

                const bsl::size_t lenCSUBSTR = bsl::strlen(CSUBSTR);

                const SR expected (CSTRING+POS,  CSTRING+POS +lenCSUBSTR);
                const SR expectedR(CSTRING+POSR, CSTRING+POSR+lenCSUBSTR);

                const SR result  = Util::strstr (SR(CSTRING), SR(CSUBSTR));
                const SR resultR = Util::strrstr(SR(CSTRING), SR(CSUBSTR));

                LOOP_ASSERT(LINE, isEqual(expected , result ));
                LOOP_ASSERT(LINE, isEqual(expectedR, resultR));
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TRIM
        //   See the "Overview" for global concerns that apply to these
        //   functions and the plans for addressing those concerns.
        //
        // Concerns:
        //: 1 The implementation defines the same whitespace character set as
        //:   'bsl::isspace'.
        //:
        //: 2 The '*trim' functions exclude exterior whitespace characters and
        //:   ignore interior whitespace characters.
        //
        // Plan:
        //: 1 Assemble a set of whitespace characters by running 'bsl::isspace'
        //:   on all character possible values, '[0 .. 255]'.  Use this
        //:   computed character set in the generation of input values.  (C-1)
        //:
        //: 2 Define input sequences having interior whitespace sequences,
        //:   and not.  (C-2)
        //
        // Testing:
        //   ltrim(const bslstl::StringRef& string);
        //   rtrim(const bslstl::StringRef& string);
        //    trim(const bslstl::StringRef& string);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "TRIM" "\n"
                                  "====" "\n";

        if (veryVerbose) { cout << "\n" "generate whitespace char set" "\n"; }

        bsl::vector<unsigned char> whiteSpaceChars;

        for (int ch = 0; ch <= 255; ++ch) {
            const unsigned char usch = static_cast<unsigned char>(ch);
            if (bsl::isspace(usch) ) {
                whiteSpaceChars.push_back(usch);
            }
        }
        const unsigned char * const WHITESPACE_CHARS = whiteSpaceChars.data();
        const bsl::size_t       NUM_WHITESPACE_CHARS = whiteSpaceChars.size();
        const int                   PAD_LIMIT        = 3;

        const char * const CSTRINGS[] = { ""
                                        , "a"
                                        , "ab"
                                        , "abc"
                                        , "abcd"
                                        , "a b"
                                        , "a cd"
                                        , "a\tb"
                                        , "a\nb"
                                        , "a\vb"
                                        , "a\fb"
                                        , "a\rb"
                                        , "ab cd"
                                        , "ab\tcd"
                                        , "ab\ncd"
                                        , "ab\vcd"
                                        , "ab\fcd"
                                        , "ab\rcd"
                                        , "ab cd"
                                        , "abc def"
                                        , "abc\tdef"
                                        , "abc\ndef"
                                        , "abc\vdef"
                                        , "abc\fdef"
                                        , "abc\rdef"
                                        , "a"
                                        , "a b"
                                        , "a b c"
                                        , "a b c d"
                                        , "a"
                                        , "a  b"
                                        , "a  b  c"
                                        , "a  b  c  d"
                                        , "a"
                                        , "a   b"
                                        , "a   b   c"
                                        , "a   b   c   d"
                                        };
        const bsl::size_t  NUM_CSTRINGS = sizeof CSTRINGS / sizeof *CSTRINGS;

        const bsl::string  LEFT_SENTINEL("[");
        const bsl::string RIGHT_SENTINEL("]");

        if (veryVeryVerbose) {
            P_(NUM_WHITESPACE_CHARS)
            P_(PAD_LIMIT)
            P_(NUM_CSTRINGS)
            P_( LEFT_SENTINEL)
             P(RIGHT_SENTINEL)
        }

        if (veryVerbose) { cout << "\n" << "main loop" << "\n"; }

        for (bsl::size_t i = 0; i < NUM_WHITESPACE_CHARS; ++i) {
            const char PAD_CHAR = WHITESPACE_CHARS[i];

            if (veryVerbose) {
                T_ P_(i)
                cout << hex
                     << "PAD_CHAR:" << " "
                     <<  PAD_CHAR   << " "
                     << dec;
                P(whitespaceLabel(PAD_CHAR))
            }

            for (bsl::size_t j = 0; j < NUM_CSTRINGS; ++j) {
                const char * const CSTRING = CSTRINGS[j];

                if (veryVerbose) { T_ T_ P_(j) P(CSTRING) }

                const bsl::string TEXT(CSTRING);

                for (int lenPrefix = 0; lenPrefix <= PAD_LIMIT; ++lenPrefix ) {
                    if (veryVerbose) { T_ T_ T_ P(lenPrefix) }

                    const bsl::string PREFIX(lenPrefix, PAD_CHAR);

                    for (int lenSuffix  = 0;
                             lenSuffix <= PAD_LIMIT;
                             ++lenSuffix ) {

                        if (veryVerbose) { T_ T_ T_ T_ P(lenSuffix) }

                        const bsl::string SUFFIX(lenSuffix, PAD_CHAR);

                        const bsl::string INPUT = LEFT_SENTINEL
                                                + PREFIX
                                                + TEXT
                                                + SUFFIX
                                                + RIGHT_SENTINEL;

                        if (veryVeryVerbose) {
                            cout         << "INPUT" << ": "
                                 <<  "|" <<  INPUT  << "|" << endl;
                        }

                        ASSERT( LEFT_SENTINEL.length()
                             + RIGHT_SENTINEL.length() <= INPUT.length());

                        bsl::size_t  adjustedLength = INPUT.length()
                                                    -   LEFT_SENTINEL.length()
                                                    -  RIGHT_SENTINEL.length();
                        const char  *content        = INPUT.data()
                                                    + LEFT_SENTINEL.length();

                              SR  mX(content, content + adjustedLength);
                        const SR&  X = mX;

                        const SR expLTrim = "" == TEXT
                                          ? SR(X.data() + PREFIX.length()
                                                        + SUFFIX.length(), 0)
                                          : SR(X.data() + PREFIX.length(),
                                               X.data() + PREFIX.length()
                                                        +   TEXT.length()
                                                        + SUFFIX.length());

                        const SR expRTrim = "" == TEXT
                                          ? SR(X.data(), 0)
                                          : SR(X.data(),
                                               X.data() + PREFIX.length()
                                                        +   TEXT.length());

                        const SR expTTrim  = "" == TEXT
                                           ?  SR(X.data(), 0)
                                           :  SR(X.data() + PREFIX.length(),
                                                 X.data() + PREFIX.length()
                                                          +   TEXT.length());

                        SR calcLTrim = Util::ltrim(X);
                        SR calcRTrim = Util::rtrim(X);
                        SR calcTTrim = Util:: trim(X);

                        ASSERT(isEqual(expLTrim, calcLTrim));
                        ASSERT(isEqual(expRTrim, calcRTrim));
                        ASSERT(isEqual(expTTrim, calcTTrim));
                    }
                }
            }
        }

        if (veryVerbose) { cout << "ltrim: whitespace char set" << endl; }
        {
            for (bsl::size_t  i = 0; i  < NUM_WHITESPACE_CHARS; ++i) {
                const char PAD_CHAR =         WHITESPACE_CHARS[i];

                if (veryVeryVerbose) { T_
                                       cout << hex
                                            << "PAD_CHAR:" << " "
                                            <<  PAD_CHAR   << " "
                                            << dec;
                                       P(whitespaceLabel(PAD_CHAR)) }

                for (int ch = 0; ch < 256; ++ch) {

                    if (veryVeryVerbose) { T_ T_
                                           cout << hex
                                                << "PAD_CHAR:" << " "
                                                <<  PAD_CHAR   << " "
                                                << dec;
                                           P(ch) }

                    bsl::string mX;   const bsl::string& X = mX;
                    mX.append(1, PAD_CHAR);
                    mX.append(1, ch);
                    ASSERT(2 == X.length());

                          SR actualL = Util::ltrim(X);
                    const SR expectL = bsl::isspace(ch)
                                       ? SR(X.data() + 2, 0)
                                       : SR(X.data() + 1, 1);
                    LOOP_ASSERT(ch, isEqual(expectL, actualL));
                }
            }
        }

        if (veryVerbose) { cout << "rtrim: whitespace char set" << endl; }
        {
            for (bsl::size_t  i = 0; i  < NUM_WHITESPACE_CHARS; ++i) {
                const char PAD_CHAR =         WHITESPACE_CHARS[i];

                if (veryVeryVerbose) { T_
                                       cout << hex
                                            << "PAD_CHAR:" << " "
                                            <<  PAD_CHAR   << " "
                                            << dec;
                                       P(whitespaceLabel(PAD_CHAR)) }

                for (int ch = 0; ch < 256; ++ch) {

                    if (veryVeryVerbose) { T_ T_
                                           cout << hex
                                                << "PAD_CHAR:" << " "
                                                <<  PAD_CHAR   << " "
                                                << dec;
                                                P(ch) }

                    bsl::string mY;   const bsl::string& Y = mY;
                    mY.append(1, ch);
                    mY.append(1, PAD_CHAR);
                    ASSERT(2 == Y.length());

                          SR actualR = Util::rtrim(Y);
                    const SR expectR = bsl::isspace(ch)
                                       ? SR(Y.data() + 0, 0)
                                       : SR(Y.data() + 0, 1);
                    LOOP_ASSERT(ch, isEqual(expectR, actualR));
                }
            }
        }

        if (veryVerbose) { cout << "trim: whitespace char set" << endl; }
        {
            for (bsl::size_t  i = 0;  i  < NUM_WHITESPACE_CHARS; ++i) {
                const char LPAD_CHAR =         WHITESPACE_CHARS[i];
            for (bsl::size_t  j = 0;  j  < NUM_WHITESPACE_CHARS; ++j) {
                const char RPAD_CHAR =         WHITESPACE_CHARS[j];

                if (veryVeryVerbose) { T_
                                       cout << hex
                                            << "LPAD_CHAR:"  << " "
                                            <<  LPAD_CHAR    << " "
                                            << "RPAD_CHAR:"  << " "
                                            <<  RPAD_CHAR    << " "
                                            << dec;
                                            P_(whitespaceLabel(LPAD_CHAR))
                                             P(whitespaceLabel(RPAD_CHAR)) }

                for (int ch = 0; ch < 256; ++ch) {

                    if (veryVeryVerbose) { T_ T_
                                           cout << hex
                                                << "LPAD_CHAR:" << " "
                                                <<  LPAD_CHAR   << " "
                                                << "RPAD_CHAR:" << " "
                                                <<  RPAD_CHAR   << " "
                                                << dec;
                                          P(ch) }

                    bsl::string mZ;   const bsl::string& Z = mZ;
                    mZ.append(1, LPAD_CHAR);
                    mZ.append(1, ch       );
                    mZ.append(1, RPAD_CHAR);
                    ASSERT(3 == Z.length());

                          SR   calcB = Util::trim(Z);
                    const SR expectB = bsl::isspace(ch)
                                       ? SR(Z.data() + 0, 0)
                                       : SR(Z.data() + 1, 1);
                    LOOP_ASSERT(ch, isEqual(expectB, calcB));
                }
            }
            }
        }

        if (veryVerbose) { cout << "\n" "default constructed input" "\n"; }
        {
            SR calcL = Util::ltrim(SR()); ASSERT(isEqual(SR(), calcL));
            SR calcR = Util::rtrim(SR()); ASSERT(isEqual(SR(), calcR));
            SR calcB = Util:: trim(SR()); ASSERT(isEqual(SR(), calcB));
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // COMPARISONS
        //   See the Overview for global concerns that apply to these functions
        //   and the plans for addressing those concerns.
        //
        // Concerns:
        //: 1 The implementation defines case conversions (for comparisons)
        //:   identically with 'bsl::toupper' and 'bsl::tolower'.
        //
        // Plan:
        //: 1 Compute expected results using 'bsl::toupper' and 'bsl::tolower'
        //:   as oracles.  (C-1)
        //
        // Testing:
        //   areEqualCaseless(const bslstl::StringRef& lhs, rhs);
        //   lowerCaseCmp    (const bslstl::StringRef& lhs, rhs);
        //   upperCaseCmp    (const bslstl::StringRef& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "COMPARISONS" "\n"
                                  "===========" "\n";

        if (verbose) cout << "\n"
                          << "'areEqualCaseless'/'lowerCaseCmp'/'upperCaseCmp'"
                          << "\n";
        {
            static const bsl::size_t  STRLEN = bsl::string().max_size();
            static const char * const aTOz   = "abcdefghijklmnopqrstuvwxyz";
            static const char * const AtoZ   = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

            char msbSetChars[26];
            for (bsl::size_t i = 0; i < sizeof msbSetChars; ++i) {
                msbSetChars[i] = static_cast<char>(aTOz[i] | 0x80);
            }

            static const struct {
                int         d_line;
                const char *d_input_p;
                bsl::size_t d_length;
            } DATA[] = {
                // LINE INPUT            LENGTH
                // ---- ---------------  ------
                  // strings of length 0 or 1
                  { L_, ""             , STRLEN }
                , { L_, " "            , STRLEN }
                , { L_, "@"            , STRLEN }
                , { L_, "!"            , STRLEN }
                , { L_, "\t"           , STRLEN }
                , { L_, "\n"           , STRLEN }
                , { L_, "1"            , STRLEN }
                , { L_, "0"            , STRLEN }
                , { L_, "Z"            , STRLEN }
                , { L_, "z"            , STRLEN }

                  // strings of length 2 with whitespace characters
                , { L_, "  "           , STRLEN }
                , { L_, " \t"          , STRLEN }
                , { L_, " z"           , STRLEN }
                , { L_, "Z "           , STRLEN }
                , { L_, "@\t"          , STRLEN }
                , { L_, "\t@"          , STRLEN }
                , { L_, "1\n"          , STRLEN }
                , { L_, "\n1"          , STRLEN }

                  // strings of length 2 consisting of letters
                , { L_, "yz"           , STRLEN }
                , { L_, "YZ"           , STRLEN }
                , { L_, "Yz"           , STRLEN }
                , { L_, "yZ"           , STRLEN }

                  // strings of length 2 consisting of letters and/or digits
                , { L_, "y1"           , STRLEN }
                , { L_, "1y"           , STRLEN }
                , { L_, "Z0"           , STRLEN }
                , { L_, "0Z"           , STRLEN }
                , { L_, "01"           , STRLEN }

                  // strings of length 2 with symbols
                , { L_, "x@"           , STRLEN }
                , { L_, "@y"           , STRLEN }
                , { L_, "@1"           , STRLEN }
                , { L_, "1@"           , STRLEN }
                , { L_, "@!"           , STRLEN }

                  // strings of length 3 with whitespace
                , { L_, "x  "          , STRLEN }
                , { L_, " y "          , STRLEN }
                , { L_, "  z"          , STRLEN }
                , { L_, "X\n\n"        , STRLEN }
                , { L_, "\nY\n"        , STRLEN }
                , { L_, "\n\nZ"        , STRLEN }
                , { L_, "xy "          , STRLEN }
                , { L_, "x z"          , STRLEN }
                , { L_, " yz"          , STRLEN }
                , { L_, "X\tZ"         , STRLEN }
                , { L_, "\tYZ"         , STRLEN }
                , { L_, "XY\t"         , STRLEN }

                  // strings of length 3 consisting of letters
                , { L_, "xyz"          , STRLEN }
                , { L_, "xyZ"          , STRLEN }
                , { L_, "xYz"          , STRLEN }
                , { L_, "xYZ"          , STRLEN }
                , { L_, "Xyz"          , STRLEN }
                , { L_, "XyZ"          , STRLEN }
                , { L_, "XYz"          , STRLEN }
                , { L_, "XYZ"          , STRLEN }

                  // strings of length 3 consisting of letters and/or digits
                , { L_, "1YZ"          , STRLEN }
                , { L_, "X1Z"          , STRLEN }
                , { L_, "XY1"          , STRLEN }
                , { L_, "X10"          , STRLEN }
                , { L_, "2Y0"          , STRLEN }
                , { L_, "21Z"          , STRLEN }
                , { L_, "210"          , STRLEN }

                  // strings of length 3 with symbols
                , { L_, "@yZ"          , STRLEN }
                , { L_, "X@z"          , STRLEN }
                , { L_, "xY@"          , STRLEN }
                , { L_, "@!z"          , STRLEN }
                , { L_, "@y!"          , STRLEN }
                , { L_, "X@!"          , STRLEN }
                , { L_, "#@!"          , STRLEN }

                  // strings with length of 4 and above
                , { L_, "wXyZ"         , STRLEN }
                , { L_, "WxYz"         , STRLEN }
                , { L_, aTOz           , STRLEN }
                , { L_, AtoZ           , STRLEN }

                  // strings with embedded null characters
                , { L_, ""             ,  0     }
                , { L_, "\0"           ,  1     }
                , { L_, "XY"           ,  2     }
                , { L_, "\0Y"          ,  2     }
                , { L_, "X\0"          ,  2     }
                , { L_, "\0\0"         ,  2     }
                , { L_, "X\0Z"         ,  3     }
                , { L_, "\0YZ"         ,  3     }
                , { L_, "XY\0"         ,  3     }
                , { L_, "\0\0\0"       ,  3     }
                , { L_, "A\0\0"        ,  3     }
                , { L_, "\0B\0"        ,  3     }
                , { L_, "\0\0C"        ,  3     }

                , { L_, "\0\a\v\'\f\"" ,  6     }
                , { L_, msbSetChars    , 26     }
            };

            const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                    const int          LINE1    = DATA[i].d_line;
                    const char * const CSTRING1 = DATA[i].d_input_p;
                    const bsl::size_t  LENGTH1  = DATA[i].d_length == STRLEN
                                                  ? bsl::strlen(CSTRING1)
                                                  : DATA[i].d_length;


                    if (veryVerbose) { T_ P_(i) P_(LINE1) P(CSTRING1) }

                for (bsl::size_t j = 0; j < NUM_DATA; ++j) {
                    const int          LINE2    = DATA[j].d_line;
                    const char * const CSTRING2 = DATA[j].d_input_p;
                    const bsl::size_t  LENGTH2  = DATA[j].d_length == STRLEN
                                                  ? bsl::strlen(CSTRING2)
                                                  : DATA[j].d_length;

                    if (veryVerbose) { T_ T_ P_(j) P_(LINE2) P(CSTRING2) }

                    // Calculate expected values.

                    bool expectedEquality;
                    {
                        if (LENGTH1 != LENGTH2) {
                            expectedEquality = false;
                        } else {
                            bsl::string stringA(CSTRING1, LENGTH1);
                            bsl::string stringB(CSTRING2, LENGTH2);

                            bsl::transform(stringA.begin(),
                                           stringA.end(),
                                           stringA.begin(), // output
                                           Local::toUpper);

                            bsl::transform(stringB.begin(),
                                           stringB.end(),
                                           stringB.begin(), // output
                                           Local::toUpper);

                            expectedEquality = stringA == stringB;
                        }
                    }

                    int expectedLowerCmp;
                    int expectedUpperCmp;
                    {
                        bsl::string string1(CSTRING1, LENGTH1);
                        bsl::string string2(CSTRING2, LENGTH2);

                        bsl::transform(string1.begin(),
                                       string1.end(),
                                       string1.begin(), // output
                                       Local::toLower);

                        bsl::transform(string2.begin(),
                                       string2.end(),
                                       string2.begin(), // output
                                       Local::toLower);

                        int    rcToLower = string1.compare(string2);
                        expectedLowerCmp = rcToLower < 0 ? -1:
                                           rcToLower > 0 ?  1:
                                           /* else */       0;

                        string1.assign(CSTRING1, LENGTH1);
                        string2.assign(CSTRING2, LENGTH2);

                        bsl::transform(string1.begin(),
                                       string1.end(),
                                       string1.begin(), // output
                                       Local::toUpper);

                        bsl::transform(string2.begin(),
                                       string2.end(),
                                       string2.begin(), // output
                                       Local::toUpper);

                        int    rcToUpper = string1.compare(string2);
                        expectedUpperCmp = rcToLower < 0 ? -1:
                                           rcToUpper > 0 ?  1:
                                           /* else */       0;
                    }

                    // Calculate actual values.

                    SR s1(CSTRING1, CSTRING1 + LENGTH1); const SR& S1 = s1;
                    SR s2(CSTRING2, CSTRING2 + LENGTH2); const SR& S2 = s2;

                    bool actualEquality = Util::areEqualCaseless(S1, S2);
                    int  actualLowerCmp = Util::lowerCaseCmp    (S1, S2);
                    int  actualUpperCmp = Util::upperCaseCmp    (S1, S2);

                    // Compare expected with actual values.

                    ASSERT(expectedEquality == actualEquality);

                    ASSERT(expectedLowerCmp == actualLowerCmp);

                    ASSERT(expectedUpperCmp == actualUpperCmp);

                    if (true == expectedEquality && true == actualEquality) {

                        ASSERT(LENGTH1 == LENGTH2);
                        ASSERT(0       == actualLowerCmp);
                        ASSERT(0       == actualUpperCmp);

                        // Construct strings with mismatched sentinel 'char's.

                        bsl::string string1("0");
                        bsl::string string2("2");

                        for (bsl::size_t i = 0; i < LENGTH1; ++i) {
                            const char ch1 = CSTRING1[i];
                            const char ch2 = CSTRING2[i];
                            string1.append(1, ch1);
                            string2.append(1, ch2);
                        }
                        string1.append("1");
                        string2.append("3");

                        ASSERT(2 <= string1.length());
                        ASSERT(2 <= string2.length());

                        const char * const ADDR1 = string1.data() + 1;
                        const char * const ADDR2 = string2.data() + 1;

                        bool RESULT = Util::areEqualCaseless(
                                                   SR(ADDR1, ADDR1 + LENGTH1),
                                                   SR(ADDR2, ADDR2 + LENGTH2));

                        LOOP2_ASSERT(LINE1, LINE2, true == RESULT);
                   }
                }
            }
        }

        if (verbose) cout << "\n" "Check default contructed input"  << "\n";
        {
            const SR DEFAULT, nonDefault("a");

            ASSERT(true  == Util::areEqualCaseless(   DEFAULT,    DEFAULT));
            ASSERT(false == Util::areEqualCaseless(   DEFAULT, nonDefault));
            ASSERT(false == Util::areEqualCaseless(nonDefault,    DEFAULT));
            ASSERT(true  == Util::areEqualCaseless(nonDefault, nonDefault));

            ASSERT( 0    == Util::lowerCaseCmp    (   DEFAULT,    DEFAULT));
            ASSERT(-1    == Util::lowerCaseCmp    (   DEFAULT, nonDefault));
            ASSERT( 1    == Util::lowerCaseCmp    (nonDefault,    DEFAULT));
            ASSERT( 0    == Util::lowerCaseCmp    (nonDefault, nonDefault));

            ASSERT( 0    == Util::upperCaseCmp    (   DEFAULT,    DEFAULT));
            ASSERT(-1    == Util::upperCaseCmp    (   DEFAULT, nonDefault));
            ASSERT( 1    == Util::upperCaseCmp    (nonDefault,    DEFAULT));
            ASSERT( 0    == Util::upperCaseCmp    (nonDefault, nonDefault));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // HELPER FUNCTIONS
        //
        // Concerns:
        //: 1 The 'whitespaceLabel', 'Local::toLower', and 'Local::toUpper'
        //:   helper function can be used with confidence in subsequent tests.
        //:
        //: 2 The 'isEqual' helper function can be used with confidence in
        //:   subsequent tests.  Specifically, the 'isEqual' function
        //:   demonstrates the expected behaviors of an equality operator:
        //:   identify, commutativity, transitivity.
        //
        // Plan:
        //: 1 Comprehensive testing: for every possible input value,
        //:   '[0 .. 255]', compare the return values of 'whitespaceLabel'
        //:   'Local::toLower', and 'Local::toUpper' to their corresponding
        //:   expected values.  (C-1)
        //:
        //: 2 Create a table of object data (one per row) in which each unique
        //:   row has a minimal change in a single attribute (i.e., 'data' and
        //:   'length') sufficient to introduce an inequality relative the
        //:   other rows.  Confirm that 'isEqual' returns 'true' only when
        //:   comparing objects constructed from values having the same index
        //:   in the table.
        //
        // Testing:
        //   HELPER FUNCTION: 'whitespaceLabel'
        //   HELPER FUNCTION: 'isEqual'
        //   HELPER FUNCTION: 'Local::toLower'
        //   HELPER FUNCTION: 'Local::toUpper'
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "HELPER FUNCTIONS" "\n"
                                  "================" "\n";

        if (verbose) cout << "\n" "test 'whitespaceLabel'" "\n";
        testWhitespaceLabel();

        if (verbose) cout << "\n" "test 'isEqual'" "\n";
        testIsEqual();

        if (verbose) cout << "\n" "test 'Helper::*'" "\n";
        testLocalFunctions();

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
// Copyright 2016 Bloomberg Finance L.P.
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
