// bdljsn_jsontestsuiteutil.cpp                                       -*-C++-*-
#include <bdljsn_jsontestsuiteutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdljsn_jsontestsuiteutil_cpp, "$Id$ $CSID$")

#include <bslma_default.h>

#include <bslmf_assert.h>

#include <bslmt_once.h>

#include <bsls_assert.h>

#include <bsl_string.h>

///Maintenance Note
///----------------
// The (manually-run) test driver case '-1' provides facilities for maintaining
// consistency between the table defined in this component and file content
// downloaded from:
// https://github.com/nst/JSONTestSuite/blob/master/test_parsers

using namespace BloombergLP;

namespace {
namespace u {

static const char *getLeftBrackets100000()
    // Return the address of a null-terminated string containing the input of
    // the 'n_structure_100000_opening_arrays.json' test point.
{
    static bsl::string leftBrackets100000(bslma::Default::globalAllocator());

    BSLMT_ONCE_DO {
        leftBrackets100000.reserve(100000);
        for (bsl::size_t i = 0; i < 100000; ++i) {
            leftBrackets100000 += '[';
        }
    }

    return leftBrackets100000.c_str();
}
static const char *const    leftBrackets100000 = getLeftBrackets100000();
static const bsl::size_t lenLeftBrackets100000 = 100000;

static const char *getOpenArrayObject50000()
    // Return the address of a null-terminated string containing the input of
    // the 'n_structure_open_array_object.json' test point.
{
    static char              openArrayObjectSubsequence[] = "[{\"\":";
    static const bsl::size_t lenOpenArrayObjectSubsequence
                                           = sizeof openArrayObjectSubsequence
                                           - 1; // terminating '/0'

    BSLMF_ASSERT(     5 == lenOpenArrayObjectSubsequence);
    BSLMF_ASSERT(250001 == lenOpenArrayObjectSubsequence * 50000 + 1);
                                                  // add 1 for terminating '\n'

    static bsl::string openArrayObject50000(bslma::Default::globalAllocator());

    BSLMT_ONCE_DO {
        openArrayObject50000.reserve(lenOpenArrayObjectSubsequence);

        for (bsl::size_t i = 0; i < 50000; ++i) {
            openArrayObject50000 += openArrayObjectSubsequence;
        }
        openArrayObject50000 += '\n';
    }

    return openArrayObject50000.c_str();
}

static const char * const    openArrayObject50000 = getOpenArrayObject50000();
static const bsl::size_t  lenOpenArrayObject50000 = 250001;

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace bdljsn {

                       // ------------------------
                       // struct JsonTestSuiteUtil
                       // ------------------------

// CLASS DATA

#define NL      "\n"
#define JSON(X) X, (sizeof(X) - 1)
#define L_ __LINE__  // current Line number

JsonTestSuiteUtil::Datum JsonTestSuiteUtil::s_data[] = {

                // Parser Error Required

  { L_, "n_array_1_true_without_comma.json",
    JSON("[1 true]"),
    e_REJECT }
, { L_, "n_array_a_invalid_utf8.json",
    JSON("[a\345]"),
    e_REJECT }
, { L_, "n_array_colon_instead_of_comma.json",
    JSON("[\"\": 1]"),
    e_REJECT }
, { L_, "n_array_comma_after_close.json",
    JSON("[\"\"],"),
    e_REJECT }
, { L_, "n_array_comma_and_number.json",
    JSON("[,1]"),
    e_REJECT }
, { L_, "n_array_double_comma.json",
    JSON("[1,,2]"),
    e_REJECT }
, { L_, "n_array_double_extra_comma.json",
    JSON("[\"x\",,]"),
    e_REJECT }
, { L_, "n_array_extra_close.json",
    JSON("[\"x\"]]"),
    e_REJECT }
, { L_, "n_array_extra_comma.json",
    JSON("[\"\",]"),
    e_REJECT }
, { L_, "n_array_incomplete.json",
    JSON("[\"x\""),
    e_REJECT }
, { L_, "n_array_incomplete_invalid_value.json",
    JSON("[x"),
    e_REJECT }
, { L_, "n_array_inner_array_no_comma.json",
    JSON("[3[4]]"),
    e_REJECT }
, { L_, "n_array_invalid_utf8.json",
    JSON("[\377]"),
    e_REJECT }
, { L_, "n_array_items_separated_by_semicolon.json",
    JSON("[1:2]"),
    e_REJECT }
, { L_, "n_array_just_comma.json",
    JSON("[,]"),
    e_REJECT }
, { L_, "n_array_just_minus.json",
    JSON("[-]"),
    e_REJECT }
, { L_, "n_array_missing_value.json",
    JSON("[   , \"\"]"),
    e_REJECT }
, { L_, "n_array_newlines_unclosed.json",
    JSON("[\"a\"," NL "4" NL ",1,"),
    e_REJECT }
, { L_, "n_array_number_and_comma.json",
    JSON("[1,]"),
    e_REJECT }
, { L_, "n_array_number_and_several_commas.json",
    JSON("[1,,]"),
    e_REJECT }
, { L_, "n_array_spaces_vertical_tab_formfeed.json",
    JSON("[\"\013a\"\\f]"),
    e_REJECT }
, { L_, "n_array_star_inside.json",
    JSON("[*]"),
    e_REJECT }
, { L_, "n_array_unclosed.json",
    JSON("[\"\""),
    e_REJECT }
, { L_, "n_array_unclosed_trailing_comma.json",
    JSON("[1,"),
    e_REJECT }
, { L_, "n_array_unclosed_with_new_lines.json",
    JSON("[1," NL "1" NL ",1"),
    e_REJECT }
, { L_, "n_array_unclosed_with_object_inside.json",
    JSON("[{}"),
    e_REJECT }
, { L_, "n_incomplete_false.json",
    JSON("[fals]"),
    e_REJECT }
, { L_, "n_incomplete_null.json",
    JSON("[nul]"),
    e_REJECT }
, { L_, "n_incomplete_true.json",
    JSON("[tru]"),
    e_REJECT }
, { L_, "n_multidigit_number_then_00.json",
    JSON("123\000"),
    e_REJECT }
, { L_, "n_number_++.json",
    JSON("[++1234]"),
    e_REJECT }
, { L_, "n_number_+1.json",
    JSON("[+1]"),
    e_REJECT }
, { L_, "n_number_+Inf.json",
    JSON("[+Inf]"),
    e_REJECT }
, { L_, "n_number_-01.json",
    JSON("[-01]"),
    e_REJECT }
, { L_, "n_number_-1.0..json",
    JSON("[-1.0.]"),
    e_REJECT }
, { L_, "n_number_-2..json",
    JSON("[-2.]"),
    e_REJECT }
, { L_, "n_number_-NaN.json",
    JSON("[-NaN]"),
    e_REJECT }
, { L_, "n_number_.-1.json",
    JSON("[.-1]"),
    e_REJECT }
, { L_, "n_number_.2e-3.json",
    JSON("[.2e-3]"),
    e_REJECT }
, { L_, "n_number_0.1.2.json",
    JSON("[0.1.2]"),
    e_REJECT }
, { L_, "n_number_0.3e+.json",
    JSON("[0.3e+]"),
    e_REJECT }
, { L_, "n_number_0.3e.json",
    JSON("[0.3e]"),
    e_REJECT }
, { L_, "n_number_0.e1.json",
    JSON("[0.e1]"),
    e_REJECT }
, { L_, "n_number_0_capital_E+.json",
    JSON("[0E+]"),
    e_REJECT }
, { L_, "n_number_0_capital_E.json",
    JSON("[0E]"),
    e_REJECT }
, { L_, "n_number_0e+.json",
    JSON("[0e+]"),
    e_REJECT }
, { L_, "n_number_0e.json",
    JSON("[0e]"),
    e_REJECT }
, { L_, "n_number_1.0e+.json",
    JSON("[1.0e+]"),
    e_REJECT }
, { L_, "n_number_1.0e-.json",
    JSON("[1.0e-]"),
    e_REJECT }
, { L_, "n_number_1.0e.json",
    JSON("[1.0e]"),
    e_REJECT }
, { L_, "n_number_1_000.json",
    JSON("[1 000.0]"),
    e_REJECT }
, { L_, "n_number_1eE2.json",
    JSON("[1eE2]"),
    e_REJECT }
, { L_, "n_number_2.e+3.json",
    JSON("[2.e+3]"),
    e_REJECT }
, { L_, "n_number_2.e-3.json",
    JSON("[2.e-3]"),
    e_REJECT }
, { L_, "n_number_2.e3.json",
    JSON("[2.e3]"),
    e_REJECT }
, { L_, "n_number_9.e+.json",
    JSON("[9.e+]"),
    e_REJECT }
, { L_, "n_number_expression.json",
    JSON("[1+2]"),
    e_REJECT }
, { L_, "n_number_hex_1_digit.json",
    JSON("[0x1]"),
    e_REJECT }
, { L_, "n_number_hex_2_digits.json",
    JSON("[0x42]"),
    e_REJECT }
, { L_, "n_number_Inf.json",
    JSON("[Inf]"),
    e_REJECT }
, { L_, "n_number_infinity.json",
    JSON("[Infinity]"),
    e_REJECT }
, { L_, "n_number_invalid+-.json",
    JSON("[0e+-1]"),
    e_REJECT }
, { L_, "n_number_invalid-negative-real.json",
    JSON("[-123.123foo]"),
    e_REJECT }
, { L_, "n_number_invalid-utf-8-in-bigger-int.json",
    JSON("[123\345]"),
    e_REJECT }
, { L_, "n_number_invalid-utf-8-in-exponent.json",
    JSON("[1e1\345]"),
    e_REJECT }
, { L_, "n_number_invalid-utf-8-in-int.json",
    JSON("[0\345]" NL),
    e_REJECT }
, { L_, "n_number_minus_infinity.json",
    JSON("[-Infinity]"),
    e_REJECT }
, { L_, "n_number_minus_sign_with_trailing_garbage.json",
    JSON("[-foo]"),
    e_REJECT }
, { L_, "n_structure_100000_opening_arrays.json",
    0, 0,  // Initialized on first call to the 'data' method.
    e_REJECT }
, { L_, "n_number_minus_space_1.json",
    JSON("[- 1]"),
    e_REJECT }
, { L_, "n_number_NaN.json",
    JSON("[NaN]"),
    e_REJECT }
, { L_, "n_number_neg_int_starting_with_zero.json",
    JSON("[-012]"),
    e_REJECT }
, { L_, "n_number_neg_real_without_int_part.json",
    JSON("[-.123]"),
    e_REJECT }
, { L_, "n_number_neg_with_garbage_at_end.json",
    JSON("[-1x]"),
    e_REJECT }
, { L_, "n_number_real_garbage_after_e.json",
    JSON("[1ea]"),
    e_REJECT }
, { L_, "n_number_real_with_invalid_utf8_after_e.json",
    JSON("[1e\345]"),
    e_REJECT }
, { L_, "n_number_real_without_fractional_part.json",
    JSON("[1.]"),
    e_REJECT }
, { L_, "n_number_starting_with_dot.json",
    JSON("[.123]"),
    e_REJECT }
, { L_, "n_number_U+FF11_fullwidth_digit_one.json",
    JSON("[\357\274\221]"),
    e_REJECT }
, { L_, "n_number_with_alpha.json",
    JSON("[1.2a-3]"),
    e_REJECT }
, { L_, "n_number_with_alpha_char.json",
    JSON("[1.8011670033376514H-308]"),
    e_REJECT }
, { L_, "n_number_with_leading_zero.json",
    JSON("[012]"),
    e_REJECT }
, { L_, "n_object_bad_value.json",
    JSON("[\"x\", truth]"),
    e_REJECT }
, { L_, "n_object_bracket_key.json",
    JSON("{[: \"x\"}" NL),
    e_REJECT }
, { L_, "n_object_comma_instead_of_colon.json",
    JSON("{\"x\", null}"),
    e_REJECT }
, { L_, "n_object_double_colon.json",
    JSON("{\"x\"::\"b\"}"),
    e_REJECT }
, { L_, "n_object_emoji.json",
    JSON("{\360\237\207\250\360\237\207\255}"),
    e_REJECT }
, { L_, "n_object_garbage_at_end.json",
    JSON("{\"a\":\"a\" 123}"),
    e_REJECT }
, { L_, "n_object_key_with_single_quotes.json",
    JSON("{key: \047value\047}"),
    e_REJECT }
, { L_, "n_object_lone_continuation_byte_in_key_and_trailing_comma.json",
    JSON("{\"\271\":\"0\",}"),
    e_REJECT }
, { L_, "n_object_missing_colon.json",
    JSON("{\"a\" b}"),
    e_REJECT }
, { L_, "n_object_missing_key.json",
    JSON("{:\"b\"}"),
    e_REJECT }
, { L_, "n_object_missing_semicolon.json",
    JSON("{\"a\" \"b\"}"),
    e_REJECT }
, { L_, "n_object_missing_value.json",
    JSON("{\"a\":"),
    e_REJECT }
, { L_, "n_object_no-colon.json",
    JSON("{\"a\""),
    e_REJECT }
, { L_, "n_object_non_string_key.json",
    JSON("{1:1}"),
    e_REJECT }
, { L_, "n_object_non_string_key_but_huge_number_instead.json",
    JSON("{9999E9999:1}"),
    e_REJECT }
, { L_, "n_object_repeated_null_null.json",
    JSON("{null:null,null:null}"),
    e_REJECT }
, { L_, "n_object_several_trailing_commas.json",
    JSON("{\"id\":0,,,,,}"),
    e_REJECT }
, { L_, "n_object_single_quote.json",
    JSON("{\047a\047:0}"),
    e_REJECT }
, { L_, "n_object_trailing_comma.json",
    JSON("{\"id\":0,}"),
    e_REJECT }
, { L_, "n_object_trailing_comment.json",
    JSON("{\"a\":\"b\"}/**/"),
    e_REJECT }
, { L_, "n_object_trailing_comment_open.json",
    JSON("{\"a\":\"b\"}/**//"),
    e_REJECT }
, { L_, "n_object_trailing_comment_slash_open.json",
    JSON("{\"a\":\"b\"}//"),
    e_REJECT }
, { L_, "n_object_trailing_comment_slash_open_incomplete.json",
    JSON("{\"a\":\"b\"}/"),
    e_REJECT }
, { L_, "n_object_two_commas_in_a_row.json",
    JSON("{\"a\":\"b\",,\"c\":\"d\"}"),
    e_REJECT }
, { L_, "n_object_unquoted_key.json",
    JSON("{a: \"b\"}"),
    e_REJECT }
, { L_, "n_object_unterminated-value.json",
    JSON("{\"a\":\"a"),
    e_REJECT }
, { L_, "n_object_with_single_string.json",
    JSON("{ \"foo\" : \"bar\", \"a\" }"),
    e_REJECT }
, { L_, "n_object_with_trailing_garbage.json",
    JSON("{\"a\":\"b\"}#"),
    e_REJECT }
, { L_, "n_single_space.json",
    JSON(" "),
    e_REJECT }
, { L_, "n_string_1_surrogate_then_escape.json",
    JSON("[\"\\uD800\\\"]"),
    e_REJECT }
, { L_, "n_string_1_surrogate_then_escape_u.json",
    JSON("[\"\\uD800\\u\"]"),
    e_REJECT }
, { L_, "n_string_1_surrogate_then_escape_u1.json",
    JSON("[\"\\uD800\\u1\"]"),
    e_REJECT }
, { L_, "n_string_1_surrogate_then_escape_u1x.json",
    JSON("[\"\\uD800\\u1x\"]"),
    e_REJECT }
, { L_, "n_string_accentuated_char_no_quotes.json",
    JSON("[\303\251]"),
    e_REJECT }
, { L_, "n_string_backslash_00.json",
    JSON("[\"\\\000\"]"),
    e_REJECT }
, { L_, "n_string_escape_x.json",
    JSON("[\"\\x00\"]"),
    e_REJECT }
, { L_, "n_string_escaped_backslash_bad.json",
    JSON("[\"\\\\\\\"]"),
    e_REJECT }
, { L_, "n_string_escaped_ctrl_char_tab.json",
    JSON("[\"\\\t\"]"),
    e_REJECT }
, { L_, "n_string_escaped_emoji.json",
    JSON("[\"\\\360\237\214\200\"]"),
    e_REJECT }
, { L_, "n_string_incomplete_escape.json",
    JSON("[\"\\\"]"),
    e_REJECT }
, { L_, "n_string_incomplete_escaped_character.json",
    JSON("[\"\\u00A\"]"),
    e_REJECT }
, { L_, "n_string_incomplete_surrogate.json",
    JSON("[\"\\uD834\\uDd\"]"),
    e_REJECT }
, { L_, "n_string_incomplete_surrogate_escape_invalid.json",
    JSON("[\"\\uD800\\uD800\\x\"]"),
    e_REJECT }
, { L_, "n_string_invalid-utf-8-in-escape.json",
    JSON("[\"\\u\345\"]"),
    e_REJECT }
, { L_, "n_string_invalid_backslash_esc.json",
    JSON("[\"\\a\"]"),
    e_REJECT }
, { L_, "n_string_invalid_unicode_escape.json",
    JSON("[\"\\uqqqq\"]"),
    e_REJECT }
, { L_, "n_string_invalid_utf8_after_escape.json",
    JSON("[\"\\\345\"]"),
    e_REJECT }
, { L_, "n_string_leading_uescaped_thinspace.json",
    JSON("[\\u0020\"asd\"]"),
    e_REJECT }
, { L_, "n_string_no_quotes_with_bad_escape.json",
    JSON("[\\n]"),
    e_REJECT }
, { L_, "n_string_single_doublequote.json",
    JSON("\""),
    e_REJECT }
, { L_, "n_string_single_quote.json",
    JSON("[\047single quote\047]"),
    e_REJECT }
, { L_, "n_string_single_string_no_double_quotes.json",
    JSON("abc"),
    e_REJECT }
, { L_, "n_string_start_escape_unclosed.json",
    JSON("[\"\\"),
    e_REJECT }
, { L_, "n_string_unescaped_ctrl_char.json", // Note: Fix in 'baljsn' package.
    JSON("[\"a\000a\"]"),
    e_REJECT }
, { L_, "n_string_unescaped_newline.json",
    JSON("[\"new\n"
            "line\"]"),
    e_REJECT }
, { L_, "n_string_unescaped_tab.json",
    JSON("[\"\t\"]"),
    e_REJECT }
, { L_, "n_string_unicode_CapitalU.json",
    JSON("\"\\UA66D\""),
    e_REJECT }
, { L_, "n_string_with_trailing_garbage.json",
    JSON("\"\"x"),
    e_REJECT }
, { L_, "n_structure_angle_bracket_..json",
    JSON("<.\076"),
    e_REJECT }
, { L_, "n_structure_angle_bracket_null.json",
    JSON("[<null\076]"),
    e_REJECT }
, { L_, "n_structure_array_trailing_garbage.json",
    JSON("[1]x"),
    e_REJECT }
, { L_, "n_structure_array_with_extra_array_close.json",
    JSON("[1]]"),
    e_REJECT }
, { L_, "n_structure_array_with_unclosed_string.json",
    JSON("[\"asd]"),
    e_REJECT }
, { L_, "n_structure_ascii-unicode-identifier.json",
    JSON("a\303\245"),
    e_REJECT }
, { L_, "n_structure_capitalized_True.json",
    JSON("[True]"),
    e_REJECT }
, { L_, "n_structure_close_unopened_array.json",
    JSON("1]"),
    e_REJECT }
, { L_, "n_structure_comma_instead_of_closing_brace.json",
    JSON("{\"x\": true,"),
    e_REJECT }
, { L_, "n_structure_double_array.json",
    JSON("[][]"),
    e_REJECT }
, { L_, "n_structure_end_array.json",
    JSON("]"),
    e_REJECT }
, { L_, "n_structure_incomplete_UTF8_BOM.json",
    JSON("\357\273{}"),
    e_REJECT }
, { L_, "n_structure_lone-invalid-utf-8.json",
    JSON("\345"),
    e_REJECT }
, { L_, "n_structure_lone-open-bracket.json",
    JSON("["),
    e_REJECT }
, { L_, "n_structure_no_data.json",
    JSON(""),
    e_REJECT }
, { L_, "n_structure_null-byte-outside-string.json",
    JSON("[\000]"),
    e_REJECT }
, { L_, "n_structure_number_with_trailing_garbage.json",
    JSON("2@"),
    e_REJECT }
, { L_, "n_structure_object_followed_by_closing_object.json",
    JSON("{}}"),
    e_REJECT }
, { L_, "n_structure_object_unclosed_no_value.json",
    JSON("{\"\":"),
    e_REJECT }
, { L_, "n_structure_object_with_comment.json",
    JSON("{\"a\":/*comment*/\"b\"}"),
    e_REJECT }
, { L_, "n_structure_object_with_trailing_garbage.json",
    JSON("{\"a\": true} \"x\""),
    e_REJECT }
, { L_, "n_structure_open_array_apostrophe.json",
    JSON("[\047"),
    e_REJECT }
, { L_, "n_structure_open_array_comma.json",
    JSON("[,"),
    e_REJECT }
, { L_, "n_structure_open_array_object.json",
    0, 0,  // Initialized on first call to the 'data' method.
    e_REJECT }
, { L_, "n_structure_open_array_open_object.json",
    JSON("[{"),
    e_REJECT }
, { L_, "n_structure_open_array_open_string.json",
    JSON("[\"a"),
    e_REJECT }
, { L_, "n_structure_open_array_string.json",
    JSON("[\"a\""),
    e_REJECT }
, { L_, "n_structure_open_object.json",
    JSON("{"),
    e_REJECT }
, { L_, "n_structure_open_object_close_array.json",
    JSON("{]"),
    e_REJECT }
, { L_, "n_structure_open_object_comma.json",
    JSON("{,"),
    e_REJECT }
, { L_, "n_structure_open_object_open_array.json",
    JSON("{["),
    e_REJECT }
, { L_, "n_structure_open_object_open_string.json",
    JSON("{\"a"),
    e_REJECT }
, { L_, "n_structure_open_object_string_with_apostrophes.json",
    JSON("{\047a\047"),
    e_REJECT }
, { L_, "n_structure_open_open.json",
    JSON("[\"\\{[\"\\{[\"\\{[\"\\{"),
    e_REJECT }
, { L_, "n_structure_single_eacute.json",
    JSON("\351"),
    e_REJECT }
, { L_, "n_structure_single_star.json",
    JSON("*"),
    e_REJECT }
, { L_, "n_structure_trailing_#.json",
    JSON("{\"a\":\"b\"}#{}"),
    e_REJECT }
, { L_, "n_structure_U+2060_word_joined.json",
    JSON("[\342\201\240]"),
    e_REJECT }
, { L_, "n_structure_uescaped_LF_before_string.json",
    JSON("[\\u000A\"\"]"),
    e_REJECT }
, { L_, "n_structure_unclosed_array.json",
    JSON("[1"),
    e_REJECT }
, { L_, "n_structure_unclosed_array_partial_null.json",
    JSON("[ false, nul"),
    e_REJECT }
, { L_, "n_structure_unclosed_array_unfinished_false.json",
    JSON("[ true, fals"),
    e_REJECT }
, { L_, "n_structure_unclosed_array_unfinished_true.json",
    JSON("[ false, tru"),
    e_REJECT }
, { L_, "n_structure_unclosed_object.json",
    JSON("{\"asd\":\"asd\""),
    e_REJECT }
, { L_, "n_structure_unicode-identifier.json",
    JSON("\303\245"),
    e_REJECT }
, { L_, "n_structure_UTF8_BOM_no_data.json",
    JSON("\357\273\277"),
    e_REJECT }
, { L_, "n_structure_whitespace_formfeed.json",
    JSON("[\014]"),
    e_REJECT }
, { L_, "n_structure_whitespace_U+2060_word_joiner.json",
    JSON("[\342\201\240]"),
    e_REJECT }

                // Parser Success Required

, { L_, "y_array_arraysWithSpaces.json",
    JSON("[[]   ]"),
    e_ACCEPT }
, { L_, "y_array_empty-string.json",
    JSON("[\"\"]"),
    e_ACCEPT }
, { L_, "y_array_empty.json",
    JSON("[]"),
    e_ACCEPT }
, { L_, "y_array_ending_with_newline.json", // Note: The file has *no* newline.
    JSON("[\"a\"]"),                        // See {SDSK 1270912158 <GO>}.
    e_ACCEPT }
, { L_, "y_array_false.json",
    JSON("[false]"),
    e_ACCEPT }
, { L_, "y_array_heterogeneous.json",
    JSON("[null, 1, \"1\", {}]"),
    e_ACCEPT }
, { L_, "y_array_null.json",
    JSON("[null]"),
    e_ACCEPT }
, { L_, "y_array_with_1_and_newline.json",
    JSON("[1\n]"),
    e_ACCEPT }
, { L_, "y_array_with_leading_space.json",
    JSON(" [1]"),
    e_ACCEPT }
, { L_, "y_array_with_several_null.json",
    JSON("[1,null,null,null,2]"),
    e_ACCEPT }
, { L_, "y_array_with_trailing_space.json",
    JSON("[2] "),
    e_ACCEPT }
, { L_, "y_number.json",
    JSON("[123e65]"),
    e_ACCEPT }
, { L_, "y_number_0e+1.json",
    JSON("[0e+1]"),
    e_ACCEPT }
, { L_, "y_number_0e1.json",
    JSON("[0e1]"),
    e_ACCEPT }
, { L_, "y_number_after_space.json",
    JSON("[ 4]"),
    e_ACCEPT }
, { L_, "y_number_double_close_to_zero.json",
    JSON("[-0.0000000000000000000000000000000000000000000000000000000000000000"
             "00000000000001]" NL),
    e_ACCEPT }
, { L_, "y_number_int_with_exp.json",
    JSON("[20e1]"),
    e_ACCEPT }
, { L_, "y_number_minus_zero.json",
    JSON("[-0]"),
    e_ACCEPT }
, { L_, "y_number_negative_int.json",
    JSON("[-123]"),
    e_ACCEPT }
, { L_, "y_number_negative_one.json",
    JSON("[-1]"),
    e_ACCEPT }
, { L_, "y_number_negative_zero.json",
    JSON("[-0]"),
    e_ACCEPT }
, { L_, "y_number_real_capital_e.json",
    JSON("[1E22]"),
    e_ACCEPT }
, { L_, "y_number_real_capital_e_neg_exp.json",
    JSON("[1E-2]"),
    e_ACCEPT }
, { L_, "y_number_real_capital_e_pos_exp.json",
    JSON("[1E+2]"),
    e_ACCEPT }
, { L_, "y_number_real_exponent.json",
    JSON("[123e45]"),
    e_ACCEPT }
, { L_, "y_number_real_fraction_exponent.json",
    JSON("[123.456e78]"),
    e_ACCEPT }
, { L_, "y_number_real_neg_exp.json",
    JSON("[1e-2]"),
    e_ACCEPT }
, { L_, "y_number_real_pos_exponent.json",
    JSON("[1e+2]"),
    e_ACCEPT }
, { L_, "y_number_simple_int.json",
    JSON("[123]"),
    e_ACCEPT }
, { L_, "y_number_simple_real.json",
    JSON("[123.456789]"),
    e_ACCEPT }
, { L_, "y_object.json",
    JSON("{\"asd\":\"sdf\", \"dfg\":\"fgh\"}"),
    e_ACCEPT }
, { L_, "y_object_basic.json",
    JSON("{\"asd\":\"sdf\"}"),
    e_ACCEPT }
, { L_, "y_object_duplicated_key.json",
    JSON("{\"a\":\"b\",\"a\":\"c\"}"),
    e_ACCEPT }
, { L_, "y_object_duplicated_key_and_value.json",
    JSON("{\"a\":\"b\",\"a\":\"b\"}"),
    e_ACCEPT }
, { L_, "y_object_empty.json",
    JSON("{}"),
    e_ACCEPT }
, { L_, "y_object_empty_key.json",
    JSON("{\"\":0}"),
    e_ACCEPT }
, { L_, "y_object_escaped_null_in_key.json",
    JSON("{\"foo\\u0000bar\": 42}"),
    e_ACCEPT }
, { L_, "y_object_extreme_numbers.json",
    JSON("{ \"min\": -1.0e+28, \"max\": 1.0e+28 }"),
    e_ACCEPT }
, { L_, "y_object_long_strings.json",
    JSON("{\"x\":[{\"id\": \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"}], \""
                    "id\": \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"}"),
    e_ACCEPT }
, { L_, "y_object_simple.json",
    JSON("{\"a\":[]}"),
    e_ACCEPT }
, { L_, "y_object_string_unicode.json",
    JSON("{\"title\":\"\\u041f\\u043e\\u043b\\u"
             "0442\\u043e\\u0440\\u0430 \\u0417\\"
             "u0435\\u043c\\u043b\\u0435\\u043a\\"
             "u043e\\u043f\\u0430\" }"),
    e_ACCEPT }
, { L_, "y_object_with_newlines.json",
    JSON("{\n\"a\": \"b\"\n}"),
    e_ACCEPT }
, { L_, "y_string_1_2_3_bytes_UTF-8_sequences.json",
    JSON("[\"\\u0060\\u012a\\u12AB\"]"),
    e_ACCEPT }
, { L_, "y_string_accepted_surrogate_pair.json",
    JSON("[\"\\uD801\\udc37\"]"),
    e_ACCEPT }
, { L_, "y_string_accepted_surrogate_pairs.json",
    JSON("[\"\\ud83d\\ude39\\ud83d\\udc8d\"]"),
    e_ACCEPT }
, { L_, "y_string_allowed_escapes.json",
    JSON("[\"\\\"\\\\\\/\\b\\f\\n\\r\\t\"]"),
    e_ACCEPT }
, { L_, "y_string_backslash_and_u_escaped_zero.json",
    JSON("[\"\\\\u0000\"]"),
    e_ACCEPT }
, { L_, "y_string_backslash_doublequotes.json",
    JSON("[\"\\\"\"]"),
    e_ACCEPT }
, { L_, "y_string_comments.json",
    JSON("[\"a/*b*/c/*d//e\"]"),
    e_ACCEPT }
, { L_, "y_string_double_escape_a.json",
    JSON("[\"\\\\a\"]"),
    e_ACCEPT }
, { L_, "y_string_double_escape_n.json",
    JSON("[\"\\\\n\"]"),
    e_ACCEPT }
, { L_, "y_string_escaped_control_character.json",
    JSON("[\"\\u0012\"]"),
    e_ACCEPT }
, { L_, "y_string_escaped_noncharacter.json",
    JSON("[\"\\uFFFF\"]"),
    e_ACCEPT }
, { L_, "y_string_in_array.json",
    JSON("[\"asd\"]"),
    e_ACCEPT }
, { L_, "y_string_in_array_with_leading_space.json",
    JSON("[ \"asd\"]"),
    e_ACCEPT }
, { L_, "y_string_last_surrogates_1_and_2.json",
    JSON("[\"\\uDBFF\\uDFFF\"]"),
    e_ACCEPT }
, { L_, "y_string_nbsp_uescaped.json",
    JSON("[\"new\\u00A0line\"]"),
    e_ACCEPT }
, { L_, "y_string_nonCharacterInUTF-8_U+10FFFF.json",
    JSON("[\"\364\217\277\277\"]"),
    e_ACCEPT }
, { L_, "y_string_nonCharacterInUTF-8_U+FFFF.json",
    JSON("[\"\357\277\277\"]"),
    e_ACCEPT }
, { L_, "y_string_null_escape.json",
    JSON("[\"\\u0000\"]"),
    e_ACCEPT }
, { L_, "y_string_one-byte-utf-8.json",
    JSON("[\"\\u002c\"]"),
    e_ACCEPT }
, { L_, "y_string_pi.json",
    JSON("[\"\317\200\"]"),
    e_ACCEPT }
, { L_, "y_string_reservedCharacterInUTF-8_U+1BFFF.json",
    JSON("[\"\360\233\277\277\"]"),
    e_ACCEPT }
, { L_, "y_string_simple_ascii.json",
    JSON("[\"asd \"]"),
    e_ACCEPT }
, { L_, "y_string_space.json",
    JSON("\" \""),
    e_ACCEPT }
, { L_, "y_string_surrogates_U+1D11E_MUSICAL_SYMBOL_G_CLEF.json",
    JSON("[\"\\uD834\\uDd1e\"]"),
    e_ACCEPT }
, { L_, "y_string_three-byte-utf-8.json",
    JSON("[\"\\u0821\"]"),
    e_ACCEPT }
, { L_, "y_string_two-byte-utf-8.json",
    JSON("[\"\\u0123\"]"),
    e_ACCEPT }
, { L_, "y_string_u+2028_line_sep.json",
    JSON("[\"\342\200\250\"]"),
    e_ACCEPT }
, { L_, "y_string_u+2029_par_sep.json",
    JSON("[\"\342\200\251\"]"),
    e_ACCEPT }
, { L_, "y_string_uEscape.json",
    JSON("[\"\\u0061\\u30af\\u30EA\\u30b9\"]"),
    e_ACCEPT }
, { L_, "y_string_uescaped_newline.json",
    JSON("[\"new\\u000Aline\"]"),
    e_ACCEPT }
, { L_, "y_string_unescaped_char_delete.json",
    JSON("[\"\177\"]"),
    e_ACCEPT }
, { L_, "y_string_unicode.json",
    JSON("[\"\\uA66D\"]"),
    e_ACCEPT }
, { L_, "y_string_unicode_2.json",
    JSON("[\"\342\215\202\343\210\264\342\215\202\"]"),
    e_ACCEPT }
, { L_, "y_string_unicode_escaped_double_quote.json",
    JSON("[\"\\u0022\"]"),
    e_ACCEPT }
, { L_, "y_string_unicode_U+10FFFE_nonchar.json",
    JSON("[\"\\uDBFF\\uDFFE\"]"),
    e_ACCEPT }
, { L_, "y_string_unicode_U+1FFFE_nonchar.json",
    JSON("[\"\\uD83F\\uDFFE\"]"),
    e_ACCEPT }
, { L_, "y_string_unicode_U+200B_ZERO_WIDTH_SPACE.json",
    JSON("[\"\\u200B\"]"),
    e_ACCEPT }
, { L_, "y_string_unicode_U+2064_invisible_plus.json",
    JSON("[\"\\u2064\"]"),
    e_ACCEPT }
, { L_, "y_string_unicode_U+FDD0_nonchar.json",
    JSON("[\"\\uFDD0\"]"),
    e_ACCEPT }
, { L_, "y_string_unicode_U+FFFE_nonchar.json",
    JSON("[\"\\uFFFE\"]"),
    e_ACCEPT }
, { L_, "y_string_unicodeEscapedBackslash.json",
    JSON("[\"\\u005C\"]"),
    e_ACCEPT }
, { L_, "y_string_utf8.json",
    JSON("[\"\342\202\254\360\235\204\236\"]"),
    e_ACCEPT }
, { L_, "y_string_with_del_character.json",
    JSON("[\"a\177a\"]"),
    e_ACCEPT }
, { L_, "y_structure_lonely_false.json",
    JSON("false"),
    e_ACCEPT }
, { L_, "y_structure_lonely_int.json",
    JSON("42"),
    e_ACCEPT }
, { L_, "y_structure_lonely_negative_real.json",
    JSON("-0.1"),
    e_ACCEPT }
, { L_, "y_structure_lonely_null.json",
    JSON("null"),
    e_ACCEPT }
, { L_, "y_structure_lonely_string.json",
    JSON("\"asd\""),
    e_ACCEPT }
, { L_, "y_structure_lonely_true.json",
    JSON("true"),
    e_ACCEPT }
, { L_, "y_structure_string_empty.json",
    JSON("\"\""),
    e_ACCEPT }
, { L_, "y_structure_trailing_newline.json",
    JSON("[\"a\"]\n"),
    e_ACCEPT }
, { L_, "y_structure_true_in_array.json",
    JSON("[true]"),
    e_ACCEPT }
, { L_, "y_structure_whitespace_array.json",
    JSON(" [] "),
    e_ACCEPT }

