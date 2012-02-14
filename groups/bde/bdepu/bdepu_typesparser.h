// bdepu_typesparser.h                                                -*-C++-*-
#ifndef INCLUDED_BDEPU_TYPESPARSER
#define INCLUDED_BDEPU_TYPESPARSER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Convert types and arrays of such types to/from text representation.
//
//@CLASSES:
//  bdepu_TypesParser: namespace for types- and array-parsing functions
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component predominantly implements two types of functions:
// 'parse' and 'generate', which form the basic building blocks for top-down
// (and possibly recursive-descent) parsers.  The 'parse' functions initialize
// the resulting object with the value extracted from the input string.  The
// 'generate' functions append a text representation of the object to a
// user-specified 'char' buffer.
//
// The component provides a parse method for each of the following types:
// CHAR, SHORT, INT, INT64, FLOAT, DOUBLE, DATE, DATETZ, TIME, TIMETZ,
// DATETIME, DATETIMETZ, and several parse methods for STRING; it also provides
// parse methods for arrays of these types.  There is a 'generate' method for
// each type for which a 'parse' method exists.
//
// The parsing functions in this component *do* consume leading whitespace.
// For each type mentioned above, and its corresponding array, we have the
// 'Raw' version of the generating function: This allows producing whitespace
// (indentation) before producing the text representing a given value, and also
// between elements for arrays.  The other version supports indentation *and*
// removal of a possible (single) terminating null from the buffer prior to
// writing the value, and addition of a null to the buffer subsequent to
// writing that value.
//
// Although parse methods must parse *any* legal textual representation of an
// object of their type, the generators each produce the canonical
// representation by default.  Legal representations are white-space-neutral,
// however, and clients can direct generators to include more or fewer levels
// of indentation.
//
// Each of the "generate" functions in this class emits text that the
// corresponding parsing function can recognize.  The "generate" functions
// operate on a 'bsl::vector<char>' buffer, which is used to maintain an
// efficient, null-terminated-string representation of the output.  The result
// of generating into a 'bsl::vector<char>' buffer will always be
// null-terminated (whether or not the buffer came in that way).
//
// The following two subsections describe the grammar defining the parsing
// rules.
//
///DEFINITION OF SYMBOLS USED IN PRODUCTION RULES
///----------------------------------------------
//
// The following grammar is used to specify regular expressions:
//..
//   .     A period (.) is a one-character RE (Regular Expression) that matches
//         any character except new-line.
//
//   []    A non-empty string of characters enclosed in square brackets ([]) is
//         a one-character RE that matches any single character in that string.
//         If, however, the first character of the string is a circumflex (^),
//         the one-character RE matches any character *except* new-line and the
//         remaining characters in the string.  The ^ has this special meaning
//         only if it occurs first in the string.  The minus (-) may be used
//         to indicate a range of consecutive characters; for example, [0-9]
//         is equivalent to [0123456789].  The - loses this special meaning if
//         it occurs first (after an initial ^, if any) or last in the string.
//         The right square bracket (]) does not terminate such a string when
//         it is the first character within it (after an initial ^, if any);
//         for example, []a-f] matches either a right square bracket (]) or
//         one of the ASCII letters a through f inclusive.
//
//   ^     Matches the beginning of the string.  See the paragraph above for
//         special interpretation of the '^' when it is inside a "[]".
//
//   $     Matches the end of the string.
//
//   \n    Matches a newline.
//
//   -     Within brackets the minus means through.  For example, [a-z] is
//         equivalent to [abcd...xyz].  The - can appear as itself only if used
//         as the first or last character.  For example, the character class
//         expression []-] matches the characters ] and -.
//
//   *     A regular expression followed by * means zero or more times.
//
//   +     A regular expression followed by + means one or more times.  For
//         example, [0-9]+ is equivalent to [0-9][0-9]*.
//
//   ?     A regular expression followed by ? means zero times or one time.
//
//   |     Logical OR between two expressions means one must be present.
//
//   {m} {m,} {m,u} Integer values enclosed in {} indicate the number of times
//         the preceding regular expression is to be applied.  The value m is
//         the minimum number and u is the maximum.  If only m is present (that
//         is, {m}), it indicates the exact number of times the regular
//         expression is to be applied.  The value {m,} is analogous to
//         {m,infinity}.  The plus (+) and star (*) operations are
//         equivalent to {1,} and {0,} respectively.
//
//   ( ... ) Parentheses are used for grouping.  An operator, for example, *,
//         +, {}, can work on a single character or on a regular expression
//         enclosed in parentheses.  For example, (a*(cb+)*)$.
//
//    \    Backslash can escape any of the symbols defined above, as well as
//         single characters that would be escaped in a C-style string.
//..
///PRODUCTION RULES FOR PARSING FUNCTIONS
///--------------------------------------
//..
// <WHITESPACE>  ::= any character for which 'isspace' returns 'true', or
//                   any legal C- or C++-style comment.
// <NONWHITESPACE> ::= any character for which 'isspace' returns 'false'.
// <EOS>          ::= End of string, or ASCII 0.
//
//                        --- char ---
// <CHAR>        ::= <WHITESPACE>* '\'' <CHAR_VAL> '\''
// <CHAR_VAL>    ::= [^ '\\'] | <ESCAPE_SEQUENCE>
// <ESCAPE_SEQUENCE> ::= <SIMPLE_ESCAPE_SEQUENCE> | <OCTAL_ESCAPE_SEQUENCE> |
//                       <HEX_ESCAPE_SEQUENCE>
// <SIMPLE_ESCAPE_SEQUENCE> ::= ['\a' '\b' '\f' '\n' '\r' '\t' '\v' '\\' '\?'
//                              '\'' '\"']
// <OCTAL_ESCAPE_SEQUENCE> ::= '\\' <OCTAL_DIGIT>{1,3}
//                             The numeric part of the value of the sequence
//                             must be in range [0 .. UCHAR_MAX].
// <HEX_ESCAPE_SEQUENCE> ::= '\\' 'x' <HEX_DIGIT>{1,2}
// <CHAR_ARRAY> ::= <WHITESPACE>* '[' (<WHITESPACE>+ <CHAR>)* <WHITESPACE>+ ']'
//
//                        --- timezone offset ---
// <TZ>         ::= (+|-) <DECIMAL_DIGIT>{4}
//                        --- date ---
// <DATE>       ::= <WHITESPACE>* <YEAR> '/' <MONTH> '/' <DAY>
//                  In addition to the correct ranges of <YEAR>, <MONTH>, and
//                  <DAY>, the whole <DATE> must be valid as specified in
//                  'bdet_Date'.
// <YEAR>       ::= <DIGIT>{1,4} in range [1 .. 9999]
// <MONTH>      ::= <DIGIT>{1,2} in range [1 .. 12]
// <DAY>        ::= <DIGIT>{1,2} in range [1 .. 31]
// <DATE_TZ>    ::= <DATE><TZ>
// <DATE_ARRAY> ::= <WHITESPACE>* '[' (<WHITESPACE>+ <DATE>)* <WHITESPACE>+
//                   ']'
// <DATE_TZ_ARRAY>  ::= <WHITESPACE>* '[' (<WHITESPACE>+ <DATE_TZ>)*
//                     <WHITESPACE>+ ']'
// <DATETIME>   ::= <WHITESPACE>* <DATE> (' ' | '_' ) <TIME>
// <DATETIME_TZ> ::= <DATETIME><TZ>
// <DATETIME_ARRAY> ::= <WHITESPACE>* '[' (<WHITESPACE>+ <DATETIME>)*
//                      <WHITESPACE>+ ']'
// <DATETIME_TZ_ARRAY> ::= <WHITESPACE>* '[' (<WHITESPACE>+ <DATETIME_TZ>)*
//                        <WHITESPACE>+ ']'
//
//                        --- numbers ---
// <DIGIT>       ::= depending on base can include characters 0-9 and case-
//               insensitive letters.  For example, octal digit is in the
//               range [0 .. 7].
// <DECIMAL_NUMBER> ::= <OPTIONAL_SIGN> <DECIMAL_DIGIT>+
// <DECIMAL_DIGIT> ::= [0123456789]
// <OCTAL_DIGIT> ::= [01234567]
// <HEX_DIGIT>   ::= [0123456789abcdefABCDEF]
// <DOUBLE>      ::= <REAL>
//                   <DOUBLE> must be in range [DBL_MIN .. DBL_MAX].
// <DOUBLE_ARRAY>::= <WHITESPACE>* '[' (<WHITESPACE>+ <DOUBLE>)* <WHITESPACE>+
//                    ']'
// <FLOAT>       ::= <REAL>
//                  <FLOAT> must be in range [FLT_MIN .. FLT_MAX].
// <FLOAT_ARRAY> ::= <WHITESPACE>* '[' (<WHITESPACE>+ <FLOAT>)* <WHITESPACE>+
//                   ']'
// <INT>         ::= <NUMBER>
//                  <INT> must be in range [INT_MIN .. INT_MAX].
// <INT_ARRAY>   ::= <WHITESPACE>* '[' (<WHITESPACE>+ <INT>)* <WHITESPACE>+ ']'
// <INT64>       ::= <NUMBER>
//                  <INT64> must be in range [LONG_MIN .. LONG_MAX].
// <INT64_ARRAY>::= <WHITESPACE>* '[' (<WHITESPACE>+ <INT64>)* <WHITESPACE>+
//                  ']'
// <INTEGER_LOCATOR> ::= <WHITESPACE>* '{' <WHITESPACE>* <DECIMAL_DIGIT>+
//                       <WHITESPACE>* '}'
// <NUMBER>     ::= <WHITESPACE>* <OPTIONAL_SIGN> <DIGIT>+
// <OPTIONAL_SIGN> ::= (+ | -)?
// <REAL>       ::= <OPTIONAL_SIGN> <DECIMAL_DIGIT>* '.'? <DECIMAL_DIGIT>*
//                  (e|E <DECIMAL_NUMBER>+)?
//                  The combined number of base-10 digits before the exponent
//                  part must be greater than 0.
// <SHORT>      ::= <NUMBER>
//                  <SHORT> must be in range [SHRT_MIN .. SHRT_MAX].
// <SHORT_ARRAY>::= <WHITESPACE>* '[' (<WHITESPACE>+ <SHORT>)* <WHITESPACE>+
//                  ']'
//
//                        --- strings ---
// <STRING>     ::= <WHITESPACE>* (<QUOTED STRING> | <UNQUOTED STRING>)
// <STRING_ARRAY> ::= <WHITESPACE>* '[' (<WHITESPACE>+ <STRING>)* <WHITESPACE>+
//                    ']'
// <UNQUOTED STRING> ::= [^ '\"']* (<WHITESPACE> | <EOS>)
//                       I.e., zero or more characters followed by a mandatory
//                       whitespace or end-of-line character on the right.
//                       None of the characters may be double-quote ('\"');
//                       the terminating whitespace or EOS is not consumed.
// <DELIMITED STRING> ::= <WHITESPACE>* <NONWHITESPACE> <CHAR_VAL>*
//                        <NONWHITESPACE>.  Note that the left and right
//                        delimiters need not be the same character, and that
//                        delimited strings may contain whitespace internally.
// <QUOTED STRING> ::= '\"' <CHAR_VAL>* '\"'
//                     I.e., zero or more characters surrounded by double
//                     quotes.  (A quoted string is clearly a special case of
//                     a delimited string).
//
//                        --- time ---
// <TIME>       ::= <WHITESPACE>* <HOUR> ':' <MINUTE>
//                  (':' <SECOND> ('.' <SDECIMAL>)? )?
//                  I.e., one to two digits representing the hour followed
//                  by a colon and exactly two digits representing the minute,
//                  optionally followed by a colon and exactly two digits
//                  representing the second, optionally followed by a period
//                  and one to three digits representing the decimal portion
//                  of a second.  In addition to the correct ranges of
//                  <HOUR>, <MINUTE>, <SECOND>, and <SDECIMAL>, the whole
//                  <TIME> must be valid as specified in 'bdet_Time'.
//                  Specifically, the only valid time where <HOUR> equals 24
//                  is when all other fields equal zero.  (Note that time
//                  24:00:00 is the internal BDE representation for an unset
//                  'bdet_Time', but is also computationally equivalent to
//                  00:00:00 and so can be "operated on" as though it were
//                  a valid 'bdet_Time'.)
// <HOUR>       ::= <DIGIT>{1,2}  in range [0 .. 24]
// <MINUTE>     ::= <DIGIT> <DIGIT>  in range [0 .. 59]
// <SECOND>     ::= <DIGIT> <DIGIT>  in range [0 .. 59]
// <SDECIMAL>   ::= <DIGIT>{1,3}  This is the decimal portion of seconds.
// <TIME_TZ>     ::= <TIME><TZ>
// <TIME_ARRAY> ::= <WHITESPACE>* '[' (<WHITESPACE>+ <TIME>)* <WHITESPACE>+ ']'
// <TIME_TZ_ARRAY> ::= <WHITESPACE>* '[' (<WHITESPACE>+ <TIME>)* <WHITESPACE>+
//                     ']'
//..
///Usage
///-----
// Suppose we have a 'Point' structure containing two 'double' values:
//..
//  struct Point {
//      double x,
//      double y;
//  };
//..
// Consider the following textual representation of an object of this type,
// representing the coordinate values x = 1.2 and y = 3.4.
//..
//  ( 1.2, 3.4 )
//..
// We can use the functions in the 'bdepu_typesparser' component to write a
// function 'parsePoint' that loads a 'Point' structure from a string
// containing a textual representation of a point as follows:
//..
//  #include <bdepu_typesparser.h>
//  #include <iostream>
//
//  enum { SUCCESS = 0, FAILURE = 1 };
//  #define FAIL_UNLESS(c) if (!(c)) { *endPos = s; return FAILURE; }
//
//  int parsePoint(const char **endPos,
//                 Point       *point,
//                 const char  *s)
//      // Parse the specified string 's' for a sequence of characters matching
//      // the pattern '(' <DOUBLE> ',' <DOUBLE> ')', ignoring white space, and
//      // place into the specified 'point' the corresponding value.  Store in
//      // the specified '*endPos' the address of the non-modifiable character
//      // immediately following the successfully parsed text, or the position
//      // at which the parse failure was detected.  Return zero on success,
//      // and a non-zero value otherwise.  The behavior is undefined if any
//      // argument is 0.
//  {
//      bdepu_parserimputil::skipWhitespace(&s, s);
//      FAIL_UNLESS(0 == bdepu_parserimputil::skipRequiredToken(&s, s, '('));
//      FAIL_UNLESS(0 == bdepu_typesparser::parseDouble(&s, &point.x, s));
//      bdepu_parserimputil::skipWhitespace(&s, s);
//      FAIL_UNLESS(0 == bdepu_parserimputil::skipRequiredToken(&s, s, ','));
//      FAIL_UNLESS(0 == bdepu_typesparser::parseDouble(&s, &point.y, s));
//      bdepu_parserimputil::skipWhitespace(&s, s);
//      FAIL_UNLESS(0 == bdepu_parserimputil::skipRequiredToken(&s, s, ')'));
//      *endPos = s;  // update end-character position
//      return SUCCESS;
//  }
//
//
//  int main(int argc, char *argv[])
//  {
//      char [] pointText = "(35,   8.777 )";
//      const char *endPos;
//      Point point;
//
//      if (0 == parsePoint(&endPos, &point, pointText)) {
//          bsl::cout << "x = " << point.x << ", y = " << point.y << bsl::endl;
//      }
//      else {
//          bsl::cerr << "Error at character " << endPos - pointText + 1
//                    << bsl::endl;
//      }
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEPU_ARRAYPARSERIMPUTIL
#include <bdepu_arrayparserimputil.h>
#endif

