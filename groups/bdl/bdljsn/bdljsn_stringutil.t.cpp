// bdljsn_stringutil.t.cpp                                            -*-C++-*-
#include <bdljsn_stringutil.h>

#include <bdlb_string.h>

#include <bdlde_utf8util.h>

#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_review.h>

#include <bsl_cstddef.h>  // 'bsl::size_t'
#include <bsl_cstdlib.h>  // 'bsl::atoi'
#include <bsl_cstring.h>  // 'bsl::strlen'
#include <bsl_ios.h>      // 'bsl::ios::badbit'
#include <bsl_iostream.h>
#include <bsl_sstream.h>  // 'bsl::ostringstream'

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test is a utility that supplies functions that convert
// arbitrary UTF-8 strings to JSON-compatible strings, and back.  The
// implementations are independent the order of testing the encoding and
// decoding functionality is somewhat arbitrary: encoding ('writeString') is
// done first.
//
// Testing is complicated by the fact that input to the decoding functions are
// allowed to depart in certain aspects of the strict rules of JSON strings.
// Writing is always in strict compliance.  The round trip of valid data to a
// JSON string and back is always guaranteed.
//
// ----------------------------------------------------------------------------
// [ 2] static int readString        (bsl::string *, bsl::string_view);
// [ 2] static int readUnquotedString(bsl::string *, bsl::string_view);
// [ 1] static int writeString(bsl::ostream& s, const bsl::string_view&);
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE

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
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdljsn::StringUtil Util;

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    using namespace BloombergLP;

    const int test = argc > 1 ? bsl::atoi(argv[1]) : 0;

    BSLA_MAYBE_UNUSED const bool             verbose = argc > 2;
    BSLA_MAYBE_UNUSED const bool         veryVerbose = argc > 3;
    BSLA_MAYBE_UNUSED const bool     veryVeryVerbose = argc > 4;
    BSLA_MAYBE_UNUSED const bool veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
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

        if (verbose) {
            bsl::cout << bsl::endl
                      << "USAGE EXAMPLE" << bsl::endl
                      << "=============" << bsl::endl;
        }

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Encoding and Decoding a JSON String
/// - - - - - - - - - - - - - - - - - - - - - - -
/// First, we initialize a string with a valid sequence of UTF-8 codepoints.
//..
    bsl::string initial("Does the name \"Ivan Pavlov\" ring a bell\a?\n");
    ASSERT(bdlde::Utf8Util::isValid(initial));
//..
// Notice that, as required by C++ syntax, several characters are represented
// by their two-character escape sequence: double quote (twice), bell, and
// newline.
//
// Then, we examine the string as output:
//..
    bsl::cout << initial << bsl::endl;
//..
// and observe:
//..
//  Does the name "Ivan Pavlov" ring a bell?
//
//..
// Notice that the backslash characters (having served their purpose of giving
// special meaning to the subsequent character) are not shown.  The BELL and
// NEWLINE characters are output but are not visible.
//
// Now, we generate JSON string equivalent of the 'initial' string.
//..
    bsl::ostringstream oss;

    int rcEncode = bdljsn::StringUtil::writeString(oss, initial);
    ASSERT(0 == rcEncode);

    bsl::string  jsonCompatibleString = oss.str();
    bsl::cout << jsonCompatibleString << bsl::endl;
//..
// and observed how the 'initial' string is represented for JSON:
//..
//  "Does the name \"Ivan Pavlov\" ring a bell\u0007?\n"
//..
// Notice that:
//: o The entire string is delimited by double quotes.
//: o The interior double quotes and new line are represented by two character
//:   escape sequences (as they were in the C++ string literal.
//: o Since JSON does not have a two character escape sequence for the BELL
//:   character, '\u0007', the 6-byte Unicode representation is used.
//
// Finally, we convert the 'jsonCompatibleString' back to its original content:
//..
    bsl::string fromJsonString;
    const int   rcDecode = bdljsn::StringUtil::readString(
                                                         &fromJsonString,
                                                         jsonCompatibleString);
    ASSERT(0       == rcDecode);
    ASSERT(initial == fromJsonString);

    bsl::cout << fromJsonString << bsl::endl;