                // Additional Entry

// Additional test case, not actually from test suite.
, { L_, "y_henry_verschell_smiley_surrogate_smiley.json",
    JSON("\"\xF0\x9F\x98\x80\\ud83d\\ude00\""),
    e_ACCEPT }

                // Implementation Dependent Behavior: accept or reject

, { L_, "i_number_double_huge_neg_exp.json",
    JSON("[123.456e-789]"),
    e_EITHER }
, { L_, "i_number_huge_exp.json",
    JSON("[0.4e0066"
       //         10        20        30        40        50
       // ----v----|----v----|----v----|----v----|----v----|----v---
         "9999999999999999999999999999999999999999999999999999999999"
         "9999999999999999999999999999999999999999999999999999999999"
         "69999999006]"),
    e_EITHER }
, { L_, "i_number_neg_int_huge_exp.json",
    JSON("[-1e+9999]"),
    e_EITHER }
, { L_, "i_number_pos_double_huge_exp.json",
    JSON("[1.5e+9999]"),
    e_EITHER }
, { L_, "i_number_real_neg_overflow.json",
    JSON("[-123123e100000]"),
    e_EITHER }
, { L_, "i_number_real_pos_overflow.json",
    JSON("[123123e100000]"),
    e_EITHER }
, { L_, "i_number_real_underflow.json",
    JSON("[123e-10000000]"),
    e_EITHER }
, { L_, "i_number_too_big_neg_int.json",
    JSON("[-123123123123123123123123123123]"),
    e_EITHER }
, { L_, "i_number_too_big_pos_int.json",
    JSON("[100000000000000000000]"),
    e_EITHER }
, { L_, "i_number_very_big_negative_int.json",
    JSON("[-237462374673276894279832749832423479823246327846]"),
    e_EITHER }
, { L_, "i_object_key_lone_2nd_surrogate.json",
    JSON("{\"\\uDFAA\":0}"),
    e_EITHER }
, { L_, "i_string_1st_surrogate_but_2nd_missing.json",
    JSON("[\"\\uDADA\"]"),
    e_EITHER }
, { L_, "i_string_1st_valid_surrogate_2nd_invalid.json",
    JSON("[\"\\uD888\\u1234\"]"),
    e_EITHER }
, { L_, "i_string_UTF-16LE_with_BOM.json",
    JSON("\377\376[\0\"\0\351\0\"\0]\0"),
    e_EITHER }
, { L_, "i_string_UTF-8_invalid_sequence.json",
    JSON("[\"\346\227\245\321\210\372\"]"),
    e_EITHER }
, { L_, "i_string_UTF8_surrogate_U+D800.json",
    JSON("[\"\355\240\200\"]"),
    e_EITHER }
, { L_, "i_string_incomplete_surrogate_and_escape_valid.json",
    JSON("[\"\\uD800\\n\"]"),
    e_EITHER }
, { L_, "i_string_incomplete_surrogate_pair.json",
    JSON("[\"\\uDd1ea\"]"),
    e_EITHER }
, { L_, "i_string_incomplete_surrogates_escape_valid.json",
    JSON("[\"\\uD800\\uD800\\n\"]"),
    e_EITHER }
, { L_, "i_string_invalid_lonely_surrogate.json",
    JSON("[\"\\ud800\"]"),
    e_EITHER }
, { L_, "i_string_invalid_surrogate.json",
    JSON("[\"\\ud800abc\"]"),
    e_EITHER }
, { L_, "i_string_invalid_utf-8.json",
    JSON("[\"\377\"]"),
    e_EITHER }
, { L_, "i_string_inverted_surrogates_U+1D11E.json",
    JSON("[\"\\uDd1e\\uD834\"]"),
    e_EITHER }
,  { L_, "i_string_iso_latin_1.json",
    JSON("[\"\351\"]"),
    e_EITHER }
, { L_, "i_string_lone_second_surrogate.json",
    JSON("[\"\\uDFAA\"]"),
    e_EITHER }
, { L_, "i_string_lone_utf8_continuation_byte.json",
    JSON("[\"\201\"]"),
    e_EITHER }
, { L_, "i_string_not_in_unicode_range.json",
    JSON("[\"\364\277\277\277\"]"),
    e_EITHER }
, { L_, "i_string_overlong_sequence_2_bytes.json",
    JSON("[\"\300\257\"]"),
    e_EITHER }
, { L_, "i_string_overlong_sequence_6_bytes.json",
    JSON("[\"\374\203\277\277\277\277\"]"),
    e_EITHER }
, { L_, "i_string_overlong_sequence_6_bytes_null.json",
    JSON("[\"\374\200\200\200\200\200\"]"),
    e_EITHER }
, { L_, "i_string_truncated-utf-8.json",
    JSON("[\"\340\377\"]"),
    e_EITHER }
, { L_, "i_string_utf16BE_no_BOM.json",
    JSON("\0[\0\"\0\351\0\"\0]"),
    e_EITHER }
, { L_, "i_string_utf16LE_no_BOM.json",
    JSON("[\0\"\0\351\0\"\0]\0"),
    e_EITHER }
, { L_, "i_structure_500_nested_arrays.json",
    JSON("[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["    // 01
         "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["    // 02
         "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["    // 03
         "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["    // 04
         "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["    // 05
         "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["    // 06
         "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["    // 07
         "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["    // 08
         "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["    // 09
         "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["    // 10
       // ----^----|----^----|----^----|----^----|----^----|
       //         10        20        30        40        50
         "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"    // 01
         "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"    // 02
         "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"    // 03
         "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"    // 04
         "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"    // 05
         "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"    // 06
         "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"    // 07
         "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"    // 08
         "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"    // 09
         "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"),  // 10
    e_EITHER }
, { L_, "i_structure_UTF-8_BOM_empty_object.json",
    JSON("\357\273\277{}"),
    e_EITHER }
};

const bsl::size_t JsonTestSuiteUtil::s_numData = sizeof  s_data
                                               / sizeof *s_data;

#undef NL
#undef JSON

// ACCESSORS
const JsonTestSuiteUtil::Datum *JsonTestSuiteUtil::data(bsl::size_t index)
{
    BSLS_ASSERT(index < s_numData);

    if ( 68 == index) {
        BSLMT_ONCE_DO {
            s_data[index].d_JSON_p =    u::leftBrackets100000;
            s_data[index].d_length = u::lenLeftBrackets100000;
        }
    }

    if (163 == index) {
        BSLMT_ONCE_DO {
            s_data[index].d_JSON_p =    u::openArrayObject50000;
            s_data[index].d_length = u::lenOpenArrayObject50000;
        }
    }

    return s_data + index;
}

bsl::size_t JsonTestSuiteUtil::numData()
{
    return s_numData;
}

}  // close package namespace
}  // close enterprise namespace

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