#ifndef INCLUDED_BDEPU_PARSERIMPUTIL
#include <bdepu_parserimputil.h>
#endif

#ifndef INCLUDED_BDEPU_TYPESPARSERIMPUTIL
#include <bdepu_typesparserimputil.h>
#endif

#ifndef INCLUDED_BDET_DATE
#include <bdet_date.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BDET_DATETIMETZ
#include <bdet_datetimetz.h>
#endif

#ifndef INCLUDED_BDET_DATETZ
#include <bdet_datetz.h>
#endif

#ifndef INCLUDED_BDET_TIME
#include <bdet_time.h>
#endif

#ifndef INCLUDED_BDET_TIMETZ
#include <bdet_timetz.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_CCTYPE
#include <bsl_cctype.h>        // 'isspace'
#endif

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>       // 'bsl::size_t'
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>       // 'strlen'
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

                        // ========================
                        // struct bdepu_TypesParser
                        // ========================

struct bdepu_TypesParser {
    // This struct provides a namespace for a suite of stateless procedures
    // that perform enhanced parsing functionality of several vocabulary types
    // and arrays of these types, skipping past any leading whitespace before
    // looking for the type in a specified null-terminated character string.
    // Also provided are stateless 'generate' procedures, each of which emit
    // text that the corresponding parsing function can recognize.  The
    // 'generate' functions operate on a 'bsl::vector<char>', which is used to
    // maintain an efficient representation of the output.

  public:
    // CLASS METHODS

                        // *** PARSING FUNCTIONS ***