//..
// and observe (again):
//..
//  Does the name "Ivan Pavlov" ring a bell?
//
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DECODING STRINGS
        //
        // Concerns:
        //: 1 Values in the valid range, including the maximum and minimum
        //:   values for this type, are parsed correctly, for 'readString' and
        //:   'readUnquotedString'.
        //:
        //: 2 The passed in variable is unmodified if the data is not valid.
        //:
        //: 3 For 'readUnquotedString', embedded unescaped '"' characters are
        //:   placed in the result.
        //:
        //: 4 The return code is 0 on success and non-zero on failure.
        //:
        //: 5 The 'readString' converts the output of 'writeString' back to the
        //:   original contents.
        //:
        //: 6 Escape sequences starting with '\U' are rejected when the
        //:   optional 'flags' argument is omitted, or specified as 'e_NONE',
        //:   and are accepted when 'flags' is set to
        //:   'e_ACCEPT_CAPITAL_UNICODE_ESCAPE'.  Escape sequences starting
        //:   with '\u' are always accepted, irrespective of 'flags'.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct rows
        //:   of string value, expected parsed value, and return code.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Provide the string value and a variable to be parsed into to
        //:     the 'readString' function.  The variable is assigned a sentinel
        //:     value before invoking the function.
        //:
        //:   2 If the parsing should succeed then verify that the variable
        //:     value matches the expected value.  Otherwise confirm that the
        //:     variable value is unmodified.
        //:
        //:   3 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.
        //:
        //:   4 Call 'readString', making sure the result is identical in
        //:     return code and/or value.
        //:
        //:   5 Call 'readUnquotedString', making sure the result (if
        //:     successful) includes the extra outer '"'.
        //:
        //:   6 When the input is valid, confirm that the expected output can
        //:     be passed through 'writeString' and then 'readString' without
        //:     change.
        //:
        //: 3 Using the table-driven technique, specify a different set of
        //:   distinct rows of string value, expected parse value, and return
        //:   code for testing 'readUnquotedString'.
        //
        // Testing:
        //   static int readString        (bsl::string *, bsl::string_view);
        //   static int readUnquotedString(bsl::string *, bsl::string_view);
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << bsl::endl
                      << "DECODING STRINGS" << bsl::endl
                      << "================" << bsl::endl;
        }
        {
            typedef bsl::string Type;

            const char *ERROR_VALUE = "";

            static const struct Data {
                int         d_line;    // line number
                const char *d_input_p; // input on the stream
                int         d_inputLen;
                const char *d_exp_p;   // expected value
                int         d_expLen;
                bool        d_isValid; // isValid flag
            } DATA[] = {
                //line input             len exp                     isValid
                //---- -----             --- ---                    -------
                {  L_, "\"\"",           -1, "",                      -1, 1  },
                {  L_, "\"ABC\"",        -1, "ABC",                   -1, 1  },
                {  L_, "\"\\\"\"",       -1, "\"",                    -1, 1  },
                {  L_, "\"\\\\\"",       -1, "\\",                    -1, 1  },
                {  L_, "\"\\b\"",        -1, "\b",                    -1, 1  },
                {  L_, "\"\\f\"",        -1, "\f",                    -1, 1  },
                {  L_, "\"\\n\"",        -1, "\n",                    -1, 1  },
                {  L_, "\"\\r\"",        -1, "\r",                    -1, 1  },
                {  L_, "\"\\t\"",        -1, "\t",                    -1, 1  },

                {  L_, "\"u0001\"",      -1, "u0001",                 -1, 1  },
                {  L_, "\"UABCD\"",      -1, "UABCD",                 -1, 1  },

                {  L_, "\"\\u0001\"",    -1, "\x01",                  -1, 1  },
                {  L_, "\"\\u0020\"",    -1, " ",                     -1, 1  },
                {  L_, "\"\\u002E\"",    -1, ".",                     -1, 1  },
                {  L_, "\"\\u0041\"",    -1, "A",                     -1, 1  },

                {  L_, "\"\\U006d\"",    -1, "m",                     -1, 1  },
                {  L_, "\"\\U007E\"",    -1, "~",                     -1, 1  },

                {  L_, "\"\\U007F\"",    -1, "\x7F",                  -1, 1  },
                {  L_, "\"\\U0080\"",    -1, "\xC2\x80",              -1, 1  },

                {  L_, "\"\\U07FF\"",    -1, "\xDF\xBF",              -1, 1  },
                {  L_, "\"\\U0800\"",    -1, "\xE0\xA0\x80",          -1, 1  },

                {  L_, "\"\\UFFFF\"",    -1, "\xEF\xBF\xBF",          -1, 1  },

                {  L_, "\"\\U02f1\"",    -1, "\xCB\xB1",              -1, 1  },
                {  L_, "\"\\U2710\"",    -1, "\xE2\x9C\x90",          -1, 1  },
                {  L_, "\"\\UD7Ff\"",    -1, "\xED\x9F\xBF",          -1, 1  },
                {  L_, "\"\\Ue000\"",    -1, "\xEE\x80\x80",          -1, 1  },

                {  L_, "\"AB\"",         -1, "AB",                    -1, 1  },
                {  L_, "\"A\\u0020B\"",  -1, "A B",                   -1, 1  },
                {  L_, "\"A\\u002eB\"",  -1, "A.B",                   -1, 1  },
                {  L_, "\"A\\u0080G\"",  -1, "A\xC2\x80G",            -1, 1  },

                {  L_, "\"\\U0000\"",    -1, "\0",                     1, 1  },
                {  L_, "\"A\\U0000B\"",  -1, "A\0B",                   3, 1  },
                {  L_, "\"A\\U0000\\u0080G\"",
                                         -1, "A\0\xC2\x80G",           5, 1  },
                {  L_, "\"A\\U0000\\ue000G\"",
                                         -1, "A\0\xEE\x80\x80G",       6, 1  },

                {  L_, "\"\0\"",          3, "\0",                     1, 1  },

                {  L_, "\"\\U02f1\0\"",   9, "\xCB\xB1\0",             3, 1  },
                {  L_, "\"\\U2710\0\"",   9, "\xE2\x9C\x90\0",         4, 1  },
                {  L_, "\"\\UD7Ff\0\"",   9, "\xED\x9F\xBF\0",         4, 1  },
                {  L_, "\"\\Ue000\0\"",   9, "\xEE\x80\x80\0",         4, 1  },

                {  L_, "\"AB\0\"",        5, "AB\0",                   3, 1  },
                {  L_, "\"A\\u0020B\0\"",11, "A B\0",                  4, 1  },
                {  L_, "\"A\\u002eB\0\"",11, "A.B\0",                  4, 1  },
                {  L_, "\"A\\u0080G\0\"",11, "A\xC2\x80G\0",           5, 1  },

                {  L_, "\"\0\\U02f1\"",   9, "\0\xCB\xB1",             3, 1  },
                {  L_, "\"\0\\U2710\"",   9, "\0\xE2\x9C\x90",         4, 1  },
                {  L_, "\"\0\\UD7Ff\"",   9, "\0\xED\x9F\xBF",         4, 1  },
                {  L_, "\"\0\\Ue000\"",   9, "\0\xEE\x80\x80",         4, 1  },

                {  L_, "\"\0AB\"",        5, "\0AB",                   3, 1  },
                {  L_, "\"\0A\\u0020B\"",11, "\0A B",                  4, 1  },
                {  L_, "\"\0A\\u002eB\"",11, "\0A.B",                  4, 1  },
                {  L_, "\"\0A\\u0080G\"",11, "\0A\xC2\x80G",           5, 1  },

                {  L_, "\"\\U000G\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\U00h0\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\U0M00\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\UX000\"",    -1, ERROR_VALUE,             -1, 0  },

                {  L_, "\"\\U7G00\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\U007G\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\UXXXX\"",    -1, ERROR_VALUE,             -1, 0  },

                {  L_, "\"A\\U7G00B\"",  -1, "A",                     -1, 0  },
                {  L_, "\"A\\UXXXXB\"",  -1, "A",                     -1, 0  },

                // Do not allow early '"' in quoted strings
                {  L_, "\"\"abcd\"",     -1, "",                      -1, 0  },
                {  L_, "\"ab\"cd\"",     -1, "ab",                    -1, 0  },
                {  L_, "\"abcd\"\"",     -1, "abcd",                  -1, 0  },

                // Values that 'strtol' accepts - '{DRQS 162368243}'.
                {  L_, "\"\\U0xFF\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\U   4\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\U  -1\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\ud83d\\u0xFF\"", -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\u   4\"", -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\u  -1\"", -1, ERROR_VALUE,         -1, 0  },

                // These error strings were copied from
                // 'bdlde_charconvertutf32' test driver.

                // Values that are not valid Unicode because they are in the
                // lower UTF-16 bit plane.

                {  L_, "\"\\UD800\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\uD8ff\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\ud917\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\Udaaf\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\Udb09\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\UdbFF\"",    -1, ERROR_VALUE,             -1, 0  },

                // values that are not valid Unicode because they are in the
                // upper UTF-16 bit plane.

                {  L_, "\"\\UDc00\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\UDcFF\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\UDd80\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\UDea7\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\UDF03\"",    -1, ERROR_VALUE,             -1, 0  },
                {  L_, "\"\\UDFFF\"",    -1, ERROR_VALUE,             -1, 0  },

                // Supplementary plane characters.

                {  L_, "\"\\ud800\\udbff\"", -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\udbad\\udbff\"", -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\udbff\\udbff\"", -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud800\\udc00\"", -1, "\xF0\x90\x80\x80",  -1, 1  },
                {  L_, "\"\\ud83d\\ude42\"", -1, "\xF0\x9F\x99\x82",  -1, 1  },
                {  L_, "\"\\udbff\\udfff\"", -1, "\xF4\x8F\xBF\xBF",  -1, 1  },
                {  L_, "\"\\ud800\\ue000\"", -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\udbad\\ue000\"", -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\udbff\\ue000\"", -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\ude4\"",  -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\ude4",    -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\ude\"",   -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\ude",     -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\ud\"",    -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\ud",      -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\u\"",     -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\u",       -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\\"",      -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\",        -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\ude4`\"", -1, ERROR_VALUE,         -1, 0  },
                {  L_, "\"\\ud83d\\ude4g\"", -1, ERROR_VALUE,         -1, 0  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const Data&        data     = DATA[i];
                const int          LINE     = data.d_line;
                const char        *IN_P     = data.d_input_p;
                const bsl::size_t  IN_LEN   = data.d_inputLen < 0
                                            ? bsl::strlen(IN_P)
                                            : data.d_inputLen;
                const char        *EXP_P    = data.d_exp_p;
                const bsl::size_t  EXP_LEN  = data.d_expLen < 0
                                            ? bsl::strlen(EXP_P)
                                            : data.d_expLen;
                const bool         IS_VALID = data.d_isValid;

                const bool HAS_SLASH_U = 0 != bdlb::String::strstr(
                                               IN_P,  static_cast<int>(IN_LEN),
                                               "\\U", 2);


                if (veryVerbose) {
                    P_(i) P_(LINE) P_(IN_LEN) P_(EXP_LEN) P_(IS_VALID)
                                                                 P(HAS_SLASH_U)
                    P(IN_P)
                }

                const bsl::string_view isb    (IN_P,     IN_LEN);
                const bsl::string_view unq_isb(IN_P + 1, IN_LEN - 2);
                const bsl::string_view EXP(EXP_P, EXP_LEN);

                Type value2 = ERROR_VALUE;
                Type value3 = ERROR_VALUE;

                const unsigned mode = HAS_SLASH_U
                                    ? Util::e_ACCEPT_CAPITAL_UNICODE_ESCAPE
                                    : Util::e_NONE;

                const int rc2 = Util::readString        (&value2, isb, mode);
                const int rc3 = Util::readUnquotedString(&value3, unq_isb,
                                                                       mode);

                ASSERTV(value2, value3, value2 == value3);

                if (IS_VALID) {
                    ASSERTV(LINE, rc3, 0 == rc3);
                }
                else {
                    ASSERTV(LINE, rc2, 0 != rc2);
                }

                if (IS_VALID) {
                    bsl::ostringstream oss;
                    bsl::string_view   original = EXP;
                    const int          rcW      = Util::writeString(oss,
                                                                    original);
                    ASSERTV(LINE, rcW, 0 == rcW);

                    bsl::string generated = oss.str();
                    bsl::string roundTrip;
                    const int   rcG = Util::readString(&roundTrip, generated);
                    ASSERTV(LINE, rcG, 0         == rcG);
                    ASSERTV(LINE,      roundTrip == original);
                }
            }

            static const Data DATA2[] = {
                //line   input   len    exp   expLen     isValid
                //----   -----   ---    ---   ------     -------
                {  L_,   "",     -1,    "",       -1,    true   },
                {  L_,   "a",    -1,    "a",       1,    true   },
                {  L_,   "ab",   -1,    "ab",      2,    true   },
            };

            const int NUM_DATA2 = sizeof DATA2 / sizeof *DATA2;

            for (int i = 0; i < NUM_DATA2; ++i) {
                const Data&        data     = DATA2[i];
                const int          LINE     = data.d_line;
                const char        *IN_P     = data.d_input_p;
                const bsl::size_t  IN_LEN   = data.d_inputLen < 0
                                            ? bsl::strlen(IN_P)
                                            : data.d_inputLen;
                const char        *EXP_P    = data.d_exp_p;
                const bsl::size_t  EXP_LEN  = data.d_expLen < 0
                                            ? bsl::strlen(EXP_P)
                                            : data.d_expLen;
                const bool         IS_VALID = data.d_isValid;

                if (veryVerbose) {
                    P_(i) P_(LINE) P_(IN_LEN) P_(EXP_LEN) P(IS_VALID)
                }

                const bsl::string_view isb(IN_P,  IN_LEN);
                const bsl::string_view EXP(EXP_P, EXP_LEN);

                Type value = ERROR_VALUE;

                const int rc = Util::readUnquotedString(&value, isb);

                if (IS_VALID) {
                    ASSERTV(LINE, rc, 0 == rc);
                }
                else {
                    ASSERTV(LINE, rc, rc);
                }
                ASSERTV(LINE, EXP, value, EXP == value);
            }
        }

        if (verbose) cout << "Test 'flags'" << endl;
        {
            struct {
                int         d_line;
                const char *d_input_p;
                int         d_expectedStrict;
                int         d_expectedAccept;

            } DATA [] = {
                //LINE INPUT          ExpS  ExpA
                //---- -------------  ----  ----
                { L_,  "\"\\u0007\"",    0,    0 }
              , { L_,  "\"\\U0007\"",   -1,    0 }
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_line;
                const char *INPUT = DATA[ti].d_input_p;
                const int   EXP_S = DATA[ti].d_expectedStrict;
                const int   EXP_A = DATA[ti].d_expectedAccept;

                const bsl::size_t      INPUT_LENGTH = bsl::strlen(INPUT);
                const bsl::string_view INPUT_SANS_QUOTES(INPUT        + 1,
                                                         INPUT_LENGTH - 2);

                if (veryVerbose) {
                    P_(LINE) P_(INPUT) P_(INPUT_SANS_QUOTES) P_(EXP_S) P(EXP_A)
                }

                int rc = -2;

                bsl::string output; bsl::string *o = &output;

                rc = Util::readString(o, INPUT);
                ASSERTV(rc, EXP_S, EXP_S == rc);

                rc = Util::readString(o, INPUT, Util::e_NONE);
                ASSERTV(rc, EXP_S, EXP_S == rc);

                rc = Util::readString(o, INPUT,
                                        Util::e_ACCEPT_CAPITAL_UNICODE_ESCAPE);
                ASSERTV(rc, EXP_A, EXP_A == rc);

                // Sans delimiting quotes

                rc = Util::readUnquotedString(o, INPUT_SANS_QUOTES);
                ASSERTV(rc, EXP_S, EXP_S == rc);

                rc = Util::readUnquotedString(o, INPUT_SANS_QUOTES,
                                                                 Util::e_NONE);
                ASSERTV(rc, EXP_S, EXP_S == rc);

                rc = Util::readUnquotedString(o, INPUT_SANS_QUOTES,
                                        Util::e_ACCEPT_CAPITAL_UNICODE_ESCAPE);
                ASSERTV(rc, EXP_A, EXP_A == rc);
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // ENCODING STRINGS
        //
        // Concerns:
        //: 1 Character are encoded as a single character string.
        //:
        //: 2 All escape characters are encoded corrected.
        //:
        //: 3 Control characters are encoded as hex.
        //:
        //: 4 Invalid UTF-8 strings are rejected and return code is '-2'.
        //:
        //: 5 The input can have embedded null ('\0') characters.
        //:
        //: 6 Writing to an 'ostream' in a "bad" state fails with return code
        //:   of '-1'.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of values that include all escaped characters and
        //:     some control characters.
        //:
        //:   2 Encode the value and verify the results.
        //:
        //: 2 Repeat for strings and Customized type.
        //
        // Testing:
        //  static int writeString(bsl::ostream& s, const bsl::string_view&);
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << bsl::endl
                      << "ENCODING STRINGS" << bsl::endl
                      << "================" << bsl::endl;
        }

        if (verbose) cout << "Encode string" << endl;
        {
            const struct {
                int         d_line;
                const char *d_value_p;
                const char *d_result_p;
            } DATA[] = {
                //LINE   VAL     RESULT
                //----   ---     ------
                { L_,    "",     "\"\""     },
                { L_,    " ",    "\" \""    },
                { L_,    "~",    "\"~\""    },

                // Text
                { L_,    "test", "\"test\"" },
                { L_,    "A quick brown fox jump over a lazy dog!",
                               "\"A quick brown fox jump over a lazy dog!\"" },

                // Escape sequences
                { L_,    "\"",   "\"\\\"\"" },
                { L_,    "\\",   "\"\\\\\"" },
                { L_,    "/",    "\"\\/\""  },
                { L_,    "\b",   "\"\\b\""  },
                { L_,    "\f",   "\"\\f\""  },
                { L_,    "\n",   "\"\\n\""  },
                { L_,    "\r",   "\"\\r\""  },
                { L_,    "\t",   "\"\\t\""  },
                { L_,    "\\/\b\f\n\r\t", "\"\\\\\\/\\b\\f\\n\\r\\t\"" },

                { L_,    "\\u0007",          "\"\\\\u0007\"" },

                { L_,    "\xc2\x80",         "\"\xc2\x80\""         },
                { L_,    "\xdf\xbf",         "\"\xdf\xbf\""         },
                { L_,    "\xe0\xa0\x80",     "\"\xe0\xa0\x80\""     },
                { L_,    "\xef\xbf\xbf",     "\"\xef\xbf\xbf\""     },
                { L_,    "\xf0\x90\x80\x80", "\"\xf0\x90\x80\x80\"" },
                { L_,    "\xf4\x8f\xbf\xbf", "\"\xf4\x8f\xbf\xbf\"" },

                { L_,  "\x01", "\"\\u0001\"" },
                { L_,  "\x02", "\"\\u0002\"" },
                { L_,  "\x03", "\"\\u0003\"" },
                { L_,  "\x04", "\"\\u0004\"" },
                { L_,  "\x05", "\"\\u0005\"" },
                { L_,  "\x06", "\"\\u0006\"" },
                { L_,  "\x07", "\"\\u0007\"" },
                { L_,  "\x08", "\"\\b\""     },  // Backspace
                { L_,  "\x09", "\"\\t\""     },  // Horizontal tab
                { L_,  "\x0A", "\"\\n\""     },  // New line
                { L_,  "\x0B", "\"\\u000b\"" },  // Vertical tab
                { L_,  "\x0C", "\"\\f\""     },  // Form feed
                { L_,  "\x0D", "\"\\r\""     },  // Carriage return
                { L_,  "\x0E", "\"\\u000e\"" },
                { L_,  "\x0F", "\"\\u000f\"" },
                { L_,  "\x10", "\"\\u0010\"" },
                { L_,  "\x11", "\"\\u0011\"" },
                { L_,  "\x12", "\"\\u0012\"" },
                { L_,  "\x13", "\"\\u0013\"" },
                { L_,  "\x14", "\"\\u0014\"" },
                { L_,  "\x15", "\"\\u0015\"" },  // NACK (Negative ACK)
                { L_,  "\x16", "\"\\u0016\"" },
                { L_,  "\x17", "\"\\u0017\"" },
                { L_,  "\x18", "\"\\u0018\"" },  // Cancel
                { L_,  "\x19", "\"\\u0019\"" },
                { L_,  "\x1A", "\"\\u001a\"" },
                { L_,  "\x1B", "\"\\u001b\"" },  // Escape
                { L_,  "\x1C", "\"\\u001c\"" },
                { L_,  "\x1D", "\"\\u001d\"" },
                { L_,  "\x1E", "\"\\u001e\"" },
                { L_,  "\x1F", "\"\\u001f\"" }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE     = DATA[ti].d_line;
                const char *const VALUE    = DATA[ti].d_value_p;
                const char *const EXPECTED = DATA[ti].d_result_p;

                if (veryVeryVerbose) cout << "Test 'char *'" << endl;
                {
                    bsl::ostringstream oss;
                    ASSERTV(LINE, 0 == Util::writeString(oss, VALUE));

                    const bsl::string result(oss.str());
                    ASSERTV(LINE, result, EXPECTED, result == EXPECTED);
                }

                if (veryVeryVerbose) cout << "Test 'string'" << endl;
                {
                    bsl::ostringstream oss;
                    ASSERTV(LINE, 0 == Util::writeString(oss,
                                                         bsl::string(VALUE)));

                    const bsl::string result(oss.str());
                    ASSERTV(LINE, result, EXPECTED, result == EXPECTED);
                }
            }
        }

        if (verbose) cout << "Encode invalid UTF-8 string" << endl;
        {
            const struct {
                int         d_line;
                const char *d_value_p;
            } DATA[] = {
                //LINE  VALUE
                //----  -----
                { L_,   "\x80"             },
                { L_,   "\xc2\x00"         },
                { L_,   "\xc2\xff"         },
                { L_,   "\xc1\xbf"         },
                { L_,   "\xe0\x9f\xbf"     },
                { L_,   "\xf0\x8f\xbf\xbf" },
                { L_,   "\xf4\x9f\xbf\xbf" }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const char *const VALUE = DATA[ti].d_value_p;

                if (veryVeryVerbose) cout << "Test 'char *'" << endl;
                {
                    bsl::ostringstream oss;
                    ASSERTV(LINE, 0 != Util::writeString(oss, VALUE));
                }

                if (veryVeryVerbose) cout << "Test 'string'" << endl;
                {
                    bsl::ostringstream oss;
                    ASSERTV(LINE, -2 == Util::writeString(oss,
                                                          bsl::string(VALUE)));
                }
            }

        }

        if (verbose) cout << "Encode with embedded nulls" << endl;
        {
            bsl::string plain("Hello, world!\n");
            ASSERTV(plain, ',' == plain.data()[5]);
            ASSERTV(plain, ' ' == plain.data()[6]);

            bsl::ostringstream ossPlain;
            int                rcPlain   = Util::writeString(ossPlain, plain);
            bsl::string        jsonPlain = ossPlain.str();
            ASSERTV(  rcPlain,                      0 ==   rcPlain);
            ASSERTV(jsonPlain, "\"Hello, world!\\n\"" == jsonPlain);

            bsl::string embedded0s(plain);
            embedded0s.data()[5] = 0;
            embedded0s.data()[6] = 0;

            bsl::ostringstream ossEmbedded0s;
            int                rcEmbedded0s = Util::writeString(ossEmbedded0s,
                                                                   embedded0s);
            bsl::string        jsonEmbedded0s = ossEmbedded0s.str();
            ASSERTV(  rcEmbedded0s,                       0 ==   rcEmbedded0s);
            ASSERTV(jsonEmbedded0s, "\"Hello\\u0000\\u0000world!\\n\""
                                                            == jsonEmbedded0s);
        }

        if (verbose) cout << "Write to a \"bad\" 'ostream'" << endl;
        {
            bsl::ostringstream oss;
            oss.setstate(bsl::ios::badbit);

            int rc = Util::writeString(oss, "hello");
            ASSERTV(rc, -1 == rc);
        }

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      } break;
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