    static int parseChar(const char **endPos,
                         char        *result,
                         const char  *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule <CHAR>
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  Return zero on success, and a non-zero value otherwise.
        // The value of '*result' is unchanged if a parse failure occurs.  The
        // behavior is undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The first non-whitespace character in 'inputString' is not a
        //      single quote ('\'').
        //   2. The second and third characters in 'inputString' are '\\'
        //      and 'x', but there is no <HEX_DIGIT> after the 'x'.
        //   3. The first non-whitespace character in 'inputString' is a
        //      backslash ('\\'), but with the remaining text does not comprise
        //      a valid <ESCAPE_SEQUENCE>.
        //   4. The character is '\0'.
        //   5. There is no terminating single-quote ('\'') in 'inputString'.
        //..

    static int parseCharArray(const char        **endPos,
                              bsl::vector<char>  *result,
                              const char         *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule
        // <CHAR_ARRAY> and place into the specified 'result' the corresponding
        // value.  Store in the specified '*endPos' the address of the
        // non-modifiable character in 'inputString' immediately following the
        // successfully parsed text, or the position at which the parse
        // failure was detected.  Return zero on success, and a non-zero value
        // otherwise.  The value of '*result' is undefined if a parse failure
        // occurs.  The behavior is undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The opening square bracket ('[') or closing square bracket
        //      (']') is missing.
        //   2. The elements of the array are not whitespace-separated from
        //      each other, or from the bounding brackets.
        //   3. One or more array elements fails to successfully parse as
        //      a <CHAR>.
        //..

    static int parseDate(const char **endPos,
                         bdet_Date   *result,
                         const char  *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule <DATE> and
        // place into the specified 'result' the corresponding value.  Store
        // in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  (If the parsing fails because the combination of <YEAR>,
        // <MONTH>, and <DAY> is invalid as specified in 'bdet_Date', the
        // '*endPos' will point at the second digit of <DAY>.)  Return zero on
        // success, and a non-zero value otherwise.  The value of '*result' is
        // unchanged if a parse failure occurs.  The behavior is undefined if
        // any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The <YEAR> is not in range [1 .. 9999].
        //   2. The <MONTH> is not in the range [1 .. 12].
        //   3. The <DAY> is not in the range [1 .. 31].
        //   4. The number of digits in <YEAR> is not in the range [1 .. 4].
        //   5. The number of digits in <MONTH> is not 1 or 2.
        //   6. The number of digits in <DAY> is not 1 or 2.
        //   7. The fields are not separated by a forward slash ('/')
        //      character.
        //   8. The <YEAR>, <MONTH>, <DAY> combination is invalid as
        //      specified in 'bdet_Date'.
        //..

    static int parseDateArray(const char             **endPos,
                              bsl::vector<bdet_Date>  *result,
                              const char              *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule
        // <DATE_ARRAY> and place into the specified 'result' the corresponding
        // value.  Store in the specified '*endPos' the address of the
        // non-modifiable character in 'inputString' immediately following the
        // successfully parsed text, or the position at which the parse
        // failure was detected.  Return zero on success, and a non-zero value
        // otherwise.  The value of '*result' is undefined if a parse failure
        // occurs.  The behavior is undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The opening square bracket ('[') or closing square bracket
        //      (']') is missing.
        //   2. The elements of the array are not whitespace-separated from
        //      each other, or from the bounding brackets.
        //   3. One or more array elements fails to successfully parse as
        //      a <DATE>.
        //..

    static int parseDatetime(const char    **endPos,
                             bdet_Datetime  *result,
                             const char     *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule <DATETIME>
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  Return zero on success, and a non-zero value otherwise.
        // The value of '*result' is unchanged if a parse failure occurs.  The
        // behavior is undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1.  There is no initial prefix in 'inputString' that can
        //       successfully parse as a <DATE>.
        //   2.  The <DATE> and <TIME> portions of 'inputString' are not
        //       separated by either exactly one space (' ') or exactly one
        //       dash ('_').
        //   3.  The characters following the space (' ') or dash ('_')
        //       do not successfully parse as a <TIME>.
        //..

    static int parseDatetimeArray(const char                 **endPos,
                                  bsl::vector<bdet_Datetime>  *result,
                                  const char                  *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule
        // <DATETIME_ARRAY> and place into the specified 'result' the
        // corresponding value.  Store in the specified '*endPos' the address
        // of the non-modifiable character in 'inputString' immediately
        // following the successfully parsed text, or the position at which
        // the parse failure was detected.  Return zero on success, and a
        // non-zero value otherwise.  The value of '*result' is undefined if
        // a parse failure occurs.  The behavior is undefined if any argument
        // is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The opening square bracket ('[') or closing square bracket
        //      (']') is missing.
        //   2. The elements of the array are not whitespace-separated from
        //      each other, or from the bounding brackets.
        //   3. One or more array elements fails to successfully parse as
        //      a <DATETIME>.
        //..

    static int parseDatetimeTz(const char      **endPos,
                               bdet_DatetimeTz  *result,
                               const char       *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule
        // <DATETIME_TZ> and place into the specified 'result' the
        // corresponding value.  Store in the specified '*endPos' the address
        // of the non-modifiable character in 'inputString' immediately
        // following the successfully parsed text, or the position at which the
        // parse failure was detected.  Return zero on success, and a non-zero
        // value otherwise.  The value of '*result' is unchanged if a parse
        // failure occurs.  The behavior is undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1.  There is no initial prefix in 'inputString' that can
        //       successfully parse as a <DATETIME>.
        //   2.  What follows the <DATETIME> prefix of 'inputString' is neither
        //       the plus ('+') or the minus ('-') characters.
        //   3.  The characters following the plus ('+') or minus ('-') do not
        // successfully parse as a decimal integer.
        //..

    static int parseDatetimeTzArray(
                                   const char                   **endPos,
                                   bsl::vector<bdet_DatetimeTz>  *result,
                                   const char                    *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule
        // <DATETIME_TZ_ARRAY> and place into the specified 'result' the
        // corresponding value.  Store in the specified '*endPos' the address
        // of the non-modifiable character in 'inputString' immediately
        // following the successfully parsed text, or the position at which
        // the parse failure was detected.  Return zero on success, and a
        // non-zero value otherwise.  The value of '*result' is undefined if
        // a parse failure occurs.  The behavior is undefined if any argument
        // is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The opening square bracket ('[') or closing square bracket
        //      (']') is missing.
        //   2. The elements of the array are not whitespace-separated from
        //      each other, or from the bounding brackets.
        //   3. One or more array elements fails to successfully parse as
        //      a <DATETIME_TZ>.
        //..

    static int parseDateTz(const char  **endPos,
                           bdet_DateTz  *result,
                           const char   *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule <DATE_TZ>
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  Return zero on success, and a non-zero value otherwise.
        // The value of '*result' is unchanged if a parse failure occurs.  The
        // behavior is undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1.  There is no initial prefix in 'inputString' that can
        //       successfully parse as a <DATE>.
        //   2.  What follows the <DATE> prefix of 'inputString' is neither the
        // plus ('+') or the minus ('-') characters.
        //   3.  The characters following the plus ('+') or minus ('-') do not
        // successfully parse as a decimal integer.
        //..

    static int parseDateTzArray(const char               **endPos,
                                bsl::vector<bdet_DateTz>  *result,
                                const char                *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule
        // <DATE_TZ_ARRAY> and place into the specified 'result' the
        // corresponding value.  Store in the specified '*endPos' the address
        // of the non-modifiable character in 'inputString' immediately
        // following the successfully parsed text, or the position at which the
        // parse failure was detected.  Return zero on success, and a non-zero
        // value otherwise.  The value of '*result' is undefined if a parse
        // failure occurs.  The behavior is undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The opening square bracket ('[') or closing square bracket
        //      (']') is missing.
        //   2. The elements of the array are not whitespace-separated from
        //      each other, or from the bounding brackets.
        //   3. One or more array elements fails to successfully parse as
        //      a <DATE_TZ>.
        //..

    static int parseDouble(const char **endPos,
                           double      *result,
                           const char  *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule <DOUBLE>
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  Return zero on success, and a non-zero value otherwise.
        // The value of '*result' is unchanged if a parse failure occurs.  The
        // behavior is undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The first character in 'inputString' is neither '+', '-',
        //      nor a decimal number.
        //   2. There is an exponent indicator (case-insensitive 'e') in
        //      'inputString' but the character following it is not a decimal
        //      number.
        //..
        // Warning: this function might produce different values for the
        // specified 'result' from the same 'inputString'.  The value of
        // 'result' will be different on platforms, where the value of FLT_DIG
        // is smaller than the number of digits in the mantissa of the number
        // in the 'inputString'.

    static int parseDoubleArray(const char          **endPos,
                                bsl::vector<double>  *result,
                                const char           *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule
        // <DOUBLE_ARRAY> and place into the specified 'result' the
        // corresponding value.  Store in the specified '*endPos' the address
        // of the non-modifiable character in 'inputString' immediately
        // following the successfully parsed text, or the position at which
        // the parse failure was detected.  Return zero on success, and a
        // non-zero value otherwise.  The value of '*result' is undefined if a
        // parse failure occurs.  The behavior is undefined if any argument is
        // 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The opening square bracket ('[') or closing square bracket
        //      (']') is missing.
        //   2. The elements of the array are not whitespace-separated from
        //      each other, or from the bounding brackets.
        //   3. One or more array elements fails to successfully parse as
        //      a <DOUBLE>.
        //..

    static int parseFloat(const char **endPos,
                          float       *result,
                          const char  *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule <FLOAT>
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character (in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  Return zero on success, and non-zero otherwise.  The
        // behavior is undefined if any argument is 0, or if '*endPos' is 0.
        // The value of '*result' is unchanged if a parse failure occurs.
        //..
        // A parse failure can occur for the following reasons:
        //   1. syntactical errors in the input string (refer to a definition
        //      of <FLOAT>).
        //   2. number of digits before the exponent part is less than one
        //      parsed value is not in the range [FLT_MIN .. FLT_MAX] for the
        //      target platform.
        //..
        // Warning: this function might produce different values for the
        // specified 'result' from the same 'inputString'.  The value of
        // 'result' will be different on platforms, where the value of FLT_DIG
        // is smaller than the number of digits in the mantissa of the number
        // in the 'inputString'.

    static int parseFloatArray(const char         **endPos,
                               bsl::vector<float>  *result,
                               const char          *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of character matching the production rule
        // <FLOAT_ARRAY> and place into the specified 'result' the
        // corresponding value.  Store in the specified '*endPos' the address
        // of the non-modifiable character in 'inputString' immediately
        // following the successfully parsed text, or the position at which
        // the parse failure was detected.  Return zero on success, and a
        // non-zero value otherwise.  The value of '*result' is undefined if a
        // parse failure occurs.  The behavior is undefined if any argument is
        // 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The opening square bracket ('[') or closing square bracket
        //      (']') is missing.
        //   2. The elements of the array are not whitespace-separated from
        //      each other, or from the bounding brackets.
        //   3. One or more array elements fails to successfully parse as
        //      a <FLOAT>.
        //..

    static int parseInt(const char **endPos,
                        int         *result,
                        const char  *inputString,
                        int          base = 10);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for the maximal sequence of characters forming an <INT> in the
        // optionally specified 'base', and place into the specified 'result'
        // the corresponding value.  Store in the specified '*endPos' the
        // address of the non-modifiable character in 'inputString' immediately
        // following the successfully parsed text, or the position at which
        // the parse failure was detected.  Return zero on success, and a
        // non-zero value otherwise.  The value of '*result' is unchanged if a
        // parse failure occurs.  The behavior is undefined if any argument is
        // 0 and unless 2 <= 'base' and 'base' <= 36 (i.e., bases where digits
        // are representable by characters in the range ['0'-'9'], ['a'-'z'],
        // or ['A'-'Z']).
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The first character of 'inputString' is not a valid digit
        //      in 'base'.
        //..

    static int parseIntArray(const char       **endPos,
                             bsl::vector<int>  *result,
                             const char        *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule
        // <INT_ARRAY> and place into the specified 'result' the corresponding
        // value.  Store in the specified '*endPos' the address of the
        // non-modifiable character in 'inputString' immediately following the
        // successfully parsed text, or the position at which the parse
        // failure was detected.  Return zero on success, and a non-zero value
        // otherwise.  The value of '*result' is undefined if a parse failure
        // occurs.  The behavior is undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The opening square bracket ('[') or closing square bracket
        //      (']') is missing.
        //   2. The elements of the array are not whitespace-separated from
        //      each other, or from the bounding brackets.
        //   3. One or more array elements fails to successfully parse as
        //      a <INT>.
        //..

    static int parseInt64(const char               **endPos,
                          bsls_PlatformUtil::Int64  *result,
                          const char                *inputString,
                          int                        base = 10);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for the maximal sequence of characters forming a valid <INT64>
        // in the optionally specified 'base', and place into the specified
        // 'result' the corresponding value.  Store in the specified '*endPos'
        // the address of the non-modifiable character in 'inputString'
        // immediately following the successfully parsed text, or the position
        // at which the parse failure was detected.  Return zero on success,
        // and a non-zero value otherwise.  The value of '*result' is
        // unchanged if a parse failure occurs.  The behavior is undefined if
        // any argument is 0 and unless 2 <= 'base' and 'base' <= 36 (i.e.,
        // bases where digits are representable by characters in the range
        // ['0'-'9'], ['a'-'z'], or ['A'-'Z']).
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The first character of 'inputString' is not a valid digit
        //      in 'base'.
        //..

    static int parseInt64Array(
                          const char                            **endPos,
                          bsl::vector<bsls_PlatformUtil::Int64>  *result,
                          const char                             *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule
        // <INT64_ARRAY> and place into the specified 'result' the
        // corresponding value.  Store in the specified '*endPos' the address
        // of the non-modifiable character in 'inputString' immediately
        // following the successfully parsed text, or the position at which
        // the parse failure was detected.  Return zero on success, and a
        // non-zero value otherwise.  The value of '*result' is undefined if a
        // parse failure occurs.  The behavior is undefined if any argument is
        // 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The opening square bracket ('[') or closing square bracket
        //      (']') is missing.
        //   2. The elements of the array are not whitespace-separated from
        //      each other, or from the bounding brackets.
        //   3. One or more array elements fails to successfully parse as
        //      a <INT64>.
        //..

    static int parseIntegerLocator(const char **endPos,
                                   int         *result,
                                   const char  *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule
        // <INTEGER_LOCATOR> and place into the specified 'result' the value
        // of the number between the braces.  Store in the specified '*endPos'
        // the address of the non-modifiable character in 'inputString'
        // immediately following the successfully parsed text, or the position
        // at which the parse failure was detected.  Return zero on success,
        // and a non-zero value otherwise.  The behavior is undefined if any
        // argument is zero.
        //
        // A parse failure can occur for the following reasons:
        //..
        //    1. The opening curly-brace ('{') or closing curly-brace ('}')
        //       is missing..
        //    2. A non-digit, non-whitespace character other than '{' or '}'
        //       is encountered.
        //    3. The parsed integer is not in the range [0 .. INT_MAX] on the
        //       target platform.
        //..

    static int parseShort(const char **endPos,
                          short       *result,
                          const char  *inputString,
                          int          base = 10);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for the maximal sequence of characters forming a valid <SHORT>
        // in the optionally specified 'base', and place into the specified
        // 'result' the corresponding value.  Store in the specified '*endPos'
        // the address of the non-modifiable character in 'inputString'
        // immediately following the successfully parsed text, or the position
        // at which the parse failure was detected.  Return zero on success,
        // and a non-zero value otherwise.  The value of '*result' is
        // unchanged if a parse failure occurs.  The behavior is undefined if
        // any argument is 0, and unless 2 <= 'base' and 'base' <= 36 (i.e.,
        // bases where digits are representable by characters in the range
        // ['0'-'9'], ['a'-'z'] or ['A'-'Z']).
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The first character of 'inputString' is not a valid digit
        //      in 'base'.
        //..

    static int parseShortArray(const char         **endPos,
                               bsl::vector<short>  *result,
                               const char          *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule
        // <SHORT_ARRAY> and place into the specified 'result' the
        // corresponding value.  Store in the specified '*endPos' the address
        // of the non-modifiable character in 'inputString' immediately
        // following the successfully parsed text, or the position at which
        // the parse failure was detected.  Return zero on success, and a
        // non-zero value otherwise.  The value of '*result' is undefined if a
        // parse failure occurs.  The behavior is undefined if any argument is
        // 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The opening square bracket ('[') or closing square bracket
        //      (']') is missing.
        //   2. The elements of the array are not whitespace-separated from
        //      each other, or from the bounding brackets.
        //   3. One or more array elements fails to successfully parse as
        //      a <SHORT>.
        //..

    static int parseTime(const char **endPos,
                         bdet_Time   *result,
                         const char  *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule <TIME>
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  Return zero on success, and a non-zero value otherwise.
        // The value of '*result' is unchanged if a parse failure occurs.  The
        // behavior is undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1.  <HOUR> is not in range [1 .. 24].
        //   2.  <MINUTE> is not in the range [0 .. 59].
        //   3.  <SECOND>, if present, is not in the range [0 .. 59].
        //   4.  <SDECIMAL>, if present, is not in the range [0 .. 999].
        //   5.  The number of digits in <HOUR> is not 1 or 2.
        //   6.  The number of digits in <MINUTE> is not exactly two.
        //   7.  The number of digits in <SECOND>, if present, is not exactly
        //       two.
        //   8.  <HOUR> and <MINUTE> fields are not separated by ':'
        //       character.
        //   9.  There is a colon (':') immediately following the <MINUTE>
        //       field but no <SECOND> after the colon.
        //   10. There is a period ('.') following the <SECOND> field but no
        //       <SDECIMAL> after the period.
        //   11. The <HOUR> equals 24, but one or more of the other
        //       fields is not equal to zero.
        //..

    static int parseTimeArray(const char             **endPos,
                              bsl::vector<bdet_Time>  *result,
                              const char              *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule
        // <TIME_ARRAY> and place into the specified 'result' the corresponding
        // value.  Store in the specified '*endPos' the address of the
        // non-modifiable character in 'inputString' immediately following the
        // successfully parsed text, or the position at which the parse
        // failure was detected.  Return zero on success, and a non-zero value
        // otherwise.  The value of '*result' is undefined if a parse failure
        // occurs.  The behavior is undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The opening square bracket ('[') or closing square bracket
        //      (']') is missing.
        //   2. The elements of the array are not whitespace-separated from
        //      each other, or from the bounding brackets.
        //   3. One or more array elements fails to successfully parse as
        //      a <TIME>.
        //..

    static int parseTimeTz(const char  **endPos,
                           bdet_TimeTz  *result,
                           const char   *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule <TIME_TZ>
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  Return zero on success, and a non-zero value otherwise.
        // The value of '*result' is unchanged if a parse failure occurs.  The
        // behavior is undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1.  There is no initial prefix in 'inputString' that can
        //       successfully parse as a <TIME>.
        //   2.  What follows the <TIME> prefix of 'inputString' is neither the
        // plus ('+') or the minus ('-') characters.
        //   3.  The characters following the plus ('+') or minus ('-') do not
        // successfully parse as a decimal integer.
        //..

    static int parseTimeTzArray(const char               **endPos,
                                bsl::vector<bdet_TimeTz>  *result,
                                const char                *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule
        // <TIME_TZ_ARRAY> and place into the specified 'result' the
        // corresponding value.  Store in the specified '*endPos' the address
        // of the non-modifiable character in 'inputString' immediately
        // following the successfully parsed text, or the position at which the
        // parse failure was detected.  Return zero on success, and a non-zero
        // value otherwise.  The value of '*result' is undefined if a parse
        // failure occurs.  The behavior is undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The opening square bracket ('[') or closing square bracket
        //      (']') is missing.
        //   2. The elements of the array are not whitespace-separated from
        //      each other, or from the bounding brackets.
        //   3. One or more array elements fails to successfully parse as
        //      a <TIME_TZ>.
        //..

                        // *** STRING PARSING FUNCTIONS ***

    static int parseDelimitedString(const char  **endPos,
                                    bsl::string  *result,
                                    const char   *inputString,
                                    char          leftDelimiter,
                                    char          rightDelimiter);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters beginning with the specified
        // 'leftDelimiter' and ending with the specified 'rightDelimiter',
        // and place into the specified 'result' all characters between the
        // two delimiters.  Store in the specified '*endPos' the address of
        // the non-modifiable character in 'inputString' immediately following
        // the successfully parsed text, or the position at which the parse
        // failure was detected.  Return zero on success, and a non-zero value
        // otherwise.  The value of '*result' is unchanged if a parse failure
        // occurs.  The behavior is undefined if any pointer argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. One or both delimiters are missing from 'inputString'.
        //   2. Some character in the sequence prior to the right delimiter
        //      is invalid as defined in
        //      'bdepu_TypesParserImpUtil::parseCharRaw'.
        //..
        // Note that there is potential for the right delimiter to be
        // misinterpreted as part of the desired text.  For example, using
        // capital A as a delimiter will lead to a misparse of the string
        // "A_some_other_stuff_and_then_\x05A" as terminating in number Hex-
        // 5-A, rather than Hex-5 and delimiter A.  The parse will then fail
        // because no right delimiter is located.  Precisely speaking, the
        // implementation of 'parseDelimitedString' repeatedly calls
        // 'bdepu_TypesParserImpUtil::parseCharRaw' and compares its' result
        // with 'rightDelimiter', rather than scanning ahead for the delimiter
        // and then validating the characters between the two.  If a multibyte
        // character (i.e., an escape sequence) appears right before the
        // terminating delimiter, and the delimiter can legitimately be
        // interpreted to belong to the multibyte character, it will be
        // consumed as part of that character, leaving no recognizable
        // delimiter to terminate the string itself.

    static int parseQuotedString(const char  **endPos,
                                 bsl::string  *result,
                                 const char   *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule
        // '<QUOTED STRING>' and place into the specified 'result' the
        // corresponding value.  Store in the specified '*endPos' the address
        // of the non-modifiable character in 'inputString' immediately
        //  following the successfully parsed text, or the position at which
        // the parse failure was detected.  Return zero on success, and a
        // non-zero value otherwise.  The value of '*result' is unchanged if a
        // parse failure occurs.  The behavior is undefined if any argument is
        // 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The opening or closing double-quote (\") character is
        //      missing.
        //   2. One of the characters in the string is invalid as
        //      defined in 'bdepu_TypesParserImpUtil::parseCharRaw'.
        //..

    static int parseSpaceSeparatedString(const char  **endPos,
                                         bsl::string  *result,
                                         const char   *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule
        // '<UNQUOTED STRING>' to allow character double-quote ('\"') (i.e.,
        // zero or more characters followed by at least one whitespace) and
        // place into the specified 'result' the corresponding value.  Store
        // in the specified '*endPos' the address of the first whitespace
        // character located after one or more non-whitespace characters have
        // been accumulated, or the position at which the parse failure was
        // detected.  Return zero on success, and a non-zero value otherwise.
        // The value of '*result' is undefined if a parse failure occurs.  The
        // behavior is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. One of the characters in the string is invalid as
        //      defined in 'bdepu_TypesParserImpUtil::parseCharRaw'.
        //..
        // Note that this is identical to 'parseUnquotedString' directly
        // above, except for allowing embedded double-quotes ('\"') within
        // the string.

    static int parseString(const char  **endPos,
                           bsl::string  *result,
                           const char   *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule <STRING>
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  Return zero on success, and a non-zero value otherwise.
        // The value of '*result' is undefined if a parse failure occurs.
        // The behavior is undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. There is an opening double-quote ('\"') character (i.e.,
        //      the first non-whitespace character is a double-quote ('\"'))
        //       but the closing double-quote ('\"') is missing..
        //   2. The first non-whitespace character is NOT a double-quote
        //      ('\"'), but a double-quote ('\"') character is subsequently
        //      found anywhere in 'inputString' prior to encountering the
        //      delimiting whitespace, or EOS.
        //   3. One of the characters in the string is invalid as defined
        //      in 'bdepu_TypesParserImpUtil::parseCharRaw'.
        //..

    static int parseUnquotedString(const char  **endPos,
                                   bsl::string  *result,
                                   const char   *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule
        // '<UNQUOTED STRING>' and place into the specified 'result' the
        // corresponding value.  Store in the specified '*endPos' the address
        // of the first whitespace character or EOS located after
        // non-whitespace characters have been found, or the position at which
        // the parse failure was detected.  Return zero on success, and a
        // non-zero value otherwise.  The value of '*result' is unchanged if a
        // parse failure occurs.  The behavior is undefined if any argument is
        // 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. A double-quote ('\"') character is found inside the string.
        //   2. One of the characters in the string is invalid as defined
        //      in 'bdepu_TypesParserImpUtil::parseCharRaw'.
        //..

    static int parseStringArray(const char               **endPos,
                                bsl::vector<bsl::string>  *result,
                                const char                *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule
        // <STRING_ARRAY> and place into the specified 'result' the
        // corresponding value.  Store in the specified '*endPos' the address
        // of the non-modifiable character in 'inputString' immediately
        // following the successfully parsed text, or the position at which
        // the parse failure was detected.  Return zero on success, and a
        // non-zero value otherwise.  The value of '*result' is undefined if a
        // parse failure occurs.  The behavior is undefined if any argument is
        // 0.  Note that if the array's last element is an unquoted string,
        // whitespace will be required before the terminating right
        // square-bracket (']') in order for the parser to recognize the
        // bracket as delimiting the array, rather than part of the string.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The opening square bracket ('[') or closing square bracket
        //      (']') is missing.
        //   2. The elements of the array are not whitespace-separated from
        //      each other, or from the bounding brackets.
        //   3. One or more array elements fails to successfully parse as
        //      a <STRING>.
        //..

                        // *** GENERATING FUNCTIONS ***

    static void generateChar(bsl::vector<char> *buffer,
                             char               value,
                             int                level = 0,
                             int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, indent the
        // character by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The character is formatted as a
        // leading single-quote ('\''), followed by the character value,
        // followed by a trailing single-quote ('\'').  The character values
        // format as:
        //..
        //     1. Printable characters (as identified by the 'isprint'
        //        library function) are generated as-is.
        //     2. Simple escape sequences (see <SIMPLE_ESCAPE_SEQUENCE>)
        //        are generated as '\\', followed by the escape literal
        //        (e.g., '\a' is generated as "\\a").
        //     3. All other characters are generated as a
        //        <HEX_ESCAPE_SEQUENCE> -- i.e., "\\x" followed by two
        //        hexadecimal digits.  For example, a character with numeric
        //        value five will be generated as "\\x05".
        //..
        // The behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseChar' function.

    static void generateCharRaw(bsl::vector<char> *buffer,
                                char               value,
                                int                level = 0,
                                int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, indent the
        // character by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The character is formatted as a
        // leading single-quote ('\''), followed by the character value,
        // followed by a trailing single-quote ('\'').  The character values
        // format as:
        //..
        //     1. Printable characters (as identified by the 'isprint'
        //        library function) are generated as-is.
        //     2. Simple escape sequences (see <SIMPLE_ESCAPE_SEQUENCE>)
        //        are generated as '\\', followed by the escape literal
        //        (e.g., '\a' is generated as "\\a").
        //     3. All other characters are generated as a
        //        <HEX_ESCAPE_SEQUENCE> -- i.e., "\\x" followed by two
        //        hexadecimal digits.  For example, a character with numeric
        //        value five will be generated as "\\x05".
        //..
        // The behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseChar' function.

    static void generateCharArray(
                                 bsl::vector<char>        *buffer,
                                 const bsl::vector<char>&  value,
                                 int                       level = 0,
                                 int                       spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseCharArray' function.

     static void generateCharArrayRaw(
                                 bsl::vector<char>        *buffer,
                                 const bsl::vector<char>&  value,
                                 int                       level = 0,
                                 int                       spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseCharArray' function.

    static void generateDate(bsl::vector<char> *buffer,
                             const bdet_Date&   value,
                             int                level = 0,
                             int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, indent the
        // date by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The date is formatted as follows:
        // four digits to represent the year, followed by a slash ('/'),
        // followed by two digits to represent the month, followed by a slash
        // ('/'), followed by two digits to represent the day.  All values
        // will have leading zeros to reach the proscribed number of digits.
        // The behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseDate' function.

    static void generateDateRaw(bsl::vector<char> *buffer,
                                const bdet_Date&   value,
                                int                level = 0,
                                int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, indent the
        // date by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The date is formatted as follows:
        // four digits to represent the year, followed by a slash ('/'),
        // followed by two digits to represent the month, followed by a slash
        // ('/'), followed by two digits to represent the day.  All values
        // will have leading zeros to reach the proscribed number of digits.
        // The behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseDate' function.

    static void generateDateArray(
                            bsl::vector<char>             *buffer,
                            const bsl::vector<bdet_Date>&  value,
                            int                            level = 0,
                            int                            spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseDateArray' function.

    static void generateDateArrayRaw(
                            bsl::vector<char>             *buffer,
                            const bsl::vector<bdet_Date>&  value,
                            int                            level = 0,
                            int                            spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseDateArray' function.

    static void generateDateTz(bsl::vector<char>  *buffer,
                               const bdet_DateTz&  value,
                               int                 level = 0,
                               int                 spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, indent the
        // date by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The date is formatted as follows:
        // four digits to represent the year, followed by a slash ('/'),
        // followed by two digits to represent the month, followed by a slash
        // ('/'), followed by two digits to represent the day.  All values
        // will have leading zeros to reach the proscribed number of digits.
        // The behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseDateTz' function.

    static void generateDateTzRaw(bsl::vector<char>  *buffer,
                                  const bdet_DateTz&  value,
                                  int                 level = 0,
                                  int                 spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, indent the
        // date by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The date is formatted as follows:
        // four digits to represent the year, followed by a slash ('/'),
        // followed by two digits to represent the month, followed by a slash
        // ('/'), followed by two digits to represent the day.  All values
        // will have leading zeros to reach the proscribed number of digits.
        // The behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseDateTz' function.

    static void generateDateTzArray(
                          bsl::vector<char>               *buffer,
                          const bsl::vector<bdet_DateTz>&  value,
                          int                              level = 0,
                          int                              spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseDateTzArray' function.

    static void generateDateTzArrayRaw(
                          bsl::vector<char>               *buffer,
                          const bsl::vector<bdet_DateTz>&  value,
                          int                              level = 0,
                          int                              spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseDateTzArray' function.

    static void generateDatetime(bsl::vector<char>    *buffer,
                                 const bdet_Datetime&  value,
                                 int                   level = 0,
                                 int                   spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, indent the
        // datetime by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The datetime is formatted as
        // follows: the date portion of the 'value' formatted as per
        // 'generateDate', followed by a single space, followed by the time
        // portion of the 'value' formatted as per 'generateTime.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseDatetime' function.

    static void generateDatetimeRaw(bsl::vector<char>    *buffer,
                                    const bdet_Datetime&  value,
                                    int                   level = 0,
                                    int                   spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, indent the
        // datetime by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The datetime is formatted as
        // follows: the date portion of the 'value' formatted as per
        // 'generateDate', followed by a single space, followed by the time
        // portion of the 'value' formatted as per 'generateTime.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseDatetime' function.

    static void generateDatetimeArray(
                        bsl::vector<char>                 *buffer,
                        const bsl::vector<bdet_Datetime>&  value,
                        int                                level = 0,
                        int                                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseDatetimeArray' function.

    static void generateDatetimeArrayRaw(
                        bsl::vector<char>                 *buffer,
                        const bsl::vector<bdet_Datetime>&  value,
                        int                                level = 0,
                        int                                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseDatetimeArray' function.

    static void generateDatetimeTz(bsl::vector<char>      *buffer,
                                   const bdet_DatetimeTz&  value,
                                   int                     level = 0,
                                   int                     spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, indent the
        // datetime by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The datetime is formatted as
        // follows: the date portion of the 'value' formatted as per
        // 'generateDate', followed by a single space, followed by the time
        // portion of the 'value' formatted as per 'generateTime.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseDatetimeTz' function.

    static void generateDatetimeTzRaw(
                                  bsl::vector<char>      *buffer,
                                  const bdet_DatetimeTz&  value,
                                  int                     level = 0,
                                  int                     spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, indent the
        // datetime by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The datetime is formatted as
        // follows: the date portion of the 'value' formatted as per
        // 'generateDate', followed by a single space, followed by the time
        // portion of the 'value' formatted as per 'generateTime.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseDatetimeTz' function.

    static void generateDatetimeTzArray(
                      bsl::vector<char>                   *buffer,
                      const bsl::vector<bdet_DatetimeTz>&  value,
                      int                                  level = 0,
                      int                                  spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseDatetimeTzArray' function.

    static void generateDatetimeTzArrayRaw(
                      bsl::vector<char>                   *buffer,
                      const bsl::vector<bdet_DatetimeTz>&  value,
                      int                                  level = 0,
                      int                                  spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseDatetimeTzArray' function.

    static void generateDouble(bsl::vector<char> *buffer,
                               double             value,
                               int                level = 0,
                               int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, indent the
        // double by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The behavior is undefined if
        // 0 == 'buffer' or 'spacesPerLevel' < 0.  Note that 'value' is
        // emitted in a form parsable by the corresponding 'parseDouble'
        // function.

    static void generateDoubleRaw(bsl::vector<char> *buffer,
                                  double             value,
                                  int                level = 0,
                                  int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, indent the
        // double by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The behavior is undefined if
        // 0 == 'buffer' or 'spacesPerLevel' < 0.  Note that 'value' is
        // emitted in a form parsable by the corresponding 'parseDouble'
        // function.

    static void generateDoubleArray(
                             bsl::vector<char>          *buffer,
                             const bsl::vector<double>&  value,
                             int                         level = 0,
                             int                         spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseDoubleArray' function.

    static void generateDoubleArrayRaw(
                             bsl::vector<char>          *buffer,
                             const bsl::vector<double>&  value,
                             int                         level = 0,
                             int                         spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseDoubleArray' function.

    static void generateFloat(bsl::vector<char> *buffer,
                              float              value,
                              int                level = 0,
                              int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, indent the
        // float by ('level' * 'spacesPerLevel') spaces.  A negative 'level'
        // suppresses indentation.  The behavior is undefined if 0 == 'buffer'
        // or 'spacesPerLevel' < 0.  Note that 'value' is emitted in a form
        // parsable by the corresponding 'parseFloat' function.

    static void generateFloatRaw(bsl::vector<char> *buffer,
                                 float              value,
                                 int                level = 0,
                                 int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'.
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  The
        // formatted float will be indented by (level * spacesPerLevel)
        // spaces.  Making 'level' negative suppresses indentation.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseFloat' function.

    static void generateFloatArray(
                                bsl::vector<char>         *buffer,
                                const bsl::vector<float>&  value,
                                int                        level = 0,
                                int                        spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseFloatArray' function.

    static void generateFloatArrayRaw(
                                bsl::vector<char>         *buffer,
                                const bsl::vector<float>&  value,
                                int                        level = 0,
                                int                        spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseFloatArray' function.

    static void generateInt(bsl::vector<char> *buffer,
                            int                value,
                            int                level = 0,
                            int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, indent the
        // int by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The behavior is undefined if
        // 0 == 'buffer' or 'spacesPerLevel' < 0.  Note that 'value' is
        // emitted in a form parsable by the corresponding 'parseInt'
        // function.

    static void generateIntRaw(bsl::vector<char> *buffer,
                               int                value,
                               int                level = 0,
                               int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, indent the
        // int by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The behavior is undefined if
        // 0 == 'buffer' or 'spacesPerLevel' < 0.  Note that 'value' is
        // emitted in a form parsable by the corresponding 'parseInt'
        // function.

    static void generateIntArray(bsl::vector<char>       *buffer,
                                 const bsl::vector<int>&  value,
                                 int                      level = 0,
                                 int                      spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseIntArray' function.

    static void generateIntArrayRaw(
                                  bsl::vector<char>       *buffer,
                                  const bsl::vector<int>&  value,
                                  int                      level = 0,
                                  int                      spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseIntArray' function.

    static void generateInt64(bsl::vector<char>        *buffer,
                              bsls_PlatformUtil::Int64  value,
                              int                       level = 0,
                              int                       spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, indent the
        // 'bsls_PlatformUtil::Int64' by ('level' * 'spacesPerLevel') spaces.
        // A negative 'level' suppresses indentation.  The behavior is
        // undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.  Note that
        // 'value' is emitted in a form parsable by the corresponding
        // 'parseInt64' function.

    static void generateInt64Raw(bsl::vector<char>        *buffer,
                                 bsls_PlatformUtil::Int64  value,
                                 int                       level = 0,
                                 int                       spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, indent the
        // 'bsls_PlatformUtil::Int64' by ('level' * 'spacesPerLevel') spaces.
        // A negative 'level' suppresses indentation.  The behavior is
        // undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.  Note that
        // 'value' is emitted in a form parsable by the corresponding
        // 'parseInt64' function.

    static void generateInt64Array(
             bsl::vector<char>                            *buffer,
             const bsl::vector<bsls_PlatformUtil::Int64>&  value,
             int                                           level = 0,
             int                                           spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseInt64Array' function.

    static void generateInt64ArrayRaw(
             bsl::vector<char>                            *buffer,
             const bsl::vector<bsls_PlatformUtil::Int64>&  value,
             int                                           level = 0,
             int                                           spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseInt64Array' function.

    static void generateIntegerLocator(bsl::vector<char> *buffer,
                                       int                value,
                                       int                level = 0,
                                       int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, indent the
        // integer locator by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The integer locator is formatted
        // as follows: a leading left curly-brace ('{'), followed by the
        // integer 'value', followed by a trailing right curly-brace ('}').
        // The behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseIntegerLocator' function.

    static void generateIntegerLocatorRaw(
                                        bsl::vector<char> *buffer,
                                        int                value,
                                        int                level = 0,
                                        int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, indent the
        // integer locator by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The integer locator is formatted
        // as follows: a leading left curly-brace ('{'), followed by the
        // integer 'value', followed by a trailing right curly-brace ('}').
        // The behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseIntegerLocator' function.

    static void generateShort(bsl::vector<char> *buffer,
                              short              value,
                              int                level = 0,
                              int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, indent the
        // short by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The behavior is undefined if
        // 0 == 'buffer' or 'spacesPerLevel' < 0.  Note that 'value' is
        // emitted in a form parsable by the corresponding 'parseShort'
        // function.

    static void generateShortRaw(bsl::vector<char> *buffer,
                                 short              value,
                                 int                level = 0,
                                 int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, indent the
        // short by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The behavior is undefined if
        // 0 == 'buffer' or 'spacesPerLevel' < 0.  Note that 'value' is
        // emitted in a form parsable by the corresponding 'parseShort'
        // function.

    static void generateShortArray(
                                bsl::vector<char>         *buffer,
                                const bsl::vector<short>&  value,
                                int                        level = 0,
                                int                        spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseShortArray' function.

    static void generateShortArrayRaw(
                                bsl::vector<char>         *buffer,
                                const bsl::vector<short>&  value,
                                int                        level = 0,
                                int                        spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseShortArray' function.

    static void generateString(bsl::vector<char>  *buffer,
                               const bsl::string&  value,
                               int                 level = 0,
                               int                 spacesPerLevel = 4);
    static void generateString(bsl::vector<char> *buffer,
                               const char        *value,
                               int                level = 0,
                               int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, indent the
        // string by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The behavior is undefined if
        // 0 == 'buffer' or 'spacesPerLevel' < 0.  Note that 'value' is
        // emitted in a form parsable by the corresponding 'parseString'
        // function.

    static void generateStringRaw(bsl::vector<char>  *buffer,
                                  const bsl::string&  value,
                                  int                 level = 0,
                                  int                 spacesPerLevel = 4);
    static void generateStringRaw(bsl::vector<char> *buffer,
                                  const char        *value,
                                  int                level = 0,
                                  int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, indent the
        // string by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The behavior is undefined if
        // 0 == 'buffer' or 'spacesPerLevel' < 0.  Note that 'value' is
        // emitted in a form parsable by the corresponding 'parseString'
        // function.

    static void generateStringArray(
                          bsl::vector<char>               *buffer,
                          const bsl::vector<bsl::string>&  value,
                          int                              level = 0,
                          int                              spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseStringArray' function.

    static void generateStringArrayRaw(
                          bsl::vector<char>               *buffer,
                          const bsl::vector<bsl::string>&  value,
                          int                              level = 0,
                          int                              spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseStringArray' function.

    static void generateTime(bsl::vector<char> *buffer,
                             const bdet_Time&   value,
                             int                level = 0,
                             int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, indent the
        // time by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The time is formatted as
        // follows: two digits to represent hours, followed by a colon,
        // followed by two digits to represent minutes, followed by a colon,
        // followed by two digits to represent seconds, followed by a colon,
        // followed by three digits to represent the decimal component of a
        // second.  All values will have leading zeros to reach the proscribed
        // number of digits.  The behavior is undefined if 0 == 'buffer' or
        // 'spacesPerLevel' < 0.  Note that 'value' is emitted in a form
        // parsable by the corresponding 'parseTime' function.

    static void generateTimeRaw(bsl::vector<char> *buffer,
                                const bdet_Time&   value,
                                int                level = 0,
                                int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, indent the
        // time by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The time is formatted as
        // follows: two digits to represent hours, followed by a colon,
        // followed by two digits to represent minutes, followed by a colon,
        // followed by two digits to represent seconds, followed by a colon,
        // followed by three digits to represent the decimal component of a
        // second.  All values will have leading zeros to reach the proscribed
        // number of digits.  The behavior is undefined if 0 == 'buffer' or
        // 'spacesPerLevel' < 0.  Note that 'value' is emitted in a form
        // parsable by the corresponding 'parseTime' function.

    static void generateTimeArray(
                            bsl::vector<char>             *buffer,
                            const bsl::vector<bdet_Time>&  value,
                            int                            level = 0,
                            int                            spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseTimeArray' function.

    static void generateTimeArrayRaw(
                            bsl::vector<char>             *buffer,
                            const bsl::vector<bdet_Time>&  value,
                            int                            level = 0,
                            int                            spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseTimeArray' function.

    static void generateTimeTz(bsl::vector<char>  *buffer,
                               const bdet_TimeTz&  value,
                               int                 level = 0,
                               int                 spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, indent the
        // time by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The time is formatted as
        // follows: two digits to represent hours, followed by a colon,
        // followed by two digits to represent minutes, followed by a colon,
        // followed by two digits to represent seconds, followed by a colon,
        // followed by three digits to represent the decimal component of a
        // second.  All values will have leading zeros to reach the proscribed
        // number of digits.  The behavior is undefined if 0 == 'buffer' or
        // 'spacesPerLevel' < 0.  Note that 'value' is emitted in a form
        // parsable by the corresponding 'parseTimeTz' function.

    static void generateTimeTzRaw(bsl::vector<char>  *buffer,
                                  const bdet_TimeTz&  value,
                                  int                 level = 0,
                                  int                 spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, indent the
        // time by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The time is formatted as
        // follows: two digits to represent hours, followed by a colon,
        // followed by two digits to represent minutes, followed by a colon,
        // followed by two digits to represent seconds, followed by a colon,
        // followed by three digits to represent the decimal component of a
        // second.  All values will have leading zeros to reach the proscribed
        // number of digits.  The behavior is undefined if 0 == 'buffer' or
        // 'spacesPerLevel' < 0.  Note that 'value' is emitted in a form
        // parsable by the corresponding 'parseTimeTz' function.

    static void generateTimeTzArray(
                          bsl::vector<char>               *buffer,
                          const bsl::vector<bdet_TimeTz>&  value,
                          int                              level = 0,
                          int                              spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseTimeTzArray' function.

    static void generateTimeTzArrayRaw(
                          bsl::vector<char>               *buffer,
                          const bsl::vector<bdet_TimeTz>&  value,
                          int                              level = 0,
                          int                              spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, both the
        // opening and closing square brackets ('[' and ']') are indented
        // by ('level' * 'spacesPerLevel') spaces.  Each array element is
        // generated on a new line and is indented by
        // (('level' + 1) * 'spacesPerLevel') spaces.  When 'level' is
        // negative, the opening bracket indentation is suppressed.  The
        // behavior is undefined if 0 == 'buffer' or 'spacesPerLevel' < 0.
        // Note that 'value' is emitted in a form parsable by the
        // corresponding 'parseTimeTzArray' function.

    static void stripNull(bsl::vector<char> *buffer);
        // Remove the trailing '\0' from the specified 'buffer', if one is
        // found.  The behavior is undefined if 'buffer' is 0.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ------------------------
                        // struct bdepu_TypesParser
                        // ------------------------

// CLASS METHODS
inline
void bdepu_TypesParser::stripNull(bsl::vector<char> *buffer)
{
    BSLS_ASSERT_SAFE(buffer);

    const bsl::size_t bufferLen = buffer->size();

    if (bufferLen && '\0' == (*buffer)[bufferLen - 1]) {
        buffer->pop_back();
    }
}

                        // *** GENERATING FUNCTIONS ***

inline
void bdepu_TypesParser::generateCharRaw(bsl::vector<char> *buffer,
                                        char               value,
                                        int                level,
                                        int                spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }
    bdepu_TypesParserImpUtil::generateChar(buffer, value);
}

inline
void bdepu_TypesParser::generateChar(bsl::vector<char> *buffer,
                                     char               value,
                                     int                level,
                                     int                spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateCharRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateCharArray(
                                      bsl::vector<char>        *buffer,
                                      const bsl::vector<char>&  value,
                                      int                       level,
                                      int                       spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateCharArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateDateRaw(bsl::vector<char> *buffer,
                                        const bdet_Date&   value,
                                        int                level,
                                        int                spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }
    bdepu_TypesParserImpUtil::generateDate(buffer, value);
}

inline
void bdepu_TypesParser::generateDate(bsl::vector<char> *buffer,
                                     const bdet_Date&   value,
                                     int                level,
                                     int                spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateDateRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateDateArray(
                                 bsl::vector<char>             *buffer,
                                 const bsl::vector<bdet_Date>&  value,
                                 int                            level,
                                 int                            spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateDateArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateDateTzRaw(bsl::vector<char>  *buffer,
                                          const bdet_DateTz&  value,
                                          int                 level,
                                          int                 spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }
    bdepu_TypesParserImpUtil::generateDateTz(buffer, value);
}

inline
void bdepu_TypesParser::generateDateTz(bsl::vector<char>  *buffer,
                                       const bdet_DateTz&  value,
                                       int                 level,
                                       int                 spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateDateTzRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateDateTzArray(
                               bsl::vector<char>               *buffer,
                               const bsl::vector<bdet_DateTz>&  value,
                               int                              level,
                               int                              spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateDateTzArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateDatetimeRaw(
                                          bsl::vector<char>    *buffer,
                                          const bdet_Datetime&  value,
                                          int                   level,
                                          int                   spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }
    bdepu_TypesParserImpUtil::generateDatetime(buffer, value);
}

inline
void bdepu_TypesParser::generateDatetime(bsl::vector<char>    *buffer,
                                         const bdet_Datetime&  value,
                                         int                   level,
                                         int                   spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateDatetimeRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateDatetimeArray(
                             bsl::vector<char>                 *buffer,
                             const bsl::vector<bdet_Datetime>&  value,
                             int                                level,
                             int                                spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateDatetimeArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateDatetimeTzRaw(
                                        bsl::vector<char>      *buffer,
                                        const bdet_DatetimeTz&  value,
                                        int                     level,
                                        int                     spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }
    bdepu_TypesParserImpUtil::generateDatetimeTz(buffer, value);
}

inline
void bdepu_TypesParser::generateDatetimeTz(
                                        bsl::vector<char>      *buffer,
                                        const bdet_DatetimeTz&  value,
                                        int                     level,
                                        int                     spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateDatetimeTzRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateDatetimeTzArray(
                           bsl::vector<char>                   *buffer,
                           const bsl::vector<bdet_DatetimeTz>&  value,
                           int                                  level,
                           int                                  spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateDatetimeTzArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateDoubleRaw(bsl::vector<char> *buffer,
                                          double             value,
                                          int                level,
                                          int                spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }
    bdepu_TypesParserImpUtil::generateDouble(buffer, value);
}

inline
void bdepu_TypesParser::generateDouble(bsl::vector<char> *buffer,
                                       double             value,
                                       int                level,
                                       int                spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }
    bdepu_TypesParserImpUtil::generateDouble(buffer, value);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateDoubleArray(
                                    bsl::vector<char>          *buffer,
                                    const bsl::vector<double>&  value,
                                    int                         level,
                                    int                         spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateDoubleArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateFloatRaw(bsl::vector<char> *buffer,
                                         float              value,
                                         int                level,
                                         int                spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }
    bdepu_TypesParserImpUtil::generateFloat(buffer, value);
}

inline
void bdepu_TypesParser::generateFloat(bsl::vector<char> *buffer,
                                      float              value,
                                      int                level,
                                      int                spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateFloatRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateFloatArray(
                                     bsl::vector<char>         *buffer,
                                     const bsl::vector<float>&  value,
                                     int                        level,
                                     int                        spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateFloatArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateIntRaw(bsl::vector<char> *buffer,
                                       int                value,
                                       int                level,
                                       int                spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }
    bdepu_TypesParserImpUtil::generateInt(buffer, value);
}

inline
void bdepu_TypesParser::generateInt(bsl::vector<char> *buffer,
                                    int                value,
                                    int                level,
                                    int                spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateIntRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateIntArray(
                                       bsl::vector<char>       *buffer,
                                       const bsl::vector<int>&  value,
                                       int                      level,
                                       int                      spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateIntArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateInt64Raw(
                                      bsl::vector<char>        *buffer,
                                      bsls_PlatformUtil::Int64  value,
                                      int                       level,
                                      int                       spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }
    bdepu_TypesParserImpUtil::generateInt64(buffer, value);
}

inline
void bdepu_TypesParser::generateInt64(
                                      bsl::vector<char>        *buffer,
                                      bsls_PlatformUtil::Int64  value,
                                      int                       level,
                                      int                       spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateInt64Raw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateInt64Array(
                  bsl::vector<char>                            *buffer,
                  const bsl::vector<bsls_PlatformUtil::Int64>&  value,
                  int                                           level,
                  int                                           spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateInt64ArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateIntegerLocatorRaw(
                                             bsl::vector<char> *buffer,
                                             int                value,
                                             int                level,
                                             int                spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }

    buffer->push_back('{');
    bdepu_TypesParserImpUtil::generateInt(buffer, value);
    buffer->push_back('}');
}

inline
void bdepu_TypesParser::generateIntegerLocator(
                                             bsl::vector<char> *buffer,
                                             int                value,
                                             int                level,
                                             int                spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateIntegerLocatorRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateShortRaw(bsl::vector<char> *buffer,
                                         short              value,
                                         int                level,
                                         int                spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }
    bdepu_TypesParserImpUtil::generateShort(buffer, value);
}

inline
void bdepu_TypesParser::generateShort(bsl::vector<char> *buffer,
                                      short              value,
                                      int                level,
                                      int                spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateShortRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateShortArray(
                                     bsl::vector<char>         *buffer,
                                     const bsl::vector<short>&  value,
                                     int                        level,
                                     int                        spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateShortArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateStringRaw(bsl::vector<char> *buffer,
                                          const char        *value,
                                          int                level,
                                          int                spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(value);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }
    bdepu_TypesParserImpUtil::generateString(buffer, value);
}

inline
void bdepu_TypesParser::generateStringRaw(bsl::vector<char>  *buffer,
                                          const bsl::string&  value,
                                          int                 level,
                                          int                 spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    generateStringRaw(buffer, value.data(), level, spacesPerLevel);
}

inline
void bdepu_TypesParser::generateString(bsl::vector<char> *buffer,
                                       const char        *value,
                                       int                level,
                                       int                spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(value);

    stripNull(buffer);
    generateStringRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateString(bsl::vector<char>  *buffer,
                                       const bsl::string&  value,
                                       int                 level,
                                       int                 spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    generateString(buffer, value.data(), level, spacesPerLevel);
}

inline
void bdepu_TypesParser::generateStringArray(
                               bsl::vector<char>               *buffer,
                               const bsl::vector<bsl::string>&  value,
                               int                              level,
                               int                              spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateStringArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateTimeRaw(bsl::vector<char> *buffer,
                                        const bdet_Time&   value,
                                        int                level,
                                        int                spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }
    bdepu_TypesParserImpUtil::generateTime(buffer, value);
}

inline
void bdepu_TypesParser::generateTime(bsl::vector<char> *buffer,
                                     const bdet_Time&   value,
                                     int                level,
                                     int                spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateTimeRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateTimeArray(
                                 bsl::vector<char>             *buffer,
                                 const bsl::vector<bdet_Time>&  value,
                                 int                            level,
                                 int                            spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateTimeArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateTimeTzRaw(bsl::vector<char>  *buffer,
                                          const bdet_TimeTz&  value,
                                          int                 level,
                                          int                 spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    if (level > 0) {
        bdepu_ParserImpUtil::generateIndentation(buffer,
                                                 level,
                                                 spacesPerLevel);
    }
    bdepu_TypesParserImpUtil::generateTimeTz(buffer, value);
}

inline
void bdepu_TypesParser::generateTimeTz(bsl::vector<char>  *buffer,
                                       const bdet_TimeTz&  value,
                                       int                 level,
                                       int                 spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateTimeTzRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdepu_TypesParser::generateTimeTzArray(
                               bsl::vector<char>               *buffer,
                               const bsl::vector<bdet_TimeTz>&  value,
                               int                              level,
                               int                              spacesPerLevel)
{
    BSLS_ASSERT_SAFE(buffer);

    stripNull(buffer);
    generateTimeTzArrayRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

                        // *** PARSING FUNCTIONS ***

inline
int bdepu_TypesParser::parseChar(const char **endPos,
                                 char        *result,
                                 const char  *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_TypesParserImpUtil::parseChar(endPos, result, inputString);
}

inline
int bdepu_TypesParser::parseCharArray(const char        **endPos,
                                      bsl::vector<char>  *result,
                                      const char         *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_ArrayParserImpUtil::parseCharArray(endPos,
                                                    result,
                                                    inputString);
}

inline
int bdepu_TypesParser::parseDate(const char **endPos,
                                 bdet_Date   *result,
                                 const char  *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_TypesParserImpUtil::parseDate(endPos, result, inputString);
}

inline
int bdepu_TypesParser::parseDateArray(const char             **endPos,
                                      bsl::vector<bdet_Date>  *result,
                                      const char              *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_ArrayParserImpUtil::parseDateArray(endPos,
                                                    result,
                                                    inputString);
}

inline
int bdepu_TypesParser::parseDatetime(const char    **endPos,
                                     bdet_Datetime  *result,
                                     const char     *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_TypesParserImpUtil::parseDatetime(endPos,
                                                   result,
                                                   inputString);
}

inline
int bdepu_TypesParser::parseDatetimeArray(
                                      const char                 **endPos,
                                      bsl::vector<bdet_Datetime>  *result,
                                      const char                  *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_ArrayParserImpUtil::parseDatetimeArray(endPos,
                                                        result,
                                                        inputString);
}

inline
int bdepu_TypesParser::parseDatetimeTz(const char      **endPos,
                                       bdet_DatetimeTz  *result,
                                       const char       *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_TypesParserImpUtil::parseDatetimeTz(endPos,
                                                     result,
                                                     inputString);
}

inline
int bdepu_TypesParser::parseDatetimeTzArray(
                                    const char                   **endPos,
                                    bsl::vector<bdet_DatetimeTz>  *result,
                                    const char                    *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_ArrayParserImpUtil::parseDatetimeTzArray(endPos,
                                                          result,
                                                          inputString);
}

inline
int bdepu_TypesParser::parseDateTz(const char   **endPos,
                                   bdet_DateTz   *result,
                                   const char    *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_TypesParserImpUtil::parseDateTz(endPos, result, inputString);
}

inline
int bdepu_TypesParser::parseDateTzArray(const char               **endPos,
                                        bsl::vector<bdet_DateTz>  *result,
                                        const char                *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_ArrayParserImpUtil::parseDateTzArray(endPos,
                                                      result,
                                                      inputString);
}

inline
int bdepu_TypesParser::parseDouble(const char **endPos,
                                   double      *result,
                                   const char  *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_TypesParserImpUtil::parseDouble(endPos, result, inputString);
}

inline
int bdepu_TypesParser::parseDoubleArray(const char          **endPos,
                                        bsl::vector<double>  *result,
                                        const char           *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_ArrayParserImpUtil::parseDoubleArray(endPos,
                                                      result,
                                                      inputString);
}

inline
int bdepu_TypesParser::parseFloat(const char **endPos,
                                  float       *result,
                                  const char  *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_TypesParserImpUtil::parseFloat(endPos, result, inputString);
}

inline
int bdepu_TypesParser::parseFloatArray(const char         **endPos,
                                       bsl::vector<float>  *result,
                                       const char          *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_ArrayParserImpUtil::parseFloatArray(endPos,
                                                     result,
                                                     inputString);
}

inline
int bdepu_TypesParser::parseInt(const char **endPos,
                                int         *result,
                                const char  *inputString,
                                int          base)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);
    BSLS_ASSERT_SAFE(2 <= base);
    BSLS_ASSERT_SAFE(     base <= 36);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_TypesParserImpUtil::parseInt(endPos,
                                              result,
                                              inputString,
                                              base);
}

inline
int bdepu_TypesParser::parseIntArray(const char       **endPos,
                                     bsl::vector<int>  *result,
                                     const char        *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_ArrayParserImpUtil::parseIntArray(endPos,
                                                   result,
                                                   inputString);
}

inline
int bdepu_TypesParser::parseInt64(const char               **endPos,
                                  bsls_PlatformUtil::Int64  *result,
                                  const char                *inputString,
                                  int                        base)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);
    BSLS_ASSERT_SAFE(2 <= base);
    BSLS_ASSERT_SAFE(     base <= 36);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_TypesParserImpUtil::parseInt64(endPos,
                                                result,
                                                inputString,
                                                base);
}

inline
int bdepu_TypesParser::parseInt64Array(
                           const char                            **endPos,
                           bsl::vector<bsls_PlatformUtil::Int64>  *result,
                           const char                             *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_ArrayParserImpUtil::parseInt64Array(endPos,
                                                     result,
                                                     inputString);
}

inline
int bdepu_TypesParser::parseShort(const char **endPos,
                                  short       *result,
                                  const char  *inputString,
                                  int          base)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);
    BSLS_ASSERT_SAFE(2 <= base);
    BSLS_ASSERT_SAFE(     base <= 36);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_TypesParserImpUtil::parseShort(endPos,
                                                result,
                                                inputString,
                                                base);
}

inline
int bdepu_TypesParser::parseShortArray(const char         **endPos,
                                       bsl::vector<short>  *result,
                                       const char          *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_ArrayParserImpUtil::parseShortArray(endPos,
                                                     result,
                                                     inputString);
}

inline
int bdepu_TypesParser::parseDelimitedString(const char  **endPos,
                                            bsl::string  *result,
                                            const char   *inputString,
                                            char          leftDelimiter,
                                            char          rightDelimiter)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_TypesParserImpUtil::parseDelimitedString(endPos,
                                                          result,
                                                          inputString,
                                                          leftDelimiter,
                                                          rightDelimiter);
}

inline
int bdepu_TypesParser::parseQuotedString(const char  **endPos,
                                         bsl::string  *result,
                                         const char   *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_TypesParserImpUtil::parseQuotedString(endPos,
                                                       result,
                                                       inputString);
}

inline
int bdepu_TypesParser::parseSpaceSeparatedString(const char  **endPos,
                                                 bsl::string  *result,
                                                 const char   *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_TypesParserImpUtil::parseSpaceDelimitedString(endPos,
                                                               result,
                                                               inputString);
}

inline
int bdepu_TypesParser::parseString(const char  **endPos,
                                   bsl::string  *result,
                                   const char   *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_TypesParserImpUtil::parseString(endPos, result, inputString);
}

inline
int bdepu_TypesParser::parseUnquotedString(const char  **endPos,
                                           bsl::string  *result,
                                           const char   *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_TypesParserImpUtil::parseUnquotedString(endPos,
                                                         result,
                                                         inputString);
}

inline
int bdepu_TypesParser::parseStringArray(const char               **endPos,
                                        bsl::vector<bsl::string>  *result,
                                        const char                *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_ArrayParserImpUtil::parseStringArray(endPos,
                                                      result,
                                                      inputString);
}

inline
int bdepu_TypesParser::parseTime(const char **endPos,
                                 bdet_Time   *result,
                                 const char  *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_TypesParserImpUtil::parseTime(endPos, result, inputString);
}

inline
int bdepu_TypesParser::parseTimeArray(const char             **endPos,
                                      bsl::vector<bdet_Time>  *result,
                                      const char              *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_ArrayParserImpUtil::parseTimeArray(endPos,
                                                    result,
                                                    inputString);
}

inline
int bdepu_TypesParser::parseTimeTzArray(const char               **endPos,
                                        bsl::vector<bdet_TimeTz>  *result,
                                        const char                *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_ArrayParserImpUtil::parseTimeTzArray(endPos,
                                                      result,
                                                      inputString);
}

inline
int bdepu_TypesParser::parseTimeTz(const char   **endPos,
                                   bdet_TimeTz   *result,
                                   const char    *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    return bdepu_TypesParserImpUtil::parseTimeTz(endPos, result, inputString);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
